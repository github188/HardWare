/*!
 *	\file	IOLog.h
 *
 *	\brief	日志记录类定义
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月9日	10:42
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
	 *	\brief	日志类
	 *
	 *	详细的类说明（可选）
	 */
	class IOLOGAPI_EXT CIOLog
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOLog(void);

		/*!
		 *	\brief	缺省析构函数
		 */
		~CIOLog(void);

		/*!
		 *	\brief	构造方法
		 *
		 *	\param	szFileName: 日志记录文件名称
		 *
		 *	\note		注意事项（可选）
		 */
		CIOLog(std::string szFileName);
		 
		/*!
		 *	\brief	构造方法
		 *
		 *	\param	szFileName: 日志记录文件名称
		 *
		 *	\note		注意事项（可选）
		 */
		CIOLog(std::string szFilePath,std::string szFileName);
		/*!
		*	\brief	带时标记录日志
		*
		*	\param	pchContent	: 要记录的内容
		*	\param	bTime		: 是否带时标,如果为TRUE,则带时标, 否则,不带
		*
		*	\retval: 0为成功,其他失败
		*/

		void Init(std::string szFileName);

		int WriteLog( const char* pchContent, bool bTime=true );

		//!写入文件
		int Write2File( bool bTime, FILE* stream, const char* pchContent);

		/*!
		*	\brief	检查文件是否达到最大
		*
		*	\retval: true为成功,false失败
		*/
		bool CheckFull();

		/*!
		*	\brief	设置文件长度上限
		*/
		void SetMaxLen(int lMax);

		void SetMaxLogDays(int nIndex);

		bool CheckStoreFolderPath();

		//!日志记录文件路径
		std::string m_szFilePath;

		//将时间转化为归档目录//
		std::string time2AchieveStorDirName(const ACE_Time_Value& time);

		//将时间转化为归档目录//
		std::string time2AchieveStorDirName(const int& nIndex);

		//将时间转化为归档目录//
		std::string getNeedDelDir();

		//!文件序号//
		int m_nCurLogIndex;

		//!将文件路径字符串中的分隔符进行格式转换//
		void spritTranslation(std::string& scr, const char oldMode,const char newMode);

		static ACE_Recursive_Thread_Mutex m_mutex;
	private:
		//对日志进行归档的定时器处理函数//
		void startUpLogTimer(int time);

		//! 归档超时定时器//
		CIOTimer* m_pLogTimer;				

		ActiveTimer* m_pLogTimerMgr;

		//!日志记录文件名称
		std::string m_szFileName;

		//!日志长度上限//
		int m_nMaxLength; 

		//!最大文件序号//
		int m_nMaxLogDays;

		//!最早文件目录名称//
		//std::string m_nOldestLogDirName;

		//!config FEP log stor exist flag
		bool m_IsCfgFolderExist; 
	};
}

#endif