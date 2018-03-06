/*!
 *	\file	IODrvScheduleMgrImpl.cpp
 *
 *	\brief	��������ʵ���ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��3��31��	16:21
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
// #include "Common/DBFAccess/DBFFile.h"
#include "IODriverMgr.h"
#include "../RTDBDriverFrame/IODriverFrameH.h"
#include "RTDBBaseClasses/NodeManagerComposite.h"
#include "IODrvScheduleMgrImpl.h"
#include "RTDBCommon/ClassDllManager.h"
#include "RTDBCommon/Debug_Msg.h"
#include "RTDBCommon/RuntimeCatlog.h"
#include "RTDBCommon/csvparser.h"
#include "RTDBCommon/DataTrans.h"
#include "RTDBPlatformClass/FieldPointType.h"
#include "RTDBPlatformClass/IOUnit.h"
#include "RTDBNodeManager/IONodeManager.h"
#include "../IOHelpCfgManager/IOHelpCfgManager.h"
#include "../RTDBDriverFrame/IOCfgDriverDbfParser.h"
#include "ace/Method_Request.h"
#include "ace/High_Res_Timer.h"

namespace MACS_SCADA_SUD
{
	/*!
	 *	\brief	ȱʡ���캯��
	 */
	CIODrvScheduleMgr::CIODrvScheduleMgr() 
		: m_bCanWork(false)
	{
		
	}

	/*!
	 *	\brief	ȱʡ��������
	 */
	CIODrvScheduleMgr::~CIODrvScheduleMgr()
	{
		
	}


	//!�������ȳ�ʼ��
	int CIODrvScheduleMgr::Init()
	{
		int result = -1;
		//!ע����������
		InitDriverDll();
		//!��ʼ��help.dbf��������Ϣ
		if ( NULL == g_pIOExtCfgManager )
		{
			g_pIOExtCfgManager = new CIOExtCfgManager();
		}

		int nRes = g_pIOExtCfgManager->Init();
		if (0 == nRes)
		{
			//!�ӽڵ�������õ�ͨ���ڵ�map
			CNodeManagerComposite* pNodeManager = g_pNodeManagerCompsite;
			CIONodeManager* pIONodeManager = pNodeManager->GetIONodeManager();
			if (pIONodeManager != NULL)
			{
				std::map<long , CIOPort*> mapIOChannel;
				pIONodeManager->GetAllIOPorts(mapIOChannel);
				if(mapIOChannel.size() > 0)
				{
					std::map<long , CIOPort*>::iterator itChannel;
					for (itChannel = mapIOChannel.begin(); 
						itChannel != mapIOChannel.end(); itChannel++)
					{
						CIOPort* pIOPort = itChannel->second;
						//!����IOPort��Ϣ���������������
						result = ProcessIOPort(pIOPort);
						if (result != 0)
						{
							break;
						}
					}
					//!�ÿ�ʼ������ʶ
					if (0 == result)
					{
						m_bCanWork = true;
					}
				}
				else
				{
					//!��ǰ�ڵ㲻����FEP���
					result = 0;
				}
			}
			else
			{
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::Init(): pIONodeManager  is NULL!")));
			}
		}
		else
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::Init(): g_pIOExtCfgManager->Init() failed!")));
		}
		return result;
	}

	int CIODrvScheduleMgr::StartWork()
	{
		if (m_bCanWork)
		{
			//!����������ʱ��
			ACE_High_Res_Timer::global_scale_factor();
			m_ActiveTimer.timer_queue()->gettimeofday(ACE_High_Res_Timer::gettimeofday_hr);
			m_ActiveTimer.activate();

			//!����ÿ�������������ʼ����
			T_MapDriverMgr::iterator it = m_mapDriverMgr.begin();
			while (it != m_mapDriverMgr.end())
			{
				if (it->second)
				{
					//!������ʼ����
					it->second->StartWork();
				}
				it++;
			}
		}

		return 0;
	}

	int CIODrvScheduleMgr::StopWork()
	{
		//!ֹͣ������ʱ���߳�
		m_ActiveTimer.deactivate();

		T_MapDriverMgr::iterator it = m_mapDriverMgr.begin();
		while (it != m_mapDriverMgr.end())
		{
			if (it->second)
			{
				it->second->StopWork();
			}
			it++;
		}

		return 0;
	}

	//!��������ж��
	int CIODrvScheduleMgr::UnInit()
	{
		m_bCanWork = false;
		
		T_MapDriverMgr::iterator itDrvMgr = m_mapDriverMgr.begin();
		while(itDrvMgr != m_mapDriverMgr.end())
		{
			if (itDrvMgr->second)
			{
				CIODriverMgr* pDriverMgr = itDrvMgr->second;
				pDriverMgr->UnInitDriver();
				delete pDriverMgr;
				pDriverMgr = NULL;
			}
			itDrvMgr ++;
		}
		m_mapDriverMgr.clear();

		T_MapDrvDbfParser::iterator itDrvParser = m_mapDrvDbfParser.begin();
		while(itDrvParser != m_mapDrvDbfParser.end())
		{
			if (itDrvParser->second)
			{
				CIOCfgDriverDbfParser* pDrvDbfParser = itDrvParser->second;
				delete pDrvDbfParser;
				pDrvDbfParser = NULL;
			}
			itDrvParser++;
		}
		m_mapDrvDbfParser.clear();

		if (g_pIOExtCfgManager)
		{
			delete g_pIOExtCfgManager;
			g_pIOExtCfgManager = NULL;
		}

		return 0;
	}

	//!����ң�ء�ң���������������������������
	int CIODrvScheduleMgr::AddWriteCmd(tIORemoteCtrl& remoteCmd)
	{
		int nResult = -1;
		FieldPointType* pfieldPoint = dynamic_cast<FieldPointType*>(remoteCmd.pNode);
		if (pfieldPoint)
		{
			CIODriverMgr* pDriverMgr = GetDrvMgrByFieldPoint(pfieldPoint);
			if (pDriverMgr)
			{
				nResult = pDriverMgr->AddWriteCmd(remoteCmd);
			}
		}
		
		return nResult;
	}

	//!����ͨ������������������������������
	int CIODrvScheduleMgr::AddCtrlPacketCmd(tIOCtrlPacketCmd& packetCmd)
	{
		int nResult = -1;
		CIODriverMgr* pDriverMgr = GetDriverMgrByNodeId(packetCmd.objNodeId);
		if (pDriverMgr)
		{
			nResult = pDriverMgr->AddCtrlPacketCmd(packetCmd);
		}
		return nResult;	
	}

	//!���ܹ���������·����������
	int CIODrvScheduleMgr::AddRunMgrCmd( tIORunMgrCmd& ioRunMgrCmd )
	{
		int nResult = -1;
		switch(ioRunMgrCmd.byObjectType)
		{
		case ObjectType_Channel:
			{
				CIOPort* pIOPort = dynamic_cast<CIOPort*>(ioRunMgrCmd.pNode);
				if (pIOPort)
				{
					CIODriverMgr* pDriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
					if (pDriverMgr)
					{
						nResult = pDriverMgr->AddRunMgrCmd(ioRunMgrCmd);
					}
				}
			}
			break;
		case ObjectType_Device:
			{
				CIOUnit* pIOUnit = dynamic_cast<CIOUnit*>(ioRunMgrCmd.pNode);
				if (pIOUnit)
				{
					CIOPort* pIOPort = pIOUnit->GetIOPort();
					if (pIOPort)
					{
						CIODriverMgr* pDriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
						if (pDriverMgr)
						{
							nResult = pDriverMgr->AddRunMgrCmd(ioRunMgrCmd);
						}
					}
				}
			}
			break;
		case ObjectType_Tag:
			{
				FieldPointType* pFieldPoint = dynamic_cast<FieldPointType*>(ioRunMgrCmd.pNode);
				if (pFieldPoint)
				{
					CIODriverMgr* pDriverMgr = GetDrvMgrByFieldPoint(pFieldPoint);
					if (pDriverMgr)
					{
						nResult = pDriverMgr->AddRunMgrCmd(ioRunMgrCmd);
					}
				}
			}
			break;
		default:
			break;
		}
		return nResult;	
	}

	//!������ݱ���
	int CIODrvScheduleMgr::UpdateDataVariable( FieldPointType* pFieldPoint )
	{
		int nResult = -1;
		if (pFieldPoint)
		{
			CIODriverMgr* pDriverMgr = GetDrvMgrByFieldPoint(pFieldPoint);
			if (pDriverMgr)
			{
				nResult = pDriverMgr->UpdateDataVariable(pFieldPoint);
			}
		}
		return nResult;
	}


	//!���ø���:����������ʹ�俪ʼ����
	int  CIODrvScheduleMgr::AddDriver(CIOPort* pIOPort)
	{
		int nResult = -1;
		if (pIOPort)
		{
			//!���������������
			nResult = ProcessIOPort(pIOPort);
			if (0 == nResult)
			{
				//!����������ʼ����
				CIODriverMgr* pDriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
				if (pDriverMgr)
				{
					nResult = pDriverMgr->StartWork();
				}
			}
		}
		return nResult;
	}

	//!���ø���:������ʼ����
	int CIODrvScheduleMgr::StartDriver(CIOPort* pIOPort)
	{
		int nResult = -1;
		if (pIOPort)
		{
			CIODriverMgr* pDriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
			if (pDriverMgr)
			{
				nResult = pDriverMgr->StartWork();
			}
		}
		return nResult;
	}

	//!���ø���:����ֹͣ����
	int CIODrvScheduleMgr::StopDriver(CIOPort* pIOPort)
	{
		int nResult = -1;
		if (pIOPort)
		{
			CIODriverMgr* pDriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
			if (pDriverMgr)
			{
				nResult = pDriverMgr->StopWork();
			}
		}
		return nResult;
	}

	//!���ø���:ɾ������
	int CIODrvScheduleMgr::DeleteDriver(CIOPort* pIOPort)
	{
		int nResult = -1;
		if (pIOPort)
		{
			CIODriverMgr* pDriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
			if (pDriverMgr)
			{
				nResult = pDriverMgr->DeleteDriver();
			}
		}
		return nResult;
	}

	//!��������������Ϣ
	int CIODrvScheduleMgr::UpdateDrvCfg(CIOPort* pIOPort)
	{
		int nResult = -1;
		if (pIOPort)
		{
			CIODriverMgr* pDriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
			if (pDriverMgr)
			{
				nResult = pDriverMgr->UpdateDrvCfg();
			}
		}
		return nResult;
	}

	//!����������·��¼�
	int CIODrvScheduleMgr::SendEvent(const char* pchDriverName, const char* pchUnitName)
	{
		// throw std::logic_error("The method or operation is not implemented.");
		return 0;
	}

	//!������������������.dbf�ļ�,��ʼ����Ӧ��������������Ϣ
	int CIODrvScheduleMgr::InitDriverDbfInfo(const char* pchDbfFileName, CIOCfgDriverDbfParser* pParser)
	{
		if (NULL == pchDbfFileName || NULL == pParser)
		{
			return -1;
		}

		std::string szFile = SingletonRuntimeCatlog::instance()->GetSetupDir()+"bin/FEPConfig/"+pchDbfFileName+".csv";

		ifstream hFile;
		CSVParser parser;

		try
		{
			hFile.open( szFile.c_str());
			if(!hFile.is_open())
			{
				MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::InitDriverDbfInfo() failed, Open File file=%s FAILED!"), szFile.c_str() ));
				return -1;
			}
			int iCount = 0;

			char pBuf[MAX_PATH];
			tIOCfgUnitDataDes tUnitDes;
			long lDataOffset = 0;
			std::string szFormat;
			std::string szComment;
			while(!hFile.eof())
			{
				hFile.getline(pBuf, MAX_PATH);
				iCount++;
				if(iCount < 2)
					continue;
				parser << pBuf;
				
				parser >> szFormat >> tUnitDes.iUnitType >> tUnitDes.iRawType >> tUnitDes.iBitWidth
					>> tUnitDes.iLow>> tUnitDes.iHigh>>szComment;
				if(szFormat.empty())
					continue;
				StringReplace(szFormat, ";", ",");

				strcpy(tUnitDes.pchFormat, szFormat.c_str());
				strcpy(tUnitDes.pchComment, szComment.c_str());
				//�õ���ǩ��ַ����������AI
				std::string str = tUnitDes.pchFormat;
				int nPos = -1;

				//! �ж��Ƿ����ַ������͵ı�ǩ
				nPos = (int)( str.find( "_" ) );
				if ( nPos!=-1 )
				{
					strcpy( tUnitDes.pchID, str.substr( 0, nPos+1 ).c_str() );
					tUnitDes.tBol.byType = 2;
					tUnitDes.lLower = 0;				//! ����
					tUnitDes.lUpper = tUnitDes.iHigh;	//! ����
				}
				else
				{
					//! ��������
					nPos = (int)( str.find('%') );
					if ( nPos==-1 )
					{
						strcpy( tUnitDes.pchID, tUnitDes.pchFormat );
						tUnitDes.tBol.byType = 1;	//! ����
						tUnitDes.lLower = 0;
						tUnitDes.lUpper = 0;
					}
					else
					{
						tUnitDes.tBol.byType = 0;	//!����ֵ
						strcpy( tUnitDes.pchID, str.substr( 0, nPos ).c_str() );
					}
				}

				//!���ݵ�ƫ��=ǰ�����������ݵĵ�ַ����֮��
				tUnitDes.lDataOffset = lDataOffset;

				if ( tUnitDes.tBol.byType==0 )
				{
					//!���ɲ������ʽ
					if ( pParser->MakeBolanExpress( tUnitDes.pchFormat, tUnitDes.tBol )!=0 )
					{
						MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::InitDriverDbfInfo() failed, MakeBolanExpress FAILED!") ));
						return -9;
					}

					//!�����ַ��������ֵ
					if ( pParser->CalcTagAddrBound( tUnitDes )!=0 )
					{
						MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::InitDriverDbfInfo() failed, CalcTagAddrBound FAILED!") ));
						return -10;
					}		
				}

				//!���ݵ�ƫ��=ǰ�����������ݵĵ�ַ����֮��+1
				long lTemp = 0;
				long lOff = 0;
				lTemp = ( tUnitDes.lUpper + 1 ) * tUnitDes.iBitWidth;
				lOff = lTemp % 8;
				if ( lOff > 0 )
				 	lTemp = lTemp / 8 + 1;
				else
				 	lTemp = lTemp / 8;

				lDataOffset += lTemp;

				pParser->AddDes( tUnitDes );
			}

			hFile.close();

		}
		catch (...)
		{
			MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::InitDriverDbfInfo() failed, Open File file=%s FAILED!"), szFile.c_str() ));
			return -1;
		}
		m_mapDrvDbfParser.insert( T_MapDrvDbfParser::value_type( pchDbfFileName, pParser ) );
		return 0;
	}

	//!�����������õ�������ǩ��������
	CIOCfgDriverDbfParser* CIODrvScheduleMgr::GetDbfParserByFileName(const char* pchDriverName)
	{
		if (NULL == pchDriverName)
		{
			return NULL;
		}
		T_MapDrvDbfParser::iterator it;
		if (pchDriverName)
		{
			it = m_mapDrvDbfParser.find(pchDriverName);
			if (it != m_mapDrvDbfParser.end())
			{
				return it->second;
			}
		}
		return NULL;
	}

	//!ͨ��NodeId��������������
	CIODriverMgr* CIODrvScheduleMgr::GetDriverMgrByNodeId( long nodeId )
	{
		T_MapDriverMgr::iterator it;
		it = m_mapDriverMgr.find(nodeId);
		if (it != m_mapDriverMgr.end())
		{
			return it->second;
		}
		return NULL;
	}

	CIODriverBase* CIODrvScheduleMgr::GetDriverBaseByNodeId( long nodeId )
	{

		T_MapDriverMgr::iterator it;
		it = m_mapDriverMgr.find(nodeId);
		if (it != m_mapDriverMgr.end())
		{
			CIODriverMgr* pDriverMgr = it->second;
			if (NULL != pDriverMgr)
			{
				return pDriverMgr->GetDriverBase();
			}			
		}
		return NULL;
	}

	//!�������ȳ�ʼ���Ƿ����
	bool CIODrvScheduleMgr::IsInitFinished()
	{
		return m_bCanWork;
	}

	//!ͨ��NodeId��������������ָ��
	CIODriverMgr* CIODrvScheduleMgr::GetDrvMgrByFieldPoint( FieldPointType* pFieldPoint )
	{
		CIODriverMgr* pDriverMgr = NULL;
		if (pFieldPoint)
		{
			//!�ҵ���Ӧ�����������
			CIOUnit* pIOUnit = pFieldPoint->getIOUnit();
			if (pIOUnit)
			{
				CIOPort* pIOPort = pIOUnit->GetIOPort();
				if (pIOPort)
				{
					pDriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
				}
			}
		}
		return pDriverMgr;
	}

	//����IOPort��Ϣ���������������
	int CIODrvScheduleMgr::ProcessIOPort(CIOPort* pIOPort )
	{
		int nResult = -1;
		if (NULL == pIOPort)
		{
			return nResult;
		}

		std::string uaPDrvName = pIOPort->getDrvName();
		if (uaPDrvName.empty())
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::ProcessIOPort Failed!, the IOPort:%s PrtclName is Null!"), 
				pIOPort->nodeId()));
			//������·Э������Ϊ�ձ�֤�ܼ�������������·
			nResult = 0;
			return nResult;
		}

		std::string strPortDrv = uaPDrvName;

		//!��ʼ��������ǩ��������(OPCUADriver����վ����RTDBDRIVER������������ǩ����)
		if (OPCUADRIVER != strPortDrv && RTDBDRIVER != strPortDrv)
		{
			CIOCfgDriverDbfParser* pDrvDbfParser = NULL;
			pDrvDbfParser = GetDbfParserByFileName(strPortDrv.c_str());
			if ( NULL == pDrvDbfParser )
			{
				pDrvDbfParser = new CIOCfgDriverDbfParser();
				if (NULL == pDrvDbfParser)
				{
					MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::Init(): new CIOCfgDriverDbfParser failed!")));
					nResult = 0;
					return nResult;
				}

				nResult = InitDriverDbfInfo(strPortDrv.c_str(), pDrvDbfParser);
				if (nResult != 0)
				{
					MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::Init(): InitDriverDbfInfo failed!")));
					delete pDrvDbfParser;
					pDrvDbfParser = NULL;
					return nResult;
				}
			}
		}

		//!��ʼ�������������
		CIODriverMgr* pIODriverMgr = new CIODriverMgr(this);
		if (NULL == pIODriverMgr)
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::ProcessIOPort new CIODriverMgr  failed!")));
			nResult = 0;
			return nResult;
		}

		//!����������ʼ��//
		nResult = pIODriverMgr->InitDriver(strPortDrv, pIOPort);
		if (0 == nResult)
		{
			m_mapDriverMgr.insert(T_MapDriverMgr::value_type(pIOPort->nodeId(), pIODriverMgr));
		}
		else
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::ProcessIOPort pIODriverMgr->InitDriver failed!")));
			delete pIODriverMgr;
			pIODriverMgr = NULL;
		}

		return nResult;
	}

	//!���ø���:���IOUnit
	int CIODrvScheduleMgr::AddUnit( CIOUnit* pIOUnit )
	{
		int nResult = -1;
		if (pIOUnit)
		{
			CIOPort* pIOPort = pIOUnit->GetIOPort();
			if (pIOPort)
			{
				CIODriverMgr* pIODriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
				if (pIODriverMgr)
				{
					nResult = pIODriverMgr->AddUnit(pIOUnit);
				}
			}
		}
		return nResult;
	}

	//!���ø���:ɾ��IOUnit
	int CIODrvScheduleMgr::DeleteUnit( CIOUnit* pIOUnit )
	{
		int nResult = -1;
		if (pIOUnit)
		{
			CIOPort* pIOPort = pIOUnit->GetIOPort();
			if (pIOPort)
			{
				CIODriverMgr* pIODriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
				if (pIODriverMgr)
				{
					nResult = pIODriverMgr->DeleteUnit(pIOUnit);
				}
			}
		}
		return nResult;
	}

	//!���ø���:�����豸��������Ϣ
	int CIODrvScheduleMgr::UpdateUnitCfg( CIOUnit* pIOUnit )
	{
		int nResult = -1;
		if (pIOUnit)
		{
			CIOPort* pIOPort = pIOUnit->GetIOPort();
			if (pIOPort)
			{
				CIODriverMgr* pDriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
				if (pDriverMgr)
				{
					nResult = pDriverMgr->UpdateDeviceCfg(pIOUnit);
				}
			}
		}
		return nResult;
	}
}
