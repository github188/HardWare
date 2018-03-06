/*!
 *	\file	IOTcpIpConnector.h
 *
 *	\brief	用于类linux系统的连接器（反应式)
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	10:49
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOTCPIPCONNECTOR_H
#define	_IOTCPIPCONNECTOR_H

#include "ace/Connector.h"
#include "ace/SOCK_Connector.h"
#include "ace/Thread_Mutex.h"


namespace MACS_SCADA_SUD
{
	class CIOPrtclParserBase;
	class CIOTcpIpSvcHandler;

	/*!
	 *	\class	CIOTcpIpConnector
	 *
	 *	\brief	反应式连接器（用于类linux系统）
	 *
	 *	详细的类说明（可选）
	 */
	class CIOTcpIpConnector : public ACE_Connector<CIOTcpIpSvcHandler, ACE_SOCK_CONNECTOR>
	{
	public:
		/*!
		*	\brief	缺省构造函数
		*/
        CIOTcpIpConnector();

		/*!
		*	\brief	缺省析构函数
		*/
		~CIOTcpIpConnector();
	public:
		/*!
		*	\brief	创建处理器
		*
		* Bridge method for creating a SVC_HANDLER.  The default is to
		* create a new SVC_HANDLER only if <sh> == 0, else <sh> is
		* unchanged.  However, subclasses can override this policy to
		* perform SVC_HANDLER creation in any way that they like (such as
		* creating subclass instances of SVC_HANDLER, using a singleton,
		* dynamically linking the handler, etc.).  Returns -1 if failure,
		* else 0.	
		*
		*	\param	CIOTcpIpSvcHandler	处理器指针
		*
		*	\retval	int	0表示成功，否则表示失败
		*/
		virtual int make_svc_handler (CIOTcpIpSvcHandler* pHandler);

		/*!
		*	\brief	初始化辅助参数
		*
		*	\param	pPrtclParser	协议类指针
		*	\param	lTimeout		超时时间
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		int Init( CIOPrtclParserBase* pPrtclParser );

		/*!
		*	\brief	获得Handler
		*
		*	\retval	CIOTcpIpSvcHandler	处理器指针
		*/
		CIOTcpIpSvcHandler* GetHandler();

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

		CIOTcpIpSvcHandler* m_pHandler;			//! 处理器指针
	};
}

#endif
