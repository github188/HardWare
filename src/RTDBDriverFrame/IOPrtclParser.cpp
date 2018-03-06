/*!
 *	\file	IOPrtclParser.cpp
 *
 *	\brief	协议类实现
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月8日	15:34
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBDriverFrame/IOPrtclParser.h"
#include "RTDBDriverFrame/IODriver.h"
#include "RTDBDriverFrame/IODeviceBase.h"
// #include "RTDB/Server/FEPPlatformClass/IOPort.h"
// #include "RTDBBaseClasses/IOCmdManagerBase.h"

namespace MACS_SCADA_SUD
{
	std::string CIOPrtclParser::s_ClassName("CIOPrtclParser");

	//!构造函数
	CIOPrtclParser::CIOPrtclParser()
		:m_nRevLength(0), m_iInnerLen(0), m_bSendPacketIsFromPeer(false), m_pbyInnerBuf(NULL),
		 m_pbyRevBuffer(NULL), m_pbySndBuffer(NULL), m_sPacketBuffer(NULL)
	{
		PRTCL_MIN_PACKET_LENGTH = 1;
		PRTCL_MAX_PACKET_LENGTH = 256;
		PRTCL_RCV_BUFFER_LENGTH = 1024;
		PRTCL_SND_BUFFER_LENGTH = 1024;
		PRTCL_MAX_INNER_LENGTH  = 2048;
		MACSDRIVER_MAX_TIMEOUT  = 1500;
		//memset( &m_cmdPacket, 0, sizeof( m_cmdPacket ) );
	}

	//!析构函数
	CIOPrtclParser::~CIOPrtclParser()
	{
		// if ( m_pbyRevBuffer )
		// {
		// 	delete []m_pbyRevBuffer;
		// 	m_pbyRevBuffer = NULL;
		// }

		// if ( m_pbySndBuffer )
		// {
		// 	delete []m_pbySndBuffer;
		// 	m_pbySndBuffer = NULL;
		// }

		// if ( m_sPacketBuffer )
		// {
		// 	delete []m_sPacketBuffer;
		// 	m_sPacketBuffer = NULL;
		// }

		// if ( m_pbyInnerBuf )
		// {
		// 	delete []m_pbyInnerBuf;
		// 	m_pbyInnerBuf = NULL;
		// }
	}

	//!组请求帧
	int CIOPrtclParser::BuildRequest(Byte* pbyData, int& nDataLen)
	{
		//!判断通道有没有被禁止
		/*	CIODriver* pDriver = (CIODriver*)m_pParam;
		if (!pDriver)
		{
		return -1;
		}
		if (pDriver->GetIOPort()->IsDisable() || pDriver->IsStopWork())
		{
		return -1;
		}*/
		return 0;
	}

	//!组请求帧
	int CIOPrtclParser::BuildRequest(Byte* pbyData, int&nDataLen, bool& bTrigTimer, int& nTimeOut)
	{
		//!判断通道有没有被禁止
		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// if (!pDriver)
		// {
		// 	return -1;
		// }
		// if (pDriver->GetIOPort()->IsDisable() || pDriver->IsStopWork())
		// {
		// 	return -1;
		// }
		return 0;
	}

	//!处理接收数据
	int CIOPrtclParser::RxDataProc(Byte* pDataBuf, int& nLen, bool& bStopTimer, bool& bSendData)
	{
		// SetRcvProcFlag( NULL, bStopTimer, bSendData );
		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// //!检查数据处理空间是否已准备就绪
		// if (!pDriver || !m_pbyInnerBuf || !m_pbyRevBuffer || !m_pbySndBuffer)
		// {
		// 	return -1;
		// }
		// //!通道是否禁止
		// if (pDriver->GetIOPort()->IsDisable())
		// {
		// 	return -2;
		// }

		// //!如果进行通信监视则发送原始数据包
		// pDriver->WriteCommData(pDataBuf, nLen);

		// //!把数据放到内部缓冲区
		// InnerAddData(pDataBuf, nLen);

		// //!在内部缓冲区执行滑动窗口找数据包,一次可能处理多个数据包
		// int nReceived = 0;
		// do 
		// {
		// 	//!可取的数据长度
		// 	nReceived = PRTCL_RCV_BUFFER_LENGTH - m_nRevLength;
		// 	if ( 0 >= nReceived || nReceived>PRTCL_RCV_BUFFER_LENGTH )
		// 	{
		// 		m_nRevLength = 0;
		// 		break;
		// 	}

		// 	//!从内部缓冲区取数分析
		// 	if ( InnerRead( m_pbyRevBuffer+m_nRevLength, nReceived, nReceived) && nReceived > 0 )
		// 	{
		// 		m_nRevLength += nReceived;
		// 		while( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH )
		// 		{
		// 			bool bCompleted = false;
		// 			int nPacketLen = 0;
		// 			memset( m_sPacketBuffer, 0, PRTCL_MAX_PACKET_LENGTH );

		// 			//!如果有多包完整数据取帧序列中最后一包完整的数据,前面数据包都丢弃
		// 			bool bTemp;
		// 			do 
		// 			{
		// 				bTemp = ExtractPacket( m_sPacketBuffer, &nPacketLen );
		// 				if (bTemp)
		// 				{
		// 					//!剩下的数据移动到接收缓冲区的开头
		// 					m_nRevLength = m_nRevLength - nPacketLen;
		// 					if (m_nRevLength < 0)
		// 					{
		// 						m_nRevLength = 0;
		// 					}
		// 					if (m_nRevLength > PRTCL_RCV_BUFFER_LENGTH)
		// 					{
		// 						m_nRevLength = PRTCL_RCV_BUFFER_LENGTH;
		// 					}
		// 					ACE_OS::memmove(m_pbyRevBuffer, m_pbyRevBuffer+nPacketLen, m_nRevLength);
		// 					bCompleted = true;
		// 				}
		// 			} while (bTemp);

		// 			//!找到一包完整的数据包进行处理
		// 			if (bCompleted)
		// 			{
		// 				ProcessRcvdPacket(m_sPacketBuffer, nPacketLen, true, bStopTimer, bSendData);
		// 				m_nRevLength = 0;
		// 				return 0;
		// 			} 
		// 			else
		// 			{
		// 				break;
		// 			}
		// 		}
		// 	}
		// } while (0 < nReceived);

		//!处理失败
		return -1;
	}

	//!处理接收数据
	int CIOPrtclParser::RxDataProc(Byte* pDataBuf, int& nLen)
	{
		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// //!检查数据处理空间是否已准备就绪
		// if (!pDriver || !m_pbyInnerBuf || !m_pbyRevBuffer || !m_pbySndBuffer)
		// {
		// 	return -1;
		// }
		// //!通道是否禁止
		// if (pDriver->GetIOPort()->IsDisable())
		// {
		// 	return -2;
		// }

		// //!如果进行通信监视则发送原始数据包
		// pDriver->WriteCommData(pDataBuf, nLen);

		// //!把数据放到内部缓冲区
		// InnerAddData(pDataBuf, nLen);

		// //!在内部缓冲区执行滑动窗口找数据包,一次可能处理多个数据包
		// int nReceived = 0;
		// do 
		// {
		// 	//!可取的数据长度
		// 	nReceived = PRTCL_RCV_BUFFER_LENGTH - m_nRevLength;
		// 	if ( 0 >= nReceived || nReceived>PRTCL_RCV_BUFFER_LENGTH )
		// 	{
		// 		m_nRevLength = 0;
		// 		break;
		// 	}

		// 	//!从内部缓冲区取数分析
		// 	if ( InnerRead( m_pbyRevBuffer+m_nRevLength, nReceived, nReceived) && nReceived > 0 )
		// 	{
		// 		m_nRevLength += nReceived;
		// 		while( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH )
		// 		{
		// 			bool bCompleted = false;
		// 			int nPacketLen = 0;
		// 			memset( m_sPacketBuffer, 0, PRTCL_MAX_PACKET_LENGTH );

		// 			//!如果有多包完整数据,取帧序列中最后一包完整的数据,前面数据包都丢弃
		// 			bool bTemp;
		// 			do 
		// 			{
		// 				bTemp = ExtractPacket( m_sPacketBuffer, &nPacketLen );
		// 				if (bTemp)
		// 				{
		// 					//!剩下的数据移动到接收缓冲区的开头
		// 					m_nRevLength = m_nRevLength - nPacketLen;
		// 					if (m_nRevLength < 0)
		// 					{
		// 						m_nRevLength = 0;
		// 					}
		// 					if (m_nRevLength > PRTCL_RCV_BUFFER_LENGTH)
		// 					{
		// 						m_nRevLength = PRTCL_RCV_BUFFER_LENGTH;
		// 					}
		// 					ACE_OS::memmove(m_pbyRevBuffer, m_pbyRevBuffer+nPacketLen, m_nRevLength);
		// 					bCompleted = true;
		// 				}
		// 			} while (bTemp);

		// 			//!找到一包完整的数据包进行处理
		// 			if (bCompleted)
		// 			{
		// 				ProcessRcvdPacket(m_sPacketBuffer, nPacketLen, true);
		// 				m_nRevLength = 0;
		// 				return 0;
		// 			} 
		// 			else
		// 			{
		// 				break;
		// 			}
		// 		}
		// 	}
		// } while (0 < nReceived);

		//!处理失败
		return -1;
	}

	//!接收到通道诊断包
	int CIOPrtclParser::RcvPcketCmd(tIOCtrlPacketCmd* ptPacket)
	{
		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// if (!pDriver)
		// {
		// 	return -1;
		// }

		// if (ptPacket->byType == IOPACKET_TYPE_SENDPACKET)		//!发送数据包
		// {
		// 	MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOPrtclParser::RcvPacketCmd 对方发过来个发送数据包!" ) ));
		// 	m_bSendPacketIsFromPeer = true;
		// 	memcpy( &m_cmdPacket, ptPacket, sizeof( m_cmdPacket ) );
		// }
		// else if (ptPacket->byType == IOPACKET_TYPE_RECVPACKET)	//!接收数据包
		// {
		// 	MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOPrtclParser::RcvPacketCmd 对方发过来个接收数据包!" ) ));
		// }
		return 0;
	}

	//!从接收缓冲区中提取一帧完整数据包
	bool CIOPrtclParser::ExtractPacket(Byte* pPacketBuffer, int* pnPacketLength)
	{
		return false;
	}

	//!处理提取出来完整数据包
	void CIOPrtclParser::ProcessRcvdPacket(Byte* pPacketBuffer, int nPacketLength, bool bFromLocal, bool& bStopTimer, bool& bSendData)
	{

	}

	//!处理提取出来完整数据包
	void CIOPrtclParser::ProcessRcvdPacket(Byte* pPacketBuffer, int nPacketLength, bool bFromLocal)
	{

	}

	//!设置接收处理中的标识
	int CIOPrtclParser::SetRcvProcFlag(CIODeviceBase* pDevice, bool& bStopTimer, bool& bSendData)
	{
		// if ( !pDevice )
		// {
		// 	bStopTimer = true;
		// 	bSendData = true;
		// 	return 0;
		// }
		// else
		// {
		// 	//! 设备处于模拟状态
		// 	if ( pDevice->IsSimulate() )
		// 	{
		// 		bStopTimer = false;
		// 		bSendData = false;
		// 	}

		// 	//! 设备为从机
		// 	if ( !pDevice->IsHot() )
		// 	{
		// 		//! 
		// 		bStopTimer = false;
		// 		bSendData = false;
		// 	}
		// }
		return 0;
	}

	//!给指定设备组通道诊断数据帧
	void CIOPrtclParser::MakeChannelDiagFrame(CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen)
	{
		nReturnLen = 0;
	}

	//!得到类名
	// std::string CIOPrtclParser::GetClassName()
	// {
	// 	return s_ClassName;
	// }

	//!得到帧数据缓冲区指针
	Byte* CIOPrtclParser::GetPacketBuffer()
	{
		return m_sPacketBuffer;
	}

	//!得到最小数据包长度
	int CIOPrtclParser::GetMinPacketLength()
	{
		return PRTCL_MIN_PACKET_LENGTH;
	}

	//!生成16位sum校验
	
	WORD CIOPrtclParser::Get16SumCode(Byte * Buf, int iWordLen)
	{
		WORD wSum = 0;
		// for(int i=0; i<iWordLen; i++)
		// 	wSum += Buf[i];
		return wSum;
	}

	//!返回16位WORD的高字节值
	Byte CIOPrtclParser::GetWordHigh(WORD wdCheck)
	{
		return Byte(wdCheck/0x100);
	}

	//!返回16位WORD的低字节值
	Byte CIOPrtclParser::GetWordLow(WORD wdCheck)
	{
		return Byte(wdCheck%0x100);
	}

	//!处理链路访问冲突,组包发送给另机
	int CIOPrtclParser::DealLinkClash(CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen)
	{
		//!如果与另机之间的通信链路断开则直接返回

		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// if (!pDriver)
		// {
		// 	return -2;
		// }

		// //!链路不冲突直接返回
		// if (!pDriver->IsPortAccessClash())
		// {
		// 	return 1;
		// }

		// //!只有设备为Hot且设备处于通道诊断状态
		// if ( !pDevice || !pDevice->IsHot() || !pDevice->IsStatusUnit() )
		// {
		// 	return 2;
		// }
		// MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOPrtclParser::DealLinkClash, 组通道诊断数据包，发送给另机!" ) ));

		// //!首先清除设备的通道诊断标识
		// pDevice->SetStatusUnit(false);

		// //!对该设备组通道诊断数据帧
		// MakeChannelDiagFrame(pDevice, pbyData, nReturnLen);
		// //!组帧失败
		// if (nReturnLen <= 0)
		// {
		// 	return 3;
		// }

		// //!发送至另机
		// if (g_pIOCmdManager && pDriver && pbyData)
		// {
		// 	tIOCtrlPacketCmd channelDiagPacket;
		// 	channelDiagPacket.objNodeId = pDriver->GetIOPort()->nodeId();
		// 	channelDiagPacket.byType = IOPACKET_TYPE_SENDPACKET;
		// 	channelDiagPacket.nLen = nReturnLen;
		// 	ACE_OS::memcpy(channelDiagPacket.byData, pbyData, nReturnLen);
		// 	int nRes = g_pIOCmdManager->SendIOChannelDiag(channelDiagPacket);
		// }

		return 0;
	}

	//!将接收到的响应数据发送到另机
	int CIOPrtclParser::DealRcvLink(bool bFromLocal, Byte* pbyData, int nDataLen)
	{
		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// if (!pDriver)
		// {
		// 	return -1;
		// }

		// //!另机组的请求包本机收到响应则进行转发
		// if (m_bSendPacketIsFromPeer && bFromLocal)
		// {
		// 	//!利用命令管理器将数据转发到另机>>>待实现


		// 	//!恢复数据包标识
		// 	m_bSendPacketIsFromPeer = false;
		// }

		return 0;
	}

	//!转换longlong类型为时间
	std::string CIOPrtclParser::llNum2Time(const long llNum2Time)
	{
		std::string strRet = "";

		// char aTime[128];
		// struct   tm*   t; 
		// long  llTime = llNum2Time/1000LL;
		// int iMsec = int(llNum2Time%llTime);

		// t = ACE_OS::localtime((const time_t*)&llTime);
		// snprintf( aTime,sizeof(aTime),"%d/%d/%d %d:%d:%d.%d",t->tm_year + 1900,t->tm_mon + 1,t->tm_mday
		// 	,t->tm_hour,t->tm_min,t->tm_sec,iMsec );
		// strRet = aTime;

		return strRet;
	}

	//!把串口数据先放到m_pbyInnerBuf中供滑动窗口使用
	void CIOPrtclParser::InnerAddData(Byte* pBuf, int iLen)
	{
		//过长处理
		// if ( m_iInnerLen+iLen>PRTCL_MAX_INNER_LENGTH && iLen<=PRTCL_MAX_INNER_LENGTH )
		// {
		// 	//m_iInnerPos = 0;
		// 	ACE_OS::memcpy( m_pbyInnerBuf, pBuf, iLen );
		// 	m_iInnerLen = iLen;
		// 	return;
		// }

		// if ( iLen>PRTCL_MAX_INNER_LENGTH )
		// {
		// 	ACE_OS::memcpy( m_pbyInnerBuf, pBuf, PRTCL_MAX_INNER_LENGTH );
		// 	m_iInnerLen = PRTCL_MAX_INNER_LENGTH;
		// 	return;
		// }

		// //短处理
		// ACE_OS::memcpy( m_pbyInnerBuf+m_iInnerLen, pBuf, iLen );
		m_iInnerLen += iLen;
	}

	//!读取内部缓冲区中数据
	bool CIOPrtclParser::InnerRead(Byte* pBuf, int iBufLen, int& iReadLen)
	{
		// if( m_iInnerLen<=0 )				//!没数就返回
		// {
		// 	iReadLen = 0;
		// 	return false;
		// }
		// if( m_iInnerLen<=iBufLen )			//!过短处理
		// {
		// 	ACE_OS::memcpy( pBuf, m_pbyInnerBuf, m_iInnerLen );
		// 	iReadLen = m_iInnerLen;
		// 	m_iInnerLen = 0;
		// }
		// else								//!长处理
		// {
		// 	ACE_OS::memcpy( pBuf, m_pbyInnerBuf, iBufLen );
		// 	iReadLen = iBufLen;
		// 	ACE_OS::memcpy( m_pbyInnerBuf, m_pbyInnerBuf+iReadLen, m_iInnerLen-iReadLen );
		// 	m_iInnerLen = m_iInnerLen - iReadLen;
		// }
		return true;
	}
	 int CIOPrtclParser::TimeOut(bool& bSendData, bool& bTrigTimer, int& nTimeOut)
	{
		return 0;
	}
	
	int CIOPrtclParser::OnConnSuccess(std::string pPeerIP)
	{

		return 0;
	}

	int CIOPrtclParser::StopTimer()
	{
		return 0;
	}
}