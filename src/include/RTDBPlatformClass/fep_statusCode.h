/*!
 *	\file	fep_statusCode.h
 *
 *	\brief	
 *	
 *
 *	\author lingling.li
 *
 *	\date	2014年5月21日	9:28
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef __FEP_STATUSCODE_H__
#define __FEP_STATUSCODE_H__

// Namespace
namespace MACS_SCADA_SUD {

//标签状态码
#define FEP_StatusCode_FieldPoint_Online		0x01
#define FEP_StatusCode_FieldPoint_Init			0x02
#define FEP_StatusCode_FieldPoint_Force			0x04
#define FEP_StatusCode_FieldPoint_Diable		0x08

const uint FPGood = 1;
const uint FPBad = 0;


}// End namespace
#endif // __FEP_STATUSCODE_H__