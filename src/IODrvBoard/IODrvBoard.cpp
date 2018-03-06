/*!
 *	\file	IODrvBoard.cpp
 *
 *	\brief	驱动层对外接口实现
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月10日	9:40
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
	//!构造函数
	CIODrvBoard::CIODrvBoard()
	{
		m_pIOBoardImpl=new CIOBoardImpl();
	}

	//!析构函数
	CIODrvBoard::~CIODrvBoard()
	{
		if ( m_pIOBoardImpl )
		{
			delete m_pIOBoardImpl;
			m_pIOBoardImpl = NULL;
		}
	}

	//!驱动器类初始化
	int CIODrvBoard::Init(CIOPrtclParserBase* pPrtclParser, std::string szBoardType, tIOPortCfg portCfg, long nTimeOut)
	{
		return m_pIOBoardImpl->Init(pPrtclParser,szBoardType,portCfg,nTimeOut);
	}

	//!通过链路发送数据
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

	//!使IO驱动器开始工作
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

	//!停止该IO驱动器
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

	//!启动定时器
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

	//!停止定时器
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

	//!返回连接状态
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

	//!断开连接
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

	//!进行连接
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

	//!进行连接
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

	//!进行连接
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

	//!进行连接
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

	//!是否能输出调试信息
	bool CIODrvBoard::CanOutput()
	{
		/*if ( g_pIODebugManager )
		{
			return g_pIODebugManager->IsModuleDebug( DEBUG_DRVBOARD );
		}*/
		return false;
	}

	//!输出调试信息
	void CIODrvBoard::OutputDebug(const char* pchDebugInfo)
	{
		/*if ( g_pIODebugManager )
		{
			g_pIODebugManager->OutputInfo( DEBUG_DRVBOARD, pchDebugInfo );
		}*/
	}
}