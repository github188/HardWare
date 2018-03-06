/*!
 *	\file	IONetSynConnector.h
 *
 *	\brief	用于windows系统的连接器（前摄式）
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	11:45
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IONETSYNCONNECTOR_H
#define	_IONETSYNCONNECTOR_H

#include "ace/Asynch_Connector.h"
#include "ace/Mutex.h"


namespace MACS_SCADA_SUD
{
	class CIOPrtclParserBase;
	class CIOTcpIpHandler;

	/*!
	 *	\class	CIONetSynConnector
	 *
	 *	\brief	前摄式连接器（用于windows系统，异步）
	 *
	 *	详细的类说明（可选）
	 */
	class CIONetSynConnector : public ACE_Asynch_Connector<CIOTcpIpHandler>
	{
	public:
		/*!
		*	\brief	缺省构造函数
		*/
        CIONetSynConnector();

		/*!
		*	\brief	缺省析构函数
		*/
		~CIONetSynConnector();

	public:
		/*!
		*	\brief	连接完成后的回调函数
		*
		*	This is called when an outstanding accept completes.
		*
		*	\param	result 连接结果
		*
		*	\retval	void
		*/
		virtual void handle_connect (const ACE_Asynch_Connect::Result &result);

		/*!
		*	\brief	创建处理器
		*
		*	This is the template method used to create new handler.
		*	Subclasses must overwrite this method if a new handler creation
		*	strategy is required.
		*
		*	\param	void
		*
		*	\retval	CIOTcpIpHandler	创建的处理器指针
		*/
		virtual CIOTcpIpHandler* make_handler (void);

		/*!
		*	\brief	初始化辅助参数
		*
		*	\param	pPrtclParser	协议类指针
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		int Init( CIOPrtclParserBase* pPrtclParser );

		/*!
		*	\brief	获得Handler
		*
		*	\retval	CIOTcpIpSvcHandler	处理器指针
		*/
		CIOTcpIpHandler* GetHandler();

		/*!
		*	\brief	设置连接的服务器地址
		*
		*	\param	port	端口号
		*	\param	pchIP	IP地址
		*
		*	\retval	void
		*/
		void SetServerAddr(int port, const char* pchIP);

		/*!
		*	\brief	连接中断时调用
		*
		*	\retval	int 0表示成功，否则表失败
		*/
		int ConnInterrupt();

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

		/*!
		*	\brief	是否已连接上
		*
		*	\retval	bool 并连接正常则返回true，否则返回false
		*/
		bool IsConnected();

	private:
		CIOPrtclParserBase* m_pPrtclParser;		//! 协议类指针
		bool m_bConnected;						//! 连接与否标志

		char m_pchSerAddr[16];					//! 服务器地址
		int m_nPort;							//! 连接端口

		CIOTcpIpHandler* m_pHandler;			//! 处理器指针
	};
}

#endif