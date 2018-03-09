#include "RTDBCommon/KiwiVariant.h"
#include <stdio.h>

namespace MACS_SCADA_SUD
{

	void T_Variant_Initialize(T_Variant* a_pValue)
	{

		if (a_pValue == NULL)
		{
			return;
		}

		memset(a_pValue, 0, sizeof(T_Variant));
	};

	void T_Variant_Clear(T_Variant* a_pValue)
	{

		if (a_pValue == NULL)
		{
			return;
		}
		if(a_pValue->vt == Kiwi_String)
		{
			if(a_pValue->value.pszValue)
			{
				delete []a_pValue->value.pszValue;
				a_pValue->value.pszValue = NULL;
			}
			a_pValue->nLen = 0;
			a_pValue->vt = Kiwi_Null;
		}
		else
			memset(a_pValue, 0, sizeof(T_Variant));
	}
	bool AlmostEqual(const float& fValue1, const float& fValue2)
	{
		if(fabs(fValue2 - fValue1) > 0.00000001)
			return true;
		return false;
	}
	bool AlmostEqual(const double& fValue1, const double& fValue2)
	{
		if(fabs(fValue2 - fValue1) > 0.00000001)
			return true;
		return false;
	}

	/** Constructs an empty variant. */
	CKiwiVariant::CKiwiVariant()
	{
		T_Variant_Initialize(&m_value);
	}

	/** Constructs a variant initialized with the passed variant value. */
	CKiwiVariant::CKiwiVariant(
		const CKiwiVariant &vVal) //!< [in] The value as CKiwiVariant
	{
		CKiwiVariant::cloneTo(vVal.m_value, m_value);
	}

	/** Constructs a variant initialized with the passed variant value. */
	CKiwiVariant::CKiwiVariant(
		const T_Variant& vVal) //!< [in] The value as T_Variant
	{
		CKiwiVariant::cloneTo(vVal, m_value);
	}

	/** Constructs a variant initialized with the passed variant value.
	* This version allows to detach the variant value from the passed structure to avoid
	* copying of variants containing a lot of data like arrays or strings.
	*/
	CKiwiVariant::CKiwiVariant(
		T_Variant *pValue,      //!< [in] The value as T_Variant
		bool detachValue)  //!< [in] Flag indicating if the passed variant should be detached
	{
		if ( detachValue == false )
		{
			CKiwiVariant::cloneTo(*pValue, m_value);
		}
		else
		{
			memcpy(&m_value, pValue, sizeof(T_Variant));
			T_Variant_Initialize(pValue);
		}
	}

	/** Constructs a variant initialized with the passed string value. */
	CKiwiVariant::CKiwiVariant(
		const std::string&  val) //!< [in] The value as std::string
	{
		T_Variant_Initialize(&m_value);
		setString(val);
	}


	/** Constructs a variant initialized with the passed SByte value. */
	CKiwiVariant::CKiwiVariant(
		char val) //!< [in] The value as Kiwi_SByte
	{
		T_Variant_Initialize(&m_value);
		setChar(val);
	}

	/** Constructs a variant initialized with the passed Int64 value. */
	CKiwiVariant::CKiwiVariant(
		Int64 val) //!< [in] The value as Int64
	{
		T_Variant_Initialize(&m_value);
		setInt64(val);
	}

	/** Constructs a variant initialized with the passed UInt64 value. */
	CKiwiVariant::CKiwiVariant(
		UInt64 val) //!< [in] The value as UInt64
	{
		T_Variant_Initialize(&m_value);
		setUInt64(val);
	}

	/** Constructs a variant initialized with the passed double value. */
	CKiwiVariant::CKiwiVariant(
		double dblVal) //!< [in] The value as double
	{
		T_Variant_Initialize(&m_value);
		setDouble(dblVal);
	}

	/** Constructs a variant initialized with the passed float value. */
	CKiwiVariant::CKiwiVariant(
		float fltVal) //!< [in] The value as float
	{
		T_Variant_Initialize(&m_value);
		setFloat(fltVal);
	}

	/** Constructs a variant initialized with the passed Int32 value. */
	CKiwiVariant::CKiwiVariant(
		int iVal) //!< [in] The value as int
	{
		T_Variant_Initialize(&m_value);
		setInt32(iVal);
	}

	/** Constructs a variant initialized with the passed Int16 value. */
	CKiwiVariant::CKiwiVariant(
		short int  iVal) //!< [in] The value as short int
	{
		T_Variant_Initialize(&m_value);
		setInt16(iVal);
	}

	/** Constructs a variant initialized with the passed UInt32 value. */
	CKiwiVariant::CKiwiVariant(
		unsigned int iVal) //!< [in] The value as unsigned int
	{
		T_Variant_Initialize(&m_value);
		setUInt32(iVal);
	}

	/** Constructs a variant initialized with the passed UInt16 value. */
	CKiwiVariant::CKiwiVariant(
		unsigned short int iVal) //!< [in] The value as unsigned short int
	{
		T_Variant_Initialize(&m_value);
		setUInt16(iVal);
	}

	/** Constructs a variant initialized with the passed bool value. */
	CKiwiVariant::CKiwiVariant(
		bool bVal) //!< [in] The value as bool
	{
		T_Variant_Initialize(&m_value);
		setBool(bVal);
	}

	/** destruction */
	CKiwiVariant::~CKiwiVariant()
	{
		T_Variant_Clear(&m_value);
	}

	//int CKiwiVariant::variantSize(OpcUa_BuiltInType type)
	//{
	//	return ms_variantsize[type];
	//}

	/** Get a pointer to the internal T_Variant.
	*  @return pointer to the internal T_Variant.
	*/
	CKiwiVariant::operator const T_Variant*() const
	{
		return &m_value;
	}

	/** Reset CKiwiVariant structure without clearing the data. */
	void CKiwiVariant::detach()
	{
		memset(&m_value, 0, sizeof(T_Variant));
	}

	/** Attach the internal variant data of the passed variant to the own variant.
	*  @param vVal a value as T_Variant.
	*/
	void CKiwiVariant::attach(const T_Variant *vVal)
	{
		clear();
		memcpy(&m_value,(void*)vVal, sizeof(T_Variant));
	}

	/** Copy CKiwiVariant data to a newly allocated T_Variant.
	*  @return data to a newly allocated T_Variant.
	*/
	T_Variant* CKiwiVariant::copy() const
	{
		return clone(m_value);
	}

	/** Copy CKiwiVariant data to an existing T_Variant structure.
	*  @param pDst the destination of this operation.
	*/
	void CKiwiVariant::copyTo(T_Variant *pDst) const
	{
		cloneTo(m_value, *pDst);
	}

	/** Copy CKiwiVariant data to a newly allocated CKiwiVariant.
	*  @param source the source of this operation.
	*  @return a newly allocated T_Variant.
	*/
	T_Variant* CKiwiVariant::clone(const T_Variant& source)
	{
		T_Variant* pCKiwiVariant = (T_Variant*)(new T_Variant);
		if (pCKiwiVariant)
		{
			cloneTo(source, *pCKiwiVariant);
		}
		return pCKiwiVariant;
	}

	/** Copy T_Variant data to an existing T_Variant structure.
	*  @param source Source T_Variant to clone.
	*  @param copy New copy of source T_Variant. copy gets not cleared within this method.
	*/
	void CKiwiVariant::cloneTo(const T_Variant& source, T_Variant& copy) // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{ // parasoft-suppress  METRICS-25 "������һ�����漰�������"
		T_Variant_Initialize(&copy);

		copy.vt = source.vt;
		switch (source.vt)
		{   // ToDo check compiler optimization of switch for whole class
		case Kiwi_Null:
		case Kiwi_Boolean:
		case Kiwi_SByte:
		case Kiwi_Byte:
		case Kiwi_Int16:
		case Kiwi_UInt16:
		case Kiwi_Int32:
		case Kiwi_UInt32:
		case Kiwi_Int64:
		case Kiwi_UInt64:
		case Kiwi_Float:
		case Kiwi_Double:
			{
				copy.value = source.value;
				break;
			}
		case Kiwi_String:
			{
				size_t nLen = strlen(source.value.pszValue);
				if (strlen(source.value.pszValue) > 0)
				{
					if(copy.vt == Kiwi_String)
						delete copy.value.pszValue;
					copy.value.pszValue = new char[source.nLen];
					strncpy(copy.value.pszValue, source.value.pszValue, source.nLen);
					copy.nLen = source.nLen;
				}
				break;
			}
		default:
			{
				// TODO DiagnosticInfo
				copy.vt           = Kiwi_Null;
				break;
			}
		}
	}

	/** Get the type of the Variant.
	*  @return the type of the Variant.
	*/
	Kiwi_BuiltInType CKiwiVariant::type() const
	{
		return (Kiwi_BuiltInType)m_value.vt;
	}

	/** Checks whether the variant is empty or not.
	*  @return true if empty false if not.
	*/
	bool CKiwiVariant::isEmpty() const
	{
		if ( m_value.vt == Kiwi_Null )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/** Assigns another variant to this variant and clears the old value.
	*/
	CKiwiVariant& CKiwiVariant::operator=(const CKiwiVariant& other)
	{
		if (this == &other) {return *this;}

		clear();
		cloneTo(other.m_value, m_value);
		return *this;
	}

	/** Assigns the content of an T_Variant variant structure to this variant and clears the old value.
	*/
	CKiwiVariant& CKiwiVariant::operator=(const T_Variant& vVal)
	{
		clear();
		cloneTo(vVal, m_value);
		return *this;
	}

	/** Assigns a string to this variant and clears the old value.
	*/
	CKiwiVariant& CKiwiVariant::operator=(const std::string &strVal)
	{
		setString(strVal);
		return *this;
	}

	/** Assigns a double value to this variant and clears the old value.
	*/
	CKiwiVariant& CKiwiVariant::operator=(double dblVal)
	{
		setDouble(dblVal);
		return *this;
	}

	/** Returns true if this variant is equal to variant other; otherwise returns false.
	* Return false if the data type of the two variants are different.
	*/
	bool CKiwiVariant::operator==(const CKiwiVariant &other) const
	{
		return compare(m_value, other.m_value);
	}

	/** Returns true if this variant is not equal to variant other; otherwise returns false.
	*  @see operator==
	*/
	bool CKiwiVariant::operator!=(const CKiwiVariant &vVal) const
	{
		return !(compare(m_value, vVal.m_value));
	}

	/** Compares two variant values.
	*  @param Val1 First value as T_Variant.
	*  @param Val2 Second value as T_Variant.
	*  @return true if equal false if not.
	*/
	bool CKiwiVariant::compare(const T_Variant& Val1, const T_Variant& Val2) // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		if ( (Val1.vt != Val2.vt))
		{
			return false;
		}
		// scalar
		switch(Val2.vt)
		{
		case Kiwi_Null:
			return true;
			break;
		case Kiwi_Boolean:
			return Val2.value.bValue == Val1.value.bValue;
			break;
		case Kiwi_SByte:
			return Val2.value.chValue == Val1.value.chValue;
			break;
		case Kiwi_Byte:
			return Val2.value.BValue == Val1.value.BValue;
			break;
		case Kiwi_Int16:
			return Val2.value.shValue == Val1.value.shValue;
			break;
		case Kiwi_UInt16:
			return Val2.value.ushValue == Val1.value.ushValue;
			break;
		case Kiwi_Int32:
			return Val2.value.nValue == Val1.value.nValue;
			break;
		case Kiwi_UInt32:
			return Val2.value.unValue == Val1.value.unValue;
			break;
		case Kiwi_Int64:
			return Val2.value.llValue == Val1.value.llValue;
			break;
		case Kiwi_UInt64:
			return Val2.value.ullValue == Val1.value.ullValue;
			break;
		case Kiwi_Float:
			//return Val2.Value.fValue == Val1.Value.fValue;
			return AlmostEqual(Val2.value.fValue, Val1.value.fValue);
			break;
		case Kiwi_Double:
			//return Val2.Value.dValue == Val1.Value.dValue;
			return AlmostEqual(Val2.value.dValue, Val1.value.dValue);
			break;
		case Kiwi_String:
			return (strcmp( Val2.value.pszValue, Val1.value.pszValue) == 0);
			break;

		default:
			return false;
		}
	}

	/** Adjust types to prepare comparison of values independent of the variant type.
	* Adjustment is only implemented for Number data types. The data type is adjusted to Double, UInt64 or Int64 depending on the passed data types.
	* Setting bForceConversion to true forces the method to always convert the variants to either Double, Int64 or UInt64.
	*/
	void CKiwiVariant::adjustTypes(CKiwiVariant& value1, CKiwiVariant& value2, bool bForceConversion)
	{
		if ( !bForceConversion && value1.m_value.vt == value2.m_value.vt )
		{
			// Nothing to do
			return;
		}

		bool is1Double = false;
		bool is2Double = false;
		bool is1Int = false; // parasoft-suppress  OPT-06 "�߼���ȷ"
		bool is2Int = false; // parasoft-suppress  OPT-06 "�߼���ȷ"
		bool is1UInt = false;
		bool is2UInt = false;

		// Classify value 1
		switch(value1.m_value.vt)
		{
		case Kiwi_Boolean:
		case Kiwi_Byte:
		case Kiwi_UInt16:
		case Kiwi_UInt32:
		case Kiwi_UInt64:
			{
				is1UInt = true;
				break;
			}
		case Kiwi_SByte:
		case Kiwi_Int16:
		case Kiwi_Int32:
		case Kiwi_Int64:
			{
				is1Int = true;
				break;
			}
		case Kiwi_Float:
		case Kiwi_Double:
			{
				is1Double = true;
				break;
			}
		default:
			// Nothing we can adjust
			return;
		}

		// Classify value 2
		switch(value2.m_value.vt)
		{
		case Kiwi_Boolean:
		case Kiwi_Byte:
		case Kiwi_UInt16:
		case Kiwi_UInt32:
		case Kiwi_UInt64:
			{
				is2UInt = true;
				break;
			}
		case Kiwi_SByte:
		case Kiwi_Int16:
		case Kiwi_Int32:
		case Kiwi_Int64:
			{
				is2Int = true;
				break;
			}
		case Kiwi_Float:
		case Kiwi_Double:
			{
				is2Double = true;
				break;
			}
		default:
			// Nothing we can adjust
			return;
		}

		// Adjust type
		//if ( is1UInt && is2UInt )
		//{
		//	value1.changeType(Kiwi_UInt64, false);
		//	value2.changeType(Kiwi_UInt64, false);
		//}
		//else if ( is1Double || is2Double )
		//{
		//	value1.changeType(Kiwi_Double, false);
		//	value2.changeType(Kiwi_Double, false);
		//}
		//else
		//{
		//	value1.changeType(Kiwi_Int64, false);
		//	value2.changeType(Kiwi_Int64, false);
		//}
	}

	/** Returns true if this variant is greater than variant other; otherwise returns false.
	* Returns false if the data type of the two variants are different.
	* Returns false if the value is not scalar.
	*/
	bool CKiwiVariant::operator>(const CKiwiVariant &other) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		if ( (m_value.vt != other.m_value.vt))
		{
			return false;
		}


		switch(m_value.vt)
		{
		case Kiwi_Null:
			return false;
		case Kiwi_Boolean:
			return m_value.value.BValue > other.m_value.value.BValue;
		case Kiwi_SByte:
			return m_value.value.chValue > other.m_value.value.chValue;
		case Kiwi_Byte:
			return m_value.value.BValue > other.m_value.value.BValue;
		case Kiwi_Int16:
			return m_value.value.shValue > other.m_value.value.shValue;
		case Kiwi_UInt16:
			return m_value.value.ushValue > other.m_value.value.ushValue;
		case Kiwi_Int32:
			return m_value.value.nValue > other.m_value.value.nValue;
		case Kiwi_UInt32:
			return m_value.value.unValue > other.m_value.value.unValue;
		case Kiwi_Int64:
			return m_value.value.llValue > other.m_value.value.llValue;
		case Kiwi_UInt64:
			return m_value.value.ullValue > other.m_value.value.ullValue;
		case Kiwi_Float:
			return m_value.value.fValue > other.m_value.value.fValue;
		case Kiwi_Double:
			return m_value.value.dValue > other.m_value.value.dValue;
		case Kiwi_String:
			{
				//std::string s1(m_value.value.pszValue);
				//std::string s2(other.m_value.value.pszValue);
				//return !((s1 < s2) || (s1 == s2));
				return false;
			}
		default:
			return false;
		}
	}

	/** Returns true if this variant is less than variant other; otherwise returns false.
	* Returns false if the data type of the two variants are different.
	* Returns false if the value is not scalar.
	*/
	bool CKiwiVariant::operator<(const CKiwiVariant &other) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		if ( (m_value.vt != other.m_value.vt) )
		{
			return false;
		}


		switch(m_value.vt)
		{
		case Kiwi_Null:
			return false;
		case Kiwi_Boolean:
			return m_value.value.bValue < other.m_value.value.bValue;
		case Kiwi_SByte:
			return m_value.value.chValue < other.m_value.value.chValue;
		case Kiwi_Byte:
			return m_value.value.BValue < other.m_value.value.BValue;
		case Kiwi_Int16:
			return m_value.value.shValue < other.m_value.value.shValue;
		case Kiwi_UInt16:
			return m_value.value.ushValue < other.m_value.value.ushValue;
		case Kiwi_Int32:
			return m_value.value.nValue < other.m_value.value.nValue;
		case Kiwi_UInt32:
			return m_value.value.unValue < other.m_value.value.unValue;
		case Kiwi_Int64:
			return m_value.value.llValue < other.m_value.value.llValue;
		case Kiwi_UInt64:
			return m_value.value.ullValue < other.m_value.value.ullValue;
		case Kiwi_Float:
			return m_value.value.fValue < other.m_value.value.fValue;
		case Kiwi_Double:
			return m_value.value.dValue < other.m_value.value.dValue;
		case Kiwi_String:
			{
				//std::string s1(m_value.value.pszValue);
				//std::string s2(other.m_value.value.pszValue);
				//return s1 < s2;
				return false;
			}
		default:
			return false;
		}
	}

	/** Returns true if this variant is greater than or equal to variant other; otherwise returns false.
	* Returns false if the data type of the two variants are different.
	* Returns false if the value is not scalar.
	*/
	bool CKiwiVariant::operator>=(const CKiwiVariant &other) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		if ( (m_value.vt != other.m_value.vt))
		{
			return false;
		}

		switch(m_value.vt)
		{
		case Kiwi_Null:
			return false;
		case Kiwi_Boolean:
			return m_value.value.bValue >= other.m_value.value.bValue;
		case Kiwi_SByte:
			return m_value.value.chValue >= other.m_value.value.chValue;
		case Kiwi_Byte:
			return m_value.value.BValue >= other.m_value.value.BValue;
		case Kiwi_Int16:
			return m_value.value.shValue >= other.m_value.value.shValue;
		case Kiwi_UInt16:
			return m_value.value.ushValue >= other.m_value.value.ushValue;
		case Kiwi_Int32:
			return m_value.value.nValue >= other.m_value.value.nValue;
		case Kiwi_UInt32:
			return m_value.value.unValue >= other.m_value.value.unValue;
		case Kiwi_Int64:
			return m_value.value.llValue >= other.m_value.value.llValue;
		case Kiwi_UInt64:
			return m_value.value.ullValue >= other.m_value.value.ullValue;
		case Kiwi_Float:
			return m_value.value.fValue >= other.m_value.value.fValue;
		case Kiwi_Double:
			return m_value.value.dValue >= other.m_value.value.dValue;
		case Kiwi_String:
			{
				//std::string s1(m_value.value.pszValue);
				//std::string s2(other.m_value.value.pszValue);
				//return !(s1 < s2);
				return false;
			}
		default:
			return false;
		}
	}

	/** Returns true if this variant is less than or equal to variant other; otherwise returns false.
	* Returns false if the data type of the two variants are different.
	* Returns false if the value is not scalar.
	*/
	bool CKiwiVariant::operator<=(const CKiwiVariant &other) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		if ( (m_value.vt != other.m_value.vt))
		{
			return false;
		}

		switch(m_value.vt)
		{
		case Kiwi_Null:
			return false;
		case Kiwi_Boolean:
			return m_value.value.bValue <= other.m_value.value.bValue;
		case Kiwi_SByte:
			return m_value.value.chValue <= other.m_value.value.chValue;
		case Kiwi_Byte:
			return m_value.value.BValue <= other.m_value.value.BValue;
		case Kiwi_Int16:
			return m_value.value.shValue <= other.m_value.value.shValue;
		case Kiwi_UInt16:
			return m_value.value.ushValue <= other.m_value.value.ushValue;
		case Kiwi_Int32:
			return m_value.value.nValue <= other.m_value.value.nValue;
		case Kiwi_UInt32:
			return m_value.value.unValue <= other.m_value.value.unValue;
		case Kiwi_Int64:
			return m_value.value.llValue <= other.m_value.value.llValue;
		case Kiwi_UInt64:
			return m_value.value.ullValue <= other.m_value.value.ullValue;
		case Kiwi_Float:
			return m_value.value.fValue <= other.m_value.value.fValue;
		case Kiwi_Double:
			return m_value.value.dValue <= other.m_value.value.dValue;
		case Kiwi_String:
			{
				//std::string s1(m_value.value.pszValue);
				//std::string s2(other.m_value.value.pszValue);
				//return ((s1 < s2) || (s1 == s2));
				return false;
			}
		default:
			return false;
		}
	}

	/**  Clear the data of the CKiwiVariant. */
	void CKiwiVariant::clear()
	{
		T_Variant_Clear(&m_value);
	}

	/** Sets a boolean value
	*/
	void CKiwiVariant::setBool(
		bool val) //!< [in] The new boolean value
	{
		clear();
		m_value.value.bValue   = val;
		m_value.vt        = Kiwi_Boolean;
	}

	/** Sets a value to Kiwi_SByte.
	*  @param val a value as Kiwi_SByte.
	*/
	void CKiwiVariant::setChar(char val)
	{
		clear();
		m_value.value.chValue = val;
		m_value.vt    = Kiwi_SByte;
	}

	/** Sets a value to short int .
	*  @param val a value as short int .
	*/
	void CKiwiVariant::setInt16(short int  val)
	{
		clear();
		m_value.value.shValue = val;
		m_value.vt    = Kiwi_Int16;
	}

	/** Sets a value to int.
	*  @param val a value as int.
	*/
	void CKiwiVariant::setInt32(int val)
	{
		clear();
		m_value.value.nValue = val;
		m_value.vt    = Kiwi_Int32;
	}

	/** Sets a value to Int64.
	*  @param val a value as Int64.
	*/
	void CKiwiVariant::setInt64(Int64 val)
	{
		clear();
		m_value.value.llValue = val;
		m_value.vt    = Kiwi_Int64;
	}

	/** Sets a value to OpcUa_Byte.
	*  @param val a value as OpcUa_Byte.
	*/
	void CKiwiVariant::setByte(unsigned char val)
	{
		clear();
		m_value.value.BValue  = val;
		m_value.vt    = Kiwi_Byte;
	}

	/** Sets a value to unsigned short int.
	*  @param val a value as unsigned short int.
	*/
	void CKiwiVariant::setUInt16(unsigned short int val)
	{
		clear();
		m_value.value.ushValue    = val;
		m_value.vt        = Kiwi_UInt16;
	}

	/** Sets a unsigned int value to the Variant.
	*  @param val a value as unsigned int.
	*/
	void CKiwiVariant::setUInt32(unsigned int val)
	{
		clear();
		m_value.value.unValue    = val;
		m_value.vt        = Kiwi_UInt32;
	}

	/** Sets a value to UInt64.
	*  @param val a value as UInt64.
	*/
	void CKiwiVariant::setUInt64(UInt64 val)
	{
		clear();
		m_value.value.ullValue    = val;
		m_value.vt        = Kiwi_UInt64;
	}

	/** Sets a value to float.
	*  @param val a value as float.
	*/
	void CKiwiVariant::setFloat(float val)
	{
		clear();
		m_value.value.fValue = val;
		m_value.vt    = Kiwi_Float;
	}

	/** Sets a value to double.
	*  @param val a value as double.
	*/
	void CKiwiVariant::setDouble(double val)
	{
		clear();
		m_value.value.dValue    = val;
		m_value.vt        = Kiwi_Double;
	}


	/** Sets a String value to the Variant.
	 *  @param val a value as UaString.
	 */
	void CKiwiVariant::setString(const std::string& val)
	{
		clear();
		m_value.nLen = val.length() + 1;
		m_value.value.pszValue = new char[m_value.nLen];
		strcpy(m_value.value.pszValue, val.c_str());
		m_value.vt = Kiwi_String;
	}
	/** Converts the CKiwiVariant into a std::string.
	*  @return the CKiwiVariant as string.
	*/
	std::string CKiwiVariant::toString() const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		const int size = 256;
		char tmp[size] = "";
		std::string ret;

		// Scalar value
		switch(m_value.vt)
		{
		case Kiwi_Null:
			break;
		case Kiwi_Boolean:
			{
				if (m_value.value.bValue == false)
				{
					strcpy(tmp, "false" );
				}
				else
				{
					strcpy(tmp, "true");
				}
				break;
			}
		case Kiwi_SByte:
			sprintf(tmp,  "%d" , m_value.value.chValue);
			break;
		case Kiwi_Byte:
			sprintf(tmp,  "%d" , m_value.value.BValue);
			break;
		case Kiwi_Int16:
			sprintf(tmp,  "%d" , m_value.value.shValue);
			break;
		case Kiwi_UInt16:
			sprintf(tmp,  "%d" , m_value.value.ushValue);
			break;
		case Kiwi_Int32:
			sprintf(tmp,  "%d" , m_value.value.nValue);
			break;
		case Kiwi_UInt32:
			sprintf(tmp,  "%d" , m_value.value.unValue);
			break;
		case Kiwi_Int64:
			sprintf(tmp,  "%d" , m_value.value.llValue);
			break;
		case Kiwi_UInt64:
			sprintf(tmp,  "%d" , m_value.value.ullValue);
			break;
		case Kiwi_Float:
			sprintf(tmp,  "%.6f", m_value.value.fValue);
			break;
		case Kiwi_Double:
			sprintf(tmp,  "%.12f", m_value.value.dValue);
			break;
		case Kiwi_String:
			{
				ret = m_value.value.pszValue;
				return ret;
			}
			break;
		}
		ret = tmp;
		return ret;
	}
	/** Converts the CKiwiVariant into a bool.
	*  @param val a value as bool.
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toBool(bool& val) const
	{
		val = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = strlen(m_value.value.pszValue);
				//if ( strLen == 1 )
				//{
				//	OpcUa_CharA* pString = OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue);
				//	if ( (pString != OpcUa_Null) && (pString[0] == '0') )
				//	{
				//		val = false;
				//	}
				//}
				//else if  ( strLen == 5 )
				//{
				//	OpcUa_String tempString;
				//	OpcUa_String_Initialize(&tempString);
				//	OpcUa_String_AttachToString( (OpcUa_StringA)"false",
				//		5,
				//		0,
				//		true,
				//		false,
				//		&tempString);
				//	if ( 0 == OpcUa_String_StrnCmp((OpcUa_String*)&m_value.value.pszValue, &tempString, 5, true) )
				//	{
				//		val = false;
				//	}
				//	OpcUa_String_Clear(&tempString);
				//}
				break;
			}
		case Kiwi_Boolean:
			{
				val = m_value.value.bValue;
				break;
			}
		case Kiwi_SByte:
		case Kiwi_Byte:
		case Kiwi_Int16:
		case Kiwi_UInt16:
		case Kiwi_Int32:
		case Kiwi_UInt32:
		case Kiwi_Int64:
		case Kiwi_UInt64:
			// everything != 0 is true
			if ( m_value.value.llValue == 0 )
			{
				val = false;
			}
			break;
		case Kiwi_Float:
			// everything != 0 is true
			//if ( m_value.value.fValue == 0.0 )
			if(AlmostEqual(m_value.value.fValue, 0.0f))
			{
				val = false;
			}
			break;
		case Kiwi_Double:
			// everything != 0 is true
			//if ( m_value.value.dValue == 0.0 )
			if(AlmostEqual(m_value.value.dValue, 0.0))
			{
				val = false;
			}
			break;
		default:
			return false;
			break;
		}
		return true;
	}

	/** Converts the CKiwiVariant into a Kiwi_SByte.
	*  @param val a value as Kiwi_SByte.
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toChar(char& val) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = OpcUa_String_StrSize((OpcUa_String*)&m_value.value.pszValue);
				//if ( strLen < 100 )
				//{
				//	long tempVal;
				//	UA_SetLastError(0);
				//	char strVal[100];
				//	strlcpy(strVal, OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue), sizeof(strVal) );
				//	char* sEnd = NULL;
				//	tempVal = strtol(strVal, &sEnd, 10);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) ||
				//		(UA_GetLastError() != 0) ||
				//		(tempVal < Kiwi_SByte_Min || tempVal > Kiwi_SByte_Max) )
				//	{
				//		status = false;
				//	}
				//	else
				//	{
				//		val = (Kiwi_SByte)tempVal;
				//	}
				//}
				//else
				{
					status = false;
				}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			val = m_value.value.chValue;
			break;
		case Kiwi_Int16:
			if (m_value.value.shValue < Byte_Min || m_value.value.shValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.shValue;
			}
			break;
		case Kiwi_Int32:
			if (m_value.value.nValue < Byte_Min || m_value.value.nValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.nValue;
			}
			break;
		case Kiwi_Int64:
			if (m_value.value.llValue < Byte_Min || m_value.value.llValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.llValue;
			}
			break;
		case Kiwi_Byte:
			if ( m_value.value.BValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.BValue;
			}
			break;
		case Kiwi_UInt16:
			if ( m_value.value.ushValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.ushValue;
			}
			break;
		case Kiwi_UInt32:
			if ( m_value.value.unValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.unValue;
			}
			break;
		case Kiwi_UInt64:
			if ( m_value.value.ullValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.ullValue;
			}
			break;
		case Kiwi_Float:
			if (m_value.value.fValue < Byte_Min || m_value.value.fValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				if ( m_value.value.fValue >= 0 )
				{
					val = (char)(m_value.value.fValue+0.5); // rounding
				}
				else
				{
					val = (char)(m_value.value.fValue-0.5); // rounding
				}
			}
			break;
		case Kiwi_Double:
			if (m_value.value.dValue < Byte_Min || m_value.value.dValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				if ( m_value.value.dValue >= 0 )
				{
					val = (char)(m_value.value.dValue+0.5); // rounding
				}
				else
				{
					val = (char)(m_value.value.dValue-0.5); // rounding
				}
			}
			break;
		default:
			status = false;
		}

		return status;
	}

	/** Converts the CKiwiVariant into a short int .
	*  @param val a value as short int .
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toInt16(short int & val) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{

		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = OpcUa_String_StrSize((OpcUa_String*)&m_value.value.pszValue);
				//if ( strLen < 100 )
				//{
				//	long tempVal;
				//	UA_SetLastError(0);
				//	char strVal[100];
				//	strlcpy(strVal, OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue), sizeof(strVal) );
				//	char* sEnd = NULL;
				//	tempVal = strtol(strVal, &sEnd, 10);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) ||
				//		(UA_GetLastError() != 0) ||
				//		(tempVal < short int _Min || tempVal > short int _Max) )
				//	{
				//		status = false;
				//	}
				//	else
				//	{
				//		val = (short int )tempVal;
				//	}
				//}
				//else
				//{
				//	status = false;
				//}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			val = m_value.value.chValue;
			break;
		case Kiwi_Int16:
			val = m_value.value.shValue;
			break;
		case Kiwi_Int32:
			if (m_value.value.nValue < Int32_Min || m_value.value.nValue > Int32_Max)
			{
				status = false;
			}
			else
			{
				val = (short int )m_value.value.nValue;
			}
			break;
		case Kiwi_Int64:
			if (m_value.value.llValue < Int64_Min
				|| m_value.value.llValue > Int64_Max)
			{
				status = false;
			}
			else
			{
				val = (short int )m_value.value.llValue;
			}
			break;
		case Kiwi_Byte:
			val = (short int )m_value.value.BValue;
			break;
		case Kiwi_UInt16:
			if ( m_value.value.ushValue > UInt16_Max)
			{
				status = false;
			}
			else
			{
				val = (short int )m_value.value.ushValue;
			}
			break;
		case Kiwi_UInt32:
			if ( m_value.value.unValue > UInt32_Max)
			{
				status = false;
			}
			else
			{
				val = (short int )m_value.value.unValue;
			}
			break;
		case Kiwi_UInt64:
			if ( m_value.value.ullValue > UInt64_Max)
			{
				status = false;
			}
			else
			{
				val = (short int )m_value.value.ullValue;
			}
			break;
		case Kiwi_Float:
			if (m_value.value.fValue < Float_Min || m_value.value.fValue > Float_Max)
			{
				status = false;
			}
			else
			{
				if ( m_value.value.fValue >= 0 )
				{
					val = (short int )(m_value.value.fValue+0.5); // rounding
				}
				else
				{
					val = (short int )(m_value.value.fValue-0.5); // rounding
				}
			}
			break;
		case Kiwi_Double:
			if (m_value.value.dValue < Double_Min || m_value.value.dValue > Double_Max)
			{
				status = false;
			}
			else
			{
				if ( m_value.value.dValue >= 0 )
				{
					val = (short int )(m_value.value.dValue+0.5); // rounding
				}
				else
				{
					val = (short int )(m_value.value.dValue-0.5); // rounding
				}
			}
			break;
		default:
			status = false;
		}

		return status;
	}

	/** Converts the CKiwiVariant into a int.
	*  @param val a value as int.
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toInt32(int& val) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = OpcUa_String_StrSize((OpcUa_String*)&m_value.value.pszValue);
				//if ( strLen < 100 )
				//{
				//	long tempVal;
				//	UA_SetLastError(0);
				//	char strVal[100];
				//	strlcpy(strVal, OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue), sizeof(strVal) );
				//	char* sEnd = NULL;
				//	tempVal = strtol(strVal, &sEnd, 10);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) ||
				//		(UA_GetLastError() != 0) ||
				//		(tempVal < int_Min || tempVal > int_Max) )
				//	{
				//		status = false;
				//	}
				//	else
				//	{
				//		val = (int)tempVal;
				//	}
				//}
				//else
				{
					status = false;
				}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			val = m_value.value.chValue;
			break;
		case Kiwi_Int16:
			val = m_value.value.shValue;
			break;
		case Kiwi_Int32:
			val = m_value.value.nValue;
			break;
		case Kiwi_Int64:
			if (m_value.value.llValue < Int64_Min || m_value.value.llValue > Int64_Max)
			{
				status = false;
			}
			else
			{
				val = (int)m_value.value.llValue;
			}
			break;
		case Kiwi_Byte:
			val = (int)m_value.value.BValue;
			break;
		case Kiwi_UInt16:
			val = (int)m_value.value.ushValue;
			break;
		case Kiwi_UInt32:
			if ( m_value.value.unValue > Int32_Max)
			{
				status = false;
			}
			else
			{
				val = (int)m_value.value.unValue;
			}
			break;
		case Kiwi_UInt64:
			if ( m_value.value.ullValue > UInt64_Max)
			{
				status = false;
			}
			else
			{
				val = (int)m_value.value.ullValue;
			}
			break;
		case Kiwi_Float:
			if (m_value.value.fValue < Float_Min || m_value.value.fValue > Float_Max)
			{
				status = false;
			}
			else
			{
				if ( m_value.value.fValue >= 0 )
				{
					val = (int)(m_value.value.fValue+0.5); // rounding
				}
				else
				{
					val = (int)(m_value.value.fValue-0.5); // rounding
				}
			}
			break;
		case Kiwi_Double:
			if (m_value.value.dValue < Double_Min || m_value.value.dValue > Double_Max)
			{
				status = false;
			}
			else
			{
				if ( m_value.value.dValue >= 0 )
				{
					val = (int)(m_value.value.dValue+0.5); // rounding
				}
				else
				{
					val = (int)(m_value.value.dValue-0.5); // rounding
				}
			}
			break;
		default:
			status = false;
		}

		return status;
	}

	/** Converts the CKiwiVariant into a Int64.
	*  @param val a value as Int64.
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toInt64(Int64& val) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{

		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = OpcUa_String_StrSize((OpcUa_String*)&m_value.value.pszValue);
				//if ( strLen < 30 )
				//{
				//	UA_SetLastError(0);
				//	char strVal[30];
				//	strlcpy(strVal, OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue), sizeof(strVal) );
				//	char* sEnd = NULL;
				//	val = strtoll(strVal, &sEnd, 10);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) || (UA_GetLastError() != 0) )
				//	{
				//		status = false;
				//	}
				//}
				//else
				{
					status = false;
				}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			val = m_value.value.chValue;
			break;
		case Kiwi_Int16:
			val = m_value.value.shValue;
			break;
		case Kiwi_Int32:
			val = m_value.value.nValue;
			break;
		case Kiwi_Int64:
			val = m_value.value.llValue;
			break;
		case Kiwi_Byte:
			val = (Int64)m_value.value.BValue;
			break;
		case Kiwi_UInt16:
			val = (Int64)m_value.value.ushValue;
			break;
		case Kiwi_UInt32:
			val = (Int64)m_value.value.unValue;
			break;
		case Kiwi_UInt64:
			if ( m_value.value.ullValue > UInt64_Max)
			{
				status = false;
			}
			else
			{
				val = (Int64)m_value.value.ullValue;
			}
			break;
		case Kiwi_Float:
			if (m_value.value.fValue < Float_Min || m_value.value.fValue > Float_Max)
			{
				status = false;
			}
			else
			{
				if ( m_value.value.fValue >= 0 )
				{
					val = (Int64)(m_value.value.fValue+0.5); // rounding
				}
				else
				{
					val = (Int64)(m_value.value.fValue-0.5); // rounding
				}
			}
			break;
		case Kiwi_Double:
			if (m_value.value.dValue < Double_Min || m_value.value.dValue > Double_Max)
			{
				status = false;
			}
			else
			{
				if ( m_value.value.dValue >= 0 )
				{
					val = (Int64)(m_value.value.dValue+0.5); // rounding
				}
				else
				{
					val = (Int64)(m_value.value.dValue-0.5); // rounding
				}
			}
			break;
		default:
			status = false;
		}

		return status;
	}

	/** Converts the CKiwiVariant into a OpcUa_Byte.
	*  @param val a value as OpcUa_Byte.
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toByte(unsigned char& val) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = OpcUa_String_StrSize((OpcUa_String*)&m_value.value.pszValue);
				//if ( strLen < 100 )
				//{
				//	unsigned long tempVal;
				//	UA_SetLastError(0);
				//	char strVal[100];
				//	strlcpy(strVal, OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue), sizeof(strVal) );
				//	char* sEnd = NULL;
				//	tempVal = strtoul(strVal, &sEnd, 10);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) ||
				//		(UA_GetLastError() != 0) ||
				//		(tempVal > OpcUa_Byte_Max) ||
				//		(strchr(strVal, '-') != OpcUa_Null) )
				//	{
				//		status = false;
				//	}
				//	else
				//	{
				//		val = (OpcUa_Byte)tempVal;
				//	}
				//}
				//else
				{
					status = false;
				}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			if (m_value.value.chValue < 0)
			{
				status = false;
			}
			else
			{
				val = m_value.value.chValue;
			}
			break;
		case Kiwi_Int16:
			if (m_value.value.shValue < 0 || m_value.value.shValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.shValue;
			}
			break;
		case Kiwi_Int32:
			if (m_value.value.nValue < 0 || m_value.value.nValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.nValue;
			}
			break;
		case Kiwi_Int64:
			if (m_value.value.llValue < 0 || m_value.value.llValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.llValue;
			}
			break;
		case Kiwi_Byte:
			val = m_value.value.BValue;
			break;
		case Kiwi_UInt16:
			if ( m_value.value.ushValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.ushValue;
			}
			break;
		case Kiwi_UInt32:
			if ( m_value.value.unValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.unValue;
			}
			break;
		case Kiwi_UInt64:
			if ( m_value.value.ullValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)m_value.value.ullValue;
			}
			break;
		case Kiwi_Float:
			if (m_value.value.fValue < 0 || m_value.value.fValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)(m_value.value.fValue+0.5); // rounding
			}
			break;
		case Kiwi_Double:
			if (m_value.value.dValue < 0 || m_value.value.dValue > Byte_Max)
			{
				status = false;
			}
			else
			{
				val = (char)(m_value.value.dValue+0.5); // rounding
			}
			break;
		default:
			status = false;
		}

		return status;
	}

	/** Converts the CKiwiVariant into a unsigned short int.
	*  @param val a value as unsigned short int.
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toUInt16(unsigned short int& val) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = OpcUa_String_StrSize((OpcUa_String*)&m_value.value.pszValue);
				//if ( strLen < 100 )
				//{
				//	unsigned long tempVal;
				//	UA_SetLastError(0);
				//	char strVal[100];
				//	strlcpy(strVal, OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue), sizeof(strVal) );
				//	char* sEnd = NULL;
				//	tempVal = strtoul(strVal, &sEnd, 10);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) ||
				//		(UA_GetLastError() != 0) ||
				//		(tempVal > unsigned short int_Max) ||
				//		(strchr(strVal, '-') != OpcUa_Null) )
				//	{
				//		status = false;
				//	}
				//	else
				//	{
				//		val = (unsigned short int)tempVal;
				//	}
				//}
				//else
				{
					status = false;
				}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			if (m_value.value.chValue < 0)
			{
				status = false;
			}
			else
			{
				val = (unsigned short int)m_value.value.chValue;
			}
			break;
		case Kiwi_Int16:
			if (m_value.value.shValue < 0 )
			{
				status = false;
			}
			else
			{
				val = (unsigned short int)m_value.value.shValue;
			}
			break;
		case Kiwi_Int32:
			if (m_value.value.nValue < 0 || m_value.value.nValue > Int32_Max)
			{
				status = false;
			}
			else
			{
				val = (unsigned short int)m_value.value.nValue;
			}
			break;
		case Kiwi_Int64:
			if (m_value.value.llValue < 0 || m_value.value.llValue > Int32_Max)
			{
				status = false;
			}
			else
			{
				val = (unsigned short int)m_value.value.llValue;
			}
			break;
		case Kiwi_Byte:
			val = m_value.value.BValue;
			break;
		case Kiwi_UInt16:
			val = m_value.value.ushValue;
			break;
		case Kiwi_UInt32:
			if ( m_value.value.unValue > Int32_Max)
			{
				status = false;
			}
			else
			{
				val = (unsigned short int)m_value.value.unValue;
			}
			break;
		case Kiwi_UInt64:
			if ( m_value.value.ullValue > Int32_Max)
			{
				status = false;
			}
			else
			{
				val = (unsigned short int)m_value.value.ullValue;
			}
			break;
		case Kiwi_Float:
			if (m_value.value.fValue < 0 || m_value.value.fValue > Int32_Max)
			{
				status = false;
			}
			else
			{
				val = (unsigned short int)(m_value.value.fValue+0.5); // rounding
			}
			break;
		case Kiwi_Double:
			if (m_value.value.dValue < 0 || m_value.value.dValue > Int32_Max)
			{
				status = false;
			}
			else
			{
				val = (unsigned short int)(m_value.value.dValue+0.5); // rounding
			}
			break;
		default:
			status = false;
		}

		return status;
	}

	/** Converts the CKiwiVariant into a unsigned int.
	*  @param val  a value as unsigned int.
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toUInt32(unsigned int& val) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{
		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = OpcUa_String_StrSize((OpcUa_String*)&m_value.value.pszValue);
				//if ( strLen < 100 )
				//{
				//	unsigned long tempVal;
				//	UA_SetLastError(0);
				//	char strVal[100];
				//	strlcpy(strVal, OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue), sizeof(strVal) );
				//	char* sEnd = NULL;
				//	tempVal = strtoul(strVal, &sEnd, 10);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) ||
				//		(UA_GetLastError() != 0) ||
				//		(tempVal > unsigned int_Max) ||
				//		(strchr(strVal, '-') != OpcUa_Null) )
				//	{
				//		status = false;
				//	}
				//	else
				//	{
				//		val = (unsigned int)tempVal;
				//	}
				//}
				//else
				{
					status = false;
				}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			if (m_value.value.chValue < 0)
			{
				status = false;
			}
			else
			{
				val = (unsigned int)m_value.value.chValue;
			}
			break;
		case Kiwi_Int16:
			if (m_value.value.shValue < 0 )
			{
				status = false;
			}
			else
			{
				val = (unsigned int)m_value.value.shValue;
			}
			break;
		case Kiwi_Int32:
			if (m_value.value.nValue < 0)
			{
				status = false;
			}
			else
			{
				val = (unsigned int)m_value.value.nValue;
			}
			break;
		case Kiwi_Int64:
			if (m_value.value.llValue < 0 || m_value.value.llValue > UInt32_Max)
			{
				status = false;
			}
			else
			{
				val = (unsigned int)m_value.value.llValue;
			}
			break;
		case Kiwi_Byte:
			val = m_value.value.BValue;
			break;
		case Kiwi_UInt16:
			val = m_value.value.ushValue;
			break;
		case Kiwi_UInt32:
			val = m_value.value.unValue;
			break;
		case Kiwi_UInt64:
			if ( m_value.value.ullValue > UInt32_Max)
			{
				status = false;
			}
			else
			{
				val = (unsigned int)m_value.value.ullValue;
			}
			break;
		case Kiwi_Float:
			if (m_value.value.fValue < 0 || m_value.value.fValue > UInt32_Max)
			{
				status = false;
			}
			else
			{
				val = (unsigned int)(m_value.value.fValue+0.5); // rounding
			}
			break;
		case Kiwi_Double:
			if (m_value.value.dValue < 0 || m_value.value.dValue > UInt32_Max)
			{
				status = false;
			}
			else
			{
				val = (unsigned int)(m_value.value.dValue+0.5); // rounding
			}
			break;
		default:
			status = false;
		}

		return status;
	}

	bool CKiwiVariant::tolong(long& val) const
	{
		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = OpcUa_String_StrSize((OpcUa_String*)&m_value.value.pszValue);
				//if ( strLen < 30 )
				//{
				//	UA_SetLastError(0);
				//	char strVal[30];
				//	strlcpy(strVal, OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue), sizeof(strVal) );
				//	char* sEnd = NULL;
				//	val = strtoull(strVal, &sEnd, 10);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) || (UA_GetLastError() != 0) || (strchr(strVal, '-') != OpcUa_Null) )
				//	{
				//		status = false;
				//	}
				//}
				//else
				{
					status = false;
				}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			if (m_value.value.chValue < 0)
			{
				status = false;
			}
			else
			{
				val = (long)m_value.value.chValue;
			}
			break;
		case Kiwi_Int16:
			if (m_value.value.shValue < 0 )
			{
				status = false;
			}
			else
			{
				val = (long)m_value.value.shValue;
			}
			break;
		case Kiwi_Int32:
			if (m_value.value.nValue < 0)
			{
				status = false;
			}
			else
			{
				val = (long)m_value.value.nValue;
			}
			break;
		case Kiwi_Int64:
			if (m_value.value.llValue < 0)
			{
				status = false;
			}
			else
			{
				val = (long)m_value.value.llValue;
			}
			break;
		case Kiwi_Byte:
			val = m_value.value.BValue;
			break;
		case Kiwi_UInt16:
			val = m_value.value.ushValue;
			break;
		case Kiwi_UInt32:
			val = m_value.value.unValue;
			break;
		case Kiwi_UInt64:
			val = m_value.value.ullValue;
			break;
		case Kiwi_Float:
			if (m_value.value.fValue < 0 || m_value.value.fValue > (float)UInt64_Max)
			{
				status = false;
			}
			else
			{
				val = (long)(m_value.value.fValue+0.5); // rounding
			}
			break;
		case Kiwi_Double:
			if (m_value.value.dValue < 0 || m_value.value.dValue > (double)UInt64_Max)
			{
				status = false;
			}
			else
			{
				val = (long)(m_value.value.dValue+0.5); // rounding
			}
			break;
		default:
			status = false;
		}

		return status;		
	}
	/** Converts the CKiwiVariant into a UInt64.
	*  @param val a value as UInt64.
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toUInt64(UInt64& val) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{

		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = OpcUa_String_StrSize((OpcUa_String*)&m_value.value.pszValue);
				//if ( strLen < 30 )
				//{
				//	UA_SetLastError(0);
				//	char strVal[30];
				//	strlcpy(strVal, OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue), sizeof(strVal) );
				//	char* sEnd = NULL;
				//	val = strtoull(strVal, &sEnd, 10);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) || (UA_GetLastError() != 0) || (strchr(strVal, '-') != OpcUa_Null) )
				//	{
				//		status = false;
				//	}
				//}
				//else
				{
					status = false;
				}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			if (m_value.value.chValue < 0)
			{
				status = false;
			}
			else
			{
				val = (UInt64)m_value.value.chValue;
			}
			break;
		case Kiwi_Int16:
			if (m_value.value.shValue < 0 )
			{
				status = false;
			}
			else
			{
				val = (UInt64)m_value.value.shValue;
			}
			break;
		case Kiwi_Int32:
			if (m_value.value.nValue < 0)
			{
				status = false;
			}
			else
			{
				val = (UInt64)m_value.value.nValue;
			}
			break;
		case Kiwi_Int64:
			if (m_value.value.llValue < 0)
			{
				status = false;
			}
			else
			{
				val = (UInt64)m_value.value.llValue;
			}
			break;
		case Kiwi_Byte:
			val = m_value.value.BValue;
			break;
		case Kiwi_UInt16:
			val = m_value.value.ushValue;
			break;
		case Kiwi_UInt32:
			val = m_value.value.unValue;
			break;
		case Kiwi_UInt64:
			val = m_value.value.ullValue;
			break;
		case Kiwi_Float:
			if (m_value.value.fValue < 0 || m_value.value.fValue > (float)UInt64_Max)
			{
				status = false;
			}
			else
			{
				val = (UInt64)(m_value.value.fValue+0.5); // rounding
			}
			break;
		case Kiwi_Double:
			if (m_value.value.dValue < 0 || m_value.value.dValue > (double)UInt64_Max)
			{
				status = false;
			}
			else
			{
				val = (UInt64)(m_value.value.dValue+0.5); // rounding
			}
			break;
		default:
			status = false;
		}

		return status;
	}

	/** Converts the CKiwiVariant into a float.
	*  @param val a value as float.
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toFloat(float& val) const // parasoft-suppress  METRICS-28 "�������ܸ��ӣ�Ȧ���Ӷȸߣ����߼���ȷ"
	{

		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = strlen(m_value.value.pszValue);
				//if ( strLen < 100 )
				//{
				//	double tempVal;
				//	char strVal[100];
				//	strncpy(strVal, m_value.value.pszValue, sizeof(strVal) );
				//	char* sEnd = NULL;
				//	tempVal = strtod(strVal, &sEnd);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) ||
				//		(UA_GetLastError() != 0) ||
				//		( (fabs(tempVal) > float_Max) || ( (fabs(tempVal)<float_Min) && ( !AlmostEqual(tempVal, 0.0)/*tempVal!=0*/) ) ) )
				//	{
				//		status = false;
				//	}
				//	else
				//	{
				//		val = (float)tempVal;
				//	}
				//}
				//else
				//{
				//	status = false;
				//}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			val = m_value.value.chValue;
			break;
		case Kiwi_Int16:
			val = m_value.value.shValue;
			break;
		case Kiwi_Int32:
			val = (float)m_value.value.nValue;
			break;
		case Kiwi_Int64:
			val = (float)m_value.value.llValue;
			break;
		case Kiwi_Byte:
			val = m_value.value.BValue;
			break;
		case Kiwi_UInt16:
			val = m_value.value.ushValue;
			break;
		case Kiwi_UInt32:
			val = (float)m_value.value.unValue;
			break;
		case Kiwi_UInt64:
			// not supported by the compiler
			status = false;
			break;
		case Kiwi_Float:
			val = m_value.value.fValue;
			break;
		case Kiwi_Double:
			if ( ((fabs(m_value.value.dValue) < Float_Min)
				&& ( !AlmostEqual(m_value.value.dValue, 0)))
				||	(fabs(m_value.value.dValue) > Float_Max) )
			{
				status = false;
			}
			else
			{
				val = (float)m_value.value.dValue;
			}
			break;
		default:
			status = false;
		}

		return status;
	}

	/** Converts the CKiwiVariant into a double.
	*  @param val a value as double.
	*  @return the OPC UA Status code.
	*/
	bool CKiwiVariant::toDouble(double& val) const
	{
		bool status = true;

		switch(m_value.vt)
		{
		case Kiwi_String:
			{
				//unsigned int strLen = strlen(m_value.value.pszValue);
				//if ( strLen < 100 )
				//{
				//	double tempVal;
				//	UA_SetLastError(0);
				//	char strVal[100];
				//	strlcpy(strVal, OpcUa_String_GetRawString((OpcUa_String*)&m_value.value.pszValue), sizeof(strVal) );
				//	char* sEnd = NULL;
				//	tempVal = strtod(strVal, &sEnd);
				//	if ( ((sEnd != NULL) && (sEnd[0] != 0)) || (UA_GetLastError() != 0) )
				//	{
				//		status = false;
				//	}
				//	else
				//	{
				//		val = tempVal;
				//	}
				//}
				//else
				//{
				//	status = false;
				//}
				break;
			}
		case Kiwi_Boolean:
			val = m_value.value.bValue;
			break;
		case Kiwi_SByte:
			val = m_value.value.chValue;
			break;
		case Kiwi_Int16:
			val = m_value.value.shValue;
			break;
		case Kiwi_Int32:
			val = (double)m_value.value.nValue;
			break;
		case Kiwi_Int64:
			val = (double)m_value.value.llValue;
			break;
		case Kiwi_Byte:
			val = m_value.value.BValue;
			break;
		case Kiwi_UInt16:
			val = m_value.value.ushValue;
			break;
		case Kiwi_UInt32:
			val = (double)m_value.value.unValue;
			break;
		case Kiwi_UInt64:
			val = (double)m_value.value.ullValue;
			break;
		case Kiwi_Float:
			val = m_value.value.fValue;
			break;
		case Kiwi_Double:
			val = m_value.value.dValue;
			break;
		default:
			status = false;
		}

		return status;
	}



	//UaDataStream &operator<<(UaDataStream &out, const CKiwiVariant &val)
	//{
	//	// get variant data
	//	const T_Variant *pVariant = (const T_Variant*)val;

	//	// write variant
	//	out << *pVariant;

	//	return out;
	//}
	//UaDataStream &operator>>(UaDataStream &in, CKiwiVariant &val)
	//{
	//	T_Variant variant;
	//	T_Variant_Initialize(&variant);

	//	// read variant
	//	in >> variant;
	//	val.attach(&variant);
	//	return in;
	//}

}
