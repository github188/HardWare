/*!
 *	\file	FepIEC104Drv.cpp
 *
 *	\brief	104主站驱动类实现源文件
 *
 *	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Drv.cpp, 219+1 2011/04/11 07:14:19 miaoweijie $
 *	$Author: miaoweijie $
 *	$Date: 2011/04/11 07:14:19 $
 *	$Revision: 219+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "FepIEC104Drv.h"
#include "FepIEC104Prtcl.h"
#include "ace/Init_ACE.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"

namespace MACS_SCADA_SUD
{
	void OnConnectTimeOut(void* p);							//! 定时器超时回调函数

	std::string CFepIEC104Drv::s_ClassName("CFepIEC104Drv");		//! 类名

	CFepIEC104Drv::~CFepIEC104Drv()
	{
		//! 删除定时器
		if(m_pConnTimer)
		{
			delete m_pConnTimer;
			m_pConnTimer = NULL;
		}
	}

	std::string CFepIEC104Drv::GetClassName()
	{
		return s_ClassName;
	}

	/*!
	*	\brief	得到继承PrtclParserBase基类的继承类名
	*
	*	\retval	返回值：继承类名
	*/
	std::string CFepIEC104Drv::GetPrtclClassName()
	{
		return "CFepIEC104Prtcl";
	}

	/*!
	*	\brief	得到继承DeviceBase基类的继承类名
	*
	*	\retval	返回值：继承类名
	*/
	std::string CFepIEC104Drv::GetDeviceClassName()
	{
		return "CFepIEC104Device";
	}

	OpcUa_Int32 CFepIEC104Drv::StartWork(void)
	{
		WriteLogAnyWay( "StartWork: Begin ..." );

		m_bHasConnected = false;
		m_pIODrvBoard = new CIODrvBoard();
		if ( m_pIODrvBoard == NULL )
		{
			MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIODriver::StartWork:m_pIODrvBoard is Null")));
			//! 输出调试信息
// 			if ( g_pIODebugManager && g_pIODebugManager->IsModuleDebug( DEBUG_DRIVERFRAME ) )
// 			{
// 				g_pIODebugManager->OutputInfo( 
// 					DEBUG_DRIVERFRAME, "CIODriver::StartWork:m_pIODrvBoard is Null!" );
// 			}

			WriteLogAnyWay( "StartWork:m_pIODrvBoard is Null!" );

			return -2;
		}
		std::string strPortType = m_pPortCfg.strType.toUtf8();
		m_pIODrvBoard->Init( m_pIOPrtclParser, strPortType, m_pPortCfg, this->GetTimeOut());

		m_pIODrvBoard->open();

		CFepIEC104Prtcl* pPrtcl = dynamic_cast<CFepIEC104Prtcl*>(m_pIOPrtclParser);
		std::string strTemp = GetDriverName();

		//! 初始化链路参数
		pPrtcl->InitConfigPar( (char*)strTemp.c_str() );

		////////////////////////////////////////////////////////
		//! 初始化网络参数
		InitNetParam();

		//! Connect and Start Timer
		Connect();

		//! 定时器
		//m_pConnTimer->SetTimer(MACSIEC104CONN_MAX_TIMEOUT);
		////////////////////////////////////////////////////////

		//开始周期定时器
		WriteLogAnyWay( "StartWork: to call m_pPeriodTimer->SetTimer" );
		pPrtcl->m_pPeriodTimer->SetTimer( PERIOD_SCAN_MS );


		//! 输出调试信息
// 		if ( g_pIODebugManager && g_pIODebugManager->IsModuleDebug( DEBUG_DRIVERFRAME ) )
// 		{
// 			g_pIODebugManager->OutputInfo( DEBUG_DRIVERFRAME, "CIODriver::StartWork !" );
// 		}

		WriteLogAnyWay( "StartWork: End!" );

		return 0;
	}

	//! 初始化网络连接参数
	OpcUa_Int32 CFepIEC104Drv::InitNetParam()
	{
		m_pConnTimer = new CIOTimer(OnConnectTimeOut, this);

		m_nConnTimes		= 0;
		m_nSerAddrNum		= 0;
		m_nCurSerAddrIndex	= 0;

		//! 服务器端口号
		m_nPort	= m_pPortCfg.nPort;		
		AddSerAddr(m_pPortCfg.strIPA_A.toUtf8());
		AddSerAddr(m_pPortCfg.strIPA_B.toUtf8());
		AddSerAddr(m_pPortCfg.strIPB_A.toUtf8());
		AddSerAddr(m_pPortCfg.strIPB_B.toUtf8());

		return 0;
	}

	//! 添加服务器
	void CFepIEC104Drv::AddSerAddr(const char* pchSerAddr)
	{
		if(pchSerAddr && strcmp( pchSerAddr,"")!=0 )
			m_stSerAddrList[m_nSerAddrNum++] = pchSerAddr;
	}

	//! 封装一下连接操作，参数为服务器序号
	OpcUa_Int32 CFepIEC104Drv::Connect(OpcUa_Int32 nIndex)
	{
		if( nIndex < 0 || nIndex >= m_nSerAddrNum )
			return -1;

		char pchLog[256];
		sprintf( pchLog, "连接服务器%s, 序号为%d ...",
			m_stSerAddrList[nIndex].c_str(), nIndex );
		WriteLogAnyWay( pchLog );

		SetConnTimer( MACSIEC104CONN_MAX_TIMEOUT );

		IsConnecting(true);
		m_pIODrvBoard->Connect( m_stSerAddrList[nIndex].c_str(), m_nPort );
		m_nCurSerAddrIndex = nIndex;	


		return 0;
	}

	/*!
	*	\brief	重连
	*
	*	如果连接失败，则重连，
	*	单个服务器连续m_byMaxReConn次连接失败则换下一个服务器
	*
	*/
	void CFepIEC104Drv::ReConnect()
	{
		//! 试连次数加1
		IncrConnTimes();

		char pchLog[256];	
		sprintf( pchLog, "重连次数为%d ...", m_nConnTimes );
		WriteLogAnyWay( pchLog );

		//! 超过最大试连次数，则试连下一个服务器
		if( m_nConnTimes >= MACSIEC104DRIVER_CONNECT_MAXTIMES )
		{
			ResetConnTimes();
			Connect( NextSerAddrIndex() );			
		}
		else
		{
			//! 重连
			Connect( m_nCurSerAddrIndex );
		}

		//! 起连接定时器
		//m_pConnTimer->SetTimer( MACSIEC104CONN_MAX_TIMEOUT );

		return;
	}


	//! 定时器超时回调函数
	void OnConnectTimeOut(void* p)					
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)p;		

		pDriver->IsConnecting(false);	

		CFepIEC104Prtcl* pPrtcl = dynamic_cast<CFepIEC104Prtcl*>(pDriver->m_pIOPrtclParser);
		if ( pPrtcl == NULL )
		{
			pDriver->WriteLogAnyWay( "Error：OnConnectTimeOut: CFepIEC104Prtcl is NULL " );
			return;
		}

		//! 日志
		char pchInfo[256];
		sprintf( pchInfo, "Error：OnConnectTimeOut，连接服务器%s超时...", pDriver->GetCurServerIP().c_str() );
		pDriver->WriteLogAnyWay( pchInfo );

		pDriver->KillConnTimer();
	}

	OpcUa_Int32 CFepIEC104Drv::WriteLogAnyWay( const char* pchLog, bool bTime )
	{
		if ( !m_pLog )
		{
			return -1;
		}

		m_pLog->WriteLog( pchLog, bTime );

		return 0;
	}

}
