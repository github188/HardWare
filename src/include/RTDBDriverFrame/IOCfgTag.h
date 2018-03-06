/*!
 *	\file	IOCfgTag.h
 *
 *	\brief	标签处理类(对应CCTool中通道地址)
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月18日	13:15
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
	 *	\brief	标签信息处理类
	 *
	 *	详细的类说明（可选）
	 */
	class IODRIVERFRAME_API CIOCfgTag
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOCfgTag();

		/*!
		 *	\brief	缺省析构函数
		 */
		~CIOCfgTag();

		/*!
		 *	\brief	获取所对应的FeildPoint对象指针
		 *
		 *	\param	无
		 *
		 *	\retval	T_lstFieldPoint	FieldPoint对象指针列表
		 */
		T_mapFieldPoints GetFieldPoints();

		/*!
		 *	\brief	设置FieldPoint对象指针
		 *
		 *	\param	FieldPointType* FieldPoint对象指针
		 *
		 *	\retval	无
		 */
		void AddFieldPoint(FieldPointType* pFieldPoint);

		/*!
		 *	\brief	取得标签名字,从FieldPoint对象中获取
		 *
		 *	\param	无
		 *
		 *	\retval	std::string 标签名
		 */
		std::string GetName();

		/*!
		 *	\brief	得到标签组态地址,从FieldPoint对象中获取
		 *
		 *	\param	无
		 *
		 *	\retval	std::string 标签组态地址
		 */
		std::string	GetTagAddr();

		/*!
		 *	\brief	获取标签计算地址
		 *
		 *	\param	无
		 *
		 *	\retval	int 标签计算地址
		 */
		int GetFinalAddr();

		/*!
		 *	\brief	设置标签计算地址
		 *
		 *	\param	lFinalAdress	标签计算地址
		 *
		 *	\retval	无
		 */
		void SetFinalAddress(int lFinalAdress);

		/*!
		 *	\brief	获取标签类型Unit_Type
		 *
		 *	\param	无
		 *
		 *	\retval	int 标签类型
		 */
		int	GetUnitType();

		/*!
		 *	\brief	设置标签类型
		 *
		 *	\param	nUnitType	标签类型
		 *
		 *	\retval	无
		 */
		void SetUnitType(int nUnitType);

		/*!
		 *	\brief	分解标签地址，得到各项的值
		 *
		 *	\param	plItemVal	存放分解后的地址项的数组首指针
		 *	\param	nLen		数组长度
		 *
		 *	\retval	int 0为成功，其他失败:-1为数组指针无效；-2为标签地址不能分解；-3分解后的个数大于数组长度
		 */
		int AnalyzeTagAddr( int* plItemVal, int nLen );

		/*!
		 *	\brief	得到标签工程数据转换信息
		 *
		 *	\retval	UaFloatArray 用户配置的工程数据转换信息
		 */
		//UaFloatArray GetRawEngConfig();

		/*!
		 *	\brief	得到该类型标签的值
		 *
		 *	\retval	UaDataValue 通道的值
		 */
		CKiwiVariant GetValue();

		//!获取通道MAP中第一个通道
		FieldPointType* GetFirst();

		/*!
		 *	\brief	获取采集通道显示名
		 *
		 *	\param	nIndex:标签索引(默认从0开始)
		 *
		 *	\retval	std::string 采集通道显示名
		 */
		std::string GetFPDesName(int nIndex = 0);

		/*!
		 *	\brief	设置通道的标签品质
		 *
		 *	\param	uQuality 标签品质(Good or Bad)
		 *
		 *	\retval	int 0为成功, 其他失败
		 */
		int setQuality(int uQuality);

		/*!
		 *	\brief	获取通道的标签品质
		 *
		 *	\param	nIndex 采集通道对象索引
		 *
		 *	\retval	int 标签品质
		 */
		int GetQuality(int nIndex = 0);

		/*!
		 *	\brief	通过索引获取采集通道对象指针
		 *
		 *	\param	nIndex 索引
		 *
		 *	\retval	FieldPointType* 采集通道对象指针
		 */
		FieldPointType* GetFPByIndex(uint nIndex = 0);

	private:
		//!对应的CFeildPoint对象指针
		T_mapFieldPoints m_mapFieldPoints;

		//!标签的计算地址
		int m_lFinalAddress;

		//!标签类型Unit_Type
		int m_nUnitType;
	};
}

#endif