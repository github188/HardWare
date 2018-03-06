/*!
*	\file	FepIEC103Prtcl.h
*
*	\brief	103��վ����Э����ͷ�ļ�
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Prtcl.h 2     11-02-24 11:25a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-02-24 11:25a $
*	$Revision: 2 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef _FEP_IEC103Prtcl_20110110_H_
#define _FEP_IEC103Prtcl_20110110_H_

#include "RTDB/Server/DBDriverFrame/IOPrtclParser.h"
#include "PublicFileH.h"

typedef struct DAILYTIME {
	OpcUa_Int32 lYear;					//! ��
	OpcUa_Int16 sMonth;				//! ��
	OpcUa_Int16 sDay;					//! ��
	OpcUa_Int16 sHour;				//! ʱ
	OpcUa_Int16 sMinute;				//! ��
	OpcUa_Int16 sSecond;				//! ��
	OpcUa_UInt16 sMSecond;	//! ����

	//!	ʱ����Ϊ0���Ǹ�������ʱ�䣬��ʱ��Ϊ������ʱ��Ϊ�������籱��ʱ��Ϊ-8��
	OpcUa_Int16 sTimeZone;

	//! ����ʱ��ȱʡֵ
	DAILYTIME()
	{
		lYear = 1970;
		sMonth = 1;
		sDay = 1;
		sHour = 0;
		sMinute = 0;
		sSecond = 0;
		sMSecond = 0;
		sTimeZone = -8;
	};
}DAILYTIME;


namespace MACS_SCADA_SUD
{
	class CFepIEC103Device;
	class CIOTimer;

	class FEPIEC103_API CFepIEC103Prtcl:
		public CIOPrtclParser
	{
		friend void OnTimeOut( void*pParent );			
		friend void CheckTimePeriodProc( void* pParent );
		friend void GIPeriodProc( void* pParent );
	public:
		//���졢����
		CFepIEC103Prtcl();
		~CFepIEC103Prtcl();

		static std::string s_ClassName;
		std::string GetClassName()
		{
			return s_ClassName;
		}

		virtual OpcUa_Int32 RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nLen );


		OpcUa_Int32 GetTagAddr( OpcUa_Byte Group, OpcUa_Byte Entry )
		{
			return Group*1000+Entry;
		}

		//����Ƿ�Ϊ��վ���͵ı���
		bool IsMasterMsg( OpcUa_Byte* pCtrl )
		{
			return ((CONTROL_FIELD*)pCtrl)->Prm == 1;
		}

		//�����豸��ַ��ȡ�豸������
		bool GetDeviceNoByAddr( OpcUa_Int32 Addr, OpcUa_Int32& DevNo );

		//�����豸���ٻ�
		void ActiveWholeCall( CFepIEC103Device* pDevice );

		/*!
		*	\brief	������֡������
		*	���ӿ����������ڷ�������ǰ����
		*	ȱʡʵ�����ж�ͨ���Ƿ񱻽�ֹ�ˣ��������ֹ�򷵻�-2��
		*
		*	\param	pbyData: Ҫ���͵�����
		*	\param	nDataLen: Ҫ���͵����ݵĳ���
		*
		*	\retval��0Ϊ�ɹ�������ʧ��
		*
		*	\note	
		*/
		virtual OpcUa_Int32 BuildRequest( OpcUa_Byte* pbyData, OpcUa_Int32&nDataLen );

		/*!
		*	\brief	������֡
		*	���ӿ����������ڷ�������ǰ����
		*	ȱʡʵ�����ж�ͨ���Ƿ񱻽�ֹ�ˣ��������ֹ�򷵻�-2��
		*
		*	\param	pbyData: Ҫ���͵�����
		*	\param	nDataLen: Ҫ���͵����ݵĳ���
		*
		*	\retval��0Ϊ�ɹ�������ʧ��
		*
		*	\note	
		*/
		OpcUa_Int32 BuildRequestEx( OpcUa_Byte* pbyData, OpcUa_Int32&nDataLen );

		//! ��ָ���豸��ͨ���������֡
		/*!
		*	\brief	��ָ���豸��ͨ���������֡
		*
		*	\param	pDevice:Ҫ�������豸ָ��
		*	\param	pbyData: ��õ�ͨ���������֡����ָ��
		*	\param	nReturnLen:����֡���ܳ���
		*
		*	\retval	����ֵ	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		*
		*	\note		ע�������ѡ��
		*/
		void MakeChannelDiagFrame( CIODeviceBase* pDevice, OpcUa_Byte* pbyData, OpcUa_Int32& nReturnLen );

		/*!
		*	\brief	���������֡
		*	
		*	\param	����	OpcUa_Byte* pBuf: ���ص����ݰ�
		*	\param	����	OpcUa_Int32& iReturnLen: �������ݰ��ĳ���	 
		*	\param	����	CFepIEC103Device* pDevice: �����豸	 
		*	\param	����	bool IsLocal: �Ƿ�Ϊ��������	 
		*
		*	\retval	����ֵ	��
		*
		*	\note		
		*/
		void BuildFrame( OpcUa_Byte* pBuf, OpcUa_Int32& iReturnLen, CFepIEC103Device* pDevice, bool IsLocal = true );

		/*!
		*	\brief	���·�����֡	
		*
		*	\param	����	OpcUa_Byte* pFrame: ֡������	 
		*
		*	\retval	����ֵ	OpcUa_Int32 : ֡����
		*
		*	\note		ע�������ѡ��
		*/
		OpcUa_Int32 BuildCtlRequest( OpcUa_Byte* sndBuf ,OpcUa_Int32& iDataLen);
		
		//׼���˳�
		void Destroy();

		//��ȡ�����ļ�
		void InitConfigPar( OpcUa_CharA* strAppName );
		
		OpcUa_CharA* MakeLog(OpcUa_Byte* pBuf, OpcUa_Int32 iLen, bool bSend=true);
		OpcUa_Byte Get8SumCode( OpcUa_Byte * Buf, OpcUa_Int32 iByteLen );
		

		CFepIEC103Device* GetNextRequestDev( void );
		OpcUa_Int32 OnConnSuccess(std::string pPeerIP );

								/*!
		*	\brief	��ʱ��ת���ɺ�����
		*
		*	\param	strTime	Ҫת����ʱ��	
		*
		*	\retval	OpcUa_Int64	��������Ϊ-1���ʾת��ʧ��
		*/
		OpcUa_Int64 CaculateMSNum( DAILYTIME strTime );

		//����Ƿ���·���ߣ����Ƿ������豸�����ߡ�
		bool IsLinkOffline();

		//����Ƿ���·�����ߣ���û��һ���豸���ߣ������豸�����߻��ߴ��ڳ�ʼ̬����
		bool IsLinkNotOnline();

		//����Ƿ���·���ɣ����Ƿ������豸�����ɻ����ߡ�
		bool IsLinkSuspicious();

		/*!
		*	\brief	�豸�Ƿ���Ҫ¼������
		*/
		bool IsEnableFaultWave()
		{
			return m_bEnableFaultWave;
		}

	protected:
		/*!
		*	\brief	 �������������ݰ�
		*	
		*	\param	����	OpcUa_Byte* pPacketBuffer: �����������ݰ�
		*	\param	����	SHORT* pnPacketLength: ���ݰ�����	
		*
		*	\retval	����ֵ	bool : �ɹ�true������false
		*
		*	\note		ע�������ѡ��
		*/
		bool ExtractPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32* pnPacketLength);

		//������ȡ�����Ľ�������֡����д����������Ҫʱ���ͱ�λ
		void ProcessRcvdPacket(	OpcUa_Byte* pPacketBuffer, OpcUa_Int32 nPacketLength, bool bFromLocal);

		//����㲥Уʱʱ��
		OpcUa_Int32 CaculateBroadPeriod( void );
	public:	
		CIOTimer*			m_pTimeOutTimer;								//��ʱ��ʱ��
		CIOTimer*			m_pPeriodTimer;									//���ڶ�ʱ��
		//CIOTimer*			m_pFaultLogTimer;								//����¼����ȡʱǿ�Ƽ�¼��־��ͨ�ű��ĵ��������ڡ�
		OpcUa_Int32	m_msFaultLogLifeCycle;							//����¼����־�����ı��������ʱ�䣬���롣0��ʾ��������־�����ġ�
		

		//IOScheduleSendPacket	m_pSendPeerPacket;							//�����ͺ���ָ��	
		bool					m_bIsAskForSendPacket;						//!�����������������		
	
		
		//bool				m_bSendPacketIsFromPeer;						//�����Ͱ��Ǳ�����Ļ���������		
		OpcUa_Int32				m_iDevIndex;									//�豸�߼���Ԫ��: 0 ~ Channel[0]->nMaxUnit-1	
		OpcUa_Int32				m_iDevLastIndex;

	
		//���弸�����ò�����ͨ����ȡ�����ļ�MacsDriver.ini���г�ʼ��
		OpcUa_Int32 m_nRcvTimeOut;							//���ճ�ʱ
		OpcUa_Int32 m_nCheckTimePeriod;						//��ʱ����, ��
		OpcUa_Int32 m_nWholeCallPeriodASDU7;				//ASDU7���ٻ�����, ��. 0��ʾ�����и������ٻ�
		OpcUa_Int32 m_nWholeCallPeriodASDU21;				//ASDU21���ٻ�����, ��. 0��ʾ�����и������ٻ�
		bool m_bEnableFaultWave;					//�Ƿ���Ҫ����¼������
		OpcUa_Int32 m_nMaxOfflineCount;						//���߼�����
		OpcUa_Int32 m_nPeerTimeOut;							//�������ʱ
		OpcUa_Int32 m_nDORelayTime;							//ң��ѡ���ӳ�ʱ��
		OpcUa_Int32 m_nSendDelay;							//���ͼ��ӳ٣�����
		//	OpcUa_Int32 m_iBroadPeriod;						//�㲥Уʱ����
		bool m_bBroadCheckTime;						//�Ƿ�ʹ�ù㲥��ʱ��1-�㲥��0-��Ե��ʱ��Ĭ��1��


		bool m_bIsTimeForBroadCheckTime;			//�㲥��ʱ���ڵ��ˡ�
		bool m_bIsBroadingCheckTime;				//���ڽ��й㲥Уʱ

		//��ʱ������
		OpcUa_Int32 m_nTimeOutCounter;
		//�ϴη��ʵ��豸��ַ
		OpcUa_Byte m_byLastDevAddr;	
		OpcUa_Byte m_byLastPeerDevAddr;	//�ӽ���֡����������վ����֡��������ͣ��ж�Ӧ���豸��ַ

		tIOCtrlRemoteCmd m_tCtrlRemoteCmd;	//! ��ǰ�����ң��ң������
		
		OpcUa_Byte m_byPrtclType;
		OpcUa_Int32 m_iConnTimeOut;
		OpcUa_Int32 m_iMaxReConn;

		OpcUa_UInt32 m_nCounter;					//! ���ڴ����¼�������. 
		OpcUa_Int32 m_nMaxLinkOffline;		//��·�����������������ڴ�ʱ����û���յ��κ��豸���ģ���϶�����·���ߡ�

		OpcUa_Int32	  m_nMaxRetryPeriod;	//���ڲ����豸����ʱ�������豸�������ڷ�������֡�Ĵ���

	private:	
		OpcUa_Int32	 m_iCurDevIndex;									//��ǰ���ڹ������豸������
		bool m_bRTUChannelTest;				//! ��ǰ����֡�Ƿ�Ϊͨ�����֡
		
	};


}
#endif	