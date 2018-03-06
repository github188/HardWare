#ifndef _KIWITYPES_20080430_H_
#define _KIWITYPES_20080430_H_

#include "OS_Ext.h"
#include "KiwiVariant.h"

using namespace MACS_SCADA_SUD;

/*============================================================================
 * The T_DataTime type
 *===========================================================================*/
typedef struct _T_DataValue
{
    T_Variant    Value;
    unsigned char StatusCode;
    long   SourceTimestamp;
}
T_DataValue;


void T_DataValue_Initialize(T_DataValue* a_pValue);

/*============================================================================
 * The T_DataTime type
 *===========================================================================*/
typedef struct _t_DateTime
{
    uint dwLowDateTime;
    uint dwHighDateTime;
}
T_DataTime;  

#endif