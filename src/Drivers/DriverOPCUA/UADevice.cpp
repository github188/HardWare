/*!
 *	\file	UADevice.cpp
 *
 *	\brief	UA Device��ʵ��
 *
 *	�������Ĳ���������Ϣ
 *
 *	\author LIFAN
 *
 *	\date	2014��5��21��	15:45
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "RTDB/Server/FEPPlatformClass/IOUnit.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"
#include "UADevice.h"
#include "HyServerRedundancy/switchserv.h"
#include "DiagSwitch/common.h"

namespace MACS_SCADA_SUD
{
	//!�����豸������
	std::string CUADevice::s_ClassName("CUADevice");
	

	CUADevice::CUADevice() : m_pSiteSubscription(NULL), m_pSiteProxy(NULL)
	{
		m_MonitoredItemIds.clear();
		m_mapMItems.clear();
	}

	CUADevice::~CUADevice()
	{
		m_MonitoredItemIds.clear();
	}

	//!��ʼ��������Ϣ
	OpcUa_Int32 CUADevice::Init()
	{
		OpcUa_Int32 nResult = -1;
		CIODriver* pDriver = (CIODriver*)GetDriver();
		//!��ǰUADriver��·�Ƿ�Ϊ��
		OpcUa_Boolean bIsHot = IsHot();		
		//!��ʼ�����ļ�����������Ϣ
		if (m_pIOUnit != NULL && NULL != pDriver)
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
					m_Requests[nIndex].RequestedParameters.QueueSize = pDriver->getQueueSize();
					m_Requests[nIndex].RequestedParameters.DiscardOldest = OpcUa_True;
					if (bIsHot)
					{
						m_Requests[nIndex].MonitoringMode = OpcUa_MonitoringMode_Reporting;
					}
					else
					{
						m_Requests[nIndex].MonitoringMode = OpcUa_MonitoringMode_Sampling;
					}
					//������ٷֱ�����
					CreateDataChangeFilter(&(m_Requests[nIndex].RequestedParameters.Filter), pUaVariable);
					m_mapMItems.insert(T_MAPMItems::value_type((OpcUa_UInt32)nIndex, pUaVariable));
					OpcUa_NodeId_Clear(&nodeId_OpcUa); //!�ͷ���Դ
				}
				nResult = 0;
			}	
		}

		return nResult;
	}

	//!��д����InitVariableInfo,UADriver����Ҫ��ʼ��������Ϣ
	OpcUa_Int32 CUADevice::InitVariableInfo( CIOUnit* pIOUnit )
	{
		return 0;
	}

	//!����״̬�����仯
	void CUADevice::subscriptionStatusChanged( OpcUa_UInt32 clientSubscriptionHandle, const UaStatus& status )
	{
		if (status.isBad())
		{
			UaString sErr(UaString("UADriver:: DBUnit: %1 Subscription no longer valid - failed with status: %2").arg(GetName().c_str()).arg(UaStatus(status).toString().toUtf8()));
 			CIODriver* pDriver = (CIODriver*)GetDriver();
 			pDriver->WriteLogAnyWay(sErr.toUtf8());
		}
	}

	//!���ݸ���
	void CUADevice::dataChange( OpcUa_UInt32 clientSubscriptionHandle, const UaDataNotifications& dataNotifications, const UaDiagnosticInfos& diagnosticInfos )
	{
		//!UADriver�����ݸ�������·��������Ϊ���ݸ��µ�����
		//MACS_ERROR((ERROR_PERFIX ACE_TEXT("CUADevice::dataChange Len:%d IsHot:%d\n"), dataNotifications.length(), IsHot()));
		if (IsHot())
		{
 			OpcUa_UInt32 index = 0;
			OpcUa_UInt32 nextIndex = 0;
			OpcUa_Int16 timeCompareFlag = 0;
			OpcUa_UInt32 uCurHandle = 0;
			OpcUa_UInt32 uNextHandle = 0;			
			OpcUa_DateTime fieldPointLastDateTime;
			memset(&fieldPointLastDateTime,0, sizeof(OpcUa_DateTime));		
			OpcUa_UInt32 dataNotificationsItem = dataNotifications.length();
			CIODriver* pDriver = (CIODriver*)GetDriver();
			if (NULL == pDriver)
			{
				return;
			}

			OpcUa_CharA chLogMsg[DEBUGINFOLEN] = {0};

			for (index = 0; index < dataNotificationsItem; index++)
			{
				nextIndex = (index+1)%dataNotificationsItem;
				uCurHandle = dataNotifications[index].ClientHandle;
				uNextHandle = dataNotifications[nextIndex].ClientHandle;
				UaVariable *pDataVariable = (m_mapMItems[uCurHandle]);
				if (OpcUa_IsGood(dataNotifications[index].Value.StatusCode))
				{
					if (NULL != pDataVariable)
					{
						fieldPointLastDateTime.dwHighDateTime = pDataVariable->value(NULL).sourceTimestamp().dwHighDateTime;
						fieldPointLastDateTime.dwLowDateTime = pDataVariable->value(NULL).sourceTimestamp().dwLowDateTime;

						UaDataValue dataValue(dataNotifications[index].Value);

						UaVariant varVal = *(dataValue.value());

						timeCompareFlag = pDriver->OpcUaDateTimeCompare(dataValue.sourceTimestamp(),fieldPointLastDateTime);
						if ((timeCompareFlag < 1) && (uCurHandle == uNextHandle)  && (0 != nextIndex) ) 
						{
							//�����ǰ���������ݶ����ж�ͬһ��ClientHandle�����Ҹ�����ʱ�����ڵ�ǰPV��ʱ��
							//���Ҹ����ݲ��Ƕ��������һ�������򲻽������ݸ���
							//snprintf(chLogMsg, DEBUGINFOLEN, "UADriver::DataChange Receive OverTimeVal: Value:%s , NodeId:%s", varVal.toString().toUtf8(), pDataVariable->nodeId().toFullString().toUtf8());
							//pDriver->WriteLogAnyWay(chLogMsg);
							continue;
						}
						UaStatus statusCode = pDataVariable->setValue(NULL, dataValue, OpcUa_False);

						//UaVariant varVal = *(dataValue.value());
						OpcUa_Float fValue = 0;
						fValue = GetVarFloatVal(varVal);
						if (statusCode.isGood())
						{
							if (pDriver->IsLog())
							{
								snprintf(chLogMsg, DEBUGINFOLEN, "UADriver::DataChange Receive: Value:%s , NodeId:%s", varVal.toString().toUtf8(), pDataVariable->nodeId().toFullString().toUtf8());
								pDriver->WriteLog(chLogMsg);
							}
						}
						else
						{
							snprintf(chLogMsg, DEBUGINFOLEN, "UADriver::DataChange Failed!, Value:%s , NodeId:%s", varVal.toString().toUtf8(), pDataVariable->nodeId().toFullString().toUtf8());
							pDriver->WriteLogAnyWay(chLogMsg);
						}
					}
				}
				else
				{
					UaStatus itemErr(dataNotifications[index].Value.StatusCode);
					UaString sErr(UaString("CUADevice:: DBUnit:%1 dataChange failed NodeId:%2 with status:%3").arg(GetName().c_str()).
						arg(pDataVariable->nodeId().toString().toUtf8()).arg(itemErr.toString().toUtf8()));
					CIODriver* pDriver = (CIODriver*)GetDriver();
					pDriver->WriteLogAnyWay(sErr.toUtf8());
				}
			}
		}
	}

	//!�����豸������
	std::string CUADevice::GetClassName()
	{
		return s_ClassName;
	}

	//!��������
	UaStatus CUADevice::createSubscription( CSiteProxy* pSiteProxy )
	{
		UaStatus result;

		if (m_pSiteSubscription != NULL)
		{
			UaString sErr(UaString("CUADevice::Error: DBUnit: %1 Subscription already created!\n").arg(GetName().c_str()));
			CIODriver* pDriver = (CIODriver*)GetDriver();
			pDriver->WriteLogAnyWay(sErr.toUtf8());
			return OpcUa_BadInvalidState;
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
				UaString sLog(UaString("UADevice:: DBUnit: %1 CreateSubscription successful!\n").arg(GetName().c_str()));
				CIODriver* pDriver = (CIODriver*)GetDriver();
				pDriver->WriteLogAnyWay(sLog.toUtf8());
			}
			else
			{
				m_pSiteProxy = NULL;
				UaString sLog(UaString("UADevice:: DBUnit: %1 CreateSubscription failed with status %2 \n").arg(GetName().c_str()).arg(result.toString().toUtf8()));
				CIODriver* pDriver = (CIODriver*)GetDriver();
				pDriver->WriteLogAnyWay(sLog.toUtf8());
			}
		}
		return result;
	}

	//!�������ļ�����
	UaStatus CUADevice::createMonitoredItems()
	{
		UaStatus result;
		if (m_pSiteSubscription)
		{
			ServiceSettings serviceSettings;
			UaMonitoredItemCreateResults createResults;

			clock_t start, end;
			start = clock();
			result = m_pSiteSubscription->createMonitoredItems(
					serviceSettings,
					OpcUa_TimestampsToReturn_Both,
					m_Requests,
					createResults
				);
			end = clock();
			OpcUa_Int32 nItemsCount = (OpcUa_Int32)m_Requests.length();
			long loff = end - start;
			char cLog[256];
			snprintf(cLog, 256, "CUADevice:: DBUnit: %s createMonitorItems count: %d using: %d ms\n",GetName().c_str(), nItemsCount, loff);
			printf(cLog);
			CIODriver* pDriver = (CIODriver*)GetDriver();
			if (NULL == pDriver)
			{
				return result;
			}
			pDriver->WriteLogAnyWay(cLog);
			if (result.isBad())
			{
				UaString sErr(UaString("UADevice:: DBUnit: %s Create MonitoredItems failed with status: %1\n").arg(GetName().c_str()).arg(result.toString().toUtf8()));
				CIODriver* pDriver = (CIODriver*)GetDriver();
				pDriver->WriteLogAnyWay(sErr.toUtf8());
			}
			else
			{
				int monitorCount = createResults.length();
				m_MonitoredItemIds.create(monitorCount);
				for (OpcUa_UInt32 i = 0; i < monitorCount; i++)
				{
					OpcUa_MonitoredItemCreateResult monitoredItemCreateResult = createResults[i];
					m_MonitoredItemIds[i] = monitoredItemCreateResult.MonitoredItemId;			
					if (0 != monitoredItemCreateResult.StatusCode)
					{
						memset(cLog, 0, 256);
						snprintf(cLog, 256, "CUADevice::createMonitoredItems[%d] failed return %d.",i, monitoredItemCreateResult.StatusCode);
						pDriver->WriteLogAnyWay(cLog);
					}
				}
			}
		}
		else
		{
			result = OpcUa_BadInvalidState;
		}

		return result;
	}

	//!��ʼ����
	UaStatus CUADevice::subscribe(CSiteProxy* pSiteProxy)
	{
		UaStatus result;
		
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

	void CUADevice::newEvents( OpcUa_UInt32 clientSubscriptionHandle, UaEventFieldLists& eventFieldList )
	{
		
	}

	//!ȡ������
	UaStatus CUADevice::unsubscribe()
	{
		UaStatus result;
		if (NULL == m_pSiteSubscription || NULL == m_pSiteProxy)
		{
			result = OpcUa_BadInvalidState;
		}

		CIODriver* pDriver = (CIODriver*)GetDriver();
		OpcUa_Int32 nCount = m_MonitoredItemIds.length();
		if ( nCount > 0)
		{
			ServiceSettings serviceSettings;
			UaStatusCodeArray delResults;
			//!ɾ�����ļ�����
			result = m_pSiteSubscription->deleteMonitoredItems(serviceSettings, m_MonitoredItemIds, delResults);
			
			if(result.isBad() && pDriver->IsLog())
			{
				UaString sErr(UaString("UADriver::UADevice: unsubscribe:deleteMonitoredItems failed Status %1 \n").arg(UaStatus(result).toString().toUtf8()));
 				pDriver->WriteLogAnyWay(sErr.toUtf8());
			}
			m_MonitoredItemIds.clear();
			m_mapMItems.clear();
		}
		
		//!ɾ������
		if (m_pSiteProxy && m_pSiteSubscription)
		{
			ServiceSettings serviceSettings;
			result = m_pSiteProxy->deleteSubscription(serviceSettings, &m_pSiteSubscription);
			if (result.isGood()) //Mod for Bug61858 on 20151202
			{
				UaString sLog(UaString("UADriver::UADevice DBUnit:%1 unsubscribe:deleteSubscription Successsful!").arg(GetName().c_str()));
 				pDriver->WriteLogAnyWay(sLog.toUtf8());
			} 
			else
			{
				UaString sErr(UaString("UADriver::UADevice DBUnit:%1 unsubscribe:deleteSubscription failed ret=%2").arg(GetName().c_str()).arg(result.toString().toUtf8()));
 				CIODriver* pDriver = (CIODriver*)GetDriver();
 				pDriver->WriteLogAnyWay(sErr.toUtf8());
 			}
			m_pSiteSubscription = NULL;
		}
		return result;
	}

	OpcUa_Double CUADevice::GetSamplingInterVal( UaVariable* pUaVariable )
	{
		OpcUa_Double nres = 500;
		if (pUaVariable)
		{
// 			UaDataValue uaInternalValue;
// 			pUaVariable->getAttributeValue(NULL, OpcUa_Attributes_MinimumSamplingInterval, uaInternalValue);
// 			OpcUa_Double dSamValue;
// 			UaVariant varSam = (*uaInternalValue.value());
// 			varSam.toDouble(dSamValue);
// 			nres = dSamValue;
			FieldPointType* pFieldPoint = dynamic_cast<FieldPointType*>(pUaVariable);
			if (pFieldPoint)
			{
				nres = pFieldPoint->GetSamplingInterval();
			}
		}
		return nres;
	}

	void CUADevice::CreateDataChangeFilter( OpcUa_ExtensionObject* pFilter, UaVariable* pVariable )
	{
		UaNodeId FPTYPE(DEF_FieldPointType, FEP_TYPE_NS_INDEX);
		//ֻ��Ҫ�Բɼ�ͨ������������Ϣ
		if (pVariable && pFilter && (FPTYPE  == pVariable->typeDefinitionId()))
		{
			OpcUa_DataChangeFilter* pDataChangeFilter = NULL;
			OpcUa_EncodeableObject_CreateExtension(&OpcUa_DataChangeFilter_EncodeableType, pFilter, 
				(OpcUa_Void**)&pDataChangeFilter);
			if (pDataChangeFilter)
			{
				FieldPointType* pFieldPoint = dynamic_cast<FieldPointType*>(pVariable);
				if (pFieldPoint)
				{
					pDataChangeFilter->DeadbandType = OpcUa_DeadbandType_Absolute;
					pDataChangeFilter->DeadbandValue = pFieldPoint->getAbsDeadZone();
					pDataChangeFilter->Trigger = OpcUa_DataChangeTrigger_StatusValue;
				}
			}
		}
	}

	void CUADevice::Read()
	{
		//�����¶��ĵķ�ʽ��ȡһ������
		UaStatus result = OpcUa_BadInvalidState;
		CIODriver* pDriver = (CIODriver*)GetDriver();
		if (NULL == m_pSiteSubscription || NULL == m_pSiteProxy || NULL == pDriver)
		{
			UaString sLog(UaString("UADriver::UADevice Read failed��because m_pSiteSubscription or m_pSiteProxy is null!"));
			pDriver->WriteLogAnyWay(sLog.toUtf8());
			return;
		}

		OpcUa_Int32 nCount = m_MonitoredItemIds.length();
		if ( nCount <= 0 || nCount != m_Requests.length())
		{
			UaString sLog(UaString("UADriver::UADevice Read not need��because m_Requests is empty or delete monitor total is not eq add monitor!"));
			pDriver->WriteLogAnyWay(sLog.toUtf8());
			return;
		}
		
		//���Ҫɾ���ľͲ���Ҫɾ��������//
		ServiceSettings subServiceSettings;
		result = m_pSiteProxy->deleteSubscription(subServiceSettings, &m_pSiteSubscription);
		if (result.isBad())
		{
			UaString sErr(UaString("CUADevice:: Read  failed because deleteSubscription failed."));
			pDriver->WriteLogAnyWay(sErr.toUtf8());
			return;
		}
		m_pSiteSubscription = NULL;
		result = subscribe(m_pSiteProxy);
		if (result.isBad())
		{
			UaString sErr(UaString("CUADevice:: Read call subscribe failed."));
			pDriver->WriteLogAnyWay(sErr.toUtf8());
		}
		else
		{
			UaString sErr(UaString("CUADevice:: Read call subscribe Successful."));
			pDriver->WriteLog(sErr.toUtf8());
		}
	}

	OpcUa_Float CUADevice::GetVarFloatVal( const UaVariant& varVal )
	{
		OpcUa_Float fRes = 0;
		UaNodeId uVarDTId = varVal.dataType();
		OpcUa_NodeId valDTId;
		uVarDTId.copyTo(&valDTId);
		if (valDTId.IdentifierType == OpcUa_IdentifierType_Numeric)
		{
			OpcUa_Int32 nDataTypeNum = valDTId.Identifier.Numeric;
			if (nDataTypeNum >= OpcUaId_Boolean && nDataTypeNum < OpcUaId_String)
			{
				 varVal.toFloat(fRes);
			}
		}
		OpcUa_NodeId_Clear(&valDTId);
		return fRes;
	}

	void CUADevice::setMonitoringMode(OpcUa_MonitoringMode mode)
	{
		CIODriver* pDriver = (CIODriver*)GetDriver();
		char cLog[256];
		memset(cLog, 0, 256);
		if (m_pSiteSubscription)
		{
			ServiceSettings serviceSettings;
			UaStatusCodeArray setMonitoringResults;
			m_pSiteSubscription->setMonitoringMode(serviceSettings,mode,m_MonitoredItemIds,setMonitoringResults);
		}		
	}

	void CUADevice::UpdateTagOffLine()
	{
		CIOUnit* pIOUnit = GetIOUnitNode();
		//!�����豸�±�ǩ����
		if (pIOUnit)
		{

			UaDateTime dateTime = UaDateTime::now();
			pIOUnit->SetTagOnLineState(OpcUa_False, dateTime);
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CUADevice::UpdateTagOffLine Will Set DBUnit:%s OffLine"), this->GetName().c_str()));
		}
	}
}
