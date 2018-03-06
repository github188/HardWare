/*!
 *	\file	IONetCltBoardThread.h
 *
 *	\brief	��Ϣѭ���߳���ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��11��	14:17
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IONetCltBoardThread_20080331_H
#define _IONetCltBoardThread_20080331_H

#include "ace/Proactor.h"
#include "ace/Task_T.h"
#include "ace/TP_Reactor.h"
#include "ace/WIN32_Proactor.h"
#include "utilities/fepcommondef.h"

namespace MACS_SCADA_SUD
{
	class CIOTcpIpConnector;
	class CIONetSynConnector;

	class ACE_ReactorEX : public ACE_Reactor
	{
	public:
		ACE_ReactorEX (ACE_Reactor_Impl *implementation = 0, int delete_implementation = 0);

		virtual ~ACE_ReactorEX(void);

		virtual int run_reactor_event_loop (REACTOR_EVENT_HOOK = 0);

	};


	/*!
	*	\class	IOComBoardDealThread
	*
	*	\brief	��Ϣѭ���߳��ࣨwindows��Ϊǰ��ʽ������ϵͳ��Ϊ��Ӧʽ����������Ϣѭ��
	*/
	class CIONetCltBoardThread : public ACE_Task<ACE_MT_SYNCH>
	{
	public:
		/*!
		*	\brief	ȱʡ���캯��
		*/
		CIONetCltBoardThread( void );

		/*!
		*	\brief	ȱʡ��������
		*/
		virtual ~CIONetCltBoardThread( void );

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
		*
		*	\retval	int	0��ʾ�ɹ��������ʧ��
		*/
		virtual int Init(tIOPortCfg tDrvCfg, CIOPrtclParserBase *pPrtcl);

		/*!
		*	\brief	��������
		*
		*	\param	pbyData Ҫд���ݵ�ͷָ��
		*	\param	nDataLen Ҫд���ݵĳ���
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		int Write( const Byte* pbyData, int nDataLen );

#ifdef _WINDOWS
		CIONetSynConnector* GetConnector()		
#else
		CIOTcpIpConnector* GetConnector()
#endif
		{
			return m_pConnector;
		}

	private:
#ifdef _WINDOWS
		ACE_WIN32_Proactor m_proactorImpl;
		ACE_Proactor* proactor_;				//! ǰ������windows�ϣ�
		CIONetSynConnector* m_pConnector;		//! �첽������������������ǰ��ʽ�¼�������
#else
		ACE_TP_Reactor m_selReactor;
		ACE_ReactorEX* reactor_;				//! ��Ӧ������linuxϵͳ�ϣ�
		CIOTcpIpConnector* m_pConnector;		//! �����������������÷�Ӧʽ�¼�������
#endif
		tIOPortCfg m_tDrvBoardCfg;				//! �˿ڲ���
		CIOPrtclParserBase* m_pPrtclParser;		//! Э��ָ��

		//! ����������Ӳ����Ѿ������ӱ������Ϊtrue������Ϊfalse
		bool m_bWorking;      
	};
}

#endif