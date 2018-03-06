/*!
 *	\file	IOTcpIpSvcHandler.cpp
 *
 *	\brief	用于类linux系统的反应式事件处理器实现
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	10:25
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
			//!	接收到服务端应答, 送协议处理器解析
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
			//! 输出调试信息
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