/*!
 *	\file	FepModbusDevice.cpp
 *
 *	\brief	ͨ��Modbus�豸��ʵ��Դ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	$Header: ?/MACS-SCADA 3.2 �������/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusDevice.cpp, 470+1 2011/04/22 01:56:47 xuhongtao $
 *	$Author: xuhongtao $
 *	$Date: 2011/04/22 01:56:47 $
 *	$Revision: 470+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "FepModbusDevice.h"
#include "FepModbusDriver.h"
#include "FepModbusPrtcl.h"
//#include "RTDBFrameAccess/DBFrameAccess.h"
//#include "HlyEventBase/EventCategoryConstant.h"

namespace MACS_SCADA_SUD
{
	std::string CFepModbusDevice::s_ClassName("CFepModbusDevice");		//! ����

	std::string CFepModbusDevice::GetClassName()
	{
		return s_ClassName;
	}

	CFepModbusDevice::CFepModbusDevice()
	{
		m_byTxTimes = 0;
		m_byRxTimes = 0;

		m_bFlag = true;

		//! ��ǰӦ���͵���ѯ֡��ţ���ʼΪ0
		m_shCurrReq = 0;

		m_bNeedCheckTime = false;

		m_nTrySendCount = 0;

		m_nValidReqCount = 0;  //! ��Ч��ѵ����֡����

		m_bSOESended = false;

		m_nSlaveTimeOuts = 0;	//! ��ʼ���ӻ��ϸ��豸�ĳ�ʱ����Ϊ0

		//! ������ѯ֡���
		m_nSpecialReqCount = 0;	 //! ������ѯ֡����
		m_nCurSpecialReq = 0;	 //! ��ǰ��������ţ�0��m_nSpecialReqCount��1

		m_bRetryPeriod = true;

		//! λ����
		m_wdBitMask[0] = 0x0001;
		m_wdBitMask[1] = 0x0003;
		m_wdBitMask[2] = 0x0007;
		m_wdBitMask[3] = 0x000F;
		m_wdBitMask[4] = 0x001F;
		m_wdBitMask[5] = 0x003F;
		m_wdBitMask[6] = 0x007F;
		m_wdBitMask[7] = 0x00FF;
		m_wdBitMask[8] = 0x01FF;
		m_wdBitMask[9] = 0x03FF;
		m_wdBitMask[10] = 0x07FF;
		m_wdBitMask[11] = 0x0FFF;
		m_wdBitMask[12] = 0x1FFF;
		m_wdBitMask[13] = 0x3FFF;
		m_wdBitMask[14] = 0x7FFF;
		m_wdBitMask[15] = 0xFFFF;			
		m_iCurrSettingGroup = 0;

		//!���豸Port���ý��г�ʼ��
		m_PortCfg.bEnable = 1;
		m_PortCfg.bExceptionAsOnline = true;
		m_PortCfg.nCRCOrder = 0;
		m_PortCfg.nRequestCount = 0;
		m_PortCfg.nSettingGroupCount = 0;
		m_PortCfg.nSettingGroupCycl = 500;
		m_PortCfg.nSndDelay = 100;
		m_PortCfg.nTimeOut = 500;
	}

	CFepModbusDevice::~CFepModbusDevice(void)
	{
		std::map<int,struCtrlFollowReq*>::iterator it;
		for ( it=m_mapCtrlFollowReq.begin(); it!=m_mapCtrlFollowReq.end();it++)
		{
			struCtrlFollowReq* struCtrl = it->second;
			if ( struCtrl )
			{
				delete struCtrl;
				struCtrl = NULL;
			}
		}

		std::map<int,Byte*>::iterator itReg;
		for ( itReg=m_mapRegister.begin(); itReg!=m_mapRegister.end();itReg++)
		{
			Byte* pbyVal = itReg->second;
			if ( pbyVal )
			{
				delete []pbyVal;
				pbyVal = NULL;
			}
		}
		vecSettingGroup.clear();
		m_mapRegister.clear();
		m_mapCtrlFollowReq.clear();
	}	

	/*!
	*	\brief	��ʼ��
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	int CFepModbusDevice::Init()
	{
		int ret = CIODeviceBase::Init();

		InitNeedRegister();

		if (ret != 0)
		{
			return ret;
		}

		//! ��ȡ�豸�����ļ�·��
		if ( !m_pIOUnitCfg->GetUnitCfgName().empty())
		{
			//! ��ȡ�����ļ�����·��
			m_strCfgFileName = CIODriver::GetFEPRunDir() + m_pIOUnitCfg->GetUnitCfgName();
		}

		//! ��ȡ�豸������Ϣ
		InitCfg();

		if( m_nDevType == DEVTYPE_TEPIF)
			m_nSpecialReqCount = 1;

		return 0;
	}

	/*!
	*	\brief	������Ҫ�ļĴ����ռ䣨��Ҫ��ֻȡ�Ĵ�����ĳλ��DI��AI����Ҫ��
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	int CFepModbusDevice::InitNeedRegister()
	{		
		//! MACSMB_DOB
		InitNeedRegByUnitType(MACSMB_DOB, 100);

		//! MACSMB_DOD
		InitNeedRegByUnitType(MACSMB_DOD, 100);

		//! MACSMB_AOD
		InitNeedRegByUnitType(MACSMB_AOD, 10000);

		//! MACSMB_AOE
		InitNeedRegByUnitType(MACSMB_AOE, 10000);

		return 0;
	}

	/*!
	*	\brief	��¼��Ҫ�ļĴ�������Ҫ��ֻȡ�Ĵ�����ĳλ��DI��AI����Ҫ��
	*
	*	\param	iUnitType		UNITTYPE
	*	\param	iDivisor		����
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	int CFepModbusDevice::InitNeedRegByUnitType(int iUnitType, int iDivisor)
	{
		if(iDivisor == 0)
			return -1;
		CIOCfgUnitType* pUnitType = GetIOCfgUnit()->GetUnitTypeByID(iUnitType);
		if ( !pUnitType )
		{
			return 0;
		}
		for ( int i=0; i < pUnitType->GetTagCount(); i++)
		{
			int iTagAddr = pUnitType->GetTagByIndex(i)->GetFinalAddr();
			int iRegAddr   = iTagAddr/iDivisor;
			Byte* pbyVal = new Byte[2];
			memset( pbyVal, 0, 2 );
			std::pair<std::map<int,Byte*>::iterator,bool> ret = 
				m_mapRegister.insert( std::pair<int,Byte*>(iRegAddr,pbyVal) );
			if ( ret.second == false )
			{
				delete []pbyVal;
			}
		}

		return 0;
	}

	/*!
	*	\brief	���챾����·����ʱ�ı�������֡(����ң��)
	*
	*	\param	pBuf		֡�׵�ַ
	*
	*	\retval	int	֡����
	*/
	int CFepModbusDevice::BuildLocalNormalFrame( Byte* pBuf )
	{
		if ( !pBuf )
		{
			return 0;
		}

		int nRet = 0;

		//���������豸�������·�����ڲ����豸����,�������ߵ��豸��Ҫ���ô����ڵ���ѯ���ƣ�����豸����ȵ������ڵ�ʱ���ܷ������֡
		//ע�⣬�����·ֻ��һ���豸����IsOffLine��ʱIsLinkOffline�ض�Ҳ�棬������������ض��������㣬�Ӷ�ÿ�ζ�����֡�ɹ���
		CFepModbusDriver* pDrv = dynamic_cast<CFepModbusDriver*>(GetDriver());
		if ( pDrv && IsOffLine() && !pDrv->IsLinkOffline() && ( pDrv->m_nMaxRetryPeriod > 0 && !m_bRetryPeriod ) )
		{
			return 0;
		}
		
		m_bRetryPeriod = false;

		nRet = BuildCheckTimeFrame( pBuf );
		if ( nRet > 0 )		//! �����Уʱ֡���򷵻�֡��
		{
			CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( m_pDriver );
			char pch[256];
			sprintf( pch, "BuildCheckTimeFrame, Frame Len = %d!", nRet );
			if( GetDevType() == DEVTYPE_855 )
				pDriver->WriteLogAnyWay( pch );
			else
				pDriver->WriteLog( pch );

			return nRet;
		}
			

		//! ������Ҫ��ÿһ����ѯ���ڽ������������ѯ֡����SOE�ȣ�
		nRet = BuildSpecialReqFrame( pBuf );
		if ( nRet > 0 )		//! ����������ѯ֡���򷵻�֡��
			return nRet;

		nRet = BuildReadSettingGroupFrame( pBuf );
		if ( nRet > 0 )		//! ����˶�ȡ��ֵ��֡���򷵻�֡��
			return nRet;

		//! ���͵�ǰ��ѯ֡
		return BuildQueryFrame( pBuf );	
	}

	/*!
	*	\brief	�������֡
	*
	*	\param	pBuf		֡�׵�ַ
	*
	*	\retval	int	֡����
	*/
	int CFepModbusDevice::BuildTestFrame( Byte* pBuf )
	{
		if ( !pBuf )
		{
			return 0;
		}

		int nReturnLen = 0;
		if( m_bTestReq )
		{
			nReturnLen = sizeof( m_reqTest );
			memcpy( pBuf, (Byte*)&m_reqTest, nReturnLen );
		}
		return nReturnLen;
	}

	/*!
	*	\brief	��Уʱ֡���߼�ʵ��
	*
	*	\param	buf		֡�׵�ַ
	*
	*	\retval	int		֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildCheckTimeFrame( Byte* pbyBuf )
	{
		if ( !pbyBuf )
		{
			return 0;
		}

		if ( HasCheckTime() && NeedCheckTime() )	//! ��ҪУʱ
		{
			SetNeedCheckTime(false);
			switch( GetDevType() )
			{
			case DEVTYPE_TEPIF:
			case DEVTYPE_TATSIA:
				return BuildTITANSCheckTimeFrame( pbyBuf );
			case DEVTYPE_NEXUS1272:
				return BuildNEXUSCheckTimeFrame( pbyBuf );
			case DEVTYPE_DBMLEsPL:
				return BuildDBMLEsPLCheckTimeFrame( pbyBuf );
			case DEVTYPE_F650:
				return BuildF650CheckTimeFrame( pbyBuf );
			case DEVTYPE_L30:
				return BuildL30CheckTimeFrame( pbyBuf );
			case DEVTYPE_855:
				return Build855CheckTimeFrame(pbyBuf);		//! �����855Э���豸����855Уʱ֡ Add by: Wangyanchao
			case DEVTYPE_DQPLC:
				return BuildDQPLCCheckTimeFrame( pbyBuf );
			case DEVTYPE_DLLSJZLP:
				return BuildDLLSJZLPCheckTimeFrame( pbyBuf );
			case DEVTYPE_ZSZD:
				return BuildZSZDCheckTimeFrame( pbyBuf );
			case DEVTYPE_LDWKQ:
				return BuildLDCheckTimeFrame( pbyBuf );
			case DEVTYPE_SEPCOS:
				return BuildSEPCOSCheckTimeFrame( pbyBuf );
			case DEVTYPE_DL_SIEMENSS73:
				return BuildDLSIEMENSS73CheckTimeFrame( pbyBuf );
			case DEVTYPE_DL_UMLESSC:
				return BuildDLUMLESSCCheckTimeFrame( pbyBuf );
			case DEVTYPE_DL_SIEMENSS72:
				return BuildDLSIEMENSS72CheckTimeFrame( pbyBuf );
			case DEVTYPE_SF_CSC268:
				return BuildSFCSC268CheckTimeFrame(pbyBuf);
			default:
				return BuildDevCheckTimeFrame( pbyBuf );
			}
		}

		return 0;
	}

	/*!
	*	\brief	���豸Уʱ֡����
	*
	*	\param	buf		֡�׵�ַ
	*
	*	\retval	int		֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildDevCheckTimeFrame( Byte* buf )
	{
		return 0;
	}

	/*!
	*	\brief	���豸��ص�������ѯ֡����SOE��
	*
	*	\param	buf		֡�׵�ַ
	*
	*	\retval	int		֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildSpecialReqFrame( Byte* pbyBuf )
	{
		int iRet = 0;
		if ( !pbyBuf )
		{
			return iRet;
		}
		if ( HasSpecialReq()		//! �������ѯ֡
			&& GetCurrReq() == 0	//! �����ѯ֡�������ļ���д�õģ���ѯ��һ������
			&& m_nCurSpecialReq < m_nSpecialReqCount	//! ��Ҫ���������ѯ֡
			)
		{		
			m_nCurSpecialReq++;
			switch( GetDevType() )
			{
			case DEVTYPE_TEPIF:				
				if ( HasSOE() )
				{
					iRet = BuildTEPIFReadWarnFrame( pbyBuf );
				}	
				break;	
			default:
				iRet = BuildDevSplReqFrame( pbyBuf, m_nCurSpecialReq );
				break;
			}
		}
		else
		{
			m_nCurSpecialReq = 0;
		}

		return iRet;
	}

	/*!
	*	\brief	������豸��ص�������ѯ֡
	*
	*	\param	buf		֡�׵�ַ
	*	\param	nIndex	����֡��ţ������ڼ��������ѯ֡��
	*
	*	\retval	int		֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildDevSplReqFrame( Byte* buf, int nIndex )
	{
		return 0;
	}

	/*!
	*	\brief	д�ļ���¼֡(Ŀǰֻ֧��д�����ļ�����)
	*
	*	\param	byBuf			֡�׵�ַ
	*	\param	strFileReq		�ļ���¼����
	*	\param	pRegVal			��¼����
	*	\param	bIsBroadcast	�Ƿ�Ϊ�㲥
	*
	*	\retval	int		֡��
	*/
	int CFepModbusDevice::BuildWriteRef( Byte* byBuf, 
		FILEREQ strFileReq, const Byte* pbyRegVal, bool bIsBroadcast)
	{
		if ( !byBuf || !pbyRegVal )
		{
			return 0;
		}

		int nFrameLen = 10 + 2*strFileReq.wdRegCount + 2;

		int i = 0;
		if ( bIsBroadcast )
			byBuf[i++] = 0;
		else
			byBuf[i++] = (Byte)GetAddr();

		byBuf[i++] = WRITE_GENERAL_REFERENCE;
		byBuf[i++] = nFrameLen - 5;
		byBuf[i++] = 6;
		byBuf[i++] = HIBYTE( strFileReq.wdFileNo );
		byBuf[i++] = LOBYTE( strFileReq.wdFileNo );
		byBuf[i++] = HIBYTE( strFileReq.wdRegAddr );
		byBuf[i++] = LOBYTE( strFileReq.wdRegAddr );
		byBuf[i++] = HIBYTE( strFileReq.wdRegCount );
		byBuf[i++] = LOBYTE( strFileReq.wdRegCount );
		for ( int j = 0; j < strFileReq.wdRegCount; j++ )
		{
			byBuf[i++] = pbyRegVal[2*j];
			byBuf[i++] = pbyRegVal[2*j+1];
		}

		WORD wCrc = CRC( byBuf, nFrameLen - 2 );

		//���CRC�ߵ�˳��
		if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
		{
			byBuf[i++] = LOBYTE(wCrc);
			byBuf[i++] = HIBYTE(wCrc);
		}
		else
		{
			byBuf[i++] = HIBYTE(wCrc);
			byBuf[i++] = LOBYTE(wCrc);
		}

		return nFrameLen;
	}

	/*!
	*	\brief	���ļ���¼֡(Ŀǰֻ֧�ֶ������ļ�����)
	*
	*	\param	byBuf			֡�׵�ַ
	*	\param	strFileReq		�ļ���¼����
	*
	*	\retval	int		֡��
	*/
	int CFepModbusDevice::BuildReadRef( Byte* byBuf, FILEREQ strFileReq)
	{
		if ( !byBuf )
		{
			return 0;
		}

		int nFrameLen = 12;

		int i = 0;

		byBuf[i++] = (Byte)GetAddr();
		byBuf[i++] = READ_GENERAL_REFERENCE;
		byBuf[i++] = nFrameLen - 5;
		byBuf[i++] = 6;
		byBuf[i++] = HIBYTE( strFileReq.wdFileNo );
		byBuf[i++] = LOBYTE( strFileReq.wdFileNo );
		byBuf[i++] = HIBYTE( strFileReq.wdRegAddr );
		byBuf[i++] = LOBYTE( strFileReq.wdRegAddr );
		byBuf[i++] = HIBYTE( strFileReq.wdRegCount );
		byBuf[i++] = LOBYTE( strFileReq.wdRegCount );

		WORD wCrc = CRC( byBuf, nFrameLen - 2 );

		//���CRC�ߵ�˳��
		if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
		{
			byBuf[i++] = LOBYTE(wCrc);
			byBuf[i++] = HIBYTE(wCrc);
		}
		else
		{
			byBuf[i++] = HIBYTE(wCrc);
			byBuf[i++] = LOBYTE(wCrc);
		}

		return nFrameLen;
	}

	/*!
	*	\brief	��TITANS��������ֱ������Уʱ֡
	*
	*	\param	buf		֡�׵�ַ
	*
	*	\retval	int		֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildTITANSCheckTimeFrame( Byte* buf )
	{
		if ( !buf )
		{
			return 0;
		}

		if ( m_nCheckTimeAddr < 400001 || m_nCheckTimeAddr > 465536 )
		{
			return 0;
		}

		int FrameLen = sizeof(TITANSCHECKTIMEREQUEST);

		//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //�����Ĵ���ʱ��,����
		ACE_Time_Value val = ACE_OS::gettimeofday();
		ACE_Date_Time dateTime;
		dateTime.update( val );

		TITANSCHECKTIMEREQUEST* pReq = (TITANSCHECKTIMEREQUEST* )buf;
		pReq->SlaveAddress = (Byte)GetAddr();
		pReq->Function = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;		//! 10H
		pReq->StartHi = HIBYTE(m_nCheckTimeAddr - 400001);		//! 00H
		pReq->StartLo = LOBYTE(m_nCheckTimeAddr - 400001);		//! 78H
		pReq->PointHi = 0x00;		//! 00H
		pReq->PointLo = 0x07;		//! 07H
		pReq->ByteCount = 0x0E;		//! 0EH
		pReq->YearHiHi = HIBYTE((WORD)(dateTime.year()/100));	//! ����ָ�λ
		pReq->YearHiLo = LOBYTE((WORD)(dateTime.year()/100));	//! ����ֵ�λ
		pReq->YearLoHi = HIBYTE((WORD)(dateTime.year()%100));	//! ����ָ�λ
		pReq->YearLoLo = LOBYTE((WORD)(dateTime.year()%100));	//! ����ֵ�λ

		pReq->MonthHi = HIBYTE((WORD)(dateTime.month()));		//! ��
		pReq->MonthLo = LOBYTE((WORD)(dateTime.month()));		//! ��
		pReq->DayHi = HIBYTE((WORD)(dateTime.day()));			//! ��
		pReq->DayLo = LOBYTE((WORD)(dateTime.day()));			//! ��
		pReq->HourHi = HIBYTE((WORD)(dateTime.hour()));			//! ʱ
		pReq->HourLo = LOBYTE((WORD)(dateTime.hour()));			//! ʱ
		pReq->MintueHi = HIBYTE((WORD)(dateTime.minute()));		//! ��
		pReq->MintueLo = LOBYTE((WORD)(dateTime.minute()));		//! ��
		pReq->SecondHi = HIBYTE((WORD)(dateTime.second()));		//! ��
		pReq->SecondLo = LOBYTE((WORD)(dateTime.second()));		//! ��	

		WORD wCrc = CRC( (Byte *)pReq, FrameLen - 2 );
		if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
		{
			pReq->CRCFirst = LOBYTE(wCrc);
			pReq->CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			pReq->CRCFirst = HIBYTE(wCrc);
			pReq->CRCSecond = LOBYTE(wCrc);
		}
		
		return FrameLen;
	}

	/*!
	*	\brief	��855Э���Уʱ֡ Add by: Wangyanchao
	*
	*	\param	buf		֡�׵�ַ
	*
	*	\retval	OpcUa_Int		֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::Build855CheckTimeFrame( Byte* buf )
	{
		if ( !buf )
		{
			CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( m_pDriver );
			char pch[256];
			sprintf( pch, "Build855CheckTimeFrame, m_nCheckTimeAddr = %d!,,valid Range 400001-465536", m_nCheckTimeAddr );
			pDriver->WriteLogAnyWay( pch );	
			return 0;
		}

		if ( m_nCheckTimeAddr < 400001 || m_nCheckTimeAddr > 465536 )
		{
			return 0;
		}

		int FrameLen = sizeof(C855CHECKTIMEREQUEST);

		//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //�����Ĵ���ʱ��,����
		ACE_Time_Value val = ACE_OS::gettimeofday();
		ACE_Date_Time dateTime;
		dateTime.update( val );

		C855CHECKTIMEREQUEST* pReq = (C855CHECKTIMEREQUEST* )buf;
		pReq->SlaveAddress = (Byte)GetAddr();
		pReq->Function = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;		//! 10H
		pReq->StartHi = HIBYTE(m_nCheckTimeAddr - 400001);		//! ��ʼλ���ֽ�
		pReq->StartLo = LOBYTE(m_nCheckTimeAddr - 400001);		//! ��ʼλ���ֽ�
		pReq->PointHi = 0x00;		//! �������ֽ�
		pReq->PointLo = 0x06;		//! �������ֽ�
		pReq->ByteCount = 0x0C;		//! �ֽڸ���
		pReq->YearHi = HIBYTE((WORD)(dateTime.year()));	//! ����ָ�λ
		pReq->YearLo = LOBYTE((WORD)(dateTime.year()));	//! ����ֵ�λ

		pReq->MonthHi = HIBYTE((WORD)(dateTime.month()));		//! ��
		pReq->MonthLo = LOBYTE((WORD)(dateTime.month()));		//! ��
		pReq->DayHi = HIBYTE((WORD)(dateTime.day()));			//! ��
		pReq->DayLo = LOBYTE((WORD)(dateTime.day()));			//! ��
		pReq->HourHi = HIBYTE((WORD)(dateTime.hour()));			//! ʱ
		pReq->HourLo = LOBYTE((WORD)(dateTime.hour()));			//! ʱ
		pReq->MintueHi = HIBYTE((WORD)(dateTime.minute()));		//! ��
		pReq->MintueLo = LOBYTE((WORD)(dateTime.minute()));		//! ��
		pReq->SecondHi = HIBYTE((WORD)(dateTime.second()));		//! ��
		pReq->SecondLo = LOBYTE((WORD)(dateTime.second()));		//! ��	

		WORD wCrc = CRC( (Byte *)pReq, FrameLen - 2 );
		if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
		{
			pReq->CRCFirst = LOBYTE(wCrc);
			pReq->CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			pReq->CRCFirst = HIBYTE(wCrc);
			pReq->CRCSecond = LOBYTE(wCrc);
		}

		return FrameLen;
	}

	
	int CFepModbusDevice::BuildNEXUSCheckTimeFrame( Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		if ( m_nCheckTimeAddr < 400001 || m_nCheckTimeAddr > 465536 )
		{
			return 0;
		}

		int FrameLen = sizeof(NEXUSCHECKTIMEREQUEST);

		//int msOffset = FrameLen*(1+8+1)*1000/58400 + 1;	//�����Ĵ���ʱ��, ����.
		//SYSTEMTIME sTime = OffsetSystemTime( msOffset );

		ACE_Time_Value val = ACE_OS::gettimeofday();
		ACE_Date_Time dateTime;
		dateTime.update( val );

		NEXUSCHECKTIMEREQUEST* pReq = (NEXUSCHECKTIMEREQUEST* )buf;
		pReq->SlaveAddress		= m_bBroadcast?0:(Byte)GetAddr();
		pReq->Function			= FORCE_ANA_MULTIPLE_OUTPUT_STATUS;
		pReq->StartHi			= HIBYTE(m_nCheckTimeAddr - 400001);
		pReq->StartLo			= LOBYTE(m_nCheckTimeAddr - 400001);
		pReq->PointHi			= 0x00;
		pReq->PointLo			= 0x05;
		pReq->ByteCount			= 0x0A;

		pReq->Century			= Byte(dateTime.year()/100);
		pReq->Year				= Byte(dateTime.year()%100);
		pReq->Month				= (Byte)dateTime.month();
		pReq->Day				= (Byte)dateTime.day();
		pReq->Hour				= (Byte)dateTime.hour();
		pReq->Mintue			= (Byte)dateTime.minute();
		pReq->Second			= (Byte)dateTime.second();
		pReq->TenMillisecond	= (Byte)(dateTime.microsec()/10000);
		pReq->WeekHi			= HIBYTE(dateTime.weekday()?dateTime.weekday():7);
		pReq->WeekLo			= LOBYTE(dateTime.weekday()?dateTime.weekday():7);

		WORD wCrc = CRC( (Byte *)pReq, FrameLen - 2 );
		//���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			pReq->CRCFirst  = LOBYTE(wCrc);
			pReq->CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			pReq->CRCFirst  = HIBYTE(wCrc);
			pReq->CRCSecond = LOBYTE(wCrc);
		}

		return FrameLen;
	}

	
	int CFepModbusDevice::BuildDBMLEsPLCheckTimeFrame( Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		//if ( m_nCheckTimeAddr < 1530 || m_nCheckTimeAddr > 1540 )
		//{
		//	CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( m_pDriver );
		//	char pchLog[MAXDEBUGARRAYLEN];
		//	//! д��־
		//	sprintf( pchLog,"CFepModbusDevicel::BuildDBMLEsPLCheckTimeFrame �豸%s��Уʱ֡�׵�ַ��Χ���豸ʹ��˵����һ�£����ô���",
		//		GetName().c_str());
		//	pDriver->WriteLogAnyWay( pchLog );
		//	return 0;
		//}

		//���ݱ�׼���ã�400001~465536�� ��ѧ�� 2013.2.18
		if ( m_nCheckTimeAddr >= 400001 || m_nCheckTimeAddr <= 465536 )
		{
			m_nCheckTimeAddr = m_nCheckTimeAddr - 400001;
		}
		if ( m_nCheckTimeAddr < 0 || m_nCheckTimeAddr > 65535 )
		{
			return 0;
		}

		int FrameLen = sizeof(DBMLEsPLCHECKTIMEREQUEST);		

		ACE_Time_Value val = ACE_OS::gettimeofday();
		ACE_Date_Time dateTime;
		dateTime.update( val );

		DBMLEsPLCHECKTIMEREQUEST* pReq = (DBMLEsPLCHECKTIMEREQUEST* )buf;
		pReq->SlaveAddress		= m_bBroadcast?0:(Byte)GetAddr();
		pReq->Function			= FORCE_ANA_MULTIPLE_OUTPUT_STATUS;//!�����룿����
		pReq->StartHi			= HIBYTE(m_nCheckTimeAddr );
		pReq->StartLo			= LOBYTE(m_nCheckTimeAddr );
		pReq->CountHi			= 0x00;
		pReq->CountLo			= 0x04;
		pReq->ByteCount			= 0x08;

		pReq->byRes			    = Dec2Bcd( Byte(dateTime.year()/100) );
		pReq->Year				= Dec2Bcd( Byte(dateTime.year()%100) );
		pReq->Month				= Dec2Bcd( (Byte)dateTime.month() );
		pReq->Day				= Dec2Bcd( (Byte)dateTime.day() );
		pReq->Hour				= Dec2Bcd( (Byte)dateTime.hour() );
		pReq->Mintue			= Dec2Bcd( (Byte)dateTime.minute() );
		pReq->Second			= Dec2Bcd( (Byte)dateTime.second() );
		pReq->TenMicrosecond	= Dec2Bcd( (Byte)(dateTime.microsec()/10000) );		

		WORD wCrc = CRC( (Byte *)pReq, FrameLen - 2 );
		//���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			pReq->CRCFirst  = LOBYTE(wCrc);
			pReq->CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			pReq->CRCFirst  = HIBYTE(wCrc);
			pReq->CRCSecond = LOBYTE(wCrc);
		}

		return FrameLen;
	}
	/*!
	*	\brief	ʮ������ת��Ϊ��Ӧ��BCD��
	*
	*	\param	int x		ʮ������	
	*
	*	\retval	int		��Ӧ��BCD����ֵ
	*
	*	\note		
	*/
	int CFepModbusDevice::Dec2Bcd(int x)
	{
		int iRet=0;
		int i=0;		
		int Res =0;	

		while(x!=0)
		{
			Res=x%10;
			x/=10;			
			iRet|= (Res<<i);
			i+=4;
		}

		return iRet;
	}

	/*!
	*	\brief	����ѯ֡
	*
	*	\param	buf		֡�׵�ַ
	*	\param	buf		�ٻ�֡���	
	*
	*	\retval	int		֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildQueryFrame( Byte* buf )
	{
		if ( !buf || m_nValidReqCount <= 0 )
		{
			return 0;
		}

		int nFrameLen = sizeof(REQUEST);
		memcpy( buf, &Request[m_shCurrReq], nFrameLen );

		m_shCurrReq++;
		if (m_shCurrReq >= m_nValidReqCount)
		{
			m_shCurrReq = 0;
		}

		CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( m_pDriver );

		if ( pDriver->IsLog() )
		{
			char pchLog[MAXDEBUGARRAYLEN];
			CFepModbusPrtcl* pPrtcl = dynamic_cast<CFepModbusPrtcl*>(GetPrtcl());
			std::string szSndPacket =  pPrtcl->GetCommData( buf, nFrameLen, COMMDATA_SND);
			//if (pDriver->IsEnglishEnv())
			//{
			//	snprintf(pchLog, sizeof(pchLog), "CFepModbusDevice::BuildQueryFrame Content:%s Number:%d!", szSndPacket.c_str(), m_shCurrReq);
			//}
			//else
			//{
			//	snprintf( pchLog, sizeof(pchLog), "CFepModbusDevice::BuildQueryFrame ��֡��%s��֡��ţ�%d!", szSndPacket.c_str(), m_shCurrReq);
			//}
			//pDriver->WriteLog( pchLog );
		}
		return nFrameLen;
	}

	/*!
	*	\brief	��TITANSֱ�����ĸ澯֡
	*
	*	\param	buf		֡�׵�ַ
	*
	*	\retval	int		֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildTEPIFReadWarnFrame( Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		int FrameLen = sizeof(TEPIFREADWARN);

		TEPIFREADWARN* pReq = (TEPIFREADWARN* )buf;
		pReq->SlaveAddress = (Byte)GetAddr();
		pReq->Function = READ_ANA_OUTPUT_STATUS;
		pReq->StartHi = 0x00;
		if(m_bFlag)
            pReq->StartLo = TEPIF_SOE_START_ADDR1;
		else
			pReq->StartLo = TEPIF_SOE_START_ADDR2;

		WORD wCount = TEPIF_SOE_POINTCOUNT;
		pReq->CountHi = HIBYTE(wCount);
		pReq->CountLo = LOBYTE(wCount);

		WORD wCrc = CRC( (Byte *)pReq, FrameLen - 2 );
		if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
		{
			pReq->CRCFirst = LOBYTE(wCrc);
			pReq->CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			pReq->CRCFirst = HIBYTE(wCrc);
			pReq->CRCSecond = LOBYTE(wCrc);
		}

		SetFlag();

		return FrameLen;
	}

	
	/*!
	*	\brief	�鴥����ȡ��ֵ�������֡
	*
	*	\param	buf		֡�׵�ַ
	*
	*	\retval	int		֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildReadSettingGroupFrame( Byte* pbyBuf )
	{
		if ( !pbyBuf || !GetSETTINGGROUP() || !ReadSettingGroupPD() )
		{
			return 0;
		}
		SetNeedRdSettingGp(false);
		int nFrameLen = 0;		
		CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( m_pDriver );
		char pchLog[MAXDEBUGARRAYLEN];
		//!��ѯ������ֵ��֡
		if (m_iCurrSettingGroup >= m_PortCfg.nSettingGroupCount)
		{
			m_iCurrSettingGroup = 0;
		}
		else
		{
			//!��鶨ֵ��֡�Ƿ񱻴���
			if (vecSettingGroup[m_iCurrSettingGroup].iReadCount> 0)
			{				
				//!��֡
				vecSettingGroup[m_iCurrSettingGroup].iReadCount--;

				REQUEST Req;
				//�����Ч����
				Req.SlaveAddress = GetAddr();
				Req.Function	 = vecSettingGroup[m_iCurrSettingGroup].Function;
				Req.StartHi		 = HIBYTE( vecSettingGroup[m_iCurrSettingGroup].StartingAddress );
				Req.StartLo		 = LOBYTE( vecSettingGroup[m_iCurrSettingGroup].StartingAddress );
				Req.PointHi		 = HIBYTE( vecSettingGroup[m_iCurrSettingGroup].PointCount );
				Req.PointLo		 = LOBYTE( vecSettingGroup[m_iCurrSettingGroup].PointCount );

				WORD wCrc = CRC( (Byte *)&Req, 6 );

				//���CRC�ߵ�˳��
				if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
				{
					Req.CRCFirst  = LOBYTE(wCrc);
					Req.CRCSecond = HIBYTE(wCrc);
				}
				else
				{
					Req.CRCFirst  = HIBYTE(wCrc);
					Req.CRCSecond = LOBYTE(wCrc);
				}
				nFrameLen = sizeof(REQUEST);
				ACE_OS::memcpy(pbyBuf,&Req,nFrameLen);	
				//if (pDriver->IsEnglishEnv())
				//{
				//	snprintf(pchLog,sizeof(pchLog), "CFepModbusDevicel::BuildReadSettingGroupFrame Device:%s ReadSettingGroup GroupNo:%d Times:%d", 
				//		GetName().c_str(),m_iCurrSettingGroup,vecSettingGroup[m_iCurrSettingGroup].iReadCountCfg - vecSettingGroup[m_iCurrSettingGroup].iReadCount);
				//}
				//else
				//{
				//	snprintf( pchLog, sizeof(pchLog), "CFepModbusDevicel::BuildReadSettingGroupFrame �豸��:%s ������ȡ��ֵ�飬��ֵ����:%d,��%d�� !!",
				//		GetName().c_str(),m_iCurrSettingGroup,vecSettingGroup[m_iCurrSettingGroup].iReadCountCfg-vecSettingGroup[m_iCurrSettingGroup].iReadCount);
				//}
				//pDriver->WriteLogAnyWay( pchLog );
				m_iCurrSettingGroup++;	
			}
			else
			{
				Byte byVal=0;
				SetVal(MACSMB_READSETTINGGROUP,m_iCurrSettingGroup+1,&byVal,1,0,true);
				m_iCurrSettingGroup++;
				nFrameLen = 0;
			}

			//!������ж�ֵ����ѵ֡�����ͣ���Ӧ����MACSMB_READSETTINGGROUPALL
			bool bReSet = false;
			for (int i=0;i<vecSettingGroup.size();i++)
			{
				if ( vecSettingGroup[i].iReadCount >  0)
				{
					bReSet = false;
					break;
				}
				else
				{					
					bReSet = true;
				}				
			}			
			if( bReSet )
			{
				Byte byVal=0;
				SetVal(MACSMB_READSETTINGGROUPALL,1,&byVal,1,0,true);
			}
		}		
	
		return nFrameLen;
	}
	/*!
	*	\brief	DOAң��֡
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң�ر�ǩ��ַ		
	*	\param	byVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildDOAReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		WORD wCmd;

		if(byVal == 1)
		{
			wCmd = 0xFF00;
		}
		else if(byVal == 0)
		{
			wCmd = 0x0000;
		}
		else
		{
			return 0;
		}

		int FrameLen = sizeof(COMMAND);

		COMMAND* pReq = (COMMAND* )buf;
		pReq->SlaveAddress = (Byte)GetAddr();

		pReq->Function = FORCE_DIG_SINGLE_OUTPUT_STATUS;

		
		WORD wCtrl = (WORD)(lTagAddr - 1);	//! ��üĴ�����ַ���˴�Ϊ����λ��
		pReq->StartHi = HIBYTE(wCtrl);
		pReq->StartLo = LOBYTE(wCtrl);
		pReq->PointHi = HIBYTE(wCmd);
		pReq->PointLo = LOBYTE(wCmd);

		WORD wCrc = CRC( (Byte *)pReq, FrameLen - 2 );
		if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
		{
			pReq->CRCFirst = LOBYTE(wCrc);
			pReq->CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			pReq->CRCFirst = HIBYTE(wCrc);
			pReq->CRCSecond = LOBYTE(wCrc);
		}

		return FrameLen;
	}


	/*!
	*	\brief	DOBң��֡
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң�ر�ǩ��ַ		
	*	\param	byVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildDOBReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 8;

		if ( byVal != 0 && byVal != 1 )
		{
			return 0;
		}

		buf[0] = (Byte)GetAddr();
		buf[1] = FORCE_ANA_SINGLE_OUTPUT_STATUS;

		//! ����Ĵ�����ַ
		int lRegAddress = lTagAddr/100 - 1;	//! 25601��ʾ��ַ40256�ĵ�2λ��
		int nBitOff	= lTagAddr%100;				//! 
		if ( nBitOff > 15 )
		{
			return 0;
		}
		
		buf[2] = HIBYTE(lRegAddress);
		buf[3] = LOBYTE(lRegAddress);

		WORD RawValue = 0;		//!< �Ĵ���ԭֵ
		std::map<int, Byte*>::iterator it;
		it = m_mapRegister.find( lRegAddress + 1);
		if ( it != m_mapRegister.end() )
		{
			Byte* pbyVal = it->second;
			if ( !pbyVal )
				return -2;
			RawValue = MAKEWORD(pbyVal[1], pbyVal[0]);
		}
		else
			return -3;

		//! ������ֵ
		WORD NewValue  = (RawValue&(~(((WORD)1)<<nBitOff)))|((byVal)<<nBitOff);
		buf[4] = HIBYTE(NewValue);
		buf[5] = LOBYTE(NewValue);

		//! ����CRC
		WORD wCrc = CRC( buf, 6 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[6] = LOBYTE(wCrc);
			buf[7] = HIBYTE(wCrc);
		}
		else
		{
			buf[6] = HIBYTE(wCrc);
			buf[7] = LOBYTE(wCrc);
		}

		//! ֡��
		return FrameLen;
	}
	/*!
	*	\brief	DOCң��֡
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң�ر�ǩ��ַ		
	*	\param	byVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildDOCReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 10;

		if ( byVal != 0 && byVal != 1 )
		{
			return 0;
		}

		buf[0] = (Byte)GetAddr();
		buf[1] = FORCE_DIG_MULTIPLE_OUTPUT_STATUS;
		buf[2] = HIBYTE(lTagAddr - 1);
		buf[3] = LOBYTE(lTagAddr - 1);
		WORD wNum = 1;			//! ��������Ȧ����
		buf[4] = HIBYTE( wNum );
		buf[5] = LOBYTE( wNum );
		buf[6] = 1;				//! �������ֽڸ���
		buf[7] = (Byte)byVal;	//! ����Ƕ��д,����Ҫ��λ!!!

		//����CRC
		WORD wCrc = CRC( buf, 8 );

		//���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[8]  = LOBYTE(wCrc);
			buf[9]  = HIBYTE(wCrc);
		}
		else
		{
			buf[8]  = HIBYTE(wCrc);
			buf[9]  = LOBYTE(wCrc);
		}

		//֡��
		return FrameLen;
	}

	/*!
	*	\brief	DOFң��֡
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң�ر�ǩ��ַ		
	*	\param	byVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildDOFReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 8;

		if ( byVal != 0 && byVal != 1 )
		{
			return 0;
		}

		buf[0] = (Byte)GetAddr();
		buf[1] = FORCE_ANA_SINGLE_OUTPUT_STATUS;

		//! ����Ĵ�����ַ
		int lRegAddress = lTagAddr/100 - 1;	//! 25601��ʾ��ַ40256�ĵ�2λ��
		int nBitOff	= lTagAddr%100;				//! 
		if ( nBitOff > 15 )
		{
			return 0;
		}

		buf[2] = HIBYTE(lRegAddress);
		buf[3] = LOBYTE(lRegAddress);

		WORD RawValue = 0;		//!< �Ĵ���ԭֵ

		//! ������ֵ
		WORD NewValue  = (RawValue&(~(((WORD)1)<<nBitOff)))|((byVal)<<nBitOff);
		buf[4] = HIBYTE(NewValue);
		buf[5] = LOBYTE(NewValue);

		//! ����CRC
		WORD wCrc = CRC( buf, 6 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[6] = LOBYTE(wCrc);
			buf[7] = HIBYTE(wCrc);
		}
		else
		{
			buf[6] = HIBYTE(wCrc);
			buf[7] = LOBYTE(wCrc);
		}

		//! ֡��
		return FrameLen;
	}


	/*!
	*	\brief	DODң��֡
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң�ر�ǩ��ַ		
	*	\param	byVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildDODReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 11;

		if ( byVal != 0 && byVal != 1 )
		{
			return 0;
		}

		buf[0] = (Byte)GetAddr();
		buf[1] = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;

		//! ����Ĵ�����ַ
		int lRegAddress = lTagAddr/100 - 1;	//! 25601��ʾ��ַ40256�ĵ�2λ��
		int nBitOff	= lTagAddr%100;				//! 
		if ( nBitOff > 15 )
		{
			return 0;
		}

		buf[2] = HIBYTE(lRegAddress);
		buf[3] = LOBYTE(lRegAddress);

		WORD RawValue = 0;		//!< �Ĵ���ԭֵ
		std::map<int, Byte*>::iterator it;
		it = m_mapRegister.find( lRegAddress + 1 );
		if ( it != m_mapRegister.end() )
		{
			Byte* pbyVal = it->second;
			if ( !pbyVal )
				return -2;
			RawValue = MAKEWORD(pbyVal[1], pbyVal[0]);
		}
		else
			return -3;

		//! ������ֵ
		WORD NewValue  = (RawValue&(~(((WORD)1)<<nBitOff)))|((byVal)<<nBitOff);

		WORD wNum = 1;	//�������Ĵ�������
		buf[4] = HIBYTE( wNum );
		buf[5] = LOBYTE( wNum );
		buf[6] = 2;	//�������ֽڸ���
		buf[7] = HIBYTE(NewValue);
		buf[8] = LOBYTE(NewValue);

		//����CRC
		WORD wCrc = CRC( buf, 9 );

		//���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[9]  = LOBYTE(wCrc);
			buf[10] = HIBYTE(wCrc);
		}
		else
		{
			buf[9]  = HIBYTE(wCrc);
			buf[10] = LOBYTE(wCrc);
		}

		//֡��
		return FrameLen;
	}

	/*!
	*	\brief	DOGң��֡
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң�ر�ǩ��ַ		
	*	\param	byVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildDOGReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 11;

		if ( byVal != 0 && byVal != 1 )
		{
			return 0;
		}

		buf[0] = (Byte)GetAddr();
		buf[1] = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;

		//! ����Ĵ�����ַ
		int lRegAddress = lTagAddr/100 - 1;	//! 25601��ʾ��ַ40256�ĵ�2λ��
		int nBitOff	= lTagAddr%100;				//! 
		if ( nBitOff > 15 )
		{
			return 0;
		}

		buf[2] = HIBYTE(lRegAddress);
		buf[3] = LOBYTE(lRegAddress);

		WORD RawValue = 0;		//!< �Ĵ���ԭֵ

		//! ������ֵ
		WORD NewValue  = (RawValue&(~(((WORD)1)<<nBitOff)))|((byVal)<<nBitOff);

		WORD wNum = 1;	//�������Ĵ�������
		buf[4] = HIBYTE( wNum );
		buf[5] = LOBYTE( wNum );
		buf[6] = 2;	//�������ֽڸ���
		buf[7] = HIBYTE(NewValue);
		buf[8] = LOBYTE(NewValue);

		//����CRC
		WORD wCrc = CRC( buf, 9 );

		//���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[9]  = LOBYTE(wCrc);
			buf[10] = HIBYTE(wCrc);
		}
		else
		{
			buf[9]  = HIBYTE(wCrc);
			buf[10] = LOBYTE(wCrc);
		}

		//֡��
		return FrameLen;
	}



	/*!
	*	\brief	DOMң��֡
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң�ر�ǩ��ַ		
	*	\param	byVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildDOMReqFrame( Byte* buf, int lTagAddr, Byte byVal, int nLen)
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}
		int lRegAddr = lTagAddr/1000 - 1;
		int lCoilCount = lTagAddr%1000;
//		int lDataLen = min( lByteCount, nLen );

		//! ֡��
		if ( !buf )
		{
			return 0;
		}

		nLen = 7 + 1 + 2;

		int i = 0;
		buf[i++] = (Byte)GetAddr();
		buf[i++] = FORCE_DIG_MULTIPLE_OUTPUT_STATUS;
		buf[i++] = HIBYTE( lRegAddr );
		buf[i++] = LOBYTE( lRegAddr );
		buf[i++] = HIBYTE( lCoilCount );
		buf[i++] = LOBYTE( lCoilCount );
		buf[i++] = 1;//(lDataLen+7)/8;
		buf[i++] = (Byte)byVal;	//! ����Ƕ��д,����Ҫ��λ!!!

		WORD wCrc = CRC( buf, nLen - 2 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[i++]  = LOBYTE(wCrc);
			buf[i++] = HIBYTE(wCrc);
		}
		else
		{
			buf[i++]  = HIBYTE(wCrc);
			buf[i++] = LOBYTE(wCrc);
		}

		return nLen;
		//return BuildCtrlMultiRegsFrame( buf, lRegAddr, (lDataLen+1)/2, byVal);
	}

	/*!
	*	\brief	��AOAң��֡��������Ϊ6��дһ���Ĵ���
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң����ǩ��ַ		
	*	\param	fVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildAOAReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 8;

		buf[0] = (Byte)GetAddr();
		buf[1] = FORCE_ANA_SINGLE_OUTPUT_STATUS;
		buf[2] = HIBYTE(lTagAddr - 1);
		buf[3] = LOBYTE(lTagAddr - 1);
		buf[4] = HIBYTE((WORD)fVal);
		buf[5] = LOBYTE((WORD)fVal);

		//! ����CRC
		WORD wCrc = CRC( buf, 6 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[6] = LOBYTE(wCrc);
			buf[7] = HIBYTE(wCrc);
		}
		else
		{
			buf[6] = HIBYTE(wCrc);
			buf[7] = LOBYTE(wCrc);
		}

		//! ֡��
		return FrameLen;
	}

	/*!
	*	\brief	��AOBң��֡��������Ϊ16��дһ���Ĵ���
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң����ǩ��ַ		
	*	\param	fVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildAOBReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 11;

		buf[0] = (Byte)GetAddr();
		buf[1] = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;
		buf[2] = HIBYTE(lTagAddr - 1);
		buf[3] = LOBYTE(lTagAddr - 1);
		WORD Num = 1;	//!< �������Ĵ�������
		buf[4] = HIBYTE( Num );
		buf[5] = LOBYTE( Num );
		buf[6] = 2;		//!< �������ֽڸ���

		buf[7] = HIBYTE((WORD)fVal);
		buf[8] = LOBYTE((WORD)fVal);

		//! ����CRC
		WORD wCrc = CRC( buf, 9 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[9]  = LOBYTE(wCrc);
			buf[10] = HIBYTE(wCrc);
		}
		else
		{
			buf[9]  = HIBYTE(wCrc);
			buf[10] = LOBYTE(wCrc);
		}

		//! ֡��
		return FrameLen;
	}

	/*!
	*	\brief	��AODң��֡��������Ϊ6�����üĴ����е�Nλ
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң����ǩ��ַ		
	*	\param	fVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildAODReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 8;

		int lRegAddr = lTagAddr/10000 - 1;
		int lBitOff = (lTagAddr%10000)/100;
		int lBitCount = lTagAddr%100;
		if ( lBitOff < 0 || lBitOff > 15 )
		{
			return -4;
		}
		if ( lBitCount < 1 || lBitCount > 16 )
		{
			return -5;
		}

		int i = 0;
		buf[i++] = (Byte)GetAddr();
		buf[i++] = FORCE_ANA_SINGLE_OUTPUT_STATUS;
		buf[i++] = HIBYTE(lRegAddr);
		buf[i++] = LOBYTE(lRegAddr);

		WORD RawValue = 0;		//!< �Ĵ���ԭֵ
		std::map<int, Byte*>::iterator it;
		it = m_mapRegister.find( lRegAddr + 1);
		if ( it != m_mapRegister.end() )
		{
			Byte* pbyVal = it->second;
			if ( !pbyVal )
				return -2;
			RawValue = MAKEWORD(pbyVal[1], pbyVal[0]);
		}
		else
			return -3;

		//! ������ֵ
		WORD wdVal = ( (((WORD)(fVal) & m_wdBitMask[lBitCount-1])) << lBitOff )
			| ( ( RawValue & m_wdBitMask[lBitOff])
			+ ((( RawValue >> lBitOff) & m_wdBitMask[lBitCount-1]) << lBitOff) );

		buf[i++] = HIBYTE(wdVal);
		buf[i++] = LOBYTE(wdVal);

		//! ����CRC
		WORD wCrc = CRC( buf, 6 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[i++]  = LOBYTE(wCrc);
			buf[i++] = HIBYTE(wCrc);
		}
		else
		{
			buf[i++]  = HIBYTE(wCrc);
			buf[i++] = LOBYTE(wCrc);
		}

		//! ֡��
		return FrameLen;
	}

	/*!
	*	\brief	��AOEң��֡��������Ϊ16�����üĴ����е�Nλ
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң����ǩ��ַ		
	*	\param	fVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildAOEReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 11;

		int lRegAddr = lTagAddr/10000 - 1;	
		int lBitOff = (lTagAddr%10000)/100;
		int lBitCount = lTagAddr%100;
		if ( lBitOff < 0 || lBitOff > 15 )
		{
			return -4;
		}
		if ( lBitCount < 1 || lBitCount > 16 )
		{
			return -5;
		}

		buf[0] = (Byte)GetAddr();
		buf[1] = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;
		buf[2] = HIBYTE(lRegAddr);
		buf[3] = LOBYTE(lRegAddr);
		WORD Num = 1;	//!< �������Ĵ�������
		buf[4] = HIBYTE( Num );
		buf[5] = LOBYTE( Num );
		buf[6] = 2;		//!< �������ֽڸ���

		WORD RawValue = 0;		//!< �Ĵ���ԭֵ
		std::map<int, Byte*>::iterator it;
		it = m_mapRegister.find( lRegAddr + 1);
		if ( it != m_mapRegister.end() )
		{
			Byte* pbyVal = it->second;
			if ( !pbyVal )
				return -2;
			RawValue = MAKEWORD(pbyVal[1], pbyVal[0]);
		}
		else
			return -3;	

		//! ������ֵ
		WORD wdVal = ( (((WORD)(fVal) & m_wdBitMask[lBitCount-1])) << lBitOff )
			| ( ( RawValue & m_wdBitMask[lBitOff])
			+ ((( RawValue >> lBitOff) & m_wdBitMask[lBitCount-1]) << lBitOff) );

		//! �Ĵ����е�ֵ
		buf[7] = HIBYTE(wdVal);
		buf[8] = LOBYTE(wdVal);

		//! ����CRC
		WORD wCrc = CRC( buf, 9 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[9]  = LOBYTE(wCrc);
			buf[10] = HIBYTE(wCrc);
		}
		else
		{
			buf[9]  = HIBYTE(wCrc);
			buf[10] = LOBYTE(wCrc);
		}

		//! ֡��
		return FrameLen;
	}

	/*!
	*	\brief	��AOFң��֡��������Ϊ6�����üĴ����е�Nλ
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң����ǩ��ַ		
	*	\param	fVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildAOFReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 8;

		int lRegAddr = lTagAddr/10000 - 1;
		int lBitOff = (lTagAddr%10000)/100;
		int lBitCount = lTagAddr%100;
		if ( lBitOff < 0 || lBitOff > 15 )
		{
			return -4;
		}
		if ( lBitCount < 1 || lBitCount > 16 )
		{
			return -5;
		}

		int i = 0;
		buf[i++] = (Byte)GetAddr();
		buf[i++] = FORCE_ANA_SINGLE_OUTPUT_STATUS;
		buf[i++] = HIBYTE(lRegAddr);
		buf[i++] = LOBYTE(lRegAddr);

		WORD RawValue = 0;		//!< �Ĵ���ԭֵ

		//! ������ֵ
		WORD wdVal = ( (((WORD)(fVal) & m_wdBitMask[lBitCount-1])) << lBitOff )
			| ( ( RawValue & m_wdBitMask[lBitOff])
			+ ((( RawValue >> lBitOff) & m_wdBitMask[lBitCount-1]) << lBitOff) );

		buf[i++] = HIBYTE(wdVal);
		buf[i++] = LOBYTE(wdVal);

		//! ����CRC
		WORD wCrc = CRC( buf, 6 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[i++]  = LOBYTE(wCrc);
			buf[i++] = HIBYTE(wCrc);
		}
		else
		{
			buf[i++]  = HIBYTE(wCrc);
			buf[i++] = LOBYTE(wCrc);
		}

		//! ֡��
		return FrameLen;
	}

	/*!
	*	\brief	��AOGң��֡��������Ϊ16�����üĴ����е�Nλ
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң����ǩ��ַ		
	*	\param	fVal		ֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildAOGReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 11;

		int lRegAddr = lTagAddr/10000 - 1;	
		int lBitOff = (lTagAddr%10000)/100;
		int lBitCount = lTagAddr%100;
		if ( lBitOff < 0 || lBitOff > 15 )
		{
			return -4;
		}
		if ( lBitCount < 1 || lBitCount > 16 )
		{
			return -5;
		}

		buf[0] = (Byte)GetAddr();
		buf[1] = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;
		buf[2] = HIBYTE(lRegAddr);
		buf[3] = LOBYTE(lRegAddr);
		WORD Num = 1;	//!< �������Ĵ�������
		buf[4] = HIBYTE( Num );
		buf[5] = LOBYTE( Num );
		buf[6] = 2;		//!< �������ֽڸ���

		WORD RawValue = 0;		//!< �Ĵ���ԭֵ

		//! ������ֵ
		WORD wdVal = ( (((WORD)(fVal) & m_wdBitMask[lBitCount-1])) << lBitOff )
			| ( ( RawValue & m_wdBitMask[lBitOff])
			+ ((( RawValue >> lBitOff) & m_wdBitMask[lBitCount-1]) << lBitOff) );

		//! �Ĵ����е�ֵ
		buf[7] = HIBYTE((WORD)fVal);
		buf[8] = LOBYTE((WORD)fVal);

		//! ����CRC
		WORD wCrc = CRC( buf, 9 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			buf[9]  = LOBYTE(wCrc);
			buf[10] = HIBYTE(wCrc);
		}
		else
		{
			buf[9]  = HIBYTE(wCrc);
			buf[10] = LOBYTE(wCrc);
		}

		//! ֡��
		return FrameLen;
	}

	/*!
	*	\brief	��AOC��AOFң��֡
	*
	*	\param	buf			֡�׵�ַ
	*	\param	lTagAddr	ң����ǩ��ַ		
	*	\param	byVal		ֵ
	*	\param	nLen		���ݳ���
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildAOCReqFrame(
		Byte* buf, int lTagAddr, Byte* byVal, int nLen )
	{
		//! У��
		if ( !buf )
		{
			return -1;
		}
		int lRegAddr = lTagAddr/1000 - 1;
		int lByteCount = lTagAddr%1000;
		int lDataLen = min( lByteCount, nLen );

		//! ֡��
		return BuildCtrlMultiRegsFrame( buf, lRegAddr, (lDataLen+1)/2, byVal);
	}

	/*!
	*	\brief	����ң��֡
	*
	*	\param	pbyBuf		֡�׵�ַ
	*	\param	wRegAddr	�Ĵ�����ַ		
	*	\param	wRegNo		�Ĵ�������
	*	\param	wRegVal		��д��Ĵ��������������WORDֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildCtrlMultiRegsFrame( 
		Byte* pbyBuf, const WORD wRegAddr, const WORD wRegNo, const Byte* pbyRegVal)
	{
		if ( !pbyBuf || !pbyRegVal )
		{
			return 0;
		}

		int nLen = 7 + 2*wRegNo + 2;
		CFepModbusPrtcl* pPrtcl = (CFepModbusPrtcl*)GetPrtcl();
		if (pPrtcl && !pPrtcl->IsModbusTCP() && nLen > MB_RTU_MAX_SEND_SIZE)
		{
			WriteLogForMO(nLen);
			return 0;
		}
		else if (pPrtcl && pPrtcl->IsModbusTCP() && nLen > MB_TCP_MAX_SEND_SIZE)
		{
			WriteLogForMO(nLen);
			return 0;
		}

		int i = 0;
		pbyBuf[i++] = (Byte)GetAddr();
		pbyBuf[i++] = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;
		pbyBuf[i++] = HIBYTE( wRegAddr );
		pbyBuf[i++] = LOBYTE( wRegAddr );
		pbyBuf[i++] = HIBYTE( wRegNo );
		pbyBuf[i++] = LOBYTE( wRegNo );
		pbyBuf[i++] = (BYTE)2*wRegNo;

		memcpy( &pbyBuf[i], pbyRegVal, wRegNo*2 );
		i += wRegNo*2;

		WORD wCrc = CRC( pbyBuf, nLen - 2 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			pbyBuf[i++]  = LOBYTE(wCrc);
			pbyBuf[i++] = HIBYTE(wCrc);
		}
		else
		{
			pbyBuf[i++]  = HIBYTE(wCrc);
			pbyBuf[i++] = LOBYTE(wCrc);
		}

		return nLen;
	}

	/*!
	*	\brief	����ң��֡
	*
	*	\param	pbyBuf		֡�׵�ַ
	*	\param	wRegAddr	�Ĵ�����ַ		
	*	\param	wRegNo		�Ĵ�������
	*	\param	wRegVal		��д��Ĵ��������������WORDֵ
	*
	*	\retval	int			֡��
	*
	*	\note		����bufҪԤ�ȷ���ռ�
	*/
	int CFepModbusDevice::BuildCtrlMultiRegsFrame( 
		Byte* pbyBuf, const WORD wRegAddr, const WORD wRegNo, const WORD* wRegVal)
	{
		if ( !pbyBuf || !wRegVal )
		{
			return 0;
		}

		int nLen = 7 + 2*wRegNo + 2;
		CFepModbusPrtcl* pPrtcl = (CFepModbusPrtcl*)GetPrtcl();
		if (pPrtcl && !pPrtcl->IsModbusTCP() && nLen > MB_RTU_MAX_SEND_SIZE)
		{
			WriteLogForMO(nLen);
			return 0;
		}
		else if (pPrtcl && pPrtcl->IsModbusTCP() && nLen > MB_TCP_MAX_SEND_SIZE)
		{
			WriteLogForMO(nLen);
			return 0;
		}

		int i = 0;
		pbyBuf[i++] = (Byte)GetAddr();
		pbyBuf[i++] = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;
		pbyBuf[i++] = HIBYTE( wRegAddr );
		pbyBuf[i++] = LOBYTE( wRegAddr );
		pbyBuf[i++] = HIBYTE( wRegNo );
		pbyBuf[i++] = LOBYTE( wRegNo );
		pbyBuf[i++] = (BYTE)2*wRegNo;
		for ( int j = 0; j < wRegNo; j++ )
		{
			pbyBuf[i++] = HIBYTE( wRegVal[j] );
			pbyBuf[i++] = LOBYTE( wRegVal[j] );
		}

		WORD wCrc = CRC( pbyBuf, nLen - 2 );

		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			pbyBuf[i++]  = LOBYTE(wCrc);
			pbyBuf[i++] = HIBYTE(wCrc);
		}
		else
		{
			pbyBuf[i++]  = HIBYTE(wCrc);
			pbyBuf[i++] = LOBYTE(wCrc);
		}

		return nLen;
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
	WORD CFepModbusDevice::CRC(const Byte * datapt,int bytecount)
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
	*	\brief	��ȡ�豸����
	*
	*	\param			
	*
	*	\retval	int	 0��ʾ�ɹ���������ʾʧ��
	*/
	int CFepModbusDevice::InitCfg()
	{
		//! ����ѡ��������Ϣ
		GetSpecialCfg(m_strCfgFileName);

		//! �˿����������Ϣ
		GetPortCfg(m_strCfgFileName);

		//! �����ѯ֡��������Ϣ
		GetReqCfg(m_strCfgFileName);

		GetSettingGroupCfg(m_strCfgFileName);

		//! ���Ϳ��ƺ�ķ�����������
		GetFollowReqCfg(m_strCfgFileName);

		//! ��ò���֡��������Ϣ
		GetTestCfg(m_strCfgFileName);

		return 0;
	}

	/*!
	*	\brief	·����ʲô����
	*
	*	\param	strPathName	·����
	*
	*	\retval	int	ֵΪDIR__TYPE��FILE_TYPE��NONE_TYPE
	*/
	int CFepModbusDevice::PathType( std::string strPathName )
	{
		//! �ַ���Ϊ��
		if( strPathName.empty() 
			|| strPathName == "."
			|| strPathName == "..")
			return NONE_TYPE;

		if(strPathName[strPathName.length()-1]=='/')
			strPathName[strPathName.length()-1] = '\0';
		//! �Ƿ����
		ACE_stat fdata;
		if( ACE_OS::stat( strPathName.c_str(), &fdata ) < 0 )
			return NONE_TYPE;
		//! �Ƿ���Ŀ¼
		if( fdata.st_mode&S_IFDIR )
			return DIR__TYPE;
		//! �Ƿ���Ŀ¼
		else
			return FILE_TYPE;

		return NONE_TYPE;
	}

	/*!
	*	\brief	����ң�ر�ǩ��ַ��ȡң�ؽ���֡
	*
	*	\param	lTagAddr		ң�ر�ǩ��ַ
	*
	*	\retval	struCtrlFollowReq*	ң�ؽ���֡
	*/ 
	struCtrlFollowReq* CFepModbusDevice::GetCtrlFwReqByTagAddr( int lTagAddr )
	{
		std::map<int, struCtrlFollowReq*>::iterator it;
		it = m_mapCtrlFollowReq.find( lTagAddr );
		if ( it != m_mapCtrlFollowReq.end() )
		{
			struCtrlFollowReq* sReq = it->second;
			if ( !sReq )
			{
				return NULL;
			}
			return sReq;
		}

		return NULL;
	}

	/*!
	*	\brief	����ң����Ӧ֡��������Ӧң�ŵ��ֵ,��bit����
	*
	*	\param	UnitType		�˺���ֻ����MACSMB_DIA��MACSMB_DIB
	*	\param	pbyData			�����׵�ַ
	*	\param	nStartAddr		��ʼ��ǩ��ַ
	*	\param	nPointCount		��ǩ����
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusDevice::UpdateDI( int UnitType, 
		Byte* pbyData, int nStartAddr, int nPointCount, bool bFromLocal )
	{
		CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( m_pDriver );
		char pchLog[MAXDEBUGARRAYLEN];		

		//У��
		if ( UnitType != MACSMB_DIA && UnitType != MACSMB_DIB )
		{
			return -1;
		}

		if ( !pbyData || nPointCount <= 0)
		{
			return -2;
		}

		int nRet = 0;

		Byte* pData = pbyData;
		int ByteCounter = nPointCount/8;	//! �������ֽ���
		int rest = nPointCount % 8;			//! ʣ���λ��
		int k = 0;
		Byte byTemp;
		Byte byOldVal = 0;
		std::string szTagName = "";
		bool bSuc = false;

		while( ByteCounter >= 1 )
		{
			for ( int j = 0; j < 8; j++ )
			{
				byTemp = (*pData>>j)&0x01;				
				Byte byChangVal = byTemp;
				bSuc = IsDIValueChange( UnitType, nStartAddr+1+8*k+j, byOldVal, byChangVal, szTagName );
				if ( bSuc )
				{
					nRet++;
					if ( pDriver )
					{
						sprintf( pchLog, "CFepModbusDevice::UpdateDI Device:%s Tag:%s changes from:%d to:%d",
							GetName().c_str(), szTagName.c_str(), byOldVal, byChangVal );
						pDriver->WriteLogAnyWay(pchLog);
					}
				}
				SetVal( UnitType, nStartAddr+1+8*k+j, &byTemp , 1, 0, bFromLocal );
			}
			k++;
			pData++;
			ByteCounter--;
		}
		//! ���ʣ���λ����Ϊ0
		if( rest != 0 )
		{
			for( int i=0; i<rest; i++ )
			{
				byTemp = (*pData>>i)&0x01;				
				Byte byChangVal = byTemp;
				bSuc = IsDIValueChange( UnitType, nStartAddr+1+8*k+i, byOldVal, byChangVal, szTagName );
				if ( bSuc )
				{
					nRet++;
					if ( pDriver )
					{
						sprintf( pchLog, "CFepModbusDevice::UpdateDI Device:%s Tag:%s changes from:%d to:%d",
							GetName().c_str(), szTagName.c_str(), byOldVal, byChangVal );
						pDriver->WriteLogAnyWay(pchLog);
					}
				}
				SetVal( UnitType, nStartAddr+1+8*k+i, &byTemp , 1, 0, bFromLocal );
			}
		}

		return nRet;
	}

	/*!
	*	\brief	����ң�����Ӧ֡��������ӦAI��(�Ĵ�����Nλ)��ֵ
	*
	*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
	*							��ģ�庯�����ڴ����ǩֵΪ���ֵ�AI��
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	template <bool bHighFirst>
	int CFepModbusDevice::UpdateAIinReg( int UnitType,
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal)
	{
		//У��
		if ( !pbyData || nRegCount <= 0)
			return -1;

		//! ���ݱ�ǩ����̬��Ϣ����
		int iTagAddr,iRegAddr,iBitOff,iBitCount;
		CIOCfgUnitType* pUnitType = GetIOCfgUnit()->GetUnitTypeByID(UnitType);
		if ( !pUnitType )
		{
			return 0;
		}

		Byte* pByte = pbyData;	
		Byte* pbyTagVal = new Byte[sizeof(float)];
		WORD wdRegVal;
		float fTagVal;
		for ( int i=0; i < pUnitType->GetTagCount(); i++)
		{
			iTagAddr = pUnitType->GetTagByIndex(i)->GetFinalAddr();
			iRegAddr = iTagAddr/10000;
			iBitOff = (iTagAddr%10000)/100;
			iBitCount = iTagAddr%100;
			if ( (iRegAddr >= nStartAddr+1) && (iRegAddr <= nStartAddr + nRegCount)
				&& (iBitOff <= 15) && (iBitCount>0) && ((iBitOff+iBitCount) <= 16) )
			{
				pByte = pbyData + (iRegAddr - nStartAddr - 1)*2;
				if ( bHighFirst )
					wdRegVal = MAKEWORD( pByte[1], pByte[0] );
				else
					wdRegVal = MAKEWORD( pByte[0], pByte[1] );
				//! ����AI��ֵ
				fTagVal = (float)(WORD)((wdRegVal>>iBitOff) & m_wdBitMask[iBitCount-1]);
				memcpy( pbyTagVal, &fTagVal, sizeof(float) );
				SetVal( UnitType, iTagAddr, pbyTagVal, sizeof(float), 0,bFromLocal);
			}
		}
		//! �ͷ�
		delete []pbyTagVal;

		return 0;
	}

	/*!
	*	\brief	����������Ҫ�ļĴ�����ֵ
	*
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusDevice::UpdateRegister( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal)
	{
		//! У��
		if ( !pbyData || nRegCount <= 0 )
			return -1;

		std::map<int,Byte*>::iterator itReg;
		for ( itReg=m_mapRegister.begin(); itReg!=m_mapRegister.end();itReg++)
		{
			int iRegAddr = itReg->first;			
			if ( (iRegAddr >= nStartAddr+1) && (iRegAddr <= nStartAddr + nRegCount) )
			{
				Byte* pbyVal = itReg->second;
				if ( pbyVal )
				{
					//! ��ʼ��ַ�ͳ���
					Byte* pbyPos = pbyData + (iRegAddr - nStartAddr - 1)*2;
					memcpy( pbyVal, pbyPos, 2 );
				}
			}
		}

		return 0;
	}

	/*!
	*	\brief	����ң�����Ӧ֡��������ӦAI��(�ַ�������)��ֵ
	*
	*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
	*							��ģ�庯�����ڴ����ǩֵΪ���ֵ�AI��
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	template <bool bHighFirst>
	int CFepModbusDevice::UpdateAIString( int UnitType,
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal)
	{
		//! У��
		if ( !pbyData || nRegCount <= 0 )
			return -1;

		//! ���ݱ�ǩ����̬��Ϣ����
		int iTagAddr,iRegAddr,iByteCount;
		CIOCfgUnitType* pUnitType = GetIOCfgUnit()->GetUnitTypeByID(UnitType);
		if ( !pUnitType )
		{
			return 0;
		}
		for ( int i=0; i < pUnitType->GetTagCount(); i++)
		{
			iTagAddr = pUnitType->GetTagByIndex(i)->GetFinalAddr();		
			iRegAddr = iTagAddr/1000;
			iByteCount = iTagAddr%1000;
			if ( (iRegAddr >= nStartAddr+1) && (iRegAddr <= nStartAddr + nRegCount) )
			{				
				//! ��ʼ��ַ�ͳ���
				Byte* pbyPos = pbyData + (iRegAddr - nStartAddr - 1)*2;
				int nTagLen = min( iByteCount, 2*nRegCount - (pbyPos - pbyData) );
				int nNeedLen = (int)((nTagLen + 1)/2)*2;
				Byte* pByte = new Byte[nNeedLen];
				memset( pByte, 0, nNeedLen );
				memcpy( pByte, pbyPos, nTagLen );
				//! �����λ��ǰ���򽻻�
				if ( !bHighFirst )
				{
					pbyPos = pByte;
					Byte byTemp;
					for( int j=0; j< nNeedLen; j++ )
					{
						byTemp = pbyPos[0];
						pbyPos[0] = pbyPos[1];
						pbyPos[1] = byTemp;
						pbyPos += 2;
					}
				}
				//! ��AI��ֵ
				SetVal( UnitType, iTagAddr, pByte, nTagLen, 0, bFromLocal );
				delete []pByte;
			}
		}

		return 0;
	}

	/*!
	*	\brief	����ң�����Ӧ֡��������ӦAI���ֵ
	*
	*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
	*							��ģ�庯�����ڴ����ǩֵΪ���ֵ�AI��
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	template <bool bSigned, bool bHighFirst>
	int CFepModbusDevice::UpdateWordAI(int UnitType, 
		Byte* pbyData,int nStartAddr, int nRegCount, bool bFromLocal)
	{
		if ( !pbyData || nRegCount <= 0)
			return -1;

		Byte* pByte = pbyData;	
		Byte* pbyTagVal = new Byte[sizeof(float)];

		WORD wdVal;
		//OpcUa_UInt16 nU16Val = 0;
		//OpcUa_Int16 n16Val = 0;

		for ( int i=0; i < nRegCount; i++ )
		{
			int TagAddr = nStartAddr + 1 + i;

			if ( bHighFirst )
				wdVal = MAKEWORD( pByte[1], pByte[0] );
			else
				wdVal = MAKEWORD( pByte[0], pByte[1] );

			//if ( bSigned )
			//{
			//	n16Val = (OpcUa_Int16)wdVal;
			//	memcpy( pbyTagVal, &n16Val, sizeof(OpcUa_Int16) );
			//}
			//else
			//{
			//	nU16Val = (OpcUa_UInt16)wdVal;
			//	memcpy( pbyTagVal, &nU16Val, sizeof(OpcUa_UInt16) );
			//}
			//SetVal( UnitType, TagAddr, pbyTagVal, sizeof(OpcUa_UInt16), 0,bFromLocal);
			pByte += 2;
		}

		delete []pbyTagVal;

		return 0;
	}

	/*!
	*	\brief	����ң����Ӧ֡�������򣬸�����ӦAI���ֵ��
	*			��ģ�庯�����ڴ����ǩֵΪ˫�ֵ�AI��
	*
	*	\param	bSigned			�з��š���true���޷��š���false
	*	\param	bHighFirst		��λ��ǰ����true����λ��ǰ����false
	*	\param	bHiWdFirst		������ǰ����true��������ǰ����false
	*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
	*							��ģ�庯�����ڴ����ǩֵΪ˫�ֵ�AI��
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	template <bool bSigned, bool bHighFirst, bool bHiWdFirst>
	int CFepModbusDevice::UpdateDWordAI(int UnitType, 
		Byte* pbyData,int nStartAddr, int nRegCount, bool bFromLocal)
	{
		if ( !pbyData || nRegCount <= 0)
			return -1;

		Byte* pByte = pbyData;	
		Byte* pbyTagVal = new Byte[sizeof(float)];

		WORD wdVal1,wdVal2;
		DWORD dwdVal;
		float fVal;

		for ( int i=0; i < nRegCount; i++ )
		{
			int TagAddr = nStartAddr + 1 + i;

			if ( bHighFirst )
			{
				wdVal1 = MAKEWORD( pByte[1], pByte[0] );
				wdVal2 = MAKEWORD( pByte[3], pByte[2] );
			}
			else
			{
				wdVal1 = MAKEWORD( pByte[0], pByte[1] );
				wdVal2 = MAKEWORD( pByte[2], pByte[3] );
			}

			if ( bHiWdFirst )
				dwdVal = MAKEDWORD( wdVal2, wdVal1 );
			else
				dwdVal = MAKEDWORD( wdVal1, wdVal2 );			

			//! ���ݲ�ͬ���������͸�ֵ
			CIOCfgTag* pObjTag = NULL;
			OBJTAG ObjTag;
			ObjTag.byUnitType = UnitType;
			ObjTag.lTagAddr = TagAddr;
			MAP_OBJTAG::iterator it = m_mapObjTag.find( ObjTag.GetIndexAddr() );
			if ( it != m_mapObjTag.end() )
			{
				pObjTag = it->second;
				if ( pObjTag )
				{
					T_mapFieldPoints mapFieldPoints = pObjTag->GetFieldPoints();
					T_mapFieldPoints::iterator it;
					FieldPointType* pFieldPoint;
					for (it = mapFieldPoints.begin(); it != mapFieldPoints.end(); it++)
					{
						pFieldPoint = it->second;
						if (pFieldPoint)
						{
							//UaNodeId dataType = pFieldPoint->dataType();
							//OpcUa_NodeId dtNodeId;
							//dataType.copyTo(&dtNodeId);
							//if (OpcUa_IdentifierType_Numeric == dtNodeId.IdentifierType)
							//{
							//	if (OpcUaId_Float == dtNodeId.Identifier.Numeric)
							//	{
							//		if (bSigned)
							//		{
							//			fVal = (float)(int)dwdVal;
							//		}
							//		else
							//		{
							//			fVal = (float)dwdVal;
							//		}
							//		memcpy(pbyTagVal, &fVal, sizeof(float));
							//		pFieldPoint->setValue(pbyTagVal, sizeof(float), 0);
							//	}
							//	else
							//	{
							//		pFieldPoint->setValue((Byte*)(&dwdVal), sizeof(DWORD), 0);
							//	}
							//}
							//OpcUa_NodeId_Clear(&dtNodeId);
						}
					}
				}
			}
			pByte += 2;
		}

		delete []pbyTagVal;

		return 0;
	}

	/*!
	*	\brief	����ң�����Ӧ֡��������ӦAI���ֵ
	*			�˺������ڴ������ض����ɵ�AI�㣨��������ģ�庯�������˵ģ�
	*
	*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusDevice::UpdateOtherAI( int UnitType, 
		Byte* pbyData,int nStartAddr, int nRegCount, bool bFromLocal )
	{
		if ( !pbyData || nRegCount <= 0)
			return -1;

		Byte* pByte = pbyData;
		int TagAddr;
		Byte pbyVal[4] = {0};
		Byte* pbyTagVal = new Byte[sizeof(float)];
		
		switch( UnitType )
		{
		case MACSMB_AII:
		case MACSMB_AIJ:
			{
				for ( int i=0; i < nRegCount - 1; i++ )
				{
					TagAddr = nStartAddr + 1 + i;	

					pbyVal[0] = pByte[3];
					pbyVal[1] = pByte[2];
					pbyVal[2] = pByte[1];
					pbyVal[3] = pByte[0];
					ACE_OS::memcpy( pbyTagVal, (void*)(float*)pbyVal, sizeof(float) );
					SetVal( UnitType, TagAddr, pbyTagVal, sizeof(float), 0, bFromLocal );

					pByte += 2;
				}
			}
			break;
		case MACSMB_AIK:
		case MACSMB_AIL:
			{
				for ( int i=0; i < nRegCount - 1; i++ )
				{
					TagAddr = nStartAddr + 1 + i;

					ACE_OS::memcpy( pbyTagVal, (void*)(float*)pByte, sizeof(float) );
					SetVal( UnitType, TagAddr, pbyTagVal, sizeof(float), 0, bFromLocal );

					pByte += 2;
				}
			}
			break;
		case MACSMB_AIM:
		case MACSMB_AIN:
			{
				for ( int i=0; i < nRegCount - 3; i++ )
				{
					TagAddr = nStartAddr + 1 + i;	

					//! ��Ӵ���--------------------------

					pByte += 2;
				}
			}
			break;
		case MACSMB_AIO:
		case MACSMB_AIP: 
			{
				for ( int i=0; i < nRegCount - 3; i++ )
				{
					TagAddr = nStartAddr + 1 + i;	

					//! ��Ӵ���--------------------------

					pByte += 2;
				}
			}
			break;
		case MACSMB_AIQ:
		case MACSMB_AIR:
			{
				for ( int i=0; i < nRegCount - 3; i++ )
				{
					TagAddr = nStartAddr + 1 + i;	

					//! ��Ӵ���--------------------------

					pByte += 2;
				}
			}
			break;	
		case MACSMB_AIFF:
			{			
				for ( int i=0; i < nRegCount; i++)
				{
					TagAddr = nStartAddr + 1 + i;				
					float fVal = MAKEWORD(pByte[3],pByte[2])*10000 + MAKEWORD( pByte[1],pByte[0] );						
					ACE_OS::memcpy( pbyTagVal, (Byte*)&fVal, sizeof(float) );
					SetVal( UnitType, TagAddr, pbyTagVal, sizeof(float), 0, bFromLocal );	

					pByte += 2;					
				}
			}
			break;
		case MACSMB_AIU:
			{
				for ( int i=0; i < nRegCount - 1; i++ )
				{
					TagAddr = nStartAddr + 1 + i;	

					pbyVal[0] = pByte[1];
					pbyVal[1] = pByte[0];
					pbyVal[2] = pByte[3];
					pbyVal[3] = pByte[2];
					ACE_OS::memcpy( pbyTagVal, (void*)(float*)pbyVal, sizeof(float) );
					SetVal( UnitType, TagAddr, pbyTagVal, sizeof(float), 0, bFromLocal );

					pByte += 2;
				}
			}
			break;
		case MACSMB_AIV:
			{
				for ( int i=0; i < nRegCount - 1; i++ )
				{
					TagAddr = nStartAddr + 1 + i;	

					pbyVal[0] = pByte[2];
					pbyVal[1] = pByte[3];
					pbyVal[2] = pByte[0];
					pbyVal[3] = pByte[1];
					ACE_OS::memcpy( pbyTagVal, (void*)(float*)pbyVal, sizeof(float) );
					SetVal( UnitType, TagAddr, pbyTagVal, sizeof(float), 0, bFromLocal );

					pByte += 2;
				}
			}
			break;
		default:
			break;
		}

		delete []pbyTagVal;

		return 0;
	}

	/*!
	*	\brief	����Input Register�е����ݣ�������Ϊ4��
	*
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	void
	*/
	int CFepModbusDevice::UpdateInputAI( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		int nRet = 0;
		//! �����MACSMB_DID���͵ı�ǩ
		CIOCfgUnitType* pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DID);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			nRet = UpdateDIinReg<false>(
				MACSMB_DID, pbyData, nStartAddr, nRegCount, bFromLocal );

		//! �����MACSMB_DIF���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIF);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			nRet = UpdateDIinReg<true>(
				MACSMB_DIF, pbyData, nStartAddr, nRegCount, bFromLocal );

		//! �����MACSMB_AIAA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIAA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
		{
			UpdateWordAI<false,false>( 
				MACSMB_AIAA, pbyData, nStartAddr, nRegCount, bFromLocal);
		}

		//! �����MACSMB_AIAB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIAB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<false,true>( 
				MACSMB_AIAB, pbyData, nStartAddr, nRegCount, bFromLocal);						

		//! �����MACSMB_AICA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AICA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<true,false>( 
				MACSMB_AICA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AICB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AICB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<true,true>( 
				MACSMB_AICB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIEA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIEA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,false,false>( 
				MACSMB_AIEA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIEB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIEB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,false>( 
				MACSMB_AIEB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIEC���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIEC);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,false,true>( 
				MACSMB_AIEC, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIED���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIED);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,true>( 
				MACSMB_AIED, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIED���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DWED);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,true>( 
			MACSMB_DWED, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIGA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIGA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,false,false>( 
				MACSMB_AIGA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIGB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIGB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,true,false>( 
				MACSMB_AIGB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIGC���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIGC);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,false,true>( 
				MACSMB_AIGC, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIGD���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIGD);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,true,true>( 
				MACSMB_AIGD, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AII���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AII);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AII, pbyData, nStartAddr, nRegCount, bFromLocal);
		//! �����MACSMB_AIK���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIK);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AIK, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AITA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AISA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIinReg<false>(
				MACSMB_AISA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AITB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AISB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIinReg<true>(
				MACSMB_AISB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIMA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIMA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIString<false>(
				MACSMB_AIMA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIMB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIMB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIString<true>(
				MACSMB_AIMB, pbyData, nStartAddr, nRegCount, bFromLocal);

		return nRet;
	}

	/*!
	*	\brief	����Holding Register�е����ݣ�������Ϊ3��
	*
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	void
	*/
	int CFepModbusDevice::UpdateHoldingAI( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		int nRet = 0;
		//! �����MACSMB_DIE���͵ı�ǩ
		CIOCfgUnitType* pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIE);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			nRet = UpdateDIinReg<false>(
				MACSMB_DIE, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_DIG���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIG);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			nRet = UpdateDIinReg<true>(
				MACSMB_DIG, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIBA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIBA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<false,false>( 
				MACSMB_AIBA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIBB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIBB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<false,true>( 
				MACSMB_AIBB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIDA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIDA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<true,false>( 
				MACSMB_AIDA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIDB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIDB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<true,true>( 
				MACSMB_AIDB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIFA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIFA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,false,false>( 
				MACSMB_AIFA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIFB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIFB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,false>( 
				MACSMB_AIFB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIFC���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIFC);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,false,true>( 
				MACSMB_AIFC, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIFD���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIFD);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,true>( 
				MACSMB_AIFD, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_DWFD���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DWFD);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,true>( 
			MACSMB_DWFD, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIHA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIHA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,false,false>( 
				MACSMB_AIHA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIHB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIHB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,true,false>( 
				MACSMB_AIHB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIHC���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIHC);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,false,true>( 
				MACSMB_AIHC, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AIHD���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIHD);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,true,true>( 
				MACSMB_AIHD, pbyData, nStartAddr, nRegCount, bFromLocal);	

		//! �����MACSMB_AIJ���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIJ);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AIJ, pbyData, nStartAddr, nRegCount, bFromLocal);
		//! �����MACSMB_AIL���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIL);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AIL, pbyData, nStartAddr, nRegCount, bFromLocal);
		//! �����MACSMB_AIU���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIU);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AIU, pbyData, nStartAddr, nRegCount, bFromLocal);
		//!�����MACSMB_AIV���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIV);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AIV, pbyData, nStartAddr, nRegCount, bFromLocal);
		//! �����MACSMB_AITA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AITA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIinReg<false>(
				MACSMB_AITA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AITB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AITB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIinReg<true>(
				MACSMB_AITB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AINA���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AINA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIString<false>(
				MACSMB_AINA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! �����MACSMB_AINB���͵ı�ǩ
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AINB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIString<true>(
				MACSMB_AINB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//!�����MACSMB_AIFF���͵ı�ǩ��ʩ�͵µ���400V����Ӧ�ã�
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIFF);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI(MACSMB_AIFF, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! ����������Ҫ�ļĴ�����ֵ
		if ( m_mapRegister.size() > 0 )
			UpdateRegister( pbyData, nStartAddr, nRegCount, bFromLocal );

		return nRet;
	}

	/*!
	*	\brief	����Holding Register�е����ݸ��µ�ֵ֮ǰ��Ҫ���е����⴦��
	*			��������Ϊ3��
	*	\param	UnitType
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusDevice::BeforeUpdateHoldingAI( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		//! �����豸���ͽ��е����⴦��
		if ( GetDevType() == DEVTYPE_TEPIF 
			&& (nStartAddr == TEPIF_SOE_START_ADDR1 
				|| nStartAddr == TEPIF_SOE_START_ADDR2) 
			&& nRegCount == TEPIF_SOE_POINTCOUNT )
		{
			return UpdateTEPIFSOE( 
				MACSMB_TEPIFSOE, pbyData,nStartAddr, nRegCount, bFromLocal );
		}
		else if ( GetDevType() == DEVTYPE_NEXUS1272
				&&	233 >= nStartAddr + 1
				&&  233 <= nStartAddr + 1 + nRegCount)
		{			
			return Update_PowFactor( 
				MACSMB_PowFactor, pbyData,nStartAddr, nRegCount, bFromLocal );
		}

		return 0;
	}

	/*!
	*	\brief	����Input Register�е����ݸ��µ�ֵ֮ǰ��Ҫ���е����⴦��
	*			��������Ϊ4��
	*	\param	UnitType
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusDevice::BeforeUpdateInputAI( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		return 0;
	}

	/*!
	*	\brief	�����ļ�����������֡������
	*
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusDevice::UpdateFileInfo( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		return 0;
	}

	/*!
	*	\brief	����AI��Ӧ֡�������򣬸������а�����DI���ֵ
	*			��UnitTypeΪMACSMB_DID��MACSMB_DIE��
	*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
	*							�˺���ֻ����MACSMB_DID��MACSMB_DIE
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	template < bool bHighFirst>
	int CFepModbusDevice::UpdateDIinReg( int UnitType,
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal)
	{
		CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( m_pDriver );
		char pchLog[MAXDEBUGARRAYLEN];

		//! У��
		if ( UnitType != MACSMB_DID && UnitType != MACSMB_DIE
			&& UnitType != MACSMB_DIF && UnitType != MACSMB_DIG)
		{
			return -1;
		}
		if ( !pbyData || nRegCount <= 0)
			return -2;

		int nRet = 0;

		//! ���ݱ�ǩ����̬��Ϣ���±�ǩֵ
		int iTagAddr,i;
		Byte* pVal = NULL;
		Byte BitOff;
		int RegAddr = 0;
		Byte RegBitOff = 0;
		Byte byVal;
		Byte byOldVal = 0;
		std::string szTagName = "";
		bool bSuc = false;
		CIOCfgUnitType* pUnitType = GetIOCfgUnit()->GetUnitTypeByID(UnitType);
		if ( !pUnitType )
		{
			return 0;
		}
		for ( i=0; i < pUnitType->GetTagCount(); i++)
		{
			iTagAddr = pUnitType->GetTagByIndex(i)->GetFinalAddr();
			RegAddr   = iTagAddr/100;
			RegBitOff = iTagAddr%100;
			if ( RegAddr >= nStartAddr+1 && RegAddr <= nStartAddr + nRegCount
				&& RegBitOff <= 15 )
			{
				pVal = pbyData + (RegAddr - nStartAddr - 1)*2;
				WORD wdVal;
				if ( bHighFirst )
					wdVal = MAKEWORD( pVal[1], pVal[0] );
				else
					wdVal = MAKEWORD( pVal[0], pVal[1] );

				byVal = (wdVal>>RegBitOff)&0x01;				
				Byte byChangVal = byVal;
				bSuc = IsDIValueChange( UnitType, iTagAddr, byOldVal, byChangVal, szTagName );
				//! �����־
				if ( bSuc )
				{
					nRet++;
					if ( pDriver )
					{
						sprintf( pchLog, "CFepModbusDevice::UpdateDIinReg Device:%s Tag:%s changes from:%d to: %d",
							GetName().c_str(), szTagName.c_str(), byOldVal, byChangVal );
						pDriver->WriteLogAnyWay(pchLog);
					}
				}
				SetVal( UnitType, iTagAddr, &byVal, 1, 0, bFromLocal );
			}
		}

		return nRet;
	}

	/*!
	*	\brief	����AI��Ӧ֡�������򣬸�������2����TINTANSֱ������SOE��ǩֵ
	*			������SOE��UnitTypeΪMACSMB_TEPIFSOE��
	*
	*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
	*							�˺���ֻ����MACSMB_TEPIFSOE
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusDevice::UpdateTEPIFSOE( int UnitType, 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		//! У��
		if ( UnitType != MACSMB_TEPIFSOE )
		{
			return -1;
		}
		if ( !pbyData || nRegCount <= 0)
			return -2;

		//! ���ݲ������򲻴���
		if( nRegCount < 12)
			return -3;

		WORD wFlag = MAKEWORD( pbyData[1], pbyData[0] );
		//! ������Ч���򲻴���
		if(wFlag == 0)
			return 0;

		//! �ñ�ǩֵ������SOE
		WORD Addr1 = MAKEWORD( pbyData[17], pbyData[16] );

		WORD Addr2 = MAKEWORD( pbyData[19], pbyData[18] );
		WORD Addr3 = MAKEWORD( pbyData[21], pbyData[20] );
		int lAddr = Addr1*1000000 + Addr2*1000 + Addr3;		//! �����ǩ��ַ
		WORD RawVal = MAKEWORD( pbyData[23], pbyData[22] );		//! ��ǩֵ

		//! ����ʱ��
		DAILYTIME strTime;
		WORD whYear = MAKEWORD( pbyData[3], pbyData[2] );												
		WORD wlYear = MAKEWORD( pbyData[5], pbyData[4] );
		strTime.lYear = (DWORD)whYear*100 + (DWORD)wlYear;

		strTime.sMonth = MAKEWORD( pbyData[7], pbyData[6] );
		strTime.sDay = MAKEWORD( pbyData[9], pbyData[8] );
		strTime.sHour = MAKEWORD( pbyData[11], pbyData[10] );
		strTime.sMinute = MAKEWORD( pbyData[13], pbyData[12] );
		strTime.sSecond = MAKEWORD( pbyData[15], pbyData[14] );

		long nTime = CaculateMSNum( strTime );

		//! �ñ�����ֵ
		SetVal( MACSMB_TEPIFSOE, lAddr, (Byte*)&RawVal , 2, nTime, true );
		FieldPointType* pIOChannel = GetTagByIndex(MACSMB_TEPIFSOE, lAddr)->GetFirst();
		if (pIOChannel)
		{
			//CDBFrameAccess::PostSOE(pIOChannel, GetDescription(), EXTERNAL_SOE, (int)RawVal, nTime);
		}

		return 0;
	}

	
	/*!
	*	\brief	����AI��Ӧ֡�������� NEXUS1272��ȱ�������
	*			��UnitTypeΪMACSMB_PowFactor��
	*
	*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
	*							�˺���ֻ����MACSMB_PowFactor
	*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
	*	\param	nStartAddr		��ʼ�Ĵ�����ַ
	*	\param	nRegCount		�Ĵ�������
	*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
	*
	*	\retval	int			0��ʾ�ɹ��������ʾʧ��
	*/
	int CFepModbusDevice::Update_PowFactor( int UnitType, 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		//! У��
		if ( UnitType != MACSMB_PowFactor )
		{
			return -1;
		}
		if ( !pbyData || nRegCount <= 0)
			return -2;
		//!pbyTempָ��ָ��������
		Byte* pbyTemp = pbyData+3;
	
		int TagAddr = -1;
		for (int i=0; i < nRegCount; i++)
		{
			TagAddr = nStartAddr + 1 + i;
			if ( TagAddr == 233 )
			{
				WORD RawVal = MAKEWORD( pbyTemp[1], pbyTemp[0] );
				if ( RawVal > 1000 && RawVal <= 2000 )
				{
					RawVal = 2000 - RawVal; 
				}
				else if ( RawVal > 2000 && RawVal <= 3000 )
				{
					RawVal = RawVal - 2000;
				}
				else if ( RawVal > 3000 && RawVal <= 4000 )
				{
					RawVal = 4000 - RawVal;
				}
				
				SetVal( MACSMB_PowFactor, TagAddr, (Byte*)&RawVal,sizeof(RawVal),0, bFromLocal );
			}
			else
			{		
				WORD RawVal = MAKEWORD( pbyTemp[1], pbyTemp[0] );
				SetVal( MACSMB_PowFactor,TagAddr, (Byte*)&RawVal,sizeof(RawVal),0, bFromLocal);
			}
			pbyData += 2;
		}		

		return 0;
	}

	/*!
	*	\brief	��ʱ��ת���ɺ�����
	*
	*	\param	strTime	Ҫת����ʱ��	
	*
	*	\retval	MACS_INT64	��������Ϊ-1���ʾת��ʧ��
	*/
	long CFepModbusDevice::CaculateMSNum( DAILYTIME strTime )
	//long CFepModbusDevice::CaculateMSNum( DAILYTIME strTime )
	{
		if ( strTime.sMonth < 1 || strTime.sMonth > 12
			|| strTime.sDay < 1 || strTime.sDay > 31
			|| strTime.sHour < 0 || strTime.sHour > 24
			|| strTime.sMinute < 0 || strTime.sMinute > 60
			|| strTime.sSecond < 0 || strTime.sSecond > 60	)
		{
			return -1;
		}
		short sMonthDay[12]={31,28,31,30,31,30,31,31,30,31,30,31};
		int lSumDay;
		int i;

		//! ����������
		lSumDay = 
			(int)(strTime.lYear-1970) * 365 +(int)(strTime.lYear-1-1968) / 4;

		for( i = 0; i < strTime.sMonth - 1; i ++ )
			lSumDay += sMonthDay[ i ];

		if ( ( strTime.sMonth > 2 ) && ( ( strTime.lYear - 1968 ) % 4 == 0 ) )
			lSumDay += 1; 

		lSumDay += strTime.sDay -1 ;

		////! �õ������1970.1.1��������
		//long nSec = (long)lSumDay * 24 * 60 * 60
		//	+ (long)strTime.sHour * 60 * 60
		//	+ (long)strTime.sMinute * 60
		//	+ (long)strTime.sSecond
		//	+ (long)strTime.sTimeZone * 60 * 60;

		////! �ܺ�����
		//long nTime = nSec * 1000 + strTime.sMSecond;
		long nSec = (long)lSumDay * 24 * 60 * 60
			+ (long)strTime.sHour * 60 * 60
			+ (long)strTime.sMinute * 60
			+ (long)strTime.sSecond
			+ (long)strTime.sTimeZone * 60 * 60;

		//! �ܺ�����
		//long nTime = nSec * 1000 + strTime.sMSecond;

		return nSec;
	}

	int CFepModbusDevice::BuildSplCtrlFrame(Byte* pbyData,const tIOCtrlRemoteCmd& tCtrlRemoteCmd,float& fCmdVal)
	{
		return 0;
	}
	/*!
	*	\brief	��������豸������Ϣ
	*
	*	\param	file_name		�����ļ�·����
	*
	*	\retval	void
	*/
	void CFepModbusDevice::GetSpecialCfg(std::string file_name)
	{
		m_nDevType = GetPrivateInifileInt("SPECIAL", "DEVICETYPE", 0, file_name.c_str());		
	
		int nVal = GetPrivateInifileInt("SPECIAL", "SOE", 0, file_name.c_str());
		if ( nVal == 1 )
		{
			m_bSOE = true;
		}
		else
		{
			m_bSOE = false;
		}

		nVal = GetPrivateInifileInt("SPECIAL", "CHECKTIME", 0, file_name.c_str());
		if ( nVal == 1 )
		{
			m_bCheckTime = true;
		}
		else
		{
			m_bCheckTime = false;
		}

		//! Уʱ����
		m_nCheckTimeCycl = GetPrivateInifileInt("SPECIAL", "CHECKTIMECYCLE", 60, file_name.c_str());
		if ( m_nCheckTimeCycl < 5 )
		{
			m_nCheckTimeCycl = 5;
		}

		//! Уʱʱ���(Сʱ) Add by: Wangyanchao
		m_nCheckTimeHour = GetPrivateInifileInt("SPECIAL",	"CHECKTIMEHOUR", 1, file_name.c_str());

		//! Уʱʱ���(����) Add by: Wangyanchao
		m_nCheckTimeMin = GetPrivateInifileInt("SPECIAL",	"CHECKTIMEMINUTE", 30, file_name.c_str());

		//! Уʱ��ʼ��ַ
		m_nCheckTimeAddr = GetPrivateInifileInt("SPECIAL", "CHECKTIMEADDR", 400020, file_name.c_str());

		//! ��ʱ�Ƿ�ʹ�ù㲥����
		nVal = GetPrivateInifileInt("SPECIAL", "BROADCAST", 0, file_name.c_str());
		if ( nVal == 1 )
		{
			m_bBroadcast = true;
		}
		else
		{
			//��Ե�ģʽ��ʱʱ����ʱ������СΪ5���ӣ�����Ӱ��������ѯЧ��
			if ( m_nCheckTimeCycl < 5 )
			{
				m_nCheckTimeCycl = 5;
			}
			m_bBroadcast = false;
		}
	}

	/*!
	*	\brief	��ö˿����������Ϣ
	*
	*	\param	file_name		�����ļ�·����
	*
	*	\retval	void
	*/
	void CFepModbusDevice::GetPortCfg(std::string file_name)
	{
		CFepModbusDriver* pDrv = dynamic_cast<CFepModbusDriver*>(GetDriver());
		if (!pDrv)
		{
			return;
		}

		//! ���߼�����
		m_nMaxOfflineCount = pDrv->GetMaxOffLineCount();
		if ( m_nMaxOfflineCount < 1 )
		{
			m_nMaxOfflineCount = 3;
		}

		//! 0---�رն˿�; 1---�����˿ڡ�Ĭ��1��
		int nVal = GetPrivateInifileInt("PORT", "Enable", 1, file_name.c_str());
		if ( nVal == 0 )
		{
			m_PortCfg.bEnable = false;
		}
		else
		{
			m_PortCfg.bEnable = true;
		}

		//! Ѳ�����ڣ����룬Ĭ��100��
		m_PortCfg.nSndDelay = GetPrivateInifileInt("PORT", "SndDelay", 100, file_name.c_str());
		if ( m_PortCfg.nSndDelay < 0  )
			m_PortCfg.nSndDelay = 100;

		//! ��ʱ�����룬Ĭ��1000��
		m_PortCfg.nTimeOut = pDrv->GetRcvTimeOut();
		if ( m_PortCfg.nTimeOut <= 0  )
			m_PortCfg.nTimeOut = 1000;

		//! �˿ڵ�����֡����
		m_PortCfg.nRequestCount = 
			GetPrivateInifileInt("PORT", "RequestCount", 0, file_name.c_str());
		if ( m_PortCfg.nRequestCount < 0 )
			m_PortCfg.nRequestCount = 0;

		m_PortCfg.nSettingGroupCount = GetPrivateInifileInt("PORT", "SettingGroupCount", 0, file_name.c_str());
		if ( nVal < 0 )
		{
			m_PortCfg.nSettingGroupCount = 0;
		}	
		m_PortCfg.nSettingGroupCycl = GetPrivateInifileInt("PORT", "SettingGroupCycl", 500, file_name.c_str());
		if ( nVal < 0 )
		{
			m_PortCfg.nSettingGroupCycl = 500;
		}	

		nVal = GetPrivateInifileInt("PORT", "ExceptionAsOnline", 1, file_name.c_str());
		if ( nVal == 0 )
		{
			m_PortCfg.bExceptionAsOnline = false;
		}
		else
		{
			m_PortCfg.bExceptionAsOnline = true;
		}

		//! 0---CRC���ֽ���ǰ; 1-CRC���ֽ���ǰ��Ĭ��0
		m_PortCfg.nCRCOrder = GetPrivateInifileInt("PORT", "CRCOrder", 0, file_name.c_str());
		if ( m_PortCfg.nCRCOrder != 0 && m_PortCfg.nCRCOrder != 1 )
			m_PortCfg.nCRCOrder = 0;
	}

	/*!
	*	\brief	�����ѯ֡��Ϣ
	*
	*	\param	file_name		�����ļ�·����
	*
	*	\retval	void
	*/
	void CFepModbusDevice::GetReqCfg(std::string file_name)
	{
		char cAppName[50]="";

		REQUEST Req;
		REQUEST_CFG RequestCfg;
		for( int i = 0; i < m_PortCfg.nRequestCount; i++ )
		{
			sprintf( cAppName, "REQUEST%d", i+1 );

			//! 0---ʹ����ʧЧ; 1---ʹ������Ч��Ĭ��1��
			RequestCfg.Enable = GetPrivateInifileInt(cAppName, "Enable", 1, file_name.c_str());
			if ( RequestCfg.Enable != 0 && RequestCfg.Enable != 1 )
			{
				RequestCfg.Enable = 1;
			}
			if ( RequestCfg.Enable == 0 ) 
			{
				continue;
			}

			RequestCfg.Function = GetPrivateInifileInt(cAppName, "Function", 0, file_name.c_str());
			if ( RequestCfg.Function != 1 && RequestCfg.Function != 2 && RequestCfg.Function != 3 && RequestCfg.Function != 4 )
			{
				RequestCfg.Function = 0;
				continue;
			}

			RequestCfg.StartingAddress = GetPrivateInifileInt(cAppName, "StartingAddress", 65536, file_name.c_str());
			if ( RequestCfg.StartingAddress < 0 && RequestCfg.Function >= 65536 )
			{
				RequestCfg.StartingAddress = 65536;
				continue;
			}

			RequestCfg.PointCount = GetPrivateInifileInt(cAppName, "PointCount", 0, file_name.c_str());
			if ( RequestCfg.PointCount <= 0 && RequestCfg.PointCount >= 1000 )
			{
				RequestCfg.PointCount = 0;
				continue;
			}

			RequestCfg.SwapCode = 0;

			//�����Ч����
			Req.SlaveAddress = GetAddr();
			Req.Function	 = RequestCfg.Function;
			Req.StartHi		 = HIBYTE( RequestCfg.StartingAddress );
			Req.StartLo		 = LOBYTE( RequestCfg.StartingAddress );
			Req.PointHi		 = HIBYTE( RequestCfg.PointCount );
			Req.PointLo		 = LOBYTE( RequestCfg.PointCount );

			WORD wCrc = CRC( (Byte *)&Req, 6 );

			//���CRC�ߵ�˳��
			if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
			{
				Req.CRCFirst  = LOBYTE(wCrc);
				Req.CRCSecond = HIBYTE(wCrc);
			}
			else
			{
				Req.CRCFirst  = HIBYTE(wCrc);
				Req.CRCSecond = LOBYTE(wCrc);
			}

			Request.push_back( Req );
			m_nValidReqCount++;
		}
		
		//! �����������
		CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( GetDriver() );
		//! ��¼��־
		if ( pDriver )
		{
			char pchLog[MAXDEBUGARRAYLEN] = {0};
			sprintf( pchLog, "CFepModbusDevice::GetReqCfg Device:%s, CfgFileName:%s, RequestFrameCount:%d!",
				GetName().c_str(), file_name.c_str(), Request.size() );
			pDriver->WriteLogAnyWay( pchLog );
		}
	}

	
	/*!
	*	\brief	��ö���ֵ��֡��Ϣ
	*
	*	\param	file_name		�����ļ�·����
	*
	*	\retval	void
	*/
	void CFepModbusDevice::GetSettingGroupCfg(std::string file_name)
	{
		char cAppName[50]="";
		
		SETTINGGROUP_CFG SettingGroupCfg;
		for( int i = 0; i < m_PortCfg.nSettingGroupCount; i++ )
		{
			sprintf( cAppName, "SETTINGGROUP%d", i+1 );

			SettingGroupCfg.strDoTagName =
				GetPrivateInifileString(cAppName,"DoTagName","",file_name.c_str());
			SettingGroupCfg.iReadCountCfg = 
				GetPrivateInifileInt(cAppName, "ReadCount", 1, file_name.c_str());
			if ( SettingGroupCfg.iReadCount < 0 )
				SettingGroupCfg.iReadCount = 0;			

			SettingGroupCfg.Function = 
				GetPrivateInifileInt(cAppName, "Function", 0, file_name.c_str());
			if ( SettingGroupCfg.Function != 1 && SettingGroupCfg.Function != 2
				&& SettingGroupCfg.Function != 3 && SettingGroupCfg.Function != 4 )
				SettingGroupCfg.Function = 0;

			SettingGroupCfg.StartingAddress = 
				GetPrivateInifileInt(cAppName, "StartingAddress", 65536, file_name.c_str());

			if ( SettingGroupCfg.StartingAddress < 0 && SettingGroupCfg.Function >= 65536 )
				SettingGroupCfg.StartingAddress = 65536;			

			SettingGroupCfg.PointCount = 
				GetPrivateInifileInt(cAppName, "PointCount", 0, file_name.c_str());
			if ( SettingGroupCfg.PointCount <= 0 && SettingGroupCfg.PointCount >= 1000 )
				SettingGroupCfg.PointCount = 0;			

			SettingGroupCfg.SwapCode = 
				GetPrivateInifileInt(cAppName, "SwapCode", 0, file_name.c_str());
			if ( SettingGroupCfg.SwapCode != 0 && SettingGroupCfg.SwapCode != 1 )
				SettingGroupCfg.SwapCode = 0;

			
			vecSettingGroup.push_back( SettingGroupCfg );
			/*vecSettingGroup[i]=SettingGroupCfg;*/

			//del   m_nValidReqCount++;
		}

		char pchLog[MAXDEBUGARRAYLEN];
		sprintf( pchLog, "CFepModbusDevice::GetSettingGroupCfg Device:%s, CfgFileName:%s, ReadSettingGroupCount:%d!",
			GetName().c_str(), file_name.c_str(), m_PortCfg.nSettingGroupCount );
		//! �����������
		CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( GetDriver() );
		//! ��¼��־
		if ( pDriver )
		{
			pDriver->WriteLogAnyWay( pchLog );
		}
	}

	
	/*!
	*	\brief	��ò���֡��Ϣ
	*
	*	\param	file_name		�����ļ�·����
	*
	*	\retval	void
	*/
	void CFepModbusDevice::GetTestCfg(std::string file_name)
	{
		char cAppName[]="TESTREQ";

		REQUEST_CFG RequestCfg;
		//! 0---ʹ����ʧЧ; 1---ʹ������Ч��Ĭ��0��
		RequestCfg.Enable = 
			GetPrivateInifileInt(cAppName, "Enable", 0, file_name.c_str());
		if ( RequestCfg.Enable != 0 && RequestCfg.Enable != 1 )
			RequestCfg.Enable = 0;
		m_bTestReq = RequestCfg.Enable;
		if ( !m_bTestReq ) 
		{
			return;
		}

		RequestCfg.Function = 
			GetPrivateInifileInt(cAppName, "Function", 0, file_name.c_str());
		if ( RequestCfg.Function != 1 && RequestCfg.Function != 2
			&& RequestCfg.Function != 3 && RequestCfg.Function != 4 )
			RequestCfg.Function = 0;

		if ( RequestCfg.Function == 0 ) 
		{
			m_bTestReq = false;
			return;
		}

		RequestCfg.StartingAddress = 
			GetPrivateInifileInt(cAppName, "StartingAddress", 65536, file_name.c_str());

		if ( RequestCfg.StartingAddress < 0 && RequestCfg.Function >= 65536 )
			RequestCfg.StartingAddress = 65536;

		if ( RequestCfg.StartingAddress == 65536 ) 
		{
			m_bTestReq = false;
			return;
		}

		RequestCfg.PointCount = 
			GetPrivateInifileInt(cAppName, "PointCount", 0, file_name.c_str());
		if ( RequestCfg.PointCount <= 0 && RequestCfg.PointCount >= 1000 )
			RequestCfg.PointCount = 0;

		if ( RequestCfg.PointCount == 0 ) 
		{
			m_bTestReq = false;
			return;
		}

		RequestCfg.SwapCode = 
			GetPrivateInifileInt(cAppName, "SwapCode", 0, file_name.c_str());
		if ( RequestCfg.SwapCode != 0 && RequestCfg.SwapCode != 1 )
			RequestCfg.SwapCode = 0;

		//! �����Ч����
		m_reqTest.SlaveAddress = GetAddr();
		m_reqTest.Function	 = RequestCfg.Function;
		m_reqTest.StartHi		 = HIBYTE( RequestCfg.StartingAddress );
		m_reqTest.StartLo		 = LOBYTE( RequestCfg.StartingAddress );
		m_reqTest.PointHi		 = HIBYTE( RequestCfg.PointCount );
		m_reqTest.PointLo		 = LOBYTE( RequestCfg.PointCount );

		WORD wCrc = CRC( (Byte *)&m_reqTest, 6 );
		//! ���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			m_reqTest.CRCFirst  = LOBYTE(wCrc);
			m_reqTest.CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			m_reqTest.CRCFirst  = HIBYTE(wCrc);
			m_reqTest.CRCSecond = LOBYTE(wCrc);
		}
	}

	/*!
	*	\brief	���ң�ذ���֡������Ϣ
	*
	*	\param	file_name		�����ļ�·����
	*
	*	\retval	void
	*/
	void CFepModbusDevice::GetFollowReqCfg(std::string file_name)
	{
		char cKeyName[50]="";

		Byte Function;
		WORD StartingAddress;
		WORD PointCount;
		int lTagAddr;
		int nCtrlFollowReqCount = 
			GetPrivateInifileInt("Ctrl", "Count", 0, file_name.c_str());
		if ( nCtrlFollowReqCount > 0 )
		{
			struCtrlFollowReq* sCtrlFollowRequest = NULL;
			int  msDelay;
			for ( int i = 0; i < nCtrlFollowReqCount; i++ )
			{
				sprintf( cKeyName, "TagAddr%d", i+1 );
				lTagAddr = 
					GetPrivateInifileInt("Ctrl", cKeyName, 0, file_name.c_str());
/*				if ( (lTagAddr >=1 && lTagAddr <= 65536)
					|| (lTagAddr >= 400001 && lTagAddr <= 465536)
					|| (lTagAddr/100 >= 400001 && lTagAddr/100 <= 465536 
					&& lTagAddr%100 >= 0 && lTagAddr%100 <= 15 )
					)
				{*/
					sprintf( cKeyName, "Function%d", i+1 );
					Function = 
						GetPrivateInifileInt("Ctrl", cKeyName, 0, file_name.c_str());
					if ( Function <= 0 || Function > 255 )
					{
						continue;
					}
					sprintf( cKeyName, "StartingAddress%d", i+1 );
					StartingAddress = 
						GetPrivateInifileInt("Ctrl", cKeyName, 0, file_name.c_str());
					if ( StartingAddress < 0 || StartingAddress > 65536 )
					{
						continue;
					}
					sprintf( cKeyName, "PointCount%d", i+1 );
					PointCount = 
						GetPrivateInifileInt("Ctrl", cKeyName, 0, file_name.c_str());
					if ( PointCount <= 0 || PointCount > 2000 )
					{
						continue;
					}
					sprintf( cKeyName, "Delay%d", i+1 );
					msDelay = 
						GetPrivateInifileInt("Ctrl", cKeyName, 1000, file_name.c_str());
					if ( msDelay < 5  )
					{
						msDelay = 5;
					}

					sCtrlFollowRequest = new struCtrlFollowReq;
					sCtrlFollowRequest->TagAddr	= lTagAddr;

					sCtrlFollowRequest->Req.SlaveAddress = (Byte)GetAddr();
					sCtrlFollowRequest->Req.Function		= Function;
					sCtrlFollowRequest->Req.StartHi		= HIBYTE(StartingAddress);
					sCtrlFollowRequest->Req.StartLo		= LOBYTE(StartingAddress);
					sCtrlFollowRequest->Req.PointHi		= HIBYTE(PointCount);
					sCtrlFollowRequest->Req.PointLo		= LOBYTE(PointCount);
					WORD wCrc = CRC( (Byte *)&sCtrlFollowRequest->Req, 6 );
					if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
					{
						sCtrlFollowRequest->Req.CRCFirst  = LOBYTE(wCrc);
						sCtrlFollowRequest->Req.CRCSecond = HIBYTE(wCrc);
					}
					else
					{
						sCtrlFollowRequest->Req.CRCFirst  = HIBYTE(wCrc);
						sCtrlFollowRequest->Req.CRCSecond = LOBYTE(wCrc);
					}
					sCtrlFollowRequest->msDelay	= msDelay;
					m_mapCtrlFollowReq.insert( std::map<int,struCtrlFollowReq*>
						::value_type(lTagAddr, sCtrlFollowRequest) );

//				}
			}

		}
	}

	/*!
	*	\brief	��ȡ��ǩ������ֵ�����Ƚ��Ƿ���nVal��ȣ�����򷵻�false�����򷵻�true���˴�ֻ����Byte�������ݣ������������ݵ��ô˽ӿڻ��쳣
	*/
	bool CFepModbusDevice::IsDIValueChange( int UnitType, int nTagAddr, Byte& byOldVal, Byte& byNewVal, std::string& strTagName )
	{
		OBJTAG ObjTag;
		ObjTag.byUnitType = UnitType;
		ObjTag.lTagAddr = nTagAddr;
		MAP_OBJTAG::iterator it = m_mapObjTag.find( ObjTag.GetIndexAddr() );
		if ( it != m_mapObjTag.end() )
		{
			CIOCfgTag* pObjTag = it->second;
			if ( pObjTag)
			{
				T_mapFieldPoints mapFieldPoints = pObjTag->GetFieldPoints();
                strTagName = pObjTag->GetName();
				if (mapFieldPoints.size() > 0)
				{
					T_mapFieldPoints::iterator itFieldPoint = mapFieldPoints.begin();
					FieldPointType* pFieldPoint = itFieldPoint->second;
					if (pFieldPoint)
					{
						//UaDataValue dataValue = pFieldPoint->value(NULL);
						//UaVariant variant = *dataValue.value();
						//variant.toByte(byOldVal);

						//! ����nVal�õ��µı�ǩֵ
						//! ����ת��
						float fRawZero = pFieldPoint->getRawZero();
						float fRawFull = pFieldPoint->getRawFull();
						float fEngZero = pFieldPoint->getEngZero();
						float fEngFull = pFieldPoint->getEngFull();
						if ( fabs( fRawFull - fRawZero ) > 0.001 && fabs( fEngFull - fEngZero ) > 0.001  )
						{
							//! ��������ת����ֵ
							byNewVal = ( (double)(Byte)byNewVal - fRawZero)*(fEngFull - fEngZero)/(fRawFull - fRawZero) + fEngZero;
						}

						if (byNewVal != byOldVal)
						{
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	int CFepModbusDevice::BuildF650CheckTimeFrame( Byte* pbyBuf )
	{
		if ( pbyBuf == NULL )
		{
			return 0;
		}

		//���ݱ�׼���ã�400001~465536�� ��ѧ�� 2013.2.18
		if ( m_nCheckTimeAddr >= 400001 || m_nCheckTimeAddr <= 465536 )
		{
			m_nCheckTimeAddr = m_nCheckTimeAddr - 400001;
		}
		if ( m_nCheckTimeAddr < 0 || m_nCheckTimeAddr > 65535 )
		{
			return 0;
		}
		
		//Synchronism example:
		//[0xFE 0x10 0xFF 0xF0 0x00 0x04 0x08 0x00 0x00 0x00 0x17 0x9B 0x53 0x3F 0x60 0xA4 0x2B] ---------> RELAY

		int FrameLen = sizeof(F650CHECKTIMEREQUEST);	
		F650CHECKTIMEREQUEST* pReq = (F650CHECKTIMEREQUEST* )pbyBuf;
		pReq->SlaveAddress		= m_bBroadcast?0:(Byte)GetAddr();
		pReq->Function			= FORCE_ANA_MULTIPLE_OUTPUT_STATUS;//!�����룿����
		pReq->StartHi			= HIBYTE(m_nCheckTimeAddr);
		pReq->StartLo			= LOBYTE(m_nCheckTimeAddr);
		pReq->CountHi			= 0x00;
		pReq->CountLo			= 0x04;
		pReq->ByteCount			= 0x08;

		Byte temp[8];
		long llTotalDays;
		long llTime;
		ACE_Time_Value val = ACE_OS::gettimeofday();	
		llTime = val.sec()*1000LL + val.usec()/1000LL;		
		
		llTotalDays = Days(2000,1,true);
		 //!2000/01/01 00:00:00 -----1970/01/01 00:00:00
		llTime -= llTotalDays * 86400000LL;

		llTime += 28800000;
		ACE_OS::memcpy(temp ,(Byte*)&llTime,8 );

		pReq->bySendTimeStap[0] = temp[7];
		pReq->bySendTimeStap[1] = temp[6];
		pReq->bySendTimeStap[2] = temp[5];
		pReq->bySendTimeStap[3] = temp[4];
		pReq->bySendTimeStap[4] = temp[3];
		pReq->bySendTimeStap[5] = temp[2];
		pReq->bySendTimeStap[6] = temp[1];
		pReq->bySendTimeStap[7] = temp[0];

		WORD wCrc = CRC( (Byte *)pReq, FrameLen - 2 );
		//���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			pReq->CRCFirst  = LOBYTE(wCrc);
			pReq->CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			pReq->CRCFirst  = HIBYTE(wCrc);
			pReq->CRCSecond = LOBYTE(wCrc);
		}	

		return FrameLen;
	}

	int CFepModbusDevice::BuildL30CheckTimeFrame( Byte* pbyBuf )
	{
		if ( pbyBuf == NULL )
		{
			return 0;
		}
		//���ݱ�׼���ã�400001~465536�� ��ѧ�� 2013.2.18
		if ( m_nCheckTimeAddr >= 400001 || m_nCheckTimeAddr <= 465536 )
		{
			m_nCheckTimeAddr = m_nCheckTimeAddr - 400001;
		}
		if ( m_nCheckTimeAddr < 0 || m_nCheckTimeAddr > 65535 )
		{
			return 0;
		}

		int FrameLen = sizeof(L30CHECKTIMEREQUEST);	
		L30CHECKTIMEREQUEST* pReq = (L30CHECKTIMEREQUEST* )pbyBuf;
		pReq->SlaveAddress		= (Byte)GetAddr();  //!���豸��֧�ֵ�����ʱ
		pReq->Function			= FORCE_ANA_MULTIPLE_OUTPUT_STATUS;//!������
		pReq->StartHi			= HIBYTE(m_nCheckTimeAddr);
		pReq->StartLo			= LOBYTE(m_nCheckTimeAddr);
		pReq->CountHi			= 0x00;
		pReq->CountLo			= 0x02;
		pReq->ByteCount			= 0x04;

		Byte byTemp[4];
		ACE_Time_Value val = ACE_OS::gettimeofday();	
		int iTime = val.sec();
		iTime += 28800;
		ACE_OS::memcpy( byTemp,(Byte*)&iTime,4 );

		pReq->bySendTimeStap[0] = byTemp[3];
		pReq->bySendTimeStap[1] = byTemp[2];
		pReq->bySendTimeStap[2] = byTemp[1];
		pReq->bySendTimeStap[3] = byTemp[0];		

		WORD wCrc = CRC( (Byte *)pReq, FrameLen - 2 );
		//���CRC�ߵ�˳��
		if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
		{
			pReq->CRCFirst  = LOBYTE(wCrc);
			pReq->CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			pReq->CRCFirst  = HIBYTE(wCrc);
			pReq->CRCSecond = LOBYTE(wCrc);
		}

		return FrameLen;
	}

	bool CFepModbusDevice::isLeap( int iYear )
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

	int CFepModbusDevice::Days( int iYear,int iMon,bool bFrom2000 )
	{
		int iRet =0;
		int iDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
		int iLeapDays[]={31,29,31,30,31,30,31,31,30,31,30,31};
		if ( bFrom2000 == false )
		{
			if ( isLeap(iYear))
			{
				for ( int i=0;i<iMon-1;i++ )
				{
					iRet += iLeapDays[i];
				}
			}
			else
			{
				for ( int i=0;i<iMon-1;i++ )
				{
					iRet += iDays[i];
				}
			}
		}
		else
		{
			for ( int k=1970;k<iYear;k++)
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
				for ( int i=0;i<iMon-1;i++ )
				{
					iRet += iLeapDays[i];
				}
			}
			else
			{
				for ( int i=0;i<iMon-1;i++ )
				{
					iRet += iDays[i];
				}
			}
		}

		return iRet;
	}


	BYTE CFepModbusDevice::BCD2HEX(BYTE bcd)
	{
		return BYTE(int(int(bcd)/16)*10+int(bcd)%16);
	}

	BYTE CFepModbusDevice::HEX2BCD(BYTE hex)
	{
		return BYTE(int(int(hex)/10)*16+int(hex)%10);
	}

	int CFepModbusDevice::BuildLDCheckTimeFrame( BYTE* buf )
	{
		//! ����У��
		if ( buf == NULL )
		{
			return 0;
		}
		if ( m_nCheckTimeAddr < 400001 || m_nCheckTimeAddr > 465536 )
		{
			return 0;
		}
		//! ֡��
		int nFrameLen = sizeof(LDCHECKTIMEREQUEST);

		//! ���ϵͳʱ��
		//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //�����Ĵ���ʱ��,����
		ACE_Time_Value val = ACE_OS::gettimeofday();
		ACE_Date_Time dateTime;
		dateTime.update( val );

		Byte byBCD;
		LDCHECKTIMEREQUEST* pReq = (LDCHECKTIMEREQUEST* )buf;
		pReq->SlaveAddress = (Byte)GetAddr();
		pReq->Function = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;		//! 10H
		pReq->StartHi = HIBYTE(m_nCheckTimeAddr - 400001);		//! 00H
		pReq->StartLo = LOBYTE(m_nCheckTimeAddr - 400001);		//! 00H
		pReq->PointHi = 0x00;		//! 00H
		pReq->PointLo = 0x04;		//! 04H
		pReq->ByteCount = 0x08;		//! 08H

		//CIOSwitchData::DectoBCD( dateTime.year()%100,  &byBCD, 1);
		//pReq->Year = byBCD;								//! ��
		//CIOSwitchData::DectoBCD( dateTime.month(),  &byBCD, 1);
		//pReq->Month = byBCD;							//! ��
		//CIOSwitchData::DectoBCD( dateTime.day(),  &byBCD, 1);
		//pReq->Day = byBCD;								//! ��
		//CIOSwitchData::DectoBCD( dateTime.hour(),  &byBCD, 1);
		//pReq->Hour = byBCD;								//! ʱ
		//CIOSwitchData::DectoBCD( dateTime.minute(),  &byBCD, 1);
		//pReq->Minute = byBCD;							//! ��
		//CIOSwitchData::DectoBCD( dateTime.second(),  &byBCD, 1);
		//pReq->Second = byBCD;							//! ��

		pReq->MsecHi = HIBYTE((WORD)(dateTime.microsec()/1000));		//! �����λ
		pReq->MsecLo = HIBYTE((WORD)(dateTime.microsec()/1000));		//! �����λ

		//! ����У����
		WORD wCrc = CRC( (Byte *)pReq, nFrameLen - 2 );
		if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
		{
			pReq->CRCFirst = LOBYTE(wCrc);
			pReq->CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			pReq->CRCFirst = HIBYTE(wCrc);
			pReq->CRCSecond = LOBYTE(wCrc);
		}
		return nFrameLen;
	}

	int CFepModbusDevice::BuildSEPCOSCheckTimeFrame(Byte* pbyBuf)
	{
		int iRet = 0;	//!����ֵ,��ʼ��Ϊ0
		if(0==pbyBuf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!ָ��Ϊ�ջ�Уʱ��ַ����Χʱ������ֵ��Ϊ-1
		}
		//!����������
		else
		{
			int FrameLen = sizeof(SEPCOSTIMEFRAME);
			const int  POINTCOUNT	= 0x09;
			const int  DATALENGTH	= 18;


			int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;			//�����Ĵ���ʱ��, ����.
			ACE_Time_Value curValue = ACE_OS::gettimeofday();		//��ȡ��ǰʱ��
			ACE_Time_Value offsetValue(0,msOffset*1000);
			curValue += offsetValue;
			ACE_Date_Time dateTime;
			dateTime.update(curValue);

			SEPCOSTIMEFRAME* pReq	= (SEPCOSTIMEFRAME* )pbyBuf;
			pReq->m_bySlaveAddr		= m_bBroadcast?0xFF:(BYTE)GetAddr();
			pReq->m_byFunction		= FORCE_ANA_MULTIPLE_OUTPUT_STATUS;
			pReq->m_byStartAddrHi	= HIBYTE(m_nCheckTimeAddr - 400001);
			pReq->m_byStartAddrLo	= LOBYTE(m_nCheckTimeAddr - 400001);
			pReq->m_byRegNoHi		= HIBYTE(POINTCOUNT);
			pReq->m_byRegNoLo		= LOBYTE(POINTCOUNT);
			pReq->m_byByteCount		= DATALENGTH;
			pReq->m_byYearHi		= 0;
			pReq->m_byYearLo		= LOBYTE(dateTime.year()%100);
			pReq->m_byMonthHi		= HIBYTE(dateTime.month());
			pReq->m_byMonthLo		= LOBYTE(dateTime.month());
			pReq->m_byDayHi			= HIBYTE(dateTime.day());
			pReq->m_byDayLo			= LOBYTE(dateTime.day());
			pReq->m_byWeekDayHi		= HIBYTE(dateTime.weekday());
			pReq->m_byWeekDayLo		= LOBYTE(dateTime.weekday());
			pReq->m_byHourHi		= HIBYTE(dateTime.hour());
			pReq->m_byHourLo		= LOBYTE(dateTime.hour());
			pReq->m_byMinuteHi		= HIBYTE(dateTime.minute());
			pReq->m_byMinuteLo		= LOBYTE(dateTime.minute());
			pReq->m_bySecondHi		= HIBYTE(dateTime.second());
			pReq->m_bySecondLo		= LOBYTE(dateTime.second());
			pReq->m_byMillisecondsHi= HIBYTE(dateTime.microsec()/1000);
			pReq->m_byMillisecondsLo= LOBYTE(dateTime.microsec()/1000);
			pReq->m_byValidate1		= 0;
			pReq->m_byValidate2		= 1;

			WORD wCrc = CRC( (BYTE *)pReq, FrameLen - 2 );

			if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
			{
				pReq->m_byCRCFirst  = LOBYTE(wCrc);
				pReq->m_byCRCSecond = HIBYTE(wCrc);
			}
			else
			{
				pReq->m_byCRCFirst  = HIBYTE(wCrc);
				pReq->m_byCRCSecond = LOBYTE(wCrc);
			}

			iRet = FrameLen;
		}
		return iRet;
	}


	int CFepModbusDevice::BuildDQPLCCheckTimeFrame(Byte* pbyBuf)
	{
		int iRet = 0;	//!����ֵ,��ʼ��Ϊ0
		if(0==pbyBuf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!ָ��Ϊ�ջ�Уʱ��ַ����Χʱ������ֵ��Ϊ-1
		}
		//!����������
		else
		{
			int nFrameLen = sizeof(DQPLCCHECKTIMEREQUEST);

			//! ���ϵͳʱ��
			//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //�����Ĵ���ʱ��,����
			ACE_Time_Value val = ACE_OS::gettimeofday();
			ACE_Date_Time dateTime;
			dateTime.update( val );

			Byte byBCD;
			DQPLCCHECKTIMEREQUEST* pReq = (DQPLCCHECKTIMEREQUEST* )pbyBuf;
			pReq->SlaveAddress = (Byte)GetAddr();
			pReq->Function = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;		//! 10H
			pReq->StartHi = HIBYTE(m_nCheckTimeAddr - 400001);		//! 00H
			pReq->StartLo = LOBYTE(m_nCheckTimeAddr - 400001);		//! 00H
			pReq->PointHi = 0x00;		//! 00H
			pReq->PointLo = 5;		
			pReq->ByteCount = 10;		

			//CIOSwitchData::DectoBCD( dateTime.year()%100,  &byBCD, 1);
			//pReq->Year = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.month(),  &byBCD, 1);
			//pReq->Month = byBCD;							//! ��
			//CIOSwitchData::DectoBCD( dateTime.day(),  &byBCD, 1);
			//pReq->Day = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.hour(),  &byBCD, 1);
			//pReq->Hour = byBCD;								//! ʱ
			//CIOSwitchData::DectoBCD( dateTime.minute(),  &byBCD, 1);
			//pReq->Minute = byBCD;							//! ��
			//CIOSwitchData::DectoBCD( dateTime.second(),  &byBCD, 1);
			//pReq->Second = byBCD;							//! ��

			//pReq->MsecHi = HIBYTE((WORD)(dateTime.microsec()/1000));		//! �����λ
			//pReq->MsecLo = LOBYTE((WORD)(dateTime.microsec()/1000));		//! �����λ
			pReq->MsecHi = 0;		//! �����λ
			pReq->MsecLo = 0;		//! �����λ

			pReq->m_byValidate1		= 0;
			pReq->m_byValidate2		= 1;

			//! ����У����
			WORD wCrc = CRC( (Byte *)pReq, nFrameLen - 2 );
			if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
			{
				pReq->CRCFirst = LOBYTE(wCrc);
				pReq->CRCSecond = HIBYTE(wCrc);
			}
			else
			{
				pReq->CRCFirst = HIBYTE(wCrc);
				pReq->CRCSecond = LOBYTE(wCrc);
			}

			iRet = nFrameLen;
		}
		return iRet;
	}


	int CFepModbusDevice::BuildDLLSJZLPCheckTimeFrame(Byte* pbyBuf)
	{
		int iRet = 0;	//!����ֵ,��ʼ��Ϊ0
		if(0==pbyBuf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!ָ��Ϊ�ջ�Уʱ��ַ����Χʱ������ֵ��Ϊ-1
		}
		//!����������
		else
		{
			int nFrameLen = sizeof(DLLSJZLPCHECKTIMEREQUEST);

			//! ���ϵͳʱ��
			//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //�����Ĵ���ʱ��,����
			ACE_Time_Value val = ACE_OS::gettimeofday();
			ACE_Date_Time dateTime;
			dateTime.update( val );

			Byte byBCD = 0;
			DLLSJZLPCHECKTIMEREQUEST* pReq = (DLLSJZLPCHECKTIMEREQUEST* )pbyBuf;
			pReq->SlaveAddress = (Byte)GetAddr();
			pReq->Function = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;		//! 10H
			pReq->StartHi = HIBYTE(m_nCheckTimeAddr - 400001);		//! 00H
			pReq->StartLo = LOBYTE(m_nCheckTimeAddr - 400001);		//! 00H
			pReq->PointHi = 0x00;		//! 00H
			pReq->PointLo = 4;		
			//pReq->ByteCount = 8;		

			//CIOSwitchData::DectoBCD( dateTime.year()/100,  &byBCD, 1);
			//pReq->YearHi = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.year()%100,  &byBCD, 1);
			//pReq->YearLo = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.month(),  &byBCD, 1);
			//pReq->Month = byBCD;							//! ��
			//CIOSwitchData::DectoBCD( dateTime.day(),  &byBCD, 1);
			//pReq->Day = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.hour(),  &byBCD, 1);
			//pReq->Hour = byBCD;								//! ʱ
			//CIOSwitchData::DectoBCD( dateTime.minute(),  &byBCD, 1);
			pReq->Minute = byBCD;							//! ��

			WORD wdMsec = dateTime.second()*1000 + dateTime.microsec()/1000;

			pReq->MsecHi = HIBYTE(wdMsec);		//! �����λ
			pReq->MsecLo = LOBYTE(wdMsec);		//! �����λ

			//! ����У����
			WORD wCrc = CRC( (Byte *)pReq, nFrameLen - 2 );
			if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
			{
				pReq->CRCFirst = LOBYTE(wCrc);
				pReq->CRCSecond = HIBYTE(wCrc);
			}
			else
			{
				pReq->CRCFirst = HIBYTE(wCrc);
				pReq->CRCSecond = LOBYTE(wCrc);
			}

			iRet = nFrameLen;
		}
		return iRet;
	}

	int CFepModbusDevice::BuildZSZDCheckTimeFrame(Byte* buf)
	{
		//! ����У��
		if ( buf == NULL )
		{
			return 0;
		}
		if ( m_nCheckTimeAddr < 400001 || m_nCheckTimeAddr > 465536 )
		{
			return 0;
		}
		//! ֡��
		int nFrameLen = sizeof(ZSZDCHECKTIMEREQUEST);

		//! ���ϵͳʱ��
		//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //�����Ĵ���ʱ��,����
		ACE_Time_Value val = ACE_OS::gettimeofday();
		ACE_Date_Time dateTime;
		dateTime.update( val );

		Byte byBCD;
		ZSZDCHECKTIMEREQUEST* pReq = (ZSZDCHECKTIMEREQUEST* )buf;
		pReq->SlaveAddress = (Byte)GetAddr();
		pReq->Function = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;		//! 10H
		pReq->StartHi = HIBYTE(m_nCheckTimeAddr - 400001);		//! 00H
		pReq->StartLo = LOBYTE(m_nCheckTimeAddr - 400001);		//! 00H
		pReq->PointHi = 0x00;		//! 00H
		pReq->PointLo = 0x03;		//! 03H
		pReq->ByteCount = 0x06;		//! 06H

		pReq->Year = dateTime.year()-2000;						//! ��
		pReq->Month = dateTime.month();							//! ��
		pReq->Day = dateTime.day();								//! ��
		pReq->Hour = dateTime.hour();							//! ʱ
		pReq->Minute = dateTime.minute();						//! ��
		pReq->Second = dateTime.second();						//! ��

		//! ����У����
		WORD wCrc = CRC( (Byte *)pReq, nFrameLen - 2 );
		if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
		{
			pReq->CRCFirst = LOBYTE(wCrc);
			pReq->CRCSecond = HIBYTE(wCrc);
		}
		else
		{
			pReq->CRCFirst = HIBYTE(wCrc);
			pReq->CRCSecond = LOBYTE(wCrc);
		}
		return nFrameLen;
	}

	int CFepModbusDevice::BuildDLSIEMENSS73CheckTimeFrame( BYTE* buf )
	{
		int iRet = 0;	//!����ֵ,��ʼ��Ϊ0
		if(0==buf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!ָ��Ϊ�ջ�Уʱ��ַ����Χʱ������ֵ��Ϊ-1
		}
		//!����������
		else
		{
			int nFrameLen = sizeof(DL_SIEMENSS73_CHECKTIMEREQUEST);

			//! ���ϵͳʱ��
			//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //�����Ĵ���ʱ��,����
			ACE_Time_Value val = ACE_OS::gettimeofday();
			ACE_Date_Time dateTime;
			dateTime.update( val );

			Byte byBCD;
			DL_SIEMENSS73_CHECKTIMEREQUEST* pReq = (DL_SIEMENSS73_CHECKTIMEREQUEST* )buf;
			pReq->SlaveAddress = (Byte)GetAddr();
			pReq->Function = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;		//! 10H
			pReq->StartHi = HIBYTE(m_nCheckTimeAddr - 400001);		//! 00H
			pReq->StartLo = LOBYTE(m_nCheckTimeAddr - 400001);		//! 00H
			pReq->PointHi = 0x00;		//! 00H
			pReq->PointLo = 5;		
			pReq->ByteCount = 10;		

			//CIOSwitchData::DectoBCD( dateTime.year()/100,  &byBCD, 1);
			//pReq->YearHi = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.year()%100,  &byBCD, 1);
			//pReq->YearLo = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.month(),  &byBCD, 1);
			//pReq->Month = byBCD;							//! ��
			//CIOSwitchData::DectoBCD( dateTime.day(),  &byBCD, 1);
			//pReq->Day = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.hour(),  &byBCD, 1);
			//pReq->Hour = byBCD;								//! ʱ
			//CIOSwitchData::DectoBCD( dateTime.minute(),  &byBCD, 1);
			//pReq->Minute = byBCD;							//! ��
			//CIOSwitchData::DectoBCD( dateTime.second(),  &byBCD, 1);
			//pReq->Second = byBCD;							//! ��

			//WORD wdMsec = dateTime.microsec()/1000;
			//CIOSwitchData::DectoBCD( wdMsec/10,  &byBCD, 1);
			//pReq->MsecHi = byBCD;
			//CIOSwitchData::DectoBCD( wdMsec%10,  &byBCD, 1);
			pReq->MsecLo = byBCD;

			pReq->Rev	 = 0;

			//! ����У����
			WORD wCrc = CRC( (Byte *)pReq, nFrameLen - 2 );
			if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
			{
				pReq->CRCFirst = LOBYTE(wCrc);
				pReq->CRCSecond = HIBYTE(wCrc);
			}
			else
			{
				pReq->CRCFirst = HIBYTE(wCrc);
				pReq->CRCSecond = LOBYTE(wCrc);
			}

			iRet = nFrameLen;
		}
		return iRet;
	}

	int CFepModbusDevice::BuildDLUMLESSCCheckTimeFrame( BYTE* buf )
	{
		int iRet = 0;	//!����ֵ,��ʼ��Ϊ0
		if(0==buf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!ָ��Ϊ�ջ�Уʱ��ַ����Χʱ������ֵ��Ϊ-1
		}
		//!����������
		else
		{
			int nFrameLen = sizeof(DL_UMLESSC_CHECKTIMEREQUEST);

			//! ���ϵͳʱ��
			//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //�����Ĵ���ʱ��,����
			ACE_Time_Value val = ACE_OS::gettimeofday();
			ACE_Date_Time dateTime;
			dateTime.update( val );

			Byte byBCD = 0;
			DL_UMLESSC_CHECKTIMEREQUEST* pReq = (DL_UMLESSC_CHECKTIMEREQUEST* )buf;
			pReq->SlaveAddress = (Byte)GetAddr();
			pReq->Function = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;		//! 10H
			pReq->StartHi = HIBYTE(m_nCheckTimeAddr - 400001);		//! 00H
			pReq->StartLo = LOBYTE(m_nCheckTimeAddr - 400001);		//! 00H
			pReq->PointHi = 0x00;		//! 00H
			pReq->PointLo = 4;		
			pReq->ByteCount = 8;		

			//CIOSwitchData::DectoBCD( dateTime.year()/100,  &byBCD, 1);
			//pReq->YearHi = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.year()%100,  &byBCD, 1);
			//pReq->YearLo = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.month(),  &byBCD, 1);
			//pReq->Month = byBCD;							//! ��
			//CIOSwitchData::DectoBCD( dateTime.day(),  &byBCD, 1);
			//pReq->Day = byBCD;								//! ��
			//CIOSwitchData::DectoBCD( dateTime.hour(),  &byBCD, 1);
			//pReq->Hour = byBCD;								//! ʱ
			//CIOSwitchData::DectoBCD( dateTime.minute(),  &byBCD, 1);
			//pReq->Minute = byBCD;							//! ��
			//CIOSwitchData::DectoBCD( dateTime.second(),  &byBCD, 1);
			//pReq->Second = byBCD;							//! ��

			//WORD wdMsec = dateTime.microsec()/1000;
			//CIOSwitchData::DectoBCD( wdMsec/10,  &byBCD, 1);
			pReq->CendSecond = byBCD;

			//! ����У����
			WORD wCrc = CRC( (Byte *)pReq, nFrameLen - 2 );
			if(m_PortCfg.nCRCOrder == CRC_LOB_FIRST)
			{
				pReq->CRCFirst = LOBYTE(wCrc);
				pReq->CRCSecond = HIBYTE(wCrc);
			}
			else
			{
				pReq->CRCFirst = HIBYTE(wCrc);
				pReq->CRCSecond = LOBYTE(wCrc);
			}

			iRet = nFrameLen;
		}
		return iRet;
	}

	int CFepModbusDevice::BuildDLSIEMENSS72CheckTimeFrame( BYTE* buf )
	{
		int iRet = 0;	//!����ֵ,��ʼ��Ϊ0
		if(0==buf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!ָ��Ϊ�ջ�Уʱ��ַ����Χʱ������ֵ��Ϊ-1
		}
		//!����������
		else
		{
			int FrameLen = sizeof(DL_SIEMENSS72_CHECKTIMEREQUEST);
			const int  POINTCOUNT	= 6;
			const int  DATALENGTH	= 12;


			//! ���ϵͳʱ��
			//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //�����Ĵ���ʱ��,����
			ACE_Time_Value val = ACE_OS::gettimeofday();
			ACE_Date_Time dateTime;
			dateTime.update( val );

			DL_SIEMENSS72_CHECKTIMEREQUEST* pReq	= (DL_SIEMENSS72_CHECKTIMEREQUEST* )buf;
			pReq->m_bySlaveAddr		= m_bBroadcast?0xFF:(BYTE)GetAddr();
			pReq->m_byFunction		= FORCE_ANA_MULTIPLE_OUTPUT_STATUS;
			pReq->m_byStartAddrHi	= HIBYTE(m_nCheckTimeAddr - 400001);
			pReq->m_byStartAddrLo	= LOBYTE(m_nCheckTimeAddr - 400001);
			pReq->m_byRegNoHi		= HIBYTE(POINTCOUNT);
			pReq->m_byRegNoLo		= LOBYTE(POINTCOUNT);
			pReq->m_byByteCount		= DATALENGTH;
			pReq->m_byYearHi		= HIBYTE(dateTime.year());
			pReq->m_byYearLo		= LOBYTE(dateTime.year());
			pReq->m_byMonthHi		= HIBYTE(dateTime.month());
			pReq->m_byMonthLo		= LOBYTE(dateTime.month());
			pReq->m_byDayHi			= HIBYTE(dateTime.day());
			pReq->m_byDayLo			= LOBYTE(dateTime.day());
			pReq->m_byHourHi		= HIBYTE(dateTime.hour());
			pReq->m_byHourLo		= LOBYTE(dateTime.hour());
			pReq->m_byMinuteHi		= HIBYTE(dateTime.minute());
			pReq->m_byMinuteLo		= LOBYTE(dateTime.minute());
			pReq->m_bySecondHi		= HIBYTE(dateTime.second());
			pReq->m_bySecondLo		= LOBYTE(dateTime.second());

			WORD wCrc = CRC( (BYTE *)pReq, FrameLen - 2 );

			if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
			{
				pReq->m_byCRCFirst  = LOBYTE(wCrc);
				pReq->m_byCRCSecond = HIBYTE(wCrc);
			}
			else
			{
				pReq->m_byCRCFirst  = HIBYTE(wCrc);
				pReq->m_byCRCSecond = LOBYTE(wCrc);
			}

			iRet = FrameLen;
		}
		return iRet;
	}

	int CFepModbusDevice::BuildSFCSC268CheckTimeFrame( BYTE* buf )
	{
		int nRet = 0;
		if (NULL == buf|| m_nCheckTimeAddr<400001|| m_nCheckTimeAddr>465536)
		{
			nRet = -1;
		}
		else
		{
			int nFrameLen = sizeof(SF_CSC268_CHECKTIME);
			const int POINTCOUNT = 4;
			int DATALENGTH = 8;
			Byte byBCD;

			//��ȡϵͳ��ǰʱ��
			ACE_Time_Value timeValue = ACE_OS::gettimeofday();
			ACE_Date_Time dateTime;
			dateTime.update(timeValue);

			SF_CSC268_CHECKTIME* pRequest = (SF_CSC268_CHECKTIME*)buf;
			ACE_OS::memset(pRequest, 0, nFrameLen);
			pRequest->bySlaveAddr = m_bBroadcast?0xFF:(Byte)GetAddr();
			pRequest->byFunc = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;
			pRequest->byStartHi = HIBYTE(m_nCheckTimeAddr-400001);
			pRequest->byStartLo = LOBYTE(m_nCheckTimeAddr-400001);
			pRequest->byRegCountHi		= HIBYTE(POINTCOUNT);
			pRequest->byRegCountLo		= LOBYTE(POINTCOUNT);
			pRequest->byCount		= DATALENGTH;
			
			WORD wYear = dateTime.year();
			pRequest->byYearHi = 0;
			pRequest->byYearLo = wYear%100;
			pRequest->byMonth = dateTime.month();
			pRequest->byDay = dateTime.day();
			pRequest->byHour = dateTime.hour();
			pRequest->byMinute = dateTime.minute();
			unsigned int nMillSecond = dateTime.second()*1000;
			pRequest->byMillSecHi = HIBYTE(nMillSecond);
			pRequest->byMillSecLo = LOBYTE(nMillSecond);

			WORD wCrc = CRC( (Byte*)pRequest, nFrameLen - 2 );
			if ( m_PortCfg.nCRCOrder == CRC_LOB_FIRST )
			{
				pRequest->byCRCFirst  = LOBYTE(wCrc);
				pRequest->byCRCSecond = HIBYTE(wCrc);
			}
			else
			{
				pRequest->byCRCFirst  = HIBYTE(wCrc);
				pRequest->byCRCSecond = LOBYTE(wCrc);
			}

			nRet = nFrameLen;
		}
		return nRet;
	}

	void CFepModbusDevice::SetHot( Byte byHot )
	{
		CIODeviceBase::SetHot(byHot);
	}

	bool CFepModbusDevice::IsReqeustExited( Byte Funcode, WORD StartAddr, WORD PointCount )
	{
		if ( PointCount == 0 
			|| (( Funcode == 1 || Funcode == 2) && PointCount > 2000 )
			|| (( Funcode == 3 || Funcode == 4) && PointCount > 125 )
			)
		{
			return false;
		}

		for ( int i = 0; i < Request.size(); i++ )
		{
			if ( Request[i].Function == Funcode 
				&& MAKEWORD( Request[i].StartLo, Request[i].StartHi) == StartAddr
				&& MAKEWORD( Request[i].PointLo, Request[i].PointHi) == PointCount
				)
			{
				return true;
			}
		}

		if ( m_bTestReq 
			&& m_reqTest.Function == Funcode 
			&& MAKEWORD( m_reqTest.StartLo, m_reqTest.StartHi) == StartAddr
			&& MAKEWORD( m_reqTest.PointLo, m_reqTest.PointHi) == PointCount
			)
		{
			return true;
		}

		if ( m_mapCtrlFollowReq.size() )
		{
			std::map<int, struCtrlFollowReq*>::iterator it = m_mapCtrlFollowReq.begin();
			for ( ; it != m_mapCtrlFollowReq.end(); it++ )
			{
				REQUEST* pReq = &(*it).second->Req;
				if (
					pReq->Function == Funcode 
					&& MAKEWORD( pReq->StartLo, pReq->StartHi) == StartAddr
					&& MAKEWORD( pReq->PointLo, pReq->PointHi) == PointCount
					)
				{
					return true;
				}
			}
		}

		return false;
	}

	bool CFepModbusDevice::IsFuzzy()
	{
		if ( Request.size() != 1 )
		{
			return false;
		}

		//��������ȿ���������Ҳ��������Ӧ��
		//XX 01 03 XX 00 YY XX XX 
		//XX 02 03 XX 00 YY XX XX 
		//���е�YY ��ΧΪ17~24ʱ���ȿ�������Ҳ��������Ӧ��
		if ( Request[0].Function == 1 || Request[0].Function == 2 
			&& Request[0].StartHi == 3
			&& Request[0].PointHi == 0
			&& Request[0].PointLo >= 17 && Request[0].PointLo <= 24
			)
		{
			return true;
		}

		return false;
	}

	bool CFepModbusDevice::IsExceptionAsOnline()
	{
		return m_PortCfg.bExceptionAsOnline;
	}

	void CFepModbusDevice::WriteLogForMO( int nLen )
	{
		CIODriver* pDriver = (CIODriver*)GetDriver();
		//char pchLog[MAXDEBUGARRAYLEN] = {0};
		//snprintf(pchLog, MAXDEBUGARRAYLEN, "CFepModbusDevice::BuildCtrlMultiRegsFrame Failed! Device:%s Length:%d > MaxCtrlFrameLen", GetName().c_str(), nLen);
		//pDriver->WriteLogAnyWay(pchLog);
	}
}
