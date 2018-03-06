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
		AnalogItem,				//! ģ����
		DiscreteItem,			//! ��ɢ��
		Device,					//! EQ�豸
		Area					//! ����
	};

	class DBOBJECTBASE_API CDBObjectBaseType
	{
	public:
		CDBObjectBaseType();
		virtual ~CDBObjectBaseType();
	public:
		//! ������Ϣ
		virtual long getArea1() const;
		virtual long getArea2() const;
		virtual long getArea3() const;

		virtual void setArea1(long nodeId);
		virtual void setArea2(long nodeId);
		virtual void setArea3(long nodeId);

		//! ��ȡ�ö��������ĸ��豸
		virtual std::string getDeviceName() = 0;
		virtual void setDevice(const long& nodeId) = 0;
		virtual long getDevice() = 0;

		//! ��ȡ��ǰ������
		virtual PointCategory getPointCategory() = 0;

		//! ��ȡ������ͼ
		virtual std::string getAlarmGraph() const = 0;

		//! ��ȡ�㵱ǰֵ
		virtual CKiwiVariant getPointCurrentValue() = 0;

	private:
		//! ������Ϣ
		long		m_Area1;
		long		m_Area2;
		long		m_Area3;
	};
}

#endif

