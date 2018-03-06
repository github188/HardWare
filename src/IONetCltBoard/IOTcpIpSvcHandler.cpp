/*!
 *	\file	IOTcpIpSvcHandler.cpp
 *
 *	\brief	������linuxϵͳ�ķ�Ӧʽ�¼�������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��11��	10:25
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IOTcpIpSvcHandler.h"
#include "RTDBBaseClasses/IOPrtclParserBase.h"
#include "IOTcpIpConnector.h"
#include "IONetCltBoard.h"
#include "RTDBCommon/Debug_Msg.h"

namespace MACS_SCADA_SUD
{
	CIOTcpIpSvcHandler::CIOTcpIpSvcHandler()
	{
		m_pPrtclParser = NULL;
		m_pParent = NULL;
	}

	CIOTcpIpSvcHandler::~CIOTcpIpSvcHandler()
	{

	}

	int CIOTcpIpSvcHandler::Init(CIOPrtclParserBase* pPrtclParser, CIOTcpIpConnector* pParent)
	{
		m_pPrtclParser = pPrtclParser;
		m_pParent = pParent;
		return 0;
	}

	int CIOTcpIpSvcHandler::open(void* p)
	{
		int nRet = 0;
		nRet = reactor()->register_handler(this,ACE_Event_Handler::READ_MASK );

		return 0;
	}

	int CIOTcpIpSvcHandler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
	{
		return 0;
	}

	int CIOTcpIpSvcHandler::handle_input(ACE_HANDLE fd/*=ACE_INVALID_HANDLE */)
	{
		int nRecvLen = 0;
		Byte pchBuf[1024];
		nRecvLen = this->peer().recv( pchBuf, 1024 );
		if ( nRecvLen>0 )
		{
			//!	���յ������Ӧ��, ��Э�鴦��������
			m_pPrtclParser->RxDataProc( pchBuf, nRecvLen );
		}

		return 0;
	}

	int CIOTcpIpSvcHandler::handle_output(ACE_HANDLE fd /*= ACE_INVALID_HANDLE */)
	{
		return 0;
	}

	int CIOTcpIpSvcHandler::Write(const OpcUa_Byte* pbyData, int nDataLen)
	{
		if( !pbyData || nDataLen <= 0 )
			return -2;

		int nSendLen = this->peer().send( pbyData, nDataLen );
		if ( nSendLen <= 0 )
		{
			MACS_ERROR(( ERROR_PERFIX
				ACE_TEXT("CIOTcpIpSvcHandler::Write - Write Failed!") ));
			//! ���������Ϣ
			if ( CIONetCltBoard::CanOutput() )
			{
				char pchDebug[DEBUGINFOLEN];
				sprintf( pchDebug, "CIOTcpIpSvcHandler::Write - Write Failed!" );	
				CIONetCltBoard::OutputDebug( pchDebug );
			}
			return -1;
		}
		return 0;
	}
}