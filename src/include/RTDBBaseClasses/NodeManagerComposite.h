/*!
*	\file	NodeManagerComposite.h
*
*	\brief	���ڵ�������ӿ���
*			����ڵ�������Ķ���ӿڣ�������ʵ����CNodeManagerCompositeImpl���
*	$Author: Wangyuanxing $
*	$Date: 14-04-01 9:09 $
*	$Revision: 1.0 $
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef __NODEMANAGERCOMPOSITE_H__
#define __NODEMANAGERCOMPOSITE_H__

#include "RTDBCommon/OS_Ext.h"
#include <map>
// #include "RTDBPlatformClass/ComponentType.h"

#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

namespace MACS_SCADA_SUD
{
	class CIONodeManager;
	/*!
	*	\class	CNodeManagerComposite
	*
	*	\brief	���ڵ�������ӿ���
	*			����ڵ�������Ķ���ӿڣ��ڽӿ��ڲ�����ʵ������Ӧ�ӿ���ɽӿڹ���
	*/
	class DBBASECLASSES_API CNodeManagerComposite
	{
	public:
		CNodeManagerComposite();
		virtual ~CNodeManagerComposite();

		/*!
		*	\brief	��ʼ��
		*			 ��ȡ�����ļ�,�ⲿ���������,֮�����StartWork��ʼ�ṩ����
		*
		*	\param	OpcServer*	[in]	Server����
		*	\retval	�����ɹ�����־
		*			0�ɹ�	����ʧ��
		*/
		virtual int Init() = 0;

		/*!
		*	\brief	ж��
		*			�ڴ���������,����ǰ�ȵ���StopWorkֹͣ����
		*	\param	��
		*	\retval	�����ɹ�����־
		*			0�ɹ�	����ʧ��
		*/
		virtual int UnInit() = 0;

		/*!
		*	\brief	��ʼ����
		*			��ַ�ռ��нڵ�Ĵ�����ʵ����
		*			���ڴ�͵�ַ�ռ��нڵ�������������,����ʱ���ܻ�ķѽϳ�ʱ��Ż������������״̬
		*	\param	��
		*	\retval	�����ɹ�����־
		*			0�ɹ�	����ʧ��
		*/
		virtual int StartWork() = 0;

		/*!
		*	\brief	ֹͣ����
		*			ͣ�̺߳ͷ���,���ٶ����ṩ OPC UA ��ط���
		*	\param	��
		*	\retval	�����ɹ�����־
		*			0�ɹ�	����ʧ��
		*/
		virtual int StopWork() = 0;

		//�����Ƿ��Ѿ�����
		virtual bool IsStarted() = 0;

		/*!
		*	\brief	�趨FEPģ�����ݿⵥԪCIONodeManagerָ��
		*			RTDB��FEP�ϲ���һ��ʱ��CIONodeManager��FEP����
		*			���߷ֿ�����ʱ	������øýӿ�
		*	\param	pIONodeManager [in]: FEPģ�����ݿⵥԪCIONodeManagerָ��
		*	\retval	��
		*/
		virtual void SetIONodeManager(CIONodeManager* pIONodeManager) = 0;

		/*!
		*	\brief	��ȡFEPģ�����ݿⵥԪNodeManagerָ��
		*			RTDB��FEP��Ӽ�����ʱ��Ҫ��ȡ��ָ��
		*			RTDB��FEP�ϲ���һ��ʱ��NodeManager��FEP����
		*			���߷ֿ�����ʱ RTDBģ���Զ�����һ��Ĭ�ϵ�CIONodeManager
		*	\param	��
		*	\retval	CIONodeManager*	CIONodeManagerָ��
		*/
		virtual CIONodeManager*	GetIONodeManager() = 0;
	
		/*!
		*	\brief	��ȡ�������µ����е��ָ�����͵�ĸ���
		*							
		*	\param	std::string sTypeName[in]������������sTypeName=""ʱ����ȡ�������е�ĸ���
		*							
		*	\param	bool bChildType = true: �Ƿ����������
		*							
		*	\retval	�����
		*/
		virtual int GetPointNum(std::string sTypeName,bool bChildType = true) = 0;	
		
		/*!
		*	\brief	���ع����ļ�
		*							
		*	\param	std::string sTypeName[in]������������sTypeName=""ʱ����ȡ�������е�ĸ���
		*							
		*	\param	bool bChildType = true: �Ƿ����������
		*							
		*	\retval	�����
		*/
		virtual int LoadProjectCfg(void) = 0;			
	};

	extern DBBASECLASSES_API CNodeManagerComposite* g_pNodeManagerCompsite;
}

#endif
