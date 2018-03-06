/*!
 *	\file	IOSwitchData.cpp
 *
 *	\brief	FEPIO数据转换
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月9日	14:02
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBDriverFrame/IOSwitchData.h"

namespace MACS_SCADA_SUD
{
	//!二进制取反
	int CIOSwitchData::Convert( Byte* dst, const Byte* src, int length )
	{
		int i;

		for(i=0; i<length; i++)
		{
			dst[i] = src[i]^0xFF;
		}
		return 0;
	}

	//!十六进制转为十进制
	unsigned int CIOSwitchData::HextoDec(const Byte* hex, int length)
	{
		int i;
		unsigned int rslt = 0;

		for(i=0; i<length; i++)
		{
			rslt += (unsigned int)(hex[i])<<(8*(length-1-i));

		}
		return rslt;
	}

	//!十进制转十六进制
	int CIOSwitchData::DectoHex(int dec, Byte* hex, int length)
	{
		int i;

		for(i=length-1; i>=0; i--)
		{
			hex[i] = (dec%256)&0xFF;
			dec /= 256;
		}

		return 0;
	}

	//!求权
	unsigned int CIOSwitchData::Power(int base, int times)
	{
		int i;
		unsigned int rslt = 1;

		for(i=0; i<times; i++)
			rslt *= base;

		return rslt;
	}

	//!BCD转10进制
	unsigned int CIOSwitchData::BCDtoDec(const Byte* bcd, int length)
	{
		int i, tmp;
		unsigned int dec = 0;

		for(i=0; i<length; i++)
		{
			tmp = ((bcd[i]>>4)&0x0F)*10 + (bcd[i]&0x0F);   
			dec += tmp * Power(100, length-1-i);          
		}
		return dec;
	}

	//!10进制转BCD
	int CIOSwitchData::DectoBCD(int Dec, Byte *Bcd, int length)
	{
		int i;
		int temp;

		for(i=length-1; i>=0; i--)
		{
			temp = Dec%100;
			Bcd[i] = ((temp/10)<<4) + ((temp%10) & 0x0F);
			Dec /= 100;
		}

		return 0;
	}

	//!单字节BCD码转十六进制
	Byte CIOSwitchData::BCDtoHex(Byte Bcd)
	{
		return Byte(int(int(Bcd)/16)*10+int(Bcd)%16);
	}

	//!单字节十六进制（值小于100）转BCD码
	Byte CIOSwitchData::HextoBCD(Byte Hex)
	{
		return Byte(int(int(Hex)/10)*16+int(Hex)%10);
	}

	//!BCD字节数组转十进制数
	bool CIOSwitchData::BCDByteToDecimal(Byte* buf, int nLen, unsigned long &dwRet)
	{
		if(nLen > 4)
		{
			return false;
		}
		unsigned long n;
		n =0;
		for(int i = 0; i < nLen; i++)
		{
			n += (unsigned long) (buf[i] & 0x0f) *((unsigned long)pow(10.0,i*2));
			n += (unsigned long)((buf[i] & 0xf0) >> 4) * 10 * ((unsigned long)pow(10.0,i*2));
		}
		dwRet = n;
		return true;
	}

	//!BCD字节数组转浮点数
	bool CIOSwitchData::BCDByteToFloat(Byte* buf, int nLen, int nPoints, double &fResult)
	{
		bool bRet = true;
		int i;
		if(nLen > 4)
		{
			return false;
		}

		int n;
		n =0;
		for(i = 0; i < nLen; i++)
		{
			n += (buf[i] & 0x0f) *((int)pow(10.0,i*2));
			n += ((buf[i] & 0xf0) >> 4) * 10 * ((int)pow(10.0,i*2));
		}
		double fScale = 1.0;
		switch(nPoints)
		{
		case 0:
			fScale = 1.0;
			break;
		case 1:
			fScale = 0.1;
			break;
		case 2:
			fScale = 0.01;
			break;
		case 3:
			fScale = 0.001;
			break;
		case 4:
			fScale = 0.0001;
			break;
		case 5:
			fScale = 0.00001;
			break;
		case 6:
			fScale = 0.000001;
			break;
		default:
			fScale = 0.000001;
		}
		fResult = ((float)(n)*fScale);

		return true;
	}

	//!BCD字节数组转字符串
	bool CIOSwitchData::BCDByteToString(Byte* buf, int nLen, char* pszRet)
	{
		int i = 0;
		for(i = 0; i < nLen; i++)
		{
			pszRet[i*2] = ((buf[nLen-i-1] &0xf0) >> 4) + '0';
			pszRet[i*2+1] = (buf[nLen-i-1] &0x0f) + '0';
		}
		pszRet[i*2] = 0;
		return true;
	}
}