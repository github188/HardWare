/*!
 *	\file	IORemoteCtrl.cpp
 *
 *	\brief	ң�ؿ�����ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��9��	9:36
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

	//!���캯��
	CIORemoteCtrl::CIORemoteCtrl(size_t nKCount /*= 64*/)
	{
		// size_t hMask = 1024 * nKCount;
		// size_t lMask = 1024 * nKCount;
		// m_pQueue = new ACE_Message_Queue<ACE_MT_SYNCH>( hMask, lMask );
	}

	//!��������
	CIORemoteCtrl::~CIORemoteCtrl(void)
	{

	}

	//!����Ϣ��ӵ�������
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

		// 	//! ��ʱʱ��
		// 	ACE_Time_Value curTime = ACE_OS::gettimeofday();
		// 	ACE_Time_Value waitTime( 0, 100 );
		// 	curTime += waitTime;

		// 	if ( m_pQueue->enqueue( pDataBlock, &curTime )<0 )
		// 	{//! ����������ʧ�ܣ��������ö���
		// 		pDataBlock->release();
		// 	}
		// }
		return 0;
	}

	//!�Ӷ�����ȡ��һ����Ϣ
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
		// //!ȡ��С��
		// nSize = __min( nSize, (int)pDataBlock->size() );
		// ACE_OS::memcpy( pBuf, pDataBlock->rd_ptr(), nSize );
		// pDataBlock->release();
		return 0;
	}

	//!�����Ƿ�����
	bool CIORemoteCtrl::IsFull()
	{
		// return m_pQueue->is_full();
		return true;
	}

	//!�����Ƿ�Ϊ��
	bool CIORemoteCtrl::IsEmpty()
	{
		// return m_pQueue->is_empty();
		return true;
	}

	//!���ö��еĴ�С
	void CIORemoteCtrl::SetQueueLimit(size_t nLimit)
	{
		// m_pQueue->high_water_mark( nLimit * 1024 );
		// m_pQueue->low_water_mark( nLimit * 1024 );
	}
}