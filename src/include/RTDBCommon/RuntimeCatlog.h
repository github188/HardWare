/*!
*	\file	RuntimeCatlog.h
*
*	\brief	运行文件目录
*
*	设计文件参考"MACS-SCADA 系统目录－－SystemCatlog.xsd"
*
*	$Header: /MACS-SCADA_SUD/OORTDB_Design/Server/publicfile/IndexFiles/RuntimeCatlog.h 16
*	$Author:
*	$Date: 07-12-12 11:35 $
*	$Revision: 16 $
*
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#ifndef _RUNTIME_CATLOG_H__
#define _RUNTIME_CATLOG_H__

#include <string>

#include "ace/OS.h"
#include "ace/Singleton.h"
#include "ace/Synch.h"
#include "ace/Configuration_Import_Export.h"
#include "RTDBCommon/OS_Ext.h"

namespace MACS_SCADA_SUD
{

	const char OPS_ROOT[] = ACE_TEXT("OPS/");
	const char Server_ROOT[] = ACE_TEXT("Server/");
	const char FEP_ROOT[] = ACE_TEXT("FEP/");
	const char Security_ROOT[] = ACE_TEXT("Access/");	//Security目录修改成Access

	const char DebugLog_ROOT[] = ACE_TEXT("DebugLog/");	//!< 调试信息输出目录

	const char Files_ROOT[] = ACE_TEXT("Files/");	//!< 配置数据下装存放目录
	const char Start_ROOT[] = ACE_TEXT("Start/");	//!< 配置数据运行存放目录
	const char Hot_ROOT[] = ACE_TEXT("Hot/");		//!< 热备份数据存放目录

	/*!
	*	\class	CRuntimeCatlog
	*
	*	\brief	运行文件目录信息类
	*/
	class CRuntimeCatlogBase
	{
	protected:
		CRuntimeCatlogBase() : m_setupDir( ACE_TEXT("") ), m_runDir( ACE_TEXT("") ), m_histDir( ACE_TEXT("") )
		{
		}

		virtual ~CRuntimeCatlogBase(){}
		/*!
		*	\brief	从配置文件中获取软件安装目录信息
		*/
		virtual void init() = 0;


	public:
		/*!
		*	\brief	将字符串中的'\'替换为'/'，以对Windows和Unix下统一目录处理
		*/
		static void normal(ACE_TString& str)
		{
			size_t len = str.length();
			for( size_t i=0; i<len; i++ )
			{
				if( str[i] == '\\')
					str[i] = '/';
			}
		}
		/*!
		*	\brief	将字符串中的'\'替换为'/'，以对Windows和Unix下统一目录处理
		*/
		static void normal(std::string& str)
		{
			size_t len = str.size();
			for( size_t i=0; i<len; i++ )
			{
				if( str[i] == '\\')
					str[i] = '/';
			}
		}

		/*!
		*	\brief	获取应用程序所在路径
		*/
		static std::string GetModulePath()
		{
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];

			_splitpath( __argv[0], drive, dir, fname, ext );

			std::string szModule;
			szModule = drive;
			szModule+= dir;

			return szModule;
		}

		/*!
		*	\brief	生成不同OS下正确的dynamic library文件名
		*/
		static std::string GenerateModuleName(const char *filename)
		{
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];

			_splitpath( filename, drive, dir, fname, ext );

			std::string szModuleName = std::string(_MODULE_PREFIX) + fname + std::string(_MODULE_POSTFIX);
			return szModuleName;
		}

		std::string GetSetupDir()
		{
			if( m_setupDir.length() <= 0 )
				this->init();

			return m_setupDir;
		}

		std::string GetRunDir()
		{
			if( m_runDir.length() <= 0 )
				this->init();

			return m_runDir;
		}

		std::string GetHistDir()
		{
			if( m_histDir.length() <= 0 )
				this->init();

			return m_histDir;
		}

		std::string GetDLDir()
		{
			if( m_dLDir.length() <= 0 )
				this->init();

			return m_dLDir;
		}

		std::string GetOps()
		{
			return GetRunDir()+OPS_ROOT;
		}
		std::string GetServer()
		{
			return GetRunDir()+Server_ROOT;
		}
		std::string GetFep()
		{
			return GetRunDir()+FEP_ROOT;
		}

		std::string GetDebugLog()
		{
			return GetSetupDir()+DebugLog_ROOT;
		}

		std::string GetServerStart()
		{
			return GetServer()+Start_ROOT;
		}

		std::string GetServerFiles()
		{
			return GetServer()+Files_ROOT;
		}

		std::string GetServerHot()
		{
			return GetServer()+Hot_ROOT;
		}

	protected:
		std::string m_setupDir;	//!< 软件安装目录
		std::string m_runDir;	//!< 运行目录
		std::string m_histDir;	//!< 历史目录
		std::string m_dLDir;	//!< 下装目录
	};

	const char IniFile[] = ACE_TEXT("IOServer.ini");
	class CRuntimeCatlog_ini : public CRuntimeCatlogBase
	{
	public:
		CRuntimeCatlog_ini(){}
		virtual ~CRuntimeCatlog_ini(){}
	protected:
		/*!
		*	\brief	配置文件为ini格式，文件名为MACS-SCADA.ini，存储于OS目录
		*
		*	\note	在Windows下文件格式为：
		*			[INIT]
		*			SetupDir=D:/MACS-SCADA系统软件
		*			RunDir=D:/MACS-SCADA系统软件/Run
		*
		*	\note	在Unix下文件格式为：
		*			[INIT]
		*			SetupDir=/MACS-SCADA System
		*			RunDir=/MACS-SCADA System/Run
		*/
		void init()
		{
			int status;
			ACE_Configuration_Heap cf;
			status = cf.open();
			if( status != 0 ) return;

			ACE_Ini_ImpExp import(cf);

			char chDir[MAX_PATH];
			ACE_OS::memset(chDir, 0, MAX_PATH);
#ifdef _WINDOWS			
			//! 获取缺省ini文件存放目录
			//GetDefaultIniDirectory( chDir , MAX_PATH);
			GetModuleFileName(NULL, chDir, MAX_PATH);
#else
			sprintf( link, "/proc/%d/exe", getpid() ); 
			int i = readlink( link, chDir, sizeof( chDir ) );
			path[i] = '\0';
#endif
			GetModulePath();
			std::string file_name(chDir);
			std::string szkey = "\\";
			std::size_t found = file_name.rfind(szkey);

			file_name.replace(++found, file_name.length() - found, IniFile);
			//file_name += IniFile;

			status = import.import_config( file_name.c_str() );
			if (status != 0)
			{
				//TRACE1_ERROR(SERVER_CORE,"CRuntimeCatlog_ini::init() failed, file=%s!\n", IniFile)
				return;
			}

			const ACE_Configuration_Section_Key &root = cf.root_section ();
			ACE_Configuration_Section_Key InitSection;
			status = cf.open_section( root, ACE_TEXT("INIT"), 0,InitSection);
			if( status != 0) return;

			//! 获取安装目录
			ACE_TString setupDir;
			cf.get_string_value(InitSection, ACE_TEXT("SetupDir"), setupDir);
			normal( setupDir );
			if( setupDir.length() > 0 )
			{
				m_setupDir = setupDir.c_str();
				if( m_setupDir[ m_setupDir.length()-1 ] != '/')
					m_setupDir += "/";
			}

			//! 获取运行目录
			ACE_TString runDir;
			cf.get_string_value(InitSection, ACE_TEXT("RunDir"), runDir);
			normal( runDir );
			if( runDir.length() > 0 )
			{
				m_runDir = runDir.c_str();
				if( m_runDir[ m_runDir.length()-1 ] != '/')
					m_runDir += "/";
			}

			//! 获取历史目录
			ACE_TString histDir;
			cf.get_string_value(InitSection, ACE_TEXT("HistDir"), histDir);
			normal( histDir );
			if( histDir.length() > 0 )
			{
				m_histDir = histDir.c_str();
				if( m_histDir[ m_histDir.length()-1 ] != '/')
					m_histDir += "/";
			}

			//! 获取下装目录
			ACE_TString dlDir;
			cf.get_string_value(InitSection, ACE_TEXT("DLDir"), dlDir);
			normal( dlDir );
			if( dlDir.length() > 0 )
			{
				m_dLDir = dlDir.c_str();
				if( m_dLDir[ m_dLDir.length()-1 ] != '/')
					m_dLDir += "/";
			}
		}
	};

	/**
	*	该Singleton允许在exe和多个dll中存在不同的单件实例，不需要共享。
	*/
	typedef ACE_Singleton<CRuntimeCatlog_ini, ACE_Null_Mutex> SingletonRuntimeCatlog;

};	// namespace MACS_SCADA_SUD

#endif
