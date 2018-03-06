/*!
 *	\file	CIOCfgUnit.cpp
 *
 *	\brief	设备的标签类型信息管理
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月17日	18:33
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBDriverFrame/IOCfgUnit.h"
#include "RTDBDriverFrame/IOCfgUnitType.h"
#include "IOCfgDriverDbfParser.h"
#include "RTDBDriverFrame/IODeviceBase.h"

namespace MACS_SCADA_SUD
{
	//!构造函数
	CIOCfgUnit::CIOCfgUnit(CIODeviceBase* pIODevice) 
	{
		m_pIODevice = pIODevice;
	}

	//!析构函数
	CIOCfgUnit::~CIOCfgUnit()
	{
		T_MapUnitType::iterator it;
		for (it = m_mapUnitTypeIndex.begin(); it != m_mapUnitTypeIndex.end(); it++)
		{
			CIOCfgUnitType* pUnitType = it->second;
			if (pUnitType)
			{
				delete pUnitType;
				pUnitType = NULL;
			}
		}
		m_mapUnitTypeIndex.clear();

		m_mapIOCfgTag.clear();
	}

	// //!获取标签类型个数
	int CIOCfgUnit::GetUnitTypeCount()
	{
		return static_cast<int>(m_mapUnitTypeIndex.size());
	}

	//!通过索引获取标签类型管理指针
	CIOCfgUnitType* CIOCfgUnit::GetUnitTypeByIndex(int iIndex)
	{
		if (m_mapUnitTypeIndex.empty())
		{
			return NULL;
		}
		if (iIndex > static_cast<int>(m_mapUnitTypeIndex.size()))
		{
			return NULL;
		}
		T_MapUnitType::iterator it;
		it = m_mapUnitTypeIndex.begin();
		for (int i = 0; i < iIndex; i++)
		{
			it ++;
		}
		return it->second;
	}

	//!通过标签类型获取标签管理对象
	CIOCfgUnitType* CIOCfgUnit::GetUnitTypeByID(int nTypeID)
	{
		T_MapUnitType::iterator it;
		it = m_mapUnitTypeIndex.find(nTypeID);
		if (it != m_mapUnitTypeIndex.end())
		{
			return it->second;
		}
		return NULL;
	}

	//!添加标签管理对象
	int CIOCfgUnit::AddUnitType(CIOCfgUnitType* pUnitType)
	{
		//mod for BUG62448 on 20151229 below 
		int rcd = -1;
		if (pUnitType)
		{
			m_mapUnitTypeIndex.insert(T_MapUnitType::value_type(pUnitType->GetUnitTypeID(), pUnitType));
			rcd = 0;
		}
		return rcd;
		//mod for BUG62448 on 20151229 above 
	}

	//!初始化UnitType列表
	int CIOCfgUnit::CreateUnitTypeList()
	{
		//Mod for bug 62452 on 20151229 below 
		//CIOCfgDriverDbfParser* pDbfParser = m_pIODevice->GetDriver()->GetDbfParser();
		if (NULL == m_pIODevice)
		{
			return -1;
		}
		CIODriverBase* pDrvBase  = m_pIODevice->GetDriver();
		if (NULL == pDrvBase)
		{
			return -1;
		}
		//Mod for bug 62452 on 20151229 above

		CIOCfgDriverDbfParser* pDbfParser = pDrvBase->GetDbfParser();
		if (pDbfParser != NULL)
		{
			int nDesCount = pDbfParser->GetTagDesCount();
			for (int i = 0; i < nDesCount; i++)
			{
				tIOCfgUnitDataDes tDataDes;
				if (pDbfParser->GetTagDesByIndex(i, tDataDes) == 0)
				{
					CIOCfgUnitType* pUnitType = NULL;
					pUnitType = new CIOCfgUnitType();
					if (pUnitType != NULL)
					{
						pUnitType->InitUnitType(tDataDes);
						AddUnitType(pUnitType);
					}
					else
					{
						return -1;
					}
				}
				else
				{
					return -1;
				}
			}
		}
		else
		{
			return -1;
		}
		return 0;
	}

	//!计算UnitType对象中的偏移值
	int CIOCfgUnit::CalcUnitTypeOffset()
	{
		int lTemp = 0;
		int lAllOff = 0;
		CIOCfgUnitType* pUnitType = NULL;
		T_MapUnitType::iterator it;
		for( it=m_mapUnitTypeIndex.begin(); it!=m_mapUnitTypeIndex.end(); it++ )
		{
			pUnitType = it->second;
			if ( !pUnitType )
				return -1;
			tIOCfgUnitType unitType;
			pUnitType->GetUnitType( unitType );
			unitType.lOffset = lAllOff;
			lTemp = pUnitType->GetTagCount() * unitType.iBitWidth;		
			if ( lTemp % 8 > 0 )
				lTemp = lTemp / 8 + 1;
			else
				lTemp = lTemp / 8;
			lAllOff += lTemp;
		}
		return 0;
	}

	//!添加CIOCfgTag索引信息
	void CIOCfgUnit::AddIOCfgTag(std::string strAddr,CIOCfgTag* pIOCfgTag)
	{
		if (pIOCfgTag != NULL && !strAddr.empty())
		{
			m_mapIOCfgTag.insert(T_MapCfgTag::value_type(strAddr, pIOCfgTag));
		}
	}

	//!通过FieldPoint的标签地址得到CIOCfgTag对象指针
	CIOCfgTag* CIOCfgUnit::GetIOCfgTagByAddr(std::string strAddr)
	{
		if (!strAddr.empty())
		{
			T_MapCfgTag::iterator itTag;
			itTag = m_mapIOCfgTag.find(strAddr);
			if (itTag != m_mapIOCfgTag.end())
			{
				return itTag->second;
			} 
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}

	//!获取设备的配置文件名
	std::string CIOCfgUnit::GetUnitCfgName()
	{
		std::string strDevCfgName = "";
		if (m_pIODevice)
		{
			strDevCfgName = m_pIODevice->GetDevCfgName();
		}
		return strDevCfgName;
	}
	

	//!获取设备地址
	std::string CIOCfgUnit::GetAddr()
	{
		std::string strDevAddr = "";
		if (m_pIODevice)
		{
			strDevAddr = m_pIODevice->GetStringAddr();
		}
		return strDevAddr;
	}

	//!获取设备的名称
	std::string CIOCfgUnit::GetName()
	{
		std::string strDevName = "";
		if (m_pIODevice)
		{
			strDevName = m_pIODevice->GetName();
		}
		return strDevName;
	}
}