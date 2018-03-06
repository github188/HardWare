/*!
 *	\file	IODeviceBase.cpp
 *
 *	\brief	�豸�����ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��17��	18:39
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
	//!�����豸�๤��
	IODRIVERFRAME_VAR  GenericFactory<CIODeviceBase> g_DrvDeviceFactory;

	//!�����豸��������
	IODRIVERFRAME_VAR std::string CIODeviceBase::s_ClassName("CIODeviceBase");

	//!���캯��
	CIODeviceBase::CIODeviceBase(void)
		:m_pIOUnit(NULL), m_pUnitState(NULL), m_pDeviceSim(NULL),
		m_pIOPort(NULL), m_pIOUnitCfg(NULL), m_nTimerID(0),
		m_pDriver(NULL), m_pPrtcl(NULL), m_byRxTimes(0), m_byTxTimes(0),
		m_bStatusUnit(false), m_bSetQualityFirstFlag(true), m_bIsBlocked(false)
	{
		
	}

	//!��������
	CIODeviceBase::~CIODeviceBase(void)
	{
		//!����Devicesim����
		if (m_pDeviceSim)
		{
			delete m_pDeviceSim;
			m_pDeviceSim = NULL;
		}

		//!����CIOCfgUnit����
		if (m_pIOUnitCfg)
		{
			delete m_pIOUnitCfg;
			m_pIOUnitCfg = NULL;
		}
	}

	//!�õ���������
	std::string CIODeviceBase::GetClassName()
	{
		return s_ClassName;
	}

	//!�豸��ǩ��ʼ��,�����豸�����а���FieldPoint������
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

		//��ȡ�豸������Ϣ
		 std::string strDevCfgFile = CIODriver::GetFEPRunDir() + m_pIOUnitCfg->GetUnitCfgName();
		 InitDevConfig(strDevCfgFile);

		return 0;
	}

	//!�����豸Ϊģ��״̬
	void CIODeviceBase::StartSimulate(Byte bySimulate)
	{
		//!����豸�Ѿ���ʼģ������ֹͣģ��������ģ��
		 if (IsSimulate())
		 {
		 	MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("CIODeviceBase::SetSimulate: Device=%s already simulate,please stop simulate!"), GetName().c_str()));
		 	return;
		 }

		 //!�豸��û����ͣ��������ģ��(��·Ϊ���豸���Կ���ģ�ⶨʱ�����������������)
		 if (IsBlock())
		 {
		 	MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("CIODeviceBase::SetSimulate: Device=%s is not Hot,can not set simulate!"), GetName().c_str()));
		 	return;
		 }

		 //!����ģ��
		 if (GetIODeviceSim())
		 {
		 	GetIODeviceSim()->SetSimMode(bySimulate);
		 	GetIODeviceSim()->Start();
		 	if (m_pIOUnit)
		 	{
		 		m_pIOUnit->StartSimulate(bySimulate);
		 	}

		 	//!���豸����
		 	SetOnLine(DEVICE_ONLINE);
		 }
	}

	//!ֹͣģ��
	void CIODeviceBase::StopSimulate(void)
	{
		//!����豸��ǰû��ģ�⵫�յ�ֹͣģ��������ֱ�ӷ���
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

	//!�豸�Ƿ���ģ��״̬
	bool CIODeviceBase::IsSimulate(void)
	{
		 return m_pIOUnit->IsSimulate();
		return false;
	}

	//!���ñ�ǩֵ
	void CIODeviceBase::SetVal(Byte byUNIT_TYPE, int lTagAddr, Byte* pbyData, int nDataLen, long nTime, bool bFromLocal)
	{
		//!ͨ����ֹ���豸��ֹ���ӻ�����ͣ����ʱ��д��ֵ
		 if (m_pIOPort->IsDisable() || this->IsDisable() || !this->IsHot() || this->IsBlock())
		 {
		 	return;
		 }
		
		 //!��FieldPoint��дֵ
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
		 			//ȡ����ֵ
		 			CKiwiVariant uaVarOld = pFieldPoint->value();
					
		 			//!���ַ�ռ���дֵ
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
		 		 		//ȡ����ֵ
		 		 		CKiwiVariant uaVarOld = pFieldPoint->value();

		 		 		//!���ַ�ռ���д��ֵ
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

	//!���ñ�ǩֵ(��ǩ��ַΪ�ַ���)
	void CIODeviceBase::SetVal(Byte byUNIT_TYPE, std::string sTagAddr, Byte* pbyData, int nDataLen, long nTime, bool bFromLocal)
	{
		
	}

	//!�õ��豸��ַ(���ַ�����ʽ��ʾ)
	std::string CIODeviceBase::GetStringAddr()
	{
		 if (m_pIOUnit)
		 {
		 	char chName[NAME_LENGTH];
		 	int nDevAddr;
		 	nDevAddr = m_pIOUnit->GetDevAddr();
		 	//MOD for BUG62992 FEP ��Ԫ����   DBDriverFrame ģ���е�IODeviceBase.cpp �е�GetStringAddr��������ACE::OS::itoa�ĺ������ʹ������ on 20160203 below
		 	//��������̬ʱ����ʹ�õ�10��������ת��ʱʹ��10����
		 	//std::string strAddr = ACE_OS::itoa(nDevAddr, chName, NAME_LENGTH); 
             std::string strAddr = ACE_OS::itoa(nDevAddr, chName, 10); 
		 	//MOD for BUG62996 FEP ��Ԫ����   DBDriverFrame ģ���е�IODeviceBase.cpp �е�GetStringAddr��������ACE::OS::itoa�ĺ������ʹ������ on 20160203 above
		 	return strAddr;
		 }
		 else
		{
			return "";
		}
	}

	//!�õ��豸��ַ
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



	//!�õ��豸����
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

	//!�豸�Ƿ�Ϊ��
	bool CIODeviceBase::IsHot()
	{
		return m_pIOUnit->PrmOrScnd();
	}

	//!�����豸������
	void CIODeviceBase::SetHot(Byte byHot)
	{
		if (m_pIOUnit)
		{
			m_pIOUnit->SetRealPrmScnd(byHot);
		}
	}

	//!�豸�Ƿ�����
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
		 	//û����̬�豸ͨ�ŵ㣬�˳�//
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
	 		//!������ǰ��·Ϊ��������������ж������ͨѶ״̬����Ϊ0
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

	 	//!��������ͨѶ״̬��DEVCOMM
	 	CIOCfgTag* pDevCommTag = GetDevCommIoCfgTag(fepDevCommStr);			
	 	if (NULL != pDevCommTag)
	 	{
	 		SetDevCommFp4IoCfgTag(pDevCommTag, byOnLine);
	 	}

	 	//!���±���ͨѶ״̬��,�����ò������͵���·Ϊ���豸��ִ����ʹ����֮��ֵ����һ��
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
	
	 //!�����豸��������״̬
	void CIODeviceBase::SetOnLine(Byte byOnLine)
	{
		 if ( NULL == m_pIOUnit)
		 {
		 	return;
		 }

		 //!Ҫ���õ���·״ֻ̬�������߻�����//
		 if (DEVICE_ONLINE != byOnLine && DEVICE_OFFLINE != byOnLine)
		 {
		 	return;
		 }
		
		 //!����״̬û�з����ı������·����״̬�л����д����������κδ���
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
		 		//!�����豸ͨѶ״̬��ǩֵ
		 		//ProcessDevCommValue(byOnLine);
		 	}
		 	else if (m_pIOUnit->IsOnline() && (IOLinkState > IOPORT_NO_SWITCH))
		 	{
		 		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::SetOnLine Dev:%s Online! IOLinkSwitch:%d\n"), this->GetName().c_str(), IOLinkState));
		 		m_pIOUnit->SetOnline(false);
		 		//!�����豸ͨѶ״̬��ǩֵ
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
		 		//!�����豸ͨѶ״̬��ǩֵ
		 		//ProcessDevCommValue(byOnLine);
		 	}
		 	else if (m_pIOUnit->IsOffLine() && (IOLinkState > IOPORT_NO_SWITCH))
		 	{
		 		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::SetOnLine Dev:%s OffLine! IOLinkSwitch:%d\n"), this->GetName().c_str(), IOLinkState));
		 		m_pIOUnit->SetOffline(false);
		 		//!�����豸ͨѶ״̬��ǩֵ
		 		//ProcessDevCommValue(byOnLine);
		 	}
		 }
		 else
		 {
			 return;
		 }

		 Byte newOnLine = m_pIOUnit->GetRunState();
		 //!���޸ĺ������״̬���͵�ʵʱ����
		 if (this->GetDriver()->GetCfgPrtclName() != OPCUADRIVER && oldOnLine != newOnLine )
		 {
		 	unsigned long nDateTime = (unsigned long)(ACE_OS::gettimeofday().sec());
		 	//CDBFrameAccess::FireFEPEvent(m_pIOUnit, byOnlineEvent, (int)byOnLine, nDateTime);			
		 }

		 //!�����̬���豸״̬��ǩ������״̬��ǩֵ//
		 if (m_pIOUnitCfg)
		 {
		 	ProcessDevCommValue(byOnLine);
		 }

		 //!�������Ϊ�豸������·״̬�����˱仯������ĸ��£��ڸ��º���ָ���·�仯״̬λ//
		 if (IOLinkState >IOPORT_NO_SWITCH)
		 {
		 	m_pIOUnit->SetIOPortSwitchStatus(IOPORT_NO_SWITCH);
		 }
	}

	//!�豸�Ƿ�����
	bool CIODeviceBase::IsOffLine()
	{
		bool bRes = false;
		 if (m_pIOUnit && (DEVICE_OFFLINE == m_pIOUnit->GetRunState()))
		 {
		 	bRes = true;
		 }
		return bRes;
	}

	//!�豸�Ƿ��ڳ�ʼ״̬
	bool CIODeviceBase::IsInit()
	{
		bool bRes = false;
		 if (m_pIOUnit && (DEVICE_INIT == m_pIOUnit->GetRunState()))
		 {
		 	bRes = true;
		 }
		return bRes;
	}

	//!�豸�Ƿ񱻽�ֹ
	bool CIODeviceBase::IsDisable()
	{
		 if (m_pIOUnit)
		 {
		 	return m_pIOUnit->IsDisable();
		 }	
		return false;
	}

	//!�����豸��ֹ״̬
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

	//!�����豸�Ƿ�����
	bool CIODeviceBase::IsExtraRange()
	{
		bool result = false;
		 if (m_pIOUnit)
		 {
		 	result = m_pIOUnit->GetExtraRange();
		 }
		return result;
	}

	//!�����豸������״̬
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

	// !�����豸״ָ̬��
	void CIODeviceBase::SetUnitState(tIOScheduleUnitState* pUnitState)
	{
		m_pUnitState = pUnitState;
	}

	// !��ȡ�豸״ָ̬��
	tIOScheduleUnitState* CIODeviceBase::GetUnitState()
	{
		return m_pUnitState;
	}

	// !�����豸�ڵ�ָ��
	void CIODeviceBase::SetIOUnitNode(CIOUnit* pUnit)
	{
		if (pUnit)
		{
			m_pIOUnit = pUnit;
		}
	}

	// !��ȡ�豸�ڵ�ָ��
	CIOUnit* CIODeviceBase::GetIOUnitNode()
	{
		return m_pIOUnit;
	}

	// !��ȡ�豸��ǩ��Ϣ�������
	CIOCfgUnit* CIODeviceBase::GetIOCfgUnit()
	{
		return m_pIOUnitCfg;
	}

	// !�����豸��Ӧ������������Ϣ��CIOPort��Ϣ
	void CIODeviceBase::SetIOCfgDriver(CIOPort* pIOPort)
	{
		if (pIOPort)
		{
			m_pIOPort = pIOPort;
		}
	}

	// !��ȡ�豸��Ӧ������������Ϣ��IOPort��Ϣ
	CIOPort* CIODeviceBase::GetIOCfgDriver()
	{
		return m_pIOPort;
	}

	// !����ģ���豸�Ķ���ָ��
	void CIODeviceBase::SetIODeviceSim(CIODeviceSimBase* pDeviceSim)
	{
		if (pDeviceSim)
		{
			m_pDeviceSim = pDeviceSim;
			//ADD for BUG61029 on 20151116 below 
			if (NULL == m_pIOUnit) 
			{
				return; //Add for BUG62552  FEP��Ԫ����  DBDriverFrame ģ��IODeviceBase.cpp ʹ��ָ����NULL�жϣ����ڱ�������
			}
            m_pIOUnit->SetIODeviceSim(pDeviceSim);
			//ADD for BUG61029 on 20151116 above
		}
	}

	// !�õ��豸��ģ�����ָ��
	CIODeviceSimBase* CIODeviceBase::GetIODeviceSim()
	{
		return m_pDeviceSim;
	}

	//!�˲�UnitType��TagAddr�Ƿ���Ч
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

	//!�õ�FeildPoint MAP��
	MAP_OBJTAG& CIODeviceBase::GetMapobjTag()
	{
		return m_mapObjTag;
	}

	//!������������ָ��
	void CIODeviceBase::SetDriver(CIODriverBase* pDriver)
	{
		if (pDriver)
		{
			m_pDriver = pDriver;
		}
	}

	//!��ȡ��������ָ��
	CIODriverBase* CIODeviceBase::GetDriver()
	{
		return m_pDriver;
	}

	//!����Э�����ָ��
	 void CIODeviceBase::SetPrtcl(CIOPrtclParserBase* pPrtcl)
	 {
	 	if (pPrtcl)
	 	{
	 		m_pPrtcl = pPrtcl;
	 	}
	 }

	//!��ȡЭ�����ָ��
	 CIOPrtclParserBase* CIODeviceBase::GetPrtcl()
	 {
	 	return m_pPrtcl;
	 }

	//!���ó�ʼ��ʶ
	void CIODeviceBase::SetQualityFirstFlag(bool bSet)
	{
		m_bSetQualityFirstFlag = bSet;
	}

	//!�Ƿ����ͨ�����
	bool CIODeviceBase::IsStatusUnit()
	{
		return m_bStatusUnit;
	}

	//!����ͨ����ϱ�ʶ
	void CIODeviceBase::SetStatusUnit(bool bStatusUnit)
	{
		m_bStatusUnit = bStatusUnit;
	}

	//!ͨ��������ȡ��ǩ����ָ��
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

	//!��ʼ��IOUnit�µ����ݱ���(FieldPoint)��Ϣ
	int CIODeviceBase::InitVariableInfo(CIOUnit* pIOUnit)
	{
		int nResult = -1;
		//Add for BUG62547 FEP��Ԫ����DriverFrameģ���IDDeviceBase.cpp�е�InitVariableInfo����û�ж���ν�����Ч�Լ�飬���ҶԳ�Ա�������ж���else���� on 20160106 below 
		if (NULL == pIOUnit)
		{
			return nResult;
		}
		//Add for BUG62547 FEP��Ԫ����DriverFrameģ���IDDeviceBase.cpp�е�InitVariableInfo����û�ж���ν�����Ч�Լ�飬���ҶԳ�Ա�������ж���else���� on 20160106 above 
		if (NULL == m_pIOUnitCfg)
		{
			m_pIOUnitCfg = new CIOCfgUnit(this);
			//!������ǩ����
			m_pIOUnitCfg->CreateUnitTypeList();
		}

		if(m_pIOUnitCfg)
		{
			//!��IOUnit�µ�FieldPoint��Ϣ��ʼ����IODevice
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

	//!�豸�Ƿ���ǿ��״̬
	bool CIODeviceBase::IsForce()
	{
		if (m_pIOUnit)
		{
			return m_pIOUnit->IsForce();
		}
		return false;
	}

	//!ǿ���豸������״̬
	void CIODeviceBase::Force( Byte byPrmScnd )
	{
		if (m_pIOUnit)
		{
			m_pIOUnit->ForceUnit(byPrmScnd);
		}
	}

	//!ȡ���豸ǿ��
	void CIODeviceBase::DeforceUnit()
	{
		if (m_pIOUnit)
		{
			m_pIOUnit->DeforceUnit();
		}
	}

	//!���ø���:������ݱ�����FeildPoint��ǩ��ַ����
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

	//!����FieldPoint����������UnitType�����ǩ��ַ
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
	 				//!��ȡ��ǩ��Ӧ��������Ϣ(�豸״̬��ǩ����)
	 				if (m_pIOUnit && false == m_pIOUnit->IsDevCommFieldPoint(strTagAddr))
	 				{
	 					int nRes = pDrvDbfParser->GetTagDesByTagAddr(pIOCfgTag->GetTagAddr().c_str(), tDataDes);
	 					if (0 == nRes)
	 					{
	 						//!����ǩ����UnitTypeֵ
	 						pIOCfgTag->SetUnitType(tDataDes.iUnitType);
	 						//!��ȡUnitType�������
	 						pIOCfgUntiType = m_pIOUnitCfg->GetUnitTypeByID(pIOCfgTag->GetUnitType());
	 						if (pIOCfgUntiType != NULL)
	 						{
	 							//!�����ǩ��ַ(FinalAddress)
	 							int lFinalAddr = 0;
	 							if (tDataDes.tBol.byType == 1)							//!������ǩ
	 							{
	 								lFinalAddr = 0;
	 							}
	 							else if (tDataDes.tBol.byType == 2)						//!�ַ�����ǩ
	 							{
	 								lFinalAddr = pIOCfgUntiType->GetStrTagCalcAddr();
	 								//!����FinalAddr
	 								pIOCfgTag->SetFinalAddress(lFinalAddr);
	 								//!���IOCfgTag
	 								pIOCfgUntiType->AddTag(pIOCfgTag);

	 								nResult = 0;
	 							}
	 							else													
	 							{
	 								bool bCalcTag = true;
	 								//!�ֽ��ǩ��ַ
	 								if (pIOCfgTag->AnalyzeTagAddr(lTagItemVal, 32) != 0)
	 								{
	 									bCalcTag = false;
	 								}
	 								if (bCalcTag)
	 								{
	 									//!�����ǩ��ַ
	 									if (pDrvDbfParser->CalcBolanExpress(tDataDes, lTagItemVal, lFinalAddr) != 0)
	 									{
	 										MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODeviceBase::InitVariableInfo:CalcBolanExpress failed! Device:%s, TagNodeId:%d, TagName:%s !"), GetName().c_str(), nNodeId, pIOCfgTag->GetName().c_str()));
	 										return nResult;
	 									}
	 									//!����FinalAddr
	 									pIOCfgTag->SetFinalAddress(lFinalAddr);
	 									//!���IOCfgTag
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
	 				//!��ͬ�ı�ǩ��ַFielPointֱ����ӵ�pIOCfgTag��
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

	//!���ø���:������ͣ������־
	void CIODeviceBase::SetBlock( bool block )
	{
		m_bIsBlocked = block;
		//!����豸��ǰ����ģ��ҲҪֹͣģ��
		if (true == block)
		{
			StopSimulate();
		}
	}

	//!�����豸��ǰ�Ƿ���ͣ����
	bool CIODeviceBase::IsBlock()
	{
		return m_bIsBlocked;
	}

	//!�õ��豸�����ļ���
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
	 		//��ȡ�豸ģ������
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
	 		//!��ǰ��·Ϊ����дֵ����ת������·���豸���в���
	 		if (!IsHot())
	 		{
	 			//!�õ�FieldPoint����
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
	 				//!��FieldPoint�ĸ�ֵ����ת�������
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
	 			//!�����ǰ��·Ϊ����ֱ�ӽ��и���ֵ�Ĳ���
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
	 //		//!�������Server�ڵ���SendCmd����ʵ������ת��
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
		 		//!���豸����
		 		SetOnLine(DEVICE_ONLINE);

		 		//!�������������ģ��
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
						//!��FieldPointTypeָ������豸����MAP��
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
		//��ͨѶ״̬����������У�����ˢ�£���Ŀ���ǲ��ڳ�ǧ���������н��в���//
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
		
		//���FEP��������ͨѶ״̬��m_strAddMapObjTag//
		CIOCfgTag* pIOCfgIsOnlineTag = m_pIOUnitCfg->GetIOCfgTagByAddr(DT_ISONLINE);
		if (NULL != pIOCfgIsOnlineTag)
		{
			AddDevComCfgTag2UnitMap(pIOCfgIsOnlineTag,DT_ISONLINE);
		}

		//���FEP����A��ͨѶ״̬��m_strAddMapObjTag//
		CIOCfgTag* pIOCfgIsOnlineTagA = m_pIOUnitCfg->GetIOCfgTagByAddr(DT_ISONLINE_A);
		if (NULL != pIOCfgIsOnlineTagA)
		{
			AddDevComCfgTag2UnitMap(pIOCfgIsOnlineTagA, DT_ISONLINE_A);
		}

		//���FEP����B��ͨѶ״̬��m_strAddMapObjTag������������ʱ������B��//
		CIOCfgTag* pIOCfgIsOnlineTagB = m_pIOUnitCfg->GetIOCfgTagByAddr(DT_ISONLINE_B);
		if (NULL != pIOCfgIsOnlineTagB)
		{
			AddDevComCfgTag2UnitMap(pIOCfgIsOnlineTagB, DT_ISONLINE_B);
		}

		return rcd;
	}
}
