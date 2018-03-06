/*!
 *	\file	FepIEC104DrvH.h
 *
 *	\brief	104��������Ԥ����ͷ�ļ�
 *
 *	�����˸����๫�õĺ꣬����Ҫ������ͷ�ļ�һ�ΰ�������
 *
 *	$Header: ?/MACS-SCADA 3.2 �������/MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104DrvH.h, 23+1 2011/04/26 05:11:09 miaoweijie $
 *	$Author: miaoweijie $
 *	$Date: 2011/04/26 05:11:09 $
 *	$Revision: 23+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOMACSIEC104DRVH_20080812_H_
#define _IOMACSIEC104DRVH_20080812_H_

#ifdef FEPIEC104DRV_EXPORTS
#define IOMACSIEC104_API MACS_DLL_EXPORT
#else
#define IOMACSIEC104_API MACS_DLL_IMPORT
#endif

#define MACSIEC104DRIVER_CONNECT_MAXTIMES	1					//! ����������������������Ӵ���
#define MACSIEC104DRIVER_MAX_TIMEOUT		2000				//! ���Ͷ�ʱ�����ʱʱ�䣨���룩
#define MACSIEC104CONN_MAX_TIMEOUT			5000				//! ���Ӷ�ʱ�����ʱʱ�䣨���룩
#define PERIOD_SCAN_MS						1000				//! ���ڶ�ʱ���������ڣ����롣

//#ifndef MAKEDWORD
//#define MAKEDWORD(l, h)	((DWORD)(((USHORT)(l)) | ((DWORD)((USHORT)(h))) << 16))
//#endif		//MAKEDWORD

//typedef unsigned short WORD;
//typedef unsigned long  DWORD;
//typedef OpcUa_UInt32 DWORD;

// #include "../../../OORTDB_Design/Server/PublicFile/OS_Ext/OS_Ext.h"
// #include "../../../OORTDB_Design/Server/PublicFile/GenericFactory/GenericFactory.h"
// #include "../../Server/PublicFile/IODataType.h"
// #include "../../../OORTDB_Design/Server/PublicFile/DebugMsg/Debug_Msg.h"
// #include "../../Server/IODebugManager/IODebugManager.h"
// #include "../../Server/IOTimer/IOTimer.h"
#include "RTDB/Server/DBDriverFrame/IODriverH.h"

#include "IEC104.h"

// #include "ace/OS.h"
// #include "ace/Get_Opt.h"
// #include "ace/Log_Msg.h"
// #include "ace/Date_Time.h"

#endif
