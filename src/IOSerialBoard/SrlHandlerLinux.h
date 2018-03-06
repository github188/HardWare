/*!
 *	\file	SrlHandlerLinux.h
 *
 *	\brief	������linuxϵͳ�ķ�Ӧʽ�¼�������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	13:20
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
	*	\brief	������linuxϵͳ�ķ�Ӧʽ�¼�������
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
		*	\brief	ȱʡ���캯��
		*/
		CSrlSvcHandler();

		/*!
		*	\brief	ȱʡ��������
		*/
		~CSrlSvcHandler();

		/*!
		*	\brief	Activate the client handler.  
		*	This is typically called by the ACE_Acceptor or ACE_Connector.
		*
		*	\param	p	�˴�Ϊ���ڲ���
		*
		*	\retval	int	0��ʾ�ɹ��������ʾʧ��
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
		*	\brief	��������ʱ�Ļص�����
		*
		*	��ȡ���ݲ���Ҫ�Լ���ɣ�Ȼ����ܴ���
		*
		*	\param	fd
		*
		*	\retval	int 0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int handle_input( ACE_HANDLE fd=ACE_INVALID_HANDLE );

		/*!
		*	\brief	�����ݷ���֮��Ļص�����
		*
		*	�˴������κβ���
		*
		*	\param	fd
		*
		*	\retval	int 0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int handle_output( ACE_HANDLE fd=ACE_INVALID_HANDLE );

		/*!
		*	\brief	�򴮿�д����
		*
		*	\param	pbyData Ҫд���ݵ�ͷָ��
		*	\param	nDataLen Ҫд���ݵĳ���
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		int Write( unsigned char * pbyData, int nDataLen );

		/*!
		*	\brief	��ʼ����������
		*
		*	\param	pPrtcl	Э����ָ��
		*	\param	nTimeOut ��ʱʱ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		int Init( CIOPrtclParserBase* m_pPrtclParser, long nTimeOut );

	protected:
		//! ���ͻ�����
		unsigned char  m_pbySendData[MAX_NET_PACKET_LENGTH];			

		//! Э����ָ��
		CIOPrtclParserBase* m_pPrtclParser;					
	};

	typedef ACE_Connector<CSrlSvcHandler, ACE_DEV_Connector> COM_CONNECTOR;
}

#endif