/*!
 *	\file	FACTORYREGISTER.cpp
 *
 *	\brief	��̬����غ�ж����ں���ʵ��Դ�ļ�
 *
 *	�����๤��ģʽ����ʼ����ʱ���๤��ע�᱾��̬���ṩ�ľ����࣬ж��ʱ����ȡ��ע��
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FACTORYREGISTER.cpp 1     11-01-21 10:02a Zhangyuguo $
 *	$Author: Zhangyuguo $
 *	$Date: 11-01-21 10:02a $
 *	$Revision: 1 $
 *
 *	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _FEP_IEC103_FACTORY_H_
#define _FEP_IEC103_FACTORY_H_

#include "FepIEC103Drv.h"
#include "FepIEC103Prtcl.h"
#include "FepIEC103Device.h"

namespace MACS_SCADA_SUD
{
	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		//! ��ʼ���๤��
		GENERIC_REGISTER_CLASS( CIOPrtclParserBase, CFepIEC103Prtcl, &g_DrvPrtclFactory );
		GENERIC_REGISTER_CLASS( CIODriverBase, CFepIEC103Drv, &g_IODriverFactory );
		GENERIC_REGISTER_CLASS( CIODeviceBase, CFepIEC103Device, &g_DrvDeviceFactory );
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS( CIOPrtclParserBase, CFepIEC103Prtcl, &g_DrvPrtclFactory );
		GENERIC_UNREGISTER_CLASS( CIODriverBase, CFepIEC103Drv, &g_IODriverFactory );
		GENERIC_UNREGISTER_CLASS( CIODeviceBase, CFepIEC103Device, &g_DrvDeviceFactory );
		return 0;
	}

}

#endif