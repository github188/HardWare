/*!
 *	\file	IOCfgDriverDbfParser.h
 *
 *	��������ͷ�ļ�
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
