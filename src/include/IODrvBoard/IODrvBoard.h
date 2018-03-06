/*!
 *	\file	IODrvBoard.h
 *
 *	\brief	���������ӿ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	9:29
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODRVBOARD_20080228_H_
#define _IODRVBOARD_20080228_H_

#include "RTDBCommon/OS_Ext.h"
#include "utilities/fepcommondef.h"

#ifdef IODRVBOARD_EXPORTS  
#define IODRVBOARD_API MACS_DLL_EXPORT
#else
#define IODRVBOARD_API MACS_DLL_IMPORT
#endif

namespace MACS_SCADA_SUD
{
	class CIOBoardImpl;
	class CIOPrtclParserBase;

	/*!
	 *	\class	CIODrvBoard
	 *
	 *	\brief	����������ӿ�
	 *
	 *	���������ļ��е������������ã����������ò�ͬ��������
	 */
	class IODRVBOARD_API CIODrvBoard
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIODrvBoard();

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIODrvBoard();

		/*!
		*	\brief	���ʼ������������������������ȵ��øú���
		*
		*	\param	pPrtclParser	Ҫ���õ���������
		*	\param	szBoardType		�忨���ͣ����ھ���ʹ���Ǹ�������
		*	\param	portCfg			�˿����ò���
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
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

		/*!
		*	\brief	��ģ���Ƿ������������Ϣ
		*
		*	\retval	trueΪ������������򲻿�
		*/
		static bool CanOutput();

		/*!
		*	\brief	���������Ϣ
		*
		*	\param	pchDebugInfo	Ҫ�������Ϣ
		*
		*	\retval	void
		*/
		static void OutputDebug( const char* pchDebugInfo );

	protected:
		//! ʵ����ָ��
		CIOBoardImpl* m_pIOBoardImpl;
	};
}

#endif