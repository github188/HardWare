#include "RTDBCommon/KiwiVariable.h"

namespace MACS_SCADA_SUD
{

	CKiwiVariable::CKiwiVariable(long	nNodeID)
		: m_nNodeID(nNodeID)
	{
		m_mapAttribute.clear();
	}


	CKiwiVariable::~CKiwiVariable(void)
	{
		std::map<std::string, CKiwiVariant*>::iterator itor = m_mapAttribute.begin();
		while(itor != m_mapAttribute.end())
		{
			CKiwiVariant* p = itor->second;
			delete p;
			itor++;
		}
		m_mapAttribute.clear();
	}

	void CKiwiVariable::InitAttribute(void)
	{
		CKiwiVariant *p = new CKiwiVariant;
		m_mapAttribute.insert(std::pair<std::string, CKiwiVariant*>("V",p));
		p = new CKiwiVariant;
		m_mapAttribute.insert(std::pair<std::string, CKiwiVariant*>("Des",p));
		p = new CKiwiVariant;
		m_mapAttribute.insert(std::pair<std::string, CKiwiVariant*>("tm",p));
		p = new CKiwiVariant;
		m_mapAttribute.insert(std::pair<std::string, CKiwiVariant*>("Q",p));
	}

	long CKiwiVariable::nodeId()
	{
		return m_nNodeID;
	}

	long CKiwiVariable::GetTimeStamp(void)
	{

		CKiwiVariant value;
		getAttributeValue("tm", value);
		long nVal;
		value.tolong(nVal);
		return nVal;
	}

	float CKiwiVariable::GetfValue()
	{
		CKiwiVariant value;
		getAttributeValue("V", value);
		float nVal;
		value.toFloat(nVal);
		return nVal;
	}

	int CKiwiVariable::GetValue()
	{
		CKiwiVariant value;
		getAttributeValue("V", value);
		int nVal;
		value.toInt32(nVal);
		return nVal;
	}

	unsigned char CKiwiVariable::GetByteValue()
	{
		CKiwiVariant value;
		getAttributeValue("V", value);
		unsigned char byVal;
		value.toByte(byVal);
		return byVal;
	}

	void CKiwiVariable::AddAttribute(const std::string& szAttribute)
	{
		CKiwiVariant *p = new CKiwiVariant;
		m_mapAttribute.insert(std::pair<std::string, CKiwiVariant*>(szAttribute,p));
	}

	bool CKiwiVariable::DelAttribute(const std::string& szAttribute)
	{		
		CKiwiVariant *p = NULL;
		std::map<std::string, CKiwiVariant*>::iterator it = m_mapAttribute.find(szAttribute);
		if(it != m_mapAttribute.end() )
		{
			m_mapAttribute.erase( it );
			delete it->second;
		}
	
		return true;
	}

	int CKiwiVariable::setAttributeValue(const std::string& szAttribute, const CKiwiVariant& value)
	{
		int nRet = -1;
		std::map<std::string, CKiwiVariant*>::iterator it = m_mapAttribute.find(szAttribute);
		if(it != m_mapAttribute.end() )
		{
			CKiwiVariant *p = it->second;
			*p = value;
			nRet = 0;
		}

		return nRet;
	}

	int CKiwiVariable::getAttributeValue(const std::string& szAttribute, CKiwiVariant& value)
	{		
		int nRet = -1;
		std::map<std::string, CKiwiVariant*>::iterator it = m_mapAttribute.find(szAttribute);
		if(it != m_mapAttribute.end() )
		{
			CKiwiVariant *p = it->second;
			value = *p;
			nRet = 0;
		}

		return nRet;
	}


}