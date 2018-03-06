/*!
 *	\file	UADevice.h
 *
 *	\brief	UA Device类
 *
 *	创建订阅并处理订阅信息
 *
 *	\author LIFAN
 *
 *	\date	2014年5月20日	14:04
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
	 *	\brief	UADevice类
	 *
	 *	详细的类说明（可选）
	 */
	class OPCUADRIVER_API CUADevice : public CIODeviceBase, public UaSubscriptionCallback
	{

	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CUADevice();

		/*!
		 *	\brief	缺省析构函数
		 */
		~CUADevice();

		/*!
		 *	\brief	初始化UADevice下的订阅信息
		 *
		 *	\param	无
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual OpcUa_Int32 Init();
		/*!
		 *	\brief	重写父类初始化标签信息
		 *	UADriver下不进行标签初始化直接返回0
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

		//!得到设备类类名
		std::string GetClassName();

		//!触发事件
		virtual void newEvents( OpcUa_UInt32 clientSubscriptionHandle, UaEventFieldLists& eventFieldList );

		//!重新再获取一拍数据
		void Read();

		//!设置监视模式
		void setMonitoringMode(OpcUa_MonitoringMode mode);

		//!数据源离线时设置UADriver中标签为离线状态
		void UpdateTagOffLine();

	private:
		//!获取订阅监视项采集周期
		OpcUa_Double GetSamplingInterVal( UaVariable* pUaVariable );

		//!处理近百分比死区
		void CreateDataChangeFilter( OpcUa_ExtensionObject* pFilter, UaVariable* pVariable );

		OpcUa_Float GetVarFloatVal( const UaVariant& varVal );

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
		//用m_MonitoredItemIds替换m_DelMItemIdVec，因为在操作监视项时实际操作对象是UaUInt32Array而不是vector<OpcUa_UInt32>
		//每次在主从切换后都要修改监视状态，使用UaUInt32Array类型的m_MonitoredItemIds无需转化，提高效率 on 20160321//
		//std::vector<OpcUa_UInt32> m_DelMItemIdVec;
		UaUInt32Array m_MonitoredItemIds;

		//!订阅监视数据MAP表
		T_MAPMItems m_mapMItems;
	};
}

#endif