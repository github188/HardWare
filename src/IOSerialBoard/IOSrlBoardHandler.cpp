/*!
 *	\file	IOSrlBoardHandler.cpp
 *
 *	\brief	用于windows系统的前摄式事件处理器实现
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月10日	11:36
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IOSrlBoardHandler.h"
#include "IOSerialBoard.h"
//#include "ace/High_res_Timer.h"
#include "RTDBBaseClasses/IOPrtclParserBase.h"
#include "RTDBCommon/Debug_Msg.h"

#define MAX_NET_PACKET_LENGTH			  4096
#define MAX_TOTAL_PACKET_LEN			  65535

namespace MACS_SCADA_SUD
{
	CIOSrlBoardHandler::CIOSrlBoardHandler()
	{
		m_pPrtclParser = NULL;
	}

	CIOSrlBoardHandler::~CIOSrlBoardHandler()
	{

	}

	int CIOSrlBoardHandler::Init( CIOPrtclParserBase* pPrtclParser, long nTimeOut )
	{
		m_pPrtclParser = pPrtclParser;
		return 0;
	}

	int CIOSrlBoardHandler::open(tIOPortCfg portCfg)
	{
		ACE_TTY_IO peer_; 
		ACE_DEV_Connector connector_; 

		//! 注意========================================
		int nPortNumber = portCfg.nPort;		//! 端口号
		char pchDevAddr[16];
		//#ifdef _WINDOWS
		//		ACE_OS::sprintf( pchDevAddr, "\\\\.\\COM%d", nPortNumber );
		//#else
		//		ACE_OS::sprintf( pchDevAddr, "/dev/ttyS%d", nPortNumber-1 );
		//#endif

		//! 因此类只在windows上使用，故修改
		ACE_OS::sprintf( pchDevAddr, "\\\\.\\COM%d", nPortNumber );
		int result = connector_.connect( peer_, 
			ACE_DEV_Addr( pchDevAddr ), 
			0, //timeout 
			ACE_Addr::sap_any, 
			1, //reuse 
			O_RDWR | FILE_FLAG_OVERLAPPED ); 

		if( result != 0 ) 
		{
			ACE_ERROR( ( LM_ERROR, 
				"BLAD: %m (%d).\n", ACE_OS::last_error() ) ); 
			//! 输出调试信息
			if ( CIOSerialBoard::CanOutput() )
			{
				char pchDebug[DEBUGINFOLEN];
				sprintf( pchDebug, "CIOSrlBoardHandler::open port %s Failed!", pchDevAddr );
				CIOSerialBoard::OutputDebug( pchDebug );
			}

			return -1;
		}
		// set the appropriate parameters 
		ACE_TTY_IO::Serial_Params params; 

		params.baudrate = portCfg.iBaudRate;//!< 波特率
		params.databits = portCfg.iDataBits;//!< 数据位。合法值为5,6,7,8。Win32上支持4位
		params.stopbits = portCfg.iStopBits;//!< 停止位。合法值为1和2
		//params.parityenb = (bool)portCfg.byParity;//!< 是否进行奇偶校验
		if(portCfg.byParity==1)
			params.paritymode = "EVEN";				//!< 奇偶校验的模式。POSIX支持"even"和"odd"。
		else if(portCfg.byParity==2)
			params.paritymode = "ODD";				//!< 奇偶
		else
			params.paritymode = "none";
		params.xonlim = 0;				//!< 在XON字符被发送之前，输入缓冲区的最小字节
		params.xofflim = 0;				//!< 在XOFF字符被发送之前，输入缓冲区的最大字节
		params.readmincharacters = 1;	//!< 对于POSIX非标准的读最少个数的字符
		params.readtimeoutmsec = 10000;	//!< 在从read返回之前，等待的时间			
		params.ctsenb = false;			//!< 是否支持CTS模式。确定是否可通过查询 Clear To Send (CTS) 线的状态发送数据。
		params.rtsenb = 0;				//!< 使用和设置RTS。0表示不使用RTS 1表示使用RTS 2表示使用RTS流控制握手(win32)
										//!< 3表示如果有了足够的用于传输的字节，RTS线变高。传输之后RTS变成低(win32)
		params.xinenb = false;			//!< 是否在输入上使用软件流控制
		params.xoutenb = false;			//!< 是否在输出上使用软件流控制
		params.modem = false;			//!< 设备是否是POSIX调制模式
		params.rcvenb = false;			//!< 是否使用receiver (POSIX)
		params.dsrenb = false;			//!< Data Set Ready (DSR) 线的状态是否起作用 (WIN32)
		params.dtrdisable = false;		//!< Data Terminal Ready状态是否起作用


		result = peer_.control( ACE_TTY_IO::SETPARAMS, &params ); 
		if( result != 0 ) 
		{ 
			//error; 
			return -2; 
		} 

		if ( this->reader_.open( *this, peer_.get_handle() ) != 0 ) 
		{ 
			ACE_ERROR( ( LM_ERROR, 
				"BLAD: %m (%d).\n", ACE_OS::last_error() ) ); 
			//! 输出调试信息
			if ( CIOSerialBoard::CanOutput() )
			{
				char pchDebug[DEBUGINFOLEN];
				sprintf( pchDebug, "CIOSrlBoardHandler::open %s reader_ Failed!", pchDevAddr );	
				CIOSerialBoard::OutputDebug( pchDebug );
			}

			return -3; 
		} 

		//! 输出调试信息
		if ( CIOSerialBoard::CanOutput() )
		{
			char pchDebug[DEBUGINFOLEN];
			sprintf( pchDebug, "CIOSrlBoardHandler::open %s reader_ Seccessed!", pchDevAddr );	
			CIOSerialBoard::OutputDebug( pchDebug );
		}

		if ( this->writer_.open( *this, peer_.get_handle() ) != 0 ) 
		{ 
			ACE_ERROR( ( LM_ERROR, 
				"BLAD: %m (%d).\n", ACE_OS::last_error() )    ); 
			//! 输出调试信息
			if ( CIOSerialBoard::CanOutput() )
			{
				char pchDebug[DEBUGINFOLEN];
				sprintf( pchDebug, "CIOSrlBoardHandler::open %s writer_ Failed!", pchDevAddr );	
				CIOSerialBoard::OutputDebug( pchDebug );
			}
			return -4; 
		} 

		//! 输出调试信息
		if ( CIOSerialBoard::CanOutput() )
		{
			char pchDebug[DEBUGINFOLEN];
			sprintf( pchDebug, "CIOSrlBoardHandler::open %s writer_ Seccessed!", pchDevAddr );	
			CIOSerialBoard::OutputDebug( pchDebug );
		}

		ACE_Message_Block* mb; 
		ACE_NEW_NORETURN( mb, ACE_Message_Block( MAX_NET_PACKET_LENGTH ) ); 
		if ( this->reader_.read( *mb, mb->space() ) != 0 ) 
		{ 
			ACE_ERROR ((LM_ERROR, "%p\n", "AIO_Proactive_Service::read"));
			mb->release(); 

			//! 输出调试信息
			if ( CIOSerialBoard::CanOutput() )
			{
				char pchDebug[DEBUGINFOLEN];
				sprintf( pchDebug, "AIO_Proactive_Service::read Message Failed!" );	
				CIOSerialBoard::OutputDebug( pchDebug );
			}
			return -5; 
		} 

		MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::open new,%x"),mb )); 

		return 0;
	}

	void CIOSrlBoardHandler::handle_read_stream(const ACE_Asynch_Read_Stream::Result& result)
	{
		//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream start,%x"), &(result.message_block()) )); 
		ACE_Message_Block& mb = result.message_block(); 
		bool sukces = result.success(); 
		int trsf = result.bytes_transferred(); 
		//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream 1111111,%x"), &(result.message_block()) )); 
		if ( !sukces ) 
		{ 
			MACS_ERROR(( ERROR_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream failed,%x"), &mb ));
		} 
		else
		{
			if ( trsf != 0 ) 
			{ 
				//	接收到服务端应答, 送协议处理器解析
				RxDataProc( result.message_block () );
				//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream 2222222222,%x"), &(result.message_block()) )); 
			} 
			else 
			{ 
				//	超时没有接收到服务端应答
				//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream 33333333,%x"), &(result.message_block()) )); 
			} 
		}
		mb.release(); 
		//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream 4444444444,%x"), &(result.message_block()) )); 

		//	读下一次应答
		ACE_Message_Block* new_mb; 
		ACE_NEW_NORETURN( new_mb, ACE_Message_Block( MAX_NET_PACKET_LENGTH ) ); 
		//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream 555555555,%x"), &(result.message_block()) )); 
		this->reader_.read( *new_mb, new_mb->space() ); 
		//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream new,%x"), new_mb )); 
		return; 
	}

	void CIOSrlBoardHandler::handle_write_stream(const ACE_Asynch_Write_Stream::Result& result)
	{
		//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "void CIOSrlBoardHandler::handle_write_stream, %x" ),&(result.message_block()) )); 
		result.message_block().release();
	}

	int CIOSrlBoardHandler::Write(unsigned char * pbyData, int nDataLen)
	{
		if( !pbyData || nDataLen <= 0)
			return -2;

		ACE_Message_Block* newRequest; 
		ACE_NEW_NORETURN( newRequest, ACE_Message_Block(nDataLen) );

		ACE_OS::memcpy( newRequest->wr_ptr(), pbyData, nDataLen );
		newRequest->wr_ptr( nDataLen );
		if( newRequest != 0 )
		{
			//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOSrlBoardHandler::Write  发送数据 *************************!" ) ));
			//	有新请求,则发送该请求
			if ( this->writer_.write( *newRequest, nDataLen ) != 0 )
			{
				MACS_ERROR(( ERROR_PERFIX ACE_TEXT( "CIOSrlBoardHandler::Write Failed!!")));
				newRequest->release();

				//! 输出调试信息
				if ( CIOSerialBoard::CanOutput() )
				{
					char pchDebug[DEBUGINFOLEN];
					sprintf( pchDebug, "CIOSrlBoardHandler::Write - send newRequest Failed!" );
					CIOSerialBoard::OutputDebug( pchDebug );
				}
				return -1;
			}
		}

		//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::Write,%x"),newRequest )); 
		return 0;
	}

	int CIOSrlBoardHandler::RxDataProc(ACE_Message_Block& input_msg)
	{
		//! 接收到的数据的长度
		int length = static_cast<int>(input_msg.length());
		return ( m_pPrtclParser->RxDataProc( (Byte*)( input_msg.rd_ptr() ), (int&)length ) );
	}
}