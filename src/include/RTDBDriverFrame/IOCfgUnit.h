/*!
 *	\file	IOCfgUnit.h
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
	 *	\brief	标签类型信息管理类
	 *
	 *	详细的类说明（可选）
	 */
	class IODRIVERFRAME_API CIOCfgUnit
	{
		//!标签类型MAP表<UNIT_TYPE,一种标签类型信息管理指针>
		typedef std::map<int, CIOCfgUnitType*>	T_MapUnitType;

		//!标签管理对象MAP表(处理多个FeildPoint使用一个偏移地址)
		typedef std::map<std::string, CIOCfgTag*> T_MapCfgTag;

	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOCfgUnit(CIODeviceBase* m_pIODevice);

		/*!
		 *	\brief	缺省析构函数
		 */
		~CIOCfgUnit();

		/*!
		 *	\brief	获取标签类型个数
		 *
		 *	\param	无
		 *
		 *	\retval	int	标签类型个数
		 */
		int	GetUnitTypeCount();

		/*!
		 *	\brief	通过索引获取标签类型管理指针
		 *
		 *	\param	iIndex	索引号，从0开始
		 *
		 *	\retval	int	标签类型个数
		 */
		CIOCfgUnitType*	GetUnitTypeByIndex( int iIndex );

		/*!
		 *	\brief	通过标签类型获取标签管理对象
		 *
		 *	\param	nTypeID	UnitType号
		 *
		 *	\retval	CIOCfgUnitType*	得到的标签类型管理对象指针
		 */
		CIOCfgUnitType*	GetUnitTypeByID( int nTypeID );

		/*!
		 *	\brief	添加标签管理对象
		 *
		 *	\param	pUnitType	标签管理对象指针	
		 *
		 *	\retval	0为成功,其他失败
		 */
		int	AddUnitType(CIOCfgUnitType* pUnitType);

		/*!
		 *	\brief	初始化UnitType列表
		 *
		 *	\param	无
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		int	CreateUnitTypeList( );

		/*!
		 *	\brief	计算UnitType对象中的偏移值
		 *
		 *	\param	无
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		int	CalcUnitTypeOffset(	);

		/*!
		 *	\brief	添加CIOCfgTag索引信息
		 *
		 *	\param	strAddr		FieldPoint偏移地址
		 *	\param	pIOCfgTag	对象指针
		 *
		 *	\retval	无
		 */
		void AddIOCfgTag(std::string strAddr,CIOCfgTag* pIOCfgTag);

		/*!
		 *	\brief	通过FieldPoint的标签地址得到CIOCfgTag对象指针
		 *
		 *	\param	strAddr		FeildPoint中配置的偏移地址
		 *
		 *	\retval	CIOCfgTag*	对象指针
		 */
		CIOCfgTag* GetIOCfgTagByAddr(std::string strAddr);

		/*!
		 *	\brief	获取设备的配置文件名
		 *
		 *	\param	空
		 *
		 *	\retval	std::string	设备配置文件名
		 */
		std::string GetUnitCfgName();

		/*!
		 *	\brief	获取设备地址
		 *
		 *	\param	空
		 *
		 *	\retval	std::string	设备地址
		 */
		std::string GetAddr();

		/*!
		 *	\brief	获取设备的名称
		 *
		 *	\param	空
		 *
		 *	\retval	std::string	设备名称
		 */
		std::string GetName();

	private:
		//!标签组信息索引
		T_MapUnitType m_mapUnitTypeIndex;

		//!对应的地址空间中设备对象指针
		CIODeviceBase* m_pIODevice;

		//!标签管理对象MAP表
		T_MapCfgTag m_mapIOCfgTag;
	};
}

#endif