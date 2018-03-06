/*!
 *	\file	IOSrlBoardDealThread.h
 *
 *	\brief	消息循环线程类头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月10日	14:47
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOSRLBOARDDEALTHREAD_20080927_H
#define _IOSRLBOARDDEALTHREAD_20080927_H

#include "ace/Proactor.h"
#include "ace/Task_T.h"
#include "IOSrlBoardHandler.h"
#include "SrlHandlerLinux.h"

namespace MACS_SCADA_SUD
{
	/*!
	 *	\class	CIOSrlBoardDealThread
	 *
	 *	\brief	消息循环线程类（windows上为前摄式，其他系统上为反应式），运行消息循环
	 *
	 *	详细的类说明（可选）
	 */
	class CIOSrlBoardDealThread : public ACE_Task<ACE_MT_SYNCH>
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOSrlBoardDealThread(void);

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIOSrlBoardDealThread(void);

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
		*	\param	nTimeOut 超时时间
		*
		*	\retval	int	0表示成功，否则表失败
		*/
		virtual int Init(tIOPortCfg tDrvCfg,CIOPrtclParserBase *pPrtcl,long nTimeOut);

		/*!
		*	\brief	向串口写数据
		*
		*	\param	pbyData 要写数据的头指针
		*	\param	nDataLen 要写数据的长度
		*
		*	\retval	0为成功，否则失败
		*/
		int Write( byte* pbyData, int nDataLen );

	public:
#ifdef _WINDOWS
		//! windows系统上使用前摄式事件处理器
		CIOSrlBoardHandler* m_pHandler;					
#else                
		//! 类linux操作系统上使用反应式事件处理器
		CSrlSvcHandler* m_pHandler;						
#endif

	protected:
#ifdef _WINDOWS
		//! 前摄器（windows上）
		ACE_Proactor* proactor_;						
#else
		//! 反应器（类linux系统上）
		ACE_Reactor* reactor_;							
#endif
		//! 超时时间
		long m_nTimeOut;								
		
		//! 端口参数
		tIOPortCfg m_tDrvBoardCfg;						
		
		//! 协议指针
		CIOPrtclParserBase* m_pPrtclParser;				

	private:
		//! 记录消息循环开始状态
		bool m_bOpened;									

	};
}

#endif