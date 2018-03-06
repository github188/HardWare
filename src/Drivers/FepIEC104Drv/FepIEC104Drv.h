/*!
 *	\file	$FILE_BASE$.$FILE_EXT$
 *
 *	\brief	104��վ������ͷ�ļ�
 *
 *	$Header: ?/MACS-SCADA 3.2 �������/MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Drv.h, 23+1 2011/04/26 05:11:09 miaoweijie $
 *	$Author: miaoweijie $
 *	$Date: 2011/04/26 05:11:09 $
 *	$Revision: 23+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef IOMACSIEC104DRIVER_20080812_H
#define IOMACSIEC104DRIVER_20080812_H

#include "FepIEC104DrvH.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"

namespace MACS_SCADA_SUD
{	
	class IOMACSIEC104_API CFepIEC104Drv :
		public CIODriver
	{
	public:
		~CFepIEC104Drv();

		/*!
		*	\brief	�������
		*
		*	\retval	std::string ����
		*/
		std::string GetClassName();

		/*!
		*	\brief	�õ��̳�PrtclParserBase����ļ̳�����
		*
		*	\retval	Э������
		*/
		virtual std::string GetPrtclClassName();

		/*!
		*	\brief	�õ��̳�DeviceBase����ļ̳�����
		*
		*	\retval	�豸����
		*/
		virtual std::string GetDeviceClassName();
		
		/*!
		*	\brief	��ʼ����
		*
		*	\retval	����ֵ��0--�ɹ�������ʧ��
		*/
		virtual OpcUa_Int32 StartWork(void);

		/*!
		*	\brief	��ʼ���������Ӳ���
		*
		*	\retval	����ֵ��0--�ɹ�������ʧ��
		*/
		OpcUa_Int32 InitNetParam();

		/*!
		*	\brief	��ӷ�����
		*
		*	\param	pchSerAddr	Ҫ��ӵķ�������ַ
		*
		*	\retval	void
		*/
		void AddSerAddr(const char* pchSerAddr);

		/*!
		*	\brief	��װһ�����Ӳ���
		*
		*	\param	nIndex	���������
		*
		*	\retval	OpcUa_Int32
		*/
		OpcUa_Int32 Connect(OpcUa_Int32 nIndex = 0);

		/*!
		*	\brief	���õ�ǰ�豸��
		*
		*	\param	nIndex	���������
		*
		*	\retval	OpcUa_Int32 Ϊ0��ʾ���óɹ��������ʾʧ��
		*/
		OpcUa_Int32 SetCurDevIndex(OpcUa_Int32 nIndex)
		{
			if( nIndex < 0 || nIndex >= GetDeviceCount() )
				return -1;

			m_iCurDevIndex = nIndex;
			return 0;
		};

	public:
		inline bool IsConnecting()
		{
			return m_bIsConnecting;
		}
		inline void IsConnecting(bool bIsConnecting)
		{
			m_bIsConnecting = bIsConnecting;
		}

		//! ��λ�����Ӵ���
		inline void ResetConnTimes()
		{
			m_nConnTimes = 0;
		}
		//! ����һ�������Ӵ���
		inline void IncrConnTimes()
		{
			m_nConnTimes++;
		}
		//! ��ȡ�����Ӵ���
		inline OpcUa_Int32 GetConnTimes()
		{
			return m_nConnTimes;
		}

		//! ָ����һ��������
		inline OpcUa_Int32 NextSerAddrIndex()
		{
			++m_nCurSerAddrIndex;
			if( m_nCurSerAddrIndex >= m_nSerAddrNum)
				m_nCurSerAddrIndex = 0;

			return m_nCurSerAddrIndex;
		}

		//! �������Ӷ�ʱ��
		inline OpcUa_Int32 SetConnTimer(OpcUa_UInt32 nElapse)
		{
			if(m_pConnTimer != NULL)
				return m_pConnTimer->SetTimer(nElapse);
			else
				return -1;
		}

		//! �ر����Ӷ�ʱ��
		inline OpcUa_Int32 KillConnTimer()
		{
			if(m_pConnTimer != NULL)
				return m_pConnTimer->KillTimer();
			else
				return -1;
		}

		//! ���Ƿ������ϵı�־
		inline void Connected(bool bConned)
		{
			m_bHasConnected = bConned;
		};

		//! �Ƿ�������
		inline bool Connected(void)
		{
			return m_bHasConnected;
		};

		/*!
		*	\brief	����
		*
		*	�������ʧ�ܣ�������
		*	��������������m_byMaxReConn������ʧ������һ��������
		*
		*/
		void ReConnect();

		OpcUa_Int32 WriteLogAnyWay( const char* pchLog, bool bTime = true );

		//! ��õ�ǰ���ӵķ�������ַ
		std::string GetCurServerIP()
		{
			return m_stSerAddrList[m_nCurSerAddrIndex];
		};

		//! ������Ԫ������ʹ��Щ�����ܷ��ʱ����˽�����ݳ�Ա
		friend void OnConnectTimeOut(void* p);							//! ��ʱ����ʱ�ص�����


	public:
		//! ����
		static std::string s_ClassName;	
		//! �����������Ƶ���
		ACE_Recursive_Thread_Mutex m_Mutex;

	private:				
		CIOTimer* m_pConnTimer;								//! ���Ӷ�ʱ��
		
		std::string m_stSerAddrList[4];						//! ��������ַ�б�	
		OpcUa_Int32 m_nPort;										//! �������˿ں�

		OpcUa_Int32 m_nConnTimes;									//! �����Ӵ���
		OpcUa_Int32 m_nSerAddrNum;									//! �������ܸ���
		OpcUa_Int32 m_nCurSerAddrIndex;								//! ��ǰ���ӵķ��������

		bool m_bIsConnecting;							//! �Ƿ���������
		bool m_bHasConnected;							//! �Ѿ������ϵı�־
	};
}

#endif
