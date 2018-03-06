/*!
*	\file	FepIEC103Drv.cpp
*
*	\brief	103��վ������Դ�ļ�
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Drv.cpp 1     11-01-21 10:02a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-01-21 10:02a $
*	$Revision: 1 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#include "FepIEC103Drv.h"
#include "FepIEC103Prtcl.h"
#include "FepIEC103Device.h"
#include "Common/CommonXmlParser/PRDCfgParser.h"

namespace MACS_SCADA_SUD
{
	void OnTmOut(void* p);
	std::string CFepIEC103Drv::s_ClassName("CFepIEC103Drv");	//! ����

	std::string CFepIEC103Drv::GetClassName()
	{
		return s_ClassName;
	}

	CFepIEC103Drv::CFepIEC103Drv()
	{
		m_iConnTimes		= 0;
		m_iSerAddrNum		= 0;
		m_iCurSerAddrIndex	= 0;
		m_pConnTimer = NULL;
		m_bCanWork = false;
		m_bIsConnecting = false;
		m_pPrdCfgParser = NULL;
	}

	CFepIEC103Drv::~CFepIEC103Drv()
	{
		if (m_pConnTimer)
		{
			delete m_pConnTimer;
			m_pConnTimer = NULL;
		}
		if (m_pPrdCfgParser)
		{
			delete m_pPrdCfgParser;
			m_pPrdCfgParser = NULL;
		}
	}

	/*!
	*	\brief	�õ��̳�PrtclParserBase����ļ̳�����
	*
	*	\retval	����ֵ���̳�����
	*/
	std::string CFepIEC103Drv::GetPrtclClassName()
	{
		return "CFepIEC103Prtcl";
	}

	std::string CFepIEC103Drv::GetDeviceClassName()
	{
		return "CFepIEC103Device";
	}


	OpcUa_Int32 CFepIEC103Drv::OpenChannel()
	{
		//!ͨ��������飬�˿ڲ�������
		if ( 0 != CIODriver::OpenChannel() )
		{
			return -1;
		}
		m_pIODrvBoard = new CIODrvBoard();
		if ( m_pIODrvBoard == NULL )
		{
			return -2;
		}
		//!ʹ������IOSerialBoard��ʼ����
		std::string strPortType = m_pPortCfg.strType.toUtf8();
		m_pIODrvBoard->Init( m_pIOPrtclParser, strPortType, m_pPortCfg, this->GetTimeOut());	
		m_pIODrvBoard->open();	

		//!���й���¼�������ļ�����
		std::string strPRDCfgFile = CIODriver::GetFEPRunDir() + "PRDCfgFile.xml";
		m_pPrdCfgParser = new CPRDCfgParser();
		m_pPrdCfgParser->ParseMainInfo(strPRDCfgFile);
		return 0;
	}

	OpcUa_Int32 CFepIEC103Drv::StartWork(void)
	{			
		CFepIEC103Prtcl* pPrtcl = 
			dynamic_cast<CFepIEC103Prtcl*>(m_pIOPrtclParser);
		std::string strTemp = GetDriverName();
		pPrtcl->InitConfigPar((OpcUa_CharA*)strTemp.c_str());

		//!��������·Ϊ��̫��
		if (1==pPrtcl->m_byPrtclType)
		{
			InitNetParam();

			pPrtcl->m_nMaxLinkOffline = m_iSerAddrNum*pPrtcl->m_nMaxOfflineCount*GetDeviceCount()*pPrtcl->m_nRcvTimeOut/1000;
			OpcUa_CharA pchMsg[128];
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Drv::StartWork: TCP��·�������ʱ�䣨IP��·����%d*�����豸������ߴ���%d*�豸����%d*��ʱʱ��%d(����)��= %d ��", 
				m_iSerAddrNum,pPrtcl->m_nMaxOfflineCount, GetDeviceCount(),pPrtcl->m_nRcvTimeOut, pPrtcl->m_nMaxLinkOffline );
			WriteLogAnyWay(pchMsg);
			Connect();
		}
		else
		{
			m_iSerAddrNum = 1;
			pPrtcl->m_nMaxLinkOffline = pPrtcl->m_nMaxOfflineCount*GetDeviceCount()*pPrtcl->m_nRcvTimeOut/1000;
			OpcUa_CharA pchMsg[128];
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Drv::StartWork: ������·�������ʱ�䣨�����豸������ߴ���%d*�豸����%d*��ʱʱ��%d(����)��= %d ��", 
				 pPrtcl->m_nMaxOfflineCount, GetDeviceCount(),pPrtcl->m_nRcvTimeOut, pPrtcl->m_nMaxLinkOffline );
			WriteLogAnyWay(pchMsg);
			//! ֱ�ӷ��͵�һ֡����
			OpcUa_Int32 iReturnLen = 0;
			pPrtcl->BuildRequest( pPrtcl->m_pbySndBuffer, iReturnLen );

			//! ������ʱ��
			if ( pPrtcl->m_pTimeOutTimer->SetTimer( pPrtcl->m_nRcvTimeOut ) == 0 )
			{
				snprintf( pchMsg, sizeof(pchMsg), 
					"CFepIEC103Drv::StartWork Master SetTimer Success,TimerID = %d....", pPrtcl->m_pTimeOutTimer->GetTimerID() );
				WriteLog(pchMsg);
			}
			else
			{
				snprintf( pchMsg, sizeof(pchMsg), 
					"CFepIEC103Drv::StartWork Master SetTimer Failed,TimerID = %d!!!!!", pPrtcl->m_pTimeOutTimer->GetTimerID() );
				WriteLog(pchMsg);
			}
		}
		m_bCanWork = true;
		//! �������ڶ�ʱ��
		WriteLogAnyWay( "StartWork: to call m_pPeriodTimer->SetTimer" );
		pPrtcl->m_pPeriodTimer->SetTimer( PERIOD_SCAN_MS );
		return 0;
	}
	//! ��ʼ���������Ӳ���
	OpcUa_Int32 CFepIEC103Drv::InitNetParam()
	{
		m_pConnTimer = new CIOTimer(OnTmOut,this);
		m_iPort	= m_pPortCfg.nPort;		
		AddSerAddr(m_pPortCfg.strIPA_A.toUtf8());
		AddSerAddr(m_pPortCfg.strIPA_B.toUtf8());
		AddSerAddr(m_pPortCfg.strIPB_A.toUtf8());
		AddSerAddr(m_pPortCfg.strIPB_B.toUtf8());

		return 0;
	}

	//! ��ӷ�����
	void CFepIEC103Drv::AddSerAddr(const OpcUa_CharA* pchSerAddr)   
	{
		if(pchSerAddr && strcmp( pchSerAddr,"")!=0 )
		{
			if( m_iSerAddrNum <= 3 )
			{
				m_strSerAddrList[m_iSerAddrNum++] = pchSerAddr;


				OpcUa_CharA pchMsg[128];
				snprintf( pchMsg, sizeof(pchMsg), 
					"CFepIEC103Drv::Peer IP=%s", pchSerAddr );
				WriteLogAnyWay(pchMsg);
			}		
		}		
	}

	std::string CFepIEC103Drv::GetSerAddr( OpcUa_Int32 Num )
	{
		std::string strRet = "";
		if( Num < m_iSerAddrNum && Num >= 0 )
		{
			strRet =  m_strSerAddrList[Num];
		}	

		return strRet;
	}

	OpcUa_Int32 CFepIEC103Drv::GetSerAddrNum()
	{
		return m_iSerAddrNum;
	}

	OpcUa_Int32 CFepIEC103Drv::GetCurSerAddrIndex()
	{
		return m_iCurSerAddrIndex;
	}


	//! ��װһ�����Ӳ���������Ϊ���������
	void CFepIEC103Drv::Connect(OpcUa_Int32 nIndex)
	{		
		if( nIndex >= 0 && nIndex < m_iSerAddrNum )			
		{
			OpcUa_CharA pchLog[256];
			sprintf( pchLog, "���ӷ�����%s, ���Ϊ%d ...",
				m_strSerAddrList[nIndex].c_str(), nIndex );
			WriteLogAnyWay( pchLog );

			//����ǰ������շ������������������豸���ߺ�����ʱ���������������ӳɹ�����Щ�豸����Զ���ߡ�
			for ( OpcUa_Int32 i = 0; i < GetDeviceCount(); i++ )
			{
				CFepIEC103Device* pDevice = dynamic_cast<CFepIEC103Device*>(GetDeviceByIndex(i));
				if ( pDevice )
				{
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes = 0;
				}
			}

			//! �����Ӷ�ʱ��
			CFepIEC103Prtcl* pPrtcl = 
				dynamic_cast<CFepIEC103Prtcl*>(m_pIOPrtclParser);
			OpcUa_Int32 nR = m_pConnTimer->SetTimer( pPrtcl->m_iConnTimeOut );
			if ( nR < 0 )
			{
				//! ��¼��־
				OpcUa_CharA chMsg[256];
				sprintf( chMsg, "ERROR��IEC103::ReConnect: SetTimer Error, nRet = %d!", nR );
				WriteLog(chMsg,true);
			}

			m_bIsConnecting = true;
			m_pIODrvBoard->Connect( m_strSerAddrList[nIndex].c_str(), m_iPort );
			m_iCurSerAddrIndex = nIndex;
		}
		return;
	}

	//! TCP���Ӷ�ʱ����ʱ�ص�����
	void OnTmOut(void* p)					
	{
		CFepIEC103Drv* pDriver = (CFepIEC103Drv*)p;
		//!�رն�ʱ��
		pDriver->GetConnTimer()->KillTimer();

		//! ������
		//ACE_Guard<ACE_Recursive_Thread_Mutex> guard( pDriver->m_Mutex );

		pDriver->m_bIsConnecting = false;

		OpcUa_CharA chMsg[128];
		sprintf( chMsg, "FepIEC103::OnTmOut: ��������%s��ʱ!", pDriver->GetCurServerAddr().c_str() );
		//! ��¼��־		
		pDriver->WriteLogAnyWay(chMsg,true);

		//! ����������ˣ�������������
		if(pDriver->m_pIODrvBoard->ConnState())
		{
			pDriver->SetConnTime( 0 );	
			sprintf( chMsg, "OnTmOut: ����%s�����ӳɹ���return!", pDriver->GetCurServerAddr().c_str() );
			//! ��¼��־
			pDriver->WriteLogAnyWay( chMsg,true );
			return;
		}

		//! ���豸����
		for( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++)
		{
			CFepIEC103Device* pDevice = dynamic_cast<CFepIEC103Device*>(pDriver->GetDeviceByIndex(i));

			if ( pDevice && !pDevice->IsSimulate() )
			{
				if ( pDevice->IsOnLine() )
				{
					OpcUa_CharA pchMsg[128];
					snprintf( pchMsg, sizeof(pchMsg), 
						"�������糬ʱ���豸%s(��ַ%d)���ɣ�", pDevice->GetName().c_str(), pDevice->GetAddr() );

					pDriver->WriteLogAnyWay(pchMsg);
				}

				pDevice->SetSuspicious( true );
			}
		}

		//! ���豸���ߵ�������������·IP������ʧ��
		if( pDriver->m_iCurSerAddrIndex >= pDriver->m_iSerAddrNum-1 )
		{
			for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				CFepIEC103Device* pDevice = dynamic_cast<CFepIEC103Device*>(pDriver->GetDeviceByIndex(i));
				if ( NULL != pDevice )
				{
					if ( pDevice->m_byRxTimes == 0 )
					{
						if ( !pDevice->IsOffLine() )
						{
							OpcUa_CharA pchMsg[128];
							snprintf( pchMsg, sizeof(pchMsg), 
								"�����豸����·IP������ʧ�ܣ��豸%s(��ַ%d)�����ˣ�", pDevice->GetName().c_str(), pDevice->GetAddr() );

							pDriver->WriteLogAnyWay(pchMsg);
						}
						pDevice->SetOnLine( DEVICE_OFFLINE );
						pDevice->m_byTxTimes = 0;
						pDevice->m_byRxTimes = 0;
					}
				}
			}
		}


		//! ����
		pDriver->m_bIsConnecting = true;

		pDriver->ReConnect(true);

	}

	/*!
	*	\brief	����
	*
	*	�������ʧ�ܣ���������
	*	��������������m_byMaxReConn������ʧ������һ��������
	*
	*/
	void CFepIEC103Drv::ReConnect( bool bNext )
	{
		//! ����������1
		IncrConnTime();

		CFepIEC103Prtcl* pPrtcl = 
			dynamic_cast<CFepIEC103Prtcl*>(m_pIOPrtclParser);


		//! �������������������������һ��������
		if( bNext || ( m_iConnTimes > pPrtcl->m_iMaxReConn ) )
		{
			m_iConnTimes = 0;
			ConnectNext();
		}
		else
		{
			//! ����
			Connect(m_iCurSerAddrIndex);
		}

		return;
	}

	/*!
	*	\brief	������һ��������
	*/
	void CFepIEC103Drv::ConnectNext()
	{
		m_iConnTimes = 0;
		if( ++m_iCurSerAddrIndex >= m_iSerAddrNum)
			m_iCurSerAddrIndex = 0;

		Connect( m_iCurSerAddrIndex );
		return;
	}

	OpcUa_Int32 CFepIEC103Drv::WriteLogAnyWay( const OpcUa_CharA* pchLog, bool bTime /*= true */ )
	{
		if ( !m_pLog )
		{
			return -1;
		}

		m_pLog->WriteLog( pchLog, bTime );

		return 0;
	}

	bool CFepIEC103Drv::CanWork()
	{
		return m_bCanWork;
	}

	CPRDCfgParser* CFepIEC103Drv::GetPRDCfgParser()
	{
		if (m_pPrdCfgParser)
		{
			return m_pPrdCfgParser;
		}
		else
		{
			return NULL;
		}
	}
}
