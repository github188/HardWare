/*!
 *	\file	IOSrlBoardHandler.cpp
 *
 *	\brief	����windowsϵͳ��ǰ��ʽ�¼�������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	11:36
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

		//! ע��========================================
		int nPortNumber = portCfg.nPort;		//! �˿ں�
		char pchDevAddr[16];
		//#ifdef _WINDOWS
		//		ACE_OS::sprintf( pchDevAddr, "\\\\.\\COM%d", nPortNumber );
		//#else
		//		ACE_OS::sprintf( pchDevAddr, "/dev/ttyS%d", nPortNumber-1 );
		//#endif

		//! �����ֻ��windows��ʹ�ã����޸�
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
			//! ���������Ϣ
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

		params.baudrate = portCfg.iBaudRate;//!< ������
		params.databits = portCfg.iDataBits;//!< ����λ���Ϸ�ֵΪ5,6,7,8��Win32��֧��4λ
		params.stopbits = portCfg.iStopBits;//!< ֹͣλ���Ϸ�ֵΪ1��2
		//params.parityenb = (bool)portCfg.byParity;//!< �Ƿ������żУ��
		if(portCfg.byParity==1)
			params.paritymode = "EVEN";				//!< ��żУ���ģʽ��POSIX֧��"even"��"odd"��
		else if(portCfg.byParity==2)
			params.paritymode = "ODD";				//!< ��ż
		else
			params.paritymode = "none";
		params.xonlim = 0;				//!< ��XON�ַ�������֮ǰ�����뻺��������С�ֽ�
		params.xofflim = 0;				//!< ��XOFF�ַ�������֮ǰ�����뻺����������ֽ�
		params.readmincharacters = 1;	//!< ����POSIX�Ǳ�׼�Ķ����ٸ������ַ�
		params.readtimeoutmsec = 10000;	//!< �ڴ�read����֮ǰ���ȴ���ʱ��			
		params.ctsenb = false;			//!< �Ƿ�֧��CTSģʽ��ȷ���Ƿ��ͨ����ѯ Clear To Send (CTS) �ߵ�״̬�������ݡ�
		params.rtsenb = 0;				//!< ʹ�ú�����RTS��0��ʾ��ʹ��RTS 1��ʾʹ��RTS 2��ʾʹ��RTS����������(win32)
										//!< 3��ʾ��������㹻�����ڴ�����ֽڣ�RTS�߱�ߡ�����֮��RTS��ɵ�(win32)
		params.xinenb = false;			//!< �Ƿ���������ʹ�����������
		params.xoutenb = false;			//!< �Ƿ��������ʹ�����������
		params.modem = false;			//!< �豸�Ƿ���POSIX����ģʽ
		params.rcvenb = false;			//!< �Ƿ�ʹ��receiver (POSIX)
		params.dsrenb = false;			//!< Data Set Ready (DSR) �ߵ�״̬�Ƿ������� (WIN32)
		params.dtrdisable = false;		//!< Data Terminal Ready״̬�Ƿ�������


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
			//! ���������Ϣ
			if ( CIOSerialBoard::CanOutput() )
			{
				char pchDebug[DEBUGINFOLEN];
				sprintf( pchDebug, "CIOSrlBoardHandler::open %s reader_ Failed!", pchDevAddr );	
				CIOSerialBoard::OutputDebug( pchDebug );
			}

			return -3; 
		} 

		//! ���������Ϣ
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
			//! ���������Ϣ
			if ( CIOSerialBoard::CanOutput() )
			{
				char pchDebug[DEBUGINFOLEN];
				sprintf( pchDebug, "CIOSrlBoardHandler::open %s writer_ Failed!", pchDevAddr );	
				CIOSerialBoard::OutputDebug( pchDebug );
			}
			return -4; 
		} 

		//! ���������Ϣ
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

			//! ���������Ϣ
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
				//	���յ������Ӧ��, ��Э�鴦��������
				RxDataProc( result.message_block () );
				//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream 2222222222,%x"), &(result.message_block()) )); 
			} 
			else 
			{ 
				//	��ʱû�н��յ������Ӧ��
				//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream 33333333,%x"), &(result.message_block()) )); 
			} 
		}
		mb.release(); 
		//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "int CIOSrlBoardHandler::handle_read_stream 4444444444,%x"), &(result.message_block()) )); 

		//	����һ��Ӧ��
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
			//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOSrlBoardHandler::Write  �������� *************************!" ) ));
			//	��������,���͸�����
			if ( this->writer_.write( *newRequest, nDataLen ) != 0 )
			{
				MACS_ERROR(( ERROR_PERFIX ACE_TEXT( "CIOSrlBoardHandler::Write Failed!!")));
				newRequest->release();

				//! ���������Ϣ
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
		//! ���յ������ݵĳ���
		int length = static_cast<int>(input_msg.length());
		return ( m_pPrtclParser->RxDataProc( (Byte*)( input_msg.rd_ptr() ), (int&)length ) );
	}
}