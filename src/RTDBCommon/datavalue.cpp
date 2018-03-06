/******************************************************************************
** uadatavalue.cpp
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
#include "RTDBCommon/datetime.h"
#include "RTDBCommon/datavalue.h"
#include <string.h>

using namespace MACS_SCADA_SUD;


void T_DataValue_Initialize(T_DataValue* a_pValue)
{

    if (a_pValue == NULL)
    {
        return;
    }
    memset(a_pValue, 0, sizeof(T_DataValue));
}

/** Constructs a UaDataValue with default settings */
UaDataValue::UaDataValue()
	:d_ptr(NULL)
{
	this->d_ptr = new T_DataValue;
	T_DataValue_Initialize(d_ptr);
}

/** Constructs a UaDataValue with another UaDataValue
 */
UaDataValue::UaDataValue(const UaDataValue &other)  //!< [in] the other UaDataValue.
{
	this->d_ptr = new T_DataValue;
	T_DataValue_Initialize(this->d_ptr);
	memcpy(this->d_ptr, other.copy(), sizeof(T_DataValue));
}

/** Constructs a UaDataValue with a list of parameters
 */
UaDataValue::UaDataValue(
    const CKiwiVariant& value,                 //!< [in] the value of a CKiwiVariant.
    unsigned char statusCode,            //!< [in] the OPC UA Status code.
    const long& sourceTimestamp)      //!< [in] the timestamp of the server.
{
}

/** Destroys the UaDataValue object */
UaDataValue::~UaDataValue()
{
    if(d_ptr)
        delete d_ptr;
    d_ptr = NULL;
}

/** Clear the data of the UaDataValue. */
void UaDataValue::clear()
{
    if(d_ptr)
    {
       //memset(d_ptr, 0, size(T_DataValue));
    }
}

/** Assignment operator.
 *  @return the reference to itself.
 */
UaDataValue& UaDataValue::operator=(const UaDataValue &other)   //!< [in] the other UaDataValue to assign.
{
    if(!d_ptr)
    {   
        d_ptr = new T_DataValue;
        T_DataValue_Initialize(d_ptr);
    }
    
    //this->d_ptr = clone(other);
    return *this;
}

/** Compare variables if they are similar.
 *  @return true if not similar false if similar.
 */
bool UaDataValue::operator==(
    const UaDataValue &other   //!< [in] the other UaDataValue to compare.
    ) const
{
    if (other.d_ptr == d_ptr)
    {
        return true;
    }
    else
    {
        return compare(*d_ptr, *other.d_ptr, false);
    }
}

/** Returns true if other is not equal to this.
 *  @see operator==
 *  @return true if other is not equal to this.
 */
bool UaDataValue::operator!=(
    const UaDataValue &other    //!< [in] the other UaDataValue to compare.
    ) const
{
    return !(operator==(other));
}

/** Return internal T_DataValue structure.
 *  @return internal T_DataValue structure.
 */
UaDataValue::operator const T_DataValue*() const
{
    return (const T_DataValue*)d_ptr;
}

/** Copy UaDataValue data to a newly allocated T_DataValue.
 *  @return a newly allocated T_DataValue.
 */
T_DataValue* UaDataValue::copy() const
{
    return clone(*d_ptr);
}

/** Copy UaDataValue data to an existing T_DataValue structure.
 */
void UaDataValue::copyTo(
    T_DataValue *pDst   //!< [in/out] the destination of this operation.
    ) const
{
    cloneTo(*d_ptr, *pDst);
}

/** Copy T_DataValue data to a newly allocated T_DataValue.
 *  @return a newly allocated T_DataValue.
 */
T_DataValue* UaDataValue::clone(const T_DataValue& source)  //!< [in] the source of this operation.
{
   T_DataValue* pUaDataValue = new T_DataValue;
        
    if (pUaDataValue)
    {
        cloneTo(source, *pUaDataValue);
    }
    return pUaDataValue;
}

/** Copy T_DataValue data to an existing T_DataValue structure.
 */
void UaDataValue::cloneTo(
    const T_DataValue& source,  //!< [in] Source OpcUa_VariableTypeAttributes to clone.
    T_DataValue& copy)          //!< [in/out] Copy of source OpcUa_VariableTypeAttributes.
{
    T_DataValue_Initialize(&copy);

    copy.StatusCode        = source.StatusCode;
    copy.SourceTimestamp   = source.SourceTimestamp;
    
    CKiwiVariant::cloneTo(source.Value, copy.Value);
}


/** Sets all fields of the data value.
 */
void UaDataValue::setDataValue(
    CKiwiVariant&        value,                //!< [in] The value to set as CKiwiVariant
     unsigned char  statusCode,           //!< [in] The status code to set
    long    sourceTimestamp)      //!< [in] The server timestamp
{

}

/** Sets all fields of the data value.
 */
void UaDataValue::setDataValue(
    T_Variant&    value,           //!< [in] The value to set as OpcUa_Variant
    unsigned char  statusCode,      //!< [in] The status code to set
    long    sourceTimestamp) //!< [in] The server timestamp
{
    
}

/** Sets the value field of the data value and provides functionality to update the timestamp fields.
 */
void UaDataValue::setValue(
    CKiwiVariant& value,                   //!< [in] The value to set as UaVariant
    bool updateTimeStamps )    //!< [in] Flag indicating if the timestamp fields should be updated
{

    T_Variant_Clear(&d_ptr->Value);
    value.copyTo(&d_ptr->Value);

    d_ptr->StatusCode = 0;
    if ( updateTimeStamps != false )
    {
        d_ptr->SourceTimestamp = time(NULL);
    }
}

/** Sets the value field of the data value and provides functionality to update the timestamp fields.
 */
void UaDataValue::setValue(
    T_Variant& value,               //!< [in] The value to set as UaVariant
    bool updateTimeStamps )    //!< [in] Flag indicating if the timestamp fields should be updated
{
    T_Variant_Clear(&d_ptr->Value);

    CKiwiVariant::cloneTo(value, d_ptr->Value);

    d_ptr->StatusCode = 0;
    if ( updateTimeStamps != false )
    {
        d_ptr->SourceTimestamp = ::time(NULL);
    }
}

/** Set the status code.
 */
void UaDataValue::setStatusCode( unsigned char statusCode )  //!< [in] the OPC UA Status code.
{
    d_ptr->StatusCode = statusCode;
}

/** Sets the source timestamp.
 */
void UaDataValue::setSourceTimestamp( long sourceTimestamp )  //!< [in] The source timestamp to set.
{
    d_ptr->SourceTimestamp = sourceTimestamp;
}

/** Get the value of a variable.
 *  @return the value of a variable.
 */
const T_Variant* UaDataValue::value() const
{
    return &d_ptr->Value;
}

/* Get the status code value */
unsigned char UaDataValue::statusCode() const
{
    return d_ptr->StatusCode;
}

/** Get the source timestamp.
 *  @return the source timestamp.
 */
long UaDataValue::sourceTimestamp() const
{
    return d_ptr->SourceTimestamp;
}


/** Compares two DataValues and returns true if they are equal.
 *  @see operator==
 */
bool UaDataValue::compare(
    const T_DataValue &dataValue1,  //!< [in] The source to compare to.
    const T_DataValue &dataValue2,  //!< [in] The destination to compare to.
    bool bCompareTimestamps)   //!< [in] Flag indicating if the timestamp part of the data value should be included in the compare (True) or ignored (False).
{
    if (dataValue1.StatusCode != dataValue2.StatusCode) return false;

    if (bCompareTimestamps != false)
    {
        if (dataValue1.SourceTimestamp != dataValue2.SourceTimestamp) return false;
    }

    return CKiwiVariant::compare(dataValue1.Value, dataValue2.Value);
}

/** Calculate if the deadband is exceeded.
 */
unsigned char UaDataValue::checkDeadbandExceeded( 
    const UaDataValue &lastDataValue,
    const UaDataValue &newDataValue,
    double absoluteDeadband,
    bool &bDeadbandExceeded)
{
    bDeadbandExceeded = false;

    // // status code is different
    // if ( lasd_ptr->statusCode() != newDataValue.statusCode())
    // {
    //     bDeadbandExceeded = true;
    //     return 0;
    // }

    // // apply deadband filter
    // if ( absoluteDeadband > 0 )
    // {
    //     // datatype is different or array type is different
    //     if ( lasd_ptr->value()->Datatype != newDataValue.value()->Datatype ||
    //         lasd_ptr->value()->ArrayType != newDataValue.value()->ArrayType )
    //     {
    //         bDeadbandExceeded = true;
    //         return 0;
    //     }

    //     // scalar type
    //     if ( newDataValue.value()->ArrayType == OpcUa_VariantArrayType_Scalar )
    //     {
    //         double oldValue = 0;
    //         double newValue = double_Max;

    //         // numeric types
    //         if ( newDataValue.value()->Datatype > OpcUaType_Boolean && newDataValue.value()->Datatype < OpcUaType_String )
    //         {
    //             CKiwiVariant variantTmp(*lasd_ptr->value());

    //             if (OpcUa_IsBad(variantTmp.toDouble(oldValue)))
    //             {
    //                 return OpcUa_BadTypeMismatch;
    //             }

    //             variantTmp = CKiwiVariant(*newDataValue.value());

    //             if ( fabs(oldValue - newValue) > absoluteDeadband )
    //             {
    //                 // Absolute deadband was exeeded
    //                 bDeadbandExceeded = true;
    //             }

    //             return 0;
    //         }
    //         else
    //         {
    //             return OpcUa_BadTypeMismatch;
    //         }
    //     }
    //     // array type
    //     else if ( newDataValue.value()->ArrayType == OpcUa_VariantArrayType_Array )
    //     {
    //         // if the size changed the deadband is exceeded
    //         if (newDataValue.value()->Value.Array.Length != lasd_ptr->value()->Value.Array.Length)
    //         {
    //             bDeadbandExceeded = true;
    //             return 0;
    //         }

    //         if ( newDataValue.value()->Datatype > OpcUaType_Boolean && newDataValue.value()->Datatype < OpcUaType_String )
    //         {
    //             // check each element
    //             int i;
    //             CKiwiVariant variantTmp;

    //             for ( i = 0; i < newDataValue.value()->Value.Array.Length; i++ )
    //             {
    //                 double oldValue = 0;
    //                 double newValue = double_Max;

    //                 switch(newDataValue.value()->Datatype)
    //                 {
    //                 case OpcUaType_Double:
    //                     oldValue = lasd_ptr->value()->Value.Array.Value.DoubleArray[i];
    //                     newValue = newDataValue.value()->Value.Array.Value.DoubleArray[i];
    //                     break;
    //                 case OpcUaType_Float:
    //                     oldValue = (double)lasd_ptr->value()->Value.Array.Value.FloatArray[i];
    //                     newValue = (double)newDataValue.value()->Value.Array.Value.FloatArray[i];
    //                     break;
    //                 case OpcUaType_SByte:
    //                     oldValue = (double)lasd_ptr->value()->Value.Array.Value.SByteArray[i];
    //                     newValue = (double)newDataValue.value()->Value.Array.Value.SByteArray[i];
    //                     break;
    //                 case OpcUaType_Byte:
    //                     oldValue = (double)lasd_ptr->value()->Value.Array.Value.ByteArray[i];
    //                     newValue = (double)newDataValue.value()->Value.Array.Value.ByteArray[i];
    //                     break;
    //                 case OpcUaType_Int16:
    //                     oldValue = (double)lasd_ptr->value()->Value.Array.Value.Int16Array[i];
    //                     newValue = (double)newDataValue.value()->Value.Array.Value.Int16Array[i];
    //                     break;
    //                 case OpcUaType_UInt16:
    //                     oldValue = (double)lasd_ptr->value()->Value.Array.Value.UInt16Array[i];
    //                     newValue = (double)newDataValue.value()->Value.Array.Value.UInt16Array[i];
    //                     break;
    //                 case OpcUaType_Int32:
    //                     oldValue = (double)lasd_ptr->value()->Value.Array.Value.Int32Array[i];
    //                     newValue = (double)newDataValue.value()->Value.Array.Value.Int32Array[i];
    //                     break;
    //                 case OpcUaType_UInt32:
    //                     oldValue = (double)lasd_ptr->value()->Value.Array.Value.UInt32Array[i];
    //                     newValue = (double)newDataValue.value()->Value.Array.Value.UInt32Array[i];
    //                     break;
    //                 case OpcUaType_Int64:
    //                     oldValue = (double)lasd_ptr->value()->Value.Array.Value.Int64Array[i];
    //                     newValue = (double)newDataValue.value()->Value.Array.Value.Int64Array[i];
    //                     break;
    //                 case OpcUaType_UInt64:
    //                     oldValue = (double)lasd_ptr->value()->Value.Array.Value.UInt64Array[i];
    //                     newValue = (double)newDataValue.value()->Value.Array.Value.UInt64Array[i];
    //                     break;
    //                 }

    //                 if ( fabs(oldValue - newValue) > absoluteDeadband )
    //                 {
    //                     // Absolute deadband was exeeded
    //                     bDeadbandExceeded = true;
    //                     break;
    //                 }
    //             }

    //             return 0;
    //         }
    //         else
    //         {
    //             return OpcUa_BadTypeMismatch;
    //         }
    //     }
    //     // matrix
    //     else
    //     {
    //         // if dimensions changed the deadband is exceeded
    //         if ( newDataValue.value()->Value.Matrix.NoOfDimensions != lasd_ptr->value()->Value.Matrix.NoOfDimensions )
    //         {
    //             bDeadbandExceeded = true;
    //             return 0;
    //         }

    //         // check each dimension
    //         int i;
    //         int index = 0;

    //         if ( newDataValue.value()->Datatype > OpcUaType_Boolean && newDataValue.value()->Datatype < OpcUaType_String )
    //         {
    //             for ( i = 0; i < newDataValue.value()->Value.Matrix.NoOfDimensions; i++)
    //             {
    //                 // if size changed the deadband is exceeded
    //                 if ( newDataValue.value()->Value.Matrix.Dimensions[i] != lasd_ptr->value()->Value.Matrix.Dimensions[i] )
    //                 {
    //                     bDeadbandExceeded = true;
    //                     return 0;
    //                 }

    //                 // check each element
    //                 int j;
    //                 for ( j = 0; j < newDataValue.value()->Value.Matrix.Dimensions[i]; j++ )
    //                 {
    //                     double oldValue = 0;
    //                     double newValue = double_Max;

    //                     switch(newDataValue.value()->Datatype)
    //                     {
    //                     case OpcUaType_Double:
    //                         oldValue = lasd_ptr->value()->Value.Matrix.Value.DoubleArray[index];
    //                         newValue = newDataValue.value()->Value.Matrix.Value.DoubleArray[index];
    //                         break;
    //                     case OpcUaType_Float:
    //                         oldValue = (double)lasd_ptr->value()->Value.Matrix.Value.FloatArray[index];
    //                         newValue = (double)newDataValue.value()->Value.Matrix.Value.FloatArray[index];
    //                         break;
    //                     case OpcUaType_SByte:
    //                         oldValue = (double)lasd_ptr->value()->Value.Matrix.Value.SByteArray[index];
    //                         newValue = (double)newDataValue.value()->Value.Matrix.Value.SByteArray[index];
    //                         break;
    //                     case OpcUaType_Byte:
    //                         oldValue = (double)lasd_ptr->value()->Value.Matrix.Value.ByteArray[index];
    //                         newValue = (double)newDataValue.value()->Value.Matrix.Value.ByteArray[index];
    //                         break;
    //                     case OpcUaType_Int16:
    //                         oldValue = (double)lasd_ptr->value()->Value.Matrix.Value.Int16Array[index];
    //                         newValue = (double)newDataValue.value()->Value.Matrix.Value.Int16Array[index];
    //                         break;
    //                     case OpcUaType_UInt16:
    //                         oldValue = (double)lasd_ptr->value()->Value.Matrix.Value.UInt16Array[index];
    //                         newValue = (double)newDataValue.value()->Value.Matrix.Value.UInt16Array[index];
    //                         break;
    //                     case OpcUaType_Int32:
    //                         oldValue = (double)lasd_ptr->value()->Value.Matrix.Value.Int32Array[index];
    //                         newValue = (double)newDataValue.value()->Value.Matrix.Value.Int32Array[index];
    //                         break;
    //                     case OpcUaType_UInt32:
    //                         oldValue = (double)lasd_ptr->value()->Value.Matrix.Value.UInt32Array[index];
    //                         newValue = (double)newDataValue.value()->Value.Matrix.Value.UInt32Array[index];
    //                         break;
    //                     case OpcUaType_Int64:
    //                         oldValue = (double)lasd_ptr->value()->Value.Matrix.Value.Int64Array[index];
    //                         newValue = (double)newDataValue.value()->Value.Matrix.Value.Int64Array[index];
    //                         break;
    //                     case OpcUaType_UInt64:
    //                         oldValue = (double)lasd_ptr->value()->Value.Matrix.Value.UInt64Array[index];
    //                         newValue = (double)newDataValue.value()->Value.Matrix.Value.UInt64Array[index];
    //                         break;
    //                     }

    //                     index++;

    //                     if ( fabs(oldValue - newValue) > absoluteDeadband )
    //                     {
    //                         // Absolute deadband was exeeded
    //                         bDeadbandExceeded = true;
    //                         return 0;
    //                     }
    //                 }
    //             }
    //         }
    //         else
    //         {
    //             return OpcUa_BadTypeMismatch;
    //         }
    //     }
    // }
    // else
    // {
    //     bDeadbandExceeded = true;
    // }
    return 0;
}

// UaDataStream &operator<<(UaDataStream &out, const UaDataValue &val)
// {
//     // get internal data
//     const T_DataValue* dataValue = (const T_DataValue*)val;

//     // write DataValue
//     out << *dataValue;

//     return out;
// }

// UaDataStream &operator>>(UaDataStream &in, UaDataValue &val)
// {
//     T_DataValue dataValue;
//     T_DataValue_Initialize(&dataValue);

//     // read DataValue
//     in >> dataValue;
//     val.attach(&dataValue);
//     return in;
// }
