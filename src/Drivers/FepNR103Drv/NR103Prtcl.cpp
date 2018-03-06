/*!
*	\file	NR103Driver.h
*
*	\brief	NR103协议解析类实现
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

	//数据接收超时处理
	void NR103RcvOutTimer(void* pParam);

	//周期状态处理定时器
	void NR103PeriodTimeOut(void* pParam);

	CNR103Prtcl::CNR103Prtcl()
		:m_iHBInternal(10),m_iWQInternal(1200),m_iCheckTPeriod(300),m_nLastAddr(0),m_nLogicalAddr(0)
		,m_nSendDelay(30),m_pRcvOutTimer(NULL),m_pPeriodTimer(NULL),m_nPeriodCount(1),m_nOffDevCount(0)
		,m_nErrPacketNum(0)
	{
		InitPrtclHelper(MIN_FRAME_LEN, MAX_FRAME_LEN, NR103_RCVBUFF, NR103_SENDBUFF, NR103_INNERBUF);
		//创建定时器对象
		m_pRcvOutTimer = new CIOTimer(NR103RcvOutTimer, this, OpcUa_True);
		m_pPeriodTimer = new CIOTimer(NR103PeriodTimeOut, this);
		//初始化内存缓冲区
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
		//关闭连接超时定时器
		if (pDriver && pDriver->GetNetConnTimer())
		{
			nRes = pDriver->GetNetConnTimer()->KillTimer();
			UaString strMsg(UaString("NR103Driver::OnConnSuccess connect IP:%1 successful!").arg(pPeerIP.c_str()));
			pDriver->WriteLogAnyWay(strMsg.toUtf8());
		}
		//!连接成功置设备校时和开始发送心跳标识
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
		//判断链路是否被禁止,禁止则直接返回
		nRes = CIOPrtclParser::BuildRequest(pbyData, nDataLen);
		if (nRes == 0)
		{
			//组请求报文帧
			nRes = BuildRequestFrame(pbyData, nDataLen);
			CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
			if (nDataLen > 0 && pDriver)
			{
				//记录日志
				pDriver->WriteCommData(m_pbySndBuffer, nDataLen, COMMDATA_SND);
				std::string strFrameData;
				strFrameData = GetCommData(m_pbySndBuffer, nDataLen, COMMDATA_SND);
				
				//记录下发报文信息
				pDriver->WriteLogAnyWay(strFrameData.c_str());
				
				if (pDriver->m_pIODrvBoard)
				{
					//发送报文
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
			//启动报文接收超时定时器
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

		//!通道如果被禁止了，则直接返回0
		CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
		if (!pDriver)
			return -1;		
		if(pDriver->IsDisable())
		{
			pDriver->WriteLog( "CNR103Driver::The Port is disable will not RxDataProc" );
			return 0;
		}
		//!加锁
		m_mutexDataSync.lock();

		//!如果需要通信监视，则发送原始数据包
		pDriver->WriteCommData( pDataBuf, nLen );

		//把数据放到内部缓冲区
		InnerAddData( pDataBuf, nLen );

		//收到新数据
		snprintf(chMsg, sizeof(chMsg), "\n\nCNR103Prtcl::RxDataProc: Receiving new Data, Len:%d ", nLen);
		pDriver->WriteLogAnyWay(chMsg);

		//从内部缓冲区中执行滑动窗口，找数据包，注意一次可能会处理多个数据包
		OpcUa_Boolean bPacketOk = false;
		OpcUa_Int32	nReceived = 0;
		do
		{
			//可取的数据长度
			nReceived = PRTCL_RCV_BUFFER_LENGTH - m_nRevLength;
			if ( 0 >= nReceived || nReceived>PRTCL_RCV_BUFFER_LENGTH )
			{
				snprintf( chMsg, sizeof(chMsg), "CNR103Prtcl::RxDataProc: Received Data is overflow, size = %d!!!!!", m_nRevLength );
				pDriver->WriteLogAnyWay(chMsg);
				m_nRevLength = 0;
				break;
			}

			//从内部缓冲区取数分析
			if( InnerRead( m_pbyRevBuffer+m_nRevLength, nReceived, nReceived) && nReceived > 0 )
			{
				m_nRevLength += nReceived;
				while( m_nRevLength >= PRTCL_MIN_PACKET_LENGTH )
				{
					OpcUa_Boolean bCompleted = false;
					OpcUa_Int32 nPacketLen = 0;

					//解析一包处理一包
					OpcUa_Boolean bTemp=false;
					do{
						bTemp = ExtractPacket( m_sPacketBuffer, &nPacketLen );
						if( bTemp )
						{
							//剩下的数据移动到接收缓冲区的开头
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

							//找到一包数据处理一包数据,防止多包同时上送时只处理最后一包。
							ProcessRcvdPacket( m_sPacketBuffer, nPacketLen, true );		
							bPacketOk = true;
						}
					}while(bTemp);

					//代码执行到这里，表面ExtractPacket到最后一轮了，则无论其成功与否，都应跳出本循环
					break;
				}
			}
		}while( 0<nReceived );

		//!解锁
		m_mutexDataSync.unlock();

		//返回处理包情况
		return bPacketOk ? 0 : -1;
	}

	bool CNR103Prtcl::ExtractPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32* pnPacketLength )
	{
		OpcUa_Boolean bACompletePacketRcvd = OpcUa_False;
		CNR103Driver* pDriver = (CNR103Driver*)(m_pParam);
		if (pPacketBuffer && pnPacketLength && pDriver)
		{
			
			pDriver->WriteLogAnyWay("CNR103Prtcl::ExtractPacket 开始解析新数据");
			OpcUa_Int32 nPacketLen = 0;
			OpcUa_Byte* pFrame = m_pbyRevBuffer;
			OpcUa_Int32 nErrorLen = 0;				//已解析过确认错误数据大小
			if (pDriver->IsLog())
			{
				std::string strContent = GetCommData(pFrame, m_nRevLength, RCV_FLAG);
				UaString uaContent(UaString("NR103Prtcl:: The RcvBuffer Len:%1, Content:%2").arg(m_nRevLength).arg(strContent.c_str()));
				pDriver->WriteLogAnyWay(uaContent.toUtf8());
			}
			//校验帧的合理性
			while (pFrame + MIN_FRAME_LEN <= m_pbyRevBuffer + m_nRevLength)
			{
				WORD wFFlag = MAKEWORD(pFrame[0],pFrame[1]);
				WORD wSFlag = MAKEWORD(pFrame[6],pFrame[7]);
				OpcUa_Int32 nAPCISize = sizeof(FrameAPCI);
				WORD wReserve = MAKEWORD(pFrame[nAPCISize-2],pFrame[nAPCISize-1]);
				//寻找报文头
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
							//TCP心跳帧
							nPacketLen = MIN_FRAME_LEN;
						}
						else
						{
							if (dwFrameLen <= MAX_FRAME_LEN)
							{
								//解析ASDU帧
								OpcUa_Int32 nASDULen = 0;
								OpcUa_Int32 nASDUBufLen = m_pbyRevBuffer + m_nRevLength - pFrame - nAPCISize;
								OpcUa_Int32 nNext = ExtractASDUFrame(&pFrame[nAPCISize], nASDUBufLen, nASDULen);
								if (F_SUCCESS == nNext)
								{
									nPacketLen = nASDULen + nAPCISize;
									if (pDriver->IsLog())
									{
										std::string strDataFrame = GetCommData(pFrame, nPacketLen, RCV_FLAG);
										UaString ustrMsg(UaString("NR103Prtcl:解析到完整包,长度:%1,Content:%2").arg(nPacketLen).arg(strDataFrame.c_str()));
										pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
									}
									//复位当前链路处理到的错误包为0
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
									UaString ustrMsg(UaString("NR103Prtcl:解析到ASDU帧中不完整包,正确长度:%1,Content:%2").arg(nDataLen).arg(strDataFrame.c_str()));
									pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
									m_nErrPacketNum ++;
									if (m_nErrPacketNum > 2)
									{
										m_nErrPacketNum = 0;
										UaString uaMsg("NR103Prtcl:超过2次解析到ASDU中错误包跳过该包数据继续解析");
										pDriver->WriteLogAnyWay(uaMsg.toUtf8());
										pFrame++;
										continue;
									}
									else
									{
										//数据包接收不完整结束本次解析尝试与后面接收数据合并解析
										break;
									}
									
								}
							}
							else
							{
								//数据包长度过长继续解析
								pFrame++;
								continue;
							}
						}
					}
					else
					{
						//数据包长度不够
						break;
					}
				}
				else
				{
					//继续寻找报文头
					pFrame++;
					continue;
				}
				
				//!解析出了合法帧
				if (nPacketLen >0)
				{
					if (nPacketLen <= MAX_FRAME_LEN)
					{
						ACE_OS::memset(pPacketBuffer, 0, MAX_FRAME_LEN);
						//解析成功将合格帧放到缓冲区
						ACE_OS::memcpy(pPacketBuffer, pFrame, nPacketLen);
						*pnPacketLength = nPacketLen;
						bACompletePacketRcvd = OpcUa_True;
						//输出解析完整帧
						std::string strPacket = GetCommData(pFrame, nPacketLen, RCV_FLAG);
						UaString ustrMsg(UaString("CNR103Prtcl::ExtractPacket receive correct Frame len: %1").arg(nPacketLen));
						pDriver->WriteLog(ustrMsg.toUtf8());
					}
					else
					{
						//虽然解析成功但数据长度溢出继续重新解析
						pFrame++;
						continue;
					}
				}
				//程序执行到此说明已解析成功或数据长度不足均可跳出循环
				break;
			}

			//去掉错误的报文
			nErrorLen = pFrame - m_pbyRevBuffer;
			if (nErrorLen > 0)
			{
				std::string strErrPacket = GetCommData(m_pbyRevBuffer, nErrorLen, RCV_FLAG);
				OpcUa_CharA pchMsg[1024];
				snprintf(pchMsg,1024,"CNR103Prtcl::ExtractPacket 去掉%d字节错误数据:%s", nErrorLen, strErrPacket.c_str());
				pDriver->WriteLogAnyWay(pchMsg);
				//减小待处理数据长度
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
				//关闭请求超时定时器
				if (m_pRcvOutTimer)
				{
					OpcUa_Int32 nRes = m_pRcvOutTimer->KillTimer();
					UaString ustrMsg("NR103Prtcl::ProcessData and kill the RctTimeOut Timer");
					pDriver->WriteLog(ustrMsg.toUtf8());
				}

				//得到相应设备对象
				FrameAPCI* pAPCI = (FrameAPCI*)(pPacketBuffer);
				WORD wDevAddr = pAPCI->wSourceDevAddr;
				CNR103Device* pDevice = (CNR103Device*)pDriver->GetDeviceByAddr(wDevAddr);
				if (pDevice)
				{
					//复位设备的发送计数
					pDevice->m_byTxTimes = 0;

					//设置设备在线
					if (bFromLocal && !pDevice->IsOnLine())
					{
						pDevice->SetOnLine(DEVICE_ONLINE);
						snprintf(chMsg,sizeof(chMsg),"设备:%s成功上线",pDevice->GetName().c_str());
						pDriver->WriteLogAnyWay(chMsg);
					}
					if (MIN_FRAME_LEN == nPacketLength)
					{
						//设备收到心跳诊断包
						OpcUa_CharA chMsg[256];
						snprintf(chMsg, 256, "NR103Prtcl: 收到设备:%s的心跳帧", pDevice->GetName().c_str());
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
								//处理ASDU10通用分类数据
								nRes = pDevice->ProcessASDU10Data(pASDUFrame, nRestByteCount);
								break;
							case TYP_IDENTIFIER:
								//处理ASDU5
								nRes = pDevice->ProcessASDU5Data(pASDUFrame, nRestByteCount);
								break;
							case TYP_TIME_SYN:
								//处理ASDU6		
								nRes = pDevice->ProcessASDU6Data(pASDUFrame, nRestByteCount);
								break;
							case TYP_DISORDER_DATA:
								nRes = pDevice->ProcessASDU23Data(pASDUFrame, nRestByteCount);
								break;
							default:
								{
									UaString ustrMsg(UaString("NR103Prtcl:设备:%1收到暂不处理ASDU:%d数据")
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
					UaString ustrMsg(UaString("NR103Prtcl:接收到报文没有找到相应设备对象,设备地址:%1").arg(wDevAddr));
					pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
				}
				//发送新的请求
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

			//启动周期处理定时器
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

			//!先组遥控,遥调帧
			nRes = BuildCtrlRequest(pbyData, nDataLen);
			if (0 == nRes && nDataLen > 0)
			{
				WORD wDevAddr = MAKEWORD(pbyData[14], pbyData[15]);
				OpcUa_Int32 nDevLogic = 0;
				if (pDriver && pDriver->GetDevIndexByAddr(wDevAddr, nDevLogic))
				{
					//将发送遥控设备索引赋给轮询索引以便立即对该设备发起请求
					m_nLogicalAddr = nDevLogic;
					m_nLastAddr = m_nLogicalAddr;
				}
				return nRes;
			}
			//!轮询链路下的设备组请求帧
			CNR103Device* pDevice = (CNR103Device*)pDriver->GetDeviceByIndex(m_nLogicalAddr);
			if (pDevice)
			{
				//!处于模拟状态则不组请求帧
				if (!pDevice->IsSimulate())
				{
					m_nLastAddr = m_nLogicalAddr;
					//!发送设备请求帧
					nRes = BuildFrame(pbyData, nDataLen, pDevice);
					if (nDataLen > 0)
					{
						//!增加设备发送请求次数
						pDevice->m_byTxTimes++;
					}

					//!切换设备轮询索引
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
		//只有主链路才发送遥控命令
		if (pbyData && pDriver && pDriver->GetHot())
		{
			OpcUa_CharA pchBuf[512];
			OpcUa_Int32 nBufLen = 512;
			nRes = pDriver->m_IORemoteCtrl.DeQueue(pchBuf, nBufLen);
			if (0==nRes)
			{
				//取出遥控命令帧
				ACE_OS::memset(&m_tRemoteCtrlCmd,0,sizeof(m_tRemoteCtrlCmd));
				memcpy((OpcUa_Byte*)(&m_tRemoteCtrlCmd), pchBuf, sizeof(m_tRemoteCtrlCmd));
				std::string strDevName = m_tRemoteCtrlCmd.pchUnitName;
				CNR103Device* pDevice = dynamic_cast<CNR103Device*>(pDriver->GetDeviceByName(strDevName));
				if (pDevice)
				{
					//设备发送次数增加
					pDevice->m_byTxTimes ++;
					if (!pDevice->IsOnLine())
					{
						OpcUa_CharA chMsg[512];
						snprintf(chMsg,512,"NR103Prtcl:设备离线下发遥控命令失败,Device:%s", pDevice->GetName().c_str());
						pDriver->WriteLogAnyWay(chMsg);
						nRes= 0;
						nDataLen = 0;
					}
					else
					{
						if (m_tRemoteCtrlCmd.byUnitType == NR103_DOSB)
						{
							//判断遥控值是否正确
							OpcUa_Byte byValue = m_tRemoteCtrlCmd.byData[0];
							if (0 == byValue || 1 == byValue)
							{
								//设置下发命令返校和遥控命令选择标签值
								OpcUa_Byte byInit = 0;
								pDevice->SetVal(NR103_DICB, m_tRemoteCtrlCmd.lTagAddress, &byInit, 1, 0, OpcUa_True);
								pDevice->SetVal(NR103_DOSB, m_tRemoteCtrlCmd.lTagAddress, &m_tRemoteCtrlCmd.byData[0], 1, 0, OpcUa_True);
								//带确认写条目
								nDataLen = pDevice->BuildASDU10DOFrame(pbyData, m_tRemoteCtrlCmd.lTagAddress, INF_WRITE_WITH_CONFIRM, m_tRemoteCtrlCmd.byData[0]);
								if (nDataLen > 0)
								{
									UaString ustrMsg(UaString("NR103Prtcl:设备%1生成遥控选择命令").arg(pDevice->GetName().c_str()));
									pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
								}
							}
							else
							{
								UaString ustrMsg(UaString("NR103Prtcl:设备:%1下发遥控选择命令值:%2")
									.arg(pDevice->GetName().c_str()).arg(byValue));
								pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
							}
						}
						else if (m_tRemoteCtrlCmd.byUnitType == NR103_DOB)
						{
							//判断遥控值是否正确
							OpcUa_Byte byValue = m_tRemoteCtrlCmd.byData[0];
							if (0 == byValue || 1 == byValue)
							{
								//设置下发命令返校和遥控命令选择标签值
								OpcUa_Byte byInit = 0;
								pDevice->SetVal(NR103_DICB, m_tRemoteCtrlCmd.lTagAddress, &byInit, 1, 0, OpcUa_True);
								pDevice->SetVal(NR103_DOB, m_tRemoteCtrlCmd.lTagAddress, &m_tRemoteCtrlCmd.byData[0], 1, 0, OpcUa_True);
								//带执行写条目
								nDataLen = pDevice->BuildASDU10DOFrame(pbyData, m_tRemoteCtrlCmd.lTagAddress, INF_WRITE_WITH_EXCUTE, m_tRemoteCtrlCmd.byData[0]);
								if (nDataLen > 0)
								{
									UaString ustrMsg(UaString("NR103Prtcl:设备%1生成遥控执行命令").arg(pDevice->GetName().c_str()));
									pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
								}
							}
							else
							{
								UaString ustrMsg(UaString("NR103Prtcl:设备:%1下发遥控执行命令值:%2")
									.arg(pDevice->GetName().c_str()).arg(byValue));
								pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
							}
						}
						else if (m_tRemoteCtrlCmd.byUnitType == NR103_RESET)
						{
							OpcUa_Byte byValue = m_tRemoteCtrlCmd.byData[0];
							//初始化信号复归命令
							OpcUa_Byte byInit = 0;
							pDevice->SetVal(NR103_RESETCB, m_tRemoteCtrlCmd.lTagAddress, &byInit, 1, 0, OpcUa_True);
							pDevice->SetVal(NR103_RESET, m_tRemoteCtrlCmd.lTagAddress, &byValue, 1, 0, OpcUa_True);
							//信号复归命令
							nDataLen = pDevice->BuildResetASDU20(pbyData, m_tRemoteCtrlCmd.lTagAddress);
							if (nDataLen > 0)
							{
								UaString ustrMsg(UaString("NR103Prtcl:设备%1生成ASDU20远方复归命令").arg(pDevice->GetName().c_str()));
								pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
							}
						}
						else if (m_tRemoteCtrlCmd.byUnitType == NR103_RDSFV)
						{
							OpcUa_Byte byValue = m_tRemoteCtrlCmd.byData[0];
							pDevice->SetVal(NR103_RDSFV, m_tRemoteCtrlCmd.lTagAddress, &byValue, 1, 0, OpcUa_True);
							//召唤定值组
							nDataLen = pDevice->BuildReadSFrame(pbyData, m_tRemoteCtrlCmd.lTagAddress);
							if (nDataLen > 0)
							{
								UaString ustrMsg(UaString("NR103Prtcl:设备%1生成读定值组命令").arg(pDevice->GetName().c_str()));
								pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
							}
						}
						//判断是否组出控制帧
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
			//!发送校时帧
			if (pDevice->IsNeedCheckT() && pDevice->IsHot())
			{
				nRes = pDevice->BuildCTFrame(pbyData, nDataLen);
				if (0 == nRes)
				{
					UaString usStrMsg(UaString("NR103Prtcl: 设备%1发送校时帧").arg(pDevice->GetName().c_str()));
					pDriver->WriteLogAnyWay(usStrMsg.toUtf8());
				}
				return nRes;
			}

			//!发送心跳帧(允许链路为从FEP发送心跳帧以判断和设备通讯状态)
			if (pDevice->IsNeedHeartB())
			{
				nRes = pDevice->BuildHBFrame(pbyData, nDataLen);
				pDevice->SetSendHBTag(OpcUa_False);
				if (0 == nRes)
				{
					UaString usStrMsg(UaString("NR103Prtcl: 设备%1发送心跳帧").arg(pDevice->GetName().c_str()));
					pDriver->WriteLogAnyWay(usStrMsg.toUtf8());
				}
				return nRes;
			}

			//!发送ASUD7总召唤帧
			if (pDevice->IsNeedASUD7() && pDevice->IsHot())
			{
				nRes = pDevice->BuildASDU7(pbyData, nDataLen);
				pDevice->SetASDU7(OpcUa_False);
				if (0 == nRes)
				{
					UaString usStrMsg(UaString("NR103Prtcl: 设备%1发送总召唤帧").arg(pDevice->GetName().c_str()));
					pDriver->WriteLogAnyWay(usStrMsg.toUtf8());
				}
				return nRes;
			}

			//!发送ASDU21总召唤帧
			if (pDevice->IsNeedASUD21() && pDevice->IsHot())
			{
				nRes = pDevice->BuildASDU21(pbyData, nDataLen);
				pDevice->SetASDU21WQ(OpcUa_False);
				if (0 == nRes)
				{
					UaString usStrMsg(UaString("NR103Prtcl: 设备%1发送ASDU21总查询帧").arg(pDevice->GetName().c_str()));
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
		//! 获取数据显示格式
		for ( int i=0; i < nPacketLen; i++ )
		{
			sprintf( pchByte, "%02X ", pFrame[i] );
			szData += pchByte;
		}
		return szData;
	}

	//解析ASDU帧
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
			//判断长度
			if (nlen < MIN_103ASDU_LEN)
			{
				nRes = F_BREAK;
				return nRes;
			}
			//判断ASDU类型
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
			//判断传送原因
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
			//解析成功
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
			//解析成功
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
			//开始检查数据
			ASDU10_HEAD* pHead = (ASDU10_HEAD*)pFrame;
			if (pHead->byNGD > 0)
			{
				OpcUa_Int32 nGIDIndex = 0;
				OpcUa_UInt32 nByteCount = sizeof(ASDU10_HEAD);	//ASDU长度
				OpcUa_UInt32 nRestByteCount = nlen - sizeof(ASDU10_HEAD);
				OpcUa_UInt32 nEGIDSize = 0;
				OpcUa_Byte* pGDPos = pFrame + sizeof(ASDU10_HEAD);
				GDATAPointHead* pDGHead = (GDATAPointHead*)pGDPos;
				for (; nGIDIndex < pHead->byNGD; nGIDIndex++)
				{
					if (pDGHead->sGDD.byDataType != GDD_DT0_NODATA && 
						(pDGHead->sGDD.byDataSize == 0 || pDGHead->sGDD.byDataNum == 0))
					{
						UaString ustrMsg(UaString("NR103Prtcl:ExtracktASDU10Frame报文中上送数据长度有误,打算上传GID:%1,实际解析到%2").arg(pHead->byNGD).arg(nGIDIndex));
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
							//移至下一数据点
							pGDPos += sizeof(GDATAPointNoData);
							pDGHead = (GDATAPointHead*)pGDPos;
						}
						else
						{
							//包长度不够
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
							//移至下一数据点
							pGDPos += (sizeof(GDATAPointHead)+nEGIDSize);
							pDGHead = (GDATAPointHead*)pGDPos;
						}
						else
						{
							//包长度不够
							UaString ustrMsg(UaString("NR103Prtcl:ExtracktASDU10Frame报文中上送数据长度不足,打算上传GID:%1,实际解析到%2").arg(pHead->byNGD).arg(nGIDIndex));
							CNR103Driver* pDriver = (CNR103Driver*)m_pParam;
							if (pDriver)
							{
								pDriver->WriteLog(ustrMsg.toUtf8());
							}
							break;
						}
					}
				}
				//包长度溢出
				if (nByteCount > MAX_NR103_FRAM_LEN)
				{
					nRes = F_CONTINUE;
					return nRes;
				}
				//每个GID都检查过了
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
				//无数据
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
				//解析成功
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
			//检查长度
			OpcUa_UInt32 nValidDataLen = sizeof(ASDU28);
			if (nValidDataLen > nlen)
			{
				nRes = F_BREAK;
				return nRes;
			}
			//解析成功
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
			//检查长度
			if (sizeof(ASDU29Head) > nlen)
			{
				nRes = F_BREAK;
				return nRes;
			}
			ASDU29Head* pHead = (ASDU29Head*)pFrame;
			OpcUa_Byte byCount = pHead->NOT;
			//检查数据长度
			OpcUa_UInt32 nValidDataLen = sizeof(ASDU29Head) + byCount*sizeof(ASDU29Head);
			if (nValidDataLen > nlen)
			{
				nRes = F_BREAK;
				return nRes;
			}
			//解析成功
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
			//解析成功
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
			//解析成功
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
			//停止定时器
			OpcUa_Int32 nRes = pPrtcl->m_pRcvOutTimer->KillTimer();
			if (0 == nRes)
			{
				//得到上次发送请求设备对象
				OpcUa_Int32 nLastDevAddr = pPrtcl->GetLastDevIndex();			
				CNR103Driver* pDriver = (CNR103Driver*)(pPrtcl->GetParam());
				if (pDriver)
				{
					UaString ustrMsg("NR103Prtcl:: RctTimeOut Timer Start Work");
					pDriver->WriteLog(ustrMsg.toUtf8());

					CNR103Device* pDevice = (CNR103Device*)pDriver->GetDeviceByIndex(nLastDevAddr);
					//判断设备是否离线
					if (pDevice && (pDevice->m_byTxTimes > pPrtcl->GetMaxOffLineCount()))
					{
						pDevice->SetOnLine(DEVICE_OFFLINE);
						//恢复设备发送计数
						pDevice->m_byTxTimes = 0;
						//离线设备个数增加
						pPrtcl->m_nOffDevCount++;
						UaString ustrMsg(UaString("NR103::RcvTimeOut 设备:%1 离线").arg(pDevice->GetName().c_str()));
						pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
					}

					if (pPrtcl->m_nOffDevCount > pDriver->GetDeviceCount())
					{
						//如果所有设备都已离线则进行进行重连
						pDriver->ReConnect();
						UaString uaMsg("NR103::RcvTimeOut 链路下所有设备都离线开始重连");
						pDriver->WriteLogAnyWay(uaMsg.toUtf8());
						pPrtcl->m_nOffDevCount = 0;
					}
					else
					{
						//如果还有设备在线发送新请求
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
				//判断设备发送总召唤时机是否已到
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
				//判断设备发送校时帧时机是否已到
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
				//判断设备发送心跳时机是否已到
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


