/*!
 *	\file	$FILE_BASE$.$FILE_EXT$
 *
 *	\brief	104主站驱动类头文件
 *
 *	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Drv.h, 23+1 2011/04/26 05:11:09 miaoweijie $
 *	$Author: miaoweijie $
 *	$Date: 2011/04/26 05:11:09 $
 *	$Revision: 23+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef IOMACSIEC104DRIVER_20080812_H
#define IOMACSIEC104DRIVER_20080812_H

#include "FepIEC104DrvH.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"

namespace MACS_SCADA_SUD
{	
	class IOMACSIEC104_API CFepIEC104Drv :
		public CIODriver
	{
	public:
		~CFepIEC104Drv();

		/*!
		*	\brief	获得类名
		*
		*	\retval	std::string 类名
		*/
		std::string GetClassName();

		/*!
		*	\brief	得到继承PrtclParserBase基类的继承类名
		*
		*	\retval	协议类名
		*/
		virtual std::string GetPrtclClassName();

		/*!
		*	\brief	得到继承DeviceBase基类的继承类名
		*
		*	\retval	设备类名
		*/
		virtual std::string GetDeviceClassName();
		
		/*!
		*	\brief	开始工作
		*
		*	\retval	返回值：0--成功，其它失败
		*/
		virtual OpcUa_Int32 StartWork(void);

		/*!
		*	\brief	初始化网络连接参数
		*
		*	\retval	返回值：0--成功，其它失败
		*/
		OpcUa_Int32 InitNetParam();

		/*!
		*	\brief	添加服务器
		*
		*	\param	pchSerAddr	要添加的服务器地址
		*
		*	\retval	void
		*/
		void AddSerAddr(const char* pchSerAddr);

		/*!
		*	\brief	封装一下连接操作
		*
		*	\param	nIndex	服务器序号
		*
		*	\retval	OpcUa_Int32
		*/
		OpcUa_Int32 Connect(OpcUa_Int32 nIndex = 0);

		/*!
		*	\brief	设置当前设备号
		*
		*	\param	nIndex	服务器序号
		*
		*	\retval	OpcUa_Int32 为0表示设置成功，否则表示失败
		*/
		OpcUa_Int32 SetCurDevIndex(OpcUa_Int32 nIndex)
		{
			if( nIndex < 0 || nIndex >= GetDeviceCount() )
				return -1;

			m_iCurDevIndex = nIndex;
			return 0;
		};

	public:
		inline bool IsConnecting()
		{
			return m_bIsConnecting;
		}
		inline void IsConnecting(bool bIsConnecting)
		{
			m_bIsConnecting = bIsConnecting;
		}

		//! 复位试连接次数
		inline void ResetConnTimes()
		{
			m_nConnTimes = 0;
		}
		//! 增加一次试连接次数
		inline void IncrConnTimes()
		{
			m_nConnTimes++;
		}
		//! 获取试连接次数
		inline OpcUa_Int32 GetConnTimes()
		{
			return m_nConnTimes;
		}

		//! 指向下一个服务器
		inline OpcUa_Int32 NextSerAddrIndex()
		{
			++m_nCurSerAddrIndex;
			if( m_nCurSerAddrIndex >= m_nSerAddrNum)
				m_nCurSerAddrIndex = 0;

			return m_nCurSerAddrIndex;
		}

		//! 设置连接定时器
		inline OpcUa_Int32 SetConnTimer(OpcUa_UInt32 nElapse)
		{
			if(m_pConnTimer != NULL)
				return m_pConnTimer->SetTimer(nElapse);
			else
				return -1;
		}

		//! 关闭连接定时器
		inline OpcUa_Int32 KillConnTimer()
		{
			if(m_pConnTimer != NULL)
				return m_pConnTimer->KillTimer();
			else
				return -1;
		}

		//! 置是否连接上的标志
		inline void Connected(bool bConned)
		{
			m_bHasConnected = bConned;
		};

		//! 是否连接上
		inline bool Connected(void)
		{
			return m_bHasConnected;
		};

		/*!
		*	\brief	重连
		*
		*	如果连接失败，则重连
		*	单个服务器连续m_byMaxReConn次连接失败则换下一个服务器
		*
		*/
		void ReConnect();

		OpcUa_Int32 WriteLogAnyWay( const char* pchLog, bool bTime = true );

		//! 获得当前连接的服务器地址
		std::string GetCurServerIP()
		{
			return m_stSerAddrList[m_nCurSerAddrIndex];
		};

		//! 声明友元函数以使这些函数能访问本类的私有数据成员
		friend void OnConnectTimeOut(void* p);							//! 定时器超时回调函数


	public:
		//! 类名
		static std::string s_ClassName;	
		//! 用于重连机制的锁
		ACE_Recursive_Thread_Mutex m_Mutex;

	private:				
		CIOTimer* m_pConnTimer;								//! 连接定时器
		
		std::string m_stSerAddrList[4];						//! 服务器地址列表	
		OpcUa_Int32 m_nPort;										//! 服务器端口号

		OpcUa_Int32 m_nConnTimes;									//! 试连接次数
		OpcUa_Int32 m_nSerAddrNum;									//! 服务器总个数
		OpcUa_Int32 m_nCurSerAddrIndex;								//! 当前连接的服务器序号

		bool m_bIsConnecting;							//! 是否正在连接
		bool m_bHasConnected;							//! 已经连接上的标志
	};
}

#endif
