/*!
*	\file	GeneralConfig.h
*
*	\brief	基础类库中常用的自定义对象类型和配置声明
*
*
*	$Author: Wangyuanxing $
*	$Date: 14-04-01 9:09 $
*	$Revision: 1.0 $
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef __GENERALConfigType_H__
#define __GENERALConfigType_H__


#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/KiwiVariant.h"

#ifdef DBPLATFORMCLASS_EXPORTS
#define DBPLATFORM_DECL MACS_DLL_EXPORT
#else
#define DBPLATFORM_DECL MACS_DLL_IMPORT
#endif

//! 数值合并延时，单位ms
const int DI_VALUEMERGE_DELAYTIME = 50;
const int VALUEMERGE_THREAD_NUM = 50;
//! 反馈超时判定间隔时间
const int FeedBack_Interval_Time = 50;
//! 报警恢复时间死区判定间隔时间
const int TimeZone_Interval_Time = 200;
//! 报警恢复时间死区判定次数
const short TimeZone_Count = 5;

//! 报警恢复时间死区高限值2分钟
const int MAX_TimeZone = 120000;

const int Operate_TimeOut = 1000;

const std::string DBPlatformClass_Module = "DBPlatformClass";

namespace MACS_SCADA_SUD{

#define OpcUa_ScriptResultUnKnown	0x80B7FFFA

/************************************************************
 报警统计事件类别
*************************************************************/
typedef enum EventIdentifier
{
	AlarmSynth = 0,
	UnAckAlarmSynth,
	ForcedSynth,
	InvalidSynth,
	SuspiciousSynth,
	OneDayNumAlmSynth,
	CurNumEQAlarmSynth
}EventIdentifier;

typedef enum LimitAlarmState
{
	NoAlarmState = 0,		//! 不需产生报警
	RecoveryState,
	LowLowState,
	LowState,
	HighState,
	HighHighState,
	OutofHighState,
	OutofLowState
}LimitAlarmState;

typedef struct writeInfo
{
	long		nodeId;
	unsigned int    AttributeId;
	std::string	    indexRange;
	CKiwiVariant		value;
	short statusCode;
}writeInfo;

/************************************************************
 Udp变量类别
*************************************************************/
const short DEF_UdpType_UnKnown = -1;
const short DEF_UdpType_Calculate = 0;
const short DEF_UdpType_Calculate_Keep = 1;
const short DEF_UdpType_Read_Only = 2;

/************************************************************
 平台报警类型
*************************************************************/
const unsigned int DEF_AlarmType_Level = 6022;
const unsigned int DEF_AlarmType_Deviation = 6023;
const unsigned int DEF_AlarmType_RateOfChange = 6024;
const unsigned int DEF_AlarmType_DiscreteCfg = 6025;
const unsigned int DEF_AlarmType_CtrlFailure = 6026;

/************************************************************
 平台特殊规格
*************************************************************/
const unsigned int DEF_Max_Alarm_Severity = 999;

/************************************************************
 操作名
*************************************************************/
#define DEF_Opt_ReadOnly		"ReadOnly"
#define DEF_Opt_Write			"Write"
#define DEF_Opt_Control			"Control"
#define DEF_Opt_ModifyCfgItem	"ModifyCfgItem"
#define DEF_Opt_PointForce		"PointForce"
#define DEF_Opt_PointDisable	"PointDisable"
#define DEF_Opt_ReadData		"ReadData"
#define DEF_Opt_UpdateData		"UpdateData"
#define DEF_Opt_AlarmMonitor	"AlarmMonitor"
#define DEF_Opt_AlarmOperate	"AlarmOperate"

/************************************************************
 FieldPoint Type Node name declaration
*************************************************************/
#define DEF_Channel										"Channel"
#define DEF_Channel_In									"In_Channel"
#define DEF_Channel_Out									"Out_Channel"
#define DEF_Channel_Select_In							"SelectInChannel"
#define DEF_Channel_Select_Out							"SelectOutChannel"
#define DEF_Channel_Ctrl_Reply							"CtrlReplyChannel"

typedef enum ChannelType
{
	UnKnownChannelType,
	InChannelType,
	OutChannelType,
	SelectInChannelType,
	SelectOutChannelType,
	CtrlReplyChannelType
}ChannelType;

/************************************************************
 DiscreteConfig Type and its instance declaration
*************************************************************/
#define DEF_Discrete_ConfigType							"DiscreteConfig"
#define DEF_Discrete_ConfigType_DiscreteType			"DiscreteConfig_DiscreteType"
#define DEF_Discrete_ConfigType_AlarmMode 				"DiscreteConfig_AlarmMode"
#define DEF_Discrete_ConfigType_DebounceTime 			"DiscreteConfig_DebounceTime"
#define DEF_Discrete_ConfigType_HistoryPeriod 			"DiscreteConfig_HistoryPeriod"
#define DEF_Discrete_ConfigType_NeedSOE 				"DiscreteConfig_NeedSOE"
#define DEF_Discrete_ConfigType_NeedConverse 			"DiscreteConfig_NeedConverse"
#define DEF_Discrete_ConfigType_NeedStatistics 			"DiscreteConfig_NeedStatistics"
#define DEF_Discrete_ConfigType_NeedCmdHistory 			"DiscreteConfig_NeedCmdHistory"
#define DEF_Discrete_ConfigType_InputChannelNum 		"DiscreteConfig_InputChannelNum"
#define DEF_Discrete_ConfigType_OutputChannelNum 		"DiscreteConfig_OutputChannelNum"
/************************************************************/


/************************************************************
 AnalogConfig Type and its instance declaration
*************************************************************/
#define DEF_AnalogConfigType                                "AnalogConfig"
#define DEF_AnalogConfigType_RangSwtMode                    "AnalogConfig_RangSwtMode"
#define DEF_AnalogConfigType_AlarmModel                     "AnalogConfig_AlarmModel"
#define DEF_AnalogConfigType_DeadZonePercentage             "AnalogConfig_DeadZonePercentage"
#define DEF_AnalogConfigType_NearlyzeroDeadZone             "AnalogConfig_NearlyzeroDeadZone"
#define DEF_AnalogConfigType_HisValueForcePeriod            "AnalogConfig_HisValueForcePeriod"
#define DEF_AnalogConfigType_CompressAccuracy               "AnalogConfig_CompressAccuracy"
#define DEF_AnalogConfigType_InputChannelNum                "AnalogConfig_InputChannelNum"
#define DEF_AnalogConfigType_OutputChannelNum               "AnalogConfig_OutputChannelNum"
#define DEF_AnalogConfigType_ValueInRange					"AnalogConfig_ValueInRange"

#define DEF_AnalogConfig_RangSwtMode						"RangSwtMode"
#define DEF_AnalogConfig_AlarmModel							"AlarmModel"
#define DEF_AnalogConfig_DeadZonePercentage					"DeadZonePercentage"
#define DEF_AnalogConfig_NearlyzeroDeadZone					"NearlyzeroDeadZone"
#define DEF_AnalogConfig_HisValueForcePeriod				"HisValueForcePeriod"
#define DEF_AnalogConfig_CompressAccuracy					"CompressAccuracy"
#define DEF_AnalogConfig_InputChannelNum					"InputChannelNum"
#define DEF_AnalogConfig_OutputChannelNum					"OutputChannelNum"
#define DEF_AnalogConfig_ValueInRange						"ValueInRange"
/************************************************************/


/************************************************************
 Alarm Type and its instance declaration
*************************************************************/
#define  DEF_LevelAlarm										"ExclusiveLevelAlarm"
#define	 DEF_DeviationAlarm									"ExclusiveDeviationAlarm"
#define  DEF_RocAlarm										"ExclusiveRateOfChangeAlarm"
#define	 DEF_CtrlFailureAlarm								"CtrlFailureAlarm"
#define  DEF_DiscreteAlarm									"DiscreteAlarm"
/************************************************************/


/************************************************************
 LevelAlarmConfig Type and its instance declaration
*************************************************************/
#define DEF_LevelAlarmConfigType                                "LevelAlarmConfig"
#define DEF_LevelAlarmConfigType_RestoreZone                    "LevelAlarmConfig_RestoreZone"
#define DEF_LevelAlarmConfigType_TimeZone                       "LevelAlarmConfig_TimeZone"

#define DEF_LevelAlarmConfigType_LimitValue						"LevelAlarmConfig_LimitValue"
#define DEF_LevelAlarmConfigType_Severity						"LevelAlarmConfig_Severity"
#define DEF_LevelAlarmConfigType_Message						"LevelAlarmConfig_Message"

#define DEF_LevelAlarmConfig_RestoreZone						"RestoreZone"
#define DEF_LevelAlarmConfig_TimeZone							"TimeZone"

#define DEF_LevelAlarmConfig_LimitValues						"LimitValues"
#define DEF_LevelAlarmConfig_Severities							"Severities"
#define DEF_LevelAlarmConfig_Messages							"Messages"
/************************************************************/



/************************************************************
 DeviationAlarmConfig Type and its instance declaration
*************************************************************/
#define DEF_DeviationAlarmConfigType                                "DeviationAlarmConfig"
#define DEF_DeviationAlarmConfigType_PrimaryDiffZone                   "DeviationAlarmConfig_PrimaryDiffZone"
#define DEF_DeviationAlarmConfigType_SecondaryDiffZone                    "DeviationAlarmConfig_SecondaryDiffZone"
#define DEF_DeviationAlarmConfigType_HighHighSeverity                   "DeviationAlarmConfig_HighHighSeverity"
#define DEF_DeviationAlarmConfigType_HighSeverity                    "DeviationAlarmConfig_HighSeverity"
#define DEF_DeviationAlarmConfigType_LowSeverity                    "DeviationAlarmConfig_LowSeverity"
#define DEF_DeviationAlarmConfigType_LowLowSeverity                   "DeviationAlarmConfig_LowLowSeverity"
#define DEF_DeviationAlarmConfigType_HighHighMessage                     "DeviationAlarmConfig_HighHighMessage"
#define DEF_DeviationAlarmConfigType_HighMessage                      "DeviationAlarmConfig_HighMessage"
#define DEF_DeviationAlarmConfigType_LowMessage                      "DeviationAlarmConfig_LowMessage"
#define DEF_DeviationAlarmConfigType_LowLowMessage                     "DeviationAlarmConfig_LowLowMessage"
#define DEF_DeviationAlarmConfigType_Target							"DeviationAlarmConfig_Target"
#define DEF_DeviationAlarmConfigType_RestoreZone                    "DeviationAlarmConfig_RestoreZone"
#define DEF_DeviationAlarmConfigType_TimeZone                       "DeviationAlarmConfig_TimeZone"

#define DEF_DeviationAlarmConfig_PrimaryDiffZone					"PrimaryDiffZone"
#define DEF_DeviationAlarmConfig_SecondaryDiffZone					"SecondaryDiffZone"
#define DEF_DeviationAlarmConfig_HighHighSeverity                   "HighHighSeverity"
#define DEF_DeviationAlarmConfig_HighSeverity						"HighSeverity"
#define DEF_DeviationAlarmConfig_LowSeverity						"LowSeverity"
#define DEF_DeviationAlarmConfig_LowLowSeverity						"LowLowSeverity"
#define DEF_DeviationAlarmConfig_HighHighMessage                     "HighHighMessage"
#define DEF_DeviationAlarmConfig_HighMessage						"HighMessage"
#define DEF_DeviationAlarmConfig_LowMessage							"LowMessage"
#define DEF_DeviationAlarmConfig_LowLowMessage						"LowLowMessage"
#define DEF_DeviationAlarmConfig_Target								"Target"
#define DEF_DeviationAlarmConfig_RestoreZone						"RestoreZone"
#define DEF_DeviationAlarmConfig_TimeZone							"TimeZone"

/************************************************************/



/************************************************************
 RocAlarmConfig Type and its instance declaration
*************************************************************/
#define DEF_RocAlarmConfigType                                "RocAlarmConfig"
#define DEF_RocAlarmConfigType_LimitValues                    "RocAlarmConfig_LimitValues"
#define DEF_RocAlarmConfigType_Severities                    "RocAlarmConfig_LowSeverities"
#define DEF_RocAlarmConfigType_Messages                      "RocAlarmConfig_Messages"
#define DEF_RocAlarmConfigType_ChangeInterval							"RocAlarmConfig_ChangeInterval"
#define DEF_RocAlarmConfigType_CaculateInterval                    "RocAlarmConfig_CaculateInterval"

#define DEF_RocAlarmConfig_LimitValues						"LimitValues"
#define DEF_RocAlarmConfig_Severities						"Severities"
#define DEF_RocAlarmConfig_Messages							"Messages"
#define DEF_RocAlarmConfig_ChangeInterval					"ChangeInterval"
#define DEF_RocAlarmConfig_CaculateInterval                 "CaculateInterval"
/************************************************************/


/************************************************************
 FieldPoint Type and its instance declaration
*************************************************************/
#define  DEF_FieldPointType									"FieldPoint"
#define DEF_FieldPointType_DeviceAddress					"FieldPoint_DeviceAddress"
#define DEF_FieldPointType_Offset							"FieldPoint_Offset"
#define DEF_FieldPointType_RawEngRange							"FieldPoint_RawEngRange"
#define DEF_FieldPointType_NeedSOE								"FieldPoint_NeedSOE"
#define DEF_FieldPointType_Values								"FieldPoint_Values"
#define DEF_FieldPointType_ValueDeses							"FieldPoint_ValueDeses"

#define DEF_FieldPoint_DeviceAddress						"DeviceAddress"
#define DEF_FieldPoint_Offset								"Offset"
#define DEF_FieldPoint_NeedConverse							"NeedConverse"
#define DEF_FieldPoint_RawEngRange							"RawEngRange"
#define DEF_FieldPoint_RangSwtMode							"RangSwtMode"
#define DEF_FieldPoint_NeedSOE								"NeedSOE"
#define DEF_FieldPoint_Values								"Values"
#define DEF_FieldPoint_ValueDeses							"ValueDeses"
/************************************************************/



/************************************************************
 Originator Type and its instance declaration
*************************************************************/
#define  DEF_OriginatorType									"Originator"
#define  DEF_OriginatorType_orCat							"Originator_orCat"
#define  DEF_OriginatorType_orIdent							"Originator_orIdent"
/************************************************************/




/************************************************************
 RegConfigParams Type and its instance declaration
*************************************************************/
#define  DEF_RegConfigParamsType									"RegConfigParams"
#define  DEF_RegConfigParamsType_CtrlModel							"RegConfigParams_CtrlModel"
#define  DEF_RegConfigParamsType_SBOOperateTimeout					"RegConfigParams_SBOOperateTimeout"
#define  DEF_RegConfigParamsType_OperateCount						"RegConfigParams_OperateCount"
#define  DEF_RegConfigParamsType_FailAlarm							"RegConfigParams_FailAlarm"
#define  DEF_RegConfigParamsType_CreateAlarm						"RegConfigParams_CreateAlarm"
#define  DEF_RegConfigParamsType_StepSize							"RegConfigParams_StepSize"

#define  DEF_RegConfigParams_CtrlModel							"CtrlModel"
#define  DEF_RegConfigParams_SBOOperateTimeout					"SBOOperateTimeout"
#define  DEF_RegConfigParams_OperateCount						"OperateCount"
#define  DEF_RegConfigParams_FailAlarm							"FailAlarm"
#define  DEF_RegConfigParams_CreateAlarm						"CreateAlarm"
#define  DEF_RegConfigParams_StepSize							"StepSize"
/************************************************************/




/************************************************************
 CtrlProcessInfo Type and its instance declaration
*************************************************************/
#define  DEF_CtrlProcessInfoType									"CtrlProcessInfo"
#define  DEF_CtrlProcessInfoType_OperateTime						"CtrlProcessInfo_OperateTime"
#define  DEF_CtrlProcessInfoType_Originator							"CtrlProcessInfo_Originator"
#define  DEF_CtrlProcessInfoType_Originator_orCat					"CtrlProcessInfo_Originator_orCat"
#define  DEF_CtrlProcessInfoType_Originator_orIdent					"CtrlProcessInfo_Originator_orIdent"
#define  DEF_CtrlProcessInfoType_OperateCount						"CtrlProcessInfo_Count"
#define  DEF_CtrlProcessInfoType_OperateStateMachine				"CtrlProcessInfo_OperateStateMachine"

#define  DEF_CtrlProcessInfo_OperateTime							"OperateTime"
#define  DEF_CtrlProcessInfo_OperateCount							"OperateCount"
#define  DEF_CtrlProcessInfo_OperateStateMachine					"OperateStateMachine"
/************************************************************/




/************************************************************
 PulseConfig Type and its instance declaration
*************************************************************/
#define  DEF_PulseConfigType									"PulseConfig"
#define  DEF_PulseConfigType_CmdQual							"PulseConfig_CmdQual"
#define  DEF_PulseConfigType_OnDur								"PulseConfig_OnDur"
#define  DEF_PulseConfigType_OffDur								"PulseConfig_OffDur"
#define  DEF_PulseConfigType_NumPls								"PulseConfig_NumPls"

#define  DEF_PulseConfig_CmdQual							"CmdQual"
#define  DEF_PulseConfig_OnDur								"OnDur"
#define  DEF_PulseConfig_OffDur								"OffDur"
#define  DEF_PulseConfig_NumPls								"NumPls"
/************************************************************/




/************************************************************
 DiscreteConfig Type and its instance declaration
*************************************************************/
#define DEF_DiscreteConfigType								"DiscreteConfig"
#define DEF_DiscreteConfigType_DiscreteType					"DiscreteConfig_DiscreteType"
#define DEF_DiscreteConfigType_AlarmModel					"DiscreteConfig_AlarmModel"
#define DEF_DiscreteConfigType_DebounceTime					"DiscreteConfig_DebounceTime"
#define DEF_DiscreteConfigType_HistoryPeriod				"DiscreteConfig_HistoryPeriod"
#define DEF_DiscreteConfigType_NeedConverse					"DiscreteConfig_NeedConverse"
#define DEF_DiscreteConfigType_NeedStatistics				"DiscreteConfig_NeedStatistics"
#define DEF_DiscreteConfigType_NeedCmdHistory				"DiscreteConfig_NeedCmdHistory"
#define DEF_DiscreteConfigType_Values						"DiscreteConfig_Values"
#define DEF_DiscreteConfigType_ValueDeses					"DiscreteConfig_ValueDeses"
#define DEF_DiscreteConfigType_ValueMerge					"DiscreteConfig_ValueMerge"
#define DEF_DiscreteConfigType_InputChannelNum				"DiscreteConfig_InputChannelNum"
#define DEF_DiscreteConfigType_OutputChannelNum				"DiscreteConfig_OutputChannelNum"

#define DEF_DiscreteConfig_DiscreteType					"DiscreteType"
#define DEF_DiscreteConfig_AlarmModel					"AlarmModel"
#define DEF_DiscreteConfig_DebounceTime					"DebounceTime"
#define DEF_DiscreteConfig_HistoryPeriod				"HistoryPeriod"
#define DEF_DiscreteConfig_NeedConverse					"NeedConverse"
#define DEF_DiscreteConfig_NeedStatistics				"NeedStatistics"
#define DEF_DiscreteConfig_NeedCmdHistory				"NeedCmdHistory"
#define DEF_DiscreteConfig_Values						"Values"
#define DEF_DiscreteConfig_ValueDeses					"ValueDeses"
#define DEF_DiscreteConfig_ValueMerge					"ValueMerge"
#define DEF_DiscreteConfig_InputChannelNum				"InputChannelNum"
#define DEF_DiscreteConfig_OutputChannelNum				"OutputChannelNum"


/************************************************************
 DiscreteAlarmConfig Type and its instance declaration
*************************************************************/
#define DEF_DiscreteAlarmConfigType							"DiscreteAlarmConfig"
#define DEF_DiscreteAlarmConfigType_ActiveState				"DiscreteAlarmConfig_ActiveState"
#define DEF_DiscreteAlarmConfigType_AlmMessage				"DiscreteAlarmConfig_AlmMessage"
#define DEF_DiscreteAlarmConfigType_AlmLevel				"DiscreteAlarmConfig_AlmLevel"
#define DEF_DiscreteAlarmConfigType_TimeZone				"DiscreteAlarmConfig_TimeZone"
#define DEF_DiscreteAlarmConfigType_AlmType					"DiscreteAlarmConfig_AlmType"

#define DEF_DiscreteAlarmConfig_HasEvent				"HasEvent"
#define DEF_DiscreteAlarmConfig_ActiveState				"ActiveState"
#define DEF_DiscreteAlarmConfig_AlmMessage				"AlmMessage"
#define DEF_DiscreteAlarmConfig_AlmLevel				"AlmLevel"
#define DEF_DiscreteAlarmConfig_DelayTime				"DelayTime"
#define DEF_DiscreteAlarmConfig_TimeZone				"TimeZone"
#define DEF_DiscreteAlarmConfig_AlmType					"AlmType"
/************************************************************/




/************************************************************
 CtrlConfigParams Type and its instance declaration
*************************************************************/
#define  DEF_CtrlConfigParamsType									"CtrlConfigParams"
#define  DEF_CtrlConfigParamsType_CtrlModel							"CtrlConfigParams_CtrlModel"
#define  DEF_CtrlConfigParamsType_PulseConfig						"CtrlConfigParams_PulseConfig"
#define  DEF_CtrlConfigParamsType_PulseConfig_CmdQual				"CtrlConfigParams_PulseConfig_CmdQual"
#define  DEF_CtrlConfigParamsType_PulseConfig_OnDur					"CtrlConfigParams_PulseConfig_OnDur"
#define  DEF_CtrlConfigParamsType_PulseConfig_OffDur				"CtrlConfigParams_PulseConfig_OffDur"
#define  DEF_CtrlConfigParamsType_PulseConfig_NumPls				"CtrlConfigParams_PulseConfig_NumPls"
#define  DEF_CtrlConfigParamsType_SBOSelectTimeout					"CtrlConfigParams_SBOSelectTimeout"
#define  DEF_CtrlConfigParamsType_SBOOperateTimeout					"CtrlConfigParams_SBOOperateTimeout"
#define  DEF_CtrlConfigParamsType_OperateCount						"CtrlConfigParams_OperateCount"
#define  DEF_CtrlConfigParamsType_CtrlTarget						"CtrlConfigParams_CtrlTarget"
#define  DEF_CtrlConfigParamsType_FailAlarm							"CtrlConfigParams_FailAlarm"
#define  DEF_CtrlConfigParamsType_CreateAlarm						"CtrlConfigParams_CreateAlarm"
#define  DEF_CtrlConfigParamsType_SBOClass							"CtrlConfigParams_SBOClass"

#define  DEF_CtrlConfigParams_CtrlModel							"CtrlModel"
#define  DEF_CtrlConfigParams_SBOSelectTimeout					"SBOSelectTimeout"
#define  DEF_CtrlConfigParams_SBOOperateTimeout					"SBOOperateTimeout"
#define  DEF_CtrlConfigParams_OperateCount						"OperateCount"
#define  DEF_CtrlConfigParams_CtrlTarget						"CtrlTarget"
#define  DEF_CtrlConfigParams_FailAlarm							"FailAlarm"
#define  DEF_CtrlConfigParams_CreateAlarm						"CreateAlarm"
#define  DEF_CtrlConfigParams_SBOClass							"SBOClass"
/************************************************************/




/************************************************************
 Component Type and its instance declaration
*************************************************************/
#define  DEF_ComponentType									"Component"
#define  DEF_ComponentType_SetItem							"Component_SetItem"
#define  DEF_ComponentType_SetItem_InputArguments			"Component_SetItem_InputArguments"

#define DEF_ValueDesp										"ValueDesp"
/************************************************************/




/************************************************************
 EQAlarmSynth Type and its instance declaration
*************************************************************/
#define  DEF_EQAlarmSynthType									"EQAlarmSynth"
#define  DEF_EQAlarmSynthType_AlarmSynthesis					"EQAlarmSynth_AlarmSynthesis"
#define  DEF_EQAlarmSynthType_UnAckAlarmSynth					"EQAlarmSynth_UnAckAlarmSynth"
#define  DEF_EQAlarmSynthType_ForcedSynthesis					"EQAlarmSynth_ForcedSynthesis"
#define  DEF_EQAlarmSynthType_DisableSynthesis					"EQAlarmSynth_DisableSynthesis"
#define  DEF_EQAlarmSynthType_LimitAlarmDisableSynthesis		"EQAlarmSynth_LimitAlarmDisableSynthesis"
#define  DEF_EQAlarmSynthType_DevAlarmDisableSynthesis			"EQAlarmSynth_DevAlarmDisableSynthesis"
#define  DEF_EQAlarmSynthType_RocAlarmDisableSynthesis			"EQAlarmSynth_RocAlarmDisableSynthesis"
#define  DEF_EQAlarmSynthType_DIAlarmDisableSynthesis			"EQAlarmSynth_DIAlarmDisableSynthesis"
#define  DEF_EQAlarmSynthType_InvalidSynthesis					"EQAlarmSynth_InvalidSynthesis"
#define  DEF_EQAlarmSynthType_SuspiciousSynth					"EQAlarmSynth_SuspiciousSynth"

#define  DEF_EQAlarmSynth_AlarmSynthesis					"AlarmSynthesis"
#define  DEF_EQAlarmSynth_UnAckAlarmSynth					"UnAckAlarmSynth"
#define  DEF_EQAlarmSynth_ForcedSynthesis					"ForcedSynthesis"
#define  DEF_EQAlarmSynth_DisableSynthesis					"DisableSynthesis"
#define  DEF_EQAlarmSynth_LimitAlarmDisableSynthesis		"LimitAlarmDisableSynthesis"
#define  DEF_EQAlarmSynth_DevAlarmDisableSynthesis			"DevAlarmDisableSynthesis"
#define  DEF_EQAlarmSynth_RocAlarmDisableSynthesis			"RocAlarmDisableSynthesis"
#define  DEF_EQAlarmSynth_DIAlarmDisableSynthesis			"DIAlarmDisableSynthesis"
#define  DEF_EQAlarmSynth_InvalidSynthesis					"InvalidSynthesis"
#define  DEF_EQAlarmSynth_SuspiciousSynth					"SuspiciousSynth"
/************************************************************/




/************************************************************
 AreaAlarmSynth Type and its instance declaration
*************************************************************/
#define  DEF_AreaAlarmSynthType									"AreaAlarmSynth"
#define  DEF_AreaAlarmSynthType_EQAlarmSynth					"AreaAlarmSynth_EQAlarmSynth"
#define  DEF_AreaAlarmSynthType_OneDayNumAlmSynth				"AreaAlarmSynth_OneDayNumAlmSynth"
#define  DEF_AreaAlarmSynthType_CurNumEQAlarmSynth				"AreaAlarmSynth_CurNumEQAlarmSynth"

#define  DEF_AreaAlarmSynth_OneDayNumAlmSynth					"OneDayNumAlmSynth"
#define  DEF_AreaAlarmSynth_CurNumEQAlarmSynth					"CurNumEQAlarmSynth"

#define  DEF_Area_AlarmGraph									"Graph"

#define  DEF_AreaAlarmSynthType_EQAlarmSynth_AlarmSynthesis		"AreaAlarmSynth_EQAlarmSynth_AlarmSynthesis"
#define  DEF_AreaAlarmSynthType_EQAlarmSynth_UnAckAlarmSynth	"AreaAlarmSynth_EQAlarmSynth_UnAckAlarmSynth"
#define  DEF_AreaAlarmSynthType_EQAlarmSynth_ForcedSynthesis	"AreaAlarmSynth_EQAlarmSynth_ForcedSynthesis"
#define  DEF_AreaAlarmSynthType_EQAlarmSynth_InvalidSynthesis	"AreaAlarmSynth_EQAlarmSynth_InvalidSynthesis"
#define  DEF_AreaAlarmSynthType_EQAlarmSynth_SuspiciousSynth	"AreaAlarmSynth_EQAlarmSynth_SuspiciousSynth"
/************************************************************/




/************************************************************
 EQ Type and its instance declaration
*************************************************************/
#define  DEF_EQType												"EQ"
#define  DEF_EQType_EQAlarmSynth									"EQ_EQAlarmSynth"
#define  DEF_EQType_EQAlarmSynth_AlarmSynthesis						"EQ_EQAlarmSynth_AlarmSynthesis"
#define  DEF_EQType_EQAlarmSynth_UnAckAlarmSynth					"EQ_EQAlarmSynth_UnAckAlarmSynth"
#define  DEF_EQType_EQAlarmSynth_ForcedSynthesis					"EQ_EQAlarmSynth_ForcedSynthesis"
#define  DEF_EQType_EQAlarmSynth_DisableSynthesis					"EQ_EQAlarmSynth_DisableSynthesis"
#define  DEF_EQType_EQAlarmSynth_LimitAlarmDisableSynthesis			"EQ_EQAlarmSynth_LimitAlarmDisableSynthesis"
#define  DEF_EQType_EQAlarmSynth_InvalidSynthesis					"EQ_EQAlarmSynth_InvalidSynthesis"
#define  DEF_EQType_EQAlarmSynth_SuspiciousSynth					"EQ_EQAlarmSynth_SuspiciousSynth"
/************************************************************/




/************************************************************
 EQ Type and its instance declaration
*************************************************************/
#define DEF_AreaType											"Area"
#define DEF_AreaType_AreaAlarmSynth								"Area_AreaAlarmSynth"
#define DEF_AreaType_AreaAlarmSynth_OneDayNumAlmSynth			"Area_AreaAlarmSynth_OneDayNumAlmSynth"
#define DEF_AreaType_AreaAlarmSynth_CurNumEQAlarmSynth			"Area_AreaAlarmSynth_CurNumEQAlarmSynth"
#define DEF_AreaType_AreaAlarmSynth_EQAlarmSynth				"Area_AreaAlarmSynth_EQAlarmSynth"
#define DEF_AreaType_AreaAlarmSynth_EQAlarmSynth_AlarmSynthesis				"Area_AreaAlarmSynth_EQAlarmSynth_AlarmSynthesis"
#define DEF_AreaType_AreaAlarmSynth_EQAlarmSynth_UnAckAlarmSynth			"Area_AreaAlarmSynth_EQAlarmSynth_UnAckAlarmSynth"
#define DEF_AreaType_AreaAlarmSynth_EQAlarmSynth_ForcedSynthesis			"Area_AreaAlarmSynth_EQAlarmSynth_ForcedSynthesis"
#define DEF_AreaType_AreaAlarmSynth_EQAlarmSynth_InvalidSynthesis			"Area_AreaAlarmSynth_EQAlarmSynth_InvalidSynthesis"
#define DEF_AreaType_AreaAlarmSynth_EQAlarmSynth_SuspiciousSynth			"Area_AreaAlarmSynth_EQAlarmSynth_SuspiciousSynth"
/************************************************************/




/************************************************************
 AI Type and its instance declaration
*************************************************************/
#define DEF_AIType											"AI"
#define DEF_AIType_PV										"AI_PV"
#define DEF_AIType_PV_Definition							"AI_PV_Definition"
#define DEF_AIType_PV_EURange								"AI_PV_EURange"
#define DEF_AIType_PV_EngineeringUnits						"AI_PV_EngineeringUnits"
#define DEF_AIType_PV_InstrumentRange						"AI_PV_InstrumentRange"
#define DEF_AIType_PV_ValuePrecision						"AI_PV_ValuePrecision"

#define DEF_AIType_QualityDetail							"AI_QualityDetail"
#define DEF_AIType_Channel									"AI_Channel"
#define DEF_AIType_Channel_DeviceAddress					"AI_Channel_DeviceAddress"
#define DEF_AIType_Channel_Offset							"AI_Channel_Offset"
#define DEF_AIType_Channel_RawEngRange						"AI_Channel_RawEngRange"
#define DEF_AIType_Channel_Values							"AI_Channel_Values"
#define DEF_AIType_Channel_ValueDeses						"AI_Channel_ValueDeses"

#define DEF_AIType_AnalogConfig								"AI_AnalogConfig"
#define DEF_AIType_AnalogConfig_RangSwtMode					"AI_AnalogConfig_RangSwtMode"
#define DEF_AIType_AnalogConfig_AlarmModel					"AI_AnalogConfig_AlarmModel"
#define DEF_AIType_AnalogConfig_DeadZonePercentage			"AI_AnalogConfig_DeadZonePercentage"
#define DEF_AIType_AnalogConfig_NearlyzeroDeadZone			"AI_AnalogConfig_NearlyzeroDeadZone"
#define DEF_AIType_AnalogConfig_HisValueForcePeriod			"AI_AnalogConfig_HisValueForcePeriod"
#define DEF_AIType_AnalogConfig_CompressAccuracy			"AI_AnalogConfig_CompressAccuracy"
#define DEF_AIType_AnalogConfig_InputChannelNum				"AI_AnalogConfig_InputChannelNum"
#define DEF_AIType_AnalogConfig_OutputChannelNum			"AI_AnalogConfig_OutputChannelNum"
#define DEF_AIType_AnalogConfig_ValueInRange				"AI_AnalogConfig_ValueInRange"

#define DEF_AIType_LevelAlarmConfig								"AI_LevelAlarmConfig"
#define DEF_AIType_LevelAlarmConfig_LimitValues				"AI_LevelAlarmConfig_LimitValues"
#define DEF_AIType_LevelAlarmConfig_Severities				"AI_LevelAlarmConfig_Severities"
#define DEF_AIType_LevelAlarmConfig_Messages				"AI_LevelAlarmConfig_Messages"
#define DEF_AIType_LevelAlarmConfig_RestoreZone					"AI_LevelAlarmConfig_RestoreZone"
#define DEF_AIType_LevelAlarmConfig_TimeZone					"AI_LevelAlarmConfig_TimeZone"

#define DEF_AIType_DeviationAlarmConfig                                "AI_DeviationAlarmConfig"
#define DEF_AIType_DeviationAlarmConfig_PrimaryDiffZone                   "AI_DeviationAlarmConfig_PrimaryDiffZone"
#define DEF_AIType_DeviationAlarmConfig_SecondaryDiffZone                    "AI_DeviationAlarmConfig_SecondaryDiffZone"
#define DEF_AIType_DeviationAlarmConfig_HighHighSeverity                   "AI_DeviationAlarmConfig_HighHighSeverity"
#define DEF_AIType_DeviationAlarmConfig_HighSeverity                    "AI_DeviationAlarmConfig_HighSeverity"
#define DEF_AIType_DeviationAlarmConfig_LowSeverity                    "AI_DeviationAlarmConfig_LowSeverity"
#define DEF_AIType_DeviationAlarmConfig_LowLowSeverity                   "AI_DeviationAlarmConfig_LowLowSeverity"
#define DEF_AIType_DeviationAlarmConfig_HighHighMessage                     "AI_DeviationAlarmConfig_HighHighMessage"
#define DEF_AIType_DeviationAlarmConfig_HighMessage                      "AI_DeviationAlarmConfig_HighMessage"
#define DEF_AIType_DeviationAlarmConfig_LowMessage                      "AI_DeviationAlarmConfig_LowMessage"
#define DEF_AIType_DeviationAlarmConfig_LowLowMessage                     "AI_DeviationAlarmConfig_LowLowMessage"
#define DEF_AIType_DeviationAlarmConfig_Target							"AI_DeviationAlarmConfig_Target"
#define DEF_AIType_DeviationAlarmConfig_RestoreZone                    "AI_DeviationAlarmConfig_RestoreZone"
#define DEF_AIType_DeviationAlarmConfig_TimeZone                       "AI_DeviationAlarmConfig_TimeZone"

#define DEF_AIType_RocAlarmConfig									"AI_RocAlarmConfig"
#define DEF_AIType_RocAlarmConfig_LimitValues						"AI_RocAlarmConfig_LimitValues"
#define DEF_AIType_RocAlarmConfig_Severities						"AI_RocAlarmConfig_Severities"
#define DEF_AIType_RocAlarmConfig_Messages							"AI_RocAlarmConfig_Messages"
#define DEF_AIType_RocAlarmConfig_ChangeInterval					"AI_RocAlarmConfig_ChangeInterval"
#define DEF_AIType_RocAlarmConfig_CaculateInterval                  "AI_RocAlarmConfig_CaculateInterval"

#define DEF_AIType_ExclusiveLevelAlarmType					"AI_ExclusiveLevelAlarmType"
#define DEF_AIType_ExclusiveDeviationAlarmType				"AI_ExclusiveDeviationAlarmType"
#define DEF_AIType_ExclusiveRocAlarmType					"AI_ExclusiveRateOfChangeAlarmType"
#define DEF_AIType_HlyRoleAlarmType							"AI_HlyRoleAlarmType"
#define DEF_AIType_Force									"AI_Force"
#define DEF_AIType_Force_InputArguments						"AI_Force_InputArguments"
#define DEF_AIType_Disable									"AI_Disable"
#define DEF_AIType_Disable_InputArguments					"AI_Disable_InputArguments"

#define DEF_AIType_CurLimitState							"CurLimitState"
#define DEF_AIType_CurDevState								"CurDevState"
#define DEF_AIType_CurRocState								"CurRocState"
#define DEF_AIType_CurAlarmState							"CurAlarmState"
/************************************************************/



/************************************************************
 AO Type and its instance declaration
*************************************************************/
#define DEF_AOType											"AO"
#define DEF_AOType_SetPoint									"AOType_SetPoint"
#define DEF_AOType_SetPoint_InputArguments					"AOType_SetPoint_InputArguments"



/************************************************************
 AIO Type and its instance declaration
*************************************************************/
#define DEF_AIOType											"AIO"
#define DEF_AIOType_SP										"AIO_SP"
#define DEF_AIOType_SP_Definition							"AIO_SP_Definition"
#define DEF_AIOType_SP_EURange								"AIO_SP_EURange"
#define DEF_AIOType_SP_EngineeringUnits						"AIO_SP_EngineeringUnits"
#define DEF_AIOType_SP_InstrumentRange						"AIO_SP_InstrumentRange"
#define DEF_AIOType_SP_ValuePrecision						"AIO_SP_ValuePrecision"
#define DEF_AIOType_RegConfigParams							"AIO_RegConfigParams"
#define DEF_AIOType_RegConfigParams_CtrlModel				"AIO_RegConfigParams_CtrlModel"
#define DEF_AIOType_RegConfigParamsType_SBOOperateTimeout	"AIO_RegConfigParams_SBOOperateTimeout"
#define DEF_AIOType_RegConfigParamsType_OperateCount		"AIO_RegConfigParams_OperateCount"
#define DEF_AIOType_RegConfigParamsType_FailAlarm			"AIO_RegConfigParams_FailAlarm"
#define DEF_AIOType_RegConfigParamsType_CreateAlarm			"AIO_RegConfigParams_CreateAlarm"
#define DEF_AIOType_RegConfigParams_StepSize				"AIO_RegConfigParams_StepSize"
#define DEF_AIOType_CtrlProcessInfo							"AIO_CtrlProcessInfo"
#define DEF_AIOType_CtrlProcessInfo_OperateTime				"AIO_CtrlProcessInfo_OperateTime"
#define DEF_AIOType_CtrlProcessInfo_Originator				"AIO_CtrlProcessInfo_Originator"
#define DEF_AIOType_CtrlProcessInfo_Originator_orCat		"AIO_CtrlProcessInfo_Originator_orCat"
#define DEF_AIOType_CtrlProcessInfo_Originator_orIdent		"AIO_CtrlProcessInfo_Originator_orIdent"
#define DEF_AIOType_CtrlProcessInfo_OperateCount			"AIO_CtrlProcessInfo_Count"
#define DEF_AIOType_CtrlProcessInfo_OperateStateMachine		"AIO_CtrlProcessInfo_OperateStateMachine"
#define	DEF_AIOType_OffNormalAlarm							"AIO_OffNormalAlarm"
#define	DEF_AIOType_CtrlFailureAlarm						"AIO_CtrlFailureAlarm"
#define DEF_AIOType_SetPoint								"AIO_SetPoint"
#define DEF_AIOType_SetPoint_InputArguments					"AIO_SetPoint_InputArguments"
#define DEF_AIOType_Operate									"AIO_Operate"
#define DEF_AIOType_Operate_InputArguments					"AIO_Operate_InputArguments"

#define DEF_AIO_SP											"SP"
/************************************************************/




/************************************************************
 DI Type and its instance declaration
*************************************************************/
#define DEF_DIType											"DI"
#define DEF_DIType_PV										"DI_PV"
#define DEF_DIType_PV_Definition							"DI_PV_Definition"
#define DEF_DIType_PV_ValuePrecision						"DI_PV_ValuePrecision"
#define DEF_DIType_QualityDetail							"DI_QualityDetail"
#define DEF_DIType_DiscreteAlarm							"DI_DiscreteAlarm"
#define DEF_DiscreteConfigType_HasEvent						"DI_HasEvent"
#define DEF_DIType_SetItem									"DI_SetItem"
#define DEF_DIType_Force									"DI_Force"
#define DEF_DIType_Force_InputArguments						"DI_Force_InputArguments"
#define DEF_DIType_Disable									"DI_Disable"
#define DEF_DIType_Disable_InputArguments					"DI_Disable_InputArguments"

#define DEF_DIType_QualityDetail							"DI_QualityDetail"
#define DEF_DIType_Channel									"DI_Channel"
#define DEF_DIType_Channel_DeviceAddress					"DI_Channel_DeviceAddress"
#define DEF_DIType_Channel_Offset							"DI_Channel_Offset"
#define DEF_DIType_Channel_RawEngRange						"DI_Channel_RawEngRange"
#define DEF_DIType_Channel_Values							"DI_Channel_Values"
#define DEF_DIType_Channel_ValueDeses						"DI_Channel_ValueDeses"

#define DEF_DIType_DiscreteConfigType								"DI_DiscreteConfig"
#define DEF_DIType_DiscreteConfigType_DiscreteType					"DI_DiscreteConfig_DiscreteType"
#define DEF_DIType_DiscreteConfigType_AlarmModel					"DI_DiscreteConfig_AlarmModel"
#define DEF_DIType_DiscreteConfigType_DebounceTime					"DI_DiscreteConfig_DebounceTime"
#define DEF_DIType_DiscreteConfigType_HistoryPeriod					"DI_DiscreteConfig_HistoryPeriod"
#define DEF_DIType_DiscreteConfigType_NeedConverse					"DI_DiscreteConfig_NeedConverse"
#define DEF_DIType_DiscreteConfigType_NeedStatistics				"DI_DiscreteConfig_NeedStatistics"
#define DEF_DIType_DiscreteConfigType_NeedCmdHistory				"DI_DiscreteConfig_NeedCmdHistory"
#define DEF_DIType_DiscreteConfigType_Values						"DI_DiscreteConfig_Values"
#define DEF_DIType_DiscreteConfigType_ValueDeses					"DI_DiscreteConfig_ValueDeses"
#define DEF_DIType_DiscreteConfigType_ValueMerge					"DI_DiscreteConfig_ValueMerge"
#define DEF_DIType_DiscreteConfigType_InputChannelNum				"DI_DiscreteConfig_InputChannelNum"
#define DEF_DIType_DiscreteConfigType_OutputChannelNum				"DI_DiscreteConfig_OutputChannelNum"

#define DEF_DIType_DiscreteAlarmConfigType							"DI_DiscreteAlarmConfig"
#define DEF_DIType_DiscreteAlarmConfigType_ActiveState				"DI_DiscreteAlarmConfig_ActiveState"
#define DEF_DIType_DiscreteAlarmConfigType_AlmMessage				"DI_DiscreteAlarmConfig_AlmMessage"
#define DEF_DIType_DiscreteAlarmConfigType_AlmLevel					"DI_DiscreteAlarmConfig_AlmLevel"
#define DEF_DIType_DiscreteAlarmConfigType_TimeZone					"DI_DiscreteAlarmConfig_TimeZone"
#define DEF_DIType_DiscreteAlarmConfigType_AlmType					"DI_DiscreteAlarmConfig_AlmType"
/************************************************************/


/************************************************************
 DO Type and its instance declaration
*************************************************************/
#define DEF_DOType											"DO"
#define DEF_DOType_SetPoint									"DO_SetPoint"
#define DEF_DOType_SetPoint_InputArguments					"DO_SetPoint_InputArguments"
#define DEF_OutValueConfigType								"OutValueConfig"
#define DEF_OutValueConfig_Values							"Values"
#define DEF_OutValueConfig_ValueDeses						"ValueDeses"


/************************************************************
 DIO Type and its instance declaration
*************************************************************/
#define DEF_DIOType											"DIO"

#define DEF_DIOType_SP										"DIO_SP"
#define DEF_DIOType_SP_Definition							"DIO_SP_Definition"
#define DEF_DIOType_SP_ValuePrecision						"DIO_SP_ValuePrecision"

#define DEF_DIOType_PV2										"DIO_PV2"
#define DEF_DIOType_PV2_Definition							"DIO_PV2_Definition"
#define DEF_DIOType_PV2_ValuePrecision						"DIO_PV2_ValuePrecision"

#define DEF_DIOType_PVS										"DIO_PVS"
#define DEF_DIOType_PVS_Definition							"DIO_PVS_Definition"
#define DEF_DIOType_PVS_ValuePrecision						"DIO_PVS_ValuePrecision"

#define DEF_DIOType_CtrlConfigParams						"DIO_CtrlConfigParams"
#define  DEF_DIOType_CtrlConfigParams_CtrlModel				"DIO_CtrlConfigParams_CtrlModel"
#define  DEF_DIOType_CtrlConfigParams_PulseConfig			"DIO_CtrlConfigParams_PulseConfig"
#define  DEF_DIOType_CtrlConfigParams_PulseConfig_CmdQual	"DIO_CtrlConfigParams_PulseConfig_CmdQual"
#define  DEF_DIOType_CtrlConfigParams_PulseConfig_OnDur		"DIO_CtrlConfigParams_PulseConfig_OnDur"
#define  DEF_DIOType_CtrlConfigParams_PulseConfig_OffDur	"DIO_CtrlConfigParams_PulseConfig_OffDur"
#define  DEF_DIOType_CtrlConfigParams_PulseConfig_NumPls	"DIO_CtrlConfigParams_PulseConfig_NumPls"
#define  DEF_DIOType_CtrlConfigParams_SBOSelectTimeout		"DIO_CtrlConfigParams_SBOSelectTimeout"
#define  DEF_DIOType_CtrlConfigParams_SBOOperateTimeout		"DIO_CtrlConfigParams_SBOOperateTimeout"
#define  DEF_DIOType_CtrlConfigParams_OperateCount			"DIO_CtrlConfigParams_OperateCount"
#define  DEF_DIOType_CtrlConfigParams_CtrlTarget			"DIO_CtrlConfigParams_Target"
#define  DEF_DIOType_CtrlConfigParams_FailAlarm				"DIO_CtrlConfigParams_FailAlarm"
#define  DEF_DIOType_CtrlConfigParams_CreateAlarm			"DIO_CtrlConfigParams_CreateAlarm"
#define  DEF_DIOType_CtrlConfigParams_SBOClass				"DIO_CtrlConfigParams_SBOClass"

#define DEF_DIOType_CtrlProcessInfo							"DIO_CtrlProcessInfo"
#define DEF_DIOType_CtrlProcessInfo_OperateTime				"DIO_CtrlProcessInfo_OperateTime"
#define DEF_DIOType_CtrlProcessInfo_Originator				"DIO_CtrlProcessInfo_Originator"
#define DEF_DIOType_CtrlProcessInfo_Originator_orCat		"DIO_CtrlProcessInfo_Originator_orCat"
#define DEF_DIOType_CtrlProcessInfo_Originator_orIdent		"DIO_CtrlProcessInfo_Originator_orIdent"
#define DEF_DIOType_CtrlProcessInfo_OperateCount			"DIO_CtrlProcessInfo_Count"
#define DEF_DIOType_CtrlProcessInfo_OperateStateMachine		"DIO_CtrlProcessInfo_OperateStateMachine"


#define DEF_DIOType_CtrlFailureCondition					"DIO_CtrlFailureCondition"
#define DEF_DIOType_CtrlFailureAlarm						"DIO_CtrlFailureAlarm"
#define DEF_DIOType_SetPoint								"DIO_SetPoint"
#define DEF_DIOType_SetPoint_InputArguments					"DIO_SetPoint_InputArguments"
#define DEF_DIOType_Select									"DIO_Select"
#define DEF_DIOType_SelectWithValue							"DIO_SelectWithValue"
#define DEF_DIOType_SelectWithValue_InputArguments			"DIO_SelectWithValue_InputArguments"
#define DEF_DIOType_Cancel									"DIO_Cancel"
#define DEF_DIOType_Operate									"DIO_Operate"
#define DEF_DIOType_Operate_InputArguments					"DIO_Operate_InputArguments"
#define DEF_DIOType_TimeActivedOperate						"DIO_TimeActivedOperate"
#define DEF_DIOType_TimeActivedOperate_InputArguments		"DIO_TimeActivedOperate_InputArguments"
#define DEF_DIOType_CommandTerminate						"DIO_CommandTerminate"

#define DEF_DIO_SP										"SP"
#define DEF_DIO_PV2										"PV2"
#define DEF_DIO_PVS										"PVS"
#define DEF_PV											"PV"
/************************************************************/




/************************************************************
 枚举类型
*************************************************************/
#define DEF_QualityDetail_Enum								"QualityDetail"
#define DEF_OriginatorCat_Enum								"OriginatorCat"
#define DEF_AIOCtrlModels_Enum								"AIOCtrlModels"
#define DEF_CtrlModels_Enum									"CtrlModels"
#define DEF_SboClass_Enum									"SboClass"
#define DEF_UDP_Enum										"UDP"
#define DEF_CmdQual_Enum									"CmdQual"
#define DEF_AlarmType_Enum									"AlarmType"
#define DEF_DiscreteType_Enum								"DiscreteType"
#define DEF_AlarmModel_Enum									"AlarmModel"
#define DEF_AlarmState_Enum									"AlarmState"
#define Base_Reg_Config										"Reg_Config"
#define Base_Reg_Config_DefaultBinary						"Reg_Config_DefaultBinary"
#define Base_Reg_Config_DefaultXml							"Reg_Config_DefaultXml"
/************************************************************/


/************************************************************
 控制模式	指遥控/遥调输出时采用的控制类型
*************************************************************/
enum AIOCtrlModels
{
	Direct_with_normal_security,			//! 常规安全的直接控制
	Direct_with_enhenced_security			//! 增强安全的直接控制
};


/************************************************************
 控制模式	指遥控/遥调输出时采用的控制类型
*************************************************************/
enum CtrlModels
{
	CtrlModel_direct_with_normal_security,			//! 常规安全的直接控制
	CtrlModel_sbo_with_normal_security,				//! 常规安全的操作之前选择控制
	CtrlModel_Direct_with_enhenced_security,		//! 增强安全的直接控制
	CtrlModel_Sbo_with_enhenced_security			//! 增强安全的操作之前选择控制
};


/************************************************************
 操作前选择类型		用于遥控带选择的控制输出
*************************************************************/
enum SboClass
{
	SboClass_operate_once,							//! 选择一次执行一次
	SboClass_operate_many							//! 选择一次执行多次
};

/************************************************************
 细节数据质量
*************************************************************/
enum QualityDetail
{
	QualityDetail_Overflow = 1,							//! 溢出
	QualityDetail_OutOfRange = 2,						//! 超量程
	QualityDetail_BadReference = 4,						//! 坏基准值
	QualityDetail_Oscillatory = 8,						//! 抖动
	QualityDetail_Failure = 16,							//! 故障
	QualityDetail_OldData = 32,							//! 值过时
	QualityDetail_Inconsistent = 64,					//! 不一致
	QualityDetail_Inaccurate = 128,						//! 不准确
	QualityDetail_Process = 512,						//! 过程
	QualityDetail_Substituted = 1048,					//! 替代值
	QualityDetail_Inhibit = 2096,						//! 值的刷新被操作员禁止
	QualityDetail_Test = 4192							//! 一个变量的值处于被测试状态
};

/************************************************************
 报警类型
*************************************************************/
enum AlarmType
{
	AlarmType_Process,							//! 过程
	AlarmType_System,							//! 系统
	AlarmType_SecurityAudit						//! 安全审计
};

/************************************************************
 离散量类型
*************************************************************/
enum DiscreteType
{
	DiscreteType_SingleBit,							//! 单位
	DiscreteType_CoupleBit,							//! 双位
	DiscreteType_MultiBit							//! 多态
};

/************************************************************
 离散量报警模型
*************************************************************/
enum AlarmModel
{
	AlarmModel_No,								//! 无报警
	AlarmModel_Exception,						//! 异常报警
	AlarmModel_Change							//! 跳闸报警
};

}

#endif
