/*!
 *	\file	IODriverMgr.cpp
 *
 *	\brief	����������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��1��	15:34
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
	//!���캯��
	CIODriverMgr::CIODriverMgr() 
		: m_pDriverBase(NULL)
		, m_pStatusCb(NULL)
		, m_lStatusId(0)
		, m_nWatchInterval(0)
	{
		m_pIODrvScheduleMgr = NULL;
	}

	//!��������
	CIODriverMgr::~CIODriverMgr()
	{
		m_pIODrvScheduleMgr = NULL;
	}

	//!���캯��
	CIODriverMgr::CIODriverMgr(CIODrvScheduleMgr* pDrvScheduleImpl)
		: m_pDriverBase(NULL), m_pStatusCb(NULL), m_lStatusId(0), m_nWatchInterval(0)
	{
		m_pIODrvScheduleMgr = pDrvScheduleImpl;
	}

	//!��ʼ������
	int CIODriverMgr::InitDriver(std::string strDrvName, CIOPort* pIOPort)
	{
		int result = -1;
		if (strDrvName.empty() || NULL == pIOPort)
		{
			return result;
		}

		//!�����������Ƽ�������Dll�ʹ�����������
		 CClassDllManager* pClassDllMgr = SingletonDllManager::instance();
		 if (pClassDllMgr != NULL)
		 {
		 	tExtInfo* pHelpInfo = g_pIOExtCfgManager->GetPrtclInfo(strDrvName);
		 	if (pHelpInfo != NULL)
		 	{
		 		ACE_DLL* pDrvDll = pClassDllMgr->LoadDrvDll(pHelpInfo->pchFileName);
		 		if (pDrvDll != NULL)
		 		{
		 			//!ʹ��ȫ��Driver�๤������Driver����
		 			m_pDriverBase = g_IODriverFactory.Create(pHelpInfo->pchClassName);
		 			if (m_pDriverBase != NULL)
		 			{
		 				//!Ϊ������������CIOPointָ��
		 				m_pDriverBase->SetIOPort(pIOPort);

		 				//!����OpenChannel����
		 				m_pDriverBase->OpenChannel();

		 				//!��ʼ�������ı�ǩ��������(OPCUADRIVER����վ��������)
		 				if (OPCUADRIVER != strDrvName && RTDBDRIVER != strDrvName)
		 				{
		 					InitDriverDbfParser(strDrvName.c_str());
		 				}

		 				//!������ʼ����Driver�����µ�����Device����
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
					
		 			//!��������״̬��Ϣ��ʱ���ص��������
		 			m_pStatusCb = new CIODrvStatusHandle(m_pDriverBase, g_pIOScheduleManager);
		 			if (m_pStatusCb != NULL)
		 			{
		 				//!�õ�ʱ������Ϣ(NodeManage API)
		 				m_nWatchInterval = m_pDriverBase->GetStatusWatchTime();

		 				//!ִ�гɹ�
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

	//!��ʼ����
	int CIODriverMgr::StartWork()
	{
		const ACE_Time_Value cur_tv = ACE_High_Res_Timer::gettimeofday_hr();
		ACE_Time_Value internal;
		long argStatus = 1;

		//!����ͨ����϶�ʱ��
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

	//!����ֹͣ����
	int CIODriverMgr::StopWork()
	{
		//!ֹͣͨ����϶�ʱ��
		if (m_lStatusId)
		{
			m_pIODrvScheduleMgr->m_ActiveTimer.cancel(m_lStatusId);
		}

		
		if (m_pDriverBase)
		{
			//!����ֹͣ����
			m_pDriverBase->StopWork();
		}

		return 0;
	}

	//!ж������
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

	//!����ң��,ң������
	int CIODriverMgr::AddWriteCmd(tIORemoteCtrl& tRemoteCmd )
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->WriteDCB(tRemoteCmd);
		}
		return nResult;
	}

	//!����ͨ������
	int CIODriverMgr::AddCtrlPacketCmd(tIOCtrlPacketCmd& tPacketCmd)
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->WritePacket(tPacketCmd);
		}
		return nResult;
	}

	//!������й�������
	int CIODriverMgr::AddRunMgrCmd( tIORunMgrCmd& tRunCmd )
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->WriteMgr(tRunCmd);
		}
		return nResult;
	}

	//!����ͨ������ָ��,��ͨ������ָ�븳���������������
	void CIODriverMgr::SetChannelPrt(CIOPort* pIOPort)
	{
		if (pIOPort != NULL && m_pDriverBase != NULL)
		{
			m_pDriverBase->SetIOPort(pIOPort);
		}
	}

	//!���þ�����������ָ��
	void CIODriverMgr::SetDrvPrt(CIODriverBase* pDriverBase)
	{
		if (pDriverBase != NULL)
		{
			m_pDriverBase = pDriverBase;
		}
	}

	//!�õ�������DBF��������
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

	//!���ø���:������ݱ����ʹ���FieldPoint��ǩ��ַ����
	int CIODriverMgr::UpdateDataVariable( FieldPointType* pFieldPoint )
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->UpdateDataVariable(pFieldPoint);
		}
		return nResult;
	}

	//!���ø���:ɾ������
	int CIODriverMgr::DeleteDriver()
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->BlockDriver();
		}
		return nResult;
	}

	//!��������������Ϣ
	int CIODriverMgr::UpdateDrvCfg()
	{
		int nResult = -1;
		if (m_pDriverBase)
		{
			nResult = m_pDriverBase->UpdateDrvCfg();
		}
		return nResult;
	}

	//!���ø���:����豸
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

	//!���ø���:ɾ���豸
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

	//!���ø���:�����豸������Ϣ
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
