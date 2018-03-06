/*!
 *	\file	FACTORYREGISTER.cpp
 *
 *	\brief	动态库加载和卸载入口函数实现源文件
 *
 *	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FACTORYREGISTER.cpp, 470+1 2011/04/22 01:56:47 xuhongtao $
 *	$Author: xuhongtao $
 *	$Date: 2011/04/22 01:56:47 $
 *	$Revision: 470+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "FepModbusDriver.h"
#include "FepModbusPrtcl.h"
#include "FepModbusDevice.h"

namespace MACS_SCADA_SUD
{
	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		//初始化类工厂
		GENERIC_REGISTER_CLASS( CIOPrtclParserBase, CFepModbusPrtcl, &g_DrvPrtclFactory );
		GENERIC_REGISTER_CLASS( CIODriverBase, CFepModbusDriver, &g_IODriverFactory );
		GENERIC_REGISTER_CLASS( CIODeviceBase, CFepModbusDevice, &g_DrvDeviceFactory );
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS( CIOPrtclParserBase, CFepModbusPrtcl, &g_DrvPrtclFactory );
		GENERIC_UNREGISTER_CLASS( CIODriverBase, CFepModbusDriver, &g_IODriverFactory );
		GENERIC_UNREGISTER_CLASS( CIODeviceBase, CFepModbusDevice, &g_DrvDeviceFactory );
		return 0;
	}
}
