/*!
*	\file	ScheduleBase.h
*
*	\brief	插件调度基类定义文件
*
*	\Author songxiaoyu
*
*	\date	2014-3-28 10:30
*
*	\version	1.0
*
*	\attention	Copyright (c) 2014, MACS-SCADA系统开发组,HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#ifndef SCHEDULEBASE_H
#define SCHEDULEBASE_H

#include "RTDBCommon/OS_Ext.h"

#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

namespace MACS_SCADA_SUD
{
	/*!
	*	\class	CScheduleBase
	*
	*	\brief	插件调度基类
	*
	*/
	class DBBASECLASSES_API CScheduleBase
	{
	public:
		CScheduleBase(void);
		virtual ~CScheduleBase(void){};
	protected:

		/*!
		*	\brief	初始化 虚函数
		*
		*	\param	无
		*
		*	\retval	int	   0：正常
		*/
		virtual int Init(void) = 0;

		/*!
		*	\brief	开始工作 虚函数
		*
		*	\param	无
		*
		*	\retval	int	   0：正常
		*/
		virtual int StartWork(void) = 0;

		/*!
		*	\brief	停止工作 虚函数
		*
		*	\param	无
		*
		*	\retval	int	   0：正常
		*/
		virtual int StopWork(void) = 0;

		/*!
		*	\brief	卸载
		*
		*	\param	无
		*
		*	\retval	int	   0：正常
		*/
		virtual int UnInit(void) = 0;       //!卸载
	};
}
#endif //!SCHEDULEBASE_H

