/*
*	\file	FepModbusPrtcl.cpp
*
*	\brief	通用Modbus协议类实现源文件
*
*	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusPrtcl.cpp, 470+1 2011/04/22 01:56:47 xuhongtao $
*	$Author: xuhongtao $
*	$Date: 2011/04/22 01:56:47 $
*	$Revision: 470+1 $
*
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#ifndef _FEP_TEPIF_PRTCL_20080513_CPP_
#define _FEP_TEPIF_PRTCL_20080513_CPP_

#include "ace/High_Res_Timer.h"
#include "FepModbusDrvH.h"
#include "FepModbusPrtcl.h"
#include "FepModbusDriver.h"
#include "FepModbusDevice.h"
#include "RTDBDriverFrame/IODriver.h"
#include "RTDBCommon/Debug_Msg.h"

namespace MACS_SCADA_SUD
{
	void OnTimeOut(void* p);				//! 超时定时器超时回调函数
	void PeriodProc(void* p);				//! 周期定时器超时回调函数
	void InsertDelayProc(void* p);			//! 返信请求延时定时器超时回调函数

	//! 周期定时器超时回调函数
	void PeriodProc( void* p )
	{
		//校验参数
		CFepModbusPrtcl* pPrtcl = (CFepModbusPrtcl*)p;
		if ( pPrtcl == NULL )
		{
			return;
		}

		CFepModbusDriver* pDriver = (CFepModbusDriver*)pPrtcl->GetParam();
		if ( pDriver && pDriver->IsStopWork() )
		{
			return;
		}
		CFepModbusDevice* pDevice = NULL;
		char pchMsg[128] = {0};
		Byte bFlag[256] = {0};	//! 广播标志,下标表示设备类型。

		for ( int i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepModbusDevice*>( pDriver->GetDeviceByIndex(i) );
			//离线的设备应该发送请求了。注意，链路上的部分设备离线时，允许离线设备按大周期（默认60秒）发送请求
			if (pDevice && pDevice->IsOffLine() && pDriver->m_nMaxRetryPeriod && pPrtcl->m_nCounter%pDriver->m_nMaxRetryPeriod == 0 )
			{
				pDevice->m_bRetryPeriod = true;
				if ( pDriver->IsLog())
				{
					//if (pDriver->IsEnglishEnv())
					//{
					//	snprintf(pchMsg, 128, "CFepModbusPrtcl::PeriodProc The Offline Device:%s Should try to Send a Request.", pDevice->GetName().c_str());
					//}
					//else
					//{
					//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc 链路上的离线设备:%s应该发送请求了", pDevice->GetName().c_str());
					//}
					pDriver->WriteLog( pchMsg );
				}
			}

			//触发读定值组
			if ( pDevice && pDevice->IsOnLine() && pDevice->GetSETTINGGROUP() )  
			{	
				int nRdSettingGpCycl = (pDevice->GetRdSettingGpCycl()+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
				if ( pPrtcl->m_nCounter % nRdSettingGpCycl == 0 )				//!设备到了读定值组的周期了
				{					
					pDevice->SetNeedRdSettingGp(true);		//置读定值组标志			
				}
			}

			if ( pDevice && pDevice->IsOnLine() && pDevice->HasCheckTime() )	//!设备需要校时
			{	
				int nCheckTime = (pDevice->GetCheckTimeCycl()*60000+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;

				int nCheckTimeHour = (int)(pDevice->GetCheckTimeHour());	//! 获得配置文件中的校时时间点(小时) Add by: Wangyanchao
				int nCheckTimeMin = (int)(pDevice->GetCheckTimeMin());		//! 获得配置文件中的校时时间点(分钟) Add by: Wangyanchao
				pPrtcl->gDateTime = ACE_OS::gettimeofday();		//! 获得当前的时间 Add by: Wangyanchao
				ACE_Date_Time now;								//! Add by: Wangyanchao
				now.update( pPrtcl->gDateTime );				//!Add by: Wangyanchao

				if (pDevice->GetDevType() == DEVTYPE_855 && (pPrtcl->m_nCounter % SCANPERIOD_XRL855 == 0))	//! 855协议设备使用定点校时 Edit by: Wangyanchao
				{
					if ( (now.hour() == nCheckTimeHour) && (now.minute() == nCheckTimeMin) )	//! 校时时间点到了 Add by: Wangyanchao
					{
						if ( pDevice->IsBroadChckTime() )	
						{
							if ( !bFlag[(Byte)pDevice->GetDevType()] )
							{
								pDevice->SetNeedCheckTime(true);	//置校时标志
								bFlag[(Byte)pDevice->GetDevType()] = true;
							}
						}
						else	//点对点对时，每个设备都置标志
						{
							pDevice->SetNeedCheckTime(true);	//置校时标志
						}
					}
				} 

				else	//!非855设备使用周期校时 Edit by: Wangyanchao
				{
					if (pPrtcl->m_nCounter % nCheckTime == 0)
					{
						//! 使用广播帧校时，对同类型的所有设备，每轮只需发送一帧而不需要对每个设备都发
						if ( pDevice->IsBroadChckTime() )	
						{
							if ( !bFlag[(Byte)pDevice->GetDevType()] )
							{
								pDevice->SetNeedCheckTime(true);	//置校时标志
								bFlag[(Byte)pDevice->GetDevType()] = true;
							}
						}
						else	//点对点对时，每个设备都置标志
						{
							pDevice->SetNeedCheckTime(true);	//置校时标志
						}
					}
				}
			}	
		}

		//判断设备离线
		if ( pDriver->GetHot())
		{
			pDriver->m_nMaxLinkOffline = pDriver->m_MaxHotLinkOffline;
		}
		else
		{
			pDriver->m_nMaxLinkOffline = pDriver->m_nMaxStandbyLinkOffline;
		}
	
		if ( pDriver->m_nMaxLinkOffline > 0 && pPrtcl->m_nCounter % pDriver->m_nMaxLinkOffline == pDriver->m_nMaxLinkOffline -1 )
		{
			for ( int i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByIndex(i));
				if ( NULL != pDevice && !pDevice->IsSimulate() && !pDevice->IsDisable() )
				{
					//!链路超时无响应
					if (!pDriver->m_bIsLinkGetRightFrame)
					{
						if (!pDevice->IsOffLine())
						{

							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc The link Over: %d Seconds There is no ResponseFrame from Outer device ，Device:%s(Address:%d)is Offline now!", pDriver->m_nMaxLinkOffline, pDevice->GetName().c_str(), pDevice->GetAddr() );
							//}
							//else
							//{
							//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc 链路超过%d秒都没有收到任何设备报文 设备%s离线了 Counter:%d！", pDriver->m_nMaxLinkOffline, pDevice->GetName().c_str(), pPrtcl->m_nCounter);
							//}
							pDriver->WriteLogAnyWay(pchMsg);							
						}
						pDevice->SetOnLine( DEVICE_OFFLINE );
					}
					else
					{
						//!链路下设备超时无响应
						if ( pDevice->m_byRxTimes == 0 )
						{
							if (!pDevice->IsOffLine())
							{

								//if (pDriver->IsEnglishEnv())
								//{
								//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc Device:%s Over:%d S no ResponseFrame from Outer device ，becomes Offline!Counter:%d", pDevice->GetName().c_str(), pDriver->m_nMaxLinkOffline);
								//}
								//else
								//{
								//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc 设备%s 超过%d秒没有收到报文 离线了!Counter:%d", pDevice->GetName().c_str(), pDriver->m_nMaxLinkOffline, pPrtcl->m_nCounter);
								//}
								pDriver->WriteLogAnyWay(pchMsg);						
							}
							pDevice->SetOnLine( DEVICE_OFFLINE );
						}
					}
					//每次检查后复归计数器。否则,部分在线设备掉线后，这些设备都永远在线。
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes = 0;
				}
			}
			//!复归链路接收报文标识
			pDriver->m_bIsLinkGetRightFrame = false;

		}
		else if ( pDriver->m_nMaxLinkOffline > 0 && pPrtcl->IsNetConn() && pDriver->m_nSerAddrNum > 1 && ((pPrtcl->m_nCounter % (pDriver->m_nMaxLinkOffline/pDriver->m_nSerAddrNum)) == (pDriver->m_nMaxLinkOffline/pDriver->m_nSerAddrNum -1)))
		{
			bool bFlag = true;
			for ( int i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByIndex(i));
				if ( NULL != pDevice )
				{
					if ( pDevice->m_byRxTimes > 0 )
					{
						pDevice->m_byRxTimes = 0;	//这里要对所有设备接收计数器清零，因此不能break。
						bFlag = false;
					}
					else if(pDevice->IsSimulate() || pDevice->IsDisable())
					{
						bFlag = false;				//当设备在模拟时不用判断设备是否已超时因为模拟时设备不发送报文
					}
				}
			}

			if ( bFlag )
			{
				char pchMsg[128] = {0};							
				//if (pDriver->IsEnglishEnv())
				//{
				//	snprintf(pchMsg, 128, "CFepModbusPrtcl::PeriodProc Current Link over:%d seconds There is no Response On the Current IP, switch to another IP Address", pDriver->m_nMaxLinkOffline/pDriver->m_nSerAddrNum);
				//}
				//else
				//{
				//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc 当前链路超过%d秒都没有收到任何设备报文，尝试连接其他设备IP！", pDriver->m_nMaxLinkOffline/pDriver->m_nSerAddrNum );
				//}
				pDriver->WriteLogAnyWay(pchMsg);
				pDriver->SetConnected( false );
				pDriver->ReConnect( true );					
			}
		}

		pPrtcl->m_nCounter++;

		if ( pDriver->IsLog())
		{
			char pchLog[128] = {0};
			//! 写日志
			sprintf( pchLog,"CFepModbusPrtcl::PeriodProc Counter = %d!!", pPrtcl->m_nCounter );
			pDriver->WriteLog( pchLog );
		}
	}

	//! 超时定时器超时回调函数
	void OnTimeOut( void* p )
	{
		if (!p)
		{
			return;
		}

		CFepModbusPrtcl* pPrtcl = (CFepModbusPrtcl*)p;
		//!获得Driver
		CFepModbusDriver* pDriver = (CFepModbusDriver*)pPrtcl->GetParam();
		if ( pDriver && pDriver->IsStopWork() )
		{
			return;
		}
		char pchInfo[256] = {0};
		int nTimerId = pPrtcl->m_pIOTimer->GetTimerID();
		int nRet = pPrtcl->m_pIOTimer->KillTimer();
		if ( nRet >= 0 )
		{
			if (pDriver->IsLog())
			{
				//sprintf( pchInfo, 
				//	"CFepModbusPrtcl:: OnTimeout Kill Timer Success, Timer Id:%d, KRes:%d", nTimerId, nRet );
				pDriver->WriteLog( pchInfo );
			}
		}
		else
		{
			if (pDriver->IsLog())
			{
				//sprintf( pchInfo, 
				//	"Error：CFepModbusPrtcl:: OnTimeout Kill Timer Error, Timer Id:%d", nTimerId, nRet );
				pDriver->WriteLog( pchInfo );
			}
		}

		//! 上次发送帧不是广播
		if ( pPrtcl->GetLastAddr() > 0 && pPrtcl->GetLastAddr() < 256 )
		{
			//! 获得上次发送帧的设备
			CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>( 
				pDriver->GetDeviceByAddr( pPrtcl->GetLastAddr() ));

			if( pDevice )
			{
				//! 强制写日志
				if ( pPrtcl->m_nSndLen > 0 )
				{
					std::string szData = pPrtcl->GetCommData( pPrtcl->m_pbySndBuffer, pPrtcl->m_nSndLen, COMMDATA_SND);

					char pchLog[MAXDEBUGARRAYLEN];
					//if (pDriver->IsEnglishEnv())
					//{
					//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::OnTimeOut The Device:%s RcvTimeOut, The Request Frame:%s",pDevice->GetName().c_str(), szData.c_str());
					//}
					//else
					//{
					//	snprintf( pchLog, sizeof(pchLog), "设备响应超时,设备名：%s, 本次发送帧：%s", pDevice->GetName().c_str(), szData.c_str());
					//}
					pDriver->WriteLogAnyWay( pchLog );
				}

				if (!pDevice->IsSimulate())
				{
					//! 增加超时次数
					pDevice->IncrTrySendCount();
				}
				
				if ( pDevice->IsSimulate() )
				{
					if (pDriver->IsLog())
					{
						char pch[256];
						//if (pDriver->IsEnglishEnv())
						//{
						//	snprintf(pch, sizeof(pch), "CFepModbusPrtcl::OnTimeOut Device:%s RcvTimeOut but the Device is Simulate", pDevice->GetName().c_str());
						//}
						//else
						//{
						//	snprintf( pch, sizeof(pch), "CFepModbusPrtcl::OnTimeOut 设备:%s 接收超时但设备处于模拟状态", pDevice->GetName().c_str());
						//}
						pDriver->WriteLog( pch );
					}
				}
				
				//! 如果是备机，且当前发送通道诊断帧，则根据超时次数来判断在线离线
				if ( !pDevice->IsHot() && pPrtcl->IsRTUChannelTest())
				{
					pDevice->SetSlaveTimeOuts( pDevice->GetSlaveTimeOuts() + 1 );	//! 从机超时加1
					pPrtcl->SetRTUChannelTest(false);								//! 清标志
				}				
			}			
		}	

		//////////////////////////////////////////////////////////////////////////////
		//! 发送新数据
		int nDataLen = 0;
		pPrtcl->BuildRequest( pPrtcl->m_pbySndBuffer, nDataLen );
		//////////////////////////////////////////////////////////////////////////////
	}

	//! 返信请求延时定时器超时回调函数	
	void InsertDelayProc( void*pParent )
	{
		//校验参数
		CFepModbusPrtcl* pPrtcl = (CFepModbusPrtcl*)pParent;
		if( !pPrtcl )
		{
			return;
		}

		//关闭定时器
		pPrtcl->m_pInsertDelayTimer->KillTimer();

		pPrtcl->SetCtrlInsertLen( pPrtcl->GetCtrlInsertTempLen() );
		pPrtcl->SetCtrlInsertTempLen( 0 );
	}


	///////////////////////////////////////////////////////////////////////////
	std::string CFepModbusPrtcl::s_ClassName("CFepModbusPrtcl");	//! 类名

	CFepModbusPrtcl::CFepModbusPrtcl(void) 
	{
		//! 创建定时器
		m_nCounter = 0;
		m_pTimerMgr = new ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>();
		ACE_High_Res_Timer::global_scale_factor (); 
		m_pTimerMgr->timer_queue()->gettimeofday( ACE_High_Res_Timer::gettimeofday_hr );
		size_t stack_size[1]={2*1024*1024};
		m_pTimerMgr->activate(THR_NEW_LWP|THR_DETACHED|THR_INHERIT_SCHED,1,0,ACE_DEFAULT_THREAD_PRIORITY,-1,0,0,0,stack_size);
		m_pIOTimer = new CIOTimer( OnTimeOut, this, true, m_pTimerMgr);
		m_pInsertDelayTimer = new CIOTimer(InsertDelayProc, this, true, m_pTimerMgr );
		m_pPeriodTimer = new CIOTimer( PeriodProc, this, false, m_pTimerMgr);

		m_nCtrlInsertLen = 0;
		m_nCtrlInsertTempLen = 0;
		m_nCtrlSendBufferLen = 0;			//! 上次发送信息有效长度

		m_nMaxResend = 0;					//! 最大重发次数
		m_nCurResend = 0;					//! 当前重复次数		

		m_byPrtclType = 0;					//! 默认是Modbus RTU
		m_wdPacketNo = 0;					//! 事务单元标识（包序号）

		m_bycmdType = 0;
		m_fCmdVal = 0;
		m_bRTUChannelTest = false;			//! 初始化通道诊断帧为false

		m_pSendData = NULL;
		m_bIsFirstTime = true;
		m_bIsBroadFrame = false;

		m_nSndLen = 0;
	}

	CFepModbusPrtcl::~CFepModbusPrtcl(void)
	{
		if(m_pTimerMgr!=NULL)
		{
			m_pTimerMgr->deactivate();
			//delete m_pTimerMgr;
		}
		if ( m_pSendData )
		{
			delete []m_pSendData;
			m_pSendData = NULL;
		}

		if ( m_pIOTimer )
		{
			m_pIOTimer->KillTimer();
			//delete m_pIOTimer;
			//m_pIOTimer = NULL;
		}

		if ( m_pPeriodTimer )
		{
			m_pPeriodTimer->KillTimer();
			//delete m_pPeriodTimer;
			//m_pPeriodTimer = NULL;
		}

		if ( m_pInsertDelayTimer )
		{
			m_pInsertDelayTimer->KillTimer();
			//delete m_pInsertDelayTimer;
			//m_pInsertDelayTimer = NULL;
		}
	}

	/*!
	*	\brief	初始化工作
	*/
	void CFepModbusPrtcl::Init( Byte byPrtclType, int nMaxReSend  )
	{
		m_byPrtclType = byPrtclType;
		m_nMaxResend = nMaxReSend;		

		if( m_byPrtclType == 1 )			//!< Modbus TCP
		{
			InitPrtclHelper( 9, MB_TCP_MAX_SEND_SIZE,
				MB_TCP_MAX_SEND_SIZE, MB_TCP_MAX_SEND_SIZE, 2*MB_TCP_MAX_SEND_SIZE );
			//! 为发送数据存储区分配空间
			m_pSendData = new SENDDATA[65536];
		}
		else
			InitPrtclHelper( 5, MB_RTU_MAX_SEND_SIZE,
			MB_RTU_MAX_SEND_SIZE, MB_RTU_MAX_SEND_SIZE, 2*MB_RTU_MAX_SEND_SIZE );
	}

	std::string CFepModbusPrtcl::GetClassName()
	{
		return s_ClassName;
	}

	//! 得到最小的包长度
	int CFepModbusPrtcl::GetMinPacketLength()
	{
		return PRTCL_MIN_PACKET_LENGTH;
	}

	/*!
	*	\brief	组请求帧
	*
	*	\param	pbyData		帧首地址
	*	\param	nDataLen	帧长
	*
	*	\retval	int	0表示成功，否则表示失败
	*/
	int CFepModbusPrtcl::BuildRequest( Byte* pbyData, int& nDataLen )
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		if (NULL == pDriver)
		{
			return -1;
		}
		
		char pchLog[MAXDEBUGARRAYLEN] =  {0};
		bool bIsRecord = pDriver->IsLog();
		CFepModbusDevice* pDevice = NULL;
		int nTimeOut = 500;			//!超时时间，组完帧后由设备超时时间确定
		int nFlag = 1;
		int nRet = 0;
		nDataLen = 0;
		
		/*
		 *	组帧，以便发送
		 *	组到合适的帧便break后执行发送操作
		 *	组帧失败则直接break以便于定时器
		 */
		while(nFlag)
		{
			nFlag = 0;
			if (pDriver->IsLog())
			{
				//snprintf(pchLog, MAXDEBUGARRAYLEN, "\n\nCFepModbusPrtcl::BuildRequest Start!");
				pDriver->WriteLog(pchLog);
			}

			if ( CIOPrtclParser::BuildRequest( pbyData, nDataLen )==-1 )
			{
				if (pDriver->IsLog())
				{
					//snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl:: BuildRequest Failed, The Driver is Diable!" );
					////! 记录日志
					//pDriver->WriteLog( pchLog );
				}
				nRet = -1;
				break;
			}

			//! 如果是网络介质，且没有连接，则不发送直接返回
			if( ( IsNetConn() )
				&& ( /*!pDriver->m_pIODrvBoard->ConnState() || */!pDriver->GetConnected() ))
			{
				nRet = -4;
				break;
			}

			//! 判断是否需要重发
			if ( m_nMaxResend > 0					//只有配置为需要重发的驱动才考虑重发
				&& m_nCurResend < m_nMaxResend		//还在重发次数范围内
				&& m_nCtrlSendBufferLen > 0			//确认上次发过了
				&& m_pbyCtrlSendBuffer[0] != 0 		//广播帧不重发
				&& ( m_pbyCtrlSendBuffer[1] == 5 || m_pbyCtrlSendBuffer[1] == 6 
				|| m_pbyCtrlSendBuffer[1] == 15 
				|| 	m_pbyCtrlSendBuffer[1] == 16 )	//遥控帧才需要考虑重发
				)
			{
				//! 取得发送帧的设备
				pDevice = dynamic_cast<CFepModbusDevice*>( 
					pDriver->GetDeviceByAddr( m_pbyCtrlSendBuffer[0] ) );				

				if ( pDevice && pDevice->IsOnLine() )	//设备在线时才考虑重发
				{
					//! 发送延时
					ACE_Time_Value tv( 0, (pDevice->GetPortCfg().nSndDelay)*1000 );
					ACE_OS::sleep( tv );

					m_nCurResend++;
					if ( m_nCurResend >= m_nMaxResend )
					{
						m_nCurResend = m_nMaxResend;
					}
					nDataLen = m_nCtrlSendBufferLen;
					memcpy( pbyData, m_pbyCtrlSendBuffer, nDataLen );

					//! 置超时时间
					nTimeOut = pDevice->GetPortCfg().nTimeOut;
					if (pDriver->IsLog())
					{
						//if (pDriver->IsEnglishEnv())
						//{
						//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest Dev:%s Refuse to Action,Starting %d times Retry");
						//}
						//else
						//{
						//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest 设备地址%d拒动，开始第%d次重试!", pDevice->GetAddr(), m_nCurResend);
						//}
						//pDriver->WriteLog( pchLog );
					}
					break;
				}
				else	
					m_nCurResend = m_nMaxResend;
			}	

			//检查并发送遥控伴随帧(伴随帧由上一条遥控生成)
			if ( m_nCtrlInsertLen > 0 )
			{
				//! 取得发送帧的设备
				pDevice = dynamic_cast<CFepModbusDevice*>( 
					pDriver->GetDeviceByAddr( m_pbyCtrlInsertBuf[0] ) );

				if ( pDevice )
				{
					//! 发送延时
					ACE_Time_Value tv( 0, (pDevice->GetPortCfg().nSndDelay)*1000 );
					ACE_OS::sleep( tv );
				}

				nDataLen = m_nCtrlInsertLen;
				memcpy( pbyData, m_pbyCtrlInsertBuf, m_nCtrlInsertLen );
				m_nCtrlInsertLen = 0;
				break;
			}

			//! 发遥控,判断如果有遥控,则发遥控
			nRet = BuildCtrlFrame( pbyData, nDataLen, nTimeOut, m_fCmdVal);
			if ( nDataLen > 0 && nRet == 0 )
			{
				/*
				 *	记录遥控数据，以便重发；此处只记录为RTU格式帧，
				 *	如需重发或处理，则根据需要再转为TCP格式帧
				 */
				ACE_OS::memset(m_pbyCtrlSendBuffer, 0, MB_RTU_MAX_SEND_SIZE);
				if (nDataLen < MB_RTU_MAX_SEND_SIZE)
				{
					ACE_OS::memcpy(m_pbyCtrlSendBuffer, pbyData, nDataLen);
					m_nCtrlSendBufferLen = nDataLen;
				}
				else
				{
					ACE_OS::memcpy(m_pbyCtrlSendBuffer, pbyData, MB_RTU_MAX_SEND_SIZE);
					m_nCtrlSendBufferLen = MB_RTU_MAX_SEND_SIZE;
				}
				//! 取得发送帧的设备
				pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByAddr( m_pbyCtrlSendBuffer[0]));
				m_nCurResend = 0;	//! 把上一次的重复计数器清零
				break;
			}

			//!MODBUS RTU，且接收到了另机发的诊断包,则直接发送
			if ( m_bSendPacketIsFromPeer && m_cmdPacket.nLen>0 && ( m_byPrtclType == 0 || m_byPrtclType ==2) )
			{
				memcpy( pbyData, m_cmdPacket.byData, m_cmdPacket.nLen );
				nDataLen = m_cmdPacket.nLen;

				//! 取得发送帧的设备
				pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByAddr( pbyData[0]));
				if ( !pDevice )
				{
					m_bSendPacketIsFromPeer = false;
					m_cmdPacket.nLen = 0;
					nRet = -1;
					break;
				}

				if (pDriver->IsLog())
				{
					std::string strContent = GetCommData(pbyData, nDataLen, COMMDATA_SND);
					//if (pDriver->IsEnglishEnv())
					//{
					//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest Send LinkDiagFrame Dev:%s", pDevice->GetName().c_str());
					//}
					//else
					//{
					//	snprintf( pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest 收到主机链路诊断包 Frame:%s 设备:%s!", strContent.c_str(), pDevice->GetName().c_str());
					//}
					pDriver->WriteLog( pchLog );
				}

				//! 置超时时间
				nTimeOut = pDevice->GetPortCfg().nTimeOut;
				m_bSendPacketIsFromPeer = false;
				m_cmdPacket.nLen = 0;
				m_bycmdType = 0;
				m_bRTUChannelTest = true;			//! 置通道诊断标志为true

				/*
				 *	清除串口缓冲区中的数据（这些应该都是另机发送帧的响应帧）
				 *	以便接到本机响应帧后的解析
				 */
				m_iInnerLen = 0;
				nRet = 0;
				break;
			}

			//! 更换设备
			pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetNextDevice());
			if( !pDevice )
			{
				nRet = -1;
				break;
			}

			//! 判断设备是否被模拟了，如果被模拟了，则不发送请求了
 			if ( pDevice->IsSimulate() )
 			{
 				//! 写日志
 				sprintf( pchLog,"CFepModbusPrtcl::BuildRequest Device:%s Is Simulated!", pDevice->GetName().c_str());
 				pDriver->WriteLog( pchLog );
 				nRet = -2;
 				break;
 			}

			//! 是MODBUS RTU，则需要进行链路诊断
			if( m_byPrtclType == 0 ||  m_byPrtclType == 2)
			{
				//! 进行链路诊断
				int nRest = 0;
				int nLinkDataLen = 0;
				nRest = DealLinkClash( pDevice, pbyData, nLinkDataLen );
				if ( nRest==0 )	//! 组出链路诊断包,并发送了
				{
					//!置通道诊断标志为true
					m_bRTUChannelTest = true;
					//!置超时时间
					nTimeOut = pDevice->GetPortCfg().nTimeOut*2;
					//!恢复其余设备通道诊断状态每次只对一个设备进行通道诊断
					pDriver->SetDevCHNCheck(false);

					if (pDriver->IsLog())
					{
						std::string strContent = GetCommData(pbyData, nLinkDataLen, COMMDATA_SND);
						//if (pDriver->IsEnglishEnv())
						//{
						//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest Build LinkDaigFrame and Send to Peer!Device:%s,Frame:%s", pDevice->GetName().c_str(), strContent.c_str());
						//}
						//else
						//{
						//	snprintf( pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest 组链路诊断包并发送给另机!Device:%s,Frame:%s",pDevice->GetName().c_str(), strContent.c_str());
						//}
						pDriver->WriteLog( pchLog );
					}
					nRest = 0;
					break;
				}
			}

			//! 发送延时
			ACE_Time_Value tv( 0, (pDevice->GetPortCfg().nSndDelay)*1000 );
			ACE_OS::sleep( tv );

			/*
			 *	对于MODBUS RTU，如果链路冲突且设备为实际主、
			 *				或链路不冲突，则进行正常的数据请求
			 *	对于MODBUS TCP，则直接发送
			 */
			if ( ( pDriver->IsPortAccessClash() && pDevice->IsHot() ) 
				|| !pDriver->IsPortAccessClash() 
				|| ( m_byPrtclType != 0 &&  m_byPrtclType != 2 ))
			{
				nDataLen = pDevice->BuildLocalNormalFrame( pbyData );
				if (nDataLen > 0)
				{
					//!组出了有效报文才按照链路超时进行等待否则尽快进入下次轮询
					nTimeOut = pDevice->GetPortCfg().nTimeOut;
				}
				m_bycmdType = 0;
			}
		}

		//无论组包成功与否，都更新发送长度。避免从机没有发送请求时，在接收超时定时器中错误的写发送请求日志
		m_nSndLen = nDataLen;

		//! 组帧正确且帧长度大于0，根据协议类型（RTU或TCP）进行备份处理
		if (nRet == 0 && nDataLen > 0)
		{			
			//! 备份并发送数据帧
			BackupAndSendRequest( pDevice, pbyData, nDataLen );	
		}		

		//! 不是广播帧才启动定时器，
		if ( !m_bIsBroadFrame )
		{
			//!防止同时启动两个定时器
			int nTId = m_pIOTimer->GetTimerID();
			int nRe = m_pIOTimer->KillTimer();
 			if (pDriver->IsLog())
 			{
 				//! 写日志
 				//snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::BuildRequest Kill Timer Id:%d Res:%d!", nTId, nRe );
 				pDriver->WriteLogAnyWay( pchLog );
 			}
			
			//! 启动定时器
			if( m_pIOTimer->SetTimer(nTimeOut) == 0 )
			{
				if (pDriver->IsLog())
				{
					//snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::BuildRequest SetTimer Success, TimerID = %d!", m_pIOTimer->GetTimerID() );
					pDriver->WriteLogAnyWay( pchLog );
				}
			}
			else
			{
				if (pDriver->IsLog())
				{
					//snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::BuildRequest SetTimer Error, TimerID = %d!", m_pIOTimer->GetTimerID() );
					pDriver->WriteLog( pchLog );
				}
			}
		}
		else
		{
			m_bIsBroadFrame = false;
		}

		return nRet;
	}

	/*!
	*	\brief	备份并发送请求
	*
	*	\param	pDevice		发送帧的设备
	*	\param	pbyData		数据包首地址
	*	\param	nDataLen	数据包长度
	*
	*	\retval	bool	成功true，否则false
	*/
	int CFepModbusPrtcl::BackupAndSendRequest( 
		CFepModbusDevice* pDevice, Byte* pbyData, int nDataLen )
	{
		if( !pbyData || nDataLen <= 0)
			return -1;

		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

		/////////////////////////////////////////////////////////////////////////
		//! 如果组帧不是在m_pbySndBuffer中进行，
		//! 则拷贝到m_pbySndBuffer缓冲区中，以便后续接受到响应帧后的分析和处理
		if( pbyData != m_pbySndBuffer)
		{
			memcpy( m_pbySndBuffer, pbyData, nDataLen );
		}

		//! 记录上次发送帧的信息，便于校验
		BackupLastRequest();

		//! 如果是TCP Modbus，则转换成TCP格式帧，并记录
		if( IsModbusTCP() )
		{
			nDataLen = MBRTU2MBTCPRequest( pbyData, nDataLen, pbyData );

			if( nDataLen > 0 )
			{
				FRAME_HEAD* pHead = (FRAME_HEAD* )pbyData;
				WORD PacketNo =  MAKEWORD(pHead->byPckNoLow, pHead->byPckNoHigh);
				m_pSendData[PacketNo].bySendFunc = m_bySendFunc;
				m_pSendData[PacketNo].wdSendStartAddr = m_wdSendStartAddr;
				m_pSendData[PacketNo].wdSendPoints = m_wdSendPoints;
			}
		}

		//! 组帧正确且帧长度大于0，则发送数据帧
		if ( nDataLen > 0)
		{
			//! 获取发送帧的设备
			if( !pDevice )
			{
				pDevice = dynamic_cast<CFepModbusDevice*>( 
					pDriver->GetDeviceByAddr( m_bySendAddr ));

				if ( !pDevice )
				{
					return -2;
				}
			}

			//! 发送次数加1
			pDevice->m_byTxTimes ++;			

			//! 发送数据帧
			SendRequest( pDevice, pbyData, nDataLen );			

			//! 如果是广播帧，则直接发送下一帧
			if( m_bySendAddr == 0 && pDevice->GetAddr() != 0 )
			{
				//! 延时100毫秒
				ACE_Time_Value tv( 0, 100 * 1000 );
				ACE_OS::sleep( tv );

				///////////////////////////////////////////////////////////////////////
				//! 发送数据
				int nDataLen = 0;
				BuildRequest( m_pbySndBuffer, nDataLen );
				///////////////////////////////////////////////////////////////////////

				//! 置广播帧标志，防止重复启动定时器
				m_bIsBroadFrame = true;
			}
		}

		return 0;
	}

	/*!
	*	\brief	发送请求
	*
	*	\param	pDevice		发送帧的设备
	*	\param	pbyData		数据包首地址
	*	\param	nDataLen	数据包长度
	*
	*	\retval	bool	成功true，否则false
	*/
	int CFepModbusPrtcl::SendRequest( 
		CFepModbusDevice* pDevice, Byte* pbyData, int nDataLen )
	{
		if( !pbyData || nDataLen <= 0)
			return -1;

		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		m_nSndLen = nDataLen;

		if (pDriver->m_pIODrvBoard)
		{
			//! 发送数据
			if( pDriver->m_pIODrvBoard->Write(pbyData, nDataLen) != 0)
			{
				//! 写日志
				if ( nDataLen>0 && pDriver->IsMonitor())
				{
					pDriver->WriteCommData( pbyData, nDataLen, COMMDATA_SND );
				}
				if (pDriver->IsLog())
				{
					pDriver->WriteLog( "CFepModbusPrtcl::SendRequest Write Failed!!" );
				}
				return -3;
			}
			else				//! 发送成功则记录日志，添加通讯监视
			{
				//! 如果是遥控或遥调
				if( m_bycmdType == 1 || m_bycmdType == 2 )
				{
					pDevice->SetVal(
						m_tCtrlRemoteCmd.byUnitType,
						m_tCtrlRemoteCmd.lTagAddress,
						m_tCtrlRemoteCmd.byData,
						m_tCtrlRemoteCmd.nDataLen,
						0, true );
				}

				//! 写日志
				pDriver->WriteLog( "CFepModbusPrtcl::Send Request Success!" );
				//! 记录通信监视
				if ( nDataLen>0 && pDriver->IsMonitor())
				{
					pDriver->WriteCommData( pbyData, nDataLen, COMMDATA_SND );
				}
			}
		}

		return 0;
	}

	//! 给指定设备组通道诊断数据帧
	void CFepModbusPrtcl::MakeChannelDiagFrame( 
		CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen )
	{
		CFepModbusDevice* pDev = dynamic_cast<CFepModbusDevice*> ( pDevice );
		if ( !pDev || pDev->IsSimulate() )
		{
			return;
		}

		//! 如果有测试帧
		if( pDev->GetTestReqStatus() )
			nReturnLen = pDev->BuildTestFrame( pbyData );
		else
			nReturnLen = pDev->BuildQueryFrame( pbyData ); 

		//! 备份数据，以便另机发送的帧本机收到响应后能正确处理
		/////////////////////////////////////////////////////////////////////////////
		//! 记录上次发送帧的信息，便于校验
		m_bySendAddr = pbyData[0];
		m_bySendFunc = pbyData[1];
		m_wdSendStartAddr = MAKEWORD( pbyData[3], pbyData[2] );
		m_wdSendPoints = MAKEWORD( pbyData[5], pbyData[4] );
	}	

	/*!
	*	\brief	分析出接收数据包
	*
	*	\param	pPacketBuffer	分析出的数据包
	*	\param	pnPacketLength	数据包长度
	*
	*	\retval	bool	成功true，否则false
	*/
	bool CFepModbusPrtcl::ExtractPacket(  Byte* pPacketBuffer, int* pnPacketLength )
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

		//校验
		if( !pPacketBuffer || !pDriver || !m_pbyRevBuffer)
			return false;
		 
		bool bACompletePacketRcvd = false;
		Byte* pFrame = m_pbyRevBuffer;
		int nCorrectLen = 0;		//!正确帧应有的长度
		bool bHasReqest = false;			//!是否从接收数据流中解析出Modbus请求帧
		char pchLog[MAXDEBUGARRAYLEN] = {0};

		//!校验帧的和理性
		while(pFrame + PRTCL_MIN_PACKET_LENGTH <= m_pbyRevBuffer + m_nRevLength )
		{
			//////////////////////////////////////MODIFYHERE START
			//缓冲区余度
			//   ---------------------------------------------------
			//   |                  |                              |
			//m_pbyRevBuffer     pFrame
			//   |<-----------------m_nRevLength------------------>|
			///////////////////////////////////////MODIFYHERE END
			if( IsModbusTCP() )				//! TCP Modbus
			{
				//! 根据记录的发送信息找到合适的数据帧帧头
				Byte bFlag = false;
				int i = 0;
				for ( i = 0; 
					i < (int)( m_nRevLength-sizeof(FRAME_HEAD)-(pFrame - m_pbyRevBuffer) );
					i++ )
				{
					FRAME_HEAD* pHead = (FRAME_HEAD*)(m_pbyRevBuffer+i);
					if ( pHead->byPrtclHigh != 0X00 
						|| pHead->byPrtclLow != 0X00
						|| pHead->bySlaveAddress != m_bySendAddr )
					{
						continue;
					}

					WORD PacketNo = MAKEWORD( pHead->byPckNoLow, pHead->byPckNoHigh );
					Byte byFunc	 = m_pbyRevBuffer[i+sizeof(FRAME_HEAD)];

					if ( ( byFunc&0x7F ) == m_pSendData[PacketNo].bySendFunc )
					{
						bFlag = true;
						break;
					}
				}
				if ( !bFlag ) //! 在接收数据中没有找到地址和功能码
				{
					break;
				}
				//! 移动pFrame到当前位
				pFrame += i;
				m_byActiveAddr = pFrame[sizeof(FRAME_HEAD)-1];
				m_byActiveFunc = pFrame[sizeof(FRAME_HEAD)];
			}
			else							//! RTU Modbus
			{
				if ( pDriver->GetHot() || !pDriver->IsPortAccessClash() )
				{
					//! 根据上一次发送帧信息，在接收数据中找到地址和功能码
					Byte bFlag = false;
					int i = 0;
					for( i=0; i < m_nRevLength-1-(pFrame - m_pbyRevBuffer); i++ )
					{
						if ( pFrame[i] == m_bySendAddr && (pFrame[i+1] & 0x7F) == m_bySendFunc)	
						{
							bFlag = true;
							break;
						}
					}
					if ( !bFlag ) //! 在接收数据中没有找到地址和功能码
					{
						break;
					}
					//! 移动pFrame到当前位
					pFrame += i;
					m_byActiveAddr = pFrame[0];
					m_byActiveFunc = pFrame[1];
				}
				else //如果是从机，并且为冲突模式时，则先尝试从接收数据流中解析出Modbus请求帧
				{
					if ( !bHasReqest )
					{
						if ( m_nRevLength + m_pbyRevBuffer - pFrame >= 8 )
						{
							if ( pFrame[0] != 0 )	//设备地址0只能是主站发送的广播帧
							{
								bool bIsReqeust = false;
								CFepModbusDevice* pDevice = NULL;
								if ( pFrame[1] == 1 || pFrame[1] == 2 ) 
								{
									if ( pFrame[2] != 3 )//注意，8个字节长的功能码1、2的响应帧，字节个数只能为3
									{
										pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByAddr( pFrame[0] ) );
										if ( pDevice )
										{
											bIsReqeust = true;
										}
									}
									else if ( pFrame[4] == 0 && pFrame[5] >= 17 && pFrame[5] <= 24 )
									{
										pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByAddr( pFrame[0] ) );
										if ( pDevice )
										{
											if ( pDevice->IsReqeustExited( pFrame[1], MAKEWORD(pFrame[3],pFrame[2]), MAKEWORD(pFrame[5],pFrame[4]) ))
											{
												bIsReqeust = true;
											}
										}
									}
								}
								else if ( pFrame[1] == 3 || pFrame[1] == 4 )	  //功能码3、4的8字节帧只能是请求帧。
								{
									pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByAddr( pFrame[0] ) );
									if ( pDevice )
									{
										if ( pDevice->IsReqeustExited( pFrame[1], MAKEWORD(pFrame[3],pFrame[2]), MAKEWORD(pFrame[5],pFrame[4]) ))
										{
											bIsReqeust = true;
										}
									}
								}

								if ( bIsReqeust )
								{
									if ( pDevice )
									{
										//! 校验CRC
										int nCorrectLen = 8;
										WORD CheckSum = CRC( pFrame, nCorrectLen - 2 );
										if (  (( pDevice->GetPortCfg().nCRCOrder == CRC_HOB_FIRST) && ( pFrame[nCorrectLen-2] == HIBYTE(CheckSum) && pFrame[nCorrectLen-1] == LOBYTE(CheckSum) ) )
											||(( pDevice->GetPortCfg().nCRCOrder == CRC_LOB_FIRST) && ( pFrame[nCorrectLen-2] == LOBYTE(CheckSum) && pFrame[nCorrectLen-1] == HIBYTE(CheckSum) ) ) )
										{
											//! 保存到本地发送缓存，以备从机下次收到响应时能成功解析出来
											memcpy( m_pbySndBuffer, pFrame, nCorrectLen );

											//! 记录发送帧的信息，便于校验
											BackupLastRequest();

											if ( pDriver->IsLog())
											{
												//char pchLog[MAXDEBUGARRAYLEN];
												std::string szSndPacket = GetCommData( pFrame, nCorrectLen, COMMDATA_SND);
												//if (pDriver->IsEnglishEnv())
												//{
												//	snprintf( pchLog, sizeof(pchLog),"CFepModbusPrtcl::ExtractPacket the Peer Recongnize RequestFrame sending by Peer,Content: %s", szSndPacket.c_str() );
												//}
												//else
												//{
												//	snprintf( pchLog, sizeof(pchLog),"CFepModbusPrtcl::ExtractPacket 从机识别出另机发送的请求帧: %s", szSndPacket.c_str() );
												//}
												pDriver->WriteLog( pchLog );
											}

											//! 去掉被处理掉的数据
											m_nRevLength -= pFrame - m_pbyRevBuffer + nCorrectLen;
											if ( m_nRevLength > 0 ) 
											{
												memmove( m_pbyRevBuffer, pFrame + nCorrectLen, m_nRevLength );
											}

											//! 如果剩余长度不足，则等待下次触发接收回调再尝试解析
											if ( m_nRevLength <= PRTCL_MIN_PACKET_LENGTH )
											{
												break;
											}
											bHasReqest = true;

											//! 移动pFrame到当前位后继续解析
											pFrame = m_pbyRevBuffer;
											continue;
										}
										else
										{
											pFrame++;
											continue;
										}
									}
									else
									{
										pFrame++;
										continue;
									}
								}
								else
								{
									pFrame++;
									continue;
								}
							}
							else
							{
								pFrame++;
								continue;
							}
						}
						else
						{
							//! 如果从机没有从接收数据流中解析出请求帧，则退回pFrame到起始位置后，尝试后面响应帧的解析
							bHasReqest = true;
							pFrame = m_pbyRevBuffer;
							continue;
						}
					}

					//下面开始尝试响应帧的解析
					//! 根据上一次发送帧信息，在接收数据中找到地址和功能码
					Byte bFlag = false;
					int i = 0;
					for( i=0; i < m_nRevLength-1-(pFrame - m_pbyRevBuffer); i++ )
					{
						if ( pFrame[i] == m_bySendAddr && (pFrame[i+1] & 0x7F) == m_bySendFunc)	
						{
							bFlag = true;
							break;
						}
					}

					if ( !bFlag ) //! 在接收数据中没有找到地址和功能码
					{
						break;
					}

					//! 移动pFrame到当前位
					pFrame += i;
					m_byActiveAddr = pFrame[0];
					m_byActiveFunc = pFrame[1];
				}
			}	

			//! 尝试解析遥控返信帧
			int nResult = ExtractCtrlPacket( pFrame, nCorrectLen );
			if (nResult == FLAG_NONE)		//! 不是遥控返信帧
			{
				//! 尝试解析轮询帧返回包
				nResult = ExtractMsgPacket(pFrame, nCorrectLen);
			}

			if (nResult != FLAG_CORRECTFRAME)
			{
				if (nResult == FLAG_NONE)		//! 也不是轮询帧返回包
				{
					//! 尝试解析文件操作返回包
					nResult = ExtractFilePacket(pFrame, nCorrectLen);
				}
				if (nResult == FLAG_NONE)		//! 也不是文件操作返回包
				{
					//! 尝试解析其他的特殊响应帧
					nResult = ExtractOtherPacket(pFrame, nCorrectLen);
				}

				if (nResult == FLAG_UNCOMPLETE)	//! 帧没接收完整
				{
					break;
				}
				//! 解析出错或解析不出当前帧
				if (nResult == FLAG_ERROR || nResult == FLAG_NONE)
				{
					pFrame++;
					continue;
				}
			}
			

			//! 解析出正确的帧（没有出错），则置标志且退出循环
			if ( nResult == FLAG_CORRECTFRAME)
			{
				bACompletePacketRcvd = true;
			}

			break;
		}

		if (bACompletePacketRcvd)
		{
			m_bycmdType = 0;	//! 还原发送帧类型为0

			//! 将合格的帧放置到输出缓冲区
			ACE_OS::memcpy( pPacketBuffer, pFrame, nCorrectLen );

			//! 去掉被处理掉的数据
			m_nRevLength -= pFrame - m_pbyRevBuffer + nCorrectLen;
			if ( m_nRevLength > 0 ) 
			{
				ACE_OS::memmove( m_pbyRevBuffer, pFrame + nCorrectLen, m_nRevLength );
			}

			//! 置正确应答帧的长度
			*pnPacketLength = nCorrectLen;
			if ( pDriver->IsLog())
			{
				//! 写日志
				//char pchLog[MAXDEBUGARRAYLEN];
				std::string szPacket = GetCommData( pPacketBuffer, nCorrectLen, COMMDATA_RCV );
				//snprintf( pchLog, sizeof(pchLog),"CFepModbusPrtcl::ExtractPacket Get a complete frame: %s", szPacket.c_str() );
				pDriver->WriteLogAnyWay( pchLog );
			}
		}
		else
		{
			//去掉已明确错误的数据部分
			int nErrorLen = pFrame - m_pbyRevBuffer;
			if ( nErrorLen > 0 )
			{
				//char pchLog[MAXDEBUGARRAYLEN];
				std::string szErrorPacket = GetCommData( m_pbyRevBuffer, nErrorLen, COMMDATA_RCV );
				//if (pDriver->IsEnglishEnv())
				//{
				//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::ExtractPacket Remove the invalid buffer length:%d Content:%s", nErrorLen, szErrorPacket.c_str());
				//}
				//else
				//{
				//	snprintf( pchLog, sizeof(pchLog),"CFepModbusPrtcl::ExtractPacket 去掉错误数据,共%d字节: %s", nErrorLen, szErrorPacket.c_str());
				//}
				pDriver->WriteLog( pchLog );
				m_nRevLength -= nErrorLen;
				if ( m_nRevLength > 0 )
				{				
					ACE_OS::memmove(m_pbyRevBuffer, m_pbyRevBuffer+nErrorLen, m_nRevLength);
				}
				else
				{
					m_nRevLength = 0;
				}
			}
		}
		return bACompletePacketRcvd;	
	}

	/*!
	*	\brief	处理完整的数据包
	*
	*	\param	pPacketBuffer	数据包
	*	\param	nPacketLength	数据包长度
	*	\param	bFromLocal		区分是本机来的还是另机来的
	*
	*	\retval	void
	*/
	void CFepModbusPrtcl::ProcessRcvdPacket( Byte* pPacketBuffer, int nPacketLength, bool bFromLocal )
	{
		int nFlag = 1;
		char pchLog[MAXDEBUGARRAYLEN] = {0};
		while(nFlag)
		{
			nFlag = 0;       //! 修改标志，使只执行一次
			
			//! 获得Driver
			CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
			if (!pDriver)
				break;

			//! KillTimer
			//int nTId = m_pIOTimer->GetTimerID();
			//int nKRe = m_pIOTimer->KillTimer();
			//if (pDriver->IsLog())
			//{
			//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket Kill Timer, Id:%d, Res:%d", nTId, nKRe);
			//	//! 写日志
			//	pDriver->WriteLogAnyWay(pchLog);
			//}

			//校验
			if( !pPacketBuffer || nPacketLength == 0 )
				break;			

			//! 根据地址找到对应的设备
			Byte byUnitAddr;
			bool IsExceptionResp = false;		//是否为异常应答
			if( IsModbusTCP() )					//! 如果是TCP Modbus,则刷新发送参数
			{
				FRAME_HEAD* pHead	= (FRAME_HEAD* )pPacketBuffer;
				WORD wdPacketNo	= MAKEWORD( pHead->byPckNoLow, pHead->byPckNoHigh );
				m_bySendFunc		= m_pSendData[wdPacketNo].bySendFunc;
				m_wdSendPoints		= m_pSendData[wdPacketNo].wdSendPoints;
				m_wdSendStartAddr	= m_pSendData[wdPacketNo].wdSendStartAddr;

				byUnitAddr = pPacketBuffer[sizeof(FRAME_HEAD)-1];
				IsExceptionResp = (pPacketBuffer[sizeof(FRAME_HEAD)]>0x80)?true:false;
			}
			else
			{
				byUnitAddr = pPacketBuffer[0];
				IsExceptionResp = (pPacketBuffer[1]>0x80)?true:false;
			}

			CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>(
				pDriver->GetDeviceByAddr( byUnitAddr ) );

			if ( !pDevice )
			{
				break;
			}

			//! 链路上收到任何设备的应答，立即复归链路离线计数器
			pDriver->m_bIsLinkGetRightFrame = true;

			//! 置为不需要重发
			m_nCurResend = m_nMaxResend;

			
			//! 如果是从机且收到的是通道诊断帧的响应，则清通道诊断标志
			if( !pDevice->IsHot())
			{
				if ( !IsExceptionResp || pDevice->IsExceptionAsOnline() )
				{
					if ( !pDevice->IsOnLine() )
					{
						//if (pDriver->IsEnglishEnv())
						//{
						//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket The Slave Device:%s(Address:%d)is Online Now!", pDevice->GetName().c_str(), pDevice->GetAddr());
						//}
						//else
						//{
						//	snprintf( pchLog, sizeof(pchLog), "CFepModbusPrtcl::ProcessRcvdPacket 从机设备%s(地址%d)在线了！", pDevice->GetName().c_str(), pDevice->GetAddr());
						//}
						pDriver->WriteLogAnyWay(pchLog);
					}
					pDevice->SetOnLine( DEVICE_ONLINE );
					m_bRTUChannelTest = false;
					pDevice->SetSlaveTimeOuts( 0 );
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes=1;
				}
			}

			//! 如果是主机或者是网络介质，则置设备在线
			if ( pDevice->IsHot() || IsNetConn() )
			{
				if ( !IsExceptionResp || pDevice->IsExceptionAsOnline() )
				{
					if ( !pDevice->IsOnLine() )
					{
						//if (pDriver->IsEnglishEnv())
						//{
						//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPracket Device:%s(Address:%d)is OnLine Now!",pDevice->GetName().c_str(), pDevice->GetAddr());
						//}
						//else
						//{
						//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPracket 设备%s(地址%d)在线了！", pDevice->GetName().c_str(), pDevice->GetAddr() );
						//}
						//pDriver->WriteLogAnyWay(pchLog);
					}

					//! 设置装置在线
					pDevice->SetOnLine( DEVICE_ONLINE );
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes=1;
					pDevice->SetTrySendCount( pDevice->GetMaxOffLine() );
				}
			}

			if ( pDevice->IsHot() )
			{
				int nRTULen;
				Byte* pDevAddr = NULL;
				if( !IsModbusTCP() )			//! 对于RTU Modbus
				{
					//! 如果收到的帧不是最近发送帧的响应，则不处理
					if ( pPacketBuffer[0] != m_bySendAddr 
						|| ((pPacketBuffer[1]&0x7F) != m_bySendFunc ) )
					{
						//snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPracket the RTUFrame is Obsolete！Addr:%d Func:%d", m_bySendAddr, m_bySendFunc);
						pDriver->WriteLogAnyWay(pchLog);
						break;
					}
					pDevAddr = pPacketBuffer;
					nRTULen = nPacketLength;
				}
				else	//! 如果是TCP Modbus，则需要把帧头去掉，然后再进行处理
				{
					pDevAddr = pPacketBuffer+sizeof(FRAME_HEAD)-1;
					nRTULen = nPacketLength-sizeof(FRAME_HEAD)+1;
				}				

				//! 如果设备处于模拟状态，则不处理响应帧
				if ( pDevice->IsSimulate() )
					break;

				Byte byTemp;
				int nRet = 0;
				//std::string szSndPacket = GetCommData( m_pbySndBuffer, m_nSndLen, COMMDATA_SND);
				//std::string szRcvPacket = GetCommData( pPacketBuffer, nPacketLength, COMMDATA_RCV);
				switch(pDevAddr[1])	//! 根据功能码不同来处理
				{
				case READ_DIG_OUTPUT_STATUS:						//! 功能码为1
					{
						//! 更新设备的DI点值
						nRet = pDevice->UpdateDI( MACSMB_DIA, pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal );
						//! 有变位时，输出报文
						if ( nRet > 0 )
						{
							std::string szSndPacket = GetCommData( m_pbySndBuffer, m_nSndLen, COMMDATA_SND);
							std::string szRcvPacket = GetCommData( pPacketBuffer, nPacketLength, COMMDATA_RCV);
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket There are %d DI changing Infos to the Request Frame:%s",nRet, szSndPacket.c_str());
							//}
							//else
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket 以上%d条变位信息来自报文:%s", nRet, szSndPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket The Response Frame:%s", szRcvPacket.c_str());
							//}
							//else
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket收到请求:%s", szRcvPacket.c_str());
							//}
							pDriver->WriteLogAnyWay( pchLog );
						}
					}
					break;
				case READ_DIG_INPUT_STATUS:							//! 功能码为2
					{
						//! 更新设备的DI点值
						nRet = pDevice->UpdateDI( MACSMB_DIB, pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal );
						//! 有变位时，输出报文
						if ( nRet > 0 )
						{
							std::string szSndPacket = GetCommData( m_pbySndBuffer, m_nSndLen, COMMDATA_SND);
							std::string szRcvPacket = GetCommData( pPacketBuffer, nPacketLength, COMMDATA_RCV);
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket There are %d DI changing Infos to the Request Frame:%s",nRet, szSndPacket.c_str());
							//}
							//else
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket 以上%d条变位信息来自报文,%s", nRet, szSndPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket The Response Frame:%s", szRcvPacket.c_str());
							//}
							//else
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket 收到请求:%s", szRcvPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
						}
					}
					break;
				case READ_ANA_INPUT_STATUS:							//! 功能码为4
					{
						//! 根据设备类型进行的特殊处理
						nRet = pDevice->BeforeUpdateInputAI( pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal );
						//! 如果处理出差且错误码是-999，则中止数据处理和发送流程
						if ( nRet == -999 )
						{
							return;
						}
						nRet = pDevice->UpdateInputAI( pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal);
						//! 有变位时，输出报文
						if ( nRet > 0 )
						{
							std::string szSndPacket = GetCommData( m_pbySndBuffer, m_nSndLen, COMMDATA_SND);
							std::string szRcvPacket = GetCommData( pPacketBuffer, nPacketLength, COMMDATA_RCV);
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket There are %d DI changing Infos to the Request Frame:%s",nRet, szSndPacket.c_str());
							//}
							//else
							//{
							//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket 以上%d条变位信息来自报文,%s", nRet, szSndPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket The Response Frame:%s", szRcvPacket.c_str());							}
							//else
							//{
							//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket 收到请求:%s", szRcvPacket.c_str() );
							//}
							pDriver->WriteLogAnyWay( pchLog );
						}
					}
					break;
				case READ_ANA_OUTPUT_STATUS:						//! 功能码为3
					{
						//! 根据设备类型进行的特殊处理
						nRet = pDevice->BeforeUpdateHoldingAI( pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal );
						//! 如果处理出差且错误码是-999，则中止数据处理和发送流程
						if ( nRet == -999 )
						{
							return;
						}

						//! 更新AI点
						nRet = pDevice->UpdateHoldingAI( pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal);
						//! 有变位时，输出报文
						if ( nRet > 0 )
						{
							std::string szSndPacket = GetCommData( m_pbySndBuffer, m_nSndLen, COMMDATA_SND);
							std::string szRcvPacket = GetCommData( pPacketBuffer, nPacketLength, COMMDATA_RCV);
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket There are %d DI changing Infos to the Request Frame:%s",nRet, szSndPacket.c_str());							
							//}
							//else
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket 以上%d条变位信息来自报文,%s", nRet, szSndPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog,MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket The Response Frame:%s", szRcvPacket.c_str());						
							//}
							//else
							//{
							//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket 收到请求:%s", szRcvPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
						}

					}
					break;
				case FORCE_DIG_SINGLE_OUTPUT_STATUS:				//! 功能码为5
				case FORCE_ANA_SINGLE_OUTPUT_STATUS:				//! 功能码为6
				case FORCE_DIG_MULTIPLE_OUTPUT_STATUS:				//! 功能码为15
				case FORCE_ANA_MULTIPLE_OUTPUT_STATUS:				//! 功能码为16
					{
						if ( nPacketLength < 8 ||
							MAKEWORD( pDevAddr[3],pDevAddr[2] ) != m_wdSendStartAddr 
							|| MAKEWORD( pDevAddr[5],pDevAddr[4] ) != m_wdSendPoints )
						{
							break;
						}

						//! 置返信点值为1
						CIOCfgUnitType* pUnitType = pDevice->GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIC);
						if ( pUnitType && pUnitType->GetTagCount() > 0 )
						{
							//置返信点为1
							byTemp = 1;
							pDevice->SetVal( MACSMB_DIC, 0, &byTemp, 1, 0, bFromLocal );
						}
					}
					break;
				case READ_GENERAL_REFERENCE:						//! 功能码为20
					{
						pDevice->UpdateFileInfo(pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal);
					}
					break;
				case WRITE_GENERAL_REFERENCE:						//! 功能码为21
					{
						//! 添加写文件返回帧处理
						//置返信点值为0
						CIOCfgUnitType* pUnitType = pDevice->GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIC);
						if ( pUnitType && pUnitType->GetTagCount() > 0 )
						{
							byTemp = 1;
							pDevice->SetVal( MACSMB_DIC, 0, &byTemp, 1, 0, bFromLocal );
						}
					}
					break;
					//! 异常帧处理
				case READ_DIG_OUTPUT_STATUS + 128:					//! 功能码为1
				case READ_DIG_INPUT_STATUS + 128:					//! 功能码为2
				case READ_ANA_INPUT_STATUS + 128:					//! 功能码为4
				case READ_ANA_OUTPUT_STATUS + 128:					//! 功能码为3
					{
						//! 添加数据帧异常处理
					}
					break;
				case FORCE_DIG_SINGLE_OUTPUT_STATUS + 128:			//! 功能码为5
				case FORCE_ANA_SINGLE_OUTPUT_STATUS + 128:			//! 功能码为6
				case FORCE_DIG_MULTIPLE_OUTPUT_STATUS + 128:		//! 功能码为15
				case FORCE_ANA_MULTIPLE_OUTPUT_STATUS + 128:		//! 功能码为16
				case WRITE_GENERAL_REFERENCE + 128:					//! 功能码为21
					{
						//置返信点值为0
						CIOCfgUnitType* pUnitType = pDevice->GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIC);
						if ( pUnitType && pUnitType->GetTagCount() > 0 )
						{
							byTemp = 0;
							pDevice->SetVal( MACSMB_DIC, 0, &byTemp, 1, 0, bFromLocal );
						}
					}
					break;
				default:
					//! 转给特殊帧处理函数处理
					ProcessRcvdSplPacket(pDevAddr, nRTULen, bFromLocal);
					break;
				}
			}
		}

	}

	/*!
	*	\brief	处理特殊的数据包（已经转化为RTU格式的数据包）
	*
	*	\param	pPacketBuffer	数据包
	*	\param	nPacketLength	数据包长度
	*	\param	bFromLocal		区分是本机来的还是另机来的
	*
	*	\retval	void
	*/		
	void CFepModbusPrtcl::ProcessRcvdSplPacket(	Byte* pPacketBuffer, int nPacketLength, bool bFromLocal )
	{
		return;
	}

	/*!
	*	\brief	计算16位数据校验和--CRC
	*
	*	1、	置一16位寄存器位全1；
	*	2、	将报文数据的高位字节异或寄存器的低八位，存入寄存器；
	*	3、	右移寄存器，最高位置0，移出的低八位存入标志位；
	*	4、	如标志位是1，则用A001异或寄存器；如标志位是0，继续步骤3；
	*	5、	重复步骤3和4，直至移位八位；
	*	6、	异或下一位字节与寄存器；
	*	7、	重复步骤3至5，直至所有报文数据均与寄存器异或并移位8次；
	*	8、	此时寄存器中即为CRC校验码，最低位先发送；
	*
	*	\param	datapt		数据起始位置
	*	\param	bytecount	字节个数
	*
	*	\retval	WORD	16位的数据校验和
	*/
	WORD CFepModbusPrtcl::CRC(const Byte * datapt,int bytecount)
	{
		if (!datapt)
			return 0;
		WORD Reg16=0xFFFF,mval=0xA001;  //0xA001=1010000000000001
		int   i;
		Byte  j,flg; 

		for(i=0;i<bytecount;i++)
		{
			Reg16^=*(datapt+i);
			for(j=0;j<8;j++)
			{
				flg=0;
				flg=Reg16&0x0001;  //b0
				Reg16>>=1;         //右移一位，置b15=0
				if(flg==1)
					Reg16=Reg16^mval;
			}
		}
		return Reg16;
	}

	/*!
	*	\brief	根据设备地址获取设备索引号
	*
	*	\param	Addr	设备地址
	*	\param	DevNo	设备地址对应的设备索引号
	*
	*	\retval	bool	
	*/
	bool CFepModbusPrtcl::GetDeviceNoByAddr( int Addr, int& DevNo )
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		CIODeviceBase* pDevice = NULL;
		for( int i=0; i< pDriver->GetDeviceCount(); i++ )
		{
			pDevice = pDriver->GetDeviceByIndex(i);
			if( pDevice && pDevice->GetAddr() == Addr )
			{
				DevNo = i;
				return true;
			}
		}

		return false;
	}

	/*!
	*	\brief	组遥控遥调帧
	*
	*	\param	pbyData		帧首地址
	*	\param	nDataLen	帧长
	*	\param	nTimeOut	超时时间
	*	\param	fCmdVal		发送的命令值
	*
	*	\retval	int	0表示成功，否则表示失败
	*/
	int CFepModbusPrtcl::BuildCtrlFrame( 
		Byte* pbyData, int& nDataLen, int& nTimeOut, float& fCmdVal )
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		nDataLen = 0;

		int nResult = 0;
		Byte pchBuf[1124] = {0};
		int nBufLen = 1124;		
		nResult = pDriver->m_IORemoteCtrl.DeQueue( (char*)pchBuf, nBufLen );
		if ( nResult==0 )
		{
			memcpy( (Byte*)(&m_tCtrlRemoteCmd), pchBuf, sizeof(m_tCtrlRemoteCmd) );

			//取得设备地址
			CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByName(m_tCtrlRemoteCmd.pchUnitName));

			if(!pDevice)
			{
				return -4;
			}

			//! 发送延时
			ACE_Time_Value tv( 0, (pDevice->GetPortCfg().nSndDelay)*1000 );
			ACE_OS::sleep( tv );
			
			//! 如果设备地址配置错误，则不发送控制帧
			if( pDevice->GetAddr() <= 0 || pDevice->GetAddr() > 255 )
			{
				if (pDriver->IsLog())
				{
					char pchLog[MAXDEBUGARRAYLEN];
					//if (pDriver->IsEnglishEnv())
					//{
					//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildCtrlFrame Dev:%s the address:%d is not right!", m_tCtrlRemoteCmd.pchUnitName, pDevice->GetAddr());
					//}
					//else
					//{
					//	snprintf( pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildCtrlFrame 设备:%s 地址:%d 配置错误!", m_tCtrlRemoteCmd.pchUnitName, pDevice->GetAddr());
					//}
					//pDriver->WriteLog( pchLog );
					return -5;
				}
			}

			//! 如果设备离线，则不发送控制帧
			if ( pDevice->IsOffLine() )
			{
				return 0;
			}

			//根据UnitType不同组发送帧,置长度
			switch( m_tCtrlRemoteCmd.byUnitType )
			{
			case MACSMB_DOA:	// DO 0X
			case MACSMB_DOB:	// AO 4X
			case MACSMB_DOC:
			case MACSMB_DOD:
			case MACSMB_DOF:
			case MACSMB_DOG:
				{
					Byte byVal;
					int nLen = __min( m_tCtrlRemoteCmd.nDataLen, 1 );
					memcpy( &byVal, m_tCtrlRemoteCmd.byData, nLen );

					//! DO 0X 功能码5
					if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOA )
						nDataLen = pDevice->BuildDOAReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);
					//! AO 4X 功能码6
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOB )
						nDataLen = pDevice->BuildDOBReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);
					//! DO 0X 功能码15
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOC )
						nDataLen = pDevice->BuildDOCReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);
					//! AO 4X 功能码16
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOD )
						nDataLen = pDevice->BuildDODReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);
					//! AO 4X 功能码6
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOF )
						nDataLen = pDevice->BuildDOFReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);
					//! AO 4X 功能码16
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOG )
						nDataLen = pDevice->BuildDOGReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);

					fCmdVal = (float)byVal;
					m_bycmdType = 1;

					//! 构造遥控伴随帧备用
					BuildCtrlFwReq(pDevice,m_tCtrlRemoteCmd.lTagAddress);
				}
				break;			
			case MACSMB_DOE:	
				break;
			case MACSMB_AOA:	//! AO 4X 功能码6
			case MACSMB_AOB:	//! AO 4X 功能码16
			case MACSMB_AOD:	//! AO 4X 功能码6，单个寄存器中N位
			case MACSMB_AOE:	//! AO 4X 功能码16，单个寄存器中N位
			case MACSMB_AOF:	//! AO 4X 功能码6，单个寄存器中N位，其余位自动为0
			case MACSMB_AOG:	//! AO 4X 功能码16，单个寄存器中N位，其余位自动为0
				{
					float fVal = 0;
					int nLen = __min( m_tCtrlRemoteCmd.nDataLen, sizeof(double) );
					if (nLen > sizeof(float))
					{
						if (sizeof(double) == nLen)
						{
							double dVal = (*(double*)m_tCtrlRemoteCmd.byData);
							fVal = (float)dVal;
						}
					}
					else
					{
						memcpy( &fVal, m_tCtrlRemoteCmd.byData, nLen );
					}

					//! AO 4X 功能码6
					if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOA )
						nDataLen = pDevice->BuildAOAReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );
					//! AO 4X 功能码16
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOB )
						nDataLen = pDevice->BuildAOBReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );
					//! AO 4X 功能码6，单个寄存器中N位
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOD )
						nDataLen = pDevice->BuildAODReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );
					//! AO 4X 功能码16，单个寄存器中N位
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOE )
						nDataLen = pDevice->BuildAOEReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );
					//! AO 4X 功能码16，单个寄存器中N位，其余位自动为0
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOF )
						nDataLen = pDevice->BuildAOFReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );
					//! AO 4X 功能码16，单个寄存器中N位，其余位自动为0
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOG )
						nDataLen = pDevice->BuildAOGReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );

					fCmdVal = fVal;
					m_bycmdType = 2;

					//! 生成返信请求
					BuildCtrlFwReq( pDevice, m_tCtrlRemoteCmd.lTagAddress );
				}
				break;
			case MACSMB_AOC:		//!< 字符串类型遥调
			case MACSMB_MOA:		//!< 数据区下发命令
				{
					nDataLen = pDevice->BuildAOCReqFrame(
						pbyData, m_tCtrlRemoteCmd.lTagAddress,
						m_tCtrlRemoteCmd.byData, m_tCtrlRemoteCmd.nDataLen );
					fCmdVal = 0;
					m_bycmdType = 2;
				}
				break;
			case MACSMB_AOT:
				{
					if ( m_tCtrlRemoteCmd.nDataLen < sizeof(float) )
					{
						return 0;
					}
					Byte pbyTemp[4];
					pbyTemp[0] = m_tCtrlRemoteCmd.byData[3];
					pbyTemp[1] = m_tCtrlRemoteCmd.byData[2];
					pbyTemp[2] = m_tCtrlRemoteCmd.byData[1];
					pbyTemp[3] = m_tCtrlRemoteCmd.byData[0];
					nDataLen = pDevice->BuildCtrlMultiRegsFrame(
						pbyData, m_tCtrlRemoteCmd.lTagAddress - 1,
						sizeof(float)/2, pbyTemp  );
					fCmdVal = 0;
					m_bycmdType = 2;
				}
				break;
			case MACSMB_AOP:
				{
					if ( m_tCtrlRemoteCmd.nDataLen < sizeof(DWORD) )
					{
						return 0;
					}
					Byte pbyTemp[4];
					pbyTemp[0] = m_tCtrlRemoteCmd.byData[3];
					pbyTemp[1] = m_tCtrlRemoteCmd.byData[2];
					pbyTemp[2] = m_tCtrlRemoteCmd.byData[1];
					pbyTemp[3] = m_tCtrlRemoteCmd.byData[0];
					nDataLen = pDevice->BuildCtrlMultiRegsFrame(
						pbyData, m_tCtrlRemoteCmd.lTagAddress - 1,
						sizeof(DWORD)/2, pbyTemp  );
					fCmdVal = 0;
					m_bycmdType = 2;
				}
				break;
			case MACSMB_DOM:
				{
					Byte byVal;
					int nLen = __min( m_tCtrlRemoteCmd.nDataLen, 1 );
					memcpy( &byVal, m_tCtrlRemoteCmd.byData, nLen );
					nDataLen = pDevice->BuildDOMReqFrame(
						pbyData, m_tCtrlRemoteCmd.lTagAddress,
						byVal, m_tCtrlRemoteCmd.nDataLen );
					fCmdVal = 0;
					m_bycmdType = 2;
				}
				break;
			case MACSMB_READSETTINGGROUP:
				{					
					Byte byVal;
					int nLen = __min( m_tCtrlRemoteCmd.nDataLen, 1 );
					memcpy( &byVal, m_tCtrlRemoteCmd.byData, nLen );

					char chTagName[50]="";
					sprintf(chTagName,"RDSETTINGGP%d",m_tCtrlRemoteCmd.lTagAddress);
					int iTemp=0;					
					for ( int i=0;i< pDevice->GetSETTINGGROUP();i++)
					{
						std::string strTemp = pDevice->vecSettingGroup[i].strDoTagName;
						iTemp = stricmp(chTagName,strTemp.c_str());
						if( iTemp == 0 )
						{
							if (byVal == 0)
							{
								pDevice->SetVal(MACSMB_READSETTINGGROUP,i,&byVal,1,0,true);
								pDevice->vecSettingGroup[i].iReadCount = 0;
							}
							else if (byVal == 1)
							{
								pDevice->vecSettingGroup[i].iReadCount = pDevice->vecSettingGroup[i].iReadCountCfg;
								pDevice->SetVal(MACSMB_READSETTINGGROUP,i+1,&byVal,1,0,true);
							}
							break;
						}						
					}
				}
				break;
			case MACSMB_READSETTINGGROUPALL:
				//!所有的处罚读取定值组遥控点置位，并设置读取次数
				{
					Byte byVal;
					int nLen = __min( m_tCtrlRemoteCmd.nDataLen, 1 );
					memcpy( &byVal, m_tCtrlRemoteCmd.byData, nLen );

					if ( byVal == 1 )
					{
						int ia=0;
						for (int i=0;i<pDevice->vecSettingGroup.size();i++)
						{
							ia = pDevice->vecSettingGroup[i].iReadCountCfg;	
							pDevice->vecSettingGroup[i].iReadCount = ia; 
							pDevice->SetVal(MACSMB_READSETTINGGROUP,i+1,&byVal,1,0,true);
						}					
					}
					else
					{
						for (int i=0;i<pDevice->vecSettingGroup.size();i++)
						{
							pDevice->vecSettingGroup[i].iReadCount = 0;	
							pDevice->SetVal(MACSMB_READSETTINGGROUP,i+1,&byVal,1,0,true);
						}
					}
					pDevice->SetVal(MACSMB_READSETTINGGROUPALL,1,&byVal,1,0,true);
				}
				break;
			default:
				nDataLen = pDevice->BuildSplCtrlFrame(pbyData,m_tCtrlRemoteCmd,fCmdVal);
				break;
			}


			if ( nDataLen > 0 )
			{
				//置返信点值为初始态。
				CIOCfgUnitType* pUnitType = pDevice->GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIC);
				if ( pUnitType && pUnitType->GetTagCount() > 0 )
				{
					Byte byTemp = 0;
					pDevice->SetVal( MACSMB_DIC, 0, &byTemp, 1, 0, true );	//置返信点为0
				}


				//! 强制写日志
				if ( nDataLen > 0 )
				{
					std::string szData = GetCommData( pbyData, nDataLen, COMMDATA_SND);
					char pchLog[MAXDEBUGARRAYLEN];
					//if (pDriver->IsEnglishEnv())
					//{
					//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::BuildCtrlFrame Device:%s,UnitType=%d,Address=%lld,Frame:%s", 
					//		pDevice->GetName().c_str(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, szData.c_str());
					//}
					//else
					//{
					//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::BuildCtrlFrame 组控制报文,设备名称=%s,UnitType=%d,标签地址=%lld,Content: %s",
					//		pDevice->GetName().c_str(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, szData.c_str());
					//}
					//pDriver->WriteLogAnyWay( pchLog );
				}

				//! 置超时时间
				nTimeOut = pDevice->GetPortCfg().nTimeOut;
			}			
		}

		return 0;
	}

	/*!
	*	\brief	分析遥控返信包
	*
	*	检查以pFrame开始的数据缓存区中是否有完整的遥控返信包
	*
	*	\param	pFrame		帧头指针
	*	\param	nCorrectLen	如有解析出正确的遥控返信包，则置该参数为正确帧的长度
	*
	*	\retval	int			含义如下：
	*				FLAG_NONE				0		不是遥控返信帧
	*				FLAG_UNCOMPLETE			1		是遥控返信帧，但没接收完整
	*				FLAG_CORRECTFRAME		2		是正确的遥控返信帧
	*				FLAG_ERROR				3		解析出错，说明给的帧头位置不对
	*/
	int CFepModbusPrtcl::ExtractCtrlPacket( const Byte* pFrame, int &nCorrectLen )
	{
		int nFlag = FLAG_NONE;

		//! 如果是遥控、遥调反馈帧
		if(	   ( m_byActiveFunc&0x7F ) == FORCE_DIG_SINGLE_OUTPUT_STATUS 
			|| ( m_byActiveFunc&0x7F ) == FORCE_ANA_SINGLE_OUTPUT_STATUS
			|| ( m_byActiveFunc&0x7F ) == FORCE_DIG_MULTIPLE_OUTPUT_STATUS
			|| ( m_byActiveFunc&0x7F ) == FORCE_ANA_MULTIPLE_OUTPUT_STATUS
			)
		{
			if( IsModbusTCP() )			//! TCP Modbus
			{
				FRAME_HEAD* pHead = (FRAME_HEAD*)(pFrame);

				//! 验证帧头中记录的长度
				WORD wdDataLen = MAKEWORD(pHead->byDataLenLow, pHead->byDataLenHigh);
				if (( m_byActiveFunc&0x80 ) == 0x80 && wdDataLen != 3)
				{
					return FLAG_ERROR;				//! 校验出错
				}
				if (( m_byActiveFunc&0x80 ) == 0x00 && wdDataLen != 6)
				{
					return FLAG_ERROR;				//! 校验出错
				}

				//! 正确的帧长
				nCorrectLen = wdDataLen + sizeof(FRAME_HEAD) - 1;

				//! 没接收完整
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}


				//! 校验设备地址
				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));
				if ( !pDevice )
				{
					return FLAG_ERROR;				//! 校验出错
				}
			}
			else						//! RTU Modbus
			{
				if ( ( m_byActiveFunc&0x80 ) == 0x80 )
				{
					nCorrectLen = 5;
				}
				else
				{
					nCorrectLen = 8;
				}				

				//! 没接收完整
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}

				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));
				if ( !pDevice )
				{
					return FLAG_ERROR;				//! 校验出错
				}

				//! 校验CRC
				WORD CheckSum = CRC( pFrame, nCorrectLen - 2 );
				if ( (( pDevice->GetPortCfg().nCRCOrder == CRC_HOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != HIBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != LOBYTE(CheckSum) ) )
					|| (( pDevice->GetPortCfg().nCRCOrder == CRC_LOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != LOBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != HIBYTE(CheckSum) ) ) )
				{
					return FLAG_ERROR;				//! 校验出错
				}
			}

			return FLAG_CORRECTFRAME;			//! 接收到完整的遥控返信帧
		}

		return nFlag;
	}

	/*!
	*	\brief	分析轮询帧返回包
	*
	*	检查以pFrame开始的数据缓存区中是否有完整的轮询帧返回包
	*
	*	\param	pFrame		帧头指针
	*	\param	nCorrectLen	如有解析出正确的遥控返信包，则置该参数为正确帧的长度
	*
	*	\retval	int			含义如下：
	*				FLAG_NONE				0		不是轮询帧返回包
	*				FLAG_UNCOMPLETE			1		是轮询帧返回包，但没接收完整
	*				FLAG_CORRECTFRAME		2		是正确的轮询帧返回包
	*				FLAG_ERROR				3		解析出错，说明给的帧头位置不对
	*/
	int CFepModbusPrtcl::ExtractMsgPacket( const Byte* pFrame, int &nCorrectLen )
	{
		int nFlag = FLAG_NONE;

		//! 如果是轮询帧返回包
		if(	   ( m_byActiveFunc&0x7F ) ==  READ_DIG_OUTPUT_STATUS
			|| ( m_byActiveFunc&0x7F ) == READ_DIG_INPUT_STATUS
			|| ( m_byActiveFunc&0x7F ) == READ_ANA_OUTPUT_STATUS
			|| ( m_byActiveFunc&0x7F ) == READ_ANA_INPUT_STATUS
			)
		{
			if( IsModbusTCP() )			//! TCP Modbus
			{
				WORD wdValiteDataLen;  //! 包长
				FRAME_HEAD* pHead = (FRAME_HEAD*)(pFrame);

				//! 验证帧头中记录的数据长度
				WORD wdDataLen = MAKEWORD( pHead->byDataLenLow, pHead->byDataLenHigh );
				WORD wdPacketNo = MAKEWORD( pHead->byPckNoLow, pHead->byPckNoHigh );
				if (( m_byActiveFunc&0x80 ) == 0x80 && wdDataLen != 3)
				{
					return FLAG_ERROR;				//! 校验出错
				}
				if (( m_byActiveFunc&0x80 ) == 0x00 )
				{
					//! 计算数据包长					
					if(	m_byActiveFunc ==  READ_DIG_OUTPUT_STATUS
						|| m_byActiveFunc == READ_DIG_INPUT_STATUS )
					{			
						wdValiteDataLen = (m_pSendData[wdPacketNo].wdSendPoints + 7)/8 + 3;
					}
					else
					{
						wdValiteDataLen = m_pSendData[wdPacketNo].wdSendPoints * 2 + 3;
					}

					if( wdDataLen != wdValiteDataLen)
						return FLAG_ERROR;				//! 校验出错
				}

				//! 正确的帧长
				nCorrectLen = wdDataLen + sizeof(FRAME_HEAD) - 1;

				//! 没接收完整
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}

				////! 校验字节个数
				//if (( m_byActiveFunc&0x80 ) == 0x00 && pFrame[sizeof(FRAME_HEAD)+1] != 0)
				//{
				//	if ( pFrame[sizeof(FRAME_HEAD)+1] != wdValiteDataLen - 3 )
				//	{
				//		return FLAG_ERROR;				//! 校验出错
				//	}
				//}	

				//! 校验设备地址
				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));

				if ( !pDevice )
				{
					return FLAG_ERROR;				//! 校验出错
				}
			}
			else						//! RTU Modbus
			{
				if ( ( m_byActiveFunc&0x80 ) == 0x80 )
				{
					nCorrectLen = 5;
				}
				else
				{
					//! 校验字节个数；此处要先校验字节个数，字节个数不对则直接丢弃，避免帧错误且长度值很大时一直在等待接收
					if(	  (m_byActiveFunc ==  READ_DIG_OUTPUT_STATUS
						|| m_byActiveFunc == READ_DIG_INPUT_STATUS)
						&& pFrame[sizeof(FRAME_HEAD)+1] != 0 )
					{				
						if ( pFrame[2] != ( m_wdSendPoints + 7)/8 )
						{
							return FLAG_ERROR;				//! 校验出错
						}
					}
					else if( m_byActiveFunc == READ_ANA_OUTPUT_STATUS
						|| m_byActiveFunc == READ_ANA_INPUT_STATUS)
					{
						if ( pFrame[2] != m_wdSendPoints * 2 )
						{
							return FLAG_ERROR;				//! 校验出错
						}
					}
					nCorrectLen = 3 + pFrame[2] + 2;
				}

				//! 没接收完整
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}

				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));

				if ( !pDevice )
				{
					return FLAG_ERROR;				//! 校验出错
				}				

				//! 校验CRC
				WORD CheckSum = CRC( pFrame, nCorrectLen - 2 );
				if ( ( ( pDevice->GetPortCfg().nCRCOrder == CRC_HOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != HIBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != LOBYTE(CheckSum) ) )
					|| (( pDevice->GetPortCfg().nCRCOrder == CRC_LOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != LOBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != HIBYTE(CheckSum) ) )
					)
				{
					return FLAG_ERROR;				//! 校验出错
				}
			}

			return FLAG_CORRECTFRAME;	//! 接收到完整且正确的轮询帧返回包
		}

		return nFlag;
	}

	/*!
	*	\brief	解析文件操作返信包
	*
	*	检查以pFrame开始的数据缓存区中是否有完整的文件操作返信包
	*
	*	\param	pFrame		帧头指针
	*	\param	nCorrectLen	如有解析出正确的遥控返信包，
	*						则置该参数为正确帧的长度
	*
	*	\retval	int			含义如下：
	*		FLAG_NONE				0		不是遥控返信帧
	*		FLAG_UNCOMPLETE			1		是遥控返信帧，但没接收完整
	*		FLAG_CORRECTFRAME		2		是正确的遥控返信帧
	*		FLAG_ERROR				3		解析出错，说明给的帧头位置不对
	*/
	int CFepModbusPrtcl::ExtractFilePacket( const Byte* pFrame, int &nCorrectLen )
	{
		int nFlag = FLAG_NONE;
		//! 如果是轮询帧返回包
		if(	   ( m_byActiveFunc&0x7F ) ==  READ_GENERAL_REFERENCE
			|| ( m_byActiveFunc&0x7F ) == WRITE_GENERAL_REFERENCE
			)
		{
			if( IsModbusTCP() )			//! TCP Modbus
			{				
				WORD wdValiteDataLen;  //! 包长
				FRAME_HEAD* pHead = (FRAME_HEAD*)(pFrame);

				//! 验证帧头中记录的数据长度
				WORD wdDataLen = MAKEWORD( pHead->byDataLenLow, pHead->byDataLenHigh );
				WORD wdPacketNo = MAKEWORD( pHead->byPckNoLow, pHead->byPckNoHigh );
				if (( m_byActiveFunc&0x80 ) == 0x80 && wdDataLen != 3)
				{
					return FLAG_ERROR;				//! 校验出错
				}
				if (( m_byActiveFunc&0x80 ) == 0x00 )
				{
					//! 计算数据包长					
					if(	m_byActiveFunc == READ_GENERAL_REFERENCE )
					{			
						wdValiteDataLen = m_pSendData[wdPacketNo].wdSendPoints * 2 + 5;
					}
					else
					{
						wdValiteDataLen = m_pSendData[wdPacketNo].wdSendPoints * 2 + 10;
					}

					if( wdDataLen != wdValiteDataLen)
						return FLAG_ERROR;				//! 校验出错
				}

				//! 正确的帧长
				nCorrectLen = wdDataLen + sizeof(FRAME_HEAD) - 1;

				//! 没接收完整
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}

				if (( m_byActiveFunc&0x80 ) == 0x00 )
				{
					//! 校验字节个数
					if ( pFrame[sizeof(FRAME_HEAD)+1] != wdValiteDataLen - 3 )
					{
						return FLAG_ERROR;				//! 校验出错
					}
				}

				if(	m_byActiveFunc == READ_GENERAL_REFERENCE )
				{
					//! 校验文件响应长度（单个文件记录）
					if ( pFrame[sizeof(FRAME_HEAD)+2] != wdValiteDataLen - 4 )
					{
						return FLAG_ERROR;				//! 校验出错
					}

					//! 校验参考类型
					if ( pFrame[sizeof(FRAME_HEAD)+3] != 6 )
					{
						return FLAG_ERROR;				//! 校验出错
					}
				}
				else if( m_byActiveFunc == WRITE_GENERAL_REFERENCE )
				{
					//! 校验参考类型
					if ( pFrame[sizeof(FRAME_HEAD)+2] != 6 )
					{
						return FLAG_ERROR;				//! 校验出错
					}
				}

				//! 校验设备地址
				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));

				if ( !pDevice )
				{
					return FLAG_ERROR;				//! 校验出错
				}				
			}
			else						//! RTU Modbus
			{
				if ( ( m_byActiveFunc&0x80 ) == 0x80 )
				{
					nCorrectLen = 5;
				}
				else
				{
					//! 校验字节个数
					if(	   m_byActiveFunc ==  READ_GENERAL_REFERENCE )
					{				
						if ( pFrame[2] != ( m_wdSendPoints * 2 + 2 ) )
						{
							return FLAG_ERROR;				//! 校验出错
						}

						//! 校验文件响应长度（单个文件记录）
						if ( pFrame[3] != ( m_wdSendPoints * 2 + 1 ) )
						{
							return FLAG_ERROR;				//! 校验出错
						}

						//! 校验参考类型
						if ( pFrame[4] != 6 )
						{
							return FLAG_ERROR;				//! 校验出错
						}
					}
					else if( m_byActiveFunc == WRITE_GENERAL_REFERENCE )
					{
						if ( pFrame[2] != ( m_wdSendPoints * 2 + 7 ) )
						{
							return FLAG_ERROR;				//! 校验出错
						}

						//! 校验参考类型
						if ( pFrame[3] != 6 )
						{
							return FLAG_ERROR;				//! 校验出错
						}
					}

					nCorrectLen = 3 + pFrame[2] + 2;
				}

				//! 没接收完整
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}

				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));

				if ( !pDevice )
				{
					return FLAG_ERROR;				//! 校验出错
				}				

				//! 校验CRC
				WORD CheckSum = CRC( pFrame, nCorrectLen - 2 );
				if ( ( ( pDevice->GetPortCfg().nCRCOrder == CRC_HOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != HIBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != LOBYTE(CheckSum) ) )
					|| (( pDevice->GetPortCfg().nCRCOrder == CRC_LOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != LOBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != HIBYTE(CheckSum) ) )
					)
				{
					return FLAG_ERROR;				//! 校验出错
				}
			}

			return FLAG_CORRECTFRAME;	//! 接收到完整且正确的轮询帧返回包
		}

		return nFlag;
	}

	/*!
	*	\brief	解析其他特殊的返信包
	*
	*	检查以pFrame开始的数据缓存区中是否有完整的其他特殊的返信包
	*
	*	\param	pFrame		帧头指针
	*	\param	nCorrectLen	如有解析出正确的遥控返信包，
	*						则置该参数为正确帧的长度
	*
	*	\retval	int			含义如下：
	*		FLAG_NONE				0		不是遥控返信帧
	*		FLAG_UNCOMPLETE			1		是遥控返信帧，但没接收完整
	*		FLAG_CORRECTFRAME		2		是正确的遥控返信帧
	*		FLAG_ERROR				3		解析出错，说明给的帧头位置不对
	*/
	int CFepModbusPrtcl::ExtractOtherPacket( const Byte* pFrame, int &nCorrectLen )
	{
		return FLAG_NONE;
	}

	/*!
	*	\brief	组遥控伴随帧
	*
	*	\param	pDevice		设备
	*	\param	lTagAddr	遥控标签地址
	*
	*	\retval	int	0表示成功，否则表示失败
	*/
	int CFepModbusPrtcl::BuildCtrlFwReq( CFepModbusDevice* pDevice, int lTagAddr )
	{
		if(!pDevice)
		{
			return -1;
		}

		//! 获得伴随帧
		struCtrlFollowReq* pFwReq = pDevice->GetCtrlFwReqByTagAddr(lTagAddr);

		if(pFwReq)
		{
			m_nCtrlInsertTempLen = sizeof(pFwReq->Req);
			memcpy( m_pbyCtrlInsertBuf, &pFwReq->Req, m_nCtrlInsertTempLen );

			//m_pInsertDelayTimer->SetTimer( pFwReq->msDelay );
		}
		return 0;
	}

	/*!
	*	\brief	生成事务单元标识
	*
	*	\param	High	标识高位
	*	\param	Low		标识低位
	*
	*	\retval	int	Modbus TCP请求大小
	*/
	void CFepModbusPrtcl::GetPacketNo( Byte& High, Byte& Low )
	{
		High = HIBYTE( m_wdPacketNo );
		Low  = LOBYTE( m_wdPacketNo );

		m_wdPacketNo = ( m_wdPacketNo + 1 ) % 65536;

		return;
	}

	/*!
	*	\brief	把Modbus RTU请求转化成Modbus TCP请求
	*
	*	\param	pRTU		RTU请求
	*	\param	iRTULen		RTU请求大小
	*	\param	pTCP		TCP请求
	*
	*	\retval	int	Modbus TCP请求大小
	*/
	int CFepModbusPrtcl::MBRTU2MBTCPRequest(Byte* pRTU,int nRTULen,Byte* pTCP)
	{
		if( pRTU == NULL || nRTULen <= 3 || nRTULen > 252 || pTCP == NULL )
		{
			return 0;
		}

		//! 得到MB TCP报文头
		WORD wdDataLen = nRTULen - 2;
		FRAME_HEAD pHead;
		GetPacketNo( pHead.byPckNoHigh, pHead.byPckNoLow );
		pHead.byPrtclHigh	= 0x00;
		pHead.byPrtclLow	= 0x00;
		pHead.byDataLenHigh	= HIBYTE(wdDataLen);
		pHead.byDataLenLow	= LOBYTE(wdDataLen);
		pHead.bySlaveAddress	= pRTU[0];

		//! 生成MB TCP报文包体
		Byte* pBody = pTCP + sizeof(FRAME_HEAD);
		memmove( pBody, pRTU+1, nRTULen-3 );

		//! 复制MB TCP报文头
		memcpy( pTCP, &pHead, sizeof(FRAME_HEAD) );

		return nRTULen - 3 + sizeof(FRAME_HEAD);
	}

	//! 连接成功
	int CFepModbusPrtcl::OnConnSuccess(std::string pPeerIP )
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		//! 连接锁
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard( pDriver->m_Mutex );

		//! 关闭连接超时定时器
		//pDriver->GetConnTimer()->KillTimer();
		pDriver->SetConnTime( 0 );
		pDriver->SetConnected( true );

		pDriver->m_nLinkOfflineConnectCount = 0;

		char pchLog[MAXDEBUGARRAYLEN] = {0};
		//if (pDriver->IsEnglishEnv())
		//{
		//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::OnConnSuccess IP:%s connect successful!", pPeerIP.c_str());
		//}
		//else
		//{
		//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::OnConnSuccess IP:%s 连接成功!", pPeerIP.c_str());
		//}
		//! 记录日志
		pDriver->WriteLogAnyWay( pchLog );

		//! 清除接收缓冲区和滑动窗口中的数据
		m_nRevLength = 0;
		m_iInnerLen = 0;

		CFepModbusDevice* pDevice = NULL;
		bool bBroadcastFlag = false;		//!是否是广播校时(用于855协议设备) Add by: Wangyanchao
		for ( int i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepModbusDevice*>( pDriver->GetDeviceByIndex(i) );
			if ( pDevice )
			{
				//连接成后，复归收发计数器。避免重连成功后再错误的连接。注意：此处不能置设备在线。
				pDevice->m_byTxTimes = 0;
				pDevice->m_byRxTimes = 0;

				pDevice->SetTrySendCount( 0 );
				if ( pDevice->HasCheckTime() && (pDevice->GetDevType() == DEVTYPE_855) )  //! 855设备重连校时 Add by: Wangyanchao
				{
					//! 使用广播帧校时，对同类型的所有设备，每轮只需发送一帧而不需要对每个设备都发

					if ( pDevice->IsBroadChckTime() )	
					{
						if ( !bBroadcastFlag )
						{
							pDevice->SetNeedCheckTime(true);	//置校时标志
							bBroadcastFlag = true;
						}
					}
					else	//点对点对时，每个设备都置标志
					{
						pDevice->SetNeedCheckTime(true);	//置校时标志
					}
				}
			}
		}
		//! 事务号归零
		m_wdPacketNo = 0;
		m_nCtrlSendBufferLen = 0;

		/////////////////////////////////////////////////////////
		//! 发送数据
		int nDataLen = 0;
		BuildRequest( m_pbySndBuffer, nDataLen );
		/////////////////////////////////////////////////////////
		return 0;
	}	

	/*!
	*	\brief	记录上次发送帧中的重要数据
	*
	*	\retval	void
	*/
	void CFepModbusPrtcl::BackupLastRequest()
	{
		if ( m_pbySndBuffer[1] == READ_GENERAL_REFERENCE
			|| m_pbySndBuffer[1] == WRITE_GENERAL_REFERENCE )		//! 文件操作请求
		{		
			m_bySendAddr		= m_pbySndBuffer[0];
			m_bySendFunc		= m_pbySndBuffer[1];
			m_wdSendStartAddr	= MAKEWORD( m_pbySndBuffer[7], m_pbySndBuffer[6] );
			m_wdSendPoints		= MAKEWORD( m_pbySndBuffer[9], m_pbySndBuffer[8] );
		}
		else
		{
			m_bySendAddr = m_pbySndBuffer[0];
			m_bySendFunc = m_pbySndBuffer[1];
			m_wdSendStartAddr = MAKEWORD( m_pbySndBuffer[3], m_pbySndBuffer[2] );
			m_wdSendPoints = MAKEWORD( m_pbySndBuffer[5], m_pbySndBuffer[4] );
		}
	}

	std::string CFepModbusPrtcl::GetCommData( Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd )
	{
		std::string szData = "";
		if (COMMDATA_RCV == byRcvOrSnd)
		{
			szData = "RCV: ";
		}
		else if(COMMDATA_SND == byRcvOrSnd)
		{
			szData = "SND: ";
		}

		char pchByte[4];
		//! 获取数据显示格式
		for ( int i=0; i < nDataLen; i++ )
		{
			sprintf( pchByte, "%02X ", pbyBuf[i] );
			szData += pchByte;
		}

		return szData;
	}

	int CFepModbusPrtcl::RxDataProc( Byte* pDataBuf, int& nLen )
	{
		//! 通道如果被禁止了，则直接返回0
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		if ( !pDriver || !m_pbyInnerBuf || !m_pbyRevBuffer || !m_sPacketBuffer )
			return -1;		
		if(  pDriver->IsDisable() )
		{
			return 0;
		}

		if ( !pDriver->CanWork() )
		{
			return 0;
		}
		char pchLog[MAXDEBUGARRAYLEN] = {0};

		//! 如果需要通信监视，则发送原始数据包
		if ( pDriver->IsMonitor() )
		{
			pDriver->WriteCommData( pDataBuf, nLen );
		}

		//把数据放到内部缓冲区
		InnerAddData( pDataBuf, nLen );
		//if (pDriver->IsLog())
		//{
		//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::RxDataProc Recieve data Len:%d", nLen);
		//	pDriver->WriteLogAnyWay(pchLog);
		//}

		//从内部缓冲区中执行滑动窗口，找数据包，注意一次可能会处理多个数据包
		int	nReceived = 0;
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
					int nPacketLen = 0;

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
							bCompleted = true;

							//Modbus TCP协议允许批量接收处理，因为协议中有“包序号”的缘故
							ProcessRcvdPacket( m_sPacketBuffer, nPacketLen, true );	
						}
					}while(bTemp);

					//找到数据包后处理掉
					if ( bCompleted )
					{
						//! 无论何种协议，都是批量解析完，解析成功后再发送一次请求
						int nDataLen = 0;
						BuildRequest( m_pbySndBuffer, nDataLen );

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

	int CFepModbusPrtcl::StopTimer()
	{
		int nRes = 0;
		//if ( m_pIOTimer )
		//{
		//	nRes = m_pIOTimer->KillTimer();
		//}

		//if ( m_pPeriodTimer )
		//{
		//	nRes = m_pPeriodTimer->KillTimer();
		//}

		//if ( m_pInsertDelayTimer )
		//{
		//	nRes = m_pInsertDelayTimer->KillTimer();
		//}
		return nRes;
	}
}

#endif
