#ifndef _SUBDATABASECOMM_H_
#define _SUBDATABASECOMM_H_

#include "RTDB/Server/DBDriverFrame/IODriverH.h"

#ifdef SUBDATABASEDRIVER_EXPORTS
#define SUBDATABASEDRIVER_API MACS_DLL_EXPORT
#else
#define SUBDATABASEDRIVER_API MACS_DLL_IMPORT
#endif

namespace MACS_SCADA_SUD
{
	typedef std::map<OpcUa_UInt32, UaVariable*> T_MAPMItems;
	const std::string SUBDATABASEDRIVER = "CSubDataBaseDriver";
	const std::string SUBDATABASEDEVICE = "CSubDataBaseDevice";
}

#endif