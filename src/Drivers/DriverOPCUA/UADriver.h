/*!
 *	\file	UADriver.h
 *
 *	\brief	UAʵ�ֽӿڶ���
 *
 *	\author LIFAN
 *
 *	\date	2014��5��19��	17:44
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
	 *	\brief	UADriver������
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class OPCUADRIVER_API CUADriver : public CIODriver, public UaSessionCallback
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CUADriver();

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CUADriver();

		/*!
		 *	\brief	��ͨ������Session����
		 *
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		virtual OpcUa_Int32 OpenChannel();

		/*!
		 *	\brief	�ر�ͨ��ɾ��Session����
		 *
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		virtual OpcUa_Int32 CloseChannel();

		/*!
		 *	\brief	�������ĺʹ������ļ�����
		 *	����Driver�µ������豸�������ĺͶ��ļ�����
		 *
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		virtual OpcUa_Int32 StartWork( void );

		/*!
		 *	\brief	ɾ�����ĺͶ��ļ�����
		 *
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		virtual OpcUa_Int32 StopWork( void );

		/*!
		 *	\brief	�õ��豸�������
		 *
		 *	\param	��
		 *
		 *	\retval	std::string �豸�������
		 */
		virtual std::string GetDeviceClassName();

		/*!
		 *	\brief	����Session״̬�ı仯
		 */
		virtual void connectionStatusChanged( OpcUa_UInt32 clientConnectionId, UaClient::ServerStatus serverStatus );

		/*!
		 *	\brief	�õ�����������
		 */
		std::string GetClassName();

		/*!
		 *	\brief	�õ�Session������
		 */
		CSiteProxy* GetSession();

		/*!
		 *	\brief	ͨ��UADriverʵ�ֱ�ǩ��ǿ�ƺͽ�ֹ
		 *
		 *	\param	tIORunMgrCmd	��ǩ��������
		 *
		 *	\retval	OpcUa_Int32		0Ϊ�ɹ�������ʧ��
		 */
		virtual OpcUa_Int32 WriteMgrForTag( tIORunMgrCmd& tRunMgr );

		/*!
		 *	\brief	ͨ��UADriverʵ�ֱ�ǩң��
		 *
		 *	\param	tIORunMgrCmd	��ǩң������
		 *
		 *	\retval	OpcUa_Int32		0Ϊ�ɹ�������ʧ��
		 */
		virtual OpcUa_Int32 WriteDCB( tIORemoteCtrl& tRemote );

		/*!
		 *	\brief	��������Դֻ��RTDB��ɫʱ��ǩǿ�ơ���ֹ��������
		 *
		 *	\param	tRunMgr			���й�������
		 *	\param	callRequest		������Ϣ
		 *
		 *	\retval	OpcUa_Int32		0Ϊ�ɹ�������ʧ��
		 */
		virtual OpcUa_Int32 ProcessRunMgr(tIORunMgrCmd& tRunMgr, CallIn& callRequest, UaVariant& varCmdParam);

		/*!
		 *	\brief	��ȡվ��Ĵ���ָ��
		 *
		 *	\param	��
		 *
		 *	\retval	CSiteProxy*		վ�����ָ��
		 */
		virtual CSiteProxy* GetSiteProxy();

		virtual void ReadOneTimeData();

		virtual void setMonitoringMode(OpcUa_MonitoringMode mode);
	private:
		/*!
		*	\brief	��UADriver��Ӧ��Session״̬�����仯ʱ����Driver������Device
		*			�������ݵ�Ԫ����״̬�û�
		 *
		 *	\param	byQuality		״̬��ʶ
		 *
		 *	\retval	��
		 */
		void SetDBUnitStatus(OpcUa_Byte byQuality);

		/*!
		 *	\brief	��UADriver��Ӧ��Session״̬�����仯ʱ����Driver������Device
		 *			�����豸��������״̬
		 *
		 *	\param	byQuality		״̬��ʶ
		 *
		 *	\retval	��
		 */
		void SetDeviceStatue( OpcUa_Byte statue );

	public:
		//!Driver������
		static std::string s_ClassName;

	private:
		//!վ��������ָ��
		CSiteProxy* m_pSiteProxy;

		//!Session�Ƿ����ɹ�
		OpcUa_Boolean m_bIsSiteProxyOk;

		//!��FEP֮������״̬�Ƿ�����
		OpcUa_Boolean m_bIsConnectedOk;
	};
}

#endif



