/*!
 *	\file	IODrvBoard.cpp
 *
 *	\brief	���������ӿ�ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	9:40
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IODrvBoard/IODrvBoard.h"
#include "IOBoardImpl.h"
#include "RTDBBaseClasses/IOPrtclParserBase.h"

namespace MACS_SCADA_SUD
{
	//!���캯��
	CIODrvBoard::CIODrvBoard()
	{
		m_pIOBoardImpl=new CIOBoardImpl();
	}

	//!��������
	CIODrvBoard::~CIODrvBoard()
	{
		if ( m_pIOBoardImpl )
		{
			delete m_pIOBoardImpl;
			m_pIOBoardImpl = NULL;
		}
	}

	//!���������ʼ��
	int CIODrvBoard::Init(CIOPrtclParserBase* pPrtclParser, std::string szBoardType, tIOPortCfg portCfg, long nTimeOut)
	{
		return m_pIOBoardImpl->Init(pPrtclParser,szBoardType,portCfg,nTimeOut);
	}

	//!ͨ����·��������
	int CIODrvBoard::Write(unsigned char* pbyData, int nDataLen)
	{
		if (m_pIOBoardImpl)
		{
			return m_pIOBoardImpl->Write(pbyData, nDataLen);
		}
		else
		{
			return -1;
		}
	}

	//!ʹIO��������ʼ����
	int CIODrvBoard::open()
	{
		if (m_pIOBoardImpl)
		{
			return m_pIOBoardImpl->open();
		}
		else
		{
			return -1;
		}
	}

	//!ֹͣ��IO������
	int CIODrvBoard::close()
	{
		if (m_pIOBoardImpl)
		{
			return m_pIOBoardImpl->close();
		}
		else
		{
			return -1;
		}
	}

	//!������ʱ��
	int CIODrvBoard::SetTimer(long nTimeOut)
	{
		if (m_pIOBoardImpl)
		{
			return m_pIOBoardImpl->SetTimer(nTimeOut);
		}
		else
		{
			return -1;
		}
	}

	//!ֹͣ��ʱ��
	int CIODrvBoard::StopTimer()
	{
		if (m_pIOBoardImpl)
		{
			return m_pIOBoardImpl->StopTimer();
		}
		else
		{
			return -1;
		}
	}

	//!��������״̬
	bool CIODrvBoard::ConnState()
	{
		if (m_pIOBoardImpl)
		{
			return m_pIOBoardImpl->ConnState();
		}
		else
		{
			return false;
		}
	}

	//!�Ͽ�����
	int CIODrvBoard::StopConn()
	{
		if (m_pIOBoardImpl)
		{
			return m_pIOBoardImpl->StopConn();
		}
		else
		{
			return -1;
		}
	}

	//!��������
	bool CIODrvBoard::Connect(long nTimeOut)
	{
		if (m_pIOBoardImpl)
		{
			return m_pIOBoardImpl->Connect(nTimeOut);
		}
		else
		{
			return false;
		}
	}

	//!��������
	bool CIODrvBoard::Connect(long nTimeOut, const char* pPeerIP, int nPort)
	{
		if (m_pIOBoardImpl && pPeerIP)
		{
			return m_pIOBoardImpl->Connect(nTimeOut, pPeerIP, nPort);
		}
		else
		{
			return false;
		}
	}

	//!��������
	int CIODrvBoard::Connect()
	{
		if (m_pIOBoardImpl)
		{
			return m_pIOBoardImpl->Connect();
		}
		else
		{
			return -1;
		}
	}

	//!��������
	int CIODrvBoard::Connect(const char* pPeerIP, int nPort)
	{
		if (m_pIOBoardImpl && pPeerIP)
		{
			return m_pIOBoardImpl->Connect(pPeerIP, nPort);
		}
		else
		{
			return false;
		}
	}

	//!�Ƿ������������Ϣ
	bool CIODrvBoard::CanOutput()
	{
		/*if ( g_pIODebugManager )
		{
			return g_pIODebugManager->IsModuleDebug( DEBUG_DRVBOARD );
		}*/
		return false;
	}

	//!���������Ϣ
	void CIODrvBoard::OutputDebug(const char* pchDebugInfo)
	{
		/*if ( g_pIODebugManager )
		{
			g_pIODebugManager->OutputInfo( DEBUG_DRVBOARD, pchDebugInfo );
		}*/
	}
}