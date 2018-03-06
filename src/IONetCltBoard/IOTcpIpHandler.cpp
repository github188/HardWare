/*!
 *	\file	IOTcpIpHandler.cpp
 *
 *	\brief	����windowsϵͳ��ǰ��ʽ�¼�������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��11��	9:43
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IOTcpIpHandler.h"
#include "RTDBBaseClasses/IOPrtclParserBase.h"
#include "IONetCltBoard.h"
#include "ace/Asynch_Connector.h"
#include "RTDBCommon/Debug_Msg.h"
#include "IONetSynConnector.h"

namespace MACS_SCADA_SUD
{
	
	CIOTcpIpHandler::CIOTcpIpHandler(void)
	{
		m_pPrtclParser = NULL;
		m_pParent = NULL;
	}

	void CIOTcpIpHandler::open(ACE_HANDLE h, ACE_Message_Block&)
	{
		time_t now = ACE_OS::gettimeofday().sec();
		char *time = ctime(&now);        //!< ��ȡ��ǰʱ����ַ�����ʽ
		this->handle (h);

		if ( this->reader_.open( *this ,ACE_INVALID_HANDLE,0,this->proactor()) != 0 ) 
		{ 
			MACS_ERROR(( ERROR_PERFIX
				ACE_TEXT("CIOTcpIpHandler::open: opening reader_ fails(%d).\n"), ACE_OS::last_error() ));
			return; 
		} 

		if ( this->writer_.open( *this ,ACE_INVALID_HANDLE,0,this->proactor()) != 0 ) 
		{ 
			MACS_ERROR(( ERROR_PERFIX
				ACE_TEXT("CIOTcpIpHandler::open: opening writer_ fails(%d).\n"), ACE_OS::last_error() ));
			return; 
		}	

		//! ������
		ACE_Message_Block* mb; 
		ACE_NEW_NORETURN( mb, ACE_Message_Block( 1024 ) ); 
		if ( this->reader_.read( *mb, mb->space() ) != 0 ) 
		{ 
			MACS_ERROR(( ERROR_PERFIX
				ACE_TEXT("AIO_Proactive_Service::read!") ));
			mb->release(); 
			return; 
		} 
	}

	int CIOTcpIpHandler::Init(CIOPrtclParserBase* pPrtcl, CIONetSynConnector* pParent)
	{
		m_pPrtclParser = pPrtcl;
		m_pParent = pParent;
		return 0;
	}

	void CIOTcpIpHandler::handle_read_stream(const ACE_Asynch_Read_Stream::Result &result)
	{
		ACE_Message_Block& mb = result.message_block(); 
		bool sukces = result.success(); 
		int trsf = result.bytes_transferred(); 
		if ( !sukces ) 
		{ 
			//! ������ʧ��,˵��ͨѶ�߶���
			mb.release(); 

			//! ֪ͨ�����ж�
			m_pParent->ConnInterrupt();

			MACS_DEBUG(( DEBUG_PERFIX
				ACE_TEXT("CIOTcpIpHandler::handle_read_stream result.success()=%d\n"), sukces ));
			return; 
		} 

		if ( trsf != 0 ) 
		{ 
			//!	���յ������Ӧ��, ��Э�鴦��������
			RxDataProc( result.message_block () );
			mb.release(); 
		}
		else
		{
			mb.release();
		}

		//!	����һ��Ӧ��
		ACE_Message_Block* new_mb; 
		ACE_NEW_NORETURN( new_mb, ACE_Message_Block( 1024 ) ); 
		this->reader_.read( *new_mb, new_mb->space() ); 
		return;
	}

	void CIOTcpIpHandler::handle_write_stream(const ACE_Asynch_Write_Stream::Result &result)
	{
		result.message_block().release();
		return;
	}

	int CIOTcpIpHandler::RxDataProc(ACE_Message_Block& input_msg)
	{
		//! ���յ������ݵĳ���
		int length = static_cast<int>(input_msg.length());
		return ( m_pPrtclParser->RxDataProc( (Byte*)( input_msg.rd_ptr() ), length ) );
	}

	int CIOTcpIpHandler::Write(const Byte* pbyData, int nDataLen)
	{
		if( !pbyData || nDataLen <= 0)
			return -2;
		ACE_Message_Block* newRequest; 
		ACE_NEW_NORETURN( newRequest, ACE_Message_Block(nDataLen) );

		ACE_OS::memcpy( newRequest->wr_ptr(), pbyData, nDataLen );
		newRequest->wr_ptr( nDataLen );
		if( newRequest != 0 )
		{
			//!	��������,���͸�����
			if ( this->writer_.write( *newRequest,newRequest->length() ) != 0 )
			{
				MACS_ERROR(( ERROR_PERFIX
					ACE_TEXT("CIOTcpIpSvcHandler::Write - Write Failed!") ));
				newRequest->release();

				//! ���������Ϣ
				if ( CIONetCltBoard::CanOutput() )
				{
					char pchDebug[DEBUGINFOLEN];
					sprintf( pchDebug, "CIOTcpIpHandler::Write - send newRequest Failed!" );
					CIONetCltBoard::OutputDebug( pchDebug );
				}
				return -1;
			}
		}
		return 0;
	}

	
}