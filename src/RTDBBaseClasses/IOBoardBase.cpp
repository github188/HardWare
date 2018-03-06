/*!
 *	\file	IOBoardBase.cpp
 *
 *	\brief	驱动器基类实现
 *
 *	详细的文件说明（可选）
 *
 *	\author Taiwanxia
 *
 *	\date	2014年4月10日	10:45
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBBaseClasses/IOBoardBase.h"

namespace MACS_SCADA_SUD
{
	//!定义驱动器类工厂
	DBBASECLASSES_API GenericFactory<CIOBoardBase> g_IOBoardFactory;

	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		//初始化类工厂
		GENERIC_REGISTER_CLASS( CIOBoardBase, CIOBoardBase, &g_IOBoardFactory );
		return 0;
	}
	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS(  CIOBoardBase, CIOBoardBase, &g_IOBoardFactory );
		return 0;
	}

	//!定义类名
	std::string CIOBoardBase::s_ClassName("CIOBoardBase");


	CIOBoardBase::CIOBoardBase():m_nTimeOut(0)
	{
		m_pPrtclParser = NULL;
	}

	CIOBoardBase::~CIOBoardBase()
	{

	}
	
	//!初始化
	int CIOBoardBase::Init(CIOPrtclParserBase* pPrtclParser, tIOPortCfg portCfg, long nTimeOut)
	{
		m_pPrtclParser = pPrtclParser;
		m_tDrvBoardCfg = portCfg;
		m_nTimeOut = nTimeOut;
		return 0;
	}

	//!通过链路发送数据
	int CIOBoardBase::Write( unsigned char* pbyData, int nDataLen )
	{
		return 0;
	}

	//!使该驱动器开始工作
	int CIOBoardBase::open( )
	{
		return 0;
	}

	//!停止该IO驱动器
	int CIOBoardBase::close()
	{
		return 0;
	}

	//!得到类名
	std::string CIOBoardBase::GetClassName()
	{
		return s_ClassName;
	}

	int CIOBoardBase::SetTimer( long nTimeOut )
	{
		return 0;
	}

	int CIOBoardBase::StopTimer()
	{
		return 0;
	}

	//!返回连接状态
	bool CIOBoardBase::ConnState()
	{
		return false;
	}

	//! 连接
	bool CIOBoardBase::Connect( long nTimeOut )
	{
		return false;
	}

	//! 连接
	bool CIOBoardBase::Connect( long nTimeOut, const char* pPeerIP, int nPort )
	{
		return false;
	}

	//! 连接
	int  CIOBoardBase::Connect( const char* pPeerIP, int nPort )
	{
		return 0;
	}

	//! 连接
	int  CIOBoardBase::Connect( )
	{
		return 0;
	}

	//!	断开连接
	int CIOBoardBase::StopConn()
	{
		return 0;
	}
}