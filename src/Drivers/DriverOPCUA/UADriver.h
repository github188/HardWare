/*!
 *	\file	UADriver.h
 *
 *	\brief	UA实现接口定义
 *
 *	\author LIFAN
 *
 *	\date	2014年5月19日	17:44
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _UADRIVER_H_
#define _UADRIVER_H_

#include "UADriverCommdef.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"
#include "RTDB/Server/FEPPlatformClass/IOPort.h"
#include "RTDB/Server/FEPPlatformClass/fep_identifiers.h"
#include "RTDB/Server/utilities/common_identifiers.h"
#include "UADevice.h"
#include "ServerProxy/siteproxycallback.h"
#include "ServerProxy/siteproxy.h"
#include "ServerProxyMgr/proxymgr.h"

namespace MACS_SCADA_SUD
{
	//class CSiteProxy;

	/*!
	 *	\class	CUADriver
	 *
	 *	\brief	UADriver驱动类
	 *
	 *	详细的类说明（可选）
	 */
	class OPCUADRIVER_API CUADriver : public CIODriver, public UaSessionCallback
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CUADriver();

		/*!
		 *	\brief	缺省析构函数
		 */
		~CUADriver();

		/*!
		 *	\brief	打开通道建立Session连接
		 *
		 *	\param	无
		 *
		 *	\retval	0为成功,其他失败
		 */
		virtual OpcUa_Int32 OpenChannel();

		/*!
		 *	\brief	关闭通道删除Session连接
		 *
		 *	\param	无
		 *
		 *	\retval	0为成功,其他失败
		 */
		virtual OpcUa_Int32 CloseChannel();

		/*!
		 *	\brief	创建订阅和创建订阅监视项
		 *	遍历Driver下的所有设备创建订阅和订阅监视项
		 *
		 *	\param	无
		 *
		 *	\retval	0为成功,其他失败
		 */
		virtual OpcUa_Int32 StartWork( void );

		/*!
		 *	\brief	删除订阅和订阅监视项
		 *
		 *	\param	无
		 *
		 *	\retval	0为成功,其他失败
		 */
		virtual OpcUa_Int32 StopWork( void );

		/*!
		 *	\brief	得到设备类的名字
		 *
		 *	\param	无
		 *
		 *	\retval	std::string 设备类的类名
		 */
		virtual std::string GetDeviceClassName();

		/*!
		 *	\brief	处理Session状态的变化
		 */
		virtual void connectionStatusChanged( OpcUa_UInt32 clientConnectionId, UaClient::ServerStatus serverStatus );

		/*!
		 *	\brief	得到驱动类类名
		 */
		std::string GetClassName();

		/*!
		 *	\brief	得到Session的连接
		 */
		CSiteProxy* GetSession();

		/*!
		 *	\brief	通过UADriver实现标签的强制和禁止
		 *
		 *	\param	tIORunMgrCmd	标签管理命令
		 *
		 *	\retval	OpcUa_Int32		0为成功，其他失败
		 */
		virtual OpcUa_Int32 WriteMgrForTag( tIORunMgrCmd& tRunMgr );

		/*!
		 *	\brief	通过UADriver实现标签遥控
		 *
		 *	\param	tIORunMgrCmd	标签遥控命令
		 *
		 *	\retval	OpcUa_Int32		0为成功，其他失败
		 */
		virtual OpcUa_Int32 WriteDCB( tIORemoteCtrl& tRemote );

		/*!
		 *	\brief	处理数据源只有RTDB角色时标签强制、禁止命令请求
		 *
		 *	\param	tRunMgr			运行管理命令
		 *	\param	callRequest		请求信息
		 *
		 *	\retval	OpcUa_Int32		0为成功，其他失败
		 */
		virtual OpcUa_Int32 ProcessRunMgr(tIORunMgrCmd& tRunMgr, CallIn& callRequest, UaVariant& varCmdParam);

		/*!
		 *	\brief	获取站点的代理指针
		 *
		 *	\param	无
		 *
		 *	\retval	CSiteProxy*		站点代理指针
		 */
		virtual CSiteProxy* GetSiteProxy();

		virtual void ReadOneTimeData();

		virtual void setMonitoringMode(OpcUa_MonitoringMode mode);
	private:
		/*!
		*	\brief	当UADriver对应的Session状态发生变化时遍历Driver下所有Device
		*			设置数据单元数据状态好坏
		 *
		 *	\param	byQuality		状态标识
		 *
		 *	\retval	无
		 */
		void SetDBUnitStatus(OpcUa_Byte byQuality);

		/*!
		 *	\brief	当UADriver对应的Session状态发生变化时遍历Driver下所有Device
		 *			设置设备在线离线状态
		 *
		 *	\param	byQuality		状态标识
		 *
		 *	\retval	无
		 */
		void SetDeviceStatue( OpcUa_Byte statue );

	public:
		//!Driver类类名
		static std::string s_ClassName;

	private:
		//!站点代理对象指针
		CSiteProxy* m_pSiteProxy;

		//!Session是否建立成功
		OpcUa_Boolean m_bIsSiteProxyOk;

		//!与FEP之间连接状态是否正常
		OpcUa_Boolean m_bIsConnectedOk;
	};
}

#endif



