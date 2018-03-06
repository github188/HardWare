/*!
 *	\file	FactoryRegister.cpp
 *
 *	\brief	UADriver��̬����غ�ж����ں���ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��5��20��	19:33
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