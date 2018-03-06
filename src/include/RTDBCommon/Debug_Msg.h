/*!
*	\file	Debug_Msg.h
*
*	\brief	调试信息输出接口
*
*	任务运行过程中产生的各种调试输出、错误、警告等信息都需要统一输出到错误系统，
*	在实时数据处理任务中需要定义相应接口以供所有模块公用，
*	目前暂时考虑采用ACE自身提供的调试宏实现该功能，但需要进行相应封装并确定接口参数。
*	当错误子系统实现后，子需要对访问接口内部的实现进行更改，而不需要修改产生信息的各个模块。
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

//////////////////////缺省允许输出所有信息//////////////////////////////

#ifdef _DEBUG

#define ACE_NDEBUG		0
#define ACE_NLOGGING	0
//! 需要在工程设置中添加宏定义：/D "ACE_NTRACE=0"，在此处添加将与缺省的相冲突，并不起作用

#endif

////////////////////////////将信息输出到调试窗口/////////////////////////////////
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

////////////////////////////将信息输出到STDERR/////////////////////////////////
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

/////////////////将信息输出到OSTREAM，文件存储于应用程序所在目录////////////////////
class CLog_fstream : public fstream
{
public:
	CLog_fstream(){}
	/*!
	*  \brief  以写方式打开文件
	*/
	void open( std::string fn )
	{
		ACE_Guard<ACE_Thread_Mutex> guard(m_mutex);

		m_fileName = fn;

		open_i(); 
	}

	/*!
	*  \brief  清空文件内容
	*/
	void reset()
	{
		if( is_full() )
		{
			//! 清空文件内容
			fstream::close();
			fstream::open(m_fileName.c_str(), ios_base::out|ios_base::trunc);
		}
	}

	/*!
	*  \brief  判断文件是否满
	*/
	bool is_full()
	{
		std::ifstream ifs( m_fileName.c_str() );
		if( ifs.is_open() )
		{
			//! 获取文件大小
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
		//! 以追加方式打开
		fstream::open(m_fileName.c_str(), ios_base::out | ios_base::app);
		reset();
	}

private:
	//! 文件最大为20M
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
				//! 判断当前是否输出调试信息到文件
				if( ACE_BIT_ENABLED( ACE_LOG_MSG->flags(), ACE_Log_Msg::OSTREAM ) )
				{
					

                                   //ACE_Log_Msg *pLog_Msg=ACE_LOG_MSG;
                                   //printf("svc,LogMsg:%X\n",(void *)pLog_Msg);
					//! 维护文件大小
					CLog_fstream *fs = dynamic_cast<CLog_fstream*>( ACE_LOG_MSG->msg_ostream() );
                                   //printf("SVC,msg_ostream:%X\n",(void *)ACE_LOG_MSG->msg_ostream());	
					
					if(fs){
						//printf("fs is not NULL!!\n");
						//printf("SVC,CLog_fstream:%X\n",(void *)fs);

						if ( fs->is_full() )             //!< 文件满则清除
						{
							u_long flags = ACE_LOG_MSG->flags();	//!< 保留标志，文件维护完成后需要恢复
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

			//! 启动维护线程
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
	std::string GetSetupDir()
	{
		int status;
		std::string setup_dir("");

		ACE_Configuration_Heap cf;
		status = cf.open();
		if( status != 0 ) return setup_dir;

		ACE_Ini_ImpExp import(cf);

		char chDir[MAX_PATH];

		//! 获取缺省ini文件存放目录
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

		//! 获取安装目录
		ACE_TString setupDir;
		cf.get_string_value(InitSection, ACE_TEXT("SetupDir"), setupDir);

		//! 将字符串中的'\'替换为'/'
		size_t len = setupDir.length();
		for( size_t i=0; i<len; i++ )
		{
			if( setupDir[i] == '\\')
				setupDir[i] = '/';
		}

		//! 确保结尾字符为'/'
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

		//! 日志信息文件统一存储在“[软件安装目录]/DebugLog/”目录下
		std::string szFileName;

		szFileName = GetSetupDir();
		if( szFileName == ACE_TEXT("") )
		{
			//! 获取配置文件失败，只好存储在应用程序所在目录
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
	CLog_fstream m_fs;		//!< 日志文件流
	bool m_exit;
	std::string m_strNameExt;//附加文件名
};

////////////////////////////将信息输出到SYSLOG/////////////////////////////////
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

////////////////////////////不输出任何信息/////////////////////////////////
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

////////////////////////////释放创建的对象/////////////////////////////////
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
*	\brief	对于MSG_CALLBACK和OSTREAM，要求每个线程分别设置
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

////////////////////////////输出宏//////////////////////////////////////
/*!
*	\brief	 以16进制格式输出缓冲区内容
*
*	调用方式为：MACS_HEX_DUMP((level, buffer, size [,text]));
*	Dumps the buffer as a string of hex digits. If provided, 
*	the optional text parameter will be printed prior to the hex string. 
*
*	\param	level,信息类别
*	\param	buffer,缓冲区指针
*	\param	size,缓冲区大小
*	\param	[,text]附加文本
*/
//#define MACS_HEX_DUMP ACE_HEX_DUMP
#define MACS_HEX_DUMP(X) { ACE_HEX_DUMP(X); CLog_TSS::initialize(); }

/*!
*	\brief	 输出警告warnings和错误errors
*
*	调用方式为：MACS_ERROR((level, string, ...));
*
*	\param	level,信息类别
*	\param	string,调试信息，支持格式指示符，类似printf()
*/
//#define MACS_ERROR ACE_ERROR
#define MACS_ERROR(X) { ACE_ERROR(X); CLog_TSS::initialize(); }

/*!
*	\brief	 输出调试信息
*
*	调用方式为：MACS_DEBUG((level, string, ...));
*
*	\param	level,信息类别
*	\param	string,调试信息，支持格式指示符，类似printf()
*/
//#define MACS_DEBUG ACE_DEBUG
#define MACS_DEBUG(X) { ACE_DEBUG(X); CLog_TSS::initialize(); }

/*	第二种定义方式
#define MACS_DEBUG(X) \
do { \
CLog_TSS::initialize(); \
int __ace_error = ACE_Log_Msg::last_error_adapter (); \
ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
ace___->conditional_set (__FILE__, __LINE__, 0, __ace_error); \
ace___->log X; \
} while (0)
*/

////////////////////////////信息级别//////////////////////////////////////
#define MSG_INFO	LM_INFO			//!< Info messages 
#define MSG_DEBUG	LM_DEBUG		//!< Debugging information
#define MSG_WARNING LM_WARNING		//!< Warning messages
#define MSG_ERROR	LM_ERROR		//!< Error messages

////////////////////////////格式指示符//////////////////////////////////////
/*!
*	Code		Argument Type	Displays
*	c			char			Single character
*	d			int				Decimal number
*	e,E,f,F,g,G	double			Double-precision floating-point number
*	l			—				行号,Line number where logging macro appears
*	M			—				严重级别的文字形式,Text form of the message severity level
*	N			—				文件名,File name where logging macro appears
*	n			—				应用程序名,Program name
*	o			int				Octal number
*	P			—				当前进程ID,Current process ID
*	s			char*			string
*	T			—				当前时间,Current time as hour:minute:sec.usec
*	D			—				当前日期,Timestamp as month/day/year hour:minute:sec.usec
*	t			—				线程ID,Calling thread's ID (1 if single threaded)
*	u			int				Unsigned decimal number
*	x,X			int				Hexadecimal number
*	@			void*			Pointer value in hexadecimal
*
*	常用输出格式组合：" %D	%n(%P):%t	"------"日期 时间	应用程序名(应用程序ID):线程ID	"
*/
#define INFO_PERFIX		MSG_INFO,		ACE_TEXT("\n%D	")
#define DEBUG_PERFIX	MSG_DEBUG,		ACE_TEXT("\n%D	")
#define WARNING_PERFIX	MSG_WARNING,	ACE_TEXT("\ndate:%D %n:%t	")
#define ERROR_PERFIX	MSG_ERROR,		ACE_TEXT("\n====== date:%D %n:%t	")

#endif
