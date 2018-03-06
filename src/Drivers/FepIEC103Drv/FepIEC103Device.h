/*!
*	\file	FepIEC103Device.h
*
*	\brief	103主站驱动设备类头文件
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Device.h 3     11-02-24 11:25a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-02-24 11:25a $
*	$Revision: 3 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/


#ifndef _FEP_IEC103Device_H_20110110_
#define _FEP_IEC103Device_H_20110110_

#include "PublicFileH.h"
#include "IEC103.h"
#include "PRDFile103Dev.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"
#include <vector>
#include <map>
using namespace std;

#define MAX_DI_NUM							1024								//DI点最大数量
#define MAX_DO_NUM							256									//DO点最大数量
#define FAULT_WAVE_TIMEOUT	600000	//故障录播接收超时,毫秒.
#define MAX_SOE 10000 //!SOE最大条目
#define UNIT_TYPE_INDEX_DIGITAL 0
#define UNIT_TYPE_INDEX_REAL    1
//#define UNIT_TYPE_INDEX_INT     2
//#define UNIT_TYPE_INDEX_STRING  3
#define FAULT_WAVE_CYCLE 15000

namespace MACS_SCADA_SUD
{	
	typedef std::map<OpcUa_Int32, OpcUa_Int32> SOEMAP;
	typedef SOEMAP::iterator SOEMAPiterator; 

	typedef std::map<OpcUa_Int32, OpcUa_Int32> PointMap;
	typedef PointMap::iterator PointMapiterator;

	typedef std::map<OpcUa_Int32, bool> StatusMap;
	typedef StatusMap::iterator StatusMapiterator; 

	typedef std::map<OpcUa_Int32, double> AIModulusMap;
	typedef AIModulusMap::iterator AIModulusMapiterator; 


	// **********************************************************************************************
	// 编 写 者: ZhouJun        
	// 创建时间: 2006-10-26 
	// 类 名 称: tUnitTypeRes
	// 功    能: 设备中数据类型的相关对照
	// 算法思想: 
	// **********************************************************************************************
	#pragma pack(push,1)
	typedef struct tUnitTypeRes
	{
		CIOCfgUnitType* pUnitTypeCfg;		//设备中数据类型的配置
		OpcUa_Byte*           pUnitTypeAddr;		//设备中数据类型对应数据库中位置指针
		OpcUa_Int32				nOffset;			//设备中数据类型的偏移
	}tUnitTypeRes;

	enum enumASDU24TOO
	{
		T_FaultSelect,				//故障选择
		T_Data_Req,					//扰动数据的请求
		T_Data_Trans_End,			//扰动数据的中止
		T_StatusChange_Req,			//请求带标志的状态变位传输
		T_StatusChange_End,			//中止带标志的状态变位传输
		T_Chanel_Req,				//通道传输的请求
		T_Chanel_End				//通知传输的中止
	};

	// **********************************************************************************************
	// 编 写 者: ZhouJun        
	// 创建时间: 2006-11-11 
	// 类 名 称: FaultDataSet
	// 功    能: 被记录的扰动表中的数据集结构
	// 算法思想: 
	// **********************************************************************************************
	typedef struct struFaultDataSet
	{
		WORD				FAN;		//故障序号
		OpcUa_Byte				SOF;		//故障的状态
		TIMESTAMP_7BYTE		TimeSet;	//时间集
	}FaultDataSet;
	#pragma pack(pop)
	typedef std::vector<struFaultDataSet> FaultDataVec;

	class CIOTimer;


	// **********************************************************************************************
	// 编 写 者: ZhouJun        
	// 创建时间: 2006-10-26 
	// 类 名 称: CFepIEC103Device
	// 功    能: 封装设备标签值管理、SOE处理、越量程事件处理、以及设备相关的各状态
	// 算法思想: 
	// **********************************************************************************************
	class CIOCfgUnit;
	class CIOCfgUnitType;
	class CIOTimer;
	class CPRDFile103Dev;
	class FEPIEC103_API CFepIEC103Device
		:public CIODeviceBase
	{
		friend void DOCTimer(void* pParent);
	public:
		//构造、析构
		CFepIEC103Device( );
		~CFepIEC103Device();
	public:	
		static std::string s_ClassName;
		std::string GetClassName()
		{
			return s_ClassName;
		}
		/*!
		*	\brief	初始化
		*
		*	\retval	0为成功，否则失败
		*/
		virtual OpcUa_Int32 Init();
		void GetTimeFromCP32Time( CP32Time2a Time, OpcUa_Byte& Second, WORD& MilliSecond );
		
		//组下行时间同步帧
		OpcUa_Int32 BuildCheckTimeFrame( OpcUa_Byte* buf, bool bBroadCast );

		//组总查询帧
		OpcUa_Int32 BuildGIFrame( OpcUa_Byte* pBuf );

		/*!
		*	\brief	组“复位通信单元”帧
		*
		*	详细的函数说明（可选）
		*
		*	\param	参数	OpcUa_Byte* pBuf: 要加的数据		
		*
		*	\retval	返回值	OpcUa_Int32 iLen: 要加的数据的长度
		*
		*	\note
		*/
		OpcUa_Int32 BuildResetCUFrame( OpcUa_Byte* pBuf );

		//组“复位帧计数器”帧
		OpcUa_Int32 BuildResetFCBFrame( OpcUa_Byte* pBuf );

		//组“请求一级数据”帧
		OpcUa_Int32 BuildLevel1DataFrame( OpcUa_Byte* pBuf );

		//组“请求二级数据”帧
		OpcUa_Int32 BuildLevel2DataFrame( OpcUa_Byte* pBuf );

		//组“遥控”命令帧
		OpcUa_Int32 BuildDOFrame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr, OpcUa_Byte bVal);

		//组“遥控”命令帧
		OpcUa_Int32 BuildDOBFrame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr, OpcUa_Byte bVal);

		//组ASDU140读取帧
		OpcUa_Int32 BuildASDU140Frame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr);

		//组ASDU232命令帧
		OpcUa_Int32 BuildASDU232Frame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr, OpcUa_Byte bVal);

		//组ASDU21总召唤帧
		OpcUa_Int32 BuildASDU21WholeCallFrame( OpcUa_Byte* pBuf );

		//组ASDU21某组召唤帧
		OpcUa_Int32 BuildASDU21GroupFrame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr );

		//组“ASDU24”帧
		OpcUa_Int32 BuildASDU24Frame ( OpcUa_Byte* pBuf, enumASDU24TOO eTOO );

		//组“ASDU25 肯定或否定认可”
		OpcUa_Int32 BuildDisDataAckFrame( OpcUa_Byte* pBuf, OpcUa_Byte bAck );

		//补偿系统时间
		//SYSTEMTIME OffsetSystemTime( OpcUa_Int32 msOffset );

		/*!
		*	\brief	FCB取反
		*	 
		*	\retval	返回值	无
		*
		*	\note		注意事项（可选）
		*/
		void ReverseFCB( void );

		//从配置文件中读取“标签是否需要SOE”
		void InitTagSOEMap(void);

		//查看标签量是否需要SOE
		bool TagNeedSOE (OpcUa_Byte byType, OpcUa_Int32 iTagAddr );
		
		bool TagNeedDevSOE (OpcUa_Byte byType, OpcUa_Int32 iTagAddr );

		//通过标签位置查“功能类型”和“信息序号”的组合
		OpcUa_Int32 GetPointAddr (OpcUa_Int32 iIndex)
		{
			PointMapiterator it = m_DIPointMap.find(iIndex);
			if (it != m_DIPointMap.end())
				return it->second;
			return -1;
		}

		//通过标签位置查“功能类型”和“信息序号”的组合
		OpcUa_Int32 GetDOPointAddr (OpcUa_Int32 iIndex)
		{
			PointMapiterator it = m_DOPointMap.find(iIndex);
			if (it != m_DOPointMap.end())
				return it->second;
			return -1;
		}

		//通过“功能类型”和“信息序号”反查标签位置
		OpcUa_Int32 GetTagIndex (OpcUa_Int32 iFunType, OpcUa_Int32 iInfoSeqNO)
		{
			OpcUa_Int32 iTagIndex = 1000*iFunType + iInfoSeqNO;
			PointMapiterator it = m_DIPointMap.begin();
			while ( it != m_DIPointMap.end())
			{
				if (it->second == iTagIndex)
					return it->first;
				it++;
			}
			return -1;
		}
	
		/*!
		*	\brief	设置从机该设备的超时次数
		*/
		void SetSlaveTimeOuts(OpcUa_Int32 nSlaveTimeOuts )
		{
			m_byStandyOffCounter = nSlaveTimeOuts;
		};

		//! 设置设备的主从
		virtual void SetHot(OpcUa_Byte byHot);

		bool IsSuspicious()
		{
			return m_bIsSuspicious;
		}

		void SetSuspicious( bool bState )
		{
			m_bIsSuspicious = bState;
		}

	protected:
		//获取某字节某位的值
		OpcUa_Byte GetBitVal( OpcUa_Byte byIn, OpcUa_Int32 iBit );						

		//设置某字节某位的值
		void SetBitVal( OpcUa_Byte* byInOut, OpcUa_Int32 iBit, OpcUa_Int32 iVal );

	public:
		CPRDFile103Dev*			m_pPRDFile;             //负责将故障录波相关报文转换为Comtrade格式的录波文件
		OpcUa_Byte					m_byTxTimes;			//发送次数
		OpcUa_Byte					m_byRxTimes;			//接收次数
		OpcUa_Int32						m_nWatchDogCounter;		//
		OpcUa_Int32						m_nWatchDog;			//
		OpcUa_Int32						m_nTagCount;			//设备实际组态标签个数
		tUnitTypeRes*			m_pTypeRes;				//各种类型数据的保存位置
		vector<OpcUa_Byte>			m_vExtraRangeFlag;		//超量程事件标志

		WORD					m_wFAN;					//故障序号

		//保存控制方向的控制域信息
		bool					m_bFCB;					//帧计数位
		bool					m_bFCV;					//帧计数有效位

		//保存监视方向的控制域信息
		bool					m_bACD;					//要求访问位
		bool					m_bDFC;					//数据流控制位

		OpcUa_Int32						m_nSendResetCUCounter;	//发送复位帧重试计数器
		bool					m_bSendRestCU;			//发送复位帧标志位
		bool					m_bReq1LevelData;		//发送一级数据请求标志位
		bool					m_bReq2LevelData;		//发送二级数据请求标志位

		bool					m_bNeedInit;			//初始化标志
		OpcUa_Int32						m_FaultTimeOut;			//故障录波超时,毫秒。
		OpcUa_Int32						m_nFautTimeOutCount;	//

		bool					m_bNeedCheckTime;		//校时标志

		bool					m_bNeedASDU21GI;		//总查询标志：ASDU21
		bool					m_bNeedASDU7GI;			//总查询标志：ASDU7
		OpcUa_Int32						m_iGINo;				//总查询次数
		bool					m_IsSOE;				//标识上传的ASDU1是否为SOE(非总查询阶段)

		bool					m_bFaultSel;			//故障选择
		OpcUa_Byte					m_byFaultNum;			//故障个数
		FaultDataVec			m_FaultDataVec;			//被记录扰动表中的数据集
		OpcUa_Byte					m_byRcvFaultNum;		//已成功接收到的故障个数

		OpcUa_Byte					m_byNOC;				//通道数目
		WORD					m_NOE;					//一个通道信息元素的数目
		WORD					m_sINT;					//信息元素（扰动值）的以微秒为单位的采样间隔						

		bool					m_bReqDisData;			//“扰动数据请求”标志
		bool					m_bReqStatusAlter;		//“请求带标志的状态变位传输”标志

		OpcUa_Byte					m_byNOT;				//带标志的状态变位的数目

		bool					m_bSendDisDataAck;		//“扰动数据认可”标志
		OpcUa_Byte					m_byDisDataAckTOO;		//“扰动数据认可”命令类型

		bool					m_bChannelReady;		//通道传输准备就绪标志
		OpcUa_Byte					m_byACC;				//通道序号

		WORD					m_sNDV;					//每个ASDU有关扰动值的数目
		WORD					m_sNFE;					//传输扰动值报文中第一个信息元素的序号，用于重组报文

		bool					m_bChannelEnd;			//通道传输结束

		OpcUa_Byte					m_byRII;

		SOEMAP					m_DISOEMap;
		SOEMAP					m_DIDevSOEMap;			//设备上送的SOE（如ASDU41）需要无条件报SOE的map表。
		SOEMAP					m_DOSOEMap;		
		SOEMAP					m_DOWithSel;

		//!点坐标
		PointMap				m_DIPointMap;
		PointMap				m_DOPointMap;
		
		AIModulusMap			m_AIModulusMap;			//AI的系数map
		OpcUa_Byte					m_byAIFUN;				//ASDU9的功能类型
		OpcUa_Byte					m_byAISEQ;				//ASDU9的信息序号

		StatusMap				m_statusMap;
		
		OpcUa_Byte m_DIs[MAX_DI_NUM];
		OpcUa_Byte m_DOAs[MAX_DO_NUM];

		bool m_bInDO;									//DO命令过程中
		OpcUa_Int32 m_iInDOIndex;								//处于DO命令过程中的点地址
		OpcUa_Int32 m_iInDOMIndex;								//!上次处理的DOM命令的点地址
		CIOTimer*				m_pDOCTimer;			//控制选择计时器	
		CIOTimer*				m_pFaultTimeOutTimer;	//故障录波超时定时器

		OpcUa_Byte					m_byFaultFUN;			//故障的功能类型
		OpcUa_Int32						m_iAIMAX;				//一个报文中AI的数量


		bool					m_bNew;					//刚被添加
		bool					m_bFirstCT;				//第一次的校时结束

		//CRITICAL_SECTION		m_FaultWaveSection;		//故障录播互斥灯
		bool m_bIsRcvingFaultWave; //!是否正在进行录波处理

		OpcUa_Byte					m_byStandyOffCounter;	//从站离线计数器

		OpcUa_Int32		m_iASDU7ReadGroupPeriod;				//ASDU7  ASDU7总召唤周期,秒. 0表示不进行ASDU7总召唤.
		//! ASDU21 周期读取的Group号及周期
		OpcUa_Int32		m_iASDU21ReadGroupPeriod;				//ASDU21 周期读取的Group号的周期,秒. 0表示不进行ASDU21总召唤.
		vector<OpcUa_Byte> m_vGroupNo;						//ASDU21 周期读取的Group号
		OpcUa_Int32		m_iGroupIndex;							//ASDU21 组索引号. 

		bool    m_bConfirmed;							//! 上次应答中控制字段中的功能码显示为0(确认帧,确认)或8(响应帧,以数据响应请求帧)

		bool m_bRetryPeriod;	//离线设备该发送请求了

		bool m_bIsSuspicious;	//设备可疑

		//根据索引得到AI的系数
		double GetModulusByTag(OpcUa_Int32 iTag)
		{
			AIModulusMapiterator it = m_AIModulusMap.find(iTag);
			if (it != m_AIModulusMap.end())
				return it->second;
			//	return -1;	//Del by hxw in 2008.1.30
			return 0;	//Add by hxw in 2008.1.30
		}

	protected:
		OpcUa_Byte					m_byBitMask[8];			//位处理协助掩码

	private:
		OpcUa_Int32						m_iTypeCount;			//数据类型的数量	
		OpcUa_Byte*					m_pDataAddr;			//设备数据保存位置	
		bool					m_bCanSendSoe;			//!启动时不判断自产SOE标志		
	};
}

#endif	
