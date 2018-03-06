/*!
*	\file	NR103Driver.h
*
*	\brief	NR103驱动类
*
*	$Date: 15-03-30$
*
*/
#include "NR103Driver.h"
#include "NR103Prtcl.h"
#include "NR103Device.h"

namespace MACS_SCADA_SUD
{
	std::string CNR103Driver::s_ClassName("CNR103Driver");

	void NR103ConnTOut(void* pParam)
	{
		CNR103Driver* pIODriver = (CNR103Driver*)(pParam);
		if (pIODriver && pIODriver->GetNetConnTimer())
		{
			OpcUa_Int32 nRes = pIODriver->GetNetConnTimer()->KillTimer();
			UaString strMsg(UaString("CNR103Driver::NR103ConnTOut connect svrAddr: %1 time out").arg(pIODriver->GetCurSvrAddr().c_str()));
			nRes = pIODriver->WriteLogAnyWay(strMsg.toUtf8());
			//处理超时期间可能连接成功情况
			if (pIODriver->m_pIODrvBoard->ConnState())
			{
				strMsg = UaString(UaString("CNR103Driver::NR103ConnTOut connect svrAddr: %1 successful").arg(pIODriver->GetCurSvrAddr().c_str()));
				pIODriver->WriteLogAnyWay(strMsg.toUtf8());
				return;		//连接成功返回
			}
			pIODriver->ReConnect();

		}
	}

	CNR103Driver::CNR103Driver()
		:m_pConnTimer(NULL),m_nPort(6000),m_nSvrAddrIndex(0),/*m_pPrtcl(NULL)*/
		 m_nCurAddrIndex(0),m_nReConnCount(0)
	{

	}

	CNR103Driver::~CNR103Driver()
	{
		if (m_pConnTimer)
		{
			m_pConnTimer->KillTimer();
		}
	}

	std::string CNR103Driver::GetClassName()
	{
		return s_ClassName;
	}

	OpcUa_Int32 CNR103Driver::OpenChannel()
	{
		OpcUa_Int32 nRes = -1;
		nRes = CIODriver::OpenChannel();
		if (0 == nRes)
		{
			m_pIODrvBoard = new CIODrvBoard();
			if (m_pIODrvBoard)
			{
				std::string strPType = m_pPortCfg.strType.toUtf8();
				nRes = m_pIODrvBoard->Init(m_pIOPrtclParser, strPType, m_pPortCfg, this->GetTimeOut());
				if (0 == nRes)
				{
					//打开驱动器
					nRes = m_pIODrvBoard->open();
					if (nRes != 0)
					{
						UaString strErrMsg("NR103Driver::OpenChannel: Open IODrvBoard Failed!\n");
						WriteLogAnyWay(strErrMsg.toUtf8());					
					}
				}
				else
				{
					//连接设备失败置离线
					SetDevsStatue(DEVICE_OFFLINE);
					UaString strErrMsg("NR103Driver::OpenChannel: Init IODrvBoard Failed!\n");
					WriteLogAnyWay(strErrMsg.toUtf8());
				}
			}
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Driver::StartWork( void )
	{
		OpcUa_Int32 nRes = -1;
		InitNetParams();
		m_pConnTimer = new CIOTimer(NR103ConnTOut,this);
		CNR103Prtcl* pPrtcl = dynamic_cast<CNR103Prtcl*>(m_pIOPrtclParser);
		if (pPrtcl)
		{
			std::string strDrvName = GetDriverName();
			//初始化驱动工作信息
			pPrtcl->InitConfig(strDrvName);
			//开始连接设备
			nRes = Connect();
		}
		return nRes;
	}

	void CNR103Driver::InitNetParams()
	{
		m_nPort	= m_pPortCfg.nPort;		
		AddSerAddr(m_pPortCfg.strIPA_A.toUtf8());
		AddSerAddr(m_pPortCfg.strIPA_B.toUtf8());
		AddSerAddr(m_pPortCfg.strIPB_A.toUtf8());
		AddSerAddr(m_pPortCfg.strIPB_B.toUtf8());
	}

	void CNR103Driver::AddSerAddr(const OpcUa_CharA* pchAddr)
	{
		if (pchAddr && (strcmp("", pchAddr)!= 0) && m_nSvrAddrIndex < NETADDRS_LEN)
		{
			m_arrSvrAddrs[m_nSvrAddrIndex++] = pchAddr;
		}
	}

	OpcUa_Int32 CNR103Driver::Connect( OpcUa_Int32 nSvrIndex /*= 0*/ )
	{
		OpcUa_Int32 nRes = -1;
		if (nSvrIndex >= 0 && nSvrIndex < NETADDRS_LEN)
		{
			CNR103Prtcl* pPrtcl = dynamic_cast<CNR103Prtcl*>(m_pIOPrtclParser);
			if (m_pIODrvBoard && m_pConnTimer && pPrtcl)
			{
				nRes = m_pIODrvBoard->Connect(m_arrSvrAddrs[nSvrIndex].c_str(), m_nPort);
				if (0 == nRes)
				{
					//开启设备连接超时定时器
					nRes = m_pConnTimer->SetTimer(pPrtcl->GetConnTimeOut());
				}
				else
				{
					//连接设备失败置离线
					SetDevsStatue(DEVICE_OFFLINE);
					UaString strMsg(UaString("NR103Driver::CNR103Driver:Connect failed,IP %1").arg(m_arrSvrAddrs[nSvrIndex].c_str()));
					WriteLogAnyWay(strMsg.toUtf8());
					//在连接失败的情况下也启动定时器进行链路轮询诊断连接
					nRes = m_pConnTimer->SetTimer(pPrtcl->GetConnTimeOut());
				}
			}
		}
		return nRes;
	}

	std::string CNR103Driver::GetPrtclClassName()
	{
		return "CNR103Prtcl";
	}

	std::string CNR103Driver::GetDeviceClassName()
	{
		return "CNR103Device";
	}

	CIOTimer* CNR103Driver::GetNetConnTimer()
	{
		return m_pConnTimer;
	}

	OpcUa_Int32 CNR103Driver::ReConnect()
	{
		OpcUa_Int32 nRes = -1;
		m_nReConnCount ++;
		CNR103Prtcl* pPrtcl = dynamic_cast<CNR103Prtcl*>(m_pIOPrtclParser);
		if (pPrtcl)
		{
			if (m_nReConnCount >=pPrtcl->GetMaxReConn())
			{
				m_nReConnCount = 0;
				m_nCurAddrIndex ++;
				if (m_nCurAddrIndex >= NETADDRS_LEN)
				{
					m_nCurAddrIndex = 0;
				}
				nRes = Connect(m_nCurAddrIndex);

				//多次连接同一IP失败则置设备离线
				SetDevsStatue(DEVICE_OFFLINE);
			}
			else
			{
				nRes = Connect(m_nCurAddrIndex);	//连接下一个IP地址
			}
		}
		return nRes;
	}

	std::string CNR103Driver::GetCurSvrAddr()
	{
		return m_arrSvrAddrs[m_nCurAddrIndex];
	}

	void CNR103Driver::SetDevsStatue( OpcUa_Byte byStatue )
	{
		//! 置设备离线
		for( OpcUa_Int32 i = 0; i < GetDeviceCount(); i++)
		{
			CIODeviceBase* pDevice = GetDeviceByIndex(i);
			if (pDevice && !pDevice->IsSimulate() )
			{
				pDevice->SetOnLine(byStatue);
			}
		}
	}

	OpcUa_Boolean CNR103Driver::IsAllOffLine()
	{
		OpcUa_Boolean bRes = OpcUa_True;
		for( OpcUa_Int32 i = 0; i < GetDeviceCount(); i++)
		{
			CIODeviceBase* pDevice = GetDeviceByIndex(i);
			if (pDevice && pDevice->IsOnLine())
			{
				bRes = OpcUa_False;
			}
		}
		return bRes;
	}

}



