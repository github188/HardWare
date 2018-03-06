/*!
 *	\file	IOSwitchData.h
 *
 *	\brief	FEPIO数据转换类头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月9日	13:30
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOSWITCHDATA_H_
#define _IOSWITCHDATA_H_

#include <stdio.h>
#include <string>
#include <math.h>
#include "RTDBDriverFrame/IODriverCommonDef.h"

namespace MACS_SCADA_SUD
{
	/*!
	 *	\class	CIOSwitchData
	 *
	 *	\brief	FEPIO数据转换类，封装了常用的数据转换接口
	 *
	 *	详细的类说明（可选）
	 */
	class IODRIVERFRAME_API CIOSwitchData
	{
	public:

		/*!
		*	\brief	二进制取反
		*
		*	\param	dst		取反后的二进制数据
		*	\param	src		二进制数据
		*	\param	length	待转换的二进制数据长度 
		*
		*	\retval	int		0 - 成功，否则为失败
		*
		*/
		static int Convert( Byte* dst, const Byte* src, int length );

		/*!
		*	\brief	十六进制转为十进制
		*
		*	\param	hex		待转换的十六进制数据
		*	\param	length	十六进制数据长度 
		*
		*	\retval	unsigned int	转换后的十进制数据
		*
		*   \note 十六进制每个字符位所表示的十进制数的范围是0 ~255，进制为256
		*      左移8位(<<8)等价乘以256
		*/
		static unsigned int HextoDec(const Byte* hex, int length);

		/*!
		*	\brief	十进制转十六进制
		*
		*	\param	dec		待转换的十进制数据
		*	\param	hex		转换后的十六进制数据
		*	\param	length	转换后的十六进制数据长度 
		*
		*	\retval	int			0 - 成功，否则为失败
		*
		*   \note 原理同十六进制转十进制
		*/
		static int DectoHex(int dec, Byte* hex, int length);

		/*!
		*	\brief	求权
		*
		*	\param	base	进制基数
		*	\param	times	权级数 
		*
		*	\retval	unsigned int	当前数据位的权
		*
		*/
		static unsigned int Power(int base, int times);

		/*!
		*	\brief	BCD转10进制
		*
		*	\param	bcd		待转换的BCD码
		*	\param	length	BCD码数据长度 
		*
		*	\retval	unsigned int	转换后的十进制数
		*
		*   \note 压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100
		*      先求每个字符所表示的十进制值，然后乘以权
		*/
		static unsigned int BCDtoDec(const Byte* bcd, int length);

		/*!
		*	\brief	十进制转BCD码
		*
		*	\param	Dec		待转换的十进制数据
		*	\param	Bcd		转换后的BCD码
		*	\param	length	BCD码数据长度
		*
		*	\retval	int			0 - 成功，否则为失败
		*
		*/
		static int DectoBCD(int Dec, Byte *Bcd, int length);

		/*!
		*	\brief	单字节BCD码转十六进制
		*
		*	\param	bcd		待转换的BCD码
		*
		*	\retval	Byte	转换后的十六进制数
		*/
		static Byte BCDtoHex( Byte Bcd );

		/*!
		*	\brief	单字节十六进制（值小于100）转BCD码
		*
		*	\param	Hex		待转换的十六进制数据
		*
		*	\retval	Byte	转换后的BCD码
		*
		*/
		static Byte HextoBCD( Byte Hex );

		/*!
		*	\brief	BCD字节数组转十进制数
		*
		*	\param	buf		待转换的BCD字节数组
		*	\param	nLen	BCD字节长度
		*	\param	dwRet	转换后的十进制数（无符号）
		*
		*	\retval	bool		true - 成功，否则为失败
		*
		*/
		static bool BCDByteToDecimal(Byte* buf, int nLen, unsigned long &dwRet);

		/*!
		*	\brief	BCD字节数组转浮点数
		*
		*	\param	buf		待转换的BCD字节数组
		*	\param	nLen	BCD字节长度
		*	\param	nPoints	浮点数的小数点位置
		*	\param	fResult	转换后的浮点数
		*
		*	\retval	bool		true - 成功，否则为失败
		*
		*/
		static bool BCDByteToFloat(Byte* buf, int nLen, int nPoints, double &fResult);

		/*!
		*	\brief	BCD字节数组转字符串
		*
		*	\param	buf		待转换的BCD字节数组
		*	\param	nLen	BCD字节长度
		*	\param	pszRet	转换后的字符串
		*
		*	\retval	bool	true - 成功，否则为失败
		*
		*/
		static bool BCDByteToString(Byte* buf, int nLen, char* pszRet);
	};
}
#endif