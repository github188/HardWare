/*!
 *	\file	FepModbusDriver.cpp
 *
 *	\brief	通用Modbus驱动类实现源文件
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusDriver.cpp 25    10-10-28 9:24 Miaoweijie $
 *	$Author: Miaoweijie $
 *	$Date: 10-10-28 9:24 $
 *	$Revision: 25 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "FepModbusDriver.h"
#include "FepModbusPrtcl.h"
#include "FepModbusDevice.h"


namespace MACS_SCADA_SUD
{
	void OnTmOut(void* p);							//! 定时器超时回调函数

	extern int GetPrivateInifileInt(const char* pchAppName, 
		const char* pchKeyName,	int nDefault, const char* pchFileName);	


	std::string CFepModbusDriver::s_ClassName("CFepModbusDriver");	//! 类名

	std::string CFepModbusDriver::GetClassName()
	{
		return s_ClassName;
	}

	CFepModbusDriver::CFepModbusDriver()
	{
		m_nWatchTime = 30000;

		//m_pConnTimer = NULL;
		m_bConnected = false;
		m_iDevIndex		= 0;
		m_iDevLastIndex = m_iDevIndex;

		m_MaxHotLinkOffline = 300;
		m_nMaxStandbyLinkOffline = 600;
		m_nMaxLinkOffline = m_nMaxStandbyLinkOffline;	
		m_nRcvTimeOut = 5000;			
		m_nMaxOfflineCount = 20;
		m_bIsLinkGetRightFrame = false;
		m_nMaxLinkOfflineConnectCount = 3*4;
		m_nLinkOfflineConnectCount = 0;	

		m_bCanWork = false;
		m_bIsEnglish = false;
	}	

	CFepModbusDriver::~CFepModbusDriver()
	{
		//if(m_pConnTimer)
		//{
		//	m_pConnTimer->KillTimer();
		//	delete m_pConnTimer;
		//	m_pConnTimer = NULL;
		//}
	}

	/*!
	*	\brief	得到继承PrtclParserBase基类的继承类名
	*
	*	\retval	返回值：继承类名
	*/
	std::string CFepModbusDriver::GetPrtclClassName()
	{
		return "CFepModbusPrtcl";
	}

	std::string CFepModbusDriver::GetDeviceClassName()
	{
		return "CFepModbusDevice";
	}

	int CFepModbusDriver::StartWork(void)
	{
		CFepModbusPrtcl* pModbusPrtcl = dynamic_cast<CFepModbusPrtcl*>(m_pIOPrtclParser);

		if (!pModbusPrtcl)
		{			
			//! 记录日志
			WriteLogAnyWay( "ERROR：CFepModbusDriver::StartWork: pModbusPrtcl == NULL!" );
			return -1;
		}

		//! 记录日志
		WriteLogAnyWay( "CFepModbusDriver::StartWork..." );

		//! 是网络介质则去连接服务器
		if( m_byPrtclType == 1 || m_byPrtclType == 2 )
		{
			//!初始化网络参数
			InitNetParam();

			if ( m_nSerAddrNum <= 0 )
			{
			//	char pchMsg[256];
			//	if(IsEnglishEnv())
			//	{
			//		snprintf(pchMsg, sizeof(pchMsg), "CFepModbusDriver::StartWork Error,Driver:%s is based On TCP but there is no IP Address Info", GetDriverName().c_str());
			//	}
			//	else
			//	{
			//		snprintf( pchMsg, sizeof(pchMsg),"CFepModbusDriver::StartWork 错误,驱动:%s基于TCP，但在组态Port中没有配置设备IP地址!!!",GetDriverName().c_str() );
			//	}
			//	WriteLogAnyWay( pchMsg );
			}
			else
			{
				m_pConnTimer = new CIOTimer(OnTmOut, this, true, pModbusPrtcl->GetTimerMgr());
			}

			if ( m_nSerAddrNum > 0 )
			{
				m_MaxHotLinkOffline = m_nSerAddrNum*m_byMaxReConn*( m_byConnTimeOut+m_nMaxOfflineCount*m_nRcvTimeOut*GetDeviceCount())/1000;
			}
			else
				m_MaxHotLinkOffline = 1*m_byMaxReConn*( m_byConnTimeOut+m_nMaxOfflineCount*m_nRcvTimeOut*GetDeviceCount())/1000;

			if ( m_byPrtclType == 2 && IsPortAccessClash() && IsFuzzy())
			{
				if ( m_nSerAddrNum > 0 )
				{
					m_nMaxStandbyLinkOffline = m_nSerAddrNum*m_byMaxReConn*( m_byConnTimeOut+3*(m_nWatchTime+m_nRcvTimeOut)*GetDeviceCount())/1000;
				}
				else
					m_nMaxStandbyLinkOffline = 1*m_byMaxReConn*( m_byConnTimeOut+3*(m_nWatchTime+m_nRcvTimeOut)*GetDeviceCount())/1000;
			}
			else
			{
				m_nMaxStandbyLinkOffline = m_MaxHotLinkOffline;
			}

			if ( m_nSerAddrNum > 0 )
			{
				m_nMaxLinkOfflineConnectCount = m_nSerAddrNum*m_byConnTimeOut*m_byMaxReConn/1000;

				//char pchMsg[256];
				//if (IsEnglishEnv())
				//{
				//	snprintf(pchMsg, sizeof(pchMsg), "CFepModbusDriver::StartWork The Most TCP connnection TimeOut:%d", m_MaxHotLinkOffline);
				//}
				//else
				//{
				//	snprintf( pchMsg, sizeof(pchMsg), 
				//		"StartWork: TCP链路最大离线时间（IP链路个数%d*同一网络最大重连次数%d*(网络连接超时时间%d(毫秒) + 单个设备最大离线次数%d*设备总数%d*超时时间%d(毫秒))）= %d 秒", 
				//		m_nSerAddrNum,m_byMaxReConn,m_byConnTimeOut,m_nMaxOfflineCount, GetDeviceCount(),m_nRcvTimeOut, m_MaxHotLinkOffline );
				//}
				//WriteLogAnyWay(pchMsg);
			}
			else
				m_nMaxLinkOfflineConnectCount = 1*m_byConnTimeOut*m_byMaxReConn/1000;

			//! Connect and Start Timer
			if ( m_nSerAddrNum > 0 )
				Connect();
			////////////////////////////////////////////////////////
		}
		else	//! 是串口则直接发数据
		{	
			m_nSerAddrNum = 1;
			m_MaxHotLinkOffline = m_nMaxOfflineCount*GetDeviceCount()*m_nRcvTimeOut/1000;
			if (m_MaxHotLinkOffline < 2)
			{
				m_MaxHotLinkOffline = 2;
			}
			if ( IsPortAccessClash()  && IsFuzzy() )
			{
				m_nMaxStandbyLinkOffline = 3*GetDeviceCount()*(m_nWatchTime+m_nRcvTimeOut)/1000;
			}
			else
			{
				m_nMaxStandbyLinkOffline = m_MaxHotLinkOffline;
			}

			//char pchMsg[256];
			//if (IsEnglishEnv())
			//{
			//	snprintf(pchMsg, sizeof(pchMsg), "CFepModbusDriver::StartWork The Most Serial Connection OffLine Time:%d", m_MaxHotLinkOffline);
			//}
			//else
			//{
			//	snprintf( pchMsg, sizeof(pchMsg), 
			//		"StartWork: 串行链路最大离线时间（单个设备最大离线次数%d*设备总数%d*超时时间%d(毫秒): %d 秒", 
			//		m_nMaxOfflineCount, GetDeviceCount(),m_nRcvTimeOut, m_MaxHotLinkOffline );
			//}
			//WriteLogAnyWay(pchMsg);

			///////////////////////////////////////////////////////////
			//! 发送数据
			int nDataLen = 0;
			pModbusPrtcl->BuildRequest( pModbusPrtcl->m_pbySndBuffer, nDataLen );
			///////////////////////////////////////////////////////////
		}

		m_bCanWork = true;

		if ( m_nSerAddrNum > 0 )
		{
			pModbusPrtcl->m_pPeriodTimer->SetTimer( PERIOD_SCAN_MS );
		}
		else
		{
			CFepModbusDevice* pDevice = NULL;
			for ( int i = 0; i < GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepModbusDevice*>(GetDeviceByIndex(i));
				if ( NULL != pDevice && !pDevice->IsSimulate() )
				{
					//if ( !pDevice->IsOffLine() )
					//{
					//	char pchMsg[128];
					//	if (IsEnglishEnv())
					//	{
					//		snprintf(pchMsg, sizeof(pchMsg), "CFepModbusDriver::StartWork The Driver is based on TCP, but there is no IP, The Address Device:%s(Address:%d) is offLine!", pDevice->GetName().c_str(), pDevice->GetAddr());
					//	}
					//	else
					//	{
					//		snprintf( pchMsg, sizeof(pchMsg), "CFepModbusDriver::StartWork 驱动基于TCP，但是确没有配置设备IP，设备%s(地址%d)一启动就离线了！", pDevice->GetName().c_str(), pDevice->GetAddr());
					//	}
					//	WriteLogAnyWay(pchMsg);
					//}
					pDevice->SetOnLine( DEVICE_OFFLINE );

					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes = 0;
				}
			}
		}
		
		return 0;
	}

	int CFepModbusDriver::StopWork( void )
	{
		int nResult = -1;
		if (m_pConnTimer)
		{
			m_pConnTimer->KillTimer();
			ACE_OS::sleep(1);
			delete m_pConnTimer;
			m_pConnTimer = NULL;
		}
		nResult = CIODriver::StopWork();
		return nResult;
	}

	int CFepModbusDriver::OpenChannel()
	{
		CIODriver::OpenChannel();

		//!获取驱动的运行环境
		//if (g_pSystemString)
		//{
		//	m_bIsEnglish = g_pSystemString->GetLanguage();
		//}

		m_pIODrvBoard = new CIODrvBoard();
		if ( m_pIODrvBoard == NULL )
		{
			MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CFepModbusDriver::StartWork:m_pIODrvBoard is Null")));
			return -2;
		}

		//! 获得Modbus协议配置信息
		ReadIniCfg();

		CFepModbusPrtcl* pModbusPrtcl = dynamic_cast<CFepModbusPrtcl*>(m_pIOPrtclParser);
		if (!pModbusPrtcl)
		{
			return -1;
		}

		//! 初始化协议类
		pModbusPrtcl->Init( m_byPrtclType, m_nMaxReSend );

		//! 初始化Board
		/*
		std::string strPortType = m_pPortCfg.strType.toUtf8();
		int nRes = m_pIODrvBoard->Init( m_pIOPrtclParser, strPortType, m_pPortCfg, this->GetTimeOut());	
		if (strPortType == "TCPIP" && 0 == nRes)
		{
		if (!m_pPortCfg.strIPA_A.isEmpty())
		{
		m_pIODrvBoard->open();
		}
		else
		{
		OpcUa_CharA pchMsg[256];
		snprintf(pchMsg, 256, "CFepModbusDriver:: %s OpenChannel failed because the IP is empty", GetDriverName().c_str());
		WriteLogAnyWay(pchMsg);
		}
		}
		else if(strPortType == "SERIAL" && 0 == nRes)
		{
		m_pIODrvBoard->open();
		}
		else
		{
		OpcUa_CharA pchMsg[256];
		snprintf(pchMsg, 256, "CFepModbusDriver:: %s OpenChannel failed Because The IODrvBoard Type:%s Init Failed!", GetDriverName().c_str(), strPortType.c_str());
		WriteLogAnyWay(pchMsg);
		return -1;
		}*/
		return 0;
	}

	//! 初始化网络连接参数
	int CFepModbusDriver::InitNetParam()
	{
		m_nConnTimes		= 0;
		m_nSerAddrNum		= 0;
		m_nCurSerAddrIndex	= 0;

		//! 服务器端口号
		m_nPort				= m_pPortCfg.nPort;
		//! 初始化服务器地址列表
		AddSerAddr( m_pPortCfg.strIPA_A.c_str());
		//AddSerAddr( m_pPortCfg.strIPA_B.toUtf8() );
		//AddSerAddr( m_pPortCfg.strIPB_A.toUtf8() );
		//AddSerAddr( m_pPortCfg.strIPB_B.toUtf8() );

		return 0;
	}

	//! 添加服务器
	void CFepModbusDriver::AddSerAddr(const char* pchSerAddr)
	{
		if( pchSerAddr && strcmp( pchSerAddr,"") != 0 
			&& m_nSerAddrNum < MAX_SERVERIP_COUNT )
			m_stSerAddrList[m_nSerAddrNum++] = pchSerAddr;
	}

	//! 封装连接操作，参数为服务器序号
	int CFepModbusDriver::Connect(int nIndex)
	{
		//! 连接锁
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard( m_Mutex );
		if( nIndex < 0 || nIndex >= m_nSerAddrNum )
		{
			return -1;
		}

		//重连前先清除收发计数器。否则，在线设备掉线后重连时，无论是否能连接成功，这些设备都永远在线。
		for ( int i = 0; i < GetDeviceCount(); i++ )
		{
			CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>(GetDeviceByIndex(i));
			if ( pDevice )
			{
				pDevice->m_byTxTimes = 0;
				pDevice->m_byRxTimes = 0;
			}
		}


		//! 起连接定时器. 
		//注意：这里必须先起连接超时定时器，再Connect，而不能反过来。因为，前者当连接成功后，进入连接成功回调函数时试图KillConnTimer时
		//会失败（因为此时实际尚未启动ConnTimer），导致虽然连接成功，但实际仍然会产生ConnTimer的TimeOut事件。2014-03-04 by hxw
		m_pConnTimer->KillTimer();
		int nR = m_pConnTimer->SetTimer( m_byConnTimeOut );
		//if ( nR < 0 )
		//{
		//	//! 记录日志
		//	char pch[256] = {0};
		//	snprintf( pch, 256, "CFepModbusDriver::Connect: SetTimer Error, nRet = %d!", nR );
		//	WriteLogAnyWay( pch );
		//}

		if ( m_nLinkOfflineConnectCount++ >= m_nMaxLinkOfflineConnectCount )
		{
			//! 置设备离线
			for( int i = 0; i < GetDeviceCount(); i++)
			{
				CIODeviceBase* pDevice = GetDeviceByIndex(i);
				if ( !pDevice->IsSimulate() )
				{
					//if ( !pDevice->IsOffLine() )
					//{
					//	char pchMsg[128];
					//	if (IsEnglishEnv())
					//	{
					//		snprintf(pchMsg, sizeof(pchMsg), "CFepModbusDriver::Connect Having Reconnted:%d but failed, Device:%s(Address:%d)is Offline!",m_nMaxLinkOfflineConnectCount,pDevice->GetName().c_str(),pDevice->GetAddr());
					//	}
					//	else
					//	{
					//		snprintf( pchMsg, sizeof(pchMsg), 
					//			"CFepModbusDriver::Connect全部链路重试%d次都没有连接成功，设备%s(地址%d)离线了！", m_nMaxLinkOfflineConnectCount, pDevice->GetName().c_str(), pDevice->GetAddr());
					//	}
					//	
					//	WriteLogAnyWay(pchMsg);
					//}

					pDevice->m_byRxTimes = 0;
					pDevice->m_byTxTimes = 0;

					pDevice->SetOnLine( DEVICE_OFFLINE );
				}
			}

		}

		//char pchLog[MAXDEBUGARRAYLEN] = {0};
		//if (IsEnglishEnv())
		//{
		//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusDriver::Connect Starting Connecting IP:%s Port:%d Index:%d", m_stSerAddrList[nIndex].c_str(), m_nPort, nIndex);
		//}
		//else
		//{
		//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusDriver::Connect 开始连接网络%s，Port=%d, 序号为%d!", m_stSerAddrList[nIndex].c_str(), m_nPort, nIndex);
		//}
		////! 记录日志
		//WriteLogAnyWay( pchLog );

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
	void CFepModbusDriver::ReConnect( bool bNext )
	{
		if (!IsStopWork())
		{
			//! 试连次数加1
			IncrConnTime();

			//! 超过最大试连次数，则试连下一个服务器
			if( bNext || ( m_nConnTimes >= m_byMaxReConn ) )
			{
				m_nConnTimes = 0;
				ConnectNext();
			}
			else
			{
				//! 重连
				Connect(m_nCurSerAddrIndex);
			}	
		}
	}

	/*!
	*	\brief	连接下一个服务器
	*/
	void CFepModbusDriver::ConnectNext()
	{
		m_nConnTimes = 0;
		if( ++m_nCurSerAddrIndex >= m_nSerAddrNum)
			m_nCurSerAddrIndex = 0;

		Connect( m_nCurSerAddrIndex );
		return;
	}

	//! 读取配置文件
	int CFepModbusDriver::ReadIniCfg( )
	{
		//! 获得驱动名
		std::string csDriverName = GetDriverName();
		//! 获取配置文件绝对路径
		std::string file_name = CIODriver::GetFEPRunDir() + MACS_DRIVER_FILENAME;

		m_nWatchTime = GetPrivateInifileInt(
			csDriverName.c_str(), ACE_TEXT("WatchTime"), 30000, file_name.c_str());
		if ( m_nWatchTime < 15000 )
		{
			m_nWatchTime = 30000;
		}

		//! PrtclType
		m_byPrtclType = (Byte)GetPrivateInifileInt(
			csDriverName.c_str(), ACE_TEXT("PrtclType"), 0, file_name.c_str());
		if( m_byPrtclType < 0 || m_byPrtclType > 2 )
			m_byPrtclType = 0;

		//! MaxReConn
		m_byMaxReConn = GetPrivateInifileInt(
			csDriverName.c_str(), ACE_TEXT("MaxReConn"), 0, file_name.c_str());
		if( m_byMaxReConn < 1 )
			m_byMaxReConn = 3;

		//! ConnTimeOut
		m_byConnTimeOut = GetPrivateInifileInt(
			csDriverName.c_str(), ACE_TEXT("ConnTimeOut"), 0, file_name.c_str());
		if( m_byConnTimeOut < 1000 || m_byConnTimeOut > 10000 )
			m_byConnTimeOut = 2000;

		//! MaxReSend
		m_nMaxReSend = GetPrivateInifileInt(
			csDriverName.c_str(), ACE_TEXT("MaxReSend"), 0, file_name.c_str());
		if( m_nMaxReSend < 0 )
			m_nMaxReSend = 0;

		m_nMaxRetryPeriod = GetPrivateProfileInt(csDriverName.c_str(), ACE_TEXT("RetryPeriod"), 5, file_name.c_str());
		if ( m_nMaxRetryPeriod < 5 )
		{
			m_nMaxRetryPeriod = 5;
		}

		char pchLog[128];
		m_nMaxRetryPeriod = m_nMaxRetryPeriod*1000/PERIOD_SCAN_MS; //转化成扫描周期的倍数

		//! 写日志
		//if (IsEnglishEnv())
		//{
		//	snprintf(pchLog, sizeof(pchLog),"\n\nCFepModbusDriver::ReadIniCfg The Diagnosis Period for Offline Device:%d S", m_nMaxRetryPeriod);
		//}
		//else
		//{
		//	snprintf( pchLog, sizeof(pchLog),"\n\nCFepModbusDriver::ReadIniCfg 链路上的离线设备诊断周期=%d S!", m_nMaxRetryPeriod );
		//}
		//WriteLogAnyWay( pchLog );

		m_nRcvTimeOut = GetPrivateProfileInt(csDriverName.c_str(), "RcvTimeOut", -1, file_name.c_str() );
		if ( m_nRcvTimeOut < 50 )
		{
			m_nRcvTimeOut = 200;
		}

		m_nMaxOfflineCount = GetPrivateProfileInt(csDriverName.c_str(), "MaxOfflineCount", -1,file_name.c_str());
		if ( m_nMaxOfflineCount < 3 )
		{
			m_nMaxOfflineCount = 3;
		}

		return 0;
	}

	/*!
	*	\brief	初始化设备
	*	得到设备类名，动态创建设备对象；
	*	得到设备模拟类名，动态创建设备模拟对象；
	*	设置设备的状态结构指针，配置对象指针；
	*	初始化设备
	*
	*	\param	pCfgUnit：设备配置对象指针
	*
	*	\retval	返回值：0--成功，其它失败
	*/
	int CFepModbusDriver::InitUnit(CIOUnit* pCfgUnit)
	{
		if (!pCfgUnit )
		{
			MACS_ERROR(( ERROR_PERFIX
				ACE_TEXT("CFepModbusDriver::InitUnit : Parameter is Null") ));
			//! 输出调试信息
			OutputDebug( "CFepModbusDriver::InitUnit : Parameter is Null" );
			return -1;
		}
		
		int nAddr = pCfgUnit->GetDevAddr();
		std::string strDevName = pCfgUnit->GetUnitName();
		if ( nAddr <= 0 || nAddr > 255 )
		{
			//UaString ustrMsg(UaString("CFepModbusPrtcl::InitUnit Device:%1 Address:%2 is not Right to Modbus Protocol!").arg(strDevName.c_str()).arg(nAddr));
			//WriteLogAnyWay(ustrMsg.toUtf8());
			return -100;
		}

		return CIODriver::InitUnit(pCfgUnit);
	}

	int CFepModbusDriver::WriteLogAnyWay( const char* pchLog, bool bTime )
	{
		//if ( !m_pLog )
		//{
		//	return -1;
		//}

		//m_pLog->WriteLog( pchLog, bTime );

		return 0;
	}

	int CFepModbusDriver::StartConnTimer()
	{
		int nR = -1;
		if (m_pConnTimer)
		{
			nR = m_pConnTimer->SetTimer( m_byConnTimeOut );
			if ( nR < 0 )
			{
				//! 记录日志
				//char pch[256];
				//sprintf( pch, "ERROR：CFepModbusDriver::ReConnect: SetTimer Error, nRet = %d!", nR );
				//WriteLogAnyWay( pch );
			}
		}
		return nR;
	}

	bool CFepModbusDriver::IsLinkOffline()
	{
		if ( GetDeviceCount() <= 0 )
		{
			return false;
		}

		CFepModbusDevice* pDevice = NULL;
		for ( int i = 0; i < GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepModbusDevice*>(GetDeviceByIndex(i));
			if (NULL != pDevice)
			{
				if ( !pDevice->IsOffLine() )
					return false;
			}
		}
		return true;
	}

	bool CFepModbusDriver::IsFuzzy()
	{
		CFepModbusDevice* pDevice = NULL;
		for ( int i = 0; i < GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepModbusDevice*>(GetDeviceByIndex(i));
			if (NULL != pDevice)
			{
				if ( pDevice->IsFuzzy() )
					return true;
			}
		}

		return false;
	}

	bool CFepModbusDriver::CanWork()
	{
		return m_bCanWork;
	}

	int CFepModbusDriver::GetMaxOffLineCount()
	{
		return m_nMaxOfflineCount;
	}

	int CFepModbusDriver::GetRcvTimeOut()
	{
		return m_nRcvTimeOut;
	}

	bool CFepModbusDriver::IsEnglishEnv()
	{
		return m_bIsEnglish;
	}

	//! 定时器超时回调函数
	void OnTmOut(void* p)					
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)p;
		//!关闭定时器
		if(pDriver && !pDriver->IsStopWork())
		{
			//! 连接锁
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard( pDriver->m_Mutex );
			pDriver->GetConnTimer()->KillTimer();

			char pchLog[MAXDEBUGARRAYLEN];
			sprintf( pchLog, "Modbus::OnTmOut: Connecting IP:%s time Out!", pDriver->GetCurServerAddr().c_str() );
			//! 记录日志
			pDriver->WriteLogAnyWay( pchLog );

			//! 如果连接上了，则清重连次数
			if(pDriver->m_pIODrvBoard && (pDriver->m_pIODrvBoard->ConnState()))
			{
				pDriver->SetConnTime( 0 );	
				sprintf( pchLog, "Modbus::OnTmOut: Connect IP:%s successful!，return!", pDriver->GetCurServerAddr().c_str() );
				//! 记录日志
				pDriver->WriteLogAnyWay( pchLog );
				pDriver->SetConnected( true );
				return;
			}
			//! 重连
			pDriver->ReConnect();
		}
	}
}
