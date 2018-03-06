/*!
 *	\file	$FILE_BASE$.$FILE_EXT$
 *
 *	\brief	104��վ�����õ����������Ͷ����ļ�
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

//===================����֡�е�ʱ���ʽ========================================//
//! �֣�0-59
typedef struct{
	OpcUa_Byte	Minute:6;
	OpcUa_Byte	Res:1;
	OpcUa_Byte	MinValid:1;		//ʱ���Ƿ���Ч
}structMin;

//! ʱ��0-23
typedef struct{
	OpcUa_Byte	Hours:5;
	OpcUa_Byte	Res:2;
	OpcUa_Byte	RU:1;	//0---��׼ʱ�䣻1--����ʱ��
}structHour;

//! �죺1-31
typedef struct{
	OpcUa_Byte	MonDay:5;
	OpcUa_Byte	WeekDay:3;
}structDay;

//! �£�1-12
typedef struct{
	OpcUa_Byte	Mon:4;
	OpcUa_Byte	Res:4;
}structMon;

//! �꣺0-99
typedef struct{
	OpcUa_Byte	Year:7;
	OpcUa_Byte	Res:1;
}structYear;

//! ����֡�е�ʱ���ʽ��
typedef struct{
	WORD		wMs;		//���룺0-59999
	structMin	stMin;		//��
	structHour	stHour;		//����Уʱ��Ч
	structDay	stDay;		//����Уʱ��Ч
	structMon	stMon;		//����Уʱ��Ч
	structYear	stYear;		//����Уʱ��Ч
}ASDUTime;	

typedef struct {
	WORD		wMs;		//���룺0-59999
}Time16bit;

typedef struct {
	WORD		wMs;		//���룺0-59999
	structMin	stMin;		//��
}Time24bit;
//===================����֡�е�ʱ���ʽ========================================//


//===================APCI��ʽ==================================================//
//! APCI���������������ָ�ʽ��I��ʽ��S��ʽ��U��ʽ��
typedef struct {
	OpcUa_Byte byCtrlArea1;
	OpcUa_Byte byCtrlArea2;
	OpcUa_Byte byCtrlArea3;
	OpcUa_Byte byCtrlArea4;
}structCtrlArea;

//! APCI������I��ʽ
typedef struct {
	WORD Flag:1;	//I��ʽ�����롣����0
	WORD Ns:15;		//�������
	WORD Res:1;		//����������0
	WORD Nr:15;		//�������
}structCtrlAreaI;

//! APCI������S��ʽ
typedef struct {
	WORD Flag:2;	//S��ʽ�����롣���ǣ�bit0=1��bit1=0��
	WORD Ns:14;		//����ȫ0��
	WORD Res:1;		//����������0
	WORD Nr:15;		//�������
}structCtrlAreaS;

//! APCI������U��ʽ
typedef struct {
	OpcUa_Byte Flag:2;		//U��ʽ�����롣���ǣ�bit0=1��bit1=1��
	OpcUa_Byte STARTDT_ACT:1;	//STARTDT��Ч. 1--��Ч(��վ��), ��ͬ.
	OpcUa_Byte STARTDT_CON:1;	//STARTDTȷ��. 1--ȷ��(��վ��), ��ͬ.
	OpcUa_Byte STOPDT_ACT:1;	//STOPDT��Ч
	OpcUa_Byte STOPDT_CON:1;	//STOPDTȷ��
	OpcUa_Byte TESTFR_ACT:1;	//TESTFR��Ч
	OpcUa_Byte TESTFR_CON:1;	//TESTFRȷ��
	OpcUa_Byte Res1:8;		//����������ȫ0��
	WORD Res2:1;		//����0
	WORD Nr:15;			//����ȫ0��
}structCtrlAreaU;

typedef union{
	structCtrlArea CtrlArea;
	structCtrlAreaI CtrlAreaI;
	structCtrlAreaS CtrlAreaS;
	structCtrlAreaU CtrlAreaU;
}unionCtrlArea;

//! APCI
typedef struct {
	OpcUa_Byte byStart;				//����68H
	OpcUa_Byte byAPDULen;				//APDU ���ȡ����253��
	unionCtrlArea unCtrlArea;	//���ϡ�
}APCI;
//===================APCI��ʽ==================================================//


//===================ASDU head��ʽ=============================================//
//! ��Ԫ������ַ
typedef struct {
	OpcUa_Byte byCommAddr;	//������ַ�����0-254��
	OpcUa_Byte bySubAddr;		//��վ��ַ
}COMADDR;

//! ASDUͷ
typedef struct {
	OpcUa_Byte byType;				//����ʶ
	OpcUa_Byte byLimit:7;				//�ɱ�ṹ�޶��ʡ�
	OpcUa_Byte bSQ:1;					//��Ϣ�����ַ�Ƿ�����. 1��ָ����һ�������Ϣ�����ַ������Ĳ�����0��ָÿ���������Ϣ�����ַ��
	OpcUa_Byte byTxReason;			//����ԭ��
	OpcUa_Byte bySrc;					//Դ��ַ��һ��Ϊ0��
	union{
		COMADDR sComAddr;		//��Ԫ������ַ
		WORD	wdCommAddr;
	};
}ASDUHead;
//===================ASDU head��ʽ=============================================//

//! ��Ϣ�����ַ�ṹ
typedef struct {
	WORD wdInfoAddr;			//��Ϣ��ַ��0-65535��
	OpcUa_Byte byRes;					//������
}structInfoAddr;


//===================����ң��֡��ʽ��ѡ��ִ�У�����������֡������֡==========//
//! ң����ϢԪ�أ�SCS(����)
typedef struct{
	OpcUa_Byte	SCS:1;
	OpcUa_Byte	Res:1;
	OpcUa_Byte	QU:5;
	OpcUa_Byte	SE:1;
}structSCS;


//! ң����ϢԪ�أ�DCS(˫��)
typedef struct{
	OpcUa_Byte	DCS:2;
	OpcUa_Byte	QU:5;
	OpcUa_Byte	SE:1;
}structDCS;

//! ң����ϢԪ�أ�RCS(����)
typedef struct{
	OpcUa_Byte	RCS:2;
	OpcUa_Byte	QU:5;
	OpcUa_Byte	SE:1;
}structRCS;

//! ң����ϢԪ��
typedef struct{
	WORD wdInfoAddr;			//��Ϣ��ַ��0-65535��
	OpcUa_Byte byRes;					//������
	union{
		structSCS sSCS;
		structDCS sDCS;
		structRCS sRCS;
	};
}unionInfoElement;

//! ң����ϢԪ��(��ʱ��)
typedef struct{
	WORD wdInfoAddr;			//��Ϣ��ַ��0-65535��
	OpcUa_Byte byRes;					//������
	union{
		structSCS sSCS;
		structDCS sDCS;
		structRCS sRCS;
	};
	ASDUTime sTime;
}unionInfoElementTime;

//! ����ң��֡��ʽ��ѡ��ִ�У�����������֡������֡��
typedef struct {
	APCI			 Apci;
	ASDUHead		 Head;		//ASDU head.
	unionInfoElement uElement;	//��ϢԪ��.
}DOFrame;

//! ����ң��֡��ʽ��ѡ��ִ�У�����������֡������֡��(��ʱ��)
typedef struct {
	APCI			 Apci;
	ASDUHead		 Head;		//ASDU head.
	unionInfoElementTime uElement;	//��ϢԪ��.
}DOFrameTime;
//===================����ң��֡��ʽ��ѡ��ִ�У�����������֡������֡==========//


//! ===================���ٻ�������֡������վ�����ٻ��Ģ�ȷ��֡�͢۽���֡��ʽ===//
typedef struct {
	WORD wdInfoAddr;//��Ϣ��ַ������Ϊ0.
	OpcUa_Byte byRes;		//������
	OpcUa_Byte byQOI;		//ACK��End����Ϊ20��ReqΪ0��20�� 
}structWholeQuery;

typedef struct {
	APCI	 Apci;
	ASDUHead Head;	//��byType==100 && byTxReason == 6; 
					//��byType==100 && byTxReason == 7; 
					//��byType==100 && byTxReason == 10.
	structWholeQuery sWholeQuery;
}WholeQueryReq,WholeQueryRespAck,WholeQueryRespEnd;
//===================���ٻ�������֡������վ�����ٻ��Ģ�ȷ��֡�͢۽���֡��ʽ===//


//===================ң�ŵ㣨���������˫�㣩����ʱ���ʽ======================//
/*
1����ASDUHead.byType == 1 && ASDUHead.byTxReason == 20ʱΪ����ң�ŵ�
2����ASDUHead.byType == 3 && ASDUHead.byTxReason == 20ʱΪ˫��ң�ŵ�

��ʱ����֡���ݽṹΪ��
  APCI + ASDUHead + SPI1 + SPI2 + ... + SPIn
��
  APCI + ASDUHead + DPI1 + DPI2 + ... + DPIn
����ASDUHead.byLimitָʾ��ң�ŵ�ĸ���n��
*/

//! ��Ʒ����ʵĵ�����Ϣ
typedef struct {
	OpcUa_Byte SPI:1;		//SPI := <0> := OFF��    <1> := ON��
	OpcUa_Byte Res:3;		//����������Ϊ0��
	OpcUa_Byte BL:1;		//BL := <0> := δ������  <1> := ������
	OpcUa_Byte SB:1;		//SB := <0> := δ��ȡ��  <1> := ��ȡ��
	OpcUa_Byte NT:1;		//NT := <0> := ��ǰֵ    <1> := �ǵ�ǰֵ
	OpcUa_Byte IV:1;		//IV := <0> := ��Ч      <1> := ��Ч
}structSIQ;

//! ��Ʒ����ʵĵ�����Ϣ
typedef struct {
	OpcUa_Byte DPI:2;		//DPI := <1> :=ȷ��״̬��OFF  <2> := ȷ��״̬��ON  <0><3> :=�м�״̬��ȷ��
	OpcUa_Byte Res:2;		//����������Ϊ0��
	OpcUa_Byte BL:1;		//BL := <0> := δ������  <1> := ������
	OpcUa_Byte SB:1;		//SB := <0> := δ��ȡ��  <1> := ��ȡ��
	OpcUa_Byte NT:1;		//NT := <0> := ��ǰֵ    <1> := �ǵ�ǰֵ
	OpcUa_Byte IV:1;		//IV := <0> := ��Ч      <1> := ��Ч
}structDIQ;

//! ����ң��ֵ���ݽṹSDI
typedef struct {
	WORD wdInfoAddr;//��Ϣ��ַ��0-65535��
	OpcUa_Byte byRes;					//������
	structSIQ sSIQ;
}structSDINoTime;

typedef struct {
	structSIQ sSIQ;
}structSDINoTimeNoInfoAddr;

//! ˫��ң��ֵ���ݽṹDPI
typedef struct {
	WORD wdInfoAddr;//��Ϣ��ַ��0-65535��
	OpcUa_Byte byRes;					//������
	structDIQ sDIQ;
}structDDINoTime;

typedef struct {
	structDIQ sDIQ;
}structDDINoTimeNoInfoAddr;
//===================ң�ŵ㣨���������˫�㣩����ʱ���ʽ======================//


//===================ң�ŵ㣨���������˫�㣩��ʱ���ʽ========================//
/*
1����ASDUHead.byType == 2 && ASDUHead.byTxReason == 20ʱΪ�����ʱ��ң�ŵ�
2����ASDUHead.byType == 4 && ASDUHead.byTxReason == 20ʱΪ˫���ʱ��ң�ŵ�

��ʱ����֡���ݽṹΪ��
  APCI + ASDUHead + SPIT1 + SPIT2 + ... + SPITn
��
  APCI + ASDUHead + DPIT1 + DPIT2 + ... + DPITn
����ASDUHead.byLimitָʾ��ң�ŵ�ĸ���n��
*/
//! ����ң��ֵ���ݽṹSPIT
typedef struct {
	structSDINoTime sSDINoTime;
	Time24bit  sTime;	
}structSDI_Time24;

typedef struct {
	structSDINoTimeNoInfoAddr sSDINoTime;
	Time24bit  sTime;	
}structSDI_Time24NoInfoAddr;

//! ˫��ң��ֵ���ݽṹDPIT
typedef struct {
	structDDINoTime sDDINoTime;
	Time24bit  sTime;	
}structDDI_Time24;

typedef struct {
	structDDINoTimeNoInfoAddr sDDINoTime;
	Time24bit  sTime;	
}structDDI_Time24NoInfoAddr;

//! ����ң��ֵ���ݽṹ
typedef struct {
	structSDINoTime sSDINoTime;
	ASDUTime  sTime;	
}structSDI_Time;

typedef struct {
	structSDINoTimeNoInfoAddr sSDINoTime;
	ASDUTime  sTime;	
}structSDI_TimeNoInfoAddr;

//! ˫��ң��ֵ���ݽṹ
typedef struct {
	structDDINoTime sDDINoTime;
	ASDUTime  sTime;	
}structDDI_Time;

typedef struct {
	structDDINoTimeNoInfoAddr sDDINoTime;
	ASDUTime  sTime;	
}structDDI_TimeNoInfoAddr;
//===================ң�ŵ㣨���������˫�㣩��ʱ���ʽ========================//


//===================ң��㲻��ʱ���ʽ========================================//
/*
1����ASDUHead.byType == 9, 11  ʱΪ����ʱ��ң���

  ��ʱ����֡���ݽṹΪ��
  APCI + ASDUHead + RT1 + RT2 + ... + RTn
  ����ASDUHead.byLimitָʾ��ң���ĸ���n��
*/
//! Ʒ��������
typedef struct {
	OpcUa_Byte OV:1;		//OV := <0> :=δ���	 <1> :=���
	OpcUa_Byte Res:3;		//����������Ϊ0��
	OpcUa_Byte BL:1;		//BL := <0> := δ������  <1> := ������
	OpcUa_Byte SB:1;		//SB := <0> := δ��ȡ��  <1> := ��ȡ��
	OpcUa_Byte NT:1;		//NT := <0> := ��ǰֵ    <1> := �ǵ�ǰֵ
	OpcUa_Byte IV:1;		//IV := <0> := ��Ч      <1> := ��Ч
}QDS;

typedef struct {
	WORD Val:15;
	WORD Sign:1;
}structNVA;

typedef struct {
	short Val;
}structSVA;

//! ����ʱ��ң����16λRT
typedef struct{
	WORD	wdInfoAddr;
	OpcUa_Byte byRes;					//������
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

//! ����Ʒ�������ʡ�����ʱ��ң����16λRT
typedef struct{
	WORD	wdInfoAddr;
	OpcUa_Byte byRes;					//������
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

//! ����ʱ���32λRT
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//������
	float fValue;
	QDS Quality;
}structRT32bit;

typedef struct {
	float fValue;
	QDS Quality;
}structRT32bitNoInfoAddr;

//===================ң��㲻��ʱ���ʽ========================================//

//===================ң����ʱ���ʽ==========================================//
/*
1����ASDUHead.byType == 10, 12 ʱΪ��ʱ��ң���

  ��ʱ����֡���ݽṹΪ��
  APCI + ASDUHead + RTT1 + RTT2 + ... + RTTn
  ����ASDUHead.byLimitָʾ��ң���ĸ���n��
*/
//��ʱ��ң16λ����RTT
typedef struct{
	structRT16bit sRT;
	Time24bit  sTime;	
}structRT16bit_Time24;

typedef struct{
	structRT16bitNoInfoAddr sRT;
	Time24bit  sTime;	
}structRT16bit_Time24NoInfoAddr;

//��ʱ��ң32λ����RTT
typedef struct{
	structRT32bit sRT;
	Time24bit  sTime;	
}structRT32bit_Time24;

typedef struct{
	structRT32bitNoInfoAddr sRT;
	Time24bit  sTime;	
}structRT32bit_Time24NoInfoAddr;

//��ʱ��ң16λ����RTT
typedef struct{
	structRT16bit sRT;
	ASDUTime  sTime;	
}structRT16bit_Time;

typedef struct{
	structRT16bitNoInfoAddr sRT;
	ASDUTime  sTime;	
}structRT16bit_TimeNoInfoAddr;

//��ʱ��ң32λ����RTT
typedef struct{
	structRT32bit sRT;
	ASDUTime  sTime;	
}structRT32bit_Time;

typedef struct{
	structRT32bitNoInfoAddr sRT;
	ASDUTime  sTime;	
}structRT32bit_TimeNoInfoAddr;
//===================ң����ʱ���ʽ==========================================//

//=========================ң�����ʽ==========================================//
typedef struct {
	OpcUa_Byte QL:7;	//<0>            ��=δ��
				//<1..15>            ��=Ϊ�����ױ�׼�ı�׼���屣��(���ݷ�Χ)
				//<16..31>            ��= Ϊ�ض�ʹ�ñ���(ר�÷�Χ)
	OpcUa_Byte SE:1;	//<0>            ��=ִ�� 
				//<1>            ����ѡ��
}structQOS;

//16λң��
typedef struct{
	WORD	wdInfoAddr;
	OpcUa_Byte byRes;					//������
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


//32λң��
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//������
	float fValue;
	structQOS sQOS;
}structRC32bit;

typedef struct {
	APCI		  Apci;
	ASDUHead	  Head;	//ASDU head.
	structRC32bit RC;	
}RC32bitFrame;


//32λ�ִ�ң��
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//������
	DWORD dwBSI;		//����״̬��Ϣ,32����
}structRC32bitstring;

typedef struct {
	APCI		  Apci;
	ASDUHead	  Head;	//ASDU head.
	structRC32bitstring RC;	
}RC32bitstringFrame;


//16λң��(��ʱ��)
typedef struct{
	WORD	wdInfoAddr;
	OpcUa_Byte byRes;					//������
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


//32λң��(��ʱ��)
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//������
	float fValue;
	structQOS sQOS;
	ASDUTime sTime;
}structRC32bitTime;

typedef struct {
	APCI		  Apci;
	ASDUHead	  Head;	//ASDU head.
	structRC32bitTime RC;	
}RC32bitTimeFrame;


//32λ�ִ�ң��(��ʱ��)
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//������
	DWORD dwBSI;		//����״̬��Ϣ,32����
	ASDUTime sTime;
}structRC32bitstringTime;

typedef struct {
	APCI		  Apci;
	ASDUHead	  Head;	//ASDU head.
	structRC32bitstringTime RC;	
}RC32bitstringTimeFrame;
//=========================ң�����ʽ==========================================//


//============================SOE��ʽ==========================================//
/*
1����ASDUHead.byType == 30 && ASDUHead.byTxReason == 3ʱΪ����SOE
2����ASDUHead.byType == 31 && ASDUHead.byTxReason == 3ʱΪ˫��SOE

��ʱ����֡���ݽṹΪ��
  APCI + ASDUHead + SPIT1 + SPIT2 + ... + SPITn
��
  APCI + ASDUHead + DPIT1 + DPIT2 + ... + DPITn
����ASDUHead.byLimitָʾ��ң�ŵ�ĸ���n��
ע��: ʱ����Ϣȫ����Ч.
*/
//============================SOE��ʽ==========================================//

//! ============================��ѹ����λ��״̬=================================
typedef struct {
	OpcUa_Byte Value:6;	//��ֵ: -64~63.
	OpcUa_Byte Sign:1;	//����λ: 1--����; 0--����.
	OpcUa_Byte T:1;		//<0> ��=�豸δ��˲��״̬      <1> ��=�豸����˲��״̬
}VTI;

//! ���VTI��ֵ, 
// ����: 
// VTI a;
// char chVTIVal = GetVTIVal(a);
//#define GetVTIVal(a) ((((VTI)(a)).Sign == 0)?(((VTI)(a)).Value):(-((~(((VTI)(a)).Value))&63)+1) )
#define GetVTIVal(a) ((((VTI)(a)).Sign == 0)?(((VTI)(a)).Value):(-((~((((VTI)(a)).Value))&63)+1)) )


//! ��ѹ����λ��״̬�仯(ASDUHead.byType == 32 && (ASDUHead.byTxReason == 5,20,33) )
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//! ������
	VTI	 sVTI;
	QDS  Quality;
}structVTI;

typedef struct {
	VTI	 sVTI;
	QDS  Quality;
}structVTINoInfoAddr;

//! ��ѹ����λ��״̬(ASDUHead.byType == 32 && (ASDUHead.byTxReason == 3,11,12) )
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
//============================��ѹ����λ��״̬=================================//

//! ============================��վԶ���ն�״̬��32���ش���=====================
typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//! ������
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
//============================��վԶ���ն�״̬��32���ش���=====================//

//! ============================�����ۼ���=======================================
typedef struct {
	OpcUa_Byte SQ:5;
	OpcUa_Byte CY:1;
	OpcUa_Byte CA:1;
	OpcUa_Byte IV:1;
}structSN;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//������
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
//============================�����ۼ���=======================================//


//! ============================��ʱ��ļ̵籣��װ���¼�=========================
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
	OpcUa_Byte byRes;					//������
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
//============================��ʱ��ļ̵籣��װ���¼�=========================//


//! ===========��ʱ��ļ̵籣��װ�ó��������¼�==================================
//! �̵籣���豸�ĵ����¼�
typedef struct {
	OpcUa_Byte GS:1;
	OpcUa_Byte SL1:1;
	OpcUa_Byte SL2:1;
	OpcUa_Byte SL3:1;
	OpcUa_Byte SIE:1;
	OpcUa_Byte SRD:1;
	OpcUa_Byte Res:2;	//����Ϊ0
}structSPE;

//! �̵����豸��Ʒ�����
typedef struct {
	OpcUa_Byte Res:4;	//����Ϊ0
	OpcUa_Byte BL:1;
	OpcUa_Byte SB:1;
	OpcUa_Byte NT:1;
	OpcUa_Byte IV:1;
}structQDP;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//! ������
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
//===========��ʱ��ļ̵籣��װ�ó��������¼�==================================//

//! ====================��ʱ��ļ̵籣��װ�ó��������·��Ϣ=====================
typedef struct {
	OpcUa_Byte GC:1;
	OpcUa_Byte CL1:1;
	OpcUa_Byte CL2:1;
	OpcUa_Byte CL3:1;
	OpcUa_Byte Res:4;	//! ����Ϊ0
}structOCI;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//! ������
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
//====================��ʱ��ļ̵籣��װ�ó��������·��Ϣ=====================//

//! ====================����λ����ĳ��鵥����Ϣ=================================
typedef struct {
	WORD wdST;
	WORD wdCD;
}structSCD;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//������
	structSCD sSCD;
	QDS sQDP;
}structDIGroup;

typedef struct {
	structSCD sSCD;
	QDS sQDP;
}structDIGroupNoInfoAddr;
//====================����λ����ĳ��鵥����Ϣ=================================//

//! ============================Уʱ=============================================
typedef struct {
	WORD		wdInfoAddr;	//! ����Ϊ0
	OpcUa_Byte		byRes;		//! ������
	ASDUTime	sTime;
}structAdjustTime;

typedef struct {
	APCI		Apci;
	ASDUHead	Head;		//! ASDU head.
	structAdjustTime sAdjustTime;
}AdjustTime;
//============================Уʱ=============================================//

//! ============================��ʼ��===========================================
typedef struct {
	OpcUa_Byte COI:7;	//<0>         ��=���ص�Դ����
				//<1>         ��=�����ֶ���λ
				//<2>         ��=Զ����λ
				//<3..31>     ��=Ϊ�����ױ�׼�ı�׼���屣��(���ݷ�Χ)
				//<32..127>   ��=Ϊ�ض�ʹ�ñ���(ר�÷�Χ)

	OpcUa_Byte BS:1;	//<0>         ��=δ�ı䵱�ز����ĳ�ʼ��
				//! <1>         ��=�ı䵱�ز�����ĳ�ʼ��
}structCOI;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//������
	structCOI sCOI;
}structInit;
//============================��ʼ��===========================================

//! ===========================�������ٻ�����====================================
typedef struct {
	OpcUa_Byte RQT:6; //! <0>            �����������������δ���ã�
				//! <1>            ���������������1��          
				//! <2>            ���������������2��            
				//! <3>            ���������������3��         
				//! <4>            ���������������4��      
				//! <5>            �����ܵ����������  
				//! <6..31>        ����Ϊ�����ױ�׼�ı�׼���屣��(���ݷ�Χ)
				//! <32..63>       ����Ϊ�ض�ʹ�ñ���(ר�÷�Χ)

	OpcUa_Byte FRZ:2;	//! <0>            ������(�޶���͸�λ��
				//! <1>            �������������᲻����λ���������ֵ�����ۼƣ�          
				//! <2>            �����������������λ���������ֵ����������Ϣ��            
				//! <3>            ������������λ

}structQCC;

typedef struct {
	WORD  wdInfoAddr;
	OpcUa_Byte byRes;					//������
	structQCC sQCC;
}structCountQuery;

typedef struct {
	APCI		Apci;
	ASDUHead	Head;		//ASDU head.
	structCountQuery sCountQuery;
}CountQueryFrame;

//===========================�������ٻ�����====================================//

//! =============================��������========================================
typedef struct {
	WORD  wdInfoAddr;	//! 0
	OpcUa_Byte byRes;			//! ������
	WORD wdFBP;			//! �̶�Ϊ0x55AA
}structTest;

typedef struct {
	WORD  wdInfoAddr;	//! 0
	OpcUa_Byte byRes;			//! ������
	WORD wdFBP;			//! �̶�Ϊ0x55AA
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
//=============================��������========================================

//! ============================��λ��������=====================================
typedef struct {
	WORD  wdInfoAddr;//! 0
	OpcUa_Byte byRes;		 //! ������
	OpcUa_Byte byQRP;		 //! <0>        ����δ����
					 //! <1>        �������̵��ܸ�λ
					 //! <2>        ������λ�¼��������ȴ�����Ĵ�ʱ�����Ϣ
					 //! <3..127>   ����Ϊ�����ױ�׼�ı�׼���屣��(���ݷ�Χ)      
					 //! <128..255> ����Ϊ�ض�ʹ�ñ���(ר�÷�Χ)

}structResetProc;
//============================��λ��������=====================================

//! ��ʱ�������
typedef struct {
	WORD  wdInfoAddr; //! 0
	OpcUa_Byte byRes;		  //! ������
	Time16bit time16;
}structDelay;

//! ============================����ֵ����=======================================
//! ����ֵ�����޶���(QPM)
//! ����ֵ�������µĲ���ֵ������Ҫ��Ĳ���ֵ��С�ı�����
//! �����������ֵ��������ֵ�����������������ֵ���䡣
//! ÿ��ϵͳ��Ψһ����Ϣ�����ַΪÿһ�ֲ������ж��塣
typedef struct {
	OpcUa_Byte KPA:6;		//! <0>            ����δ��
					//! <1>            ��������ֵ        
					//! <2>            ����ƽ��ϵ��(�˲�ʱ�䳣��)      
					//! <3>            ��������ֵ���͵�����        
					//! <4>            ��������ֵ���͵�����      
					//! <5..31>        ����Ϊ�����ױ�׼�ı�׼���屣��(���ݷ�Χ)
					//! <32..63>       ����Ϊ�ض�ʹ�ñ���(ר�÷�Χ)

	OpcUa_Byte LOP1:1;	//���ز����ı�: <0> ��=δ�ı�	<1>   �����ı�
	OpcUa_Byte LOP2:1;	//����������:   <0> ��=����     <1>   ����δ����
}structQPM;

//! 16λ����ֵ����
typedef struct{
	WORD	wdInfoAddr;
	OpcUa_Byte byRes;					//������
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

//! 32λ����ֵ����
typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//������
	float fValue;
	structQPM sQPM;
}structPara32bit;

typedef struct {
	float fValue;
	structQPM sQPM;
}structPara32bitNoInfoAddr;
//============================����ֵ����=======================================//


//! ============================��������=========================================//
typedef struct {
	OpcUa_Byte QPA:6; //<0>            ����δ��
				//<1>            ��������/��ֹ������֮ǰװ�صĲ���(��Ϣ�����ַ=0)1)     
				//<2>            ��������/��ֹ������Ѱַ��Ϣ����Ĳ���1)
				//<3>            ��������/��ֹ������Ѱַ�ĳ���ѭ�������ڴ������Ϣ����       
				//<4..31>        ����Ϊ�����ױ�׼�ı�׼���屣��(���ݷ�Χ)
				//<32..63>       ����Ϊ�ض�ʹ�ñ���(ר�÷�Χ)

	OpcUa_Byte Res:2;
}structQPA;

typedef struct {
	WORD wdInfoAddr;
	OpcUa_Byte byRes;					//������
	structQPA sQPA;
}structParaAct;
//! ============================��������=========================================//

//! ��ǩ����UnitType
enum{
	MACS_IEC104_DI = 1,	//! ң��
	MACS_IEC104_AI,		//! ң��
	MACS_IEC104_DOF,	//! ����. ����ң��/ң���㹲��һ�����ŵ�.1��ʾ�ɹ�,0��ʾʧ�ܻ����ڵȴ����ؽ��.
	MACS_IEC104_SDOa,	//! ң��. ��ң��ѡ��
	MACS_IEC104_SDOb,	//! ң��. ��ң��ִ��
	MACS_IEC104_SDOc,	//! ң��. ��ң������
	MACS_IEC104_DDOa,	//! ң��. ˫ң��ѡ��
	MACS_IEC104_DDOb,	//! ң��. ˫ң��ִ��
	MACS_IEC104_DDOc,	//! ң��. ˫ң������
	MACS_IEC104_UPDOa,	//! ң��. ������ѡ��
	MACS_IEC104_UPDOb,	//! ң��. ������ִ��
	MACS_IEC104_UPDOc,	//! ң��. ����������
	MACS_IEC104_TSDOa,	//! ң��. ��ң����ʱ�꣩��ѡ��
	MACS_IEC104_TSDOb,	//! ң��. ��ң����ʱ�꣩��ִ��
	MACS_IEC104_TSDOc,	//! ң��. ��ң����ʱ�꣩������
	MACS_IEC104_TDDOa,	//! ң��. ˫ң����ʱ�꣩��ѡ��
	MACS_IEC104_TDDOb,	//! ң��. ˫ң����ʱ�꣩��ִ��
	MACS_IEC104_TDDOc,	//! ң��. ˫ң����ʱ�꣩������
	MACS_IEC104_TUPDOa,	//! ң��. ��������ʱ�꣩��ѡ��
	MACS_IEC104_TUPDOb,	//! ң��. ��������ʱ�꣩��ִ��
	MACS_IEC104_TUPDOc,	//! ң��. ��������ʱ�꣩������
	MACS_IEC104_NVAAOa,	//! ң������һ��ֵ��ѡ��
	MACS_IEC104_NVAAOb,	//! ң������һ��ֵ��ִ��
	MACS_IEC104_NVAAOc,	//! ң������һ��ֵ������
	MACS_IEC104_SVAAOa,	//! ң������Ȼ�ֵ��ѡ��
	MACS_IEC104_SVAAOb,	//! ң������Ȼ�ֵ��ִ��
	MACS_IEC104_SVAAOc,	//! ң������Ȼ�ֵ������
	MACS_IEC104_FAOa,	//! ң�����̸�������ѡ��
	MACS_IEC104_FAOb,	//! ң�����̸�������ִ��
	MACS_IEC104_FAOc,	//! ң�����̸�����������
	MACS_IEC104_DWAOa,	//! ң����32λ�ִ���ѡ��
	MACS_IEC104_DWAOb,	//! ң����32λ�ִ���ִ��
	MACS_IEC104_DWAOc,	//! ң����32λ�ִ�������
	MACS_IEC104_TNVAAOa,	//! ң������һ��ֵ����ʱ�꣩��ѡ��
	MACS_IEC104_TNVAAOb,	//! ң������һ��ֵ����ʱ�꣩��ִ��
	MACS_IEC104_TNVAAOc,	//! ң������һ��ֵ����ʱ�꣩������
	MACS_IEC104_TSVAAOa,	//! ң������Ȼ�ֵ����ʱ�꣩��ѡ��
	MACS_IEC104_TSVAAOb,	//! ң������Ȼ�ֵ����ʱ�꣩��ִ��
	MACS_IEC104_TSVAAOc,	//! ң������Ȼ�ֵ����ʱ�꣩������
	MACS_IEC104_TFAOa,	//! ң�����̸���������ʱ�꣩��ѡ��
	MACS_IEC104_TFAOb,	//! ң�����̸���������ʱ�꣩��ִ��
	MACS_IEC104_TFAOc,	//! ң�����̸���������ʱ�꣩������
	MACS_IEC104_TDWAOa,	//! ң����32λ�ִ�����ʱ�꣩��ѡ��
	MACS_IEC104_TDWAOb,	//! ң����32λ�ִ�����ʱ�꣩��ִ��
	MACS_IEC104_TDWAOc,	//! ң����32λ�ִ�����ʱ�꣩������

	MACS_IEC104_JDBH = MACS_IEC104_AI,	//! �̵籣��
	MACS_IEC104_JDBHX = MACS_IEC104_AI,	//! �̵籣��(��չ)
	MACS_IEC104_CS = MACS_IEC104_AI,	//! ����
	MACS_IEC104_SD = MACS_IEC104_AI,	//! �趨
	MACS_IEC104_DNLJ = MACS_IEC104_AI,	//! �����ۼ���
	MACS_IEC104_BWZ = MACS_IEC104_AI,	//! ��λ����Ϣ
	MACS_IEC104_EJZ = MACS_IEC104_AI,	//! �򱻿�վ���Ͷ�������Ϣ
	MACS_IEC104_BCD = MACS_IEC104_AI,	//! BCD ��
	MACS_IEC104_RTS = MACS_IEC104_AI,	//! Զ���ն�״̬. DWORD(32λ)
	MACS_IEC104_FT = MACS_IEC104_AI		//! �ļ�����
};

//�������͵����ʼλ��( ����������λ�� = ��Ϣ�����ַ-��������ʼ��ַ )������λ�ü��������.
/*
#define	DI_START_ADDR	0X01		//ң�š���Ϣ�����ַ��ΧΪ1H~400H��
#define DI_END_ADDR		0X400		//ң�š���Ϣ�����ַ��ΧΪ1H~400H��	
#define MAX_DI_NUM		(DI_END_ADDR-DI_START_ADDR+1)	//1024
#define	AI_START_ADDR	0X701		//ң�����Ϣ�����ַ��ΧΪ701H~900H
#define	AI_END_ADDR		0X900		//ң�����Ϣ�����ַ��ΧΪ701H~900H
#define MAX_AI_NUM		(AI_END_ADDR-AI_START_ADDR+1)	//512
#define DO_START_ADDR	0XB01		//ң�ء�������ַ��ΧΪB01H~B80H��
#define DO_END_ADDR		0XB80		//ң�ء�������ַ��ΧΪB01H~B80H��
#define MAX_DO_NUM		(DO_END_ADDR-DO_START_ADDR+1)	//128
#define JDBH_START_ADDR1 0X401		//�̵籣������Ϣ�����ַ��ΧΪ401H~700H��
									//����˵�ַ��Χ�����û�������ʹ�õ�ַ��ΧE01H~FFFH��
#define JDBH_END_ADDR1  0X700		//�̵籣������Ϣ�����ַ��ΧΪ401H~700H��
									//����˵�ַ��Χ�����û�������ʹ�õ�ַ��ΧE01H~FFFH��
#define MAX_JDBH_NUM1	(JDBH_END_ADDR1-JDBH_START_ADDR1+1)	//768
#define JDBH_START_ADDR2 0XE01		//�̵籣������Ϣ�����ַ��ΧΪ401H~700H��
									//����˵�ַ��Χ�����û�������ʹ�õ�ַ��ΧE01H~FFFH��
#define JDBH_END_ADDR2  0XFFF		//�̵籣������Ϣ�����ַ��ΧΪ401H~700H��
									//����˵�ַ��Χ�����û�������ʹ�õ�ַ��ΧE01H~FFFH��
#define MAX_JDBH_NUM2	(JDBH_END_ADDR2-JDBH_START_ADDR2+1)	//768
#define CS_START_ADDR	0X901		//������ַ��ΧΪ901H~B00H��
#define CS_END_ADDR		0XB00		//������ַ��ΧΪ901H~B00H��
#define MAX_CS_NUM		(CS_END_ADDR-CS_START_ADDR+1)	//512
#define SD_START_ADDR	0XB81		//�趨��ַ��ΧΪB81H~C00H��
#define SD_END_ADDR		0XC00		//�趨��ַ��ΧΪB81H~C00H��
#define MAX_SD_NUM		(SD_END_ADDR-SD_START_ADDR+1)	//128
#define DNLJ_START_ADDR 0XC01		//�����ۼ�����ַ��ΧΪC01H~C80H��
#define DNLJ_END_ADDR	0XC81		//�����ۼ�����ַ��ΧΪC01H~C80H��
#define MAX_DNLJ_NUM	(DNLJ_END_ADDR-DNLJ_START_ADDR+1)	//128
#define BWZ_START_ADDR	0XC81		//��λ����Ϣ��ַ��ΧΪC81H~CA0H
#define BWZ_END_ADDR	0XCA0		//��λ����Ϣ��ַ��ΧΪC81H~CA0H
#define MAX_BWZ_NUM		(BWZ_END_ADDR-BWZ_START_ADDR+1)		//32
#define EJZ_START_ADDR	0XCA1		//�򱻿�վ���Ͷ�������Ϣ��ַ��ΧΪCA1H~CB0H�� 
#define EJZ_END_ADDR	0XCB0		//�򱻿�վ���Ͷ�������Ϣ��ַ��ΧΪCA1H~CB0H�� 
#define MAX_EJZ_NUM		(EJZ_END_ADDR-EJZ_START_ADDR+1)		//16
#define BCD_START_ADDR	0XCB1		//BCD ���ַ��ΧΪ CB1H~CC0H�� 
#define BCD_END_ADDR	0XCC0		//BCD ���ַ��ΧΪ CB1H~CC0H�� 
#define MAX_BCD_NUM		(BCD_END_ADDR-BCD_START_ADDR+1)	
#define RTS_START_ADDR	0XCC1		//Զ���ն�״̬��ַCC1H�� 
#define RTS_END_ADDR	0XCC1		//Զ���ն�״̬��ַCC1H��
#define MAX_RTS_NUM		(RTS_END_ADDR-RTS_START_ADDR+1) 
#define FT_START_ADDR	0XCC2		//�ļ����͵�ַCC2H~E00H��
#define FT_END_ADDR		0XE00		//�ļ����͵�ַCC2H~E00H��
#define MAX_FT_NUM		(FT_END_ADDR-FT_START_ADDR+1)
*/
//�������͵����ʼλ��( ����������λ�� = ��Ϣ�����ַ-��������ʼ��ַ )������λ�ü��������.
/*	//Del by hxw in 2008.05.15
#define	DI_START_ADDR	0X0001		//ң�š���Ϣ�����ַ��Χ
#define DI_END_ADDR		0X1000		//ң�š���Ϣ�����ַ��Χ	
#define MAX_DI_NUM		(DI_END_ADDR-DI_START_ADDR+1)	
#define	AI_START_ADDR	0X4001		//ң�����Ϣ�����ַ��Χ
#define	AI_END_ADDR		0X5000		//ң�����Ϣ�����ַ��Χ
#define MAX_AI_NUM		(AI_END_ADDR-AI_START_ADDR+1)	
#define DO_START_ADDR	0X6001		//ң�ء�������ַ��Χ
#define DO_END_ADDR		0X6200		//ң�ء�������ַ��Χ
#define MAX_DO_NUM		(DO_END_ADDR-DO_START_ADDR+1)
#define JDBH_START_ADDR 0X1001		//�̵籣������Ϣ�����ַ��Χ
#define JDBH_END_ADDR   0X4000		//�̵籣������Ϣ�����ַ��Χ
#define MAX_JDBH_NUM	(JDBH_END_ADDR-JDBH_START_ADDR+1)
#define CS_START_ADDR	0X5001		//������ַ��Χ
#define CS_END_ADDR		0X6000		//������ַ��Χ
#define MAX_CS_NUM		(CS_END_ADDR-CS_START_ADDR+1)	
#define SD_START_ADDR	0X6201		//�趨��ַ��Χ
#define SD_END_ADDR		0X6400		//�趨��ַ��Χ
#define MAX_SD_NUM		(SD_END_ADDR-SD_START_ADDR+1)
#define DNLJ_START_ADDR 0X6401		//�����ۼ�����ַ��Χ
#define DNLJ_END_ADDR	0X6600		//�����ۼ�����ַ��Χ
#define MAX_DNLJ_NUM	(DNLJ_END_ADDR-DNLJ_START_ADDR+1)	
#define BWZ_START_ADDR	0X6601		//��λ����Ϣ��ַ��Χ
#define BWZ_END_ADDR	0X6700		//��λ����Ϣ��ַ��Χ
#define MAX_BWZ_NUM		(BWZ_END_ADDR-BWZ_START_ADDR+1)		
#define EJZ_START_ADDR	0X6701		//�򱻿�վ���Ͷ�������Ϣ��ַ��Χ
#define EJZ_END_ADDR	0X6800		//�򱻿�վ���Ͷ�������Ϣ��ַ��Χ
#define MAX_EJZ_NUM		(EJZ_END_ADDR-EJZ_START_ADDR+1)	
#define RTS_START_ADDR	0X6801		//Զ���ն�״̬��ַ
#define RTS_END_ADDR	0X6801		//Զ���ն�״̬��ַ
#define MAX_RTS_NUM		(RTS_END_ADDR-RTS_START_ADDR+1) 
#define FT_START_ADDR	0X6802		//�ļ����͵�ַ
#define FT_END_ADDR		0X7000		//�ļ����͵�ַ
#define MAX_FT_NUM		(FT_END_ADDR-FT_START_ADDR+1)
*/
#define	DI_START_ADDR	0X0001		//! ң�š���Ϣ�����ַ��Χ
#define DI_END_ADDR		0XFFFF		//! ң�š���Ϣ�����ַ��Χ
#define MAX_DI_NUM		(DI_END_ADDR-DI_START_ADDR+1)	
#define	AI_START_ADDR	0X0001		//! ң�����Ϣ�����ַ��Χ
#define	AI_END_ADDR		0XFFFF		//! ң�����Ϣ�����ַ��Χ
#define MAX_AI_NUM		(AI_END_ADDR-AI_START_ADDR+1)	
#define DO_START_ADDR	0X0001		//! ң�ء�������ַ��Χ
#define DO_END_ADDR		0XFFFF		//! ң�ء�������ַ��Χ
#define MAX_DO_NUM		(DO_END_ADDR-DO_START_ADDR+1)
#define JDBH_START_ADDR 0X0001		//! �̵籣������Ϣ�����ַ��Χ
#define JDBH_END_ADDR   0XFFFF		//! �̵籣������Ϣ�����ַ��Χ
#define MAX_JDBH_NUM	(JDBH_END_ADDR-JDBH_START_ADDR+1)
#define CS_START_ADDR	0X0001		//! ������ַ��Χ
#define CS_END_ADDR		0XFFFF		//! ������ַ��Χ
#define MAX_CS_NUM		(CS_END_ADDR-CS_START_ADDR+1)	
#define SD_START_ADDR	0X0001		//! �趨��ַ��Χ
#define SD_END_ADDR		0XFFFF		//! �趨��ַ��Χ
#define MAX_SD_NUM		(SD_END_ADDR-SD_START_ADDR+1)
#define DNLJ_START_ADDR 0X0001		//! �����ۼ�����ַ��Χ
#define DNLJ_END_ADDR	0XFFFF		//! �����ۼ�����ַ��Χ
#define MAX_DNLJ_NUM	(DNLJ_END_ADDR-DNLJ_START_ADDR+1)	
#define BWZ_START_ADDR	0X0001		//! ��λ����Ϣ��ַ��Χ
#define BWZ_END_ADDR	0XFFFF		//! ��λ����Ϣ��ַ��Χ
#define MAX_BWZ_NUM		(BWZ_END_ADDR-BWZ_START_ADDR+1)		
#define EJZ_START_ADDR	0X0001		//! �򱻿�վ���Ͷ�������Ϣ��ַ��Χ
#define EJZ_END_ADDR	0XFFFF		//! �򱻿�վ���Ͷ�������Ϣ��ַ��Χ
#define MAX_EJZ_NUM		(EJZ_END_ADDR-EJZ_START_ADDR+1)	
#define RTS_START_ADDR	0X0001		//! Զ���ն�״̬��ַ
#define RTS_END_ADDR	0XFFFF		//! Զ���ն�״̬��ַ
#define MAX_RTS_NUM		(RTS_END_ADDR-RTS_START_ADDR+1) 
#define FT_START_ADDR	0X0001		//! �ļ����͵�ַ
#define FT_END_ADDR		0XFFFF		//! �ļ����͵�ַ
#define MAX_FT_NUM		(FT_END_ADDR-FT_START_ADDR+1)


//! IEC8705-104��Լ����APDU��������
#define MIN_APDU_LEN		4
#define MAX_APDU_LEN		253
#define MIN_I_APDU_LEN		12		//! I��ʽ��С֡��(���ٻ�ȷ��, ���ٻ�����)	

//! IEC8705-104��Լ������ʼ��,��ʼ��
#define PACK_HEAD			 0x68 //! �ɱ�֡��������ʼ��
#define YC_RFAC                24 //! ң������ѹ������(32/8=4)

//! IEC8705-104��Լ���Ʒ������еĹ�����
#define MFUN_ResetLink       0x00 //! ��λԶ����·
#define MFUN_ResetProcess    0x01 //! ��λ�û�����
#define MFUN_TestLink        0x02 //! ������·
#define MFUN_SendData3       0x03 //! ��������(Ҫȷ��)
#define MFUN_SendData4       0x04 //! ��������(����ȷ��)
#define MFUN_Visit           0x08 //! ��Ӧ֡Ӧ˵������Ҫ��
#define MFUN_CallLinkStatus  0x09 //! �ٻ���·״̬
#define MFUN_Request1        0x0a //! �ٻ�1������
#define MFUN_Request2        0x0b //! �ٻ�2������

//! IEC8705-104��Լ���ӷ������еĹ�����
#define RFUN_Confirm         0x00 //! ȷ��
#define RFUN_LinkBUsy        0x01 //! ��·æ
#define RFUN_RespondWithData 0x08 //! ��������Ӧ����
#define RFUN_DataWithoutCall 0x09 //! �����ٻ�������
#define RFUN_LinkStatus      0x0b //! ����·״̬
#define RFUN_LinkNotWork     0x0e //! ��·����δ����
#define RFUN_LinkNotFinish   0x0f //! ��·����δ���

#define RDIR                 0x80 //! �������ֽڵ�bit7=1,��ʾ���䷽��ΪRTU->��վ
#define RACD                 0x20 //! �������ֽڵ�bit5=1,��ʾRTU�е�ǰ��ͻ��������1������

//! IEC8705-104��Լ���Ʒ�������ͱ�ʶ
#define C_SC_NA_1            45   //! ����ң������
#define C_DC_NA_1            46   //! ˫��ң������
#define C_RC_NA_1            47   //! ��������
#define C_SE_NA_1            48   //! �趨����, ��һ��ֵ
#define C_SE_NB_1			 49   //! �趨����, ��Ȼ�ֵ
#define C_SE_NC_1		     50   //! �趨����, �̸�����
#define C_BO_NA_1			 51   //! �趨����, 32���ش�
#define C_SC_TA_1			 58	  //! ��ʱ��CP56Time2a�ĵ�����
#define C_DC_TA_1			 59	  //! ��ʱ��CP56Time2a��˫����
#define C_RC_TA_1			 60   //! ��ʱ��CP56Time2a����������
#define C_SE_TA_1			 61   //! ��ʱ��CP56Time2a���趨ֵ����,��һ��ֵ
#define C_SE_TB_1			 62	  //! ��ʱ��CP56Time2a���趨ֵ����,��Ȼ�ֵ
#define C_SE_TC_1			 63	  //! ��ʱ��CP56Time2a���趨ֵ����,�̸�����
#define C_BO_TA_1			 64	  //! ��ʱ��CP56Time2a��32���ش�


#define C_IC_NA_1            100  //! �ٻ�����
#define C_CI_NA_1            101  //! ���������ٻ�����
#define C_RD_NA_1            102  //! ����������
#define C_CS_NA_1            103  //! ʱ��ͬ������
#define C_TS_NA_1            104  //! ��������
#define C_RP_NA_1            105  //! ��λ��������
#define C_CD_NA_1			 106  //! ��ʱ�������
#define C_TS_TA_1			 107  //! ��ʱ��CP56Time2a�Ĳ�������


//! �ڼ��ӷ��������Ӧ�÷������ݵ�Ԫ
#define P_ME_NA_1			 110 //! ����ֵ����, ��һ��ֵ
#define P_ME_NB_1			 111 //! ����ֵ����,  ��Ȼ�ֵ
#define P_ME_NC_1			 112 //! ����ֵ����,  �̸�����
#define P_AC_NA_1			 113 //! ��������

//! �ļ������Ӧ�÷������ݵ�Ԫ
#define F_FR_NA_1			 120 //! �ļ�׼������
#define F_SR_NA_1			 121 //! ��׼������
#define F_SC_NA_1			 122 //! �ٻ�Ŀ¼, ѡ���ļ�, �ٻ��ļ�,�ٻ���
#define F_LS_NA_1			 123 //! ���Ľ�,���Ķ�
#define F_AF_NA_1			 124 //! �Ͽ��ļ�, �Ͽɽ�
#define F_SG_NA_1			 125 //! ��
#define F_DR_TA_1			 126 //! Ŀ¼


//! IEC8705-104��Լ���ӷ�������ͱ�ʶ
#define M_SP_NA_1            0x01 //! ����ʱ��ĵ�ң
#define M_SP_TA_1            0x02 //! ��ʱ��ĵ�ң
#define M_DP_NA_1            0x03 //! ����ʱ���˫ң
#define M_DP_TA_1            0x04 //! ��ʱ���˫ң
#define M_ST_NA_1            0x05 //! ����ʱ��Ĳ�λ����Ϣ(��ѹ���ֽ�ͷ)
#define M_ST_TA_1            0x06 //! ��ʱ��Ĳ�λ����Ϣ(��ѹ���ֽ�ͷ)
#define M_BO_NA_1            0x07 //! ��վԶ���ն�״̬(32λλ��)
#define M_BO_TA_1            0x08 //! ��ʱ�����վԶ���ն�״̬(32λλ��)
#define M_ME_NA_1            0x09 //! ����ʱ�����ͨ����ֵ
#define M_ME_TA_1            0x0a //! ��ʱ�����ͨ����ֵ
#define M_ME_NB_1            0x0b //! Measured value,scaled value 
#define M_ME_TB_1            0x0c //! Measured value,scaled value with time tag 
#define M_ME_NC_1            0x0d //! ����ʱ��Ķ̸������ֵ
#define M_ME_TC_1            0x0e //! ��ʱ��Ķ̸������ֵ
#define M_IT_NA_1            0x0f //! ����ʱ��ĵ����������
#define M_IT_TA_1            0x10 //! ��ʱ��ĵ����������
#define M_EP_TA_1            0x11 //! ��ʱ��ĵ��������豸�¼�
#define M_EP_TB_1            0x12 //! ��ʱ��ĳ��鱣���豸�¼�
#define M_EP_TC_1			 0x13 //! ��ʱ��ļ̵籣��װ�ó��������·��Ϣ
#define M_PS_NA_1            0x14 //! ����״̬��λ����ĳ��鵥����Ϣ
#define M_ME_ND_1            0x15 //! ����Ʒ����������ͨ����ֵ
#define M_SP_TB_1			 0x1E //! ��ʱ��CP56Time2a�ĵ�����Ϣ
#define M_DP_TB_1			 0x1F //! ��ʱ��CP56Time2a��˫����Ϣ
#define M_ST_TB_1		     0x20 //! ��ʱ��Ĳ�λ����Ϣ
#define M_BO_TB_1            0x21 //! ��ʱ��CP56Time2a��32���ش�
#define M_ME_TD_1			 0x22 //! ����ֵ, ��ʱ��CP56Time2a�Ĺ�һ��ֵ
#define M_ME_TE_1			 0x23 //! ����ֵ, ��ʱ��CP56Time2a�ı�Ȼ�ֵ
#define M_ME_TF_1			 0x24 //! ����ֵ, ��ʱ��CP56Time2a�Ķ̸�����
#define M_IT_TB_1			 0x25 //! ��ʱ��CP56Time2a���ۼ���
#define M_EP_TD_1			 0X26 //! ��ʱ��CP56Time2a�ļ̵籣��װ���¼�
#define M_EP_TE_1			 M_EP_TB_1	
#define M_EP_TF_1			 40   //! ��ʱ��CP56Time2a�ļ̵籣��װ�ó��������·��Ϣ
#define M_EI_NA_1            0x46 //! ��ʼ������(70)
#define M_BD_NA_1            0xe8 //! BCD���ˮλֵ

//! IEC8705-104��Լ�Ĵ���ԭ��
#define CAUSE_cyc            0x01 //! ����ѭ��
#define CAUSE_back           0x02 //! ����ɨ��
#define CAUSE_spont          0x03 //! ͻ��
#define CAUSE_init           0x04 //! ��ʼ��
#define CAUSE_req            0x05 //! ���������
#define CAUSE_act            0x06 //! ����
#define CAUSE_actcon         0x07 //! ����ȷ��
#define CAUSE_deact          0x08 //! ֹͣ����
#define CAUSE_daactcon       0x09 //! ֹͣ����ȷ��
#define CAUSE_actterm        0x0a //! �������
#define CAUSE_retrem         0x0b //! Զ����������ķ�����Ϣ
#define CAUSE_retloc         0x0c //! ������������ķ�����Ϣ
#define CAUSE_file           0x0d //! �ļ�����
#define CAUSE_OpcUa_Introgen       0x14 //! ��Ӧ���ٻ�(20)
#define CAUSE_OpcUa_Intro1         0x15 //! ��Ӧ��1���ٻ�(21)
#define CAUSE_OpcUa_Intro2         0x16 //! ��Ӧ��2���ٻ�(22)
#define CAUSE_OpcUa_Intro3         0x17 //! ��Ӧ��3���ٻ�(23)
#define CAUSE_OpcUa_Intro4         0x18 //! ��Ӧ��4���ٻ�(24)
#define CAUSE_OpcUa_Intro5         0x19 //! ��Ӧ��5���ٻ�(25)
#define CAUSE_OpcUa_Intro6         0x1a //! ��Ӧ��6���ٻ�(26)
#define CAUSE_OpcUa_Intro7         0x1b //! ��Ӧ��7���ٻ�(27)
#define CAUSE_OpcUa_Intro8         0x1c //! ��Ӧ��8���ٻ�(28)
#define CAUSE_OpcUa_Intro9         0x1d //! ��Ӧ��9���ٻ�(29)
#define CAUSE_OpcUa_Intro10        0x1e //! ��Ӧ��10���ٻ�(30)
#define CAUSE_OpcUa_Intro11        0x1f //! ��Ӧ��11���ٻ�(31)
#define CAUSE_OpcUa_Intro12        0x20 //! ��Ӧ��12���ٻ�(32)
#define CAUSE_OpcUa_Intro13        0x21 //! ��Ӧ��13���ٻ�(33)
#define CAUSE_OpcUa_Intro14        0x22 //! ��Ӧ��14���ٻ�(34)
#define CAUSE_OpcUa_Intro15        0x23 //! ��Ӧ��15���ٻ�(35)
#define CAUSE_OpcUa_Intro16        0x24 //! ��Ӧ��16���ٻ�(36)
#define CAUSE_reqcogen       0x25 //! ��Ӧ������ٻ�(37)
#define CAUSE_reqco1         0x26 //! ��Ӧ��1�����ٻ�(38)
#define CAUSE_reqco2         0x27 //! ��Ӧ��2�����ٻ�(39)
#define CAUSE_reqco3         0x28 //! ��Ӧ��3�����ٻ�(40)
#define CAUSE_reqco4         0x29 //! ��Ӧ��4�����ٻ�(41)

#pragma pack(pop)

#endif
