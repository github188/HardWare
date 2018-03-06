#ifndef _FEPSUBSTATIONDRIVER_H_
#define _FEPSUBSTATIONDRIVER_H_

#include "RTDB/Server/DBDriverFrame/IODriver.h"
#include "RTDB/Server/FEPPlatformClass/IOPort.h"
#include "RTDB/Server/FEPPlatformClass/fep_identifiers.h"
#include "RTDB/Server/utilities/common_identifiers.h"
#include "ServerProxy/siteproxycallback.h"
#include "ServerProxy/siteproxy.h"
#include "ServerProxyMgr/proxymgr.h"
#include "FepSubStationComm.h"
#include "FepSubStationDevice.h"

namespace MACS_SCADA_SUD
{

	class FEPSUBSTATIONDRIVER_API CFepSubStationDriver : public CSubDataBaseDriver
	{
	public:
		CFepSubStationDriver();

		~CFepSubStationDriver();	

		std::string GetClassName();
		
		virtual OpcUa_Int32 StartWork( void );

		virtual std::string GetDeviceClassName();
		virtual void  GetAllDbUnitId();
		virtual OpcUa_Boolean IsAllDbUnitFromSameRtdb(std::string &rtdbSiteName);

		//暂时屏蔽子站驱动的写功能
		virtual OpcUa_Int32 WriteMgrForPort(tIORunMgrCmd& tRunMgr);
		virtual OpcUa_Int32 WriteMgrForUnit(tIORunMgrCmd& tRunMgr);
		virtual OpcUa_Int32 WriteMgrForTag( tIORunMgrCmd& tRunMgr );
	private:
		std::string GetRtdbSiteName();
	public:
		//!Driver类类名//
		static std::string s_ClassName;
		static std::string s_MatchDeviceClassName;
		std::vector<OpcUa_UInt16> m_dbUnit;
	};
}

#endif