/*!
 *	\file	IODrvStatusHandle.cpp
 *
 *	\brief	����״̬��Ϣ��ʱ���ص�������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��3��	11:45
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
		//����IODriverBase��Ľӿ�
		if (!m_pDriverBase)
		{
			return -1;
		}

		//! ����˫��,��˫������������ʱ�Ž������
		// if ( g_pDiagServ && g_pDiagServ->isConnected())
		// {
		// 	m_pDriverBase->StatusUnit();
		// }
		return 0;
	}

}