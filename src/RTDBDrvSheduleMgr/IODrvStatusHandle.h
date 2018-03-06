/*!
 *	\file	IODrvStatusHandle.h
 *
 *	\brief	����״̬��Ϣ��ʱ���ص�������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��4��3��	11:40
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IODRVSTATUSHANDLE_H_
#define _IODRVSTATUSHANDLE_H_

#include "ace/Auto_Event.h"
#include "ace/Event_Handler.h"

namespace MACS_SCADA_SUD
{
	class CIODrvScheduleMgrBase;
	class CIODriverBase;

	/*!
	*	\class	CIODrvStatusHandle
	*
	*	\brief	StatusUnit��ʱ���Ļص���
	*
	*	���ڵ���������StatusUnit����
	*/
	class CIODrvStatusHandle : public ACE_Event_Handler
	{
	public:
		CIODrvStatusHandle( CIODriverBase* pDriverBase, CIODrvScheduleMgrBase* pImpl );
		~CIODrvStatusHandle(void);

		/*!
		*	\brief	 ��ʱ�ص�����
		*
		*	\param	tv	�ص��ú���ʱ�ľ���ʱ��
		*	\param	arg ����scheduleע��ص�ʱ����Ĳ���
		*
		*	\retval	int	0Ϊ�ɹ���-1Ϊʧ��
		*/
		virtual int handle_timeout (const ACE_Time_Value &tv, const void *arg);

	private:
		//!������ָ��
		CIODriverBase* m_pDriverBase;

		//! ��������ʵ�ֶ���ָ��
		CIODrvScheduleMgrBase* m_pImpl;

	};
}

#endif