/*!
 *	\file	SrlHandlerLinux.cpp
 *
 *	\brief	������linuxϵͳ�ķ�Ӧʽ�¼�������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	13:57
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "SrlHandlerLinux.h"
#include "IOSerialBoard.h"
#include "RTDBCommon/Debug_Msg.h"
#include "RTDBBaseClasses/IOPrtclParserBase.h"

namespace MACS_SCADA_SUD
{


	CSrlSvcHandler::CSrlSvcHandler()
	{

	}

	CSrlSvcHandler::~CSrlSvcHandler()
	{

	}

	int CSrlSvcHandler::open(void* p)
	{
		int nRet = 0;
		nRet = reactor()->register_handler(this,ACE_Event_Handler::READ_MASK );

		return nRet;
	}

	int CSrlSvcHandler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
	{
		return 0;
	}

	int CSrlSvcHandler::handle_input(ACE_HANDLE fd/*=ACE_INVALID_HANDLE */)
	{
		int nRecvLen = 0;
		byte pchBuf[256];
		nRecvLen = this->peer().recv( pchBuf, 256 );
		if ( nRecvLen>0 )
		{
			//	���յ������Ӧ��, ��Э�鴦��������
			m_pPrtclParser->RxDataProc( pchBuf, nRecvLen );
		}

		return 0;
	}

	int CSrlSvcHandler::handle_output(ACE_HANDLE fd/*=ACE_INVALID_HANDLE */)
	{
		return 0;
	}

	int CSrlSvcHandler::Write(unsigned char * pbyData, int nDataLen)
	{
		if( !pbyData || nDataLen <= 0)
			return -2;

		int nSendLen = this->peer().send( pbyData, nDataLen );
		if ( nSendLen <= 0 )
		{
			ACE_ERROR( (LM_ERROR, ACE_TEXT( "%p\n"), ACE_TEXT( "starting write" )));
			//! ���������Ϣ
			if ( CIOSerialBoard::CanOutput() )
			{
				char pchDebug[DEBUGINFOLEN];
				sprintf( pchDebug, "CSrlSvcHandler::Write - Write Failed!" );	
				CIOSerialBoard::OutputDebug( pchDebug );
			}
			return -1;
		}

		return 0;
	}

	int CSrlSvcHandler::Init(CIOPrtclParserBase* pPrtclParser, long nTimeOut)
	{
		m_pPrtclParser = pPrtclParser;
		return 0;
	}
}