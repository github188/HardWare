/*!
 *	\file	FepModbusDrvH.h
 *
 *	\brief	ͨ��Modbus�����й��õĺ�ͽṹ�����ļ�
 *
 *	$Header: ?/MACS-SCADA 3.2 �������/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusDrvH.h, 470+1 2011/04/22 01:56:47 xuhongtao $
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

//! ���ڶ�ʱ���������ڣ����롣
const int PERIOD_SCAN_MS = 1000;	
const int SCANPERIOD_XRL855 = 60000/PERIOD_SCAN_MS; //855Э���豸��ʱɨ�����ڣ�һ���Ӽ��һ��


//! ����ֵ��������ڣ����롣
const int PERIOD_RdSettingGp_MS = 1000;	
//! ���ڴ���������ݰ�����
const int MB_RTU_MAX_SEND_SIZE = 256;
//! ���紫��������ݰ�����
const int MB_TCP_MAX_SEND_SIZE = 65536;
//! �ӻ��豸�����ʱ����������ͨ�����֡�ҳ�ʱʱ���¼��
const int MACSDEVICE_SLAVE_MAX_TIMEOUT = 3;
const int MAXDEBUGARRAYLEN = 1124; //!������Ϣ��󳤶�
const int MAXSETTINGGROUP = 64; //!������ȡ��ֵ��ң�ص�������
//! ������
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

//! ·������
enum
{
	DIR__TYPE = 0,		//!< �ļ���
	FILE_TYPE = 1,		//!< �ļ�
	NONE_TYPE = -1		//!< ������
};


//! ��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ
enum UnitType
{
	MACSMB_DIA = 1,			//!< ң�� 0X	        
	MACSMB_DIB,				//!< ң�� 1X	          
	MACSMB_DIC,				//!< ����
	MACSMB_DID,				//!< ң�� 3X(LB FRST)	
	MACSMB_DIE,				//!< ң�� 4X(LB FRST)	
	MACSMB_AIAA,			//!< ң�� 3X WORD(LB FRST)
	MACSMB_AIAB,			//!< ң�� 3X WORD(HB FRST)
	MACSMB_AIBA,			//!< ң�� 4X WORD(LB FRST)
	MACSMB_AIBB,			//!< ң�� 4X WORD(HB FRST)
	MACSMB_AICA,			//!< ң�� 3X SHORT(LB FRST)	
	MACSMB_AICB,			//!< ң�� 3X SHORT(HB FRST)
	MACSMB_AIDA,			//!< ң�� 4X SHORT(LB FRST)
	MACSMB_AIDB,			//!< ң�� 4X SHORT(HB FRST)
	MACSMB_AIEA,			//!< ң�� 3X DWORD(LB LW FRST)
	MACSMB_AIEB,			//!< ң�� 3X DWORD(HB LW FRST)	
	MACSMB_AIEC,			//!< ң�� 3X DWORD(LB HW FRST)	
	MACSMB_AIED,			//!< ң�� 3X DWORD(HB HW FRST)	
	MACSMB_AIFA,			//!< ң�� 4X DWORD(LB LW FRST)	
	MACSMB_AIFB,			//!< ң�� 4X DWORD(HB LW FRST)
	MACSMB_AIFC,			//!< ң�� 4X DWORD(LB HW FRST)
	MACSMB_AIFD,			//!< ң�� 4X DWORD(HB HW FRST)	
	MACSMB_AIGA,			//!< ң�� 3X int(LB LW FRST)
	MACSMB_AIGB,			//!< ң�� 3X int(HB LW FRST)	
	MACSMB_AIGC,			//!< ң�� 3X int(LB HW FRST)
	MACSMB_AIGD,			//!< ң�� 3X int(HB HW FRST)
	MACSMB_AIHA,			//!< ң�� 4X int(LB LW FRST)	
	MACSMB_AIHB,			//!< ң�� 4X int(HB LW FRST)	
	MACSMB_AIHC,			//!< ң�� 4X int(LB HW FRST)	
	MACSMB_AIHD,			//!< ң�� 4X int(HB HW FRST)	
	MACSMB_AII,				//!< ң�� 3X FLOAT	
	MACSMB_AIJ,				//!< ң�� 4X FLOAT	
	MACSMB_AOA,				//!< ң�� 4X ������6	
	MACSMB_AOB,				//!< ң�� 4X ������16
	MACSMB_DOA,				//!< ң�� 0X ������5	
	MACSMB_DOB,				//!< ң�� 4X ������6	
	MACSMB_DOC,				//!< ң�� 0X ������15
	MACSMB_DOD,				//!< ң�� 4X ������16
	MACSMB_DOE,				//ң�� 4X ������5,������0����Ӧ0000����1����ӦFF00��������0��65535������ɣ�����	
	MACSMB_AIK,				//ң�� 3X FLOAT(*(OpcUa_Float*)buf)������ɣ�����	
	MACSMB_AIL,				//ң�� 4X FLOAT(*(OpcUa_Float*)buf)������ɣ�����
	MACSMB_AIM,				//ң�� 3X BCD_INT64, 0x0123456789012345 = 123456789012345(ʮ����)������ɣ�����	
	MACSMB_AIN,				//ң�� 4X BCD_INT64������ɣ�����
	MACSMB_AIO,				//ң�� 3X INT64	������ɣ�����
	MACSMB_AIP,				//ң�� 4X INT64������ɣ�����
	MACSMB_AIQ,				//ң�� 3X 16λ������(HB FRST)����00 01��ʾ1��80 01��ʾ-1��������ɣ�����
	MACSMB_AIR,				//ң�� 4X 16λ������(HB FRST)������ɣ�����	
	MACSMB_SEPCOSSOE, 		//SEPCOS SOE��ǩ��int��ֵ = ���ͺ�*1000+�����ͺţ�����ɣ����� 
	MACSMB_SEPCOSEXT, 		//SEPCOS�豸������Ƶ㣨����¼����SOE��������ɣ�����
	MACSMB_TEPIFSOE,		//!< ����2����TINTANSֱ������SOE��ǩ��
							//!< ��ַ = �澯���¼�������*1000000 + �������*1000 + ��·�ţ�ֵ = �澯ֵ
	MACSMB_AOC = 51,		//!< ң�� 4X ������16 �ַ�������
	MACSMB_AIMA,			//!< ң�� 3X ������4 String LB FRST
	MACSMB_AIMB,			//!< ң�� 3X ������4 String HB FRST
	MACSMB_AINA,			//!< ң�� 4X ������3 String LB FRST
	MACSMB_AINB,			//!< ң�� 4X ������3 String HB FRST	
	MACSMB_AOD,				//!< ң�� 4X ������6
	MACSMB_AISA,			//!< ң�� 3X �����Ĵ����ڣ���ĳһλ��ʼ��ָ����������ʾ���� WORD(LB FRST)
	MACSMB_AISB,			//!< ң�� 3X �����Ĵ����ڣ���ĳһλ��ʼ��ָ����������ʾ���� WORD(HB FRST)
	MACSMB_AITA,			//!< ң�� 4X �����Ĵ����ڣ���ĳһλ��ʼ��ָ����������ʾ���� WORD(LB FRST)
	MACSMB_AITB,			//!< ң�� 4X �����Ĵ����ڣ���ĳһλ��ʼ��ָ����������ʾ���� WORD(HB FRST)
	MACSMB_DIF,				//!< ң�� 3X(HB FRST)	
	MACSMB_DIG,				//!< ң�� 4X(HB FRST)
	MACSMB_AOE,				//!< ң�� 4X ������16 
	MACSMB_MOA,				//!< ң�� 4X ������16 BYTE[]����
	MACSMB_AIFF,            //!< ң�� 4X ������3 HB_LW FRS   HW*10000+LW = ���������
	MACSMB_PowFactor,       //!< ң�� 4X ������3 HB_LW FRS   NEXUS1272��ȱ�������
	MACSMB_READSETTINGGROUP,//!������ȡ��ֵ������ң�ص�
	MACSMB_DOM,				//ң�� 0X ������15 ���ƶ����Ȧ
	MACSMB_READSETTINGGROUPALL, //!������ȡ���ж�ֵ������ң�ص�
	MACSMB_AOT,				//!ң�� 4X ������16 һ���·�OpcUa_Float���͵����ݣ�ռ�����Ĵ���
	MACSMB_DWED,			//!< ң�� 3X DWORD(HB HW FRST)����ǩֵ��ת����FLOAT
	MACSMB_DWFD,			//!< ң�� 4X DWORD(HB HW FRST)����ǩֵ��ת����FLOAT
	MACSMB_AIU,				//!< ң�� 4X FLOAT
	MACSMB_AIV,				//!< ң�� 4X FLOAT(HB LW FRST)
	MACSMB_AOP,				//!< ң�� 4X ������16.д�����Ĵ���.DWORD(HB HW FRST)
	MACSMB_AOF,				//!< ң�� 4X ������6.�����Ĵ����еĶ�λ������λ�Զ�Ϊ0	
	MACSMB_AOG,				//!< ң�� 4X ������16.�����Ĵ����еĶ�λ������λ�Զ�Ϊ0
	MACSMB_DOF,				//!< ң�� 4X ������6.�����Ĵ����е�һλ������λ�Զ�Ϊ0	
	MACSMB_DOG				//!< ң�� 4X ������16.�����Ĵ����е�һλ������λ�Զ�Ϊ0
};


//! �豸���ͣ���������ѡ��(�Ǳ�׼Modbus����, ��SOE, ��ʱ, ����¼��, д��ֵ��)
enum DevType
{
	DEVTYPE_PMC = 1,		//ABB PMC 916������ɣ�����
	DEVTYPE_PMAC,			//���� PMAC 720������ɣ�����
	DEVTYPE_DKZ,			//����DKZ������ɣ�����
	DEVTYPE_AC31,			//ABB AC31 PLC������ɣ�����
	DEVTYPE_RSI,			//ABB RSI������ɣ�����
	DEVTYPE_NEXUS1272,		//NEXUS1272��ȱ�����ɣ�����
	DEVTYPE_PSME20,			//PSM-E20ֱ����������ɣ�����
	DEVTYPE_HLSPLG,			//����ʱ��˾����ר����������ɢ�������ϵͳ������ɣ�����
	DEVTYPE_BJ10AZZSDL,		//��������֧����ɢ�������װ�ã�����ɣ�����
	DEVTYPE_SEPCOS,			//����10����750vSEPCOSװ��
	DEVTYPE_TEPIF,			//! ����2����TINTANSֱ����
	DEVTYPE_TATSIA,			//! ����2����TINTANS������
	DEVTYPE_DBMLEsPL,       //!��������8����750V�����豸DB-MLEs-PL��ʱ	
	DEVTYPE_F650,			//!��������8����F650�����豸��ʱ	
	DEVTYPE_L30,			//!��������8����L30�����豸��ʱ	
	DEVTYPE_855,			//����855Э���豸��ʱ	Add by: Wangyanchao

	DEVTYPE_DQPLC,			//!��������14���ߴ�ȫPLC��S7-200����ʱ	
	DEVTYPE_DLLSJZLP,		//!��������14���ߴ�����˳��ֱ������ʱ	
	DEVTYPE_ZSZD,			//!��������14���������ƶ���ʱ	
	DEVTYPE_LDWKQ,			//!��������14����LD�¿�����ʱ	

	DEVTYPE_DL_SIEMENSS73,	//!������1500V���ع�S7-300�豸Уʱ(��������1��2����)	
	DEVTYPE_DL_UMLESSC,		//!�Ϻ�����1500V���ع�U-MLEs-SC�豸Уʱ(��������1��2����)	
	DEVTYPE_DL_SIEMENSS72,	//!�Ϻ�����1500V���ع�S7-200�豸Уʱ(��������1��2����)	

	DEVTYPE_SF_CSC268		//!���������෿��CSC268�豸Уʱ(Add by:Lifan)
};

//! ����֡��״̬�����ڱ�ʾ�������
enum RevState
{
	FLAG_NONE = 0,
	FLAG_UNCOMPLETE,
	FLAG_CORRECTFRAME,
	FLAG_ERROR,
};

//! CRCУ����˳��
const Byte CRC_LOB_FIRST = 0;		//! CRC���ֽ���ǰ
const Byte CRC_HOB_FIRST = 1;		//! CRC���ֽ���ǰ

const int MAX_CT_STRING_SIZE = 128;	//! дString�Ĵ���������ֽڳ���

const int MAX_SERVERIP_COUNT = 4;	//! ����������ַ����

//! �����豸���
////////////////////////TINTANS/////////////////////////////////
//! ����2����TINTANSֱ������SOE֡����ʼ��ַ
const int TEPIF_SOE_START_ADDR1 = 130;
//! ����2����TINTANSֱ������SOE֡����ʼ��ַ
const int TEPIF_SOE_START_ADDR2	= 150;
//! ����2����TINTANSֱ������SOE֡��ȡ�ļĴ�������
const int TEPIF_SOE_POINTCOUNT	= 12;
////////////////////////TINTANS/////////////////////////////////

#pragma pack( push, 1 )

typedef struct {
	Byte	byPckNoHigh;	//! TCP˳��Ÿ��ֽڡ�
							//! ˳����������������ӻ���Ӧ֡���ơ�0~65535ѭ����
	Byte	byPckNoLow;		//! TCP˳��ŵ��ֽ�
	Byte	byPrtclHigh;	//! �̶�Ϊ0X00
	Byte	byPrtclLow;		//! �̶�Ϊ0X00
	Byte	byDataLenHigh;	//! ���ݶγ��ȸ��ֽ�
	Byte	byDataLenLow;	//! ���ݶγ��ȵ��ֽ�
	Byte	bySlaveAddress;	//! �豸��ַ
}FRAME_HEAD;

typedef struct {
	Byte bySendFunc;		//! �������֡������
	WORD wdSendStartAddr;	//! �������֡��ʼ��ַ
	WORD wdSendPoints;		//! �������֡����������AI��Ϊ�Ĵ�����������DI��Ϊbit����
}SENDDATA;

//! ��׼�·�֡
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

//! ң�غ��������ͷ��ŵ����󣬴��豸�����ļ������á�
typedef struct {
	int TagAddr;	//���Ʊ�ǩ�ľ��Ե�ַ����Χ1��65536��400001��465536��40000101��46553615��
	REQUEST Req;	//ң�ص�����Ӧ���ŵ����ڵ�����
	int msDelay;	//���Ϳ���֡����msDelay������ٷ�����������ȷ������������Ч��
}struCtrlFollowReq;

//! �ļ���¼����
typedef struct {
	Byte byRefType;		//! �ο�����
	WORD wdFileNo;		//! �ļ���
	WORD wdRegAddr;		//! ��¼��
	WORD wdRegCount;	//! ��¼����
}FILEREQ;

//! ���ļ���¼����֡
typedef struct {
	Byte SlaveAddress;
	Byte Function; 
	Byte DataCount;		//! ���ݳ���
	FILEREQ FileReq;	//! Ҫ���ļ���¼������
	Byte CRCFirst;
	Byte CRCSecond;
}READ_REF_REQ;

//! TITANS��������ֱ�������豸Уʱ֡
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 78H
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 07H
	Byte ByteCount;		//! 0EH
	Byte YearHiHi;		//! ����ָ�λ
	Byte YearHiLo;		//! ����ֵ�λ
	Byte YearLoHi;		//! ����ָ�λ
	Byte YearLoLo;		//! ����ֵ�λ
	Byte MonthHi;		//! ��
	Byte MonthLo;		//! ��
	Byte DayHi;			//! ��
	Byte DayLo;			//! ��
	Byte HourHi;		//! ʱ
	Byte HourLo;		//! ʱ
	Byte MintueHi;		//! ��
	Byte MintueLo;		//! ��
	Byte SecondHi;		//! ��
	Byte SecondLo;		//! ��
	Byte CRCFirst;
	Byte CRCSecond;
}TITANSCHECKTIMEREQUEST;

//! 855Э����豸Уʱ֡ Add by: Wangyanchao
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! ��ʼ��ַ��λ
	Byte StartLo;		//! ��ʼ��ַ��λ
	Byte PointHi;		//! ������λ
	Byte PointLo;		//! ������λ
	Byte ByteCount;		//! �ֽڸ���
	Byte YearHi;		//! ��
	Byte YearLo;		//! ��
	Byte MonthHi;		//! ��
	Byte MonthLo;		//! ��
	Byte DayHi;			//! ��
	Byte DayLo;			//! ��
	Byte HourHi;		//! ʱ
	Byte HourLo;		//! ʱ
	Byte MintueHi;		//! ��
	Byte MintueLo;		//! ��
	Byte SecondHi;		//! ��
	Byte SecondLo;		//! ��
	Byte CRCFirst;
	Byte CRCSecond;
}C855CHECKTIMEREQUEST;

//! TITANSֱ�����ĸ澯֡
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 03H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 82H �� 96H
	Byte CountHi;		//! 00H
	Byte CountLo;		//! 0CH
	Byte CRCFirst;
	Byte CRCSecond;
}TEPIFREADWARN;

//!NEXUS1272��ȱ��豸Уʱ֡
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

//! ��������8����750V�����豸DB-MLEs-PLУʱ֡
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 03H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 82H �� 96H
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

//! ��������8����F650�����豸Уʱ֡
typedef struct {
	Byte SlaveAddress;
	Byte Function;		
	Byte StartHi;		
	Byte StartLo;		
	Byte CountHi;		
	Byte CountLo;		
	Byte ByteCount;
	//!����ʱ��
	Byte bySendTimeStap[8];
	Byte CRCFirst;
	Byte CRCSecond;
}F650CHECKTIMEREQUEST;

//! ��������8����L30�����豸Уʱ֡
typedef struct {
	Byte SlaveAddress;
	Byte Function;		
	Byte StartHi;		
	Byte StartLo;		
	Byte CountHi;		
	Byte CountLo;		
	Byte ByteCount;
	//!����ʱ��
	Byte bySendTimeStap[4];
	Byte CRCFirst;
	Byte CRCSecond;
}L30CHECKTIMEREQUEST;

typedef struct DAILYTIME {
	int lYear;					//! ��
	short sMonth;				//! ��
	short sDay;					//! ��
    short sHour;				//! ʱ
	short sMinute;				//! ��
	short sSecond;				//! ��
	unsigned short sMSecond;	//! ����
	
	//!	ʱ����Ϊ0���Ǹ�������ʱ�䣬��ʱ��Ϊ������ʱ��Ϊ�������籱��ʱ��Ϊ-8��
	short sTimeZone;

	//! ����ʱ��ȱʡֵ
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

//! LD�豸Уʱ֡
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 00H
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 04H
	Byte ByteCount;		//! 08H
	Byte Year;			//! ��
	Byte Month;			//! ��
	Byte Day;			//! ��
	Byte Hour;			//! ʱ
	Byte Minute;		//! ��
	Byte Second;		//! ��
	Byte MsecHi;		//! �����λ
	Byte MsecLo;		//! �����λ
	Byte CRCFirst;
	Byte CRCSecond;
}LDCHECKTIMEREQUEST;

typedef struct  
{
	Byte	m_bySlaveAddr;		/*!< ���͵�ַ��*/
	Byte	m_byFunction;		/*!< �����롣*/
	Byte	m_byStartAddrHi;	/*!< ��ʼ�Ĵ�����λ��*/
	Byte	m_byStartAddrLo;	/*!< ��ʼ�Ĵ�����λ��*/
	Byte	m_byRegNoHi;		/*!< �Ĵ���������λ��*/
	Byte	m_byRegNoLo;		/*!< �Ĵ���������λ��*/
	Byte	m_byByteCount;		/*!< �ֽڸ�����*/
	Byte	m_byYearHi;			/*!< �꣬��λ��*/
	Byte	m_byYearLo;			/*!< �꣬��λ��*/
	Byte	m_byMonthHi;		/*!< �£���λ��*/
	Byte	m_byMonthLo;		/*!< �£���λ��*/
	Byte	m_byDayHi;			/*!< �죬��λ��*/
	Byte	m_byDayLo;			/*!< �죬��λ��*/
	Byte	m_byWeekDayHi;		/*!< �죨�ܣ�����λ��*/
	Byte	m_byWeekDayLo;		/*!< �죨�ܣ�����λ��*/
	Byte	m_byHourHi;			/*!< Сʱ����λ��*/
	Byte	m_byHourLo;			/*!< Сʱ����λ��*/
	Byte	m_byMinuteHi;		/*!< ���ӣ���λ��*/
	Byte	m_byMinuteLo;		/*!< ���ӣ���λ��*/
	Byte	m_bySecondHi;		/*!< �룬��λ��*/
	Byte	m_bySecondLo;		/*!< �룬��λ��*/
	Byte	m_byMillisecondsHi;	/*!< ���룬��λ��*/
	Byte	m_byMillisecondsLo;	/*!< ���룬��λ��*/
	Byte	m_byValidate1;		/*!< ��*/
	Byte	m_byValidate2;		/*!< ��*/
	Byte	m_byCRCFirst;		/*!< CRCУ���һλ��*/
	Byte	m_byCRCSecond;		/*!< CRCУ��ڶ�λ��*/
}SEPCOSTIMEFRAME;

//! ��ȫPLC��S7-200���豸Уʱ֡
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 00H
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 05H
	Byte ByteCount;		//! 0AH
	Byte Year;			//! ��
	Byte Month;			//! ��
	Byte Day;			//! ��
	Byte Hour;			//! ʱ
	Byte Minute;		//! ��
	Byte Second;		//! ��
	Byte MsecHi;		//! �����λ����Զ��0��
	Byte MsecLo;		//! �����λ����Զ��0��
	Byte m_byValidate1;		/*!< ��*/
	Byte m_byValidate2;		/*!< ��*/
	Byte CRCFirst;
	Byte CRCSecond;
}DQPLCCHECKTIMEREQUEST;

//! ������˳��ֱ�����豸Уʱ֡
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 00H
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 04H
	//Byte ByteCount;		//! 08H
	Byte YearHi;		/*!< �꣬��λ��*/
	Byte YearLo;		/*!< �꣬��λ��*/
	Byte Month;			//! ��
	Byte Day;			//! ��
	Byte Hour;			//! ʱ
	Byte Minute;		//! ��
	Byte MsecHi;		//! �����λ��0~59999
	Byte MsecLo;		//! �����λ
	Byte CRCFirst;
	Byte CRCSecond;
}DLLSJZLPCHECKTIMEREQUEST;

//! �����ƶ��豸Уʱ֡
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 00H
	Byte StartLo;		//! 00H
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 03H
	Byte ByteCount;		//! 06H
	Byte Year;		    //! ��: ʵ�����ȥ2000
	Byte Month;			//! ��
	Byte Day;			//! ��
	Byte Hour;			//! ʱ
	Byte Minute;		//! ��
	Byte Second;		//! ��
	Byte CRCFirst;
	Byte CRCSecond;
}ZSZDCHECKTIMEREQUEST;

//! ������1500V���ع�S7-300�豸Уʱ֡(��������1��2����)
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 
	Byte StartLo;		//! 
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 05H
	Byte ByteCount;		//! 0AH
	Byte YearHi;		//! �� 20H(BCD, ��ͬ)
	Byte YearLo;		//! �� 13H
	Byte Month;			//! �� 
	Byte Day;			//! ��
	Byte Hour;			//! ʱ
	Byte Minute;		//! ��
	Byte Second;		//! ��
	Byte MsecHi;		//! �ٷ��� 0~99H
	Byte MsecLo;		//! ����   0~9H
	Byte Rev;			//! 0
	Byte CRCFirst;
	Byte CRCSecond;
}DL_SIEMENSS73_CHECKTIMEREQUEST;

//! �Ϻ�����1500V���ع�U-MLEs-SC�豸Уʱ֡(��������1��2����)
typedef struct {
	Byte SlaveAddress;
	Byte Function;		//! 10H
	Byte StartHi;		//! 
	Byte StartLo;		//! 
	Byte PointHi;		//! 00H
	Byte PointLo;		//! 04H
	Byte ByteCount;		//! 08H
	Byte YearHi;		//! �� 20H����λ���������ֽڣ�
	Byte YearLo;		//! �� 13H(BCD, ��ͬ)
	Byte Month;			//! �� 
	Byte Day;			//! ��
	Byte Hour;			//! ʱ
	Byte Minute;		//! ��
	Byte Second;		//! ��
	Byte CendSecond;	//! �ٷ��� 0~99H
	Byte CRCFirst;
	Byte CRCSecond;
}DL_UMLESSC_CHECKTIMEREQUEST;

//!�����෿��CSC268�豸Уʱ֡
typedef struct  
{
	Byte bySlaveAddr;		//!�豸��ַ
	Byte byFunc;			//!������
	Byte byStartHi;		//!��ʼ��ַ��λ
	Byte byStartLo;		//!��ʼ��ַ��λ
	Byte byRegCountHi;	//!�Ĵ���������λ
	Byte byRegCountLo;	//!�Ĵ���������λ
	Byte byCount;			//!�ֽ���
	Byte byYearHi;		//!��-���ֽ�
	Byte byYearLo;		//!��-���ֽ�
	Byte byMonth;			//!��
	Byte byDay;			//!��
	Byte byHour;			//!ʱ
	Byte byMinute;		//!��
	Byte byMillSecHi;		//!����-���ֽ�
	Byte byMillSecLo;		//!����-���ֽ�
	Byte byCRCFirst;		//!CRCУ��
	Byte byCRCSecond;		
}SF_CSC268_CHECKTIME;

#ifndef MAKEDWORD
#define MAKEDWORD(l, h) ((DWORD)(((WORD)(l)) | ((DWORD)((WORD)(h))) << 16))
#endif

//! �Ϻ�����1500V���ع�S7-200�豸Уʱ֡(��������1��2����)����Ҫ����ṹ������
typedef struct {
	Byte	m_bySlaveAddr;		/*!< ���͵�ַ��*/
	Byte	m_byFunction;		/*!< �����롣*/
	Byte	m_byStartAddrHi;	/*!< ��ʼ�Ĵ�����λ��*/
	Byte	m_byStartAddrLo;	/*!< ��ʼ�Ĵ�����λ��*/
	Byte	m_byRegNoHi;		/*!< �Ĵ���������λ��*/
	Byte	m_byRegNoLo;		/*!< �Ĵ���������λ��*/
	Byte	m_byByteCount;		/*!< �ֽڸ�����*/
	Byte	m_byYearHi;			/*!< �꣬��λ��*/
	Byte	m_byYearLo;			/*!< �꣬��λ��*/
	Byte	m_byMonthHi;		/*!< �£���λ��*/
	Byte	m_byMonthLo;		/*!< �£���λ��*/
	Byte	m_byDayHi;			/*!< �죬��λ��*/
	Byte	m_byDayLo;			/*!< �죬��λ��*/
	Byte	m_byHourHi;			/*!< Сʱ����λ��*/
	Byte	m_byHourLo;			/*!< Сʱ����λ��*/
	Byte	m_byMinuteHi;		/*!< ���ӣ���λ��*/
	Byte	m_byMinuteLo;		/*!< ���ӣ���λ��*/
	Byte	m_bySecondHi;		/*!< �룬��λ��*/
	Byte	m_bySecondLo;		/*!< �룬��λ��*/
	Byte	m_byCRCFirst;		/*!< CRCУ���һλ��*/
	Byte	m_byCRCSecond;		/*!< CRCУ��ڶ�λ��*/
}DL_SIEMENSS72_CHECKTIMEREQUEST;



#pragma pack( pop )

namespace MACS_SCADA_SUD
{
/*!
*	\brief	��ȡini�����ļ���ĳ��������Ϣ
*
*	\param	pchAppName	�ڵ�������"[]"������
*	\param	pchKeyName	�ؼ�����
*	\param	nDefault	Ĭ��ֵ����ȡʧ���򷵻ظ�ֵ
*	\param	pchFileName	�ļ���
*
*	\retval	int	ini�����ļ��ж�Ӧ���ֵ����ȡʧ���򷵻�Ĭ��ֵ
*/
extern FEPMODBUSDRV_API int GetPrivateInifileInt(const char* pchAppName, 
				const char* pchKeyName, int nDefault, const char* pchFileName);

FEPMODBUSDRV_API std::string GetPrivateInifileString(const char* pchAppName,
				const char* pchKeyName, std::string  strDefault, const char* pchFileName);
}

#endif
