/*!
 *	\file	fep_identifiers.h
 *
 *	\brief	FEP类型定义Id
 *
 *
 *	\author lingling.li
 *
 *	\date	2014年4月11日	11:30
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef __fep_identifiers_h__
#define __fep_identifiers_h__


#define FepId_ChannelType                       200

#define FepId_ChannelType_RealPrmScnd			(FepId_ChannelType+1)
#define FepId_ChannelType_IsOK					(FepId_ChannelType+2)
#define FepId_ChannelType_IsSimulate			(FepId_ChannelType+3)
#define FepId_ChannelType_SimMode				(FepId_ChannelType+4)
#define FepId_ChannelType_IsDisable				(FepId_ChannelType+5)
#define FepId_ChannelType_IsForce				(FepId_ChannelType+6)
#define FepId_ChannelType_ForcePrmScnd			(FepId_ChannelType+7)
#define FepId_ChannelType_Weight				(FepId_ChannelType+8)
#define FepId_ChannelType_PeerWeight			(FepId_ChannelType+9)
#define FepId_ChannelType_IsMonitor				(FepId_ChannelType+10)
#define FepId_ChannelType_IsRecord				(FepId_ChannelType+11)
#define FepId_ChannelType_WatchTime				(FepId_ChannelType+12)
#define FepId_ChannelType_PollTime				(FepId_ChannelType+13)
#define FepId_ChannelType_TimeOut				(FepId_ChannelType+14)
#define FepId_ChannelType_CommDataFormat		(FepId_ChannelType+15)
#define FepId_ChannelType_DrvName				(FepId_ChannelType+16)
#define FepId_ChannelType_Type					(FepId_ChannelType+17)
#define FepId_ChannelType_Port					(FepId_ChannelType+18)
#define FepId_ChannelType_BaudRate				(FepId_ChannelType+19)
#define FepId_ChannelType_DataBits				(FepId_ChannelType+20)
#define FepId_ChannelType_StopBits				(FepId_ChannelType+21)
#define FepId_ChannelType_Parity				(FepId_ChannelType+22)
#define FepId_ChannelType_DefaultPrmScnd		(FepId_ChannelType+23)
#define FepId_ChannelType_PortAccessMode		(FepId_ChannelType+24)
#define FepId_ChannelType_SwitchAlgName			(FepId_ChannelType+25)
#define FepId_ChannelType_Option				(FepId_ChannelType+28)
#define FepId_ChannelType_IPA_A					(FepId_ChannelType+29)
#define FepId_ChannelType_IPA_B					(FepId_ChannelType+29)
#define FepId_ChannelType_IPB_A					(FepId_ChannelType+30)
#define FepId_ChannelType_IPB_B					(FepId_ChannelType+31)

#define FepId_ChannelType_SendCmd					(FepId_ChannelType+32)
#define FepId_ChannelType_SendCmd_InputArguments	(FepId_ChannelType+33)
#define FepId_ChannelType_SendCmd_OutputArguments	(FepId_ChannelType+34)


#define FepId_ChannelType_Device                        300

#define FepId_ChannelType_Device_RealPrmScnd			(FepId_ChannelType_Device+1)
#define FepId_ChannelType_Device_IsOnline				(FepId_ChannelType_Device+2)
#define FepId_ChannelType_Device_IsSimulate				(FepId_ChannelType_Device+3)
#define FepId_ChannelType_Device_SimMode				(FepId_ChannelType_Device+4)
#define FepId_ChannelType_Device_IsDisable				(FepId_ChannelType_Device+5)
#define FepId_ChannelType_Device_IsForce				(FepId_ChannelType_Device+6)
#define FepId_ChannelType_Device_ForcePrmScnd			(FepId_ChannelType_Device+7)
#define FepId_ChannelType_Device_Power					(FepId_ChannelType_Device+8)
#define FepId_ChannelType_Device_DeviceAddr				(FepId_ChannelType_Device+10)
#define FepId_ChannelType_Device_DefaultPrmScnd			(FepId_ChannelType_Device+11)
#define FepId_ChannelType_Device_IsExtraRange           (FepId_ChannelType_Device+12)


#define FepId_DeviceType                        400

#define FepId_DeviceType_RealPrmScnd			(FepId_DeviceType+1)
#define FepId_DeviceType_IsOnline				(FepId_DeviceType+2)
#define FepId_DeviceType_IsSimulate				(FepId_DeviceType+3)
#define FepId_DeviceType_SimMode				(FepId_DeviceType+4)
#define FepId_DeviceType_IsDisable				(FepId_DeviceType+5)
#define FepId_DeviceType_IsForce				(FepId_DeviceType+6)
#define FepId_DeviceType_ForcePrmScnd			(FepId_DeviceType+7)
#define FepId_DeviceType_Power					(FepId_DeviceType+8)
#define FepId_DeviceType_DeviceAddr				(FepId_DeviceType+10)
#define FepId_DeviceType_DefaultPrmScnd			(FepId_DeviceType+11)
#define FepId_DeviceType_IsExtraRange           (FepId_DeviceType+12)

#define FepId_DeviceType_SendCmd					(FepId_DeviceType+13)
#define FepId_DeviceType_SendCmd_InputArguments		(FepId_DeviceType+14)
#define FepId_DeviceType_SendCmd_OutputArguments	(FepId_DeviceType+15)

const unsigned char MASTER = 1;           //!主机
const unsigned char SLAVE = 0;            //!从机     可能要和其他定义合并

#endif // __fep_identifiers_h__

