/*!
 *	\file	IOBoardBase.h
 *
 *	\brief	����������ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Taiwanxia
 *
 *	\date	2014��4��10��	9:48
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOBOARDBASE_H_
#define _IOBOARDBASE_H_

#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/GenericFactory.h"
#include "utilities/fepcommondef.h"

#ifndef DBBASECLASSES_API
	#ifdef DBBASECLASSES_EXPORTS
	#define DBBASECLASSES_API MACS_DLL_EXPORT
	#else
	#define DBBASECLASSES_API MACS_DLL_IMPORT
	#endif
#endif

namespace MACS_SCADA_SUD
{
	class CIOPrtclParserBase;

	/*!
	 *	\class	CIOBoardBase
	 *
	 *	\brief	����������
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class DBBASECLASSES_API CIOBoardBase
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOBoardBase();

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIOBoardBase();

		/*!
		 *	\brief	���ʼ������������������������ȵ��øú���
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pPrtclParser	Э�������ָ��
		 *	\param	tIOCfgPort		Port������Ϣ
		 *	\param	nTimeOut		��ʱʱ��
		 *
		 *	\retval	int		0Ϊ�ɹ�����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int Init( CIOPrtclParserBase* pPrtclParser, tIOPortCfg portCfg, long nTimeOut);

		/*!
		 *	\brief	ͨ����·��������
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pbyData		Ҫ����������ָ��
		 *	\param	nDataLen	Ҫ�������ݳ���
		 *
		 *	\retval	int		0Ϊ�ɹ�����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int Write( unsigned char * pbyData, int nDataLen );

		/*!
		*	\brief	ʹ��IO��������ʼ����
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual int open( );

		/*!
		*	\brief	ֹͣ��IO������
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual int close();

		/*!
		*	\brief	������ʱ��
		*
		*	\param	nTimeOut ��ʱ��ʱ����
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual int SetTimer( long nTimeOut );

		/*!
		*	\brief	ֹͣ��ʱ��
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual int StopTimer();

		/*!
		*	\brief	��������״̬
		*
		*	\retval	��������Ϊtrue ����Ϊfalse 
		*/
		virtual bool ConnState();

		/*!
		*	\brief	�Ͽ�����
		*/
		virtual int StopConn();

		/*!
		*	\brief	���ӣ����ӵ�ַ�Ͷ˿ڲ��䣬��������
		*
		*	��������趨�ĳ�ʱʱ�仹û�����ӳɹ����򷵻�ʧ��
		*
		*	\param	nTimeOut ��ʱʱ��
		*
		*	\retval	true��ʾ���ӳɹ���false��ʾ����ʧ��
		*/
		virtual bool Connect( long nTimeOut );

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
		virtual bool Connect( long nTimeOut, const char* pPeerIP, int nPort );

		/*!
		*	\brief	���ӣ����ӵ�ַ�Ͷ˿ڲ��䣬��������
		*
		*	������������
		*
		*	\retval	int
		*
		*	\note	���ؽ������ʾ�ɰܣ���Ҫ�ⲿ��ʱ������ȷ�������Ƿ�ɹ�
		*/
		virtual int  Connect( );

		/*!
		*	\brief	���ӣ����ӵ�ַ�Ͷ˿ڲ��䣬��������
		*
		*	������������
		*
		*	\retval	int
		*
		*	\note	���ؽ������ʾ�ɰܣ���Ҫ�ⲿ��ʱ������ȷ�������Ƿ�ɹ�
		*/
		virtual int  Connect( const char* pPeerIP, int nPort );

		/*!
		*	\brief	��ȡ����
		*
		*	\retval	std::string ����
		*/
		std::string GetClassName();

	protected:
		//! Э����ָ��
		CIOPrtclParserBase* m_pPrtclParser;	
		//! Port������Ϣ
		tIOPortCfg m_tDrvBoardCfg;					
		//! ��ʱ��ʱ�����
		long m_nTimeOut;

	public:
		//! ����
		static std::string s_ClassName;
	};

	
#ifdef IODRVBOARD_EXPORTS
#define IODRVBOARD_VAR MACS_DLL_EXPORT
#else
#define IODRVBOARD_VAR MACS_DLL_IMPORT
#endif
	//!�����������๤��ȫ�ֱ���
	extern DBBASECLASSES_API GenericFactory<CIOBoardBase> g_IOBoardFactory;

#ifndef IODRVBOARD_EXPORTS
	extern "C" MACS_DLL_IMPORT int InitDll();
	extern "C" MACS_DLL_IMPORT int UnInitDll();
#endif

}

#endif