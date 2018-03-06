/*!
 *	\file	IODriver.cpp
 *
 *	\brief	驱动类实现
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月22日	19:54
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
	//!类名
	std::string CIODriver::s_ClassName("CIODriver");

	//!构造函数
	CIODriver::CIODriver()
		:  m_pIOPrtclParser(NULL),m_pIOPort(NULL),m_pIODrvBoard(NULL), m_pLog(NULL), 
		m_pChannelState(NULL),  m_pCommLog(NULL), m_nCommDataFormat(16), m_nSineY(30),
		m_bIsBlocked(false), m_nTimeOut(1000), m_nWatchTime(30000), m_nPollTime(1000),
		m_iCurDevIndex(0),m_nRemoteQueueLimit(64),m_nLogFileSize(2),m_nLogFileStoreDays(0),m_QueueSize(6),m_pDbfParser(NULL)
	{
		m_strFilePath = SingletonRuntimeCatlog::instance()->GetDebugLog() + "FEP/";
		m_mapDevices.clear();
	}

	//!析构函数
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

	//!得到驱动类类名
	std::string CIODriver::GetClassName()
	{
		return s_ClassName;
	}

	//!初始化设备
	int CIODriver::InitUnit(CIOUnit* pIOUnit)
	{
		int nResult = -1;
		 if (NULL == pIOUnit)
		 {
		 	return nResult;
		 }

		 //!创建IODevice对象
		 std::string strDevName = GetDeviceClassName();
		 if (!strDevName.empty())
		 {
		 	CIODeviceBase* pDevice = g_DrvDeviceFactory.Create(strDevName);
		 	if (pDevice != NULL)
		 	{
		 		//!设置设备对象Driver和Prtcl对象信息
		 		pDevice->SetDriver(this);
		 		//pDevice->SetPrtcl(m_pIOPrtclParser);

		 		//!设置IOUnit,IOPort节点信息
		 		pDevice->SetIOUnitNode(pIOUnit);
		 		pDevice->SetIOCfgDriver(m_pIOPort);

		 		//!初始化数据变量信息
		 		nResult = pDevice->InitVariableInfo(pIOUnit);
		 		if (nResult != 0)
		 		{
		 			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriver::InitUnit(): InitVariableInfo failed! Device:%s"), pDevice->GetName().c_str()));
		 		}
		 		//!创建设备模拟对象
		 		std::string strDevSimName = GetDeviceSimClassName();
		 		CIODeviceSimBase* pDevSimBase = g_DrvSimFactory.Create(strDevSimName);
		 		if (pDevSimBase != NULL)
		 		{
		 			pDevSimBase->SetParent(pDevice);
		 			pDevice->SetIODeviceSim(pDevSimBase);
						
		 			pDevice->Init();

		 			//!保存设备对象
		 			m_mapDevices.insert(T_MapDevices::value_type(pIOUnit->GetUnitName(), pDevice));

		 			//!执行成功
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

	//!打开通道
	int CIODriver::OpenChannel()
	{
		int nResult = 0;
		 //!获取端口配置信息
		 m_pIOPort->getIOPortCfg(m_pPortCfg);

		 //!创建协议解析类对象
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

		 //!初始化驱动配置信息
		 InitDriverCfg();

		 //create store log folder
		 CreateDriverLogFolder();

		 //!设置遥控队列的大小
		 SetCtrlQueueLimit(m_nRemoteQueueLimit);

		 //!创建日志对象
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

		 //!创建通信监视对象
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

		 //!得到是否是双机配置
		 m_bDoubleComputer = m_pIOPort->IsDouble();

		return nResult;
	}

	//!关闭通道
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

	//!将遥控指令放入遥控队列
	int CIODriver::WriteDCB(tIORemoteCtrl& tRemote)
	{
		int nResult = 0;  //BUG 62790 单元测试 DBDriverFrame模块中的IODriver函数WriteDCB在执行成功时返回码为执行失败
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

		 					//!检查标签地址合法性
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

		 						//!通道禁止或设备禁止则不执行写标签指令
		 						if (!m_pIOPort->IsDisable() && !pDevice->IsDisable())
		 						{
		 							//!如果有量程转换则转换为通信值
		 							pFieldPoint->RangeData(tRCtrlCmd.byData, tRCtrlCmd.nDataLen, false);

		 							//!如果设备没有模拟且在线则将遥控命令放入遥控队列
		 							if (!pDevice->IsSimulate() && pDevice->IsOnLine())
		 							{
		 								nResult = m_IORemoteCtrl.Enqueue((char*)(&tRCtrlCmd), sizeof(tIOCtrlRemoteCmd));
		 							}
		 							else if (pDevice->IsOnLine() && pDevice->IsHot())
		 							{
		 								//!支持设备在线改值
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

	//!根据命令管理通道或设备运行状态
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

	//!发送通道诊断包命令
	int CIODriver::WritePacket(tIOCtrlPacketCmd& tPacket)
	{
		int nResult = -1;
		//!只有双机才处理诊断包
		// if (!m_bDoubleComputer)
		// {
		// 	return nResult;
		// }

		 //!处理另机发送过来数据包
		 if (m_pIOPrtclParser)
		 {
		 	nResult = m_pIOPrtclParser->RcvPcketCmd(&tPacket);
		 }

		return nResult;
	}

	//!设置设备的通道诊断标识
	int CIODriver::StatusUnit(void)
	{
		int nResult = -1;
		//!如果链路不冲突则直接返回
		if (!IsPortAccessClash())
		{
			return nResult;
		}

		//!设置设备的通道诊断状态
		SetDevCHNCheck(true);

		return nResult;
	}

	//!创建驱动器对象、初始化并打开驱动器
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

	//!关闭驱动器并删除驱动器对象
	int CIODriver::StopWork(void)
	{
		int nRes = -1;
		//!停止定时器
		 CIOPrtclParser* pIOPrtclParser = (CIOPrtclParser*)(GetIOPrtclParse());
		 if (pIOPrtclParser)
		 {
		 	nRes = pIOPrtclParser->StopTimer();
		 }
		 //!关闭驱动器
		 if (m_pIODrvBoard)
		 {
			 nRes = m_pIODrvBoard->close();
		 }
		 m_bIsBlocked = true;
		return nRes;
	}

	//!得到协议解析类类名
	std::string CIODriver::GetPrtclClassName()
	{
		return "CIOPrtclParser";
	}

	//!得到设备类类名
	std::string CIODriver::GetDeviceClassName()
	{
		return "CIODeviceBase";
	}

	//!得到设备模拟类类名
	std::string CIODriver::GetDeviceSimClassName()
	{
		return "CIODeviceSimBase";
	}

	//!得到下一个设备对象指针
	CIODeviceBase* CIODriver::GetNextDevice()
	{
		m_iCurDevIndex++;
		if (m_iCurDevIndex >= GetDeviceCount())
		{
			m_iCurDevIndex = 0;
		}
		return GetDeviceByIndex(m_iCurDevIndex);
	}

	//!得到当前轮训到的设备对象指针
	CIODeviceBase* CIODriver::GetCurDevice()
	{
		if (m_iCurDevIndex >= GetDeviceCount())
		{
			m_iCurDevIndex = 0;
		}
		return GetDeviceByIndex(m_iCurDevIndex);
	}

	//!通过设备名得到设备对象指针
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

	//!通过设备地址得到设备类对象指针
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

	//!通过设备地址得到设备类对象指针
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

	//!得到所有设备
	T_MapDevices& CIODriver::GetAllDevice()
	{
		return m_mapDevices;
	}

	//!得到设备总数
	int CIODriver::GetDeviceCount()
	{
		return m_mapDevices.size();
	}

	//!按序号得到设备对象指针
	CIODeviceBase* CIODriver::GetDeviceByIndex(int iIndex)
	{
		if (m_mapDevices.empty() )
		{
			return NULL;
		}
		//62996 FEP 单元测试   DBDriverFrame 模块中的IODriver.cpp 中的GetDeviceByIndex中对索引号的处理有误, 索引对于负数，等于容器的个数时返回均不正确
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

	//!得到驱动名称实际返回用户配置的Port名称
	std::string CIODriver::GetDriverName()
	{
		std::string strDrvName = "";
		strDrvName = m_pIOPort->getDrvName();
		return strDrvName;
	}

	//!得到驱动配置的协议名称
	std::string CIODriver::GetCfgPrtclName()
	{
		std::string strPrtclName = "";
		strPrtclName = m_pIOPort->getDrvName();
		return strPrtclName;
	}

	//!设置遥控队列的大小
	void CIODriver::SetCtrlQueueLimit(size_t nLimit)
	{
		 m_IORemoteCtrl.SetQueueLimit(nLimit);
	}

	//!写驱动运行日志
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

	//!将驱动通信信息写入调试工具同时写入日志
	int CIODriver::WriteCommData(Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd/*=0 */)
	{
		int nResult = -1;  
		/*
		if (this->IsMonitor())
		{
			//!将通信数据写入调试工具
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

		//!将通信监视数据写入日志文件
		 nResult = WriteCommDataToLog(pbyBuf, nDataLen, byRcvOrSnd);//MOD for BUG63093 WriteCommData函数没有正确的返回 on 20160203
		return nResult;
	}

	//!写驱动通信信息写入日志日志
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

		 	//!数据显示格式
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

	//!链路访问是否冲突
	bool CIODriver::IsPortAccessClash()
	{
		return m_pPortCfg.byPortAccessMode;
	}

	//!发送遥控之后的SOE
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

	//!获取超时时间
	int CIODriver::GetTimeOut()
	{
		return m_nTimeOut;
	}

	//!获取协议处理类指针
	CIOPrtclParserBase* CIODriver::GetIOPrtclParse()
	{
		return m_pIOPrtclParser;
	}

	//!通道是否被禁止
	bool CIODriver::IsDisable()
	{
		 return m_pIOPort->IsDisable();
	}

	//!是否处于模拟状态
	bool CIODriver::IsSimulate()
	{
		 return m_pIOPort->IsSimulate();
		return true;
	}

	//!设置通道状态
	// void CIODriver::SetChannelState(tIOScheduleChannelState* pChannelState)
	// {
	// 	// m_pChannelState = pChannelState;
	// }

	//!设置通道的模拟方式
	void CIODriver::SetSimulate(Byte bySimMode)
	{
		 if (m_pIOPort && !m_pIOPort->IsSimulate())
		 {
		 	m_pIOPort->SimulateChannel(bySimMode);
		 	//!该Driver下所有设备开启模拟
		 	T_MapDevices::iterator it = m_mapDevices.begin();
		 	while(it != m_mapDevices.end())
		 	{
		 		it->second->StartSimulate(bySimMode);
		 		it++;
		 	}
		 }
	}

	//!停止模拟
	void CIODriver::StopSimulate(void)
	{
		 if (m_pIOPort && m_pIOPort->IsSimulate())
		 {
		 	m_pIOPort->StopSimulate();
		 	//!该Driver下所有设备关闭模拟
		 	T_MapDevices::iterator it = m_mapDevices.begin();
		 	while(it != m_mapDevices.end())
		 	{
		 		it->second->StopSimulate();
		 		it++;
		 	}
		 }
	}

	//!设置通道禁止
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

	//!是否记录日志
	bool CIODriver::IsLog()
	{
		if (m_pIOPort)
		{
			return m_pIOPort->IsRecord();
		}
		return false;
	}

	//!设置是否记录日志标志
	void CIODriver::SetLog(bool byLog)
	{
		if (m_pIOPort)
		{
			m_pIOPort->SetRecord(byLog);
		}
	}

	//!是否处于通信监视状态
	bool CIODriver::IsMonitor()
	{
		if (m_pIOPort)
		{
			return m_pIOPort->IsMonitor();
		}
		return 0;
	}

	//!设置是否进行通信监视
	void CIODriver::SetMonitor(bool byMonitor)
	{
		if (m_pIOPort)
		{
			return m_pIOPort->SetMonitor(byMonitor);
		}
	}
	//!得到通道主从状态
	Byte CIODriver::GetHot()
	{
		if (m_pIOPort)
		{
			return (Byte)m_pIOPort->GetRealPrmScnd();
		}
		return 0;
	}

	//!设置通道冷热状态
	void CIODriver::SetHot(Byte byHot)
	{
		if (m_pIOPort)
		{
			m_pIOPort->SetRealPrmScnd(byHot);
		}
	}

	//!向设备发送运行管理命令
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
		 		case CmdValue_Simulate:							//!开始模拟
		 			{
		 				if ((tRunMgr.byCmdParam.data()) != NULL)
		 				{
		 					pDevice->StartSimulate(*(tRunMgr.byCmdParam.data()));
		 				}
		 			}
		 			break;
		 		case CmdValue_StopSimulate:						//!停止模拟
		 			pDevice->StopSimulate();
		 			break;
		 		case CmdValue_Disable:							//!设备禁止
		 			pDevice->SetDisable(true);
		 			break;
		 		case CmdValue_Enable:							//!取消禁止
		 			pDevice->SetDisable(false);
		 			break;
		 		case CmdValue_ExtraRangeEvent_1:				//!打开超量程事件开关
		 			pDevice->SetExtraRange(true);
		 			break;
		 		case CmdValue_ExtraRangeEvent_0:				//!关闭超量程事件开关
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

	//!向通道发送运行管理命令
	int CIODriver::WriteMgrForPort(tIORunMgrCmd& tRunMgr)
	{
		int nResult = -1;
		 if (m_pIOPort)
		 {
		 	switch(tRunMgr.byCmdValue)
		 	{
		 	case CmdValue_RedunForce:					//!强制主从
		 		{
		 			if ((tRunMgr.byCmdParam.data()) != NULL)
		 			{
		 				if (Redun_Prim == *(tRunMgr.byCmdParam.data()))
		 				{
		 					//!强制主
		 					this->ForcePort(true);
		 				} 
		 				else if(Redun_Second == *(tRunMgr.byCmdParam.data()) )
		 				{
		 					//!强制从
		 					this->ForcePort(false);
		 				}
		 			}
		 		}				
		 		break;
		 	case CmdValue_UnRedunForce:					//!取消强制
		 		this->DeforcePort();
		 		break;
		 	case CmdValue_Disable:						//!通道禁止
		 		this->SetDisable(true);
		 		break;
		 	case CmdValue_Enable:						//!取消禁止
		 		this->SetDisable(false);			
		 		break;
		 	case CmdValue_Simulate:						//!开始模拟
		 		{
		 			if ((tRunMgr.byCmdParam.data()) != NULL)
		 			{
		 				this->SetSimulate(*(tRunMgr.byCmdParam.data()));
		 			}
		 		}
		 		break;
		 	case CmdValue_StopSimulate:					//!停止模拟
		 		this->StopSimulate();
		 		break;
		 	case CmdValue_Monitor:						//!开始通信监视
		 		this->SetMonitor(true);
		 		break;
		 	case CmdValue_StopMonitor:					//!停止通信监视
		 		this->SetMonitor(false);			
		 		break;
		 	case CmdValue_StartLog:						//!开始记录日志
		 		this->SetLog(true);				
		 		break;
		 	case CmdValue_StopLog:						//!停止记录日志
		 		this->SetLog(false);
		 		break;
		 	default:
		 		break;
		 	}
		 	nResult = 0;
		 }
		return nResult;
	}

	//!获取驱动标签解析对象
	 CIOCfgDriverDbfParser* CIODriver::GetDbfParser()
	 {
	 	return m_pDbfParser;
	 }

	 //!设置驱动标签解析对象
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

	//!获取IOPort对象指针
	 CIOPort* CIODriver::GetIOPort()
	 {
	 	return m_pIOPort;
	 }

	 // !设置IOPort对象指针
	 void CIODriver::SetIOPort(CIOPort* pIOPort)
	 {
	 	if (pIOPort != NULL)
	 	{
	 		m_pIOPort = pIOPort;
	 	}
	 }

	//!从MacsDriver.ini文件中获取时间信息配置
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

		 //!正选模拟Y值大小
		 status = cf.get_string_value(DrvSection, ACE_TEXT("SineY"), strV);
		 sscanf(strV.c_str(), "%d", &nValue);
		 if (0 == status)
		 {
		 	m_nSineY = (int)nValue;
		 }

		return;
	}

	//!获取Status检测时间
	int CIODriver::GetStatusWatchTime()
	{
		return m_nWatchTime;
	}

	//!输出调试信息
	void CIODriver::OutputDebug( const char* pchInfo )
	{
		OutputDebugString(pchInfo);
	}

	//!处理对标签标签运行管理命令
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
		 			if (GetHot())	//!通道为主才执行
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

	//!通道是否处于强制状态
	bool CIODriver::IsForce()
	{
		if (m_pIOPort)
		{
			return m_pIOPort->IsForce();
		}
		return false;
	}

	//!强制通道状态
	void CIODriver::ForcePort( Byte byPrmScnd )
	{
		if (m_pIOPort && !m_pIOPort->IsForce())
		{
			m_pIOPort->ForceChannel(byPrmScnd);
			//long nDateTime = long(UaDateTime::now());
			//CDBFrameAccess::FireFEPEvent(m_pIOPort, FEP_FORCE_NETWORK, (int)byPrmScnd, nDateTime);
		}
	}

	//!取消通道强制状态
	void CIODriver::DeforcePort()
	{
		if (m_pIOPort && m_pIOPort->IsForce())
		{
			m_pIOPort->DeforceChannel();
			//long nDateTime = long(UaDateTime::now());
			//CDBFrameAccess::FireFEPEvent(m_pIOPort, FEP_CANCELFORCE_NETWORK, nDateTime);
		}
	}

	//!配置更新:添加数据变量和处理标签地址更新
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

	//!配置更新:删除驱动
	int CIODriver::BlockDriver()
	{
		int nResult = -1;
		
		//!置驱动停止工作标识
		 m_bIsBlocked = true;
		 //!停止数据采集和收发
		 nResult = StopWork();

		 //!遍历设备置设备的停止工作标识
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

	//!得到正弦
	int CIODriver::GetSineY()
	{
		return m_nSineY;
	}

	//!更新驱动配置
	int CIODriver::UpdateDrvCfg()
	{
		int nResult = -1;
		
		//!驱动停止工作
		 nResult = StopWork();
		 if (0 == nResult)
		 {
		 	//!读取IOPort配置信息
		 	m_pIOPort->getIOPortCfg(m_pPortCfg);
		 }
		 //!驱动重新开始工作
		 nResult = StartWork();

		return nResult;
	}

	//!配置更新:暂停指定设备的工作
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

	//!配置更新:更新设备的配置信息
	int CIODriver::UpdateDeviceCfg( CIOUnit* pIOUnit )
	{
		int nResult = -1;
		 if (pIOUnit)
		 {
		 	std::string strDevName = pIOUnit->GetUnitName();
		 	CIODeviceBase* pIODevice = GetDeviceByName(strDevName);
		 	if (pIODevice)
		 	{
		 		nResult = 0; //ADD for BUG 63124  FEP 单元测试UpdateDeviceCfg函数没有正确的返回值 on 20160203
		 		//!先暂停工作
		 		pIODevice->SetBlock(true);

		 		//!做一些需要的工作(重新初始化变量或重新读取配置文件)

		 		//!设备重新开始工作
		 		ACE_OS::sleep( ACE_Time_Value(0,500*1000) );
		 		pIODevice->SetBlock(false);				
		 	}
		 }
		return nResult;
	}

	//!获取驱动所在站点名称
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

	//!获取驱动公共信息配置文件名称
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
 		//创建FEP驱动日志文件目录,如果目录存在则不作任何操作//
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
		 	//!实际为主的设备才进行通道诊断
		 	if (pDevice && pDevice->IsHot())
		 	{
		 		pDevice->SetStatusUnit(bCheck);
		 	}
		 }
	}

	int CIODriver::ProcessHotSim()
	{
		int nRes = 0;

		//!当前链路下是否有设备处于模拟状态
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
		//!链路主将命令转发到链路从的另机上
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
		 	//!转发遥控命令到另机
		 	nRes = g_pIOCmdManager->SendIOChannelDiag(tPacketCmd);
		 //	//snprintf(pchLog, 256, "CIODriver::SyncWriteCmd2SP Success! The Node: %s", tPacketCmd.objNodeId.toFullString().toUtf8());
		 //	WriteLogAnyWay(pchLog);

		 }
		return nRes;
	}
}
