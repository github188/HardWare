/*!
 *	\file	FepIEC104Prtcl.cpp
 *
 *	\brief	104��վЭ����ʵ��Դ�ļ�
 *
 *	$Header: ?/MACS-SCADA 3.2 �������/MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Prtcl.cpp, 219+1 2011/04/11 07:14:19 miaoweijie $
 *	$Author: miaoweijie $
 *	$Date: 2011/04/11 07:14:19 $
 *	$Revision: 219+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOLDBFIFORNETPRTCL_20080513_CPP_
#define _IOLDBFIFORNETPRTCL_20080513_CPP_

#include "FepIEC104Prtcl.h"
#include "FepIEC104Drv.h"
#include "FepIEC104Device.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"

#define MAX(a,b) ((a)>(b) ? (a):(b))

namespace MACS_SCADA_SUD
{
	void OnTimeOut(void* p);							//! ��ʱ����ʱ�ص�����
	void PeriodProc( void* pParent );					//! ���ڶ�ʱ���ص�����		
	
	std::string CFepIEC104Prtcl::s_ClassName("CFepIEC104Prtcl");		//! ����

	CFepIEC104Prtcl::CFepIEC104Prtcl() 
	{
		//InitPrtclHelper( 6, 2048, 1024, 1024, 4096 );		//eldest
		InitPrtclHelper( 6, 255, 65536, 255, 65536*2 );	//2013.12.3
		//InitPrtclHelper( 6, 2048, 65536, 255, 65536*2 );	//2013.12.6
		m_iPeriodCount= 0;		//��ʼ��Ϊ0���Ա�����ʱ����ִ��һ�Ρ�
		m_iForcedReconnectCount = 0;

		m_nSndLen = 0;
		//m_pBuf = new OpcUa_Byte[1024];//hbbΪ����pPrtcl->BuildRequest( pPrtcl->m_pBuf, pPrtcl->m_nSndLen );�����ڴ�
		m_pBuf = NULL;//Modiby by hxw in 2013.12.3. �������ڱ��������迪���ڴ�

		m_bIsLogValueError = true;
		m_bCheckSDI = true;
		m_bCheckDDI = true;
		m_bCheckVTI = true;

		//! ������ʱ��
		m_pIOTimer = new CIOTimer( OnTimeOut, this, true );
		m_pPeriodTimer = new CIOTimer( PeriodProc, this );

		m_wdValidDataLen	= 0;
		m_nOfflineCount		= 0;

		m_shAdiLogical = 0;
		m_lastDevNo = 0;

		m_bNeedSendSFrame	= false;
		m_bNeedStopDT		= false;
		m_bNeedStartDT		= true;
		m_bNeedSendTest		= false;
		m_bNeedStartDTcon	= false;
		m_bNeedStopDTcon	= false;
		m_bNeedSendTestcon	= false;

		//��ʱ�����롣50~10000��Ĭ��1000��
		m_TimeOut = 1000;

		//�������ӵĳ�ʱ�����룬Ĭ��30000
		m_t0 = 30000;

		//���ͻ����APDU�ĳ�ʱ�����룬Ĭ��15000
		m_t1 = 15000;

		//�����ݱ���t2<t1ʱȷ�ϵĳ�ʱ�����룬Ĭ��10000
		m_t2 = 10000;

		//���ڿ���t3>t1״̬�·��Ͳ���֡�ĳ�ʱ�����룬Ĭ��20000
		m_t3 = 30000;

		//�������ٻ����ڣ����룬Ĭ��60000
		m_t4 = 60000;

		m_TestCycl = 10000;

		//Уʱ����, ��.Ĭ��3600.
		m_CheckTimeCycl = 3600;

		//���߼�����Ĭ��15.
		m_nMaxOfflineCount = 15;

		m_bDebugLog = false;
		m_bLog = false;

		m_bNeedConnected = false;		//����ʱ��StartWork()�Զ����ӣ���˳�ֵ������TRUE��

		m_nSFrameCount = 0;
		m_nIFrameCount = 0;
		m_nLastIFrameCount = 0;

	}

	CFepIEC104Prtcl::~CFepIEC104Prtcl(void)
	{
		//! ɾ����ʱ��
		if(m_pIOTimer)
		{
			delete m_pIOTimer;
			m_pIOTimer = NULL;
		}

		if(m_pPeriodTimer)
		{
			delete m_pPeriodTimer;
			m_pPeriodTimer = NULL;
		}
		//delete m_pBuf;//hbb�ͷ��ڴ�. Modiby by hxw in 2013.12.3
	}

	std::string CFepIEC104Prtcl::GetClassName()
	{
		return s_ClassName;
	}

	/*
	�õ���С�İ�����
	*/
	OpcUa_Int32 CFepIEC104Prtcl::GetMinPacketLength()
	{
		return PRTCL_MIN_PACKET_LENGTH;
	}

	/*
	������֡
	*/
	OpcUa_Int32 CFepIEC104Prtcl::BuildRequest( 
		OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen )
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "BuildRequest Starting --------------!" ) ));
		pDriver->WriteLog( "BuildRequest: Start......." );

		//������
		nDataLen = 0;
                
		//! ���û�����ӣ�ֱ�ӷ���
		CFepIEC104Device* pDevice = NULL;

		if ( m_bNeedConnected )
		{

			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest return -4!" ) ));
			pDriver->WriteLog( "BuildRequest: ��·δ����, ֱ�ӷ���." );
			return -4;
		}		
		///////////////////////////////////////////////////////////

		if ( CIOPrtclParser::BuildRequest( pbyData, nDataLen )==-1 )
		{
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest return -1  000000000!" ) ));
			pDriver->WriteLog( "BuildRequest: CIOPrtclParser::BuildRequest return -1" );
			return -1;
		}

		bool bSendYK = false;

		//ң�ء�ң�������������ȷ��͡�
		//���Ϊ�˷�ֹң�ض�ʧ���ӻ�Ҳ����ң�ء�����104��Լ���ӻ�Ҳ�ɷ��Ϳ������hexuewen
		nDataLen = BuildCtlRequest( pbyData );
		if ( nDataLen > 0 )
		{
			OpcUa_Int32 i;

			ASDUHead* pHead = ( ASDUHead* )( pbyData + sizeof(APCI) );
			//���߼���Ԫ���л�ΪDO��AO�����豸���Ա��´ξ�����ѯ�����豸��
			if ( GetDeviceNoByAddr( (OpcUa_Int32)pHead->wdCommAddr, i ) )
			{
				m_shAdiLogical = i;
				pDevice = dynamic_cast<CFepIEC104Device*>( 
					pDriver->GetDeviceByIndex(m_shAdiLogical) );
				if ( pDevice )
				{
					//�÷��ŵ�Ϊ0
					OpcUa_Byte OpcUa_Bytemp = 0;
					pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytemp, 1, 0, true );	
				}
			}

			pDriver->WriteLog( "���ɿ�������֡��" );
			m_bycmdType = 1;

			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest ����ң������!" ) ));
			bSendYK= true;
		}
		else//! ��ң����������ѯ֡��ע�⣬��һ��ÿ�ζ����뷢������
		{
			short LastLogicalDevNo = m_shAdiLogical;
			do{
				pDevice = dynamic_cast<CFepIEC104Device*>( 
					pDriver->GetDeviceByIndex(m_shAdiLogical) );

				if( !pDevice )
				{
					MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest return -1  222222222!" ) ));
					return -1;
				}

				//! �ж��豸�Ƿ�ģ���ˣ������ģ���ˣ��򲻷���������
				if ( pDevice->IsSimulate() )
				{
					MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest return -2!" ) ));
					return -2;
				}				

				if ( pDevice->IsHot() )		//! Ϊʵ����,�������������������.
				{
					pDriver->WriteLog( "BuildRequest: to BuildLocalNormalFrame...." );

					nDataLen = BuildLocalNormalFrame( pbyData );

					m_bycmdType = 0;

					//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest ������֡!" ) ));

				}
				else			//! Ϊʵ����,����������Ĵӻ�����.
				{
					pDriver->WriteLog( "BuildRequest: to BuildLocalStandbyFrame...." );

					nDataLen = BuildLocalStandbyFrame( pbyData );

					m_bycmdType = 0;
				}

				//�л��߼��豸��,׼���´ε�����
				m_shAdiLogical ++;
				if ( m_shAdiLogical >= pDriver->GetDeviceCount() )
				{
					m_shAdiLogical = 0;
				}

				//��֡�ɹ��󣬼��˳�ѭ��
				if ( nDataLen > 0 )
				{
					break;
				}

			}while ( LastLogicalDevNo != m_shAdiLogical );
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//! ��������
		if(nDataLen > 0)
		{
			//�ж��Ƿ�����
			pDevice->m_byTxTimes ++;
			//if (pDevice->m_byTxTimes > MACSIEC104_NEEDCHECK_AFTERTXTIMES )
			//{
			//	pDevice->SetOnLine( pDevice->m_byRxTimes > 0 );
			//	pDevice->m_byTxTimes = 0;
			//	pDevice->m_byRxTimes = 0;
			//}

			////! KillTimer
			m_pIOTimer->KillTimer();
			////! ������ʱ��
			m_pIOTimer->SetTimer(m_TimeOut);
   
			//���ӳ�10�����ٷ���
			ACE_Time_Value tv(0,10000);
			ACE_OS::sleep(tv);

			if( pDriver->m_pIODrvBoard->Write(pbyData, nDataLen) !=  0)
			{
				//UpdateDevState();
				return -3;
			}
                        
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest Write Frame !!!!!!!!!!!!!!!!" ) ));

			if ( bSendYK )
			{
				MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::�����緢��ң��!" ) ));
			}

			//else
			//{
			//	m_nOfflineCount = 0;

			//	//��λ���ӱ�־
			//	m_bNeedConnected = false;
			//}
		}
		else
		{
			//static OpcUa_Int32 nCounts = 0;
/*
			//
			//if( nCounts%10 == 0 )
			//{
				//�ж��Ƿ�����
				pDevice->m_byTxTimes ++;
				//if (pDevice->m_byTxTimes > MACSIEC104_NEEDCHECK_AFTERTXTIMES )
				//{
				//	pDevice->SetOnLine( pDevice->m_byRxTimes > 0 );
				//	pDevice->m_byTxTimes = 0;
				//	pDevice->m_byRxTimes = 0;
				//}

				//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send S format Frame Count=%d!" ), m_nSFrameCount ));
				nDataLen = pDevice->BuildSFrame(pbyData);
				//m_nSFrameCount ++;

				////! KillTimer
				m_pIOTimer->KillTimer();
				////! ������ʱ��
				m_pIOTimer->SetTimer(MACSIEC104DRIVER_MAX_TIMEOUT);

				if( pDriver->m_pIODrvBoard->Write(pbyData, nDataLen) !=  0)
				{
					UpdateDevState();
					return -3;
				}

				
			//}

			//nCounts++;
*/

			//���������ճ�ʱ����Ҫע���ڳ�ʱ�ص��������ж��ϴ��Ƿ�Ϊ�ճ�ʱ��
			////! KillTimer
			m_pIOTimer->KillTimer();
			////! ������ʱ��
			m_pIOTimer->SetTimer(m_TimeOut);

		}
		
		/////////////////////////////////////////////////////////////////

		//! д��־
		pDriver->WriteLog( "Build Request!" );

		//! ��¼ͨ�ż���
		if ( nDataLen>0 )
		{
			pDriver->WriteLog( "Send Request!" );
			pDriver->WriteCommData( pbyData, nDataLen, COMMDATA_SND );
		}

		//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl::BuildRequest return 0!" ) ));
		return 0;
	}

	// *****************************************************
	// ��������: ���·�����֡
	// �� �� ֵ: OpcUa_Int32 : ֡����
	// ����˵��: 
	//    OpcUa_Byte* pFrame: ֡������
	// *****************************************************
	OpcUa_Int32 CFepIEC104Prtcl::BuildCtlRequest( OpcUa_Byte* sndBuf )
	{
		if ( !sndBuf )
			return 0;

		OpcUa_Int32 nRet = 0;
		OpcUa_Byte pchBuf[512];
		OpcUa_Int32 nBufLen = 512;	
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		nRet = pDriver->m_IORemoteCtrl.DeQueue( (char*)pchBuf, nBufLen );

		OpcUa_Int32 nLength = 0;	//��ʼ��֡��
		if ( nRet==0 )
		{
			char chMsg[1024];		
			pDriver->WriteLogAnyWay("���·�����֡... ...");

			memcpy( (OpcUa_Byte*)(&m_tCtrlRemoteCmd), pchBuf, sizeof(m_tCtrlRemoteCmd) );

			//ȡ���豸��ַ
			CFepIEC104Device* pDevice = NULL;
			pDevice = dynamic_cast<CFepIEC104Device*>( 
				pDriver->GetDeviceByName(m_tCtrlRemoteCmd.pchUnitName) );
			if ( !pDevice )
			{
				pDriver->WriteLogAnyWay( "�����豸�����ڣ�" );
				return 0;
			}			

			WORD Address = (WORD)m_tCtrlRemoteCmd.lTagAddress;	//��ǩ��ַ
			OpcUa_Byte* buf	 = m_tCtrlRemoteCmd.byData;				//ֵ

			switch( m_tCtrlRemoteCmd.byUnitType )
			{
			case MACS_IEC104_SDOa:			//ң��. ��ң��ѡ��
				nLength = pDevice->BuildSDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_SDOb:			//ң��. ��ң��ִ��
				nLength = pDevice->BuildSDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_SDOc:			//ң��. ��ң������
				nLength = pDevice->BuildSDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DDOa:			//ң��. ˫ң��ѡ��
				nLength = pDevice->BuildDDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DDOb:			//ң��. ˫ң��ִ��
				nLength = pDevice->BuildDDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DDOc:			//ң��. ˫ң������
				nLength = pDevice->BuildDDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_UPDOa:			//ң��. ������ѡ��
				nLength = pDevice->BuildUPDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_UPDOb:			//ң��. ������ִ��
				nLength = pDevice->BuildUPDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_UPDOc:			//ң��. ����������
				nLength = pDevice->BuildUPDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSDOa:			//ң��. ��ң����ʱ�꣩��ѡ��
				nLength = pDevice->BuildTSDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSDOb:			//ң��. ��ң����ʱ�꣩��ִ��
				nLength = pDevice->BuildTSDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSDOc:			//ң��. ��ң����ʱ�꣩������
				nLength = pDevice->BuildTSDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDDOa:			//ң��. ˫ң����ʱ�꣩��ѡ��
				nLength = pDevice->BuildTDDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDDOb:			//ң��. ˫ң����ʱ�꣩��ִ��
				nLength = pDevice->BuildTDDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDDOc:			//ң��. ˫ң����ʱ�꣩������
				nLength = pDevice->BuildTDDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TUPDOa:		//ң��. ��������ʱ�꣩��ѡ��
				nLength = pDevice->BuildTUPDOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TUPDOb:		//ң��. ��������ʱ�꣩��ִ��
				nLength = pDevice->BuildTUPDObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TUPDOc:		//ң��. ��������ʱ�꣩������
				nLength = pDevice->BuildTUPDOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_NVAAOa:		//ң������һ��ֵ��ѡ��
				nLength = pDevice->BuildNVAAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_NVAAOb:		//ң������һ��ֵ��ִ��
				nLength = pDevice->BuildNVAAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_NVAAOc:		//ң������һ��ֵ������
				nLength = pDevice->BuildNVAAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_SVAAOa:		//ң������Ȼ�ֵ��ѡ��
				nLength = pDevice->BuildSVAAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_SVAAOb:		//ң������Ȼ�ֵ��ִ��
				nLength = pDevice->BuildSVAAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_SVAAOc:		//ң������Ȼ�ֵ������
				nLength = pDevice->BuildSVAAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_FAOa:			//ң�����̸�������ѡ��
				nLength = pDevice->BuildFAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_FAOb:			//ң�����̸�������ִ��
				nLength = pDevice->BuildFAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_FAOc:			//ң�����̸�����������
				nLength = pDevice->BuildFAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DWAOa:			//ң����32λ�ִ���ѡ��
				nLength = pDevice->BuildDWAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DWAOb:			//ң����32λ�ִ���ִ��
				nLength = pDevice->BuildDWAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_DWAOc:			//ң����32λ�ִ�������
				nLength = pDevice->BuildDWAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TNVAAOa:		//ң������һ��ֵ����ʱ�꣩��ѡ��
				nLength = pDevice->BuildTNVAAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TNVAAOb:		//ң������һ��ֵ����ʱ�꣩��ִ��
				nLength = pDevice->BuildTNVAAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TNVAAOc:		//ң������һ��ֵ����ʱ�꣩������
				nLength = pDevice->BuildTNVAAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSVAAOa:		//ң������Ȼ�ֵ����ʱ�꣩��ѡ��
				nLength = pDevice->BuildTSVAAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSVAAOb:		//ң������Ȼ�ֵ����ʱ�꣩��ִ��
				nLength = pDevice->BuildTSVAAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TSVAAOc:		//ң������Ȼ�ֵ����ʱ�꣩������
				nLength = pDevice->BuildTSVAAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TFAOa:			//ң�����̸���������ʱ�꣩��ѡ��
				nLength = pDevice->BuildTFAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TFAOb:			//ң�����̸���������ʱ�꣩��ִ��
				nLength = pDevice->BuildTFAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TFAOc:			//ң�����̸���������ʱ�꣩������
				nLength = pDevice->BuildTFAOcFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDWAOa:		//ң����32λ�ִ�����ʱ�꣩��ѡ��
				nLength = pDevice->BuildTDWAOaFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDWAOb:		//ң����32λ�ִ�����ʱ�꣩��ִ��
				nLength = pDevice->BuildTDWAObFrame( sndBuf, Address, buf );
				break;
			case MACS_IEC104_TDWAOc:		//ң����32λ�ִ�����ʱ�꣩������
				nLength = pDevice->BuildTDWAOcFrame( sndBuf, Address, buf );
				break;
			default:
				break;
			}

			snprintf(chMsg,sizeof(chMsg)
				,"��������(�豸��:%s,�豸��ַ:%d),UnitType=%d, ��Ϣ��ַ=%d, ֵ=%d!"
				,pDevice->GetName().c_str(),pDevice->GetAddr(),m_tCtrlRemoteCmd.byUnitType,Address,m_tCtrlRemoteCmd.byData[0]);
			pDriver->WriteLogAnyWay(chMsg);

			if ( nLength > 0 )
			{
				std::string szSndPacket = GetCommData( sndBuf, nLength, 0 );
				snprintf( chMsg, sizeof(chMsg),"�������%d�ֽ�: %s", nLength, szSndPacket.c_str() );
				pDriver->WriteLogAnyWay( chMsg );
			}
		}

		return nLength;
	}

	//! ���ڶ�ʱ���ص�����
	void PeriodProc( void* pParent )
	{
		//У�����
		CFepIEC104Prtcl* g_pPrtcl = (CFepIEC104Prtcl*)pParent;
		if ( g_pPrtcl == NULL )
		{
			return;
		}

		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)g_pPrtcl->GetParam();

		pDriver->WriteLog( "PeriodProc...");

		char pchLog[128];
		sprintf(pchLog, "PeriodProc: Before UpdateDevState: OfflineCount=%d,MaxOfflineCount=%d",g_pPrtcl->m_nOfflineCount,g_pPrtcl->m_nMaxOfflineCount );
		pDriver->WriteLog( pchLog );

		g_pPrtcl->m_nOfflineCount++;

		g_pPrtcl->UpdateDevState();

		CFepIEC104Device* pDevice = NULL;
		const OpcUa_Int32 nReconnect        = (g_pPrtcl->m_t0+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
		const OpcUa_Int32 nWholeCall		= (g_pPrtcl->m_t1+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
		//const OpcUa_Int32 nSFrame			= (g_pPrtcl->m_t3+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
		const OpcUa_Int32 nSFrame			= (3000+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;				//ÿ���������һ��
		const OpcUa_Int32 nTestFrame		= (g_pPrtcl->m_TestCycl+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;				
		const OpcUa_Int32 nCheckTime		= (g_pPrtcl->m_CheckTimeCycl*1000+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
		const OpcUa_Int32 nCountWholeCall	= (g_pPrtcl->m_t4+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
		const OpcUa_Int32 nMaxForcedReconnectCount = (MACSIEC104CONN_MAX_TIMEOUT)/1000 + 2;	//ǿ������

		sprintf(pchLog, "PeriodProc: After UpdateDevState: OfflineCount=%d,MaxOfflineCount=%d,PeriodCount=%d,SFrameCount=%d,TestFrame=%d",
			g_pPrtcl->m_nOfflineCount,g_pPrtcl->m_nMaxOfflineCount, g_pPrtcl->m_iPeriodCount, nSFrame, nTestFrame );
		pDriver->WriteLog( pchLog );

		//��̬���µ�����־��ʶ
		if ( g_pPrtcl->m_iPeriodCount % 10 == 0 )
		{
			std::string strAppName = pDriver->GetDriverName();
			char* pchAppName = (char* )strAppName.c_str();

			//! ��ȡ�����ļ�����·��
			std::string file_name = CIODriver::GetFEPRunDir() + MACS_DRIVER_FILENAME;	

			OpcUa_Int32 iTemp = GetPrivateProfileInt( pchAppName, "Debug", 0,  file_name.c_str() );
			g_pPrtcl->m_bDebugLog = (iTemp == 1)?true:false;

			iTemp = GetPrivateProfileInt( pchAppName, "Log", 0,  file_name.c_str() );
			g_pPrtcl->m_bLog = (iTemp == 1)?true:false;
		}

		if ( !g_pPrtcl->GetLinkState() && !pDriver->Connected() )	//�����豸������
		{
			if ( nReconnect && g_pPrtcl->m_iPeriodCount % nReconnect == 0 )	//���ӣ���������վʱ�䵽��
			{
				pDriver->WriteLogAnyWay( "PeriodProc: Need to reconnect...");
				g_pPrtcl->m_bNeedConnected = true;
			}
		}
		else if( pDriver->Connected() )	//��������豸�����ߵ���·���Ѿ������ˣ���Ӧ���ж����±�ʶ
		{
			for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepIEC104Device*>( pDriver->GetDeviceByIndex(i) );
				if ( pDevice )
				{
					if ( nWholeCall && g_pPrtcl->m_iPeriodCount % nWholeCall == 0 )				//���ٻ����ڵ���
					{
						pDevice->NeedWholeQuery(true);
					}
					if ( nSFrame && g_pPrtcl->m_iPeriodCount % nSFrame == 0 )					//����S֡���ڵ���
					{
						pDriver->WriteLog( "Need to send S Frame");
						g_pPrtcl->m_bNeedSendSFrame = true;
					}
					if ( nTestFrame && g_pPrtcl->m_iPeriodCount % nTestFrame == 0 )				//����Test֡���ڵ���
					{
						g_pPrtcl->m_bNeedSendTest = true;
					}
					if ( nCheckTime && g_pPrtcl->m_iPeriodCount % nCheckTime == 0 )				//��ʱ���ڵ���
					{
						pDevice->NeedCheckTime(true);
					}
					if ( nCountWholeCall && g_pPrtcl->m_iPeriodCount % nCountWholeCall == 0 )		//�ۼ������ٻ����ڵ���
					{
						pDevice->NeedCountQuery(true);
					}
				}
			}
		}

		if ( g_pPrtcl->m_bNeedConnected )
		{
			if( !pDriver->IsConnecting() || g_pPrtcl->m_iForcedReconnectCount % nMaxForcedReconnectCount == nMaxForcedReconnectCount-1 )
			{
				g_pPrtcl->m_iForcedReconnectCount = 0;
				pDriver->WriteLog( "PeriodProc: Start to reconnect." );
				g_pPrtcl->m_bNeedConnected = false;	//����һ�κ����۳ɹ�����´������ȴ����ڵ������ԡ�
				pDriver->IsConnecting(true);
				pDriver->ResetConnTimes();
				pDriver->Connected(false);
				pDriver->Connect( pDriver->NextSerAddrIndex() );
			}
			else
			{
				g_pPrtcl->m_iForcedReconnectCount++;
				pDriver->WriteLog( "PeriodProc: but it's connecting now." );
			}
		}

		g_pPrtcl->m_iPeriodCount++;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//OpcUa_Byte pbyBuf[256];
		//OpcUa_Int32 nDataLen = 0;
		//bool bTrigTimer = false;
		//long nTimeOut = 0;
		////! ��������
		//g_pPrtcl->BuildRequest( pbyBuf, nDataLen, bTrigTimer, nTimeOut );
	}


	//! ��ʱ����
	void OnTimeOut( void* p )
	{
		CFepIEC104Prtcl* pPrtcl = (CFepIEC104Prtcl*)p;

		if( !pPrtcl )
		{
			return;
		}

		//! KillTimer
		OpcUa_Int32 iRtn = pPrtcl->m_pIOTimer->KillTimer();

		//! �Ƿ�ģ������
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)pPrtcl->GetParam();
		CFepIEC104Device* pDevice = NULL;
		pDevice = dynamic_cast<CFepIEC104Device*> ( 
			pDriver->GetDeviceByIndex( pPrtcl->m_lastDevNo ) );

		if ( pPrtcl->m_nSndLen > 0 )
		{
			//! ��־
			if ( iRtn == 0 )	//��ʱ��ʱ����Ϊ��ʱ����Ϊ��ĳ�ʱ�ˡ�2014.9.11 hxw
			{
				std::string szData = pPrtcl->GetCommData( pPrtcl->m_pbySndBuffer, pPrtcl->m_nSndLen, 0 );
				char pchLog[1024];
				snprintf( pchLog, sizeof(pchLog), "�豸��Ӧ��ʱ,�豸����%s, ���η���֡��%s",
					pDevice->GetName().c_str(), szData.c_str() );
				pDriver->WriteLogAnyWay( pchLog );
			}

			if ( pDevice && pDevice->IsSimulate() )
			{
				pPrtcl->m_pIOTimer->SetTimer( pPrtcl->m_TimeOut );
				return;
			}

			//�ͷŶ�ռ��־
			if ( pDevice )
			{
				pDevice->IsCheckTiming(false);
				pDevice->WholeQuerying(false);
				pDevice->CountQuerying(false);
			}

			//! �����豸״̬
			//pPrtcl->UpdateDevState();
		}
		
		//! �����·״̬Ϊfalse����������һ��
		//if ( !pPrtcl->GetLinkState() )
		if ( pPrtcl->m_bNeedConnected )
		{
/*
			if( !pDriver->IsConnecting() )
			{
				pPrtcl->m_bNeedConnected = false;	//����һ�κ����۳ɹ�����´������ȴ����ڵ������ԡ�
				pDriver->IsConnecting(true);
				pDriver->ResetConnTimes();
				pDriver->Connected(false);
				pDriver->Connect( pDriver->NextSerAddrIndex() );
				//pDriver->SetConnTimer( MACSIEC104CONN_MAX_TIMEOUT );
			}
*/

			pDriver->WriteLog( "OnTimeOut: ��·δ����, ֱ�ӷ���." );
			return;
		}
		
		///////////////////////////////////////////////////////////

		//if(pPrtcl->m_bycmdType == 1)
		//{
			////ȡ���豸��ַ
			//CIODeviceBase* pDevice = NULL;
			//pDevice = pDriver->GetDeviceByName(pPrtcl->m_tCtrlRemoteCmd.pchUnitName);
		
			//pDriver->PostYKSOE( 
			//	pPrtcl->m_tCtrlRemoteCmd.pchUnitName,
			//	pPrtcl->m_tCtrlRemoteCmd.byUnitType,
			//	pPrtcl->m_tCtrlRemoteCmd.lTagAddress,
			//	0);
		//}

		/////////////////////////////////////////////////////////
		//OpcUa_Byte pbyBuf[256];
		//OpcUa_Int32 nDataLen = 0;
		//! ��������
		pDriver->WriteLog( "OnTimeOut: to call BuildRequest" );
		pPrtcl->BuildRequest( pPrtcl->m_pbySndBuffer, pPrtcl->m_nSndLen );
	}

	void  CFepIEC104Prtcl::UpdateDevState()
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		char pchLog[256];
		CIODeviceBase* pDevice = NULL;	
		OpcUa_Int32 i =0;

		//�������һ��ʱ�䶼û���յ��κ�104���ģ�����Ϊ�����ˡ�
		if ( m_nOfflineCount>= m_nMaxOfflineCount )	
		{
			m_nOfflineCount = 0;

			pDriver->Connected( false );
			
			for ( i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = pDriver->GetDeviceByIndex( i );
				if ( pDevice && !pDevice->IsSimulate()  )
				{
					if ( pDevice->IsOnLine() )
					{
						sprintf( pchLog, "�豸 %s ����һ��ʱ�䶼û���յ��κ�104����, ������", pDevice->GetName().c_str() );
						pDriver->WriteLogAnyWay( pchLog );
					}

					pDevice->SetOnLine( DEVICE_OFFLINE );
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes = 0;
				}
			}			
		}

		//�豸ΪHotʱ�� TCP���ӳɹ���Ϊ���ߣ��˺��������һ��ʱ�䶼û���յ�104 I��ʽ���ģ�����Ϊ�����ˡ�
		if ( m_iPeriodCount % m_nMaxOfflineCount == m_nMaxOfflineCount-1 )
		{
			for ( i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = pDriver->GetDeviceByIndex( i );
				if ( pDevice && !pDevice->IsSimulate()  )
				{
					if ( pDevice->IsHot() )
					{
						if ( m_nIFrameCount == m_nLastIFrameCount )
						{
							if ( pDevice->IsOnLine() )
							{
								sprintf( pchLog, "���豸(Hot) %s ����һ��ʱ��û���յ�104 I��ʽ����, ������", pDevice->GetName().c_str() );
								pDriver->WriteLogAnyWay( pchLog );
							}

							pDevice->SetOnLine( DEVICE_OFFLINE );
							pDevice->m_byTxTimes = 0;
							pDevice->m_byRxTimes = 0;
						}
					}
				}
			}
			m_nLastIFrameCount = m_nIFrameCount;	//���������ȵ��¸�������ڵ�ʱ�����m_nIFrameCountû�б仯������ΪHot�豸���ߡ�
		}

	}

	// *****************************************************
	// ��������: �������ݷ������ݰ�������
	// �� �� ֵ: BOOL : ������ݰ�����true������false
	// ����˵��: 
	//     OpcUa_Byte* pBuf: ���ݰ�
	//       OpcUa_Int32 iLen: ���ݰ�����
	// *****************************************************
	OpcUa_Int32 CFepIEC104Prtcl::RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nLen )
	{

		//! ͨ���������ֹ�ˣ���ֱ�ӷ���0
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		if (!pDriver)
			return -1;	

		char strLog[256];
		if ( IsLogForRxDataProc() )
		{
			pDriver->WriteLogAnyWay( "RxDataProc: begin...." );
		}

		if(  pDriver->IsDisable() )
		{
			if ( IsLogForRxDataProc() )
			{
				pDriver->WriteLogAnyWay( "ͨ������ֹ" );
			}
			return 0;
		}

		if ( IsLogForRxDataProc() )
		{
			pDriver->WriteLogAnyWay( "RxDataProc: WriteCommData begin...." );
		}

		///////////////////////////////////////////////////////
		//! �����Ҫͨ�ż��ӣ�����ԭʼ���ݰ�
		pDriver->WriteCommData( pDataBuf, nLen );

		if ( IsLogForRxDataProc() )
		{
			pDriver->WriteLogAnyWay( "RxDataProc: WriteCommData end.." );
		}

		//�����ݷŵ��ڲ�������
		InnerAddData( pDataBuf, nLen );

		if ( IsLogForRxDataProc() )
		{
			pDriver->WriteLogAnyWay( "RxDataProc: InnerAddData end.." );
		}

		//���ڲ���������ִ�л������ڣ������ݰ���ע��һ�ο��ܻᴦ�������ݰ�
		bool bPacketOk = false;
		OpcUa_Int32	nReceived = 0;
		do
		{
			//��ȡ�����ݳ���
			nReceived = PRTCL_RCV_BUFFER_LENGTH - m_nRevLength;
			if (0 >= nReceived || nReceived > PRTCL_RCV_BUFFER_LENGTH)
			{
				m_nRevLength = 0;
				m_bIncompleteFrame	= false;
				break;
			}

			//���ڲ�������ȡ������
			if( InnerRead( m_pbyRevBuffer+m_nRevLength, nReceived, nReceived) 
				&& nReceived > 0 )
			{
				m_nRevLength += nReceived;
				m_bIncompleteFrame = false;

				while( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH && !m_bIncompleteFrame )
				{
					if ( m_wdValidDataLen > 0 )	//ǰһ��ExtractPacket()�ѳ����жϽ����ɹ�
					{
						if ( m_bExtractError )	//ǰһ�ν����ɹ������ݴ���ʧ��
						{//ȥ�����ֽڣ����½���

							if ( IsLogValueError() )
							{
								char strLog[1024]={0};
								snprintf( strLog, sizeof(strLog),"ǰһ�ν����ɹ�����������ʱʧ��,ȥ������(%02X)�������½���!", m_pBuf[0] );
								pDriver->WriteLogAnyWay( strLog );

								std::string szSndPacket = GetCommData( m_sPacketBuffer, m_wdValidDataLen, 1 );
								snprintf( strLog, sizeof(strLog),"����ʧ�ܵ�ԭ����(��%d�ֽ�): %s", m_wdValidDataLen, szSndPacket.c_str() );
								pDriver->WriteLogAnyWay( strLog );
							}

							m_nRevLength--;	
							ACE_OS::memcpy( m_pbyRevBuffer, m_pBuf+1, m_nRevLength );	
						}
						else					//ǰһ�ν����ɹ������ݴ�������
						{
							//ȥ����һ���ѽ������ֽ�
							m_nRevLength -= m_wdValidDataLen;
							if ( m_nRevLength > 0 )
							{
								ACE_OS::memcpy( m_pbyRevBuffer, 
									m_pBuf+m_wdValidDataLen, m_nRevLength );
							}
						}

						m_wdValidDataLen = 0;
					}

					if ( ( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH && !m_bIncompleteFrame ) )
					{
						if ( ExtractPacket( ) ) 
						{
							if ( IsLogForRxDataProc() )
							{
								sprintf( strLog, "Before ProcessRcvdPacket: ValidLen = %d, LeftLen = %d, Incomplete =%d, ExtractError=%d", 
									m_wdValidDataLen,m_nRevLength,m_bIncompleteFrame,m_bExtractError);
								pDriver->WriteLogAnyWay( strLog );
							}

							//�ҵ����ݰ������
							ProcessRcvdPacket( true );
							bPacketOk = true;		//ֻҪ�ɹ�������һ�ξͷ��سɹ�.

							if ( IsLogForRxDataProc() )
							{
								sprintf( strLog, "After ProcessRcvdPacket: ValidLen = %d, LeftLen = %d, Incomplete =%d, ExtractError=%d", 
									m_wdValidDataLen,m_nRevLength,m_bIncompleteFrame,m_bExtractError);
								pDriver->WriteLogAnyWay( strLog );
							}
						}
						else
						{
							if ( IsLogForRxDataProc() )
							{
								sprintf( strLog, "ExtractPacket failed: ValidLen = %d, LeftLen = %d, Incomplete =%d, ExtractError=%d", 
									m_wdValidDataLen,m_nRevLength,m_bIncompleteFrame,m_bExtractError);
								pDriver->WriteLogAnyWay( strLog );
							}
						}
					}
				}
			}
		}while( 0 < nReceived );

		if ( IsLogForRxDataProc() )
		{
			pDriver->WriteLogAnyWay( "RxDataProc: BuildRequest begin..." );
		}

		//////////////////////////////////////////////////////////////////////////////
		//! ��֡����������
		//OpcUa_Int32 nDataLen = 0;
		BuildRequest( m_pbySndBuffer, m_nSndLen );
		//////////////////////////////////////////////////////////////////////////////

		if ( IsLogForRxDataProc() )
		{
			sprintf( strLog,"RxDataProc: BuildRequest end. SndLen = %d", m_nSndLen );
			pDriver->WriteLogAnyWay( strLog );
		}

		//���ش�������
		return bPacketOk ? 0 : -1;
	}

	/*!
	*	\brief	�������������ݰ�
	*
	*	\param	pPacketBuffer	�����������ݰ�
	*	\param	pnPacketLength	���ݰ�����
	*
	*	\retval	bool	�ɹ�true������false
	*/
	bool CFepIEC104Prtcl::ExtractPacket()
	{
		char strLog[256];

		m_wdValidDataLen = 0;
		m_pBuf = m_pbyRevBuffer;
		m_bIncompleteFrame = false;

		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		if ( IsLog() )
		{
			char* pT = new char[MAX(m_nRevLength*3+10, 4)];
			OpcUa_Int32 i;
			for ( i = 0; i < m_nRevLength; i++ )
			{
				sprintf( pT + 3*i, " %02x", m_pBuf[i] );
			}
			sprintf( pT + 3*i, "\n" );

			//! д��־
			pDriver->WriteLog( pT );

			delete []pT;
		}

		while ( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH )
		{
			if ( m_pBuf[0] != PACK_HEAD )		//֡���ֽڷǷ�
			{
				sprintf( strLog, "֡���ֽڷǷ�: %d", m_pBuf[0] );

				//! д��־
				pDriver->WriteLog( strLog );

				m_pBuf++;
				m_nRevLength--;
				continue;
			}

			if ( m_pBuf[1] < MIN_APDU_LEN || m_pBuf[1] > MAX_APDU_LEN )	//APDU���ȷǷ�
			{
				sprintf( strLog, "APDU���ȷǷ�: %d", m_pBuf[1] );

				//! д��־
				pDriver->WriteLog( strLog );

				m_pBuf++;
				m_nRevLength--;
				continue;
			}

			if ( m_pBuf[1] == MIN_APDU_LEN )	//��֡,������S��ʽ��U��ʽ
			{
				if ( IsCtrlAreaS(m_pBuf+2) )	//S��ʽ
				{
					//! д��־
					pDriver->WriteLog( "S��ʽ" );

					//		m_bNeedSendSFrame = true;	//�ñ�־, ��Ҫ��վ����S֡

					//�޸ı��ط�����ţ�����վ���롣�������������ط���
					APCI* pAPCI = (APCI* )m_pBuf;
					WORD wdNs	= pAPCI->unCtrlArea.CtrlAreaS.Nr;
					CFepIEC104Device* pDevice = NULL;
					for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
					{
						pDevice = (CFepIEC104Device* )pDriver->GetDeviceByIndex(i);
						if ( pDevice )
						{
							//	pDevice->SetNs( wdNs );
						}
					}

					m_wdValidDataLen = MIN_APDU_LEN + 2;
					break;
				}
				else if ( IsCtrlAreaU(m_pBuf+2) )	//U��ʽ
				{
					//! д��־
					pDriver->WriteLog( "U��ʽ" );

					structCtrlAreaU* pU = (structCtrlAreaU* )( m_pBuf + 2);
					if ( pU->STARTDT_ACT )
					{
						//! д��־
						pDriver->WriteLogAnyWay( "U��ʽ: StartDT��Ч" );
						m_bNeedStartDTcon = true;
					}
					else if ( pU->STARTDT_CON )
					{
						//! д��־
						pDriver->WriteLogAnyWay( "U��ʽ: StartDTȷ��" );
						m_bNeedStartDT = false;
					}
					else if ( pU->STOPDT_ACT )
					{
						//! д��־
						pDriver->WriteLogAnyWay( "U��ʽ: StopDT��Ч" );
						m_bNeedStopDTcon = true;
					}
					else if ( pU->STOPDT_CON )
					{
						//! д��־
						pDriver->WriteLogAnyWay( "U��ʽ: StopDTȷ��" );
						m_bNeedStopDT = false;
					}
					else if ( pU->TESTFR_ACT )
					{
						//! д��־
						pDriver->WriteLog( "U��ʽ: TestFr��Ч" );
						m_bNeedSendTestcon = true;
					}
					else if ( pU->TESTFR_CON )
					{
						//! д��־
						pDriver->WriteLog( "U��ʽ: TestFrȷ��" );
						m_bNeedSendTest = false;
					}

					m_wdValidDataLen = MIN_APDU_LEN + 2;
					break;
				}
				else	//�޷���ʶ�ĸ�ʽ
				{
					//! д��־
					pDriver->WriteLog( "U��ʽ: �޷���ʶ�ĸ�ʽ" );
					m_pBuf++;
					m_nRevLength--;
					continue;
				}
			}

			if ( m_pBuf[1] >= MIN_I_APDU_LEN && IsCtrlAreaI(m_pBuf+2) )	//I��ʽ
			{
				if ( m_nRevLength < m_pBuf[1]+2 )	//I��ʽ֡���ջ�������
				{//������: ���������(HEX)Ϊ68 68 4 ..., ����һ�����ŵ�68ʱ, �ᶪ֡.
					//�����˽���취: ��ProcessRcvdPacket()�����ж�֡�Ƿ�����.

					//! д��־
					pDriver->WriteLog( "I��ʽ: I��ʽ֡���ջ�������" );

					m_bIncompleteFrame = true;
				}
				else
				{
					//��Ϊ�ɹ��յ�I��ʽ֡
					//! д��־
					pDriver->WriteLog( "I��ʽ: �ɹ��յ�I��ʽ֡" );

					m_wdValidDataLen = m_pBuf[1]+2;
				}

				break;
			}

			//�޷���ʶ�ĸ�ʽ
			//! д��־
			pDriver->WriteLog( "�޷���ʶ�ĸ�ʽ" );

			m_pBuf++;
			m_nRevLength--;
			continue;
		}

		//1.�Ƚ��ϸ��֡���õ���Ч���ݻ�����
		if ( m_wdValidDataLen > 0 )
		{
			ACE_OS::memcpy(m_sPacketBuffer, m_pBuf, m_wdValidDataLen );
		}

		//2.��ȥ������ȷ��������ݲ��֡�ע�⣬���ﲽ��1�Ͳ���2�Ĵ����ܴ��ҡ�
		OpcUa_Int32 nErrorLen = m_pBuf - m_pbyRevBuffer;
		if ( nErrorLen > 0 )
		{
			char strLog[1024]={0};
			std::string szErrorPacket = GetCommData( m_pbyRevBuffer, nErrorLen, 1 );
			snprintf( strLog, sizeof(strLog),"ȥ����������,��%d�ֽ�: %s", nErrorLen, szErrorPacket.c_str() );
			pDriver->WriteLogAnyWay( strLog );

			//m_nRevLength -= nErrorLen;
			if ( m_nRevLength > 0 )
			{				
				ACE_OS::memcpy(m_pbyRevBuffer, m_pBuf, m_nRevLength);
				m_pBuf = m_pbyRevBuffer;	//����memcpy��ɺ󣬱����m_pBufָ�븴λ��m_pbyRevBuffer��ָ�롣m_pBuf�ڽ������Ĳ����л����õ���2014.9.11 hxw
			}
			else
				m_nRevLength = 0;
		}

		m_bycmdType = 0;

		return ( m_wdValidDataLen > 0 );
	}

	// *****************************************************
	// ��������: �������������ݰ�
	// �㷨˼��: ����豸��ֹ�򲻲����ⲿ�¼�
	// �� �� ֵ: void : �ú����������κ�ֵ
	// ����˵��: 
	//         BOOL bFromLocal: �����Ǳ������Ļ����������
	// *****************************************************
	void CFepIEC104Prtcl::ProcessRcvdPacket( bool bFromLocal )
	{
		//////////////////////////////////////////////////////////////////////////////
		//! KillTimer
		m_pIOTimer->KillTimer();
		//////////////////////////////////////////////////////////////////////////////

		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		if (!pDriver)
			return;

		if (pDriver->IsStopWork())
		{
			return;
		}

		m_bExtractError			= false;
		char strLog[1024];
		APCI* pAPCI = (APCI* )m_sPacketBuffer;
		
		//���"�������"��������ȷ��֡�������ܸ��µ����ء�
		WORD wdNr = pAPCI->unCtrlArea.CtrlAreaI.Ns;	

		//���"�������"��������ȷ��֡�������ܸ��µ����ء�
		WORD wdNs = pAPCI->unCtrlArea.CtrlAreaI.Nr; 

		ASDUHead* pHead = NULL;

		CFepIEC104Device* pDevice = NULL;
		if ( IsCtrlAreaI( &pAPCI->unCtrlArea ) )	//I��ʽ֡, ���¾����豸״̬	
		{
			m_nIFrameCount++;

			m_nOfflineCount = 0;

			pHead = (ASDUHead* )(m_sPacketBuffer + sizeof(APCI));

			//���ݵ�ַ�ҵ���Ӧ���߼���Ԫ��
			OpcUa_Int32 UnitAddr = (OpcUa_Int32)pHead->wdCommAddr;
			pDevice = dynamic_cast<CFepIEC104Device*>( 
				pDriver->GetDeviceByAddr( UnitAddr ) );
			if ( !pDevice )
				return;

/*	//�ŵ����棬��ֹ����ʧ�ܵĴ����ģ����ְ��豸�������ˡ�2014.9.11
			//����װ������
			if( bFromLocal )
			{
				pDevice->GenNextNr();

				//��ʱ����S֡
				m_bNeedSendSFrame = pDevice->IsNeedSendSFrame();

				pDevice->SetOnLine( DEVICE_ONLINE );	//�����豸�յ�I֡һ����Ϊ����
				pDevice->m_byTxTimes = 0;
				pDevice->m_byRxTimes++;
			}

			//��λ���ӱ�־
			m_bNeedConnected = false;
*/
		}
		else if ( IsCtrlAreaS( &pAPCI->unCtrlArea ) 
			|| IsCtrlAreaU( &pAPCI->unCtrlArea ) ) //S��ʽ��U��ʽ֡, ���������豸״̬
		{
			if ( bFromLocal )
			{
				m_nOfflineCount = 0;

				CIODeviceBase* pTempDevice = NULL;	//�ֲ�����, ��Ӱ�����Ĳ���.
				for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
				{
					pTempDevice = pDriver->GetDeviceByIndex(i);
					if ( pTempDevice )
					{
						if ( !pTempDevice->IsHot() )	//���豸�յ�S֡����U֡����Ϊ���ߣ������豸��Hot�������յ�I֡����Ϊ����
						{
							pTempDevice->SetOnLine( DEVICE_ONLINE );
						}
						pTempDevice->m_byTxTimes = 0;
						pTempDevice->m_byRxTimes++;
					}
				}

				//��λ���ӱ�־
				m_bNeedConnected = false;
			}
		}

		if ( !pDevice || !pHead )
		{
			pDriver->WriteLog( "��I��ʽ֡" );
			return;
		}

		if ( !pDevice->IsHot() )
		{
			pDriver->WriteLog( "�ӻ�������I��ʽ֡" );
			return;
		}

		//�����ݿ�
		if ( !pDevice->IsSimulate() )	//����豸����ģ��״̬���������豸�����ݲ������ݿ⡣
		{
			//S��ʽ��U��ʽ֡���ڽ���ʱ����, �˴�����. ֻ����I��ʽ֡.
			if ( !IsCtrlAreaI( &pAPCI->unCtrlArea ) )	
			{
				pDriver->WriteLog( "��I��ʽ֡" );
				return;
			}

			////! ʱ��
			//ACE_Time_Value cur_tv = ACE_OS::gettimeofday();
			//OpcUa_Int64 nTime = ((OpcUa_Int64)cur_tv.sec())*1000 + cur_tv.usec()/1000;

			OpcUa_Int32 i;
			OpcUa_Int32 nNum;
			float ftempVal = 0;
			OpcUa_Byte byOldVal = 0;
			std::string szTagName = "";
			OpcUa_Int32 iCount=0;
			switch( pHead->byType )	//���ͱ�ʶ
			{
			case M_SP_NA_1:		//����ʱ��ĵ�����Ϣ
				{
					pDriver->WriteLog( "����ʱ��ĵ�����Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structSDINoTimeNoInfoAddr* pPoOpcUa_Int = (structSDINoTimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structSDINoTimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						if( !IsValidSDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( *pPoOpcUa_Int ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSIQ.SPI;

								if( pDevice->IsDIValueChange( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "����ʱ��ĵ�����Ϣ: �豸'%s',������ǩ:%s,��Ϣ��ַ%d, ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(), pStartInfoAddr->wdInfoAddr+i,byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_DI, 
									pStartInfoAddr->wdInfoAddr+i, 
									&OpcUa_Bytemp, 1, 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structSDINoTime* pPoOpcUa_Int = (structSDINoTime* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structSDINoTime)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						if( !IsValidSDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( *pPoOpcUa_Int ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSIQ.SPI;

								if( pDevice->IsDIValueChange( MACS_IEC104_DI, pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "����ʱ��ĵ�����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d, ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(), pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_DI, 
									pPoOpcUa_Int->wdInfoAddr, 
									&OpcUa_Bytemp, 1, 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_SP_TA_1:		//��ʱ��ĵ�����Ϣ
				{
					pDriver->WriteLog( "��ʱ��ĵ�����Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structSDI_Time24NoInfoAddr* pPoOpcUa_Int = (structSDI_Time24NoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structSDI_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						if( !IsValidSDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( pPoOpcUa_Int->sSDINoTime ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSDINoTime.sSIQ.SPI;

								if( pDevice->IsDIValueChange( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "��ʱ��ĵ�����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(), pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_DI, 
									pStartInfoAddr->wdInfoAddr+i, 
									&OpcUa_Bytemp, 1, llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structSDI_Time24* pPoOpcUa_Int = (structSDI_Time24* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structSDI_Time24)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						if( !IsValidSDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( pPoOpcUa_Int->sSDINoTime ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSDINoTime.sSIQ.SPI;

								if( pDevice->IsDIValueChange( MACS_IEC104_DI, pPoOpcUa_Int->sSDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "��ʱ��ĵ�����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(), pPoOpcUa_Int->sSDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_DI, 
									pPoOpcUa_Int->sSDINoTime.wdInfoAddr, 
									&OpcUa_Bytemp, 1, llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_DP_NA_1:		//����ʱ���˫����Ϣ
				{
					pDriver->WriteLog( "����ʱ���˫����Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structDDINoTimeNoInfoAddr* pPoOpcUa_Int = (structDDINoTimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structDDINoTimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						if( !IsValidDDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( *pPoOpcUa_Int ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDIQ.DPI;

								if ( pDevice->IsExitTag( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i ) )
								{
									if( pDevice->IsDIValueChange( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
									{
										sprintf( strLog, "����ʱ���˫����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
											pDevice->GetName().c_str(), szTagName.c_str(),pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
										pDriver->WriteLogAnyWay(strLog);
										iCount++;
									}
								}
								else if( pDevice->IsAIValueChange( MACS_IEC104_AI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "����ʱ���˫����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(),pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_DI, 
									pStartInfoAddr->wdInfoAddr+i, 
									&OpcUa_Bytemp, 1, 0, true );
								
								//Add by hxw in 2013.3.19
								ftempVal = (float)OpcUa_Bytemp;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i,
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDDINoTime* pPoOpcUa_Int = (structDDINoTime* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structDDINoTime)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						if( !IsValidDDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( *pPoOpcUa_Int ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDIQ.DPI;

								if ( pDevice->IsExitTag( MACS_IEC104_DI, pPoOpcUa_Int->wdInfoAddr ) )
								{
									if( pDevice->IsDIValueChange( MACS_IEC104_DI, pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
									{
										sprintf( strLog, "����ʱ���˫����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
											pDevice->GetName().c_str(), szTagName.c_str(), pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp );
										pDriver->WriteLogAnyWay(strLog);
										iCount++;
									}
								}
								else if( pDevice->IsAIValueChange( MACS_IEC104_AI, pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "����ʱ���˫����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(),pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}


								pDevice->SetVal( MACS_IEC104_DI, 
									pPoOpcUa_Int->wdInfoAddr, 
									&OpcUa_Bytemp, 1, 0, true );
								
								//Add by hxw in 2013.3.19
								ftempVal = (float)OpcUa_Bytemp;
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->wdInfoAddr,
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );

							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_DP_TA_1:		//��ʱ���˫����Ϣ
				{
					pDriver->WriteLog( "��ʱ���˫����Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structDDI_Time24NoInfoAddr* pPoOpcUa_Int = (structDDI_Time24NoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structDDI_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						if( !IsValidDDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( pPoOpcUa_Int->sDDINoTime ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDDINoTime.sDIQ.DPI;

								if ( pDevice->IsExitTag( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i ) )
								{
									if( pDevice->IsDIValueChange( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
									{
										sprintf( strLog, "��ʱ���˫����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
											pDevice->GetName().c_str(), szTagName.c_str(), pStartInfoAddr->wdInfoAddr+i,byOldVal, OpcUa_Bytemp );
										pDriver->WriteLogAnyWay(strLog);
										iCount++;
									}
								}
								else if( pDevice->IsAIValueChange( MACS_IEC104_AI, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "��ʱ���˫����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(),pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}


								pDevice->SetVal( MACS_IEC104_DI, 
									pStartInfoAddr->wdInfoAddr+i, 
									&OpcUa_Bytemp, 1, llTime, true );
								
								//Add by hxw in 2013.3.19
								ftempVal = (float)OpcUa_Bytemp;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i,
									(OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDDI_Time24* pPoOpcUa_Int = (structDDI_Time24* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structDDI_Time24)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							break;
						}

						if( !IsValidDDI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( pPoOpcUa_Int->sDDINoTime ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDDINoTime.sDIQ.DPI;

								if ( pDevice->IsExitTag( MACS_IEC104_DI, pPoOpcUa_Int->sDDINoTime.wdInfoAddr ) )
								{
									if( pDevice->IsDIValueChange( MACS_IEC104_DI, pPoOpcUa_Int->sDDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
									{
										sprintf( strLog, "��ʱ���˫����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
											pDevice->GetName().c_str(), szTagName.c_str(), pPoOpcUa_Int->sDDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp );
										pDriver->WriteLogAnyWay(strLog);
										iCount++;
									}
								}
								else if( pDevice->IsAIValueChange( MACS_IEC104_AI, pPoOpcUa_Int->sDDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "��ʱ���˫����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(),pPoOpcUa_Int->sDDINoTime.wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}


								pDevice->SetVal( MACS_IEC104_DI, 
									pPoOpcUa_Int->sDDINoTime.wdInfoAddr, 
									&OpcUa_Bytemp, 1,llTime, true );
								
								//Add by hxw in 2013.3.19
								ftempVal = (float)OpcUa_Bytemp;
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->sDDINoTime.wdInfoAddr,
									(OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ST_NA_1:		//����ʱ��Ĳ�λ����Ϣ
				{
					pDriver->WriteLog( "����ʱ��Ĳ�λ����Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structVTINoInfoAddr* pPoOpcUa_Int = (structVTINoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structVTINoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						if( !IsValidVTI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->sVTI);
								OpcUa_Byte OpcUa_Bytemp = (OpcUa_Byte)ftempVal;

								if( pDevice->IsAIValueChange( MACS_IEC104_BWZ, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "����ʱ��Ĳ�λ����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(),pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_BWZ, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structVTI* pPoOpcUa_Int = (structVTI* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structVTI)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						if( !IsValidVTI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->sVTI);
								OpcUa_Byte OpcUa_Bytemp = (OpcUa_Byte)ftempVal;

								if( pDevice->IsAIValueChange( MACS_IEC104_BWZ, pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "����ʱ��Ĳ�λ����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(),pPoOpcUa_Int->wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_BWZ, 
									pPoOpcUa_Int->wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ST_TA_1:		//��ʱ��Ĳ�λ����Ϣ
				{
					pDriver->WriteLog( "��ʱ��Ĳ�λ����Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structVTI_Time24NoInfoAddr* pPoOpcUa_Int = (structVTI_Time24NoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structVTI_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						if( !IsValidVTI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( pPoOpcUa_Int->ssVTI ) )	//��Ч
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->ssVTI.sVTI);
								OpcUa_Byte OpcUa_Bytemp = (OpcUa_Byte)ftempVal;

								if( pDevice->IsAIValueChange( MACS_IEC104_BWZ, pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "��ʱ��Ĳ�λ����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(),pStartInfoAddr->wdInfoAddr+i, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_BWZ, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structVTI_Time24* pPoOpcUa_Int = (structVTI_Time24* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structVTI_Time24)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						if( !IsValidVTI( pPoOpcUa_Int, nNum ) )
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( pPoOpcUa_Int->ssVTI ) )	//��Ч
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->ssVTI.sVTI);
								OpcUa_Byte OpcUa_Bytemp = (OpcUa_Byte)ftempVal;

								if( pDevice->IsAIValueChange( MACS_IEC104_BWZ, pPoOpcUa_Int->ssVTI.wdInfoAddr, byOldVal, OpcUa_Bytemp, szTagName ) )
								{
									sprintf( strLog, "��ʱ��Ĳ�λ����Ϣ: �豸'%s'������ǩ:%s,��Ϣ��ַ%d ����%d��%d�ı�λ",
										pDevice->GetName().c_str(), szTagName.c_str(),pPoOpcUa_Int->ssVTI.wdInfoAddr, byOldVal, OpcUa_Bytemp );
									pDriver->WriteLogAnyWay(strLog);
									iCount++;
								}

								pDevice->SetVal( MACS_IEC104_BWZ, 
									pPoOpcUa_Int->ssVTI.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_BO_NA_1:		//32���ش�
				{
					pDriver->WriteLog( "32���ش�" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRTSNoInfoAddr* pPoOpcUa_Int = (structRTSNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRTSNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRTS* pPoOpcUa_Int = (structRTS* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRTS)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, 
									pPoOpcUa_Int->wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_BO_TA_1:		//��ʱ���32���ش�
				{
					pDriver->WriteLog( "��ʱ���32���ش�" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRTS_Time24NoInfoAddr* pPoOpcUa_Int = (structRTS_Time24NoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRTS_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( pPoOpcUa_Int->sRTS ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRTS.dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRTS_Time24* pPoOpcUa_Int = (structRTS_Time24* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRTS_Time24)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( pPoOpcUa_Int->sRTS ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRTS.dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, 
									pPoOpcUa_Int->sRTS.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_NA_1:		//����ֵ, ��һ��ֵ
				{
					pDriver->WriteLog( "����ֵ, ��һ��ֵ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bitNoInfoAddr* pPoOpcUa_Int = (structRT16bitNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bitNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit* pPoOpcUa_Int = (structRT16bit* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRT16bit)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TA_1:		//����ֵ, ��ʱ��Ĺ�һ��ֵ
				{
					pDriver->WriteLog( "����ֵ, ��ʱ��Ĺ�һ��ֵ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bit_Time24NoInfoAddr* pPoOpcUa_Int = 
							(structRT16bit_Time24NoInfoAddr* )(m_sPacketBuffer 
							+ sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bit_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sRT.sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit_Time24* pPoOpcUa_Int = (structRT16bit_Time24* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRT16bit_Time24)+sizeof(APCI)
							+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sRT.sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->sRT.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_NB_1:		//����ֵ, ��Ȼ�ֵ
				{
					pDriver->WriteLog( "����ֵ, ��Ȼ�ֵ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bitNoInfoAddr* pPoOpcUa_Int = (structRT16bitNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bitNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit* pPoOpcUa_Int = (structRT16bit* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRT16bit)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TB_1:		//����ֵ, ��ʱ��ı�Ȼ�ֵ
				{
					pDriver->WriteLog( "����ֵ, ��ʱ��ı�Ȼ�ֵ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bit_Time24NoInfoAddr* pPoOpcUa_Int 
							= (structRT16bit_Time24NoInfoAddr* )(m_sPacketBuffer 
							+ sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bit_Time24NoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 
									llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit_Time24* pPoOpcUa_Int = (structRT16bit_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRT16bit_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, pPoOpcUa_Int->sRT.wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_NC_1:		//����ֵ, �̸�����
				{
					pDriver->WriteLog( "����ֵ, �̸�����" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structRT32bitNoInfoAddr* pPoOpcUa_Int = (structRT32bitNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structRT32bitNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->fValue;
								pDevice->SetVal( MACS_IEC104_AI, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT32bit* pPoOpcUa_Int = (structRT32bit* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRT32bit)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->fValue;
								pDevice->SetVal( MACS_IEC104_AI, pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TC_1:		//����ֵ,��ʱ��̸�����
				{
					pDriver->WriteLog( "����ֵ,��ʱ��̸�����" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structRT32bit_Time24NoInfoAddr* pPoOpcUa_Int = (structRT32bit_Time24NoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structRT32bit_Time24NoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.fValue;
								pDevice->SetVal( MACS_IEC104_AI, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float),llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT32bit_Time24* pPoOpcUa_Int = (structRT32bit_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRT32bit_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.fValue;
								pDevice->SetVal( MACS_IEC104_AI, pPoOpcUa_Int->sRT.wdInfoAddr,  (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_IT_NA_1:		//�ۻ���
				{
					pDriver->WriteLog( "�ۻ���" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structDNLJNoInfoAddr* pPoOpcUa_Int = (structDNLJNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structDNLJNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDNLJ* pPoOpcUa_Int = (structDNLJ* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structDNLJ)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_IT_TA_1:		//��ʱ����ۻ���
				{
					pDriver->WriteLog( "��ʱ����ۻ���" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structDNLJ_Time24NoInfoAddr* pPoOpcUa_Int = (structDNLJ_Time24NoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structDNLJ_Time24NoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( pPoOpcUa_Int->sDNLJ ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sDNLJ.Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDNLJ_Time24* pPoOpcUa_Int = (structDNLJ_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structDNLJ_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( pPoOpcUa_Int->sDNLJ ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sDNLJ.Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, pPoOpcUa_Int->sDNLJ.wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EP_TA_1:		//��ʱ��ļ̵籣��װ���¼�
				{
					pDriver->WriteLog( "��ʱ��ļ̵籣��װ���¼�" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structJDBH_Time24NoInfoAddr* pPoOpcUa_Int = (structJDBH_Time24NoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structJDBH_Time24NoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBH( pPoOpcUa_Int->sJDBHNoTime ) )	//��Ч
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Int32)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
								ftempVal = (float)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES;
								pDevice->SetVal( MACS_IEC104_JDBH, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structJDBH_Time24* pPoOpcUa_Int = (structJDBH_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structJDBH_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int64 llTime;
						llTime = Time24Bit2MACSINT64( pPoOpcUa_Int->sTime );
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBH( pPoOpcUa_Int->sJDBHNoTime ) )	//��Ч
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pPoOpcUa_Int->sJDBHNoTime.wdInfoAddr, (OpcUa_Int32)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
								ftempVal = (float)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES;
								pDevice->SetVal( MACS_IEC104_JDBH, pPoOpcUa_Int->sJDBHNoTime.wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), llTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EP_TB_1:		//��ʱ��ļ̵籣��װ�ó��������¼�
				{
					pDriver->WriteLog( "��ʱ��ļ̵籣��װ�ó��������¼�" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structJDBHGroupAct_Time24NoInfoAddr* pPoOpcUa_Int = (structJDBHGroupAct_Time24NoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structJDBHGroupAct_Time24NoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupAct( pPoOpcUa_Int->sJDBHGroupActNoTime ) )	//��Ч
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupActNoTime.byVal, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structJDBHGroupAct_Time24* pPoOpcUa_Int = (structJDBHGroupAct_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structJDBHGroupAct_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupAct( pPoOpcUa_Int->sJDBHGroupActNoTime ) )	//��Ч
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pPoOpcUa_Int->sJDBHGroupActNoTime.wdInfoAddr, (OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupActNoTime.byVal, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EP_TC_1:		//��ʱ��ļ̵籣��װ�ó��������·��Ϣ
				{
					pDriver->WriteLog( "��ʱ��ļ̵籣��װ�ó��������·��Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structJDBHGroupOutput_Time24NoInfoAddr* pPoOpcUa_Int = (structJDBHGroupOutput_Time24NoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structJDBHGroupOutput_Time24NoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupOutput( pPoOpcUa_Int->sJDBHGroupOutputNoTime ) )	//��Ч
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupOutputNoTime.byVal, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structJDBHGroupOutput_Time24* pPoOpcUa_Int = (structJDBHGroupOutput_Time24* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structJDBHGroupOutput_Time24)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupOutput( pPoOpcUa_Int->sJDBHGroupOutputNoTime ) )	//��Ч
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, pPoOpcUa_Int->sJDBHGroupOutputNoTime.wdInfoAddr, (OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupOutputNoTime.byVal, Time24bitToSystime( pPoOpcUa_Int->sTime ) );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_PS_NA_1:		//����λ����ĳ��鵥����Ϣ
				{
					pDriver->WriteLog( "����λ����ĳ��鵥����Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structDIGroupNoInfoAddr* pPoOpcUa_Int = (structDIGroupNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structDIGroupNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int32 k;
						WORD Addr;
						OpcUa_Byte Val;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDIGroup( *pPoOpcUa_Int ) )
							{
								for ( k = 0; k < 16; k++ )
								{
									Addr = pStartInfoAddr->wdInfoAddr+k;
									if ( Addr <= DI_END_ADDR )	//��Ч
									{
										Val = (pPoOpcUa_Int->sSCD.wdST>>k)&0X01;
										pDevice->SetVal( MACS_IEC104_DI, Addr, &Val, 1, 0, true );
									}
								}
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDIGroup* pPoOpcUa_Int = (structDIGroup* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structDIGroup)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int32 k;
						WORD Addr;
						OpcUa_Byte Val;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDIGroup( *pPoOpcUa_Int ) )
							{
								for ( k = 0; k < 16; k++ )
								{
									Addr = pPoOpcUa_Int->wdInfoAddr+k;
									if ( Addr <= DI_END_ADDR )	//��Ч
									{
										Val = (pPoOpcUa_Int->sSCD.wdST>>k)&0X01;
										pDevice->SetVal( MACS_IEC104_DI, Addr, &Val, 1, 0, true );
									}
								}
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_ND_1:		//����ֵ, ����Ʒ�������ʵĹ�һ��ֵ
				{
					pDriver->WriteLog( "����ֵ, ����Ʒ�������ʵĹ�һ��ֵ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structRT16bitNoQDSNoInfoAddr* pPoOpcUa_Int = (structRT16bitNoQDSNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structRT16bitNoQDSNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( pStartInfoAddr->wdInfoAddr+i >= AI_START_ADDR && pStartInfoAddr->wdInfoAddr+i <= AI_END_ADDR  )	//��Ч
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_AI, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bitNoQDS* pPoOpcUa_Int = (structRT16bitNoQDS* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRT16bitNoQDS)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= AI_START_ADDR && pPoOpcUa_Int->wdInfoAddr <= AI_END_ADDR  )	//��Ч
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_AI, pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_SP_TB_1:		//��ʱ��CP56Time2a�ĵ�����Ϣ
				{
					pDriver->WriteLog( "��ʱ��CP56Time2a�ĵ�����Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structSDI_TimeNoInfoAddr* pPoOpcUa_Int = (structSDI_TimeNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structSDI_TimeNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}
						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( pPoOpcUa_Int->sSDINoTime ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSDINoTime.sSIQ.SPI;
								pDevice->SetVal( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, &OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structSDI_Time* pPoOpcUa_Int = (structSDI_Time* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structSDI_Time)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidSDI( pPoOpcUa_Int->sSDINoTime ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sSDINoTime.sSIQ.SPI;
								pDevice->SetVal( MACS_IEC104_DI, pPoOpcUa_Int->sSDINoTime.wdInfoAddr, &OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_DP_TB_1:		//��ʱ��CP56Time2a��˫����Ϣ
				{
					pDriver->WriteLog( "��ʱ��CP56Time2a��˫����Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structDDI_TimeNoInfoAddr* pPoOpcUa_Int = (structDDI_TimeNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structDDI_TimeNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( pPoOpcUa_Int->sDDINoTime ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDDINoTime.sDIQ.DPI;
								pDevice->SetVal( MACS_IEC104_DI, pStartInfoAddr->wdInfoAddr+i, &OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDDI_Time* pPoOpcUa_Int = (structDDI_Time* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structDDI_Time)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDDI( pPoOpcUa_Int->sDDINoTime ) )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sDDINoTime.sDIQ.DPI;
								pDevice->SetVal( MACS_IEC104_DI, pPoOpcUa_Int->sDDINoTime.wdInfoAddr, &OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ST_TB_1:		//��ʱ��Ĳ�λ����Ϣ
				{
					pDriver->WriteLog( "��ʱ��Ĳ�λ����Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structVTI_TimeNoInfoAddr* pPoOpcUa_Int = (structVTI_TimeNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structVTI_TimeNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( pPoOpcUa_Int->ssVTI ) )	//��Ч
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->ssVTI.sVTI);
								pDevice->SetVal( MACS_IEC104_BWZ, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structVTI_Time* pPoOpcUa_Int = (structVTI_Time* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structVTI_Time)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int64 lCurTime;						
						lCurTime = ASDUTIME2MACSINT64( pPoOpcUa_Int->sTime ) ;
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidVTI( pPoOpcUa_Int->ssVTI ) )	//��Ч
							{
								ftempVal = (float)GetVTIVal(pPoOpcUa_Int->ssVTI.sVTI);
								pDevice->SetVal( MACS_IEC104_BWZ, pPoOpcUa_Int->ssVTI.wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_BO_TB_1:		//��ʱ��CP56Time2a��32���ش�
				{
					pDriver->WriteLog( "��ʱ��CP56Time2a��32���ش�" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						structRTS_TimeNoInfoAddr* pPoOpcUa_Int = (structRTS_TimeNoInfoAddr* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) + sizeof(structInfoAddr) );
						if ( nNum*sizeof(structRTS_TimeNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}


						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( pPoOpcUa_Int->sRTS ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRTS.dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, pStartInfoAddr->wdInfoAddr+i, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRTS_Time* pPoOpcUa_Int = (structRTS_Time* )(m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRTS_Time)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRTS( pPoOpcUa_Int->sRTS ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRTS.dwRTS;
								pDevice->SetVal( MACS_IEC104_RTS, pPoOpcUa_Int->sRTS.wdInfoAddr, (OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TD_1:		//����ֵ, ��ʱ��CP56Time2a�Ĺ�һ��ֵ
				{
					pDriver->WriteLog( "����ֵ, ��ʱ��CP56Time2a�Ĺ�һ��ֵ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bit_TimeNoInfoAddr* pPoOpcUa_Int = (structRT16bit_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) 
							+ sizeof(ASDUHead) + sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bit_TimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sRT.sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit_Time* pPoOpcUa_Int = (structRT16bit_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRT16bit_Time)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sRT.sNVA);
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->sRT.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TE_1:		//����ֵ, ��ʱ��CP56Time2a�ı�Ȼ�ֵ
				{
					pDriver->WriteLog( "����ֵ, ��ʱ��CP56Time2a�ı�Ȼ�ֵ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT16bit_TimeNoInfoAddr* pPoOpcUa_Int = (structRT16bit_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead)
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT16bit_TimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT16bit_Time* pPoOpcUa_Int = (structRT16bit_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structRT16bit_Time)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT16bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.sSVA.Val;
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->sRT.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_ME_TF_1:		//����ֵ, ��ʱ��CP56Time2a�Ķ̸�����
				{
					pDriver->WriteLog( "����ֵ, ��ʱ��CP56Time2a�Ķ̸�����" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structRT32bit_TimeNoInfoAddr* pPoOpcUa_Int 
							= (structRT32bit_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structRT32bit_TimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.fValue;
								pDevice->SetVal( MACS_IEC104_AI, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structRT32bit_Time* pPoOpcUa_Int = (structRT32bit_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structRT32bit_Time)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidRT32bit( pPoOpcUa_Int->sRT ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sRT.fValue;
								pDevice->SetVal( MACS_IEC104_AI, 
									pPoOpcUa_Int->sRT.wdInfoAddr, 
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_IT_TB_1:		//��ʱ��CP56Time2a���ۼ���
				{
					pDriver->WriteLog( "��ʱ��CP56Time2a���ۼ���" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structDNLJ_TimeNoInfoAddr* pPoOpcUa_Int = (structDNLJ_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structDNLJ_TimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( pPoOpcUa_Int->sDNLJ ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sDNLJ.Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structDNLJ_Time* pPoOpcUa_Int = (structDNLJ_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structDNLJ_Time)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidDNLJ( pPoOpcUa_Int->sDNLJ ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sDNLJ.Val;
								pDevice->SetVal( MACS_IEC104_DNLJ, 
									pPoOpcUa_Int->sDNLJ.wdInfoAddr,  
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EP_TD_1:		//��ʱ��CP56Time2a�ļ̵籣��װ���¼�
				{
					pDriver->WriteLog( "��ʱ��CP56Time2a�ļ̵籣��װ���¼�" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structJDBH_TimeNoInfoAddr* pPoOpcUa_Int = (structJDBH_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structJDBH_TimeNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}
						OpcUa_Int64 lCurTime;
						lCurTime = ASDUTIME2MACSINT64(pPoOpcUa_Int->sTime);
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBH( pPoOpcUa_Int->sJDBHNoTime ) )	//��Ч
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, 
								//	pStartInfoAddr->wdInfoAddr+i, 
								//	(OpcUa_Int32)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES, 
								//	ASDUTimeToSystime( 0/*pPoOpcUa_Int->sTime*/ ) );

								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sJDBHNoTime.sSEP.ES;
								pDevice->SetVal( MACS_IEC104_JDBH, 
									pStartInfoAddr->wdInfoAddr+i, &OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structJDBH_Time* pPoOpcUa_Int = (structJDBH_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structJDBH_Time)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						OpcUa_Int64 lCurTime;
						lCurTime = ASDUTIME2MACSINT64(pPoOpcUa_Int->sTime);
						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBH( pPoOpcUa_Int->sJDBHNoTime ) )	//��Ч
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, 
								//	pPoOpcUa_Int->sJDBHNoTime.wdInfoAddr, 
								//	(OpcUa_Int32)pPoOpcUa_Int->sJDBHNoTime.sSEP.ES, 
								//	ASDUTimeToSystime( 0/*pPoOpcUa_Int->sTime*/ ) );
								OpcUa_Byte OpcUa_Bytemp = pPoOpcUa_Int->sJDBHNoTime.sSEP.ES;
								pDevice->SetVal( MACS_IEC104_JDBH, 
									pPoOpcUa_Int->sJDBHNoTime.wdInfoAddr, 
									&OpcUa_Bytemp, 1, lCurTime, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EP_TF_1:		//��ʱ��CP56Time2a�ļ̵籣��װ�ó��������·��Ϣ
				{
					pDriver->WriteLog( "��ʱ��CP56Time2a�ļ̵籣��װ�ó��������·��Ϣ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structJDBHGroupOutput_TimeNoInfoAddr* pPoOpcUa_Int 
							= (structJDBHGroupOutput_TimeNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structJDBHGroupOutput_TimeNoInfoAddr)
							+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupOutput( pPoOpcUa_Int->sJDBHGroupOutputNoTime ) )	//��Ч
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, 
								//	pStartInfoAddr->wdInfoAddr+i, 
								//	(OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupOutputNoTime.byVal, 
								//	ASDUTimeToSystime( 0/*pPoOpcUa_Int->sTime*/ ) );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structJDBHGroupOutput_Time* pPoOpcUa_Int = (structJDBHGroupOutput_Time* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structJDBHGroupOutput_Time)+sizeof(APCI)
							+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidJDBHGroupOutput( pPoOpcUa_Int->sJDBHGroupOutputNoTime ) )	//��Ч
							{
								//pDevice->SendSOE( MACS_IEC104_JDBH, 
								//	pPoOpcUa_Int->sJDBHGroupOutputNoTime.wdInfoAddr, 
								//	(OpcUa_Int32)pPoOpcUa_Int->sJDBHGroupOutputNoTime.byVal, 
								//	ASDUTimeToSystime( 0/*pPoOpcUa_Int->sTime*/ ) );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SC_NA_1:		//����ң������
			case C_DC_NA_1:		//˫��ң������
			case C_RC_NA_1:		//��������
				{
					if ( pHead->byType == C_SC_NA_1 )
					{
						sprintf( strLog, "����ң������, ����ԭ��: %d", pHead->byTxReason );
					}
					else if ( pHead->byType == C_DC_NA_1 )
					{
						sprintf( strLog, "˫��ң������, ����ԭ��: %d", pHead->byTxReason );
					}
					else
					{
						sprintf( strLog, "��������, ����ԭ��: %d", pHead->byTxReason );
					}
					pDriver->WriteLog( strLog );

					unionInfoElement* pPoOpcUa_Int = (unionInfoElement* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(unionInfoElement)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= DO_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= DO_END_ADDR  )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//�÷��ŵ�Ϊ1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SE_NA_1:		//�趨����, ��һ��ֵ
			case C_SE_NB_1:		//�趨����, ��Ȼ�ֵ
				{
					if ( pHead->byType == C_SE_NA_1 )
					{
						sprintf( strLog, "�趨����, ��һ��ֵ, ����ԭ��: %d", 
							pHead->byTxReason );
					}
					else if ( pHead->byType == C_SE_NA_1 )
					{
						sprintf( strLog, "�趨����, ��Ȼ�ֵ, ����ԭ��: %d", 
							pHead->byTxReason );
					}
					pDriver->WriteLog( strLog );

					structRC16bit* pPoOpcUa_Int = (structRC16bit* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC16bit)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= AI_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= AI_END_ADDR  )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//�÷��ŵ�Ϊ1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SE_NC_1:		//�趨����, �̸�����
				{
					sprintf( strLog, "�趨����, �̸�����, ����ԭ��: %d", 
						pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structRC32bit* pPoOpcUa_Int = (structRC32bit* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC32bit)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							//	if ( pPoOpcUa_Int->wdInfoAddr >= AI_START_ADDR 
							//		&& pPoOpcUa_Int->wdInfoAddr <= AI_END_ADDR  )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//�÷��ŵ�Ϊ1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_BO_NA_1:		//�趨����, 32���ش�
				{
					sprintf( strLog, "�趨����, 32���ش�, ����ԭ��: %d", 
						pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structRC32bitstring* pPoOpcUa_Int = (structRC32bitstring* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC32bitstring)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= RTS_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= RTS_END_ADDR  )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//�÷��ŵ�Ϊ1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SC_TA_1:		//��ʱ��CP56Time2a�ĵ�����
			case C_DC_TA_1:		//��ʱ��CP56Time2a��˫����
			case C_RC_TA_1:		//��ʱ��CP56Time2a����������
				{
					if ( pHead->byType == C_SC_TA_1 )
					{
						sprintf( strLog, "��ʱ��CP56Time2a�ĵ�����, ����ԭ��: %d", 
							pHead->byTxReason );
					}
					else if ( pHead->byType == C_DC_TA_1 )
					{
						sprintf( strLog, "��ʱ��CP56Time2a��˫����, ����ԭ��: %d", 
							pHead->byTxReason );
					}
					else
					{
						sprintf( strLog, "��ʱ��CP56Time2a����������, ����ԭ��: %d", 
							pHead->byTxReason );
					}
					pDriver->WriteLog( strLog );

					unionInfoElementTime* pPoOpcUa_Int = (unionInfoElementTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(unionInfoElementTime)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= DO_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= DO_END_ADDR  )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//�÷��ŵ�Ϊ1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SE_TA_1:		//��ʱ��CP56Time2a���趨ֵ����,��һ��ֵ
			case C_SE_TB_1:		//��ʱ��CP56Time2a���趨ֵ����,��Ȼ�ֵ
				{
					if ( pHead->byType == C_SE_NA_1 )
					{
						sprintf( strLog, 
							"��ʱ��CP56Time2a���趨ֵ����, ��һ��ֵ, ����ԭ��: %d", 
							pHead->byTxReason );
					}
					else if ( pHead->byType == C_SE_NA_1 )
					{
						sprintf( strLog, 
							"��ʱ��CP56Time2a���趨ֵ����, ��Ȼ�ֵ, ����ԭ��: %d", 
							pHead->byTxReason );
					}
					pDriver->WriteLog( strLog );

					structRC16bitTime* pPoOpcUa_Int = (structRC16bitTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC16bitTime)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= AI_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= AI_END_ADDR  )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//�÷��ŵ�Ϊ1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_SE_TC_1:		//��ʱ��CP56Time2a���趨ֵ����,�̸�����
				{
					sprintf( strLog,"��ʱ��CP56Time2a���趨ֵ����, �̸�����, ����ԭ��: %d", 
						pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structRC32bitTime* pPoOpcUa_Int = (structRC32bitTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC32bitTime)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							//	if ( pPoOpcUa_Int->wdInfoAddr >= AI_START_ADDR && pPoOpcUa_Int->wdInfoAddr <= AI_END_ADDR  )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//�÷��ŵ�Ϊ1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case C_BO_TA_1:		//��ʱ��CP56Time2a��32���ش�
				{
					sprintf( strLog, "��ʱ��CP56Time2a��32���ش�, ����ԭ��: %d", 
						pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structRC32bitstringTime* pPoOpcUa_Int = (structRC32bitstringTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

					nNum = pHead->byLimit;
					if ( nNum*sizeof(structRC32bitstringTime)+sizeof(APCI)+sizeof(ASDUHead) != m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon 
						|| pHead->byTxReason == CAUSE_actterm )
					{
						for ( i = 0; i < nNum; i++ )
						{
							if ( pPoOpcUa_Int->wdInfoAddr >= RTS_START_ADDR 
								&& pPoOpcUa_Int->wdInfoAddr <= RTS_END_ADDR  )	//��Ч
							{
								OpcUa_Byte OpcUa_Bytempt = 1;
								pDevice->SetVal( MACS_IEC104_DOF, 0, &OpcUa_Bytempt, 1, 0, true );	//�÷��ŵ�Ϊ1
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case M_EI_NA_1:		//��ʼ������
				{
					structInit* pPoOpcUa_Int = (structInit* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					sprintf( strLog, "��ʼ������(COI=%d, BS=%d)", 
						pPoOpcUa_Int->sCOI.COI, pPoOpcUa_Int->sCOI.BS );
					pDriver->WriteLog( strLog );

					nNum = pHead->byLimit;
					if ( nNum*sizeof(structInit)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����A" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( nNum != 1 || pHead->byTxReason != CAUSE_init )
					{
						pDriver->WriteLog( "֡����B" );
						m_bExtractError = true;	//��������
						break;
					}

					switch( pPoOpcUa_Int->sCOI.COI ) 
					{
					case 0:		//���ص�Դ����
						break;
					case 1:		//�����ֶ���λ
						break;
					case 2:		//Զ����λ
						break;
					default:
						break;
					}

					if ( pPoOpcUa_Int->sCOI.BS )	//�ı䵱�ز�����ĳ�ʼ��
					{
					}
					else	//δ�ı䵱�ز����ĳ�ʼ��
					{
					}
				}
				break;
			case C_IC_NA_1:		//�ٻ�����
				{
					sprintf( strLog, "�ٻ�����, ����ԭ��: %d", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structWholeQuery* pPoOpcUa_Int = (structWholeQuery* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structWholeQuery)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����A" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "֡����B" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actterm 
						|| pHead->byTxReason == CAUSE_daactcon )	//�������(���ٻ�����)�����Կ�ʼ��һ�����ٻ��ˡ�
					{
						//���ݵ�ַ�ҵ���Ӧ���߼���Ԫ��
						CFepIEC104Device* pDevice = dynamic_cast<CFepIEC104Device*> (
							pDriver->GetDeviceByAddr( (OpcUa_Int32)pHead->wdCommAddr ));
						if ( pDevice )
						{
							//		pDevice->NeedWholeQuery(true);
							pDevice->WholeQuerying(false);		//�ͷŶ�ռ��־
						}
					}
				}
				break;
			case C_CI_NA_1:		//�������ٻ�����
				{
					sprintf( strLog, "�������ٻ�����, ����ԭ��: %d", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structCountQuery* pPoOpcUa_Int = (structCountQuery* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structCountQuery)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����A" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "֡����B" );
						m_bExtractError = true;	//��������
						break;
					}

					//������������Կ�ʼ��һ�μ������ٻ��ˡ�
					if ( pHead->byTxReason == CAUSE_actterm )	
					{
						//���ݵ�ַ�ҵ���Ӧ���߼���Ԫ��
						CFepIEC104Device* pDevice = dynamic_cast<CFepIEC104Device*>( 
							pDriver->GetDeviceByAddr( (OpcUa_Int32)pHead->wdCommAddr ) );
						if ( pDevice )
						{
							//		pDevice->NeedCountQuery(true);
							pDevice->CountQuerying(false);		//�ͷŶ�ռ��־
						}
					}
				}
				break;
			case C_RD_NA_1:		//������
				sprintf( strLog, "������" );
				pDriver->WriteLog( strLog );

				break;
			case C_CS_NA_1:		//ʱ��ͬ������
				{
					sprintf( strLog, "Уʱ(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structAdjustTime* pPoOpcUa_Int = (structAdjustTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

					nNum = pHead->byLimit;
					if ( nNum*sizeof(structAdjustTime)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����A" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "֡����B" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_actterm )	//Уʱ�ɹ���
					{
						//���ݵ�ַ�ҵ���Ӧ���߼���Ԫ��
						CFepIEC104Device* pDevice = dynamic_cast<CFepIEC104Device*>( 
							pDriver->GetDeviceByAddr( (OpcUa_Int32)pHead->wdCommAddr ) );
						if ( pDevice )
						{
							pDevice->IsCheckTiming(false);		//�ͷŶ�ռ��־
						}
					}
				}
				break;
			case C_TS_NA_1:		//��������
				{
					sprintf( strLog, "����(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structTest* pPoOpcUa_Int = (structTest* )(m_sPacketBuffer 
						+ sizeof(APCI) + sizeof(ASDUHead) );

					nNum = pHead->byLimit;
					if ( nNum*sizeof(structTest)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����A" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 || pPoOpcUa_Int->wdFBP != 0x55AA )
					{
						pDriver->WriteLog( "֡����B" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon )	//���Գɹ���
					{

					}
				}
				break;
			case C_RP_NA_1:		//��λ��������
				{
					sprintf( strLog, "��λ����(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structResetProc* pPoOpcUa_Int = (structResetProc* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structResetProc)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����A" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "֡����B" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon )	//��λ���̳ɹ���
					{

					}
				}
				break;
			case C_CD_NA_1:		//��ʱ�������
				{
					sprintf( strLog, "��ʱ�������(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structDelay* pPoOpcUa_Int = (structDelay* )(m_sPacketBuffer 
						+ sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structDelay)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����A" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "֡����B" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon )	//�ɹ���
					{

					}
				}
				break;
			case C_TS_TA_1:		//��ʱ��CP56Time2a�Ĳ�������
				{
					sprintf( strLog, "��ʱ��Ĳ���(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structTestTime* pPoOpcUa_Int = (structTestTime* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structTestTime)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����A" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 || pPoOpcUa_Int->wdFBP != 0x55AA )
					{
						pDriver->WriteLog( "֡����B" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon )	//���Գɹ���
					{

					}
				}
				break;
			case P_ME_NA_1:		//����ֵ����, ��һ��ֵ
				{
					pDriver->WriteLog( "����ֵ����, ��һ��ֵ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structPara16bitNoInfoAddr* pPoOpcUa_Int = (structPara16bitNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structPara16bitNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara16bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_CS, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structPara16bit* pPoOpcUa_Int = (structPara16bit* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structPara16bit)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara16bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)GetNVAVal( pPoOpcUa_Int->sNVA);
								pDevice->SetVal( MACS_IEC104_CS, 
									pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case P_ME_NB_1:		//����ֵ����,  ��Ȼ�ֵ
				{
					pDriver->WriteLog( "����ֵ����, ��Ȼ�ֵ" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structPara16bitNoInfoAddr* pPoOpcUa_Int = (structPara16bitNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) 
							+ sizeof(structInfoAddr) );

						if ( nNum*sizeof(structPara16bitNoInfoAddr)+sizeof(APCI)
							+sizeof(ASDUHead)+sizeof(structInfoAddr) != m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara16bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sSVA.Val;
								pDevice->SetVal( MACS_IEC104_CS, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structPara16bit* pPoOpcUa_Int = (structPara16bit* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
						if ( nNum*sizeof(structPara16bit)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara16bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->sSVA.Val;
								pDevice->SetVal( MACS_IEC104_CS, 
									pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case P_ME_NC_1:		//����ֵ����, �̸�����
				{
					pDriver->WriteLog( "����ֵ����, �̸�����" );

					nNum = pHead->byLimit;
					if ( pHead->bSQ )	//��Ϣ�����ַ����
					{
						structInfoAddr* pStartInfoAddr = (structInfoAddr* )( 
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						structPara32bitNoInfoAddr* pPoOpcUa_Int = (structPara32bitNoInfoAddr* )(
							m_sPacketBuffer + sizeof(APCI) 
							+ sizeof(ASDUHead) + sizeof(structInfoAddr) );

						if ( nNum*sizeof(structPara32bitNoInfoAddr)+sizeof(APCI)+sizeof(ASDUHead)+sizeof(structInfoAddr) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							m_bExtractError = true;	//��������
							pDriver->WriteLog( "֡����" );
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara32bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->fValue;
								pDevice->SetVal( MACS_IEC104_CS, 
									pStartInfoAddr->wdInfoAddr+i, 
									(OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
					else
					{
						structPara32bit* pPoOpcUa_Int = (structPara32bit* )(
							m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );

						if ( nNum*sizeof(structPara32bit)+sizeof(APCI)+sizeof(ASDUHead) 
							!= m_wdValidDataLen )	//���ȷǷ�
						{
							pDriver->WriteLog( "֡����" );
							m_bExtractError = true;	//��������
							break;
						}

						for ( i = 0; i < nNum; i++ )
						{
							if ( IsValidPara32bit( *pPoOpcUa_Int ) )	//��Ч
							{
								ftempVal = (float)pPoOpcUa_Int->fValue;
								pDevice->SetVal( MACS_IEC104_CS, 
									pPoOpcUa_Int->wdInfoAddr, (OpcUa_Byte*)&ftempVal, 
									sizeof(float), 0, true );
							}
							pPoOpcUa_Int++;
						}
					}
				}
				break;
			case P_AC_NA_1:		//��������
				{
					sprintf( strLog, "��������(TxReason=%d!)", pHead->byTxReason );
					pDriver->WriteLog( strLog );

					structParaAct* pPoOpcUa_Int = (structParaAct* )(
						m_sPacketBuffer + sizeof(APCI) + sizeof(ASDUHead) );
					nNum = pHead->byLimit;
					if ( nNum*sizeof(structParaAct)+sizeof(APCI)+sizeof(ASDUHead) 
						!= m_wdValidDataLen )	//���ȷǷ�
					{
						pDriver->WriteLog( "֡����A" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( nNum != 1 || pPoOpcUa_Int->wdInfoAddr != 0 )
					{
						pDriver->WriteLog( "֡����B" );
						m_bExtractError = true;	//��������
						break;
					}

					if ( pHead->byTxReason == CAUSE_actcon 
						|| pHead->byTxReason == CAUSE_daactcon )	//�ɹ���
					{
					}
				}
				break;
			case F_FR_NA_1:		//�ļ�׼������
				break;
			case F_SR_NA_1:		//��׼������
				break;
			case F_SC_NA_1:		//�ٻ�Ŀ¼, ѡ���ļ�, �ٻ��ļ�,�ٻ���
				break;
			case F_LS_NA_1:		//���Ľ�,���Ķ�
				break;
			case F_AF_NA_1:		//�Ͽ��ļ�, �Ͽɽ�
				break;
			case F_SG_NA_1:		//��
				break;
			case F_DR_TA_1:		//Ŀ¼
				break;
			default:
				break;
			}

			//��DI��DI2�ı�λ��Ϣ
			if ( iCount > 0 )
			{
				std::string szSndPacket = GetCommData( m_sPacketBuffer, m_wdValidDataLen, 1 );
				snprintf( strLog, sizeof(strLog),"���ձ�λ֡����λ����%d����%d�ֽ�: %s", iCount, m_wdValidDataLen, szSndPacket.c_str() );
				pDriver->WriteLogAnyWay( strLog );
			}

			if ( !m_bExtractError )	//��������
			{
				if( bFromLocal )
				{
					//		pDevice->SetNr( wdNr );	//���½������
					//		pDevice->SetNs( wdNs ); //���·������

					//����װ������
					{
						pDevice->GenNextNr();

						//��ʱ����S֡
						m_bNeedSendSFrame = pDevice->IsNeedSendSFrame();

						pDevice->SetOnLine( DEVICE_ONLINE );	//�����豸�յ�I֡һ����Ϊ����
						pDevice->m_byTxTimes = 0;
						pDevice->m_byRxTimes++;
					}

					//��λ���ӱ�־
					m_bNeedConnected = false;
				}
			}
		}
		else	//����豸����ģ��̬�������Ǹ��½�����š�����������
		{
			if( bFromLocal )
			{
				//	pDevice->SetNr( wdNr );	//���½������
				//	pDevice->SetNs( wdNs ); //���·������

				//����װ������
				{
					pDevice->GenNextNr();

					//��ʱ����S֡
					m_bNeedSendSFrame = pDevice->IsNeedSendSFrame();

					pDevice->SetOnLine( DEVICE_ONLINE );	//�����豸�յ�I֡һ����Ϊ����
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes++;
				}

				//��λ���ӱ�־
				m_bNeedConnected = false;
			}
		}		
	}

	//! ���ӳɹ�
	OpcUa_Int32 CFepIEC104Prtcl::OnConnSuccess(std::string pPeerIP )
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		//! ������
		//ACE_Guard<ACE_Recursive_Thread_Mutex> guard( pDriver->m_Mutex );

		m_bNeedConnected = false;
		
		pDriver->Connected(true);

		//! �ر����ӳ�ʱ��ʱ��
		pDriver->KillConnTimer();

		//! ��־
		char pchInfo[256];
		sprintf( pchInfo, "���ӷ�����%s�ɹ�...", pPeerIP.c_str() );
		pDriver->WriteLogAnyWay( pchInfo );
		
		pDriver->IsConnecting(false);	

		//! �������ڶ�ʱ��
/*
		m_pPeriodTimer->KillTimer();
		m_pPeriodTimer->SetTimer( PERIOD_SCAN_MS );
*/

		//���ӳɹ�����Ԥ��StartDT��־�����Ƿ���������StartDT֡������Ҫ���豸������״̬��
		//�������Ԥ�ã�
		ForStartDT();

		//���ͺͽ���������¹��㡣Add by hxw in 2014.5.4
		ResetAllDeviceNs();
		ResetAllDeviceNr();


/*	
		for(OpcUa_Int32 j=0;j<pDriver->GetDeviceCount();j++)
		{
			//! �������豸����
			pDriver->GetDeviceByIndex(j)->SetOnLine( DEVICE_ONLINE );   
		}	
*/

		m_nOfflineCount = 0;	//���ӳɺ����¿�ʼ��һ���������
		m_iPeriodCount  = 0;	//���ӳɺ�����׼�����͸�����վ���ģ���S֡��

		//////////////////////////////////////////////////////////////////////////////
		//! ��������
		//OpcUa_Int32 nDataLen = 0;
		BuildRequest( m_pbySndBuffer, m_nSndLen );
		//////////////////////////////////////////////////////////////////////////////
		return 0;
	}

	// *****************************************************
	// ��������: ���챾����·����ʱ�ı�������֡(����ң��)
	// �� �� ֵ: OpcUa_Int32 : 
	// ����˵��: 
	//    OpcUa_Byte* pBuf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Prtcl::BuildLocalNormalFrame( OpcUa_Byte* pBuf )
	{
		if ( !pBuf )
		{
			return 0;
		}

		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		if ( m_bNeedStopDTcon )			//��Ҫ����STOPDTȷ��֡
		{
			m_bNeedStopDTcon = false;
			pDriver->WriteLogAnyWay( "Send STOPDTcon Frame!" );
			return CFepIEC104Device::BuildStopFrameCON( pBuf );
		}

		if ( m_bNeedStartDTcon )		//��Ҫ����STARTDTȷ��֡
		{
			pDriver->WriteLogAnyWay( "Send STARTDTcon Frame!" );
			m_bNeedStartDTcon = false;
			return CFepIEC104Device::BuildStartFrameCON( pBuf );
		}

		if ( m_bNeedSendTestcon )		//��Ҫ����TESTFRȷ��֡
		{
			pDriver->WriteLog( "Send TESTFRcon Frame!" );
			m_bNeedSendTestcon = false;
			return CFepIEC104Device::BuildTestFrameCON( pBuf );
		}

		if ( m_bNeedStopDT )			//��Ҫ����STOPDT֡
		{
			//������������StopDT��Ƶ��
			ACE_Time_Value tv(0,500000);	
			ACE_OS::sleep(tv);
	
			m_bNeedStopDT = false;
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send STOPDT Frame!" ) ));
			pDriver->WriteLogAnyWay( "Send STOPDT Frame!" );
			return CFepIEC104Device::BuildStopFrame( pBuf );
		}

		if ( m_bNeedStartDT )			//��Ҫ����STARTDT֡
		{//ע��: �˴�����λm_bNeedStartDT��־,�յ���Ӧ֡�Ÿ�λ.
			//������������StartDT��Ƶ��
			ACE_Time_Value tv(0,500000);	
			ACE_OS::sleep(tv);

			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "------------------CFepIEC104Prtcl Send STARTDT Frame------------------!" ) ));
			pDriver->WriteLogAnyWay( "Send STARTDT Frame!" );
			return CFepIEC104Device::BuildStartFrame( pBuf );
		}

		if ( m_bNeedSendTest )			//��Ҫ����TESTFR��Ч֡
		{
			pDriver->WriteLog( "Send TESTFR��Ч Frame!" );
			m_bNeedSendTest = false;
			return CFepIEC104Device::BuildTestFrame( pBuf );
		}

		CFepIEC104Device* pDevice = dynamic_cast<CFepIEC104Device*>( 
			pDriver->GetDeviceByIndex(m_shAdiLogical) );

		if ( m_bNeedSendSFrame )	//��Ҫ����S��ʽ֡
		{
			pDriver->WriteLog( "Send S Frame!" );
			m_bNeedSendSFrame = false;
			//��λw����
			pDevice->ResetCtrlAreaINum();
			return pDevice->BuildSFrame( pBuf );
		}

		if ( pDevice->IsTimeForCheckTime() )	//��ҪУʱ��
		{
			pDriver->WriteLogAnyWay( "Send CheckTime Frame!" );
			pDevice->IsCheckTiming(true);	//�ö�ռ��־
			pDevice->IsCheckTiming(false);//hbb����Уʱ֡û��Ӧ�ù����������������� ˵��3.1��վ��Уʱ֡�Ĺ�����ַΪ0 �ö�ռ��־
			/*!
			*	�����豸�ţ��Ա�TimeOutʱ�ͷŶ�ռ��־��
			*	ע�⣺������m_shAdiLogical��������ң�ػ�ң���������
			*/
			m_lastDevNo = m_shAdiLogical;		
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send CheckTime Frame!" ) ));
			return pDevice->BuildCheckTimeFrame( pBuf );
		}

		if ( pDevice->IsTimeForWholeQuery() )	//��Ҫ�ܲ�ѯ��
		{
			pDriver->WriteLog( "Send WholeQuery Frame!" );
			pDevice->WholeQuerying(true);	//�ö�ռ��־
			
			/*! 
			 *	�����豸�ţ��Ա�TimeOutʱ�ͷŶ�ռ��־��
			 *	ע�⣺������m_shAdiLogical��������ң�ػ�ң���������
			 */
			m_lastDevNo = m_shAdiLogical;		
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send WholeQuery Frame!" ) ));
			return pDevice->BuildWholeQueryFrame( pBuf );
		}

		if ( pDevice->IsTimeForCountQuery() )	//��Ҫ��������ѯ��
		{
			pDriver->WriteLog( "Send CountQuery Frame!" );
			pDevice->CountQuerying(true);	//�ö�ռ��־
			
			/*! 
			 *	�����豸�ţ��Ա�TimeOutʱ�ͷŶ�ռ��־��
			 *	ע�⣺������m_shAdiLogical��������ң�ػ�ң���������
			 */
			m_lastDevNo = m_shAdiLogical;		

			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send CountQuery Frame!" ) ));
			return pDevice->BuildCountQueryFrame( pBuf );
		}

		//����ʱ����S��ʽ֡
		//return pDevice->BuildSFrame( pBuf );

		return 0;
	}

	// *****************************************************
	// ��������: ���챾����·����ʱ�ı��شӻ�����֡(����ң��)
	// �� �� ֵ: OpcUa_Int32 : 
	// ����˵��: 
	//    OpcUa_Byte* pBuf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Prtcl::BuildLocalStandbyFrame( OpcUa_Byte* pBuf )
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;

		pDriver->WriteLog( "BuildLocalStandbyFrame: start...." );

		if ( !pBuf )
		{
			pDriver->WriteLog( "BuildLocalStandbyFrame: pBuf=NULL." );
			return 0;
		}


		if ( m_bNeedStopDTcon )			//��Ҫ����STOPDTȷ��֡
		{
			m_bNeedStopDTcon = false;
			pDriver->WriteLogAnyWay( "Send STOPDTcon Frame!" );
			return CFepIEC104Device::BuildStopFrameCON( pBuf );
		}

		if ( m_bNeedStartDTcon )		//��Ҫ����STARTDTȷ��֡
		{
			pDriver->WriteLogAnyWay( "Send STARTDTcon Frame!" );
			m_bNeedStartDTcon = false;
			return CFepIEC104Device::BuildStartFrameCON( pBuf );
		}

		if ( m_bNeedSendTestcon )		//��Ҫ����TESTFRȷ��֡
		{
			pDriver->WriteLog( "Send TESTFRcon Frame!" );
			m_bNeedSendTestcon = false;
			return CFepIEC104Device::BuildTestFrameCON( pBuf );
		}

		if ( m_bNeedStopDT )			//��Ҫ����STOPDT֡
		{
			m_bNeedStopDT = false;
			MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Prtcl Send STOPDT Frame!" ) ));
			pDriver->WriteLogAnyWay( "Send STOPDT Frame!" );
			return CFepIEC104Device::BuildStopFrame( pBuf );
		}

		if ( m_bNeedSendTest )			//��Ҫ����TESTFR��Ч֡
		{
			pDriver->WriteLog( "Send TESTFR��Ч֡ Frame!" );
			m_bNeedSendTest = false;
			return CFepIEC104Device::BuildTestFrame( pBuf );
		}

		CFepIEC104Device* pDevice = dynamic_cast<CFepIEC104Device*>( 
			pDriver->GetDeviceByIndex(m_shAdiLogical) );

		pDriver->WriteLog( "BuildLocalStandbyFrame: check SFrame flag...." );
		if ( m_bNeedSendSFrame )	//��Ҫ����S��ʽ֡
		{
			pDriver->WriteLog( "Send S Frame!" );
			m_bNeedSendSFrame = false;
			//��λw����
			pDevice->ResetCtrlAreaINum();
			return pDevice->BuildSFrame( pBuf );
		}

		return 0;
	}

	// *****************************************************
	// ��������: ��֤�Ƿ�Ϊ��ЧIP
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    char* ip: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidIP( const char* ip )
	{
		OpcUa_Int32 iPos = 0;
		OpcUa_Int32 iSegPos = 0;
		OpcUa_Int32 iSegCnt = 0; //�ָ���Ÿ���
		OpcUa_Int32 IPArea = 0;  //�������͵�ÿ�ε�ַ

		char temp[4];
		memset(&temp,0,4);

		for( iPos=0; ip[iPos] != '\0'; iPos++,iSegPos++ )
		{
			if ('.' == ip[iPos])
			{
				iSegCnt++;
				iPos++;
				IPArea = atoi(temp);				
				if ( IPArea < 0 || IPArea > 255 )
				{
					return false;
				}
				memset(&temp,0,4);
				iSegPos = 0;
			}

			temp[iSegPos] = ip[iPos];

			if (iSegPos > 2 || iSegCnt > 3) //IPV4 ÿ�����Ϊ��λ�����±�0��ʼ�� ���������ָ���ţ�//
			{
				return false;
			}
		}

		return true;
	}

	// *****************************************************
	// ��������: �����豸����������¹���
	// �㷨˼��: 
	// ����ģ��: 
	// �� �� ֵ: void : �ú����������κ�ֵ
	// *****************************************************
	void CFepIEC104Prtcl::ResetAllDeviceNr()
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		CFepIEC104Device* pDevice = NULL;
		for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC104Device*>( pDriver->GetDeviceByIndex(i) );
			if( pDevice )
			{
				pDevice->ResetNr();
			}
		}
	}



	// *****************************************************
	// ��������: �����豸����������¹���
	// �� �� ֵ: void : �ú����������κ�ֵ
	// *****************************************************
	void CFepIEC104Prtcl::ResetAllDeviceNs()
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		CFepIEC104Device* pDevice = NULL;
		for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepIEC104Device*>( pDriver->GetDeviceByIndex(i) );
			if( pDevice )
			{
				pDevice->ResetNs();
			}
		}
	}

	// *****************************************************
	// ��������: �Ƿ�t0�������
	// �� �� ֵ: bool : 
	// *****************************************************
/*
	bool CFepIEC104Prtcl::IsTimeForConnectEx()
	{
		if ( m_bNeedConnected )
		{
			m_bNeedConnected = false;

			return true;
		}

		return false;
	}
*/




	// *****************************************************
	// ��������: ��֤����ʱ��ĵ�����ϢSPI��Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structSDINoTime& SPI: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidSDI( structSDINoTime& SDI )
	{
		if ( IsCheckSDI() )
		{
			return ( !SDI.sSIQ.BL && !SDI.sSIQ.IV 
				&& !SDI.sSIQ.NT && !SDI.sSIQ.Res && !SDI.sSIQ.SB 
				&& SDI.wdInfoAddr >= DI_START_ADDR && SDI.wdInfoAddr <= DI_END_ADDR );
		}
		else
		{
		return ( /*!SDI.sSIQ.BL &&*/ !SDI.sSIQ.IV 
			/*&& !SDI.sSIQ.NT && !SDI.sSIQ.Res && !SDI.sSIQ.SB*/ 
			&& SDI.wdInfoAddr >= DI_START_ADDR && SDI.wdInfoAddr <= DI_END_ADDR );
		}
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDINoTimeNoInfoAddr& SDI )
	{
		if ( IsCheckSDI() )
		{
			return ( !SDI.sSIQ.BL && !SDI.sSIQ.IV 
				&& !SDI.sSIQ.NT && !SDI.sSIQ.Res && !SDI.sSIQ.SB 
				);
		}
		else
		{
		return ( /*!SDI.sSIQ.BL &&*/ !SDI.sSIQ.IV 
			/*&& !SDI.sSIQ.NT && !SDI.sSIQ.Res && !SDI.sSIQ.SB*/ 
			);
		}
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDI_Time24NoInfoAddr& SDI )
	{
		if ( IsCheckSDI() )
		{
			return ( !SDI.sSDINoTime.sSIQ.BL && !SDI.sSDINoTime.sSIQ.IV 
				&& !SDI.sSDINoTime.sSIQ.NT && !SDI.sSDINoTime.sSIQ.Res && !SDI.sSDINoTime.sSIQ.SB );
		}
		else
		{
		return ( /*!SDI.sSDINoTime.sSIQ.BL &&*/ !SDI.sSDINoTime.sSIQ.IV 
			/*&& !SDI.sSDINoTime.sSIQ.NT && !SDI.sSDINoTime.sSIQ.Res && !SDI.sSDINoTime.sSIQ.SB*/ );
		}
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDINoTimeNoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidSDI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDINoTime* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidSDI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidSDI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidSDI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidSDI( structSDI_Time24& SDI )
	{
		if ( IsCheckSDI() )
		{
			return ( !SDI.sSDINoTime.sSIQ.BL && !SDI.sSDINoTime.sSIQ.IV 
				&& !SDI.sSDINoTime.sSIQ.NT && !SDI.sSDINoTime.sSIQ.Res && !SDI.sSDINoTime.sSIQ.SB 
				&& SDI.sSDINoTime.wdInfoAddr >= DI_START_ADDR && SDI.sSDINoTime.wdInfoAddr <= DI_END_ADDR );
		}
		else
		{
			return ( /*!SDI.sSDINoTime.sSIQ.BL &&*/ !SDI.sSDINoTime.sSIQ.IV 
				/*&& !SDI.sSDINoTime.sSIQ.NT && !SDI.sSDINoTime.sSIQ.Res && !SDI.sSIQ.SB*/ 
				&& SDI.sSDINoTime.wdInfoAddr >= DI_START_ADDR && SDI.sSDINoTime.wdInfoAddr <= DI_END_ADDR );
		}
	}

	// *****************************************************
	// ��������: ��֤����ʱ���˫����ϢDPI��Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structDDINoTime& DDI: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidDDI( structDDINoTime& DDI )
	{
		if ( IsCheckDDI() )
		{
			return ( !DDI.sDIQ.BL && !DDI.sDIQ.IV 
				&& !DDI.sDIQ.NT && !DDI.sDIQ.Res && !DDI.sDIQ.SB
				&& DDI.wdInfoAddr >= DI_START_ADDR && DDI.wdInfoAddr <= DI_END_ADDR );
		}
		else
		{
			return ( /*!DDI.sDIQ.BL &&*/ !DDI.sDIQ.IV 
				/*&& !DDI.sDIQ.NT && !DDI.sDIQ.Res && !DDI.sDIQ.SB*/
				&& DDI.wdInfoAddr >= DI_START_ADDR && DDI.wdInfoAddr <= DI_END_ADDR );
		}
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDINoTimeNoInfoAddr& DDI )
	{
		if ( IsCheckDDI() )
		{
			return ( !DDI.sDIQ.BL && !DDI.sDIQ.IV 
				&& !DDI.sDIQ.NT && !DDI.sDIQ.Res && !DDI.sDIQ.SB
				);
		}
		else
		{
		return ( /*!DDI.sDIQ.BL &&*/ !DDI.sDIQ.IV 
			/*&& !DDI.sDIQ.NT && !DDI.sDIQ.Res && !DDI.sDIQ.SB*/
			);
		}
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDINoTimeNoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidDDI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDINoTime* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidDDI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidDDI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidDDI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDI_Time24NoInfoAddr& DDI )
	{
		if ( IsCheckDDI() )
		{
			return ( !DDI.sDDINoTime.sDIQ.BL && !DDI.sDDINoTime.sDIQ.IV 
				&& !DDI.sDDINoTime.sDIQ.NT && !DDI.sDDINoTime.sDIQ.Res && !DDI.sDDINoTime.sDIQ.SB );
		}
		else
		{
			return ( /*!DDI.sDDINoTime.sDIQ.BL &&*/ !DDI.sDDINoTime.sDIQ.IV 
				/*&& !DDI.sDDINoTime.sDIQ.NT && !DDI.sDDINoTime.sDIQ.Res && !DDI.sDDINoTime.sDIQ.SB*/ );
		}
	}

	bool CFepIEC104Prtcl::IsValidDDI( structDDI_Time24& DDI )
	{
		if ( IsCheckDDI() )
		{
			return ( !DDI.sDDINoTime.sDIQ.BL && !DDI.sDDINoTime.sDIQ.IV 
				&& !DDI.sDDINoTime.sDIQ.NT && !DDI.sDDINoTime.sDIQ.Res && !DDI.sDDINoTime.sDIQ.SB
				&& DDI.sDDINoTime.wdInfoAddr >= DI_START_ADDR && DDI.sDDINoTime.wdInfoAddr <= DI_END_ADDR );
		}
		else
		{
			return ( /*!DDI.sDDINoTime.sDIQ.BL &&*/ !DDI.sDDINoTime.sDIQ.IV 
				/*&& !DDI.sDDINoTime.sDIQ.NT && !DDI.sDDINoTime.sDIQ.Res && !DDI.sDDINoTime.sDIQ.SB*/
				&& DDI.sDDINoTime.wdInfoAddr >= DI_START_ADDR && DDI.sDDINoTime.wdInfoAddr <= DI_END_ADDR);
		}
	}

	// *****************************************************
	// ��������: ��֤��λ����ϢVTI��Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structVTI& VTI: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidVTI( structVTI& VTI )
	{
		if ( IsCheckVTI() )
		{
		return ( !VTI.Quality.BL && !VTI.Quality.IV 
			&& !VTI.Quality.NT && !VTI.Quality.OV
			&& !VTI.Quality.Res && !VTI.Quality.SB && !VTI.sVTI.T 
			&& VTI.wdInfoAddr >= BWZ_START_ADDR && VTI.wdInfoAddr <= BWZ_END_ADDR  );
		}
		else
		{
		return ( /*!VTI.Quality.BL &&*/ !VTI.Quality.IV 
			/*&& !VTI.Quality.NT && !VTI.Quality.OV
			&& !VTI.Quality.Res && !VTI.Quality.SB && !VTI.sVTI.T*/ 
			&& VTI.wdInfoAddr >= BWZ_START_ADDR && VTI.wdInfoAddr <= BWZ_END_ADDR  );
		}
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTINoInfoAddr& VTI )
	{
		if ( IsCheckVTI() )
		{
		return ( !VTI.Quality.BL && !VTI.Quality.IV 
			&& !VTI.Quality.NT && !VTI.Quality.OV
			&& !VTI.Quality.Res && !VTI.Quality.SB && !VTI.sVTI.T 
			);
		}
		else
		{
		return ( /*!VTI.Quality.BL &&*/ !VTI.Quality.IV 
			/*&& !VTI.Quality.NT && !VTI.Quality.OV
			&& !VTI.Quality.Res && !VTI.Quality.SB && !VTI.sVTI.T*/ 
			);
		}
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTINoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidVTI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTI* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidVTI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidVTI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum )
	{
		for ( OpcUa_Int32 i = 0; i < nNum; i++ )
		{
			if ( !IsValidVTI( *pPoOpcUa_Int ) )	//��Ч
			{
				return false;
			}
			pPoOpcUa_Int++;
		}

		return true;
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTI_Time24NoInfoAddr& VTI )
	{
		if ( IsCheckVTI() )
		{
		return ( !VTI.ssVTI.Quality.BL && !VTI.ssVTI.Quality.IV 
			&& !VTI.ssVTI.Quality.NT && !VTI.ssVTI.Quality.OV
			&& !VTI.ssVTI.Quality.Res && !VTI.ssVTI.Quality.SB && !VTI.ssVTI.sVTI.T 
			);
		}
		else
		{
		return ( /*!VTI.ssVTI.Quality.BL &&*/ !VTI.ssVTI.Quality.IV 
			/*&& !VTI.ssVTI.Quality.NT && !VTI.ssVTI.Quality.OV
			&& !VTI.ssVTI.Quality.Res && !VTI.ssVTI.Quality.SB && !VTI.ssVTI.sVTI.T*/ 
			);
		}
	}

	bool CFepIEC104Prtcl::IsValidVTI( structVTI_Time24& VTI )
	{
		if ( IsCheckVTI() )
		{
		return ( !VTI.ssVTI.Quality.BL && !VTI.ssVTI.Quality.IV 
			&& !VTI.ssVTI.Quality.NT && !VTI.ssVTI.Quality.OV
			&& !VTI.ssVTI.Quality.Res && !VTI.ssVTI.Quality.SB && !VTI.ssVTI.sVTI.T 
			&& VTI.ssVTI.wdInfoAddr >= BWZ_START_ADDR && VTI.ssVTI.wdInfoAddr <= BWZ_END_ADDR
			);
		}
		else
		{
		return ( /*!VTI.ssVTI.Quality.BL &&*/ !VTI.ssVTI.Quality.IV 
			/*&& !VTI.ssVTI.Quality.NT && !VTI.ssVTI.Quality.OV
			&& !VTI.ssVTI.Quality.Res && !VTI.ssVTI.Quality.SB && !VTI.ssVTI.sVTI.T*/ 
			&& VTI.ssVTI.wdInfoAddr >= BWZ_START_ADDR && VTI.ssVTI.wdInfoAddr <= BWZ_END_ADDR
			);
		}
	}
	// *****************************************************
	// ��������: ��֤��վԶ���ն�״̬��32���ش�����ϢRTS��Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structRTS& RTS: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidRTS( structRTS& RTS )
	{
		return( /*!RTS.Quality.BL &&*/ !RTS.Quality.IV 
			/*&& !RTS.Quality.NT && !RTS.Quality.OV
			&& !RTS.Quality.Res && !RTS.Quality.SB */
			&& RTS.wdInfoAddr >= RTS_START_ADDR && RTS.wdInfoAddr <= RTS_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidRTS( structRTSNoInfoAddr& RTS )
	{
		return( /*!RTS.Quality.BL &&*/ !RTS.Quality.IV 
			/*&& !RTS.Quality.NT && !RTS.Quality.OV
			&& !RTS.Quality.Res && !RTS.Quality.SB */
			);
	}



	// *****************************************************
	// ��������: ��֤ң����16λRT��ϢRTS��Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structRT16bit& RT16: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidRT16bit( structRT16bit& RT16 )
	{
		return( /*!RT16.Quality.BL &&*/ !RT16.Quality.IV 
			/*&& !RT16.Quality.NT && !RT16.Quality.OV
			&& !RT16.Quality.Res && !RT16.Quality.SB*/ 
			&& RT16.wdInfoAddr >= AI_START_ADDR && RT16.wdInfoAddr <= AI_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidRT16bit( structRT16bitNoInfoAddr& RT16 )
	{
		return( /*!RT16.Quality.BL &&*/ !RT16.Quality.IV 
			/*&& !RT16.Quality.NT && !RT16.Quality.OV
			&& !RT16.Quality.Res && !RT16.Quality.SB*/ 
			);
	}


	// *****************************************************
	// ��������: ��֤ң����32λRT��ϢRTS��Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structRT32bit& RT32: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidRT32bit( structRT32bit& RT32 )
	{
		return( /*!RT32.Quality.BL &&*/ !RT32.Quality.IV 
			/*&& !RT32.Quality.NT && !RT32.Quality.OV
			&& !RT32.Quality.Res && !RT32.Quality.SB */
			&& RT32.wdInfoAddr >= AI_START_ADDR && RT32.wdInfoAddr <= AI_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidRT32bit( structRT32bitNoInfoAddr& RT32 )
	{
		return( /*!RT32.Quality.BL &&*/ !RT32.Quality.IV 
			/*&& !RT32.Quality.NT && !RT32.Quality.OV
			&& !RT32.Quality.Res && !RT32.Quality.SB */
			);
	}



	// *****************************************************
	// ��������: ��֤ң����16λ����ֵ������Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structPara16bit& Para16: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidPara16bit( structPara16bit& Para16 )
	{
		return ( !Para16.sQPM.KPA 
			&& Para16.wdInfoAddr >= CS_START_ADDR 
			&& Para16.wdInfoAddr <= CS_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidPara16bit( structPara16bitNoInfoAddr& Para16 )
	{
		return ( !Para16.sQPM.KPA );
	}


	// *****************************************************
	// ��������: ��֤ң����32λ����ֵ������Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structPara32bit& Para32: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidPara32bit( structPara32bit& Para32 )
	{
		return ( !Para32.sQPM.KPA 
			&& Para32.wdInfoAddr >= CS_START_ADDR 
			&& Para32.wdInfoAddr <= CS_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidPara32bit( structPara32bitNoInfoAddr& Para32 )
	{
		return ( !Para32.sQPM.KPA );
	}


	// *****************************************************
	// ��������: ��֤�����ۻ�����Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structDNLJ& DNLJ: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidDNLJ( structDNLJ& DNLJ )
	{
		return ( !DNLJ.sSN.IV 
			&& DNLJ.wdInfoAddr >= DNLJ_START_ADDR 
			&& DNLJ.wdInfoAddr <= DNLJ_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidDNLJ( structDNLJNoInfoAddr& DNLJ )
	{
		return ( !DNLJ.sSN.IV );
	}


	// *****************************************************
	// ��������: ��֤��ʱ��ļ̵籣��װ���¼���Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structJDBHNoTime& JDBH: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidJDBH( structJDBHNoTime& JDBH )
	{
		return ( !JDBH.sSEP.IV 
			&& JDBH.wdInfoAddr >= JDBH_START_ADDR 
			&& JDBH.wdInfoAddr <= JDBH_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidJDBH( structJDBHNoTimeNoInfoAddr& JDBH )
	{
		return ( !JDBH.sSEP.IV );
	}


	// *****************************************************
	// ��������: ��֤��ʱ��ļ̵籣��װ�ó��������¼���Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structJDBHGroupActNoTime& JDBHGroup: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidJDBHGroupAct( 
		structJDBHGroupActNoTime& JDBHGroup )
	{
		return ( !JDBHGroup.sQDP.IV 
			&& JDBHGroup.wdInfoAddr >=JDBH_START_ADDR 
			&& JDBHGroup.wdInfoAddr <= JDBH_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidJDBHGroupAct( 
		structJDBHGroupActNoTimeNoInfoAddr& JDBHGroup )
	{
		return ( !JDBHGroup.sQDP.IV );
	}



	// *****************************************************
	// ��������: ��֤��ʱ��ļ̵籣��װ�ó��������·��Ϣ��Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structJDBHGroupOutputNoTime& JDBHGroup: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidJDBHGroupOutput( 
		structJDBHGroupOutputNoTime& JDBHGroup )
	{
		return ( !JDBHGroup.sQDP.IV 
			&& JDBHGroup.wdInfoAddr >=JDBH_START_ADDR 
			&& JDBHGroup.wdInfoAddr <= JDBH_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidJDBHGroupOutput( 
		structJDBHGroupOutputNoTimeNoInfoAddr& JDBHGroup )
	{
		return ( !JDBHGroup.sQDP.IV );
	}



	// *****************************************************
	// ��������: ��֤����λ����ĳ��鵥����Ϣ��Ч��
	// �� �� ֵ: bool : 
	// ����˵��: 
	//    structDIGroup& DIGroup: 
	// *****************************************************
	bool CFepIEC104Prtcl::IsValidDIGroup( structDIGroup& DIGroup )
	{
		return ( !DIGroup.sQDP.IV 
			&& DIGroup.wdInfoAddr >= DI_START_ADDR 
			&& DIGroup.wdInfoAddr <= DI_END_ADDR );
	}

	bool CFepIEC104Prtcl::IsValidDIGroup( structDIGroupNoInfoAddr& DIGroup )
	{
		return ( !DIGroup.sQDP.IV );
	}
	// *****************************************************
	// ��������: ������NVA��ֵ
	// �� �� ֵ: float : 
	// ����˵��: 
	//    structNVA& NVA: 
	// *****************************************************
	float CFepIEC104Prtcl::GetNVAVal( structNVA& NVA )
	{
		float fVal = 0.0;
		if ( NVA.Sign == 1 )	//����
		{
			WORD wdT = ~((WORD)NVA.Val) + 1;
			fVal = (float)((OpcUa_Double)(-wdT) / (((WORD)1) << 15 ));
		}
		else	//����
		{
			fVal = (float)((OpcUa_Double)(NVA.Val) / (((WORD)1) << 15 ));
		}

		return fVal;
	}

	bool CFepIEC104Prtcl::IsCtrlAreaI( const void* pBuf )
	{
		if ( pBuf == NULL )
		{
			return false;
		}

		structCtrlAreaI* pI = (structCtrlAreaI* )pBuf;

		return pI->Flag == 0;
	}

	bool CFepIEC104Prtcl::IsCtrlAreaS( const void* pBuf )
	{
		if ( pBuf == NULL )
		{
			return false;
		}

		structCtrlAreaS* pS = (structCtrlAreaS* )pBuf;

		return pS->Flag == 1;
	}

	bool CFepIEC104Prtcl::IsCtrlAreaU( const void* pBuf )
	{
		if ( pBuf == NULL )
		{
			return false;
		}

		structCtrlAreaU* pU = (structCtrlAreaU* )pBuf;

		return pU->Flag == 3;
	}

	// *****************************************************
	// ��������: �����豸��ַ��ȡ�豸������
	// �� �� ֵ: BOOL : 
	// ����˵��: 
	//      OpcUa_Int32 Addr: �豸��ַ
	//    OpcUa_Int32& DevNo: �豸��ַ��Ӧ���豸������
	// *****************************************************
	bool CFepIEC104Prtcl::GetDeviceNoByAddr( OpcUa_Int32 Addr, OpcUa_Int32& DevNo )
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		CIODeviceBase* pDevice = NULL;
		for( OpcUa_Int32 i=0; i< pDriver->GetDeviceCount(); i++ )
		{
			pDevice = pDriver->GetDeviceByIndex(i);
			if( pDevice && pDevice->GetAddr() == Addr )
			{
				DevNo = i;
				return true;
			}
		}

		return false;
	}

	// *****************************************************
	// ��������: �����·״̬��true--������һ���豸���ߣ�false--�����豸�����ߡ�
	// �� �� ֵ: BOOL : 
	// *****************************************************
	bool CFepIEC104Prtcl::GetLinkState()
	{
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		CIODeviceBase* pDevice = NULL;
		for ( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = pDriver->GetDeviceByIndex(i);
			if( pDevice && pDevice->IsOnLine() )
			{
				return true;
			}
		}

		return false;
	}

	OpcUa_Int64 CFepIEC104Prtcl::ASDUTIME2MACSINT64(const ASDUTime& tTime )
	{		
		OpcUa_Int64 llTime =0;
		OpcUa_Int32 iYear,iMon,iDay,iHour,iMin;
		iYear = tTime.stYear.Year;
		iMon = tTime.stMon.Mon;
		iDay = tTime.stDay.MonDay;
		iHour = tTime.stHour.Hours;
		iMin = tTime.stMin.Minute;
		OpcUa_Int64 llTotalDays = 0;
		//! �֣�0-59
		if ( tTime.stMin.MinValid == 0)
		{
			//!��0-99
			//llTime = iYear * 31536000 + 946080000; //!31536000*��+30������

			llTotalDays = Days(2000+iYear,iMon,true);
			llTotalDays += iDay-1;
			llTime += llTotalDays * 86400000LL;

			//! ʱ��0-23
			if ( tTime.stHour.RU  == 0 )
			{
				llTime += (iHour-8) * 3600000LL;
			}
			else
			{
				//!����ʱ�ȱ�׼ʱ����һ��Сʱ
				llTime += (iHour-9) * 3600000LL;
			}

			//!��
			llTime += iMin*60000;

			//!����0-599999			
			llTime += tTime.wMs;
		}
		else
		{
			return 0;
		}
		
		return llTime;		
	}

	OpcUa_Int64 CFepIEC104Prtcl::Time24Bit2MACSINT64( const Time24bit& tTime )
	{
		OpcUa_Int64 llTime=0;
		if ( tTime.stMin.MinValid == 0 )
		{
			ACE_Time_Value val = ACE_OS::gettimeofday();
			ACE_Date_Time dateTime;
			dateTime.update(val);
						
			llTime = val.sec()*1000LL + val.usec()/1000LL;
			if ( dateTime.minute() != tTime.stMin.Minute )
			{
				llTime -= dateTime.minute()*60000; 
				llTime += tTime.stMin.Minute*60000;
			}
			
			llTime -= dateTime.second()*1000;
			llTime -= dateTime.microsec()/1000;
			llTime += tTime.wMs;						
		}
		else
		{
			return 0;
		}	

		return llTime;
	}

	bool CFepIEC104Prtcl::isLeap( OpcUa_Int32 iYear )
	{
		bool bRet = false;
		if ( iYear % 4 == 0 )
		{
			if ( iYear % 100 != 0  || iYear % 400 == 0)
			{
				bRet = true;
			}			
		}
		else
		{
			bRet = false;
		}

		return bRet;
	}

	OpcUa_Int32 CFepIEC104Prtcl::Days( OpcUa_Int32 iYear,OpcUa_Int32 iMon,bool bFrom1970 )
	{
		OpcUa_Int32 iRet =0;
		OpcUa_Int32 iDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
		OpcUa_Int32 iLeapDays[]={31,29,31,30,31,30,31,31,30,31,30,31};
		if ( bFrom1970 == false )
		{
			if ( isLeap(iYear))
			{
				for ( OpcUa_Int32 i=0;i<iMon-1;i++ )
				{
					iRet += iLeapDays[i];
				}
			}
			else
			{
				for ( OpcUa_Int32 i=0;i<iMon-1;i++ )
				{
					iRet += iDays[i];
				}
			}
		}
		else
		{
			for ( OpcUa_Int32 k=1970;k<iYear;k++)
			{
				if ( isLeap(k))
				{
					iRet += 366;
				}
				else
				{
					iRet += 365;
				}
			}

			if ( isLeap(iYear))
			{
				for ( OpcUa_Int32 i=0;i<iMon-1;i++ )
				{
					iRet += iLeapDays[i];
				}
			}
			else
			{
				for ( OpcUa_Int32 i=0;i<iMon-1;i++ )
				{
					iRet += iDays[i];
				}
			}
		}

		return iRet;
	}

	void CFepIEC104Prtcl::InitConfigPar( char* strAppName )
	{
		if ( strAppName == NULL )
		{
			return;
		}

		//! ��ȡ�����ļ�����·��
		std::string file_name = CIODriver::GetFEPRunDir() + MACS_DRIVER_FILENAME;	

		//������������룬Ĭ��10000
		m_t0 = GetPrivateProfileInt( strAppName, "t0", 10000,  file_name.c_str() );
		if ( m_t0 <= 5000 )
		{
			m_t0 = 10000;
		}


		//���ٻ����ڣ����룬Ĭ��30000
		m_t1 = GetPrivateProfileInt(strAppName, "t1", 30000, file_name.c_str()); 
		if ( m_t1 < 5000 )
		{
			m_t1 = 30000;
		}

		//�����ݱ���t2<t1ʱȷ�ϵĳ�ʱ�����룬Ĭ��10000
		m_t2 = GetPrivateProfileInt(strAppName, "t2", 10000, file_name.c_str()); 
		if ( m_t2 < 1000 )
		{
			m_t2 = 10000;
		}

		//���ڿ���t3>t1״̬�·��Ͳ���֡�ĳ�ʱ�����룬Ĭ��20000
		m_t3 = GetPrivateProfileInt(strAppName, "t3", 20000, file_name.c_str()); 
		if ( m_t3 < 1000 )
		{
			m_t3 = 20000;
		}

		//�������ٻ����ڣ����룬Ĭ��60000
		m_t4 = GetPrivateProfileInt(strAppName, "t4", 60000, file_name.c_str()); 
		if ( m_t4 < 5000 )
		{
			m_t4 = 60000;
		}

		//���߼�����Ĭ��10.
		m_nMaxOfflineCount = GetPrivateProfileInt(strAppName, "Offline", 15, file_name.c_str());
		if ( m_nMaxOfflineCount < 10 )
		{
			m_nMaxOfflineCount = 10;
		}

		//��ʱ�����롣50~10000��Ĭ��1000��
		m_TimeOut = GetPrivateProfileInt(strAppName, "TimeOutCycl", 1000, file_name.c_str());
		if ( m_TimeOut < 500 )
		{
			m_TimeOut = 1000;
		}

		//����Test����֡���ڣ����롣0��ʾ�����Ͳ���֡��
		m_TestCycl = GetPrivateProfileInt(strAppName, "TestCycl", 10000, file_name.c_str());
		if ( m_TestCycl < 1000 && m_TestCycl != 0 )
		{
			m_TestCycl = 10000;
		}

		//Уʱ����, ��.Ĭ��3600.
		m_CheckTimeCycl = GetPrivateProfileInt(strAppName, "CheckTimeCycl", 3600, file_name.c_str());
		if ( m_CheckTimeCycl < 5 && m_CheckTimeCycl != 0 )
		{
			m_CheckTimeCycl = 3600;
		}

		OpcUa_Int32 iTemp = GetPrivateProfileInt(strAppName, "IsLogValueError", 1, file_name.c_str());
		m_bIsLogValueError = (iTemp == 0)?false:true;
		
		iTemp = GetPrivateProfileInt(strAppName, "CheckSDI", 1, file_name.c_str());
		m_bCheckSDI = (iTemp == 0)?false:true;

		iTemp = GetPrivateProfileInt(strAppName, "CheckDDI", 1, file_name.c_str());
		m_bCheckDDI = (iTemp == 0)?false:true;

		iTemp = GetPrivateProfileInt(strAppName, "CheckVTI", 1, file_name.c_str());
		m_bCheckVTI = (iTemp == 0)?false:true;

		char pchTemp[1024]={0};
		sprintf( pchTemp, "t0=%d, t1=%d, t2=%d, t3=%d, t4=%d, Offline=%d, TimeOutCycl=%d,TestCycl=%d, CheckTimeCycl=%d, IsLogValueError=%d,CheckSDI=%d,CheckDDI=%d,CheckVTI=%d", \
			m_t0, m_t1,m_t2,m_t3,m_t4,m_nMaxOfflineCount,m_TimeOut,m_TestCycl,m_CheckTimeCycl,m_bIsLogValueError,m_bCheckSDI,m_bCheckDDI,m_bCheckVTI);
		CFepIEC104Drv* pDriver = (CFepIEC104Drv*)m_pParam;
		pDriver->WriteLogAnyWay( pchTemp );
	}


	std::string CFepIEC104Prtcl::GetCommData( OpcUa_Byte* pbyBuf, OpcUa_Int32 nDataLen, OpcUa_Byte byRcvOrSnd )
	{
		std::string szData = "";
		if ( byRcvOrSnd == 1 )
		{
			szData = "RCV: ";
		}
		else
		{
			szData = "SND: ";
		}

		char pchOpcUa_Byte[4];
		//! ��ȡ������ʾ��ʽ
		for ( OpcUa_Int32 i=0; i < nDataLen; i++ )
		{
			sprintf( pchOpcUa_Byte, "%02X ", pbyBuf[i] );
			szData += pchOpcUa_Byte;
		}

		return szData;
	}

	void CFepIEC104Prtcl::ForStartDT()
	{
		//m_bNeedSendSFrame	= false;
		m_bNeedStopDT		= false;
		m_bNeedStartDT		= true;
		//m_bNeedSendTest		= false;
		m_bNeedStartDTcon	= false;
		m_bNeedStopDTcon	= false;
		//m_bNeedSendTestcon	= false;
	}

	void CFepIEC104Prtcl::ForStopDT()
	{
		//m_bNeedSendSFrame	= false;
		m_bNeedStopDT		= true;
		m_bNeedStartDT		= false;
		//m_bNeedSendTest		= false;
		m_bNeedStartDTcon	= false;
		m_bNeedStopDTcon	= false;
		//m_bNeedSendTestcon	= false;
	}

	bool CFepIEC104Prtcl::IsLogForRxDataProc()
	{
		return m_bDebugLog;
	}

	bool CFepIEC104Prtcl::IsLog()
	{
		return m_bLog;
	}

	bool CFepIEC104Prtcl::IsCheckSDI()
	{
		return m_bCheckSDI;
	}

	bool CFepIEC104Prtcl::IsCheckDDI()
	{
		return m_bCheckDDI;
	}

	bool CFepIEC104Prtcl::IsCheckVTI()
	{
		return m_bCheckVTI;
	}

	bool CFepIEC104Prtcl::IsLogValueError()
	{
		return m_bIsLogValueError;
	}
}

#endif
