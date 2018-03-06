/*!
 *	\file	IOBoardImpl.cpp
 *
 *	\brief	调用类工厂，创建合适的驱动处理类
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月14日	9:12
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "utilities/fepcommondef.h"
#include "IOBoardImpl.h"
#include "IODrvBoard/IODrvBoard.h"
#include "RTDBCommon/ClassDllManager.h"
#include "RTDBCommon/RuntimeCatlog.h"
#include "ace/DLL.h"
#include "../IOHelpCfgManager/IOHelpCfgManager.h"

namespace MACS_SCADA_SUD
{
	//!构造函数
	CIOBoardImpl::CIOBoardImpl()
	{
		m_pBoardBase = NULL;
	}

	//!析构函数
	CIOBoardImpl::~CIOBoardImpl()
	{

	}

	//!根据不同配置信息创建驱动器对象
	int CIOBoardImpl::Init(CIOPrtclParserBase* pPrtclParser, std::string szBoardType, tIOPortCfg portCfg, long nTimeOut)
	{
		tExtInfo *extInfo = NULL;
		if(!g_pIOExtCfgManager)
		{
			return -1;
		}
		extInfo = g_pIOExtCfgManager->GetBoardInfo(szBoardType);

		/* add by sxl in 2009.3.18 因为IOBoardBase.dll本身是固定加载的，如果不特殊处理则被又加载一次，
			当远程调试时会发现全局类工厂的指针变了，创建对象不成功;但是本机运行没有问题.
		*/
		if ( stricmp( extInfo->pchClassName, "CIOBoardBase" )==0 )		
		{
			GENERIC_REGISTER_CLASS( CIOBoardBase, CIOBoardBase, &g_IOBoardFactory );
			m_pBoardBase = g_IOBoardFactory.Create( extInfo->pchClassName );
		}
		else
		{
			//!装载相应串口类驱动库
			
			ACE_DLL* pDll = SingletonDllManager::instance()->LoadDll(extInfo->pchFileName);
			if(!pDll)
			{
				//! 输出调试信息
				if ( CIODrvBoard::CanOutput() )
				{
					char pchDebug[DEBUGINFOLEN];
					sprintf( pchDebug, "CIOBoardImpl::Init Loading Driver File %s Failed!", extInfo->pchFileName );	
					CIODrvBoard::OutputDebug( pchDebug );
				}
				return -3;
			}

			//! 输出调试信息
			if ( CIODrvBoard::CanOutput() )
			{
				char pchDebug[DEBUGINFOLEN];
				sprintf( pchDebug, "CIOBoardImpl::Init Loading Driver File %s Successed!", extInfo->pchFileName );	
				CIODrvBoard::OutputDebug( pchDebug );
			}

			m_pBoardBase = g_IOBoardFactory.Create( extInfo->pchClassName );

			if ( !m_pBoardBase )
			{
				return -4;
			}	
	
		}

		m_pBoardBase->Init(pPrtclParser,portCfg,nTimeOut);
		return 0;
	}

	//!通过链路发送数据
	int CIOBoardImpl::Write(unsigned char* pbyData, int nDataLen)
	{
		return m_pBoardBase->Write(pbyData,nDataLen);
	}

	//!使该IO驱动器开始工作
	int CIOBoardImpl::open()
	{
		return m_pBoardBase->open();
	}

	//!停止该IO驱动器
	int CIOBoardImpl::close()
	{
		return m_pBoardBase->close();
	}

	//!启动定时器
	int CIOBoardImpl::SetTimer(long nTimeOut)
	{
		return m_pBoardBase->SetTimer( nTimeOut );
	}

	//!停止定时器
	int CIOBoardImpl::StopTimer()
	{
		return m_pBoardBase->StopTimer();
	}

	//!返回连接状态
	bool CIOBoardImpl::ConnState()
	{
		return m_pBoardBase->ConnState();
	}

	//!断开连接
	int CIOBoardImpl::StopConn()
	{
		return m_pBoardBase->StopConn();
	}
	
	bool CIOBoardImpl::Connect(long nTimeOut)
	{
		return m_pBoardBase->Connect(nTimeOut);
	}

	bool CIOBoardImpl::Connect(long nTimeOut, const char* pPeerIP, int nPort)
	{
		return m_pBoardBase->Connect(nTimeOut, pPeerIP, nPort);
	}

	int CIOBoardImpl::Connect()
	{
		return m_pBoardBase->Connect();
	}

	int CIOBoardImpl::Connect(const char* pPeerIP, int nPort)
	{
		return m_pBoardBase->Connect(pPeerIP, nPort);
	}
}
