#ifndef ZDATABUFFER_CPP
#define ZDATABUFFER_CPP

#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zerror.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zexceptionmin.h>
#include <ztoolset/zfunctions.h>

#include <ztoolset/zcharset.h>

#include <zcrypt/zcrypt.h>
#include <zcrypt/checksum.h>
#include <zcrypt/md5.h>

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
   _free(wOutString);
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
    _free(wPlainBuffer);
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
/*

/**
 * @brief ZDataBuffer::encryptAES256 encrypts current object's content to AES256 according mandatory given Key and Vector
 *  and put the resulting encrypted data to optional ZDataBuffer pZDB.
 * If pZDB is omitted, then encrypted data replaces current object's content.
 * @param pKey      mandatory AES256 key object
 * @param pVector   mandatory AES256 vector object
 * @param pZDB      optional ZDataBuffer that will receive encrypted data. If omitted encrypted data replaces current object's content.
 * @return  a ZStatus see ZCrypt::encryptToFile() for having return status values and conditions
 */
ZStatus
ZDataBuffer::encryptAES256(zbs::ZCryptKeyAES256 pKey, zbs::ZCryptVectorAES256 pVector, ZDataBuffer *pZDB) /** encrypts current object's content to AES256 according given Key and Vector */
{
  ZCryptAES256 wCrypt;

    unsigned char* wCryptedBuffer=nullptr;
    size_t wCryptedBufferLen=0;
    ZStatus wSt= wCrypt.encrypt(&wCryptedBuffer,&wCryptedBufferLen,Data,Size,pKey,pVector);
    if (wSt!=ZS_SUCCESS)
            {
            _free(wCryptedBuffer); // free if not nullptr
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
    _free(wCryptedBuffer);
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
ZDataBuffer::encryptAES256toFile(const char*pFileName,ZCryptKeyAES256 pKey, ZCryptVectorAES256 pVector) /** encrypts current object's content to AES256 according given Key and Vector */
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
ZStatus
ZDataBuffer::uncryptAES256(ZCryptKeyAES256 pKey, ZCryptVectorAES256 pVector, ZDataBuffer *pZDB) /** uncrypts current object's content to AES256 according given Key and Vector */
{
  ZCryptAES256 wCrypt;

    unsigned char* wPlainTextBuffer=nullptr;
    size_t wPlainTextBufferLen=0;
    ZStatus wSt= wCrypt.uncrypt(&wPlainTextBuffer,&wPlainTextBufferLen,Data,Size,pKey,pVector);
    if (wSt!=ZS_SUCCESS)
            {
            _free(wPlainTextBuffer);
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
    _free(wPlainTextBuffer);
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

    ZStatus wSt=  wCrypt.uncryptFromFile(pFileName,&wPlainText,&wPlainTextLen,pKey,pVector);
    if (wSt!=ZS_SUCCESS)
        {
        _free(wPlainText);
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
ZDataBuffer::allocate(ssize_t pSize)
{
_MODULEINIT_
    if ((pSize>__INVALID_SIZE__)||((ssize_t)pSize<0))
    {
        fprintf(stderr,
                "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid size %ld \n",
                _GET_FUNCTION_NAME_,
                pSize);
        _ABORT_ ;

    }
    if (Data==nullptr)
        Data=(unsigned char*)malloc((size_t)(pSize+1));
    else
    {
        //       free(Data);
        Data=(unsigned char*)realloc(Data,(size_t)(pSize+1));
    }
    if (Data==nullptr)
    { // ZException not defined yet
        fprintf(stderr,
                "Module   %s\n"
                "Status   %s\n"
                "Severity %s\n"
                "-F-ERRMALLOC  Cannot allocate/reallocate memory of size %ld for buffer\n",
                _GET_FUNCTION_NAME_,
                decode_ZStatus( ZS_MEMERROR),
                decode_Severity( Severity_Fatal),
                pSize);
        _ABORT_;
    }
    Size=pSize;
    DataChar=(char *)Data;
    VoidPtr=(void*)Data;
    WDataChar=(wchar_t*)Data;
    _RETURN_ Data;
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
ZDataBuffer::extend(ssize_t pSize)
{
_MODULEINIT_
    if ((pSize>__INVALID_SIZE__)||(pSize<0))
    {
        fprintf(stderr,
                "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid size %ld \n",
                _GET_FUNCTION_NAME_,
                pSize);
        _ABORT_;
    }
    if (Data==nullptr)
    {
        Data=(unsigned char*)malloc(pSize+1);
        if (Data==nullptr)
        { // ZException not defined yet
            fprintf(stderr,
                    "Module   %s\n"
                    "Status   %s\n"
                    "Severity %s\n"
                    "-F-ERRMALLOC  Cannot extend memory of size %ld (initial size %ld) for buffer\n",
                    _GET_FUNCTION_NAME_,
                    decode_ZStatus( ZS_MEMERROR),
                    decode_Severity( Severity_Fatal),
                    pSize,
                    Size);
            _ABORT_;
        }

        Size=pSize;
        DataChar=(char *)Data;
        VoidPtr=(char*)Data;
        WDataChar=(wchar_t*)Data;
        _RETURN_ Data;
    }// Data==nullptr

    Data =(unsigned char*)realloc(Data,Size+pSize);
   if (Data==nullptr)
        { // ZException not defined yet
        fprintf(stderr,
                "Module   %s\n"
                "Status   %s\n"
                "Severity %s\n"
                "ZDataBuffer-F-ERRMALLOC  Cannot extend memory of size %ld (initial size %ld) for buffer\n",
                _GET_FUNCTION_NAME_,
                decode_ZStatus( ZS_MEMERROR),
                decode_Severity( Severity_Fatal),
                pSize,
                Size);
        _ABORT_;
        }
    unsigned char* wExtentPtr=(unsigned char*)(Data + Size);
    Size+=pSize;
    DataChar=(char *)Data;
    VoidPtr=(void*)Data;
    WDataChar=(wchar_t*)Data;
    _RETURN_ wExtentPtr;  // returns the first byte of extended memory
}// extend
/**
 * @brief ZDataBuffer::extendBZero extends allocated space with pSize bytes and set the new allocated space to binary 0
 *
 * @param[in] pSize size to extend
 * @return    pointer to first available byte of extended space as a unsigned char*
 */
unsigned char*
ZDataBuffer::extendBZero(ssize_t pSize)
{
    if (!(pSize>0))
        return Data;
    unsigned char* wData=extend(pSize);
    memset(wData,0,pSize);
    return wData;
}

/**
 * @brief ZDataBuffer::bsearch   Binary search
 *
 *  searches in a binary mode for a Key contained in pKey with pKeySize length
 *  in ZDataBuffer::Data Content of Size length.
 *
 *  Returns the offset of pKey if it has been found. -1 if nothing has been found
 *
 * @param[in] pKey   a void* pointer to byte sequence to search for
 * @param[in] pKeySize  length of byte sequence
 * @param[in] pOffset   starting offset for search in ZDataBuffer::Data
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */
ssize_t
ZDataBuffer::bsearch (void *pKey,
                      const size_t pKeySize,
                      const size_t pOffset)
{
    long widx = 0;
    long wistart = -1;
    char *wKey = (char *)pKey;
    long wHighIdx = Size;

    for (long wi=pOffset; wi <wHighIdx ;wi++)
    {
        if (wKey[widx]==DataChar[wi])
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
    char *wKey = (char *)pKey;
    long wHighIdx = this->Size ;
    char ToCompareFromKey ;
    char ToCompareFromArray;

    for (ssize_t wi=pOffset; wi <wHighIdx ;wi++)
    {
        ToCompareFromArray = DataChar[wi];
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
    char *wKey = (char *)pKey;
    long wHighIdx = this->Size ;
    long wistart=0;
    char ToCompareFromKey ;
    char ToCompareFromArray;

    if (wHighIdx<pSize)
        return -1;

    ssize_t wi=pOffset;
    wistart=pOffset;
    while (wi<pSize)
    {
        ToCompareFromArray = DataChar[wi];
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
    char *wKey = (char *)pKey;
    char ToCompareFromArray , ToCompareFromKey;
    //long wHighIdx = Size;

    if (pOffset<0)
        pOffset = Size;
    if (pOffset>Size)
        exit (EXIT_FAILURE) ;

    for (long wi=pOffset; wi >= 0 ;wi--)
    {
        ToCompareFromArray = DataChar[wi];
        ToCompareFromKey = wKey[widx];
        if ((ToCompareFromKey >=cst_lowercase_begin)&&(ToCompareFromKey <=cst_lowercase_end))
            ToCompareFromKey -= cst_upperization ;

        if ((ToCompareFromArray>=cst_lowercase_begin)&&(ToCompareFromArray<=cst_lowercase_end))
            ToCompareFromArray -= cst_upperization ;

        if (ToCompareFromKey==ToCompareFromArray)
            //            if (wKey[widx]==DataChar[wi])
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
    char *wKey = (char *)pKey;
    //long wHighIdx = Size;

    if (pOffset<0)
        pOffset = Size;
    if (pOffset>Size)
        exit (EXIT_FAILURE) ;

    for (long wi=pOffset; wi >= 0 ;wi--)
    {
        if (wKey[widx]==DataChar[wi])
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
    long wSize ;

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

    memset (&DataChar[pStart],pChar,wSize);
    return *this;
}

ZDataBuffer&
ZDataBuffer::setString(const char* pString)
{
    size_t wStrlen=strlen(pString)+1;
    allocate(wStrlen);
    memmove(DataChar,pString,wStrlen);
    return(*this);
}

ZDataBuffer&
ZDataBuffer::appendString(const char* pString)
{
    size_t wOld=Size;
    size_t wStrlen=strlen(pString)+1;
    extend(wStrlen);
    memmove(&DataChar[wOld],pString,wStrlen);
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
    memmove(DataChar,pString,wStrlen);
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
    memmove(DataChar,pString,wStrlen);
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
    memmove(DataChar,pString,wStrlen);
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
    memmove(DataChar,pString,wStrlen);
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

    pLineChar.allocate(pSize+1);
    pLineHexa.allocate((pSize*3)+1);

    for (size_t wi=0; wi < pSize;wi ++)
    {
        wHexa = wPtr[wi];
        _Zsprintf(&pLineHexa.DataChar[wj],"%02X ",wHexa);
        if ((wPtr[wi]>31)&&(wPtr[wi]<127))
        {
            pLineChar.DataChar[wi]=wPtr[wi];
            wj+=3;
            continue;
        }
        if (((wPtr[wi]>6)&&(wPtr[wi]<14))||(wPtr[wi]==27))
        {
            pLineChar.DataChar[wi] = '@' ;
            wj+=3;
            continue;
        }
        pLineChar.DataChar[wi] = '.' ;
        wj+=3;
    }// for
    pLineHexa.DataChar[pSize*3]='\0';
    pLineChar.DataChar[pSize]='\0';
    return ;
} // dumpHexa


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
             wRulerHexa.DataChar,
             wRulerAscii.DataChar);

    long wOffset=0;
    int wL=0;
    while ((wOffset+pColumn)<Size)
    {
        dumpHexa(wOffset,pColumn,wLineHexa,wLineChar);
        fprintf(pOutput,"%6d |%s |%s|\n",wL,wLineHexa.DataChar,wLineChar.DataChar);
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

    fprintf(pOutput,wFormat,wL,wLineHexa.DataChar,wLineChar.DataChar);
*/

}//Dump
void ZDataBuffer::Dump(const int pColumn, ssize_t pLimit, FILE* pOutput)
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

    size_t wLimit;
    if (pLimit>0)
        wLimit = ((size_t)pLimit) > Size?Size:(size_t)pLimit;
    else
        wLimit = Size;
    fprintf (pOutput,
             "Offset  %s  %s\n",
             wRulerHexa.DataChar,
             wRulerAscii.DataChar);

    size_t wOffset=0;
    int wL=0;
    while ((wOffset+(size_t)pColumn)<wLimit)
        {
        dumpHexa(wOffset,pColumn,wLineHexa,wLineChar);
        fprintf(pOutput,"%6d |%s |%s|\n",wL,wLineHexa.DataChar,wLineChar.DataChar);
        wL+=pColumn;
        wOffset+=(pColumn);
        }

    if (pLimit < Size)
        {
        fprintf (pOutput," <%ld> bytes more not dumped.\n",Size-pLimit);
        return;
        }
    if (wOffset < wLimit)
        {
        char wFormat [50];

        sprintf (wFormat,"#6d |#-%ds |#-%ds|\n",pColumn*3,pColumn);
        wFormat[0]='%';
        for (int wi=0;wFormat [wi]!='\0';wi++)
            if (wFormat[wi]=='#')
                wFormat[wi]='%';

        dumpHexa(wOffset,(wLimit-wOffset),wLineHexa,wLineChar);

        fprintf(pOutput,wFormat,wL,wLineHexa.DataChar,wLineChar.DataChar);
        }
    return;
}//DumpLimit

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
        _Zsprintf(&pLineHexa.DataChar[wj],"%02X ",wHexa);
        if ((wPtr[wi]>31)&&(wPtr[wi]<127))
        {
            pLineChar.DataChar[wi]=wPtr[wi];
            wj+=3;
            continue;
        }
        if (((wPtr[wi]>6)&&(wPtr[wi]<14))||(wPtr[wi]==27))
        {
            pLineChar.DataChar[wi] = '@' ;
            wj+=3;
            continue;
        }
        pLineChar.DataChar[wi] = '.' ;
        wj+=3;
    }// for
    pLineHexa.DataChar[pSize*3]='\0';
    pLineChar.DataChar[pSize]='\0';
    return;
} // dumpHexa

//====================ZBlob : Export Import methods =================
ZDataBuffer*
ZBlob::_exportURF(ZDataBuffer*pUniversal)
{

    uint64_t wSize=getByteSize()+sizeof(ZTypeBase)+sizeof(uint64_t);
    ZTypeBase wType=ZType_Blob;
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    pUniversal->setData(&wType,sizeof(ZTypeBase));

    wSize=reverseByteOrder_Conditional<uint64_t>(wSize);
    pUniversal->appendData(&wSize, sizeof(wSize));

    pUniversal->appendData(Data,Size);
    return pUniversal;
}


/**
 * @brief ZBlob::_importURF import data from data in URF format.
 *                                      pSize contains the available space in bytes.
 * @param pUniversal a pointer to flat URF data to import and set ZBlob with
 * @param pSize size IN BYTES available for import (and not in number or characters -sizeof(wchar_t) )
 * @return
 */
ZStatus
ZBlob::_importURF(unsigned char* pUniversal)
{
_MODULEINIT_
    ZTypeBase wType;
    uint64_t wSize;
    size_t wOffset=0;
    memmove(&wType,Data,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    // control wType
    if (wType!=ZType_Blob)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid type : expected <%s> while given type is <%lX> <%s>\n",
                              decode_ZType(ZType_Blob),
                              wType,
                              decode_ZType(wType));
        _RETURN_ ZS_INVTYPE;
    }
    wOffset+= sizeof(ZTypeBase);

    memmove(&wSize,Data+wOffset,sizeof(wSize));
    wSize=reverseByteOrder_Conditional<uint64_t>(wSize);
    wOffset+= sizeof(wSize);
    setData(pUniversal+wOffset,(size_t)wSize);
    _RETURN_ ZS_SUCCESS;

}// _importURF

ZStatus
ZBlob::getUniversalFromURF(unsigned char* pURFDataPtr,ZDataBuffer& pUniversal)
{
    uint64_t wEffectiveUSize ;
    ZTypeBase wType;
    unsigned char* wURFDataPtr = pURFDataPtr;

    memmove(&wType,wURFDataPtr,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    wURFDataPtr += sizeof (ZTypeBase);
    if (wType!=ZType_Blob)
    {
        fprintf (stderr,
                 "%s>> Error invalid URF data type <%X> <%s> while getting universal value of <%s> ",
                 _GET_FUNCTION_NAME_,
                 wType,
                 decode_ZType(wType),
                 decode_ZType(ZType_Blob));
        return ZS_INVTYPE;
    }

    memmove (&wEffectiveUSize,wURFDataPtr,sizeof(uint64_t));        // first is URF byte size (including URF header size)
    wEffectiveUSize=reverseByteOrder_Conditional<uint64_t>(wEffectiveUSize);
    wURFDataPtr += sizeof (uint64_t);

    wEffectiveUSize = wEffectiveUSize - (uint32_t)(sizeof(ZTypeBase) + (sizeof(uint32_t)*2)); // compute net Universal size
    pUniversal.allocateBZero((wEffectiveUSize)); // fixed string must have canonical characters count allocated

    memmove(pUniversal.Data,wURFDataPtr,wEffectiveUSize);
    return ZS_SUCCESS;
}//getUniversalFromURF





/** @endcond */ //Development



#endif // ZDATABUFFER_CPP
