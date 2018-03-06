/*!
*	\file	FepIEC103Prtcl.cpp
*
*	\brief	103主站协议类源文件
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Prtcl.cpp 5     11-02-24 11:25a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-02-24 11:25a $
*	$Revision: 5 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#include "FepIEC103Prtcl.h"
#include "FepIEC103Device.h"
#include "FepIEC103Drv.h"
#include "IEC103.h"
#include "PublicFileH.h"
//#include "../../Server/IOFrameAccess/IOFrameAccess.h"

#include <math.h>

namespace MACS_SCADA_SUD
{
	std::string CFepIEC103Prtcl::s_ClassName("CFepIEC103Prtcl");
	void OnTimeOut( void* pParent );			
	void PeriodProc( void* pParent );

	CFepIEC103Prtcl::CFepIEC103Prtcl()
	{
	//	InitPrtclHelper( 5, 64, 1024, 255, 4096 );	//Del by hxw in 2013.7.31
		InitPrtclHelper( 1, 255, 1024, 255, 4096 );	//Add by hxw in 2013.7.31
		m_bRTUChannelTest = false;
		m_byPrtclType = 0;
		m_iConnTimeOut = 2;
		//初始状态	
		m_iCurDevIndex = 0;
		m_bIsAskForSendPacket   = false;
		m_bEnableFaultWave = false;
		m_nWholeCallPeriodASDU7 = 0;
		m_nWholeCallPeriodASDU21 = 0;

		//超时计数器
		m_nTimeOutCounter = 0;
		m_iDevIndex		= 0;
		m_iDevLastIndex = m_iDevIndex;

		m_nMaxLinkOffline = 300;	//初始化为300秒

		m_nCounter = 1;	//初始化为1，第一次不进行校时和总查询

		//超时定时器
		m_pTimeOutTimer = new CIOTimer(OnTimeOut, this, true);
		m_pPeriodTimer = new CIOTimer(PeriodProc, this);
		//m_pFaultLogTimer = new CIOTimer(  FaultLogProc, this ,true);
		m_msFaultLogLifeCycle = 300*1000;	//5分钟

		m_byLastDevAddr = 0;
		m_byLastPeerDevAddr = 0;

		//协助滑动窗口		
		ACE_OS::memset(m_pbyInnerBuf,0,4096);
		m_iInnerLen = 0;

		m_bIsTimeForBroadCheckTime = false;	
		m_bIsBroadingCheckTime = false;
	}

	CFepIEC103Prtcl::~CFepIEC103Prtcl()
	{	
		if ( m_pTimeOutTimer )
		{
			m_pTimeOutTimer->KillTimer();
			//delete m_pTimeOutTimer;
			//m_pTimeOutTimer = NULL;
		}
		if (m_pPeriodTimer)
		{
			m_pPeriodTimer->KillTimer();
			//delete m_pPeriodTimer;
			//m_pPeriodTimer = NULL;
		}
		//if (m_pFaultLogTimer)
		//{
		//	delete m_pFaultLogTimer;
		//	m_pFaultLogTimer = NULL;
		//}
	}

	/*!
	*	\brief	周期检查校时时机
	*	
	*	\param	void* pParent	协议处理对象	
	*
	*	\retval	返回值	无
	*
	*	\note		
	*/
	void PeriodProc( void* pParent )
	{
		//校验参数
		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)pParent;
		if ( pPrtcl == NULL )
		{
			return;
		}

		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(pPrtcl->GetParam());
		if ( !pDrv->CanWork() )
		{
			return;
		}

		if (pPrtcl->m_nMaxLinkOffline <= 0)
		{
			return;
		}

		CFepIEC103Device* pDevice = NULL;
		if ( pPrtcl->m_nCounter % (pPrtcl->m_nMaxLinkOffline/pDrv->m_iSerAddrNum) == (pPrtcl->m_nMaxLinkOffline/pDrv->m_iSerAddrNum) -1 )
		{
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
				if ( NULL != pDevice )
				{
					if ( pDevice->m_byRxTimes == 0 )
					{
						if ( !pDevice->IsOffLine() )
						{
							OpcUa_CharA pchMsg[128];
							snprintf( pchMsg, sizeof(pchMsg), 
								"%d秒没有收到任何设备报文，设备%s(地址%d)可疑！", (pPrtcl->m_nMaxLinkOffline/pDrv->m_iSerAddrNum), pDevice->GetName().c_str(), pDevice->GetAddr() );

							pDrv->WriteLogAnyWay(pchMsg);
						}
						pDevice->SetSuspicious( true );
					}
				}
			}
		}

		if ( pPrtcl->m_nCounter % pPrtcl->m_nMaxLinkOffline == pPrtcl->m_nMaxLinkOffline -1 )
		{
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
				if ( NULL != pDevice  && !pDevice->IsSimulate() )
				{
					if ( pDevice->m_byRxTimes == 0 )
					{
						if ( !pDevice->IsOffLine() )
						{
							OpcUa_CharA pchMsg[128];
							snprintf( pchMsg, sizeof(pchMsg), 
								"超过%d秒都没有收到任何设备报文，设备%s(地址%d)离线了！", pPrtcl->m_nMaxLinkOffline, pDevice->GetName().c_str(), pDevice->GetAddr() );

							pDrv->WriteLogAnyWay(pchMsg);
						}
						pDevice->SetOnLine( DEVICE_OFFLINE );
					}

					//每次检查后复归计数器。否则，部分在线设备掉线后，这些设备都永远在线。
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes = 0;
				}
			}
		}

		//如果链路离线或可疑，则周期尝试重连链路。注意，链路处于初始化态（刚启动时）不能重连链路。
		if ( 1 == pPrtcl->m_byPrtclType && ( pPrtcl->IsLinkOffline() || pPrtcl->IsLinkSuspicious() ) )
		{
			if ( pPrtcl->m_nCounter % 30 == 30-1 //每30秒无条件重连一次
				|| ( pDrv->m_iSerAddrNum > 1 && pDrv->m_iCurSerAddrIndex != 0 )  )	//TCP链路下有多个设备IP时，尝试一遍后再休息，之间不休息。
			{
				if ( !pDrv->m_bIsConnecting )
				{
					pDrv->WriteLogAnyWay( "TCP链路离线或可疑，重连TCP Server端..." );		

					pDrv->m_bIsConnecting = true;
					pDrv->ReConnect( true );
				}
				else
				{
					pDrv->WriteLogAnyWay( "TCP链路正在重连，稍后再尝试重连TCP Server端..." );		
				}
			}
		}
		
		//! 校时周期到
		if( pPrtcl->m_nCheckTimePeriod && pPrtcl->m_nCounter % pPrtcl->m_nCheckTimePeriod == 0 )
		{
			if ( pPrtcl->m_bBroadCheckTime )	//广播对时
			{
				pPrtcl->m_bIsTimeForBroadCheckTime = true;	//只需置链路对时标志
			}
			else	//点对点对时
			{				
				for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
				{
					pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
					if (NULL != pDevice)
					{
						pDevice->m_bNeedCheckTime = true;	//逐个设备置对时标志
					}
				}
			}

			pDrv->WriteLog( "校时周期到了！" );		
		}

		//! ASDU7总查询周期到
/*
		if( pPrtcl->m_nWholeCallPeriodASDU7 && pPrtcl->m_nCounter % pPrtcl->m_nWholeCallPeriodASDU7 == 0 )
		{
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
				pDevice->m_bNeedASDU7GI = true;	
			}

			pDrv->WriteLog( "ASDU7总查询周期到了！" );
		}
*/

		//! ASDU21所有组的总查询周期到
/*
		if( pPrtcl->m_nWholeCallPeriodASDU21 && pPrtcl->m_nCounter % pPrtcl->m_nWholeCallPeriodASDU21 == 0 )
		{
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
				pDevice->m_bNeedASDU21GI = true;	
			}

			pDrv->WriteLog( "ASDU21所有组的总查询周期到了！" );
		}
*/

		//if( pPrtcl->m_nWholeCallPeriodASDU21 && pPrtcl->m_nCounter % pPrtcl->m_nWholeCallPeriodASDU21 == 0 )
		{
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));

				//离线的设备应该发送请求了
				if ( pPrtcl->m_nMaxRetryPeriod && pPrtcl->m_nCounter%pPrtcl->m_nMaxRetryPeriod == 0 )	//
				{
					pDevice->m_bRetryPeriod = true;
				}

				//! ASDU7分组总召唤周期到
				if( pDevice->m_iASDU7ReadGroupPeriod && pPrtcl->m_nCounter % pDevice->m_iASDU7ReadGroupPeriod == 0 )
				{
					pDevice->m_bNeedASDU7GI = true;	

					OpcUa_CharA pchMsg[128];
					snprintf( pchMsg, sizeof(pchMsg), 
						"ASDU7 设备%s总查询周期到了！", pDevice->GetName().c_str() );
					pDrv->WriteLog(pchMsg);

				}

				//! ASDU21分组总召唤周期到
				if( pDevice->m_iASDU21ReadGroupPeriod && pPrtcl->m_nCounter % pDevice->m_iASDU21ReadGroupPeriod == 0 )
				{
					pDevice->m_bNeedASDU21GI = true;	

					OpcUa_CharA pchMsg[128];
					snprintf( pchMsg, sizeof(pchMsg), 
						"ASDU21 设备%s分组总查询周期到了！", pDevice->GetName().c_str() );
					pDrv->WriteLog(pchMsg);

				}
			}
		}


		pPrtcl->m_nCounter++;
	}

	//// **********************************************************************************************
	//// 编 写 者: Hexuewen        函数创建时间: 2008.11.27 
	//// 名    称: FaultLogProc
	//// 功    能: 故障录波日志生命周期函数
	//// **********************************************************************************************
	//void FaultLogProc( void* pParent )
	//{
	//	//校验参数l
	//	CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)pParent;
	//	if ( pPrtcl == NULL )
	//	{
	//		return;
	//	}
	//	CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(pPrtcl->GetParam());

	//	pPrtcl->m_pFaultLogTimer->KillTimer();

	//	CIOChannelBase* p103Channel = pDrv->GetChannelObj();
	//	//del 2011-1018
	//	//p103Channel->SetMonitor( false );
	//	//p103Channel->SetLog( false );

	//	MACS_ERROR((ERROR_PERFIX ACE_TEXT("IEC103 故障录波日志记录生命周期到了")));
	//}

	/*!
	*	\brief	 接收超时处理
	*	
	*
	*	\param	参数	void*pParent: 协议处理对象	
	*
	*	\retval	返回值	无
	*
	*	\note		接收不到数据就再次启动发送
	*/
	void OnTimeOut( void* pParent )
	{
		//校验参数
		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)pParent;
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*) (pPrtcl->m_pParam);
		//CFepIEC103Device* pDevice =dynamic_cast<CFepIEC103Device*>( pDrv->GetDeviceByAddr( pPrtcl->m_byLastDevAddr ) );
		//if (pDevice)
		//{
		//	pDevice->m_pPRDFile->ProduceCfgFile();
		//	pDevice->m_pPRDFile->ProduceDatFile();
		//}
		pDrv->WriteLog( "进入驱动超时定时器回调!!!!" );

		OpcUa_CharA pchMsg[128];
		OpcUa_Int32 nRet = pPrtcl->m_pTimeOutTimer->KillTimer();
		//关定时器
		if ( nRet >= 0 )
		{
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Prtcl::TimeOut KillTimer Success,Timers Count = %d....", nRet );
			pDrv->WriteLog(pchMsg);
		}
		else
		{
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Prtcl::TimeOut KillTimer Failed,Timers Count = %d!!!!!", nRet );
			pDrv->WriteLog(pchMsg);
		}

		//从机判离线需要三次计数，大概90秒
		if( pPrtcl->m_bSendPacketIsFromPeer )
		{
			CFepIEC103Device* pDevice =dynamic_cast<CFepIEC103Device*>( pDrv->GetDeviceByAddr( pPrtcl->m_byLastDevAddr ) );
			if (pDevice)
			{
				pDevice->m_byStandyOffCounter++;
			}			
			if ( pDevice && ( pDevice->m_byStandyOffCounter >= 3) && !pDevice->IsSimulate())
			{
/*	//从机的离线按链路离线时间计算 by hxw in 2014.1.17
				if ( pDevice->IsOnLine() )
				{
					snprintf( pchMsg, sizeof(pchMsg), 
						"CFepIEC103Prtcl::TimeOut: Standby device(Name=%s, Addr=%d) is offline!!!!!", pDevice->GetName().c_str(), pDevice->GetAddr() );
					pDrv->WriteLogAnyWay(pchMsg);
				}

				pDevice->SetOnLine( DEVICE_OFFLINE );
*/
			}
		}

		pPrtcl->m_nTimeOutCounter++;

		OpcUa_Int32 iReturnLen = 0;
		pPrtcl->BuildRequest( pPrtcl->m_pbySndBuffer, iReturnLen );

		//! 启动定时器		
		if ( pPrtcl->m_pTimeOutTimer->SetTimer( pPrtcl->m_nRcvTimeOut ) == 0 )
		{
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Prtcl::TimeOut SetTimer Success,TimerID = %d....", pPrtcl->m_pTimeOutTimer->GetTimerID() );
			pDrv->WriteLog(pchMsg);
		}
		else
		{
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Prtcl::TimeOut SetTimer Failed,TimerID = %d!!!!!", pPrtcl->m_pTimeOutTimer->GetTimerID() );
			pDrv->WriteLog(pchMsg);
		}
	}


	//! 连接成功
	OpcUa_Int32 CFepIEC103Prtcl::OnConnSuccess(std::string pPeerIP )
	{
		OpcUa_CharA chMsg[255];
		CFepIEC103Drv* pDriver = (CFepIEC103Drv*)m_pParam;	
		pDriver->GetConnTimer()->KillTimer();
		snprintf(chMsg,sizeof(chMsg), "%s 连接成功!" ,pPeerIP.c_str());

		pDriver->WriteLogAnyWay(chMsg);		

		pDriver->m_bIsConnecting = false;

		//连接成后，去掉设备可疑状态，复归收发计数器。避免重连成功后再错误的连接。但此处不能置设备在线。
		CFepIEC103Device* pDevice = NULL;
		for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC103Device*>(pDriver->GetDeviceByIndex(i));
			if ( NULL != pDevice )
			{
				pDevice->SetSuspicious( false );
				pDevice->m_byTxTimes = 0;
				pDevice->m_byRxTimes = 0;
			}
		}

		OpcUa_Int32 iReturnLen = 0;
		BuildRequest( m_pbySndBuffer, iReturnLen );

		//! 启动定时器
		if ( m_pTimeOutTimer->SetTimer( m_nRcvTimeOut ) == 0 )
		{
			snprintf( chMsg, sizeof(chMsg), 
				"CFepIEC103Prtcl::OnConnSuccess SetTimer Success,TimerID = %d....", m_pTimeOutTimer->GetTimerID() );
			pDriver->WriteLog(chMsg);
		}
		else
		{
			snprintf( chMsg, sizeof(chMsg), 
				"CFepIEC103Prtcl::OnConnSuccess SetTimer Failed,TimerID = %d!!!!!", m_pTimeOutTimer->GetTimerID() );
			pDriver->WriteLog(chMsg);
		}

		return 0;
	}

	void CFepIEC103Prtcl::InitConfigPar( OpcUa_CharA* strAppName )
	{
		if ( strAppName == NULL )
		{
			return;
		}

		//! 获取配置文件绝对路径
		std::string file_name = CIODriver::GetFEPRunDir() + MACS_DRIVER_FILENAME;

		m_byPrtclType = GetPrivateProfileInt(strAppName, "PrtclType",0, file_name.c_str() );
		if ( m_byPrtclType < 0 || m_byPrtclType > 2 )
		{
			m_byPrtclType = 0;
		}

		//!若物理链路非串口
		if ( 0 != m_byPrtclType )
		{
			//建立连接的超时，毫秒
			m_iConnTimeOut = GetPrivateProfileInt( strAppName, "ConnTimeOut", 2000,  file_name.c_str() );
			if ( m_iConnTimeOut <= 0 )
			{
				m_iConnTimeOut = 30000;
			}

			//同一IP最大重连次数	 
			m_iMaxReConn = GetPrivateProfileInt( strAppName, "MaxReConn", 3,  file_name.c_str() );
			if ( m_iMaxReConn <= 0 )
			{
				m_iMaxReConn = 3;
			}

		}	


		m_nRcvTimeOut = GetPrivateProfileInt(strAppName, "RcvTimeOut", -1, file_name.c_str() );
		if ( m_nRcvTimeOut == -1 ) //没有配置，使用老版本的读法
		{
			m_nRcvTimeOut = GetPrivateProfileInt("FepIEC103", "RcvTimeOut", 500, file_name.c_str() );
		}
		if ( m_nRcvTimeOut < 100 )
		{
			m_nRcvTimeOut = 500;
		}

		m_nCheckTimePeriod = GetPrivateProfileInt(strAppName, "CheckTimePeriod", -1, file_name.c_str());
		if ( m_nCheckTimePeriod == -1 )
		{
			m_nCheckTimePeriod = GetPrivateProfileInt("FepIEC103", "CheckTimePeriod", 300, file_name.c_str());
		}
		if ( m_nCheckTimePeriod < 5 )
		{
			m_nCheckTimePeriod = 300;
		}

		m_nMaxRetryPeriod = GetPrivateProfileInt(strAppName, "RetryPeriod", 1, file_name.c_str());
		if ( m_nMaxRetryPeriod < 0 )
		{
			m_nMaxRetryPeriod = 0;
		}
		m_nMaxRetryPeriod = m_nMaxRetryPeriod*60;	//调整为秒数


/*
		m_nWholeCallPeriodASDU7 = GetPrivateProfileInt(strAppName, "GeneralInquire", -1, file_name.c_str()); 
		if ( m_nWholeCallPeriodASDU7 == -1 )
		{
			m_nWholeCallPeriodASDU7 = GetPrivateProfileInt("FepIEC103", "GeneralInquire", 60, file_name.c_str()); 
		}
		if ( m_nWholeCallPeriodASDU7 != 0 ) //0表示不进行总召唤
		{
			if ( m_nWholeCallPeriodASDU7 < 30 )
			{
				m_nWholeCallPeriodASDU7 = 60;
			}
		}
*/

/*
		m_nWholeCallPeriodASDU21 = GetPrivateProfileInt(strAppName, "ASDU21GIPeriod", -1, file_name.c_str()); 
		if ( m_nWholeCallPeriodASDU21 == -1 )
		{
			m_nWholeCallPeriodASDU21 = GetPrivateProfileInt("FepIEC103", "ASDU21GIPeriod", 60, file_name.c_str()); 
		}
		if ( m_nWholeCallPeriodASDU21 != 0 ) //0表示不进行总召唤
		{
			if ( m_nWholeCallPeriodASDU21 < 30 )
			{
				m_nWholeCallPeriodASDU21 = 60;
			}
		}

		if ( m_nWholeCallPeriodASDU7 != 0 && m_nWholeCallPeriodASDU21 != 0 ) //如果同时要求进行ASDU7和ASDU21的两类总召唤，则控制两类总找之间的间隔
		{
			if ( fabs(double(m_nWholeCallPeriodASDU7-m_nWholeCallPeriodASDU21)) < 30 )
			{
				if ( m_nWholeCallPeriodASDU7 < m_nWholeCallPeriodASDU21 + 30)
				{
					m_nWholeCallPeriodASDU7 = m_nWholeCallPeriodASDU21 + 60;
				}
				else if ( m_nWholeCallPeriodASDU21 < m_nWholeCallPeriodASDU7 + 30)
				{
					m_nWholeCallPeriodASDU21 = m_nWholeCallPeriodASDU7 + 60;
				}
			}
		}
*/

		OpcUa_Int32 bTemp = GetPrivateProfileInt(strAppName, "Wave", -1, file_name.c_str()); 
		if ( bTemp == -1 )
		{
			bTemp = GetPrivateProfileInt("FepIEC103", "Wave", 0,file_name.c_str()); 
		}
		if ( bTemp != 0 && bTemp != 1 )
		{
			bTemp = 0;
		}
		m_bEnableFaultWave = (bTemp)?true:false;
		if ( m_bEnableFaultWave )
		{
			m_nWholeCallPeriodASDU7 = max( FAULT_WAVE_TIMEOUT+2000, m_nWholeCallPeriodASDU7 );	//总召唤的周期不能小于或太接近于故障录播的超时。
		}

		if ( m_bEnableFaultWave )
		{
			CFepIEC103Device* pDevice = NULL;
			CFepIEC103Drv* pDrv = (CFepIEC103Drv*)GetParam();
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
				if (NULL != pDevice)
				{
					if ( pDevice->m_iASDU7ReadGroupPeriod )
					{
						pDevice->m_iASDU7ReadGroupPeriod = max( FAULT_WAVE_TIMEOUT+2000, pDevice->m_iASDU7ReadGroupPeriod );	//总召唤的周期不能小于或太接近于故障录播的超时。
					}

					if ( pDevice->m_iASDU21ReadGroupPeriod )
					{
						pDevice->m_iASDU21ReadGroupPeriod = max( FAULT_WAVE_TIMEOUT+2000, pDevice->m_iASDU21ReadGroupPeriod );	//总召唤的周期不能小于或太接近于故障录播的超时。
					}
				}
			}
		}


		m_nMaxOfflineCount = GetPrivateProfileInt(strAppName, "MaxOfflineCount", -1,file_name.c_str());
		if ( m_nMaxOfflineCount == -1 )
		{
			m_nMaxOfflineCount = GetPrivateProfileInt("FepIEC103", "MaxOfflineCount", 10,file_name.c_str());
		}
		if ( m_nMaxOfflineCount < 3 )
		{
			m_nMaxOfflineCount = 10;
		}

		m_nDORelayTime = 1000*GetPrivateProfileInt(strAppName, "DORelayTime", -1, file_name.c_str()); 
		if ( m_nDORelayTime == -1000 )
		{
			m_nDORelayTime = 1000*GetPrivateProfileInt("FepIEC103", "DORelayTime", 30, file_name.c_str()); 
		}
		if ( m_nDORelayTime < 0 )
		{
			m_nDORelayTime = 30000;
		}
		m_nPeerTimeOut = 2*m_nRcvTimeOut + 100;

		m_nSendDelay = GetPrivateProfileInt(strAppName, "SendDelay", -1, file_name.c_str()); 
		if ( m_nSendDelay == -1 )
		{
			m_nSendDelay = GetPrivateProfileInt("FepIEC103", "SendDelay", 20, file_name.c_str()); 
		}
		if ( m_nSendDelay < 10 )
		{
			m_nSendDelay = 20;
		}

		OpcUa_Int32 bBroadCast = GetPrivateProfileInt(strAppName, "BroadCast", -1, file_name.c_str()); 
		if ( bBroadCast == -1 )
		{
			bBroadCast = GetPrivateProfileInt("FepIEC103", "BroadCast", 1, file_name.c_str()); 
		}
		if ( bBroadCast == 1 )
		{
			m_bBroadCheckTime = true;
		}
		else
			m_bBroadCheckTime = false;

		//录波时记录日志及报文的持续时间。默认300秒。
		OpcUa_Int32 nTemp = GetPrivateProfileInt("FepIEC103", "WaveLogTime", 300,file_name.c_str()); 
		if ( nTemp < 0 )
		{
			nTemp = 0;
		}
		m_msFaultLogLifeCycle = nTemp*1000;

	}


	/*!
	*	\brief	根据设备地址获取设备索引号
	*
	*	详细的函数说明（可选）
	*
	*	\param	参数	OpcUa_Int32 Addr: 设备地址
	*	\param	参数	OpcUa_Int32& DevNo: 设备地址对应的设备索引号		
	*
	*	\retval	返回值	bool 设备地址存在返回true，否则返回false
	*
	*	\note		
	*/
	bool CFepIEC103Prtcl::GetDeviceNoByAddr( OpcUa_Int32 Addr, OpcUa_Int32& DevNo )
	{
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)m_pParam;
		for( OpcUa_Int32 i=0; i< pDrv->GetDeviceCount(); i++ )
		{
			CFepIEC103Device* pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
			if( pDevice && pDevice->GetAddr() == Addr )
			{
				DevNo = i;
				return true;
			}
		}

		return false;
	}


	//激活设备总召唤
	void CFepIEC103Prtcl::ActiveWholeCall( CFepIEC103Device* pDevice )

	{
		if ( !pDevice )
		{
			return;
		}

		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)m_pParam;

		//一次只激活一类总召唤
		if ( pDevice->m_iASDU7ReadGroupPeriod )
		{
			pDevice->m_bNeedASDU7GI = true;

			OpcUa_CharA pchMsg[128];
			snprintf( pchMsg, sizeof(pchMsg), 
				"激活ASDU7 设备%s总查询！", pDevice->GetName().c_str() );
			pDrv->WriteLog(pchMsg);
		}
		else if ( pDevice->m_iASDU21ReadGroupPeriod )
		{
			pDevice->m_bNeedASDU21GI = true;

			OpcUa_CharA pchMsg[128];
			snprintf( pchMsg, sizeof(pchMsg), 
				"激活ASDU21 设备%s组查询！", pDevice->GetName().c_str() );
			pDrv->WriteLog(pchMsg);
		}
		else
		{
			OpcUa_CharA pchMsg[128];
			snprintf( pchMsg, sizeof(pchMsg), 
				"无需激活激活设备%s的ASDU7总查询或者ASDU21组查询！", pDevice->GetName().c_str() );
			pDrv->WriteLog(pchMsg);
		}
	}


	//! 给指定设备组通道诊断数据帧
	void CFepIEC103Prtcl::MakeChannelDiagFrame( CIODeviceBase* pDevice, OpcUa_Byte* pbyData, OpcUa_Int32& nReturnLen )
	{
		CFepIEC103Device* pIEC103Device = NULL;
		pIEC103Device = dynamic_cast<CFepIEC103Device*>(pDevice);
		if (!pIEC103Device)
		{
			return;
		}

		BuildFrame( pbyData, nReturnLen, pIEC103Device, false );
		
		return;
	}

	/*!
	*	\brief	组请求帧并发送
	*	本接口由驱动器在发送数据前调用
	*	缺省实现是判断通道是否被禁止了，如果被禁止则返回-2。
	*
	*	\param	pbyData: 要发送的数据
	*	\param	nDataLen: 要发送的数据的长度
	*
	*	\retval　0为成功，其他失败
	*
	*	\note	
	*/
	OpcUa_Int32 CFepIEC103Prtcl::BuildRequest( OpcUa_Byte* pBuf, OpcUa_Int32& iReturnLen )
	{
		OpcUa_Int32 nRet = 0;
		//! 组帧
		nRet = BuildRequestEx(pBuf, iReturnLen);
		
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*) m_pParam;
		//! 发送
		if( iReturnLen > 0 )
		{
			//! 写报文
			pDrv->WriteCommData( m_pbySndBuffer, iReturnLen, COMMDATA_SND );
			try
			{
				if ( pDrv->m_pIODrvBoard->Write( m_pbySndBuffer, iReturnLen) == 0 )
				{
					pDrv->WriteLog("CFepIEC103Prtcl::BuildRequest Write Success....");
				}
				else
				{
					pDrv->WriteLog("CFepIEC103Prtcl::BuildRequest Write Failed!!!!!");
				}
			}
			catch ( ... )
			{
				pDrv->WriteLog("CFepIEC103Prtcl::BuildRequest Write Exception!!!!!");
			}
		}

		if ( m_bIsBroadingCheckTime )
		{
			pDrv->WriteLog("CFepIEC103Prtcl::BuildRequest is Sending Broad CheckTime Frame....");
			iReturnLen = 0;
			m_bIsBroadingCheckTime = false;

			//! 休眠200ms
			ACE_OS::sleep(ACE_Time_Value(0,200*1000));

			//! 直接发送下一帧
			nRet = BuildRequest( pBuf, iReturnLen );
		}

		return nRet;
	}

	/*!
	*	\brief	组请求帧
	*	本接口由驱动器在发送数据前调用
	*	缺省实现是判断通道是否被禁止了，如果被禁止则返回-2。
	*
	*	\param	pbyData: 要发送的数据
	*	\param	nDataLen: 要发送的数据的长度
	*
	*	\retval　0为成功，其他失败
	*
	*	\note	
	*/
	OpcUa_Int32 CFepIEC103Prtcl::BuildRequestEx( OpcUa_Byte* pBuf, OpcUa_Int32& iReturnLen )
	{
		OpcUa_Int32 iRet = 0;
		iReturnLen	= 0;		
		OpcUa_CharA chMsg[255];

		//遥控、遥调命令总是优先发送
		iRet = BuildCtlRequest( pBuf,iReturnLen );
		if ( iReturnLen >0 && iRet == 0 )
		{
			OpcUa_Int32 i;
			//把逻辑单元号切换为DO、AO所属设备，以便下次就能轮询到该设备。
			if ( GetDeviceNoByAddr( pBuf[5], i ) )		/////////////////////////////////需要修改index	
			{
				m_iDevIndex = i;
			}
			return iRet;
		}
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*) m_pParam;
		CFepIEC103Device* pDevice = NULL;

		pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(m_iDevIndex));//得到当前设备指针;
		if (NULL == pDevice)
		{
			return iRet;
		}

		//! 判断设备是否被模拟了，如果被模拟了，则不发送请求了
		if (pDevice && pDevice->IsSimulate() )
		{
			//! 写日志
			sprintf( chMsg,"CFepIEC103Prtcl::BuildRequest Device Is Simulated!" );
			pDrv->WriteLog( chMsg );
			return -2;
		}

		if( pDrv->IsPortAccessClash() )	//无论串口还是网络，都可能是冲突模式
		{
			//! 进行链路诊断
			OpcUa_Int32 nRest = 0;
			OpcUa_Int32 nLinkDataLen = 0;
			nRest = DealLinkClash( pDevice, pBuf, nLinkDataLen );
			if ( nRest==0 )	//! 组出链路诊断包,并发送了
			{
				//! 置通道诊断标志为true
				m_bRTUChannelTest = true;				

				snprintf(chMsg,sizeof(chMsg),"CFepIEC103Prtcl::BuildRequest 组链路诊断包并发送给另机!" );
				pDrv->WriteLog(chMsg);

				nRest = 0;	

				return 0;
			}

			//! 如果接收到了另机发的诊断包,则直接发送
			if ( m_bSendPacketIsFromPeer && m_cmdPacket.nLen>0 )
			{				
				memcpy( pBuf, m_cmdPacket.byData, m_cmdPacket.nLen );
				//恢复
				iReturnLen = m_cmdPacket.nLen;			

				snprintf(chMsg,sizeof(chMsg),"CFepIEC103Prtcl::BuildRequest 发送链路诊断包!" );
				pDrv->WriteLog(chMsg);

				if (5 == m_cmdPacket.nLen)
				{
					//! 取得发送帧的设备
					pDevice = dynamic_cast<CFepIEC103Device*>( 
						pDrv->GetDeviceByAddr( pBuf[2] ) );
				}
				else
				{
					//! 取得发送帧的设备
					pDevice = dynamic_cast<CFepIEC103Device*>( 
						pDrv->GetDeviceByAddr( pBuf[5] ) );
				}				

				if ( !pDevice )
				{
					m_bSendPacketIsFromPeer = false;
					m_cmdPacket.nLen = 0;			
				}			
				m_bSendPacketIsFromPeer = false;
				m_cmdPacket.nLen = 0;
				if (pDevice)
				{
					m_byLastDevAddr = pDevice->GetAddr();
				}

				m_bRTUChannelTest = true;			//! 置通道诊断标志为true


				m_iInnerLen = 0;	

				return iRet;
			}
		}


		m_iDevLastIndex = m_iDevIndex; //记住上一次的设备
		if ( ( pDrv->IsPortAccessClash() && pDevice->IsHot() ) 
			|| !pDrv->IsPortAccessClash()  )
		{
			do 
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(m_iDevIndex));//得到当前设备指针;

				BuildFrame(pBuf, iReturnLen, pDevice);//每调用一次本函数，m_iDevIndex会指向下一个设备

			    //组帧成功；或者遍历所有设备一次后无论是否组帧成功均退出循环	
				if ( iReturnLen > 0  
					|| pDrv->GetDeviceCount() == 1 
					|| m_iDevLastIndex == m_iDevIndex )
				{
					break;
				}
			}while(true);
		}


		//判断是否在线
		if( iReturnLen > 0 )	
		{
			pDevice->m_byTxTimes ++;
			if (pDevice->m_byTxTimes > m_nMaxOfflineCount  && !pDevice->IsSimulate() )
			{
				if ( pDevice->IsOnLine() )
				{
					OpcUa_CharA pchMsg[128];
					snprintf( pchMsg, sizeof(pchMsg), 
						"%d次都没有收到任何报文，设备%s(地址%d)可疑！", m_nMaxOfflineCount, pDevice->GetName().c_str(), pDevice->GetAddr() );

					pDrv->WriteLogAnyWay(pchMsg);
				}

				pDevice->SetSuspicious( true );
			}

/*	//离线统一在PeriodProc中判读
			if (pDevice->m_byTxTimes > m_nMaxOfflineCount  && !pDevice->IsSimulate() )
			{
				if ( pDevice->IsOnLine() )
				{
					OpcUa_CharA pchMsg[128];
					snprintf( pchMsg, sizeof(pchMsg), 
						"连续%d次都没有收到任何报文，设备%s(地址%d)离线了！", m_nMaxOfflineCount, pDevice->GetName().c_str(), pDevice->GetAddr() );

					pDrv->WriteLogAnyWay(pchMsg);
				}

				pDevice->SetOnLine( DEVICE_OFFLINE );
				pDevice->m_byTxTimes = 0;
				pDevice->m_byRxTimes = 0;
			}
*/
		}

		return 0;
	}


	/*!
	*	\brief	构造所需的帧
	*	
	*	\param	参数	OpcUa_Byte* pBuf: 返回的数据包
	*	\param	参数	OpcUa_Int32& iReturnLen: 返回数据包的长度	 
	*
	*	\retval	返回值	无
	*
	*	\note		
	*/
	void CFepIEC103Prtcl::BuildFrame ( OpcUa_Byte* pBuf, OpcUa_Int32& iReturnLen, CFepIEC103Device* pDevice, bool IsLocal )
	{
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)m_pParam;		
		ACE_OS::memset(pBuf,0,255);
		OpcUa_CharA chMsg[255];

		//给另机的诊断请求总是为二级数据帧。这样丢了也没关系。否则，录波请求丢失时会导致录波获取意外终止。
		if ( !IsLocal )
		{
			//请求二级数据帧
			snprintf(chMsg,sizeof(chMsg),"给另机发送...设备(地址%d)请求二级数据帧.", pDevice->GetAddr());
			pDrv->WriteLog(chMsg);
			iReturnLen = pDevice->BuildLevel2DataFrame( pBuf );
			MakeLog(pBuf, iReturnLen);		 

			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
		}

		//对于离线设备：如果链路上属于部分设备离线,并且离线的设备需要采用大周期的轮询机制，则该设备必须等到大周期到时才能发送诊断帧
		if ( pDevice->IsOffLine() && !IsLinkOffline() && ( m_nMaxRetryPeriod > 0 && !pDevice->m_bRetryPeriod ) )
		{
			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}

			return;
		}

		pDevice->m_bRetryPeriod = false;

		if ( !pDevice->IsOnLine() )
		{			
			snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)初始化(原因:离线).", pDevice->GetAddr());
			pDrv->WriteLog(chMsg);

			pDevice->m_bNew = true;
			pDevice->m_bFirstCT = true;
			iReturnLen = pDevice->BuildResetCUFrame(pBuf);
			MakeLog(pBuf, iReturnLen);
			 
			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}

		if (pDevice->m_bNeedInit)			//初始化
		{
			snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)初始化.", pDevice->GetAddr());
			pDrv->WriteLog(chMsg);
			
			iReturnLen = pDevice->BuildResetCUFrame( pBuf );

			//	pDevice->m_bNeedInit = false;
			MakeLog(pBuf, iReturnLen);			

			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}

		//第X个通道传输请求或者中止
		if (pDevice->m_bChannelReady)		//通道传输准备就绪
		{
			snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)通道传输准备就绪.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);	

			pDevice->m_bChannelReady = false;
			iReturnLen = pDevice->BuildASDU24Frame( pBuf, T_Chanel_Req );

			MakeLog(pBuf, iReturnLen);			
			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}


		//ASDU25 肯定或者否定认可
		if (pDevice->m_bSendDisDataAck)			//“扰动数据认可”标志
		{			
			snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)扰动数据认可标志.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);
			
			pDevice->m_bSendDisDataAck = false;
			iReturnLen = pDevice->BuildDisDataAckFrame( pBuf, pDevice->m_byDisDataAckTOO );

			MakeLog(pBuf, iReturnLen);		

			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;

		}

		//请求带标志的状态变位传输
		if (pDevice->m_bReqStatusAlter)			//带标志的状态变位传输准备就绪
		{			
			snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)带标志的状态变位传输准备就绪.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);			

			pDevice->m_bReqStatusAlter = false;
			iReturnLen = pDevice->BuildASDU24Frame( pBuf, T_StatusChange_Req );

			MakeLog(pBuf, iReturnLen);			
			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;		//仅仅是测试使用
		}

		//扰动数据请求
		if (pDevice->m_bReqDisData)		//扰动数据传输准备就绪
		{			
			snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)扰动数据传输准备就绪.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);
			
			pDevice->m_bReqDisData = false;
			iReturnLen = pDevice->BuildASDU24Frame( pBuf, T_Data_Req );

			MakeLog(pBuf, iReturnLen);
			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;		//仅仅是测试使用
		}

		//故障选择
		if (pDevice->m_bFaultSel)		
		{			
			snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)故障选择.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);

			//故障选择的处理需要进一步的讨论确定
			//EnterCriticalSection( &pDevice->m_FaultWaveSection );
			pDevice->m_wFAN = pDevice->m_FaultDataVec[0].FAN;
			//LeaveCriticalSection( &pDevice->m_FaultWaveSection );

			pDevice->m_bFaultSel = false;
			iReturnLen = pDevice->BuildASDU24Frame( pBuf, T_FaultSelect );		//仅仅是测试使用
			MakeLog(pBuf, iReturnLen);
			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}

		//复位FCB帧
		if (pDevice->m_bSendRestCU)
		{			
			if (  ++pDevice->m_nSendResetCUCounter <= 3 )
			{
				iReturnLen = pDevice->BuildResetFCBFrame( pBuf );				
				snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)复位FCB帧.", pDevice->GetAddr() );				
			}
			else
			{
				pDevice->m_nSendResetCUCounter = 0;
				iReturnLen = pDevice->BuildResetCUFrame( pBuf );				
				snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)重试复位FCB帧超过3次,复位设备.", pDevice->GetAddr() );				
			}

			pDrv->WriteLog( chMsg );
			MakeLog(pBuf, iReturnLen);		

			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}


		//注意：按照103标准，复位（初始化）设备后需要先对时，再进行总查询。by hxw。
		if ( ( m_bIsTimeForBroadCheckTime || pDevice->m_bNeedCheckTime ) //需要校时了
			&& !pDevice->m_bIsRcvingFaultWave && !pDevice->IsStatusUnit() )			//如果正在进行故障录播处理，则暂缓。
		{
			snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)%s校时帧.", pDevice->GetAddr(), (m_bBroadCheckTime)?"广播":"点对点" );
			pDrv->WriteLog(chMsg);

			iReturnLen = pDevice->BuildCheckTimeFrame( pBuf, m_bBroadCheckTime );
			m_bIsTimeForBroadCheckTime = false;
			pDevice->m_bNeedCheckTime = false;

			m_bIsBroadingCheckTime = true;
			MakeLog(pBuf, iReturnLen);
			
			return;
		}

		//请求一级数据帧
		if (pDevice->m_bReq1LevelData)
		{			
			snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)请求一级数据帧.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);

			iReturnLen = pDevice->BuildLevel1DataFrame( pBuf );
			MakeLog(pBuf, iReturnLen);

			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}


		//总查询ASDU7
		if (pDevice->m_bNeedASDU7GI && !pDevice->m_bIsRcvingFaultWave)				
		{			
			snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)总查询帧ASDU7.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);

			iReturnLen = pDevice->BuildGIFrame( pBuf );
			pDevice->m_bNeedASDU7GI = false;
			pDevice->m_IsSOE = false;
			MakeLog(pBuf, iReturnLen);			

			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}

		//总查询ASDU21
		if ( pDevice->m_bNeedASDU21GI && !pDevice->m_bIsRcvingFaultWave)				
		{	
			if ( pDevice->m_vGroupNo.size() > 0 )	//ASDU21: 分组召唤
			{
				
/*	//ASDU21召唤周期到时，强制终止二级数据传输过程，立即开始ASDU21。等ASDU21读取完成后，空闲时再更新二级数据。 hxw 2013.4.14
				if ( pDevice->m_bConfirmed )	//
				{
					//请求二级数据帧
					snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)请求二级数据帧.", pDevice->GetAddr());
					pDrv->WriteLog(chMsg);
					iReturnLen = pDevice->BuildLevel2DataFrame( pBuf );
					MakeLog(pBuf, iReturnLen);		 
				}
				else
*/
				{
					iReturnLen = pDevice->BuildASDU21GroupFrame( pBuf, pDevice->m_vGroupNo[pDevice->m_iGroupIndex]*1000 + 0 );					
					if ( iReturnLen > 0 )
					{
						snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)组号=%d总查询帧ASDU21.", pDevice->GetAddr(), pDevice->m_vGroupNo[pDevice->m_iGroupIndex] );
						pDrv->WriteLog(chMsg);
					}

					pDevice->m_iGroupIndex++;
					if ( pDevice->m_iGroupIndex >= pDevice->m_vGroupNo.size() )
					{
						pDevice->m_iGroupIndex = 0;
						pDevice->m_bNeedASDU21GI = false;
						pDrv->WriteLog("最后一组ASDU21召唤发送完了");
					}
				}
			}
			else	//ASDU21: 总召唤所有组
			{
				snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)总查询帧ASDU21.", pDevice->GetAddr() );
				pDrv->WriteLog(chMsg);

				iReturnLen = pDevice->BuildASDU21WholeCallFrame( pBuf );
				pDevice->m_bNeedASDU21GI = false;
				pDevice->m_IsSOE = false;
				MakeLog(pBuf, iReturnLen);			
			}

			//切换逻辑设备号
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}

		//请求二级数据帧
		snprintf(chMsg,sizeof(chMsg),"发送...设备(地址%d)请求二级数据帧.", pDevice->GetAddr());
		pDrv->WriteLog(chMsg);
		iReturnLen = pDevice->BuildLevel2DataFrame( pBuf );
		MakeLog(pBuf, iReturnLen);		 

		//切换逻辑设备号
		m_iDevIndex ++;
		if ( m_iDevIndex >= pDrv->GetDeviceCount() )
		{
			m_iDevIndex = 0;
		}

		return;
	}


	OpcUa_Int32 CFepIEC103Prtcl::RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nBufLen )
	{

		//! 通道如果被禁止了，则直接返回0
		CFepIEC103Drv* pDriver = (CFepIEC103Drv*)m_pParam;
		if ( !pDriver || !m_pbyInnerBuf || !m_pbyRevBuffer || !m_sPacketBuffer )
			return -1;		
// 		if(  pDriver->GetChannelObj()->IsDisable() )
// 		{
// 			return 0;
// 		}

		if ( !pDriver->CanWork() )
		{
			return 0;
		}

		///////////////////////////////////////////////////////////////////////////////////
		//! 如果需要通信监视，则发送原始数据包
		pDriver->WriteCommData( pDataBuf, nBufLen );

		//把数据放到内部缓冲区
		InnerAddData( pDataBuf, nBufLen );

		//从内部缓冲区中执行滑动窗口，找数据包，注意一次可能会处理多个数据包
		OpcUa_Int32	nReceived = 0;
		do
		{
			//可取的数据长度
			nReceived = PRTCL_RCV_BUFFER_LENGTH - m_nRevLength;
			if ( 0 >= nReceived || nReceived>PRTCL_RCV_BUFFER_LENGTH )
			{
				m_nRevLength = 0;
				break;
			}
			//从内部缓冲区取数分析
			if( InnerRead( m_pbyRevBuffer+m_nRevLength, nReceived, nReceived) && nReceived > 0 )
			{
				m_nRevLength += nReceived;
				while( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH )
				{
					bool bCompleted = false;
					OpcUa_Int32 nPacketLen = 0;
					memset( m_sPacketBuffer, 0, PRTCL_MAX_PACKET_LENGTH );

					//取帧序列中最后一帧作为分析用的数据，前面的帧丢弃掉。
					bool bTemp;
					do{
						if ( m_nRevLength < PRTCL_MIN_PACKET_LENGTH)
						{
							break;
						}

						bTemp = ExtractPacket( m_sPacketBuffer, &nPacketLen );
						if( bTemp )
						{
							//剩下的数据移动到接收缓冲区的开头
							m_nRevLength = m_nRevLength - nPacketLen;
							if (m_nRevLength< 0)
							{	
								m_nRevLength = 0;
							}
							if ( m_nRevLength>PRTCL_RCV_BUFFER_LENGTH )
							{
								m_nRevLength = PRTCL_RCV_BUFFER_LENGTH;
							}

							if ( m_nRevLength > 0 )
							{
								ACE_OS::memcpy( m_pbyRevBuffer, m_pbyRevBuffer+nPacketLen, m_nRevLength );
							}

							bCompleted = true;
						}
					}while(bTemp);

					//找到数据包后处理掉
					if ( bCompleted )
					{
						ProcessRcvdPacket( m_sPacketBuffer, nPacketLen, true );						
						m_nRevLength = 0;
						return 0;
					}
					else
					{
						break;
					}
				}
			}
		}while( 0<nReceived );


		//返回处理包情况
		return -1;
	}



	/*!
	*	\brief	 分析出接收数据包
	*	
	*	\param	参数	OpcUa_Byte* pPacketBuffer: 分析出的数据包
	*	\param	参数	SHORT* pnPacketLength: 数据包长度	
	*
	*	\retval	返回值	bool : 成功true，否则false
	*
	*	\note		注意事项（可选）
	*/
	bool CFepIEC103Prtcl::ExtractPacket(OpcUa_Byte* pPacketBuffer, OpcUa_Int32* pnPacketLength)
	{
		CFepIEC103Drv* pDriver = (CFepIEC103Drv*)(m_pParam);
		pDriver->WriteLog( "CFepIEC103Prtcl::ExtractPacket 收到数据帧....." );

		//校验
		if( !pPacketBuffer || !pnPacketLength || m_nRevLength <= 0 )
			return false;
		//
		bool bACompletePacketRcvd = false;
		OpcUa_Byte * pFrame;
		pFrame = m_pbyRevBuffer;

		//校验帧的合理性

		OpcUa_Int32 nErrorLen = 0;	//已解析过的确认错误的数据大小.
		while( pFrame < m_pbyRevBuffer + m_nRevLength )
		{

			//1、先尝试按照E5/A2帧来解析. 介于E5的特殊性，这里我们可以不认可非首字节为E5/A2作为E5/A2短帧. 因为10或68报文中很可能包好E5/A2字节。
			if ( (pFrame[0] == 0xE5 || pFrame[0] == 0xA2) 
				&& m_nRevLength == 1 )	//如果接收长度大于1,则不考虑E5/A2短帧
			{
				//将合格的帧放置到输出缓冲区
				memcpy(pPacketBuffer, pFrame, 1 );

				//置被处理掉数据的长度
				*pnPacketLength = 1;

				bACompletePacketRcvd = true;
				break;
			}
			else
			{
				//frame type
				OpcUa_Byte frameType = pFrame[0];
				if( frameType != GUDINGZHEN_START_ID && frameType != BIANCHANGZHEN_START_ID )
				{
					pFrame++;
					continue;
				}

				if( frameType == BIANCHANGZHEN_START_ID )
				{
					//len
					if (m_pbyRevBuffer + m_nRevLength - pFrame < 3 )		//第二和第三位是长度位
					{
						return false;
					}
					if ( pFrame[1] != pFrame[2] )
					{
						pFrame++;
						continue;
					}
					if ( m_pbyRevBuffer + m_nRevLength - pFrame < pFrame[1] + 6 )	
					{
						return false;
					}
					//start code
					if ( 0x68 != pFrame[3])
					{
						pFrame++;
						continue;
					}

					//ctrl code
					//Add by hxw in 2007.04.04
/*
					CONTROL_FIELD* pCtrl = (CONTROL_FIELD* )&pFrame[4];
					if ( pCtrl->Prm == 1 )	//主站发送的报文,而不是从站响应的报文.
					{
						pFrame++;
						continue;
					}
*/

					//crc
					OpcUa_Byte bySumCode = Get8SumCode( &pFrame[4], pFrame[1] );
					if( bySumCode != pFrame[pFrame[1]+4] )
					{
						pFrame++;
						continue;
					}

					//end code
					if (  0x16 != pFrame[pFrame[1]+5] )
					{
						pFrame++;
						continue;	
					}
					else
					{
						//将合格的帧放置到输出缓冲区
						memcpy(pPacketBuffer, pFrame, pFrame[1]+6 );

						//置被处理掉数据的长度
						*pnPacketLength =  pFrame[1] + 6;

						bACompletePacketRcvd = true;
					}
				}
				else if( frameType == GUDINGZHEN_START_ID )
				{
					//len
					if( m_pbyRevBuffer + m_nRevLength - pFrame < 5 )
					{
						return false;
					}

					//ctrl code
					//Add by hxw in 2007.04.04
/*
                    CONTROL_FIELD* pCtrl = (CONTROL_FIELD* )&pFrame[1];
					if ( pCtrl->Prm == 1 )	//主站发送的报文,而不是从站响应的报文.
					{
						pFrame++;
						continue;
					}
*/

/*
					if ( pCtrl->FunCode != 0
						&& pCtrl->FunCode != 1 
						&& pCtrl->FunCode != 8 
						&& pCtrl->FunCode != 9 
						&& pCtrl->FunCode != 11 
						&& pCtrl->FunCode != 14 
						&& pCtrl->FunCode != 15 
						)	//非从站响应的合法功能码
					{
						pFrame++;
						continue;
					}
*/
					//crc
					OpcUa_Byte bySumCode = Get8SumCode( &pFrame[1], 2 );
					if( bySumCode != pFrame[3] )
					{
						pFrame++;
						continue;
					}

					//end code
					if( 0x16 != pFrame[4] )
					{
						pFrame++;
						continue;
					}
					else
					{
						//将合格的帧放置到输出缓冲区
						memcpy(pPacketBuffer, pFrame, 5 );

						//置被处理掉数据的长度
						*pnPacketLength =  5;

						bACompletePacketRcvd = true;
					}
				}			
				else
				{
					pFrame++;
					continue;
				}

			}

			if (bACompletePacketRcvd)
			{
				bool bIsMasterMsg = false;		//接收报文是否来自主站
				if (0x68 == pFrame[0] )			//可变长帧
				{
					bIsMasterMsg = IsMasterMsg( pFrame+4);
				}
				else if (0x10 ==pFrame[0])		//固定帧
				{
					bIsMasterMsg = IsMasterMsg( pFrame+1);
				}

				//! 输出收到的完整正确帧内容
				std::string pchMsg;
				OpcUa_CharA chNow[4];
				for ( OpcUa_Int32 k = 0; k < *pnPacketLength; k++ )
				{
					sprintf( chNow, "%02X ", pFrame[k]);
					pchMsg += chNow;
				}
				//! 写日志
				OpcUa_CharA pchLog[1024];
				sprintf( pchLog,"CFepIEC103Prtcl::ExtractPacket Received a complete %s frame:%s!!", bIsMasterMsg?"Master":"Slave", pchMsg.c_str() );
				pDriver->WriteLog( pchLog );

				//如果是来自另机的主站发送帧，保存帧中的设备地址，然后丢弃该帧，继续解析！
				if ( bIsMasterMsg )
				{
					if (0x68 == pFrame[0] )					//可变长帧
					{
						m_byLastPeerDevAddr = pFrame[5];
					}
					else if (0x10 ==pFrame[0])		//固定帧
					{
						m_byLastPeerDevAddr = pFrame[2];
					}

					//安全起见，可先覆盖本地发送区。万一后续又有新的发送时，则本操作自动失效。
					//本操作可以兼容以下现象：可识别出E5认为是设备1发送的
					//主机收发到以下报文：
					//2014-01-16 11:07:10:621121 SND: 10 40 02 42 16 
					//2014-01-16 11:07:10:673946 RCV: 10 5B 01 5C 16 E5 
					//如果不这么做，那么主机会认为E5是设备2发送的，这是错误的！
					memcpy( m_pbySndBuffer, pFrame, *pnPacketLength );	

					m_nRevLength -= *pnPacketLength;
					if ( m_nRevLength > 0 )
					{				
						ACE_OS::memcpy(m_pbyRevBuffer, &m_pbyRevBuffer[*pnPacketLength], m_nRevLength);
					}
					else
						m_nRevLength = 0;

					//上面已经处理了，那么丢弃该帧
					bACompletePacketRcvd = false;
					*pnPacketLength = 0;
					continue;
				}

				break;
			}
		}

		//去掉已明确错误的数据部分
		nErrorLen = pFrame - m_pbyRevBuffer;
		if ( nErrorLen > 0 )
		{
			m_nRevLength -= nErrorLen;
			if ( m_nRevLength > 0 )
			{				
				ACE_OS::memcpy(m_pbyRevBuffer, &m_pbyRevBuffer[nErrorLen], m_nRevLength);
			}
			else
				m_nRevLength = 0;
		}

		return bACompletePacketRcvd;
	}



	/*!
	*	\brief	 处理完整的数据包
	*
	*	详细的函数说明（可选）
	*
	*	\param	参数	OpcUa_Byte* pPacketBuffer: 数据包
	*	\param	参数	SHORT nPacketLength: 数据包长度
	*	\param	参数	bool bFromLocal: 区分是本机来的还是另机来的
	*
	*	\retval	返回值	无
	*
	*	\note		ASDU1、ASDU2、ASDU41、ASDU43 SOE时标来自设备
	*/
	void CFepIEC103Prtcl::ProcessRcvdPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32 nPacketLength, bool bFromLocal )
	{
		//校验
		if( !pPacketBuffer || nPacketLength == 0 )
			return;

		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)m_pParam;

		OpcUa_CharA chMsg[255];
		try
		{
			OpcUa_Int32 nRet = m_pTimeOutTimer->KillTimer();
			//关定时器
			if ( nRet >= 0 )
			{
				snprintf( chMsg, sizeof(chMsg), 
					"CFepIEC103Prtcl::ProcessRcvdPacket KillTimer Success,Timers Count = %d....", nRet );
				pDrv->WriteLog(chMsg);
			}
			else
			{
				snprintf( chMsg, sizeof(chMsg), 
					"CFepIEC103Prtcl::ProcessRcvdPacket KillTimer Failed,Timers Count = %d!!!!!", nRet );
				pDrv->WriteLog(chMsg);
			}
		}
		catch ( ... )
		{
			pDrv->WriteLog("CFepIEC103Prtcl::ProcessRcvdPacket KillTimer Exception!");
		}

		OpcUa_Byte byTrue =1;
		OpcUa_Byte byFalse = 0;
		
		//地址域
		OpcUa_Byte byUnitAddr = 0;
		//控制域
		CONTROL_FIELD* pCtrl = NULL;

		//帧类型代号
		OpcUa_Int32 iFrameType = pPacketBuffer[0];

		bool bIsOnebyteResp = false;	//是否一个字节的响应帧
		if (0x68 == iFrameType)					//可变长帧
		{
			byUnitAddr = pPacketBuffer[5];
			pCtrl	= (CONTROL_FIELD* )&pPacketBuffer[4];

			//收到设备的10或68报文后，清除本地发送缓存，避免以下情况时误认为E5是设备2的应答：
			//2014-01-22 17:13:34:379619 RCV: 10 40 02 42 16 
			//2014-01-22 17:13:34:888785 RCV: 68 0D 0D 68 53 01 15 81 2A 01 FE F1 00 01 01 00 01 07 16 E5 
			memset( m_pbySndBuffer, 0, PRTCL_SND_BUFFER_LENGTH );

			/*		//此处可以去掉. 因为在解析部分已经过滤掉了主站发送帧(Prm=1)
			if ( (pPacketBuffer[6] == TYP_TIME_SYN) || (pPacketBuffer[6] == TYP_GENERAL_QUERY_START) || (pPacketBuffer[6] == TYP_DISORDER_DATA_TRANS) ||
			(pPacketBuffer[6] == TYP_DISORDER_DATA_ACK) ||(pPacketBuffer[6] == TYP_GENERAL_CMD) )
			{
			return;
			}
			*/
		}
		else if (0x10 ==iFrameType)		//固定帧
		{
			byUnitAddr = pPacketBuffer[2];
			pCtrl	= (CONTROL_FIELD* )&pPacketBuffer[1];

			//收到设备的10或68报文后，清除本地发送缓存，避免以下情况时误认为E5是设备2的应答：
			//2014-01-22 17:13:34:379619 RCV: 10 40 02 42 16 
			//2014-01-22 17:13:34:888785 RCV: 68 0D 0D 68 53 01 15 81 2A 01 FE F1 00 01 01 00 01 07 16 E5 
			memset( m_pbySndBuffer, 0, PRTCL_SND_BUFFER_LENGTH );
		}
		else if ( 0xE5 == iFrameType || 0xA2 == iFrameType )	//一个字节的响应帧。这是设备地址从上次发送报文从获取
		{
			bIsOnebyteResp = true;
			
			CFepIEC103Device* pDevAny =dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex( 0 ));

			//Add by hxw in 2014.2.21. 如果从机收到E5短帧，一律丢弃，不作为在线依据，原因是实际通信时E5帧可能存在错拍接收，从而导致设备误诊的情况。
			if ( !pDevAny->IsHot() )
			{
				pDrv->WriteLog("CFepIEC103Prtcl::ProcessRcvdPacket: Received message from Standby link  is E5 or A2 , so abort!");
				return;
			}

			if ( pDevAny->IsHot() || m_bRTUChannelTest )
			{
				if (0x68 == m_pbySndBuffer[0])			//可变长帧
				{
					byUnitAddr = m_pbySndBuffer[5];
				}
				else if (0x10 ==m_pbySndBuffer[0])		//固定帧
				{
					byUnitAddr = m_pbySndBuffer[2];
				}
			}
			else //从机如果收到一个字节的短帧，但上次的请求又不是本机发送的，该设备必然是上次从接收报文中解析得到的另机发送的报文地址。
			{
				byUnitAddr = m_byLastPeerDevAddr;
			}
		}
		
		CFepIEC103Device* pDevice =dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByAddr( byUnitAddr ));
		if ( !pDevice )
		{
			pDrv->WriteLog("CFepIEC103Prtcl::ProcessRcvdPacket: can't get device from received message , so abort!");
			return;
		}

		if (!pDevice->IsHot())
		{
			if ( !pDevice->IsOnLine() )
			{
				OpcUa_CharA pchMsg[128];
				snprintf( pchMsg, sizeof(pchMsg), 
					"设备%s(地址%d)在线了！", pDevice->GetName().c_str(), pDevice->GetAddr() );
				pDrv->WriteLogAnyWay(pchMsg);
			}

			//从设备只要收非一个字节的短帧报文，就
			pDevice->SetSuspicious( false );
			pDevice->SetOnLine( DEVICE_ONLINE );
			pDevice->m_byTxTimes = 0;
			pDevice->m_byRxTimes=1;

			//! 如果是从机且收到的是通道诊断帧的响应，则清通道诊断标志
			if(  m_bRTUChannelTest )
			{
				m_bRTUChannelTest = false;
				pDevice->SetSlaveTimeOuts( 0 );
			}

			/////////////////////////////////////////
			//! 发送下一帧
			OpcUa_Int32 iReturnLen = 0;
			BuildRequest( m_pbySndBuffer, iReturnLen);

			//! 启动定时器
			if ( m_pTimeOutTimer->SetTimer( m_nRcvTimeOut ) == 0 )
			{
				snprintf( chMsg, sizeof(chMsg), 
					"CFepIEC103Prtcl::ProcessRcvdPacket Slave  SetTimer Success,TimerID = %d....", m_pTimeOutTimer->GetTimerID() );
				pDrv->WriteLog(chMsg);
			}
			else
			{
				snprintf( chMsg, sizeof(chMsg), 
					"CFepIEC103Prtcl::ProcessRcvdPacket Slave  SetTimer Failed,TimerID = %d!!!!!", m_pTimeOutTimer->GetTimerID() );
				pDrv->WriteLog(chMsg);
			}

			return;
		}

		pDevice->ReverseFCB();

		//设置装置在线
		if( bFromLocal )
		{
			if ( !pDevice->IsOnLine() )
			{
				OpcUa_CharA pchMsg[128];
				snprintf( pchMsg, sizeof(pchMsg), 
					"设备%s(地址%d)在线了！", pDevice->GetName().c_str(), pDevice->GetAddr() );
				pDrv->WriteLogAnyWay(pchMsg);
			}

			pDevice->SetSuspicious( false );
			pDevice->SetOnLine( DEVICE_ONLINE );
			pDevice->m_byTxTimes = 0;
			pDevice->m_byRxTimes=1;
		}


		if ( bIsOnebyteResp )
		{
			//如果收到的是一字节短帧，则下次的一二级数据策略为：反转，即上次请求一级数据，这次就改请求二级数据；上次请求二级数据，这次就改请求一级数据
			if ( pDevice->m_bReq1LevelData )
			{
				pDevice->m_bReq1LevelData = false;
				pDevice->m_bReq2LevelData = true;
				pDrv->WriteLog("上次收到的是一字节短帧应答，而上次请求为一级数据，这次就改请求二级数据");
			}
			else
			{
				pDevice->m_bReq1LevelData = true;
				pDevice->m_bReq2LevelData = false;
				pDrv->WriteLog("上次收到的是一字节短帧应答，而上次请求为二级数据，这次就改请求一级数据");
			}
		}
		else
		{//非一字节的正常帧，则需处理控制字段信息CONTROL_FIELD

			pDrv->WriteLog("非一字节的正常帧，则需处理控制字段信息CONTROL_FIELD");

			//复位复位标志或一级/二级数据请求标志
			pDevice->m_bSendRestCU = false;	//Del by hxw.2007.10.16
			pDevice->m_bReq1LevelData = false;
			pDevice->m_bReq2LevelData = false;

			//获得控制字段
			OpcUa_Byte tempACD = pCtrl->FCB_ACD;
			OpcUa_Byte tempDFC = pCtrl->FCV_DFC;
			OpcUa_Byte byFunc  = pCtrl->FunCode;

			pDevice->m_bConfirmed = false;	//先复位

			//检查功能码
			switch (byFunc)
			{
			case FRAME_FUNC_CONFIRM:
				pDevice->m_bConfirmed = true;	
				pDevice->m_bNeedInit = false;
				if ( pDevice->m_bSendRestCU )
				{
					pDevice->m_bSendRestCU = false;
				}
				if (pDevice->m_bInDO)	//DO命令的确认，说明设备正确执行
				{
					pDevice->m_bInDO = false;
					pDevice->SetVal(MacsDIS103_DIC, pDevice->m_iInDOIndex, &byTrue,1,0, true);
					if (pDevice->TagNeedSOE(MacsDIS103_DO,pDevice->m_iInDOIndex)
						|| pDevice->TagNeedDevSOE(MacsDIS103_DIC,pDevice->m_iInDOIndex))
					{					
// 						CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 							pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)byTrue, 0);		//! 发送SOE
					}
				}
				break;
			case FRAME_FUNC_DENY:
				if (pDevice->m_bInDO)	//DO命令的确认，说明设备正确执行
				{
					pDevice->m_bInDO = false;
					pDevice->SetVal(MacsDIS103_DIC, pDevice->m_iInDOIndex, &byFalse,1,0, true);
				}
				else
				{
					pDevice->m_bSendRestCU = true;	//Add by hxw 2007.10.16				
					snprintf(chMsg,sizeof(chMsg)
						,"设备(地址%d)控制域功能码为1，指示链路忙，尝试RestCU！", pDevice->GetAddr());
					pDrv->WriteLog(chMsg);
				}
				break;
			case FRAME_FUNC_RET_DATA:
				pDevice->m_bConfirmed = true;	
				break;
			case FRAME_FUNC_NO_DATA:
				/*	//Del by hxw in 2008.05.04
				pDevice->m_bSendRestCU = true;	//Add by hxw 2007.10.16
				strLog.Format( "设备(地址%d)控制域功能码为9，指示无数据应答，尝试RestCU！", pDevice->GetAddr() );
				*/
				break;
			case FRAME_FUNC_ACK_REQUEST:
				break;
			case 15:
				break;
			default:
				break;
			}


			//!继电保护设备希望向用户传送一级用户数据
			if (1 == tempACD)
			{			
				if (1 == tempDFC)
				{
					//!继电保护设备的缓冲区已满
					pDevice->m_bSendRestCU = true;
					pDrv->WriteLog("设备请求Reset！");
				}
				else
				{
					//!继电保护设备可以接受新数据
					pDevice->m_bReq1LevelData = true;
					pDevice->m_nSendResetCUCounter = 0;
				}			
			}
			else//!继电保护设备希望向用户传送二级用户数据
			{			
				if (1 == tempDFC)
				{
					pDevice->m_bSendRestCU = true;
					pDrv->WriteLog("设备请求Reset！");
				}
				else
				{
					pDevice->m_bReq2LevelData = true;
					pDevice->m_nSendResetCUCounter = 0;
				}
			}
		}

		WORD *pGIN = NULL;
		OpcUa_Byte* pByte = NULL;
		OpcUa_Int32 ibase = -1;

		CIOCfgTag* pTag = NULL;

		OpcUa_Byte bDIValue = 0;
		OpcUa_Byte Time[4] = {0};
		OpcUa_Byte bAINum = 0;
		OpcUa_Int32 iTagIndex = 0;
		OpcUa_Int32 iSecond = 0;		

		switch ( pPacketBuffer[0] )
		{
		case 0xE5:				//一个字节的响应帧
		case 0xA2:				//一个字节的响应帧
			break;
		case GUDINGZHEN_START_ID:				//固定帧
			break;
		case BIANCHANGZHEN_START_ID:			//可变长帧
			{
				CHANGEABLE_FRAME_STRUCT *RevFrame = (CHANGEABLE_FRAME_STRUCT *)pPacketBuffer;
				ASDU_HEAD_STRUCT *ReplyData = (ASDU_HEAD_STRUCT *)RevFrame->UserData;

				//获得ASDU Head元素
				OpcUa_Byte TypeFlg = ReplyData->TypeFlg;			//类型标识
				OpcUa_Byte COT	  = ReplyData->COT;				//传送原因
				OpcUa_Byte InfoSum = (ReplyData->VSQ&0x7F);		//VSQ的低7位表示信息元素的个数
				OpcUa_Byte bFunType	  = ReplyData->FunType;		//功能类型
				OpcUa_Int32 InfoNo		  = ReplyData->InfNumber;	//信息序号
				bool bInfoCompressed  = (ReplyData->VSQ&0x80)==0;	//false表示Inf不连续，此时每个点均带Info.

				OpcUa_Int32 iStartTagAddr = bFunType*1000 + InfoNo;	//其实标签地址。仅用于Tag%U(0,0,255)%*1000:%+U(0,0,255)型标签。
				switch (pPacketBuffer[6])			//ASDU类型
				{
				case TYP_WITH_TIME:		//ASDU1
					{
						ASDU1_DPItm *pPoint = (ASDU1_DPItm *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( ASDU1_DPItm ) )
						{
							snprintf(chMsg,sizeof(chMsg)
								,"设备(地址%d)变位DI ASDU1长度非法!", pDevice->GetAddr());
							pDrv->WriteLog(chMsg);
							
							break;
						}

						std::string strCOT = "未知";
						if ( COT == 1 )
						{
							strCOT = "变位突发";
						}
						else if ( COT == 9 )
						{
							strCOT = "总召唤";
						}
						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)DI ASDU1(传送原因%d(%s)信息个数%d,功能类型%d,信息序号%d,值%d)!"
							, pDevice->GetAddr(),COT, strCOT.c_str(), InfoSum, bFunType, InfoNo, pPoint->DPI.DPI -1 );
						pDrv->WriteLog(chMsg);

						for ( OpcUa_Int32 i = 0; i < InfoSum; i++ )
						{
							if( pPoint->DPI.DPI == 1 || pPoint->DPI.DPI == 2 )	//valid
							{
								bDIValue = pPoint->DPI.DPI -1;	//把双点值转成与单点值的含义一致

								//找到标签
								iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
								if (-1 == iTagIndex)
								{									
									snprintf(chMsg,sizeof(chMsg)
										,"ASDU1: DI点[功能类型%d,信息序号%d,值%d]未配置!", bFunType, InfoNo, bDIValue);
									pDrv->WriteLog(chMsg);
								}
									
								OpcUa_Int64 llTime= 0;
								if ( pPoint->Time.IV == 1 )			//时间无效
								{
									pDevice->m_bNeedCheckTime = true;												
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI变位 报文中IV=1, ASDU%d, 提示时间无效, 需要对时!", pDevice->GetAddr(), 1);
									pDrv->WriteLog(chMsg);
								}
								else if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
									pDrv->WriteLog(chMsg);

									WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
									OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;

									ACE_Time_Value val = ACE_OS::gettimeofday();	
									OpcUa_Int64 llTime = val.sec()*1000LL + val.usec()/1000LL;	
									ACE_Date_Time dtime;
									dtime.update( val );

									llTime -= ( dtime.hour()*3600000 +dtime.minute()*60000+dtime.second()*1000+dtime.microsec()/1000);
									llTime += nTime;
									//llTime += 28800000;
								}
								else
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI,ASDU%d, 报文中时间有效, 但为夏时制!",pDevice->GetAddr(), 1 );
									pDrv->WriteLog(chMsg);
								}

								if ( -1 != iTagIndex )
								{
									pDevice->SetVal( MacsDIS103_DI,iTagIndex,  &(pDevice->m_DIs[iTagIndex] = bDIValue),sizeof(OpcUa_Byte),llTime , bFromLocal );
									
									if ( (pDevice->TagNeedSOE(MacsDIS103_DI,iTagIndex)
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI,iTagIndex))
										&&bDIValue)
									{
// 										CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 											pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)bDIValue, llTime);		//! 发送SOE
									}
								}

								pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo,  &bDIValue, sizeof(OpcUa_Byte), llTime , bFromLocal );
								OpcUa_Float fValue = (OpcUa_Float)pPoint->DPI.DPI;
								pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo,  (OpcUa_Byte*)&fValue, sizeof(fValue), llTime , bFromLocal );

								OpcUa_Byte Second;
								WORD MilliSecond;
								pDevice->GetTimeFromCP32Time( pPoint->Time, Second, MilliSecond );									
								snprintf(chMsg,sizeof(chMsg)
									,"ASDU1: 更新DI点[功能类型%d,信息序号%d,值%d,秒:%d,毫秒:%d]到IO!"
									, bFunType, InfoNo, bDIValue,Second,MilliSecond);
								pDrv->WriteLog(chMsg);
							}
							pPoint++;
							InfoNo++;
						}
					}
					break;
				case TYP_WITH_TINE_REL:		//ASDU2
					{
						ASDU2_DPItm *pPoint = (ASDU2_DPItm *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( ASDU2_DPItm ) )
						{							
							snprintf(chMsg,sizeof(chMsg)
								,"设备(地址%d)变位DI ASDU2长度非法!", pDevice->GetAddr());
							pDrv->WriteLog(chMsg);
							
							break;
						}

						std::string strCOT = "未知";
						if ( COT == 1 )
						{
							strCOT = "变位突发";
						}
						else if ( COT == 9 )
						{
							strCOT = "总召唤";
						}

						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)变位DI ASDU2(传送原因%d(%s)信息个数%d,功能类型%d,信息序号%d,值%d)!"
							, pDevice->GetAddr(),COT, strCOT.c_str(), InfoSum, bFunType, InfoNo, pPoint->DPI.DPI -1 );
						pDrv->WriteLog(chMsg);
						
						for ( OpcUa_Int32 i = 0; i < InfoSum; i++ )
						{
							if( pPoint->DPI.DPI == 1 || pPoint->DPI.DPI == 2 )	//valid
							{
								bDIValue = pPoint->DPI.DPI -1;	//把双点值转成与单点值的含义一致

								//找到标签
								iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
								if (-1 == iTagIndex)
								{									
									snprintf(chMsg,sizeof(chMsg)
										,"ASDU2: DI点[功能类型%d,信息序号%d,值%d]未配置!", bFunType, InfoNo, bDIValue);
									pDrv->WriteLog(chMsg);
								}

								OpcUa_Int64 llTime = 0;
								if ( pPoint->Time.IV == 1 )			//时间无效
								{
									pDevice->m_bNeedCheckTime = true;												
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI变位 报文中IV=1, ASDU%d, 提示时间无效, 需要对时!", pDevice->GetAddr(), 2);
									pDrv->WriteLog(chMsg);
								}
								else if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 2);
									pDrv->WriteLog(chMsg);

									WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
									OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;									
									
									ACE_Time_Value val = ACE_OS::gettimeofday();	
									OpcUa_Int64 llTime = val.sec()*1000LL + val.usec()/1000LL;	
									ACE_Date_Time dtime;
									dtime.update( val );

									llTime -= ( dtime.hour()*3600000 +dtime.minute()*60000+dtime.second()*1000+dtime.microsec()/1000);
									llTime += nTime;
								}
								else
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI,ASDU%d, 报文中时间有效, 但为夏时制!",pDevice->GetAddr(), 2 );
									pDrv->WriteLog(chMsg);
								}										
								
								if ( -1 != iTagIndex )
								{
									pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = bDIValue),sizeof(OpcUa_Byte), llTime , bFromLocal);
									if ( (pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
										&&bDIValue)
									{							
																			
// 										CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 											pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)bDIValue, llTime);		//! 发送SOE
																	
									}
								}

								pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo,  &bDIValue, sizeof(OpcUa_Byte), llTime , bFromLocal );
								OpcUa_Float fValue = (OpcUa_Float)pPoint->DPI.DPI;
								pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo,  (OpcUa_Byte*)&fValue, sizeof(fValue), llTime , bFromLocal );

								OpcUa_Byte Second;
								WORD MilliSecond;
								pDevice->GetTimeFromCP32Time( pPoint->Time, Second, MilliSecond );									
								snprintf(chMsg,sizeof(chMsg)
									,"ASDU2: 更新DI点[功能类型%d,信息序号%d,值%d,秒:%d,毫秒:%d]到IO!"
									, bFunType, InfoNo, bDIValue,Second,MilliSecond);
								pDrv->WriteLog(chMsg);
							}
							pPoint++;
							InfoNo++;
						}
					}
					break;

				case TYP_VALUE_WITH_TIME:		//ASDU4
					{
						bFunType = pPacketBuffer[10];
						InfoNo = pPacketBuffer[11];
						bAINum = (pPacketBuffer[7]&0x7F);

						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)带相对时间的有时标的被测值报文ASDU4(Fun:%d,Info:%d,信息个数%d)!"
							, pDevice->GetAddr(),bFunType,InfoNo,InfoSum);
						pDrv->WriteLog(chMsg);

						pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo, pPacketBuffer+12,sizeof(OpcUa_Float),0, bFromLocal );

						if ((bFunType != pDevice->m_byAIFUN)||(InfoNo != pDevice->m_byAISEQ) )
							break;

						if( bAINum != 1 )
							break;

						pDevice->SetVal( MacsDIS103_AI, 14, pPacketBuffer+12,sizeof(OpcUa_Float),0, bFromLocal );
					}
					break;
				case TYP_VALUE1:		//ASDU3
				case TYP_VALUE2:		//ASDU9
					{
						bFunType = pPacketBuffer[10];
						InfoNo = pPacketBuffer[11];
						bAINum = (pPacketBuffer[7]&0x7F);

/*
						if (bAINum > 16|| bAINum < 3)
							break;
*/

						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)AI ASDU9(信息个数%d功能类型%d,信息序号%d)!"
							, pDevice->GetAddr(), InfoSum, bFunType, InfoNo);
						pDrv->WriteLog(chMsg);

						MEA_AI* pAI = (MEA_AI* )(pPacketBuffer+12);
						for( OpcUa_Int32 i = 0; i < bAINum; i++ )
						{
							if ( pAI->ER == 0 )	//valid
							{
								//((FLOAT)(pAI->Value)) / 4095 *12/10;	//归一化值
								OpcUa_Float temp = (pDevice->GetModulusByTag(i+1))*((OpcUa_Float)(pAI->Value));
								//	OpcUa_Float temp = ((OpcUa_Float)(pAI->Value));	//根据量程上下限自动计算工程值。
								//	pDevice->SetVal( MacsDIS103_AI, iStartTagAddr+i, temp, bFromLocal );	//可以考虑换成通用的
								pDevice->SetVal( MacsDIS103_AI, i+1, (OpcUa_Byte*)&temp,sizeof(temp),0, bFromLocal );

								OpcUa_Float fVal = ((OpcUa_Float)(pAI->Value));
								pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo, (OpcUa_Byte*)&fVal,sizeof(fVal),0, bFromLocal );
							}
							InfoNo++;
							pAI++;
						}
					}
					break;
				case TYP_IDENTIFIER:						//ASDU5，标识报文
					{
						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)制造商标识报文ASDU5!.", pDevice->GetAddr());

						pDrv->WriteLog( chMsg );
						MakeLog(pPacketBuffer, nPacketLength, false);

						if ( pDevice->m_bNew )
							pDevice->m_bNew = false;
						else									//如果不是新添加的设备，就不用立刻做校时和总查询了
							break;
						if ( COT_CU == pPacketBuffer[8] )
						{
							//激活设备总召唤
							ActiveWholeCall( pDevice );

							if ( m_bBroadCheckTime ) 
							{
								m_bIsTimeForBroadCheckTime = true;
							}
							else
								pDevice->m_bNeedCheckTime = true;

							pDrv->WriteLog( "ASDU5触发对时！" );
						}
						if ( COT_START == pPacketBuffer[8]|| COT_POWERON == pPacketBuffer[8])		//初始化结束
						{
							//激活设备总召唤
							ActiveWholeCall( pDevice );

							if ( m_bBroadCheckTime )
							{
								m_bIsTimeForBroadCheckTime = true;
							}
							else
								pDevice->m_bNeedCheckTime = true;

							pDrv->WriteLog( "ASDU5触发对时！" );
						}
					}
					break;
				case TYP_TIME_SYN:							//ASDU6，时间同步反馈报文
					{						
						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)时间同步反馈报文(控制域功能码%d)!."
							, pDevice->GetAddr(), pCtrl->FunCode );
						pDrv->WriteLog( chMsg );
						MakeLog(pPacketBuffer, nPacketLength, false);
					}
					break;
				case TYP_GENERAL_QUERY_END:			//总查询（总召唤）终止报文类型标识  ASDU8	
					{
						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)总查询（总召唤）终止!."
							, pDevice->GetAddr() );
						pDrv->WriteLog( chMsg );
						MakeLog(pPacketBuffer, nPacketLength, false);
						
						//pDevice->SetCanSendSoe(true);
					}
					break;
				case TYP_DISORDER_DATA:				//被记录的扰动表	ASDU23
					{
						if ( IsEnableFaultWave() )
						{
							MakeLog(pPacketBuffer, nPacketLength, false);

							if ( ((pPacketBuffer[7]&0x7F) > 0)&&((pPacketBuffer[7]&0x7F) < 9) )
							{
								//Add by hxw in 2008.11.27. 录波获取时，强制记录日志及报文
								//if ( m_msFaultLogLifeCycle > 0 )
								//{
								//	CIOChannelBase* p103Channel = pDrv->GetChannelObj();
								//	//del 2011-1018
								//	//p103Channel->SetMonitor( false );							
								//	//p103Channel->SetLog(true);
								//	m_pFaultLogTimer->SetTimer( m_msFaultLogLifeCycle );
								//}


								//如果被记录的扰动表不空（也就是发生了故障），会进入这里，
								//程序会将所有故障记录到一个vector中，并将该设备的“故障选择标志”（pDevice->m_bFaultSel）置为true。
								pDevice->m_bIsRcvingFaultWave = true;
								pDevice->m_pFaultTimeOutTimer->KillTimer();
								pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

								pDevice->m_byFaultFUN = pPacketBuffer[10];
								//del zyg EnterCriticalSection( &pDevice->m_FaultWaveSection );
								pDevice->m_FaultDataVec.clear();
								//del zyg LeaveCriticalSection( &pDevice->m_FaultWaveSection );
								pDevice->m_byFaultNum = (pPacketBuffer[7]&0x7F);

								snprintf(chMsg,sizeof(chMsg)
									,"设备(地址%d)的扰动表不为空(功能类型-%d,故障个数-%d)!"
									, pDevice->GetAddr(),pDevice->m_byFaultFUN, pDevice->m_byFaultNum );
								pDrv->WriteLog(chMsg);

								//记录故障序号和故障状态
								//del zyg EnterCriticalSection( &pDevice->m_FaultWaveSection );
								for(OpcUa_Int32 i = 0; i < pDevice->m_byFaultNum; i++)
								{
									FaultDataSet faultData;
									faultData.FAN = MAKEWORD(pPacketBuffer[12+10*i], pPacketBuffer[12+10*i+1]) ;
									faultData.SOF = pPacketBuffer[12+10*i+2];
									memcpy(&(faultData.TimeSet), &(pPacketBuffer[12+10*i+3]), 7);

									pDevice->m_FaultDataVec.push_back(faultData);
								}
								//del zyg LeaveCriticalSection( &pDevice->m_FaultWaveSection );

								pDevice->m_bFaultSel = true;	//等m_FaultDataVec.push_back完毕后再置标志,以保证线程安全.
								pDevice->m_byRcvFaultNum = 0;

								/*	if ( !(SendNoteMessage(pDevice)) ) 
								TRACE("向文件服务共享内存发送通知消息失败！");

								if( !(SendFaultFrame( pDevice, pPacketBuffer, nPacketLength )) )
								TRACE("向文件服务共享内存添加报文失败！");*/
								pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);
							}
							else if ((pPacketBuffer[7]&0x7F) == 0)			//扰动表为空
							{						
								snprintf(chMsg,sizeof(chMsg)
									,"设备(地址%d)的扰动表为空!", pDevice->GetAddr() );
								pDrv->WriteLog(chMsg);
								pDevice->m_bIsRcvingFaultWave = false;
								pDevice->m_pFaultTimeOutTimer->KillTimer();
								pDevice->m_byRcvFaultNum = 0;
							}					
						}
						else
						{
							snprintf(chMsg,sizeof(chMsg)
								,"收到设备(地址%d)的ASDU23，但驱动为设备不需要录波，不进入录波获取流程!", pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
						}
					}
					break;
				case TYP_DISORDER_DATA_READY:					//扰动数据传输准备就绪		ASDU26				
					MakeLog(pPacketBuffer, nPacketLength, false);
					
					if ( pDevice->m_wFAN == MAKEWORD(pPacketBuffer[14], pPacketBuffer[15]) )
					{
						pDevice->m_bIsRcvingFaultWave = true;
						pDevice->m_pFaultTimeOutTimer->KillTimer();
						pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

						pDevice->m_byNOC = pPacketBuffer[18];
						pDevice->m_NOE = MAKEWORD(pPacketBuffer[19], pPacketBuffer[20]);
						//信息元素间隔
						pDevice->m_sINT = MAKEWORD(pPacketBuffer[21], pPacketBuffer[22]);
						//第一个信息元素的时标
						TIMESTAMP_4BYTE timeSet;
						memcpy(&timeSet, &pPacketBuffer[23], 4);
						//可以发“扰动数据请求报文”了
						pDevice->m_bReqDisData = true;		

						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)的扰动数据传输准备就绪(故障序号-%d,通道数目-%d, 一个通道信息元素的数目-%d, 采样间隔-%d微秒)!"
							, pDevice->GetAddr(),pDevice->m_wFAN, pDevice->m_byNOC, pDevice->m_NOE, pDevice->m_sINT );
						pDrv->WriteLog(chMsg);
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);
					}
					break;
				case TYP_CHANNEL_READY:							//被记录的第X个通道传输准备就绪  ASDU27
					MakeLog(pPacketBuffer, nPacketLength, false);

					if ( pDevice->m_wFAN == MAKEWORD(pPacketBuffer[14], pPacketBuffer[15]) )
					{
						pDevice->m_bIsRcvingFaultWave = true;
						pDevice->m_pFaultTimeOutTimer->KillTimer();
						pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

						pDevice->m_bChannelReady = true;
						pDevice->m_byACC = pPacketBuffer[16];
						/*17---20代表一次额定值*/	/*21---24代表二次额定值*/  /*25---28代表参比因子*/						
						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)的被记录的第X个通道传输准备就绪(故障序号-%d,通道序号-%d)!"
							, pDevice->GetAddr(),pDevice->m_wFAN, pDevice->m_byACC );
						pDrv->WriteLog(chMsg);
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);
					}
					break;
				case TYP_STATUS_ALTER_READY:					//带标志的状态变位传输准备就绪		ASDU28
					MakeLog(pPacketBuffer, nPacketLength, false);

					if ( pDevice->m_wFAN == MAKEWORD(pPacketBuffer[14], pPacketBuffer[15]) )
					{					
						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)的带标志的状态变位传输准备就绪(故障序号-%d)!"
							, pDevice->GetAddr(),pDevice->m_wFAN );
						pDrv->WriteLog(chMsg);

						pDevice->m_bIsRcvingFaultWave = true;
						pDevice->m_pFaultTimeOutTimer->KillTimer();
						pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

						//可以发“请求带标志的状态变位传输”报文了
						pDevice->m_bReqStatusAlter = true;

						/*if( !(SendFaultFrame( pDevice, pPacketBuffer, nPacketLength )) )
						TRACE("向文件服务共享内存添加报文失败！");*/
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);
					}
					break;
				case TYP_STATUS_ALTER_TRANS:					//传送带标志的状态变位的初始状态或状态变位	ASDU29
					MakeLog(pPacketBuffer, nPacketLength, false);
					
					if ( pDevice->m_wFAN == MAKEWORD(pPacketBuffer[12], pPacketBuffer[13]) )
					{
						pDevice->m_bIsRcvingFaultWave = true;
						pDevice->m_pFaultTimeOutTimer->KillTimer();
						pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

						//记录带标志的状态变位的数目
						pDevice->m_byNOT = pPacketBuffer[14];

						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)传送带标志的状态变位的初始状态或状态变位(故障序号-%d, 带标志的状态变位的数目-%d)!"
							, pDevice->GetAddr(),pDevice->m_wFAN,pDevice->m_byNOT );
						pDrv->WriteLog(chMsg);

						if (0 == MAKEWORD(pPacketBuffer[15], pPacketBuffer[16]))	//初始状态(TAP == 0)
						{
							pDevice->m_statusMap.clear();
							for (OpcUa_Int32 i = 0; i < pDevice->m_byNOT; i++)
							{
								bool bDPI = false;
								if((pPacketBuffer[17+3*i+2]&0x03) == 2)
									bDPI = true;
								else if((pPacketBuffer[17+3*i+2]&0x03) == 1)
									bDPI = false;
								/*else
								TRACE("该点DPI值异常！");*/
								//把初始状态记下来
								pDevice->m_statusMap.insert(StatusMap::value_type(
									1000*pPacketBuffer[17+3*i]+pPacketBuffer[17+3*i+1], bDPI ));
							}
						}
						else														//状态变位(关于TAP还不清楚)
						{
							for (OpcUa_Int32 i = 0; i < pDevice->m_byNOT; i++)
							{
								bool bDPI = false;
								if((pPacketBuffer[17+3*i+2]&0x03) == 2)
									bDPI = true;
								else if((pPacketBuffer[17+3*i+2]&0x03) == 1)
									bDPI = false;
								/*else
								TRACE("该点DPI值异常！");*/

								StatusMapiterator it;

								it = pDevice->m_statusMap.find( 1000*pPacketBuffer[17+3*i]+pPacketBuffer[17+3*i+1] );
								if ( it == pDevice->m_statusMap.end() )
								{
									//错误处理
									break;
								}
								else
								{
									if (it->second != bDPI)		//正常情况下这两个值不同
									{
										//
									}
									else
									{
										//错误处理
									}
								}
							}
						}
						/*if( !(SendFaultFrame( pDevice, pPacketBuffer, nPacketLength )) )
						TRACE("向文件服务共享内存添加报文失败！");*/
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);
					}
					break;
				case TYP_DISORDER_TRANS:		//ASDU30
					MakeLog(pPacketBuffer, nPacketLength, false);
					
					if ( (pDevice->m_wFAN == MAKEWORD(pPacketBuffer[14], pPacketBuffer[15])) &&
						pDevice->m_byACC == pPacketBuffer[16])
					{
						pDevice->m_bIsRcvingFaultWave = true;
						pDevice->m_pFaultTimeOutTimer->KillTimer();
						pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

						pDevice->m_bChannelEnd = true;
						pDevice->m_sNDV = pPacketBuffer[17];
						pDevice->m_sNFE = MAKEWORD(pPacketBuffer[18], pPacketBuffer[19]);

						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)传送扰动值(故障序号-%d,通道序号-%d,每个ASDU有关扰动值的数目-%d, 传输扰动值报文中第一个信息元素的序号-%d )!"
							, pDevice->GetAddr(),pDevice->m_wFAN,pDevice->m_byACC,pDevice->m_sNDV,pDevice->m_sNFE );
						pDrv->WriteLog(chMsg);

						for (OpcUa_Int32 i = 0; i < pDevice->m_sNDV; i += 2)
						{
							//解析每个扰动值   从pPacketBuffer[20], pPacketBuffer[21]开始
							//MAKEWORD(pPacketBuffer[20+i], pPacketBuffer[21+i]);
						}
						/*if( !(SendFaultFrame( pDevice, pPacketBuffer, nPacketLength )) )
						TRACE("向文件服务共享内存添加报文失败！");*/
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);

					}
					break;
				case TYP_TRANS_END:		//ASDU31
					MakeLog(pPacketBuffer, nPacketLength, false);
					
					if ( pDevice->m_wFAN == MAKEWORD(pPacketBuffer[14], pPacketBuffer[15]) )
					{
						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)传送结束[%d]..."
							, pDevice->GetAddr(), pPacketBuffer[12] );
						pDrv->WriteLog(chMsg);
						
						//可以发“扰动数据认可”报文了
						pDevice->m_bSendDisDataAck = true;
						switch (pPacketBuffer[12])
						{
						case TOO_ALTER_END_WITHOUTPAUSE:			//带标志的状态变位传输结束							
							snprintf(chMsg,sizeof(chMsg)
								,"设备(地址%d)带标志的状态变位传输结束(DI Good)!"
								, pDevice->GetAddr()  );
							pDrv->WriteLog(chMsg);

							pDevice->m_byDisDataAckTOO = TOO_STATUSCHANGED_SUCCESS;			//肯定
							/*什么时候否定？*/

							break;
						case TOO_CHANEL_END_WITHOUTPAUSE:							
							snprintf(chMsg,sizeof(chMsg)
								,"设备(地址%d)不带中止的通道传输的结束(AI Good)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							pDevice->m_byDisDataAckTOO = TOO_CHANEL_SUCCESS;			//肯定
							/*什么时候否定？*/

							break;
						case TOO_DISDATA_END_WITHOUTPAUSE:							
							snprintf(chMsg,sizeof(chMsg)
								,"设备(地址%d)不带中止的扰动数据传输的结束(Wave Finished)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);

							pDevice->m_byDisDataAckTOO = TOO_DISDATA_SUCCESS;			//肯定
							/*什么时候否定？*/

							pDevice->m_byRcvFaultNum++;
							if ( pDevice->m_byRcvFaultNum == pDevice->m_byFaultNum )	//设备所有的故障录播都接收完了.
							{							
								pDevice->m_byRcvFaultNum = 0;
							}
							break;
						case 39:							
							snprintf(chMsg,sizeof(chMsg)
								,"设备(地址%d)由控制系统所中止的带标志的状态变位传输的结束(Bad)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							break;
						case 40:							
							snprintf(chMsg,sizeof(chMsg)
								,"设备(地址%d)由设备所中止的带标志的状态变位传输的结束(Bad)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							break;
						case 33:							
							snprintf(chMsg,sizeof(chMsg)
								,"设备(地址%d)由控制系统所中止的扰动数据传输的结束(Bad)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							break;
						case 34:							
							snprintf(chMsg,sizeof(chMsg)
								, "设备(地址%d)由设备所中止的扰动数据传输的结束(Bad)!", pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							break;
						case 36:							
							snprintf(chMsg,sizeof(chMsg)
								, "设备(地址%d)由控制系统所中止的通道传输的结束(Bad)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							break;
						case 37:							
							snprintf(chMsg,sizeof(chMsg)
								, "设备(地址%d)由设备所中止的通道传输的结束(Bad)!"
								, pDevice->GetAddr());
							pDrv->WriteLog(chMsg);
							break;

						default:
							break;
						}
						//zyg  if( !(SendFaultFrame( pDevice, pPacketBuffer, nPacketLength )) )
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);

					}
					break;
				case 40:	//ASDU40: 总召唤时单点状态量全遥信响应
					if ( bInfoCompressed )
					{
						SINGLE_POINT *pPoint = (SINGLE_POINT *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( SINGLE_POINT ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->IV == 0)//valid
							{
								//找到标签
								iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
								if (-1 == iTagIndex)
								{								
									snprintf(chMsg,sizeof(chMsg)
										, "ASDU40: DI点[功能类型%d,信息序号%d,值%d]未配置!"
										, bFunType, InfoNo, pPoint->SPI);
									pDrv->WriteLog(chMsg);
								}

								if (-1 != iTagIndex)
								{
									pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = pPoint->SPI), sizeof(OpcUa_Byte),0,bFromLocal );
									if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
										&&pPoint->SPI)
									{										
// 										CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 											pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)pPoint->SPI, 0);		//! 发送SOE
									}
								}

								OpcUa_Byte bDIValue = pPoint->SPI;
								pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo,  &bDIValue, sizeof(OpcUa_Byte), 0 , bFromLocal );
							}
							pPoint++;
							InfoNo++;
						}
					}
					else
					{
						SINGLE_POINT_INFO *pPoint = (SINGLE_POINT_INFO *)((OpcUa_Byte*)(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( SINGLE_POINT_INFO ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->IV == 0)//valid
							{
								iTagIndex = pDevice->GetTagIndex(bFunType, pPoint->Info);
								if (-1 == iTagIndex)
								{									
									snprintf(chMsg,sizeof(chMsg)
										, "ASDU40: DI点[功能类型%d,信息序号%d,值%d]未配置!"
										, bFunType, pPoint->Info, pPoint->SPI);
									pDrv->WriteLog(chMsg);
								}

								if (-1 != iTagIndex)
								{
									pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = pPoint->SPI),sizeof(OpcUa_Byte),0, bFromLocal );
									if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
										&&pPoint->SPI)
									{										
// 										CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 											pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)pPoint->SPI, 0);		//! 发送SOE
									}
								}

								OpcUa_Byte bDIValue = pPoint->SPI;
								pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+pPoint->Info, &bDIValue, sizeof(OpcUa_Byte), 0 , bFromLocal );
							}
							pPoint++;

						}
					}
					break;
				case 41:	//ASDU41: 上送SOE
					if ( bInfoCompressed )
					{
						SIQ_TIME *pPoint = (SIQ_TIME *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( SIQ_TIME ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->SIQ.IV == 0)//valid
							{
								//找到标签
								iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
								if (-1 == iTagIndex)
								{									
									snprintf(chMsg,sizeof(chMsg)
										,"ASDU41: DI点[功能类型%d,信息序号%d,值%d]未配置!"
										, bFunType, InfoNo, pPoint->SIQ.SPI);
									pDrv->WriteLog(chMsg);
								}

	
								if (-1 != iTagIndex)
								{
									pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = pPoint->SIQ.SPI), sizeof(OpcUa_Byte),0, bFromLocal );
									if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
										&&pPoint->SIQ.SPI)
									{											
										if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
										{											
											WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
											OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;
// 											CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 												pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)pPoint->SIQ.SPI, nTime);		//! 发送SOE
										}
									}
								}

								if ( pPoint->Time.IV == 1 )			//时间无效
								{
									pDevice->m_bNeedCheckTime = true;												
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI变位 报文中IV=1, ASDU%d, 提示时间无效, 需要对时!", pDevice->GetAddr(), 1);
									pDrv->WriteLog(chMsg);
								}
								else if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
									pDrv->WriteLog(chMsg);

									WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
									OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;

									OpcUa_Byte bDIValue = pPoint->SIQ.SPI;
									pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo, &bDIValue, sizeof(OpcUa_Byte), nTime , bFromLocal );
								}
								else
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI,ASDU%d, 报文中时间有效, 但为夏时制!",pDevice->GetAddr(), 41 );
									pDrv->WriteLog(chMsg);
								}										
							}
							pPoint++;
							InfoNo++;
						}
					}
					else
					{
						SIQ_TIME_INFO *pPoint = (SIQ_TIME_INFO *)((OpcUa_Byte*)(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( SIQ_TIME_INFO ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->SIQ.IV == 0)//valid
							{
								//找到标签
								iTagIndex = pDevice->GetTagIndex(bFunType, pPoint->Info);
								if (-1 == iTagIndex)
								{									
									snprintf(chMsg,sizeof(chMsg)
										,"ASDU41: DI点[功能类型%d,信息序号%d,值%d]未配置!"
										, bFunType, pPoint->Info, pPoint->SIQ.SPI);
									pDrv->WriteLog(chMsg);
								}

								if (-1 != iTagIndex)
								{
									pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = pPoint->SIQ.SPI), sizeof(OpcUa_Byte), 0, bFromLocal );
									if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
										&&pPoint->SIQ.SPI)
									{
										if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
										{											
											WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
											OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;
// 											CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 												pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)pPoint->SIQ.SPI, nTime);		//! 发送SOE
										}
									}
								}

								if ( pPoint->Time.IV == 1 )			//时间无效
								{
									pDevice->m_bNeedCheckTime = true;												
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI变位 报文中IV=1, ASDU%d, 提示时间无效, 需要对时!", pDevice->GetAddr(), 41);
									pDrv->WriteLog(chMsg);
								}
								else if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 41);
									pDrv->WriteLog(chMsg);

									WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
									OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;

									OpcUa_Byte bDIValue = pPoint->SIQ.SPI;
									pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+pPoint->Info, &bDIValue, sizeof(OpcUa_Byte), nTime , bFromLocal );

								}
								else
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"设备(地址%d)DI,ASDU%d, 报文中时间有效, 但为夏时制!",pDevice->GetAddr(), 41 );
									pDrv->WriteLog(chMsg);
								}				
							}

							pPoint++;
						}
					}
					break;
				case 42:	//ASDU40: 总召唤时双点状态量全遥信响应
					if ( bInfoCompressed )
					{
						DOUBLE_POINT *pPoint = (DOUBLE_POINT *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( DOUBLE_POINT ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->IV == 0 )//valid
							{
								if ( pPoint->DPI == 1 || pPoint->DPI == 2 )
								{
									OpcUa_Byte Val = pPoint->DPI-1;

									//找到标签
									iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
									if (-1 == iTagIndex)
									{										
										snprintf(chMsg,sizeof(chMsg)
											,"ASDU42: DI点[功能类型%d,信息序号%d,值%d]未配置!"
											, bFunType, InfoNo, Val);
										pDrv->WriteLog(chMsg);
									}

									if (-1 != iTagIndex)
									{
										pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val),sizeof(OpcUa_Byte),0, bFromLocal );
										if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
											|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
											&&Val)
										{											
// 											CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 												pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)Val, 0);		//! 发送SOE
										}
									}

									pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo, &Val, sizeof(OpcUa_Byte), 0 , bFromLocal );
									OpcUa_Float fValue = (OpcUa_Float)pPoint->DPI;
									pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo, (OpcUa_Byte*)&fValue, sizeof(fValue), 0 , bFromLocal );
								}
							}
							pPoint++;
							InfoNo++;
						}
					}
					else
					{
						DOUBLE_POINT_INFO *pPoint = (DOUBLE_POINT_INFO *)((OpcUa_Byte*)(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( DOUBLE_POINT_INFO ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if( pPoint->IV == 0 )//valid
							{
								if ( pPoint->DPI == 1 || pPoint->DPI == 2 )
								{
									OpcUa_Byte Val = pPoint->DPI - 1;

									//找到标签
									iTagIndex = pDevice->GetTagIndex(bFunType, pPoint->Info);
									if (-1 == iTagIndex)
									{										
										snprintf(chMsg,sizeof(chMsg)
											,"ASDU42: DI点[功能类型%d,信息序号%d,值%d]未配置!"
											, bFunType, pPoint->Info, Val);
										pDrv->WriteLog(chMsg);
									}

									if (-1 != iTagIndex)
									{
										pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val),sizeof(OpcUa_Byte),0, bFromLocal );
									}

									pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+pPoint->Info, &Val, sizeof(OpcUa_Byte), 0 , bFromLocal );
									OpcUa_Float fValue = (OpcUa_Float)pPoint->DPI;
									pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+pPoint->Info, (OpcUa_Byte*)&fValue, sizeof(fValue), 0 , bFromLocal );
								}
							}
							pPoint++;
						}
					}
					break;
				case 43:	//ASDU43: 上送双点SOE
					if ( bInfoCompressed )
					{
						DIQ_TIME *pPoint = (DIQ_TIME *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( DIQ_TIME ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->DIQ.IV == 0)//valid
							{
								if ( pPoint->DIQ.DPI == 1 || pPoint->DIQ.DPI == 2 )
								{
									OpcUa_Byte Val = pPoint->DIQ.DPI - 1;

									//找到标签
									iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
									if (-1 == iTagIndex)
									{										
										snprintf(chMsg,sizeof(chMsg)
											,"ASDU43: DI点[功能类型%d,信息序号%d,值%d]未配置!"
											, bFunType, InfoNo, Val);
										pDrv->WriteLog(chMsg);
									}

									if (-1 != iTagIndex)
									{
										pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val), sizeof(OpcUa_Byte),0, bFromLocal );
										if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
											|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
											&&Val)
										{									
											if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
											{											
												WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
												OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;
// 												CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 													pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , Val, nTime);		//! 发送SOE
											}
										}
									}

									if ( pPoint->Time.IV == 1 )			//时间无效
									{
										pDevice->m_bNeedCheckTime = true;												
										snprintf(chMsg,sizeof(chMsg)
											,"设备(地址%d)DI变位 报文中IV=1, ASDU%d, 提示时间无效, 需要对时!", pDevice->GetAddr(), 43);
										pDrv->WriteLog(chMsg);
									}
									else if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
									{											
										snprintf(chMsg,sizeof(chMsg)
											,"设备(地址%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 43);
										pDrv->WriteLog(chMsg);

										WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
										OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;

										pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo, &Val, sizeof(OpcUa_Byte), nTime , bFromLocal );
										OpcUa_Float fValue = (OpcUa_Float)pPoint->DIQ.DPI;
										pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo, (OpcUa_Byte*)&fValue, sizeof(fValue), nTime , bFromLocal );
									}
									else
									{											
										snprintf(chMsg,sizeof(chMsg)
											,"设备(地址%d)DI,ASDU%d, 报文中时间有效, 但为夏时制!",pDevice->GetAddr(), 43 );
										pDrv->WriteLog(chMsg);
									}				
								}
							}
							pPoint++;
							InfoNo++;
						}
					}
					else
					{
						DIQ_TIME_INFO *pPoint = (DIQ_TIME_INFO *)((OpcUa_Byte*)(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( DIQ_TIME_INFO ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->DIQ.IV == 0)//valid
							{
								if ( pPoint->DIQ.DPI == 1 || pPoint->DIQ.DPI == 2 )
								{
									OpcUa_Byte Val = pPoint->DIQ.DPI - 1;

									//找到标签
									iTagIndex = pDevice->GetTagIndex(bFunType, pPoint->Info);
									if (-1 == iTagIndex)
									{										
										snprintf(chMsg,sizeof(chMsg)
											,"ASDU43: DI点[功能类型%d,信息序号%d,值%d]未配置!"
											, bFunType, pPoint->Info, Val);
										pDrv->WriteLog(chMsg);
									}

									if (-1 != iTagIndex)
									{
										pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val), sizeof(OpcUa_Byte),0, bFromLocal );
										if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
											|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
											&&Val)
										{
											if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
											{											
												WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
												OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;
// 												CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 													pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , Val, nTime);		//! 发送SOE
											}
										}
									}

									if ( pPoint->Time.IV == 1 )			//时间无效
									{
										pDevice->m_bNeedCheckTime = true;												
										snprintf(chMsg,sizeof(chMsg)
											,"设备(地址%d)DI变位 报文中IV=1, ASDU%d, 提示时间无效, 需要对时!", pDevice->GetAddr(), 43);
										pDrv->WriteLog(chMsg);
									}
									else if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
									{											
										snprintf(chMsg,sizeof(chMsg)
											,"设备(地址%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 43);
										pDrv->WriteLog(chMsg);

										WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
										OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;

										pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+pPoint->Info, &Val, sizeof(OpcUa_Byte), nTime , bFromLocal );
										OpcUa_Float fValue = (OpcUa_Float)pPoint->DIQ.DPI;
										pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+pPoint->Info, (OpcUa_Byte*)&fValue, sizeof(fValue), nTime , bFromLocal );
									}
									else
									{											
										snprintf(chMsg,sizeof(chMsg)
											,"设备(地址%d)DI,ASDU%d, 报文中时间有效, 但为夏时制!",pDevice->GetAddr(), 43 );
										pDrv->WriteLog(chMsg);
									}				
								}
							}
							pPoint++;
						}
					}
					break;
				case 44:	//ASDU44: 上送全遥信
					if ( bInfoCompressed )
					{
						SCD_QDS *pPoint = (SCD_QDS *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( SCD_QDS ) )
						{
							break;
						}

						OpcUa_Byte Val = 0;
						for(OpcUa_Byte p = 0; p < InfoSum; p++)	//每个信息元包括16个DI
						{
							if( pPoint->Quality.IV == 0 )//valid
							{
								for ( OpcUa_Int32 i = 0; i < 16; i++ )
								{
									if ( InfoNo < MAX_IEC103_INF )
									{
										Val = (OpcUa_Byte)((pPoint->DISet >> i)&0x01);

										//找到标签
										iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
										if (-1 == iTagIndex)
										{											
											snprintf(chMsg,sizeof(chMsg)
												,"ASDU44: DI点[功能类型%d,信息序号%d,值%d]未配置!"
												, bFunType, InfoNo, Val);
											pDrv->WriteLog(chMsg);
										}

										if (-1 != iTagIndex)
										{
											pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val),sizeof(OpcUa_Byte),0, bFromLocal );
											if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
												|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
												&&Val)
											{												
// 												CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 													pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)Val, 0);		//! 发送SOE
											}
										}

										pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo, &Val, sizeof(OpcUa_Byte), 0 , bFromLocal );
									}
									InfoNo++;
								}
							}
							pPoint++;
						}
					}
					else
					{
						SCD_QDS_INFO *pPoint = (SCD_QDS_INFO *)((OpcUa_Byte* )(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( SCD_QDS_INFO ) )
						{
							break;
						}

						OpcUa_Byte Val = 0;
						for(OpcUa_Byte p = 0; p < InfoSum; p++)	//每个信息元包括16个DI
						{
							if( pPoint->Quality.IV == 0 )//valid
							{
								for ( OpcUa_Int32 i = 0; i < 16; i++ )
								{
									if ( pPoint->Info < MAX_IEC103_INF )
									{
										Val = (OpcUa_Byte)((pPoint->DISet >> i)&0x01);

										//找到标签
										iTagIndex = pDevice->GetTagIndex(bFunType, pPoint->Info);
										if (-1 == iTagIndex)
										{										
											snprintf(chMsg,sizeof(chMsg)
												,"ASDU44: DI点[功能类型%d,信息序号%d,值%d]未配置!"
												, bFunType, pPoint->Info, Val);
											pDrv->WriteLog(chMsg);
										}

										if (-1 != iTagIndex)
										{
											pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val),sizeof(OpcUa_Byte),0, bFromLocal );
											if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
												|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
												&&Val)
											{												
// 												CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 													pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)Val, 0);		//! 发送SOE
											}
										}

										pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+pPoint->Info, &Val, sizeof(OpcUa_Byte), 0 , bFromLocal );
									}
								}
							}
							pPoint++;
						}
					}
					break;
				case 50:	//ASDU50: 遥测上传
				case 51:	//ASDU51: 遥测上传
					if ( bInfoCompressed )
					{
						MEA_AI *pPoint = (MEA_AI *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( MEA_AI ) )
						{
							break;
						}

						for(OpcUa_Int32 i = 0; i < InfoSum; i++)
						{
							if( pPoint->ER == 0 )//valid
							{
								OpcUa_Float temp = (pDevice->GetModulusByTag(i+1))*((OpcUa_Float)(pPoint->Value));
								pDevice->SetVal( MacsDIS103_AI, i+1, (OpcUa_Byte*)&temp,sizeof(temp),0, bFromLocal );

								temp = (OpcUa_Float)pPoint->Value;
								pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo, (OpcUa_Byte*)&temp, sizeof(temp), 0, bFromLocal );
							}
							InfoNo++;
							pPoint++;
						}
					}
					else
					{
						MEA_AI_INFO *pPoint = (MEA_AI_INFO *)((OpcUa_Byte* )(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 7) / sizeof( MEA_AI_INFO ) )
						{
							break;
						}

						for(OpcUa_Int32 i = 0; i < InfoSum; i++)
						{
							if( pPoint->ER == 0 )//valid
							{
								OpcUa_Float temp = (pDevice->GetModulusByTag(i+1))*((OpcUa_Float)(pPoint->Value));
								pDevice->SetVal( MacsDIS103_AI, i+1, (OpcUa_Byte*)&temp,sizeof(temp),0, bFromLocal );

								temp = (OpcUa_Float)pPoint->Value;
								pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+pPoint->Info, (OpcUa_Byte*)&temp, sizeof(temp), 0, bFromLocal );
							}
							pPoint++;
						}
					}
					break;
				case TYP_ASDU17:			//施耐德保护定值
					{
/*
ASDU140为我司线路差动保护P521 103的私有ASDU，在总召唤中不予上传，经过下发相应的报文方可上传。 
例如：发送: 68 08 08 68 73 01 8C 81 14 01 01 02 99 16      FUN:01  INF:02  读取地址为0201的当前定值 
( 
链路地址: 1      ASDU 140      COT: 20  一般命令 
应用服务数据单元公共地址: 1 
) 
接收: 10 20 01 21 16   装置回应确认帧 
发送: 10 7A 01 7B 16   请求一级数据 
接收: 10 20 01 21 16   装置回应确认帧 
发送: 10 5A 01 5B 16   请求一级数据 
接收: 68 0E 0E 68 08 01 11 81 45 01 01 02 C8 00 50 E2 B8 14 AA 16     地址为0201的定值为C8   (C8=200    装置内部设定为2.00) 
( 
链路地址: 1      ASDU 17      COT: 69   
应用服务数据单元公共地址: 1 
) 
0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 
68 0E 0E 68 08 01 11 81 45 01 01 02 C8 00 50 E2 B8 14 AA 16
*/
						OpcUa_Int32 iTagAddr = 0;	//标签地址 = Fun*1000+Inf
						bFunType = pPacketBuffer[10];
						InfoNo = pPacketBuffer[11];
						bAINum = (pPacketBuffer[7]&0x7F);

						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)带相对时间的有时标的被测值报文ASDU17(Fun:%d,Info:%d,信息个数%d)!"
							, pDevice->GetAddr(),bFunType,InfoNo,InfoSum);
						pDrv->WriteLog(chMsg);

						MEA_AI_ASDU17* pVal = (MEA_AI_ASDU17* )(pPacketBuffer+12);
						for ( OpcUa_Int32 i = 0; i < bAINum; i++ )
						{
							iTagAddr = GetTagAddr( bFunType, InfoNo );
							OpcUa_Float fVal = pVal->Value;
							pDevice->SetVal( MacsDIS103_AIB, iTagAddr, (OpcUa_Byte* )&fVal, sizeof(OpcUa_Float),0, bFromLocal );
							pVal++;
							InfoNo++;
						}
					}
					break;
				case TYP_ASDU171:			//施耐德保护有功电度及无功电度
					{
						/*
						///////////////////////////////
						有功电度：
						68 08 08 68 73 01 8C 81 00 01 41 08 CB 16     //ASDU140: 读施耐德保护定值组报文类型标识,COT = 0, AI65:08
						E5

						10 5A 01 5B 16
						E5

						10 7B 01 7C 16
						10 29 01 2A 2A 16

						10 5A 01 5B 16
						//有功电度：3.50 MWh = 20 20 20 20 20 20 20 33 2E 35 30 20 4D 57 68（ASCII码）
						//ASDU171: AI65:08 = 20 20 20 20 20 20 20 33 2E 35 30 20 4D 57 68 20 20 20 20 20 83 31 B2 0C
						68 20 20 68 08 01 AB 81 08 01 41 08 20 20 20 20 20 20 20 33 2E 35 30 20 4D 57 68 20 20 20 20 20 83 31 B2 0C 6B 16

						10 7A 01 7B 16
						E5


						/////////////////////////////////////////
						无功电度：
						68 08 08 68 73 01 8C 81 00 01 43 08 CD 16    //ASDU140: 读施耐德保护定值组报文类型标识,COT = 0, AI67:08
						E5  

						10 5A 01 5B 16  
						E5  

						10 7B 01 7C 16  
						10 29 01 2A 16  

						10 5A 01 5B 16  
						//ASDU171: AI67:08  = 0.29 Mvarh  = 20 20 20  20 20 20  20 30 2E 32 39 20 4D 76 61  72 68（ASCII码）
						68 20 20 68 08 01 AB 81 08 01 43 08 20 20 20 20 20 20 20 30 2E 32 39 20 4D 76 61 72 68 20 20 20 A7 0E B7 0C 28 16
						*/
						OpcUa_Int32 iTagAddr = 0;	//标签地址 = Fun*1000+Inf
						bFunType = pPacketBuffer[10];
						InfoNo = pPacketBuffer[11];
						bAINum = (pPacketBuffer[7]&0x7F);

						snprintf(chMsg,sizeof(chMsg)
							,"设备(地址%d)施耐德保护电度ASDU171(Fun:%d,Info:%d,信息个数%d)!"
							, pDevice->GetAddr(),bFunType,InfoNo,InfoSum);
						pDrv->WriteLog(chMsg);

						if ( nPacketLength >= 15 )
						{
							OpcUa_CharA pchTemp[255];
							ACE_OS::memset( pchTemp, 0, sizeof(pchTemp) );
							ACE_OS::memcpy( pchTemp, pPacketBuffer+12, nPacketLength-14 );

							iTagAddr = GetTagAddr( bFunType, InfoNo );
							OpcUa_Float fVal = (OpcUa_Float)::atof( pchTemp );
							pDevice->SetVal( MacsDIS103_AIB, iTagAddr, (OpcUa_Byte* )&fVal, sizeof(OpcUa_Float), 0, bFromLocal );
						}
					}

					break;
				case TYP_COMM_DATA:			//通用分类数据报文类型标识
					{
						OpcUa_Int32 iRestByteCount = nPacketLength - sizeof(FT1_2_HEAD) - sizeof(FT1_2_TAIL);	//剩余字节个数，在下面不断计算
						ASDU10_HEAD_STRUCT* pASDUHead = (ASDU10_HEAD_STRUCT* )(pPacketBuffer+sizeof(FT1_2_HEAD));
						OpcUa_Byte* pASDUStart = pPacketBuffer+sizeof(FT1_2_HEAD);
						OpcUa_Int32 iTagAddr = 0;	//标签地址 = GROUP*1000+ENTRY
						if ( pASDUHead->NGD > 0 )	//有数据
						{
							OpcUa_Int32 i=0;
							OpcUa_Int32 j,k;
							OpcUa_Byte byValue;
							OpcUa_Int32 iValue;
							OpcUa_Float fValue;
							OpcUa_Int32 iByteCount = sizeof(ASDU10_HEAD_STRUCT);	//最大报文长度，在下面不断累加计算得出。
							OpcUa_Int32 ioneGIDSize=0;
							OpcUa_Byte* pGDPos = pASDUStart + sizeof(ASDU10_HEAD_STRUCT) ;		//通用分类数据区的位置
							GDATAPointHead* pDGHead = (GDATAPointHead* )pGDPos;	//通用分类数据区Head

							//分析COT并把COT分为三类同时作出相关处理: 
							//1.系统消息----根据设备行为需要,执行校时,总召唤. 
							//2.肯定响应----把相应Group和Entry的DIC标签置为1
							//3.否定响应----把相应Group和Entry的DIC标签置为0
							//注意, Group和Entry所对应的真正响应值更新到各自DI和AI.
							if ( pASDUHead->NGD == 1 )
							{
								ASDU10NoDataSet* pASDU = (ASDU10NoDataSet* )pASDUStart;

								iTagAddr = GetTagAddr(pASDU->Gin.Group,pASDU->Gin.Entry );

								switch ( COT )
								{
								case COT_CU:						//复位通信单元
								case COT_START:						//启动/重新启动
								case COT_POWERON:					//电源合上
									{
										//激活设备总召唤
										ActiveWholeCall( pDevice );

										if ( m_bBroadCheckTime )
										{
											m_bIsTimeForBroadCheckTime = true;
										}
										else
											pDevice->m_bNeedCheckTime = true;
									}
									break;
								case COT_GENERAL_CMD_CONFIRM:		//命令的肯定认可
								case COT_COMM_WRITE_CMD_CONFIRM:	//通用分类写命令的肯定认可
								case COT_COMM_READ_CMD_VALID:		//对通用分类读命令有效数据响应
								case COT_COMM_WRITE_CONFIRM:		//通用分类写确认
									{
										byValue = 1;
										pDevice->SetVal( MacsDIS103_DICB, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(BYTE),0, bFromLocal );
									}
									break;
								case COT_GENERAL_CMD_DENY:			//命令的否定认可
								case COT_COMM_WRITE_CMD_DENY:		//通用分类写命令的否定认可
								case COT_COMM_READ_CMD_INVALID:		//对通用分类读命令无效数据响应
									{
										byValue = 0;
										pDevice->SetVal( MacsDIS103_DICB, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(BYTE),0, bFromLocal );
									}
									break;
								default:
									break;
								}
							}

							//遍历通用分类数据。注意，各个数据类型可能会不相同。
							for ( i = 0; i < pASDUHead->NGD; i++ )
							{
								if ( pDGHead->sGDD.DataType == GDD_DT0_NODATA )
								{

									if ( iRestByteCount >= sizeof(GDATAPointNoData) )
									{
										//处理当前数据点
										iTagAddr = GetTagAddr( pDGHead->sGIN.Group, pDGHead->sGIN.Entry );


										//移至下一数据点
										iRestByteCount -= sizeof(GDATAPointNoData);
										iByteCount += sizeof(GDATAPointNoData);
										pGDPos += sizeof(GDATAPointNoData);
										pDGHead = (GDATAPointHead* )pGDPos;
									}
									else
									{
										pDrv->WriteLog( "剩余长度不足!" );
										break;
									}
								}
								else 
								{
									ioneGIDSize = pDGHead->sGDD.DataSize*pDGHead->sGDD.Number;
									if ( iRestByteCount >= sizeof(GDATAPointHead)+ioneGIDSize )
									{
										//处理当前数据点
										if ( pDGHead->KOD == KOD1_ACTUALVALUE )	//GDD中包含数据时,只处理"实际数据")
										{
											OpcUa_Byte* pCur = pASDUStart + iByteCount + sizeof(GDATAPointHead);
											switch ( pDGHead->sGDD.DataType )
											{
											case GDD_DT1_OS8ASCII:	//注意：采用UI相同的值合成法，当每个数据的长度超过4字节时会失真
											case GDD_DT2_BS1:
											case GDD_DT3_UI:
											case GDD_DT4_I:
											case GDD_DT22_ResponseCode:
											case GDD_DT24_Index:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{//这里咱没有考虑无符号
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													//合成数值. 注意, 如果长度超过4字节,则值会失真.
													iValue = 0;
		/*
													for( k=pDGHead->sGDD.DataSize-1; k>=0; k-- )
													{

														iValue += OpcUa_Int32( pCur[k] * pow( 0x100, pDGHead->sGDD.DataSize-1-k ) );
													}
		*/
													for( k = 0; k < pDGHead->sGDD.DataSize; k++ )
													{

														iValue += OpcUa_Int32( pCur[k] * pow( 256.0, k ) );
													}

													fValue = (OpcUa_Float)iValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT5_UFLOAT:	//暂没有特别处理
											case GDD_DT6_FLOAT:
											case GDD_DT7_FLOAT:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													//合成数值
													fValue = *(OpcUa_Float* )pCur;	//验证是否值转换正确，否则考虑逆序转换
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT8_DOUBLE:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													//合成数值
													fValue = (OpcUa_Float)(*(double* )pCur);	
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT9_DPI:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													byValue = *pCur;	

													fValue = (OpcUa_Float)byValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													if ( byValue == 1 || byValue == 2 )
													{
														byValue -= 1; 
														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT10_SPI:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													byValue = *pCur;	
													if ( byValue == 0 || byValue == 1 )
													{
														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT11_DPI:	//这里不能忽略0和3.
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													byValue = *pCur;	
													if ( byValue <= 3 )
													{
														fValue = (OpcUa_Float)byValue;	
														pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT12_AITime:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													//合成数值
													MEA_AI* pData = (MEA_AI* )pCur;
													if ( pData->ER == 0 && pData->OV == 0 )
													{
														fValue = (OpcUa_Float )pData->Value;	
														pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT18_DPITime:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													CP48* pPoint = (CP48* )pCur;
													byValue = pPoint->DPI;	

													fValue = (OpcUa_Float)byValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													if ( byValue == 1 || byValue == 2 )
													{
														byValue -= 1; 

														//发送SOE
														if ( pPoint->Time.IV == 1 )			//时间无效
														{
															pDevice->m_bNeedCheckTime = true;												
															snprintf(chMsg,sizeof(chMsg)
																,"CP48:设备(地址%d)DI变位SOE 报文中IV=1, ASDU10,GDD_DataType=%d, 提示时间无效, 需要对时!", pDevice->GetAddr(), pDGHead->sGDD.DataType);
															pDrv->WriteLog(chMsg);
														}
														else if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP48:设备(地址%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
															pDrv->WriteLog(chMsg);

															CP32Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															//! 计算时间
															DAILYTIME strTime;
															strTime.lYear    = dtime.year();
															strTime.sMonth   = dtime.month();
															strTime.sDay     = dtime.day();
															strTime.sHour    = pTime->Hour;
															strTime.sMinute  = pTime->Minute;
															strTime.sSecond  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															strTime.sMSecond = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;

															OpcUa_Int64 nTime = CaculateMSNum( strTime );

// 															CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIG,
// 																iTagAddr, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)byValue, nTime);		//! 发送SOE。 hexuewen note：这里实际是有隐患的，可能导致年月日不一致。
														}
														else
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP48:设备(地址%d)DI变位SOE,ASDU10,GDD_DataType=%d, 报文中时间有效, 但为夏时制!",pDevice->GetAddr(), pDGHead->sGDD.DataType );
															pDrv->WriteLog(chMsg);
														}										

														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT19_DPITime:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													CP80* pPoint = (CP80* )pCur;
													byValue = pPoint->DPI;	

													fValue = (OpcUa_Float)byValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													if ( byValue == 1 || byValue == 2 )
													{
														byValue -= 1; 

														//发送SOE
														if ( pPoint->Time.IV == 1 )			//时间无效
														{
															pDevice->m_bNeedCheckTime = true;												
															snprintf(chMsg,sizeof(chMsg)
																,"CP80:设备(地址%d)DI变位SOE 报文中IV=1, ASDU10,GDD_DataType=%d, 提示时间无效, 需要对时!", pDevice->GetAddr(), pDGHead->sGDD.DataType);
															pDrv->WriteLog(chMsg);
														}
														else if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP80:设备(地址%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
															pDrv->WriteLog(chMsg);

		/*
															CP32Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															struct tm structTime;
															structTime.tm_year = dtime.year();
															structTime.tm_mon  = dtime.month();
															structTime.tm_mday = dtime.day();

															structTime.tm_hour = pTime->Hour;
															structTime.tm_min  = pTime->Minute;
															structTime.tm_sec  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															OpcUa_Int64 nTime   = (OpcUa_Int64)mktime(&structTime);
															nTime = nTime*1000 + MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;
		*/

															CP32Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															//! 计算时间
															DAILYTIME strTime;
															strTime.lYear    = dtime.year();
															strTime.sMonth   = dtime.month();
															strTime.sDay     = dtime.day();
															strTime.sHour    = pTime->Hour;
															strTime.sMinute  = pTime->Minute;
															strTime.sSecond  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															strTime.sMSecond = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;

															OpcUa_Int64 nTime = CaculateMSNum( strTime );

 															//CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIG,
 															//	iTagAddr, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)byValue, nTime);		//! 发送SOE。 hexuewen note：这里实际是有隐患的，可能导致年月日不一致。
														}
														else
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP80:设备(地址%d)DI变位SOE,ASDU10,GDD_DataType=%d, 报文中时间有效, 但为夏时制!",pDevice->GetAddr(), pDGHead->sGDD.DataType );
															pDrv->WriteLog(chMsg);
														}										

														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT203_CP72:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													CP72* pPoint = (CP72* )pCur;
													byValue = pPoint->DPI;

													fValue = (OpcUa_Float)byValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													if ( byValue == 1 || byValue == 2 )
													{
														byValue -= 1; 

														//发送SOE
														if ( pPoint->Time.IV == 1 )			//时间无效
														{
															pDevice->m_bNeedCheckTime = true;												
															snprintf(chMsg,sizeof(chMsg)
																,"CP72:设备(地址%d)DI变位SOE 报文中IV=1, ASDU10,GDD_DataType=%d, 提示时间无效, 需要对时!", pDevice->GetAddr(), pDGHead->sGDD.DataType);
															pDrv->WriteLog(chMsg);
														}
														else if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP72:设备(地址%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
															pDrv->WriteLog(chMsg);

		/*
															CP56Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															struct tm structTime;
															structTime.tm_year = dtime.year();
															structTime.tm_mon  = dtime.month();
															structTime.tm_mday = dtime.day();

															structTime.tm_year = pTime->Year + (dtime.year()/100)*100;
															structTime.tm_mon  = pTime->Mon;
															structTime.tm_mday = pTime->Day;
															structTime.tm_hour = pTime->Hour;
															structTime.tm_min  = pTime->Minute;
															structTime.tm_sec  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															OpcUa_Int64 nTime   = (OpcUa_Int64)mktime(&structTime);
															nTime = nTime*1000 + MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;
		*/

															CP56Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															//! 计算时间
															DAILYTIME strTime;
															strTime.lYear    = pTime->Year + (dtime.year()/100)*100;
															strTime.sMonth   = pTime->Mon;
															strTime.sDay     = pTime->Day;
															strTime.sHour    = pTime->Hour;
															strTime.sMinute  = pTime->Minute;
															strTime.sSecond  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															strTime.sMSecond = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;

															OpcUa_Int64 nTime = CaculateMSNum( strTime );

// 															CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIG,
// 																iTagAddr, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)byValue, nTime);		//! 发送SOE。 hexuewen note：这里实际是有隐患的，可能导致年月日不一致。
														}
														else
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP72:设备(地址%d)DI变位SOE,ASDU10,GDD_DataType=%d, 报文中时间有效, 但为夏时制!",pDevice->GetAddr(), pDGHead->sGDD.DataType );
															pDrv->WriteLog(chMsg);
														}										

														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT204_CP104:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													CP104* pPoint = (CP104* )pCur;
													byValue = pPoint->DPI;

													fValue = (OpcUa_Float)byValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													if ( byValue == 1 || byValue == 2 )
													{
														byValue -= 1; 

														//发送SOE
														if ( pPoint->Time.IV == 1 )			//时间无效
														{
															pDevice->m_bNeedCheckTime = true;												
															snprintf(chMsg,sizeof(chMsg)
																,"CP104:设备(地址%d)DI变位SOE 报文中IV=1, ASDU10,GDD_DataType=%d, 提示时间无效, 需要对时!", pDevice->GetAddr(), pDGHead->sGDD.DataType);
															pDrv->WriteLog(chMsg);
														}
														else if ( pPoint->Time.SU == 0 )	//时间有效, 且不为夏时制
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP104:设备(地址%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
															pDrv->WriteLog(chMsg);

		/*
															CP56Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															struct tm structTime;
															structTime.tm_year = dtime.year();
															structTime.tm_mon  = dtime.month();
															structTime.tm_mday = dtime.day();

															structTime.tm_year = pTime->Year + (dtime.year()/100)*100;
															structTime.tm_mon  = pTime->Mon;
															structTime.tm_mday = pTime->Day;
															structTime.tm_hour = pTime->Hour;
															structTime.tm_min  = pTime->Minute;
															structTime.tm_sec  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															OpcUa_Int64 nTime   = (OpcUa_Int64)mktime(&structTime);
															nTime = nTime*1000 + MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;
		*/

															CP56Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															//! 计算时间
															DAILYTIME strTime;
															strTime.lYear    = pTime->Year + (dtime.year()/100)*100;
															strTime.sMonth   = pTime->Mon;
															strTime.sDay     = pTime->Day;
															strTime.sHour    = pTime->Hour;
															strTime.sMinute  = pTime->Minute;
															strTime.sSecond  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															strTime.sMSecond = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;

															OpcUa_Int64 nTime = CaculateMSNum( strTime );

// 															CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIG,
// 																iTagAddr, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)byValue, nTime);		//! 发送SOE。 hexuewen note：这里实际是有隐患的，可能导致年月日不一致。
														}
														else
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP104:设备(地址%d)DI变位SOE,ASDU10,GDD_DataType=%d, 报文中时间有效, 但为夏时制!",pDevice->GetAddr(), pDGHead->sGDD.DataType );
															pDrv->WriteLog(chMsg);
														}										

														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											default:
												break;
											}
										}
										else
										{
											snprintf(chMsg,sizeof(chMsg)
												,"设备(地址%d)ASDU10,组号%d,条目号%d的KOD%d为非实际值(1) !.", pDevice->GetAddr(), pDGHead->sGIN.Group, pDGHead->sGIN.Entry, pDGHead->KOD);

											pDrv->WriteLog( chMsg );
										}

										//移至下一数据点
										iRestByteCount -= sizeof(GDATAPointHead)+ioneGIDSize;
										iByteCount += sizeof(GDATAPointHead)+ioneGIDSize;
										pGDPos += sizeof(GDATAPointHead)+ioneGIDSize;
										pDGHead = (GDATAPointHead* )pGDPos;
									}
									else
									{
										pDrv->WriteLog( "剩余长度不足!" );
										break;
									}
								}
							}
						}
						else	//无数据
						{
							if ( nPacketLength == sizeof(ASDU10NoDataSet) )
							{
								ASDU10NoDataSet* pASDU = (ASDU10NoDataSet* )pASDUStart;

								iTagAddr = GetTagAddr(pASDU->Gin.Group,pASDU->Gin.Entry );

								//当收到无数据的ASDU10应答时,把COT值写入组号和条目号所在COT标签.
								BYTE byValue = pASDUHead->COT;
								pDevice->SetVal( MacsDIS103_COT, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(BYTE),0, bFromLocal );

								//分析COT并把COT分为三类同时作出相关处理: 
								//1.系统消息----根据设备行为需要,执行校时,总召唤. 
								//2.肯定响应----把相应Group和Entry的DIC标签置为1
								//3.否定响应----把相应Group和Entry的DIC标签置为0
								//注意, Group和Entry所对应的真正响应值更新到各自DI和AI.
								switch ( COT )
								{
								case COT_CU:						//复位通信单元
								case COT_START:						//启动/重新启动
								case COT_POWERON:					//电源合上
									{
										//激活设备总召唤
										ActiveWholeCall( pDevice );

										if ( m_bBroadCheckTime )
										{
											m_bIsTimeForBroadCheckTime = true;
										}
										else
											pDevice->m_bNeedCheckTime = true;
									}
									break;
								case COT_GENERAL_CMD_CONFIRM:		//命令的肯定认可
								case COT_COMM_WRITE_CMD_CONFIRM:	//通用分类写命令的肯定认可
								case COT_COMM_READ_CMD_VALID:		//对通用分类读命令有效数据响应
								case COT_COMM_WRITE_CONFIRM:		//通用分类写确认
									{
										byValue = 1;
										pDevice->SetVal( MacsDIS103_DICB, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(BYTE),0, bFromLocal );
									}
									break;
								case COT_GENERAL_CMD_DENY:			//命令的否定认可
								case COT_COMM_WRITE_CMD_DENY:		//通用分类写命令的否定认可
								case COT_COMM_READ_CMD_INVALID:		//对通用分类读命令无效数据响应
									{
										byValue = 0;
										pDevice->SetVal( MacsDIS103_DICB, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(BYTE),0, bFromLocal );
									}
									break;
								default:
									break;
								}
							}
						}
					}
					break;
				case 205:			//脉冲量的采集
					//待实现
					break;
				default:					
					snprintf(chMsg,sizeof(chMsg)
						,"设备(地址%d)收到未知ASDU%d!"
						, pDevice->GetAddr(), pPacketBuffer[6]);
					pDrv->WriteLog( chMsg );
					
					break;
				}
			}
			break;
		default:
			break;
		}


		pDrv->WriteLog("IEC103  CFepIEC103Prtcl::ProcessRcvdPacket处理完数据帧，发送下一次请求");

		//! 发送延迟
		ACE_Time_Value tv( m_nSendDelay/1000, (m_nSendDelay%1000)*1000 );
		ACE_OS::sleep(tv);

		OpcUa_Int32 iReturnLen = 0;
		BuildRequest( m_pbySndBuffer, iReturnLen );

		//! 启动定时器
		if ( m_pTimeOutTimer->SetTimer( m_nRcvTimeOut ) == 0 )
		{
			snprintf( chMsg, sizeof(chMsg), 
				"CFepIEC103Prtcl::ProcessRcvdPacket Master  SetTimer Success,TimerID = %d....", m_pTimeOutTimer->GetTimerID() );
			pDrv->WriteLog(chMsg);
		}
		else
		{
			snprintf( chMsg, sizeof(chMsg), 
				"CFepIEC103Prtcl::ProcessRcvdPacket Master  SetTimer Failed,TimerID = %d!!!!!", m_pTimeOutTimer->GetTimerID() );
			pDrv->WriteLog(chMsg);
		}
	}


	/*!
	*	\brief	组下发命令帧	
	*
	*	\param	参数	OpcUa_Byte* pFrame: 帧数据区	 
	*
	*	\retval	返回值	OpcUa_Int32 : 帧长度
	*
	*	\note		注意事项（可选）
	*/
	OpcUa_Int32 CFepIEC103Prtcl::BuildCtlRequest( OpcUa_Byte* sndBuf ,OpcUa_Int32& iDataLen)
	{
		if ( !sndBuf )
			return 0;
		OpcUa_CharA chMsg[256];		

		CFepIEC103Drv* pDriver = (CFepIEC103Drv*)m_pParam;
		OpcUa_Int32 iResult = 0;
		OpcUa_Byte pchBuf[512];
		OpcUa_Int32 nBufLen = 512;		
		iResult = pDriver->m_IORemoteCtrl.DeQueue( (OpcUa_CharA*)pchBuf, nBufLen );
		if ( iResult == 0 )
		{
			std::string strLog;
			pDriver->WriteLogAnyWay("组下发命令帧... ...");
			
			memcpy( (OpcUa_Byte*)(&m_tCtrlRemoteCmd), pchBuf, sizeof(m_tCtrlRemoteCmd) );
			CFepIEC103Device* pDevice = dynamic_cast<CFepIEC103Device*>(pDriver->GetDeviceByName(m_tCtrlRemoteCmd.pchUnitName));
			assert(pDevice);
			OpcUa_Int32 nLength = 0;

			switch ( m_tCtrlRemoteCmd.byUnitType )
			{
			case MacsDIS103_DOS:	//遥控选择
				{
					if (pDevice->IsOnLine())		//设备可用，将对应的DOA置1
					{
						if (1 == m_tCtrlRemoteCmd.byData[0])			//收到DOM置1命令以后，先将上次的DOA和DOC清0
						{
							OpcUa_Byte byTemp = 0;
							pDevice->m_iInDOMIndex = m_tCtrlRemoteCmd.lTagAddress;

							pDevice->SetVal(MacsDIS103_DIS, m_tCtrlRemoteCmd.lTagAddress, &(pDevice->m_DOAs[pDevice->m_iInDOMIndex]  = 1),1,0, true);
							if (pDevice->TagNeedSOE(MacsDIS103_DO, m_tCtrlRemoteCmd.lTagAddress) 
								|| pDevice->TagNeedDevSOE(MacsDIS103_DI, m_tCtrlRemoteCmd.lTagAddress))
							{						
// 								CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIS,
// 									m_tCtrlRemoteCmd.lTagAddress, IOEVENTTYPE_OUTER_SOE , pDevice->m_DOAs[pDevice->m_iInDOMIndex], 0);		//! 发送SOE
							}						
							snprintf(chMsg,sizeof(chMsg)
								,"遥控选择命令DOS(设备名:%s,设备地址:%d),值(%d)有效!"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);
						}
						else
						{				
							snprintf(chMsg,sizeof(chMsg)
								,"遥控选择命令DOS(设备名:%s,设备地址:%d),标签地址:%d),值(%d)无效!"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);

							return 0;
						}					
					}
					else							//设备不可用
					{				
						snprintf(chMsg,sizeof(chMsg)
							,"遥控选择命令DOS无效(设备名:%s,设备地址:%d),设备离线!"
							,pDevice->GetName().c_str(),pDevice->GetAddr());
						pDriver->WriteLogAnyWay(chMsg);

						return 0;
					}
				}
				break;
			case MacsDIS103_DO:		//遥控执行
				{
					SOEMAPiterator it;
					it = pDevice->m_DOWithSel.find( m_tCtrlRemoteCmd.lTagAddress );
					if ( it == pDevice->m_DOWithSel.end())
						return 0;

					if (0 == it->second)
						pDevice->m_pDOCTimer->KillTimer();

					pDevice->m_iInDOIndex = m_tCtrlRemoteCmd.lTagAddress;	
					if ( it->second )
					{
						if (pDevice->m_DOAs[m_tCtrlRemoteCmd.lTagAddress]  == 1)
						{
							pDevice->SetVal(MacsDIS103_DIS, m_tCtrlRemoteCmd.lTagAddress, &(pDevice->m_DOAs[m_tCtrlRemoteCmd.lTagAddress]  = 0),1,0,true);
							nLength = pDevice->BuildDOFrame(sndBuf, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);					
							OpcUa_Byte bytemp = 1;
							pDevice->SetVal(MacsDIS103_DO,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);
							snprintf(chMsg,sizeof(chMsg)
								,"遥控执行命令DO(设备名:%s,设备地址:%d),标签地址:%d),值(%d)有效!"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);

							//计时30s，将遥控返信复位
							pDevice->m_pDOCTimer->SetTimer(m_nDORelayTime);
						}
						else
						{					
							snprintf(chMsg,sizeof(chMsg)
								,"遥控执行命令DO(设备名:%s,设备地址:%d,标签地址:%d),值(%d)无效!---请先执行遥控选择"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);
							
							return 0;
						}

					}			
					else
					{					
						iDataLen = pDevice->BuildDOFrame(sndBuf,m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						OpcUa_Byte bytemp = m_tCtrlRemoteCmd.byData[0];
						pDevice->SetVal(MacsDIS103_DO,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);
					}

					pDevice->m_bInDO = true;

					snprintf(chMsg,sizeof(chMsg)
						,"遥控命令DO: %s"
						,MakeLog(sndBuf, nLength));
					pDriver->WriteLogAnyWay( chMsg );		
				}
				break;
			case MacsDIS103_DIG://遥信--Group:Entry
				{
					OpcUa_Byte bytemp = m_tCtrlRemoteCmd.byData[0];
					pDevice->SetVal(MacsDIS103_DIG,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"遥信点DIG值被上层修改：(设备名:%s,设备地址:%d,标签地址:%d),值(%d)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_AIG://遥测--Group:Entry
				{
					OpcUa_Float bytemp = *((OpcUa_Float* )m_tCtrlRemoteCmd.byData);
					pDevice->SetVal(MacsDIS103_AIG,m_tCtrlRemoteCmd.lTagAddress,(OpcUa_Byte* )&bytemp,sizeof(OpcUa_Float),0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"遥测点AIG值被上层修改：(设备名:%s,设备地址:%d,标签地址:%d),值(%f)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,bytemp);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_DIB://遥信--Fun:Inf
				{
					OpcUa_Byte bytemp = m_tCtrlRemoteCmd.byData[0];
					pDevice->SetVal(MacsDIS103_DIB,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"遥信点DIB值被上层修改：(设备名:%s,设备地址:%d,标签地址:%d),值(%d)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_AIB://遥测--Fun:Inf
				{
					OpcUa_Float bytemp = *((OpcUa_Float* )m_tCtrlRemoteCmd.byData);
					pDevice->SetVal(MacsDIS103_AIB,m_tCtrlRemoteCmd.lTagAddress,(OpcUa_Byte* )&bytemp,sizeof(OpcUa_Float),0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"遥测点AIB值被上层修改：(设备名:%s,设备地址:%d,标签地址:%d),值(%f)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,bytemp);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_DICB://下发命令返校--Fun:In
				{
					OpcUa_Byte bytemp = m_tCtrlRemoteCmd.byData[0];
					pDevice->SetVal(MacsDIS103_DICB,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"遥控执行返校点DICB值被上层修改：(设备名:%s,设备地址:%d,标签地址:%d),值(%d)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_COT://无数据ASDU10应答的COT
				{
					OpcUa_Byte bytemp = m_tCtrlRemoteCmd.byData[0];
					pDevice->SetVal(MacsDIS103_COT,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"无数据ASDU10应答的COT值被上层修改：(设备名:%s,设备地址:%d,标签地址:%d),值(%d)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_DOB://遥控执行命令--Fun:Inf
				{
					if( 0 != m_tCtrlRemoteCmd.byData[0] && 1 != m_tCtrlRemoteCmd.byData[0] )
					{				
						snprintf(chMsg,sizeof(chMsg)
							,"遥控执行命令DOB(设备名:%s,设备地址:%d),标签类型:%d,标签地址:%d),值(%d)无效!"
							,pDevice->GetName().c_str(), pDevice->GetAddr(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						pDriver->WriteLogAnyWay(chMsg);

						return 0;
					}	

					OpcUa_Byte byValue =0;
					pDevice->SetVal(MacsDIS103_DICB, m_tCtrlRemoteCmd.lTagAddress,&byValue,1,0,true);

					iDataLen = pDevice->BuildDOBFrame(sndBuf, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);					
					if ( iDataLen > 0 )
					{
						snprintf(chMsg,sizeof(chMsg)
							,"生成遥控执行命令DOB(设备名:%s,设备地址:%d),标签类型:%d,标签地址:%d),值(%d): "
							,pDevice->GetName().c_str(), pDevice->GetAddr(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						pDriver->WriteLogAnyWay(chMsg);
						pDriver->WriteCommDataToLog( sndBuf, iDataLen,1 );
					}
				}
				break;
			case MacsDIS103_DOSB://遥控选择命令--Fun:Inf
				{
					if (pDevice->IsOnLine())		//设备可用，将对应的DOA置1
					{
						if (1 == m_tCtrlRemoteCmd.byData[0])			//收到遥控选择命令以后，直接置DICB为1，表示选择成功，不选择到设备
						{
							OpcUa_Byte byValue =1;
							pDevice->SetVal(MacsDIS103_DICB, m_tCtrlRemoteCmd.lTagAddress,&byValue,1,0,true);

							snprintf(chMsg,sizeof(chMsg)
								,"遥控选择命令DOSB(设备名:%s,设备地址:%d),值(%d)有效!"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);
						}
						else
						{				
							snprintf(chMsg,sizeof(chMsg)
								,"遥控选择命令DOSB(设备名:%s,设备地址:%d),标签地址:%d),值(%d)无效!"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);

							return 0;
						}					
					}
					else							//设备不可用
					{				
						snprintf(chMsg,sizeof(chMsg)
							,"遥控选择命令DOSB无效(设备名:%s,设备地址:%d),设备离线!"
							,pDevice->GetName().c_str(),pDevice->GetAddr());
						pDriver->WriteLogAnyWay(chMsg);

						return 0;
					}
				}
				break;
			case MacsDIS103_RdFV://读定值--Group:Entry: ASDU140
				{
					OpcUa_Byte byValue =0;
					pDevice->SetVal(MacsDIS103_DICB, m_tCtrlRemoteCmd.lTagAddress,&byValue,1,0,true);

					iDataLen = pDevice->BuildASDU140Frame(sndBuf, m_tCtrlRemoteCmd.lTagAddress );					
					if ( iDataLen > 0 )
					{
						snprintf(chMsg,sizeof(chMsg)
							,"生成读定值命令RdFV(设备名:%s,设备地址:%d),标签类型:%d,标签地址:%d),值(%d): "
							,pDevice->GetName().c_str(), pDevice->GetAddr(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						pDriver->WriteLogAnyWay(chMsg);
						pDriver->WriteCommDataToLog( sndBuf, iDataLen,1 );
					}
				}
				break;
			case MacsDIS103_WrFV://写定值--Group:Entry: ASDU232
				{
					OpcUa_Byte byValue =0;
					pDevice->SetVal(MacsDIS103_DICB, m_tCtrlRemoteCmd.lTagAddress,&byValue,1,0,true);

					iDataLen = pDevice->BuildASDU232Frame(sndBuf, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0] );					
					if ( iDataLen > 0 )
					{
						snprintf(chMsg,sizeof(chMsg)
							,"生成写定值命令WrFV(设备名:%s,设备地址:%d),标签类型:%d,标签地址:%d),值(%d): "
							,pDevice->GetName().c_str(), pDevice->GetAddr(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						pDriver->WriteLogAnyWay(chMsg);
						pDriver->WriteCommDataToLog( sndBuf, iDataLen,1 );
					}
				}
				break;
			case MacsDIS103_ASDU21RD://ASDU21读取--Group:Entry。注意，Entry=0表示读取整组
				{
					OpcUa_Byte byValue =0;
					pDevice->SetVal(MacsDIS103_DICB, m_tCtrlRemoteCmd.lTagAddress,&byValue,1,0,true);

					iDataLen = pDevice->BuildASDU21GroupFrame( sndBuf, m_tCtrlRemoteCmd.lTagAddress );					
					if ( iDataLen > 0 )
					{
						snprintf(chMsg,sizeof(chMsg)
							,"生成ASDU21读取命令ASDU21RD(设备名:%s,设备地址:%d),标签类型:%d,标签地址:%d),值(%d): "
							,pDevice->GetName().c_str(), pDevice->GetAddr(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						pDriver->WriteLogAnyWay(chMsg);
						pDriver->WriteCommDataToLog( sndBuf, iDataLen,1 );
					}
				}
				break;
			default:
				break;
			}
		}

		return iResult;
	}

	OpcUa_Int32 CFepIEC103Prtcl::CaculateBroadPeriod( void )
	{
		CP56Time2a	LocalTime;

		ACE_Time_Value NowTime;	
		ACE_Date_Time dateTime;			
		NowTime = ACE_OS::gettimeofday();
		dateTime.update( NowTime );
		memset(&LocalTime, 0, sizeof(CP56Time2a));			

		WORD MillSec = WORD(dateTime.second()) * 1000 + WORD(dateTime.microsec()/1000);

		LocalTime.LowMillSec	= LOBYTE(MillSec);
		LocalTime.HighMillSec	= HIBYTE(MillSec);
		LocalTime.Minute		= (OpcUa_Byte)dateTime.minute();
		LocalTime.Hour			= (OpcUa_Byte)dateTime.hour();
		LocalTime.Day			= (OpcUa_Byte)dateTime.day();
		LocalTime.Week			= (dateTime.weekday()==0)?7:(OpcUa_Byte)dateTime.weekday();
		LocalTime.Mon			= (OpcUa_Byte)dateTime.month();
		LocalTime.Year			= dateTime.year() % 100;

		return (59-LocalTime.Minute)*60 + (60-dateTime.second());
	}

	OpcUa_CharA* CFepIEC103Prtcl::MakeLog(OpcUa_Byte* pbyPacketBuffer, OpcUa_Int32 wMsgLen, bool bSend)
	{
		OpcUa_CharA chMsg[1024];
		if( pbyPacketBuffer )
		{
			CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(m_pParam);
			OpcUa_Int32 j = 0;
			snprintf( chMsg, sizeof(chMsg), "%d个字符有效:", wMsgLen );
			for ( OpcUa_Int32 i = 0; i < wMsgLen; i++ )
			{
				snprintf( chMsg, sizeof(chMsg), "%s %02X", chMsg, pbyPacketBuffer[i] );
			}

			pDrv->WriteLog(chMsg, true);
		}
		return "";
	}

	/*!
	*	\brief	 生成8位sum校验	
	*
	*	\param	参数	OpcUa_Byte * Buf: 要校验的数据包
	*	\param	参数	OpcUa_Int32 iByteLen: 包长度		
	*
	*	\retval	返回值	OpcUa_Byte: 8位校验和的值
	*
	*	\note		
	*/
	OpcUa_Byte CFepIEC103Prtcl::Get8SumCode( OpcUa_Byte * Buf, OpcUa_Int32 iByteLen )
	{
		if( !Buf || iByteLen == 0 )
			return 0;
		OpcUa_Byte bSum = 0;
		for(OpcUa_Int32 i=0; i<iByteLen; i++)
			bSum += Buf[i];
		return bSum;
	}

	/*!
	*	\brief	把时间转换成毫秒数
	*
	*	\param	strTime	要转换的时间	
	*
	*	\retval	OpcUa_Int64	毫秒数，为-1则表示转换失败
	*/
	OpcUa_Int64 CFepIEC103Prtcl::CaculateMSNum( DAILYTIME strTime )
	{
		if ( strTime.sMonth < 1 || strTime.sMonth > 12
			|| strTime.sDay < 1 || strTime.sDay > 31
			|| strTime.sHour < 0 || strTime.sHour > 24
			|| strTime.sMinute < 0 || strTime.sMinute > 60
			|| strTime.sSecond < 0 || strTime.sSecond > 60	)
		{
			return -1;
		}
		OpcUa_Int16 sMonthDay[12]={31,28,31,30,31,30,31,31,30,31,30,31};
		OpcUa_Int32 lSumDay;
		OpcUa_Int32 i;

		//! 计算总天数
		lSumDay = 
			(OpcUa_Int32)(strTime.lYear-1970) * 365 +(OpcUa_Int32)(strTime.lYear-1-1968) / 4;

		for( i = 0; i < strTime.sMonth - 1; i ++ )
			lSumDay += sMonthDay[ i ];

		if ( ( strTime.sMonth > 2 ) && ( ( strTime.lYear - 1968 ) % 4 == 0 ) )
			lSumDay += 1; 

		lSumDay += strTime.sDay -1 ;

		//! 得到相对于1970.1.1的总秒数
		OpcUa_Int64 nSec = (OpcUa_Int64)lSumDay * 24 * 60 * 60
			+ (OpcUa_Int64)strTime.sHour * 60 * 60
			+ (OpcUa_Int64)strTime.sMinute * 60
			+ (OpcUa_Int64)strTime.sSecond
			+ (OpcUa_Int64)strTime.sTimeZone * 60 * 60;

		//! 总毫秒数
		OpcUa_Int64 nTime = nSec * 1000 + strTime.sMSecond;

		return nTime;
	}

	bool CFepIEC103Prtcl::IsLinkOffline()
	{
		CFepIEC103Device* pDevice = NULL;
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(m_pParam);

		if ( pDrv->GetDeviceCount() <= 0 )
		{
			return false;
		}

		for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
			if (NULL != pDevice)
			{
				if ( !pDevice->IsOffLine() )
					return false;
			}
		}

		return true;
	}

	bool CFepIEC103Prtcl::IsLinkNotOnline()
	{
		CFepIEC103Device* pDevice = NULL;
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(m_pParam);

		if ( pDrv->GetDeviceCount() <= 0 )
		{
			return false;
		}

		for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
			if (NULL != pDevice)
			{
				if ( pDevice->IsOnLine() )
					return false;
			}
		}

		return true;
	}

	bool CFepIEC103Prtcl::IsLinkSuspicious()
	{
		CFepIEC103Device* pDevice = NULL;
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(m_pParam);

		//如果链路下没有设备，就不必重连了
		if ( pDrv->GetDeviceCount() <= 0 )
		{
			return false;
		}

		for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
			if (NULL != pDevice)
			{
				if ( !pDevice->IsSuspicious() )
					return false;
			}
		}

		return true;
	}
}