/*!
 *	\file	IODeviceSimBase.h
 *
 *	\brief	设备模拟类基类头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author LIFAN
 *
 *	\date	2014年4月22日	8:16
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODEVICESIMBASE_H_
#define _IODEVICESIMBASE_H_

#include "RTDBDriverFrame/IODriverH.h"
#include "RTDBCommon/IOTimer.h"

#define  PI 3.1415926

namespace MACS_SCADA_SUD
{
	class CIODeviceBase;
	class CIOCfgUnit;
	/*!
	 *	\class	CIODeviceSimBase
	 *
	 *	\brief	设备模拟类
	 *
	 *	详细的类说明（可选）
	 */
	class IODRIVERFRAME_API CIODeviceSimBase
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIODeviceSimBase();

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIODeviceSimBase();

		/*!
		 *	\brief	获取类名
		 */
		std::string GetClassName();

		/*!
		 *	\brief	获取类名
		 *
		 *	\param	bySimMode 模拟模式
		 *
		 *	\retval 无
		 */
		virtual void SetSimMode(Byte bySimMode);

		/*!
		 *	\brief	随机模拟
		 *
		 *	\param	p 
		 *
		 *	\retval 0为成功,其他失败
		 */
		virtual int Random(void* p);

		/*!
		 *	\brief	递增模拟
		 *
		 *	\param	p 
		 *
		 *	\retval 0为成功,其他失败
		 */
		virtual int Increase(void* p);

		/*!
		 *	\brief	正炫模拟
		 *
		 *	\param	p 
		 *
		 *	\retval 0为成功,其他失败
		 */
		virtual int Sine(void* p);

		/*!
		 *	\brief	启动模拟
		 *
		 *	\param	p 
		 *
		 *	\retval 0为成功,其他失败
		 */
		virtual int Start(void);

		/*!
		 *	\brief	启动模拟
		 *
		 *	\param	p 
		 *
		 *	\retval 0为成功,其他失败
		 */
		virtual int Stop(void);

		/*!
		 *	\brief	设置标签类别管理对象指针 
		 *
		 *	\param	pIOUnitCfg 标签类别管理对象指针 
		 *
		 *	\retval 0为成功,其他失败
		 */
		void SetIOCfgUnit(CIOCfgUnit* pIOUnitCfg);

		/*!
		 *	\brief	获取标签类别管理对象指针
		 *
		 *	\param	无
		 *
		 *	\retval CIOCfgUnit* 标签类别管理对象指针 
		 */
		CIOCfgUnit* GetIOCfgUnit();

		/*!
		 *	\brief	设置对应设备对象指针
		 *
		 *	\param	pParent	设备对象指针
		 *
		 *	\retval 无 
		 */
		void SetParent(CIODeviceBase* pParent);

		/*!
		 *	\brief	获取对应设备对象指针
		 *
		 *	\param	无
		 *
		 *	\retval CIODeviceBase* 设备对象指针 
		 */
		CIODeviceBase* GetParent();

		/*!
		 *	\brief	获取模拟类型
		 *
		 *	\param	无
		 *
		 *	\retval Byte 模拟类型
		 */
		Byte GetSimMode();

		/*!
		 *	\brief	得到随机字符
		 *
		 *	\param	chStr	得到的字符
		 *
		 *	\retval 无
		 */
		void GetRandString( char& chStr );

		//设置模拟的周期
		void SetSimPeriod( int nInternal );

	public:
		const static int FULLDEG = 360;
		int nDegree;
		//!类名
		static std::string s_ClassName;

		//!模拟模式
		Byte m_bySimMode;

		// //!标签类别管理对象指针
		CIOCfgUnit* m_pIOCfgUnit;

		// //!设备对象指针
		CIODeviceBase* m_pDevice;

		// //!定时器
		// CIOTimer* m_pIOTimer;

		//!模拟周期
		int m_nSimPeriod;
	};

	//!输出全局变量
	extern IODRIVERFRAME_VAR GenericFactory<CIODeviceSimBase> g_DrvSimFactory;
}

#endif