/*!
 *	\file	IOPrtclParser.cpp
 *
 *	\brief	Э����ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��8��	15:34
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

	//!���캯��
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

	//!��������
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

	//!������֡
	int CIOPrtclParser::BuildRequest(Byte* pbyData, int& nDataLen)
	{
		//!�ж�ͨ����û�б���ֹ
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

	//!������֡
	int CIOPrtclParser::BuildRequest(Byte* pbyData, int&nDataLen, bool& bTrigTimer, int& nTimeOut)
	{
		//!�ж�ͨ����û�б���ֹ
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

	//!�����������
	int CIOPrtclParser::RxDataProc(Byte* pDataBuf, int& nLen, bool& bStopTimer, bool& bSendData)
	{
		// SetRcvProcFlag( NULL, bStopTimer, bSendData );
		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// //!������ݴ���ռ��Ƿ���׼������
		// if (!pDriver || !m_pbyInnerBuf || !m_pbyRevBuffer || !m_pbySndBuffer)
		// {
		// 	return -1;
		// }
		// //!ͨ���Ƿ��ֹ
		// if (pDriver->GetIOPort()->IsDisable())
		// {
		// 	return -2;
		// }

		// //!�������ͨ�ż�������ԭʼ���ݰ�
		// pDriver->WriteCommData(pDataBuf, nLen);

		// //!�����ݷŵ��ڲ�������
		// InnerAddData(pDataBuf, nLen);

		// //!���ڲ�������ִ�л������������ݰ�,һ�ο��ܴ��������ݰ�
		// int nReceived = 0;
		// do 
		// {
		// 	//!��ȡ�����ݳ���
		// 	nReceived = PRTCL_RCV_BUFFER_LENGTH - m_nRevLength;
		// 	if ( 0 >= nReceived || nReceived>PRTCL_RCV_BUFFER_LENGTH )
		// 	{
		// 		m_nRevLength = 0;
		// 		break;
		// 	}

		// 	//!���ڲ�������ȡ������
		// 	if ( InnerRead( m_pbyRevBuffer+m_nRevLength, nReceived, nReceived) && nReceived > 0 )
		// 	{
		// 		m_nRevLength += nReceived;
		// 		while( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH )
		// 		{
		// 			bool bCompleted = false;
		// 			int nPacketLen = 0;
		// 			memset( m_sPacketBuffer, 0, PRTCL_MAX_PACKET_LENGTH );

		// 			//!����ж����������ȡ֡���������һ������������,ǰ�����ݰ�������
		// 			bool bTemp;
		// 			do 
		// 			{
		// 				bTemp = ExtractPacket( m_sPacketBuffer, &nPacketLen );
		// 				if (bTemp)
		// 				{
		// 					//!ʣ�µ������ƶ������ջ������Ŀ�ͷ
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

		// 			//!�ҵ�һ�����������ݰ����д���
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

		//!����ʧ��
		return -1;
	}

	//!�����������
	int CIOPrtclParser::RxDataProc(Byte* pDataBuf, int& nLen)
	{
		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// //!������ݴ���ռ��Ƿ���׼������
		// if (!pDriver || !m_pbyInnerBuf || !m_pbyRevBuffer || !m_pbySndBuffer)
		// {
		// 	return -1;
		// }
		// //!ͨ���Ƿ��ֹ
		// if (pDriver->GetIOPort()->IsDisable())
		// {
		// 	return -2;
		// }

		// //!�������ͨ�ż�������ԭʼ���ݰ�
		// pDriver->WriteCommData(pDataBuf, nLen);

		// //!�����ݷŵ��ڲ�������
		// InnerAddData(pDataBuf, nLen);

		// //!���ڲ�������ִ�л������������ݰ�,һ�ο��ܴ��������ݰ�
		// int nReceived = 0;
		// do 
		// {
		// 	//!��ȡ�����ݳ���
		// 	nReceived = PRTCL_RCV_BUFFER_LENGTH - m_nRevLength;
		// 	if ( 0 >= nReceived || nReceived>PRTCL_RCV_BUFFER_LENGTH )
		// 	{
		// 		m_nRevLength = 0;
		// 		break;
		// 	}

		// 	//!���ڲ�������ȡ������
		// 	if ( InnerRead( m_pbyRevBuffer+m_nRevLength, nReceived, nReceived) && nReceived > 0 )
		// 	{
		// 		m_nRevLength += nReceived;
		// 		while( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH )
		// 		{
		// 			bool bCompleted = false;
		// 			int nPacketLen = 0;
		// 			memset( m_sPacketBuffer, 0, PRTCL_MAX_PACKET_LENGTH );

		// 			//!����ж����������,ȡ֡���������һ������������,ǰ�����ݰ�������
		// 			bool bTemp;
		// 			do 
		// 			{
		// 				bTemp = ExtractPacket( m_sPacketBuffer, &nPacketLen );
		// 				if (bTemp)
		// 				{
		// 					//!ʣ�µ������ƶ������ջ������Ŀ�ͷ
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

		// 			//!�ҵ�һ�����������ݰ����д���
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

		//!����ʧ��
		return -1;
	}

	//!���յ�ͨ����ϰ�
	int CIOPrtclParser::RcvPcketCmd(tIOCtrlPacketCmd* ptPacket)
	{
		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// if (!pDriver)
		// {
		// 	return -1;
		// }

		// if (ptPacket->byType == IOPACKET_TYPE_SENDPACKET)		//!�������ݰ�
		// {
		// 	MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOPrtclParser::RcvPacketCmd �Է����������������ݰ�!" ) ));
		// 	m_bSendPacketIsFromPeer = true;
		// 	memcpy( &m_cmdPacket, ptPacket, sizeof( m_cmdPacket ) );
		// }
		// else if (ptPacket->byType == IOPACKET_TYPE_RECVPACKET)	//!�������ݰ�
		// {
		// 	MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOPrtclParser::RcvPacketCmd �Է����������������ݰ�!" ) ));
		// }
		return 0;
	}

	//!�ӽ��ջ���������ȡһ֡�������ݰ�
	bool CIOPrtclParser::ExtractPacket(Byte* pPacketBuffer, int* pnPacketLength)
	{
		return false;
	}

	//!������ȡ�����������ݰ�
	void CIOPrtclParser::ProcessRcvdPacket(Byte* pPacketBuffer, int nPacketLength, bool bFromLocal, bool& bStopTimer, bool& bSendData)
	{

	}

	//!������ȡ�����������ݰ�
	void CIOPrtclParser::ProcessRcvdPacket(Byte* pPacketBuffer, int nPacketLength, bool bFromLocal)
	{

	}

	//!���ý��մ����еı�ʶ
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
		// 	//! �豸����ģ��״̬
		// 	if ( pDevice->IsSimulate() )
		// 	{
		// 		bStopTimer = false;
		// 		bSendData = false;
		// 	}

		// 	//! �豸Ϊ�ӻ�
		// 	if ( !pDevice->IsHot() )
		// 	{
		// 		//! 
		// 		bStopTimer = false;
		// 		bSendData = false;
		// 	}
		// }
		return 0;
	}

	//!��ָ���豸��ͨ���������֡
	void CIOPrtclParser::MakeChannelDiagFrame(CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen)
	{
		nReturnLen = 0;
	}

	//!�õ�����
	// std::string CIOPrtclParser::GetClassName()
	// {
	// 	return s_ClassName;
	// }

	//!�õ�֡���ݻ�����ָ��
	Byte* CIOPrtclParser::GetPacketBuffer()
	{
		return m_sPacketBuffer;
	}

	//!�õ���С���ݰ�����
	int CIOPrtclParser::GetMinPacketLength()
	{
		return PRTCL_MIN_PACKET_LENGTH;
	}

	//!����16λsumУ��
	
	WORD CIOPrtclParser::Get16SumCode(Byte * Buf, int iWordLen)
	{
		WORD wSum = 0;
		// for(int i=0; i<iWordLen; i++)
		// 	wSum += Buf[i];
		return wSum;
	}

	//!����16λWORD�ĸ��ֽ�ֵ
	Byte CIOPrtclParser::GetWordHigh(WORD wdCheck)
	{
		return Byte(wdCheck/0x100);
	}

	//!����16λWORD�ĵ��ֽ�ֵ
	Byte CIOPrtclParser::GetWordLow(WORD wdCheck)
	{
		return Byte(wdCheck%0x100);
	}

	//!������·���ʳ�ͻ,������͸����
	int CIOPrtclParser::DealLinkClash(CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen)
	{
		//!��������֮���ͨ����·�Ͽ���ֱ�ӷ���

		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// if (!pDriver)
		// {
		// 	return -2;
		// }

		// //!��·����ͻֱ�ӷ���
		// if (!pDriver->IsPortAccessClash())
		// {
		// 	return 1;
		// }

		// //!ֻ���豸ΪHot���豸����ͨ�����״̬
		// if ( !pDevice || !pDevice->IsHot() || !pDevice->IsStatusUnit() )
		// {
		// 	return 2;
		// }
		// MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOPrtclParser::DealLinkClash, ��ͨ��������ݰ������͸����!" ) ));

		// //!��������豸��ͨ����ϱ�ʶ
		// pDevice->SetStatusUnit(false);

		// //!�Ը��豸��ͨ���������֡
		// MakeChannelDiagFrame(pDevice, pbyData, nReturnLen);
		// //!��֡ʧ��
		// if (nReturnLen <= 0)
		// {
		// 	return 3;
		// }

		// //!���������
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

	//!�����յ�����Ӧ���ݷ��͵����
	int CIOPrtclParser::DealRcvLink(bool bFromLocal, Byte* pbyData, int nDataLen)
	{
		// CIODriver* pDriver = (CIODriver*)m_pParam;
		// if (!pDriver)
		// {
		// 	return -1;
		// }

		// //!����������������յ���Ӧ�����ת��
		// if (m_bSendPacketIsFromPeer && bFromLocal)
		// {
		// 	//!�������������������ת�������>>>��ʵ��


		// 	//!�ָ����ݰ���ʶ
		// 	m_bSendPacketIsFromPeer = false;
		// }

		return 0;
	}

	//!ת��longlong����Ϊʱ��
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

	//!�Ѵ��������ȷŵ�m_pbyInnerBuf�й���������ʹ��
	void CIOPrtclParser::InnerAddData(Byte* pBuf, int iLen)
	{
		//��������
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

		// //�̴���
		// ACE_OS::memcpy( m_pbyInnerBuf+m_iInnerLen, pBuf, iLen );
		m_iInnerLen += iLen;
	}

	//!��ȡ�ڲ�������������
	bool CIOPrtclParser::InnerRead(Byte* pBuf, int iBufLen, int& iReadLen)
	{
		// if( m_iInnerLen<=0 )				//!û���ͷ���
		// {
		// 	iReadLen = 0;
		// 	return false;
		// }
		// if( m_iInnerLen<=iBufLen )			//!���̴���
		// {
		// 	ACE_OS::memcpy( pBuf, m_pbyInnerBuf, m_iInnerLen );
		// 	iReadLen = m_iInnerLen;
		// 	m_iInnerLen = 0;
		// }
		// else								//!������
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