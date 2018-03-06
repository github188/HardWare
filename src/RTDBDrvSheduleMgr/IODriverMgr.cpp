/*!
 *	\file	IODriverMgr.cpp
 *
 *	\brief	驱动管理类实现
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月1日	15:34
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "ace/DLL.h"
#include "ace/Timer_Heap.h"
#include "ace/High_Res_Timer.h"
 #include "RTDBCommon/Debug_Msg.h"
#include "IODriverMgr.h"
#include "IODrvStatusHandle.h"
#include "IODrvScheduleMgrImpl.h"
#include "RTDBDriverFrame/IODriverBase.h"
// #include "DBDriverFrame/IOCfgDriverDbfParser.h"
#include "RTDBPlatformClass/IOPort.h"
#include "RTDBPlatformClass/IOUnit.h"
#include "RTDBPlatformClass/FieldPointType.h"
 #include "../IOHelpCfgManager/IOHelpCfgManager.h"
#include "RTDBCommon/ClassDllManager.h"


namespace MACS_SCADA_SUD
{
	//!构造函数
	CIODriverMgr::CIODriverMgr() 
		: m_pDriverBase(NULL)
		, m_pStatusCb(NULL)
		, m_lStatusId(0)
		, m_nWatchInterval(0)
	{
		m_pIODrvScheduleMgr = NULL;
	}

	//!析构函数
	CIODriverMgr::~CIODriverMgr()
	{
		m_pIODrvScheduleMgr = NULL;
	}

	//!构造函数
	CIODriverMgr::CIODriverMgr(CIODrvScheduleMgr* pDrvScheduleImpl)
		: m_pDriverBase(NULL), m_pStatusCb(NULL), m_lStatusId(0), m_nWatchInterval(0)
	{
		m_pIODrvScheduleMgr = pDrvScheduleImpl;
	}

	//!初始化驱动
	int CIODriverMgr::InitDriver(std::string strDrvName, CIOPort* pIOPort)
	{
		int result = -1;
		if (strDrvName.empty() || NULL == pIOPort)
		{
			return result;
		}

		//!根据驱动名称加载驱动Dll和创建驱动对象
		 CClassDllManager* pClassDllMgr = SingletonDllManager::instance();
		 if (pClassDllMgr != NULL)
		 {
		 	tExtInfo* pHelpInfo = g_pIOExtCfgManager->GetPrtclInfo(strDrvName);
		 	if (pHelpInfo != NULL)
		 	{
		 		ACE_DLL* pDrvDll = pClassDllMgr->LoadDrvDll(pHelpInfo->pchFileName);
		 		if (pDrvDll != NULL)
		 		{
		 			//!使用全局Driver类工厂创建Driver对象
		 			m_pDriverBase = g_IODriverFactory.Create(pHelpInfo->pchClassName);
		 			if (m_pDriverBase != NULL)
		 			{
		 				//!为驱动对象设置CIOPoint指针
		 				m_pDriverBase->SetIOPort(pIOPort);

		 				//!进行OpenChannel操作
		 				m_pDriverBase->OpenChannel();

		 				//!初始化驱动的标签解析对象(OPCUADRIVER和子站驱动除外)
		 				if (OPCUADRIVER != strDrvName && RTDBDRIVER != strDrvName)
		 				{
		 					InitDriverDbfParser(strDrvName.c_str());
		 				}

		 				//!遍历初始化该Driver对象下的所有Device对象
		 				int nUnitCount = pIOPort->GetUnitCount();
		 				for (int i = 0; i < nUnitCount; i++)
		 				{
		 					//!NodeManager API
		 					CIOUnit* pUnit = pIOPort->GetUnitByIndex(i);
		 					if (pUnit != NULL)
		 					{
		 						m_pDriverBase->InitUnit(pUnit);
		 					}
		 				}
		 			} 
		 			else
		 			{
						MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriverMgr::InitDriver(): Create m_pDriverBase failed! DrvName:%s"), pHelpInfo->pchClassName));
		 				return result;
		 			}
					
		 			//!创建驱动状态信息定时器回调处理对象
		 			m_pStatusCb = new CIODrvStatusHandle(m_pDriverBase, g_pIOScheduleManager);
		 			if (m_pStatusCb != NULL)
		 			{
		 				//!得到时间间隔信息(NodeManage API)
		 				m_nWatchInterval = m_pDriverBase->GetStatusWatchTime();

		 				//!执行成功
		 				result = 0;
		 			}
		 			else
		 			{
		 				 MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriverMgr::InitDriver(): new CIODrvStatusHandle() failed!")));
		 			}
		 		}
		 		else
		 		{
		 			 MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriverMgr::InitDriver(): pClassDllMgr->LoadDll failed!FileName:%s "), pHelpInfo->pchFileName));
		 		}
		 	}
			else
			{
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriverMgr::InitDriver(): g_pIOExtCfgManager->GetPrtclInfo failed! DrvName: %s "), strDrvName.c_str()));
			}
		 } 
		 else
		 {
		 	 MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriverMgr::InitDriver(): pClassDllMgr is NULL!")));
		 }
		return result;
	}

	//!开始工作
	int CIODriverMgr::StartWork()
	{
		const ACE_Time_Value cur_tv = ACE_High_Res_Timer::gettimeofday_hr();
		ACE_Time_Value internal;
		long argStatus = 1;

		//!开启通道诊断定时器
		internal.msec(m_nWatchInterval);
		m_lStatusId = m_pIODrvScheduleMgr->m_ActiveTimer.schedule(m_pStatusCb, &argStatus, cur_tv + ACE_Time_Value(1L), internal);
		if (m_lStatusId > 0 && m_pDriverBase)
		{
			int nResult = m_pDriverBase->StartWork();
			if (nResult != 0)
			{
				 MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriverMgr::StarWork m_pDriverBase->StartWork Failed!, Driver:%s\n"), m_pDriverBase->GetDriverName().c_str()));
				return -1;
			} 
		} 
		else
		{
			 MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriverMgr::StarWork Schedule STATUS Timer Failed!, Driver:%s\n"), m_pDriverBase->GetDriverName().c_str()));
			return -1;
		}

		return 0;
	}

	//!驱动停止工作
	int CIODriverMgr::StopWork()
	{
		//!停止通道诊断定时器
		if (m_lStatusId)
		{
			m_pIODrvScheduleMgr->m_ActiveTimer.cancel(m_lStatusId);
		}

		
		if (m_pDriverBase)
		{
			//!驱动停止工作
			m_pDriverBase->StopWork();
		}

		return 0;
	}

	//!卸载驱动
	int CIODriverMgr::UnInitDriver()
	{
		if (m_pStatusCb)
		{
			delete m_pStatusCb;
			m_pStatusCb = NULL;
		}

		if (m_pDriverBase)
		{
			m_pDriverBase->CloseChannel();

			delete m_pDriverBase;
			m_pDriverBase = NULL;
		}
		return 0;
	}

	//!处理遥控,遥调命令
	int CIODriverMgr::AddWriteCmd(tIORemoteCtrl& tRemoteCmd )
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->WriteDCB(tRemoteCmd);
		}
		return nResult;
	}

	//!处理通道命令
	int CIODriverMgr::AddCtrlPacketCmd(tIOCtrlPacketCmd& tPacketCmd)
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->WritePacket(tPacketCmd);
		}
		return nResult;
	}

	//!添加运行管理命令
	int CIODriverMgr::AddRunMgrCmd( tIORunMgrCmd& tRunCmd )
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->WriteMgr(tRunCmd);
		}
		return nResult;
	}

	//!设置通道对象指针,将通道对象指针赋给具体的驱动对象
	void CIODriverMgr::SetChannelPrt(CIOPort* pIOPort)
	{
		if (pIOPort != NULL && m_pDriverBase != NULL)
		{
			m_pDriverBase->SetIOPort(pIOPort);
		}
	}

	//!设置具体驱动对象指针
	void CIODriverMgr::SetDrvPrt(CIODriverBase* pDriverBase)
	{
		if (pDriverBase != NULL)
		{
			m_pDriverBase = pDriverBase;
		}
	}

	//!得到驱动的DBF解析对象
	int CIODriverMgr::InitDriverDbfParser(const char* pchDriverName)
	{
		int nResult = -1;
		if (NULL == pchDriverName)
		{
			return nResult;
		}

		CIOCfgDriverDbfParser* pDrvDbfParser = m_pIODrvScheduleMgr->GetDbfParserByFileName(pchDriverName);
		if (pDrvDbfParser != NULL)
		{
			m_pDriverBase->SetDbfParser(pDrvDbfParser);
			nResult = 0;
		}
		else
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIODriverMgr::InitDriverDbfParser(): m_pIODrvScheduleMgrImpl->GetDbfParserByFileName failed! Driver:%s"), pchDriverName));
		}
		return nResult;
	}

	//!配置更新:添加数据变量和处理FieldPoint标签地址更新
	int CIODriverMgr::UpdateDataVariable( FieldPointType* pFieldPoint )
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->UpdateDataVariable(pFieldPoint);
		}
		return nResult;
	}

	//!配置更新:删除驱动
	int CIODriverMgr::DeleteDriver()
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->BlockDriver();
		}
		return nResult;
	}

	//!更新驱动配置信息
	int CIODriverMgr::UpdateDrvCfg()
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->UpdateDrvCfg();
		}
		return nResult;
	}

	//!配置更新:添加设备
	int CIODriverMgr::AddUnit( CIOUnit* pIOUnit )
	{
		int nResult = -1;
		if (pIOUnit)
		{
			if (m_pDriverBase)
			{
				nResult = m_pDriverBase->InitUnit(pIOUnit);
			}
		}
		return nResult;
	}

	//!配置更新:删除设备
	int CIODriverMgr::DeleteUnit( CIOUnit* pIOUnit )
	{
		int nResult = -1;
		if (pIOUnit)
		{
			if (m_pDriverBase)
			{
				nResult = m_pDriverBase->BlockDevice(pIOUnit);
			}
		}
		return nResult;
	}

	//!配置更新:更新设备配置信息
	int CIODriverMgr::UpdateDeviceCfg( CIOUnit* pIOUnit )
	{
		int nResult = -1;
		if (pIOUnit)
		{
			if (m_pDriverBase)
			{
				nResult = m_pDriverBase->UpdateDeviceCfg(pIOUnit);
			}
		}
		return nResult;
	}

	CIODriverBase* CIODriverMgr::GetDriverBase()
	{
         return m_pDriverBase;
	}

	int CIODriverMgr::ProcessHotSim()
	{
		int nRes = 0;
		if (m_pDriverBase)
		{
			nRes = m_pDriverBase->ProcessHotSim();
		}
		else
		{
			nRes = -1;
		}
		return nRes;
	}
}
