/*!
 *	\file	UADriverCommdef.h
 *
 *	\brief	UADriver公共定义文件
 *
 *	\author LIFAN
 *
 *	\date	2014年5月20日	15:49
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _UADRIVERCOMMDEF_H_
#define _UADRIVERCOMMDEF_H_

#include "RTDB/Server/DBDriverFrame/IODriverH.h"

#ifdef FEPOPCUADRIVER_EXPORTS
#define OPCUADRIVER_API MACS_DLL_EXPORT
#else
#define OPCUADRIVER_API MACS_DLL_IMPORT
#endif

namespace MACS_SCADA_SUD
{
	//!订阅信息
// 	typedef struct SubscribeTetred
// 	{
// 		CSiteSubscription*				pSubscription;			//!订阅对象指针
// 		SubscriptionSettings*			pSubscribSettings;		//!订阅配置信息
// 		UaMonitoredItemCreateRequests*	pItemRequests;			//!请求监视对象列表指针
// 		UaMonitoredItemCreateResults*	pItemResults;			//!监视应答对象列表指针
// 
// 	}SubscribeTetred;

	
}

#endif