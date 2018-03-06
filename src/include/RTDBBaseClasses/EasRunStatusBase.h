/*!
 *	\file	EasRunStatusBase.h
 *
 *	\brief	报警运行状态变化接口类
 *
 *	\author	wangzan
 *
 *	\date	2014-07-24
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014-2024, SCADA V4平台开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _EAS_RUNSTATUS_BASE_H_
#define _EAS_RUNSTATUS_BASE_H_

#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/KiwiVariant.h"

#ifdef DBBASECLASSES_EXPORTS
#define EAS_CALL_BACK_API MACS_DLL_EXPORT
#else
#define EAS_CALL_BACK_API MACS_DLL_IMPORT
#endif

namespace MACS_SCADA_SUD
{
	class CEasCommand;

	/*!
	 *	\class	CEasRunStatusBase
	 *
	 *	\brief  报警组件用于响应数据库单元状态变化、处理角色登陆及获取报警类别
	 *
	 *  \des    报警运行状态变化接口类
	 *
	 *  \comment  
	 */
	class EAS_CALL_BACK_API CEasRunStatusBase
	{

	public:
		/*!
		*	\brief	构造/析构函数
		*
		*	\param	无
		*
		*	\retval	无
		*
		*	\note	
		*/
		CEasRunStatusBase();
		virtual ~CEasRunStatusBase();

	public:

		/*!
		*	\brief	数据库单元状态变化通知接口
		*
		*	\param	namespaceIndex : 数据库单元号
		*	\param  isMaster : 当前的主从状态
		*
		*	\retval	0成功，-1失败
		*
		*	\note	
		*/
		virtual int OnDBUnitStatusChange(unsigned short namespaceIndex, bool isMaster) = 0;

		/*!
		*	\brief	角色登陆接口，用于响应有新角色登陆到系统后的处理
		*
		*	\param	roleName : 角色名
		*
		*	\retval	0成功，-1失败
		*
		*	\note	
		*/
		virtual int OnRoleLogin(std::string roleName) = 0;

		/*!
		*	\brief	角色退出或被删除接口
		*
		*	\param	roleName : 角色名
		*
		*	\retval	0成功，-1失败
		*
		*	\note	
		*/
		virtual int OnRoleLogout(std::string roleName) = 0;

		/*!
		*	\brief	获取报警的类别信息
		*
		*	\param	eventId : 事件id
		*
		*	\retval	报警类别码
		*
		*	\note	
		*/
		virtual unsigned int GetCategory(CKiwiVariant eventId) = 0;

		/*!
		*	\brief	报警命令转发失败处理
		*
		*	\param	userName(in) : 用户名
		*	\param	roleName(in) : 角色名
		*	\param	request(in)  : 命令参数
		*	\param	siteName(in) : 站点名
		*	\param	cmdType(in)	 : 命令类型,取值来源于EasCommand.h文件中的EasCommandType枚举
		*
		*	\retval	0成功，其他失败
		*
		*	\note	
		*/
		// virtual int BackupEasCmd(std::string userName, std::string roleName, 
		// 	const void& request, std::string siteName, int cmdType) = 0;

		/*!
		*	\brief	当产生新报警/执行重复的命令时，需要将之前过时的命令删除
		*
		*	\param	conditionNode(in)	: 报警点nodeid
		*	\param	cmdType(in)			: 命令类型
		*
		*	\retval	0成功，其他失败
		*
		*/
		virtual int RemoveEasCmd(CKiwiVariant conditionNode, int cmdType) = 0;

		/*!
		*	\brief	正本源网络异常处理
		*			针对每个实时库单元, 部署为副本时, 运行过程中如果跟直接正本源发生网络中断故障, 
		*			在恢复连接时需要对该实时库单元的报警做一次对其操作
		*
		*	\param	无
		*
		*	\retval	0成功，其他失败
		*
		*	\note	
		*/
		virtual int OnSourceSiteConnectException() = 0;
	};

	extern  EAS_CALL_BACK_API CEasRunStatusBase* g_pEasRunStatus;     //!报警状态变化响应接口指针
}

#endif
