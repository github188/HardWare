/*!
 *	\file	IORemoteCtrl.h
 *
 *	\brief	ң�ؿ����ඨ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��9��	9:34
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOREMOTECTRL_H_
#define _IOREMOTECTRL_H_

#include "RTDBCommon/OS_Ext.h"
#include "ace/Message_Queue.h"
#include "ace/Synch_Traits.h"
#include "RTDBDriverFrame/IODriverH.h"

namespace MACS_SCADA_SUD
{
	//!����Ĭ�ϴ�СΪ16*1024��16K��
	typedef ACE_Message_Queue<ACE_MT_SYNCH> REMOTECTRL_QUEUE;

	/*!
	 *	\class	����
	 *
	 *	\brief	ACE��Ϣ���еķ�װ����Ĭ�ϴ�СΪ64*1024
	 *
	 *	Ϊ�����ȳ�����
	 */
	class  IODRIVERFRAME_API CIORemoteCtrl
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIORemoteCtrl(size_t nKCount = 64);

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIORemoteCtrl(void);

		/*!
		*	\brief	����Ϣ����������
		*
		*	\param	pBuf	:��Ϣ����
		*	\param	nSize	:��Ϣ���ݵĳ���
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		int Enqueue( char* pBuf, int nSize );

		/*!
		*	\brief	�Ӷ�����ȡ��һ����Ϣ
		*
		*	\param	pBuf	:��Ϣ����
		*	\param	nSize	:��Ϣ���ݵĳ���
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		int DeQueue( char* pBuf, int& nSize );

		/*!
		*	\brief	�����Ƿ���
		*
		*	\retval trueΪ����������falseΪ����δ��
		*/
		bool IsFull();

		/*!
		*	\brief	�����Ƿ�Ϊ��
		*
		*	\retval trueΪ����Ϊ�գ�falseΪ���зǿ�
		*/
		bool IsEmpty();

		/*!
		*	\brief	���ö��д�С
		*
		*	\param	nLimit	:���д�С
		*
		*	\retval void
		*/
		void SetQueueLimit(size_t nLimit);

	protected:
		//!����ָ��
		// REMOTECTRL_QUEUE* m_pQueue;
	};
}

#endif