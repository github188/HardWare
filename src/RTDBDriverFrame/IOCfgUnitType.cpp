/*!
 *	\file	IOCfgUnitType.cpp
 *
 *	\brief	һ�����ͱ�ǩ��Ϣ������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��17��	18:58
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "RTDBDriverFrame/IOCfgUnitType.h"
#include "RTDBDriverFrame/IOCfgTag.h"
#include <string.h>

namespace MACS_SCADA_SUD
{
	//!���캯��
	CIOCfgUnitType::CIOCfgUnitType() : m_nTagCount(0)
	{
		memset(&m_tUnitType, 0, sizeof(tIOCfgUnitType));
	}

	//!��������
	CIOCfgUnitType::~CIOCfgUnitType()
	{
		std::vector<CIOCfgTag*>::iterator it;
		for (it = m_vecTags.begin(); it != m_vecTags.end(); it++)
		{
			CIOCfgTag* pTag = *it;
			if (pTag)
			{
				delete pTag;
				pTag = NULL;
			}
		}
		m_vecTags.clear();
		m_mapIOTag.clear();
	}
	
	//!�õ���ǩ����
	int CIOCfgUnitType::GetTagCount()
	{
		return m_nTagCount;
	}

	//!ͨ��������ȡ��ǩ����
	CIOCfgTag* CIOCfgUnitType::GetTagByIndex(int lIndex)
	{
		if ( lIndex < 0 || lIndex >= m_nTagCount )
		{
			return NULL;
		}
		else
		{
			return m_vecTags[lIndex];
		}
	}

	//!ͨ����ǩ�����ַ��ñ�ǩ����
	CIOCfgTag* CIOCfgUnitType::GetTagByTagAddr(int lTagAddr)
	{
		T_MapIOTag::iterator it;
		it = m_mapIOTag.find(lTagAddr);
		if (it != m_mapIOTag.end())
		{
			CIOCfgTag* pTag = it->second;
			if (pTag)
			{
				return pTag;
			}
		}
		return NULL;
	}

	//!��ӱ�ǩ
	int CIOCfgUnitType::AddTag(CIOCfgTag* pTag)
	{
		if (!pTag)
		{
			return -1;
		}

		//!�����ǩMAP��
		m_mapIOTag.insert(T_MapIOTag::value_type(pTag->GetFinalAddr(), pTag));

		//!�����ǩ���������
		m_vecTags.push_back(pTag);

		//!��ǩ������1
		m_nTagCount ++;
		return 0;
	}

	//!���ر�ǩ����Ϊ�ַ�����ǩ��ַ(�򵥷��ر�ǩ����)
	int CIOCfgUnitType::GetStrTagCalcAddr()
	{
		return m_nTagCount;
	}

	//!��ʼ��
	int CIOCfgUnitType::InitUnitType(tIOCfgUnitDataDes tDataDes)
	{
		m_tUnitType.iBitWidth = tDataDes.iBitWidth;
		m_tUnitType.iRawType = tDataDes.iRawType;
		m_tUnitType.iUnitType = tDataDes.iUnitType;
		m_tUnitType.lLower = tDataDes.lLower;
		m_tUnitType.lUpper = tDataDes.lUpper;
		m_tUnitType.lOffset = 0;
		strcpy( m_tUnitType.pchFormat, tDataDes.pchFormat );
		return 1;
	}
	
	//!��ȡ��ǩ����
	int CIOCfgUnitType::GetUnitTypeID()
	{
		return m_tUnitType.iUnitType;
	}

	//!��ȡUnitType
	int CIOCfgUnitType::GetUnitType(tIOCfgUnitType& unitType)
	{
		unitType = m_tUnitType;
		return 0;
	}

	//!��ȡλ���
	int CIOCfgUnitType::GetBitWidth()
	{
		return m_tUnitType.iBitWidth;
	}

	//!��ȡֵ���ֽڸ���
	int CIOCfgUnitType::GetByteCount()
	{
		return ( m_tUnitType.iBitWidth + 7 ) / 8;
	}

	//!����һ�ֱ�ǩ���ͽṹ
	void CIOCfgUnitType::SetUnitTypeCfg(tIOCfgUnitType& cfgUnitType)
	{
		m_tUnitType = cfgUnitType;
	}
}