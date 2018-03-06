/******************************************************************************
** uadatetime.cpp
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
******************************************************************************/
#include "RTDBCommon/datetime.h"
#include "RTDBCommon/Kiwivariant.h"
#include "RTDBCommon/datavalue.h"

using namespace MACS_SCADA_SUD;

/** Creates a UaDateTime object initialized with a null value.
*/
UaDateTime::UaDateTime()
{
    memset(&m_dateTime, 0, sizeof(T_DataTime));
}

/** Creates a UaDateTime object initialized with the passed T_DataTime structure.
 *
 * @param dateTime the T_DataTime structure used to initialize the UaDateTime object
 */
UaDateTime::UaDateTime(const T_DataTime &dateTime)
{
    m_dateTime = dateTime;
}

#ifdef _UA_WIN32

/** Creates a UaDateTime object initialized with the passed Windows FILETIME value.
 *
 * This constructor is only available on Windows.
 *
 * @param fileTime  The initial date time as FILETIME.
 */
UaDateTime::UaDateTime(const FILETIME &fileTime)
{
    m_dateTime.dwLowDateTime  = fileTime.dwLowDateTime;
    m_dateTime.dwHighDateTime = fileTime.dwHighDateTime;
}

/** Assign a new time using Windows FILETIME value.
 *
 * This operator is only available on Windows.
 *
 * @param fileTime The time as FILETIME.
 * @return the date time object.
 */
UaDateTime& UaDateTime::operator=(const FILETIME &fileTime)
{
    m_dateTime.dwLowDateTime  = fileTime.dwLowDateTime;
    m_dateTime.dwHighDateTime = fileTime.dwHighDateTime;
    return *this;
}

/** Get the time as Windows FILETIME value.
 *
 * This operator is only available on Windows.
 *
 * @return the time as Windows FILETIME.
 */
UaDateTime::operator FILETIME() const
{
    FILETIME ret;
    ret.dwLowDateTime = m_dateTime.dwLowDateTime;
    ret.dwHighDateTime = m_dateTime.dwHighDateTime;
    return ret;
}
#endif

/** Creates a UaDateTime object initialized with the passed int64_t value.
 *
 * @param time  The time as int64_t.
 */
UaDateTime::UaDateTime(const Int64 &timedata)
{
    m_dateTime.dwLowDateTime = (uint)(timedata & 0xFFFFFFFF);
    m_dateTime.dwHighDateTime = (uint)(timedata >> 32);
}

/** Assign a new time using a int64_t value.
 *
 * @param time The time as int64_t.
 * @return the date time.
 */
UaDateTime& UaDateTime::operator=(const UInt64 &timedata)
{
    m_dateTime.dwLowDateTime = (uint)(timedata & 0xFFFFFFFF);
    m_dateTime.dwHighDateTime = (uint)(timedata >> 32);
    return *this;
}

/** Get the time as int64_t value.
 *
 * @return the int64_t.
 */
// UaDateTime::operator int64_t() const
// {
//     return (int64_t)(((int64_t)m_dateTime.dwHighDateTime << 32) + m_dateTime.dwLowDateTime);
// }

/** Destroys the UaDateTime object.
*/
UaDateTime::~UaDateTime()
{
}

/** Get the time as T_DataTime structure.
 *
 * @return the UaDateTime.
 */
UaDateTime::operator T_DataTime() const
{
    return m_dateTime;
}

/** Returns a UaDateTime object representing the current date and time.
 *
 * @code
 * UaDateTime currentTime;
 * currentTime = UaDateTime::now();
 * @endcode
 *
 * @return the date and the time of now.
 */
UaDateTime UaDateTime::now()
{
    // return T_DataTime_UtcNow();
    UaDateTime datetime;
    return datetime;
}

/** Get UaDateTime from string in ISO 8601 format.
 *
 * @code
 * UaDateTime myTime;
 * std::string sTime("2012-05-30T09:30:10Z");
 * myTime = UaDateTime::fromString(sTime);
 * @endcode
 *
 * ISO 8601 string format is specified in the following form "YYYY-MM-DDThh:mm:ss" where:
 *
 * Placeholder | Description
 * -------------|--------------------------
 * YYYY         | indicates the year
 * MM           | indicates the month
 * DD           | indicates the day
 * T            | indicates the start of the required time section
 * hh           | indicates the hour
 * mm           | indicates the minute
 * ss           | indicates the second
 *
 * Example: 2002-05-30T09:00:00
 *
 * \note
 * All components are required!
 *
 * <b>Time Zones:</b><br>
 * To specify a time zone, you can either enter a dateTime in UTC time by
 * adding a "Z" behind the time: 2012-05-30T09:30:10<b>Z</b><br>
 * or you can specify an offset from the UTC time by adding a positive or
 * negative time behind the time: 2012-05-30T09:30:10<b>-06:00</b> or
 * 2012-05-30T09:30:10<b>+06:00</b>
 *
 * @param sTime the string to convert.
 * @return UaDateTime object initialized from the string.
 */
UaDateTime UaDateTime::fromString(const std::string& sTime)
{
    UaDateTime     ret;
    T_DataTime tempTime;

    // if ( OpcUa_IsGood(T_DataTime_GetDateTimeFromString( (char*)sTime.toUtf8(), &tempTime) ) )
    // {
    //     ret = tempTime;
    // }
    return ret;
}

/** Get UaDateTime from time_t.
 *
 * @code
 * UaDateTime myTime;
 * time_t tt = 1393398000; // Wed, 26 Feb 2014 07:00:00 GMT
 * myTime = UaDateTime::fromTime_t(tt);
 * @endcode
 *
 * @param tim the time_t struct used to initialize the UaDateTime object
 * @return UaDateTime object initialized from time_t.
 */
UaDateTime UaDateTime::fromTime_t(time_t tim)
{
    Int64 unixtime = 0;
    T_DataTime dateTime;

    unixtime = tim;
    // unixtime += UA_SECS_BETWEEN_EPOCHS;
    // unixtime *= UA_SECS_TO_100NS;

    dateTime.dwHighDateTime = (uint)(unixtime >> 32);
    dateTime.dwLowDateTime  = (uint)(unixtime & 0xffffffff);

    return dateTime;
}


/** Converts the UaDateTime value to UNIX timestamp format time_t.
 *
 * \note This format only has a precision of seconds. The milliseconds part will be cut.
 *
 * The time range is different as well and goes from 1970-01-01 till 2038-01-18 on 32&nbsp;bit systems.
 *
 * @return UaDateTime value in UNIX timestamp format time_t.
 */
time_t UaDateTime::toTime_t() const
{
    // OpcUa_Uint64_t unixtime = ((OpcUa_Uint64_t)m_dateTime.dwHighDateTime << 32) + m_dateTime.dwLowDateTime;
     time_t tTime;

    // if ( unixtime <= (UA_SECS_BETWEEN_EPOCHS * UA_SECS_TO_100NS) )
    // {
    //     return 0;
    // }

    // unixtime -= UA_SECS_BETWEEN_EPOCHS * UA_SECS_TO_100NS;
    // tTime = (time_t) (unixtime / UA_SECS_TO_100NS);

    return tTime;
}


/** Returns the milliseconds part of the UaDateTime object.
 *
 * @return the milliseconds part of this dateTime.
 */
int UaDateTime::msec() const
{
    Int64 unixtime = ((Int64)m_dateTime.dwHighDateTime << 32) + m_dateTime.dwLowDateTime;
    int msec;

    // convert 100ns intervals to milli seconds + modulo 1000 to get only the milliseconds
    // msec = (int) ((unixtime / UA_MSECS_TO_100NS) % 1000);

    return msec;
}

/** Converts the UaDateTime into a std::string.
 *
 * @see fromString for a description of the string format.
 *
 * @return UaDateTime value converted to a string.
 */
std::string UaDateTime::toString() const
{
    std::string sDateTime;
    char sTemp[100];

    // if ( OpcUa_IsGood(T_DataTime_GetStringFromDateTime(m_dateTime, sTemp, 100)) )
    // {
    //     sDateTime = sTemp;
    // }

    return sDateTime;
}

/** Converts the date part of the UaDateTime into a std::string.
 *
 * @see fromString for a description of the string format.
 *
 * @return UaDateTime value date part converted to a string.
 */
std::string UaDateTime::toDateString() const
{
    std::string sDateTime;
    char sTemp[100];
    char* pString = NULL;

    // if ( OpcUa_IsGood(T_DataTime_GetStringFromDateTime(m_dateTime, sTemp, 100)) )
    // {
    //     pString = sTemp;

    //     while ( pString != NULL && pString[0] != (char)'T' )
    //     {
    //         pString++;
    //     }
    //     if ( pString[0] == (char)'T' )
    //     {
    //         pString[0] = 0;
    //     }

    //     sDateTime = sTemp;
    // }

    return sDateTime;
}

/** Converts the time part of the UaDateTime into a std::string.
 *
 * @see fromString for a description of the string format.
 *
 * @return UaDateTime value time part converted to a string.
 */
std::string UaDateTime::toTimeString() const
{
    std::string sDateTime;
    char sTemp[100];
    char* pString = NULL;

    // if ( OpcUa_IsGood(T_DataTime_GetStringFromDateTime(m_dateTime, sTemp, 100)) )
    // {
    //     pString = sTemp;

    //     while ( pString != NULL && pString[0] != (char)'T' )
    //     {
    //         pString++;
    //     }
    //     if ( pString[0] == (char)'T' )
    //     {
    //         pString++;
    //         sDateTime = pString;
    //     }
    //     else
    //     {
    //         sDateTime = sTemp;
    //     }
    // }

    return sDateTime;
}

/** Checks whether UaDateTime is null or not.
 *
 * @return true if null, false if not.
 */
bool UaDateTime::isNull() const
{
    if (m_dateTime.dwHighDateTime == 0 && m_dateTime.dwLowDateTime == 0)
    {
        return true;
    }
    return false;
}

/** Returns the milliseconds from the own time value to the passed time value.
 *
 * Calculates the milliseconds between the value of the UaDateTime object and the passed UaDateTime object. If the passed in
 * time is later than the internal time, the return value is positive. If the passed in value is earlier than the internal
 * time, the returned value is negative.
 *
 * @param to the UaDateTime used to calculate the difference in milliseconds.
 * @return the difference in milliseconds between the internal value and the passed in value.
 */
int UaDateTime::msecsTo(const UaDateTime &to) const
{
    Int64 i64from, i64to;
    // convert "from" to 64bit number
    i64from = m_dateTime.dwHighDateTime;
    i64from <<= 32;
    i64from |= m_dateTime.dwLowDateTime;
    // convert "to" to 64bit number
    i64to = to.m_dateTime.dwHighDateTime;
    i64to <<= 32;
    i64to |= to.m_dateTime.dwLowDateTime;
    // calculate difference
    i64to -= i64from;
    // convert 100ns intervalls to ms
    i64to /= 10000;
    return (int)i64to;
}

/** Returns the seconds from the own time value to the passed time value.
 *
 * Calculates the seconds between the value of the UaDateTime object and the passed UaDateTime object. If the passed in
 * time is later than the internal time, the return value is positive. If the passed in value is earlier than the internal
 * time, the returned value is negative.
 *
 * @param to the UaDateTime used to calculate difference in seconds.
 * @return the difference in seconds between the internal value and the passed in value.
 */
int UaDateTime::secsTo(const UaDateTime &to) const
{
    Int64 i64from, i64to;
    // convert "from" to 64bit number
    i64from = m_dateTime.dwHighDateTime;
    i64from <<= 32;
    i64from |= m_dateTime.dwLowDateTime;
    // convert "to" to 64bit number
    i64to = to.m_dateTime.dwHighDateTime;
    i64to <<= 32;
    i64to |= to.m_dateTime.dwLowDateTime;
    // calculate difference
    i64to -= i64from;
    // convert 100ns intervalls to seconds
    i64to /= 10000000;
    return (int)i64to;
}

/** Returns the days from the own time value to the passed time value.
 *
 * Calculates the days between the value of the UaDateTime object and the passed UaDateTime object. If the passed in
 * time is later than the internal time, the return value is positive. If the passed in value is earlier than the internal
 * time, the returned value is negative.
 *
 * @param to the UaDateTime used to calculate difference in days.
 * @return the difference in days between the internal value and the passed in value.
 */
int UaDateTime::daysTo(const UaDateTime &to) const
{
    Int64 i64from, i64to;
    // convert "from" to 64bit number
    i64from = m_dateTime.dwHighDateTime;
    i64from <<= 32;
    i64from |= m_dateTime.dwLowDateTime;
    // convert "to" to 64bit number
    i64to = to.m_dateTime.dwHighDateTime;
    i64to <<= 32;
    i64to |= to.m_dateTime.dwLowDateTime;
    // calculate difference
    i64to -= i64from;
    // convert 100ns intervals to days
#ifndef _UA_WIN32
    i64to /= 864000000000ll;
#else
    i64to /= 864000000000i64;
#endif
    return (int)i64to;
}

/** Adds the passed number of seconds to the date/time value.
 *
 * @param secs the number of seconds to add.
 */
void UaDateTime::addSecs(int secs)
{
    Int64 i64from, i64add = secs;
    // convert this to 64bit number
    i64from = m_dateTime.dwHighDateTime;
    i64from <<= 32;
    i64from |= m_dateTime.dwLowDateTime;
    // convert secs to 100ns
    i64add *= 10000000;
    // add secs
    i64from += i64add;
    // convert back to T_DataTime
    m_dateTime.dwLowDateTime = (int)(i64from & 0xffffffff);
    m_dateTime.dwHighDateTime = (int)(i64from >> 32);
}

/** Adds the passed number of milliseconds to the date/time value.
 *
 * @param msecs the number of milliseconds to add.
 */
void UaDateTime::addMilliSecs(int msecs)
{
    Int64 i64from, i64add = msecs;
    // convert this to 64bit number
    i64from = m_dateTime.dwHighDateTime;
    i64from <<= 32;
    i64from |= m_dateTime.dwLowDateTime;
    // convert ms to 100ns
    i64add *= 10000;
    // add secs
    i64from += i64add;
    // convert back to T_DataTime
    m_dateTime.dwLowDateTime = (int)(i64from & 0xffffffff);
    m_dateTime.dwHighDateTime = (int)(i64from >> 32);
}

/** Copy the internal date/time value into an T_DataTime structure.
 */
void UaDateTime::copyTo(T_DataTime *copy) const
{
    if (copy != NULL)
    {
        *copy = m_dateTime;
    }
}

/** Less than operator for comparing two date/time values.
 *
 * @param other the UaDateTime to compare.
 * @return true if time of the object is earlier than other, false if not.
 */
bool UaDateTime::operator<(const UaDateTime &other) const
{
    if (m_dateTime.dwHighDateTime < other.m_dateTime.dwHighDateTime)
    {
        return true;
    }
    else if (m_dateTime.dwHighDateTime > other.m_dateTime.dwHighDateTime)
    {
        return false;
    }
    else
    {
        return (m_dateTime.dwLowDateTime < other.m_dateTime.dwLowDateTime);
    }
}

/** Greater than operator for comparing two date/time values.
 *
 * @param other the UaDateTime to compare.
 * @return true if time of the object is later than other, false if not.
 */
bool UaDateTime::operator>(const UaDateTime &other) const
{
    if (m_dateTime.dwHighDateTime > other.m_dateTime.dwHighDateTime)
    {
        return true;
    }
    else if (m_dateTime.dwHighDateTime < other.m_dateTime.dwHighDateTime)
    {
        return false;
    }
    else
    {
        return (m_dateTime.dwLowDateTime > other.m_dateTime.dwLowDateTime);
    }
}

/** Equals operator for comparing two date/time values.
 *
 * @param other the UaDateTime to compare.
 * @return true if other equals this, false if not.
 */
bool UaDateTime::operator==(const UaDateTime &other) const
{
    return (m_dateTime.dwHighDateTime == other.m_dateTime.dwHighDateTime &&
            m_dateTime.dwLowDateTime == other.m_dateTime.dwLowDateTime);
}

/** Not Equals operator for comparing two date/time values.
 *
 * @see operator==
 * @param other the UaDateTime to compare.
 * @return true if other is not equal to this.
 */
bool UaDateTime::operator!=(const UaDateTime &other) const
{
    return (m_dateTime.dwHighDateTime != other.m_dateTime.dwHighDateTime ||
            m_dateTime.dwLowDateTime != other.m_dateTime.dwLowDateTime);
}

/** Copies the internal date/time value to a UaDataValue.
 *
 * The value is set with the date/time and the status is set to OpcUa_Good.
 *
 * @param dataValue the UaDataValue to update.
 * @param updateTimeStamps If true, the timestamps of the DataValue are set to the current time.
 *                         Otherwise the timestamps are not changed.
 */
void UaDateTime::toDataValue(UaDataValue &dataValue, bool bupdateTimeStamps) const
{
    CKiwiVariant variant;
    //toVariant(variant);
    dataValue.setValue(variant, bupdateTimeStamps);
}

/** Copies the internal date/time value to a T_DataValue structure.
 *
 * The value is set with the date/time and the status is set to OpcUa_Good.
 *
 * @param dataValue the UaDataValue to update.
 * @param updateTimeStamps If true, the timestamps of the DataValue are set to the current time.
 *                         Otherwise the timestamps are initialized to null.
 */
void UaDateTime::toDataValue(T_DataValue &dataValue, bool updateTimeStamps) const
{
    T_Variant_Clear(&dataValue.Value);
    //toVariant(dataValue.Value);

    if (updateTimeStamps != false)
    {
        // dataValue.SourceTimestamp = T_DataTime_UtcNow();
        // dataValue.ServerTimestamp = T_DataTime_UtcNow();
    }
}

// /** Writes the internal date/time value to the stream.
//  */
// UaDataStream &operator<<(UaDataStream &out, const UaDateTime &val)
// {
//     T_DataTime dateTime = (T_DataTime)val;

//     // write DateTime
//     out << dateTime;

//     return out;
// }

// /** Reads a date/time value from the stream.
//  */
// UaDataStream &operator>>(UaDataStream &in, UaDateTime &val)
// {
//     T_DataTime dateTime;
//     T_DataTime_Initialize(&dateTime);

//     // read DateTime
//     in >> dateTime;
//     val = UaDateTime(dateTime);
//     return in;
// }
