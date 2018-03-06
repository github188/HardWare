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
		 *	\brief	缺省构造函数
		 */
		CSubDataBaseDevice();

		~CSubDataBaseDevice();

		std::string GetClassName();

		virtual OpcUa_Int32 Init();

		/*!
		 *	\brief	重写父类初始化标签信息
		 *	Driver下不进行标签初始化直接返回0
		 *
		 *	\param	pIOUnit IOUnit节点指针
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual OpcUa_Int32 InitVariableInfo( CIOUnit* pIOUnit );

		/*!
		 *	\brief	监视Subscription状态
		 *
		 *	\param	clientSubscriptionHandle	
		 *	\param	status
		 *
		 *	\retval	无
		 */
		virtual void subscriptionStatusChanged( OpcUa_UInt32 clientSubscriptionHandle, const UaStatus& status );

		/*!
		 *	\brief	服务端数据变化
		 *
		 *	\param	clientSubscriptionHandle	
		 *	\param	dataNotifications
		 *	\param	diagnosticInfos
		 *
		 *	\retval	无
		 */
		virtual void dataChange( OpcUa_UInt32 clientSubscriptionHandle, const UaDataNotifications& dataNotifications, const UaDiagnosticInfos& diagnosticInfos );

		//!开始订阅
		UaStatus subscribe(CSiteProxy* pSiteProxy);

		//!创建订阅
		UaStatus createSubscription(CSiteProxy* pSiteProxy);

		//!创建订阅监视项
		UaStatus createMonitoredItems();

		//!停止订阅
		UaStatus unsubscribe();


		//!触发事件
		virtual void newEvents( OpcUa_UInt32 clientSubscriptionHandle, UaEventFieldLists& eventFieldList );

		//借用drive的日志函数写日志
		OpcUa_Int32 WriteLogAnyWay( const OpcUa_CharA* pchLog, bool bTime=true );
		virtual void Read();
		virtual void ClearResource();
	private:
		//!获取订阅监视项采集周期
		OpcUa_Double GetSamplingInterVal( UaVariable* pUaVariable );
		
	public:
		//!订阅对象
		CSiteSubscription* m_pSiteSubscription;

		//!Session对象
		CSiteProxy* m_pSiteProxy;

		//!类名
		static std::string s_ClassName;

		//!监视项请求对象列表
		UaMonitoredItemCreateRequests m_Requests;

		//!删除监视项ID数组
		std::vector<OpcUa_UInt32> m_DelMItemIdVec;

		//!订阅监视数据MAP表
		T_MAPMItems m_mapMItems;

		//FEP子站时需要存放FEP下所有DbUnitId，用于找到对用的实时库站点名称//
		std::map<OpcUa_UInt16,OpcUa_UInt32> m_DbUnitIdMap;

	};
}

#endif