/*!
 *	\file	IORemoteCtrl.cpp
 *
 *	\brief	遥控控制类实现
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月9日	9:36
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "RTDBDriverFrame/IORemoteCtrl.h"
// #include "RTDBCommon/Debug_Msg.h"

namespace MACS_SCADA_SUD
{

	//!构造函数
	CIORemoteCtrl::CIORemoteCtrl(size_t nKCount /*= 64*/)
	{
		// size_t hMask = 1024 * nKCount;
		// size_t lMask = 1024 * nKCount;
		// m_pQueue = new ACE_Message_Queue<ACE_MT_SYNCH>( hMask, lMask );
	}

	//!析构函数
	CIORemoteCtrl::~CIORemoteCtrl(void)
	{

	}

	//!将消息添加到队列中
	int CIORemoteCtrl::Enqueue(char* pBuf, int nSize)
	{
		// if ( !pBuf || nSize < 1 )
		// {			
		// 	return -1;
		// }
		// if ( IsFull() )
		// {
		// 	MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIORemoteCtrl::Enqueue:Data is Full")));
		// 	return -2;
		// }
		// ACE_Message_Block* pDataBlock = NULL;
		// ACE_NEW_RETURN ( pDataBlock, ACE_Message_Block (nSize), -1 );
		// if ( pDataBlock && pDataBlock->wr_ptr() )
		// {
		// 	ACE_OS::memcpy( pDataBlock->wr_ptr(), pBuf, nSize );

		// 	//! 超时时间
		// 	ACE_Time_Value curTime = ACE_OS::gettimeofday();
		// 	ACE_Time_Value waitTime( 0, 100 );
		// 	curTime += waitTime;

		// 	if ( m_pQueue->enqueue( pDataBlock, &curTime )<0 )
		// 	{//! 如果加入队列失败，则析构该对象
		// 		pDataBlock->release();
		// 	}
		// }
		return 0;
	}

	//!从队列中取得一条信息
	int CIORemoteCtrl::DeQueue(char* pBuf, int& nSize)
	{
		// if ( IsEmpty() )
		// {
		// 	//MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIORemoteCtrl::DeQueue:m_Queue is Empty")));
		// 	return -1;
		// }
		// ACE_Message_Block* pDataBlock;
		// m_pQueue->dequeue( pDataBlock );
		// if ( !pDataBlock )
		// {
		// 	MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIORemoteCtrl::DeQueue:Queue is Empty")));
		// 	return -2;
		// }
		// //!取最小的
		// nSize = __min( nSize, (int)pDataBlock->size() );
		// ACE_OS::memcpy( pBuf, pDataBlock->rd_ptr(), nSize );
		// pDataBlock->release();
		return 0;
	}

	//!队列是否已满
	bool CIORemoteCtrl::IsFull()
	{
		// return m_pQueue->is_full();
		return true;
	}

	//!队列是否为空
	bool CIORemoteCtrl::IsEmpty()
	{
		// return m_pQueue->is_empty();
		return true;
	}

	//!设置队列的大小
	void CIORemoteCtrl::SetQueueLimit(size_t nLimit)
	{
		// m_pQueue->high_water_mark( nLimit * 1024 );
		// m_pQueue->low_water_mark( nLimit * 1024 );
	}
}