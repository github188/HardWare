/*!
 *	\file	FepModbusDriver.cpp
 *
 *	\brief	ͨ��Modbus������ʵ��Դ�ļ�
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusDriver.cpp 25    10-10-28 9:24 Miaoweijie $
 *	$Author: Miaoweijie $
 *	$Date: 10-10-28 9:24 $
 *	$Revision: 25 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "FepModbusDriver.h"
#include "FepModbusPrtcl.h"
#include "FepModbusDevice.h"


namespace MACS_SCADA_SUD
{
	void OnTmOut(void* p);							//! ��ʱ����ʱ�ص�����

	extern int GetPrivateInifileInt(const char* pchAppName, 
		const char* pchKeyName,	int nDefault, const char* pchFileName);	


	std::string CFepModbusDriver::s_ClassName("CFepModbusDriver");	//! ����

	std::string CFepModbusDriver::GetClassName()
	{
		return s_ClassName;
	}

	CFepModbusDriver::CFepModbusDriver()
	{
		m_nWatchTime = 30000;

		//m_pConnTimer = NULL;
		m_bConnected = false;
		m_iDevIndex		= 0;
		m_iDevLastIndex = m_iDevIndex;

		m_MaxHotLinkOffline = 300;
		m_nMaxStandbyLinkOffline = 600;
		m_nMaxLinkOffline = m_nMaxStandbyLinkOffline;	
		m_nRcvTimeOut = 5000;			
		m_nMaxOfflineCount = 20;
		m_bIsLinkGetRightFrame = false;
		m_nMaxLinkOfflineConnectCount = 3*4;
		m_nLinkOfflineConnectCount = 0;	

		m_bCanWork = false;
		m_bIsEnglish = false;
	}	

	CFepModbusDriver::~CFepModbusDriver()
	{
		//if(m_pConnTimer)
		//{
		//	m_pConnTimer->KillTimer();
		//	delete m_pConnTimer;
		//	m_pConnTimer = NULL;
		//}
	}

	/*!
	*	\brief	�õ��̳�PrtclParserBase����ļ̳�����
	*
	*	\retval	����ֵ���̳�����
	*/
	std::string CFepModbusDriver::GetPrtclClassName()
	{
		return "CFepModbusPrtcl";
	}

	std::string CFepModbusDriver::GetDeviceClassName()
	{
		return "CFepModbusDevice";
	}

	int CFepModbusDriver::StartWork(void)
	{
		CFepModbusPrtcl* pModbusPrtcl = dynamic_cast<CFepModbusPrtcl*>(m_pIOPrtclParser);

		if (!pModbusPrtcl)
		{			
			//! ��¼��־
			WriteLogAnyWay( "ERROR��CFepModbusDriver::StartWork: pModbusPrtcl == NULL!" );
			return -1;
		}

		//! ��¼��־
		WriteLogAnyWay( "CFepModbusDriver::StartWork..." );

		//! �����������ȥ���ӷ�����
		if( m_byPrtclType == 1 || m_byPrtclType == 2 )
		{
			//!��ʼ���������
			InitNetParam();

			if ( m_nSerAddrNum <= 0 )
			{
			//	char pchMsg[256];
			//	if(IsEnglishEnv())
			//	{
			//		snprintf(pchMsg, sizeof(pchMsg), "CFepModbusDriver::StartWork Error,Driver:%s is based On TCP but there is no IP Address Info", GetDriverName().c_str());
			//	}
			//	else
			//	{
			//		snprintf( pchMsg, sizeof(pchMsg),"CFepModbusDriver::StartWork ����,����:%s����TCP��������̬Port��û�������豸IP��ַ!!!",GetDriverName().c_str() );
			//	}
			//	WriteLogAnyWay( pchMsg );
			}
			else
			{
				m_pConnTimer = new CIOTimer(OnTmOut, this, true, pModbusPrtcl->GetTimerMgr());
			}

			if ( m_nSerAddrNum > 0 )
			{
				m_MaxHotLinkOffline = m_nSerAddrNum*m_byMaxReConn*( m_byConnTimeOut+m_nMaxOfflineCount*m_nRcvTimeOut*GetDeviceCount())/1000;
			}
			else
				m_MaxHotLinkOffline = 1*m_byMaxReConn*( m_byConnTimeOut+m_nMaxOfflineCount*m_nRcvTimeOut*GetDeviceCount())/1000;

			if ( m_byPrtclType == 2 && IsPortAccessClash() && IsFuzzy())
			{
				if ( m_nSerAddrNum > 0 )
				{
					m_nMaxStandbyLinkOffline = m_nSerAddrNum*m_byMaxReConn*( m_byConnTimeOut+3*(m_nWatchTime+m_nRcvTimeOut)*GetDeviceCount())/1000;
				}
				else
					m_nMaxStandbyLinkOffline = 1*m_byMaxReConn*( m_byConnTimeOut+3*(m_nWatchTime+m_nRcvTimeOut)*GetDeviceCount())/1000;
			}
			else
			{
				m_nMaxStandbyLinkOffline = m_MaxHotLinkOffline;
			}

			if ( m_nSerAddrNum > 0 )
			{
				m_nMaxLinkOfflineConnectCount = m_nSerAddrNum*m_byConnTimeOut*m_byMaxReConn/1000;

				//char pchMsg[256];
				//if (IsEnglishEnv())
				//{
				//	snprintf(pchMsg, sizeof(pchMsg), "CFepModbusDriver::StartWork The Most TCP connnection TimeOut:%d", m_MaxHotLinkOffline);
				//}
				//else
				//{
				//	snprintf( pchMsg, sizeof(pchMsg), 
				//		"StartWork: TCP��·�������ʱ�䣨IP��·����%d*ͬһ���������������%d*(�������ӳ�ʱʱ��%d(����) + �����豸������ߴ���%d*�豸����%d*��ʱʱ��%d(����))��= %d ��", 
				//		m_nSerAddrNum,m_byMaxReConn,m_byConnTimeOut,m_nMaxOfflineCount, GetDeviceCount(),m_nRcvTimeOut, m_MaxHotLinkOffline );
				//}
				//WriteLogAnyWay(pchMsg);
			}
			else
				m_nMaxLinkOfflineConnectCount = 1*m_byConnTimeOut*m_byMaxReConn/1000;

			//! Connect and Start Timer
			if ( m_nSerAddrNum > 0 )
				Connect();
			////////////////////////////////////////////////////////
		}
		else	//! �Ǵ�����ֱ�ӷ�����
		{	
			m_nSerAddrNum = 1;
			m_MaxHotLinkOffline = m_nMaxOfflineCount*GetDeviceCount()*m_nRcvTimeOut/1000;
			if (m_MaxHotLinkOffline < 2)
			{
				m_MaxHotLinkOffline = 2;
			}
			if ( IsPortAccessClash()  && IsFuzzy() )
			{
				m_nMaxStandbyLinkOffline = 3*GetDeviceCount()*(m_nWatchTime+m_nRcvTimeOut)/1000;
			}
			else
			{
				m_nMaxStandbyLinkOffline = m_MaxHotLinkOffline;
			}

			//char pchMsg[256];
			//if (IsEnglishEnv())
			//{
			//	snprintf(pchMsg, sizeof(pchMsg), "CFepModbusDriver::StartWork The Most Serial Connection OffLine Time:%d", m_MaxHotLinkOffline);
			//}
			//else
			//{
			//	snprintf( pchMsg, sizeof(pchMsg), 
			//		"StartWork: ������·�������ʱ�䣨�����豸������ߴ���%d*�豸����%d*��ʱʱ��%d(����): %d ��", 
			//		m_nMaxOfflineCount, GetDeviceCount(),m_nRcvTimeOut, m_MaxHotLinkOffline );
			//}
			//WriteLogAnyWay(pchMsg);

			///////////////////////////////////////////////////////////
			//! ��������
			int nDataLen = 0;
			pModbusPrtcl->BuildRequest( pModbusPrtcl->m_pbySndBuffer, nDataLen );
			///////////////////////////////////////////////////////////
		}

		m_bCanWork = true;

		if ( m_nSerAddrNum > 0 )
		{
			pModbusPrtcl->m_pPeriodTimer->SetTimer( PERIOD_SCAN_MS );
		}
		else
		{
			CFepModbusDevice* pDevice = NULL;
			for ( int i = 0; i < GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepModbusDevice*>(GetDeviceByIndex(i));
				if ( NULL != pDevice && !pDevice->IsSimulate() )
				{
					//if ( !pDevice->IsOffLine() )
					//{
					//	char pchMsg[128];
					//	if (IsEnglishEnv())
					//	{
					//		snprintf(pchMsg, sizeof(pchMsg), "CFepModbusDriver::StartWork The Driver is based on TCP, but there is no IP, The Address Device:%s(Address:%d) is offLine!", pDevice->GetName().c_str(), pDevice->GetAddr());
					//	}
					//	else
					//	{
					//		snprintf( pchMsg, sizeof(pchMsg), "CFepModbusDriver::StartWork ��������TCP������ȷû�������豸IP���豸%s(��ַ%d)һ�����������ˣ�", pDevice->GetName().c_str(), pDevice->GetAddr());
					//	}
					//	WriteLogAnyWay(pchMsg);
					//}
					pDevice->SetOnLine( DEVICE_OFFLINE );

					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes = 0;
				}
			}
		}
		
		return 0;
	}

	int CFepModbusDriver::StopWork( void )
	{
		int nResult = -1;
		if (m_pConnTimer)
		{
			m_pConnTimer->KillTimer();
			ACE_OS::sleep(1);
			delete m_pConnTimer;
			m_pConnTimer = NULL;
		}
		nResult = CIODriver::StopWork();
		return nResult;
	}

	int CFepModbusDriver::OpenChannel()
	{
		CIODriver::OpenChannel();

		//!��ȡ���������л���
		//if (g_pSystemString)
		//{
		//	m_bIsEnglish = g_pSystemString->GetLanguage();
		//}

		m_pIODrvBoard = new CIODrvBoard();
		if ( m_pIODrvBoard == NULL )
		{
			MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CFepModbusDriver::StartWork:m_pIODrvBoard is Null")));
			return -2;
		}

		//! ���ModbusЭ��������Ϣ
		ReadIniCfg();

		CFepModbusPrtcl* pModbusPrtcl = dynamic_cast<CFepModbusPrtcl*>(m_pIOPrtclParser);
		if (!pModbusPrtcl)
		{
			return -1;
		}

		//! ��ʼ��Э����
		pModbusPrtcl->Init( m_byPrtclType, m_nMaxReSend );

		//! ��ʼ��Board
		/*
		std::string strPortType = m_pPortCfg.strType.toUtf8();
		int nRes = m_pIODrvBoard->Init( m_pIOPrtclParser, strPortType, m_pPortCfg, this->GetTimeOut());	
		if (strPortType == "TCPIP" && 0 == nRes)
		{
		if (!m_pPortCfg.strIPA_A.isEmpty())
		{
		m_pIODrvBoard->open();
		}
		else
		{
		OpcUa_CharA pchMsg[256];
		snprintf(pchMsg, 256, "CFepModbusDriver:: %s OpenChannel failed because the IP is empty", GetDriverName().c_str());
		WriteLogAnyWay(pchMsg);
		}
		}
		else if(strPortType == "SERIAL" && 0 == nRes)
		{
		m_pIODrvBoard->open();
		}
		else
		{
		OpcUa_CharA pchMsg[256];
		snprintf(pchMsg, 256, "CFepModbusDriver:: %s OpenChannel failed Because The IODrvBoard Type:%s Init Failed!", GetDriverName().c_str(), strPortType.c_str());
		WriteLogAnyWay(pchMsg);
		return -1;
		}*/
		return 0;
	}

	//! ��ʼ���������Ӳ���
	int CFepModbusDriver::InitNetParam()
	{
		m_nConnTimes		= 0;
		m_nSerAddrNum		= 0;
		m_nCurSerAddrIndex	= 0;

		//! �������˿ں�
		m_nPort				= m_pPortCfg.nPort;
		//! ��ʼ����������ַ�б�
		AddSerAddr( m_pPortCfg.strIPA_A.c_str());
		//AddSerAddr( m_pPortCfg.strIPA_B.toUtf8() );
		//AddSerAddr( m_pPortCfg.strIPB_A.toUtf8() );
		//AddSerAddr( m_pPortCfg.strIPB_B.toUtf8() );

		return 0;
	}

	//! ��ӷ�����
	void CFepModbusDriver::AddSerAddr(const char* pchSerAddr)
	{
		if( pchSerAddr && strcmp( pchSerAddr,"") != 0 
			&& m_nSerAddrNum < MAX_SERVERIP_COUNT )
			m_stSerAddrList[m_nSerAddrNum++] = pchSerAddr;
	}

	//! ��װ���Ӳ���������Ϊ���������
	int CFepModbusDriver::Connect(int nIndex)
	{
		//! ������
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard( m_Mutex );
		if( nIndex < 0 || nIndex >= m_nSerAddrNum )
		{
			return -1;
		}

		//����ǰ������շ������������������豸���ߺ�����ʱ�������Ƿ������ӳɹ�����Щ�豸����Զ���ߡ�
		for ( int i = 0; i < GetDeviceCount(); i++ )
		{
			CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>(GetDeviceByIndex(i));
			if ( pDevice )
			{
				pDevice->m_byTxTimes = 0;
				pDevice->m_byRxTimes = 0;
			}
		}


		//! �����Ӷ�ʱ��. 
		//ע�⣺��������������ӳ�ʱ��ʱ������Connect�������ܷ���������Ϊ��ǰ�ߵ����ӳɹ��󣬽������ӳɹ��ص�����ʱ��ͼKillConnTimerʱ
		//��ʧ�ܣ���Ϊ��ʱʵ����δ����ConnTimer����������Ȼ���ӳɹ�����ʵ����Ȼ�����ConnTimer��TimeOut�¼���2014-03-04 by hxw
		m_pConnTimer->KillTimer();
		int nR = m_pConnTimer->SetTimer( m_byConnTimeOut );
		//if ( nR < 0 )
		//{
		//	//! ��¼��־
		//	char pch[256] = {0};
		//	snprintf( pch, 256, "CFepModbusDriver::Connect: SetTimer Error, nRet = %d!", nR );
		//	WriteLogAnyWay( pch );
		//}

		if ( m_nLinkOfflineConnectCount++ >= m_nMaxLinkOfflineConnectCount )
		{
			//! ���豸����
			for( int i = 0; i < GetDeviceCount(); i++)
			{
				CIODeviceBase* pDevice = GetDeviceByIndex(i);
				if ( !pDevice->IsSimulate() )
				{
					//if ( !pDevice->IsOffLine() )
					//{
					//	char pchMsg[128];
					//	if (IsEnglishEnv())
					//	{
					//		snprintf(pchMsg, sizeof(pchMsg), "CFepModbusDriver::Connect Having Reconnted:%d but failed, Device:%s(Address:%d)is Offline!",m_nMaxLinkOfflineConnectCount,pDevice->GetName().c_str(),pDevice->GetAddr());
					//	}
					//	else
					//	{
					//		snprintf( pchMsg, sizeof(pchMsg), 
					//			"CFepModbusDriver::Connectȫ����·����%d�ζ�û�����ӳɹ����豸%s(��ַ%d)�����ˣ�", m_nMaxLinkOfflineConnectCount, pDevice->GetName().c_str(), pDevice->GetAddr());
					//	}
					//	
					//	WriteLogAnyWay(pchMsg);
					//}

					pDevice->m_byRxTimes = 0;
					pDevice->m_byTxTimes = 0;

					pDevice->SetOnLine( DEVICE_OFFLINE );
				}
			}

		}

		//char pchLog[MAXDEBUGARRAYLEN] = {0};
		//if (IsEnglishEnv())
		//{
		//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusDriver::Connect Starting Connecting IP:%s Port:%d Index:%d", m_stSerAddrList[nIndex].c_str(), m_nPort, nIndex);
		//}
		//else
		//{
		//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusDriver::Connect ��ʼ��������%s��Port=%d, ���Ϊ%d!", m_stSerAddrList[nIndex].c_str(), m_nPort, nIndex);
		//}
		////! ��¼��־
		//WriteLogAnyWay( pchLog );

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
	void CFepModbusDriver::ReConnect( bool bNext )
	{
		if (!IsStopWork())
		{
			//! ����������1
			IncrConnTime();

			//! �������������������������һ��������
			if( bNext || ( m_nConnTimes >= m_byMaxReConn ) )
			{
				m_nConnTimes = 0;
				ConnectNext();
			}
			else
			{
				//! ����
				Connect(m_nCurSerAddrIndex);
			}	
		}
	}

	/*!
	*	\brief	������һ��������
	*/
	void CFepModbusDriver::ConnectNext()
	{
		m_nConnTimes = 0;
		if( ++m_nCurSerAddrIndex >= m_nSerAddrNum)
			m_nCurSerAddrIndex = 0;

		Connect( m_nCurSerAddrIndex );
		return;
	}

	//! ��ȡ�����ļ�
	int CFepModbusDriver::ReadIniCfg( )
	{
		//! ���������
		std::string csDriverName = GetDriverName();
		//! ��ȡ�����ļ�����·��
		std::string file_name = CIODriver::GetFEPRunDir() + MACS_DRIVER_FILENAME;

		m_nWatchTime = GetPrivateInifileInt(
			csDriverName.c_str(), ACE_TEXT("WatchTime"), 30000, file_name.c_str());
		if ( m_nWatchTime < 15000 )
		{
			m_nWatchTime = 30000;
		}

		//! PrtclType
		m_byPrtclType = (Byte)GetPrivateInifileInt(
			csDriverName.c_str(), ACE_TEXT("PrtclType"), 0, file_name.c_str());
		if( m_byPrtclType < 0 || m_byPrtclType > 2 )
			m_byPrtclType = 0;

		//! MaxReConn
		m_byMaxReConn = GetPrivateInifileInt(
			csDriverName.c_str(), ACE_TEXT("MaxReConn"), 0, file_name.c_str());
		if( m_byMaxReConn < 1 )
			m_byMaxReConn = 3;

		//! ConnTimeOut
		m_byConnTimeOut = GetPrivateInifileInt(
			csDriverName.c_str(), ACE_TEXT("ConnTimeOut"), 0, file_name.c_str());
		if( m_byConnTimeOut < 1000 || m_byConnTimeOut > 10000 )
			m_byConnTimeOut = 2000;

		//! MaxReSend
		m_nMaxReSend = GetPrivateInifileInt(
			csDriverName.c_str(), ACE_TEXT("MaxReSend"), 0, file_name.c_str());
		if( m_nMaxReSend < 0 )
			m_nMaxReSend = 0;

		m_nMaxRetryPeriod = GetPrivateProfileInt(csDriverName.c_str(), ACE_TEXT("RetryPeriod"), 5, file_name.c_str());
		if ( m_nMaxRetryPeriod < 5 )
		{
			m_nMaxRetryPeriod = 5;
		}

		char pchLog[128];
		m_nMaxRetryPeriod = m_nMaxRetryPeriod*1000/PERIOD_SCAN_MS; //ת����ɨ�����ڵı���

		//! д��־
		//if (IsEnglishEnv())
		//{
		//	snprintf(pchLog, sizeof(pchLog),"\n\nCFepModbusDriver::ReadIniCfg The Diagnosis Period for Offline Device:%d S", m_nMaxRetryPeriod);
		//}
		//else
		//{
		//	snprintf( pchLog, sizeof(pchLog),"\n\nCFepModbusDriver::ReadIniCfg ��·�ϵ������豸�������=%d S!", m_nMaxRetryPeriod );
		//}
		//WriteLogAnyWay( pchLog );

		m_nRcvTimeOut = GetPrivateProfileInt(csDriverName.c_str(), "RcvTimeOut", -1, file_name.c_str() );
		if ( m_nRcvTimeOut < 50 )
		{
			m_nRcvTimeOut = 200;
		}

		m_nMaxOfflineCount = GetPrivateProfileInt(csDriverName.c_str(), "MaxOfflineCount", -1,file_name.c_str());
		if ( m_nMaxOfflineCount < 3 )
		{
			m_nMaxOfflineCount = 3;
		}

		return 0;
	}

	/*!
	*	\brief	��ʼ���豸
	*	�õ��豸��������̬�����豸����
	*	�õ��豸ģ����������̬�����豸ģ�����
	*	�����豸��״̬�ṹָ�룬���ö���ָ�룻
	*	��ʼ���豸
	*
	*	\param	pCfgUnit���豸���ö���ָ��
	*
	*	\retval	����ֵ��0--�ɹ�������ʧ��
	*/
	int CFepModbusDriver::InitUnit(CIOUnit* pCfgUnit)
	{
		if (!pCfgUnit )
		{
			MACS_ERROR(( ERROR_PERFIX
				ACE_TEXT("CFepModbusDriver::InitUnit : Parameter is Null") ));
			//! ���������Ϣ
			OutputDebug( "CFepModbusDriver::InitUnit : Parameter is Null" );
			return -1;
		}
		
		int nAddr = pCfgUnit->GetDevAddr();
		std::string strDevName = pCfgUnit->GetUnitName();
		if ( nAddr <= 0 || nAddr > 255 )
		{
			//UaString ustrMsg(UaString("CFepModbusPrtcl::InitUnit Device:%1 Address:%2 is not Right to Modbus Protocol!").arg(strDevName.c_str()).arg(nAddr));
			//WriteLogAnyWay(ustrMsg.toUtf8());
			return -100;
		}

		return CIODriver::InitUnit(pCfgUnit);
	}

	int CFepModbusDriver::WriteLogAnyWay( const char* pchLog, bool bTime )
	{
		//if ( !m_pLog )
		//{
		//	return -1;
		//}

		//m_pLog->WriteLog( pchLog, bTime );

		return 0;
	}

	int CFepModbusDriver::StartConnTimer()
	{
		int nR = -1;
		if (m_pConnTimer)
		{
			nR = m_pConnTimer->SetTimer( m_byConnTimeOut );
			if ( nR < 0 )
			{
				//! ��¼��־
				//char pch[256];
				//sprintf( pch, "ERROR��CFepModbusDriver::ReConnect: SetTimer Error, nRet = %d!", nR );
				//WriteLogAnyWay( pch );
			}
		}
		return nR;
	}

	bool CFepModbusDriver::IsLinkOffline()
	{
		if ( GetDeviceCount() <= 0 )
		{
			return false;
		}

		CFepModbusDevice* pDevice = NULL;
		for ( int i = 0; i < GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepModbusDevice*>(GetDeviceByIndex(i));
			if (NULL != pDevice)
			{
				if ( !pDevice->IsOffLine() )
					return false;
			}
		}
		return true;
	}

	bool CFepModbusDriver::IsFuzzy()
	{
		CFepModbusDevice* pDevice = NULL;
		for ( int i = 0; i < GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepModbusDevice*>(GetDeviceByIndex(i));
			if (NULL != pDevice)
			{
				if ( pDevice->IsFuzzy() )
					return true;
			}
		}

		return false;
	}

	bool CFepModbusDriver::CanWork()
	{
		return m_bCanWork;
	}

	int CFepModbusDriver::GetMaxOffLineCount()
	{
		return m_nMaxOfflineCount;
	}

	int CFepModbusDriver::GetRcvTimeOut()
	{
		return m_nRcvTimeOut;
	}

	bool CFepModbusDriver::IsEnglishEnv()
	{
		return m_bIsEnglish;
	}

	//! ��ʱ����ʱ�ص�����
	void OnTmOut(void* p)					
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)p;
		//!�رն�ʱ��
		if(pDriver && !pDriver->IsStopWork())
		{
			//! ������
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard( pDriver->m_Mutex );
			pDriver->GetConnTimer()->KillTimer();

			char pchLog[MAXDEBUGARRAYLEN];
			sprintf( pchLog, "Modbus::OnTmOut: Connecting IP:%s time Out!", pDriver->GetCurServerAddr().c_str() );
			//! ��¼��־
			pDriver->WriteLogAnyWay( pchLog );

			//! ����������ˣ�������������
			if(pDriver->m_pIODrvBoard && (pDriver->m_pIODrvBoard->ConnState()))
			{
				pDriver->SetConnTime( 0 );	
				sprintf( pchLog, "Modbus::OnTmOut: Connect IP:%s successful!��return!", pDriver->GetCurServerAddr().c_str() );
				//! ��¼��־
				pDriver->WriteLogAnyWay( pchLog );
				pDriver->SetConnected( true );
				return;
			}
			//! ����
			pDriver->ReConnect();
		}
	}
}
