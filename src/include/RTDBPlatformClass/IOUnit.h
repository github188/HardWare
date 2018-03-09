/*!
 *	\file	IOUnit.h
 *
 *	\brief	�豸��
 *
 *
 *	\author lingling.li
 *
 *	\date	2014��4��14��	16:27
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4ϵͳ������,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */


#ifndef __IOUnit_H__
#define __IOUnit_H__

#include "RTDBPlatformClass/fep_identifiers.h"
#include "RTDBPlatformClass/IOPort.h"
#include "RTDBPlatformClass/FieldPointType.h"
#include "RTDBPlatformClass/FepObjectType.h"
#include "RTDBCommon/KiwiVariable.h"

#ifndef FEPPLATFORMCLASS_API
#ifdef FEPPLATFORMCLASS_EXPORTS
# define FEPPLATFORMCLASS_API __declspec(dllexport)
#else
# define FEPPLATFORMCLASS_API __declspec(dllimport)
#endif
#endif

// Namespace
namespace MACS_SCADA_SUD 
{
	class CIODeviceBase;
	class CIODeviceSimBase;

	class FEPPLATFORMCLASS_API CIOUnit:
		public CFepObjectType
	{
	protected:
		virtual ~CIOUnit();
	public:
		// construction / destruction
		CIOUnit(const long& nId);
		CIOUnit(const long& nodeId, const std::string& name);
		CIOUnit(const long& nodeId, const std::string& szUnit, const std::string& szDevice);

		static void createTypes();
		static void clearStaticMembers();

		virtual long  typeDefinitionId() const;
		//�����豸������ͨ��
		void setIOPort(CIOPort* pIOPort);
		//������ݱ���������ΪIO������PV���������ڲ���������ʵʱ�����
		bool addTag(CKiwiVariable* pTag);
		//ɾ�����ݱ���������ΪIO������PV���������ڲ���������ʵʱ�����
		bool DeleteTag(CKiwiVariable* pTag);
		//��ȡ���ݱ���������ΪIO������PV���������ڲ�����
		void GetAllTags(std::vector<CKiwiVariable*> &vecTags);

		std::string GetUnitName(void);
		/*!
		*	\brief	��ȡ�豸��ַ
		*
		*	\retval	�豸��ַ
		*/
		int GetDevAddr();

		/*!
		*	\brief	�����豸��ַ
		*
		*	\param	iDeviceAddr �豸��ַ
		*/
		void SetDevAddr(int iDeviceAddr);

		/*!
		*	\brief	��ȡ�豸�����ļ���
		*
		*	\retval	�豸�����ļ���
		*/
		std::string GetDevCfgFileName();

		/*!
		*	\brief	�豸�Ƿ�����
		*
		*	\retval	����״̬��true��ʾ���ߣ�false��ʾ������
		*/
		bool IsOnline();

		/*!
		*	\brief	�豸�Ƿ�����
		*
		*	\retval	����״̬��true��ʾ���ߣ�false��ʾ������
		*/
		bool IsOffLine();

		/*!
		*	\brief	��ȡ�豸����״̬
		*
		*	\retval	����״̬����ʼ:0������1������:2
		*/
		Byte GetRunState();

		/*!
		*	\brief	�豸�Ƿ�ģ��
		*
		*	\retval	�Ƿ���ģ��״̬
		*/
		bool IsSimulate();

		//!������·���л�״̬
		void SetIOPortSwitchStatus(IOPortSwitchStatus flag);

		//!�õ���·�л�״̬
		IOPortSwitchStatus GetIOPortSwitchStatus();

		/*!
		*	\brief	�豸�Ƿ�ģ��
		*
		*	\retval	�Ƿ���ģ��״̬
		*/
		void SetIODeviceSim(CIODeviceSimBase* pDeviceSim);
		
		/*!
		*	\brief	�豸�Ƿ�ģ��
		*
		*	\retval	�Ƿ���ģ��״̬
		*/
		CIODeviceSimBase* GetIODeviceSim();

		//!��ȡIOUnit����Ҫ���������ݵ�Variable
		int GetNeedBackUpVar( std::vector<CKiwiVariable*>& vecHotVariable );
		/*!
		*	\brief	�����豸ģ��
		*
		*	\param	simMode	ģ�����ͣ����ҡ�����������ȣ������SimulateType
		*/
		void StartSimulate(Byte simMode);
		/*!
		*	\brief	ֹͣ�豸ģ��
		*/
		void StopSimulate();

		/*!
		*	\brief	�豸�Ƿ񱻽�ֹ
		*
		*	\retval	����ֵ	�Ƿ�����
		*/
		bool IsDisable();

		/*!
		*	\brief	�豸�Ƿ�ǿ��
		*
		*	\retval	����ֵ	�Ƿ�ǿ��
		*/
		bool IsForce();

		/*!
		*	\brief	�豸����״̬
		*
		*	\retval	����ֵ	����״̬��1Ϊ����0Ϊ��
		*/
		Byte PrmOrScnd();

		/*!
		*	\brief	�豸�ĵ�ǰ����״̬��1Ϊ����0Ϊ��
		*
		*	\param	byState	�豸������״̬
		*
		*	\retval	0��ʾ�ɹ���-1��ʾ��������
		*/
		int SetRealPrmScnd(Byte byState);

		/*!
		*	\brief	�����豸����
		*	\param	bChangeW �Ƿ�ı���·Ȩ��
		*/
		void SetOnline(bool bChangeW = true);

		/*!
		*	\brief	�����豸����
		*	\param	bChangeW �Ƿ�ı���·Ȩ��
		*/
		void SetOffline(bool bChangeW = true);

		/*!
		*	\brief	��ֹ�豸
		*/
		void DisableUnit();

		/*!
		*	\brief	����豸��ֹ
		*/
		void EnableUnit();

		/*!
		*	\brief	ǿ���豸������״̬
		*
		*	\param	PrmScnd	ǿ������״̬��1Ϊǿ������0Ϊǿ��
		*/
		void ForceUnit(Byte PrmScnd);

		/*!
		*	\brief	���ǿ��
		*/
		void DeforceUnit();

		/*!
		*	\brief	���ó�����
		*
		*	\param	bExtraRange	�Ƿ�����
		*/
		void SetExtraRange(bool bExtraRange);

		/*!
		*	\brief	�Ƿ�����
		*
		*	\retval	�Ƿ�����
		*/
		bool GetExtraRange();

		/*!
		*	\brief	��ȡ�豸����ͨ����ָ��
		*
		*	\retval	�豸������ͨ��ָ��
		*/
		CIOPort* GetIOPort();

		/*!
		*	\brief	ͨ���ڵ�Ż�ȡ���ݱ�������ָ��
		*
		*	\param	strNodeId	���ݱ����ڵ�Id
		*
		*	\retval	����ֵ	���ݱ���
		*/
		CKiwiVariable* GetDataVariableByNodeId(long strNodeId);

		/*!
		*	\brief	��ȡ�豸�����ݱ�������
		*
		*	\retval	���ݱ�������
		*/
		unsigned int GetVariableCount();

		/*!
		*	\brief	ͨ�����ݱ�����map���е��������ʱ���
		*
		*	\param	iIndex	���ݱ���������
		*
		*	\retval	 ���ݱ���ָ��
		*/
		CKiwiVariable* GetVariableByIndex(unsigned int iIndex);

			//!��ȡ�������豸ͨѶ״̬��ͨ����ַ
		std::string GetLocalDevCommType();

		//!��ȡ�Ի����豸ͨѶ״̬��ͨ����ַ
		std::string GetPeerDevCommType();

		//!ͨ����ǩ��ַ�ж��ǲ���DevComm���
		bool IsDevCommFieldPoint(const std::string &offSet);

		//!����ͨ��״̬��
		void SetPeerDevComm(bool isOnLine);

		//!��ȡ�豸��ַ(string)
		std::string GetStrDevAddr();

		/*!
		*	\brief ������·�µ����б�ǩ������״̬
		*
		*	\param	isOnLine	�Ƿ�����
		*
		*/
		void SetTagOnLineState(bool isOnLine, long dateTime);

	protected:
		//��������
		int sendCmd(
			//const ServiceContext& serviceContext,
			Byte& byCmdVal,
			std::string& bysCmdParam,
			int& result);

	private:
		/*!
		*	\brief	��ʼ���豸���ӽڵ�
		*
		*	\param	bCreateProp	�Ƿ���Ҫ��������ʼ������
		*
		*/
		void initialize(bool bCreateProp = true);

		/*!
		*	\brief ������·�µ����б�ǩ�Ľ�ֹ״̬
		*
		*	\param	bDisable	�Ƿ��ֹ
		*
		*/
		void SetTagDisableState(bool bDisable);

	protected:
		Byte	m_bSimulate;
		Byte	m_bOnline;
		int		m_iDeviceAddr;
		///����ͨ��ָ��
		CIOPort* m_pIOPort;
		//���ݱ����б�����ΪIO������PV���������ڲ�����
		std::map<long, CKiwiVariable*> m_mapDataVariables;

		std::string szUnitName;

	public:
		std::map<std::string, FieldPointType*> m_mapStrAddTag;

	private:
		//!�豸��������·����ת�����
		IOPortSwitchStatus m_SwitchStatus;
		//!ģʽ�豸����ָ��
		CIODeviceSimBase* m_pDeviceSim;
	};
}

#endif

