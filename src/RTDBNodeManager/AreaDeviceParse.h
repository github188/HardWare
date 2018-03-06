/*!
*	\file	AreaDeviceParse.h
*
*	\brief	�ڵ������������豸������
*			���򱨾�ע��
*			�ű�����
*			��Ϊ�������ദ��Ref�����������Reverse�෴�����
*
*			�����������һ������������������������
*			���������豸�������豸�²�����������
*			�豸�¿��������豸
*			�㼶��ϵ�������£�
*
*			���ݿⵥԪ�ļ���
*					����Area1
*						����Area2
*							����Area3
*								����EQ1
*									����EQ2
*										����EQ3
*											����AI001
*			
*			��ôAI001���������ΪArea123�������豸ΪEQ3
*
*			$AREA����Ƕ�ף����Թ���ϵͳ�ּ���
*			$AREA��Organizes����������֯�����������$EQ��������
*
*			$EQ���Ծۺ�������$EQ��װ����Ϊ�Լ��Ĳ���
*			$EQ��HasComponent������ϸ�����������$EQ�������Ͷ���
*
*
*			�������Area����Ҫ�����⴦��������ͳ�ƹ��ܣ���Ҫ��Area�¹��ص����ж���ע�ᡣ
*			��Area���󴴽�ʱ��������ReferenceList�ҵ�ÿ�����ض���
*			��ʱ��Ҫ��������ǣ�
*				A������֮ǰ�������ˣ�ֱ��ע��
*				B��������δ�������Ƚ��ö���NodeId��¼��map���У�����objectCreated����ʱ��
*					���ж��ö����Ƿ���map���У�������ڣ���ReferenceList�����ҵ�Area������ִ��ע��
*
*			ʵʱ��ڵ�������ศ����
*			
*
*	$Author: Wangyuanxing $
*	$Date: 14-04-01 9:09 $
*	$Revision: 1.0 $
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef __AREADEVICEPARSE_H__
#define __AREADEVICEPARSE_H__

#include "basenodes.h"
#include "uabasenodes.h"
#include "RTDB/Server/DBPlatformClass/BaUserData.h"
#include "RTDB/Server/DBPlatformClass/ComponentType.h"
#include "RTDB/Server/DBPlatformClass/CallBackVariable.h"
#include "IOTypeNodeManager.h"

#include <map>
using namespace std;

namespace MACS_SCADA_SUD
{
	/*!
	*	\brief	����ڵ�Ĳ�ι�ϵ
	*			�����3������
	*			level��ע��ǰ�ڵ��ǵڼ�������
	*			parentNode��ע���ϲ�����ڵ��NodeId
	*			bSetOver��ע�ýڵ�Info��Ϣ�Ƿ��ע���
	*/
	typedef struct AreaLevelInfo
	{
		OpcUa_UInt16	level;
		UaNodeId		parentNode;
		OpcUa_Boolean	bSetOver;
	};

	/*!
	*	\brief	��ͨ�����ڵ��������豸�ڵ���Ϣ
	*			AreaNodeId��ע��ǰ�ڵ������ص�����ڵ�NodeId
	*			DevNodeId��ע��ǰ�ڵ������ص��豸�ڵ�NodeId
	*			������һ��ȷ����ǰ�ڵ���ϼ�������Ϣ,
	*			����AreaNodeId�����򼶱�map���ж�λ����
	*/
	typedef struct AreaDevInfo
	{
		UaNodeId		AreaNodeId;
		UaNodeId		DevNodeId;
	};

	/*!
	*	\brief	��ͨ�����ڵ������������豸�ڵ���Ϣ
	*/
	typedef struct ADInfo
	{
		UaNodeId		Area1;
		UaNodeId		Area2;
		UaNodeId		Area3;
		//UaNodeId		Dev;
	};

	class CAreaDeviceParse
	{
	public:
		CAreaDeviceParse();
		virtual ~CAreaDeviceParse();

		void SetIndex(OpcUa_UInt16 nsIndex);

		/*!
		*	\brief	�����ڵ�����NodeId,�ж��Ƿ�ΪAreaType����
		*			
		*	\param	typeId	[in]	�ڵ�����NodeId
		*	\retval	OpcUa_Boolean
		*/
		virtual OpcUa_Boolean IsAreaType(UaNodeId typeId);

		/*!
		*	\brief	�����ڵ�����NodeId,�ж��Ƿ�ΪFolderType����
		*			
		*	\param	typeId	[in]	�ڵ�����NodeId
		*	\retval	OpcUa_Boolean
		*/
		virtual OpcUa_Boolean IsFolderType(UaNodeId typeId);

		/*!
		*	\brief	�����ڵ�����NodeId,�ж��Ƿ�ΪEQType����
		*			
		*	\param	typeId	[in]	�ڵ�����NodeId
		*	\retval	OpcUa_Boolean
		*/
		virtual OpcUa_Boolean IsEQType(UaNodeId typeId);

		/*!
		*	\brief	�����ڵ�����NodeId,�ж��Ƿ�Ϊ������������������
		*			
		*	\param	typeId	[in]	�ڵ�����NodeId
		*	\retval	OpcUa_Boolean
		*/
		virtual OpcUa_Boolean IsPointType(UaNodeId NodeId);

		/*!
		*	\brief	object���󴴽�ʱ, ���ô˽ӿڽ�����������豸��Ϣ��ϵ
		*
		*	\param	pNode	[in]			�ڵ����
		*	\param	pObject	[in]			�ڵ�
		*	\retval	��
		*/
		virtual void parseNodeAreaDeviceInfo(UaObject* pNode, UaBase::Object* pObject);

		
		/*!
		*	\brief	�����нڵ������Ϻ� ͳһͳ�ƻ������������������豸��Ϣ
		*
		*	\retval	��
		*/
		virtual void SetNodeAreaAndDevInfo();

		/*!
		*	\brief	��ȡEQ�ڵ������ص�Area�ڵ�NodeId
		*
		*	\param	EQNodeId	[in]		EQ�ڵ�NodeId
		*	\param	nodeId		[in,out]	Area�ڵ�NodeId
		*	\retval	OpcUa_Boolean					�����Ƿ�ɹ���־
		*/
		virtual OpcUa_Boolean getEQNodeInArea(UaNodeId EQNodeId, UaNodeId& nodeId);

		/*!
		*	\brief	����һ�������ڵ��ֱ�ӹ�����Area����ڵ�NodeId,��ȡ��3��Area�ڵ�NodeId��Ϣ
		*
		*	\param	nodeId	[in]			�����ڵ����ڵ���ײ�����ڵ�NodeId
		*	\param	pInfo	[in,out]		�����ڵ��3��������豸�ڵ�NodeId��Ϣ����
		*	\retval	��
		*/
		virtual void setAreaInfo(UaNodeId nodeId, ADInfo* pInfo);
		virtual ADInfo* getAreaInfo(UaNodeId nodeId);
		virtual void setAllAreaInfo();

		////////////////////////////////////���²���ΪArea����ͳ��ע�Ṧ�ܷ���//////////////////////////////////////
		UaNode* FindTargetNode(UaNodeId nodeId);


		////////////////////////////////////���²���ΪMethod�����ű����ܷ���//////////////////////////////////////
		virtual void DealMethodCreated(UaMethod* pNewNode, UaBase::Method *pMethod);
		virtual void DealAllMethodScript();
		virtual void ChangeAllScript();
		virtual void ChangeScript(UaMethod* pMethod, ScriptRegion* pRegion);
		virtual void DealAllUserScript();
		virtual void CreateValueChange();
		virtual void GetTypeChangeSources(UaNodeId sourceId, UaString sType, UaString sSuffix, OpcUa_Boolean bInclude, vector<UaNodeId> &vecSources);
		virtual void DealWithTypeChangeSources(UaString sSource, ScriptRegion* pRegion);
		virtual void AddChangeVariable(UaNodeId sourceNode, ScriptRegion* pScriptRegion);
		virtual void AddUnitChangeVariables();

		//! ��ȡ�ڵ�ĸ�����ڵ�NodeId
		OpcUa_Boolean FindParentNode(UaBase::BaseNode* pNode, UaNodeId &nodeId);

		//! ������չģ���෽���е�ֵ�仯Դͨ���ʵ����
		virtual void DealWithExtendObjectMethod(ComponentType* pPoint);

		////////////////////////////////////���²���Ϊ�����豸�������㴴���¼�֪ͨ�����ܷ���//////////////////////////////////////
		virtual void MonitorEventTree(UaNodeId source, UaNodeId target);
		virtual void MonitorDBUnitFolder(UaBase::Object* pObject);
		virtual void GetAllEventTree(map<UaNodeId, vector<UaNodeId> >& treeMap);
		virtual void ClearResources();

	protected:

		OpcUa_UInt16							m_iNamespaceIndex;

		/*!
		*	\brief	����Ϊ3������map�б� 
		*			����keyΪAreaType�ڵ����FolderType�ڵ��NodeId, 
		*			valueΪ��ǰ�ڵ�����򼶱�����ϼ�����ڵ��NodeId
		*/
		map<UaNodeId, AreaLevelInfo*>			m_mapArea;


		/*!
		*	\brief	����m_mapArea����ڵ�map����ÿ��Area�ڵ��3��������ͳ��
		*			�����ÿ����������������
		*/
		map<UaNodeId, ADInfo*>					m_mapAreaWhole;

		/*
		*	\brief	�豸�ڵ�Ĳ㼶��ϵ
		*			keyΪ�豸�ڵ��NodeId
		*			valueΪ�豸�ڵ��ϲ�ڵ��NodeId
		*			�ϲ�ڵ������EQ\Area\Folder
		*/
		map<UaNodeId, UaNodeId>					m_mapEQ;

		map<UaNodeId, UaNodeId>					m_mapNodeInArea;
		map<UaNodeId, UaNodeId>					m_mapNodeInEQ;

		vector<ScriptRegion*>					m_vecEndScripts;

		map<UaNodeId, vector<UaNodeId> >		m_mapEventReferences;
		map<UaNodeId, CCallBackVariable*>		m_mapChangeSources;

		vector<UaMethod*>						m_vecMethodWithScript;
		static IOTypeNodeManager*				s_pIOTypeNodeManager;
	};

}
#endif