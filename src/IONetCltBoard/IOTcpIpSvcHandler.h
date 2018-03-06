/*!
 *	\file	IOTcpIpSvcHandler.h
 *
 *	\brief	������linuxϵͳ�ķ�Ӧʽ�¼�������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��11��	10:11
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
	*	\brief	������linuxϵͳ�ķ�Ӧʽ�¼�������
	*/
	class CIOTcpIpSvcHandler : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOTcpIpSvcHandler();

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIOTcpIpSvcHandler();

	public:
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
		virtual int handle_output( ACE_HANDLE fd = ACE_INVALID_HANDLE );

		/*!
		*	\brief	��ʼ����������
		*
		*	\param	pPrtcl	Э����ָ��
		*	\param	nTimeOut ��ʱʱ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		virtual int Init( CIOPrtclParserBase* pPrtclParser, CIOTcpIpConnector* pParent );

		/*!
		*	\brief	��������
		*
		*	\param	pbyData Ҫд���ݵ�ͷָ��
		*	\param	nDataLen Ҫд���ݵĳ���
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		int Write( const OpcUa_Byte* pbyData, int nDataLen );

	private:
		CIOPrtclParserBase* m_pPrtclParser;		//! Э����ָ��
		CIOTcpIpConnector* m_pParent;			//! �������ص�ָ��
	};
}

#endif