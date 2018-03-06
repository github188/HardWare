/*!
 *	\file	IOSrlBoardDealThread.h
 *
 *	\brief	��Ϣѭ���߳���ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��10��	14:47
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
	 *	\brief	��Ϣѭ���߳��ࣨwindows��Ϊǰ��ʽ������ϵͳ��Ϊ��Ӧʽ����������Ϣѭ��
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class CIOSrlBoardDealThread : public ACE_Task<ACE_MT_SYNCH>
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOSrlBoardDealThread(void);

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIOSrlBoardDealThread(void);

		/*!
		*	\brief	������Ϣѭ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		virtual int open();

		/*!
		*	\brief	�˳���Ϣѭ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		int close();

		/*!
		*	\brief	��Ϣѭ���߳���ڣ�������Ϣѭ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		virtual int svc( void );

		/*!
		*	\brief	��ʼ�����ڲ���
		*
		*	\param	tDrvCfg	���ڲ���
		*	\param	pPrtcl	Э����ָ��
		*	\param	nTimeOut ��ʱʱ��
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		virtual int Init(tIOPortCfg tDrvCfg,CIOPrtclParserBase *pPrtcl,long nTimeOut);

		/*!
		*	\brief	�򴮿�д����
		*
		*	\param	pbyData Ҫд���ݵ�ͷָ��
		*	\param	nDataLen Ҫд���ݵĳ���
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		int Write( byte* pbyData, int nDataLen );

	public:
#ifdef _WINDOWS
		//! windowsϵͳ��ʹ��ǰ��ʽ�¼�������
		CIOSrlBoardHandler* m_pHandler;					
#else                
		//! ��linux����ϵͳ��ʹ�÷�Ӧʽ�¼�������
		CSrlSvcHandler* m_pHandler;						
#endif

	protected:
#ifdef _WINDOWS
		//! ǰ������windows�ϣ�
		ACE_Proactor* proactor_;						
#else
		//! ��Ӧ������linuxϵͳ�ϣ�
		ACE_Reactor* reactor_;							
#endif
		//! ��ʱʱ��
		long m_nTimeOut;								
		
		//! �˿ڲ���
		tIOPortCfg m_tDrvBoardCfg;						
		
		//! Э��ָ��
		CIOPrtclParserBase* m_pPrtclParser;				

	private:
		//! ��¼��Ϣѭ����ʼ״̬
		bool m_bOpened;									

	};
}

#endif