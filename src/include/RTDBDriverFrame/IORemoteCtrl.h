/*!
 *	\file	IORemoteCtrl.h
 *
 *	\brief	遥控控制类定义
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月9日	9:34
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOREMOTECTRL_H_
#define _IOREMOTECTRL_H_

#include "RTDBCommon/OS_Ext.h"
#include "ace/Message_Queue.h"
#include "ace/Synch_Traits.h"
#include "RTDBDriverFrame/IODriverH.h"

namespace MACS_SCADA_SUD
{
	//!队列默认大小为16*1024（16K）
	typedef ACE_Message_Queue<ACE_MT_SYNCH> REMOTECTRL_QUEUE;

	/*!
	 *	\class	类名
	 *
	 *	\brief	ACE消息队列的封装队列默认大小为64*1024
	 *
	 *	为先入先出队列
	 */
	class  IODRIVERFRAME_API CIORemoteCtrl
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIORemoteCtrl(size_t nKCount = 64);

		/*!
		 *	\brief	缺省析构函数
		 */
		~CIORemoteCtrl(void);

		/*!
		*	\brief	将消息添加入队列中
		*
		*	\param	pBuf	:消息内容
		*	\param	nSize	:消息内容的长度
		*
		*	\retval	0为成功，其他失败
		*/
		int Enqueue( char* pBuf, int nSize );

		/*!
		*	\brief	从队列中取得一条消息
		*
		*	\param	pBuf	:消息内容
		*	\param	nSize	:消息内容的长度
		*
		*	\retval	0为成功，其他失败
		*/
		int DeQueue( char* pBuf, int& nSize );

		/*!
		*	\brief	队列是否满
		*
		*	\retval true为队列已满，false为队列未满
		*/
		bool IsFull();

		/*!
		*	\brief	队列是否为空
		*
		*	\retval true为队列为空，false为队列非空
		*/
		bool IsEmpty();

		/*!
		*	\brief	设置队列大小
		*
		*	\param	nLimit	:队列大小
		*
		*	\retval void
		*/
		void SetQueueLimit(size_t nLimit);

	protected:
		//!队列指针
		// REMOTECTRL_QUEUE* m_pQueue;
	};
}

#endif