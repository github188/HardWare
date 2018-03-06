/*!
 *	\file	IOSrlBoardDealThread.cpp
 *
 *	\brief	��Ϣѭ���߳���ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	15:19
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
		proactor_ = new ACE_Proactor( new ACE_WIN32_Proactor, 1 );		//! 1��ʾ��ProactorҪɾ����ʵ�ֶ���
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
		/* ����һ���Լ���ǰ��������
		ע��ƽ̨���죺
		Window��ȱʡ��ΪACE_WIN32_Proactor��
		Linux��ȱʡΪACE_POSIX_SIG_Proactor����Proactor�ǻ����źţ��˴������������У�ֻ����ACE_POSIX_AIOCB_Proactor��
		SUN��ΪACE_SUN_Proactor
		*/
#ifdef _WINDOWS
		//!< ��������		
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
        int nPortNumber = m_tDrvBoardCfg.nPort;				//! �˿ں�
		char pchDevAddr[16];
		ACE_OS::sprintf( pchDevAddr, "/dev/%s%d", stPrefix.c_str(), nPortNumber-1 );

		chComMod(pchDevAddr);                              //! ȡ�����ڻ�������

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
			//! ���������Ϣ
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

		params.baudrate = m_tDrvBoardCfg.iBaudRate;//!< ������
		params.databits = m_tDrvBoardCfg.iDataBits;//!< ����λ���Ϸ�ֵΪ5,6,7,8��Win32��֧��4λ
		params.stopbits = m_tDrvBoardCfg.iStopBits;//!< ֹͣλ���Ϸ�ֵΪ1��2
		//params.parityenb = (bool)m_tDrvBoardCfg.byParity;//!< �Ƿ������żУ��
		if(m_tDrvBoardCfg.byParity==1)
			params.paritymode = "EVEN";			//!< ��żУ���ģʽ��POSIX֧��"even"��"odd"��
		if(m_tDrvBoardCfg.byParity==2)
			params.paritymode = "ODD";			//!< ��żУ���ģʽ��POSIX֧��"even"��"odd"��

		params.xonlim = 0;						//!< ��XON�ַ�������֮ǰ�����뻺��������С�ֽ�
		params.xofflim = 0;						//!< ��XOFF�ַ�������֮ǰ�����뻺����������ֽ�
		params.readmincharacters = 1;			//!< ����POSIX�Ǳ�׼�Ķ����ٸ������ַ�
		params.readtimeoutmsec = 10000;			//!< �ڴ�read����֮ǰ���ȴ���ʱ��			
		params.ctsenb = false;					//!< �Ƿ�֧��CTSģʽ��ȷ���Ƿ��ͨ����ѯ Clear To Send (CTS) �ߵ�״̬�������ݡ�
		params.rtsenb = 0;						//!< ʹ�ú�����RTS��0��ʾ��ʹ��RTS 1��ʾʹ��RTS 2��ʾʹ��RTS����������(win32)
												//!< 3��ʾ��������㹻�����ڴ�����ֽڣ�RTS�߱�ߡ�����֮��RTS��ɵ�(win32)
		params.xinenb = false;					//!< �Ƿ���������ʹ�����������
		params.xoutenb = false;					//!< �Ƿ��������ʹ�����������
		params.modem = false;					//!< �豸�Ƿ���POSIX����ģʽ
		params.rcvenb = true;					//!< �Ƿ�ʹ��receiver (POSIX)
		params.dsrenb = false;					//!< Data Set Ready (DSR) �ߵ�״̬�Ƿ������� (WIN32)
		params.dtrdisable = false;				//!< Data Terminal Ready״̬�Ƿ�������


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