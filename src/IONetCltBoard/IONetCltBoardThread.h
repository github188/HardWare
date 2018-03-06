/*!
 *	\file	IONetCltBoardThread.h
 *
 *	\brief	消息循环线程类头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月11日	14:17
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IONetCltBoardThread_20080331_H
#define _IONetCltBoardThread_20080331_H

#include "ace/Proactor.h"
#include "ace/Task_T.h"
#include "ace/TP_Reactor.h"
#include "ace/WIN32_Proactor.h"
#include "utilities/fepcommondef.h"

namespace MACS_SCADA_SUD
{
	class CIOTcpIpConnector;
	class CIONetSynConnector;

	class ACE_ReactorEX : public ACE_Reactor
	{
	public:
		ACE_ReactorEX (ACE_Reactor_Impl *implementation = 0, int delete_implementation = 0);

		virtual ~ACE_ReactorEX(void);

		virtual int run_reactor_event_loop (REACTOR_EVENT_HOOK = 0);

	};


	/*!
	*	\class	IOComBoardDealThread
	*
	*	\brief	消息循环线程类（windows上为前摄式，其他系统上为反应式），运行消息循环
	*/
	class CIONetCltBoardThread : public ACE_Task<ACE_MT_SYNCH>
	{
	public:
		/*!
		*	\brief	缺省构造函数
		*/
		CIONetCltBoardThread( void );

		/*!
		*	\brief	缺省析构函数
		*/
		virtual ~CIONetCltBoardThread( void );

		/*!
		*	\brief	启动消息循环
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		virtual int open();

		/*!
		*	\brief	退出消息循环
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		int close();

		/*!
		*	\brief	消息循环线程入口，运行消息循环
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		virtual int svc( void );

		/*!
		*	\brief	初始化串口参数
		*
		*	\param	tDrvCfg	串口参数
		*	\param	pPrtcl	协议类指针
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		virtual int Init(tIOPortCfg tDrvCfg, CIOPrtclParserBase *pPrtcl);

		/*!
		*	\brief	发送数据
		*
		*	\param	pbyData 要写数据的头指针
		*	\param	nDataLen 要写数据的长度
		*
		*	\retval	0为成功，否则失败
		*/
		int Write( const Byte* pbyData, int nDataLen );

#ifdef _WINDOWS
		CIONetSynConnector* GetConnector()		
#else
		CIOTcpIpConnector* GetConnector()
#endif
		{
			return m_pConnector;
		}

	private:
#ifdef _WINDOWS
		ACE_WIN32_Proactor m_proactorImpl;
		ACE_Proactor* proactor_;				//! 前摄器（windows上）
		CIONetSynConnector* m_pConnector;		//! 异步连接器，由它来调用前摄式事件处理器
#else
		ACE_TP_Reactor m_selReactor;
		ACE_ReactorEX* reactor_;				//! 反应器（类linux系统上）
		CIOTcpIpConnector* m_pConnector;		//! 连接器，由它来调用反应式事件处理器
#endif
		tIOPortCfg m_tDrvBoardCfg;				//! 端口参数
		CIOPrtclParserBase* m_pPrtclParser;		//! 协议指针

		//! 如果正常连接并且已经有连接被激活，则为true，否则为false
		bool m_bWorking;      
	};
}

#endif