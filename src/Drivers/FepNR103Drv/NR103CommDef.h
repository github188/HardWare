/**
* purpose: NR103�����������ݶ���
* 
**/
#ifndef _NR103COMMDEF_H_
#define _NR103COMMDEF_H_

#include "RTDB/Server/DBDriverFrame/IODriverH.h"

#ifdef FEPNR103DRV_EXPORTS
#define FEPNR103Drv_API MACS_DLL_EXPORT
#else
#define FEPNR103Drv_API MACS_DLL_IMPORT
#endif

#pragma pack(push, 1)

namespace MACS_SCADA_SUD
{
//!��ǩ���Ͷ���
#define		NR103_DIG           1           //!ң��--Group:Entry
#define		NR103_AIG			2			//!ң��--Group:Entry
#define		NR103_DOB			3			//!ң��ִ������--Fun:Inf
#define		NR103_DOSB			4			//!ң��ѡ������--Fun:Inf
#define		NR103_DICB          5           //!�·����У--Fun:Inf
#define		NR103_COT           6           //!������ASDU10Ӧ��COT
#define		NR103_RDSFV			7			//!ASDU21��ȡ--Group:Entry
#define		NR103_RESET			8			//!�źŸ�������
#define		NR103_RESETCB		9			//!Զ�����������

//!���Ĵ�������
#define		MIN_FRAME_LEN		28					//!��С���ĳ���
#define		MAX_FRAME_LEN		1028				//!����ĳ���
#define		NR103_RCVBUFF		5*MAX_FRAME_LEN		//!���ջ�����
#define		NR103_INNERBUF		10*MAX_FRAME_LEN	//!�ڲ�������
#define		NR103_SENDBUFF		268					//!���ͻ�����

//!�������ͱ�ʶ
#define		TYP_COMM_DATA		10			//!ͨ�÷�������
//!���Ʒ���
#define		TYP_TIME_SYN		6			//!ʱ��ͬ������
#define		TYP_GENERAL_QUERY_START 7		//!�ܲ�ѯ
#define		TYP_GENERAL_CMD		20			//!һ�������
#define		TYP_COMM_CMD		21			//!ͨ�÷��������
#define		TYP_DISORDER_DT		24			//!�Ŷ����ݴ��������
#define		TYP_DISORDER_ACK	25			//!�Ŷ������Ͽ������
#define		TYP_CHAINDATA_REQ	203			//!������������
#define		TYP_GENHISDATA_QUE	220			//!ͨ����ʷ���ݲ�ѯ
#define		TYP_FDIR_QUE		221			//!�ļ�Ŀ¼�ٻ�
#define		TYP_FDIR_RES		222			//!�ļ�Ŀ¼��Ӧ
#define		TYP_FDIR_RED		223			//!�ļ�׼������
#define		TYP_NODE_RED		224			//!��׼������
#define		TYP_SECTION_SND		225			//!�εķ���֡
#define		TYP_LSLF_REP		226			//!���Ķλ����Ľڱ�ʾ֡
#define		TYP_FILE_ACK		227			//!�ļ����ȷ��֡
#define		TYP_RCTL_CHECK		228			//!ң��У������
//!���ӷ���
#define		TYP_ASDU1			1
#define		TYP_IDENTIFIER		5			//!��ʶ��������
#define		TYP_COMM_IDENT		11			//!ͨ�÷����ʶ
#define		TYP_DISORDER_DATA	23			//!�Ŷ����ݱ���
#define		TYP_DISORDER_DREADY 26			//!�Ŷ����ݴ���׼������
#define		TYP_CHANNEL_READY	27			//!ͨ�����ݴ����汻����
#define		TYP_STATUS_READY	28			//!����־״̬��λ����׼������
#define		TYP_STATUS_TRANS	29			//!����־״̬��λ���䱨��
#define		TYP_DISORDER_TRANS	30			//!�Ŷ�ֵ���䱨��
#define		TYP_TRANS_END		31			//!���ͽ�������
#define		TYP_CHAINDATA_TRANS 204			//!�������ݴ���
#define		TYP_RCTLCHECK_RES	229			//!ң��У����

//!�ɱ�ṹ�޶���
#define		VSQ_MOST			129		

//!����ԭ��
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

//!��������
#define FUN_NR103DISORDER			90		//NR103�Ŷ����ݴ���
#define FUN_DISTANCE_PRO			128		//���뱣��
#define FUN_CURRENT_PRO				160		//��������
#define FUN_TRANSFORMER_DIF_PRO		176		//��ѹ�������
#define FUN_LINE_DIF_PRO			192		//��·�����
#define FUN_COMM_SORT				254		//ͨ�÷��๦��
#define FUN_GLB						255		//ȫ�ֹ�������

//!��Ϣ���
#define INF_NOUSE						0		//δʹ��
#define INF_TIME_SYN_OR_QUERY_BEGIN		0		//ʱ��ͬ�����ܲ�ѯ�����ٻ���������
#define INF_FCB							2		//��λ֡����λ
#define INF_CU							3		//��λͨ�ŵ�Ԫ
#define INF_START_OR_RESTART			4		//����/��������---------����������
#define INF_POWERON						5		//��Դ����--------------����������
#define INF_REST						19		//NR103Զ����������INF
#define	INF_READ_TEAM_TITLE				240		//�������
#define	INF_READ_ALL_ITEM				241		//��һ�����ȫ����Ŀ��ֵ������
#define	INF_READ_SINGLE_CATALOG			243		//��������Ŀ��Ŀ¼
#define	INF_READ_SINGLE_ITEM			244		//��������Ŀ��ֵ������
#define INF_COMM_DATA_GENERAL_QUERY		245		//��ͨ�÷��������ܲ�ѯ
#define INF_WRITE						248		//д��Ŀ
#define INF_WRITE_WITH_CONFIRM			249		//��ȷ�ϵ�д��Ŀ
#define INF_WRITE_WITH_EXCUTE			250		//��ִ�е�д��Ŀ
#define INF_WRITE_WITH_CANCEL			251		//д��Ŀ��ֹ

//!��������
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

//!�Ŷ�ֵ����
#define TOV_UNUSED						0		//δ��
#define TOV_SHUNSHI_VALUE				1		//˲ʱֵ
#define TOV_FUZHI_VALUE				   201		//��ֵ

//!ʵ��ͨ�����
#define	ACC_NO_CHANEL					0		//��ͨ������ʱʹ��
#define	ACC_I_L1						1		//IL1
#define	ACC_I_L2						2		//IL2
#define	ACC_I_L3						3		//IL3
#define ACC_I_N							4		//IN
#define ACC_U_L1E						5		//UL1E
#define ACC_U_L2E						6		//UL2E
#define ACC_U_L3E						7		//UL3E
#define ACC_U_EN						8		//UEN

//!ͨ�÷�����������GDD----DataType
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

//!�������KOD
#define KOD0_NONE				0
#define KOD1_ACTUALVALUE		1	//ʵ��ֵ 
#define KOD2_DEFAULTVALUE		2	//ȱʡֵ 
#define KOD3_RANGE				3	//���̣���Сֵ�����ֵ�������� 
#define KOD4_NOUSE				4	//δ�� 
#define KOD5_CORRECTDEGREE		5	//׼ȷ�� 
#define KOD6_FACTOR				6	//����
#define KOD7_RATIO				7	//%�α� 
#define KOD8_LIST				8	//�б� 
#define KOD9_DIMESION			9	//���� 
#define KOD10_DISCRIPTION		10	//���� 
#define KOD11_NOUSE				11	//δ�� 
#define KOD12_PASSWORD			12	//������Ŀ 
#define KOD13_READONLY			13	//ֻ�� 
#define KOD14_WRITEONLY			14	//ֻд 
#define KOD15_NOUSE				15	//δ�� 
#define KOD16_NOUSE				16	//δ�� 
#define KOD17_NOUSE				17	//δ�� 
#define KOD18_NOUSE				18	//δ�� 
#define KOD19_FUN_INFO			19	//��Ӧ�Ĺ������ͺ���Ϣ��� 
#define KOD20_EVENT				20	//��Ӧ���¼� 
#define KOD21_TEXTMATRIX		21	//�б���ı�����
#define KOD22_VALUEMATRIX		22	//�б��ֵ���� 
#define KOD23_RELEVANTENTRY		23	//���������Ŀ 

//!RII
#define	RII_CALL_IDENTIFIER				1

//!���ı�ʶ�ַ�
#define HEAD_FLAG				0xEB90
#define RESERVE_FLAG			0XFFFF

//!�豸����
#define DT_SC1				1	//վ�ؽڵ�����1
#define DT_SC2				2	//վ�ؽڵ�����2
#define DT_PROMGR			3	//������������
#define DT_ROUT				40	//·���豸
#define DT_PRO				41	//�����豸
#define DT_MEACTL			42	//����豸
#define DT_PROMEACTL		43	//��������豸
#define DT_FAULTREC			44	//����¼���豸
#define DT_MEASURE			45	//�����豸
#define DT_PREERRCAL		46	//����У���豸
#define DT_OTHERS			49	//����

//!�豸����״̬
#define DNS_S_AB				0x0050	//������A,B������

//!��·��ַ�б���
#define NETADDRS_LEN			4	

#define RCV_FLAG				0		//���ձ���
#define SND_FLAG				1		//���ͱ���

#define F_CONTINUE				0		//�����´�ѭ��
#define F_BREAK					1		//����ѭ��
#define F_SUCCESS				2		//�������Ϸ�����
#define F_NOTHINGTD				-1		//���趯��

#define MIN_103ASDU_LEN			6		//103ASDU������С����
#define MIN_IEC103_COMM_LEN		11		//��Сͨ�÷�������ASDU����
#define MAX_NR103_FRAM_LEN		1000	//NR103���ASDU����

#define FieldPoint_Init			0x02		//FieldPoint�ĳ�ʼ״̬

#define PERIOD_TIME				1000		//�豸״̬ά����ʱ������

//!���Ľṹ
typedef struct
{
	WORD wFirstFlag;			//���ı�ʶ
	DWORD dwLength;				//���ĳ���
	WORD wSecondFlag;			//���ı�ʶ
	WORD wSourceFacAddr;		//Դ��վ��ַ
	WORD wSourceDevAddr;		//Դ�豸��ַ
	WORD wDesFacAddr;			//Ŀ�곧վ��ַ
	WORD wDesDevAddr;			//Ŀ���豸��ַ
	WORD wDataNumber;			//���ݱ��
	WORD wDevType;				//�豸����
	WORD wDevNetState;			//�豸����״̬
	WORD wFRouterAddr;			//�׼�·�ɵ�ַ
	WORD wLRouterAddr;			//ĩ��·�ɵ�ַ
	WORD wReserve1;				//�����ֽ�
}FrameAPCI,FrameHBeat;

//��š���Ŀ��
typedef struct  
{
	OpcUa_Byte byGroup;
	OpcUa_Byte byEntry;
}GIN;

//ͨ�÷�����������
typedef struct
{
	OpcUa_Byte byDataType;
	OpcUa_Byte byDataSize;
	OpcUa_Byte byDataNum:7;
	OpcUa_Byte byCont:1;
}GDD;

typedef struct 
{
	OpcUa_Byte	bySPI:1;	
	OpcUa_Byte  byRES:3;	
	OpcUa_Byte	bySB:1;				
	OpcUa_Byte	byBL:1;			
	OpcUa_Byte	byNT:1;		
	OpcUa_Byte	byIV:1;		
}SINGLE_POINT;

typedef struct 
{
	OpcUa_Byte	byInfo;
	OpcUa_Byte	bySPI:1;	
	OpcUa_Byte  byRES:3;	
	OpcUa_Byte	bySB:1;				
	OpcUa_Byte	byBL:1;			
	OpcUa_Byte	byNT:1;		
	OpcUa_Byte	byIV:1;		
}SINGLE_POINT_INFO;

//��Ʒ����������ֵ
typedef struct  
{
	OpcUa_Int16 Value:13;
	OpcUa_Int16 Res:1;
	OpcUa_Int16 ER:1;
	OpcUa_Int16 OV:1;
}MEA_AI;

typedef struct 
{
	OpcUa_Byte	byDPI:2;	
	OpcUa_Byte  byRES:2;	
	OpcUa_Byte	bySB:1;				
	OpcUa_Byte	byBL:1;			
	OpcUa_Byte	byNT:1;		
	OpcUa_Byte	byIV:1;		
}DOUBLE_POINT;

typedef struct 
{
	OpcUa_Byte	Info;
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte  RES:2;	
	OpcUa_Byte	SB:1;				
	OpcUa_Byte	BL:1;			
	OpcUa_Byte	NT:1;		
	OpcUa_Byte	IV:1;		
}DOUBLE_POINT_INFO;

//7�ֽ�ʱ����Ϣ
typedef struct
{
	OpcUa_Byte LowMillSec;
	OpcUa_Byte HighMillSec;
	OpcUa_Byte Minute:6;
	OpcUa_Byte Res1:1;
	OpcUa_Byte IV:1;
	OpcUa_Byte Hour:5;
	OpcUa_Byte Res2:2;
	OpcUa_Byte SU:1;
	OpcUa_Byte Day:5;
	OpcUa_Byte Week:3;
	OpcUa_Byte Mon:4;
	OpcUa_Byte Res3:4;
	OpcUa_Byte Year:7;
	OpcUa_Byte Res4:1;
}CP56Time2a;

//4�ֽ�ʱ����Ϣ
typedef struct
{
	OpcUa_Byte LowMillSec;
	OpcUa_Byte HighMillSec;
	OpcUa_Byte Minute:6;
	OpcUa_Byte Res1:1;
	OpcUa_Byte IV:1;
	OpcUa_Byte Hour:5;
	OpcUa_Byte Res2:2;
	OpcUa_Byte SU:1;
}CP32Time2a;

typedef struct  
{
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte	RES:6;
	CP32Time2a	Time;
	OpcUa_Byte	SIN;
}CP48;

typedef struct  
{
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte    RES:6;
	WORD	RET;			//���ʱ��
	WORD	NOF;			//�����������
	CP32Time2a Time;
	OpcUa_Byte    SIN;
}CP80;

typedef struct  
{
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte  RES:6;
	CP56Time2a	Time;
	OpcUa_Byte  SIN;
}CP72;

typedef struct  
{
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte    RES:6;
	WORD	RET;			//���ʱ��
	WORD	NOF;			//�����������
	CP56Time2a Time;
	OpcUa_Byte    SIN;
}CP104;

typedef struct  
{
	OpcUa_Byte byType;			//���ͱ�ʶ
	OpcUa_Byte byVSQ;			//�ɱ�ṹ�ṹ�޶���
	OpcUa_Byte byCOT;			//����ԭ��
	OpcUa_Byte byAddr;			//ASDU��ַ
	OpcUa_Byte byFun;			//��������
	OpcUa_Byte byInf;			//��Ϣ���
}ASDU_HEAD;

typedef struct 
{
	ASDU_HEAD	sASDUHead;
	OpcUa_Byte  byRII;
	OpcUa_Byte  byNGD:6;
	OpcUa_Byte  byCOUNT:1;
	OpcUa_Byte	byCNTU:1;
}ASDU10_HEAD;

//ʱ��ͬ��֡
typedef struct
{
	FrameAPCI sFrameAPCI;
	ASDU_HEAD sASUDHead;
	CP56Time2a sTimeStamp;
}ASDU6Frame;

//ASDU7�ܲ�ѯ֡
typedef struct
{
	FrameAPCI sFrameAPCI;
	ASDU_HEAD sASDUHead;
	OpcUa_Byte bySCN;
}ASDU7Frame;

//��ʶ����:ASDU5
typedef struct
{
	FrameAPCI sFrameAPCI;
	ASDU_HEAD sASDUHead;
	OpcUa_Byte byCOL;			//���ݼ���
	OpcUa_CharA Info[12];
}ASDU5Frame;

//ASDU21�ܲ�ѯ����
typedef struct
{
	FrameAPCI sFrameAPCI;
	ASDU10_HEAD sASDU21WQ;
}ASDU21WQFrame;

//ASDU21��ֵ���ѯ
typedef struct  
{
	FrameAPCI sFrameAPCI;	
	ASDU10_HEAD sASDU21H;
	GIN sGIN;					//ͨ�÷����ʶ���:���+��Ŀ��
	OpcUa_Byte byKOD;		
}ASDU21ReadGroup;

//ASDU10ͨ�����ݽ���ṹ
typedef struct{
	FrameAPCI sFrameAPCI;
	ASDU10_HEAD sASDU10H;
	GIN sGIN;
	OpcUa_Byte byKOD;
	GDD sGDD;
	OpcUa_Byte byGID;
}ASDU10_1NDG,ASDU10_Reset,ASDU10_RemoteCtrl;

//ASDU20Զ����������
typedef struct {
	FrameAPCI sFrameAPCI;
	ASDU_HEAD sHead;
	OpcUa_Byte byDCO;
	OpcUa_Byte byRII;
}ASDU20_Rest;

typedef struct 
{
	ASDU10_HEAD sHead;
	GIN			Gin;			
	OpcUa_Byte	KOD;			
}ASDU10NoDataSet;

typedef struct{
	GIN sGIN;
	OpcUa_Byte byKOD;
}GDATAPointNoData;

typedef struct{
	GIN sGIN;
	OpcUa_Byte byKOD;
	GDD sGDD;
}GDATAPointHead;

//�޷�������
typedef struct{
	GDATAPointHead sHead;
	OpcUa_Byte byData;
}DATAPoint_DT3_UI1;

typedef struct{
	GDATAPointHead sHead;
	WORD wData;
}DataPoint_DT3_UI2;

typedef struct{
	GDATAPointHead sHead;
	DWORD dwData;
}DATAPoint_DT3_UI4;

//������
typedef struct{
	GDATAPointHead sHead;
	OpcUa_Float fData;
}DATAPoint_DT6,DATAPoint_DT7;

//���㡢˫����Ϣ
typedef struct{
	GDATAPointHead sHead;
	OpcUa_Byte byData;
}DATAPoint_DT9,DATAPoint_DT10;

//��Ʒ����������ֵ
typedef struct
{
	GDATAPointHead sHead;
	MEA_AI data;
}DATAPoint_DT12;

//��4�ֽ�ʱ�걨��
typedef struct
{
	GDATAPointHead sHead;
	CP48 Data;
}DataPoint_DT18;

//��7�ֽ�ʱ�걨��
typedef struct
{
	GDATAPointHead sHead;
	CP72 Data;
}DataPoint_DT203;

//�Ŷ����ݴ�������� ASDU23�����ݼ�
typedef struct {
	OpcUa_Byte		FaultNoLB;			//������ŵ��ֽ�
	OpcUa_Byte		FaultNoHB;			//������Ÿ��ֽ�
	OpcUa_Byte		SOF;				//����״̬
	CP56Time2a		Time;
}ASDU23DataSet;

//�Ŷ����ݴ�������� ASDU28
typedef struct {
	ASDU_HEAD		sHead;
	OpcUa_Byte		Rev1;
	OpcUa_Byte		Rev2;
	OpcUa_Byte		TOV;		//�Ŷ�ֵ����: <1>˲ʱֵ  <201> ��ֵ  ��B.6.27
	OpcUa_Byte		FaultNoLB;	//������ŵ��ֽ�
	OpcUa_Byte		FaultNoHB;	//������Ÿ��ֽ�
}ASDU28;

//�Ŷ����ݴ�������� ASDU29
typedef struct {
	ASDU_HEAD		sHead;
	OpcUa_Byte		FaultNoLB;	//������ŵ��ֽ�
	OpcUa_Byte		FaultNoHB;	//������Ÿ��ֽ�
	OpcUa_Byte		NOT;		//����־��״̬��λ����Ŀ
}ASDU29Head;
//�Ŷ����ݴ�������� ASDU29�����ݼ�
typedef struct {
	WORD			TAP;		//��־��λ��
	OpcUa_Byte		Fun;		//��������
	OpcUa_Byte		Inf;		//��Ϣ���
	OpcUa_Byte		DPI:2;
	OpcUa_Byte		Rev:6;		//0
}ASDU29DataSet;

//�Ŷ����ݴ�������� ASDU31
typedef struct {
	ASDU_HEAD		sHead;
	OpcUa_Byte		Rev1;
	OpcUa_Byte		TOO;		//��������
	OpcUa_Byte		FaultNoLB;	//������ŵ��ֽ�
	OpcUa_Byte		FaultNoHB;	//������Ÿ��ֽ�
	OpcUa_Byte		ACC;		//ʵ��ͨ�����
}ASDU31;

//NR103��ӦԶ�������ASDU1
typedef struct{
	ASDU_HEAD sHead;
	OpcUa_Byte byDPI;
	CP32Time2a sTime;
	OpcUa_Byte bySIN;
}ASDU1_REST;

typedef struct DAILYTIME {
	OpcUa_Int32 lYear;				//! ��
	OpcUa_Int16 sMonth;				//! ��
	OpcUa_Int16 sDay;				//! ��
	OpcUa_Int16 sHour;				//! ʱ
	OpcUa_Int16 sMinute;			//! ��
	OpcUa_Int16 sSecond;			//! ��
	OpcUa_UInt16 sMSecond;			//! ����

	//!	ʱ����Ϊ0���Ǹ�������ʱ�䣬��ʱ��Ϊ������ʱ��Ϊ�������籱��ʱ��Ϊ-8��
	OpcUa_Int16 sTimeZone;

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

#pragma pack(pop)
}
#endif