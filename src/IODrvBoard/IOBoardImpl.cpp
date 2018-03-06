/*!
 *	\file	IOBoardImpl.cpp
 *
 *	\brief	�����๤�����������ʵ�����������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��14��	9:12
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
	//!���캯��
	CIOBoardImpl::CIOBoardImpl()
	{
		m_pBoardBase = NULL;
	}

	//!��������
	CIOBoardImpl::~CIOBoardImpl()
	{

	}

	//!���ݲ�ͬ������Ϣ��������������
	int CIOBoardImpl::Init(CIOPrtclParserBase* pPrtclParser, std::string szBoardType, tIOPortCfg portCfg, long nTimeOut)
	{
		tExtInfo *extInfo = NULL;
		if(!g_pIOExtCfgManager)
		{
			return -1;
		}
		extInfo = g_pIOExtCfgManager->GetBoardInfo(szBoardType);

		/* add by sxl in 2009.3.18 ��ΪIOBoardBase.dll�����ǹ̶����صģ���������⴦�����ּ���һ�Σ�
			��Զ�̵���ʱ�ᷢ��ȫ���๤����ָ����ˣ��������󲻳ɹ�;���Ǳ�������û������.
		*/
		if ( stricmp( extInfo->pchClassName, "CIOBoardBase" )==0 )		
		{
			GENERIC_REGISTER_CLASS( CIOBoardBase, CIOBoardBase, &g_IOBoardFactory );
			m_pBoardBase = g_IOBoardFactory.Create( extInfo->pchClassName );
		}
		else
		{
			//!װ����Ӧ������������
			
			ACE_DLL* pDll = SingletonDllManager::instance()->LoadDll(extInfo->pchFileName);
			if(!pDll)
			{
				//! ���������Ϣ
				if ( CIODrvBoard::CanOutput() )
				{
					char pchDebug[DEBUGINFOLEN];
					sprintf( pchDebug, "CIOBoardImpl::Init Loading Driver File %s Failed!", extInfo->pchFileName );	
					CIODrvBoard::OutputDebug( pchDebug );
				}
				return -3;
			}

			//! ���������Ϣ
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

	//!ͨ����·��������
	int CIOBoardImpl::Write(unsigned char* pbyData, int nDataLen)
	{
		return m_pBoardBase->Write(pbyData,nDataLen);
	}

	//!ʹ��IO��������ʼ����
	int CIOBoardImpl::open()
	{
		return m_pBoardBase->open();
	}

	//!ֹͣ��IO������
	int CIOBoardImpl::close()
	{
		return m_pBoardBase->close();
	}

	//!������ʱ��
	int CIOBoardImpl::SetTimer(long nTimeOut)
	{
		return m_pBoardBase->SetTimer( nTimeOut );
	}

	//!ֹͣ��ʱ��
	int CIOBoardImpl::StopTimer()
	{
		return m_pBoardBase->StopTimer();
	}

	//!��������״̬
	bool CIOBoardImpl::ConnState()
	{
		return m_pBoardBase->ConnState();
	}

	//!�Ͽ�����
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
