/*!
 *	\file	IOPrtclParserBase.cpp
 *
 *	\brief	协议类基类实现
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月8日	14:54
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBBaseClasses/IOPrtclParserBase.h"

namespace MACS_SCADA_SUD
{
	//!定义全局变量
	DBBASECLASSES_API GenericFactory<CIOPrtclParserBase> g_DrvPrtclFactory;

	//!协议类名
	std::string CIOPrtclParserBase::s_ClassName("CIOPrtclParserBase");

	//!构造函数
	CIOPrtclParserBase::CIOPrtclParserBase():m_pParam(NULL),m_pHandle(NULL)
	{

	}

	//!析构函数
	CIOPrtclParserBase::~CIOPrtclParserBase()
	{

	}

	//!得到类名
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

	//!组请求帧
	int CIOPrtclParserBase::BuildRequest(Byte* pbyData, int& nDataLen)
	{
		return 0;
	}

	//!组请求帧
	int CIOPrtclParserBase::BuildRequest(Byte* pbyData, int&nDataLen, bool& bTrigTimer, int& nTimeOut)
	{
		return 0;
	}
	
	//!处理接收数据
	int CIOPrtclParserBase::RxDataProc(Byte* pDataBuf, int& nLen, bool& bStopTimer, bool& bSendData)
	{
		return 0;
	}

	//!处理接收数据
	int CIOPrtclParserBase::RxDataProc(Byte* pDataBuf, int& nLen)
	{
		return 0;
	}
	
	//!超时处理
	int CIOPrtclParserBase::TimeOut(bool& bSendData, bool& bTrigTimer, int& nTimeOut)
	{
		return 0;
	}
	
	//!接收到通道诊断包
	int CIOPrtclParserBase::RcvPcketCmd(tIOCtrlPacketCmd* ptPacket)
	{
		return 0;
	}

	//!连接成功
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
