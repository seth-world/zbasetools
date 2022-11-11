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

int cvtHexa (utf8_t pDigit) {
  if (pDigit==0)
    return -2;
  if ((pDigit >= '0') && (pDigit <= '9'))
    return int (pDigit - '0') ;
  if ((pDigit >= 'A') && (pDigit <= 'F'))
    return int (pDigit - 'A' + 10) ;
  return -1;
}
int cvtHexa (utf8_t* &pDigit) {
  int wValue=0;
  int wDig=0;
  if ((wValue=cvtHexa(*pDigit))<0)
      return wValue;
  pDigit++;
  if ((wDig=cvtHexa(*pDigit))<0)
      return wValue;
  pDigit++;
  wValue = wValue * 16;
  wValue += wDig;
  return wValue;
}

ZStatus
md5::fromHexa(const utf8VaryingString& pInput)
{
  if (pInput.isEmpty()) {
    fprintf(stderr,
        "md5::fromHexa-F-NULLPTR Input string to convert is empty.\n");
    return ZS_EMPTY;
  }
  long wLen = pInput.strlen();
  long wInternLen = pInput.strlen()/2;
  if (wLen != (cst_md5Hexa)) {
    fprintf(stderr,
            "md5::fromHexa-F-INVSIZ Invalid size of input string from Hexadecimal convertion - md5 string has size %ld but must be %ld\n",
            wLen,
            cst_md5Hexa);

    return ZS_INVSIZE;
  }
  uint8_t* wPtr=pInput.Data;
  uint8_t* wPtrEnd=pInput.Data+wLen;
  for (int wi = 0; (wPtr < wPtrEnd) && (wi < cst_md5); wi++) {
    int wV=cvtHexa(wPtr);
    if (content[wi] < 0)
      break;
    content[wi]=(unsigned char)wV;
  }
  return ZS_SUCCESS;
}//fromHex
/**
 * @brief md5::toHex  converts md5 content to printable hexadecimal value
 * @param pHexaString a char string defined by calling routine as  char pHexaString[cst_md5Hex+1]
 * @return string after conversion
 */


utf8VaryingString md5::toHexa(void) const
{
  utf8VaryingString wH;
  wH.allocateUnitsBZero((cst_md5*2)+1);
  int wi=cst_md5;
  utf8_t* wPtr=wH.Data;
  while (wi--) {
    utfSprintf(ZCHARSET_UTF8,wPtr,(const utf8_t*)"%02X",(unsigned int)content[wi]);
    wPtr += 2 ;
  }
  /*    for(wi = 0; wi < cst_checksum; wi++)
        std::sprintf(&wH.content[wi*2],"%02X", (unsigned int)content[wi]);
*/
  return(wH);
}

bool md5::operator == (const char* pInput) {
  return(::strncmp(toHexa().toCChar(),pInput,cst_checksumHexa));
}



size_t
md5::_exportURF(ZDataBuffer& pUniversal) const
{
  ZTypeBase wType=ZType_MD5;
  size_t wUniversalSize=sizeof(content)+sizeof(ZTypeBase);

  unsigned char* wPtr=pUniversal.extend(wUniversalSize);

  _exportAtomicPtr<ZTypeBase>(ZType_MD5,wPtr);
  memmove(wPtr,content,cst_md5); // no reverse : checksum is Endian free
  return wUniversalSize;
}

size_t
md5::_exportURF_Ptr(unsigned char*& pURF) const
{
  size_t wUniversalSize=sizeof(content)+sizeof(ZTypeBase);
  _exportAtomicPtr<ZTypeBase>(ZType_MD5,pURF);
  memmove(pURF,content,cst_md5); // no reverse : checksum is Endian free
  pURF += cst_md5;
  return wUniversalSize;
}

size_t
md5::getURFSize() const {
  return sizeof(ZTypeBase) + cst_md5 ;
}
size_t
md5::getURFHeaderSize() const {
  return sizeof(ZTypeBase) ;
}
size_t
md5::getUniversalSize() const {
  return cst_md5 ;
}

ssize_t
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

    return -1;
  }
  memmove(content,(pUniversal+sizeof(ZTypeBase)),cst_md5);

  pUniversal += cst_md5 + sizeof(ZTypeBase);

  return ssize_t(cst_md5 + sizeof(ZTypeBase));
}// _importURF


#endif // MD5_CPP
