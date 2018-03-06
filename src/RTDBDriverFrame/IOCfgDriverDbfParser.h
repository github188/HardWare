/*!
 *	\file	IOCfgDriverDbfParser.h
 *
 *	\brief	驱动对应的驱动名.dbf文件处理类
 *
 *	详细的文件说明（可选）
 *
 *	\author LIFAN
 *
 *	\date	2014年4月21日	9:49
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODRIVERDBFPARSER_H_
#define _IODRIVERDBFPARSER_H_

#include "RTDBDriverFrame/IODriverCommonDef.h"
#include "utilities/fepcommondef.h"
#include <vector>


namespace MACS_SCADA_SUD
{
	/*!
	 *	\class	CIOCfgDriverDbfParser
	 *
	 *	\brief	驱动标签地址解析类
	 *
	 *	支持格式:
	 *	%U：UnitAddress =VALUE。
	 *	%+U：UnitAddress += VALUE。
	 *	%*N：UnitAddress *= N。
	 *	%+N：UnitAddress += N。
	 *	%S：字符串; 标签的计算地址由配置管理自动分配。以"_"为分界,例如: DI_%S, DI_AA,DI_BB
	 *	数组：(注意:目前不支持)
	 *	如：标签名为Data，标签地址为DI0[5]，表示从标签地址DI0开始的5个标签:
	 *	Data[0]->DI0；Data[1]->DI1；Data[2]->DI2；Data[3]->DI3；Data[4]->DI4.
	 *
	 */
	class IODRIVERFRAME_API CIOCfgDriverDbfParser
	{
		//!标签格式描述信息索引表
		typedef std::vector<tIOCfgUnitDataDes> T_VecDataDes;

	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOCfgDriverDbfParser();

		/*!
		 *	\brief	缺省析构函数
		 */
		~CIOCfgDriverDbfParser();

		/*!
		 *	\brief	根据Template分解生成波兰表达式
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pchFormat: 标签地址
		 *	\param	tBol: 分解后的波兰表达式
		 *
		 *	\retval	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		int	MakeBolanExpress( const char* pchFormat, tBOLAN& tBol);

		/*!
		 *	\brief	计算标签的地址
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	tDataDes: 设备的标签格式描述信息
		 *	\param	plItemVal: 标签地址中各项的数值
		 *	\param	lTagAddr: 计算后得到的标签计算地址
		 *
		 *	\retval	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		int	CalcBolanExpress( tIOCfgUnitDataDes tDataDes, int* plItemVal, int& lTagAddr );

		/*!
		 *	\brief	计算标签地址的限值
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	设备的标签格式描述信息,输入输出, 修改其lLower和lUpper
		 *
		 *	\retval	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		int CalcTagAddrBound( tIOCfgUnitDataDes& tDataDes );

		/*!
		 *	\brief	增加一条描述
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	dataDes: 要增加的设备的标签格式描述信息
		 *
		 *	\retval	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		void AddDes( tIOCfgUnitDataDes& dataDes );

		/*!
		 *	\brief	获取标签地址模板的条数
		 */
		int	GetTagDesCount();

		/*!
		 *	\brief	通过索引号获取标签模板结构
		 *
		 *	\param	iIndex: 索引号
		 *	\param	tDataDes: 得到的标签格式描述信息
		 *
		 *	\retval	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		int	GetTagDesByIndex( int iIndex, tIOCfgUnitDataDes& tDataDes);

		/*!
		 *	\brief	通过标签的组态地址获取标签的描叙结构
		 *
		 *	\param	pchTagAddr: 标签地址
		 *	\param	tDataDes: 得到的标签格式描述信息
		 *
		 *	\retval	0为成功,其他失败
		 */
		int	GetTagDesByTagAddr( const char* pchTagAddr, tIOCfgUnitDataDes& tDataDes );

	private:
		//!标签格式信息描述索引表
		T_VecDataDes m_vecDes;
	};
}

#endif