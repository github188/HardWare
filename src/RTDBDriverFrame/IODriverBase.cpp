/*!
 *	\file	IODriverBase.cpp
 *
 *	\brief	��������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��8��	9:43
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBDriverFrame/IODriverBase.h"

namespace MACS_SCADA_SUD
{
	//! ���������๤��
	IODRIVERFRAME_VAR GenericFactory<CIODriverBase> g_IODriverFactory;

	std::string CIODriverBase::s_ClassName("CIODriverBase");

	//!���췽��
	CIODriverBase::CIODriverBase()
	{

	}

	//!��������
	CIODriverBase::~CIODriverBase()
	{

	}

	//!��ȡ��������
	std::string CIODriverBase::getClassName()
	{
		return s_ClassName;
	}


	void CIODriverBase::ReadOneTimeData()
	{

	}
	int CIODriverBase::ProcessHotSim()
	{
		return 0;
	}

//	short CIODriverBase::OpcUaDateTimeCompare(const OpcUa_DateTime& A, const OpcUa_DateTime& B)
//	{
//		if ( A.dwHighDateTime > B.dwHighDateTime)
//		{
//			return 1;
//		}
//		else if(A.dwHighDateTime == B.dwHighDateTime)
//		{
//			if(A.dwLowDateTime > B.dwLowDateTime)
//				return 1;
//			else if(A.dwLowDateTime == B.dwLowDateTime)
//				return 0;
//			else
//				return -1;
//		}
//		else
//			return -1;
//	}
}
