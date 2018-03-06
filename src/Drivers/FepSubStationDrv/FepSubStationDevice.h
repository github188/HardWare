#ifndef _FEPSUBSTATIONDEVICE_H_
#define _FEPSUBSTATIONDEVICE_H_

#include "FepSubStationComm.h"
#include "ServerProxy/sitesubscriptioncallback.h"
#include "ServerProxy/sitesubscription.h"
#include "ServerProxy/siteproxy.h"

namespace MACS_SCADA_SUD
{
	class FEPSUBSTATIONDRIVER_API CFepSubStationDevice : public CSubDataBaseDevice
	{

	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CFepSubStationDevice();

		~CFepSubStationDevice();

		std::string GetClassName();
	public:
		//!类名
		static std::string s_ClassName;;
	};
}

#endif