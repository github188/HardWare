/*!
 *	\file	IODrvStatusHandle.h
 *
 *	\brief	驱动状态信息定时监测回调处理器
 *
 *	详细的文件说明（可选）
 *
 *	\author LIFAN
 *
 *	\date	2014年4月3日	11:40
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
	*	\brief	StatusUnit定时器的回调类
	*
	*	周期调用驱动的StatusUnit函数
	*/
	class CIODrvStatusHandle : public ACE_Event_Handler
	{
	public:
		CIODrvStatusHandle( CIODriverBase* pDriverBase, CIODrvScheduleMgrBase* pImpl );
		~CIODrvStatusHandle(void);

		/*!
		*	\brief	 超时回调函数
		*
		*	\param	tv	回调该函数时的绝对时间
		*	\param	arg 调用schedule注册回调时传入的参数
		*
		*	\retval	int	0为成功，-1为失败
		*/
		virtual int handle_timeout (const ACE_Time_Value &tv, const void *arg);

	private:
		//!　驱动指针
		CIODriverBase* m_pDriverBase;

		//! 驱动调度实现对象指针
		CIODrvScheduleMgrBase* m_pImpl;

	};
}

#endif