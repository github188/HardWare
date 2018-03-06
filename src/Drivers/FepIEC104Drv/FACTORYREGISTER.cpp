/*!
 *	\file	FACTORYREGISTER.cpp
 *
 *	\brief	动态库加载和卸载入口函数实现源文件
 *
 *	利用类工厂模式，初始加载时向类工厂注册本动态库提供的具体类，卸载时反向取消注册
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FACTORYREGISTER.cpp 3     09-03-05 15:03 Litianhui $
 *	$Author: Litianhui $
 *	$Date: 09-03-05 15:03 $
 *	$Revision: 3 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "FepIEC104Drv.h"
#include "FepIEC104Prtcl.h"
#include "FepIEC104Device.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"

namespace MACS_SCADA_SUD
{
	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		//! 初始化类工厂
		GENERIC_REGISTER_CLASS( CIOPrtclParserBase, CFepIEC104Prtcl, &g_DrvPrtclFactory );
		GENERIC_REGISTER_CLASS( CIODriverBase, CFepIEC104Drv, &g_IODriverFactory );
		GENERIC_REGISTER_CLASS( CIODeviceBase, CFepIEC104Device, &g_DrvDeviceFactory );
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS( CIOPrtclParserBase, CFepIEC104Prtcl, &g_DrvPrtclFactory );
		GENERIC_UNREGISTER_CLASS( CIODriverBase, CFepIEC104Drv, &g_IODriverFactory );
		GENERIC_UNREGISTER_CLASS( CIODeviceBase, CFepIEC104Device, &g_DrvDeviceFactory );
		return 0;
	}
}
