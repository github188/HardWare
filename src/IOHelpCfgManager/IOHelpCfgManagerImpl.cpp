/*!
 *	\file	IOHelpCfgManagerImpl.cpp
 *
 *	\brief	Help.dbf�ļ���Ϣ����ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��3��	9:30
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

	//! ��ʼ�������������ļ�help.dbf����������������
	int CIOExtCfgImpl::Init()
	{
		//��ȡƽ̨(V4)help.dbf�����ļ��������ȡ�ɹ����ܼ�����������
		std::string szHelpFile = GetFepInstallPath() + "Help.csv";
		int rcd = readHelpCfg(szHelpFile);
		if (0 != rcd)
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgImpl::Init call readHelpPdf file %s failed, return code = %d!\n"), szHelpFile.c_str(), rcd));
			return rcd;
		}

		//��ȡ��Ʒ(M4)help.dbf�����ļ���������ļ��ɹ�(�ļ�����)�����Ƕ�ȡ����ʧ�����˳���������������������ļ�������Ҳ���Լ�����
		//std::string szHelpFileAddDev = GetFepInstallPath() + "Help_AppDev.dbf";
		//rcd = readHelpCfg(szHelpFileAddDev);
		//if (rcd > 1)
		//{
		//	MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOExtCfgImpl::Init call readHelpPdf file %s failed, return code = %d!\n"), szHelpFile.c_str(), rcd));
		//	return rcd;
		//}

		//! ��ʼ����ϣ��ÿ��Կ�ʼ�����ı�־
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
				//! ע�⣺����ȡ�����ݲ�������Ч��У�飬��Ϊ�϶�����Ч�ġ�
				tExtInfo extInfo;
				strcpy( extInfo.pchFileName, szFileName.c_str() );
				strcpy( extInfo.pchClassName, szClassName.c_str() );

					//! �������ͼ��벻ͬ����������
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
	//! ��ʼ����
	int CIOExtCfgImpl::StartWork()
	{
		return 0;
	}

	//! �Ƿ���Կ�ʼ����
	bool CIOExtCfgImpl::CanWork()
	{
		return m_bCanWork;
	}

	//! ֹͣ����
	int CIOExtCfgImpl::StopWork()
	{
		return 0;
	}

	//! ж��
	int CIOExtCfgImpl::UnInit()
	{
		m_bCanWork = false;
		m_mapPrtclIndex.clear();
		m_mapBoardIndex.clear();
		m_mapAlgIndex.clear();
		m_mapServiceIndex.clear();
		return 0;
	}

	//! ��ȡЭ�����չ��Ϣ
	tExtInfo* CIOExtCfgImpl::GetPrtclInfo( std::string szPrtclName )
	{
		//! �Ƚ�����ת��Ϊ��д��,��Ϊ���е�����Ϊ��д��.
		StrToUpper( szPrtclName);

		tMapStrExtInfo::iterator it;
		it = m_mapPrtclIndex.find( szPrtclName );
		if ( it!=m_mapPrtclIndex.end() )
			return &(it->second);

		return NULL;
	}

	//! ��ȡ�������չ��Ϣ
	tExtInfo* CIOExtCfgImpl::GetBoardInfo( std::string szBoardName )
	{
		//! �Ƚ�����ת��Ϊ��д��,��Ϊ���е�����Ϊ��д��.
		StrToUpper( szBoardName );

		tMapStrExtInfo::iterator it;
		it = m_mapBoardIndex.find( szBoardName );
		if ( it!=m_mapBoardIndex.end() )
			return &(it->second);
		return NULL;
	}

	//! ��ȡ�����л��㷨����չ��Ϣ
	tExtInfo* CIOExtCfgImpl::GetSwitchAlgInfo( std::string szAlgName )
	{
		//! �Ƚ�����ת��Ϊ��д��,��Ϊ���е�����Ϊ��д��.
		StrToUpper( szAlgName );

		tMapStrExtInfo::iterator it;
		it = m_mapAlgIndex.find( szAlgName );
		if ( it!=m_mapAlgIndex.end() )
			return &(it->second);
		return NULL;
	}

	//! ��ȡ����������չ��Ϣ
	tExtInfo* CIOExtCfgImpl::GetServiceInfo( std::string szServiceName )
	{
		//! �Ƚ�����ת��Ϊ��д��,��Ϊ���е�����Ϊ��д��.
		StrToUpper( szServiceName );

		tMapStrExtInfo::iterator it;
		it = m_mapServiceIndex.find( szServiceName );
		if ( it!=m_mapServiceIndex.end() )
			return &(it->second);
		return NULL;
	}

	//! ��ȡ��װĿ¼��Ϣ�������װĿ¼/bin/FEPConfig
	std::string CIOExtCfgImpl::GetFepInstallPath()
	{
		return MACS_SCADA_SUD::SingletonRuntimeCatlog::instance()->GetSetupDir() + "bin/FEPConfig/";
	}

	//! ��ȡ����Ŀ¼��Ϣ������Ŀ¼/FEP/Start/
	std::string CIOExtCfgImpl::GetFepStartPath()
	{
		return MACS_SCADA_SUD::SingletonRuntimeCatlog::instance()->GetFep() + "Start/";
	}

	//! ��ȡ�ȱ���Ŀ¼��Ϣ������Ŀ¼/FEP/hot/
	std::string CIOExtCfgImpl::GetFepHotPath()
	{
		return MACS_SCADA_SUD::SingletonRuntimeCatlog::instance()->GetFep() + "Hot/";
	}
}