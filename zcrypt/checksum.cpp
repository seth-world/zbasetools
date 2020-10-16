#ifndef CHECKSUM_CPP
#define CHECKSUM_CPP

#include <zcrypt/checksum.h>
//#include <ztoolset/uristring.h>
//#include <ztoolset/zexceptionmin.h>
#include <cstdio>
//#include <ztoolset/utfvtemplatestring.h>
//#include <ztoolset/zutfstrings.h>
#include <stdint.h>

#include <openssl/sha.h>  // required for checksum

#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zerror.h>

//-----------checkSum-------------------------

#include <openssl/sha.h>

/**
 * @brief checkSum::compute computes/recomputes SHA256 checksum.
 * @param pInBuffer
 * @param pLen
 */
void
checkSum::compute(unsigned char *pInBuffer,size_t pLen)
{

    clear();
    SHA256((unsigned char *)pInBuffer,pLen,content);
    return;
}
/*checkSum &
checkSum::compute(ZDataBuffer &pDataBuffer)
{
    compute (pDataBuffer.Data,pDataBuffer.Size);
    return *this;
}
*/

checkSum &
checkSum::fromHex(const char* pInput, const size_t pSize)
{
_MODULEINIT_
    if (pSize <= cst_checksumHex)
                    {
                fprintf(stderr,
                        "%s-F-INVSIZ Fatal Error : invalid size of input string from Hexadecimal convertion.\n"
                        " Checksum hexadecimal string size must be greater than %ld\n",
                        _GET_FUNCTION_NAME_,
                        cst_checksumHex);
                _ABORT_
                    }
    char wBuf[3];
    wBuf[2]='\0';
    for(int wi=0; wi<cst_checksumHex;wi++)
                {
                ::strncpy(wBuf,(const char*)&pInput[wi*2],2);
                content[wi]=(uint8_t)strtoul(wBuf,nullptr,16);
                }
    _RETURN_ (*this);
}//fromHex

bool
checkSum::compareHex(const char* pDesc)
{
    return(::strncmp(toHex(),pDesc,cst_checksumHex));
}


const char*
checkSum::toHex(void)
{
int wi;
    for(wi = 0; wi < cst_checksum; wi++)
         std:sprintf(&StrHexa[wi*2],"%02X", (unsigned int)content[wi]);
    return(StrHexa);
}

bool checkSum::operator == (const char* pInput) {return(::strncmp(toHex(),pInput,cst_checksumHex));}

#include <ztoolset/ztypetype.h>

/**
 * @brief checkSum::_exportURF
 *
 *  Nota Bene : Checksum is processed and used as a wide big endian value
 *
 * @param pUniversal
 * @return
 */
ZDataBuffer*
checkSum::_exportURF(ZDataBuffer*pUniversal)
{
ZTypeBase wType=ZType_CheckSum;
size_t wUniversalSize=sizeof(content)+sizeof(ZTypeBase);

    pUniversal->allocateBZero(wUniversalSize);
                                                    // URF Header is
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);    // ZTypeBase in reverseOrder if LE (if little endian)

    memmove(pUniversal->Data,&wType,sizeof(ZTypeBase));
    memmove((pUniversal->Data+sizeof(wType)),content,cst_checksum); // no reverse : checksum is Endian free
    return pUniversal;
}
ZStatus
checkSum::_importURF(unsigned char* &pUniversal)
{
ZTypeBase   wType;
_MODULEINIT_

    memmove(&wType,pUniversal,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    if (wType!=ZType_CheckSum)
        {
        fprintf(stderr,
                "%s-S-INVTYPE Severe Error : Error requesting to import URF data to checkSum while URF Header type is <%X> <%s>.\n",
                _GET_FUNCTION_NAME_,
                wType,
                decode_ZType(wType));

        _RETURN_ ZS_INVTYPE;
        }
    memmove(content,(pUniversal+sizeof(ZTypeBase)),cst_checksum);

    _RETURN_ ZS_SUCCESS;
}// _importURF

ZStatus
checkSum::getUniversalFromURF(unsigned char* pURFDataPtr,ZDataBuffer& pUniversal)
{
 ZTypeBase wType;
 unsigned char* wURFDataPtr = pURFDataPtr;

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
    return ZS_SUCCESS;
}//getUniversalFromURF



#ifdef QT_CORE_LIB
checkSum &
checkSum::fromQByteArray (const QByteArray &pQBA)
{
_MODULEINIT_
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
/*
checkSum &
checkSum::fromQDataBuffer (const ZDataBuffer &pQDB)
{
    if (pQDB.Size!=cst_checksum)
                    {
                    ZException.setMessage(_GET_FUNCTION_NAME_,
                                          ZS_INVOP,
                                          Severity_Fatal,
                                          "-F-INVCHECKSUMINPUT Invalid checksum input size %d in place of %d required (SHA256)\n",
                                          pQDB.Size,
                                          cst_checksum);
                    ZException.exit_abort();
                    }
    memmove(content,pQDB.Data,pQDB.Size);
    return *this;
}
*/


#endif // QT_CORELIB



#endif // CHECKSUM_CPP
