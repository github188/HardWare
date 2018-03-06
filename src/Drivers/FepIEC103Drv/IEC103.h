/*!
*	\file	IEC103.h
*
*	\brief	103主站驱动用到的数据类型定义文件
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

#define PERIOD_SCAN_MS						1000				//! 周期定时器调度周期，毫秒。

/*
TEMPLATE						UNIT_TYPE	RAW_TYPE	BIT_WIDTH	LOW	HIGH	COMMENT

DI%U(0,1,1024)						0	0	1	0	0	遥信量
AI%U(0,1,20)						1	2	32	0	0	遥测量
DO%U(0,1,255)						2	0	1	0	0	遥控量
DOS%U(0,1,255)						3	0	1	0	0	遥控选择量
DIC%U(0,1,255)						4	0	1	0	0	遥控返信量
DIS%U(0,1,255)						5	0	1	0	0	选择返信量

DIG%U(0,0,255)%*1000:%+U(0,0,255)	6	0	1	0	0	遥信--Group:Index
AIG%U(0,0,255)%*1000:%+U(0,0,255)	7	2	32	0	0	遥测--Group:Index
DIB%U(0,0,255)%*1000:%+U(0,0,255)	8	0	1	0	0	遥信--Fun:Inf
AIB%U(0,0,255)%*1000:%+U(0,0,255)	9	2	32	0	0	遥测--Fun:Inf
DOB%U(0,0,255)%*1000:%+U(0,0,255)	10	0	1	0	0	遥控执行命令--Fun:Inf
DOSB%U(0,0,255)%*1000:%+U(0,0,255)	11	0	1	0	0	遥控选择命令--Fun:Inf
DICB%U(0,0,255)%*1000:%+U(0,0,255)	12	0	1	0	0	下发命令返校--Fun:In
COT%U(0,0,255)%*1000:%+U(0,0,255)	13	8	8	0	0	无数据ASDU10应答的COT
RdFV%U(0,0,255)%*1000:%+U(0,0,255)	14	0	1	0	0	读定值--Group:Index
WrFV%U(0,0,255)%*1000:%+U(0,0,255)	15	8	8	0	0	写定值--Group:Index
*/


//下列值对应MacsDIS103.dbf文件中的UnitType项的值
#define MacsDIS103_DI		0			//遥信	        
#define MacsDIS103_AI		1			//遥测	          
#define MacsDIS103_DO		2			//遥控
#define MacsDIS103_DOS		3			//遥控确认点
#define MacsDIS103_DIC		4			//遥控返校点
#define MacsDIS103_DIS		5			//选择返校点

//Add by hxw in 2012.9.5
#define MacsDIS103_DIG		6			//遥信--Group:Entry
#define MacsDIS103_AIG		7			//遥测--Group:Entry
#define MacsDIS103_DIB		8			//遥信--Fun:Inf
#define MacsDIS103_AIB		9			//遥测--Fun:Inf
#define MacsDIS103_DOB		10			//遥控执行命令--Fun:Inf
#define MacsDIS103_DOSB		11			//遥控选择命令--Fun:Inf
#define MacsDIS103_DICB		12			//下发命令返校--Fun:Inf或Group:Entry
#define MacsDIS103_COT		13			//无数据ASDU10应答的COT
#define MacsDIS103_RdFV		14			//读定值--Group:Entry: ASDU140
#define MacsDIS103_WrFV		15			//写定值--Group:Entry: ASDU232
#define MacsDIS103_ASDU21RD	16			//ASDU21读取--Group:Entry。注意，Entry=0表示读取整组

//////////////////////////////////////报文类型标识//////////////////////////////////
//上下行报文
#define TYP_COMM_DATA				10		//通用分类数据报文类型标识

//下行报文
#define TYP_TIME_SYN				6		//时间同步报文类型标识
#define TYP_GENERAL_QUERY_START		7		//总查询（总召唤）启动报文类型标识
#define TYP_GENERAL_CMD				20		//一般命令报文类型标识
#define TYP_COMM_CMD				21		//通用分类命令报文类型标识
#define TYP_DISORDER_DATA_TRANS		24		//扰动数据传输的命令报文类型标识
#define TYP_DISORDER_DATA_ACK		25		//扰动数据传输的认可报文类型标识
#define TYP_ASDU140					140		//读施耐德保护定值组报文类型标识
#define TYP_ASDU232					232		//写施耐德保护定值组报文类型标识

//上行报文
#define TYP_WITH_TIME				1		//带时标的报文
#define TYP_WITH_TINE_REL			2		//带相对时间的报文
#define TYP_VALUE1					3		//被测值I
#define TYP_VALUE_WITH_TIME			4		//带相对时间的有时标的被测值报文
#define TYP_IDENTIFIER				5		//标识报文类型标识
#define TYP_GENERAL_QUERY_END		8		//总查询（总召唤）终止报文类型标识
#define TYP_VALUE2					9		//被测值II
#define TYP_COMM_IDENTIFIER			11		//通用分类标识报文类型标识
#define TYP_ASDU17					17		//施耐德保护定值
#define TYP_DISORDER_DATA			23		//被记录的扰动表报文类型标识
#define TYP_DISORDER_DATA_READY		26		//扰动数据传输准备就绪报文类型标识
#define TYP_CHANNEL_READY			27		//被记录的通道传输准备就绪报文类型标识
#define TYP_STATUS_ALTER_READY		28		//带标志的状态变位传输准备就绪报文类型标识
#define TYP_STATUS_ALTER_TRANS		29		//传送带标志的状态变位报文类型标识
#define TYP_DISORDER_TRANS			30		//传输扰动值报文类型标识
#define TYP_TRANS_END				31		//传送结束报文类型标识
#define TYP_ASDU171					171		//施耐德保护有功电度及无功电度

////////////////////////////////////可变结构限定词///////////////////////////////////
#define VSQ_MOST	129		//绝大多数报文的该项值都为81(除了“被记录的扰动表”)


////////////////////////////////////传送原因（除标注为“双向”外，其余均仅用于监视上行方向）///////////////////////////////////
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


/////////////////////////////////功能类型/////////////////////////////////////
#define FUN_DISTANCE_PRO			128		//距离保护
#define FUN_CURRENT_PRO				160		//过流保护
#define FUN_TRANSFORMER_DIF_PRO		176		//变压器差动保护
#define FUN_LINE_DIF_PRO			192		//线路差动保护
#define FUN_COMM_SORT				254		//通用分类功能
#define FUN_GLB						255		//全局功能类型



//////////////////////////////////信息序号（除标注为“仅监视上行”外，其余均可用于下行及上行方向）////////////////////////////////////
//240 ～255 为通用分类功能
#define INF_NOUSE						0		//未使用
#define INF_TIME_SYN_OR_QUERY_BEGIN		0		//时间同步或总查询（总召唤）的启动
#define INF_FCB							2		//复位帧计数位
#define INF_CU							3		//复位通信单元
#define INF_START_OR_RESTART			4		//启动/重新启动---------仅监视上行
#define INF_POWERON						5		//电源合上--------------仅监视上行
#define	INF_READ_TEAM_TITLE				240		//读组标题
#define	INF_READ_ALL_ITEM				241		//读一个组的全部条目的值或属性
#define	INF_READ_SINGLE_CATALOG			243		//读单个条目的目录
#define	INF_READ_SINGLE_ITEM			244		//读单个条目的值或属性
#define INF_COMM_DATA_GENERAL_QUERY		245		//对通用分类数据总查询
#define INF_WRITE						248		//写条目
#define INF_WRITE_WITH_CONFIRM			249		//带确认的写条目
#define INF_WRITE_WITH_EXCUTE			250		//带执行的写条目
#define INF_WRITE_WITH_CANCEL			251		//写条目终止


/////////////////////////////////命令类型/////////////////////////////////////
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


/////////////////////////////////扰动值类型///////////////////////////////////
#define TOV_UNUSED						0		//未用
#define TOV_SHUNSHI_VALUE				1		//瞬时值
#define TOV_FUZHI_VALUE				   201		//幅值

/////////////////////////////////实际通道序号/////////////////////////////////
#define	ACC_NO_CHANEL					0		//无通道传输时使用
#define	ACC_I_L1						1		//IL1
#define	ACC_I_L2						2		//IL2
#define	ACC_I_L3						3		//IL3
#define ACC_I_N							4		//IN
#define ACC_U_L1E						5		//UL1E
#define ACC_U_L2E						6		//UL2E
#define ACC_U_L3E						7		//UL3E
#define ACC_U_EN						8		//UEN

/////////////////////////////////通用分类数据描述GDD----DataType//////////////////////////////
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

//描述类别KOD
#define KOD0_NONE				0
#define KOD1_ACTUALVALUE		1 //  实际值 
#define KOD2_DEFAULTVALUE		2 //  缺省值 
#define KOD3_RANGE				3 //  量程（最小值，最大值，步长） 
#define KOD4_NOUSE				4 //  未用 
#define KOD5_CORRECTDEGREE		5 //  准确度 
#define KOD6_FACTOR				6 //  因子
#define KOD7_RATIO				7 // % 参比 
#define KOD8_LIST				8 //  列表 
#define KOD9_DIMESION			9 //  量纲 
#define KOD10_DISCRIPTION		10 // 描述 
#define KOD11_NOUSE				11 // 未用 
#define KOD12_PASSWORD			12 // 口令条目 
#define KOD13_READONLY			13 // 只读 
#define KOD14_WRITEONLY			14 // 只写 
#define KOD15_NOUSE				15 // 未用 
#define KOD16_NOUSE				16 // 未用 
#define KOD17_NOUSE				17 // 未用 
#define KOD18_NOUSE				18 // 未用 
#define KOD19_FUN_INFO			19 // 相应的功能类型和信息序号 
#define KOD20_EVENT				20 // 相应的事件 
#define KOD21_TEXTMATRIX		21 // 列表的文本阵列
#define KOD22_VALUEMATRIX		22 // 列表的值阵列 
#define KOD23_RELEVANTENTRY		23 // 相关联的条目 

/////////////////////////////////RII/////////////////////////////////////
#define	RII_CALL_IDENTIFIER				1

//////////////////////////与FT1.2相关的//////////////////////////////////
#define GUDINGZHEN_START_ID					0x10
#define BIANCHANGZHEN_START_ID				0x68
#define GUDINGZHEN_END_ID					0x16
#define BIANCHANGZHEN_END_ID				0x16
#define GUDINGZHEN_SIZE						5

/*=======================================================
//CONTROL_FIELD含义说明:
控制域分“主 . 从”和“从 . 主”两种情况。

（1）	“主 .  从”报文的控制域

D7       D6       D5       D4       D3    D2    D1     D0
备用     PRM     FCB      FCV                功能码
0/1      1	

（A）	PRM（启动报文位）表明信息传输方向，PRM=1由主站至子站；PRM=0由子站至主站。
（B）	FCB（桢记数位）。FCB = 0 / 1——主站每向从站发送新一轮的“发送/确认”或“请求/响应”传输服务时，将FCB取反。主站为每个从站保存一个FCB的拷贝，若超时未收到应答，则主站重发，重发报文的FCB保持不变，重发次数最多不超过3次。若重发3次后仍未收到预期应答，则结束本轮传输服务。
（C）	FCV （桢记数有效位），FCV= 0表明 FCB的变化无效，FCV=1表明 FCB的变化有效。发送/无回答服务、广播报文不考虑报文丢失和重复传输，无需改变FCB状态，这些桢FCV常为0
（D）	功能码定义（主---从方向）
功能码	 帧类型	        功能描述	        FCV状态
0	     发送/确认帧	复位通信单元        0
3	     发送/确认帧	传送数据	        1
4	     发送/无回答帧	传送数据	        0
7	     复位帧计数位	传送数据	        0
9	     请求/响应帧	召唤链路状态        0
10	     请求/响应帧	召唤1级数据	        1
11（b）	 请求/响应帧	召唤2级数据（遥测）	1
注：功能码等于3、4的报文为可变帧长报文，其余报文均为固定帧长报文。

(2) “从 . 主”报文的控制域

D7     D6    D5    D4     D3   D2   D1    D0
备用   PRM  ACD  DFC           功能码
0       0       
(A)	ACD（要求访问位）。ACD = 1，通知主站，从站有I级数据请求传送。
(B)	DFC（数据流控制位）。 DFC = 0表示从站可以接受数据，DFC ＝1表示从站缓冲区已满，无法接受新数据。
(C)	功能码（从---主方向）
功能码	帧类型	    功能
0	    确认帧	    确认
1	    确认帧	    链路忙，未收到报文
8	    响应帧	    以数据包响应请求帧
9	    响应帧	    从站没有所召唤的数据
11	    响应帧	    从站以链路状态响应主站请求
注：功能码等于8的报文为可变帧长报文，其余报文均为固定帧长报文。
============================================================*/


#pragma pack(push, 1)
typedef struct 
{
	OpcUa_Byte	FunCode:4;	//功能码.见下面宏定义	
	OpcUa_Byte	FCV_DFC:1;	//帧记数有效位.	
	OpcUa_Byte	FCB_ACD:1;	//帧记数位.
	OpcUa_Byte	Prm:1;		//信息传输方向.PRM=1由主站至子站；PRM=0由子站至主站
	OpcUa_Byte	Rev:1;		//保留,0或1.
}CONTROL_FIELD;	//控制字段

//控制系统向继电保护设备(或测控设备)传输的FT1.2帧中功能码的定义
#define FRAME_FUNC_RESET_CU					0			//复位通信单元(CU)，用于发送/确认帧	
#define FRAME_FUNC_SEND_DATA				3			//传送数据，用于发送/确认帧	
#define FRAME_FUNC_SEND_DATA2				4			//传送数据，用于发送/无回答帧	
#define FRAME_FUNC_SEND_DATA3				7			//传送数据，用于复位帧计数位(FCB)
#define FRAME_FUNC_INQUIRE_LINK_STATUS		9			//召唤链路状态，用于请求/响应帧
#define FRAME_FUNC_INQUIRE_LEVEL1_DATA		10			//召唤1级用户数据，用于请求/响应帧
#define FRAME_FUNC_INQUIRE_LEVEL2_DATA		11			//召唤2级用户数据，用于请求/响应帧	
//继电保护设备(或测控设备)向控制系统传输的FT1.2帧中功能码的定义
#define FRAME_FUNC_CONFIRM					0			//确认帧,确认
#define FRAME_FUNC_DENY						1			//确认帧,链路忙，未收到报文
#define FRAME_FUNC_RET_DATA					8			//响应帧,以数据响应请求帧
#define FRAME_FUNC_NO_DATA					9			//响应帧,无所召唤的数据
#define	FRAME_FUNC_ACK_REQUEST				11			//响应帧,以链路状态或访问请求回答请求帧

//////////////////////////////////////////////////////////////
////////////////103协议报文ASDU的基本元素结构/////////////////
#define MAX_IEC103_USER_DATA_LEN		255	//标准为253，但实际可达255	
#define MAX_IEC103_GID_LEN				16
#define MAX_IEC103_FRAM_LEN				(MAX_IEC103_USER_DATA_LEN+6)
#define	MAX_IEC103_REVBUF_LEN			(MAX_IEC103_FRAM_LEN * 5)
#define MIN_IEC103G_GD_LEN				6	//单个通用分类数据的最小长度：DATAPointHead

#define	IEC103_START_CODE			0x68	
#define	IEC103_END_CODE				0x16
#define IEC103_SHORT_START_CODE		0x10	

#define IEC103_BROADCAST_ADDRESS	255
//类型标示
#define IEC103_SENDTIME_TYPE		6 

//传送原因
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

//! ASDU10帧头
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
带品质描述词的被测值=MEA(MVAL)

=BS1 1 0 1  
其中溢出位=OV
0 =无溢出 

1 =溢出 

2 0 1  
差错位=ER:=BS1
0 =被测值(MVAL)有效 

1 =被测值(MVAL)无效 

未用(常为
3 0 0 ) 
备用位=RES:=BS1
4 16 -1 1 2 12  
被测值=MVAL:=F13
若MVAL 为溢出时,它被分别置成正或负的最大值 而且当 OV =1 最大的 MVAL 可以是以是+/-1.2 或+/-2.4倍的额定值
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
带品质描述词的被测值=MEA(MVAL)

BIT15 =  溢出位（OV），0 =  无溢出，1=  溢出 
BIT14 =  差错位（ER），0 =被测值（MVAL）有效，1 =被测值（MVAL）无效 
BIT13 =  备用位（RES），通常为0 
BIT12~BIT0 =  被测值（MVAL）*/
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
故障的状态（SOF） 
故障的状态（SOF）=BS8{TP，TM，TEST，OTEV，RES} 
TP = BIT0 = 0    被记录的故障未跳闸 = 1   被记录的故障跳闸 
TM = BIT1 = 0   扰动数据等待传输 = 1   扰动数据正在传输 
TEST = BIT2 = 0   在正常操作时记录的扰动数据 = 1   在测试模式时记录的扰动数据 
OTEV = BIT3 = 0   由启动/ 检出故障触发被记录的扰动数据 = 1   由其他事件触发被记录的扰动数据 
RES = BIT4～BIT7 = 未用
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
	WORD	RET;	//相对时间
	WORD	NOF;	//电网故障序号
	CP32Time2a Time;
	OpcUa_Byte    SIN;
}CP80;

typedef struct  
{
	OpcUa_Byte	DPI:2;	
	OpcUa_Byte    RES:6;
	WORD	RET;	//相对时间
	WORD	NOF;	//电网故障序号
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
	OpcUa_Byte DCS:2;		//1--分; 2--合. 0, 3非法.
	OpcUa_Byte QU:4;		//0--被寻址的控制功能属性; 4--不闭锁重合闸跳闸命令;其余略.
	OpcUa_Byte ACT:1;		//1. SE=1,ACT=0, 遥控选择; 2. SE=0,ACT=0, 遥控执行;
	OpcUa_Byte SE:1;		//3. SE=1,ACT=1, 遥控撤销.
}DCC;



//数据单元标识符
typedef struct {
	OpcUa_Byte TypeID;					//类型标识
	OpcUa_Byte AlterStruct;				//可变结构限定词
	OpcUa_Byte TransReason;				//传输原因
	OpcUa_Byte CommonAddr;				//应用服务数据单元公共地址
}ASDU_ID;

//信息体标识符
typedef struct {
	OpcUa_Byte FuncType;					//功能类型
	OpcUa_Byte InfoSeqNO;					//信息序号
}INFOBODY_ID;

//信息体时标(4BYPE)-103规约中标准格式
typedef struct {
	WORD Millisecond;				//时标ms
	OpcUa_Byte Min;						//时标min
	OpcUa_Byte Hour;						//时标hour
}TIMESTAMP_4BYTE;

//信息体时标(7BYPE)-110kvGIS传输通讯规约中规定的一种时标格式
typedef struct {
	WORD Millisecond;				//时标ms    0~59999ms
	OpcUa_Byte Min;						//时标min	0~59min
	OpcUa_Byte Hour;						//时标hour	0~23h
	OpcUa_Byte Date;						//星期(高4位)(1-7)	日(低4位)(1-31)
	OpcUa_Byte Month;						//月		(1-12)
	OpcUa_Byte Year;						//年		(0-99)
}TIMESTAMP_7BYTE;
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
/////////////////////几种103兼容报文ASDU结构////////////////////////
//时间同步帧 ASDU06
typedef struct {
	ASDU_ID				ASDU_id;		//ASDU标识符
	INFOBODY_ID			InfoBody_id;	//信息体标识符
	TIMESTAMP_7BYTE		TimeStamp;		//信息体时标
}TIME_SYN;

// 总查询(总召唤)的启动 ASDU07(或终止 ASDU08)
typedef struct {
	ASDU_ID				ASDU_id;		//ASDU标识符
	INFOBODY_ID			InfoBody_id;	//信息体标识符
	OpcUa_Byte				ScanNO;			//扫描序号
}GENERAL_INQUIRE;

//一般命令 ASDU20
typedef struct {
	ASDU_ID				ASDU_id;		//ASDU标识符
	INFOBODY_ID			InfoBody_id;	//信息体标识符
	OpcUa_Byte				DCO;			//双命令
	OpcUa_Byte				RII;			//返回信息标识符
}GENRRAL_CMD;

//扰动数据传输的命令 ASDU24
typedef struct {
	ASDU_ID				ASDU_id;		//ASDU标识符
	INFOBODY_ID			InfoBody_id;	//信息体标识符
	OpcUa_Byte				TOO;			//命令类型
	OpcUa_Byte				TOV;			//扰动值的类型
	WORD				FAN;			//故障序号
	OpcUa_Byte				ACC;			//实际通道序号
}DDS_CMD;

////////////////////////以上是兼容报文ASDU///////////////////////////
/////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
/*****************110kvGIS通讯信号定义表中(通用)报文的ASDU****************/
//请求遥信、遥测数据 ASDU21: NOG不为0
/*
typedef struct {
	ASDU_ID			ASDU_id;		//ASDU标识符
	INFOBODY_ID		InfoBody_id;	//信息体标识符
	OpcUa_Byte			RII;			//返回信息标识符,与主站命令中的RII一致
	OpcUa_Byte			NOG;			//通用分类标识数目
	WORD			GIN;			//通用分类标识序号:组号＋条目号
	OpcUa_Byte			KOD;			//描述类别
}READ_REQUEST;
*/
typedef struct {
	OpcUa_Byte	Type;	//ASDU标识符
	OpcUa_Byte	VSQ;				
	OpcUa_Byte	COT;			
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
	OpcUa_Byte    RII;	//返回信息标识符,与主站命令中的RII一致
	OpcUa_Byte    NOG;	//通用分类标识数目=0. 如读取整个目录，用于总召唤
	WORD	GIN;			//通用分类标识序号:组号＋条目号
	OpcUa_Byte	KOD;			//描述类别
}READ_REQUEST;

typedef struct {
	OpcUa_Byte	Type;	//ASDU标识符
	OpcUa_Byte	VSQ;				
	OpcUa_Byte	COT;			
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
}SFRAMEBODY;

//请求遥信、遥测数据 ASDU21: NOG为0
typedef struct {
	OpcUa_Byte	Type;	//ASDU标识符
	OpcUa_Byte	VSQ;				
	OpcUa_Byte	COT;			
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
	OpcUa_Byte    RII;	//返回信息标识符,与主站命令中的RII一致
	OpcUa_Byte    NGD;	//通用分类标识数目=0. 如读取整个目录，用于总召唤
}READ_REQUESToneNGD;

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
*/
//ASDU21
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

//遥控命令 ASDU10
typedef struct {
	READ_REQUEST	RequestHead;
	OpcUa_Byte			GDD_2PointInfo;
	OpcUa_Byte			GDD_DateWide;
	OpcUa_Byte			GDD_Num;
	OpcUa_Byte			GID;
}WRITE_REQUEST;
/////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
/*FT1.2 可变帧长帧由下列几部分组成：
固定长度的报文头(4BYTE)
L个八位位组 := 控制域＋地址域＋链路用户数据
帧校验和(CS)
结束字符(16H)
*/
//FT1.2 可变帧的报文头
/*  Start := 68H					 */
/*  DataLen := 链路用户数据的长度+2  */
typedef struct {
	OpcUa_Byte Start;						//启动字符
	OpcUa_Byte DataLen;					//控制域＋地址域＋链路用户数据的长度
	OpcUa_Byte DataLenRepeat;				//同DataLen
	OpcUa_Byte StartRepeat;				//用Start
	union{
		CONTROL_FIELD sControl;		
		OpcUa_Byte Control;
	};								//控制域
	OpcUa_Byte Addr;						//地址域
}FT1_2_HEAD;

//FT1.2 可变帧的报文尾
/*	End := 16H	*/
typedef struct {
	OpcUa_Byte Check_Sum;					//校验和
	OpcUa_Byte End;						//结束字符
}FT1_2_TAIL;
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
/*FT1.2 固定帧长帧由下列几部分组成：
启动字符 := 10
控制域(C)
地址域(A)
帧校验和(CS)
结束字符(16H)
*/
typedef struct {
	OpcUa_Byte Start;						//启动字符
	union{
		CONTROL_FIELD sControl;		
		OpcUa_Byte Control;
	};								//控制域
	OpcUa_Byte Addr;						//地址域
	OpcUa_Byte Check_Sum;					//帧校验和
	OpcUa_Byte End;						//结束字符
}FT1_2_FIXED_FRAME;
///////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
/************************在链路层上传输的帧格式*********************/
//ASDU6,时间同步帧
typedef struct {
	FT1_2_HEAD		TSFrameHead;	//链路层报文头
	TIME_SYN		TSFrameBody;
	FT1_2_TAIL		TSFrameTail;	//链路层报文尾
} TSFRAME;

//ASDU7 总查询开始
typedef struct {
	FT1_2_HEAD			GIFrameHead;	//链路层报文头
	GENERAL_INQUIRE		GIFrameBody;
	FT1_2_TAIL			GIFrameTail;	//链路层报文尾
} GIFRAME;

//ASDU20 一般命令
typedef struct {
	FT1_2_HEAD		GCFrameHead;	//链路层报文头
	GENRRAL_CMD		GCFrameBody;
	FT1_2_TAIL		GCFrameTail;	//链路层报文尾
} GCFRAME;

//ASDU21 通用分类（读）命令
typedef struct {
	FT1_2_HEAD					GSCMDFrameHead;	//链路层报文头
	READ_REQUEST				GSCMDFrameBody;
	FT1_2_TAIL					GSCMDFrameTail;	//链路层报文尾
} GSREADCMDFRAME;

//ASDU21 通用分类（读）命令L 总召唤
typedef struct {
	FT1_2_HEAD					GSCMDFrameHead;	//链路层报文头
	READ_REQUESToneNGD			GSCMDFrameBody;
	FT1_2_TAIL					GSCMDFrameTail;	//链路层报文尾
} ASDU21_WHOLECALL;

//ASDU21 通用分类（读）命令，读一组
typedef struct {
	FT1_2_HEAD					Head;	//链路层报文头
	ASDU21_1NGD					Body;
	FT1_2_TAIL					Tail;	//链路层报文尾
} ASDU21_1NGDFRAME;

//ASDU10 通用分类（写）命令
typedef struct {
	FT1_2_HEAD					GSCMDFrameHead;	//链路层报文头
	WRITE_REQUEST				GSCMDFrameBody;
	FT1_2_TAIL					GSCMDFrameTail;	//链路层报文尾
} GSWRITECMDFRAME;

//ASDU23 扰动数据传输的命令
typedef struct {
	FT1_2_HEAD					DDTFrameHead;	//链路层报文头
	DDS_CMD						DDTFrameBody;	
	FT1_2_TAIL					DDTFrameTail;	//链路层报文尾
} DDTCMDFRAME;

//ASDU140读命令
typedef struct {
	FT1_2_HEAD					GSCMDFrameHead;	//链路层报文头
	OpcUa_Byte	Type;	//ASDU标识符
	OpcUa_Byte	VSQ;	//0x81			
	OpcUa_Byte	COT;	//20		
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
	FT1_2_TAIL					GSCMDFrameTail;	//链路层报文尾
} ASDU140FRAME;

//ASDU232写命令
typedef struct {
	FT1_2_HEAD					GSCMDFrameHead;	//链路层报文头
	OpcUa_Byte	Type;	//ASDU标识符
	OpcUa_Byte	VSQ;	//0x81			
	OpcUa_Byte	COT;	//20		
	OpcUa_Byte	Addr;
	OpcUa_Byte	Fun;
	OpcUa_Byte	Inf;
	OpcUa_Byte    Val;	//写值
	OpcUa_Byte    RII;	//0
	FT1_2_TAIL					GSCMDFrameTail;	//链路层报文尾
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

//SIEMENS 设备特殊测量值
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

////////////////////////////ASDU10的单点通用数据结构/////////////////////////////////////////////
//ASDU10的通用数据结构公用头部
typedef struct {
	GIN		sGIN;				
	OpcUa_Byte	KOD;	
}GDATAPointNoData;

typedef struct {
	GIN		sGIN;				
	OpcUa_Byte	KOD;	
	GDD		sGDD;
}GDATAPointHead;

//3 =  无符号整数（UI ）
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


//6 =  浮点数（F） （注意：本标准中上送的数据类型为 6 的浮点数也是IEEE 标准754 短实数） 
//7 = IEEE 标准754 短实数（R32.23） 
typedef struct {
	GDATAPointHead sHead;
	OpcUa_Float  Data;
}DATAPoint_DT6,DATAPoint_DT7;

//9 =  双点信息，见B.6.5
//10 = 单点信息 
typedef struct {
	GDATAPointHead sHead;
	OpcUa_Byte  Data;
}DATAPoint_DT9,DATAPoint_DT10;

//12:带品质描述的被测值  见B.6.8
typedef struct {
	GDATAPointHead sHead;
	MEA_AI  Data;
}DATAPoint_DT12;

/*
18 = 带时标的报文 
= CP48{双点信息（DPI），备用（RES），四个 8 位位组时间（TIME）， 
附加信息（SIN）} 
其中   双点信息（DPI） = UI2[0 ～1]<0 ～3>，见B.6.5 
备用（RES）= BS6[2～7]<0> 
四个8 位位组时间（TIME）= CP32Time2a[8～39]    见B.6.28 
附加信息（SIN） = UI8[40～47]<0～3>，见B.6.23
*/
typedef struct {
	GDATAPointHead sHead;
	CP48  Data;
}DATAPoint_DT18;


/*
203 =  带七字节时标的报文 
= CP72{双点信息（DPI），备用（RES），七个 8 位位组时间（TIME）， 
附加信息（SIN）} 
其中   双点信息（DPI） = UI2[0 ～1]<0 ～3>，见B.6.5 
备用（RES）= BS6[2～7]<0> 
七个8 位位组时间（TIME）= CP56Time2a[8～63]    见B.6.29 
附加信息（SIN） = UI8[64～71]<0～3>，见B.6.23 
*/
typedef struct {
	GDATAPointHead sHead;
	CP72  Data;
}DATAPoint_DT203;


#pragma pack(pop)

#endif
