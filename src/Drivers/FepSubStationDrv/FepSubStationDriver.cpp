#include "FepSubStationDriver.h"
#include "Common/CommonXmlParser/DeployManager.h"

namespace MACS_SCADA_SUD
{
	//!定义驱动类类名//
	std::string CFepSubStationDriver::s_ClassName(FEPSUBSTATIONDRIVER);
	std::string CFepSubStationDriver::s_MatchDeviceClassName(FEPSUBSTATIONDEVICE);	

	CFepSubStationDriver::CFepSubStationDriver(void)
	{
		UaString sLog(UaString("%1::Construct successful! \n").arg(s_ClassName.c_str()));
		WriteLogAnyWay(sLog.toUtf8());
	}


	CFepSubStationDriver::~CFepSubStationDriver(void)
	{
		UaString sLog(UaString("%1::Destructor successful! \n").arg(s_ClassName.c_str()));
		WriteLogAnyWay(sLog.toUtf8());
	}
	//!返回驱动类类名//
	std::string CFepSubStationDriver::GetClassName()
	{
		return s_ClassName;
	}
	
	std::string CFepSubStationDriver::GetRtdbSiteName()
	{
		//从配置文件中读取实时数据库站点信息//
		std::string  rtdbSiteName = "";//"Site_RTDB";
		GetAllDbUnitId();
		if (!IsAllDbUnitFromSameRtdb(rtdbSiteName))
		{
			UaString sLog(UaString("%1::Err DbUnite is not come from same RTDB site, source proxySiteUri return null.\n").arg(s_ClassName.c_str()));
			WriteLogAnyWay(sLog.toUtf8());
			rtdbSiteName="";
		}
		return rtdbSiteName;
	}
	
	OpcUa_Int32 CFepSubStationDriver::StartWork( void )
	{
		UaString sLog(UaString("%1::StartWork enter.\n").arg(s_ClassName.c_str()));
		WriteLogAnyWay(sLog.toUtf8());

		m_serverType = HY_SERV_TYPE_FEP;
		m_proxySiteUri = GetRtdbSiteName();
		SessionConnectInfo pSessionConnectInfo;
		pSessionConnectInfo.nWatchdogTimeout = 5000;

		return CSubDataBaseDriver::StartWork(&pSessionConnectInfo,NULL,this);
	}
	
	std::string CFepSubStationDriver::GetDeviceClassName()
	{
		return s_MatchDeviceClassName;
	}

	void CFepSubStationDriver::GetAllDbUnitId()
	{
		OpcUa_Int32 nCount = GetDeviceCount();
		for (OpcUa_Int32 index = 0; index < nCount; index++)
		{
			CFepSubStationDevice* pDevice = static_cast<CFepSubStationDevice*>(GetDeviceByIndex(index));
			if (pDevice)
			{
				std::map<OpcUa_UInt16,OpcUa_UInt32>::iterator it = pDevice->m_DbUnitIdMap.begin();
				for (it; it !=  pDevice->m_DbUnitIdMap.end(); it++)
				{
					m_dbUnit.push_back(it->first);
				}
			}
		}
	}

	OpcUa_Boolean CFepSubStationDriver::IsAllDbUnitFromSameRtdb(std::string &rtdbSiteName)
	{
		if (NULL == g_pDeployManager)
		{
			return OpcUa_False;			
		}

		UaString strSite = g_pDeployManager->GetDeploySite();
		siteIndex* pSite = g_pDeployManager->getSiteIndex(strSite);
		if (NULL == pSite)
		{
			return OpcUa_False;	
		}		

		std::vector<OpcUa_UInt16>::iterator it = m_dbUnit.begin();
		OpcUa_UInt16 dbUnitId = 0;
		for (it; it !=m_dbUnit.end(); it++)
		{
			dbUnitId = *it;
			vector<DBUnitInfo>::iterator vecFepTagIt = pSite->softwareConfig.vecFepTagUnits.begin();
			for (vecFepTagIt; vecFepTagIt != pSite->softwareConfig.vecFepTagUnits.end(); vecFepTagIt++)
			{
				if (vecFepTagIt->ID == dbUnitId)
				{
					if (rtdbSiteName.empty())
					{
						rtdbSiteName = (vecFepTagIt->rtdbSiteName).toUtf8();
					}
					else
					{
						if (rtdbSiteName != (vecFepTagIt->rtdbSiteName).toUtf8())
						{
							return OpcUa_False;
						}
					}
				}
			}

		}

		return OpcUa_True;
	}

	OpcUa_Int32 CFepSubStationDriver::WriteMgrForPort(tIORunMgrCmd& tRunMgr)
	{
		return 0;
	}

	OpcUa_Int32 CFepSubStationDriver::WriteMgrForUnit(tIORunMgrCmd& tRunMgr)
	{
		return 0;
	}
	OpcUa_Int32 CFepSubStationDriver::WriteMgrForTag( tIORunMgrCmd& tRunMgr )
	{
		return 0;
	}
}