/*!
 *	\file	NodeManagerComposite.cpp
 *
 *	\brief	���ڵ�������ӿ���ʵ��Դ�ļ�
 *
 *	$Author: Wangyuanxing $
 *	$Date: 14-04-01 9:09 $
 *	$Revision: 1.0 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBBaseClasses/NodeManagerComposite.h"

namespace MACS_SCADA_SUD
{
	DBBASECLASSES_API CNodeManagerComposite* g_pNodeManagerCompsite = NULL;

	CNodeManagerComposite::CNodeManagerComposite()
	{
	}

	CNodeManagerComposite::~CNodeManagerComposite()
	{
	}
}
