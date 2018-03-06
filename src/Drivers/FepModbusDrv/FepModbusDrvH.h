/*!
 *	\file	FepModbusDrvH.h
 *
 *	\brief	通用Modbus驱动中共用的宏和结构定义文件
 *
 *	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusDrvH.h, 470+1 2011/04/22 01:56:47 xuhongtao $
 *	$Author: xuhongtao $
 *	$Date: 2011/04/22 01:56:47 $
 *	$Revision: 470+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _FEP_MODBUS_DRVH_20081103_
#define _FEP_MODBUS_DRVH_20081103_

#ifdef FEPMODBUSDRV_EXPORTS
#define FEPMODBUSDRV_API MACS_DLL_EXPORT
#else
#define FEPMODBUSDRV_API MACS_DLL_IMPORT
#endif

#include <string>
#include <vector>
#include <map>
#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/GenericFactory.h"
#include "RTDBCommon/Debug_Msg.h"
#include "RTDBCommon/RuntimeCatlog.h"
//#include "Common/IOTimer/IOTimer.h"
#include "RTDBDriverFrame/IODeviceBase.h"
#include "RTDBDriverFrame/IOCfgUnit.h"
#include "RTDBDriverFrame/IOCfgTag.h"
#include "RTDBDriverFrame/IOCfgUnitType.h"
//#include "Common/IOLog/IOLog.h"

#include "ace/OS.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "ace/Date_Time.h"

//#include "userdatabase.h"
#include "RTDBPlatformClass/FieldPointType.h"
#include "RTDBPlatformClass/IOUnit.h"

//! 周期定时器调度周期，毫秒。
const int PERIOD_SCAN_MS = 1000;	
const int SCANPERIOD_XRL855 = 60000/PERIOD_SCAN_MS; //855协议设备对时扫描周期，一分钟检查一次


//! 读定值组调度周期，毫秒。
const int PERIOD_RdSettingGp_MS = 1000;	
//! 串口传输最大数据包长度
const int MB_RTU_MAX_SEND_SIZE = 256;
//! 网络传输最大数据包长度
const int MB_TCP_MAX_SEND_SIZE = 65536;
//! 从机设备的最大超时次数（发送通道诊断帧且超时时需记录）
const int MACSDEVICE_SLAVE_MAX_TIMEOUT = 3;
const int MAXDEBUGARRAYLEN = 1124; //!调试信息最大长度
const int MAXSETTINGGROUP = 64; //!触发读取定值组遥控点最大个数
//! 功能码
enum FunCode
{
	READ_DIG_OUTPUT_STATUS = 1,
	READ_DIG_INPUT_STATUS,
	READ_ANA_OUTPUT_STATUS,
	READ_ANA_INPUT_STATUS,
	FORCE_DIG_SINGLE_OUTPUT_STATUS,
	FORCE_ANA_SINGLE_OUTPUT_STATUS,
	FORCE_DIG_MULTIPLE_OUTPUT_STATUS = 15,
	FORCE_ANA_MULTIPLE_OUTPUT_STATUS = 16,
	READ_GENERAL_REFERENCE = 20,
	WRITE_GENERAL_REFERENCE = 21,
};

//! 路径类型
enum
{
	DIR__TYPE = 0,		//!< 文件夹
	FILE_TYPE = 1,		//!< 文件
	NONE_TYPE = -1		//!< 不存在
};


//! 该值对应macsMB.dbf文件中的UnitType项的值
enum UnitType
{
	MACSMB_DIA = 1,			//!< 遥信 0X	        
	MACSMB_DIB,				//!< 遥信 1X	          
	MACSMB_DIC,				//!< 返信
	MACSMB_DID,				//!< 遥信 3X(LB FRST)	
	MACSMB_DIE,				//!< 遥信 4X(LB FRST)	
	MACSMB_AIAA,			//!< 遥测 3X WORD(LB FRST)
	MACSMB_AIAB,			//!< 遥测 3X WORD(HB FRST)
	MACSMB_AIBA,			//!< 遥测 4X WORD(LB FRST)
	MACSMB_AIBB,			//!< 遥测 4X WORD(HB FRST)
	MACSMB_AICA,			//!< 遥测 3X SHORT(LB FRST)	
	MACSMB_AICB,			//!< 遥测 3X SHORT(HB FRST)
	MACSMB_AIDA,			//!< 遥测 4X SHORT(LB FRST)
	MACSMB_AIDB,			//!< 遥测 4X SHORT(HB FRST)
	MACSMB_AIEA,			//!< 遥测 3X DWORD(LB LW FRST)
	MACSMB_AIEB,			//!< 遥测 3X DWORD(HB LW FRST)	
	MACSMB_AIEC,			//!< 遥测 3X DWORD(LB HW FRST)	
	MACSMB_AIED,			//!< 遥测 3X DWORD(HB HW FRST)	
	MACSMB_AIFA,			//!< 遥测 4X DWORD(LB LW FRST)	
	MACSMB_AIFB,			//!< 遥测 4X DWORD(HB LW FRST)
	MACSMB_AIFC,			//!< 遥测 4X DWORD(LB HW FRST)
	MACSMB_AIFD,			//!< 遥测 4X DWORD(HB HW FRST)	
	MACSMB_AIGA,			//!< 遥测 3X int(LB LW FRST)
	MACSMB_AIGB,			//!< 遥测 3X int(HB LW FRST)	
	MACSMB_AIGC,			//!< 遥测 3X int(LB HW FRST)
	MACSMB_AIGD,			//!< 遥测 3X int(HB HW FRST)
	MACSMB_AIHA,			//!< 遥测 4X int(LB LW FRST)	
	MACSMB_AIHB,			//!< 遥测 4X int(HB LW FRST)	
	MACSMB_AIHC,			//!< 遥测 4X int(LB HW FRST)	
	MACSMB_AIHD,			//!< 遥测 4X int(HB HW FRST)	
	MACSMB_AII,				//!< 遥测 3X FLOAT	
	MACSMB_AIJ,				//!< 遥测 4X FLOAT	
	MACSMB_AOA,				//!< 遥调 4X 功能码6	
	MACSMB_AOB,				//!< 遥调 4X 功能码16
	MACSMB_DOA,				//!< 遥控 0X 功能码5	
	MACSMB_DOB,				//!< 遥控 4X 功能码6	
	MACSMB_DOC,				//!< 遥控 0X 功能码15
	MACSMB_DOD,				//!< 遥控 4X 功能码16
	MACSMB_DOE,				//遥控 4X 功能码5,但不是0（对应0000）和1（对应FF00），而是0～65535，待完成！！！	
	MACSMB_AIK,				//遥测 3X FLOAT(*(OpcUa_Float*)buf)，待完成！！！	
	MACSMB_AIL,				//遥测 4X FLOAT(*(OpcUa_Float*)buf)，待完成！！！
	MACSMB_AIM,				//遥测 3X BCD_INT64, 0x0123456789012345 = 123456789012345(十进制)，待完成！！！	
	MACSMB_AIN,				//遥测 4X BCD_INT64，待完成！！！
	MACSMB_AIO,				//遥测 3X INT64	，待完成！！！
	MACSMB_AIP,				//遥测 4X INT64，待完成！！！
	MACSMB_AIQ,				//遥测 3X 16位定点数(HB FRST)，如00 01表示1，80 01表示-1。，待完成！！！
	MACSMB_AIR,				//遥测 4X 16位定点数(HB FRST)，待完成！！！	
	MACSMB_SEPCOSSOE, 		//SEPCOS SOE标签，int。值 = 类型号*1000+子类型号，待完成！！！ 
	MACSMB_SEPCOSEXT, 		//SEPCOS设备特殊控制点（故障录波、SOE），待完成！！！
	MACSMB_TEPIFSOE,		//!< 深圳2号线TINTANS直流屏的SOE标签。
							//!< 地址 = 告警（事件）类型*1000000 + 充电机组号*1000 + 分路号，值 = 告警值
	MACSMB_AOC = 51,		//!< 遥调 4X 功能码16 字符串类型
	MACSMB_AIMA,			//!< 遥测 3X 功能码4 String LB FRST
	MACSMB_AIMB,			//!< 遥测 3X 功能码4 String HB FRST
	MACSMB_AINA,			//!< 遥测 4X 功能码3 String LB FRST
	MACSMB_AINB,			//!< 遥测 4X 功能码3 String HB FRST	
	MACSMB_AOD,				//!< 遥调 4X 功能码6
	MACSMB_AISA,			//!< 遥测 3X 单个寄存器内，从某一位开始，指定长度所表示的数 WORD(LB FRST)
	MACSMB_AISB,			//!< 遥测 3X 单个寄存器内，从某一位开始，指定长度所表示的数 WORD(HB FRST)
	MACSMB_AITA,			//!< 遥测 4X 单个寄存器内，从某一位开始，指定长度所表示的数 WORD(LB FRST)
	MACSMB_AITB,			//!< 遥测 4X 单个寄存器内，从某一位开始，指定长度所表示的数 WORD(HB FRST)
	MACSMB_DIF,				//!< 遥信 3X(HB FRST)	
	MACSMB_DIG,				//!< 遥信 4X(HB FRST)
	MACSMB_AOE,				//!< 遥调 4X 功能码16 
	MACSMB_MOA,				//!< 遥调 4X 功能码16 BYTE[]类型
	MACSMB_AIFF,            //!< 遥测 4X 功能码3 HB_LW FRS   HW*10000+LW = 电度量数据
	MACSMB_PowFactor,       //!< 遥测 4X 功能码3 HB_LW FRS   NEXUS1272电度表功率因数
	MACSMB_READSETTINGGROUP,//!触发读取定值组虚拟遥控点
	MACSMB_DOM,				//遥控 0X 功能码15 控制多个线圈
	MACSMB_READSETTINGGROUPALL, //!触发读取所有定值组虚拟遥控点
	MACSMB_AOT,				//!遥调 4X 功能码16 一次下发OpcUa_Float类型的数据，占两个寄存器
	MACSMB_DWED,			//!< 遥测 3X DWORD(HB HW FRST)，标签值不转换成FLOAT
	MACSMB_DWFD,			//!< 遥测 4X DWORD(HB HW FRST)，标签值不转换成FLOAT
	MACSMB_AIU,				//!< 遥测 4X FLOAT
	MACSMB_AIV,				//!< 遥测 4X FLOAT(HB LW FRST)
	MACSMB_AOP,				//!< 遥调 4X 功能码16.写整个寄存器.DWORD(HB HW FRST)
	MACSMB_AOF,				//!< 遥调 4X 功能码6.单个寄存器中的多位，其余位自动为0	
	MACSMB_AOG,				//!< 遥调 4X 功能码16.单个寄存器中的多位，其余位自动为0
	MACSMB_DOF,				//!< 遥控 4X 功能码6.单个寄存器中的一位，其余位自动为0	
	MACSMB_DOG				//!< 遥控 4X 功能码16.单个寄存器中的一位，其余位自动为0
};


//! 设备类型，用于特殊选项(非标准Modbus功能, 如SOE, 对时, 故障录波, 写定值组)
enum DevType
{
	DEVTYPE_PMC = 1,		//ABB PMC 916，待完成！！！
	DEVTYPE_PMAC,			//白云 PMAC 720，待完成！！！
	DEVTYPE_DKZ,			//南瑞DKZ，待完成！！！
	DEVTYPE_AC31,			//ABB AC31 PLC，待完成！！！
	DEVTYPE_RSI,			//ABB RSI，待完成！！！
	DEVTYPE_NEXUS1272,		//NEXUS1272电度表，待完成！！！
	DEVTYPE_PSME20,			//PSM-E20直流屏，待完成！！！
	DEVTYPE_HLSPLG,			//和利时公司地铁专用排流柜及杂散电流监测系统，待完成！！！
	DEVTYPE_BJ10AZZSDL,		//北京奥运支线杂散电流监测装置，待完成！！！
	DEVTYPE_SEPCOS,			//北京10号线750vSEPCOS装置
	DEVTYPE_TEPIF,			//! 深圳2号线TINTANS直流屏
	DEVTYPE_TATSIA,			//! 深圳2号线TINTANS交流屏
	DEVTYPE_DBMLEsPL,       //!北京地铁8号线750V保护设备DB-MLEs-PL对时	
	DEVTYPE_F650,			//!北京地铁8号线F650保护设备对时	
	DEVTYPE_L30,			//!北京地铁8号线L30保护设备对时	
	DEVTYPE_855,			//港铁855协议设备对时	Add by: Wangyanchao

	DEVTYPE_DQPLC,			//!北京地铁14号线大全PLC（S7-200）对时	
	DEVTYPE_DLLSJZLP,		//!北京地铁14号线大连旅顺交直流屏对时	
	DEVTYPE_ZSZD,			//!北京地铁14号线再生制动对时	
	DEVTYPE_LDWKQ,			//!北京地铁14号线LD温控器对时	

	DEVTYPE_DL_SIEMENSS73,	//!西门子1500V开关柜S7-300设备校时(大连地铁1、2号线)	
	DEVTYPE_DL_UMLESSC,		//!上海成套1500V开关柜U-MLEs-SC设备校时(大连地铁1、2号线)	
	DEVTYPE_DL_SIEMENSS72,	//!上海成套1500V开关柜S7-200设备校时(大连地铁1、2号线)	

	DEVTYPE_SF_CSC268		//!北京地铁燕房线CSC268设备校时(Add by:Lifan)
};

//! 接收帧的状态，用于表示解析结果
enum RevState
{
	FLAG_NONE = 0,
	FLAG_UNCOMPLETE,
	FLAG_CORRECTFRAME,
	FLAG_ERROR,
};

//! CRC校验码顺序
const Byte CRC_LOB_FIRST = 0;		//! CRC低字节在前
const Byte CRC_HOB_FIRST = 1;		//! CRC高字节在前

const int MAX_CT_STRING_SIZE = 128;	//! 写String寄存器的最大字节长度

const int MAX_SERVERIP_COUNT = 4;	//! 最大服务器地址个数

//! 特殊设备相关
////////////////////////TINTANS/////////////////////////////////
//! 深圳2号线TINTANS直流屏的SOE帧的起始地址
const int TEPIF_SOE_START_ADDR1 = 130;
//! 深圳2号线TINTANS直流屏的SOE帧的起始地址
const int TEPIF_SOE_START_ADDR2	= 150;
//! 深圳2号线TINTANS直流屏的SOE帧读取的寄存器个数
const int TEPIF_SOE_POINTCOUNT	= 12;
////////////////////////TINTANS/////////////////////////////////

#pragma pack( push, 1 )

typedef struct {
	Byte	byPckNoHigh;	//! TCP顺序号高字节。
							//! 顺序号由主机给定，从机响应帧复制。0~65535循环。
	Byte	byPckNoLow;		//! TCP顺序号低字节
	Byte	byPrtclHigh;	//! 固定为0X00
	Byte	byPrtclLow;		//! 固定为0X00
	Byte	byDataLenHigh;	//! 数据段长度高字节
	Byte	byDataLenLow;	//! 数据段长度低字节
	Byte	bySlaveAddress;	//! 设备地址
}FRAME_HEAD;

typedef struct {
	Byte bySendFunc;		//! 最近发送帧功能码
	WORD wdSendStartAddr;	//! 最近发送帧起始地址
	WORD wdSendPoints;		//! 最近发送帧点数（对于AI，为寄存器数；对于DI，为bit数）
}SENDDATA;

//! 标准下发帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;
	Byte StartHi;
	Byte StartLo;
	Byte PointHi;
	Byte PointLo;
	Byte CRCFirst;
	Byte CRCSecond;
}REQUEST,COMMAND;

//! 遥控后立即发送返信点请求，从设备配置文件中配置。
typedef struct {
	int TagAddr;	//控制标签的绝对地址，范围1～65536，400001～465536，40000101～46553615。
	REQUEST Req;	//遥控点所对应返信点所在的请求
	int msDelay;	//发送控制帧后间隔msDelay毫秒后再发返信请求，以确保返信请求有效。
}struCtrlFollowReq;

//! 文件记录属性
typedef struct {
	Byte byRefType;		//! 参考类型
	WORD wdFileNo;		//! 文件号
	WORD wdRegAddr;		//! 记录号
	WORD wdRegCount;	//! 记录个数
}FILEREQ;

//! 读文件记录请求帧
typedef struct {
	Byte SlaveAddress;
	Byte Function; 
	Byte DataCount;		//! 数据长度
	FILEREQ FileReq;	//! 要读文件记录的属性
	Byte CRCFirst;
	Byte CRCSecond;
}READ_REF_REQ;

//! TITANS交流屏或直流屏的设备校时帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 78H
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 07H
	Byte ByteCount;		//! 0EH
	Byte YearHiHi;		//! 年高字高位
	Byte YearHiLo;		//! 年高字低位
	Byte YearLoHi;		//! 年低字高位
	Byte YearLoLo;		//! 年低字低位
	Byte MonthHi;		//! 月
	Byte MonthLo;		//! 月
	Byte DayHi;			//! 日
	Byte DayLo;			//! 日
	Byte HourHi;		//! 时
	Byte HourLo;		//! 时
	Byte MintueHi;		//! 分
	Byte MintueLo;		//! 分
	Byte SecondHi;		//! 秒
	Byte SecondLo;		//! 秒
	Byte CRCFirst;
	Byte CRCSecond;
}TITANSCHECKTIMEREQUEST;

//! 855协议的设备校时帧 Add by: Wangyanchao
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 起始地址高位
	Byte StartLo;		//! 起始地址低位
	Byte PointHi;		//! 点数高位
	Byte PointLo;		//! 点数低位
	Byte ByteCount;		//! 字节个数
	Byte YearHi;		//! 年
	Byte YearLo;		//! 年
	Byte MonthHi;		//! 月
	Byte MonthLo;		//! 月
	Byte DayHi;			//! 日
	Byte DayLo;			//! 日
	Byte HourHi;		//! 时
	Byte HourLo;		//! 时
	Byte MintueHi;		//! 分
	Byte MintueLo;		//! 分
	Byte SecondHi;		//! 秒
	Byte SecondLo;		//! 秒
	Byte CRCFirst;
	Byte CRCSecond;
}C855CHECKTIMEREQUEST;

//! TITANS直流屏的告警帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 03H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 82H 或 96H
	Byte CountHi;		//! 00H
	Byte CountLo;		//! 0CH
	Byte CRCFirst;
	Byte CRCSecond;
}TEPIFREADWARN;

//!NEXUS1272电度表设备校时帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;
	Byte StartHi;
	Byte StartLo;
	Byte PointHi;
	Byte PointLo;
	Byte ByteCount;
	Byte Century;
	Byte Year;
	Byte Month;
	Byte Day;
	Byte Hour;
	Byte Mintue;
	Byte Second;
	Byte TenMillisecond;
	Byte WeekHi;
	Byte WeekLo;
	Byte CRCFirst;
	Byte CRCSecond;
}NEXUSCHECKTIMEREQUEST;

//! 北京地铁8号线750V保护设备DB-MLEs-PL校时帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 03H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 82H 或 96H
	Byte CountHi;		//! 00H
	Byte CountLo;		//! 0CH
	Byte ByteCount;

	Byte byRes;
	Byte Year;
	Byte Month;
	Byte Day;
	Byte Hour;
	Byte Mintue;
	Byte Second;
	Byte TenMicrosecond;

	Byte CRCFirst;
	Byte CRCSecond;
}DBMLEsPLCHECKTIMEREQUEST;

//! 北京地铁8号线F650保护设备校时帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;		
	Byte StartHi;		
	Byte StartLo;		
	Byte CountHi;		
	Byte CountLo;		
	Byte ByteCount;
	//!发送时间
	Byte bySendTimeStap[8];
	Byte CRCFirst;
	Byte CRCSecond;
}F650CHECKTIMEREQUEST;

//! 北京地铁8号线L30保护设备校时帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;		
	Byte StartHi;		
	Byte StartLo;		
	Byte CountHi;		
	Byte CountLo;		
	Byte ByteCount;
	//!发送时间
	Byte bySendTimeStap[4];
	Byte CRCFirst;
	Byte CRCSecond;
}L30CHECKTIMEREQUEST;

typedef struct DAILYTIME {
	int lYear;					//! 年
	short sMonth;				//! 月
	short sDay;					//! 日
    short sHour;				//! 时
	short sMinute;				//! 分
	short sSecond;				//! 秒
	unsigned short sMSecond;	//! 毫秒
	
	//!	时区（为0则是格林威治时间，东时区为负，西时区为正，例如北京时间为-8）
	short sTimeZone;

	//! 构造时赋缺省值
	DAILYTIME()
	{
		lYear = 1970;
		sMonth = 1;
		sDay = 1;
		sHour = 0;
		sMinute = 0;
		sSecond = 0;
		sMSecond = 0;
		sTimeZone = -8;
	};
}DAILYTIME;

//! LD设备校时帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 00H
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 04H
	Byte ByteCount;		//! 08H
	Byte Year;			//! 年
	Byte Month;			//! 月
	Byte Day;			//! 日
	Byte Hour;			//! 时
	Byte Minute;		//! 分
	Byte Second;		//! 秒
	Byte MsecHi;		//! 毫秒高位
	Byte MsecLo;		//! 毫秒低位
	Byte CRCFirst;
	Byte CRCSecond;
}LDCHECKTIMEREQUEST;

typedef struct  
{
	Byte	m_bySlaveAddr;		/*!< 发送地址。*/
	Byte	m_byFunction;		/*!< 功能码。*/
	Byte	m_byStartAddrHi;	/*!< 起始寄存器高位。*/
	Byte	m_byStartAddrLo;	/*!< 起始寄存器低位。*/
	Byte	m_byRegNoHi;		/*!< 寄存器个数高位。*/
	Byte	m_byRegNoLo;		/*!< 寄存器个数低位。*/
	Byte	m_byByteCount;		/*!< 字节个数。*/
	Byte	m_byYearHi;			/*!< 年，高位。*/
	Byte	m_byYearLo;			/*!< 年，低位。*/
	Byte	m_byMonthHi;		/*!< 月，高位。*/
	Byte	m_byMonthLo;		/*!< 月，低位。*/
	Byte	m_byDayHi;			/*!< 天，高位。*/
	Byte	m_byDayLo;			/*!< 天，低位。*/
	Byte	m_byWeekDayHi;		/*!< 天（周），高位。*/
	Byte	m_byWeekDayLo;		/*!< 天（周），低位。*/
	Byte	m_byHourHi;			/*!< 小时，高位。*/
	Byte	m_byHourLo;			/*!< 小时，低位。*/
	Byte	m_byMinuteHi;		/*!< 分钟，高位。*/
	Byte	m_byMinuteLo;		/*!< 分钟，低位。*/
	Byte	m_bySecondHi;		/*!< 秒，高位。*/
	Byte	m_bySecondLo;		/*!< 秒，低位。*/
	Byte	m_byMillisecondsHi;	/*!< 毫秒，高位。*/
	Byte	m_byMillisecondsLo;	/*!< 毫秒，低位。*/
	Byte	m_byValidate1;		/*!< 。*/
	Byte	m_byValidate2;		/*!< 。*/
	Byte	m_byCRCFirst;		/*!< CRC校验第一位。*/
	Byte	m_byCRCSecond;		/*!< CRC校验第二位。*/
}SEPCOSTIMEFRAME;

//! 大全PLC（S7-200）设备校时帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 00H
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 05H
	Byte ByteCount;		//! 0AH
	Byte Year;			//! 年
	Byte Month;			//! 月
	Byte Day;			//! 日
	Byte Hour;			//! 时
	Byte Minute;		//! 分
	Byte Second;		//! 秒
	Byte MsecHi;		//! 毫秒高位（永远置0）
	Byte MsecLo;		//! 毫秒低位（永远置0）
	Byte m_byValidate1;		/*!< 。*/
	Byte m_byValidate2;		/*!< 。*/
	Byte CRCFirst;
	Byte CRCSecond;
}DQPLCCHECKTIMEREQUEST;

//! 大连旅顺交直流屏设备校时帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 00H
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 04H
	//Byte ByteCount;		//! 08H
	Byte YearHi;		/*!< 年，高位。*/
	Byte YearLo;		/*!< 年，低位。*/
	Byte Month;			//! 月
	Byte Day;			//! 日
	Byte Hour;			//! 时
	Byte Minute;		//! 分
	Byte MsecHi;		//! 毫秒高位：0~59999
	Byte MsecLo;		//! 毫秒低位
	Byte CRCFirst;
	Byte CRCSecond;
}DLLSJZLPCHECKTIMEREQUEST;

//! 再生制动设备校时帧
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 00H
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 03H
	Byte ByteCount;		//! 06H
	Byte Year;		    //! 年: 实际年减去2000
	Byte Month;			//! 月
	Byte Day;			//! 日
	Byte Hour;			//! 时
	Byte Minute;		//! 分
	Byte Second;		//! 秒
	Byte CRCFirst;
	Byte CRCSecond;
}ZSZDCHECKTIMEREQUEST;

//! 西门子1500V开关柜S7-300设备校时帧(大连地铁1、2号线)
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 
	Byte StartLo;		//! 
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 05H
	Byte ByteCount;		//! 0AH
	Byte YearHi;		//! 年 20H(BCD, 下同)
	Byte YearLo;		//! 年 13H
	Byte Month;			//! 月 
	Byte Day;			//! 日
	Byte Hour;			//! 时
	Byte Minute;		//! 分
	Byte Second;		//! 秒
	Byte MsecHi;		//! 百分秒 0~99H
	Byte MsecLo;		//! 毫秒   0~9H
	Byte Rev;			//! 0
	Byte CRCFirst;
	Byte CRCSecond;
}DL_SIEMENSS73_CHECKTIMEREQUEST;

//! 上海成套1500V开关柜U-MLEs-SC设备校时帧(大连地铁1、2号线)
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 
	Byte StartLo;		//! 
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 04H
	Byte ByteCount;		//! 08H
	Byte YearHi;		//! 年 20H（下位机不理会该字节）
	Byte YearLo;		//! 年 13H(BCD, 下同)
	Byte Month;			//! 月 
	Byte Day;			//! 日
	Byte Hour;			//! 时
	Byte Minute;		//! 分
	Byte Second;		//! 秒
	Byte CendSecond;	//! 百分秒 0~99H
	Byte CRCFirst;
	Byte CRCSecond;
}DL_UMLESSC_CHECKTIMEREQUEST;

//!北京燕房线CSC268设备校时帧
typedef struct  
{
	Byte bySlaveAddr;		//!设备地址
	Byte byFunc;			//!功能码
	Byte byStartHi;		//!起始地址高位
	Byte byStartLo;		//!起始地址低位
	Byte byRegCountHi;	//!寄存器数量高位
	Byte byRegCountLo;	//!寄存器数量低位
	Byte byCount;			//!字节数
	Byte byYearHi;		//!年-高字节
	Byte byYearLo;		//!年-低字节
	Byte byMonth;			//!月
	Byte byDay;			//!日
	Byte byHour;			//!时
	Byte byMinute;		//!分
	Byte byMillSecHi;		//!毫秒-高字节
	Byte byMillSecLo;		//!毫秒-低字节
	Byte byCRCFirst;		//!CRC校验
	Byte byCRCSecond;		
}SF_CSC268_CHECKTIME;

#ifndef MAKEDWORD
#define MAKEDWORD(l, h) ((DWORD)(((WORD)(l)) | ((DWORD)((WORD)(h))) << 16))
#endif

//! 上海成套1500V开关柜S7-200设备校时帧(大连地铁1、2号线)：需要澄清结构！！！
typedef struct {
	Byte	m_bySlaveAddr;		/*!< 发送地址。*/
	Byte	m_byFunction;		/*!< 功能码。*/
	Byte	m_byStartAddrHi;	/*!< 起始寄存器高位。*/
	Byte	m_byStartAddrLo;	/*!< 起始寄存器低位。*/
	Byte	m_byRegNoHi;		/*!< 寄存器个数高位。*/
	Byte	m_byRegNoLo;		/*!< 寄存器个数低位。*/
	Byte	m_byByteCount;		/*!< 字节个数。*/
	Byte	m_byYearHi;			/*!< 年，高位。*/
	Byte	m_byYearLo;			/*!< 年，低位。*/
	Byte	m_byMonthHi;		/*!< 月，高位。*/
	Byte	m_byMonthLo;		/*!< 月，低位。*/
	Byte	m_byDayHi;			/*!< 天，高位。*/
	Byte	m_byDayLo;			/*!< 天，低位。*/
	Byte	m_byHourHi;			/*!< 小时，高位。*/
	Byte	m_byHourLo;			/*!< 小时，低位。*/
	Byte	m_byMinuteHi;		/*!< 分钟，高位。*/
	Byte	m_byMinuteLo;		/*!< 分钟，低位。*/
	Byte	m_bySecondHi;		/*!< 秒，高位。*/
	Byte	m_bySecondLo;		/*!< 秒，低位。*/
	Byte	m_byCRCFirst;		/*!< CRC校验第一位。*/
	Byte	m_byCRCSecond;		/*!< CRC校验第二位。*/
}DL_SIEMENSS72_CHECKTIMEREQUEST;



#pragma pack( pop )

namespace MACS_SCADA_SUD
{
/*!
*	\brief	获取ini配置文件中某项配置信息
*
*	\param	pchAppName	节点名，即"[]"中内容
*	\param	pchKeyName	关键字名
*	\param	nDefault	默认值，读取失败则返回该值
*	\param	pchFileName	文件名
*
*	\retval	int	ini配置文件中对应项的值，读取失败则返回默认值
*/
extern FEPMODBUSDRV_API int GetPrivateInifileInt(const char* pchAppName, 
				const char* pchKeyName, int nDefault, const char* pchFileName);

FEPMODBUSDRV_API std::string GetPrivateInifileString(const char* pchAppName,
				const char* pchKeyName, std::string  strDefault, const char* pchFileName);
}

#endif
