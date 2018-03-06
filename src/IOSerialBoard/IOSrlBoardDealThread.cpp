/*!
 *	\file	IOSrlBoardDealThread.cpp
 *
 *	\brief	消息循环线程类实现
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月10日	15:19
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IOSerialBoard.h"
#include "IOSrlBoardDealThread.h"
#include "ChmodLinux.h"
#include "ace/TP_Reactor.h"
#include "ace/WIN32_Proactor.h"
#include "ace/Global_Macros.h"
#include "ace/High_Res_Timer.h"
#include "RTDBCommon/Debug_Msg.h"

namespace MACS_SCADA_SUD
{
	
	CIOSrlBoardDealThread::CIOSrlBoardDealThread(void)
	{
#ifdef _WINDOWS
		proactor_ = new ACE_Proactor( new ACE_WIN32_Proactor, 1 );		//! 1表示该Proactor要删除该实现对象
		m_pHandler = new CIOSrlBoardHandler();
		m_pHandler->proactor( proactor_ );
#else
		reactor_ = new ACE_Reactor( new ACE_TP_Reactor );
		m_pHandler = new CSrlSvcHandler();		
		m_pHandler->reactor( reactor_ );
#endif
	}

	CIOSrlBoardDealThread::~CIOSrlBoardDealThread(void)
	{
		if ( m_pHandler )
		{
			delete m_pHandler;
			m_pHandler = NULL;
		}
#ifdef _WINDOWS
		if ( proactor_ )
		{
			delete proactor_;
			proactor_ = NULL;
		}
#else
		if ( reactor_ )
		{
			delete reactor_;
			reactor_ = NULL;
		}
#endif
	}

	int CIOSrlBoardDealThread::Init(tIOPortCfg tDrvCfg,CIOPrtclParserBase *pPrtcl,long nTimeOut)
	{
		m_nTimeOut = nTimeOut;
		m_tDrvBoardCfg = tDrvCfg;
		m_pPrtclParser = pPrtcl;
		m_bOpened = false;

		m_pHandler->Init( m_pPrtclParser, m_nTimeOut );
		return 0;
	}

	int CIOSrlBoardDealThread::open()
	{
		activate(THR_NEW_LWP|THR_DETACHED|THR_INHERIT_SCHED);
		return 0;
	}

	int CIOSrlBoardDealThread::close()
	{
#ifdef _WINDOWS
		if ( proactor_ )
			proactor_->proactor_end_event_loop();
#else
		if ( reactor_ )
			reactor_->end_reactor_event_loop();
#endif
		m_bOpened = false;
		return 0;
	}

	int CIOSrlBoardDealThread::svc(void)
	{
		/* 创建一个自己的前摄器对象。
		注意平台差异：
		Window下缺省即为ACE_WIN32_Proactor；
		Linux下缺省为ACE_POSIX_SIG_Proactor，该Proactor是基于信号，此处不能正常运行，只能用ACE_POSIX_AIOCB_Proactor；
		SUN下为ACE_SUN_Proactor
		*/
#ifdef _WINDOWS
		//!< 建立连接		
		int nRet = m_pHandler->open(this->m_tDrvBoardCfg); 
		if ( nRet != 0 )
		{
			return -1;
		}
		m_bOpened = true;
		if ( proactor_->proactor_run_event_loop( ) == -1 )
		{
			m_bOpened = false;
			return -2;
		}
		proactor_->proactor_reset_event_loop ();
		
#else
		//if (!g_pIOCfgManager)
		//{
		//	return -1;
		//}
		//std::string stPrefix = g_pIOCfgManager->GetComFileName();
		std::string stPrefix = m_tDrvBoardCfg.strSrlBrdPrefix.toUtf8();
        int nPortNumber = m_tDrvBoardCfg.nPort;				//! 端口号
		char pchDevAddr[16];
		ACE_OS::sprintf( pchDevAddr, "/dev/%s%d", stPrefix.c_str(), nPortNumber-1 );

		chComMod(pchDevAddr);                              //! 取消串口回显特性

		COM_CONNECTOR comConnector( reactor_ );
		int nRet = comConnector.connect( m_pHandler, 
			ACE_DEV_Addr(pchDevAddr),
			ACE_Synch_Options::defaults,
			ACE_DEV_Addr(pchDevAddr),
			0,
			O_RDWR|O_NOCTTY,
			0);

		if(nRet != 0)
		{
			MACS_ERROR( (ERROR_PERFIX ACE_TEXT("BLAD: %m (%d).\n"), ACE_OS::last_error() ) ); 
			//! 输出调试信息
			if ( CIOSerialBoard::CanOutput() )
			{
				char pchDebug[DEBUGINFOLEN];
				sprintf( pchDebug, "CIOSrlBoardDealThread::svc open port %s Failed!", pchDevAddr );
				CIOSerialBoard::OutputDebug( pchDebug );
			}
			return -1;
		}
		// set the appropriate parameters 
		ACE_TTY_IO::Serial_Params params; 

		params.baudrate = m_tDrvBoardCfg.iBaudRate;//!< 波特率
		params.databits = m_tDrvBoardCfg.iDataBits;//!< 数据位。合法值为5,6,7,8。Win32上支持4位
		params.stopbits = m_tDrvBoardCfg.iStopBits;//!< 停止位。合法值为1和2
		//params.parityenb = (bool)m_tDrvBoardCfg.byParity;//!< 是否进行奇偶校验
		if(m_tDrvBoardCfg.byParity==1)
			params.paritymode = "EVEN";			//!< 奇偶校验的模式。POSIX支持"even"和"odd"。
		if(m_tDrvBoardCfg.byParity==2)
			params.paritymode = "ODD";			//!< 奇偶校验的模式。POSIX支持"even"和"odd"。

		params.xonlim = 0;						//!< 在XON字符被发送之前，输入缓冲区的最小字节
		params.xofflim = 0;						//!< 在XOFF字符被发送之前，输入缓冲区的最大字节
		params.readmincharacters = 1;			//!< 对于POSIX非标准的读最少个数的字符
		params.readtimeoutmsec = 10000;			//!< 在从read返回之前，等待的时间			
		params.ctsenb = false;					//!< 是否支持CTS模式。确定是否可通过查询 Clear To Send (CTS) 线的状态发送数据。
		params.rtsenb = 0;						//!< 使用和设置RTS。0表示不使用RTS 1表示使用RTS 2表示使用RTS流控制握手(win32)
												//!< 3表示如果有了足够的用于传输的字节，RTS线变高。传输之后RTS变成低(win32)
		params.xinenb = false;					//!< 是否在输入上使用软件流控制
		params.xoutenb = false;					//!< 是否在输出上使用软件流控制
		params.modem = false;					//!< 设备是否是POSIX调制模式
		params.rcvenb = true;					//!< 是否使用receiver (POSIX)
		params.dsrenb = false;					//!< Data Set Ready (DSR) 线的状态是否起作用 (WIN32)
		params.dtrdisable = false;				//!< Data Terminal Ready状态是否起作用


		int result = m_pHandler->peer().control( ACE_TTY_IO::SETPARAMS, &params ); 
		if( result != 0 ) 
		{ 
			return -2; 
		}

		m_bOpened = true;

		reactor_->run_reactor_event_loop();
#endif

		return 0;
	}


	int CIOSrlBoardDealThread::Write(byte* pbyData, int nDataLen)
	{
		if(m_bOpened)
		{
			return m_pHandler->Write(pbyData,nDataLen);
		}
		else
		{
			//MACS_ERROR( (ERROR_PERFIX ACE_TEXT( "IOSerialBoard::Write The Thread is not opened!" )));
			return -1;
		}
		return 0;
	}
}