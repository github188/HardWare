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
		 *	\brief	ȱʡ���캯��
		 */
		CFepSubStationDevice();

		~CFepSubStationDevice();

		std::string GetClassName();
	public:
		//!����
		static std::string s_ClassName;;
	};
}

#endif