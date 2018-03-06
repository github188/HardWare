/*!
*	\file	FactoryRegister.cpp
*
*	\brief	向工厂类注册
*
*	$Date: 15-03-30$
*
*/
#include "NR103Driver.h"
#include "NR103Device.h"
#include "NR103Prtcl.h"

namespace MACS_SCADA_SUD
{
	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		GENERIC_REGISTER_CLASS( CIOPrtclParserBase, CNR103Prtcl, &g_DrvPrtclFactory );
		GENERIC_REGISTER_CLASS( CIODriverBase, CNR103Driver, &g_IODriverFactory );
		GENERIC_REGISTER_CLASS( CIODeviceBase, CNR103Device, &g_DrvDeviceFactory );
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS( CIOPrtclParserBase, CNR103Prtcl, &g_DrvPrtclFactory );
		GENERIC_UNREGISTER_CLASS( CIODriverBase, CNR103Driver, &g_IODriverFactory );
		GENERIC_UNREGISTER_CLASS( CIODeviceBase, CNR103Device, &g_DrvDeviceFactory );
		return 0;
	}
}
