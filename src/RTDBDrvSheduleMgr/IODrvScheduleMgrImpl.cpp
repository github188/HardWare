/*!
 *	\file	IODrvScheduleMgrImpl.cpp
 *
 *	\brief	驱动调度实现文件
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年3月31日	16:21
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
	 *	\brief	缺省构造函数
	 */
	CIODrvScheduleMgr::CIODrvScheduleMgr() 
		: m_bCanWork(false)
	{
		
	}

	/*!
	 *	\brief	缺省析构函数
	 */
	CIODrvScheduleMgr::~CIODrvScheduleMgr()
	{
		
	}


	//!驱动调度初始化
	int CIODrvScheduleMgr::Init()
	{
		int result = -1;
		//!注册驱动基类
		InitDriverDll();
		//!初始化help.dbf中配置信息
		if ( NULL == g_pIOExtCfgManager )
		{
			g_pIOExtCfgManager = new CIOExtCfgManager();
		}

		int nRes = g_pIOExtCfgManager->Init();
		if (0 == nRes)
		{
			//!从节点管理器得到通道节点map
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
						//!根据IOPort信息创建驱动管理对象
						result = ProcessIOPort(pIOPort);
						if (result != 0)
						{
							break;
						}
					}
					//!置开始工作标识
					if (0 == result)
					{
						m_bCanWork = true;
					}
				}
				else
				{
					//!当前节点不运行FEP情况
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
			//!激活主动定时器
			ACE_High_Res_Timer::global_scale_factor();
			m_ActiveTimer.timer_queue()->gettimeofday(ACE_High_Res_Timer::gettimeofday_hr);
			m_ActiveTimer.activate();

			//!遍历每个驱动管理对象开始工作
			T_MapDriverMgr::iterator it = m_mapDriverMgr.begin();
			while (it != m_mapDriverMgr.end())
			{
				if (it->second)
				{
					//!驱动开始工作
					it->second->StartWork();
				}
				it++;
			}
		}

		return 0;
	}

	int CIODrvScheduleMgr::StopWork()
	{
		//!停止主动定时器线程
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

	//!驱动调度卸载
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

	//!接受遥控、遥调命令，并将命令加入驱动命令队列
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

	//!接受通道诊断命令，并将命令加入驱动命令队列
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

	//!接受管理命令处理，下发给驱动框架
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

	//!添加数据变量
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


	//!配置更新:新增驱动并使其开始工作
	int  CIODrvScheduleMgr::AddDriver(CIOPort* pIOPort)
	{
		int nResult = -1;
		if (pIOPort)
		{
			//!创建驱动管理对象
			nResult = ProcessIOPort(pIOPort);
			if (0 == nResult)
			{
				//!新增驱动开始工作
				CIODriverMgr* pDriverMgr = GetDriverMgrByNodeId(pIOPort->nodeId());
				if (pDriverMgr)
				{
					nResult = pDriverMgr->StartWork();
				}
			}
		}
		return nResult;
	}

	//!配置更新:驱动开始工作
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

	//!配置更新:驱动停止工作
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

	//!配置更新:删除驱动
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

	//!更新驱动配置信息
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

	//!向驱动框架下发事件
	int CIODrvScheduleMgr::SendEvent(const char* pchDriverName, const char* pchUnitName)
	{
		// throw std::logic_error("The method or operation is not implemented.");
		return 0;
	}

	//!根据驱动名解析驱动.dbf文件,初始化相应驱动解析对象信息
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
				//得到标签地址的描述，如AI
				std::string str = tUnitDes.pchFormat;
				int nPos = -1;

				//! 判断是否是字符串类型的标签
				nPos = (int)( str.find( "_" ) );
				if ( nPos!=-1 )
				{
					strcpy( tUnitDes.pchID, str.substr( 0, nPos+1 ).c_str() );
					tUnitDes.tBol.byType = 2;
					tUnitDes.lLower = 0;				//! 下限
					tUnitDes.lUpper = tUnitDes.iHigh;	//! 上限
				}
				else
				{
					//! 其他类型
					nPos = (int)( str.find('%') );
					if ( nPos==-1 )
					{
						strcpy( tUnitDes.pchID, tUnitDes.pchFormat );
						tUnitDes.tBol.byType = 1;	//! 常量
						tUnitDes.lLower = 0;
						tUnitDes.lUpper = 0;
					}
					else
					{
						tUnitDes.tBol.byType = 0;	//!　数值
						strcpy( tUnitDes.pchID, str.substr( 0, nPos ).c_str() );
					}
				}

				//!数据的偏移=前几种类型数据的地址上限之和
				tUnitDes.lDataOffset = lDataOffset;

				if ( tUnitDes.tBol.byType==0 )
				{
					//!生成波兰表达式
					if ( pParser->MakeBolanExpress( tUnitDes.pchFormat, tUnitDes.tBol )!=0 )
					{
						MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::InitDriverDbfInfo() failed, MakeBolanExpress FAILED!") ));
						return -9;
					}

					//!计算地址的上下限值
					if ( pParser->CalcTagAddrBound( tUnitDes )!=0 )
					{
						MACS_ERROR(( ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::InitDriverDbfInfo() failed, CalcTagAddrBound FAILED!") ));
						return -10;
					}		
				}

				//!数据的偏移=前几种类型数据的地址上限之和+1
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

	//!根据驱动名得到驱动标签解析对象
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

	//!通过NodeId获得驱动管理对象
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

	//!驱动调度初始化是否完成
	bool CIODrvScheduleMgr::IsInitFinished()
	{
		return m_bCanWork;
	}

	//!通过NodeId获得驱动管理对象指针
	CIODriverMgr* CIODrvScheduleMgr::GetDrvMgrByFieldPoint( FieldPointType* pFieldPoint )
	{
		CIODriverMgr* pDriverMgr = NULL;
		if (pFieldPoint)
		{
			//!找到对应驱动管理对象
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

	//根据IOPort信息创建驱动管理对象
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
			//运行链路协议名称为空保证能加载其他正常链路
			nResult = 0;
			return nResult;
		}

		std::string strPortDrv = uaPDrvName;

		//!初始化驱动标签解析对象(OPCUADriver和子站驱动RTDBDRIVER不进行驱动标签解析)
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

		//!初始化驱动管理对象
		CIODriverMgr* pIODriverMgr = new CIODriverMgr(this);
		if (NULL == pIODriverMgr)
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODrvScheduleMgr::ProcessIOPort new CIODriverMgr  failed!")));
			nResult = 0;
			return nResult;
		}

		//!进行驱动初始化//
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

	//!配置更新:添加IOUnit
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

	//!配置更新:删除IOUnit
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

	//!配置更新:更新设备的配置信息
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
