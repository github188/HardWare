/*!
 *	\file	IOTcpIpHandler.h
 *
 *	\brief	用于windows系统的前摄式事件处理器
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	8:54
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
	 *	\brief	前摄式事件处理器（目前仅用于windows系统）
	 *
	 *	详细的类说明（可选）
	 */
	class CIOTcpIpHandler : public ACE_Service_Handler
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOTcpIpHandler(void);

		/*!
		 *	\brief	缺省析构函数
		 */
		~CIOTcpIpHandler ()
		{
			if (this->handle () != ACE_INVALID_HANDLE)
			{
				ACE_OS::closesocket (this->handle ());
			}
		}

	public:
		//!打开通道进行数据读取
		virtual void open (ACE_HANDLE h, ACE_Message_Block&);

		/*!
		*	\brief	读取数据完成后的调用函数
		*
		*	This method will be called when an asynchronous read completes on a stream.
		*
		*	\param	result	记录了读取的数据内容和相关信息
		*
		*	\retval	void
		*/
		virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);

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
		virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result &result);

		/*!
		*	\brief	初始化辅助参数
		*
		*	\param	pPrtcl	协议类指针
		*	\param	nTimeOut 超时时间
		*	\param	pParent 连接器回调指针
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		virtual int Init( CIOPrtclParserBase* pPrtcl, CIONetSynConnector* pParent );

		//!接收数据的处理
		virtual int RxDataProc( ACE_Message_Block& input_msg );

		/*!
		*	\brief	发送数据
		*
		*	\param	pbyData 要写数据的头指针
		*	\param	nDataLen 要写数据的长度
		*
		*	\retval	0为成功，否则失败
		*/
		int Write( const Byte* pbyData, int nDataLen );

	private:
		ACE_Asynch_Read_Stream reader_;				//! 异步数据流读写器，只用于读
		ACE_Asynch_Write_Stream writer_;			//! 异步数据流读写器，只用于写

		CIOPrtclParserBase* m_pPrtclParser;			//! 协议类指针
		CIONetSynConnector* m_pParent;				//! 连接器回调指针
	};
}


#endif