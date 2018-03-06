/*!
 *	\file	IOCfgDriverDbfParser.h
 *
 *	公共定义头文件
 *
 *	\author JYF
 */
#ifndef _IODRIVERCOMMONDEF_H_
#define _IODRIVERCOMMONDEF_H_

//#include "ace/OS.h"
#include "RTDBCommon/OS_Ext.h"

#ifdef IODRIVERFRAME_EXPORTS
#define IODRIVERFRAME_API MACS_DLL_EXPORT
#define IODRIVERFRAME_VAR MACS_DLL_EXPORT
#else
#define IODRIVERFRAME_API MACS_DLL_IMPORT
#define IODRIVERFRAME_VAR MACS_DLL_IMPORT
#endif


#endif
