/*!
*	\file	Debug_Msg.h
*
*	\brief	������Ϣ����ӿ�
*
*	�������й����в����ĸ��ֵ�����������󡢾������Ϣ����Ҫͳһ���������ϵͳ��
*	��ʵʱ���ݴ�����������Ҫ������Ӧ�ӿ��Թ�����ģ�鹫�ã�
*	Ŀǰ��ʱ���ǲ���ACE�����ṩ�ĵ��Ժ�ʵ�ָù��ܣ�����Ҫ������Ӧ��װ��ȷ���ӿڲ�����
*	��������ϵͳʵ�ֺ�����Ҫ�Է��ʽӿ��ڲ���ʵ�ֽ��и��ģ�������Ҫ�޸Ĳ�����Ϣ�ĸ���ģ�顣
*
*	$Header: /MACS-SCADA_SUD/OORTDB_Design/Server/PublicFile/DebugMsg/Debug_Msg.h 43    09-06-29 14:03 Litianhui $
*	$Author: Litianhui $
*	$Date: 09-06-29 14:03 $ 
*	$Revision: 43 $
*
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#ifndef _DEBUG_MSG_H__
#define _DEBUG_MSG_H__

#include <string>
#include <fstream>
#include <iostream>

#include "ace/OS.h"
#include "ace/SString.h"
#include "ace/Singleton.h"
#include "ace/Synch.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Msg_Callback.h"
#include "ace/Log_Record.h"
#include "ace/Task.h"
#include "ace/Configuration_Import_Export.h"

#include "OS_Ext.h"

//////////////////////ȱʡ�������������Ϣ//////////////////////////////

#ifdef _DEBUG

#define ACE_NDEBUG		0
#define ACE_NLOGGING	0
//! ��Ҫ�ڹ�����������Ӻ궨�壺/D "ACE_NTRACE=0"���ڴ˴���ӽ���ȱʡ�����ͻ������������

#endif

////////////////////////////����Ϣ��������Դ���/////////////////////////////////
/*
*	Callback objects are not inherited by the ACE_Log_Msg instances created 
*	for any threads you create. So if you're going to be using callback 
*	objects with multithreaded applications, you need to take special care 
*	that each thread is given an appropriate callback instance. 
*/
class CLog_DebugOutWnd :  public ACE_Log_Msg_Callback
{
public:
	CLog_DebugOutWnd(){}
	void log(ACE_Log_Record &log_record)
	{
		OutputDebugString( log_record.msg_data() );
	}

	typedef ACE_Singleton<CLog_DebugOutWnd, ACE_Thread_Mutex> SingletonLog_DebugOutWnd;

	static int initialize()
	{
		ACE_LOG_MSG->clr_flags( ACE_LOG_MSG->flags() );

		if( ACE_LOG_MSG->msg_callback() == 0 )
		{
			ACE_LOG_MSG->msg_callback( SingletonLog_DebugOutWnd::instance() );
		}
		ACE_LOG_MSG->set_flags(ACE_Log_Msg::MSG_CALLBACK);

		return 0;
	}
};

////////////////////////////����Ϣ�����STDERR/////////////////////////////////
class CLog_StdErr
{
public:
	static int initialize()
	{
		ACE_LOG_MSG->clr_flags( ACE_LOG_MSG->flags() );

		ACE_LOG_MSG->set_flags(ACE_Log_Msg::STDERR);

		return 0;
	}
};

/////////////////����Ϣ�����OSTREAM���ļ��洢��Ӧ�ó�������Ŀ¼////////////////////
class CLog_fstream : public fstream
{
public:
	CLog_fstream(){}
	/*!
	*  \brief  ��д��ʽ���ļ�
	*/
	void open( std::string fn )
	{
		ACE_Guard<ACE_Thread_Mutex> guard(m_mutex);

		m_fileName = fn;

		open_i(); 
	}

	/*!
	*  \brief  ����ļ�����
	*/
	void reset()
	{
		if( is_full() )
		{
			//! ����ļ�����
			fstream::close();
			fstream::open(m_fileName.c_str(), ios_base::out|ios_base::trunc);
		}
	}

	/*!
	*  \brief  �ж��ļ��Ƿ���
	*/
	bool is_full()
	{
		std::ifstream ifs( m_fileName.c_str() );
		if( ifs.is_open() )
		{
			//! ��ȡ�ļ���С
			ifs.seekg( 0, ios::end );
			unsigned long len = ifs.tellg();

			if(len > MAX_LENGTH)
				return true;
		}

		return false;
	}

private:
	void open_i()
	{
		if( is_open() ) return;

		/*!
		*	\note	The combination in|out|app is _illegal_ according to the ISO
		standard, closely followed by libstdc++-v3: see 27.8.1.3,2
		Table 11--File open modes

		+---------------------------------------------------+
		|   ios_base Flag combination      stdio equivalent |
		|binary   in   out   trunc   app                    |
		+---------------------------------------------------+
		|               +                  "w"              |
		+---------------------------------------------------+
		|               +             +    "a"              |
		+---------------------------------------------------+
		|               +      +           "w"              |
		+---------------------------------------------------+
		|         +                        "r"              |
		+---------------------------------------------------+
		|         +     +                  "r+"             |
		+---------------------------------------------------+
		|         +     +      +           "w+"             |
		+---------------------------------------------------+
		|  +            +                  "wb"             |
		+---------------------------------------------------+
		|  +            +             +    "ab"             |
		+---------------------------------------------------+
		|  +            +      +           "wb"             |
		+---------------------------------------------------+
		|  +      +                        "rb"             |
		+---------------------------------------------------+
		|  +      +     +                  "r+b"            |
		+---------------------------------------------------+
		|  +      +     +      +           "w+b"            |
		+---------------------------------------------------+
		If mode is not some combination of flags shown in the table then the
		open fails.
		*/
		//! ��׷�ӷ�ʽ��
		fstream::open(m_fileName.c_str(), ios_base::out | ios_base::app);
		reset();
	}

private:
	//! �ļ����Ϊ20M
	const static unsigned long MAX_LENGTH = 20*1024*1024;

	ACE_Thread_Mutex m_mutex;

	std::string m_fileName;
};

class CLog_DebugFile : public ACE_Task_Base
{
public:
	typedef ACE_Singleton<CLog_DebugFile, ACE_Thread_Mutex> SingletonLogDF;
	CLog_DebugFile() : m_exit( false ) , m_strNameExt( "" )
	{
	}
	virtual int svc (void)
	{
		while( !m_exit )
		{
			try{
				//! �жϵ�ǰ�Ƿ����������Ϣ���ļ�
				if( ACE_BIT_ENABLED( ACE_LOG_MSG->flags(), ACE_Log_Msg::OSTREAM ) )
				{
					

                                   //ACE_Log_Msg *pLog_Msg=ACE_LOG_MSG;
                                   //printf("svc,LogMsg:%X\n",(void *)pLog_Msg);
					//! ά���ļ���С
					CLog_fstream *fs = dynamic_cast<CLog_fstream*>( ACE_LOG_MSG->msg_ostream() );
                                   //printf("SVC,msg_ostream:%X\n",(void *)ACE_LOG_MSG->msg_ostream());	
					
					if(fs){
						//printf("fs is not NULL!!\n");
						//printf("SVC,CLog_fstream:%X\n",(void *)fs);

						if ( fs->is_full() )             //!< �ļ��������
						{
							u_long flags = ACE_LOG_MSG->flags();	//!< ������־���ļ�ά����ɺ���Ҫ�ָ�
							ACE_LOG_MSG->clr_flags( flags );
							ACE_LOG_MSG->msg_ostream( 0 );
							fs->reset();
	
							ACE_OS::sleep(1);
							ACE_LOG_MSG->msg_ostream( fs );
							ACE_LOG_MSG->set_flags(flags);
						}
					}
					else
					{
						//printf("fs is NULL!!\n");
						
						CLog_DebugFile *log = SingletonLogDF::instance();
					
						CLog_fstream *lfs = log->get_fstream();
						lfs->open( log->GetFileName() );
			
						ACE_OSTREAM_TYPE *output = (ACE_OSTREAM_TYPE *)( lfs );
						ACE_LOG_MSG->msg_ostream( output );
						
						//printf("SVC,new CLog_fstream:%X\n",(void *)fs);
						//printf("SVC,new msg_ostream:%X\n",(void *)output );
					}
				}
			}
			catch(...)
			{
				throw;
			}

			ACE_OS::sleep( 60 );
		}


		return 0;
	};

	void terminate()
	{
		m_exit = true;
	}

	void setNameExt( std::string strNameExt )
	{
		m_strNameExt = strNameExt;
	}
public:

	static int initialize()
	{
		
		//ACE_Log_Msg *pLog_Msg=ACE_LOG_MSG;
		//printf("Initiallize,LogMsg:%X\n",(void *)pLog_Msg);
		ACE_LOG_MSG->clr_flags( ACE_LOG_MSG->flags() );
		
		//printf("Initiallize,msg_ostream:%X\n",(void *)ACE_LOG_MSG->msg_ostream());		
		if( ACE_LOG_MSG->msg_ostream() == 0 )
		{
			CLog_DebugFile *log = SingletonLogDF::instance();

			CLog_fstream *fs = log->get_fstream();
			fs->open( log->GetFileName() );

			ACE_OSTREAM_TYPE *output = dynamic_cast<ACE_OSTREAM_TYPE *>( fs );
			ACE_LOG_MSG->msg_ostream( output );
			//printf("Initiallize,new CLog_fstream:%X\n",(void *)fs);
			//printf("Initiallize,new msg_ostream:%X\n",(void *)output );

			//! ����ά���߳�
			log->activate(THR_NEW_LWP | THR_DETACHED | THR_INHERIT_SCHED);
		}
		ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);

		return 0;
	};

	static void shutdown()
	{
		SingletonLogDF::instance()->terminate();
	};

private:
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
	std::string GetSetupDir()
	{
		int status;
		std::string setup_dir("");

		ACE_Configuration_Heap cf;
		status = cf.open();
		if( status != 0 ) return setup_dir;

		ACE_Ini_ImpExp import(cf);

		char chDir[MAX_PATH];

		//! ��ȡȱʡini�ļ����Ŀ¼
		GetDefaultIniDirectory( chDir , MAX_PATH);

		const char IniFile[] = ACE_TEXT("/SCADA.ini");
		std::string file_name(chDir);
		file_name += IniFile;

		status = import.import_config( file_name.c_str() );
		if (status != 0) return setup_dir;

		const ACE_Configuration_Section_Key &root = cf.root_section ();
		ACE_Configuration_Section_Key InitSection;
		status = cf.open_section( root, ACE_TEXT("INIT"), 0,InitSection);
		if( status != 0) return setup_dir;

		//! ��ȡ��װĿ¼
		ACE_TString setupDir;
		cf.get_string_value(InitSection, ACE_TEXT("SetupDir"), setupDir);

		//! ���ַ����е�'\'�滻Ϊ'/'
		size_t len = setupDir.length();
		for( size_t i=0; i<len; i++ )
		{
			if( setupDir[i] == '\\')
				setupDir[i] = '/';
		}

		//! ȷ����β�ַ�Ϊ'/'
		if( setupDir.length() > 0 )
		{
			setup_dir = setupDir.c_str();
			if( setup_dir[ setup_dir.length()-1 ] != '/')
				setup_dir += "/";
		}

		return setup_dir;
	}

	std::string GetFileName()
	{
		char driver[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath( __argv[0], driver, dir, fname, ext );

		//! ��־��Ϣ�ļ�ͳһ�洢�ڡ�[�����װĿ¼]/DebugLog/��Ŀ¼��
		std::string szFileName;

		szFileName = GetSetupDir();
		if( szFileName == ACE_TEXT("") )
		{
			//! ��ȡ�����ļ�ʧ�ܣ�ֻ�ô洢��Ӧ�ó�������Ŀ¼
			szFileName = std::string(driver) + dir;
		}
		else
		{
			szFileName += ACE_TEXT("DebugLog/");

			ACE_stat stFileInfo;
			memset(&stFileInfo, 0, sizeof(ACE_stat));

			if(ACE_OS::stat(szFileName.c_str(), &stFileInfo))
			{
				ACE_OS::mkdir(szFileName.c_str());
			}
		}

		szFileName = szFileName + fname + m_strNameExt + ACE_TEXT(".log");

		return szFileName;
	};

	CLog_fstream *get_fstream()
	{
		return &m_fs;
	}

private:
	CLog_fstream m_fs;		//!< ��־�ļ���
	bool m_exit;
	std::string m_strNameExt;//�����ļ���
};

////////////////////////////����Ϣ�����SYSLOG/////////////////////////////////
class CLog_SysLog
{
public:
	static int initialize(std::string logger_key)
	{
		ACE_LOG_MSG->clr_flags( ACE_LOG_MSG->flags() );

		ACE_LOG_MSG->open( __argv[0], ACE_Log_Msg::SYSLOG, logger_key.c_str() );

		return 0;
	}
};

////////////////////////////������κ���Ϣ/////////////////////////////////
class CLog_Null
{
public:
	static int initialize()
	{
		ACE_LOG_MSG->clr_flags( ACE_LOG_MSG->flags() );

		ACE_LOG_MSG->set_flags( ACE_Log_Msg::SILENT );

		return 0;
	}
};

////////////////////////////�ͷŴ����Ķ���/////////////////////////////////
class CLog_Cleanup
{
public:
	static int shutdown()
	{
		CLog_Null::initialize();

		return 0;
	}
};

/*!
*	\class	CLog_TSS
*
*	\brief	����MSG_CALLBACK��OSTREAM��Ҫ��ÿ���̷ֱ߳�����
*/
class CLog_TSS
{
public:
	static void initialize()
	{
		if( ACE_LOG_MSG->flags() == ACE_Log_Msg::OSTREAM && ACE_LOG_MSG->msg_ostream() == 0 )
			CLog_DebugFile::initialize();

		if( ACE_LOG_MSG->flags() == ACE_Log_Msg::MSG_CALLBACK && ACE_LOG_MSG->msg_callback() == 0 )
			CLog_DebugOutWnd::initialize();
	};
};

////////////////////////////�����//////////////////////////////////////
/*!
*	\brief	 ��16���Ƹ�ʽ�������������
*
*	���÷�ʽΪ��MACS_HEX_DUMP((level, buffer, size [,text]));
*	Dumps the buffer as a string of hex digits. If provided, 
*	the optional text parameter will be printed prior to the hex string. 
*
*	\param	level,��Ϣ���
*	\param	buffer,������ָ��
*	\param	size,��������С
*	\param	[,text]�����ı�
*/
//#define MACS_HEX_DUMP ACE_HEX_DUMP
#define MACS_HEX_DUMP(X) { ACE_HEX_DUMP(X); CLog_TSS::initialize(); }

/*!
*	\brief	 �������warnings�ʹ���errors
*
*	���÷�ʽΪ��MACS_ERROR((level, string, ...));
*
*	\param	level,��Ϣ���
*	\param	string,������Ϣ��֧�ָ�ʽָʾ��������printf()
*/
//#define MACS_ERROR ACE_ERROR
#define MACS_ERROR(X) { ACE_ERROR(X); CLog_TSS::initialize(); }

/*!
*	\brief	 ���������Ϣ
*
*	���÷�ʽΪ��MACS_DEBUG((level, string, ...));
*
*	\param	level,��Ϣ���
*	\param	string,������Ϣ��֧�ָ�ʽָʾ��������printf()
*/
//#define MACS_DEBUG ACE_DEBUG
#define MACS_DEBUG(X) { ACE_DEBUG(X); CLog_TSS::initialize(); }

/*	�ڶ��ֶ��巽ʽ
#define MACS_DEBUG(X) \
do { \
CLog_TSS::initialize(); \
int __ace_error = ACE_Log_Msg::last_error_adapter (); \
ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
ace___->conditional_set (__FILE__, __LINE__, 0, __ace_error); \
ace___->log X; \
} while (0)
*/

////////////////////////////��Ϣ����//////////////////////////////////////
#define MSG_INFO	LM_INFO			//!< Info messages 
#define MSG_DEBUG	LM_DEBUG		//!< Debugging information
#define MSG_WARNING LM_WARNING		//!< Warning messages
#define MSG_ERROR	LM_ERROR		//!< Error messages

////////////////////////////��ʽָʾ��//////////////////////////////////////
/*!
*	Code		Argument Type	Displays
*	c			char			Single character
*	d			int				Decimal number
*	e,E,f,F,g,G	double			Double-precision floating-point number
*	l			��				�к�,Line number where logging macro appears
*	M			��				���ؼ����������ʽ,Text form of the message severity level
*	N			��				�ļ���,File name where logging macro appears
*	n			��				Ӧ�ó�����,Program name
*	o			int				Octal number
*	P			��				��ǰ����ID,Current process ID
*	s			char*			string
*	T			��				��ǰʱ��,Current time as hour:minute:sec.usec
*	D			��				��ǰ����,Timestamp as month/day/year hour:minute:sec.usec
*	t			��				�߳�ID,Calling thread's ID (1 if single threaded)
*	u			int				Unsigned decimal number
*	x,X			int				Hexadecimal number
*	@			void*			Pointer value in hexadecimal
*
*	���������ʽ��ϣ�" %D	%n(%P):%t	"------"���� ʱ��	Ӧ�ó�����(Ӧ�ó���ID):�߳�ID	"
*/
#define INFO_PERFIX		MSG_INFO,		ACE_TEXT("\n%D	")
#define DEBUG_PERFIX	MSG_DEBUG,		ACE_TEXT("\n%D	")
#define WARNING_PERFIX	MSG_WARNING,	ACE_TEXT("\ndate:%D %n:%t	")
#define ERROR_PERFIX	MSG_ERROR,		ACE_TEXT("\n====== date:%D %n:%t	")

#endif
