#ifndef MD5_CPP
#define MD5_CPP

#include <zcrypt/md5.h>

#include <cstdio>

#include <openssl/sha.h>
#include <ztoolset/zerror.h>
#include <ztoolset/utfvaryingstring.h>

md5& md5::_copyFrom(const md5 &pIn)
{
  for (size_t wi=0;wi<sizeof(content);wi++)
    content[wi]=pIn.content[wi];
  return *this;
}


md5&
md5::compute(unsigned char* pData, const size_t pSize)
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, pData, pSize);
    MD5_Final(content, &ctx);
    return *this;
}

ZStatus
md5::fromHexa(const utf8VaryingString& pInput)
{

  if (pInput.strlen()!=cst_md5Hexa)
                    {
                    fprintf(stderr,
                            "md5::fromHexa-F-INVSIZ Invalid size of input string from Hexadecimal convertion - md5 string size must be at least %ld\n",
                            cst_md5Hexa);
                    return ZS_INVSIZE;
                    }
    char wBuf[3];
    wBuf[2]='\0';
    for(int wi=0; wi<cst_md5Hexa;wi++)
                {
                ::strncpy(wBuf,(const char*)&pInput.Data[wi*2],2);
                content[wi]=(unsigned char)strtoul(wBuf,nullptr,16);
                }
    return ZS_SUCCESS;
}//fromHex
/**
 * @brief md5::toHex  converts md5 content to printable hexadecimal value
 * @param pHexaString a char string defined by calling routine as  char pHexaString[cst_md5Hex+1]
 * @return string after conversion
 */
CharMan md5::toHexa() const
{
//utfdescString wHexaString;
int wi;
CharMan wHexa;
  if (!isValid())
    return CharMan("<invalid>");
  for(wi = 0; wi < cst_md5; wi++)
         ::sprintf(&wHexa.content[wi*2],"%02X", (unsigned int)content[wi]);
    return(wHexa);
}


#endif // MD5_CPP
