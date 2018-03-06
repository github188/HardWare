/*!
 *	\file	FactoryRegister.cpp
 *
 *	\brief	UADriver动态库加载和卸载入口函数实现
 *
 *	详细的文件说明（可选）
 *
 *	\author LIFAN
 *
 *	\date	2014年5月20日	19:33
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "UADriver.h"
#include "UADevice.h"

namespace MACS_SCADA_SUD
{
	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		GENERIC_REGISTER_CLASS( CIODriverBase, CUADriver, &g_IODriverFactory );
		GENERIC_REGISTER_CLASS( CIODeviceBase, CUADevice, &g_DrvDeviceFactory);
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS( CIODriverBase, CUADriver, &g_IODriverFactory );
		GENERIC_UNREGISTER_CLASS(  CIODeviceBase, CUADevice, &g_DrvDeviceFactory);
		return 0;
	}
}