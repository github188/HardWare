/*!
*	\file	Server_SignalHandler.h
*
*	\brief	Server�����˳��źŴ���
*
*	��Unix/Linux��ͨ�������ź�SIGTERM��ʹ��RtdbManager�����˳�
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
	 *	\note	ACE_Event��Unix/Linux��ͨ��ʹ��IPC����ģ��ʵ�֣���Windows��ͬ���ǣ�
	 *			��ʱӵ��Event��������н����Ѿ�ȫ����ֹ��Ҳ����ȷ��Event�����Զ�ɾ����
	 *			Ϊ����ǰ�β���Event�����ϵͳ�´����е�Ӱ�죬Ӧ��ϵͳ�´�����ǰͨ��
	 *			�ű�����ȷ���޲���Event�����������Ϊipcrm
	 *
	 *			��Eventֻ�ڽ����ڲ�ʹ�ã���ͨ��ACE_Event::remove()��Чɾ��Event����
	 */
	static inline void server_signal_handler (int signo)
	{
		ACE_TRACE (ACE_TEXT ("::server_signal_handler"));
		if (signo == SIGTERM)
		{
			//! ������ֹϵͳ����
			ACE_Event stopEvent( 1, 1, USYNC_PROCESS, STOP_SERVER );
			stopEvent.signal();
		}
	}

	/*!
	*	\brief	ע������źŴ�����
	*
	*	��Ӧ�ó����ʼ��ʱ����register_rtdb_Handler
	*/
	inline void register_server_Handler()
	{
		//ָ��SIGTERM�źŵĴ�����
		ACE_OS::signal (SIGTERM, (ACE_SignalHandler) server_signal_handler);
	}
}

#endif //SERVER_SIGNAL_HANDLER_H