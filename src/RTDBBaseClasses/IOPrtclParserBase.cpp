/*!
 *	\file	IOPrtclParserBase.cpp
 *
 *	\brief	Э�������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��8��	14:54
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBBaseClasses/IOPrtclParserBase.h"

namespace MACS_SCADA_SUD
{
	//!����ȫ�ֱ���
	DBBASECLASSES_API GenericFactory<CIOPrtclParserBase> g_DrvPrtclFactory;

	//!Э������
	std::string CIOPrtclParserBase::s_ClassName("CIOPrtclParserBase");

	//!���캯��
	CIOPrtclParserBase::CIOPrtclParserBase():m_pParam(NULL),m_pHandle(NULL)
	{

	}

	//!��������
	CIOPrtclParserBase::~CIOPrtclParserBase()
	{

	}

	//!�õ�����
	std::string CIOPrtclParserBase::getClassName()
	{
		return s_ClassName;
	}

	void CIOPrtclParserBase::SetHandle(void* pHandle)
	{
		m_pHandle = pHandle;
	}

	void* CIOPrtclParserBase::GetHandle()
	{
		return m_pHandle;
	}

	//!������֡
	int CIOPrtclParserBase::BuildRequest(Byte* pbyData, int& nDataLen)
	{
		return 0;
	}

	//!������֡
	int CIOPrtclParserBase::BuildRequest(Byte* pbyData, int&nDataLen, bool& bTrigTimer, int& nTimeOut)
	{
		return 0;
	}
	
	//!�����������
	int CIOPrtclParserBase::RxDataProc(Byte* pDataBuf, int& nLen, bool& bStopTimer, bool& bSendData)
	{
		return 0;
	}

	//!�����������
	int CIOPrtclParserBase::RxDataProc(Byte* pDataBuf, int& nLen)
	{
		return 0;
	}
	
	//!��ʱ����
	int CIOPrtclParserBase::TimeOut(bool& bSendData, bool& bTrigTimer, int& nTimeOut)
	{
		return 0;
	}
	
	//!���յ�ͨ����ϰ�
	int CIOPrtclParserBase::RcvPcketCmd(tIOCtrlPacketCmd* ptPacket)
	{
		return 0;
	}

	//!���ӳɹ�
	int CIOPrtclParserBase::OnConnSuccess(std::string pPeerIP)
	{
		return 0;
	}

	void CIOPrtclParserBase::SetParam( void* pParam )
	{
		m_pParam = pParam;
	}

	void* CIOPrtclParserBase::GetParam()
	{
		return m_pParam;
	}

	int CIOPrtclParserBase::StopTimer()
	{
		return 0;
	}
}
