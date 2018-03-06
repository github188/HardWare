#include "RTDB/Server/FEPPlatformClass/IOUnit.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"
#include "FepSubStationDevice.h"
#include "HyServerRedundancy/switchserv.h"
#include "DiagSwitch/common.h"

namespace MACS_SCADA_SUD
{
	//!定义设备类类名//
	std::string CFepSubStationDevice::s_ClassName(FEPSUBSTATIONDEVICE);


	CFepSubStationDevice::CFepSubStationDevice()
	{
		UaString sLog(UaString("%1::Construct successful! \n").arg(s_ClassName.c_str()));
		WriteLogAnyWay(sLog.toUtf8());
	}

	CFepSubStationDevice::~CFepSubStationDevice()
	{
		UaString sLog(UaString("%1::Destructor successful! \n").arg(s_ClassName.c_str()));
		WriteLogAnyWay(sLog.toUtf8());
	}

	std::string CFepSubStationDevice::GetClassName()
	{
		return s_ClassName;
	}
}
