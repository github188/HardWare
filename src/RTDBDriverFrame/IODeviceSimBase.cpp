/*!
 *	\file	IODeviceSimBase.cpp
 *
 *	\brief	设备模拟基类实现
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月22日	8:20
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "IODeviceSimBase.h"
#include "RTDBDriverFrame/IODeviceBase.h"
#include "RTDBDriverFrame/IOCfgUnit.h"
#include "RTDBPlatformClass/IOPort.h"

namespace MACS_SCADA_SUD
{
	void OnSimTimeOut(void* p);

	//!定义驱动类工厂
	IODRIVERFRAME_VAR GenericFactory<CIODeviceSimBase> g_DrvSimFactory;

	//!定义模拟类类名
	std::string CIODeviceSimBase::s_ClassName("CIODeviceSimBase");

	//!定时器回调函数
	void OnSimTimeOut(void* p)
	{
		// CIODeviceSimBase* pDeviceSim = (CIODeviceSimBase*)p;
		// if (NULL == pDeviceSim || NULL == pDeviceSim->GetParent())
		// {
		// 	return;
		// }

		// //!通道禁止、设备禁止、设备离线则直接返回
		// CIODeviceBase* pDevice = pDeviceSim->GetParent();
		// if (pDevice->GetIOCfgDriver()->IsDisable() || pDevice->IsDisable() || !pDevice->IsHot())
		// {
		// 	return;
		// }

		// //!得到当前设备的模拟方式,为正玄则进行角度参数变化
		// if (pDeviceSim->GetSimMode() == SimulateType_Sin)
		// {
		// 	pDeviceSim->nDegree++;
		// 	if (pDeviceSim->nDegree > pDeviceSim->FULLDEG)
		// 	{
		// 		pDeviceSim->nDegree = 0;
		// 	}
		// }

		// //!从设备对象中取出相应的标签对象进行模拟
		// MAP_OBJTAG::iterator it = pDeviceSim->GetParent()->GetMapobjTag().begin();
		// while(it != pDeviceSim->GetParent()->GetMapobjTag().end())
		// {
		// 	CIOCfgTag* pObjTag = it->second;
		// 	if (pObjTag)
		// 	{
		// 		switch(pDeviceSim->GetSimMode())
		// 		{
		// 		case SimulateType_Sin:
		// 			pDeviceSim->Sine(pObjTag);
		// 			break;
		// 		case SimulateType_Random:
		// 			pDeviceSim->Random(pObjTag);
		// 			break;
		// 		case SimulateType_Increase:
		// 			pDeviceSim->Increase(pObjTag);
		// 			break;
		// 		case SimulateType_Unknown:
		// 			break;
		// 		default:
		// 			break;
		// 		}
		// 	}
		// 	it++;
		// }

	}

	//!构造函数
	CIODeviceSimBase::CIODeviceSimBase()
		// :m_pIOCfgUnit(NULL), m_bySimMode(SimulateType_Sin), m_pDevice(NULL), m_pIOTimer(NULL)
		// ,m_nSimPeriod(1000)
	{
		//!创建定时器
		// m_pIOTimer = new CIOTimer(OnSimTimeOut, this);
		// nDegree = 0;
	}

	//!析构函数
	CIODeviceSimBase::~CIODeviceSimBase()
	{
		// if (m_pIOTimer)
		// {
		// 	m_pIOTimer->KillTimer();
		// 	ACE_OS::sleep(ACE_Time_Value(1));
		// 	//delete m_pIOTimer;
		// 	//m_pIOTimer = NULL;
		// }
	}

	//!获取类名
	std::string CIODeviceSimBase::GetClassName()
	{
		return s_ClassName;
	}

	//!设置模拟方式
	void CIODeviceSimBase::SetSimMode(Byte bySimMode)
	{
		m_bySimMode = bySimMode;
	}

	//!随机模拟
	int CIODeviceSimBase::Random(void* p)
	{
		int nResult = -1;
		// CIOCfgTag* pObjTag = (CIOCfgTag*)p;
		// //!如果当前链路不是为主并不进行模拟计算
		// if (NULL == pObjTag || !m_pDevice->IsHot())
		// {
		// 	return nResult;
		// }

		// Byte byVal[256] = {0};
		// ACE_OS::memset(byVal, 0, 256);
		// int nDataLen = 0;
		// T_mapFieldPoints lstPoints = pObjTag->GetFieldPoints();
		// T_mapFieldPoints::iterator itPoints;
		// FieldPointType* pFieldPoint = NULL;
		// for (itPoints = lstPoints.begin(); itPoints != lstPoints.end(); itPoints++)
		// {
		// 	pFieldPoint = itPoints->second;
		// 	UaNodeId dataType = pFieldPoint->dataType();
		// 	OpcUa_NodeId dtNodeId;
		// 	dataType.copyTo(&dtNodeId);
		// 	if (OpcUa_IdentifierType_Numeric == dtNodeId.IdentifierType)
		// 	{
		// 		switch(dtNodeId.Identifier.Numeric)
		// 		{
		// 		case OpcUaId_Boolean:
		// 			{
		// 				OpcUa_Variant pValue = *(pFieldPoint->value(NULL).value());
		// 				UaVariant pVaraint(pValue);
		// 				OpcUa_Boolean uaBool;
		// 				byVal[0] = (pVaraint.toBool(uaBool))?0:1;
		// 				nDataLen = 1;
		// 			}
		// 			break;
		// 		case OpcUaId_Int32:
		// 			{
		// 				int nVal = rand();
		// 				memcpy(byVal, &nVal, sizeof(int));
		// 				nDataLen = sizeof(int);
		// 			}
		// 			break;
		// 		case OpcUaId_Byte:
		// 			{
		// 				byVal[0] = rand()%0xFF;
		// 				nDataLen = 1;
		// 			}
		// 			break;
		// 		case OpcUaId_SByte:
		// 			{
		// 				byVal[0] = (OpcUa_SByte)(rand());
		// 				nDataLen = 1;
		// 			}
		// 			break;
		// 		case OpcUaId_Double:
		// 			{
		// 				OpcUa_Double val = ((OpcUa_Double)(rand())) / 100;
		// 				memcpy(byVal, &val, sizeof(OpcUa_Double));
		// 				nDataLen = sizeof(OpcUa_Double);
		// 			}
		// 			break;
		// 		case OpcUaId_Float:
		// 			{
		// 				OpcUa_Float val = ((OpcUa_Float)(rand())) / 100;
		// 				memcpy(byVal, &val, sizeof(OpcUa_Float));
		// 				nDataLen = sizeof(OpcUa_Float);
		// 			}
		// 			break;
		// 		case OpcUaId_Int16:
		// 			{
		// 				OpcUa_Int16 val = (OpcUa_Int16)(rand());
		// 				memcpy(byVal, &val, sizeof(OpcUa_Int16));
		// 				nDataLen = sizeof(OpcUa_Int16);
		// 			}
		// 			break;
		// 		case OpcUaId_Int64:
		// 			{
		// 				OpcUa_Int64 val = (OpcUa_Int64)(rand());
		// 				memcpy(byVal, &val, sizeof(OpcUa_Int64));
		// 				nDataLen = sizeof(OpcUa_Int64);
		// 			}
		// 			break;
		// 		case OpcUaId_UInt16:
		// 			{
		// 				OpcUa_UInt16 val = (OpcUa_UInt16)(rand());
		// 				memcpy(byVal, &val, sizeof(OpcUa_UInt16));
		// 				nDataLen = sizeof(OpcUa_UInt16);
		// 			}
		// 			break;
		// 		case OpcUaId_UInt32:
		// 			{
		// 				OpcUa_UInt32 val = (OpcUa_UInt32)(rand());
		// 				memcpy(byVal, &val, sizeof(OpcUa_UInt32));
		// 				nDataLen = sizeof(OpcUa_UInt32);
		// 			}
		// 			break;
		// 		case OpcUaId_UInt64:
		// 			{
		// 				OpcUa_UInt64 val = (OpcUa_UInt64)(rand());
		// 				memcpy(byVal, &val, sizeof(OpcUa_UInt64));
		// 				nDataLen = sizeof(OpcUa_UInt64);
		// 			}
		// 			break;
		// 		case OpcUaId_String:
		// 			{
		// 				char chStr;
		// 				GetRandString(chStr);
		// 				memcpy(byVal, &chStr, 1);
		// 				nDataLen = 1;
		// 				byVal[1] = 0;
		// 			}
		// 			break;
		// 		default:
		// 			break;
		// 		}

		// 		OpcUa_NodeId_Clear(&dtNodeId);

		// 		//!置时标
		// 		OpcUa_Int64 lCurTime;
		// 		ACE_Time_Value val = ACE_OS::gettimeofday();
		// 		lCurTime = (OpcUa_Int64(val.sec()))*1000 + val.usec()/1000;
		// 		UaStatus status =pFieldPoint->setValue(byVal, nDataLen,lCurTime);
		// 		if (status.isGood())
		// 		{
		// 			nResult = 0;
		// 		}
		// 	}
		// 	OpcUa_NodeId_Clear(&dtNodeId);   //!释放资源
		// }
		return nResult;
	}

	//!递增模拟
	int CIODeviceSimBase::Increase(void* p)
	{
		int nResult = -1;
		// CIOCfgTag* pIOTag = (CIOCfgTag*)p;
		// //!如果当前链路不是为主并不进行模拟计算
		// if (NULL == pIOTag || !m_pDevice->IsHot())
		// {
		// 	return nResult;
		// }
		// Byte byVal[256];
		// ACE_OS::memset(byVal, 0, 256);
		// int nLen = 0;
		// T_mapFieldPoints mapFPs = pIOTag->GetFieldPoints();
		// T_mapFieldPoints::iterator itFP = mapFPs.begin();
		// FieldPointType* pFieldPoint = NULL;
		// UaDataValue dataValue;
		// UaVariant uaVariant;
		// for (; itFP != mapFPs.end(); itFP++)
		// {
		// 	pFieldPoint = itFP->second;
		// 	if (pFieldPoint)
		// 	{
		// 		UaNodeId dataType = pFieldPoint->dataType();
		// 		OpcUa_NodeId dtNodeId;
		// 		dataType.copyTo(&dtNodeId);
		// 		if (OpcUa_IdentifierType_Numeric == dtNodeId.IdentifierType)
		// 		{
		// 			switch(dtNodeId.Identifier.Numeric)
		// 			{
		// 			case OpcUaId_Boolean:
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					OpcUa_Boolean bVal;
		// 					uaVariant.toBool(bVal);
		// 					//MOD for Bug #58766 DI点的通道（BOOLean型）勾选“是否翻转”，在线对该DI点所在的设备开递增模拟，DI点的通道值不会发生变化 on 20150828 below
		// 					if (pFieldPoint->getRawFull() == pFieldPoint->getEngFull())
		// 					{
		// 						bVal = ~bVal;
		// 					}
		// 					//MOD for Bug #58766 DI点的通道（BOOLean型）勾选“是否翻转”，在线对该DI点所在的设备开递增模拟，DI点的通道值不会发生变化 on 20150828 above
		// 					nLen = sizeof(OpcUa_Boolean);
		// 					memcpy(byVal, &bVal, nLen);
		// 				}
		// 				break;
		// 			case OpcUaId_Int16:
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					OpcUa_Int16 nValue;
		// 					uaVariant.toInt16(nValue);
		// 					nValue += 1;
		// 					nLen = sizeof(OpcUa_Int16);
		// 					memcpy(byVal, &nValue, nLen);
		// 				}
		// 				break;
		// 			case OpcUaId_Int32:
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					int nValue;
		// 					uaVariant.toInt32(nValue);
		// 					nValue += 1;
		// 					nLen = sizeof(int);
		// 					memcpy(byVal, &nValue, nLen);
		// 				}
		// 				break;
		// 			case OpcUaId_Int64:
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					OpcUa_Int64 nValue;
		// 					uaVariant.toInt64(nValue);
		// 					nValue += 1;
		// 					nLen = sizeof(OpcUa_Int64);
		// 					memcpy(byVal, &nValue, nLen);
		// 				}
		// 				break;
		// 			case OpcUaId_Byte: 
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					Byte nValue;
		// 					uaVariant.toByte(nValue);
		// 					nValue += 1;
		// 					nLen = sizeof(Byte);
		// 					memcpy(byVal, &nValue, nLen);
		// 				}
		// 				break;
		// 			case OpcUaId_Float:
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					OpcUa_Float nValue;
		// 					uaVariant.toFloat(nValue);
		// 					nValue += 0.1;
		// 					nLen = sizeof(OpcUa_Float);
		// 					memcpy(byVal, &nValue, nLen);
		// 				}
		// 				break;
		// 			case OpcUaId_Double:
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					OpcUa_Double nValue;
		// 					uaVariant.toDouble(nValue);
		// 					nValue += 0.001;
		// 					nLen = sizeof(OpcUa_Double);
		// 					memcpy(byVal, &nValue, nLen);
		// 				}
		// 				break;
		// 			case OpcUaId_UInt16:
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					OpcUa_UInt16 nValue;
		// 					uaVariant.toUInt16(nValue);
		// 					nValue += 1;
		// 					nLen = sizeof(OpcUa_UInt16);
		// 					memcpy(byVal, &nValue, nLen);
		// 				}
		// 				break;
		// 			case OpcUaId_UInt32:
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					OpcUa_UInt32 nValue;
		// 					uaVariant.toUInt32(nValue);
		// 					nValue += 1;
		// 					nLen = sizeof(OpcUa_UInt32);
		// 					memcpy(byVal, &nValue, nLen);
		// 				}
		// 				break;
		// 			case OpcUaId_UInt64:
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					OpcUa_UInt64 nValue;
		// 					uaVariant.toUInt64(nValue);
		// 					nValue += 1;
		// 					nLen = sizeof(OpcUa_UInt64);
		// 					memcpy(byVal, &nValue, nLen);
		// 				}
		// 				break;
		// 			case OpcUaId_SByte:
		// 				{
		// 					dataValue = pFieldPoint->value(NULL);
		// 					uaVariant = *dataValue.value();
		// 					OpcUa_SByte nValue;
		// 					uaVariant.toSByte(nValue);
		// 					nValue += 1;
		// 					nLen = sizeof(OpcUa_SByte);
		// 					memcpy(byVal, &nValue, nLen);
		// 				}
		// 				break;
		// 			default:
		// 				break;
		// 			}
		// 		}
		// 		OpcUa_NodeId_Clear(&dtNodeId);
		// 		//!置时标
		// 		OpcUa_Int64 lCurTime;
		// 		ACE_Time_Value val = ACE_OS::gettimeofday();
		// 		lCurTime = (OpcUa_Int64(val.sec()))*1000 + val.usec()/1000;
		// 		UaStatus statu = pFieldPoint->setValue(byVal, nLen, lCurTime);
		// 		if (statu.isGood())
		// 		{
		// 			nResult = 0;
		// 		}
		// 	}
		// }

		return nResult;
	}

	//!正玄模拟
	int CIODeviceSimBase::Sine(void* p)
	{
		int nResult = -1;

		// CIOCfgTag* pIOTag = (CIOCfgTag*)p;
		// //!如果当前链路不是为主并不进行模拟计算
		// if (NULL == pIOTag || !m_pDevice->IsHot())
		// {
		// 	return nResult;
		// }

		// Byte byVal[256] = {0};
		// OpcUa_UInt32 nLen = 0;
		// T_mapFieldPoints lstPoints = pIOTag->GetFieldPoints();
		// T_mapFieldPoints::iterator itPoints;
		// FieldPointType* pFieldPoint = NULL;
		// OpcUa_UInt32 nSineY = m_pDevice->GetDriver()->GetSineY();
		// for (itPoints = lstPoints.begin(); itPoints != lstPoints.end(); itPoints++)
		// {
		// 	pFieldPoint = itPoints->second;
		// 	UaNodeId dataType = pFieldPoint->dataType();
		// 	OpcUa_NodeId dtNodeId;
		// 	dataType.copyTo(&dtNodeId);

		// 	switch(dtNodeId.Identifier.Numeric)
		// 	{
		// 	case OpcUaId_Float:
		// 		{
		// 			OpcUa_Float fValue = nSineY*sin(nDegree*2*PI/FULLDEG);
		// 			ACE_OS::memcpy(byVal, &fValue, sizeof(OpcUa_Float));
		// 			nLen = sizeof(OpcUa_Float);
		// 		}
		// 		break;
		// 	case OpcUaId_Double:
		// 		{
		// 			OpcUa_Double fValue = nSineY*sin(nDegree*2*PI/FULLDEG);
		// 			ACE_OS::memcpy(byVal, &fValue, sizeof(OpcUa_Double));
		// 			nLen = sizeof(OpcUa_Double);
		// 		}
		// 		break;
		// 	default:
		// 		break;
		// 	}

		// 	OpcUa_NodeId_Clear(&dtNodeId);
		// 	//!置时标
		// 	OpcUa_Int64 lCurTime;
		// 	ACE_Time_Value val = ACE_OS::gettimeofday();
		// 	lCurTime = (OpcUa_Int64(val.sec()))*1000 + val.usec()/1000;
		// 	UaStatus status =pFieldPoint->setValue(byVal, nLen, lCurTime);
		// 	if (status.isGood())
		// 	{
		// 		nResult = 0;
		// 	}
		// }
		return nResult;
	}

	//!启动模拟
	int CIODeviceSimBase::Start(void)
	{
		int nResult = -1;
		// if (m_pIOTimer)
		// {
		// 	nResult = m_pIOTimer->SetTimer(m_nSimPeriod);		//!因根据配置文件读取
		// }
		return nResult;
	}

	//!停止模拟
	int CIODeviceSimBase::Stop(void)
	{
		int nResult = -1;
		// if (m_pIOTimer)
		// {
		// 	nResult = m_pIOTimer->KillTimer();
		// }
		return nResult;
	}

	//!设置标签类别管理对象指针 
	void CIODeviceSimBase::SetIOCfgUnit(CIOCfgUnit* pIOUnitCfg)
	{
		// if (pIOUnitCfg)
		// {
		// 	m_pIOCfgUnit = pIOUnitCfg;
		// }
	}

	//!获取标签类别管理对象指针
	CIOCfgUnit* CIODeviceSimBase::GetIOCfgUnit()
	{
		// return m_pIOCfgUnit;
		return NULL;
	}

	//!设置对应设备对象指针
	void CIODeviceSimBase::SetParent(CIODeviceBase* pParent)
	{
		// if (pParent)
		// {
		// 	m_pDevice = pParent;
		// }
	}

	//!获取对应设备对象指针
	CIODeviceBase* CIODeviceSimBase::GetParent()
	{
		// return m_pDevice;
		return NULL;
	}

	//!获取模拟类型
	Byte CIODeviceSimBase::GetSimMode()
	{
		// return m_bySimMode;
		return NULL;
	}

	//!得到随机字符
	void CIODeviceSimBase::GetRandString(char& chStr)
	{
		// int nData = rand();
		// nData = nData % 9;
		// chStr = 0x41 + nData;
	}

	void CIODeviceSimBase::SetSimPeriod( int nInternal )
	{
		m_nSimPeriod = nInternal;
	}

	
}
