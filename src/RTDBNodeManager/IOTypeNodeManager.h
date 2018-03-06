/*!
 *	\file	IOTypeNodeManager.h
 *
 *	\brief	FEP���ͽڵ������
 *	
 *
 *	\author lingling.li
 *
 *	\date	2014��4��21��	13:56
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4ϵͳ������,HollySys Co.,Ltd
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
		vector<UaString>	vecSrc;		//! ֵ�仯Դ�б�
		UaString			sScript;	//! ��Ӧ�ű�
	};

/*!
 *	\class	IOTypeNodeManager
 *
 *	\brief	��̵���˵������ѡ��
 *
 *	��ϸ����˵������ѡ��
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

	//��ñ�������ָ��
	UaVariable* getInstanceDeclarationVariable(OpcUa_UInt32 numericIdentifier);

	//����FEP����
	static void createFepTypes();
	//����RTDB����
	static void createRTDBTypes();

	/*!
	*`	\brief	�����Զ�������µ��Զ��巽��
	*			���Զ�����󴴽�ʵ��ʱ,������д�ɲ�д
	*			��д�Ļ�client���ڵ��øö���ĸ÷���ʱ
	*			������ObjectType�²��ҷ���ģ��ִ��
	*
	*	\param	��
	*	\retval	��
	*/
	void RecordObjectTypeMethods();

	/*!
	*`	\brief	��ȡ�Զ���Ķ�������
	*
	*	\param	std::vector<UaNodeId> &vecObjectType[out]:���ص��Զ����������
	*	\retval	��
	*/
	void GetSelfObjectType(std::vector<UaNodeId> &vecObjectType);

	/*!
	*`	\brief	���ݶ���Id�ͷ���Idȷ���Զ����������Ƿ����method����
	*
	*			��������:
	*			AIType---->AI001Type[Fun1 Fun2]---->AI002Type[Fun3 Fun4]
	*			�Զ�������AI001Type�̳���AIType,������������Fun1��Fun2
	*			�Զ�������AI002Type�̳���AI001Type,���������µķ���Fun3��Fun4
	*			Ŀǰ����һ��AI002Type���͵Ľڵ�Test,�ͻ���Ԥ�����䷽��Fun2
	*
	*			��������:
	*			ע��ǰ��:������ǿ�а�Test�ڵ��TypeDefinitionId��Ϊ��AIType
	*			RTDBXmlUaNodeFactoryNamespace�ڲ���¼�������Զ������͵ļ̳й�ϵ�Ͷ���ʵ�������ʼTypeDefinitionId
	*			���ҵ�����Test����������AI002Type,���շ�����Fun2���䷽�������в��� ʧ��
	*			��Ҫ�ݹ���AI001Type�ķ��������м������� ֱ���鵽�������������ֹ
	*			
	*
	*	\param1	UaNode*			[in]	��Ч����ڵ�
	*	\param2	OpcUa_NodeId*	[in]	Ԥ���õķ���Id
	*	\retval	MethodHandleUaNode*	
	*/
	MethodHandleUaNode* GetSelfMethodHandle(UaNode* pObject, OpcUa_NodeId* pMethodNodeId);

	//! ����ʵ���������ͻ�ȡֵ�仯Դ�б�
	OpcUa_Boolean GetChangeSources(UaNodeId typeDefinitionId, vector<ValueTriggerScript*>& vecSources);

	OpcUa_Boolean GetChangeSourcesEx(UaNodeId typeDefinitionId, map<OpcUa::BaseMethod*, vector<ValueTriggerScript*> >& mapSources);

	//! ģ������ķ���ʵ���ﲻ����д ʵ������ǿ�����ӷ���
	OpcUa_Boolean GetSelfMethods(UaNodeId typeDefinitionId, vector<OpcUa::BaseMethod*>& vecMethods);

protected:

private:
	//FEP���ʹ���������־
	static bool s_feptypeCreated;
	//RTDB���ʹ�����־
	static bool s_rtdbtypeCreated;
	//! allNodesAndReferencesCreated�ӿ��Ƿ����
	static bool s_typeCreated;

	//! ��¼ObjectType�¹��ط���
	map<UaNodeId, vector<OpcUa::BaseMethod*> >	m_mapObjectTypeMethods;
	//! ��¼ObjectType���ýڵ� Ϊ�����Ʒ�����¼����
	map<UaObjectType*, std::list<UaBase::Reference> >		m_mapObjectType;
	//! ��¼ģ�巽���µ�ֵ�仯Դ��Ϣ(���ٹ��ķ�������ע�ض�������)
	map<UaNodeId, vector<ValueTriggerScript*> >	m_mapValueChangeObjectScript;
	//! ��¼ģ�巽���µ�ֵ�仯Դ��Ϣ(���ķ���������������������)
	map<OpcUa::BaseMethod*, vector<ValueTriggerScript*> > m_mapValueChangeMethodScript;
};
}
#endif


