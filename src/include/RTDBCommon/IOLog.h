/*!
 *	\file	IOLog.h
 *
 *	\brief	��־��¼�ඨ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��9��	10:42
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOLOG_H_
#define _IOLOG_H_

#include <string>
#include "ace/OS.h"
#include "ace/Synch.h"
#include "ace/Message_Queue.h"
#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/IOTimer.h"

#ifdef DBCOMMON_EXPORTS 
#define IOLOGAPI_EXT  MACS_DLL_EXPORT
#else
#define IOLOGAPI_EXT  MACS_DLL_IMPORT
#endif

namespace MACS_SCADA_SUD
{
	/*!
	 *	\class	CIOLog
	 *
	 *	\brief	��־��
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class IOLOGAPI_EXT CIOLog
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOLog(void);

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIOLog(void);

		/*!
		 *	\brief	���췽��
		 *
		 *	\param	szFileName: ��־��¼�ļ�����
		 *
		 *	\note		ע�������ѡ��
		 */
		CIOLog(std::string szFileName);
		 
		/*!
		 *	\brief	���췽��
		 *
		 *	\param	szFileName: ��־��¼�ļ�����
		 *
		 *	\note		ע�������ѡ��
		 */
		CIOLog(std::string szFilePath,std::string szFileName);
		/*!
		*	\brief	��ʱ���¼��־
		*
		*	\param	pchContent	: Ҫ��¼������
		*	\param	bTime		: �Ƿ��ʱ��,���ΪTRUE,���ʱ��, ����,����
		*
		*	\retval: 0Ϊ�ɹ�,����ʧ��
		*/

		void Init(std::string szFileName);

		int WriteLog( const char* pchContent, bool bTime=true );

		//!д���ļ�
		int Write2File( bool bTime, FILE* stream, const char* pchContent);

		/*!
		*	\brief	����ļ��Ƿ�ﵽ���
		*
		*	\retval: trueΪ�ɹ�,falseʧ��
		*/
		bool CheckFull();

		/*!
		*	\brief	�����ļ���������
		*/
		void SetMaxLen(int lMax);

		void SetMaxLogDays(int nIndex);

		bool CheckStoreFolderPath();

		//!��־��¼�ļ�·��
		std::string m_szFilePath;

		//��ʱ��ת��Ϊ�鵵Ŀ¼//
		std::string time2AchieveStorDirName(const ACE_Time_Value& time);

		//��ʱ��ת��Ϊ�鵵Ŀ¼//
		std::string time2AchieveStorDirName(const int& nIndex);

		//��ʱ��ת��Ϊ�鵵Ŀ¼//
		std::string getNeedDelDir();

		//!�ļ����//
		int m_nCurLogIndex;

		//!���ļ�·���ַ����еķָ������и�ʽת��//
		void spritTranslation(std::string& scr, const char oldMode,const char newMode);

		static ACE_Recursive_Thread_Mutex m_mutex;
	private:
		//����־���й鵵�Ķ�ʱ��������//
		void startUpLogTimer(int time);

		//! �鵵��ʱ��ʱ��//
		CIOTimer* m_pLogTimer;				

		ActiveTimer* m_pLogTimerMgr;

		//!��־��¼�ļ�����
		std::string m_szFileName;

		//!��־��������//
		int m_nMaxLength; 

		//!����ļ����//
		int m_nMaxLogDays;

		//!�����ļ�Ŀ¼����//
		//std::string m_nOldestLogDirName;

		//!config FEP log stor exist flag
		bool m_IsCfgFolderExist; 
	};
}

#endif