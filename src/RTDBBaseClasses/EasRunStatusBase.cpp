/*!
 *	\file	EasRunStatusBase.cpp
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

#include "RTDBBaseClasses/EasRunStatusBase.h"

namespace MACS_SCADA_SUD
{
	EAS_CALL_BACK_API CEasRunStatusBase* g_pEasRunStatus = NULL;

	CEasRunStatusBase::CEasRunStatusBase()
	{

	}

	CEasRunStatusBase::~CEasRunStatusBase()
	{

	}
}
