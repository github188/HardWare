/*!
*	\file	FieldPointType.h
*
*	\brief	FieldPoint作为一种变量数据类型派生自BaseDataVariableType
*			它带有一个可选的通道地址,用来存放数据的I/O通道信息,
*			等价于M3 FEP上的Tag
*
*	$Author: Wangyuanxing $
*	$Date: 14-04-01 9:09 $
*	$Revision: 1.0 $
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef __FIELDPOINTTYPE_H__
#define __FIELDPOINTTYPE_H__
#include "RTDBPlatformClass/fep_identifiers.h"
#include "RTDBPlatformClass/GeneralConfig.h"
#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/datetime.h"
#include "RTDBCommon/KiwiVariant.h"
#include "RTDBCommon/nodebase.h"
#include <map>
#include <string>

#ifndef FEPPLATFORMCLASS_API
#ifdef FEPPLATFORMCLASS_EXPORTS
# define FEPPLATFORMCLASS_API MACS_DLL_EXPORT
#else
# define FEPPLATFORMCLASS_API MACS_DLL_IMPORT
#endif
#endif

namespace MACS_SCADA_SUD
{
	class CIOUnit;
	class ComponentType;
	/*!
	*	\class	FieldPointType
	*
	*	\brief	通道地址,用来存放数据的I/O通道信息
	*/
	class FEPPLATFORMCLASS_API FieldPointType
	 : public CNode
	{
		FieldPointType();
	protected:
		virtual ~FieldPointType();
	public:
		static void createTypes();
		static void clearStaticMembers();

		virtual void SetConfigValues(std::map<std::string, CKiwiVariant>& values);
		virtual void CreateOptionalProperty(std::map<std::string, CKiwiVariant>& values);

		virtual void			setChannelType(ChannelType type);
		virtual ChannelType		getChannelType() const;
		virtual void			setIOUnit(CIOUnit* pIOUnit);
		virtual CIOUnit*		getIOUnit();

		//UaMutex m_statusCodeLock; //标签值状态码读写锁

		virtual void setDeviceAddress(const std::string& deviceAddress);
		virtual std::string getDeviceAddress() const;

		virtual void setOffset(const std::string& offset);
		virtual std::string getOffset() const;

		// NeedConverse field
		virtual void setNeedConverse(bool setvalue);
		virtual bool getNeedConverse() const;

		virtual void setRawZero(float rawZero);
		virtual float getRawZero() const;

		virtual void setRawFull(float rawFull);
		virtual float getRawFull() const;

		virtual void setEngZero(float engZero);
		virtual float getEngZero() const;

		virtual void setEngFull(float engFull);
		virtual float getEngFull() const;

		// RangSwtMode field
		virtual void setRangSwtMode(unsigned short setvalue);
		virtual unsigned short getRangSwtMode() const;

		// NeedSOE field
		virtual void setNeedSOE(bool setvalue);
		virtual bool getNeedSOE() const;

		//! AbsDeadZone
		void setAbsDeadZone(float setvalue);
		float getAbsDeadZone();

		// ValueDeses field
		virtual void setValueDes(const int iValue, std::string strDes);
		virtual std::string getValueDes(const int iValue) const;

		Byte 		setValue(CKiwiVariant* pVarValue, int nDataLen, long nTime);
		CKiwiVariant Value();
		virtual Byte setQualityDetail(uint quality);
		virtual uint getQualityDetail();

		//标签强制/取消强制
		virtual Byte force(Byte * pbyData, int nDataLen);
		virtual Byte cancelForce();

		virtual Byte Disable();
		virtual Byte cancelDisable();
		//设置标志在线/离线
		virtual Byte setOnline( long dateTime );
		virtual Byte setOffline( long dateTime );

		/*!
		 *	\brief	进行工程数据和原始数据的转换
		 *
		 *	\param	pByData		数据指针
		 *	\param	isEngToRaw	True:原始数据转换为工程数据,False:工程数据转换为原始数据
		 *
		 *	\retval	无
		 */
		void RangeData(Byte* pByData, int nDataLen,  bool isRawToEng);

		/*!
		 *	\brief	重写父类Value方法如果设备处于INIT(初始)态则标签也处于INIT(初始)态
		 *
		 *	\param	pSession	session指针
		 *
		 *	\retval	UaDataValue
		 */
		bool getDecCommFlag();

		//!设置采集通道的采样周期
		void SetSamplingInterval(unsigned int nSamplingInterval);

		//!获取采集通道采样周期
		unsigned int GetSamplingInterval();

		/*!
		 *	\brief	当实时服务与FEP链接恢复时设置UADriver中采集通道的UaVariant状态
		 *			恢复之前保留的采集通道的状态
		 *
		 *	\param	dateTime	时标信息
		 *
		 *	\retval	void
		 */
		void setUATagOnline( long dateTime );

		/*!
		 *	\brief	当实时服务与FEP链接断开时设置UADriver中采集通道的状态
		 *			保留当前采集通道的状态然后再将通道状态置为离线
		 *
		 *	\param	dateTime	时标信息
		 *
		 *	\retval	void
		 */
		std::string displayName();

		/*!
		 *	\brief	当实时服务与FEP链接断开时设置UADriver中采集通道的状态
		 *			保留当前采集通道的状态然后再将通道状态置为离线
		 *
		 *	\param	dateTime	时标信息
		 *
		 *	\retval	void
		 */
		void setTagOffline( long dateTime );

		//! read or write data variable

		//virtual void setChildValue(const std::string& sName, const CKiwiVariant& value);

		//virtual CKiwiVariant getChildValue(const std::string& sName) const;

		virtual CKiwiVariant value();
	private:
		void initialize();

	private:
        // ComponentType*                  m_pParent;
        CIOUnit*                        m_pIOUnit;
		ChannelType						m_Type;					//!in or out型标志
		CKiwiVariant                    m_realVal;	//!强制/禁止状态期间的标签实际值
		float						    m_fAbsDeadZone;
		unsigned int					m_nSamplingInterval;	//!采集通道采样周期
		bool						    m_uOldStatus;			//!采集通道上一时刻的状态
	};
}

#endif
