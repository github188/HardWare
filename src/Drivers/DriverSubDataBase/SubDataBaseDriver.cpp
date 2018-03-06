#include "SubDataBaseDriver.h"
#include "Common/DataChange/DataTrans.h"
#include "Common/OS_Ext/ShowDbgInfo.h"
#include "Common/CommonXmlParser/DeployManager.h"
#include "HlyEventBase/EventCategoryConstant.h"
#include "RTDB/Server/DBPlatformClass/ComponentType.h"
#include "RTDB/Server/DBFrameAccess/DBFrameAccess.h"
#include "HyServerRedundancy/switchserv.h"
#include "DiagSwitch/common.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"
#include "RTDB/Server/FEPPlatformClass/IOUnit.h"

namespace MACS_SCADA_SUD
{
	//!��������������//
	std::string CSubDataBaseDriver::s_ClassName(SUBDATABASEDRIVER);
	std::string CSubDataBaseDriver::s_MatchDeviceClassName(SUBDATABASEDEVICE);	

	CSubDataBaseDriver::CSubDataBaseDriver(void) : m_pSiteProxy(NULL), m_bIsConnected(OpcUa_False),m_proxySiteUri(""),m_serverType(-1)
	{

	}

	CSubDataBaseDriver::~CSubDataBaseDriver(void)
	{
		string m_proxySiteUri = "";
		m_serverType = -1;
	}

	OpcUa_Int32 CSubDataBaseDriver::OpenChannel()
	{
		return CIODriver::OpenChannel();
	}

	//!�Ͽ�Session����
	OpcUa_Int32 CSubDataBaseDriver::CloseChannel()
	{
		return CIODriver::CloseChannel();
	}

	//!��������������//
	std::string CSubDataBaseDriver::GetClassName()
	{
		return s_ClassName;
	}

	OpcUa_Int32 CSubDataBaseDriver::StartWork(SessionConnectInfo* pSessionConnectInfo,	SessionSecurityInfo* pSessionSecurityInfo,UaSessionCallback* pCallback )
	{
		OpcUa_Int32 nResult = -1;
		//!�������Ӵ���
		if (NULL == m_pSiteProxy)
		{
			if (NULL != g_pProxyMgr)
			{
				UaString usURI(m_proxySiteUri.c_str());
				m_pSiteProxy = g_pProxyMgr->CreateSiteProxy(usURI, m_serverType, NULL,NULL,pCallback);
				if (m_pSiteProxy)
				{
					nResult = 0;
					m_bIsConnected = OpcUa_True;
					UaString sLog(UaString("CSubDataBaseDriver::StartWork create siteProxy: %1 successful! \n").arg(usURI.toUtf8()));
					WriteLogAnyWay(sLog.toUtf8());
				}
				else
				{
					m_bIsConnected = OpcUa_False;
					SetDeviceStatue(DEVICE_OFFLINE);
					UaString sLog(UaString("CSubDataBaseDriver::StartWork create siteProxy: %1 failed! \n").arg(usURI.toUtf8()));
					WriteLogAnyWay(sLog.toUtf8());
				}
			}
		}
		//!�������ĺͶ��ļ�����
		//UaThread::msleep(5000);
		if (NULL != m_pSiteProxy)
		{
			for (OpcUa_Int32 nIndex = 0; nIndex < GetDeviceCount(); nIndex++)
			{
				CSubDataBaseDevice* pDevice = (CSubDataBaseDevice*)GetDeviceByIndex(nIndex);
				UaStatus status = pDevice->subscribe(m_pSiteProxy);
				if (status.isGood())
				{
					nResult = 0;
				}
				else
				{
					nResult = -1;
					UaString sLog(UaString("CSubDataBaseDriver::StartWork device:%1 subscribe failed!").arg(pDevice->GetName().c_str()));
					WriteLogAnyWay(sLog.toUtf8());
				}
			}
		}
		return nResult;
	}

	//!ֹͣ����
	OpcUa_Int32 CSubDataBaseDriver::StopWork( void )
	{
		OpcUa_Int32 nResult = -1;
		//!ȡ������
		for (OpcUa_Int32 nIndex = 0; nIndex < GetDeviceCount(); nIndex++)
		{
			CSubDataBaseDevice* pDevice = (CSubDataBaseDevice*)GetDeviceByIndex(nIndex);
			UaStatus status = pDevice->unsubscribe();
			if (status.isGood())
			{
				nResult = 0;
			}
			else
			{
				nResult = -1;
				UaString sLog(UaString("CSubDataBaseDrivers::Err device:%1 unsubscribe failed!").arg(pDevice->GetName().c_str()));
				WriteLog(sLog.toUtf8());
			}
		}

		//!ɾ�����Ӵ���
		if (0 == nResult && m_pSiteProxy)
		{
			ServiceSettings serviceSettings;
			UaStatus status = m_pSiteProxy->disconnect(serviceSettings, OpcUa_True);
			if (status.isGood())
			{
				nResult = 0;
				m_bIsConnected = OpcUa_False;
				UaString sLog(UaString("CSubDataBaseDriver::StopWork disconnect Session:%1 success.\n").arg(GetDriverName().c_str()));
				WriteLog(sLog.toUtf8());
				if (m_pSiteProxy)
				{
					delete m_pSiteProxy;
					m_pSiteProxy = NULL;
				}
			} 
			else
			{
				UaString sLog(UaString("CSubDataBaseDriver::StopWork disconnect Session:%1 failed.\n").arg(GetDriverName().c_str()));
				WriteLog(sLog.toUtf8());
				nResult = -1;
			}
		}
		return nResult;
	}

	std::string CSubDataBaseDriver::GetDeviceClassName()
	{
		return s_MatchDeviceClassName;
	}
	//!Session״̬�����仯
	void CSubDataBaseDriver::connectionStatusChanged( OpcUa_UInt32 clientConnectionId, UaClient::ServerStatus serverStatus )
	{
		OpcUa_ReferenceParameter(clientConnectionId);
		EXTERNAL_EVENT eventType = EXTERNAL_SOE;
		OpcUa_Int32 eventValue = -1;

		switch(serverStatus)
		{
		case UaClient::Disconnected:
		case UaClient::ConnectionErrorApiReconnect:
		case UaClient::ServerShutdown:
			{
				UaString sLog(UaString("CSubDataBaseDriver::The DataSource %1 connection status changed to Disconnected!\n").arg(GetDriverName().c_str()));
				WriteLogAnyWay(sLog.toUtf8());
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CSubDataBaseDriver::The DataSource %s connection status changed to Disconnected!\n\n"), GetDriverName().c_str()));

				m_bIsConnected = OpcUa_False;		//!��������״̬
				SetDBUnitStatus(HY_QUALITY_BAD);	//!�������ݿⵥԪ�û�
				SetDeviceStatue(DEVICE_OFFLINE);	//!�����豸״̬	

				eventType = FEP_DATASOURCE_OFFLINE;
				eventValue = (OpcUa_Int32)(DEVICE_OFFLINE);
			}
			break;
		case UaClient::Connected:
			{
				UaString sLog(UaString("CSubDataBaseDriver::The DataSource %1 connection status changed to Connected!\n").arg(GetDriverName().c_str()));
				WriteLogAnyWay(sLog.toUtf8());
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CSubDataBaseDriver::The DataSource %s connection status changed to Connected!\n\n"), GetDriverName().c_str()));

				m_bIsConnected = OpcUa_True;	//!��������״̬
				SetDBUnitStatus(HY_QUALITY_GOOD);	//!�������ݿⵥԪ�û�
				SetDeviceStatue(DEVICE_ONLINE);		//!�����豸״̬	

				//��statusCodeΪ0ʱ��ʵʱ�����վ����������ʱ�����ȷ��Ͷ�����Ϣ��//
				//���ŷ���connected��Ϣ��������վ�����Ὣ�Լ�StatusCode��Ϊ1����FEPΪ0������ȥ��ȡһ������//
				ReadOneTimeData();
			}
			break;
		case UaClient::ConnectionWarningWatchdogTimeout:
			{
				UaString sLog(UaString("CSubDataBaseDriver::The DataSource %1 connection status changed to Timeout!\n").arg(GetDriverName().c_str()));
				WriteLogAnyWay(sLog.toUtf8());
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CSubDataBaseDriver::The DataSource %s connection status changed to Timeout!\n"), GetDriverName().c_str()));
			}
			break;
		case UaClient::NewSessionCreated:
			{
				UaString sLog(UaString("CSubDataBaseDriver::The DataSource %1 connection status changed to NewSessionCreated!\n").arg(GetDriverName().c_str()));
				WriteLogAnyWay(sLog.toUtf8());
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CSubDataBaseDriver::The DataSource %s connection status changed to NewSessionCreated!\n"), GetDriverName().c_str()));
			}
			break;  
		default:
			break;
		}

		if (FEP_DATASOURCE_OFFLINE == eventType )
		{
			OpcUa_Int64 nDateTime = OpcUa_Int64(UaDateTime::now());
			CDBFrameAccess::FireUAEvent(this->GetIOPort(), eventType, eventValue, nDateTime);
		}

	}

	//!�õ�Session����
	CSiteProxy* CSubDataBaseDriver::GetSession()
	{
		if (m_pSiteProxy)
		{
			return m_pSiteProxy;
		}
		return NULL;
	}

	//!ͨ��CSubDataBaseDriverʵ�ֱ�ǩ��ǿ�ƺͽ�ֹ
	OpcUa_Int32 CSubDataBaseDriver::WriteMgrForTag( tIORunMgrCmd& tRunMgr )
	{
   	    OpcUa_Int32 nRes = -1;
		return nRes;
	}

	OpcUa_Int32 CSubDataBaseDriver::WriteDCB( tIORemoteCtrl& tRemote )
	{
		OpcUa_Int32 redCode = -1;
		return redCode;
	}

	OpcUa_Int32 CSubDataBaseDriver::ProcessRunMgr(tIORunMgrCmd& tRunMgr, CallIn& callRequest)
	{
		OpcUa_Int32 redCode = -1;
		return redCode;
	}

	CSiteProxy* CSubDataBaseDriver::GetSiteProxy()
	{
		if (m_pSiteProxy)
		{
			return m_pSiteProxy;
		}
		else
		{
			return NULL;
		}
	}

	void CSubDataBaseDriver::SetDBUnitStatus(OpcUa_Byte byQuality)
	{
		OpcUa_Int32 nCount = GetDeviceCount();
		for (OpcUa_Int32 index = 0; index < nCount; index++)
		{
			CIODeviceBase* pDevice = GetDeviceByIndex(index);
			if (pDevice)
			{
				OpcUa_Int16 unNo = (OpcUa_Int16)pDevice->GetAddr();
				CSwitchServ::SetQuality(unNo,byQuality);
			}
		}
	}
	void CSubDataBaseDriver::SetDeviceStatue( OpcUa_Byte statue )
	{
		OpcUa_Int32 nCount = GetDeviceCount();
		for (OpcUa_Int32 index = 0; index < nCount; index++)
		{
			CIODeviceBase* pDevice = GetDeviceByIndex(index);
			if (pDevice)
			{
				pDevice->SetOnLine(statue);
			}
		}
	}

	void CSubDataBaseDriver::ReadOneTimeData()
	{	
		OpcUa_CharA pchLog[DEBUGINFOLEN] = {0};
		OpcUa_Int32 nCount = GetDeviceCount();
		for (OpcUa_Int32 index = 0; index < nCount; index++)
		{
			CSubDataBaseDevice* pDevice = static_cast<CSubDataBaseDevice*>(GetDeviceByIndex(index));
			if (NULL == pDevice || !pDevice->IsHot() )
			{
				continue;
			}

			if (m_bIsConnected)
			{
				pDevice->Read();

				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CSubDataBaseDriver::ReadOneTimeData Dev:%s Read a new Frame Data form RTDB\n"), pDevice->GetName().c_str()));
				snprintf(pchLog, DEBUGINFOLEN, "CSubDataBaseDriver::ReadOneTimeData Dev:%s Read a new Frame Data form RTDB\n", pDevice->GetName().c_str());
				WriteLogAnyWay(pchLog);	

			}
			else
			{
				CIOUnit* pUnit = pDevice->GetIOUnitNode();
				if (NULL != pUnit)
				{
					pUnit->SetOffline(OpcUa_False);

					MACS_ERROR((ERROR_PERFIX ACE_TEXT("CSubDataBaseDriver::ReadOneTimeData The Connection is Bad Set Dev:%s Offline!\n"), pDevice->GetName().c_str()));						
					snprintf(pchLog, DEBUGINFOLEN, "CSubDataBaseDriver::ReadOneTimeData The Connection is Bad Set Dev:%s Offline!\n", pDevice->GetName().c_str());
					WriteLogAnyWay(pchLog);
				}
			}
		}
	}
}