#include "Common/DebugMsg/Debug_Msg.h"
#include "myservercallback.h"
#include "mysession.h"
#include "nodemanagerroot.h"
#include "serverconfigdata.h"
#include <set>
#include "Common/CommonXmlParser/DeployManager.h"
#include "RTDB/Server/DBBaseClasses/EasRunStatusBase.h"
#include "Common/DataChange/DataTrans.h"
#include "Common/GlobalVariable/RunVariable.h"

using namespace MACS_SCADA_SUD;

MyServerCallback::MyServerCallback()
{
}

MyServerCallback::~MyServerCallback()
{
}

Session* MyServerCallback::createSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken)
{
    // Create the application specific session where we can store our own information during user authentication (logonSessionUser)
    // We will used this information later for user authorisation like in browse, read and write
    MySession* pSession = new MySession(sessionID, authenticationToken);
    return pSession;
}

UaStatus MyServerCallback::logonSessionUser(Session* pSession, UaUserIdentityToken* pUserIdentityToken, ServerConfig* pServerConfig)
{
	UaStatus ret;

    if ( pServerConfig == NULL )
    {
        ret = OpcUa_Bad;
    }

	OpcUa_Boolean  bEnableAnonymous;
    OpcUa_Boolean  bEnableUserPw;
    OpcUa_Boolean  bEnableCertificate;
    OpcUa_Boolean  bEnableKerberosTicket;

    pServerConfig->getUserIdentityTokenConfig(bEnableAnonymous, bEnableUserPw, bEnableCertificate, bEnableKerberosTicket);

    if ( pUserIdentityToken->getTokenType() == OpcUa_UserTokenType_Anonymous )
    {
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("logonSessionUser...............UserTokenType_Anonymous\n")));

        if ( bEnableAnonymous == OpcUa_False )
        {
            // Return error if Anonymous is not enabled
            ret = OpcUa_BadIdentityTokenRejected;
        }
        else
        {
            // Return OK but we do not set any user information in MySession
            ret = OpcUa_Good;
        }
    }
    else if ( pUserIdentityToken->getTokenType() == OpcUa_UserTokenType_UserName )
    {
		MACS_ERROR((ERROR_PERFIX ACE_TEXT("logonSessionUser...............UserTokenType_UserName\n")));

        if ( bEnableUserPw == OpcUa_False )
        {
            // Return error if User/Password is not enabled
            ret = OpcUa_BadIdentityTokenRejected;

			MACS_ERROR((ERROR_PERFIX ACE_TEXT("logonSessionUser...............UserTokenType_UserName but disable userpw BadIdentityTokenRejected\n")));
        }
        else
        {
            // Check user and password and set user related information on MySession
            UaUserIdentityTokenUserPassword* pUserPwToken = (UaUserIdentityTokenUserPassword*)pUserIdentityToken;
			UaString sName = pUserPwToken->sUserName;
			if(sName.isNull() || sName.isEmpty())
			{
				ret = OpcUa_Bad;

				MACS_ERROR((ERROR_PERFIX ACE_TEXT("logonSessionUser...............UserTokenType_UserName EnableUserPw but Empty Params\n")));

				return ret;
			}

			//! 获取客户端session的ip或者名字
			UaString sSessionName = pSession->getSessionName();
			if(sSessionName.length() < 1)
				sSessionName = UaString("UnknownSession");
			UaString sIp = pSession->getClientNetworkAddress();
			if(sIp.length() < 1)
				sIp = UaString("UnKnownIP");
			UaString sInfo = UaString("Client:%1  IP:%2").arg(sSessionName).arg(sIp);

			MACS_ERROR((ERROR_PERFIX ACE_TEXT("logonSessionUser...............Client[%s] EnableUserPw Params[%s]\n"), 
						sInfo.toUtf8(), sName.toUtf8()));

			((MySession*)pSession)->setUserIdentityToken(sName);
			
			//! 登陆连接字符串格式:用户名+角色名*工位 没有分割符的话全当角色名
			std::string sUserName;
			std::string sRoleName;
			std::string sPosName;

			ParseUserRoleAndPos(sName.toUtf8(), sUserName, sRoleName, sPosName);
            
			/*
			std::string strCxt = sName.toUtf8();
			string::size_type len = strCxt.size();
			string::size_type pos = strCxt.find('+');
			if(pos != string::npos)
			{
				sRoleName = strCxt.substr(pos + 1, len-pos-1);
								
				sUserName = strCxt.substr(0, pos);
			}
			else
			{
				//! 没有分割符的话全当角色名
				sRoleName = strCxt;
			}
			*/

			((MySession*)pSession)->setUser(UaString(sUserName.c_str()));
			((MySession*)pSession)->setRole(UaString(sRoleName.c_str()));
			((MySession*)pSession)->setPos(UaString(sPosName.c_str()));

			ret = OpcUa_Good;
        }
    }

	//! 处理角色登陆
	if (g_pEasRunStatus)
	{
		MySession* pMySession = dynamic_cast<MySession*>(pSession);
		UaString roleName = pMySession->roleName();
		if (!roleName.isNull() && !roleName.isEmpty())
		{
			g_pEasRunStatus->OnRoleLogin(roleName);
			MACS_DEBUG((DEBUG_PERFIX ACE_TEXT("MyServerCallback::logonSessionUser -- client:%s, roleName:%s connected."), 
				pSession->getClientNetworkAddress().toUtf8(), roleName.toUtf8()));
		}
	}

    return ret;
}

void MyServerCallback::afterLoadConfiguration(ServerConfig* pServerConfig)
{
	UaServerApplicationCallback::afterLoadConfiguration(pServerConfig);

	
    ServerConfigData* pServerConfigData = dynamic_cast<ServerConfigData*>(pServerConfig);
	/*
    // Make sure the demo server product information can not be overwritten by the config file
    pServerConfigData->setProductUri("urn:UnifiedAutomation:UaServerCpp");
    pServerConfigData->setManufacturerName("Unified Automation GmbH");
    pServerConfigData->setProductName("C++ SDK OPC UA Demo Server");
    pServerConfigData->setSoftwareVersion(SERVERCONFIG_SOFTWAREVERSION);
    pServerConfigData->setBuildNumber(SERVERCONFIG_BUILDNUMBER);
	*/

	//添加命名空间配置
	if ((pServerConfigData != NULL) && (g_pDeployManager != NULL))
	{
		UaString sSiteName = g_pDeployManager->GetDeploySite();
		siteIndex* pSite = g_pDeployManager->getSiteIndex(sSiteName);
		if ( NULL == pSite )
		{
			printf("getSiteIndex failed!\n");
			return ;
		}
		
		//获得当前的命名空间配置
		std::vector<DBUnit*> vecUnit;
		g_pDeployManager->GetDBUnits(sSiteName, vecUnit);

		UaUInt16Array namespaceIndexArray;
		UaStringArray namespaceUriArray;
		UaBoolArray   allowRenameUriArray;
		UaStringArray uniqueIdArray;
		UaStatus ret = pServerConfig->getConfiguredNamespaces(namespaceIndexArray, namespaceUriArray, 
			allowRenameUriArray, uniqueIdArray);
		if (!ret.isGood())
		{
			printf("getConfiguredNamespaces failed");
			return;
		}

		int iIndexSize = namespaceIndexArray.length();
		int iUriSize = namespaceUriArray.length();
		int iSize = (iIndexSize<iUriSize)? iIndexSize:iUriSize;
		std::map<OpcUa_UInt16, UaString> mapNSIndex;
		for (int i = 0; i < iSize; i++)
		{
			mapNSIndex[namespaceIndexArray[i]] = namespaceUriArray[i];
		}

		//获得站点角色
		OpcUa_Int32 iSiteRole = g_pDeployManager->GetSiteRole();
		if (iSiteRole & Identifier_Role_RTDB) //服务角色为实时库
		{
			int iUnitSize = pSite->softwareConfig.vecDBUnits.size();
			for (int i = 0; i < iUnitSize; i++)
			{
				DBUnitInfo* const pUnitInfo = &(pSite->softwareConfig.vecDBUnits[i]->unitInfo);
				UaString sUri = UaString("urn:RTDB:%1").arg(pUnitInfo->dbName);
				std::map<OpcUa_UInt16, UaString>::iterator itNS = mapNSIndex.find(pUnitInfo->ID);
				if ((itNS != mapNSIndex.end()) && (itNS->second == sUri))
				{
					continue;
				}

				AddUnitNameAndUrl(pUnitInfo->dbName, sUri);

				pServerConfigData->ServerConfigData::removeConfiguredNamespace(pUnitInfo->ID);
				pServerConfigData->ServerConfigData::addConfiguredNamespace(pUnitInfo->ID, sUri, OpcUa_False, pUnitInfo->dbName);
				mapNSIndex[pUnitInfo->ID] = sUri;
			}
		}
		else if (iSiteRole & Identifier_Role_FEP) //服务角色为独立的FEP
		{
			int iUnitSize = pSite->softwareConfig.vecFepTagUnits.size();
			for (int i = 0; i < iUnitSize; i++)
			{
				DBUnitInfo* const pUnitInfo = &(pSite->softwareConfig.vecFepTagUnits[i]);
				UaString sUri = UaString("urn:RTDB:%1").arg(pUnitInfo->dbName);
				std::map<OpcUa_UInt16, UaString>::iterator itNS = mapNSIndex.find(pUnitInfo->ID);
				if ((itNS != mapNSIndex.end()) && (itNS->second == sUri))
				{
					continue;
				}

				AddUnitNameAndUrl(pUnitInfo->dbName, sUri);

				pServerConfigData->ServerConfigData::removeConfiguredNamespace(pUnitInfo->ID);
				pServerConfigData->ServerConfigData::addConfiguredNamespace(pUnitInfo->ID, sUri, OpcUa_False, pUnitInfo->dbName);
				mapNSIndex[pUnitInfo->ID] = sUri;
			}
		}
	}
}

UaStatus MyServerCallback::verifyUserWithOS(const UaString& sUserName, const UaString& sPassword)
{
    OpcUa_ReferenceParameter(sUserName);
    OpcUa_ReferenceParameter(sPassword);
    UaStatus ret = OpcUa_BadUserAccessDenied;

    // Only Windows sample code available at the moment
#ifndef _WIN32_WCE
#ifdef _WIN32

    HANDLE userContextHandle = 0;
    UaString tempUserName;
    UaString uaDomain;
    int i;
    int iBSPos = sUserName.find("\\");
    int iAtPos = sUserName.find("@");
    if (iBSPos > 0 && iAtPos == -1)
    {
        for (i = 0; i < iBSPos; i++) {uaDomain += sUserName.at(i).data();}
        for (i = iBSPos + 1; i < sUserName.length(); i++) {tempUserName += sUserName.at(i).data();}
    }
    else if (iAtPos > 0 && iBSPos == -1)
    {
        for (i = 0; i < iAtPos; i++) {tempUserName += sUserName.at(i).data();}
        for (i = iAtPos + 1; i < sUserName.length(); i++) {uaDomain += sUserName.at(i).data();}
    }
    else
    {
        tempUserName = sUserName;
    }

#ifdef  UNICODE
    UaByteArray wsUserName = tempUserName.toUtf16();
    LPTSTR lpszUsername = (wchar_t*)(const UaUShort*)wsUserName;
    UaByteArray wsDomain = uaDomain.toUtf16();
    LPTSTR lpszDomain = (wsDomain.size() > 0) ? ((wchar_t*)(const UaUShort*)wsDomain) : NULL;
    UaByteArray wsPassword = sPassword.toUtf16();
    LPTSTR lpszPassword = (wchar_t*)(const UaUShort*)wsPassword;
	
#else   /* UNICODE */
    char *lpszUsername = (char*)tempUserName.toUtf8();
    char *lpszDomain = (uaDomain.length() > 0) ? ((char*)uaDomain.toUtf8()) : NULL;
    char *lpszPassword = (char*)sPassword.toUtf8();
#endif /* UNICODE */

    // http://msdn2.microsoft.com/en-us/library/Aa378184.aspx
    // requires include Windows.h
    // requires lib Advapi32.lib
    BOOL logOnResult = LogonUser(
        lpszUsername,
        lpszDomain,
        lpszPassword,
        LOGON32_LOGON_INTERACTIVE,     // DWORD dwLogonType,
        LOGON32_PROVIDER_DEFAULT,  // DWORD dwLogonProvider,
        &userContextHandle);

    if ( logOnResult == FALSE )
    {
        ret = OpcUa_BadUserAccessDenied;
    }
    else
    {
        CloseHandle(userContextHandle);
        ret = OpcUa_Good;
    }
#endif /* _WIN32 */
#endif /* _WIN32_WCE */

    return ret;
}
