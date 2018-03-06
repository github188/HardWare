/*!
*	\file	FepIEC103Drv.cpp
*
*	\brief	103主站驱动类源文件
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Drv.cpp 1     11-01-21 10:02a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-01-21 10:02a $
*	$Revision: 1 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#include "FepIEC103Drv.h"
#include "FepIEC103Prtcl.h"
#include "FepIEC103Device.h"
#include "Common/CommonXmlParser/PRDCfgParser.h"

namespace MACS_SCADA_SUD
{
	void OnTmOut(void* p);
	std::string CFepIEC103Drv::s_ClassName("CFepIEC103Drv");	//! 类名

	std::string CFepIEC103Drv::GetClassName()
	{
		return s_ClassName;
	}

	CFepIEC103Drv::CFepIEC103Drv()
	{
		m_iConnTimes		= 0;
		m_iSerAddrNum		= 0;
		m_iCurSerAddrIndex	= 0;
		m_pConnTimer = NULL;
		m_bCanWork = false;
		m_bIsConnecting = false;
		m_pPrdCfgParser = NULL;
	}

	CFepIEC103Drv::~CFepIEC103Drv()
	{
		if (m_pConnTimer)
		{
			delete m_pConnTimer;
			m_pConnTimer = NULL;
		}
		if (m_pPrdCfgParser)
		{
			delete m_pPrdCfgParser;
			m_pPrdCfgParser = NULL;
		}
	}

	/*!
	*	\brief	得到继承PrtclParserBase基类的继承类名
	*
	*	\retval	返回值：继承类名
	*/
	std::string CFepIEC103Drv::GetPrtclClassName()
	{
		return "CFepIEC103Prtcl";
	}

	std::string CFepIEC103Drv::GetDeviceClassName()
	{
		return "CFepIEC103Device";
	}


	OpcUa_Int32 CFepIEC103Drv::OpenChannel()
	{
		//!通道对象检验，端口参数设置
		if ( 0 != CIODriver::OpenChannel() )
		{
			return -1;
		}
		m_pIODrvBoard = new CIODrvBoard();
		if ( m_pIODrvBoard == NULL )
		{
			return -2;
		}
		//!使驱动器IOSerialBoard开始工作
		std::string strPortType = m_pPortCfg.strType.toUtf8();
		m_pIODrvBoard->Init( m_pIOPrtclParser, strPortType, m_pPortCfg, this->GetTimeOut());	
		m_pIODrvBoard->open();	

		//!进行故障录播配置文件解析
		std::string strPRDCfgFile = CIODriver::GetFEPRunDir() + "PRDCfgFile.xml";
		m_pPrdCfgParser = new CPRDCfgParser();
		m_pPrdCfgParser->ParseMainInfo(strPRDCfgFile);
		return 0;
	}

	OpcUa_Int32 CFepIEC103Drv::StartWork(void)
	{			
		CFepIEC103Prtcl* pPrtcl = 
			dynamic_cast<CFepIEC103Prtcl*>(m_pIOPrtclParser);
		std::string strTemp = GetDriverName();
		pPrtcl->InitConfigPar((OpcUa_CharA*)strTemp.c_str());

		//!若物理链路为以太网
		if (1==pPrtcl->m_byPrtclType)
		{
			InitNetParam();

			pPrtcl->m_nMaxLinkOffline = m_iSerAddrNum*pPrtcl->m_nMaxOfflineCount*GetDeviceCount()*pPrtcl->m_nRcvTimeOut/1000;
			OpcUa_CharA pchMsg[128];
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Drv::StartWork: TCP链路最大离线时间（IP链路个数%d*单个设备最大离线次数%d*设备总数%d*超时时间%d(毫秒)）= %d 秒", 
				m_iSerAddrNum,pPrtcl->m_nMaxOfflineCount, GetDeviceCount(),pPrtcl->m_nRcvTimeOut, pPrtcl->m_nMaxLinkOffline );
			WriteLogAnyWay(pchMsg);
			Connect();
		}
		else
		{
			m_iSerAddrNum = 1;
			pPrtcl->m_nMaxLinkOffline = pPrtcl->m_nMaxOfflineCount*GetDeviceCount()*pPrtcl->m_nRcvTimeOut/1000;
			OpcUa_CharA pchMsg[128];
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Drv::StartWork: 串行链路最大离线时间（单个设备最大离线次数%d*设备总数%d*超时时间%d(毫秒)）= %d 秒", 
				 pPrtcl->m_nMaxOfflineCount, GetDeviceCount(),pPrtcl->m_nRcvTimeOut, pPrtcl->m_nMaxLinkOffline );
			WriteLogAnyWay(pchMsg);
			//! 直接发送第一帧数据
			OpcUa_Int32 iReturnLen = 0;
			pPrtcl->BuildRequest( pPrtcl->m_pbySndBuffer, iReturnLen );

			//! 启动定时器
			if ( pPrtcl->m_pTimeOutTimer->SetTimer( pPrtcl->m_nRcvTimeOut ) == 0 )
			{
				snprintf( pchMsg, sizeof(pchMsg), 
					"CFepIEC103Drv::StartWork Master SetTimer Success,TimerID = %d....", pPrtcl->m_pTimeOutTimer->GetTimerID() );
				WriteLog(pchMsg);
			}
			else
			{
				snprintf( pchMsg, sizeof(pchMsg), 
					"CFepIEC103Drv::StartWork Master SetTimer Failed,TimerID = %d!!!!!", pPrtcl->m_pTimeOutTimer->GetTimerID() );
				WriteLog(pchMsg);
			}
		}
		m_bCanWork = true;
		//! 启动周期定时器
		WriteLogAnyWay( "StartWork: to call m_pPeriodTimer->SetTimer" );
		pPrtcl->m_pPeriodTimer->SetTimer( PERIOD_SCAN_MS );
		return 0;
	}
	//! 初始化网络连接参数
	OpcUa_Int32 CFepIEC103Drv::InitNetParam()
	{
		m_pConnTimer = new CIOTimer(OnTmOut,this);
		m_iPort	= m_pPortCfg.nPort;		
		AddSerAddr(m_pPortCfg.strIPA_A.toUtf8());
		AddSerAddr(m_pPortCfg.strIPA_B.toUtf8());
		AddSerAddr(m_pPortCfg.strIPB_A.toUtf8());
		AddSerAddr(m_pPortCfg.strIPB_B.toUtf8());

		return 0;
	}

	//! 添加服务器
	void CFepIEC103Drv::AddSerAddr(const OpcUa_CharA* pchSerAddr)   
	{
		if(pchSerAddr && strcmp( pchSerAddr,"")!=0 )
		{
			if( m_iSerAddrNum <= 3 )
			{
				m_strSerAddrList[m_iSerAddrNum++] = pchSerAddr;


				OpcUa_CharA pchMsg[128];
				snprintf( pchMsg, sizeof(pchMsg), 
					"CFepIEC103Drv::Peer IP=%s", pchSerAddr );
				WriteLogAnyWay(pchMsg);
			}		
		}		
	}

	std::string CFepIEC103Drv::GetSerAddr( OpcUa_Int32 Num )
	{
		std::string strRet = "";
		if( Num < m_iSerAddrNum && Num >= 0 )
		{
			strRet =  m_strSerAddrList[Num];
		}	

		return strRet;
	}

	OpcUa_Int32 CFepIEC103Drv::GetSerAddrNum()
	{
		return m_iSerAddrNum;
	}

	OpcUa_Int32 CFepIEC103Drv::GetCurSerAddrIndex()
	{
		return m_iCurSerAddrIndex;
	}


	//! 封装一下连接操作，参数为服务器序号
	void CFepIEC103Drv::Connect(OpcUa_Int32 nIndex)
	{		
		if( nIndex >= 0 && nIndex < m_iSerAddrNum )			
		{
			OpcUa_CharA pchLog[256];
			sprintf( pchLog, "连接服务器%s, 序号为%d ...",
				m_strSerAddrList[nIndex].c_str(), nIndex );
			WriteLogAnyWay( pchLog );

			//重连前先清除收发计数器。否则，在线设备掉线后重连时，无论死否能连接成功，这些设备都永远在线。
			for ( OpcUa_Int32 i = 0; i < GetDeviceCount(); i++ )
			{
				CFepIEC103Device* pDevice = dynamic_cast<CFepIEC103Device*>(GetDeviceByIndex(i));
				if ( pDevice )
				{
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes = 0;
				}
			}

			//! 起连接定时器
			CFepIEC103Prtcl* pPrtcl = 
				dynamic_cast<CFepIEC103Prtcl*>(m_pIOPrtclParser);
			OpcUa_Int32 nR = m_pConnTimer->SetTimer( pPrtcl->m_iConnTimeOut );
			if ( nR < 0 )
			{
				//! 记录日志
				OpcUa_CharA chMsg[256];
				sprintf( chMsg, "ERROR：IEC103::ReConnect: SetTimer Error, nRet = %d!", nR );
				WriteLog(chMsg,true);
			}

			m_bIsConnecting = true;
			m_pIODrvBoard->Connect( m_strSerAddrList[nIndex].c_str(), m_iPort );
			m_iCurSerAddrIndex = nIndex;
		}
		return;
	}

	//! TCP连接定时器超时回调函数
	void OnTmOut(void* p)					
	{
		CFepIEC103Drv* pDriver = (CFepIEC103Drv*)p;
		//!关闭定时器
		pDriver->GetConnTimer()->KillTimer();

		//! 连接锁
		//ACE_Guard<ACE_Recursive_Thread_Mutex> guard( pDriver->m_Mutex );

		pDriver->m_bIsConnecting = false;

		OpcUa_CharA chMsg[128];
		sprintf( chMsg, "FepIEC103::OnTmOut: 连接网络%s超时!", pDriver->GetCurServerAddr().c_str() );
		//! 记录日志		
		pDriver->WriteLogAnyWay(chMsg,true);

		//! 如果连接上了，则清重连次数
		if(pDriver->m_pIODrvBoard->ConnState())
		{
			pDriver->SetConnTime( 0 );	
			sprintf( chMsg, "OnTmOut: 网络%s已连接成功，return!", pDriver->GetCurServerAddr().c_str() );
			//! 记录日志
			pDriver->WriteLogAnyWay( chMsg,true );
			return;
		}

		//! 置设备可疑
		for( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++)
		{
			CFepIEC103Device* pDevice = dynamic_cast<CFepIEC103Device*>(pDriver->GetDeviceByIndex(i));

			if ( pDevice && !pDevice->IsSimulate() )
			{
				if ( pDevice->IsOnLine() )
				{
					OpcUa_CharA pchMsg[128];
					snprintf( pchMsg, sizeof(pchMsg), 
						"连接网络超时，设备%s(地址%d)可疑！", pDevice->GetName().c_str(), pDevice->GetAddr() );

					pDriver->WriteLogAnyWay(pchMsg);
				}

				pDevice->SetSuspicious( true );
			}
		}

		//! 置设备离线的条件：所有链路IP都连接失败
		if( pDriver->m_iCurSerAddrIndex >= pDriver->m_iSerAddrNum-1 )
		{
			for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				CFepIEC103Device* pDevice = dynamic_cast<CFepIEC103Device*>(pDriver->GetDeviceByIndex(i));
				if ( NULL != pDevice )
				{
					if ( pDevice->m_byRxTimes == 0 )
					{
						if ( !pDevice->IsOffLine() )
						{
							OpcUa_CharA pchMsg[128];
							snprintf( pchMsg, sizeof(pchMsg), 
								"所有设备的链路IP都连接失败，设备%s(地址%d)离线了！", pDevice->GetName().c_str(), pDevice->GetAddr() );

							pDriver->WriteLogAnyWay(pchMsg);
						}
						pDevice->SetOnLine( DEVICE_OFFLINE );
						pDevice->m_byTxTimes = 0;
						pDevice->m_byRxTimes = 0;
					}
				}
			}
		}


		//! 重连
		pDriver->m_bIsConnecting = true;

		pDriver->ReConnect(true);

	}

	/*!
	*	\brief	重连
	*
	*	如果连接失败，则重连，
	*	单个服务器连续m_byMaxReConn次连接失败则换下一个服务器
	*
	*/
	void CFepIEC103Drv::ReConnect( bool bNext )
	{
		//! 试连次数加1
		IncrConnTime();

		CFepIEC103Prtcl* pPrtcl = 
			dynamic_cast<CFepIEC103Prtcl*>(m_pIOPrtclParser);


		//! 超过最大试连次数，则试连下一个服务器
		if( bNext || ( m_iConnTimes > pPrtcl->m_iMaxReConn ) )
		{
			m_iConnTimes = 0;
			ConnectNext();
		}
		else
		{
			//! 重连
			Connect(m_iCurSerAddrIndex);
		}

		return;
	}

	/*!
	*	\brief	连接下一个服务器
	*/
	void CFepIEC103Drv::ConnectNext()
	{
		m_iConnTimes = 0;
		if( ++m_iCurSerAddrIndex >= m_iSerAddrNum)
			m_iCurSerAddrIndex = 0;

		Connect( m_iCurSerAddrIndex );
		return;
	}

	OpcUa_Int32 CFepIEC103Drv::WriteLogAnyWay( const OpcUa_CharA* pchLog, bool bTime /*= true */ )
	{
		if ( !m_pLog )
		{
			return -1;
		}

		m_pLog->WriteLog( pchLog, bTime );

		return 0;
	}

	bool CFepIEC103Drv::CanWork()
	{
		return m_bCanWork;
	}

	CPRDCfgParser* CFepIEC103Drv::GetPRDCfgParser()
	{
		if (m_pPrdCfgParser)
		{
			return m_pPrdCfgParser;
		}
		else
		{
			return NULL;
		}
	}
}
