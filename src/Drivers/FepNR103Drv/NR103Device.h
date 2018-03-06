/*!
*	\file	NR103Driver.h
*
*	\brief	NR103������
*
*	$Date: 15-03-30$
*
*/
#ifndef _NR103DEVICE_H_
#define _NR103DEVICE_H_

#include "NR103CommDef.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"

namespace MACS_SCADA_SUD
{
	class CNR103Driver;

	class FEPNR103Drv_API CNR103Device : public CIODeviceBase
	{
	public:
		CNR103Device();

		~CNR103Device();

		std::string GetClassName();

		//!�����豸�ĳ�ʼ��
		virtual OpcUa_Int32 Init();

		//!��ʼ���豸������Ϣ
		OpcUa_Int32 InitDevConfig();

		//!����ASDU21��������Ϣ
		void ProcessGroupNo( std::string &strFilePath );
		
		//!�����豸Уʱ��ʶ
		void SetCheckTFlag( OpcUa_Boolean bIsNeedCT );
		
		//!�����豸������ʶ
		void SetSendHBTag( OpcUa_Boolean bSendHB );

		//!���÷���ASDU21�ܲ�ѯ�ı�ʶ
		void SetASDU21WQ(  OpcUa_Boolean bFlag  );

		//!�����ܲ�ѯ��ʶ
		void SetASDU7( OpcUa_Boolean bFlag );

		//!��ʼ�����ݱ��
		void InitDataNum(OpcUa_Int32 nDataNum);

		//!�豸�Ƿ���ҪУʱ
		OpcUa_Boolean IsNeedCheckT();

		//!�豸�Ƿ���Ҫ��������
		OpcUa_Boolean IsNeedHeartB();

		//!�Ƿ���Ҫ����ASDU7���ٻ�
		OpcUa_Boolean IsNeedASUD7();

		//!�Ƿ���Ҫ����ASDU21���ٻ�
		OpcUa_Boolean IsNeedASUD21();

		//!��Уʱ֡
		OpcUa_Int32 BuildCTFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!��APCI
		OpcUa_Int32 BuildAPCIFrame( FrameAPCI* pAPCI, DWORD dwFrameLen );

		//!������֡
		OpcUa_Int32 BuildHBFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!��ASDU7���ٻ�֡
		OpcUa_Int32 BuildASDU7( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!��ASDU21���ٻ�֡
		OpcUa_Int32 BuildASDU21( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );
		
		/*!
		 *	\brief	�����豸��ASDU10ͨ�÷�������
		 *
		 *	\param	pASDUFrame		: ������ASDU������ָ��
		 *	\param	nDataLen		: ASDU���ݳ���
		 *
		 *	\retval	����ֵ	-1:ʧ�ܣ�0:�ɹ�
		 */
		OpcUa_Int32 ProcessASDU10Data( const OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nDataLen );

		//!����ASDU10��������Ӧ
		OpcUa_Byte ProASDU10NoDataSet( OpcUa_Byte byCOT, OpcUa_Int32 nTagAddr );

		//!�õ���ǩ��ַ
		OpcUa_Int32 GetTagAddr( OpcUa_Byte byGroup, OpcUa_Byte byEntry );
		
		//!����GID����
		OpcUa_Int32 ProcessGID( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver);
		
		//!����UI����GID
		OpcUa_Int32 ProcessDTUI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver);
		
		//!����Float����GID
		OpcUa_Int32 ProcessDTFloat( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!����double����GID
		OpcUa_Int32 ProcessDTDouble( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );

		//!����˫����Ϣ
		OpcUa_Int32 ProcessDTDPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!��������Ϣ
		OpcUa_Int32 ProcessDTSPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!��˲��Ͳ���˫����Ϣ
		OpcUa_Int32 ProcessDT11DPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!�����Ʒ����������ֵ
		OpcUa_Int32 ProcessDTAIWT( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!��ʱ�걨��
		OpcUa_Int32 ProcessDT18DPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!�����ʱ���ʱ�걨��
		OpcUa_Int32 ProcessDT19DPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );

		//!�����º��ֵ
		void CheckUpdateVal( OpcUa_Int32 nTagType, OpcUa_Int32 nTagAddr, CNR103Driver* pDriver );

		//!��7�ֽ�ʱ�걨��
		OpcUa_Int32 ProcessDT203( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!�����ʱ��7�ֽ�ʱ�걨��
		OpcUa_Int32 Processdt204( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );

		//!���������Ϣ
		void WriteProInfo( GDATAPointHead* pGDHead, OpcUa_Float fValue, CNR103Driver* pDriver, OpcUa_Boolean isAnyWay=OpcUa_False );
		
		//!����ASDU5����
		OpcUa_Int32 ProcessASDU5Data( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount );
		
		//!����ASDU6����
		OpcUa_Int32 ProcessASDU6Data( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount );
		
		//!����NR103Զ�������ASDU1��������
		OpcUa_Int32 ProcessASDU1FRest( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount );

		//!�����豸�ϴ��Ŷ�����
		OpcUa_Int32 ProcessASDU23Data( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount );

		//!��ң������
		OpcUa_Int32 BuildASDU10DOFrame( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress, OpcUa_Byte byType, OpcUa_Byte byData );
		
		//!�����ֵ������
		OpcUa_Int32 BuildReadSFrame( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress );
		
		//!���źŸ�������byASDU10
		OpcUa_Int32 BuildResetFrame( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress, OpcUa_Boolean bExe = true );
		
		//!����103Զ����������ASDU2O
		OpcUa_Int32 BuildResetASDU20( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress );
		
		//ͨ����ǰ���ڼ��������
		OpcUa_Int64 CaculateMSNum( DAILYTIME sTime );
		
		//DI�Ƿ�����λ��������־
		OpcUa_Boolean IsDIValueChange(OpcUa_Byte byType, OpcUa_Int32 nTagAdrr, OpcUa_Byte byNewVal, CNR103Driver* pDriver);
		
		//���SOE��ʱ����Ϣ
		void WriteSOETime( std::string strDevName, OpcUa_Int32 nTagAddr, DAILYTIME sTime, CNR103Driver* pDriver );

	public:
		static std::string s_ClassName;
	private:
		WORD m_wSFacAddr;					//!Դ��վ��ַ
		WORD m_wDFacAddr;					//!Ŀ�곧վ��ַ
		WORD m_wSDevAddr;					//!Դ�豸��ַ
		WORD m_wDDevAddr;					//!Ŀ���豸��ַ
		WORD m_wFRoutAddr;					//!�׼�·�ɵ�ַ
		WORD m_wLRoutAddr;					//!ĩ��·�ɵ�ַ
		WORD m_nDataNum;					//!���ݱ��
		WORD m_wDevType;					//!�豸����
		WORD m_wDNetState;					//!�豸����״̬
		OpcUa_Boolean m_bIsNeedCheckT;		//!�Ƿ���ҪУʱ
		OpcUa_Boolean m_bIsNeedSendHB;		//!�Ƿ���Ҫ��������
		OpcUa_Boolean m_bNeedASDU21GI;		//!�Ƿ���ҪASDU21���ٻ�
		OpcUa_Boolean m_bNeedASDU7;			//!�Ƿ���ҪASDU7���ٻ�
		OpcUa_Int32 m_nASDU21GCount;		//!��ҪASDU21��ֵ���ٻ���Ŀ
		vector<OpcUa_Int32> m_vec21GroupNo;	//!��ֵ�ٻ���
		OpcUa_Int32 m_bySCN;				//!ɨ�����к�
	};
}

#endif