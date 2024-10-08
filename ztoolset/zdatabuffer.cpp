#ifndef ZDATABUFFER_CPP
#define ZDATABUFFER_CPP

#include "zdatabuffer.h"
#include "zerror.h"
#include "uristring.h"
#include "zexceptionmin.h"
#include "zfunctions.h"

#include "zcharset.h"

#include <zcrypt/zcrypt.h>
#include <zcrypt/checksum.h>
#include <zcrypt/md5.h>

//#include "zutfstrings.h"

#include <zxml/zxmlprimitives.h>
#include "utfvaryingstring.h"

ZDataBuffer::ZDataBuffer(void): Data(nullptr), Size(0) ,CryptMethod(ZCM_Uncrypted)
{return;}



//#include <openssl/bio.h>
//#include <openssl/evp.h>
//#include <openssl/buffer.h>



ZStatus ZDataBuffer::operator <<(uriString & pURI)
{
    return pURI.loadContent(*this);
}

/**
 * @brief ZDataBuffer::encodeB64 Encode the ZDataBuffer content with Base 64 encoding method (OpenSSL).
 *
 *  Warning: Encrypts the exact content length of ZDataBuffer given by Size, including termination character ('\0') if any.
 *
 * @param pZDB  a pointer to a ZDataBuffer. if omitted(nullptr) then B64 converted data will be set to current ZDataBuffer object content.
 * @return  a ZStatus
 */
ZStatus
ZDataBuffer::encryptB64( ZDataBuffer*pZDB )
{
    unsigned char*  wOutString;
    size_t          wOutSize;

   ZStatus wSt= ::encryptB64( &wOutString,&wOutSize,Data,Size); // call base routine from /zcrypt/zcrypt.h
   if (wSt!=ZS_SUCCESS)
                return wSt;
    if (!pZDB)  // if no ZDataBuffer provided : use current and replace plain text with B64
        {
        setData(wOutString,wOutSize);
        }
        else
        {
         pZDB->setData(wOutString,wOutSize);
        }
   zfree(wOutString);
   return wSt;
} //-----------------encodeB64-------------------
 #ifdef __COMMENT__
    BIO *bio, *b64;
    BUF_MEM *bptr;
    char*  wPtr;
    int wRet;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line do not add \n
    int wLen = BIO_write(b64, Data, Size);
    if (wLen<1)
    {
        fprintf(stderr,"%s-E>BIO_write fails to copy ZDataBuffer data (size is %ld>\n",
                _GET_FUNCTION_NAME_,
                Size);
        return (*this);
    }
    wRet=BIO_flush(b64);
    //    BIO_get_mem_ptr(b64, &bptr);
    //    setData(bptr->data,bptr->length);
    wLen=BIO_get_mem_data(b64, &wPtr);
    setData(wPtr,wLen);
    addTermination();

    BIO_free_all(b64);
    CryptMethod = ZCM_Base64;
    return *this;

} //-----------------encodeB64-------------------
#endif // __COMMENT__


/**
 * @brief ZDataBuffer::decodeB64 decodes ZDataBuffer content using Base 64 encoding facilities (OpenSSL).
 * @return a reference to ZDataBuffer with decoded data
 */
ZStatus
ZDataBuffer::uncryptB64(ZDataBuffer *pZDB)
{
    unsigned char* wPlainBuffer;
    size_t wPlainSize=0;
    ZStatus wSt=::uncryptB64(&wPlainBuffer,&wPlainSize,Data,Size); // call base routine from /zcrypt/zcrypt.h
    if (wSt!=ZS_SUCCESS)
                 return wSt;
     if (!pZDB)  // if no ZDataBuffer provided : use current and replace plain text with B64
         {
         setData(wPlainBuffer,wPlainSize);
         }
         else
         {
          pZDB->setData(wPlainBuffer,wPlainSize);
         }
    zfree(wPlainBuffer);
    return wSt;
}
#ifdef __COMMENT__

    //----------------------------------------------
    BIO *bio,*b64 ;

    int decodeLen = calcDecodeLengthB64(Data,Size);

    unsigned char *buffer = (unsigned char *)malloc(decodeLen+1);
    buffer[decodeLen]='\0';

    bio = BIO_new_mem_buf(Data, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
    Size = BIO_read(bio, buffer, Size);
    //     assert(Size == (decodeLen-1)); //length should equal decodeLen, else something went horribly wrong
    BIO_free_all(bio);
    //     allocate (Size+1);
    //     memmove (Data,buffer,Size);
    //     Data[Size]='\0';
    setData(buffer,Size);
    //    addTermination();

    free(buffer);
    CryptMethod = ZCM_Uncrypted;
    return(*this);


} // ------------decodeB64---------------------
#endif // __COMMENT__

/**
 * @brief ZDataBuffer::encryptAES256 encrypts current object's content to AES256 according mandatory given Key and Vector
 *  and put the resulting encrypted data to optional ZDataBuffer pZDB.
 * If pZDB is omitted, then encrypted data replaces current object's content.
 * @param pKey      mandatory AES256 key object
 * @param pVector   mandatory AES256 vector object
 * @param pZDB      optional ZDataBuffer that will receive encrypted data. If omitted encrypted data replaces current object's content.
 * @return  a ZStatus see ZCrypt::encryptToFile() for having return status values and conditions
 */
 /** encrypts current object's content to AES256 according given Key and Vector */
ZStatus
ZDataBuffer::encryptAES256( const ZCryptKeyAES256& pKey,
                            const ZCryptVectorAES256& pVector,
                            ZDataBuffer *pZDB)
{
  ZCryptAES256 wCrypt;

    unsigned char* wCryptedBuffer=nullptr;
    size_t wCryptedBufferLen=0;
    ZStatus wSt= wCrypt.encrypt(&wCryptedBuffer,&wCryptedBufferLen,Data,Size,pKey,pVector);
    if (wSt!=ZS_SUCCESS)
            {
            zfree(wCryptedBuffer); // free if not nullptr
            return wSt;
            }
     if (!pZDB)  // if no ZDataBuffer provided : use current and replace plain text with B64
         {
         setData(wCryptedBuffer,wCryptedBufferLen);
         }
         else
         {
          pZDB->setData(wCryptedBuffer,wCryptedBufferLen);
         }
    zfree(wCryptedBuffer);
    return wSt;
}// ZDataBuffer::encryptAES256
/**
 * @brief ZDataBuffer::encryptAES256toFile encrypts AES256 full current content of ZDataBuffer object and writes it to pFileName file.
 * @param[in] pFileName a valid path for a file (existing or not)
 * @param[in] pKey      mandatory ZCryptKeyAES256 containing crypting key value
 * @param[in] pVector   mandatory ZCryptVectorAES256 containing crypting vector value
 * @return  a ZStatus see ZCrypt::encryptToFile() for having return status values and conditions
 */
ZStatus
ZDataBuffer::encryptAES256toFile (  const char*pFileName,
                                    const ZCryptKeyAES256& pKey,
                                    const ZCryptVectorAES256& pVector)
{
  ZCryptAES256 wCrypt;
    return  wCrypt.encryptToFile(pFileName,Data,Size,pKey,pVector);
}
/**
 * @brief ZDataBuffer::uncryptAES256 uncrypts current object's content to AES256 according mandatory given Key and Vector
 *  and put the resulting uncrypted data to optional ZDataBuffer pZDB.
 * If pZDB is omitted, then plain text data replaces current object's content.
 * @param pKey      mandatory AES256 key object
 * @param pVector   mandatory AES256 vector object
 * @param pZDB      optional ZDataBuffer that will receive plain text (uncrypted) data. If omitted uncrypted data replaces current object's content.
 * @return  a ZStatus see ZCrypt::uncrypt() for having return status values and conditions
 */
/** uncrypts current object's content to AES256 according given Key and Vector */
ZStatus
ZDataBuffer::uncryptAES256 (const ZCryptKeyAES256& pKey,
                            const ZCryptVectorAES256& pVector,
                            ZDataBuffer *pZDB)
{
  ZCryptAES256 wCrypt;

    unsigned char* wPlainTextBuffer=nullptr;
    size_t wPlainTextBufferLen=0;
    ZStatus wSt= wCrypt.uncrypt(wPlainTextBuffer,wPlainTextBufferLen,Data,Size,pKey,pVector);
    if (wSt!=ZS_SUCCESS)
            {
//            _free(wPlainTextBuffer);   // already done within ZCrypt::uncypt()
            return wSt;
            }
     if (!pZDB)  // if no ZDataBuffer provided : use current and replace plain text with B64
         {
         setData(wPlainTextBuffer,wPlainTextBufferLen);
         }
         else
         {
          pZDB->setData(wPlainTextBuffer,wPlainTextBufferLen);
         }
    zfree(wPlainTextBuffer);
    return wSt;
}// uncryptAES256
/**
 * @brief ZDataBuffer::uncryptAES256FromFile loads AES256 encrypted data from file pFileName
 *   uncrypts AES256 its content and stores it within current object.
 * @param[in] pFileName a valid path for a file (existing or not)
 * @param[in] pKey      mandatory ZCryptKeyAES256 containing crypting key value
 * @param[in] pVector   mandatory ZCryptVectorAES256 containing crypting vector value
 * @return  a ZStatus see ZCrypt::uncryptToFile() for having return status values and conditions
 */
ZStatus
ZDataBuffer::uncryptAES256FromFile(const char*pFileName,ZCryptKeyAES256 pKey, ZCryptVectorAES256 pVector) /** uncrypts current object's content to AES256 according given Key and Vector */
{
  ZCryptAES256 wCrypt;
  unsigned char* wPlainText=nullptr;
  size_t wPlainTextLen=0;

    ZStatus wSt=  wCrypt.uncryptFromFile(pFileName,wPlainText,wPlainTextLen,pKey,pVector);
    if (wSt!=ZS_SUCCESS)
        {
        zfree(wPlainText);
        return wSt;
        }
    setData(wPlainText,wPlainTextLen);
    return wSt;
}// uncryptAES256FromFile

ZStatus ZDataBuffer::fomFile(uriString &pURI)
{
    return pURI.loadContent(*this);
}// fomFile
/**
 * @brief ZDataBuffer::getmd5 computes a md5 cheksum from current ZDataBuffer full content.
 * @return a md5 object
 */
md5 ZDataBuffer::getmd5( void )
{
    return md5(Data,Size);
} //-----------------getmd5-------------------

#include <zcrypt/checksum.h>
/**
 * @brief ZDataBuffer::getcheckSum creates and return a checkSum object from current ZDataBuffer full content.
 * @return a checkSum object
 */
checkSum
ZDataBuffer::getcheckSum(void)
{
    return checkSum(Data,Size);
}
/**
 * @brief ZDataBuffer::newcheckSum generates a checksum object from ZDataBuffer content using new checkSum().
 * Created object must further be deleted.
 * @return a pointer to created checkSum object
 */
checkSum*
ZDataBuffer::newcheckSum(void)
{
    return new checkSum(Data,Size);
}




/**
 * @brief ZDataBuffer::allocate      Allocates pSize bytes to storage space.
 *
 * If ZDataBuffer contains data : existing data will NOT be lost
 *
 * Size to be allocated is controled to be a positive number and less than parameter __INVALID_SIZE__
 *
 * If an invalid size is detected, then this is considered as a programmer error and excution is aborted using abort(),
 * with an appropriate message written to stderr.
 *
 * __INVALID_SIZE__ (zerror.h) is defined as the maximum size to be allocated for ZDataBuffer.
 *
 *
 * @param[in] pSize size in bytes to allocate
 * @return a pointer to allocated memory as unsigned char*
 */
unsigned char*
ZDataBuffer::allocate(size_t pSize)
{

    if ((pSize > __INVALID_SIZE__)||((ssize_t)pSize<0))
      {
        fprintf(stderr,
                "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid size %ld %lX.\n",
                _GET_FUNCTION_NAME_,
                pSize,pSize);
        exit(EXIT_FAILURE);
      }
     /* allocate/reallocate enough size for requested byte size plus end sign cst_ZBUFFEREND */
    if (Data==nullptr)
        Data=zmalloc<unsigned char>((size_t)(pSize));
      else
        {
        Data=zrealloc(Data,(size_t)(pSize));
         }
    Size=pSize;

    return  Data;
}//allocate

unsigned char*
ZDataBuffer::allocate_old(size_t pSize)
{
  uint32_t* wP2=nullptr;

  if ((pSize>__INVALID_SIZE__)||((ssize_t)pSize<0))
  {
    fprintf(stderr,
        "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid size %ld %lX.\n",
        _GET_FUNCTION_NAME_,
        pSize,pSize);
      exit(EXIT_FAILURE);
  }
  /* allocate/reallocate enough size for requested byte size plus end sign cst_ZBUFFEREND */
  if (Data==nullptr)
    Data=zmalloc<unsigned char>((size_t)(pSize+sizeof(uint32_t)));
  else
  {
    wP2=(uint32_t*)&Data[Size];
    *wP2=0;/* clear former cst_ZBUFFEREND marker before reallocating memory */
    Data=zrealloc(Data,(size_t)(pSize)+sizeof(uint32_t));
  }
  Size=pSize;

  /* set the end of allocated / reallocated memory as cst_ZMEND */
  wP2=(uint32_t*)&Data[Size];
  *wP2=cst_ZBUFFEREND;

  return  Data;
}//allocate
/**
 * @brief ZDataBuffer::allocateBZero allocates pSize bytes and sets it to binary zero
 * @param[in] pSize size in bytes to allocate
 * @return a pointer to allocated memory as unsigned char*
 */
unsigned char*
ZDataBuffer::allocateBZero(ssize_t pSize)
{
    memset(allocate(pSize),0,pSize);
    return Data;
}

/**
 * @brief ZDataBuffer::extend
 *  Extends allocated memory space with pSize bytes without loosing data :
 *
 *  New size is then computed with Size + pSize (Existing size + extension size)
 *
 *  Existing data is NOT lost and will be copied at beginning of new allocated space
 *
 * @param[in] pSize size to extend
 * @return    pointer to first available byte of extended space as a unsigned char*
 */

unsigned char*
ZDataBuffer::extend(size_t pSize)
{
  uint32_t* wP2=nullptr;
  if ((pSize>__INVALID_SIZE__)||((ssize_t)pSize<0))
    {
      fprintf(stderr,
          "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid size %ld %lX.\n",
          _GET_FUNCTION_NAME_,
          pSize,pSize);
      exit(EXIT_FAILURE);
    }
  if (Data==nullptr)
      {
        Data=zmalloc<unsigned char>(pSize);
        Size=pSize;
        return  Data;
      }// Data==nullptr

    Data =zrealloc(Data,Size+pSize);
    unsigned char* wExtentPtr=Data+Size ;

    Size+=pSize;

    return  wExtentPtr;  // returns the first byte of extended memory
}// extend

unsigned char*
ZDataBuffer::extend_old(size_t pSize)
{
  uint32_t* wP2=nullptr;
  if ((pSize>__INVALID_SIZE__)||((ssize_t)pSize<0))
  {
    fprintf(stderr,
        "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid size %ld %lX.\n",
        _GET_FUNCTION_NAME_,
        pSize,pSize);
    exit(EXIT_FAILURE);
  }
  if (Data==nullptr)
  {
    Data=zmalloc<unsigned char>(pSize+sizeof(uint32_t));
    Size=pSize;

    /* set the end of allocated / reallocated memory as cst_ZMEND */
    wP2=(uint32_t*)&Data[Size];
    *wP2=cst_ZBUFFEREND;

    return  Data;
  }// Data==nullptr


  wP2=(uint32_t*)&Data[Size];
  *wP2=0;/* clear former cst_ZBUFFEREND marker before reallocating memory */
  Data =zrealloc(Data,Size+pSize+sizeof(uint32_t));

  unsigned char* wExtentPtr=(unsigned char*)(Data + Size);

  Size+=pSize;

  /* set the end of allocated / reallocated memory as cst_ZMEND */
  wP2=(uint32_t*)&Data[Size];
  *wP2=cst_ZBUFFEREND;

  return  wExtentPtr;  // returns the first byte of extended memory
}// extend
/**
 * @brief ZDataBuffer::extendBZero extends allocated space with pSize bytes and set the new allocated space to binary 0
 *
 * @param[in] pSize size to extend
 * @return    pointer to first available byte of extended space as a unsigned char*
 */
unsigned char*
ZDataBuffer::extendBZero(size_t pSize)
{
    unsigned char* wData=extend(pSize);
    memset(wData,0,pSize);
    return wData;
}

const ZDataBuffer&
ZDataBuffer::truncate(size_t pLen)
{
  if (pLen >= Size)   /* cannot truncate a greater size than actual size */
      return *this;

  allocate(pLen);
  return *this;
 }



ssize_t
ZDataBuffer::bsearch (void *pKey,
                      const size_t pKeySize,
                      const size_t pOffset)
{
    long widx = 0;
    long wistart = -1;
    const unsigned char *wKey = (const unsigned char *)pKey;
    long wHighIdx = Size;

    for (long wi=pOffset; wi < wHighIdx ;wi++)
    {
        if (wKey[widx]==Data[wi])
        {
            if (wistart==-1)
                wistart=wi;
            widx++;
            if (widx==pKeySize)
            {
                return (wistart);
            }
            continue;
        }// if wKey
        widx=0;
        wistart=-1;
    }
    return (-1) ;
}//bsearch

ssize_t
ZDataBuffer::bsearch (const ZDataBuffer *pKey,
                     const size_t pOffset)
{
    if (pKey->isEmpty())
        return -1;
    long wKeyLen = pKey->Size;
    long widx = 0;
    long wistart = -1;
    const unsigned char* wKey = (const unsigned char *)pKey->Data;
    long wHighIdx = Size;

    for (long wi=pOffset; wi < wHighIdx ;wi++)
    {
        if (wKey[widx]==Data[wi])
        {
            if (wistart==-1)
                wistart=wi;
            widx++;
            if (widx==wKeyLen)
            {
                return (wistart);
            }
            continue;
        }// if wKey
        widx=0;
        wistart=-1;
    }
    return (-1) ;
}//bsearch

ssize_t
ZDataBuffer::bsearch (const utf8VaryingString *pKey,
                     const size_t pOffset)
{
    if (pKey->isEmpty())
        return -1;
    long wKeyLen = pKey->strlen();
    long widx = 0;
    long wistart = -1;
    const unsigned char* wKey = (const unsigned char *)pKey->Data;
    long wHighIdx = Size;

    for (long wi=pOffset; wi < wHighIdx ;wi++)
    {
        if (wKey[widx]==Data[wi])
        {
            if (wistart==-1)
                wistart=wi;
            widx++;
            if (widx==wKeyLen)
            {
                return (wistart);
            }
            continue;
        }// if wKey
        widx=0;
        wistart=-1;
    }
    return (-1) ;
}//bsearch
/**
 * @brief ZDataBuffer::bsearch   Binary search
 *
 *  searches in a binary mode for a Key contained in pKey with pKeySize length
 *  in ZDataBuffer::Data Content of Size length.
 *
 *  Returns the offset of pKey if it has been found. -1 if nothing has been found
 *
 * @param[in] pKey      ZDataBuffer containing byte sequence to search for, with its length as Size of ZDataBuffer
 * @param[in] pOffset   starting offset for search in ZDataBuffer::Data
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */

ssize_t
ZDataBuffer::bsearch (ZDataBuffer &pKey,const size_t pOffset)

{
    return (bsearch(pKey.Data,pKey.Size,pOffset)) ;
}//bsearch

/**
 * @brief ZDataBuffer::bsearchCaseRegardless binary search BUT when it founds a character compares CASE REGARDLESS. returns an offset from pOffset within the whole ZDataBuffer considered as a bulk of bytes
 * @param pKey
 * @param pSize
 * @param pOffset
 * @return
 */
ssize_t
ZDataBuffer::bsearchCaseRegardless (void *pKey, ssize_t pSize, ssize_t pOffset)
{
    long widx = 0;
    long wistart = -1;
    unsigned char *wKey = (unsigned char *)pKey;
    long wHighIdx = this->Size ;
    unsigned char ToCompareFromKey ;
    unsigned char ToCompareFromArray;

    for (ssize_t wi=pOffset; wi <wHighIdx ;wi++)
    {
        ToCompareFromArray = Data[wi];
        ToCompareFromKey = wKey[widx];
        if ((ToCompareFromKey >= cst_lowercase_begin)&&(ToCompareFromKey <= cst_lowercase_end))
            ToCompareFromKey -= cst_upperization ;

        if ((ToCompareFromArray>=cst_lowercase_begin)&&(ToCompareFromArray<=cst_lowercase_end))
            ToCompareFromArray -= cst_upperization ;

        if (ToCompareFromKey==ToCompareFromArray)
        {
            if (wistart==-1)
                wistart=wi;
            widx++;
            if (widx==pSize)
            {
                return (wistart);
            }
            continue;
        }// if wKey
        widx=0;
        wistart=-1;
    }
    return (-1) ;
}//bsearchCaseRegardless

ssize_t
ZDataBuffer::bsearchCaseRegardless (ZDataBuffer &pKey,ssize_t pOffset)
{
    return (bsearchCaseRegardless(pKey.Data,pKey.Size,pOffset));
}

ssize_t
ZDataBuffer::bstartwithCaseRegardless (void *pKey, ssize_t pSize, ssize_t pOffset)
{
    unsigned char *wKey = (unsigned char *)pKey;
    long wHighIdx = this->Size ;
    long wistart=0;
    unsigned char ToCompareFromKey ;
    unsigned char ToCompareFromArray;

    if (wHighIdx<pSize)
        return -1;

    ssize_t wi=pOffset;
    wistart=pOffset;
    while (wi<pSize)
    {
        ToCompareFromArray = Data[wi];
        ToCompareFromKey = wKey[wi];
        if ((ToCompareFromKey >=cst_lowercase_begin)&&(ToCompareFromKey <=cst_lowercase_end))
            ToCompareFromKey -= cst_upperization ;

        if ((ToCompareFromArray>=cst_lowercase_begin)&&(ToCompareFromArray<=cst_lowercase_end))
            ToCompareFromArray -= cst_upperization ;

        if (ToCompareFromKey!=ToCompareFromArray)
            return -1;
        wi++;
    }
    return (0) ; // OK
}//bstartwithCaseRegardless

ssize_t
ZDataBuffer::bstartwithCaseRegardless (ZDataBuffer pKey,size_t pOffset)
{
    return (bstartwithCaseRegardless(pKey.Data,pKey.Size,pOffset));
}

/**
 * @brief breverseSearch   Binary REVERSE search regardless case
 *
 *  Reverse search in a binary mode for a Key contained in pKey with pKeySize length
 *  in ZDataBuffer::Data Content of Size length.
 *
 *  Returns the offset of pKey if it has been found. -1 if nothing has been found
 *
 *  The returned offset is the STARTING byte position of pKey in ZDataBuffer content
 *
 * @param[in] pKey   a void* pointer to byte sequence to search for
 * @param[in] pKeySize  length of byte sequense
 * @param[in] pOffset   starting offset for reverse search in Buffer
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */
ssize_t
ZDataBuffer::breverseSearchCaseRegardless(void *pKey,
                                          ssize_t pKeySize,
                                          ssize_t pOffset)
{
    long widx = pKeySize-1;
    unsigned char *wKey = (unsigned char *)pKey;
    unsigned char ToCompareFromArray , ToCompareFromKey;
    //long wHighIdx = Size;

    if (pOffset<0)
        pOffset = Size;
    if (pOffset>Size)
        exit (EXIT_FAILURE) ;

    for (long wi=pOffset; wi >= 0 ;wi--)
    {
        ToCompareFromArray = Data[wi];
        ToCompareFromKey = wKey[widx];
        if ((ToCompareFromKey >=cst_lowercase_begin)&&(ToCompareFromKey <=cst_lowercase_end))
            ToCompareFromKey -= cst_upperization ;

        if ((ToCompareFromArray>=cst_lowercase_begin)&&(ToCompareFromArray<=cst_lowercase_end))
            ToCompareFromArray -= cst_upperization ;

        if (ToCompareFromKey==ToCompareFromArray)
            //            if (wKey[widx]==Data[wi])
        {
            widx--;
            if (widx<0)
            {
                return (wi);
            }
            continue;
        }// if wKey
        widx=pKeySize-1;
    }
    return (-1) ;
}//breverseSearchCaseRe
/**
 * @brief breverseSearchCaseRegardless   Binary REVERSE search regardless cases
 *
 *  Reverse search in a binary mode for a Key contained in pKey with pKeySize length
 *  in ZDataBuffer::Data Content of Size length.
 *
 *  Returns the offset of pKey if it has been found. -1 if nothing has been found
 *
 *  The returned offset is the STARTING byte position of pKey in ZDataBuffer content
 *
 * @param[in] pKey      ZDataBuffer containing byte sequence to search for, with its length as Size of ZDataBuffer
 * @param[in] pOffset   starting offset for search in ZDataBuffer::Data
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */
ssize_t
ZDataBuffer::breverseSearchCaseRegardless (ZDataBuffer &pKey,
                                           const ssize_t pOffset)
{

    return (breverseSearchCaseRegardless(pKey.Data,pKey.Size,pOffset)) ;
}//breverseSearchCaseRegardless
/**
 * @brief breverseSearch   Binary REVERSE search  Reverse search in a binary mode for a Key contained in pKey with pKeySize length
 *  in ZDataBuffer::Data Content of Size length.
 *
 * This means finds the first occurrence of pKey on pKeySize length starting by the end of ZDataBuffer::Data content (Size) if pOffset is -1
 * or pOffset if pOffset > 0.
 *
 *  Returns
 *  - the offset of pKey if it has been found.
 *  - (-1) if nothing has been found
 *
 *  The returned offset is the STARTING byte position of pKey in ZDataBuffer content from the end of ZDataBuffer content.
 *
 * @param[in] pKey   a void* pointer to byte sequence to search for
 * @param[in] pKeySize  length of byte sequense
 * @param[in] pOffset   starting offset for reverse search in Buffer
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */
ssize_t
ZDataBuffer::breverseSearch (void *pKey,
                             ssize_t pKeySize,
                             ssize_t pOffset)
{
    long widx = pKeySize-1;
    unsigned char *wKey = (unsigned char *)pKey;
    //long wHighIdx = Size;

    if (pOffset<0)
        pOffset = Size;
    if (pOffset>Size)
        exit (EXIT_FAILURE) ;

    for (long wi=pOffset; wi >= 0 ;wi--)
    {
        if (wKey[widx]==Data[wi])
        {
            widx--;
            if (widx<0)
            {
                return (wi);
            }
            continue;
        }// if wKey
        widx=pKeySize-1;
    }
    return (-1) ;
}//breverseSearch
/**
 * @brief breverseSearch   Binary REVERSE search
 *
 *  Reverse search in a binary mode for a Key contained in pKey with pKeySize length
 *  in ZDataBuffer::Data Content of Size length.
 *
 *  Returns the offset of pKey if it has been found. -1 if nothing has been found
 *
 *  The returned offset is the STARTING byte position of pKey in ZDataBuffer content
 *
 * @param[in] pKey      ZDataBuffer containing byte sequence to search for, with its length as Size of ZDataBuffer
 * @param[in] pOffset   starting offset for search in ZDataBuffer::Data
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */
ssize_t
ZDataBuffer::breverseSearch (ZDataBuffer &pKey,
                             const ssize_t pOffset)
{

    return (breverseSearch(pKey.Data,pKey.Size,pOffset)) ;
}//breverseSearch




const unsigned char*
ZDataBuffer::firstNotinSet(const char *pSet)
{
    return((const unsigned char*)_firstNotinSet((const char*)Data,pSet));
}

char*
ZDataBuffer::LTrim(const char *pSet)
{
    return(_LTrim((char*)Data,pSet));
}

char*
ZDataBuffer::RTrim(const char *pSet)
{return(_RTrim((char*)Data,pSet));}

char*
ZDataBuffer::Trim(const char *pSet)
{return(_Trim((char*)Data,pSet));}


/**
 * @brief ZDataBuffer::setChar         set content to pChar starting at pStart byte for pSize long
 *
   - pStart AND pSize are ommitted, then the whole content (0 length Size) is set to pChar
   - pStart + pSize >  actual Size : Size is adjusted (extend method) to pStart+pSize+1
 *
 *
 *
 * @param pChar
 * @param pStart
 * @param pSize
 * @return a reference to current ZDataBuffer object
 */

ZDataBuffer &
ZDataBuffer::setChar(const char pChar,size_t pStart,long pSize)
{
    long wSize = -1;

    if (pSize==-1)
    {
        wSize=Size;
    }
    if ((pStart+wSize)>Size)
    {
        wSize=(pStart+pSize)-Size ;
        extend(wSize); // make room
        wSize=pSize;  // get size again
    }

    memset (&Data[pStart],pChar,wSize);
    return *this;
}

ZDataBuffer&
ZDataBuffer::setString(const char* pString)
{
    size_t wStrlen=strlen(pString)+1;
    allocate(wStrlen);
    memmove(Data,pString,wStrlen);
    return(*this);
}

ZDataBuffer&
ZDataBuffer::appendString(const char* pString)
{
    size_t wOld=Size;
    size_t wStrlen=strlen(pString)+1;
    extend(wStrlen);
    memmove(&Data[wOld],pString,wStrlen);
    return(*this);
}
ZDataBuffer&
ZDataBuffer::setUtf8(const utf8_t* pString)
{
    clearData();
    if (pString==nullptr)
                return *this;
    size_t wStrlen=utfStrlen<utf8_t>(pString);
    allocate(wStrlen+1);
    memmove(Data,pString,wStrlen);
    return(*this);
}
ZDataBuffer&
ZDataBuffer::setUtf16(const utf16_t* pString)
{
    clearData();
    if (pString==nullptr)
                return *this;
    size_t wStrlen=utfStrlen<utf16_t>(pString);
    allocate(wStrlen+1);
    memmove(Data,pString,wStrlen);
    return(*this);
}
ZDataBuffer&
ZDataBuffer::setUtf32(const utf32_t* pString)
{
    clearData();
    if (pString==nullptr)
                return *this;
    size_t wStrlen=utfStrlen<utf32_t>(pString);
    allocate((wStrlen+1));
    memmove(Data,pString,wStrlen);
    return(*this);
}
ZDataBuffer&
ZDataBuffer::setUcs4(const ucs4_t* pString)
{
    clearData();
    if (pString==nullptr)
                return *this;
    size_t wStrlen=utfStrlen<ucs4_t>(pString);
    allocate((wStrlen+1));
    memmove(Data,pString,wStrlen);
    return(*this);
}
//--------ZDataBuffer content tests-----------------------

/**
 * @brief ZDataBuffer::isEqual Tests whether the content given by void* on pSize length exists in ZDataBuffer::Data
 * If pSize is ommitted (<1) then test is made on size of ZDataBuffer content.
 * @param[in] pCompare void* pointer on data to compare
 * @param[in] pSize Length to test on. if <1 then size of ZDataBuffer content is taken
 * @return true if equal, false if not
 */
bool
ZDataBuffer::isEqual (const void *pCompare,long pSize)
{
    ssize_t wsize=(pSize<1)?Size:pSize;
    if (wsize>Size)
        wsize=Size; // nevertheless required. Do not suppress!
    return(memcmp(Data,pCompare,wsize)==0);
}


/**
 * @brief ZDataBuffer::isGreater Tests whether the content given by void* on pSize length is greater than the same segment ZDataBuffer::Data
 * If pSize is ommitted (<1) then test is made on size of ZDataBuffer content.
 * @param[in] pCompare void* pointer on data to compare
 * @param[in] pSize Length to test on. if <1 then size of ZDataBuffer content is taken
 * @return true if greater, false if not
 */
bool
ZDataBuffer::isGreater (const void *pCompare,long pSize)
{
    ssize_t wsize=(pSize<1)?Size:pSize;
    if (wsize>Size)
        wsize=Size; // nevertheless required. Do not suppress!
    return(memcmp(Data,pCompare,wsize)>0);
}
/**
 * @brief ZDataBuffer::isLess Tests whether the content given by void* on pSize length is less than the same segment ZDataBuffer::Data
 * If pSize is ommitted (<1) then test is made on size of ZDataBuffer content.
 * @param[in] pCompare void* pointer on data to compare
 * @param[in] pSize Length to test on. if <1 then size of ZDataBuffer content is taken
 * @return true if is less, false if not
 */
bool
ZDataBuffer::isLess (const void *pCompare,long pSize)
{
    ssize_t wsize=(pSize<1)?Size:pSize;
    if (wsize>Size)
        wsize=Size; // nevertheless required. Do not suppress!
    return(memcmp(Data,pCompare,wsize)<0);
}



ZDataBuffer&
ZDataBuffer::setCheckSum(const checkSum& pChecksum)
{
  setData(pChecksum.content,sizeof(pChecksum.content));
  return *this;
}

/**
 * @brief ZDataBuffer::dumpHexa dumps a segment of data and returns both ascii and hexadecimal values.
 *
 *  Returns the current ZDataBuffer content using 2 ZDataBuffer data fields :
 *   - one for printable hexadecimal result,
 *   - the other is printable ascii result.
 *
 *  Content dump is done from pOffset on pSize length
 *
 * @param[in] pOffset   Offset in bytes (starting from 0) of data to be printed
 * @param[in] pSize     Size in bytes of the data to dump
 * @param[out] pLineHexa content expressed in hexadecimal
 * @param[out] pLineChar content in ascii
 * @return
 */
void
ZDataBuffer::dumpHexa (const size_t pOffset,const long pSize,ZDataBuffer &pLineHexa,ZDataBuffer &pLineChar)
{
    size_t wj=0;
    unsigned char wHexa ;
    unsigned char* wPtr = &Data[pOffset] ;

    pLineChar.allocateBZero(pSize*2+1); /* twice the number of possible characters because  of  "¶" */
    pLineHexa.allocateBZero((pSize*3)+1);

    int wiUtf8=0;

    for (size_t wi=0; wi < pSize;wi ++)
    {
        wHexa = wPtr[wi];
        _Zsprintf((char*)&pLineHexa.Data[wj],"%02X ",wHexa);
        if ((wPtr[wi]>31)&&(wPtr[wi]<127))
        {
            pLineChar.Data[wiUtf8]=wPtr[wi];
            wj+=3;
            wiUtf8++;
            continue;
        }
        if (((wPtr[wi]>6)&&(wPtr[wi]<14))||(wPtr[wi]==27))
        {
          strcpy((char*)&pLineChar.Data[wiUtf8],"¶");  // "¶"
            wj+=3;
            wiUtf8+=sizeof("¶")-1;
            continue;
        }
        pLineChar.Data[wiUtf8] = '.' ;
        wj+=3;
        wiUtf8++;
    }// for
    pLineHexa.Data[pSize*3]='\0';
    pLineChar.Data[wiUtf8]='\0';
    return ;
} // dumpHexa

void
ZDataBuffer::dumpHexa_1 (const size_t pOffset,const long pSize,ZDataBuffer &pLineHexa,ZDataBuffer &pLineChar)
{
  size_t wj=0;
  unsigned char wHexa ;
  unsigned char* wPtr = &Data[pOffset] ;

  pLineChar.allocate(pSize+1);
  pLineHexa.allocate((pSize*3)+1);

  int wiUtf8=0;

  for (size_t wi=0; wi < pSize;wi ++)
  {
    wHexa = wPtr[wi];
    _Zsprintf((char*)&pLineHexa.Data[wj],"%02X ",wHexa);
    if ((wPtr[wi]>31)&&(wPtr[wi]<127))
    {
      pLineChar.Data[wi]=wPtr[wi];
      wj+=3;
      continue;
    }
    if (((wPtr[wi]>6)&&(wPtr[wi]<14))||(wPtr[wi]==27))
    {
      pLineChar.Data[wi] = '@' ;  // "¶"
      wj+=3;
      continue;
    }
    pLineChar.Data[wi] = '.' ;
    wj+=3;
  }// for
  pLineHexa.Data[pSize*3]='\0';
  pLineChar.Data[pSize]='\0';
  return ;
} // dumpHexa_1

/**
 * @brief ZDataBuffer::Dump reports to pOutput the whole content of ZDataBuffer in both ascii and hexa.
 *
 *
 * @param[in] pColumn   Number of bytes displayed both in ascii and hexadecimal per row
 * @param[in] pOutput   a FILE* pointer where the reporting will be made. By default, set to stdout.
 */
void ZDataBuffer::Dump_old(const int pColumn,FILE* pOutput)
{
    ZDataBuffer wRulerAscii;
    ZDataBuffer wRulerHexa;

    int wColumn=rulerSetup (wRulerHexa, wRulerAscii,pColumn);


    ZDataBuffer wLineChar;
    ZDataBuffer wLineHexa;


    fprintf (pOutput,
             "Offset  %s  %s\n",
             wRulerHexa.Data,
             wRulerAscii.Data);

    long wOffset=0;
    int wL=0;
    while ((wOffset+pColumn)<Size)
    {
        dumpHexa(wOffset,pColumn,wLineHexa,wLineChar);
        fprintf(pOutput,"%6d |%s |%s|\n",wL,wLineHexa.Data,wLineChar.Data);
        wL+=pColumn;
        wOffset+=(pColumn);
    }

/*
    char wFormat [50];

    sprintf (wFormat,"#6d |#-%ds |#-%ds|\n",pColumn*3,pColumn);
    wFormat[0]='%';
    for (int wi=0;wFormat [wi]!='\0';wi++)
        if (wFormat[wi]=='#')
            wFormat[wi]='%';

    dumpHexa(wOffset,(Size-wOffset),wLineHexa,wLineChar);

    fprintf(pOutput,wFormat,wL,wLineHexa.Data,wLineChar.Data);
*/

}//Dump_old


void ZDataBuffer::Dump_old2(const int pColumn, ssize_t pLimit, FILE* pOutput)
{
    ZDataBuffer wRulerAscii;
    ZDataBuffer wRulerHexa;

    int wColumn=rulerSetup (wRulerHexa, wRulerAscii,pColumn);


    ZDataBuffer wLineChar;
    ZDataBuffer wLineHexa;

    if (!Size)
        {
        fprintf(pOutput,"<null>\n");
        return;
        }

    long    wLimit;
    long    wReminder;
    if ( (pLimit > 0 ) && ((size_t)pLimit < Size ) )
        wReminder = wLimit = (size_t)pLimit;
    else
        wReminder = wLimit = Size;

    fprintf (pOutput,
             "Offset  %s  %s\n",
             wRulerHexa.Data,
             wRulerAscii.Data);

    size_t wOffset=0;
    int wL=0;
//    while ((wOffset+(size_t)pColumn) < wLimit)
    while ( wReminder > (size_t)pColumn )
        {
        dumpHexa(wOffset,pColumn,wLineHexa,wLineChar);
        fprintf(pOutput,"%6d |%s |%s|\n",wL,wLineHexa.Data,wLineChar.Data);
        wL+=pColumn;
        wOffset+=(pColumn);
        wReminder -= pColumn;
        }

    if (pLimit < Size)
        {
        fprintf (pOutput," <%ld> bytes more not dumped.\n",Size-pLimit);
        return;
        }

        if (wReminder > 0)
        {
//        dumpHexa(wOffset,(wLimit-wOffset),wLineHexa,wLineChar);
        dumpHexa(wOffset,wReminder,wLineHexa,wLineChar);

        int wUtf8Column=pColumn;
        /* count utf8 characters */
        for (long wi=0;(wLineChar.Data[wi] != 0) && (wi < wLineChar.Size);wi++)
          {
          if ( wLineChar.Data[wi] > 127)
            {
            wUtf8Column ++;
            wi++;     // it is an utf8, skip next (hoping not three units per char
            }
          }

        char wFormat [50];

        sprintf (wFormat,"#6d |#-%ds |#-%ds|\n",pColumn*3,wUtf8Column);
        wFormat[0]='%';
        for (int wi=0;wFormat [wi]!='\0';wi++)
            if (wFormat[wi]=='#')
                wFormat[wi]='%';



        fprintf(pOutput,wFormat,wL,wLineHexa.Data,wLineChar.Data);
        }
    return;
}//Dump
void
ZDataBuffer::Dump(const int pWidth,ssize_t pLimit,FILE* pOutput)
{
    fprintf(pOutput,zmemDump(Data,Size,pWidth,pLimit).toCChar());
}


utf8VaryingString
ZDataBuffer::DumpS(const int pWidth,ssize_t pLimit)
{
  return zmemDump(Data,Size,pWidth,pLimit);
}

utf8VaryingString
ZDataBuffer::simpleDump(bool pAlpha,ssize_t pLimit) {
    if ((!Data)||(!Size))
    {
        return "<null>";
    }
    utf8VaryingString wReturn;
    size_t wSize = Size;
    if (pLimit > 0) {
        if (pLimit < wSize)
            wSize=pLimit;
    }

    if (pAlpha) {
        wReturn.allocateUnitsBZero(wSize+1);
        for (int wi=0; wi < wSize; wi++) {
            if (std::isalpha(int(Data[wi])))
                wReturn.Data[wi]= Data[wi];
            else
                wReturn.Data[wi]= '.';
        }
        return wReturn ;
    }

    wReturn.allocateUnitsBZero((wSize*2)+1);
    for (long wi=0 ; wi < wSize ; ) {
        int wByte = Data[wi];
        int wHigh = wByte / 16 ;
        int wLow = wByte - ( wHigh * 16 ) ;

        if (wHigh > 9)
            wReturn.Data[wi] = wHigh + 'A' ;
        else
            wReturn.Data[wi] = wHigh + '0' ;
        wi++;
        if (wLow > 9)
            wReturn.Data[wi] = wLow + 'A' ;
        else
            wReturn.Data[wi] = wLow + '0' ;
        wi++;
    }// for
    return wReturn;
} // ZDataBuffer::simpleDump

//---------End ZDataBuffer--------------------------


//---------ZDataBuffer related functions-------------

template <class _Tp>
/**
 * @brief ZDataBufferFromZArray template function that loads the whole content of a ZArray into given ZDataBuffer.

 Moves without conversion a whole ZArray content ( of type _Tp ) into the ZDataBuffer :
 - allocates the required space to load data
 - moves the data


 * @param pDataBuffer   ZDataBuffer to receive raw data
 * @param pZArray       ZArray template to move data from
 * @return              returns a reference to ZDataBuffer
 */
ZDataBuffer&
ZDataBufferFromZArray (ZDataBuffer &pDataBuffer,zbs::ZArray<_Tp> &pZArray)

{
    size_t wLen = sizeof(zbs::ZArray<_Tp>);
    size_t wLenTotal = wLen+pZArray.ZAllocatedSize + 1;
    pDataBuffer.allocate(wLenTotal);
    memmove(pDataBuffer.Data,&pZArray,wLen);                                    // move data structure
    memmove(&pDataBuffer.Data[wLen],pZArray.Tab,pZArray.ZAllocatedSize+1);     // move data content
    return pDataBuffer;
} //ZDataBufferfromZArray

/** @cond Development
  */
/**
 * @brief rulerSetup
 *      sets up the heading ruler for a block dump according pColumn (number of bytes per row)
 *      returns pColumn eventually modified to make text fit into rulers size
 *
 *
 * @param pRulerHexa
 * @param pRulerAscii
 * @param pColumn
 * @return the effective number of columns
 */
int rulerSetup (ZDataBuffer &pRulerHexa, ZDataBuffer &pRulerAscii,int pColumn)
{
    const char *wHexaTitle = "HexaDecimal" ;
    const char *wAsciiTitle = "Ascii";

    int wColumn=pColumn;
    if (wColumn<strlen(wAsciiTitle))
    {
        wColumn=strlen(wAsciiTitle);
    }

    pRulerHexa.allocate ((wColumn*3)+1);
    pRulerAscii.allocate(wColumn+1);

    pRulerHexa.setChar('_');
    pRulerAscii.setChar('_');

    pRulerAscii[wColumn]='\0';
    pRulerHexa[wColumn*3]='\0';


    long wSHexa = ((wColumn*3)-strlen(wHexaTitle))/2;
    long wSAscii = (wColumn-strlen(wAsciiTitle))/2;

    pRulerHexa.changeData((void*)wHexaTitle,strlen(wHexaTitle),wSHexa);
    pRulerAscii.changeData((void*)wAsciiTitle,strlen(wAsciiTitle),wSAscii);
    return wColumn;
}




void
dumpHexa (void *pPtr,long pSize,ZDataBuffer &pLineHexa,ZDataBuffer &pLineChar)
{
    size_t wj=0;
    unsigned char wHexa ;
    unsigned char* wPtr =(unsigned char*) pPtr ;

    pLineChar.allocate(pSize+1);
    pLineHexa.allocate((pSize*3)+1);

    for (size_t wi=0; wi < pSize;wi ++)
    {
        wHexa = wPtr[wi];
        _Zsprintf((char*)&pLineHexa.Data[wj],"%02X ",wHexa);
        if ((wPtr[wi]>31)&&(wPtr[wi]<127))
        {
            pLineChar.Data[wi]=wPtr[wi];
            wj+=3;
            continue;
        }
        if (((wPtr[wi]>6)&&(wPtr[wi]<14))||(wPtr[wi]==27))
        {
            pLineChar.Data[wi] = '@' ;
            wj+=3;
            continue;
        }
        pLineChar.Data[wi] = '.' ;
        wj+=3;
    }// for
    pLineHexa.Data[pSize*3]='\0';
    pLineChar.Data[pSize]='\0';
    return;
} // dumpHexa

ZStatus _importZStatus(const unsigned char*pPtrIn)
{
  ZStatusBase wSt1;
  _importAtomic<ZStatusBase>(wSt1,pPtrIn);
  return (ZStatus)wSt1;
}

ZDataBuffer& _exportZStatus(ZStatus pValue,ZDataBuffer& pZDB)
{
  ZStatusBase wSt1=pValue;
  wSt1 = reverseByteOrder_Conditional<ZStatusBase>(wSt1);
  pZDB.appendData(&wSt1,sizeof(ZStatusBase));
  return pZDB;
}

size_t
ZDataBuffer::_exportURF(ZDataBuffer &pURF) const
{
  unsigned char* wURF_Ptr,*wPtrIn=Data;
  uint64_t wByteSize=Size;

  wURF_Ptr=pURF.extend(Size+sizeof(ZTypeBase)+sizeof(uint64_t));
  _exportAtomicPtr<ZTypeBase>(ZType_Blob,wURF_Ptr);
  _exportAtomicPtr<uint64_t>(wByteSize,wURF_Ptr);
  while (wByteSize--)
    *wURF_Ptr++=*wPtrIn++;

  return Size+sizeof(ZTypeBase)+sizeof(uint64_t);
}

size_t
ZDataBuffer::getURFSize() const {
  return Size+sizeof(ZTypeBase)+sizeof(uint64_t);
}
size_t
ZDataBuffer::getURFHeaderSize() const {
  return sizeof(ZTypeBase)+sizeof(uint64_t);
}

size_t
ZDataBuffer::_exportURF_Ptr(unsigned char* &pURF) const
{
  unsigned char* wPtrIn=Data;
  uint64_t wByteSize=Size;

  _exportAtomicPtr<ZTypeBase>(ZType_Blob,pURF);
  _exportAtomicPtr<uint64_t>(wByteSize,pURF);
  while (wByteSize--)
    *pURF++=*wPtrIn++;

  return Size+sizeof(ZTypeBase)+sizeof(uint64_t);
}

ssize_t
ZDataBuffer::_importURF(const unsigned char* &pURF)
{
  uint64_t wByteSize=Size;
  ZTypeBase wType;
  _importAtomic<ZTypeBase>(wType,pURF);
  if (wType!=ZType_Blob)
    return 0;
  _importAtomic<uint64_t>(wByteSize,pURF);
  unsigned char* wPtrIn=allocate(wByteSize);
  while (wByteSize--)
    *wPtrIn++=*pURF++;

  return ssize_t(Size+sizeof(ZTypeBase)+sizeof(uint64_t));
}

/** @endcond */ //Development

/*
utf8VaryingString
ZDataBuffer::toXml(const utf8VaryingString& pName, int pLevel)
{
  return  fmtXMLBlob(pName,*this,pLevel);

}

ZStatus
ZDataBuffer::fromXml(zxmlElement* pBlobRoot, ZaiErrors* pErrorLog, ZaiE_Severity pSeverity)
{
utf8VaryingString wContent = fmtXMLnode("zblob", pLevel);

wContent += fmtXMLCdatanode(this);
wContent += fmtXMLint64("id", id, pLevel+1);
wContent += fmtXMLint64("entity", Entity, pLevel+1);
//    wContent += fmtXMLint64("datarank", DataRank, pLevel);
wContent += fmtXMLendnode("zblob", pLevel);
return wContent;
}
*/
#endif // ZDATABUFFER_CPP
