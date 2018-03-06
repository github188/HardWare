/*!
 *	\file	IODeviceBase.cpp
 *
 *	\brief	设备基类的实现
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月17日	18:39
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IODeviceSimBase.h"
#include "RTDBDriverFrame/IODriver.h"
#include "RTDBDriverFrame/IODeviceBase.h"
#include "RTDBDriverFrame/IOCfgUnit.h"
#include "RTDBDriverFrame/IOCfgTag.h"
#include "RTDBDriverFrame/IOCfgUnitType.h"
#include "IOCfgDriverDbfParser.h"
#include "RTDBPlatformClass/IOPort.h"
#include "RTDBPlatformClass/IOUnit.h"
#include "RTDBPlatformClass/FieldPointType.h"
#include "RTDBCommon/Debug_Msg.h"

namespace MACS_SCADA_SUD
{
	//!定义设备类工厂
	IODRIVERFRAME_VAR  GenericFactory<CIODeviceBase> g_DrvDeviceFactory;

	//!定义设备基类类名
	IODRIVERFRAME_VAR std::string CIODeviceBase::s_ClassName("CIODeviceBase");

	//!构造函数
	CIODeviceBase::CIODeviceBase(void)
		:m_pIOUnit(NULL), m_pUnitState(NULL), m_pDeviceSim(NULL),
		m_pIOPort(NULL), m_pIOUnitCfg(NULL), m_nTimerID(0),
		m_pDriver(NULL), m_pPrtcl(NULL), m_byRxTimes(0), m_byTxTimes(0),
		m_bStatusUnit(false), m_bSetQualityFirstFlag(true), m_bIsBlocked(false)
	{
		
	}

	//!析构函数
	CIODeviceBase::~CIODeviceBase(void)
	{
		//!析构Devicesim对象
		if (m_pDeviceSim)
		{
			delete m_pDeviceSim;
			m_pDeviceSim = NULL;
		}

		//!析构CIOCfgUnit对象
		if (m_pIOUnitCfg)
		{
			delete m_pIOUnitCfg;
			m_pIOUnitCfg = NULL;
		}
	}

	//!得到驱动名称
	std::string CIODeviceBase::GetClassName()
	{
		return s_ClassName;
	}

	//!设备标签初始化,建立设备对象中包含FieldPoint索引表
	int CIODeviceBase::Init()
	{
		int rcd = -1;
		 if (!m_pIOUnitCfg)
		 {
		 	MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::Init(): m_pIOCfgUnit is NULL! Device:%s"), GetName().c_str()));
		 	return rcd;
		 }

		 rcd = InitMapObjTag();
         if (0 != rcd)
         {
		 	return rcd;
         }

		 rcd = InitStrAddMapObjTag();
		 if (0 != rcd)
		 {
		 	return rcd;
		 }

		//读取设备配置信息
		 std::string strDevCfgFile = CIODriver::GetFEPRunDir() + m_pIOUnitCfg->GetUnitCfgName();
		 InitDevConfig(strDevCfgFile);

		return 0;
	}

	//!设置设备为模拟状态
	void CIODeviceBase::StartSimulate(Byte bySimulate)
	{
		//!如果设备已经开始模拟则先停止模拟再启动模拟
		 if (IsSimulate())
		 {
		 	MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("CIODeviceBase::SetSimulate: Device=%s already simulate,please stop simulate!"), GetName().c_str()));
		 	return;
		 }

		 //!设备并没有暂停工作启动模拟(链路为从设备可以开启模拟定时器但并不会更新数据)
		 if (IsBlock())
		 {
		 	MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("CIODeviceBase::SetSimulate: Device=%s is not Hot,can not set simulate!"), GetName().c_str()));
		 	return;
		 }

		 //!启动模拟
		 if (GetIODeviceSim())
		 {
		 	GetIODeviceSim()->SetSimMode(bySimulate);
		 	GetIODeviceSim()->Start();
		 	if (m_pIOUnit)
		 	{
		 		m_pIOUnit->StartSimulate(bySimulate);
		 	}

		 	//!置设备在线
		 	SetOnLine(DEVICE_ONLINE);
		 }
	}

	//!停止模拟
	void CIODeviceBase::StopSimulate(void)
	{
		//!如果设备当前没有模拟但收到停止模拟命令则直接返回
		if (!IsSimulate())
		{
			MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("CIODeviceBase::SetSimulate: Device=%s is not Simulating,can not stop simulating!"), GetName().c_str()));
			return;
		}

		if (GetIODeviceSim() && !IsBlock())
		{
			GetIODeviceSim()->Stop();
			GetIODeviceSim()->m_bySimMode = 0;
			if (m_pIOUnit)
			{
				m_pIOUnit->StopSimulate();
			}
		}
	}

	//!设备是否处于模拟状态
	bool CIODeviceBase::IsSimulate(void)
	{
		 return m_pIOUnit->IsSimulate();
		return false;
	}

	//!设置标签值
	void CIODeviceBase::SetVal(Byte byUNIT_TYPE, int lTagAddr, Byte* pbyData, int nDataLen, long nTime, bool bFromLocal)
	{
		//!通道禁止、设备禁止、从机、暂停工作时不写入值
		 if (m_pIOPort->IsDisable() || this->IsDisable() || !this->IsHot() || this->IsBlock())
		 {
		 	return;
		 }
		
		 //!向FieldPoint中写值
		 T_mapFieldPoints lstFPoints;
		 FieldPointType* pFieldPoint = NULL;
		 T_mapFieldPoints::iterator itFP;
		 OBJTAG tOBJTag;
		 tOBJTag.byUnitType = byUNIT_TYPE;
		 tOBJTag.lTagAddr = lTagAddr;
		 MAP_OBJTAG::iterator it = m_mapObjTag.find(tOBJTag.GetIndexAddr());
		 if (it != m_mapObjTag.end())
		 {
		 	CIOCfgTag* pIOCfgTag = it->second;
		 	lstFPoints = pIOCfgTag->GetFieldPoints();
		 	if (1 == lstFPoints.size())
		 	{
		 		pFieldPoint = lstFPoints.begin()->second;
		 		if (pFieldPoint)
		 		{
		 			//取出旧值
		 			CKiwiVariant uaVarOld = pFieldPoint->value();
					
		 			//!向地址空间中写值
		 			//pFieldPoint->setValue(pbyData, nDataLen, nTime);
		 			if (pFieldPoint->getNeedSOE())
		 			{
		 				int nRes = FireSOEEvent(pFieldPoint, uaVarOld, nTime);
		 			}
		 		}
		 	}
		 	else if(lstFPoints.size() > 1)
		 	{
		 		Byte* byArrVal = new Byte[nDataLen];
		 		 for (itFP = lstFPoints.begin(); itFP != lstFPoints.end(); itFP++)
		 		 {
		 		 	memset(byArrVal, 0, nDataLen);
		 		 	memcpy(byArrVal, pbyData, nDataLen);
		 		 	pFieldPoint = itFP->second;
		 		 	if (pFieldPoint)
		 		 	{
		 		 		//取出旧值
		 		 		CKiwiVariant uaVarOld = pFieldPoint->value();

		 		 		//!向地址空间中写入值
		 		 		//pFieldPoint->setValue(byArrVal, nDataLen, nTime);
		 		 		if (pFieldPoint->getNeedSOE())
		 		 		{
		 		 			int nRes = FireSOEEvent(pFieldPoint, uaVarOld, nTime);
		 		 		}
		 		 	}
		 		 }
		 		delete []byArrVal;
		 	}
		 }
	}

	//!设置标签值(标签地址为字符串)
	void CIODeviceBase::SetVal(Byte byUNIT_TYPE, std::string sTagAddr, Byte* pbyData, int nDataLen, long nTime, bool bFromLocal)
	{
		
	}

	//!得到设备地址(以字符串形式表示)
	std::string CIODeviceBase::GetStringAddr()
	{
		 if (m_pIOUnit)
		 {
		 	char chName[NAME_LENGTH];
		 	int nDevAddr;
		 	nDevAddr = m_pIOUnit->GetDevAddr();
		 	//MOD for BUG62992 FEP 单元测试   DBDriverFrame 模块中的IODeviceBase.cpp 中的GetStringAddr函数调用ACE::OS::itoa的函数入参使用有误 on 20160203 below
		 	//由于在组态时都是使用的10进制所以转化时使用10进制
		 	//std::string strAddr = ACE_OS::itoa(nDevAddr, chName, NAME_LENGTH); 
             std::string strAddr = ACE_OS::itoa(nDevAddr, chName, 10); 
		 	//MOD for BUG62996 FEP 单元测试   DBDriverFrame 模块中的IODeviceBase.cpp 中的GetStringAddr函数调用ACE::OS::itoa的函数入参使用有误 on 20160203 above
		 	return strAddr;
		 }
		 else
		{
			return "";
		}
	}

	//!得到设备地址
	int CIODeviceBase::GetAddr()
	{
		if (m_pIOUnit)
		{
			int nDevAddr;
			nDevAddr = m_pIOUnit->GetDevAddr();
			return nDevAddr;
		} 
		else
		{
			return -1;
		}
	}



	//!得到设备名称
	std::string CIODeviceBase::GetName()
	{
		std::string strResult = "";
		if (m_pIOUnit)
		{
			std::string uaDevName = m_pIOUnit->GetUnitName();
			if (!uaDevName.empty())
			{
				strResult = uaDevName;
			}
		} 
		return strResult;
	}

	std::string CIODeviceBase::GetDescription()
	{
		std::string strResult = "";
		if (m_pIOUnit)
		{
			std::string uaDevName = m_pIOUnit->GetUnitName();
			if (uaDevName.empty())
			{
				strResult = m_pIOUnit->GetUnitName();
			}
		} 
		return strResult;
	}

	//!设备是否为主
	bool CIODeviceBase::IsHot()
	{
		return m_pIOUnit->PrmOrScnd();
	}

	//!设置设备的主从
	void CIODeviceBase::SetHot(Byte byHot)
	{
		if (m_pIOUnit)
		{
			m_pIOUnit->SetRealPrmScnd(byHot);
		}
	}

	//!设备是否在线
	bool CIODeviceBase::IsOnLine()
	{
		 return m_pIOUnit->IsOnline();
	}

	CIOCfgTag* CIODeviceBase::GetDevCommIoCfgTag(std::string DecCommName)
	{
		 if (DecCommName.empty())
		 {
		 	return NULL;
		 }

		 STRMAP_OBJTAG::iterator it = m_strAddMapObjTag.find(DecCommName);
		 if (it == m_strAddMapObjTag.end())
		 {
		 	//没有组态设备通信点，退出//
		 	return NULL;
		 }
		 else
		 {
		 	return it->second;
		 }		
		return NULL;
	}

	bool CIODeviceBase::SetDevCommFp4IoCfgTag(CIOCfgTag* pIoCfgTag, Byte byOnLine)
	{
	    if (NULL == pIoCfgTag)
	    {
			return false;
	    }

		if (pIoCfgTag->GetFieldPoints().size() < 0)
		{
			return false;
		}

		if (0 == pIoCfgTag->GetFieldPoints().size())
		{
			return true;
		}

		 T_mapFieldPoints mapFPoints = pIoCfgTag->GetFieldPoints();
		 T_mapFieldPoints::iterator itFP = mapFPoints.begin();
		 for (; itFP != mapFPoints.end(); itFP++)
		 {
		 	FieldPointType* pFiledPoint = itFP->second;
		 	if (pFiledPoint)
		 	{
		 		//pFiledPoint->setValue(&byOnLine, sizeof(byOnLine), 0);
		 	}
		 }

		return true;
	}

	bool CIODeviceBase::ProcessDevCommValue(Byte byOnLine)
	{
		std::string fepDevCommStr = "";	
		std::string locDevCommStr = "";	
		std::string peerDevCommStr = "";		

	 	if (NULL == m_pIOUnit || NULL == m_pIOPort)
	 	{
	 		return false;
	 	}
	 	else
	 	{
	 		fepDevCommStr =  std::string(DT_ISONLINE);
	 		locDevCommStr = m_pIOUnit->GetLocalDevCommType();
	 		peerDevCommStr = m_pIOUnit->GetPeerDevCommType();
	 	}		
		
	 	if (IsHot())
	 	{
	 		//!本机当前链路为主且与另机链接中断则将另机通讯状态点置为0
	 		if (!m_pIOPort->GetConnect2Peer())  
	 		{
	 			CIOCfgTag* pPeerCommTag = GetDevCommIoCfgTag(peerDevCommStr);
	 			if (NULL != pPeerCommTag)
	 			{
	 				Byte byPeerOnLine = 0; 
	 				SetDevCommFp4IoCfgTag(pPeerCommTag, byPeerOnLine);
	 			}
	 		}
	 	}

	 	//!更新整机通讯状态点DEVCOMM
	 	CIOCfgTag* pDevCommTag = GetDevCommIoCfgTag(fepDevCommStr);			
	 	if (NULL != pDevCommTag)
	 	{
	 		SetDevCommFp4IoCfgTag(pDevCommTag, byOnLine);
	 	}

	 	//!更新本机通讯状态点,并将该操作发送到链路为主设备上执行已使主从之间值保持一致
	 	CIOCfgTag* pLocCommTag = GetDevCommIoCfgTag(locDevCommStr);			
	 	if (NULL != pLocCommTag)
	 	{
	 		SetDevCommFp4IoCfgTag(pLocCommTag, byOnLine);
	 		int rcd = SetVal2Hot(pLocCommTag, byOnLine);
	 		if ( 0 != rcd)
	 		{
	 			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::ProcessDevCommValue SetVal2Hot failed rcd = %d!\n"), rcd));
	 		}
	 	}
		
		return true;
	 }
	
	 //!设置设备在线离线状态
	void CIODeviceBase::SetOnLine(Byte byOnLine)
	{
		 if ( NULL == m_pIOUnit)
		 {
		 	return;
		 }

		 //!要设置的链路状态只能是在线或离线//
		 if (DEVICE_ONLINE != byOnLine && DEVICE_OFFLINE != byOnLine)
		 {
		 	return;
		 }
		
		 //!在线状态没有发生改变或发生链路主从状态切换进行处理，否则不做任何处理
		 //EXTERNAL_EVENT byOnlineEvent;
		 IOPortSwitchStatus IOLinkState = m_pIOUnit->GetIOPortSwitchStatus();
		 Byte oldOnLine = m_pIOUnit->GetRunState();
		 if (DEVICE_ONLINE == byOnLine)
		 {
		 	if (!m_pIOUnit->IsOnline())
		 	{
		 		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::SetOnLine Dev:%s Online! IOLinkSwitch:%d\n"), this->GetName().c_str(), IOLinkState));
		 		m_pIOUnit->SetOnline();
		 		//byOnlineEvent = FEP_DEVICE_ONLINE;
		 		//!更新设备通讯状态标签值
		 		//ProcessDevCommValue(byOnLine);
		 	}
		 	else if (m_pIOUnit->IsOnline() && (IOLinkState > IOPORT_NO_SWITCH))
		 	{
		 		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::SetOnLine Dev:%s Online! IOLinkSwitch:%d\n"), this->GetName().c_str(), IOLinkState));
		 		m_pIOUnit->SetOnline(false);
		 		//!更新设备通讯状态标签值
		 		//ProcessDevCommValue(byOnLine);
		 	}
		 }
		 else if(DEVICE_OFFLINE == byOnLine)
		 {
		 	if (!m_pIOUnit->IsOffLine())
		 	{
		 		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::SetOnLine Dev:%s OffLine! IOLinkSwitch:%d\n"), this->GetName().c_str(), IOLinkState));
		 		m_pIOUnit->SetOffline();
		 		//byOnlineEvent = FEP_DEVICE_OFFLINE;
		 		//!更新设备通讯状态标签值
		 		//ProcessDevCommValue(byOnLine);
		 	}
		 	else if (m_pIOUnit->IsOffLine() && (IOLinkState > IOPORT_NO_SWITCH))
		 	{
		 		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::SetOnLine Dev:%s OffLine! IOLinkSwitch:%d\n"), this->GetName().c_str(), IOLinkState));
		 		m_pIOUnit->SetOffline(false);
		 		//!更新设备通讯状态标签值
		 		//ProcessDevCommValue(byOnLine);
		 	}
		 }
		 else
		 {
			 return;
		 }

		 Byte newOnLine = m_pIOUnit->GetRunState();
		 //!将修改后的在线状态发送到实时服务
		 if (this->GetDriver()->GetCfgPrtclName() != OPCUADRIVER && oldOnLine != newOnLine )
		 {
		 	unsigned long nDateTime = (unsigned long)(ACE_OS::gettimeofday().sec());
		 	//CDBFrameAccess::FireFEPEvent(m_pIOUnit, byOnlineEvent, (int)byOnLine, nDateTime);			
		 }

		 //!如果组态了设备状态标签则设置状态标签值//
		 if (m_pIOUnitCfg)
		 {
		 	ProcessDevCommValue(byOnLine);
		 }

		 //!如果是因为设备所在链路状态发生了变化而引起的更新，在更新后还需恢复链路变化状态位//
		 if (IOLinkState >IOPORT_NO_SWITCH)
		 {
		 	m_pIOUnit->SetIOPortSwitchStatus(IOPORT_NO_SWITCH);
		 }
	}

	//!设备是否离线
	bool CIODeviceBase::IsOffLine()
	{
		bool bRes = false;
		 if (m_pIOUnit && (DEVICE_OFFLINE == m_pIOUnit->GetRunState()))
		 {
		 	bRes = true;
		 }
		return bRes;
	}

	//!设备是否处于初始状态
	bool CIODeviceBase::IsInit()
	{
		bool bRes = false;
		 if (m_pIOUnit && (DEVICE_INIT == m_pIOUnit->GetRunState()))
		 {
		 	bRes = true;
		 }
		return bRes;
	}

	//!设备是否被禁止
	bool CIODeviceBase::IsDisable()
	{
		 if (m_pIOUnit)
		 {
		 	return m_pIOUnit->IsDisable();
		 }	
		return false;
	}

	//!设置设备禁止状态
	void CIODeviceBase::SetDisable(Byte byDisable)
	{
		CIODriver* pDriver = dynamic_cast<CIODriver*>(GetDriver());
		if (pDriver)
		{
			unsigned long nDateTime = (unsigned long)(ACE_OS::gettimeofday().sec());
			if (m_pIOUnit && byDisable == TRUE && !m_pIOUnit->IsDisable())
			{
				m_pIOUnit->DisableUnit();
				if (IsHot())
				{
					//CDBFrameAccess::FireFEPEvent(m_pIOUnit, FEP_DISABLE_DEVICE, (int)byDisable, nDateTime);
				}
			}
			else if (m_pIOUnit && byDisable == FALSE && m_pIOUnit->IsDisable() && !pDriver->IsDisable())
			{
				m_pIOUnit->EnableUnit();
				if (IsHot())
				{
					//CDBFrameAccess::FireFEPEvent(m_pIOUnit, FEP_ENABLE_DEVICE, (int)byDisable, nDateTime);
				}
			}
		}
	}

	//!设置设备是否超量程
	bool CIODeviceBase::IsExtraRange()
	{
		bool result = false;
		 if (m_pIOUnit)
		 {
		 	result = m_pIOUnit->GetExtraRange();
		 }
		return result;
	}

	//!设置设备超量程状态
	void CIODeviceBase::SetExtraRange(Byte byExtraRange)
	{
		 if (m_pIOUnit && byExtraRange)
		 {
		 	m_pIOUnit->SetExtraRange(true);
		 }
		 else if (m_pIOUnit && byExtraRange)
		 {
		 	m_pIOUnit->SetExtraRange(false);
		 }
	}

	// !设置设备状态指针
	void CIODeviceBase::SetUnitState(tIOScheduleUnitState* pUnitState)
	{
		m_pUnitState = pUnitState;
	}

	// !获取设备状态指针
	tIOScheduleUnitState* CIODeviceBase::GetUnitState()
	{
		return m_pUnitState;
	}

	// !设置设备节点指针
	void CIODeviceBase::SetIOUnitNode(CIOUnit* pUnit)
	{
		if (pUnit)
		{
			m_pIOUnit = pUnit;
		}
	}

	// !获取设备节点指针
	CIOUnit* CIODeviceBase::GetIOUnitNode()
	{
		return m_pIOUnit;
	}

	// !获取设备标签信息管理对象
	CIOCfgUnit* CIODeviceBase::GetIOCfgUnit()
	{
		return m_pIOUnitCfg;
	}

	// !设置设备对应的驱动配置信息即CIOPort信息
	void CIODeviceBase::SetIOCfgDriver(CIOPort* pIOPort)
	{
		if (pIOPort)
		{
			m_pIOPort = pIOPort;
		}
	}

	// !获取设备对应的驱动配置信息即IOPort信息
	CIOPort* CIODeviceBase::GetIOCfgDriver()
	{
		return m_pIOPort;
	}

	// !设置模拟设备的对象指针
	void CIODeviceBase::SetIODeviceSim(CIODeviceSimBase* pDeviceSim)
	{
		if (pDeviceSim)
		{
			m_pDeviceSim = pDeviceSim;
			//ADD for BUG61029 on 20151116 below 
			if (NULL == m_pIOUnit) 
			{
				return; //Add for BUG62552  FEP单元测试  DBDriverFrame 模块IODeviceBase.cpp 使用指针无NULL判断，存在崩溃风险
			}
            m_pIOUnit->SetIODeviceSim(pDeviceSim);
			//ADD for BUG61029 on 20151116 above
		}
	}

	// !得到设备的模拟对象指针
	CIODeviceSimBase* CIODeviceBase::GetIODeviceSim()
	{
		return m_pDeviceSim;
	}

	//!核查UnitType、TagAddr是否有效
	bool CIODeviceBase::CheckUnitTypeAndTagAddr(Byte byUnitType, int lTagAddr)
	{
		OBJTAG tOBJTag;
		 tOBJTag.byUnitType = byUnitType;
		 tOBJTag.lTagAddr = lTagAddr;
		 MAP_OBJTAG::iterator it = m_mapObjTag.find(tOBJTag.GetIndexAddr());
		 if (it != m_mapObjTag.end())
		 {
		 	return true;
		 }
		return false;
	}

	//!得到FeildPoint MAP表
	MAP_OBJTAG& CIODeviceBase::GetMapobjTag()
	{
		return m_mapObjTag;
	}

	//!设置驱动对象指针
	void CIODeviceBase::SetDriver(CIODriverBase* pDriver)
	{
		if (pDriver)
		{
			m_pDriver = pDriver;
		}
	}

	//!获取驱动对象指针
	CIODriverBase* CIODeviceBase::GetDriver()
	{
		return m_pDriver;
	}

	//!设置协议对象指针
	 void CIODeviceBase::SetPrtcl(CIOPrtclParserBase* pPrtcl)
	 {
	 	if (pPrtcl)
	 	{
	 		m_pPrtcl = pPrtcl;
	 	}
	 }

	//!获取协议对象指针
	 CIOPrtclParserBase* CIODeviceBase::GetPrtcl()
	 {
	 	return m_pPrtcl;
	 }

	//!设置初始标识
	void CIODeviceBase::SetQualityFirstFlag(bool bSet)
	{
		m_bSetQualityFirstFlag = bSet;
	}

	//!是否进行通道诊断
	bool CIODeviceBase::IsStatusUnit()
	{
		return m_bStatusUnit;
	}

	//!设置通道诊断标识
	void CIODeviceBase::SetStatusUnit(bool bStatusUnit)
	{
		m_bStatusUnit = bStatusUnit;
	}

	//!通过索引获取标签对象指针
	CIOCfgTag* CIODeviceBase::GetTagByIndex(int nUnitType, int lTagAddr)
	{
		CIOCfgTag* pIOCfgTag = NULL;
		OBJTAG tOBJTag;
		tOBJTag.byUnitType = (Byte)nUnitType;
		tOBJTag.lTagAddr = lTagAddr;
		MAP_OBJTAG::iterator it = m_mapObjTag.find(tOBJTag.GetIndexAddr());
		if (it != m_mapObjTag.end())
		{
			pIOCfgTag = it->second;
		}
		return pIOCfgTag;
	}

	//!初始化IOUnit下的数据变量(FieldPoint)信息
	int CIODeviceBase::InitVariableInfo(CIOUnit* pIOUnit)
	{
		int nResult = -1;
		//Add for BUG62547 FEP单元测试DriverFrame模块的IDDeviceBase.cpp中的InitVariableInfo函数没有对入参进行有效性检查，并且对成员变量的判断用else即可 on 20160106 below 
		if (NULL == pIOUnit)
		{
			return nResult;
		}
		//Add for BUG62547 FEP单元测试DriverFrame模块的IDDeviceBase.cpp中的InitVariableInfo函数没有对入参进行有效性检查，并且对成员变量的判断用else即可 on 20160106 above 
		if (NULL == m_pIOUnitCfg)
		{
			m_pIOUnitCfg = new CIOCfgUnit(this);
			//!创建标签类型
			m_pIOUnitCfg->CreateUnitTypeList();
		}

		if(m_pIOUnitCfg)
		{
			//!将IOUnit下的FieldPoint信息初始化到IODevice
			std::vector<CKiwiVariable*> vecTags;
			pIOUnit->GetAllTags(vecTags);
			int nCount = vecTags.size();
			if (NULL == m_pDriver)
			{
				return nResult; //Add for for BUG62547
			}
			CIOCfgDriverDbfParser* pDrvDbfParser = m_pDriver->GetDbfParser();
			if (pDrvDbfParser != NULL)
			{
				nResult = 0;
				for (int nIndex = 0; nIndex < nCount; nIndex++)
				{
					FieldPointType* pFeildPoint = dynamic_cast<FieldPointType*>(vecTags[nIndex]);
					if (pFeildPoint)
					{
						nResult = ProcessFieldPoint(pFeildPoint, pDrvDbfParser);
					}
				}
			}
			else
			{
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::InitVariableInfo: GetDbfParser is NULL! Device:%s"), GetName().c_str()));
			}
		}

		return nResult;
	}

	//!设备是否处于强制状态
	bool CIODeviceBase::IsForce()
	{
		if (m_pIOUnit)
		{
			return m_pIOUnit->IsForce();
		}
		return false;
	}

	//!强制设备的主从状态
	void CIODeviceBase::Force( Byte byPrmScnd )
	{
		if (m_pIOUnit)
		{
			m_pIOUnit->ForceUnit(byPrmScnd);
		}
	}

	//!取消设备强制
	void CIODeviceBase::DeforceUnit()
	{
		if (m_pIOUnit)
		{
			m_pIOUnit->DeforceUnit();
		}
	}

	//!配置更新:添加数据变量和FeildPoint标签地址更新
	int CIODeviceBase::UpdateDataVariable( FieldPointType* pFieldPoint )
	{
		int nResult = -1;
		if (pFieldPoint)
		{
			CIOCfgDriverDbfParser* pDrvDbfParser = m_pDriver->GetDbfParser();
			nResult = ProcessFieldPoint(pFieldPoint, pDrvDbfParser);
		}
		return nResult;
	}

	//!处理FieldPoint将其放入合适UnitType计算标签地址
	 int CIODeviceBase::ProcessFieldPoint( FieldPointType* pFieldPoint, CIOCfgDriverDbfParser* pDrvDbfParser)
	 {
	 	int nResult = -1;
	 	tIOCfgUnitDataDes tDataDes;
	 	CIOCfgUnitType* pIOCfgUntiType = NULL;
	 	int lTagItemVal[32];
	 	if (pFieldPoint != NULL && pDrvDbfParser != NULL && m_pIOUnitCfg != NULL)
	 	{
	 		long nNodeId = pFieldPoint->nodeId();
			std::string uaTagAddr = pFieldPoint->getOffset();
	 		if (!uaTagAddr.empty())
	 		{
	 			std::string strTagAddr = uaTagAddr;
	 			CIOCfgTag* pIOCfgTag = m_pIOUnitCfg->GetIOCfgTagByAddr(strTagAddr);
	 			if (NULL == pIOCfgTag)
	 			{
	 				pIOCfgTag = new CIOCfgTag();
	 				m_pIOUnitCfg->AddIOCfgTag(strTagAddr, pIOCfgTag);
	 				pIOCfgTag->AddFieldPoint(pFieldPoint);
	 				//!获取标签对应的描述信息(设备状态标签除外)
	 				if (m_pIOUnit && false == m_pIOUnit->IsDevCommFieldPoint(strTagAddr))
	 				{
	 					int nRes = pDrvDbfParser->GetTagDesByTagAddr(pIOCfgTag->GetTagAddr().c_str(), tDataDes);
	 					if (0 == nRes)
	 					{
	 						//!给标签设置UnitType值
	 						pIOCfgTag->SetUnitType(tDataDes.iUnitType);
	 						//!获取UnitType管理对象
	 						pIOCfgUntiType = m_pIOUnitCfg->GetUnitTypeByID(pIOCfgTag->GetUnitType());
	 						if (pIOCfgUntiType != NULL)
	 						{
	 							//!计算标签地址(FinalAddress)
	 							int lFinalAddr = 0;
	 							if (tDataDes.tBol.byType == 1)							//!常量标签
	 							{
	 								lFinalAddr = 0;
	 							}
	 							else if (tDataDes.tBol.byType == 2)						//!字符串标签
	 							{
	 								lFinalAddr = pIOCfgUntiType->GetStrTagCalcAddr();
	 								//!设置FinalAddr
	 								pIOCfgTag->SetFinalAddress(lFinalAddr);
	 								//!添加IOCfgTag
	 								pIOCfgUntiType->AddTag(pIOCfgTag);

	 								nResult = 0;
	 							}
	 							else													
	 							{
	 								bool bCalcTag = true;
	 								//!分解标签地址
	 								if (pIOCfgTag->AnalyzeTagAddr(lTagItemVal, 32) != 0)
	 								{
	 									bCalcTag = false;
	 								}
	 								if (bCalcTag)
	 								{
	 									//!计算标签地址
	 									if (pDrvDbfParser->CalcBolanExpress(tDataDes, lTagItemVal, lFinalAddr) != 0)
	 									{
	 										MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::InitVariableInfo:CalcBolanExpress failed! Device:%s, TagNodeId:%d, TagName:%s !"), GetName().c_str(), nNodeId, pIOCfgTag->GetName().c_str()));
	 										return nResult;
	 									}
	 									//!设置FinalAddr
	 									pIOCfgTag->SetFinalAddress(lFinalAddr);
	 									//!添加IOCfgTag
	 									pIOCfgUntiType->AddTag(pIOCfgTag);

	 									nResult = 0;
	 								}
	 							}
	 						} 
	 						else
	 						{
	 							MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::InitVariableInfo:GetUnitTypeByID failed! Device:%s, TagNodeId:%d, TagName:%s !"), GetName().c_str(), nNodeId, pIOCfgTag->GetName().c_str()));
	 						}
	 					} 
	 					else
	 					{
	 						MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::InitVariableInfo:GetTagDesByTagAddr failed! Device:%s, TagNodeId:%d, TagName:%s !"), GetName().c_str(), nNodeId, pIOCfgTag->GetName().c_str()));
	 						return nResult;
	 					}
	 				}
	 			}
	 			else if (pIOCfgTag)
	 			{
	 				//!相同的标签地址FielPoint直接添加到pIOCfgTag中
	 				pIOCfgTag->AddFieldPoint(pFieldPoint);
	 				nResult = 0;
	 			}
			}
			else
			{
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::InitVariableInfo:Tag's OffSet is NULL!TagNodeId:%d "), nNodeId));
			}
	 	}
	 	return nResult;
	 }

	//!配置更新:设置暂停工作标志
	void CIODeviceBase::SetBlock( bool block )
	{
		m_bIsBlocked = block;
		//!如果设备当前正在模拟也要停止模拟
		if (true == block)
		{
			StopSimulate();
		}
	}

	//!返回设备当前是否暂停工作
	bool CIODeviceBase::IsBlock()
	{
		return m_bIsBlocked;
	}

	//!得到设备配置文件名
	std::string CIODeviceBase::GetDevCfgName()
	{
		std::string strDevCfg = "";
		if (m_pIOUnit)
		{
			std::string uaDevCfg = m_pIOUnit->GetDevCfgFileName();
			if (!uaDevCfg.empty())
			{
				strDevCfg = uaDevCfg;
				strDevCfg += ".ini";
			}
		}
		return strDevCfg;
	}

	 void CIODeviceBase::InitDevConfig( std::string strDevCfgFile )
	 {
	 	if (!strDevCfgFile.empty())
	 	{
	 		//读取设备模拟周期
	 		int nSimPeriod = GetPrivateProfileInt("DEVICE", "SIMPERIOD", 1000, strDevCfgFile.c_str());
	 		if (nSimPeriod > 0)
	 		{
	 			SetDevSimPeriod(nSimPeriod);
	 		}
	 	}
	 }

	 int CIODeviceBase::SetDevSimPeriod( int mInternal )
	 {
		 int nRes = -1;
		 if (mInternal > 0 && m_pDeviceSim)
		 {
			 m_pDeviceSim->SetSimPeriod(mInternal);
			 nRes = 0;
		 }
		 return nRes;
	 }

	 int CIODeviceBase::FireSOEEvent( FieldPointType* pFieldPoint, CKiwiVariant& OldVarint, long nTime )
	 {
		 int nRes = -1;
		 if (pFieldPoint)
		 {
			 //CKiwiVariant newVariant = *(pFieldPoint->value(NULL).value());
			 //if (newVariant != OldVarint)
			 //{
				// if (0 == nTime)
				// {
				//	 nTime = int64(UaDateTime::now());
				// }
				// else if(nTime > 0)
				// {
				//	 UaDateTime uaDataTime;
				//	 ACE_Time_Value  tmValue;
				//	 tmValue.set_msec(nTime);
				//	 ACE_Date_Time dtTime;
				//	 dtTime.update(tmValue);

				//	 OpcUa_CharA pchDate[256];
				//	 snprintf(pchDate, 256, "%04d-%02d-%02dT%02d:%02d:%02d.%03d+08:00", dtTime.year(), dtTime.month(), 
				//		 dtTime.day(), dtTime.hour(), dtTime.minute(), dtTime.second(), dtTime.microsec()/1000);
				//	 string ustrDate(pchDate);
				//	 uaDataTime=UaDateTime::fromString(ustrDate);
				//	 nTime = int64(uaDataTime);
				// }
				// int nNewVal;
				// newVariant.toInt32(nNewVal);
				// nRes = CDBFrameAccess::PostSOE(pFieldPoint, GetDescription(), EXTERNAL_SOE, nNewVal, nTime);
				// if (m_pDriver)
				// {
				//	 CIODriver* pDriver = (CIODriver*)m_pDriver;
				//	 string uaStrMsg(string("FireSOEEvent For Tag:%1, Device:%2, Result:%3").
				//		 arg(pFieldPoint->nodeId().toFullString().toUtf8()).arg(GetName().c_str()).arg(nRes));
				//	 pDriver->WriteLogAnyWay(uaStrMsg.toUtf8());
				// }
			 //}
		 }
		 return nRes;
	 }

	 int CIODeviceBase::SetVal2Hot( Byte byUNIT_TYPE, int lTagAddr, Byte* pbyData, int nDataLen, long nTime, bool bFromLocal )
	 {
	 	int nRes = 0;
	 	if (pbyData && nDataLen > 0 && !m_pIOPort->IsDisable() && !this->IsDisable() && !this->IsBlock())
	 	{
	 		//!当前链路为从则将写值操作转发到链路主设备进行操作
	 		if (!IsHot())
	 		{
	 			//!得到FieldPoint对象
	 			T_mapFieldPoints lstFPoints;
	 			FieldPointType* pFieldPoint = NULL;
	 			T_mapFieldPoints::iterator itFP;
	 			OBJTAG tOBJTag;
	 			tOBJTag.byUnitType = byUNIT_TYPE;
	 			tOBJTag.lTagAddr = lTagAddr;
	 			MAP_OBJTAG::iterator it = m_mapObjTag.find(tOBJTag.GetIndexAddr());
	 			if (it != m_mapObjTag.end())
	 			{
	 				CIOCfgTag* pIOCfgTag = it->second;
	 				lstFPoints = pIOCfgTag->GetFieldPoints();
	 				//HySession* pPeerSvrSession = GetEquivallentProxy();
	 				//!将FieldPoint的改值操作转发到另机
	 				for (itFP = lstFPoints.begin(); itFP != lstFPoints.end(); itFP++)
	 				{
	 					pFieldPoint = itFP->second;
	 					if (pFieldPoint)
	 					{
	 						long uNodeId = pFieldPoint->nodeId();
	 						//std::string uCmdParam;
	 						//uCmdParam.setByteString(nDataLen, pbyData);
	 						//nRes = SyncVal2HotDev(pPeerSvrSession, uNodeId, CmdValue_SyncVal2Hot, uCmdParam);
	 					}
	 				}

	 			}
	 		}
	 		else
	 		{
	 			//!如果当前链路为主则直接进行更新值的操作
	 			SetVal(byUNIT_TYPE, lTagAddr, pbyData, nDataLen, nTime, bFromLocal);
	 		}
	 	}
	 	else
	 	{
	 		nRes = -1;

	 	}
	 	return nRes;
	 }


	 int CIODeviceBase::SetVal2Hot(CIOCfgTag* pIOCfgTag, Byte byOnLine)
	 {
	 	if (m_pIOPort->IsDisable() || this->IsDisable()|| this->IsBlock())
	 	{
	 		return -2; //not allow write 
	 	}

	 	if (NULL == pIOCfgTag)
	 	{
	 		return -3; //parameter error
	 	}	

	 	if (pIOCfgTag->GetFieldPoints().size() > 0)
	 	{
	 		T_mapFieldPoints mapFPoints = pIOCfgTag->GetFieldPoints();
	 		Byte byVal = byOnLine;
	 		T_mapFieldPoints::iterator fpIt = mapFPoints.begin();
	 		//HySession* pPeerSvrSession = GetEquivallentProxy();
	 		for (; fpIt != mapFPoints.end(); fpIt++)
	 		{
	 			FieldPointType* pFieldPoint = fpIt->second;
	 			if (pFieldPoint)
	 			{
	 				long uNodeId = pFieldPoint->nodeId();
	 				//std::string uCmdParam;
	 				//uCmdParam.setByteString(sizeof(byOnLine), &byOnLine);
	 				//int rcd = SyncVal2HotDev(pPeerSvrSession, uNodeId, CmdValue_SyncVal2Hot, uCmdParam);
	 				//if (0 != rcd)
	 				//{
	 				//	MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase:: SyncVal2HotDev to UaNodeId[%s] failed."),uNodeId.toFullString().toUtf8()));
	 				//}
	 			}
	 		}
	 	}

	     return 0;
	 }

	 //HySession* CIODeviceBase::GetEquivallentProxy()
	 //{
	 //	HySession* pEquivalentProxy = NULL;
	 //	if (g_pDiagServ != NULL)
	 //	{
	 //		pEquivalentProxy = g_pDiagServ->CreateOtherProxy();
	 //	}
	 //	return pEquivalentProxy;
	 //}

	 //int CIODeviceBase::SyncVal2HotDev( HySession* pPeerSvrSession, UaNodeId uNodeId, Byte cmdVal, UaByteString uCmdParam )
	 //{
	 //	int nRes = -1;
	 //	if (pPeerSvrSession && g_pDiagServ && g_pDiagServ->isConnected())
	 //	{
	 //		ServiceSettings serviceSettings;
	 //		CallIn callRequest;
	 //		CallOut callResult;
	 //		//!调用另机Server节点下SendCmd方法实现命令转发
	 //		callRequest.objectId = UaNodeId(FepId_GlobalMethodObject, 0);
	 //		callRequest.methodId = UaNodeId(FepId_Method_SendCmd, 0);
	 //		callRequest.inputArguments.create(3);
	 //		UaVariant dataVariant;
	 //		dataVariant.setNodeId(uNodeId);
	 //		dataVariant.copyTo(&callRequest.inputArguments[0]);
	 //		dataVariant.setByte(cmdVal);
	 //		dataVariant.copyTo(&callRequest.inputArguments[1]);
	 //		dataVariant.setByteString(uCmdParam, OpcUa_True);
	 //		dataVariant.copyTo(&callRequest.inputArguments[2]);

	 //		UaStatus isSuccess = pPeerSvrSession->call(serviceSettings, callRequest, callResult);
	 //		if (isSuccess.isBad())
	 //		{
	 //			OpcUa_CharA pchLogInfo[DEBUGINFOLEN] = {0};
	 //			snprintf(pchLogInfo, DEBUGINFOLEN, "CIODeviceBase::SyncVal2HotDev failed with status %s, NodeId:%s, DevName:%s\n", 
	 //				isSuccess.toString().toUtf8(), uNodeId.toFullString().toUtf8(), GetName().c_str());
	 //			CIODriver* pDriver = (CIODriver*)m_pDriver;
	 //			pDriver->WriteLogAnyWay(pchLogInfo);
	 //		}
	 //		else
	 //		{
	 //			OpcUa_CharA pchLogInfo[DEBUGINFOLEN] = {0};
	 //			snprintf(pchLogInfo, DEBUGINFOLEN, "CIODeviceBase::SyncVal2HotDev Successful with NodeId:%s, DevName:%s\n", 
	 //				uNodeId.toFullString().toUtf8(), GetName().c_str());
	 //			CIODriver* pDriver = (CIODriver*)m_pDriver;
	 //			pDriver->WriteLogAnyWay(pchLogInfo);
	 //			//!Return Successful
	 //			nRes = 0;
	 //		}

	 //	}
	 //	return nRes;
	 //}

	int CIODeviceBase::ProcesHotSim()
	{
		int nRes = 0;
		 if (IsSimulate())
		 {
		 	Byte bySimMode = GetSimMode();
		 	if (m_pDeviceSim)
		 	{
		 		//!置设备在线
		 		SetOnLine(DEVICE_ONLINE);

		 		//!服务启动后接着模拟
		 		m_pDeviceSim->SetSimMode(bySimMode);
		 		nRes = m_pDeviceSim->Start();
		 	}
		 }
		return nRes;
	}

	Byte CIODeviceBase::GetSimMode()
	{
		Byte bySimMode = 0;
		if (m_pIOUnit)
		{
			//bySimMode = m_pIOUnit->GetSimMode();
		}
		return bySimMode;
	}
	
	int CIODeviceBase::InitMapObjTag()
	{
		OBJTAG tOBJTag;
		int nUnitTypeCount = m_pIOUnitCfg->GetUnitTypeCount();
		for (int i = 0; i < nUnitTypeCount; i++)
		{
			CIOCfgUnitType* pIOCfgUnitType = m_pIOUnitCfg->GetUnitTypeByIndex(i);
			if (pIOCfgUnitType)
			{
				tOBJTag.byUnitType = (Byte)pIOCfgUnitType->GetUnitTypeID();
				for (int j = 0; j < pIOCfgUnitType->GetTagCount(); j++)
				{
					CIOCfgTag* pIOCfgTag = pIOCfgUnitType->GetTagByIndex(j);
					if (pIOCfgTag)
					{
						tOBJTag.lTagAddr = pIOCfgTag->GetFinalAddr();
						//!将FieldPointType指针加入设备对象MAP中
						if (pIOCfgTag->GetFieldPoints().size() > 0)
						{
							m_mapObjTag.insert(MAP_OBJTAG::value_type(tOBJTag.GetIndexAddr(), pIOCfgTag));
						}
						else
						{
							MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::InitMapObjTag: a IOCfgTag's FieldPoints is NULL!")));
							return -1;
						}
					}
				}
			}
		}

		return 0;
	}

	bool CIODeviceBase::AddDevComCfgTag2UnitMap(CIOCfgTag* pIoCfgTag, std::string DecCommName)
	{
		if (NULL == pIoCfgTag)
		{
			return false;
		}

		if (pIoCfgTag->GetFieldPoints().size() < 0)
		{
			return false;
		}

		if (0 == pIoCfgTag->GetFieldPoints().size())
		{
			return true;
		}

		m_strAddMapObjTag.insert(STRMAP_OBJTAG::value_type(DecCommName, pIoCfgTag));
		//将通讯状态点放入容器中，用作刷新，其目的是不在成千上万点个点中进行查找//
		if (NULL != m_pIOUnit)
		{
			T_mapFieldPoints mapFPoints = pIoCfgTag->GetFieldPoints();
			T_mapFieldPoints::iterator itFP = mapFPoints.begin();
			for (; itFP != mapFPoints.end(); itFP++)
			{
				FieldPointType* pFiledPoint = itFP->second;
				if (pFiledPoint)
				{
					m_pIOUnit->m_mapStrAddTag.insert(make_pair(DecCommName,pFiledPoint));
				}
			}				
		}

		return true;
	}

	int CIODeviceBase::InitStrAddMapObjTag()
	{
		int rcd = 0;
		m_strAddMapObjTag.clear();
		
		//添加FEP服务整机通讯状态到m_strAddMapObjTag//
		CIOCfgTag* pIOCfgIsOnlineTag = m_pIOUnitCfg->GetIOCfgTagByAddr(DT_ISONLINE);
		if (NULL != pIOCfgIsOnlineTag)
		{
			AddDevComCfgTag2UnitMap(pIOCfgIsOnlineTag,DT_ISONLINE);
		}

		//添加FEP服务A机通讯状态到m_strAddMapObjTag//
		CIOCfgTag* pIOCfgIsOnlineTagA = m_pIOUnitCfg->GetIOCfgTagByAddr(DT_ISONLINE_A);
		if (NULL != pIOCfgIsOnlineTagA)
		{
			AddDevComCfgTag2UnitMap(pIOCfgIsOnlineTagA, DT_ISONLINE_A);
		}

		//添加FEP服务B机通讯状态到m_strAddMapObjTag，当单机配置时不存在B机//
		CIOCfgTag* pIOCfgIsOnlineTagB = m_pIOUnitCfg->GetIOCfgTagByAddr(DT_ISONLINE_B);
		if (NULL != pIOCfgIsOnlineTagB)
		{
			AddDevComCfgTag2UnitMap(pIOCfgIsOnlineTagB, DT_ISONLINE_B);
		}

		return rcd;
	}
}
