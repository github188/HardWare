

#include "ace/Null_Mutex.h"
#include "RTDBNodeManager/IONodeManager.h"
#include "utilities/ConstDefination.h"
#include "RTDBBaseClasses/NodeManagerComposite.h"
#include "RTDBBaseClasses/IOCmdManagerBase.h"
#include "RTDBBaseClasses/IODrvScheduleMgrBase.h"
#include "RTDBCommon/Debug_Msg.h"

namespace MACS_SCADA_SUD {

unsigned short CIONodeManager::s_typenamespaceIndex = FEP_TYPE_NS_INDEX;

CIONodeManager::CIONodeManager(const std::string& sNamespaceUri)
	// :NodeManagerNodeSetXml(sNamespaceUri)
{
}

CIONodeManager::~CIONodeManager(void)
{
}

 void CIONodeManager::objectCreated(const std::string& szModule, const std::string& szDevice, int nType)
 {
	 if ( nType == FepId_ChannelType)
	 {
		//在线添加链路
		MAP_PORT::iterator itor = m_mapPorts.begin();
		while(itor != m_mapPorts.end())
		{
			if(szModule == itor->second->getDrvName())
				return;
			itor++;
		}
		CIOPort* pIOPort = new CIOPort(m_mapPorts.size(), szModule);
		if (pIOPort != NULL)
		{
			AddPort(pIOPort, true);
		}
	 }
	 else if(nType == FepId_DeviceType)
	 {
		 CIOUnit* pIOUnit = new CIOUnit(0, szModule, szDevice);

		 if (pIOUnit != NULL)
		 {
			 CIOPort* pIOPort = GetPort(szModule);

			 //CIOPort* pIOPort = pIOUnit->GetIOPort();
			 if (pIOPort != NULL)
			 {
				 pIOUnit->setIOPort(pIOPort);
					 //在线添加设备
				 pIOPort->AddUnit(szDevice, pIOUnit);
				 //通知驱动调度
				 if (g_pIOScheduleManager != NULL)
				 {
					 if (g_pIOScheduleManager->IsInitFinished())
					 {
						 g_pIOScheduleManager->AddUnit(pIOUnit);
					 }
				 }				
				 //在线删除设备
				 //pIOPort->DelUnit(pIOUnit->nodeId());
				 pIOUnit->SetDeleteFlag(true);

				 //通知驱动调度
				 if (g_pIOScheduleManager != NULL)
				 {
					 if (g_pIOScheduleManager->IsInitFinished())
					 {
						 g_pIOScheduleManager->DeleteUnit(pIOUnit);
					 }
				 }	
			 }
		 }	
	 }
 }

Byte CIONodeManager::afterStartUp()
{
	Byte ret = 0;
	// if(g_pDeployManager)
	// {
	// 	/*当存在数据源不在本地的数据库单元时，增加“FEP”文件夹节点,
	// 	 *并为所有这样的外部数据源创建相应的虚拟链路节点
	// 	 */
	// 	std::string sSite = g_pDeployManager->GetDeploySite();
	// 	//! 获取数据库单元信息
	// 	std::vector<int> vecDBUnit;
	// 	g_pDeployManager->GetDBUnits(vecDBUnit);
	// 	std::set<std::string> setDataSource;
	// 	for (size_t i = 0; i != vecDBUnit.size(); i++)
	// 	{
	// 		DataSource* pSource = g_pDeployManager->GetDBUnitDataSource(
	// 			sSite, vecDBUnit[i]);
	// 		if ((pSource != NULL)&&(!pSource->dbSource.isEmpty()))
	// 		{
	// 			if (pSource->dbSource != sSite)
	// 			{
	// 				setDataSource.insert(pSource->dbSource);
	// 			}
	// 		}
	// 	}
	// 	if (setDataSource.size() > 0)
	// 	{
	// 		UaFolder* pDataFolder = new UaFolder("FEP", long("FEP", IONODEMANAGER_NS_INDEX), "en");
	// 		ret = addNodeAndReference(long(OpcUaId_ObjectsFolder), pDataFolder, OpcUaId_Organizes);
	// 		UA_// assert(ret.isGood());

	// 		std::set<std::string>::iterator it = setDataSource.begin();
	// 		while (it != setDataSource.end())
	// 		{
	// 			CreateUaPort(*it);
	// 			it++;
	// 		}
	// 	}
	// }

	return ret;
}
Byte CIONodeManager::beforeShutDown()
{
	Byte ret = 0;
	return ret;
}

// Byte CIONodeManager::beginWrite(    
// 	OpcUa_Handle      hIOManagerContext,
// 	unsigned int      callbackHandle,
// 	VariableHandle*   pVariableHandle,
// 	OpcUa_WriteValue* pWriteValue)
// {
// 	char sDebug[255];
// 	//modify for BUG60875 在UA视图中手工勾选IsSimulate后，SCADAServer服务不断重启 on 20121205 below
// 	snprintf(sDebug, 255,"IO[%d]::receive beginWrite then check transfer********\n", getNameSpaceIndex());
// 	//modify for BUG60875 在UA视图中手工勾选IsSimulate后，SCADAServer服务不断重启 on 20121205 above
// 	SingletonDbgInfo::instance()->OutputDebugInfo(sDebug);

// 	//首先进行命令转发
// 	if (g_pSyncCmdServ != NULL)
// 	{
// 		bool bRet = g_pSyncCmdServ->doSyncOrTransWriteCmd(hIOManagerContext, callbackHandle, pWriteValue);
// 		if (!bRet)
// 		{
// 			return OpcUa_Bad;
// 		}
// 	}
// 	//modify for BUG60875 在UA视图中手工勾选IsSimulate后，SCADAServer服务不断重启 on 20121205 below
// 	snprintf(sDebug, 255, "IO[%d]::Point receive beginWrite then execute directly========\n", getNameSpaceIndex());
// 	SingletonDbgInfo::instance()->OutputDebugInfo(sDebug);
// 	//modify for BUG60875 在UA视图中手工勾选IsSimulate后，SCADAServer服务不断重启 on 20121205 above

// 	return IOManagerUaNode::beginWrite(hIOManagerContext, callbackHandle, pVariableHandle, pWriteValue);
// }

// Byte CIONodeManager::writeValues(const CKiwiVariableArray &arrCKiwiVariables, const PDataValueArray &arrpDataValues, ByteCodeArray &arrStatusCodes)
// {
// 	Byte ret = 0;
// 	unsigned int i;
// 	unsigned int count = arrCKiwiVariables.length();

// 	// Create result array
// 	arrStatusCodes.create(count);

// 	for ( i=0; i<count; i++ )
// 	{
// 		CKiwiVariable* pVariable = arrCKiwiVariables[i];
// 		//如果变量为标签，通过命令管理器发送命令给驱动调度
// 		if ((pVariable != NULL) && (pVariable->typeDefinitionId() == long(DEF_FieldPointType, 0)))
// 		{
// 			if (g_pIOCmdManager != NULL)
// 			{
// 				//获得标签值
// 				UaDataValue dataValue;
// 				pVariable->getAttributeValue(NULL, OpcUa_Attributes_Value, dataValue);

// 				//将标签值转换成字符串
// 				UaVariant var(*(dataValue.value()));
// 				std::string strValue = var.toString();

// 				tIORemoteCtrl tRemoteCtrlCmd;
// 				tRemoteCtrlCmd.pNode = pVariable;
// 				tRemoteCtrlCmd.bysValue.setByteString(strValue.length(), (OpcUa_Byte*)strValue.toUtf8());

// 				g_pIOCmdManager->WriteDataVariable(tRemoteCtrlCmd);
// 			}
// 		}
// 	}

// 	return ret;
// }

void CIONodeManager::run()
{
	
}

Byte CIONodeManager::CreatePort(std::string sDriverName, tIOPortCfg IOPortCfg)
{
	Byte ret = 0;
	//创建链路节点
	 CIOPort* pPort = new CIOPort(m_mapPorts.size(), sDriverName/*, sDriverName, IONODEMANAGER_NS_INDEX, this*/);

	 //设置链路的属性
	 //tIOPortCfg IOPortCfg;
	 //pPort->getIOPortCfg(IOPortCfg);
	 //IOPortCfg.iPower = 1;
	 pPort->setIOPortCfg(IOPortCfg);
	 //pPort->SetRealPrmScnd(IOPortCfg.byDefaultPrmScnd);

	 //pPort->setDrvName(std::string(OPCUADRIVER.c_str()));

	 	//将链路节点加入链路列表
	 if(!AddPort(pPort))
	 {
		 MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIONodeManager::AddUaPort add IOPort node %s failed!"), 
			sDriverName));
	 }

	return ret;
}

Byte CIONodeManager::CreateUnit(std::string sDriverName, std::string sDeviceName, int iDevAddr)
{
	Byte ret = 0;
	CIOPort* pPort = GetPort(sDriverName);
	if (pPort != NULL)
	{
		//创建设备节点
		std::string sUnitName = sDriverName +":" + sDeviceName;
		long lUnitID = iDevAddr;
		CIOUnit* pUnit = new CIOUnit(lUnitID, sUnitName, sDeviceName);
		if (pPort && pUnit)
		{
			//设置设备地址
			pUnit->SetDevAddr(iDevAddr);
			pUnit->setIOPort(pPort);
			pPort->AddUnit(sUnitName, pUnit);
		}
		else
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIONodeManager::CreateDriver add IOUnit node %s failed!"), 
				sUnitName));
		}	
	}
	else
	{
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIONodeManager::AddUaDevice IOPort node %s is't exist!"), 
			sDriverName));
		ret = -1;
	}
	return ret;
}

long CIONodeManager::AddUnit(long sNodeId, std::string sUnitName, tIOUnitCfg tCfg, long sPort)
{
	return 0;
}

//long AddUaUnit(ServerInfo tCfg, long sPort);

bool CIONodeManager::AddTag(CKiwiVariable* pTag)
{
	if (pTag == NULL)
	{
		// assert(false);
		return false;
	}

	FieldPointType* pFieldPoint = dynamic_cast<FieldPointType*>(pTag);
	if (pFieldPoint == NULL)
	{
		return false;
	}

	std::string strDeviceName = pFieldPoint->getDeviceAddress();
	MAP_PORTID::iterator itor = m_mapPortIDs.find(strDeviceName);
	long DeviceNodeId = itor->second;

	MAP_PORT::iterator itPort = m_mapPorts.find(DeviceNodeId);
	if (itPort != m_mapPorts.end())
	{
		CIOPort *pIOPort = itPort->second;
		CIOUnit* pDevice = pIOPort->GetUnitByName("");
		if (pDevice != NULL)
		{
			return pDevice->addTag(pTag);
		}
	}
	else
	{
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIONodeManager::AddTag device %s isn't exist!"), 
			strDeviceName));
	}
	return false;
}

bool CIONodeManager::DeleteTag(CKiwiVariable* pTag)
{
	if (pTag == NULL)
	{
		// assert(false);
		return false;
	}
	FieldPointType* pFieldPoint = dynamic_cast<FieldPointType*>(pTag);
	if (pFieldPoint == NULL)
	{
		return false;
	}

	// std::string strDeviceName = pFieldPoint->getDeviceAddress();
	// long DeviceNodeId(strDeviceName, getNameSpaceIndex());

	// UaNode* pNode = getNode(DeviceNodeId);
	// if (pNode != NULL)
	// {
	// 	CIOUnit* pDevice = dynamic_cast<CIOUnit*> (pNode);
	// 	if (pDevice != NULL)
	// 	{
	// 		Byte ret = 0;
	// 		ret = deleteUaReference(pDevice, pFieldPoint, long(OpcUaId_HasComponent, 0));
	// 		if (ret.isNotGood())
	// 		{
	// 			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIONodeManager::DeleteTag deleteUaReference %s failed!"), 
	// 				pNode->nodeId().toFullString().toUtf8()));
	// 		}
	// 		return pDevice->DeleteTag(pTag);
	// 	}
	// }
	// else
	// {
	// 	MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIONodeManager::DeleteTag device %s isn't exist!"), 
	// 		DeviceNodeId.toFullString().toUtf8()));
	// }
	return false;
}

bool CIONodeManager::AddTag(std::string sDriverName, std::string sDeviceName, CKiwiVariable* pTag)
{
	if (pTag == NULL)
	{
		// assert(false);
		return false;
	}
	std::string sUnitName = sDriverName + ":" + sDeviceName;

	CIOPort* pPort = GetPort(sDriverName);

	if(!pPort)
		return false;

	CIOUnit* pDevice = pPort->GetUnitByName(sUnitName);

	if (pDevice != NULL)
	{
		return pDevice->addTag(pTag);
	}
	else
	{
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIONodeManager::AddTag device %s isn't exist!"), 
			sDriverName));
	}
	return false;
}

bool CIONodeManager::DeleteTag( std::string sDriverName, std::string sDeviceName, CKiwiVariable* pTag )
{
	if (pTag == NULL)
	{
		// assert(false);
		return false;
	}
	std::string sUnitName = sDriverName + sDeviceName;

	CIOPort* pPort = GetPort(sDriverName);

	if(!pPort)
		return false;

	CIOUnit* pDevice = pPort->GetUnitByName(sUnitName);

	if (pDevice != NULL)
	{	
		return pDevice->DeleteTag(pTag);
	 }
	 else
	 {
	 	MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIONodeManager::DeleteUaTag device %s isn't exist!"), 
	 		sUnitName));
	 }
	return false;
}

void CIONodeManager::GetAllIOPorts(MAP_PORT& mapPorts)
{
	/*!
	 *	获取通道列表，供驱动调度调用.
	 */	
	//对m_mapPorts加锁 lilingling 2015.12.30

	ACE_Guard<ACE_Thread_Mutex> guard( m_mapPortslock ); 
	// UaMutexLocker lock(&m_mapPortslock);
	MAP_PORT::iterator iter = m_mapPorts.begin();

	while ( iter != m_mapPorts.end())
	{
		mapPorts.insert(std::make_pair(iter->first,iter->second)); 
		iter++;
	}
}

/** Static method to get the namespace index for the type namespace managed by this class.
*/
short CIONodeManager::getTypeNamespace()
{
	return s_typenamespaceIndex;
}

//添加通道
bool CIONodeManager::AddPort(CIOPort* pIOPort, bool bOnline)
{
	bool bRet = false;

	if (pIOPort != NULL)
	{
		//对m_mapPorts加锁 lilingling 2015.12.30
		ACE_Guard<ACE_Thread_Mutex> guard( m_mapPortslock ); 
		//UaMutexLocker lock(&m_mapPortslock);
		if (m_mapPorts.find(pIOPort->nodeId()) == m_mapPorts.end())
		{
			m_mapPorts[pIOPort->nodeId()] = pIOPort;
			bRet = true;
		}
		if (bOnline)
		{
			//通知驱动调度
			if (g_pIOScheduleManager != NULL)
			{
				if (g_pIOScheduleManager->IsInitFinished())
				{
					g_pIOScheduleManager->AddDriver(pIOPort);
				}
			}				
		}
	}

	return bRet;
}

//删除通道
bool CIONodeManager::DelPort(const long& portId, bool bOnline)
{
	bool bRet = false;
	//对m_mapPorts加锁 lilingling 2015.12.30
	ACE_Guard<ACE_Thread_Mutex> guard( m_mapPortslock ); 
	// UaMutexLocker lock(&m_mapPortslock);
	std::map<long, CIOPort*>::iterator it;
	it = m_mapPorts.find(portId);
	 if (it != m_mapPorts.end())
	 {
		 if (bOnline)
		 {
			 //通知驱动调度
			 if (g_pIOScheduleManager != NULL)
			 {
				 if (g_pIOScheduleManager->IsInitFinished())
				 {
					 g_pIOScheduleManager->DeleteDriver(it->second);
				 }
			 }
			 it->second->SetDeleteFlag(true);
		 }
		 else
		 {
			 m_mapPorts.erase(it);
		 }
		 bRet = true;
	 }
	return bRet;
}

CIOPort* CIONodeManager::GetPort(std::string sName)
{
	ACE_Guard<ACE_Thread_Mutex> guard( m_mapPortslock ); 

	MAP_PORT::iterator itPort = m_mapPorts.begin();
	while (itPort != m_mapPorts.end())
	{
		if(itPort->second->getDrvName() == sName)
		{
			return itPort->second;
		}
		itPort++;
	}
	return NULL;
}

int CIONodeManager::GetNeedBackupVar( std::vector<CKiwiVariable*>& vecHotVariable )
{
	int nRes = 0;
	MAP_PORT::iterator iter = m_mapPorts.begin();
	while (iter != m_mapPorts.end())
	{
		CIOPort* pIOPort = iter->second;
		if (pIOPort)
		{
			nRes = pIOPort->GetNeedBackupVar(vecHotVariable);
		}
		iter++;
	}
	return nRes;
}

}// End namespace
