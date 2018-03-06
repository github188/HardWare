/*!
 *	\file	EasRunStatusBase.h
 *
 *	\brief	��������״̬�仯�ӿ���
 *
 *	\author	wangzan
 *
 *	\date	2014-07-24
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014-2024, SCADA V4ƽ̨������,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _EAS_RUNSTATUS_BASE_H_
#define _EAS_RUNSTATUS_BASE_H_

#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/KiwiVariant.h"

#ifdef DBBASECLASSES_EXPORTS
#define EAS_CALL_BACK_API MACS_DLL_EXPORT
#else
#define EAS_CALL_BACK_API MACS_DLL_IMPORT
#endif

namespace MACS_SCADA_SUD
{
	class CEasCommand;

	/*!
	 *	\class	CEasRunStatusBase
	 *
	 *	\brief  �������������Ӧ���ݿⵥԪ״̬�仯�������ɫ��½����ȡ�������
	 *
	 *  \des    ��������״̬�仯�ӿ���
	 *
	 *  \comment  
	 */
	class EAS_CALL_BACK_API CEasRunStatusBase
	{

	public:
		/*!
		*	\brief	����/��������
		*
		*	\param	��
		*
		*	\retval	��
		*
		*	\note	
		*/
		CEasRunStatusBase();
		virtual ~CEasRunStatusBase();

	public:

		/*!
		*	\brief	���ݿⵥԪ״̬�仯֪ͨ�ӿ�
		*
		*	\param	namespaceIndex : ���ݿⵥԪ��
		*	\param  isMaster : ��ǰ������״̬
		*
		*	\retval	0�ɹ���-1ʧ��
		*
		*	\note	
		*/
		virtual int OnDBUnitStatusChange(unsigned short namespaceIndex, bool isMaster) = 0;

		/*!
		*	\brief	��ɫ��½�ӿڣ�������Ӧ���½�ɫ��½��ϵͳ��Ĵ���
		*
		*	\param	roleName : ��ɫ��
		*
		*	\retval	0�ɹ���-1ʧ��
		*
		*	\note	
		*/
		virtual int OnRoleLogin(std::string roleName) = 0;

		/*!
		*	\brief	��ɫ�˳���ɾ���ӿ�
		*
		*	\param	roleName : ��ɫ��
		*
		*	\retval	0�ɹ���-1ʧ��
		*
		*	\note	
		*/
		virtual int OnRoleLogout(std::string roleName) = 0;

		/*!
		*	\brief	��ȡ�����������Ϣ
		*
		*	\param	eventId : �¼�id
		*
		*	\retval	���������
		*
		*	\note	
		*/
		virtual unsigned int GetCategory(CKiwiVariant eventId) = 0;

		/*!
		*	\brief	��������ת��ʧ�ܴ���
		*
		*	\param	userName(in) : �û���
		*	\param	roleName(in) : ��ɫ��
		*	\param	request(in)  : �������
		*	\param	siteName(in) : վ����
		*	\param	cmdType(in)	 : ��������,ȡֵ��Դ��EasCommand.h�ļ��е�EasCommandTypeö��
		*
		*	\retval	0�ɹ�������ʧ��
		*
		*	\note	
		*/
		// virtual int BackupEasCmd(std::string userName, std::string roleName, 
		// 	const void& request, std::string siteName, int cmdType) = 0;

		/*!
		*	\brief	�������±���/ִ���ظ�������ʱ����Ҫ��֮ǰ��ʱ������ɾ��
		*
		*	\param	conditionNode(in)	: ������nodeid
		*	\param	cmdType(in)			: ��������
		*
		*	\retval	0�ɹ�������ʧ��
		*
		*/
		virtual int RemoveEasCmd(CKiwiVariant conditionNode, int cmdType) = 0;

		/*!
		*	\brief	����Դ�����쳣����
		*			���ÿ��ʵʱ�ⵥԪ, ����Ϊ����ʱ, ���й����������ֱ������Դ���������жϹ���, 
		*			�ڻָ�����ʱ��Ҫ�Ը�ʵʱ�ⵥԪ�ı�����һ�ζ������
		*
		*	\param	��
		*
		*	\retval	0�ɹ�������ʧ��
		*
		*	\note	
		*/
		virtual int OnSourceSiteConnectException() = 0;
	};

	extern  EAS_CALL_BACK_API CEasRunStatusBase* g_pEasRunStatus;     //!����״̬�仯��Ӧ�ӿ�ָ��
}

#endif
