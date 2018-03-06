/*!
*	\file	FepIEC103Device.h
*
*	\brief	103��վ�����豸��ͷ�ļ�
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Device.h 3     11-02-24 11:25a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-02-24 11:25a $
*	$Revision: 3 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/


#ifndef _FEP_IEC103Device_H_20110110_
#define _FEP_IEC103Device_H_20110110_

#include "PublicFileH.h"
#include "IEC103.h"
#include "PRDFile103Dev.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"
#include <vector>
#include <map>
using namespace std;

#define MAX_DI_NUM							1024								//DI���������
#define MAX_DO_NUM							256									//DO���������
#define FAULT_WAVE_TIMEOUT	600000	//����¼�����ճ�ʱ,����.
#define MAX_SOE 10000 //!SOE�����Ŀ
#define UNIT_TYPE_INDEX_DIGITAL 0
#define UNIT_TYPE_INDEX_REAL    1
//#define UNIT_TYPE_INDEX_INT     2
//#define UNIT_TYPE_INDEX_STRING  3
#define FAULT_WAVE_CYCLE 15000

namespace MACS_SCADA_SUD
{	
	typedef std::map<OpcUa_Int32, OpcUa_Int32> SOEMAP;
	typedef SOEMAP::iterator SOEMAPiterator; 

	typedef std::map<OpcUa_Int32, OpcUa_Int32> PointMap;
	typedef PointMap::iterator PointMapiterator;

	typedef std::map<OpcUa_Int32, bool> StatusMap;
	typedef StatusMap::iterator StatusMapiterator; 

	typedef std::map<OpcUa_Int32, double> AIModulusMap;
	typedef AIModulusMap::iterator AIModulusMapiterator; 


	// **********************************************************************************************
	// �� д ��: ZhouJun        
	// ����ʱ��: 2006-10-26 
	// �� �� ��: tUnitTypeRes
	// ��    ��: �豸���������͵���ض���
	// �㷨˼��: 
	// **********************************************************************************************
	#pragma pack(push,1)
	typedef struct tUnitTypeRes
	{
		CIOCfgUnitType* pUnitTypeCfg;		//�豸���������͵�����
		OpcUa_Byte*           pUnitTypeAddr;		//�豸���������Ͷ�Ӧ���ݿ���λ��ָ��
		OpcUa_Int32				nOffset;			//�豸���������͵�ƫ��
	}tUnitTypeRes;

	enum enumASDU24TOO
	{
		T_FaultSelect,				//����ѡ��
		T_Data_Req,					//�Ŷ����ݵ�����
		T_Data_Trans_End,			//�Ŷ����ݵ���ֹ
		T_StatusChange_Req,			//�������־��״̬��λ����
		T_StatusChange_End,			//��ֹ����־��״̬��λ����
		T_Chanel_Req,				//ͨ�����������
		T_Chanel_End				//֪ͨ�������ֹ
	};

	// **********************************************************************************************
	// �� д ��: ZhouJun        
	// ����ʱ��: 2006-11-11 
	// �� �� ��: FaultDataSet
	// ��    ��: ����¼���Ŷ����е����ݼ��ṹ
	// �㷨˼��: 
	// **********************************************************************************************
	typedef struct struFaultDataSet
	{
		WORD				FAN;		//�������
		OpcUa_Byte				SOF;		//���ϵ�״̬
		TIMESTAMP_7BYTE		TimeSet;	//ʱ�伯
	}FaultDataSet;
	#pragma pack(pop)
	typedef std::vector<struFaultDataSet> FaultDataVec;

	class CIOTimer;


	// **********************************************************************************************
	// �� д ��: ZhouJun        
	// ����ʱ��: 2006-10-26 
	// �� �� ��: CFepIEC103Device
	// ��    ��: ��װ�豸��ǩֵ����SOE����Խ�����¼������Լ��豸��صĸ�״̬
	// �㷨˼��: 
	// **********************************************************************************************
	class CIOCfgUnit;
	class CIOCfgUnitType;
	class CIOTimer;
	class CPRDFile103Dev;
	class FEPIEC103_API CFepIEC103Device
		:public CIODeviceBase
	{
		friend void DOCTimer(void* pParent);
	public:
		//���졢����
		CFepIEC103Device( );
		~CFepIEC103Device();
	public:	
		static std::string s_ClassName;
		std::string GetClassName()
		{
			return s_ClassName;
		}
		/*!
		*	\brief	��ʼ��
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual OpcUa_Int32 Init();
		void GetTimeFromCP32Time( CP32Time2a Time, OpcUa_Byte& Second, WORD& MilliSecond );
		
		//������ʱ��ͬ��֡
		OpcUa_Int32 BuildCheckTimeFrame( OpcUa_Byte* buf, bool bBroadCast );

		//���ܲ�ѯ֡
		OpcUa_Int32 BuildGIFrame( OpcUa_Byte* pBuf );

		/*!
		*	\brief	�顰��λͨ�ŵ�Ԫ��֡
		*
		*	��ϸ�ĺ���˵������ѡ��
		*
		*	\param	����	OpcUa_Byte* pBuf: Ҫ�ӵ�����		
		*
		*	\retval	����ֵ	OpcUa_Int32 iLen: Ҫ�ӵ����ݵĳ���
		*
		*	\note
		*/
		OpcUa_Int32 BuildResetCUFrame( OpcUa_Byte* pBuf );

		//�顰��λ֡��������֡
		OpcUa_Int32 BuildResetFCBFrame( OpcUa_Byte* pBuf );

		//�顰����һ�����ݡ�֡
		OpcUa_Int32 BuildLevel1DataFrame( OpcUa_Byte* pBuf );

		//�顰����������ݡ�֡
		OpcUa_Int32 BuildLevel2DataFrame( OpcUa_Byte* pBuf );

		//�顰ң�ء�����֡
		OpcUa_Int32 BuildDOFrame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr, OpcUa_Byte bVal);

		//�顰ң�ء�����֡
		OpcUa_Int32 BuildDOBFrame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr, OpcUa_Byte bVal);

		//��ASDU140��ȡ֡
		OpcUa_Int32 BuildASDU140Frame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr);

		//��ASDU232����֡
		OpcUa_Int32 BuildASDU232Frame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr, OpcUa_Byte bVal);

		//��ASDU21���ٻ�֡
		OpcUa_Int32 BuildASDU21WholeCallFrame( OpcUa_Byte* pBuf );

		//��ASDU21ĳ���ٻ�֡
		OpcUa_Int32 BuildASDU21GroupFrame( OpcUa_Byte* pBuf, OpcUa_Int32 iTagAddr );

		//�顰ASDU24��֡
		OpcUa_Int32 BuildASDU24Frame ( OpcUa_Byte* pBuf, enumASDU24TOO eTOO );

		//�顰ASDU25 �϶�����Ͽɡ�
		OpcUa_Int32 BuildDisDataAckFrame( OpcUa_Byte* pBuf, OpcUa_Byte bAck );

		//����ϵͳʱ��
		//SYSTEMTIME OffsetSystemTime( OpcUa_Int32 msOffset );

		/*!
		*	\brief	FCBȡ��
		*	 
		*	\retval	����ֵ	��
		*
		*	\note		ע�������ѡ��
		*/
		void ReverseFCB( void );

		//�������ļ��ж�ȡ����ǩ�Ƿ���ҪSOE��
		void InitTagSOEMap(void);

		//�鿴��ǩ���Ƿ���ҪSOE
		bool TagNeedSOE (OpcUa_Byte byType, OpcUa_Int32 iTagAddr );
		
		bool TagNeedDevSOE (OpcUa_Byte byType, OpcUa_Int32 iTagAddr );

		//ͨ����ǩλ�ò顰�������͡��͡���Ϣ��š������
		OpcUa_Int32 GetPointAddr (OpcUa_Int32 iIndex)
		{
			PointMapiterator it = m_DIPointMap.find(iIndex);
			if (it != m_DIPointMap.end())
				return it->second;
			return -1;
		}

		//ͨ����ǩλ�ò顰�������͡��͡���Ϣ��š������
		OpcUa_Int32 GetDOPointAddr (OpcUa_Int32 iIndex)
		{
			PointMapiterator it = m_DOPointMap.find(iIndex);
			if (it != m_DOPointMap.end())
				return it->second;
			return -1;
		}

		//ͨ�����������͡��͡���Ϣ��š������ǩλ��
		OpcUa_Int32 GetTagIndex (OpcUa_Int32 iFunType, OpcUa_Int32 iInfoSeqNO)
		{
			OpcUa_Int32 iTagIndex = 1000*iFunType + iInfoSeqNO;
			PointMapiterator it = m_DIPointMap.begin();
			while ( it != m_DIPointMap.end())
			{
				if (it->second == iTagIndex)
					return it->first;
				it++;
			}
			return -1;
		}
	
		/*!
		*	\brief	���ôӻ����豸�ĳ�ʱ����
		*/
		void SetSlaveTimeOuts(OpcUa_Int32 nSlaveTimeOuts )
		{
			m_byStandyOffCounter = nSlaveTimeOuts;
		};

		//! �����豸������
		virtual void SetHot(OpcUa_Byte byHot);

		bool IsSuspicious()
		{
			return m_bIsSuspicious;
		}

		void SetSuspicious( bool bState )
		{
			m_bIsSuspicious = bState;
		}

	protected:
		//��ȡĳ�ֽ�ĳλ��ֵ
		OpcUa_Byte GetBitVal( OpcUa_Byte byIn, OpcUa_Int32 iBit );						

		//����ĳ�ֽ�ĳλ��ֵ
		void SetBitVal( OpcUa_Byte* byInOut, OpcUa_Int32 iBit, OpcUa_Int32 iVal );

	public:
		CPRDFile103Dev*			m_pPRDFile;             //���𽫹���¼����ر���ת��ΪComtrade��ʽ��¼���ļ�
		OpcUa_Byte					m_byTxTimes;			//���ʹ���
		OpcUa_Byte					m_byRxTimes;			//���մ���
		OpcUa_Int32						m_nWatchDogCounter;		//
		OpcUa_Int32						m_nWatchDog;			//
		OpcUa_Int32						m_nTagCount;			//�豸ʵ����̬��ǩ����
		tUnitTypeRes*			m_pTypeRes;				//�����������ݵı���λ��
		vector<OpcUa_Byte>			m_vExtraRangeFlag;		//�������¼���־

		WORD					m_wFAN;					//�������

		//������Ʒ���Ŀ�������Ϣ
		bool					m_bFCB;					//֡����λ
		bool					m_bFCV;					//֡������Чλ

		//������ӷ���Ŀ�������Ϣ
		bool					m_bACD;					//Ҫ�����λ
		bool					m_bDFC;					//����������λ

		OpcUa_Int32						m_nSendResetCUCounter;	//���͸�λ֡���Լ�����
		bool					m_bSendRestCU;			//���͸�λ֡��־λ
		bool					m_bReq1LevelData;		//����һ�����������־λ
		bool					m_bReq2LevelData;		//���Ͷ������������־λ

		bool					m_bNeedInit;			//��ʼ����־
		OpcUa_Int32						m_FaultTimeOut;			//����¼����ʱ,���롣
		OpcUa_Int32						m_nFautTimeOutCount;	//

		bool					m_bNeedCheckTime;		//Уʱ��־

		bool					m_bNeedASDU21GI;		//�ܲ�ѯ��־��ASDU21
		bool					m_bNeedASDU7GI;			//�ܲ�ѯ��־��ASDU7
		OpcUa_Int32						m_iGINo;				//�ܲ�ѯ����
		bool					m_IsSOE;				//��ʶ�ϴ���ASDU1�Ƿ�ΪSOE(���ܲ�ѯ�׶�)

		bool					m_bFaultSel;			//����ѡ��
		OpcUa_Byte					m_byFaultNum;			//���ϸ���
		FaultDataVec			m_FaultDataVec;			//����¼�Ŷ����е����ݼ�
		OpcUa_Byte					m_byRcvFaultNum;		//�ѳɹ����յ��Ĺ��ϸ���

		OpcUa_Byte					m_byNOC;				//ͨ����Ŀ
		WORD					m_NOE;					//һ��ͨ����ϢԪ�ص���Ŀ
		WORD					m_sINT;					//��ϢԪ�أ��Ŷ�ֵ������΢��Ϊ��λ�Ĳ������						

		bool					m_bReqDisData;			//���Ŷ��������󡱱�־
		bool					m_bReqStatusAlter;		//���������־��״̬��λ���䡱��־

		OpcUa_Byte					m_byNOT;				//����־��״̬��λ����Ŀ

		bool					m_bSendDisDataAck;		//���Ŷ������Ͽɡ���־
		OpcUa_Byte					m_byDisDataAckTOO;		//���Ŷ������Ͽɡ���������

		bool					m_bChannelReady;		//ͨ������׼��������־
		OpcUa_Byte					m_byACC;				//ͨ�����

		WORD					m_sNDV;					//ÿ��ASDU�й��Ŷ�ֵ����Ŀ
		WORD					m_sNFE;					//�����Ŷ�ֵ�����е�һ����ϢԪ�ص���ţ��������鱨��

		bool					m_bChannelEnd;			//ͨ���������

		OpcUa_Byte					m_byRII;

		SOEMAP					m_DISOEMap;
		SOEMAP					m_DIDevSOEMap;			//�豸���͵�SOE����ASDU41����Ҫ��������SOE��map��
		SOEMAP					m_DOSOEMap;		
		SOEMAP					m_DOWithSel;

		//!������
		PointMap				m_DIPointMap;
		PointMap				m_DOPointMap;
		
		AIModulusMap			m_AIModulusMap;			//AI��ϵ��map
		OpcUa_Byte					m_byAIFUN;				//ASDU9�Ĺ�������
		OpcUa_Byte					m_byAISEQ;				//ASDU9����Ϣ���

		StatusMap				m_statusMap;
		
		OpcUa_Byte m_DIs[MAX_DI_NUM];
		OpcUa_Byte m_DOAs[MAX_DO_NUM];

		bool m_bInDO;									//DO���������
		OpcUa_Int32 m_iInDOIndex;								//����DO��������еĵ��ַ
		OpcUa_Int32 m_iInDOMIndex;								//!�ϴδ����DOM����ĵ��ַ
		CIOTimer*				m_pDOCTimer;			//����ѡ���ʱ��	
		CIOTimer*				m_pFaultTimeOutTimer;	//����¼����ʱ��ʱ��

		OpcUa_Byte					m_byFaultFUN;			//���ϵĹ�������
		OpcUa_Int32						m_iAIMAX;				//һ��������AI������


		bool					m_bNew;					//�ձ����
		bool					m_bFirstCT;				//��һ�ε�Уʱ����

		//CRITICAL_SECTION		m_FaultWaveSection;		//����¼�������
		bool m_bIsRcvingFaultWave; //!�Ƿ����ڽ���¼������

		OpcUa_Byte					m_byStandyOffCounter;	//��վ���߼�����

		OpcUa_Int32		m_iASDU7ReadGroupPeriod;				//ASDU7  ASDU7���ٻ�����,��. 0��ʾ������ASDU7���ٻ�.
		//! ASDU21 ���ڶ�ȡ��Group�ż�����
		OpcUa_Int32		m_iASDU21ReadGroupPeriod;				//ASDU21 ���ڶ�ȡ��Group�ŵ�����,��. 0��ʾ������ASDU21���ٻ�.
		vector<OpcUa_Byte> m_vGroupNo;						//ASDU21 ���ڶ�ȡ��Group��
		OpcUa_Int32		m_iGroupIndex;							//ASDU21 ��������. 

		bool    m_bConfirmed;							//! �ϴ�Ӧ���п����ֶ��еĹ�������ʾΪ0(ȷ��֡,ȷ��)��8(��Ӧ֡,��������Ӧ����֡)

		bool m_bRetryPeriod;	//�����豸�÷���������

		bool m_bIsSuspicious;	//�豸����

		//���������õ�AI��ϵ��
		double GetModulusByTag(OpcUa_Int32 iTag)
		{
			AIModulusMapiterator it = m_AIModulusMap.find(iTag);
			if (it != m_AIModulusMap.end())
				return it->second;
			//	return -1;	//Del by hxw in 2008.1.30
			return 0;	//Add by hxw in 2008.1.30
		}

	protected:
		OpcUa_Byte					m_byBitMask[8];			//λ����Э������

	private:
		OpcUa_Int32						m_iTypeCount;			//�������͵�����	
		OpcUa_Byte*					m_pDataAddr;			//�豸���ݱ���λ��	
		bool					m_bCanSendSoe;			//!����ʱ���ж��Բ�SOE��־		
	};
}

#endif	
