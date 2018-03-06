/*!
 *	\file	IONetCltBoard.h
 *
 *	\brief	��̫���ͻ��˵���������ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��11��	15:46
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IONETCLTBOARD_20080228_H
#define _IONETCLTBOARD_20080228_H

#include "RTDBCommon/OS_Ext.h"
#include "RTDBBaseClasses/IOBoardBase.h"
#include "IONetCltBoardThread.h"

#ifdef IONETCLTBOARD_EXPORTS
#define IONETCLTBOARD_API MACS_DLL_EXPORT
#else
#define IONETCLTBOARD_API MACS_DLL_IMPORT
#endif

namespace MACS_SCADA_SUD
{
	/*!
	 *	\class	CIONetCltBoard
	 *
	 *	\brief	��̫���ͻ��˵ķ��ʽӿڷ�װ
	 *
	 *	�ṩ�˽������ӡ��Ͽ����ӡ��������ݡ���ȡ����״̬�Ľӿڣ�����ֻ��װ�˵�������
	 *	Windowsƽ̨�ǻ���ǰ����ʵ�ֵģ�Linuxƽ̨�ǻ��ڷ�Ӧ��ʵ�ֵġ�
	 */
	class IONETCLTBOARD_API CIONetCltBoard : public CIOBoardBase
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIONetCltBoard();

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIONetCltBoard();

		/*!
		*	\brief	��������
		*
		*	\param	pbyData��Ҫ���͵����ݵ���ָ��
		*	\param	nDataLen��Ҫ���͵����ݵĳ���
		*
		*	\retval	0Ϊ�ɹ�������Ϊʧ��
		*/
		virtual int Write( Byte* pbyData, int nDataLen );

		/*!
		*	\brief	��������
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual int open( );

		/*!
		*	\brief	�ر�������
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual int close( );

		/*!
		*	\brief	��������״̬
		*
		*	\retval	��������Ϊtrue ����Ϊfalse 
		*/
		virtual bool ConnState();

		/*!
		*	\brief	�Ͽ�����
		*
		*	\retval	int 0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int StopConn();

		/*!
		*	\brief	��������
		*
		*	\param	nTimeOut�����ӳ�ʱʱ��
		*
		*	\retval	trueΪ�ɹ���falseΪʧ��
		*/
		virtual bool Connect( long nTimeOut );	

		/*!
		*	\brief	�������ӣ���������
		*
		*	\param	nTimeOut�����ӳ�ʱʱ��
		*	\param	pPeerIP��Ҫ���ӵķ���˵�IP��ַ
		*	\param	nPort���˿ں�
		*
		*	\retval	trueΪ�ɹ���falseΪʧ��
		*/
		virtual bool Connect( long nTimeOut, const char* pPeerIP, int nPort );

		/*!
		*	\brief	�������ӣ����ؽ������ʾ�ɰ�
		*
		*	\retval	int
		*/
		virtual int  Connect( );

		/*!
		*	\brief	�������ӣ���������
		*
		*	\param	pPeerIP��Ҫ���ӵķ���˵�IP��ַ
		*	\param	nPort���˿ں�
		*
		*	\retval	int
		*/
		virtual int  Connect( const char* pPeerIP, int nPort );

		//! ��ģ���Ƿ������������Ϣ
		static bool CanOutput();

		//! ���������Ϣ
		static void OutputDebug( const char* pchDebugInfo );

		//! ��ȡ����
		std::string GetClassName() ;

	private:
		//! �����߳�ָ��
		CIONetCltBoardThread *m_pComDealThd;	

	public:
		//! ����
		static std::string s_ClassName;	
	};

#ifndef IONETCLTBOARD_EXPORTS
	extern "C" MACS_DLL_IMPORT int InitDll();
	extern "C" MACS_DLL_IMPORT int UnInitDll();
#endif
}

#endif