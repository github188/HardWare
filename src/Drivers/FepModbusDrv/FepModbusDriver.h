/*!
 *	\file	FepModbusDriver.h
 *
 *	\brief	ͨ��Modbus����������������ͷ�ļ�
 *
 *	$Header: ?/MACS-SCADA 3.2 �������/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusDriver.h, 470+1 2011/04/22 01:56:47 xuhongtao $
 *	$Author: xuhongtao $
 *	$Date: 2011/04/22 01:56:47 $
 *	$Revision: 470+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _FEP_MODBUS_DRIVER_20080514_H
#define _FEP_MODBUS_DRIVER_20080514_H

#include "FepModbusDrvH.h"
#include "RTDBDriverFrame/IODriver.h"

namespace MACS_SCADA_SUD
{	
	/*!
	*	\class	CFepModbusDriver
	*
	*	\brief	TEP-I-Fֱ����΢�����װ������������������
	*/
	class FEPMODBUSDRV_API CFepModbusDriver :
		public CIODriver
	{
	public:
		//! ȱʡ���캯��
		CFepModbusDriver();
		//! ȱʡ��������
		~CFepModbusDriver();

	public:
		//! �������
		std::string GetClassName();

		/*!
		*	\brief	�õ��̳�PrtclParserBase����ļ̳�����
		*
		*	\retval	����ֵ���̳�����
		*/
		virtual std::string GetPrtclClassName();

		/*!
		*	\brief	�õ��̳�DeviceBase����ļ̳�����
		*
		*	\retval	����ֵ���̳�����
		*/
		virtual std::string GetDeviceClassName();

		/*!
		*	\brief	��ʼ����
		*
		*	\retval	����ֵ��0--�ɹ�������ʧ��
		*/
		virtual int StartWork(void);

		/*!
		*	\brief	ֹͣ����
		*
		*	\retval	����ֵ��0--�ɹ�������ʧ��
		*/
		virtual int StopWork(void);

		/*!
		*	\brief	��ʼ���豸
		*	�õ��豸��������̬�����豸����
		*	�õ��豸ģ����������̬�����豸ģ�����
		*	�����豸��״̬�ṹָ�룬���ö���ָ�룻
		*	��ʼ���豸
		*
		*	\param	unitState���豸״ָ̬��
		*	\param	pCfgUnit���豸���ö���ָ��
		*
		*	\retval	����ֵ��0--�ɹ�������ʧ��
		*/
		virtual int InitUnit(CIOUnit* pCfgUnit);

		/*!
		*	\brief	��ͨ��
		*
		*	����ͨ������������ͨ�����������������߳�
		*
		*	\param	portCfg	��¼ͨ����ϸ���õĽṹ��
		*
		*	\retval	int	�ɹ��򷵻�0
		*/
		virtual int OpenChannel();

		//! ����TCP Modbus��һЩ����
		//////////////////////////////////////////////////////////////////////
		//! ��ʼ���������Ӳ���
		virtual int InitNetParam();

		//! ��ӷ�����
		void AddSerAddr(const char* pchSerAddr);

		//! ��װһ�����Ӳ���������Ϊ���������
		int Connect(int nIndex = 0);

		//! �������Ӵ���
		inline void SetConnTime(int nTime)
		{
			m_nConnTimes = nTime;
		};

		//! �����Ӵ�����1
		inline void IncrConnTime()
		{
			m_nConnTimes ++;
		};

		/*!
		*	\brief	������Ӷ�ʱ��ָ��
		*/
		inline CIOTimer* GetConnTimer()
		{
			return m_pConnTimer;
		}

		inline bool GetConnected()
		{
			return m_bConnected;
		}

		inline void SetConnected( bool bConnected )
		{
			m_bConnected = bConnected;
		}

		/*!
		*	\brief	����
		*
		*	�������ʧ�ܣ�������
		*	��������������m_byMaxReConn������ʧ������һ��������
		*
		*/
		void ReConnect(  bool bNext = false );

		/*!
		*	\brief	������һ��������
		*/
		void ConnectNext();	

		//! ֱ�Ӽ�¼��־
		int WriteLogAnyWay( const char* pchLog, bool bTime = true );

		/*!
		*	\brief	��õ�ǰ���ӵķ�����IP
		*/
		std::string GetCurServerAddr()
		{
			return m_stSerAddrList[m_nCurSerAddrIndex];
		};

		//����Ƿ���·���ߣ����Ƿ������豸�����ߡ�
		bool IsLinkOffline();

		//����Ƿ��������һ���豸IsFuzzy()Ϊtrue;
		bool IsFuzzy();

		bool CanWork();

		friend void OnTimeOut(void* p);

		//!����TCPIP���Ӷ�ʱ��
		int StartConnTimer();

		//!�õ��豸������ߴ���
		int GetMaxOffLineCount();

		//!�����豸���Ľ��ճ�ʱʱ��
		int GetRcvTimeOut();

		//!����Ƿ�ΪӢ������
		bool IsEnglishEnv();


	public:
		//! ����
		static std::string s_ClassName;	
		//! �����������Ƶ���
		ACE_Recursive_Thread_Mutex m_Mutex;
		
		int m_nWatchTime;		//��·��ͻģʽʱ���ӻ���������ڡ����롣

		int	m_iDevIndex;		//�豸�߼���Ԫ��: 0 ~ Channel[0]->nMaxUnit-1	
		int	m_iDevLastIndex;
		int	m_nMaxRetryPeriod;	//���ڲ����豸����ʱ�������豸�������ڷ�������֡�Ĵ���. ��λ�����ӡ�

		int m_nMaxLinkOffline;				//��·�������������
		int m_MaxHotLinkOffline;			//��·��ͻģʽʱ��������·�����������������ڴ�ʱ����û���յ��κ��豸���ģ���϶�����·���ߡ�
		int m_nMaxStandbyLinkOffline;		//��·��ͻģʽʱ���ӻ���·�����������������ڴ�ʱ����û���յ��κ��豸���ģ���϶�����·���ߡ�
		int m_nRcvTimeOut;					//���ճ�ʱ
		int m_nMaxOfflineCount;				//���߼�����
		bool m_bIsLinkGetRightFrame;		//��·���Ƿ���ܵ���ȷ��Ӧ֡
		int m_nMaxLinkOfflineConnectCount;	//�����·���ӳ�ʱ����
		int m_nLinkOfflineConnectCount;		//��ǰ��·���ӳ�ʱ����

		//! ͬһ����������������������ڴ������������ʱ��Ч����Ĭ��Ϊ3
		int m_byMaxReConn;
		//! �������ӳ�ʱʱ�䣨��λΪms����������ʱ������Ϊ����ʧ�ܣ�Ĭ��Ϊ2000ms
		int m_byConnTimeOut;

		int m_nSerAddrNum;					//! �������ܸ���

	protected:
		CIOTimer* m_pConnTimer;				//! ���Ӷ�ʱ��

		std::string m_stSerAddrList[MAX_SERVERIP_COUNT];	//! ��������ַ�б�
		int m_nPort;						//! �������˿ں�	

		int m_nConnTimes;					//! �����Ӵ���
		int m_nCurSerAddrIndex;				//! ��ǰ���ӵķ��������
		int m_nMaxReSend;					//! �ط�����������ң��ң��֡��Ч

		//! Э�����ͣ�0-Modbus RTU��1-Modbus TCP��2-���������Modbus RTU
		Byte m_byPrtclType;	
		
		//! �Ƿ������ӱ�־
		bool m_bConnected;

		bool m_bCanWork;

		bool m_bIsEnglish;
	protected:
		//! ��ȡ�����ļ�
		int ReadIniCfg();

	};	
}

#endif
