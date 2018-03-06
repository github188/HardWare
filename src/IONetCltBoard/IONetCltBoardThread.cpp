/*!
 *	\file	IONetCltBoardThread.cpp
 *
 *	\brief	消息循环线程类实现
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	14:34
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "IOTcpIpSvcHandler.h"
#include "IOTcpIpHandler.h"
#include "IONetCltBoardThread.h"
#include "IOTcpIpConnector.h"
#include "IONetSynConnector.h"
#include "RTDBCommon/Debug_Msg.h"

#define MAX_ROLL_TIMES 2

namespace MACS_SCADA_SUD
{

	ACE_ReactorEX::ACE_ReactorEX(ACE_Reactor_Impl *implementation /*= 0*/, int delete_implementation /*= 0*/)
		: ACE_Reactor(implementation,delete_implementation)
	{

	}

	ACE_ReactorEX::~ACE_ReactorEX(void)
	{

	}

	int ACE_ReactorEX::run_reactor_event_loop(REACTOR_EVENT_HOOK eh/*= 0*/)
	{
		ACE_TRACE ("ACE_Reactor::run_reactor_event_loop");

		if (this->reactor_event_loop_done ())
			return 0;

		while (1)
		{
			int result = this->implementation_->handle_events ();

			if (eh != 0 && (*eh)(this))
				continue;
			else if (result == -1 && this->implementation_->deactivated ())
				return 0;
			else if (result == -1)
				return -1;
			ACE_OS::sleep( ACE_Time_Value(0,10*1000) );
		}

		ACE_NOTREACHED (return 0;)
	}



	CIONetCltBoardThread::CIONetCltBoardThread(void)
	{
		m_pConnector = NULL;
#ifdef _WINDOWS
		proactor_ = NULL;
#else
		reactor_ = NULL;
#endif
	}

	CIONetCltBoardThread::~CIONetCltBoardThread(void)
	{
		if (m_pConnector)
		{
			delete m_pConnector;
			m_pConnector = NULL;
		}
#ifdef _WINDOWS
		delete proactor_;
		proactor_ = NULL;
#else
		delete reactor_;
		reactor_ = NULL;
#endif
	}

	int CIONetCltBoardThread::open()
	{
		activate(THR_NEW_LWP|THR_DETACHED|THR_INHERIT_SCHED);
		return 0;
	}

	int CIONetCltBoardThread::Init(tIOPortCfg tDrvCfg, CIOPrtclParserBase *pPrtcl)
	{
		m_tDrvBoardCfg = tDrvCfg;
		m_pPrtclParser = pPrtcl;

#ifdef _WINDOWS
		proactor_ = new ACE_Proactor(&m_proactorImpl, 1);
		//! 连接类
		m_pConnector = new CIONetSynConnector();
#else
		reactor_ = new ACE_ReactorEX(&m_selReactor);
		//! 连接类
		m_pConnector = new CIOTcpIpConnector();
#endif
		m_pConnector->Init(m_pPrtclParser);
		//! 初始化连接，根据配置
		if( m_tDrvBoardCfg.strIPA_A != "")
		{
			m_pConnector->SetServerAddr(m_tDrvBoardCfg.nPort, m_tDrvBoardCfg.strIPA_A.c_str());
		}

		m_bWorking = false;
		return 0;
	}

	int CIONetCltBoardThread::close()
	{
#ifdef _WINDOWS
		//! 关闭proactor_循环
		if ( proactor_ )
			proactor_->proactor_end_event_loop();
#else
		//! 关闭reactor_循环
		if ( reactor_ )
			reactor_->end_reactor_event_loop();
#endif

		m_bWorking = false;
		return 0;
	}

	int CIONetCltBoardThread::svc(void)
	{
		/* 创建一个自己的前摄器对象。
		注意平台差异：
		Window下缺省即为ACE_WIN32_Proactor；
		Linux下缺省为ACE_POSIX_SIG_Proactor，该Proactor是基于信号，此处不能正常运行，只能用ACE_POSIX_CB_Proactor；
		SUN下为ACE_SUN_Proactor
		*/
#ifdef _WINDOWS
		m_pConnector->open( 0, proactor_ );

		m_bWorking = true;

		//!	进入前摄器循环
		if ( proactor_->proactor_run_event_loop( ) == -1 )
		{
			m_bWorking = false;
			return -1;
		}
		proactor_->proactor_reset_event_loop ();
#else
		m_pConnector->open( reactor_ );

		m_bWorking = true;

		//!	进入前摄器循环
		reactor_->run_reactor_event_loop( );
#endif
		return 0;
	}

	int CIONetCltBoardThread::Write(const Byte* pbyData, int nDataLen)
	{
		int nRet = 0;
		if ( !m_pConnector )
		{
			nRet = -12;
		}
		else
		{
			if( m_pConnector->IsConnected() )
			{
				nRet = m_pConnector->GetHandler()->Write(pbyData,nDataLen);
			}
			else
			{
				MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("CIONetCltBoardThread::The Connector is not connected!")));
				nRet = -13;
			}
		}
		return nRet;
	}
}