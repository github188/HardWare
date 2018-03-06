/*!
 *	\file	monitortask.h
 *
 *	\brief	监视线程头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author	yl
 *
 *	\date	2014年7月17日	11:11
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
 *	\brief	监视线程
 *
 *	详细的类说明（可选）
 */
class CMonitorTask : public ACE_Task_Base
{
public:
	CMonitorTask(){};
	virtual ~CMonitorTask(){};

	//! 线程体
	virtual int svc();
};
#endif // MONITORTASK_H_