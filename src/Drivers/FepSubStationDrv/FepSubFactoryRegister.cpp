/*!
 *	\file	FepSubFactoryRegister.cpp
 *
 *	\brief	FepSubStationDriver��̬����غ�ж����ں���ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *
 *	\date	2015��11��20��	19:33
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2015, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "FepSubStationDriver.h"
#include "FepSubStationDevice.h"

namespace MACS_SCADA_SUD
{
	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		GENERIC_REGISTER_CLASS( CIODriverBase, CFepSubStationDriver, &g_IODriverFactory );
		GENERIC_REGISTER_CLASS( CIODeviceBase, CFepSubStationDevice, &g_DrvDeviceFactory);
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS( CIODriverBase, CFepSubStationDriver, &g_IODriverFactory );
		GENERIC_UNREGISTER_CLASS(  CIODeviceBase, CFepSubStationDevice, &g_DrvDeviceFactory);
		return 0;
	}
}