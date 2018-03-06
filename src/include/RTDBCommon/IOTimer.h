
/*!
*	\file	CIOTimer.h
*
*	\brief
*
*	\author miaoweijie
*
*	\date	2008��4��14��	9:30
*
*	\version	1.0
*
*	\attention	Copyright (c) 2002-2008, MACS-SCADAϵͳ������,HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef _IOTIMER_20080430_H_
#define _IOTIMER_20080430_H_

#include "OS_Ext.h"
#include "ace/Timer_Heap.h"
#include "ace/Timer_Queue_Adapters.h"

#ifdef DBCOMMON_EXPORTS
	#define IOTIMER_API MACS_DLL_EXPORT
#else
	#define IOTIMER_API MACS_DLL_IMPORT
#endif


typedef void(*OnIOTimer)(void*pParent);	//�������Ļص�����ԭ��
typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> ActiveTimer;

namespace MACS_SCADA_SUD
{
	/*!
	*	\class	CIOTimer
	*
	*	\brief	��ʱ���࣬��װ�������͹ر�һ����ʱ��
	*
	*	����ʱ�������޸�ϵͳʱ���޹صģ��������������ǰ�޸Ķ��ܴ�����
	*	һ����ʱ��һ���̣߳���ֹ������
	*/
	class IOTIMER_API CIOTimer :
		public ACE_Event_Handler
	{
	public:
		//! bOneShot: trueΪֻ����һ��, falseΪ��ʱ��������
		/*!
		*	\brief	��ʱ�����캯��
		*
		*	\param	OnTimer����ʱ�ص�����ָ�룬���Ͷ���
		*	typedef void(*OnIOTimer)(void*pParent)
		*	\param	pParent�����붨ʱ�ص������Ĳ���
		*	\param	bOneShot���Ƿ񴥷�һ�Σ�false�����ڴ�����true��ֻ����һ�Ρ�
		*
		*	\retval
		*
		*	\note		ע�������ѡ��
		*/
		CIOTimer( OnIOTimer OnTimer, void* pParent, bool bOneShot=false ,ActiveTimer* pTimerMgr=NULL);
		virtual ~CIOTimer(void);

	public:
		//! ȫ�ֶ�ʱ�����У�ֻ��ʼ��һ��
		ActiveTimer* m_pTimerMgr;

		//! ��ʱ���Ļص�����
		virtual int handle_timeout(const ACE_Time_Value& tv, const void* arg);
		/*!
		*	\brief	������ʱ��
		*
		*	\param	nElapse����ʱ��ʱ��������λ����
		*
		*	\retval	 0Ϊ�ɹ�������ʧ��
		*
		*/
		int SetTimer( unsigned int nElapse);

		/*!
		*	\brief	�رն�ʱ��
		*
		*	\retval	 0Ϊ�ɹ�������ʧ��
		*
		*/
		int KillTimer();

		//! ��ȡ��ʱ��ID
		long GetTimerID()
		{
			return m_lTimerID;
		}

	private:
		//! ��ʱ��ID
		long		m_lTimerID;
		//! ��ʱ���ص�����
		OnIOTimer	m_OnTimer;
		//! ��ʱ������ָ��
		void*		m_pParent;
		//! ��ʱ����������Ƿ������Լ������������m_bOnlyHandlerΪtrue
		bool		m_bOnlyHandler;
		//! �Ƿ�ֻ����һ��
		bool		m_bOneShot;
	};

}

#endif
