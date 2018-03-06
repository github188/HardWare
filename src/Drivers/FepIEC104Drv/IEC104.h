/*!
 *	\file	$FILE_BASE$.$FILE_EXT$
 *
 *	\brief	104主站驱动用到的数据类型定义文件
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/IEC104.h 6     09-09-28 16:41 Miaoweijie $
 *	$Author: Miaoweijie $
 *	$Date: 09-09-28 16:41 $
 *	$Revision: 6 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef	_IEC_104_20080812_H
#define _IEC_104_20080812_H

#pragma pack(push, 1)

//===================所有帧中的时间格式========================================//
//! 分：0-59
typedef struct{
	OpcUa_Byte	Minute:6;
	OpcUa_Byte	Res:1;
	OpcUa_Byte	MinValid:1;		//时间是否有效
}structMin;

//! 时：0-23
typedef struct{
	OpcUa_Byte	Hours:5;
	OpcUa_Byte	Res:2;
	OpcUa_Byte	RU:1;	//0---标准时间；1--－夏时制
}structHour;

//! 天：1-31
typedef struct{
	OpcUa_Byte	MonDay:5;
	OpcUa_Byte	WeekDay:3;
}structDay;

//! 月：1-12
typedef struct{
	OpcUa_Byte	Mon:4;
	OpcUa_Byte	Res:4;
}structMon;

//! 年：0-99
typedef struct{
	OpcUa_Byte	Year:7;
	OpcUa_Byte	Res:1;
}structYear;

//! 所有帧中的时间格式。
typedef struct{
	WORD		wMs;		//毫秒：0-59999
	structMin	stMin;		//分
	structHour	stHour;		//仅对校时有效
	structDay	stDay;		//仅对校时有效
	structMon	stMon;		//仅对校时有效
	structYear	stYear;		//仅对校时有效
}ASDUTime;	

typedef struct {
	WORD		wMs;		//毫秒：0-59999
}Time16bit;

typedef struct {
	WORD		wMs;		//毫秒：0-59999
	structMin	stMin;		//分
}Time24bit;
//===================所有帧中的时间格式========================================//


//===================APCI格式==================================================//
//! APCI控制域，有以下三种格式（I格式，S格式，U格式）
typedef struct {
	OpcUa_Byte byCtrlArea1;
	OpcUa_Byte byCtrlArea2;
	OpcUa_Byte byCtrlArea3;
	OpcUa_Byte byCtrlArea4;
}structCtrlArea;

//! APCI控制域：I格式
typedef struct {
	WORD Flag:1;	//I格式特征码。总是0
	WORD Ns:15;		//发送序号
	WORD Res:1;		//保留。总是0
	WORD Nr:15;		//接收序号
}structCtrlAreaI;

//! APCI控制域：S格式
typedef struct {
	WORD Flag:2;	//S格式特征码。总是：bit0=1，bit1=0。
	WORD Ns:14;		//总是全0。
	WORD Res:1;		//保留。总是0
	WORD Nr:15;		//接收序号
}structCtrlAreaS;

//! APCI控制域：U格式
typedef struct {
	OpcUa_Byte Flag:2;		//U格式特征码。总是：bit0=1，bit1=1。
	OpcUa_Byte STARTDT_ACT:1;	//STARTDT生效. 1--生效(主站置), 下同.
	OpcUa_Byte STARTDT_CON:1;	//STARTDT确认. 1--确认(从站置), 下同.
	OpcUa_Byte STOPDT_ACT:1;	//STOPDT生效
	OpcUa_Byte STOPDT_CON:1;	//STOPDT确认
	OpcUa_Byte TESTFR_ACT:1;	//TESTFR生效
	OpcUa_Byte TESTFR_CON:1;	//TESTFR确认
	OpcUa_Byte Res1:8;		//保留。总是全0。
	WORD Res2:1;		//总是0
	WORD Nr:15;			//总是全0。
}structCtrlAreaU;

typedef union{
	structCtrlArea CtrlArea;
	structCtrlAreaI CtrlAreaI;
	structCtrlAreaS CtrlAreaS;
	structCtrlAreaU CtrlAreaU;
}unionCtrlArea;

//! APCI
typedef struct {
	OpcUa_Byte byStart;				//总是68H
	OpcUa_Byte byAPDULen;				//APDU 长度。最大253。
	unionCtrlArea unCtrlArea;	//见上。
}APCI;
//===================APCI格式==================================================//


//===================ASDU head格式=============================================//
//! 单元公共地址
typedef struct {
	OpcUa_Byte byCommAddr;	//公共地址：组号0-254。
	OpcUa_Byte bySubAddr;		//子站地址
}COMADDR;

//! ASDU头
typedef struct {
	OpcUa_Byte byType;				//类别标识
	OpcUa_Byte byLimit:7;				//可变结构限定词。
	OpcUa_Byte bSQ:1;					//信息对象地址是否连续. 1是指仅第一个点带信息对象地址，后面的不带。0是指每个点均带信息对象地址。
	OpcUa_Byte byTxReason;			//传输原因
	OpcUa_Byte bySrc;					//源地址。一般为0。
	union{
		COMADDR sComAddr;		//单元公共地址
		WORD	wdCommAddr;
	};
}ASDUHead;
//===================ASDU head格式=============================================//

//! 信息对象地址结构
typedef struct {
	WORD wdInfoAddr;			//信息地址：0-65535。
	OpcUa_Byte byRes;					//保留。
}structInfoAddr;


//===================所有遥控帧格式：选择，执行，撤销的下行帧及上行帧==========//
//! 遥控信息元素：SCS(单点)
typedef struct{
	OpcUa_Byte	SCS:1;
	OpcUa_Byte	Res:1;
	OpcUa_Byte	QU:5;
	OpcUa_Byte	SE:1;
}structSCS;


//! 遥控信息元素：DCS(双点)
typedef struct{
	OpcUa_Byte	DCS:2;
	OpcUa_Byte	QU:5;
	OpcUa_Byte	SE:1;
}structDCS;

//! 遥控信息元素：RCS(升降)
typedef struct{
	OpcUa_Byte	RCS:2;
	OpcUa_Byte	QU:5;
	OpcUa_Byte	SE:1;
}structRCS;

//! 遥控信息元素
typedef struct{
	WORD wdInfoAddr;			//信息地址：0-65535。
	OpcUa_Byte byRes;					//保留。
	union{
		structSCS sSCS;
		structDCS sDCS;
		structRCS sRCS;
	};
}unionInfoElement;

//! 遥控信息元素(带时标)
typedef struct{
	WORD wdInfoAddr;			//信息地址：0-65535。
	OpcUa_Byte byRes;					//保留。
	union{
		structSCS sSCS;
		structDCS sDCS;
		structRCS sRCS;
	};
	ASDUTime sTime;
}unionInfoElementTime;

//! 所有遥控帧格式：选择，执行，撤销的下行帧及上行帧。
typedef struct {
	APCI			 Apci;
	ASDUHead		 Head;		//ASDU head.
	unionInfoElement uElement;	//信息元素.
}DOFrame;

//! 所有遥控帧格式：选择，执行，撤销的下行帧及上行帧。(带时标)
typedef struct {
	APCI			 Apci;
	ASDUHead		 Head;		//ASDU head.
	unionInfoElementTime uElement;	//信息元素.
}DOFrameTime;
//===================所有遥控帧格式：选择，执行，撤销的下行帧及上行帧==========//


//! ===================总召唤①下行帧，及子站对总召唤的②确认帧和③结束帧格式===//
typedef struct {
	WORD wdInfoAddr;//信息地址：总是为0.
	OpcUa_Byte byRes;		//保留。
	OpcUa_Byte byQOI;		//ACK和End总是为20，Req为0或20。 
}structWholeQuery;

typedef struct {
	APCI	 Apci;
	ASDUHead Head;	//①byType==100 && byTxReason == 6; 
					//②byType==100 && byTxReason == 7; 
					//③byType==100 && byTxReason == 10.
	structWholeQuery sWholeQuery;
}WholeQueryReq,WholeQueryRespAck,WholeQueryRespEnd;
//===================总召唤①下行帧，及子站对总召唤的②确认帧和③结束帧格式===//


//===================遥信点（包括单点和双点）不带时标格式======================//
/*
1、当ASDUHead.byType == 1 && ASDUHead.byTxReason == 20时为单点遥信点
2、当ASDUHead.byType == 3 && ASDUHead.byTxReason == 20时为双点遥信点

此时上行帧数据结构为：
  APCI + ASDUHead + SPI1 + SPI2 + ... + SPIn
或
  APCI + ASDUHead + DPI1 + DPI2 + ... + DPIn
其中ASDUHead.byLimit指示了遥信点的个数n。
*/

//! 带品质描词的单点信息
typedef struct {
	OpcUa_Byte SPI:1;		//SPI := <0> := OFF开    <1> := ON合
	OpcUa_Byte Res:3;		//保留。总是为0。
	OpcUa_Byte BL:1;		//BL := <0> := 未被锁定  <1> := 被锁定
	OpcUa_Byte SB:1;		//SB := <0> := 未被取代  <1> := 被取代
	OpcUa_Byte NT:1;		//NT := <0> := 当前值    <1> := 非当前值
	OpcUa_Byte IV:1;		//IV := <0> := 有效      <1> := 无效
}structSIQ;

//! 带品质描词的单点信息
typedef struct {
	OpcUa_Byte DPI:2;		//DPI := <1> :=确定状态开OFF  <2> := 确定状态合ON  <0><3> :=中间状态或不确定
	OpcUa_Byte Res:2;		//保留。总是为0。
	OpcUa_Byte BL:1;		//BL := <0> := 未被锁定  <1> := 被锁定
	OpcUa_Byte SB:1;		//SB := <0> := 未被取代  <1> := 被取代
	OpcUa_Byte NT:1;		//NT := <0> := 当前值    <1> := 非当前值
	OpcUa_Byte IV:1;		//IV := <0> := 有效      <1> := 无效
}structDIQ;

//! 单点遥信值数据结构SDI
typedef struct {
	WORD wdInfoAddr;//信息地址：0-65535。
	OpcUa_Byte byRes;					//保留。
	structSIQ sSIQ;
}structSDINoTime;

typedef struct {
	structSIQ sSIQ;
}structSDINoTimeNoInfoAddr;

//! 双点遥信值数据结构DPI
typedef struct {
	WORD wdInfoAddr;//信息地址：0-65535。
	OpcUa_Byte byRes;					//保留。
	structDIQ sDIQ;
}structDDINoTime;

typedef struct {
	structDIQ sDIQ;
}structDDINoTimeNoInfoAddr;
//===================遥信点（包括单点和双点）不带时标格式======================//


//===================遥信点（包括单点和双点）带时标格式========================//
/*
1、当ASDUHead.byType == 2 && ASDUHead.byTxReason == 20时为单点带时标遥信点
2、当ASDUHead.byType == 4 && ASDUHead.byTxReason == 20时为双点带时标遥信点

此时上行帧数据结构为：
  APCI + ASDUHead + SPIT1 + SPIT2 + ... + SPITn
或
  APCI + ASDUHead + DPIT1 + DPIT2 + ... + DPITn
其中ASDUHead.byLimit指示了遥信点的个数n。
*/
//! 单点遥信值数据结构SPIT
typedef struct {
	structSDINoTime sSDINoTime;
	Time24bit  sTime;	
}structSDI_Time24;

typedef struct {
	structSDINoTimeNoInfoAddr sSDINoTime;
	Time24bit  sTime;	
}structSDI_Time24NoInfoAddr;

//! 双点遥信值数据结构DPIT
typedef struct {
	structDDINoTime sDDINoTime;
	Time24bit  sTime;	
}structDDI_Time24;

typedef struct {
	structDDINoTimeNoInfoAddr sDDINoTime;
	Time24bit  sTime;	
}structDDI_Time24NoInfoAddr;

//! 单点遥信值数据结构
typedef struct {
	structSDINoTime sSDINoTime;
	ASDUTime  sTime;	
}structSDI_Time;

typedef struct {
	structSDINoTimeNoInfoAddr sSDINoTime;
	ASDUTime  sTime;	
}structSDI_TimeNoInfoAddr;

//! 双点遥信值数据结构
typedef struct {
	structDDINoTime sDDINoTime;
	ASDUTime  sTime;	
}structDDI_Time;

typedef struct {
	structDDINoTimeNoInfoAddr sDDINoTime;
	ASDUTime  sTime;	
}structDDI_TimeNoInfoAddr;
//===================遥信点（包括单点和双点）带时标格式========================//


//===================遥测点不带时标格式========================================//
/*
1、当ASDUHead.byType == 9, 11  时为不带时标遥测点

  此时上行帧数据结构为：
  APCI + ASDUHead + RT1 + RT2 + ... + RTn
  其中ASDUHead.byLimit指示了遥测点的个数n。
*/
//! 品质描述词
typedef struct {
	OpcUa_Byte OV:1;		//OV := <0> :=未溢出	 <1> :=溢出
	OpcUa_Byte Res:3;		//保留。总是为0。
	OpcUa_Byte BL:1;		//BL := <0> := 未被锁定  <1> := 被锁定
	OpcUa_Byte SB:1;		//SB := <0> := 未被取代  <1> := 被取代
	OpcUa_Byte NT:1;		//NT := <0> := 当前值    <1> := 非当前值
	OpcUa_Byte IV:1;		//IV := <0> := 有效      <1> := 无效
}QDS;

typedef struct {
	WORD Val:15;
	WORD Sign:1;
}structNVA;

typedef struct {
	short Val;
}structSVA;

//! 不带时标遥测量16位RT
typedef struct{
	WORD	wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	union{
		structNVA sNVA;
		structSVA sSVA;
	};
	QDS	Quality;
}structRT16bit;

typedef struct{
	union{
		structNVA sNVA;
		structSVA sSVA;
	};
	QDS	Quality;
}structRT16bitNoInfoAddr;

//! 不带品质描述词、不带时标遥测量16位RT
typedef struct{
	WORD	wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	union{
		structNVA sNVA;
		structSVA sSVA;
	};
}structRT16bitNoQDS;

typedef struct{
	union{
		structNVA sNVA;
		structSVA sSVA;
	};
}structRT16bitNoQDSNoInfoAddr;

//! 不带时标的32位RT
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	float fValue;
	QDS Quality;
}structRT32bit;

typedef struct {
	float fValue;
	QDS Quality;
}structRT32bitNoInfoAddr;

//===================遥测点不带时标格式========================================//

//===================遥测点带时标格式==========================================//
/*
1、当ASDUHead.byType == 10, 12 时为带时标遥测点

  此时上行帧数据结构为：
  APCI + ASDUHead + RTT1 + RTT2 + ... + RTTn
  其中ASDUHead.byLimit指示了遥测点的个数n。
*/
//带时标遥16位测量RTT
typedef struct{
	structRT16bit sRT;
	Time24bit  sTime;	
}structRT16bit_Time24;

typedef struct{
	structRT16bitNoInfoAddr sRT;
	Time24bit  sTime;	
}structRT16bit_Time24NoInfoAddr;

//带时标遥32位测量RTT
typedef struct{
	structRT32bit sRT;
	Time24bit  sTime;	
}structRT32bit_Time24;

typedef struct{
	structRT32bitNoInfoAddr sRT;
	Time24bit  sTime;	
}structRT32bit_Time24NoInfoAddr;

//带时标遥16位测量RTT
typedef struct{
	structRT16bit sRT;
	ASDUTime  sTime;	
}structRT16bit_Time;

typedef struct{
	structRT16bitNoInfoAddr sRT;
	ASDUTime  sTime;	
}structRT16bit_TimeNoInfoAddr;

//带时标遥32位测量RTT
typedef struct{
	structRT32bit sRT;
	ASDUTime  sTime;	
}structRT32bit_Time;

typedef struct{
	structRT32bitNoInfoAddr sRT;
	ASDUTime  sTime;	
}structRT32bit_TimeNoInfoAddr;
//===================遥测点带时标格式==========================================//

//=========================遥调点格式==========================================//
typedef struct {
	OpcUa_Byte QL:7;	//<0>            ：=未用
				//<1..15>            ：=为本配套标准的标准定义保留(兼容范围)
				//<16..31>            ：= 为特定使用保留(专用范围)
	OpcUa_Byte SE:1;	//<0>            ：=执行 
				//<1>            ：＝选择
}structQOS;

//16位遥调
typedef struct{
	WORD	wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	union{
		structNVA sNVA;
		structSVA sSVA;
	};
	structQOS sQOS;
}structRC16bit;


typedef struct {
	APCI		  Apci;
	ASDUHead	  Head;	//ASDU head.
	structRC16bit RC;	
}RC16bitFrame;


//32位遥调
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	float fValue;
	structQOS sQOS;
}structRC32bit;

typedef struct {
	APCI		  Apci;
	ASDUHead	  Head;	//ASDU head.
	structRC32bit RC;	
}RC32bitFrame;


//32位字串遥调
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	DWORD dwBSI;		//二进状态信息,32比特
}structRC32bitstring;

typedef struct {
	APCI		  Apci;
	ASDUHead	  Head;	//ASDU head.
	structRC32bitstring RC;	
}RC32bitstringFrame;


//16位遥调(带时标)
typedef struct{
	WORD	wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	union{
		structNVA sNVA;
		structSVA sSVA;
	};
	structQOS sQOS;
	ASDUTime sTime;
}structRC16bitTime;

typedef struct {
	APCI		  Apci;
	ASDUHead	  Head;	//ASDU head.
	structRC16bitTime RC;	
}RC16bitTimeFrame;


//32位遥调(带时标)
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	float fValue;
	structQOS sQOS;
	ASDUTime sTime;
}structRC32bitTime;

typedef struct {
	APCI		  Apci;
	ASDUHead	  Head;	//ASDU head.
	structRC32bitTime RC;	
}RC32bitTimeFrame;


//32位字串遥调(带时标)
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	DWORD dwBSI;		//二进状态信息,32比特
	ASDUTime sTime;
}structRC32bitstringTime;

typedef struct {
	APCI		  Apci;
	ASDUHead	  Head;	//ASDU head.
	structRC32bitstringTime RC;	
}RC32bitstringTimeFrame;
//=========================遥调点格式==========================================//


//============================SOE格式==========================================//
/*
1、当ASDUHead.byType == 30 && ASDUHead.byTxReason == 3时为单点SOE
2、当ASDUHead.byType == 31 && ASDUHead.byTxReason == 3时为双点SOE

此时上行帧数据结构为：
  APCI + ASDUHead + SPIT1 + SPIT2 + ... + SPITn
或
  APCI + ASDUHead + DPIT1 + DPIT2 + ... + DPITn
其中ASDUHead.byLimit指示了遥信点的个数n。
注意: 时间信息全部有效.
*/
//============================SOE格式==========================================//

//! ============================调压开关位置状态=================================
typedef struct {
	OpcUa_Byte Value:6;	//数值: -64~63.
	OpcUa_Byte Sign:1;	//符号位: 1--负数; 0--正数.
	OpcUa_Byte T:1;		//<0> ：=设备未在瞬变状态      <1> ：=设备处于瞬变状态
}VTI;

//! 获得VTI数值, 
// 例如: 
// VTI a;
// char chVTIVal = GetVTIVal(a);
//#define GetVTIVal(a) ((((VTI)(a)).Sign == 0)?(((VTI)(a)).Value):(-((~(((VTI)(a)).Value))&63)+1) )
#define GetVTIVal(a) ((((VTI)(a)).Sign == 0)?(((VTI)(a)).Value):(-((~((((VTI)(a)).Value))&63)+1)) )


//! 调压开关位置状态变化(ASDUHead.byType == 32 && (ASDUHead.byTxReason == 5,20,33) )
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//! 保留。
	VTI	 sVTI;
	QDS  Quality;
}structVTI;

typedef struct {
	VTI	 sVTI;
	QDS  Quality;
}structVTINoInfoAddr;

//! 调压开关位置状态(ASDUHead.byType == 32 && (ASDUHead.byTxReason == 3,11,12) )
typedef struct {
	structVTI ssVTI;
	Time24bit  sTime;	
}structVTI_Time24;

typedef struct {
	structVTINoInfoAddr ssVTI;
	Time24bit  sTime;	
}structVTI_Time24NoInfoAddr;

typedef struct {
	structVTI ssVTI;
	ASDUTime  sTime;	
}structVTI_Time;

typedef struct {
	structVTINoInfoAddr ssVTI;
	ASDUTime  sTime;	
}structVTI_TimeNoInfoAddr;
//============================调压开关位置状态=================================//

//! ============================子站远程终端状态（32比特串）=====================
typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//! 保留。
	DWORD dwRTS;
	QDS  Quality;
}structRTS;

typedef struct {
	DWORD dwRTS;
	QDS  Quality;
}structRTSNoInfoAddr;

typedef struct {
	structRTS sRTS;
	Time24bit  sTime;	
}structRTS_Time24;

typedef struct {
	structRTSNoInfoAddr sRTS;
	Time24bit  sTime;	
}structRTS_Time24NoInfoAddr;

typedef struct {
	structRTS sRTS;
	ASDUTime  sTime;	
}structRTS_Time;

typedef struct {
	structRTSNoInfoAddr sRTS;
	ASDUTime  sTime;	
}structRTS_TimeNoInfoAddr;
//============================子站远程终端状态（32比特串）=====================//

//! ============================电能累计量=======================================
typedef struct {
	OpcUa_Byte SQ:5;
	OpcUa_Byte CY:1;
	OpcUa_Byte CA:1;
	OpcUa_Byte IV:1;
}structSN;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	OpcUa_UInt32 Val;
	structSN sSN;
}structDNLJ;

typedef struct {
	OpcUa_UInt32 Val;
	structSN sSN;
}structDNLJNoInfoAddr;

typedef struct {
	structDNLJ sDNLJ;
	Time24bit  sTime;	
}structDNLJ_Time24;

typedef struct {
	structDNLJNoInfoAddr sDNLJ;
	Time24bit  sTime;	
}structDNLJ_Time24NoInfoAddr;

typedef struct {
	structDNLJ sDNLJ;
	ASDUTime  sTime;	
}structDNLJ_Time;

typedef struct {
	structDNLJNoInfoAddr sDNLJ;
	ASDUTime  sTime;	
}structDNLJ_TimeNoInfoAddr;
//============================电能累计量=======================================//


//! ============================带时标的继电保护装置事件=========================
typedef struct {
	OpcUa_Byte ES:2;
	OpcUa_Byte Res:2;
	OpcUa_Byte BL:1;
	OpcUa_Byte SB:1;
	OpcUa_Byte NT:1;
	OpcUa_Byte IV:1;
}structSEP;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	structSEP sSEP;
	Time16bit sTime16bit;
}structJDBHNoTime;

typedef struct {
	structSEP sSEP;
	Time16bit sTime16bit;
}structJDBHNoTimeNoInfoAddr;

typedef struct {
	structJDBHNoTime sJDBHNoTime;
	Time24bit sTime;
}structJDBH_Time24;

typedef struct {
	structJDBHNoTimeNoInfoAddr sJDBHNoTime;
	Time24bit sTime;
}structJDBH_Time24NoInfoAddr;

typedef struct {
	structJDBHNoTime sJDBHNoTime;
	ASDUTime sTime;
}structJDBH_Time;

typedef struct {
	structJDBHNoTimeNoInfoAddr sJDBHNoTime;
	ASDUTime sTime;
}structJDBH_TimeNoInfoAddr;
//============================带时标的继电保护装置事件=========================//


//! ===========带时标的继电保护装置成组启动事件==================================
//! 继电保护设备的单个事件
typedef struct {
	OpcUa_Byte GS:1;
	OpcUa_Byte SL1:1;
	OpcUa_Byte SL2:1;
	OpcUa_Byte SL3:1;
	OpcUa_Byte SIE:1;
	OpcUa_Byte SRD:1;
	OpcUa_Byte Res:2;	//总是为0
}structSPE;

//! 继电佯保设备的品质描词
typedef struct {
	OpcUa_Byte Res:4;	//总是为0
	OpcUa_Byte BL:1;
	OpcUa_Byte SB:1;
	OpcUa_Byte NT:1;
	OpcUa_Byte IV:1;
}structQDP;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//! 保留。
	union{
		structSPE sSPE;
		OpcUa_Byte byVal;
	};
	structQDP sQDP;
	Time16bit sTime16bit;
}structJDBHGroupActNoTime;

typedef struct {
	union{
		structSPE sSPE;
		OpcUa_Byte byVal;
	};
	structQDP sQDP;
	Time16bit sTime16bit;
}structJDBHGroupActNoTimeNoInfoAddr;

typedef struct {
	structJDBHGroupActNoTime sJDBHGroupActNoTime;
	Time24bit sTime;
}structJDBHGroupAct_Time24;

typedef struct {
	structJDBHGroupActNoTimeNoInfoAddr sJDBHGroupActNoTime;
	Time24bit sTime;
}structJDBHGroupAct_Time24NoInfoAddr;

typedef struct {
	structJDBHGroupActNoTime sJDBHGroupActNoTime;
	ASDUTime sTime;
}structJDBHGroupAct_Time;

typedef struct {
	structJDBHGroupActNoTimeNoInfoAddr sJDBHGroupActNoTime;
	ASDUTime sTime;
}structJDBHGroupAct_TimeNoInfoAddr;
//===========带时标的继电保护装置成组启动事件==================================//

//! ====================带时标的继电保护装置成组输出电路信息=====================
typedef struct {
	OpcUa_Byte GC:1;
	OpcUa_Byte CL1:1;
	OpcUa_Byte CL2:1;
	OpcUa_Byte CL3:1;
	OpcUa_Byte Res:4;	//! 总是为0
}structOCI;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//! 保留。
	union{
		structOCI sOCI;
		OpcUa_Byte byVal;
	};
	structQDP sQDP;
	Time16bit sTime16bit;
}structJDBHGroupOutputNoTime;

typedef struct {
	union{
		structOCI sOCI;
		OpcUa_Byte byVal;
	};
	structQDP sQDP;
	Time16bit sTime16bit;
}structJDBHGroupOutputNoTimeNoInfoAddr;

typedef struct {
	structJDBHGroupOutputNoTime sJDBHGroupOutputNoTime;
	Time24bit sTime;
}structJDBHGroupOutput_Time24;

typedef struct {
	structJDBHGroupOutputNoTimeNoInfoAddr sJDBHGroupOutputNoTime;
	Time24bit sTime;
}structJDBHGroupOutput_Time24NoInfoAddr;

typedef struct {
	structJDBHGroupOutputNoTime sJDBHGroupOutputNoTime;
	ASDUTime sTime;
}structJDBHGroupOutput_Time;

typedef struct {
	structJDBHGroupOutputNoTimeNoInfoAddr sJDBHGroupOutputNoTime;
	ASDUTime sTime;
}structJDBHGroupOutput_TimeNoInfoAddr;
//====================带时标的继电保护装置成组输出电路信息=====================//

//! ====================带变位检出的成组单点信息=================================
typedef struct {
	WORD wdST;
	WORD wdCD;
}structSCD;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	structSCD sSCD;
	QDS sQDP;
}structDIGroup;

typedef struct {
	structSCD sSCD;
	QDS sQDP;
}structDIGroupNoInfoAddr;
//====================带变位检出的成组单点信息=================================//

//! ============================校时=============================================
typedef struct {
	WORD		wdInfoAddr;	//! 总是为0
	OpcUa_Byte		byRes;		//! 保留。
	ASDUTime	sTime;
}structAdjustTime;

typedef struct {
	APCI		Apci;
	ASDUHead	Head;		//! ASDU head.
	structAdjustTime sAdjustTime;
}AdjustTime;
//============================校时=============================================//

//! ============================初始化===========================================
typedef struct {
	OpcUa_Byte COI:7;	//<0>         ：=当地电源合上
				//<1>         ：=当地手动复位
				//<2>         ：=远方复位
				//<3..31>     ：=为本配套标准的标准定义保留(兼容范围)
				//<32..127>   ：=为特定使用保留(专用范围)

	OpcUa_Byte BS:1;	//<0>         ：=未改变当地参数的初始化
				//! <1>         ：=改变当地参数后的初始化
}structCOI;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	structCOI sCOI;
}structInit;
//============================初始化===========================================

//! ===========================计数量召唤命令====================================
typedef struct {
	OpcUa_Byte RQT:6; //! <0>            ：＝无请求计数量（未采用）
				//! <1>            ：＝请求计数量第1组          
				//! <2>            ：＝请求计数量第2组            
				//! <3>            ：＝请求计数量第3组         
				//! <4>            ：＝请求计数量第4组      
				//! <5>            ：＝总的请求计数量  
				//! <6..31>        ：＝为本配套标准的标准定义保留(兼容范围)
				//! <32..63>       ：＝为特定使用保留(专用范围)

	OpcUa_Byte FRZ:2;	//! <0>            ：＝读(无冻结和复位）
				//! <1>            ：＝计数量冻结不带复位（被冻结的值代表累计）          
				//! <2>            ：＝计数量冻结带复位（被冻结的值代表增量信息）            
				//! <3>            ：＝计数量复位

}structQCC;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	structQCC sQCC;
}structCountQuery;

typedef struct {
	APCI		Apci;
	ASDUHead	Head;		//ASDU head.
	structCountQuery sCountQuery;
}CountQueryFrame;

//===========================计数量召唤命令====================================//

//! =============================测试命令========================================
typedef struct {
	WORD  wdInfoAddr;	//! 0
	OpcUa_Byte byRes;			//! 保留。
	WORD wdFBP;			//! 固定为0x55AA
}structTest;

typedef struct {
	WORD  wdInfoAddr;	//! 0
	OpcUa_Byte byRes;			//! 保留。
	WORD wdFBP;			//! 固定为0x55AA
	ASDUTime sTime;
}structTestTime;

typedef struct {
	APCI		Apci;
	ASDUHead	Head;		//! ASDU head.
	structTest  sTest;
}TestFrame;

typedef struct {
	APCI		Apci;
	ASDUHead	Head;		//! ASDU head.
	structTestTime  sTest;
}TestFrameTime;
//=============================测试命令========================================

//! ============================复位进程命令=====================================
typedef struct {
	WORD  wdInfoAddr;//! 0
	OpcUa_Byte byRes;		 //! 保留。
	OpcUa_Byte byQRP;		 //! <0>        ：＝未采用
					 //! <1>        ：＝进程的总复位
					 //! <2>        ：＝复位事件缓冲区等待处理的带时标的信息
					 //! <3..127>   ：＝为本配套标准的标准定义保留(兼容范围)      
					 //! <128..255> ：＝为特定使用保留(专用范围)

}structResetProc;
//============================复位进程命令=====================================

//! 延时获得命令
typedef struct {
	WORD  wdInfoAddr; //! 0
	OpcUa_Byte byRes;		  //! 保留。
	Time16bit time16;
}structDelay;

//! ============================测量值参数=======================================
//! 测量值参数限定词(QPM)
//! 门限值是引起新的测量值传输所要求的测量值最小改变量。
//! 传输的上下限值是这样的值，超过它将引起测量值传输。
//! 每个系统由唯一的信息对象地址为每一种参数进行定义。
typedef struct {
	OpcUa_Byte KPA:6;		//! <0>            ：＝未用
					//! <1>            ：＝门限值        
					//! <2>            ：＝平滑系数(滤波时间常数)      
					//! <3>            ：＝测量值传送的上限        
					//! <4>            ：＝测量值传送的下限      
					//! <5..31>        ：＝为本配套标准的标准定义保留(兼容范围)
					//! <32..63>       ：＝为特定使用保留(专用范围)

	OpcUa_Byte LOP1:1;	//当地参数改变: <0> ：=未改变	<1>   ：＝改变
	OpcUa_Byte LOP2:1;	//参数在运行:   <0> ：=运行     <1>   ：＝未运行
}structQPM;

//! 16位测量值参数
typedef struct{
	WORD	wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	union{
		structNVA sNVA;
		structSVA sSVA;
	};
	structQPM sQPM;
}structPara16bit;

typedef struct{
	union{
		structNVA sNVA;
		structSVA sSVA;
	};
	structQPM sQPM;
}structPara16bitNoInfoAddr;

//! 32位测量值参数
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	float fValue;
	structQPM sQPM;
}structPara32bit;

typedef struct {
	float fValue;
	structQPM sQPM;
}structPara32bitNoInfoAddr;
//============================测量值参数=======================================//


//! ============================参数激活=========================================//
typedef struct {
	OpcUa_Byte QPA:6; //<0>            ：＝未用
				//<1>            ：＝激活/仃止激活这之前装载的参数(信息对象地址=0)1)     
				//<2>            ：＝激活/仃止激活所寻址信息对象的参数1)
				//<3>            ：＝激活/仃止激活所寻址的持续循环或周期传输的信息对象       
				//<4..31>        ：＝为本配套标准的标准定义保留(兼容范围)
				//<32..63>       ：＝为特定使用保留(专用范围)

	OpcUa_Byte Res:2;
}structQPA;

typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//保留。
	structQPA sQPA;
}structParaAct;
//! ============================参数激活=========================================//

//! 标签类型UnitType
enum{
	MACS_IEC104_DI = 1,	//! 遥信
	MACS_IEC104_AI,		//! 遥测
	MACS_IEC104_DOF,	//! 返信. 所有遥控/遥调点共用一个返信点.1表示成功,0表示失败或正在等待返回结果.
	MACS_IEC104_SDOa,	//! 遥控. 单遥，选择
	MACS_IEC104_SDOb,	//! 遥控. 单遥，执行
	MACS_IEC104_SDOc,	//! 遥控. 单遥，撤销
	MACS_IEC104_DDOa,	//! 遥控. 双遥，选择
	MACS_IEC104_DDOb,	//! 遥控. 双遥，执行
	MACS_IEC104_DDOc,	//! 遥控. 双遥，撤销
	MACS_IEC104_UPDOa,	//! 遥控. 升降，选择
	MACS_IEC104_UPDOb,	//! 遥控. 升降，执行
	MACS_IEC104_UPDOc,	//! 遥控. 升降，撤销
	MACS_IEC104_TSDOa,	//! 遥控. 单遥（带时标），选择
	MACS_IEC104_TSDOb,	//! 遥控. 单遥（带时标），执行
	MACS_IEC104_TSDOc,	//! 遥控. 单遥（带时标），撤销
	MACS_IEC104_TDDOa,	//! 遥控. 双遥（带时标），选择
	MACS_IEC104_TDDOb,	//! 遥控. 双遥（带时标），执行
	MACS_IEC104_TDDOc,	//! 遥控. 双遥（带时标），撤销
	MACS_IEC104_TUPDOa,	//! 遥控. 升降（带时标），选择
	MACS_IEC104_TUPDOb,	//! 遥控. 升降（带时标），执行
	MACS_IEC104_TUPDOc,	//! 遥控. 升降（带时标），撤销
	MACS_IEC104_NVAAOa,	//! 遥调，归一化值，选择
	MACS_IEC104_NVAAOb,	//! 遥调，归一化值，执行
	MACS_IEC104_NVAAOc,	//! 遥调，归一化值，撤销
	MACS_IEC104_SVAAOa,	//! 遥调，标度化值，选择
	MACS_IEC104_SVAAOb,	//! 遥调，标度化值，执行
	MACS_IEC104_SVAAOc,	//! 遥调，标度化值，撤销
	MACS_IEC104_FAOa,	//! 遥调，短浮点数，选择
	MACS_IEC104_FAOb,	//! 遥调，短浮点数，执行
	MACS_IEC104_FAOc,	//! 遥调，短浮点数，撤销
	MACS_IEC104_DWAOa,	//! 遥调，32位字串，选择
	MACS_IEC104_DWAOb,	//! 遥调，32位字串，执行
	MACS_IEC104_DWAOc,	//! 遥调，32位字串，撤销
	MACS_IEC104_TNVAAOa,	//! 遥调，归一化值（带时标），选择
	MACS_IEC104_TNVAAOb,	//! 遥调，归一化值（带时标），执行
	MACS_IEC104_TNVAAOc,	//! 遥调，归一化值（带时标），撤销
	MACS_IEC104_TSVAAOa,	//! 遥调，标度化值（带时标），选择
	MACS_IEC104_TSVAAOb,	//! 遥调，标度化值（带时标），执行
	MACS_IEC104_TSVAAOc,	//! 遥调，标度化值（带时标），撤销
	MACS_IEC104_TFAOa,	//! 遥调，短浮点数（带时标），选择
	MACS_IEC104_TFAOb,	//! 遥调，短浮点数（带时标），执行
	MACS_IEC104_TFAOc,	//! 遥调，短浮点数（带时标），撤销
	MACS_IEC104_TDWAOa,	//! 遥调，32位字串（带时标），选择
	MACS_IEC104_TDWAOb,	//! 遥调，32位字串（带时标），执行
	MACS_IEC104_TDWAOc,	//! 遥调，32位字串（带时标），撤销

	MACS_IEC104_JDBH = MACS_IEC104_AI,	//! 继电保护
	MACS_IEC104_JDBHX = MACS_IEC104_AI,	//! 继电保护(扩展)
	MACS_IEC104_CS = MACS_IEC104_AI,	//! 参数
	MACS_IEC104_SD = MACS_IEC104_AI,	//! 设定
	MACS_IEC104_DNLJ = MACS_IEC104_AI,	//! 电能累计量
	MACS_IEC104_BWZ = MACS_IEC104_AI,	//! 步位置信息
	MACS_IEC104_EJZ = MACS_IEC104_AI,	//! 向被控站传送二进制信息
	MACS_IEC104_BCD = MACS_IEC104_AI,	//! BCD 码
	MACS_IEC104_RTS = MACS_IEC104_AI,	//! 远动终端状态. DWORD(32位)
	MACS_IEC104_FT = MACS_IEC104_AI		//! 文件传送
};

//各种类型点的起始位置( 该类型数据位置 = 信息对象地址-该类型起始地址 )、结束位置及最大点个数.
/*
#define	DI_START_ADDR	0X01		//遥信∶信息对象地址范围为1H~400H。
#define DI_END_ADDR		0X400		//遥信∶信息对象地址范围为1H~400H。	
#define MAX_DI_NUM		(DI_END_ADDR-DI_START_ADDR+1)	//1024
#define	AI_START_ADDR	0X701		//遥测∶信息对象地址范围为701H~900H
#define	AI_END_ADDR		0X900		//遥测∶信息对象地址范围为701H~900H
#define MAX_AI_NUM		(AI_END_ADDR-AI_START_ADDR+1)	//512
#define DO_START_ADDR	0XB01		//遥控、升降地址范围为B01H~B80H。
#define DO_END_ADDR		0XB80		//遥控、升降地址范围为B01H~B80H。
#define MAX_DO_NUM		(DO_END_ADDR-DO_START_ADDR+1)	//128
#define JDBH_START_ADDR1 0X401		//继电保护∶信息对象地址范围为401H~700H，
									//如果此地址范围不够用还可扩充使用地址范围E01H~FFFH。
#define JDBH_END_ADDR1  0X700		//继电保护∶信息对象地址范围为401H~700H，
									//如果此地址范围不够用还可扩充使用地址范围E01H~FFFH。
#define MAX_JDBH_NUM1	(JDBH_END_ADDR1-JDBH_START_ADDR1+1)	//768
#define JDBH_START_ADDR2 0XE01		//继电保护∶信息对象地址范围为401H~700H，
									//如果此地址范围不够用还可扩充使用地址范围E01H~FFFH。
#define JDBH_END_ADDR2  0XFFF		//继电保护∶信息对象地址范围为401H~700H，
									//如果此地址范围不够用还可扩充使用地址范围E01H~FFFH。
#define MAX_JDBH_NUM2	(JDBH_END_ADDR2-JDBH_START_ADDR2+1)	//768
#define CS_START_ADDR	0X901		//参数地址范围为901H~B00H。
#define CS_END_ADDR		0XB00		//参数地址范围为901H~B00H。
#define MAX_CS_NUM		(CS_END_ADDR-CS_START_ADDR+1)	//512
#define SD_START_ADDR	0XB81		//设定地址范围为B81H~C00H。
#define SD_END_ADDR		0XC00		//设定地址范围为B81H~C00H。
#define MAX_SD_NUM		(SD_END_ADDR-SD_START_ADDR+1)	//128
#define DNLJ_START_ADDR 0XC01		//电能累计量地址范围为C01H~C80H。
#define DNLJ_END_ADDR	0XC81		//电能累计量地址范围为C01H~C80H。
#define MAX_DNLJ_NUM	(DNLJ_END_ADDR-DNLJ_START_ADDR+1)	//128
#define BWZ_START_ADDR	0XC81		//步位置信息地址范围为C81H~CA0H
#define BWZ_END_ADDR	0XCA0		//步位置信息地址范围为C81H~CA0H
#define MAX_BWZ_NUM		(BWZ_END_ADDR-BWZ_START_ADDR+1)		//32
#define EJZ_START_ADDR	0XCA1		//向被控站传送二进制信息地址范围为CA1H~CB0H。 
#define EJZ_END_ADDR	0XCB0		//向被控站传送二进制信息地址范围为CA1H~CB0H。 
#define MAX_EJZ_NUM		(EJZ_END_ADDR-EJZ_START_ADDR+1)		//16
#define BCD_START_ADDR	0XCB1		//BCD 码地址范围为 CB1H~CC0H。 
#define BCD_END_ADDR	0XCC0		//BCD 码地址范围为 CB1H~CC0H。 
#define MAX_BCD_NUM		(BCD_END_ADDR-BCD_START_ADDR+1)	
#define RTS_START_ADDR	0XCC1		//远动终端状态地址CC1H。 
#define RTS_END_ADDR	0XCC1		//远动终端状态地址CC1H。
#define MAX_RTS_NUM		(RTS_END_ADDR-RTS_START_ADDR+1) 
#define FT_START_ADDR	0XCC2		//文件传送地址CC2H~E00H。
#define FT_END_ADDR		0XE00		//文件传送地址CC2H~E00H。
#define MAX_FT_NUM		(FT_END_ADDR-FT_START_ADDR+1)
*/
//各种类型点的起始位置( 该类型数据位置 = 信息对象地址-该类型起始地址 )、结束位置及最大点个数.
/*	//Del by hxw in 2008.05.15
#define	DI_START_ADDR	0X0001		//遥信∶信息对象地址范围
#define DI_END_ADDR		0X1000		//遥信∶信息对象地址范围	
#define MAX_DI_NUM		(DI_END_ADDR-DI_START_ADDR+1)	
#define	AI_START_ADDR	0X4001		//遥测∶信息对象地址范围
#define	AI_END_ADDR		0X5000		//遥测∶信息对象地址范围
#define MAX_AI_NUM		(AI_END_ADDR-AI_START_ADDR+1)	
#define DO_START_ADDR	0X6001		//遥控、升降地址范围
#define DO_END_ADDR		0X6200		//遥控、升降地址范围
#define MAX_DO_NUM		(DO_END_ADDR-DO_START_ADDR+1)
#define JDBH_START_ADDR 0X1001		//继电保护∶信息对象地址范围
#define JDBH_END_ADDR   0X4000		//继电保护∶信息对象地址范围
#define MAX_JDBH_NUM	(JDBH_END_ADDR-JDBH_START_ADDR+1)
#define CS_START_ADDR	0X5001		//参数地址范围
#define CS_END_ADDR		0X6000		//参数地址范围
#define MAX_CS_NUM		(CS_END_ADDR-CS_START_ADDR+1)	
#define SD_START_ADDR	0X6201		//设定地址范围
#define SD_END_ADDR		0X6400		//设定地址范围
#define MAX_SD_NUM		(SD_END_ADDR-SD_START_ADDR+1)
#define DNLJ_START_ADDR 0X6401		//电能累计量地址范围
#define DNLJ_END_ADDR	0X6600		//电能累计量地址范围
#define MAX_DNLJ_NUM	(DNLJ_END_ADDR-DNLJ_START_ADDR+1)	
#define BWZ_START_ADDR	0X6601		//步位置信息地址范围
#define BWZ_END_ADDR	0X6700		//步位置信息地址范围
#define MAX_BWZ_NUM		(BWZ_END_ADDR-BWZ_START_ADDR+1)		
#define EJZ_START_ADDR	0X6701		//向被控站传送二进制信息地址范围
#define EJZ_END_ADDR	0X6800		//向被控站传送二进制信息地址范围
#define MAX_EJZ_NUM		(EJZ_END_ADDR-EJZ_START_ADDR+1)	
#define RTS_START_ADDR	0X6801		//远动终端状态地址
#define RTS_END_ADDR	0X6801		//远动终端状态地址
#define MAX_RTS_NUM		(RTS_END_ADDR-RTS_START_ADDR+1) 
#define FT_START_ADDR	0X6802		//文件传送地址
#define FT_END_ADDR		0X7000		//文件传送地址
#define MAX_FT_NUM		(FT_END_ADDR-FT_START_ADDR+1)
*/
#define	DI_START_ADDR	0X0001		//! 遥信∶信息对象地址范围
#define DI_END_ADDR		0XFFFF		//! 遥信∶信息对象地址范围
#define MAX_DI_NUM		(DI_END_ADDR-DI_START_ADDR+1)	
#define	AI_START_ADDR	0X0001		//! 遥测∶信息对象地址范围
#define	AI_END_ADDR		0XFFFF		//! 遥测∶信息对象地址范围
#define MAX_AI_NUM		(AI_END_ADDR-AI_START_ADDR+1)	
#define DO_START_ADDR	0X0001		//! 遥控、升降地址范围
#define DO_END_ADDR		0XFFFF		//! 遥控、升降地址范围
#define MAX_DO_NUM		(DO_END_ADDR-DO_START_ADDR+1)
#define JDBH_START_ADDR 0X0001		//! 继电保护∶信息对象地址范围
#define JDBH_END_ADDR   0XFFFF		//! 继电保护∶信息对象地址范围
#define MAX_JDBH_NUM	(JDBH_END_ADDR-JDBH_START_ADDR+1)
#define CS_START_ADDR	0X0001		//! 参数地址范围
#define CS_END_ADDR		0XFFFF		//! 参数地址范围
#define MAX_CS_NUM		(CS_END_ADDR-CS_START_ADDR+1)	
#define SD_START_ADDR	0X0001		//! 设定地址范围
#define SD_END_ADDR		0XFFFF		//! 设定地址范围
#define MAX_SD_NUM		(SD_END_ADDR-SD_START_ADDR+1)
#define DNLJ_START_ADDR 0X0001		//! 电能累计量地址范围
#define DNLJ_END_ADDR	0XFFFF		//! 电能累计量地址范围
#define MAX_DNLJ_NUM	(DNLJ_END_ADDR-DNLJ_START_ADDR+1)	
#define BWZ_START_ADDR	0X0001		//! 步位置信息地址范围
#define BWZ_END_ADDR	0XFFFF		//! 步位置信息地址范围
#define MAX_BWZ_NUM		(BWZ_END_ADDR-BWZ_START_ADDR+1)		
#define EJZ_START_ADDR	0X0001		//! 向被控站传送二进制信息地址范围
#define EJZ_END_ADDR	0XFFFF		//! 向被控站传送二进制信息地址范围
#define MAX_EJZ_NUM		(EJZ_END_ADDR-EJZ_START_ADDR+1)	
#define RTS_START_ADDR	0X0001		//! 远动终端状态地址
#define RTS_END_ADDR	0XFFFF		//! 远动终端状态地址
#define MAX_RTS_NUM		(RTS_END_ADDR-RTS_START_ADDR+1) 
#define FT_START_ADDR	0X0001		//! 文件传送地址
#define FT_END_ADDR		0XFFFF		//! 文件传送地址
#define MAX_FT_NUM		(FT_END_ADDR-FT_START_ADDR+1)


//! IEC8705-104规约报文APDU长度限制
#define MIN_APDU_LEN		4
#define MAX_APDU_LEN		253
#define MIN_I_APDU_LEN		12		//! I格式最小帧长(总召唤确认, 总召唤结束)	

//! IEC8705-104规约报文起始码,起始码
#define PACK_HEAD			 0x68 //! 可变帧长报文起始码
#define YC_RFAC                24 //! 遥测量的压缩因子(32/8=4)

//! IEC8705-104规约控制方向报文中的功能码
#define MFUN_ResetLink       0x00 //! 复位远方链路
#define MFUN_ResetProcess    0x01 //! 复位用户进程
#define MFUN_TestLink        0x02 //! 测试链路
#define MFUN_SendData3       0x03 //! 传送数据(要确认)
#define MFUN_SendData4       0x04 //! 传送数据(无需确认)
#define MFUN_Visit           0x08 //! 响应帧应说明访问要求
#define MFUN_CallLinkStatus  0x09 //! 召唤链路状态
#define MFUN_Request1        0x0a //! 召唤1级数据
#define MFUN_Request2        0x0b //! 召唤2级数据

//! IEC8705-104规约监视方向报文中的功能码
#define RFUN_Confirm         0x00 //! 确认
#define RFUN_LinkBUsy        0x01 //! 链路忙
#define RFUN_RespondWithData 0x08 //! 以数据响应请求
#define RFUN_DataWithoutCall 0x09 //! 无所召唤的数据
#define RFUN_LinkStatus      0x0b //! 唤链路状态
#define RFUN_LinkNotWork     0x0e //! 链路服务未工作
#define RFUN_LinkNotFinish   0x0f //! 链路服务未完成

#define RDIR                 0x80 //! 控制域字节的bit7=1,表示传输方向为RTU->主站
#define RACD                 0x20 //! 控制域字节的bit5=1,表示RTU中当前有突发产生的1级数据

//! IEC8705-104规约控制方向的类型标识
#define C_SC_NA_1            45   //! 单点遥控命令
#define C_DC_NA_1            46   //! 双点遥控命令
#define C_RC_NA_1            47   //! 升降命令
#define C_SE_NA_1            48   //! 设定命令, 规一化值
#define C_SE_NB_1			 49   //! 设定命令, 标度化值
#define C_SE_NC_1		     50   //! 设定命令, 短浮点数
#define C_BO_NA_1			 51   //! 设定命令, 32比特串
#define C_SC_TA_1			 58	  //! 带时标CP56Time2a的单命令
#define C_DC_TA_1			 59	  //! 带时标CP56Time2a的双命令
#define C_RC_TA_1			 60   //! 带时标CP56Time2a的升降命令
#define C_SE_TA_1			 61   //! 带时标CP56Time2a的设定值命令,规一化值
#define C_SE_TB_1			 62	  //! 带时标CP56Time2a的设定值命令,标度化值
#define C_SE_TC_1			 63	  //! 带时标CP56Time2a的设定值命令,短浮点数
#define C_BO_TA_1			 64	  //! 带时标CP56Time2a的32比特串


#define C_IC_NA_1            100  //! 召唤命令
#define C_CI_NA_1            101  //! 电能脉冲召唤命令
#define C_RD_NA_1            102  //! 读数据命令
#define C_CS_NA_1            103  //! 时钟同步命令
#define C_TS_NA_1            104  //! 测试命令
#define C_RP_NA_1            105  //! 复位进程命令
#define C_CD_NA_1			 106  //! 延时获得命令
#define C_TS_TA_1			 107  //! 带时标CP56Time2a的测试命令


//! 在监视方向参数的应用服务数据单元
#define P_ME_NA_1			 110 //! 测量值参数, 归一化值
#define P_ME_NB_1			 111 //! 测量值参数,  标度化值
#define P_ME_NC_1			 112 //! 测量值参数,  短浮点数
#define P_AC_NA_1			 113 //! 参数激活

//! 文件传输的应用服务数据单元
#define F_FR_NA_1			 120 //! 文件准备就绪
#define F_SR_NA_1			 121 //! 节准备就绪
#define F_SC_NA_1			 122 //! 召唤目录, 选择文件, 召唤文件,召唤节
#define F_LS_NA_1			 123 //! 最后的节,最后的段
#define F_AF_NA_1			 124 //! 认可文件, 认可节
#define F_SG_NA_1			 125 //! 段
#define F_DR_TA_1			 126 //! 目录


//! IEC8705-104规约监视方向的类型标识
#define M_SP_NA_1            0x01 //! 不带时标的单遥
#define M_SP_TA_1            0x02 //! 带时标的单遥
#define M_DP_NA_1            0x03 //! 不带时标的双遥
#define M_DP_TA_1            0x04 //! 带时标的双遥
#define M_ST_NA_1            0x05 //! 不带时表的步位置信息(变压器分接头)
#define M_ST_TA_1            0x06 //! 带时表的步位置信息(变压器分接头)
#define M_BO_NA_1            0x07 //! 子站远动终端状态(32位位串)
#define M_BO_TA_1            0x08 //! 带时标的子站远动终端状态(32位位串)
#define M_ME_NA_1            0x09 //! 不带时标的普通测量值
#define M_ME_TA_1            0x0a //! 带时标的普通测量值
#define M_ME_NB_1            0x0b //! Measured value,scaled value 
#define M_ME_TB_1            0x0c //! Measured value,scaled value with time tag 
#define M_ME_NC_1            0x0d //! 不带时标的短浮点测量值
#define M_ME_TC_1            0x0e //! 带时标的短浮点测量值
#define M_IT_NA_1            0x0f //! 不带时标的电能脉冲计数
#define M_IT_TA_1            0x10 //! 带时标的电能脉冲计数
#define M_EP_TA_1            0x11 //! 带时标的单个保护设备事件
#define M_EP_TB_1            0x12 //! 带时标的成组保护设备事件
#define M_EP_TC_1			 0x13 //! 带时标的继电保护装置成组输出电路信息
#define M_PS_NA_1            0x14 //! 具有状态变位检出的成组单点信息
#define M_ME_ND_1            0x15 //! 不带品质描述的普通测量值
#define M_SP_TB_1			 0x1E //! 带时标CP56Time2a的单点信息
#define M_DP_TB_1			 0x1F //! 带时标CP56Time2a的双点信息
#define M_ST_TB_1		     0x20 //! 带时标的步位置信息
#define M_BO_TB_1            0x21 //! 带时标CP56Time2a的32比特串
#define M_ME_TD_1			 0x22 //! 测量值, 带时标CP56Time2a的规一化值
#define M_ME_TE_1			 0x23 //! 测量值, 带时标CP56Time2a的标度化值
#define M_ME_TF_1			 0x24 //! 测量值, 带时标CP56Time2a的短浮点数
#define M_IT_TB_1			 0x25 //! 带时标CP56Time2a的累计量
#define M_EP_TD_1			 0X26 //! 带时标CP56Time2a的继电保护装置事件
#define M_EP_TE_1			 M_EP_TB_1	
#define M_EP_TF_1			 40   //! 带时标CP56Time2a的继电保护装置成组输出电路信息
#define M_EI_NA_1            0x46 //! 初始化结束(70)
#define M_BD_NA_1            0xe8 //! BCD码的水位值

//! IEC8705-104规约的传送原因
#define CAUSE_cyc            0x01 //! 周期循环
#define CAUSE_back           0x02 //! 背景扫描
#define CAUSE_spont          0x03 //! 突发
#define CAUSE_init           0x04 //! 初始化
#define CAUSE_req            0x05 //! 请求或被请求
#define CAUSE_act            0x06 //! 激活
#define CAUSE_actcon         0x07 //! 激活确认
#define CAUSE_deact          0x08 //! 停止激活
#define CAUSE_daactcon       0x09 //! 停止激活确认
#define CAUSE_actterm        0x0a //! 激活结束
#define CAUSE_retrem         0x0b //! 远程命令引起的返送信息
#define CAUSE_retloc         0x0c //! 当地命令引起的返送信息
#define CAUSE_file           0x0d //! 文件传送
#define CAUSE_OpcUa_Introgen       0x14 //! 响应总召唤(20)
#define CAUSE_OpcUa_Intro1         0x15 //! 响应第1组召唤(21)
#define CAUSE_OpcUa_Intro2         0x16 //! 响应第2组召唤(22)
#define CAUSE_OpcUa_Intro3         0x17 //! 响应第3组召唤(23)
#define CAUSE_OpcUa_Intro4         0x18 //! 响应第4组召唤(24)
#define CAUSE_OpcUa_Intro5         0x19 //! 响应第5组召唤(25)
#define CAUSE_OpcUa_Intro6         0x1a //! 响应第6组召唤(26)
#define CAUSE_OpcUa_Intro7         0x1b //! 响应第7组召唤(27)
#define CAUSE_OpcUa_Intro8         0x1c //! 响应第8组召唤(28)
#define CAUSE_OpcUa_Intro9         0x1d //! 响应第9组召唤(29)
#define CAUSE_OpcUa_Intro10        0x1e //! 响应第10组召唤(30)
#define CAUSE_OpcUa_Intro11        0x1f //! 响应第11组召唤(31)
#define CAUSE_OpcUa_Intro12        0x20 //! 响应第12组召唤(32)
#define CAUSE_OpcUa_Intro13        0x21 //! 响应第13组召唤(33)
#define CAUSE_OpcUa_Intro14        0x22 //! 响应第14组召唤(34)
#define CAUSE_OpcUa_Intro15        0x23 //! 响应第15组召唤(35)
#define CAUSE_OpcUa_Intro16        0x24 //! 响应第16组召唤(36)
#define CAUSE_reqcogen       0x25 //! 响应电度总召唤(37)
#define CAUSE_reqco1         0x26 //! 响应第1组电度召唤(38)
#define CAUSE_reqco2         0x27 //! 响应第2组电度召唤(39)
#define CAUSE_reqco3         0x28 //! 响应第3组电度召唤(40)
#define CAUSE_reqco4         0x29 //! 响应第4组电度召唤(41)

#pragma pack(pop)

#endif
