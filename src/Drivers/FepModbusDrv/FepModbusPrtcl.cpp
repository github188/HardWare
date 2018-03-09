/*
*	\file	FepModbusPrtcl.cpp
*
*	\brief	ͨ��ModbusЭ����ʵ��Դ�ļ�
*
*	$Header: ?/MACS-SCADA 3.2 �������/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusPrtcl.cpp, 470+1 2011/04/22 01:56:47 xuhongtao $
*	$Author: xuhongtao $
*	$Date: 2011/04/22 01:56:47 $
*	$Revision: 470+1 $
*
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#ifndef _FEP_TEPIF_PRTCL_20080513_CPP_
#define _FEP_TEPIF_PRTCL_20080513_CPP_

#include "ace/High_Res_Timer.h"
#include "FepModbusDrvH.h"
#include "FepModbusPrtcl.h"
#include "FepModbusDriver.h"
#include "FepModbusDevice.h"
#include "RTDBDriverFrame/IODriver.h"
#include "RTDBCommon/Debug_Msg.h"

namespace MACS_SCADA_SUD
{
	void OnTimeOut(void* p);				//! ��ʱ��ʱ����ʱ�ص�����
	void PeriodProc(void* p);				//! ���ڶ�ʱ����ʱ�ص�����
	void InsertDelayProc(void* p);			//! ����������ʱ��ʱ����ʱ�ص�����

	//! ���ڶ�ʱ����ʱ�ص�����
	void PeriodProc( void* p )
	{
		//У�����
		CFepModbusPrtcl* pPrtcl = (CFepModbusPrtcl*)p;
		if ( pPrtcl == NULL )
		{
			return;
		}

		CFepModbusDriver* pDriver = (CFepModbusDriver*)pPrtcl->GetParam();
		if ( pDriver && pDriver->IsStopWork() )
		{
			return;
		}
		CFepModbusDevice* pDevice = NULL;
		char pchMsg[128] = {0};
		Byte bFlag[256] = {0};	//! �㲥��־,�±��ʾ�豸���͡�

		for ( int i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepModbusDevice*>( pDriver->GetDeviceByIndex(i) );
			//���ߵ��豸Ӧ�÷��������ˡ�ע�⣬��·�ϵĲ����豸����ʱ�����������豸�������ڣ�Ĭ��60�룩��������
			if (pDevice && pDevice->IsOffLine() && pDriver->m_nMaxRetryPeriod && pPrtcl->m_nCounter%pDriver->m_nMaxRetryPeriod == 0 )
			{
				pDevice->m_bRetryPeriod = true;
				if ( pDriver->IsLog())
				{
					//if (pDriver->IsEnglishEnv())
					//{
					//	snprintf(pchMsg, 128, "CFepModbusPrtcl::PeriodProc The Offline Device:%s Should try to Send a Request.", pDevice->GetName().c_str());
					//}
					//else
					//{
					//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc ��·�ϵ������豸:%sӦ�÷���������", pDevice->GetName().c_str());
					//}
					pDriver->WriteLog( pchMsg );
				}
			}

			//��������ֵ��
			if ( pDevice && pDevice->IsOnLine() && pDevice->GetSETTINGGROUP() )  
			{	
				int nRdSettingGpCycl = (pDevice->GetRdSettingGpCycl()+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;
				if ( pPrtcl->m_nCounter % nRdSettingGpCycl == 0 )				//!�豸���˶���ֵ���������
				{					
					pDevice->SetNeedRdSettingGp(true);		//�ö���ֵ���־			
				}
			}

			if ( pDevice && pDevice->IsOnLine() && pDevice->HasCheckTime() )	//!�豸��ҪУʱ
			{	
				int nCheckTime = (pDevice->GetCheckTimeCycl()*60000+PERIOD_SCAN_MS-1) / PERIOD_SCAN_MS;

				int nCheckTimeHour = (int)(pDevice->GetCheckTimeHour());	//! ��������ļ��е�Уʱʱ���(Сʱ) Add by: Wangyanchao
				int nCheckTimeMin = (int)(pDevice->GetCheckTimeMin());		//! ��������ļ��е�Уʱʱ���(����) Add by: Wangyanchao
				pPrtcl->gDateTime = ACE_OS::gettimeofday();		//! ��õ�ǰ��ʱ�� Add by: Wangyanchao
				ACE_Date_Time now;								//! Add by: Wangyanchao
				now.update( pPrtcl->gDateTime );				//!Add by: Wangyanchao

				if (pDevice->GetDevType() == DEVTYPE_855 && (pPrtcl->m_nCounter % SCANPERIOD_XRL855 == 0))	//! 855Э���豸ʹ�ö���Уʱ Edit by: Wangyanchao
				{
					if ( (now.hour() == nCheckTimeHour) && (now.minute() == nCheckTimeMin) )	//! Уʱʱ��㵽�� Add by: Wangyanchao
					{
						if ( pDevice->IsBroadChckTime() )	
						{
							if ( !bFlag[(Byte)pDevice->GetDevType()] )
							{
								pDevice->SetNeedCheckTime(true);	//��Уʱ��־
								bFlag[(Byte)pDevice->GetDevType()] = true;
							}
						}
						else	//��Ե��ʱ��ÿ���豸���ñ�־
						{
							pDevice->SetNeedCheckTime(true);	//��Уʱ��־
						}
					}
				} 

				else	//!��855�豸ʹ������Уʱ Edit by: Wangyanchao
				{
					if (pPrtcl->m_nCounter % nCheckTime == 0)
					{
						//! ʹ�ù㲥֡Уʱ����ͬ���͵������豸��ÿ��ֻ�跢��һ֡������Ҫ��ÿ���豸����
						if ( pDevice->IsBroadChckTime() )	
						{
							if ( !bFlag[(Byte)pDevice->GetDevType()] )
							{
								pDevice->SetNeedCheckTime(true);	//��Уʱ��־
								bFlag[(Byte)pDevice->GetDevType()] = true;
							}
						}
						else	//��Ե��ʱ��ÿ���豸���ñ�־
						{
							pDevice->SetNeedCheckTime(true);	//��Уʱ��־
						}
					}
				}
			}	
		}

		//�ж��豸����
		if ( pDriver->GetHot())
		{
			pDriver->m_nMaxLinkOffline = pDriver->m_MaxHotLinkOffline;
		}
		else
		{
			pDriver->m_nMaxLinkOffline = pDriver->m_nMaxStandbyLinkOffline;
		}
	
		if ( pDriver->m_nMaxLinkOffline > 0 && pPrtcl->m_nCounter % pDriver->m_nMaxLinkOffline == pDriver->m_nMaxLinkOffline -1 )
		{
			for ( int i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByIndex(i));
				if ( NULL != pDevice && !pDevice->IsSimulate() && !pDevice->IsDisable() )
				{
					//!��·��ʱ����Ӧ
					if (!pDriver->m_bIsLinkGetRightFrame)
					{
						if (!pDevice->IsOffLine())
						{

							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc The link Over: %d Seconds There is no ResponseFrame from Outer device ��Device:%s(Address:%d)is Offline now!", pDriver->m_nMaxLinkOffline, pDevice->GetName().c_str(), pDevice->GetAddr() );
							//}
							//else
							//{
							//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc ��·����%d�붼û���յ��κ��豸���� �豸%s������ Counter:%d��", pDriver->m_nMaxLinkOffline, pDevice->GetName().c_str(), pPrtcl->m_nCounter);
							//}
							pDriver->WriteLogAnyWay(pchMsg);							
						}
						pDevice->SetOnLine( DEVICE_OFFLINE );
					}
					else
					{
						//!��·���豸��ʱ����Ӧ
						if ( pDevice->m_byRxTimes == 0 )
						{
							if (!pDevice->IsOffLine())
							{

								//if (pDriver->IsEnglishEnv())
								//{
								//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc Device:%s Over:%d S no ResponseFrame from Outer device ��becomes Offline!Counter:%d", pDevice->GetName().c_str(), pDriver->m_nMaxLinkOffline);
								//}
								//else
								//{
								//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc �豸%s ����%d��û���յ����� ������!Counter:%d", pDevice->GetName().c_str(), pDriver->m_nMaxLinkOffline, pPrtcl->m_nCounter);
								//}
								pDriver->WriteLogAnyWay(pchMsg);						
							}
							pDevice->SetOnLine( DEVICE_OFFLINE );
						}
					}
					//ÿ�μ��󸴹������������,���������豸���ߺ���Щ�豸����Զ���ߡ�
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes = 0;
				}
			}
			//!������·���ձ��ı�ʶ
			pDriver->m_bIsLinkGetRightFrame = false;

		}
		else if ( pDriver->m_nMaxLinkOffline > 0 && pPrtcl->IsNetConn() && pDriver->m_nSerAddrNum > 1 && ((pPrtcl->m_nCounter % (pDriver->m_nMaxLinkOffline/pDriver->m_nSerAddrNum)) == (pDriver->m_nMaxLinkOffline/pDriver->m_nSerAddrNum -1)))
		{
			bool bFlag = true;
			for ( int i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByIndex(i));
				if ( NULL != pDevice )
				{
					if ( pDevice->m_byRxTimes > 0 )
					{
						pDevice->m_byRxTimes = 0;	//����Ҫ�������豸���ռ��������㣬��˲���break��
						bFlag = false;
					}
					else if(pDevice->IsSimulate() || pDevice->IsDisable())
					{
						bFlag = false;				//���豸��ģ��ʱ�����ж��豸�Ƿ��ѳ�ʱ��Ϊģ��ʱ�豸�����ͱ���
					}
				}
			}

			if ( bFlag )
			{
				char pchMsg[128] = {0};							
				//if (pDriver->IsEnglishEnv())
				//{
				//	snprintf(pchMsg, 128, "CFepModbusPrtcl::PeriodProc Current Link over:%d seconds There is no Response On the Current IP, switch to another IP Address", pDriver->m_nMaxLinkOffline/pDriver->m_nSerAddrNum);
				//}
				//else
				//{
				//	snprintf( pchMsg, 128, "CFepModbusPrtcl::PeriodProc ��ǰ��·����%d�붼û���յ��κ��豸���ģ��������������豸IP��", pDriver->m_nMaxLinkOffline/pDriver->m_nSerAddrNum );
				//}
				pDriver->WriteLogAnyWay(pchMsg);
				pDriver->SetConnected( false );
				pDriver->ReConnect( true );					
			}
		}

		pPrtcl->m_nCounter++;

		if ( pDriver->IsLog())
		{
			char pchLog[128] = {0};
			//! д��־
			sprintf( pchLog,"CFepModbusPrtcl::PeriodProc Counter = %d!!", pPrtcl->m_nCounter );
			pDriver->WriteLog( pchLog );
		}
	}

	//! ��ʱ��ʱ����ʱ�ص�����
	void OnTimeOut( void* p )
	{
		if (!p)
		{
			return;
		}

		CFepModbusPrtcl* pPrtcl = (CFepModbusPrtcl*)p;
		//!���Driver
		CFepModbusDriver* pDriver = (CFepModbusDriver*)pPrtcl->GetParam();
		if ( pDriver && pDriver->IsStopWork() )
		{
			return;
		}
		char pchInfo[256] = {0};
		int nTimerId = pPrtcl->m_pIOTimer->GetTimerID();
		int nRet = pPrtcl->m_pIOTimer->KillTimer();
		if ( nRet >= 0 )
		{
			if (pDriver->IsLog())
			{
				//sprintf( pchInfo, 
				//	"CFepModbusPrtcl:: OnTimeout Kill Timer Success, Timer Id:%d, KRes:%d", nTimerId, nRet );
				pDriver->WriteLog( pchInfo );
			}
		}
		else
		{
			if (pDriver->IsLog())
			{
				//sprintf( pchInfo, 
				//	"Error��CFepModbusPrtcl:: OnTimeout Kill Timer Error, Timer Id:%d", nTimerId, nRet );
				pDriver->WriteLog( pchInfo );
			}
		}

		//! �ϴη���֡���ǹ㲥
		if ( pPrtcl->GetLastAddr() > 0 && pPrtcl->GetLastAddr() < 256 )
		{
			//! ����ϴη���֡���豸
			CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>( 
				pDriver->GetDeviceByAddr( pPrtcl->GetLastAddr() ));

			if( pDevice )
			{
				//! ǿ��д��־
				if ( pPrtcl->m_nSndLen > 0 )
				{
					std::string szData = pPrtcl->GetCommData( pPrtcl->m_pbySndBuffer, pPrtcl->m_nSndLen, COMMDATA_SND);

					char pchLog[MAXDEBUGARRAYLEN];
					//if (pDriver->IsEnglishEnv())
					//{
					//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::OnTimeOut The Device:%s RcvTimeOut, The Request Frame:%s",pDevice->GetName().c_str(), szData.c_str());
					//}
					//else
					//{
					//	snprintf( pchLog, sizeof(pchLog), "�豸��Ӧ��ʱ,�豸����%s, ���η���֡��%s", pDevice->GetName().c_str(), szData.c_str());
					//}
					pDriver->WriteLogAnyWay( pchLog );
				}

				if (!pDevice->IsSimulate())
				{
					//! ���ӳ�ʱ����
					pDevice->IncrTrySendCount();
				}
				
				if ( pDevice->IsSimulate() )
				{
					if (pDriver->IsLog())
					{
						char pch[256];
						//if (pDriver->IsEnglishEnv())
						//{
						//	snprintf(pch, sizeof(pch), "CFepModbusPrtcl::OnTimeOut Device:%s RcvTimeOut but the Device is Simulate", pDevice->GetName().c_str());
						//}
						//else
						//{
						//	snprintf( pch, sizeof(pch), "CFepModbusPrtcl::OnTimeOut �豸:%s ���ճ�ʱ���豸����ģ��״̬", pDevice->GetName().c_str());
						//}
						pDriver->WriteLog( pch );
					}
				}
				
				//! ����Ǳ������ҵ�ǰ����ͨ�����֡������ݳ�ʱ�������ж���������
				if ( !pDevice->IsHot() && pPrtcl->IsRTUChannelTest())
				{
					pDevice->SetSlaveTimeOuts( pDevice->GetSlaveTimeOuts() + 1 );	//! �ӻ���ʱ��1
					pPrtcl->SetRTUChannelTest(false);								//! ���־
				}				
			}			
		}	

		//////////////////////////////////////////////////////////////////////////////
		//! ����������
		int nDataLen = 0;
		pPrtcl->BuildRequest( pPrtcl->m_pbySndBuffer, nDataLen );
		//////////////////////////////////////////////////////////////////////////////
	}

	//! ����������ʱ��ʱ����ʱ�ص�����	
	void InsertDelayProc( void*pParent )
	{
		//У�����
		CFepModbusPrtcl* pPrtcl = (CFepModbusPrtcl*)pParent;
		if( !pPrtcl )
		{
			return;
		}

		//�رն�ʱ��
		pPrtcl->m_pInsertDelayTimer->KillTimer();

		pPrtcl->SetCtrlInsertLen( pPrtcl->GetCtrlInsertTempLen() );
		pPrtcl->SetCtrlInsertTempLen( 0 );
	}


	///////////////////////////////////////////////////////////////////////////
	std::string CFepModbusPrtcl::s_ClassName("CFepModbusPrtcl");	//! ����

	CFepModbusPrtcl::CFepModbusPrtcl(void) 
	{
		//! ������ʱ��
		m_nCounter = 0;
		m_pTimerMgr = new ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>();
		ACE_High_Res_Timer::global_scale_factor (); 
		m_pTimerMgr->timer_queue()->gettimeofday( ACE_High_Res_Timer::gettimeofday_hr );
		size_t stack_size[1]={2*1024*1024};
		m_pTimerMgr->activate(THR_NEW_LWP|THR_DETACHED|THR_INHERIT_SCHED,1,0,ACE_DEFAULT_THREAD_PRIORITY,-1,0,0,0,stack_size);
		m_pIOTimer = new CIOTimer( OnTimeOut, this, true, m_pTimerMgr);
		m_pInsertDelayTimer = new CIOTimer(InsertDelayProc, this, true, m_pTimerMgr );
		m_pPeriodTimer = new CIOTimer( PeriodProc, this, false, m_pTimerMgr);

		m_nCtrlInsertLen = 0;
		m_nCtrlInsertTempLen = 0;
		m_nCtrlSendBufferLen = 0;			//! �ϴη�����Ϣ��Ч����

		m_nMaxResend = 0;					//! ����ط�����
		m_nCurResend = 0;					//! ��ǰ�ظ�����		

		m_byPrtclType = 0;					//! Ĭ����Modbus RTU
		m_wdPacketNo = 0;					//! ����Ԫ��ʶ������ţ�

		m_bycmdType = 0;
		m_fCmdVal = 0;
		m_bRTUChannelTest = false;			//! ��ʼ��ͨ�����֡Ϊfalse

		m_pSendData = NULL;
		m_bIsFirstTime = true;
		m_bIsBroadFrame = false;

		m_nSndLen = 0;
	}

	CFepModbusPrtcl::~CFepModbusPrtcl(void)
	{
		if(m_pTimerMgr!=NULL)
		{
			m_pTimerMgr->deactivate();
			//delete m_pTimerMgr;
		}
		if ( m_pSendData )
		{
			delete []m_pSendData;
			m_pSendData = NULL;
		}

		if ( m_pIOTimer )
		{
			m_pIOTimer->KillTimer();
			//delete m_pIOTimer;
			//m_pIOTimer = NULL;
		}

		if ( m_pPeriodTimer )
		{
			m_pPeriodTimer->KillTimer();
			//delete m_pPeriodTimer;
			//m_pPeriodTimer = NULL;
		}

		if ( m_pInsertDelayTimer )
		{
			m_pInsertDelayTimer->KillTimer();
			//delete m_pInsertDelayTimer;
			//m_pInsertDelayTimer = NULL;
		}
	}

	/*!
	*	\brief	��ʼ������
	*/
	void CFepModbusPrtcl::Init( Byte byPrtclType, int nMaxReSend  )
	{
		m_byPrtclType = byPrtclType;
		m_nMaxResend = nMaxReSend;		

		if( m_byPrtclType == 1 )			//!< Modbus TCP
		{
			InitPrtclHelper( 9, MB_TCP_MAX_SEND_SIZE,
				MB_TCP_MAX_SEND_SIZE, MB_TCP_MAX_SEND_SIZE, 2*MB_TCP_MAX_SEND_SIZE );
			//! Ϊ�������ݴ洢������ռ�
			m_pSendData = new SENDDATA[65536];
		}
		else
			InitPrtclHelper( 5, MB_RTU_MAX_SEND_SIZE,
			MB_RTU_MAX_SEND_SIZE, MB_RTU_MAX_SEND_SIZE, 2*MB_RTU_MAX_SEND_SIZE );
	}

	std::string CFepModbusPrtcl::GetClassName()
	{
		return s_ClassName;
	}

	//! �õ���С�İ�����
	int CFepModbusPrtcl::GetMinPacketLength()
	{
		return PRTCL_MIN_PACKET_LENGTH;
	}

	/*!
	*	\brief	������֡
	*
	*	\param	pbyData		֡�׵�ַ
	*	\param	nDataLen	֡��
	*
	*	\retval	int	0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusPrtcl::BuildRequest( Byte* pbyData, int& nDataLen )
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		if (NULL == pDriver)
		{
			return -1;
		}
		
		char pchLog[MAXDEBUGARRAYLEN] =  {0};
		bool bIsRecord = pDriver->IsLog();
		CFepModbusDevice* pDevice = NULL;
		int nTimeOut = 500;			//!��ʱʱ�䣬����֡�����豸��ʱʱ��ȷ��
		int nFlag = 1;
		int nRet = 0;
		nDataLen = 0;
		
		/*
		 *	��֡���Ա㷢��
		 *	�鵽���ʵ�֡��break��ִ�з��Ͳ���
		 *	��֡ʧ����ֱ��break�Ա��ڶ�ʱ��
		 */
		while(nFlag)
		{
			nFlag = 0;
			if (pDriver->IsLog())
			{
				//snprintf(pchLog, MAXDEBUGARRAYLEN, "\n\nCFepModbusPrtcl::BuildRequest Start!");
				pDriver->WriteLog(pchLog);
			}

			if ( CIOPrtclParser::BuildRequest( pbyData, nDataLen )==-1 )
			{
				if (pDriver->IsLog())
				{
					//snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl:: BuildRequest Failed, The Driver is Diable!" );
					////! ��¼��־
					//pDriver->WriteLog( pchLog );
				}
				nRet = -1;
				break;
			}

			//! �����������ʣ���û�����ӣ��򲻷���ֱ�ӷ���
			if( ( IsNetConn() )
				&& ( /*!pDriver->m_pIODrvBoard->ConnState() || */!pDriver->GetConnected() ))
			{
				nRet = -4;
				break;
			}

			//! �ж��Ƿ���Ҫ�ط�
			if ( m_nMaxResend > 0					//ֻ������Ϊ��Ҫ�ط��������ſ����ط�
				&& m_nCurResend < m_nMaxResend		//�����ط�������Χ��
				&& m_nCtrlSendBufferLen > 0			//ȷ���ϴη�����
				&& m_pbyCtrlSendBuffer[0] != 0 		//�㲥֡���ط�
				&& ( m_pbyCtrlSendBuffer[1] == 5 || m_pbyCtrlSendBuffer[1] == 6 
				|| m_pbyCtrlSendBuffer[1] == 15 
				|| 	m_pbyCtrlSendBuffer[1] == 16 )	//ң��֡����Ҫ�����ط�
				)
			{
				//! ȡ�÷���֡���豸
				pDevice = dynamic_cast<CFepModbusDevice*>( 
					pDriver->GetDeviceByAddr( m_pbyCtrlSendBuffer[0] ) );				

				if ( pDevice && pDevice->IsOnLine() )	//�豸����ʱ�ſ����ط�
				{
					//! ������ʱ
					ACE_Time_Value tv( 0, (pDevice->GetPortCfg().nSndDelay)*1000 );
					ACE_OS::sleep( tv );

					m_nCurResend++;
					if ( m_nCurResend >= m_nMaxResend )
					{
						m_nCurResend = m_nMaxResend;
					}
					nDataLen = m_nCtrlSendBufferLen;
					memcpy( pbyData, m_pbyCtrlSendBuffer, nDataLen );

					//! �ó�ʱʱ��
					nTimeOut = pDevice->GetPortCfg().nTimeOut;
					if (pDriver->IsLog())
					{
						//if (pDriver->IsEnglishEnv())
						//{
						//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest Dev:%s Refuse to Action,Starting %d times Retry");
						//}
						//else
						//{
						//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest �豸��ַ%d�ܶ�����ʼ��%d������!", pDevice->GetAddr(), m_nCurResend);
						//}
						//pDriver->WriteLog( pchLog );
					}
					break;
				}
				else	
					m_nCurResend = m_nMaxResend;
			}	

			//��鲢����ң�ذ���֡(����֡����һ��ң������)
			if ( m_nCtrlInsertLen > 0 )
			{
				//! ȡ�÷���֡���豸
				pDevice = dynamic_cast<CFepModbusDevice*>( 
					pDriver->GetDeviceByAddr( m_pbyCtrlInsertBuf[0] ) );

				if ( pDevice )
				{
					//! ������ʱ
					ACE_Time_Value tv( 0, (pDevice->GetPortCfg().nSndDelay)*1000 );
					ACE_OS::sleep( tv );
				}

				nDataLen = m_nCtrlInsertLen;
				memcpy( pbyData, m_pbyCtrlInsertBuf, m_nCtrlInsertLen );
				m_nCtrlInsertLen = 0;
				break;
			}

			//! ��ң��,�ж������ң��,��ң��
			nRet = BuildCtrlFrame( pbyData, nDataLen, nTimeOut, m_fCmdVal);
			if ( nDataLen > 0 && nRet == 0 )
			{
				/*
				 *	��¼ң�����ݣ��Ա��ط����˴�ֻ��¼ΪRTU��ʽ֡��
				 *	�����ط������������Ҫ��תΪTCP��ʽ֡
				 */
				ACE_OS::memset(m_pbyCtrlSendBuffer, 0, MB_RTU_MAX_SEND_SIZE);
				if (nDataLen < MB_RTU_MAX_SEND_SIZE)
				{
					ACE_OS::memcpy(m_pbyCtrlSendBuffer, pbyData, nDataLen);
					m_nCtrlSendBufferLen = nDataLen;
				}
				else
				{
					ACE_OS::memcpy(m_pbyCtrlSendBuffer, pbyData, MB_RTU_MAX_SEND_SIZE);
					m_nCtrlSendBufferLen = MB_RTU_MAX_SEND_SIZE;
				}
				//! ȡ�÷���֡���豸
				pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByAddr( m_pbyCtrlSendBuffer[0]));
				m_nCurResend = 0;	//! ����һ�ε��ظ�����������
				break;
			}

			//!MODBUS RTU���ҽ��յ������������ϰ�,��ֱ�ӷ���
			if ( m_bSendPacketIsFromPeer && m_cmdPacket.nLen>0 && ( m_byPrtclType == 0 || m_byPrtclType ==2) )
			{
				memcpy( pbyData, m_cmdPacket.byData, m_cmdPacket.nLen );
				nDataLen = m_cmdPacket.nLen;

				//! ȡ�÷���֡���豸
				pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByAddr( pbyData[0]));
				if ( !pDevice )
				{
					m_bSendPacketIsFromPeer = false;
					m_cmdPacket.nLen = 0;
					nRet = -1;
					break;
				}

				if (pDriver->IsLog())
				{
					std::string strContent = GetCommData(pbyData, nDataLen, COMMDATA_SND);
					//if (pDriver->IsEnglishEnv())
					//{
					//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest Send LinkDiagFrame Dev:%s", pDevice->GetName().c_str());
					//}
					//else
					//{
					//	snprintf( pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest �յ�������·��ϰ� Frame:%s �豸:%s!", strContent.c_str(), pDevice->GetName().c_str());
					//}
					pDriver->WriteLog( pchLog );
				}

				//! �ó�ʱʱ��
				nTimeOut = pDevice->GetPortCfg().nTimeOut;
				m_bSendPacketIsFromPeer = false;
				m_cmdPacket.nLen = 0;
				m_bycmdType = 0;
				m_bRTUChannelTest = true;			//! ��ͨ����ϱ�־Ϊtrue

				/*
				 *	������ڻ������е����ݣ���ЩӦ�ö����������֡����Ӧ֡��
				 *	�Ա�ӵ�������Ӧ֡��Ľ���
				 */
				m_iInnerLen = 0;
				nRet = 0;
				break;
			}

			//! �����豸
			pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetNextDevice());
			if( !pDevice )
			{
				nRet = -1;
				break;
			}

			//! �ж��豸�Ƿ�ģ���ˣ������ģ���ˣ��򲻷���������
 			if ( pDevice->IsSimulate() )
 			{
 				//! д��־
 				sprintf( pchLog,"CFepModbusPrtcl::BuildRequest Device:%s Is Simulated!", pDevice->GetName().c_str());
 				pDriver->WriteLog( pchLog );
 				nRet = -2;
 				break;
 			}

			//! ��MODBUS RTU������Ҫ������·���
			if( m_byPrtclType == 0 ||  m_byPrtclType == 2)
			{
				//! ������·���
				int nRest = 0;
				int nLinkDataLen = 0;
				nRest = DealLinkClash( pDevice, pbyData, nLinkDataLen );
				if ( nRest==0 )	//! �����·��ϰ�,��������
				{
					//!��ͨ����ϱ�־Ϊtrue
					m_bRTUChannelTest = true;
					//!�ó�ʱʱ��
					nTimeOut = pDevice->GetPortCfg().nTimeOut*2;
					//!�ָ������豸ͨ�����״̬ÿ��ֻ��һ���豸����ͨ�����
					pDriver->SetDevCHNCheck(false);

					if (pDriver->IsLog())
					{
						std::string strContent = GetCommData(pbyData, nLinkDataLen, COMMDATA_SND);
						//if (pDriver->IsEnglishEnv())
						//{
						//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest Build LinkDaigFrame and Send to Peer!Device:%s,Frame:%s", pDevice->GetName().c_str(), strContent.c_str());
						//}
						//else
						//{
						//	snprintf( pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildRequest ����·��ϰ������͸����!Device:%s,Frame:%s",pDevice->GetName().c_str(), strContent.c_str());
						//}
						pDriver->WriteLog( pchLog );
					}
					nRest = 0;
					break;
				}
			}

			//! ������ʱ
			ACE_Time_Value tv( 0, (pDevice->GetPortCfg().nSndDelay)*1000 );
			ACE_OS::sleep( tv );

			/*
			 *	����MODBUS RTU�������·��ͻ���豸Ϊʵ������
			 *				����·����ͻ���������������������
			 *	����MODBUS TCP����ֱ�ӷ���
			 */
			if ( ( pDriver->IsPortAccessClash() && pDevice->IsHot() ) 
				|| !pDriver->IsPortAccessClash() 
				|| ( m_byPrtclType != 0 &&  m_byPrtclType != 2 ))
			{
				nDataLen = pDevice->BuildLocalNormalFrame( pbyData );
				if (nDataLen > 0)
				{
					//!�������Ч���ĲŰ�����·��ʱ���еȴ����򾡿�����´���ѯ
					nTimeOut = pDevice->GetPortCfg().nTimeOut;
				}
				m_bycmdType = 0;
			}
		}

		//��������ɹ���񣬶����·��ͳ��ȡ�����ӻ�û�з�������ʱ���ڽ��ճ�ʱ��ʱ���д����д����������־
		m_nSndLen = nDataLen;

		//! ��֡��ȷ��֡���ȴ���0������Э�����ͣ�RTU��TCP�����б��ݴ���
		if (nRet == 0 && nDataLen > 0)
		{			
			//! ���ݲ���������֡
			BackupAndSendRequest( pDevice, pbyData, nDataLen );	
		}		

		//! ���ǹ㲥֡��������ʱ����
		if ( !m_bIsBroadFrame )
		{
			//!��ֹͬʱ����������ʱ��
			int nTId = m_pIOTimer->GetTimerID();
			int nRe = m_pIOTimer->KillTimer();
 			if (pDriver->IsLog())
 			{
 				//! д��־
 				//snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::BuildRequest Kill Timer Id:%d Res:%d!", nTId, nRe );
 				pDriver->WriteLogAnyWay( pchLog );
 			}
			
			//! ������ʱ��
			if( m_pIOTimer->SetTimer(nTimeOut) == 0 )
			{
				if (pDriver->IsLog())
				{
					//snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::BuildRequest SetTimer Success, TimerID = %d!", m_pIOTimer->GetTimerID() );
					pDriver->WriteLogAnyWay( pchLog );
				}
			}
			else
			{
				if (pDriver->IsLog())
				{
					//snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::BuildRequest SetTimer Error, TimerID = %d!", m_pIOTimer->GetTimerID() );
					pDriver->WriteLog( pchLog );
				}
			}
		}
		else
		{
			m_bIsBroadFrame = false;
		}

		return nRet;
	}

	/*!
	*	\brief	���ݲ���������
	*
	*	\param	pDevice		����֡���豸
	*	\param	pbyData		���ݰ��׵�ַ
	*	\param	nDataLen	���ݰ�����
	*
	*	\retval	bool	�ɹ�true������false
	*/
	int CFepModbusPrtcl::BackupAndSendRequest( 
		CFepModbusDevice* pDevice, Byte* pbyData, int nDataLen )
	{
		if( !pbyData || nDataLen <= 0)
			return -1;

		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

		/////////////////////////////////////////////////////////////////////////
		//! �����֡������m_pbySndBuffer�н��У�
		//! �򿽱���m_pbySndBuffer�������У��Ա�������ܵ���Ӧ֡��ķ����ʹ���
		if( pbyData != m_pbySndBuffer)
		{
			memcpy( m_pbySndBuffer, pbyData, nDataLen );
		}

		//! ��¼�ϴη���֡����Ϣ������У��
		BackupLastRequest();

		//! �����TCP Modbus����ת����TCP��ʽ֡������¼
		if( IsModbusTCP() )
		{
			nDataLen = MBRTU2MBTCPRequest( pbyData, nDataLen, pbyData );

			if( nDataLen > 0 )
			{
				FRAME_HEAD* pHead = (FRAME_HEAD* )pbyData;
				WORD PacketNo =  MAKEWORD(pHead->byPckNoLow, pHead->byPckNoHigh);
				m_pSendData[PacketNo].bySendFunc = m_bySendFunc;
				m_pSendData[PacketNo].wdSendStartAddr = m_wdSendStartAddr;
				m_pSendData[PacketNo].wdSendPoints = m_wdSendPoints;
			}
		}

		//! ��֡��ȷ��֡���ȴ���0����������֡
		if ( nDataLen > 0)
		{
			//! ��ȡ����֡���豸
			if( !pDevice )
			{
				pDevice = dynamic_cast<CFepModbusDevice*>( 
					pDriver->GetDeviceByAddr( m_bySendAddr ));

				if ( !pDevice )
				{
					return -2;
				}
			}

			//! ���ʹ�����1
			pDevice->m_byTxTimes ++;			

			//! ��������֡
			SendRequest( pDevice, pbyData, nDataLen );			

			//! ����ǹ㲥֡����ֱ�ӷ�����һ֡
			if( m_bySendAddr == 0 && pDevice->GetAddr() != 0 )
			{
				//! ��ʱ100����
				ACE_Time_Value tv( 0, 100 * 1000 );
				ACE_OS::sleep( tv );

				///////////////////////////////////////////////////////////////////////
				//! ��������
				int nDataLen = 0;
				BuildRequest( m_pbySndBuffer, nDataLen );
				///////////////////////////////////////////////////////////////////////

				//! �ù㲥֡��־����ֹ�ظ�������ʱ��
				m_bIsBroadFrame = true;
			}
		}

		return 0;
	}

	/*!
	*	\brief	��������
	*
	*	\param	pDevice		����֡���豸
	*	\param	pbyData		���ݰ��׵�ַ
	*	\param	nDataLen	���ݰ�����
	*
	*	\retval	bool	�ɹ�true������false
	*/
	int CFepModbusPrtcl::SendRequest( 
		CFepModbusDevice* pDevice, Byte* pbyData, int nDataLen )
	{
		if( !pbyData || nDataLen <= 0)
			return -1;

		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		m_nSndLen = nDataLen;

		if (pDriver->m_pIODrvBoard)
		{
			//! ��������
			if( pDriver->m_pIODrvBoard->Write(pbyData, nDataLen) != 0)
			{
				//! д��־
				if ( nDataLen>0 && pDriver->IsMonitor())
				{
					pDriver->WriteCommData( pbyData, nDataLen, COMMDATA_SND );
				}
				if (pDriver->IsLog())
				{
					pDriver->WriteLog( "CFepModbusPrtcl::SendRequest Write Failed!!" );
				}
				return -3;
			}
			else				//! ���ͳɹ����¼��־�����ͨѶ����
			{
				//! �����ң�ػ�ң��
				if( m_bycmdType == 1 || m_bycmdType == 2 )
				{
					pDevice->SetVal(
						m_tCtrlRemoteCmd.byUnitType,
						m_tCtrlRemoteCmd.lTagAddress,
						m_tCtrlRemoteCmd.byData,
						m_tCtrlRemoteCmd.nDataLen,
						0, true );
				}

				//! д��־
				pDriver->WriteLog( "CFepModbusPrtcl::Send Request Success!" );
				//! ��¼ͨ�ż���
				if ( nDataLen>0 && pDriver->IsMonitor())
				{
					pDriver->WriteCommData( pbyData, nDataLen, COMMDATA_SND );
				}
			}
		}

		return 0;
	}

	//! ��ָ���豸��ͨ���������֡
	void CFepModbusPrtcl::MakeChannelDiagFrame( 
		CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen )
	{
		CFepModbusDevice* pDev = dynamic_cast<CFepModbusDevice*> ( pDevice );
		if ( !pDev || pDev->IsSimulate() )
		{
			return;
		}

		//! ����в���֡
		if( pDev->GetTestReqStatus() )
			nReturnLen = pDev->BuildTestFrame( pbyData );
		else
			nReturnLen = pDev->BuildQueryFrame( pbyData ); 

		//! �������ݣ��Ա�������͵�֡�����յ���Ӧ������ȷ����
		/////////////////////////////////////////////////////////////////////////////
		//! ��¼�ϴη���֡����Ϣ������У��
		m_bySendAddr = pbyData[0];
		m_bySendFunc = pbyData[1];
		m_wdSendStartAddr = MAKEWORD( pbyData[3], pbyData[2] );
		m_wdSendPoints = MAKEWORD( pbyData[5], pbyData[4] );
	}	

	/*!
	*	\brief	�������������ݰ�
	*
	*	\param	pPacketBuffer	�����������ݰ�
	*	\param	pnPacketLength	���ݰ�����
	*
	*	\retval	bool	�ɹ�true������false
	*/
	bool CFepModbusPrtcl::ExtractPacket(  Byte* pPacketBuffer, int* pnPacketLength )
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

		//У��
		if( !pPacketBuffer || !pDriver || !m_pbyRevBuffer)
			return false;
		 
		bool bACompletePacketRcvd = false;
		Byte* pFrame = m_pbyRevBuffer;
		int nCorrectLen = 0;		//!��ȷ֡Ӧ�еĳ���
		bool bHasReqest = false;			//!�Ƿ�ӽ����������н�����Modbus����֡
		char pchLog[MAXDEBUGARRAYLEN] = {0};

		//!У��֡�ĺ�����
		while(pFrame + PRTCL_MIN_PACKET_LENGTH <= m_pbyRevBuffer + m_nRevLength )
		{
			//////////////////////////////////////MODIFYHERE START
			//���������
			//   ---------------------------------------------------
			//   |                  |                              |
			//m_pbyRevBuffer     pFrame
			//   |<-----------------m_nRevLength------------------>|
			///////////////////////////////////////MODIFYHERE END
			if( IsModbusTCP() )				//! TCP Modbus
			{
				//! ���ݼ�¼�ķ�����Ϣ�ҵ����ʵ�����֡֡ͷ
				Byte bFlag = false;
				int i = 0;
				for ( i = 0; 
					i < (int)( m_nRevLength-sizeof(FRAME_HEAD)-(pFrame - m_pbyRevBuffer) );
					i++ )
				{
					FRAME_HEAD* pHead = (FRAME_HEAD*)(m_pbyRevBuffer+i);
					if ( pHead->byPrtclHigh != 0X00 
						|| pHead->byPrtclLow != 0X00
						|| pHead->bySlaveAddress != m_bySendAddr )
					{
						continue;
					}

					WORD PacketNo = MAKEWORD( pHead->byPckNoLow, pHead->byPckNoHigh );
					Byte byFunc	 = m_pbyRevBuffer[i+sizeof(FRAME_HEAD)];

					if ( ( byFunc&0x7F ) == m_pSendData[PacketNo].bySendFunc )
					{
						bFlag = true;
						break;
					}
				}
				if ( !bFlag ) //! �ڽ���������û���ҵ���ַ�͹�����
				{
					break;
				}
				//! �ƶ�pFrame����ǰλ
				pFrame += i;
				m_byActiveAddr = pFrame[sizeof(FRAME_HEAD)-1];
				m_byActiveFunc = pFrame[sizeof(FRAME_HEAD)];
			}
			else							//! RTU Modbus
			{
				if ( pDriver->GetHot() || !pDriver->IsPortAccessClash() )
				{
					//! ������һ�η���֡��Ϣ���ڽ����������ҵ���ַ�͹�����
					Byte bFlag = false;
					int i = 0;
					for( i=0; i < m_nRevLength-1-(pFrame - m_pbyRevBuffer); i++ )
					{
						if ( pFrame[i] == m_bySendAddr && (pFrame[i+1] & 0x7F) == m_bySendFunc)	
						{
							bFlag = true;
							break;
						}
					}
					if ( !bFlag ) //! �ڽ���������û���ҵ���ַ�͹�����
					{
						break;
					}
					//! �ƶ�pFrame����ǰλ
					pFrame += i;
					m_byActiveAddr = pFrame[0];
					m_byActiveFunc = pFrame[1];
				}
				else //����Ǵӻ�������Ϊ��ͻģʽʱ�����ȳ��Դӽ����������н�����Modbus����֡
				{
					if ( !bHasReqest )
					{
						if ( m_nRevLength + m_pbyRevBuffer - pFrame >= 8 )
						{
							if ( pFrame[0] != 0 )	//�豸��ַ0ֻ������վ���͵Ĺ㲥֡
							{
								bool bIsReqeust = false;
								CFepModbusDevice* pDevice = NULL;
								if ( pFrame[1] == 1 || pFrame[1] == 2 ) 
								{
									if ( pFrame[2] != 3 )//ע�⣬8���ֽڳ��Ĺ�����1��2����Ӧ֡���ֽڸ���ֻ��Ϊ3
									{
										pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByAddr( pFrame[0] ) );
										if ( pDevice )
										{
											bIsReqeust = true;
										}
									}
									else if ( pFrame[4] == 0 && pFrame[5] >= 17 && pFrame[5] <= 24 )
									{
										pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByAddr( pFrame[0] ) );
										if ( pDevice )
										{
											if ( pDevice->IsReqeustExited( pFrame[1], MAKEWORD(pFrame[3],pFrame[2]), MAKEWORD(pFrame[5],pFrame[4]) ))
											{
												bIsReqeust = true;
											}
										}
									}
								}
								else if ( pFrame[1] == 3 || pFrame[1] == 4 )	  //������3��4��8�ֽ�ֻ֡��������֡��
								{
									pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByAddr( pFrame[0] ) );
									if ( pDevice )
									{
										if ( pDevice->IsReqeustExited( pFrame[1], MAKEWORD(pFrame[3],pFrame[2]), MAKEWORD(pFrame[5],pFrame[4]) ))
										{
											bIsReqeust = true;
										}
									}
								}

								if ( bIsReqeust )
								{
									if ( pDevice )
									{
										//! У��CRC
										int nCorrectLen = 8;
										WORD CheckSum = CRC( pFrame, nCorrectLen - 2 );
										if (  (( pDevice->GetPortCfg().nCRCOrder == CRC_HOB_FIRST) && ( pFrame[nCorrectLen-2] == HIBYTE(CheckSum) && pFrame[nCorrectLen-1] == LOBYTE(CheckSum) ) )
											||(( pDevice->GetPortCfg().nCRCOrder == CRC_LOB_FIRST) && ( pFrame[nCorrectLen-2] == LOBYTE(CheckSum) && pFrame[nCorrectLen-1] == HIBYTE(CheckSum) ) ) )
										{
											//! ���浽���ط��ͻ��棬�Ա��ӻ��´��յ���Ӧʱ�ܳɹ���������
											memcpy( m_pbySndBuffer, pFrame, nCorrectLen );

											//! ��¼����֡����Ϣ������У��
											BackupLastRequest();

											if ( pDriver->IsLog())
											{
												//char pchLog[MAXDEBUGARRAYLEN];
												std::string szSndPacket = GetCommData( pFrame, nCorrectLen, COMMDATA_SND);
												//if (pDriver->IsEnglishEnv())
												//{
												//	snprintf( pchLog, sizeof(pchLog),"CFepModbusPrtcl::ExtractPacket the Peer Recongnize RequestFrame sending by Peer,Content: %s", szSndPacket.c_str() );
												//}
												//else
												//{
												//	snprintf( pchLog, sizeof(pchLog),"CFepModbusPrtcl::ExtractPacket �ӻ�ʶ���������͵�����֡: %s", szSndPacket.c_str() );
												//}
												pDriver->WriteLog( pchLog );
											}

											//! ȥ���������������
											m_nRevLength -= pFrame - m_pbyRevBuffer + nCorrectLen;
											if ( m_nRevLength > 0 ) 
											{
												memmove( m_pbyRevBuffer, pFrame + nCorrectLen, m_nRevLength );
											}

											//! ���ʣ�೤�Ȳ��㣬��ȴ��´δ������ջص��ٳ��Խ���
											if ( m_nRevLength <= PRTCL_MIN_PACKET_LENGTH )
											{
												break;
											}
											bHasReqest = true;

											//! �ƶ�pFrame����ǰλ���������
											pFrame = m_pbyRevBuffer;
											continue;
										}
										else
										{
											pFrame++;
											continue;
										}
									}
									else
									{
										pFrame++;
										continue;
									}
								}
								else
								{
									pFrame++;
									continue;
								}
							}
							else
							{
								pFrame++;
								continue;
							}
						}
						else
						{
							//! ����ӻ�û�дӽ����������н���������֡�����˻�pFrame����ʼλ�ú󣬳��Ժ�����Ӧ֡�Ľ���
							bHasReqest = true;
							pFrame = m_pbyRevBuffer;
							continue;
						}
					}

					//���濪ʼ������Ӧ֡�Ľ���
					//! ������һ�η���֡��Ϣ���ڽ����������ҵ���ַ�͹�����
					Byte bFlag = false;
					int i = 0;
					for( i=0; i < m_nRevLength-1-(pFrame - m_pbyRevBuffer); i++ )
					{
						if ( pFrame[i] == m_bySendAddr && (pFrame[i+1] & 0x7F) == m_bySendFunc)	
						{
							bFlag = true;
							break;
						}
					}

					if ( !bFlag ) //! �ڽ���������û���ҵ���ַ�͹�����
					{
						break;
					}

					//! �ƶ�pFrame����ǰλ
					pFrame += i;
					m_byActiveAddr = pFrame[0];
					m_byActiveFunc = pFrame[1];
				}
			}	

			//! ���Խ���ң�ط���֡
			int nResult = ExtractCtrlPacket( pFrame, nCorrectLen );
			if (nResult == FLAG_NONE)		//! ����ң�ط���֡
			{
				//! ���Խ�����ѯ֡���ذ�
				nResult = ExtractMsgPacket(pFrame, nCorrectLen);
			}

			if (nResult != FLAG_CORRECTFRAME)
			{
				if (nResult == FLAG_NONE)		//! Ҳ������ѯ֡���ذ�
				{
					//! ���Խ����ļ��������ذ�
					nResult = ExtractFilePacket(pFrame, nCorrectLen);
				}
				if (nResult == FLAG_NONE)		//! Ҳ�����ļ��������ذ�
				{
					//! ���Խ���������������Ӧ֡
					nResult = ExtractOtherPacket(pFrame, nCorrectLen);
				}

				if (nResult == FLAG_UNCOMPLETE)	//! ֡û��������
				{
					break;
				}
				//! ������������������ǰ֡
				if (nResult == FLAG_ERROR || nResult == FLAG_NONE)
				{
					pFrame++;
					continue;
				}
			}
			

			//! ��������ȷ��֡��û�г��������ñ�־���˳�ѭ��
			if ( nResult == FLAG_CORRECTFRAME)
			{
				bACompletePacketRcvd = true;
			}

			break;
		}

		if (bACompletePacketRcvd)
		{
			m_bycmdType = 0;	//! ��ԭ����֡����Ϊ0

			//! ���ϸ��֡���õ����������
			ACE_OS::memcpy( pPacketBuffer, pFrame, nCorrectLen );

			//! ȥ���������������
			m_nRevLength -= pFrame - m_pbyRevBuffer + nCorrectLen;
			if ( m_nRevLength > 0 ) 
			{
				ACE_OS::memmove( m_pbyRevBuffer, pFrame + nCorrectLen, m_nRevLength );
			}

			//! ����ȷӦ��֡�ĳ���
			*pnPacketLength = nCorrectLen;
			if ( pDriver->IsLog())
			{
				//! д��־
				//char pchLog[MAXDEBUGARRAYLEN];
				std::string szPacket = GetCommData( pPacketBuffer, nCorrectLen, COMMDATA_RCV );
				//snprintf( pchLog, sizeof(pchLog),"CFepModbusPrtcl::ExtractPacket Get a complete frame: %s", szPacket.c_str() );
				pDriver->WriteLogAnyWay( pchLog );
			}
		}
		else
		{
			//ȥ������ȷ��������ݲ���
			int nErrorLen = pFrame - m_pbyRevBuffer;
			if ( nErrorLen > 0 )
			{
				//char pchLog[MAXDEBUGARRAYLEN];
				std::string szErrorPacket = GetCommData( m_pbyRevBuffer, nErrorLen, COMMDATA_RCV );
				//if (pDriver->IsEnglishEnv())
				//{
				//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::ExtractPacket Remove the invalid buffer length:%d Content:%s", nErrorLen, szErrorPacket.c_str());
				//}
				//else
				//{
				//	snprintf( pchLog, sizeof(pchLog),"CFepModbusPrtcl::ExtractPacket ȥ����������,��%d�ֽ�: %s", nErrorLen, szErrorPacket.c_str());
				//}
				pDriver->WriteLog( pchLog );
				m_nRevLength -= nErrorLen;
				if ( m_nRevLength > 0 )
				{				
					ACE_OS::memmove(m_pbyRevBuffer, m_pbyRevBuffer+nErrorLen, m_nRevLength);
				}
				else
				{
					m_nRevLength = 0;
				}
			}
		}
		return bACompletePacketRcvd;	
	}

	/*!
	*	\brief	�������������ݰ�
	*
	*	\param	pPacketBuffer	���ݰ�
	*	\param	nPacketLength	���ݰ�����
	*	\param	bFromLocal		�����Ǳ������Ļ����������
	*
	*	\retval	void
	*/
	void CFepModbusPrtcl::ProcessRcvdPacket( Byte* pPacketBuffer, int nPacketLength, bool bFromLocal )
	{
		int nFlag = 1;
		char pchLog[MAXDEBUGARRAYLEN] = {0};
		while(nFlag)
		{
			nFlag = 0;       //! �޸ı�־��ʹִֻ��һ��
			
			//! ���Driver
			CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
			if (!pDriver)
				break;

			//! KillTimer
			//int nTId = m_pIOTimer->GetTimerID();
			//int nKRe = m_pIOTimer->KillTimer();
			//if (pDriver->IsLog())
			//{
			//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket Kill Timer, Id:%d, Res:%d", nTId, nKRe);
			//	//! д��־
			//	pDriver->WriteLogAnyWay(pchLog);
			//}

			//У��
			if( !pPacketBuffer || nPacketLength == 0 )
				break;			

			//! ���ݵ�ַ�ҵ���Ӧ���豸
			Byte byUnitAddr;
			bool IsExceptionResp = false;		//�Ƿ�Ϊ�쳣Ӧ��
			if( IsModbusTCP() )					//! �����TCP Modbus,��ˢ�·��Ͳ���
			{
				FRAME_HEAD* pHead	= (FRAME_HEAD* )pPacketBuffer;
				WORD wdPacketNo	= MAKEWORD( pHead->byPckNoLow, pHead->byPckNoHigh );
				m_bySendFunc		= m_pSendData[wdPacketNo].bySendFunc;
				m_wdSendPoints		= m_pSendData[wdPacketNo].wdSendPoints;
				m_wdSendStartAddr	= m_pSendData[wdPacketNo].wdSendStartAddr;

				byUnitAddr = pPacketBuffer[sizeof(FRAME_HEAD)-1];
				IsExceptionResp = (pPacketBuffer[sizeof(FRAME_HEAD)]>0x80)?true:false;
			}
			else
			{
				byUnitAddr = pPacketBuffer[0];
				IsExceptionResp = (pPacketBuffer[1]>0x80)?true:false;
			}

			CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>(
				pDriver->GetDeviceByAddr( byUnitAddr ) );

			if ( !pDevice )
			{
				break;
			}

			//! ��·���յ��κ��豸��Ӧ������������·���߼�����
			pDriver->m_bIsLinkGetRightFrame = true;

			//! ��Ϊ����Ҫ�ط�
			m_nCurResend = m_nMaxResend;

			
			//! ����Ǵӻ����յ�����ͨ�����֡����Ӧ������ͨ����ϱ�־
			if( !pDevice->IsHot())
			{
				if ( !IsExceptionResp || pDevice->IsExceptionAsOnline() )
				{
					if ( !pDevice->IsOnLine() )
					{
						//if (pDriver->IsEnglishEnv())
						//{
						//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket The Slave Device:%s(Address:%d)is Online Now!", pDevice->GetName().c_str(), pDevice->GetAddr());
						//}
						//else
						//{
						//	snprintf( pchLog, sizeof(pchLog), "CFepModbusPrtcl::ProcessRcvdPacket �ӻ��豸%s(��ַ%d)�����ˣ�", pDevice->GetName().c_str(), pDevice->GetAddr());
						//}
						pDriver->WriteLogAnyWay(pchLog);
					}
					pDevice->SetOnLine( DEVICE_ONLINE );
					m_bRTUChannelTest = false;
					pDevice->SetSlaveTimeOuts( 0 );
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes=1;
				}
			}

			//! ���������������������ʣ������豸����
			if ( pDevice->IsHot() || IsNetConn() )
			{
				if ( !IsExceptionResp || pDevice->IsExceptionAsOnline() )
				{
					if ( !pDevice->IsOnLine() )
					{
						//if (pDriver->IsEnglishEnv())
						//{
						//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPracket Device:%s(Address:%d)is OnLine Now!",pDevice->GetName().c_str(), pDevice->GetAddr());
						//}
						//else
						//{
						//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPracket �豸%s(��ַ%d)�����ˣ�", pDevice->GetName().c_str(), pDevice->GetAddr() );
						//}
						//pDriver->WriteLogAnyWay(pchLog);
					}

					//! ����װ������
					pDevice->SetOnLine( DEVICE_ONLINE );
					pDevice->m_byTxTimes = 0;
					pDevice->m_byRxTimes=1;
					pDevice->SetTrySendCount( pDevice->GetMaxOffLine() );
				}
			}

			if ( pDevice->IsHot() )
			{
				int nRTULen;
				Byte* pDevAddr = NULL;
				if( !IsModbusTCP() )			//! ����RTU Modbus
				{
					//! ����յ���֡�����������֡����Ӧ���򲻴���
					if ( pPacketBuffer[0] != m_bySendAddr 
						|| ((pPacketBuffer[1]&0x7F) != m_bySendFunc ) )
					{
						//snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPracket the RTUFrame is Obsolete��Addr:%d Func:%d", m_bySendAddr, m_bySendFunc);
						pDriver->WriteLogAnyWay(pchLog);
						break;
					}
					pDevAddr = pPacketBuffer;
					nRTULen = nPacketLength;
				}
				else	//! �����TCP Modbus������Ҫ��֡ͷȥ����Ȼ���ٽ��д���
				{
					pDevAddr = pPacketBuffer+sizeof(FRAME_HEAD)-1;
					nRTULen = nPacketLength-sizeof(FRAME_HEAD)+1;
				}				

				//! ����豸����ģ��״̬���򲻴�����Ӧ֡
				if ( pDevice->IsSimulate() )
					break;

				Byte byTemp;
				int nRet = 0;
				//std::string szSndPacket = GetCommData( m_pbySndBuffer, m_nSndLen, COMMDATA_SND);
				//std::string szRcvPacket = GetCommData( pPacketBuffer, nPacketLength, COMMDATA_RCV);
				switch(pDevAddr[1])	//! ���ݹ����벻ͬ������
				{
				case READ_DIG_OUTPUT_STATUS:						//! ������Ϊ1
					{
						//! �����豸��DI��ֵ
						nRet = pDevice->UpdateDI( MACSMB_DIA, pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal );
						//! �б�λʱ���������
						if ( nRet > 0 )
						{
							std::string szSndPacket = GetCommData( m_pbySndBuffer, m_nSndLen, COMMDATA_SND);
							std::string szRcvPacket = GetCommData( pPacketBuffer, nPacketLength, COMMDATA_RCV);
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket There are %d DI changing Infos to the Request Frame:%s",nRet, szSndPacket.c_str());
							//}
							//else
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket ����%d����λ��Ϣ���Ա���:%s", nRet, szSndPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket The Response Frame:%s", szRcvPacket.c_str());
							//}
							//else
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket�յ�����:%s", szRcvPacket.c_str());
							//}
							pDriver->WriteLogAnyWay( pchLog );
						}
					}
					break;
				case READ_DIG_INPUT_STATUS:							//! ������Ϊ2
					{
						//! �����豸��DI��ֵ
						nRet = pDevice->UpdateDI( MACSMB_DIB, pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal );
						//! �б�λʱ���������
						if ( nRet > 0 )
						{
							std::string szSndPacket = GetCommData( m_pbySndBuffer, m_nSndLen, COMMDATA_SND);
							std::string szRcvPacket = GetCommData( pPacketBuffer, nPacketLength, COMMDATA_RCV);
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket There are %d DI changing Infos to the Request Frame:%s",nRet, szSndPacket.c_str());
							//}
							//else
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket ����%d����λ��Ϣ���Ա���,%s", nRet, szSndPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket The Response Frame:%s", szRcvPacket.c_str());
							//}
							//else
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket �յ�����:%s", szRcvPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
						}
					}
					break;
				case READ_ANA_INPUT_STATUS:							//! ������Ϊ4
					{
						//! �����豸���ͽ��е����⴦��
						nRet = pDevice->BeforeUpdateInputAI( pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal );
						//! �����������Ҵ�������-999������ֹ���ݴ���ͷ�������
						if ( nRet == -999 )
						{
							return;
						}
						nRet = pDevice->UpdateInputAI( pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal);
						//! �б�λʱ���������
						if ( nRet > 0 )
						{
							std::string szSndPacket = GetCommData( m_pbySndBuffer, m_nSndLen, COMMDATA_SND);
							std::string szRcvPacket = GetCommData( pPacketBuffer, nPacketLength, COMMDATA_RCV);
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket There are %d DI changing Infos to the Request Frame:%s",nRet, szSndPacket.c_str());
							//}
							//else
							//{
							//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket ����%d����λ��Ϣ���Ա���,%s", nRet, szSndPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket The Response Frame:%s", szRcvPacket.c_str());							}
							//else
							//{
							//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket �յ�����:%s", szRcvPacket.c_str() );
							//}
							pDriver->WriteLogAnyWay( pchLog );
						}
					}
					break;
				case READ_ANA_OUTPUT_STATUS:						//! ������Ϊ3
					{
						//! �����豸���ͽ��е����⴦��
						nRet = pDevice->BeforeUpdateHoldingAI( pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal );
						//! �����������Ҵ�������-999������ֹ���ݴ���ͷ�������
						if ( nRet == -999 )
						{
							return;
						}

						//! ����AI��
						nRet = pDevice->UpdateHoldingAI( pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal);
						//! �б�λʱ���������
						if ( nRet > 0 )
						{
							std::string szSndPacket = GetCommData( m_pbySndBuffer, m_nSndLen, COMMDATA_SND);
							std::string szRcvPacket = GetCommData( pPacketBuffer, nPacketLength, COMMDATA_RCV);
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket There are %d DI changing Infos to the Request Frame:%s",nRet, szSndPacket.c_str());							
							//}
							//else
							//{
							//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket ����%d����λ��Ϣ���Ա���,%s", nRet, szSndPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
							//if (pDriver->IsEnglishEnv())
							//{
							//	snprintf(pchLog,MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket The Response Frame:%s", szRcvPacket.c_str());						
							//}
							//else
							//{
							//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::ProcessRcvdPacket �յ�����:%s", szRcvPacket.c_str() );
							//}
							//pDriver->WriteLogAnyWay( pchLog );
						}

					}
					break;
				case FORCE_DIG_SINGLE_OUTPUT_STATUS:				//! ������Ϊ5
				case FORCE_ANA_SINGLE_OUTPUT_STATUS:				//! ������Ϊ6
				case FORCE_DIG_MULTIPLE_OUTPUT_STATUS:				//! ������Ϊ15
				case FORCE_ANA_MULTIPLE_OUTPUT_STATUS:				//! ������Ϊ16
					{
						if ( nPacketLength < 8 ||
							MAKEWORD( pDevAddr[3],pDevAddr[2] ) != m_wdSendStartAddr 
							|| MAKEWORD( pDevAddr[5],pDevAddr[4] ) != m_wdSendPoints )
						{
							break;
						}

						//! �÷��ŵ�ֵΪ1
						CIOCfgUnitType* pUnitType = pDevice->GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIC);
						if ( pUnitType && pUnitType->GetTagCount() > 0 )
						{
							//�÷��ŵ�Ϊ1
							byTemp = 1;
							pDevice->SetVal( MACSMB_DIC, 0, &byTemp, 1, 0, bFromLocal );
						}
					}
					break;
				case READ_GENERAL_REFERENCE:						//! ������Ϊ20
					{
						pDevice->UpdateFileInfo(pDevAddr+3, m_wdSendStartAddr, m_wdSendPoints, bFromLocal);
					}
					break;
				case WRITE_GENERAL_REFERENCE:						//! ������Ϊ21
					{
						//! ���д�ļ�����֡����
						//�÷��ŵ�ֵΪ0
						CIOCfgUnitType* pUnitType = pDevice->GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIC);
						if ( pUnitType && pUnitType->GetTagCount() > 0 )
						{
							byTemp = 1;
							pDevice->SetVal( MACSMB_DIC, 0, &byTemp, 1, 0, bFromLocal );
						}
					}
					break;
					//! �쳣֡����
				case READ_DIG_OUTPUT_STATUS + 128:					//! ������Ϊ1
				case READ_DIG_INPUT_STATUS + 128:					//! ������Ϊ2
				case READ_ANA_INPUT_STATUS + 128:					//! ������Ϊ4
				case READ_ANA_OUTPUT_STATUS + 128:					//! ������Ϊ3
					{
						//! �������֡�쳣����
					}
					break;
				case FORCE_DIG_SINGLE_OUTPUT_STATUS + 128:			//! ������Ϊ5
				case FORCE_ANA_SINGLE_OUTPUT_STATUS + 128:			//! ������Ϊ6
				case FORCE_DIG_MULTIPLE_OUTPUT_STATUS + 128:		//! ������Ϊ15
				case FORCE_ANA_MULTIPLE_OUTPUT_STATUS + 128:		//! ������Ϊ16
				case WRITE_GENERAL_REFERENCE + 128:					//! ������Ϊ21
					{
						//�÷��ŵ�ֵΪ0
						CIOCfgUnitType* pUnitType = pDevice->GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIC);
						if ( pUnitType && pUnitType->GetTagCount() > 0 )
						{
							byTemp = 0;
							pDevice->SetVal( MACSMB_DIC, 0, &byTemp, 1, 0, bFromLocal );
						}
					}
					break;
				default:
					//! ת������֡����������
					ProcessRcvdSplPacket(pDevAddr, nRTULen, bFromLocal);
					break;
				}
			}
		}

	}

	/*!
	*	\brief	������������ݰ����Ѿ�ת��ΪRTU��ʽ�����ݰ���
	*
	*	\param	pPacketBuffer	���ݰ�
	*	\param	nPacketLength	���ݰ�����
	*	\param	bFromLocal		�����Ǳ������Ļ����������
	*
	*	\retval	void
	*/		
	void CFepModbusPrtcl::ProcessRcvdSplPacket(	Byte* pPacketBuffer, int nPacketLength, bool bFromLocal )
	{
		return;
	}

	/*!
	*	\brief	����16λ����У���--CRC
	*
	*	1��	��һ16λ�Ĵ���λȫ1��
	*	2��	���������ݵĸ�λ�ֽ����Ĵ����ĵͰ�λ������Ĵ�����
	*	3��	���ƼĴ��������λ��0���Ƴ��ĵͰ�λ�����־λ��
	*	4��	���־λ��1������A001���Ĵ��������־λ��0����������3��
	*	5��	�ظ�����3��4��ֱ����λ��λ��
	*	6��	�����һλ�ֽ���Ĵ�����
	*	7��	�ظ�����3��5��ֱ�����б������ݾ���Ĵ��������λ8�Σ�
	*	8��	��ʱ�Ĵ����м�ΪCRCУ���룬���λ�ȷ��ͣ�
	*
	*	\param	datapt		������ʼλ��
	*	\param	bytecount	�ֽڸ���
	*
	*	\retval	WORD	16λ������У���
	*/
	WORD CFepModbusPrtcl::CRC(const Byte * datapt,int bytecount)
	{
		if (!datapt)
			return 0;
		WORD Reg16=0xFFFF,mval=0xA001;  //0xA001=1010000000000001
		int   i;
		Byte  j,flg; 

		for(i=0;i<bytecount;i++)
		{
			Reg16^=*(datapt+i);
			for(j=0;j<8;j++)
			{
				flg=0;
				flg=Reg16&0x0001;  //b0
				Reg16>>=1;         //����һλ����b15=0
				if(flg==1)
					Reg16=Reg16^mval;
			}
		}
		return Reg16;
	}

	/*!
	*	\brief	�����豸��ַ��ȡ�豸������
	*
	*	\param	Addr	�豸��ַ
	*	\param	DevNo	�豸��ַ��Ӧ���豸������
	*
	*	\retval	bool	
	*/
	bool CFepModbusPrtcl::GetDeviceNoByAddr( int Addr, int& DevNo )
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		CIODeviceBase* pDevice = NULL;
		for( int i=0; i< pDriver->GetDeviceCount(); i++ )
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

	/*!
	*	\brief	��ң��ң��֡
	*
	*	\param	pbyData		֡�׵�ַ
	*	\param	nDataLen	֡��
	*	\param	nTimeOut	��ʱʱ��
	*	\param	fCmdVal		���͵�����ֵ
	*
	*	\retval	int	0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusPrtcl::BuildCtrlFrame( 
		Byte* pbyData, int& nDataLen, int& nTimeOut, float& fCmdVal )
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		nDataLen = 0;

		int nResult = 0;
		Byte pchBuf[1124] = {0};
		int nBufLen = 1124;		
		nResult = pDriver->m_IORemoteCtrl.DeQueue( (char*)pchBuf, nBufLen );
		if ( nResult==0 )
		{
			memcpy( (Byte*)(&m_tCtrlRemoteCmd), pchBuf, sizeof(m_tCtrlRemoteCmd) );

			//ȡ���豸��ַ
			CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>(pDriver->GetDeviceByName(m_tCtrlRemoteCmd.pchUnitName));

			if(!pDevice)
			{
				return -4;
			}

			//! ������ʱ
			ACE_Time_Value tv( 0, (pDevice->GetPortCfg().nSndDelay)*1000 );
			ACE_OS::sleep( tv );
			
			//! ����豸��ַ���ô����򲻷��Ϳ���֡
			if( pDevice->GetAddr() <= 0 || pDevice->GetAddr() > 255 )
			{
				if (pDriver->IsLog())
				{
					char pchLog[MAXDEBUGARRAYLEN];
					//if (pDriver->IsEnglishEnv())
					//{
					//	snprintf(pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildCtrlFrame Dev:%s the address:%d is not right!", m_tCtrlRemoteCmd.pchUnitName, pDevice->GetAddr());
					//}
					//else
					//{
					//	snprintf( pchLog, sizeof(pchLog), "CFepModbusPrtcl::BuildCtrlFrame �豸:%s ��ַ:%d ���ô���!", m_tCtrlRemoteCmd.pchUnitName, pDevice->GetAddr());
					//}
					//pDriver->WriteLog( pchLog );
					return -5;
				}
			}

			//! ����豸���ߣ��򲻷��Ϳ���֡
			if ( pDevice->IsOffLine() )
			{
				return 0;
			}

			//����UnitType��ͬ�鷢��֡,�ó���
			switch( m_tCtrlRemoteCmd.byUnitType )
			{
			case MACSMB_DOA:	// DO 0X
			case MACSMB_DOB:	// AO 4X
			case MACSMB_DOC:
			case MACSMB_DOD:
			case MACSMB_DOF:
			case MACSMB_DOG:
				{
					Byte byVal;
					int nLen = __min( m_tCtrlRemoteCmd.nDataLen, 1 );
					memcpy( &byVal, m_tCtrlRemoteCmd.byData, nLen );

					//! DO 0X ������5
					if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOA )
						nDataLen = pDevice->BuildDOAReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);
					//! AO 4X ������6
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOB )
						nDataLen = pDevice->BuildDOBReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);
					//! DO 0X ������15
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOC )
						nDataLen = pDevice->BuildDOCReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);
					//! AO 4X ������16
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOD )
						nDataLen = pDevice->BuildDODReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);
					//! AO 4X ������6
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOF )
						nDataLen = pDevice->BuildDOFReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);
					//! AO 4X ������16
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_DOG )
						nDataLen = pDevice->BuildDOGReqFrame(
						pbyData,m_tCtrlRemoteCmd.lTagAddress,byVal);

					fCmdVal = (float)byVal;
					m_bycmdType = 1;

					//! ����ң�ذ���֡����
					BuildCtrlFwReq(pDevice,m_tCtrlRemoteCmd.lTagAddress);
				}
				break;			
			case MACSMB_DOE:	
				break;
			case MACSMB_AOA:	//! AO 4X ������6
			case MACSMB_AOB:	//! AO 4X ������16
			case MACSMB_AOD:	//! AO 4X ������6�������Ĵ�����Nλ
			case MACSMB_AOE:	//! AO 4X ������16�������Ĵ�����Nλ
			case MACSMB_AOF:	//! AO 4X ������6�������Ĵ�����Nλ������λ�Զ�Ϊ0
			case MACSMB_AOG:	//! AO 4X ������16�������Ĵ�����Nλ������λ�Զ�Ϊ0
				{
					float fVal = 0;
					int nLen = __min( m_tCtrlRemoteCmd.nDataLen, sizeof(double) );
					if (nLen > sizeof(float))
					{
						if (sizeof(double) == nLen)
						{
							double dVal = (*(double*)m_tCtrlRemoteCmd.byData);
							fVal = (float)dVal;
						}
					}
					else
					{
						memcpy( &fVal, m_tCtrlRemoteCmd.byData, nLen );
					}

					//! AO 4X ������6
					if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOA )
						nDataLen = pDevice->BuildAOAReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );
					//! AO 4X ������16
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOB )
						nDataLen = pDevice->BuildAOBReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );
					//! AO 4X ������6�������Ĵ�����Nλ
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOD )
						nDataLen = pDevice->BuildAODReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );
					//! AO 4X ������16�������Ĵ�����Nλ
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOE )
						nDataLen = pDevice->BuildAOEReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );
					//! AO 4X ������16�������Ĵ�����Nλ������λ�Զ�Ϊ0
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOF )
						nDataLen = pDevice->BuildAOFReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );
					//! AO 4X ������16�������Ĵ�����Nλ������λ�Զ�Ϊ0
					else if( m_tCtrlRemoteCmd.byUnitType == MACSMB_AOG )
						nDataLen = pDevice->BuildAOGReqFrame( 
						pbyData,m_tCtrlRemoteCmd.lTagAddress, fVal );

					fCmdVal = fVal;
					m_bycmdType = 2;

					//! ���ɷ�������
					BuildCtrlFwReq( pDevice, m_tCtrlRemoteCmd.lTagAddress );
				}
				break;
			case MACSMB_AOC:		//!< �ַ�������ң��
			case MACSMB_MOA:		//!< �������·�����
				{
					nDataLen = pDevice->BuildAOCReqFrame(
						pbyData, m_tCtrlRemoteCmd.lTagAddress,
						m_tCtrlRemoteCmd.byData, m_tCtrlRemoteCmd.nDataLen );
					fCmdVal = 0;
					m_bycmdType = 2;
				}
				break;
			case MACSMB_AOT:
				{
					if ( m_tCtrlRemoteCmd.nDataLen < sizeof(float) )
					{
						return 0;
					}
					Byte pbyTemp[4];
					pbyTemp[0] = m_tCtrlRemoteCmd.byData[3];
					pbyTemp[1] = m_tCtrlRemoteCmd.byData[2];
					pbyTemp[2] = m_tCtrlRemoteCmd.byData[1];
					pbyTemp[3] = m_tCtrlRemoteCmd.byData[0];
					nDataLen = pDevice->BuildCtrlMultiRegsFrame(
						pbyData, m_tCtrlRemoteCmd.lTagAddress - 1,
						sizeof(float)/2, pbyTemp  );
					fCmdVal = 0;
					m_bycmdType = 2;
				}
				break;
			case MACSMB_AOP:
				{
					if ( m_tCtrlRemoteCmd.nDataLen < sizeof(DWORD) )
					{
						return 0;
					}
					Byte pbyTemp[4];
					pbyTemp[0] = m_tCtrlRemoteCmd.byData[3];
					pbyTemp[1] = m_tCtrlRemoteCmd.byData[2];
					pbyTemp[2] = m_tCtrlRemoteCmd.byData[1];
					pbyTemp[3] = m_tCtrlRemoteCmd.byData[0];
					nDataLen = pDevice->BuildCtrlMultiRegsFrame(
						pbyData, m_tCtrlRemoteCmd.lTagAddress - 1,
						sizeof(DWORD)/2, pbyTemp  );
					fCmdVal = 0;
					m_bycmdType = 2;
				}
				break;
			case MACSMB_DOM:
				{
					Byte byVal;
					int nLen = __min( m_tCtrlRemoteCmd.nDataLen, 1 );
					memcpy( &byVal, m_tCtrlRemoteCmd.byData, nLen );
					nDataLen = pDevice->BuildDOMReqFrame(
						pbyData, m_tCtrlRemoteCmd.lTagAddress,
						byVal, m_tCtrlRemoteCmd.nDataLen );
					fCmdVal = 0;
					m_bycmdType = 2;
				}
				break;
			case MACSMB_READSETTINGGROUP:
				{					
					Byte byVal;
					int nLen = __min( m_tCtrlRemoteCmd.nDataLen, 1 );
					memcpy( &byVal, m_tCtrlRemoteCmd.byData, nLen );

					char chTagName[50]="";
					sprintf(chTagName,"RDSETTINGGP%d",m_tCtrlRemoteCmd.lTagAddress);
					int iTemp=0;					
					for ( int i=0;i< pDevice->GetSETTINGGROUP();i++)
					{
						std::string strTemp = pDevice->vecSettingGroup[i].strDoTagName;
						iTemp = stricmp(chTagName,strTemp.c_str());
						if( iTemp == 0 )
						{
							if (byVal == 0)
							{
								pDevice->SetVal(MACSMB_READSETTINGGROUP,i,&byVal,1,0,true);
								pDevice->vecSettingGroup[i].iReadCount = 0;
							}
							else if (byVal == 1)
							{
								pDevice->vecSettingGroup[i].iReadCount = pDevice->vecSettingGroup[i].iReadCountCfg;
								pDevice->SetVal(MACSMB_READSETTINGGROUP,i+1,&byVal,1,0,true);
							}
							break;
						}						
					}
				}
				break;
			case MACSMB_READSETTINGGROUPALL:
				//!���еĴ�����ȡ��ֵ��ң�ص���λ�������ö�ȡ����
				{
					Byte byVal;
					int nLen = __min( m_tCtrlRemoteCmd.nDataLen, 1 );
					memcpy( &byVal, m_tCtrlRemoteCmd.byData, nLen );

					if ( byVal == 1 )
					{
						int ia=0;
						for (int i=0;i<pDevice->vecSettingGroup.size();i++)
						{
							ia = pDevice->vecSettingGroup[i].iReadCountCfg;	
							pDevice->vecSettingGroup[i].iReadCount = ia; 
							pDevice->SetVal(MACSMB_READSETTINGGROUP,i+1,&byVal,1,0,true);
						}					
					}
					else
					{
						for (int i=0;i<pDevice->vecSettingGroup.size();i++)
						{
							pDevice->vecSettingGroup[i].iReadCount = 0;	
							pDevice->SetVal(MACSMB_READSETTINGGROUP,i+1,&byVal,1,0,true);
						}
					}
					pDevice->SetVal(MACSMB_READSETTINGGROUPALL,1,&byVal,1,0,true);
				}
				break;
			default:
				nDataLen = pDevice->BuildSplCtrlFrame(pbyData,m_tCtrlRemoteCmd,fCmdVal);
				break;
			}


			if ( nDataLen > 0 )
			{
				//�÷��ŵ�ֵΪ��ʼ̬��
				CIOCfgUnitType* pUnitType = pDevice->GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIC);
				if ( pUnitType && pUnitType->GetTagCount() > 0 )
				{
					Byte byTemp = 0;
					pDevice->SetVal( MACSMB_DIC, 0, &byTemp, 1, 0, true );	//�÷��ŵ�Ϊ0
				}


				//! ǿ��д��־
				if ( nDataLen > 0 )
				{
					std::string szData = GetCommData( pbyData, nDataLen, COMMDATA_SND);
					char pchLog[MAXDEBUGARRAYLEN];
					//if (pDriver->IsEnglishEnv())
					//{
					//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::BuildCtrlFrame Device:%s,UnitType=%d,Address=%lld,Frame:%s", 
					//		pDevice->GetName().c_str(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, szData.c_str());
					//}
					//else
					//{
					//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::BuildCtrlFrame ����Ʊ���,�豸����=%s,UnitType=%d,��ǩ��ַ=%lld,Content: %s",
					//		pDevice->GetName().c_str(), m_tCtrlRemoteCmd.byUnitType, m_tCtrlRemoteCmd.lTagAddress, szData.c_str());
					//}
					//pDriver->WriteLogAnyWay( pchLog );
				}

				//! �ó�ʱʱ��
				nTimeOut = pDevice->GetPortCfg().nTimeOut;
			}			
		}

		return 0;
	}

	/*!
	*	\brief	����ң�ط��Ű�
	*
	*	�����pFrame��ʼ�����ݻ��������Ƿ���������ң�ط��Ű�
	*
	*	\param	pFrame		֡ͷָ��
	*	\param	nCorrectLen	���н�������ȷ��ң�ط��Ű������øò���Ϊ��ȷ֡�ĳ���
	*
	*	\retval	int			�������£�
	*				FLAG_NONE				0		����ң�ط���֡
	*				FLAG_UNCOMPLETE			1		��ң�ط���֡����û��������
	*				FLAG_CORRECTFRAME		2		����ȷ��ң�ط���֡
	*				FLAG_ERROR				3		��������˵������֡ͷλ�ò���
	*/
	int CFepModbusPrtcl::ExtractCtrlPacket( const Byte* pFrame, int &nCorrectLen )
	{
		int nFlag = FLAG_NONE;

		//! �����ң�ء�ң������֡
		if(	   ( m_byActiveFunc&0x7F ) == FORCE_DIG_SINGLE_OUTPUT_STATUS 
			|| ( m_byActiveFunc&0x7F ) == FORCE_ANA_SINGLE_OUTPUT_STATUS
			|| ( m_byActiveFunc&0x7F ) == FORCE_DIG_MULTIPLE_OUTPUT_STATUS
			|| ( m_byActiveFunc&0x7F ) == FORCE_ANA_MULTIPLE_OUTPUT_STATUS
			)
		{
			if( IsModbusTCP() )			//! TCP Modbus
			{
				FRAME_HEAD* pHead = (FRAME_HEAD*)(pFrame);

				//! ��֤֡ͷ�м�¼�ĳ���
				WORD wdDataLen = MAKEWORD(pHead->byDataLenLow, pHead->byDataLenHigh);
				if (( m_byActiveFunc&0x80 ) == 0x80 && wdDataLen != 3)
				{
					return FLAG_ERROR;				//! У�����
				}
				if (( m_byActiveFunc&0x80 ) == 0x00 && wdDataLen != 6)
				{
					return FLAG_ERROR;				//! У�����
				}

				//! ��ȷ��֡��
				nCorrectLen = wdDataLen + sizeof(FRAME_HEAD) - 1;

				//! û��������
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}


				//! У���豸��ַ
				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));
				if ( !pDevice )
				{
					return FLAG_ERROR;				//! У�����
				}
			}
			else						//! RTU Modbus
			{
				if ( ( m_byActiveFunc&0x80 ) == 0x80 )
				{
					nCorrectLen = 5;
				}
				else
				{
					nCorrectLen = 8;
				}				

				//! û��������
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}

				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));
				if ( !pDevice )
				{
					return FLAG_ERROR;				//! У�����
				}

				//! У��CRC
				WORD CheckSum = CRC( pFrame, nCorrectLen - 2 );
				if ( (( pDevice->GetPortCfg().nCRCOrder == CRC_HOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != HIBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != LOBYTE(CheckSum) ) )
					|| (( pDevice->GetPortCfg().nCRCOrder == CRC_LOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != LOBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != HIBYTE(CheckSum) ) ) )
				{
					return FLAG_ERROR;				//! У�����
				}
			}

			return FLAG_CORRECTFRAME;			//! ���յ�������ң�ط���֡
		}

		return nFlag;
	}

	/*!
	*	\brief	������ѯ֡���ذ�
	*
	*	�����pFrame��ʼ�����ݻ��������Ƿ�����������ѯ֡���ذ�
	*
	*	\param	pFrame		֡ͷָ��
	*	\param	nCorrectLen	���н�������ȷ��ң�ط��Ű������øò���Ϊ��ȷ֡�ĳ���
	*
	*	\retval	int			�������£�
	*				FLAG_NONE				0		������ѯ֡���ذ�
	*				FLAG_UNCOMPLETE			1		����ѯ֡���ذ�����û��������
	*				FLAG_CORRECTFRAME		2		����ȷ����ѯ֡���ذ�
	*				FLAG_ERROR				3		��������˵������֡ͷλ�ò���
	*/
	int CFepModbusPrtcl::ExtractMsgPacket( const Byte* pFrame, int &nCorrectLen )
	{
		int nFlag = FLAG_NONE;

		//! �������ѯ֡���ذ�
		if(	   ( m_byActiveFunc&0x7F ) ==  READ_DIG_OUTPUT_STATUS
			|| ( m_byActiveFunc&0x7F ) == READ_DIG_INPUT_STATUS
			|| ( m_byActiveFunc&0x7F ) == READ_ANA_OUTPUT_STATUS
			|| ( m_byActiveFunc&0x7F ) == READ_ANA_INPUT_STATUS
			)
		{
			if( IsModbusTCP() )			//! TCP Modbus
			{
				WORD wdValiteDataLen;  //! ����
				FRAME_HEAD* pHead = (FRAME_HEAD*)(pFrame);

				//! ��֤֡ͷ�м�¼�����ݳ���
				WORD wdDataLen = MAKEWORD( pHead->byDataLenLow, pHead->byDataLenHigh );
				WORD wdPacketNo = MAKEWORD( pHead->byPckNoLow, pHead->byPckNoHigh );
				if (( m_byActiveFunc&0x80 ) == 0x80 && wdDataLen != 3)
				{
					return FLAG_ERROR;				//! У�����
				}
				if (( m_byActiveFunc&0x80 ) == 0x00 )
				{
					//! �������ݰ���					
					if(	m_byActiveFunc ==  READ_DIG_OUTPUT_STATUS
						|| m_byActiveFunc == READ_DIG_INPUT_STATUS )
					{			
						wdValiteDataLen = (m_pSendData[wdPacketNo].wdSendPoints + 7)/8 + 3;
					}
					else
					{
						wdValiteDataLen = m_pSendData[wdPacketNo].wdSendPoints * 2 + 3;
					}

					if( wdDataLen != wdValiteDataLen)
						return FLAG_ERROR;				//! У�����
				}

				//! ��ȷ��֡��
				nCorrectLen = wdDataLen + sizeof(FRAME_HEAD) - 1;

				//! û��������
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}

				////! У���ֽڸ���
				//if (( m_byActiveFunc&0x80 ) == 0x00 && pFrame[sizeof(FRAME_HEAD)+1] != 0)
				//{
				//	if ( pFrame[sizeof(FRAME_HEAD)+1] != wdValiteDataLen - 3 )
				//	{
				//		return FLAG_ERROR;				//! У�����
				//	}
				//}	

				//! У���豸��ַ
				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));

				if ( !pDevice )
				{
					return FLAG_ERROR;				//! У�����
				}
			}
			else						//! RTU Modbus
			{
				if ( ( m_byActiveFunc&0x80 ) == 0x80 )
				{
					nCorrectLen = 5;
				}
				else
				{
					//! У���ֽڸ������˴�Ҫ��У���ֽڸ������ֽڸ���������ֱ�Ӷ���������֡�����ҳ���ֵ�ܴ�ʱһֱ�ڵȴ�����
					if(	  (m_byActiveFunc ==  READ_DIG_OUTPUT_STATUS
						|| m_byActiveFunc == READ_DIG_INPUT_STATUS)
						&& pFrame[sizeof(FRAME_HEAD)+1] != 0 )
					{				
						if ( pFrame[2] != ( m_wdSendPoints + 7)/8 )
						{
							return FLAG_ERROR;				//! У�����
						}
					}
					else if( m_byActiveFunc == READ_ANA_OUTPUT_STATUS
						|| m_byActiveFunc == READ_ANA_INPUT_STATUS)
					{
						if ( pFrame[2] != m_wdSendPoints * 2 )
						{
							return FLAG_ERROR;				//! У�����
						}
					}
					nCorrectLen = 3 + pFrame[2] + 2;
				}

				//! û��������
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}

				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));

				if ( !pDevice )
				{
					return FLAG_ERROR;				//! У�����
				}				

				//! У��CRC
				WORD CheckSum = CRC( pFrame, nCorrectLen - 2 );
				if ( ( ( pDevice->GetPortCfg().nCRCOrder == CRC_HOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != HIBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != LOBYTE(CheckSum) ) )
					|| (( pDevice->GetPortCfg().nCRCOrder == CRC_LOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != LOBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != HIBYTE(CheckSum) ) )
					)
				{
					return FLAG_ERROR;				//! У�����
				}
			}

			return FLAG_CORRECTFRAME;	//! ���յ���������ȷ����ѯ֡���ذ�
		}

		return nFlag;
	}

	/*!
	*	\brief	�����ļ��������Ű�
	*
	*	�����pFrame��ʼ�����ݻ��������Ƿ����������ļ��������Ű�
	*
	*	\param	pFrame		֡ͷָ��
	*	\param	nCorrectLen	���н�������ȷ��ң�ط��Ű���
	*						���øò���Ϊ��ȷ֡�ĳ���
	*
	*	\retval	int			�������£�
	*		FLAG_NONE				0		����ң�ط���֡
	*		FLAG_UNCOMPLETE			1		��ң�ط���֡����û��������
	*		FLAG_CORRECTFRAME		2		����ȷ��ң�ط���֡
	*		FLAG_ERROR				3		��������˵������֡ͷλ�ò���
	*/
	int CFepModbusPrtcl::ExtractFilePacket( const Byte* pFrame, int &nCorrectLen )
	{
		int nFlag = FLAG_NONE;
		//! �������ѯ֡���ذ�
		if(	   ( m_byActiveFunc&0x7F ) ==  READ_GENERAL_REFERENCE
			|| ( m_byActiveFunc&0x7F ) == WRITE_GENERAL_REFERENCE
			)
		{
			if( IsModbusTCP() )			//! TCP Modbus
			{				
				WORD wdValiteDataLen;  //! ����
				FRAME_HEAD* pHead = (FRAME_HEAD*)(pFrame);

				//! ��֤֡ͷ�м�¼�����ݳ���
				WORD wdDataLen = MAKEWORD( pHead->byDataLenLow, pHead->byDataLenHigh );
				WORD wdPacketNo = MAKEWORD( pHead->byPckNoLow, pHead->byPckNoHigh );
				if (( m_byActiveFunc&0x80 ) == 0x80 && wdDataLen != 3)
				{
					return FLAG_ERROR;				//! У�����
				}
				if (( m_byActiveFunc&0x80 ) == 0x00 )
				{
					//! �������ݰ���					
					if(	m_byActiveFunc == READ_GENERAL_REFERENCE )
					{			
						wdValiteDataLen = m_pSendData[wdPacketNo].wdSendPoints * 2 + 5;
					}
					else
					{
						wdValiteDataLen = m_pSendData[wdPacketNo].wdSendPoints * 2 + 10;
					}

					if( wdDataLen != wdValiteDataLen)
						return FLAG_ERROR;				//! У�����
				}

				//! ��ȷ��֡��
				nCorrectLen = wdDataLen + sizeof(FRAME_HEAD) - 1;

				//! û��������
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}

				if (( m_byActiveFunc&0x80 ) == 0x00 )
				{
					//! У���ֽڸ���
					if ( pFrame[sizeof(FRAME_HEAD)+1] != wdValiteDataLen - 3 )
					{
						return FLAG_ERROR;				//! У�����
					}
				}

				if(	m_byActiveFunc == READ_GENERAL_REFERENCE )
				{
					//! У���ļ���Ӧ���ȣ������ļ���¼��
					if ( pFrame[sizeof(FRAME_HEAD)+2] != wdValiteDataLen - 4 )
					{
						return FLAG_ERROR;				//! У�����
					}

					//! У��ο�����
					if ( pFrame[sizeof(FRAME_HEAD)+3] != 6 )
					{
						return FLAG_ERROR;				//! У�����
					}
				}
				else if( m_byActiveFunc == WRITE_GENERAL_REFERENCE )
				{
					//! У��ο�����
					if ( pFrame[sizeof(FRAME_HEAD)+2] != 6 )
					{
						return FLAG_ERROR;				//! У�����
					}
				}

				//! У���豸��ַ
				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));

				if ( !pDevice )
				{
					return FLAG_ERROR;				//! У�����
				}				
			}
			else						//! RTU Modbus
			{
				if ( ( m_byActiveFunc&0x80 ) == 0x80 )
				{
					nCorrectLen = 5;
				}
				else
				{
					//! У���ֽڸ���
					if(	   m_byActiveFunc ==  READ_GENERAL_REFERENCE )
					{				
						if ( pFrame[2] != ( m_wdSendPoints * 2 + 2 ) )
						{
							return FLAG_ERROR;				//! У�����
						}

						//! У���ļ���Ӧ���ȣ������ļ���¼��
						if ( pFrame[3] != ( m_wdSendPoints * 2 + 1 ) )
						{
							return FLAG_ERROR;				//! У�����
						}

						//! У��ο�����
						if ( pFrame[4] != 6 )
						{
							return FLAG_ERROR;				//! У�����
						}
					}
					else if( m_byActiveFunc == WRITE_GENERAL_REFERENCE )
					{
						if ( pFrame[2] != ( m_wdSendPoints * 2 + 7 ) )
						{
							return FLAG_ERROR;				//! У�����
						}

						//! У��ο�����
						if ( pFrame[3] != 6 )
						{
							return FLAG_ERROR;				//! У�����
						}
					}

					nCorrectLen = 3 + pFrame[2] + 2;
				}

				//! û��������
				if ( m_nRevLength + m_pbyRevBuffer - pFrame < nCorrectLen ) 
				{
					return FLAG_UNCOMPLETE;
				}

				CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;

				CFepModbusDevice* pDevice = dynamic_cast<CFepModbusDevice*>
					( pDriver->GetDeviceByAddr( m_byActiveAddr ));

				if ( !pDevice )
				{
					return FLAG_ERROR;				//! У�����
				}				

				//! У��CRC
				WORD CheckSum = CRC( pFrame, nCorrectLen - 2 );
				if ( ( ( pDevice->GetPortCfg().nCRCOrder == CRC_HOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != HIBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != LOBYTE(CheckSum) ) )
					|| (( pDevice->GetPortCfg().nCRCOrder == CRC_LOB_FIRST) 
					&& ( pFrame[nCorrectLen-2] != LOBYTE(CheckSum) 
					&& pFrame[nCorrectLen-1] != HIBYTE(CheckSum) ) )
					)
				{
					return FLAG_ERROR;				//! У�����
				}
			}

			return FLAG_CORRECTFRAME;	//! ���յ���������ȷ����ѯ֡���ذ�
		}

		return nFlag;
	}

	/*!
	*	\brief	������������ķ��Ű�
	*
	*	�����pFrame��ʼ�����ݻ��������Ƿ�����������������ķ��Ű�
	*
	*	\param	pFrame		֡ͷָ��
	*	\param	nCorrectLen	���н�������ȷ��ң�ط��Ű���
	*						���øò���Ϊ��ȷ֡�ĳ���
	*
	*	\retval	int			�������£�
	*		FLAG_NONE				0		����ң�ط���֡
	*		FLAG_UNCOMPLETE			1		��ң�ط���֡����û��������
	*		FLAG_CORRECTFRAME		2		����ȷ��ң�ط���֡
	*		FLAG_ERROR				3		��������˵������֡ͷλ�ò���
	*/
	int CFepModbusPrtcl::ExtractOtherPacket( const Byte* pFrame, int &nCorrectLen )
	{
		return FLAG_NONE;
	}

	/*!
	*	\brief	��ң�ذ���֡
	*
	*	\param	pDevice		�豸
	*	\param	lTagAddr	ң�ر�ǩ��ַ
	*
	*	\retval	int	0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusPrtcl::BuildCtrlFwReq( CFepModbusDevice* pDevice, int lTagAddr )
	{
		if(!pDevice)
		{
			return -1;
		}

		//! ��ð���֡
		struCtrlFollowReq* pFwReq = pDevice->GetCtrlFwReqByTagAddr(lTagAddr);

		if(pFwReq)
		{
			m_nCtrlInsertTempLen = sizeof(pFwReq->Req);
			memcpy( m_pbyCtrlInsertBuf, &pFwReq->Req, m_nCtrlInsertTempLen );

			//m_pInsertDelayTimer->SetTimer( pFwReq->msDelay );
		}
		return 0;
	}

	/*!
	*	\brief	��������Ԫ��ʶ
	*
	*	\param	High	��ʶ��λ
	*	\param	Low		��ʶ��λ
	*
	*	\retval	int	Modbus TCP�����С
	*/
	void CFepModbusPrtcl::GetPacketNo( Byte& High, Byte& Low )
	{
		High = HIBYTE( m_wdPacketNo );
		Low  = LOBYTE( m_wdPacketNo );

		m_wdPacketNo = ( m_wdPacketNo + 1 ) % 65536;

		return;
	}

	/*!
	*	\brief	��Modbus RTU����ת����Modbus TCP����
	*
	*	\param	pRTU		RTU����
	*	\param	iRTULen		RTU�����С
	*	\param	pTCP		TCP����
	*
	*	\retval	int	Modbus TCP�����С
	*/
	int CFepModbusPrtcl::MBRTU2MBTCPRequest(Byte* pRTU,int nRTULen,Byte* pTCP)
	{
		if( pRTU == NULL || nRTULen <= 3 || nRTULen > 252 || pTCP == NULL )
		{
			return 0;
		}

		//! �õ�MB TCP����ͷ
		WORD wdDataLen = nRTULen - 2;
		FRAME_HEAD pHead;
		GetPacketNo( pHead.byPckNoHigh, pHead.byPckNoLow );
		pHead.byPrtclHigh	= 0x00;
		pHead.byPrtclLow	= 0x00;
		pHead.byDataLenHigh	= HIBYTE(wdDataLen);
		pHead.byDataLenLow	= LOBYTE(wdDataLen);
		pHead.bySlaveAddress	= pRTU[0];

		//! ����MB TCP���İ���
		Byte* pBody = pTCP + sizeof(FRAME_HEAD);
		memmove( pBody, pRTU+1, nRTULen-3 );

		//! ����MB TCP����ͷ
		memcpy( pTCP, &pHead, sizeof(FRAME_HEAD) );

		return nRTULen - 3 + sizeof(FRAME_HEAD);
	}

	//! ���ӳɹ�
	int CFepModbusPrtcl::OnConnSuccess(std::string pPeerIP )
	{
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		//! ������
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard( pDriver->m_Mutex );

		//! �ر����ӳ�ʱ��ʱ��
		//pDriver->GetConnTimer()->KillTimer();
		pDriver->SetConnTime( 0 );
		pDriver->SetConnected( true );

		pDriver->m_nLinkOfflineConnectCount = 0;

		char pchLog[MAXDEBUGARRAYLEN] = {0};
		//if (pDriver->IsEnglishEnv())
		//{
		//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::OnConnSuccess IP:%s connect successful!", pPeerIP.c_str());
		//}
		//else
		//{
		//	snprintf( pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::OnConnSuccess IP:%s ���ӳɹ�!", pPeerIP.c_str());
		//}
		//! ��¼��־
		pDriver->WriteLogAnyWay( pchLog );

		//! ������ջ������ͻ��������е�����
		m_nRevLength = 0;
		m_iInnerLen = 0;

		CFepModbusDevice* pDevice = NULL;
		bool bBroadcastFlag = false;		//!�Ƿ��ǹ㲥Уʱ(����855Э���豸) Add by: Wangyanchao
		for ( int i = 0; i < pDriver->GetDeviceCount(); i++ )
		{
			pDevice = dynamic_cast<CFepModbusDevice*>( pDriver->GetDeviceByIndex(i) );
			if ( pDevice )
			{
				//���ӳɺ󣬸����շ������������������ɹ����ٴ�������ӡ�ע�⣺�˴��������豸���ߡ�
				pDevice->m_byTxTimes = 0;
				pDevice->m_byRxTimes = 0;

				pDevice->SetTrySendCount( 0 );
				if ( pDevice->HasCheckTime() && (pDevice->GetDevType() == DEVTYPE_855) )  //! 855�豸����Уʱ Add by: Wangyanchao
				{
					//! ʹ�ù㲥֡Уʱ����ͬ���͵������豸��ÿ��ֻ�跢��һ֡������Ҫ��ÿ���豸����

					if ( pDevice->IsBroadChckTime() )	
					{
						if ( !bBroadcastFlag )
						{
							pDevice->SetNeedCheckTime(true);	//��Уʱ��־
							bBroadcastFlag = true;
						}
					}
					else	//��Ե��ʱ��ÿ���豸���ñ�־
					{
						pDevice->SetNeedCheckTime(true);	//��Уʱ��־
					}
				}
			}
		}
		//! ����Ź���
		m_wdPacketNo = 0;
		m_nCtrlSendBufferLen = 0;

		/////////////////////////////////////////////////////////
		//! ��������
		int nDataLen = 0;
		BuildRequest( m_pbySndBuffer, nDataLen );
		/////////////////////////////////////////////////////////
		return 0;
	}	

	/*!
	*	\brief	��¼�ϴη���֡�е���Ҫ����
	*
	*	\retval	void
	*/
	void CFepModbusPrtcl::BackupLastRequest()
	{
		if ( m_pbySndBuffer[1] == READ_GENERAL_REFERENCE
			|| m_pbySndBuffer[1] == WRITE_GENERAL_REFERENCE )		//! �ļ���������
		{		
			m_bySendAddr		= m_pbySndBuffer[0];
			m_bySendFunc		= m_pbySndBuffer[1];
			m_wdSendStartAddr	= MAKEWORD( m_pbySndBuffer[7], m_pbySndBuffer[6] );
			m_wdSendPoints		= MAKEWORD( m_pbySndBuffer[9], m_pbySndBuffer[8] );
		}
		else
		{
			m_bySendAddr = m_pbySndBuffer[0];
			m_bySendFunc = m_pbySndBuffer[1];
			m_wdSendStartAddr = MAKEWORD( m_pbySndBuffer[3], m_pbySndBuffer[2] );
			m_wdSendPoints = MAKEWORD( m_pbySndBuffer[5], m_pbySndBuffer[4] );
		}
	}

	std::string CFepModbusPrtcl::GetCommData( Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd )
	{
		std::string szData = "";
		if (COMMDATA_RCV == byRcvOrSnd)
		{
			szData = "RCV: ";
		}
		else if(COMMDATA_SND == byRcvOrSnd)
		{
			szData = "SND: ";
		}

		char pchByte[4];
		//! ��ȡ������ʾ��ʽ
		for ( int i=0; i < nDataLen; i++ )
		{
			sprintf( pchByte, "%02X ", pbyBuf[i] );
			szData += pchByte;
		}

		return szData;
	}

	int CFepModbusPrtcl::RxDataProc( Byte* pDataBuf, int& nLen )
	{
		//! ͨ���������ֹ�ˣ���ֱ�ӷ���0
		CFepModbusDriver* pDriver = (CFepModbusDriver*)m_pParam;
		if ( !pDriver || !m_pbyInnerBuf || !m_pbyRevBuffer || !m_sPacketBuffer )
			return -1;		
		if(  pDriver->IsDisable() )
		{
			return 0;
		}

		if ( !pDriver->CanWork() )
		{
			return 0;
		}
		char pchLog[MAXDEBUGARRAYLEN] = {0};

		//! �����Ҫͨ�ż��ӣ�����ԭʼ���ݰ�
		if ( pDriver->IsMonitor() )
		{
			pDriver->WriteCommData( pDataBuf, nLen );
		}

		//�����ݷŵ��ڲ�������
		InnerAddData( pDataBuf, nLen );
		//if (pDriver->IsLog())
		//{
		//	snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusPrtcl::RxDataProc Recieve data Len:%d", nLen);
		//	pDriver->WriteLogAnyWay(pchLog);
		//}

		//���ڲ���������ִ�л������ڣ������ݰ���ע��һ�ο��ܻᴦ�������ݰ�
		int	nReceived = 0;
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
					int nPacketLen = 0;

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
							bCompleted = true;

							//Modbus TCPЭ�������������մ�����ΪЭ�����С�����š���Ե��
							ProcessRcvdPacket( m_sPacketBuffer, nPacketLen, true );	
						}
					}while(bTemp);

					//�ҵ����ݰ������
					if ( bCompleted )
					{
						//! ���ۺ���Э�飬�������������꣬�����ɹ����ٷ���һ������
						int nDataLen = 0;
						BuildRequest( m_pbySndBuffer, nDataLen );

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

	int CFepModbusPrtcl::StopTimer()
	{
		int nRes = 0;
		//if ( m_pIOTimer )
		//{
		//	nRes = m_pIOTimer->KillTimer();
		//}

		//if ( m_pPeriodTimer )
		//{
		//	nRes = m_pPeriodTimer->KillTimer();
		//}

		//if ( m_pInsertDelayTimer )
		//{
		//	nRes = m_pInsertDelayTimer->KillTimer();
		//}
		return nRes;
	}
}

#endif
