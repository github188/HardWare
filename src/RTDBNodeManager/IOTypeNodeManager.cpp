/*!
 *	\file	IOTypeNodeManager.cpp
 *
 *	\brief	FEP类型节点管理器
 *	
 *
 *	\author lingling.li
 *
 *	\date	2014年9月2日	16:27
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IOTypeNodeManager.h"
#include "../utilities/ConstDefination.h"
#include "RTDB/Server/utilities/fepcommondef.h"
#include "nodemanagerroot.h"
#include "methodfolder.h"
#include "../FEPPlatformClass/IOPort.h"
#include "../FEPPlatformClass/IOUnit.h"
#include "../FEPPlatformClass/FieldPointType.h"

#include "../DBPlatformClass/GeneralDataType.h"
#include "../DBPlatformClass/ComponentType.h"
#include "../DBPlatformClass/AIType.h"
#include "../DBPlatformClass/AIOType.h"
#include "../DBPlatformClass/AOType.h"
#include "../DBPlatformClass/AnalogConfigType.h"
#include "../DBPlatformClass/AreaAlarmSynthType.h"
#include "../DBPlatformClass/AreaType.h"
#include "../DBPlatformClass/CtrlConfigParamsType.h"
#include "../DBPlatformClass/CtrlProcessInfoType.h"
#include "../DBPlatformClass/DeviationAlarmConfigType.h"
#include "../DBPlatformClass/DIType.h"
#include "../DBPlatformClass/DIOType.h"
#include "../DBPlatformClass/DOType.h"
#include "../DBPlatformClass/EQAlarmSynthType.h"
#include "../DBPlatformClass/EQType.h"
#include "../DBPlatformClass/LevelAlarmConfigType.h"
#include "../DBPlatformClass/OriginatorType.h"
#include "../DBPlatformClass/PulseConfigType.h"
#include "../DBPlatformClass/RegConfigParamsType.h"
#include "../DBPlatformClass/RocAlarmConfigType.h"
#include "../DBPlatformClass/BaUserData.h"

#include "Common/DataChange/WildCardsManager.h"
#include "Common/DataChange/DataTrans.h"
#include "../DBPlatformClass/MyInstanceFactory.h"
#include "HlyEventBase/HlyAlarmType.h"
#include "HlyEventBase/HlyEventBase_dll.h"

namespace MACS_SCADA_SUD {

bool IOTypeNodeManager::s_feptypeCreated = false;
bool IOTypeNodeManager::s_rtdbtypeCreated = false;
bool IOTypeNodeManager::s_typeCreated = false;

IOTypeNodeManager::IOTypeNodeManager(const UaString& sNamespaceUri)
	:NodeManagerNodeSetXml(sNamespaceUri)
{
	m_mapObjectTypeMethods.clear();
	m_mapObjectType.clear();
	m_mapValueChangeObjectScript.clear();
	m_mapValueChangeMethodScript.clear();
}

IOTypeNodeManager::~IOTypeNodeManager(void)
{
	m_mapObjectTypeMethods.clear();
	m_mapObjectType.clear();

	map<UaNodeId, vector<ValueTriggerScript*> >::iterator it;
	vector<ValueTriggerScript*>::iterator it2;
	for(it = m_mapValueChangeObjectScript.begin(); it != m_mapValueChangeObjectScript.end(); it++)
	{
		for(it2 = (it->second).begin(); it2 != (it->second).end(); it2++)
		{
			delete *it2;
		}
		(it->second).clear();
	}
	m_mapValueChangeObjectScript.clear();

	map<OpcUa::BaseMethod*, vector<ValueTriggerScript*> >::iterator it3;
	for(it3 = m_mapValueChangeMethodScript.begin(); it3 != m_mapValueChangeMethodScript.end(); it3++)
	{
		for(it2 = (it3->second).begin(); it2 != (it3->second).end(); it2++)
		{
			delete *it2;
		}
		(it3->second).clear();
	}
	m_mapValueChangeMethodScript.clear();
}

UaStatus IOTypeNodeManager::afterStartUp()
{
	UaStatus ret;

	if(getNameSpaceIndex() == FEP_TYPE_NS_INDEX)
	{
		///创建方法文件夹
		NodeManagerRoot* pNodeManagerRoot = NodeManagerRoot::CreateRootNodeManager();
		if (pNodeManagerRoot != NULL)
		{
			CMethodFolder* pMethodFolder = new CMethodFolder(FepId_GlobalMethodObject, UaNodeId(FepId_GlobalMethodObject, 0), "en");
			ret = pNodeManagerRoot->addNodeAndReference(UaNodeId(OpcUaId_Server), pMethodFolder, OpcUaId_HasComponent);
			UA_ASSERT(ret.isGood());
		}

		if(!s_feptypeCreated)
			createFepTypes();
	}
	else if(getNameSpaceIndex() == RTDB_TYPE_NS_INDEX)
	{
		if(!s_rtdbtypeCreated)
			createRTDBTypes();
	}

	return ret;
}

UaStatus   IOTypeNodeManager::beforeShutDown()
{
	UaStatus ret;
	NodeManagerUaNode::m_pTable->clear(false);
	return ret;
}

void IOTypeNodeManager::objectTypeCreated(UaObjectType* pNewNode, UaBase::ObjectType *pObjectType)
{
	if(NULL != pNewNode && NULL != pObjectType)
		m_mapObjectType[pNewNode] = pObjectType->references();
}

void IOTypeNodeManager::methodCreated(UaMethod* pNewNode, UaBase::Method *pMethod)
{
	pNewNode->setUserData(pMethod->userData());
}

void IOTypeNodeManager::allNodesAndReferencesCreated()
{
	if(getNameSpaceIndex() == RTDB_TYPE_NS_INDEX)
	{
		if(!s_typeCreated)
		{
			RecordObjectTypeMethods();
			s_typeCreated = true;
		}
	}
}

OpcUa_Boolean IOTypeNodeManager::GetSelfMethods(UaNodeId typeDefinitionId, vector<OpcUa::BaseMethod*>& vecMethods)
{
	OpcUa_Boolean bFind = OpcUa_False;

	vecMethods.clear();
	map<UaNodeId, vector<OpcUa::BaseMethod*> >::iterator it;

	UaNodeId typeId = typeDefinitionId;
	OpcUa_Boolean bBaseClass = g_pNodeFactoryManager->CheckBelongtoDirectPlatformClass(typeId);

	//! A模板挂载周期脚本 B模板不重写周期脚本 B模板实例没有周期脚本问题
	while(!bBaseClass)
	{
		it = m_mapObjectTypeMethods.find(typeId);
		if(it != m_mapObjectTypeMethods.end())
		{
			bFind = OpcUa_True;
		
			//! 除了值变化触发方法不关心 其它均关心
			vector<OpcUa::BaseMethod*> vec = it->second;
			vector<OpcUa::BaseMethod*>::iterator it2;
			OpcUa::BaseMethod* pMethod = NULL;
			MyMethodUserData* pUserData = NULL;
			ScriptRegion* pScriptRegion = NULL;
			for(it2 = vec.begin(); it2 != vec.end(); it2++)
			{
				pMethod = *it2;
				if(NULL != pMethod)
				{
					pUserData = (MyMethodUserData*)pMethod->getUserData();
					if(NULL != pUserData)
					{
						pScriptRegion = pUserData->getUserScript();
						if(pScriptRegion)
						{
							if(pScriptRegion->scriptItem.triggerMode != TriggerMode_Value_Change)
							{
								//! 通配符目前是一次解析后续多次使用 如果这里不加载重载方法而是在objectCreated内加载静态方法 
								//! 执行方法时脚本内的通配符不解析会有问题
								vecMethods.push_back(pMethod);
							}
						}
					}
				}
			}
		}

		//! 再往模板的上层模板找
		typeId = g_pNodeFactoryManager->GetSubTypeId(typeId);
		bBaseClass = g_pNodeFactoryManager->CheckBelongtoDirectPlatformClass(typeId);
	}

	return bFind;
}

void IOTypeNodeManager::RecordObjectTypeMethods()
{
	if(!m_mapObjectType.empty())
	{
		MyMethodUserData* pUserData = NULL;
		OpcUa::BaseMethod* pMethod = NULL;
		ScriptRegion* pScriptRegion = NULL;
		map<UaNodeId, vector<OpcUa::BaseMethod*> >::iterator it;
		map<UaObjectType*, std::list<UaBase::Reference> >::iterator itType;
		map<UaNodeId, vector<ValueTriggerScript*> >::iterator itChange;
		map<OpcUa::BaseMethod*, vector<ValueTriggerScript*> >::iterator itMethod;
		for(itType = m_mapObjectType.begin(); itType != m_mapObjectType.end(); itType++)
		{
			m_mapObjectTypeMethods[itType->first->nodeId()] = vector<OpcUa::BaseMethod*>();
			it = m_mapObjectTypeMethods.find(itType->first->nodeId());

			std::list<UaBase::Reference> lstReferences = itType->second;
			std::list<UaBase::Reference>::const_iterator itRef = lstReferences.begin();
			while (itRef != lstReferences.end())
			{
				if (itRef->referenceTypeId().identifierNumeric() == OpcUaId_HasComponent)
				{
					if(itRef->isForward())
					{
						UaNode* pNode = findNode(itRef->targetNodeId());
						if(pNode && pNode->nodeClass() == OpcUa_NodeClass_Method)
						{
							pMethod = dynamic_cast<OpcUa::BaseMethod*>(pNode);
							it->second.push_back(pMethod);

							//! 处理模板方法中含值变化触发脚本的方法
							pUserData = (MyMethodUserData*)pMethod->getUserData();
							if(NULL != pUserData)
							{
								pScriptRegion = pUserData->getUserScript();
								if(pScriptRegion)
								{
									switch(pScriptRegion->scriptItem.triggerMode)
									{
									case TriggerMode_Value_Change:
										{
											vector<UaString> changeSources;
											pUserData->GetValueChangeSources(changeSources);

											/*
											itChange = m_mapValueChangeObjectScript.find(it->first);
											if(itChange != m_mapValueChangeObjectScript.end())
											{
												ValueTriggerScript* pScript = new ValueTriggerScript();
												pScript->vecSrc = changeSources;
												pScript->sScript = pScriptRegion->scriptCxt.sScript;

												(itChange->second).push_back(pScript);												
											}
											else
											{
												ValueTriggerScript* pScript = new ValueTriggerScript();
												pScript->vecSrc = changeSources;
												pScript->sScript = pScriptRegion->scriptCxt.sScript;
												vector<ValueTriggerScript*> vecTrigger;
												vecTrigger.push_back(pScript);
												m_mapValueChangeObjectScript[it->first] = vecTrigger;
											}						
											*/

											itMethod = m_mapValueChangeMethodScript.find(pMethod);
											if(itMethod != m_mapValueChangeMethodScript.end())
											{
												ValueTriggerScript* pScript = new ValueTriggerScript();
												pScript->vecSrc = changeSources;
												pScript->sScript = pScriptRegion->scriptCxt.sScript;

												(itMethod->second).push_back(pScript);
											}
											else
											{
												ValueTriggerScript* pScript = new ValueTriggerScript();
												pScript->vecSrc = changeSources;
												pScript->sScript = pScriptRegion->scriptCxt.sScript;
												vector<ValueTriggerScript*> vecTrigger;
												vecTrigger.push_back(pScript);
												m_mapValueChangeMethodScript[pMethod] = vecTrigger;
											}
										}
										break;
									}
								}
							}
						}
					}
				}
				itRef++;
			}
		}
	}
}
void IOTypeNodeManager::GetSelfObjectType(std::vector<UaNodeId> &vecObjectType)
{
	vecObjectType.clear();
	map<UaObjectType*, std::list<UaBase::Reference> >::iterator itType;
	for(itType = m_mapObjectType.begin(); itType != m_mapObjectType.end(); itType++)
	{
		vecObjectType.push_back(itType->first->typeDefinitionId());
	}
}
MethodHandleUaNode* IOTypeNodeManager::GetSelfMethodHandle(UaNode* pObject, OpcUa_NodeId* pMethodNodeId)
{
	MethodHandleUaNode* pMethodHandle = NULL;

	if(NULL == pObject || NULL == pMethodNodeId)
		return pMethodHandle;

	UaNodeId typeId;

	if(g_pNodeFactoryManager)
	{
		typeId = g_pNodeFactoryManager->GetObjectTypeId(pObject->nodeId());
	}
	
	OpcUa_Boolean bFind = OpcUa_False;
	while(!typeId.isNull() && !bFind)
	{
		map<UaNodeId, vector<OpcUa::BaseMethod*> >::const_iterator it;
		it = m_mapObjectTypeMethods.find(typeId);
		if(it != m_mapObjectTypeMethods.end())
		{
			UaString strMethod = string_getlast(pMethodNodeId);
			if(!strMethod.isEmpty())
			{
				vector<OpcUa::BaseMethod*>::const_iterator itVec;
				for(itVec = it->second.begin(); itVec != it->second.end(); itVec++)
				{
					if(IsInBrowseName((*itVec)->browseName(), strMethod.toUtf8()))
					{
						pMethodHandle = new MethodHandleUaNode;
						((MethodHandleUaNode*)pMethodHandle)->setUaNodes((UaObject*)pObject, (UaMethod*)(*itVec));
						bFind = OpcUa_True;
						break;
					}
				}
			}
		}

		if(!bFind)
		{
			typeId = g_pNodeFactoryManager->GetSubTypeId(typeId);
		}
	}

	return pMethodHandle;
}

//! 依据实例对象类型获取值变化源列表
OpcUa_Boolean IOTypeNodeManager::GetChangeSources(UaNodeId typeDefinitionId, vector<ValueTriggerScript*>& vecSources)
{
	OpcUa_Boolean ret = OpcUa_False;
	vecSources.clear();

	/*
	map<UaNodeId, vector<ValueTriggerScript*> >::iterator it;
	it = m_mapValueChangeObjectScript.find(typeDefinitionId);
	if(it != m_mapValueChangeObjectScript.end())
	{
		ret = OpcUa_True;
		vecSources = it->second;
	}
	*/

	return ret;
}

OpcUa_Boolean IOTypeNodeManager::GetChangeSourcesEx(UaNodeId typeDefinitionId, map<OpcUa::BaseMethod*, vector<ValueTriggerScript*> >& mapSources)
{
	OpcUa_Boolean ret = OpcUa_False;
	mapSources.clear();

	//! 值变化触发方法需要逐层找不能只找本层 否则上层模板挂的值变化触发方法可能会丢失
	UaNodeId typeId = typeDefinitionId;
	OpcUa_Boolean bBaseClass = g_pNodeFactoryManager->CheckBelongtoDirectPlatformClass(typeId);
	while(!bBaseClass)
	{
		map<UaNodeId, vector<OpcUa::BaseMethod*> >::iterator it;
		it = m_mapObjectTypeMethods.find(typeId);
		if(it != m_mapObjectTypeMethods.end())
		{
			map<OpcUa::BaseMethod*, vector<ValueTriggerScript*> >::iterator itScript;
			m_mapValueChangeMethodScript;

			vector<OpcUa::BaseMethod*>::iterator itMethod;
			for(itMethod = (it->second).begin(); itMethod != (it->second).end(); itMethod++)
			{
				itScript = m_mapValueChangeMethodScript.find(*itMethod);
				if(itScript != m_mapValueChangeMethodScript.end())
				{
					mapSources[*itMethod] = itScript->second;
				}
			}
		}

		//! 再往模板的上层模板找
		typeId = g_pNodeFactoryManager->GetSubTypeId(typeId);
		bBaseClass = g_pNodeFactoryManager->CheckBelongtoDirectPlatformClass(typeId);
	}

	if(!mapSources.empty())
		ret = OpcUa_True;

	return ret;
}

void IOTypeNodeManager::createFepTypes()
{
	if ( s_feptypeCreated == false )
	{
		s_feptypeCreated = true;

		//! FEP
		CIOUnit::createTypes();
		CIOPort::createTypes();
		FieldPointType::createTypes();
	}
}

void IOTypeNodeManager::createRTDBTypes()
{
	if ( s_rtdbtypeCreated == false )
	{
		s_rtdbtypeCreated = true;

		//! RTDB
		GeneralDataType::createTypes();
		ComponentType::createTypes();

		/*
		AnalogConfigType::createTypes();
		EQAlarmSynthType::createTypes();
		AreaAlarmSynthType::createTypes();
		CtrlConfigParamsType::createTypes();
		CtrlProcessInfoType::createTypes();
		DeviationAlarmConfigType::createTypes();		
		LevelAlarmConfigType::createTypes();
		OriginatorType::createTypes();
		PulseConfigType::createTypes();
		RegConfigParamsType::createTypes();
		RocAlarmConfigType::createTypes();
		DiscreteConfigType::createTypes();
		DiscreteAlarmConfigType::createTypes();
		*/

		AIType::createTypes();
		AIOType::createTypes();
		AOType::createTypes();
		DIType::createTypes();
		DIOType::createTypes();
		DOType::createTypes();
		EQType::createTypes();
		AreaType::createTypes();

		RegistEventType();
	}
}

UaVariable* IOTypeNodeManager::getInstanceDeclarationVariable(OpcUa_UInt32 numericIdentifier)
{
	// Try to find the instance declaration node with the numeric identifier 
	// and the namespace index of this node manager
	UaNode* pNode = findNode(UaNodeId(numericIdentifier, getNameSpaceIndex()));
	if ( (pNode != NULL) && (pNode->nodeClass() == OpcUa_NodeClass_Variable) )
	{
		// Return the node if valid and a variable
		return (UaVariable*)pNode;
	}
	else
	{
		return NULL;
	}
}

}// End namespace

