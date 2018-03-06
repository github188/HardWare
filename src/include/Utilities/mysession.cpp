#include "mysession.h"

/** construction.
 *  @param sessionID Session Id created by the server application.
 *  @param authenticationToken Secret session Id created by the server application.
 */
MySession::MySession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken)
: UaSession(sessionID, authenticationToken),m_inUserGroup(MySession::UserGroups_Anonymous),
m_bJSCallFlag(OpcUa_False),m_bTransferFlag(OpcUa_False)
{
}

/** destruction */
MySession::~MySession()
{
}

void MySession::setUserGroup(UserGroups inUserGroup)
{
    m_inUserGroup = inUserGroup;
}

void MySession::setUser(const UaString& sUserName)
{
    m_sUserName = sUserName;
}

void MySession::setRole(const UaString& sRoleName)
{
	m_sRoleName = sRoleName;
}

void MySession::setPos(const UaString& sPosName)
{
	m_sPosName = sPosName;
}

void MySession::setJSCallFlag(const OpcUa_Boolean& bJSCallFlag)
{
	m_bJSCallFlag = bJSCallFlag;
}

void MySession::setTransferFlag(const OpcUa_Boolean& bTransferFlag)
{
	m_bTransferFlag = bTransferFlag;
}

void MySession::setUserIdentityToken(UaString strUserIdentityToken)
{
	m_UserIdentityToken = strUserIdentityToken;
}