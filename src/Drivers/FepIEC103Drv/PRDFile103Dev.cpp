#include "RTDB/Server/DBDriverFrame/IODriver.h"
#include "PRDFile103Dev.h"
#include "PublicFileH.h"
#include "FepIEC103Drv.h"
#include "Common/CommonXmlParser/PRDCfgParser.h"

namespace MACS_SCADA_SUD
{
	bool operator<(const T_DIFunInf& tLeft, const T_DIFunInf& tRight)
	{
		if ( tLeft.nFun<tRight.nFun )
		{
			return true;
		}
		if ( tLeft.nFun>tRight.nFun )
		{
			return false;
		}
		return tLeft.nInf<tRight.nInf;
	}

	bool operator==(const T_DIFunInf& tLeft, const T_DIFunInf& tRight)
	{
		if ( tLeft.nFun==tRight.nFun && tLeft.nInf==tRight.nInf )
		{
			return true;
		}
		return false;
	}


	CPRDFile103Dev::CPRDFile103Dev(void)
	{
		m_pLog = NULL;
		Clear();
		Init();		
	}

	CPRDFile103Dev::~CPRDFile103Dev(void)
	{
		Clear();
		if (m_pIEC103Drv)
		{
			m_pIEC103Drv = NULL;
		}
		if(m_pLog)
		{
			m_pLog = NULL;
		}
	}

	CPRDFile103Dev::CPRDFile103Dev( std::string strDrvName,std::string szDevName, CFepIEC103Drv* pIEC103Drv )
	{
		m_pLog = NULL;
		m_strDrvName = strDrvName;
		m_szDevName = szDevName;
		m_pIEC103Drv = pIEC103Drv;
	}

	void CPRDFile103Dev::Init(  )
	{
		OpcUa_CharA buf[64];
		//! ��ȡ�����ļ�����·��
		std::string file_name = CIODriver::GetFEPRunDir() + MACS_DRIVER_FILENAME;
		

		OpcUa_Int32 iIndex = 0;			
		std::string strDir;
		strDir = m_szDevName;
		//!����¼���ļ��Ĵ��·��
		OpcUa_Int32 iDebugMode = GetPrivateProfileInt( "FepIEC103", "DebugMode",0, file_name.c_str());
		//!����ģʽ
		if ( iDebugMode )
		{			
			GetPrivateProfileString( "FepIEC103", "PRDFileDir","/mnt/Comtrade/",buf, 255, file_name.c_str());
			m_szPrdFilePath = buf;
			m_szPrdFilePath  += strDir;				
		}
		//!��������ģʽ
		else
		{
			//!����¼���ļ��Ĵ��·��
			GetPrivateProfileString( m_strDrvName.c_str(), "PRDFileDir","/mnt/Comtrade/F650/",buf, 255, file_name.c_str());
			m_szPrdFilePath = buf;
			m_szPrdFilePath  += strDir;			
		}		
		
		try
		{
			if (m_pIEC103Drv)
			{
				CPRDCfgParser* prdParser = m_pIEC103Drv->GetPRDCfgParser();
				if (prdParser)
				{
					CWaveItemType* pWaveItem = prdParser->GetWaveItem();
					if (pWaveItem)
					{
						OpcUa_Int32 nDrvCount = pWaveItem->GetDrvItemCount();
						if (nDrvCount > 0)
						{
							for (OpcUa_Int32 i = 0; i < nDrvCount; i++)
							{
								CDriverItemType* pDrvItem = pWaveItem->GetDrvItemAt(i);
								std::string strDrvName = pDrvItem->GetDrvName();
								if (m_strDrvName == strDrvName)
								{
									Parse103PRDCfg(pDrvItem, m_szDevName);
								}
							}
						}
					}
				}
			}		
		}
		catch(...)
		{			
			OpcUa_CharA chMsg[256];
			snprintf(chMsg,sizeof(chMsg),"����103¼�������ļ�ʧ�� CPRDFile103Dev::Init failed, file= %s!\n");
			if (m_pLog)
			{
				m_pLog->WriteLog( chMsg );
			}		
		}
		return;
	}

	OpcUa_Int32 CPRDFile103Dev::DealRcvData( OpcUa_Byte* pbyTemp )
	{
		OpcUa_Byte* pbyData = pbyTemp + 6;
		//! ���ͱ�ʶ
		OpcUa_Byte byType = pbyData[0];
		
		OpcUa_CharA chMsg[32];
		switch ( byType )
		{
		case 23:
			if ( DealASDU23( pbyData )!=0 )
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 23 -1");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
				return -1;
			}
			else
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 23");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
			}
			break;
		case 26:
			if ( DealASDU26( pbyData )!=0 )
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 26 -2");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
				return -2;
			}
			else
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 26");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
			}
			break;
		case 28:
			if ( DealASDU28( pbyData )!=0 )
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 28 -3");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
				return -3;
			}
			else
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 28");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
			}
			break;
		case 29:
			if ( DealASDU29( pbyData )!=0 )
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 29 -4");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
				return -4;
			}
			else
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 29");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
			}
			break;
		case 31:
			if ( DealASDU31( pbyData )!=0 )
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 31 -5");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
				return -5;
			}
			else
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 31");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
			}
			break;
		case 27:
			if ( DealASDU27( pbyData )!=0 )
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 27 -6");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
				return -6;
			}
			else
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 27");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
			}
			break;
		case 30:
			if ( DealASDU30( pbyData )!=0 )
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 30  -6");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
				return -6;
			}
			else
			{
				snprintf(chMsg,sizeof(chMsg),"DealRcvData 30");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
			}
			break;
		default:
			break;
		}

		return 0;
	}

	//! �����־
	void CPRDFile103Dev::ClearFlag()
	{
		m_byPartType = 0;
		m_byFrameLabel = 0;
		m_byCompleteType = 0;
	}

	void CPRDFile103Dev::Clear()
	{
		ClearFlag();

		m_mapFaultNumberTimeIndex.clear();
		m_nCurFaultNumber = 0;
		m_nNetFaultNumber = 0;
		m_nChannelCount = 0;
		m_nElementCount = 0;
		m_nElementInterval = 0;
		m_szRecordTime = "";
		m_mapDIChannel.clear();
		m_mapAIChannel.clear();

		T_AIChannelValIndex::iterator it;
		for( it=m_mapAIVal.begin(); it!=m_mapAIVal.end(); it++ )
		{
			it->second.clear();
		}
		m_mapAIVal.clear();

		/*	//! Del by hxw in 2009.04.07. 
		m_nMinVal = 0;
		m_nMaxVal = 0;
		*/
		//! Add by hxw in 2009.04.07. ����Сֵ�����ĳ�ֵ��Ϊ�������ֵ�������ֵ�����ĳ�ֵ��Ϊ������Сֵ������ȷ�����������������ܵõ�ʵ�ʵ���ֵ��
		m_nMinVal = 32767;
		m_nMaxVal = -32768;
	}

	//! �õ�4����λλ��Ķ�����ʱ����ַ���
	std::string CPRDFile103Dev::Get4BytesTimeString( OpcUa_Byte* pData )
	{
		OpcUa_Int32 nMSec = 0;
		memcpy( &nMSec, pData, 2 );
		T_MinByte min;
		memcpy( &min, pData+2, 1 );
		T_HourByte hour;
		memcpy( &hour, pData+3, 1 );
		std::string szTime;
		OpcUa_CharA pchTime[64];
		OpcUa_Int32 nSec = nMSec/1000;
		nMSec = nMSec%1000;
		//! ʱ���ʽ��hh:mm:ss.ssssss
		sprintf( pchTime, "%02d:%02d:%02d.%06d", hour.HOUR, min.MIN, nSec, nMSec*1000 );		// *1000 for us
		szTime = pchTime;
		return szTime;
	}

	//! �õ�7����λλ��Ķ�����ʱ����ַ���
	std::string CPRDFile103Dev::Get7BytesTimeString( OpcUa_Byte* pData )
	{
		OpcUa_Int32 nMSec = 0;
		memcpy( &nMSec, pData, 2 );
		T_MinByte min;
		memcpy( &min, pData+2, 1 );
		T_HourByte hour;
		memcpy( &hour, pData+3, 1 );
		T_DayByte day;
		memcpy( &day, pData+4, 1 );
		T_MonthByte month;
		memcpy( &month, pData+5, 1 );
		T_YearByte year;
		memcpy( &year, pData+6, 1 );
		std::string szTime;
		OpcUa_CharA pchTime[64];
		OpcUa_Int32 nSec = nMSec/1000;
		nMSec = nMSec%1000;

		OpcUa_Int32 nYear = year.YEAR + 2000;
		//! ʱ���ʽ��dd/mm/yyyy,hh:mm:ss.ssssss
		sprintf( pchTime, "%02d/%02d/%04d,%02d:%02d:%02d.%06d", 
			day.DAY, month.MONTH, nYear,
			hour.HOUR, min.MIN, nSec, nMSec*1000 );		// *1000 for us

		szTime = pchTime;
		return szTime;
	}

	//! �������ASDU23
	OpcUa_Int32 CPRDFile103Dev::DealASDU23( OpcUa_Byte* pData )
	{
		OpcUa_CharA chMsg[256];
		//! �����ڴ�ͱ�־
		Clear();

		//! �ֽ�֡--------------------------------

		//! ��ϢԪ�ص���Ŀ
		OpcUa_Byte byCount = pData[1];
		//! ����ÿ����ϢԪ�صĹ�����ź͹��Ϸ���ʱ��
		for ( OpcUa_Int32 i=0; i<byCount ;i++ )
		{
			OpcUa_Int32 nFaultNumber = 0;
			//! �������
			memcpy( &nFaultNumber, pData+6+i*10, 2 );
			std::string szTime;
			//! ���Ϸ���ʱ��
			szTime = Get7BytesTimeString( pData+6+i*10+3 );

			//! ������������
			m_mapFaultNumberTimeIndex.insert( std::map<OpcUa_Int32, std::string>::value_type( nFaultNumber, szTime ) );

			if ( i == byCount-1 )
			{
				T_FaultType faultInfo;			
				snprintf(chMsg,sizeof(chMsg),"%d",nFaultNumber);
				faultInfo.szFaultNumber = chMsg;
				faultInfo.szOcurTime = szTime;
				//!�����ö��豸������¼���ļ���Ϣ
				//m_pDealImpl->SetLatestFaultFile( m_szDevName, faultInfo);
			}
		}

		//! �ñ�־
		m_byPartType = 1;
		m_byFrameLabel = 23;
		m_byCompleteType = 0;
		
		snprintf(chMsg,sizeof(chMsg),"DealASDU23,byCount=%d",byCount);
		if (m_pLog)
		{
			m_pLog->WriteLog( chMsg );
		}
		

		return 0;
	}

	//! �������ASDU26
	OpcUa_Int32 CPRDFile103Dev::DealASDU26( OpcUa_Byte* pData )
	{
		std::string szTime;
		if ( m_byPartType==1 && m_byFrameLabel==23 )
		{
			//! ��ǰ����Ĺ������
			memcpy( &m_nCurFaultNumber, pData+8, 2 );

			//! �����������
			memcpy( &m_nNetFaultNumber, pData+10, 2 );

			//! ͨ����Ŀ
			memcpy( &m_nChannelCount, pData+12, 1 );

			//! һ��ͨ����ϢԪ�ص���Ŀ
			memcpy( &m_nElementCount, pData+13, 2 );

			//! ��ϢԪ�ؼ�ļ��
			memcpy( &m_nElementInterval, pData+15, 2 );

			//! ��ʼ��¼��ʱ��
			std::string str;
			szTime = Get4BytesTimeString( pData+17 );
			//! �ҵ��ù��ϵĹ��Ϸ���ʱ�䣬�õ�������Ϣ��
			std::map<OpcUa_Int32, std::string>::iterator it;
			it = m_mapFaultNumberTimeIndex.find( m_nCurFaultNumber );
			if ( it!=m_mapFaultNumberTimeIndex.end() )
			{
				str = it->second;
			}
			else
			{
				ClearFlag();
				return -2;
			}
			str = str.substr( 0, 11 );
			m_szRecordTime = str + szTime;
		}
		else
		{
			ClearFlag();
			return -1;
		}

		//! �ñ�־
		m_byPartType = 1;
		m_byFrameLabel = 26;
		m_byCompleteType = 0;

		OpcUa_CharA chMsg[256];
		snprintf(chMsg,sizeof(chMsg),"DealASDU26,m_nCurFaultNumber=%d,m_nNetFaultNumber=%d,m_nChannelCount = %d,m_nElementInterval =%d,szTime=%s"
			,m_nCurFaultNumber,m_nNetFaultNumber,m_nChannelCount,m_nElementInterval,szTime.c_str());
		if (m_pLog)
		{
			m_pLog->WriteLog( chMsg );
		}
		

		return 0;
	}

	//! �������ASDU28
	OpcUa_Int32 CPRDFile103Dev::DealASDU28( OpcUa_Byte* pData )
	{
		if ( m_byPartType==1 && m_byFrameLabel==26 )
		{
			//! У���������Ƿ���ȷ
			OpcUa_Int32 nFaultNumber = 0;
			memcpy( &nFaultNumber, pData+8, 2 );
			if ( nFaultNumber==m_nCurFaultNumber )
			{
				m_byPartType = 2;
				m_byFrameLabel = 28;
				m_byCompleteType = 0;
			}
			else
			{
				ClearFlag();
				return -2;
			}
		}
		else
		{
			ClearFlag();
			return -1;
		}

		OpcUa_CharA chMsg[32];
		snprintf(chMsg,sizeof(chMsg),"DealASDU28");
		if (m_pLog)
		{
			m_pLog->WriteLog( chMsg );
		}
		

		return 0;
	}

	//! �������ASDU29
	OpcUa_Int32 CPRDFile103Dev::DealASDU29( OpcUa_Byte* pData )
	{
		//! ��־��λ��
		OpcUa_Int32 nTap = 0;
		memcpy( &nTap, pData+9, 2 );

		OpcUa_Byte byFlag = 0;
		if ( nTap==0 && m_byPartType==2 && m_byFrameLabel==28 )		//����־��״̬��λ�ĳ�ʼ״̬
		{
			byFlag = 1;
		}
		else if ( nTap>0 && m_byPartType==2 && m_byFrameLabel==29 )	//����־��״̬��λ
		{
			byFlag = 2;
		}
		else
		{
			ClearFlag();
			return -1;
		}

		//! У���������Ƿ���ȷ
		OpcUa_Int32 nFaultNumber = 0;
		memcpy( &nFaultNumber, pData+6, 2 );
		if ( nFaultNumber!=m_nCurFaultNumber )
		{
			ClearFlag();
			return -2;
		}

		//
		OpcUa_Byte byCount = pData[8];
		for( OpcUa_Int32 i=0; i<byCount; i++ )
		{
			T_DIFunInf funInf;
			memcpy( &(funInf.nFun), pData+11+i*3, 1 );
			memcpy( &(funInf.nInf), pData+11+i*3+1, 1 );
			OpcUa_Byte byVal = pData[11+i*3+2] - 1;			//! ע��pData[11+i*3+2]��1Ϊ����2Ϊ��
			T_DIChannelValType diVal;
			if ( byFlag==1 )
			{
				diVal.nOrgVal = byVal;
				diVal.nOrgPos = 0;
				diVal.nChgPos = 0;
				diVal.nChgVal = 0;
				m_mapDIChannel.insert( std::map<T_DIFunInf, T_DIChannelValType>::value_type( funInf, diVal ) );
			}
			else
			{
				std::map<T_DIFunInf, T_DIChannelValType>::iterator it;
				it = m_mapDIChannel.find( funInf );
				if ( it!=m_mapDIChannel.end() )
				{
					it->second.nChgVal = byVal;
					it->second.nChgPos = nTap;
				}
				else
				{
					diVal.nOrgVal = 0;
					diVal.nOrgPos = 0;
					diVal.nChgPos = nTap;
					diVal.nChgVal = byVal;
					m_mapDIChannel.insert( std::map<T_DIFunInf, T_DIChannelValType>::value_type( funInf, diVal ) );
				}
			}
		}

		//! �ñ�־
		m_byPartType = 2;
		m_byFrameLabel = 29;
		m_byCompleteType = 0;

		OpcUa_CharA chMsg[32];
		snprintf(chMsg,sizeof(chMsg),"DealASDU29");
		if (m_pLog)
		{
			m_pLog->WriteLog( chMsg );
		}
		

		return 0;
	}

	//! �������ASDU31
	OpcUa_Int32 CPRDFile103Dev::DealASDU31( OpcUa_Byte* pData )
	{
		//! У���������Ƿ���ȷ
		OpcUa_Int32 nFaultNumber = 0;
		memcpy( &nFaultNumber, pData+8, 2 );
		if ( nFaultNumber!=m_nCurFaultNumber )
		{
			ClearFlag();
			return -2;
		}

		//! ��������
		OpcUa_Byte byCommandType = pData[6];

		OpcUa_CharA chMsg[64];
		snprintf(chMsg,sizeof(chMsg),"DealASDU31:%d",byCommandType);
		if (m_pLog)
		{
			m_pLog->WriteLog( chMsg );
		}
		//! ������ֹ���Ŷ����ݴ���Ľ���
		if ( byCommandType==32)
		{
			m_byCompleteType = 1;
		}
		//! �ɿ���ϵͳ����ֹ���Ŷ����ݴ���Ľ���
		else if ( byCommandType==33)
		{
			m_byCompleteType = 1;
		}
		//! �ɼ̵籣���豸����ֹ���Ŷ����ݴ���Ľ���
		else if ( byCommandType==34 )
		{
			m_byCompleteType = 1;
		}
		//! ������ֹ��ͨ������Ľ���
		else if ( byCommandType==35 )
		{
			m_byCompleteType = 3;
		}
		//! �ɿ���ϵͳ����ֹ��ͨ������Ľ���
		else if ( byCommandType==36 )
		{
			m_byCompleteType = 3;
		}
		//! �ɼ̵籣���豸����ֹ��ͨ������Ľ���
		else if ( byCommandType==37 )
		{
			m_byCompleteType = 3;
		}
		//! ������ֹ�Ĵ���־��״̬��λ�Ĵ���Ľ���
		else if ( byCommandType==38 )
		{
			m_byCompleteType = 2;
		}
		//! �ɿ���ϵͳ����ֹ�Ĵ���־��״̬��λ�Ĵ���Ľ���
		else if ( byCommandType==39 )
		{
			m_byCompleteType = 2;
		}
		//! �ɼ̵籣���豸����ֹ�Ĵ���־��״̬��λ����Ľ���
		else if ( byCommandType==40 )
		{
			m_byCompleteType = 2;
		}
		else
		{
			ClearFlag();
			return -1;
		}

		//! �Ŷ����ݴ�������������ļ�
		if ( m_byCompleteType == 1 )
		{ 
			//OpcUa_CharA chMsg[64];
			snprintf(chMsg,sizeof(chMsg),"DealASDU31:����¼���ļ�");
			if (m_pLog)
			{
				m_pLog->WriteLog( chMsg );
			}
			
			

			ProduceCfgFile();
			ProduceDatFile();

		}

		return 0;
	}

	//! �������ASDU27
	OpcUa_Int32 CPRDFile103Dev::DealASDU27( OpcUa_Byte* pData )
	{
		//! У���������Ƿ���ȷ
		OpcUa_Int32 nFaultNumber = 0;
		memcpy( &nFaultNumber, pData+8, 2 );
		if ( nFaultNumber!=m_nCurFaultNumber )
		{
			ClearFlag();
			return -2;
		}

		if ( m_byCompleteType==2 || m_byCompleteType==3 )
		{
			OpcUa_Byte byAcc = pData[10];
			T_AIChannelInfo aiInfo;
			//! һ�ζֵ
			memcpy( &(aiInfo.fPrim), pData+11, 4 );

			//! ���ζֵ
			memcpy( &(aiInfo.fSecond), pData+15, 4 );

			//! �α�����
			memcpy( &(aiInfo.fRFA), pData+19, 4 );

			//! ������������
			m_mapAIChannel.insert( std::map<OpcUa_Byte, T_AIChannelInfo>::value_type( byAcc, aiInfo ) );

			//! �޸ı�־
			m_byPartType = 3;
			m_byFrameLabel = byAcc;
			m_byCompleteType = 0;

			OpcUa_CharA chMsg[256];
			snprintf(chMsg,sizeof(chMsg),"DealASDU27,byAcc=%d ,fPrim=%g,fSecond=%g,fRFA = %g"
				,byAcc,aiInfo.fPrim,aiInfo.fSecond,aiInfo.fRFA);
			if (m_pLog)
			{
				m_pLog->WriteLog( chMsg );
			}
			
		}
		return 0;
	}

	/*	Del by hxw in 2009.04.08
	OpcUa_Int32 CPRDFile103Dev::DealASDU30( OpcUa_Byte* pData )
	{
	if ( m_byCompleteType>0 )
	{
	ClearFlag();
	return -2;
	}

	//! У���������Ƿ���ȷ
	OpcUa_Int32 nFaultNumber = 0;
	memcpy( &nFaultNumber, pData+8, 2 );
	if ( nFaultNumber!=m_nCurFaultNumber )
	{
	ClearFlag();
	return -3;
	}

	//! ʵ��ͨ�����
	OpcUa_Byte byAcc = pData[10];

	if ( m_byPartType==3 && byAcc==m_byFrameLabel )
	{
	//! Ӧ�÷������ݵ�Ԫ��һ����ϢԪ�ص����
	OpcUa_Int32 nNEF = 0;
	memcpy( &nNEF, pData+12, 2 );

	std::map<OpcUa_Byte, T_AIChannelInfo>::iterator it;
	it = m_mapAIChannel.find( byAcc );
	if ( it==m_mapAIChannel.end() )
	{
	ClearFlag();
	return -4;
	}

	//! ÿ��Ӧ�÷������ݵ�Ԫ�й��Ŷ�ֵ����Ŀ
	OpcUa_Int32 nValCount = 0;
	memcpy( &nValCount, pData+11, 1 );		//? �˴����׼��ͬ����׼����2���ֽ�
	it->second.nDataCount += nValCount;

	T_AIChannelVAL vecVal;
	vecVal.resize( m_nElementCount );
	T_AIChannelValIndex::iterator itVal;
	itVal = m_mapAIVal.find( byAcc );

	//! �Ŷ�ֵ
	for ( OpcUa_Int32 i=0; i<nValCount; i++ )
	{
	OpcUa_Int16 nVal = 0;
	memcpy( &nVal, pData+14+i*2, 2 );

	//! ���������Сֵ
	if ( nVal>=m_nMaxVal )
	{
	m_nMaxVal = nVal;
	}
	if ( nVal<m_nMinVal )
	{
	m_nMinVal = nVal;
	}

	//! ����Ѿ���ֵ�ˣ���ֱ�Ӽ���
	if ( itVal!=m_mapAIVal.end() )
	{
	//itVal->second.push_back( nVal );
	itVal->second[i] = nVal;
	}
	//! û�У������VECTOR��
	else
	{
	//vecVal.push_back( nVal );
	vecVal[i] = nVal;
	}
	}

	OpcUa_Int32 nCount = vecVal.size();

	//! û�е�, ������������
	if ( itVal==m_mapAIVal.end() )
	{
	m_mapAIVal.insert( T_AIChannelValIndex::value_type( byAcc, vecVal ) );
	}

	}
	else
	{
	ClearFlag();
	return -1;
	}

	return 0;
	}

	*/


	// Add by hxw in 2009.04.08=================begin
	//! �������ASDU30. 
	OpcUa_Int32 CPRDFile103Dev::DealASDU30( OpcUa_Byte* pData )
	{
		OpcUa_CharA chMsg[32];
		if ( m_byCompleteType>0 )
		{			
			snprintf(chMsg,sizeof(chMsg),"DealASDU30 -2");
			if (m_pLog)
			{
				m_pLog->WriteLog( chMsg );
			}

			ClearFlag();
			return -2;
		}

		//! У���������Ƿ���ȷ
		OpcUa_Int32 nFaultNumber = 0;
		memcpy( &nFaultNumber, pData+8, 2 );
		if ( nFaultNumber!=m_nCurFaultNumber )
		{
			snprintf(chMsg,sizeof(chMsg),"DealASDU30 -3");
			if (m_pLog)
			{
				m_pLog->WriteLog( chMsg );
			}
			ClearFlag();
			return -3;
		}

		//! ʵ��ͨ�����
		OpcUa_Byte byAcc = pData[10];

		if ( m_byPartType==3 && byAcc==m_byFrameLabel )
		{
			//! Ӧ�÷������ݵ�Ԫ��һ����ϢԪ�ص����
			OpcUa_Int32 nNEF = 0;
			memcpy( &nNEF, pData+12, 2 );

			std::map<OpcUa_Byte, T_AIChannelInfo>::iterator it;
			it = m_mapAIChannel.find( byAcc );
			if ( it==m_mapAIChannel.end() )
			{
				snprintf(chMsg,sizeof(chMsg),"DealASDU30 -4");
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
				ClearFlag();
				return -4;
			}

			//! ÿ��Ӧ�÷������ݵ�Ԫ�й��Ŷ�ֵ����Ŀ
			OpcUa_Int32 nValCount = 0;
			memcpy( &nValCount, pData+11, 1 );		//? �˴����׼��ͬ����׼����2���ֽ�
			it->second.nDataCount += nValCount;

			T_AIChannelVAL vecVal;
			//vecVal.resize( m_nElementCount );		//Del by hxw in 2009.04.07. ����������һ�ζ���Ԫ�ص�����С����Ϊһ��ͨ���������Ŷ�ֵ��Ŀ������Ҫ��֡���ͣ�һ֡���25��AI�㣩
			vecVal.resize( nValCount );				//Add by hxw in 2009.04.07. ��СΪ��֡���Ŷ�ֵ����

			T_AIChannelValIndex::iterator itVal;
			itVal = m_mapAIVal.find( byAcc );

			//! �Ŷ�ֵ
			OpcUa_Int16 nVal = 0;
			for ( OpcUa_Int32 i=0; i<nValCount; i++ )
			{
				memcpy( &nVal, pData+14+i*2, 2 );

				//! ���������Сֵ
				if ( nVal>m_nMaxVal )
				{
					m_nMaxVal = nVal;
				}
				if ( nVal<m_nMinVal )
				{
					m_nMinVal = nVal;
				}

				//! ����Ѿ���ֵ�ˣ���ֱ�Ӽ���. 2009.04.07����˵����by hxw���������ͨ���Ѿ������ˣ���ֱ�Ӱѵ�ֵ���������뼴�ɡ�
				if ( itVal!=m_mapAIVal.end() )
				{
					itVal->second.push_back( nVal );	//<! Add by hxw in 2009.04.07��ע�⣬������[i]�ķ�ʽ������ú���ͬͨ���ĵ�ֵ���ص�����ǰ0-nValCount-1���㣬���Ǵ���ģ�
					//itVal->second[i] = nVal;			//<! Del by hxw in 2009.04.07
				}
				//! û�У������VECTOR��
				else
				{
					//vecVal.push_back( nVal );			
					vecVal[i] = nVal;					
				}
			}

			//! û�е�, ������������
			if ( itVal==m_mapAIVal.end() )
			{
				m_mapAIVal.insert( T_AIChannelValIndex::value_type( byAcc, vecVal ) );
			}

		}
		else
		{
			snprintf(chMsg,sizeof(chMsg),"DealASDU30 -1");
			if (m_pLog)
			{
				m_pLog->WriteLog( chMsg );
			}
			ClearFlag();
			return -1;
		}

		//OpcUa_CharA chMsg[32];
		snprintf(chMsg,sizeof(chMsg),"DealASDU30");
		if (m_pLog)
		{
			m_pLog->WriteLog( chMsg );
		}
		

		return 0;
	}
	// Add by hxw in 2009.04.08=================end


	//! ���ɵ�ǰ������ŵ��ļ���
	OpcUa_Int32 CPRDFile103Dev::GetComtradeFileName()
	{
		std::string szCurTime;
		std::map<OpcUa_Int32, std::string>::iterator it;
		it = m_mapFaultNumberTimeIndex.find( m_nCurFaultNumber );
		if ( it!=m_mapFaultNumberTimeIndex.end() )
		{
			szCurTime = it->second;
		}
		else
		{
			return -1;
		}
		OpcUa_CharA pchBuf[128];
		strcpy( pchBuf, szCurTime.c_str() );
		OpcUa_Int32 nYear, nMonth, nDay, nHour, nMin, nSec, nMSec;
		sscanf( pchBuf, "%02d/%02d/%04d,%02d:%02d:%02d.%06d", &nDay, &nMonth, &nYear,  &nHour, &nMin, &nSec, &nMSec );
		sprintf( pchBuf, "%04d%02d%02d%02d%02d%02d%06d", nYear, nMonth, nDay, nHour, nMin, nSec, nMSec );

		szCurTime = pchBuf;
		std::string sz;
		sprintf( pchBuf, "%d", m_nCurFaultNumber );
		m_szCurFileName = m_szDevName + "_" + szCurTime + "_" + pchBuf;

		return 0;
	}	

	//! ����CFG�ļ�
	OpcUa_Int32 CPRDFile103Dev::ProduceCfgFile()
	{
		GetComtradeFileName();
		//ACE_stat fdata;
		//if( ACE_OS::stat( m_szPrdFilePath.c_str(), &fdata ) < 0 )
		//{
		//	ACE_OS::mkdir( m_szPrdFilePath.c_str() );
		//}
		////!��Ŀ¼
		//if ( !(fdata.st_mode&S_IFDIR) )
		//{
		//	ACE_OS::mkdir( m_szPrdFilePath.c_str() );
		//}	
		CreateDir( m_szPrdFilePath.c_str());

		OpcUa_CharA chMsg[256];	
		std::string strCfgFileName;	
		snprintf(chMsg,sizeof(chMsg),"%s/%s.cfg", m_szPrdFilePath.c_str(), m_szCurFileName.c_str());
		strCfgFileName = chMsg;			
		
		if (m_pLog)
		{
			m_pLog->WriteLog( chMsg );
		}

		ofstream fWrite;
		fWrite.open(strCfgFileName.c_str());
		if ( !fWrite )
		{
			snprintf(chMsg,sizeof(chMsg),"CPRDFile103Dev::ProduceDatFile ,create cfg file failed");
			if (m_pLog)
			{
				m_pLog->WriteLog( chMsg );
			}
			return -1;
		}
		std::string strLine;

		//! ��վ���ƣ��豸���ƣ��汾��
		snprintf(chMsg,sizeof(chMsg),"%s,%s,%d", "Beijing Metro Line 8", m_szDevName.c_str(), 1999);
		strLine = chMsg;
		fWrite<<strLine<<std::endl;	

		//! ͨ��������ģ����ͨ������A��������ͨ������D	
		snprintf(chMsg,sizeof(chMsg),"%d,%dA,%dD",
			m_mapDIChannel.size()+m_mapAIChannel.size(), 
			m_mapAIChannel.size(), 
			m_mapDIChannel.size() 
			);
		strLine = chMsg;
		fWrite<<strLine<<std::endl;	

		//! ģ����ͨ��
		//! ͨ�������ţ�ͨ�����ƣ����A/B/C/N/R/Y/B����ʵ��ͨ���ţ���λ��A/V����ϵ��a��ϵ��b��
		//! �Ӳ������ڿ�ʼ��ʱ��ƫ��������ã�ȱʡΪ0������Сֵ��MIN��ȱʡ-1000�������ֵ��MAX��ȱʡ1000����
		//! �һ��ֵ�������ֵ��ʱ�䵥λ��S��
		OpcUa_Int32 nIndex = 0;
		std::map<OpcUa_Byte, T_AIChannelInfo>::iterator itAi;
		for ( itAi=m_mapAIChannel.begin(); itAi!=m_mapAIChannel.end(); itAi++ )
		{
			nIndex ++;
			OpcUa_Byte nAcc = itAi->first;
			T_CtdChannelInfo channelInfo;
			
			if ( GetCtdChannelInfo( nAcc, channelInfo )!=0 )
			{				
				OpcUa_CharA pchTempBuf[64];
				sprintf( pchTempBuf, "UnknownAiChanNameAcc%d", nAcc );
				channelInfo.szName  = pchTempBuf;
				channelInfo.szPhase = "?";
				channelInfo.szUnits = "?";				
			}
			snprintf(chMsg,sizeof(chMsg),"%d,%s,%s,%d,%s,%.6f,%.6f,%.6f,%d,%d,%.6f,%.6f,S",
				nIndex, channelInfo.szName.c_str(), channelInfo.szPhase.c_str(), nAcc, channelInfo.szUnits.c_str(),
				itAi->second.fRFA, 0.0, 0.0, m_nMinVal, m_nMaxVal, 
				itAi->second.fPrim, itAi->second.fSecond 
				);

			strLine = chMsg;
			fWrite<<strLine<<std::endl;	
		}

		//! ������ͨ��
		//! ͨ���ţ�ͨ�����ƣ�ͨ����� ��·�ţ�ͨ������״̬��0/1��
		nIndex = 0;
		std::map<T_DIFunInf, T_DIChannelValType>::iterator itDi;
		for ( itDi=m_mapDIChannel.begin(); itDi!=m_mapDIChannel.end(); itDi++ )
		{
			nIndex ++;
			T_CtdChgInfo chgInfo;			
			if ( GetCtdChgInfo( itDi->first.nFun, itDi->first.nInf, chgInfo )!=0 )
			{				
				OpcUa_CharA pchTempBuf[64];
				sprintf( pchTempBuf, "UnknownDiLineFun%dInf%d", itDi->first.nFun, itDi->first.nInf );
				chgInfo.szLineNo = pchTempBuf;
				sprintf( pchTempBuf, "UnknowDiChanNameFun%dInf%d", itDi->first.nFun, itDi->first.nInf );
				chgInfo.szName = pchTempBuf;				
			}

			if (m_pLog)
			{
				snprintf(chMsg,sizeof(chMsg),"ProduceCfgFile ״̬����������:nFun=%d,nInf=%d",itDi->first.nFun, itDi->first.nInf);
				if (m_pLog)
				{
					m_pLog->WriteLog( chMsg );
				}
			}

			snprintf(chMsg,sizeof(chMsg),"%d,%s,,%s,%d",
				nIndex, chgInfo.szName.c_str(), chgInfo.szLineNo.c_str(), itDi->second.nOrgVal
				);
			strLine = chMsg;
			fWrite<<strLine<<std::endl;	
		}

		//! ��·Ƶ��	
		fWrite<<"50" <<std::endl;	

		//! �����ʸ���	
		fWrite<<"1" <<std::endl;


		//! �����ʣ���������
		OpcUa_Float fSec = m_nElementCount*m_nElementInterval/(1000000*1.0);
		OpcUa_Float fSample = m_nElementCount*1.0/fSec;
		snprintf(chMsg,sizeof(chMsg), "%.6f,%d", fSample, m_nElementCount);
		strLine = chMsg;	
		fWrite<<strLine <<std::endl;


		//! ��ʼ��¼��ʱ��
		strLine = m_szRecordTime.c_str();	
		fWrite<<strLine<<std::endl;


		//! ���Ϸ�����ʱ��
		std::map<OpcUa_Int32, std::string>::iterator it;
		it = m_mapFaultNumberTimeIndex.find( m_nCurFaultNumber );
		if ( it==m_mapFaultNumberTimeIndex.end() )
		{
			fWrite.close();
			return -4;
		}	
		snprintf(chMsg,sizeof(chMsg), "%s", it->second.c_str());
		strLine = chMsg;	
		fWrite<<strLine  <<std::endl;

		//! �����ļ�����	
		fWrite<<"ASCII"<<std::endl;

		//!	
		fWrite<<"1.000000"<<std::endl;


		fWrite.close();
		return 0;
	}

	//! ����dat�ļ�
	OpcUa_Int32 CPRDFile103Dev::ProduceDatFile()
	{	
		std::string strCfgFileName;
		OpcUa_CharA chMsg[256];
		snprintf(chMsg,sizeof(chMsg),"%s/%s.dat",m_szPrdFilePath.c_str(), m_szCurFileName.c_str());
		strCfgFileName = chMsg;

		if (m_pLog)
		{
			m_pLog->WriteLog( chMsg );
		}

		ofstream fWrite;
		fWrite.open(strCfgFileName.c_str());
		if ( !fWrite )
		{
			snprintf(chMsg,sizeof(chMsg),"CPRDFile103Dev::ProduceDatFile ,create data file failed");
			if (m_pLog)
			{
				m_pLog->WriteLog( chMsg );
			}
			return -1;
		}
		std::string strLine;	
		//! ����ͨ����ϢԪ����Ŀ
		for ( OpcUa_Int32 i=0; i<m_nElementCount; i++ )
		{
			//! ÿ�еĵ�һ��������ֵ�ֱ��ʾ��ϢԪ����ţ���1��ʼ������ϢԪ�ؼ���ۼƼ��
			ACE_OS::sprintf( chMsg,/*sizeof(chMsg),*/ "%d,%d", i+1, i*m_nElementInterval );

			//! ������������ֵ��ʾ����ϢԪ�غ�������ģ����ͨ����Ӧ��AI��
			T_AIChannelValIndex::iterator itAi;
			for ( itAi=m_mapAIVal.begin(); itAi!=m_mapAIVal.end(); itAi++ )
			{
				OpcUa_Int32 nDataCount = itAi->second.size();
				ACE_OS::sprintf( chMsg,/*sizeof(chMsg), */"%s,%d", chMsg, itAi->second[i] );
			}


			//! ������������ֵ��ʾ����ϢԪ�غ�������������ͨ����Ӧ��DI����DIֵ�����㷨��
			std::map<T_DIFunInf, T_DIChannelValType>::iterator itDi;
			for ( itDi=m_mapDIChannel.begin(); itDi!=m_mapDIChannel.end(); itDi++ )
			{
				OpcUa_Byte byVal = 0;
				T_DIFunInf funInf = itDi->first;
				T_DIChannelValType chnVal = itDi->second;

				if ( chnVal.nChgPos==chnVal.nOrgPos )
				{
					byVal = chnVal.nOrgVal;
				}
				else
				{
					if ( i<chnVal.nChgPos && i>=chnVal.nOrgPos )
					{
						byVal = chnVal.nOrgVal;
					}
					else
					{
						byVal = chnVal.nChgVal;
					}
				}

				ACE_OS::sprintf( chMsg, "%s,%d", chMsg, byVal );
			}
			strLine = chMsg;

			fWrite<<strLine<<std::endl;
		}

		fWrite.close();
		return 0;
	}

	//! �õ�ָ�����������ŵ�״̬��λ����Ϣ
	OpcUa_Int32 CPRDFile103Dev::GetCtdChgInfo( OpcUa_Int16 nFun, OpcUa_Int16 nInf, T_CtdChgInfo& chgInfo )
	{
		T_DIFunInf ctdFunInf;
		ctdFunInf.nFun = nFun;
		ctdFunInf.nInf = nInf;

		T_mapCtdChgIndexType::iterator it;
		it = m_mapStateInfo.find( ctdFunInf );
		if ( it!=m_mapStateInfo.end() )
		{
			chgInfo = it->second;
			return 0;
		}
		return -1;
	}

	//! �õ�ָ��ʵ��ͨ����ŵ�ͨ����Ϣ
	OpcUa_Int32 CPRDFile103Dev::GetCtdChannelInfo( OpcUa_Int16 nAcc, T_CtdChannelInfo& channelInfo )
	{
		T_mapCtdChannelType::iterator it;
		it = m_mapChannelInfo.find( nAcc );
		if ( it!=m_mapChannelInfo.end() )
		{
			channelInfo = it->second;
			return 0;
		}
		return -1;
	}


	//! ȡ��ĳ��Ŀ¼�ĸ�Ŀ¼��
	std::string CPRDFile103Dev::GetParentDir( std::string sDir )
	{
		OpcUa_CharA pchBuf[256];
		memset( pchBuf, 0, 256 );
		sprintf( pchBuf, "%s", sDir.c_str() );
		OpcUa_CharA* pchTemp = pchBuf + 255;
		while( pchTemp > pchBuf && *pchTemp != '\\' && *pchTemp != '/' )
			pchTemp--;
		*pchTemp = '\0';
		return pchBuf;
	}

	bool CPRDFile103Dev::CreateDir( std::string strDirName )
	{
		if( strDirName == "" )
			return false;

		//���Ŀ¼�Ƿ����
		if( ExistDir( strDirName ) )
			return true;

		//������Ŀ¼
		std::string sParentDir = GetParentDir( strDirName );
		CreateDir( sParentDir );

		//������Ŀ¼
		ACE_OS::mkdir( strDirName.c_str() );
		return true;
	}

	bool CPRDFile103Dev::ExistDir( std::string strDirName )
	{
		//�Ƿ����
		ACE_stat fdata;
		if( ACE_OS::stat( strDirName.c_str(), &fdata ) < 0 )
			return false;
		//!��Ŀ¼
		if ( !(fdata.st_mode&S_IFDIR) )
		{
			return false;
		}			
		return true;
	}

	void CPRDFile103Dev::Parse103PRDCfg(CDriverItemType* pDrvItem, std::string DevName)
	{
		if (pDrvItem)
		{
			CDeviceItemType* pDevItem = pDrvItem->GetDevByName(DevName);
			if (pDevItem)
			{
				OpcUa_Int32 nAnalogCount = pDevItem->GetAnalogCount();
				for (OpcUa_Int32 nIndex = 0; nIndex < nAnalogCount; nIndex++)
				{
					CAnalogItem* pAnalog = pDevItem->GetAnalogAt(nIndex);
					if (pAnalog)
					{
						T_CtdChannelInfo AnalogTemp;
						AnalogTemp.szName = pAnalog->GetChannelName();
						AnalogTemp.szPhase = pAnalog->GetABCN();
						AnalogTemp.szUnits = pAnalog->GetUnit();
						OpcUa_Int16 nACC = pAnalog->GetACC();
						m_mapChannelInfo.insert(T_mapCtdChannelType::value_type(nACC,AnalogTemp));
					}
				}
				OpcUa_Int32 nStateCount = pDevItem->GetStateCount();
				for (OpcUa_Int32 nIndex = 0; nIndex < nStateCount; nIndex++)
				{
					CStateItem* pState = pDevItem->GetStateAt(nIndex);
					if (pState)
					{
						T_CtdChgInfo chgInf;
						chgInf.szName = pState->GetDIName();			//ͨ������								 
						chgInf.szLineNo  = pState->GetLineNo();		//��·��ʶ

						T_DIFunInf nFunInf;
						nFunInf.nFun = pState->GetFUN();
						nFunInf.nInf = pState->GetINF();
						m_mapStateInfo.insert( T_mapCtdChgIndexType::value_type(nFunInf,chgInf) );
					}
				}
			}
		}
	}
}
