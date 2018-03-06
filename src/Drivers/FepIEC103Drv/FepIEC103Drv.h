/*!
*	\file	FepIEC103Drv.h
*
*	\brief	103��վ������ͷ�ļ�
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Drv.h 1     11-01-21 10:02a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-01-21 10:02a $
*	$Revision: 1 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef _FEP_IEC103Drv_20110110_H_
#define _FEP_IEC103Drv_20110110_H_

#include "PublicFileH.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"
namespace MACS_SCADA_SUD
{
	class CPRDCfgParser;

	/*!
	*	\class	CFepIEC103Drv
	*
	*	\brief	IEC103����������
	*/
	class FEPIEC103_API CFepIEC103Drv 
		:public CIODriver
	{
		friend void OnTmOut(void* p);		
	public:
		CFepIEC103Drv();
		//! ȱʡ��������
		~CFepIEC103Drv();

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
		virtual OpcUa_Int32 StartWork(void);

		/*!
		*	\brief	��ͨ��
		*
		*	����ͨ������������ͨ�����������������߳�
		*
		*	\param	portCfg	��¼ͨ����ϸ���õĽṹ��
		*
		*	\retval	OpcUa_Int32	�ɹ��򷵻�0
		*/
		virtual OpcUa_Int32 OpenChannel();


		//! ��ʼ���������Ӳ���
		OpcUa_Int32 InitNetParam();

		//! ��ӷ�����
		void AddSerAddr(const OpcUa_CharA* pchSerAddr);

		//! ��װһ�����Ӳ���������Ϊ���������
		void Connect(OpcUa_Int32 iIndex = 0);	

		//! ��÷���IP
		std::string GetSerAddr( OpcUa_Int32 Num );

		//! ��÷���IP��ַ�ܸ���
		OpcUa_Int32 GetSerAddrNum();

		//! ��õ�ǰ���ӵķ�����IP���
		OpcUa_Int32 GetCurSerAddrIndex();

		//! ��������Ӵ���
		OpcUa_Int32 GetConnTimes();

		//! �������Ӵ���
		inline void SetConnTime(OpcUa_Int32 nTime)
		{
			m_iConnTimes = nTime;
		};

		//! �����Ӵ�����1
		inline void IncrConnTime()
		{
			m_iConnTimes ++;
		};

		/*!
		*	\brief	������Ӷ�ʱ��ָ��
		*/
		inline CIOTimer* GetConnTimer()
		{
			return m_pConnTimer;
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

		/*!
		*	\brief	��õ�ǰ���ӵķ�����IP
		*/
		std::string GetCurServerAddr()
		{
			return m_strSerAddrList[m_iCurSerAddrIndex];
		};

		//! �����־ָ��
		CIOLog* GetLog()
		{
			return m_pLog;
		}

		//! ֱ�Ӽ�¼��־
		OpcUa_Int32 WriteLogAnyWay( const OpcUa_CharA* pchLog, bool bTime = true );

		bool CanWork();

		CPRDCfgParser* GetPRDCfgParser();

	public:
		//! ����
		static std::string s_ClassName;
		std::string m_strSerAddrList[4];				//!< ��������ַ�б�	
		OpcUa_Int32 m_iPort;
		OpcUa_Int32 m_iConnTimes;						//! �����Ӵ���
		OpcUa_Int32 m_iSerAddrNum;						//! �������ܸ���
		OpcUa_Int32 m_iCurSerAddrIndex;					//! ��ǰ���ӵķ��������
		CIOTimer* m_pConnTimer;
		//! �����������Ƶ���
		ACE_Recursive_Thread_Mutex m_Mutex;
		bool m_bIsConnecting;
	private:
		CPRDCfgParser* m_pPrdCfgParser;					//!����¼�������ļ���������

		bool m_bCanWork;	
		
	};
}

#endif
