/*!
 *	\file	monitortask.h
 *
 *	\brief	�����߳�ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author	yl
 *
 *	\date	2014��7��17��	11:11
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef MONITORTASK_H_
#define MONITORTASK_H_

#include "ace/Task.h"

using namespace std;

/*!
 *	\class	CMonitorTask
 *
 *	\brief	�����߳�
 *
 *	��ϸ����˵������ѡ��
 */
class CMonitorTask : public ACE_Task_Base
{
public:
	CMonitorTask(){};
	virtual ~CMonitorTask(){};

	//! �߳���
	virtual int svc();
};
#endif // MONITORTASK_H_