/*!
*	\file	RuntimeCatlog.h
*
*	\brief	�����ļ�Ŀ¼
*
*	����ļ��ο�"MACS-SCADA ϵͳĿ¼����SystemCatlog.xsd"
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
	const char Security_ROOT[] = ACE_TEXT("Access/");	//SecurityĿ¼�޸ĳ�Access

	const char DebugLog_ROOT[] = ACE_TEXT("DebugLog/");	//!< ������Ϣ���Ŀ¼

	const char Files_ROOT[] = ACE_TEXT("Files/");	//!< ����������װ���Ŀ¼
	const char Start_ROOT[] = ACE_TEXT("Start/");	//!< �����������д��Ŀ¼
	const char Hot_ROOT[] = ACE_TEXT("Hot/");		//!< �ȱ������ݴ��Ŀ¼

	/*!
	*	\class	CRuntimeCatlog
	*
	*	\brief	�����ļ�Ŀ¼��Ϣ��
	*/
	class CRuntimeCatlogBase
	{
	protected:
		CRuntimeCatlogBase() : m_setupDir( ACE_TEXT("") ), m_runDir( ACE_TEXT("") ), m_histDir( ACE_TEXT("") )
		{
		}

		virtual ~CRuntimeCatlogBase(){}
		/*!
		*	\brief	�������ļ��л�ȡ�����װĿ¼��Ϣ
		*/
		virtual void init() = 0;


	public:
		/*!
		*	\brief	���ַ����е�'\'�滻Ϊ'/'���Զ�Windows��Unix��ͳһĿ¼����
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
		*	\brief	���ַ����е�'\'�滻Ϊ'/'���Զ�Windows��Unix��ͳһĿ¼����
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
		*	\brief	��ȡӦ�ó�������·��
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
		*	\brief	���ɲ�ͬOS����ȷ��dynamic library�ļ���
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
		std::string m_setupDir;	//!< �����װĿ¼
		std::string m_runDir;	//!< ����Ŀ¼
		std::string m_histDir;	//!< ��ʷĿ¼
		std::string m_dLDir;	//!< ��װĿ¼
	};

	const char IniFile[] = ACE_TEXT("IOServer.ini");
	class CRuntimeCatlog_ini : public CRuntimeCatlogBase
	{
	public:
		CRuntimeCatlog_ini(){}
		virtual ~CRuntimeCatlog_ini(){}
	protected:
		/*!
		*	\brief	�����ļ�Ϊini��ʽ���ļ���ΪMACS-SCADA.ini���洢��OSĿ¼
		*
		*	\note	��Windows���ļ���ʽΪ��
		*			[INIT]
		*			SetupDir=D:/MACS-SCADAϵͳ���
		*			RunDir=D:/MACS-SCADAϵͳ���/Run
		*
		*	\note	��Unix���ļ���ʽΪ��
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
			//! ��ȡȱʡini�ļ����Ŀ¼
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

			//! ��ȡ��װĿ¼
			ACE_TString setupDir;
			cf.get_string_value(InitSection, ACE_TEXT("SetupDir"), setupDir);
			normal( setupDir );
			if( setupDir.length() > 0 )
			{
				m_setupDir = setupDir.c_str();
				if( m_setupDir[ m_setupDir.length()-1 ] != '/')
					m_setupDir += "/";
			}

			//! ��ȡ����Ŀ¼
			ACE_TString runDir;
			cf.get_string_value(InitSection, ACE_TEXT("RunDir"), runDir);
			normal( runDir );
			if( runDir.length() > 0 )
			{
				m_runDir = runDir.c_str();
				if( m_runDir[ m_runDir.length()-1 ] != '/')
					m_runDir += "/";
			}

			//! ��ȡ��ʷĿ¼
			ACE_TString histDir;
			cf.get_string_value(InitSection, ACE_TEXT("HistDir"), histDir);
			normal( histDir );
			if( histDir.length() > 0 )
			{
				m_histDir = histDir.c_str();
				if( m_histDir[ m_histDir.length()-1 ] != '/')
					m_histDir += "/";
			}

			//! ��ȡ��װĿ¼
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
	*	��Singleton������exe�Ͷ��dll�д��ڲ�ͬ�ĵ���ʵ��������Ҫ����
	*/
	typedef ACE_Singleton<CRuntimeCatlog_ini, ACE_Null_Mutex> SingletonRuntimeCatlog;

};	// namespace MACS_SCADA_SUD

#endif
