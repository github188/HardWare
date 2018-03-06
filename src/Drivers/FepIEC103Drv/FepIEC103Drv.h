/*!
*	\file	FepIEC103Drv.h
*
*	\brief	103主站驱动类头文件
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Drv.h 1     11-01-21 10:02a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-01-21 10:02a $
*	$Revision: 1 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef _FEP_IEC103Drv_20110110_H_
#define _FEP_IEC103Drv_20110110_H_

#include "PublicFileH.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"
namespace MACS_SCADA_SUD
{
	class CPRDCfgParser;

	/*!
	*	\class	CFepIEC103Drv
	*
	*	\brief	IEC103驱动管理类
	*/
	class FEPIEC103_API CFepIEC103Drv 
		:public CIODriver
	{
		friend void OnTmOut(void* p);		
	public:
		CFepIEC103Drv();
		//! 缺省析构函数
		~CFepIEC103Drv();

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
		virtual OpcUa_Int32 StartWork(void);

		/*!
		*	\brief	打开通道
		*
		*	设置通道参数，并打开通道，启动驱动运行线程
		*
		*	\param	portCfg	记录通道详细设置的结构体
		*
		*	\retval	OpcUa_Int32	成功则返回0
		*/
		virtual OpcUa_Int32 OpenChannel();


		//! 初始化网络连接参数
		OpcUa_Int32 InitNetParam();

		//! 添加服务器
		void AddSerAddr(const OpcUa_CharA* pchSerAddr);

		//! 封装一下连接操作，参数为服务器序号
		void Connect(OpcUa_Int32 iIndex = 0);	

		//! 获得服务IP
		std::string GetSerAddr( OpcUa_Int32 Num );

		//! 获得服务IP地址总个数
		OpcUa_Int32 GetSerAddrNum();

		//! 获得当前连接的服务器IP序号
		OpcUa_Int32 GetCurSerAddrIndex();

		//! 获得试连接次数
		OpcUa_Int32 GetConnTimes();

		//! 置试连接次数
		inline void SetConnTime(OpcUa_Int32 nTime)
		{
			m_iConnTimes = nTime;
		};

		//! 试连接次数加1
		inline void IncrConnTime()
		{
			m_iConnTimes ++;
		};

		/*!
		*	\brief	获得连接定时器指针
		*/
		inline CIOTimer* GetConnTimer()
		{
			return m_pConnTimer;
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

		/*!
		*	\brief	获得当前连接的服务器IP
		*/
		std::string GetCurServerAddr()
		{
			return m_strSerAddrList[m_iCurSerAddrIndex];
		};

		//! 获得日志指针
		CIOLog* GetLog()
		{
			return m_pLog;
		}

		//! 直接记录日志
		OpcUa_Int32 WriteLogAnyWay( const OpcUa_CharA* pchLog, bool bTime = true );

		bool CanWork();

		CPRDCfgParser* GetPRDCfgParser();

	public:
		//! 类名
		static std::string s_ClassName;
		std::string m_strSerAddrList[4];				//!< 服务器地址列表	
		OpcUa_Int32 m_iPort;
		OpcUa_Int32 m_iConnTimes;						//! 试连接次数
		OpcUa_Int32 m_iSerAddrNum;						//! 服务器总个数
		OpcUa_Int32 m_iCurSerAddrIndex;					//! 当前连接的服务器序号
		CIOTimer* m_pConnTimer;
		//! 用于重连机制的锁
		ACE_Recursive_Thread_Mutex m_Mutex;
		bool m_bIsConnecting;
	private:
		CPRDCfgParser* m_pPrdCfgParser;					//!故障录播配置文件解析对象

		bool m_bCanWork;	
		
	};
}

#endif
