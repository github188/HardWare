/*!
*	\file	NR103Driver.h
*
*	\brief	NR103驱动类实现
*
*	$Date: 15-03-30$
*
*/
#include "NR103Device.h"
#include "NR103Driver.h"
#include <math.h>

namespace MACS_SCADA_SUD
{
	std::string CNR103Device::s_ClassName = "CNR103Device";

	CNR103Device::CNR103Device()
		:m_wSFacAddr(0),m_wDFacAddr(0),m_wSDevAddr(0),m_wDDevAddr(0),m_wFRoutAddr(0),m_wLRoutAddr(0),
		 m_nDataNum(0),m_wDevType(0),m_nASDU21GCount(0),m_wDNetState(DNS_S_AB),m_bIsNeedCheckT(OpcUa_False),
		 m_bNeedASDU21GI(OpcUa_False),m_bNeedASDU7(OpcUa_False),m_bIsNeedSendHB(OpcUa_False),m_bySCN(0)
	{

	}

	CNR103Device::~CNR103Device()
	{

	}

	std::string CNR103Device::GetClassName()
	{
		return s_ClassName;
	}

	OpcUa_Int32 CNR103Device::Init()
	{
		OpcUa_Int32 nRes = -1;
		nRes = CIODeviceBase::Init();
		if (0 == nRes)
		{
			nRes = InitDevConfig();
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::InitDevConfig()
	{
		OpcUa_Int32 nRes = -1;
		std::string strFilePath = CIODriver::GetFEPRunDir() + GetDevCfgName();
		if (!strFilePath.empty())
		{
			m_wDDevAddr = (WORD)GetAddr();
			m_wSFacAddr = (WORD)GetPrivateProfileInt("Device", "SFacAddr", 0, strFilePath.c_str());
			m_wDFacAddr = (WORD)GetPrivateProfileInt("Device", "DFacAddr", 0, strFilePath.c_str());
			m_wFRoutAddr = (WORD)GetPrivateProfileInt("Device", "FRoutAddr", 0, strFilePath.c_str());
			m_wLRoutAddr = (WORD)GetPrivateProfileInt("Device", "LRoutAddr", 0, strFilePath.c_str());
			m_wDevType = GetPrivateProfileInt("Device", "DevType", DT_PROMEACTL, strFilePath.c_str());
			m_wDNetState = GetPrivateProfileInt("Device", "DevNState", DNS_S_AB, strFilePath.c_str());
			m_bNeedASDU7 = GetPrivateProfileInt("ASDU7", "IsNeed", 0, strFilePath.c_str());
			m_nASDU21GCount = GetPrivateProfileInt("ASDU21", "GroupCount", 0, strFilePath.c_str());
			if (m_nASDU21GCount > 0)
			{
				ProcessGroupNo(strFilePath);
			}
			nRes = 0;
		}
		return nRes;
	}

	void CNR103Device::ProcessGroupNo( std::string &strFilePath ) 
	{
		if (!strFilePath.empty())
		{
			OpcUa_CharA chBuf[256];
			GetPrivateProfileString("ASDU21", "GroupNo", "", chBuf, 256, strFilePath.c_str());
			std::string strASDUGNo = chBuf;
			if (!strASDUGNo.empty())
			{
				OpcUa_Int32 nFPos = 0;
				OpcUa_Int32 nSPos = 0;
				OpcUa_Int32 nIndex = 0;
				OpcUa_Int32 nGroupNo = 0;
				while (true)
				{
					nSPos = strASDUGNo.find(",", nFPos);
					if (nSPos < 0 || nIndex >= m_nASDU21GCount)
					{
						break;
					}
					std::string strGNo = strASDUGNo.substr(nFPos, (nSPos - nFPos));
					nGroupNo = ACE_OS::atoi(strGNo.c_str());
					if (nGroupNo > 0 && nGroupNo < 256)
					{
						m_vec21GroupNo.push_back(nGroupNo);
					}
					nIndex++;
					nFPos = nSPos+1;
				}
			}
		}
	}

	void CNR103Device::SetCheckTFlag( OpcUa_Boolean bIsNeedCT )
	{
		m_bIsNeedCheckT = bIsNeedCT;
	}

	void CNR103Device::SetSendHBTag( OpcUa_Boolean bSendHB )
	{
		m_bIsNeedSendHB = bSendHB;
	}

	void CNR103Device::SetASDU21WQ( OpcUa_Boolean bFlag )
	{
		m_bNeedASDU21GI = bFlag;
	}

	void CNR103Device::SetASDU7( OpcUa_Boolean bFlag )
	{
		m_bNeedASDU7 = bFlag;
	}

	OpcUa_Boolean CNR103Device::IsNeedCheckT()
	{
		return m_bIsNeedCheckT;
	}

	OpcUa_Boolean CNR103Device::IsNeedHeartB()
	{
		return m_bIsNeedSendHB;
	}

	OpcUa_Boolean CNR103Device::IsNeedASUD7()
	{
		return m_bNeedASDU7;
	}

	OpcUa_Boolean CNR103Device::IsNeedASUD21()
	{
		return m_bNeedASDU21GI;
	}

	OpcUa_Int32 CNR103Device::BuildCTFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen )
	{
		OpcUa_Int32 nRes = -1;
		if (pbyData)
		{
			ACE_Time_Value nowTime;
			ACE_Date_Time dateTime;

			ASDU6Frame* pData = (ASDU6Frame*)pbyData;
			ACE_OS::memset(pData, 0, sizeof(ASDU6Frame));
			FrameAPCI* pAPCI = (FrameAPCI*)(&pData->sFrameAPCI);
			ASDU_HEAD* pHead = (ASDU_HEAD*)(&pData->sASUDHead);
			CP56Time2a* pTime = (CP56Time2a*)(&pData->sTimeStamp);
			DWORD dwFrameLen = sizeof(ASDU6Frame) - 8;

			nRes = BuildAPCIFrame(pAPCI, dwFrameLen);

			pHead->byType = TYP_TIME_SYN;
			pHead->byVSQ = 0x81;
			pHead->byCOT = COT_TIME_SYN;
			pHead->byAddr = m_wDDevAddr;
			pHead->byFun = FUN_GLB;
			pHead->byInf = INF_TIME_SYN_OR_QUERY_BEGIN;

			WORD millSec = 0;
			nowTime = ACE_OS::gettimeofday();
			dateTime.update(nowTime);
			millSec = (WORD)(dateTime.second()*1000 + dateTime.microsec()/1000);
			pTime->LowMillSec = LOBYTE(millSec);
			pTime->HighMillSec = HIBYTE(millSec);
			pTime->Minute = (OpcUa_Byte)dateTime.minute();
			pTime->Hour = (OpcUa_Byte)dateTime.hour();
			pTime->Day = (OpcUa_Byte)dateTime.day();
			pTime->Week = (dateTime.day()==0) ? 7:(OpcUa_Byte)dateTime.weekday();
			pTime->Mon = (OpcUa_Byte)dateTime.month();
			pTime->Year = dateTime.year() % 100;

			nDataLen = sizeof(ASDU6Frame);
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::BuildHBFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen )
	{
		OpcUa_Int32 nRes = -1;
		if (pbyData)
		{
			FrameAPCI* pData = (FrameAPCI*)pbyData;
			nDataLen = sizeof(FrameAPCI);
			ACE_OS::memset(pData, 0, nDataLen);
			DWORD dwFrameLen = nDataLen - 8;
			nRes = BuildAPCIFrame(pData, dwFrameLen);
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::BuildASDU7( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen )
	{
		OpcUa_Int32 nRes;
		if (pbyData)
		{
			ASDU7Frame* pFrame = (ASDU7Frame*)pbyData;
			nDataLen = sizeof(ASDU7Frame);
			ACE_OS::memset(pFrame,0,nDataLen);
			FrameAPCI* pAPCI = (FrameAPCI*)(&pFrame->sFrameAPCI);
			ASDU_HEAD* pASDUHead = (ASDU_HEAD*)(&pFrame->sASDUHead);
			nRes = BuildAPCIFrame(pAPCI, nDataLen-8);

			pASDUHead->byType = TYP_GENERAL_QUERY_START;
			pASDUHead->byVSQ = VSQ_MOST;
			pASDUHead->byCOT = COT_GENERAL_QUERY;
			pASDUHead->byAddr = m_wDDevAddr;
			pASDUHead->byFun = FUN_GLB;
			pASDUHead->byInf = INF_TIME_SYN_OR_QUERY_BEGIN;
			pFrame->bySCN = (m_bySCN++)%256;

			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::BuildASDU21( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen )
	{
		OpcUa_Int32 nRes;
		if (pbyData)
		{
			ASDU21WQFrame* pFrame = (ASDU21WQFrame*)pbyData;
			nDataLen = sizeof(ASDU21WQFrame);
			ACE_OS::memset(pFrame,0,nDataLen);
			FrameAPCI* pAPCI = (FrameAPCI*)(&pFrame->sFrameAPCI);
			ASDU10_HEAD* pASDU21WQ = (ASDU10_HEAD*)(&pFrame->sASDU21WQ);

			nRes = BuildAPCIFrame(pAPCI,nDataLen-8);
			
			pASDU21WQ->sASDUHead.byType = TYP_COMM_CMD;
			pASDU21WQ->sASDUHead.byVSQ = VSQ_MOST;
			pASDU21WQ->sASDUHead.byCOT = COT_GENERAL_QUERY;
			pASDU21WQ->sASDUHead.byAddr = m_wDDevAddr;
			pASDU21WQ->sASDUHead.byFun = FUN_COMM_SORT;
			pASDU21WQ->sASDUHead.byInf = INF_COMM_DATA_GENERAL_QUERY;
			pASDU21WQ->byRII = 0;
			pASDU21WQ->byCNTU=0;
			pASDU21WQ->byCOUNT=0;
			pASDU21WQ->byNGD=0;

			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessASDU10Data( const OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nDataLen )
	{
		OpcUa_Int32 nRes = -1;
		CNR103Driver* pDriver = (CNR103Driver*)m_pDriver;
		if (pASDUFrame && nDataLen >= MIN_IEC103_COMM_LEN )
		{
			ASDU10_HEAD* pHead = (ASDU10_HEAD*)pASDUFrame;
			if (pHead->byNGD > 0)
			{
				OpcUa_Int32 nTagAddr = 0;
				OpcUa_Byte byValue;
				OpcUa_Int32 nValue;
				OpcUa_Float fValue;
				OpcUa_Int32 nByteCount = sizeof(ASDU10_HEAD);
				OpcUa_Int32 nRestByteCount = nDataLen - sizeof(ASDU10_HEAD);
				OpcUa_Int32 nOneGIDSize = 0;
				OpcUa_Byte* pGDPos = (OpcUa_Byte*)(pASDUFrame + sizeof(ASDU10_HEAD));
				OpcUa_Byte byCOT = pHead->sASDUHead.byCOT;
				GDATAPointHead* pGDHead = (GDATAPointHead*)pGDPos;
				//分析COT并把COT分为三类作出相关处理
				//1.系统消息----根据设备行为需要,执行校时,总召唤. 
				//2.肯定响应----把相应Group和Entry的DIC标签置为1
				//3.否定响应----把相应Group和Entry的DIC标签置为0
				//注意, Group和Entry所对应的真正响应值更新到各自DI和AI.
				if (pHead->byNGD == 1)
				{
					ASDU10NoDataSet* pASDU = (ASDU10NoDataSet*)pASDUFrame;
					nTagAddr = GetTagAddr(pASDU->Gin.byGroup, pASDU->Gin.byEntry);
					byValue = ProASDU10NoDataSet(byCOT, nTagAddr);
				}
				//遍历通用分类数据
				for (OpcUa_Int32 nIndex = 0; nIndex < pHead->byNGD; nIndex++)
				{
					if (pGDHead->sGDD.byDataType == GDD_DT0_NODATA)
					{
						if (nRestByteCount >= sizeof(GDATAPointNoData))
						{
							nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry);
							//移动至下一数据点
							nRestByteCount -= sizeof(GDATAPointNoData);
							nByteCount += sizeof(GDATAPointNoData);
							pGDPos += sizeof(GDATAPointNoData);
							pGDHead = (GDATAPointHead*)pGDPos;
						}
					}
					else
					{
						nOneGIDSize = (pGDHead->sGDD.byDataSize)*(pGDHead->sGDD.byDataNum);
						if (nRestByteCount >= sizeof(GDATAPointHead)+nOneGIDSize)
						{
							if (pGDHead->byKOD == KOD1_ACTUALVALUE)
							{
								OpcUa_Byte* pCurData = (OpcUa_Byte*)(pASDUFrame + nByteCount + sizeof(GDATAPointHead));
								nRes = ProcessGID(pCurData, pGDHead, pDriver);
							}
							else
							{
								UaString ustrMsg(UaString("设备:%1处理ASDU10,组号:%2,条目号:%3的KOD:%4为非实际值").arg(GetName().c_str())
									.arg(pGDHead->sGIN.byGroup).arg(pGDHead->sGIN.byEntry).arg(pGDHead->byKOD));
								pDriver->WriteLog(ustrMsg.toUtf8());
							}
							//移动至下一数据点
							nRestByteCount -= sizeof(GDATAPointHead) + nOneGIDSize;
							nByteCount += sizeof(GDATAPointHead)+nOneGIDSize;
							pGDPos += sizeof(GDATAPointHead)+nOneGIDSize;
							pGDHead = (GDATAPointHead*)pGDPos;
						}
					}
				}

				//如果有变位发生需要输出变位报文

			}
			else		//无数据
			{
				if (sizeof(ASDU10NoDataSet) == nDataLen)
				{
					ASDU10NoDataSet* pASDU = (ASDU10NoDataSet*)pASDUFrame;
					OpcUa_Int32 nTagAddr = GetTagAddr(pASDU->Gin.byGroup,pASDU->Gin.byEntry);
					OpcUa_Byte byCOT = pHead->sASDUHead.byCOT;
					//当收到无数据ASDU10应答时把COT值写入组号和条目所在标签
					SetVal(NR103_COT, nTagAddr, (OpcUa_Byte*)&byCOT, sizeof(byCOT), 0, true);
					OpcUa_Byte nRes = ProASDU10NoDataSet(byCOT, nTagAddr);
				}
			}
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::GetTagAddr( OpcUa_Byte byGroup, OpcUa_Byte byEntry )
	{
		return byGroup*1000+byEntry;
	}

	OpcUa_Int32 CNR103Device::ProcessGID( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			switch(pGDHead->sGDD.byDataType)
			{
			case GDD_DT1_OS8ASCII:	//注意：采用UI相同的值合成法，当每个数据的长度超过4字节时会失真
			case GDD_DT2_BS1:
			case GDD_DT3_UI:
			case GDD_DT4_I:
				nRes = ProcessDTUI(pCurData, pGDHead, pDriver);
				break;
			case GDD_DT5_UFLOAT:	//暂没有特别处理
			case GDD_DT6_FLOAT:
			case GDD_DT7_FLOAT:
				nRes = ProcessDTFloat(pCurData, pGDHead, pDriver);
				break;
			case GDD_DT8_DOUBLE:
				nRes = ProcessDTDouble(pCurData, pGDHead,pDriver);
				break;
			case GDD_DT9_DPI:
				nRes = ProcessDTDPI(pCurData, pGDHead, pDriver);
				break;
			case GDD_DT10_SPI:
				nRes = ProcessDTSPI(pCurData, pGDHead, pDriver);
				break;
			case GDD_DT11_DPI:
				nRes = ProcessDT11DPI(pCurData, pGDHead, pDriver);
				break;
			case GDD_DT12_AITime:
				nRes = ProcessDTAIWT(pCurData, pGDHead, pDriver);
				break;
			case GDD_DT18_DPITime:
				nRes = ProcessDT18DPI(pCurData, pGDHead, pDriver);
				break;
			case GDD_DT19_DPITime:
				nRes = ProcessDT19DPI(pCurData, pGDHead, pDriver);
				break;
			case GDD_DT203_CP72:
				nRes = ProcessDT203(pCurData, pGDHead, pDriver);
				break;
			case GDD_DT204_CP104:
				nRes = Processdt204(pCurData, pGDHead, pDriver);
				break;
			default:
				{
					UaString ustrMsg(UaString("设备:%1 接收到不能处理数据类型 DataType:%2 ").arg(GetName().c_str()).arg(pGDHead->sGDD.byDataType));
					pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
				}
				break;
			}
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessDTUI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver)
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			OpcUa_Int32 nTagAddr = 0;
			for (OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++)
			{
				nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);
				OpcUa_Int32 nValue = 0;
				for (OpcUa_Int32 k=0; k < pGDHead->sGDD.byDataSize; k++)
				{
					nValue += int(pbyData[k] * pow((float)256, k));
				}
				OpcUa_Float fValue = (OpcUa_Float)nValue;
				WriteProInfo(pGDHead, fValue, pDriver);

				//更新值
				SetVal(NR103_AIG, nTagAddr, (OpcUa_Byte*)(&fValue), sizeof(OpcUa_Float), 0, true);
				pbyData += pGDHead->sGDD.byDataSize;
			}
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessDTFloat( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			OpcUa_Int32 nTagAddr = 0;
			OpcUa_Float fValue;
			for( OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++ )
			{
				nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);

				//合成数值
				fValue = *(float* )pbyData;	//验证是否值转换正确，否则考虑逆序转换
				WriteProInfo(pGDHead, fValue, pDriver);

				SetVal(NR103_AIG, nTagAddr, (OpcUa_Byte*)(&fValue), sizeof(OpcUa_Float), 0, true);
				pbyData += pGDHead->sGDD.byDataSize;
			}
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessDTDouble( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			for (OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++)
			{
				OpcUa_Int32 nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);
				OpcUa_Float fValue = (OpcUa_Float)(*(double*)pbyData);
				SetVal(NR103_AIG, nTagAddr, (OpcUa_Byte*)&fValue, sizeof(OpcUa_Float), 0, OpcUa_True);
				WriteProInfo(pGDHead, fValue, pDriver);
				pbyData += pGDHead->sGDD.byDataSize;
			}
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessDTDPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			for (OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++)
			{
				OpcUa_Int32 nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);
				OpcUa_Byte byValue = *pbyData;
				OpcUa_Float fValue = byValue;
				SetVal(NR103_AIG, nTagAddr, (OpcUa_Byte*)&fValue, sizeof(OpcUa_Float), 0, OpcUa_True);
				WriteProInfo(pGDHead, fValue, pDriver);
				
				if (fValue == 1 || fValue == 2)
				{
					fValue -= 1;
					//SOE发送交给平台来产生
					SetVal(NR103_DIG, nTagAddr, (OpcUa_Byte*)&fValue, sizeof(fValue), 0, OpcUa_True);
				}

				pbyData += pGDHead->sGDD.byDataSize;
			}
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessDTSPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			for (OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++)
			{
				OpcUa_Int32 nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);
				OpcUa_Byte byValue = *pbyData;
				OpcUa_Float fValue = byValue;
				if (0 == fValue || 1 == fValue)
				{
					SetVal(NR103_DIG, nTagAddr, (OpcUa_Byte*)&fValue, sizeof(fValue), 0, OpcUa_True);
				}
				
				WriteProInfo(pGDHead, fValue, pDriver);
				pbyData += pGDHead->sGDD.byDataSize;
			}
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessDT11DPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			for (OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++)
			{
				OpcUa_Int32 nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);
				OpcUa_Byte byValue = *pbyData;
				OpcUa_Float fValue = byValue;
				if (fValue <= 3)
				{
					SetVal(NR103_AIG, nTagAddr, (OpcUa_Byte*)&fValue, sizeof(fValue), 0, OpcUa_True);
				}
				WriteProInfo(pGDHead, fValue, pDriver);
				pbyData += pGDHead->sGDD.byDataSize;
			}
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessDTAIWT( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			for (OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++)
			{
				OpcUa_Int32 nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);
				MEA_AI* pData = (MEA_AI*)pbyData;
				if (pData->ER == 0 && pData->OV == 0)
				{
					OpcUa_Float fValue = (OpcUa_Float)pData->Value;
					SetVal(NR103_AIG, nTagAddr, (OpcUa_Byte*)(&fValue), sizeof(OpcUa_Float), 0, OpcUa_True);
					WriteProInfo(pGDHead, fValue, pDriver);
				}
				pbyData += pGDHead->sGDD.byDataSize;
			}
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessDT18DPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			for (OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++)
			{
				OpcUa_Int32 nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);
				CP48* pPoint = (CP48*)pCurData;
				OpcUa_Byte byValue = pPoint->DPI;
				SetVal(NR103_AIG, nTagAddr, &byValue, sizeof(OpcUa_Byte), 0, OpcUa_True);
				if (byValue == 1 || byValue == 2)
				{
					byValue -= 1;
					//判断DI是否变位
					OpcUa_Boolean bIsChange = IsDIValueChange(NR103_DIG, nTagAddr, byValue, pDriver);

					if (pPoint->Time.IV == 1)
					{
						//时间无效
						SetVal(NR103_DIG, nTagAddr, &byValue, sizeof(byValue), 0, OpcUa_True);
						UaString uaStrMsg(UaString("NR103Device:%1 地址:%2 DI变位 DPI报文中时间信息不对需要校时").arg(GetName().c_str()).arg(nTagAddr));
						pDriver->WriteLogAnyWay(uaStrMsg.toUtf8());
					}
					else if (pPoint->Time.SU == 0)
					{
						//时间有效且不为夏时制
						CP32Time2a* pTime = &pPoint->Time;
						ACE_Time_Value val = ACE_OS::gettimeofday();
						ACE_Date_Time dtime;
						dtime.update(val);
						
						//处理上传时标
						DAILYTIME sTime;
						sTime.lYear = dtime.year();
						sTime.sMonth = dtime.month();
						sTime.sDay = dtime.day();
						sTime.sHour = pTime->Hour;
						sTime.sMinute = pTime->Minute;
						sTime.sSecond = MAKEWORD(pTime->LowMillSec, pTime->HighMillSec)/1000;
						sTime.sMSecond = MAKEWORD(pTime->LowMillSec, pTime->HighMillSec)%1000;
						if (bIsChange)
						{
							//输出DI变位时间信息
							WriteSOETime(GetName(), nTagAddr, sTime, pDriver);
						}
						OpcUa_Int64 nTime = CaculateMSNum(sTime);
						SetVal(NR103_DIG, nTagAddr, &byValue, sizeof(byValue), nTime, OpcUa_True);
					}
					else
					{
						UaString uastrMsg(UaString("NR103Device:%1 DI变位信息中时间为夏时制").arg(GetName().c_str()));
						pDriver->WriteLogAnyWay(uastrMsg.toUtf8());
					}
				}
				WriteProInfo(pGDHead, byValue, pDriver);
				pbyData += pGDHead->sGDD.byDataSize;
			}
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessDT19DPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			for (OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++)
			{
				OpcUa_Int32 nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);
				CP80* pPoint = (CP80*)pCurData;
				OpcUa_Byte byValue = pPoint->DPI;
				//OpcUa_Float fValue = byValue;
				SetVal(NR103_AIG, nTagAddr, (OpcUa_Byte*)(&byValue), sizeof(OpcUa_Byte), 0, OpcUa_True);
				if (byValue == 1 || byValue == 2)
				{
					byValue -= 1;
					//判断DI是否变位
					OpcUa_Boolean bIsChange = IsDIValueChange(NR103_DIG, nTagAddr, byValue, pDriver);

					if (pPoint->Time.IV == 1)
					{
						//时间无效
						SetVal(NR103_DIG, nTagAddr, &byValue, sizeof(byValue), 0, OpcUa_True);
						UaString uaStrMsg(UaString("NR103Device:%1 地址:%2 DI变位 DPI报文中时间信息不对需要校时").arg(GetName().c_str()).arg(nTagAddr));
					}
					else if (pPoint->Time.SU == 0)
					{
						//时间有效且不为夏时制
						CP32Time2a* pTime = &pPoint->Time;
						ACE_Time_Value val = ACE_OS::gettimeofday();
						ACE_Date_Time dtime;
						dtime.update(val);

						//处理上传时标
						DAILYTIME sTime;
						sTime.lYear = dtime.year();
						sTime.sMonth = dtime.month();
						sTime.sDay = dtime.day();
						sTime.sHour = pTime->Hour;
						sTime.sMinute = pTime->Minute;
						sTime.sSecond = MAKEWORD(pTime->LowMillSec, pTime->HighMillSec)/1000;
						sTime.sMSecond = MAKEWORD(pTime->LowMillSec, pTime->HighMillSec)%1000;
						if (bIsChange)
						{
							//输出DI变位时间信息
							WriteSOETime(GetName(), nTagAddr, sTime, pDriver);
						}
						OpcUa_Int64 nTime = CaculateMSNum(sTime);
						SetVal(NR103_DIG, nTagAddr, &byValue, sizeof(byValue), nTime, OpcUa_True);

						//检查更新后的值
						CheckUpdateVal(NR103_DIG, nTagAddr, pDriver);

					}
					else
					{
						UaString uastrMsg(UaString("NR103Device:%1 DI变位信息中时间为夏时制").arg(GetName().c_str()));
						pDriver->WriteLogAnyWay(uastrMsg.toUtf8());
					}
				}
				WriteProInfo(pGDHead, byValue, pDriver);
				pbyData += pGDHead->sGDD.byDataSize;
			}
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessDT203( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			for (OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++)
			{
				OpcUa_Int32 nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);
				CP72* pPoint = (CP72*)pCurData;
				OpcUa_Byte byValue = pPoint->DPI;
				//OpcUa_Float fValue = byValue;
				SetVal(NR103_AIG, nTagAddr, &byValue, sizeof(byValue), 0, OpcUa_True);
				if (byValue == 1 || byValue == 2)
				{
					byValue -= 1;
					//判断DI是否变位
					OpcUa_Boolean bIsChange = IsDIValueChange(NR103_DIG, nTagAddr, byValue, pDriver);

					if (pPoint->Time.IV == 1)
					{
						//时间无效
						SetVal(NR103_DIG, nTagAddr, &byValue, sizeof(byValue), 0, OpcUa_True);
						UaString uaStrMsg(UaString("NR103Device:%1 地址:%2 DI变位 DPI报文中时间信息不对需要校时").arg(GetName().c_str()).arg(nTagAddr));
					}
					else if (pPoint->Time.SU == 0)
					{
						//时间有效且不为夏时制
						CP56Time2a* pTime = &pPoint->Time;
						ACE_Time_Value val = ACE_OS::gettimeofday();
						ACE_Date_Time dtime;
						dtime.update(val);

						//处理上传时标
						DAILYTIME sTime;
						sTime.lYear = pTime->Year + (dtime.year()/100)*100;
						sTime.sMonth = pTime->Mon;
						sTime.sDay = pTime->Day;
						sTime.sHour = pTime->Hour;
						sTime.sMinute = pTime->Minute;
						sTime.sSecond = MAKEWORD(pTime->LowMillSec, pTime->HighMillSec)/1000;
						sTime.sMSecond = MAKEWORD(pTime->LowMillSec, pTime->HighMillSec)%1000;
						if (bIsChange)
						{
							//输出DI变位时间信息
							WriteSOETime(GetName(), nTagAddr, sTime, pDriver);
						}
						OpcUa_Int64 nTime = CaculateMSNum(sTime);
						SetVal(NR103_DIG, nTagAddr, &byValue, sizeof(byValue), nTime, OpcUa_True);
					}
					else
					{
						UaString uastrMsg(UaString("NR103Device:%1 DI变位信息中时间为夏时制").arg(GetName().c_str()));
						pDriver->WriteLogAnyWay(uastrMsg.toUtf8());
					}
				}
				WriteProInfo(pGDHead, byValue, pDriver);
				pbyData += pGDHead->sGDD.byDataSize;

			}
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::Processdt204( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver )
	{
		OpcUa_Int32 nRes = -1;
		if (pCurData && pGDHead && pDriver)
		{
			OpcUa_Byte* pbyData = pCurData;
			for (OpcUa_Int32 j = 0; j < pGDHead->sGDD.byDataNum; j++)
			{
				OpcUa_Int32 nTagAddr = GetTagAddr(pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry+j);
				CP104* pPoint = (CP104*)pCurData;
				OpcUa_Byte byValue = pPoint->DPI;
				//OpcUa_Float fValue = byValue;
				SetVal(NR103_AIG, nTagAddr, &byValue, sizeof(byValue), 0, OpcUa_True);
				if (byValue == 1 || byValue == 2)
				{
					byValue -= 1;
					//判断DI是否变位
					OpcUa_Boolean bIsChange = IsDIValueChange(NR103_DIG, nTagAddr, byValue, pDriver);

					if (pPoint->Time.IV == 1)
					{
						//时间无效
						SetVal(NR103_DIG, nTagAddr, &byValue, sizeof(byValue), 0, OpcUa_True);
						UaString uaStrMsg(UaString("NR103Device:%1 地址:%2 DI变位 DPI报文中时间信息不对需要校时").arg(GetName().c_str()).arg(nTagAddr));
					}
					else if (pPoint->Time.SU == 0)
					{
						//时间有效且不为夏时制
						CP56Time2a* pTime = &pPoint->Time;
						ACE_Time_Value val = ACE_OS::gettimeofday();
						ACE_Date_Time dtime;
						dtime.update(val);

						//处理上传时标
						DAILYTIME sTime;
						sTime.lYear = pTime->Year + (dtime.year()/100)*100;
						sTime.sMonth = pTime->Mon;
						sTime.sDay = pTime->Day;
						sTime.sHour = pTime->Hour;
						sTime.sMinute = pTime->Minute;
						sTime.sSecond = MAKEWORD(pTime->LowMillSec, pTime->HighMillSec)/1000;
						sTime.sMSecond = MAKEWORD(pTime->LowMillSec, pTime->HighMillSec)%1000;
						if (bIsChange)
						{
							//输出DI变位时间信息
							WriteSOETime(GetName(), nTagAddr, sTime, pDriver);
						}
						OpcUa_Int64 nTime = CaculateMSNum(sTime);
						SetVal(NR103_DIG, nTagAddr, &byValue, sizeof(byValue), nTime, OpcUa_True);
					}
					else
					{
						UaString uastrMsg(UaString("NR103Device:%1 DI变位信息中时间为夏时制").arg(GetName().c_str()));
						pDriver->WriteLogAnyWay(uastrMsg.toUtf8());
					}
				}
				WriteProInfo(pGDHead, byValue, pDriver);
				pbyData += pGDHead->sGDD.byDataSize;
			}
			nRes = 0;
		}
		return nRes;
	}

	void CNR103Device::WriteProInfo( GDATAPointHead* pGDHead, OpcUa_Float fValue, CNR103Driver* pDriver, OpcUa_Boolean isAnyWay )
	{
		if (pGDHead && pDriver)
		{
			OpcUa_CharA chMsg[256];
			snprintf(chMsg, sizeof(chMsg), "NR103Device:%s UpdataValue Addrss=%02d:%02d DataType=%d Value=%f",
				GetName().c_str(), pGDHead->sGIN.byGroup, pGDHead->sGIN.byEntry, pGDHead->sGDD.byDataType, fValue);
			if (isAnyWay)
			{
				pDriver->WriteLogAnyWay(chMsg);
			}
			else
			{
				pDriver->WriteLog(chMsg);
			}
		}
	}

	OpcUa_Byte CNR103Device::ProASDU10NoDataSet( OpcUa_Byte byCOT, OpcUa_Int32 nTagAddr )
	{
		OpcUa_Byte byValue = -1;
		switch(byCOT)
		{
		case COT_CU:						//复位通信单元
		case COT_START:						//启动/重新启动
		case COT_POWERON:					//电源合上
			{
				m_bNeedASDU21GI = OpcUa_True;
				m_bIsNeedCheckT = OpcUa_True;
			}
			break;
		case COT_GENERAL_CMD_CONFIRM:		//命令的肯定认可
		case COT_COMM_WRITE_CMD_CONFIRM:	//通用分类写命令的肯定认可
		case COT_COMM_READ_CMD_VALID:		//对通用分类读命令有效数据响应
		case COT_COMM_WRITE_CONFIRM:		//通用分类写确认
			{
				byValue = 1;
				SetVal(NR103_DICB, nTagAddr, (byte*)&byValue, sizeof(OpcUa_Byte), 0, OpcUa_True);
				UaString ustrMsg(UaString("NR103Device:%1 DICB:%2 收到命令肯定认可").arg(GetName().c_str()).arg(nTagAddr));
				CNR103Driver* pDriver = (CNR103Driver*)m_pDriver;
				pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
			}
			break;
		case COT_GENERAL_CMD_DENY:			//命令的否定认可
		case COT_COMM_WRITE_CMD_DENY:		//通用分类写命令的否定认可
		case COT_COMM_READ_CMD_INVALID:		//对通用分类读命令无效数据响应
			{
				byValue = 0;
				SetVal( NR103_DICB, nTagAddr, (byte* )&byValue, sizeof(OpcUa_Byte),0, OpcUa_True );
				UaString ustrMsg(UaString("NR103Device:%1 DICB:%2 收到命令否定认可").arg(GetName().c_str()).arg(nTagAddr));
				CNR103Driver* pDriver = (CNR103Driver*)m_pDriver;
				pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
			}
			break;
		default:
			break;
		}	
		return byValue;
	}

	OpcUa_Int32 CNR103Device::ProcessASDU5Data( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount )
	{
		OpcUa_Int32 nRes = -1;
		OpcUa_Int32 nASDU5Size = sizeof(ASDU5Frame)-sizeof(FrameAPCI);
		CNR103Driver* pDriver = (CNR103Driver*)m_pDriver;
		if (pASDUFrame && nRestByteCount >= nASDU5Size && m_pDriver)
		{
			ASDU_HEAD* pHead = (ASDU_HEAD*)pASDUFrame;
			OpcUa_Byte byCOT = pHead->byCOT;
			if (COT_CU == byCOT || COT_START == byCOT || COT_POWERON == byCOT)
			{
				m_bIsNeedCheckT=OpcUa_True;
				m_bNeedASDU21GI=OpcUa_True;
			}
			UaString ustrMsg(UaString("CNR103Device:设备:%1收到ASDU5报文").arg(GetName().c_str()));
			pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessASDU6Data( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount )
	{
		OpcUa_Int32 nRes = -1;
		OpcUa_Int32 nASDU6Size = sizeof(ASDU6Frame)-sizeof(FrameAPCI);
		CNR103Driver* pDriver = (CNR103Driver*)m_pDriver;
		if (pASDUFrame && nRestByteCount >= nASDU6Size && m_pDriver)
		{
			ASDU_HEAD* pHead = (ASDU_HEAD*)pASDUFrame;
			OpcUa_Byte byCOT = pHead->byCOT;
			if (COT_TIME_SYN == byCOT)
			{
				m_bIsNeedCheckT=OpcUa_False;
				//成功校时后开始发送心跳帧和ASDU21总查询帧
				SetSendHBTag(OpcUa_True);
				SetASDU21WQ(OpcUa_True);
			}
			UaString ustrMsg(UaString("CNR103Device:设备:%1校时成功").arg(GetName().c_str()));
			pDriver->WriteLogAnyWay(ustrMsg.toUtf8());
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessASDU1FRest( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount )
	{
		OpcUa_Int32 nRes = -1;
		OpcUa_Int32 nASDU1FRestLen = sizeof(ASDU1_REST);
		CNR103Driver* pDriver = (CNR103Driver*)m_pDriver;
		if (pASDUFrame && nRestByteCount >= nASDU1FRestLen && m_pDriver)
		{
			ASDU_HEAD* pHead = (ASDU_HEAD*)pASDUFrame;
			OpcUa_Byte byCOT = pHead->byCOT;
			OpcUa_Int32 nTagAddr = 0;
			OpcUa_Byte byVal = -1;
			if (byCOT == COT_GENERAL_CMD_CONFIRM)
			{
				byVal = 1;
			}
			else if (byCOT == COT_GENERAL_CMD_DENY)
			{
				byVal = 0;
			}
			else
			{
				return nRes;
			}
			nTagAddr = GetTagAddr(pHead->byFun, pHead->byInf);
			SetVal(NR103_RESETCB, nTagAddr, &byVal, sizeof(OpcUa_Byte), 0, OpcUa_True);
			OpcUa_CharA pchMsg[256];
			snprintf(pchMsg, 256, "NR103Device:%s 收到远方复归返信 Addr Fun%02d:Inf%02d COT:%d", GetName().c_str(), pHead->byFun, pHead->byInf, byCOT);
			pDriver->WriteLogAnyWay(pchMsg);
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::ProcessASDU23Data( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount )
	{
		OpcUa_Int32 nRes = -1;
		OpcUa_Int32 nASDU23NODataLen = sizeof(ASDU_HEAD);
		CNR103Driver* pDriver = (CNR103Driver*)m_pDriver;
		if (pASDUFrame && nRestByteCount >= nASDU23NODataLen && m_pDriver)
		{
			ASDU_HEAD* pASDU23Head = (ASDU_HEAD*)pASDUFrame;
			OpcUa_Byte byDataCount = pASDUFrame[5];
			OpcUa_CharA pchMsg[256];
			snprintf(pchMsg, 256, "NR103Device:%s Revice DISORDER DATA dataCount is :%d", GetName().c_str(), byDataCount);
			pDriver->WriteLogAnyWay(pchMsg);
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::BuildAPCIFrame( FrameAPCI* pAPCI, DWORD dwFrameLen )
	{
		OpcUa_Int32 nRes = -1;
		if (pAPCI && dwFrameLen>0)
		{
			pAPCI->wFirstFlag = HEAD_FLAG;
			pAPCI->dwLength = dwFrameLen;
			pAPCI->wSecondFlag = HEAD_FLAG;
			pAPCI->wSourceFacAddr = m_wSFacAddr;
			pAPCI->wSourceDevAddr = m_wSDevAddr;
			pAPCI->wDesFacAddr = m_wDFacAddr;
			pAPCI->wDesDevAddr = m_wDDevAddr;
			pAPCI->wDataNumber = m_nDataNum++;
			pAPCI->wDevType = m_wDevType;
			pAPCI->wDevNetState = m_wDNetState;
			pAPCI->wFRouterAddr = m_wFRoutAddr;
			pAPCI->wLRouterAddr = m_wLRoutAddr;
			pAPCI->wReserve1 = RESERVE_FLAG;
			nRes = 0;
		}
		return nRes;
	}

	OpcUa_Int32 CNR103Device::BuildASDU10DOFrame( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress, OpcUa_Byte byType, OpcUa_Byte byData )
	{
		OpcUa_Int32 nDataLen = 0;
		if (pbyData)
		{
			OpcUa_Int32 nGroup = lTagAddress/1000;
			OpcUa_Int32 nEntry = lTagAddress%1000;
			if (nGroup > 255 || nEntry > 255)
			{
				return nDataLen;
			}
			ASDU10_RemoteCtrl* pFrame = (ASDU10_RemoteCtrl*)pbyData;
			nDataLen = sizeof(ASDU10_RemoteCtrl);
			ACE_OS::memset(pFrame, 0, nDataLen);
			FrameAPCI* pAPCI = (FrameAPCI*)(&pFrame->sFrameAPCI);
			ASDU10_HEAD* pHead = (ASDU10_HEAD*)(&pFrame->sASDU10H);

			
			OpcUa_Int32 nRes = BuildAPCIFrame(pAPCI, nDataLen - 8);
			pHead->sASDUHead.byType = TYP_COMM_DATA;
			pHead->sASDUHead.byVSQ = VSQ_MOST;
			pHead->sASDUHead.byCOT = COT_COMM_WRITE_CMD_CONFIRM;
			pHead->sASDUHead.byAddr = (OpcUa_Byte)m_wDDevAddr;
			pHead->sASDUHead.byFun = FUN_COMM_SORT;
			pHead->sASDUHead.byInf = byType;
			pHead->byRII = 0;
			pHead->byNGD = 1;
			pHead->byCNTU = 0;
			pHead->byCOUNT =0;
			pFrame->sGIN.byGroup = (OpcUa_Byte)nGroup;
			pFrame->sGIN.byEntry = (OpcUa_Byte)nEntry;
			pFrame->byKOD = KOD1_ACTUALVALUE;
			pFrame->sGDD.byDataType = GDD_DT9_DPI;
			pFrame->sGDD.byDataSize = 1;
			pFrame->sGDD.byDataNum = 1;
			pFrame->sGDD.byCont=0;
			pFrame->byGID = byData?2:1;
		}
		return nDataLen;
	}

	OpcUa_Int32 CNR103Device::BuildReadSFrame( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress )
	{
		OpcUa_Int32 nDataLen = 0;
		if (pbyData)
		{
			OpcUa_Int32 nGroup = lTagAddress/1000;
			OpcUa_Int32 nEntry = lTagAddress%1000;
			if (nGroup > 255 || nEntry > 255)
			{
				return nDataLen;
			}
			ASDU21ReadGroup* pFrame = (ASDU21ReadGroup*)(pbyData);
			nDataLen = sizeof(ASDU21ReadGroup);
			ACE_OS::memset(pFrame, 0, nDataLen);
			FrameAPCI* pAPCI = (FrameAPCI*)(&pFrame->sFrameAPCI);;
			ASDU10_HEAD* pHead = (ASDU10_HEAD*)(&pFrame->sASDU21H);
			
			OpcUa_Int32 nRes = BuildAPCIFrame(pAPCI, nDataLen-8);
			pHead->sASDUHead.byType = TYP_COMM_CMD;
			pHead->sASDUHead.byVSQ = VSQ_MOST;
			pHead->sASDUHead.byCOT = COT_COMM_READ_CMD_VALID;
			pHead->sASDUHead.byAddr = (OpcUa_Byte)m_wDDevAddr;
			pHead->sASDUHead.byFun = FUN_COMM_SORT;
			pHead->sASDUHead.byInf = INF_READ_ALL_ITEM;
			pHead->byRII = 0;
			pHead->byNGD = 1;
			pHead->byCOUNT = 0;
			pHead->byCNTU = 0;
			pFrame->sGIN.byGroup = (OpcUa_Byte)nGroup;
			pFrame->sGIN.byEntry = (OpcUa_Byte)nEntry;
			pFrame->byKOD = KOD1_ACTUALVALUE;

		}
		return nDataLen;
	}

	OpcUa_Int32 CNR103Device::BuildResetFrame( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress, OpcUa_Boolean bExe )
	{
		OpcUa_Int32 nDataLen = 0;
		if (pbyData)
		{
			OpcUa_Int32 nGroup = lTagAddress/1000;
			OpcUa_Int32 nEntry = lTagAddress%1000;
			if (nGroup > 255 || nEntry > 255)
			{
				return nDataLen;
			}
			ASDU10_Reset* pFrame = (ASDU10_Reset*)pbyData;
			nDataLen = sizeof(ASDU10_Reset);
			ACE_OS::memset(pFrame, 0, nDataLen);
			FrameAPCI* pAPCI = (FrameAPCI*)(&pFrame->sFrameAPCI);
			ASDU10_HEAD* pHead = (ASDU10_HEAD*)(&pFrame->sASDU10H);
			
			OpcUa_Int32 nRes = BuildAPCIFrame(pAPCI, nDataLen-8);
			pHead->sASDUHead.byType=TYP_COMM_DATA;
			pHead->sASDUHead.byVSQ=VSQ_MOST;
			pHead->sASDUHead.byCOT=COT_COMM_WRITE_CMD_CONFIRM;
			pHead->sASDUHead.byAddr=(OpcUa_Byte)m_wDDevAddr;
			pHead->sASDUHead.byFun=FUN_COMM_SORT;
			if (bExe)
			{
				pHead->sASDUHead.byInf = INF_WRITE_WITH_EXCUTE;
			}
			else
			{
				pHead->sASDUHead.byInf = INF_WRITE_WITH_CONFIRM;
			}
			pFrame->sASDU10H.byRII = 0;
			pFrame->sASDU10H.byNGD = 1;
			pFrame->sASDU10H.byCNTU = 0;
			pFrame->sASDU10H.byCOUNT = 0;
			pFrame->sGIN.byGroup = nGroup;
			pFrame->sGIN.byEntry = nEntry;
			pFrame->byKOD = KOD1_ACTUALVALUE;
			pFrame->sGDD.byDataType = GDD_DT3_UI;
			pFrame->sGDD.byDataSize=1;
			pFrame->sGDD.byDataNum=1;
			pFrame->sGDD.byCont=1;
			pFrame->byGID=1;

		}
		return nDataLen;
	}

	OpcUa_Int32 CNR103Device::BuildResetASDU20( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress )
	{
		OpcUa_Int32 nDataLen = 0;
		if (pbyData)
		{
			OpcUa_Int32 nFun = lTagAddress/1000;
			OpcUa_Int32 nInf = lTagAddress%1000;
			if (nFun > 255 || nInf > 255)
			{
				return nDataLen;
			}

			ASDU20_Rest* pFrame = (ASDU20_Rest*)pbyData;
			nDataLen = sizeof(ASDU20_Rest);
			ACE_OS::memset(pFrame, 0, nDataLen);
			FrameAPCI* pAPCI = (FrameAPCI*)(&pFrame->sFrameAPCI);
			ASDU_HEAD* pHead = (ASDU_HEAD*)(&pFrame->sHead);
			OpcUa_Int32 nRes = BuildAPCIFrame(pAPCI, nDataLen-8);
			pHead->byType = TYP_GENERAL_CMD;
			pHead->byVSQ = VSQ_MOST;
			pHead->byCOT = COT_GENERAL_CMD_CONFIRM;
			pHead->byAddr = (OpcUa_Byte)m_wDDevAddr;
			pHead->byFun = nFun;
			pHead->byInf = nInf;
			pFrame->byDCO = 0;
			pFrame->byRII = 0;
		}
		return nDataLen;
	}

	OpcUa_Int64 CNR103Device::CaculateMSNum( DAILYTIME sTime )
	{
		if ( sTime.sMonth < 1 || sTime.sMonth > 12
			|| sTime.sDay < 1 || sTime.sDay > 31
			|| sTime.sHour < 0 || sTime.sHour > 24
			|| sTime.sMinute < 0 || sTime.sMinute > 60
			|| sTime.sSecond < 0 || sTime.sSecond > 60	)
		{
			return -1;
		}
		OpcUa_Int16 sMonthDay[12]={31,28,31,30,31,30,31,31,30,31,30,31};
		OpcUa_Int32 lSumDay;
		OpcUa_Int32 i;
		//! 计算总天数
		lSumDay = 	(OpcUa_Int32)(sTime.lYear-1970) * 365 +(OpcUa_Int32)(sTime.lYear-1-1968) / 4;
		for( i = 0; i < sTime.sMonth - 1; i ++ )
			lSumDay += sMonthDay[ i ];

		if ( ( sTime.sMonth > 2 ) && ( ( sTime.lYear - 1968 ) % 4 == 0 ) )
			lSumDay += 1; 

		lSumDay += sTime.sDay -1 ;

		OpcUa_Int64 nSec = (OpcUa_Int64)lSumDay * 24 * 60 * 60
			+ (OpcUa_Int64)sTime.sHour * 60 * 60
			+ (OpcUa_Int64)sTime.sMinute * 60
			+ (OpcUa_Int64)sTime.sSecond
			+ (OpcUa_Int64)sTime.sTimeZone * 60 * 60;

		//总毫秒数
		OpcUa_Int64 nTime = nSec * 1000 + sTime.sMSecond;
		return nTime;
	}

	OpcUa_Boolean CNR103Device::IsDIValueChange(OpcUa_Byte byType, OpcUa_Int32 nTagAdrr, OpcUa_Byte byNewVal, CNR103Driver* pDriver)
	{
		OpcUa_Boolean bRes = OpcUa_False;
		FieldPointType* pFieldPoint;
		CIOCfgTag* pIOTag = GetTagByIndex(byType, nTagAdrr);
		if (pIOTag && pDriver)
		{
			pFieldPoint = pIOTag->GetFirst();
			//只有FieldPoint不处于初始状态才判断
			if (pFieldPoint && !(pFieldPoint->value(NULL).statusCode() & FieldPoint_Init))
			{
				UaVariant uaVarOld = *(pFieldPoint->value(NULL).value());
				OpcUa_Byte byOldValue;
				uaVarOld.toByte(byOldValue);
				if (byOldValue != byNewVal)
				{
					bRes = OpcUa_True;
					OpcUa_CharA pchMsg[256];
					snprintf(pchMsg, 256, "NR103Device:%s 标签 DIG%d 发生%d到%d变位", 
						GetName().c_str(), nTagAdrr, byOldValue, byNewVal);
					pDriver->WriteLogAnyWay(pchMsg);
				}
			}
		}
		return bRes;
	}

	void CNR103Device::WriteSOETime( std::string strDevName, OpcUa_Int32 nTagAddr, DAILYTIME sTime, CNR103Driver* pDriver )
	{
		if (!strDevName.empty() && pDriver)
		{
			OpcUa_Int32 nGroup = nTagAddr/1000;
			OpcUa_Int32 nItem = nTagAddr%1000;
			OpcUa_CharA pchMsg[256];
			snprintf(pchMsg, 256, "NR103Device:%s DI变位 Address=%02d:%02d Time=%02d:%02d:%02d,%03d ms",
				strDevName.c_str(), nGroup, nItem, sTime.sHour, sTime.sMinute, sTime.sSecond, sTime.sMSecond);
			pDriver->WriteLogAnyWay(pchMsg);
		}
	}

	void CNR103Device::InitDataNum( OpcUa_Int32 nDataNum )
	{
		m_nDataNum = nDataNum;
	}

	void CNR103Device::CheckUpdateVal( OpcUa_Int32 nTagType, OpcUa_Int32 nTagAddr, CNR103Driver* pDriver )
	{
		//查看更新后的值
		CIOCfgTag* pTag = GetTagByIndex(nTagType, nTagAddr);
		if (pTag && pDriver)
		{
			T_mapFieldPoints fps = pTag->GetFieldPoints();
			T_mapFieldPoints::iterator itFP = fps.begin();
			for (; itFP != fps.end(); itFP++)
			{
				FieldPointType* pFP = itFP->second;
				if (pFP)
				{
					UaVariant variant = *(pFP->value(NULL).value());
					OpcUa_Byte byVal;
					variant.toByte(byVal);
					OpcUa_CharA chMsg[256];
					snprintf(chMsg, 256, "NR103Prtcl::ProcessDT19DPI after setVal the Tag DIG%d Value is %d", nTagAddr, byVal);
					pDriver->WriteLogAnyWay(chMsg);
				}
			}
		}
	}

}



