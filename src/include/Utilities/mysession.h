#ifndef MYSESSION_H
#define MYSESSION_H

#include "uaserver/uasession.h"
#include "userdatabase.h"

class MySession: public UaSession
{
    UA_DISABLE_COPY(MySession);
protected:
    /** Prohibit use of default constructor */
    MySession();
    /** Prohibit use of default destructor. Session object is reference counted*/
    virtual ~MySession();

public:
    enum UserGroups
    {
        UserGroups_Anonymous = 0,
        UserGroups_Operators = 1,
        UserGroups_Admin     = 2,
        UserGroups_Owner     = 3
    };

    MySession(OpcUa_Int32 sessionID, const UaNodeId &authenticationToken);

    void setUserGroup(UserGroups inUserGroup);
    inline UserGroups inUserGroup() const {return m_inUserGroup;}

    void setUser(const UaString& sUserName);
    inline UaString userName() const {return m_sUserName;}

	void setRole(const UaString& sRoleName);
	inline UaString roleName() const {return m_sRoleName;}

	void setPos(const UaString& sPosName);
	inline UaString posName() const {return m_sPosName;}

	void setJSCallFlag(const OpcUa_Boolean& bJSCallFlag);
	inline OpcUa_Boolean getJSCallFlag() const {return m_bJSCallFlag;}

	void setTransferFlag(const OpcUa_Boolean& bTransferFlag);
	inline OpcUa_Boolean getTransferFlag() const {return m_bTransferFlag;}

	void setUserIdentityToken(UaString strUserIdentityToken);
	inline UaString getUserIdentityToken() const {return m_UserIdentityToken;}

private:
    UserGroups	m_inUserGroup;
    UaString	m_sUserName;
	UaString	m_sRoleName;
	UaString	m_sPosName;
	OpcUa_Boolean m_bJSCallFlag;        //!是否是JS接口调用时创建的mysession
	OpcUa_Boolean m_bTransferFlag;      //!是否要做命令复示

	UaString m_UserIdentityToken;		//! 客户端报警类型

};

/** User data class used to limit access to variables that require user authentication
 */
class AuthorizationUserData : public UserDataBase
{
    UA_DISABLE_COPY(AuthorizationUserData);
public:
    AuthorizationUserData(
        UaString              userName,
        MySession::UserGroups userGroup,
        OpcUa_Byte            accessLevelOtherUsers = 0)
    : m_userName(userName),
      m_userGroup(userGroup),
      m_accessLevelOtherUsers(accessLevelOtherUsers)
    {}
    virtual ~AuthorizationUserData(){}

    /** Indicates if this is a state variable. */
    inline UaString userName() const { return m_userName; }
    /** Returns the device address. */
    inline MySession::UserGroups userGroup() const { return m_userGroup; }
    /** Returns the access level for not authorized users. */
    inline OpcUa_Byte accessLevelOtherUsers() const { return m_accessLevelOtherUsers; }

private:
    UaString               m_userName;
    MySession::UserGroups  m_userGroup;
    OpcUa_Byte             m_accessLevelOtherUsers;
};


#endif // MYSESSION_H

