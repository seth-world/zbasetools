#ifndef ENCODEDSTRING_H
#define ENCODEDSTRING_H
#include <config/zconfig.h>
#include <ztoolset/templatestring.h>
#include <ztoolset/zerror.h>
#include <ztoolset/zdatabuffer.h>
#include <string.h>
#include <ztoolset/zlimit.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zatomicconvert.h>

#include <ztoolset/zcharset.h>


class encodedString: ZDataBuffer
{
public:

size_t                      Canonical=0;
ZCryptMethod_type           CryptMethod=ZCM_Uncrypted;
const ZCharset_type         Charset=ZCHARSET_UTF8;
uint8_t*                    content=nullptr;
uint8_t                     UnitSize=1;

    typedef ZDataBuffer     _Base;

    encodedString();
    ZStatus fromASCII(const unsigned char* pInString, size_t pInSize);
    ZStatus toASCII(ZDataBuffer& pZDB);
    ZStatus fromISOLatin1(const unsigned char* pInString, size_t pInSize);
    ZStatus toISOLatin1(ZDataBuffer& pZDB);
    ZStatus fromUtf8 ( utf8_t* pString);
    ZStatus fromUtf16(utf16_t* pInString,bool &pLittleEndian);
    ZStatus fromUtf32(ucs4_t* pInString,bool &pLittleEndian);
    ZStatus toUtf16(ZDataBuffer& pZDB);
    ZStatus fromCharset(const ZCharset_type pCharset,const unsigned char* pInString, size_t pInSize);
    ZStatus toCharset(const ZCharset_type pCharset,ZDataBuffer& pZDB);
    ZStatus fromGuessCharset(const ZCharset_type &pCharset,const unsigned char* pInString, size_t pInSize);
};

#endif // ENCODEDSTRING_H
