/*!
 *	\file	NodeManagerCompositeImpl.cpp
 *
 *	\brief	根节点管理器实现类实现源文件
 *
 *	$Author: Wangyuanxing $
 *	$Date: 14-04-01 9:09 $
 *	$Revision: 1.0 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "NodeManagerCompositeImpl.h"
#include "RTDBBaseClasses/NodeManagerComposite.h"

// #include "uathread.h"

#if SUPPORT_XML_PARSER
// #include "xmldocument.h"
#endif

#include "utilities/ConstDefination.h"
#include "utilities/shutdown.h"
#include "RTDBNodeManager/IONodeManager.h"

#include "RTDBCommon/RuntimeCatlog.h"
#include "RTDBCommon/DataTrans.h"
#include "RTDBCommon/CommonInfoParser.h"
#include "RTDBCommon/Debug_Msg.h"
// #include "Common/LogProxy/LogProxy.h"

namespace MACS_SCADA_SUD
{
	CNodeManagerCompositeImpl::CNodeManagerCompositeImpl()
		:m_pIONodeManager(NULL)
	{
	}

	CNodeManagerCompositeImpl::~CNodeManagerCompositeImpl()
	{
		if(m_pIONodeManager)
		{
			delete m_pIONodeManager;
			m_pIONodeManager = NULL;
		}
	}

	int CNodeManagerCompositeImpl::Init()
	{
		int ret = 0;

		LoadProjectCfg();

		return ret;
	}

	int CNodeManagerCompositeImpl::UnInit()
	{
		int ret = 0;



		return ret;
	}

	int CNodeManagerCompositeImpl::StartWork()
	{
		int ret = 0;
		bool bSuccess = false;

		return ret;
	}

	int CNodeManagerCompositeImpl::StopWork()
	{
		int ret = 0;

		printf("***************************************************\n");
		printf(" Shutting down server\n");
		printf("***************************************************\n");

		return ret;
	}

	bool CNodeManagerCompositeImpl::IsStarted()
	{
		return false;
	}
	// UaNode* CNodeManagerCompositeImpl::GetNodeById(long nodeId)
	// {
	// 	CUnitNodeManager* pNodeManager = GetNodeManagerByNamespace(nodeId.namespaceIndex());
	// 	if(NULL != pNodeManager)
	// 		return pNodeManager->GetNodeById(nodeId);
	// 	else
	// 	{
	// 		NodeManagerRoot* pNodeManagerRoot = NodeManagerRoot::CreateRootNodeManager();
	// 		if(pNodeManagerRoot)
	// 			return pNodeManagerRoot->getNode(nodeId);
	// 	}

	// 	return NULL;
	// }

	// UaNode* CNodeManagerCompositeImpl::GetNodeById(std::string nodeIdstr)
	// {
	// 	long nodeId(long::fromXmlString(nodeIdstr));
	// 	return GetNodeById(nodeId);
	// }

	void CNodeManagerCompositeImpl::SetIONodeManager(CIONodeManager* pIONodeManager)
	{
		m_pIONodeManager = pIONodeManager;
	}

	CIONodeManager* CNodeManagerCompositeImpl::GetIONodeManager()
	{
		return m_pIONodeManager;
	}

	int CNodeManagerCompositeImpl::LoadProjectCfg()
	{
		int ret = 0;
		std::string path = SingletonRuntimeCatlog::instance()->GetSetupDir() + "bin/files/ProjectCfg.xml";
		CommonInfoParser commoninfo(path);
		
		if( -1 !=commoninfo.readXmlFile())
		{
			printf("Server LoadProjectCfg success\n");
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("LoadProjectCfg success!\n")));
		}
		else
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("LoadProjectCfg failed!\n")));
			return ret;
		}

		if(!m_pIONodeManager)
		{
			m_pIONodeManager  = new CIONodeManager("NodeManager");
			if(m_pIONodeManager)
			{
				std::map<std::string, T_DrvInfo*>::iterator itor = g_sDrvInfos.begin();
				int iDrvIndex = 1;
				printf("IoServer Init Driver Items\n");

				while(itor!= g_sDrvInfos.end())
				{
					T_DrvInfo* pDrvInfo = itor->second;

					std::map<std::string, tIOPortCfg*>::iterator itIOPortCfgs = g_mapIOPortCfgs.find(pDrvInfo->szChnl);
					
					if(itIOPortCfgs != g_mapIOPortCfgs.end())
					{
						m_pIONodeManager->CreatePort(pDrvInfo->szModule, *(itIOPortCfgs->second));
					}

					m_pIONodeManager->CreateUnit(pDrvInfo->szModule, pDrvInfo->szName, iDrvIndex);
					
					iDrvIndex++;

					itor++;
				}
				printf("IoServer Add Driver Items Count:%d\n", iDrvIndex);

				printf("IoServer Add TagItems\n");

				std::map<std::string, T_TagItem*>::iterator itTagItems = g_sTagItems.begin();
				long iTagIndex = 0;
				while(itTagItems != g_sTagItems.end())
				{
					T_TagItem* pTagItem = itTagItems->second;
					FieldPointType* pVar = new FieldPointType(iTagIndex);
					
					TranslateTagItem2Varient(*pTagItem, *pVar);

					m_pIONodeManager->AddTag(pTagItem->szDrvName, pTagItem->szDeviceName, pVar);

					itTagItems++;
					iTagIndex++;
				}
				printf("IoServer Add TagItems Count:%d\n", iTagIndex);
			}
		}

		return ret;
	}
	int CNodeManagerCompositeImpl::GetPointNum(std::string sTypeName,bool bChildType/* = true*/)
	{
		return 0;
	}
}
