#ifndef MYSERVERCALLBACK_H
#define MYSERVERCALLBACK_H

#include "opcserver.h"
#include "serverconfig.h"

class MyServerCallback: public UaServerApplicationCallback
{
    UA_DISABLE_COPY(MyServerCallback);
public:
    /** Construction */
    MyServerCallback();
    /** Destruction */
    virtual ~MyServerCallback();

    virtual Session* createSession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken);
    virtual UaStatus logonSessionUser(Session* pSession, UaUserIdentityToken* pUserIdentityToken, ServerConfig* pServerConfig);
    virtual void afterLoadConfiguration(ServerConfig* pServerConfig);

private:
    UaStatus verifyUserWithOS(const UaString& sUserName, const UaString& sPassword);

private:
	std::map<UaString, OpcUa_Int16> m_mapRole;
};

#endif // MYSERVERCALLBACK_H
