/*!
 *	\file	IODrvScheduleMgrBase.cpp
 *
 *	\brief	驱动调度管理基类实现
 *
 *	详细的文件说明（可选）
 *
 *	\author miaoweijie
 *
 *	\date	2014年4月8日	14:54
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBBaseClasses/IODrvScheduleMgrBase.h"

namespace MACS_SCADA_SUD
{
	DBBASECLASSES_API CIODrvScheduleMgrBase* g_pIOScheduleManager = NULL;

	/*!
		*	\brief	缺省构造函数
		*/

	CIODrvScheduleMgrBase::CIODrvScheduleMgrBase()
	{

	}
	/*!
		*	\brief	缺省析构函数
		*/
	CIODrvScheduleMgrBase::~CIODrvScheduleMgrBase()
	{

	}

	/*!
	*	\brief	驱动调度初始化
	*
	*	详细的函数说明（可选）
	*
	*	\param	无
	*
	*	\retval	int		0为成功,其他失败
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::Init()
	{
		return 0;
	}

	/*!
	*	\brief	驱动调度开始工作
	*
	*	详细的函数说明（可选）
	*
	*	\param	无
	*
	*	\retval	int		0为成功,其他失败
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::StartWork()
	{
		return 0;
	}

	/*!
	*	\brief	驱动调度停止工作
	*
	*	详细的函数说明（可选）
	*
	*	\param	无
	*
	*	\retval	int		0为成功,其他失败
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::StopWork()
	{
		return 0;
	}

	/*!
	*	\brief	驱动调度卸载工作
	*
	*	详细的函数说明（可选）
	*
	*	\retval	int		0为成功,其他失败
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::UnInit()
	{
		return 0;
	}

	/*!
	*	\brief	删除数据变量
	*
	*	如果不存在则直接添加。只要NodeId相同则认为是一个数据变量
	*
	*	\param	pchDriverName	驱动名
	*	\param	pchUnitName		设备名
	*	\param	nodeId			dataVariable的NodeId
	*
	*	\retval	int		0为成功,其他失败
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::DeleteDataVariable(
		const char* pchDriverName, const char* pchUnitName, long nodeId)
	{
		return 0;
	}

	/*!
	*	\brief	创建驱动
	*
	*	详细的函数说明（可选）
	*
	*	\param	pchDriverName	驱动名称
	*
	*	\retval	CIODriverMgr*	驱动管理对象指针
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::AddDriver(CIOPort* pIOPort)
	{
		return -1;
	}

	/*!
	*	\brief	启动驱动开始工作
	*
	*	详细的函数说明（可选）
	*
	*	\param	pIOPort			IOPort对象指针
	*
	*	\retval	int		0为成功,其他失败
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::StartDriver(CIOPort* pIOPort)
	{
		return 0;
	}

	/*!
	*	\brief	配置更新:驱动停止工作
	*
	*	详细的函数说明（可选）
	*
	*	\param	pIOPort			IOPort对象指针
	*
	*	\retval	int		0为成功,其他失败
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::StopDriver(CIOPort* pIOPort)
	{
		return 0;
	}

	/*!
	*	\brief	配置更新:删除驱动
	*
	*	详细的函数说明（可选）
	*
	*	\param	pchDriverName	驱动名称
	*
	*	\retval	int		0为成功,其他失败
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::DeleteDriver(CIOPort* pIOPort)
	{
		return 0;
	}

	/*!
	*	\brief	更新驱动配置信息
	*
	*	详细的函数说明（可选）
	*
	*	\param	pchDriverName	驱动名称
	*
	*	\retval	int		0为成功,其他失败
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::UpdateDrvCfg(CIOPort* pIOPort)
	{
		return 0;
	}

	CIODriverBase* CIODrvScheduleMgrBase::GetDriverBaseByNodeId( long nodeId )
	{
		return NULL;
	}
	/*!
	*	\brief	下发事件信息
	*
	*	将系统内部产生的事件下发给某个外部设备
	*
	*	\param	pchDriverName	驱动名称
	*	\param	pchUnitName		设备名称
	*
	*	\retval	int		0为成功,其他失败
	*
	*	\note		注意事项（可选）
	*/
	int CIODrvScheduleMgrBase::SendEvent(const char* pchDriverName, const char* pchUnitName)
	{
		return 0;
	}
}