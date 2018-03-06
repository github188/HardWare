/*!
*	\file	NR103Driver.h
*
*	\brief	NR103驱动类
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

		//!初始化链路连接地址列表
		void InitNetParams();
		//!添加链路连接地址
		void AddSerAddr(const OpcUa_CharA* pchAddr);

		OpcUa_Int32 Connect(OpcUa_Int32 nSvrIndex = 0);

		virtual std::string GetPrtclClassName();

		virtual std::string GetDeviceClassName();

		CIOTimer* GetNetConnTimer();
		//!进行重连
		OpcUa_Int32 ReConnect();
		
		//!得到当前正在连接IP地址
		std::string GetCurSvrAddr();

		//!设置链路下设备设备状态
		void SetDevsStatue( OpcUa_Byte byStatue );

		//!是否所有设备都已离线
		OpcUa_Boolean IsAllOffLine();
	public:
		static std::string s_ClassName;
	private:
		std::string m_arrSvrAddrs[4];			//!链路地址数组
		CIOTimer* m_pConnTimer;
		OpcUa_UInt32 m_nSvrAddrIndex;
		OpcUa_UInt32 m_nCurAddrIndex;
		OpcUa_UInt32 m_nReConnCount;			//!连接超时次数
		OpcUa_Int32 m_nPort;					//!端口号
		//CNR103Prtcl* m_pPrtcl;					//!协议解析类指针
	};
}

#endif