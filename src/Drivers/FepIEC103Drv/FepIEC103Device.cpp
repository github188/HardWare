/*!
*	\file	FepIEC103Device.cpp
*
*	\brief	103主站设备类源文件
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Device.cpp 4     11-02-24 11:25a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-02-24 11:25a $
*	$Revision: 4 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#include "FepIEC103Device.h"
#include "PublicFileH.h"
#include "IEC103.h"
#include <math.h>
#include "FepIEC103Prtcl.h"
#include "FepIEC103Drv.h"
#include "PRDFile103Dev.h"
namespace MACS_SCADA_SUD
{
	std::string CFepIEC103Device::s_ClassName("CFepIEC103Device");
	void DOCTimer(void* pParent)
	{
		CFepIEC103Device* pDevice = (CFepIEC103Device*)pParent;
		if ( pDevice == NULL )
		{
			return;
		}
		//pDevice->m_pDOCTimer->KillTimer();
		pDevice->m_pDOCTimer->KillTimer();
		OpcUa_Byte byTemp = 0;
		pDevice->SetVal(MacsDIS103_DIC, pDevice->m_iInDOIndex, &byTemp,1,0, true);

	}

	void FaultTimeOutProc(void* pParent)
	{
		CFepIEC103Device* pDevice = (CFepIEC103Device*)pParent;
		if ( pDevice == NULL )
		{
			return;
		}

		pDevice->m_pFaultTimeOutTimer->KillTimer();

		const OpcUa_Int32 nMaxCount = (pDevice->m_FaultTimeOut-FAULT_WAVE_CYCLE+1)/FAULT_WAVE_CYCLE;
		if ( pDevice->m_bIsRcvingFaultWave == false )	//当前没有在做录播处理
		{
			pDevice->m_nFautTimeOutCount = 0;
		}
		else if ( pDevice->m_nFautTimeOutCount++ > nMaxCount )
		{
			pDevice->m_nFautTimeOutCount = 0;
			pDevice->m_bNeedInit = true;	//复位链路
			pDevice->m_bNew	= true;			//当收到ASDU5后需要重新激活总召唤及对时。

			pDevice->m_bIsRcvingFaultWave = false;
		}
	
		pDevice->m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_CYCLE );

	}

	CFepIEC103Device::CFepIEC103Device( )
	{
		m_bRetryPeriod = true;

		m_byTxTimes = 0;		
		m_byRxTimes = 0;
		m_bIsRcvingFaultWave = false;
		m_iInDOIndex = 0;
		m_iInDOMIndex = 0;
        //设置FCB与FCV，将它们都置为false
		m_bFCB = false;
		m_bFCV = false;

		m_nWatchDogCounter = 0;

		m_bSendRestCU = false;	//Del by hxw
		//	m_bSendRestCU = true;		//Add by hxw
		m_bReq1LevelData = false;
		m_bReq2LevelData = false;
		m_nFautTimeOutCount = 0;
		m_nSendResetCUCounter = 0;

		//与控制系统及设备初始化相关的标志位
		m_bNeedInit = true;;

		//与校时相关的标志位
		m_bNeedCheckTime = false;

		//总查询标志位
		m_bNeedASDU7GI = false;
		m_bNeedASDU21GI = false;
		m_iGINo = 1;

		m_IsSOE = false;

		//故障选择标志位
		m_bFaultSel = false;
		//故障个数
		m_byFaultNum = 0;
		//信息序号
		m_byRII = 0;

		m_byRcvFaultNum = 0;

		m_FaultDataVec.clear();

		//故障序号
		m_wFAN = 0;

		m_byNOC = 0;
		m_NOE = 0;
		m_sINT = 0;

		m_bReqDisData = false;
		m_bReqStatusAlter = false;

		m_bSendDisDataAck = false;
		m_byDisDataAckTOO = 0;

		m_byNOT = 0;

		m_byACC = 0;
		m_bChannelReady = false;

		m_sNDV = 0;
		m_sNFE = 0;

		m_bChannelEnd = false;

		m_DISOEMap.clear();
		m_DIDevSOEMap.clear();
		m_DOSOEMap.clear();
		m_DOWithSel.clear();
		m_statusMap.clear();
		m_DIPointMap.clear();
		m_DOPointMap.clear();		

		m_bInDO = false;	
		m_pDOCTimer = new CIOTimer( DOCTimer, this, true );
		m_pFaultTimeOutTimer = new CIOTimer( FaultTimeOutProc, this ,true );
		m_bIsRcvingFaultWave = false;

		m_pFaultTimeOutTimer->SetTimer( FAULT_WAVE_CYCLE );
		m_byStandyOffCounter = 0;

		m_iASDU7ReadGroupPeriod = 0;
		m_iASDU21ReadGroupPeriod = 0;			
		m_vGroupNo.clear();
		m_iGroupIndex = 0;
		m_bConfirmed = false;

	}


	
	CFepIEC103Device::~CFepIEC103Device()
	{
		if ( m_pDOCTimer )
		{
			delete m_pDOCTimer;
			m_pDOCTimer = NULL;
		}
		if(m_pPRDFile)
		{
			delete m_pPRDFile;
			m_pPRDFile = NULL;
		}
		if(m_pFaultTimeOutTimer)
		{
			delete m_pFaultTimeOutTimer;
			m_pFaultTimeOutTimer = NULL;
		}
	}

	/*!
	*	\brief	初始化
	*
	*	\retval	0为成功，否则失败
	*/
	OpcUa_Int32 CFepIEC103Device::Init()
	{
		OpcUa_Int32 ret = CIODeviceBase::Init();
		if (ret != 0)
		{
			return ret;
		}

		std::string strPrtclName = GetDriver()->GetCfgPrtclName();
		CFepIEC103Drv* pIEC103Drv = dynamic_cast<CFepIEC103Drv*>(m_pDriver);
		if ( pIEC103Drv )
		{
			m_pPRDFile = new CPRDFile103Dev(strPrtclName.c_str(), GetName().c_str(),pIEC103Drv);
			m_pPRDFile->m_pLog = pIEC103Drv->GetLog();
			m_pPRDFile->Init();
		}
		//! 获取设备配置信息
		InitTagSOEMap();
		return 0;
	}

	// **********************************************************************************************
	// 编 写 者: ZhouJun        函数创建时间: 2006-10-26 
	// 函数名称: CFepIEC103Device::GetBitVal
	// 函数功能: 获取字节某位的值
	// 算法思想: 借助掩码
	// 调用模块: 
	// 返 回 值: OpcUa_Byte : 位值
	// 参数说明: 
	//     OpcUa_Byte byIn: 字节
	//      OpcUa_Int32 iBit: 位
	// **********************************************************************************************
	OpcUa_Byte CFepIEC103Device::GetBitVal( OpcUa_Byte byIn, OpcUa_Int32 iBit )
	{
		OpcUa_Byte byTemp = byIn;
		byTemp = (byTemp<<(7-iBit));
		byTemp = (byTemp>>7);
		return byTemp;
	}


	// **********************************************************************************************
	// 编 写 者: ZhouJun        函数创建时间: 2006-10-26 
	// 函数名称: CFepIEC103Device::SetBitVal
	// 函数功能: 设置某字节某位的值
	// 算法思想: 以下实现算法可能不对，需要再改
	// 调用模块: 
	// 返 回 值: void : 该函数不返回任何值
	// 参数说明: 
	//     OpcUa_Byte* byInOut: 字节的输入和输出
	//          OpcUa_Int32 iBit: 位
	//          OpcUa_Int32 iVal: 值
	// **********************************************************************************************
	void CFepIEC103Device::SetBitVal( OpcUa_Byte* byInOut, OpcUa_Int32 iBit, OpcUa_Int32 iVal )
	{
		//设备已禁止，不修改值，直接返回。
		if ( IsDisable() )
			return;

		OpcUa_Byte byTemp = iVal?1:0;
		OpcUa_Byte byTemp2 = *byInOut;
		if( byTemp )
		{
			byTemp = (byTemp<<iBit);
			*byInOut = byTemp2 | byTemp;
		}
		else
		{
			byTemp = 1;
			byTemp = (byTemp<<iBit);
			byTemp = 0xff - byTemp;
			*byInOut = byTemp2 & byTemp;
		}
	}

	/*!
	*	\brief	FCB取反
	*	 
	*	\retval	返回值	无
	*
	*	\note		注意事项（可选）
	*/
	void CFepIEC103Device::ReverseFCB( void )
	{
		if (m_bFCB)
			m_bFCB = false;
		else
			m_bFCB = true;
	}

	/*!
	*	\brief	从配置文件中读取“标签是否需要SOE”
	*	
	*	\retval	返回值	无
	*
	*	\note		注意事项（可选）
	*/
	void CFepIEC103Device::InitTagSOEMap(void)
	{
		//std::string strComment = m_pIOUnitCfg->GetUnitCfgName();
		std::string strFileName = m_pIOUnitCfg->GetUnitCfgName();

		//! 获取配置文件绝对路径
		std::string strIniFilePath = CIODriver::GetFEPRunDir() + strFileName;

		////故障录波超时，毫秒
		m_FaultTimeOut = GetPrivateProfileInt("PARAM", "FaultTimeOut", 1800, strIniFilePath.c_str());	//配置单位为秒
		if ( m_FaultTimeOut < 60 )
		{
			m_FaultTimeOut = 60;
		}
		m_FaultTimeOut = m_FaultTimeOut*1000;	//转化成毫秒

		m_nWatchDog = GetPrivateProfileInt("PARAM", "WatchDog", 30, strIniFilePath.c_str());	//配置单位为秒
		if ( m_nWatchDog < 1 )
		{
			m_nWatchDog = 10;
		}

		OpcUa_Int32 iDIMax = GetPrivateProfileInt("NUM", "DIMax", 1, strIniFilePath.c_str());
		OpcUa_CharA chSoeID[MAX_SOE];
		for (OpcUa_Int32 i = 1; i <= iDIMax; i++)
		{
			std::string strTemp = "DI";		
			strTemp += itoa(i,chSoeID,10);		
			m_DISOEMap.insert( SOEMAP::value_type(i, GetPrivateProfileInt("SOE", strTemp.c_str(), 1, strIniFilePath.c_str())) );
			m_DIDevSOEMap.insert( SOEMAP::value_type(i, GetPrivateProfileInt("DEVSOE", strTemp.c_str(), 0, strIniFilePath.c_str())) );

			OpcUa_CharA buf[MAX_PATH];
			GetPrivateProfileString("DI", strTemp.c_str(), "", buf, 255, strIniFilePath.c_str());
			std::string strTemp2 = buf;
			if (!strTemp2.empty())
			{
				OpcUa_Int32 iPosPoint = strTemp2.find(",");
				if ( iPosPoint == -1 )	//Add by hxw in 2009.7.9
				{
					continue;
				}
				std::string  strLeft = strTemp2.substr(0,iPosPoint);
				std::string  strRight = strTemp2.substr(iPosPoint+1, strTemp2.length());
				OpcUa_Int32 iAddr = 1000*atoi(strLeft.c_str())+atoi(strRight.c_str());
				m_DIPointMap.insert(PointMap::value_type(i,iAddr));
			}
		}

		OpcUa_Int32 iDOMax = GetPrivateProfileInt("NUM", "DOMax", 1, strIniFilePath.c_str());
		for (OpcUa_Int32 i = 1; i <= iDOMax; i++)
		{
			std::string strTemp = "DO";		
			strTemp += itoa(i,chSoeID,10);	
			m_DOSOEMap.insert( SOEMAP::value_type(i, GetPrivateProfileInt("SOE", strTemp.c_str(), 1, strIniFilePath.c_str())) );
			m_DOWithSel.insert( SOEMAP::value_type(i, GetPrivateProfileInt("SELECT", strTemp.c_str(), 0, strIniFilePath.c_str())) );

			OpcUa_CharA buf[MAX_PATH];
			GetPrivateProfileString("DO", strTemp.c_str(), "", buf, 255, strIniFilePath.c_str());
			std::string strTemp2 = buf;
			if (!strTemp2.empty())
			{
				OpcUa_Int32 iPosPoint = strTemp2.find(',');
				if ( iPosPoint == -1 )	//Add by hxw in 2009.7.9
				{
					continue;
				}
				std::string  strLeft = strTemp2.substr(0,iPosPoint);
				std::string  strRight = strTemp2.substr(iPosPoint+1,strTemp2.length());
				OpcUa_Int32 iAddr = 1000*atoi(strLeft.c_str())+atoi(strRight.c_str());
				m_DOPointMap.insert(PointMap::value_type(i,iAddr));
			}
		}

		m_iAIMAX = GetPrivateProfileInt("NUM", "AIMax", 1, strIniFilePath.c_str());
		for (OpcUa_Int32 i = 1; i <= m_iAIMAX; i++)
		{			
			std::string strTemp = "AI";		
			strTemp += itoa(i,chSoeID,10);	
			OpcUa_CharA buf[MAX_PATH];
			GetPrivateProfileString("AI_modulus", strTemp.c_str(), "1", buf, 255, strIniFilePath.c_str());
			std::string strTemp2 = buf;
			OpcUa_Float dModulus = atof(strTemp2.c_str());
			m_AIModulusMap.insert(AIModulusMap::value_type(i , dModulus));
		}

		OpcUa_CharA buf[MAX_PATH];
		GetPrivateProfileString("AI", "AI", "", buf, 255, strIniFilePath.c_str());
		std::string strTemp2 = buf;
		if (!strTemp2.empty())
		{
			OpcUa_Int32 iPosPoint = strTemp2.find(',');
			if ( iPosPoint != -1 )	//Add by hxw in 2009.7.9
			{
				std::string strLeft = strTemp2.substr(0,iPosPoint);
				std::string strRight = strTemp2.substr(iPosPoint + 1,strTemp2.length());
				m_byAIFUN = atoi(strLeft.c_str());
				m_byAISEQ = atoi(strRight.c_str());		
			}
		}

		//! 读取ASDU21需要定期召唤的组号及召唤周期
		m_iASDU21ReadGroupPeriod = GetPrivateProfileInt("ASDU21", "Period", 0, strIniFilePath.c_str());
		if ( m_iASDU21ReadGroupPeriod < 0 )
		{
			m_iASDU21ReadGroupPeriod = 0;
		}

		m_iASDU7ReadGroupPeriod = GetPrivateProfileInt("ASDU7", "Period", 0, strIniFilePath.c_str());
		if ( m_iASDU7ReadGroupPeriod < 0 )
		{
			m_iASDU7ReadGroupPeriod = 0;
		}

		OpcUa_Int32 iASDU21GroupCount = GetPrivateProfileInt("ASDU21", "GroupCount", 0, strIniFilePath.c_str());
		if ( iASDU21GroupCount > 0 )
		{
			GetPrivateProfileString("ASDU21", "GroupNo", "", buf, 255, strIniFilePath.c_str());
			std::string strTemp = buf;
			if (!strTemp.empty())
			{
				OpcUa_Int32 iPos = 0;
				OpcUa_Int32 iIndex = 0;
				OpcUa_Int32 GroupNo = atoi(strTemp.c_str());
				if ( GroupNo > 0 && GroupNo < 256 )
				{
					m_vGroupNo.push_back( GroupNo );
					iIndex++;
				}

				while(1)
				{
					iPos = strTemp.find(",", iPos+1);
					if (iPos==-1||iIndex>=iASDU21GroupCount)
						break;

					std::string  strRight = strTemp.substr(iPos+1,strTemp.length());
					GroupNo = atoi(strRight.c_str());
					if ( GroupNo > 0 && GroupNo < 256 )
					{
						m_vGroupNo.push_back( GroupNo );
						iIndex++;
					}
				}
			}
		}
	
		return;
	}

	// **********************************************************************************************
	// 编 写 者: zhoujun        函数创建时间: 2006-9-19 
	// 函数名称: CMacsDIS103Device::TagNeedSOE
	// 函数功能: FCB取反
	// 算法思想: 
	// 调用模块: 
	// 返 回 值: BOOL 
	// 参数说明: iTagAddr		标签地址
	// **********************************************************************************************
	bool CFepIEC103Device::TagNeedSOE ( OpcUa_Byte byType, OpcUa_Int32 iTagAddr )
	{
		SOEMAPiterator it;
		if ( MacsDIS103_DI == byType )
		{
			it = m_DISOEMap.find( iTagAddr );
			if ( it == m_DISOEMap.end() )
				return false;
			else
			{
				return it->second;
			}
		}
		if ( MacsDIS103_DO == byType )
		{
			it = m_DOSOEMap.find( iTagAddr );
			if ( it == m_DOSOEMap.end() )
				return false;
			else
			{
				return it->second;
			}
		}
		return false;
	}

	bool  CFepIEC103Device::TagNeedDevSOE ( OpcUa_Byte byType, OpcUa_Int32 iTagAddr )
	{
		SOEMAPiterator it;
		if ( MacsDIS103_DI == byType )
		{
			it = m_DIDevSOEMap.find( iTagAddr );
			if ( it == m_DIDevSOEMap.end() )
				return false;
			else
			{
				return it->second;
			}
		}
		return false;
	}


	/*!
	*	\brief	组下行时间同步包
	*	
	*
	*	\param	bool bBroadCast	 	true为广播对时
	*	\param	OpcUa_Byte* buf: 时间同步包
	*
	*	\retval	返回值	时间同步包长。0表示无时间同步包。
	*
	*	\note		
	*/
	OpcUa_Int32 CFepIEC103Device::BuildCheckTimeFrame( OpcUa_Byte* buf, bool bBroadCast )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		OpcUa_CharA chMsg[255];
		bBroadCast = true;	//调试发现对设备点对点对时后设备所显示的时间不正确。一律限制为广播对时。

		ACE_Time_Value NowTime;	
		ACE_Date_Time dateTime;		
		CHANGEABLE_FRAME_STRUCT *TimeData = (CHANGEABLE_FRAME_STRUCT *)buf;
		ASDU_HEAD_STRUCT	*ASDU6 = (ASDU_HEAD_STRUCT *)TimeData->UserData;
		CP56Time2a	*LocalTime = (CP56Time2a*)(ASDU6 + 1);
		WORD	MillSec = 0;
		memset(buf, 0, MAX_IEC103_FRAM_LEN);	

		TimeData->Head.StartCode = IEC103_START_CODE;
		TimeData->Head.Len = 2 + sizeof(ASDU_HEAD_STRUCT) + sizeof(CP56Time2a);
		TimeData->Head.Relen = 2 + sizeof(ASDU_HEAD_STRUCT) + sizeof(CP56Time2a);
		TimeData->Head.ReStartCode = IEC103_START_CODE;

		TimeData->ControlField.Rev = 0;
		TimeData->ControlField.Prm = 1;
		TimeData->ControlField.FunCode = (bBroadCast)?4:3;
		TimeData->ControlField.FCV_DFC = 0;
		TimeData->ControlField.FCB_ACD = 0;

		TimeData->Address = (bBroadCast)?IEC103_BROADCAST_ADDRESS:GetAddr();

		ASDU6->TypeFlg = IEC103_SENDTIME_TYPE;
		ASDU6->VSQ = 0x81;
		ASDU6->COT = COT_TIME_SYN;
		ASDU6->ASDU_COM_ADD = TimeData->Address;
		ASDU6->FunType = FUN_GLB;
		ASDU6->InfNumber = INF_NOUSE;

		ACE_OS::memset( LocalTime, 0,sizeof(CP56Time2a) );
		NowTime = ACE_OS::gettimeofday();
		dateTime.update( NowTime );

		MillSec = (WORD)(dateTime.second() * 1000 + dateTime.microsec()/1000);	

		LocalTime->LowMillSec	= LOBYTE(MillSec);
		LocalTime->HighMillSec	= HIBYTE(MillSec);
		LocalTime->Minute		= (OpcUa_Byte)dateTime.minute();
		LocalTime->Hour			= (OpcUa_Byte)dateTime.hour();
		LocalTime->Day			= (OpcUa_Byte)dateTime.day();
		LocalTime->Week			= (dateTime.day()==0)?7:(OpcUa_Byte)dateTime.day();
		LocalTime->Mon			= (OpcUa_Byte)dateTime.month();
		LocalTime->Year			= dateTime.year() % 100;

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		buf[TimeData->Head.Len + sizeof(FRAME_HEAD)] = pPrtcl->Get8SumCode((OpcUa_Byte *)&buf[4], (WORD)TimeData->Head.Len);
		buf[TimeData->Head.Len + sizeof(FRAME_HEAD) + 1] = IEC103_END_CODE;

		return TimeData->Head.Len + 6;
	}

	// **********************************************************************************************
	// 编 写 者: zhoujun        函数创建时间: 2006-9-5
	// 函数名称: CFepIEC103Device::BuildGIFrame
	// 函数功能: 组总查询帧
	// 算法思想: 
	// 调用模块: 
	// 返 回 值: OpcUa_Int32 : 时间同步包长。0表示无时间同步包。
	// 参数说明: 
	//    OpcUa_Byte* buf: 时间同步包
	// **********************************************************************************************
	OpcUa_Int32 CFepIEC103Device::BuildGIFrame( OpcUa_Byte* pBuf )
	{
		if ( pBuf == NULL )
		{
			return 0;
		}
		OpcUa_Int32 FrameLen = sizeof(GIFRAME)/sizeof(OpcUa_Byte);

		GIFRAME GIFrame;

		GIFrame.GIFrameHead.Start = BIANCHANGZHEN_START_ID;
		GIFrame.GIFrameHead.DataLen = sizeof(GENERAL_INQUIRE)/sizeof(OpcUa_Byte) + 2;
		GIFrame.GIFrameHead.DataLenRepeat = GIFrame.GIFrameHead.DataLen;
		GIFrame.GIFrameHead.StartRepeat = BIANCHANGZHEN_START_ID;

		GIFrame.GIFrameHead.Addr = GetAddr();

		GIFrame.GIFrameHead.Control = 0;
		GIFrame.GIFrameHead.Control  <<= 1;
		GIFrame.GIFrameHead.Control += 1;
		GIFrame.GIFrameHead.Control <<= 1;
		if (m_bFCB)
			GIFrame.GIFrameHead.Control += 1;
		GIFrame.GIFrameHead.Control <<= 1;
		GIFrame.GIFrameHead.Control += 1;		//m_bFCV应该是1
		GIFrame.GIFrameHead.Control <<= 4;
		GIFrame.GIFrameHead.Control += FRAME_FUNC_SEND_DATA;

		GIFrame.GIFrameBody.ASDU_id.TypeID = TYP_GENERAL_QUERY_START;
		GIFrame.GIFrameBody.ASDU_id.AlterStruct = VSQ_MOST;
		GIFrame.GIFrameBody.ASDU_id.TransReason = COT_GENERAL_QUERY;
		GIFrame.GIFrameBody.ASDU_id.CommonAddr = GIFrame.GIFrameHead.Addr;
		GIFrame.GIFrameBody.InfoBody_id.FuncType = FUN_GLB;
		GIFrame.GIFrameBody.InfoBody_id.InfoSeqNO = 0;
		GIFrame.GIFrameBody.ScanNO = m_iGINo;

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		GIFrame.GIFrameTail.Check_Sum = pPrtcl->Get8SumCode((OpcUa_Byte*)(&GIFrame)+4, GIFrame.GIFrameHead.DataLen);
		GIFrame.GIFrameTail.End = BIANCHANGZHEN_END_ID;

		memcpy(pBuf, &GIFrame, FrameLen);

		return FrameLen;


	}

	/*!
	*	\brief	组“复位通信单元”帧
	*	
	*	\param	参数	OpcUa_Byte* pBuf: 要加的数据		
	*
	*	\retval	返回值	OpcUa_Int32 iLen: 要加的数据的长度
	*
	*	\note
	*/
	OpcUa_Int32 CFepIEC103Device::BuildResetCUFrame( OpcUa_Byte* pBuf )
	{
		OpcUa_Int32 FrameLen = GUDINGZHEN_SIZE;

		FT1_2_FIXED_FRAME ResetCU;
		ResetCU.Start = GUDINGZHEN_START_ID;
		//复位通信单元帧的控制域为01000000
		ResetCU.Control = 0x40;
		ResetCU.Addr = GetAddr();
		ResetCU.Check_Sum = ResetCU.Control + ResetCU.Addr;
		ResetCU.End = GUDINGZHEN_END_ID;

		memcpy(pBuf, &ResetCU, FrameLen);

		return FrameLen;
	}

	/*!
	*	\brief	组“复位帧计数器”帧	 
	*
	*	\param	参数	OpcUa_Int32 iLen: 要加的数据的长度
	*	\param	参数	OpcUa_Byte* pBuf: 要加的数据	
	*
	*	\retval	返回值	帧长
	*
	*	\note		注意事项（可选）
	*/
	OpcUa_Int32 CFepIEC103Device::BuildResetFCBFrame( OpcUa_Byte* pBuf )
	{
		OpcUa_Int32 FrameLen = GUDINGZHEN_SIZE;

		FT1_2_FIXED_FRAME ResetFCB;
		ResetFCB.Start = GUDINGZHEN_START_ID;
		//复位通信单元帧的控制域为01000111
		ResetFCB.Control = 0x47;
		ResetFCB.Addr = GetAddr();
		ResetFCB.Check_Sum = ResetFCB.Control + ResetFCB.Addr;
		ResetFCB.End = GUDINGZHEN_END_ID;

		memcpy(pBuf, &ResetFCB, FrameLen);

		m_bFCB = false;

		return FrameLen;
	}

	/*!
	*	\brief	组“请求一级数据”帧	 
	*
	*	\param	参数	OpcUa_Byte* pBuf: 要加的数据
	*
	*	\retval	返回值	该函数不返回任何值
	*
	*	\note		注意事项（可选）
	*/
	OpcUa_Int32 CFepIEC103Device::BuildLevel1DataFrame( OpcUa_Byte* pBuf )
	{
		OpcUa_Int32 FrameLen = GUDINGZHEN_SIZE;

		FT1_2_FIXED_FRAME ReqLevelData;
		ReqLevelData.Start = GUDINGZHEN_START_ID;
		//!01FCB1 1010
		ReqLevelData.Control = 0;
		ReqLevelData.Control <<= 1;
		ReqLevelData.Control += 1;
		ReqLevelData.Control <<= 1;
		if (m_bFCB)
			ReqLevelData.Control += 1;
		ReqLevelData.Control <<= 1;
		ReqLevelData.Control += 1;		//m_bFCV应该是1

		ReqLevelData.Control <<= 4;
		ReqLevelData.Control += FRAME_FUNC_INQUIRE_LEVEL1_DATA;

		ReqLevelData.Addr = GetAddr();
		ReqLevelData.Check_Sum = ReqLevelData.Control + ReqLevelData.Addr;
		ReqLevelData.End = GUDINGZHEN_END_ID;

		memcpy(pBuf, &ReqLevelData, FrameLen);

		return FrameLen;
	}


	/*!
	*	\brief	 组“请求二级数据”帧	 
	*
	*	\param	参数	OpcUa_Byte* pBuf: 要加的数据		
	*
	*	\retval	返回值	该函数不返回任何值
	*
	*	\note		注意事项（可选）
	*/
	OpcUa_Int32 CFepIEC103Device::BuildLevel2DataFrame( OpcUa_Byte* pBuf )
	{
		OpcUa_Int32 FrameLen = GUDINGZHEN_SIZE;

		FT1_2_FIXED_FRAME ReqLevelData;
		ReqLevelData.Start = GUDINGZHEN_START_ID;
		//!01FCB1 1011
		ReqLevelData.Control = 0;
		ReqLevelData.Control <<= 1;
		ReqLevelData.Control += 1;
		ReqLevelData.Control <<= 1;
		if (m_bFCB)
			ReqLevelData.Control += 1;
		ReqLevelData.Control <<= 1;
		ReqLevelData.Control += 1;			//m_bFCV应该是1

		ReqLevelData.Control <<= 4;
		ReqLevelData.Control += FRAME_FUNC_INQUIRE_LEVEL2_DATA;

		ReqLevelData.Addr = GetAddr();
		ReqLevelData.Check_Sum = ReqLevelData.Control + ReqLevelData.Addr;
		ReqLevelData.End = GUDINGZHEN_END_ID;

		memcpy(pBuf, &ReqLevelData, FrameLen);

		return FrameLen;
	}


	OpcUa_Int32 CFepIEC103Device::BuildDOFrame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr, OpcUa_Byte bVal )
	{
		if ( pBuf == NULL )
		{
			return 0;
		}

		OpcUa_Int32 iPointAddr = GetDOPointAddr( iTagAddr ); 
		if ( iPointAddr == -1 )
		{
			return 0;
		}

		OpcUa_Int32 FrameLen = sizeof(GCFRAME)/sizeof(OpcUa_Byte);

		GCFRAME GCFrame;

		GCFrame.GCFrameHead.Start = BIANCHANGZHEN_START_ID;
		GCFrame.GCFrameHead.DataLen = sizeof(GENRRAL_CMD)/sizeof(OpcUa_Byte) + 2;
		GCFrame.GCFrameHead.DataLenRepeat = GCFrame.GCFrameHead.DataLen;
		GCFrame.GCFrameHead.StartRepeat = BIANCHANGZHEN_START_ID;

		GCFrame.GCFrameHead.Addr = GetAddr();

		GCFrame.GCFrameHead.Control = 0;
		GCFrame.GCFrameHead.Control  <<= 1;
		GCFrame.GCFrameHead.Control += 1;
		GCFrame.GCFrameHead.Control <<= 1;
		if (m_bFCB)
			GCFrame.GCFrameHead.Control += 1;
		GCFrame.GCFrameHead.Control <<= 1;
		GCFrame.GCFrameHead.Control += 1;		//m_bFCV应该是1
		GCFrame.GCFrameHead.Control <<= 4;
		GCFrame.GCFrameHead.Control += FRAME_FUNC_SEND_DATA;

		GCFrame.GCFrameBody.ASDU_id.TypeID = TYP_GENERAL_CMD;
		GCFrame.GCFrameBody.ASDU_id.AlterStruct = VSQ_MOST;
		GCFrame.GCFrameBody.ASDU_id.TransReason = COT_GENERAL_CMD_CONFIRM;
		GCFrame.GCFrameBody.ASDU_id.CommonAddr = GCFrame.GCFrameHead.Addr;

		GCFrame.GCFrameBody.InfoBody_id.FuncType = iPointAddr/1000;
		GCFrame.GCFrameBody.InfoBody_id.InfoSeqNO = iPointAddr%1000;
		if (bVal)
			GCFrame.GCFrameBody.DCO = 2;
		else
			GCFrame.GCFrameBody.DCO = 1;
		GCFrame.GCFrameBody.RII = 53;

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		GCFrame.GCFrameTail.Check_Sum = pPrtcl->Get8SumCode((OpcUa_Byte*)(&GCFrame)+4, GCFrame.GCFrameHead.DataLen);
		GCFrame.GCFrameTail.End = BIANCHANGZHEN_END_ID;

		memcpy(pBuf, &GCFrame, FrameLen);

		return FrameLen;
	}



	OpcUa_Int32 CFepIEC103Device::BuildDOBFrame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr, OpcUa_Byte bVal )
	{
		if ( pBuf == NULL )
		{
			return 0;
		}

		OpcUa_Int32 FrameLen = sizeof(GCFRAME)/sizeof(OpcUa_Byte);

		GCFRAME GCFrame;

		GCFrame.GCFrameHead.Start = BIANCHANGZHEN_START_ID;
		GCFrame.GCFrameHead.DataLen = sizeof(GENRRAL_CMD)/sizeof(OpcUa_Byte) + 2;
		GCFrame.GCFrameHead.DataLenRepeat = GCFrame.GCFrameHead.DataLen;
		GCFrame.GCFrameHead.StartRepeat = BIANCHANGZHEN_START_ID;

		GCFrame.GCFrameHead.Addr = GetAddr();

		GCFrame.GCFrameHead.Control = 0;
		GCFrame.GCFrameHead.Control  <<= 1;
		GCFrame.GCFrameHead.Control += 1;
		GCFrame.GCFrameHead.Control <<= 1;
		if (m_bFCB)
			GCFrame.GCFrameHead.Control += 1;
		GCFrame.GCFrameHead.Control <<= 1;
		GCFrame.GCFrameHead.Control += 1;		//m_bFCV应该是1
		GCFrame.GCFrameHead.Control <<= 4;
		GCFrame.GCFrameHead.Control += FRAME_FUNC_SEND_DATA;

		GCFrame.GCFrameBody.ASDU_id.TypeID = TYP_GENERAL_CMD;
		GCFrame.GCFrameBody.ASDU_id.AlterStruct = VSQ_MOST;
		GCFrame.GCFrameBody.ASDU_id.TransReason = COT_GENERAL_CMD_CONFIRM;
		GCFrame.GCFrameBody.ASDU_id.CommonAddr = GCFrame.GCFrameHead.Addr;

		GCFrame.GCFrameBody.InfoBody_id.FuncType = iTagAddr/1000;
		GCFrame.GCFrameBody.InfoBody_id.InfoSeqNO = iTagAddr%1000;
		if (bVal)
			GCFrame.GCFrameBody.DCO = 2;
		else
			GCFrame.GCFrameBody.DCO = 1;
		GCFrame.GCFrameBody.RII = 53;

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		GCFrame.GCFrameTail.Check_Sum = pPrtcl->Get8SumCode((OpcUa_Byte*)(&GCFrame)+4, GCFrame.GCFrameHead.DataLen);
		GCFrame.GCFrameTail.End = BIANCHANGZHEN_END_ID;

		memcpy(pBuf, &GCFrame, FrameLen);

		return FrameLen;
	}


	/*!
	*	\brief	组“ASDU24”帧
	*	
	*	\param	参数	OpcUa_Byte* pBuf: 要加的数据
	*	\param	
	*	\param	
	*
	*	\retval	返回值	 该函数不返回任何值
	*
	*	\note		注意事项（可选）
	*/
	OpcUa_Int32 CFepIEC103Device::BuildASDU24Frame ( OpcUa_Byte* pBuf, enumASDU24TOO eTOO )
	{
		OpcUa_Int32 FrameLen = sizeof(DDTCMDFRAME)/sizeof(OpcUa_Byte);

		DDTCMDFRAME DDTCMDFrame;

		DDTCMDFrame.DDTFrameHead.Start = BIANCHANGZHEN_START_ID;
		DDTCMDFrame.DDTFrameHead.DataLen = sizeof(DDS_CMD)/sizeof(OpcUa_Byte) + 2;
		DDTCMDFrame.DDTFrameHead.DataLenRepeat = DDTCMDFrame.DDTFrameHead.DataLen;
		DDTCMDFrame.DDTFrameHead.StartRepeat = BIANCHANGZHEN_START_ID;

		DDTCMDFrame.DDTFrameHead.Control = 0;
		DDTCMDFrame.DDTFrameHead.Control <<= 1;
		DDTCMDFrame.DDTFrameHead.Control += 1;
		DDTCMDFrame.DDTFrameHead.Control <<= 1;
		if (m_bFCB)
			DDTCMDFrame.DDTFrameHead.Control += 1;
		DDTCMDFrame.DDTFrameHead.Control <<= 1;
		DDTCMDFrame.DDTFrameHead.Control += 1;
		DDTCMDFrame.DDTFrameHead.Control <<= 4;
		DDTCMDFrame.DDTFrameHead.Control += FRAME_FUNC_SEND_DATA;

		DDTCMDFrame.DDTFrameHead.Addr = GetAddr();

		DDTCMDFrame.DDTFrameBody.ASDU_id.TypeID = TYP_DISORDER_DATA_TRANS;
		DDTCMDFrame.DDTFrameBody.ASDU_id.AlterStruct = VSQ_MOST;
		DDTCMDFrame.DDTFrameBody.ASDU_id.TransReason = COT_DISORDER_DATA_TRANS;
		DDTCMDFrame.DDTFrameBody.ASDU_id.CommonAddr = DDTCMDFrame.DDTFrameHead.Addr;
		DDTCMDFrame.DDTFrameBody.InfoBody_id.FuncType = m_byFaultFUN;
		DDTCMDFrame.DDTFrameBody.InfoBody_id.InfoSeqNO = INF_NOUSE;
		switch (eTOO)
		{
		case T_FaultSelect:
			DDTCMDFrame.DDTFrameBody.TOO = TOO_FAULT_SELCET;
			break;
		case T_Data_Req:
			DDTCMDFrame.DDTFrameBody.TOO = TOO_START_DISDATA;
			break;
		case T_Data_Trans_End:
			DDTCMDFrame.DDTFrameBody.TOO = TOO_PAUSE_DISDATA;
			break;
		case T_StatusChange_Req:
			DDTCMDFrame.DDTFrameBody.TOO = TOO_START_STACHANGE_WITHSIG;
			break;
		case T_StatusChange_End:
			DDTCMDFrame.DDTFrameBody.TOO = TOO_PAUSE_STACHANGE_WITHSIG;
			break;
		case T_Chanel_Req:
			DDTCMDFrame.DDTFrameBody.TOO = TOO_CHANEL_REQUSET;
			break;
		default:
			break;
		}

		DDTCMDFrame.DDTFrameBody.TOV = TOV_SHUNSHI_VALUE;
		DDTCMDFrame.DDTFrameBody.FAN = m_wFAN;			//需要检查高低位是否正确
		DDTCMDFrame.DDTFrameBody.ACC = m_byACC;

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		DDTCMDFrame.DDTFrameTail.Check_Sum = pPrtcl->Get8SumCode((OpcUa_Byte*)(&DDTCMDFrame)+4, DDTCMDFrame.DDTFrameHead.DataLen);
		DDTCMDFrame.DDTFrameTail.End = BIANCHANGZHEN_END_ID;

		memcpy(pBuf, &DDTCMDFrame, FrameLen);

		return FrameLen;
	}

	// **********************************************************************************************
	// 编 写 者: zhoujun        函数创建时间: 2006_9_11 
	// 函数名称: CMacsGZ103Device::BuildDisDataAckFrame
	// 函数功能: 组“ASDU25 肯定或否定认可”帧
	// 算法思想: 
	// 调用模块: 
	// 返 回 值: void : 该函数不返回任何值
	// 参数说明: 
	//		CMacsGZ103Device* pDevice: 指定的设备
	//		OpcUa_Byte* pBuf: 要加的数据
	//      OpcUa_Int32 iLen: 要加的数据的长度
	// **********************************************************************************************
	OpcUa_Int32 CFepIEC103Device::BuildDisDataAckFrame( OpcUa_Byte* pBuf, OpcUa_Byte bAck )
	{
		OpcUa_Int32 FrameLen = sizeof(DDTCMDFRAME)/sizeof(OpcUa_Byte);

		DDTCMDFRAME DDTCMDFrame;

		DDTCMDFrame.DDTFrameHead.Start = BIANCHANGZHEN_START_ID;
		DDTCMDFrame.DDTFrameHead.DataLen = sizeof(DDS_CMD)/sizeof(OpcUa_Byte) + 2;
		DDTCMDFrame.DDTFrameHead.DataLenRepeat = DDTCMDFrame.DDTFrameHead.DataLen;
		DDTCMDFrame.DDTFrameHead.StartRepeat = BIANCHANGZHEN_START_ID;

		DDTCMDFrame.DDTFrameHead.Control = 0;
		DDTCMDFrame.DDTFrameHead.Control <<= 1;
		DDTCMDFrame.DDTFrameHead.Control += 1;
		DDTCMDFrame.DDTFrameHead.Control <<= 1;
		if (m_bFCB)
			DDTCMDFrame.DDTFrameHead.Control += 1;
		DDTCMDFrame.DDTFrameHead.Control <<= 1;
		DDTCMDFrame.DDTFrameHead.Control += 1;
		DDTCMDFrame.DDTFrameHead.Control <<= 4;
		DDTCMDFrame.DDTFrameHead.Control += FRAME_FUNC_SEND_DATA;

		DDTCMDFrame.DDTFrameHead.Addr = GetAddr();

		DDTCMDFrame.DDTFrameBody.ASDU_id.TypeID = TYP_DISORDER_DATA_ACK;
		DDTCMDFrame.DDTFrameBody.ASDU_id.AlterStruct = VSQ_MOST;
		DDTCMDFrame.DDTFrameBody.ASDU_id.TransReason = COT_DISORDER_DATA_TRANS;
		DDTCMDFrame.DDTFrameBody.ASDU_id.CommonAddr = DDTCMDFrame.DDTFrameHead.Addr;
		DDTCMDFrame.DDTFrameBody.InfoBody_id.FuncType = m_byFaultFUN;
		DDTCMDFrame.DDTFrameBody.InfoBody_id.InfoSeqNO = INF_NOUSE;

		DDTCMDFrame.DDTFrameBody.TOO = bAck;
		//
		//	if (m_bStatusInitValue == m_bStatusChangedValue)
		//		DDTCMDFrame.DDTFrameBody.TOO = TOO_STATUSCHANGED_FAIL;
		//	else
		//		DDTCMDFrame.DDTFrameBody.TOO = TOO_STATUSCHANGED_SUCCESS;
		//
		DDTCMDFrame.DDTFrameBody.TOV = TOV_SHUNSHI_VALUE;
		DDTCMDFrame.DDTFrameBody.FAN = m_wFAN;
		DDTCMDFrame.DDTFrameBody.ACC = m_byACC;

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		DDTCMDFrame.DDTFrameTail.Check_Sum = pPrtcl->Get8SumCode((OpcUa_Byte*)(&DDTCMDFrame)+4, DDTCMDFrame.DDTFrameHead.DataLen);
		DDTCMDFrame.DDTFrameTail.End = BIANCHANGZHEN_END_ID;

		memcpy(pBuf, &DDTCMDFrame, FrameLen);

		return FrameLen;
	}

	void CFepIEC103Device::GetTimeFromCP32Time( CP32Time2a Time, OpcUa_Byte& Second, WORD& MilliSecond )
	{
		WORD millesec	= MAKEWORD(Time.LowMillSec, Time.HighMillSec);
		Second			= millesec / 1000;
		MilliSecond		= millesec % 1000;
	}

	// **********************************************************************************************
	// 编 写 者: Hexuewen        函数创建时间: 2012-9-5
	// 函数名称: CFepIEC103Device::BuildASDU21WholeCallFrame
	// 函数功能: 组ASDU21总召唤帧
	// 算法思想: 
	// 调用模块: 
	// 返 回 值: OpcUa_Int32 : ASDU21总召唤帧包长。0表示无ASDU21总召唤帧。
	// 参数说明: 
	//    OpcUa_Byte* buf: ASDU21总召唤帧 
	OpcUa_Int32 CFepIEC103Device::BuildASDU21WholeCallFrame( OpcUa_Byte* pBuf )
	{
		if ( pBuf == NULL )
		{
			return 0;
		}
		OpcUa_Int32 FrameLen = sizeof(ASDU21_WHOLECALL)/sizeof(OpcUa_Byte);

		ASDU21_WHOLECALL Frame;

		memset( &Frame, 0, sizeof(Frame));

		Frame.GSCMDFrameHead.Start = BIANCHANGZHEN_START_ID;
		Frame.GSCMDFrameHead.DataLen = sizeof(ASDU21_WHOLECALL)/sizeof(OpcUa_Byte) - 6;
		Frame.GSCMDFrameHead.DataLenRepeat = Frame.GSCMDFrameHead.DataLen;
		Frame.GSCMDFrameHead.StartRepeat = BIANCHANGZHEN_START_ID;

		Frame.GSCMDFrameHead.Addr = GetAddr();

		Frame.GSCMDFrameHead.Control = 0;
		Frame.GSCMDFrameHead.Control  <<= 1;
		Frame.GSCMDFrameHead.Control += 1;
		Frame.GSCMDFrameHead.Control <<= 1;
		if (m_bFCB)
			Frame.GSCMDFrameHead.Control += 1;
		Frame.GSCMDFrameHead.Control <<= 1;
		Frame.GSCMDFrameHead.Control += 1;		//m_bFCV应该是1
		Frame.GSCMDFrameHead.Control <<= 4;
		Frame.GSCMDFrameHead.Control += FRAME_FUNC_SEND_DATA;

		Frame.GSCMDFrameBody.Type		= TYP_COMM_CMD;
		Frame.GSCMDFrameBody.VSQ		= VSQ_MOST;
		Frame.GSCMDFrameBody.COT		= COT_GENERAL_QUERY;
		Frame.GSCMDFrameBody.Addr		= GetAddr();
		Frame.GSCMDFrameBody.Fun		= FUN_COMM_SORT;
		Frame.GSCMDFrameBody.Inf		= INF_COMM_DATA_GENERAL_QUERY;
		Frame.GSCMDFrameBody.RII		= 0;
		Frame.GSCMDFrameBody.NGD		= 0;

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		Frame.GSCMDFrameTail.Check_Sum = pPrtcl->Get8SumCode((OpcUa_Byte*)(&Frame)+4, Frame.GSCMDFrameHead.DataLen);
		Frame.GSCMDFrameTail.End = BIANCHANGZHEN_END_ID;

		memcpy(pBuf, &Frame, FrameLen);

		return FrameLen;

	}

/*
	ASDU140为我司线路差动保护P521 103的私有ASDU，在总召唤中不予上传，经过下发相应的报文方可上传。 
		例如：发送: 68 08 08 68 73 01 8C 81 14 01 01 02 99 16      FUN:01  INF:02  读取地址为0201的当前定值 
		( 
		链路地址: 1      ASDU 140      COT: 20  一般命令 
		应用服务数据单元公共地址: 1 
		) 
		68 08 08 68 73 01 
		8C 81 14 01 01 02 
		99 16
*/
	OpcUa_Int32 CFepIEC103Device::BuildASDU140Frame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr )
	{
		if ( pBuf == NULL )
		{
			return 0;
		}

		OpcUa_Int32 Fun = iTagAddr/1000;
		OpcUa_Int32 Inf = iTagAddr%1000;
		if ( Fun > 255 || Inf > 255 )
		{
			return 0;
		}

		OpcUa_Int32 FrameLen = sizeof(ASDU140FRAME)/sizeof(OpcUa_Byte);

		ASDU140FRAME Frame;

		memset( &Frame, 0, sizeof(Frame));

		Frame.GSCMDFrameHead.Start = BIANCHANGZHEN_START_ID;
		Frame.GSCMDFrameHead.DataLen = sizeof(ASDU140FRAME)/sizeof(OpcUa_Byte) - 6;
		Frame.GSCMDFrameHead.DataLenRepeat = Frame.GSCMDFrameHead.DataLen;
		Frame.GSCMDFrameHead.StartRepeat = BIANCHANGZHEN_START_ID;

		Frame.GSCMDFrameHead.Addr = GetAddr();

		Frame.GSCMDFrameHead.Control = 0;
		Frame.GSCMDFrameHead.Control  <<= 1;
		Frame.GSCMDFrameHead.Control += 1;
		Frame.GSCMDFrameHead.Control <<= 1;
		if (m_bFCB)
			Frame.GSCMDFrameHead.Control += 1;
		Frame.GSCMDFrameHead.Control <<= 1;
		Frame.GSCMDFrameHead.Control += 1;		//m_bFCV应该是1
		Frame.GSCMDFrameHead.Control <<= 4;
		Frame.GSCMDFrameHead.Control += FRAME_FUNC_SEND_DATA;

		Frame.Type		= TYP_ASDU140;
		Frame.VSQ		= VSQ_MOST;
		Frame.COT		= COT_GENERAL_CMD_CONFIRM;
		Frame.Addr		= GetAddr();
		Frame.Fun		= Fun;
		Frame.Inf		= Inf;

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		Frame.GSCMDFrameTail.Check_Sum = pPrtcl->Get8SumCode((OpcUa_Byte*)(&Frame)+4, Frame.GSCMDFrameHead.DataLen);
		Frame.GSCMDFrameTail.End = BIANCHANGZHEN_END_ID;

		memcpy(pBuf, &Frame, FrameLen);

		return FrameLen;
	}

	OpcUa_Int32 CFepIEC103Device::BuildASDU232Frame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr, OpcUa_Byte bVal)
	{
		if ( pBuf == NULL )
		{
			return 0;
		}

		OpcUa_Int32 Fun = iTagAddr/1000;
		OpcUa_Int32 Inf = iTagAddr%1000;
		if ( Fun > 255 || Inf > 255 )
		{
			return 0;
		}

		OpcUa_Int32 FrameLen = sizeof(ASDU232FRAME)/sizeof(OpcUa_Byte);

		ASDU232FRAME Frame;

		memset( &Frame, 0, sizeof(Frame));

		Frame.GSCMDFrameHead.Start = BIANCHANGZHEN_START_ID;
		Frame.GSCMDFrameHead.DataLen = sizeof(ASDU232FRAME)/sizeof(OpcUa_Byte) - 6;
		Frame.GSCMDFrameHead.DataLenRepeat = Frame.GSCMDFrameHead.DataLen;
		Frame.GSCMDFrameHead.StartRepeat = BIANCHANGZHEN_START_ID;

		Frame.GSCMDFrameHead.Addr = GetAddr();

		Frame.GSCMDFrameHead.Control = 0;
		Frame.GSCMDFrameHead.Control  <<= 1;
		Frame.GSCMDFrameHead.Control += 1;
		Frame.GSCMDFrameHead.Control <<= 1;
		if (m_bFCB)
			Frame.GSCMDFrameHead.Control += 1;
		Frame.GSCMDFrameHead.Control <<= 1;
		Frame.GSCMDFrameHead.Control += 1;		//m_bFCV应该是1
		Frame.GSCMDFrameHead.Control <<= 4;
		Frame.GSCMDFrameHead.Control += FRAME_FUNC_SEND_DATA;

		Frame.Type		= TYP_ASDU232;
		Frame.VSQ		= VSQ_MOST;
//		Frame.COT		= COT_GENERAL_CMD_CONFIRM;
		Frame.COT		= 0;
		Frame.Addr		= GetAddr();
		Frame.Fun		= Fun;
		Frame.Inf		= Inf;
		Frame.Val		= bVal;
		Frame.RII		= 0;

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		Frame.GSCMDFrameTail.Check_Sum = pPrtcl->Get8SumCode((OpcUa_Byte*)(&Frame)+4, Frame.GSCMDFrameHead.DataLen);
		Frame.GSCMDFrameTail.End = BIANCHANGZHEN_END_ID;

		memcpy(pBuf, &Frame, FrameLen);

		return FrameLen;
	}

/*
读取组号为35H的全部值或属性 
发送: 68 0D 0D 68 73 01 15 81 2A 01 FE F1 49 01 35 00 01 A4 16 
( 
 链路地址: 1      ASDU 21      COT: 42  通用分类读命令 
 应用服务数据单元公共地址: 1 
 功能类型(FUN): 254      信息序号(INF): 241  读一个组的全部条目的值或属性 
 返回信息标识(RII): 73      通用分类标识数目(NOG): 1 
 组号: 53      实际值 
 ) 
 68 0D 0D 68 73 01 
 15 81 2A 01 FE F1 49 01 35 00 01 
 A4 16
 typedef struct {
 OpcUa_Byte	Type;	//ASDU标识符
 OpcUa_Byte	VSQ;				
 OpcUa_Byte	COT;			
 OpcUa_Byte	Addr;
 OpcUa_Byte	Fun;
 OpcUa_Byte	Inf;
 OpcUa_Byte    RII;	//返回信息标识符,与主站命令中的RII一致
 OpcUa_Byte    NGD;	//通用分类标识数目=1


 GIN			sGIN;		//通用分类标识序号:组号＋条目号
 OpcUa_Byte		KOD;		//描述类别
 }ASDU21_1NGD;


 类型标识                    数据
 可变结构限定词(VSQ)         标识
 传送原因   (COT)            7.2中
 应用服务数据单元公共地址 
 功能类型(TYP)：＝GEN           
 信息序号  (INF)                  
 返回信息标识符 (RII)              
 通用分类标识数目  (NOG)      n
 通用分类标识序号(GIN)       NO.1
 类型描述   (KOD)			 NO.1
 ...
 ...
 通用分类标识序号(GIN)       NO.n
 类型描述   (KOD)			 NO.n

*/
	OpcUa_Int32 CFepIEC103Device::BuildASDU21GroupFrame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr )
	{
		if ( pBuf == NULL )
		{
			return 0;
		}

		OpcUa_Int32 Group = iTagAddr/1000;
		OpcUa_Int32 Entry = iTagAddr%1000;
		if ( Group > 255 || Entry > 255 )
		{
			return 0;
		}

		OpcUa_Int32 FrameLen = sizeof(ASDU21_1NGDFRAME)/sizeof(OpcUa_Byte);

		ASDU21_1NGDFRAME Frame;

		memset( &Frame, 0, sizeof(Frame));

		Frame.Head.Start = BIANCHANGZHEN_START_ID;
		Frame.Head.DataLen = sizeof(ASDU21_1NGDFRAME)/sizeof(OpcUa_Byte) - 6;
		Frame.Head.DataLenRepeat = Frame.Head.DataLen;
		Frame.Head.StartRepeat = BIANCHANGZHEN_START_ID;

		Frame.Head.Addr = GetAddr();

		Frame.Head.Control = 0;
		Frame.Head.Control  <<= 1;
		Frame.Head.Control += 1;
		Frame.Head.Control <<= 1;
		if (m_bFCB)
			Frame.Head.Control += 1;
		Frame.Head.Control <<= 1;
		Frame.Head.Control += 1;		//m_bFCV应该是1
		Frame.Head.Control <<= 4;
		Frame.Head.Control += FRAME_FUNC_SEND_DATA;

		Frame.Body.Type		= TYP_COMM_CMD;
		Frame.Body.VSQ		= VSQ_MOST;
		Frame.Body.COT		= COT_COMM_READ_CMD_VALID;
		Frame.Body.Addr		= GetAddr();
		Frame.Body.Fun		= FUN_COMM_SORT;
		Frame.Body.Inf		= INF_READ_ALL_ITEM;
		Frame.Body.RII		= 0;
		Frame.Body.NGD		= 1;
		Frame.Body.sGIN.Group		= Group;
		Frame.Body.sGIN.Entry		= Entry;	//0表示整组

		Frame.Body.KOD				= KOD1_ACTUALVALUE;

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		Frame.Tail.Check_Sum = pPrtcl->Get8SumCode((OpcUa_Byte*)(&Frame)+4, Frame.Head.DataLen);
		Frame.Tail.End = BIANCHANGZHEN_END_ID;

		memcpy(pBuf, &Frame, FrameLen);

		return FrameLen;
	}

	void CFepIEC103Device::SetHot( OpcUa_Byte byHot )
	{
		MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "SetHot: Device=%s to byHot=%d!" ),
			GetName().c_str(), byHot ));

		CFepIEC103Prtcl* pPrtcl = (CFepIEC103Prtcl*)GetPrtcl();
		if ( pPrtcl )
		{
			OpcUa_CharA pchTemp[1024]={0};
			sprintf( pchTemp, "SetHot: Device(State=%s)=%s to byHot=%d!", IsOnLine()?"Online":(IsOffLine()?"Offline":"Init"),GetName().c_str(), byHot );
			CFepIEC103Drv* pDriver = (CFepIEC103Drv*)GetDriver();
			pDriver->WriteLogAnyWay( pchTemp );
		}

		m_pUnitState->byHot = byHot;
	}
}
