/*!
 *	\file	IONetSynConnector.cpp
 *
 *	\brief	用于windows系统的连接器（前摄式）实现
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	13:51
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IONetSynConnector.h"
#include "IOTcpIpHandler.h"
#include "RTDBBaseClasses/IOPrtclParserBase.h"
#include "IONetCltBoardThread.h"
#include "ace/Asynch_Connector.h"
#include "RTDBCommon/Debug_Msg.h"

namespace MACS_SCADA_SUD
{
	CIONetSynConnector::CIONetSynConnector()
	{
		m_bConnected = false;
		m_pPrtclParser = NULL;
		m_pHandler = NULL;
	}

	CIONetSynConnector::~CIONetSynConnector()
	{
		delete m_pHandler;
		m_pHandler = NULL;
	}

	void CIONetSynConnector::handle_connect(const ACE_Asynch_Connect::Result &result)
	{
		ACE_Asynch_Connector<CIOTcpIpHandler>::handle_connect(result);

		if ( (!result.success ()) || (result.connect_handle () == ACE_INVALID_HANDLE)
			|| (result.error () != 0))
		{
			m_bConnected = false;
			return;
		}

		m_bConnected = true;
		if(m_pPrtclParser)
		{
			m_pPrtclParser->OnConnSuccess(m_pchSerAddr);
		}
	}

	CIOTcpIpHandler* CIONetSynConnector::make_handler(void)
	{
		return m_pHandler;
	}

	int CIONetSynConnector::Init(CIOPrtclParserBase* pPrtclParser)
	{
		m_pPrtclParser = pPrtclParser;

		m_pHandler = new CIOTcpIpHandler();
		m_pHandler->Init( m_pPrtclParser, this );
		return 0;
	}

	CIOTcpIpHandler* CIONetSynConnector::GetHandler()
	{
		return m_pHandler;
	}

	void CIONetSynConnector::SetServerAddr(int port, const char* pchIP)
	{
		m_nPort = port;
		strcpy( m_pchSerAddr, pchIP);
	}

	int CIONetSynConnector::ConnInterrupt()
	{
		if(!m_bConnected)
			return 0;

		m_bConnected = false;

		//! 关闭socket
		if (m_pHandler->handle () != ACE_INVALID_HANDLE)
		{
			ACE_OS::shutdown(m_pHandler->handle(),2);
			ACE_OS::closesocket (m_pHandler->handle ());
			m_pHandler->handle(ACE_INVALID_HANDLE);
		}
		return 0;
	}

	bool CIONetSynConnector::Connect(long nTimeOut)
	{
		m_bConnected = false;

		//! 关闭socket
		if (m_pHandler->handle () != ACE_INVALID_HANDLE)
		{
			ACE_OS::shutdown(m_pHandler->handle(),2);
			ACE_OS::closesocket (m_pHandler->handle ());
			m_pHandler->handle(ACE_INVALID_HANDLE);
		}

		this->cancel();
		this->connect( ACE_INET_Addr(m_nPort ,m_pchSerAddr) );

		ACE_Time_Value tv( 0, 1000);
		long nTimes = 0;
		while(nTimes < nTimeOut)
		{
			ACE_OS::sleep(tv);

			//! 如果连接成功则直接返回成功
			if (m_bConnected)
			{
				return m_bConnected;
			}
			nTimes++;
		}

		//! 超时，则返回连接状态
		return m_bConnected;
	}

	bool CIONetSynConnector::Connect(long nTimeOut, const char* pPeerIP, int nPort)
	{
		m_bConnected = false;

		//! 关闭socket
		if (m_pHandler->handle () != ACE_INVALID_HANDLE)
		{
			ACE_OS::shutdown(m_pHandler->handle(),2);
			ACE_OS::closesocket (m_pHandler->handle ());
			m_pHandler->handle(ACE_INVALID_HANDLE);
		}

		SetServerAddr(nPort, pPeerIP);

		this->cancel();
		this->connect( ACE_INET_Addr(nPort ,pPeerIP) );

		ACE_Time_Value tv( 0, 1000);
		long nTimes = 0;
		while(nTimes < nTimeOut)
		{
			ACE_OS::sleep(tv);

			//! 如果连接成功则直接返回成功
			if (m_bConnected)
			{
				return m_bConnected;
			}
			nTimes++;
		}

		//! 超时，则返回连接状态
		return m_bConnected;
	}

	int CIONetSynConnector::Connect()
	{
		m_bConnected = false;

		//! 关闭socket
		if (m_pHandler->handle () != ACE_INVALID_HANDLE)
		{
			ACE_OS::shutdown(m_pHandler->handle(),2);
			ACE_OS::closesocket (m_pHandler->handle ());
			m_pHandler->handle(ACE_INVALID_HANDLE);
		}

		this->cancel();
		return this->connect( ACE_INET_Addr(m_nPort, m_pchSerAddr) );
	}

	int CIONetSynConnector::Connect(const char* pPeerIP, int nPort)
	{
		m_bConnected = false;

		//! 关闭socket
		if (m_pHandler->handle () != ACE_INVALID_HANDLE)
		{
			ACE_OS::shutdown(m_pHandler->handle(),2);
			ACE_OS::closesocket (m_pHandler->handle ());
			m_pHandler->handle(ACE_INVALID_HANDLE);
		}

		SetServerAddr(nPort, pPeerIP);

		this->cancel();
		return this->connect( ACE_INET_Addr(nPort, pPeerIP) );
	}

	bool CIONetSynConnector::IsConnected()
	{
		return m_bConnected;
	}
}
