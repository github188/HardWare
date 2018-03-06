/*!
 *	\file	IONetSynConnector.h
 *
 *	\brief	����windowsϵͳ����������ǰ��ʽ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��11��	11:45
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
	 *	\brief	ǰ��ʽ������������windowsϵͳ���첽��
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class CIONetSynConnector : public ACE_Asynch_Connector<CIOTcpIpHandler>
	{
	public:
		/*!
		*	\brief	ȱʡ���캯��
		*/
        CIONetSynConnector();

		/*!
		*	\brief	ȱʡ��������
		*/
		~CIONetSynConnector();

	public:
		/*!
		*	\brief	������ɺ�Ļص�����
		*
		*	This is called when an outstanding accept completes.
		*
		*	\param	result ���ӽ��
		*
		*	\retval	void
		*/
		virtual void handle_connect (const ACE_Asynch_Connect::Result &result);

		/*!
		*	\brief	����������
		*
		*	This is the template method used to create new handler.
		*	Subclasses must overwrite this method if a new handler creation
		*	strategy is required.
		*
		*	\param	void
		*
		*	\retval	CIOTcpIpHandler	�����Ĵ�����ָ��
		*/
		virtual CIOTcpIpHandler* make_handler (void);

		/*!
		*	\brief	��ʼ����������
		*
		*	\param	pPrtclParser	Э����ָ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		int Init( CIOPrtclParserBase* pPrtclParser );

		/*!
		*	\brief	���Handler
		*
		*	\retval	CIOTcpIpSvcHandler	������ָ��
		*/
		CIOTcpIpHandler* GetHandler();

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

		CIOTcpIpHandler* m_pHandler;			//! ������ָ��
	};
}

#endif