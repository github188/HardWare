/*!
 *	\file	FepIEC104Drv.cpp
 *
 *	\brief	104��վ������ʵ��Դ�ļ�
 *
 *	$Header: ?/MACS-SCADA 3.2 �������/MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Drv.cpp, 219+1 2011/04/11 07:14:19 miaoweijie $
 *	$Author: miaoweijie $
 *	$Date: 2011/04/11 07:14:19 $
 *	$Revision: 219+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "FepIEC104Drv.h"
#include "FepIEC104Prtcl.h"
#include "ace/Init_ACE.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"

namespace MACS_SCADA_SUD
{
	void OnConnectTimeOut(void* p);							//! ��ʱ����ʱ�ص�����

	std::string CFepIEC104Drv::s_ClassName("CFepIEC104Drv");		//! ����

	CFepIEC104Drv::~CFepIEC104Drv()
	{
		//! ɾ����ʱ��
		if(m_pConnTimer)
		{
			delete m_pConnTimer;
			m_pConnTimer = NULL;
		}
	}

	std::string CFepIEC104Drv::GetClassName()
	{
		return s_ClassName;
	}

	/*!
	*	\brief	�õ��̳�PrtclParserBase����ļ̳�����
	*
	*	\retval	����ֵ���̳�����
	*/
	std::string CFepIEC104Drv::GetPrtclClassName()
	{
		return "CFepIEC104Prtcl";
	}

	/*!
	*	\brief	�õ��̳�DeviceBase����ļ̳�����
	*
	*	\retval	����ֵ���̳�����
	*/
	std::string CFepIEC104Drv::GetDeviceClassName()
	{
		return "CFepIEC104Device";
	}

	OpcUa_Int32 CFepIEC104Drv::StartWork(void)
	{
		WriteLogAnyWay( "StartWork: Begin ..." );

		m_bHasConnected = false;
		m_pIODrvBoard = new CIODrvBoard();
		if ( m_pIODrvBoard == NULL )
		{
			MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIODriver::StartWork:m_pIODrvBoard is Null")));
			//! ���������Ϣ
// 			if ( g_pIODebugManager && g_pIODebugManager->IsModuleDebug( DEBUG_DRIVERFRAME ) )
// 			{
// 				g_pIODebugManager->OutputInfo( 
// 					DEBUG_DRIVERFRAME, "CIODriver::StartWork:m_pIODrvBoard is Null!" );
// 			}

			WriteLogAnyWay( "StartWork:m_pIODrvBoard is Null!" );

			return -2;
		}
		std::string strPortType = m_pPortCfg.strType.toUtf8();
		m_pIODrvBoard->Init( m_pIOPrtclParser, strPortType, m_pPortCfg, this->GetTimeOut());

		m_pIODrvBoard->open();

		CFepIEC104Prtcl* pPrtcl = dynamic_cast<CFepIEC104Prtcl*>(m_pIOPrtclParser);
		std::string strTemp = GetDriverName();

		//! ��ʼ����·����
		pPrtcl->InitConfigPar( (char*)strTemp.c_str() );

		////////////////////////////////////////////////////////
		//! ��ʼ���������
		InitNetParam();

		//! Connect and Start Timer
		Connect();

		//! ��ʱ��
		//m_pConnTimer->SetTimer(MACSIEC104CONN_MAX_TIMEOUT);
		////////////////////////////////////////////////////////

		//��ʼ���ڶ�ʱ��
		WriteLogAnyWay( "StartWork: to call m_pPeriodTimer->SetTimer" );
		pPrtcl->m_pPeriodTimer->SetTimer( PERIOD_SCAN_MS );


		//! ���������Ϣ
// 		if ( g_pIODebugManager && g_pIODebugManager->IsModuleDebug( DEBUG_DRIVERFRAME ) )
// 		{
// 			g_pIODebugManager->OutputInfo( DEBUG_DRIVERFRAME, "CIODriver::StartWork !" );
// 		}

		WriteLogAnyWay( "StartWork: End!" );

		return 0;
	}

	//! ��ʼ���������Ӳ���
	OpcUa_Int32 CFepIEC104Drv::InitNetParam()
	{
		m_pConnTimer = new CIOTimer(OnConnectTimeOut, this);

		m_nConnTimes		= 0;
		m_nSerAddrNum		= 0;
		m_nCurSerAddrIndex	= 0;

		//! �������˿ں�
		m_nPort	= m_pPortCfg.nPort;		
		AddSerAddr(m_pPortCfg.strIPA_A.toUtf8());
		AddSerAddr(m_pPortCfg.strIPA_B.toUtf8());
		AddSerAddr(m_pPortCfg.strIPB_A.toUtf8());
		AddSerAddr(m_pPortCfg.strIPB_B.toUtf8());

		return 0;
	}

	//! ��ӷ�����
	void CFepIEC104Drv::AddSerAddr(const char* pchSerAddr)
	{
		if(pchSerAddr && strcmp( pchSerAddr,"")!=0 )
			m_stSerAddrList[m_nSerAddrNum++] = pchSerAddr;
	}

	//! ��װһ�����Ӳ���������Ϊ���������
	OpcUa_Int32 CFepIEC104Drv::Connect(OpcUa_Int32 nIndex)
	{
		if( nIndex < 0 || nIndex >= m_nSerAddrNum )
			return -1;

		char pchLog[256];
		sprintf( pchLog, "���ӷ�����%s, ���Ϊ%d ...",
			m_stSerAddrList[nIndex].c_str(), nIndex );
		WriteLogAnyWay( pchLog );

		SetConnTimer( MACSIEC104CONN_MAX_TIMEOUT );

		IsConnecting(true);
		m_pIODrvBoard->Connect( m_stSerAddrList[nIndex].c_str(), m_nPort );
		m_nCurSerAddrIndex = nIndex;	


		return 0;
	}

	/*!
	*	\brief	����
	*
	*	�������ʧ�ܣ���������
	*	��������������m_byMaxReConn������ʧ������һ��������
	*
	*/
	void CFepIEC104Drv::ReConnect()
	{
		//! ����������1
		IncrConnTimes();

		char pchLog[256];	
		sprintf( pchLog, "��������Ϊ%d ...", m_nConnTimes );
		WriteLogAnyWay( pchLog );

		//! �������������������������һ��������
		if( m_nConnTimes >= MACSIEC104DRIVER_CONNECT_MAXTIMES )
		{
			ResetConnTimes();
			Connect( NextSerAddrIndex() );			
		}
		else
		{
			//! ����
			Connect( m_nCurSerAddrIndex );
		}

		//! �����Ӷ�ʱ��
		//m_pConnTimer->SetTimer( MACSIEC104CONN_MAX_TIMEOUT );

		return;
	}


	//! ��ʱ����ʱ�ص�����
	void OnConnectTimeOut(void* p)					
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)p;		

		pDriver->IsConnecting(false);	

		CFepIEC104Prtcl* pPrtcl = dynamic_cast<CFepIEC104Prtcl*>(pDriver->m_pIOPrtclParser);
		if ( pPrtcl == NULL )
		{
			pDriver->WriteLogAnyWay( "Error��OnConnectTimeOut: CFepIEC104Prtcl is NULL " );
			return;
		}

		//! ��־
		char pchInfo[256];
		sprintf( pchInfo, "Error��OnConnectTimeOut�����ӷ�����%s��ʱ...", pDriver->GetCurServerIP().c_str() );
		pDriver->WriteLogAnyWay( pchInfo );

		pDriver->KillConnTimer();
	}

	OpcUa_Int32 CFepIEC104Drv::WriteLogAnyWay( const char* pchLog, bool bTime )
	{
		if ( !m_pLog )
		{
			return -1;
		}

		m_pLog->WriteLog( pchLog, bTime );

		return 0;
	}

}
