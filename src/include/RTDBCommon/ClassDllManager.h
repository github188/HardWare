#ifndef _IOCLASSDLLMGRIMPL_H_20080424134913_
#define _IOCLASSDLLMGRIMPL_H_20080424134913_

#pragma once

#include "ace/DLL.h"
#include "ace/Singleton.h"
#include "ace/Synch.h"
#include "OS_Ext.h"

#include <string>
#include <vector>
#include <map>

#ifdef DBCOMMON_EXPORTS 
#define CLASSDLLMGR_EXT  MACS_DLL_EXPORT
#else
#define CLASSDLLMGR_EXT  MACS_DLL_IMPORT
#endif

/////////////////////////////////////////////////////////////////////////////

/*!
 *	\file	ClassDllManager.h
 *
 *	\brief	ʵ���ඨ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author	
 *
 *	\date	2006-04-24 13:49:13
 *
 *	\versEasn	1.0
 *
 *	\attentEasn	Copyright (c) 2002-2008, MACS-SCADAϵͳ������,HollySys Co.,Ltd
 *	\attentEasn	All rights reserved.
 */

namespace MACS_SCADA_SUD
{
	/*!
	*	\class	ClassDllManager
	*
	*	\brief	��̬����ع���
	*/
	class CLASSDLLMGR_EXT CClassDllManager
	{
	public:
		typedef std::map<std::string,ACE_DLL*> DLL_HANDLE_MAP_TYPE;

		CClassDllManager();
		~CClassDllManager();

		//////////////////////////////////////////////////////////////////////
		//Add your Interface Functions Here.
		

		//////////////////////////////////////////////////////////////////////
		//Override virtual member functions.
		/*!
		*	\brief	��ʼ����̬�⡣
		*
		*	\param	����	��
		*
		*	\retval	����ֵ	0Ϊ�ɹ�������Ϊʧ��
		*
		*	\note		ע�������ѡ��
		*/
		int Init();			

		/*!
		*	\brief	ж�ض�̬�⡣
		*
		*	\param	����	��
		*
		*	\retval	����ֵ	0Ϊ�ɹ�������Ϊʧ��
		*
		*	\note		ע�������ѡ��
		*/
		int UnInit();		

		/*!
		*	\brief	���Ժ���
		*
		*	\param	����	��
		*
		*	\retval	����ֵ	0Ϊ�ɹ�������Ϊʧ��
		*
		*	\note		ע�������ѡ��
		*/
		int Test();			

		ACE_DLL* Exist(std::string DllName);
		ACE_DLL* LoadDll(std::string DllName);
		ACE_DLL* LoadDrvDll(std::string DllName);
		ACE_DLL* GetDll(std::string DllName);
		
	private:
		//Add your Implement Here.
		DLL_HANDLE_MAP_TYPE m_dll_handle_map;

		std::string GetModulePath(void);
		//! ��������̻߳���
		ACE_Thread_Mutex m_MapMutex;
	};

	typedef ACE_Singleton<CClassDllManager, ACE_Null_Mutex> SingletonDllManager;
}

#endif
