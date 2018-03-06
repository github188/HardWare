/*!
 *	\file	EventProducerBase.h
 *
 *	\brief	事件产生基类
 *
 *
 *	\author wangzan
 *
 *	\date	2014.04.01
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _EVENT_PRODUCER_BASE_H_
#define _EVENT_PRODUCER_BASE_H_

#include "RTDBCommon/KiwiVariant.h"

#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif


namespace MACS_SCADA_SUD
{
	typedef struct EventParam
	{
		void*			pNode;							//! 事件源节点
		void* pConditionNode;	//! 报警子对象节点,普通事件置NULL
		unsigned int	uiType;							//! 事件类别
		unsigned int	uiSeverity;						//! 事件等级
		long	    dtTime;							//! 时间发生时间
		std::string strMessage;						//! 事件描述
		unsigned int	uiChgCode;						//! 报警状态码
		std::string		opsName;						//! 操作员名
		std::string		opsComment;						//! 操作员描述
		std::string		opsCmdPosition;					//! 工位

		//! 以下字段在事件源节点不存在的情况下使用
		std::string	 strPointName;	//! 点名
		std::string	 strDevName;	//! 设备名
		std::string	 strArea1;		//! 区域1
		std::string	 strArea2;		//! 区域2
		std::string	 strArea3;		//! 区域3
		std::string     strDomain;     //! 域名称


		EventParam()
		{
			uiType = 0;
			uiSeverity = 0;
			pNode = NULL;
			pConditionNode = NULL;
			uiChgCode = 0;
		}
	} ;

	/*!
	 *	\class	CEventProducerBase
	 *
	 *	\brief	事件产生类
	 *
	 *	由实时库调用
	 */
	class DBBASECLASSES_API CEventProducerBase /* parasoft-suppress  CODSTA-CPP-19 "纯虚接口，不需要构造函数" */ /* parasoft-suppress  OOP-23 "纯虚接口定义，没有任何成员，因此不需要构造/析构函数" */
	{
	public:
		/*!
		 *	\brief	事件产生接口,实时库组件调用
		 *
		 *	\param	[in]pEventParam：事件参数
		 *
		 *	\retval	0:成功;非0:失败
		 *
		 *	\note	注意事项（可选）
		 */
		virtual unsigned int FireEvent(EventParam* pEventParam) = 0;
	};

	extern DBBASECLASSES_API CEventProducerBase* g_pEventProducer;
}

#endif
