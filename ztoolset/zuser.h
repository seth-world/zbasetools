#ifndef ZUSER_H
#define ZUSER_H

#include <zconfig.h>
#include <ztoolset/zerror.h>
#include <ztoolset/zexceptionmin.h>
//#include <ztoolset/zfunctions.h>
#include <ztoolset/zutfstrings.h>

#ifdef __GNUG__
#include <unistd.h>
#include <pwd.h>
#define SECURITY_MAX_SID_SIZE 100
#endif // __GNUG__

constexpr size_t cst_uidbin_maxlen = SECURITY_MAX_SID_SIZE;
#define __UID_GNU__ 'G'
#define __UID_WINDOWS__ 'W'



namespace zbs{

#ifdef __GNUG__
typedef uint8_t  BYTE;
typedef uint32_t DWORD;

#define ANYSIZE_ARRAY 1

#ifndef SID_IDENTIFIER_AUTHORITY_DEFINED

#define SID_IDENTIFIER_AUTHORITY_DEFINED

typedef struct _SID_IDENTIFIER_AUTHORITY {

    BYTE  Value[6];

} SID_IDENTIFIER_AUTHORITY, *PSID_IDENTIFIER_AUTHORITY;

#endif

#ifndef SID_DEFINED

#define SID_DEFINED

typedef struct _SID {

   BYTE  Revision;

   BYTE  SubAuthorityCount;

   SID_IDENTIFIER_AUTHORITY IdentifierAuthority;

#ifdef MIDL_PASS

   [size_is(SubAuthorityCount)] DWORD SubAuthority[*];

#else // MIDL_PASS

   DWORD SubAuthority[ANYSIZE_ARRAY];

#endif // MIDL_PASS

} SID, *PISID;

#endif
#endif // __GNUG__

class ZUserId
{
public:
    ZUserId()
    {
    memset(Content,0,cst_uidbin_maxlen);
    }
    ZUserId& fromZUserId(ZUserId& pId) {memmove(this,&pId,sizeof(pId)); return *this;}
    ZUserId& operator = (ZUserId& pId) {memmove(this,&pId,sizeof(pId)); return *this;}

#ifdef __USE_WINDOWS__
    ZUserId (SID pSid) {}

    ZUserId& operator = (SID pId) {memset (this,0,sizeof(ZUserId)); memmove(this,&pId,sizeof(pId)); return *this;}

    char Type=__UID_WINDOWS__;
    int8_t Content[cst_uidbin_maxlen];
    SID*   Sid = Content;
    size_t Sid_len ;
#else

    void current(void) {uid_t wuid = getuid(); memset(Content,0,sizeof(Content)); memmove(Content,&wuid,sizeof(wuid));}

//====================Linux SID Emulation================================


    ZUserId (uid_t pSid) {memmove(Content,&pSid,sizeof(pSid));}

    ZUserId& operator = (uid_t pId) { memset (this,0,sizeof(ZUserId)); memmove(this,&pId,sizeof(pId)); return *this;}

    utfdescString toString()
    {
        uid_t wuid ;
        utfdescString wReturn;
        memmove (&wuid,Content,sizeof(uid_t));
        wReturn.sprintf("%c-%d",Type,(int)wuid);
        return wReturn;
    }

    char Type=__UID_GNU__;
    int8_t Content[cst_uidbin_maxlen];
    uid_t* Sid =(uid_t*) Content;
    size_t Sid_len = sizeof(uid_t);
#endif

};

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
class ZUser
{
public:
    ZUser();
    ~ZUser();

    ZUser & setToCurrentUser(void);
    ZStatus fromName(const char* pName);

    ZStatus setToName(const char* pName);
    ZStatus setToUserId (ZUserId pUserId);

    utfidentityString   getName();
    uriString           getHomeDir();
    static uriString    getCurrentDir();
    static bool         isRoot() ;

    utfdescString          UserIdToString() {return UserId.toString();}
    utfdescString          GroupIdToString(){return GroupId.toString();}

    ZUserId& getUserId() {return UserId;}
    ZUserId& getGroupId() {return GroupId;}
/* NO NO NO
    static ZStatus authenticate(descString pUsername,descString pPassword);
#ifdef __USE_LDAP__
    static ZStatus authenticate_Ldap(descString pDN,descString pCredentials);
#endif //__USE_LDAP__
*/

    bool isInit() {return Init;}
protected:
    ZUserId UserId;
    ZUserId GroupId;
    bool Init=false;
    utfidentityString* UserName=nullptr;
    uriString*  Home=nullptr;

#ifdef __GNUG__
    struct passwd pwUserData;
#endif
};

} // namespace zbs
#endif // ZUSER_H
