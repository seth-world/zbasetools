#ifndef ZUSER_CPP
#define ZUSER_CPP
#include <ztoolset/zsystemuser.h>
#include <stdio.h>                  /* defines FILENAME_MAX */
#include <ztoolset/uristring.h>
#include <zio/zdir.h>

using namespace zbs;

ZSystemUser::ZSystemUser()
{
 memset (&SystemUserId.content,0,cst_uidbin_maxlen);
 memset (&SystemGroupId.content,0,cst_uidbin_maxlen);
 Init=false;
}
ZSystemUser::~ZSystemUser()
{
/*    if (UserName)
            delete UserName;
    if (Home)
            delete Home;
*/
}

#ifdef __USE_WINDOWS__
#include <windows.h>
#else
#include <unistd.h>
#include <pwd.h>
#include <shadow.h>

#endif


ZSystemUser&
ZSystemUser::setToCurrentUser(void)
{
ZStatus wSt;
//    uid_t wuid = getuid();
    ZSystemUserId wUserId;
    wUserId.current();
    /*
    memmove(UserId.Sid,&wuid,sizeof(wuid));
    pwUserData = getpwuid(wuid);
    */
    wSt=setToUserId(wUserId);
    if (wSt!=ZS_SUCCESS)
        {
        ZException.addToLast(" while setting current user id ");
        ZException.exit_abort();
        }
    Init=true;
    return *this;
}

ZStatus
ZSystemUser::setToName(const char* pName)
{
int wRet;
char wBuffer[500];
struct passwd*  wUsercontent_ptr=nullptr;
    errno=0;
    wRet = getpwnam_r(pName,&pwUserData,wBuffer,500,&wUsercontent_ptr);
    if (wRet!=0)
        {
        ZException.getErrno(wRet,
                            _GET_FUNCTION_NAME_,
                            ZS_FILEERROR,
                            Severity_Fatal,
                            "Invalid user management files. getpwnam_r is not successfull. Aborting.");

         ZException.exit_abort();
         }
    if (wUsercontent_ptr==nullptr)
         {
            ZException.getErrno(errno,
                           _GET_FUNCTION_NAME_,
                           ZS_AUTHREJECTED,
                           Severity_Error,
                           "Invalid username <%s>. Not found in system users base",pName);
            if (__ZNETVERBOSE__)
                    fprintf (stderr,"%s>> Invalid username <%s>. Not found in system users base\n",
                             _GET_FUNCTION_NAME_,
                             pName);
            return ZS_AUTHREJECTED;
         } // nullptr
    uid_t wuid = getuid();
    memmove(SystemUserId.Sid,&wuid,sizeof(wuid));
    errno=0;
    Init=true;
}

ZStatus
ZSystemUser::setToUserId(ZSystemUserId pUserId)
{
struct passwd* wUsercontent_ptr=nullptr;
uid_t wuid;
int wRet=0;
char wBuffer[500];

    memmove(&wuid,pUserId.Sid,sizeof(wuid));
    errno=0;
    wRet = getpwuid_r(wuid,&pwUserData,wBuffer,sizeof(wBuffer),&wUsercontent_ptr);
    if (wRet!=0)
        {
        ZException.getErrno(wRet,
                            _GET_FUNCTION_NAME_,
                            ZS_FILEERROR,
                            Severity_Fatal,
                            "Error while accessing user management files. getpwnam_r is not successfull. Aborting.");

         ZException.exit_abort();
         }
    if (wUsercontent_ptr==nullptr)
         {
            ZException.getErrno(errno,
                           _GET_FUNCTION_NAME_,
                           ZS_AUTHREJECTED,
                           Severity_Error,
                           "Invalid uid <%d>. Not found in system users base",(int)wuid);
            if (__ZNETVERBOSE__)
                    fprintf (stderr,"%s>> Invalid uid <%d>. Not found in system users base\n",
                             _GET_FUNCTION_NAME_,
                             (int)wuid);
            return ZS_AUTHREJECTED;
         } // nullptr
//    if (UserName!=nullptr)
//                    delete UserName;
         /*    UserName=new utfidentityString((const utf8_t*)pwUserData.pw_name);
    if (Home!=nullptr)
                    delete Home;
    Home=new uriString(pwUserData.pw_dir);
*/
    SystemUserName = pwUserData.pw_name;
    Home=pwUserData.pw_dir;
    SystemUserId = pUserId;
    SystemGroupId = (uid_t)pwUserData.pw_gid;

    Init=true;
    return ZS_SUCCESS;
}
utf8String
ZSystemUser::getSystemName() const
{
    if (!isInit())
        fprintf(stderr,
                "ZSystemUser::getHomeDir-E-NOTINIT System user has not been set to a user while "
                "querying user name.\n");
 //   if (!isInit())
 //       setToCurrentUser();
/*        {

        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVVALUE,
                              Severity_Fatal,
                              " ZUser has not been initialized with a user content before being used");
        ZException.exit_abort();
        }
    if (UserName==nullptr)
            {
            delete UserName;
            }
    UserName =new utfidentityString((const utf8_t*)pwUserData.pw_name);*/

    return SystemUserName;
}//getName

#ifdef __USE_WINDOWS__
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif //__USE_WINDOWS__

ZDir ZSystemUser::getHomeDir() const
{
    if (!isInit())
        fprintf(stderr,
                "ZSystemUser::getHomeDir-E-NOTINIT System user has not been set to a user while "
                "querying home directory.\n");
//        setToCurrentUser();
/*        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVVALUE,
                              Severity_Fatal,
                              " ZUser has not been initialized with a user content before being used");
        ZException.exit_abort();
        }
    if (Home==nullptr)
            {
            delete Home;
            }
    Home =new uriString(pwUserData.pw_dir);*/
    return Home;
}//getHomeDir


ZDir ZSystemUser::getCurrentDir( )
{
 char wCurrentPath[FILENAME_MAX];
 ZDir wCurrentDir;
 if (!GetCurrentDir(wCurrentPath, sizeof(wCurrentPath)))
     {
     ZException.getErrno(-1,
                         _GET_FUNCTION_NAME_,
                         ZS_SYSTEMERROR,
                         Severity_Fatal,
                         "System error while getting current directory"
                         );
     ZException.exit_abort();
     }
 wCurrentPath[sizeof(wCurrentPath) - 1] = '\0'; /* not really required */
 wCurrentDir = wCurrentPath;

 return wCurrentDir;
}

/*
descString
ZUser::UserIdToString()
{
    if (!isInit())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVVALUE,
                              Severity_Fatal,
                              " ZUser has not been initialized with a user content before being used");
        ZException.exit_abort();
        }
    UserId.toString();
}
descString
ZUser::UserIdToString()
{
    if (!isInit())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVVALUE,
                              Severity_Fatal,
                              " ZUser has not been initialized with a user content before being used");
        ZException.exit_abort();
        }
    UserId.toString();
}
descString
ZUser::GroupIdToString()
{
    if (!isInit())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVVALUE,
                              Severity_Fatal,
                              " ZUser has not been initialized with a user content before being used");
        ZException.exit_abort();
        }

}
*/
#ifdef __USE_LINUX__

bool ZSystemUser::isRoot() const
{
    return getuid()==0;
}

#endif

uint64_t formatAuthority (_SID_IDENTIFIER_AUTHORITY pAuthority)
{
uint64_t wAuthority_value=0;
int wz = 0;

    for (wz=sizeof(pAuthority.Value)-1;wz>-1;wz--)
                        wAuthority_value += pAuthority.Value[wz]* (256^(5-wz));
    return wAuthority_value;
}

ZStatus
SIDtoString(_SID *pSID,utfdescString** pSIDString_ptr)
{

int wRevision = (int)pSID->Revision;
int SubAuthCount=(int) pSID->SubAuthorityCount;
_SID_IDENTIFIER_AUTHORITY wAuthority = pSID->IdentifierAuthority;
int wz = 0;
char wBuf[50];
utfdescString wSIDRet;
    if ((wRevision!=1)||(SubAuthCount>10))
        {
        *pSIDString_ptr = nullptr;
        return ZS_MALFORMED;
        }

// authority is a struct of 6 bytes msb[0] to lsb[5]

    (*pSIDString_ptr)->sprintf("S-%lld",(int)wRevision,formatAuthority(wAuthority)); // authority is a struct of 6 bytes msb to lsb
    while (wz<SubAuthCount)
        {
        memset(wBuf,0,sizeof(wBuf));
        snprintf(wBuf,50,"-%d",(int)pSID->SubAuthority[wz]);     // SubAuthority is a DWORD (int32_t)
        wSIDRet += wBuf;
        wz++;
        }
    (*pSIDString_ptr)->fromUtf(wSIDRet.toUtf());
    return ZS_SUCCESS;
}

#ifdef __USE_WINDOWS__
//---must be linked with Shell32.lib - calls Shell32.dll------
#include <Windows.h>

#include <Shlobj.h>

bool ZUser::isRoot()
/*++
Routine Description: This routine returns TRUE if the caller's
process is a member of the Administrators local group. Caller is NOT
expected to be impersonating anyone and is expected to be able to
open its own process and process token.
Arguments: None.
Return Value:
   TRUE - Caller has Administrators local group.
   FALSE - Caller does not have Administrators local group. --
*/
{
BOOL b;
SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
PSID AdministratorsGroup;
b = AllocateAndInitializeSid(
    &NtAuthority,
    2,
    SECURITY_BUILTIN_DOMAIN_RID,
    DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0,
    &AdministratorsGroup);
if (!b)
{
    ZException.getLastError(_GET_FUNCTION_NAME_,
                            ZS_SYSTEMERROR,
                            Severity_Fatal,
                            "AllocateAndInitializeSid error : cannot parse user SID");
    ZException.exit_abort();
}
if(b)
{
/*    if (!CheckTokenMembership( NULL, AdministratorsGroup, &b))
    {
         b = FALSE;
    }
*/
    b= CheckTokenMembership( NULL, AdministratorsGroup, &b);
    FreeSid(AdministratorsGroup);
}

return((bool)b);
}
/**
 * @brief _getLastWindowsError  returns the last windows error code and description string
 * @param[out] pErr          returned last error code
 * @param[out] pMessage      char buffer
 * @param[in]  pAvailableSize available size for the message to be written
 */
void
_getLastWindowsError (int &pErr,char* pMessage,size_t pAvailableSize)
{
DWORD wErrorid = ::GetLastError();
    pErr=wErrorid;
    LPSTR messageBuffer = nullptr;
    size_t wSize = FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                  NULL,
                                   wErrorid,
                                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                   (LPSTR)&pMessage,
                                   pAvailableSize,
                                   NULL);
    pMessage[wSize]='\0';
    return;
} // _getLastWindowsError

//===================getuid geteuid emulation====================================

#include <sddl.h>

#include <iostream>
#include <iomanip>
#include <memory>

struct heap_delete
{
    typedef LPVOID pointer;
    void operator()(LPVOID p)
    {
        ::HeapFree(::GetProcessHeap(), 0, p);
    }
};
typedef std::unique_ptr<LPVOID, heap_delete> heap_unique_ptr;

struct handle_delete
{
    typedef HANDLE pointer;
    void operator()(HANDLE p)
    {
        ::CloseHandle(p);
    }
};
typedef std::unique_ptr<HANDLE, handle_delete> handle_unique_ptr;


BOOL GetUserSID(HANDLE token, PSID* sid)
{
    if (
        token == nullptr || token == INVALID_HANDLE_VALUE
        || sid == nullptr
        )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    DWORD tokenInformationLength = 0;
    ::GetTokenInformation(
        token, TokenUser, nullptr, 0, &tokenInformationLength);
    if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        return FALSE;
    }
    heap_unique_ptr data(
        ::HeapAlloc(
            ::GetProcessHeap(), HEAP_ZERO_MEMORY,
            tokenInformationLength));
    if (data.get() == nullptr)
    {
        return FALSE;
    }
    BOOL getTokenInfo = ::GetTokenInformation(
        token, TokenUser, data.get(),
        tokenInformationLength, &tokenInformationLength);
    if (! getTokenInfo)
    {
        return FALSE;
    }
    PTOKEN_USER pTokenUser = (PTOKEN_USER)(data.get());
    DWORD sidLength = ::GetLengthSid(pTokenUser->User.Sid);
    heap_unique_ptr sidPtr(
        ::HeapAlloc(
            GetProcessHeap(), HEAP_ZERO_MEMORY, sidLength));
    PSID sidL = (PSID)(sidPtr.get());
    if (sidL == nullptr)
    {
        return FALSE;
    }
    BOOL copySid = ::CopySid(sidLength, sidL, pTokenUser->User.Sid);
    if (! copySid)
    {
        return FALSE;
    }
    if (!IsValidSid(sidL))
    {
        return FALSE;
    }
    *sid = sidL;
    sidPtr.release();
    return TRUE;
}


uid_t GetUID(HANDLE token)
{
    PSID sid = nullptr;
    BOOL getSID = GetUserSID(token, &sid);
    if (! getSID || ! sid)
    {
        return -1;
    }
    heap_unique_ptr sidPtr((LPVOID)(sid));
    LPWSTR stringSid = nullptr;
    BOOL convertSid = ::ConvertSidToStringSidW(
        sid, &stringSid);
    if (! convertSid)
    {
        return -1;
    }
    uid_t ret = -1;
    LPCWSTR p = ::wcsrchr(stringSid, L'-');
    if (p && ::iswdigit(p[1]))
    {
        ++p;
        ret = ::_wtoi(p);
    }
    ::LocalFree(stringSid);
    return ret;
}//GetUID
uid_t getuid()
{
    HANDLE process = ::GetCurrentProcess();
    handle_unique_ptr processPtr(process);
    HANDLE token = nullptr;
    BOOL openToken = ::OpenProcessToken(
        process, TOKEN_READ|TOKEN_QUERY_SOURCE, &token);
    if (! openToken)
    {
        return -1;
    }
    handle_unique_ptr tokenPtr(token);
    uid_t ret = GetUID(token);
    return ret;
}

uid_t geteuid()
{
    HANDLE process = ::GetCurrentProcess();
    HANDLE thread = ::GetCurrentThread();
    HANDLE token = nullptr;
    BOOL openToken = ::OpenThreadToken(
        thread, TOKEN_READ|TOKEN_QUERY_SOURCE, FALSE, &token);
    if (! openToken && ::GetLastError() == ERROR_NO_TOKEN)
    {
        openToken = ::OpenThreadToken(
            thread, TOKEN_READ|TOKEN_QUERY_SOURCE, TRUE, &token);
        if (! openToken && ::GetLastError() == ERROR_NO_TOKEN)
        {
            openToken = ::OpenProcessToken(
                process, TOKEN_READ|TOKEN_QUERY_SOURCE, &token);
        }
    }
    if (! openToken)
    {
        return -1;
    }
    handle_unique_ptr tokenPtr(token);
    uid_t ret = GetUID(token);
    return ret;
}


//=================getpwuid emulation=================================

#ifdef __COMMENT__
struct passwd
{
  char *pw_name;		/* Username.  */
  char *pw_passwd;		/* Password.  */
  uid_t pw_uid;		/* User ID.  */
  gid_t pw_gid;		/* Group ID.  */
  char *pw_gecos;		/* Real name.  */
  char *pw_dir;			/* Home directory.  */
  char *pw_shell;		/* Shell program.  */
};
#endif // __COMMENT__
struct passwdData
{
    passwdData()
    {
     wPwd.pw_name = pw_name.content;
     wPwd.pw_passwd = pw_passwd.content;
     wPwd.pw_uid = 0;
     wPwd.pw_gid = 0;
     wPwd.pw_dir = pw_dir.content;
     wPwd.pw_shell = pw_shell.content;
    }
descString  pw_name;
descString  pw_passwd;
uid_t       pw_uid;
gid_t       pw_gid;
uriString   pw_dir;
descString  pw_shell;
struct passwd wPwd;
};

struct passwd *
getpwuid(uid_t puid)
{
descString wDomainName;
LPTSTR wDomainNamePtr=(LPTSTR)wDomainName.content;
descString wUsername;
LPTSTR wUsernameNamePtr=(LPTSTR)wUsername.content;
SID wuid =(SID) puid;
SID_NAME_USE wSidNameUse;
DWORD wUserNameSize = cst_desclen;
DWORD wDomainNameSize=cst_desclen;

/*  see : https://msdn.microsoft.com/en-us/library/windows/desktop/aa379166(v=vs.85).aspx
 *
 * BOOL WINAPI LookupAccountSid(
  _In_opt_  LPCTSTR       lpSystemName,
  _In_      PSID          lpSid,
  _Out_opt_ LPTSTR        lpName,
  _Inout_   LPDWORD       cchName,
  _Out_opt_ LPTSTR        lpReferencedDomainName,
  _Inout_   LPDWORD       cchReferencedDomainName,
  _Out_     PSID_NAME_USE peUse
);*/

BOOL wRet= LookupAccountSid(
  nullptr,
  &wuid,
  (LPTSTR)wUsername.content,
  &wUserNameSize,
  (LPTSTR)wUsername.content,
  &wDomainNameSize,
  &wSidNameUse
);
    if (!wRet)
        {
        ZException.getLastError(_GET_FUNCTION_NAME_,
                                ZS_AUTHREJECTED,
                                Severity_Error,
                                "LookupAccountSid failed for uid <%d>",(int)puid);
        return nullptr;
        }
    passwdData* wPwdData = new passwdData ;
    wPwdData->pw_uid = puid;
    wPwdData->pw_gid = SidNameUse.gid;
    return wPwdData->wPwd;
}//getpwuid

#endif

ZSystemUser &ZSystemUser::_copyFrom(const ZSystemUser &pIn)
{
    SystemUserId = pIn.SystemUserId;
    SystemGroupId = pIn.SystemGroupId;
    SystemUserName = pIn.SystemUserName;

    Home = pIn.Home;
    return *this;
}

void ZSystemUser::clear()
{
    SystemUserId.clear();
    SystemGroupId.clear();
    SystemUserName.clear();
    Home.clear();
#ifdef __GNUG__
    memset(&pwUserData,0,sizeof(pwUserData));
#endif
}



#endif // ZUSER_CPP
