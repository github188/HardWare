
/*!
*	\file	CIOTimer.h
*
*	\brief
*
*	\author miaoweijie
*
*	\date	2008年4月14日	9:30
*
*	\version	1.0
*
*	\attention	Copyright (c) 2002-2008, MACS-SCADA系统开发组,HollySys Co.,Ltd
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


typedef void(*OnIOTimer)(void*pParent);	//外面来的回调函数原型
typedef ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap> ActiveTimer;

namespace MACS_SCADA_SUD
{
	/*!
	*	\class	CIOTimer
	*
	*	\brief	定时器类，封装了启动和关闭一个定时器
	*
	*	本定时器是与修改系统时间无关的，即无论向后还是向前修改都能触发；
	*	一个定时器一个线程，防止阻塞。
	*/
	class IOTIMER_API CIOTimer :
		public ACE_Event_Handler
	{
	public:
		//! bOneShot: true为只触发一次, false为按时间间隔触发
		/*!
		*	\brief	定时器构造函数
		*
		*	\param	OnTimer：定时回调函数指针，类型定义
		*	typedef void(*OnIOTimer)(void*pParent)
		*	\param	pParent：传入定时回调函数的参数
		*	\param	bOneShot：是否触发一次，false则周期触发；true则只触发一次。
		*
		*	\retval
		*
		*	\note		注意事项（可选）
		*/
		CIOTimer( OnIOTimer OnTimer, void* pParent, bool bOneShot=false ,ActiveTimer* pTimerMgr=NULL);
		virtual ~CIOTimer(void);

	public:
		//! 全局定时器队列，只初始化一次
		ActiveTimer* m_pTimerMgr;

		//! 定时器的回调函数
		virtual int handle_timeout(const ACE_Time_Value& tv, const void* arg);
		/*!
		*	\brief	启动定时器
		*
		*	\param	nElapse：定时器时间间隔，单位毫秒
		*
		*	\retval	 0为成功；其它失败
		*
		*/
		int SetTimer( unsigned int nElapse);

		/*!
		*	\brief	关闭定时器
		*
		*	\retval	 0为成功；其它失败
		*
		*/
		int KillTimer();

		//! 获取定时器ID
		long GetTimerID()
		{
			return m_lTimerID;
		}

	private:
		//! 定时器ID
		long		m_lTimerID;
		//! 定时器回调函数
		OnIOTimer	m_OnTimer;
		//! 定时器父类指针
		void*		m_pParent;
		//! 定时器管理对象是否属于自己，如果不是则m_bOnlyHandler为true
		bool		m_bOnlyHandler;
		//! 是否只触发一次
		bool		m_bOneShot;
	};

}

#endif
