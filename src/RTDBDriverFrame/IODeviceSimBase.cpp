/*!
 *	\file	IODeviceSimBase.cpp
 *
 *	\brief	�豸ģ�����ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��22��	8:20
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

	//!���������๤��
	IODRIVERFRAME_VAR GenericFactory<CIODeviceSimBase> g_DrvSimFactory;

	//!����ģ��������
	std::string CIODeviceSimBase::s_ClassName("CIODeviceSimBase");

	//!��ʱ���ص�����
	void OnSimTimeOut(void* p)
	{
		// CIODeviceSimBase* pDeviceSim = (CIODeviceSimBase*)p;
		// if (NULL == pDeviceSim || NULL == pDeviceSim->GetParent())
		// {
		// 	return;
		// }

		// //!ͨ����ֹ���豸��ֹ���豸������ֱ�ӷ���
		// CIODeviceBase* pDevice = pDeviceSim->GetParent();
		// if (pDevice->GetIOCfgDriver()->IsDisable() || pDevice->IsDisable() || !pDevice->IsHot())
		// {
		// 	return;
		// }

		// //!�õ���ǰ�豸��ģ�ⷽʽ,Ϊ��������нǶȲ����仯
		// if (pDeviceSim->GetSimMode() == SimulateType_Sin)
		// {
		// 	pDeviceSim->nDegree++;
		// 	if (pDeviceSim->nDegree > pDeviceSim->FULLDEG)
		// 	{
		// 		pDeviceSim->nDegree = 0;
		// 	}
		// }

		// //!���豸������ȡ����Ӧ�ı�ǩ�������ģ��
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

	//!���캯��
	CIODeviceSimBase::CIODeviceSimBase()
		// :m_pIOCfgUnit(NULL), m_bySimMode(SimulateType_Sin), m_pDevice(NULL), m_pIOTimer(NULL)
		// ,m_nSimPeriod(1000)
	{
		//!������ʱ��
		// m_pIOTimer = new CIOTimer(OnSimTimeOut, this);
		// nDegree = 0;
	}

	//!��������
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

	//!��ȡ����
	std::string CIODeviceSimBase::GetClassName()
	{
		return s_ClassName;
	}

	//!����ģ�ⷽʽ
	void CIODeviceSimBase::SetSimMode(Byte bySimMode)
	{
		m_bySimMode = bySimMode;
	}

	//!���ģ��
	int CIODeviceSimBase::Random(void* p)
	{
		int nResult = -1;
		// CIOCfgTag* pObjTag = (CIOCfgTag*)p;
		// //!�����ǰ��·����Ϊ����������ģ�����
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

		// 		//!��ʱ��
		// 		OpcUa_Int64 lCurTime;
		// 		ACE_Time_Value val = ACE_OS::gettimeofday();
		// 		lCurTime = (OpcUa_Int64(val.sec()))*1000 + val.usec()/1000;
		// 		UaStatus status =pFieldPoint->setValue(byVal, nDataLen,lCurTime);
		// 		if (status.isGood())
		// 		{
		// 			nResult = 0;
		// 		}
		// 	}
		// 	OpcUa_NodeId_Clear(&dtNodeId);   //!�ͷ���Դ
		// }
		return nResult;
	}

	//!����ģ��
	int CIODeviceSimBase::Increase(void* p)
	{
		int nResult = -1;
		// CIOCfgTag* pIOTag = (CIOCfgTag*)p;
		// //!�����ǰ��·����Ϊ����������ģ�����
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
		// 					//MOD for Bug #58766 DI���ͨ����BOOLean�ͣ���ѡ���Ƿ�ת�������߶Ը�DI�����ڵ��豸������ģ�⣬DI���ͨ��ֵ���ᷢ���仯 on 20150828 below
		// 					if (pFieldPoint->getRawFull() == pFieldPoint->getEngFull())
		// 					{
		// 						bVal = ~bVal;
		// 					}
		// 					//MOD for Bug #58766 DI���ͨ����BOOLean�ͣ���ѡ���Ƿ�ת�������߶Ը�DI�����ڵ��豸������ģ�⣬DI���ͨ��ֵ���ᷢ���仯 on 20150828 above
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
		// 		//!��ʱ��
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

	//!����ģ��
	int CIODeviceSimBase::Sine(void* p)
	{
		int nResult = -1;

		// CIOCfgTag* pIOTag = (CIOCfgTag*)p;
		// //!�����ǰ��·����Ϊ����������ģ�����
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
		// 	//!��ʱ��
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

	//!����ģ��
	int CIODeviceSimBase::Start(void)
	{
		int nResult = -1;
		// if (m_pIOTimer)
		// {
		// 	nResult = m_pIOTimer->SetTimer(m_nSimPeriod);		//!����������ļ���ȡ
		// }
		return nResult;
	}

	//!ֹͣģ��
	int CIODeviceSimBase::Stop(void)
	{
		int nResult = -1;
		// if (m_pIOTimer)
		// {
		// 	nResult = m_pIOTimer->KillTimer();
		// }
		return nResult;
	}

	//!���ñ�ǩ���������ָ�� 
	void CIODeviceSimBase::SetIOCfgUnit(CIOCfgUnit* pIOUnitCfg)
	{
		// if (pIOUnitCfg)
		// {
		// 	m_pIOCfgUnit = pIOUnitCfg;
		// }
	}

	//!��ȡ��ǩ���������ָ��
	CIOCfgUnit* CIODeviceSimBase::GetIOCfgUnit()
	{
		// return m_pIOCfgUnit;
		return NULL;
	}

	//!���ö�Ӧ�豸����ָ��
	void CIODeviceSimBase::SetParent(CIODeviceBase* pParent)
	{
		// if (pParent)
		// {
		// 	m_pDevice = pParent;
		// }
	}

	//!��ȡ��Ӧ�豸����ָ��
	CIODeviceBase* CIODeviceSimBase::GetParent()
	{
		// return m_pDevice;
		return NULL;
	}

	//!��ȡģ������
	Byte CIODeviceSimBase::GetSimMode()
	{
		// return m_bySimMode;
		return NULL;
	}

	//!�õ�����ַ�
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
