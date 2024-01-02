#ifndef USERID_H
#define USERID_H

#include <stdint.h>

#include "utfvaryingstring.h"

#ifdef __GNUG__
#include <unistd.h>
#include <pwd.h>
#define SECURITY_MAX_SID_SIZE 100
#endif // __GNUG__

constexpr size_t cst_uidbin_maxlen = SECURITY_MAX_SID_SIZE;
#define __UID_GNU__ 'G'
#define __UID_WINDOWS__ 'W'






#ifdef __GNUG__
typedef uint8_t BYTE;
typedef uint32_t DWORD;
typedef uid_t userid_type;

#define ANYSIZE_ARRAY 1

#ifndef SID_IDENTIFIER_AUTHORITY_DEFINED

#define SID_IDENTIFIER_AUTHORITY_DEFINED

typedef struct _SID_IDENTIFIER_AUTHORITY
{
    BYTE Value[6];

} SID_IDENTIFIER_AUTHORITY, *PSID_IDENTIFIER_AUTHORITY;

#endif

#ifndef SID_DEFINED

#define SID_DEFINED

typedef struct _SID
{
    BYTE Revision;

    BYTE SubAuthorityCount;

    SID_IDENTIFIER_AUTHORITY IdentifierAuthority;

#ifdef MIDL_PASS

    [size_is(SubAuthorityCount)] DWORD SubAuthority[*];

#else // MIDL_PASS

    DWORD SubAuthority[ANYSIZE_ARRAY];

#endif // MIDL_PASS

} SID, *PISID;

#endif
#else //windows

typedef SID userid_type;

#endif // __GNUG__



namespace zbs {
class ZSystemUserId
{
public:
    ZSystemUserId() { clear(); }

    ZSystemUserId(const ZSystemUserId &pIn) { _copyFrom(pIn); }
    ZSystemUserId(const ZSystemUserId &&pIn) { _copyFrom(pIn); }

    ZSystemUserId(userid_type pUid)
    {
        clear();
        setUid(pUid);
    }

    void clear() { memset(content, 0, cst_uidbin_maxlen); }

    ZSystemUserId &_copyFrom(const ZSystemUserId &pId)
    {
        memmove(content, pId.content, sizeof(content));
        return *this;
    }

    ZSystemUserId &operator=(const ZSystemUserId &pIn) { return _copyFrom(pIn); }
    ZSystemUserId &operator=(const ZSystemUserId &&pIn) { return _copyFrom(pIn); }

#ifdef __USE_WINDOWS__
    //    ZSystemUserId (SID pSid) {}

    //    ZSystemUserId& operator = (SID pId) {memset (this,0,sizeof(ZUserId)); memmove(this,&pId,sizeof(pId)); return *this;}

    char Type = __UID_WINDOWS__;
    int8_t Content[cst_uidbin_maxlen];
    SID *Sid = Content;
    size_t Sid_len;


#else

    void current(void)
    {
        userid_type wuid = getuid();
        memset(content, 0, sizeof(content));
        memmove(content, &wuid, sizeof(wuid));
    }

    //====================Linux SID Emulation================================

    ZSystemUserId &operator=(const userid_type pId)
    {
        clear();
        memmove(content, &pId, sizeof(pId));
        return *this;
    }

    void setUid(const userid_type pId)
    {
        clear();
        memmove(content, &pId, sizeof(userid_type));
    }

    utf8VaryingString toString()
    {
        userid_type wuid;
        utf8VaryingString wReturn;
        memmove(&wuid, content, sizeof(userid_type));
        wReturn.sprintf("%c-%d", Type, (int) wuid);
        return wReturn;
    }
    char Type = __UID_GNU__;
    int8_t content[cst_uidbin_maxlen];
    userid_type *Sid = (userid_type *) content;
    size_t Sid_len = sizeof(userid_type);
#endif



    int compare(const ZSystemUserId &pIn) const
    {
        int wComp = 0;
        int wi = 0;
        while (wi < cst_uidbin_maxlen)
            if ((wComp = content[wi] - pIn.content[wi]) != 0)
                break;
            else
                wi++;
        return wComp;
    }

    bool operator==(const ZSystemUserId &pIn) const { return compare(pIn) == 0; }
};

} // namespace zbs


#endif // USERID_H
