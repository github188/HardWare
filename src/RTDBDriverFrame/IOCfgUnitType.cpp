/*!
 *	\file	IOCfgUnitType.cpp
 *
 *	\brief	一种类型标签信息管理类
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月17日	18:58
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
	//!构造函数
	CIOCfgUnitType::CIOCfgUnitType() : m_nTagCount(0)
	{
		memset(&m_tUnitType, 0, sizeof(tIOCfgUnitType));
	}

	//!析构函数
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
	
	//!得到标签个数
	int CIOCfgUnitType::GetTagCount()
	{
		return m_nTagCount;
	}

	//!通过索引获取标签对象
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

	//!通过标签计算地址获得标签对象
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

	//!添加标签
	int CIOCfgUnitType::AddTag(CIOCfgTag* pTag)
	{
		if (!pTag)
		{
			return -1;
		}

		//!加入标签MAP表
		m_mapIOTag.insert(T_MapIOTag::value_type(pTag->GetFinalAddr(), pTag));

		//!加入标签序号索引表
		m_vecTags.push_back(pTag);

		//!标签个数加1
		m_nTagCount ++;
		return 0;
	}

	//!返回标签类型为字符串标签地址(简单返回标签个数)
	int CIOCfgUnitType::GetStrTagCalcAddr()
	{
		return m_nTagCount;
	}

	//!初始化
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
	
	//!获取标签类型
	int CIOCfgUnitType::GetUnitTypeID()
	{
		return m_tUnitType.iUnitType;
	}

	//!获取UnitType
	int CIOCfgUnitType::GetUnitType(tIOCfgUnitType& unitType)
	{
		unitType = m_tUnitType;
		return 0;
	}

	//!获取位宽度
	int CIOCfgUnitType::GetBitWidth()
	{
		return m_tUnitType.iBitWidth;
	}

	//!获取值的字节个数
	int CIOCfgUnitType::GetByteCount()
	{
		return ( m_tUnitType.iBitWidth + 7 ) / 8;
	}

	//!设置一种标签类型结构
	void CIOCfgUnitType::SetUnitTypeCfg(tIOCfgUnitType& cfgUnitType)
	{
		m_tUnitType = cfgUnitType;
	}
}