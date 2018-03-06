/*!
 *	\file	monitortask.cpp
 *
 *	\brief	监视线程源文件
 *
 *	详细的文件说明（可选）
 *
 *	\author	yl
 *
 *	\date	2014年7月17日	11:12
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "monitortask.h"
#include "common.h"

#include "ace/Reactor.h"

/*!
 *	\brief	线程体
 *
 *	详细的函数说明（可选）
 *
 *	\param	无
 *
 *	\retval	执行事件循环结果
 *
 *	\note	注意事项（可选）
 */
int CMonitorTask::svc()
{
	int ret = 0;

	/* select_reactor 可能在阻塞过程中被信号中断（errno=4<EINTR>） */
	while(ACE_Reactor::instance()->reactor_event_loop_done() == 0)
	{
		ACE_Reactor::instance()->owner( ACE_OS::thr_self() );

		ret = ACE_Reactor::instance()->run_reactor_event_loop();
		if(ret >= 0)
		{
			break;
		}
	}

	return ret;
}