/*!
 *	\file	IOPort.cpp
 *
 *	\brief	通道类
 *	
 *
 *	\author lingling.li
 *
 *	\date	2014年4月14日	16:27
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBPlatformClass/IOPort.h"
#include "RTDBPlatformClass/IOUnit.h"
#include "RTDBBaseClasses/IOCmdManagerBase.h"
#include "RTDBBaseClasses/SyncManagerBase.h"
#include "FEPPlatformClass_inc.h"
#include "RTDBCommon/Debug_Msg.h"
#include "RTDBCommon/datavalue.h"

// Namespace
namespace MACS_SCADA_SUD {

/**  Constructs an instance of the class CIOPort with all components
*/
CIOPort::CIOPort(long nNodeID, std::string sDriverName)
	: CFepObjectType(nNodeID)
{
    // UaStatus      addStatus;

    // if ( s_typeNodesCreated == false )
    // {
    //     createTypes();
    // }

	// m_pRealPrmScnd = NULL;
	// m_pIsOK = NULL;
	// m_pIsSimulate = NULL;
	// m_pSimMode = NULL;
	// m_pIsDisable = NULL;
	// m_pIsForce = NULL;
	// m_pForcePrmScnd = NULL;
	// m_pWeight = NULL;
	// m_pPeerWeight = NULL;
	// m_pIsMonitor = NULL;
	// m_pIsRecord = NULL;
	// m_pDrvName = NULL;
	// m_pType = NULL;
	// m_pPort = NULL;
	// m_pBaudRate = NULL;
	// m_pDataBits = NULL;
	// m_pStopBits = NULL;
	// m_pParity = NULL;
	// m_pDefaultPrmScnd = NULL;
	// m_pPortAccessMode = NULL;
	// m_pSwitchAlgName = NULL;
	// m_pOption = NULL;
	// m_pIP = NULL;
	// m_byRealPrmScndBak = 2;
	// m_bIsInPassvie = false;
	// m_PeerPrmScndWeight = 0;
	// m_Connect2PeerStatus = false;
	m_szDrvName = sDriverName;

}

/** Initialize the object with all member nodes
*/
void CIOPort::initialize(bool bCreateProp)
{
}

/** Destruction
*/
CIOPort::~CIOPort()
{
}

/** Create the related type nodes
*/
void CIOPort::createTypes()
{
}

 //发送命令
 int CIOPort::sendCmd(
 	//const ServiceContext& serviceContext,
 	Byte& byCmdVal, 
 	std::string& bysCmdParam, 
 	int& result)
 {
 	//OpcUa_ReferenceParameter(serviceContext);
 	if (g_pIOCmdManager == NULL)
 	{
 		printf("g_pIOCmdManager is NULL!");
 		return -1;
 	}

 	tIORunMgrCmd tRunMgrCmd;
 	tRunMgrCmd.pNode = this;
 	tRunMgrCmd.byObjectType = ObjectType_Channel;     //实体类型
 	tRunMgrCmd.byCmdValue = byCmdVal;                 //命令值
 	tRunMgrCmd.byCmdParam = bysCmdParam;			  //命令参数

 	result = g_pIOCmdManager->SendIORunMgrCmd(tRunMgrCmd);

 	return result;
 }

void CIOPort::setIOPortCfg(const tIOPortCfg& IOPortCfg)
{
	 // Variable Type 链路类型
	m_szType = IOPortCfg.strType;

	m_unPort = IOPortCfg.nPort;
	 // Variable BaudRate 波特率
	m_unBoadRate = IOPortCfg.iBaudRate;
	 // Variable DataBits 数据位
	m_unDataBits = IOPortCfg.iDataBits;
	 // Variable StopBits 停止位
	m_unStopBits = IOPortCfg.iStopBits;
	 // Variable Parity 奇偶校验 0无校验，1偶校验，2奇校验
	m_unParity = IOPortCfg.byParity;
	 // Variable IPA_A A设备的A网IP地址
	m_szIP = IOPortCfg.strIPA_A;
	
}
void CIOPort::getIOPortCfg(tIOPortCfg& IOPortCfg) const
{
	// Variable Type 链路类型
	IOPortCfg.strType = m_szType ;

	IOPortCfg.nPort = m_unPort;
	// Variable BaudRate 波特率
	IOPortCfg.iBaudRate = m_unBoadRate;
	// Variable DataBits 数据位
	IOPortCfg.iDataBits = m_unDataBits;
	// Variable StopBits 停止位
	IOPortCfg.iStopBits = m_unStopBits;
	// Variable Parity 奇偶校验 0无校验，1偶校验，2奇校验
	IOPortCfg.byParity = m_unParity;
	// Variable IPA_A A设备的A网IP地址
	IOPortCfg.strIPA_A = m_szIP;
}

void CIOPort::setDrvName(const std::string& strDrvName)
{
	m_szDrvName = strDrvName;
}

std::string CIOPort::getDrvName() const
{
	return m_szDrvName;
}

bool CIOPort::IsOK()
{
	CKiwiVariant var;
	bool ret = false;
	// if (m_pIsOK != NULL)
	// {
	// 	UaDataValue dataValue(m_pIsOK->value(NULL));
	// 	var = *dataValue.value();
	// 	OpcUa_StatusCode status = var.toBool(ret);
	// 	if (status != OpcUa_Good)
	// 	{
	// 		ret = false;
	// 	}
	// }
	return ret;
}

bool CIOPort::IsSimulate()
{
	CKiwiVariant var;
	bool ret = false;
	// if (m_pIsSimulate != NULL)
	// {
	// 	UaDataValue dataValue(m_pIsSimulate->value(NULL));
	// 	var = *dataValue.value();
	// 	var.toBool(ret);
	// }
	return ret;
}

bool CIOPort::IsDisable()
{
	CKiwiVariant var;
	bool ret = false;
	// if (m_pIsDisable != NULL)
	// {
	// 	UaDataValue dataValue(m_pIsDisable->value(NULL));
	// 	var = *dataValue.value();
	// 	var.toBool(ret);
	// }
	return ret;
}

bool CIOPort::IsForce()
{
	CKiwiVariant var;
	bool ret = false;
	// if (m_pIsForce != NULL)
	// {
	// 	UaDataValue dataValue(m_pIsForce->value(NULL));
	// 	var = *dataValue.value();
	// 	var.toBool(ret);
	// }
	return ret;
}

int CIOPort::SetRealPrmScnd(Byte byState)
{
	//判断当前是否强制主从状态,
	//如果为强制主从状态，则不允许设置实际主从
	ACE_Guard<ACE_Thread_Mutex> guard( m_RealPrmScndLock ); 
	// UaMutexLocker locker(&m_RealPrmScndLock);
	// if (IsForce())
	// {
	// 	return -1;
	// }
	// if (byState != 0 && byState != 1)
	// {
	// 	return -1;
	// }
	// //assert(m_pRealPrmScnd);
	// if (m_pRealPrmScnd != NULL)
	// {
	// 	CKiwiVariant var;
	// 	var.setByte(byState);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pRealPrmScnd->setValue(NULL, dataValue, false);
	// }
	return 0;
}

//add for bug 58002 on 201508018 below
void CIOPort::SetRealPrmScnd()
{
	//恢复实际主从状态//
	Byte targetRealPrmScnd;
	UaDataValue dataValue;
	CKiwiVariant var;


	ACE_Guard<ACE_Thread_Mutex> guard( m_RealPrmScndLock ); 
	 // UaMutexLocker locker(&m_RealPrmScndLock);

	int localWeight = GetIOChannelWeight();
	int peerWeight = GetPeerPrmScndWeight();

	 //判断最新状态//
	if ( localWeight > peerWeight)
	{
		targetRealPrmScnd = MASTER;
	}
	else if (localWeight < peerWeight)
	{
		targetRealPrmScnd = SLAVE;
	}
	else
	{
		targetRealPrmScnd = GetDefaultPrmScnd();
	}

	 //根据实际值将链路状态刷新//
	 var.setByte(targetRealPrmScnd);
	 dataValue.setValue(var, true);
	 //m_pRealPrmScnd->setValue(NULL, dataValue, false);

}
//add for bug 58002 on 201508018 above

Byte CIOPort::GetRealPrmScnd()
{
	//assert(m_pRealPrmScnd);
	CKiwiVariant var;
	Byte ret = 2;
	// if (m_pRealPrmScnd != NULL)
	// {
	// 	UaDataValue dataValue(m_pRealPrmScnd->value(NULL));
	// 	var = *dataValue.value();
	// 	var.toByte(ret);
	// }
	return ret;
}

void CIOPort::SetCommState(bool isOK)
{
	// if (m_pIsOK != NULL)
	// {
	// 	CKiwiVariant var;
	// 	var.setBool(isOK);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pIsOK->setValue(NULL, dataValue, false);
	// }
}

void CIOPort::SimulateChannel(Byte simMode)
{
	// if ((m_pSimMode != NULL) && (m_pIsSimulate != NULL))
	// {
	// 	CKiwiVariant var;
	// 	UaDataValue dataValue;
	// 	var.setByte(simMode);
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pSimMode->setValue(NULL, dataValue, false);

	// 	var.setBool(OpcUa_True);
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pIsSimulate->setValue(NULL, dataValue, false);
	// }
}

void CIOPort::StopSimulate()
{
	// if ((m_pSimMode != NULL) && (m_pIsSimulate != NULL))
	// {
	// 	CKiwiVariant var;
	// 	var.setBool(false);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pIsSimulate->setValue(NULL, dataValue, false);

	// 	var.setByte(0);
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pSimMode->setValue(NULL, dataValue, false);
	// }
}

void CIOPort::DisableChannel()
{
	// if (m_pIsDisable != NULL)
	// {
	// 	CKiwiVariant var;
	// 	var.setBool(OpcUa_True);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pIsDisable->setValue(NULL, dataValue, false);
	// 	SetUnitDisableState(OpcUa_True);
	// 	//!进入热数据
	// 	if(NULL != g_pHotQueue)
	// 	{
	// 		g_pHotQueue->UpdateHot(m_pIsDisable->nodeId(), dataValue);
	// 	}
	// }
}

void CIOPort::EnableChannel()
{
	// if (m_pIsDisable != NULL)
	// {
	// 	CKiwiVariant var;
	// 	var.setBool(false);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pIsDisable->setValue(NULL, dataValue, false);
	// 	SetUnitDisableState(false);
	// 	//!进入热数据
	// 	if(NULL != g_pHotQueue)
	// 	{
	// 		g_pHotQueue->UpdateHot(m_pIsDisable->nodeId(), dataValue);
	// 	}
	// }
}

void CIOPort::ForceChannel(Byte PrmOrScnd)
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_RealPrmScndLock ); 
	// UaMutexLocker locker(&m_RealPrmScndLock);
	// if ((m_pIsForce != NULL) && (m_pForcePrmScnd != NULL) && (!IsForce()))
	// {
	// 	CKiwiVariant var;
	// 	UaDataValue dataValue;
	// 	//由非强制变为强制
	// 	m_byRealPrmScndBak = GetRealPrmScnd();
	// 	var.setBool(OpcUa_True);
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pIsForce->setValue(NULL, dataValue, false);

	// 	var.setByte(PrmOrScnd);
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pForcePrmScnd->setValue(NULL, dataValue, false);

	// 	//设置实际主从
	// 	if ((m_pRealPrmScnd != NULL))
	// 	{
	// 		var.setByte(PrmOrScnd);
	// 		dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 		m_pRealPrmScnd->setValue(NULL, dataValue, false);
	// 	}
	// }
}

void CIOPort::DeforceChannel()
{
	// if ((m_pIsForce != NULL) && (m_pForcePrmScnd != NULL))
	// {
	// 	//判断当前是否处于强制状态
	// 	if (IsForce())
	// 	{
	// 		CKiwiVariant var;
	// 		UaDataValue dataValue;
	// 		//取消强制状态
	// 		var.setBool(false);
	// 		dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 		m_pIsForce->setValue(NULL, dataValue, false);

	// 		//设置强制主从状态为未设置
	// 		var.setByte(Redun_NotSet);
	// 		dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 		m_pForcePrmScnd->setValue(NULL, dataValue, false);

	// 		//恢复实际主从状态
	// 		SetRealPrmScnd();	
	// 	}
	// }
}

int CIOPort::GetIOChannelWeight()
{
	CKiwiVariant var;
	int ret = 0;
	// if (m_pWeight != NULL)
	// {
	// 	UaDataValue dataValue(m_pWeight->value(NULL));
	// 	var = *dataValue.value();
	// 	var.toInt32(ret);
	// }
	return ret;
}

void CIOPort::SetIOChannelWeight(int iWeight)
{
	// if (m_pWeight != NULL)
	// {
	// 	CKiwiVariant var;
	// 	UaDataValue dataValue;
	// 	var.setInt32(iWeight);
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pWeight->setValue(NULL, dataValue, false);
	// }
	// if (g_pSyncManager != NULL)
	// {
	// 	g_pSyncManager->NotifyLWUpdate(OpcUa_True, this);
	// }
	// //!根据链路权重设置链路好坏状态
	// if (iWeight > 0)
	// {
	// 	SetCommState(OpcUa_True);
	// }
	// else
	// {
	// 	SetCommState(false);
	// }
}

void CIOPort::SetRecord(bool bRecord)
{
	// if (m_pIsRecord != NULL)
	// {
	// 	CKiwiVariant var;
	// 	UaDataValue dataValue;
	// 	var.setBool(bRecord);
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pIsRecord->setValue(NULL, dataValue, false);
	// }
}

bool CIOPort::IsRecord()
{
	CKiwiVariant var;
	bool ret = false;
	// if (m_pIsRecord != NULL)
	// {
	// 	UaDataValue dataValue(m_pIsRecord->value(NULL));
	// 	var = *dataValue.value();
	// 	var.toBool(ret);
	// }
	return ret;
}

void CIOPort::SetMonitor(bool bMonitor)
{
	// if (m_pIsMonitor != NULL)
	// {
	// 	CKiwiVariant var;
	// 	UaDataValue dataValue;
	// 	var.setBool(bMonitor);
	// 	dataValue.setValue(var, OpcUa_True, OpcUa_True);
	// 	m_pIsMonitor->setValue(NULL, dataValue, false);
	// }
}

bool CIOPort::IsMonitor()
{
	CKiwiVariant var;
	bool ret = false;
	// if (m_pIsMonitor != NULL)
	// {
	// 	UaDataValue dataValue(m_pIsMonitor->value(NULL));
	// 	var = *dataValue.value();
	// 	var.toBool(ret);
	// }
	return ret;
}

bool CIOPort::IsDouble()
{
	bool ret = false;
	// if (g_pDiagServ)
	// {
	// 	if (OpcUa_RedundancySupport_None != g_pDiagServ->GetRedundancySupport())
	// 	{
	// 		ret = OpcUa_True;
	// 	}
	// }
	return ret;
}

CIOUnit* CIOPort::GetUnitByName(std::string strUnitName)
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_mapIOUnitsLock ); 
	// UaMutexLocker lock(&m_mapIOUnitsLock);
	std::map<std::string, CIOUnit*>::iterator it = m_mapIOUnits.find(strUnitName);
	if (it != m_mapIOUnits.end())
	{
		return it->second;
	}
	return NULL;
}

int CIOPort::GetUnitCount()
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_mapIOUnitsLock ); 
	// UaMutexLocker lock(&m_mapIOUnitsLock);
	return m_mapIOUnits.size();
}

CIOUnit* CIOPort::GetUnitByIndex(int iIndex)
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_mapIOUnitsLock ); 
	// UaMutexLocker lock(&m_mapIOUnitsLock);
	if ((iIndex < 0)||(iIndex >= m_mapIOUnits.size())) //BUG61379 FEP单元测试，FEPPlatFormClass模块中的IOPort.cpp中的函数GetUnitByIndex，索引越界没处理//
	{
		return NULL;
	}
	std::map<std::string, CIOUnit*>::iterator it = m_mapIOUnits.begin();
	it = m_mapIOUnits.begin();
	int iIndexTmp = 0;
	while(iIndexTmp < iIndex)
	{
		iIndexTmp++;
		it++;
	}
	return it->second;
}

int CIOPort::AddUnit(std::string strUnitName, CIOUnit* pUnit)
{
	 if (pUnit == NULL)
	 {
	 	return -1;
	 }

	 ACE_Guard<ACE_Thread_Mutex> guard( m_mapIOUnitsLock ); 
	 // UaMutexLocker lock(&m_mapIOUnitsLock);
	 if (m_mapIOUnits.find(strUnitName) != m_mapIOUnits.end())
	 {
		 MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIONodeManager::AddUnit IOUnit node %s already exist!"), 
			 strUnitName));
	 	return -1;
	 }
	 m_mapIOUnits[strUnitName] = pUnit;
	return 0;
}

int CIOPort::DelUnit(std::string szNode)
{
	if (szNode.empty())
	{
		return -1;
	}
	ACE_Guard<ACE_Thread_Mutex> guard( m_mapIOUnitsLock ); 
	 // UaMutexLocker lock(&m_mapIOUnitsLock);
	 std::map<std::string, CIOUnit*>::iterator it = m_mapIOUnits.find(szNode);
	 if (it == m_mapIOUnits.end())
	 {
	 	return -1;
	 }
	 m_mapIOUnits.erase(it);	
	return 0;
}

void CIOPort::GetTag(std::vector<CKiwiVariable*> &vecTags)
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_mapIOUnitsLock ); 
	// UaMutexLocker lock(&m_mapIOUnitsLock);
	std::map<std::string, CIOUnit*>::iterator it = m_mapIOUnits.begin();
	for (; it != m_mapIOUnits.end(); it++)
	{
		it->second->GetAllTags(vecTags);
	}
}

// OpcUa::BaseDataVariableType* CIOPort::GetWeightNode()
// {
// 	if (m_pWeight != NULL)
// 	{
// 		return m_pWeight;
// 	}
// 	else
// 	{
// 		return NULL;
// 	}
// }

// OpcUa::BaseDataVariableType* CIOPort::GetIsForceNode()
// {
// 	if (m_pIsForce != NULL)
// 	{
// 		return m_pIsForce;
// 	}
// 	else
// 	{
// 		return NULL;
// 	}
// }

// OpcUa::BaseDataVariableType* CIOPort::GetPForveValue()
// {
// 	if (m_pForcePrmScnd)
// 	{
// 		return m_pForcePrmScnd;
// 	}
// 	else
// 	{
// 		return NULL;
// 	}
// }

Byte CIOPort::GetForcePrmScnd()
{
	Byte byRet = 2;
	// if (m_pForcePrmScnd)
	// {
	// 	CKiwiVariant uaVar;
	// 	UaDataValue uaDataVal(m_pForcePrmScnd->value(NULL));
	// 	uaVar = *uaDataVal.value();
	// 	uaVar.toByte(byRet);
	// }
	return byRet;
}

//!恢复Port的默认主从状态
void CIOPort::RecoverDefaultPrmScnd()
{
	// if ((m_pDefaultPrmScnd != NULL) && (m_pRealPrmScnd != NULL))
	// {
	// 	UaDataValue dataValue;
	// 	CKiwiVariant variant;
	// 	dataValue = m_pDefaultPrmScnd->value(NULL);
	// 	variant = *dataValue.value();
	// 	Byte byPrmScnd;
	// 	variant.toByte(byPrmScnd);
	// 	SetRealPrmScnd(byPrmScnd);
	// }
}

void CIOPort::SetUnitDisableState(bool bDisable)
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_mapIOUnitsLock ); 
	// UaMutexLocker lock(&m_mapIOUnitsLock);
	std::map<std::string, CIOUnit*>::iterator it = m_mapIOUnits.begin();
	for (; it != m_mapIOUnits.end(); it++)
	{
		CIOUnit* pUnit = it->second;
		if (pUnit != NULL)
		{
			if (bDisable)
			{
				pUnit->DisableUnit();
			}
			else
			{
				pUnit->EnableUnit();
			}
		}
	}
}

bool CIOPort::isInPassiveState()
{
	return m_bIsInPassvie;
}

void CIOPort::SetPassiveState( bool bState )
{
	m_bIsInPassvie = bState;
}

//被动升主或降从
void CIOPort::PassiveSPrimScnd( Byte state )
{
	m_byRealPrmScndBak = GetRealPrmScnd();
	SetRealPrmScnd(state);
}

//从被动升主或降从中恢复
void CIOPort::RecorvePassiveState()
{
	SetRealPrmScnd(m_byRealPrmScndBak);
}

// OpcUa::BaseDataVariableType* CIOPort::GetIsSimulateNode()
// {
// 	if (m_pIsSimulate)
// 	{
// 		return m_pIsSimulate;
// 	}
// 	else
// 	{
// 		return NULL;
// 	}
// }

// OpcUa::BaseDataVariableType* CIOPort::GetSimulateModeNode()
// {
// 	if (m_pSimMode)
// 	{
// 		return m_pSimMode;
// 	}
// 	else
// 	{
// 		return NULL;
// 	}
// }

// OpcUa::BaseDataVariableType* CIOPort::GetIsDisableNode()
// {
// 	if (m_pIsDisable)
// 	{
// 		return m_pIsDisable;
// 	}
// 	else
// 	{
// 		return NULL;
// 	}
// }

Byte CIOPort::GetDefaultPrmScnd()
{
	CKiwiVariant var;
	Byte ret = 2;
	// if (m_pDefaultPrmScnd)
	// {
	// 	UaDataValue dataValue = m_pDefaultPrmScnd->value(NULL);
	// 	var = *(dataValue.value());
	// 	var.toByte(ret);
	// }
	return ret;
}

void CIOPort::PutPeerPrmScndWeight(int iWeight)
{
	m_PeerPrmScndWeight = iWeight;
}

int CIOPort::GetPeerPrmScndWeight()
{
	return m_PeerPrmScndWeight;
}


void CIOPort::SetIOPortSwitchStatus(IOPortSwitchStatus flag)
{
	//遍历端口下所有数据单元//
	ACE_Guard<ACE_Thread_Mutex> guard( m_mapIOUnitsLock ); 
	// UaMutexLocker lock(&m_mapIOUnitsLock);
	//std::map<std::string, CIOUnit*>::iterator itx = m_mapIOUnits.begin();
	//for (itx; itx != m_mapIOUnits.end(); itx++)
	//{
	//	CIOUnit* pUnit = itx->second;
	//	if (NULL != pUnit)
	//	{
	//		pUnit->SetIOPortSwitchStatus(flag);
	//	}
	//}
}

int CIOPort::GetNeedBackupVar( std::vector<CKiwiVariable*>& vecHotVariable )
{
	int nRes = 0;
	//!是否禁止
	// if (GetIsDisableNode())
	// {
	// 	vecHotVariable.push_back(GetIsDisableNode());
	// }

	 //!处理该链路下设备需要进热数据Varaible
	 std::map<std::string, CIOUnit*>::iterator iter = m_mapIOUnits.begin();
	 while (iter != m_mapIOUnits.end())
	 {
	 	CIOUnit* pIOUnit = iter->second;
	 	if (pIOUnit)
	 	{
	 		nRes = pIOUnit->GetNeedBackUpVar(vecHotVariable);
	 	}
	 	iter++;
	 }

	return nRes;
}

Byte CIOPort::GetSimMode()
{
	Byte nRes = 0;
	// if (m_pSimMode)
	// {
	// 	UaDataValue uvalSimMode = m_pSimMode->value(NULL);
	// 	CKiwiVariant varSimMode = *(uvalSimMode.value());
	// 	varSimMode.toByte(nRes);
	// }
	return nRes;
}

void CIOPort::SetPeerDevComm(bool isOnLine )
{
	std::map<std::string, CIOUnit*>::iterator iter = m_mapIOUnits.begin();
	while (iter != m_mapIOUnits.end())
	{
		CIOUnit* pIOUnit = iter->second;
		if (pIOUnit)
		{
			 pIOUnit->SetPeerDevComm(isOnLine);
		}
		iter++;
	}
}

bool CIOPort::GetConnect2Peer()
{
	return m_Connect2PeerStatus;
}

void CIOPort::SetConnect2Peer(bool flag)

{
	m_Connect2PeerStatus = flag;
}

} 


