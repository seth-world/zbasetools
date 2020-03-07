#ifndef MD5_CPP
#define MD5_CPP

#include <zcrypt/md5.h>

#include <cstdio>

#include <openssl/sha.h>
#include <ztoolset/zerror.h>

md5&
md5::compute(unsigned char* pData, const size_t pSize)
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, pData, pSize);
    MD5_Final(content, &ctx);
    return *this;
}

md5&
md5::fromHex(const char* pInput)
{
_MODULEINIT_
    if (::strlen(pInput)<=cst_md5Hex)
                    {
                    fprintf(stderr,
                            "%s>> Fatal Error : invalid size of input string from Hexadecimal convertion - md5 string size must be at least %ld\n",
                            _GET_FUNCTION_NAME_,
                            cst_md5Hex);
                    _ABORT_;
                    }
    char wBuf[3];
    wBuf[2]='\0';
    for(int wi=0; wi<cst_md5Hex;wi++)
                {
                ::strncpy(wBuf,(const char*)pInput[wi*2],2);
                content[wi]=(unsigned char)strtoul(wBuf,nullptr,16);
                }
    _RETURN_ (*this);
}//fromHex
/**
 * @brief md5::toHex  converts md5 content to printable hexadecimal value
 * @param pHexaString a char string defined by calling routine as  char pHexaString[cst_md5Hex+1]
 * @return string after conversion
 */
char*
md5::toHex(char* pHexaString)
{
//utfdescString wHexaString;
int wi;
    for(wi = 0; wi < cst_md5; wi++)
         ::sprintf(&pHexaString[wi*2],"%02X", (unsigned int)content[wi]);
    return(pHexaString);
}


#endif // MD5_CPP
