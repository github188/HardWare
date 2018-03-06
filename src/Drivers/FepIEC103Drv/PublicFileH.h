/*!
 *	\file	PublicFileH.h
 *
 *	\brief	103��������Ԥ����ͷ�ļ�
 *
 *	�����˸����๫�õĺ꣬����Ҫ������ͷ�ļ�һ�ΰ�������
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
		*	\brief	��ȡini�����ļ���ĳ��������Ϣ
		*
		*	\param	pchAppName	�ڵ�������"[]"������
		*	\param	pchKeyName	�ؼ�����
		*	\param	iDefault	Ĭ��ֵ����ȡʧ���򷵻ظ�ֵ
		*	\param	pchFileName	�ļ���
		*
		*	\retval	OpcUa_Int32	ini�����ļ��ж�Ӧ���ֵ����ȡʧ���򷵻�Ĭ��ֵ
		*/
		//extern COMMONBASECODE_API OpcUa_UInt32  GetPrivateProfileInt(const OpcUa_CharA* pchAppName, 
													//const OpcUa_CharA* pchKeyName, OpcUa_Int32 iDefault, const OpcUa_CharA* pchFileName);

		/*!
		*	\brief	��ȡini�����ļ���ĳ��������Ϣ
		*
		*	\param	pchAppName	�ڵ�������"[]"������
		*	\param	pchKeyName	�ؼ�����
		*	\param	strDefault	Ĭ��ֵ����ȡʧ���򷵻ظ�ֵ
		*	\param	cReturnedString	������
		*	\param	nSize	��������С
		*	\param	pchFileName	�ļ���
		*
		*	\retval	const OpcUa_CharA*	ini�����ļ��ж�Ӧ���ֵ����ȡʧ���򷵻�Ĭ��ֵ
		*/
		//extern COMMONBASECODE_API OpcUa_UInt32 GetPrivateProfileString( const OpcUa_CharA * szAppName, const OpcUa_CharA * szKeyName, const OpcUa_CharA * szDefault,
		//											OpcUa_CharA * cReturnedString, OpcUa_UInt32 nSize, const OpcUa_CharA * szFileName);

	#endif

#endif
