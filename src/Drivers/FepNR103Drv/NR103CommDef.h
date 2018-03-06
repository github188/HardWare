/**
* purpose: NR103驱动公共数据定义
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
//!标签类型定义
#define		NR103_DIG           1           //!遥信--Group:Entry
#define		NR103_AIG			2			//!遥测--Group:Entry
#define		NR103_DOB			3			//!遥控执行命令--Fun:Inf
#define		NR103_DOSB			4			//!遥控选择命令--Fun:Inf
#define		NR103_DICB          5           //!下发命令返校--Fun:Inf
#define		NR103_COT           6           //!无数据ASDU10应答COT
#define		NR103_RDSFV			7			//!ASDU21读取--Group:Entry
#define		NR103_RESET			8			//!信号复归命令
#define		NR103_RESETCB		9			//!远方复归命令返信

//!报文处理缓冲区
#define		MIN_FRAME_LEN		28					//!最小报文长度
#define		MAX_FRAME_LEN		1028				//!最大报文长度
#define		NR103_RCVBUFF		5*MAX_FRAME_LEN		//!接收缓冲区
#define		NR103_INNERBUF		10*MAX_FRAME_LEN	//!内部缓冲区
#define		NR103_SENDBUFF		268					//!发送缓冲区

//!报文类型标识
#define		TYP_COMM_DATA		10			//!通用分类数据
//!控制方向
#define		TYP_TIME_SYN		6			//!时间同步报文
#define		TYP_GENERAL_QUERY_START 7		//!总查询
#define		TYP_GENERAL_CMD		20			//!一般命令报文
#define		TYP_COMM_CMD		21			//!通用分类命令报文
#define		TYP_DISORDER_DT		24			//!扰动数据传输命令报文
#define		TYP_DISORDER_ACK	25			//!扰动数据认可命令报文
#define		TYP_CHAINDATA_REQ	203			//!联锁数据请求
#define		TYP_GENHISDATA_QUE	220			//!通用历史数据查询
#define		TYP_FDIR_QUE		221			//!文件目录召唤
#define		TYP_FDIR_RES		222			//!文件目录响应
#define		TYP_FDIR_RED		223			//!文件准备就绪
#define		TYP_NODE_RED		224			//!节准备就绪
#define		TYP_SECTION_SND		225			//!段的发送帧
#define		TYP_LSLF_REP		226			//!最后的段或最后的节表示帧
#define		TYP_FILE_ACK		227			//!文件或节确认帧
#define		TYP_RCTL_CHECK		228			//!遥控校验命令
//!监视方向
#define		TYP_ASDU1			1
#define		TYP_IDENTIFIER		5			//!标识报文类型
#define		TYP_COMM_IDENT		11			//!通用分类标识
#define		TYP_DISORDER_DATA	23			//!扰动数据报文
#define		TYP_DISORDER_DREADY 26			//!扰动数据传输准备就绪
#define		TYP_CHANNEL_READY	27			//!通道数据传输真被就绪
#define		TYP_STATUS_READY	28			//!带标志状态变位传输准备就绪
#define		TYP_STATUS_TRANS	29			//!带标志状态变位传输报文
#define		TYP_DISORDER_TRANS	30			//!扰动值传输报文
#define		TYP_TRANS_END		31			//!传送结束报文
#define		TYP_CHAINDATA_TRANS 204			//!连锁数据传送
#define		TYP_RCTLCHECK_RES	229			//!遥控校验结果

//!可变结构限定词
#define		VSQ_MOST			129		

//!传送原因
#define COT_AUTO					1		//自发（突发）
#define COT_CYC						2		//循环
#define COT_FCB						3		//复位帧计数位
#define COT_CU						4		//复位通信单元
#define COT_START					5		//启动
#define COT_RESTART					5		//重新启动
#define COT_POWERON					6		//电源合上
#define COT_TEST					7		//测试
#define COT_TIME_SYN				8		//时间同步---------------双向
#define COT_GENERAL_QUERY			9		//总查询（总召唤）-------双向
#define COT_GENERAL_QUERY_END		10		//总查询（总召唤）终止
#define COT_LOCAL_OP				11		//当地操作
#define COT_REMOTE_OP				12		//远方操作
#define COT_GENERAL_CMD_CONFIRM		20		//命令的肯定认可---------双向
#define COT_GENERAL_CMD_DENY		21		//命令的否定认可
#define COT_DISORDER_DATA_TRANS		31		//扰动数据的传输---------双向
#define COT_COMM_WRITE_CMD			40		//通用分类写命令
#define COT_COMM_READ_CMD			42		//通用分类读命令
#define COT_COMM_WRITE_CMD_CONFIRM	40		//通用分类写命令的肯定认可-------------双向
#define COT_COMM_WRITE_CMD_DENY		41		//通用分类写命令的否定认可
#define COT_COMM_READ_CMD_VALID		42		//对通用分类读命令有效数据响应---------双向
#define COT_COMM_READ_CMD_INVALID	43		//对通用分类读命令无效数据响应
#define COT_COMM_WRITE_CONFIRM		44		//通用分类写确认

//!功能类型
#define FUN_NR103DISORDER			90		//NR103扰动数据传输
#define FUN_DISTANCE_PRO			128		//距离保护
#define FUN_CURRENT_PRO				160		//过流保护
#define FUN_TRANSFORMER_DIF_PRO		176		//变压器差动保护
#define FUN_LINE_DIF_PRO			192		//线路差动保护
#define FUN_COMM_SORT				254		//通用分类功能
#define FUN_GLB						255		//全局功能类型

//!信息序号
#define INF_NOUSE						0		//未使用
#define INF_TIME_SYN_OR_QUERY_BEGIN		0		//时间同步或总查询（总召唤）的启动
#define INF_FCB							2		//复位帧计数位
#define INF_CU							3		//复位通信单元
#define INF_START_OR_RESTART			4		//启动/重新启动---------仅监视上行
#define INF_POWERON						5		//电源合上--------------仅监视上行
#define INF_REST						19		//NR103远方复归命令INF
#define	INF_READ_TEAM_TITLE				240		//读组标题
#define	INF_READ_ALL_ITEM				241		//读一个组的全部条目的值或属性
#define	INF_READ_SINGLE_CATALOG			243		//读单个条目的目录
#define	INF_READ_SINGLE_ITEM			244		//读单个条目的值或属性
#define INF_COMM_DATA_GENERAL_QUERY		245		//对通用分类数据总查询
#define INF_WRITE						248		//写条目
#define INF_WRITE_WITH_CONFIRM			249		//带确认的写条目
#define INF_WRITE_WITH_EXCUTE			250		//带执行的写条目
#define INF_WRITE_WITH_CANCEL			251		//写条目终止

//!命令类型
#define TOO_FAULT_SELCET				1		//故障的选择
#define TOO_START_DISDATA				2		//请求扰动数据
#define TOO_PAUSE_DISDATA				3		//中止扰动数据
#define TOO_CHANEL_REQUSET				8		//通道的请求
#define TOO_CHANEL_PAUSE				9		//通道的中止
#define TOO_START_STACHANGE_WITHSIG		16		//请求带标志的状态变位
#define TOO_PAUSE_STACHANGE_WITHSIG		17		//中止带标志的状态变位

#define	TOO_REQ_DIS_TABLE				24		//请求被记录的扰动表
#define	TOO_DISDATA_END_WITHOUTPAUSE	32		//不带中止的扰动数据传输的结束
#define	TOO_DISDATA_END_WITHCONPAUSE	33		//由控制系统所中止的扰动数据传输的结束
#define	TOO_DISDATA_END_WITHDEVPAUSE	34		//由继电保护设备所中止的扰动数据传输的结束
#define	TOO_CHANEL_END_WITHOUTPAUSE		35		//不带中止的通道传输的结束
#define	TOO_CHANEL_END_WITHCONPAUSE		36		//由控制系统所中止的通道传输的结束
#define	TOO_CHANEL_END_WITHDEVPAUSE		37		//由继电保护设备所中止的通道传输的结束
#define	TOO_ALTER_END_WITHOUTPAUSE		38		//不带中止的带标志的状态变位传输的结束
#define	TOO_ALTER_END_WITHCONPAUSE		39		//由控制系统所中止的带标志的状态变位传输的结束
#define	TOO_ALTER_END_WITHDEVPAUSE		40		//由继电保护设备所中止的带标志的状态变位传输的结束

#define TOO_DISDATA_SUCCESS				64		//扰动数据传输成功
#define TOO_DISDATA_FAIL				65		//扰动数据传输不成功
#define TOO_CHANEL_SUCCESS				66		//通道传输成功
#define TOO_CHANEL_FAIL					67		//通道传输不成功
#define TOO_STATUSCHANGED_SUCCESS		68		//带标志的状态变位传输成功
#define TOO_STATUSCHANGED_FAIL			69		//带标志的状态变位传输不成功

//!扰动值类型
#define TOV_UNUSED						0		//未用
#define TOV_SHUNSHI_VALUE				1		//瞬时值
#define TOV_FUZHI_VALUE				   201		//幅值

//!实际通道序号
#define	ACC_NO_CHANEL					0		//无通道传输时使用
#define	ACC_I_L1						1		//IL1
#define	ACC_I_L2						2		//IL2
#define	ACC_I_L3						3		//IL3
#define ACC_I_N							4		//IN
#define ACC_U_L1E						5		//UL1E
#define ACC_U_L2E						6		//UL2E
#define ACC_U_L3E						7		//UL3E
#define ACC_U_EN						8		//UEN

//!通用分类数据描述GDD----DataType
#define GDD_DT0_NODATA			0	//无数据
#define GDD_DT1_OS8ASCII		1	//ASCII 8 位码
#define GDD_DT2_BS1				2	//成组8 位串
#define GDD_DT3_UI				3	//无符号整数
#define GDD_DT4_I				4	//整数
#define GDD_DT5_UFLOAT			5	//无符号浮点数
#define GDD_DT6_FLOAT			6	//浮点数（F）:也是IEEE 标准754 短实数
#define GDD_DT7_FLOAT			7	//IEEE 标准754 短实数（R32.23）
#define GDD_DT8_DOUBLE			8	//IEEE 标准754 实数（R64.53）
#define GDD_DT9_DPI				9	//双点信息，见B.6.5
#define GDD_DT10_SPI			10	//单点信息
#define GDD_DT11_DPI			11	//带瞬变和差错的双点信息:0= 瞬变（TRANSIENT ） 1= 开（OFF ） 2= 合（ON）3= 出错（ERROR）
#define GDD_DT12_AITime			12	//带品质描述的被测值  见B.6.8
#define GDD_DT18_DPITime		18	//带时标的报文:= CP48{双点信息（DPI），备用（RES），四个 8 位位组时间（TIME）， 附加信息（SIN）} 
#define GDD_DT19_DPITime		19	//带相对时间的时标报文:= CP48{双点信息（DPI），备用（RES），相对时间, 电网故障序号, 四个 8 位位组时间（TIME）， 附加信息（SIN）} 
#define GDD_DT22_ResponseCode	22	//通用分类回答码. 见7.2.6.36
#define GDD_DT24_Index			24	//索引
#define GDD_DT203_CP72			203	//带七字节时标的报文:= CP72{双点信息（DPI），备用（RES），七个 8 位位组时间（TIME），附加信息（SIN）} 
#define GDD_DT204_CP104			204	//带相对时间的七字节时标的报文:= CP72{双点信息（DPI），备用（RES），相对时间, 电网故障序号, 七个 8 位位组时间（TIME），附加信息（SIN）} 

//!描述类别KOD
#define KOD0_NONE				0
#define KOD1_ACTUALVALUE		1	//实际值 
#define KOD2_DEFAULTVALUE		2	//缺省值 
#define KOD3_RANGE				3	//量程（最小值，最大值，步长） 
#define KOD4_NOUSE				4	//未用 
#define KOD5_CORRECTDEGREE		5	//准确度 
#define KOD6_FACTOR				6	//因子
#define KOD7_RATIO				7	//%参比 
#define KOD8_LIST				8	//列表 
#define KOD9_DIMESION			9	//量纲 
#define KOD10_DISCRIPTION		10	//描述 
#define KOD11_NOUSE				11	//未用 
#define KOD12_PASSWORD			12	//口令条目 
#define KOD13_READONLY			13	//只读 
#define KOD14_WRITEONLY			14	//只写 
#define KOD15_NOUSE				15	//未用 
#define KOD16_NOUSE				16	//未用 
#define KOD17_NOUSE				17	//未用 
#define KOD18_NOUSE				18	//未用 
#define KOD19_FUN_INFO			19	//相应的功能类型和信息序号 
#define KOD20_EVENT				20	//相应的事件 
#define KOD21_TEXTMATRIX		21	//列表的文本阵列
#define KOD22_VALUEMATRIX		22	//列表的值阵列 
#define KOD23_RELEVANTENTRY		23	//相关联的条目 

//!RII
#define	RII_CALL_IDENTIFIER				1

//!报文标识字符
#define HEAD_FLAG				0xEB90
#define RESERVE_FLAG			0XFFFF

//!设备类型
#define DT_SC1				1	//站控节点类型1
#define DT_SC2				2	//站控节点类型2
#define DT_PROMGR			3	//保护管理类型
#define DT_ROUT				40	//路由设备
#define DT_PRO				41	//保护设备
#define DT_MEACTL			42	//测控设备
#define DT_PROMEACTL		43	//保护测控设备
#define DT_FAULTREC			44	//故障录播设备
#define DT_MEASURE			45	//计量设备
#define DT_PREERRCAL		46	//防误校验设备
#define DT_OTHERS			49	//其他

//!设备网络状态
#define DNS_S_AB				0x0050	//单机主A,B网正常

//!链路地址列表长度
#define NETADDRS_LEN			4	

#define RCV_FLAG				0		//接收报文
#define SND_FLAG				1		//发送报文

#define F_CONTINUE				0		//进入下次循环
#define F_BREAK					1		//跳出循环
#define F_SUCCESS				2		//解析到合法报文
#define F_NOTHINGTD				-1		//无需动作

#define MIN_103ASDU_LEN			6		//103ASDU数据最小长度
#define MIN_IEC103_COMM_LEN		11		//最小通用分类上行ASDU长度
#define MAX_NR103_FRAM_LEN		1000	//NR103最大ASDU长度

#define FieldPoint_Init			0x02		//FieldPoint的初始状态

#define PERIOD_TIME				1000		//设备状态维护定时器周期

//!报文结构
typedef struct
{
	WORD wFirstFlag;			//报文标识
	DWORD dwLength;				//报文长度
	WORD wSecondFlag;			//报文标识
	WORD wSourceFacAddr;		//源厂站地址
	WORD wSourceDevAddr;		//源设备地址
	WORD wDesFacAddr;			//目标厂站地址
	WORD wDesDevAddr;			//目标设备地址
	WORD wDataNumber;			//数据编号
	WORD wDevType;				//设备类型
	WORD wDevNetState;			//设备网络状态
	WORD wFRouterAddr;			//首级路由地址
	WORD wLRouterAddr;			//末级路由地址
	WORD wReserve1;				//保留字节
}FrameAPCI,FrameHBeat;

//组号、条目号
typedef struct  
{
	OpcUa_Byte byGroup;
	OpcUa_Byte byEntry;
}GIN;

//通用分类数据描述
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

//带品质描述被测值
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

//7字节时间信息
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

//4字节时间信息
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
	WORD	RET;			//相对时间
	WORD	NOF;			//电网故障序号
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
	WORD	RET;			//相对时间
	WORD	NOF;			//电网故障序号
	CP56Time2a Time;
	OpcUa_Byte    SIN;
}CP104;

typedef struct  
{
	OpcUa_Byte byType;			//类型标识
	OpcUa_Byte byVSQ;			//可变结构结构限定词
	OpcUa_Byte byCOT;			//传送原因
	OpcUa_Byte byAddr;			//ASDU地址
	OpcUa_Byte byFun;			//功能类型
	OpcUa_Byte byInf;			//信息序号
}ASDU_HEAD;

typedef struct 
{
	ASDU_HEAD	sASDUHead;
	OpcUa_Byte  byRII;
	OpcUa_Byte  byNGD:6;
	OpcUa_Byte  byCOUNT:1;
	OpcUa_Byte	byCNTU:1;
}ASDU10_HEAD;

//时间同步帧
typedef struct
{
	FrameAPCI sFrameAPCI;
	ASDU_HEAD sASUDHead;
	CP56Time2a sTimeStamp;
}ASDU6Frame;

//ASDU7总查询帧
typedef struct
{
	FrameAPCI sFrameAPCI;
	ASDU_HEAD sASDUHead;
	OpcUa_Byte bySCN;
}ASDU7Frame;

//标识报文:ASDU5
typedef struct
{
	FrameAPCI sFrameAPCI;
	ASDU_HEAD sASDUHead;
	OpcUa_Byte byCOL;			//兼容级别
	OpcUa_CharA Info[12];
}ASDU5Frame;

//ASDU21总查询报文
typedef struct
{
	FrameAPCI sFrameAPCI;
	ASDU10_HEAD sASDU21WQ;
}ASDU21WQFrame;

//ASDU21定值组查询
typedef struct  
{
	FrameAPCI sFrameAPCI;	
	ASDU10_HEAD sASDU21H;
	GIN sGIN;					//通用分类标识序号:组号+条目号
	OpcUa_Byte byKOD;		
}ASDU21ReadGroup;

//ASDU10通用数据解决结构
typedef struct{
	FrameAPCI sFrameAPCI;
	ASDU10_HEAD sASDU10H;
	GIN sGIN;
	OpcUa_Byte byKOD;
	GDD sGDD;
	OpcUa_Byte byGID;
}ASDU10_1NDG,ASDU10_Reset,ASDU10_RemoteCtrl;

//ASDU20远方复归命令
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

//无符号整数
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

//浮点数
typedef struct{
	GDATAPointHead sHead;
	OpcUa_Float fData;
}DATAPoint_DT6,DATAPoint_DT7;

//单点、双点信息
typedef struct{
	GDATAPointHead sHead;
	OpcUa_Byte byData;
}DATAPoint_DT9,DATAPoint_DT10;

//带品质描述被测值
typedef struct
{
	GDATAPointHead sHead;
	MEA_AI data;
}DATAPoint_DT12;

//带4字节时标报文
typedef struct
{
	GDATAPointHead sHead;
	CP48 Data;
}DataPoint_DT18;

//带7字节时标报文
typedef struct
{
	GDATAPointHead sHead;
	CP72 Data;
}DataPoint_DT203;

//扰动数据传输的命令 ASDU23的数据集
typedef struct {
	OpcUa_Byte		FaultNoLB;			//故障序号低字节
	OpcUa_Byte		FaultNoHB;			//故障序号高字节
	OpcUa_Byte		SOF;				//故障状态
	CP56Time2a		Time;
}ASDU23DataSet;

//扰动数据传输的命令 ASDU28
typedef struct {
	ASDU_HEAD		sHead;
	OpcUa_Byte		Rev1;
	OpcUa_Byte		Rev2;
	OpcUa_Byte		TOV;		//扰动值类型: <1>瞬时值  <201> 幅值  见B.6.27
	OpcUa_Byte		FaultNoLB;	//故障序号低字节
	OpcUa_Byte		FaultNoHB;	//故障序号高字节
}ASDU28;

//扰动数据传输的命令 ASDU29
typedef struct {
	ASDU_HEAD		sHead;
	OpcUa_Byte		FaultNoLB;	//故障序号低字节
	OpcUa_Byte		FaultNoHB;	//故障序号高字节
	OpcUa_Byte		NOT;		//带标志的状态变位的数目
}ASDU29Head;
//扰动数据传输的命令 ASDU29的数据集
typedef struct {
	WORD			TAP;		//标志的位置
	OpcUa_Byte		Fun;		//功能类型
	OpcUa_Byte		Inf;		//信息序号
	OpcUa_Byte		DPI:2;
	OpcUa_Byte		Rev:6;		//0
}ASDU29DataSet;

//扰动数据传输的命令 ASDU31
typedef struct {
	ASDU_HEAD		sHead;
	OpcUa_Byte		Rev1;
	OpcUa_Byte		TOO;		//命令类型
	OpcUa_Byte		FaultNoLB;	//故障序号低字节
	OpcUa_Byte		FaultNoHB;	//故障序号高字节
	OpcUa_Byte		ACC;		//实际通道序号
}ASDU31;

//NR103响应远方复归的ASDU1
typedef struct{
	ASDU_HEAD sHead;
	OpcUa_Byte byDPI;
	CP32Time2a sTime;
	OpcUa_Byte bySIN;
}ASDU1_REST;

typedef struct DAILYTIME {
	OpcUa_Int32 lYear;				//! 年
	OpcUa_Int16 sMonth;				//! 月
	OpcUa_Int16 sDay;				//! 日
	OpcUa_Int16 sHour;				//! 时
	OpcUa_Int16 sMinute;			//! 分
	OpcUa_Int16 sSecond;			//! 秒
	OpcUa_UInt16 sMSecond;			//! 毫秒

	//!	时区（为0则是格林威治时间，东时区为负，西时区为正，例如北京时间为-8）
	OpcUa_Int16 sTimeZone;

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

#pragma pack(pop)
}
#endif