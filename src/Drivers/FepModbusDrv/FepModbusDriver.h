/*!
 *	\file	FepModbusDriver.h
 *
 *	\brief	通用Modbus驱动的驱动管理类头文件
 *
 *	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusDriver.h, 470+1 2011/04/22 01:56:47 xuhongtao $
 *	$Author: xuhongtao $
 *	$Date: 2011/04/22 01:56:47 $
 *	$Revision: 470+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _FEP_MODBUS_DRIVER_20080514_H
#define _FEP_MODBUS_DRIVER_20080514_H

#include "FepModbusDrvH.h"
#include "RTDBDriverFrame/IODriver.h"

namespace MACS_SCADA_SUD
{	
	/*!
	*	\class	CFepModbusDriver
	*
	*	\brief	TEP-I-F直流屏微机监控装置驱动的驱动管理类
	*/
	class FEPMODBUSDRV_API CFepModbusDriver :
		public CIODriver
	{
	public:
		//! 缺省构造函数
		CFepModbusDriver();
		//! 缺省析构函数
		~CFepModbusDriver();

	public:
		//! 获得类名
		std::string GetClassName();

		/*!
		*	\brief	得到继承PrtclParserBase基类的继承类名
		*
		*	\retval	返回值：继承类名
		*/
		virtual std::string GetPrtclClassName();

		/*!
		*	\brief	得到继承DeviceBase基类的继承类名
		*
		*	\retval	返回值：继承类名
		*/
		virtual std::string GetDeviceClassName();

		/*!
		*	\brief	开始工作
		*
		*	\retval	返回值：0--成功，其它失败
		*/
		virtual int StartWork(void);

		/*!
		*	\brief	停止工作
		*
		*	\retval	返回值：0--成功，其它失败
		*/
		virtual int StopWork(void);

		/*!
		*	\brief	初始化设备
		*	得到设备类名，动态创建设备对象；
		*	得到设备模拟类名，动态创建设备模拟对象；
		*	设置设备的状态结构指针，配置对象指针；
		*	初始化设备
		*
		*	\param	unitState：设备状态指针
		*	\param	pCfgUnit：设备配置对象指针
		*
		*	\retval	返回值：0--成功，其它失败
		*/
		virtual int InitUnit(CIOUnit* pCfgUnit);

		/*!
		*	\brief	打开通道
		*
		*	设置通道参数，并打开通道，启动驱动运行线程
		*
		*	\param	portCfg	记录通道详细设置的结构体
		*
		*	\retval	int	成功则返回0
		*/
		virtual int OpenChannel();

		//! 用于TCP Modbus的一些函数
		//////////////////////////////////////////////////////////////////////
		//! 初始化网络连接参数
		virtual int InitNetParam();

		//! 添加服务器
		void AddSerAddr(const char* pchSerAddr);

		//! 封装一下连接操作，参数为服务器序号
		int Connect(int nIndex = 0);

		//! 置试连接次数
		inline void SetConnTime(int nTime)
		{
			m_nConnTimes = nTime;
		};

		//! 试连接次数加1
		inline void IncrConnTime()
		{
			m_nConnTimes ++;
		};

		/*!
		*	\brief	获得连接定时器指针
		*/
		inline CIOTimer* GetConnTimer()
		{
			return m_pConnTimer;
		}

		inline bool GetConnected()
		{
			return m_bConnected;
		}

		inline void SetConnected( bool bConnected )
		{
			m_bConnected = bConnected;
		}

		/*!
		*	\brief	重连
		*
		*	如果连接失败，则重连
		*	单个服务器连续m_byMaxReConn次连接失败则换下一个服务器
		*
		*/
		void ReConnect(  bool bNext = false );

		/*!
		*	\brief	连接下一个服务器
		*/
		void ConnectNext();	

		//! 直接记录日志
		int WriteLogAnyWay( const char* pchLog, bool bTime = true );

		/*!
		*	\brief	获得当前连接的服务器IP
		*/
		std::string GetCurServerAddr()
		{
			return m_stSerAddrList[m_nCurSerAddrIndex];
		};

		//检查是否链路离线，即是否所有设备都离线。
		bool IsLinkOffline();

		//检查是否存在任意一个设备IsFuzzy()为true;
		bool IsFuzzy();

		bool CanWork();

		friend void OnTimeOut(void* p);

		//!启动TCPIP连接定时器
		int StartConnTimer();

		//!得到设备最大离线次数
		int GetMaxOffLineCount();

		//!返回设备报文接收超时时间
		int GetRcvTimeOut();

		//!软件是否为英文配置
		bool IsEnglishEnv();


	public:
		//! 类名
		static std::string s_ClassName;	
		//! 用于重连机制的锁
		ACE_Recursive_Thread_Mutex m_Mutex;
		
		int m_nWatchTime;		//链路冲突模式时，从机的诊断周期。毫秒。

		int	m_iDevIndex;		//设备逻辑单元号: 0 ~ Channel[0]->nMaxUnit-1	
		int	m_iDevLastIndex;
		int	m_nMaxRetryPeriod;	//用于部分设备离线时，离线设备按大周期发送请求帧的处理. 单位：分钟。

		int m_nMaxLinkOffline;				//链路最大离线秒数：
		int m_MaxHotLinkOffline;			//链路冲突模式时，主机链路最大离线秒数：如果在此时间内没有收到任何设备报文，则断定该链路离线。
		int m_nMaxStandbyLinkOffline;		//链路冲突模式时，从机链路最大离线秒数：如果在此时间内没有收到任何设备报文，则断定该链路离线。
		int m_nRcvTimeOut;					//接收超时
		int m_nMaxOfflineCount;				//离线计数器
		bool m_bIsLinkGetRightFrame;		//链路上是否接受到正确响应帧
		int m_nMaxLinkOfflineConnectCount;	//最大链路连接超时次数
		int m_nLinkOfflineConnectCount;		//当前链路连接超时次数

		//! 同一网络最大重连次数（仅对于传输介质是网络时有效），默认为3
		int m_byMaxReConn;
		//! 网络连接超时时间（单位为ms），超过此时间则认为连接失败，默认为2000ms
		int m_byConnTimeOut;

		int m_nSerAddrNum;					//! 服务器总个数

	protected:
		CIOTimer* m_pConnTimer;				//! 连接定时器

		std::string m_stSerAddrList[MAX_SERVERIP_COUNT];	//! 服务器地址列表
		int m_nPort;						//! 服务器端口号	

		int m_nConnTimes;					//! 试连接次数
		int m_nCurSerAddrIndex;				//! 当前连接的服务器序号
		int m_nMaxReSend;					//! 重发次数，仅对遥控遥调帧有效

		//! 协议类型：0-Modbus RTU；1-Modbus TCP；2-基于网络的Modbus RTU
		Byte m_byPrtclType;	
		
		//! 是否已连接标志
		bool m_bConnected;

		bool m_bCanWork;

		bool m_bIsEnglish;
	protected:
		//! 读取配置文件
		int ReadIniCfg();

	};	
}

#endif
