/*!
 *	\file	IOSerialBoard.cpp
 *
 *	\brief	������������ʵ����
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	17:34
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
		//��ʼ���๤��
		GENERIC_REGISTER_CLASS( CIOBoardBase, CIOSerialBoard, &g_IOBoardFactory );
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS(  CIOBoardBase, CIOSerialBoard, &g_IOBoardFactory );
		return 0;
	}

	//! ����
	std::string CIOSerialBoard::s_ClassName("CIOSerialBoard");		
	
	//!���췽��
	CIOSerialBoard::CIOSerialBoard()
	{
		m_pComDealThd = new CIOSrlBoardDealThread();
	}

	//!��������
	CIOSerialBoard::~CIOSerialBoard()
	{
		delete m_pComDealThd;
		m_pComDealThd = NULL;
	}

	//!�򴮿�д����
	int CIOSerialBoard::Write(unsigned char* pbyData, int nDataLen)
	{
		return m_pComDealThd->Write(pbyData, nDataLen);
	}

	//!��������
	int CIOSerialBoard::open()
	{
		m_pComDealThd->Init(m_tDrvBoardCfg,m_pPrtclParser,m_nTimeOut);
		m_pComDealThd->open();
		return 0;
	}

	//!�ر�������
	int CIOSerialBoard::close()
	{
		m_pComDealThd->close();
		return 0;
	}

	//!��ģ���Ƿ������������Ϣ
	bool CIOSerialBoard::CanOutput()
	{
		//!SCADA V4��g_pIODebugManager���ܵ�ʵ��
		return false;
	}

	void CIOSerialBoard::OutputDebug(const char* pchDebugInfo)
	{
		//!SCADA V4��g_pIODebugManager���ܵ�ʵ��
	}
	
	std::string CIOSerialBoard::GetClassName()
	{
		return s_ClassName;
	}
}