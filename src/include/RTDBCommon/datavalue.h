/******************************************************************************
** uadatavalue.h
**
** Copyright (c) 2006-2014 Unified Automation GmbH All rights reserved.
**
** Software License Agreement ("SLA") Version 2.3
**
** Unless explicitly acquired and licensed from Licensor under another
** license, the contents of this file are subject to the Software License
** Agreement ("SLA") Version 2.3, or subsequent versions
** as allowed by the SLA, and You may not copy or use this file in either
** source code or executable form, except in compliance with the terms and
** conditions of the SLA.
**
** All software distributed under the SLA is provided strictly on an
** "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
** AND LICENSOR HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING WITHOUT
** LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
** PURPOSE, QUIET ENJOYMENT, OR NON-INFRINGEMENT. See the SLA for specific
** language governing rights and limitations under the SLA.
**
** The complete license agreement can be found here:
** http://unifiedautomation.com/License/SLA/2.3/
**
** Project: C++ OPC SDK base module
**
** Portable Ua DataValue class.
**
******************************************************************************/
#ifndef UADATAVALUE_H
#define UADATAVALUE_H
#include "KiwiTypes.h"
namespace MACS_SCADA_SUD
{


/** @ingroup CppBaseLibraryClass
 *  @brief Wrapper class for the UA stack structure T_DataValue.
 *
 *  This class encapsulates the native T_DataValue structure
 *  and handles memory allocation and cleanup for you.
 *  UaDataValue uses implicit sharing to avoid needless copying and to boost the performance.
 *  Only if you modify a shared DataValue it creates a copy for that (copy-on-write).
 *  So assigning another UaDataValue or passing it as parameter needs constant time is nearly as fast as assigning a pointer.
 */
class COMMONBASECODE_API UaDataValue
{
public:
    UaDataValue();
    UaDataValue(const UaDataValue &other);
    UaDataValue(const CKiwiVariant& value, unsigned char statusCode, const long& sourceTimestamp);
    ~UaDataValue();
    void clear();

    UaDataValue& operator=(const UaDataValue &other);

    bool operator==(const UaDataValue &other) const;
    bool operator!=(const UaDataValue &other) const;

    operator const T_DataValue*() const;

    T_DataValue* copy() const;
    void copyTo(T_DataValue *pDst) const;

    static T_DataValue* clone(const T_DataValue& source);
    static void cloneTo(const T_DataValue& source, T_DataValue& copy);

    void setDataValue(CKiwiVariant& value, unsigned char statusCode, long sourceTimestamp);
    void setDataValue(T_Variant& value, unsigned char statusCode, long sourceTimestamp);

    void setValue( CKiwiVariant& value, bool updateTimeStamps = false );
    void setValue( T_Variant& value, bool updateTimeStamps = false );

    void setStatusCode( unsigned char statusCode );
    void setSourceTimestamp( long sourceTimestamp );

    const T_Variant*    value() const;
    unsigned char       statusCode() const;
    long                sourceTimestamp() const;

    bool  isSourceTimestampSet() const;

    static bool compare(const T_DataValue&, const T_DataValue&, bool bCompareTimestamps);
    static unsigned char checkDeadbandExceeded(const UaDataValue &lastDataValue,
         const UaDataValue &newDataValue, double absoluteDeadband, bool &bDeadbandExceeded);

private:
    T_DataValue* d_ptr;

    // friend COMMONBASECODE_API UaDataStream &operator<<(UaDataStream &, const UaDataValue &);
    // friend COMMONBASECODE_API UaDataStream &operator>>(UaDataStream &, UaDataValue &);
};

// // define UaDataStream operators for UaDataValue
// COMMONBASECODE_API UaDataStream &operator<<(UaDataStream &, const UaDataValue &);
// COMMONBASECODE_API UaDataStream &operator>>(UaDataStream &, UaDataValue &);

// /** Object array of UaDataValue object. */
// typedef UaObjectArray<UaDataValue>      UaDataValueArray;
};

#endif // UADATAVALUE_H
