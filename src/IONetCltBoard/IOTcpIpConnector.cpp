/*!
 *	\file	IOTcpIpConnector.cpp
 *
 *	\brief	������linuxϵͳ������������Ӧʽ����ʵ���ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��11��	11:09
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IOTcpIpSvcHandler.h"
#include "RTDBBaseClasses/IOPrtclParserBase.h"
#include "RTDBCommon/Debug_Msg.h"
#include "ace/Connector.h"
#include "IOTcpIpConnector.h"

namespace MACS_SCADA_SUD
{
	CIOTcpIpConnector::CIOTcpIpConnector()
	{
		m_bConnected = false;
		m_pPrtclParser = NULL;
		m_pHandler = NULL;
	}

	CIOTcpIpConnector::~CIOTcpIpConnector()
	{
		delete m_pHandler;
		m_pHandler = NULL;
	}

	int CIOTcpIpConnector::Init(CIOPrtclParserBase* pPrtclParser)
	{
		m_pPrtclParser = pPrtclParser;

		m_pHandler = new CIOTcpIpSvcHandler();
		m_pHandler->Init( m_pPrtclParser, this );
		return 0;
	}

	int CIOTcpIpConnector::make_svc_handler(CIOTcpIpSvcHandler* pHandler)
	{
		pHandler->reactor( this->reactor() );
		return 0;
	}

	CIOTcpIpSvcHandler* CIOTcpIpConnector::GetHandler()
	{
		return m_pHandler;
	}

	void CIOTcpIpConnector::SetServerAddr(int port, const char* pchIP)
	{
		m_nPort = port;
		strcpy( m_pchSerAddr, pchIP);
	}

	int CIOTcpIpConnector::ConnInterrupt()
	{
		if(!m_bConnected)
			return 0;

		m_bConnected = false;

		//! �ر�
		this->close();

		return 0;
	}

	bool CIOTcpIpConnector::Connect(long nTimeOut)
	{
		m_bConnected = false;

		//! �ر�socket
		if (m_pHandler->get_handle () != ACE_INVALID_HANDLE)
		{
			ACE_OS::shutdown(m_pHandler->get_handle(),2);
			ACE_OS::closesocket (m_pHandler->get_handle ());
			m_pHandler->set_handle(ACE_INVALID_HANDLE);
		}

		this->cancel(m_pHandler);

		int ret = this->connect( m_pHandler, ACE_INET_Addr(m_nPort, m_pchSerAddr) );
		if(ret == 0)
		{
			m_bConnected = true;
			if(m_pPrtclParser)
			{
				m_pPrtclParser->OnConnSuccess(m_pchSerAddr);
			}
		}

		//! ��ʱ���򷵻�����״̬
		return m_bConnected;
	}

	bool CIOTcpIpConnector::Connect(long nTimeOut, const char* pPeerIP, int nPort)
	{
		m_bConnected = false;

		//! �ر�socket
		if (m_pHandler->get_handle () != ACE_INVALID_HANDLE)
		{
			ACE_OS::shutdown(m_pHandler->get_handle(),2);
			ACE_OS::closesocket (m_pHandler->get_handle ());
			m_pHandler->set_handle(ACE_INVALID_HANDLE);
		}

		SetServerAddr(nPort, pPeerIP);

		this->cancel(m_pHandler);

		int ret = this->connect( m_pHandler, ACE_INET_Addr(nPort, pPeerIP) );

		if(ret == 0)
		{
			m_bConnected = true;
			if(m_pPrtclParser)
			{
				m_pPrtclParser->OnConnSuccess(m_pchSerAddr);
			}
		}

		//! ��ʱ���򷵻�����״̬
		return m_bConnected;
	}

	int CIOTcpIpConnector::Connect()
	{
		m_bConnected = false;

		//! �ر�socket
		if (m_pHandler->get_handle () != ACE_INVALID_HANDLE)
		{
			ACE_OS::shutdown(m_pHandler->get_handle(),2);
			ACE_OS::closesocket (m_pHandler->get_handle ());
			m_pHandler->set_handle(ACE_INVALID_HANDLE);
		}

		this->cancel(m_pHandler);

		//! ����
		int ret = this->connect( m_pHandler, ACE_INET_Addr(m_nPort, m_pchSerAddr) );

		if(ret == 0)
		{
			m_bConnected = true;
			if(m_pPrtclParser)
			{
				m_pPrtclParser->OnConnSuccess(m_pchSerAddr);
			}
		}

		return ret;
	}

	int CIOTcpIpConnector::Connect(const char* pPeerIP, int nPort)
	{
		m_bConnected = false;

		//! �ر�socket
		if (m_pHandler->get_handle () != ACE_INVALID_HANDLE)
		{
			ACE_OS::shutdown( m_pHandler->get_handle(), 2 );
			ACE_OS::closesocket (m_pHandler->get_handle ());
			m_pHandler->set_handle(ACE_INVALID_HANDLE);
		}

		SetServerAddr(nPort, pPeerIP);

		this->cancel(m_pHandler);
		int ret = this->connect( m_pHandler, ACE_INET_Addr(nPort, pPeerIP) );

		if(ret == 0)
		{
			m_bConnected = true;
			if(m_pPrtclParser)
			{
				m_pPrtclParser->OnConnSuccess(m_pchSerAddr);
			}

		}
		return ret;
	}

	bool CIOTcpIpConnector::IsConnected()
	{
		return m_bConnected;
	}
}


