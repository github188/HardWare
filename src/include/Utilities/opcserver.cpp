/******************************************************************************
** opcserver.cpp
**
** Copyright (c) 2006-2013 Unified Automation GmbH All rights reserved.
**
** Software License Agreement ("SLA") Version 2.3
**
** Unless explicitly acquired and licensed from Licensor under another
** license, the contents of this file are subject to the Software License
** Agreement ("SLA") Version 2.3, or subsequent versions
** as allowed by the SLA, and You may not copy or use this file in either
** source code or executable form, except in compliance with the terms and
** conditions of the SLA.
**
** All software distributed under the SLA is provided strictly on an
** "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
** AND LICENSOR HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING WITHOUT
** LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
** PURPOSE, QUIET ENJOYMENT, OR NON-INFRINGEMENT. See the SLA for specific
** language governing rights and limitations under the SLA.
**
** The complete license agreement can be found here:
** http://unifiedautomation.com/License/SLA/2.3/
**
** Project: C++ OPC Server SDK sample code
**
** Description: Main OPC Server object class.
**
******************************************************************************/
#include "opcserver.h"
#include "uamodule.h"
#include "uasession.h"


#include "nodemanagerroot.h"
#include "Common/HotData/HotDataMsgQueue.h"
#include "RTDB/Server/DBPlatformClass/CallBackPV.h"
#include "../../02.HA/HAService/HAService.h"
#include "../../02.HA/HD/HDSource/HDSourceOORTDB/HDSourceOORTDBMgr.h"
#include "Common/RuntimeCatlog/RuntimeCatlog.h"
#include "Common/GlobalVariable/RunVariable.h"
#include "Common/CommonXmlParser/DeployManager.h"
#include "Common/OS_Ext/ShowDbgInfo.h"
#include "RTDB/Server/DBBaseClasses/NodeManagerComposite.h"
#include "RTDB/Server/DBBaseClasses/IODrvScheduleMgrBase.h"
#include "RTDB/Server/DBBaseClasses/SyncManagerBase.h"
#include "RTDB/Server/DBPluginManager/PluginSchedule.h"
#include "RTDB/Server/DBBaseClasses/IOCmdManagerBase.h"
#include "../../01.RTDB/Server/ScadaServer/InitAndFini.h"
#include "Perm/common/IPermClient.h"
#include "RTDB/Server/DBPlatformClass/CallBackVariable.h"

using namespace MACS_SCADA_SUD;
using namespace UaClientSdk;
#ifndef UA_BUILD_DATE_ZONE
#define UA_BUILD_DATE_ZONE 1 // Must match UTC offset and daylight saving time at build date
#endif /* UA_BUILD_DATE_ZONE */

/** Class containing the private members for the OpcServer class. */
class OpcServerPrivate
{
public:
    OpcServerPrivate()
    : m_pUaModule(NULL)
    {}
    ~OpcServerPrivate(){}

    UaModule*          m_pUaModule;
};

/** Construction. */
OpcServer::OpcServer()
{
    d = new OpcServerPrivate;
}

/** Destruction. */
OpcServer::~OpcServer()
{
    if ( isStarted() != OpcUa_False )
    {
        UaLocalizedText reason("en","Application shut down");
        stop(0, reason);
    }

    if ( d->m_pUaModule )
    {
        delete d->m_pUaModule;
        d->m_pUaModule = NULL;
    }

    delete d;
    d = NULL;
}

UaStatus OpcServer::afterInitialize()
{
    UaStatus ret;

    // Create and initialize UA server module
    d->m_pUaModule = new UaModule;
    // Check if we have a specialized implementation provided by the application
    UaServer *pUaServer = NULL;
    UaServerApplicationCallback* pCallback = getServerApplicationCallback();
    if (pCallback)
    {
        pUaServer = pCallback->createUaServer();
    }
    int result = d->m_pUaModule->initialize(getServerConfig(),pUaServer);
    if ( 0 != result )
    {
        return OpcUa_BadInternalError;
    }

	//! 启动权限客户端代理
	IPermClient* pPermClient = CreatePermClient();
	if (pPermClient)
	{
		pPermClient->StartWork();
	}
	else
	{
		printf("OpcServer::afterStartUp -- CreatePermClient() failed.\n");
		return OpcUa_Bad;
	}

	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... PermProxy Initialzie and StartWork ok")));

    return ret;
}

UaStatus OpcServer::afterStartUp()
{
    UaStatus ret;

	/*
	SetAllFinishSerial(OpcUa_False);
    vector<OpcUa_Boolean> vecSignals;
	
	OpcUa_Boolean bSignalsOk = OpcUa_False;
	while(!bSignalsOk)
	{
		GetAllHandles(vecSignals);

		if(!vecSignals.empty())
		{
			OpcUa_Boolean bAllOk = OpcUa_True;
			//! 查看是否所有文件都异步加载完毕
			for(int i=0; i < vecSignals.size(); i++)
			{
				if(!vecSignals[i])
				{
					bAllOk = OpcUa_False;
					break;
				}
			}

			if(bAllOk)
			{
				bSignalsOk = OpcUa_True;
				break;
			}
			else
			{
				UaThread::msleep(3000);
				printf("Wait for 3s!\n");
			}
		}
		else
		{
			bSignalsOk = OpcUa_True;
		}
	}

	printf("Wait for all[%d] finished!\n", vecSignals.size());
	*/

	SetAllFinishSerial(OpcUa_True);
	vector<OpcUa_Boolean> vecCreatedSignals;

	OpcUa_Boolean bSignalsOk = OpcUa_False;
	while(!bSignalsOk)
	{
		GetAllCreatedHandles(vecCreatedSignals);

		if(!vecCreatedSignals.empty())
		{
			OpcUa_Boolean bAllOk = OpcUa_True;
			//! 查看是否所有文件都异步加载完毕
			for(int i=0; i < vecCreatedSignals.size(); i++)
			{
				if(!vecCreatedSignals[i])
				{
					bAllOk = OpcUa_False;
					break;
				}
			}

			if(bAllOk)
			{
				bSignalsOk = OpcUa_True;
				break;
			}
			else
			{
				UaThread::msleep(500);
				printf("Wait for unit logical 500ms!\n");
			}
		}
		else
		{
			bSignalsOk = OpcUa_True;
		}
	}

	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... Load Binary files ok")));

	//! 初始化并启动冗余
	if (0 != InitAndStartRedundancy())
	{
		return OpcUa_Bad;
	}
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... Initial Redundancy ok")));

	/*OpcUa_Boolean bOk = g_pNodeManagerCompsite->InitDataBaseFromOtherSvr();*/
	std::map<OpcUa_UInt16, OpcUa_Boolean> mapResult;
	OpcUa_Boolean bOk = g_pNodeManagerCompsite->InitAndRecoveryData(mapResult);
	if(bOk)
	{
		SingletonDbgInfo::instance()->MonitorVariable("before load hot");

		MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... Initial Load hot begin")));
		g_pNodeManagerCompsite->LoadLocalHotData(mapResult);
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... Initial Load hot end then to load config begin")));
		g_pNodeManagerCompsite->LoadKeepConfigHotData(mapResult);
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... Initial Load hot Config scalar over")));
		g_pNodeManagerCompsite->LoadArrayConfigHotData(mapResult);
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... Initial Load hot Config array over")));

		SingletonDbgInfo::instance()->MonitorVariable("after load hot");
	}

	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... Load HotData ok")));

	g_pNodeManagerCompsite->ChangeWildCardsScript();

	unsigned long time1 = GetTickCount();
	g_pNodeManagerCompsite->Insertinit();
	unsigned long time2 = GetTickCount();
	printf("Insert initial data time[%ld]\n", (time2 - time1)/1000);

	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... Initialize Hot ok")));

	//! 上面插入初值后再启动内部监视项的热数据收集线程
	if(NULL != g_pNodeManagerCompsite)
		g_pNodeManagerCompsite->TriggerAllNodesAndReferenceCreated();
	
	//设置实时服务地址空间及热数据加载完毕标识
	SetStarted(OpcUa_True);

	g_pNodeManagerCompsite->TriggerValueChange();

	//! 设置软件版本号
	g_pDeployManager->SetSoftwareVersionNode();
	
	// We support historical data access
	if(NULL != g_pHAService)
		g_pHAService->pHistoryServerCapabilities()->setAccessHistoryDataCapability(OpcUa_True);

	//! 初始化并启动报警服务
	if (0 != InitAndStartALM())
	{
		return OpcUa_Bad;
	}

	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... AlarmService Initialzie and StartWork ok")));

	//! 初始化并启动事件服务
	if (0 != InitAndStartHE())
	{
		return OpcUa_Bad;
	}

	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... EventService Initialzie and StartWork ok")));

	SetCallbackValid();

	SingletonDbgInfo::instance()->MonitorVariable("before load plugin");

	int nResult = 0;
	nResult = g_pPluginManager->CallBack("AfterLoad");
	if (0 != nResult)
	{
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start..........Plugin failed!\n")));
		printf("Plugin Load failed********************!\n");
		return OpcUa_Bad;
		//return -6;   //!当存在级别为1的插件加载失败时，服务退出。
	}

	//! 启动历史数据采集时机：尽可能晚启动但是要在IOSchduleManager之前启动
	//! 启动历史数据采集
	if(NULL != g_pHDSourceOORTDBMgr)
	{
		g_pHDSourceOORTDBMgr->StartUp(getServerManager());
		g_pHDSourceOORTDBMgr->ActivateMonitor();
	}
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... HistoryManager Initialzie and StartWork ok")));

	nResult = g_pIOScheduleManager->Init();
	if (0 != nResult)
	{
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer start service failed with IOSchduleManager::Init!\n")));
		return OpcUa_Bad;
	}
	nResult = g_pIOScheduleManager->StartWork();
	if (0 != nResult)
	{
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer start service failed with IOSchduleManager::StartWork!\n")));
		return OpcUa_Bad;
	}

	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... IOScheduleManager Initialzie and StartWork ok")));

	nResult = g_pSyncManager->Init();
	if (0 != nResult)
	{
		printf("ScadaServer start service failed with g_pSyncManager::Init!\n");
		return OpcUa_Bad;
	}
	nResult = g_pSyncManager->StartWork();
	if (0 != nResult)
	{
		printf("ScadaServer start service failed with g_pSyncManager::StartWork!\n");
		return OpcUa_Bad;
	}

	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... SyncManager Initialzie and StartWork ok")));

	nResult = g_pIOCmdManager->Init();
	if (0 != nResult)
	{
		printf("ScadaServer start service failed with g_pIOCmdManager::Init!\n");
		return OpcUa_Bad;
	}
	nResult = g_pIOCmdManager->StartWork();
	if (0 != nResult)
	{
		printf("ScadaServer start service failed with g_pIOCmdManager::StartWork!\n");
		return OpcUa_Bad;
	}

	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... IOCmdManager Initialzie and StartWork ok")));

    // Start UA server module
    int result = d->m_pUaModule->startUp(getCoreModule());
    if ( 0 != result )
    {
        d->m_pUaModule->shutDown();
        delete d->m_pUaModule;
        d->m_pUaModule = NULL;

        return OpcUa_BadInternalError;
    }

	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... UaModule StartUp ok")));

    UaString        sRejectedCertificateDirectory;
    OpcUa_UInt32    nRejectedCertificatesCount;
    UaEndpointArray uaEndpointArray;
    getServerConfig()->getEndpointConfiguration(
        sRejectedCertificateDirectory,
        nRejectedCertificatesCount,
        uaEndpointArray);
    if ( uaEndpointArray.length() > 0 )
    {
        printf("***************************************************\n");
        printf(" Server opened endpoints for following URLs:\n");
        OpcUa_UInt32 idx;
        bool bError = false;
        for ( idx=0; idx<uaEndpointArray.length(); idx++ )
        {
            if ( uaEndpointArray[idx]->isOpened() )
            {
                printf("     %s\n", uaEndpointArray[idx]->sEndpointUrl().toUtf8());
            }
            else
            {
                bError = true;
            }
        }
        if ( bError )
        {
            printf("\n");
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            printf("!!!! The following endpoints URLs failed:\n");
            for ( idx=0; idx<uaEndpointArray.length(); idx++ )
            {
                if ( uaEndpointArray[idx]->isOpened() == false )
                {
                    printf("!!!! %s\n", uaEndpointArray[idx]->sEndpointUrl().toUtf8());
                }
            }
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            printf("\n");
        }
        printf("***************************************************\n");
    }
	
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Start............... Server Opened and Supply Service ok")));

    return ret;
}

UaStatus OpcServer::beforeShutdown()
{
    UaStatus ret;

	//请将此项放在最前
	//关闭运行信息采集线程
	CDiagnoseDataCollector::getInstance()->stop();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... DiagnoseDataCollector stop ok")));

	SetExit();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... SetExit ok")));

	//! 触发关闭时脚本
	g_pNodeManagerCompsite->ExecuteEndScriptBeforeShutDown();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... ExecuteEndScriptBeforeShutDown stop ok")));

	//! Bug68971停服务过程中的历史数据无法补齐
	//! 停止数据历史
	FiniHD();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... HD stop ok")));

	if(NULL != g_pHotQueue)
		g_pHotQueue->RecordLastValue();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... Record HotData ok")));

	//! 停止事件历史
	FiniHE();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... HE stop ok")));

	//! 停止报警服务
	FiniALM();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... ALM stop ok")));

	//停止权限客户端代理
	CreatePermClient()->StopWork();
	CreatePermClient()->UnInit();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... PermClient stop ok")));

	g_pScriptScheduleManager->StopWork();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... ScriptScheduleManager stop ok")));

	//! 停止g_pIOCmdManager
	g_pIOCmdManager->StopWork();	
	g_pIOCmdManager->UnInit();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... IOCmdManager stop ok")));

	//! 停止g_pSyncManager
	g_pSyncManager->StopWork();	
	g_pSyncManager->UnInit();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... SyncManager stop ok")));

	//! 停止g_pIOScheduleManager
	g_pIOScheduleManager->StopWork();	
	g_pIOScheduleManager->UnInit();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... IOScheduleManager stop ok")));

	//! 停止冗余
	FiniRedundancy();
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... FiniRedundancy stop ok")));

    // Stop UA server module
    if ( d->m_pUaModule )
    {
        d->m_pUaModule->shutDown();
        delete d->m_pUaModule;
        d->m_pUaModule = NULL;
    }
	MACS_ERROR((ERROR_PERFIX ACE_TEXT("ScadaServer Stop............... UA server module stop ok")));

    return ret;
}

Session* OpcServer::createDefaultSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken)
{
    return new ::UaSession(sessionID, authenticationToken);
}

/** Get the build date from the static compiled in string.
 *  @return the build date from the static compiled in string.
 */
OpcUa_DateTime OpcServer::getBuildDate() const
{
    static OpcUa_DateTime date;
    static const char szDate[] = __DATE__; /* "Mon DD YYYY" */
    static char szISO[] = "YYYY-MM-DDT"__TIME__"Z";
    static const char* Months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    char mon = 0;

    /* set year */
    szISO[0] = szDate[7];
    szISO[1] = szDate[8];
    szISO[2] = szDate[9];
    szISO[3] = szDate[10];

    /* set month */
    while ( (strncmp(Months[(int)mon], szDate, 3) != 0) && (mon < 11) )
    {
        mon++;
    }
    mon++;
    szISO[5] = '0' + mon / 10;
    szISO[6] = '0' + mon % 10;

    /* set day */
    szISO[8] = szDate[4];
    szISO[9] = szDate[5];

    /* convert to UA time */
    OpcUa_DateTime_GetDateTimeFromString(szISO, &date);

    /* correct time */
    UaDateTime buildDate(date);
    buildDate.addSecs(UA_BUILD_DATE_ZONE * 3600 * -1);

    return buildDate;
}
