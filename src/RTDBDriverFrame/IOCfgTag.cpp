/*!
 *	\file	IOCfgTag.cpp
 *
 *	\brief	标签处理类(对应CCTool中通道地址)实现
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月19日	23:59
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include <string.h>
#include "RTDBCommon/KiwiVariant.h"
#include "RTDBDriverFrame/IOCfgTag.h"
#include "RTDBPlatformClass/FieldPointType.h"

namespace MACS_SCADA_SUD
{
	
	//!构造函数
	CIOCfgTag::CIOCfgTag() :
		m_lFinalAddress(0),
		m_nUnitType(0)
	{
		
	}
	
	//!析构函数
	CIOCfgTag::~CIOCfgTag()
	{

	}

	//!获取所对应的FeildPoint对象指针
	T_mapFieldPoints CIOCfgTag::GetFieldPoints()
	{
		return m_mapFieldPoints;
	}

	//!设置FieldPoint对象指针
	void CIOCfgTag::AddFieldPoint(FieldPointType* pFieldPoint)
	{
		if (pFieldPoint != NULL)
		{
			T_mapFieldPoints::iterator it = m_mapFieldPoints.find(pFieldPoint->nodeId());
			if (it == m_mapFieldPoints.end())
			{
				m_mapFieldPoints.insert(T_mapFieldPoints::value_type(pFieldPoint->nodeId(), pFieldPoint));
			}
		}
	}

	//!得到标签的name(就是标签地址)
	std::string CIOCfgTag::GetName()
	{
		std::string strDName = "";
		if (m_mapFieldPoints.size() > 0)
		{
			T_mapFieldPoints::iterator it = m_mapFieldPoints.begin();
			std::string sAddr = it->second->getOffset();
			if(!sAddr.empty())
			{
				strDName = sAddr;
			}
		} 
		return strDName;
	}

	//!得到标签的配置地址
	std::string CIOCfgTag::GetTagAddr()
	{
		return GetName();
	}

	//!得到标签的计算地址
	int CIOCfgTag::GetFinalAddr()
	{
		return m_lFinalAddress;
	}

	//!设置标签计算地址
	void CIOCfgTag::SetFinalAddress(int lFinalAdress)
	{
		m_lFinalAddress = lFinalAdress;
	}

	//!获取标签类型Unit_Type
	int CIOCfgTag::GetUnitType()
	{
		return m_nUnitType;
	}

	//!设置标签类型
	void CIOCfgTag::SetUnitType(int nUnitType)
	{
		m_nUnitType = nUnitType;
	}

	//!分解标签地址，得到各项的值
	int CIOCfgTag::AnalyzeTagAddr(int* plItemVal, int nLen)
	{
		if ( !plItemVal )
			return -1;

		int lAddrVal[32];
		char digitStr[32];
		int nAddrCount = 0;
		char lpStr[64];
		strcpy( lpStr, GetTagAddr().c_str());
		int t = 0;
		for( int i=0; lpStr[i]!='\0'; i++ )
		{
			if ('0'<=lpStr[i] && lpStr[i]<='9')
			{
				t = 0;
				digitStr[t] = lpStr[i];						//数字存入要转化成数值的字符串
				while('0'<=lpStr[i+1] && lpStr[i+1]<='9')
				{											//将连续数字入要转化成数值的字符串
					digitStr[++t] = lpStr[i+1];
					i++;
				}
				digitStr[++t]='\0';
				lAddrVal[nAddrCount] = atol(digitStr);
				nAddrCount ++;
			}
		}
		if ( nAddrCount == 0 )								//该地址不能分解
			return -2;
		if ( nAddrCount > nLen )
			return -3;
		memcpy( plItemVal, lAddrVal, nAddrCount * sizeof(int) );
		return 0;
	}

	// UaFloatArray CIOCfgTag::GetRawEngConfig()
	// {
	// 	UaFloatArray fArrValues;
	// 	fArrValues.create(4);
	// 	if (m_mapFieldPoints.size() > 0)
	// 	{
	// 		T_mapFieldPoints::iterator it = m_mapFieldPoints.begin();
	// 		fArrValues[0] = it->second->getRawZero();
	// 		fArrValues[1] = it->second->getRawFull();
	// 		fArrValues[2] = it->second->getEngZero();
	// 		fArrValues[3] = it->second->getEngFull();
	// 	}
	// 	return fArrValues;
	// }

	//得到该类型标签的值
	CKiwiVariant CIOCfgTag::GetValue()
	{
		CKiwiVariant uaValue;
		if (m_mapFieldPoints.size() > 0)
		{
			T_mapFieldPoints::iterator it = m_mapFieldPoints.begin();
			uaValue = it->second->value();
		}
		return uaValue;
	}

	FieldPointType* CIOCfgTag::GetFirst()
	{
		FieldPointType* pIOChannel = NULL;
		if (m_mapFieldPoints.size() > 0)
		{
			T_mapFieldPoints::iterator it = m_mapFieldPoints.begin();
			pIOChannel = it->second;
		}
		return pIOChannel;
	}

	std::string CIOCfgTag::GetFPDesName( int nIndex /*= 0*/ )
	{
		std::string strRes;
		FieldPointType* pFP = GetFPByIndex(nIndex);
		if (pFP )
		{
			strRes = pFP->displayName();
		}
		return strRes;
	}

	int CIOCfgTag::setQuality( int uQuality )
	{
		int nRes = -1;
		bool uStatus;
		for (int nIndex = 0; nIndex < m_mapFieldPoints.size(); nIndex++)
		{
			FieldPointType* pFP = GetFPByIndex(nIndex);
			if (pFP)
			{
				uStatus = pFP->setQualityDetail(uQuality);
				if (uStatus)
				{
					nRes = 0;
				}
				else
				{
					nRes = -1;
				}
			}
		}
		return nRes;
	}

	FieldPointType* CIOCfgTag::GetFPByIndex( uint nIndex /*= 0*/ )
	{
		FieldPointType* pFieldPoint = NULL;
		if (nIndex >= m_mapFieldPoints.size())
		{
			return pFieldPoint;
		}
		T_mapFieldPoints::iterator itFP = m_mapFieldPoints.begin();
		for (uint i = 0; i < nIndex; ++i)
		{
			itFP++;
		}
		pFieldPoint = itFP->second;

		return pFieldPoint;
	}

	int CIOCfgTag::GetQuality(int nIndex /*= 0*/ )
	{
		int nQuality = 0;
		FieldPointType* pFP = GetFPByIndex(nIndex);
		if (pFP)
		{
			nQuality = pFP->getQualityDetail();
		}
		return nQuality;
	}
}