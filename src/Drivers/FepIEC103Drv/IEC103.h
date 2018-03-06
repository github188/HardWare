/*!
*	\file	IEC103.h
*
*	\brief	103��վ�����õ����������Ͷ����ļ�
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/IEC103.h 1     11-01-21 10:02a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-01-21 10:02a $
*	$Revision: 1 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef __IEC103_H_
#define __IEC103_H_

#define PERIOD_SCAN_MS						1000				//! ���ڶ�ʱ���������ڣ����롣

/*
TEMPLATE						UNIT_TYPE	RAW_TYPE	BIT_WIDTH	LOW	HIGH	COMMENT

DI%U(0,1,1024)						0	0	1	0	0	ң����
AI%U(0,1,20)						1	2	32	0	0	ң����
DO%U(0,1,255)						2	0	1	0	0	ң����
DOS%U(0,1,255)						3	0	1	0	0	ң��ѡ����
DIC%U(0,1,255)						4	0	1	0	0	ң�ط�����
DIS%U(0,1,255)						5	0	1	0	0	ѡ������

DIG%U(0,0,255)%*1000:%+U(0,0,255)	6	0	1	0	0	ң��--Group:Index
AIG%U(0,0,255)%*1000:%+U(0,0,255)	7	2	32	0	0	ң��--Group:Index
DIB%U(0,0,255)%*1000:%+U(0,0,255)	8	0	1	0	0	ң��--Fun:Inf
AIB%U(0,0,255)%*1000:%+U(0,0,255)	9	2	32	0	0	ң��--Fun:Inf
DOB%U(0,0,255)%*1000:%+U(0,0,255)	10	0	1	0	0	ң��ִ������--Fun:Inf
DOSB%U(0,0,255)%*1000:%+U(0,0,255)	11	0	1	0	0	ң��ѡ������--Fun:Inf
DICB%U(0,0,255)%*1000:%+U(0,0,255)	12	0	1	0	0	�·����У--Fun:In
COT%U(0,0,255)%*1000:%+U(0,0,255)	13	8	8	0	0	������ASDU10Ӧ���COT
RdFV%U(0,0,255)%*1000:%+U(0,0,255)	14	0	1	0	0	����ֵ--Group:Index
WrFV%U(0,0,255)%*1000:%+U(0,0,255)	15	8	8	0	0	д��ֵ--Group:Index
*/


//����ֵ��ӦMacsDIS103.dbf�ļ��е�UnitType���ֵ
#define MacsDIS103_DI		0			//ң��	        
#define MacsDIS103_AI		1			//ң��	          
#define MacsDIS103_DO		2			//ң��
#define MacsDIS103_DOS		3			//ң��ȷ�ϵ�
#define MacsDIS103_DIC		4			//ң�ط�У��
#define MacsDIS103_DIS		5			//ѡ��У��

//Add by hxw in 2012.9.5
#define MacsDIS103_DIG		6			//ң��--Group:Entry
#define MacsDIS103_AIG		7			//ң��--Group:Entry
#define MacsDIS103_DIB		8			//ң��--Fun:Inf
#define MacsDIS103_AIB		9			//ң��--Fun:Inf
#define MacsDIS103_DOB		10			//ң��ִ������--Fun:Inf
#define MacsDIS103_DOSB		11			//ң��ѡ������--Fun:Inf
#define MacsDIS103_DICB		12			//�·����У--Fun:Inf��Group:Entry
#define MacsDIS103_COT		13			//������ASDU10Ӧ���COT
#define MacsDIS103_RdFV		14			//����ֵ--Group:Entry: ASDU140
#define MacsDIS103_WrFV		15			//д��ֵ--Group:Entry: ASDU232
#define MacsDIS103_ASDU21RD	16			//ASDU21��ȡ--Group:Entry��ע�⣬Entry=0��ʾ��ȡ����

//////////////////////////////////////�������ͱ�ʶ//////////////////////////////////
//�����б���
#define TYP_COMM_DATA				10		//ͨ�÷������ݱ������ͱ�ʶ

//���б���
#define TYP_TIME_SYN				6		//ʱ��ͬ���������ͱ�ʶ
#define TYP_GENERAL_QUERY_START		7		//�ܲ�ѯ�����ٻ��������������ͱ�ʶ
#define TYP_GENERAL_CMD				20		//һ����������ͱ�ʶ
#define TYP_COMM_CMD				21		//ͨ�÷�����������ͱ�ʶ
#define TYP_DISORDER_DATA_TRANS		24		//�Ŷ����ݴ������������ͱ�ʶ
#define TYP_DISORDER_DATA_ACK		25		//�Ŷ����ݴ�����Ͽɱ������ͱ�ʶ
#define TYP_ASDU140					140		//��ʩ�͵±�����ֵ�鱨�����ͱ�ʶ
#define TYP_ASDU232					232		//дʩ�͵±�����ֵ�鱨�����ͱ�ʶ

//���б���
#define TYP_WITH_TIME				1		//��ʱ��ı���
#define TYP_WITH_TINE_REL			2		//�����ʱ��ı���
#define TYP_VALUE1					3		//����ֵI
#define TYP_VALUE_WITH_TIME			4		//�����ʱ�����ʱ��ı���ֵ����
#define TYP_IDENTIFIER				5		//��ʶ�������ͱ�ʶ
#define TYP_GENERAL_QUERY_END		8		//�ܲ�ѯ�����ٻ�����ֹ�������ͱ�ʶ
#define TYP_VALUE2					9		//����ֵII
#define TYP_COMM_IDENTIFIER			11		//ͨ�÷����ʶ�������ͱ�ʶ
#define TYP_ASDU17					17		//ʩ�͵±�����ֵ
#define TYP_DISORDER_DATA			23		//����¼���Ŷ��������ͱ�ʶ
#define TYP_DISORDER_DATA_READY		26		//�Ŷ����ݴ���׼�������������ͱ�ʶ
#define TYP_CHANNEL_READY			27		//����¼��ͨ������׼�������������ͱ�ʶ
#define TYP_STATUS_ALTER_READY		28		//����־��״̬��λ����׼�������������ͱ�ʶ
#define TYP_STATUS_ALTER_TRANS		29		//���ʹ���־��״̬��λ�������ͱ�ʶ
#define TYP_DISORDER_TRANS			30		//�����Ŷ�ֵ�������ͱ�ʶ
#define TYP_TRANS_END				31		//���ͽ����������ͱ�ʶ
#define TYP_ASDU171					171		//ʩ�͵±����й���ȼ��޹����

////////////////////////////////////�ɱ�ṹ�޶���///////////////////////////////////
#define VSQ_MOST	129		//����������ĵĸ���ֵ��Ϊ81(���ˡ�����¼���Ŷ���)


////////////////////////////////////����ԭ�򣨳���עΪ��˫���⣬����������ڼ������з���///////////////////////////////////
#define COT_AUTO					1		//�Է���ͻ����
#define COT_CYC						2		//ѭ��
#define COT_FCB						3		//��λ֡����λ
#define COT_CU						4		//��λͨ�ŵ�Ԫ
#define COT_START					5		//����
#define COT_RESTART					5		//��������
#define COT_POWERON					6		//��Դ����
#define COT_TEST					7		//����
#define COT_TIME_SYN				8		//ʱ��ͬ��---------------˫��
#define COT_GENERAL_QUERY			9		//�ܲ�ѯ�����ٻ���-------˫��
#define COT_GENERAL_QUERY_END		10		//�ܲ�ѯ�����ٻ�����ֹ
#define COT_LOCAL_OP				11		//���ز���
#define COT_REMOTE_OP				12		//Զ������
#define COT_GENERAL_CMD_CONFIRM		20		//����Ŀ϶��Ͽ�---------˫��
#define COT_GENERAL_CMD_DENY		21		//����ķ��Ͽ�
#define COT_DISORDER_DATA_TRANS		31		//�Ŷ����ݵĴ���---------˫��
#define COT_COMM_WRITE_CMD			40		//ͨ�÷���д����
#define COT_COMM_READ_CMD			42		//ͨ�÷��������
#define COT_COMM_WRITE_CMD_CONFIRM	40		//ͨ�÷���д����Ŀ϶��Ͽ�-------------˫��
#define COT_COMM_WRITE_CMD_DENY		41		//ͨ�÷���д����ķ��Ͽ�
#define COT_COMM_READ_CMD_VALID		42		//��ͨ�÷����������Ч������Ӧ---------˫��
#define COT_COMM_READ_CMD_INVALID	43		//��ͨ�÷����������Ч������Ӧ
#define COT_COMM_WRITE_CONFIRM		44		//ͨ�÷���дȷ��


/////////////////////////////////��������/////////////////////////////////////
#define FUN_DISTANCE_PRO			128		//���뱣��
#define FUN_CURRENT_PRO				160		//��������
#define FUN_TRANSFORMER_DIF_PRO		176		//��ѹ�������
#define FUN_LINE_DIF_PRO			192		//��·�����
#define FUN_COMM_SORT				254		//ͨ�÷��๦��
#define FUN_GLB						255		//ȫ�ֹ�������



//////////////////////////////////��Ϣ��ţ�����עΪ�����������С��⣬��������������м����з���////////////////////////////////////
//240 ��255 Ϊͨ�÷��๦��
#define INF_NOUSE						0		//δʹ��
#define INF_TIME_SYN_OR_QUERY_BEGIN		0		//ʱ��ͬ�����ܲ�ѯ�����ٻ���������
#define INF_FCB							2		//��λ֡����λ
#define INF_CU							3		//��λͨ�ŵ�Ԫ
#define INF_START_OR_RESTART			4		//����/��������---------����������
#define INF_POWERON						5		//��Դ����--------------����������
#define	INF_READ_TEAM_TITLE				240		//�������
#define	INF_READ_ALL_ITEM				241		//��һ�����ȫ����Ŀ��ֵ������
#define	INF_READ_SINGLE_CATALOG			243		//��������Ŀ��Ŀ¼
#define	INF_READ_SINGLE_ITEM			244		//��������Ŀ��ֵ������
#define INF_COMM_DATA_GENERAL_QUERY		245		//��ͨ�÷��������ܲ�ѯ
#define INF_WRITE						248		//д��Ŀ
#define INF_WRITE_WITH_CONFIRM			249		//��ȷ�ϵ�д��Ŀ
#define INF_WRITE_WITH_EXCUTE			250		//��ִ�е�д��Ŀ
#define INF_WRITE_WITH_CANCEL			251		//д��Ŀ��ֹ


/////////////////////////////////��������/////////////////////////////////////
#define TOO_FAULT_SELCET				1		//���ϵ�ѡ��
#define TOO_START_DISDATA				2		//�����Ŷ�����
#define TOO_PAUSE_DISDATA				3		//��ֹ�Ŷ�����
#define TOO_CHANEL_REQUSET				8		//ͨ��������
#define TOO_CHANEL_PAUSE				9		//ͨ������ֹ
#define TOO_START_STACHANGE_WITHSIG		16		//�������־��״̬��λ
#define TOO_PAUSE_STACHANGE_WITHSIG		17		//��ֹ����־��״̬��λ

#define	TOO_REQ_DIS_TABLE				24		//���󱻼�¼���Ŷ���
#define	TOO_DISDATA_END_WITHOUTPAUSE	32		//������ֹ���Ŷ����ݴ���Ľ���
#define	TOO_DISDATA_END_WITHCONPAUSE	33		//�ɿ���ϵͳ����ֹ���Ŷ����ݴ���Ľ���
#define	TOO_DISDATA_END_WITHDEVPAUSE	34		//�ɼ̵籣���豸����ֹ���Ŷ����ݴ���Ľ���
#define	TOO_CHANEL_END_WITHOUTPAUSE		35		//������ֹ��ͨ������Ľ���
#define	TOO_CHANEL_END_WITHCONPAUSE		36		//�ɿ���ϵͳ����ֹ��ͨ������Ľ���
#define	TOO_CHANEL_END_WITHDEVPAUSE		37		//�ɼ̵籣���豸����ֹ��ͨ������Ľ���
#define	TOO_ALTER_END_WITHOUTPAUSE		38		//������ֹ�Ĵ���־��״̬��λ����Ľ���
#define	TOO_ALTER_END_WITHCONPAUSE		39		//�ɿ���ϵͳ����ֹ�Ĵ���־��״̬��λ����Ľ���
#define	TOO_ALTER_END_WITHDEVPAUSE		40		//�ɼ̵籣���豸����ֹ�Ĵ���־��״̬��λ����Ľ���

#define TOO_DISDATA_SUCCESS				64		//�Ŷ����ݴ���ɹ�
#define TOO_DISDATA_FAIL				65		//�Ŷ����ݴ��䲻�ɹ�
#define TOO_CHANEL_SUCCESS				66		//ͨ������ɹ�
#define TOO_CHANEL_FAIL					67		//ͨ�����䲻�ɹ�
#define TOO_STATUSCHANGED_SUCCESS		68		//����־��״̬��λ����ɹ�
#define TOO_STATUSCHANGED_FAIL			69		//����־��״̬��λ���䲻�ɹ�


/////////////////////////////////�Ŷ�ֵ����///////////////////////////////////
#define TOV_UNUSED						0		//δ��
#define TOV_SHUNSHI_VALUE				1		//˲ʱֵ
#define TOV_FUZHI_VALUE				   201		//��ֵ

/////////////////////////////////ʵ��ͨ�����/////////////////////////////////
#define	ACC_NO_CHANEL					0		//��ͨ������ʱʹ��
#define	ACC_I_L1						1		//IL1
#define	ACC_I_L2						2		//IL2
#define	ACC_I_L3						3		//IL3
#define ACC_I_N							4		//IN
#define ACC_U_L1E						5		//UL1E
#define ACC_U_L2E						6		//UL2E
#define ACC_U_L3E						7		//UL3E
#define ACC_U_EN						8		//UEN

/////////////////////////////////ͨ�÷�����������GDD----DataType//////////////////////////////
#define GDD_DT0_NODATA			0	//������
#define GDD_DT1_OS8ASCII		1	//ASCII 8 λ��
#define GDD_DT2_BS1				2	//����8 λ��
#define GDD_DT3_UI				3	//�޷�������
#define GDD_DT4_I				4	//����
#define GDD_DT5_UFLOAT			5	//�޷��Ÿ�����
#define GDD_DT6_FLOAT			6	//��������F��:Ҳ��IEEE ��׼754 ��ʵ��
#define GDD_DT7_FLOAT			7	//IEEE ��׼754 ��ʵ����R32.23��
#define GDD_DT8_DOUBLE			8	//IEEE ��׼754 ʵ����R64.53��
#define GDD_DT9_DPI				9	//˫����Ϣ����B.6.5
#define GDD_DT10_SPI			10	//������Ϣ
#define GDD_DT11_DPI			11	//��˲��Ͳ���˫����Ϣ:0= ˲�䣨TRANSIENT �� 1= ����OFF �� 2= �ϣ�ON��3= ����ERROR��
#define GDD_DT12_AITime			12	//��Ʒ�������ı���ֵ  ��B.6.8
#define GDD_DT18_DPITime		18	//��ʱ��ı���:= CP48{˫����Ϣ��DPI�������ã�RES�����ĸ� 8 λλ��ʱ�䣨TIME���� ������Ϣ��SIN��} 
#define GDD_DT19_DPITime		19	//�����ʱ���ʱ�걨��:= CP48{˫����Ϣ��DPI�������ã�RES�������ʱ��, �����������, �ĸ� 8 λλ��ʱ�䣨TIME���� ������Ϣ��SIN��} 
#define GDD_DT22_ResponseCode	22	//ͨ�÷���ش���. ��7.2.6.36
#define GDD_DT24_Index			24	//����
#define GDD_DT203_CP72			203	//�����ֽ�ʱ��ı���:= CP72{˫����Ϣ��DPI�������ã�RES�����߸� 8 λλ��ʱ�䣨TIME����������Ϣ��SIN��} 
#define GDD_DT204_CP104			204	//�����ʱ������ֽ�ʱ��ı���:= CP72{˫����Ϣ��DPI�������ã�RES�������ʱ��, �����������, �߸� 8 λλ��ʱ�䣨TIME����������Ϣ��SIN��} 

//�������KOD
#define KOD0_NONE				0
#define KOD1_ACTUALVALUE		1 //  ʵ��ֵ 
#define KOD2_DEFAULTVALUE		2 //  ȱʡֵ 
#define KOD3_RANGE				3 //  ���̣���Сֵ�����ֵ�������� 
#define KOD4_NOUSE				4 //  δ�� 
#define KOD5_CORRECTDEGREE		5 //  ׼ȷ�� 
#define KOD6_FACTOR				6 //  ����
#define KOD7_RATIO				7 // % �α� 
#define KOD8_LIST				8 //  �б� 
#define KOD9_DIMESION			9 //  ���� 
#define KOD10_DISCRIPTION		10 // ���� 
#define KOD11_NOUSE				11 // δ�� 
#define KOD12_PASSWORD			12 // ������Ŀ 
#define KOD13_READONLY			13 // ֻ�� 
#define KOD14_WRITEONLY			14 // ֻд 
#define KOD15_NOUSE				15 // δ�� 
#define KOD16_NOUSE				16 // δ�� 
#define KOD17_NOUSE				17 // δ�� 
#define KOD18_NOUSE				18 // δ�� 
#define KOD19_FUN_INFO			19 // ��Ӧ�Ĺ������ͺ���Ϣ��� 
#define KOD20_EVENT				20 // ��Ӧ���¼� 
#define KOD21_TEXTMATRIX		21 // �б���ı�����
#define KOD22_VALUEMATRIX		22 // �б��ֵ���� 
#define KOD23_RELEVANTENTRY		23 // ���������Ŀ 

/////////////////////////////////RII/////////////////////////////////////
#define	RII_CALL_IDENTIFIER				1

//////////////////////////��FT1.2��ص�//////////////////////////////////
#define GUDINGZHEN_START_ID					0x10
#define BIANCHANGZHEN_START_ID				0x68
#define GUDINGZHEN_END_ID					0x16
#define BIANCHANGZHEN_END_ID				0x16
#define GUDINGZHEN_SIZE						5

/*=======================================================
//CONTROL_FIELD����˵��:
������֡��� . �ӡ��͡��� . �������������

��1��	���� .  �ӡ����ĵĿ�����

D7       D6       D5       D4       D3    D2    D1     D0
����     PRM     FCB      FCV                ������
0/1      1	

��A��	PRM����������λ��������Ϣ���䷽��PRM=1����վ����վ��PRM=0����վ����վ��
��B��	FCB�������λ����FCB = 0 / 1������վÿ���վ������һ�ֵġ�����/ȷ�ϡ�������/��Ӧ���������ʱ����FCBȡ������վΪÿ����վ����һ��FCB�Ŀ���������ʱδ�յ�Ӧ������վ�ط����ط����ĵ�FCB���ֲ��䣬�ط�������಻����3�Ρ����ط�3�κ���δ�յ�Ԥ��Ӧ����������ִ������
��C��	FCV ���������Чλ����FCV= 0���� FCB�ı仯��Ч��FCV=1���� FCB�ı仯��Ч������/�޻ش���񡢹㲥���Ĳ����Ǳ��Ķ�ʧ���ظ����䣬����ı�FCB״̬����Щ��FCV��Ϊ0
��D��	�����붨�壨��---�ӷ���
������	 ֡����	        ��������	        FCV״̬
0	     ����/ȷ��֡	��λͨ�ŵ�Ԫ        0
3	     ����/ȷ��֡	��������	        1
4	     ����/�޻ش�֡	��������	        0
7	     ��λ֡����λ	��������	        0
9	     ����/��Ӧ֡	�ٻ���·״̬        0
10	     ����/��Ӧ֡	�ٻ�1������	        1
11��b��	 ����/��Ӧ֡	�ٻ�2�����ݣ�ң�⣩	1
ע�����������3��4�ı���Ϊ�ɱ�֡�����ģ����౨�ľ�Ϊ�̶�֡�����ġ�

(2) ���� . �������ĵĿ�����

D7     D6    D5    D4     D3   D2   D1    D0
����   PRM  ACD  DFC           ������
0       0       
(A)	ACD��Ҫ�����λ����ACD = 1��֪ͨ��վ����վ��I�����������͡�
(B)	DFC������������λ���� DFC = 0��ʾ��վ���Խ������ݣ�DFC ��1��ʾ��վ�������������޷����������ݡ�
(C)	�����루��---������
������	֡����	    ����
0	    ȷ��֡	    ȷ��
1	    ȷ��֡	    ��·æ��δ�յ�����
8	    ��Ӧ֡	    �����ݰ���Ӧ����֡
9	    ��Ӧ֡	    ��վû�����ٻ�������
11	    ��Ӧ֡	    ��վ����·״̬��Ӧ��վ����
ע�����������8�ı���Ϊ�ɱ�֡�����ģ����౨�ľ�Ϊ�̶�֡�����ġ�
============================================================*/


#pragma pack(push, 1)
typedef struct 
{
	OpcUa_Byte	FunCode:4;	//������.������궨��	
	OpcUa_Byte	FCV_DFC:1;	//֡������Чλ.	
	OpcUa_Byte	FCB_ACD:1;	//֡����λ.
	OpcUa_Byte	Prm:1;		//��Ϣ���䷽��.PRM=1����վ����վ��PRM=0����վ����վ
	OpcUa_Byte	Rev:1;		//����,0��1.
}CONTROL_FIELD;	//�����ֶ�

//����ϵͳ��̵籣���豸(�����豸)�����FT1.2֡�й�����Ķ���
#define FRAME_FUNC_RESET_CU					0			//��λͨ�ŵ�Ԫ(CU)�����ڷ���/ȷ��֡	
#define FRAME_FUNC_SEND_DATA				3			//�������ݣ����ڷ���/ȷ��֡	
#define FRAME_FUNC_SEND_DATA2				4			//�������ݣ����ڷ���/�޻ش�֡	
#define FRAME_FUNC_SEND_DATA3				7			//�������ݣ����ڸ�λ֡����λ(FCB)
#define FRAME_FUNC_INQUIRE_LINK_STATUS		9			//�ٻ���·״̬����������/��Ӧ֡
#define FRAME_FUNC_INQUIRE_LEVEL1_DATA		10			//�ٻ�1���û����ݣ���������/��Ӧ֡
#define FRAME_FUNC_INQUIRE_LEVEL2_DATA		11			//�ٻ�2���û����ݣ���������/��Ӧ֡	
//�̵籣���豸(�����豸)�����ϵͳ�����FT1.2֡�й�����Ķ���
#define FRAME_FUNC_CONFIRM					0			//ȷ��֡,ȷ��
#define FRAME_FUNC_DENY						1			//ȷ��֡,��·æ��δ�յ�����
#define FRAME_FUNC_RET_DATA					8			//��Ӧ֡,��������Ӧ����֡
#define FRAME_FUNC_NO_DATA					9			//��Ӧ֡,�����ٻ�������
#define	FRAME_FUNC_ACK_REQUEST				11			//��Ӧ֡,����·״̬���������ش�����֡

//////////////////////////////////////////////////////////////
////////////////103Э�鱨��ASDU�Ļ���Ԫ�ؽṹ/////////////////
#define MAX_IEC103_USER_DATA_LEN		255	//��׼Ϊ253����ʵ�ʿɴ�255	
#define MAX_IEC103_GID_LEN				16
#define MAX_IEC103_FRAM_LEN				(MAX_IEC103_USER_DATA_LEN+6)
#define	MAX_IEC103_REVBUF_LEN			(MAX_IEC103_FRAM_LEN * 5)
#define MIN_IEC103G_GD_LEN				6	//����ͨ�÷������ݵ���С���ȣ�DATAPointHead

#define	IEC103_START_CODE			0x68	
#define	IEC103_END_CODE				0x16
#define IEC103_SHORT_START_CODE		0x10	

#define IEC103_BROADCAST_ADDRESS	255
//���ͱ�ʾ
#define IEC103_SENDTIME_TYPE		6 

//����ԭ��
#define IEC103_COT_BROADCAST_TIME	8

#define MAX_IEC103_INF 256

typedef struct 
{
	OpcUa_Byte	StartCode;			
	OpcUa_Byte	Len;				
	OpcUa_Byte	Relen;				
	OpcUa_Byte	ReStartCode;		
}FRAME_HEAD;

typedef struct 
{
	FRAME_HEAD		Head;		
	CONTROL_FIELD	ControlField;
	OpcUa_Byte			Address;
	OpcUa_Byte			UserData[MAX_IEC103_USER_DATA_LEN + 2];
}CHANGEABLE_FRAME_STRUCT;

typedef struct 
{
	OpcUa_Byte			StartCode;	
	CONTROL_FIELD	ControlField;
	OpcUa_Byte			Address;
	OpcUa_Byte			CheckSum;
	OpcUa_Byte			EndCode;	
}SHORT_FRAME_STRUCT;

typedef	struct 
{
	OpcUa_Byte	TypeFlg;			
	OpcUa_Byte	VSQ;				
	OpcUa_Byte	COT;			
	OpcUa_Byte	ASDU_COM_ADD;
	OpcUa_Byte	FunType;
	OpcUa_Byte	InfNumber;
}ASDU_HEAD_STRUCT;

//! ASDU10֡ͷ
typedef struct 
{
	OpcUa_Byte	Type;			
	OpcUa_Byte	VSQ;				
	OpcUa_Byte	COT;			
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
	OpcUa_Byte    RII;
	OpcUa_Byte    NGD;
}ASDU10_HEAD_STRUCT;

typedef struct 
{
	OpcUa_Byte	Group;				
	OpcUa_Byte	Entry;					
}GIN;

typedef struct 
{
	ASDU10_HEAD_STRUCT sHead;
	GIN		Gin;				
	OpcUa_Byte	KOD;			
}ASDU10NoDataSet;

typedef struct 
{
	GIN		Gin;				
	OpcUa_Byte	KOD;			
}GENERAL_CAT_FLAG;

typedef struct 
{
	OpcUa_Byte	DataType;		
	OpcUa_Byte	DataSize;			
	OpcUa_Byte	Number:7;			
	OpcUa_Byte	CONT:1;			
}GDD;

typedef struct 
{
	GENERAL_CAT_FLAG	GeneralCatFlg;
	GDD	Gdd;
}DATA_SET_FIXEDPART;


typedef struct 
{
	OpcUa_Byte	SPI:1;	
	OpcUa_Byte   RES:3;	
	OpcUa_Byte	SB:1;				
	OpcUa_Byte	BL:1;			
	OpcUa_Byte	NT:1;		
	OpcUa_Byte	IV:1;		
}SINGLE_POINT;
	
typedef struct 
{
	OpcUa_Byte	Info;
	OpcUa_Byte	SPI:1;	
	OpcUa_Byte   RES:3;	
	OpcUa_Byte	SB:1;				
	OpcUa_Byte	BL:1;			
	OpcUa_Byte	NT:1;		
	OpcUa_Byte	IV:1;		
}SINGLE_POINT_INFO;


/*
typedef struct 
{
OpcUa_Byte	DPI:2;	
OpcUa_Byte   RES:6;	
}DOUBLE_POINT_INF;
*/
typedef struct 
{
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte   RES:2;	
	OpcUa_Byte	SB:1;				
	OpcUa_Byte	BL:1;			
	OpcUa_Byte	NT:1;		
	OpcUa_Byte	IV:1;		
}DOUBLE_POINT;

typedef struct 
{
	OpcUa_Byte	Info;
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte   RES:2;	
	OpcUa_Byte	SB:1;				
	OpcUa_Byte	BL:1;			
	OpcUa_Byte	NT:1;		
	OpcUa_Byte	IV:1;		
}DOUBLE_POINT_INFO;

typedef struct 
{
	OpcUa_Byte	OV:1;	
	OpcUa_Byte   RES:3;	
	OpcUa_Byte	BL:1;			
	OpcUa_Byte	SB:1;				
	OpcUa_Byte	NT:1;		
	OpcUa_Byte	IV:1;		
}QDS;

typedef struct  
{
	OpcUa_CharA Val:7;
	OpcUa_CharA T:1;
}VTI;

/*
��Ʒ�������ʵı���ֵ=MEA(MVAL)

=BS1 1 0 1  
�������λ=OV
0 =����� 

1 =��� 

2 0 1  
���λ=ER:=BS1
0 =����ֵ(MVAL)��Ч 

1 =����ֵ(MVAL)��Ч 

δ��(��Ϊ
3 0 0 ) 
����λ=RES:=BS1
4 16 -1 1 2 12  
����ֵ=MVAL:=F13
��MVAL Ϊ���ʱ,�����ֱ��ó����򸺵����ֵ ���ҵ� OV =1 ���� MVAL ����������+/-1.2 ��+/-2.4���Ķֵ
*/
/*	//Del by hxw in 2012.9.5
//MEA
typedef struct  
{
	OpcUa_Int16 OV:1;
	OpcUa_Int16 ER:1;
	OpcUa_Int16 Res:1;
	OpcUa_Int16 Value:13;
}MEA_AI;
*/

/*
��Ʒ�������ʵı���ֵ=MEA(MVAL)

BIT15 =  ���λ��OV����0 =  �������1=  ��� 
BIT14 =  ���λ��ER����0 =����ֵ��MVAL����Ч��1 =����ֵ��MVAL����Ч 
BIT13 =  ����λ��RES����ͨ��Ϊ0 
BIT12~BIT0 =  ����ֵ��MVAL��*/
//MEA
typedef struct  
{
	OpcUa_Int16 OV:1;
	OpcUa_Int16 ER:1;
	OpcUa_Int16 Res:1;
	OpcUa_Int16 Value:13;
}MEA_AI;

/*
typedef struct  
{
	OpcUa_Int16 OV:1;
	OpcUa_Int16 ER:1;
	OpcUa_Int16 Res:1;
	OpcUa_Int16 Value:13;
	OpcUa_Byte Rev[4];
}MEA_AI_ASDU17;
*/

typedef struct  
{
	OpcUa_Int16 Value;
	OpcUa_Byte Rev[4];
}MEA_AI_ASDU17;

/*
���ϵ�״̬��SOF�� 
���ϵ�״̬��SOF��=BS8{TP��TM��TEST��OTEV��RES} 
TP = BIT0 = 0    ����¼�Ĺ���δ��բ = 1   ����¼�Ĺ�����բ 
TM = BIT1 = 0   �Ŷ����ݵȴ����� = 1   �Ŷ��������ڴ��� 
TEST = BIT2 = 0   ����������ʱ��¼���Ŷ����� = 1   �ڲ���ģʽʱ��¼���Ŷ����� 
OTEV = BIT3 = 0   ������/ ������ϴ�������¼���Ŷ����� = 1   �������¼���������¼���Ŷ����� 
RES = BIT4��BIT7 = δ��
*/
typedef struct  
{
	OpcUa_Byte TP:1;
	OpcUa_Byte TM:1;
	OpcUa_Byte TEST:1;
	OpcUa_Byte OTEV:1;
	OpcUa_Byte RES:4;
}SOF;
	
typedef struct 
{
	OpcUa_Byte	LowMillSec;
	OpcUa_Byte	HighMillSec;
	OpcUa_Byte	Minute:6;
	OpcUa_Byte	Res1:1;
	OpcUa_Byte	IV:1;
	OpcUa_Byte	Hour:5;
	OpcUa_Byte	Res2:2;
	OpcUa_Byte	SU:1;
	OpcUa_Byte	Day:5;
	OpcUa_Byte	Week:3;		//1-7
	OpcUa_Byte	Mon:4;
	OpcUa_Byte	Res3:4;
	OpcUa_Byte	Year:7;		//0-99
	OpcUa_Byte	Res4:1;
}CP56Time2a;

typedef struct 
{
	OpcUa_Byte	LowMillSec;
	OpcUa_Byte	HighMillSec;
	OpcUa_Byte	Minute:6;
	OpcUa_Byte	Res1:1;
	OpcUa_Byte	IV:1;
	OpcUa_Byte	Hour:5;
	OpcUa_Byte	Res2:2;
	OpcUa_Byte	SU:1;
}CP32Time2a;

typedef struct  
{
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte    RES:6;
	CP32Time2a Time;
	OpcUa_Byte    SIN;
}CP48;

typedef struct  
{
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte    RES:6;
	CP56Time2a Time;
	OpcUa_Byte    SIN;
}CP72;

typedef struct  
{
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte    RES:6;
	WORD	RET;	//���ʱ��
	WORD	NOF;	//�����������
	CP32Time2a Time;
	OpcUa_Byte    SIN;
}CP80;

typedef struct  
{
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte    RES:6;
	WORD	RET;	//���ʱ��
	WORD	NOF;	//�����������
	CP56Time2a Time;
	OpcUa_Byte    SIN;
}CP104;



//ASDU1
typedef struct  
{
	DOUBLE_POINT DPI;
	CP32Time2a Time;
}ASDU1_DPItm;

//ASDU2
typedef struct  
{
	DOUBLE_POINT DPI;
	WORD	RET;
	WORD	FAN;
	CP32Time2a Time;
}ASDU2_DPItm;

/*
//ASDU38
typedef struct  
{
VTI sVTI;
QDS Quality;
}VTI_QDS;

//ASDU39
typedef struct  
{
VTI sVTI;
QDS Quality;
FOUR_BYTE_TIME_INF Time;
}VTI_QDStm;

//ASDU41
typedef struct  
{
SINGLE_POINT_INF SIQ;
FOUR_BYTE_TIME_INF Time;
}SIQ_TIME;

//ASDU44
typedef struct 
{
WORD DISet;
WORD Change;
QDS Quality;
}SCD_QDS;

*/

//ASDU64
typedef struct  
{
	OpcUa_Byte DCS:2;		//1--��; 2--��. 0, 3�Ƿ�.
	OpcUa_Byte QU:4;		//0--��Ѱַ�Ŀ��ƹ�������; 4--�������غ�բ��բ����;������.
	OpcUa_Byte ACT:1;		//1. SE=1,ACT=0, ң��ѡ��; 2. SE=0,ACT=0, ң��ִ��;
	OpcUa_Byte SE:1;		//3. SE=1,ACT=1, ң�س���.
}DCC;



//���ݵ�Ԫ��ʶ��
typedef struct {
	OpcUa_Byte TypeID;					//���ͱ�ʶ
	OpcUa_Byte AlterStruct;				//�ɱ�ṹ�޶���
	OpcUa_Byte TransReason;				//����ԭ��
	OpcUa_Byte CommonAddr;				//Ӧ�÷������ݵ�Ԫ������ַ
}ASDU_ID;

//��Ϣ���ʶ��
typedef struct {
	OpcUa_Byte FuncType;					//��������
	OpcUa_Byte InfoSeqNO;					//��Ϣ���
}INFOBODY_ID;

//��Ϣ��ʱ��(4BYPE)-103��Լ�б�׼��ʽ
typedef struct {
	WORD Millisecond;				//ʱ��ms
	OpcUa_Byte Min;						//ʱ��min
	OpcUa_Byte Hour;						//ʱ��hour
}TIMESTAMP_4BYTE;

//��Ϣ��ʱ��(7BYPE)-110kvGIS����ͨѶ��Լ�й涨��һ��ʱ���ʽ
typedef struct {
	WORD Millisecond;				//ʱ��ms    0~59999ms
	OpcUa_Byte Min;						//ʱ��min	0~59min
	OpcUa_Byte Hour;						//ʱ��hour	0~23h
	OpcUa_Byte Date;						//����(��4λ)(1-7)	��(��4λ)(1-31)
	OpcUa_Byte Month;						//��		(1-12)
	OpcUa_Byte Year;						//��		(0-99)
}TIMESTAMP_7BYTE;
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
/////////////////////����103���ݱ���ASDU�ṹ////////////////////////
//ʱ��ͬ��֡ ASDU06
typedef struct {
	ASDU_ID				ASDU_id;		//ASDU��ʶ��
	INFOBODY_ID			InfoBody_id;	//��Ϣ���ʶ��
	TIMESTAMP_7BYTE		TimeStamp;		//��Ϣ��ʱ��
}TIME_SYN;

// �ܲ�ѯ(���ٻ�)������ ASDU07(����ֹ ASDU08)
typedef struct {
	ASDU_ID				ASDU_id;		//ASDU��ʶ��
	INFOBODY_ID			InfoBody_id;	//��Ϣ���ʶ��
	OpcUa_Byte				ScanNO;			//ɨ�����
}GENERAL_INQUIRE;

//һ������ ASDU20
typedef struct {
	ASDU_ID				ASDU_id;		//ASDU��ʶ��
	INFOBODY_ID			InfoBody_id;	//��Ϣ���ʶ��
	OpcUa_Byte				DCO;			//˫����
	OpcUa_Byte				RII;			//������Ϣ��ʶ��
}GENRRAL_CMD;

//�Ŷ����ݴ�������� ASDU24
typedef struct {
	ASDU_ID				ASDU_id;		//ASDU��ʶ��
	INFOBODY_ID			InfoBody_id;	//��Ϣ���ʶ��
	OpcUa_Byte				TOO;			//��������
	OpcUa_Byte				TOV;			//�Ŷ�ֵ������
	WORD				FAN;			//�������
	OpcUa_Byte				ACC;			//ʵ��ͨ�����
}DDS_CMD;

////////////////////////�����Ǽ��ݱ���ASDU///////////////////////////
/////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
/*****************110kvGISͨѶ�źŶ������(ͨ��)���ĵ�ASDU****************/
//����ң�š�ң������ ASDU21: NOG��Ϊ0
/*
typedef struct {
	ASDU_ID			ASDU_id;		//ASDU��ʶ��
	INFOBODY_ID		InfoBody_id;	//��Ϣ���ʶ��
	OpcUa_Byte			RII;			//������Ϣ��ʶ��,����վ�����е�RIIһ��
	OpcUa_Byte			NOG;			//ͨ�÷����ʶ��Ŀ
	WORD			GIN;			//ͨ�÷����ʶ���:��ţ���Ŀ��
	OpcUa_Byte			KOD;			//�������
}READ_REQUEST;
*/
typedef struct {
	OpcUa_Byte	Type;	//ASDU��ʶ��
	OpcUa_Byte	VSQ;				
	OpcUa_Byte	COT;			
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
	OpcUa_Byte    RII;	//������Ϣ��ʶ��,����վ�����е�RIIһ��
	OpcUa_Byte    NOG;	//ͨ�÷����ʶ��Ŀ=0. ���ȡ����Ŀ¼���������ٻ�
	WORD	GIN;			//ͨ�÷����ʶ���:��ţ���Ŀ��
	OpcUa_Byte	KOD;			//�������
}READ_REQUEST;

typedef struct {
	OpcUa_Byte	Type;	//ASDU��ʶ��
	OpcUa_Byte	VSQ;				
	OpcUa_Byte	COT;			
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
}SFRAMEBODY;

//����ң�š�ң������ ASDU21: NOGΪ0
typedef struct {
	OpcUa_Byte	Type;	//ASDU��ʶ��
	OpcUa_Byte	VSQ;				
	OpcUa_Byte	COT;			
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
	OpcUa_Byte    RII;	//������Ϣ��ʶ��,����վ�����е�RIIһ��
	OpcUa_Byte    NGD;	//ͨ�÷����ʶ��Ŀ=0. ���ȡ����Ŀ¼���������ٻ�
}READ_REQUESToneNGD;

/*
��ȡ���Ϊ35H��ȫ��ֵ������ 
����: 68 0D 0D 68 73 01 15 81 2A 01 FE F1 49 01 35 00 01 A4 16 
( 
 ��·��ַ: 1      ASDU 21      COT: 42  ͨ�÷�������� 
 Ӧ�÷������ݵ�Ԫ������ַ: 1 
 ��������(FUN): 254      ��Ϣ���(INF): 241  ��һ�����ȫ����Ŀ��ֵ������ 
 ������Ϣ��ʶ(RII): 73      ͨ�÷����ʶ��Ŀ(NOG): 1 
 ���: 53      ʵ��ֵ 
 ) 
*/
//ASDU21
typedef struct {
	OpcUa_Byte	Type;	//ASDU��ʶ��
	OpcUa_Byte	VSQ;				
	OpcUa_Byte	COT;			
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
	OpcUa_Byte    RII;	//������Ϣ��ʶ��,����վ�����е�RIIһ��
	OpcUa_Byte    NGD;	//ͨ�÷����ʶ��Ŀ=1


	GIN			sGIN;		//ͨ�÷����ʶ���:��ţ���Ŀ��
	OpcUa_Byte		KOD;		//�������
}ASDU21_1NGD;

//ң������ ASDU10
typedef struct {
	READ_REQUEST	RequestHead;
	OpcUa_Byte			GDD_2PointInfo;
	OpcUa_Byte			GDD_DateWide;
	OpcUa_Byte			GDD_Num;
	OpcUa_Byte			GID;
}WRITE_REQUEST;
/////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
/*FT1.2 �ɱ�֡��֡�����м�������ɣ�
�̶����ȵı���ͷ(4BYTE)
L����λλ�� := �����򣫵�ַ����·�û�����
֡У���(CS)
�����ַ�(16H)
*/
//FT1.2 �ɱ�֡�ı���ͷ
/*  Start := 68H					 */
/*  DataLen := ��·�û����ݵĳ���+2  */
typedef struct {
	OpcUa_Byte Start;						//�����ַ�
	OpcUa_Byte DataLen;					//�����򣫵�ַ����·�û����ݵĳ���
	OpcUa_Byte DataLenRepeat;				//ͬDataLen
	OpcUa_Byte StartRepeat;				//��Start
	union{
		CONTROL_FIELD sControl;		
		OpcUa_Byte Control;
	};								//������
	OpcUa_Byte Addr;						//��ַ��
}FT1_2_HEAD;

//FT1.2 �ɱ�֡�ı���β
/*	End := 16H	*/
typedef struct {
	OpcUa_Byte Check_Sum;					//У���
	OpcUa_Byte End;						//�����ַ�
}FT1_2_TAIL;
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
/*FT1.2 �̶�֡��֡�����м�������ɣ�
�����ַ� := 10
������(C)
��ַ��(A)
֡У���(CS)
�����ַ�(16H)
*/
typedef struct {
	OpcUa_Byte Start;						//�����ַ�
	union{
		CONTROL_FIELD sControl;		
		OpcUa_Byte Control;
	};								//������
	OpcUa_Byte Addr;						//��ַ��
	OpcUa_Byte Check_Sum;					//֡У���
	OpcUa_Byte End;						//�����ַ�
}FT1_2_FIXED_FRAME;
///////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
/************************����·���ϴ����֡��ʽ*********************/
//ASDU6,ʱ��ͬ��֡
typedef struct {
	FT1_2_HEAD		TSFrameHead;	//��·�㱨��ͷ
	TIME_SYN		TSFrameBody;
	FT1_2_TAIL		TSFrameTail;	//��·�㱨��β
} TSFRAME;

//ASDU7 �ܲ�ѯ��ʼ
typedef struct {
	FT1_2_HEAD			GIFrameHead;	//��·�㱨��ͷ
	GENERAL_INQUIRE		GIFrameBody;
	FT1_2_TAIL			GIFrameTail;	//��·�㱨��β
} GIFRAME;

//ASDU20 һ������
typedef struct {
	FT1_2_HEAD		GCFrameHead;	//��·�㱨��ͷ
	GENRRAL_CMD		GCFrameBody;
	FT1_2_TAIL		GCFrameTail;	//��·�㱨��β
} GCFRAME;

//ASDU21 ͨ�÷��ࣨ��������
typedef struct {
	FT1_2_HEAD					GSCMDFrameHead;	//��·�㱨��ͷ
	READ_REQUEST				GSCMDFrameBody;
	FT1_2_TAIL					GSCMDFrameTail;	//��·�㱨��β
} GSREADCMDFRAME;

//ASDU21 ͨ�÷��ࣨ��������L ���ٻ�
typedef struct {
	FT1_2_HEAD					GSCMDFrameHead;	//��·�㱨��ͷ
	READ_REQUESToneNGD			GSCMDFrameBody;
	FT1_2_TAIL					GSCMDFrameTail;	//��·�㱨��β
} ASDU21_WHOLECALL;

//ASDU21 ͨ�÷��ࣨ���������һ��
typedef struct {
	FT1_2_HEAD					Head;	//��·�㱨��ͷ
	ASDU21_1NGD					Body;
	FT1_2_TAIL					Tail;	//��·�㱨��β
} ASDU21_1NGDFRAME;

//ASDU10 ͨ�÷��ࣨд������
typedef struct {
	FT1_2_HEAD					GSCMDFrameHead;	//��·�㱨��ͷ
	WRITE_REQUEST				GSCMDFrameBody;
	FT1_2_TAIL					GSCMDFrameTail;	//��·�㱨��β
} GSWRITECMDFRAME;

//ASDU23 �Ŷ����ݴ��������
typedef struct {
	FT1_2_HEAD					DDTFrameHead;	//��·�㱨��ͷ
	DDS_CMD						DDTFrameBody;	
	FT1_2_TAIL					DDTFrameTail;	//��·�㱨��β
} DDTCMDFRAME;

//ASDU140������
typedef struct {
	FT1_2_HEAD					GSCMDFrameHead;	//��·�㱨��ͷ
	OpcUa_Byte	Type;	//ASDU��ʶ��
	OpcUa_Byte	VSQ;	//0x81			
	OpcUa_Byte	COT;	//20		
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
	FT1_2_TAIL					GSCMDFrameTail;	//��·�㱨��β
} ASDU140FRAME;

//ASDU232д����
typedef struct {
	FT1_2_HEAD					GSCMDFrameHead;	//��·�㱨��ͷ
	OpcUa_Byte	Type;	//ASDU��ʶ��
	OpcUa_Byte	VSQ;	//0x81			
	OpcUa_Byte	COT;	//20		
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
	OpcUa_Byte    Val;	//дֵ
	OpcUa_Byte    RII;	//0
	FT1_2_TAIL					GSCMDFrameTail;	//��·�㱨��β
} ASDU232FRAME;
/////////////////////////////////////////////////////////////////////////

//ASDU38
typedef struct  
{
	VTI sVTI;
	QDS Quality;
}VTI_QDS;
typedef struct  
{
	OpcUa_Byte Info;
	VTI sVTI;
	QDS Quality;
}VTI_QDS_INFO;

//ASDU39
typedef struct  
{
	VTI sVTI;
	QDS Quality;
	CP32Time2a Time;
}VTI_QDStm;
typedef struct  
{
	OpcUa_Byte Info;
	VTI sVTI;
	QDS Quality;
	CP32Time2a Time;
}VTI_QDStm_INFO;

//ASDU41
typedef struct  
{
	SINGLE_POINT SIQ;
	CP32Time2a Time;
}SIQ_TIME;
typedef struct  
{
	OpcUa_Byte Info;
	SINGLE_POINT SIQ;
	CP32Time2a Time;
}SIQ_TIME_INFO;

//ASDU43
typedef struct  
{
	DOUBLE_POINT DIQ;
	CP32Time2a Time;
}DIQ_TIME;
typedef struct  
{
	OpcUa_Byte Info;
	DOUBLE_POINT DIQ;
	CP32Time2a Time;
}DIQ_TIME_INFO;

//ASDU44
typedef struct 
{
	WORD DISet;
	WORD Change;
	QDS Quality;
}SCD_QDS;
typedef struct 
{
	OpcUa_Byte Info;
	WORD DISet;
	WORD Change;
	QDS Quality;
}SCD_QDS_INFO;

//ASDU50
typedef struct  
{
	OpcUa_Byte Info;
	OpcUa_Int16 OV:1;
	OpcUa_Int16 ER:1;
	OpcUa_Int16 Res:1;
	OpcUa_Int16 Value:13;
}MEA_AI_INFO;

//ASDU72
typedef struct  
{
	DWORD BitString32;
	QDS Quality;
	CP32Time2a Time;
}ASDU72;

typedef struct  
{
	OpcUa_Byte Info;
	DWORD BitString32;
	QDS Quality;
	CP32Time2a Time;
}ASDU72_INFO;

//SIEMENS �豸�������ֵ
typedef struct {
	OpcUa_Int32 Value:29;
	OpcUa_Int32 Res1:1;
	OpcUa_Int32 InternalError:1;
	OpcUa_Int32 Res2:1;
}sMV;

//ASDU205
typedef struct  
{
	sMV MV;
	CP32Time2a Time;
}ASDU205;

////////////////////////////ASDU10�ĵ���ͨ�����ݽṹ/////////////////////////////////////////////
//ASDU10��ͨ�����ݽṹ����ͷ��
typedef struct {
	GIN		sGIN;				
	OpcUa_Byte	KOD;	
}GDATAPointNoData;

typedef struct {
	GIN		sGIN;				
	OpcUa_Byte	KOD;	
	GDD		sGDD;
}GDATAPointHead;

//3 =  �޷���������UI ��
typedef struct {
	GDATAPointHead sHead;
	OpcUa_Byte  Data;
}DATAPoint_DT3_UI1;

typedef struct {
	GDATAPointHead sHead;
	WORD  Data;
}DATAPoint_DT3_UI2;

typedef struct {
	GDATAPointHead sHead;
	DWORD  Data;
}DATAPoint_DT3_UI4;


//6 =  ��������F�� ��ע�⣺����׼�����͵���������Ϊ 6 �ĸ�����Ҳ��IEEE ��׼754 ��ʵ���� 
//7 = IEEE ��׼754 ��ʵ����R32.23�� 
typedef struct {
	GDATAPointHead sHead;
	OpcUa_Float  Data;
}DATAPoint_DT6,DATAPoint_DT7;

//9 =  ˫����Ϣ����B.6.5
//10 = ������Ϣ 
typedef struct {
	GDATAPointHead sHead;
	OpcUa_Byte  Data;
}DATAPoint_DT9,DATAPoint_DT10;

//12:��Ʒ�������ı���ֵ  ��B.6.8
typedef struct {
	GDATAPointHead sHead;
	MEA_AI  Data;
}DATAPoint_DT12;

/*
18 = ��ʱ��ı��� 
= CP48{˫����Ϣ��DPI�������ã�RES�����ĸ� 8 λλ��ʱ�䣨TIME���� 
������Ϣ��SIN��} 
����   ˫����Ϣ��DPI�� = UI2[0 ��1]<0 ��3>����B.6.5 
���ã�RES��= BS6[2��7]<0> 
�ĸ�8 λλ��ʱ�䣨TIME��= CP32Time2a[8��39]    ��B.6.28 
������Ϣ��SIN�� = UI8[40��47]<0��3>����B.6.23
*/
typedef struct {
	GDATAPointHead sHead;
	CP48  Data;
}DATAPoint_DT18;


/*
203 =  �����ֽ�ʱ��ı��� 
= CP72{˫����Ϣ��DPI�������ã�RES�����߸� 8 λλ��ʱ�䣨TIME���� 
������Ϣ��SIN��} 
����   ˫����Ϣ��DPI�� = UI2[0 ��1]<0 ��3>����B.6.5 
���ã�RES��= BS6[2��7]<0> 
�߸�8 λλ��ʱ�䣨TIME��= CP56Time2a[8��63]    ��B.6.29 
������Ϣ��SIN�� = UI8[64��71]<0��3>����B.6.23 
*/
typedef struct {
	GDATAPointHead sHead;
	CP72  Data;
}DATAPoint_DT203;


#pragma pack(pop)

#endif
