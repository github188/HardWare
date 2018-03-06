/*!
 *	\file	IOLog.cpp
 *
 *	\brief	
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月9日	11:52
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "ace/Date_Time.h"
#include "RTDBCommon/IOLog.h"
#include "RTDBCommon/Debug_Msg.h"

namespace MACS_SCADA_SUD
{
	
	const unsigned int ONE_DAY_MSEC = 86400000;//1day = 1000*3600*24 msec;
	const std::string WEE_HOURS = "23:53:00";
	const int M_S = 1048576; //!1M: 1024*1024
	const int MX_FILE_NAME_LENTH = 512; //最大文件长度//	
	void ArchiveStoreLog(void* p);

	ACE_Recursive_Thread_Mutex CIOLog::m_mutex;
	//!构造函数//
	CIOLog::CIOLog(void):m_nCurLogIndex(0),m_pLogTimer(NULL),m_pLogTimerMgr(NULL),
		m_nMaxLength(0), m_nMaxLogDays(0), m_IsCfgFolderExist(false)
	{
	}

	//!构造函数//
	CIOLog::CIOLog(std::string szFileName):m_pLogTimer(NULL),m_pLogTimerMgr(NULL)
	{
		Init(szFileName);
	}
	//!构造函数//
	CIOLog::CIOLog(std::string szFilePath,std::string szFileName):m_pLogTimer(NULL),m_pLogTimerMgr(NULL)
	{
		m_szFilePath = szFilePath;
		Init(szFileName);

    }

	void CIOLog::Init(std::string szFileName)
	{
		m_szFileName = szFileName+".log";
		m_nMaxLength = 2*M_S; //! 默认约2M
		m_nCurLogIndex = 0;
		m_nMaxLogDays = 0;
		m_IsCfgFolderExist = true;
	}

	CIOLog::~CIOLog(void)
	{
		if (NULL == m_pLogTimerMgr)
		{
			delete m_pLogTimerMgr;
		}

		if (NULL == m_pLogTimer)
		{
			delete m_pLogTimer;
		}
	}

	//!带时标记录日志//
	int CIOLog::WriteLog(const char* pchContent, bool bTime/*=true */)
	{		
		int nResult = -1;

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex );
		//判断文件是否已写满如果已写满先删除原有文件//
		CheckFull();

		//写打开文件//
		FILE* stream = NULL;
		stream = fopen( m_szFileName.c_str(), "a" );
		if ( !stream || false == m_IsCfgFolderExist)
		{
			return nResult;
		}

		fseek( stream, 0L, SEEK_END );

		if ( bTime )
		{
			ACE_Time_Value val = ACE_OS::gettimeofday();
			ACE_Date_Time  datetime;
			datetime.update( val );
			fprintf( stream, "%04d-%02d-%02d %02d:%02d:%02d:%d %s\n", 
				datetime.year(), datetime.month(), datetime.day(), datetime.hour(), 
				datetime.minute(), datetime.second(), datetime.microsec(), pchContent );
			nResult = 0;
		}
		else
		{
			fprintf( stream, "%s\n", pchContent );
			nResult = 0;
		}

		fclose( stream );
		return nResult;
	}

	//!检查文件是否达到最大//
	bool CIOLog::CheckFull()
	{
		//打开文件//
		FILE* stream = NULL;
		int nLen = 0;
		stream = fopen( m_szFileName.c_str(), "r" );
		if ( !stream )
		{
			return false;
		}
		fseek( stream, 0L, SEEK_END );
		nLen = ftell( stream );		
		fclose( stream );

		if ( nLen >= m_nMaxLength )
		{
			if (0 == m_nMaxLogDays)
			{
				//不需要归档，当文件达到最大长多时直接删除原文件//
				ACE_OS::unlink( m_szFileName.c_str() );
			}
			else
			{
				//重命名已经写满的文件//
				char pchBuf[MX_FILE_NAME_LENTH];		
				char pureFileName[MX_FILE_NAME_LENTH];
				int fileNameLenth = m_szFileName.length();

				//增加文件索引号前先删除 ".log"后缀//
				memset(pureFileName,0,MX_FILE_NAME_LENTH);
				memcpy(pureFileName,m_szFileName.c_str(),fileNameLenth - 4);  

				//构造新的文件名//
				memset(pchBuf,0,MX_FILE_NAME_LENTH);
				sprintf(pchBuf, "%s_%d.log", pureFileName, m_nCurLogIndex);

				rename(m_szFileName.c_str(),pchBuf);

				//文件编号增加//
				m_nCurLogIndex++;  
			}
	        
			///文件写满//
			return true;  
		}

	    ///文件没有写满//
		return false;
	}

	//!设置文件长度上限//
	void CIOLog::SetMaxLen(int lMax)
	{
		if (lMax < 0)
		{
			m_nMaxLength = M_S;
		}
		else if (lMax > 10)
		{
			m_nMaxLength = 10*M_S;
		}
		else
		{
			m_nMaxLength = lMax*M_S;
		}
	}

	void CIOLog::SetMaxLogDays( int nIndex )
	{
		//nIndex归档天数。设置的nIndex为0，说明只保留一份日志文件，即只有一份大小为m_nMaxLength文件，写满覆盖//
		if (nIndex <= 0)
		{
			m_nMaxLogDays = 0;
		}
		else if(nIndex > 90)
		{
			m_nMaxLogDays = 90;
		}
		else
		{
			m_nMaxLogDays = nIndex;
		}
		
		//设置的nIndex大于0，按天进行归档，保留最近的nIndex天内的日志//
		if (0 < m_nMaxLogDays)
		{			
			//mkdir only when create direct success.
			if (CheckStoreFolderPath())
			{
				//默认定时器周期为1秒
				startUpLogTimer(1000);
			}
		}
	}

	bool CIOLog::CheckStoreFolderPath()
	{
		bool status = false;

		if(access(m_szFilePath.c_str(),0))
		{
			MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("ERROR CIOLog::CheckStoreFolderPath Log[%s] folder path failed."), m_szFileName.c_str()));
			m_IsCfgFolderExist = false;
			status = false;
		}
		else
		{			
			status = true;
		}

		return status;
	}

	int CIOLog::Write2File( bool bTime, FILE* stream, const char* pchContent) 
	{
		int nResult = -1;
		if (stream && pchContent)
		{
			if ( bTime )
			{
				ACE_Time_Value val = ACE_OS::gettimeofday();
				ACE_Date_Time  datetime;
				datetime.update( val );
				fprintf( stream, "%04d-%02d-%02d %02d:%02d:%02d:%d %s\n", 
					datetime.year(), datetime.month(), datetime.day(), datetime.hour(), 
					datetime.minute(), datetime.second(), datetime.microsec(), pchContent );
				nResult = 0;
			}
			else
			{
				fprintf( stream, "%s\n", pchContent );
				nResult = 0;
			}
		}
		return nResult;
	}

	void CIOLog::startUpLogTimer(int time)
	{
		m_pLogTimerMgr = NULL;
		m_pLogTimer = new CIOTimer(ArchiveStoreLog, this, false, m_pLogTimerMgr );
		m_pLogTimer->SetTimer(time);
	}

	std::string CIOLog::getNeedDelDir()
	{
		std::string oldestDirStr = "";
		oldestDirStr = time2AchieveStorDirName(m_nMaxLogDays);

		//FILE* stream = fopen( oldestDirStr.c_str(), "r" );
        return oldestDirStr;
	}

	std::string  CIOLog::time2AchieveStorDirName(const ACE_Time_Value& time)
	{
		char pchBuf[MX_FILE_NAME_LENTH];
		ACE_Date_Time  datetime;
		datetime.update( time );

			
		memset(pchBuf,0,MX_FILE_NAME_LENTH);
		sprintf(pchBuf, "%04d%02d%02d",datetime.year(), datetime.month(), datetime.day());

		std::string dirNameStr = std::string(pchBuf);
		return dirNameStr;
	}


	std::string  CIOLog::time2AchieveStorDirName(const int& nIndex)
	{
		char pchBuf[MX_FILE_NAME_LENTH];
		const ACE_Time_Value curt_tv = ACE_OS::gettimeofday();

		long new_tv =  nIndex*ONE_DAY_MSEC; //将传入的天数nIndex转化为hao秒数//

		ACE_Time_Value interval = ACE_Time_Value(new_tv/1000L,new_tv%1000*1000);

		ACE_Date_Time  datetime;
		datetime.update( curt_tv - interval );

			
		memset(pchBuf,0,MX_FILE_NAME_LENTH);
		sprintf(pchBuf, "%04d%02d%02d",datetime.year(), datetime.month(), datetime.day());

		std::string dirNameStr = std::string(pchBuf);
		return dirNameStr;
	}
	void CIOLog::spritTranslation(std::string& scr, const char oldMode,const char newMode)
	{
		unsigned int srcSize = scr.length();
		for (int idx = 0; idx < srcSize; idx++)
		{
			if (oldMode == scr[idx])
			{
				scr[idx] = newMode;
			}
		}
	}
	void ArchiveStoreLog(void* p)
	{
		if (NULL == p)
		{
			return;
		}
		
		CIOLog *pIOLog = static_cast<CIOLog*>(p);		
		char pchBuf[MX_FILE_NAME_LENTH];
		//获取当前时间,如果是归档时刻，就进行归档处理//
		const ACE_Time_Value curt_tv = ACE_OS::gettimeofday();
		ACE_Date_Time  datetime;
		datetime.update(curt_tv);

		memset(pchBuf,0,MX_FILE_NAME_LENTH);
		sprintf(pchBuf, "%02d:%02d:%02d",datetime.hour(),datetime.minute(), datetime.second());
		std::string curTimeStr = std::string(pchBuf);
		
		if ( WEE_HOURS == curTimeStr)
		{	
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard(pIOLog->m_mutex );
			pid_t status = 0;
			char pCharBuf[MX_FILE_NAME_LENTH];
			std::string needRunCmdStr = "";
			std::string logFilePathStr = pIOLog->m_szFilePath;
			std::string curDayStr = pIOLog->time2AchieveStorDirName(curt_tv);
			curDayStr = logFilePathStr + curDayStr;
			//先归档前一天的日志//			
			if(access(curDayStr.c_str(),0))
			{					
				//make catalog first, and then move files
#ifndef WIN32
				needRunCmdStr = "mkdir "+ curDayStr;

				memset(pCharBuf,0,MX_FILE_NAME_LENTH);
				memcpy(pCharBuf,needRunCmdStr.c_str(),needRunCmdStr.length());
				system(pCharBuf);
				
               needRunCmdStr = "mv "+ logFilePathStr+"*.log " +curDayStr;//shell 命令格式,注意空格//
#else	
				_mkdir(curDayStr.c_str());

				pIOLog->spritTranslation(logFilePathStr,'/','\\');			
				needRunCmdStr = "move /y "+ logFilePathStr+"*.log " +curDayStr +" >nul 2>nul ";//dos 命令格式,注意空格//
#endif
				memset(pCharBuf,0,MX_FILE_NAME_LENTH);
				memcpy(pCharBuf,needRunCmdStr.c_str(),needRunCmdStr.length());
				status = system(pCharBuf);	
				if (0 != status )
				{
					MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("ERROR CIOLog::Create Store LogDir[%s] failed."),needRunCmdStr.c_str()));
				}
				else
				{
			        MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("INFOR CIOLog::Create Store LogDir[%s] finish."),needRunCmdStr.c_str()));
				}
				
			}

			pIOLog->m_nCurLogIndex = 0;

			//删除超过最大归档时间的目录//
			std::string needDelDirStr = pIOLog->getNeedDelDir();
			logFilePathStr = pIOLog->m_szFilePath;
			needDelDirStr = logFilePathStr + needDelDirStr;
			if (!access(needDelDirStr.c_str(),0))
			{

				needRunCmdStr.clear();
#ifndef WIN32
                needRunCmdStr = "rm -r "+ needDelDirStr;
#else		
				pIOLog->spritTranslation(needDelDirStr,'/','\\');
				needRunCmdStr = "rd /s/q "+ needDelDirStr +" >nul 2>nul ";
#endif
				memset(pCharBuf,0,MX_FILE_NAME_LENTH);
				memcpy(pCharBuf,needRunCmdStr.c_str(),needRunCmdStr.length());				
				status = system(pCharBuf);
				if (0 != status )
				{
					MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("ERROR CIOLog::Delete Store LogDir[%s] failed."),needRunCmdStr.c_str()));
				}
				else
				{
					MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("INFOR CIOLog::Delete Store LogDir[%s] finish."),needRunCmdStr.c_str()));
				}
			}

		}

	}

}