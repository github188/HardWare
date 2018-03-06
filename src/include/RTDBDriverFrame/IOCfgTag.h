/*!
 *	\file	IOCfgTag.h
 *
 *	\brief	��ǩ������(��ӦCCTool��ͨ����ַ)
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��18��	13:15
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOCFGTAG_H_
#define _IOCFGTAG_H_

#include <list>
#include <map>
#include "RTDBDriverFrame/IODriverCommonDef.h"

namespace MACS_SCADA_SUD
{
	class FieldPointType;
	class CKiwiVariant;

	typedef std::map<long, FieldPointType*> T_mapFieldPoints;

	/*!
	 *	\class	CIOCfgTag
	 *
	 *	\brief	��ǩ��Ϣ������
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class IODRIVERFRAME_API CIOCfgTag
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOCfgTag();

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIOCfgTag();

		/*!
		 *	\brief	��ȡ����Ӧ��FeildPoint����ָ��
		 *
		 *	\param	��
		 *
		 *	\retval	T_lstFieldPoint	FieldPoint����ָ���б�
		 */
		T_mapFieldPoints GetFieldPoints();

		/*!
		 *	\brief	����FieldPoint����ָ��
		 *
		 *	\param	FieldPointType* FieldPoint����ָ��
		 *
		 *	\retval	��
		 */
		void AddFieldPoint(FieldPointType* pFieldPoint);

		/*!
		 *	\brief	ȡ�ñ�ǩ����,��FieldPoint�����л�ȡ
		 *
		 *	\param	��
		 *
		 *	\retval	std::string ��ǩ��
		 */
		std::string GetName();

		/*!
		 *	\brief	�õ���ǩ��̬��ַ,��FieldPoint�����л�ȡ
		 *
		 *	\param	��
		 *
		 *	\retval	std::string ��ǩ��̬��ַ
		 */
		std::string	GetTagAddr();

		/*!
		 *	\brief	��ȡ��ǩ�����ַ
		 *
		 *	\param	��
		 *
		 *	\retval	int ��ǩ�����ַ
		 */
		int GetFinalAddr();

		/*!
		 *	\brief	���ñ�ǩ�����ַ
		 *
		 *	\param	lFinalAdress	��ǩ�����ַ
		 *
		 *	\retval	��
		 */
		void SetFinalAddress(int lFinalAdress);

		/*!
		 *	\brief	��ȡ��ǩ����Unit_Type
		 *
		 *	\param	��
		 *
		 *	\retval	int ��ǩ����
		 */
		int	GetUnitType();

		/*!
		 *	\brief	���ñ�ǩ����
		 *
		 *	\param	nUnitType	��ǩ����
		 *
		 *	\retval	��
		 */
		void SetUnitType(int nUnitType);

		/*!
		 *	\brief	�ֽ��ǩ��ַ���õ������ֵ
		 *
		 *	\param	plItemVal	��ŷֽ��ĵ�ַ���������ָ��
		 *	\param	nLen		���鳤��
		 *
		 *	\retval	int 0Ϊ�ɹ�������ʧ��:-1Ϊ����ָ����Ч��-2Ϊ��ǩ��ַ���ֽܷ⣻-3�ֽ��ĸ����������鳤��
		 */
		int AnalyzeTagAddr( int* plItemVal, int nLen );

		/*!
		 *	\brief	�õ���ǩ��������ת����Ϣ
		 *
		 *	\retval	UaFloatArray �û����õĹ�������ת����Ϣ
		 */
		//UaFloatArray GetRawEngConfig();

		/*!
		 *	\brief	�õ������ͱ�ǩ��ֵ
		 *
		 *	\retval	UaDataValue ͨ����ֵ
		 */
		CKiwiVariant GetValue();

		//!��ȡͨ��MAP�е�һ��ͨ��
		FieldPointType* GetFirst();

		/*!
		 *	\brief	��ȡ�ɼ�ͨ����ʾ��
		 *
		 *	\param	nIndex:��ǩ����(Ĭ�ϴ�0��ʼ)
		 *
		 *	\retval	std::string �ɼ�ͨ����ʾ��
		 */
		std::string GetFPDesName(int nIndex = 0);

		/*!
		 *	\brief	����ͨ���ı�ǩƷ��
		 *
		 *	\param	uQuality ��ǩƷ��(Good or Bad)
		 *
		 *	\retval	int 0Ϊ�ɹ�, ����ʧ��
		 */
		int setQuality(int uQuality);

		/*!
		 *	\brief	��ȡͨ���ı�ǩƷ��
		 *
		 *	\param	nIndex �ɼ�ͨ����������
		 *
		 *	\retval	int ��ǩƷ��
		 */
		int GetQuality(int nIndex = 0);

		/*!
		 *	\brief	ͨ��������ȡ�ɼ�ͨ������ָ��
		 *
		 *	\param	nIndex ����
		 *
		 *	\retval	FieldPointType* �ɼ�ͨ������ָ��
		 */
		FieldPointType* GetFPByIndex(uint nIndex = 0);

	private:
		//!��Ӧ��CFeildPoint����ָ��
		T_mapFieldPoints m_mapFieldPoints;

		//!��ǩ�ļ����ַ
		int m_lFinalAddress;

		//!��ǩ����Unit_Type
		int m_nUnitType;
	};
}

#endif