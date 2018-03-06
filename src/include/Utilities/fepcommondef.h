/*!
 *	\file	fepcommondef.h
 *
 *	\brief	FEP公共数据定义
 *
 *
 *	\author lingling.li
 *
 *	\date	2014年4月15日	9:14
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef fepcommondef_h__
#define fepcommondef_h__

#include <map>
#include <string>
#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/nodebase.h"

#ifdef IONODEMANAGER_EXPORTS
# define IONODEMANAGER_API MACS_DLL_EXPORT
#else
# define IONODEMANAGER_API MACS_DLL_IMPORT
#endif

//! 名称长度
const int NAME_LENGTH = 33;
//! 遥控命令中数据的最大长度，为满足字符串类型和BTYE[]类型，故定义成1024
const int MAX_REMOTECMD_LEN = 2048;
//! 通道诊断中数据的最大长度
const int MAX_PACKETCMD_LEN = 1024;

//! 流式数据的最大长度
const int MAX_STREAMDATA_LEN = 2048*256;
//! 驱动调度中最大命令长度
const int MAX_SCHEDULECMD_LEN = 2048;
//!调试信息长度
const int DEBUGINFOLEN = 1024;

//!成功标识
const static int Success = 0;

//!OPCUADriver驱动名
static const std::string OPCUADRIVER = "FepOPCUADriver";
static const std::string RTDBDRIVER = "RtdbDriver";

// Namespace
namespace MACS_SCADA_SUD {

class CIOPort;

typedef std::map<long, CIOPort*> MAP_PORT;	//!	通道节点号与通道指针MAP表
typedef std::map<std::string, long> MAP_PORTID;	//!	通道节点号与通道指针MAP表

//对模拟算法类型，仅对模拟命令有效。1-随机模拟；2-递增模拟；3-正弦模拟
typedef enum SimulateType
{
	SimulateType_Unknown = 0,	//未知类型
	SimulateType_Random = 1,	//随机模拟
	SimulateType_Increase = 2,	//递增模拟
	SimulateType_Sin = 3,		//正弦模拟
}emSimulateType;

//对象类型枚举定义
typedef enum ObjectType
{
	ObjectType_Unknown = 0,		//未知类型
	ObjectType_Channel = 1,		//通道类型
	ObjectType_Device = 2,		//设备类型
	ObjectType_Tag = 3,			//标签类型
	ObjectType_Site = 4,		//站点类型
}emObjectType;

//命令值枚举定义
typedef enum CmdValue
{
	CmdValue_Unknown = 0,					//未知
	CmdValue_Force = 1,						//强制，nodeId应为标签
	CmdValue_UnForce = 2,					//取消强制，nodeId应为标签
	CmdValue_Disable = 3,					//禁止，nodeId应为标签
	CmdValue_Enable = 4,					//取消禁止，nodeId应为标签

	CmdValue_Simulate = 5,					//模拟，nodeId应为设备或通道
	CmdValue_StopSimulate = 6,				//取消模拟，nodeId应为设备或通道
	CmdValue_Monitor = 7,					//通讯监视，nodeId应为通道
	CmdValue_StopMonitor = 8,				//取消通讯监视，nodeId应为通道
	CmdValue_StartLog = 9,					//记录日志，nodeId应为通道
	CmdValue_StopLog= 10,					//取消记录日志，nodeId应为通道
	CmdValue_Start = 11,					//启动，nodeId应为设备或通道
	CmdValue_Stop = 12,						//停止，nodeId应为设备或通道
	CmdValue_RedunForce = 13,				//主从强制，nodeId应为设备或通道
	CmdValue_UnRedunForce = 14,				//取消主从强制，nodeId应为设备或通道

	CmdValue_ExtraRangeEvent_1 = 15,		//!打开超量程事件开关
	CmdValue_ExtraRangeEvent_0 = 16,		//!关闭超量程事件开关

	CmdValue_SyncVal2Hot = 17,				//!将链路为从设备上更新值操作转发到链路为主设备
	IOPACKET_TYPE_SENDPACKET = 18,			//!向另机发送通道诊断包
	IOPACKET_TYPE_RECVPACKET = 19,			//!收到另机发来的数据包

	IOPACKET_TYPE_SYNIOCMD2SN = 20,			//!将遥控命令转发到链路为从另机
} CMDVALUE;

//!主从强制命令定义
typedef enum RedunStatus
{
	Redun_Second = 0, //强制从
	Redun_Prim = 1,   //强制主
	Redun_NotSet = 2, //未设置主从强制

}RedunStatus;


typedef enum IOPortSwitchStatus
{
	IOPORT_NO_SWITCH = 0,
	IOPORT_SlAVE_TO_MASTER ,
	IOPORT_MASTER_TO_SlAVE,
}IOPortSwitchStatus;

//!	端口配置结构体
typedef struct tIOPortCfg
{
	std::string     strType;			//!	链路类型：TCPIP、串口等
	int  nPort;				//!	端口号
	int	 iBaudRate;			//!	波特率
	int  iDataBits;			//!	数据位
	int	 iStopBits;			//!	停止位
	Byte	 byParity;			//!	奇偶校验 0无校验，1偶校验，2奇校验
	Byte	 byPortAccessMode;	//!	通道访问模式：1冲突；0不冲突
//	Byte	 byDefaultPrmScnd;	//!	默认主从
//	std::string	 strSwitchAlgName;	//!	主从切换算法名称
	int     iPower;			//!	权值
	std::string	 strOption;			//!	扩展配置
	std::string	 strIPA_A;			//!	A设备的A网IP地址
//	std::string	 strIPA_B;			//!	A设备的B网IP地址
//	std::string	 strIPB_A;			//!	B设备的A网IP地址
//	std::string	 strIPB_B;			//!	B设备的B网IP地址
	std::string     strSrlBrdPrefix;   //!	串口设备在linux环境上的名称前缀
}tIOPortCfg;

//!	驱动配置结构体
typedef struct tIODrvCfg
{
	int  iWatchTime;		//!	诊断时间
	int	 iPollTime;			//!	轮训时间
	int	 iTimeOut;			//!	超时时间
	Byte   byCommDataFormat;	//!	通讯监视的数据格式，缺省为16进制
	std::string     strDrvName;		//!	驱动名
}tIODrvCfg;

//!	设备配置结构体
typedef struct tIOUnitCfg
{
	int  iAddr;				//!	设备地址
	std::string	 strCfgName;		//!	配置文件名
}tIOUnitCfg;

typedef struct tIODataVariableCfg
{
	std::string m_strCfgAddress;   //标签配置地址
	int     m_lFinalAddress;   //计算地址
	int     m_iTagType;        //标签类型，由配置地址解析得出
	int     m_iDataType;       //数据类型
	float  m_fLowRange;        //原始刻度量程范围
	float   m_fHeightRange;
	float  m_fEngineerLowRange;        //工程刻度量程范围
	float   m_fEngineerHeightRange;
	float  m_fDeadZone;       //死区
}tIODataVariableCfg;


//!包通道诊断命令
typedef struct tIOCtrlPacketCmd
{
	long	    objNodeId;					//对象节点Id
	Byte    byType;						//包类型
	int  nLen;						//数据长度
	Byte byData[MAX_PACKETCMD_LEN];	//数据
}tIOCtrlPacketCmd;

//!遥控指令实时服务使用
typedef struct tIORemoteCtrl
{
	CNode*	pNode;						//FieldPoint节点指针
	std::string bysValue;				//值
}tIORemoteCtrl;


//!运行管理命令
typedef struct tIORunMgrCmd
{
	CNode*			pNode;				//对象节点
	Byte		byObjectType;		//操作对象类别
	Byte		byCmdValue;			//命令值，具体见命令值的宏定义CmdValue
	std::string	byCmdParam;			//对模拟命令，表示模拟算法类型: 1-随机模拟；2-递增模拟；3-正弦模拟
										//对标签强制命令，表示强制值
										//对主从强制命令，表示主或从，为RedunStatus类型
}tIORunMgrCmd;

//!通道状态结构
typedef struct tIOScheduleChannelState
{
	char sDriverName[NAME_LENGTH];		//驱动名
	char sChannelName[NAME_LENGTH];		//通道名
	Byte byGood;							//通道好坏: 1--好, 0---坏
	Byte bySimulate;						//是否模拟运行: 1--模拟, 0---没模拟
	Byte bySimMode;						//模拟算法类型
	Byte byDisable;						//通道是否被禁止: 1--被禁止, 0--没被禁止
	Byte byLog;							//通道是否记录日志: 1--记录日志, 0---不记录日志
	Byte byMonitor;						//通道是否通信监视: 1--通信监视, 0---不通信监视
	Byte byHot;							//通道是否为热机: 1--是热机, 0--是冷机
}tIOScheduleChannelState;

//!设备状态
typedef struct tIOScheduleUnitState
{
	char sUnitName[NAME_LENGTH];			//设备名
	Byte byOnline;						//是否在线: 1--在线, 0---离线
	Byte bySimulate;						//是否模拟运行: 1--模拟, 0---没模拟
	Byte bySimMode;						//模拟算法类型
	Byte byDisable;						//是否被禁止: 1--被禁止, 0--没被禁止
	Byte byHot;							//是否为热机: 1--是热机, 0--是冷机
	Byte byExtraRange;					//是否报超量程事件: 1--报, 0--不报.
}tIOScheduleUnitState;

//!驱动名.dbf中一条记录配置结构
typedef struct tIOCfgUnitType
{
	char		pchFormat[128];				//标签地址模板格式
	int		lLower;						//标签有效地址的下限
	int		lUpper;						//标签有效地址的上限
	int		iUnitType;					//UnitType
	int		iRawType;					//RawType
	int		iBitWidth;					//位宽度
	int		lOffset;					//本类型的数据在设备数据区中的首偏移
}tIOCfgUnitType;

//!波兰表达式中的元素
typedef struct tBOLITEM
{
	Byte		byType;						//类型标识：0－数值；1－变量；2－运算符；
	int		lValue;						//值，对于0－数值来说
	char		chOper;						//操作符 +-*/
	int		nPos;						//变量在表达式中的位置
	//如ST%U(0,0,127)%*10:%+U(0,0,8)%*1000:%+U(0,0,500)%*1000:%+U(0,0,500),ST1:2:3:1，分解为A,B,C,D四个变量
	//则A记录为0，B为1，C为2，D为3
	int		lUpper;						//范围中的上限
	int		lLower;						//范围中的下限
	int		lDiffer;					//校正值
}tBOLITEM;

//!分解的波兰表达式
typedef struct tBOLAN
{
	Byte		byType;						//0表示%U，1表示常项,2表示%S
	int		    nCount;						//表达式中项的个数
	tBOLITEM		tItem[32];					//表达式中的项
}tBOLAN;

//!一种标签类型的描述结构
typedef struct tIOCfgUnitDataDes
{
	char		pchFormat[128];
	char		pchID[16];					//标识符，如AI%U(0.0.10)中的AI
	tBOLAN		tBol;						//分解的波兰表达式
	int			lLower;						//标签计算地址的下限
	int			lUpper;						//标签计算地址的上限
	int			lDataOffset;				//本类型的数据在设备数据中的首偏移地址
	int			iUnitType;
	int			iRawType;
	int			iBitWidth;
	int			iLow;
	int			iHigh;
	char		pchComment[64];
}tIOCfgUnitDataDes;

//! 存储ClassName和DllFileName结构体
typedef struct tExtInfo
{
	//! 文件名，即动态库名,包含后缀.dll
	char pchFileName[64];
	//! 类名
	char pchClassName[32];
}tExtInfo;

const int COMMDATALEN = 1024;		//!通信监视数据包最大长度

//!通信监视数据结构
typedef struct tCommData
{
	char		pchDriverName[NAME_LENGTH];	//!驱动名称
	long	dataTime;					//!时标
	Byte	byDataType;					//!发送或接受的标识:0为接收,1为发送
	int	nDataLen;					//!数据长度
	Byte	pbyData[COMMDATALEN];		//!数据内容
}tCommData;

//!遥控命令结构FEP使用
typedef struct tIOCtrlRemoteCmd
{
	char pchUnitName[NAME_LENGTH];	//!设备名称
	Byte	byUnitType;					//!标签类型
	long long	lTagAddress;				//!标签地址
	int nDataLen;					//!数据有效长度
	Byte	byData[MAX_REMOTECMD_LEN];	//!数据
}tIOCtrlRemoteCmd;

typedef struct tIORemoteWriteCmd
{
	char pchUnitName[NAME_LENGTH];	//!设备名称
	Byte	byUnitType;					//!标签类型
	long long	lTagAddress;				//!标签地址
	int nDataLen;					//!数据有效长度
	Byte  byData[0];					//!可变长度数组
}tIORemoteWriteCmd;

//!链路主从状态切换方式
typedef enum PortSwtichMode
{
	SLAVE2MASTER = 0,		//!从升主
	MASTER2SLAVE = 1,		//!主降从
	RECOVER2DEFALUT = 2		//!恢复默认
}PortSwtichMode;

typedef struct tIOCmdCallBackParam
{
	void* pMethodHandle;
	void* pExtendParam;
}tIOCmdCallBackParam;
//!接收的通信监视数据
const int COMMDATA_RCV = 0;
//!发送的通信监视数据
const int COMMDATA_SND = 1;

//! 驱动相关的信息>>>From LF
const Byte DEVICE_ONLINE  = 1;		//! 设备在线
const Byte DEVICE_OFFLINE = 0;		//! 设备离线
const Byte DEVICE_INIT    = 2;		//! 设备初始态

//!通道诊断包类型>>>From LF
const Byte IOPACKET_TYPE_ASKFOR_SENDPACKET  = 1;				//请求组包

//!常用文件名
#define MACS_DRIVER_FILENAME "MacsDriver.ini"

//!驱动通用参数文件名
#define MACS_COMM_DRIVER_FILENAME "CommDriver.ini"


//!设置通讯状态标签
#define DT_ISONLINE		"DEVCOMM"
#define DT_ISONLINE_A	"DEVCOMMA"
#define DT_ISONLINE_B	"DEVCOMMB"

//!死区百分比转换单位
const double DEADZONE_UNIT = 0.00001;

//!事件结构定义
typedef struct tIOEventElement
{
	Byte		byPrimType;			//0:外部事件；1：内部事件
	Byte		byType;				//事件索引
	int		    tTime;				//时标
	char		pchTagName[16];		//标签名
	int		    nVal;				//值
	Byte		byValid;			//有效标志
	char			pchTagDes[64];		//标签描述
	char			pchValDes[64];		//值描述
}tIOEventElement;

//! 调试信息的数据结构,即队列中的内容
typedef struct tDebugInfo
{
	char pchInfo[DEBUGINFOLEN];			//! 信息内容
}tDebugInfo;

//! 对外提供数据服务配置信息
typedef struct tIOServerInfo
{
	char pchServerName [64];				//!对外服务的名称
	char pchCfgFileName[64];				//!对外服务对应的配置文件名
}tIOServerInfo;

//!冗余FEP之间链路状态信息同步周期(采样周期和发布周期)
const uint PortInfoSyncPeriod = 200;

typedef struct tCommDriverCfg
{
	int		LogFileStorDays;		//单位：天。日志归档周期天数。0表示不归档，只生成一个文件，写满后新文件替换旧文件。
	int		LogFileSize;			//单位M。单个日志最大长度，默认2。
	bool	CommuMonitorFlag;		//驱动通讯监视开关,默认0。0：关闭； 1:打开。
	std::string		LogFilePath;			//数据类型
	std::string		SrlBoardPrefix;			//串口驱动中使用的设备前缀字符串.默认值: ttyM
	bool	IsSendEvent2RT;			//FEP是否向实时服务发送事件


	int		Count;					//要加载的对外服务的个数。如果不需要FEP提供子站服务，则该项配置为0
	std::string		Server1;				//要加载的对外服务的名称,104子站服务
	std::string		Server1Cfg;				//要加载的对外服务的名称,104子站服务
	bool	IOSub104NeedLog;		//104子站是否开启日志功能
	bool	IOSub104NeedComm;		//104子站是否开启通信监视功能

	std::string		Server2;				//要加载的对外服务的名称,Modbus子站服务
	std::string		Server2Cfg;				//Modbus子站服务对应的配置文件名称
	bool	IOSubModbusNeedLog;		//Modbus子站是否开启日志功能
	bool	IOSubModbusNeedComm ;	//Modbus子站是否开启通信监视功能

	tCommDriverCfg()
	{
		LogFileStorDays = 0;
		LogFileSize = 2;
		CommuMonitorFlag = false;
		LogFilePath = "";
		SrlBoardPrefix = "ttyM";
		IsSendEvent2RT = false;
		Count = 0;
		Server1 = "IOSub104Manager";
		Server1Cfg = "Sub104.xml";
		IOSub104NeedLog = false;
		IOSub104NeedComm = false;
		Server2 = "IOSubModbusMgr";
		Server2Cfg = "SubModbusCfg.xml";
		IOSubModbusNeedLog = false;
		IOSubModbusNeedComm = false;
	}
}tCommDriverCfg;

const static std::string MOD_DrvSchedule = "DBDrvScheduleMgr";
const static std::string MOD_DBFrameAccess = "DBFrameAccess";
const static std::string MOD_FepPlatformClass = "FEPPlatformClass";

}// End namespace

#endif // fepcommondef_h__
