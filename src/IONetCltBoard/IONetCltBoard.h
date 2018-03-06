/*!
 *	\file	IONetCltBoard.h
 *
 *	\brief	以太网客户端的驱动器类头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	15:46
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IONETCLTBOARD_20080228_H
#define _IONETCLTBOARD_20080228_H

#include "RTDBCommon/OS_Ext.h"
#include "RTDBBaseClasses/IOBoardBase.h"
#include "IONetCltBoardThread.h"

#ifdef IONETCLTBOARD_EXPORTS
#define IONETCLTBOARD_API MACS_DLL_EXPORT
#else
#define IONETCLTBOARD_API MACS_DLL_IMPORT
#endif

namespace MACS_SCADA_SUD
{
	/*!
	 *	\class	CIONetCltBoard
	 *
	 *	\brief	以太网客户端的访问接口封装
	 *
	 *	提供了建立连接、断开连接、发送数据、获取连接状态的接口，本库只封装了单条网络
	 *	Windows平台是基于前摄器实现的；Linux平台是基于反应器实现的。
	 */
	class IONETCLTBOARD_API CIONetCltBoard : public CIOBoardBase
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIONetCltBoard();

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIONetCltBoard();

		/*!
		*	\brief	发送数据
		*
		*	\param	pbyData：要发送的数据的首指针
		*	\param	nDataLen：要发送的数据的长度
		*
		*	\retval	0为成功，其他为失败
		*/
		virtual int Write( Byte* pbyData, int nDataLen );

		/*!
		*	\brief	打开驱动器
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int open( );

		/*!
		*	\brief	关闭驱动器
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int close( );

		/*!
		*	\brief	返回连接状态
		*
		*	\retval	连接正常为true 否则为false 
		*/
		virtual bool ConnState();

		/*!
		*	\brief	断开连接
		*
		*	\retval	int 0表示成功，否则表示失败
		*/
		virtual int StopConn();

		/*!
		*	\brief	建立连接
		*
		*	\param	nTimeOut：连接超时时间
		*
		*	\retval	true为成功，false为失败
		*/
		virtual bool Connect( long nTimeOut );	

		/*!
		*	\brief	建立连接，用于重连
		*
		*	\param	nTimeOut：连接超时时间
		*	\param	pPeerIP：要连接的服务端的IP地址
		*	\param	nPort：端口号
		*
		*	\retval	true为成功，false为失败
		*/
		virtual bool Connect( long nTimeOut, const char* pPeerIP, int nPort );

		/*!
		*	\brief	建立连接，返回结果不表示成败
		*
		*	\retval	int
		*/
		virtual int  Connect( );

		/*!
		*	\brief	建立连接，用于重连
		*
		*	\param	pPeerIP：要连接的服务端的IP地址
		*	\param	nPort：端口号
		*
		*	\retval	int
		*/
		virtual int  Connect( const char* pPeerIP, int nPort );

		//! 本模块是否能输出调试信息
		static bool CanOutput();

		//! 输出调试信息
		static void OutputDebug( const char* pchDebugInfo );

		//! 获取类名
		std::string GetClassName() ;

	private:
		//! 处理线程指针
		CIONetCltBoardThread *m_pComDealThd;	

	public:
		//! 类名
		static std::string s_ClassName;	
	};

#ifndef IONETCLTBOARD_EXPORTS
	extern "C" MACS_DLL_IMPORT int InitDll();
	extern "C" MACS_DLL_IMPORT int UnInitDll();
#endif
}

#endif