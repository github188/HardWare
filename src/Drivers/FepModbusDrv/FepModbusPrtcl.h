/*!
 *	\file	FepModbusPrtcl.h
 *
 *	\brief	ͨ��Modbus������Э����ͷ�ļ�
 *
 *	$Header: ?/MACS-SCADA 3.2 �������/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusPrtcl.h, 470+1 2011/04/22 01:56:47 xuhongtao $
 *	$Author: xuhongtao $
 *	$Date: 2011/04/22 01:56:47 $
 *	$Revision: 470+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _FEP_MODBUS_PRTCL_20080513_H_
#define _FEP_MODBUS_PRTCL_20080513_H_

#include "ace/Timer_Heap.h"
#include "ace/Timer_Queue_Adapters.h"
#include "RTDBDriverFrame/IOPrtclParser.h"
#include "FepModbusDrvH.h"
#include "FepModbusDriver.h"

namespace MACS_SCADA_SUD
{	
	class CFepModbusDevice;
	/*!
	*	\class	CFepModbusPrtcl
	*
	*	\brief	TEP-I-Fֱ����΢�����װ��������Э����
	*/
	class FEPMODBUSDRV_API CFepModbusPrtcl:
		public CIOPrtclParser
	{
	public:
		/*!
		*	\brief	ȱʡ���캯��
		*/
		CFepModbusPrtcl(void);
		/*!
		*	\brief	ȱʡ��������
		*/
		~CFepModbusPrtcl(void);

	public:
		/*!
		*	\brief	��ʼ������
		*/
		virtual void Init( Byte byPrtclType, int nMaxReSend );

		virtual int RxDataProc( Byte* pDataBuf, int& nLen );

		/*!
		*	\brief	������֡
		*
		*	\param	pbyData		֡�׵�ַ
		*	\param	nDataLen	֡��
		*
		*	\retval	int	0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int BuildRequest( Byte* pbyData, int& nDataLen );

		/*!
		*	\brief	���ݲ���������
		*
		*	\param	pDevice		����֡���豸
		*	\param	pbyData		���ݰ��׵�ַ
		*	\param	nDataLen	���ݰ�����
		*
		*	\retval	bool	�ɹ�true������false
		*/
		int BackupAndSendRequest( 
			CFepModbusDevice* pDevice, Byte* pbyData, int nDataLen );

		/*!
		*	\brief	�õ���С�İ�����
		*
		*	\retval	int	��С����
		*/
		virtual int GetMinPacketLength();

		//! ���ӳɹ�
		virtual int OnConnSuccess(std::string pPeerIP );

		/*!
		*	\brief	�õ��������֡���豸��ַ
		*
		*	\retval	Byte �������֡���豸��ַ
		*/
		inline Byte GetLastAddr()
		{
			return m_bySendAddr;
		};

		//! ��ǰ����֡�Ƿ�Ϊͨ�����֡
		inline bool IsRTUChannelTest()
		{
			return m_bRTUChannelTest;
		};

		//! ����ͨ�����֡��־
		inline void SetRTUChannelTest(bool bRTUChannelTest)
		{
			m_bRTUChannelTest = bRTUChannelTest;
		};

		//! ����ң�ذ���֡����
		inline void SetCtrlInsertLen(int nCtrlInsertLen)
		{
			m_nCtrlInsertLen = nCtrlInsertLen;
		};

		//! ��ý�Ҫ���͵�ң�ذ���֡����
		inline int GetCtrlInsertTempLen()
		{
			return m_nCtrlInsertTempLen;
		};

		//! ���ý�Ҫ���͵�ң�ذ���֡����
		inline void SetCtrlInsertTempLen( int nCtrlInsertTempLen )
		{
			m_nCtrlInsertTempLen = nCtrlInsertTempLen;
		};

		//! ���Э���Ƿ�ΪMODBUS TCP
		inline bool IsModbusTCP()
		{
			return ( m_byPrtclType == 1 );
		}

		//! ���Э���Ƿ�ΪMODBUS TCP
		inline bool IsNetConn()
		{
			return ( m_byPrtclType == 1 || m_byPrtclType == 2 );
		}

		//! �����������ͱ�־
		inline void SetCmdType( Byte byCmdType)
		{
			m_bycmdType = byCmdType;
		};
		ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>* GetTimerMgr(){return m_pTimerMgr;};
		//! ���ݱ������ݻ������ַ�����byRcvOrSnd��1ΪRcv��0ΪSnd��
		std::string GetCommData( Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd );

		//! �������
		std::string GetClassName();

		//! ������Ԫ������ʹ��Щ�����ܷ��ʱ����˽�л򱣻������ݳ�Ա
		friend void OnTimeOut(void* p);
		friend void PeriodProc(void* p);
		friend void InsertDelayProc(void* p);

	protected:
		/*!
		*	\brief	��������Ԫ��ʶ
		*
		*	\param	High	��ʶ��λ
		*	\param	Low		��ʶ��λ
		*
		*	\retval	int	Modbus TCP�����С
		*/
		void GetPacketNo( Byte& High, Byte& Low );

		/*!
		*	\brief	��Modbus RTU����ת����Modbus TCP����
		*
		*	\param	pRTU		RTU����
		*	\param	iRTULen		RTU�����С
		*	\param	pTCP		TCP����
		*
		*	\retval	int	Modbus TCP�����С
		*/
		int MBRTU2MBTCPRequest( Byte* pRTU, int iRTULen, Byte* pTCP ); 

		/*!
		*	\brief	�������������ݰ�
		*
		*	�ӽ��ջ���������ȡһ��֡����ȡ��֡���ݱ�����pPacketBuffer��
		*
		*	\param	pPacketBuffer	�����������ݰ�֡ͷ
		*	\param	pnPacketLength	����������ݵĳ���
		*
		*	\retval	bool	�ɹ�true������false
		*/
		bool ExtractPacket( Byte* pPacketBuffer, int* pnPacketLength);

		/*!
		*	\brief	������ѯ֡���ذ�
		*
		*	�����pFrame��ʼ�����ݻ��������Ƿ�����������ѯ֡���ذ�
		*
		*	\param	pFrame		֡ͷָ��
		*	\param	nCorrectLen	���н�������ȷ��ң�ط��Ű���
		*						���øò���Ϊ��ȷ֡�ĳ���
		*
		*	\retval	int			�������£�
		*		FLAG_NONE				0		������ѯ֡���ذ�
		*		FLAG_UNCOMPLETE			1		����ѯ֡���ذ�����û��������
		*		FLAG_CORRECTFRAME		2		����ȷ����ѯ֡���ذ�
		*		FLAG_ERROR				3		��������˵������֡ͷλ�ò���
		*/
		int ExtractMsgPacket( const Byte* pFrame, int &nCorrectLen );

		/*!
		*	\brief	����ң�ط��Ű�
		*
		*	�����pFrame��ʼ�����ݻ��������Ƿ���������ң�ط��Ű�
		*
		*	\param	pFrame		֡ͷָ��
		*	\param	nCorrectLen	���н�������ȷ��ң�ط��Ű���
		*						���øò���Ϊ��ȷ֡�ĳ���
		*
		*	\retval	int			�������£�
		*		FLAG_NONE				0		����ң�ط���֡
		*		FLAG_UNCOMPLETE			1		��ң�ط���֡����û��������
		*		FLAG_CORRECTFRAME		2		����ȷ��ң�ط���֡
		*		FLAG_ERROR				3		��������˵������֡ͷλ�ò���
		*/
		int ExtractCtrlPacket( const Byte* pFrame, int &nCorrectLen );

		/*!
		*	\brief	����ң�ļ��������Ű�
		*
		*	�����pFrame��ʼ�����ݻ��������Ƿ����������ļ��������Ű�
		*
		*	\param	pFrame		֡ͷָ��
		*	\param	nCorrectLen	���н�������ȷ��ң�ط��Ű���
		*						���øò���Ϊ��ȷ֡�ĳ���
		*
		*	\retval	int			�������£�
		*		FLAG_NONE				0		����ң�ط���֡
		*		FLAG_UNCOMPLETE			1		��ң�ط���֡����û��������
		*		FLAG_CORRECTFRAME		2		����ȷ��ң�ط���֡
		*		FLAG_ERROR				3		��������˵������֡ͷλ�ò���
		*/
		int ExtractFilePacket( const Byte* pFrame, int &nCorrectLen );

		/*!
		*	\brief	������������ķ��Ű�
		*
		*	�����pFrame��ʼ�����ݻ��������Ƿ�����������������ķ��Ű�
		*
		*	\param	pFrame		֡ͷָ��
		*	\param	nCorrectLen	���н�������ȷ��ң�ط��Ű���
		*						���øò���Ϊ��ȷ֡�ĳ���
		*
		*	\retval	int			�������£�
		*		FLAG_NONE				0		����ң�ط���֡
		*		FLAG_UNCOMPLETE			1		��ң�ط���֡����û��������
		*		FLAG_CORRECTFRAME		2		����ȷ��ң�ط���֡
		*		FLAG_ERROR				3		��������˵������֡ͷλ�ò���
		*/
		virtual int ExtractOtherPacket( const Byte* pFrame, int &nCorrectLen );

		/*!
		*	\brief	�������������ݰ�
		*
		*	������ȡ�����Ľ�������֡����д����������Ҫʱ���ͱ�λ
		*
		*	\param	pPacketBuffer	���ݰ�
		*	\param	nPacketLength	���ݰ�����
		*	\param	bFromLocal		�����Ǳ������Ļ����������
		*
		*	\retval	void
		*/		
		virtual void ProcessRcvdPacket(	Byte* pPacketBuffer, int nPacketLength, bool bFromLocal );

		/*!
		*	\brief	������������ݰ����Ѿ�ת��ΪRTU��ʽ�����ݰ���
		*
		*	\param	pPacketBuffer	���ݰ�
		*	\param	nPacketLength	���ݰ�����
		*	\param	bFromLocal		�����Ǳ������Ļ����������
		*
		*	\retval	void
		*/		
		virtual void ProcessRcvdSplPacket(Byte* pPacketBuffer, int nPacketLength, bool bFromLocal);

		/*!
		*	\brief	����16λ����У���--CRC
		*
		*	1��	��һ16λ�Ĵ���λȫ1��
		*	2��	���������ݵĸ�λ�ֽ����Ĵ����ĵͰ�λ������Ĵ�����
		*	3��	���ƼĴ��������λ��0���Ƴ��ĵͰ�λ�����־λ��
		*	4��	���־λ��1������A001���Ĵ��������־λ��0����������3��
		*	5��	�ظ�����3��4��ֱ����λ��λ��
		*	6��	�����һλ�ֽ���Ĵ�����
		*	7��	�ظ�����3��5��ֱ�����б������ݾ���Ĵ��������λ8�Σ�
		*	8��	��ʱ�Ĵ����м�ΪCRCУ���룬���λ�ȷ��ͣ�
		*
		*	\param	datapt		������ʼλ��
		*	\param	bytecount	�ֽڸ���
		*
		*	\retval	WORD	16λ������У���
		*/
		WORD CRC(const Byte * datapt,int bytecount);

		/*!
		*	\brief	�����豸��ַ��ȡ�豸������
		*
		*	\param	Addr	�豸��ַ
		*	\param	DevNo	�豸��ַ��Ӧ���豸������
		*
		*	\retval	bool	true��ʾ�ҵ���false��ʾû��
		*/
		bool GetDeviceNoByAddr( int Addr, int& DevNo );

		/*!
		*	\brief	��¼�ϴη���֡�е���Ҫ����
		*
		*	\retval	void
		*/
		void BackupLastRequest();

		/*!
		*	\brief	��ң��ң��֡
		*
		*	\param	pbyData		֡�׵�ַ
		*	\param	nDataLen	֡��
		*	\param	nTimeOut	��ʱʱ��
		*	\param	fCmdVal		���͵�����ֵ
		*
		*	\retval	int	0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int BuildCtrlFrame( 
			Byte* pbyData, int& nDataLen, int& nTimeOut, float& fCmdVal );

		/*!
		*	\brief	��ң�ذ���֡
		*
		*	\param	pDevice		�豸
		*	\param	lTagAddr	ң�ر�ǩ��ַ
		*
		*	\retval	int	0��ʾ�ɹ��������ʾʧ��
		*/
		int BuildCtrlFwReq( CFepModbusDevice* pDevice, int lTagAddr );

		/*!
		*	\brief	��������
		*
		*	\param	pDevice		����֡���豸
		*	\param	pbyData		���ݰ��׵�ַ
		*	\param	nDataLen	���ݰ�����
		*
		*	\retval	bool	�ɹ�true������false
		*/
		virtual int SendRequest(
			CFepModbusDevice* pDevice, Byte* pbyData, int nDataLen );

		/*!
		*	\brief	��ָ���豸��ͨ���������֡
		*
		*	\param	pDevice:Ҫ�������豸ָ��
		*	\param	pbyData: ��õ�ͨ���������֡����ָ��
		*	\param	nReturnLen:����֡���ܳ���
		*
		*	\retval	void
		*/
		void MakeChannelDiagFrame( 
			CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen );


		//! ��ȡ�����ļ�
		int ReadIniCfg(std::string csDriverName);

		//!��ʱ��ֹͣ����
		virtual int StopTimer();


	public:
		//! ����
		static std::string s_ClassName;	

		//! ��¼���η������ݵĳ���
		int m_nSndLen;

		CIOTimer* m_pPeriodTimer;			//! ���ڶ�ʱ��	

		
	protected:
		//! ��ʱ��
		ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>* m_pTimerMgr;
		CIOTimer* m_pIOTimer;				//! ��ʱ��ʱ��
		CIOTimer* m_pInsertDelayTimer;		//! ����������ʱ��ʱ��

		//! Э�����ͣ�0-Modbus RTU��1-Modbus TCP��2-���������Modbus RTU
		Byte m_byPrtclType;

		bool m_bRTUChannelTest;				//! ��ǰ����֡�Ƿ�Ϊͨ�����֡

		//! �ϴη���ң��֡��¼
		Byte m_pbyCtrlSendBuffer[MB_RTU_MAX_SEND_SIZE];	
		int	 m_nCtrlSendBufferLen;	//! �ϴη���ң��֡��Ч����

		/*
		 *	����ط�������0��1��2����������
		 *	0��ʾ���ظ���1��ʾ�ط�һ�Ρ�
		 *	ע�⣬�豸���߲��ط����㲥���ط���Ŀǰ�ݶ���ң�ز��ط�
		 */
		int	  m_nMaxResend;

		int	  m_nCurResend;			//! ��ǰ�ظ�����		

		Byte m_byActiveAddr;			//! ��ǰ���������е��豸��ַ�����ڽ�����֡��
		Byte m_byActiveFunc;			//! ��ǰ���������еĹ����루���ڽ�����֡��	

		Byte m_bycmdType;				//! �������ͱ�־��1Ϊң�أ�2Ϊң��
		float m_fCmdVal;				//! ң��ң��ʱ���͵�����ֵ
		tIOCtrlRemoteCmd m_tCtrlRemoteCmd;	//! ��ǰ�����ң��ң������		

		//! TCP/IP Modbusר��
		WORD m_wdPacketNo;							//! ����Ԫ��ʶ������ţ�
		/*
		 *	������͵��������ݹؼ����ֻ���.�±�Ϊ�������к�(0~65535)
		 *	�ڼ��� TCP/IP Modbus������ʱ��Ϊ�俪�ٿռ�
		 */
		SENDDATA* m_pSendData;

		//! ����������ÿ�η��������������¼֮��
		Byte m_bySendAddr;		//! �������֡�豸��ַ
		Byte m_bySendFunc;		//! �������֡������
		WORD m_wdSendStartAddr;			//! �������֡��ʼ��ַ
		WORD m_wdSendPoints;			//! �������֡������AI��Ϊ�Ĵ�������DI��Ϊbit����

		//! ң�ذ���֡������ң�غ�����ŵ���һ������
		int  m_nCtrlInsertTempLen;					//! 
		int  m_nCtrlInsertLen;						//! ��С. ÿ�η��ͺ�����
		Byte m_pbyCtrlInsertBuf[MB_RTU_MAX_SEND_SIZE];

		bool m_bIsFirstTime;

		bool m_bIsBroadFrame;						//! ��ǰ֡Ϊ�㲥֡���ж��Ƿ���Ҫ������ʱ��
	public:
		unsigned int m_nCounter;;					//! Уʱ����
		unsigned int gCountSetting;					//! ��ֵ�����
		ACE_Time_Value gDateTime;					//! ��ǰ��ʱ�䣬���ڶ���Уʱ��ʱ��ȶ� Add by: Wangyanchao
	};
}
#endif
