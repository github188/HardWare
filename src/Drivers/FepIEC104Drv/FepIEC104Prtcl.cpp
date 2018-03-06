/*!
 *	\file	FepIEC104Prtcl.cpp
 *
 *	\brief	104主站协议类实现源文件
 *
 *	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Prtcl.cpp, 219+1 2011/04/11 07:14:19 miaoweijie $
 *	$Author: miaoweijie $
 *	$Date: 2011/04/11 07:14:19 $
 *	$Revision: 219+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOLDBFIFORNETPRTCL_20080513_CPP_
#define _IOLDBFIFORNETPRTCL_20080513_CPP_

#include "FepIEC104Prtcl.h"
#include "FepIEC104Drv.h"
#include "FepIEC104Device.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"

#define MAX(a,b) ((a)>(b) ? (a):(b))

namespace MACS_SCADA_SUD
{
	void OnTimeOut(void* p);							//! 定时器超时回调函数
	void PeriodProc( void* pParent );					//! 周期定时器回调函数		
	
	std::string CFepIEC104Prtcl::s_ClassName("CFepIEC104Prtcl");		//! 类名

	CFepIEC104Prtcl::CFepIEC104Prtcl() 
	{
		//InitPrtclHelper( 6, 2048, 1024, 1024, 4096 );		//eldest
		InitPrtclHelper( 6, 255, 65536, 255, 65536*2 );	//2013.12.3
		//InitPrtclHelper( 6, 2048, 65536, 255, 65536*2 );	//2013.12.6
		m_iPeriodCount= 0;		//初始化为0，以便启动时立即执行一次。
		m_iForcedReconnectCount = 0;

		m_nSndLen = 0;
		//m_pBuf = new OpcUa_Byte[1024];//hbb为创建pPrtcl->BuildRequest( pPrtcl->m_pBuf, pPrtcl->m_nSndLen );开辟内存
		m_pBuf = NULL;//Modiby by hxw in 2013.12.3. 滑动窗口变量，无需开辟内存

		m_bIsLogValueError = true;
		m_bCheckSDI = true;
		m_bCheckDDI = true;
		m_bCheckVTI = true;

		//! 创建定时器
		m_pIOTimer = new CIOTimer( OnTimeOut, this, true );
		m_pPeriodTimer = new CIOTimer( PeriodProc, this );

		m_wdValidDataLen	= 0;
		m_nOfflineCount		= 0;

		m_shAdiLogical = 0;
		m_lastDevNo = 0;

		m_bNeedSendSFrame	= false;
		m_bNeedStopDT		= false;
		m_bNeedStartDT		= true;
		m_bNeedSendTest		= false;
		m_bNeedStartDTcon	= false;
		m_bNeedStopDTcon	= false;
		m_bNeedSendTestcon	= false;

		//超时，毫秒。50~10000，默认1000。
		m_TimeOut = 1000;

		//建立连接的超时，毫秒，默认30000
		m_t0 = 30000;

		//发送或测试APDU的超时，毫秒，默认15000
		m_t1 = 15000;

		//无数据保文t2<t1时确认的超时，毫秒，默认10000
		m_t2 = 10000;

		//长期空闲t3>t1状态下发送测试帧的超时，毫秒，默认20000
		m_t3 = 30000;

		//计数量召唤周期，毫秒，默认60000
		m_t4 = 60000;

		m_TestCycl = 10000;

		//校时周期, 秒.默认3600.
		m_CheckTimeCycl = 3600;

		//离线计数，默认15.
		m_nMaxOfflineCount = 15;

		m_bDebugLog = false;
		m_bLog = false;

		m_bNeedConnected = false;		//启动时由StartWork()自动连接，因此初值无需置TRUE。

		m_nSFrameCount = 0;
		m_nIFrameCount = 0;
		m_nLastIFrameCount = 0;

	}

	CFepIEC104Prtcl::~CFepIEC104Prtcl(void)
	{
		//! 删除定时器
		if(m_pIOTimer)
		{
			delete m_pIOTimer;
			m_pIOTimer = NULL;
		}

		if(m_pPeriodTimer)
		{
			delete m_pPeriodTimer;
			m_pPeriodTimer = NULL;
		}
		//delete m_pBuf;//hbb释放内存. Modiby by hxw in 2013.12.3
	}

	std::string CFepIEC104Prtcl::GetClassName()
	{
		return s_ClassName;
	}

	/*
	得到最小的包长度
	*/
	OpcUa_Int32 CFepIEC104Prtcl::GetMinPacketLength()
	{
		return PRTCL_MIN_PACKET_LENGTH;
	}

	/*
	组请求帧
	*/
	OpcUa_Int32 CFepIEC104Prtcl::BuildRequest( 
		OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen )
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "BuildRequest Starting --------------!" ) ));
		pDriver->WriteLog( "BuildRequest: Start......." );

		//先清零
		nDataLen = 0;
                
		//! 如果没有连接，直接返回
		CFepIEC104Device* pDevice = NULL;

		if ( m_bNeedConnected )
		{

			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest return -4!" ) ));
			pDriver->WriteLog( "BuildRequest: 链路未连接, 直接返回." );
			return -4;
		}		
		///////////////////////////////////////////////////////////

		if ( CIOPrtclParser::BuildRequest( pbyData, nDataLen )==-1 )
		{
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest return -1  000000000!" ) ));
			pDriver->WriteLog( "BuildRequest: CIOPrtclParser::BuildRequest return -1" );
			return -1;
		}

		bool bSendYK = false;

		//遥控、遥调命令总是优先发送。
		//这里，为了防止遥控丢失，从机也发送遥控。按照104规约，从机也可发送控制命令。hexuewen
		nDataLen = BuildCtlRequest( pbyData );
		if ( nDataLen > 0 )
		{
			OpcUa_Int32 i;

			ASDUHead* pHead = ( ASDUHead* )( pbyData + sizeof(APCI) );
			//把逻辑单元号切换为DO、AO所属设备，以便下次就能轮询到该设备。
			if ( GetDeviceNoByAddr( (OpcUa_Int32)pHead->wdCommAddr, i ) )
			{
				m_shAdiLogical = i;
				pDevice = dynamic_cast<CFepIEC104Device*>( 
					pDriver->GetDeviceByIndex(m_shAdiLogical) );
				if ( pDevice )
				{
					//置返信点为0
					OpcUa_Byte OpcUa_Bytemp = 0;
					pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytemp, 1, 0, true );	
				}
			}

			pDriver->WriteLog( "生成控制命令帧！" );
			m_bycmdType = 1;

			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest 发送遥控命令!" ) ));
			bSendYK= true;
		}
		else//! 无遥控则尝试组轮询帧。注意，不一定每次都必须发送请求。
		{
			short LastLogicalDevNo = m_shAdiLogical;
			do{
				pDevice = dynamic_cast<CFepIEC104Device*>( 
					pDriver->GetDeviceByIndex(m_shAdiLogical) );

				if( !pDevice )
				{
					MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest return -1  222222222!" ) ));
					return -1;
				}

				//! 判断设备是否被模拟了，如果被模拟了，则不发送请求了
				if ( pDevice->IsSimulate() )
				{
					MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest return -2!" ) ));
					return -2;
				}				

				if ( pDevice->IsHot() )		//! 为实际主,则进行正常的数据请求.
				{
					pDriver->WriteLog( "BuildRequest: to BuildLocalNormalFrame...." );

					nDataLen = BuildLocalNormalFrame( pbyData );

					m_bycmdType = 0;

					//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest 组请求帧!" ) ));

				}
				else			//! 为实际主,则进行正常的从机请求.
				{
					pDriver->WriteLog( "BuildRequest: to BuildLocalStandbyFrame...." );

					nDataLen = BuildLocalStandbyFrame( pbyData );

					m_bycmdType = 0;
				}

				//切换逻辑设备号,准备下次的请求
				m_shAdiLogical ++;
				if ( m_shAdiLogical >= pDriver->GetDeviceCount() )
				{
					m_shAdiLogical = 0;
				}

				//组帧成功后，即退出循环
				if ( nDataLen > 0 )
				{
					break;
				}

			}while ( LastLogicalDevNo != m_shAdiLogical );
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//! 发送数据
		if(nDataLen > 0)
		{
			//判断是否在线
			pDevice->m_byTxTimes ++;
			//if (pDevice->m_byTxTimes > MACSIEC104_NEEDCHECK_AFTERTXTIMES )
			//{
			//	pDevice->SetOnLine( pDevice->m_byRxTimes > 0 );
			//	pDevice->m_byTxTimes = 0;
			//	pDevice->m_byRxTimes = 0;
			//}

			////! KillTimer
			m_pIOTimer->KillTimer();
			////! 启动定时器
			m_pIOTimer->SetTimer(m_TimeOut);
   
			//先延迟10毫秒再发送
			ACE_Time_Value tv(0,10000);
			ACE_OS::sleep(tv);

			if( pDriver->m_pIODrvBoard->Write(pbyData, nDataLen) !=  0)
			{
				//UpdateDevState();
				return -3;
			}
                        
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest Write Frame !!!!!!!!!!!!!!!!" ) ));

			if ( bSendYK )
			{
				MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::从网络发送遥控!" ) ));
			}

			//else
			//{
			//	m_nOfflineCount = 0;

			//	//复位连接标志
			//	m_bNeedConnected = false;
			//}
		}
		else
		{
			//static OpcUa_Int32 nCounts = 0;
/*
			//
			//if( nCounts%10 == 0 )
			//{
				//判断是否在线
				pDevice->m_byTxTimes ++;
				//if (pDevice->m_byTxTimes > MACSIEC104_NEEDCHECK_AFTERTXTIMES )
				//{
				//	pDevice->SetOnLine( pDevice->m_byRxTimes > 0 );
				//	pDevice->m_byTxTimes = 0;
				//	pDevice->m_byRxTimes = 0;
				//}

				//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send S format Frame Count=%d!" ), m_nSFrameCount ));
				nDataLen = pDevice->BuildSFrame(pbyData);
				//m_nSFrameCount ++;

				////! KillTimer
				m_pIOTimer->KillTimer();
				////! 启动定时器
				m_pIOTimer->SetTimer(MACSIEC104DRIVER_MAX_TIMEOUT);

				if( pDriver->m_pIODrvBoard->Write(pbyData, nDataLen) !=  0)
				{
					UpdateDevState();
					return -3;
				}

				
			//}

			//nCounts++;
*/

			//允许启动空超时。但要注意在超时回调函数中判断上次是否为空超时。
			////! KillTimer
			m_pIOTimer->KillTimer();
			////! 启动定时器
			m_pIOTimer->SetTimer(m_TimeOut);

		}
		
		/////////////////////////////////////////////////////////////////

		//! 写日志
		pDriver->WriteLog( "Build Request!" );

		//! 记录通信监视
		if ( nDataLen>0 )
		{
			pDriver->WriteLog( "Send Request!" );
			pDriver->WriteCommData( pbyData, nDataLen, COMMDATA_SND );
		}

		//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest return 0!" ) ));
		return 0;
	}

	// *****************************************************
	// 函数功能: 组下发命令帧
	// 返 回 值: OpcUa_Int32 : 帧长度
	// 参数说明: 
	//    OpcUa_Byte* pFrame: 帧数据区
	// *****************************************************
	OpcUa_Int32 CFepIEC104Prtcl::BuildCtlRequest( OpcUa_Byte* sndBuf )
	{
		if ( !sndBuf )
			return 0;

		OpcUa_Int32 nRet = 0;
		OpcUa_Byte pchBuf[512];
		OpcUa_Int32 nBufLen = 512;	
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		nRet = pDriver->m_IORemoteCtrl.DeQueue( (char*)pchBuf, nBufLen );

		OpcUa_Int32 nLength = 0;	//初始化帧长
		if ( nRet==0 )
		{
			char chMsg[1024];		
			pDriver->WriteLogAnyWay("组下发命令帧... ...");

			memcpy( (OpcUa_Byte*)(&m_tCtrlRemoteCmd), pchBuf, sizeof(m_tCtrlRemoteCmd) );

			//取得设备地址
			CFepIEC104Device* pDevice = NULL;
			pDevice = dynamic_cast<CFepIEC104Device*>( 
				pDriver->GetDeviceByName(m_tCtrlRemoteCmd.pchUnitName) );
			if ( !pDevice )
			{
				pDriver->WriteLogAnyWay( "控制设备不存在！" );
				return 0;
			}			

			WORD Address = (WORD)m_tCtrlRemoteCmd.lTagAddress;	//标签地址
			OpcUa_Byte* buf	 = m_tCtrlRemoteCmd.byData;				//值

			switch( m_tCtrlRemoteCmd.byUnitType )
			{
			case MACS_IEC104_SDOa:			//遥控. 单遥，选择
				nLength = pDevice->BuildSDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_SDOb:			//遥控. 单遥，执行
				nLength = pDevice->BuildSDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_SDOc:			//遥控. 单遥，撤销
				nLength = pDevice->BuildSDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DDOa:			//遥控. 双遥，选择
				nLength = pDevice->BuildDDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DDOb:			//遥控. 双遥，执行
				nLength = pDevice->BuildDDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DDOc:			//遥控. 双遥，撤销
				nLength = pDevice->BuildDDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_UPDOa:			//遥控. 升降，选择
				nLength = pDevice->BuildUPDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_UPDOb:			//遥控. 升降，执行
				nLength = pDevice->BuildUPDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_UPDOc:			//遥控. 升降，撤销
				nLength = pDevice->BuildUPDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSDOa:			//遥控. 单遥（带时标），选择
				nLength = pDevice->BuildTSDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSDOb:			//遥控. 单遥（带时标），执行
				nLength = pDevice->BuildTSDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSDOc:			//遥控. 单遥（带时标），撤销
				nLength = pDevice->BuildTSDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDDOa:			//遥控. 双遥（带时标），选择
				nLength = pDevice->BuildTDDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDDOb:			//遥控. 双遥（带时标），执行
				nLength = pDevice->BuildTDDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDDOc:			//遥控. 双遥（带时标），撤销
				nLength = pDevice->BuildTDDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TUPDOa:		//遥控. 升降（带时标），选择
				nLength = pDevice->BuildTUPDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TUPDOb:		//遥控. 升降（带时标），执行
				nLength = pDevice->BuildTUPDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TUPDOc:		//遥控. 升降（带时标），撤销
				nLength = pDevice->BuildTUPDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_NVAAOa:		//遥调，归一化值，选择
				nLength = pDevice->BuildNVAAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_NVAAOb:		//遥调，归一化值，执行
				nLength = pDevice->BuildNVAAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_NVAAOc:		//遥调，归一化值，撤销
				nLength = pDevice->BuildNVAAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_SVAAOa:		//遥调，标度化值，选择
				nLength = pDevice->BuildSVAAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_SVAAOb:		//遥调，标度化值，执行
				nLength = pDevice->BuildSVAAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_SVAAOc:		//遥调，标度化值，撤销
				nLength = pDevice->BuildSVAAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_FAOa:			//遥调，短浮点数，选择
				nLength = pDevice->BuildFAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_FAOb:			//遥调，短浮点数，执行
				nLength = pDevice->BuildFAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_FAOc:			//遥调，短浮点数，撤销
				nLength = pDevice->BuildFAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DWAOa:			//遥调，32位字串，选择
				nLength = pDevice->BuildDWAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DWAOb:			//遥调，32位字串，执行
				nLength = pDevice->BuildDWAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DWAOc:			//遥调，32位字串，撤销
				nLength = pDevice->BuildDWAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TNVAAOa:		//遥调，归一化值（带时标），选择
				nLength = pDevice->BuildTNVAAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TNVAAOb:		//遥调，归一化值（带时标），执行
				nLength = pDevice->BuildTNVAAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TNVAAOc:		//遥调，归一化值（带时标），撤销
				nLength = pDevice->BuildTNVAAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSVAAOa:		//遥调，标度化值（带时标），选择
				nLength = pDevice->BuildTSVAAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSVAAOb:		//遥调，标度化值（带时标），执行
				nLength = pDevice->BuildTSVAAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSVAAOc:		//遥调，标度化值（带时标），撤销
				nLength = pDevice->BuildTSVAAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TFAOa:			//遥调，短浮点数（带时标），选择
				nLength = pDevice->BuildTFAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TFAOb:			//遥调，短浮点数（带时标），执行
				nLength = pDevice->BuildTFAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TFAOc:			//遥调，短浮点数（带时标），撤销
				nLength = pDevice->BuildTFAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDWAOa:		//遥调，32位字串（带时标），选择
				nLength = pDevice->BuildTDWAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDWAOb:		//遥调，32位字串（带时标），执行
				nLength = pDevice->BuildTDWAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDWAOc:		//遥调，32位字串（带时标），撤销
				nLength = pDevice->BuildTDWAOcFrame( sndBuf, Address, buf );
				break;
			default:
				break;
			}

			snprintf(chMsg,sizeof(chMsg)
				,"控制命令(设备名:%s,设备地址:%d),UnitType=%d, 信息地址=%d, 值=%d!"
				,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.byUnitType,Address,m_tCtrlRemoteCmd.byData[0]);
			pDriver->WriteLogAnyWay(chMsg);

			if ( nLength > 0 )
			{
				std::string szSndPacket = GetCommData( sndBuf, nLength, 0 );
				snprintf( chMsg, sizeof(chMsg),"控制命令共%d字节: %s", nLength, szSndPacket.c_str() );
				pDriver->WriteLogAnyWay( chMsg );
			}
		}

		return nLength;
	}

	//! 周期定时器回调函数
	void PeriodProc( void* pParent )
	{
		//校验参数
		CFepIEC104Prtcl* g_pPrtcl = (CFepIEC104Prtcl*)pParent;
		if ( g_pPrtcl == NULL )
		{
			return;
		}

		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)g_pPrtcl->GetParam();

		pDriver->WriteLog( "PeriodProc...");

		char pchLog[128];
		sprintf(pchLog, "PeriodProc: Before UpdateDevState: OfflineCount=%d,MaxOfflineCount=%d",g_pPrtcl->m_nOfflineCount,g_pPrtcl->m_nMaxOfflineCount );
		pDriver->WriteLog( pchLog );

		g_pPrtcl->m_nOfflineCount++;

		g_pPrtcl->UpdateDevState();

		CFepIEC104Device* pDevice = NULL;
		const OpcUa_Int32 nReconnect        = (g_pPrtcl->m_t0+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
		const OpcUa_Int32 nWholeCall		= (g_pPrtcl->m_t1+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
		//const OpcUa_Int32 nSFrame			= (g_pPrtcl->m_t3+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
		const OpcUa_Int32 nSFrame			= (3000+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;				//每隔三秒诊断一次
		const OpcUa_Int32 nTestFrame		= (g_pPrtcl->m_TestCycl+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;				
		const OpcUa_Int32 nCheckTime		= (g_pPrtcl->m_CheckTimeCycl*1000+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
		const OpcUa_Int32 nCountWholeCall	= (g_pPrtcl->m_t4+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
		const OpcUa_Int32 nMaxForcedReconnectCount = (MACSIEC104CONN_MAX_TIMEOUT)/1000 + 2;	//强制重连

		sprintf(pchLog, "PeriodProc: After UpdateDevState: OfflineCount=%d,MaxOfflineCount=%d,PeriodCount=%d,SFrameCount=%d,TestFrame=%d",
			g_pPrtcl->m_nOfflineCount,g_pPrtcl->m_nMaxOfflineCount, g_pPrtcl->m_iPeriodCount, nSFrame, nTestFrame );
		pDriver->WriteLog( pchLog );

		//动态更新调试日志标识
		if ( g_pPrtcl->m_iPeriodCount % 10 == 0 )
		{
			std::string strAppName = pDriver->GetDriverName();
			char* pchAppName = (char* )strAppName.c_str();

			//! 获取配置文件绝对路径
			std::string file_name = CIODriver::GetFEPRunDir() + MACS_DRIVER_FILENAME;	

			OpcUa_Int32 iTemp = GetPrivateProfileInt( pchAppName, "Debug", 0,  file_name.c_str() );
			g_pPrtcl->m_bDebugLog = (iTemp == 1)?true:false;

			iTemp = GetPrivateProfileInt( pchAppName, "Log", 0,  file_name.c_str() );
			g_pPrtcl->m_bLog = (iTemp == 1)?true:false;
		}

		if ( !g_pPrtcl->GetLinkState() && !pDriver->Connected() )	//所有设备均离线
		{
			if ( nReconnect && g_pPrtcl->m_iPeriodCount % nReconnect == 0 )	//连接（重连）子站时间到了
			{
				pDriver->WriteLogAnyWay( "PeriodProc: Need to reconnect...");
				g_pPrtcl->m_bNeedConnected = true;
			}
		}
		else if( pDriver->Connected() )	//如果所有设备均离线但链路又已经建立了，则应该判断以下标识
		{
			for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC104Device*>( pDriver->GetDeviceByIndex(i) );
				if ( pDevice )
				{
					if ( nWholeCall && g_pPrtcl->m_iPeriodCount % nWholeCall == 0 )				//总召唤周期到了
					{
						pDevice->NeedWholeQuery(true);
					}
					if ( nSFrame && g_pPrtcl->m_iPeriodCount % nSFrame == 0 )					//发送S帧周期到了
					{
						pDriver->WriteLog( "Need to send S Frame");
						g_pPrtcl->m_bNeedSendSFrame = true;
					}
					if ( nTestFrame && g_pPrtcl->m_iPeriodCount % nTestFrame == 0 )				//发送Test帧周期到了
					{
						g_pPrtcl->m_bNeedSendTest = true;
					}
					if ( nCheckTime && g_pPrtcl->m_iPeriodCount % nCheckTime == 0 )				//对时周期到了
					{
						pDevice->NeedCheckTime(true);
					}
					if ( nCountWholeCall && g_pPrtcl->m_iPeriodCount % nCountWholeCall == 0 )		//累计量总召唤周期到了
					{
						pDevice->NeedCountQuery(true);
					}
				}
			}
		}

		if ( g_pPrtcl->m_bNeedConnected )
		{
			if( !pDriver->IsConnecting() || g_pPrtcl->m_iForcedReconnectCount % nMaxForcedReconnectCount == nMaxForcedReconnectCount-1 )
			{
				g_pPrtcl->m_iForcedReconnectCount = 0;
				pDriver->WriteLog( "PeriodProc: Start to reconnect." );
				g_pPrtcl->m_bNeedConnected = false;	//重连一次后无论成功与否，下次重连等待周期到再重试。
				pDriver->IsConnecting(true);
				pDriver->ResetConnTimes();
				pDriver->Connected(false);
				pDriver->Connect( pDriver->NextSerAddrIndex() );
			}
			else
			{
				g_pPrtcl->m_iForcedReconnectCount++;
				pDriver->WriteLog( "PeriodProc: but it's connecting now." );
			}
		}

		g_pPrtcl->m_iPeriodCount++;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//OpcUa_Byte pbyBuf[256];
		//OpcUa_Int32 nDataLen = 0;
		//bool bTrigTimer = false;
		//long nTimeOut = 0;
		////! 发送数据
		//g_pPrtcl->BuildRequest( pbyBuf, nDataLen, bTrigTimer, nTimeOut );
	}


	//! 超时处理
	void OnTimeOut( void* p )
	{
		CFepIEC104Prtcl* pPrtcl = (CFepIEC104Prtcl*)p;

		if( !pPrtcl )
		{
			return;
		}

		//! KillTimer
		OpcUa_Int32 iRtn = pPrtcl->m_pIOTimer->KillTimer();

		//! 是否模拟运行
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)pPrtcl->GetParam();
		CFepIEC104Device* pDevice = NULL;
		pDevice = dynamic_cast<CFepIEC104Device*> ( 
			pDriver->GetDeviceByIndex( pPrtcl->m_lastDevNo ) );

		if ( pPrtcl->m_nSndLen > 0 )
		{
			//! 日志
			if ( iRtn == 0 )	//超时定时器不为空时才认为真的超时了。2014.9.11 hxw
			{
				std::string szData = pPrtcl->GetCommData( pPrtcl->m_pbySndBuffer, pPrtcl->m_nSndLen, 0 );
				char pchLog[1024];
				snprintf( pchLog, sizeof(pchLog), "设备响应超时,设备名：%s, 本次发送帧：%s",
					pDevice->GetName().c_str(), szData.c_str() );
				pDriver->WriteLogAnyWay( pchLog );
			}

			if ( pDevice && pDevice->IsSimulate() )
			{
				pPrtcl->m_pIOTimer->SetTimer( pPrtcl->m_TimeOut );
				return;
			}

			//释放独占标志
			if ( pDevice )
			{
				pDevice->IsCheckTiming(false);
				pDevice->WholeQuerying(false);
				pDevice->CountQuerying(false);
			}

			//! 更新设备状态
			//pPrtcl->UpdateDevState();
		}
		
		//! 如果链路状态为false，则试连下一个
		//if ( !pPrtcl->GetLinkState() )
		if ( pPrtcl->m_bNeedConnected )
		{
/*
			if( !pDriver->IsConnecting() )
			{
				pPrtcl->m_bNeedConnected = false;	//重连一次后无论成功与否，下次重连等待周期到再重试。
				pDriver->IsConnecting(true);
				pDriver->ResetConnTimes();
				pDriver->Connected(false);
				pDriver->Connect( pDriver->NextSerAddrIndex() );
				//pDriver->SetConnTimer( MACSIEC104CONN_MAX_TIMEOUT );
			}
*/

			pDriver->WriteLog( "OnTimeOut: 链路未连接, 直接返回." );
			return;
		}
		
		///////////////////////////////////////////////////////////

		//if(pPrtcl->m_bycmdType == 1)
		//{
			////取得设备地址
			//CIODeviceBase* pDevice = NULL;
			//pDevice = pDriver->GetDeviceByName(pPrtcl->m_tCtrlRemoteCmd.pchUnitName);
		
			//pDriver->PostYKSOE( 
			//	pPrtcl->m_tCtrlRemoteCmd.pchUnitName,
			//	pPrtcl->m_tCtrlRemoteCmd.byUnitType,
			//	pPrtcl->m_tCtrlRemoteCmd.lTagAddress,
			//	0);
		//}

		/////////////////////////////////////////////////////////
		//OpcUa_Byte pbyBuf[256];
		//OpcUa_Int32 nDataLen = 0;
		//! 发送数据
		pDriver->WriteLog( "OnTimeOut: to call BuildRequest" );
		pPrtcl->BuildRequest( pPrtcl->m_pbySndBuffer, pPrtcl->m_nSndLen );
	}

	void  CFepIEC104Prtcl::UpdateDevState()
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		char pchLog[256];
		CIODeviceBase* pDevice = NULL;	
		OpcUa_Int32 i =0;

		//如果持续一段时间都没有收到任何104报文，则认为离线了。
		if ( m_nOfflineCount>= m_nMaxOfflineCount )	
		{
			m_nOfflineCount = 0;

			pDriver->Connected( false );
			
			for ( i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = pDriver->GetDeviceByIndex( i );
				if ( pDevice && !pDevice->IsSimulate()  )
				{
					if ( pDevice->IsOnLine() )
					{
						sprintf( pchLog, "设备 %s 持续一段时间都没有收到任何104报文, 离线了", pDevice->GetName().c_str() );
						pDriver->WriteLogAnyWay( pchLog );
					}

					pDevice->SetOnLine( DEVICE_OFFLINE );
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes = 0;
				}
			}			
		}

		//设备为Hot时： TCP连接成功后为在线，此后如果持续一段时间都没有收到104 I格式报文，则认为离线了。
		if ( m_iPeriodCount % m_nMaxOfflineCount == m_nMaxOfflineCount-1 )
		{
			for ( i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = pDriver->GetDeviceByIndex( i );
				if ( pDevice && !pDevice->IsSimulate()  )
				{
					if ( pDevice->IsHot() )
					{
						if ( m_nIFrameCount == m_nLastIFrameCount )
						{
							if ( pDevice->IsOnLine() )
							{
								sprintf( pchLog, "主设备(Hot) %s 持续一段时间没有收到104 I格式报文, 离线了", pDevice->GetName().c_str() );
								pDriver->WriteLogAnyWay( pchLog );
							}

							pDevice->SetOnLine( DEVICE_OFFLINE );
							pDevice->m_byTxTimes = 0;
							pDevice->m_byRxTimes = 0;
						}
					}
				}
			}
			m_nLastIFrameCount = m_nIFrameCount;	//记下来，等到下个诊断周期到时，如果m_nIFrameCount没有变化，可认为Hot设备离线。
		}

	}

	// *****************************************************
	// 函数功能: 接收数据分析数据包并处理
	// 返 回 值: BOOL : 解出数据包返回true，否则false
	// 参数说明: 
	//     OpcUa_Byte* pBuf: 数据包
	//       OpcUa_Int32 iLen: 数据包长度
	// *****************************************************
	OpcUa_Int32 CFepIEC104Prtcl::RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nLen )
	{

		//! 通道如果被禁止了，则直接返回0
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		if (!pDriver)
			return -1;	

		char strLog[256];
		if ( IsLogForRxDataProc() )
		{
			pDriver->WriteLogAnyWay( "RxDataProc: begin...." );
		}

		if(  pDriver->IsDisable() )
		{
			if ( IsLogForRxDataProc() )
			{
				pDriver->WriteLogAnyWay( "通道被禁止" );
			}
			return 0;
		}

		if ( IsLogForRxDataProc() )
		{
			pDriver->WriteLogAnyWay( "RxDataProc: WriteCommData begin...." );
		}

		///////////////////////////////////////////////////////
		//! 如果需要通信监视，则发送原始数据包
		pDriver->WriteCommData( pDataBuf, nLen );

		if ( IsLogForRxDataProc() )
		{
			pDriver->WriteLogAnyWay( "RxDataProc: WriteCommData end.." );
		}

		//把数据放到内部缓冲区
		InnerAddData( pDataBuf, nLen );

		if ( IsLogForRxDataProc() )
		{
			pDriver->WriteLogAnyWay( "RxDataProc: InnerAddData end.." );
		}

		//从内部缓冲区中执行滑动窗口，找数据包，注意一次可能会处理多个数据包
		bool bPacketOk = false;
		OpcUa_Int32	nReceived = 0;
		do
		{
			//可取的数据长度
			nReceived = PRTCL_RCV_BUFFER_LENGTH - m_nRevLength;
			if (0 >= nReceived || nReceived > PRTCL_RCV_BUFFER_LENGTH)
			{
				m_nRevLength = 0;
				m_bIncompleteFrame	= false;
				break;
			}

			//从内部缓冲区取数分析
			if( InnerRead( m_pbyRevBuffer+m_nRevLength, nReceived, nReceived) 
				&& nReceived > 0 )
			{
				m_nRevLength += nReceived;
				m_bIncompleteFrame = false;

				while( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH && !m_bIncompleteFrame )
				{
					if ( m_wdValidDataLen > 0 )	//前一次ExtractPacket()已初步判断解析成功
					{
						if ( m_bExtractError )	//前一次解析成功的数据处理失败
						{//去掉首字节，重新解析

							if ( IsLogValueError() )
							{
								char strLog[1024]={0};
								snprintf( strLog, sizeof(strLog),"前一次解析成功但处理数据时失败,去掉首字(%02X)节再重新解析!", m_pBuf[0] );
								pDriver->WriteLogAnyWay( strLog );

								std::string szSndPacket = GetCommData( m_sPacketBuffer, m_wdValidDataLen, 1 );
								snprintf( strLog, sizeof(strLog),"处理失败的原报文(共%d字节): %s", m_wdValidDataLen, szSndPacket.c_str() );
								pDriver->WriteLogAnyWay( strLog );
							}

							m_nRevLength--;	
							ACE_OS::memcpy( m_pbyRevBuffer, m_pBuf+1, m_nRevLength );	
						}
						else					//前一次解析成功的数据处理无误
						{
							//去掉上一次已解析的字节
							m_nRevLength -= m_wdValidDataLen;
							if ( m_nRevLength > 0 )
							{
								ACE_OS::memcpy( m_pbyRevBuffer, 
									m_pBuf+m_wdValidDataLen, m_nRevLength );
							}
						}

						m_wdValidDataLen = 0;
					}

					if ( ( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH && !m_bIncompleteFrame ) )
					{
						if ( ExtractPacket( ) ) 
						{
							if ( IsLogForRxDataProc() )
							{
								sprintf( strLog, "Before ProcessRcvdPacket: ValidLen = %d, LeftLen = %d, Incomplete =%d, ExtractError=%d", 
									m_wdValidDataLen,m_nRevLength,m_bIncompleteFrame,m_bExtractError);
								pDriver->WriteLogAnyWay( strLog );
							}

							//找到数据包后处理掉
							ProcessRcvdPacket( true );
							bPacketOk = true;		//只要成功解析过一次就返回成功.

							if ( IsLogForRxDataProc() )
							{
								sprintf( strLog, "After ProcessRcvdPacket: ValidLen = %d, LeftLen = %d, Incomplete =%d, ExtractError=%d", 
									m_wdValidDataLen,m_nRevLength,m_bIncompleteFrame,m_bExtractError);
								pDriver->WriteLogAnyWay( strLog );
							}
						}
						else
						{
							if ( IsLogForRxDataProc() )
							{
								sprintf( strLog, "ExtractPacket failed: ValidLen = %d, LeftLen = %d, Incomplete =%d, ExtractError=%d", 
									m_wdValidDataLen,m_nRevLength,m_bIncompleteFrame,m_bExtractError);
								pDriver->WriteLogAnyWay( strLog );
							}
						}
					}
				}
			}
		}while( 0 < nReceived );

		if ( IsLogForRxDataProc() )
		{
			pDriver->WriteLogAnyWay( "RxDataProc: BuildRequest begin..." );
		}

		//////////////////////////////////////////////////////////////////////////////
		//! 组帧并发送数据
		//OpcUa_Int32 nDataLen = 0;
		BuildRequest( m_pbySndBuffer, m_nSndLen );
		//////////////////////////////////////////////////////////////////////////////

		if ( IsLogForRxDataProc() )
		{
			sprintf( strLog,"RxDataProc: BuildRequest end. SndLen = %d", m_nSndLen );
			pDriver->WriteLogAnyWay( strLog );
		}

		//返回处理包情况
		return bPacketOk ? 0 : -1;
	}

	/*!
	*	\brief	分析出接收数据包
	*
	*	\param	pPacketBuffer	分析出的数据包
	*	\param	pnPacketLength	数据包长度
	*
	*	\retval	bool	成功true，否则false
	*/
	bool CFepIEC104Prtcl::ExtractPacket()
	{
		char strLog[256];

		m_wdValidDataLen = 0;
		m_pBuf = m_pbyRevBuffer;
		m_bIncompleteFrame = false;

		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		if ( IsLog() )
		{
			char* pT = new char[MAX(m_nRevLength*3+10, 4)];
			OpcUa_Int32 i;
			for ( i = 0; i < m_nRevLength; i++ )
			{
				sprintf( pT + 3*i, " %02x", m_pBuf[i] );
			}
			sprintf( pT + 3*i, "\n" );

			//! 写日志
			pDriver->WriteLog( pT );

			delete []pT;
		}

		while ( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH )
		{
			if ( m_pBuf[0] != PACK_HEAD )		//帧首字节非法
			{
				sprintf( strLog, "帧首字节非法: %d", m_pBuf[0] );

				//! 写日志
				pDriver->WriteLog( strLog );

				m_pBuf++;
				m_nRevLength--;
				continue;
			}

			if ( m_pBuf[1] < MIN_APDU_LEN || m_pBuf[1] > MAX_APDU_LEN )	//APDU长度非法
			{
				sprintf( strLog, "APDU长度非法: %d", m_pBuf[1] );

				//! 写日志
				pDriver->WriteLog( strLog );

				m_pBuf++;
				m_nRevLength--;
				continue;
			}

			if ( m_pBuf[1] == MIN_APDU_LEN )	//短帧,可能是S格式或U格式
			{
				if ( IsCtrlAreaS(m_pBuf+2) )	//S格式
				{
					//! 写日志
					pDriver->WriteLog( "S格式" );

					//		m_bNeedSendSFrame = true;	//置标志, 需要主站发送S帧

					//修改本地发送序号，与子站对齐。因驱动不考虑重发！
					APCI* pAPCI = (APCI* )m_pBuf;
					WORD wdNs	= pAPCI->unCtrlArea.CtrlAreaS.Nr;
					CFepIEC104Device* pDevice = NULL;
					for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
					{
						pDevice = (CFepIEC104Device* )pDriver->GetDeviceByIndex(i);
						if ( pDevice )
						{
							//	pDevice->SetNs( wdNs );
						}
					}

					m_wdValidDataLen = MIN_APDU_LEN + 2;
					break;
				}
				else if ( IsCtrlAreaU(m_pBuf+2) )	//U格式
				{
					//! 写日志
					pDriver->WriteLog( "U格式" );

					structCtrlAreaU* pU = (structCtrlAreaU* )( m_pBuf + 2);
					if ( pU->STARTDT_ACT )
					{
						//! 写日志
						pDriver->WriteLogAnyWay( "U格式: StartDT生效" );
						m_bNeedStartDTcon = true;
					}
					else if ( pU->STARTDT_CON )
					{
						//! 写日志
						pDriver->WriteLogAnyWay( "U格式: StartDT确认" );
						m_bNeedStartDT = false;
					}
					else if ( pU->STOPDT_ACT )
					{
						//! 写日志
						pDriver->WriteLogAnyWay( "U格式: StopDT生效" );
						m_bNeedStopDTcon = true;
					}
					else if ( pU->STOPDT_CON )
					{
						//! 写日志
						pDriver->WriteLogAnyWay( "U格式: StopDT确认" );
						m_bNeedStopDT = false;
					}
					else if ( pU->TESTFR_ACT )
					{
						//! 写日志
						pDriver->WriteLog( "U格式: TestFr生效" );
						m_bNeedSendTestcon = true;
					}
					else if ( pU->TESTFR_CON )
					{
						//! 写日志
						pDriver->WriteLog( "U格式: TestFr确认" );
						m_bNeedSendTest = false;
					}

					m_wdValidDataLen = MIN_APDU_LEN + 2;
					break;
				}
				else	//无法辨识的格式
				{
					//! 写日志
					pDriver->WriteLog( "U格式: 无法辨识的格式" );
					m_pBuf++;
					m_nRevLength--;
					continue;
				}
			}

			if ( m_pBuf[1] >= MIN_I_APDU_LEN && IsCtrlAreaI(m_pBuf+2) )	//I格式
			{
				if ( m_nRevLength < m_pBuf[1]+2 )	//I格式帧接收还不完整
				{//有隐患: 比如接收区(HEX)为68 68 4 ..., 多了一个干扰的68时, 会丢帧.
					//已有了解决办法: 在ProcessRcvdPacket()中再判断帧是否无误.

					//! 写日志
					pDriver->WriteLog( "I格式: I格式帧接收还不完整" );

					m_bIncompleteFrame = true;
				}
				else
				{
					//认为成功收到I格式帧
					//! 写日志
					pDriver->WriteLog( "I格式: 成功收到I格式帧" );

					m_wdValidDataLen = m_pBuf[1]+2;
				}

				break;
			}

			//无法辨识的格式
			//! 写日志
			pDriver->WriteLog( "无法辨识的格式" );

			m_pBuf++;
			m_nRevLength--;
			continue;
		}

		//1.先将合格的帧放置到有效数据缓冲区
		if ( m_wdValidDataLen > 0 )
		{
			ACE_OS::memcpy(m_sPacketBuffer, m_pBuf, m_wdValidDataLen );
		}

		//2.再去掉已明确错误的数据部分。注意，这里步骤1和步骤2的次序不能打乱。
		OpcUa_Int32 nErrorLen = m_pBuf - m_pbyRevBuffer;
		if ( nErrorLen > 0 )
		{
			char strLog[1024]={0};
			std::string szErrorPacket = GetCommData( m_pbyRevBuffer, nErrorLen, 1 );
			snprintf( strLog, sizeof(strLog),"去掉错误数据,共%d字节: %s", nErrorLen, szErrorPacket.c_str() );
			pDriver->WriteLogAnyWay( strLog );

			//m_nRevLength -= nErrorLen;
			if ( m_nRevLength > 0 )
			{				
				ACE_OS::memcpy(m_pbyRevBuffer, m_pBuf, m_nRevLength);
				m_pBuf = m_pbyRevBuffer;	//以上memcpy完成后，必须把m_pBuf指针复位到m_pbyRevBuffer首指针。m_pBuf在接下来的操作中还会用到。2014.9.11 hxw
			}
			else
				m_nRevLength = 0;
		}

		m_bycmdType = 0;

		return ( m_wdValidDataLen > 0 );
	}

	// *****************************************************
	// 函数功能: 处理完整的数据包
	// 算法思想: 如果设备禁止则不产生外部事件
	// 返 回 值: void : 该函数不返回任何值
	// 参数说明: 
	//         BOOL bFromLocal: 区分是本机来的还是另机来的
	// *****************************************************
	void CFepIEC104Prtcl::ProcessRcvdPacket( bool bFromLocal )
	{
		//////////////////////////////////////////////////////////////////////////////
		//! KillTimer
		m_pIOTimer->KillTimer();
		//////////////////////////////////////////////////////////////////////////////

		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		if (!pDriver)
			return;

		if (pDriver->IsStopWork())
		{
			return;
		}

		m_bExtractError			= false;
		char strLog[1024];
		APCI* pAPCI = (APCI* )m_sPacketBuffer;
		
		//获得"接收序号"，但必须确认帧无误后才能更新到本地。
		WORD wdNr = pAPCI->unCtrlArea.CtrlAreaI.Ns;	

		//获得"发送序号"，但必须确认帧无误后才能更新到本地。
		WORD wdNs = pAPCI->unCtrlArea.CtrlAreaI.Nr; 

		ASDUHead* pHead = NULL;

		CFepIEC104Device* pDevice = NULL;
		if ( IsCtrlAreaI( &pAPCI->unCtrlArea ) )	//I格式帧, 更新具体设备状态	
		{
			m_nIFrameCount++;

			m_nOfflineCount = 0;

			pHead = (ASDUHead* )(m_sPacketBuffer + sizeof(APCI));

			//根据地址找到对应的逻辑单元号
			OpcUa_Int32 UnitAddr = (OpcUa_Int32)pHead->wdCommAddr;
			pDevice = dynamic_cast<CFepIEC104Device*>( 
				pDriver->GetDeviceByAddr( UnitAddr ) );
			if ( !pDevice )
				return;

/*	//放到后面，防止处理失败的错误报文，但又把设备置在线了。2014.9.11
			//设置装置在线
			if( bFromLocal )
			{
				pDevice->GenNextNr();

				//适时发送S帧
				m_bNeedSendSFrame = pDevice->IsNeedSendSFrame();

				pDevice->SetOnLine( DEVICE_ONLINE );	//主从设备收到I帧一律认为在线
				pDevice->m_byTxTimes = 0;
				pDevice->m_byRxTimes++;
			}

			//复位连接标志
			m_bNeedConnected = false;
*/
		}
		else if ( IsCtrlAreaS( &pAPCI->unCtrlArea ) 
			|| IsCtrlAreaU( &pAPCI->unCtrlArea ) ) //S格式或U格式帧, 更新所有设备状态
		{
			if ( bFromLocal )
			{
				m_nOfflineCount = 0;

				CIODeviceBase* pTempDevice = NULL;	//局部变量, 不影响后面的操作.
				for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
				{
					pTempDevice = pDriver->GetDeviceByIndex(i);
					if ( pTempDevice )
					{
						if ( !pTempDevice->IsHot() )	//从设备收到S帧或者U帧可认为在线；但主设备（Hot）必须收到I帧才认为在线
						{
							pTempDevice->SetOnLine( DEVICE_ONLINE );
						}
						pTempDevice->m_byTxTimes = 0;
						pTempDevice->m_byRxTimes++;
					}
				}

				//复位连接标志
				m_bNeedConnected = false;
			}
		}

		if ( !pDevice || !pHead )
		{
			pDriver->WriteLog( "非I格式帧" );
			return;
		}

		if ( !pDevice->IsHot() )
		{
			pDriver->WriteLog( "从机不处理I格式帧" );
			return;
		}

		//入数据库
		if ( !pDevice->IsSimulate() )	//如果设备处于模拟状态，则来自设备的数据不入数据库。
		{
			//S格式和U格式帧已在解析时考虑, 此处忽略. 只考虑I格式帧.
			if ( !IsCtrlAreaI( &pAPCI->unCtrlArea ) )	
			{
				pDriver->WriteLog( "非I格式帧" );
				return;
			}

			////! 时标
			//ACE_Time_Value cur_tv = ACE_OS::gettimeofday();
			//OpcUa_Int64 nTime = ((OpcUa_Int64)cur_tv.sec())*1000 + cur_tv.usec()/1000;

			OpcUa_Int32 i;
			OpcUa_Int32 nNum;
			float ftempVal = 0;
			OpcUa_Byte byOldVal = 0;
			std::string szTagName = "";
			OpcUa_Int32 iCount=0;
			switch( pHead->byType )	//类型标识
			{
			case M_SP_NA_1:		//不带时标的单点信息
				{
					pDriver->WriteLog( "不带时标的单点信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structSDINoTimeNoInfoAddr* pPoOpcUa_Int = (structSDINoTimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structSDINoTimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						if( !IsValidSDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( *pPoOpcUa_Int ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSIQ.SPI;

								if( pDevice->IsDIValueChange( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "不带时标的单点信息: 设备'%s',所属标签:%s,信息地址%d, 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(), pStartInfoAddr->wdInfoAddr+i,byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_DI, 
									pStartInfoAddr->wdInfoAddr+i, 
									&OpcUa_Bytemp, 1, 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structSDINoTime* pPoOpcUa_Int = (structSDINoTime* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structSDINoTime)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						if( !IsValidSDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( *pPoOpcUa_Int ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSIQ.SPI;

								if( pDevice->IsDIValueChange( MACS_IEC104_DI, pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "不带时标的单点信息: 设备'%s'所属标签:%s,信息地址%d, 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(), pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_DI, 
									pPoOpcUa_Int->wdInfoAddr, 
									&OpcUa_Bytemp, 1, 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_SP_TA_1:		//带时标的单点信息
				{
					pDriver->WriteLog( "带时标的单点信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structSDI_Time24NoInfoAddr* pPoOpcUa_Int = (structSDI_Time24NoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structSDI_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						if( !IsValidSDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( pPoOpcUa_Int->sSDINoTime ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSDINoTime.sSIQ.SPI;

								if( pDevice->IsDIValueChange( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "带时标的单点信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(), pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_DI, 
									pStartInfoAddr->wdInfoAddr+i, 
									&OpcUa_Bytemp, 1, llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structSDI_Time24* pPoOpcUa_Int = (structSDI_Time24* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structSDI_Time24)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						if( !IsValidSDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( pPoOpcUa_Int->sSDINoTime ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSDINoTime.sSIQ.SPI;

								if( pDevice->IsDIValueChange( MACS_IEC104_DI, pPoOpcUa_Int->sSDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "带时标的单点信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(), pPoOpcUa_Int->sSDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_DI, 
									pPoOpcUa_Int->sSDINoTime.wdInfoAddr, 
									&OpcUa_Bytemp, 1, llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_DP_NA_1:		//不带时标的双点信息
				{
					pDriver->WriteLog( "不带时标的双点信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structDDINoTimeNoInfoAddr* pPoOpcUa_Int = (structDDINoTimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structDDINoTimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						if( !IsValidDDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( *pPoOpcUa_Int ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDIQ.DPI;

								if ( pDevice->IsExitTag( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i ) )
								{
									if( pDevice->IsDIValueChange( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
									{
										sprintf( strLog, "不带时标的双点信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
											pDevice->GetName().c_str(), szTagName.c_str(),pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
										pDriver->WriteLogAnyWay(strLog);
										iCount++;
									}
								}
								else if( pDevice->IsAIValueChange( MACS_IEC104_AI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "不带时标的双点信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(),pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_DI, 
									pStartInfoAddr->wdInfoAddr+i, 
									&OpcUa_Bytemp, 1, 0, true );
								
								//Add by hxw in 2013.3.19
								ftempVal = (float)OpcUa_Bytemp;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i,
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDDINoTime* pPoOpcUa_Int = (structDDINoTime* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structDDINoTime)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						if( !IsValidDDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( *pPoOpcUa_Int ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDIQ.DPI;

								if ( pDevice->IsExitTag( MACS_IEC104_DI, pPoOpcUa_Int->wdInfoAddr ) )
								{
									if( pDevice->IsDIValueChange( MACS_IEC104_DI, pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
									{
										sprintf( strLog, "不带时标的双点信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
											pDevice->GetName().c_str(), szTagName.c_str(), pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp );
										pDriver->WriteLogAnyWay(strLog);
										iCount++;
									}
								}
								else if( pDevice->IsAIValueChange( MACS_IEC104_AI, pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "不带时标的双点信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(),pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}


								pDevice->SetVal( MACS_IEC104_DI, 
									pPoOpcUa_Int->wdInfoAddr, 
									&OpcUa_Bytemp, 1, 0, true );
								
								//Add by hxw in 2013.3.19
								ftempVal = (float)OpcUa_Bytemp;
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->wdInfoAddr,
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );

							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_DP_TA_1:		//带时标的双点信息
				{
					pDriver->WriteLog( "带时标的双点信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structDDI_Time24NoInfoAddr* pPoOpcUa_Int = (structDDI_Time24NoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structDDI_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						if( !IsValidDDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( pPoOpcUa_Int->sDDINoTime ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDDINoTime.sDIQ.DPI;

								if ( pDevice->IsExitTag( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i ) )
								{
									if( pDevice->IsDIValueChange( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
									{
										sprintf( strLog, "带时标的双点信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
											pDevice->GetName().c_str(), szTagName.c_str(), pStartInfoAddr->wdInfoAddr+i,byOldVal, OpcUa_Bytemp );
										pDriver->WriteLogAnyWay(strLog);
										iCount++;
									}
								}
								else if( pDevice->IsAIValueChange( MACS_IEC104_AI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "带时标的双点信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(),pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}


								pDevice->SetVal( MACS_IEC104_DI, 
									pStartInfoAddr->wdInfoAddr+i, 
									&OpcUa_Bytemp, 1, llTime, true );
								
								//Add by hxw in 2013.3.19
								ftempVal = (float)OpcUa_Bytemp;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i,
									(OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDDI_Time24* pPoOpcUa_Int = (structDDI_Time24* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structDDI_Time24)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							break;
						}

						if( !IsValidDDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( pPoOpcUa_Int->sDDINoTime ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDDINoTime.sDIQ.DPI;

								if ( pDevice->IsExitTag( MACS_IEC104_DI, pPoOpcUa_Int->sDDINoTime.wdInfoAddr ) )
								{
									if( pDevice->IsDIValueChange( MACS_IEC104_DI, pPoOpcUa_Int->sDDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
									{
										sprintf( strLog, "带时标的双点信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
											pDevice->GetName().c_str(), szTagName.c_str(), pPoOpcUa_Int->sDDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp );
										pDriver->WriteLogAnyWay(strLog);
										iCount++;
									}
								}
								else if( pDevice->IsAIValueChange( MACS_IEC104_AI, pPoOpcUa_Int->sDDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "带时标的双点信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(),pPoOpcUa_Int->sDDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}


								pDevice->SetVal( MACS_IEC104_DI, 
									pPoOpcUa_Int->sDDINoTime.wdInfoAddr, 
									&OpcUa_Bytemp, 1,llTime, true );
								
								//Add by hxw in 2013.3.19
								ftempVal = (float)OpcUa_Bytemp;
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->sDDINoTime.wdInfoAddr,
									(OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ST_NA_1:		//不带时标的步位置信息
				{
					pDriver->WriteLog( "不带时标的步位置信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structVTINoInfoAddr* pPoOpcUa_Int = (structVTINoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structVTINoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						if( !IsValidVTI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->sVTI);
								OpcUa_Byte OpcUa_Bytemp = (OpcUa_Byte)ftempVal;

								if( pDevice->IsAIValueChange( MACS_IEC104_BWZ, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "不带时标的步位置信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(),pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_BWZ, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structVTI* pPoOpcUa_Int = (structVTI* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structVTI)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						if( !IsValidVTI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->sVTI);
								OpcUa_Byte OpcUa_Bytemp = (OpcUa_Byte)ftempVal;

								if( pDevice->IsAIValueChange( MACS_IEC104_BWZ, pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "不带时标的步位置信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(),pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_BWZ, 
									pPoOpcUa_Int->wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ST_TA_1:		//带时标的步位置信息
				{
					pDriver->WriteLog( "带时标的步位置信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structVTI_Time24NoInfoAddr* pPoOpcUa_Int = (structVTI_Time24NoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structVTI_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						if( !IsValidVTI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( pPoOpcUa_Int->ssVTI ) )	//有效
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->ssVTI.sVTI);
								OpcUa_Byte OpcUa_Bytemp = (OpcUa_Byte)ftempVal;

								if( pDevice->IsAIValueChange( MACS_IEC104_BWZ, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "带时标的步位置信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(),pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_BWZ, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structVTI_Time24* pPoOpcUa_Int = (structVTI_Time24* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structVTI_Time24)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						if( !IsValidVTI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( pPoOpcUa_Int->ssVTI ) )	//有效
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->ssVTI.sVTI);
								OpcUa_Byte OpcUa_Bytemp = (OpcUa_Byte)ftempVal;

								if( pDevice->IsAIValueChange( MACS_IEC104_BWZ, pPoOpcUa_Int->ssVTI.wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "带时标的步位置信息: 设备'%s'所属标签:%s,信息地址%d 发生%d到%d的变位",
										pDevice->GetName().c_str(), szTagName.c_str(),pPoOpcUa_Int->ssVTI.wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_BWZ, 
									pPoOpcUa_Int->ssVTI.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_BO_NA_1:		//32比特串
				{
					pDriver->WriteLog( "32比特串" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRTSNoInfoAddr* pPoOpcUa_Int = (structRTSNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRTSNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRTS* pPoOpcUa_Int = (structRTS* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRTS)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, 
									pPoOpcUa_Int->wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_BO_TA_1:		//带时标的32比特串
				{
					pDriver->WriteLog( "带时标的32比特串" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRTS_Time24NoInfoAddr* pPoOpcUa_Int = (structRTS_Time24NoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRTS_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( pPoOpcUa_Int->sRTS ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRTS.dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRTS_Time24* pPoOpcUa_Int = (structRTS_Time24* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRTS_Time24)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( pPoOpcUa_Int->sRTS ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRTS.dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, 
									pPoOpcUa_Int->sRTS.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_NA_1:		//测量值, 规一化值
				{
					pDriver->WriteLog( "测量值, 规一化值" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bitNoInfoAddr* pPoOpcUa_Int = (structRT16bitNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bitNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit* pPoOpcUa_Int = (structRT16bit* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRT16bit)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TA_1:		//测量值, 带时标的规一化值
				{
					pDriver->WriteLog( "测量值, 带时标的规一化值" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bit_Time24NoInfoAddr* pPoOpcUa_Int = 
							(structRT16bit_Time24NoInfoAddr* )(m_sPacketBuffer 
							+ sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bit_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sRT.sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit_Time24* pPoOpcUa_Int = (structRT16bit_Time24* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRT16bit_Time24)+sizeof(APCI)
							+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sRT.sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->sRT.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_NB_1:		//测量值, 标度化值
				{
					pDriver->WriteLog( "测量值, 标度化值" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bitNoInfoAddr* pPoOpcUa_Int = (structRT16bitNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bitNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit* pPoOpcUa_Int = (structRT16bit* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRT16bit)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TB_1:		//测量值, 带时标的标度化值
				{
					pDriver->WriteLog( "测量值, 带时标的标度化值" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bit_Time24NoInfoAddr* pPoOpcUa_Int 
							= (structRT16bit_Time24NoInfoAddr* )(m_sPacketBuffer 
							+ sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bit_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit_Time24* pPoOpcUa_Int = (structRT16bit_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRT16bit_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, pPoOpcUa_Int->sRT.wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_NC_1:		//测量值, 短浮点数
				{
					pDriver->WriteLog( "测量值, 短浮点数" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structRT32bitNoInfoAddr* pPoOpcUa_Int = (structRT32bitNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structRT32bitNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->fValue;
								pDevice->SetVal( MACS_IEC104_AI, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT32bit* pPoOpcUa_Int = (structRT32bit* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRT32bit)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->fValue;
								pDevice->SetVal( MACS_IEC104_AI, pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TC_1:		//测量值,带时标短浮点数
				{
					pDriver->WriteLog( "测量值,带时标短浮点数" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structRT32bit_Time24NoInfoAddr* pPoOpcUa_Int = (structRT32bit_Time24NoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structRT32bit_Time24NoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.fValue;
								pDevice->SetVal( MACS_IEC104_AI, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float),llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT32bit_Time24* pPoOpcUa_Int = (structRT32bit_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRT32bit_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.fValue;
								pDevice->SetVal( MACS_IEC104_AI, pPoOpcUa_Int->sRT.wdInfoAddr,  (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_IT_NA_1:		//累积量
				{
					pDriver->WriteLog( "累积量" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structDNLJNoInfoAddr* pPoOpcUa_Int = (structDNLJNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structDNLJNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDNLJ* pPoOpcUa_Int = (structDNLJ* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structDNLJ)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_IT_TA_1:		//带时标的累积量
				{
					pDriver->WriteLog( "带时标的累积量" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structDNLJ_Time24NoInfoAddr* pPoOpcUa_Int = (structDNLJ_Time24NoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structDNLJ_Time24NoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( pPoOpcUa_Int->sDNLJ ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sDNLJ.Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDNLJ_Time24* pPoOpcUa_Int = (structDNLJ_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structDNLJ_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( pPoOpcUa_Int->sDNLJ ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sDNLJ.Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, pPoOpcUa_Int->sDNLJ.wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EP_TA_1:		//带时标的继电保护装置事件
				{
					pDriver->WriteLog( "带时标的继电保护装置事件" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structJDBH_Time24NoInfoAddr* pPoOpcUa_Int = (structJDBH_Time24NoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structJDBH_Time24NoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBH( pPoOpcUa_Int->sJDBHNoTime ) )	//有效
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Int32)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
								ftempVal = (float)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES;
								pDevice->SetVal( MACS_IEC104_JDBH, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structJDBH_Time24* pPoOpcUa_Int = (structJDBH_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structJDBH_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBH( pPoOpcUa_Int->sJDBHNoTime ) )	//有效
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pPoOpcUa_Int->sJDBHNoTime.wdInfoAddr, (OpcUa_Int32)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
								ftempVal = (float)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES;
								pDevice->SetVal( MACS_IEC104_JDBH, pPoOpcUa_Int->sJDBHNoTime.wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EP_TB_1:		//带时标的继电保护装置成组启动事件
				{
					pDriver->WriteLog( "带时标的继电保护装置成组启动事件" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structJDBHGroupAct_Time24NoInfoAddr* pPoOpcUa_Int = (structJDBHGroupAct_Time24NoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structJDBHGroupAct_Time24NoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupAct( pPoOpcUa_Int->sJDBHGroupActNoTime ) )	//有效
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupActNoTime.byVal, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structJDBHGroupAct_Time24* pPoOpcUa_Int = (structJDBHGroupAct_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structJDBHGroupAct_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupAct( pPoOpcUa_Int->sJDBHGroupActNoTime ) )	//有效
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pPoOpcUa_Int->sJDBHGroupActNoTime.wdInfoAddr, (OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupActNoTime.byVal, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EP_TC_1:		//带时标的继电保护装置成组输出电路信息
				{
					pDriver->WriteLog( "带时标的继电保护装置成组输出电路信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structJDBHGroupOutput_Time24NoInfoAddr* pPoOpcUa_Int = (structJDBHGroupOutput_Time24NoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structJDBHGroupOutput_Time24NoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupOutput( pPoOpcUa_Int->sJDBHGroupOutputNoTime ) )	//有效
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupOutputNoTime.byVal, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structJDBHGroupOutput_Time24* pPoOpcUa_Int = (structJDBHGroupOutput_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structJDBHGroupOutput_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupOutput( pPoOpcUa_Int->sJDBHGroupOutputNoTime ) )	//有效
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pPoOpcUa_Int->sJDBHGroupOutputNoTime.wdInfoAddr, (OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupOutputNoTime.byVal, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_PS_NA_1:		//带变位检出的成组单点信息
				{
					pDriver->WriteLog( "带变位检出的成组单点信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structDIGroupNoInfoAddr* pPoOpcUa_Int = (structDIGroupNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structDIGroupNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int32 k;
						WORD Addr;
						OpcUa_Byte Val;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDIGroup( *pPoOpcUa_Int ) )
							{
								for ( k = 0; k < 16; k++ )
								{
									Addr = pStartInfoAddr->wdInfoAddr+k;
									if ( Addr <= DI_END_ADDR )	//有效
									{
										Val = (pPoOpcUa_Int->sSCD.wdST>>k)&0X01;
										pDevice->SetVal( MACS_IEC104_DI, Addr, &Val, 1, 0, true );
									}
								}
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDIGroup* pPoOpcUa_Int = (structDIGroup* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structDIGroup)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int32 k;
						WORD Addr;
						OpcUa_Byte Val;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDIGroup( *pPoOpcUa_Int ) )
							{
								for ( k = 0; k < 16; k++ )
								{
									Addr = pPoOpcUa_Int->wdInfoAddr+k;
									if ( Addr <= DI_END_ADDR )	//有效
									{
										Val = (pPoOpcUa_Int->sSCD.wdST>>k)&0X01;
										pDevice->SetVal( MACS_IEC104_DI, Addr, &Val, 1, 0, true );
									}
								}
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_ND_1:		//测量值, 不带品质描述词的规一化值
				{
					pDriver->WriteLog( "测量值, 不带品质描述词的规一化值" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structRT16bitNoQDSNoInfoAddr* pPoOpcUa_Int = (structRT16bitNoQDSNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structRT16bitNoQDSNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( pStartInfoAddr->wdInfoAddr+i >= AI_START_ADDR && pStartInfoAddr->wdInfoAddr+i <= AI_END_ADDR  )	//有效
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_AI, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bitNoQDS* pPoOpcUa_Int = (structRT16bitNoQDS* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRT16bitNoQDS)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= AI_START_ADDR && pPoOpcUa_Int->wdInfoAddr <= AI_END_ADDR  )	//有效
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_AI, pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_SP_TB_1:		//带时标CP56Time2a的单点信息
				{
					pDriver->WriteLog( "带时标CP56Time2a的单点信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structSDI_TimeNoInfoAddr* pPoOpcUa_Int = (structSDI_TimeNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structSDI_TimeNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}
						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( pPoOpcUa_Int->sSDINoTime ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSDINoTime.sSIQ.SPI;
								pDevice->SetVal( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, &OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structSDI_Time* pPoOpcUa_Int = (structSDI_Time* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structSDI_Time)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( pPoOpcUa_Int->sSDINoTime ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSDINoTime.sSIQ.SPI;
								pDevice->SetVal( MACS_IEC104_DI, pPoOpcUa_Int->sSDINoTime.wdInfoAddr, &OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_DP_TB_1:		//带时标CP56Time2a的双点信息
				{
					pDriver->WriteLog( "带时标CP56Time2a的双点信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structDDI_TimeNoInfoAddr* pPoOpcUa_Int = (structDDI_TimeNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structDDI_TimeNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( pPoOpcUa_Int->sDDINoTime ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDDINoTime.sDIQ.DPI;
								pDevice->SetVal( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, &OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDDI_Time* pPoOpcUa_Int = (structDDI_Time* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structDDI_Time)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( pPoOpcUa_Int->sDDINoTime ) )	//有效
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDDINoTime.sDIQ.DPI;
								pDevice->SetVal( MACS_IEC104_DI, pPoOpcUa_Int->sDDINoTime.wdInfoAddr, &OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ST_TB_1:		//带时标的步位置信息
				{
					pDriver->WriteLog( "带时标的步位置信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structVTI_TimeNoInfoAddr* pPoOpcUa_Int = (structVTI_TimeNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structVTI_TimeNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( pPoOpcUa_Int->ssVTI ) )	//有效
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->ssVTI.sVTI);
								pDevice->SetVal( MACS_IEC104_BWZ, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structVTI_Time* pPoOpcUa_Int = (structVTI_Time* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structVTI_Time)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( pPoOpcUa_Int->ssVTI ) )	//有效
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->ssVTI.sVTI);
								pDevice->SetVal( MACS_IEC104_BWZ, pPoOpcUa_Int->ssVTI.wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_BO_TB_1:		//带时标CP56Time2a的32比特串
				{
					pDriver->WriteLog( "带时标CP56Time2a的32比特串" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structRTS_TimeNoInfoAddr* pPoOpcUa_Int = (structRTS_TimeNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structRTS_TimeNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}


						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( pPoOpcUa_Int->sRTS ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRTS.dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRTS_Time* pPoOpcUa_Int = (structRTS_Time* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRTS_Time)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( pPoOpcUa_Int->sRTS ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRTS.dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, pPoOpcUa_Int->sRTS.wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TD_1:		//测量值, 带时标CP56Time2a的规一化值
				{
					pDriver->WriteLog( "测量值, 带时标CP56Time2a的规一化值" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bit_TimeNoInfoAddr* pPoOpcUa_Int = (structRT16bit_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) 
							+ sizeof(ASDUHead) + sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bit_TimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sRT.sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit_Time* pPoOpcUa_Int = (structRT16bit_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRT16bit_Time)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sRT.sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->sRT.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TE_1:		//测量值, 带时标CP56Time2a的标度化值
				{
					pDriver->WriteLog( "测量值, 带时标CP56Time2a的标度化值" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bit_TimeNoInfoAddr* pPoOpcUa_Int = (structRT16bit_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead)
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bit_TimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit_Time* pPoOpcUa_Int = (structRT16bit_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRT16bit_Time)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->sRT.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TF_1:		//测量值, 带时标CP56Time2a的短浮点数
				{
					pDriver->WriteLog( "测量值, 带时标CP56Time2a的短浮点数" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT32bit_TimeNoInfoAddr* pPoOpcUa_Int 
							= (structRT32bit_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT32bit_TimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.fValue;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT32bit_Time* pPoOpcUa_Int = (structRT32bit_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRT32bit_Time)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( pPoOpcUa_Int->sRT ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.fValue;
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->sRT.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_IT_TB_1:		//带时标CP56Time2a的累计量
				{
					pDriver->WriteLog( "带时标CP56Time2a的累计量" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structDNLJ_TimeNoInfoAddr* pPoOpcUa_Int = (structDNLJ_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structDNLJ_TimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( pPoOpcUa_Int->sDNLJ ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sDNLJ.Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDNLJ_Time* pPoOpcUa_Int = (structDNLJ_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structDNLJ_Time)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( pPoOpcUa_Int->sDNLJ ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sDNLJ.Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, 
									pPoOpcUa_Int->sDNLJ.wdInfoAddr,  
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EP_TD_1:		//带时标CP56Time2a的继电保护装置事件
				{
					pDriver->WriteLog( "带时标CP56Time2a的继电保护装置事件" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structJDBH_TimeNoInfoAddr* pPoOpcUa_Int = (structJDBH_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structJDBH_TimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}
						OpcUa_Int64 lCurTime;
						lCurTime = ASDUTIME2MACSINT64(pPoOpcUa_Int->sTime);
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBH( pPoOpcUa_Int->sJDBHNoTime ) )	//有效
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, 
								//	pStartInfoAddr->wdInfoAddr+i, 
								//	(OpcUa_Int32)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES, 
								//	ASDUTimeToSystime( 0/*pPoOpcUa_Int->sTime*/ ) );

								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sJDBHNoTime.sSEP.ES;
								pDevice->SetVal( MACS_IEC104_JDBH, 
									pStartInfoAddr->wdInfoAddr+i, &OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structJDBH_Time* pPoOpcUa_Int = (structJDBH_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structJDBH_Time)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						OpcUa_Int64 lCurTime;
						lCurTime = ASDUTIME2MACSINT64(pPoOpcUa_Int->sTime);
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBH( pPoOpcUa_Int->sJDBHNoTime ) )	//有效
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, 
								//	pPoOpcUa_Int->sJDBHNoTime.wdInfoAddr, 
								//	(OpcUa_Int32)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES, 
								//	ASDUTimeToSystime( 0/*pPoOpcUa_Int->sTime*/ ) );
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sJDBHNoTime.sSEP.ES;
								pDevice->SetVal( MACS_IEC104_JDBH, 
									pPoOpcUa_Int->sJDBHNoTime.wdInfoAddr, 
									&OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EP_TF_1:		//带时标CP56Time2a的继电保护装置成组输出电路信息
				{
					pDriver->WriteLog( "带时标CP56Time2a的继电保护装置成组输出电路信息" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structJDBHGroupOutput_TimeNoInfoAddr* pPoOpcUa_Int 
							= (structJDBHGroupOutput_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structJDBHGroupOutput_TimeNoInfoAddr)
							+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) 
							!= m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupOutput( pPoOpcUa_Int->sJDBHGroupOutputNoTime ) )	//有效
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, 
								//	pStartInfoAddr->wdInfoAddr+i, 
								//	(OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupOutputNoTime.byVal, 
								//	ASDUTimeToSystime( 0/*pPoOpcUa_Int->sTime*/ ) );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structJDBHGroupOutput_Time* pPoOpcUa_Int = (structJDBHGroupOutput_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structJDBHGroupOutput_Time)+sizeof(APCI)
							+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupOutput( pPoOpcUa_Int->sJDBHGroupOutputNoTime ) )	//有效
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, 
								//	pPoOpcUa_Int->sJDBHGroupOutputNoTime.wdInfoAddr, 
								//	(OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupOutputNoTime.byVal, 
								//	ASDUTimeToSystime( 0/*pPoOpcUa_Int->sTime*/ ) );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SC_NA_1:		//单点遥控命令
			case C_DC_NA_1:		//双点遥控命令
			case C_RC_NA_1:		//升降命令
				{
					if ( pHead->byType == C_SC_NA_1 )
					{
						sprintf( strLog, "单点遥控命令, 传输原因: %d", pHead->byTxReason );
					}
					else if ( pHead->byType == C_DC_NA_1 )
					{
						sprintf( strLog, "双点遥控命令, 传输原因: %d", pHead->byTxReason );
					}
					else
					{
						sprintf( strLog, "升降命令, 传输原因: %d", pHead->byTxReason );
					}
					pDriver->WriteLog( strLog );

					unionInfoElement* pPoOpcUa_Int = (unionInfoElement* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(unionInfoElement)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= DO_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= DO_END_ADDR  )	//有效
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//置返信点为1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SE_NA_1:		//设定命令, 规一化值
			case C_SE_NB_1:		//设定命令, 标度化值
				{
					if ( pHead->byType == C_SE_NA_1 )
					{
						sprintf( strLog, "设定命令, 规一化值, 传输原因: %d", 
							pHead->byTxReason );
					}
					else if ( pHead->byType == C_SE_NA_1 )
					{
						sprintf( strLog, "设定命令, 标度化值, 传输原因: %d", 
							pHead->byTxReason );
					}
					pDriver->WriteLog( strLog );

					structRC16bit* pPoOpcUa_Int = (structRC16bit* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC16bit)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= AI_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= AI_END_ADDR  )	//有效
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//置返信点为1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SE_NC_1:		//设定命令, 短浮点数
				{
					sprintf( strLog, "设定命令, 短浮点数, 传输原因: %d", 
						pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structRC32bit* pPoOpcUa_Int = (structRC32bit* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC32bit)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							//	if ( pPoOpcUa_Int->wdInfoAddr >= AI_START_ADDR 
							//		&& pPoOpcUa_Int->wdInfoAddr <= AI_END_ADDR  )	//有效
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//置返信点为1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_BO_NA_1:		//设定命令, 32比特串
				{
					sprintf( strLog, "设定命令, 32比特串, 传输原因: %d", 
						pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structRC32bitstring* pPoOpcUa_Int = (structRC32bitstring* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC32bitstring)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= RTS_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= RTS_END_ADDR  )	//有效
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//置返信点为1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SC_TA_1:		//带时标CP56Time2a的单命令
			case C_DC_TA_1:		//带时标CP56Time2a的双命令
			case C_RC_TA_1:		//带时标CP56Time2a的升降命令
				{
					if ( pHead->byType == C_SC_TA_1 )
					{
						sprintf( strLog, "带时标CP56Time2a的单命令, 传输原因: %d", 
							pHead->byTxReason );
					}
					else if ( pHead->byType == C_DC_TA_1 )
					{
						sprintf( strLog, "带时标CP56Time2a的双命令, 传输原因: %d", 
							pHead->byTxReason );
					}
					else
					{
						sprintf( strLog, "带时标CP56Time2a的升降命令, 传输原因: %d", 
							pHead->byTxReason );
					}
					pDriver->WriteLog( strLog );

					unionInfoElementTime* pPoOpcUa_Int = (unionInfoElementTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(unionInfoElementTime)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= DO_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= DO_END_ADDR  )	//有效
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//置返信点为1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SE_TA_1:		//带时标CP56Time2a的设定值命令,规一化值
			case C_SE_TB_1:		//带时标CP56Time2a的设定值命令,标度化值
				{
					if ( pHead->byType == C_SE_NA_1 )
					{
						sprintf( strLog, 
							"带时标CP56Time2a的设定值命令, 规一化值, 传输原因: %d", 
							pHead->byTxReason );
					}
					else if ( pHead->byType == C_SE_NA_1 )
					{
						sprintf( strLog, 
							"带时标CP56Time2a的设定值命令, 标度化值, 传输原因: %d", 
							pHead->byTxReason );
					}
					pDriver->WriteLog( strLog );

					structRC16bitTime* pPoOpcUa_Int = (structRC16bitTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC16bitTime)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= AI_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= AI_END_ADDR  )	//有效
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//置返信点为1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SE_TC_1:		//带时标CP56Time2a的设定值命令,短浮点数
				{
					sprintf( strLog,"带时标CP56Time2a的设定值命令, 短浮点数, 传输原因: %d", 
						pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structRC32bitTime* pPoOpcUa_Int = (structRC32bitTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC32bitTime)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							//	if ( pPoOpcUa_Int->wdInfoAddr >= AI_START_ADDR && pPoOpcUa_Int->wdInfoAddr <= AI_END_ADDR  )	//有效
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//置返信点为1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_BO_TA_1:		//带时标CP56Time2a的32比特串
				{
					sprintf( strLog, "带时标CP56Time2a的32比特串, 传输原因: %d", 
						pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structRC32bitstringTime* pPoOpcUa_Int = (structRC32bitstringTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC32bitstringTime)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= RTS_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= RTS_END_ADDR  )	//有效
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//置返信点为1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EI_NA_1:		//初始化结束
				{
					structInit* pPoOpcUa_Int = (structInit* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					sprintf( strLog, "初始化结束(COI=%d, BS=%d)", 
						pPoOpcUa_Int->sCOI.COI, pPoOpcUa_Int->sCOI.BS );
					pDriver->WriteLog( strLog );

					nNum = pHead->byLimit;
					if ( nNum*sizeof(structInit)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误A" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( nNum != 1 || pHead->byTxReason != CAUSE_init )
					{
						pDriver->WriteLog( "帧有误B" );
						m_bExtractError = true;	//解析有误
						break;
					}

					switch( pPoOpcUa_Int->sCOI.COI ) 
					{
					case 0:		//当地电源合上
						break;
					case 1:		//当地手动复位
						break;
					case 2:		//远方复位
						break;
					default:
						break;
					}

					if ( pPoOpcUa_Int->sCOI.BS )	//改变当地参数后的初始化
					{
					}
					else	//未改变当地参数的初始化
					{
					}
				}
				break;
			case C_IC_NA_1:		//召唤命令
				{
					sprintf( strLog, "召唤命令, 传输原因: %d", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structWholeQuery* pPoOpcUa_Int = (structWholeQuery* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structWholeQuery)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误A" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "帧有误B" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actterm 
						|| pHead->byTxReason == CAUSE_daactcon )	//激活结束(总召唤结束)。可以开始下一次总召唤了。
					{
						//根据地址找到对应的逻辑单元号
						CFepIEC104Device* pDevice = dynamic_cast<CFepIEC104Device*> (
							pDriver->GetDeviceByAddr( (OpcUa_Int32)pHead->wdCommAddr ));
						if ( pDevice )
						{
							//		pDevice->NeedWholeQuery(true);
							pDevice->WholeQuerying(false);		//释放独占标志
						}
					}
				}
				break;
			case C_CI_NA_1:		//计数量召唤命令
				{
					sprintf( strLog, "计数量召唤命令, 传输原因: %d", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structCountQuery* pPoOpcUa_Int = (structCountQuery* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structCountQuery)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误A" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "帧有误B" );
						m_bExtractError = true;	//解析有误
						break;
					}

					//激活结束。可以开始下一次计数量召唤了。
					if ( pHead->byTxReason == CAUSE_actterm )	
					{
						//根据地址找到对应的逻辑单元号
						CFepIEC104Device* pDevice = dynamic_cast<CFepIEC104Device*>( 
							pDriver->GetDeviceByAddr( (OpcUa_Int32)pHead->wdCommAddr ) );
						if ( pDevice )
						{
							//		pDevice->NeedCountQuery(true);
							pDevice->CountQuerying(false);		//释放独占标志
						}
					}
				}
				break;
			case C_RD_NA_1:		//读命令
				sprintf( strLog, "读命令" );
				pDriver->WriteLog( strLog );

				break;
			case C_CS_NA_1:		//时钟同步命令
				{
					sprintf( strLog, "校时(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structAdjustTime* pPoOpcUa_Int = (structAdjustTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

					nNum = pHead->byLimit;
					if ( nNum*sizeof(structAdjustTime)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误A" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "帧有误B" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_actterm )	//校时成功！
					{
						//根据地址找到对应的逻辑单元号
						CFepIEC104Device* pDevice = dynamic_cast<CFepIEC104Device*>( 
							pDriver->GetDeviceByAddr( (OpcUa_Int32)pHead->wdCommAddr ) );
						if ( pDevice )
						{
							pDevice->IsCheckTiming(false);		//释放独占标志
						}
					}
				}
				break;
			case C_TS_NA_1:		//测试命令
				{
					sprintf( strLog, "测试(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structTest* pPoOpcUa_Int = (structTest* )(m_sPacketBuffer 
						+ sizeof(APCI) + sizeof(ASDUHead) );

					nNum = pHead->byLimit;
					if ( nNum*sizeof(structTest)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误A" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 || pPoOpcUa_Int->wdFBP != 0x55AA )
					{
						pDriver->WriteLog( "帧有误B" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon )	//测试成功！
					{

					}
				}
				break;
			case C_RP_NA_1:		//复位进程命令
				{
					sprintf( strLog, "复位进程(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structResetProc* pPoOpcUa_Int = (structResetProc* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structResetProc)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误A" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "帧有误B" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon )	//复位进程成功！
					{

					}
				}
				break;
			case C_CD_NA_1:		//延时获得命令
				{
					sprintf( strLog, "延时获得命令(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structDelay* pPoOpcUa_Int = (structDelay* )(m_sPacketBuffer 
						+ sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structDelay)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误A" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "帧有误B" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon )	//成功！
					{

					}
				}
				break;
			case C_TS_TA_1:		//带时标CP56Time2a的测试命令
				{
					sprintf( strLog, "带时标的测试(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structTestTime* pPoOpcUa_Int = (structTestTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structTestTime)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误A" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 || pPoOpcUa_Int->wdFBP != 0x55AA )
					{
						pDriver->WriteLog( "帧有误B" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon )	//测试成功！
					{

					}
				}
				break;
			case P_ME_NA_1:		//测量值参数, 归一化值
				{
					pDriver->WriteLog( "测量值参数, 归一化值" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structPara16bitNoInfoAddr* pPoOpcUa_Int = (structPara16bitNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structPara16bitNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara16bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_CS, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structPara16bit* pPoOpcUa_Int = (structPara16bit* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structPara16bit)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara16bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_CS, 
									pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case P_ME_NB_1:		//测量值参数,  标度化值
				{
					pDriver->WriteLog( "测量值参数, 标度化值" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structPara16bitNoInfoAddr* pPoOpcUa_Int = (structPara16bitNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structPara16bitNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara16bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sSVA.Val;
								pDevice->SetVal( MACS_IEC104_CS, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structPara16bit* pPoOpcUa_Int = (structPara16bit* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structPara16bit)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara16bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->sSVA.Val;
								pDevice->SetVal( MACS_IEC104_CS, 
									pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case P_ME_NC_1:		//测量值参数, 短浮点数
				{
					pDriver->WriteLog( "测量值参数, 短浮点数" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//信息对象地址连续
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structPara32bitNoInfoAddr* pPoOpcUa_Int = (structPara32bitNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) 
							+ sizeof(ASDUHead) + sizeof(structInfoAddr) );

						if ( nNum*sizeof(structPara32bitNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) 
							!= m_wdValidDataLen )	//长度非法
						{
							m_bExtractError = true;	//解析有误
							pDriver->WriteLog( "帧有误" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara32bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->fValue;
								pDevice->SetVal( MACS_IEC104_CS, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structPara32bit* pPoOpcUa_Int = (structPara32bit* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structPara32bit)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//长度非法
						{
							pDriver->WriteLog( "帧有误" );
							m_bExtractError = true;	//解析有误
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara32bit( *pPoOpcUa_Int ) )	//有效
							{
								ftempVal = (float)pPoOpcUa_Int->fValue;
								pDevice->SetVal( MACS_IEC104_CS, 
									pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case P_AC_NA_1:		//参数激活
				{
					sprintf( strLog, "参数激活(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structParaAct* pPoOpcUa_Int = (structParaAct* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structParaAct)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//长度非法
					{
						pDriver->WriteLog( "帧有误A" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "帧有误B" );
						m_bExtractError = true;	//解析有误
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon )	//成功！
					{
					}
				}
				break;
			case F_FR_NA_1:		//文件准备就绪
				break;
			case F_SR_NA_1:		//节准备就绪
				break;
			case F_SC_NA_1:		//召唤目录, 选择文件, 召唤文件,召唤节
				break;
			case F_LS_NA_1:		//最后的节,最后的段
				break;
			case F_AF_NA_1:		//认可文件, 认可节
				break;
			case F_SG_NA_1:		//段
				break;
			case F_DR_TA_1:		//目录
				break;
			default:
				break;
			}

			//有DI或DI2的变位信息
			if ( iCount > 0 )
			{
				std::string szSndPacket = GetCommData( m_sPacketBuffer, m_wdValidDataLen, 1 );
				snprintf( strLog, sizeof(strLog),"接收变位帧，变位点数%d，共%d字节: %s", iCount, m_wdValidDataLen, szSndPacket.c_str() );
				pDriver->WriteLogAnyWay( strLog );
			}

			if ( !m_bExtractError )	//解析无误
			{
				if( bFromLocal )
				{
					//		pDevice->SetNr( wdNr );	//更新接收序号
					//		pDevice->SetNs( wdNs ); //更新发送序号

					//设置装置在线
					{
						pDevice->GenNextNr();

						//适时发送S帧
						m_bNeedSendSFrame = pDevice->IsNeedSendSFrame();

						pDevice->SetOnLine( DEVICE_ONLINE );	//主从设备收到I帧一律认为在线
						pDevice->m_byTxTimes = 0;
						pDevice->m_byRxTimes++;
					}

					//复位连接标志
					m_bNeedConnected = false;
				}
			}
		}
		else	//如果设备处于模拟态，则总是更新接收序号。但可能有误！
		{
			if( bFromLocal )
			{
				//	pDevice->SetNr( wdNr );	//更新接收序号
				//	pDevice->SetNs( wdNs ); //更新发送序号

				//设置装置在线
				{
					pDevice->GenNextNr();

					//适时发送S帧
					m_bNeedSendSFrame = pDevice->IsNeedSendSFrame();

					pDevice->SetOnLine( DEVICE_ONLINE );	//主从设备收到I帧一律认为在线
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes++;
				}

				//复位连接标志
				m_bNeedConnected = false;
			}
		}		
	}

	//! 连接成功
	OpcUa_Int32 CFepIEC104Prtcl::OnConnSuccess(std::string pPeerIP )
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		//! 连接锁
		//ACE_Guard<ACE_Recursive_Thread_Mutex> guard( pDriver->m_Mutex );

		m_bNeedConnected = false;
		
		pDriver->Connected(true);

		//! 关闭连接超时定时器
		pDriver->KillConnTimer();

		//! 日志
		char pchInfo[256];
		sprintf( pchInfo, "连接服务器%s成功...", pPeerIP.c_str() );
		pDriver->WriteLogAnyWay( pchInfo );
		
		pDriver->IsConnecting(false);	

		//! 启动周期定时器
/*
		m_pPeriodTimer->KillTimer();
		m_pPeriodTimer->SetTimer( PERIOD_SCAN_MS );
*/

		//连接成功后，先预置StartDT标志，但是否真正发送StartDT帧，还需要看设备的主从状态。
		//这里必须预置！
		ForStartDT();

		//发送和接收序号重新归零。Add by hxw in 2014.5.4
		ResetAllDeviceNs();
		ResetAllDeviceNr();


/*	
		for(OpcUa_Int32 j=0;j<pDriver->GetDeviceCount();j++)
		{
			//! 置所有设备在线
			pDriver->GetDeviceByIndex(j)->SetOnLine( DEVICE_ONLINE );   
		}	
*/

		m_nOfflineCount = 0;	//连接成后重新开始新一轮离线诊断
		m_iPeriodCount  = 0;	//连接成后立即准备发送各类主站报文，如S帧。

		//////////////////////////////////////////////////////////////////////////////
		//! 发送数据
		//OpcUa_Int32 nDataLen = 0;
		BuildRequest( m_pbySndBuffer, m_nSndLen );
		//////////////////////////////////////////////////////////////////////////////
		return 0;
	}

	// *****************************************************
	// 函数功能: 构造本地链路在线时的本地下行帧(不含遥控)
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//    OpcUa_Byte* pBuf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Prtcl::BuildLocalNormalFrame( OpcUa_Byte* pBuf )
	{
		if ( !pBuf )
		{
			return 0;
		}

		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		if ( m_bNeedStopDTcon )			//需要发送STOPDT确认帧
		{
			m_bNeedStopDTcon = false;
			pDriver->WriteLogAnyWay( "Send STOPDTcon Frame!" );
			return CFepIEC104Device::BuildStopFrameCON( pBuf );
		}

		if ( m_bNeedStartDTcon )		//需要发送STARTDT确认帧
		{
			pDriver->WriteLogAnyWay( "Send STARTDTcon Frame!" );
			m_bNeedStartDTcon = false;
			return CFepIEC104Device::BuildStartFrameCON( pBuf );
		}

		if ( m_bNeedSendTestcon )		//需要发送TESTFR确认帧
		{
			pDriver->WriteLog( "Send TESTFRcon Frame!" );
			m_bNeedSendTestcon = false;
			return CFepIEC104Device::BuildTestFrameCON( pBuf );
		}

		if ( m_bNeedStopDT )			//需要发送STOPDT帧
		{
			//控制连续发送StopDT的频率
			ACE_Time_Value tv(0,500000);	
			ACE_OS::sleep(tv);
	
			m_bNeedStopDT = false;
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send STOPDT Frame!" ) ));
			pDriver->WriteLogAnyWay( "Send STOPDT Frame!" );
			return CFepIEC104Device::BuildStopFrame( pBuf );
		}

		if ( m_bNeedStartDT )			//需要发送STARTDT帧
		{//注意: 此处不复位m_bNeedStartDT标志,收到响应帧才复位.
			//控制连续发送StartDT的频率
			ACE_Time_Value tv(0,500000);	
			ACE_OS::sleep(tv);

			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "------------------CFepIEC104Prtcl Send STARTDT Frame------------------!" ) ));
			pDriver->WriteLogAnyWay( "Send STARTDT Frame!" );
			return CFepIEC104Device::BuildStartFrame( pBuf );
		}

		if ( m_bNeedSendTest )			//需要发送TESTFR生效帧
		{
			pDriver->WriteLog( "Send TESTFR生效 Frame!" );
			m_bNeedSendTest = false;
			return CFepIEC104Device::BuildTestFrame( pBuf );
		}

		CFepIEC104Device* pDevice = dynamic_cast<CFepIEC104Device*>( 
			pDriver->GetDeviceByIndex(m_shAdiLogical) );

		if ( m_bNeedSendSFrame )	//需要发送S格式帧
		{
			pDriver->WriteLog( "Send S Frame!" );
			m_bNeedSendSFrame = false;
			//复位w计数
			pDevice->ResetCtrlAreaINum();
			return pDevice->BuildSFrame( pBuf );
		}

		if ( pDevice->IsTimeForCheckTime() )	//需要校时了
		{
			pDriver->WriteLogAnyWay( "Send CheckTime Frame!" );
			pDevice->IsCheckTiming(true);	//置独占标志
			pDevice->IsCheckTiming(false);//hbb由于校时帧没有应用故在组完后立即清理掉 说明3.1子站回校时帧的公共地址为0 置独占标志
			/*!
			*	保存设备号，以备TimeOut时释放独占标志。
			*	注意：不能用m_shAdiLogical，后者在遥控或遥调后会变更。
			*/
			m_lastDevNo = m_shAdiLogical;		
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send CheckTime Frame!" ) ));
			return pDevice->BuildCheckTimeFrame( pBuf );
		}

		if ( pDevice->IsTimeForWholeQuery() )	//需要总查询了
		{
			pDriver->WriteLog( "Send WholeQuery Frame!" );
			pDevice->WholeQuerying(true);	//置独占标志
			
			/*! 
			 *	保存设备号，以备TimeOut时释放独占标志。
			 *	注意：不能用m_shAdiLogical，后者在遥控或遥调后会变更。
			 */
			m_lastDevNo = m_shAdiLogical;		
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send WholeQuery Frame!" ) ));
			return pDevice->BuildWholeQueryFrame( pBuf );
		}

		if ( pDevice->IsTimeForCountQuery() )	//需要计数量查询了
		{
			pDriver->WriteLog( "Send CountQuery Frame!" );
			pDevice->CountQuerying(true);	//置独占标志
			
			/*! 
			 *	保存设备号，以备TimeOut时释放独占标志。
			 *	注意：不能用m_shAdiLogical，后者在遥控或遥调后会变更。
			 */
			m_lastDevNo = m_shAdiLogical;		

			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send CountQuery Frame!" ) ));
			return pDevice->BuildCountQueryFrame( pBuf );
		}

		//空闲时发送S格式帧
		//return pDevice->BuildSFrame( pBuf );

		return 0;
	}

	// *****************************************************
	// 函数功能: 构造本地链路在线时的本地从机下行帧(不含遥控)
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//    OpcUa_Byte* pBuf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Prtcl::BuildLocalStandbyFrame( OpcUa_Byte* pBuf )
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		pDriver->WriteLog( "BuildLocalStandbyFrame: start...." );

		if ( !pBuf )
		{
			pDriver->WriteLog( "BuildLocalStandbyFrame: pBuf=NULL." );
			return 0;
		}


		if ( m_bNeedStopDTcon )			//需要发送STOPDT确认帧
		{
			m_bNeedStopDTcon = false;
			pDriver->WriteLogAnyWay( "Send STOPDTcon Frame!" );
			return CFepIEC104Device::BuildStopFrameCON( pBuf );
		}

		if ( m_bNeedStartDTcon )		//需要发送STARTDT确认帧
		{
			pDriver->WriteLogAnyWay( "Send STARTDTcon Frame!" );
			m_bNeedStartDTcon = false;
			return CFepIEC104Device::BuildStartFrameCON( pBuf );
		}

		if ( m_bNeedSendTestcon )		//需要发送TESTFR确认帧
		{
			pDriver->WriteLog( "Send TESTFRcon Frame!" );
			m_bNeedSendTestcon = false;
			return CFepIEC104Device::BuildTestFrameCON( pBuf );
		}

		if ( m_bNeedStopDT )			//需要发送STOPDT帧
		{
			m_bNeedStopDT = false;
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send STOPDT Frame!" ) ));
			pDriver->WriteLogAnyWay( "Send STOPDT Frame!" );
			return CFepIEC104Device::BuildStopFrame( pBuf );
		}

		if ( m_bNeedSendTest )			//需要发送TESTFR生效帧
		{
			pDriver->WriteLog( "Send TESTFR生效帧 Frame!" );
			m_bNeedSendTest = false;
			return CFepIEC104Device::BuildTestFrame( pBuf );
		}

		CFepIEC104Device* pDevice = dynamic_cast<CFepIEC104Device*>( 
			pDriver->GetDeviceByIndex(m_shAdiLogical) );

		pDriver->WriteLog( "BuildLocalStandbyFrame: check SFrame flag...." );
		if ( m_bNeedSendSFrame )	//需要发送S格式帧
		{
			pDriver->WriteLog( "Send S Frame!" );
			m_bNeedSendSFrame = false;
			//复位w计数
			pDevice->ResetCtrlAreaINum();
			return pDevice->BuildSFrame( pBuf );
		}

		return 0;
	}

	// *****************************************************
	// 函数功能: 验证是否为有效IP
	// 返 回 值: bool : 
	// 参数说明: 
	//    char* ip: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidIP( const char* ip )
	{
		OpcUa_Int32 iPos = 0;
		OpcUa_Int32 iSegPos = 0;
		OpcUa_Int32 iSegCnt = 0; //分割符号个数
		OpcUa_Int32 IPArea = 0;  //整数类型的每段地址

		char temp[4];
		memset(&temp,0,4);

		for( iPos=0; ip[iPos] != '\0'; iPos++,iSegPos++ )
		{
			if ('.' == ip[iPos])
			{
				iSegCnt++;
				iPos++;
				IPArea = atoi(temp);				
				if ( IPArea < 0 || IPArea > 255 )
				{
					return false;
				}
				memset(&temp,0,4);
				iSegPos = 0;
			}

			temp[iSegPos] = ip[iPos];

			if (iSegPos > 2 || iSegCnt > 3) //IPV4 每段最多为三位数（下标0开始） ，有三个分割符号，//
			{
				return false;
			}
		}

		return true;
	}

	// *****************************************************
	// 函数功能: 所有设备接收序号重新归零
	// 算法思想: 
	// 调用模块: 
	// 返 回 值: void : 该函数不返回任何值
	// *****************************************************
	void CFepIEC104Prtcl::ResetAllDeviceNr()
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		CFepIEC104Device* pDevice = NULL;
		for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC104Device*>( pDriver->GetDeviceByIndex(i) );
			if( pDevice )
			{
				pDevice->ResetNr();
			}
		}
	}



	// *****************************************************
	// 函数功能: 所有设备发送序号重新归零
	// 返 回 值: void : 该函数不返回任何值
	// *****************************************************
	void CFepIEC104Prtcl::ResetAllDeviceNs()
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		CFepIEC104Device* pDevice = NULL;
		for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC104Device*>( pDriver->GetDeviceByIndex(i) );
			if( pDevice )
			{
				pDevice->ResetNs();
			}
		}
	}

	// *****************************************************
	// 函数功能: 是否t0间隔到了
	// 返 回 值: bool : 
	// *****************************************************
/*
	bool CFepIEC104Prtcl::IsTimeForConnectEx()
	{
		if ( m_bNeedConnected )
		{
			m_bNeedConnected = false;

			return true;
		}

		return false;
	}
*/




	// *****************************************************
	// 函数功能: 验证不带时标的单点信息SPI有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structSDINoTime& SPI: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidSDI( structSDINoTime& SDI )
	{
		if ( IsCheckSDI() )
		{
			return ( !SDI.sSIQ.BL && !SDI.sSIQ.IV 
				&& !SDI.sSIQ.NT && !SDI.sSIQ.Res && !SDI.sSIQ.SB 
				&& SDI.wdInfoAddr >= DI_START_ADDR && SDI.wdInfoAddr <= DI_END_ADDR );
		}
		else
		{
		return ( /*!SDI.sSIQ.BL &&*/ !SDI.sSIQ.IV 
			/*&& !SDI.sSIQ.NT && !SDI.sSIQ.Res && !SDI.sSIQ.SB*/ 
			&& SDI.wdInfoAddr >= DI_START_ADDR && SDI.wdInfoAddr <= DI_END_ADDR );
		}
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDINoTimeNoInfoAddr& SDI )
	{
		if ( IsCheckSDI() )
		{
			return ( !SDI.sSIQ.BL && !SDI.sSIQ.IV 
				&& !SDI.sSIQ.NT && !SDI.sSIQ.Res && !SDI.sSIQ.SB 
				);
		}
		else
		{
		return ( /*!SDI.sSIQ.BL &&*/ !SDI.sSIQ.IV 
			/*&& !SDI.sSIQ.NT && !SDI.sSIQ.Res && !SDI.sSIQ.SB*/ 
			);
		}
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDI_Time24NoInfoAddr& SDI )
	{
		if ( IsCheckSDI() )
		{
			return ( !SDI.sSDINoTime.sSIQ.BL && !SDI.sSDINoTime.sSIQ.IV 
				&& !SDI.sSDINoTime.sSIQ.NT && !SDI.sSDINoTime.sSIQ.Res && !SDI.sSDINoTime.sSIQ.SB );
		}
		else
		{
		return ( /*!SDI.sSDINoTime.sSIQ.BL &&*/ !SDI.sSDINoTime.sSIQ.IV 
			/*&& !SDI.sSDINoTime.sSIQ.NT && !SDI.sSDINoTime.sSIQ.Res && !SDI.sSDINoTime.sSIQ.SB*/ );
		}
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDINoTimeNoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidSDI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDINoTime* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidSDI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidSDI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidSDI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDI_Time24& SDI )
	{
		if ( IsCheckSDI() )
		{
			return ( !SDI.sSDINoTime.sSIQ.BL && !SDI.sSDINoTime.sSIQ.IV 
				&& !SDI.sSDINoTime.sSIQ.NT && !SDI.sSDINoTime.sSIQ.Res && !SDI.sSDINoTime.sSIQ.SB 
				&& SDI.sSDINoTime.wdInfoAddr >= DI_START_ADDR && SDI.sSDINoTime.wdInfoAddr <= DI_END_ADDR );
		}
		else
		{
			return ( /*!SDI.sSDINoTime.sSIQ.BL &&*/ !SDI.sSDINoTime.sSIQ.IV 
				/*&& !SDI.sSDINoTime.sSIQ.NT && !SDI.sSDINoTime.sSIQ.Res && !SDI.sSIQ.SB*/ 
				&& SDI.sSDINoTime.wdInfoAddr >= DI_START_ADDR && SDI.sSDINoTime.wdInfoAddr <= DI_END_ADDR );
		}
	}

	// *****************************************************
	// 函数功能: 验证不带时标的双点信息DPI有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structDDINoTime& DDI: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidDDI( structDDINoTime& DDI )
	{
		if ( IsCheckDDI() )
		{
			return ( !DDI.sDIQ.BL && !DDI.sDIQ.IV 
				&& !DDI.sDIQ.NT && !DDI.sDIQ.Res && !DDI.sDIQ.SB
				&& DDI.wdInfoAddr >= DI_START_ADDR && DDI.wdInfoAddr <= DI_END_ADDR );
		}
		else
		{
			return ( /*!DDI.sDIQ.BL &&*/ !DDI.sDIQ.IV 
				/*&& !DDI.sDIQ.NT && !DDI.sDIQ.Res && !DDI.sDIQ.SB*/
				&& DDI.wdInfoAddr >= DI_START_ADDR && DDI.wdInfoAddr <= DI_END_ADDR );
		}
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDINoTimeNoInfoAddr& DDI )
	{
		if ( IsCheckDDI() )
		{
			return ( !DDI.sDIQ.BL && !DDI.sDIQ.IV 
				&& !DDI.sDIQ.NT && !DDI.sDIQ.Res && !DDI.sDIQ.SB
				);
		}
		else
		{
		return ( /*!DDI.sDIQ.BL &&*/ !DDI.sDIQ.IV 
			/*&& !DDI.sDIQ.NT && !DDI.sDIQ.Res && !DDI.sDIQ.SB*/
			);
		}
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDINoTimeNoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidDDI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDINoTime* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidDDI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidDDI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidDDI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDI_Time24NoInfoAddr& DDI )
	{
		if ( IsCheckDDI() )
		{
			return ( !DDI.sDDINoTime.sDIQ.BL && !DDI.sDDINoTime.sDIQ.IV 
				&& !DDI.sDDINoTime.sDIQ.NT && !DDI.sDDINoTime.sDIQ.Res && !DDI.sDDINoTime.sDIQ.SB );
		}
		else
		{
			return ( /*!DDI.sDDINoTime.sDIQ.BL &&*/ !DDI.sDDINoTime.sDIQ.IV 
				/*&& !DDI.sDDINoTime.sDIQ.NT && !DDI.sDDINoTime.sDIQ.Res && !DDI.sDDINoTime.sDIQ.SB*/ );
		}
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDI_Time24& DDI )
	{
		if ( IsCheckDDI() )
		{
			return ( !DDI.sDDINoTime.sDIQ.BL && !DDI.sDDINoTime.sDIQ.IV 
				&& !DDI.sDDINoTime.sDIQ.NT && !DDI.sDDINoTime.sDIQ.Res && !DDI.sDDINoTime.sDIQ.SB
				&& DDI.sDDINoTime.wdInfoAddr >= DI_START_ADDR && DDI.sDDINoTime.wdInfoAddr <= DI_END_ADDR );
		}
		else
		{
			return ( /*!DDI.sDDINoTime.sDIQ.BL &&*/ !DDI.sDDINoTime.sDIQ.IV 
				/*&& !DDI.sDDINoTime.sDIQ.NT && !DDI.sDDINoTime.sDIQ.Res && !DDI.sDDINoTime.sDIQ.SB*/
				&& DDI.sDDINoTime.wdInfoAddr >= DI_START_ADDR && DDI.sDDINoTime.wdInfoAddr <= DI_END_ADDR);
		}
	}

	// *****************************************************
	// 函数功能: 验证步位置信息VTI有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structVTI& VTI: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidVTI( structVTI& VTI )
	{
		if ( IsCheckVTI() )
		{
		return ( !VTI.Quality.BL && !VTI.Quality.IV 
			&& !VTI.Quality.NT && !VTI.Quality.OV
			&& !VTI.Quality.Res && !VTI.Quality.SB && !VTI.sVTI.T 
			&& VTI.wdInfoAddr >= BWZ_START_ADDR && VTI.wdInfoAddr <= BWZ_END_ADDR  );
		}
		else
		{
		return ( /*!VTI.Quality.BL &&*/ !VTI.Quality.IV 
			/*&& !VTI.Quality.NT && !VTI.Quality.OV
			&& !VTI.Quality.Res && !VTI.Quality.SB && !VTI.sVTI.T*/ 
			&& VTI.wdInfoAddr >= BWZ_START_ADDR && VTI.wdInfoAddr <= BWZ_END_ADDR  );
		}
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTINoInfoAddr& VTI )
	{
		if ( IsCheckVTI() )
		{
		return ( !VTI.Quality.BL && !VTI.Quality.IV 
			&& !VTI.Quality.NT && !VTI.Quality.OV
			&& !VTI.Quality.Res && !VTI.Quality.SB && !VTI.sVTI.T 
			);
		}
		else
		{
		return ( /*!VTI.Quality.BL &&*/ !VTI.Quality.IV 
			/*&& !VTI.Quality.NT && !VTI.Quality.OV
			&& !VTI.Quality.Res && !VTI.Quality.SB && !VTI.sVTI.T*/ 
			);
		}
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTINoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidVTI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTI* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidVTI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidVTI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidVTI( *pPoOpcUa_Int ) )	//无效
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTI_Time24NoInfoAddr& VTI )
	{
		if ( IsCheckVTI() )
		{
		return ( !VTI.ssVTI.Quality.BL && !VTI.ssVTI.Quality.IV 
			&& !VTI.ssVTI.Quality.NT && !VTI.ssVTI.Quality.OV
			&& !VTI.ssVTI.Quality.Res && !VTI.ssVTI.Quality.SB && !VTI.ssVTI.sVTI.T 
			);
		}
		else
		{
		return ( /*!VTI.ssVTI.Quality.BL &&*/ !VTI.ssVTI.Quality.IV 
			/*&& !VTI.ssVTI.Quality.NT && !VTI.ssVTI.Quality.OV
			&& !VTI.ssVTI.Quality.Res && !VTI.ssVTI.Quality.SB && !VTI.ssVTI.sVTI.T*/ 
			);
		}
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTI_Time24& VTI )
	{
		if ( IsCheckVTI() )
		{
		return ( !VTI.ssVTI.Quality.BL && !VTI.ssVTI.Quality.IV 
			&& !VTI.ssVTI.Quality.NT && !VTI.ssVTI.Quality.OV
			&& !VTI.ssVTI.Quality.Res && !VTI.ssVTI.Quality.SB && !VTI.ssVTI.sVTI.T 
			&& VTI.ssVTI.wdInfoAddr >= BWZ_START_ADDR && VTI.ssVTI.wdInfoAddr <= BWZ_END_ADDR
			);
		}
		else
		{
		return ( /*!VTI.ssVTI.Quality.BL &&*/ !VTI.ssVTI.Quality.IV 
			/*&& !VTI.ssVTI.Quality.NT && !VTI.ssVTI.Quality.OV
			&& !VTI.ssVTI.Quality.Res && !VTI.ssVTI.Quality.SB && !VTI.ssVTI.sVTI.T*/ 
			&& VTI.ssVTI.wdInfoAddr >= BWZ_START_ADDR && VTI.ssVTI.wdInfoAddr <= BWZ_END_ADDR
			);
		}
	}
	// *****************************************************
	// 函数功能: 验证子站远程终端状态（32比特串）信息RTS有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structRTS& RTS: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidRTS( structRTS& RTS )
	{
		return( /*!RTS.Quality.BL &&*/ !RTS.Quality.IV 
			/*&& !RTS.Quality.NT && !RTS.Quality.OV
			&& !RTS.Quality.Res && !RTS.Quality.SB */
			&& RTS.wdInfoAddr >= RTS_START_ADDR && RTS.wdInfoAddr <= RTS_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidRTS( structRTSNoInfoAddr& RTS )
	{
		return( /*!RTS.Quality.BL &&*/ !RTS.Quality.IV 
			/*&& !RTS.Quality.NT && !RTS.Quality.OV
			&& !RTS.Quality.Res && !RTS.Quality.SB */
			);
	}



	// *****************************************************
	// 函数功能: 验证遥测量16位RT信息RTS有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structRT16bit& RT16: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidRT16bit( structRT16bit& RT16 )
	{
		return( /*!RT16.Quality.BL &&*/ !RT16.Quality.IV 
			/*&& !RT16.Quality.NT && !RT16.Quality.OV
			&& !RT16.Quality.Res && !RT16.Quality.SB*/ 
			&& RT16.wdInfoAddr >= AI_START_ADDR && RT16.wdInfoAddr <= AI_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidRT16bit( structRT16bitNoInfoAddr& RT16 )
	{
		return( /*!RT16.Quality.BL &&*/ !RT16.Quality.IV 
			/*&& !RT16.Quality.NT && !RT16.Quality.OV
			&& !RT16.Quality.Res && !RT16.Quality.SB*/ 
			);
	}


	// *****************************************************
	// 函数功能: 验证遥测量32位RT信息RTS有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structRT32bit& RT32: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidRT32bit( structRT32bit& RT32 )
	{
		return( /*!RT32.Quality.BL &&*/ !RT32.Quality.IV 
			/*&& !RT32.Quality.NT && !RT32.Quality.OV
			&& !RT32.Quality.Res && !RT32.Quality.SB */
			&& RT32.wdInfoAddr >= AI_START_ADDR && RT32.wdInfoAddr <= AI_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidRT32bit( structRT32bitNoInfoAddr& RT32 )
	{
		return( /*!RT32.Quality.BL &&*/ !RT32.Quality.IV 
			/*&& !RT32.Quality.NT && !RT32.Quality.OV
			&& !RT32.Quality.Res && !RT32.Quality.SB */
			);
	}



	// *****************************************************
	// 函数功能: 验证遥测量16位测量值参数有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structPara16bit& Para16: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidPara16bit( structPara16bit& Para16 )
	{
		return ( !Para16.sQPM.KPA 
			&& Para16.wdInfoAddr >= CS_START_ADDR 
			&& Para16.wdInfoAddr <= CS_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidPara16bit( structPara16bitNoInfoAddr& Para16 )
	{
		return ( !Para16.sQPM.KPA );
	}


	// *****************************************************
	// 函数功能: 验证遥测量32位测量值参数有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structPara32bit& Para32: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidPara32bit( structPara32bit& Para32 )
	{
		return ( !Para32.sQPM.KPA 
			&& Para32.wdInfoAddr >= CS_START_ADDR 
			&& Para32.wdInfoAddr <= CS_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidPara32bit( structPara32bitNoInfoAddr& Para32 )
	{
		return ( !Para32.sQPM.KPA );
	}


	// *****************************************************
	// 函数功能: 验证电能累积量有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structDNLJ& DNLJ: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidDNLJ( structDNLJ& DNLJ )
	{
		return ( !DNLJ.sSN.IV 
			&& DNLJ.wdInfoAddr >= DNLJ_START_ADDR 
			&& DNLJ.wdInfoAddr <= DNLJ_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidDNLJ( structDNLJNoInfoAddr& DNLJ )
	{
		return ( !DNLJ.sSN.IV );
	}


	// *****************************************************
	// 函数功能: 验证带时标的继电保护装置事件有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structJDBHNoTime& JDBH: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidJDBH( structJDBHNoTime& JDBH )
	{
		return ( !JDBH.sSEP.IV 
			&& JDBH.wdInfoAddr >= JDBH_START_ADDR 
			&& JDBH.wdInfoAddr <= JDBH_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidJDBH( structJDBHNoTimeNoInfoAddr& JDBH )
	{
		return ( !JDBH.sSEP.IV );
	}


	// *****************************************************
	// 函数功能: 验证带时标的继电保护装置成组启动事件有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structJDBHGroupActNoTime& JDBHGroup: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidJDBHGroupAct( 
		structJDBHGroupActNoTime& JDBHGroup )
	{
		return ( !JDBHGroup.sQDP.IV 
			&& JDBHGroup.wdInfoAddr >=JDBH_START_ADDR 
			&& JDBHGroup.wdInfoAddr <= JDBH_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidJDBHGroupAct( 
		structJDBHGroupActNoTimeNoInfoAddr& JDBHGroup )
	{
		return ( !JDBHGroup.sQDP.IV );
	}



	// *****************************************************
	// 函数功能: 验证带时标的继电保护装置成组输出电路信息有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structJDBHGroupOutputNoTime& JDBHGroup: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidJDBHGroupOutput( 
		structJDBHGroupOutputNoTime& JDBHGroup )
	{
		return ( !JDBHGroup.sQDP.IV 
			&& JDBHGroup.wdInfoAddr >=JDBH_START_ADDR 
			&& JDBHGroup.wdInfoAddr <= JDBH_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidJDBHGroupOutput( 
		structJDBHGroupOutputNoTimeNoInfoAddr& JDBHGroup )
	{
		return ( !JDBHGroup.sQDP.IV );
	}



	// *****************************************************
	// 函数功能: 验证带变位检出的成组单点信息有效性
	// 返 回 值: bool : 
	// 参数说明: 
	//    structDIGroup& DIGroup: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidDIGroup( structDIGroup& DIGroup )
	{
		return ( !DIGroup.sQDP.IV 
			&& DIGroup.wdInfoAddr >= DI_START_ADDR 
			&& DIGroup.wdInfoAddr <= DI_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidDIGroup( structDIGroupNoInfoAddr& DIGroup )
	{
		return ( !DIGroup.sQDP.IV );
	}
	// *****************************************************
	// 函数功能: 解析出NVA的值
	// 返 回 值: float : 
	// 参数说明: 
	//    structNVA& NVA: 
	// *****************************************************
	float CFepIEC104Prtcl::GetNVAVal( structNVA& NVA )
	{
		float fVal = 0.0;
		if ( NVA.Sign == 1 )	//负数
		{
			WORD wdT = ~((WORD)NVA.Val) + 1;
			fVal = (float)((OpcUa_Double)(-wdT) / (((WORD)1) << 15 ));
		}
		else	//正数
		{
			fVal = (float)((OpcUa_Double)(NVA.Val) / (((WORD)1) << 15 ));
		}

		return fVal;
	}

	bool CFepIEC104Prtcl::IsCtrlAreaI( const void* pBuf )
	{
		if ( pBuf == NULL )
		{
			return false;
		}

		structCtrlAreaI* pI = (structCtrlAreaI* )pBuf;

		return pI->Flag == 0;
	}

	bool CFepIEC104Prtcl::IsCtrlAreaS( const void* pBuf )
	{
		if ( pBuf == NULL )
		{
			return false;
		}

		structCtrlAreaS* pS = (structCtrlAreaS* )pBuf;

		return pS->Flag == 1;
	}

	bool CFepIEC104Prtcl::IsCtrlAreaU( const void* pBuf )
	{
		if ( pBuf == NULL )
		{
			return false;
		}

		structCtrlAreaU* pU = (structCtrlAreaU* )pBuf;

		return pU->Flag == 3;
	}

	// *****************************************************
	// 函数功能: 根据设备地址获取设备索引号
	// 返 回 值: BOOL : 
	// 参数说明: 
	//      OpcUa_Int32 Addr: 设备地址
	//    OpcUa_Int32& DevNo: 设备地址对应的设备索引号
	// *****************************************************
	bool CFepIEC104Prtcl::GetDeviceNoByAddr( OpcUa_Int32 Addr, OpcUa_Int32& DevNo )
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		CIODeviceBase* pDevice = NULL;
		for( OpcUa_Int32 i=0; i< pDriver->GetDeviceCount(); i++ )
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

	// *****************************************************
	// 函数功能: 获得链路状态：true--至少有一个设备在线；false--所有设备均离线。
	// 返 回 值: BOOL : 
	// *****************************************************
	bool CFepIEC104Prtcl::GetLinkState()
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		CIODeviceBase* pDevice = NULL;
		for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = pDriver->GetDeviceByIndex(i);
			if( pDevice && pDevice->IsOnLine() )
			{
				return true;
			}
		}

		return false;
	}

	OpcUa_Int64 CFepIEC104Prtcl::ASDUTIME2MACSINT64(const ASDUTime& tTime )
	{		
		OpcUa_Int64 llTime =0;
		OpcUa_Int32 iYear,iMon,iDay,iHour,iMin;
		iYear = tTime.stYear.Year;
		iMon = tTime.stMon.Mon;
		iDay = tTime.stDay.MonDay;
		iHour = tTime.stHour.Hours;
		iMin = tTime.stMin.Minute;
		OpcUa_Int64 llTotalDays = 0;
		//! 分：0-59
		if ( tTime.stMin.MinValid == 0)
		{
			//!年0-99
			//llTime = iYear * 31536000 + 946080000; //!31536000*年+30年秒数

			llTotalDays = Days(2000+iYear,iMon,true);
			llTotalDays += iDay-1;
			llTime += llTotalDays * 86400000LL;

			//! 时：0-23
			if ( tTime.stHour.RU  == 0 )
			{
				llTime += (iHour-8) * 3600000LL;
			}
			else
			{
				//!夏令时比标准时间早一个小时
				llTime += (iHour-9) * 3600000LL;
			}

			//!分
			llTime += iMin*60000;

			//!毫秒0-599999			
			llTime += tTime.wMs;
		}
		else
		{
			return 0;
		}
		
		return llTime;		
	}

	OpcUa_Int64 CFepIEC104Prtcl::Time24Bit2MACSINT64( const Time24bit& tTime )
	{
		OpcUa_Int64 llTime=0;
		if ( tTime.stMin.MinValid == 0 )
		{
			ACE_Time_Value val = ACE_OS::gettimeofday();
			ACE_Date_Time dateTime;
			dateTime.update(val);
						
			llTime = val.sec()*1000LL + val.usec()/1000LL;
			if ( dateTime.minute() != tTime.stMin.Minute )
			{
				llTime -= dateTime.minute()*60000; 
				llTime += tTime.stMin.Minute*60000;
			}
			
			llTime -= dateTime.second()*1000;
			llTime -= dateTime.microsec()/1000;
			llTime += tTime.wMs;						
		}
		else
		{
			return 0;
		}	

		return llTime;
	}

	bool CFepIEC104Prtcl::isLeap( OpcUa_Int32 iYear )
	{
		bool bRet = false;
		if ( iYear % 4 == 0 )
		{
			if ( iYear % 100 != 0  || iYear % 400 == 0)
			{
				bRet = true;
			}			
		}
		else
		{
			bRet = false;
		}

		return bRet;
	}

	OpcUa_Int32 CFepIEC104Prtcl::Days( OpcUa_Int32 iYear,OpcUa_Int32 iMon,bool bFrom1970 )
	{
		OpcUa_Int32 iRet =0;
		OpcUa_Int32 iDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
		OpcUa_Int32 iLeapDays[]={31,29,31,30,31,30,31,31,30,31,30,31};
		if ( bFrom1970 == false )
		{
			if ( isLeap(iYear))
			{
				for ( OpcUa_Int32 i=0;i<iMon-1;i++ )
				{
					iRet += iLeapDays[i];
				}
			}
			else
			{
				for ( OpcUa_Int32 i=0;i<iMon-1;i++ )
				{
					iRet += iDays[i];
				}
			}
		}
		else
		{
			for ( OpcUa_Int32 k=1970;k<iYear;k++)
			{
				if ( isLeap(k))
				{
					iRet += 366;
				}
				else
				{
					iRet += 365;
				}
			}

			if ( isLeap(iYear))
			{
				for ( OpcUa_Int32 i=0;i<iMon-1;i++ )
				{
					iRet += iLeapDays[i];
				}
			}
			else
			{
				for ( OpcUa_Int32 i=0;i<iMon-1;i++ )
				{
					iRet += iDays[i];
				}
			}
		}

		return iRet;
	}

	void CFepIEC104Prtcl::InitConfigPar( char* strAppName )
	{
		if ( strAppName == NULL )
		{
			return;
		}

		//! 获取配置文件绝对路径
		std::string file_name = CIODriver::GetFEPRunDir() + MACS_DRIVER_FILENAME;	

		//重连间隔，毫秒，默认10000
		m_t0 = GetPrivateProfileInt( strAppName, "t0", 10000,  file_name.c_str() );
		if ( m_t0 <= 5000 )
		{
			m_t0 = 10000;
		}


		//总召唤周期，毫秒，默认30000
		m_t1 = GetPrivateProfileInt(strAppName, "t1", 30000, file_name.c_str()); 
		if ( m_t1 < 5000 )
		{
			m_t1 = 30000;
		}

		//无数据保文t2<t1时确认的超时，毫秒，默认10000
		m_t2 = GetPrivateProfileInt(strAppName, "t2", 10000, file_name.c_str()); 
		if ( m_t2 < 1000 )
		{
			m_t2 = 10000;
		}

		//长期空闲t3>t1状态下发送测试帧的超时，毫秒，默认20000
		m_t3 = GetPrivateProfileInt(strAppName, "t3", 20000, file_name.c_str()); 
		if ( m_t3 < 1000 )
		{
			m_t3 = 20000;
		}

		//计数量召唤周期，毫秒，默认60000
		m_t4 = GetPrivateProfileInt(strAppName, "t4", 60000, file_name.c_str()); 
		if ( m_t4 < 5000 )
		{
			m_t4 = 60000;
		}

		//离线计数，默认10.
		m_nMaxOfflineCount = GetPrivateProfileInt(strAppName, "Offline", 15, file_name.c_str());
		if ( m_nMaxOfflineCount < 10 )
		{
			m_nMaxOfflineCount = 10;
		}

		//超时，毫秒。50~10000，默认1000。
		m_TimeOut = GetPrivateProfileInt(strAppName, "TimeOutCycl", 1000, file_name.c_str());
		if ( m_TimeOut < 500 )
		{
			m_TimeOut = 1000;
		}

		//发送Test测试帧周期，毫秒。0表示不发送测试帧。
		m_TestCycl = GetPrivateProfileInt(strAppName, "TestCycl", 10000, file_name.c_str());
		if ( m_TestCycl < 1000 && m_TestCycl != 0 )
		{
			m_TestCycl = 10000;
		}

		//校时周期, 秒.默认3600.
		m_CheckTimeCycl = GetPrivateProfileInt(strAppName, "CheckTimeCycl", 3600, file_name.c_str());
		if ( m_CheckTimeCycl < 5 && m_CheckTimeCycl != 0 )
		{
			m_CheckTimeCycl = 3600;
		}

		OpcUa_Int32 iTemp = GetPrivateProfileInt(strAppName, "IsLogValueError", 1, file_name.c_str());
		m_bIsLogValueError = (iTemp == 0)?false:true;
		
		iTemp = GetPrivateProfileInt(strAppName, "CheckSDI", 1, file_name.c_str());
		m_bCheckSDI = (iTemp == 0)?false:true;

		iTemp = GetPrivateProfileInt(strAppName, "CheckDDI", 1, file_name.c_str());
		m_bCheckDDI = (iTemp == 0)?false:true;

		iTemp = GetPrivateProfileInt(strAppName, "CheckVTI", 1, file_name.c_str());
		m_bCheckVTI = (iTemp == 0)?false:true;

		char pchTemp[1024]={0};
		sprintf( pchTemp, "t0=%d, t1=%d, t2=%d, t3=%d, t4=%d, Offline=%d, TimeOutCycl=%d,TestCycl=%d, CheckTimeCycl=%d, IsLogValueError=%d,CheckSDI=%d,CheckDDI=%d,CheckVTI=%d", \
			m_t0, m_t1,m_t2,m_t3,m_t4,m_nMaxOfflineCount,m_TimeOut,m_TestCycl,m_CheckTimeCycl,m_bIsLogValueError,m_bCheckSDI,m_bCheckDDI,m_bCheckVTI);
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		pDriver->WriteLogAnyWay( pchTemp );
	}


	std::string CFepIEC104Prtcl::GetCommData( OpcUa_Byte* pbyBuf, OpcUa_Int32 nDataLen, OpcUa_Byte byRcvOrSnd )
	{
		std::string szData = "";
		if ( byRcvOrSnd == 1 )
		{
			szData = "RCV: ";
		}
		else
		{
			szData = "SND: ";
		}

		char pchOpcUa_Byte[4];
		//! 获取数据显示格式
		for ( OpcUa_Int32 i=0; i < nDataLen; i++ )
		{
			sprintf( pchOpcUa_Byte, "%02X ", pbyBuf[i] );
			szData += pchOpcUa_Byte;
		}

		return szData;
	}

	void CFepIEC104Prtcl::ForStartDT()
	{
		//m_bNeedSendSFrame	= false;
		m_bNeedStopDT		= false;
		m_bNeedStartDT		= true;
		//m_bNeedSendTest		= false;
		m_bNeedStartDTcon	= false;
		m_bNeedStopDTcon	= false;
		//m_bNeedSendTestcon	= false;
	}

	void CFepIEC104Prtcl::ForStopDT()
	{
		//m_bNeedSendSFrame	= false;
		m_bNeedStopDT		= true;
		m_bNeedStartDT		= false;
		//m_bNeedSendTest		= false;
		m_bNeedStartDTcon	= false;
		m_bNeedStopDTcon	= false;
		//m_bNeedSendTestcon	= false;
	}

	bool CFepIEC104Prtcl::IsLogForRxDataProc()
	{
		return m_bDebugLog;
	}

	bool CFepIEC104Prtcl::IsLog()
	{
		return m_bLog;
	}

	bool CFepIEC104Prtcl::IsCheckSDI()
	{
		return m_bCheckSDI;
	}

	bool CFepIEC104Prtcl::IsCheckDDI()
	{
		return m_bCheckDDI;
	}

	bool CFepIEC104Prtcl::IsCheckVTI()
	{
		return m_bCheckVTI;
	}

	bool CFepIEC104Prtcl::IsLogValueError()
	{
		return m_bIsLogValueError;
	}
}

#endif
