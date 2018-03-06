/*!
*	\file	NodeManagerCompositeImpl.h
*
*	\brief	���ڵ������ʵ���� 
*			ʵ�ָ��ڵ�������ĸ�����ӿ�
*
*	$Author: Wangyuanxing $
*	$Date: 14-04-01 9:09 $
*	$Revision: 1.0 $
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef __NODEMANAGERCOMPOSITEIMPL_H__
#define __NODEMANAGERCOMPOSITEIMPL_H__

#include <map>
#include <vector>
#include "RTDBCommon/datavalue.h"
#include "NodeManagerGeneral.h"
#include "RTDBBaseClasses/NodeManagerComposite.h"

using namespace std;

namespace MACS_SCADA_SUD
{
	class CIONodeManager;
	/*!
	*	\class	CNodeManagerCompositeImpl
	*
	*	\brief	���ڵ������ʵ����
	*/
	class IONODEMANAGER_DECL CNodeManagerCompositeImpl
		:public CNodeManagerComposite		
	{
	public:
		CNodeManagerCompositeImpl();
		virtual ~CNodeManagerCompositeImpl();

		/*!
		*	\brief	 ��ʼ��
		*			 ��ʽ������ΪNodeManager; 
		*			 xml����ʽΪModule,�����ᴴ����Ӧ��NodeManager
		*
		*	\retval	 �����ɹ�����־
		*			 0�ɹ�	����ʧ��
		*/
		int Init();

		/*!
		*	\brief	 ж��
		*			 ��ַ�ռ���ʵʱ��ı������л�
		*			 ���ڴ�ָ��Ļ���
		*	\param	 ��
		*	\retval	 �����ɹ�����־
		*			 0�ɹ�	����ʧ��
		*/
		int UnInit();

		/*!
		*	\brief	 ��ʼ���� 
		*			 �������ø����߳�
		*			 ������NodeManager��Module�����ڵ�
		*			 �����ṩOpc UA����
		*	\param	 ��
		*	\retval	 �����ɹ�����־
		*			 0�ɹ�	����ʧ��
		*/
		int StartWork();

		/*!
		*	\brief	 ֹͣ���� 
		*			 ֹͣ���ø����߳�
		*			 ����ֹͣ�ṩOpc UA����
		*	\param	 ��
		*	\retval	 �����ɹ�����־
		*			 0�ɹ�	����ʧ��
		*/
		int StopWork();

		//�����Ƿ��Ѿ�����
		bool IsStarted();

		/*!
		*	\brief	�趨FEPģ�����ݿⵥԪCIONodeManagerָ��
		*			RTDB��FEP�ϲ���һ��ʱ��CIONodeManager��FEP����
		*			���߷ֿ�����ʱ	������øýӿ�
		*	\param	pIONodeManager [in]: FEPģ�����ݿⵥԪCIONodeManagerָ��
		*	\retval	��
		*/
		void SetIONodeManager(CIONodeManager* pIONodeManager);

		/*!
		*	\brief	��ȡFEPģ�����ݿⵥԪNodeManagerָ��
		*			RTDB��FEP��Ӽ�����ʱ��Ҫ��ȡ��ָ��
		*			RTDB��FEP�ϲ���һ��ʱ��NodeManager��FEP����
		*			���߷ֿ�����ʱ RTDBģ���Զ�����һ��Ĭ�ϵ�CIONodeManager
		*	\param	��
		*	\retval	CIONodeManager*	CIONodeManagerָ��
		*/
		CIONodeManager*	GetIONodeManager();
		
		/*!
		*	\brief	�жϵ��Ƿ�����ָ��������
		*							
		*	\param	std::string sPointNodeId[in]�����NodeId��
		*							
		*	\param	std::string sClassName[in]����������
		*							
		*	\param	bool bFindBase[in]���Ƿ���һ�������͡�
		*							
		*	\retval	���ý����0���ɹ���-1��ʧ�ܣ�-2����������
		*/
		int IsKindOfPointClass(std::string sPointNodeId,std::string sClassName,bool bFindBase = true);
		
		/*!
		*	\brief	��ȡ�������µ����е��ָ�����͵�ĸ���
		*							
		*	\param	std::string sTypeName[in]������������sTypeName=""ʱ����ȡ�������е�ĸ���
		*							
		*	\param	bool bChildType = true: �Ƿ����������
		*							
		*	\retval	�����
		*/
		virtual int GetPointNum(std::string sTypeName,bool bChildType = true);		
		/*!
		*	\brief	 ���ر��ع�������
		*/
		virtual int LoadProjectCfg();
	private:

		/*!
		*	\brief	����ServerConfig.xml�����õ������ռ䣬
		*			����Xml����������,����ӵ�m_pServer��
		*/
		bool createXmlParserModule();

		void DealWithConfigValues(const CKiwiVariant& nodes, const CKiwiVariant& nodevalues);
		void SetConfigLatestValue(const CKiwiVariant& nodes, const CKiwiVariant& nodevalues, bool bTimeCompare = false);
		
	private:
		std::string							m_sRunDir;

		//! �ڵ������
		CIONodeManager*                    m_pIONodeManager;

		std::map<std::string, void *> 		m_mapFunAndName;
	};
}

#endif
