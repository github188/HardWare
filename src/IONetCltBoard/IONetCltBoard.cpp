/*!
 *	\file	IONetCltBoard.cpp
 *
 *	\brief	以太网客户端的驱动器类实现文件
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	16:56
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IONetCltBoard.h"
#include "RTDBCommon/GenericFactory.h"
#include "IONetCltBoardThread.h"
#include "IOTcpIpHandler.h"
#include "IOTcpIpSvcHandler.h"
#include "IONetSynConnector.h"
#include "IOTcpIpConnector.h"
//#include "IODebugManager.h"

namespace MACS_SCADA_SUD
{
	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		//初始化类工厂
		GENERIC_REGISTER_CLASS( CIOBoardBase, CIONetCltBoard, &g_IOBoardFactory );
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS(CIOBoardBase, CIONetCltBoard, &g_IOBoardFactory );
		return 0;
	}

	//!类名
	std::string CIONetCltBoard::s_ClassName("CIONetCltBoard");

	
	CIONetCltBoard::CIONetCltBoard()
	{
		m_pComDealThd = new CIONetCltBoardThread();
	}

	CIONetCltBoard::~CIONetCltBoard()
	{
		delete m_pComDealThd;
		m_pComDealThd = NULL;
	}

	int CIONetCltBoard::Write(Byte* pbyData, int nDataLen)
	{
		return m_pComDealThd->Write(pbyData,nDataLen);
	}

	int CIONetCltBoard::open()
	{
		m_pComDealThd->Init(m_tDrvBoardCfg, m_pPrtclParser);
		m_pComDealThd->open();

		ACE_OS::sleep(1);
		return 0;
	}

	int CIONetCltBoard::close()
	{
		m_pComDealThd->close();
		return 0;
	}

	bool CIONetCltBoard::ConnState()
	{
		return m_pComDealThd->GetConnector()->IsConnected();
	}

	int CIONetCltBoard::StopConn()
	{
		return m_pComDealThd->GetConnector()->ConnInterrupt();
	}

	bool CIONetCltBoard::Connect(long nTimeOut)
	{
		return m_pComDealThd->GetConnector()->Connect(nTimeOut);
	}

	bool CIONetCltBoard::Connect(long nTimeOut, const char* pPeerIP, int nPort)
	{
		return m_pComDealThd->GetConnector()->Connect(nTimeOut, pPeerIP, nPort);
	}

	int CIONetCltBoard::Connect()
	{
		return m_pComDealThd->GetConnector()->Connect();
	}

	int CIONetCltBoard::Connect(const char* pPeerIP, int nPort)
	{
		return m_pComDealThd->GetConnector()->Connect(pPeerIP, nPort);
	}

	bool CIONetCltBoard::CanOutput()
	{
		//if ( g_pIODebugManager )
		//{
		//	return g_pIODebugManager->IsModuleDebug( DEBUG_COMXBOARD );
		//}
		return false;
	}

	void CIONetCltBoard::OutputDebug(const char* pchDebugInfo)
	{
		//if ( g_pIODebugManager )
		//{
		//	g_pIODebugManager->OutputInfo( DEBUG_COMXBOARD, pchDebugInfo );
		//}
	}

	std::string CIONetCltBoard::GetClassName()
	{
		return s_ClassName;
	}
}