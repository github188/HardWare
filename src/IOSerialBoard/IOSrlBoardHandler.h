/*!
 *	\file	IOSrlBoardHandler.h
 *
 *	\brief	用于windows系统的前摄式事件处理器
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月10日	11:12
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
	 *	\brief	前摄式事件处理器（目前仅用于windows系统）
	 *
	 *	详细的类说明（可选）
	 */
	class CIOSrlBoardHandler : public ACE_Handler
	{
		public:
		/*!
		*	\brief	缺省构造函数
		*/
		CIOSrlBoardHandler();

		/*!
		*	\brief	缺省析构函数
		*/
		virtual ~CIOSrlBoardHandler();

		/*!
		*	\brief	打开端口，并发送数据
		*
		*	\param	portCfg	串口参数
		*
		*	\retval	int 0为成功，否则失败
		*/
		virtual int open( tIOPortCfg portCfg);

		/*!
		*	\brief	读取数据完成后的调用函数
		*
		*	This method will be called when an asynchronous read completes on a stream.
		*
		*	\param	result	记录了读取的数据内容和相关信息
		*
		*	\retval	void
		*/
		virtual void handle_read_stream( 
			const ACE_Asynch_Read_Stream::Result& result );

		/*!
		*	\brief	写数据完成后的调用函数
		*
		*	This method will be called when an asynchronous write completes on a stream.
		*
		*	\param	result	记录了读取的数据内容和相关信息
		*
		*	\retval	void
		*
		*	\note	一定要在此函数中完成数据空间的释放，否则会造成内存泄露
		*/
		virtual void handle_write_stream( 
			const ACE_Asynch_Write_Stream::Result& result );

		/*!
		*	\brief	向串口写数据
		*
		*	\param	pbyData 要写数据的头指针
		*	\param	nDataLen 要写数据的长度
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int Write( unsigned char * pbyData, int nDataLen );

		/*!
		*	\brief	初始化辅助参数
		*
		*	\param	pPrtcl	协议类指针
		*	\param	nTimeOut 超时时间
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		virtual int Init( CIOPrtclParserBase* m_pPrtclParser, long nTimeOut );

		/*!
		*	\brief	封装一下接受响应函数，当有数据来时在此函数中调用协议类中的RxDataProc
		*
		*	\param	input_msg	接受到的数据
		*	\param	bStopTimer	是否停止超时服务器
		*	\param	bSendData	处理完成后是否再次发送数据
		*
		*	\retval	int	0表示成功，否则表示失败
		*/
		virtual int RxDataProc( ACE_Message_Block& input_msg );

	protected:
		//! 异步数据流读写器，只用于读
		ACE_Asynch_Read_Stream reader_;			

		//! 异步数据流读写器，只用于写
		ACE_Asynch_Write_Stream writer_;		

		//! 协议类指针
		CIOPrtclParserBase* m_pPrtclParser;		
	};
}

#endif