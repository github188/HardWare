/*!
 *	\file	IODrvStatusHandle.cpp
 *
 *	\brief	驱动状态信息定时监测回调处理器实现
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月3日	11:45
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IODrvStatusHandle.h"
#include "RTDBBaseClasses/IODrvScheduleMgrBase.h"
#include "RTDBDriverFrame/IODriverBase.h"
// #include "DiagSwitch/diagserv.h"

namespace MACS_SCADA_SUD
{


	CIODrvStatusHandle::CIODrvStatusHandle( CIODriverBase* pDriverBase, CIODrvScheduleMgrBase* pImpl )
	{
		m_pDriverBase = pDriverBase;
		m_pImpl = pImpl;
	}

	CIODrvStatusHandle::~CIODrvStatusHandle(void)
	{
		m_pDriverBase = NULL;
		m_pImpl = NULL;
	}

	int CIODrvStatusHandle::handle_timeout(const ACE_Time_Value &tv, const void *arg)
	{
		//调用IODriverBase里的接口
		if (!m_pDriverBase)
		{
			return -1;
		}

		//! 仅当双机,且双机都连接正常时才进行诊断
		// if ( g_pDiagServ && g_pDiagServ->isConnected())
		// {
		// 	m_pDriverBase->StatusUnit();
		// }
		return 0;
	}

}