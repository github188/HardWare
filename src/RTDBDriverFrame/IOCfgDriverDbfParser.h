/*!
 *	\file	IOCfgDriverDbfParser.h
 *
 *	\brief	������Ӧ��������.dbf�ļ�������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��4��21��	9:49
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODRIVERDBFPARSER_H_
#define _IODRIVERDBFPARSER_H_

#include "RTDBDriverFrame/IODriverCommonDef.h"
#include "utilities/fepcommondef.h"
#include <vector>


namespace MACS_SCADA_SUD
{
	/*!
	 *	\class	CIOCfgDriverDbfParser
	 *
	 *	\brief	������ǩ��ַ������
	 *
	 *	֧�ָ�ʽ:
	 *	%U��UnitAddress =VALUE��
	 *	%+U��UnitAddress += VALUE��
	 *	%*N��UnitAddress *= N��
	 *	%+N��UnitAddress += N��
	 *	%S���ַ���; ��ǩ�ļ����ַ�����ù����Զ����䡣��"_"Ϊ�ֽ�,����: DI_%S, DI_AA,DI_BB
	 *	���飺(ע��:Ŀǰ��֧��)
	 *	�磺��ǩ��ΪData����ǩ��ַΪDI0[5]����ʾ�ӱ�ǩ��ַDI0��ʼ��5����ǩ:
	 *	Data[0]->DI0��Data[1]->DI1��Data[2]->DI2��Data[3]->DI3��Data[4]->DI4.
	 *
	 */
	class IODRIVERFRAME_API CIOCfgDriverDbfParser
	{
		//!��ǩ��ʽ������Ϣ������
		typedef std::vector<tIOCfgUnitDataDes> T_VecDataDes;

	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOCfgDriverDbfParser();

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIOCfgDriverDbfParser();

		/*!
		 *	\brief	����Template�ֽ����ɲ������ʽ
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pchFormat: ��ǩ��ַ
		 *	\param	tBol: �ֽ��Ĳ������ʽ
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		int	MakeBolanExpress( const char* pchFormat, tBOLAN& tBol);

		/*!
		 *	\brief	�����ǩ�ĵ�ַ
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	tDataDes: �豸�ı�ǩ��ʽ������Ϣ
		 *	\param	plItemVal: ��ǩ��ַ�и������ֵ
		 *	\param	lTagAddr: �����õ��ı�ǩ�����ַ
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		int	CalcBolanExpress( tIOCfgUnitDataDes tDataDes, int* plItemVal, int& lTagAddr );

		/*!
		 *	\brief	�����ǩ��ַ����ֵ
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	�豸�ı�ǩ��ʽ������Ϣ,�������, �޸���lLower��lUpper
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		int CalcTagAddrBound( tIOCfgUnitDataDes& tDataDes );

		/*!
		 *	\brief	����һ������
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	dataDes: Ҫ���ӵ��豸�ı�ǩ��ʽ������Ϣ
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		void AddDes( tIOCfgUnitDataDes& dataDes );

		/*!
		 *	\brief	��ȡ��ǩ��ַģ�������
		 */
		int	GetTagDesCount();

		/*!
		 *	\brief	ͨ�������Ż�ȡ��ǩģ��ṹ
		 *
		 *	\param	iIndex: ������
		 *	\param	tDataDes: �õ��ı�ǩ��ʽ������Ϣ
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		int	GetTagDesByIndex( int iIndex, tIOCfgUnitDataDes& tDataDes);

		/*!
		 *	\brief	ͨ����ǩ����̬��ַ��ȡ��ǩ������ṹ
		 *
		 *	\param	pchTagAddr: ��ǩ��ַ
		 *	\param	tDataDes: �õ��ı�ǩ��ʽ������Ϣ
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int	GetTagDesByTagAddr( const char* pchTagAddr, tIOCfgUnitDataDes& tDataDes );

	private:
		//!��ǩ��ʽ��Ϣ����������
		T_VecDataDes m_vecDes;
	};
}

#endif