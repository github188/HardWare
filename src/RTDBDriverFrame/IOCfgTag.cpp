/*!
 *	\file	IOCfgTag.cpp
 *
 *	\brief	��ǩ������(��ӦCCTool��ͨ����ַ)ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��19��	23:59
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
	
	//!���캯��
	CIOCfgTag::CIOCfgTag() :
		m_lFinalAddress(0),
		m_nUnitType(0)
	{
		
	}
	
	//!��������
	CIOCfgTag::~CIOCfgTag()
	{

	}

	//!��ȡ����Ӧ��FeildPoint����ָ��
	T_mapFieldPoints CIOCfgTag::GetFieldPoints()
	{
		return m_mapFieldPoints;
	}

	//!����FieldPoint����ָ��
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

	//!�õ���ǩ��name(���Ǳ�ǩ��ַ)
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

	//!�õ���ǩ�����õ�ַ
	std::string CIOCfgTag::GetTagAddr()
	{
		return GetName();
	}

	//!�õ���ǩ�ļ����ַ
	int CIOCfgTag::GetFinalAddr()
	{
		return m_lFinalAddress;
	}

	//!���ñ�ǩ�����ַ
	void CIOCfgTag::SetFinalAddress(int lFinalAdress)
	{
		m_lFinalAddress = lFinalAdress;
	}

	//!��ȡ��ǩ����Unit_Type
	int CIOCfgTag::GetUnitType()
	{
		return m_nUnitType;
	}

	//!���ñ�ǩ����
	void CIOCfgTag::SetUnitType(int nUnitType)
	{
		m_nUnitType = nUnitType;
	}

	//!�ֽ��ǩ��ַ���õ������ֵ
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
				digitStr[t] = lpStr[i];						//���ִ���Ҫת������ֵ���ַ���
				while('0'<=lpStr[i+1] && lpStr[i+1]<='9')
				{											//������������Ҫת������ֵ���ַ���
					digitStr[++t] = lpStr[i+1];
					i++;
				}
				digitStr[++t]='\0';
				lAddrVal[nAddrCount] = atol(digitStr);
				nAddrCount ++;
			}
		}
		if ( nAddrCount == 0 )								//�õ�ַ���ֽܷ�
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

	//�õ������ͱ�ǩ��ֵ
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