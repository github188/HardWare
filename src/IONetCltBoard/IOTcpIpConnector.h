/*!
 *	\file	IOTcpIpConnector.h
 *
 *	\brief	������linuxϵͳ������������Ӧʽ)
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��11��	10:49
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
	 *	\brief	��Ӧʽ��������������linuxϵͳ��
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class CIOTcpIpConnector : public ACE_Connector<CIOTcpIpSvcHandler, ACE_SOCK_CONNECTOR>
	{
	public:
		/*!
		*	\brief	ȱʡ���캯��
		*/
        CIOTcpIpConnector();

		/*!
		*	\brief	ȱʡ��������
		*/
		~CIOTcpIpConnector();
	public:
		/*!
		*	\brief	����������
		*
		* Bridge method for creating a SVC_HANDLER.  The default is to
		* create a new SVC_HANDLER only if <sh> == 0, else <sh> is
		* unchanged.  However, subclasses can override this policy to
		* perform SVC_HANDLER creation in any way that they like (such as
		* creating subclass instances of SVC_HANDLER, using a singleton,
		* dynamically linking the handler, etc.).  Returns -1 if failure,
		* else 0.	
		*
		*	\param	CIOTcpIpSvcHandler	������ָ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int make_svc_handler (CIOTcpIpSvcHandler* pHandler);

		/*!
		*	\brief	��ʼ����������
		*
		*	\param	pPrtclParser	Э����ָ��
		*	\param	lTimeout		��ʱʱ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		int Init( CIOPrtclParserBase* pPrtclParser );

		/*!
		*	\brief	���Handler
		*
		*	\retval	CIOTcpIpSvcHandler	������ָ��
		*/
		CIOTcpIpSvcHandler* GetHandler();

		/*!
		*	\brief	�������ӵķ�������ַ
		*
		*	\param	port	�˿ں�
		*	\param	pchIP	IP��ַ
		*
		*	\retval	void
		*/
		void SetServerAddr(int port, const char* pchIP);

		/*!
		*	\brief	�����ж�ʱ����
		*
		*	\retval	int 0��ʾ�ɹ��������ʧ��
		*/
		int ConnInterrupt();

		/*!
		*	\brief	���ӣ����ӵ�ַ�Ͷ˿ڲ��䣬��������
		*
		*	��������趨�ĳ�ʱʱ�仹û�����ӳɹ����򷵻�ʧ��
		*
		*	\param	nTimeOut ��ʱʱ��
		*
		*	\retval	true��ʾ���ӳɹ���false��ʾ����ʧ��
		*/
		bool Connect( long nTimeOut );

		/*!
		*	\brief	���ӣ����ݸ��������ӵ�ַ�Ͷ˿ڽ�������
		*
		*	��������趨�ĳ�ʱʱ�仹û�����ӳɹ����򷵻�ʧ��
		*
		*	\param	nTimeOut	��ʱʱ��
		*	\param	pPeerIP		���IP��ַ
		*	\param	nPort		���Ӷ˿�
		*
		*	\retval	true��ʾ���ӳɹ���false��ʾ����ʧ��
		*/
		bool Connect( long nTimeOut, const char* pPeerIP, int nPort );

		/*!
		*	\brief	���ӣ����ӵ�ַ�Ͷ˿ڲ��䣬��������
		*
		*	������������
		*
		*	\retval	int
		*
		*	\note	���ؽ������ʾ�ɰܣ���Ҫ�ⲿ��ʱ������ȷ�������Ƿ�ɹ�
		*/
		int  Connect( );

		/*!
		*	\brief	���ӣ����ӵ�ַ�Ͷ˿ڲ��䣬��������
		*
		*	������������
		*
		*	\retval	int
		*
		*	\note	���ؽ������ʾ�ɰܣ���Ҫ�ⲿ��ʱ������ȷ�������Ƿ�ɹ�
		*/
		int  Connect( const char* pPeerIP, int nPort );
		
		/*!
		*	\brief	�Ƿ���������
		*
		*	\retval	bool �����������򷵻�true�����򷵻�false
		*/
		bool IsConnected();

	private:
		CIOPrtclParserBase* m_pPrtclParser;		//! Э����ָ��
		bool m_bConnected;						//! ��������־

		char m_pchSerAddr[16];					//! ��������ַ
		int m_nPort;							//! ���Ӷ˿�

		CIOTcpIpSvcHandler* m_pHandler;			//! ������ָ��
	};
}

#endif
