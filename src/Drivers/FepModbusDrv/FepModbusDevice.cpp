/*!
 *	\file	FepModbusDevice.cpp
 *
 *	\brief	通用Modbus设备类实现源文件
 *
 *	详细的文件说明（可选）
 *
 *	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusDevice.cpp, 470+1 2011/04/22 01:56:47 xuhongtao $
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
	std::string CFepModbusDevice::s_ClassName("CFepModbusDevice");		//! 类名

	std::string CFepModbusDevice::GetClassName()
	{
		return s_ClassName;
	}

	CFepModbusDevice::CFepModbusDevice()
	{
		m_byTxTimes = 0;
		m_byRxTimes = 0;

		m_bFlag = true;

		//! 当前应发送的轮询帧序号，初始为0
		m_shCurrReq = 0;

		m_bNeedCheckTime = false;

		m_nTrySendCount = 0;

		m_nValidReqCount = 0;  //! 有效轮训请求帧数量

		m_bSOESended = false;

		m_nSlaveTimeOuts = 0;	//! 初始化从机上该设备的超时次数为0

		//! 特殊轮询帧相关
		m_nSpecialReqCount = 0;	 //! 特殊轮询帧个数
		m_nCurSpecialReq = 0;	 //! 当前特殊请求号，0～m_nSpecialReqCount－1

		m_bRetryPeriod = true;

		//! 位掩码
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

		//!对设备Port配置进行初始化
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
	*	\brief	初始化
	*
	*	\retval	0为成功，否则失败
	*/
	int CFepModbusDevice::Init()
	{
		int ret = CIODeviceBase::Init();

		InitNeedRegister();

		if (ret != 0)
		{
			return ret;
		}

		//! 获取设备配置文件路径
		if ( !m_pIOUnitCfg->GetUnitCfgName().empty())
		{
			//! 获取配置文件绝对路径
			m_strCfgFileName = CIODriver::GetFEPRunDir() + m_pIOUnitCfg->GetUnitCfgName();
		}

		//! 获取设备配置信息
		InitCfg();

		if( m_nDevType == DEVTYPE_TEPIF)
			m_nSpecialReqCount = 1;

		return 0;
	}

	/*!
	*	\brief	开辟需要的寄存器空间（主要是只取寄存器中某位的DI或AI点需要）
	*
	*	\retval	0为成功，否则失败
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
	*	\brief	记录需要的寄存器（主要是只取寄存器中某位的DI或AI点需要）
	*
	*	\param	iUnitType		UNITTYPE
	*	\param	iDivisor		除数
	*
	*	\retval	0为成功，否则失败
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
	*	\brief	构造本地链路在线时的本地下行帧(不含遥控)
	*
	*	\param	pBuf		帧首地址
	*
	*	\retval	int	帧长度
	*/
	int CFepModbusDevice::BuildLocalNormalFrame( Byte* pBuf )
	{
		if ( !pBuf )
		{
			return 0;
		}

		int nRet = 0;

		//对于离线设备：如果链路上属于部分设备离线,并且离线的设备需要采用大周期的轮询机制，则该设备必须等到大周期到时才能发送诊断帧
		//注意，如果链路只有一个设备，则IsOffLine真时IsLinkOffline必定也真，因此以下条件必定不会满足，从而每次都会组帧成功。
		CFepModbusDriver* pDrv = dynamic_cast<CFepModbusDriver*>(GetDriver());
		if ( pDrv && IsOffLine() && !pDrv->IsLinkOffline() && ( pDrv->m_nMaxRetryPeriod > 0 && !m_bRetryPeriod ) )
		{
			return 0;
		}
		
		m_bRetryPeriod = false;

		nRet = BuildCheckTimeFrame( pBuf );
		if ( nRet > 0 )		//! 组出了校时帧，则返回帧长
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
			

		//! 根据需要在每一个轮询周期结束后发送特殊查询帧（如SOE等）
		nRet = BuildSpecialReqFrame( pBuf );
		if ( nRet > 0 )		//! 组出了特殊查询帧，则返回帧长
			return nRet;

		nRet = BuildReadSettingGroupFrame( pBuf );
		if ( nRet > 0 )		//! 组出了读取定值组帧，则返回帧长
			return nRet;

		//! 发送当前轮询帧
		return BuildQueryFrame( pBuf );	
	}

	/*!
	*	\brief	构造测试帧
	*
	*	\param	pBuf		帧首地址
	*
	*	\retval	int	帧长度
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
	*	\brief	组校时帧的逻辑实现
	*
	*	\param	buf		帧首地址
	*
	*	\retval	int		帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildCheckTimeFrame( Byte* pbyBuf )
	{
		if ( !pbyBuf )
		{
			return 0;
		}

		if ( HasCheckTime() && NeedCheckTime() )	//! 需要校时
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
				return Build855CheckTimeFrame(pbyBuf);		//! 如果是855协议设备，组855校时帧 Add by: Wangyanchao
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
	*	\brief	组设备校时帧内容
	*
	*	\param	buf		帧首地址
	*
	*	\retval	int		帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildDevCheckTimeFrame( Byte* buf )
	{
		return 0;
	}

	/*!
	*	\brief	组设备相关的特殊轮询帧，如SOE等
	*
	*	\param	buf		帧首地址
	*
	*	\retval	int		帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildSpecialReqFrame( Byte* pbyBuf )
	{
		int iRet = 0;
		if ( !pbyBuf )
		{
			return iRet;
		}
		if ( HasSpecialReq()		//! 有特殊查询帧
			&& GetCurrReq() == 0	//! 常规查询帧（配置文件中写好的）轮询完一个周期
			&& m_nCurSpecialReq < m_nSpecialReqCount	//! 需要发送特殊查询帧
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
	*	\brief	组具体设备相关的特殊轮询帧
	*
	*	\param	buf		帧首地址
	*	\param	nIndex	特殊帧序号（即：第几个特殊查询帧）
	*
	*	\retval	int		帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildDevSplReqFrame( Byte* buf, int nIndex )
	{
		return 0;
	}

	/*!
	*	\brief	写文件记录帧(目前只支持写单个文件请求)
	*
	*	\param	byBuf			帧首地址
	*	\param	strFileReq		文件记录属性
	*	\param	pRegVal			记录数据
	*	\param	bIsBroadcast	是否为广播
	*
	*	\retval	int		帧长
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

		//获得CRC高低顺序
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
	*	\brief	读文件记录帧(目前只支持读单个文件请求)
	*
	*	\param	byBuf			帧首地址
	*	\param	strFileReq		文件记录属性
	*
	*	\retval	int		帧长
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

		//获得CRC高低顺序
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
	*	\brief	组TITANS交流屏或直流屏的校时帧
	*
	*	\param	buf		帧首地址
	*
	*	\retval	int		帧长
	*
	*	\note		参数buf要预先分配空间
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

		//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //补偿的串口时间,毫秒
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
		pReq->YearHiHi = HIBYTE((WORD)(dateTime.year()/100));	//! 年高字高位
		pReq->YearHiLo = LOBYTE((WORD)(dateTime.year()/100));	//! 年高字低位
		pReq->YearLoHi = HIBYTE((WORD)(dateTime.year()%100));	//! 年低字高位
		pReq->YearLoLo = LOBYTE((WORD)(dateTime.year()%100));	//! 年低字低位

		pReq->MonthHi = HIBYTE((WORD)(dateTime.month()));		//! 月
		pReq->MonthLo = LOBYTE((WORD)(dateTime.month()));		//! 月
		pReq->DayHi = HIBYTE((WORD)(dateTime.day()));			//! 日
		pReq->DayLo = LOBYTE((WORD)(dateTime.day()));			//! 日
		pReq->HourHi = HIBYTE((WORD)(dateTime.hour()));			//! 时
		pReq->HourLo = LOBYTE((WORD)(dateTime.hour()));			//! 时
		pReq->MintueHi = HIBYTE((WORD)(dateTime.minute()));		//! 分
		pReq->MintueLo = LOBYTE((WORD)(dateTime.minute()));		//! 分
		pReq->SecondHi = HIBYTE((WORD)(dateTime.second()));		//! 秒
		pReq->SecondLo = LOBYTE((WORD)(dateTime.second()));		//! 秒	

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
	*	\brief	组855协议的校时帧 Add by: Wangyanchao
	*
	*	\param	buf		帧首地址
	*
	*	\retval	OpcUa_Int		帧长
	*
	*	\note		参数buf要预先分配空间
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

		//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //补偿的串口时间,毫秒
		ACE_Time_Value val = ACE_OS::gettimeofday();
		ACE_Date_Time dateTime;
		dateTime.update( val );

		C855CHECKTIMEREQUEST* pReq = (C855CHECKTIMEREQUEST* )buf;
		pReq->SlaveAddress = (Byte)GetAddr();
		pReq->Function = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;		//! 10H
		pReq->StartHi = HIBYTE(m_nCheckTimeAddr - 400001);		//! 起始位高字节
		pReq->StartLo = LOBYTE(m_nCheckTimeAddr - 400001);		//! 起始位低字节
		pReq->PointHi = 0x00;		//! 点数高字节
		pReq->PointLo = 0x06;		//! 点数低字节
		pReq->ByteCount = 0x0C;		//! 字节个数
		pReq->YearHi = HIBYTE((WORD)(dateTime.year()));	//! 年高字高位
		pReq->YearLo = LOBYTE((WORD)(dateTime.year()));	//! 年高字低位

		pReq->MonthHi = HIBYTE((WORD)(dateTime.month()));		//! 月
		pReq->MonthLo = LOBYTE((WORD)(dateTime.month()));		//! 月
		pReq->DayHi = HIBYTE((WORD)(dateTime.day()));			//! 日
		pReq->DayLo = LOBYTE((WORD)(dateTime.day()));			//! 日
		pReq->HourHi = HIBYTE((WORD)(dateTime.hour()));			//! 时
		pReq->HourLo = LOBYTE((WORD)(dateTime.hour()));			//! 时
		pReq->MintueHi = HIBYTE((WORD)(dateTime.minute()));		//! 分
		pReq->MintueLo = LOBYTE((WORD)(dateTime.minute()));		//! 分
		pReq->SecondHi = HIBYTE((WORD)(dateTime.second()));		//! 秒
		pReq->SecondLo = LOBYTE((WORD)(dateTime.second()));		//! 秒	

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

		//int msOffset = FrameLen*(1+8+1)*1000/58400 + 1;	//补偿的串口时间, 毫秒.
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
		//获得CRC高低顺序
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
		//	//! 写日志
		//	sprintf( pchLog,"CFepModbusDevicel::BuildDBMLEsPLCheckTimeFrame 设备%s：校时帧首地址范围与设备使用说明不一致，配置错误！",
		//		GetName().c_str());
		//	pDriver->WriteLogAnyWay( pchLog );
		//	return 0;
		//}

		//兼容标准配置：400001~465536。 贺学文 2013.2.18
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
		pReq->Function			= FORCE_ANA_MULTIPLE_OUTPUT_STATUS;//!功能码？？？
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
		//获得CRC高低顺序
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
	*	\brief	十进制数转换为相应的BCD码
	*
	*	\param	int x		十进制数	
	*
	*	\retval	int		对应的BCD码数值
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
	*	\brief	组轮询帧
	*
	*	\param	buf		帧首地址
	*	\param	buf		召唤帧序号	
	*
	*	\retval	int		帧长
	*
	*	\note		参数buf要预先分配空间
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
			//	snprintf( pchLog, sizeof(pchLog), "CFepModbusDevice::BuildQueryFrame 组帧：%s，帧编号：%d!", szSndPacket.c_str(), m_shCurrReq);
			//}
			//pDriver->WriteLog( pchLog );
		}
		return nFrameLen;
	}

	/*!
	*	\brief	组TITANS直流屏的告警帧
	*
	*	\param	buf		帧首地址
	*
	*	\retval	int		帧长
	*
	*	\note		参数buf要预先分配空间
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
	*	\brief	组触发读取定值组的请求帧
	*
	*	\param	buf		帧首地址
	*
	*	\retval	int		帧长
	*
	*	\note		参数buf要预先分配空间
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
		//!轮询各个定值组帧
		if (m_iCurrSettingGroup >= m_PortCfg.nSettingGroupCount)
		{
			m_iCurrSettingGroup = 0;
		}
		else
		{
			//!检查定值组帧是否被触发
			if (vecSettingGroup[m_iCurrSettingGroup].iReadCount> 0)
			{				
				//!组帧
				vecSettingGroup[m_iCurrSettingGroup].iReadCount--;

				REQUEST Req;
				//获得有效请求。
				Req.SlaveAddress = GetAddr();
				Req.Function	 = vecSettingGroup[m_iCurrSettingGroup].Function;
				Req.StartHi		 = HIBYTE( vecSettingGroup[m_iCurrSettingGroup].StartingAddress );
				Req.StartLo		 = LOBYTE( vecSettingGroup[m_iCurrSettingGroup].StartingAddress );
				Req.PointHi		 = HIBYTE( vecSettingGroup[m_iCurrSettingGroup].PointCount );
				Req.PointLo		 = LOBYTE( vecSettingGroup[m_iCurrSettingGroup].PointCount );

				WORD wCrc = CRC( (Byte *)&Req, 6 );

				//获得CRC高低顺序
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
				//	snprintf( pchLog, sizeof(pchLog), "CFepModbusDevicel::BuildReadSettingGroupFrame 设备名:%s 触发读取定值组，定值组编号:%d,第%d次 !!",
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

			//!如果所有定值组轮训帧被发送，则应清零MACSMB_READSETTINGGROUPALL
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
	*	\brief	DOA遥控帧
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥控标签地址		
	*	\param	byVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildDOAReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! 校验
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

		
		WORD wCtrl = (WORD)(lTagAddr - 1);	//! 获得寄存器地址（此处为控制位）
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
	*	\brief	DOB遥控帧
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥控标签地址		
	*	\param	byVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildDOBReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! 校验
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

		//! 计算寄存器地址
		int lRegAddress = lTagAddr/100 - 1;	//! 25601表示地址40256的第2位。
		int nBitOff	= lTagAddr%100;				//! 
		if ( nBitOff > 15 )
		{
			return 0;
		}
		
		buf[2] = HIBYTE(lRegAddress);
		buf[3] = LOBYTE(lRegAddress);

		WORD RawValue = 0;		//!< 寄存器原值
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

		//! 计算新值
		WORD NewValue  = (RawValue&(~(((WORD)1)<<nBitOff)))|((byVal)<<nBitOff);
		buf[4] = HIBYTE(NewValue);
		buf[5] = LOBYTE(NewValue);

		//! 计算CRC
		WORD wCrc = CRC( buf, 6 );

		//! 获得CRC高低顺序
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

		//! 帧长
		return FrameLen;
	}
	/*!
	*	\brief	DOC遥控帧
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥控标签地址		
	*	\param	byVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildDOCReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! 校验
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
		WORD wNum = 1;			//! 数据区线圈个数
		buf[4] = HIBYTE( wNum );
		buf[5] = LOBYTE( wNum );
		buf[6] = 1;				//! 数据区字节个数
		buf[7] = (Byte)byVal;	//! 如果是多点写,则需要按位!!!

		//计算CRC
		WORD wCrc = CRC( buf, 8 );

		//获得CRC高低顺序
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

		//帧长
		return FrameLen;
	}

	/*!
	*	\brief	DOF遥控帧
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥控标签地址		
	*	\param	byVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildDOFReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! 校验
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

		//! 计算寄存器地址
		int lRegAddress = lTagAddr/100 - 1;	//! 25601表示地址40256的第2位。
		int nBitOff	= lTagAddr%100;				//! 
		if ( nBitOff > 15 )
		{
			return 0;
		}

		buf[2] = HIBYTE(lRegAddress);
		buf[3] = LOBYTE(lRegAddress);

		WORD RawValue = 0;		//!< 寄存器原值

		//! 计算新值
		WORD NewValue  = (RawValue&(~(((WORD)1)<<nBitOff)))|((byVal)<<nBitOff);
		buf[4] = HIBYTE(NewValue);
		buf[5] = LOBYTE(NewValue);

		//! 计算CRC
		WORD wCrc = CRC( buf, 6 );

		//! 获得CRC高低顺序
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

		//! 帧长
		return FrameLen;
	}


	/*!
	*	\brief	DOD遥控帧
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥控标签地址		
	*	\param	byVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildDODReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! 校验
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

		//! 计算寄存器地址
		int lRegAddress = lTagAddr/100 - 1;	//! 25601表示地址40256的第2位。
		int nBitOff	= lTagAddr%100;				//! 
		if ( nBitOff > 15 )
		{
			return 0;
		}

		buf[2] = HIBYTE(lRegAddress);
		buf[3] = LOBYTE(lRegAddress);

		WORD RawValue = 0;		//!< 寄存器原值
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

		//! 计算新值
		WORD NewValue  = (RawValue&(~(((WORD)1)<<nBitOff)))|((byVal)<<nBitOff);

		WORD wNum = 1;	//数据区寄存器个数
		buf[4] = HIBYTE( wNum );
		buf[5] = LOBYTE( wNum );
		buf[6] = 2;	//数据区字节个数
		buf[7] = HIBYTE(NewValue);
		buf[8] = LOBYTE(NewValue);

		//计算CRC
		WORD wCrc = CRC( buf, 9 );

		//获得CRC高低顺序
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

		//帧长
		return FrameLen;
	}

	/*!
	*	\brief	DOG遥控帧
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥控标签地址		
	*	\param	byVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildDOGReqFrame( Byte* buf, int lTagAddr, Byte byVal )
	{
		//! 校验
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

		//! 计算寄存器地址
		int lRegAddress = lTagAddr/100 - 1;	//! 25601表示地址40256的第2位。
		int nBitOff	= lTagAddr%100;				//! 
		if ( nBitOff > 15 )
		{
			return 0;
		}

		buf[2] = HIBYTE(lRegAddress);
		buf[3] = LOBYTE(lRegAddress);

		WORD RawValue = 0;		//!< 寄存器原值

		//! 计算新值
		WORD NewValue  = (RawValue&(~(((WORD)1)<<nBitOff)))|((byVal)<<nBitOff);

		WORD wNum = 1;	//数据区寄存器个数
		buf[4] = HIBYTE( wNum );
		buf[5] = LOBYTE( wNum );
		buf[6] = 2;	//数据区字节个数
		buf[7] = HIBYTE(NewValue);
		buf[8] = LOBYTE(NewValue);

		//计算CRC
		WORD wCrc = CRC( buf, 9 );

		//获得CRC高低顺序
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

		//帧长
		return FrameLen;
	}



	/*!
	*	\brief	DOM遥控帧
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥控标签地址		
	*	\param	byVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildDOMReqFrame( Byte* buf, int lTagAddr, Byte byVal, int nLen)
	{
		//! 校验
		if ( !buf )
		{
			return -1;
		}
		int lRegAddr = lTagAddr/1000 - 1;
		int lCoilCount = lTagAddr%1000;
//		int lDataLen = min( lByteCount, nLen );

		//! 帧长
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
		buf[i++] = (Byte)byVal;	//! 如果是多点写,则需要按位!!!

		WORD wCrc = CRC( buf, nLen - 2 );

		//! 获得CRC高低顺序
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
	*	\brief	组AOA遥调帧，功能码为6，写一个寄存器
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥调标签地址		
	*	\param	fVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildAOAReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! 校验
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

		//! 计算CRC
		WORD wCrc = CRC( buf, 6 );

		//! 获得CRC高低顺序
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

		//! 帧长
		return FrameLen;
	}

	/*!
	*	\brief	组AOB遥调帧，功能码为16，写一个寄存器
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥调标签地址		
	*	\param	fVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildAOBReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! 校验
		if ( !buf )
		{
			return -1;
		}

		int FrameLen = 11;

		buf[0] = (Byte)GetAddr();
		buf[1] = FORCE_ANA_MULTIPLE_OUTPUT_STATUS;
		buf[2] = HIBYTE(lTagAddr - 1);
		buf[3] = LOBYTE(lTagAddr - 1);
		WORD Num = 1;	//!< 数据区寄存器个数
		buf[4] = HIBYTE( Num );
		buf[5] = LOBYTE( Num );
		buf[6] = 2;		//!< 数据区字节个数

		buf[7] = HIBYTE((WORD)fVal);
		buf[8] = LOBYTE((WORD)fVal);

		//! 计算CRC
		WORD wCrc = CRC( buf, 9 );

		//! 获得CRC高低顺序
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

		//! 帧长
		return FrameLen;
	}

	/*!
	*	\brief	组AOD遥调帧，功能码为6，设置寄存器中的N位
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥调标签地址		
	*	\param	fVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildAODReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! 校验
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

		WORD RawValue = 0;		//!< 寄存器原值
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

		//! 计算新值
		WORD wdVal = ( (((WORD)(fVal) & m_wdBitMask[lBitCount-1])) << lBitOff )
			| ( ( RawValue & m_wdBitMask[lBitOff])
			+ ((( RawValue >> lBitOff) & m_wdBitMask[lBitCount-1]) << lBitOff) );

		buf[i++] = HIBYTE(wdVal);
		buf[i++] = LOBYTE(wdVal);

		//! 计算CRC
		WORD wCrc = CRC( buf, 6 );

		//! 获得CRC高低顺序
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

		//! 帧长
		return FrameLen;
	}

	/*!
	*	\brief	组AOE遥调帧，功能码为16，设置寄存器中的N位
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥调标签地址		
	*	\param	fVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildAOEReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! 校验
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
		WORD Num = 1;	//!< 数据区寄存器个数
		buf[4] = HIBYTE( Num );
		buf[5] = LOBYTE( Num );
		buf[6] = 2;		//!< 数据区字节个数

		WORD RawValue = 0;		//!< 寄存器原值
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

		//! 计算新值
		WORD wdVal = ( (((WORD)(fVal) & m_wdBitMask[lBitCount-1])) << lBitOff )
			| ( ( RawValue & m_wdBitMask[lBitOff])
			+ ((( RawValue >> lBitOff) & m_wdBitMask[lBitCount-1]) << lBitOff) );

		//! 寄存器中的值
		buf[7] = HIBYTE(wdVal);
		buf[8] = LOBYTE(wdVal);

		//! 计算CRC
		WORD wCrc = CRC( buf, 9 );

		//! 获得CRC高低顺序
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

		//! 帧长
		return FrameLen;
	}

	/*!
	*	\brief	组AOF遥调帧，功能码为6，设置寄存器中的N位
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥调标签地址		
	*	\param	fVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildAOFReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! 校验
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

		WORD RawValue = 0;		//!< 寄存器原值

		//! 计算新值
		WORD wdVal = ( (((WORD)(fVal) & m_wdBitMask[lBitCount-1])) << lBitOff )
			| ( ( RawValue & m_wdBitMask[lBitOff])
			+ ((( RawValue >> lBitOff) & m_wdBitMask[lBitCount-1]) << lBitOff) );

		buf[i++] = HIBYTE(wdVal);
		buf[i++] = LOBYTE(wdVal);

		//! 计算CRC
		WORD wCrc = CRC( buf, 6 );

		//! 获得CRC高低顺序
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

		//! 帧长
		return FrameLen;
	}

	/*!
	*	\brief	组AOG遥调帧，功能码为16，设置寄存器中的N位
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥调标签地址		
	*	\param	fVal		值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildAOGReqFrame( Byte* buf, int lTagAddr, float fVal )
	{
		//! 校验
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
		WORD Num = 1;	//!< 数据区寄存器个数
		buf[4] = HIBYTE( Num );
		buf[5] = LOBYTE( Num );
		buf[6] = 2;		//!< 数据区字节个数

		WORD RawValue = 0;		//!< 寄存器原值

		//! 计算新值
		WORD wdVal = ( (((WORD)(fVal) & m_wdBitMask[lBitCount-1])) << lBitOff )
			| ( ( RawValue & m_wdBitMask[lBitOff])
			+ ((( RawValue >> lBitOff) & m_wdBitMask[lBitCount-1]) << lBitOff) );

		//! 寄存器中的值
		buf[7] = HIBYTE((WORD)fVal);
		buf[8] = LOBYTE((WORD)fVal);

		//! 计算CRC
		WORD wCrc = CRC( buf, 9 );

		//! 获得CRC高低顺序
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

		//! 帧长
		return FrameLen;
	}

	/*!
	*	\brief	组AOC、AOF遥调帧
	*
	*	\param	buf			帧首地址
	*	\param	lTagAddr	遥调标签地址		
	*	\param	byVal		值
	*	\param	nLen		数据长度
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
	*/
	int CFepModbusDevice::BuildAOCReqFrame(
		Byte* buf, int lTagAddr, Byte* byVal, int nLen )
	{
		//! 校验
		if ( !buf )
		{
			return -1;
		}
		int lRegAddr = lTagAddr/1000 - 1;
		int lByteCount = lTagAddr%1000;
		int lDataLen = min( lByteCount, nLen );

		//! 帧长
		return BuildCtrlMultiRegsFrame( buf, lRegAddr, (lDataLen+1)/2, byVal);
	}

	/*!
	*	\brief	组多点遥调帧
	*
	*	\param	pbyBuf		帧首地址
	*	\param	wRegAddr	寄存器地址		
	*	\param	wRegNo		寄存器个数
	*	\param	wRegVal		待写入寄存器的正常次序的WORD值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
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

		//! 获得CRC高低顺序
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
	*	\brief	组多点遥调帧
	*
	*	\param	pbyBuf		帧首地址
	*	\param	wRegAddr	寄存器地址		
	*	\param	wRegNo		寄存器个数
	*	\param	wRegVal		待写入寄存器的正常次序的WORD值
	*
	*	\retval	int			帧长
	*
	*	\note		参数buf要预先分配空间
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

		//! 获得CRC高低顺序
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
	*	\brief	计算16位数据校验和--CRC
	*
	*	1、	置一16位寄存器位全1；
	*	2、	将报文数据的高位字节异或寄存器的低八位，存入寄存器；
	*	3、	右移寄存器，最高位置0，移出的低八位存入标志位；
	*	4、	如标志位是1，则用A001异或寄存器；如标志位是0，继续步骤3；
	*	5、	重复步骤3和4，直至移位八位；
	*	6、	异或下一位字节与寄存器；
	*	7、	重复步骤3至5，直至所有报文数据均与寄存器异或并移位8次；
	*	8、	此时寄存器中即为CRC校验码，最低位先发送；
	*
	*	\param	datapt		数据起始位置
	*	\param	bytecount	字节个数
	*
	*	\retval	WORD	16位的数据校验和
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
				Reg16>>=1;         //右移一位，置b15=0
				if(flg==1)
					Reg16=Reg16^mval;
			}
		}
		return Reg16;
	}

	/*!
	*	\brief	读取设备配置
	*
	*	\param			
	*
	*	\retval	int	 0表示成功，其他表示失败
	*/
	int CFepModbusDevice::InitCfg()
	{
		//! 特殊选项配置信息
		GetSpecialCfg(m_strCfgFileName);

		//! 端口相关配置信息
		GetPortCfg(m_strCfgFileName);

		//! 获得轮询帧的配置信息
		GetReqCfg(m_strCfgFileName);

		GetSettingGroupCfg(m_strCfgFileName);

		//! 发送控制后的返信请求配置
		GetFollowReqCfg(m_strCfgFileName);

		//! 获得测试帧的配置信息
		GetTestCfg(m_strCfgFileName);

		return 0;
	}

	/*!
	*	\brief	路径是什么类型
	*
	*	\param	strPathName	路径名
	*
	*	\retval	int	值为DIR__TYPE，FILE_TYPE或NONE_TYPE
	*/
	int CFepModbusDevice::PathType( std::string strPathName )
	{
		//! 字符串为空
		if( strPathName.empty() 
			|| strPathName == "."
			|| strPathName == "..")
			return NONE_TYPE;

		if(strPathName[strPathName.length()-1]=='/')
			strPathName[strPathName.length()-1] = '\0';
		//! 是否存在
		ACE_stat fdata;
		if( ACE_OS::stat( strPathName.c_str(), &fdata ) < 0 )
			return NONE_TYPE;
		//! 是否是目录
		if( fdata.st_mode&S_IFDIR )
			return DIR__TYPE;
		//! 是否是目录
		else
			return FILE_TYPE;

		return NONE_TYPE;
	}

	/*!
	*	\brief	根据遥控标签地址获取遥控紧随帧
	*
	*	\param	lTagAddr		遥控标签地址
	*
	*	\retval	struCtrlFollowReq*	遥控紧随帧
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
	*	\brief	处理遥信响应帧，更新相应遥信点的值,按bit更新
	*
	*	\param	UnitType		此函数只处理MACSMB_DIA或MACSMB_DIB
	*	\param	pbyData			数据首地址
	*	\param	nStartAddr		起始标签地址
	*	\param	nPointCount		标签点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
	*/
	int CFepModbusDevice::UpdateDI( int UnitType, 
		Byte* pbyData, int nStartAddr, int nPointCount, bool bFromLocal )
	{
		CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( m_pDriver );
		char pchLog[MAXDEBUGARRAYLEN];		

		//校验
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
		int ByteCounter = nPointCount/8;	//! 完整的字节数
		int rest = nPointCount % 8;			//! 剩余的位数
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
		//! 如果剩余的位数不为0
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
	*	\brief	处理遥测的响应帧，更新相应AI点(寄存器中N位)的值
	*
	*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
	*							此模板函数用于处理标签值为单字的AI点
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
	*/
	template <bool bHighFirst>
	int CFepModbusDevice::UpdateAIinReg( int UnitType,
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal)
	{
		//校验
		if ( !pbyData || nRegCount <= 0)
			return -1;

		//! 根据标签的组态信息更新
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
				//! 计算AI点值
				fTagVal = (float)(WORD)((wdRegVal>>iBitOff) & m_wdBitMask[iBitCount-1]);
				memcpy( pbyTagVal, &fTagVal, sizeof(float) );
				SetVal( UnitType, iTagAddr, pbyTagVal, sizeof(float), 0,bFromLocal);
			}
		}
		//! 释放
		delete []pbyTagVal;

		return 0;
	}

	/*!
	*	\brief	更新其他需要的寄存器的值
	*
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
	*/
	int CFepModbusDevice::UpdateRegister( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal)
	{
		//! 校验
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
					//! 起始地址和长度
					Byte* pbyPos = pbyData + (iRegAddr - nStartAddr - 1)*2;
					memcpy( pbyVal, pbyPos, 2 );
				}
			}
		}

		return 0;
	}

	/*!
	*	\brief	处理遥测的响应帧，更新相应AI点(字符串类型)的值
	*
	*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
	*							此模板函数用于处理标签值为单字的AI点
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
	*/
	template <bool bHighFirst>
	int CFepModbusDevice::UpdateAIString( int UnitType,
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal)
	{
		//! 校验
		if ( !pbyData || nRegCount <= 0 )
			return -1;

		//! 根据标签的组态信息更新
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
				//! 起始地址和长度
				Byte* pbyPos = pbyData + (iRegAddr - nStartAddr - 1)*2;
				int nTagLen = min( iByteCount, 2*nRegCount - (pbyPos - pbyData) );
				int nNeedLen = (int)((nTagLen + 1)/2)*2;
				Byte* pByte = new Byte[nNeedLen];
				memset( pByte, 0, nNeedLen );
				memcpy( pByte, pbyPos, nTagLen );
				//! 如果低位在前，则交换
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
				//! 置AI点值
				SetVal( UnitType, iTagAddr, pByte, nTagLen, 0, bFromLocal );
				delete []pByte;
			}
		}

		return 0;
	}

	/*!
	*	\brief	处理遥测的响应帧，更新相应AI点的值
	*
	*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
	*							此模板函数用于处理标签值为单字的AI点
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
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
	*	\brief	根据遥测响应帧的数据域，更新相应AI点的值，
	*			此模板函数用于处理标签值为双字的AI点
	*
	*	\param	bSigned			有符号——true；无符号——false
	*	\param	bHighFirst		高位在前——true；低位在前——false
	*	\param	bHiWdFirst		高字在前——true；低字在前——false
	*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
	*							此模板函数用于处理标签值为双字的AI点
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
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

			//! 根据不同的数据类型赋值
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
	*	\brief	处理遥测的响应帧，更新相应AI点的值
	*			此函数用于处理无特定规律的AI点（上面两个模板函数处理不了的）
	*
	*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
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

					//! 添加处理--------------------------

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

					//! 添加处理--------------------------

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

					//! 添加处理--------------------------

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
	*	\brief	更新Input Register中的数据（功能码为4）
	*
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	void
	*/
	int CFepModbusDevice::UpdateInputAI( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		int nRet = 0;
		//! 如果有MACSMB_DID类型的标签
		CIOCfgUnitType* pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DID);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			nRet = UpdateDIinReg<false>(
				MACSMB_DID, pbyData, nStartAddr, nRegCount, bFromLocal );

		//! 如果有MACSMB_DIF类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIF);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			nRet = UpdateDIinReg<true>(
				MACSMB_DIF, pbyData, nStartAddr, nRegCount, bFromLocal );

		//! 如果有MACSMB_AIAA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIAA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
		{
			UpdateWordAI<false,false>( 
				MACSMB_AIAA, pbyData, nStartAddr, nRegCount, bFromLocal);
		}

		//! 如果有MACSMB_AIAB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIAB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<false,true>( 
				MACSMB_AIAB, pbyData, nStartAddr, nRegCount, bFromLocal);						

		//! 如果有MACSMB_AICA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AICA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<true,false>( 
				MACSMB_AICA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AICB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AICB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<true,true>( 
				MACSMB_AICB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIEA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIEA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,false,false>( 
				MACSMB_AIEA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIEB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIEB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,false>( 
				MACSMB_AIEB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIEC类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIEC);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,false,true>( 
				MACSMB_AIEC, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIED类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIED);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,true>( 
				MACSMB_AIED, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIED类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DWED);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,true>( 
			MACSMB_DWED, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIGA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIGA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,false,false>( 
				MACSMB_AIGA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIGB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIGB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,true,false>( 
				MACSMB_AIGB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIGC类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIGC);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,false,true>( 
				MACSMB_AIGC, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIGD类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIGD);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,true,true>( 
				MACSMB_AIGD, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AII类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AII);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AII, pbyData, nStartAddr, nRegCount, bFromLocal);
		//! 如果有MACSMB_AIK类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIK);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AIK, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AITA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AISA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIinReg<false>(
				MACSMB_AISA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AITB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AISB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIinReg<true>(
				MACSMB_AISB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIMA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIMA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIString<false>(
				MACSMB_AIMA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIMB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIMB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIString<true>(
				MACSMB_AIMB, pbyData, nStartAddr, nRegCount, bFromLocal);

		return nRet;
	}

	/*!
	*	\brief	更新Holding Register中的数据（功能码为3）
	*
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	void
	*/
	int CFepModbusDevice::UpdateHoldingAI( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		int nRet = 0;
		//! 如果有MACSMB_DIE类型的标签
		CIOCfgUnitType* pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIE);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			nRet = UpdateDIinReg<false>(
				MACSMB_DIE, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_DIG类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DIG);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			nRet = UpdateDIinReg<true>(
				MACSMB_DIG, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIBA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIBA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<false,false>( 
				MACSMB_AIBA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIBB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIBB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<false,true>( 
				MACSMB_AIBB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIDA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIDA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<true,false>( 
				MACSMB_AIDA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIDB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIDB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateWordAI<true,true>( 
				MACSMB_AIDB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIFA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIFA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,false,false>( 
				MACSMB_AIFA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIFB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIFB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,false>( 
				MACSMB_AIFB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIFC类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIFC);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,false,true>( 
				MACSMB_AIFC, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIFD类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIFD);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,true>( 
				MACSMB_AIFD, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_DWFD类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_DWFD);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<false,true,true>( 
			MACSMB_DWFD, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIHA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIHA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,false,false>( 
				MACSMB_AIHA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIHB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIHB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,true,false>( 
				MACSMB_AIHB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIHC类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIHC);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,false,true>( 
				MACSMB_AIHC, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AIHD类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIHD);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateDWordAI<true,true,true>( 
				MACSMB_AIHD, pbyData, nStartAddr, nRegCount, bFromLocal);	

		//! 如果有MACSMB_AIJ类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIJ);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AIJ, pbyData, nStartAddr, nRegCount, bFromLocal);
		//! 如果有MACSMB_AIL类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIL);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AIL, pbyData, nStartAddr, nRegCount, bFromLocal);
		//! 如果有MACSMB_AIU类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIU);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AIU, pbyData, nStartAddr, nRegCount, bFromLocal);
		//!如果有MACSMB_AIV类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIV);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI( MACSMB_AIV, pbyData, nStartAddr, nRegCount, bFromLocal);
		//! 如果有MACSMB_AITA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AITA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIinReg<false>(
				MACSMB_AITA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AITB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AITB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIinReg<true>(
				MACSMB_AITB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AINA类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AINA);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIString<false>(
				MACSMB_AINA, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 如果有MACSMB_AINB类型的标签
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AINB);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateAIString<true>(
				MACSMB_AINB, pbyData, nStartAddr, nRegCount, bFromLocal);

		//!如果有MACSMB_AIFF类型的标签（施耐德电器400V特殊应用）
		pUnitType = GetIOCfgUnit()->GetUnitTypeByID(MACSMB_AIFF);
		if ( pUnitType && pUnitType->GetTagCount() > 0 )
			UpdateOtherAI(MACSMB_AIFF, pbyData, nStartAddr, nRegCount, bFromLocal);

		//! 更新其他需要的寄存器的值
		if ( m_mapRegister.size() > 0 )
			UpdateRegister( pbyData, nStartAddr, nRegCount, bFromLocal );

		return nRet;
	}

	/*!
	*	\brief	根据Holding Register中的数据更新点值之前需要进行的特殊处理
	*			（功能码为3）
	*	\param	UnitType
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
	*/
	int CFepModbusDevice::BeforeUpdateHoldingAI( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		//! 根据设备类型进行的特殊处理
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
	*	\brief	根据Input Register中的数据更新点值之前需要进行的特殊处理
	*			（功能码为4）
	*	\param	UnitType
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
	*/
	int CFepModbusDevice::BeforeUpdateInputAI( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		return 0;
	}

	/*!
	*	\brief	处理文件读操作返回帧的数据
	*
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
	*/
	int CFepModbusDevice::UpdateFileInfo( 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		return 0;
	}

	/*!
	*	\brief	根据AI响应帧的数据域，更新其中包含的DI点的值
	*			（UnitType为MACSMB_DID或MACSMB_DIE）
	*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
	*							此函数只处理MACSMB_DID或MACSMB_DIE
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
	*/
	template < bool bHighFirst>
	int CFepModbusDevice::UpdateDIinReg( int UnitType,
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal)
	{
		CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( m_pDriver );
		char pchLog[MAXDEBUGARRAYLEN];

		//! 校验
		if ( UnitType != MACSMB_DID && UnitType != MACSMB_DIE
			&& UnitType != MACSMB_DIF && UnitType != MACSMB_DIG)
		{
			return -1;
		}
		if ( !pbyData || nRegCount <= 0)
			return -2;

		int nRet = 0;

		//! 根据标签的组态信息更新标签值
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
				//! 变更日志
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
	*	\brief	根据AI响应帧的数据域，更新深圳2号线TINTANS直流屏的SOE标签值
	*			并发送SOE（UnitType为MACSMB_TEPIFSOE）
	*
	*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
	*							此函数只处理MACSMB_TEPIFSOE
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
	*/
	int CFepModbusDevice::UpdateTEPIFSOE( int UnitType, 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		//! 校验
		if ( UnitType != MACSMB_TEPIFSOE )
		{
			return -1;
		}
		if ( !pbyData || nRegCount <= 0)
			return -2;

		//! 数据不够，则不处理
		if( nRegCount < 12)
			return -3;

		WORD wFlag = MAKEWORD( pbyData[1], pbyData[0] );
		//! 数据无效，则不处理
		if(wFlag == 0)
			return 0;

		//! 置标签值并发送SOE
		WORD Addr1 = MAKEWORD( pbyData[17], pbyData[16] );

		WORD Addr2 = MAKEWORD( pbyData[19], pbyData[18] );
		WORD Addr3 = MAKEWORD( pbyData[21], pbyData[20] );
		int lAddr = Addr1*1000000 + Addr2*1000 + Addr3;		//! 计算标签地址
		WORD RawVal = MAKEWORD( pbyData[23], pbyData[22] );		//! 标签值

		//! 计算时间
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

		//! 置报警点值
		SetVal( MACSMB_TEPIFSOE, lAddr, (Byte*)&RawVal , 2, nTime, true );
		FieldPointType* pIOChannel = GetTagByIndex(MACSMB_TEPIFSOE, lAddr)->GetFirst();
		if (pIOChannel)
		{
			//CDBFrameAccess::PostSOE(pIOChannel, GetDescription(), EXTERNAL_SOE, (int)RawVal, nTime);
		}

		return 0;
	}

	
	/*!
	*	\brief	根据AI响应帧的数据域， NEXUS1272电度表功率因数
	*			（UnitType为MACSMB_PowFactor）
	*
	*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
	*							此函数只处理MACSMB_PowFactor
	*	\param	pbyData			数据首地址,注意不是帧首地址
	*	\param	nStartAddr		起始寄存器地址
	*	\param	nRegCount		寄存器点数
	*	\param	bFromLocal		是否本地发送帧的响应
	*
	*	\retval	int			0表示成功，否则表示失败
	*/
	int CFepModbusDevice::Update_PowFactor( int UnitType, 
		Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal )
	{
		//! 校验
		if ( UnitType != MACSMB_PowFactor )
		{
			return -1;
		}
		if ( !pbyData || nRegCount <= 0)
			return -2;
		//!pbyTemp指针指向数据区
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
	*	\brief	把时间转换成毫秒数
	*
	*	\param	strTime	要转换的时间	
	*
	*	\retval	MACS_INT64	毫秒数，为-1则表示转换失败
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

		//! 计算总天数
		lSumDay = 
			(int)(strTime.lYear-1970) * 365 +(int)(strTime.lYear-1-1968) / 4;

		for( i = 0; i < strTime.sMonth - 1; i ++ )
			lSumDay += sMonthDay[ i ];

		if ( ( strTime.sMonth > 2 ) && ( ( strTime.lYear - 1968 ) % 4 == 0 ) )
			lSumDay += 1; 

		lSumDay += strTime.sDay -1 ;

		////! 得到相对于1970.1.1的总秒数
		//long nSec = (long)lSumDay * 24 * 60 * 60
		//	+ (long)strTime.sHour * 60 * 60
		//	+ (long)strTime.sMinute * 60
		//	+ (long)strTime.sSecond
		//	+ (long)strTime.sTimeZone * 60 * 60;

		////! 总毫秒数
		//long nTime = nSec * 1000 + strTime.sMSecond;
		long nSec = (long)lSumDay * 24 * 60 * 60
			+ (long)strTime.sHour * 60 * 60
			+ (long)strTime.sMinute * 60
			+ (long)strTime.sSecond
			+ (long)strTime.sTimeZone * 60 * 60;

		//! 总毫秒数
		//long nTime = nSec * 1000 + strTime.sMSecond;

		return nSec;
	}

	int CFepModbusDevice::BuildSplCtrlFrame(Byte* pbyData,const tIOCtrlRemoteCmd& tCtrlRemoteCmd,float& fCmdVal)
	{
		return 0;
	}
	/*!
	*	\brief	获得特殊设备配置信息
	*
	*	\param	file_name		配置文件路径名
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

		//! 校时周期
		m_nCheckTimeCycl = GetPrivateInifileInt("SPECIAL", "CHECKTIMECYCLE", 60, file_name.c_str());
		if ( m_nCheckTimeCycl < 5 )
		{
			m_nCheckTimeCycl = 5;
		}

		//! 校时时间点(小时) Add by: Wangyanchao
		m_nCheckTimeHour = GetPrivateInifileInt("SPECIAL",	"CHECKTIMEHOUR", 1, file_name.c_str());

		//! 校时时间点(分钟) Add by: Wangyanchao
		m_nCheckTimeMin = GetPrivateInifileInt("SPECIAL",	"CHECKTIMEMINUTE", 30, file_name.c_str());

		//! 校时起始地址
		m_nCheckTimeAddr = GetPrivateInifileInt("SPECIAL", "CHECKTIMEADDR", 400020, file_name.c_str());

		//! 对时是否使用广播命令
		nVal = GetPrivateInifileInt("SPECIAL", "BROADCAST", 0, file_name.c_str());
		if ( nVal == 1 )
		{
			m_bBroadcast = true;
		}
		else
		{
			//点对点模式对时时，对时周期最小为5分钟，避免影响数据轮询效率
			if ( m_nCheckTimeCycl < 5 )
			{
				m_nCheckTimeCycl = 5;
			}
			m_bBroadcast = false;
		}
	}

	/*!
	*	\brief	获得端口相关配置信息
	*
	*	\param	file_name		配置文件路径名
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

		//! 离线计数器
		m_nMaxOfflineCount = pDrv->GetMaxOffLineCount();
		if ( m_nMaxOfflineCount < 1 )
		{
			m_nMaxOfflineCount = 3;
		}

		//! 0---关闭端口; 1---启动端口。默认1。
		int nVal = GetPrivateInifileInt("PORT", "Enable", 1, file_name.c_str());
		if ( nVal == 0 )
		{
			m_PortCfg.bEnable = false;
		}
		else
		{
			m_PortCfg.bEnable = true;
		}

		//! 巡检周期，毫秒，默认100。
		m_PortCfg.nSndDelay = GetPrivateInifileInt("PORT", "SndDelay", 100, file_name.c_str());
		if ( m_PortCfg.nSndDelay < 0  )
			m_PortCfg.nSndDelay = 100;

		//! 超时，毫秒，默认1000。
		m_PortCfg.nTimeOut = pDrv->GetRcvTimeOut();
		if ( m_PortCfg.nTimeOut <= 0  )
			m_PortCfg.nTimeOut = 1000;

		//! 端口的请求帧个数
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

		//! 0---CRC低字节在前; 1-CRC高字节在前。默认0
		m_PortCfg.nCRCOrder = GetPrivateInifileInt("PORT", "CRCOrder", 0, file_name.c_str());
		if ( m_PortCfg.nCRCOrder != 0 && m_PortCfg.nCRCOrder != 1 )
			m_PortCfg.nCRCOrder = 0;
	}

	/*!
	*	\brief	获得轮询帧信息
	*
	*	\param	file_name		配置文件路径名
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

			//! 0---使请求失效; 1---使请求有效。默认1。
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

			//获得有效请求。
			Req.SlaveAddress = GetAddr();
			Req.Function	 = RequestCfg.Function;
			Req.StartHi		 = HIBYTE( RequestCfg.StartingAddress );
			Req.StartLo		 = LOBYTE( RequestCfg.StartingAddress );
			Req.PointHi		 = HIBYTE( RequestCfg.PointCount );
			Req.PointLo		 = LOBYTE( RequestCfg.PointCount );

			WORD wCrc = CRC( (Byte *)&Req, 6 );

			//获得CRC高低顺序
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
		
		//! 获得驱动对象
		CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( GetDriver() );
		//! 记录日志
		if ( pDriver )
		{
			char pchLog[MAXDEBUGARRAYLEN] = {0};
			sprintf( pchLog, "CFepModbusDevice::GetReqCfg Device:%s, CfgFileName:%s, RequestFrameCount:%d!",
				GetName().c_str(), file_name.c_str(), Request.size() );
			pDriver->WriteLogAnyWay( pchLog );
		}
	}

	
	/*!
	*	\brief	获得读定值组帧信息
	*
	*	\param	file_name		配置文件路径名
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
		//! 获得驱动对象
		CFepModbusDriver* pDriver = dynamic_cast<CFepModbusDriver*>( GetDriver() );
		//! 记录日志
		if ( pDriver )
		{
			pDriver->WriteLogAnyWay( pchLog );
		}
	}

	
	/*!
	*	\brief	获得测试帧信息
	*
	*	\param	file_name		配置文件路径名
	*
	*	\retval	void
	*/
	void CFepModbusDevice::GetTestCfg(std::string file_name)
	{
		char cAppName[]="TESTREQ";

		REQUEST_CFG RequestCfg;
		//! 0---使请求失效; 1---使请求有效。默认0。
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

		//! 获得有效请求
		m_reqTest.SlaveAddress = GetAddr();
		m_reqTest.Function	 = RequestCfg.Function;
		m_reqTest.StartHi		 = HIBYTE( RequestCfg.StartingAddress );
		m_reqTest.StartLo		 = LOBYTE( RequestCfg.StartingAddress );
		m_reqTest.PointHi		 = HIBYTE( RequestCfg.PointCount );
		m_reqTest.PointLo		 = LOBYTE( RequestCfg.PointCount );

		WORD wCrc = CRC( (Byte *)&m_reqTest, 6 );
		//! 获得CRC高低顺序
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
	*	\brief	获得遥控伴随帧配置信息
	*
	*	\param	file_name		配置文件路径名
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
	*	\brief	获取标签的整型值，并比较是否与nVal相等，相等则返回false，否则返回true；此处只考虑Byte类型数据，其他类型数据调用此接口会异常
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

						//! 根据nVal得到新的标签值
						//! 量程转换
						float fRawZero = pFieldPoint->getRawZero();
						float fRawFull = pFieldPoint->getRawFull();
						float fEngZero = pFieldPoint->getEngZero();
						float fEngFull = pFieldPoint->getEngFull();
						if ( fabs( fRawFull - fRawZero ) > 0.001 && fabs( fEngFull - fEngZero ) > 0.001  )
						{
							//! 经过量程转换的值
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

		//兼容标准配置：400001~465536。 贺学文 2013.2.18
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
		pReq->Function			= FORCE_ANA_MULTIPLE_OUTPUT_STATUS;//!功能码？？？
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
		//获得CRC高低顺序
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
		//兼容标准配置：400001~465536。 贺学文 2013.2.18
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
		pReq->SlaveAddress		= (Byte)GetAddr();  //!本设备仅支持单步对时
		pReq->Function			= FORCE_ANA_MULTIPLE_OUTPUT_STATUS;//!功能码
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
		//获得CRC高低顺序
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
		//! 参数校验
		if ( buf == NULL )
		{
			return 0;
		}
		if ( m_nCheckTimeAddr < 400001 || m_nCheckTimeAddr > 465536 )
		{
			return 0;
		}
		//! 帧长
		int nFrameLen = sizeof(LDCHECKTIMEREQUEST);

		//! 获得系统时间
		//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //补偿的串口时间,毫秒
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
		//pReq->Year = byBCD;								//! 年
		//CIOSwitchData::DectoBCD( dateTime.month(),  &byBCD, 1);
		//pReq->Month = byBCD;							//! 月
		//CIOSwitchData::DectoBCD( dateTime.day(),  &byBCD, 1);
		//pReq->Day = byBCD;								//! 日
		//CIOSwitchData::DectoBCD( dateTime.hour(),  &byBCD, 1);
		//pReq->Hour = byBCD;								//! 时
		//CIOSwitchData::DectoBCD( dateTime.minute(),  &byBCD, 1);
		//pReq->Minute = byBCD;							//! 分
		//CIOSwitchData::DectoBCD( dateTime.second(),  &byBCD, 1);
		//pReq->Second = byBCD;							//! 秒

		pReq->MsecHi = HIBYTE((WORD)(dateTime.microsec()/1000));		//! 毫秒高位
		pReq->MsecLo = HIBYTE((WORD)(dateTime.microsec()/1000));		//! 毫秒低位

		//! 计算校验码
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
		int iRet = 0;	//!返回值,初始化为0
		if(0==pbyBuf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!指针为空或校时地址超范围时，返回值置为-1
		}
		//!主工作流程
		else
		{
			int FrameLen = sizeof(SEPCOSTIMEFRAME);
			const int  POINTCOUNT	= 0x09;
			const int  DATALENGTH	= 18;


			int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;			//补偿的串口时间, 毫秒.
			ACE_Time_Value curValue = ACE_OS::gettimeofday();		//获取当前时间
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
		int iRet = 0;	//!返回值,初始化为0
		if(0==pbyBuf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!指针为空或校时地址超范围时，返回值置为-1
		}
		//!主工作流程
		else
		{
			int nFrameLen = sizeof(DQPLCCHECKTIMEREQUEST);

			//! 获得系统时间
			//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //补偿的串口时间,毫秒
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
			//pReq->Year = byBCD;								//! 年
			//CIOSwitchData::DectoBCD( dateTime.month(),  &byBCD, 1);
			//pReq->Month = byBCD;							//! 月
			//CIOSwitchData::DectoBCD( dateTime.day(),  &byBCD, 1);
			//pReq->Day = byBCD;								//! 日
			//CIOSwitchData::DectoBCD( dateTime.hour(),  &byBCD, 1);
			//pReq->Hour = byBCD;								//! 时
			//CIOSwitchData::DectoBCD( dateTime.minute(),  &byBCD, 1);
			//pReq->Minute = byBCD;							//! 分
			//CIOSwitchData::DectoBCD( dateTime.second(),  &byBCD, 1);
			//pReq->Second = byBCD;							//! 秒

			//pReq->MsecHi = HIBYTE((WORD)(dateTime.microsec()/1000));		//! 毫秒高位
			//pReq->MsecLo = LOBYTE((WORD)(dateTime.microsec()/1000));		//! 毫秒低位
			pReq->MsecHi = 0;		//! 毫秒高位
			pReq->MsecLo = 0;		//! 毫秒低位

			pReq->m_byValidate1		= 0;
			pReq->m_byValidate2		= 1;

			//! 计算校验码
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
		int iRet = 0;	//!返回值,初始化为0
		if(0==pbyBuf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!指针为空或校时地址超范围时，返回值置为-1
		}
		//!主工作流程
		else
		{
			int nFrameLen = sizeof(DLLSJZLPCHECKTIMEREQUEST);

			//! 获得系统时间
			//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //补偿的串口时间,毫秒
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
			//pReq->YearHi = byBCD;								//! 年
			//CIOSwitchData::DectoBCD( dateTime.year()%100,  &byBCD, 1);
			//pReq->YearLo = byBCD;								//! 年
			//CIOSwitchData::DectoBCD( dateTime.month(),  &byBCD, 1);
			//pReq->Month = byBCD;							//! 月
			//CIOSwitchData::DectoBCD( dateTime.day(),  &byBCD, 1);
			//pReq->Day = byBCD;								//! 日
			//CIOSwitchData::DectoBCD( dateTime.hour(),  &byBCD, 1);
			//pReq->Hour = byBCD;								//! 时
			//CIOSwitchData::DectoBCD( dateTime.minute(),  &byBCD, 1);
			pReq->Minute = byBCD;							//! 分

			WORD wdMsec = dateTime.second()*1000 + dateTime.microsec()/1000;

			pReq->MsecHi = HIBYTE(wdMsec);		//! 毫秒高位
			pReq->MsecLo = LOBYTE(wdMsec);		//! 毫秒低位

			//! 计算校验码
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
		//! 参数校验
		if ( buf == NULL )
		{
			return 0;
		}
		if ( m_nCheckTimeAddr < 400001 || m_nCheckTimeAddr > 465536 )
		{
			return 0;
		}
		//! 帧长
		int nFrameLen = sizeof(ZSZDCHECKTIMEREQUEST);

		//! 获得系统时间
		//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //补偿的串口时间,毫秒
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

		pReq->Year = dateTime.year()-2000;						//! 年
		pReq->Month = dateTime.month();							//! 月
		pReq->Day = dateTime.day();								//! 日
		pReq->Hour = dateTime.hour();							//! 时
		pReq->Minute = dateTime.minute();						//! 分
		pReq->Second = dateTime.second();						//! 秒

		//! 计算校验码
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
		int iRet = 0;	//!返回值,初始化为0
		if(0==buf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!指针为空或校时地址超范围时，返回值置为-1
		}
		//!主工作流程
		else
		{
			int nFrameLen = sizeof(DL_SIEMENSS73_CHECKTIMEREQUEST);

			//! 获得系统时间
			//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //补偿的串口时间,毫秒
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
			//pReq->YearHi = byBCD;								//! 年
			//CIOSwitchData::DectoBCD( dateTime.year()%100,  &byBCD, 1);
			//pReq->YearLo = byBCD;								//! 年
			//CIOSwitchData::DectoBCD( dateTime.month(),  &byBCD, 1);
			//pReq->Month = byBCD;							//! 月
			//CIOSwitchData::DectoBCD( dateTime.day(),  &byBCD, 1);
			//pReq->Day = byBCD;								//! 日
			//CIOSwitchData::DectoBCD( dateTime.hour(),  &byBCD, 1);
			//pReq->Hour = byBCD;								//! 时
			//CIOSwitchData::DectoBCD( dateTime.minute(),  &byBCD, 1);
			//pReq->Minute = byBCD;							//! 分
			//CIOSwitchData::DectoBCD( dateTime.second(),  &byBCD, 1);
			//pReq->Second = byBCD;							//! 秒

			//WORD wdMsec = dateTime.microsec()/1000;
			//CIOSwitchData::DectoBCD( wdMsec/10,  &byBCD, 1);
			//pReq->MsecHi = byBCD;
			//CIOSwitchData::DectoBCD( wdMsec%10,  &byBCD, 1);
			pReq->MsecLo = byBCD;

			pReq->Rev	 = 0;

			//! 计算校验码
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
		int iRet = 0;	//!返回值,初始化为0
		if(0==buf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!指针为空或校时地址超范围时，返回值置为-1
		}
		//!主工作流程
		else
		{
			int nFrameLen = sizeof(DL_UMLESSC_CHECKTIMEREQUEST);

			//! 获得系统时间
			//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //补偿的串口时间,毫秒
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
			//pReq->YearHi = byBCD;								//! 年
			//CIOSwitchData::DectoBCD( dateTime.year()%100,  &byBCD, 1);
			//pReq->YearLo = byBCD;								//! 年
			//CIOSwitchData::DectoBCD( dateTime.month(),  &byBCD, 1);
			//pReq->Month = byBCD;							//! 月
			//CIOSwitchData::DectoBCD( dateTime.day(),  &byBCD, 1);
			//pReq->Day = byBCD;								//! 日
			//CIOSwitchData::DectoBCD( dateTime.hour(),  &byBCD, 1);
			//pReq->Hour = byBCD;								//! 时
			//CIOSwitchData::DectoBCD( dateTime.minute(),  &byBCD, 1);
			//pReq->Minute = byBCD;							//! 分
			//CIOSwitchData::DectoBCD( dateTime.second(),  &byBCD, 1);
			//pReq->Second = byBCD;							//! 秒

			//WORD wdMsec = dateTime.microsec()/1000;
			//CIOSwitchData::DectoBCD( wdMsec/10,  &byBCD, 1);
			pReq->CendSecond = byBCD;

			//! 计算校验码
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
		int iRet = 0;	//!返回值,初始化为0
		if(0==buf||m_nCheckTimeAddr<400001||m_nCheckTimeAddr>465536)
		{
			iRet = -1;	//!指针为空或校时地址超范围时，返回值置为-1
		}
		//!主工作流程
		else
		{
			int FrameLen = sizeof(DL_SIEMENSS72_CHECKTIMEREQUEST);
			const int  POINTCOUNT	= 6;
			const int  DATALENGTH	= 12;


			//! 获得系统时间
			//int msOffset = FrameLen*(1+8+1)*1000/9600 + 1;  //补偿的串口时间,毫秒
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

			//获取系统当前时间
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

		//以下请求既可以是请求，也可以是响应：
		//XX 01 03 XX 00 YY XX XX 
		//XX 02 03 XX 00 YY XX XX 
		//其中当YY 范围为17~24时，既可以请求，也可以是响应。
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
