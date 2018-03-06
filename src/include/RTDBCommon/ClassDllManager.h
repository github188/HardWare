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
 *	\brief	实现类定义
 *
 *	详细的文件说明（可选）
 *
 *	\author	
 *
 *	\date	2006-04-24 13:49:13
 *
 *	\versEasn	1.0
 *
 *	\attentEasn	Copyright (c) 2002-2008, MACS-SCADA系统开发组,HollySys Co.,Ltd
 *	\attentEasn	All rights reserved.
 */

namespace MACS_SCADA_SUD
{
	/*!
	*	\class	ClassDllManager
	*
	*	\brief	动态库加载管理
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
		*	\brief	初始化动态库。
		*
		*	\param	参数	无
		*
		*	\retval	返回值	0为成功，其它为失败
		*
		*	\note		注意事项（可选）
		*/
		int Init();			

		/*!
		*	\brief	卸载动态库。
		*
		*	\param	参数	无
		*
		*	\retval	返回值	0为成功，其它为失败
		*
		*	\note		注意事项（可选）
		*/
		int UnInit();		

		/*!
		*	\brief	测试函数
		*
		*	\param	参数	无
		*
		*	\retval	返回值	0为成功，其它为失败
		*
		*	\note		注意事项（可选）
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
		//! 类库表访问线程互斥
		ACE_Thread_Mutex m_MapMutex;
	};

	typedef ACE_Singleton<CClassDllManager, ACE_Null_Mutex> SingletonDllManager;
}

#endif
