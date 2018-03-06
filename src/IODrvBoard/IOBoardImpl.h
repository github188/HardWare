/*!
 *	\file	IOBoardImpl.h
 *
 *	\brief	�����๤�����������ʵ�����������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��14��	8:55
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOBOARDIMPL_20080228_H_
#define _IOBOARDIMPL_20080228_H_

#include "RTDBBaseClasses/IOBoardBase.h"
#include "RTDBCommon/GenericFactory.h"

namespace MACS_SCADA_SUD
{
	class CIOPrtclParserBase;

	/*!
	 *	\class	CIOBoardImpl
	 *
	 *	\brief	������ʵ����
	 *
	 *	���������ļ��е������������ã����������ò�ͬ��������
	 */
	class CIOBoardImpl
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOBoardImpl();

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIOBoardImpl();

		/*!
		 *	\brief	���ʼ������������������������ȵ��øú���
		 *
		 *	����������Ϣ�����������������
		 *
		 *	\param	pPrtclParser	Э�������
		 *	\param	szBoardType		�忨���ͣ����ھ���ʹ���Ǹ�������
		 *	\param	portCfg			�˿�������Ϣ
		 *	\param	nTimeOut		��ʱʱ��
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		int Init( CIOPrtclParserBase* pPrtclParser, std::string szBoardType, tIOPortCfg portCfg, long nTimeOut);

		/*!
		 *	\brief	ͨ����·��������
		 *
		 *	\param	pbyData		Ҫ���͵���������
		 *	\param	nDataLen	Ҫ���͵����ݵĳ���
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 */
		int Write( unsigned char* pbyData, int nDataLen );

		/*!
		 *	\brief	ʹ��IO��������ʼ����
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 */
		int open ( );

		/*!
		 *	\brief	ֹͣ��IO������
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 */
		int close ( );

		/*!
		 *	\brief	������ʱ��
		 *
		 *	\param	nTimeOut ��ʱ��ʱ����
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 */
		int SetTimer( long nTimeOut );

		/*!
		 *	\brief	ֹͣ��ʱ��
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 */
		int StopTimer();

		/*!
		 *	\brief	��������״̬
		 *
		 *	\retval	��������Ϊtrue ����Ϊfalse 
		 */
		bool ConnState();

		/*!
		 *	\brief	�Ͽ�����
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 */
		int StopConn();

		/*!
		 *	\brief	���ӣ����ӵ�ַ�Ͷ˿ڲ��䣬��������
		 *
		 *	��������趨�ĳ�ʱʱ�仹û�����ӳɹ����򷵻�ʧ��
		 *
		 *	\param	nTimeOut ��ʱʱ��
		 *
		 *	\retval	true��ʾ���ӳɹ���false��ʾ����ʧ��
		 */
		bool Connect( long nTimeOut );

		/*!
		 *	\brief	���ӣ����ݸ��������ӵ�ַ�Ͷ˿ڽ�������
		 *
		 *	��������趨�ĳ�ʱʱ�仹û�����ӳɹ����򷵻�ʧ��
		 *
		 *	\param	nTimeOut	��ʱʱ��
		 *	\param	pPeerIP		���IP��ַ
		 *	\param	nPort		���Ӷ˿�
		 *
		 *	\retval	true��ʾ���ӳɹ���false��ʾ����ʧ��
		 */
		bool Connect( long nTimeOut, const char* pPeerIP, int nPort );

		/*!
		 *	\brief	���ӣ����ӵ�ַ�Ͷ˿ڲ��䣬��������
		 *
		 *	������������
		 *
		 *	\retval	int
		 *
		 *	\note	���ؽ������ʾ�ɰܣ���Ҫ�ⲿ��ʱ������ȷ�������Ƿ�ɹ�
		 */
		int  Connect( );

		/*!
		 *	\brief	���ӣ����ӵ�ַ�Ͷ˿ڲ��䣬��������
		 *
		 *	������������
		 *
		 *	\retval	int
		 *
		 *	\note	���ؽ������ʾ�ɰܣ���Ҫ�ⲿ��ʱ������ȷ�������Ƿ�ɹ�
		 */
		int  Connect( const char* pPeerIP, int nPort );

	protected:
		//!����������ָ��,���ݲ�ͬ����������
		CIOBoardBase* m_pBoardBase;
	};
}


#endif