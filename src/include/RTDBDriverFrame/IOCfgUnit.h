/*!
 *	\file	IOCfgUnit.h
 *
 *	\brief	�豸�ı�ǩ������Ϣ����
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��17��	18:33
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOCFGUNIT_H_
#define _IOCFGUNIT_H_

#include "RTDBDriverFrame/IODriverCommonDef.h"
#include <map>
#include "utilities/fepcommondef.h"

namespace MACS_SCADA_SUD
{
	class CIOCfgUnitType;
	class CIODeviceBase;
	class CIOCfgTag;

	/*!
	 *	\class	CIOCfgUnit
	 *
	 *	\brief	��ǩ������Ϣ������
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class IODRIVERFRAME_API CIOCfgUnit
	{
		//!��ǩ����MAP��<UNIT_TYPE,һ�ֱ�ǩ������Ϣ����ָ��>
		typedef std::map<int, CIOCfgUnitType*>	T_MapUnitType;

		//!��ǩ�������MAP��(������FeildPointʹ��һ��ƫ�Ƶ�ַ)
		typedef std::map<std::string, CIOCfgTag*> T_MapCfgTag;

	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOCfgUnit(CIODeviceBase* m_pIODevice);

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIOCfgUnit();

		/*!
		 *	\brief	��ȡ��ǩ���͸���
		 *
		 *	\param	��
		 *
		 *	\retval	int	��ǩ���͸���
		 */
		int	GetUnitTypeCount();

		/*!
		 *	\brief	ͨ��������ȡ��ǩ���͹���ָ��
		 *
		 *	\param	iIndex	�����ţ���0��ʼ
		 *
		 *	\retval	int	��ǩ���͸���
		 */
		CIOCfgUnitType*	GetUnitTypeByIndex( int iIndex );

		/*!
		 *	\brief	ͨ����ǩ���ͻ�ȡ��ǩ�������
		 *
		 *	\param	nTypeID	UnitType��
		 *
		 *	\retval	CIOCfgUnitType*	�õ��ı�ǩ���͹������ָ��
		 */
		CIOCfgUnitType*	GetUnitTypeByID( int nTypeID );

		/*!
		 *	\brief	��ӱ�ǩ�������
		 *
		 *	\param	pUnitType	��ǩ�������ָ��	
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int	AddUnitType(CIOCfgUnitType* pUnitType);

		/*!
		 *	\brief	��ʼ��UnitType�б�
		 *
		 *	\param	��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		int	CreateUnitTypeList( );

		/*!
		 *	\brief	����UnitType�����е�ƫ��ֵ
		 *
		 *	\param	��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		int	CalcUnitTypeOffset(	);

		/*!
		 *	\brief	���CIOCfgTag������Ϣ
		 *
		 *	\param	strAddr		FieldPointƫ�Ƶ�ַ
		 *	\param	pIOCfgTag	����ָ��
		 *
		 *	\retval	��
		 */
		void AddIOCfgTag(std::string strAddr,CIOCfgTag* pIOCfgTag);

		/*!
		 *	\brief	ͨ��FieldPoint�ı�ǩ��ַ�õ�CIOCfgTag����ָ��
		 *
		 *	\param	strAddr		FeildPoint�����õ�ƫ�Ƶ�ַ
		 *
		 *	\retval	CIOCfgTag*	����ָ��
		 */
		CIOCfgTag* GetIOCfgTagByAddr(std::string strAddr);

		/*!
		 *	\brief	��ȡ�豸�������ļ���
		 *
		 *	\param	��
		 *
		 *	\retval	std::string	�豸�����ļ���
		 */
		std::string GetUnitCfgName();

		/*!
		 *	\brief	��ȡ�豸��ַ
		 *
		 *	\param	��
		 *
		 *	\retval	std::string	�豸��ַ
		 */
		std::string GetAddr();

		/*!
		 *	\brief	��ȡ�豸������
		 *
		 *	\param	��
		 *
		 *	\retval	std::string	�豸����
		 */
		std::string GetName();

	private:
		//!��ǩ����Ϣ����
		T_MapUnitType m_mapUnitTypeIndex;

		//!��Ӧ�ĵ�ַ�ռ����豸����ָ��
		CIODeviceBase* m_pIODevice;

		//!��ǩ�������MAP��
		T_MapCfgTag m_mapIOCfgTag;
	};
}

#endif