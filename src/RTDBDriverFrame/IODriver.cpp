/*!
 *	\file	IODriver.cpp
 *
 *	\brief	������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��22��	19:54
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "RTDBDriverFrame/IODriver.h"
#include "RTDBCommon/Debug_Msg.h"
#include "RTDBCommon/RuntimeCatlog.h"
// #include "Common/IOLog/IOLog.h"
//#include "Common/OS_Ext/ShowDbgInfo.h"
#include "RTDBPlatformClass/IOPort.h"
#include "RTDBPlatformClass/IOUnit.h"
#include "RTDBPlatformClass/FieldPointType.h"
// #include "RTDB/Server/IODrvBoard/IODrvBoard.h"
// #include "RTDBFrameAccess/DBFrameAccess.h"
// #include "HlyEventBase/EventCategoryConstant.h"
// #include "IOCfgDriverDbfParser.h"
#include "RTDBDriverFrame/IODeviceBase.h"
#include "RTDBDriverFrame/IOPrtclParser.h"
#include "IODeviceSimBase.h"
#include "RTDBBaseClasses/IOPrtclParserBase.h"
#include "RTDBBaseClasses/IOCmdManagerBase.h"
#include "../IOHelpCfgManager/IOHelpCfgManager.h"

namespace MACS_SCADA_SUD
{
	//!����
	std::string CIODriver::s_ClassName("CIODriver");

	//!���캯��
	CIODriver::CIODriver()
		:  m_pIOPrtclParser(NULL),m_pIOPort(NULL),m_pIODrvBoard(NULL), m_pLog(NULL), 
		m_pChannelState(NULL),  m_pCommLog(NULL), m_nCommDataFormat(16), m_nSineY(30),
		m_bIsBlocked(false), m_nTimeOut(1000), m_nWatchTime(30000), m_nPollTime(1000),
		m_iCurDevIndex(0),m_nRemoteQueueLimit(64),m_nLogFileSize(2),m_nLogFileStoreDays(0),m_QueueSize(6),m_pDbfParser(NULL)
	{
		m_strFilePath = SingletonRuntimeCatlog::instance()->GetDebugLog() + "FEP/";
		m_mapDevices.clear();
	}

	//!��������
	CIODriver::~CIODriver()
	{
		T_MapDevices::iterator itDev = m_mapDevices.begin();
		for (; itDev != m_mapDevices.end(); ++itDev)
		{
			CIODeviceBase* pDevice = itDev->second;
			if (pDevice)
			{
				delete pDevice;
				pDevice = NULL;
			}
		}
		m_mapDevices.clear();
	}

	//!�õ�����������
	std::string CIODriver::GetClassName()
	{
		return s_ClassName;
	}

	//!��ʼ���豸
	int CIODriver::InitUnit(CIOUnit* pIOUnit)
	{
		int nResult = -1;
		 if (NULL == pIOUnit)
		 {
		 	return nResult;
		 }

		 //!����IODevice����
		 std::string strDevName = GetDeviceClassName();
		 if (!strDevName.empty())
		 {
		 	CIODeviceBase* pDevice = g_DrvDeviceFactory.Create(strDevName);
		 	if (pDevice != NULL)
		 	{
		 		//!�����豸����Driver��Prtcl������Ϣ
		 		pDevice->SetDriver(this);
		 		//pDevice->SetPrtcl(m_pIOPrtclParser);

		 		//!����IOUnit,IOPort�ڵ���Ϣ
		 		pDevice->SetIOUnitNode(pIOUnit);
		 		pDevice->SetIOCfgDriver(m_pIOPort);

		 		//!��ʼ�����ݱ�����Ϣ
		 		nResult = pDevice->InitVariableInfo(pIOUnit);
		 		if (nResult != 0)
		 		{
		 			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriver::InitUnit(): InitVariableInfo failed! Device:%s"), pDevice->GetName().c_str()));
		 		}
		 		//!�����豸ģ�����
		 		std::string strDevSimName = GetDeviceSimClassName();
		 		CIODeviceSimBase* pDevSimBase = g_DrvSimFactory.Create(strDevSimName);
		 		if (pDevSimBase != NULL)
		 		{
		 			pDevSimBase->SetParent(pDevice);
		 			pDevice->SetIODeviceSim(pDevSimBase);
						
		 			pDevice->Init();

		 			//!�����豸����
		 			m_mapDevices.insert(T_MapDevices::value_type(pIOUnit->GetUnitName(), pDevice));

		 			//!ִ�гɹ�
		 			nResult = 0;
		 		}
		 		else
		 		{
		 			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriver::InitUnit(): g_DrvSimFactory.Create(%s) failed!"), strDevSimName.c_str()));
		 		}
		 	} 
		 	else
		 	{
		 		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriver::InitUnit(): g_DrvDeviceFactory.Create(%s) failed!"), strDevName.c_str()));
		 	}
		 }
		return nResult;
	}

	//!��ͨ��
	int CIODriver::OpenChannel()
	{
		int nResult = 0;
		 //!��ȡ�˿�������Ϣ
		 m_pIOPort->getIOPortCfg(m_pPortCfg);

		 //!����Э����������
		 std::string strPrtclCName = GetPrtclClassName();
		 if (!strPrtclCName.empty())
		 {
			 m_pIOPrtclParser = g_DrvPrtclFactory.Create(strPrtclCName);
			 if (m_pIOPrtclParser != NULL)
			 {
				 m_pIOPrtclParser->SetParam(this);
			 }
			 else
		 	{
		 		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriver::OpenChannel(): m_pIOPrtclParser is NULL!")));
		 		nResult = -1;
		 	}
		 }

		 //!��ʼ������������Ϣ
		 InitDriverCfg();

		 //create store log folder
		 CreateDriverLogFolder();

		 //!����ң�ض��еĴ�С
		 SetCtrlQueueLimit(m_nRemoteQueueLimit);

		 //!������־����
		 char pchBuf[512];
		 std::string strFileName;
		 std::string strLogPath;
		 strLogPath = m_strFilePath;
		 strFileName = GetDriverName();
		 sprintf(pchBuf, "%s%s", strLogPath.c_str(), strFileName.c_str());
		 m_pLog = new CIOLog(m_strFilePath, pchBuf);
		 if (NULL == m_pLog)
		 {
		 	nResult = -1;
		 }
		 else
		 {
		 	m_pLog->SetMaxLen(m_nLogFileSize);
		 	m_pLog->SetMaxLogDays(m_nLogFileStoreDays);
		 }

		 //!����ͨ�ż��Ӷ���
		 sprintf(pchBuf, "%s%s_COMM", strLogPath.c_str(), strFileName.c_str());
		 m_pCommLog = new CIOLog(m_strFilePath, pchBuf);
		 if (NULL == m_pCommLog)
		 {
		 	nResult = -1;
		 }
		 else
		 {
		 	m_pCommLog->SetMaxLen(m_nLogFileSize);
		 	m_pCommLog->SetMaxLogDays(m_nLogFileStoreDays);
		 }

		 //!�õ��Ƿ���˫������
		 m_bDoubleComputer = m_pIOPort->IsDouble();

		return nResult;
	}

	//!�ر�ͨ��
	int CIODriver::CloseChannel()
	{
		if ( m_pCommLog )
		{
			delete m_pCommLog;
			m_pCommLog = NULL;
		}


		if ( m_pLog )
		{
			delete m_pLog;
			m_pLog = NULL;
		}

		if (m_pIODrvBoard)
		{
			delete m_pIODrvBoard;
			m_pIODrvBoard = NULL;
		}

		if (m_pIOPrtclParser)
		{
			delete m_pIOPrtclParser;
			m_pIOPrtclParser = NULL;
		}

		return 0;
	}

	//!��ң��ָ�����ң�ض���
	int CIODriver::WriteDCB(tIORemoteCtrl& tRemote)
	{
		int nResult = 0;  //BUG 62790 ��Ԫ���� DBDriverFrameģ���е�IODriver����WriteDCB��ִ�гɹ�ʱ������Ϊִ��ʧ��
		 FieldPointType* pFieldPoint = dynamic_cast<FieldPointType*>(tRemote.pNode);
		 if (pFieldPoint && !m_bIsBlocked)
		 {
		 	CIOUnit* pIOUnit = pFieldPoint->getIOUnit();
		 	if (pIOUnit)
		 	{
		 		std::string strdevName = pIOUnit->GetUnitName();
		 		CIODeviceBase* pDevice = GetDeviceByName(strdevName);
		 		if (pDevice != NULL)
		 		{
		 			CIOCfgUnit* pCfgUnit = pDevice->GetIOCfgUnit();
		 			std::string sAddr = pFieldPoint->getOffset();
		 			if (!sAddr.empty() && (tRemote.bysValue.length() > 0))
		 			{
		 				std::string strAddr = sAddr;
		 				if (pCfgUnit && !strAddr.empty())
		 				{
		 					CIOCfgTag* pCfgTag = pCfgUnit->GetIOCfgTagByAddr(strAddr);

		 					//!����ǩ��ַ�Ϸ���
		 					if (pCfgTag && (pDevice->CheckUnitTypeAndTagAddr(pCfgTag->GetUnitType(), pCfgTag->GetFinalAddr())))
		 					{
		 						tIOCtrlRemoteCmd tRCtrlCmd;
		 						ACE_OS::memset(&tRCtrlCmd, 0, sizeof(tIOCtrlRemoteCmd));
		 						tRCtrlCmd.byUnitType = pCfgTag->GetUnitType();
		 						tRCtrlCmd.lTagAddress = pCfgTag->GetFinalAddr();
		 						int nCmdDataLen = tRemote.bysValue.length();
		 						if ( nCmdDataLen <= MAX_REMOTECMD_LEN)
		 						{
		 							tRCtrlCmd.nDataLen = nCmdDataLen;
		 						}
		 						else
		 						{
		 							tRCtrlCmd.nDataLen = MAX_REMOTECMD_LEN;
		 							char strDebug[256] = {0};
		 							//snprintf(strDebug, 256, "CIODriver::WriteDCB failed!The RemoteCmd Len:%d > MAX_REMOTECMD_LEN:%d, Channel:%s", nCmdDataLen, MAX_REMOTECMD_LEN, pFieldPoint->getOffset().toUtf8());
		 							WriteLogAnyWay(strDebug);
		 							nResult = -1;
		 							return nResult;
		 						}
		 						ACE_OS::strcpy(tRCtrlCmd.pchUnitName, strdevName.c_str());
		 						ACE_OS::memcpy(tRCtrlCmd.byData, tRemote.bysValue.data(), tRCtrlCmd.nDataLen);

		 						//!ͨ����ֹ���豸��ֹ��ִ��д��ǩָ��
		 						if (!m_pIOPort->IsDisable() && !pDevice->IsDisable())
		 						{
		 							//!���������ת����ת��Ϊͨ��ֵ
		 							pFieldPoint->RangeData(tRCtrlCmd.byData, tRCtrlCmd.nDataLen, false);

		 							//!����豸û��ģ����������ң���������ң�ض���
		 							if (!pDevice->IsSimulate() && pDevice->IsOnLine())
		 							{
		 								nResult = m_IORemoteCtrl.Enqueue((char*)(&tRCtrlCmd), sizeof(tIOCtrlRemoteCmd));
		 							}
		 							else if (pDevice->IsOnLine() && pDevice->IsHot())
		 							{
		 								//!֧���豸���߸�ֵ
		 								pDevice->SetVal(tRCtrlCmd.byUnitType, tRCtrlCmd.lTagAddress, tRCtrlCmd.byData, tRCtrlCmd.nDataLen, 0, true);
		 							}
		 						} 
		 						else
		 						{
		 							char strDebug[256] = {0};
		 							//snprintf(strDebug, 256, "CIODriver::WriteDCB failed!Port or Device is Disable!Port:%s, Device:%s", this->GetDriverName().c_str(), strdevName.c_str());
		 							WriteLogAnyWay(strDebug);
		 							nResult = -1; //BUG 62790  
		 						}
		 					}
		 					else
		 					{
		 						char strDebug[256] = {0};
		 						//snprintf(strDebug, 256, "CIODriver::WriteDCB failed!Device's Tag Address is illegal!Port:%s, Device:%s", this->GetDriverName().c_str(), strdevName.c_str());
		 						WriteLogAnyWay(strDebug);
		 						nResult = -1; //BUG 62790
		 					}
		 				}
		 			}	
		 		}
		 		else
		 		{
		 			char strDebug[256] = {0};
		 			//snprintf(strDebug, 256, "CIODriver::WriteDCB failed!The Device is NULL!Port:%s, Device:%s", this->GetDriverName().c_str(), strdevName.c_str());
		 			WriteLogAnyWay(strDebug);
		 			nResult = -1; //BUG 62790
		 		}
		 	}
		 }
		return nResult;
	}

	//!�����������ͨ�����豸����״̬
	int CIODriver::WriteMgr(tIORunMgrCmd& tRunMgr)
	{
		int nResult = -1;
		if (!m_bIsBlocked)
		{
			switch(tRunMgr.byObjectType)
			{
			case ObjectType_Device:
				nResult = WriteMgrForUnit(tRunMgr);
				break;
			case ObjectType_Channel:
				nResult = WriteMgrForPort(tRunMgr);
				break;
			case ObjectType_Tag:
				nResult = WriteMgrForTag(tRunMgr);
				break;
			default:
				break;
			}
		}
		return nResult;
	}

	//!����ͨ����ϰ�����
	int CIODriver::WritePacket(tIOCtrlPacketCmd& tPacket)
	{
		int nResult = -1;
		//!ֻ��˫���Ŵ�����ϰ�
		// if (!m_bDoubleComputer)
		// {
		// 	return nResult;
		// }

		 //!����������͹������ݰ�
		 if (m_pIOPrtclParser)
		 {
		 	nResult = m_pIOPrtclParser->RcvPcketCmd(&tPacket);
		 }

		return nResult;
	}

	//!�����豸��ͨ����ϱ�ʶ
	int CIODriver::StatusUnit(void)
	{
		int nResult = -1;
		//!�����·����ͻ��ֱ�ӷ���
		if (!IsPortAccessClash())
		{
			return nResult;
		}

		//!�����豸��ͨ�����״̬
		SetDevCHNCheck(true);

		return nResult;
	}

	//!�������������󡢳�ʼ������������
	int CIODriver::StartWork(void)
	{
		int nResult = -1;
		if (m_bIsBlocked)
		{
			m_bIsBlocked = false;
		}
		if (NULL == m_pIODrvBoard)
		{
			m_pIODrvBoard = new CIODrvBoard();
		}
		if (m_pIODrvBoard != NULL)
		{
			std::string strPortType = m_pPortCfg.strType;
			nResult = m_pIODrvBoard->Init(m_pIOPrtclParser, strPortType, m_pPortCfg, this->GetTimeOut());
			if (nResult == 0)
			{
				nResult = m_pIODrvBoard->open();
			}
			else
			{
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriver::StartWork: m_pIOBoard Init Faile! Driver:%s\n"), this->GetDriverName().c_str()));
			}
		}
		else
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriver::StartWork: new m_pIOBoard Faile! Driver:%s\n"), this->GetDriverName().c_str()));
		}

		return nResult;
	}

	//!�ر���������ɾ������������
	int CIODriver::StopWork(void)
	{
		int nRes = -1;
		//!ֹͣ��ʱ��
		 CIOPrtclParser* pIOPrtclParser = (CIOPrtclParser*)(GetIOPrtclParse());
		 if (pIOPrtclParser)
		 {
		 	nRes = pIOPrtclParser->StopTimer();
		 }
		 //!�ر�������
		 if (m_pIODrvBoard)
		 {
			 nRes = m_pIODrvBoard->close();
		 }
		 m_bIsBlocked = true;
		return nRes;
	}

	//!�õ�Э�����������
	std::string CIODriver::GetPrtclClassName()
	{
		return "CIOPrtclParser";
	}

	//!�õ��豸������
	std::string CIODriver::GetDeviceClassName()
	{
		return "CIODeviceBase";
	}

	//!�õ��豸ģ��������
	std::string CIODriver::GetDeviceSimClassName()
	{
		return "CIODeviceSimBase";
	}

	//!�õ���һ���豸����ָ��
	CIODeviceBase* CIODriver::GetNextDevice()
	{
		m_iCurDevIndex++;
		if (m_iCurDevIndex >= GetDeviceCount())
		{
			m_iCurDevIndex = 0;
		}
		return GetDeviceByIndex(m_iCurDevIndex);
	}

	//!�õ���ǰ��ѵ�����豸����ָ��
	CIODeviceBase* CIODriver::GetCurDevice()
	{
		if (m_iCurDevIndex >= GetDeviceCount())
		{
			m_iCurDevIndex = 0;
		}
		return GetDeviceByIndex(m_iCurDevIndex);
	}

	//!ͨ���豸���õ��豸����ָ��
	CIODeviceBase* CIODriver::GetDeviceByName(std::string szName)
	{
		std::string szUnitName;
		T_MapDevices::iterator itDev = m_mapDevices.begin();
		while(itDev != m_mapDevices.end())
		{
			szUnitName = itDev->first;
			if (ACE_OS::strcasecmp(szUnitName.c_str(), szName.c_str()) == 0)
			{
				return itDev->second;
			}
			itDev++;
		}
		return NULL;
	}

	//!ͨ���豸��ַ�õ��豸�����ָ��
	CIODeviceBase* CIODriver::GetDeviceByAddr(int nUnitAddr)
	{
		T_MapDevices::iterator itDev;
		for (itDev = m_mapDevices.begin(); itDev != m_mapDevices.end(); itDev++)
		{
			CIODeviceBase* pDevice = itDev->second;
			if (pDevice->GetAddr() == nUnitAddr)
			{
				return pDevice;
			}
		}
		return NULL;
	}

	//!ͨ���豸��ַ�õ��豸�����ָ��
	CIODeviceBase* CIODriver::GetDeviceByAddr(std::string sUnitAddr)
	{
		T_MapDevices::iterator itDev;
		for (itDev = m_mapDevices.begin(); itDev != m_mapDevices.end(); itDev++)
		{
			CIODeviceBase* pDevice = itDev->second;
			if (ACE_OS::strcasecmp(pDevice->GetStringAddr().c_str() , sUnitAddr.c_str()) == 0)
			{
				return pDevice;
			}
		}
		return NULL;
	}

	//!�õ������豸
	T_MapDevices& CIODriver::GetAllDevice()
	{
		return m_mapDevices;
	}

	//!�õ��豸����
	int CIODriver::GetDeviceCount()
	{
		return m_mapDevices.size();
	}

	//!����ŵõ��豸����ָ��
	CIODeviceBase* CIODriver::GetDeviceByIndex(int iIndex)
	{
		if (m_mapDevices.empty() )
		{
			return NULL;
		}
		//62996 FEP ��Ԫ����   DBDriverFrame ģ���е�IODriver.cpp �е�GetDeviceByIndex�ж������ŵĴ�������, �������ڸ��������������ĸ���ʱ���ؾ�����ȷ
		if (iIndex >= static_cast<int>(m_mapDevices.size())) 
		{
			return NULL;
		}

		T_MapDevices::iterator itDev;
		itDev = m_mapDevices.begin();
		for(int i = 0; i < iIndex; i++)
		{
			itDev++;
		}
		return itDev->second;
	}

	//!�õ���������ʵ�ʷ����û����õ�Port����
	std::string CIODriver::GetDriverName()
	{
		std::string strDrvName = "";
		strDrvName = m_pIOPort->getDrvName();
		return strDrvName;
	}

	//!�õ��������õ�Э������
	std::string CIODriver::GetCfgPrtclName()
	{
		std::string strPrtclName = "";
		strPrtclName = m_pIOPort->getDrvName();
		return strPrtclName;
	}

	//!����ң�ض��еĴ�С
	void CIODriver::SetCtrlQueueLimit(size_t nLimit)
	{
		 m_IORemoteCtrl.SetQueueLimit(nLimit);
	}

	//!д����������־
	int CIODriver::WriteLog(const char* pchLog, bool bTime/*=true */)
	{
		int nResult = -1;
		 if (this->IsLog())
		 {
			 if (m_pLog)
			 {
				 m_pLog->WriteLog(pchLog, bTime);
				 nResult = 0;
			 }
		 }
		return nResult;
	}

	//!������ͨ����Ϣд����Թ���ͬʱд����־
	int CIODriver::WriteCommData(Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd/*=0 */)
	{
		int nResult = -1;  
		/*
		if (this->IsMonitor())
		{
			//!��ͨ������д����Թ���
			tCommData commData;
			strcpy(commData.pchDriverName, GetDriverName().c_str());
			ACE_Time_Value val = ACE_OS::gettimeofday();
			long lCurTime = long(val.sec())*1000 + val.usec();
			commData.dataTime = lCurTime;
			commData.nDataLen = __min(COMMDATALEN, nDataLen);
			if (0 == byRcvOrSnd)
			{
				commData.byDataType = COMMDATA_RCV;
			} 
			else
			{
				commData.byDataType =  COMMDATA_SND;
			}
			ACE_OS::memcpy(commData.pbyData, pbyBuf, commData.nDataLen);
			
			
			nResult = 0;
		}*/

		//!��ͨ�ż�������д����־�ļ�
		 nResult = WriteCommDataToLog(pbyBuf, nDataLen, byRcvOrSnd);//MOD for BUG63093 WriteCommData����û����ȷ�ķ��� on 20160203
		return nResult;
	}

	//!д����ͨ����Ϣд����־��־
	int CIODriver::WriteCommDataToLog(Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd/*=0 */)
	{
		 if (this->IsMonitor())
		 {
		 	std::string szContent;
		 	char pchContent[1024];
		 	char pchData[16];

		 	if (nDataLen > 1024)
		 	{
		 		return -1;
		 	}

		 	//!������ʾ��ʽ
		 	if (m_nCommDataFormat == 0)
		 	{
		 		for (int i = 0; i < nDataLen; i++)
		 		{
		 			sprintf(pchData, "%d ", pbyBuf[i]);
		 			szContent += pchData;
		 		}
		 	}
		 	else if (m_nCommDataFormat == 16)
		 	{
		 		for(int i = 0; i < nDataLen; i++)
		 		{
		 			sprintf(pchData, "%02X ", pbyBuf[i]);
		 			szContent += pchData;
		 		}
		 	}
		 	else
		 	{
		 		sprintf(pchContent, "%s ", pbyBuf);
		 		szContent = pchContent;
		 	}

		 	std::string szLogStr;
		 	if (byRcvOrSnd == 0)
		 	{
		 		szLogStr = "RCV " + szContent;
		 	} 
		 	else
		 	{
		 		szLogStr = "SND " + szContent;
		 	}

		 	//m_pCommLog->WriteLog(szLogStr.c_str());
		 }

		return 0;
	}

	//!��·�����Ƿ��ͻ
	bool CIODriver::IsPortAccessClash()
	{
		return m_pPortCfg.byPortAccessMode;
	}

	//!����ң��֮���SOE
	int CIODriver::PostYKSOE(std::string szUnitName, int nUnitType, int nTagAddr, int nVal, long nTime /*= 0 */)
	{
		int nRes = -1;
		 CIODeviceBase* pDevice = GetDeviceByName(szUnitName);
		 if (pDevice)
		 {
		 	CIOCfgTag* pIOTag = pDevice->GetTagByIndex(nUnitType, nTagAddr);
		 	FieldPointType* pFieldPoint = pIOTag->GetFirst();
		 	if (pFieldPoint)
		 	{
		 		//nRes = CDBFrameAccess::PostSOE(pFieldPoint, szUnitName, EXTERNAL_SOE, nVal, nTime);
		 	}
		 }
		return nRes;
	}

	//!��ȡ��ʱʱ��
	int CIODriver::GetTimeOut()
	{
		return m_nTimeOut;
	}

	//!��ȡЭ�鴦����ָ��
	CIOPrtclParserBase* CIODriver::GetIOPrtclParse()
	{
		return m_pIOPrtclParser;
	}

	//!ͨ���Ƿ񱻽�ֹ
	bool CIODriver::IsDisable()
	{
		 return m_pIOPort->IsDisable();
	}

	//!�Ƿ���ģ��״̬
	bool CIODriver::IsSimulate()
	{
		 return m_pIOPort->IsSimulate();
		return true;
	}

	//!����ͨ��״̬
	// void CIODriver::SetChannelState(tIOScheduleChannelState* pChannelState)
	// {
	// 	// m_pChannelState = pChannelState;
	// }

	//!����ͨ����ģ�ⷽʽ
	void CIODriver::SetSimulate(Byte bySimMode)
	{
		 if (m_pIOPort && !m_pIOPort->IsSimulate())
		 {
		 	m_pIOPort->SimulateChannel(bySimMode);
		 	//!��Driver�������豸����ģ��
		 	T_MapDevices::iterator it = m_mapDevices.begin();
		 	while(it != m_mapDevices.end())
		 	{
		 		it->second->StartSimulate(bySimMode);
		 		it++;
		 	}
		 }
	}

	//!ֹͣģ��
	void CIODriver::StopSimulate(void)
	{
		 if (m_pIOPort && m_pIOPort->IsSimulate())
		 {
		 	m_pIOPort->StopSimulate();
		 	//!��Driver�������豸�ر�ģ��
		 	T_MapDevices::iterator it = m_mapDevices.begin();
		 	while(it != m_mapDevices.end())
		 	{
		 		it->second->StopSimulate();
		 		it++;
		 	}
		 }
	}

	//!����ͨ����ֹ
	void CIODriver::SetDisable(Byte byDisable)
	{
		long nDateTime = time(NULL);
		if (m_pIOPort && !m_pIOPort->IsDisable() && byDisable)
		{
			m_pIOPort->DisableChannel();
			//if (GetHot())
			//{
			//	CDBFrameAccess::FireFEPEvent(m_pIOPort, FEP_DISABLE_NETWORK, (int)byDisable, nDateTime);
			//}
		}
		else if (m_pIOPort && m_pIOPort->IsDisable() && byDisable)
		{
			m_pIOPort->EnableChannel();
			//if (GetHot())
			//{
			//	CDBFrameAccess::FireFEPEvent(m_pIOPort, FEP_ENABLE_NETWOKR, (int)byDisable, nDateTime);
			//}
		}
	}

	//!�Ƿ��¼��־
	bool CIODriver::IsLog()
	{
		if (m_pIOPort)
		{
			return m_pIOPort->IsRecord();
		}
		return false;
	}

	//!�����Ƿ��¼��־��־
	void CIODriver::SetLog(bool byLog)
	{
		if (m_pIOPort)
		{
			m_pIOPort->SetRecord(byLog);
		}
	}

	//!�Ƿ���ͨ�ż���״̬
	bool CIODriver::IsMonitor()
	{
		if (m_pIOPort)
		{
			return m_pIOPort->IsMonitor();
		}
		return 0;
	}

	//!�����Ƿ����ͨ�ż���
	void CIODriver::SetMonitor(bool byMonitor)
	{
		if (m_pIOPort)
		{
			return m_pIOPort->SetMonitor(byMonitor);
		}
	}
	//!�õ�ͨ������״̬
	Byte CIODriver::GetHot()
	{
		if (m_pIOPort)
		{
			return (Byte)m_pIOPort->GetRealPrmScnd();
		}
		return 0;
	}

	//!����ͨ������״̬
	void CIODriver::SetHot(Byte byHot)
	{
		if (m_pIOPort)
		{
			m_pIOPort->SetRealPrmScnd(byHot);
		}
	}

	//!���豸�������й�������
	int CIODriver::WriteMgrForUnit(tIORunMgrCmd& tRunMgr)
	{
		int nResult = -1;
		 CIOUnit* pIOUnit = dynamic_cast<CIOUnit*>(tRunMgr.pNode);
		 if (pIOUnit)
		 {
		 	std::string strDevName = pIOUnit->GetUnitName();
		 	CIODeviceBase* pDevice = GetDeviceByName(strDevName);
		 	if (pDevice != NULL)
		 	{
		 		switch(tRunMgr.byCmdValue)
		 		{
		 		case CmdValue_Simulate:							//!��ʼģ��
		 			{
		 				if ((tRunMgr.byCmdParam.data()) != NULL)
		 				{
		 					pDevice->StartSimulate(*(tRunMgr.byCmdParam.data()));
		 				}
		 			}
		 			break;
		 		case CmdValue_StopSimulate:						//!ֹͣģ��
		 			pDevice->StopSimulate();
		 			break;
		 		case CmdValue_Disable:							//!�豸��ֹ
		 			pDevice->SetDisable(true);
		 			break;
		 		case CmdValue_Enable:							//!ȡ����ֹ
		 			pDevice->SetDisable(false);
		 			break;
		 		case CmdValue_ExtraRangeEvent_1:				//!�򿪳������¼�����
		 			pDevice->SetExtraRange(true);
		 			break;
		 		case CmdValue_ExtraRangeEvent_0:				//!�رճ������¼�����
		 			pDevice->SetExtraRange(false);
		 			break;
		 		default:
		 			break;
		 		}
		 		nResult = 0;
		 	}
		 	else
		 	{
		 		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriver::WriteMgrForUnit failed!GetDevice() is NULL!Device:%s"), strDevName.c_str()));
		 	}
		 }
		return nResult;
	}

	//!��ͨ���������й�������
	int CIODriver::WriteMgrForPort(tIORunMgrCmd& tRunMgr)
	{
		int nResult = -1;
		 if (m_pIOPort)
		 {
		 	switch(tRunMgr.byCmdValue)
		 	{
		 	case CmdValue_RedunForce:					//!ǿ������
		 		{
		 			if ((tRunMgr.byCmdParam.data()) != NULL)
		 			{
		 				if (Redun_Prim == *(tRunMgr.byCmdParam.data()))
		 				{
		 					//!ǿ����
		 					this->ForcePort(true);
		 				} 
		 				else if(Redun_Second == *(tRunMgr.byCmdParam.data()) )
		 				{
		 					//!ǿ�ƴ�
		 					this->ForcePort(false);
		 				}
		 			}
		 		}				
		 		break;
		 	case CmdValue_UnRedunForce:					//!ȡ��ǿ��
		 		this->DeforcePort();
		 		break;
		 	case CmdValue_Disable:						//!ͨ����ֹ
		 		this->SetDisable(true);
		 		break;
		 	case CmdValue_Enable:						//!ȡ����ֹ
		 		this->SetDisable(false);			
		 		break;
		 	case CmdValue_Simulate:						//!��ʼģ��
		 		{
		 			if ((tRunMgr.byCmdParam.data()) != NULL)
		 			{
		 				this->SetSimulate(*(tRunMgr.byCmdParam.data()));
		 			}
		 		}
		 		break;
		 	case CmdValue_StopSimulate:					//!ֹͣģ��
		 		this->StopSimulate();
		 		break;
		 	case CmdValue_Monitor:						//!��ʼͨ�ż���
		 		this->SetMonitor(true);
		 		break;
		 	case CmdValue_StopMonitor:					//!ֹͣͨ�ż���
		 		this->SetMonitor(false);			
		 		break;
		 	case CmdValue_StartLog:						//!��ʼ��¼��־
		 		this->SetLog(true);				
		 		break;
		 	case CmdValue_StopLog:						//!ֹͣ��¼��־
		 		this->SetLog(false);
		 		break;
		 	default:
		 		break;
		 	}
		 	nResult = 0;
		 }
		return nResult;
	}

	//!��ȡ������ǩ��������
	 CIOCfgDriverDbfParser* CIODriver::GetDbfParser()
	 {
	 	return m_pDbfParser;
	 }

	 //!����������ǩ��������
	 void CIODriver::SetDbfParser(CIOCfgDriverDbfParser* pDbfParser)
	 {
	 	if (pDbfParser)
	 	{
	 		m_pDbfParser = pDbfParser;
	 	}
	 }

	int CIODriver::StopUnit(std::string strUnitName)
	{
		T_MapDevices::iterator itDev = m_mapDevices.find(strUnitName);
		if (itDev != m_mapDevices.end())
		{
			CIODeviceBase* pDevice = itDev->second;
			m_mapDevices.erase(itDev);
			if (pDevice)
			{
				delete pDevice;
				pDevice = NULL;
			}
		}

		return 0;
	}

	//!��ȡIOPort����ָ��
	 CIOPort* CIODriver::GetIOPort()
	 {
	 	return m_pIOPort;
	 }

	 // !����IOPort����ָ��
	 void CIODriver::SetIOPort(CIOPort* pIOPort)
	 {
	 	if (pIOPort != NULL)
	 	{
	 		m_pIOPort = pIOPort;
	 	}
	 }

	//!��MacsDriver.ini�ļ��л�ȡʱ����Ϣ����
	void CIODriver::InitDriverCfg()
	{
		 InitDriverCommCfg();

		 InitOpcUaDriverCfg();

		 int status;
		 ACE_Configuration_Heap cf;
		 status = cf.open();
		 if (status != 0)
		 {
		 	return;
		 }
		 ACE_Ini_ImpExp import(cf);
		 std::string file_name = GetFEPRunDir() + MACS_DRIVER_FILENAME;
		 status = import.import_config(file_name.c_str());
		 if (status != 0)
		 {
		 	return;
		 }
		
		 const ACE_Configuration_Section_Key& root = cf.root_section();
		 ACE_Configuration_Section_Key DrvSection;
		 std::string strPort = GetDriverName();
		 status = cf.open_section(root, strPort.c_str(), 0, DrvSection);
		 if (status != 0)
		 {	
		 	return;
		 }

		 int nValue;
		 std::string strValue;
		 ACE_TString strV;
		 status = cf.get_string_value(DrvSection, ACE_TEXT("WatchTime"), strV);
		 sscanf(strV.c_str(), "%d", &nValue);
		 if (0 == status)
		 {
		 	m_nWatchTime = (int)nValue;
		 }

		 status = cf.get_string_value(DrvSection, ACE_TEXT("PollTime"), strV);
		 sscanf(strV.c_str(), "%d", &nValue);
		 if (0 == status)
		 {
		 	m_nPollTime = (int)nValue;
		 }

		 status = cf.get_string_value(DrvSection, ACE_TEXT("TimeOut"), strV);
		 sscanf(strV.c_str(), "%d", &nValue);
		 if (0 == status)
		 {
		 	m_nTimeOut = (int)nValue;
		 } 

		 status = cf.get_string_value(DrvSection, ACE_TEXT("CommDataFormat"), strV);
		 sscanf(strV.c_str(), "%d", &nValue);
		 if (0 == status)
		 {
		 	m_nCommDataFormat = (int)nValue;
		 }

		 status = cf.get_string_value(DrvSection, ACE_TEXT("CtrlQueueLimit"), strV);
		 sscanf(strV.c_str(), "%d", &nValue);
		 if (0 == status)
		 {
		 	m_nRemoteQueueLimit = (size_t)nValue;
		 }

		 //!��ѡģ��Yֵ��С
		 status = cf.get_string_value(DrvSection, ACE_TEXT("SineY"), strV);
		 sscanf(strV.c_str(), "%d", &nValue);
		 if (0 == status)
		 {
		 	m_nSineY = (int)nValue;
		 }

		return;
	}

	//!��ȡStatus���ʱ��
	int CIODriver::GetStatusWatchTime()
	{
		return m_nWatchTime;
	}

	//!���������Ϣ
	void CIODriver::OutputDebug( const char* pchInfo )
	{
		OutputDebugString(pchInfo);
	}

	//!����Ա�ǩ��ǩ���й�������
	int CIODriver::WriteMgrForTag( tIORunMgrCmd& tRunMgr )
	{
		int nResult = -1;
		 FieldPointType* pFieldPoint = dynamic_cast<FieldPointType*>(tRunMgr.pNode);
		 int nLength = tRunMgr.byCmdParam.length();
		 Byte status;
		 if (pFieldPoint)
		 {
		 	switch(tRunMgr.byCmdValue)
		 	{
		 	case CmdValue_Force:
		 		{
		 			if (GetHot())	//!ͨ��Ϊ����ִ��
		 			{
		 				status = pFieldPoint->force((Byte*)(tRunMgr.byCmdParam.data()), nLength);
		 			}
		 		}
		 		break;
		 	case CmdValue_UnForce:
		 		{
		 			if (GetHot())
		 			{
		 				status = pFieldPoint->cancelForce();
		 			}
		 		}
		 		break;
		 	case CmdValue_Disable:
		 		{
		 			if (GetHot())
		 			{
		 				status = pFieldPoint->Disable();
		 			}
		 		}
		 		break;
		 	case CmdValue_Enable:
		 		{
		 			CIOUnit* pIOUnit = pFieldPoint->getIOUnit();
		 			if (GetHot() && pIOUnit && (!pIOUnit->IsDisable()))
		 			{
		 				status = pFieldPoint->cancelDisable();
		 			}
		 		}
		 		break;
		 	case CmdValue_SyncVal2Hot:
		 		{
		 			CIOUnit* pIOUnit = pFieldPoint->getIOUnit();
		 			if (GetHot() && pIOUnit && !(pIOUnit->IsDisable()))
		 			{
		 				//pFieldPoint->setValue((Byte*)(tRunMgr.byCmdParam.data()), nLength, 0);
		 			}
		 		}
		 		break;
		 	default:
		 		break;
		 	}

		 	if (status == 0)
		 	{
		 		nResult = 0;
		 	}
		 }
		return nResult;
	}

	//!ͨ���Ƿ���ǿ��״̬
	bool CIODriver::IsForce()
	{
		if (m_pIOPort)
		{
			return m_pIOPort->IsForce();
		}
		return false;
	}

	//!ǿ��ͨ��״̬
	void CIODriver::ForcePort( Byte byPrmScnd )
	{
		if (m_pIOPort && !m_pIOPort->IsForce())
		{
			m_pIOPort->ForceChannel(byPrmScnd);
			//long nDateTime = long(UaDateTime::now());
			//CDBFrameAccess::FireFEPEvent(m_pIOPort, FEP_FORCE_NETWORK, (int)byPrmScnd, nDateTime);
		}
	}

	//!ȡ��ͨ��ǿ��״̬
	void CIODriver::DeforcePort()
	{
		if (m_pIOPort && m_pIOPort->IsForce())
		{
			m_pIOPort->DeforceChannel();
			//long nDateTime = long(UaDateTime::now());
			//CDBFrameAccess::FireFEPEvent(m_pIOPort, FEP_CANCELFORCE_NETWORK, nDateTime);
		}
	}

	//!���ø���:������ݱ����ʹ����ǩ��ַ����
	int CIODriver::UpdateDataVariable( FieldPointType* pFieldPoint )
	{
		int nResult = -1;
		if (pFieldPoint)
		{
			CIOUnit* pIOUnit = pFieldPoint->getIOUnit();
			if (pIOUnit)
			{
				std::string strDevName = pIOUnit->GetUnitName();
				CIODeviceBase* pDeviceBase = GetDeviceByName(strDevName);
				if (pDeviceBase)
				{
					nResult = pDeviceBase->UpdateDataVariable(pFieldPoint);
				}
			}
		}
		return nResult;
	}

	//!���ø���:ɾ������
	int CIODriver::BlockDriver()
	{
		int nResult = -1;
		
		//!������ֹͣ������ʶ
		 m_bIsBlocked = true;
		 //!ֹͣ���ݲɼ����շ�
		 nResult = StopWork();

		 //!�����豸���豸��ֹͣ������ʶ
		 int nDevCount = GetDeviceCount();
		 for (int nIndex = 0; nIndex < nDevCount; nIndex++)
		 {
		 	CIODeviceBase* pIODevice = GetDeviceByIndex(nIndex);
		 	if (pIODevice)
		 	{
		 		pIODevice->SetBlock(true);
		 	}
		 }
		
		return nResult;
	}

	//!�õ�����
	int CIODriver::GetSineY()
	{
		return m_nSineY;
	}

	//!������������
	int CIODriver::UpdateDrvCfg()
	{
		int nResult = -1;
		
		//!����ֹͣ����
		 nResult = StopWork();
		 if (0 == nResult)
		 {
		 	//!��ȡIOPort������Ϣ
		 	m_pIOPort->getIOPortCfg(m_pPortCfg);
		 }
		 //!�������¿�ʼ����
		 nResult = StartWork();

		return nResult;
	}

	//!���ø���:��ָͣ���豸�Ĺ���
	int CIODriver::BlockDevice( CIOUnit* pIOUnit )
	{
		int nResult = -1;
		if (pIOUnit)
		{
			std::string strDevName = pIOUnit->GetUnitName();
			CIODeviceBase* pIODevice = GetDeviceByName(strDevName);
			if (pIODevice)
			{
				pIODevice->SetBlock(true);
				nResult = 0;
			}
		}
		return nResult;
	}

	//!���ø���:�����豸��������Ϣ
	int CIODriver::UpdateDeviceCfg( CIOUnit* pIOUnit )
	{
		int nResult = -1;
		 if (pIOUnit)
		 {
		 	std::string strDevName = pIOUnit->GetUnitName();
		 	CIODeviceBase* pIODevice = GetDeviceByName(strDevName);
		 	if (pIODevice)
		 	{
		 		nResult = 0; //ADD for BUG 63124  FEP ��Ԫ����UpdateDeviceCfg����û����ȷ�ķ���ֵ on 20160203
		 		//!����ͣ����
		 		pIODevice->SetBlock(true);

		 		//!��һЩ��Ҫ�Ĺ���(���³�ʼ�����������¶�ȡ�����ļ�)

		 		//!�豸���¿�ʼ����
		 		ACE_OS::sleep( ACE_Time_Value(0,500*1000) );
		 		pIODevice->SetBlock(false);				
		 	}
		 }
		return nResult;
	}

	//!��ȡ��������վ������
	std::string CIODriver::GetFEPRunDir()
	{
		std::string strRunDir = "";
		//if (g_pDeployManager)
		{
			//string usSiteName = g_pDeployManager->GetDeploySite();
			//if (!usSiteName.isNull() && !usSiteName.isEmpty())
			{
				//strRunDir = SingletonRuntimeCatlog::instance()->GetRunDir()+"Server/" + usSiteName.toUtf8() + "/FEP/"; 
				strRunDir = SingletonRuntimeCatlog::instance()->GetRunDir()+"bin/files/";
			}
		}
		return strRunDir;
	}

	//!��ȡ����������Ϣ�����ļ�����
	std::string CIODriver::GetRunCommFepDir()
	{
		std::string strRunDir = "";
		if (NULL != g_pIOExtCfgManager)
		{
			strRunDir = g_pIOExtCfgManager->GetFepCommDrvDir();
		}
		return strRunDir;
	}

	bool CIODriver::IsStopWork()
	{
		return m_bIsBlocked;
	}

	int CIODriver::WriteLogAnyWay( const char* pchLog, bool bTime/*=true */ )
	{
		int nResult = -1;
		if (m_pLog)
		{
			m_pLog->WriteLog(pchLog, bTime);
			nResult = 0;
		}
		return nResult;
	}

	bool CIODriver::IsDouble()
	{
		return m_bDoubleComputer;
	}

	bool CIODriver::GetDevIndexByAddr( int devAddr, int& nIndex )
	{
		bool bRes = false;
		for (int index = 0; index < m_mapDevices.size(); index++)
		{
			CIODeviceBase* pDevice = GetDeviceByIndex(index);
			if (pDevice && pDevice->GetAddr() == devAddr)
			{
				nIndex = index;
				bRes = true;
			}
		}
		return bRes;
	}

	void CIODriver::InitDriverCommCfg()
	{
		if (NULL != g_pIOExtCfgManager)
		{
			tCommDriverCfg* pCmDrvCfg = g_pIOExtCfgManager->GetFepDecCommDrvCfg();
			if (NULL == pCmDrvCfg)
			{
				return;
			}
			m_nLogFileSize = pCmDrvCfg->LogFileSize;
			this->SetMonitor(pCmDrvCfg->CommuMonitorFlag);
			m_nLogFileStoreDays = pCmDrvCfg->LogFileStorDays;
			if (!pCmDrvCfg->LogFilePath.empty())
			{
				m_strFilePath = pCmDrvCfg->LogFilePath;
			}
			m_pPortCfg.strSrlBrdPrefix =  pCmDrvCfg->SrlBoardPrefix.c_str() ;
		}
		return;
	}

	void CIODriver::InitOpcUaDriverCfg()
	{
		if (NULL != g_pIOExtCfgManager)
		{
			//tUADriverCfg* pUaDrvCfg = g_pIOExtCfgManager->GetUADrvCfg();
			//if (NULL == pUaDrvCfg)
			//{
			//	return;
			//}
			//m_QueueSize = pUaDrvCfg->QueueSize;
		}
		return;
	}

	int CIODriver::getQueueSize()
	{
		return m_QueueSize;
	}

	void CIODriver::CreateDriverLogFolder()
	{
 		pid_t status = 0;
 		bool needTryAgain = false;
 		short tryTimes = 0;
 		//����FEP������־�ļ�Ŀ¼,���Ŀ¼���������κβ���//
 		do 
 		{
 			status = 0;
 			if(access(m_strFilePath.c_str(),0))
 			{
 #ifndef WIN32
 				const OpcUa_UInt16 BUFLENTH = 256;
 				char pCharBuf[BUFLENTH];
 				std::string needRunCmdStr = "mkdir " + m_strFilePath;
 				memset(pCharBuf,0,BUFLENTH);
 				memcpy(pCharBuf,needRunCmdStr.c_str(),needRunCmdStr.length());
 				status = system(pCharBuf);
 #else	
 				status = _mkdir(m_strFilePath.c_str());
 #endif
 			}
 			if (0 != status )
 			{
 				MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("CIODriver::CreateDriverLogFile Failed, Path: %s  then Will Use Default FilePath"), m_strFilePath.c_str()));
 				m_strFilePath = SingletonRuntimeCatlog::instance()->GetDebugLog() + "FEP/";
 				needTryAgain = true;
 				if (tryTimes >= 2)
 				{
 					needTryAgain = false;
 					MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("CIODriver::CreateDriverLogFile Failed, use default Path:%s create twice failed."), m_strFilePath.c_str()));
 				}
 				tryTimes++;
 			}
 			else
 			{
 				MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("INFOR CIODriver::CreateDriverLogFile[%s] folder success."), m_strFilePath.c_str()));
 				needTryAgain = false;
 			}			
 		} while (needTryAgain);		
	}

	void CIODriver::SetDevCHNCheck( bool bCheck )
	{
		 T_MapDevices::iterator it;
		 for(it = m_mapDevices.begin(); it != m_mapDevices.end(); it++)
		 {
		 	CIODeviceBase* pDevice = it->second;
		 	//!ʵ��Ϊ�����豸�Ž���ͨ�����
		 	if (pDevice && pDevice->IsHot())
		 	{
		 		pDevice->SetStatusUnit(bCheck);
		 	}
		 }
	}

	int CIODriver::ProcessHotSim()
	{
		int nRes = 0;

		//!��ǰ��·���Ƿ����豸����ģ��״̬
		T_MapDevices::iterator itDev = m_mapDevices.begin();
		while(itDev != m_mapDevices.end())
		{
			CIODeviceBase* pIODevice = itDev->second;
			if (pIODevice)
			{
				nRes = pIODevice->ProcesHotSim();
			}
			itDev++;
		}

		return nRes;
	}

	Byte CIODriver::GetSimMode()
	{
		Byte bySimMod = 1;
		if (m_pIOPort)
		{
			bySimMod = m_pIOPort->GetSimMode();
		}
		return bySimMod;
	}

	int CIODriver::SyncWriteCmd2SP( tIORemoteCtrl& tRemote )
	{
		int nRes = -1;
		//!��·��������ת������·�ӵ������
		 if (GetHot() && g_pIOCmdManager)
		 {
		 	char pchLog[256] = {0};
		 	tIOCtrlPacketCmd tPacketCmd;
		 //	tPacketCmd.objNodeId = tRemote.pNode->nodeId();
		 //	tPacketCmd.byType = IOPACKET_TYPE_SYNIOCMD2SN;
		 	int nRemoteDLen= tRemote.bysValue.length();
			if (nRemoteDLen <= MAX_PACKETCMD_LEN)
			{
				tPacketCmd.nLen = nRemoteDLen;
			}
		 	else
		 	{
		 		tPacketCmd.nLen = MAX_PACKETCMD_LEN;
		 		//snprintf(pchLog, 256, "CIODriver::SyncWriteCmd2SP Failed! The RemoteCmdLen:%d > MaxRCmdLen:%d, Node:%s", nRemoteDLen, MAX_PACKETCMD_LEN);
		 		WriteLogAnyWay(pchLog);
		 		nRes = -1;
		 		return nRes;
		 	}
		 	ACE_OS::memcpy(tPacketCmd.byData, tRemote.bysValue.data(), tPacketCmd.nLen);
		 	//!ת��ң��������
		 	nRes = g_pIOCmdManager->SendIOChannelDiag(tPacketCmd);
		 //	//snprintf(pchLog, 256, "CIODriver::SyncWriteCmd2SP Success! The Node: %s", tPacketCmd.objNodeId.toFullString().toUtf8());
		 //	WriteLogAnyWay(pchLog);

		 }
		return nRes;
	}
}
