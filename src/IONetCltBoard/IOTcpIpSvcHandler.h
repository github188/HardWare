/*!
 *	\file	IOTcpIpSvcHandler.h
 *
 *	\brief	用于类linux系统的反应式事件处理器
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	10:11
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOTCPIPSVCHANDLER_H_20080901_
#define _IOTCPIPSVCHANDLER_H_20080901_

#include "RTDBCommon/OS_Ext.h"
#include "ace/Svc_Handler.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/INET_Addr.h"
#include "ace/Reactor_Notification_Strategy.h"
#include "ace/OS.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"

typedef unsigned char OpcUa_Byte;

namespace MACS_SCADA_SUD
{
	class CIOPrtclParserBase;
	class CIOTcpIpConnector;

	/*!
	*	\class	CIOTcpIpSvcHandler
	*
	*	\brief	用于类linux系统的反应式事件处理器
	*/
	class CIOTcpIpSvcHandler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOTcpIpSvcHandler();

		/*!
		 *	\brief	缺省析构函数
		 */
		~CIOTcpIpSvcHandler();

	public:
		/*!
		*	\brief	Activate the client handler.  
		*	This is typically called by the ACE_Acceptor or ACE_Connector.
		*
		*	\param	p	此处为串口参数
		*
		*	\retval	int	0表示成功，否则表示失败
		*/
		virtual int open( void* p );

		/**
		* Perform termination activities on the SVC_HANDLER.  The default
		* behavior is to close down the <peer_> (to avoid descriptor leaks)
		* and to <destroy> this object (to avoid memory leaks)!  If you
		* don't want this behavior make sure you override this method...
		*/
		virtual int handle_close( ACE_HANDLE handle, ACE_Reactor_Mask close_mask );

		/*!
		*	\brief	有数据来时的回调函数
		*
		*	读取数据操作要自己完成，然后才能处理
		*
		*	\param	fd
		*
		*	\retval	int 0表示成功，否则表示失败
		*/
		virtual int handle_input( ACE_HANDLE fd=ACE_INVALID_HANDLE );

		/*!
		*	\brief	有数据发送之后的回调函数
		*
		*	此处不做任何操作
		*
		*	\param	fd
		*
		*	\retval	int 0表示成功，否则表示失败
		*/
		virtual int handle_output( ACE_HANDLE fd = ACE_INVALID_HANDLE );

		/*!
		*	\brief	初始化辅助参数
		*
		*	\param	pPrtcl	协议类指针
		*	\param	nTimeOut 超时时间
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		virtual int Init( CIOPrtclParserBase* pPrtclParser, CIOTcpIpConnector* pParent );

		/*!
		*	\brief	发送数据
		*
		*	\param	pbyData 要写数据的头指针
		*	\param	nDataLen 要写数据的长度
		*
		*	\retval	0为成功，否则失败
		*/
		int Write( const OpcUa_Byte* pbyData, int nDataLen );

	private:
		CIOPrtclParserBase* m_pPrtclParser;		//! 协议类指针
		CIOTcpIpConnector* m_pParent;			//! 连接器回调指针
	};
}

#endif