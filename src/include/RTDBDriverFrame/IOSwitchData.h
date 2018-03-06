/*!
 *	\file	IOSwitchData.h
 *
 *	\brief	FEPIO����ת����ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author Miaoweijie
 *
 *	\date	2014��4��9��	13:30
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
	 *	\brief	FEPIO����ת���࣬��װ�˳��õ�����ת���ӿ�
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class IODRIVERFRAME_API CIOSwitchData
	{
	public:

		/*!
		*	\brief	������ȡ��
		*
		*	\param	dst		ȡ����Ķ���������
		*	\param	src		����������
		*	\param	length	��ת���Ķ��������ݳ��� 
		*
		*	\retval	int		0 - �ɹ�������Ϊʧ��
		*
		*/
		static int Convert( Byte* dst, const Byte* src, int length );

		/*!
		*	\brief	ʮ������תΪʮ����
		*
		*	\param	hex		��ת����ʮ����������
		*	\param	length	ʮ���������ݳ��� 
		*
		*	\retval	unsigned int	ת�����ʮ��������
		*
		*   \note ʮ������ÿ���ַ�λ����ʾ��ʮ�������ķ�Χ��0 ~255������Ϊ256
		*      ����8λ(<<8)�ȼ۳���256
		*/
		static unsigned int HextoDec(const Byte* hex, int length);

		/*!
		*	\brief	ʮ����תʮ������
		*
		*	\param	dec		��ת����ʮ��������
		*	\param	hex		ת�����ʮ����������
		*	\param	length	ת�����ʮ���������ݳ��� 
		*
		*	\retval	int			0 - �ɹ�������Ϊʧ��
		*
		*   \note ԭ��ͬʮ������תʮ����
		*/
		static int DectoHex(int dec, Byte* hex, int length);

		/*!
		*	\brief	��Ȩ
		*
		*	\param	base	���ƻ���
		*	\param	times	Ȩ���� 
		*
		*	\retval	unsigned int	��ǰ����λ��Ȩ
		*
		*/
		static unsigned int Power(int base, int times);

		/*!
		*	\brief	BCDת10����
		*
		*	\param	bcd		��ת����BCD��
		*	\param	length	BCD�����ݳ��� 
		*
		*	\retval	unsigned int	ת�����ʮ������
		*
		*   \note ѹ��BCD��һ���ַ�����ʾ��ʮ�������ݷ�ΧΪ0 ~ 99,����Ϊ100
		*      ����ÿ���ַ�����ʾ��ʮ����ֵ��Ȼ�����Ȩ
		*/
		static unsigned int BCDtoDec(const Byte* bcd, int length);

		/*!
		*	\brief	ʮ����תBCD��
		*
		*	\param	Dec		��ת����ʮ��������
		*	\param	Bcd		ת�����BCD��
		*	\param	length	BCD�����ݳ���
		*
		*	\retval	int			0 - �ɹ�������Ϊʧ��
		*
		*/
		static int DectoBCD(int Dec, Byte *Bcd, int length);

		/*!
		*	\brief	���ֽ�BCD��תʮ������
		*
		*	\param	bcd		��ת����BCD��
		*
		*	\retval	Byte	ת�����ʮ��������
		*/
		static Byte BCDtoHex( Byte Bcd );

		/*!
		*	\brief	���ֽ�ʮ�����ƣ�ֵС��100��תBCD��
		*
		*	\param	Hex		��ת����ʮ����������
		*
		*	\retval	Byte	ת�����BCD��
		*
		*/
		static Byte HextoBCD( Byte Hex );

		/*!
		*	\brief	BCD�ֽ�����תʮ������
		*
		*	\param	buf		��ת����BCD�ֽ�����
		*	\param	nLen	BCD�ֽڳ���
		*	\param	dwRet	ת�����ʮ���������޷��ţ�
		*
		*	\retval	bool		true - �ɹ�������Ϊʧ��
		*
		*/
		static bool BCDByteToDecimal(Byte* buf, int nLen, unsigned long &dwRet);

		/*!
		*	\brief	BCD�ֽ�����ת������
		*
		*	\param	buf		��ת����BCD�ֽ�����
		*	\param	nLen	BCD�ֽڳ���
		*	\param	nPoints	��������С����λ��
		*	\param	fResult	ת����ĸ�����
		*
		*	\retval	bool		true - �ɹ�������Ϊʧ��
		*
		*/
		static bool BCDByteToFloat(Byte* buf, int nLen, int nPoints, double &fResult);

		/*!
		*	\brief	BCD�ֽ�����ת�ַ���
		*
		*	\param	buf		��ת����BCD�ֽ�����
		*	\param	nLen	BCD�ֽڳ���
		*	\param	pszRet	ת������ַ���
		*
		*	\retval	bool	true - �ɹ�������Ϊʧ��
		*
		*/
		static bool BCDByteToString(Byte* buf, int nLen, char* pszRet);
	};
}
#endif