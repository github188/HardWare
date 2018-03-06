/*!
 *	\file	UADriverCommdef.h
 *
 *	\brief	UADriver���������ļ�
 *
 *	\author LIFAN
 *
 *	\date	2014��5��20��	15:49
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
	//!������Ϣ
// 	typedef struct SubscribeTetred
// 	{
// 		CSiteSubscription*				pSubscription;			//!���Ķ���ָ��
// 		SubscriptionSettings*			pSubscribSettings;		//!����������Ϣ
// 		UaMonitoredItemCreateRequests*	pItemRequests;			//!������Ӷ����б�ָ��
// 		UaMonitoredItemCreateResults*	pItemResults;			//!����Ӧ������б�ָ��
// 
// 	}SubscribeTetred;

	
}

#endif