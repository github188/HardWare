#ifndef DBOBJECTBASETYPE_H
#define DBOBJECTBASETYPE_H

#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/KiwiVariant.h"
#include <string.h>

#ifndef DBOBJECTBASE_API
#ifdef DBBASECLASSES_EXPORTS
#define DBOBJECTBASE_API MACS_DLL_EXPORT
#else
#define DBOBJECTBASE_API MACS_DLL_IMPORT
#endif
#endif

namespace MACS_SCADA_SUD
{
	typedef enum PointCategory
	{
		Other = 0,
		AnalogItem,				//! 模拟量
		DiscreteItem,			//! 离散量
		Device,					//! EQ设备
		Area					//! 区域
	};

	class DBOBJECTBASE_API CDBObjectBaseType
	{
	public:
		CDBObjectBaseType();
		virtual ~CDBObjectBaseType();
	public:
		//! 区域信息
		virtual long getArea1() const;
		virtual long getArea2() const;
		virtual long getArea3() const;

		virtual void setArea1(long nodeId);
		virtual void setArea2(long nodeId);
		virtual void setArea3(long nodeId);

		//! 获取该对象属于哪个设备
		virtual std::string getDeviceName() = 0;
		virtual void setDevice(const long& nodeId) = 0;
		virtual long getDevice() = 0;

		//! 获取当前点类型
		virtual PointCategory getPointCategory() = 0;

		//! 获取报警推图
		virtual std::string getAlarmGraph() const = 0;

		//! 获取点当前值
		virtual CKiwiVariant getPointCurrentValue() = 0;

	private:
		//! 区域信息
		long		m_Area1;
		long		m_Area2;
		long		m_Area3;
	};
}

#endif

