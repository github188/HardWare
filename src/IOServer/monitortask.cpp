/*!
 *	\file	monitortask.cpp
 *
 *	\brief	�����߳�Դ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author	yl
 *
 *	\date	2014��7��17��	11:12
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
 *	\brief	�߳���
 *
 *	��ϸ�ĺ���˵������ѡ��
 *
 *	\param	��
 *
 *	\retval	ִ���¼�ѭ�����
 *
 *	\note	ע�������ѡ��
 */
int CMonitorTask::svc()
{
	int ret = 0;

	/* select_reactor ���������������б��ź��жϣ�errno=4<EINTR>�� */
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