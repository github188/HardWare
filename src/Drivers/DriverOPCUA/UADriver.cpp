/*!
 *	\file	UADriver.cpp
 *
 *	\brief	UADriver类实现
 *
 *	\author LIFAN
 *
 *	\date	2014年5月19日	16:07
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "UADriver.h"
#include "Common/DataChange/DataTrans.h"
#include "Common/OS_Ext/ShowDbgInfo.h"
#include "Common/CommonXmlParser/DeployManager.h"
#include "HlyEventBase/EventCategoryConstant.h"
#include "RTDB/Server/DBPlatformClass/ComponentType.h"
#include "RTDB/Server/DBFrameAccess/DBFrameAccess.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"
#include "RTDB/Server/FEPPlatformClass/IOUnit.h"
#include "HyServerRedundancy/switchserv.h"
#include "DiagSwitch/common.h"

namespace MACS_SCADA_SUD
{
	//!定义驱动类类名
	std::string CUADriver::s_ClassName("CUADriver");

	CUADriver::CUADriver(void)
		:m_pSiteProxy(NULL), m_bIsSiteProxyOk(OpcUa_False), m_bIsConnectedOk(OpcUa_False)
	{

	}


	CUADriver::~CUADriver(void)
	{

	}

	//!建立Session连接
	OpcUa_Int32 CUADriver::OpenChannel()
	{
		OpcUa_Int32 nResult = -1;
		nResult = CIODriver::OpenChannel();
		return nResult;
	}

	//!断开Session连接
	OpcUa_Int32 CUADriver::CloseChannel()
	{
		OpcUa_Int32 nResult = -1;
		nResult = CIODriver::CloseChannel();
		return nResult;
	}

	//!开始工作
	OpcUa_Int32 CUADriver::StartWork( void )
	{
		OpcUa_Int32 nResult = -1;
		//!创建连接代理
		if (NULL == m_pSiteProxy)
		{
			if (NULL != g_pProxyMgr)
			{
				UaString usURI(GetDriverName().c_str());
				m_pSiteProxy = g_pProxyMgr->CreateSiteProxy(usURI, HY_SERV_TYPE_RTDB, NULL,NULL,this);
				if (m_pSiteProxy)
				{
					nResult = 0;
					m_bIsSiteProxyOk = OpcUa_True;
					UaString sLog(UaString("CUADriver::StartWork create siteProxy: %1 successful! \n").arg(usURI.toUtf8()));
					WriteLogAnyWay(sLog.toUtf8());
				}
				else
				{
					m_bIsSiteProxyOk = OpcUa_False;
					SetDeviceStatue(DEVICE_OFFLINE);
					UaString sLog(UaString("CUADriver::StartWork create siteProxy: %1 failed! \n").arg(usURI.toUtf8()));
					WriteLogAnyWay(sLog.toUtf8());
				}
			}
		}
		//!创建订阅和订阅监视项
		//UaThread::msleep(5000);
		if (m_bIsSiteProxyOk && m_pSiteProxy != NULL)
		{
			for (OpcUa_Int32 nIndex = 0; nIndex < GetDeviceCount(); nIndex++)
			{
				CUADevice* pDevice = (CUADevice*)GetDeviceByIndex(nIndex);
				UaStatus status = pDevice->subscribe(m_pSiteProxy);
				if (status.isGood())
				{
					nResult = 0;
				}
				else
				{
					nResult = -1;
					UaString sLog(UaString("CUADriver::StartWork device:%1 subscribe failed!").arg(pDevice->GetName().c_str()));
					WriteLogAnyWay(sLog.toUtf8());
				}
			}
		}
		return nResult;
	}

	//!停止工作
	OpcUa_Int32 CUADriver::StopWork( void )
	{
		OpcUa_Int32 nResult = -1;
		//!取消订阅
		for (OpcUa_Int32 nIndex = 0; nIndex < GetDeviceCount(); nIndex++)
		{
			CUADevice* pDevice = (CUADevice*)GetDeviceByIndex(nIndex);
			UaStatus status = pDevice->unsubscribe();
			if (status.isGood())
			{
				nResult = 0;
			}
			else
			{
				nResult = -1;
				UaString sLog(UaString("CUADriver::StopWork device:%1 unsubscribe failed!").arg(pDevice->GetName().c_str()));
				WriteLog(sLog.toUtf8());
			}
		}

		//!删除连接代理
		if (0 == nResult && m_pSiteProxy)
		{
			ServiceSettings serviceSettings;
			UaStatus status = m_pSiteProxy->disconnect(serviceSettings, OpcUa_True);
			if (status.isGood())
			{
				nResult = 0;
				m_bIsSiteProxyOk = OpcUa_False;
				UaString sLog(UaString("CUADriver::StopWork disconnect Session:%1\n").arg(GetDriverName().c_str()));
				WriteLog(sLog.toUtf8());
			} 
			else
			{
				nResult = -1;
			}
		}
		return nResult;
	}

	//!返回设备类类名
	std::string CUADriver::GetDeviceClassName()
	{
		return "CUADevice";
	}

	//!返回驱动类类名
	std::string CUADriver::GetClassName()
	{
		return s_ClassName;
	}

	//!Session状态发生变化
	void CUADriver::connectionStatusChanged( OpcUa_UInt32 clientConnectionId, UaClient::ServerStatus serverStatus )
	{
		OpcUa_ReferenceParameter(clientConnectionId);
		EXTERNAL_EVENT eventType = EXTERNAL_SOE;
		OpcUa_Int32 eventValue = -1;
		UaDateTime uStartT = UaDateTime::now();
		UaDateTime uTempTime;
		OpcUa_Int32  uCostTIme = 0;

		switch(serverStatus)
		{
		case UaClient::Disconnected:
        case UaClient::ConnectionErrorApiReconnect:
        case UaClient::ServerShutdown:
			{
				UaString sLog(UaString("CUADriver::The DataSource %1 connection status changed to Disconnected!\n").arg(GetDriverName().c_str()));
				WriteLogAnyWay(sLog.toUtf8());
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CUADriver::The DataSource %s connection status changed to Disconnected!\n\n"), GetDriverName().c_str()));

				m_bIsConnectedOk = OpcUa_False;		//!更新连接状态
				SetDBUnitStatus(HY_QUALITY_BAD);	//!设置数据库单元好坏
				SetDeviceStatue(DEVICE_OFFLINE);	//!设置设备状态

				eventType = FEP_DATASOURCE_OFFLINE;
				eventValue = (OpcUa_Int32)(DEVICE_OFFLINE);
				uCostTIme = uTempTime.msecsTo(UaDateTime::now()) - uTempTime.msecsTo(uStartT);
				sLog = UaString("CUADriver::The DataSource connection status changed to Disconnected! ProCost: %1 ms").arg(uCostTIme);
				WriteLogAnyWay(sLog.toUtf8());
			}
			break;
		case UaClient::Connected:
			{
				UaString sLog(UaString("CUADriver::The DataSource %1 connection status changed to Connected!\n").arg(GetDriverName().c_str()));
				WriteLogAnyWay(sLog.toUtf8());
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CUADriver::The DataSource %s connection status changed to Connected!\n\n"), GetDriverName().c_str()));

				m_bIsConnectedOk = OpcUa_True;		//!更新连接状态
				SetDBUnitStatus(HY_QUALITY_GOOD);	//!设置数据库单元好坏
				SetDeviceStatue(DEVICE_ONLINE);		//!设置设备状态

				eventType = FEP_DATASOURCE_ONLINE;
				eventValue = (OpcUa_Int32)(DEVICE_ONLINE);
				uCostTIme = uTempTime.msecsTo(UaDateTime::now()) - uTempTime.msecsTo(uStartT);
				sLog = UaString("CUADriver::The DataSource connection status changed to Connected! ProCost: %1 ms").arg(uCostTIme);
				WriteLogAnyWay(sLog.toUtf8());
			}
			break;
		case UaClient::ConnectionWarningWatchdogTimeout:
			{
				UaString sLog(UaString("CUADriver::The DataSource %1 connection status changed to Timeout!\n").arg(GetDriverName().c_str()));
				WriteLogAnyWay(sLog.toUtf8());
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CUADriver::The DataSource %s connection status changed to Timeout!\n"), GetDriverName().c_str()));
			}
			break;
		case UaClient::NewSessionCreated:
			{
				UaString sLog(UaString("CUADriver::The DataSource %1 connection status changed to NewSessionCreated!\n").arg(GetDriverName().c_str()));
				WriteLogAnyWay(sLog.toUtf8());
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CUADriver::The DataSource %s connection status changed to NewSessionCreated!\n"), GetDriverName().c_str()));
			}
			break;  
		default:
			break;
		}

		if (FEP_DATASOURCE_OFFLINE == eventType || FEP_DATASOURCE_ONLINE == eventType)
		{
			OpcUa_Int64 nDateTime = OpcUa_Int64(UaDateTime::now());
			CDBFrameAccess::FireUAEvent(this->GetIOPort(), eventType, eventValue, nDateTime);
		}

		uCostTIme = uTempTime.msecsTo(UaDateTime::now()) - uTempTime.msecsTo(uStartT);
		UaString Log = UaString("CUADriver::The DataSource connection status changed! ProCost: %1 ms").arg(uCostTIme);
		WriteLogAnyWay(Log.toUtf8());
	}

	//!得到Session连接
	CSiteProxy* CUADriver::GetSession()
	{
		if (m_pSiteProxy)
		{
			return m_pSiteProxy;
		}
		return NULL;
	}

	//!通过UADriver实现标签的强制和禁止
	OpcUa_Int32 CUADriver::WriteMgrForTag( tIORunMgrCmd& tRunMgr )
	{
		OpcUa_Int32 nRes = -1;
		UaVariable* pVariable = dynamic_cast<UaVariable*>(tRunMgr.pNode);
		if (NULL == pVariable)
		{
			UaString ustrLog(UaString("UaDriver::WriteMgrForTag Failed!The tRunMgr.pNode is Null or this is not a Variable!"));
			WriteLogAnyWay(ustrLog.toUtf8());
			return nRes;
		}
		UaStatus result;
		ServiceSettings serviceSettings;
		CallIn callRequest;
		CallOut callResult;
		UaString ustrNodeId = tRunMgr.pNode->nodeId().toFullString();
		UaVariant varCmdParam;
		UaNodeId varDType = pVariable->dataType();
		OpcUa_Int32 varCmdPLen = tRunMgr.byCmdParam.length();

		//!转换数据
		result = TranslateData(varDType, (OpcUa_Byte*)tRunMgr.byCmdParam.data(), varCmdPLen, varCmdParam);
		if (result.isNotGood())
		{
			UaString ustrLog(UaString("UaDriver::WriteMgrForTag TranslateData Failed! NodeId:%1, DataType:%2, Len:%3")
				.arg(ustrNodeId.toUtf8()).arg(varCmdParam.toString().toUtf8()).arg(varCmdPLen));
			WriteLogAnyWay(ustrLog.toUtf8());
			return nRes;
		}

		//!得到UADriver数据源所引用站点角色
		UaString usSourceName(GetDriverName().c_str());
		OpcUa_Int32 nSiteRole = g_pDeployManager->GetSiteRole(usSourceName);

		//!当数据源有FEP角色时调用Server节点下SendCmd方法
		if (nSiteRole & Identifier_Role_FEP)
		{
			callRequest.objectId = UaNodeId(FepId_GlobalMethodObject, 0);
			callRequest.methodId = UaNodeId(FepId_Method_SendCmd, 0);
			callRequest.inputArguments.resize(3);
			UaVariant variant;
			variant.setNodeId(tRunMgr.pNode->nodeId());
			variant.copyTo(&callRequest.inputArguments[0]);
			variant.setByte(tRunMgr.byCmdValue);
			variant.copyTo(&callRequest.inputArguments[1]);
			variant.setByteString(tRunMgr.byCmdParam, OpcUa_True);
			variant.copyTo(&callRequest.inputArguments[2]);
		}
		//!当数据源有RTDB角色并没有FEP角色则调用AI、DI下相应Force、Disable方法
		else if ((nSiteRole & Identifier_Role_RTDB) && !(nSiteRole & Identifier_Role_FEP))
		{
			nRes = ProcessRunMgr(tRunMgr, callRequest, varCmdParam);
		}

		//!调用Call服务
		if (m_pSiteProxy)
		{
			result = m_pSiteProxy->call(serviceSettings, callRequest, callResult);
			if (result.isGood())
			{
				if (callResult.callResult.isGood())
				{
					nRes = 0;
					UaString slog(UaString("UaDriver::WriteMgrForTag NodeId: %1 successful! CmdValue:%2 CmdParamV:%3\n").arg(ustrNodeId.toUtf8())
						.arg(tRunMgr.byCmdValue).arg(varCmdParam.toString().toUtf8()));
					WriteLogAnyWay(slog.toUtf8());
				}
				else
				{
					
					UaString slog(UaString("UaDriver::WriteMgrForTag NodeId: %1 failed with status %2,CmdValue:%3 CmdParamV:%4\n").arg(ustrNodeId.toUtf8()).arg(callResult.callResult.toString().toUtf8())
						.arg(tRunMgr.byCmdValue).arg(varCmdParam.toString().toUtf8()));
					WriteLogAnyWay(slog.toUtf8());
				}
			}
			else
			{
				UaString slog(UaString("UaDriver::WriteMgrForTag NodeId: %1 failed with status %2,CmdValue:%3 CmdParamV:%4\n").arg(ustrNodeId.toUtf8()).arg(callResult.callResult.toString().toUtf8())
					.arg(tRunMgr.byCmdValue).arg(varCmdParam.toString().toUtf8()));
				WriteLogAnyWay(slog.toUtf8());
			}
		}
		
		return nRes;
	}

	//!通过UADriver实现标签遥控
	OpcUa_Int32 CUADriver::WriteDCB( tIORemoteCtrl& tRemote )
	{
		OpcUa_Int32 nResult = -1;

		UaStatus result;
		ServiceSettings serviceSettings;
		UaWriteValues writeValues;
		UaStatusCodeArray writeResults;
		UaDiagnosticInfos diagnosticInfos;
		char strDebugInfo[1024] = {0};

		writeValues.create(1);
		writeValues[0].AttributeId = OpcUa_Attributes_Value;
		UaString strNodeId = tRemote.pNode->nodeId().toFullString();
		OpcUa_NodeId_CopyTo(tRemote.pNode->nodeId(), &writeValues[0].NodeId);
		UaVariant variant;

		if(tRemote.pNode->nodeClass() == OpcUa_NodeClass_Variable)
		{
			UaVariable* pVar = (UaVariable*)tRemote.pNode;
			if(pVar)
			{
				UaString strData;
				//!调试
				if (IsLog())
				{
					snprintf(strDebugInfo, 1024, "UADriver-WriteDCB before Translate Value:%s , length:%d\n", (OpcUa_CharA*)tRemote.bysValue.data(), tRemote.bysValue.length());
					SingletonDbgInfo::instance()->OutputDebugInfo(strDebugInfo);
					WriteLog(strDebugInfo);
				}
				
				result = TranslateData(pVar->dataType(), (OpcUa_Byte*)tRemote.bysValue.data(), tRemote.bysValue.length(), variant);
				
				if (IsLog())
				{
					snprintf(strDebugInfo, 1024, "UADriver-WriteDCB After Translate Value:%s , length:%d\n", variant.toString().toUtf8(), variant.toString().length());
					SingletonDbgInfo::instance()->OutputDebugInfo(strDebugInfo);
					WriteLog(strDebugInfo);
				}

				if (result.isGood())
				{
					variant.copyTo(&writeValues[0].Value.Value);

					if (m_pSiteProxy)
					{
						result = m_pSiteProxy->write(serviceSettings, writeValues, writeResults, diagnosticInfos);
						if (result.isBad())
						{							
							UaString slog(UaString("UADriver::WriteDCB Write failed with status %1, Node:%2 \n")
								.arg(result.toString().toUtf8()).arg(strNodeId.toUtf8()));
							WriteLogAnyWay(slog.toUtf8());
						}
						else
						{							
							OpcUa_Boolean bIsExistErr = OpcUa_False;
							for (OpcUa_UInt32 i = 0; i < writeResults.length(); i++)
							{
								if (OpcUa_IsBad(writeResults[i]))
								{
									UaString slog(UaString("Write failed for Node:%1 with status %2\n").arg(strNodeId.toUtf8()).arg(UaStatus(writeResults[i]).toString().toUtf8()));
									WriteLogAnyWay(slog.toUtf8());
									bIsExistErr = OpcUa_True;
								}
							}
							if (!bIsExistErr)
							{
								nResult = 0;
								UaString slog(UaString("UADriver::WriteDCB Write Successful with value: %1 length: %2, Node:%3 \n")
									.arg(variant.toString().toUtf8()).arg(variant.toString().length()).arg(strNodeId.toUtf8()));
								WriteLogAnyWay(slog.toUtf8());
							}
						}
					}
				}
			}
		}
		return nResult;
	}

	//!设置数据库单元状态
	void CUADriver::SetDBUnitStatus( OpcUa_Byte byQuality )
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

	void CUADriver::SetDeviceStatue( OpcUa_Byte statue )
	{
		//MACS_ERROR((ERROR_PERFIX ACE_TEXT("CUADriver::SetDeviceStatue DBSource:%s Status:%d!\n"), GetDriverName().c_str(), statue));
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

	//!处理数据源只有RTDB角色时标签强制、禁止命令请求
	OpcUa_Int32 CUADriver::ProcessRunMgr( tIORunMgrCmd& tRunMgr, CallIn& callRequest, UaVariant& varCmdParam )
	{
		OpcUa_Int32 nResult = -1;
		FieldPointType* pFieldPoint = dynamic_cast<FieldPointType*>(tRunMgr.pNode);
		if (pFieldPoint)
		{
			ComponentType* pComponent = pFieldPoint->getParent();
			if (pComponent)
			{
				callRequest.objectId = pComponent->nodeId();
				PointCategory enumCategory = pComponent->getPointCategory();
				//!根据类型进行处理
				if (AnalogItem == enumCategory)				//!AI模拟量类型
				{
					switch(tRunMgr.byCmdValue)
					{
					case CmdValue_Force:
					case CmdValue_UnForce:
						{
							callRequest.methodId = UaNodeId(DEF_AIType_Force, RTDB_TYPE_NS_INDEX);
							callRequest.inputArguments.create(2);
							UaVariant variant;
							if (CmdValue_Force == tRunMgr.byCmdValue)
							{
								variant.setBool(OpcUa_True);
							}
							else if (CmdValue_UnForce == tRunMgr.byCmdValue)
							{
								variant.setBool(OpcUa_False);
							}
							variant.copyTo(&callRequest.inputArguments[0]);
							varCmdParam.copyTo(&callRequest.inputArguments[1]);

							nResult = 0;
						}
						break;
					case CmdValue_Enable:
					case CmdValue_Disable:
						{
							callRequest.methodId = UaNodeId(DEF_AIType_Disable, RTDB_TYPE_NS_INDEX);
							callRequest.inputArguments.create(1);
							UaVariant variant;
							if (CmdValue_Enable == tRunMgr.byCmdValue)
							{
								variant.setBool(OpcUa_False);
							}
							else if (CmdValue_Disable == tRunMgr.byCmdValue)
							{
								variant.setBool(OpcUa_True);
							}
							variant.copyTo(&callRequest.inputArguments[0]);

							nResult = 0;
						}
						break;
					default:
						break;
					}
				}
				else if (DiscreteItem == enumCategory)		//!DI离散量类型
				{
					switch(tRunMgr.byCmdValue)
					{
					case CmdValue_Force:
					case CmdValue_UnForce:
						{
							callRequest.methodId = UaNodeId(DEF_DIType_Force, RTDB_TYPE_NS_INDEX);
							callRequest.inputArguments.create(2);
							UaVariant variant;
							if (CmdValue_Force == tRunMgr.byCmdValue)
							{
								variant.setBool(OpcUa_True);
							}
							else if (CmdValue_UnForce == tRunMgr.byCmdValue)
							{
								variant.setBool(OpcUa_False);
							}
							variant.copyTo(&callRequest.inputArguments[0]);
							varCmdParam.copyTo(&callRequest.inputArguments[1]);

							nResult = 0;
						}
						break;
					case CmdValue_Enable:
					case CmdValue_Disable:
						{
							callRequest.methodId = UaNodeId(DEF_DIType_Disable, RTDB_TYPE_NS_INDEX);
							callRequest.inputArguments.create(1);
							UaVariant variant;
							if (CmdValue_Enable == tRunMgr.byCmdValue)
							{
								variant.setBool(OpcUa_False);
							}
							else if (CmdValue_Disable == tRunMgr.byCmdValue)
							{
								variant.setBool(OpcUa_True);
							}
							variant.copyTo(&callRequest.inputArguments[0]);

							nResult = 0;
						}
						break;
					default:
						break;
					}
				}
			}
		}
		return nResult;
	}

	//!获取站点的代理指针
	inline CSiteProxy* CUADriver::GetSiteProxy()
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

	void CUADriver::ReadOneTimeData()
	{
		OpcUa_Int32 nCount = GetDeviceCount();
		OpcUa_CharA pchLog[DEBUGINFOLEN] = {0};
		for (OpcUa_Int32 index = 0; index < nCount; index++)
		{
			CUADevice* pDevice = static_cast<CUADevice*>(GetDeviceByIndex(index));
			if (pDevice)
			{
				if (m_bIsConnectedOk)
				{
					MACS_ERROR((ERROR_PERFIX ACE_TEXT("CUADriver::ReadOneTimeData Dev:%s Read a new Frame Data form FEP\n"), pDevice->GetName().c_str()));
					pDevice->Read();
					snprintf(pchLog, DEBUGINFOLEN, "CUADriver::ReadOneTimeData Dev:%s Read a new Frame Data form FEP\n", pDevice->GetName().c_str());
					WriteLogAnyWay(pchLog);
				}
			}
		}
	}

	void CUADriver::setMonitoringMode(OpcUa_MonitoringMode mode)
	{
		OpcUa_Int32 nCount = GetDeviceCount();
		for (OpcUa_Int32 index = 0; index < nCount; index++)
		{
			CUADevice* pDevice = static_cast<CUADevice*>(GetDeviceByIndex(index));
			if (pDevice)
			{
				//!设置监视模式
				pDevice->setMonitoringMode(mode);

				//!如果与数据源链接断开则设置采集通道状态为离线(只有在当前链路为主时需要设置)
				if (!m_bIsConnectedOk && OpcUa_MonitoringMode_Reporting == mode)
				{
					pDevice->UpdateTagOffLine();
				}
			}
		}
		std::string strMsg;
		if (OpcUa_MonitoringMode_Disabled == mode)
		{
			strMsg = "Disabled!";
		}
		else if (OpcUa_MonitoringMode_Reporting == mode)
		{
			strMsg = "Reporting!";
		}
		else if (OpcUa_MonitoringMode_Sampling == mode)
		{
			strMsg = "Sampling!";
		}
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CUADriver::setMonitoringMode mode:%d Port:%s IsConnectedOk:%d \n"), mode, strMsg.c_str(), m_bIsConnectedOk));
	}
}
