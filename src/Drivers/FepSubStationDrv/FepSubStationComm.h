#ifndef _FEPSUBSTATIONCOMM_H_
#define _FEPSUBSTATIONCOMM_H_

#include "RTDB/Server/DBDriverFrame/IODriverH.h"
#include "SubDataBaseComm.h"
#include "SubDataBaseDevice.h"
#include "SubDataBaseDriver.h"

#ifdef FEPSUBSTATION_EXPORTS
#define FEPSUBSTATIONDRIVER_API MACS_DLL_EXPORT
#else
#define FEPSUBSTATIONDRIVER_API MACS_DLL_IMPORT
#endif

namespace MACS_SCADA_SUD
{
	const std::string FEPSUBSTATIONDRIVER = "CFepSubStationDriver";
	const std::string FEPSUBSTATIONDEVICE = "CFepSubStationDevice";
}

#endif