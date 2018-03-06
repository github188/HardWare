#include "ace/Date_Time.h"
#include "RTDBCommon/datavalue.h"
// #include "RTDBCommon/HotDataMsgQueue.h"
#include "RTDBPlatformClass/FieldPointType.h"
#include "RTDBPlatformClass/GeneralConfig.h"
#include "RTDBPlatformClass/IOUnit.h"
#include "RTDBPlatformClass/fep_statusCode.h"

using namespace MACS_SCADA_SUD;

FieldPointType::FieldPointType()	
{
	initialize();
}

void FieldPointType::initialize()
{
	m_fAbsDeadZone = 0.0;
}

FieldPointType::~FieldPointType()
{
	// m_pParent = NULL;
	m_pIOUnit = NULL;
}

void FieldPointType::SetConfigValues(std::map<std::string, CKiwiVariant>& values)
{
}

void FieldPointType::CreateOptionalProperty(std::map<std::string, CKiwiVariant>& values)
{
}

void FieldPointType::createTypes()
{
}

void FieldPointType::setChannelType(ChannelType type)
{
	m_Type = type;
}

ChannelType FieldPointType::getChannelType() const
{
	return m_Type;
}

void FieldPointType::setIOUnit(CIOUnit* pIOUnit)
{
	m_pIOUnit = pIOUnit;
}

CIOUnit* FieldPointType::getIOUnit()
{
	return m_pIOUnit;
}
bool FieldPointType::getDecCommFlag()
{
	std::string offSet = getOffset();
	bool rcd = false;
	if (offSet == DT_ISONLINE || offSet == DT_ISONLINE_A|| offSet == DT_ISONLINE_B)
	{
		rcd = true;
	}
	return rcd;
}

// Byte FieldPointType::setValue(Session* pSession, const UaDataValue& dataValue, bool checkAccessLevel)
// {
// 	// UaMutexLocker lock(&m_statusCodeLock);
// 	Byte ret = OpcUa_Bad;
// 	bool bSetValueFailed = false;
// 	bool bParentOrTypeNotMatch = false;
// 	int iSiteRole = g_pDeployManager->GetSiteRole();
// 	bool bRTDB = false;
// 	//�жϵ�ǰ�����Ƿ��Ѿ������ ������Ҫ�˳�
// 	if(!IsStarted() || IsExit())
// 	{
// 		//����û������ɣ�˵�����ڳ�ʼ����ַ�ռ� ������Ҫ�˳�
// 		ret = BaseDataVariableType::setValue(pSession, dataValue, checkAccessLevel);
// 		std::string sErr(std::string("FieldPointType::setValue the Server is not Started,just update Inchannel value now, ChannelId:%1.\n").arg(this->nodeId().toString().toUtf8()));
// 		SingletonDbgInfo::instance()->OutputDebugInfo(sErr.toUtf8());
// 		LOG_INFO(MOD_FepPlatformClass, sErr.toUtf8(), this);
// 	}
// 	else
// 	{
// 		if ((iSiteRole & Identifier_Role_RTDB) != 0)
// 		{
// 			bRTDB = true;
// 		}
// 		unsigned short iNSIndex = nodeId().namespaceIndex();
// 		if((HY_LEVEL_SLAVE == CSwitchServ::GetCurrentLevel(iNSIndex)))
// 		{
// 			//! ����Ϊ�ӻ�������
// 			ret = BaseDataVariableType::setValue(pSession, dataValue, checkAccessLevel);
// 			int nRes = -1;
// 			if (ret.isGood())
// 			{
// 				nRes = 0;
// 			}
// 			if(ret.isGood())
// 			{
// 				//! ��¼������
// 				if(NULL != g_pHotQueue)
// 					g_pHotQueue->UpdateHot(nodeId(), this->value(pSession));
				
// 				if(m_Type != InChannelType && NULL != m_pParent)
// 				{
// 					m_pParent->CallBack(this);
// 				}
// 				else
// 				{					
// 					bParentOrTypeNotMatch = true;
// 				}
// 			}
// 			else
// 			{
// 				bSetValueFailed = true;
// 			}
// 		}
// 		else if (bRTDB && !g_pDeployManager->IsDBUnitDSLocalSite(nodeId().namespaceIndex()))
// 		{
// 			//����ΪRTDB���Ҹõ������Դ���Ǳ�վ��
// 			ret = BaseDataVariableType::setValue(pSession, dataValue, checkAccessLevel);
// 			int nRes = -1;
// 			if (ret.isGood())
// 			{
// 				nRes = 0;
// 			}
// 			if(ret.isGood())
// 			{
// 				//! ��¼������
// 				if(NULL != g_pHotQueue)
// 					g_pHotQueue->UpdateHot(nodeId(), this->value(pSession));

// 				if(NULL != m_pParent)
// 				{
// 					m_pParent->CallBack(this);
// 				}
// 				else
// 				{
// 					bParentOrTypeNotMatch = true;
// 				}
// 			}
// 			else
// 			{
// 				bSetValueFailed = true;	
// 			}
// 		}
// 		else
// 		{
// 			//��ȡ��ǩ״̬�����force״̬����Ҫ���⴦��
// 			UaDataValue curVal = this->value(pSession);
// 			if ((curVal.statusCode() & FEP_StatusCode_FieldPoint_Force) 
// 				|| (curVal.statusCode() & FEP_StatusCode_FieldPoint_Diable))
// 			{
// 				if (!(dataValue.statusCode() & FEP_StatusCode_FieldPoint_Force) 
// 					|| !(dataValue.statusCode() & FEP_StatusCode_FieldPoint_Diable))
// 				{
// 					ret = BaseDataVariableType::setValue(pSession, dataValue, checkAccessLevel);
// 					int nRes = -1;
// 					if (ret.isGood())
// 					{
// 						nRes = 0;
// 					}
// 					if (ret.isGood())
// 					{
// 						//! ��¼������
// 						if(NULL != g_pHotQueue)
// 							g_pHotQueue->UpdateHot(nodeId(), this->value(pSession));

// 						if(NULL != m_pParent)
// 						{
// 							m_pParent->CallBack(this);
// 						}
// 						else
// 						{
// 							bParentOrTypeNotMatch = true;
// 						}
// 					}
// 					else
// 					{
// 						bSetValueFailed = true;
// 					}
// 				}
// 				else
// 				{
// 					m_realValDuringState = dataValue;
// 					UaDateTime dt = UaDateTime::now();
// 					if ( m_realValDuringState.isServerTimestampSet() == false )
// 					{
// 						m_realValDuringState.setServerTimestamp(dt);
// 					}
// 					if ( m_realValDuringState.isSourceTimestampSet() == false )
// 					{
// 						m_realValDuringState.setSourceTimestamp(dt);
// 					}
// 				}
// 			}
// 			else
// 			{
// 				ret = BaseDataVariableType::setValue(pSession, dataValue, checkAccessLevel);
// 				int nRes = -1;
// 				if (ret.isGood())
// 				{
// 					nRes = 0;
// 				}
// 				if(ret.isGood())
// 				{
// 					//! ��¼������
// 					if(NULL != g_pHotQueue)
// 						g_pHotQueue->UpdateHot(nodeId(), this->value(pSession));

// 					if(NULL != m_pParent)
// 					{
// 						m_pParent->CallBack(this);
// 					}
// 					else
// 					{
// 						bParentOrTypeNotMatch = true;
// 					}
// 				}
// 				else
// 				{
// 					bSetValueFailed = true;
// 				}
// 			}
// 		}
// 	}
// 	if (bSetValueFailed && bRTDB)
// 	{	
// 		std::string sErr(std::string("FieldPointType::SetVal Failed FieldPoint:%1, SetValueFailed the Site has RTDB Role\n").arg(this->nodeId().toString().toUtf8()));
// 		SingletonDbgInfo::instance()->OutputDebugInfo(sErr.toUtf8());
// 		LOG_ERROR(MOD_FepPlatformClass, sErr.toUtf8(), this);
//     }

// 	if (bParentOrTypeNotMatch && bRTDB)
// 	{
// 		std::string sErr(std::string("FieldPointType::SetVal ParentOrTypeNotMatch FieldPoint:%1\n").arg(this->nodeId().toString().toUtf8()));
// 		SingletonDbgInfo::instance()->OutputDebugInfo(sErr.toUtf8());
// 		LOG_INFO(MOD_FepPlatformClass, sErr.toUtf8(), this);
// 	}
// 	return ret;
// }

void FieldPointType::setDeviceAddress(const std::string& DeviceAddress)
{
	CKiwiVariant value;
	value.setString(DeviceAddress);
	// //setChildValue(DEF_FieldPoint_DeviceAddress, value);
}

std::string FieldPointType::getDeviceAddress() const
{
	std::string ret;
	// CKiwiVariant defaultValue = getChildValue(DEF_FieldPoint_DeviceAddress);
	// std::string ret = defaultValue.toString();
	return ret;
}

void FieldPointType::setOffset(const std::string& offset)
{
	CKiwiVariant value;
	value.setString(offset);
	//setChildValue(DEF_FieldPoint_Offset, value);
}

std::string FieldPointType::getOffset() const
{
	std::string ret;
	// CKiwiVariant defaultValue = getChildValue(DEF_FieldPoint_Offset);
	// std::string ret = defaultValue.toString();
	return ret;
}

// NeedConverse field
void FieldPointType::setNeedConverse(bool setvalue)
{
	CKiwiVariant value;
	value.setBool(setvalue);
	//setChildValue(DEF_FieldPoint_NeedConverse, value);
}

bool FieldPointType::getNeedConverse() const
{
	bool ret = false;	
	// CKiwiVariant value = getChildValue(DEF_FieldPoint_NeedConverse);
	// value.toBool(ret);
	return ret;
}

// RangSwtMode field
void FieldPointType::setRangSwtMode(unsigned short setvalue)
{
	CKiwiVariant value;
	value.setUInt16(setvalue);
	//setChildValue(DEF_FieldPoint_RangSwtMode, value);
}

unsigned short FieldPointType::getRangSwtMode() const
{
	unsigned short ret = 0;
	// CKiwiVariant value = getChildValue(DEF_FieldPoint_RangSwtMode);
	// value.toUInt16(ret);
	return ret;
}

void FieldPointType::setRawZero(float rawZero)
{
	// UaFloatArray fArray;
	// CKiwiVariant uValue = getChildValue(DEF_FieldPoint_RawEngRange);
	// uValue.toFloatArray(fArray);
	// fArray[0] = rawZero;
	// uValue.setFloatArray(fArray);	
	// //setChildValue(DEF_FieldPoint_RawEngRange, uValue);
}

float FieldPointType::getRawZero() const
{
	// UaFloatArray fArray;
	// CKiwiVariant uValue = getChildValue(DEF_FieldPoint_RawEngRange);
	// uValue.toFloatArray(fArray);

	// return fArray[0];
	return 0.0;
}

void FieldPointType::setRawFull(float rawFull)
{
	// UaFloatArray fArray;
	// CKiwiVariant uValue = getChildValue(DEF_FieldPoint_RawEngRange);
	// uValue.toFloatArray(fArray);
	// fArray[1] = rawFull;
	// uValue.setFloatArray(fArray);	
	// //setChildValue(DEF_FieldPoint_RawEngRange, uValue);
}

float FieldPointType::getRawFull() const
{
	// UaFloatArray fArray;
	// CKiwiVariant uValue = getChildValue(DEF_FieldPoint_RawEngRange);
	// uValue.toFloatArray(fArray);

	// return fArray[1];
	return 0.0;
}

void FieldPointType::setEngZero(float engZero)
{
	// UaFloatArray fArray;
	// CKiwiVariant uValue = getChildValue(DEF_FieldPoint_RawEngRange);
	// uValue.toFloatArray(fArray);
	// fArray[2] = engZero;
	// uValue.setFloatArray(fArray);	
	// //setChildValue(DEF_FieldPoint_RawEngRange, uValue);
}

float FieldPointType::getEngZero() const
{
	// UaFloatArray fArray;
	// CKiwiVariant uValue = getChildValue(DEF_FieldPoint_RawEngRange);
	// uValue.toFloatArray(fArray);

	// return fArray[2];
	return 0.0;
}

void FieldPointType::setEngFull(float engFull)
{
	// UaFloatArray fArray;
	// CKiwiVariant uValue = getChildValue(DEF_FieldPoint_RawEngRange);
	// uValue.toFloatArray(fArray);
	// fArray[3] = engFull;
	// uValue.setFloatArray(fArray);	
	// //setChildValue(DEF_FieldPoint_RawEngRange, uValue);
}

float FieldPointType::getEngFull() const
{
	// UaFloatArray fArray;
	// CKiwiVariant uValue = getChildValue(DEF_FieldPoint_RawEngRange);
	// uValue.toFloatArray(fArray);

	// return fArray[3];
	return 0.0;
}

// NeedSOE field
void FieldPointType::setNeedSOE(bool setvalue)
{
	// CKiwiVariant value;
	// value.setBool(setvalue);
	// //setChildValue(DEF_FieldPoint_NeedSOE, value);
}

bool FieldPointType::getNeedSOE() const
{
	bool ret = false;	
	// CKiwiVariant value = getChildValue(DEF_FieldPoint_NeedSOE);
	// OpcUa_StatusCode status = value.toBool(ret);
	// if (status != 0)
	// {
	// 	ret = false;
	// }
	return ret;
}

void FieldPointType::setAbsDeadZone(float setvalue)
{
	m_fAbsDeadZone = setvalue;
}

float FieldPointType::getAbsDeadZone()
{
	return m_fAbsDeadZone;
}

void FieldPointType::setValueDes(const int iValue, std::string strDes)
{
	// std::string retText("");
	// CKiwiVariant value = getChildValue(DEF_FieldPoint_Values);
	// UaInt32Array iArray;
	// value.toInt32Array(iArray);
	// int i = 0;
	// //! �ҵ�setvalue��Ӧ�����
	// for (i = 0; i < iArray.length(); i++)
	// {
	// 	if ( iArray[i] == iValue )
	// 	{	
	// 		value = getChildValue(DEF_FieldPoint_ValueDeses);
	// 		std::stringArray strArray;
	// 		value.toStringArray(strArray);
	// 		strDes.copyTo(&strArray[i]);
	// 		value.setStringArray(strArray);
	// 		//setChildValue(DEF_FieldPoint_ValueDeses, value);
	// 		break;
	// 	}
	// }
}
std::string FieldPointType::getValueDes(const int iValue) const
{
	std::string retText("");

	// CKiwiVariant value = getChildValue(DEF_FieldPoint_Values);
	// UaInt32Array iArray;
	// value.toInt32Array(iArray);
	// int i = 0;
	// //! �ҵ�setvalue��Ӧ�����
	// for (i = 0; i < iArray.length(); i++)
	// {
	// 	if ( iValue == iArray[i] )
	// 	{
	// 		value = getChildValue(DEF_FieldPoint_ValueDeses);
	// 		std::stringArray strArray;
	// 		value.toStringArray(strArray);
	// 		retText = strArray[i];
	// 		break;
	// 	}
	// }

	return retText;
}

Byte FieldPointType::setValue(CKiwiVariant* pbyData, int nDataLen, long nTime)
{
	 if (pbyData == NULL || nDataLen <= 0)
	 {
	 	return -1;//OpcUa_BadInvalidState; //mod for ut on 20151217
	 }

	 //UaDataValue curVal = this->value();
	 //if (curVal.statusCode() & FEP_StatusCode_FieldPoint_Force ||
		// curVal.statusCode() & FEP_StatusCode_FieldPoint_Diable)
	 //{
		// return -1;
	 //}

	 Byte ret = 0;
	 CKiwiVariant value;

	// //!�Ƚ��й�������ת��
	// RangeData(pbyData, nDataLen, true);

	// UaNodeId nodeId = dataType();
	// ret = TranslateData(nodeId, pbyData, nDataLen, value);
	// if(ret.isGood())
	// {
	// 	UaDataValue dataValue;
	// 	dataValue.setValue(value, true, false);
	// 	dataValue.setStatusCode(curVal.statusCode());

	// 	//! �趨ʱ���
	// 	if (nTime > 0)
	// 	{
	// 		UaDateTime uaDataTime;
	// 		ACE_Time_Value  tmValue;
	// 		tmValue.set_msec((OpcUa_UInt64)nTime);
	// 		ACE_Date_Time dtTime;
	// 		dtTime.update(tmValue);

	// 		OpcUa_CharA pchDate[256];
	// 		snprintf(pchDate, 256, "%04d-%02d-%02dT%02d:%02d:%02d.%03d+08:00", dtTime.year(), dtTime.month(), 
	// 			dtTime.day(), dtTime.hour(), dtTime.minute(), dtTime.second(), dtTime.microsec()/1000);
	// 		std::string ustrDate(pchDate);
	// 		uaDataTime=UaDateTime::fromString(ustrDate);
	// 		dataValue.setSourceTimestamp((OpcUa_DateTime)uaDataTime);
	// 	}

	// 	//! ��Variable�趨ֵ�ص�
	// 	ret = setValue(NULL, dataValue, false);
	// }
	// else
	// {
	// 	//BUG60108 ���ķ������Ӻ���·���Ӳ�һ�£�ͬԴ����PV����In_Channel��ֵ��ʱ���״̬�붼��һ�£����Ǳ��֣� ���Դ�ӡ on 20151205 
	// 	std::string sErr(std::string("FieldPointType: setValue call TranslateData() process NodeId[%1] return bad.\n").arg(this->nodeId().toString().toUtf8()));
	// 	SingletonDbgInfo::instance()->OutputDebugInfo(sErr.toUtf8());
	// }
	return ret;
}

Byte FieldPointType::setQualityDetail(uint quality)
{
	Byte reStatus = -1;
	// if (FPGood == quality)
	// {
	// 	reStatus = setOnline(UaDateTime::now());
	// }
	// else if (FPBad == quality)
	// {
	// 	reStatus = setOffline(UaDateTime::now());
	// }
	return reStatus;
}

uint FieldPointType::getQualityDetail()
{
	uint nRes = 0;
	// ByteCode statuCode = this->value(NULL).statusCode();
	// nRes = (uint)statuCode.statusCode();
	return nRes;
}

//��ǩǿ��/ȡ��ǿ��
Byte FieldPointType::force(Byte * pbyData, int nDataLen)
{
	// UaMutexLocker lock(&m_statusCodeLock);

	// if (pbyData == NULL)
	// {
	// 	return OpcUa_BadInvalidArgument;
	// }

	Byte ret = 0;
	// CKiwiVariant value;

	// long uaNodeId = dataType();
	// ret = TranslateData(uaNodeId, pbyData, nDataLen, value);

	// if(ret.isGood())
	// {
	// 	UaDataValue dataValue;
	// 	//dataValue = this->value(NULL);
	// 	dataValue.setValue(value, true, false);
	// 	UaDataValue curVal = this->value(NULL);
	// 	if(curVal.statusCode() & FEP_StatusCode_FieldPoint_Diable)
	// 	{
	// 		//��ǰ�Ѿ����ڽ�ֹ״̬����Ҫȡ����ֹ������ǿ��
	// 		curVal.setStatusCode((curVal.statusCode() & ~FEP_StatusCode_FieldPoint_Diable) | FEP_StatusCode_FieldPoint_Force);
	// 	}
	// 	else if(!(curVal.statusCode() & FEP_StatusCode_FieldPoint_Force))
	// 	{
	// 		//�����ǩʵ��ֵ
	// 		m_realValDuringState = this->value(NULL);
	// 	}
	// 	//����Ϊǿ��״̬
	// 	dataValue.setStatusCode(curVal.statusCode() | FEP_StatusCode_FieldPoint_Force);
	// 	//����ʱ���
	// 	UaDateTime timeStamp = UaDateTime::now();
	// 	dataValue.setSourceTimestamp(timeStamp);
	// 	ret = BaseDataVariableType::setValue(NULL, dataValue, false);
	// 	if (ret.isGood() && g_pHotQueue != NULL)
	// 	{
	// 		g_pHotQueue->UpdateHot(nodeId(), this->value(NULL));
	// 	}

	// 	if(NULL != m_pParent)
	// 		m_pParent->CallBack(this);
	// }
	return ret;
}

Byte FieldPointType::cancelForce()
{
	// UaMutexLocker lock(&m_statusCodeLock);

	Byte ret = 0;
	// UaDataValue dataValue;
	// dataValue = this->value(NULL);
	// if (dataValue.statusCode() & FEP_StatusCode_FieldPoint_Force)
	// {
	// 	//����״̬��
	// 	//m_realValDuringState.setStatusCode(dataValue.statusCode() & ~(FEP_StatusCode_FieldPoint_Force));
	// 	ret = BaseDataVariableType::setValue(NULL, m_realValDuringState, false);
	// 	if(ret.isBad())
	// 	{
	// 		dataValue.setStatusCode(dataValue.statusCode() & ~(FEP_StatusCode_FieldPoint_Force));
	// 		ret = BaseDataVariableType::setValue(NULL, dataValue, false);
	// 	}

	// 	if (ret.isGood() && g_pHotQueue != NULL)
	// 	{
	// 		g_pHotQueue->UpdateHot(nodeId(), this->value(NULL));
	// 	}

	// 	if(NULL != m_pParent)
	// 		m_pParent->CallBack(this);
	// }
	// else 
	// {
	// 	ret = OpcUa_BadInvalidState;
	// }

	return ret;
}

//��ǩ��ֹ/ȡ����ֹ
Byte FieldPointType::Disable()
{
	// UaMutexLocker lock(&m_statusCodeLock);

	Byte ret = 0;
	// UaDataValue dataValue;
	// dataValue = this->value(NULL);
	// if (dataValue.statusCode() & FEP_StatusCode_FieldPoint_Diable)
	// {
	// 	return ret;
	// }
	// if (dataValue.statusCode() & FEP_StatusCode_FieldPoint_Force)
	// {
	// 	//��ǰ�Ѿ�����ǿ��״̬����Ҫȡ��ǿ�ƣ����ý�ֹ
	// 	dataValue.setStatusCode((dataValue.statusCode() & ~FEP_StatusCode_FieldPoint_Force)
	// 		| FEP_StatusCode_FieldPoint_Diable);
	// }
	// else
	// {
	// 	//�����ǩʵ��ֵ
	// 	m_realValDuringState = this->value(NULL);
	// 	//���ý�ֹ״̬
	// 	dataValue.setStatusCode(dataValue.statusCode() | FEP_StatusCode_FieldPoint_Diable);
	// }
	// UaDateTime timeStamp = UaDateTime::now();
	// dataValue.setSourceTimestamp(timeStamp);
	// ret = BaseDataVariableType::setValue(NULL, dataValue, false);
	// if (ret.isGood() && g_pHotQueue != NULL)
	// {
	// 	g_pHotQueue->UpdateHot(nodeId(), this->value(NULL));
	// }
	// if(NULL != m_pParent)
	// 	m_pParent->CallBack(this);
	return ret;
}

Byte FieldPointType::cancelDisable()
{
	// UaMutexLocker lock(&m_statusCodeLock);

	Byte ret = 0;
	// UaDataValue dataValue;
	// dataValue = this->value(NULL);
	// if (dataValue.statusCode() & FEP_StatusCode_FieldPoint_Diable)
	// {
	// 	//����״̬��
	// 	m_realValDuringState.setStatusCode(dataValue.statusCode() & ~(FEP_StatusCode_FieldPoint_Diable));
	// 	ret = BaseDataVariableType::setValue(NULL, m_realValDuringState, false);
	// 	if(ret.isBad())
	// 	{
	// 		dataValue.setStatusCode(dataValue.statusCode() & ~(FEP_StatusCode_FieldPoint_Diable));
	// 		ret = BaseDataVariableType::setValue(NULL, dataValue, false);
	// 	}
	// 	if (ret.isGood() && g_pHotQueue != NULL)
	// 	{
	// 		g_pHotQueue->UpdateHot(nodeId(), this->value(NULL));
	// 	}

	// 	if(NULL != m_pParent)
	// 		m_pParent->CallBack(this);
	// }
	// else 
	// {
	// 	ret = OpcUa_BadInvalidState;
	// }

	return ret;
}
//���ñ�־����/����
Byte FieldPointType::setOnline(long dateTime)
{
	// UaMutexLocker lock(&m_statusCodeLock);

	Byte ret = -1;
	// UaDataValue dataValue;
	// dataValue = this->value(NULL);
	
	// OpcUa_StatusCode ByteCode = dataValue.statusCode();
	// if (!(ByteCode & FEP_StatusCode_FieldPoint_Online) || (ByteCode & FEP_StatusCode_FieldPoint_Init))
	// {
	// 	//�����µ�״̬��
	// 	dataValue.setStatusCode((dataValue.statusCode() | FEP_StatusCode_FieldPoint_Online) & ~FEP_StatusCode_FieldPoint_Init);
	// 	dataValue.setSourceTimestamp(dateTime);
	// 	ret = BaseDataVariableType::setValue(NULL, dataValue, false);
	// 	if (ret.isGood())
	// 	{
	// 		std::string ustrMsg(std::string("FeildPointType:: setOnline nodeId[%1] Success.").arg(this->nodeId().toFullString().toUtf8()));
	// 		LOG_INFO(MOD_FepPlatformClass, ustrMsg.toUtf8(), this);
	// 	}
	// 	else
	// 	{
	// 		std::string ustrMsg(std::string("FeildPointType:: setOnline nodeId[%1] Failed.").arg(this->nodeId().toFullString().toUtf8()));
	// 		LOG_ERROR(MOD_FepPlatformClass, ustrMsg.toUtf8(), this);
	// 	}
	// }
	// else
	// {
	// 	//!Already Online
	// 	ret = 0;
	// }

	// //!����������
	// if (ret.isGood() && NULL != g_pHotQueue)
	// {
	// 	g_pHotQueue->UpdateHot(nodeId(), this->value(NULL));
	// }

	// //!����PVֵ
	// bool isCallParent = false;
	// if(ret.isGood() && NULL != m_pParent)
	// {
	// 	m_pParent->CallBack(this);
	// 	isCallParent = true; 
	// }

	return ret;
}
Byte FieldPointType::setOffline(long dateTime)
{
	// UaMutexLocker lock(&m_statusCodeLock);

	Byte ret = -1;
	// UaDataValue dataValue;
	// dataValue = this->value(NULL);
	// OpcUa_StatusCode ByteCode = dataValue.statusCode();
	// if ((ByteCode & FEP_StatusCode_FieldPoint_Online) || (ByteCode & FEP_StatusCode_FieldPoint_Init))
	// {
	// 	//����״̬��
	// 	dataValue.setStatusCode((dataValue.statusCode() & ~(FEP_StatusCode_FieldPoint_Online)) & ~FEP_StatusCode_FieldPoint_Init);
	// 	dataValue.setSourceTimestamp(dateTime);
	// 	ret = BaseDataVariableType::setValue(NULL, dataValue, false);
	// 	if (ret.isGood())
	// 	{
	// 		std::string ustrMsg(std::string("FeildPointType:: setOffline nodeId[%1] Success.").arg(this->nodeId().toFullString().toUtf8()));
	// 		LOG_ERROR(MOD_FepPlatformClass, ustrMsg.toUtf8(), this);
	// 	}
	// 	else
	// 	{
	// 		std::string ustrMsg(std::string("FeildPointType:: setOffline nodeId[%1] Failed.").arg(this->nodeId().toFullString().toUtf8()));
	// 		LOG_ERROR(MOD_FepPlatformClass, ustrMsg.toUtf8(), this);
	// 	}
	// }
	// else
	// {
	// 	//!Already OffLine
	// 	ret = 0;
	// }

	// //!����������
	// if (ret.isGood() && NULL != g_pHotQueue)
	// {
	// 	g_pHotQueue->UpdateHot(nodeId(), this->value(NULL));
	// }

	// //!����PVֵ
	// bool isCallParent = false;
	// if(ret.isGood() && NULL != m_pParent)
	// {
	// 	m_pParent->CallBack(this);
	// 	isCallParent = true;
	// }
	
	return ret;
}

// ComponentType* FieldPointType::getParent()
// {
// 	if (m_pParent)
// 	{
// 		return m_pParent;
// 	}
// 	else
// 	{
// 		return NULL;
// 	}
// }

//!���й������ݺ�ԭʼ���ݵĹ���ת��
void FieldPointType::RangeData( Byte* pByData, int nDataLen,  bool isRawToEng )
{
	if (pByData != NULL)
	{
		float fRawZero = getRawZero();
		float fRawFull = getRawFull();
		float fEngZero = getEngZero();
		float fEngFull = getEngFull();
		//UaNodeId uadataType = dataType();
		//if (fabs(fRawFull - fRawZero) > 0.001 && fabs(fEngFull - fEngZero) > 0.001)
		//{
		//	RangeConver(pByData, nDataLen, uadataType, fRawZero, fRawFull, fEngZero, fEngFull, isRawToEng);
		//}
	}
}



void FieldPointType::SetSamplingInterval( uint nSamplingInterval )
{
	m_nSamplingInterval = nSamplingInterval;
}


uint FieldPointType::GetSamplingInterval()
{
	return m_nSamplingInterval;
}

void FieldPointType::setUATagOnline( long dateTime )
{
	// UaMutexLocker lock(&m_statusCodeLock);
	// if (!m_uOldStatus.isUncertain())
	// {
	// 	Byte ret = -1;
	// 	UaDataValue dataValue;
	// 	dataValue = this->value(NULL);
	// 	dataValue.setStatusCode(m_uOldStatus.statusCode());
	// 	dataValue.setSourceTimestamp(dateTime);

	// 	ret = BaseDataVariableType::setValue(NULL, dataValue, false);
	// 	if (ret.isGood())
	// 	{
	// 		std::string ustrMsg(std::string("FieldPointType::setUATagOnline nodeId[%1] Success.").arg(this->nodeId().toFullString().toUtf8()));
	// 		LOG_INFO(MOD_FepPlatformClass, ustrMsg.toUtf8(), this);
	// 	}
	// 	else
	// 	{
	// 		std::string ustrMsg(std::string("FieldPointType::setUATagOnline nodeId[%1] Failed.").arg(this->nodeId().toFullString().toUtf8()));
	// 		LOG_ERROR(MOD_FepPlatformClass, ustrMsg.toUtf8(), this);
	// 	}

	// 	//!����������
	// 	if (ret.isGood() && NULL != g_pHotQueue)
	// 	{
	// 		g_pHotQueue->UpdateHot(nodeId(), this->value(NULL));
	// 	}

	// 	//!����PVֵ
	// 	bool isCallParent = false;
	// 	if(ret.isGood() && NULL != m_pParent)
	// 	{
	// 		m_pParent->CallBack(this);
	// 		isCallParent = true; 
	// 	}
	// }
}

std::string FieldPointType::displayName()
{
	return std::string("");
}

// void FieldPointType::setUATagOffline( UaDateTime dateTime )
// {
// 	UaDataValue fpVal = this->value(NULL);
// 	m_uOldStatus = fpVal.statusCode();
// 	std::string ustrMsg(std::string("FieldPointType::setUATagOffline nodeId[%1].").arg(this->nodeId().toFullString().toUtf8()));
// 	LOG_INFO(MOD_FepPlatformClass, ustrMsg.toUtf8(), this);
// 	setOffline(dateTime);
// }

CKiwiVariant FieldPointType::value()
{
	return m_realVal;
}