/*!
*	\file	Server_SignalHandler.h
*
*	\brief	Server正常退出信号处理
*
*	在Unix/Linux下通过发送信号SIGTERM将使得RtdbManager正常退出
*
*	$Header: /MACS-SCADA_SUD/OORTDB_Design/Server/RtdbManager/Rtdb_SignalHandler.h 2     08-09-17 9:11 Litianhui $
*	$Author: Miaoweijie $
*	$Date: 08-09-17 9:11 $
*	$Revision: 2 $
*
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#ifndef SERVER_SIGNAL_HANDLER_H
#define SERVER_SIGNAL_HANDLER_H
#include "ace/OS.h"
#include "ace/Event.h"
#include "ace/Signal.h"

namespace MACS_SCADA_SUD
{
	const char STOP_SERVER[] = ACE_TEXT("/STOP_SERVER_RTDB");

	/*!
	 *	\note	ACE_Event在Unix/Linux下通过使用IPC机制模拟实现，与Windows不同的是，
	 *			即时拥有Event对象的所有进程已经全部终止，也不能确保Event对象自动删除。
	 *			为避免前次残留Event对象对系统下次运行的影响，应在系统下次运行前通过
	 *			脚本命令确保无残留Event对象，相关命令为ipcrm
	 *
	 *			如Event只在进程内部使用，可通过ACE_Event::remove()有效删除Event对象
	 */
	static inline void server_signal_handler (int signo)
	{
		ACE_TRACE (ACE_TEXT ("::server_signal_handler"));
		if (signo == SIGTERM)
		{
			//! 正常终止系统运行
			ACE_Event stopEvent( 1, 1, USYNC_PROCESS, STOP_SERVER );
			stopEvent.signal();
		}
	}

	/*!
	*	\brief	注册相关信号处理体
	*
	*	在应用程序初始化时调用register_rtdb_Handler
	*/
	inline void register_server_Handler()
	{
		//指定SIGTERM信号的处理函数
		ACE_OS::signal (SIGTERM, (ACE_SignalHandler) server_signal_handler);
	}
}

#endif //SERVER_SIGNAL_HANDLER_H