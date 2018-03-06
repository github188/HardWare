/*!
*	\file	FepIEC103Prtcl.cpp
*
*	\brief	103��վЭ����Դ�ļ�
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Prtcl.cpp 5     11-02-24 11:25a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-02-24 11:25a $
*	$Revision: 5 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#include "FepIEC103Prtcl.h"
#include "FepIEC103Device.h"
#include "FepIEC103Drv.h"
#include "IEC103.h"
#include "PublicFileH.h"
//#include "../../Server/IOFrameAccess/IOFrameAccess.h"

#include <math.h>

namespace MACS_SCADA_SUD
{
	std::string CFepIEC103Prtcl::s_ClassName("CFepIEC103Prtcl");
	void OnTimeOut( void* pParent );			
	void PeriodProc( void* pParent );

	CFepIEC103Prtcl::CFepIEC103Prtcl()
	{
	//	InitPrtclHelper( 5, 64, 1024, 255, 4096 );	//Del by hxw in 2013.7.31
		InitPrtclHelper( 1, 255, 1024, 255, 4096 );	//Add by hxw in 2013.7.31
		m_bRTUChannelTest = false;
		m_byPrtclType = 0;
		m_iConnTimeOut = 2;
		//��ʼ״̬	
		m_iCurDevIndex = 0;
		m_bIsAskForSendPacket   = false;
		m_bEnableFaultWave = false;
		m_nWholeCallPeriodASDU7 = 0;
		m_nWholeCallPeriodASDU21 = 0;

		//��ʱ������
		m_nTimeOutCounter = 0;
		m_iDevIndex		= 0;
		m_iDevLastIndex = m_iDevIndex;

		m_nMaxLinkOffline = 300;	//��ʼ��Ϊ300��

		m_nCounter = 1;	//��ʼ��Ϊ1����һ�β�����Уʱ���ܲ�ѯ

		//��ʱ��ʱ��
		m_pTimeOutTimer = new CIOTimer(OnTimeOut, this, true);
		m_pPeriodTimer = new CIOTimer(PeriodProc, this);
		//m_pFaultLogTimer = new CIOTimer(  FaultLogProc, this ,true);
		m_msFaultLogLifeCycle = 300*1000;	//5����

		m_byLastDevAddr = 0;
		m_byLastPeerDevAddr = 0;

		//Э����������		
		ACE_OS::memset(m_pbyInnerBuf,0,4096);
		m_iInnerLen = 0;

		m_bIsTimeForBroadCheckTime = false;	
		m_bIsBroadingCheckTime = false;
	}

	CFepIEC103Prtcl::~CFepIEC103Prtcl()
	{	
		if ( m_pTimeOutTimer )
		{
			m_pTimeOutTimer->KillTimer();
			//delete m_pTimeOutTimer;
			//m_pTimeOutTimer = NULL;
		}
		if (m_pPeriodTimer)
		{
			m_pPeriodTimer->KillTimer();
			//delete m_pPeriodTimer;
			//m_pPeriodTimer = NULL;
		}
		//if (m_pFaultLogTimer)
		//{
		//	delete m_pFaultLogTimer;
		//	m_pFaultLogTimer = NULL;
		//}
	}

	/*!
	*	\brief	���ڼ��Уʱʱ��
	*	
	*	\param	void* pParent	Э�鴦�����	
	*
	*	\retval	����ֵ	��
	*
	*	\note		
	*/
	void PeriodProc( void* pParent )
	{
		//У�����
		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)pParent;
		if ( pPrtcl == NULL )
		{
			return;
		}

		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(pPrtcl->GetParam());
		if ( !pDrv->CanWork() )
		{
			return;
		}

		if (pPrtcl->m_nMaxLinkOffline <= 0)
		{
			return;
		}

		CFepIEC103Device* pDevice = NULL;
		if ( pPrtcl->m_nCounter % (pPrtcl->m_nMaxLinkOffline/pDrv->m_iSerAddrNum) == (pPrtcl->m_nMaxLinkOffline/pDrv->m_iSerAddrNum) -1 )
		{
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
				if ( NULL != pDevice )
				{
					if ( pDevice->m_byRxTimes == 0 )
					{
						if ( !pDevice->IsOffLine() )
						{
							OpcUa_CharA pchMsg[128];
							snprintf( pchMsg, sizeof(pchMsg), 
								"%d��û���յ��κ��豸���ģ��豸%s(��ַ%d)���ɣ�", (pPrtcl->m_nMaxLinkOffline/pDrv->m_iSerAddrNum), pDevice->GetName().c_str(), pDevice->GetAddr() );

							pDrv->WriteLogAnyWay(pchMsg);
						}
						pDevice->SetSuspicious( true );
					}
				}
			}
		}

		if ( pPrtcl->m_nCounter % pPrtcl->m_nMaxLinkOffline == pPrtcl->m_nMaxLinkOffline -1 )
		{
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
				if ( NULL != pDevice  && !pDevice->IsSimulate() )
				{
					if ( pDevice->m_byRxTimes == 0 )
					{
						if ( !pDevice->IsOffLine() )
						{
							OpcUa_CharA pchMsg[128];
							snprintf( pchMsg, sizeof(pchMsg), 
								"����%d�붼û���յ��κ��豸���ģ��豸%s(��ַ%d)�����ˣ�", pPrtcl->m_nMaxLinkOffline, pDevice->GetName().c_str(), pDevice->GetAddr() );

							pDrv->WriteLogAnyWay(pchMsg);
						}
						pDevice->SetOnLine( DEVICE_OFFLINE );
					}

					//ÿ�μ��󸴹�����������򣬲��������豸���ߺ���Щ�豸����Զ���ߡ�
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes = 0;
				}
			}
		}

		//�����·���߻���ɣ������ڳ���������·��ע�⣬��·���ڳ�ʼ��̬��������ʱ������������·��
		if ( 1 == pPrtcl->m_byPrtclType && ( pPrtcl->IsLinkOffline() || pPrtcl->IsLinkSuspicious() ) )
		{
			if ( pPrtcl->m_nCounter % 30 == 30-1 //ÿ30������������һ��
				|| ( pDrv->m_iSerAddrNum > 1 && pDrv->m_iCurSerAddrIndex != 0 )  )	//TCP��·���ж���豸IPʱ������һ�������Ϣ��֮�䲻��Ϣ��
			{
				if ( !pDrv->m_bIsConnecting )
				{
					pDrv->WriteLogAnyWay( "TCP��·���߻���ɣ�����TCP Server��..." );		

					pDrv->m_bIsConnecting = true;
					pDrv->ReConnect( true );
				}
				else
				{
					pDrv->WriteLogAnyWay( "TCP��·�����������Ժ��ٳ�������TCP Server��..." );		
				}
			}
		}
		
		//! Уʱ���ڵ�
		if( pPrtcl->m_nCheckTimePeriod && pPrtcl->m_nCounter % pPrtcl->m_nCheckTimePeriod == 0 )
		{
			if ( pPrtcl->m_bBroadCheckTime )	//�㲥��ʱ
			{
				pPrtcl->m_bIsTimeForBroadCheckTime = true;	//ֻ������·��ʱ��־
			}
			else	//��Ե��ʱ
			{				
				for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
				{
					pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
					if (NULL != pDevice)
					{
						pDevice->m_bNeedCheckTime = true;	//����豸�ö�ʱ��־
					}
				}
			}

			pDrv->WriteLog( "Уʱ���ڵ��ˣ�" );		
		}

		//! ASDU7�ܲ�ѯ���ڵ�
/*
		if( pPrtcl->m_nWholeCallPeriodASDU7 && pPrtcl->m_nCounter % pPrtcl->m_nWholeCallPeriodASDU7 == 0 )
		{
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
				pDevice->m_bNeedASDU7GI = true;	
			}

			pDrv->WriteLog( "ASDU7�ܲ�ѯ���ڵ��ˣ�" );
		}
*/

		//! ASDU21��������ܲ�ѯ���ڵ�
/*
		if( pPrtcl->m_nWholeCallPeriodASDU21 && pPrtcl->m_nCounter % pPrtcl->m_nWholeCallPeriodASDU21 == 0 )
		{
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
				pDevice->m_bNeedASDU21GI = true;	
			}

			pDrv->WriteLog( "ASDU21��������ܲ�ѯ���ڵ��ˣ�" );
		}
*/

		//if( pPrtcl->m_nWholeCallPeriodASDU21 && pPrtcl->m_nCounter % pPrtcl->m_nWholeCallPeriodASDU21 == 0 )
		{
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));

				//���ߵ��豸Ӧ�÷���������
				if ( pPrtcl->m_nMaxRetryPeriod && pPrtcl->m_nCounter%pPrtcl->m_nMaxRetryPeriod == 0 )	//
				{
					pDevice->m_bRetryPeriod = true;
				}

				//! ASDU7�������ٻ����ڵ�
				if( pDevice->m_iASDU7ReadGroupPeriod && pPrtcl->m_nCounter % pDevice->m_iASDU7ReadGroupPeriod == 0 )
				{
					pDevice->m_bNeedASDU7GI = true;	

					OpcUa_CharA pchMsg[128];
					snprintf( pchMsg, sizeof(pchMsg), 
						"ASDU7 �豸%s�ܲ�ѯ���ڵ��ˣ�", pDevice->GetName().c_str() );
					pDrv->WriteLog(pchMsg);

				}

				//! ASDU21�������ٻ����ڵ�
				if( pDevice->m_iASDU21ReadGroupPeriod && pPrtcl->m_nCounter % pDevice->m_iASDU21ReadGroupPeriod == 0 )
				{
					pDevice->m_bNeedASDU21GI = true;	

					OpcUa_CharA pchMsg[128];
					snprintf( pchMsg, sizeof(pchMsg), 
						"ASDU21 �豸%s�����ܲ�ѯ���ڵ��ˣ�", pDevice->GetName().c_str() );
					pDrv->WriteLog(pchMsg);

				}
			}
		}


		pPrtcl->m_nCounter++;
	}

	//// **********************************************************************************************
	//// �� д ��: Hexuewen        ��������ʱ��: 2008.11.27 
	//// ��    ��: FaultLogProc
	//// ��    ��: ����¼����־�������ں���
	//// **********************************************************************************************
	//void FaultLogProc( void* pParent )
	//{
	//	//У�����l
	//	CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)pParent;
	//	if ( pPrtcl == NULL )
	//	{
	//		return;
	//	}
	//	CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(pPrtcl->GetParam());

	//	pPrtcl->m_pFaultLogTimer->KillTimer();

	//	CIOChannelBase* p103Channel = pDrv->GetChannelObj();
	//	//del 2011-1018
	//	//p103Channel->SetMonitor( false );
	//	//p103Channel->SetLog( false );

	//	MACS_ERROR((ERROR_PERFIX ACE_TEXT("IEC103 ����¼����־��¼�������ڵ���")));
	//}

	/*!
	*	\brief	 ���ճ�ʱ����
	*	
	*
	*	\param	����	void*pParent: Э�鴦�����	
	*
	*	\retval	����ֵ	��
	*
	*	\note		���ղ������ݾ��ٴ���������
	*/
	void OnTimeOut( void* pParent )
	{
		//У�����
		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)pParent;
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*) (pPrtcl->m_pParam);
		//CFepIEC103Device* pDevice =dynamic_cast<CFepIEC103Device*>( pDrv->GetDeviceByAddr( pPrtcl->m_byLastDevAddr ) );
		//if (pDevice)
		//{
		//	pDevice->m_pPRDFile->ProduceCfgFile();
		//	pDevice->m_pPRDFile->ProduceDatFile();
		//}
		pDrv->WriteLog( "����������ʱ��ʱ���ص�!!!!" );

		OpcUa_CharA pchMsg[128];
		OpcUa_Int32 nRet = pPrtcl->m_pTimeOutTimer->KillTimer();
		//�ض�ʱ��
		if ( nRet >= 0 )
		{
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Prtcl::TimeOut KillTimer Success,Timers Count = %d....", nRet );
			pDrv->WriteLog(pchMsg);
		}
		else
		{
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Prtcl::TimeOut KillTimer Failed,Timers Count = %d!!!!!", nRet );
			pDrv->WriteLog(pchMsg);
		}

		//�ӻ���������Ҫ���μ��������90��
		if( pPrtcl->m_bSendPacketIsFromPeer )
		{
			CFepIEC103Device* pDevice =dynamic_cast<CFepIEC103Device*>( pDrv->GetDeviceByAddr( pPrtcl->m_byLastDevAddr ) );
			if (pDevice)
			{
				pDevice->m_byStandyOffCounter++;
			}			
			if ( pDevice && ( pDevice->m_byStandyOffCounter >= 3) && !pDevice->IsSimulate())
			{
/*	//�ӻ������߰���·����ʱ����� by hxw in 2014.1.17
				if ( pDevice->IsOnLine() )
				{
					snprintf( pchMsg, sizeof(pchMsg), 
						"CFepIEC103Prtcl::TimeOut: Standby device(Name=%s, Addr=%d) is offline!!!!!", pDevice->GetName().c_str(), pDevice->GetAddr() );
					pDrv->WriteLogAnyWay(pchMsg);
				}

				pDevice->SetOnLine( DEVICE_OFFLINE );
*/
			}
		}

		pPrtcl->m_nTimeOutCounter++;

		OpcUa_Int32 iReturnLen = 0;
		pPrtcl->BuildRequest( pPrtcl->m_pbySndBuffer, iReturnLen );

		//! ������ʱ��		
		if ( pPrtcl->m_pTimeOutTimer->SetTimer( pPrtcl->m_nRcvTimeOut ) == 0 )
		{
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Prtcl::TimeOut SetTimer Success,TimerID = %d....", pPrtcl->m_pTimeOutTimer->GetTimerID() );
			pDrv->WriteLog(pchMsg);
		}
		else
		{
			snprintf( pchMsg, sizeof(pchMsg), 
				"CFepIEC103Prtcl::TimeOut SetTimer Failed,TimerID = %d!!!!!", pPrtcl->m_pTimeOutTimer->GetTimerID() );
			pDrv->WriteLog(pchMsg);
		}
	}


	//! ���ӳɹ�
	OpcUa_Int32 CFepIEC103Prtcl::OnConnSuccess(std::string pPeerIP )
	{
		OpcUa_CharA chMsg[255];
		CFepIEC103Drv* pDriver = (CFepIEC103Drv*)m_pParam;	
		pDriver->GetConnTimer()->KillTimer();
		snprintf(chMsg,sizeof(chMsg), "%s ���ӳɹ�!" ,pPeerIP.c_str());

		pDriver->WriteLogAnyWay(chMsg);		

		pDriver->m_bIsConnecting = false;

		//���ӳɺ�ȥ���豸����״̬�������շ������������������ɹ����ٴ�������ӡ����˴��������豸���ߡ�
		CFepIEC103Device* pDevice = NULL;
		for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC103Device*>(pDriver->GetDeviceByIndex(i));
			if ( NULL != pDevice )
			{
				pDevice->SetSuspicious( false );
				pDevice->m_byTxTimes = 0;
				pDevice->m_byRxTimes = 0;
			}
		}

		OpcUa_Int32 iReturnLen = 0;
		BuildRequest( m_pbySndBuffer, iReturnLen );

		//! ������ʱ��
		if ( m_pTimeOutTimer->SetTimer( m_nRcvTimeOut ) == 0 )
		{
			snprintf( chMsg, sizeof(chMsg), 
				"CFepIEC103Prtcl::OnConnSuccess SetTimer Success,TimerID = %d....", m_pTimeOutTimer->GetTimerID() );
			pDriver->WriteLog(chMsg);
		}
		else
		{
			snprintf( chMsg, sizeof(chMsg), 
				"CFepIEC103Prtcl::OnConnSuccess SetTimer Failed,TimerID = %d!!!!!", m_pTimeOutTimer->GetTimerID() );
			pDriver->WriteLog(chMsg);
		}

		return 0;
	}

	void CFepIEC103Prtcl::InitConfigPar( OpcUa_CharA* strAppName )
	{
		if ( strAppName == NULL )
		{
			return;
		}

		//! ��ȡ�����ļ�����·��
		std::string file_name = CIODriver::GetFEPRunDir() + MACS_DRIVER_FILENAME;

		m_byPrtclType = GetPrivateProfileInt(strAppName, "PrtclType",0, file_name.c_str() );
		if ( m_byPrtclType < 0 || m_byPrtclType > 2 )
		{
			m_byPrtclType = 0;
		}

		//!��������·�Ǵ���
		if ( 0 != m_byPrtclType )
		{
			//�������ӵĳ�ʱ������
			m_iConnTimeOut = GetPrivateProfileInt( strAppName, "ConnTimeOut", 2000,  file_name.c_str() );
			if ( m_iConnTimeOut <= 0 )
			{
				m_iConnTimeOut = 30000;
			}

			//ͬһIP�����������	 
			m_iMaxReConn = GetPrivateProfileInt( strAppName, "MaxReConn", 3,  file_name.c_str() );
			if ( m_iMaxReConn <= 0 )
			{
				m_iMaxReConn = 3;
			}

		}	


		m_nRcvTimeOut = GetPrivateProfileInt(strAppName, "RcvTimeOut", -1, file_name.c_str() );
		if ( m_nRcvTimeOut == -1 ) //û�����ã�ʹ���ϰ汾�Ķ���
		{
			m_nRcvTimeOut = GetPrivateProfileInt("FepIEC103", "RcvTimeOut", 500, file_name.c_str() );
		}
		if ( m_nRcvTimeOut < 100 )
		{
			m_nRcvTimeOut = 500;
		}

		m_nCheckTimePeriod = GetPrivateProfileInt(strAppName, "CheckTimePeriod", -1, file_name.c_str());
		if ( m_nCheckTimePeriod == -1 )
		{
			m_nCheckTimePeriod = GetPrivateProfileInt("FepIEC103", "CheckTimePeriod", 300, file_name.c_str());
		}
		if ( m_nCheckTimePeriod < 5 )
		{
			m_nCheckTimePeriod = 300;
		}

		m_nMaxRetryPeriod = GetPrivateProfileInt(strAppName, "RetryPeriod", 1, file_name.c_str());
		if ( m_nMaxRetryPeriod < 0 )
		{
			m_nMaxRetryPeriod = 0;
		}
		m_nMaxRetryPeriod = m_nMaxRetryPeriod*60;	//����Ϊ����


/*
		m_nWholeCallPeriodASDU7 = GetPrivateProfileInt(strAppName, "GeneralInquire", -1, file_name.c_str()); 
		if ( m_nWholeCallPeriodASDU7 == -1 )
		{
			m_nWholeCallPeriodASDU7 = GetPrivateProfileInt("FepIEC103", "GeneralInquire", 60, file_name.c_str()); 
		}
		if ( m_nWholeCallPeriodASDU7 != 0 ) //0��ʾ���������ٻ�
		{
			if ( m_nWholeCallPeriodASDU7 < 30 )
			{
				m_nWholeCallPeriodASDU7 = 60;
			}
		}
*/

/*
		m_nWholeCallPeriodASDU21 = GetPrivateProfileInt(strAppName, "ASDU21GIPeriod", -1, file_name.c_str()); 
		if ( m_nWholeCallPeriodASDU21 == -1 )
		{
			m_nWholeCallPeriodASDU21 = GetPrivateProfileInt("FepIEC103", "ASDU21GIPeriod", 60, file_name.c_str()); 
		}
		if ( m_nWholeCallPeriodASDU21 != 0 ) //0��ʾ���������ٻ�
		{
			if ( m_nWholeCallPeriodASDU21 < 30 )
			{
				m_nWholeCallPeriodASDU21 = 60;
			}
		}

		if ( m_nWholeCallPeriodASDU7 != 0 && m_nWholeCallPeriodASDU21 != 0 ) //���ͬʱҪ�����ASDU7��ASDU21���������ٻ����������������֮��ļ��
		{
			if ( fabs(double(m_nWholeCallPeriodASDU7-m_nWholeCallPeriodASDU21)) < 30 )
			{
				if ( m_nWholeCallPeriodASDU7 < m_nWholeCallPeriodASDU21 + 30)
				{
					m_nWholeCallPeriodASDU7 = m_nWholeCallPeriodASDU21 + 60;
				}
				else if ( m_nWholeCallPeriodASDU21 < m_nWholeCallPeriodASDU7 + 30)
				{
					m_nWholeCallPeriodASDU21 = m_nWholeCallPeriodASDU7 + 60;
				}
			}
		}
*/

		OpcUa_Int32 bTemp = GetPrivateProfileInt(strAppName, "Wave", -1, file_name.c_str()); 
		if ( bTemp == -1 )
		{
			bTemp = GetPrivateProfileInt("FepIEC103", "Wave", 0,file_name.c_str()); 
		}
		if ( bTemp != 0 && bTemp != 1 )
		{
			bTemp = 0;
		}
		m_bEnableFaultWave = (bTemp)?true:false;
		if ( m_bEnableFaultWave )
		{
			m_nWholeCallPeriodASDU7 = max( FAULT_WAVE_TIMEOUT+2000, m_nWholeCallPeriodASDU7 );	//���ٻ������ڲ���С�ڻ�̫�ӽ��ڹ���¼���ĳ�ʱ��
		}

		if ( m_bEnableFaultWave )
		{
			CFepIEC103Device* pDevice = NULL;
			CFepIEC103Drv* pDrv = (CFepIEC103Drv*)GetParam();
			for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
				if (NULL != pDevice)
				{
					if ( pDevice->m_iASDU7ReadGroupPeriod )
					{
						pDevice->m_iASDU7ReadGroupPeriod = max( FAULT_WAVE_TIMEOUT+2000, pDevice->m_iASDU7ReadGroupPeriod );	//���ٻ������ڲ���С�ڻ�̫�ӽ��ڹ���¼���ĳ�ʱ��
					}

					if ( pDevice->m_iASDU21ReadGroupPeriod )
					{
						pDevice->m_iASDU21ReadGroupPeriod = max( FAULT_WAVE_TIMEOUT+2000, pDevice->m_iASDU21ReadGroupPeriod );	//���ٻ������ڲ���С�ڻ�̫�ӽ��ڹ���¼���ĳ�ʱ��
					}
				}
			}
		}


		m_nMaxOfflineCount = GetPrivateProfileInt(strAppName, "MaxOfflineCount", -1,file_name.c_str());
		if ( m_nMaxOfflineCount == -1 )
		{
			m_nMaxOfflineCount = GetPrivateProfileInt("FepIEC103", "MaxOfflineCount", 10,file_name.c_str());
		}
		if ( m_nMaxOfflineCount < 3 )
		{
			m_nMaxOfflineCount = 10;
		}

		m_nDORelayTime = 1000*GetPrivateProfileInt(strAppName, "DORelayTime", -1, file_name.c_str()); 
		if ( m_nDORelayTime == -1000 )
		{
			m_nDORelayTime = 1000*GetPrivateProfileInt("FepIEC103", "DORelayTime", 30, file_name.c_str()); 
		}
		if ( m_nDORelayTime < 0 )
		{
			m_nDORelayTime = 30000;
		}
		m_nPeerTimeOut = 2*m_nRcvTimeOut + 100;

		m_nSendDelay = GetPrivateProfileInt(strAppName, "SendDelay", -1, file_name.c_str()); 
		if ( m_nSendDelay == -1 )
		{
			m_nSendDelay = GetPrivateProfileInt("FepIEC103", "SendDelay", 20, file_name.c_str()); 
		}
		if ( m_nSendDelay < 10 )
		{
			m_nSendDelay = 20;
		}

		OpcUa_Int32 bBroadCast = GetPrivateProfileInt(strAppName, "BroadCast", -1, file_name.c_str()); 
		if ( bBroadCast == -1 )
		{
			bBroadCast = GetPrivateProfileInt("FepIEC103", "BroadCast", 1, file_name.c_str()); 
		}
		if ( bBroadCast == 1 )
		{
			m_bBroadCheckTime = true;
		}
		else
			m_bBroadCheckTime = false;

		//¼��ʱ��¼��־�����ĵĳ���ʱ�䡣Ĭ��300�롣
		OpcUa_Int32 nTemp = GetPrivateProfileInt("FepIEC103", "WaveLogTime", 300,file_name.c_str()); 
		if ( nTemp < 0 )
		{
			nTemp = 0;
		}
		m_msFaultLogLifeCycle = nTemp*1000;

	}


	/*!
	*	\brief	�����豸��ַ��ȡ�豸������
	*
	*	��ϸ�ĺ���˵������ѡ��
	*
	*	\param	����	OpcUa_Int32 Addr: �豸��ַ
	*	\param	����	OpcUa_Int32& DevNo: �豸��ַ��Ӧ���豸������		
	*
	*	\retval	����ֵ	bool �豸��ַ���ڷ���true�����򷵻�false
	*
	*	\note		
	*/
	bool CFepIEC103Prtcl::GetDeviceNoByAddr( OpcUa_Int32 Addr, OpcUa_Int32& DevNo )
	{
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)m_pParam;
		for( OpcUa_Int32 i=0; i< pDrv->GetDeviceCount(); i++ )
		{
			CFepIEC103Device* pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
			if( pDevice && pDevice->GetAddr() == Addr )
			{
				DevNo = i;
				return true;
			}
		}

		return false;
	}


	//�����豸���ٻ�
	void CFepIEC103Prtcl::ActiveWholeCall( CFepIEC103Device* pDevice )

	{
		if ( !pDevice )
		{
			return;
		}

		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)m_pParam;

		//һ��ֻ����һ�����ٻ�
		if ( pDevice->m_iASDU7ReadGroupPeriod )
		{
			pDevice->m_bNeedASDU7GI = true;

			OpcUa_CharA pchMsg[128];
			snprintf( pchMsg, sizeof(pchMsg), 
				"����ASDU7 �豸%s�ܲ�ѯ��", pDevice->GetName().c_str() );
			pDrv->WriteLog(pchMsg);
		}
		else if ( pDevice->m_iASDU21ReadGroupPeriod )
		{
			pDevice->m_bNeedASDU21GI = true;

			OpcUa_CharA pchMsg[128];
			snprintf( pchMsg, sizeof(pchMsg), 
				"����ASDU21 �豸%s���ѯ��", pDevice->GetName().c_str() );
			pDrv->WriteLog(pchMsg);
		}
		else
		{
			OpcUa_CharA pchMsg[128];
			snprintf( pchMsg, sizeof(pchMsg), 
				"���輤����豸%s��ASDU7�ܲ�ѯ����ASDU21���ѯ��", pDevice->GetName().c_str() );
			pDrv->WriteLog(pchMsg);
		}
	}


	//! ��ָ���豸��ͨ���������֡
	void CFepIEC103Prtcl::MakeChannelDiagFrame( CIODeviceBase* pDevice, OpcUa_Byte* pbyData, OpcUa_Int32& nReturnLen )
	{
		CFepIEC103Device* pIEC103Device = NULL;
		pIEC103Device = dynamic_cast<CFepIEC103Device*>(pDevice);
		if (!pIEC103Device)
		{
			return;
		}

		BuildFrame( pbyData, nReturnLen, pIEC103Device, false );
		
		return;
	}

	/*!
	*	\brief	������֡������
	*	���ӿ����������ڷ�������ǰ����
	*	ȱʡʵ�����ж�ͨ���Ƿ񱻽�ֹ�ˣ��������ֹ�򷵻�-2��
	*
	*	\param	pbyData: Ҫ���͵�����
	*	\param	nDataLen: Ҫ���͵����ݵĳ���
	*
	*	\retval��0Ϊ�ɹ�������ʧ��
	*
	*	\note	
	*/
	OpcUa_Int32 CFepIEC103Prtcl::BuildRequest( OpcUa_Byte* pBuf, OpcUa_Int32& iReturnLen )
	{
		OpcUa_Int32 nRet = 0;
		//! ��֡
		nRet = BuildRequestEx(pBuf, iReturnLen);
		
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*) m_pParam;
		//! ����
		if( iReturnLen > 0 )
		{
			//! д����
			pDrv->WriteCommData( m_pbySndBuffer, iReturnLen, COMMDATA_SND );
			try
			{
				if ( pDrv->m_pIODrvBoard->Write( m_pbySndBuffer, iReturnLen) == 0 )
				{
					pDrv->WriteLog("CFepIEC103Prtcl::BuildRequest Write Success....");
				}
				else
				{
					pDrv->WriteLog("CFepIEC103Prtcl::BuildRequest Write Failed!!!!!");
				}
			}
			catch ( ... )
			{
				pDrv->WriteLog("CFepIEC103Prtcl::BuildRequest Write Exception!!!!!");
			}
		}

		if ( m_bIsBroadingCheckTime )
		{
			pDrv->WriteLog("CFepIEC103Prtcl::BuildRequest is Sending Broad CheckTime Frame....");
			iReturnLen = 0;
			m_bIsBroadingCheckTime = false;

			//! ����200ms
			ACE_OS::sleep(ACE_Time_Value(0,200*1000));

			//! ֱ�ӷ�����һ֡
			nRet = BuildRequest( pBuf, iReturnLen );
		}

		return nRet;
	}

	/*!
	*	\brief	������֡
	*	���ӿ����������ڷ�������ǰ����
	*	ȱʡʵ�����ж�ͨ���Ƿ񱻽�ֹ�ˣ��������ֹ�򷵻�-2��
	*
	*	\param	pbyData: Ҫ���͵�����
	*	\param	nDataLen: Ҫ���͵����ݵĳ���
	*
	*	\retval��0Ϊ�ɹ�������ʧ��
	*
	*	\note	
	*/
	OpcUa_Int32 CFepIEC103Prtcl::BuildRequestEx( OpcUa_Byte* pBuf, OpcUa_Int32& iReturnLen )
	{
		OpcUa_Int32 iRet = 0;
		iReturnLen	= 0;		
		OpcUa_CharA chMsg[255];

		//ң�ء�ң�������������ȷ���
		iRet = BuildCtlRequest( pBuf,iReturnLen );
		if ( iReturnLen >0 && iRet == 0 )
		{
			OpcUa_Int32 i;
			//���߼���Ԫ���л�ΪDO��AO�����豸���Ա��´ξ�����ѯ�����豸��
			if ( GetDeviceNoByAddr( pBuf[5], i ) )		/////////////////////////////////��Ҫ�޸�index	
			{
				m_iDevIndex = i;
			}
			return iRet;
		}
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*) m_pParam;
		CFepIEC103Device* pDevice = NULL;

		pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(m_iDevIndex));//�õ���ǰ�豸ָ��;
		if (NULL == pDevice)
		{
			return iRet;
		}

		//! �ж��豸�Ƿ�ģ���ˣ������ģ���ˣ��򲻷���������
		if (pDevice && pDevice->IsSimulate() )
		{
			//! д��־
			sprintf( chMsg,"CFepIEC103Prtcl::BuildRequest Device Is Simulated!" );
			pDrv->WriteLog( chMsg );
			return -2;
		}

		if( pDrv->IsPortAccessClash() )	//���۴��ڻ������磬�������ǳ�ͻģʽ
		{
			//! ������·���
			OpcUa_Int32 nRest = 0;
			OpcUa_Int32 nLinkDataLen = 0;
			nRest = DealLinkClash( pDevice, pBuf, nLinkDataLen );
			if ( nRest==0 )	//! �����·��ϰ�,��������
			{
				//! ��ͨ����ϱ�־Ϊtrue
				m_bRTUChannelTest = true;				

				snprintf(chMsg,sizeof(chMsg),"CFepIEC103Prtcl::BuildRequest ����·��ϰ������͸����!" );
				pDrv->WriteLog(chMsg);

				nRest = 0;	

				return 0;
			}

			//! ������յ������������ϰ�,��ֱ�ӷ���
			if ( m_bSendPacketIsFromPeer && m_cmdPacket.nLen>0 )
			{				
				memcpy( pBuf, m_cmdPacket.byData, m_cmdPacket.nLen );
				//�ָ�
				iReturnLen = m_cmdPacket.nLen;			

				snprintf(chMsg,sizeof(chMsg),"CFepIEC103Prtcl::BuildRequest ������·��ϰ�!" );
				pDrv->WriteLog(chMsg);

				if (5 == m_cmdPacket.nLen)
				{
					//! ȡ�÷���֡���豸
					pDevice = dynamic_cast<CFepIEC103Device*>( 
						pDrv->GetDeviceByAddr( pBuf[2] ) );
				}
				else
				{
					//! ȡ�÷���֡���豸
					pDevice = dynamic_cast<CFepIEC103Device*>( 
						pDrv->GetDeviceByAddr( pBuf[5] ) );
				}				

				if ( !pDevice )
				{
					m_bSendPacketIsFromPeer = false;
					m_cmdPacket.nLen = 0;			
				}			
				m_bSendPacketIsFromPeer = false;
				m_cmdPacket.nLen = 0;
				if (pDevice)
				{
					m_byLastDevAddr = pDevice->GetAddr();
				}

				m_bRTUChannelTest = true;			//! ��ͨ����ϱ�־Ϊtrue


				m_iInnerLen = 0;	

				return iRet;
			}
		}


		m_iDevLastIndex = m_iDevIndex; //��ס��һ�ε��豸
		if ( ( pDrv->IsPortAccessClash() && pDevice->IsHot() ) 
			|| !pDrv->IsPortAccessClash()  )
		{
			do 
			{
				pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(m_iDevIndex));//�õ���ǰ�豸ָ��;

				BuildFrame(pBuf, iReturnLen, pDevice);//ÿ����һ�α�������m_iDevIndex��ָ����һ���豸

			    //��֡�ɹ������߱��������豸һ�κ������Ƿ���֡�ɹ����˳�ѭ��	
				if ( iReturnLen > 0  
					|| pDrv->GetDeviceCount() == 1 
					|| m_iDevLastIndex == m_iDevIndex )
				{
					break;
				}
			}while(true);
		}


		//�ж��Ƿ�����
		if( iReturnLen > 0 )	
		{
			pDevice->m_byTxTimes ++;
			if (pDevice->m_byTxTimes > m_nMaxOfflineCount  && !pDevice->IsSimulate() )
			{
				if ( pDevice->IsOnLine() )
				{
					OpcUa_CharA pchMsg[128];
					snprintf( pchMsg, sizeof(pchMsg), 
						"%d�ζ�û���յ��κα��ģ��豸%s(��ַ%d)���ɣ�", m_nMaxOfflineCount, pDevice->GetName().c_str(), pDevice->GetAddr() );

					pDrv->WriteLogAnyWay(pchMsg);
				}

				pDevice->SetSuspicious( true );
			}

/*	//����ͳһ��PeriodProc���ж�
			if (pDevice->m_byTxTimes > m_nMaxOfflineCount  && !pDevice->IsSimulate() )
			{
				if ( pDevice->IsOnLine() )
				{
					OpcUa_CharA pchMsg[128];
					snprintf( pchMsg, sizeof(pchMsg), 
						"����%d�ζ�û���յ��κα��ģ��豸%s(��ַ%d)�����ˣ�", m_nMaxOfflineCount, pDevice->GetName().c_str(), pDevice->GetAddr() );

					pDrv->WriteLogAnyWay(pchMsg);
				}

				pDevice->SetOnLine( DEVICE_OFFLINE );
				pDevice->m_byTxTimes = 0;
				pDevice->m_byRxTimes = 0;
			}
*/
		}

		return 0;
	}


	/*!
	*	\brief	���������֡
	*	
	*	\param	����	OpcUa_Byte* pBuf: ���ص����ݰ�
	*	\param	����	OpcUa_Int32& iReturnLen: �������ݰ��ĳ���	 
	*
	*	\retval	����ֵ	��
	*
	*	\note		
	*/
	void CFepIEC103Prtcl::BuildFrame ( OpcUa_Byte* pBuf, OpcUa_Int32& iReturnLen, CFepIEC103Device* pDevice, bool IsLocal )
	{
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)m_pParam;		
		ACE_OS::memset(pBuf,0,255);
		OpcUa_CharA chMsg[255];

		//������������������Ϊ��������֡����������Ҳû��ϵ������¼������ʧʱ�ᵼ��¼����ȡ������ֹ��
		if ( !IsLocal )
		{
			//�����������֡
			snprintf(chMsg,sizeof(chMsg),"���������...�豸(��ַ%d)�����������֡.", pDevice->GetAddr());
			pDrv->WriteLog(chMsg);
			iReturnLen = pDevice->BuildLevel2DataFrame( pBuf );
			MakeLog(pBuf, iReturnLen);		 

			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
		}

		//���������豸�������·�����ڲ����豸����,�������ߵ��豸��Ҫ���ô����ڵ���ѯ���ƣ�����豸����ȵ������ڵ�ʱ���ܷ������֡
		if ( pDevice->IsOffLine() && !IsLinkOffline() && ( m_nMaxRetryPeriod > 0 && !pDevice->m_bRetryPeriod ) )
		{
			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}

			return;
		}

		pDevice->m_bRetryPeriod = false;

		if ( !pDevice->IsOnLine() )
		{			
			snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)��ʼ��(ԭ��:����).", pDevice->GetAddr());
			pDrv->WriteLog(chMsg);

			pDevice->m_bNew = true;
			pDevice->m_bFirstCT = true;
			iReturnLen = pDevice->BuildResetCUFrame(pBuf);
			MakeLog(pBuf, iReturnLen);
			 
			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}

		if (pDevice->m_bNeedInit)			//��ʼ��
		{
			snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)��ʼ��.", pDevice->GetAddr());
			pDrv->WriteLog(chMsg);
			
			iReturnLen = pDevice->BuildResetCUFrame( pBuf );

			//	pDevice->m_bNeedInit = false;
			MakeLog(pBuf, iReturnLen);			

			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}

		//��X��ͨ���������������ֹ
		if (pDevice->m_bChannelReady)		//ͨ������׼������
		{
			snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)ͨ������׼������.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);	

			pDevice->m_bChannelReady = false;
			iReturnLen = pDevice->BuildASDU24Frame( pBuf, T_Chanel_Req );

			MakeLog(pBuf, iReturnLen);			
			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}


		//ASDU25 �϶����߷��Ͽ�
		if (pDevice->m_bSendDisDataAck)			//���Ŷ������Ͽɡ���־
		{			
			snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)�Ŷ������Ͽɱ�־.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);
			
			pDevice->m_bSendDisDataAck = false;
			iReturnLen = pDevice->BuildDisDataAckFrame( pBuf, pDevice->m_byDisDataAckTOO );

			MakeLog(pBuf, iReturnLen);		

			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;

		}

		//�������־��״̬��λ����
		if (pDevice->m_bReqStatusAlter)			//����־��״̬��λ����׼������
		{			
			snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)����־��״̬��λ����׼������.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);			

			pDevice->m_bReqStatusAlter = false;
			iReturnLen = pDevice->BuildASDU24Frame( pBuf, T_StatusChange_Req );

			MakeLog(pBuf, iReturnLen);			
			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;		//�����ǲ���ʹ��
		}

		//�Ŷ���������
		if (pDevice->m_bReqDisData)		//�Ŷ����ݴ���׼������
		{			
			snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)�Ŷ����ݴ���׼������.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);
			
			pDevice->m_bReqDisData = false;
			iReturnLen = pDevice->BuildASDU24Frame( pBuf, T_Data_Req );

			MakeLog(pBuf, iReturnLen);
			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;		//�����ǲ���ʹ��
		}

		//����ѡ��
		if (pDevice->m_bFaultSel)		
		{			
			snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)����ѡ��.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);

			//����ѡ��Ĵ�����Ҫ��һ��������ȷ��
			//EnterCriticalSection( &pDevice->m_FaultWaveSection );
			pDevice->m_wFAN = pDevice->m_FaultDataVec[0].FAN;
			//LeaveCriticalSection( &pDevice->m_FaultWaveSection );

			pDevice->m_bFaultSel = false;
			iReturnLen = pDevice->BuildASDU24Frame( pBuf, T_FaultSelect );		//�����ǲ���ʹ��
			MakeLog(pBuf, iReturnLen);
			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}

		//��λFCB֡
		if (pDevice->m_bSendRestCU)
		{			
			if (  ++pDevice->m_nSendResetCUCounter <= 3 )
			{
				iReturnLen = pDevice->BuildResetFCBFrame( pBuf );				
				snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)��λFCB֡.", pDevice->GetAddr() );				
			}
			else
			{
				pDevice->m_nSendResetCUCounter = 0;
				iReturnLen = pDevice->BuildResetCUFrame( pBuf );				
				snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)���Ը�λFCB֡����3��,��λ�豸.", pDevice->GetAddr() );				
			}

			pDrv->WriteLog( chMsg );
			MakeLog(pBuf, iReturnLen);		

			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}


		//ע�⣺����103��׼����λ����ʼ�����豸����Ҫ�ȶ�ʱ���ٽ����ܲ�ѯ��by hxw��
		if ( ( m_bIsTimeForBroadCheckTime || pDevice->m_bNeedCheckTime ) //��ҪУʱ��
			&& !pDevice->m_bIsRcvingFaultWave && !pDevice->IsStatusUnit() )			//������ڽ��й���¼���������ݻ���
		{
			snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)%sУʱ֡.", pDevice->GetAddr(), (m_bBroadCheckTime)?"�㲥":"��Ե�" );
			pDrv->WriteLog(chMsg);

			iReturnLen = pDevice->BuildCheckTimeFrame( pBuf, m_bBroadCheckTime );
			m_bIsTimeForBroadCheckTime = false;
			pDevice->m_bNeedCheckTime = false;

			m_bIsBroadingCheckTime = true;
			MakeLog(pBuf, iReturnLen);
			
			return;
		}

		//����һ������֡
		if (pDevice->m_bReq1LevelData)
		{			
			snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)����һ������֡.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);

			iReturnLen = pDevice->BuildLevel1DataFrame( pBuf );
			MakeLog(pBuf, iReturnLen);

			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}


		//�ܲ�ѯASDU7
		if (pDevice->m_bNeedASDU7GI && !pDevice->m_bIsRcvingFaultWave)				
		{			
			snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)�ܲ�ѯ֡ASDU7.", pDevice->GetAddr() );
			pDrv->WriteLog(chMsg);

			iReturnLen = pDevice->BuildGIFrame( pBuf );
			pDevice->m_bNeedASDU7GI = false;
			pDevice->m_IsSOE = false;
			MakeLog(pBuf, iReturnLen);			

			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}

		//�ܲ�ѯASDU21
		if ( pDevice->m_bNeedASDU21GI && !pDevice->m_bIsRcvingFaultWave)				
		{	
			if ( pDevice->m_vGroupNo.size() > 0 )	//ASDU21: �����ٻ�
			{
				
/*	//ASDU21�ٻ����ڵ�ʱ��ǿ����ֹ�������ݴ�����̣�������ʼASDU21����ASDU21��ȡ��ɺ󣬿���ʱ�ٸ��¶������ݡ� hxw 2013.4.14
				if ( pDevice->m_bConfirmed )	//
				{
					//�����������֡
					snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)�����������֡.", pDevice->GetAddr());
					pDrv->WriteLog(chMsg);
					iReturnLen = pDevice->BuildLevel2DataFrame( pBuf );
					MakeLog(pBuf, iReturnLen);		 
				}
				else
*/
				{
					iReturnLen = pDevice->BuildASDU21GroupFrame( pBuf, pDevice->m_vGroupNo[pDevice->m_iGroupIndex]*1000 + 0 );					
					if ( iReturnLen > 0 )
					{
						snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)���=%d�ܲ�ѯ֡ASDU21.", pDevice->GetAddr(), pDevice->m_vGroupNo[pDevice->m_iGroupIndex] );
						pDrv->WriteLog(chMsg);
					}

					pDevice->m_iGroupIndex++;
					if ( pDevice->m_iGroupIndex >= pDevice->m_vGroupNo.size() )
					{
						pDevice->m_iGroupIndex = 0;
						pDevice->m_bNeedASDU21GI = false;
						pDrv->WriteLog("���һ��ASDU21�ٻ���������");
					}
				}
			}
			else	//ASDU21: ���ٻ�������
			{
				snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)�ܲ�ѯ֡ASDU21.", pDevice->GetAddr() );
				pDrv->WriteLog(chMsg);

				iReturnLen = pDevice->BuildASDU21WholeCallFrame( pBuf );
				pDevice->m_bNeedASDU21GI = false;
				pDevice->m_IsSOE = false;
				MakeLog(pBuf, iReturnLen);			
			}

			//�л��߼��豸��
			m_iDevIndex ++;
			if ( m_iDevIndex >= pDrv->GetDeviceCount() )
			{
				m_iDevIndex = 0;
			}
			return;
		}

		//�����������֡
		snprintf(chMsg,sizeof(chMsg),"����...�豸(��ַ%d)�����������֡.", pDevice->GetAddr());
		pDrv->WriteLog(chMsg);
		iReturnLen = pDevice->BuildLevel2DataFrame( pBuf );
		MakeLog(pBuf, iReturnLen);		 

		//�л��߼��豸��
		m_iDevIndex ++;
		if ( m_iDevIndex >= pDrv->GetDeviceCount() )
		{
			m_iDevIndex = 0;
		}

		return;
	}


	OpcUa_Int32 CFepIEC103Prtcl::RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nBufLen )
	{

		//! ͨ���������ֹ�ˣ���ֱ�ӷ���0
		CFepIEC103Drv* pDriver = (CFepIEC103Drv*)m_pParam;
		if ( !pDriver || !m_pbyInnerBuf || !m_pbyRevBuffer || !m_sPacketBuffer )
			return -1;		
// 		if(  pDriver->GetChannelObj()->IsDisable() )
// 		{
// 			return 0;
// 		}

		if ( !pDriver->CanWork() )
		{
			return 0;
		}

		///////////////////////////////////////////////////////////////////////////////////
		//! �����Ҫͨ�ż��ӣ�����ԭʼ���ݰ�
		pDriver->WriteCommData( pDataBuf, nBufLen );

		//�����ݷŵ��ڲ�������
		InnerAddData( pDataBuf, nBufLen );

		//���ڲ���������ִ�л������ڣ������ݰ���ע��һ�ο��ܻᴦ�������ݰ�
		OpcUa_Int32	nReceived = 0;
		do
		{
			//��ȡ�����ݳ���
			nReceived = PRTCL_RCV_BUFFER_LENGTH - m_nRevLength;
			if ( 0 >= nReceived || nReceived>PRTCL_RCV_BUFFER_LENGTH )
			{
				m_nRevLength = 0;
				break;
			}
			//���ڲ�������ȡ������
			if( InnerRead( m_pbyRevBuffer+m_nRevLength, nReceived, nReceived) && nReceived > 0 )
			{
				m_nRevLength += nReceived;
				while( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH )
				{
					bool bCompleted = false;
					OpcUa_Int32 nPacketLen = 0;
					memset( m_sPacketBuffer, 0, PRTCL_MAX_PACKET_LENGTH );

					//ȡ֡���������һ֡��Ϊ�����õ����ݣ�ǰ���֡��������
					bool bTemp;
					do{
						if ( m_nRevLength < PRTCL_MIN_PACKET_LENGTH)
						{
							break;
						}

						bTemp = ExtractPacket( m_sPacketBuffer, &nPacketLen );
						if( bTemp )
						{
							//ʣ�µ������ƶ������ջ������Ŀ�ͷ
							m_nRevLength = m_nRevLength - nPacketLen;
							if (m_nRevLength< 0)
							{	
								m_nRevLength = 0;
							}
							if ( m_nRevLength>PRTCL_RCV_BUFFER_LENGTH )
							{
								m_nRevLength = PRTCL_RCV_BUFFER_LENGTH;
							}

							if ( m_nRevLength > 0 )
							{
								ACE_OS::memcpy( m_pbyRevBuffer, m_pbyRevBuffer+nPacketLen, m_nRevLength );
							}

							bCompleted = true;
						}
					}while(bTemp);

					//�ҵ����ݰ������
					if ( bCompleted )
					{
						ProcessRcvdPacket( m_sPacketBuffer, nPacketLen, true );						
						m_nRevLength = 0;
						return 0;
					}
					else
					{
						break;
					}
				}
			}
		}while( 0<nReceived );


		//���ش�������
		return -1;
	}



	/*!
	*	\brief	 �������������ݰ�
	*	
	*	\param	����	OpcUa_Byte* pPacketBuffer: �����������ݰ�
	*	\param	����	SHORT* pnPacketLength: ���ݰ�����	
	*
	*	\retval	����ֵ	bool : �ɹ�true������false
	*
	*	\note		ע�������ѡ��
	*/
	bool CFepIEC103Prtcl::ExtractPacket(OpcUa_Byte* pPacketBuffer, OpcUa_Int32* pnPacketLength)
	{
		CFepIEC103Drv* pDriver = (CFepIEC103Drv*)(m_pParam);
		pDriver->WriteLog( "CFepIEC103Prtcl::ExtractPacket �յ�����֡....." );

		//У��
		if( !pPacketBuffer || !pnPacketLength || m_nRevLength <= 0 )
			return false;
		//
		bool bACompletePacketRcvd = false;
		OpcUa_Byte * pFrame;
		pFrame = m_pbyRevBuffer;

		//У��֡�ĺ�����

		OpcUa_Int32 nErrorLen = 0;	//�ѽ�������ȷ�ϴ�������ݴ�С.
		while( pFrame < m_pbyRevBuffer + m_nRevLength )
		{

			//1���ȳ��԰���E5/A2֡������. ����E5�������ԣ��������ǿ��Բ��Ͽɷ����ֽ�ΪE5/A2��ΪE5/A2��֡. ��Ϊ10��68�����кܿ��ܰ���E5/A2�ֽڡ�
			if ( (pFrame[0] == 0xE5 || pFrame[0] == 0xA2) 
				&& m_nRevLength == 1 )	//������ճ��ȴ���1,�򲻿���E5/A2��֡
			{
				//���ϸ��֡���õ����������
				memcpy(pPacketBuffer, pFrame, 1 );

				//�ñ���������ݵĳ���
				*pnPacketLength = 1;

				bACompletePacketRcvd = true;
				break;
			}
			else
			{
				//frame type
				OpcUa_Byte frameType = pFrame[0];
				if( frameType != GUDINGZHEN_START_ID && frameType != BIANCHANGZHEN_START_ID )
				{
					pFrame++;
					continue;
				}

				if( frameType == BIANCHANGZHEN_START_ID )
				{
					//len
					if (m_pbyRevBuffer + m_nRevLength - pFrame < 3 )		//�ڶ��͵���λ�ǳ���λ
					{
						return false;
					}
					if ( pFrame[1] != pFrame[2] )
					{
						pFrame++;
						continue;
					}
					if ( m_pbyRevBuffer + m_nRevLength - pFrame < pFrame[1] + 6 )	
					{
						return false;
					}
					//start code
					if ( 0x68 != pFrame[3])
					{
						pFrame++;
						continue;
					}

					//ctrl code
					//Add by hxw in 2007.04.04
/*
					CONTROL_FIELD* pCtrl = (CONTROL_FIELD* )&pFrame[4];
					if ( pCtrl->Prm == 1 )	//��վ���͵ı���,�����Ǵ�վ��Ӧ�ı���.
					{
						pFrame++;
						continue;
					}
*/

					//crc
					OpcUa_Byte bySumCode = Get8SumCode( &pFrame[4], pFrame[1] );
					if( bySumCode != pFrame[pFrame[1]+4] )
					{
						pFrame++;
						continue;
					}

					//end code
					if (  0x16 != pFrame[pFrame[1]+5] )
					{
						pFrame++;
						continue;	
					}
					else
					{
						//���ϸ��֡���õ����������
						memcpy(pPacketBuffer, pFrame, pFrame[1]+6 );

						//�ñ���������ݵĳ���
						*pnPacketLength =  pFrame[1] + 6;

						bACompletePacketRcvd = true;
					}
				}
				else if( frameType == GUDINGZHEN_START_ID )
				{
					//len
					if( m_pbyRevBuffer + m_nRevLength - pFrame < 5 )
					{
						return false;
					}

					//ctrl code
					//Add by hxw in 2007.04.04
/*
                    CONTROL_FIELD* pCtrl = (CONTROL_FIELD* )&pFrame[1];
					if ( pCtrl->Prm == 1 )	//��վ���͵ı���,�����Ǵ�վ��Ӧ�ı���.
					{
						pFrame++;
						continue;
					}
*/

/*
					if ( pCtrl->FunCode != 0
						&& pCtrl->FunCode != 1 
						&& pCtrl->FunCode != 8 
						&& pCtrl->FunCode != 9 
						&& pCtrl->FunCode != 11 
						&& pCtrl->FunCode != 14 
						&& pCtrl->FunCode != 15 
						)	//�Ǵ�վ��Ӧ�ĺϷ�������
					{
						pFrame++;
						continue;
					}
*/
					//crc
					OpcUa_Byte bySumCode = Get8SumCode( &pFrame[1], 2 );
					if( bySumCode != pFrame[3] )
					{
						pFrame++;
						continue;
					}

					//end code
					if( 0x16 != pFrame[4] )
					{
						pFrame++;
						continue;
					}
					else
					{
						//���ϸ��֡���õ����������
						memcpy(pPacketBuffer, pFrame, 5 );

						//�ñ���������ݵĳ���
						*pnPacketLength =  5;

						bACompletePacketRcvd = true;
					}
				}			
				else
				{
					pFrame++;
					continue;
				}

			}

			if (bACompletePacketRcvd)
			{
				bool bIsMasterMsg = false;		//���ձ����Ƿ�������վ
				if (0x68 == pFrame[0] )			//�ɱ䳤֡
				{
					bIsMasterMsg = IsMasterMsg( pFrame+4);
				}
				else if (0x10 ==pFrame[0])		//�̶�֡
				{
					bIsMasterMsg = IsMasterMsg( pFrame+1);
				}

				//! ����յ���������ȷ֡����
				std::string pchMsg;
				OpcUa_CharA chNow[4];
				for ( OpcUa_Int32 k = 0; k < *pnPacketLength; k++ )
				{
					sprintf( chNow, "%02X ", pFrame[k]);
					pchMsg += chNow;
				}
				//! д��־
				OpcUa_CharA pchLog[1024];
				sprintf( pchLog,"CFepIEC103Prtcl::ExtractPacket Received a complete %s frame:%s!!", bIsMasterMsg?"Master":"Slave", pchMsg.c_str() );
				pDriver->WriteLog( pchLog );

				//����������������վ����֡������֡�е��豸��ַ��Ȼ������֡������������
				if ( bIsMasterMsg )
				{
					if (0x68 == pFrame[0] )					//�ɱ䳤֡
					{
						m_byLastPeerDevAddr = pFrame[5];
					}
					else if (0x10 ==pFrame[0])		//�̶�֡
					{
						m_byLastPeerDevAddr = pFrame[2];
					}

					//��ȫ��������ȸ��Ǳ��ط���������һ���������µķ���ʱ���򱾲����Զ�ʧЧ��
					//���������Լ����������󣺿�ʶ���E5��Ϊ���豸1���͵�
					//�����շ������±��ģ�
					//2014-01-16 11:07:10:621121 SND: 10 40 02 42 16 
					//2014-01-16 11:07:10:673946 RCV: 10 5B 01 5C 16 E5 
					//�������ô������ô��������ΪE5���豸2���͵ģ����Ǵ���ģ�
					memcpy( m_pbySndBuffer, pFrame, *pnPacketLength );	

					m_nRevLength -= *pnPacketLength;
					if ( m_nRevLength > 0 )
					{				
						ACE_OS::memcpy(m_pbyRevBuffer, &m_pbyRevBuffer[*pnPacketLength], m_nRevLength);
					}
					else
						m_nRevLength = 0;

					//�����Ѿ������ˣ���ô������֡
					bACompletePacketRcvd = false;
					*pnPacketLength = 0;
					continue;
				}

				break;
			}
		}

		//ȥ������ȷ��������ݲ���
		nErrorLen = pFrame - m_pbyRevBuffer;
		if ( nErrorLen > 0 )
		{
			m_nRevLength -= nErrorLen;
			if ( m_nRevLength > 0 )
			{				
				ACE_OS::memcpy(m_pbyRevBuffer, &m_pbyRevBuffer[nErrorLen], m_nRevLength);
			}
			else
				m_nRevLength = 0;
		}

		return bACompletePacketRcvd;
	}



	/*!
	*	\brief	 �������������ݰ�
	*
	*	��ϸ�ĺ���˵������ѡ��
	*
	*	\param	����	OpcUa_Byte* pPacketBuffer: ���ݰ�
	*	\param	����	SHORT nPacketLength: ���ݰ�����
	*	\param	����	bool bFromLocal: �����Ǳ������Ļ����������
	*
	*	\retval	����ֵ	��
	*
	*	\note		ASDU1��ASDU2��ASDU41��ASDU43 SOEʱ�������豸
	*/
	void CFepIEC103Prtcl::ProcessRcvdPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32 nPacketLength, bool bFromLocal )
	{
		//У��
		if( !pPacketBuffer || nPacketLength == 0 )
			return;

		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)m_pParam;

		OpcUa_CharA chMsg[255];
		try
		{
			OpcUa_Int32 nRet = m_pTimeOutTimer->KillTimer();
			//�ض�ʱ��
			if ( nRet >= 0 )
			{
				snprintf( chMsg, sizeof(chMsg), 
					"CFepIEC103Prtcl::ProcessRcvdPacket KillTimer Success,Timers Count = %d....", nRet );
				pDrv->WriteLog(chMsg);
			}
			else
			{
				snprintf( chMsg, sizeof(chMsg), 
					"CFepIEC103Prtcl::ProcessRcvdPacket KillTimer Failed,Timers Count = %d!!!!!", nRet );
				pDrv->WriteLog(chMsg);
			}
		}
		catch ( ... )
		{
			pDrv->WriteLog("CFepIEC103Prtcl::ProcessRcvdPacket KillTimer Exception!");
		}

		OpcUa_Byte byTrue =1;
		OpcUa_Byte byFalse = 0;
		
		//��ַ��
		OpcUa_Byte byUnitAddr = 0;
		//������
		CONTROL_FIELD* pCtrl = NULL;

		//֡���ʹ���
		OpcUa_Int32 iFrameType = pPacketBuffer[0];

		bool bIsOnebyteResp = false;	//�Ƿ�һ���ֽڵ���Ӧ֡
		if (0x68 == iFrameType)					//�ɱ䳤֡
		{
			byUnitAddr = pPacketBuffer[5];
			pCtrl	= (CONTROL_FIELD* )&pPacketBuffer[4];

			//�յ��豸��10��68���ĺ�������ط��ͻ��棬�����������ʱ����ΪE5���豸2��Ӧ��
			//2014-01-22 17:13:34:379619 RCV: 10 40 02 42 16 
			//2014-01-22 17:13:34:888785 RCV: 68 0D 0D 68 53 01 15 81 2A 01 FE F1 00 01 01 00 01 07 16 E5 
			memset( m_pbySndBuffer, 0, PRTCL_SND_BUFFER_LENGTH );

			/*		//�˴�����ȥ��. ��Ϊ�ڽ��������Ѿ����˵�����վ����֡(Prm=1)
			if ( (pPacketBuffer[6] == TYP_TIME_SYN) || (pPacketBuffer[6] == TYP_GENERAL_QUERY_START) || (pPacketBuffer[6] == TYP_DISORDER_DATA_TRANS) ||
			(pPacketBuffer[6] == TYP_DISORDER_DATA_ACK) ||(pPacketBuffer[6] == TYP_GENERAL_CMD) )
			{
			return;
			}
			*/
		}
		else if (0x10 ==iFrameType)		//�̶�֡
		{
			byUnitAddr = pPacketBuffer[2];
			pCtrl	= (CONTROL_FIELD* )&pPacketBuffer[1];

			//�յ��豸��10��68���ĺ�������ط��ͻ��棬�����������ʱ����ΪE5���豸2��Ӧ��
			//2014-01-22 17:13:34:379619 RCV: 10 40 02 42 16 
			//2014-01-22 17:13:34:888785 RCV: 68 0D 0D 68 53 01 15 81 2A 01 FE F1 00 01 01 00 01 07 16 E5 
			memset( m_pbySndBuffer, 0, PRTCL_SND_BUFFER_LENGTH );
		}
		else if ( 0xE5 == iFrameType || 0xA2 == iFrameType )	//һ���ֽڵ���Ӧ֡�������豸��ַ���ϴη��ͱ��Ĵӻ�ȡ
		{
			bIsOnebyteResp = true;
			
			CFepIEC103Device* pDevAny =dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex( 0 ));

			//Add by hxw in 2014.2.21. ����ӻ��յ�E5��֡��һ�ɶ���������Ϊ�������ݣ�ԭ����ʵ��ͨ��ʱE5֡���ܴ��ڴ��Ľ��գ��Ӷ������豸����������
			if ( !pDevAny->IsHot() )
			{
				pDrv->WriteLog("CFepIEC103Prtcl::ProcessRcvdPacket: Received message from Standby link  is E5 or A2 , so abort!");
				return;
			}

			if ( pDevAny->IsHot() || m_bRTUChannelTest )
			{
				if (0x68 == m_pbySndBuffer[0])			//�ɱ䳤֡
				{
					byUnitAddr = m_pbySndBuffer[5];
				}
				else if (0x10 ==m_pbySndBuffer[0])		//�̶�֡
				{
					byUnitAddr = m_pbySndBuffer[2];
				}
			}
			else //�ӻ�����յ�һ���ֽڵĶ�֡�����ϴε������ֲ��Ǳ������͵ģ����豸��Ȼ���ϴδӽ��ձ����н����õ���������͵ı��ĵ�ַ��
			{
				byUnitAddr = m_byLastPeerDevAddr;
			}
		}
		
		CFepIEC103Device* pDevice =dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByAddr( byUnitAddr ));
		if ( !pDevice )
		{
			pDrv->WriteLog("CFepIEC103Prtcl::ProcessRcvdPacket: can't get device from received message , so abort!");
			return;
		}

		if (!pDevice->IsHot())
		{
			if ( !pDevice->IsOnLine() )
			{
				OpcUa_CharA pchMsg[128];
				snprintf( pchMsg, sizeof(pchMsg), 
					"�豸%s(��ַ%d)�����ˣ�", pDevice->GetName().c_str(), pDevice->GetAddr() );
				pDrv->WriteLogAnyWay(pchMsg);
			}

			//���豸ֻҪ�շ�һ���ֽڵĶ�֡���ģ���
			pDevice->SetSuspicious( false );
			pDevice->SetOnLine( DEVICE_ONLINE );
			pDevice->m_byTxTimes = 0;
			pDevice->m_byRxTimes=1;

			//! ����Ǵӻ����յ�����ͨ�����֡����Ӧ������ͨ����ϱ�־
			if(  m_bRTUChannelTest )
			{
				m_bRTUChannelTest = false;
				pDevice->SetSlaveTimeOuts( 0 );
			}

			/////////////////////////////////////////
			//! ������һ֡
			OpcUa_Int32 iReturnLen = 0;
			BuildRequest( m_pbySndBuffer, iReturnLen);

			//! ������ʱ��
			if ( m_pTimeOutTimer->SetTimer( m_nRcvTimeOut ) == 0 )
			{
				snprintf( chMsg, sizeof(chMsg), 
					"CFepIEC103Prtcl::ProcessRcvdPacket Slave  SetTimer Success,TimerID = %d....", m_pTimeOutTimer->GetTimerID() );
				pDrv->WriteLog(chMsg);
			}
			else
			{
				snprintf( chMsg, sizeof(chMsg), 
					"CFepIEC103Prtcl::ProcessRcvdPacket Slave  SetTimer Failed,TimerID = %d!!!!!", m_pTimeOutTimer->GetTimerID() );
				pDrv->WriteLog(chMsg);
			}

			return;
		}

		pDevice->ReverseFCB();

		//����װ������
		if( bFromLocal )
		{
			if ( !pDevice->IsOnLine() )
			{
				OpcUa_CharA pchMsg[128];
				snprintf( pchMsg, sizeof(pchMsg), 
					"�豸%s(��ַ%d)�����ˣ�", pDevice->GetName().c_str(), pDevice->GetAddr() );
				pDrv->WriteLogAnyWay(pchMsg);
			}

			pDevice->SetSuspicious( false );
			pDevice->SetOnLine( DEVICE_ONLINE );
			pDevice->m_byTxTimes = 0;
			pDevice->m_byRxTimes=1;
		}


		if ( bIsOnebyteResp )
		{
			//����յ�����һ�ֽڶ�֡�����´ε�һ�������ݲ���Ϊ����ת�����ϴ�����һ�����ݣ���ξ͸�����������ݣ��ϴ�����������ݣ���ξ͸�����һ������
			if ( pDevice->m_bReq1LevelData )
			{
				pDevice->m_bReq1LevelData = false;
				pDevice->m_bReq2LevelData = true;
				pDrv->WriteLog("�ϴ��յ�����һ�ֽڶ�֡Ӧ�𣬶��ϴ�����Ϊһ�����ݣ���ξ͸������������");
			}
			else
			{
				pDevice->m_bReq1LevelData = true;
				pDevice->m_bReq2LevelData = false;
				pDrv->WriteLog("�ϴ��յ�����һ�ֽڶ�֡Ӧ�𣬶��ϴ�����Ϊ�������ݣ���ξ͸�����һ������");
			}
		}
		else
		{//��һ�ֽڵ�����֡�����账������ֶ���ϢCONTROL_FIELD

			pDrv->WriteLog("��һ�ֽڵ�����֡�����账������ֶ���ϢCONTROL_FIELD");

			//��λ��λ��־��һ��/�������������־
			pDevice->m_bSendRestCU = false;	//Del by hxw.2007.10.16
			pDevice->m_bReq1LevelData = false;
			pDevice->m_bReq2LevelData = false;

			//��ÿ����ֶ�
			OpcUa_Byte tempACD = pCtrl->FCB_ACD;
			OpcUa_Byte tempDFC = pCtrl->FCV_DFC;
			OpcUa_Byte byFunc  = pCtrl->FunCode;

			pDevice->m_bConfirmed = false;	//�ȸ�λ

			//��鹦����
			switch (byFunc)
			{
			case FRAME_FUNC_CONFIRM:
				pDevice->m_bConfirmed = true;	
				pDevice->m_bNeedInit = false;
				if ( pDevice->m_bSendRestCU )
				{
					pDevice->m_bSendRestCU = false;
				}
				if (pDevice->m_bInDO)	//DO�����ȷ�ϣ�˵���豸��ȷִ��
				{
					pDevice->m_bInDO = false;
					pDevice->SetVal(MacsDIS103_DIC, pDevice->m_iInDOIndex, &byTrue,1,0, true);
					if (pDevice->TagNeedSOE(MacsDIS103_DO,pDevice->m_iInDOIndex)
						|| pDevice->TagNeedDevSOE(MacsDIS103_DIC,pDevice->m_iInDOIndex))
					{					
// 						CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 							pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)byTrue, 0);		//! ����SOE
					}
				}
				break;
			case FRAME_FUNC_DENY:
				if (pDevice->m_bInDO)	//DO�����ȷ�ϣ�˵���豸��ȷִ��
				{
					pDevice->m_bInDO = false;
					pDevice->SetVal(MacsDIS103_DIC, pDevice->m_iInDOIndex, &byFalse,1,0, true);
				}
				else
				{
					pDevice->m_bSendRestCU = true;	//Add by hxw 2007.10.16				
					snprintf(chMsg,sizeof(chMsg)
						,"�豸(��ַ%d)����������Ϊ1��ָʾ��·æ������RestCU��", pDevice->GetAddr());
					pDrv->WriteLog(chMsg);
				}
				break;
			case FRAME_FUNC_RET_DATA:
				pDevice->m_bConfirmed = true;	
				break;
			case FRAME_FUNC_NO_DATA:
				/*	//Del by hxw in 2008.05.04
				pDevice->m_bSendRestCU = true;	//Add by hxw 2007.10.16
				strLog.Format( "�豸(��ַ%d)����������Ϊ9��ָʾ������Ӧ�𣬳���RestCU��", pDevice->GetAddr() );
				*/
				break;
			case FRAME_FUNC_ACK_REQUEST:
				break;
			case 15:
				break;
			default:
				break;
			}


			//!�̵籣���豸ϣ�����û�����һ���û�����
			if (1 == tempACD)
			{			
				if (1 == tempDFC)
				{
					//!�̵籣���豸�Ļ���������
					pDevice->m_bSendRestCU = true;
					pDrv->WriteLog("�豸����Reset��");
				}
				else
				{
					//!�̵籣���豸���Խ���������
					pDevice->m_bReq1LevelData = true;
					pDevice->m_nSendResetCUCounter = 0;
				}			
			}
			else//!�̵籣���豸ϣ�����û����Ͷ����û�����
			{			
				if (1 == tempDFC)
				{
					pDevice->m_bSendRestCU = true;
					pDrv->WriteLog("�豸����Reset��");
				}
				else
				{
					pDevice->m_bReq2LevelData = true;
					pDevice->m_nSendResetCUCounter = 0;
				}
			}
		}

		WORD *pGIN = NULL;
		OpcUa_Byte* pByte = NULL;
		OpcUa_Int32 ibase = -1;

		CIOCfgTag* pTag = NULL;

		OpcUa_Byte bDIValue = 0;
		OpcUa_Byte Time[4] = {0};
		OpcUa_Byte bAINum = 0;
		OpcUa_Int32 iTagIndex = 0;
		OpcUa_Int32 iSecond = 0;		

		switch ( pPacketBuffer[0] )
		{
		case 0xE5:				//һ���ֽڵ���Ӧ֡
		case 0xA2:				//һ���ֽڵ���Ӧ֡
			break;
		case GUDINGZHEN_START_ID:				//�̶�֡
			break;
		case BIANCHANGZHEN_START_ID:			//�ɱ䳤֡
			{
				CHANGEABLE_FRAME_STRUCT *RevFrame = (CHANGEABLE_FRAME_STRUCT *)pPacketBuffer;
				ASDU_HEAD_STRUCT *ReplyData = (ASDU_HEAD_STRUCT *)RevFrame->UserData;

				//���ASDU HeadԪ��
				OpcUa_Byte TypeFlg = ReplyData->TypeFlg;			//���ͱ�ʶ
				OpcUa_Byte COT	  = ReplyData->COT;				//����ԭ��
				OpcUa_Byte InfoSum = (ReplyData->VSQ&0x7F);		//VSQ�ĵ�7λ��ʾ��ϢԪ�صĸ���
				OpcUa_Byte bFunType	  = ReplyData->FunType;		//��������
				OpcUa_Int32 InfoNo		  = ReplyData->InfNumber;	//��Ϣ���
				bool bInfoCompressed  = (ReplyData->VSQ&0x80)==0;	//false��ʾInf����������ʱÿ�������Info.

				OpcUa_Int32 iStartTagAddr = bFunType*1000 + InfoNo;	//��ʵ��ǩ��ַ��������Tag%U(0,0,255)%*1000:%+U(0,0,255)�ͱ�ǩ��
				switch (pPacketBuffer[6])			//ASDU����
				{
				case TYP_WITH_TIME:		//ASDU1
					{
						ASDU1_DPItm *pPoint = (ASDU1_DPItm *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( ASDU1_DPItm ) )
						{
							snprintf(chMsg,sizeof(chMsg)
								,"�豸(��ַ%d)��λDI ASDU1���ȷǷ�!", pDevice->GetAddr());
							pDrv->WriteLog(chMsg);
							
							break;
						}

						std::string strCOT = "δ֪";
						if ( COT == 1 )
						{
							strCOT = "��λͻ��";
						}
						else if ( COT == 9 )
						{
							strCOT = "���ٻ�";
						}
						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)DI ASDU1(����ԭ��%d(%s)��Ϣ����%d,��������%d,��Ϣ���%d,ֵ%d)!"
							, pDevice->GetAddr(),COT, strCOT.c_str(), InfoSum, bFunType, InfoNo, pPoint->DPI.DPI -1 );
						pDrv->WriteLog(chMsg);

						for ( OpcUa_Int32 i = 0; i < InfoSum; i++ )
						{
							if( pPoint->DPI.DPI == 1 || pPoint->DPI.DPI == 2 )	//valid
							{
								bDIValue = pPoint->DPI.DPI -1;	//��˫��ֵת���뵥��ֵ�ĺ���һ��

								//�ҵ���ǩ
								iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
								if (-1 == iTagIndex)
								{									
									snprintf(chMsg,sizeof(chMsg)
										,"ASDU1: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!", bFunType, InfoNo, bDIValue);
									pDrv->WriteLog(chMsg);
								}
									
								OpcUa_Int64 llTime= 0;
								if ( pPoint->Time.IV == 1 )			//ʱ����Ч
								{
									pDevice->m_bNeedCheckTime = true;												
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI��λ ������IV=1, ASDU%d, ��ʾʱ����Ч, ��Ҫ��ʱ!", pDevice->GetAddr(), 1);
									pDrv->WriteLog(chMsg);
								}
								else if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
									pDrv->WriteLog(chMsg);

									WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
									OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;

									ACE_Time_Value val = ACE_OS::gettimeofday();	
									OpcUa_Int64 llTime = val.sec()*1000LL + val.usec()/1000LL;	
									ACE_Date_Time dtime;
									dtime.update( val );

									llTime -= ( dtime.hour()*3600000 +dtime.minute()*60000+dtime.second()*1000+dtime.microsec()/1000);
									llTime += nTime;
									//llTime += 28800000;
								}
								else
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI,ASDU%d, ������ʱ����Ч, ��Ϊ��ʱ��!",pDevice->GetAddr(), 1 );
									pDrv->WriteLog(chMsg);
								}

								if ( -1 != iTagIndex )
								{
									pDevice->SetVal( MacsDIS103_DI,iTagIndex,  &(pDevice->m_DIs[iTagIndex] = bDIValue),sizeof(OpcUa_Byte),llTime , bFromLocal );
									
									if ( (pDevice->TagNeedSOE(MacsDIS103_DI,iTagIndex)
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI,iTagIndex))
										&&bDIValue)
									{
// 										CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 											pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)bDIValue, llTime);		//! ����SOE
									}
								}

								pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo,  &bDIValue, sizeof(OpcUa_Byte), llTime , bFromLocal );
								OpcUa_Float fValue = (OpcUa_Float)pPoint->DPI.DPI;
								pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo,  (OpcUa_Byte*)&fValue, sizeof(fValue), llTime , bFromLocal );

								OpcUa_Byte Second;
								WORD MilliSecond;
								pDevice->GetTimeFromCP32Time( pPoint->Time, Second, MilliSecond );									
								snprintf(chMsg,sizeof(chMsg)
									,"ASDU1: ����DI��[��������%d,��Ϣ���%d,ֵ%d,��:%d,����:%d]��IO!"
									, bFunType, InfoNo, bDIValue,Second,MilliSecond);
								pDrv->WriteLog(chMsg);
							}
							pPoint++;
							InfoNo++;
						}
					}
					break;
				case TYP_WITH_TINE_REL:		//ASDU2
					{
						ASDU2_DPItm *pPoint = (ASDU2_DPItm *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( ASDU2_DPItm ) )
						{							
							snprintf(chMsg,sizeof(chMsg)
								,"�豸(��ַ%d)��λDI ASDU2���ȷǷ�!", pDevice->GetAddr());
							pDrv->WriteLog(chMsg);
							
							break;
						}

						std::string strCOT = "δ֪";
						if ( COT == 1 )
						{
							strCOT = "��λͻ��";
						}
						else if ( COT == 9 )
						{
							strCOT = "���ٻ�";
						}

						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)��λDI ASDU2(����ԭ��%d(%s)��Ϣ����%d,��������%d,��Ϣ���%d,ֵ%d)!"
							, pDevice->GetAddr(),COT, strCOT.c_str(), InfoSum, bFunType, InfoNo, pPoint->DPI.DPI -1 );
						pDrv->WriteLog(chMsg);
						
						for ( OpcUa_Int32 i = 0; i < InfoSum; i++ )
						{
							if( pPoint->DPI.DPI == 1 || pPoint->DPI.DPI == 2 )	//valid
							{
								bDIValue = pPoint->DPI.DPI -1;	//��˫��ֵת���뵥��ֵ�ĺ���һ��

								//�ҵ���ǩ
								iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
								if (-1 == iTagIndex)
								{									
									snprintf(chMsg,sizeof(chMsg)
										,"ASDU2: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!", bFunType, InfoNo, bDIValue);
									pDrv->WriteLog(chMsg);
								}

								OpcUa_Int64 llTime = 0;
								if ( pPoint->Time.IV == 1 )			//ʱ����Ч
								{
									pDevice->m_bNeedCheckTime = true;												
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI��λ ������IV=1, ASDU%d, ��ʾʱ����Ч, ��Ҫ��ʱ!", pDevice->GetAddr(), 2);
									pDrv->WriteLog(chMsg);
								}
								else if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 2);
									pDrv->WriteLog(chMsg);

									WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
									OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;									
									
									ACE_Time_Value val = ACE_OS::gettimeofday();	
									OpcUa_Int64 llTime = val.sec()*1000LL + val.usec()/1000LL;	
									ACE_Date_Time dtime;
									dtime.update( val );

									llTime -= ( dtime.hour()*3600000 +dtime.minute()*60000+dtime.second()*1000+dtime.microsec()/1000);
									llTime += nTime;
								}
								else
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI,ASDU%d, ������ʱ����Ч, ��Ϊ��ʱ��!",pDevice->GetAddr(), 2 );
									pDrv->WriteLog(chMsg);
								}										
								
								if ( -1 != iTagIndex )
								{
									pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = bDIValue),sizeof(OpcUa_Byte), llTime , bFromLocal);
									if ( (pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
										&&bDIValue)
									{							
																			
// 										CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 											pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)bDIValue, llTime);		//! ����SOE
																	
									}
								}

								pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo,  &bDIValue, sizeof(OpcUa_Byte), llTime , bFromLocal );
								OpcUa_Float fValue = (OpcUa_Float)pPoint->DPI.DPI;
								pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo,  (OpcUa_Byte*)&fValue, sizeof(fValue), llTime , bFromLocal );

								OpcUa_Byte Second;
								WORD MilliSecond;
								pDevice->GetTimeFromCP32Time( pPoint->Time, Second, MilliSecond );									
								snprintf(chMsg,sizeof(chMsg)
									,"ASDU2: ����DI��[��������%d,��Ϣ���%d,ֵ%d,��:%d,����:%d]��IO!"
									, bFunType, InfoNo, bDIValue,Second,MilliSecond);
								pDrv->WriteLog(chMsg);
							}
							pPoint++;
							InfoNo++;
						}
					}
					break;

				case TYP_VALUE_WITH_TIME:		//ASDU4
					{
						bFunType = pPacketBuffer[10];
						InfoNo = pPacketBuffer[11];
						bAINum = (pPacketBuffer[7]&0x7F);

						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)�����ʱ�����ʱ��ı���ֵ����ASDU4(Fun:%d,Info:%d,��Ϣ����%d)!"
							, pDevice->GetAddr(),bFunType,InfoNo,InfoSum);
						pDrv->WriteLog(chMsg);

						pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo, pPacketBuffer+12,sizeof(OpcUa_Float),0, bFromLocal );

						if ((bFunType != pDevice->m_byAIFUN)||(InfoNo != pDevice->m_byAISEQ) )
							break;

						if( bAINum != 1 )
							break;

						pDevice->SetVal( MacsDIS103_AI, 14, pPacketBuffer+12,sizeof(OpcUa_Float),0, bFromLocal );
					}
					break;
				case TYP_VALUE1:		//ASDU3
				case TYP_VALUE2:		//ASDU9
					{
						bFunType = pPacketBuffer[10];
						InfoNo = pPacketBuffer[11];
						bAINum = (pPacketBuffer[7]&0x7F);

/*
						if (bAINum > 16|| bAINum < 3)
							break;
*/

						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)AI ASDU9(��Ϣ����%d��������%d,��Ϣ���%d)!"
							, pDevice->GetAddr(), InfoSum, bFunType, InfoNo);
						pDrv->WriteLog(chMsg);

						MEA_AI* pAI = (MEA_AI* )(pPacketBuffer+12);
						for( OpcUa_Int32 i = 0; i < bAINum; i++ )
						{
							if ( pAI->ER == 0 )	//valid
							{
								//((FLOAT)(pAI->Value)) / 4095 *12/10;	//��һ��ֵ
								OpcUa_Float temp = (pDevice->GetModulusByTag(i+1))*((OpcUa_Float)(pAI->Value));
								//	OpcUa_Float temp = ((OpcUa_Float)(pAI->Value));	//���������������Զ����㹤��ֵ��
								//	pDevice->SetVal( MacsDIS103_AI, iStartTagAddr+i, temp, bFromLocal );	//���Կ��ǻ���ͨ�õ�
								pDevice->SetVal( MacsDIS103_AI, i+1, (OpcUa_Byte*)&temp,sizeof(temp),0, bFromLocal );

								OpcUa_Float fVal = ((OpcUa_Float)(pAI->Value));
								pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo, (OpcUa_Byte*)&fVal,sizeof(fVal),0, bFromLocal );
							}
							InfoNo++;
							pAI++;
						}
					}
					break;
				case TYP_IDENTIFIER:						//ASDU5����ʶ����
					{
						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)�����̱�ʶ����ASDU5!.", pDevice->GetAddr());

						pDrv->WriteLog( chMsg );
						MakeLog(pPacketBuffer, nPacketLength, false);

						if ( pDevice->m_bNew )
							pDevice->m_bNew = false;
						else									//�����������ӵ��豸���Ͳ���������Уʱ���ܲ�ѯ��
							break;
						if ( COT_CU == pPacketBuffer[8] )
						{
							//�����豸���ٻ�
							ActiveWholeCall( pDevice );

							if ( m_bBroadCheckTime ) 
							{
								m_bIsTimeForBroadCheckTime = true;
							}
							else
								pDevice->m_bNeedCheckTime = true;

							pDrv->WriteLog( "ASDU5������ʱ��" );
						}
						if ( COT_START == pPacketBuffer[8]|| COT_POWERON == pPacketBuffer[8])		//��ʼ������
						{
							//�����豸���ٻ�
							ActiveWholeCall( pDevice );

							if ( m_bBroadCheckTime )
							{
								m_bIsTimeForBroadCheckTime = true;
							}
							else
								pDevice->m_bNeedCheckTime = true;

							pDrv->WriteLog( "ASDU5������ʱ��" );
						}
					}
					break;
				case TYP_TIME_SYN:							//ASDU6��ʱ��ͬ����������
					{						
						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)ʱ��ͬ����������(����������%d)!."
							, pDevice->GetAddr(), pCtrl->FunCode );
						pDrv->WriteLog( chMsg );
						MakeLog(pPacketBuffer, nPacketLength, false);
					}
					break;
				case TYP_GENERAL_QUERY_END:			//�ܲ�ѯ�����ٻ�����ֹ�������ͱ�ʶ  ASDU8	
					{
						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)�ܲ�ѯ�����ٻ�����ֹ!."
							, pDevice->GetAddr() );
						pDrv->WriteLog( chMsg );
						MakeLog(pPacketBuffer, nPacketLength, false);
						
						//pDevice->SetCanSendSoe(true);
					}
					break;
				case TYP_DISORDER_DATA:				//����¼���Ŷ���	ASDU23
					{
						if ( IsEnableFaultWave() )
						{
							MakeLog(pPacketBuffer, nPacketLength, false);

							if ( ((pPacketBuffer[7]&0x7F) > 0)&&((pPacketBuffer[7]&0x7F) < 9) )
							{
								//Add by hxw in 2008.11.27. ¼����ȡʱ��ǿ�Ƽ�¼��־������
								//if ( m_msFaultLogLifeCycle > 0 )
								//{
								//	CIOChannelBase* p103Channel = pDrv->GetChannelObj();
								//	//del 2011-1018
								//	//p103Channel->SetMonitor( false );							
								//	//p103Channel->SetLog(true);
								//	m_pFaultLogTimer->SetTimer( m_msFaultLogLifeCycle );
								//}


								//�������¼���Ŷ����գ�Ҳ���Ƿ����˹��ϣ�����������
								//����Ὣ���й��ϼ�¼��һ��vector�У��������豸�ġ�����ѡ���־����pDevice->m_bFaultSel����Ϊtrue��
								pDevice->m_bIsRcvingFaultWave = true;
								pDevice->m_pFaultTimeOutTimer->KillTimer();
								pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

								pDevice->m_byFaultFUN = pPacketBuffer[10];
								//del zyg EnterCriticalSection( &pDevice->m_FaultWaveSection );
								pDevice->m_FaultDataVec.clear();
								//del zyg LeaveCriticalSection( &pDevice->m_FaultWaveSection );
								pDevice->m_byFaultNum = (pPacketBuffer[7]&0x7F);

								snprintf(chMsg,sizeof(chMsg)
									,"�豸(��ַ%d)���Ŷ���Ϊ��(��������-%d,���ϸ���-%d)!"
									, pDevice->GetAddr(),pDevice->m_byFaultFUN, pDevice->m_byFaultNum );
								pDrv->WriteLog(chMsg);

								//��¼������ź͹���״̬
								//del zyg EnterCriticalSection( &pDevice->m_FaultWaveSection );
								for(OpcUa_Int32 i = 0; i < pDevice->m_byFaultNum; i++)
								{
									FaultDataSet faultData;
									faultData.FAN = MAKEWORD(pPacketBuffer[12+10*i], pPacketBuffer[12+10*i+1]) ;
									faultData.SOF = pPacketBuffer[12+10*i+2];
									memcpy(&(faultData.TimeSet), &(pPacketBuffer[12+10*i+3]), 7);

									pDevice->m_FaultDataVec.push_back(faultData);
								}
								//del zyg LeaveCriticalSection( &pDevice->m_FaultWaveSection );

								pDevice->m_bFaultSel = true;	//��m_FaultDataVec.push_back��Ϻ����ñ�־,�Ա�֤�̰߳�ȫ.
								pDevice->m_byRcvFaultNum = 0;

								/*	if ( !(SendNoteMessage(pDevice)) ) 
								TRACE("���ļ��������ڴ淢��֪ͨ��Ϣʧ�ܣ�");

								if( !(SendFaultFrame( pDevice, pPacketBuffer, nPacketLength )) )
								TRACE("���ļ��������ڴ���ӱ���ʧ�ܣ�");*/
								pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);
							}
							else if ((pPacketBuffer[7]&0x7F) == 0)			//�Ŷ���Ϊ��
							{						
								snprintf(chMsg,sizeof(chMsg)
									,"�豸(��ַ%d)���Ŷ���Ϊ��!", pDevice->GetAddr() );
								pDrv->WriteLog(chMsg);
								pDevice->m_bIsRcvingFaultWave = false;
								pDevice->m_pFaultTimeOutTimer->KillTimer();
								pDevice->m_byRcvFaultNum = 0;
							}					
						}
						else
						{
							snprintf(chMsg,sizeof(chMsg)
								,"�յ��豸(��ַ%d)��ASDU23��������Ϊ�豸����Ҫ¼����������¼����ȡ����!", pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
						}
					}
					break;
				case TYP_DISORDER_DATA_READY:					//�Ŷ����ݴ���׼������		ASDU26				
					MakeLog(pPacketBuffer, nPacketLength, false);
					
					if ( pDevice->m_wFAN == MAKEWORD(pPacketBuffer[14], pPacketBuffer[15]) )
					{
						pDevice->m_bIsRcvingFaultWave = true;
						pDevice->m_pFaultTimeOutTimer->KillTimer();
						pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

						pDevice->m_byNOC = pPacketBuffer[18];
						pDevice->m_NOE = MAKEWORD(pPacketBuffer[19], pPacketBuffer[20]);
						//��ϢԪ�ؼ��
						pDevice->m_sINT = MAKEWORD(pPacketBuffer[21], pPacketBuffer[22]);
						//��һ����ϢԪ�ص�ʱ��
						TIMESTAMP_4BYTE timeSet;
						memcpy(&timeSet, &pPacketBuffer[23], 4);
						//���Է����Ŷ����������ġ���
						pDevice->m_bReqDisData = true;		

						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)���Ŷ����ݴ���׼������(�������-%d,ͨ����Ŀ-%d, һ��ͨ����ϢԪ�ص���Ŀ-%d, �������-%d΢��)!"
							, pDevice->GetAddr(),pDevice->m_wFAN, pDevice->m_byNOC, pDevice->m_NOE, pDevice->m_sINT );
						pDrv->WriteLog(chMsg);
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);
					}
					break;
				case TYP_CHANNEL_READY:							//����¼�ĵ�X��ͨ������׼������  ASDU27
					MakeLog(pPacketBuffer, nPacketLength, false);

					if ( pDevice->m_wFAN == MAKEWORD(pPacketBuffer[14], pPacketBuffer[15]) )
					{
						pDevice->m_bIsRcvingFaultWave = true;
						pDevice->m_pFaultTimeOutTimer->KillTimer();
						pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

						pDevice->m_bChannelReady = true;
						pDevice->m_byACC = pPacketBuffer[16];
						/*17---20����һ�ζֵ*/	/*21---24������ζֵ*/  /*25---28����α�����*/						
						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)�ı���¼�ĵ�X��ͨ������׼������(�������-%d,ͨ�����-%d)!"
							, pDevice->GetAddr(),pDevice->m_wFAN, pDevice->m_byACC );
						pDrv->WriteLog(chMsg);
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);
					}
					break;
				case TYP_STATUS_ALTER_READY:					//����־��״̬��λ����׼������		ASDU28
					MakeLog(pPacketBuffer, nPacketLength, false);

					if ( pDevice->m_wFAN == MAKEWORD(pPacketBuffer[14], pPacketBuffer[15]) )
					{					
						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)�Ĵ���־��״̬��λ����׼������(�������-%d)!"
							, pDevice->GetAddr(),pDevice->m_wFAN );
						pDrv->WriteLog(chMsg);

						pDevice->m_bIsRcvingFaultWave = true;
						pDevice->m_pFaultTimeOutTimer->KillTimer();
						pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

						//���Է����������־��״̬��λ���䡱������
						pDevice->m_bReqStatusAlter = true;

						/*if( !(SendFaultFrame( pDevice, pPacketBuffer, nPacketLength )) )
						TRACE("���ļ��������ڴ���ӱ���ʧ�ܣ�");*/
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);
					}
					break;
				case TYP_STATUS_ALTER_TRANS:					//���ʹ���־��״̬��λ�ĳ�ʼ״̬��״̬��λ	ASDU29
					MakeLog(pPacketBuffer, nPacketLength, false);
					
					if ( pDevice->m_wFAN == MAKEWORD(pPacketBuffer[12], pPacketBuffer[13]) )
					{
						pDevice->m_bIsRcvingFaultWave = true;
						pDevice->m_pFaultTimeOutTimer->KillTimer();
						pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

						//��¼����־��״̬��λ����Ŀ
						pDevice->m_byNOT = pPacketBuffer[14];

						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)���ʹ���־��״̬��λ�ĳ�ʼ״̬��״̬��λ(�������-%d, ����־��״̬��λ����Ŀ-%d)!"
							, pDevice->GetAddr(),pDevice->m_wFAN,pDevice->m_byNOT );
						pDrv->WriteLog(chMsg);

						if (0 == MAKEWORD(pPacketBuffer[15], pPacketBuffer[16]))	//��ʼ״̬(TAP == 0)
						{
							pDevice->m_statusMap.clear();
							for (OpcUa_Int32 i = 0; i < pDevice->m_byNOT; i++)
							{
								bool bDPI = false;
								if((pPacketBuffer[17+3*i+2]&0x03) == 2)
									bDPI = true;
								else if((pPacketBuffer[17+3*i+2]&0x03) == 1)
									bDPI = false;
								/*else
								TRACE("�õ�DPIֵ�쳣��");*/
								//�ѳ�ʼ״̬������
								pDevice->m_statusMap.insert(StatusMap::value_type(
									1000*pPacketBuffer[17+3*i]+pPacketBuffer[17+3*i+1], bDPI ));
							}
						}
						else														//״̬��λ(����TAP�������)
						{
							for (OpcUa_Int32 i = 0; i < pDevice->m_byNOT; i++)
							{
								bool bDPI = false;
								if((pPacketBuffer[17+3*i+2]&0x03) == 2)
									bDPI = true;
								else if((pPacketBuffer[17+3*i+2]&0x03) == 1)
									bDPI = false;
								/*else
								TRACE("�õ�DPIֵ�쳣��");*/

								StatusMapiterator it;

								it = pDevice->m_statusMap.find( 1000*pPacketBuffer[17+3*i]+pPacketBuffer[17+3*i+1] );
								if ( it == pDevice->m_statusMap.end() )
								{
									//������
									break;
								}
								else
								{
									if (it->second != bDPI)		//���������������ֵ��ͬ
									{
										//
									}
									else
									{
										//������
									}
								}
							}
						}
						/*if( !(SendFaultFrame( pDevice, pPacketBuffer, nPacketLength )) )
						TRACE("���ļ��������ڴ���ӱ���ʧ�ܣ�");*/
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);
					}
					break;
				case TYP_DISORDER_TRANS:		//ASDU30
					MakeLog(pPacketBuffer, nPacketLength, false);
					
					if ( (pDevice->m_wFAN == MAKEWORD(pPacketBuffer[14], pPacketBuffer[15])) &&
						pDevice->m_byACC == pPacketBuffer[16])
					{
						pDevice->m_bIsRcvingFaultWave = true;
						pDevice->m_pFaultTimeOutTimer->KillTimer();
						pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_TIMEOUT );

						pDevice->m_bChannelEnd = true;
						pDevice->m_sNDV = pPacketBuffer[17];
						pDevice->m_sNFE = MAKEWORD(pPacketBuffer[18], pPacketBuffer[19]);

						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)�����Ŷ�ֵ(�������-%d,ͨ�����-%d,ÿ��ASDU�й��Ŷ�ֵ����Ŀ-%d, �����Ŷ�ֵ�����е�һ����ϢԪ�ص����-%d )!"
							, pDevice->GetAddr(),pDevice->m_wFAN,pDevice->m_byACC,pDevice->m_sNDV,pDevice->m_sNFE );
						pDrv->WriteLog(chMsg);

						for (OpcUa_Int32 i = 0; i < pDevice->m_sNDV; i += 2)
						{
							//����ÿ���Ŷ�ֵ   ��pPacketBuffer[20], pPacketBuffer[21]��ʼ
							//MAKEWORD(pPacketBuffer[20+i], pPacketBuffer[21+i]);
						}
						/*if( !(SendFaultFrame( pDevice, pPacketBuffer, nPacketLength )) )
						TRACE("���ļ��������ڴ���ӱ���ʧ�ܣ�");*/
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);

					}
					break;
				case TYP_TRANS_END:		//ASDU31
					MakeLog(pPacketBuffer, nPacketLength, false);
					
					if ( pDevice->m_wFAN == MAKEWORD(pPacketBuffer[14], pPacketBuffer[15]) )
					{
						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)���ͽ���[%d]..."
							, pDevice->GetAddr(), pPacketBuffer[12] );
						pDrv->WriteLog(chMsg);
						
						//���Է����Ŷ������Ͽɡ�������
						pDevice->m_bSendDisDataAck = true;
						switch (pPacketBuffer[12])
						{
						case TOO_ALTER_END_WITHOUTPAUSE:			//����־��״̬��λ�������							
							snprintf(chMsg,sizeof(chMsg)
								,"�豸(��ַ%d)����־��״̬��λ�������(DI Good)!"
								, pDevice->GetAddr()  );
							pDrv->WriteLog(chMsg);

							pDevice->m_byDisDataAckTOO = TOO_STATUSCHANGED_SUCCESS;			//�϶�
							/*ʲôʱ��񶨣�*/

							break;
						case TOO_CHANEL_END_WITHOUTPAUSE:							
							snprintf(chMsg,sizeof(chMsg)
								,"�豸(��ַ%d)������ֹ��ͨ������Ľ���(AI Good)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							pDevice->m_byDisDataAckTOO = TOO_CHANEL_SUCCESS;			//�϶�
							/*ʲôʱ��񶨣�*/

							break;
						case TOO_DISDATA_END_WITHOUTPAUSE:							
							snprintf(chMsg,sizeof(chMsg)
								,"�豸(��ַ%d)������ֹ���Ŷ����ݴ���Ľ���(Wave Finished)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);

							pDevice->m_byDisDataAckTOO = TOO_DISDATA_SUCCESS;			//�϶�
							/*ʲôʱ��񶨣�*/

							pDevice->m_byRcvFaultNum++;
							if ( pDevice->m_byRcvFaultNum == pDevice->m_byFaultNum )	//�豸���еĹ���¼������������.
							{							
								pDevice->m_byRcvFaultNum = 0;
							}
							break;
						case 39:							
							snprintf(chMsg,sizeof(chMsg)
								,"�豸(��ַ%d)�ɿ���ϵͳ����ֹ�Ĵ���־��״̬��λ����Ľ���(Bad)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							break;
						case 40:							
							snprintf(chMsg,sizeof(chMsg)
								,"�豸(��ַ%d)���豸����ֹ�Ĵ���־��״̬��λ����Ľ���(Bad)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							break;
						case 33:							
							snprintf(chMsg,sizeof(chMsg)
								,"�豸(��ַ%d)�ɿ���ϵͳ����ֹ���Ŷ����ݴ���Ľ���(Bad)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							break;
						case 34:							
							snprintf(chMsg,sizeof(chMsg)
								, "�豸(��ַ%d)���豸����ֹ���Ŷ����ݴ���Ľ���(Bad)!", pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							break;
						case 36:							
							snprintf(chMsg,sizeof(chMsg)
								, "�豸(��ַ%d)�ɿ���ϵͳ����ֹ��ͨ������Ľ���(Bad)!"
								, pDevice->GetAddr() );
							pDrv->WriteLog(chMsg);
							break;
						case 37:							
							snprintf(chMsg,sizeof(chMsg)
								, "�豸(��ַ%d)���豸����ֹ��ͨ������Ľ���(Bad)!"
								, pDevice->GetAddr());
							pDrv->WriteLog(chMsg);
							break;

						default:
							break;
						}
						//zyg  if( !(SendFaultFrame( pDevice, pPacketBuffer, nPacketLength )) )
						pDevice->m_pPRDFile->DealRcvData(pPacketBuffer);

					}
					break;
				case 40:	//ASDU40: ���ٻ�ʱ����״̬��ȫң����Ӧ
					if ( bInfoCompressed )
					{
						SINGLE_POINT *pPoint = (SINGLE_POINT *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( SINGLE_POINT ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->IV == 0)//valid
							{
								//�ҵ���ǩ
								iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
								if (-1 == iTagIndex)
								{								
									snprintf(chMsg,sizeof(chMsg)
										, "ASDU40: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!"
										, bFunType, InfoNo, pPoint->SPI);
									pDrv->WriteLog(chMsg);
								}

								if (-1 != iTagIndex)
								{
									pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = pPoint->SPI), sizeof(OpcUa_Byte),0,bFromLocal );
									if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
										&&pPoint->SPI)
									{										
// 										CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 											pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)pPoint->SPI, 0);		//! ����SOE
									}
								}

								OpcUa_Byte bDIValue = pPoint->SPI;
								pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo,  &bDIValue, sizeof(OpcUa_Byte), 0 , bFromLocal );
							}
							pPoint++;
							InfoNo++;
						}
					}
					else
					{
						SINGLE_POINT_INFO *pPoint = (SINGLE_POINT_INFO *)((OpcUa_Byte*)(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( SINGLE_POINT_INFO ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->IV == 0)//valid
							{
								iTagIndex = pDevice->GetTagIndex(bFunType, pPoint->Info);
								if (-1 == iTagIndex)
								{									
									snprintf(chMsg,sizeof(chMsg)
										, "ASDU40: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!"
										, bFunType, pPoint->Info, pPoint->SPI);
									pDrv->WriteLog(chMsg);
								}

								if (-1 != iTagIndex)
								{
									pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = pPoint->SPI),sizeof(OpcUa_Byte),0, bFromLocal );
									if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
										&&pPoint->SPI)
									{										
// 										CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 											pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)pPoint->SPI, 0);		//! ����SOE
									}
								}

								OpcUa_Byte bDIValue = pPoint->SPI;
								pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+pPoint->Info, &bDIValue, sizeof(OpcUa_Byte), 0 , bFromLocal );
							}
							pPoint++;

						}
					}
					break;
				case 41:	//ASDU41: ����SOE
					if ( bInfoCompressed )
					{
						SIQ_TIME *pPoint = (SIQ_TIME *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( SIQ_TIME ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->SIQ.IV == 0)//valid
							{
								//�ҵ���ǩ
								iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
								if (-1 == iTagIndex)
								{									
									snprintf(chMsg,sizeof(chMsg)
										,"ASDU41: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!"
										, bFunType, InfoNo, pPoint->SIQ.SPI);
									pDrv->WriteLog(chMsg);
								}

	
								if (-1 != iTagIndex)
								{
									pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = pPoint->SIQ.SPI), sizeof(OpcUa_Byte),0, bFromLocal );
									if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
										&&pPoint->SIQ.SPI)
									{											
										if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
										{											
											WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
											OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;
// 											CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 												pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)pPoint->SIQ.SPI, nTime);		//! ����SOE
										}
									}
								}

								if ( pPoint->Time.IV == 1 )			//ʱ����Ч
								{
									pDevice->m_bNeedCheckTime = true;												
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI��λ ������IV=1, ASDU%d, ��ʾʱ����Ч, ��Ҫ��ʱ!", pDevice->GetAddr(), 1);
									pDrv->WriteLog(chMsg);
								}
								else if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
									pDrv->WriteLog(chMsg);

									WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
									OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;

									OpcUa_Byte bDIValue = pPoint->SIQ.SPI;
									pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo, &bDIValue, sizeof(OpcUa_Byte), nTime , bFromLocal );
								}
								else
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI,ASDU%d, ������ʱ����Ч, ��Ϊ��ʱ��!",pDevice->GetAddr(), 41 );
									pDrv->WriteLog(chMsg);
								}										
							}
							pPoint++;
							InfoNo++;
						}
					}
					else
					{
						SIQ_TIME_INFO *pPoint = (SIQ_TIME_INFO *)((OpcUa_Byte*)(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( SIQ_TIME_INFO ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->SIQ.IV == 0)//valid
							{
								//�ҵ���ǩ
								iTagIndex = pDevice->GetTagIndex(bFunType, pPoint->Info);
								if (-1 == iTagIndex)
								{									
									snprintf(chMsg,sizeof(chMsg)
										,"ASDU41: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!"
										, bFunType, pPoint->Info, pPoint->SIQ.SPI);
									pDrv->WriteLog(chMsg);
								}

								if (-1 != iTagIndex)
								{
									pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = pPoint->SIQ.SPI), sizeof(OpcUa_Byte), 0, bFromLocal );
									if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
										|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
										&&pPoint->SIQ.SPI)
									{
										if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
										{											
											WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
											OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;
// 											CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 												pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)pPoint->SIQ.SPI, nTime);		//! ����SOE
										}
									}
								}

								if ( pPoint->Time.IV == 1 )			//ʱ����Ч
								{
									pDevice->m_bNeedCheckTime = true;												
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI��λ ������IV=1, ASDU%d, ��ʾʱ����Ч, ��Ҫ��ʱ!", pDevice->GetAddr(), 41);
									pDrv->WriteLog(chMsg);
								}
								else if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 41);
									pDrv->WriteLog(chMsg);

									WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
									OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;

									OpcUa_Byte bDIValue = pPoint->SIQ.SPI;
									pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+pPoint->Info, &bDIValue, sizeof(OpcUa_Byte), nTime , bFromLocal );

								}
								else
								{											
									snprintf(chMsg,sizeof(chMsg)
										,"�豸(��ַ%d)DI,ASDU%d, ������ʱ����Ч, ��Ϊ��ʱ��!",pDevice->GetAddr(), 41 );
									pDrv->WriteLog(chMsg);
								}				
							}

							pPoint++;
						}
					}
					break;
				case 42:	//ASDU40: ���ٻ�ʱ˫��״̬��ȫң����Ӧ
					if ( bInfoCompressed )
					{
						DOUBLE_POINT *pPoint = (DOUBLE_POINT *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( DOUBLE_POINT ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->IV == 0 )//valid
							{
								if ( pPoint->DPI == 1 || pPoint->DPI == 2 )
								{
									OpcUa_Byte Val = pPoint->DPI-1;

									//�ҵ���ǩ
									iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
									if (-1 == iTagIndex)
									{										
										snprintf(chMsg,sizeof(chMsg)
											,"ASDU42: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!"
											, bFunType, InfoNo, Val);
										pDrv->WriteLog(chMsg);
									}

									if (-1 != iTagIndex)
									{
										pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val),sizeof(OpcUa_Byte),0, bFromLocal );
										if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
											|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
											&&Val)
										{											
// 											CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 												pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)Val, 0);		//! ����SOE
										}
									}

									pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo, &Val, sizeof(OpcUa_Byte), 0 , bFromLocal );
									OpcUa_Float fValue = (OpcUa_Float)pPoint->DPI;
									pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo, (OpcUa_Byte*)&fValue, sizeof(fValue), 0 , bFromLocal );
								}
							}
							pPoint++;
							InfoNo++;
						}
					}
					else
					{
						DOUBLE_POINT_INFO *pPoint = (DOUBLE_POINT_INFO *)((OpcUa_Byte*)(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( DOUBLE_POINT_INFO ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if( pPoint->IV == 0 )//valid
							{
								if ( pPoint->DPI == 1 || pPoint->DPI == 2 )
								{
									OpcUa_Byte Val = pPoint->DPI - 1;

									//�ҵ���ǩ
									iTagIndex = pDevice->GetTagIndex(bFunType, pPoint->Info);
									if (-1 == iTagIndex)
									{										
										snprintf(chMsg,sizeof(chMsg)
											,"ASDU42: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!"
											, bFunType, pPoint->Info, Val);
										pDrv->WriteLog(chMsg);
									}

									if (-1 != iTagIndex)
									{
										pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val),sizeof(OpcUa_Byte),0, bFromLocal );
									}

									pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+pPoint->Info, &Val, sizeof(OpcUa_Byte), 0 , bFromLocal );
									OpcUa_Float fValue = (OpcUa_Float)pPoint->DPI;
									pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+pPoint->Info, (OpcUa_Byte*)&fValue, sizeof(fValue), 0 , bFromLocal );
								}
							}
							pPoint++;
						}
					}
					break;
				case 43:	//ASDU43: ����˫��SOE
					if ( bInfoCompressed )
					{
						DIQ_TIME *pPoint = (DIQ_TIME *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( DIQ_TIME ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->DIQ.IV == 0)//valid
							{
								if ( pPoint->DIQ.DPI == 1 || pPoint->DIQ.DPI == 2 )
								{
									OpcUa_Byte Val = pPoint->DIQ.DPI - 1;

									//�ҵ���ǩ
									iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
									if (-1 == iTagIndex)
									{										
										snprintf(chMsg,sizeof(chMsg)
											,"ASDU43: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!"
											, bFunType, InfoNo, Val);
										pDrv->WriteLog(chMsg);
									}

									if (-1 != iTagIndex)
									{
										pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val), sizeof(OpcUa_Byte),0, bFromLocal );
										if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
											|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
											&&Val)
										{									
											if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
											{											
												WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
												OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;
// 												CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 													pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , Val, nTime);		//! ����SOE
											}
										}
									}

									if ( pPoint->Time.IV == 1 )			//ʱ����Ч
									{
										pDevice->m_bNeedCheckTime = true;												
										snprintf(chMsg,sizeof(chMsg)
											,"�豸(��ַ%d)DI��λ ������IV=1, ASDU%d, ��ʾʱ����Ч, ��Ҫ��ʱ!", pDevice->GetAddr(), 43);
										pDrv->WriteLog(chMsg);
									}
									else if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
									{											
										snprintf(chMsg,sizeof(chMsg)
											,"�豸(��ַ%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 43);
										pDrv->WriteLog(chMsg);

										WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
										OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;

										pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo, &Val, sizeof(OpcUa_Byte), nTime , bFromLocal );
										OpcUa_Float fValue = (OpcUa_Float)pPoint->DIQ.DPI;
										pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo, (OpcUa_Byte*)&fValue, sizeof(fValue), nTime , bFromLocal );
									}
									else
									{											
										snprintf(chMsg,sizeof(chMsg)
											,"�豸(��ַ%d)DI,ASDU%d, ������ʱ����Ч, ��Ϊ��ʱ��!",pDevice->GetAddr(), 43 );
										pDrv->WriteLog(chMsg);
									}				
								}
							}
							pPoint++;
							InfoNo++;
						}
					}
					else
					{
						DIQ_TIME_INFO *pPoint = (DIQ_TIME_INFO *)((OpcUa_Byte*)(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( DIQ_TIME_INFO ) )
						{
							break;
						}

						for(OpcUa_Byte p = 0; p < InfoSum; p++)
						{
							if(pPoint->DIQ.IV == 0)//valid
							{
								if ( pPoint->DIQ.DPI == 1 || pPoint->DIQ.DPI == 2 )
								{
									OpcUa_Byte Val = pPoint->DIQ.DPI - 1;

									//�ҵ���ǩ
									iTagIndex = pDevice->GetTagIndex(bFunType, pPoint->Info);
									if (-1 == iTagIndex)
									{										
										snprintf(chMsg,sizeof(chMsg)
											,"ASDU43: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!"
											, bFunType, pPoint->Info, Val);
										pDrv->WriteLog(chMsg);
									}

									if (-1 != iTagIndex)
									{
										pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val), sizeof(OpcUa_Byte),0, bFromLocal );
										if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
											|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
											&&Val)
										{
											if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
											{											
												WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
												OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;
// 												CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 													pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , Val, nTime);		//! ����SOE
											}
										}
									}

									if ( pPoint->Time.IV == 1 )			//ʱ����Ч
									{
										pDevice->m_bNeedCheckTime = true;												
										snprintf(chMsg,sizeof(chMsg)
											,"�豸(��ַ%d)DI��λ ������IV=1, ASDU%d, ��ʾʱ����Ч, ��Ҫ��ʱ!", pDevice->GetAddr(), 43);
										pDrv->WriteLog(chMsg);
									}
									else if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
									{											
										snprintf(chMsg,sizeof(chMsg)
											,"�豸(��ַ%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 43);
										pDrv->WriteLog(chMsg);

										WORD millesec	= MAKEWORD(pPoint->Time.LowMillSec, pPoint->Time.HighMillSec);
										OpcUa_Int64 nTime = pPoint->Time.Hour *  60 * 60 * 1000 + pPoint->Time.Minute * 60 * 1000 + millesec;

										pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+pPoint->Info, &Val, sizeof(OpcUa_Byte), nTime , bFromLocal );
										OpcUa_Float fValue = (OpcUa_Float)pPoint->DIQ.DPI;
										pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+pPoint->Info, (OpcUa_Byte*)&fValue, sizeof(fValue), nTime , bFromLocal );
									}
									else
									{											
										snprintf(chMsg,sizeof(chMsg)
											,"�豸(��ַ%d)DI,ASDU%d, ������ʱ����Ч, ��Ϊ��ʱ��!",pDevice->GetAddr(), 43 );
										pDrv->WriteLog(chMsg);
									}				
								}
							}
							pPoint++;
						}
					}
					break;
				case 44:	//ASDU44: ����ȫң��
					if ( bInfoCompressed )
					{
						SCD_QDS *pPoint = (SCD_QDS *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 9) / sizeof( SCD_QDS ) )
						{
							break;
						}

						OpcUa_Byte Val = 0;
						for(OpcUa_Byte p = 0; p < InfoSum; p++)	//ÿ����ϢԪ����16��DI
						{
							if( pPoint->Quality.IV == 0 )//valid
							{
								for ( OpcUa_Int32 i = 0; i < 16; i++ )
								{
									if ( InfoNo < MAX_IEC103_INF )
									{
										Val = (OpcUa_Byte)((pPoint->DISet >> i)&0x01);

										//�ҵ���ǩ
										iTagIndex = pDevice->GetTagIndex(bFunType, InfoNo);
										if (-1 == iTagIndex)
										{											
											snprintf(chMsg,sizeof(chMsg)
												,"ASDU44: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!"
												, bFunType, InfoNo, Val);
											pDrv->WriteLog(chMsg);
										}

										if (-1 != iTagIndex)
										{
											pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val),sizeof(OpcUa_Byte),0, bFromLocal );
											if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
												|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
												&&Val)
											{												
// 												CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 													pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)Val, 0);		//! ����SOE
											}
										}

										pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+InfoNo, &Val, sizeof(OpcUa_Byte), 0 , bFromLocal );
									}
									InfoNo++;
								}
							}
							pPoint++;
						}
					}
					else
					{
						SCD_QDS_INFO *pPoint = (SCD_QDS_INFO *)((OpcUa_Byte* )(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( SCD_QDS_INFO ) )
						{
							break;
						}

						OpcUa_Byte Val = 0;
						for(OpcUa_Byte p = 0; p < InfoSum; p++)	//ÿ����ϢԪ����16��DI
						{
							if( pPoint->Quality.IV == 0 )//valid
							{
								for ( OpcUa_Int32 i = 0; i < 16; i++ )
								{
									if ( pPoint->Info < MAX_IEC103_INF )
									{
										Val = (OpcUa_Byte)((pPoint->DISet >> i)&0x01);

										//�ҵ���ǩ
										iTagIndex = pDevice->GetTagIndex(bFunType, pPoint->Info);
										if (-1 == iTagIndex)
										{										
											snprintf(chMsg,sizeof(chMsg)
												,"ASDU44: DI��[��������%d,��Ϣ���%d,ֵ%d]δ����!"
												, bFunType, pPoint->Info, Val);
											pDrv->WriteLog(chMsg);
										}

										if (-1 != iTagIndex)
										{
											pDevice->SetVal( MacsDIS103_DI, iTagIndex, &(pDevice->m_DIs[iTagIndex] = Val),sizeof(OpcUa_Byte),0, bFromLocal );
											if ((pDevice->TagNeedSOE(MacsDIS103_DI, iTagIndex) 
												|| pDevice->TagNeedDevSOE(MacsDIS103_DI, iTagIndex))
												&&Val)
											{												
// 												CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIC,
// 													pDevice->m_iInDOIndex, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)Val, 0);		//! ����SOE
											}
										}

										pDevice->SetVal( MacsDIS103_DIB, bFunType*1000+pPoint->Info, &Val, sizeof(OpcUa_Byte), 0 , bFromLocal );
									}
								}
							}
							pPoint++;
						}
					}
					break;
				case 50:	//ASDU50: ң���ϴ�
				case 51:	//ASDU51: ң���ϴ�
					if ( bInfoCompressed )
					{
						MEA_AI *pPoint = (MEA_AI *)(ReplyData + 1);
						if ( InfoSum != (RevFrame->Head.Len - 8) / sizeof( MEA_AI ) )
						{
							break;
						}

						for(OpcUa_Int32 i = 0; i < InfoSum; i++)
						{
							if( pPoint->ER == 0 )//valid
							{
								OpcUa_Float temp = (pDevice->GetModulusByTag(i+1))*((OpcUa_Float)(pPoint->Value));
								pDevice->SetVal( MacsDIS103_AI, i+1, (OpcUa_Byte*)&temp,sizeof(temp),0, bFromLocal );

								temp = (OpcUa_Float)pPoint->Value;
								pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+InfoNo, (OpcUa_Byte*)&temp, sizeof(temp), 0, bFromLocal );
							}
							InfoNo++;
							pPoint++;
						}
					}
					else
					{
						MEA_AI_INFO *pPoint = (MEA_AI_INFO *)((OpcUa_Byte* )(ReplyData + 1)-1);
						if ( InfoSum != (RevFrame->Head.Len - 7) / sizeof( MEA_AI_INFO ) )
						{
							break;
						}

						for(OpcUa_Int32 i = 0; i < InfoSum; i++)
						{
							if( pPoint->ER == 0 )//valid
							{
								OpcUa_Float temp = (pDevice->GetModulusByTag(i+1))*((OpcUa_Float)(pPoint->Value));
								pDevice->SetVal( MacsDIS103_AI, i+1, (OpcUa_Byte*)&temp,sizeof(temp),0, bFromLocal );

								temp = (OpcUa_Float)pPoint->Value;
								pDevice->SetVal( MacsDIS103_AIB, bFunType*1000+pPoint->Info, (OpcUa_Byte*)&temp, sizeof(temp), 0, bFromLocal );
							}
							pPoint++;
						}
					}
					break;
				case TYP_ASDU17:			//ʩ�͵±�����ֵ
					{
/*
ASDU140Ϊ��˾��·�����P521 103��˽��ASDU�������ٻ��в����ϴ��������·���Ӧ�ı��ķ����ϴ��� 
���磺����: 68 08 08 68 73 01 8C 81 14 01 01 02 99 16      FUN:01  INF:02  ��ȡ��ַΪ0201�ĵ�ǰ��ֵ 
( 
��·��ַ: 1      ASDU 140      COT: 20  һ������ 
Ӧ�÷������ݵ�Ԫ������ַ: 1 
) 
����: 10 20 01 21 16   װ�û�Ӧȷ��֡ 
����: 10 7A 01 7B 16   ����һ������ 
����: 10 20 01 21 16   װ�û�Ӧȷ��֡ 
����: 10 5A 01 5B 16   ����һ������ 
����: 68 0E 0E 68 08 01 11 81 45 01 01 02 C8 00 50 E2 B8 14 AA 16     ��ַΪ0201�Ķ�ֵΪC8   (C8=200    װ���ڲ��趨Ϊ2.00) 
( 
��·��ַ: 1      ASDU 17      COT: 69   
Ӧ�÷������ݵ�Ԫ������ַ: 1 
) 
0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 
68 0E 0E 68 08 01 11 81 45 01 01 02 C8 00 50 E2 B8 14 AA 16
*/
						OpcUa_Int32 iTagAddr = 0;	//��ǩ��ַ = Fun*1000+Inf
						bFunType = pPacketBuffer[10];
						InfoNo = pPacketBuffer[11];
						bAINum = (pPacketBuffer[7]&0x7F);

						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)�����ʱ�����ʱ��ı���ֵ����ASDU17(Fun:%d,Info:%d,��Ϣ����%d)!"
							, pDevice->GetAddr(),bFunType,InfoNo,InfoSum);
						pDrv->WriteLog(chMsg);

						MEA_AI_ASDU17* pVal = (MEA_AI_ASDU17* )(pPacketBuffer+12);
						for ( OpcUa_Int32 i = 0; i < bAINum; i++ )
						{
							iTagAddr = GetTagAddr( bFunType, InfoNo );
							OpcUa_Float fVal = pVal->Value;
							pDevice->SetVal( MacsDIS103_AIB, iTagAddr, (OpcUa_Byte* )&fVal, sizeof(OpcUa_Float),0, bFromLocal );
							pVal++;
							InfoNo++;
						}
					}
					break;
				case TYP_ASDU171:			//ʩ�͵±����й���ȼ��޹����
					{
						/*
						///////////////////////////////
						�й���ȣ�
						68 08 08 68 73 01 8C 81 00 01 41 08 CB 16     //ASDU140: ��ʩ�͵±�����ֵ�鱨�����ͱ�ʶ,COT = 0, AI65:08
						E5

						10 5A 01 5B 16
						E5

						10 7B 01 7C 16
						10 29 01 2A 2A 16

						10 5A 01 5B 16
						//�й���ȣ�3.50 MWh = 20 20 20 20 20 20 20 33 2E 35 30 20 4D 57 68��ASCII�룩
						//ASDU171: AI65:08 = 20 20 20 20 20 20 20 33 2E 35 30 20 4D 57 68 20 20 20 20 20 83 31 B2 0C
						68 20 20 68 08 01 AB 81 08 01 41 08 20 20 20 20 20 20 20 33 2E 35 30 20 4D 57 68 20 20 20 20 20 83 31 B2 0C 6B 16

						10 7A 01 7B 16
						E5


						/////////////////////////////////////////
						�޹���ȣ�
						68 08 08 68 73 01 8C 81 00 01 43 08 CD 16    //ASDU140: ��ʩ�͵±�����ֵ�鱨�����ͱ�ʶ,COT = 0, AI67:08
						E5  

						10 5A 01 5B 16  
						E5  

						10 7B 01 7C 16  
						10 29 01 2A 16  

						10 5A 01 5B 16  
						//ASDU171: AI67:08  = 0.29 Mvarh  = 20 20 20  20 20 20  20 30 2E 32 39 20 4D 76 61  72 68��ASCII�룩
						68 20 20 68 08 01 AB 81 08 01 43 08 20 20 20 20 20 20 20 30 2E 32 39 20 4D 76 61 72 68 20 20 20 A7 0E B7 0C 28 16
						*/
						OpcUa_Int32 iTagAddr = 0;	//��ǩ��ַ = Fun*1000+Inf
						bFunType = pPacketBuffer[10];
						InfoNo = pPacketBuffer[11];
						bAINum = (pPacketBuffer[7]&0x7F);

						snprintf(chMsg,sizeof(chMsg)
							,"�豸(��ַ%d)ʩ�͵±������ASDU171(Fun:%d,Info:%d,��Ϣ����%d)!"
							, pDevice->GetAddr(),bFunType,InfoNo,InfoSum);
						pDrv->WriteLog(chMsg);

						if ( nPacketLength >= 15 )
						{
							OpcUa_CharA pchTemp[255];
							ACE_OS::memset( pchTemp, 0, sizeof(pchTemp) );
							ACE_OS::memcpy( pchTemp, pPacketBuffer+12, nPacketLength-14 );

							iTagAddr = GetTagAddr( bFunType, InfoNo );
							OpcUa_Float fVal = (OpcUa_Float)::atof( pchTemp );
							pDevice->SetVal( MacsDIS103_AIB, iTagAddr, (OpcUa_Byte* )&fVal, sizeof(OpcUa_Float), 0, bFromLocal );
						}
					}

					break;
				case TYP_COMM_DATA:			//ͨ�÷������ݱ������ͱ�ʶ
					{
						OpcUa_Int32 iRestByteCount = nPacketLength - sizeof(FT1_2_HEAD) - sizeof(FT1_2_TAIL);	//ʣ���ֽڸ����������治�ϼ���
						ASDU10_HEAD_STRUCT* pASDUHead = (ASDU10_HEAD_STRUCT* )(pPacketBuffer+sizeof(FT1_2_HEAD));
						OpcUa_Byte* pASDUStart = pPacketBuffer+sizeof(FT1_2_HEAD);
						OpcUa_Int32 iTagAddr = 0;	//��ǩ��ַ = GROUP*1000+ENTRY
						if ( pASDUHead->NGD > 0 )	//������
						{
							OpcUa_Int32 i=0;
							OpcUa_Int32 j,k;
							OpcUa_Byte byValue;
							OpcUa_Int32 iValue;
							OpcUa_Float fValue;
							OpcUa_Int32 iByteCount = sizeof(ASDU10_HEAD_STRUCT);	//����ĳ��ȣ������治���ۼӼ���ó���
							OpcUa_Int32 ioneGIDSize=0;
							OpcUa_Byte* pGDPos = pASDUStart + sizeof(ASDU10_HEAD_STRUCT) ;		//ͨ�÷�����������λ��
							GDATAPointHead* pDGHead = (GDATAPointHead* )pGDPos;	//ͨ�÷���������Head

							//����COT����COT��Ϊ����ͬʱ������ش���: 
							//1.ϵͳ��Ϣ----�����豸��Ϊ��Ҫ,ִ��Уʱ,���ٻ�. 
							//2.�϶���Ӧ----����ӦGroup��Entry��DIC��ǩ��Ϊ1
							//3.����Ӧ----����ӦGroup��Entry��DIC��ǩ��Ϊ0
							//ע��, Group��Entry����Ӧ��������Ӧֵ���µ�����DI��AI.
							if ( pASDUHead->NGD == 1 )
							{
								ASDU10NoDataSet* pASDU = (ASDU10NoDataSet* )pASDUStart;

								iTagAddr = GetTagAddr(pASDU->Gin.Group,pASDU->Gin.Entry );

								switch ( COT )
								{
								case COT_CU:						//��λͨ�ŵ�Ԫ
								case COT_START:						//����/��������
								case COT_POWERON:					//��Դ����
									{
										//�����豸���ٻ�
										ActiveWholeCall( pDevice );

										if ( m_bBroadCheckTime )
										{
											m_bIsTimeForBroadCheckTime = true;
										}
										else
											pDevice->m_bNeedCheckTime = true;
									}
									break;
								case COT_GENERAL_CMD_CONFIRM:		//����Ŀ϶��Ͽ�
								case COT_COMM_WRITE_CMD_CONFIRM:	//ͨ�÷���д����Ŀ϶��Ͽ�
								case COT_COMM_READ_CMD_VALID:		//��ͨ�÷����������Ч������Ӧ
								case COT_COMM_WRITE_CONFIRM:		//ͨ�÷���дȷ��
									{
										byValue = 1;
										pDevice->SetVal( MacsDIS103_DICB, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(BYTE),0, bFromLocal );
									}
									break;
								case COT_GENERAL_CMD_DENY:			//����ķ��Ͽ�
								case COT_COMM_WRITE_CMD_DENY:		//ͨ�÷���д����ķ��Ͽ�
								case COT_COMM_READ_CMD_INVALID:		//��ͨ�÷����������Ч������Ӧ
									{
										byValue = 0;
										pDevice->SetVal( MacsDIS103_DICB, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(BYTE),0, bFromLocal );
									}
									break;
								default:
									break;
								}
							}

							//����ͨ�÷������ݡ�ע�⣬�����������Ϳ��ܻ᲻��ͬ��
							for ( i = 0; i < pASDUHead->NGD; i++ )
							{
								if ( pDGHead->sGDD.DataType == GDD_DT0_NODATA )
								{

									if ( iRestByteCount >= sizeof(GDATAPointNoData) )
									{
										//����ǰ���ݵ�
										iTagAddr = GetTagAddr( pDGHead->sGIN.Group, pDGHead->sGIN.Entry );


										//������һ���ݵ�
										iRestByteCount -= sizeof(GDATAPointNoData);
										iByteCount += sizeof(GDATAPointNoData);
										pGDPos += sizeof(GDATAPointNoData);
										pDGHead = (GDATAPointHead* )pGDPos;
									}
									else
									{
										pDrv->WriteLog( "ʣ�೤�Ȳ���!" );
										break;
									}
								}
								else 
								{
									ioneGIDSize = pDGHead->sGDD.DataSize*pDGHead->sGDD.Number;
									if ( iRestByteCount >= sizeof(GDATAPointHead)+ioneGIDSize )
									{
										//����ǰ���ݵ�
										if ( pDGHead->KOD == KOD1_ACTUALVALUE )	//GDD�а�������ʱ,ֻ����"ʵ������")
										{
											OpcUa_Byte* pCur = pASDUStart + iByteCount + sizeof(GDATAPointHead);
											switch ( pDGHead->sGDD.DataType )
											{
											case GDD_DT1_OS8ASCII:	//ע�⣺����UI��ͬ��ֵ�ϳɷ�����ÿ�����ݵĳ��ȳ���4�ֽ�ʱ��ʧ��
											case GDD_DT2_BS1:
											case GDD_DT3_UI:
											case GDD_DT4_I:
											case GDD_DT22_ResponseCode:
											case GDD_DT24_Index:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{//������û�п����޷���
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													//�ϳ���ֵ. ע��, ������ȳ���4�ֽ�,��ֵ��ʧ��.
													iValue = 0;
		/*
													for( k=pDGHead->sGDD.DataSize-1; k>=0; k-- )
													{

														iValue += OpcUa_Int32( pCur[k] * pow( 0x100, pDGHead->sGDD.DataSize-1-k ) );
													}
		*/
													for( k = 0; k < pDGHead->sGDD.DataSize; k++ )
													{

														iValue += OpcUa_Int32( pCur[k] * pow( 256.0, k ) );
													}

													fValue = (OpcUa_Float)iValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT5_UFLOAT:	//��û���ر���
											case GDD_DT6_FLOAT:
											case GDD_DT7_FLOAT:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													//�ϳ���ֵ
													fValue = *(OpcUa_Float* )pCur;	//��֤�Ƿ�ֵת����ȷ������������ת��
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT8_DOUBLE:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													//�ϳ���ֵ
													fValue = (OpcUa_Float)(*(double* )pCur);	
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT9_DPI:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													byValue = *pCur;	

													fValue = (OpcUa_Float)byValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													if ( byValue == 1 || byValue == 2 )
													{
														byValue -= 1; 
														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT10_SPI:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													byValue = *pCur;	
													if ( byValue == 0 || byValue == 1 )
													{
														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT11_DPI:	//���ﲻ�ܺ���0��3.
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													byValue = *pCur;	
													if ( byValue <= 3 )
													{
														fValue = (OpcUa_Float)byValue;	
														pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT12_AITime:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													//�ϳ���ֵ
													MEA_AI* pData = (MEA_AI* )pCur;
													if ( pData->ER == 0 && pData->OV == 0 )
													{
														fValue = (OpcUa_Float )pData->Value;	
														pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT18_DPITime:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													CP48* pPoint = (CP48* )pCur;
													byValue = pPoint->DPI;	

													fValue = (OpcUa_Float)byValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													if ( byValue == 1 || byValue == 2 )
													{
														byValue -= 1; 

														//����SOE
														if ( pPoint->Time.IV == 1 )			//ʱ����Ч
														{
															pDevice->m_bNeedCheckTime = true;												
															snprintf(chMsg,sizeof(chMsg)
																,"CP48:�豸(��ַ%d)DI��λSOE ������IV=1, ASDU10,GDD_DataType=%d, ��ʾʱ����Ч, ��Ҫ��ʱ!", pDevice->GetAddr(), pDGHead->sGDD.DataType);
															pDrv->WriteLog(chMsg);
														}
														else if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP48:�豸(��ַ%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
															pDrv->WriteLog(chMsg);

															CP32Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															//! ����ʱ��
															DAILYTIME strTime;
															strTime.lYear    = dtime.year();
															strTime.sMonth   = dtime.month();
															strTime.sDay     = dtime.day();
															strTime.sHour    = pTime->Hour;
															strTime.sMinute  = pTime->Minute;
															strTime.sSecond  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															strTime.sMSecond = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;

															OpcUa_Int64 nTime = CaculateMSNum( strTime );

// 															CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIG,
// 																iTagAddr, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)byValue, nTime);		//! ����SOE�� hexuewen note������ʵ�����������ģ����ܵ��������ղ�һ�¡�
														}
														else
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP48:�豸(��ַ%d)DI��λSOE,ASDU10,GDD_DataType=%d, ������ʱ����Ч, ��Ϊ��ʱ��!",pDevice->GetAddr(), pDGHead->sGDD.DataType );
															pDrv->WriteLog(chMsg);
														}										

														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT19_DPITime:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													CP80* pPoint = (CP80* )pCur;
													byValue = pPoint->DPI;	

													fValue = (OpcUa_Float)byValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													if ( byValue == 1 || byValue == 2 )
													{
														byValue -= 1; 

														//����SOE
														if ( pPoint->Time.IV == 1 )			//ʱ����Ч
														{
															pDevice->m_bNeedCheckTime = true;												
															snprintf(chMsg,sizeof(chMsg)
																,"CP80:�豸(��ַ%d)DI��λSOE ������IV=1, ASDU10,GDD_DataType=%d, ��ʾʱ����Ч, ��Ҫ��ʱ!", pDevice->GetAddr(), pDGHead->sGDD.DataType);
															pDrv->WriteLog(chMsg);
														}
														else if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP80:�豸(��ַ%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
															pDrv->WriteLog(chMsg);

		/*
															CP32Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															struct tm structTime;
															structTime.tm_year = dtime.year();
															structTime.tm_mon  = dtime.month();
															structTime.tm_mday = dtime.day();

															structTime.tm_hour = pTime->Hour;
															structTime.tm_min  = pTime->Minute;
															structTime.tm_sec  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															OpcUa_Int64 nTime   = (OpcUa_Int64)mktime(&structTime);
															nTime = nTime*1000 + MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;
		*/

															CP32Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															//! ����ʱ��
															DAILYTIME strTime;
															strTime.lYear    = dtime.year();
															strTime.sMonth   = dtime.month();
															strTime.sDay     = dtime.day();
															strTime.sHour    = pTime->Hour;
															strTime.sMinute  = pTime->Minute;
															strTime.sSecond  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															strTime.sMSecond = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;

															OpcUa_Int64 nTime = CaculateMSNum( strTime );

 															//CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIG,
 															//	iTagAddr, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)byValue, nTime);		//! ����SOE�� hexuewen note������ʵ�����������ģ����ܵ��������ղ�һ�¡�
														}
														else
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP80:�豸(��ַ%d)DI��λSOE,ASDU10,GDD_DataType=%d, ������ʱ����Ч, ��Ϊ��ʱ��!",pDevice->GetAddr(), pDGHead->sGDD.DataType );
															pDrv->WriteLog(chMsg);
														}										

														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT203_CP72:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													CP72* pPoint = (CP72* )pCur;
													byValue = pPoint->DPI;

													fValue = (OpcUa_Float)byValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													if ( byValue == 1 || byValue == 2 )
													{
														byValue -= 1; 

														//����SOE
														if ( pPoint->Time.IV == 1 )			//ʱ����Ч
														{
															pDevice->m_bNeedCheckTime = true;												
															snprintf(chMsg,sizeof(chMsg)
																,"CP72:�豸(��ַ%d)DI��λSOE ������IV=1, ASDU10,GDD_DataType=%d, ��ʾʱ����Ч, ��Ҫ��ʱ!", pDevice->GetAddr(), pDGHead->sGDD.DataType);
															pDrv->WriteLog(chMsg);
														}
														else if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP72:�豸(��ַ%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
															pDrv->WriteLog(chMsg);

		/*
															CP56Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															struct tm structTime;
															structTime.tm_year = dtime.year();
															structTime.tm_mon  = dtime.month();
															structTime.tm_mday = dtime.day();

															structTime.tm_year = pTime->Year + (dtime.year()/100)*100;
															structTime.tm_mon  = pTime->Mon;
															structTime.tm_mday = pTime->Day;
															structTime.tm_hour = pTime->Hour;
															structTime.tm_min  = pTime->Minute;
															structTime.tm_sec  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															OpcUa_Int64 nTime   = (OpcUa_Int64)mktime(&structTime);
															nTime = nTime*1000 + MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;
		*/

															CP56Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															//! ����ʱ��
															DAILYTIME strTime;
															strTime.lYear    = pTime->Year + (dtime.year()/100)*100;
															strTime.sMonth   = pTime->Mon;
															strTime.sDay     = pTime->Day;
															strTime.sHour    = pTime->Hour;
															strTime.sMinute  = pTime->Minute;
															strTime.sSecond  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															strTime.sMSecond = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;

															OpcUa_Int64 nTime = CaculateMSNum( strTime );

// 															CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIG,
// 																iTagAddr, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)byValue, nTime);		//! ����SOE�� hexuewen note������ʵ�����������ģ����ܵ��������ղ�һ�¡�
														}
														else
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP72:�豸(��ַ%d)DI��λSOE,ASDU10,GDD_DataType=%d, ������ʱ����Ч, ��Ϊ��ʱ��!",pDevice->GetAddr(), pDGHead->sGDD.DataType );
															pDrv->WriteLog(chMsg);
														}										

														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											case GDD_DT204_CP104:
												for( j = 0; j < pDGHead->sGDD.Number; j++ )
												{
													iTagAddr = GetTagAddr(pDGHead->sGIN.Group, pDGHead->sGIN.Entry+j);

													CP104* pPoint = (CP104* )pCur;
													byValue = pPoint->DPI;

													fValue = (OpcUa_Float)byValue;
													pDevice->SetVal( MacsDIS103_AIG, iTagAddr, (OpcUa_Byte* )&fValue,sizeof(OpcUa_Float),0, bFromLocal );

													if ( byValue == 1 || byValue == 2 )
													{
														byValue -= 1; 

														//����SOE
														if ( pPoint->Time.IV == 1 )			//ʱ����Ч
														{
															pDevice->m_bNeedCheckTime = true;												
															snprintf(chMsg,sizeof(chMsg)
																,"CP104:�豸(��ַ%d)DI��λSOE ������IV=1, ASDU10,GDD_DataType=%d, ��ʾʱ����Ч, ��Ҫ��ʱ!", pDevice->GetAddr(), pDGHead->sGDD.DataType);
															pDrv->WriteLog(chMsg);
														}
														else if ( pPoint->Time.SU == 0 )	//ʱ����Ч, �Ҳ�Ϊ��ʱ��
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP104:�豸(��ַ%d)DI, ASDU%d, SOE!", pDevice->GetAddr(), 1);
															pDrv->WriteLog(chMsg);

		/*
															CP56Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															struct tm structTime;
															structTime.tm_year = dtime.year();
															structTime.tm_mon  = dtime.month();
															structTime.tm_mday = dtime.day();

															structTime.tm_year = pTime->Year + (dtime.year()/100)*100;
															structTime.tm_mon  = pTime->Mon;
															structTime.tm_mday = pTime->Day;
															structTime.tm_hour = pTime->Hour;
															structTime.tm_min  = pTime->Minute;
															structTime.tm_sec  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															OpcUa_Int64 nTime   = (OpcUa_Int64)mktime(&structTime);
															nTime = nTime*1000 + MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;
		*/

															CP56Time2a *pTime = &pPoint->Time;
															ACE_Time_Value val = ACE_OS::gettimeofday();	
															ACE_Date_Time dtime;
															dtime.update( val );

															//! ����ʱ��
															DAILYTIME strTime;
															strTime.lYear    = pTime->Year + (dtime.year()/100)*100;
															strTime.sMonth   = pTime->Mon;
															strTime.sDay     = pTime->Day;
															strTime.sHour    = pTime->Hour;
															strTime.sMinute  = pTime->Minute;
															strTime.sSecond  = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)/1000;
															strTime.sMSecond = MAKEWORD(pTime->LowMillSec,pTime->HighMillSec)%1000;

															OpcUa_Int64 nTime = CaculateMSNum( strTime );

// 															CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIG,
// 																iTagAddr, IOEVENTTYPE_OUTER_SOE , (OpcUa_Int32)byValue, nTime);		//! ����SOE�� hexuewen note������ʵ�����������ģ����ܵ��������ղ�һ�¡�
														}
														else
														{											
															snprintf(chMsg,sizeof(chMsg)
																,"CP104:�豸(��ַ%d)DI��λSOE,ASDU10,GDD_DataType=%d, ������ʱ����Ч, ��Ϊ��ʱ��!",pDevice->GetAddr(), pDGHead->sGDD.DataType );
															pDrv->WriteLog(chMsg);
														}										

														pDevice->SetVal( MacsDIS103_DIG, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(byValue), 0, bFromLocal );
													}

													pCur += pDGHead->sGDD.DataSize;
												}
												break;
											default:
												break;
											}
										}
										else
										{
											snprintf(chMsg,sizeof(chMsg)
												,"�豸(��ַ%d)ASDU10,���%d,��Ŀ��%d��KOD%dΪ��ʵ��ֵ(1) !.", pDevice->GetAddr(), pDGHead->sGIN.Group, pDGHead->sGIN.Entry, pDGHead->KOD);

											pDrv->WriteLog( chMsg );
										}

										//������һ���ݵ�
										iRestByteCount -= sizeof(GDATAPointHead)+ioneGIDSize;
										iByteCount += sizeof(GDATAPointHead)+ioneGIDSize;
										pGDPos += sizeof(GDATAPointHead)+ioneGIDSize;
										pDGHead = (GDATAPointHead* )pGDPos;
									}
									else
									{
										pDrv->WriteLog( "ʣ�೤�Ȳ���!" );
										break;
									}
								}
							}
						}
						else	//������
						{
							if ( nPacketLength == sizeof(ASDU10NoDataSet) )
							{
								ASDU10NoDataSet* pASDU = (ASDU10NoDataSet* )pASDUStart;

								iTagAddr = GetTagAddr(pASDU->Gin.Group,pASDU->Gin.Entry );

								//���յ������ݵ�ASDU10Ӧ��ʱ,��COTֵд����ź���Ŀ������COT��ǩ.
								BYTE byValue = pASDUHead->COT;
								pDevice->SetVal( MacsDIS103_COT, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(BYTE),0, bFromLocal );

								//����COT����COT��Ϊ����ͬʱ������ش���: 
								//1.ϵͳ��Ϣ----�����豸��Ϊ��Ҫ,ִ��Уʱ,���ٻ�. 
								//2.�϶���Ӧ----����ӦGroup��Entry��DIC��ǩ��Ϊ1
								//3.����Ӧ----����ӦGroup��Entry��DIC��ǩ��Ϊ0
								//ע��, Group��Entry����Ӧ��������Ӧֵ���µ�����DI��AI.
								switch ( COT )
								{
								case COT_CU:						//��λͨ�ŵ�Ԫ
								case COT_START:						//����/��������
								case COT_POWERON:					//��Դ����
									{
										//�����豸���ٻ�
										ActiveWholeCall( pDevice );

										if ( m_bBroadCheckTime )
										{
											m_bIsTimeForBroadCheckTime = true;
										}
										else
											pDevice->m_bNeedCheckTime = true;
									}
									break;
								case COT_GENERAL_CMD_CONFIRM:		//����Ŀ϶��Ͽ�
								case COT_COMM_WRITE_CMD_CONFIRM:	//ͨ�÷���д����Ŀ϶��Ͽ�
								case COT_COMM_READ_CMD_VALID:		//��ͨ�÷����������Ч������Ӧ
								case COT_COMM_WRITE_CONFIRM:		//ͨ�÷���дȷ��
									{
										byValue = 1;
										pDevice->SetVal( MacsDIS103_DICB, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(BYTE),0, bFromLocal );
									}
									break;
								case COT_GENERAL_CMD_DENY:			//����ķ��Ͽ�
								case COT_COMM_WRITE_CMD_DENY:		//ͨ�÷���д����ķ��Ͽ�
								case COT_COMM_READ_CMD_INVALID:		//��ͨ�÷����������Ч������Ӧ
									{
										byValue = 0;
										pDevice->SetVal( MacsDIS103_DICB, iTagAddr, (OpcUa_Byte* )&byValue,sizeof(BYTE),0, bFromLocal );
									}
									break;
								default:
									break;
								}
							}
						}
					}
					break;
				case 205:			//�������Ĳɼ�
					//��ʵ��
					break;
				default:					
					snprintf(chMsg,sizeof(chMsg)
						,"�豸(��ַ%d)�յ�δ֪ASDU%d!"
						, pDevice->GetAddr(), pPacketBuffer[6]);
					pDrv->WriteLog( chMsg );
					
					break;
				}
			}
			break;
		default:
			break;
		}


		pDrv->WriteLog("IEC103  CFepIEC103Prtcl::ProcessRcvdPacket����������֡��������һ������");

		//! �����ӳ�
		ACE_Time_Value tv( m_nSendDelay/1000, (m_nSendDelay%1000)*1000 );
		ACE_OS::sleep(tv);

		OpcUa_Int32 iReturnLen = 0;
		BuildRequest( m_pbySndBuffer, iReturnLen );

		//! ������ʱ��
		if ( m_pTimeOutTimer->SetTimer( m_nRcvTimeOut ) == 0 )
		{
			snprintf( chMsg, sizeof(chMsg), 
				"CFepIEC103Prtcl::ProcessRcvdPacket Master  SetTimer Success,TimerID = %d....", m_pTimeOutTimer->GetTimerID() );
			pDrv->WriteLog(chMsg);
		}
		else
		{
			snprintf( chMsg, sizeof(chMsg), 
				"CFepIEC103Prtcl::ProcessRcvdPacket Master  SetTimer Failed,TimerID = %d!!!!!", m_pTimeOutTimer->GetTimerID() );
			pDrv->WriteLog(chMsg);
		}
	}


	/*!
	*	\brief	���·�����֡	
	*
	*	\param	����	OpcUa_Byte* pFrame: ֡������	 
	*
	*	\retval	����ֵ	OpcUa_Int32 : ֡����
	*
	*	\note		ע�������ѡ��
	*/
	OpcUa_Int32 CFepIEC103Prtcl::BuildCtlRequest( OpcUa_Byte* sndBuf ,OpcUa_Int32& iDataLen)
	{
		if ( !sndBuf )
			return 0;
		OpcUa_CharA chMsg[256];		

		CFepIEC103Drv* pDriver = (CFepIEC103Drv*)m_pParam;
		OpcUa_Int32 iResult = 0;
		OpcUa_Byte pchBuf[512];
		OpcUa_Int32 nBufLen = 512;		
		iResult = pDriver->m_IORemoteCtrl.DeQueue( (OpcUa_CharA*)pchBuf, nBufLen );
		if ( iResult == 0 )
		{
			std::string strLog;
			pDriver->WriteLogAnyWay("���·�����֡... ...");
			
			memcpy( (OpcUa_Byte*)(&m_tCtrlRemoteCmd), pchBuf, sizeof(m_tCtrlRemoteCmd) );
			CFepIEC103Device* pDevice = dynamic_cast<CFepIEC103Device*>(pDriver->GetDeviceByName(m_tCtrlRemoteCmd.pchUnitName));
			assert(pDevice);
			OpcUa_Int32 nLength = 0;

			switch ( m_tCtrlRemoteCmd.byUnitType )
			{
			case MacsDIS103_DOS:	//ң��ѡ��
				{
					if (pDevice->IsOnLine())		//�豸���ã�����Ӧ��DOA��1
					{
						if (1 == m_tCtrlRemoteCmd.byData[0])			//�յ�DOM��1�����Ժ��Ƚ��ϴε�DOA��DOC��0
						{
							OpcUa_Byte byTemp = 0;
							pDevice->m_iInDOMIndex = m_tCtrlRemoteCmd.lTagAddress;

							pDevice->SetVal(MacsDIS103_DIS, m_tCtrlRemoteCmd.lTagAddress, &(pDevice->m_DOAs[pDevice->m_iInDOMIndex]  = 1),1,0, true);
							if (pDevice->TagNeedSOE(MacsDIS103_DO, m_tCtrlRemoteCmd.lTagAddress) 
								|| pDevice->TagNeedDevSOE(MacsDIS103_DI, m_tCtrlRemoteCmd.lTagAddress))
							{						
// 								CIOFrameAccess::PostSOE( pDevice->GetName().c_str(), MacsDIS103_DIS,
// 									m_tCtrlRemoteCmd.lTagAddress, IOEVENTTYPE_OUTER_SOE , pDevice->m_DOAs[pDevice->m_iInDOMIndex], 0);		//! ����SOE
							}						
							snprintf(chMsg,sizeof(chMsg)
								,"ң��ѡ������DOS(�豸��:%s,�豸��ַ:%d),ֵ(%d)��Ч!"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);
						}
						else
						{				
							snprintf(chMsg,sizeof(chMsg)
								,"ң��ѡ������DOS(�豸��:%s,�豸��ַ:%d),��ǩ��ַ:%d),ֵ(%d)��Ч!"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);

							return 0;
						}					
					}
					else							//�豸������
					{				
						snprintf(chMsg,sizeof(chMsg)
							,"ң��ѡ������DOS��Ч(�豸��:%s,�豸��ַ:%d),�豸����!"
							,pDevice->GetName().c_str(),pDevice->GetAddr());
						pDriver->WriteLogAnyWay(chMsg);

						return 0;
					}
				}
				break;
			case MacsDIS103_DO:		//ң��ִ��
				{
					SOEMAPiterator it;
					it = pDevice->m_DOWithSel.find( m_tCtrlRemoteCmd.lTagAddress );
					if ( it == pDevice->m_DOWithSel.end())
						return 0;

					if (0 == it->second)
						pDevice->m_pDOCTimer->KillTimer();

					pDevice->m_iInDOIndex = m_tCtrlRemoteCmd.lTagAddress;	
					if ( it->second )
					{
						if (pDevice->m_DOAs[m_tCtrlRemoteCmd.lTagAddress]  == 1)
						{
							pDevice->SetVal(MacsDIS103_DIS, m_tCtrlRemoteCmd.lTagAddress, &(pDevice->m_DOAs[m_tCtrlRemoteCmd.lTagAddress]  = 0),1,0,true);
							nLength = pDevice->BuildDOFrame(sndBuf, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);					
							OpcUa_Byte bytemp = 1;
							pDevice->SetVal(MacsDIS103_DO,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);
							snprintf(chMsg,sizeof(chMsg)
								,"ң��ִ������DO(�豸��:%s,�豸��ַ:%d),��ǩ��ַ:%d),ֵ(%d)��Ч!"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);

							//��ʱ30s����ң�ط��Ÿ�λ
							pDevice->m_pDOCTimer->SetTimer(m_nDORelayTime);
						}
						else
						{					
							snprintf(chMsg,sizeof(chMsg)
								,"ң��ִ������DO(�豸��:%s,�豸��ַ:%d,��ǩ��ַ:%d),ֵ(%d)��Ч!---����ִ��ң��ѡ��"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);
							
							return 0;
						}

					}			
					else
					{					
						iDataLen = pDevice->BuildDOFrame(sndBuf,m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						OpcUa_Byte bytemp = m_tCtrlRemoteCmd.byData[0];
						pDevice->SetVal(MacsDIS103_DO,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);
					}

					pDevice->m_bInDO = true;

					snprintf(chMsg,sizeof(chMsg)
						,"ң������DO: %s"
						,MakeLog(sndBuf, nLength));
					pDriver->WriteLogAnyWay( chMsg );		
				}
				break;
			case MacsDIS103_DIG://ң��--Group:Entry
				{
					OpcUa_Byte bytemp = m_tCtrlRemoteCmd.byData[0];
					pDevice->SetVal(MacsDIS103_DIG,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"ң�ŵ�DIGֵ���ϲ��޸ģ�(�豸��:%s,�豸��ַ:%d,��ǩ��ַ:%d),ֵ(%d)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_AIG://ң��--Group:Entry
				{
					OpcUa_Float bytemp = *((OpcUa_Float* )m_tCtrlRemoteCmd.byData);
					pDevice->SetVal(MacsDIS103_AIG,m_tCtrlRemoteCmd.lTagAddress,(OpcUa_Byte* )&bytemp,sizeof(OpcUa_Float),0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"ң���AIGֵ���ϲ��޸ģ�(�豸��:%s,�豸��ַ:%d,��ǩ��ַ:%d),ֵ(%f)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,bytemp);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_DIB://ң��--Fun:Inf
				{
					OpcUa_Byte bytemp = m_tCtrlRemoteCmd.byData[0];
					pDevice->SetVal(MacsDIS103_DIB,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"ң�ŵ�DIBֵ���ϲ��޸ģ�(�豸��:%s,�豸��ַ:%d,��ǩ��ַ:%d),ֵ(%d)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_AIB://ң��--Fun:Inf
				{
					OpcUa_Float bytemp = *((OpcUa_Float* )m_tCtrlRemoteCmd.byData);
					pDevice->SetVal(MacsDIS103_AIB,m_tCtrlRemoteCmd.lTagAddress,(OpcUa_Byte* )&bytemp,sizeof(OpcUa_Float),0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"ң���AIBֵ���ϲ��޸ģ�(�豸��:%s,�豸��ַ:%d,��ǩ��ַ:%d),ֵ(%f)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,bytemp);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_DICB://�·����У--Fun:In
				{
					OpcUa_Byte bytemp = m_tCtrlRemoteCmd.byData[0];
					pDevice->SetVal(MacsDIS103_DICB,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"ң��ִ�з�У��DICBֵ���ϲ��޸ģ�(�豸��:%s,�豸��ַ:%d,��ǩ��ַ:%d),ֵ(%d)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_COT://������ASDU10Ӧ���COT
				{
					OpcUa_Byte bytemp = m_tCtrlRemoteCmd.byData[0];
					pDevice->SetVal(MacsDIS103_COT,m_tCtrlRemoteCmd.lTagAddress,&bytemp,1,0,true);

					snprintf(chMsg,sizeof(chMsg)
						,"������ASDU10Ӧ���COTֵ���ϲ��޸ģ�(�豸��:%s,�豸��ַ:%d,��ǩ��ַ:%d),ֵ(%d)!"
						,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress,m_tCtrlRemoteCmd.byData[0]);
					pDriver->WriteLogAnyWay(chMsg);
				}
				break;
			case MacsDIS103_DOB://ң��ִ������--Fun:Inf
				{
					if( 0 != m_tCtrlRemoteCmd.byData[0] && 1 != m_tCtrlRemoteCmd.byData[0] )
					{				
						snprintf(chMsg,sizeof(chMsg)
							,"ң��ִ������DOB(�豸��:%s,�豸��ַ:%d),��ǩ����:%d,��ǩ��ַ:%d),ֵ(%d)��Ч!"
							,pDevice->GetName().c_str(), pDevice->GetAddr(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						pDriver->WriteLogAnyWay(chMsg);

						return 0;
					}	

					OpcUa_Byte byValue =0;
					pDevice->SetVal(MacsDIS103_DICB, m_tCtrlRemoteCmd.lTagAddress,&byValue,1,0,true);

					iDataLen = pDevice->BuildDOBFrame(sndBuf, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);					
					if ( iDataLen > 0 )
					{
						snprintf(chMsg,sizeof(chMsg)
							,"����ң��ִ������DOB(�豸��:%s,�豸��ַ:%d),��ǩ����:%d,��ǩ��ַ:%d),ֵ(%d): "
							,pDevice->GetName().c_str(), pDevice->GetAddr(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						pDriver->WriteLogAnyWay(chMsg);
						pDriver->WriteCommDataToLog( sndBuf, iDataLen,1 );
					}
				}
				break;
			case MacsDIS103_DOSB://ң��ѡ������--Fun:Inf
				{
					if (pDevice->IsOnLine())		//�豸���ã�����Ӧ��DOA��1
					{
						if (1 == m_tCtrlRemoteCmd.byData[0])			//�յ�ң��ѡ�������Ժ�ֱ����DICBΪ1����ʾѡ��ɹ�����ѡ���豸
						{
							OpcUa_Byte byValue =1;
							pDevice->SetVal(MacsDIS103_DICB, m_tCtrlRemoteCmd.lTagAddress,&byValue,1,0,true);

							snprintf(chMsg,sizeof(chMsg)
								,"ң��ѡ������DOSB(�豸��:%s,�豸��ַ:%d),ֵ(%d)��Ч!"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);
						}
						else
						{				
							snprintf(chMsg,sizeof(chMsg)
								,"ң��ѡ������DOSB(�豸��:%s,�豸��ַ:%d),��ǩ��ַ:%d),ֵ(%d)��Ч!"
								,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
							pDriver->WriteLogAnyWay(chMsg);

							return 0;
						}					
					}
					else							//�豸������
					{				
						snprintf(chMsg,sizeof(chMsg)
							,"ң��ѡ������DOSB��Ч(�豸��:%s,�豸��ַ:%d),�豸����!"
							,pDevice->GetName().c_str(),pDevice->GetAddr());
						pDriver->WriteLogAnyWay(chMsg);

						return 0;
					}
				}
				break;
			case MacsDIS103_RdFV://����ֵ--Group:Entry: ASDU140
				{
					OpcUa_Byte byValue =0;
					pDevice->SetVal(MacsDIS103_DICB, m_tCtrlRemoteCmd.lTagAddress,&byValue,1,0,true);

					iDataLen = pDevice->BuildASDU140Frame(sndBuf, m_tCtrlRemoteCmd.lTagAddress );					
					if ( iDataLen > 0 )
					{
						snprintf(chMsg,sizeof(chMsg)
							,"���ɶ���ֵ����RdFV(�豸��:%s,�豸��ַ:%d),��ǩ����:%d,��ǩ��ַ:%d),ֵ(%d): "
							,pDevice->GetName().c_str(), pDevice->GetAddr(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						pDriver->WriteLogAnyWay(chMsg);
						pDriver->WriteCommDataToLog( sndBuf, iDataLen,1 );
					}
				}
				break;
			case MacsDIS103_WrFV://д��ֵ--Group:Entry: ASDU232
				{
					OpcUa_Byte byValue =0;
					pDevice->SetVal(MacsDIS103_DICB, m_tCtrlRemoteCmd.lTagAddress,&byValue,1,0,true);

					iDataLen = pDevice->BuildASDU232Frame(sndBuf, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0] );					
					if ( iDataLen > 0 )
					{
						snprintf(chMsg,sizeof(chMsg)
							,"����д��ֵ����WrFV(�豸��:%s,�豸��ַ:%d),��ǩ����:%d,��ǩ��ַ:%d),ֵ(%d): "
							,pDevice->GetName().c_str(), pDevice->GetAddr(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						pDriver->WriteLogAnyWay(chMsg);
						pDriver->WriteCommDataToLog( sndBuf, iDataLen,1 );
					}
				}
				break;
			case MacsDIS103_ASDU21RD://ASDU21��ȡ--Group:Entry��ע�⣬Entry=0��ʾ��ȡ����
				{
					OpcUa_Byte byValue =0;
					pDevice->SetVal(MacsDIS103_DICB, m_tCtrlRemoteCmd.lTagAddress,&byValue,1,0,true);

					iDataLen = pDevice->BuildASDU21GroupFrame( sndBuf, m_tCtrlRemoteCmd.lTagAddress );					
					if ( iDataLen > 0 )
					{
						snprintf(chMsg,sizeof(chMsg)
							,"����ASDU21��ȡ����ASDU21RD(�豸��:%s,�豸��ַ:%d),��ǩ����:%d,��ǩ��ַ:%d),ֵ(%d): "
							,pDevice->GetName().c_str(), pDevice->GetAddr(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, m_tCtrlRemoteCmd.byData[0]);
						pDriver->WriteLogAnyWay(chMsg);
						pDriver->WriteCommDataToLog( sndBuf, iDataLen,1 );
					}
				}
				break;
			default:
				break;
			}
		}

		return iResult;
	}

	OpcUa_Int32 CFepIEC103Prtcl::CaculateBroadPeriod( void )
	{
		CP56Time2a	LocalTime;

		ACE_Time_Value NowTime;	
		ACE_Date_Time dateTime;			
		NowTime = ACE_OS::gettimeofday();
		dateTime.update( NowTime );
		memset(&LocalTime, 0, sizeof(CP56Time2a));			

		WORD MillSec = WORD(dateTime.second()) * 1000 + WORD(dateTime.microsec()/1000);

		LocalTime.LowMillSec	= LOBYTE(MillSec);
		LocalTime.HighMillSec	= HIBYTE(MillSec);
		LocalTime.Minute		= (OpcUa_Byte)dateTime.minute();
		LocalTime.Hour			= (OpcUa_Byte)dateTime.hour();
		LocalTime.Day			= (OpcUa_Byte)dateTime.day();
		LocalTime.Week			= (dateTime.weekday()==0)?7:(OpcUa_Byte)dateTime.weekday();
		LocalTime.Mon			= (OpcUa_Byte)dateTime.month();
		LocalTime.Year			= dateTime.year() % 100;

		return (59-LocalTime.Minute)*60 + (60-dateTime.second());
	}

	OpcUa_CharA* CFepIEC103Prtcl::MakeLog(OpcUa_Byte* pbyPacketBuffer, OpcUa_Int32 wMsgLen, bool bSend)
	{
		OpcUa_CharA chMsg[1024];
		if( pbyPacketBuffer )
		{
			CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(m_pParam);
			OpcUa_Int32 j = 0;
			snprintf( chMsg, sizeof(chMsg), "%d���ַ���Ч:", wMsgLen );
			for ( OpcUa_Int32 i = 0; i < wMsgLen; i++ )
			{
				snprintf( chMsg, sizeof(chMsg), "%s %02X", chMsg, pbyPacketBuffer[i] );
			}

			pDrv->WriteLog(chMsg, true);
		}
		return "";
	}

	/*!
	*	\brief	 ����8λsumУ��	
	*
	*	\param	����	OpcUa_Byte * Buf: ҪУ������ݰ�
	*	\param	����	OpcUa_Int32 iByteLen: ������		
	*
	*	\retval	����ֵ	OpcUa_Byte: 8λУ��͵�ֵ
	*
	*	\note		
	*/
	OpcUa_Byte CFepIEC103Prtcl::Get8SumCode( OpcUa_Byte * Buf, OpcUa_Int32 iByteLen )
	{
		if( !Buf || iByteLen == 0 )
			return 0;
		OpcUa_Byte bSum = 0;
		for(OpcUa_Int32 i=0; i<iByteLen; i++)
			bSum += Buf[i];
		return bSum;
	}

	/*!
	*	\brief	��ʱ��ת���ɺ�����
	*
	*	\param	strTime	Ҫת����ʱ��	
	*
	*	\retval	OpcUa_Int64	��������Ϊ-1���ʾת��ʧ��
	*/
	OpcUa_Int64 CFepIEC103Prtcl::CaculateMSNum( DAILYTIME strTime )
	{
		if ( strTime.sMonth < 1 || strTime.sMonth > 12
			|| strTime.sDay < 1 || strTime.sDay > 31
			|| strTime.sHour < 0 || strTime.sHour > 24
			|| strTime.sMinute < 0 || strTime.sMinute > 60
			|| strTime.sSecond < 0 || strTime.sSecond > 60	)
		{
			return -1;
		}
		OpcUa_Int16 sMonthDay[12]={31,28,31,30,31,30,31,31,30,31,30,31};
		OpcUa_Int32 lSumDay;
		OpcUa_Int32 i;

		//! ����������
		lSumDay = 
			(OpcUa_Int32)(strTime.lYear-1970) * 365 +(OpcUa_Int32)(strTime.lYear-1-1968) / 4;

		for( i = 0; i < strTime.sMonth - 1; i ++ )
			lSumDay += sMonthDay[ i ];

		if ( ( strTime.sMonth > 2 ) && ( ( strTime.lYear - 1968 ) % 4 == 0 ) )
			lSumDay += 1; 

		lSumDay += strTime.sDay -1 ;

		//! �õ������1970.1.1��������
		OpcUa_Int64 nSec = (OpcUa_Int64)lSumDay * 24 * 60 * 60
			+ (OpcUa_Int64)strTime.sHour * 60 * 60
			+ (OpcUa_Int64)strTime.sMinute * 60
			+ (OpcUa_Int64)strTime.sSecond
			+ (OpcUa_Int64)strTime.sTimeZone * 60 * 60;

		//! �ܺ�����
		OpcUa_Int64 nTime = nSec * 1000 + strTime.sMSecond;

		return nTime;
	}

	bool CFepIEC103Prtcl::IsLinkOffline()
	{
		CFepIEC103Device* pDevice = NULL;
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(m_pParam);

		if ( pDrv->GetDeviceCount() <= 0 )
		{
			return false;
		}

		for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
			if (NULL != pDevice)
			{
				if ( !pDevice->IsOffLine() )
					return false;
			}
		}

		return true;
	}

	bool CFepIEC103Prtcl::IsLinkNotOnline()
	{
		CFepIEC103Device* pDevice = NULL;
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(m_pParam);

		if ( pDrv->GetDeviceCount() <= 0 )
		{
			return false;
		}

		for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
			if (NULL != pDevice)
			{
				if ( pDevice->IsOnLine() )
					return false;
			}
		}

		return true;
	}

	bool CFepIEC103Prtcl::IsLinkSuspicious()
	{
		CFepIEC103Device* pDevice = NULL;
		CFepIEC103Drv* pDrv = (CFepIEC103Drv*)(m_pParam);

		//�����·��û���豸���Ͳ���������
		if ( pDrv->GetDeviceCount() <= 0 )
		{
			return false;
		}

		for ( OpcUa_Int32 i = 0; i < pDrv->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC103Device*>(pDrv->GetDeviceByIndex(i));
			if (NULL != pDevice)
			{
				if ( !pDevice->IsSuspicious() )
					return false;
			}
		}

		return true;
	}
}