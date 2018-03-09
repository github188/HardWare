/*!
 *	\file	IOUnit.cpp
 *
 *	\brief	设备类
 *	
 *
 *	\author lingling.li
 *
 *	\date	2014年4月14日	16:26
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBPlatformClass/IOUnit.h"
#include "RTDBBaseClasses/IOCmdManagerBase.h"
#include "FEPPlatformClass_inc.h"
#include "utilities/fepcommondef.h"
#include "RTDBCommon/datavalue.h"
#include "RTDBCommon/Debug_Msg.h"
// #include "Common/HotData/HotDataMsgQueue.h"

// Namespace
namespace MACS_SCADA_SUD {

/**  Constructs an instance of the class CIOUnit with all components
*/
CIOUnit::CIOUnit(const long& nId)
	:CFepObjectType(nId)
	, m_pDeviceSim(NULL)
	, m_pIOPort(NULL)
	, m_bSimulate(0)
	, m_bOnline(0)
	, m_iDeviceAddr(-1)
{
    // UaStatus      addStatus;
	
    // if ( s_typeNodesCreated == false )
    // {
    //     createTypes();
    // }

	// m_pIsOnline = NULL;
	// m_pIsSimulate = NULL;
	// m_pSimMode = NULL;
	// m_pIsDisable = NULL;
	// m_pIsExtraRange = NULL;
	// m_pAddr = NULL;
	// m_pIOPort = NULL;
	// m_SwitchStatus = IOPORT_NO_SWITCH;
	// m_pDeviceSim = NULL;
	// m_mapStrAddTag.clear();
	//数据变量列表，可能为IO变量、PV变量或者内部变量

	m_mapDataVariables.clear();

	m_mapStrAddTag.clear();
}

CIOUnit::CIOUnit(const long& nodeId, const std::string& szUnit)
	: CFepObjectType(nodeId)
	, szUnitName(szUnitName)
	, m_pDeviceSim(NULL)
	, m_pIOPort(NULL)
	, m_bSimulate(0)
	, m_bOnline(0)
	, m_iDeviceAddr(-1)
{
	SetNodeID(nodeId);
	//数据变量列表，可能为IO变量、PV变量或者内部变量
	m_mapDataVariables.clear();

	m_mapStrAddTag.clear();

}

CIOUnit::CIOUnit(const long& nodeId, const std::string& szUnit, const std::string& szDevice)
	: CFepObjectType(nodeId)
	, szUnitName(szUnit)
	, m_pDeviceSim(NULL)
	, m_pIOPort(NULL)
	, m_bSimulate(0)
	, m_bOnline(0)
	, m_iDeviceAddr(-1)
{
	SetNodeID(nodeId);
	//数据变量列表，可能为IO变量、PV变量或者内部变量
	m_mapDataVariables.clear();

	m_mapStrAddTag.clear();

}
/** Initialize the object with all member nodes
*/
void CIOUnit::initialize(bool bCreateProp)
{
	// if ( s_typeNodesCreated == false )
    // {
    //     createTypes();
	// }
	m_pDeviceSim = NULL;
	m_pIOPort = NULL;
	m_bSimulate = 0;
	m_bOnline = 0;
	m_iDeviceAddr= -1;
}

/** Destruction
*/
CIOUnit::~CIOUnit()
{
	m_pDeviceSim = NULL;
	m_pIOPort = NULL;

	//数据变量列表，可能为IO变量、PV变量或者内部变量
	std::map<long, CKiwiVariable*>::iterator itor = m_mapDataVariables.begin();
	while(	itor != m_mapDataVariables.end())
	{
		delete itor->second;
	}
	m_mapDataVariables.clear();

	//std::map<std::string, FieldPointType*>::iterator itor2 = m_mapStrAddTag.begin();
	//while(	itor2 != m_mapStrAddTag.end())
	//{
	//	delete itor2->second;

	//	itor2++;
	//}
	m_mapStrAddTag.clear();
}

/** Create the related type nodes
*/
void CIOUnit::createTypes()
{

}

/** Clear the static members of the class
*/
void CIOUnit::clearStaticMembers()
{
}

/** Returns the type definition NodeId for the TemperatureSensorType
*/
long CIOUnit::typeDefinitionId() const
{
    long ret = 0;
    return ret;
}

/** Implementation of the MethodManager interface method beginCall
*/
// UaStatus CIOUnit::beginCall(
// 	MethodManagerCallback* pCallback,
// 	const ServiceContext&  serviceContext,
// 	OpcUa_UInt32           callbackHandle,
// 	MethodHandle*          pMethodHandle,
// 	const CKiwiVariantArray&  inputArguments)
// {
// 	UaStatus            ret;

// 	OpcUa::MethodCallJob* pCallJob = new OpcUa::MethodCallJob;
// 	pCallJob->initialize(this, pCallback, serviceContext, callbackHandle, pMethodHandle, inputArguments);
// 	ret = NodeManagerRoot::CreateRootNodeManager()->pServerManager()->getThreadPool()->addJob(pCallJob);
// 	if ( ret.isBad() )
// 	{
// 		delete pCallJob;
// 	}

// 	return ret;
// }

// /** Synchronous version of the MethodManager method call executed in a worker thread
// */
// UaStatus CIOUnit::call(
// 	const ServiceContext&  serviceContext,
// 	MethodHandle*          pMethodHandle,
// 	const CKiwiVariantArray&  inputArguments,
// 	CKiwiVariantArray&        outputArguments,
// 	UaStatusCodeArray&     inputArgumentResults,
// 	UaDiagnosticInfos&     inputArgumentDiag)
// {
// 	UaStatus            ret;
// 	MethodHandleUaNode* pMethodHandleUaNode = (MethodHandleUaNode*)pMethodHandle;
// 	UaMethod*           pMethod             = NULL;

// 	if (pMethodHandleUaNode)
// 	{
// 		pMethod = pMethodHandleUaNode->pUaMethod();

// 		if (pMethod)
// 		{
// 			if ( (s_pSendCmdMethod != NULL) && (pMethod->nodeId() == s_pSendCmdMethod->nodeId() ) )
// 			{
// 				if ( inputArguments.length() != 2 )
// 				{
// 					ret = OpcUa_BadInvalidArgument;
// 				}
// 				else
// 				{
// 					inputArgumentResults.create(2);

// 					int result = 0;
// 					CKiwiVariant value;
// 					UaStatus tmpRet;

// 					Byte byCmdValue;
// 					value = inputArguments[ 0 ];
// 					tmpRet = value.toByte(byCmdValue);
// 					inputArgumentResults[ 0 ] = tmpRet.statusCode();
// 					if (tmpRet.isNotGood()) {ret = tmpRet;}

// 					std::string bysCmdParam;
// 					value = inputArguments[ 1 ];
// 					tmpRet = value.toByteString(bysCmdParam);
// 					inputArgumentResults[ 1 ] = tmpRet.statusCode();
// 					if (tmpRet.isNotGood()) {ret = tmpRet;}

// 					if ( ret.isGood() )
// 					{
// 						//Add for Bug62048：FEP 代码走查，FEPPlatformClass中IOUnit.cpp 中的call函数中的返回值用uastatus 和 int 混用 below
// 						int tmpRcd = this->sendCmd(serviceContext, byCmdValue, bysCmdParam, result);
// 						if (0 == tmpRcd)
// 						{
// 							ret = 0; 
// 						}
// 						else
// 						{
// 							ret = OpcUa_BadInvalidState;
// 						}
// 						//Add for Bug62048：FEP 代码走查，FEPPlatformClass中IOUnit.cpp 中的call函数中的返回值用uastatus 和 int 混用 above
// 					}
// 					if ( ret.isGood() )
// 					{
// 						outputArguments.create(1);

// 						value.setInt32(result);
// 						outputArguments[ 0 ] = *(OpcUa_Variant*)(const OpcUa_Variant*)value;
// 						value.detach();
// 					}
// 				}
// 			}
// 			else
// 			{
// 				return BaseObjectType::call(serviceContext, pMethodHandle, inputArguments, outputArguments, inputArgumentResults, inputArgumentDiag);
// 			}
// 		}
// 		else
// 		{
// 			assert(false);
// 			ret = OpcUa_BadInvalidArgument;
// 		}
// 	}
// 	else
// 	{
// 		assert(false);
// 		ret = OpcUa_BadInvalidArgument;
// 	}

// 	return ret;
// }


 int CIOUnit::sendCmd(
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
 	tRunMgrCmd.byObjectType = ObjectType_Device;      //实体类型
 	tRunMgrCmd.byCmdValue = byCmdVal;                 //命令值
 	tRunMgrCmd.byCmdParam = bysCmdParam;			  //命令参数

 	result = g_pIOCmdManager->SendIORunMgrCmd(tRunMgrCmd);

 	return result;
 }

void CIOUnit::setIOPort(CIOPort* pIOPort)
{
	m_pIOPort = pIOPort;
}

bool CIOUnit::addTag(CKiwiVariable* pTag)
{
	if (pTag == NULL)
	{
		// assert(false);
		return false;
	}
	if (m_mapDataVariables.find(pTag->nodeId()) != m_mapDataVariables.end())
	{
		//已经存在
		return false;
	}
	m_mapDataVariables[pTag->nodeId()] = pTag;
	//如果为IO变量，需要设置变量的设备
	FieldPointType* pFieldPoint = dynamic_cast<FieldPointType*>(pTag);
	if (pFieldPoint != NULL)
	{
		pFieldPoint->setIOUnit(this);
	}
	return true;
}

bool CIOUnit::DeleteTag(CKiwiVariable* pTag)
{
	if (pTag == NULL)
	{
		// assert(false);
		return false;
	}
	std::map<long, CKiwiVariable*>::iterator it = m_mapDataVariables.find(pTag->nodeId());
	if (it == m_mapDataVariables.end())
	{
		//不存在
		return false;
	}
	m_mapDataVariables.erase(it);
	return true;
}
std::string CIOUnit::GetUnitName(void)
{
	return szUnitName;
}
void CIOUnit::GetAllTags(std::vector<CKiwiVariable*> &vecTags)
{
	std::map<long, CKiwiVariable*>::iterator it = m_mapDataVariables.begin();
	for (; it != m_mapDataVariables.end(); it++)
	{
		vecTags.push_back(it->second);
	}
}

int CIOUnit::GetDevAddr()
{
	return m_iDeviceAddr;
}

void CIOUnit::SetDevAddr(int iDeviceAddr)
{
	m_iDeviceAddr = iDeviceAddr;
}

std::string CIOUnit::GetDevCfgFileName()
{
	return szUnitName;
}

bool CIOUnit::IsOnline()
{
	return (m_bOnline == DEVICE_ONLINE);
}

Byte CIOUnit::GetRunState()
{
	return m_bOnline;
}

bool CIOUnit::IsSimulate()
{
	// assert(m_pIsSimulate != NULL);
	// CKiwiVariant var;
	bool ret = false;
	// if (m_pIsSimulate)
	// {
	// 	UaDataValue dataValue(m_pIsSimulate->value(NULL));
	// 	var = *dataValue.value();
	// 	var.toBool(ret);
	// }
	return ret;
}

//!设置链路的切换状态
void CIOUnit::SetIOPortSwitchStatus(IOPortSwitchStatus flag)
{
	m_SwitchStatus = flag;
}

//!得到链路切换状态
IOPortSwitchStatus CIOUnit::GetIOPortSwitchStatus()
{
	return m_SwitchStatus;
}

void CIOUnit::StartSimulate(Byte simMode)
{
	// if (m_pSimMode && m_pIsSimulate)
	// {
	// 	CKiwiVariant var;
	// 	UaDataValue dataValue;
	// 	var.setByte(simMode);
	// 	dataValue.setValue(var, true, true);
	// 	m_pSimMode->setValue(NULL, dataValue, false);

	// 	var.setBool(true);
	// 	dataValue.setValue(var, true, true);
	// 	m_pIsSimulate->setValue(NULL, dataValue, false);
	// }
}

void CIOUnit::StopSimulate()
{
	// if (m_pIsSimulate)
	// {
	// 	CKiwiVariant var;
	// 	var.setBool(false);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, true, true);
	// 	m_pIsSimulate->setValue(NULL, dataValue, false);

	// 	var.setByte(0);
	// 	dataValue.setValue(var, true, true);
	// 	m_pSimMode->setValue(NULL, dataValue, false);
	// }
}

bool CIOUnit::IsDisable()
{
	// assert(m_pIsDisable != NULL);
	// CKiwiVariant var;
	bool ret = false;
	// if (m_pIsDisable)
	// {
	// 	UaDataValue dataValue(m_pIsDisable->value(NULL));
	// 	var = *dataValue.value();
	// 	var.toBool(ret);
	// }
	return ret;
}

bool CIOUnit::IsForce()
{
	// assert(m_pIOPort);
	return m_pIOPort->IsForce();
}

Byte CIOUnit::PrmOrScnd()
{
	// assert(m_pIOPort);
	return m_pIOPort->GetRealPrmScnd();
}

int CIOUnit::SetRealPrmScnd(Byte byState)
{
	// assert(m_pIOPort);
	return m_pIOPort->SetRealPrmScnd(byState);
}

void CIOUnit::SetOnline(bool bChangeW)
{
	// UaMutexLocker lock(&m_IsOnlineLock);
	// assert(m_pIsOnline != NULL);

	// //!得到设备上一时刻状态
	// Byte byState = GetRunState();

	// //!改变设备所属通道权重值
	// CIOPort* pIOPort = GetIOPort();
	// if (pIOPort && (DEVICE_ONLINE != byState) && bChangeW)
	// {
	// 	int nWeight = pIOPort->GetIOChannelWeight();
	// 	nWeight++;
	// 	MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOUnit::SetOnline Dev:%s Update Port:%s Weight:%d"), this->browseName().toString().toUtf8(), 
	// 		pIOPort->browseName().toString().toUtf8(), nWeight));
	// 	pIOPort->SetIOChannelWeight(nWeight);
	// }

	// if (m_pIsOnline)
	// {
	// 	CKiwiVariant var;
	// 	var.setByte(DEVICE_ONLINE);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, true, true);
	// 	m_pIsOnline->setValue(NULL, dataValue, false);
	// 	//!设置设备下标签在线
	// 	long dateTime = long::now();
	// 	SetTagOnLineState(true, dateTime);
	// }
}

void CIOUnit::SetOffline(bool bChangeW)
{
	// UaMutexLocker lock(&m_IsOnlineLock);
	// assert(m_pIsOnline != NULL);

	// //!得到设备上一时刻状态
	// Byte byState = GetRunState();

	// //!只有在设备上一时刻状态为在线才对设备在线个数-1,
	// CIOPort* pIOPort = GetIOPort();
	// if (pIOPort && (DEVICE_ONLINE == byState) && bChangeW)
	// {
	// 	int nWeight = pIOPort->GetIOChannelWeight();
	// 	if (nWeight > 0)
	// 	{
	// 		nWeight--;
	// 	}
	// 	MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOUnit::SetOffline Dev:%s Update Port:%s Weight:%d"), this->browseName().toString().toUtf8(), 
	// 		pIOPort->browseName().toString().toUtf8(), nWeight));
	// 	pIOPort->SetIOChannelWeight(nWeight);
	// }

	// //!在线离线标识置为离线
	// if (m_pIsOnline)
	// {
	// 	CKiwiVariant var;
	// 	var.setByte(DEVICE_OFFLINE);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, true, true);
	// 	m_pIsOnline->setValue(NULL, dataValue, false);
	// }

	// //!设置设备下标签离线
	// long dateTime = long::now();
	// SetTagOnLineState(false, dateTime);
}

void CIOUnit::DisableUnit()
{
	// assert(m_pIsDisable != NULL);
	// if (m_pIsDisable)
	// {
	// 	CKiwiVariant var;
	// 	var.setBool(true);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, true, true);
	// 	m_pIsDisable->setValue(NULL, dataValue, false);
	// 	//!只有设备所属链路为主时才更新标签上的状态
	// 	if (true == PrmOrScnd())
	// 	{
	// 		SetTagDisableState(true);
	// 	}

	// 	//!进入热数据
	// 	if(NULL != g_pHotQueue)
	// 	{
	// 		g_pHotQueue->UpdateHot(m_pIsDisable->nodeId(), dataValue);
	// 	}
	// }
}

void CIOUnit::EnableUnit()
{
	// assert(m_pIsDisable != NULL);
	// if (m_pIsDisable)
	// {
	// 	CKiwiVariant var;
	// 	var.setBool(false);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, true, true);
	// 	m_pIsDisable->setValue(NULL, dataValue, false);
	// 	//!只有设备所属链路为主时才更新标签上的状态
	// 	if (true == PrmOrScnd())
	// 	{
	// 		SetTagDisableState(false);
	// 	}

	// 	//!进入热数据
	// 	if(NULL != g_pHotQueue)
	// 	{
	// 		g_pHotQueue->UpdateHot(m_pIsDisable->nodeId(), dataValue);
	// 	}
	// }
}

void CIOUnit::ForceUnit(Byte PrmScnd)
{
	// assert(m_pIOPort);
	 return m_pIOPort->ForceChannel(PrmScnd);
}

void CIOUnit::DeforceUnit()
{
	// assert(m_pIOPort);
	 return m_pIOPort->DeforceChannel();
}

void CIOUnit::SetExtraRange(bool bExtraRange)
{
	// assert(m_pIsExtraRange != NULL);
	// if (m_pIsExtraRange)
	// {
	// 	CKiwiVariant var;
	// 	var.setBool(bExtraRange);
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(var, true, true);
	// 	m_pIsExtraRange->setValue(NULL, dataValue, false);
	// }
}

bool CIOUnit::GetExtraRange()
{
	// assert(m_pIsExtraRange != NULL);
	// CKiwiVariant var;
	bool ret = false;
	// if (m_pIsExtraRange)
	// {
	// 	UaDataValue dataValue(m_pIsExtraRange->value(NULL));
	// 	var = *dataValue.value();
	// 	var.toBool(ret);
	// }
	return ret;
}

CIOPort* CIOUnit::GetIOPort()
{
	return m_pIOPort;
}

CKiwiVariable* CIOUnit::GetDataVariableByNodeId(long strNodeId)
{
	std::map<long, CKiwiVariable*>::iterator it;
	it = m_mapDataVariables.find(strNodeId);
	if (it != m_mapDataVariables.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

unsigned int CIOUnit::GetVariableCount()
{
	return m_mapDataVariables.size();
}

 CKiwiVariable* CIOUnit::GetVariableByIndex(unsigned int iIndex)
{
	//Mod for BUG62146 FEP单元测试 on 20151217 below
	if ((iIndex < 0)||(iIndex >= m_mapDataVariables.size()))
	{
		return NULL;
	}
	
	std::map<long, CKiwiVariable*>::iterator it = m_mapDataVariables.begin();
	unsigned int iIndexTmp = 0;
	while(iIndexTmp < iIndex)
	{
		iIndexTmp++;
		it++;
	}

	return it->second;
	//Mod for BUG62146 FEP单元测试 on 20151217 above
}

 void CIOUnit::SetTagDisableState(bool bDisable)
 {
	 std::map<long, CKiwiVariable*>::iterator it = m_mapDataVariables.begin();
	 for (; it != m_mapDataVariables.end(); it++)
	 {
		 CKiwiVariable* pVar = it->second;
		 if ((pVar != NULL)/* && (pVar->typeDefinitionId() == long(DEF_FieldPointType, FEP_TYPE_NS_INDEX))*/)
		 {
			 FieldPointType* pTag = dynamic_cast<FieldPointType*>(pVar);
			 if (pTag != NULL)
			 {
				 if (bDisable)
				 {
					 pTag->Disable();
				 }
				 else
				 {
					 pTag->cancelDisable();
				 }
			 }
		 }
	  }
 }

 bool CIOUnit::IsDevCommFieldPoint(const std::string &offSet)
 {
	 bool red = false;
	 if (offSet == DT_ISONLINE || offSet == DT_ISONLINE_A|| offSet == DT_ISONLINE_B)
	 {
		 red = true;
	 }
	 return red;
 }

std::string CIOUnit::GetLocalDevCommType()
 {
	//  int iAorB = g_pDeployManager->GetABIdentifier();
	//  if (Identifier_MachineA == iAorB)
	//  {
	// 	 return DT_ISONLINE_A;
	//  }
	//  if (Identifier_MachineB == iAorB)
	//  {
	// 	 return DT_ISONLINE_B;
	//  }
	 return "";
 }

std::string CIOUnit::GetPeerDevCommType()
{
	// int iAorB = g_pDeployManager->GetABIdentifier();
	// if (Identifier_MachineA == iAorB)
	// {
	// 	return DT_ISONLINE_B;
	// }
	// if (Identifier_MachineB == iAorB)
	// {
	// 	return DT_ISONLINE_A;
	// }
	return "";
}

void CIOUnit::SetPeerDevComm(bool isOnLine)
{
	std::string peerDevCommStr = GetPeerDevCommType();
	Byte byOnLine = 0;
	std::map<std::string, FieldPointType*>::iterator it = m_mapStrAddTag.find(peerDevCommStr);
	if (it != m_mapStrAddTag.end())
	{
		FieldPointType* pFiledPoint = it->second;
		if (isOnLine)
		{
			byOnLine = 1;
		}
		else
		{
			byOnLine = 0;
		}
		//pFiledPoint->setValue(&byOnLine, sizeof(byOnLine), 0);
	}
}

 void CIOUnit::SetTagOnLineState(bool isOnLine, long dateTime)
 { 	 
	  if(NULL == this->GetIOPort())
	  {
	 	 return;
	  }
	  std::string uDrvName = this->GetIOPort()->getDrvName();	
	  std::string strDrvName = uDrvName;
	  Byte primMark = PrmOrScnd();
	  std::map<long, CKiwiVariable*>::iterator it = m_mapDataVariables.begin();
	  for (; it != m_mapDataVariables.end(); it++)
	  {
	 	 CKiwiVariable* pVar = it->second;
	 	 if ((pVar != NULL)/* && (pVar->typeDefinitionId() == long(DEF_FieldPointType, FEP_TYPE_NS_INDEX))*/)
	 	 {
	 		 FieldPointType* pTag = dynamic_cast<FieldPointType*>(pVar);
	 		 if (pTag != NULL)
	 		 {
	 			 //!从机数据由主机同步而来，所以主机更新数据从机不更新,DEVCOMM节点也是//
	 			 if (!primMark)
	 			 {
	 				 continue;
	 			 }

	 			 //对于UADriver上采集的FieldPoint不设置在线的时标和状态而是以DataChange的初始值为准
	 			 if (isOnLine)
	 			 {
	 				 if(strDrvName == OPCUADRIVER)
	 				 {
	 					 pTag->setUATagOnline(dateTime);
	 				 }
	 				 else
	 				 {
	 					 pTag->setOnline(dateTime);
	 				 }
	 			 }
	 			 else
	 			 {
	 				 if (strDrvName == OPCUADRIVER)
	 				 {
	 					 pTag->setUATagOffline(dateTime);
	 				 }
	 				 else
	 				 {
	 					 pTag->setOffline(dateTime);
	 				 }
	 			 }
	 		 }
	 	 }
	  }
 }

 bool CIOUnit::IsOffLine()
 {
	//  UaMutexLocker lock(&m_IsOnlineLock);
	//  assert(m_pIsOnline != NULL);
	//  CKiwiVariant var;
	//  Byte ret = 0;
	//  if (m_pIsOnline)
	//  {
	// 	 UaDataValue dataValue(m_pIsOnline->value(NULL));
	// 	 var = *dataValue.value();
	// 	 var.toByte(ret);
	//  }
	//  return (ret == DEVICE_OFFLINE);
	return false;
 }

//ADD for Bug57996：FEP链路主从切换后，标签值能上来，状态码为0  on 20150826 below
//  void CIOUnit::SetIOPortSwitchStatus(IOPortSwitchStatus flag)
//  {
// 	 m_SwitchStatus = flag;
//  }

 void CIOUnit::SetIODeviceSim(CIODeviceSimBase* pDeviceSim)
 {
	 m_pDeviceSim = pDeviceSim;
}
	
 CIODeviceSimBase* CIOUnit::GetIODeviceSim()
 {
	 return m_pDeviceSim;
 }

 int CIOUnit::GetNeedBackUpVar( std::vector<CKiwiVariable*>& vecHotVariable )
 {
	 int nRes = 0;
	//  if (GetIsDisableNode())
	//  {
	// 	 vecHotVariable.push_back(GetIsDisableNode());
	//  }
	 return nRes;
 }

//  Byte CIOUnit::GetSimMode()
//  {
// 	Byte bySimMode = 0;
// 	if (m_pSimMode)
// 	{
// 		UaDataValue valSimMode(m_pSimMode->value(NULL));
// 		CKiwiVariant varSimMode = *(valSimMode.value());
// 		varSimMode.toByte(bySimMode);
// 	}
// 	return bySimMode;
//  }

	std::string CIOUnit::GetStrDevAddr()
	{
		std::string strDevAddr = "";
		// UaDataValue uDataVal;
		// CKiwiVariant uDTVar;
		// if (m_pAddr)
		// {
		// 	uDataVal = m_pAddr->value(NULL);
		// 	uDTVar = *uDataVal.value();
		// 	std::string ustrDevAdrr = uDTVar.toString();
		// 	if (!ustrDevAdrr.isEmpty())
		// 	{
		// 		strDevAddr = ustrDevAdrr.toUtf8();
		// 	}
		// }
		return strDevAddr;
	}
}


