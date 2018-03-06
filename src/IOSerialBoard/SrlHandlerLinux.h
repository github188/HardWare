/*!
 *	\file	SrlHandlerLinux.h
 *
 *	\brief	用于类linux系统的反应式事件处理器
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月10日	13:20
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOSRLHANDLERFORLINUX_20080927_H
#define _IOSRLHANDLERFORLINUX_20080927_H

#include "ace/DEV_Addr.h"
#include "ace/DEV_Connector.h"
#include "ace/DEV_IO.h"
#include "ace/TTY_IO.h"
#include "ace/Reactor.h"
#include "ace/Svc_Handler.h"
#include "ace/Connector.h"
#include "RTDBCommon/OS_Ext.h"
#include "utilities/fepcommondef.h"


#define MAX_NET_PACKET_LENGTH			  4096
#define MAX_TOTAL_PACKET_LEN			  65535

typedef unsigned char byte;
namespace MACS_SCADA_SUD
{
	class CIOPrtclParserBase;

	/*!
	*	\class	CSrlSvcHandler
	*
	*	\brief	用于类linux系统的反应式事件处理器
	*
	* This class provides a well-defined interface that the
	* Acceptor and Connector pattern factories use as their target.
	* Typically, client applications will subclass ACE_Svc_Handler
	* and do all the interesting work in the subclass.  One thing
	* that the ACE_Svc_Handler does contain is a PEER_STREAM
	* endpoint that is initialized by an ACE_Acceptor or
	* ACE_Connector when a connection is established successfully.
	* This endpoint is used to exchange data between a
	* ACE_Svc_Handler and the peer it is connected with.
	*/
	class CSrlSvcHandler : public ACE_Svc_Handler</*ACE_DEV_STREAM*/ACE_TTY_IO, ACE_NULL_SYNCH>
	{
	public:
		/*!
		*	\brief	缺省构造函数
		*/
		CSrlSvcHandler();

		/*!
		*	\brief	缺省析构函数
		*/
		~CSrlSvcHandler();

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
		virtual int handle_output( ACE_HANDLE fd=ACE_INVALID_HANDLE );

		/*!
		*	\brief	向串口写数据
		*
		*	\param	pbyData 要写数据的头指针
		*	\param	nDataLen 要写数据的长度
		*
		*	\retval	0为成功，否则失败
		*/
		int Write( unsigned char * pbyData, int nDataLen );

		/*!
		*	\brief	初始化辅助参数
		*
		*	\param	pPrtcl	协议类指针
		*	\param	nTimeOut 超时时间
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		int Init( CIOPrtclParserBase* m_pPrtclParser, long nTimeOut );

	protected:
		//! 发送缓冲区
		unsigned char  m_pbySendData[MAX_NET_PACKET_LENGTH];			

		//! 协议类指针
		CIOPrtclParserBase* m_pPrtclParser;					
	};

	typedef ACE_Connector<CSrlSvcHandler, ACE_DEV_Connector> COM_CONNECTOR;
}

#endif