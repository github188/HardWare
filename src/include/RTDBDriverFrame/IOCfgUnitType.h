/*!
 *	\file	IOCfgUnitType.h
 *
 *	\brief	一种标签类型信息管理头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月17日	18:54
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
	 *	\brief	一种标签类型信息管理类
	 *
	 *	详细的类说明（可选）
	 */
	class IODRIVERFRAME_API CIOCfgUnitType
	{
		//!标签索引表定义
		typedef std::map<int, CIOCfgTag*> T_MapIOTag;
		typedef std::vector<CIOCfgTag*> T_VecIOTag;

	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOCfgUnitType();

		/*!
		 *	\brief	缺省析构函数
		 */
		~CIOCfgUnitType();

		/*!
		 *	\brief	获取标签个数
		 *
		 *	\param	无
		 *
		 *	\retval	int	标签个数
		 */
		int GetTagCount();

		/*!
		 *	\brief	按索引号获取标签对象指针
		 *
		 *	\param	lIndex	标签缩影
		 *
		 *	\retval	FieldPointType*	标签对象指针
		 */
		CIOCfgTag* GetTagByIndex( int lIndex );

		/*!
		 *	\brief	通过标签计算地址获取标签对象指针
		 *
		 *	\param	lIndex	标签缩影
		 *
		 *	\retval	FieldPointType*	标签对象指针
		 */
		CIOCfgTag* GetTagByTagAddr( int lTagAddr );

		/*!
		 *	\brief	在列表中增加标签对象指针
		 *
		 *	\param	FieldPointType*	标签对象
		 *
		 *	\retval	int	成功0,其他失败
		 */
		int	AddTag( CIOCfgTag* pTag );

		/*!
		 *	\brief	计算地址格式为字符串的标签的计算地址
		 *
		 *	\param	无
		 *
		 *	\retval	int 得到的标签计算地址
		 */
		int GetStrTagCalcAddr();

		/*!
		 *	\brief	初始化
		 *
		 *	\param	tIOCfgUnitDataDes 一种标签类型的描述结构
		 *
		 *	\retval	int	0为成功，其他失败
		 */
		int InitUnitType( tIOCfgUnitDataDes tDataDes );

		/*!
		 *	\brief	初始化
		 *
		 *	\param	无
		 *
		 *	\retval	int	标签类型
		 */
		int GetUnitTypeID();

		/*!
		 *	\brief	获取结构tIOCfgUnitType
		 *
		 *	\param	unitType: 得到数据描述结构
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		int GetUnitType( tIOCfgUnitType& unitType);

		/*!
		 *	\brief	获取位宽度
		 *
		 *	\param	无
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		int GetBitWidth();

		/*!
		 *	\brief	获取值的字节个数
		 *
		 *	\param	无
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		int GetByteCount();

		/*!
		 *	\brief	设置一种标签类型结构
		 *
		 *	\param	tIOCfgUnitType& 标签类型结构
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		void SetUnitTypeCfg( tIOCfgUnitType& cfgUnitType );

	private:
		//!标签计算地址与标签指针的对应,注意: 此处限定不同的标签其标签地址不能相同
		T_MapIOTag m_mapIOTag;

		//!标签的索引号与标签指针的对应,每个UnitType中的标签的索引号为从0开始分配
		T_VecIOTag m_vecTags;

		//!对应驱动名.dbf文件中类型描述信息
		tIOCfgUnitType m_tUnitType;

		//!本组中的标签个数
		int m_nTagCount;

	};
}

#endif