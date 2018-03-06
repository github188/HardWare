/*!
 *	\file	PublicFileH.h
 *
 *	\brief	103驱动工程预编译头文件
 *
 *	定义了各个类公用的宏，并把要包含的头文件一次包含进来
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/PublicFileH.h 1     11-01-21 10:02a Zhangyuguo $
 *	$Author: Zhangyuguo $
 *	$Date: 11-01-21 10:02a $
 *	$Revision: 1 $
 *
 *	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _FEP_IEC103PublicFileH_20110110_H_
#define _FEP_IEC103PublicFileH_20110110_H_

#ifdef FEPIEC103DRV_EXPORTS
#define FEPIEC103_API MACS_DLL_EXPORT
#else
#define FEPIEC103_API MACS_DLL_IMPORT
#endif

//#ifndef MAKEDWORD
//#define MAKEDWORD(l, h)	((DWORD)(((WORD)(l)) | ((DWORD)((WORD)(h))) << 16))
//#endif		

#include "RTDB/Server/DBDriverFrame/IODriverH.h"

#include "ace/OS.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "ace/Date_Time.h"

#include "IEC103.h"
#include <string>

//typedef OpcUa_UInt16  WORD;
//typedef OpcUa_UInt32  DWORD;

	#ifndef _WINDOWS
		/*!
		*	\brief	获取ini配置文件中某项配置信息
		*
		*	\param	pchAppName	节点名，即"[]"中内容
		*	\param	pchKeyName	关键字名
		*	\param	iDefault	默认值，读取失败则返回该值
		*	\param	pchFileName	文件名
		*
		*	\retval	OpcUa_Int32	ini配置文件中对应项的值，读取失败则返回默认值
		*/
		//extern COMMONBASECODE_API OpcUa_UInt32  GetPrivateProfileInt(const OpcUa_CharA* pchAppName, 
													//const OpcUa_CharA* pchKeyName, OpcUa_Int32 iDefault, const OpcUa_CharA* pchFileName);

		/*!
		*	\brief	获取ini配置文件中某项配置信息
		*
		*	\param	pchAppName	节点名，即"[]"中内容
		*	\param	pchKeyName	关键字名
		*	\param	strDefault	默认值，读取失败则返回该值
		*	\param	cReturnedString	缓冲区
		*	\param	nSize	缓冲区大小
		*	\param	pchFileName	文件名
		*
		*	\retval	const OpcUa_CharA*	ini配置文件中对应项的值，读取失败则返回默认值
		*/
		//extern COMMONBASECODE_API OpcUa_UInt32 GetPrivateProfileString( const OpcUa_CharA * szAppName, const OpcUa_CharA * szKeyName, const OpcUa_CharA * szDefault,
		//											OpcUa_CharA * cReturnedString, OpcUa_UInt32 nSize, const OpcUa_CharA * szFileName);

	#endif

#endif
