/*!
 *	\file	IOBoardImpl.h
 *
 *	\brief	调用类工厂，创建合适的驱动处理类
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月14日	8:55
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOBOARDIMPL_20080228_H_
#define _IOBOARDIMPL_20080228_H_

#include "RTDBBaseClasses/IOBoardBase.h"
#include "RTDBCommon/GenericFactory.h"

namespace MACS_SCADA_SUD
{
	class CIOPrtclParserBase;

	/*!
	 *	\class	CIOBoardImpl
	 *
	 *	\brief	驱动器实现类
	 *
	 *	根据配置文件中的主板类型配置，创建并调用不同的驱动器
	 */
	class CIOBoardImpl
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOBoardImpl();

		/*!
		 *	\brief	缺省析构函数
		 */
		~CIOBoardImpl();

		/*!
		 *	\brief	类初始化函数，创建类对象后必须首先调用该函数
		 *
		 *	根据配置信息创建具体的驱动器类
		 *
		 *	\param	pPrtclParser	协议解析类
		 *	\param	szBoardType		板卡类型，用于决定使用那个驱动类
		 *	\param	portCfg			端口配置信息
		 *	\param	nTimeOut		超时时间
		 *
		 *	\retval	0为成功，否则失败
		 *
		 *	\note		注意事项（可选）
		 */
		int Init( CIOPrtclParserBase* pPrtclParser, std::string szBoardType, tIOPortCfg portCfg, long nTimeOut);

		/*!
		 *	\brief	通过链路发送数据
		 *
		 *	\param	pbyData		要发送的数据内容
		 *	\param	nDataLen	要发送的数据的长度
		 *
		 *	\retval	0为成功，否则失败
		 */
		int Write( unsigned char* pbyData, int nDataLen );

		/*!
		 *	\brief	使该IO驱动器开始工作
		 *
		 *	\retval	0为成功，否则失败
		 */
		int open ( );

		/*!
		 *	\brief	停止该IO驱动器
		 *
		 *	\retval	0为成功，否则失败
		 */
		int close ( );

		/*!
		 *	\brief	启动定时器
		 *
		 *	\param	nTimeOut 定时器时间间隔
		 *
		 *	\retval	0为成功，否则失败
		 */
		int SetTimer( long nTimeOut );

		/*!
		 *	\brief	停止定时器
		 *
		 *	\retval	0为成功，否则失败
		 */
		int StopTimer();

		/*!
		 *	\brief	返回连接状态
		 *
		 *	\retval	连接正常为true 否则为false 
		 */
		bool ConnState();

		/*!
		 *	\brief	断开连接
		 *
		 *	\retval	0为成功，否则失败
		 */
		int StopConn();

		/*!
		 *	\brief	连接；连接地址和端口不变，用于重连
		 *
		 *	如果超过设定的超时时间还没有连接成功，则返回失败
		 *
		 *	\param	nTimeOut 超时时间
		 *
		 *	\retval	true表示连接成功，false表示连接失败
		 */
		bool Connect( long nTimeOut );

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
		bool Connect( long nTimeOut, const char* pPeerIP, int nPort );

		/*!
		 *	\brief	连接；连接地址和端口不变，用于重连
		 *
		 *	发送连接请求
		 *
		 *	\retval	int
		 *
		 *	\note	返回结果不表示成败，需要外部定时器辅助确定连接是否成功
		 */
		int  Connect( );

		/*!
		 *	\brief	连接；连接地址和端口不变，用于重连
		 *
		 *	发送连接请求
		 *
		 *	\retval	int
		 *
		 *	\note	返回结果不表示成败，需要外部定时器辅助确定连接是否成功
		 */
		int  Connect( const char* pPeerIP, int nPort );

	protected:
		//!驱动器对象指针,根据不同配置来创建
		CIOBoardBase* m_pBoardBase;
	};
}


#endif