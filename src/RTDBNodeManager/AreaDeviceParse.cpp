#include "AreaDeviceParse.h"
#include "nodemanagerroot.h"
#include "GeneralConfig.h"
#include "AreaType.h"
#include "ScriptExecuteCaller.h"
#include "Common/DataChange/ScriptParser.h"
#include "Common/HotData/HotDataMsgQueue.h"
#include "RTDB/Server/DBPlatformClass/MyInstanceFactory.h"
#include "RTDB/Server/DBPlatformClass/PermAuthorization.h"
#include "Common/GlobalVariable/RunVariable.h"

using namespace MACS_SCADA_SUD;
IOTypeNodeManager* CAreaDeviceParse::s_pIOTypeNodeManager = NULL;

CAreaDeviceParse::CAreaDeviceParse():m_iNamespaceIndex(0)
{
	m_mapArea.clear();
	m_mapEQ.clear();
	m_mapChangeSources.clear();
}

CAreaDeviceParse::~CAreaDeviceParse()
{
	s_pIOTypeNodeManager = NULL;
	ClearResources();

	m_vecMethodWithScript.clear();

	vector<ScriptRegion*>::iterator it3;
	for(it3 = m_vecEndScripts.begin(); it3 != m_vecEndScripts.end(); it3++)
	{
		delete *it3;
		*it3 = NULL;
	}
	m_vecEndScripts.clear();
}

void CAreaDeviceParse::SetIndex(OpcUa_UInt16 nsIndex)
{
	m_iNamespaceIndex = nsIndex;
}

OpcUa_Boolean CAreaDeviceParse::IsAreaType(UaNodeId typeId)
{
	OpcUa_Boolean ret = OpcUa_False;

	if(NULL != g_pNodeFactoryManager)
	{
		ret = g_pNodeFactoryManager->CheckBelongto(typeId, UaNodeId(DEF_AreaType, RTDB_TYPE_NS_INDEX));
	}

	return ret;
}

OpcUa_Boolean CAreaDeviceParse::IsEQType(UaNodeId typeId)
{
	OpcUa_Boolean ret = OpcUa_False;

	if(NULL != g_pNodeFactoryManager)
	{
		ret = g_pNodeFactoryManager->CheckBelongto(typeId, UaNodeId(DEF_EQType, RTDB_TYPE_NS_INDEX));
	}
	
	return ret;
}

OpcUa_Boolean CAreaDeviceParse::IsPointType(UaNodeId NodeId)
{
	OpcUa_Boolean ret = OpcUa_False;

	NodeManagerRoot* pNodeManagerRoot = NodeManagerRoot::CreateRootNodeManager();
	if(pNodeManagerRoot)
	{
		UaNode* pSource = pNodeManagerRoot->getNode(NodeId);
		if(pSource)
		{
			ComponentType* pNode = dynamic_cast<ComponentType*>(pSource);
			if(pNode)
				ret = OpcUa_True;
		}
	}

	return ret;
}

OpcUa_Boolean CAreaDeviceParse::IsFolderType(UaNodeId typeId)
{
	OpcUa_Boolean ret = OpcUa_False;

	if (typeId.namespaceIndex() == 0)
	{
		OpcUa_NodeId NodeId;
		typeId.copyTo(&NodeId);

		if(NodeId.IdentifierType == OpcUa_IdentifierType_Numeric)
		{
			if(NodeId.Identifier.Numeric == OpcUaId_FolderType)
			{
				ret = OpcUa_True;
			}
		}

		OpcUa_NodeId_Clear(&NodeId);  //!�ͷ���Դ		
	}

	return ret;
}


void CAreaDeviceParse::parseNodeAreaDeviceInfo(UaObject* pNode, UaBase::Object* pObject)
{
	OpcUa_ReferenceParameter(pObject);

	OpcUa_Boolean bRet = OpcUa_False;
	UaNode* pTargetNode = NULL;
	NodeManagerRoot* pNodeManagerRoot = NodeManagerRoot::CreateRootNodeManager();

	OpcUa_Boolean bArea = IsAreaType(pObject->typeDefinitionId());
	OpcUa_Boolean bEQ = IsEQType(pObject->typeDefinitionId());

	if(bArea)
	{
		std::list<UaBase::Reference> lstReferences = pObject->references();
		std::list<UaBase::Reference>::const_iterator itRef = lstReferences.begin();
		while (itRef != lstReferences.end())
		{
			if (itRef->referenceTypeId().identifierNumeric() == OpcUaId_Organizes)
			{
				if (!itRef->isForward())
				{
					//! ��ǰArea�ڵ�����������ڵ���,��¼��ǰArea�ڵ�ĸ��ڵ�
					AreaLevelInfo* pInfo = new AreaLevelInfo();
					pInfo->bSetOver = OpcUa_False;
					pInfo->parentNode = itRef->targetNodeId();

					ADInfo* pADInfo = new ADInfo();

					//! ���ڵ���������ݿⵥԪ�ڵ� 1������
					if(pInfo->parentNode == GetDBUnitId(m_iNamespaceIndex))
					{
						pInfo->level = DEF_Area_Level_1;
						pInfo->bSetOver = OpcUa_True;

						pADInfo->Area1 = pObject->nodeId();
					}
					else
					{
						map<UaNodeId, AreaLevelInfo*>::iterator it = m_mapArea.find(pInfo->parentNode);
						if(it != m_mapArea.end())
						{
							AreaLevelInfo* pParentInfo = it->second;
							//! ���ڵ��Ѿ����ù��ȼ�
							if(NULL != pParentInfo && pParentInfo->bSetOver)
							{
								pInfo->level = pParentInfo->level + 1;
								pInfo->bSetOver = OpcUa_True;

								if(pInfo->level == DEF_Area_Level_2)
								{
									pADInfo->Area2 = pObject->nodeId();
									pADInfo->Area1 = pInfo->parentNode;
								}
								else if(pInfo->level == DEF_Area_Level_3)
								{
									pADInfo->Area3 = pObject->nodeId();
									pADInfo->Area2 = pInfo->parentNode;
									//! �������һ������
									ADInfo* pTmp = m_mapAreaWhole[pADInfo->Area2];
									if(NULL != pTmp)
										pADInfo->Area1 = pTmp->Area1;
								}
							}
						}
					}

					m_mapArea[pObject->nodeId()] = pInfo;	
					//! ��¼��map����
					m_mapAreaWhole[pObject->nodeId()] = pADInfo;

					CDBObjectBaseType* pArea = dynamic_cast<CDBObjectBaseType*>(pNode);
					if(NULL != pArea)
					{
						pArea->setArea1(pADInfo->Area1);
						pArea->setArea2(pADInfo->Area2);
						pArea->setArea3(pADInfo->Area3);
					}
				}
				else
				{
					//! ��ǰArea�ڵ��¹���һϵ�л�������,��¼��������ĸ��ڵ�Ϊ��ǰArea�ڵ�
					//! ����EQ�ڵ����AI DI��ĸ��ڵ�Ϊ��ǰArea�ڵ�
					m_mapNodeInArea[itRef->targetNodeId()] = pObject->nodeId();

					//! �¼�֪ͨ��
					MonitorEventTree(pObject->nodeId(), itRef->targetNodeId());
				}
			}

			itRef++;
		}	
	}
	else if(bEQ)
	{
		std::list<UaBase::Reference> lstReferences = pObject->references();
		std::list<UaBase::Reference>::const_iterator itRef = lstReferences.begin();
		while (itRef != lstReferences.end())
		{
			bRet = OpcUa_False;

			if (!itRef->isForward())
			{
				if (itRef->referenceTypeId().identifierNumeric() == OpcUaId_Organizes || 
					itRef->referenceTypeId().identifierNumeric() == OpcUaId_HasComponent)
				{
					if(m_mapEQ.find(pObject->nodeId()) == m_mapEQ.end())
					{
						//! ��ǰEQ�ڵ�����������ڵ���,��¼��ǰEQ�ڵ�ĸ��ڵ�
						m_mapEQ[pObject->nodeId()] = itRef->targetNodeId();
					}
					else
					{
						//! �����豸һ�������Area�ڵ��£�����ȴ��ǿ�йҽӵ����ݿⵥԪ�ڵ���
						// !���Կ��ܻὫ��EQ�豸�������ڵ�Area�ڵ�����������
						pTargetNode = pNodeManagerRoot->getNode(itRef->targetNodeId());
						if(pTargetNode)
						{
							bRet = IsFolderType(pTargetNode->typeDefinitionId());
						}

						if(!bRet)
						{
							//! ��ǰEQ�ڵ�����������ڵ���,��¼��ǰEQ�ڵ�ĸ��ڵ�
							m_mapEQ[pObject->nodeId()] = itRef->targetNodeId();
						}
					}

					CDBObjectBaseType* pObj = dynamic_cast<CDBObjectBaseType*>(pNode);
					if(NULL != pObj)
					{
						//! ��������
						UaNodeId AreaNodeId;
						OpcUa_Boolean ret = getEQNodeInArea(pObject->nodeId(), AreaNodeId);
						if(ret)
						{
							ADInfo* pADInfo = getAreaInfo(AreaNodeId);

							if(pADInfo)
							{
								pObj->setArea1(pADInfo->Area1);
								pObj->setArea2(pADInfo->Area2);
								pObj->setArea3(pADInfo->Area3);
							}
						}

						//! �����豸
						map<UaNodeId, UaNodeId>::iterator itEQ = m_mapEQ.find(itRef->targetNodeId());
						if(itEQ != m_mapEQ.end())
						{
							pObj->setDevice(itRef->targetNodeId());

							/*
							char sDebug[1024];
							snprintf(sDebug, 1024, "Alarm::Point[%s] EQ[%s]\n", 
									pObject->nodeId().toXmlString().toUtf8(), itRef->targetNodeId().toXmlString().toUtf8());
							OutputDebugString(sDebug);
							*/
						}
					}	
				}
			}
			else
			{
				if(itRef->referenceTypeId().identifierNumeric() == OpcUaId_HasComponent)
				{
					//! ��ǰEQ1�¹������豸EQ2����AI DI�㣬 EQ2��һ�������Point��
					m_mapNodeInEQ[itRef->targetNodeId()] = pObject->nodeId();

					//! �¼�֪ͨ��
					MonitorEventTree(pObject->nodeId(), itRef->targetNodeId());
				}
			}

			itRef++;
		}
	}
	else
	{
		//! ��ͨ��
		map<UaNodeId, UaNodeId>::iterator it = m_mapNodeInArea.find(pObject->nodeId());
		if(it != m_mapNodeInArea.end())
		{
			//! ��ͨ��ֱ�ӹ��ص�����ڵ���
			CDBObjectBaseType* pObj = dynamic_cast<CDBObjectBaseType*>(pNode);
			if(NULL != pObj)
			{
				map<UaNodeId, ADInfo*>::iterator itArea = m_mapAreaWhole.find(it->second);
				if(itArea != m_mapAreaWhole.end())
				{
					ADInfo* pInfo = itArea->second;
					if(NULL != pInfo)
					{
						pObj->setArea1(pInfo->Area1);
						pObj->setArea2(pInfo->Area2);
						pObj->setArea3(pInfo->Area3);
					}
				}
			}
		}
		else
		{
			//! ��ͨ��ֱ�ӹ��ص��豸�ڵ���
			map<UaNodeId, UaNodeId>::iterator itEQ = m_mapNodeInEQ.find(pObject->nodeId());
			if(itEQ != m_mapNodeInEQ.end())
			{
				CDBObjectBaseType* pObj = dynamic_cast<CDBObjectBaseType*>(pNode);
				if(NULL != pObj)
				{
					//! �����豸
					pObj->setDevice(itEQ->second);

					/*
					char sDebug[1024];
					snprintf(sDebug, 1024, "Alarm::Point[%s] EQ[%s]\n", 
							pObject->nodeId().toXmlString().toUtf8(), itEQ->second.toXmlString().toUtf8());
					OutputDebugString(sDebug);
					*/

					//! ��������
					UaNodeId AreaNodeId;
					OpcUa_Boolean ret = getEQNodeInArea(itEQ->second, AreaNodeId);
					if(ret)
					{
						ADInfo* pADInfo = getAreaInfo(AreaNodeId);

						if(pADInfo)
						{
							pObj->setArea1(pADInfo->Area1);
							pObj->setArea2(pADInfo->Area2);
							pObj->setArea3(pADInfo->Area3);
						}
					}
				}
			}
		}
	}
}

void CAreaDeviceParse::SetNodeAreaAndDevInfo()
{

}

OpcUa_Boolean CAreaDeviceParse::getEQNodeInArea(UaNodeId EQNodeId, UaNodeId& nodeId)
{
	OpcUa_Boolean ret = OpcUa_False;

	map<UaNodeId, UaNodeId>::iterator it = m_mapEQ.find(EQNodeId);
	if(it != m_mapEQ.end())
	{
		map<UaNodeId, AreaLevelInfo*>::iterator itArea = m_mapArea.find(it->second);
		
		if(itArea != m_mapArea.end())
		{
			//! EQ�ڵ�ֱ�ӹ�����Area�ڵ���
			nodeId = it->second;
			ret = OpcUa_True;
		}
		else
		{
			//! EQ�ڵ���ܹ�����EQ�ڵ��� �����丸�ڵ�����ϲ�
			ret = getEQNodeInArea(it->second, nodeId);
		}
	}

	return ret;
}

void CAreaDeviceParse::setAllAreaInfo()
{
	NodeManagerRoot* pNodeManagerRoot = NodeManagerRoot::CreateRootNodeManager();
	map<UaNodeId, AreaLevelInfo*>::iterator it;
	for(it = m_mapArea.begin(); it != m_mapArea.end(); it++)
	{
		ADInfo* pInfo = new ADInfo();
		setAreaInfo(it->first, pInfo);

		//! ��¼��map����
		m_mapAreaWhole[it->first] = pInfo;

		//! ������������������
		UaNode* pNode = pNodeManagerRoot->getNode(it->first);
		if(NULL != pNode)
		{
			CDBObjectBaseType* pArea = dynamic_cast<CDBObjectBaseType*>(pNode);
			if(NULL != pArea)
			{
				pArea->setArea1(pInfo->Area1);
				pArea->setArea2(pInfo->Area2);
				pArea->setArea3(pInfo->Area3);
			}
		}
	}
}

ADInfo* CAreaDeviceParse::getAreaInfo(UaNodeId nodeId)
{
	map<UaNodeId, ADInfo*>::iterator it = m_mapAreaWhole.find(nodeId);
	if(it != m_mapAreaWhole.end())
		return it->second;

	return NULL;
}

void CAreaDeviceParse::setAreaInfo(UaNodeId nodeId, ADInfo* pInfo)
{
	map<UaNodeId, AreaLevelInfo*>::iterator it = m_mapArea.find(nodeId);
	if(it != m_mapArea.end())
	{
		AreaLevelInfo* pLevel = it->second;
		if(pLevel)
		{
			OpcUa_UInt16 level = pLevel->level;
			if(level == DEF_Area_Level_1)
			{
				pInfo->Area1 = it->first;
				return;
			}
			else if(level == DEF_Area_Level_2)
			{
				pInfo->Area2 = it->first;
				//! ����������Area1��Ϣ
				pInfo->Area1 = pLevel->parentNode;
				return;
			}
			else if(level == DEF_Area_Level_3)
			{
				pInfo->Area3 = it->first;
				//! ����������Area2��Ϣ
				setAreaInfo(pLevel->parentNode, pInfo);
			}
		}
	}
}

UaNode* CAreaDeviceParse::FindTargetNode(UaNodeId nodeId)
{
	NodeManagerRoot* pNodeManagerRoot = NodeManagerRoot::CreateRootNodeManager();
	if(pNodeManagerRoot)
		return pNodeManagerRoot->getNode(nodeId);

	return NULL;
}

//! ��ȡ�ڵ�ĸ�����ڵ�NodeId
OpcUa_Boolean CAreaDeviceParse::FindParentNode(UaBase::BaseNode* pNode, UaNodeId &nodeId)
{
	OpcUa_Boolean ret = OpcUa_False;

	std::list<UaBase::Reference> lstReferences = pNode->references();
	std::list<UaBase::Reference>::const_iterator itRef = lstReferences.begin();
	while (itRef != lstReferences.end())
	{
		if (!itRef->isForward())
		{
			if (itRef->referenceTypeId() == OpcUaId_HasComponent)
			{
				nodeId = itRef->targetNodeId();
				ret = OpcUa_True;
				break;
			}
		}
		itRef++;
	}

	return ret;
}

//! ������չģ���෽���е�ֵ�仯Դͨ���ʵ����
void CAreaDeviceParse::DealWithExtendObjectMethod(ComponentType* pPoint)
{
	if(NULL == s_pIOTypeNodeManager)
	{
		NodeManagerRoot* pNodeManagerRoot = NodeManagerRoot::CreateRootNodeManager();
		NodeManager* pNodeManager = pNodeManagerRoot->getNodeManagerByNamespace(RTDB_TYPE_NS_INDEX);
		if(pNodeManager)
		{
			s_pIOTypeNodeManager = dynamic_cast<IOTypeNodeManager*>(pNodeManager);
		}
	}

	if(NULL != s_pIOTypeNodeManager)
	{
		/*
		vector<ValueTriggerScript*> vecSrc;
		OpcUa_Boolean ret = s_pIOTypeNodeManager->GetChangeSources(pPoint->typeDefinitionId(), vecSrc);
		if(ret)
		{
			vector<ValueTriggerScript*>::iterator it;
			for(it = vecSrc.begin(); it != vecSrc.end(); it++)
			{
				ValueTriggerScript* pSrc = *it;

				if(NULL != pSrc)
				{
					vector<UaString>::iterator it2;
					for(it2 = pSrc->vecSrc.begin(); it2 != pSrc->vecSrc.end(); it2++)
					{
						//! ����ֵ�仯Դͨ���
						UaString sScript = *it2;
						DealWithWildCardsEx(pPoint->nodeId(), sScript);

						//! ����ű���ͨ���
						UaString sScriptBody = pSrc->sScript;
						DealWithWildCardsEx(pPoint->nodeId(), sScriptBody);

						ScriptRegion* pScript = new ScriptRegion();
						pScript->scriptCxt.sScript = sScriptBody;
						//! ��¼��������ַ��� ��������滻ֵ�仯Դͨ���
						pScript->oriScriptCxt = sScriptBody;

						//g_pCallBackMgr->AddCallBackVariable(UaNodeId::fromXmlString(sScript), pScript);
						DealWithTypeChangeSources(sScript, pScript);
					}
				}
			}
		}
		*/

		map<OpcUa::BaseMethod*, vector<ValueTriggerScript*> > mapSources;
		OpcUa_Boolean ret = s_pIOTypeNodeManager->GetChangeSourcesEx(pPoint->typeDefinitionId(), mapSources);
		if(ret)
		{
			vector<ValueTriggerScript*>::iterator it;

			map<OpcUa::BaseMethod*, vector<ValueTriggerScript*> >::iterator itMethod;
			for(itMethod = mapSources.begin(); itMethod != mapSources.end(); itMethod++)
			{
				//! ģ����ķ�����ֵ�仯���� �������ʵ����д�˸÷��� ����ʵ��Ϊ׼
				UaQualifiedName sBrowse(itMethod->first->browseName().name(), pPoint->nodeId().namespaceIndex());
				UaNode* pSameMethod = pPoint->getTargetNodeByBrowseName(sBrowse);
				if(NULL != pSameMethod)
					continue;

				vector<ValueTriggerScript*> vecSrc = itMethod->second;
				for(it = vecSrc.begin(); it != vecSrc.end(); it++)
				{
					ValueTriggerScript* pSrc = *it;

					if(NULL != pSrc)
					{
						vector<UaString>::iterator it2;
						for(it2 = pSrc->vecSrc.begin(); it2 != pSrc->vecSrc.end(); it2++)
						{
							//! ����ֵ�仯Դͨ���
							UaString sScript = *it2;
							DealWithWildCardsEx(pPoint->nodeId(), sScript);

							//! ����ű���ͨ���
							UaString sScriptBody = pSrc->sScript;
							DealWithWildCardsEx(pPoint->nodeId(), sScriptBody);

							ScriptRegion* pScript = new ScriptRegion();
							pScript->scriptCxt.sScript = sScriptBody;
							//! ��¼��������ַ��� ��������滻ֵ�仯Դͨ���
							pScript->oriScriptCxt = sScriptBody;

							//g_pCallBackMgr->AddCallBackVariable(UaNodeId::fromXmlString(sScript), pScript);
							DealWithTypeChangeSources(sScript, pScript);
						}
					}
				}
				vecSrc.clear();
			}
		}
	}
}

void CAreaDeviceParse::MonitorEventTree(UaNodeId source, UaNodeId target)
{
	map<UaNodeId, vector<UaNodeId> >::iterator it = m_mapEventReferences.find(source);
	if(it != m_mapEventReferences.end())
	{
		it->second.push_back(target);
	}
	else
	{
		vector<UaNodeId> vecEvent;
		vecEvent.push_back(target);
		m_mapEventReferences.insert(make_pair<UaNodeId, vector<UaNodeId> >(source, vecEvent));
	}
}

void CAreaDeviceParse::MonitorDBUnitFolder(UaBase::Object* pObject)
{
	std::list<UaBase::Reference> lstReferences = pObject->references();
	std::list<UaBase::Reference>::const_iterator itRef = lstReferences.begin();
	while (itRef != lstReferences.end())
	{
		if (itRef->isForward())
		{
			if (itRef->referenceTypeId() == OpcUaId_HasComponent ||
				itRef->referenceTypeId() == OpcUaId_Organizes)
			{
				MonitorEventTree(pObject->nodeId(), itRef->targetNodeId());
			}
		}
		itRef++;
	}
}

void CAreaDeviceParse::GetAllEventTree(map<UaNodeId, vector<UaNodeId> > & treeMap)
{
	treeMap = m_mapEventReferences;
}

void CAreaDeviceParse::ClearResources()
{
	if(!m_mapEventReferences.empty())
	{
		map<UaNodeId, vector<UaNodeId> >::iterator it;
		for(it = m_mapEventReferences.begin(); it != m_mapEventReferences.end(); it++)
		{
			if(!it->second.empty())
				it->second.clear();
		}
	}
	m_mapEventReferences.clear();

	if(!m_mapArea.empty())
	{
		map<UaNodeId, AreaLevelInfo*>::iterator it;
		for(it = m_mapArea.begin(); it != m_mapArea.end(); it++)
		{
			if(NULL != it->second)
			{
				delete it->second;
				it->second = NULL;
			}
		}
	}
	m_mapArea.clear();


	if(!m_mapAreaWhole.empty())
	{
		map<UaNodeId, ADInfo*>::iterator it;
		for(it = m_mapAreaWhole.begin(); it != m_mapAreaWhole.end(); it++)
		{
			if(NULL != it->second)
			{
				delete it->second;
				it->second = NULL;
			}
		}
	}
	m_mapAreaWhole.clear();

	m_mapEQ.clear();
	m_mapNodeInArea.clear();
	m_mapNodeInEQ.clear();
}

void CAreaDeviceParse::DealMethodCreated(UaMethod* pNewNode, UaBase::Method *pMethod)
{
	//! �������нڵ���δ������� ���������¼
	if(NULL != pNewNode)
		m_vecMethodWithScript.push_back(pNewNode);
}

void CAreaDeviceParse::DealAllMethodScript()
{
	//! �����ű������滻
	ChangeAllScript();

	//! �����ű�ִ��
	DealAllUserScript();
}

void CAreaDeviceParse::ChangeAllScript()
{
	if(!m_vecMethodWithScript.empty())
	{
		UaMethod* pMethod = NULL;
		MyMethodUserData* pUserData = NULL;
		ScriptRegion* pScriptRegion = NULL;
		vector<UaMethod*>::iterator it;

		for(it = m_vecMethodWithScript.begin(); it != m_vecMethodWithScript.end(); it++)
		{
			pMethod = *it;
			pUserData = (MyMethodUserData*)pMethod->getUserData();
			if(pUserData)
			{
				if(pUserData->IsSystem())
				{
					//! ϵͳ����ֻ������ǰ�ͷ�����
					if(pUserData->hasFront())
					{
						pScriptRegion = pUserData->getFrontScript();
						ChangeScript(pMethod, pScriptRegion);
					}

					if(pUserData->hasBack())
					{
						pScriptRegion = pUserData->getBackScript();
						ChangeScript(pMethod, pScriptRegion);
					}

					if(pUserData->hasFeedFront())
					{
						pScriptRegion = pUserData->getFeedFrontScript();
						ChangeScript(pMethod, pScriptRegion);
					}

					if(pUserData->hasFeedBack())
					{
						pScriptRegion = pUserData->getFeedBackScript();
						ChangeScript(pMethod, pScriptRegion);
					}

				}
				else
				{
					//! �û��Զ��巽��ֻ�����û�����
					pScriptRegion = pUserData->getUserScript();
					ChangeScript(pMethod, pScriptRegion);
				}
			}
		}
	}
}

void CAreaDeviceParse::ChangeScript(UaMethod* pMethod, ScriptRegion* pRegion)
{
	if(NULL != pMethod && NULL != pRegion)
	{
		UaString sScript = pRegion->scriptCxt.sScript;
		DealWithWildCards(pMethod->nodeId(), sScript);
		pRegion->scriptCxt.sScript = sScript;
		pRegion->oriScriptCxt = sScript;

		//! �ű����� ���ڶ�ʱע��ʱ��Ҫ
		pRegion->scriptCxt.sName = pMethod->nodeId().toXmlString();
	}
}

void CAreaDeviceParse::GetTypeChangeSources(UaNodeId sourceId, UaString sType, UaString sSuffix, OpcUa_Boolean bInclude, vector<UaNodeId> &vecSources)
{
	if(!sourceId.isNull())
	{
		UaNode* pNode = NULL;
		UaNode* pTarget = NULL;
		UaNodeId targetId;
		map<UaNodeId, OpcUa_Int16> mapSources;

		UaNodeId typeId;
		if(sType == DEF_FieldPointType)
			typeId.setNodeId(sType, FEP_TYPE_NS_INDEX);
		else
			typeId.setNodeId(sType, RTDB_TYPE_NS_INDEX);

		NodeManagerRoot* pNodeManagerRoot = NodeManagerRoot::CreateRootNodeManager();
		if(pNodeManagerRoot)
		{
			pNode = pNodeManagerRoot->getNode(sourceId);
			if(pNode)
			{
				UaNode* pTargetNode = NULL;
				UaNodeId referenceTypeId;
				UaReference* pReference = NULL;
				UaReferenceLists* pReferenceList = pNode->getUaReferenceLists();

				if(pReferenceList)
				{
					pReference = (UaReference*)pReferenceList->pTargetNodes();
					while (pReference)
					{		
						pTargetNode = pReference->pTargetNode();
						if(pTargetNode)
						{
							targetId = pTargetNode->nodeId();
							pTarget = pNodeManagerRoot->getNode(targetId);

							if(bInclude)
							{
								if(NULL != g_pNodeFactoryManager)
								{
									if(g_pNodeFactoryManager->CheckBelongto(pTarget->typeDefinitionId(), typeId))
										mapSources[targetId] = 1;
								}
							}
							else
							{
								if(pTarget->typeDefinitionId() == typeId)
								{
									mapSources[targetId] = 1;
								}
							}		
						}							

						pReference = pReference->pNextForwardReference();
					}
				}
			}
		}

		//! ��Ӻ�׺
		if(!sSuffix.isNull() && !sSuffix.isEmpty())
		{
			UaString sNodeId;
			map<UaNodeId, OpcUa_Int16>::iterator it;
			for(it = mapSources.begin(); it != mapSources.end(); it++)
			{
				sNodeId = it->first.toXmlString();
				sNodeId += sSuffix;
				vecSources.push_back(UaNodeId::fromXmlString(sNodeId));
			}
		}
		else
		{
			map<UaNodeId, OpcUa_Int16>::iterator it;
			for(it = mapSources.begin(); it != mapSources.end(); it++)
			{
				vecSources.push_back(it->first);
			}
		}
	}
}

void CAreaDeviceParse::CreateValueChange()
{
	if(!m_vecMethodWithScript.empty())
	{
		UaMethod* pMethod = NULL;
		MyMethodUserData* pUserData = NULL;
		ScriptRegion* pScriptRegion = NULL;
		vector<UaMethod*>::iterator it;

		for(it = m_vecMethodWithScript.begin(); it != m_vecMethodWithScript.end(); it++)
		{
			pMethod = *it;
			pUserData = (MyMethodUserData*)pMethod->getUserData();
			if(pUserData)
			{
				//! �������û��Զ��巽��  ϵͳ������ǰ��ű������޴�����
				if(!pUserData->IsSystem())
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

								vector<UaString>::iterator it2;
								for(it2 = changeSources.begin(); it2 != changeSources.end(); it2++)
								{
									UaString sSource = *it2;

									//! Modify by wyx 2014.11.18 ֵ�仯ԴҲ���Ժ�ͨ���
									DealWithWildCards(pMethod->nodeId(), sSource);

									DealWithTypeChangeSources(sSource, pScriptRegion);
								}
							}
							break;
						default:
							break;
						}
					}
				}
			}
		}
	}
}

void CAreaDeviceParse::DealAllUserScript()
{
	if(!m_vecMethodWithScript.empty())
	{
		UaMethod* pMethod = NULL;
		MyMethodUserData* pUserData = NULL;
		ScriptRegion* pScriptRegion = NULL;
		vector<UaMethod*>::iterator it;

		for(it = m_vecMethodWithScript.begin(); it != m_vecMethodWithScript.end(); it++)
		{
			pMethod = *it;
			pUserData = (MyMethodUserData*)pMethod->getUserData();
			if(pUserData)
			{
				//! �������û��Զ��巽��  ϵͳ������ǰ��ű������޴�����
				if(!pUserData->IsSystem())
				{
					pScriptRegion = pUserData->getUserScript();
					if(pScriptRegion)
					{
						switch(pScriptRegion->scriptItem.triggerMode)
						{
						case TriggerMode_Period:
							{
								UaVariant vart;
								/*
								if(NULL != g_pHotQueue)
								{
									//! ���������ݼ���
									OpcUa_UInt32 period = g_pHotQueue->GetPeriod(pMethod->nodeId());
									if(0 != period)
										pScriptRegion->scriptItem.period = period;
								}
								*/
								DealWithScript(pScriptRegion, vart);
							}

							break;
						case TriggerMode_SetTime:
							{
								UaVariant vart;
								DealWithScript(pScriptRegion, vart);
							}

							break;
						case TriggerMode_Begin:
							{
								//! ����ʱ��������ִ������
								if(!IsSlave(m_iNamespaceIndex))
								{
									UaVariant vart;
									DealWithScript(pScriptRegion, vart);
								}

							}

							break;

						case TriggerMode_End:
							m_vecEndScripts.push_back(pScriptRegion);
							
							break;
						default:
							break;
						}
					}
				}
			}
		}
	}
}

void CAreaDeviceParse::AddChangeVariable(UaNodeId sourceNode, ScriptRegion* pScriptRegion)
{
	map<UaNodeId, CCallBackVariable*>::iterator it = m_mapChangeSources.find(sourceNode);
	if(it == m_mapChangeSources.end())
	{
		CCallBackVariable* pVariable = new CCallBackVariable(sourceNode);
		pVariable->AddCallBackInfo(pScriptRegion);
		m_mapChangeSources.insert(make_pair<UaNodeId, CCallBackVariable*>(sourceNode, pVariable));
	}
	else
	{
		it->second->AddCallBackInfo(pScriptRegion);
	}
}

void CAreaDeviceParse::AddUnitChangeVariables()
{
	if(!m_mapChangeSources.empty())
	{
		if(NULL != g_pCallBackMgr)
			g_pCallBackMgr->AddUnitChangeSources(m_mapChangeSources);
	}
	m_mapChangeSources.clear();
}

void CAreaDeviceParse::DealWithTypeChangeSources(UaString sSource, ScriptRegion* pScriptRegion)
{
	UaString sType;
	UaString sSuffix;
	OpcUa_Boolean bInclude;
	UaNodeId node = DealWithChangeScript(sSource, sType, sSuffix, bInclude);

	if(sType.isNull() || sType.isEmpty())
		//g_pCallBackMgr->AddCallBackVariable(UaNodeId::fromXmlString(sSource), pScriptRegion, m_iNamespaceIndex);
		AddChangeVariable(UaNodeId::fromXmlString(sSource), pScriptRegion);
	else
	{
		vector<UaNodeId> vecChanges;
		GetTypeChangeSources(node, sType, sSuffix, bInclude, vecChanges);
		vector<UaNodeId>::iterator it3;
		for(it3 = vecChanges.begin(); it3 != vecChanges.end(); it3++)
		{
			//g_pCallBackMgr->AddCallBackVariable(*it3, pScriptRegion, m_iNamespaceIndex);
			AddChangeVariable(*it3, pScriptRegion);
		}							
	}
}
