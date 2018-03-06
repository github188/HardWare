/*!
 *	\file	IOSerialBoard.h
 *
 *	\brief	������������ʵ����ͷ�ļ������������̲�����
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	17:29
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOSERIALBOARD_20080927_H
#define _IOSERIALBOARD_20080927_H

#include "RTDBCommon/OS_Ext.h"
#include "RTDBBaseClasses/IOBoardBase.h"


#ifdef IOSERIALBOARD_EXPORTS
#define IOSERIALBOARD_API MACS_DLL_EXPORT
#else
#define IOSERIALBOARD_API MACS_DLL_IMPORT
#endif


namespace MACS_SCADA_SUD
{
	class CIOSrlBoardDealThread;

	/*!
	*	\class	CIOComxBoard
	*
	*	\brief	������������ʵ����
	*
	*	�����������涨�������̣�ֻ�ṩ��һ��ӿ�
	*/
	class IOSERIALBOARD_API CIOSerialBoard : public CIOBoardBase
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOSerialBoard();

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIOSerialBoard();

		/*!
		*	\brief	�򴮿�д����
		*
		*	\param	pbyData Ҫд���ݵ�ͷָ��
		*	\param	nDataLen Ҫд���ݵĳ���
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual int Write( unsigned char* pbyData, int nDataLen );

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
		*	\brief	��ģ���Ƿ������������Ϣ
		*
		*	\retval	trueΪ�����������������
		*/
		static bool CanOutput();

		/*!
		*	\brief	���������Ϣ
		*
		*	\param	pchDebugInfo Ҫ�������Ϣ
		*
		*	\retval	void
		*/
		static void OutputDebug( const char* pchDebugInfo );

		/*!
		*	\brief	��ȡ����
		*
		*	\retval	std::string ����
		*/
		std::string GetClassName() ;

	private:
		//!�����߳�ָ��
		CIOSrlBoardDealThread* m_pComDealThd;

	public:
		//! ����
		static std::string s_ClassName;
	};

#ifndef IOSERIALBOARD_EXPORTS
	extern "C" MACS_DLL_IMPORT int InitDll();
	extern "C" MACS_DLL_IMPORT int UnInitDll();
#endif
}

#endif