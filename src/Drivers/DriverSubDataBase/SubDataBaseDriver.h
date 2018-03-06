#ifndef _SUBDATABASEDRIVER_H_
#define _SUBDATABASEDRIVER_H_

#include "RTDB/Server/DBDriverFrame/IODriver.h"
#include "RTDB/Server/FEPPlatformClass/IOPort.h"
#include "RTDB/Server/FEPPlatformClass/fep_identifiers.h"
#include "RTDB/Server/utilities/common_identifiers.h"
#include "ServerProxy/siteproxycallback.h"
#include "ServerProxy/siteproxy.h"
#include "ServerProxyMgr/proxymgr.h"
#include "SubDataBaseComm.h"
#include "SubDataBaseDevice.h"

namespace MACS_SCADA_SUD
{

	class SUBDATABASEDRIVER_API CSubDataBaseDriver : public CIODriver, public UaSessionCallback
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CSubDataBaseDriver();

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CSubDataBaseDriver();

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
		 *	\brief	�õ�����������
		 */
		std::string GetClassName();

		/*!
		 *	\brief	�������ĺʹ������ļ�����
		 *	����Driver�µ������豸�������ĺͶ��ļ�����
		 *
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		virtual OpcUa_Int32 StartWork(SessionConnectInfo* pSessionConnectInfo,	SessionSecurityInfo* pSessionSecurityInfo,UaSessionCallback* pCallback );

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
		virtual OpcUa_Int32 ProcessRunMgr(tIORunMgrCmd& tRunMgr, CallIn& callRequest);

		/*!
		 *	\brief	��ȡվ��Ĵ���ָ��
		 *
		 *	\param	��
		 *
		 *	\retval	CSiteProxy*		վ�����ָ��
		 */
		virtual CSiteProxy* GetSiteProxy();

		virtual void SetDeviceStatue( OpcUa_Byte statue );	

		virtual void ReadOneTimeData();
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
			

	public:
		//!Driver������
		static std::string s_ClassName;
		static std::string s_MatchDeviceClassName;

		std::string m_proxySiteUri;
		OpcUa_Int32 m_serverType;
	private:
		//!վ��������ָ��
		CSiteProxy* m_pSiteProxy;

		//!Session�Ƿ����ɹ�
		OpcUa_Boolean m_bIsConnected;
	};
}

#endif