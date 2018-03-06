/*!
 *	\file	IOSrlBoardHandler.h
 *
 *	\brief	����windowsϵͳ��ǰ��ʽ�¼�������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	11:12
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOSRLBOARDHANDLER_20080927_H
#define _IOSRLBOARDHANDLER_20080927_H

#include "ace/Asynch_IO.h"
#include "ace/OS.h"
#include "ace/Message_Block.h"
#include "ace/DEV_Connector.h"
#include "ace/TTY_IO.h"
#include "ace/Proactor.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "RTDBCommon/OS_Ext.h"
#include "utilities/fepcommondef.h"


#define MAX_BUF_SIZE 4096

namespace MACS_SCADA_SUD
{
	class CIOPrtclParserBase;

	/*!
	 *	\class	CIOSrlBoardHandler
	 *
	 *	\brief	ǰ��ʽ�¼���������Ŀǰ������windowsϵͳ��
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class CIOSrlBoardHandler : public ACE_Handler
	{
		public:
		/*!
		*	\brief	ȱʡ���캯��
		*/
		CIOSrlBoardHandler();

		/*!
		*	\brief	ȱʡ��������
		*/
		virtual ~CIOSrlBoardHandler();

		/*!
		*	\brief	�򿪶˿ڣ�����������
		*
		*	\param	portCfg	���ڲ���
		*
		*	\retval	int 0Ϊ�ɹ�������ʧ��
		*/
		virtual int open( tIOPortCfg portCfg);

		/*!
		*	\brief	��ȡ������ɺ�ĵ��ú���
		*
		*	This method will be called when an asynchronous read completes on a stream.
		*
		*	\param	result	��¼�˶�ȡ���������ݺ������Ϣ
		*
		*	\retval	void
		*/
		virtual void handle_read_stream( 
			const ACE_Asynch_Read_Stream::Result& result );

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
		virtual void handle_write_stream( 
			const ACE_Asynch_Write_Stream::Result& result );

		/*!
		*	\brief	�򴮿�д����
		*
		*	\param	pbyData Ҫд���ݵ�ͷָ��
		*	\param	nDataLen Ҫд���ݵĳ���
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual int Write( unsigned char * pbyData, int nDataLen );

		/*!
		*	\brief	��ʼ����������
		*
		*	\param	pPrtcl	Э����ָ��
		*	\param	nTimeOut ��ʱʱ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		virtual int Init( CIOPrtclParserBase* m_pPrtclParser, long nTimeOut );

		/*!
		*	\brief	��װһ�½�����Ӧ����������������ʱ�ڴ˺����е���Э�����е�RxDataProc
		*
		*	\param	input_msg	���ܵ�������
		*	\param	bStopTimer	�Ƿ�ֹͣ��ʱ������
		*	\param	bSendData	������ɺ��Ƿ��ٴη�������
		*
		*	\retval	int	0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int RxDataProc( ACE_Message_Block& input_msg );

	protected:
		//! �첽��������д����ֻ���ڶ�
		ACE_Asynch_Read_Stream reader_;			

		//! �첽��������д����ֻ����д
		ACE_Asynch_Write_Stream writer_;		

		//! Э����ָ��
		CIOPrtclParserBase* m_pPrtclParser;		
	};
}

#endif