/*!
*	\file	NR103Driver.h
*
*	\brief	NR103Э�������ʵ��
*
*	$Date: 15-03-30$
*
*/
#include "NR103Prtcl.h"
#include "NR103Driver.h"
#include "NR103Device.h"

namespace MACS_SCADA_SUD
{
	std::string CNR103Prtcl::s_ClassName = "CNR103Prtcl";

	//���ݽ��ճ�ʱ����
	void NR103RcvOutTimer(void* pParam);

	//����״̬����ʱ��
	void NR103PeriodTimeOut(void* pParam);

	CNR103Prtcl::CNR103Prtcl()
		:m_iHBInternal(10),m_iWQInternal(1200),m_iCheckTPeriod(300),m_nLastAddr(0),m_nLogicalAddr(0)
		,m_nSendDelay(30),m_pRcvOutTimer(NULL),m_pPeriodTimer(NULL),m_nPeriodCount(1),m_nOffDevCount(0)
		,m_nErrPacketNum(0)
	{
		InitPrtclHelper(MIN_FRAME_LEN, MAX_FRAME_LEN, NR103_RCVBUFF, NR103_SENDBUFF, NR103_INNERBUF);
		//������ʱ������
		m_pRcvOutTimer = new CIOTimer(NR103RcvOutTimer, this, OpcUa_True);
		m_pPeriodTimer = new CIOTimer(NR103PeriodTimeOut, this);
		//��ʼ���ڴ滺����
		ACE_OS::memset(m_pbyInnerBuf, 0, NR103_INNERBUF);
		ACE_OS::memset(m_pbyRevBuffer, 0, NR103_RCVBUFF);
	}

	CNR103Prtcl::~CNR103Prtcl()
	{
		if (m_pRcvOutTimer)
		{
			m_pRcvOutTimer->KillTimer();
		}
		if (m_pPeriodTimer)
		{
			m_pPeriodTimer->KillTimer();
		}
	}

	OpcUa_Int32 CNR103Prtcl::OnConnSuccess( std::string pPeerIP )
	{
		OpcUa_Int32 nRes = -1;
		CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
		//�ر����ӳ�ʱ��ʱ��
		if (pDriver && pDriver->GetNetConnTimer())
		{
			nRes = pDriver->GetNetConnTimer()->KillTimer();
			UaString strMsg(UaString("NR103Driver::OnConnSuccess connect IP:%1 successful!").arg(pPeerIP.c_str()));
			pDriver->WriteLogAnyWay(strMsg.toUtf8());
		}
		//!���ӳɹ����豸Уʱ�Ϳ�ʼ����������ʶ
		if (pDriver)
		{
			for (OpcUa_Int32 nIndex = 0; nIndex < pDriver->GetDeviceCount(); nIndex++)
			{
				CNR103Device* pDevice = (CNR103Device*)(pDriver->GetDeviceByIndex(nIndex));
				pDevice->SetCheckTFlag(OpcUa_True);
				pDevice->SetSendHBTag(OpcUa_True);
				pDevice->InitDataNum(0);
			}
		}

		OpcUa_Int32 nReturnLen = 0;
		nRes = BuildRequest(m_pbySndBuffer, nReturnLen);

		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::BuildRequest( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen )
	{
		OpcUa_Int32 nRes = -1;
		//�ж���·�Ƿ񱻽�ֹ,��ֹ��ֱ�ӷ���
		nRes = CIOPrtclParser::BuildRequest(pbyData, nDataLen);
		if (nRes == 0)
		{
			//��������֡
			nRes = BuildRequestFrame(pbyData, nDataLen);
			CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
			if (nDataLen > 0 && pDriver)
			{
				//��¼��־
				pDriver->WriteCommData(m_pbySndBuffer, nDataLen, COMMDATA_SND);
				std::string strFrameData;
				strFrameData = GetCommData(m_pbySndBuffer, nDataLen, COMMDATA_SND);
				
				//��¼�·�������Ϣ
				pDriver->WriteLogAnyWay(strFrameData.c_str());
				
				if (pDriver->m_pIODrvBoard)
				{
					//���ͱ���
					nRes = pDriver->m_pIODrvBoard->Write(pbyData, nDataLen);
					if (0 == nRes)
					{
						UaString ustrMsg(UaString("CNR103Prtcl::BuildRequest Build Frame and send successful!\n"));
						pDriver->WriteLog(ustrMsg.toUtf8());
					}
					else
					{
						UaString ustrMsg(UaString("CNR103Prtcl::BuildRequest Build Frame but send failed!\n"));
						pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
					}
				}
			}
			//�������Ľ��ճ�ʱ��ʱ��
			if (m_pRcvOutTimer && pDriver)
			{
				OpcUa_Int32 nRes = m_pRcvOutTimer->SetTimer(GetRcvTimeOut());
				UaString ustrMsg;
				if (0 == nRes)
				{
					ustrMsg = UaString("CNR103Prtcl::BuildRequest Start RcvTimeOut Timer Successful!");
				}
				else
				{
					ustrMsg = UaString("CNR103Prtcl::BuildRequest Start RcvTimeOut Timer failed!");
				}
				pDriver->WriteLog(ustrMsg.toUtf8());
			}
		}
		else
		{
			UaString ustrMsg(UaString("CNR103Prtcl::BuildRequest failed because the Port has stop work or disable"));
			WriteLog(ustrMsg);
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nLen )
	{
		OpcUa_CharA chMsg[255];

		//!ͨ���������ֹ�ˣ���ֱ�ӷ���0
		CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
		if (!pDriver)
			return -1;		
		if(pDriver->IsDisable())
		{
			pDriver->WriteLog( "CNR103Driver::The Port is disable will not RxDataProc" );
			return 0;
		}
		//!����
		m_mutexDataSync.lock();

		//!�����Ҫͨ�ż��ӣ�����ԭʼ���ݰ�
		pDriver->WriteCommData( pDataBuf, nLen );

		//�����ݷŵ��ڲ�������
		InnerAddData( pDataBuf, nLen );

		//�յ�������
		snprintf(chMsg, sizeof(chMsg), "\n\nCNR103Prtcl::RxDataProc: Receiving new Data, Len:%d ", nLen);
		pDriver->WriteLogAnyWay(chMsg);

		//���ڲ���������ִ�л������ڣ������ݰ���ע��һ�ο��ܻᴦ�������ݰ�
		OpcUa_Boolean bPacketOk = false;
		OpcUa_Int32	nReceived = 0;
		do
		{
			//��ȡ�����ݳ���
			nReceived = PRTCL_RCV_BUFFER_LENGTH - m_nRevLength;
			if ( 0 >= nReceived || nReceived>PRTCL_RCV_BUFFER_LENGTH )
			{
				snprintf( chMsg, sizeof(chMsg), "CNR103Prtcl::RxDataProc: Received Data is overflow, size = %d!!!!!", m_nRevLength );
				pDriver->WriteLogAnyWay(chMsg);
				m_nRevLength = 0;
				break;
			}

			//���ڲ�������ȡ������
			if( InnerRead( m_pbyRevBuffer+m_nRevLength, nReceived, nReceived) && nReceived > 0 )
			{
				m_nRevLength += nReceived;
				while( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH )
				{
					OpcUa_Boolean bCompleted = false;
					OpcUa_Int32 nPacketLen = 0;

					//����һ������һ��
					OpcUa_Boolean bTemp=false;
					do{
						bTemp = ExtractPacket( m_sPacketBuffer, &nPacketLen );
						if( bTemp )
						{
							//ʣ�µ������ƶ������ջ������Ŀ�ͷ
							m_nRevLength = m_nRevLength - nPacketLen;
							if (m_nRevLength< 0)
							{	
								m_nRevLength = 0;
							}
							if ( m_nRevLength>PRTCL_RCV_BUFFER_LENGTH )
							{
								m_nRevLength = PRTCL_RCV_BUFFER_LENGTH;
							}

							if ( m_nRevLength > 0 )
							{
								ACE_OS::memmove( m_pbyRevBuffer, m_pbyRevBuffer+nPacketLen, m_nRevLength );
							}

							bCompleted = true;

							//�ҵ�һ�����ݴ���һ������,��ֹ���ͬʱ����ʱֻ�������һ����
							ProcessRcvdPacket( m_sPacketBuffer, nPacketLen, true );		
							bPacketOk = true;
						}
					}while(bTemp);

					//����ִ�е��������ExtractPacket�����һ���ˣ���������ɹ���񣬶�Ӧ������ѭ��
					break;
				}
			}
		}while( 0<nReceived );

		//!����
		m_mutexDataSync.unlock();

		//���ش�������
		return bPacketOk ? 0 : -1;
	}

	bool CNR103Prtcl::ExtractPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32* pnPacketLength )
	{
		OpcUa_Boolean bACompletePacketRcvd = OpcUa_False;
		CNR103Driver* pDriver = (CNR103Driver*)(m_pParam);
		if (pPacketBuffer && pnPacketLength && pDriver)
		{
			
			pDriver->WriteLogAnyWay("CNR103Prtcl::ExtractPacket ��ʼ����������");
			OpcUa_Int32 nPacketLen = 0;
			OpcUa_Byte* pFrame = m_pbyRevBuffer;
			OpcUa_Int32 nErrorLen = 0;				//�ѽ�����ȷ�ϴ������ݴ�С
			if (pDriver->IsLog())
			{
				std::string strContent = GetCommData(pFrame, m_nRevLength, RCV_FLAG);
				UaString uaContent(UaString("NR103Prtcl:: The RcvBuffer Len:%1, Content:%2").arg(m_nRevLength).arg(strContent.c_str()));
				pDriver->WriteLogAnyWay(uaContent.toUtf8());
			}
			//У��֡�ĺ�����
			while (pFrame + MIN_FRAME_LEN <= m_pbyRevBuffer + m_nRevLength)
			{
				WORD wFFlag = MAKEWORD(pFrame[0],pFrame[1]);
				WORD wSFlag = MAKEWORD(pFrame[6],pFrame[7]);
				OpcUa_Int32 nAPCISize = sizeof(FrameAPCI);
				WORD wReserve = MAKEWORD(pFrame[nAPCISize-2],pFrame[nAPCISize-1]);
				//Ѱ�ұ���ͷ
				if (HEAD_FLAG == wFFlag && HEAD_FLAG == wSFlag && RESERVE_FLAG == wReserve)
				{
					WORD wLenL = MAKEWORD(pFrame[2], pFrame[3]);
					WORD wLenH = MAKEWORD(pFrame[4], pFrame[5]);
					DWORD dwDataLen = MAKEDWORD(wLenL, wLenH);
					DWORD dwFrameLen = dwDataLen + 8;
					if (dwFrameLen >= MIN_FRAME_LEN)
					{
						if (MIN_FRAME_LEN == dwFrameLen)
						{
							//TCP����֡
							nPacketLen = MIN_FRAME_LEN;
						}
						else
						{
							if (dwFrameLen <= MAX_FRAME_LEN)
							{
								//����ASDU֡
								OpcUa_Int32 nASDULen = 0;
								OpcUa_Int32 nASDUBufLen = m_pbyRevBuffer + m_nRevLength - pFrame - nAPCISize;
								OpcUa_Int32 nNext = ExtractASDUFrame(&pFrame[nAPCISize], nASDUBufLen, nASDULen);
								if (F_SUCCESS == nNext)
								{
									nPacketLen = nASDULen + nAPCISize;
									if (pDriver->IsLog())
									{
										std::string strDataFrame = GetCommData(pFrame, nPacketLen, RCV_FLAG);
										UaString ustrMsg(UaString("NR103Prtcl:������������,����:%1,Content:%2").arg(nPacketLen).arg(strDataFrame.c_str()));
										pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
									}
									//��λ��ǰ��·�����Ĵ����Ϊ0
									m_nErrPacketNum = 0;
								}
								else if (F_CONTINUE == nNext)
								{
									pFrame++;
									continue;
								}
								else if (F_BREAK == nNext )
								{
									break;
								}
								else if (F_NOTHINGTD == nNext)
								{
									OpcUa_UInt nDataLen = nASDULen+nAPCISize;
									std::string strDataFrame = GetCommData(pFrame, nASDUBufLen, RCV_FLAG);
									UaString ustrMsg(UaString("NR103Prtcl:������ASDU֡�в�������,��ȷ����:%1,Content:%2").arg(nDataLen).arg(strDataFrame.c_str()));
									pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
									m_nErrPacketNum ++;
									if (m_nErrPacketNum > 2)
									{
										m_nErrPacketNum = 0;
										UaString uaMsg("NR103Prtcl:����2�ν�����ASDU�д���������ð����ݼ�������");
										pDriver->WriteLogAnyWay(uaMsg.toUtf8());
										pFrame++;
										continue;
									}
									else
									{
										//���ݰ����ղ������������ν������������������ݺϲ�����
										break;
									}
									
								}
							}
							else
							{
								//���ݰ����ȹ�����������
								pFrame++;
								continue;
							}
						}
					}
					else
					{
						//���ݰ����Ȳ���
						break;
					}
				}
				else
				{
					//����Ѱ�ұ���ͷ
					pFrame++;
					continue;
				}
				
				//!�������˺Ϸ�֡
				if (nPacketLen >0)
				{
					if (nPacketLen <= MAX_FRAME_LEN)
					{
						ACE_OS::memset(pPacketBuffer, 0, MAX_FRAME_LEN);
						//�����ɹ����ϸ�֡�ŵ�������
						ACE_OS::memcpy(pPacketBuffer, pFrame, nPacketLen);
						*pnPacketLength = nPacketLen;
						bACompletePacketRcvd = OpcUa_True;
						//�����������֡
						std::string strPacket = GetCommData(pFrame, nPacketLen, RCV_FLAG);
						UaString ustrMsg(UaString("CNR103Prtcl::ExtractPacket receive correct Frame len: %1").arg(nPacketLen));
						pDriver->WriteLog(ustrMsg.toUtf8());
					}
					else
					{
						//��Ȼ�����ɹ������ݳ�������������½���
						pFrame++;
						continue;
					}
				}
				//����ִ�е���˵���ѽ����ɹ������ݳ��Ȳ����������ѭ��
				break;
			}

			//ȥ������ı���
			nErrorLen = pFrame - m_pbyRevBuffer;
			if (nErrorLen > 0)
			{
				std::string strErrPacket = GetCommData(m_pbyRevBuffer, nErrorLen, RCV_FLAG);
				OpcUa_CharA pchMsg[1024];
				snprintf(pchMsg,1024,"CNR103Prtcl::ExtractPacket ȥ��%d�ֽڴ�������:%s", nErrorLen, strErrPacket.c_str());
				pDriver->WriteLogAnyWay(pchMsg);
				//��С���������ݳ���
				m_nRevLength -= nErrorLen;
				if (m_nRevLength > 0)
				{
					ACE_OS::memcpy(m_pbyRevBuffer, m_pbyRevBuffer+nErrorLen, m_nRevLength);
				}
				else
				{
					m_nRevLength = 0;
				}
			}
		}
		return bACompletePacketRcvd;
	}

	void CNR103Prtcl::ProcessRcvdPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32 nPacketLength, bool bFromLocal )
	{
		if (pPacketBuffer && nPacketLength >= MIN_FRAME_LEN)
		{
			OpcUa_CharA chMsg[256];
			CNR103Driver* pDriver = (CNR103Driver*)m_pParam;

			if (pDriver)
			{
				//�ر�����ʱ��ʱ��
				if (m_pRcvOutTimer)
				{
					OpcUa_Int32 nRes = m_pRcvOutTimer->KillTimer();
					UaString ustrMsg("NR103Prtcl::ProcessData and kill the RctTimeOut Timer");
					pDriver->WriteLog(ustrMsg.toUtf8());
				}

				//�õ���Ӧ�豸����
				FrameAPCI* pAPCI = (FrameAPCI*)(pPacketBuffer);
				WORD wDevAddr = pAPCI->wSourceDevAddr;
				CNR103Device* pDevice = (CNR103Device*)pDriver->GetDeviceByAddr(wDevAddr);
				if (pDevice)
				{
					//��λ�豸�ķ��ͼ���
					pDevice->m_byTxTimes = 0;

					//�����豸����
					if (bFromLocal && !pDevice->IsOnLine())
					{
						pDevice->SetOnLine(DEVICE_ONLINE);
						snprintf(chMsg,sizeof(chMsg),"�豸:%s�ɹ�����",pDevice->GetName().c_str());
						pDriver->WriteLogAnyWay(chMsg);
					}
					if (MIN_FRAME_LEN == nPacketLength)
					{
						//�豸�յ�������ϰ�
						OpcUa_CharA chMsg[256];
						snprintf(chMsg, 256, "NR103Prtcl: �յ��豸:%s������֡", pDevice->GetName().c_str());
						pDriver->WriteLogAnyWay(chMsg);
					}
					else
					{
						OpcUa_UInt32 nRestByteCount = nPacketLength - sizeof(FrameAPCI);
						if (nRestByteCount >= MIN_103ASDU_LEN)
						{
							OpcUa_Byte* pASDUFrame = &pPacketBuffer[sizeof(FrameAPCI)];
							ASDU_HEAD* pASDUHead = (ASDU_HEAD*)pASDUFrame;
							OpcUa_Byte byTYPFlag = pASDUHead->byType;
							OpcUa_Byte byCOT = pASDUHead->byCOT;
							OpcUa_Byte byInfoSum = (pASDUHead->byVSQ & 0x7F);
							OpcUa_Byte byFunType = pASDUHead->byFun;
							OpcUa_Byte byInfoNo = pASDUHead->byInf;
							OpcUa_Int32 nRes = -1;
							switch(byTYPFlag)
							{
							case TYP_ASDU1:
								nRes = pDevice->ProcessASDU1FRest(pASDUFrame, nRestByteCount);
								break;
							case TYP_COMM_DATA:
								//����ASDU10ͨ�÷�������
								nRes = pDevice->ProcessASDU10Data(pASDUFrame, nRestByteCount);
								break;
							case TYP_IDENTIFIER:
								//����ASDU5
								nRes = pDevice->ProcessASDU5Data(pASDUFrame, nRestByteCount);
								break;
							case TYP_TIME_SYN:
								//����ASDU6		
								nRes = pDevice->ProcessASDU6Data(pASDUFrame, nRestByteCount);
								break;
							case TYP_DISORDER_DATA:
								nRes = pDevice->ProcessASDU23Data(pASDUFrame, nRestByteCount);
								break;
							default:
								{
									UaString ustrMsg(UaString("NR103Prtcl:�豸:%1�յ��ݲ�����ASDU:%d����")
										.arg(pDevice->GetName().c_str()).arg(byTYPFlag));
									pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
								}
								break;
							}		
						}
					}
				}
				else
				{
					UaString ustrMsg(UaString("NR103Prtcl:���յ�����û���ҵ���Ӧ�豸����,�豸��ַ:%1").arg(wDevAddr));
					pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
				}
				//�����µ�����
				ACE_Time_Value tv(m_nSendDelay/1000, (m_nSendDelay%1000)*1000);
				ACE_OS::sleep(tv);
				OpcUa_Int32 nReturnLen = 0;
				BuildRequest(m_pbySndBuffer, nReturnLen);
			}
		}
	}
	

	std::string CNR103Prtcl::GetClassName()
	{
		return s_ClassName;
	}

	void CNR103Prtcl::InitConfig(std::string strDrvName)
	{
		if (!strDrvName.empty())
		{
			std::string strFilePath = CIODriver::GetFEPRunDir() + MACS_DRIVER_FILENAME;
			m_iConnTimeOut = (OpcUa_UInt32)GetPrivateProfileInt(strDrvName.c_str(), "ConnTimeOut", 3000, strFilePath.c_str());
			OpcUa_Int32 nTempValue = 0;
			if (m_iConnTimeOut < 1000)
			{
				m_iConnTimeOut = 3000;
			}
			m_iMaxReConn = (OpcUa_UInt32)GetPrivateProfileInt(strDrvName.c_str(), "MaxReConn", 3, strFilePath.c_str());
			if (m_iMaxReConn < 3)
			{
				m_iMaxReConn = 3;
			}
			m_iRcvTimeOut = (OpcUa_UInt32)GetPrivateProfileInt(strDrvName.c_str(),"RcvTimeOut", 3000, strFilePath.c_str());
			if (m_iRcvTimeOut < 3000)
			{
				m_iRcvTimeOut = 3000;
			}
			m_iMaxOffCount = (OpcUa_UInt32)GetPrivateProfileInt(strDrvName.c_str(), "MaxOfflineCount", 5, strFilePath.c_str());
			if (m_iMaxOffCount < 5)
			{
				m_iMaxOffCount = 5;
			}
			m_nSendDelay = (OpcUa_Int32)GetPrivateProfileInt(strDrvName.c_str(), "SendDelay", 30, strFilePath.c_str());
			if (m_nSendDelay < 5 || m_nSendDelay > 1000)
			{
				m_nSendDelay = 30;
			}
			nTempValue = (OpcUa_UInt32)GetPrivateProfileInt(strDrvName.c_str(), "GeneralInquire", 20, strFilePath.c_str());
			if (nTempValue < 10)
			{
				nTempValue = 20;
			}
			m_iWQInternal = nTempValue *  60;

			nTempValue = (OpcUa_UInt32)GetPrivateProfileInt(strDrvName.c_str(), "CheckTPeriod", 10, strFilePath.c_str());
			if (nTempValue < 10)
			{
				nTempValue = 10;
			}
			m_iCheckTPeriod = nTempValue*60;

			m_iHBInternal = (OpcUa_UInt32)GetPrivateProfileInt(strDrvName.c_str(), "HartBeatInternal", 10, strFilePath.c_str());
			if (m_iHBInternal < 10)
			{
				m_iHBInternal = 10;
			}

			//�������ڴ���ʱ��
			if (m_pPeriodTimer)
			{
				m_pPeriodTimer->SetTimer(PERIOD_TIME);
			}
		}
	}

	OpcUa_UInt32 CNR103Prtcl::GetConnTimeOut()
	{
		return m_iConnTimeOut;
	}

	OpcUa_UInt32 CNR103Prtcl::GetMaxReConn()
	{
		return m_iMaxReConn;
	}

	void CNR103Prtcl::WriteLog( UaString ustrMsg, OpcUa_Boolean isWriteAnyWay /*= OpcUa_False*/ )
	{
		CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
		if (pDriver)
		{
			if (isWriteAnyWay)
			{
				pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
			}
			else
			{
				pDriver->WriteLog(ustrMsg.toUtf8());
			}
		}
	}

	OpcUa_Int32 CNR103Prtcl::BuildRequestFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen )
	{
		OpcUa_Int32 nRes = -1;
		if (pbyData != NULL)
		{
			CNR103Driver* pDriver = (CNR103Driver*)m_pParam;

			//!����ң��,ң��֡
			nRes = BuildCtrlRequest(pbyData, nDataLen);
			if (0 == nRes && nDataLen > 0)
			{
				WORD wDevAddr = MAKEWORD(pbyData[14], pbyData[15]);
				OpcUa_Int32 nDevLogic = 0;
				if (pDriver && pDriver->GetDevIndexByAddr(wDevAddr, nDevLogic))
				{
					//������ң���豸����������ѯ�����Ա������Ը��豸��������
					m_nLogicalAddr = nDevLogic;
					m_nLastAddr = m_nLogicalAddr;
				}
				return nRes;
			}
			//!��ѯ��·�µ��豸������֡
			CNR103Device* pDevice = (CNR103Device*)pDriver->GetDeviceByIndex(m_nLogicalAddr);
			if (pDevice)
			{
				//!����ģ��״̬��������֡
				if (!pDevice->IsSimulate())
				{
					m_nLastAddr = m_nLogicalAddr;
					//!�����豸����֡
					nRes = BuildFrame(pbyData, nDataLen, pDevice);
					if (nDataLen > 0)
					{
						//!�����豸�����������
						pDevice->m_byTxTimes++;
					}

					//!�л��豸��ѯ����
					m_nLogicalAddr++;
					if (m_nLogicalAddr >= pDriver->GetDeviceCount())
					{
						m_nLogicalAddr = 0;
					}
				} 
				else
				{
					UaString ustrMsg(UaString("CNR103Prtcl::BuildRequestFrame The Device: %1 is simulating now will not Send Request!").arg(pDevice->GetName().c_str()));
					pDriver->WriteLog(ustrMsg.toUtf8());
				}
			}
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::BuildCtrlRequest( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen )
	{
		OpcUa_Int32 nRes = 0;
		CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
		//ֻ������·�ŷ���ң������
		if (pbyData && pDriver && pDriver->GetHot())
		{
			OpcUa_CharA pchBuf[512];
			OpcUa_Int32 nBufLen = 512;
			nRes = pDriver->m_IORemoteCtrl.DeQueue(pchBuf, nBufLen);
			if (0==nRes)
			{
				//ȡ��ң������֡
				ACE_OS::memset(&m_tRemoteCtrlCmd,0,sizeof(m_tRemoteCtrlCmd));
				memcpy((OpcUa_Byte*)(&m_tRemoteCtrlCmd), pchBuf, sizeof(m_tRemoteCtrlCmd));
				std::string strDevName = m_tRemoteCtrlCmd.pchUnitName;
				CNR103Device* pDevice = dynamic_cast<CNR103Device*>(pDriver->GetDeviceByName(strDevName));
				if (pDevice)
				{
					//�豸���ʹ�������
					pDevice->m_byTxTimes ++;
					if (!pDevice->IsOnLine())
					{
						OpcUa_CharA chMsg[512];
						snprintf(chMsg,512,"NR103Prtcl:�豸�����·�ң������ʧ��,Device:%s", pDevice->GetName().c_str());
						pDriver->WriteLogAnyWay(chMsg);
						nRes= 0;
						nDataLen = 0;
					}
					else
					{
						if (m_tRemoteCtrlCmd.byUnitType == NR103_DOSB)
						{
							//�ж�ң��ֵ�Ƿ���ȷ
							OpcUa_Byte byValue = m_tRemoteCtrlCmd.byData[0];
							if (0 == byValue || 1 == byValue)
							{
								//�����·����У��ң������ѡ���ǩֵ
								OpcUa_Byte byInit = 0;
								pDevice->SetVal(NR103_DICB, m_tRemoteCtrlCmd.lTagAddress, &byInit, 1, 0, OpcUa_True);
								pDevice->SetVal(NR103_DOSB, m_tRemoteCtrlCmd.lTagAddress, &m_tRemoteCtrlCmd.byData[0], 1, 0, OpcUa_True);
								//��ȷ��д��Ŀ
								nDataLen = pDevice->BuildASDU10DOFrame(pbyData, m_tRemoteCtrlCmd.lTagAddress, INF_WRITE_WITH_CONFIRM, m_tRemoteCtrlCmd.byData[0]);
								if (nDataLen > 0)
								{
									UaString ustrMsg(UaString("NR103Prtcl:�豸%1����ң��ѡ������").arg(pDevice->GetName().c_str()));
									pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
								}
							}
							else
							{
								UaString ustrMsg(UaString("NR103Prtcl:�豸:%1�·�ң��ѡ������ֵ:%2")
									.arg(pDevice->GetName().c_str()).arg(byValue));
								pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
							}
						}
						else if (m_tRemoteCtrlCmd.byUnitType == NR103_DOB)
						{
							//�ж�ң��ֵ�Ƿ���ȷ
							OpcUa_Byte byValue = m_tRemoteCtrlCmd.byData[0];
							if (0 == byValue || 1 == byValue)
							{
								//�����·����У��ң������ѡ���ǩֵ
								OpcUa_Byte byInit = 0;
								pDevice->SetVal(NR103_DICB, m_tRemoteCtrlCmd.lTagAddress, &byInit, 1, 0, OpcUa_True);
								pDevice->SetVal(NR103_DOB, m_tRemoteCtrlCmd.lTagAddress, &m_tRemoteCtrlCmd.byData[0], 1, 0, OpcUa_True);
								//��ִ��д��Ŀ
								nDataLen = pDevice->BuildASDU10DOFrame(pbyData, m_tRemoteCtrlCmd.lTagAddress, INF_WRITE_WITH_EXCUTE, m_tRemoteCtrlCmd.byData[0]);
								if (nDataLen > 0)
								{
									UaString ustrMsg(UaString("NR103Prtcl:�豸%1����ң��ִ������").arg(pDevice->GetName().c_str()));
									pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
								}
							}
							else
							{
								UaString ustrMsg(UaString("NR103Prtcl:�豸:%1�·�ң��ִ������ֵ:%2")
									.arg(pDevice->GetName().c_str()).arg(byValue));
								pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
							}
						}
						else if (m_tRemoteCtrlCmd.byUnitType == NR103_RESET)
						{
							OpcUa_Byte byValue = m_tRemoteCtrlCmd.byData[0];
							//��ʼ���źŸ�������
							OpcUa_Byte byInit = 0;
							pDevice->SetVal(NR103_RESETCB, m_tRemoteCtrlCmd.lTagAddress, &byInit, 1, 0, OpcUa_True);
							pDevice->SetVal(NR103_RESET, m_tRemoteCtrlCmd.lTagAddress, &byValue, 1, 0, OpcUa_True);
							//�źŸ�������
							nDataLen = pDevice->BuildResetASDU20(pbyData, m_tRemoteCtrlCmd.lTagAddress);
							if (nDataLen > 0)
							{
								UaString ustrMsg(UaString("NR103Prtcl:�豸%1����ASDU20Զ����������").arg(pDevice->GetName().c_str()));
								pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
							}
						}
						else if (m_tRemoteCtrlCmd.byUnitType == NR103_RDSFV)
						{
							OpcUa_Byte byValue = m_tRemoteCtrlCmd.byData[0];
							pDevice->SetVal(NR103_RDSFV, m_tRemoteCtrlCmd.lTagAddress, &byValue, 1, 0, OpcUa_True);
							//�ٻ���ֵ��
							nDataLen = pDevice->BuildReadSFrame(pbyData, m_tRemoteCtrlCmd.lTagAddress);
							if (nDataLen > 0)
							{
								UaString ustrMsg(UaString("NR103Prtcl:�豸%1���ɶ���ֵ������").arg(pDevice->GetName().c_str()));
								pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
							}
						}
						//�ж��Ƿ��������֡
						if (nDataLen > 0)
						{
							nRes = 0;
						}
					}
				}
			}
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::BuildFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen, CNR103Device* pDevice )
	{
		OpcUa_Int32 nRes = -1;
		CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
		if (pbyData && pDriver && pDevice)
		{
			//!����Уʱ֡
			if (pDevice->IsNeedCheckT() && pDevice->IsHot())
			{
				nRes = pDevice->BuildCTFrame(pbyData, nDataLen);
				if (0 == nRes)
				{
					UaString usStrMsg(UaString("NR103Prtcl: �豸%1����Уʱ֡").arg(pDevice->GetName().c_str()));
					pDriver->WriteLogAnyWay(usStrMsg.toUtf8());
				}
				return nRes;
			}

			//!��������֡(������·Ϊ��FEP��������֡���жϺ��豸ͨѶ״̬)
			if (pDevice->IsNeedHeartB())
			{
				nRes = pDevice->BuildHBFrame(pbyData, nDataLen);
				pDevice->SetSendHBTag(OpcUa_False);
				if (0 == nRes)
				{
					UaString usStrMsg(UaString("NR103Prtcl: �豸%1��������֡").arg(pDevice->GetName().c_str()));
					pDriver->WriteLogAnyWay(usStrMsg.toUtf8());
				}
				return nRes;
			}

			//!����ASUD7���ٻ�֡
			if (pDevice->IsNeedASUD7() && pDevice->IsHot())
			{
				nRes = pDevice->BuildASDU7(pbyData, nDataLen);
				pDevice->SetASDU7(OpcUa_False);
				if (0 == nRes)
				{
					UaString usStrMsg(UaString("NR103Prtcl: �豸%1�������ٻ�֡").arg(pDevice->GetName().c_str()));
					pDriver->WriteLogAnyWay(usStrMsg.toUtf8());
				}
				return nRes;
			}

			//!����ASDU21���ٻ�֡
			if (pDevice->IsNeedASUD21() && pDevice->IsHot())
			{
				nRes = pDevice->BuildASDU21(pbyData, nDataLen);
				pDevice->SetASDU21WQ(OpcUa_False);
				if (0 == nRes)
				{
					UaString usStrMsg(UaString("NR103Prtcl: �豸%1����ASDU21�ܲ�ѯ֡").arg(pDevice->GetName().c_str()));
					pDriver->WriteLogAnyWay(usStrMsg.toUtf8());
				}
				return nRes;
			}
		}
		return nRes;
	}

	std::string CNR103Prtcl::GetCommData( OpcUa_Byte* pFrame, OpcUa_Int32 nPacketLen, OpcUa_Int32 byRcvOrSnd )
	{
		std::string szData = "";
		if ( byRcvOrSnd == 1 )
		{
			szData = "SND: ";
		}
		else
		{
			szData = "RCV: ";
		}

		char pchByte[4];
		//! ��ȡ������ʾ��ʽ
		for ( int i=0; i < nPacketLen; i++ )
		{
			sprintf( pchByte, "%02X ", pFrame[i] );
			szData += pchByte;
		}
		return szData;
	}

	//����ASDU֡
	OpcUa_Int32 CNR103Prtcl::ExtractASDUFrame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen )
	{
		OpcUa_Int32 nRes = F_NOTHINGTD;
		if (pFrame && nlen > 0)
		{
			OpcUa_Byte byFType = pFrame[0];
			OpcUa_Byte byVSQ = pFrame[1];
			OpcUa_Byte byCOT = pFrame[2];
			OpcUa_Byte byFun = pFrame[4];
			OpcUa_Byte byInf = pFrame[5];
			//�жϳ���
			if (nlen < MIN_103ASDU_LEN)
			{
				nRes = F_BREAK;
				return nRes;
			}
			//�ж�ASDU����
			if (byFType != TYP_COMM_DATA && byFType != TYP_TIME_SYN && byFType != TYP_IDENTIFIER
				&& byFType != TYP_COMM_IDENT && byFType != TYP_DISORDER_DATA && byFType != TYP_STATUS_READY
				&& byFType != TYP_STATUS_TRANS && byFType != TYP_TRANS_END && byFType != TYP_ASDU1)
			{
				nRes = F_CONTINUE;
				UaString ustrMsg(UaString("CNR103Prtcl::ExtractASDUFrame unknow ASDU: %1").arg(byFType));
				CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
				pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
				return nRes;
			}
			//�жϴ���ԭ��
			if ( byCOT != COT_AUTO && byCOT != COT_CYC && byCOT != COT_FCB && byCOT != COT_CU
				&& byCOT != COT_START && byCOT != COT_RESTART && byCOT != COT_POWERON && byCOT != COT_TEST
				&& byCOT != COT_TIME_SYN && byCOT != COT_GENERAL_QUERY && byCOT != COT_GENERAL_QUERY_END && byCOT != COT_GENERAL_CMD_CONFIRM
				&& byCOT != COT_GENERAL_CMD_DENY && byCOT != COT_DISORDER_DATA_TRANS && byCOT != COT_COMM_WRITE_CMD && byCOT != COT_COMM_READ_CMD
				&& byCOT != COT_COMM_WRITE_CMD_CONFIRM && byCOT != COT_COMM_WRITE_CMD_DENY && byCOT != COT_COMM_READ_CMD_VALID && byCOT != COT_COMM_READ_CMD_INVALID
				&& byCOT != COT_COMM_WRITE_CONFIRM )
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			if (TYP_ASDU1 == byFType)
			{
				nRes = ExtractASDU1Frame(pFrame, nlen, nASDULen);
			}
			if (TYP_IDENTIFIER == byFType)
			{
				nRes = ExtractASDU5Frame(pFrame, nlen, nASDULen);
			}
			else if (TYP_TIME_SYN == byFType)
			{
				nRes = ExtractASDU6Frame(pFrame, nlen, nASDULen);
			}
			else if (TYP_COMM_DATA == byFType)
			{
				nRes = ExtracktASDU10Frame(pFrame, nlen, nASDULen);
			}
			else if (TYP_DISORDER_DATA == byFType)
			{
				nRes = ExtractASDU23Frame(pFrame, nlen, nASDULen);
			}
			else if (TYP_STATUS_READY == byFType)
			{
				nRes = ExtractASDU28Frame(pFrame, nlen, nASDULen);
			}
			else if (TYP_STATUS_TRANS == byFType)
			{
				nRes = ExtractASDU29Frame(pFrame, nlen, nASDULen);
			}
			else if (TYP_TRANS_END == byFType)
			{
				nRes = ExtractASDU31Frame(pFrame, nlen, nASDULen);
			}
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::ExtractASDU5Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen )
	{
		OpcUa_Int32 nRes = F_NOTHINGTD;
		if (pFrame && nlen > 0)
		{
			if (VSQ_MOST != pFrame[1])
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			if (FUN_GLB != pFrame[4])
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			if (INF_TIME_SYN_OR_QUERY_BEGIN != pFrame[5])
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			if (nlen < (sizeof(ASDU5Frame)-sizeof(FrameAPCI)))
			{
				nRes = F_BREAK;
				return nRes;
			}
			//�����ɹ�
			nASDULen = (sizeof(ASDU5Frame)-sizeof(FrameAPCI));
			nRes = F_SUCCESS;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::ExtractASDU6Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen )
	{
		OpcUa_Int32 nRes = F_NOTHINGTD;
		if (pFrame && nlen > 0)
		{
			//VSQ
			if (VSQ_MOST != pFrame[1])
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			//Fun
			if (FUN_GLB != pFrame[4])
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			//Inf
			if (INF_TIME_SYN_OR_QUERY_BEGIN != pFrame[5])
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			if (nlen < (sizeof(ASDU6Frame)-sizeof(FrameAPCI)))
			{
				nRes = F_BREAK;
				return nRes;
			}
			//�����ɹ�
			nASDULen = (sizeof(ASDU6Frame)-sizeof(FrameAPCI));
			nRes = F_SUCCESS;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::ExtracktASDU10Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen )
	{
		OpcUa_Int32 nRes = F_NOTHINGTD;
		if (pFrame && nlen > 0)
		{
			//VSQ
			if (VSQ_MOST != pFrame[1])
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			//min len
			if (nlen < MIN_IEC103_COMM_LEN)
			{
				nRes = F_BREAK;
				return nRes;
			}
			//Fun
			if (pFrame[4] != FUN_COMM_SORT && pFrame[4] != FUN_GLB)
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			//��ʼ�������
			ASDU10_HEAD* pHead = (ASDU10_HEAD*)pFrame;
			if (pHead->byNGD > 0)
			{
				OpcUa_Int32 nGIDIndex = 0;
				OpcUa_UInt32 nByteCount = sizeof(ASDU10_HEAD);	//ASDU����
				OpcUa_UInt32 nRestByteCount = nlen - sizeof(ASDU10_HEAD);
				OpcUa_UInt32 nEGIDSize = 0;
				OpcUa_Byte* pGDPos = pFrame + sizeof(ASDU10_HEAD);
				GDATAPointHead* pDGHead = (GDATAPointHead*)pGDPos;
				for (; nGIDIndex < pHead->byNGD; nGIDIndex++)
				{
					if (pDGHead->sGDD.byDataType != GDD_DT0_NODATA && 
						(pDGHead->sGDD.byDataSize == 0 || pDGHead->sGDD.byDataNum == 0))
					{
						UaString ustrMsg(UaString("NR103Prtcl:ExtracktASDU10Frame�������������ݳ�������,�����ϴ�GID:%1,ʵ�ʽ�����%2").arg(pHead->byNGD).arg(nGIDIndex));
						CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
						if (pDriver)
						{
							pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
						}
						break;
					}

					if (pDGHead->sGDD.byDataType == GDD_DT0_NODATA)
					{
						if (nRestByteCount >= sizeof(GDATAPointNoData))
						{
							nRestByteCount -= sizeof(GDATAPointNoData);
							nByteCount += sizeof(GDATAPointNoData);
							if (nByteCount > MAX_NR103_FRAM_LEN)
							{
								break;
							}
							//������һ���ݵ�
							pGDPos += sizeof(GDATAPointNoData);
							pDGHead = (GDATAPointHead*)pGDPos;
						}
						else
						{
							//�����Ȳ���
							break;
						}
					}
					else
					{
						nEGIDSize = (pDGHead->sGDD.byDataSize)*(pDGHead->sGDD.byDataNum);
						if (nRestByteCount >= sizeof(GDATAPointHead)+nEGIDSize)
						{
							nRestByteCount -= sizeof(GDATAPointHead)+nEGIDSize;
							nByteCount += sizeof(GDATAPointHead)+nEGIDSize;
							if (nByteCount > MAX_NR103_FRAM_LEN)
							{
								break;
							}
							//������һ���ݵ�
							pGDPos += (sizeof(GDATAPointHead)+nEGIDSize);
							pDGHead = (GDATAPointHead*)pGDPos;
						}
						else
						{
							//�����Ȳ���
							UaString ustrMsg(UaString("NR103Prtcl:ExtracktASDU10Frame�������������ݳ��Ȳ���,�����ϴ�GID:%1,ʵ�ʽ�����%2").arg(pHead->byNGD).arg(nGIDIndex));
							CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
							if (pDriver)
							{
								pDriver->WriteLog(ustrMsg.toUtf8());
							}
							break;
						}
					}
				}
				//���������
				if (nByteCount > MAX_NR103_FRAM_LEN)
				{
					nRes = F_CONTINUE;
					return nRes;
				}
				//ÿ��GID��������
				if (nGIDIndex == pHead->byNGD)
				{
					nASDULen = nByteCount;
					nRes = F_SUCCESS;
				}
				else if (nGIDIndex > 0 && nGIDIndex < pHead->byNGD)
				{
					nASDULen = nByteCount;
				}
			}
			else
			{
				//������
				nASDULen = sizeof(ASDU10_HEAD);
				nRes = F_SUCCESS;
			}
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::ExtractASDU23Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen )
	{
		OpcUa_Int32 nRes = F_NOTHINGTD;
		if (pFrame && nlen > 0)
		{
			if (nlen < MIN_103ASDU_LEN)
			{
				nRes = F_BREAK;
				return nRes;
			}
			OpcUa_Byte byCOT = pFrame[2];
			if (byCOT != COT_GENERAL_QUERY && byCOT != COT_DISORDER_DATA_TRANS)
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			OpcUa_Byte byFun = pFrame[4];
			if (byFun != FUN_GLB && byFun != FUN_NR103DISORDER)
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			OpcUa_Byte byCount = pFrame[1];
			if (byCount == 0)
			{
				nASDULen = sizeof(ASDU_HEAD);
				nRes = F_SUCCESS;
			}
			else
			{
				OpcUa_UInt32 nValidDataLen = sizeof(ASDU_HEAD)+(byCount*sizeof(ASDU23DataSet));
				if (nValidDataLen > nlen)
				{
					nRes = F_BREAK;
					return nRes;
				}
				//�����ɹ�
				nASDULen = nValidDataLen;
				nRes = F_SUCCESS;
			}
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::ExtractASDU28Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen )
	{
		OpcUa_Int32 nRes = F_NOTHINGTD;
		if (pFrame && nlen > 0)
		{
			OpcUa_Byte byVSQ = pFrame[1];
			OpcUa_Byte byCOT = pFrame[2];
			OpcUa_Byte byFUN = pFrame[4];
			if (VSQ_MOST != byVSQ || COT_DISORDER_DATA_TRANS != byCOT || FUN_GLB != byFUN)
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			//��鳤��
			OpcUa_UInt32 nValidDataLen = sizeof(ASDU28);
			if (nValidDataLen > nlen)
			{
				nRes = F_BREAK;
				return nRes;
			}
			//�����ɹ�
			nASDULen = nValidDataLen;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::ExtractASDU29Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen )
	{
		OpcUa_Int32 nRes = F_NOTHINGTD;
		if (pFrame && nlen > 0)
		{
			OpcUa_Byte byVSQ = pFrame[1];
			OpcUa_Byte byCOT = pFrame[2];
			OpcUa_Byte byFUN = pFrame[4];
			if (VSQ_MOST != byVSQ || COT_DISORDER_DATA_TRANS != byCOT || FUN_GLB != byFUN)
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			//��鳤��
			if (sizeof(ASDU29Head) > nlen)
			{
				nRes = F_BREAK;
				return nRes;
			}
			ASDU29Head* pHead = (ASDU29Head*)pFrame;
			OpcUa_Byte byCount = pHead->NOT;
			//������ݳ���
			OpcUa_UInt32 nValidDataLen = sizeof(ASDU29Head) + byCount*sizeof(ASDU29Head);
			if (nValidDataLen > nlen)
			{
				nRes = F_BREAK;
				return nRes;
			}
			//�����ɹ�
			nASDULen = nValidDataLen;
			nRes = F_SUCCESS;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::ExtractASDU31Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen )
	{
		OpcUa_Int32 nRes = F_NOTHINGTD;
		if (pFrame && nlen > 0)
		{
			OpcUa_Byte byVSQ = pFrame[1];
			OpcUa_Byte byCOT = pFrame[2];
			OpcUa_Byte byFUN = pFrame[4];
			if (VSQ_MOST != byVSQ || COT_DISORDER_DATA_TRANS != byCOT || FUN_GLB != byFUN)
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			OpcUa_UInt32 nValidPacketLen = sizeof(ASDU31);
			if (nValidPacketLen > nlen)
			{
				nRes = F_BREAK;
				return nRes;
			}
			//�����ɹ�
			nASDULen = nValidPacketLen;
			nRes = F_SUCCESS;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::ExtractASDU1Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen )
	{
		OpcUa_Int32 nRes = F_NOTHINGTD;
		if (pFrame && nlen > 0)
		{
			OpcUa_Byte byVSQ = pFrame[1];
			OpcUa_Byte byCOT = pFrame[2];
			if (byVSQ != VSQ_MOST || (byCOT != COT_GENERAL_CMD_CONFIRM && byCOT != COT_GENERAL_CMD_DENY))
			{
				nRes = F_CONTINUE;
				return nRes;
			}
			OpcUa_Int32 nValidPacketLen = sizeof(ASDU1_REST);
			if (nValidPacketLen > nlen)
			{
				nRes = F_BREAK;
				return nRes;
			}
			//�����ɹ�
			nASDULen = nValidPacketLen;
			nRes = F_SUCCESS;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Prtcl::GetLastDevIndex()
	{
		return m_nLastAddr;
	}

	OpcUa_Int32 CNR103Prtcl::GetRcvTimeOut()
	{
		return m_iRcvTimeOut;
	}

	OpcUa_Int32 CNR103Prtcl::GetMaxOffLineCount()
	{
		return m_iMaxOffCount;
	}

	void NR103RcvOutTimer(void* pParam)
	{
		CNR103Prtcl* pPrtcl = (CNR103Prtcl*)pParam;
		if (pPrtcl)
		{
			//ֹͣ��ʱ��
			OpcUa_Int32 nRes = pPrtcl->m_pRcvOutTimer->KillTimer();
			if (0 == nRes)
			{
				//�õ��ϴη��������豸����
				OpcUa_Int32 nLastDevAddr = pPrtcl->GetLastDevIndex();			
				CNR103Driver* pDriver = (CNR103Driver*)(pPrtcl->GetParam());
				if (pDriver)
				{
					UaString ustrMsg("NR103Prtcl:: RctTimeOut Timer Start Work");
					pDriver->WriteLog(ustrMsg.toUtf8());

					CNR103Device* pDevice = (CNR103Device*)pDriver->GetDeviceByIndex(nLastDevAddr);
					//�ж��豸�Ƿ�����
					if (pDevice && (pDevice->m_byTxTimes > pPrtcl->GetMaxOffLineCount()))
					{
						pDevice->SetOnLine(DEVICE_OFFLINE);
						//�ָ��豸���ͼ���
						pDevice->m_byTxTimes = 0;
						//�����豸��������
						pPrtcl->m_nOffDevCount++;
						UaString ustrMsg(UaString("NR103::RcvTimeOut �豸:%1 ����").arg(pDevice->GetName().c_str()));
						pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
					}

					if (pPrtcl->m_nOffDevCount > pDriver->GetDeviceCount())
					{
						//��������豸������������н�������
						pDriver->ReConnect();
						UaString uaMsg("NR103::RcvTimeOut ��·�������豸�����߿�ʼ����");
						pDriver->WriteLogAnyWay(uaMsg.toUtf8());
						pPrtcl->m_nOffDevCount = 0;
					}
					else
					{
						//��������豸���߷���������
						OpcUa_Int32 nDataLen = 0;
						pPrtcl->BuildRequest(pPrtcl->m_pbySndBuffer, nDataLen);
					}
				}
			}
		}
	}

	void NR103PeriodTimeOut(void* pParam)
	{
		CNR103Prtcl* pPrtcl = (CNR103Prtcl*)pParam;
		if (pPrtcl)
		{
			CNR103Driver* pDriver = (CNR103Driver*)(pPrtcl->GetParam());
			if (pDriver)
			{
				//�ж��豸�������ٻ�ʱ���Ƿ��ѵ�
				if ((pPrtcl->m_nPeriodCount % (pPrtcl->m_iWQInternal-1)) == 0)
				{
					for( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++)
					{
						CNR103Device* pDevice = (CNR103Device*)pDriver->GetDeviceByIndex(i);
						if (pDevice)
						{
							pDevice->SetASDU21WQ(OpcUa_True);
						}
					}
				}
				//�ж��豸����Уʱ֡ʱ���Ƿ��ѵ�
				else if ((pPrtcl->m_nPeriodCount % (pPrtcl->m_iCheckTPeriod-1)) == 0)
				{
					for( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++)
					{
						CNR103Device* pDevice = (CNR103Device*)pDriver->GetDeviceByIndex(i);
						if (pDevice)
						{
							pDevice->SetCheckTFlag(OpcUa_True);
						}
					}
				}
				//�ж��豸��������ʱ���Ƿ��ѵ�
				else if ((pPrtcl->m_nPeriodCount % pPrtcl->m_iHBInternal) == 0)
				{
					for( OpcUa_Int32 i = 0; i < pDriver->GetDeviceCount(); i++)
					{
						CNR103Device* pDevice = (CNR103Device*)pDriver->GetDeviceByIndex(i);
						if (pDevice)
						{
							pDevice->SetSendHBTag(OpcUa_True);
						}
					}
				}
				
			}
			pPrtcl->m_nPeriodCount ++;
		}
	}
}


