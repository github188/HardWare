/*!
 *	\file	IOTcpIpHandler.cpp
 *
 *	\brief	用于windows系统的前摄式事件处理器实现
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	9:43
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
		char *time = ctime(&now);        //!< 获取当前时间的字符串格式
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

		//! 读数据
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
			//! 读操作失败,说明通讯线断了
			mb.release(); 

			//! 通知连接中断
			m_pParent->ConnInterrupt();

			MACS_DEBUG(( DEBUG_PERFIX
				ACE_TEXT("CIOTcpIpHandler::handle_read_stream result.success()=%d\n"), sukces ));
			return; 
		} 

		if ( trsf != 0 ) 
		{ 
			//!	接收到服务端应答, 送协议处理器解析
			RxDataProc( result.message_block () );
			mb.release(); 
		}
		else
		{
			mb.release();
		}

		//!	读下一次应答
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
		//! 接收到的数据的长度
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
			//!	有新请求,则发送该请求
			if ( this->writer_.write( *newRequest,newRequest->length() ) != 0 )
			{
				MACS_ERROR(( ERROR_PERFIX
					ACE_TEXT("CIOTcpIpSvcHandler::Write - Write Failed!") ));
				newRequest->release();

				//! 输出调试信息
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