/*!
 *	\file	IOTcpIpHandler.h
 *
 *	\brief	����windowsϵͳ��ǰ��ʽ�¼�������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��11��	8:54
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOTCPIPHANDLER_H_20080901_
#define _IOTCPIPHANDLER_H_20080901_

#include "ace/Asynch_IO.h"
#include "ace/OS.h"
#include "ace/Message_Block.h"
#include "ace/Proactor.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "RTDBCommon/OS_Ext.h"
//#include "opcua_platformdefs.h"
//#include "opcua_baseobjecttype.h"

typedef unsigned char OpcUa_Byte;

namespace MACS_SCADA_SUD
{
	class CIONetSynConnector;
	class CIOPrtclParserBase;

	/*!
	 *	\class	CIOTcpIpHandler
	 *
	 *	\brief	ǰ��ʽ�¼���������Ŀǰ������windowsϵͳ��
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class CIOTcpIpHandler : public ACE_Service_Handler
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOTcpIpHandler(void);

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIOTcpIpHandler ()
		{
			if (this->handle () != ACE_INVALID_HANDLE)
			{
				ACE_OS::closesocket (this->handle ());
			}
		}

	public:
		//!��ͨ���������ݶ�ȡ
		virtual void open (ACE_HANDLE h, ACE_Message_Block&);

		/*!
		*	\brief	��ȡ������ɺ�ĵ��ú���
		*
		*	This method will be called when an asynchronous read completes on a stream.
		*
		*	\param	result	��¼�˶�ȡ���������ݺ������Ϣ
		*
		*	\retval	void
		*/
		virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);

		/*!
		*	\brief	д������ɺ�ĵ��ú���
		*
		*	This method will be called when an asynchronous write completes on a stream.
		*
		*	\param	result	��¼�˶�ȡ���������ݺ������Ϣ
		*
		*	\retval	void
		*
		*	\note	һ��Ҫ�ڴ˺�����������ݿռ���ͷţ����������ڴ�й¶
		*/
		virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result &result);

		/*!
		*	\brief	��ʼ����������
		*
		*	\param	pPrtcl	Э����ָ��
		*	\param	nTimeOut ��ʱʱ��
		*	\param	pParent �������ص�ָ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		virtual int Init( CIOPrtclParserBase* pPrtcl, CIONetSynConnector* pParent );

		//!�������ݵĴ���
		virtual int RxDataProc( ACE_Message_Block& input_msg );

		/*!
		*	\brief	��������
		*
		*	\param	pbyData Ҫд���ݵ�ͷָ��
		*	\param	nDataLen Ҫд���ݵĳ���
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		int Write( const Byte* pbyData, int nDataLen );

	private:
		ACE_Asynch_Read_Stream reader_;				//! �첽��������д����ֻ���ڶ�
		ACE_Asynch_Write_Stream writer_;			//! �첽��������д����ֻ����д

		CIOPrtclParserBase* m_pPrtclParser;			//! Э����ָ��
		CIONetSynConnector* m_pParent;				//! �������ص�ָ��
	};
}


#endif