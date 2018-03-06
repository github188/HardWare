/*!
 *	\file	IOBoardBase.cpp
 *
 *	\brief	����������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Taiwanxia
 *
 *	\date	2014��4��10��	10:45
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBBaseClasses/IOBoardBase.h"

namespace MACS_SCADA_SUD
{
	//!�����������๤��
	DBBASECLASSES_API GenericFactory<CIOBoardBase> g_IOBoardFactory;

	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		//��ʼ���๤��
		GENERIC_REGISTER_CLASS( CIOBoardBase, CIOBoardBase, &g_IOBoardFactory );
		return 0;
	}
	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS(  CIOBoardBase, CIOBoardBase, &g_IOBoardFactory );
		return 0;
	}

	//!��������
	std::string CIOBoardBase::s_ClassName("CIOBoardBase");


	CIOBoardBase::CIOBoardBase():m_nTimeOut(0)
	{
		m_pPrtclParser = NULL;
	}

	CIOBoardBase::~CIOBoardBase()
	{

	}
	
	//!��ʼ��
	int CIOBoardBase::Init(CIOPrtclParserBase* pPrtclParser, tIOPortCfg portCfg, long nTimeOut)
	{
		m_pPrtclParser = pPrtclParser;
		m_tDrvBoardCfg = portCfg;
		m_nTimeOut = nTimeOut;
		return 0;
	}

	//!ͨ����·��������
	int CIOBoardBase::Write( unsigned char* pbyData, int nDataLen )
	{
		return 0;
	}

	//!ʹ����������ʼ����
	int CIOBoardBase::open( )
	{
		return 0;
	}

	//!ֹͣ��IO������
	int CIOBoardBase::close()
	{
		return 0;
	}

	//!�õ�����
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

	//!��������״̬
	bool CIOBoardBase::ConnState()
	{
		return false;
	}

	//! ����
	bool CIOBoardBase::Connect( long nTimeOut )
	{
		return false;
	}

	//! ����
	bool CIOBoardBase::Connect( long nTimeOut, const char* pPeerIP, int nPort )
	{
		return false;
	}

	//! ����
	int  CIOBoardBase::Connect( const char* pPeerIP, int nPort )
	{
		return 0;
	}

	//! ����
	int  CIOBoardBase::Connect( )
	{
		return 0;
	}

	//!	�Ͽ�����
	int CIOBoardBase::StopConn()
	{
		return 0;
	}
}