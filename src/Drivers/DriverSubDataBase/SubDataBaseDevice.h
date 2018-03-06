#ifndef _SUBDATABASEDEVICE_H_
#define _SUBDATABASEDEVICE_H_

#include "SubDataBaseComm.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"
#include "ServerProxy/sitesubscriptioncallback.h"
#include "ServerProxy/sitesubscription.h"
#include "ServerProxy/siteproxy.h"

namespace MACS_SCADA_SUD
{
	class SUBDATABASEDRIVER_API CSubDataBaseDevice : public CIODeviceBase, public UaSubscriptionCallback
	{

	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CSubDataBaseDevice();

		~CSubDataBaseDevice();

		std::string GetClassName();

		virtual OpcUa_Int32 Init();

		/*!
		 *	\brief	��д�����ʼ����ǩ��Ϣ
		 *	Driver�²����б�ǩ��ʼ��ֱ�ӷ���0
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


		//!�����¼�
		virtual void newEvents( OpcUa_UInt32 clientSubscriptionHandle, UaEventFieldLists& eventFieldList );

		//����drive����־����д��־
		OpcUa_Int32 WriteLogAnyWay( const OpcUa_CharA* pchLog, bool bTime=true );
		virtual void Read();
		virtual void ClearResource();
	private:
		//!��ȡ���ļ�����ɼ�����
		OpcUa_Double GetSamplingInterVal( UaVariable* pUaVariable );
		
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
		std::vector<OpcUa_UInt32> m_DelMItemIdVec;

		//!���ļ�������MAP��
		T_MAPMItems m_mapMItems;

		//FEP��վʱ��Ҫ���FEP������DbUnitId�������ҵ����õ�ʵʱ��վ������//
		std::map<OpcUa_UInt16,OpcUa_UInt32> m_DbUnitIdMap;

	};
}

#endif