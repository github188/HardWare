/*!
 *	\file	common_identifiers.h
 *
 *	\brief	公共的节点Id定义
 *	
 *
 *	\author lingling.li
 *
 *	\date	2014年9月15日	11:24
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef __COMMON_IDENTIFIERS_H__
#define __COMMON_IDENTIFIERS_H__

#include <string>
using namespace std;

#define FepId_GlobalMethodObject					"Method"
#define FepId_Method_SendCmd						"Method.SendCmd"
#define FepId_Method_SendForceCmd					"Method.SendForceCmd"
#define FepId_Method_ExcuteScript					"Method.ExcuteScript"

//! 定义事件产生方法节点
#define Event_Method_FireEvent						"Method.FireEvent"	
#define Event_Method_FireEventWithoutNode			"Method.FireEventWithoutNode"

//! 报警方法的identifier
#define Eas_Method_SuppressAlarm					"Method.SuppressAlarm"
#define Eas_Method_UnSuppressAlarm					"Method.UnSuppressAlarm"
#define Eas_Method_FireAlarm				        "Method.FireAlarm"


#define Server_Method_ExcuteGlobalFunc				"Method.ExcuteGlobalFunc"
#define Server_Method_GetCurrentRunScript			"Method_GetCurrentRunScript"
#define Server_Method_NotifyCmdExcuteInfo			"Method.NotifyCmdExcuteInfo"

#define Sql_Method_Exec								"Method.Exec_Sql"
#define Sql_Method_Query							"Method.Query_Sql"
#define Server_Method_GetPointList					"Method.GetPointList"
#define Server_Method_GetPointList_InputArguments	"Method.GetPointList.InputArguments"
#define Server_Method_GetPointList_OutputArguments	"Method.GetPointList.OutputArguments"
#define Server_Method_ChangeHistory					"Method.ChangeHistory"
#define Server_Method_ChangeHistory_InputArguments	"Method.ChangeHistory.InputArguments"
#define Server_Method_ChangeHistory_OutputArguments	"Method.ChangeHistory.OutputArguments"
#define Server_Method_GetChangedHistory				"Method.GetChangedHistory"
#define Server_Method_GetChangedHistory_InputArguments	"Method.GetChangedHistory.InputArguments"
#define Server_Method_GetChangedHistory_OutputArguments	"Method.GetChangedHistory.OutputArguments"
#define Server_Method_SyncConfig					"Method.SyncConfig"
#define Server_Method_SyncConfig_InputArguments		"Method.SyncConfig.InputArguments"
#define Server_Method_SyncConfig_OutputArguments	"Method.SyncConfig.OutputArguments"
#define Server_Method_ConfigDataSync				"Method.ConfigDataSync"
#define Server_Method_ConfigDataSync_InputArguments	"Method.ConfigDataSync.InputArguments"

//服务加载的下装数据的版本
#define Server_Method_GetLoadFileVersion			"Method.GetLoadFileVersion"

#define Server_Method_GetHistDirSpaceInfo			"Method.GetHistDirSpaceInfo"

const std::string DBNodeManager_Module = "DBNodeManager";

#endif // __COMMON_IDENTIFIERS_H__
