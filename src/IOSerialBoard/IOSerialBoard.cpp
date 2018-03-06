/*!
 *	\file	IOSerialBoard.cpp
 *
 *	\brief	串口类驱动器实现类
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月10日	17:34
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IOSerialBoard.h"
#include "IOSrlBoardDealThread.h"

namespace MACS_SCADA_SUD
{
	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		//初始化类工厂
		GENERIC_REGISTER_CLASS( CIOBoardBase, CIOSerialBoard, &g_IOBoardFactory );
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS(  CIOBoardBase, CIOSerialBoard, &g_IOBoardFactory );
		return 0;
	}

	//! 类名
	std::string CIOSerialBoard::s_ClassName("CIOSerialBoard");		
	
	//!构造方法
	CIOSerialBoard::CIOSerialBoard()
	{
		m_pComDealThd = new CIOSrlBoardDealThread();
	}

	//!析构方法
	CIOSerialBoard::~CIOSerialBoard()
	{
		delete m_pComDealThd;
		m_pComDealThd = NULL;
	}

	//!向串口写数据
	int CIOSerialBoard::Write(unsigned char* pbyData, int nDataLen)
	{
		return m_pComDealThd->Write(pbyData, nDataLen);
	}

	//!打开驱动器
	int CIOSerialBoard::open()
	{
		m_pComDealThd->Init(m_tDrvBoardCfg,m_pPrtclParser,m_nTimeOut);
		m_pComDealThd->open();
		return 0;
	}

	//!关闭驱动器
	int CIOSerialBoard::close()
	{
		m_pComDealThd->close();
		return 0;
	}

	//!本模块是否能输出调试信息
	bool CIOSerialBoard::CanOutput()
	{
		//!SCADA V4中g_pIODebugManager功能的实现
		return false;
	}

	void CIOSerialBoard::OutputDebug(const char* pchDebugInfo)
	{
		//!SCADA V4中g_pIODebugManager功能的实现
	}
	
	std::string CIOSerialBoard::GetClassName()
	{
		return s_ClassName;
	}
}