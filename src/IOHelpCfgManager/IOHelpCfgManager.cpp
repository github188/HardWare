/*!
 *	\file	IOHelpCfgManager.cpp
 *
 *	\brief	Help.dbf信息管理实现
 *
 *	详细的文件说明（可选）
 *
 *	\author 
 *
 *	\date	2014年4月3日	9:05
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "ace/Singleton.h"
#include "IOHelpCfgManager.h"
#include "IOHelpCfgManagerImpl.h"
//#include "RTDBCommon/CommonXmlParser/DeployManager.h"
#include "RTDBCommon/RuntimeCatlog.h"
#include "RTDBCommon/Debug_Msg.h"

namespace MACS_SCADA_SUD
{
	IOHELPCFGMANAGER_API CIOExtCfgManager* g_pIOExtCfgManager = NULL;

	CIOExtCfgManager::CIOExtCfgManager():m_pImpl(NULL),m_fepCommCfg(NULL)/*,m_uaDriverCfg(NULL)*/
	{
		if (NULL == m_pImpl)
		{
			m_pImpl = new CIOExtCfgImpl();
		}
		if (NULL == m_fepCommCfg)
		{
			m_fepCommCfg = new tCommDriverCfg();
		}	
		//if (NULL == m_uaDriverCfg)
		//{
		//	m_uaDriverCfg = new tUADriverCfg();
		//}
	}

	CIOExtCfgManager::~CIOExtCfgManager()
	{
		if ( m_pImpl )
		{
			delete m_pImpl;
			m_pImpl = NULL;
		}
		if (m_fepCommCfg)
		{
			delete m_fepCommCfg;
			m_fepCommCfg = NULL;
		}
		//if (m_uaDriverCfg)
		//{
		//	delete m_uaDriverCfg;
		//	m_uaDriverCfg = NULL;
		//}
	}

	//! 初始化：加载配置文件help.dbf，创建各个索引表
	int CIOExtCfgManager::Init()
	{
		int rcd = SetFepDecCommDrvCfg();
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::Init -> call SetFepDecCommDrvCfg return %d."), rcd));

		//int iSiteRole = g_pDeployManager->GetSiteRole();
		//if (iSiteRole & Identifier_Role_RTDB)
		//{
		//	rcd = SetUADrvCfg();
		//	MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::Init -> call SetUADrvCfg return %d."), rcd));
		//}

		return m_pImpl->Init();
	}

	tCommDriverCfg* CIOExtCfgManager::GetFepDecCommDrvCfg()
	{
		return m_fepCommCfg;
	}

	//!获取驱动公共信息配置文件名称
	std::string CIOExtCfgManager::GetFepCommDrvDir()
	{
		return SingletonRuntimeCatlog::instance()->GetRunDir() + "bin/FEPConfig/";
	}

	void CIOExtCfgManager::TransforSingleChar(std::string &srcString, char oldChar, char newChar)
	{
		int StrLenth = srcString.length();
		for (int i = 0; i < StrLenth; i++)
		{
			if (oldChar == srcString[i])
			{
				srcString[i] = newChar;
			}
		}
	}

	int CIOExtCfgManager::SetFepDecCommDrvCfg()
	{
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::SetFepDecCommDrvCfg -> read CommDriver.ini")));

		if (NULL == m_fepCommCfg)
		{
			return -1;
		}
		
		int status;
		ACE_Configuration_Heap cf;
		status = cf.open();
		if (status != 0)
		{
			return -2;
		}
		ACE_Ini_ImpExp import(cf);
		std::string file_name = GetFepCommDrvDir() + MACS_COMM_DRIVER_FILENAME;
		status = import.import_config(file_name.c_str());
		if (status != 0)
		{
			return -3;
		}

		//!---------------获取FEP_Common配置信息---------------
		const ACE_Configuration_Section_Key& root = cf.root_section();
		ACE_Configuration_Section_Key DrvSection;
		status = cf.open_section(root, "FEP_Common", 0, DrvSection);
		if (status != 0)
		{	
			return -4;
		}

		int nValue = 0;
		std::string strValue = "";
		ACE_TString strV;

		//!驱动日志文件大小
		status = cf.get_string_value(DrvSection, ACE_TEXT("LogFileSize"), strV);
		if (0 == status)
		{
			sscanf(strV.c_str(), "%d", &nValue);
			m_fepCommCfg->LogFileSize = nValue;
		}
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::SetFepDecCommDrvCfg LogFileSize : %d"), m_fepCommCfg->LogFileSize));

		//!是否打开通信监视开关
		status = cf.get_string_value(DrvSection, ACE_TEXT("CommuMonitorFlag"), strV);
		if (0 == status)
		{
			sscanf(strV.c_str(), "%d", &nValue);
			if (1 == nValue)
			{
				m_fepCommCfg->CommuMonitorFlag = true;
			}	
		}
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::SetFepDecCommDrvCfg CommuMonitorFlag : %d"), m_fepCommCfg->CommuMonitorFlag));

		//!驱动日志文件保存天数
		status = cf.get_string_value(DrvSection, ACE_TEXT("LogFileStorDays"), strV);
		if (0 == status)
		{
			sscanf(strV.c_str(), "%d", &nValue);
			m_fepCommCfg->LogFileStorDays = nValue;
		}
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::SetFepDecCommDrvCfg LogFileStorDays : %d"), m_fepCommCfg->LogFileStorDays));

		//!驱动日志文件路径
		if (0 < m_fepCommCfg->LogFileStorDays)
		{
			status = cf.get_string_value(DrvSection, ACE_TEXT("LogFilePath"), strV);
			if (0 == status)
			{
				strValue = strV.c_str();
				m_fepCommCfg->LogFilePath = strValue;
			}

			TransforSingleChar(m_fepCommCfg->LogFilePath, '\\','/');
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::SetFepDecCommDrvCfg LogFileStorDays : %s"), m_fepCommCfg->LogFilePath.c_str()));
		}

		//!串口驱动中使用的设备前缀字符串,仅linux下使用
		status = cf.get_string_value(DrvSection, ACE_TEXT("SrlBoardPrefix"), strV);
		if (0 == status)
		{
			strValue = strV.c_str();
			m_fepCommCfg->SrlBoardPrefix = strValue;
		}
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::SetFepDecCommDrvCfg SrlBoardPrefix : %s"), m_fepCommCfg->SrlBoardPrefix.c_str()));

		//!FEP是否向实时服务发送事件
		status = cf.get_string_value(DrvSection, ACE_TEXT("IsNeedSendEvent"), strV);
		if (0 == status)
		{
			sscanf(strV.c_str(), "%d", &nValue);
			if (1 == nValue)
			{
				m_fepCommCfg->IsSendEvent2RT = true;
			}
		}
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::SetFepDecCommDrvCfg IsSendEvent2RT : %d"), m_fepCommCfg->IsSendEvent2RT));


		//!---------------获取子站驱动配置信息---------------
		status = cf.open_section( root, "IOServer", 0, DrvSection );
		if( status!=0) 
		{
			return -4;
		}

		status = cf.get_string_value( DrvSection, "IOSub104NeedLog", strV );
		if (0 == status)
		{
			sscanf(strV.c_str(), "%d", &nValue);
			if (1 == nValue)
			{
				m_fepCommCfg->IOSub104NeedLog = true;
			}	
		}
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::SetFepDecCommDrvCfg IOSub104NeedLog : %d"), m_fepCommCfg->IOSub104NeedLog));

		status = cf.get_string_value( DrvSection, "IOSub104NeedComm", strV );
		if (0 == status)
		{
			sscanf(strV.c_str(), "%d", &nValue);
			if (1 == nValue)
			{
				m_fepCommCfg->IOSub104NeedComm = true;
			}	
		}
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::SetFepDecCommDrvCfg IOSub104NeedComm : %d"), m_fepCommCfg->IOSub104NeedComm));
		return 0;
	}

	int CIOExtCfgManager::SetUADrvCfg()
	{
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgManager::SetUADrvCfg -> This site has RTDB role so start read MACS-SCADA-RTDB.ini for UaDriver")));

		//if (NULL == m_uaDriverCfg)
		//{
		//	return -1;
		//}

		int status;
		ACE_Configuration_Heap cf;
		status = cf.open();
		if (status != 0)
		{
			return -2;
		}
		ACE_Ini_ImpExp import(cf);
		std::string file_name = SingletonRuntimeCatlog::instance()->GetSetupDir() + "bin/RTDBConfig/MACS-SCADA-RTDB.ini";
		status = import.import_config(file_name.c_str());
		if (status != 0)
		{
			return -3;
		}

		//!OPCUADriver
		const ACE_Configuration_Section_Key& root = cf.root_section();
		ACE_Configuration_Section_Key DrvSection;
		status = cf.open_section(root, "OPCUADriver", 0, DrvSection);
		if (status != 0)
		{	
			return -4;
		}

		int nValue;
		std::string strValue;
		ACE_TString strV;

		//!QueueSize大小//
		status = cf.get_string_value(DrvSection, ACE_TEXT("QueueSize"), strV);
		//if (0 == status)
		//{
		//	sscanf(strV.c_str(), "%d", &nValue);
		//	m_uaDriverCfg->QueueSize = nValue;
		//}
		//MACS_ERROR((ERROR_PERFIX ACE_TEXT("m_uaDriverCfg ...... %d"), m_uaDriverCfg->QueueSize));

		////!UADriver是否发送事件
		//status = cf.get_string_value(DrvSection, ACE_TEXT("IsNeedSendFEPBadEvent"), strV);
		//if (0 == status)
		//{
		//	sscanf(strV.c_str(), "%d", &nValue);
		//	m_uaDriverCfg->IsSendFepBadEvent = nValue;
		//}
		//MACS_ERROR((ERROR_PERFIX ACE_TEXT("m_uaDriverCfg ...... %d"), m_uaDriverCfg->IsSendFepBadEvent));

		return 0;
	}

	//tUADriverCfg* CIOExtCfgManager::GetUADrvCfg()
	//{
	//	return m_uaDriverCfg;
	//}

	//! 开始工作
	int CIOExtCfgManager::StartWork()
	{
		return m_pImpl->StartWork();
	}

	//! 是否可以开始工作
	bool CIOExtCfgManager::CanWork()
	{
		return m_pImpl->CanWork();
	}

	//! 停止工作
	int CIOExtCfgManager::StopWork()
	{
		return m_pImpl->StopWork();
	}

	//! 卸载
	int CIOExtCfgManager::UnInit()
	{
		return m_pImpl->UnInit();
	}

	//! 获取协议的扩展信息
	tExtInfo* CIOExtCfgManager::GetPrtclInfo( std::string szPrtclName )
	{
		return m_pImpl->GetPrtclInfo( szPrtclName );
	}

	//! 获取主板的扩展信息
	tExtInfo* CIOExtCfgManager::GetBoardInfo( std::string szBoardName )
	{
		return m_pImpl->GetBoardInfo( szBoardName );
	}

	//! 获取主从切换算法的扩展信息
	tExtInfo* CIOExtCfgManager::GetSwitchAlgInfo( std::string szAlgName )
	{
		return m_pImpl->GetSwitchAlgInfo( szAlgName );
	}

	//! 获取对外服务的扩展信息
	tExtInfo* CIOExtCfgManager::GetServiceInfo( std::string szServiceName )
	{
		return m_pImpl->GetServiceInfo( szServiceName );
	}

	//! 获取安装目录信息：软件安装目录/Bin/FEPConfig/
	std::string CIOExtCfgManager::GetFepInstallPath()
	{
		return m_pImpl->GetFepInstallPath();
	}

	//! 获取运行目录信息：运行目录/FEP/Start/
	std::string CIOExtCfgManager::GetFepStartPath()
	{
		return m_pImpl->GetFepStartPath();
	}

	//! 获取热备份目录信息：运行目录/FEP/hot/
	std::string CIOExtCfgManager::GetFepHotPath()
	{
		return m_pImpl->GetFepHotPath();
	}

	bool CIOExtCfgManager::GetIsSendEvent2RT()
	{
		if (m_fepCommCfg)
		{
			return m_fepCommCfg->IsSendEvent2RT;
		}
		return false;
	}

	bool CIOExtCfgManager::GetIsUADrvSendEvent()
	{
		//if (m_uaDriverCfg)
		//{
		//	return m_uaDriverCfg->IsSendFepBadEvent;
		//}
		return false;
	}
}