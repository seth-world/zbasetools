#ifndef CHECKSUM_CPP
#define CHECKSUM_CPP

#include <zcrypt/checksum.h>
#include <cstdio>
#include <stdint.h>

#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zerror.h>

//-----------checkSum-------------------------

#include <openssl/sha.h>

#include <ztoolset/utfvaryingstring.h>

#include <ztoolset/ztypetype.h>


/**
 * @brief checkSum::compute computes/recomputes SHA256 checksum.
 * @param pInBuffer
 * @param pLen
 */
void
checkSum::compute(const unsigned char *pInBuffer,size_t pLen)
{

    clear();
    SHA256((const unsigned char *)pInBuffer,pLen,content);
    return;
}
/*checkSum &
checkSum::compute(ZDataBuffer &pDataBuffer)
{
    compute (pDataBuffer.Data,pDataBuffer.Size);
    return *this;
}
*/

ZStatus checkSum::fromHexa(const char* pInput, const size_t pSize)
{

    if (pSize <= cst_checksumHexa)
                    {
                fprintf(stderr,
                        "%s-F-INVSIZ Fatal Error : invalid size of input string from Hexadecimal convertion.\n"
                        " Checksum hexadecimal string size must be greater than or equal to <%d>\n",
                        _GET_FUNCTION_NAME_,
                        cst_checksumHexa);
                return ZS_INVSIZE;
                    }
    char wBuf[3];
    wBuf[2]='\0';
    for(int wi=0; wi<cst_checksumHexa;wi++)
                {
                ::strncpy(wBuf,(const char*)&pInput[wi*2],2);
                content[wi]=(uint8_t)strtoul(wBuf,nullptr,16);
                }
    return ZS_SUCCESS;
} //fromHexa

ZStatus checkSum::fromHexa(const utf8VaryingString&pInput)
{
    if (pInput.strlen() != cst_checksumHexa)
    {
        fprintf(stderr,
            "checkSum::fromHexa-F-INVSIZ Invalid size of input string from Hexadecimal convertion.\n"
            " Checksum hexadecimal string size must be greater than or equal to <%d>\n",
            cst_checksumHexa);
        return ZS_INVSIZE;
    }
    char wBuf[3];
    wBuf[2]='\0';
    for(int wi=0; wi < cst_checksumHexa;wi++)
    {
        ::strncpy(wBuf,(const char*)&pInput.Data[wi*2],2);
        content[wi]=(uint8_t)strtoul(wBuf,nullptr,16);
    }
    return ZS_SUCCESS;
}//fromHexa

bool
checkSum::compareHexa(const char* pDesc)
{
    checkSum wComp;
    wComp.fromHexa(pDesc, strlen(pDesc));
    return memcmp(content, wComp.content, cst_checksum) == 0;
//    return(::strncmp(toHexa(),pDesc,cst_checksumHex));
}


utf8VaryingString checkSum::toHexa(void) const
{
  utf8VaryingString wH;
    wH.allocateUnitsBZero((cst_checksum*2)+1);
  int wi=cst_checksum;
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

bool checkSum::operator == (const char* pInput) {
  return(::strncmp(toHexa().toCChar(),pInput,cst_checksumHexa));
}

checkSum& checkSum::_copyFrom(const checkSum &pIn)
{
  for (size_t wi=0;wi<sizeof(content);wi++)
    content[wi]=pIn.content[wi];
  return *this;
}



/**
 * @brief checkSum::_exportURF
 *
 *  Nota Bene : Checksum is processed and used as a wide big endian value
 *
 * @param pUniversal
 * @return
 */
size_t
checkSum::_exportURF(ZDataBuffer& pUniversal) const
{

size_t wUniversalSize=cst_checksum+sizeof(ZTypeBase);

unsigned char* wPtr=pUniversal.extend(wUniversalSize);

    _exportAtomicPtr<ZTypeBase>(ZType_CheckSum,wPtr);
    pUniversal.extend(wUniversalSize);

    memmove(wPtr,content,cst_checksum); // no reverse : checksum is Endian free
    return wUniversalSize;
}
size_t
checkSum::_exportURF_Ptr(unsigned char*& pURF) const
{
  size_t wUniversalSize=sizeof(content)+sizeof(ZTypeBase);

  _exportAtomicPtr<ZTypeBase>(ZType_CheckSum,pURF);
  memmove((pURF),content,cst_checksum); // no reverse : checksum is Endian free
  pURF += cst_checksum;
  return wUniversalSize;
}

size_t
checkSum::getURFSize() const {
  return sizeof(ZTypeBase) + cst_checksum ;
}
size_t
checkSum::getURFHeaderSize() const {
  return sizeof(ZTypeBase) ;
}
size_t
checkSum::getUniversalSize() const {
  return cst_checksum ;
}
ssize_t
checkSum::_importURF(const unsigned char* &pUniversal)
{
ZTypeBase   wType;


    memmove(&wType,pUniversal,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    if (wType!=ZType_CheckSum)
        {
        fprintf(stderr,
                "%s-S-INVTYPE Severe Error : Error requesting to import URF data to checkSum while URF Header type is <%X> <%s>.\n",
                _GET_FUNCTION_NAME_,
                wType,
                decode_ZType(wType));

        return ZS_INVTYPE;
        }
    memmove(content,(pUniversal+sizeof(ZTypeBase)),cst_checksum);
    pUniversal += cst_checksum + sizeof(ZTypeBase);
    return ssize_t(cst_checksum + sizeof(ZTypeBase));
}// _importURF

ZStatus
checkSum::getUniversalFromURF(const unsigned char* pURFDataPtr,ZDataBuffer& pUniversal,const unsigned char** pURFDataPtrOut)
{
 ZTypeBase wType;
 const unsigned char* wURFDataPtr = pURFDataPtr;

     memmove(&wType,wURFDataPtr,sizeof(ZTypeBase));
     wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
     wURFDataPtr += sizeof (ZTypeBase);
     if (wType!=ZType_CheckSum)
         {
         fprintf (stderr,
                  "%s>> Error invalid URF data type <%X> <%s> while expecting <%s> ",
                  _GET_FUNCTION_NAME_,
                  wType,
                  decode_ZType(wType),
                  decode_ZType(ZType_CheckSum));
         return ZS_INVTYPE;
         }

    pUniversal.setData(wURFDataPtr,cst_checksum);

    if (pURFDataPtrOut)
      {
      *pURFDataPtrOut = wURFDataPtr + cst_checksum;
      }
    return ZS_SUCCESS;
}//getUniversalFromURF

ZDataBuffer
checkSum::_export() const
{
  ZDataBuffer wReturn;
  wReturn.setData(content,sizeof(content));
  return wReturn;
}

size_t
checkSum::_import(const unsigned char* &pUniversal)
{
  memmove(content,pUniversal,cst_checksum);
  pUniversal += cst_checksum;
  return cst_checksum;
}// _import


#ifdef __COMMENT__
#ifdef QT_CORE_LIB
checkSum &
checkSum::fromQByteArray (const QByteArray &pQBA)
{
    if (pQBA.size()!=cst_checksum)
                {
                fprintf(stderr,
                        "%s-F-INVCHECKSUMINPUT Invalid checksum input size %d in place of %d required (SHA256)\n.\n",
                        _GET_FUNCTION_NAME_,
                        pQBA.size(),
                        cst_checksum);
                _ABORT_
                 }
    memmove(content,pQBA.data(),pQBA.size());
    return *this;
}//fromQByteArray
#endif // QT_CORELIB
#endif // __COMMENT__


#endif // CHECKSUM_CPP
