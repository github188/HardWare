/*!
 *	\file	IOCfgUnitType.h
 *
 *	\brief	һ�ֱ�ǩ������Ϣ����ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��17��	18:54
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOCFGUNITTYPE_H_
#define _IOCFGUNITTYPE_H_

#include <map>
#include <string>
#include "RTDBDriverFrame/IODriverCommonDef.h"
#include "utilities/fepcommondef.h"


namespace MACS_SCADA_SUD
{
	class CIOCfgTag;

	/*!
	 *	\class	CIOCfgUnitType
	 *
	 *	\brief	һ�ֱ�ǩ������Ϣ������
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class IODRIVERFRAME_API CIOCfgUnitType
	{
		//!��ǩ��������
		typedef std::map<int, CIOCfgTag*> T_MapIOTag;
		typedef std::vector<CIOCfgTag*> T_VecIOTag;

	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOCfgUnitType();

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIOCfgUnitType();

		/*!
		 *	\brief	��ȡ��ǩ����
		 *
		 *	\param	��
		 *
		 *	\retval	int	��ǩ����
		 */
		int GetTagCount();

		/*!
		 *	\brief	�������Ż�ȡ��ǩ����ָ��
		 *
		 *	\param	lIndex	��ǩ��Ӱ
		 *
		 *	\retval	FieldPointType*	��ǩ����ָ��
		 */
		CIOCfgTag* GetTagByIndex( int lIndex );

		/*!
		 *	\brief	ͨ����ǩ�����ַ��ȡ��ǩ����ָ��
		 *
		 *	\param	lIndex	��ǩ��Ӱ
		 *
		 *	\retval	FieldPointType*	��ǩ����ָ��
		 */
		CIOCfgTag* GetTagByTagAddr( int lTagAddr );

		/*!
		 *	\brief	���б������ӱ�ǩ����ָ��
		 *
		 *	\param	FieldPointType*	��ǩ����
		 *
		 *	\retval	int	�ɹ�0,����ʧ��
		 */
		int	AddTag( CIOCfgTag* pTag );

		/*!
		 *	\brief	�����ַ��ʽΪ�ַ����ı�ǩ�ļ����ַ
		 *
		 *	\param	��
		 *
		 *	\retval	int �õ��ı�ǩ�����ַ
		 */
		int GetStrTagCalcAddr();

		/*!
		 *	\brief	��ʼ��
		 *
		 *	\param	tIOCfgUnitDataDes һ�ֱ�ǩ���͵������ṹ
		 *
		 *	\retval	int	0Ϊ�ɹ�������ʧ��
		 */
		int InitUnitType( tIOCfgUnitDataDes tDataDes );

		/*!
		 *	\brief	��ʼ��
		 *
		 *	\param	��
		 *
		 *	\retval	int	��ǩ����
		 */
		int GetUnitTypeID();

		/*!
		 *	\brief	��ȡ�ṹtIOCfgUnitType
		 *
		 *	\param	unitType: �õ����������ṹ
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		int GetUnitType( tIOCfgUnitType& unitType);

		/*!
		 *	\brief	��ȡλ���
		 *
		 *	\param	��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		int GetBitWidth();

		/*!
		 *	\brief	��ȡֵ���ֽڸ���
		 *
		 *	\param	��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		int GetByteCount();

		/*!
		 *	\brief	����һ�ֱ�ǩ���ͽṹ
		 *
		 *	\param	tIOCfgUnitType& ��ǩ���ͽṹ
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		void SetUnitTypeCfg( tIOCfgUnitType& cfgUnitType );

	private:
		//!��ǩ�����ַ���ǩָ��Ķ�Ӧ,ע��: �˴��޶���ͬ�ı�ǩ���ǩ��ַ������ͬ
		T_MapIOTag m_mapIOTag;

		//!��ǩ�����������ǩָ��Ķ�Ӧ,ÿ��UnitType�еı�ǩ��������Ϊ��0��ʼ����
		T_VecIOTag m_vecTags;

		//!��Ӧ������.dbf�ļ�������������Ϣ
		tIOCfgUnitType m_tUnitType;

		//!�����еı�ǩ����
		int m_nTagCount;

	};
}

#endif