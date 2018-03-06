#include "RTDBBaseClasses/DBObjectBaseType.h"

namespace MACS_SCADA_SUD
{
	CDBObjectBaseType::CDBObjectBaseType()
	{
	}
	CDBObjectBaseType::~CDBObjectBaseType()
	{

	}

	long CDBObjectBaseType::getArea1() const
	{
		return m_Area1;
	}

	void CDBObjectBaseType::setArea1(long nodeId)
	{
		m_Area1 = nodeId;
	}

	long CDBObjectBaseType::getArea2() const
	{
		return m_Area2;
	}

	void CDBObjectBaseType::setArea2(long nodeId)
	{
		m_Area2 = nodeId;
	}

	long CDBObjectBaseType::getArea3() const
	{
		return m_Area3;
	}

	void CDBObjectBaseType::setArea3(long nodeId)
	{
		m_Area3 = nodeId;
	}
}