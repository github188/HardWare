/*
 * Written by Dylan Wu at 2012-02-02
 */
#include "ace/Signal.h"
#include "ace/Dynamic.h"
#include "ace/Init_ACE.h"
#include "ace/Date_Time.h"
#include "ace/Event.h"
#include "RTDBCommon/ClassDllManager.h"
#include "RTDBCommon/Debug_Msg.h"
#include "RTDBCommon/Runtimecatlog.h"
#include "../RTDBDrvSheduleMgr/IODrvScheduleMgrImpl.h"
#include "../RTDBNodeManager/NodeManagerCompositeImpl.h"
#include "RTDBBaseClasses/NodeManagerComposite.h"
#include "Server_SignalHandler.h"
#include "monitortask.h"
#include "common.h"
#include "shutdown.h"
#include <atltime.h>


#ifdef _WINDOWS
const char STARTSERVERS_RUNNING[] = "/STARTSERVERS_RUNNING";
#else
#define LOCKFILE "/var/run/StartServers.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#endif

const char STARTSERVERS_START[] = "-START";
const char STARTSERVERS_STOP[]	= "-STOP";
const char STARTSERVERS_SHOW[]	= "-SHOW";

bool g_bShow = false;


// int StartServersMain(int argc, char* argv[])
// {
// 	int ret = 0;

// 	//! 只能运行一个进程
// 	MACS_DEBUG((INFO_PERFIX ACE_TEXT("%s%s\n"), HY_ACE_DEBUG_PREFIX,HY_MSG_CHECK_MONIPROC_ISRUNNING));

// #ifdef _WINDOWS
// 	HANDLE hEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, STARTSERVERS_RUNNING);
// 	if( hEvent != NULL )
// 	{//! 进程已经在运行
// 		MACS_DEBUG((WARNING_PERFIX ACE_TEXT("%s%s\n"), HY_ACE_DEBUG_PREFIX,HY_MSG_CHECK_MONIPROC_RUNNING));
// 		::CloseHandle( hEvent );
// 		return ret;
// 	}
// 	else
// 	{
// 		MACS_DEBUG((INFO_PERFIX ACE_TEXT("%s%s\n"), HY_ACE_DEBUG_PREFIX,HY_MSG_NOTRUNNING));
// 		::CreateEvent(NULL, TRUE, FALSE, STARTSERVERS_RUNNING);
// 	}
// #endif	
//     //! 启动监视进程
//     MACS_DEBUG((INFO_PERFIX ACE_TEXT("%s%s\n"),HY_ACE_DEBUG_PREFIX,HY_MSG_CHECK_MONIPROC_START));

//     CMonitorTask task;
//     if (task.activate() >= 0)
//     {
//         MACS_DEBUG((INFO_PERFIX ACE_TEXT("%s%s\n\n"),HY_ACE_DEBUG_PREFIX,HY_MSG_SUCCESS));
//         MACS_DEBUG((INFO_PERFIX ACE_TEXT("%s%s\n"),HY_ACE_DEBUG_PREFIX,HY_MSG_SERVICE_START));
        
//         // Wait for user command to terminate the server thread.
//         while ( ShutDownFlag() == 0  )
//         {
//             Sleep(10);
//         } //while End
//         //!停止服务开始
//     }
//     else
//     {
//         MACS_DEBUG((ERROR_PERFIX ACE_TEXT("%s%s\n\n"),HY_ACE_DEBUG_PREFIX,HY_MSG_FAILURE));
//     }

// 	return ret;
// }


//MACS_SCADA_SUD::CIODrvScheduleMgr* g_pIODrvScheduleMgr = NULL;

int main(int argc, char* argv[])
{       
	using namespace MACS_SCADA_SUD;	
	// Initialize the ACE
	ACE::init();	

	__argc = argc;
	__argv = argv;

	int ret = 0;

	//! 将启动信息无条件写入文件
	ACE_LOG_MSG->open( argv[0] );	//!< 设置信息输出中的应用程序名argv[0]
	u_long level = MSG_DEBUG | MSG_WARNING | MSG_ERROR;
	ACE_LOG_MSG->priority_mask(level, ACE_Log_Msg::PROCESS);
	CLog_DebugFile::initialize();

	MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("Welcome to IOServer ...............")));

	std::string path = SingletonRuntimeCatlog::instance()->GetRunDir();

	if(!g_pNodeManagerCompsite)
		g_pNodeManagerCompsite = new CNodeManagerCompositeImpl;
	g_pNodeManagerCompsite->Init();

    g_pIOScheduleManager = new CIODrvScheduleMgr;

    if(!g_pIOScheduleManager)
	{
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("IOServer start service failed with IOSchduleManager::construct!\n")));
		return -1;
	}

	int nResult = g_pIOScheduleManager->Init();
	if (0 != nResult)
	{
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("IOServer start service failed with IOSchduleManager::Init!\n")));
		return -1;
	}

    nResult = g_pIOScheduleManager->StartWork();


	MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("IOServer Start Success!!!!")));
	//! 调整输出级别，只输出调试信息
    ACE_LOG_MSG->priority_mask(MSG_ERROR, ACE_Log_Msg::PROCESS);	
	
	// Wait for user command to terminate the server thread.
	printf("Press x or X to exit!");
	while ( ShutDownFlag() == 0  )
	{
		Sleep(10);
	} //while End
	//!停止服务开始
    
    return 0;    
}

