/*!
 *	\file	UADevice.h
 *
 *	\brief	UA Device��
 *
 *	�������Ĳ���������Ϣ
 *
 *	\author LIFAN
 *
 *	\date	2014��5��20��	14:04
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _UADEVICE_H_
#define _UADEVICE_H_

#include "UADriverCommdef.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"
#include "ServerProxy/sitesubscriptioncallback.h"
#include "ServerProxy/sitesubscription.h"
#include "ServerProxy/siteproxy.h"

namespace MACS_SCADA_SUD
{
	//class CSiteSubscription;

	typedef std::map<OpcUa_UInt32, UaVariable*> T_MAPMItems;
	/*!
	 *	\class	CUADevice
	 *
	 *	\brief	UADevice��
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class OPCUADRIVER_API CUADevice : public CIODeviceBase, public UaSubscriptionCallback
	{

	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CUADevice();

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CUADevice();

		/*!
		 *	\brief	��ʼ��UADevice�µĶ�����Ϣ
		 *
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual OpcUa_Int32 Init();
		/*!
		 *	\brief	��д�����ʼ����ǩ��Ϣ
		 *	UADriver�²����б�ǩ��ʼ��ֱ�ӷ���0
		 *
		 *	\param	pIOUnit IOUnit�ڵ�ָ��
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual OpcUa_Int32 InitVariableInfo( CIOUnit* pIOUnit );

		/*!
		 *	\brief	����Subscription״̬
		 *
		 *	\param	clientSubscriptionHandle	
		 *	\param	status
		 *
		 *	\retval	��
		 */
		virtual void subscriptionStatusChanged( OpcUa_UInt32 clientSubscriptionHandle, const UaStatus& status );

		/*!
		 *	\brief	��������ݱ仯
		 *
		 *	\param	clientSubscriptionHandle	
		 *	\param	dataNotifications
		 *	\param	diagnosticInfos
		 *
		 *	\retval	��
		 */
		virtual void dataChange( OpcUa_UInt32 clientSubscriptionHandle, const UaDataNotifications& dataNotifications, const UaDiagnosticInfos& diagnosticInfos );

		//!��ʼ����
		UaStatus subscribe(CSiteProxy* pSiteProxy);

		//!��������
		UaStatus createSubscription(CSiteProxy* pSiteProxy);

		//!�������ļ�����
		UaStatus createMonitoredItems();

		//!ֹͣ����
		UaStatus unsubscribe();

		//!�õ��豸������
		std::string GetClassName();

		//!�����¼�
		virtual void newEvents( OpcUa_UInt32 clientSubscriptionHandle, UaEventFieldLists& eventFieldList );

		//!�����ٻ�ȡһ������
		void Read();

		//!���ü���ģʽ
		void setMonitoringMode(OpcUa_MonitoringMode mode);

		//!����Դ����ʱ����UADriver�б�ǩΪ����״̬
		void UpdateTagOffLine();

	private:
		//!��ȡ���ļ�����ɼ�����
		OpcUa_Double GetSamplingInterVal( UaVariable* pUaVariable );

		//!������ٷֱ�����
		void CreateDataChangeFilter( OpcUa_ExtensionObject* pFilter, UaVariable* pVariable );

		OpcUa_Float GetVarFloatVal( const UaVariant& varVal );

	public:
		//!���Ķ���
		CSiteSubscription* m_pSiteSubscription;

		//!Session����
		CSiteProxy* m_pSiteProxy;

		//!����
		static std::string s_ClassName;

		//!��������������б�
		UaMonitoredItemCreateRequests m_Requests;

		//!ɾ��������ID����
		//��m_MonitoredItemIds�滻m_DelMItemIdVec����Ϊ�ڲ���������ʱʵ�ʲ���������UaUInt32Array������vector<OpcUa_UInt32>
		//ÿ���������л���Ҫ�޸ļ���״̬��ʹ��UaUInt32Array���͵�m_MonitoredItemIds����ת�������Ч�� on 20160321//
		//std::vector<OpcUa_UInt32> m_DelMItemIdVec;
		UaUInt32Array m_MonitoredItemIds;

		//!���ļ�������MAP��
		T_MAPMItems m_mapMItems;
	};
}

#endif