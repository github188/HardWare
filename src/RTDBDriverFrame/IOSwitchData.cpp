/*!
 *	\file	IOSwitchData.cpp
 *
 *	\brief	FEPIO����ת��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��9��	14:02
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBDriverFrame/IOSwitchData.h"

namespace MACS_SCADA_SUD
{
	//!������ȡ��
	int CIOSwitchData::Convert( Byte* dst, const Byte* src, int length )
	{
		int i;

		for(i=0; i<length; i++)
		{
			dst[i] = src[i]^0xFF;
		}
		return 0;
	}

	//!ʮ������תΪʮ����
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

	//!ʮ����תʮ������
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

	//!��Ȩ
	unsigned int CIOSwitchData::Power(int base, int times)
	{
		int i;
		unsigned int rslt = 1;

		for(i=0; i<times; i++)
			rslt *= base;

		return rslt;
	}

	//!BCDת10����
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

	//!10����תBCD
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

	//!���ֽ�BCD��תʮ������
	Byte CIOSwitchData::BCDtoHex(Byte Bcd)
	{
		return Byte(int(int(Bcd)/16)*10+int(Bcd)%16);
	}

	//!���ֽ�ʮ�����ƣ�ֵС��100��תBCD��
	Byte CIOSwitchData::HextoBCD(Byte Hex)
	{
		return Byte(int(int(Hex)/10)*16+int(Hex)%10);
	}

	//!BCD�ֽ�����תʮ������
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

	//!BCD�ֽ�����ת������
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

	//!BCD�ֽ�����ת�ַ���
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