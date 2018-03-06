#pragma once
#include <vector>
#include <map>
#include <string>
#include "PublicFileH.h"

#define COMTRADE_FILE_PATH "E:\\"

namespace MACS_SCADA_SUD
{
#pragma pack(push,1)
	typedef std::map<std::string, std::string> T_mapStringType;

	//! 
	typedef std::vector<std::string> T_vecString;
	
	
	typedef struct T_CtdChgInfo
	{
		std::string szName;			//通道名称
		std::string szLineNo;		//线路标识
	}T_CtdChgInfo;

	//! 数字量通道的点标识
	typedef struct T_DIFunInf
	{
		OpcUa_Byte nFun;		//功能类型
		OpcUa_Byte nInf;		//信息序号
	}T_DIFunInf;

	//! 数字量通道的值记录
	typedef struct T_DIChannelValType
	{
		OpcUa_Int32 nOrgPos;	//初值的位置，通常为0
		OpcUa_Byte nOrgVal;	//初值
		OpcUa_Int32 nChgPos;	//变位之后的位置
		OpcUa_Byte nChgVal;	//变位之后的值
	}T_DIChannelValType;

	//! 带标志的状态变位的信息索引表
	typedef std::map<T_DIFunInf, T_CtdChgInfo> T_mapCtdChgIndexType;

	//! COMTRADE通道信息
	typedef struct T_CtdChannelInfo
	{
		std::string szName;			//通道名称
		std::string szPhase;		//通道相别
		std::string szUnits;		//通道值的单位
	}T_CtdChannelInfo;

	//! COMTRADE通道索引表;<通道的实际序号，通道信息>
	typedef std::map<OpcUa_Int16, T_CtdChannelInfo> T_mapCtdChannelType;
///////////////////////////////////////////////
	//! 故障录波标识
	typedef struct T_FaultType
	{
		std::string szFaultNumber;		//故障序号
		std::string szOcurTime;			//故障发生时间
	}T_FaultType;
	/////////////////////////////////////



//! 模拟量通道的描述信息
typedef struct T_AIChannelInfo
{
	//OpcUa_Byte byAcc;			//实际通道序号
	OpcUa_Float	fPrim;			//额定一次值
	OpcUa_Float   fSecond;		//额定二次值
	OpcUa_Float	fRFA;			//参比因子

	OpcUa_Int32		nNFE;		//应用服务数据单元的第一个信息元素的序号

	OpcUa_Int32		nDataCount;	//接收到的扰动值的数目，是可变的
}T_AIChannelInfo;

typedef std::vector<OpcUa_Int32> T_AIChannelVAL;

typedef std::map<OpcUa_Byte, T_AIChannelVAL> T_AIChannelValIndex;

//! 分字节
typedef struct{
	OpcUa_Byte	MIN:6;
	OpcUa_Byte	RES:1;
	OpcUa_Byte	IV:1;
}T_MinByte;

//! 时字节
typedef struct{
	OpcUa_Byte HOUR:5;
	OpcUa_Byte RES:2;
	OpcUa_Byte SU:1;
}T_HourByte;

//! 天字节
typedef struct{
	OpcUa_Byte DAY:5;
	OpcUa_Byte WEEK:3;
}T_DayByte;

//! 月字节
typedef struct{
	OpcUa_Byte MONTH:4;
	OpcUa_Byte RES:4;
}T_MonthByte;

//! 年字节
typedef struct{
	OpcUa_Byte YEAR:7;
	OpcUa_Byte RES:1;
}T_YearByte;
#pragma pack(pop)

class CFepIEC103Drv;
class CDriverItemType;

class CPRDFile103Dev
{
public:
	CPRDFile103Dev(void);
	~CPRDFile103Dev(void);
	CPRDFile103Dev( std::string strDrvName, std::string strDevName, CFepIEC103Drv* pDeal );

public:
	//! 解析录波响应包
	OpcUa_Int32 DealRcvData( OpcUa_Byte* pbyData );

	//! 生成CFG文件
	OpcUa_Int32 ProduceCfgFile();

	//! 生成dat文件
	OpcUa_Int32 ProduceDatFile();

	//! 初始化
	void Init();

private:
	//! 处理解析ASDU23
	OpcUa_Int32 DealASDU23( OpcUa_Byte* pData );

	//! 处理解析ASDU26
	OpcUa_Int32 DealASDU26( OpcUa_Byte* pData );

	//! 处理解析ASDU28
	OpcUa_Int32 DealASDU28( OpcUa_Byte* pData );

	//! 处理解析ASDU29
	OpcUa_Int32 DealASDU29( OpcUa_Byte* pData );

	//! 处理解析ASDU31
	OpcUa_Int32 DealASDU31( OpcUa_Byte* pData );

	//! 处理解析ASDU27
	OpcUa_Int32 DealASDU27( OpcUa_Byte* pData );

	//! 处理解析ASDU30
	OpcUa_Int32 DealASDU30( OpcUa_Byte* pData );

	//! 清内存
	void Clear();

	//! 清除标志
	void ClearFlag();

	//! 得到7个八位位组的二进制时间的字符串
	std::string Get7BytesTimeString( OpcUa_Byte* pData );

	//! 得到4个八位位组的二进制时间的字符串
	std::string Get4BytesTimeString( OpcUa_Byte* pData );

	//! 得到当前故障序号的文件名
	OpcUa_Int32 GetComtradeFileName();
	
	//! 得到指定功能码和序号的状态变位的信息
	OpcUa_Int32 GetCtdChgInfo( OpcUa_Int16 nFun, OpcUa_Int16 nInf, T_CtdChgInfo& chgInfo );
	
	//! 得到指定实际通道序号的通道信息
	OpcUa_Int32 GetCtdChannelInfo( OpcUa_Int16 nAcc, T_CtdChannelInfo& channelInfo );
	bool ExistDir( std::string strDirName );
	bool CreateDir( std::string strDirName );
	std::string GetParentDir( std::string sDir );
	void Parse103PRDCfg( CDriverItemType* pDrvItem, std::string szDevName );

private:
	//! 本类所对应的设备名称
	std::string m_szDevName;

	////! 上一包来自的域名
	//std::string m_szDomainName;

	//! —————————————— ASDU23 —————————————— 
	//! 本装置中所有的故障序号和发生时间的对应数组，时间的格式为：dd/mm/yyyy,hh:mm:ss.ssssss 
	std::map<OpcUa_Int32, std::string> m_mapFaultNumberTimeIndex;

	//! —————————————— ASDU26 —————————————— 
	//! 当前请求的故障序号 —— ASDU26
	OpcUa_Int32 m_nCurFaultNumber;

	//! 电网故障序号（注：在COMTRADE文件中没有记录） —— ASDU26
	OpcUa_Int32 m_nNetFaultNumber;

	//! 通道数目（注：是否只是模拟量通道，不包含数字量的？） —— ASDU26
	OpcUa_Int32 m_nChannelCount;

	//! 一个通道信息元素的数目 —— ASDU26
	OpcUa_Int32 m_nElementCount;

	//! 信息元素间的间隔
	OpcUa_Int32 m_nElementInterval;

	//! 开始记录故障的时间串:dd/mm/yyyy,hh:mm:ss.ssssss  —— ASDU26
	std::string m_szRecordTime;

	//! —————————————— ASDU28 —————————————— 
	//! 判断故障序号是否有效

	//! —————————————— ASDU29 —————————————— 
	std::map<T_DIFunInf, T_DIChannelValType> m_mapDIChannel;

	//! —————————————— ASDU31 —————————————— 
	//! 数字量通道传输结束

	//! —————————————— 第N个通道传输过程 —————————————— 
	//! —————————————— ASDU27 ——————————————
	std::map<OpcUa_Byte, T_AIChannelInfo> m_mapAIChannel;

	//! —————————————— ASDU30 ——————————————
	T_AIChannelValIndex m_mapAIVal;

	//! —————————————— ASDU31 —————————————— 
	//! 第N个通道传输结束

	//! —————————————— ASDU31 —————————————— 
	//! 扰动数据传输结束

	//! 最小值
	OpcUa_Int32 m_nMinVal;

	//! 最大值
	OpcUa_Int32 m_nMaxVal;

private:
	//! 当前进行的第几部分的标志：0为无意义；1为初始化过程；2为状态变位过程；3为通道过程。
	OpcUa_Byte m_byPartType;

	//! 当前接收帧的标识: m_byPartType=1和2时，为ASDU的号；当m_byPartType=3时，为实际通道号。
	OpcUa_Byte m_byFrameLabel;

	//! 结束类型的标志：2为状态变位传输结束；3为通道传输结束；1为扰动数据全部传输结束。0无意义。
	OpcUa_Byte m_byCompleteType;

	//! 故障发生的文件名串“设备名_日期时间_故障号+后缀名”，其中时间为“YYYYMMDDhhmmssssssss”
	std::string m_szCurFileName;

	//CPRDFile103Deal* m_pDealImpl;
	std::string m_szPrdFilePath; //故障录波文件存放路径

	T_mapCtdChannelType m_mapChannelInfo;	//模拟量通道信息
	T_mapCtdChgIndexType m_mapStateInfo;	//状态量通道信息
	std::string m_strDrvName;				//!所属驱动组态名
	CFepIEC103Drv* m_pIEC103Drv;

public:
	CIOLog* m_pLog;
};
}
