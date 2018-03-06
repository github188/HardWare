/*!
 *	\file	IODriverH.h
 *
 *	\brief	�����๫��ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��14��	15:08
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

//!��ǩ���ͺͱ�ǩ��ַ��Ϣ����ṹ��
typedef struct OBJTAG
{
	//!��ǩ����
	Byte byUnitType;

	//!��ǩ��ַ
	int lTagAddr;

	//!С��("<")�ж�
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

	//!�Ƿ����
	bool operator == ( const OBJTAG ObjTag ) const
	{ 
		return byUnitType==ObjTag.byUnitType && lTagAddr==ObjTag.lTagAddr ;
	}

	//!��ȡ��ǩ��������ַ
	long long GetIndexAddr()
	{
		return (long long)lTagAddr*1000 + byUnitType;
	}

}OBJTAG;

#endif
