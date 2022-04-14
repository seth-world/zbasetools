#ifndef UTFVTEMPLATESTRING_CPP
#define UTFVTEMPLATESTRING_CPP

#include <ztoolset/utfvtemplatestring.h>
#include <ztoolset/zerror.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zexceptionmin.h>
#include <ztoolset/zfunctions.h>

#include <ztoolset/zcharset.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>




/*
int Base64Decode(char* b64message, unsigned char** buffer, size_t* length) { //Decodes a base64 encoded string
    BIO *bio, *b64;

    int decodeLen = calcDecodeLength(b64message);
    *buffer = (unsigned char*)malloc(decodeLen + 1);
    (*buffer)[decodeLen] = '\0';

    bio = BIO_new_mem_buf(b64message, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
    *length = BIO_read(bio, *buffer, strlen(b64message));
    assert(*length == decodeLen); //length should equal decodeLen, else something went horribly wrong
    BIO_free_all(bio);
*/

#include <zcrypt/checksum.h>


//---------utfVaryingString related functions-------------


/** @cond Development
  */


//====================ZBlob : Export Import methods : see <zdatabuffer.cpp> =================


/*
ZStatus
utf8VString::fromUtf16(const utf16_t* pString)
{
    size_t wStrSize = utfStrlen<utf16_t>(pString)+1;
    size_t wStrByteSize= wStrSize*sizeof(utf16_t);
    ZDataBuffer pConvertedString;
//        allocateBZero(utfStrlen<utf16_t>(pString)+1);
    ZStatus wSt=iconvFromTo(ZCHARSET_UTF16,(const unsigned char*)pString,wStrByteSize,ZCHARSET_UTF8,&pConvertedString);
    setData(pConvertedString);
    return wSt;
}
ZStatus
utf8VString::fromUtf32(const utf32_t* pString)
{
    size_t wStrSize = utfStrlen<utf32_t>(pString)+1;
    size_t wStrByteSize= wStrSize*sizeof(utf16_t);
    ZDataBuffer pConvertedString;
//        allocateBZero(utfStrlen<utf16_t>(pString)+1);
    ZStatus wSt=iconvFromTo(ZCHARSET_UTF32,(const unsigned char*)pString,wStrByteSize,ZCHARSET_UTF8,&pConvertedString);
    setData(pConvertedString);
    return wSt;
}


ZStatus fromUtf16(const utf16_t* pString)
{
    size_t wStrSize = utfStrlen<utf16_t>(pString)+1;
    size_t wStrByteSize= wStrSize*sizeof(utf16_t);
    ZDataBuffer pConvertedString;
//        allocateBZero(utfStrlen<utf16_t>(pString)+1);
    ZStatus wSt=iconvFromTo(ZCHARSET_UTF16,(const unsigned char*)pString,wStrByteSize,ZCHARSET_UTF8,&pConvertedString);
    setData(pConvertedString);
    return wSt;
}
ZStatus
utf8VString::fromUtf32(const utf32_t* pString)
{
    size_t wStrSize = utfStrlen<utf32_t>(pString)+1;
    size_t wStrByteSize= wStrSize*sizeof(utf16_t);
    ZDataBuffer pConvertedString;
//        allocateBZero(utfStrlen<utf16_t>(pString)+1);
    ZStatus wSt=iconvFromTo(ZCHARSET_UTF32,(const unsigned char*)pString,wStrByteSize,ZCHARSET_UTF8,&pConvertedString);
    setData(pConvertedString);
    return wSt;
}
*/

/** @endcond */ //Development

#endif // UTFVTEMPLATESTRING_CPP
