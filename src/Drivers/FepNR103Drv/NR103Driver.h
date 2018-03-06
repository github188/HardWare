/*!
*	\file	NR103Driver.h
*
*	\brief	NR103������
*
*	$Date: 15-03-30$
*
*/
#ifndef _NR103DRIVER_H_
#define _NR103DRIVER_H_
#include "NR103CommDef.h"
#include "RTDB/Server/DBDriverFrame/IODriver.h"

namespace MACS_SCADA_SUD
{
	class CNR103Prtcl;

	class FEPNR103Drv_API CNR103Driver : public CIODriver
	{
	public:
		CNR103Driver();

		~CNR103Driver();

		virtual OpcUa_Int32 StartWork( void );

		virtual OpcUa_Int32 OpenChannel();

		std::string GetClassName();

		//!��ʼ����·���ӵ�ַ�б�
		void InitNetParams();
		//!�����·���ӵ�ַ
		void AddSerAddr(const OpcUa_CharA* pchAddr);

		OpcUa_Int32 Connect(OpcUa_Int32 nSvrIndex = 0);

		virtual std::string GetPrtclClassName();

		virtual std::string GetDeviceClassName();

		CIOTimer* GetNetConnTimer();
		//!��������
		OpcUa_Int32 ReConnect();
		
		//!�õ���ǰ��������IP��ַ
		std::string GetCurSvrAddr();

		//!������·���豸�豸״̬
		void SetDevsStatue( OpcUa_Byte byStatue );

		//!�Ƿ������豸��������
		OpcUa_Boolean IsAllOffLine();
	public:
		static std::string s_ClassName;
	private:
		std::string m_arrSvrAddrs[4];			//!��·��ַ����
		CIOTimer* m_pConnTimer;
		OpcUa_UInt32 m_nSvrAddrIndex;
		OpcUa_UInt32 m_nCurAddrIndex;
		OpcUa_UInt32 m_nReConnCount;			//!���ӳ�ʱ����
		OpcUa_Int32 m_nPort;					//!�˿ں�
		//CNR103Prtcl* m_pPrtcl;					//!Э�������ָ��
	};
}

#endif