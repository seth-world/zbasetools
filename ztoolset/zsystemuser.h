#ifndef ZUSER_H
#define ZUSER_H

#include <zconfig.h>

#include <ztoolset/userid.h>

#include <ztoolset/zerror.h>
#include <ztoolset/zexceptionmin.h>
//#include <ztoolset/zfunctions.h>
#include <ztoolset/zutfstrings.h>

#include <zio/zdir.h>

namespace zbs {

/**
 * @brief The ZUser class
 *      - set user as current user
 *      - set user as registrated user locally or thru ldap
 *
 *      - get user name
 *      - get user identification ZUserId
 *      - get user home directory
 * Authenticate user :
 *      - locally as a system user
 *      - thru ldap
 */



class ZSystemUser
{
public:
    ZSystemUser();
    ~ZSystemUser();
    ZSystemUser(const ZSystemUser &pIn) { _copyFrom(pIn); }
    ZSystemUser(const ZSystemUser &&pIn) { _copyFrom(pIn); }

    ZSystemUser &operator=(const ZSystemUser &pIn) { return _copyFrom(pIn); }
    ZSystemUser &operator=(const ZSystemUser &&pIn) { return _copyFrom(pIn); }

    ZSystemUser &_copyFrom(const ZSystemUser &pIn);


    void clear();

    ZSystemUser & setToCurrentUser(void);
    ZStatus fromName(const char* pName);

    ZStatus setToName(const char* pName);
    ZStatus setToUserId (ZSystemUserId pUserId);

    utf8String    getSystemName() const;
    ZDir          getHomeDir() const;

    static ZDir   getCurrentDir();

    bool          isRoot() const ;
    utfcodeString UserIdToString()  {return SystemUserId.toString();}
    utfcodeString GroupIdToString() {return SystemGroupId.toString();}

    ZSystemUserId getUserId() const {return SystemUserId;}
    ZSystemUserId getGroupId() const {return SystemGroupId;}


    static ZSystemUser getUser(const ZSystemUserId &pUid);

/* NO NO NO
    static ZStatus authenticate(descString pUsername,descString pPassword);
#ifdef __USE_LDAP__
    static ZStatus authenticate_Ldap(descString pDN,descString pCredentials);
#endif //__USE_LDAP__
*/

    bool isInit() const {return Init;}
protected:
    ZSystemUserId SystemUserId;
    ZSystemUserId SystemGroupId;
    bool Init=false;
    utf8String SystemUserName;
    ZDir  Home;

#ifdef __GNUG__
    struct passwd pwUserData;
#endif
};

} // namespace zbs
#endif // ZUSER_H
