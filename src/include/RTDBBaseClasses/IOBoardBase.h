/*!
 *	\file	IOBoardBase.h
 *
 *	\brief	驱动器基类头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author Taiwanxia
 *
 *	\date	2014年4月10日	9:48
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOBOARDBASE_H_
#define _IOBOARDBASE_H_

#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/GenericFactory.h"
#include "utilities/fepcommondef.h"

#ifndef DBBASECLASSES_API
	#ifdef DBBASECLASSES_EXPORTS
	#define DBBASECLASSES_API MACS_DLL_EXPORT
	#else
	#define DBBASECLASSES_API MACS_DLL_IMPORT
	#endif
#endif

namespace MACS_SCADA_SUD
{
	class CIOPrtclParserBase;

	/*!
	 *	\class	CIOBoardBase
	 *
	 *	\brief	驱动器基类
	 *
	 *	详细的类说明（可选）
	 */
	class DBBASECLASSES_API CIOBoardBase
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOBoardBase();

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIOBoardBase();

		/*!
		 *	\brief	类初始化函数，创建类对象后必须首先调用该函数
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pPrtclParser	协议解析类指针
		 *	\param	tIOCfgPort		Port配置信息
		 *	\param	nTimeOut		超时时间
		 *
		 *	\retval	int		0为成功其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int Init( CIOPrtclParserBase* pPrtclParser, tIOPortCfg portCfg, long nTimeOut);

		/*!
		 *	\brief	通过链路发送数据
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pbyData		要发送数据首指针
		 *	\param	nDataLen	要发送数据长度
		 *
		 *	\retval	int		0为成功其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int Write( unsigned char * pbyData, int nDataLen );

		/*!
		*	\brief	使该IO驱动器开始工作
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int open( );

		/*!
		*	\brief	停止该IO驱动器
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int close();

		/*!
		*	\brief	启动定时器
		*
		*	\param	nTimeOut 定时器时间间隔
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int SetTimer( long nTimeOut );

		/*!
		*	\brief	停止定时器
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int StopTimer();

		/*!
		*	\brief	返回连接状态
		*
		*	\retval	连接正常为true 否则为false 
		*/
		virtual bool ConnState();

		/*!
		*	\brief	断开连接
		*/
		virtual int StopConn();

		/*!
		*	\brief	连接；连接地址和端口不变，用于重连
		*
		*	如果超过设定的超时时间还没有连接成功，则返回失败
		*
		*	\param	nTimeOut 超时时间
		*
		*	\retval	true表示连接成功，false表示连接失败
		*/
		virtual bool Connect( long nTimeOut );

		/*!
		*	\brief	连接；根据给定的连接地址和端口进行连接
		*
		*	如果超过设定的超时时间还没有连接成功，则返回失败
		*
		*	\param	nTimeOut	超时时间
		*	\param	pPeerIP		另端IP地址
		*	\param	nPort		连接端口
		*
		*	\retval	true表示连接成功，false表示连接失败
		*/
		virtual bool Connect( long nTimeOut, const char* pPeerIP, int nPort );

		/*!
		*	\brief	连接；连接地址和端口不变，用于重连
		*
		*	发送连接请求
		*
		*	\retval	int
		*
		*	\note	返回结果不表示成败，需要外部定时器辅助确定连接是否成功
		*/
		virtual int  Connect( );

		/*!
		*	\brief	连接；连接地址和端口不变，用于重连
		*
		*	发送连接请求
		*
		*	\retval	int
		*
		*	\note	返回结果不表示成败，需要外部定时器辅助确定连接是否成功
		*/
		virtual int  Connect( const char* pPeerIP, int nPort );

		/*!
		*	\brief	获取类名
		*
		*	\retval	std::string 类名
		*/
		std::string GetClassName();

	protected:
		//! 协议类指针
		CIOPrtclParserBase* m_pPrtclParser;	
		//! Port配置信息
		tIOPortCfg m_tDrvBoardCfg;					
		//! 超时定时器间隔
		long m_nTimeOut;

	public:
		//! 类名
		static std::string s_ClassName;
	};

	
#ifdef IODRVBOARD_EXPORTS
#define IODRVBOARD_VAR MACS_DLL_EXPORT
#else
#define IODRVBOARD_VAR MACS_DLL_IMPORT
#endif
	//!声明驱动器类工厂全局变量
	extern DBBASECLASSES_API GenericFactory<CIOBoardBase> g_IOBoardFactory;

#ifndef IODRVBOARD_EXPORTS
	extern "C" MACS_DLL_IMPORT int InitDll();
	extern "C" MACS_DLL_IMPORT int UnInitDll();
#endif

}

#endif