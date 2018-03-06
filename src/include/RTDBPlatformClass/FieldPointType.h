/*!
*	\file	FieldPointType.h
*
*	\brief	FieldPoint��Ϊһ�ֱ�����������������BaseDataVariableType
*			������һ����ѡ��ͨ����ַ,����������ݵ�I/Oͨ����Ϣ,
*			�ȼ���M3 FEP�ϵ�Tag
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
	*	\brief	ͨ����ַ,����������ݵ�I/Oͨ����Ϣ
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

		//UaMutex m_statusCodeLock; //��ǩֵ״̬���д��

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

		//��ǩǿ��/ȡ��ǿ��
		virtual Byte force(Byte * pbyData, int nDataLen);
		virtual Byte cancelForce();

		virtual Byte Disable();
		virtual Byte cancelDisable();
		//���ñ�־����/����
		virtual Byte setOnline( long dateTime );
		virtual Byte setOffline( long dateTime );

		/*!
		 *	\brief	���й������ݺ�ԭʼ���ݵ�ת��
		 *
		 *	\param	pByData		����ָ��
		 *	\param	isEngToRaw	True:ԭʼ����ת��Ϊ��������,False:��������ת��Ϊԭʼ����
		 *
		 *	\retval	��
		 */
		void RangeData(Byte* pByData, int nDataLen,  bool isRawToEng);

		/*!
		 *	\brief	��д����Value��������豸����INIT(��ʼ)̬���ǩҲ����INIT(��ʼ)̬
		 *
		 *	\param	pSession	sessionָ��
		 *
		 *	\retval	UaDataValue
		 */
		bool getDecCommFlag();

		//!���òɼ�ͨ���Ĳ�������
		void SetSamplingInterval(unsigned int nSamplingInterval);

		//!��ȡ�ɼ�ͨ����������
		unsigned int GetSamplingInterval();

		/*!
		 *	\brief	��ʵʱ������FEP���ӻָ�ʱ����UADriver�вɼ�ͨ����UaVariant״̬
		 *			�ָ�֮ǰ�����Ĳɼ�ͨ����״̬
		 *
		 *	\param	dateTime	ʱ����Ϣ
		 *
		 *	\retval	void
		 */
		void setUATagOnline( long dateTime );

		/*!
		 *	\brief	��ʵʱ������FEP���ӶϿ�ʱ����UADriver�вɼ�ͨ����״̬
		 *			������ǰ�ɼ�ͨ����״̬Ȼ���ٽ�ͨ��״̬��Ϊ����
		 *
		 *	\param	dateTime	ʱ����Ϣ
		 *
		 *	\retval	void
		 */
		std::string displayName();

		/*!
		 *	\brief	��ʵʱ������FEP���ӶϿ�ʱ����UADriver�вɼ�ͨ����״̬
		 *			������ǰ�ɼ�ͨ����״̬Ȼ���ٽ�ͨ��״̬��Ϊ����
		 *
		 *	\param	dateTime	ʱ����Ϣ
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
		ChannelType						m_Type;					//!in or out�ͱ�־
		CKiwiVariant                    m_realVal;	//!ǿ��/��ֹ״̬�ڼ�ı�ǩʵ��ֵ
		float						    m_fAbsDeadZone;
		unsigned int					m_nSamplingInterval;	//!�ɼ�ͨ����������
		bool						    m_uOldStatus;			//!�ɼ�ͨ����һʱ�̵�״̬
	};
}

#endif
