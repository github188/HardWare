#include "RTDB/Server/FEPPlatformClass/IOUnit.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"
#include "SubDataBaseDevice.h"
#include "HyServerRedundancy/switchserv.h"
#include "DiagSwitch/common.h"

namespace MACS_SCADA_SUD
{
	//!定义设备类类名//
	std::string CSubDataBaseDevice::s_ClassName(SUBDATABASEDEVICE);

	CSubDataBaseDevice::CSubDataBaseDevice() : m_pSiteSubscription(NULL), m_pSiteProxy(NULL)
	{
		ClearResource();
	}

	CSubDataBaseDevice::~CSubDataBaseDevice()
	{
		ClearResource();
	}
	void CSubDataBaseDevice::ClearResource()
	{
		m_DelMItemIdVec.clear();
		m_mapMItems.clear();
		m_DbUnitIdMap.clear();
	}

	std::string CSubDataBaseDevice::GetClassName()
	{
		return s_ClassName;
	}

	//!初始化订阅信息
	OpcUa_Int32 CSubDataBaseDevice::Init()
	{
		OpcUa_Int32 nResult = -1;

		//!初始化订阅监视项请求信息
		if (m_pIOUnit != NULL)
		{
			OpcUa_Int32 nCount = m_pIOUnit->GetVariableCount();
			if (nCount > 0)
			{
				m_Requests.create(nCount);
				std::vector<UaVariable*> vecTags;
				m_pIOUnit->GetAllTags(vecTags);
				for (OpcUa_Int32 nIndex = 0; nIndex < nCount; nIndex++)
				{
					UaVariable* pUaVariable = vecTags[nIndex];
					UaNodeId nodeId = pUaVariable->nodeId();
					OpcUa_NodeId nodeId_OpcUa;
					nodeId.copyTo(&nodeId_OpcUa);
					OpcUa_NodeId_CopyTo(&nodeId_OpcUa, &m_Requests[nIndex].ItemToMonitor.NodeId);					
					m_Requests[nIndex].ItemToMonitor.AttributeId = OpcUa_Attributes_Value;
					m_Requests[nIndex].RequestedParameters.ClientHandle = nIndex;
					m_Requests[nIndex].RequestedParameters.SamplingInterval = GetSamplingInterVal(pUaVariable);
					m_Requests[nIndex].RequestedParameters.QueueSize = 1;
					m_Requests[nIndex].RequestedParameters.DiscardOldest = OpcUa_True;
					m_Requests[nIndex].MonitoringMode = OpcUa_MonitoringMode_Reporting;

					m_DelMItemIdVec.push_back((OpcUa_UInt32)nIndex);
					m_mapMItems.insert(T_MAPMItems::value_type((OpcUa_UInt32)nIndex, pUaVariable));


					//FEP子站订阅实时库使用
					OpcUa_UInt32 times = 0;
					OpcUa_UInt16 dbUniteId = 0;
					OpcUa_UInt16 isExist = 0;
					dbUniteId = nodeId_OpcUa.NamespaceIndex;
					isExist = m_DbUnitIdMap.count(dbUniteId);
					if (1 == isExist) 
					{
						times = m_DbUnitIdMap[dbUniteId];
						times += 1;
						m_DbUnitIdMap[dbUniteId] = times;
					}
					else
					{
						m_DbUnitIdMap[dbUniteId] = 1;
					}


					OpcUa_NodeId_Clear(&nodeId_OpcUa); //!释放资源
				}
				nResult = 0;
			}	

			UaString slog(UaString("%1::Init Create %2 monitor items,recode %3\n").arg(s_ClassName.c_str()).arg(nCount).arg(nResult));
			WriteLogAnyWay(slog.toUtf8());
		}

		return nResult;
	}

	OpcUa_Int32 CSubDataBaseDevice::InitVariableInfo( CIOUnit* pIOUnit )
	{
		//暂不做操作//
		OpcUa_Int32 nResult = 0;

		return nResult;
	}

	void CSubDataBaseDevice::subscriptionStatusChanged( OpcUa_UInt32 clientSubscriptionHandle, const UaStatus& status )
	{
		if (status.isBad())
		{
			UaString sErr(UaString("CSubDataBaseDevice:: DBUnit: %1 Subscription no longer valid - failed with status: %2").arg(GetName().c_str()).arg(UaStatus(status).toString().toUtf8()));
			WriteLogAnyWay(sErr.toUtf8());
		}
	}

	void CSubDataBaseDevice::dataChange( OpcUa_UInt32 clientSubscriptionHandle, const UaDataNotifications& dataNotifications, const UaDiagnosticInfos& diagnosticInfos )
	{
		if (!IsHot())
		{
			//从机不做数据变化更新，仅从主机同步数据//
			return;
		}
		
		for (OpcUa_UInt32 index = 0; index < dataNotifications.length(); index++)
		{
			OpcUa_UInt32 uHandle = dataNotifications[index].ClientHandle;
			UaVariable* pDataVariable = m_mapMItems[uHandle];
			if (OpcUa_IsGood(dataNotifications[index].Value.StatusCode) && NULL != pDataVariable)
			{
				UaDataValue dataValue(dataNotifications[index].Value);
				pDataVariable->setValue(NULL, dataValue, OpcUa_False);
				CIODriver* pDriver = (CIODriver*)GetDriver();
				if (NULL != pDriver && pDriver->IsLog())
				{
					UaDateTime sourceTime(dataValue.sourceTimestamp());
					UaDateTime updateTime = UaDateTime::now();
					UaDateTime tempTime;
					OpcUa_Int32 nUpdateConstMsec = tempTime.msecsTo(updateTime) - tempTime.msecsTo(sourceTime);
					OpcUa_CharA chLogMsg[DEBUGINFOLEN];
					snprintf(chLogMsg, DEBUGINFOLEN, "CSubDataBaseDevice::DataChange,%d , %s", nUpdateConstMsec, pDataVariable->nodeId().toString().toUtf8());
					pDriver->WriteLog(chLogMsg);
				}
			}
			else
			{
				UaStatus itemErr(dataNotifications[index].Value.StatusCode);
				UaString sErr(UaString("CSubDataBaseDevice::Err DBUnit:%1 dataChange failed NodeId:%2 with status:%3").arg(GetName().c_str()).
					arg(pDataVariable->nodeId().toString().toUtf8()).arg(itemErr.toString().toUtf8()));
				WriteLogAnyWay(sErr.toUtf8());
			}
		}

	}

	UaStatus CSubDataBaseDevice::subscribe(CSiteProxy* pSiteProxy)
	{
		UaStatus result = OpcUa_BadInvalidState;
		if (pSiteProxy)
		{
			result = createSubscription(pSiteProxy);
			if (result.isGood())
			{
				result = createMonitoredItems();
			}
		}
		return result;
	}

	UaStatus CSubDataBaseDevice::createSubscription(CSiteProxy* pSiteProxy)
	{
		UaStatus result = OpcUa_BadInvalidState;
		if (NULL != m_pSiteSubscription)
		{
			UaString sErr(UaString("CSubDataBaseDevice::Err DBUnit: %1 Subscription already created!\n").arg(GetName().c_str()));
			WriteLogAnyWay(sErr.toUtf8());
			return result;
		}

		if (pSiteProxy)
		{
			m_pSiteProxy = pSiteProxy;
			ServiceSettings serviceSettings;
			SubscriptionSettings subscriptionSettings;
			subscriptionSettings.publishingInterval = 100;
			result = pSiteProxy->createSubscription(serviceSettings, this, 1, subscriptionSettings, OpcUa_True, &m_pSiteSubscription);
			if (result.isGood())
			{
				UaString sLog(UaString("CSubDataBaseDevice:: DBUnit: %1 CreateSubscription successful!\n").arg(GetName().c_str()));
				WriteLogAnyWay(sLog.toUtf8());
			}
			else
			{
				UaString sLog(UaString("CSubDataBaseDevice:: DBUnit: %1 CreateSubscription failed with status %2 \n").arg(GetName().c_str()).arg(result.toString().toUtf8()));
				WriteLogAnyWay(sLog.toUtf8());
			}
		}
		return result;
	}

	UaStatus CSubDataBaseDevice::createMonitoredItems()
	{
		UaStatus result = OpcUa_BadInvalidState;
		OpcUa_Int32 nItemsCount = (OpcUa_Int32)m_Requests.length();
		if (m_pSiteSubscription && nItemsCount> 0)
		{
			ServiceSettings serviceSettings;
			UaMonitoredItemCreateResults createResults;

			clock_t start, end;
			start = clock();
			result = m_pSiteSubscription->createMonitoredItems(serviceSettings,
				                                               OpcUa_TimestampsToReturn_Both,
				                                               m_Requests,
				                                               createResults);
			end = clock();
			
			long loff = end - start;
			char cLog[256];
			snprintf(cLog, 256, "CSubDataBaseDevice:: DBUnit: %s createMonitorItems count: %d using: %d ms\n",GetName().c_str(), nItemsCount, loff);
			printf(cLog);
			WriteLogAnyWay(cLog);
			if (result.isBad())
			{
				UaString sErr(UaString("CSubDataBaseDevice::Err DBUnit: %s Create MonitoredItems failed with status: %1\n").arg(GetName().c_str()).arg(result.toString().toUtf8()));
				WriteLogAnyWay(sErr.toUtf8());
			}
		}
		else
		{
			result = OpcUa_BadInvalidState;
			UaString sErr(UaString("CSubDataBaseDevice::Err DBUnit: %s Create MonitoredItems failed with status: %1.because monitor item is 0 or m_pSiteSubscription is null. \n").arg(GetName().c_str()).arg(result.toString().toUtf8()));
			WriteLogAnyWay(sErr.toUtf8());
			
		}
		return result;
	}

	//!停止订阅
	UaStatus CSubDataBaseDevice::unsubscribe()
	{
		UaStatus result = OpcUa_BadInvalidState;
		if (NULL == m_pSiteSubscription || NULL == m_pSiteProxy)
		{
			return result;
		}

		OpcUa_Int32 nCount = m_DelMItemIdVec.size();
		if ( nCount > 0)
		{
			ServiceSettings monitorServiceSettings;
			UaStatusCodeArray delResults;
			UaUInt32Array monitoredItemIds;
			monitoredItemIds.create(nCount);
			for (OpcUa_Int32 nIndex = 0; nIndex < nCount; nIndex++)
			{
				monitoredItemIds[nIndex] = m_DelMItemIdVec[nIndex];
			}

			//!删除订阅监视项
			result = m_pSiteSubscription->deleteMonitoredItems(monitorServiceSettings, monitoredItemIds, delResults);

			if(result.isBad())
			{
				UaString sErr(UaString("CSubDataBaseDevice::UADevice: unsubscribe:deleteMonitoredItems failed Status %1 \n").arg(UaStatus(result).toString().toUtf8()));
				WriteLogAnyWay(sErr.toUtf8());
			}
			monitoredItemIds.clear();
			m_DelMItemIdVec.clear();
			m_mapMItems.clear();
		}

		//!删除订阅
		ServiceSettings subServiceSettings;
		result = m_pSiteProxy->deleteSubscription(subServiceSettings, &m_pSiteSubscription);
		if (result.isGood() )
		{
			UaString sLog(UaString("CSubDataBaseDevice:: DBUnit:%1 unsubscribe:deleteSubscription Successsful!").arg(GetName().c_str()));
			WriteLogAnyWay(sLog.toUtf8());
		} 
		else
		{
			UaString sErr(UaString("CSubDataBaseDevice:: DBUnit:%1 unsubscribe:deleteSubscription failed ret=%2").arg(GetName().c_str()).arg(result.toString().toUtf8()));
			WriteLogAnyWay(sErr.toUtf8());
		}
		m_pSiteSubscription = NULL;

		return result;
	}


	//!触发事件
	void CSubDataBaseDevice::newEvents( OpcUa_UInt32 clientSubscriptionHandle, UaEventFieldLists& eventFieldList )
	{
		return;
	}

	OpcUa_Double CSubDataBaseDevice::GetSamplingInterVal( UaVariable* pUaVariable )
	{
		OpcUa_Double nres = 500;
		if (pUaVariable)
		{
			UaDataValue uaInternalValue;
			pUaVariable->getAttributeValue(NULL, OpcUa_Attributes_MinimumSamplingInterval, uaInternalValue);
			OpcUa_Double dSamValue;
			UaVariant varSam = (*uaInternalValue.value());
			varSam.toDouble(dSamValue);
			nres = dSamValue;
		}
		return nres;
	}

	OpcUa_Int32 CSubDataBaseDevice::WriteLogAnyWay( const OpcUa_CharA* pchLog, bool bTime/*=true */ )
	{
		OpcUa_Int32 reCode = -1;
		CIODriver* pDriver = (CIODriver*)GetDriver();
		if (NULL != pDriver)
		{
			pDriver->WriteLogAnyWay(pchLog,bTime);
			reCode = 0;
		}
		return reCode;
	}

	void CSubDataBaseDevice::Read()
	{
		UaStatus result = OpcUa_BadInvalidState;
		CIODriver* pDriver = (CIODriver*)GetDriver();
		if (NULL == m_pSiteSubscription || NULL == m_pSiteProxy || NULL == pDriver)
		{
			UaString sLog(UaString("CSubDataBaseDevice::Read failed，because m_pSiteSubscription or m_pSiteProxy is null!"));
			pDriver->WriteLogAnyWay(sLog.toUtf8());
			return;
		}

		OpcUa_Int32 nCount = m_DelMItemIdVec.size();
		if ( nCount <= 0 || nCount != m_Requests.length())
		{
			UaString sLog(UaString("CSubDataBaseDevice::Not need read，because m_Requests is empty or delete monitor total is not eq add monitor!"));
			pDriver->WriteLogAnyWay(sLog.toUtf8());
			return;
		}

		//如果要删订阅就不需要删除监视项//
		ServiceSettings subServiceSettings;
		result = m_pSiteProxy->deleteSubscription(subServiceSettings, &m_pSiteSubscription);
		if (result.isBad())
		{
			UaString sErr(UaString("CSubDataBaseDevice::Read  failed because deleteSubscription failed."));
			pDriver->WriteLogAnyWay(sErr.toUtf8());
			return;
		}
		m_pSiteSubscription = NULL;
		result = subscribe(m_pSiteProxy);
		if (result.isBad())
		{
			UaString sErr(UaString("CSubDataBaseDevice::Read call subscribe failed."));
			pDriver->WriteLogAnyWay(sErr.toUtf8());
		}
		else
		{
			UaString sErr(UaString("CSubDataBaseDevices::Read call subscribe Successful."));
			pDriver->WriteLog(sErr.toUtf8());
		}
	}
}
