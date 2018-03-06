/*!
 *	\file	IODriverFrameH.h
 *
 *	\brief	注册类工厂
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月23日	18:09
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODRIVERFRAME_H_
#define _IODRIVERFRAME_H_

#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/GenericFactory.h"
#include "RTDBDriverFrame/IODeviceBase.h"
#include "IODeviceSimBase.h"
#include "RTDBDriverFrame/IOPrtclParser.h"

namespace MACS_SCADA_SUD
{
	extern "C" MACS_DLL_EXPORT int InitDriverDll()
	{
		//初始化类工厂
		GENERIC_REGISTER_CLASS( CIODeviceBase, CIODeviceBase, &g_DrvDeviceFactory );
		GENERIC_REGISTER_CLASS( CIODeviceSimBase, CIODeviceSimBase, &g_DrvSimFactory );
		GENERIC_REGISTER_CLASS( CIOPrtclParserBase, CIOPrtclParser, &g_DrvPrtclFactory );
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDriverDll()
	{
		GENERIC_UNREGISTER_CLASS( CIODeviceBase, CIODeviceBase, &g_DrvDeviceFactory );
		GENERIC_UNREGISTER_CLASS( CIODeviceSimBase, CIODeviceSimBase, &g_DrvSimFactory );
		GENERIC_UNREGISTER_CLASS( CIOPrtclParserBase, CIOPrtclParser, &g_DrvPrtclFactory );
		return 0;
	}
}

#endif