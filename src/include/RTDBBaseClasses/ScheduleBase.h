/*!
*	\file	ScheduleBase.h
*
*	\brief	������Ȼ��ඨ���ļ�
*
*	\Author songxiaoyu
*
*	\date	2014-3-28 10:30
*
*	\version	1.0
*
*	\attention	Copyright (c) 2014, MACS-SCADAϵͳ������,HollySys Co.,Ltd
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
	*	\brief	������Ȼ���
	*
	*/
	class DBBASECLASSES_API CScheduleBase
	{
	public:
		CScheduleBase(void);
		virtual ~CScheduleBase(void){};
	protected:

		/*!
		*	\brief	��ʼ�� �麯��
		*
		*	\param	��
		*
		*	\retval	int	   0������
		*/
		virtual int Init(void) = 0;

		/*!
		*	\brief	��ʼ���� �麯��
		*
		*	\param	��
		*
		*	\retval	int	   0������
		*/
		virtual int StartWork(void) = 0;

		/*!
		*	\brief	ֹͣ���� �麯��
		*
		*	\param	��
		*
		*	\retval	int	   0������
		*/
		virtual int StopWork(void) = 0;

		/*!
		*	\brief	ж��
		*
		*	\param	��
		*
		*	\retval	int	   0������
		*/
		virtual int UnInit(void) = 0;       //!ж��
	};
}
#endif //!SCHEDULEBASE_H

