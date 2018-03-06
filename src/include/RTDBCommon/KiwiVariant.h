#ifndef  _KIWI_VARIANT_
#define _KIWI_VARIANT_

#include <string.h>
#include <math.h>
#include <iostream>
#include "RTDBCommon/OS_Ext.h"


#define SByte_Min     -128
#define SByte_Max     127
#define Byte_Min      0
#define Byte_Max      255
#define Int16_Min     -32768
#define Int16_Max     32767
#define UInt16_Min    0
#define UInt16_Max    65535
#define Int32_Min     -2147483647L-1
#define Int32_Max     2147483647L
#define UInt32_Min    0UL
#define UInt32_Max    4294967295UL
#ifdef _MSC_VER
#define Int64_Min     (-9223372036854775807i64-1)
#define Int64_Max     9223372036854775807i64
#define UInt64_Min    0
#define UInt64_Max    18446744073709551615ui64
#else /* _MSC_VER */
#define Int64_Min     -9223372036854775807ll-1
#define Int64_Max     9223372036854775807ll
#define UInt64_Min    0
#define UInt64_Max    18446744073709551615ull
#endif /* _MSC_VER */
	/* defined as FLT_MIN in "%ProgramFiles\Microsoft Visual Studio 8\VC\include\float.h" */
	/* #define FLT_MIN         1.175494351e-38F */
#define Float_Min     (float)1.175494351e-38F
	/* defined as FLT_MAX in "%ProgramFiles\Microsoft Visual Studio 8\VC\include\float.h" */
	/* #define FLT_MAX         3.402823466e+38F */
#define Float_Max     (float)3.402823466e+38F
	/* defined as DBL_MIN in "%ProgramFiles\Microsoft Visual Studio 8\VC\include\float.h" */
	/* #define DBL_MIN         2.2250738585072014e-308 */
#define Double_Min    (double)2.2250738585072014e-308
	/* defined as DBL_MAX in "%ProgramFiles\Microsoft Visual Studio 8\VC\include\float.h" */
	/* #define DBL_MAX         1.7976931348623158e+308 */
#define Double_Max    (double)1.7976931348623158e+308

namespace MACS_SCADA_SUD
{

	/* A union that contains one of the built in types. */
	typedef enum _KiwiDataType
	{
		Kiwi_Null = 0,
		Kiwi_Boolean = 1,
		Kiwi_SByte = 2,
		Kiwi_Byte = 3,
		Kiwi_Int16 = 4,
		Kiwi_UInt16 = 5,
		Kiwi_Int32 = 6,
		Kiwi_UInt32 = 7,
		Kiwi_Int64 = 8,
		Kiwi_UInt64 = 9,
		Kiwi_Float = 10,
		Kiwi_Double = 11,
		Kiwi_String = 12,
		Kiwi_Variant = 13,
		Kiwi_DataTime = 13,
	}
	Kiwi_BuiltInType;

#ifdef _MSC_VER
	typedef __int64             Int64;
	typedef unsigned __int64    UInt64;
#else /* _MSC_VER */
	typedef long long           Int64;
	typedef unsigned long long  UInt64;
#endif /* _MSC_VER */



	typedef union _VariantUnion
	{
		bool					bValue;
		char					chValue;
		unsigned char           BValue;
		short 				    shValue;
		unsigned short          ushValue;
		int						nValue;
		unsigned int            unValue;
		long					lValue;
		unsigned long			ulValue;
		Int64					llValue;
		UInt64					ullValue;
		float					fValue;
		double					dValue;
		char*					pszValue;
	}
	VariantUnion;

	typedef struct _t_Variant
	{
		unsigned int vt;
		unsigned int nLen;
		VariantUnion value;
	}T_Variant;

	void T_Variant_Initialize(T_Variant* a_pValue);
	void T_Variant_Clear(T_Variant* a_pValue);
	bool AlmostEqual(const float& fValue1, const float& fValue2);
	bool AlmostEqual(const double& fValue1, const double& fValue2);


	class COMMONBASECODE_API CKiwiVariant
	{
	public:
		CKiwiVariant(void);
		CKiwiVariant(const CKiwiVariant&);
		CKiwiVariant(const T_Variant&);
		CKiwiVariant(const std::string&);
		CKiwiVariant(T_Variant* pValue, bool detachValue = false);
		CKiwiVariant(bool);
		CKiwiVariant(char);
		CKiwiVariant(unsigned char);
		CKiwiVariant(int);
		CKiwiVariant(unsigned int);
		CKiwiVariant(Int64);
		CKiwiVariant(short int);
		CKiwiVariant(unsigned short int);
		CKiwiVariant(UInt64);
		CKiwiVariant(float);
		CKiwiVariant(double);
		CKiwiVariant(const char*);
		~CKiwiVariant();

		void     clear();
		bool     isEmpty() const;

		Kiwi_BuiltInType type() const;

		CKiwiVariant& operator=(const T_Variant&);
		CKiwiVariant& operator=(const CKiwiVariant&);
		CKiwiVariant& operator=(double);
		CKiwiVariant& operator=(const std::string&);

		bool operator==(const CKiwiVariant&) const;
		bool operator!=(const CKiwiVariant&) const;
		static bool compare(const T_Variant&, const T_Variant&);
		static void adjustTypes(CKiwiVariant& value1, CKiwiVariant& value2, bool bForceConversion = false);

		bool operator>(const CKiwiVariant&) const;
		bool operator<(const CKiwiVariant&) const;
		bool operator>=(const CKiwiVariant&) const;
		bool operator<=(const CKiwiVariant&) const;

		operator const T_Variant*() const;

		void detach();
		void attach(const T_Variant *pSrc);
		//�˷������ú���Ҫ�ͷ�
		T_Variant* copy() const;
		void copyTo(T_Variant *pDst) const;

		static T_Variant* clone(const T_Variant& source);
		static void cloneTo(const T_Variant& source, T_Variant& copy);

		void setBool(bool val);
		void setChar(char val);
		void setInt16(short val);
		void setInt32(int val);
		void setInt64(Int64 val);
		void setByte(unsigned char val);
		void setUInt16(unsigned short int val);
		void setUInt32(unsigned int val);
		void setUInt64(UInt64 val);
		void setFloat(float val);
		void setDouble(double val);
		void setString(const std::string& val);

		bool toBool(bool& val) const;
		bool toChar(char& val) const;
		bool toInt16(short& val) const;
		bool toInt32(int& val) const;
		bool toInt64(Int64& val) const;
		bool toByte(unsigned char & val) const;
		bool toUInt16(unsigned short & val) const;
		bool toUInt32(unsigned int & val) const;
		bool tolong(long& val) const;
		bool toUInt64(UInt64& val) const;
		bool toFloat(float& val) const;
		bool toDouble(double& val) const;

		std::string toString() const;

	private:
		T_Variant m_value;

		//friend iostream &operator<<(UaDataStream &, const CKiwiVariant &);
		//friend iostream UaDataStream &operator>>(UaDataStream &, CKiwiVariant &);
	};

};

#endif
