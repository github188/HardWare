/*!
 *	\file	IOTypeNodeManager.h
 *
 *	\brief	FEP类型节点管理器
 *	
 *
 *	\author lingling.li
 *
 *	\date	2014年4月21日	13:56
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef IOTypeNodeManager_h__
#define IOTypeNodeManager_h__
#pragma once

#include "opcua_platformdefs.h"
#include "opcua_baseobjecttype.h"
#include "nodemanagernodesetxml.h"
#include "methodhandleuanode.h"

namespace MACS_SCADA_SUD {

	typedef struct ValueTriggerScript
	{
		vector<UaString>	vecSrc;		//! 值变化源列表
		UaString			sScript;	//! 对应脚本
	};

/*!
 *	\class	IOTypeNodeManager
 *
 *	\brief	简短的类说明（可选）
 *
 *	详细的类说明（可选）
 */
class IOTypeNodeManager :
	public NodeManagerNodeSetXml
{
	UA_DISABLE_COPY(IOTypeNodeManager);
public:
	IOTypeNodeManager(const UaString& sNamespaceUri);
	virtual ~IOTypeNodeManager();

	// NodeManagerUaNode implementation
	virtual UaStatus   afterStartUp();
	virtual UaStatus   beforeShutDown();

	virtual void objectTypeCreated(UaObjectType* pNewNode, UaBase::ObjectType *pObjectType);
	virtual void methodCreated(UaMethod* pNewNode, UaBase::Method *pMethod);
	virtual void allNodesAndReferencesCreated();

	//获得变量声明指针
	UaVariable* getInstanceDeclarationVariable(OpcUa_UInt32 numericIdentifier);

	//创建FEP类型
	static void createFepTypes();
	//创建RTDB类型
	static void createRTDBTypes();

	/*!
	*`	\brief	处理自定义对象下的自定义方法
	*			该自定义对象创建实例时,方法可写可不写
	*			不写的话client端在调用该对象的该方法时
	*			必须向ObjectType下查找方法模板执行
	*
	*	\param	空
	*	\retval	空
	*/
	void RecordObjectTypeMethods();

	/*!
	*`	\brief	获取自定义的对象类型
	*
	*	\param	std::vector<UaNodeId> &vecObjectType[out]:返回的自定义对象类型
	*	\retval	空
	*/
	void GetSelfObjectType(std::vector<UaNodeId> &vecObjectType);

	/*!
	*`	\brief	依据对象Id和方法Id确定自定义类型中是否存在method声明
	*
	*			举例如下:
	*			AIType---->AI001Type[Fun1 Fun2]---->AI002Type[Fun3 Fun4]
	*			自定义类型AI001Type继承于AIType,挂载两个方法Fun1和Fun2
	*			自定义类型AI002Type继承于AI001Type,挂载两个新的方法Fun3和Fun4
	*			目前创建一个AI002Type类型的节点Test,客户端预调用其方法Fun2
	*
	*			处理流程:
	*			注意前提:代码里强行把Test节点的TypeDefinitionId改为了AIType
	*			RTDBXmlUaNodeFactoryNamespace内部记录了所有自定义类型的继承关系和对象实例的最初始TypeDefinitionId
	*			先找到对象Test的类型声明AI002Type,按照方法名Fun2在其方法链表中查找 失败
	*			需要递归往AI001Type的方法链表中继续查找 直到查到基础类库类型终止
	*			
	*
	*	\param1	UaNode*			[in]	有效对象节点
	*	\param2	OpcUa_NodeId*	[in]	预调用的方法Id
	*	\retval	MethodHandleUaNode*	
	*/
	MethodHandleUaNode* GetSelfMethodHandle(UaNode* pObject, OpcUa_NodeId* pMethodNodeId);

	//! 依据实例对象类型获取值变化源列表
	OpcUa_Boolean GetChangeSources(UaNodeId typeDefinitionId, vector<ValueTriggerScript*>& vecSources);

	OpcUa_Boolean GetChangeSourcesEx(UaNodeId typeDefinitionId, map<OpcUa::BaseMethod*, vector<ValueTriggerScript*> >& mapSources);

	//! 模板内组的方法实例里不再重写 实例必须强行增加方法
	OpcUa_Boolean GetSelfMethods(UaNodeId typeDefinitionId, vector<OpcUa::BaseMethod*>& vecMethods);

protected:

private:
	//FEP类型创建创建标志
	static bool s_feptypeCreated;
	//RTDB类型创建标志
	static bool s_rtdbtypeCreated;
	//! allNodesAndReferencesCreated接口是否处理过
	static bool s_typeCreated;

	//! 记录ObjectType下挂载方法
	map<UaNodeId, vector<OpcUa::BaseMethod*> >	m_mapObjectTypeMethods;
	//! 记录ObjectType配置节点 为二进制方法记录服务
	map<UaObjectType*, std::list<UaBase::Reference> >		m_mapObjectType;
	//! 记录模板方法下的值变化源信息(不再关心方法而是注重对象类型)
	map<UaNodeId, vector<ValueTriggerScript*> >	m_mapValueChangeObjectScript;
	//! 记录模板方法下的值变化源信息(关心方法不关心整个对象类型)
	map<OpcUa::BaseMethod*, vector<ValueTriggerScript*> > m_mapValueChangeMethodScript;
};
}
#endif


