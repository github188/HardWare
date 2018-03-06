/*!
 *	\file	IODriverH.h
 *
 *	\brief	驱动类公共头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月14日	15:08
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IODRIVERH_20080421_H_
#define _IODRIVERH_20080421_H_

 #include "ace/OS.h"
 #include "ace/OS_NS_time.h"
 #include "ace/Method_Request.h"
 #include "ace/Singleton.h"
 #include "ace/Message_Queue.h"
 #include "ace/Activation_Queue.h"
 #include "ace/Timer_Heap.h"
 #include "ace/Synch.h"
 #include "ace/Date_Time.h"

#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/GenericFactory.h"
#include "RTDBCommon/Debug_Msg.h"
#include "RTDBCommon/ClassDllManager.h"
#include "RTDBCommon/RuntimeCatlog.h"
// #include "Common/CommonXmlParser/SystemString.h"
// #include "Common/CommonXmlParser/DeployManager.h"
#include "RTDBCommon/IOLog.h"
#include "RTDBCommon/IOTimer.h"

#include "RTDBBaseClasses/IOBoardBase.h"
#include "RTDBBaseClasses/IOPrtclParserBase.h"

#include "../IODrvBoard/IODrvBoard.h"

#include "IODriverBase.h"
#include "IOCfgUnit.h"
#include "IOCfgTag.h"
#include "RTDBPlatformClass/FieldPointType.h"


// #include "IOSwitchData.h"

// typedef uint16_t  WORD;
// typedef uint   DWORD;

typedef unsigned char BYTE;
typedef unsigned char byte;
typedef unsigned short WORD;

#ifndef MAKEDWORD
#define MAKEDWORD(l, h)	((DWORD)(((WORD)(l)) | ((DWORD)((WORD)(h))) << 16))
#endif  //MAKEDWORD

#ifndef MAKEWORD
#define MAKEWORD(l, h)	((WORD)(((BYTE)(l)) | ((WORD)((BYTE)(h))) << 8))
#endif	//MAKEWORD

#ifndef LOWORD
#define LOWORD(l)           ((WORD)(((DWORD)(l)) & 0xffff))
#endif

#ifndef HIWORD
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
#endif

#ifndef LOBYTE
#define LOBYTE(w)           ((BYTE)(((DWORD)(w)) & 0xff))
#endif

#ifndef HIBYTE
#define HIBYTE(w)           ((BYTE)((((DWORD)(w)) >> 8) & 0xff))
#endif


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//!标签类型和标签地址信息处理结构体
typedef struct OBJTAG
{
	//!标签类型
	Byte byUnitType;

	//!标签地址
	int lTagAddr;

	//!小于("<")判断
	bool operator < ( const OBJTAG ObjTag ) const 
	{
		if ( byUnitType < ObjTag.byUnitType  )
			return true;
		else if ( byUnitType == ObjTag.byUnitType )
		{
			return lTagAddr < ObjTag.lTagAddr;
		}
		return false;
	}

	//!是否相等
	bool operator == ( const OBJTAG ObjTag ) const
	{ 
		return byUnitType==ObjTag.byUnitType && lTagAddr==ObjTag.lTagAddr ;
	}

	//!获取标签的索引地址
	long long GetIndexAddr()
	{
		return (long long)lTagAddr*1000 + byUnitType;
	}

}OBJTAG;

#endif
