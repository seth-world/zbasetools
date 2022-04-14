#ifndef MD5_CPP
#define MD5_CPP

#include <zcrypt/md5.h>

#include <cstdio>

#include <openssl/sha.h>
#include <ztoolset/zerror.h>
#include <ztoolset/utfvaryingstring.h>

#include <ztoolset/zdatabuffer.h>

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

ZDataBuffer*
md5::_exportURF(ZDataBuffer*pUniversal) const
{
  ZTypeBase wType=ZType_MD5;
  size_t wUniversalSize=sizeof(content)+sizeof(ZTypeBase);

  pUniversal->extend(wUniversalSize);
  // URF Header is
  wType=reverseByteOrder_Conditional<ZTypeBase>(wType);    // ZTypeBase in reverseOrder if LE (if little endian)

  memmove(pUniversal->Data,&wType,sizeof(ZTypeBase));
  memmove((pUniversal->Data+sizeof(wType)),content,cst_md5); // no reverse : checksum is Endian free
  return pUniversal;
}

size_t
md5::_exportURF_Ptr(unsigned char*& pURF) const
{
  size_t wUniversalSize=sizeof(content)+sizeof(ZTypeBase);
  _exportAtomicPtr<ZTypeBase>(ZType_MD5,pURF);
  memmove((pURF),content,cst_md5); // no reverse : checksum is Endian free
  return wUniversalSize;
}

size_t
md5::getURFSize() const {
  return sizeof(ZTypeBase) + cst_md5 ;
}


ZStatus
md5::_importURF(const unsigned char* &pUniversal)
{
  ZTypeBase   wType;


  memmove(&wType,pUniversal,sizeof(ZTypeBase));
  wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
  if (wType!=ZType_MD5)
  {
    fprintf(stderr,
        "%s-S-INVTYPE Severe Error : Error requesting to import URF data to checkSum while URF Header type is <%X> <%s>.\n",
        _GET_FUNCTION_NAME_,
        wType,
        decode_ZType(wType));

    return ZS_INVTYPE;
  }
  memmove(content,(pUniversal+sizeof(ZTypeBase)),cst_md5);

  return ZS_SUCCESS;
}// _importURF


#endif // MD5_CPP
