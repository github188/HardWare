/*!
 *	\file	FepIEC104Device.cpp
 *
 *	\brief	104主站设备类实现源文件
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Device.cpp 3     09-03-05 15:03 Litianhui $
 *	$Author: Litianhui $
 *	$Date: 09-03-05 15:03 $
 *	$Revision: 3 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "FepIEC104Device.h"
#include "math.h"
#include "FepIEC104Prtcl.h"
#include "FepIEC104Drv.h"

/*
Template       Unit_type Raw_type Bit_width Low High Comment 
d%U(0,0,65535) 0         0        1         0   0    digital 
r%U(0,0,65535) 1         2        32        0   0    float 
i%U(0,0,65535) 2         1        32        0   0    OpcUa_Int32 
s%U(0,0,65535) 3         7        2048      0   0    string 
*/
#define UNIT_TYPE_INDEX_DIGITAL 1
#define UNIT_TYPE_INDEX_REAL    9
//#define UNIT_TYPE_INDEX_OpcUa_Int     2
//#define UNIT_TYPE_INDEX_STRING  3

namespace MACS_SCADA_SUD
{
	std::string CFepIEC104Device::s_ClassName("CFepIEC104Device");		//! 类名

	std::string CFepIEC104Device::GetClassName()
	{
		return s_ClassName;
	}

	CFepIEC104Device::CFepIEC104Device( )
	{
		m_bState			= false;
		m_wdNs				= 0;
		m_wdNr				= 0;
		m_wCtrlAreaINum		= 0;
		m_nCtrlAreaIMax		= 8;
		m_bNeedWholeQuery	= false;		
		m_bNeedCountQuery	= false;
		m_bIsCheckTiming	= false;
		m_bWholeQuerying	= false;
		m_bCountQuerying	= false;
		m_bNeedCheckTime	= false;
	}


	// *****************************************************
	// 函数功能: 组下行测试包: TESTFR
	// 返 回 值: OpcUa_Int32 : 测试包长。0表示无测试包。
	// 参数说明: 
	//    OpcUa_Byte* buf: 测试包
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTestFrame( OpcUa_Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		APCI* pReq = (APCI* )buf;
		ACE_OS::memset( pReq, 0, sizeof(APCI) );
		pReq->byStart							= PACK_HEAD;
		pReq->byAPDULen							= 0X04;
		pReq->unCtrlArea.CtrlAreaU.Flag			= 0X03;
		pReq->unCtrlArea.CtrlAreaU.Nr			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res1			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res2			= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_CON	= 0;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_CON	= 0;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_ACT	= 1;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_CON	= 0;

		return sizeof(APCI);
	}


	// *****************************************************
	// 函数功能: 组下行测试确认包: TESTFR
	// 返 回 值: OpcUa_Int32 : 测试包长。0表示无测试包。
	// 参数说明: 
	//    OpcUa_Byte* buf: 测试包
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTestFrameCON( OpcUa_Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		APCI* pReq = (APCI* )buf;
		ACE_OS::memset( pReq, 0, sizeof(APCI) );
		pReq->byStart							= PACK_HEAD;
		pReq->byAPDULen							= 0X04;
		pReq->unCtrlArea.CtrlAreaU.Flag			= 0X03;
		pReq->unCtrlArea.CtrlAreaU.Nr			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res1			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res2			= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_CON	= 0;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_CON	= 0;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_CON	= 1;

		return sizeof(APCI);
	}


	// *****************************************************
	// 函数功能: 组STOPDT帧
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//    OpcUa_Byte* buf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildStopFrame( OpcUa_Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		APCI* pReq = (APCI* )buf;
		ACE_OS::memset( pReq, 0, sizeof(APCI) );
		pReq->byStart							= PACK_HEAD;
		pReq->byAPDULen							= 0X04;
		pReq->unCtrlArea.CtrlAreaU.Flag			= 0X03;
		pReq->unCtrlArea.CtrlAreaU.Nr			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res1			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res2			= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_CON	= 0;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_ACT	= 1;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_CON	= 0;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_CON	= 0;

		return sizeof(APCI);
	}


	// *****************************************************
	// 函数功能: 组STOPDT确认帧
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//    OpcUa_Byte* buf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildStopFrameCON( OpcUa_Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		APCI* pReq = (APCI* )buf;
		ACE_OS::memset( pReq, 0, sizeof(APCI) );
		pReq->byStart							= PACK_HEAD;
		pReq->byAPDULen							= 0X04;
		pReq->unCtrlArea.CtrlAreaU.Flag			= 0X03;
		pReq->unCtrlArea.CtrlAreaU.Nr			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res1			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res2			= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_CON	= 0;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_CON	= 1;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_CON	= 0;

		return sizeof(APCI);
	}


	// *****************************************************
	// 函数功能: 组STARTDT帧
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//    OpcUa_Byte* buf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildStartFrame( OpcUa_Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		APCI* pReq = (APCI* )buf;
		ACE_OS::memset( pReq, 0, sizeof(APCI) );
		pReq->byStart							= PACK_HEAD;
		pReq->byAPDULen							= 0X04;
		pReq->unCtrlArea.CtrlAreaU.Flag			= 0X03;
		pReq->unCtrlArea.CtrlAreaU.Nr			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res1			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res2			= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_ACT	= 1;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_CON	= 0;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_CON	= 0;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_CON	= 0;

		return sizeof(APCI);
	}

	// *****************************************************
	// 函数功能: 组STARTDT确认帧
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//    OpcUa_Byte* buf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildStartFrameCON( OpcUa_Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		APCI* pReq = (APCI* )buf;
		ACE_OS::memset( pReq, 0, sizeof(APCI) );
		pReq->byStart							= PACK_HEAD;
		pReq->byAPDULen							= 0X04;
		pReq->unCtrlArea.CtrlAreaU.Flag			= 0X03;
		pReq->unCtrlArea.CtrlAreaU.Nr			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res1			= 0;
		pReq->unCtrlArea.CtrlAreaU.Res2			= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.STARTDT_CON	= 1;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.STOPDT_CON	= 0;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_ACT	= 0;
		pReq->unCtrlArea.CtrlAreaU.TESTFR_CON	= 0;

		return sizeof(APCI);
	}


	// *****************************************************
	// 函数功能: 组S格式帧
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//    OpcUa_Byte* buf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildSFrame( OpcUa_Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		APCI* pReq = (APCI* )buf;
		ACE_OS::memset( pReq, 0, sizeof(APCI) );
		pReq->byStart						= PACK_HEAD;
		pReq->byAPDULen						= 0X04;
		pReq->unCtrlArea.CtrlAreaS.Flag		= 0X01;
		pReq->unCtrlArea.CtrlAreaS.Nr		= m_wdNr;
		pReq->unCtrlArea.CtrlAreaS.Ns		= 0;
		pReq->unCtrlArea.CtrlAreaS.Res		= 0;

		return sizeof(APCI);
	}


	// *****************************************************
	// 函数功能: 组总召唤帧
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//    OpcUa_Byte* buf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildWholeQueryFrame( OpcUa_Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		WholeQueryReq* pReq = (WholeQueryReq* )buf;
		ACE_OS::memset( pReq, 0, sizeof(WholeQueryReq) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(WholeQueryReq)-2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_IC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->sWholeQuery.byQOI			= 20;
		pReq->sWholeQuery.wdInfoAddr	= 0;
		pReq->sWholeQuery.byRes			= 0;

		return sizeof(WholeQueryReq);
	}



	// *****************************************************
	// 函数功能: 组计数量召唤帧
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//    OpcUa_Byte* buf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildCountQueryFrame( OpcUa_Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		CountQueryFrame* pReq = (CountQueryFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(CountQueryFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(CountQueryFrame)-2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_CI_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->sCountQuery.sQCC.RQT		= 5;
		pReq->sCountQuery.sQCC.FRZ		= 0;
		pReq->sCountQuery.wdInfoAddr	= 0;
		pReq->sCountQuery.byRes			= 0;

		return sizeof(CountQueryFrame);
	}


	// *****************************************************
	// 函数功能: 组校时帧
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//    OpcUa_Byte* buf: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildCheckTimeFrame( OpcUa_Byte* buf )
	{
		if ( buf == NULL )
		{
			return 0;
		}

		AdjustTime* pReq = (AdjustTime* )buf;
		ACE_OS::memset( pReq, 0, sizeof(AdjustTime) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(AdjustTime) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_CS_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->sAdjustTime.wdInfoAddr	= 0;
		pReq->sAdjustTime.byRes			= 0;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->sAdjustTime.sTime = SystimeToASDUTime( sysTime );

		return sizeof(AdjustTime);
	}



	// *****************************************************
	// 函数功能: 组帧：遥控. 单遥，选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildSDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			//! 写日志
			//pDriver->WriteLog( "控制：传入参数非法！" );
			return 0;
		}

		DOFrame* pReq = (DOFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sSCS.SCS		= *pVal;
		pReq->uElement.sSCS.Res		= 0;
		pReq->uElement.sSCS.QU		= 0;
		pReq->uElement.sSCS.SE		= 1;

		return sizeof(DOFrame);
	}


	// *****************************************************
	// 函数功能: 组帧：遥控. 单遥，执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildSDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrame* pReq = (DOFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sSCS.SCS		= *pVal;
		pReq->uElement.sSCS.Res		= 0;
		pReq->uElement.sSCS.QU		= 0;
		pReq->uElement.sSCS.SE		= 0;

		return sizeof(DOFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥控. 单遥，撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildSDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrame* pReq = (DOFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_SC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sSCS.SCS		= *pVal;
		pReq->uElement.sSCS.Res		= 0;
		pReq->uElement.sSCS.QU		= 0;
		pReq->uElement.sSCS.SE		= 1;

		return sizeof(DOFrame);
	}

	// *****************************************************
	// 函数功能: 组帧：遥控. 双遥，选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildDDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrame* pReq = (DOFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_DC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 1;

		return sizeof(DOFrame);
	}


	// *****************************************************
	// 函数功能: 组帧：遥控. 双遥，执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildDDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrame* pReq = (DOFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_DC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 0;

		return sizeof(DOFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥控. 双遥，撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildDDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrame* pReq = (DOFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_DC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 1;

		return sizeof(DOFrame);
	}

	// *****************************************************
	// 函数功能: 组帧：遥控. 升降，选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildUPDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrame* pReq = (DOFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_RC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 1;

		return sizeof(DOFrame);
	}


	// *****************************************************
	// 函数功能: 组帧：遥控. 升降，执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildUPDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrame* pReq = (DOFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_RC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 0;

		return sizeof(DOFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥控. 升降，撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildUPDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrame* pReq = (DOFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_RC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 1;

		return sizeof(DOFrame);
	}


	// *****************************************************
	// 函数功能: 组帧：遥控. 单遥（带时标），选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTSDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR
			|| ( *pVal != 1 && *pVal != 0 ) )
		{
			return 0;
		}

		DOFrameTime* pReq = (DOFrameTime* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrameTime) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrameTime) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sSCS.SCS		= *pVal;
		pReq->uElement.sSCS.Res		= 0;
		pReq->uElement.sSCS.QU		= 0;
		pReq->uElement.sSCS.SE		= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->uElement.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(DOFrameTime);
	}


	// *****************************************************
	// 函数功能: 组帧：遥控. 单遥（带时标），执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTSDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR
			|| ( *pVal != 1 && *pVal != 0 ) )
		{
			return 0;
		}

		DOFrameTime* pReq = (DOFrameTime* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrameTime) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrameTime) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sSCS.SCS		= *pVal;
		pReq->uElement.sSCS.Res		= 0;
		pReq->uElement.sSCS.QU		= 0;
		pReq->uElement.sSCS.SE		= 0;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->uElement.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(DOFrameTime);
	}




	// *****************************************************
	// 函数功能: 组帧：遥控. 单遥（带时标），撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTSDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR
			|| ( *pVal != 1 && *pVal != 0 ) )
		{
			return 0;
		}

		DOFrameTime* pReq = (DOFrameTime* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrameTime) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrameTime) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_SC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sSCS.SCS		= *pVal;
		pReq->uElement.sSCS.Res		= 0;
		pReq->uElement.sSCS.QU		= 0;
		pReq->uElement.sSCS.SE		= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->uElement.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(DOFrameTime);
	}


	// *****************************************************
	// 函数功能: 组帧：遥控. 双遥（带时标），选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTDDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR
			|| ( *pVal != 1 && *pVal != 2 ) )
		{
			return 0;
		}

		DOFrameTime* pReq = (DOFrameTime* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrameTime) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrameTime) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_DC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->uElement.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(DOFrameTime);
	}


	// *****************************************************
	// 函数功能: 组帧：遥控. 双遥（带时标），执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTDDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR
			|| ( *pVal != 1 && *pVal != 2 ) )
		{
			return 0;
		}

		DOFrameTime* pReq = (DOFrameTime* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrameTime) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrameTime) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_DC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 0;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->uElement.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(DOFrameTime);
	}




	// *****************************************************
	// 函数功能: 组帧：遥控. 双遥（带时标），撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTDDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR
			|| ( *pVal != 1 && *pVal != 2 ) )
		{
			return 0;
		}

		DOFrameTime* pReq = (DOFrameTime* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrameTime) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrameTime) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_DC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->uElement.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(DOFrameTime);
	}


	// *****************************************************
	// 函数功能: 组帧：遥控. 升降（带时标），选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTUPDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrameTime* pReq = (DOFrameTime* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrameTime) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrameTime) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_RC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->uElement.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(DOFrameTime);
	}


	// *****************************************************
	// 函数功能: 组帧：遥控. 升降（带时标），执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTUPDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrameTime* pReq = (DOFrameTime* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrameTime) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrameTime) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_RC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 0;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->uElement.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(DOFrameTime);
	}




	// *****************************************************
	// 函数功能: 组帧：遥控. 升降（带时标），撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTUPDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL || Addr < DO_START_ADDR || Addr > DO_END_ADDR )
		{
			return 0;
		}

		DOFrameTime* pReq = (DOFrameTime* )buf;
		ACE_OS::memset( pReq, 0, sizeof(DOFrameTime) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(DOFrameTime) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_RC_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->uElement.wdInfoAddr	= Addr;
		pReq->uElement.byRes		= 0;
		pReq->uElement.sDCS.DCS		= *pVal;
		pReq->uElement.sDCS.QU		= 0;
		pReq->uElement.sDCS.SE		= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->uElement.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(DOFrameTime);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，归一化值，选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildNVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitFrame* pReq = (RC16bitFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		short shVal					= (short)(*(float*)pVal);
		structNVA sNVA;
		memcpy( &sNVA, &shVal, 2 );
		pReq->RC.sNVA				= sNVA;	//*(structNVA* )(*((short)(*(float*)pVal)));
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		return sizeof(RC16bitFrame);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，归一化值，执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildNVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitFrame* pReq = (RC16bitFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		short shVal					= (short)(*(float*)pVal);
		structNVA sNVA;
		memcpy( &sNVA, &shVal, 2 );
		pReq->RC.sNVA				= sNVA;	//*(structNVA* )(*((short)(*(float*)pVal)));
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 0;

		return sizeof(RC16bitFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥调，归一化值，撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildNVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitFrame* pReq = (RC16bitFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_SE_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		short shVal					= (short)(*(float*)pVal);
		structNVA sNVA;
		memcpy( &sNVA, &shVal, 2 );
		pReq->RC.sNVA				= sNVA;	//*(structNVA* )(*((short)(*(float*)pVal)));
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		return sizeof(RC16bitFrame);
	}


	// *****************************************************
	// 函数功能: 组帧：遥调，标度化值，选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildSVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitFrame* pReq = (RC16bitFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NB_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.sSVA.Val			= (short)(*(float*)pVal);
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		return sizeof(RC16bitFrame);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，标度化值，执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildSVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitFrame* pReq = (RC16bitFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NB_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.sSVA.Val			= (short)(*(float*)pVal);
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 0;

		return sizeof(RC16bitFrame);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，标度化值，撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildSVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitFrame* pReq = (RC16bitFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_SE_NB_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.sSVA.Val			= (short)(*(float*)pVal);
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		return sizeof(RC16bitFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥调，短浮点数，选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildFAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC32bitFrame* pReq = (RC32bitFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC32bitFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC32bitFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NC_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.fValue				= *(float*)pVal;
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		return sizeof(RC32bitFrame);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，短浮点数，执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildFAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC32bitFrame* pReq = (RC32bitFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC32bitFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC32bitFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NC_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.fValue				= *(float*)pVal;
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 0;

		return sizeof(RC32bitFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥调，短浮点数，撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildFAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC32bitFrame* pReq = (RC32bitFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC32bitFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC32bitFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_SE_NC_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.fValue				= *(float*)pVal;
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		return sizeof(RC32bitFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥调，32位字串，选择
	// 算法思想: 104不支持32位字串遥调时的选择动作
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildDWAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		return 0;
	}




	// *****************************************************
	// 函数功能: 组帧：遥调，32位字串，执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildDWAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC32bitstringFrame* pReq = (RC32bitstringFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC32bitstringFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC32bitstringFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_BO_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.dwBSI				= (DWORD)(*(float*)pVal);

		return sizeof(RC32bitstringFrame);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，32位字串，撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildDWAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC32bitstringFrame* pReq = (RC32bitstringFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC32bitstringFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC32bitstringFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_BO_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.dwBSI				= (DWORD)(*(float*)pVal);

		return sizeof(RC32bitstringFrame);
	}

	// *****************************************************
	// 函数功能: 组帧：遥调，归一化值（带时标），选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTNVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitTimeFrame* pReq = (RC16bitTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		short shVal					= (short)(*(float*)pVal);
		structNVA sNVA;
		memcpy( &sNVA, &shVal, 2 );
		pReq->RC.sNVA				= sNVA;	//*(structNVA* )(*((short)(*(float*)pVal)));
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC16bitTimeFrame);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，归一化值（带时标），执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTNVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitTimeFrame* pReq = (RC16bitTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		short shVal					= (short)(*(float*)pVal);
		structNVA sNVA;
		memcpy( &sNVA, &shVal, 2 );
		pReq->RC.sNVA				= sNVA;	//*(structNVA* )(*((short)(*(float*)pVal)));
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 0;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC16bitTimeFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥调，归一化值（带时标），撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTNVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitTimeFrame* pReq = (RC16bitTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_SE_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		short shVal					= (short)(*(float*)pVal);
		structNVA sNVA;
		memcpy( &sNVA, &shVal, 2 );
		pReq->RC.sNVA				= sNVA;	//*(structNVA* )(*((short)(*(float*)pVal)));
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC16bitTimeFrame);
	}


	// *****************************************************
	// 函数功能: 组帧：遥调，标度化值（带时标），选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTSVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitTimeFrame* pReq = (RC16bitTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NB_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.sSVA.Val			= (short)(*(float*)pVal);
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC16bitTimeFrame);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，标度化值（带时标），执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTSVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitTimeFrame* pReq = (RC16bitTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NB_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.sSVA.Val			= (short)(*(float*)pVal);
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 0;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC16bitTimeFrame);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，标度化值（带时标），撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTSVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC16bitTimeFrame* pReq = (RC16bitTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC16bitTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC16bitTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_SE_NB_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.sSVA.Val			= (short)(*(float*)pVal);
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC16bitTimeFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥调，短浮点数（带时标），选择
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTFAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC32bitTimeFrame* pReq = (RC32bitTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC32bitTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC32bitTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NC_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.fValue				= *(float*)pVal;
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC32bitTimeFrame);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，短浮点数（带时标），执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTFAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC32bitTimeFrame* pReq = (RC32bitTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC32bitTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC32bitTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_SE_NC_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.fValue				= *(float*)pVal;
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 0;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC32bitTimeFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥调，短浮点数（带时标），撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTFAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC32bitTimeFrame* pReq = (RC32bitTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC32bitTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC32bitTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_SE_NC_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.fValue				= *(float*)pVal;
		pReq->RC.sQOS.QL			= 0;
		pReq->RC.sQOS.SE			= 1;

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC32bitTimeFrame);
	}




	// *****************************************************
	// 函数功能: 组帧：遥调，32位字串（带时标），选择
	// 算法思想: 104不支持32位字串遥调时的选择动作
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTDWAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		return 0;
	}




	// *****************************************************
	// 函数功能: 组帧：遥调，32位字串（带时标），执行
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTDWAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC32bitstringTimeFrame* pReq = (RC32bitstringTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC32bitstringTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC32bitstringTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_act;
		pHead->byType		= C_BO_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.dwBSI				= (DWORD)(*(float*)pVal);

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC32bitstringTimeFrame);
	}



	// *****************************************************
	// 函数功能: 组帧：遥调，32位字串（带时标），撤销
	// 返 回 值: OpcUa_Int32 : 
	// 参数说明: 
	//     OpcUa_Byte* buf: 
	//     WORD Addr: 
	//    OpcUa_Byte* pVal: 
	// *****************************************************
	OpcUa_Int32 CFepIEC104Device::BuildTDWAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal )
	{
		if ( buf == NULL || pVal == NULL )
		{
			return 0;
		}

		RC32bitstringTimeFrame* pReq = (RC32bitstringTimeFrame* )buf;
		ACE_OS::memset( pReq, 0, sizeof(RC32bitstringTimeFrame) );

		APCI* pAPCI = &pReq->Apci;
		pAPCI->byStart						= PACK_HEAD;
		pAPCI->byAPDULen					= sizeof(RC32bitstringTimeFrame) - 2;
		pAPCI->unCtrlArea.CtrlAreaI.Flag	= 0;
		pAPCI->unCtrlArea.CtrlAreaI.Nr		= m_wdNr;
		pAPCI->unCtrlArea.CtrlAreaI.Ns		= GenNextNs();
		pAPCI->unCtrlArea.CtrlAreaI.Res		= 0;

		ASDUHead* pHead		= &pReq->Head;
		pHead->byLimit		= 1;
		pHead->bSQ			= 0;
		pHead->byTxReason	= CAUSE_deact;
		pHead->byType		= C_BO_NA_1;
		pHead->wdCommAddr	= (WORD)GetAddr();

		pReq->RC.wdInfoAddr			= Addr;
		pReq->RC.byRes				= 0;
		pReq->RC.dwBSI				= (DWORD)(*(float*)pVal);

		ACE_Time_Value sysTime = ACE_OS::gettimeofday();
		pReq->RC.sTime		= SystimeToASDUTime( sysTime );

		return sizeof(RC32bitstringTimeFrame);
	}


	// *****************************************************
	// 函数功能: 把本地时间转化为ASDUTime
	// 返 回 值: ASDUTime : 
	// 参数说明: 
	//    ACE_Time_Value sysTime: 
	// *****************************************************
	ASDUTime CFepIEC104Device::SystimeToASDUTime( ACE_Time_Value sysTime )
	{
		//ACE_Time_Value val = ACE_OS::gettimeofday();
		ACE_Date_Time dateTime;
		dateTime.update( sysTime );
		
		ASDUTime asduTime;
		ACE_OS::memset( &asduTime, 0, sizeof(asduTime));
		asduTime.wMs			= dateTime.second()*1000 + dateTime.microsec()/1000;
		asduTime.stMin.Minute	= (OpcUa_Byte)dateTime.minute(); 
		asduTime.stMin.MinValid	= 1;
		asduTime.stMin.Res		= 0;
		asduTime.stHour.Hours	= (OpcUa_Byte)dateTime.hour();
		asduTime.stHour.Res		= 0;
		asduTime.stHour.RU		= 0;
		asduTime.stDay.MonDay	= (OpcUa_Byte)dateTime.day();
		asduTime.stDay.WeekDay	= (OpcUa_Byte)((dateTime.weekday() ==0)?7:dateTime.weekday());
		asduTime.stMon.Mon		= (OpcUa_Byte)dateTime.month();
		asduTime.stMon.Res		= 0;
		asduTime.stYear.Year	= dateTime.year() % 100;
		asduTime.stYear.Res		= 0;

		return asduTime;
	}


	// *****************************************************
	// 函数功能: 产生下一个发送序号.
	// 算法思想: 0~32768. 超过32738以后归0.
	// 调用模块: 
	// 返 回 值: WORD : 
	// *****************************************************
	WORD CFepIEC104Device::GenNextNs()
	{
		WORD wdNs = m_wdNs;

		m_wdNs++;
		if ( m_wdNs > 32768 )
		{
			m_wdNs = 0;
		}

		return wdNs;
	}


	WORD CFepIEC104Device::GenNextNr()
	{
		WORD wdNr = m_wdNr;

		m_wdNr++;
		if ( m_wdNr > 32768 )
		{
			m_wdNr = 0;
		}

		return wdNr;
	}


	// *****************************************************
	// 函数功能: 修改发送序号. 当收到子站发送来的S格式帧后把帧中的Nr更新到m_wdNs.
	// 返 回 值: void : 该函数不返回任何值
	// 参数说明: 
	//    WORD Ns: 
	// *****************************************************
	void CFepIEC104Device::SetNs( WORD Ns )
	{
		if ( Ns <= 32768 )
		{
			m_wdNs = Ns;
		}
	}


	// *****************************************************
	// 函数功能: 修改接收序号. 原则: 新收到的序号Nr与m_wdNr比较,以下两种情况均更新
	// 算法思想: (1). Nr大于m_wdNr时.(2). m_wdNr = 32768(最大序号)
	// 调用模块: 
	// 返 回 值: void : 该函数不返回任何值
	// 参数说明: 
	//    WORD Nr: 
	// *****************************************************
	void CFepIEC104Device::SetNr( WORD Nr )
	{
		if ( Nr <= 32768 )
		{
			//if ( Nr > m_wdNr || m_wdNr == 32768 )
			{
				m_wdNr = Nr;
			}
		}
	}



	// *****************************************************
	// 函数功能: 是否需要校时了
	// 算法思想: m_TimeCheckTime1初始化为0, 这样确保启动时即校时一次.
	// 返 回 值: bool : 
	// *****************************************************
	bool CFepIEC104Device::IsTimeForCheckTime()
	{
		//是否正在处理其他事务
		if ( m_bWholeQuerying || m_bCountQuerying )
		{
			return false;
		}

		if ( m_bNeedCheckTime )
		{
			m_bNeedCheckTime = false;
			return true;
		}

		return false;
	}


	// *****************************************************
	// 函数功能: 是否总召唤时间到了
	// 返 回 值: bool : 
	// *****************************************************
	bool CFepIEC104Device::IsTimeForWholeQuery()
	{
		//是否正在处理其他事务
		if ( m_bIsCheckTiming || m_bCountQuerying )
		{
			return false;
		}

		if ( m_bNeedWholeQuery )
		{
			m_bNeedWholeQuery = false;

			return true;
		}

		return false;

	}


	// *****************************************************
	// 函数功能: 是否计数量召唤时间到了
	// 返 回 值: bool : 
	// *****************************************************
	bool CFepIEC104Device::IsTimeForCountQuery()
	{
		//是否正在处理其他事务
		if ( m_bIsCheckTiming || m_bWholeQuerying )
		{
			return false;
		}

		if ( m_bNeedCountQuery )
		{
			m_bNeedCountQuery = false;

			return true;
		}

		return false;

	}

	bool CFepIEC104Device::IsNeedSendSFrame()
	{
		m_wCtrlAreaINum++;
		if ( m_wCtrlAreaINum >= m_nCtrlAreaIMax )
		{
			m_wCtrlAreaINum = 0;

			return true;
		}
		else
		{
			return false;
		}
	}


	/*!
	*	\brief	获取标签的整型值，并比较是否与nVal相等，相等则返回false，否则返回true；此处只考虑OpcUa_Byte类型数据，其他类型数据调用此接口会异常
	*/
	bool CFepIEC104Device::IsDIValueChange( OpcUa_Int32 UnitType, OpcUa_Int32 nTagAddr, OpcUa_Byte& byOldVal, OpcUa_Byte& byNewVal, std::string& strTagName )
	{
		OBJTAG ObjTag;
		ObjTag.byUnitType = UnitType;
		ObjTag.lTagAddr = nTagAddr;
		MAP_OBJTAG::iterator it = m_mapObjTag.find( ObjTag.GetIndexAddr() );
		if ( it != m_mapObjTag.end() )
		{
			CIOCfgTag* pObjTag = it->second;
			if ( pObjTag )
			{
				UaVariant variantV = *(pObjTag->GetValue()).value();
				variantV.toByte(byOldVal);
				strTagName = pObjTag->GetName();
				//! 量程转换
				float fRawZero = pObjTag->GetRawEngConfig()[0];
				float fRawFull = pObjTag->GetRawEngConfig()[1];
				float fEngZero = pObjTag->GetRawEngConfig()[2];
				float fEngFull = pObjTag->GetRawEngConfig()[3];
				if ( fabs( fRawFull - fRawZero ) > 0.001 && fabs( fEngFull - fEngZero ) > 0.001  )
				{
					//! 经过量程转换的值
					byNewVal = ( (OpcUa_Double)(OpcUa_Byte)byNewVal - fRawZero)*(fEngFull - fEngZero)/(fRawFull - fRawZero) + fEngZero;
				}

				if (byNewVal != byOldVal)
				{
					return true;
				}
			}
		}

		return false;
	}

	bool CFepIEC104Device::IsAIValueChange( OpcUa_Int32 UnitType, OpcUa_Int32 nTagAddr, OpcUa_Byte& byOldVal, OpcUa_Byte& byNewVal, std::string& strTagName )
	{
		OBJTAG ObjTag;
		ObjTag.byUnitType = UnitType;
		ObjTag.lTagAddr = nTagAddr;
		MAP_OBJTAG::iterator it = m_mapObjTag.find( ObjTag.GetIndexAddr() );
		if ( it != m_mapObjTag.end() )
		{
			CIOCfgTag* pObjTag = it->second;
			if ( pObjTag )
			{
				UaVariant variantV = *(pObjTag->GetValue()).value();
				byOldVal =variantV.toByte(byOldVal);
				strTagName = pObjTag->GetName();
				//! 量程转换
				float fRawZero = pObjTag->GetRawEngConfig()[0];
				float fRawFull = pObjTag->GetRawEngConfig()[1];
				float fEngZero = pObjTag->GetRawEngConfig()[2];
				float fEngFull = pObjTag->GetRawEngConfig()[3];
				if ( fabs( fRawFull - fRawZero ) > 0.001 && fabs( fEngFull - fEngZero ) > 0.001  )
				{
					//! 经过量程转换的值
					byNewVal = ( (OpcUa_Double)(OpcUa_Byte)byNewVal - fRawZero)*(fEngFull - fEngZero)/(fRawFull - fRawZero) + fEngZero;
				}

				if (byNewVal != byOldVal)
				{
					return true;
				}
			}
		}

		return false;
	}

	bool CFepIEC104Device::IsExitTag( OpcUa_Int32 UnitType, OpcUa_Int32 nTagAddr )
	{
		OBJTAG ObjTag;
		ObjTag.byUnitType = UnitType;
		ObjTag.lTagAddr = nTagAddr;
		MAP_OBJTAG::iterator it = m_mapObjTag.find( ObjTag.GetIndexAddr() );
		if ( it != m_mapObjTag.end() )
		{
			CIOCfgTag* pObjTag = it->second;
			if ( pObjTag )
				return true;
		}

		return false;
	}

	void CFepIEC104Device::SetHot( OpcUa_Byte byHot )
	{
		
		MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CFepIEC104Device::SetHot: Device=%s to byHot=%d!" ),
			GetName().c_str(), byHot ));

		CFepIEC104Prtcl* pPrtcl = (CFepIEC104Prtcl*)GetPrtcl();
		if ( pPrtcl )
		{
			char pchTemp[1024]={0};
			sprintf( pchTemp, "CFepIEC104Device::SetHot: Device=%s to byHot=%d!", GetName().c_str(), byHot );
			CFepIEC104Drv* pDriver = (CFepIEC104Drv*)GetDriver();
			pDriver->WriteLogAnyWay( pchTemp );

			if ( byHot )	//简单起见，只要是置为Hot，无论之前是否Hot，均认为需要重新激活StartDT
			{
				pDriver->WriteLogAnyWay( "CFepIEC104Device::SetHot: Need to StartDT" );
				pPrtcl->ForStartDT();
			}
			else
			{
				pDriver->WriteLogAnyWay( "CFepIEC104Device::SetHot: Need to StopDT" );
				pPrtcl->ForStopDT();
			}
		}

		m_pUnitState->byHot = byHot;
	}
}
