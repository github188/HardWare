/*!
 *	\file	IOHelpCfgManagerImpl.cpp
 *
 *	\brief	Help.dbf文件信息管理实现
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月3日	9:30
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#include "ace/Singleton.h"
#include "IOHelpCfgManagerImpl.h"
#include "RTDBCommon/RuntimeCatlog.h"
#include "RTDBCommon/csvparser.h"
//#include "Common/DBFAccess/DBFFile.h"
#include "IOHelpCfgManager.h"
#include "RTDBCommon/Debug_Msg.h"
#include <fstream>
#include "RTDBCommon/DataTrans.h"

namespace MACS_SCADA_SUD
{
	CIOExtCfgImpl::CIOExtCfgImpl(void) : m_bCanWork( false )
	{
	}

	CIOExtCfgImpl::~CIOExtCfgImpl(void)
	{
	}

	//! 初始化：加载配置文件help.dbf，创建各个索引表
	int CIOExtCfgImpl::Init()
	{
		//读取平台(V4)help.dbf配置文件，必须读取成功才能继续其他操作
		std::string szHelpFile = GetFepInstallPath() + "Help.csv";
		int rcd = readHelpCfg(szHelpFile);
		if (0 != rcd)
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgImpl::Init call readHelpPdf file %s failed, return code = %d!\n"), szHelpFile.c_str(), rcd));
			return rcd;
		}

		//读取产品(M4)help.dbf配置文件，如果打开文件成功(文件存在)，但是读取数据失败则退出，否则继续其他操作（文件不存在也可以继续）
		//std::string szHelpFileAddDev = GetFepInstallPath() + "Help_AppDev.dbf";
		//rcd = readHelpCfg(szHelpFileAddDev);
		//if (rcd > 1)
		//{
		//	MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgImpl::Init call readHelpPdf file %s failed, return code = %d!\n"), szHelpFile.c_str(), rcd));
		//	return rcd;
		//}

		//! 初始化完毕，置可以开始工作的标志
		m_bCanWork = true;

		return 0;
	}
	int CIOExtCfgImpl::readHelpCfg(const std::string &szHelpFile)
	{
		ifstream hFile;
		CSVParser parser;
		
		try
		{
			hFile.open( szHelpFile.c_str());
			
			int iCount = 0;
			std::string szType, szName, szClassName, szFileName;

			char pBuf[MAX_PATH];

			while(!hFile.eof())
			{
				hFile.getline(pBuf, MAX_PATH);
				iCount++;
				if(iCount < 2)
					continue;
				parser << pBuf;
				parser >> szType >> szName >> szClassName >> szFileName;
				if(szType.empty() || szName.empty() || szClassName.empty() || szFileName.empty())
					continue;

				StrToUpper( szName);
				//! 注意：这里取出内容不进行有效性校验，认为肯定是有效的。
				tExtInfo extInfo;
				strcpy( extInfo.pchFileName, szFileName.c_str() );
				strcpy( extInfo.pchClassName, szClassName.c_str() );

					//! 根据类型加入不同的索引表中
				if ( ACE_OS::strncasecmp( szType.c_str(), "protocol", szType.length() )==0 )
				{
					m_mapPrtclIndex.insert( tMapStrExtInfo::value_type( szName, extInfo ) );
				}
				else if ( ACE_OS::strncasecmp( szType.c_str(), "boardtype", szType.length() )==0 )
				{
					m_mapBoardIndex.insert( tMapStrExtInfo::value_type( szName, extInfo ) );
				}
				else if ( ACE_OS::strncasecmp( szType.c_str(), "switchalgtype", szType.length() )==0 )
				{
					m_mapAlgIndex.insert( tMapStrExtInfo::value_type( szName, extInfo ) );
				}
				else if ( ACE_OS::strncasecmp( szType.c_str(), "servicetype", szType.length() )==0 )
				{
					m_mapServiceIndex.insert( tMapStrExtInfo::value_type( szName, extInfo ) );
				}
			}

		}
		catch (...)
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgImpl::readHelpCfg the file %s is not exist or open failed.\n"), szHelpFile.c_str()));
			return 1;
		}
		
		return 0;
	}
	//! 开始工作
	int CIOExtCfgImpl::StartWork()
	{
		return 0;
	}

	//! 是否可以开始工作
	bool CIOExtCfgImpl::CanWork()
	{
		return m_bCanWork;
	}

	//! 停止工作
	int CIOExtCfgImpl::StopWork()
	{
		return 0;
	}

	//! 卸载
	int CIOExtCfgImpl::UnInit()
	{
		m_bCanWork = false;
		m_mapPrtclIndex.clear();
		m_mapBoardIndex.clear();
		m_mapAlgIndex.clear();
		m_mapServiceIndex.clear();
		return 0;
	}

	//! 获取协议的扩展信息
	tExtInfo* CIOExtCfgImpl::GetPrtclInfo( std::string szPrtclName )
	{
		//! 先将名称转换为大写的,因为表中的名称为大写的.
		StrToUpper( szPrtclName);

		tMapStrExtInfo::iterator it;
		it = m_mapPrtclIndex.find( szPrtclName );
		if ( it!=m_mapPrtclIndex.end() )
			return &(it->second);

		return NULL;
	}

	//! 获取主板的扩展信息
	tExtInfo* CIOExtCfgImpl::GetBoardInfo( std::string szBoardName )
	{
		//! 先将名称转换为大写的,因为表中的名称为大写的.
		StrToUpper( szBoardName );

		tMapStrExtInfo::iterator it;
		it = m_mapBoardIndex.find( szBoardName );
		if ( it!=m_mapBoardIndex.end() )
			return &(it->second);
		return NULL;
	}

	//! 获取主从切换算法的扩展信息
	tExtInfo* CIOExtCfgImpl::GetSwitchAlgInfo( std::string szAlgName )
	{
		//! 先将名称转换为大写的,因为表中的名称为大写的.
		StrToUpper( szAlgName );

		tMapStrExtInfo::iterator it;
		it = m_mapAlgIndex.find( szAlgName );
		if ( it!=m_mapAlgIndex.end() )
			return &(it->second);
		return NULL;
	}

	//! 获取对外服务的扩展信息
	tExtInfo* CIOExtCfgImpl::GetServiceInfo( std::string szServiceName )
	{
		//! 先将名称转换为大写的,因为表中的名称为大写的.
		StrToUpper( szServiceName );

		tMapStrExtInfo::iterator it;
		it = m_mapServiceIndex.find( szServiceName );
		if ( it!=m_mapServiceIndex.end() )
			return &(it->second);
		return NULL;
	}

	//! 获取安装目录信息：软件安装目录/bin/FEPConfig
	std::string CIOExtCfgImpl::GetFepInstallPath()
	{
		return MACS_SCADA_SUD::SingletonRuntimeCatlog::instance()->GetSetupDir() + "bin/FEPConfig/";
	}

	//! 获取运行目录信息：运行目录/FEP/Start/
	std::string CIOExtCfgImpl::GetFepStartPath()
	{
		return MACS_SCADA_SUD::SingletonRuntimeCatlog::instance()->GetFep() + "Start/";
	}

	//! 获取热备份目录信息：运行目录/FEP/hot/
	std::string CIOExtCfgImpl::GetFepHotPath()
	{
		return MACS_SCADA_SUD::SingletonRuntimeCatlog::instance()->GetFep() + "Hot/";
	}
}