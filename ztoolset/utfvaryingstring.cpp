#ifndef UTFVARYINGSTRING_CPP
#define UTFVARYINGSTRING_CPP

#include <ztoolset/utfvaryingstring.h>
//#include <ztoolset/zbasedatatypes.h>
#include <ztoolset/zcharset.h>


#include <ztoolset/utfkey.h>

void instantiate(void)
{
    utfStrlen<unsigned char>((const unsigned char*)"daslkfjsafkl");
    utfStrlen<char>((const char*)"daslkfjsafkl");
}

template <class _Utf>
void
utfVaryingString<_Utf>::newSortKey(void)
{
    if (SortKey!=nullptr)
            delete SortKey;
    SortKey=new utfKey(nullptr);
}



//=============== MD5 - B64 Encoding Decoding - Checksum - MD5 ==========================
size_t UVScalcDecodeLengthB64(const uint8_t *b64input, size_t pSize) { //Calculates the length of a decoded string
    //    size_t len = strlen(b64input), // cannot have strlen for utf16 and utf32
    size_t len = pSize, padding = 0;

    if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
        padding = 2;
    else if (b64input[len-1] == '=') //last char is =
        padding = 1;

    return (len*3)/4 - padding;
} //UVScalcDecodeLengthB64

/**
 * @brief utf8VaryingString::encodeB64 Encode the utfVaryingString content with Base 64 encoding method (OpenSSL)
 *
 *  Encode the exact content length of utfVaryingString given by Size, including termination character ('\0').
 *
 * @return  a reference to utfVaryingString with encoded data
 */

utf8VaryingString&
utf8VaryingString::encodeB64( void )
{
    BIO *bio, *b64;
    BUF_MEM *bptr;
    const unsigned char*  wPtr;
    int wRet;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line do not add \n
    int wLen = BIO_write(b64, Data, ByteSize);
    if (wLen<1)
    {
        fprintf(stderr,"%s-E>BIO_write fails to copy utfVaryingString data (byte size is %ld>\n",
                _GET_FUNCTION_NAME_,
                ByteSize);
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


#include <cassert>

/**
 * @brief utfVaryingString::decodeB64 decodes utfVaryingString content using Base 64 encoding facilities (OpenSSL).
 * @return a reference to utfVaryingString with decoded data
 */

utf8VaryingString&
utf8VaryingString::decodeB64(void)
{
    //----------------------------------------------
    BIO *bio,*b64 ;

    int decodeLen = UVScalcDecodeLengthB64(DataByte,ByteSize);

    utf8_t *buffer = (utf8_t *)malloc(decodeLen+1);
    buffer[decodeLen]='\0';

    bio = BIO_new_mem_buf(Data, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
    ByteSize = BIO_read(bio, buffer, ByteSize);
    //     assert(Size == (decodeLen-1)); //length should equal decodeLen, else something went horribly wrong
    BIO_free_all(bio);
    //     allocate (Size+1);
    //     memmove (Data,buffer,Size);
    //     Data[Size]='\0';
    setData(buffer,ByteSize);
    //    addTermination();

    free(buffer);
    CryptMethod = ZCM_Uncrypted;
    //    return(*this);
} // ------------decodeB64---------------------


/**
 * @brief utf8VaryingString::encodeB64 Encode the utfVaryingString content with Base 64 encoding method (OpenSSL)
 *
 *  Encode the exact content length of utfVaryingString given by Size, including termination character ('\0').
 *
 * @return  a reference to utfVaryingString with encoded data
 */


/** @brief encryptAES256() Encrypts to a (returned) ZDataBuffer current string content to AES256 according given Key and Vector */
ZStatus utf8VaryingString::encryptAES256(
    ZDataBuffer &pEncryptedZDB, const ZCryptKeyAES256 &pKey, const ZCryptVectorAES256 &pVector)
{
    ZDataBuffer wEncryptedZDB(Data, strlen());
    return wEncryptedZDB.encryptAES256(pKey, pVector);
}

ZStatus utf8VaryingString::uncryptAES256(const ZDataBuffer &pEncryptedZDB,
    const ZCryptKeyAES256 &pKey,
    const ZCryptVectorAES256 &pVector)
{
    ZDataBuffer wZDB(pEncryptedZDB);
    ZStatus wSt = wZDB.uncryptAES256(pKey, pVector);
    if (wSt != ZS_SUCCESS)
        return wSt;
    setData(wZDB.Data, wZDB.Size);
    addConditionalTermination();
    return ZS_SUCCESS;
}


//-----------------encodeB64-------------------




 //================ utf8VaryingString conversions to and from other formats==============================


const char*
utf8VaryingString::toCChar() const
{
    return (const char*)Data ;
}// utf8VaryingString::toCChar

ZDataBuffer&
utf8VaryingString::toCString(ZDataBuffer& pZDB)
{
    pZDB.clear();
    char *wStr=nullptr;

    Context.reset();
   if ((Context.Status=utf8StrToCharStr(&wStr,_Base::getCharset() ,Data,getByteSize()))<0)
    {
    fprintf (stderr,"%s>> conversion to C String failed at source character unit offset <%d> status <0x%X> <%s>\n",
             _GET_FUNCTION_NAME_,
             (int)(Context.InPtr-Context.Start),
             Context.Status,
             decode_UST(Context.Status));
    _free(wStr);
    return pZDB;
    }
    pZDB.setString(wStr);
    free(wStr);
    return pZDB;
}// utf8VaryingString::toCString

#include <ztoolset/cnvcallback.h>   // for custom callbacks
#include <unicode/ucnv.h>           // for icu converters

utf16VaryingString*
utf16VaryingString::fromLatin1(const char* pInString)
{
_MODULEINIT_
    UErrorCode wErr=U_ZERO_ERROR;
    UConverter* wConv=  ucnv_open("UTF-16",&wErr);
    if (testIcuFatal(_GET_FUNCTION_NAME_,
                     wErr,
                     WarningSignal,
                     " Opening utf8 converter")<0)
                                    {_RETURN_ nullptr;}
    UConverterFromUCallback wOldCallback;
    const void* wOldContext;
    ucnv_setFromUCallBack(wConv,
                          &_fromCallback,
                          (const void*)&Context,
                          &wOldCallback,
                          &wOldContext,
                          &wErr);
    if (testIcuFatal(_GET_FUNCTION_NAME_,
                     wErr,
                     WarningSignal,
                     " setting up from unicode callback of utf16 converter")<0)
                                    {_RETURN_ nullptr;}
    Context.reset();

    size_t wStrlen=0;
    while (pInString[wStrlen++]);
    wStrlen++;
    size_t wByteSize=wStrlen*sizeof(utf16_t);
    /*
    int32_t wTheoLen= ucnv_fromAlgorithmic(wConv,
                                           UCNV_LATIN_1,
                         (char *)nullptr, (int32_t) 0,
                         pInString, (int32_t) wStrLen,
                         &wErr);
    */
    wErr=U_ZERO_ERROR;
    int32_t wTheoLen;
    allocateUnitsBZero(wStrlen);
    wTheoLen= ucnv_fromAlgorithmic(wConv,
                                   UCNV_LATIN_1,
                         (char *)Data, (int32_t) wByteSize,
                         pInString, (int32_t) wStrlen,
                         &wErr);
    ucnv_close(wConv);                      // anyway close converter

    if (testIcuError(_GET_FUNCTION_NAME_,
                     wErr,
                     WarningSignal,
                     "while converting Latin-1 to utf16")<0)
                        {_RETURN_ nullptr;}

    _RETURN_ this;
}

UST_Status_type
utf16VaryingString::fromEncoding(const char* pInString,const char*pEncoding)
{
_MODULEINIT_
UST_Status_type wSt;
    UErrorCode wErr=U_ZERO_ERROR;
    UConverter* wConv=  ucnv_open(pEncoding,&wErr);
    if ((wSt=testIcuFatal(_GET_FUNCTION_NAME_,
                     wErr,
                     WarningSignal,
                     " Opening converter for <%s>",pEncoding)) < UST_SEVERE)
                                    {_RETURN_ wSt;}

    UConverterToUCallback wOldCallback;
    const void* wOldContext;
    ucnv_setToUCallBack(wConv,
                          &_toCallback,
                          (const void*)&Context,
                          &wOldCallback,
                          &wOldContext,
                          &wErr);
    if ((wSt=testIcuFatal(_GET_FUNCTION_NAME_,
                     wErr,
                     WarningSignal,
                     " setting up from unicode callback of converter name <%s>",pEncoding)) < UST_SEVERE)
                                    {_RETURN_ wSt;}
    Context.reset();

    size_t wStrlen=0;
    while (pInString[wStrlen++]&& (wStrlen<__STRING_MAX_LENGTH__));
    if (wStrlen >=__STRING_MAX_LENGTH__)
                    _ABORT_;
    wStrlen++;
    size_t wByteSize=wStrlen*sizeof(utf16_t);

    wErr=U_ZERO_ERROR;

    allocateUnitsBZero(wStrlen);
    int32_t wTheoLen;
    wTheoLen=ucnv_toUChars(wConv,(UChar*)Data,wByteSize,pInString,wStrlen,&wErr);

    ucnv_close(wConv);                      // anyway close converter
    if ((wSt=testIcuFatal(_GET_FUNCTION_NAME_,
                     wErr,
                     WarningSignal,
                     "while converting %s to utf16",pEncoding)) < UST_SEVERE)
                                    {_RETURN_ wSt;}
    if (testIcuError(_GET_FUNCTION_NAME_,
                     wErr,
                     WarningSignal,
                     "while converting %s to utf16",pEncoding)<0)
                        {_RETURN_ wSt;}

    _RETURN_ wSt;
}


UST_Status_type
utf8VaryingString::fromUtf8(const utf8_t* pInString)
{
_MODULEINIT_
utf32_t wCodePoint=0;

size_t wOutCount=0,wInCount=0;
utf8_t wUtf8Char[5];
const utf8_t* wInPtr;
utf8_t* wOutPtr ;
size_t wOutUnitOffset=0;
utfSCErr_struct wError;

    clearData();
    Context.reset();
    Context.setStart(pInString);
    Context.OutUnitOffset=0;

    if (!pInString)
         {
         Context.Status=UST_NULLPTR;
         _RETURN_ UST_NULLPTR;
         }
    if (!*pInString)
        {
        Context.Status=UST_EMPTY;
        _RETURN_ UST_EMPTY;
        }
    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }

    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=ZBOM_UTF8))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr);

    Context.utfSetPosition<utf8_t>(wInPtr);

    allocateUnitsBZero(Context.Strlen+1); // starting with current number of utf8 character units in input string
    wInPtr=Context.utfGetEffective<utf8_t>();
    wOutCount=0;
    while (true)
    {
    wOutPtr=Data+Context.OutUnitOffset;
    for (size_t wi=0;wi<sizeof(wUtf8Char);wi++)
                                    wUtf8Char[wi]=0;
//     Context.setPosition( wInPtr);
     Context.InPtr = (uint8_t*)wInPtr;
     Context.InUnitOffset = wInPtr-pInString;
     Context.InByteOffset = Context.InPtr -Context.Start;

     Context.OutPtr= (uint8_t*)wOutPtr;
//     Context.OutUnitOffset = wOutUnitOffset;
     Context.OutByteOffset = ((uint8_t*)Data)-((uint8_t*)wOutPtr);

     Context.Status = utf8CharSize(wInPtr,&wInCount);

     if (Context.Status==UST_ENDOFSTRING)
                                     break;
     Context.CharUnits+=wInCount;
     Context.CodePoint=0;
     wOutCount= wInCount;    // because we transfer utf8 to utf8 (excepted if illegal)

     if (Context.Status < 0)
         {
         Context.ErrTo ++;
         if (Context.TraceTo)
             {
             wError.Status = Context.Status;
             wError.Action=Context.Action;
             wError.ToFrom= SCErr_ToUnicode;
             wError.UnitOffset = Context.InUnitOffset;
             wError.ByteOffset = Context.InByteOffset;
             wError.Ptr = Context.InPtr;
             wError.codePoint = wCodePoint;
             wError.Size=wInCount;
             Context.ErrorQueue->push(wError);
             }
          if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             addConditionalTermination();// end of string mark
                             _RETURN_ Context.Status;
                             }
          if (Context.Action==ZCNV_Skip)
              {
              wInPtr += wInCount;                // just skip
              Context.Count++;
              continue;
              }
          if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
             {
             size_t wEscapeSize=0;
             while (true)
                 {
                 ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                 UST_Status_type wSt=utfEscapeUtf8<utf8_t>(wOutPtr,
                                                            wInPtr,
                                                            wInCount,
                                                            wRemaining,
                                                            Context.Action,
                                                            wEscapeSize);
                 if (wSt!=UST_TRUNCATED)
                        break;
                 allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                 wOutPtr = Data + Context.OutUnitOffset;
                 }
             Context.OutUnitOffset += wEscapeSize;
             Context.Escaped++;
             Context.OutUnitOffset +=wEscapeSize;
             wOutPtr+=wEscapeSize;
             wInPtr+= wInCount;
             Context.Count++;
             continue;
             }// Context.Action >= ZCNV_Escape_xx

          wOutCount=0;           // compute char units of utf8Replacement
          while (utf8Replacement[wOutCount])
                         wOutCount++;
         Context.Substit++;
         }// if (Context.Status < 0)

     ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
     if (wCountReminder < 0)
         {
         wCountReminder = -wCountReminder;
         wCountReminder++;
         allocateUnitsBZero(wCountReminder);
         wOutPtr = Data + Context.OutUnitOffset;
         }

     if (Context.Status<0)
         for (size_t wi=0;wi<sizeof(utf8Replacement);wi++)  //
                     {
                     *wOutPtr = utf8Replacement[wi];
                     wOutPtr++;
                     Context.OutUnitOffset++;
                     }
     else
         for (size_t wi=0;wi<wInCount;wi++)  //
                     {
                     *wOutPtr = wInPtr[wi];
                     wOutPtr++;
                     Context.OutUnitOffset++;
                     }

//     nadd(wUtf8Char,wOutCount);  // extends allocation only when necessary after end of string

     Context.OutUnitOffset += wOutCount;
     Context.Count ++; // One character has been processed
     wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

    }// while true
    addConditionalTermination();
//     *wUtf8Ptr=(utf8_t)'\0';
    _RETURN_ UST_SUCCESS;
}//utf8VaryingString::fromUtf8


UST_Status_type
utf8VaryingString::getByChunk(const utf8_t* pInString,size_t pChunkSize)
{
_MODULEINIT_
utf32_t wCodePoint=0;

size_t wOutCount=0,wInCount=0;
utf8_t wUtf8Char[5];

utf8_t* wOutPtr ;
size_t wOutUnitOffset=0;
utfSCErr_struct wError;

utf8_t wSavedChunk[5] = {0,0,0,0,0};

size_t  wRemainingChunk;
const utf8_t* wInPtr;

    clearData();
    Context.reset();
    Context.setStart(pInString);
    Context.OutUnitOffset=0;

    Context.InByteOffset=0;
    Context.InUnitOffset=0;

    if (!pInString)
         {
         Context.Status=UST_NULLPTR;
         _RETURN_ UST_NULLPTR;
         }
    if (!*pInString)
        {
        Context.Status=UST_EMPTY;
        _RETURN_ UST_EMPTY;
        }

    wRemainingChunk = pChunkSize;

    extendCharsBZero(pChunkSize);
    wOutPtr=Data+Context.OutUnitOffset;

    if (!Context.Strlen) // is that a warm start ?
    {// No ; initial call
    _Base::clear();
    // Context.Strlen has the number of char units for the string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(pChunkSize > 4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=ZBOM_UTF8))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any

    Context.utfSetPosition<utf8_t>(wInPtr);
    }
    else // warm start
    {
    wInPtr=pInString;
    wOutPtr=Context.OutPtr ;  // restore out pointer to previous state
//    wOutUnitOffset = Context.OutUnitOffset;

    if (Context.Status==UST_TRUNCATEDCHAR) // if last status shows previous chunk ended in middle of an utf8 char
        {
        size_t wi=0;
        for (; wi< Context.SavedChunkSize ;wi++)
                    wSavedChunk[wi] = static_cast<utf8_t*>(Context.SavedChunk)[wi]; /* get saved part of character*/
        for (wi=Context.SavedChunkSize; wi< Context.SavedChunkFullSize ;wi++)       /* complement with input string*/
                {
                wSavedChunk[wi]=*wInPtr;
                wInPtr++;
                }
        wSavedChunk[wi]=0;  /* last byte is zero */

        Context.setPosition(wInPtr);
        Context.InByteOffset=Context.InPtr-Context.Start;
        Context.InUnitOffset = pInString-wInPtr;
//        Context.Status = utf8CharSizeChunk((const utf8_t*)wSavedChunk,&wInCount,Context.SavedChunkFullSize);

        Context.Status = utf8Decode((const utf8_t*)wSavedChunk,&wCodePoint,&wInCount,&Context.SavedChunkFullSize);

        wRemainingChunk -= Context.InUnitOffset;
        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;

        if (Context.Status<0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf8_t)'\0'; // end of string mark
                                _RETURN_ Context.Status;
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += (wInCount - Context.SavedChunkSize );                // just skip
                 Context.Count++;
                 goto utf8Varying_getByChunk_Main;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
                {
                size_t wEscapeSize=0;
                while (true)
                    {
                    ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                    UST_Status_type wSt=utfEscapeUtf8<utf8_t>(wOutPtr,
                                                              (const utf8_t*)wSavedChunk,
                                                              wInCount,
                                                              wRemaining,
                                                              Context.Action,
                                                              wEscapeSize);
                    if (wSt!=UST_TRUNCATED)
                           break;
                    allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                    wOutPtr = Data + Context.OutUnitOffset;
                    }
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;

                wInPtr += (wInCount - Context.SavedChunkSize );
                Context.Count++;
                goto utf8Varying_getByChunk_Main;
                }// Context.Action >= ZCNV_Escape_xx

             if ((Context.OutUnitOffset+wInCount) >= (_Base::getUnitCount()-2)) /* allocated unit size is not enough */
                         {
                         extendCharsBZero(pChunkSize);
                         wOutPtr=Data+Context.OutUnitOffset;
                         }
            for (size_t wi=0;wi<sizeof(utf8Replacement);wi++)  //
                        {
                        *wOutPtr = utf8Replacement[wi];
                        wOutPtr++;
                        Context.OutUnitOffset++;
//                        wOutUnitOffset++;
                        }
            }//if (Context.Status<0)
        else
            for (size_t wi=0;wi<wInCount;wi++)  //
                        {
                        *wOutPtr = wInPtr[wi];
                        wOutPtr++;
                        Context.OutUnitOffset++;
//                        wOutUnitOffset++;
                        }

        }// if (Context.Status==UST_TRUNCATEDCHAR)
    }// Warm start

utf8Varying_getByChunk_Main:

    Context.Strlen+=pChunkSize;  // add chunk size to overall unit length
    /*  reset saved utf8 character */
    for (size_t wi=0;wi< sizeof(wSavedChunk);wi++)
                                wSavedChunk[wi]=0;
    Context.SavedChunkSize=0;
    Context.SavedChunkFullSize=0;

    _free(Context.SavedChunk);
    wInPtr=Context.utfGetEffective<utf8_t>();

    while (true)
    {
    wOutPtr=Data+Context.OutUnitOffset;
    for (size_t wi=0;wi<sizeof(wUtf8Char);wi++)
                                    wUtf8Char[wi]=0;
//     Context.setPosition( wInPtr);
     Context.InPtr = (uint8_t*)wInPtr;
     Context.InUnitOffset = wInPtr-pInString;
     Context.InByteOffset = Context.InPtr -Context.Start;

     Context.OutPtr= (uint8_t*)wOutPtr;
//     Context.OutUnitOffset = wOutUnitOffset;
     Context.OutByteOffset = ((uint8_t*)Data)-((uint8_t*)wOutPtr);

     Context.Status = utf8Decode(wInPtr,&wCodePoint,&wInCount,&wRemainingChunk);

     if (Context.Status==UST_ENDOFSTRING)
                                     break;
     if (Context.Status==UST_TRUNCATEDCHAR)  // utf char is cut in middle of units
         {  // save it
         _free(Context.SavedChunk);

         Context.SavedChunk=calloc(5,sizeof(utf8_t));
         utf8_t* wPSavedChunk= (utf8_t*)Context.SavedChunk;  /* allocate an utf8 char max length */
         for(size_t wi=0;wi<5;wi++)                          /* set allocated space to 0 */
                     wPSavedChunk[wi]=0;
         for (size_t wi=0;wi < wRemainingChunk;wi++)
                     wPSavedChunk[wi]= wInPtr[wi];/* set context with units already acquired from chunk till end of chunk */
         Context.SavedChunkSize=wRemainingChunk; /* set context with size that belongs to current chunk */
         Context.SavedChunkFullSize=wInCount;    /* set context with total theorical length of character */

         _RETURN_ UST_TRUNCATEDCHAR;
         }
     wRemainingChunk -= wInCount;
     Context.CharUnits+=wInCount;
     Context.CodePoint=wCodePoint;

     if (Context.Status < 0)
         {
         Context.ErrTo ++;
         if (Context.TraceTo)
             {
             wError.Status = Context.Status;
             wError.Action=Context.Action;
             wError.ToFrom= SCErr_ToUnicode;
             wError.UnitOffset = Context.InUnitOffset;
             wError.ByteOffset = Context.InByteOffset;
             wError.Ptr = Context.InPtr;
             wError.codePoint = wCodePoint;
             wError.Size=wInCount;
             Context.ErrorQueue->push(wError);
             }
          if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             addConditionalTermination();// end of string mark
                             _RETURN_ Context.Status;
                             }
          if (Context.Action==ZCNV_Skip)
              {
              wInPtr += wInCount;                // just skip
              Context.Count++;
              continue;
              }
          if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
             {
             size_t wEscapeSize=0;
             while (true)
                 {
                 ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                 UST_Status_type wSt=utfEscapeUtf8<utf8_t>(wOutPtr,
                                                            wInPtr,
                                                            wInCount,
                                                            wRemaining,
                                                            Context.Action,
                                                            wEscapeSize);
                 if (wSt!=UST_TRUNCATED)
                        break;
                 allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                 wOutPtr = Data + Context.OutUnitOffset;
                 }
             Context.OutUnitOffset += wEscapeSize;
             Context.Escaped++;
             Context.OutUnitOffset +=wEscapeSize;
             wOutPtr+=wEscapeSize;
             wInPtr+= wInCount;
             Context.Count++;
             continue;
             }// Context.Action >= ZCNV_Escape_xx
          wOutCount=0;           // compute char units of utf8Replacement
          while (utf8Replacement[wOutCount])
                         wOutCount++;
         Context.Substit++;
         }// if (Context.Status < 0)

     ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
     if (wCountReminder < 0)
         {
         wCountReminder = -wCountReminder;
         wCountReminder++;
         allocateUnitsBZero(wCountReminder);
         wOutPtr = Data + Context.OutUnitOffset;
         }

     if (Context.Status<0)
         for (size_t wi=0;wi<sizeof(utf8Replacement);wi++)  //
                     {
                     *wOutPtr = utf8Replacement[wi];
                     wOutPtr++;
//                     wOutUnitOffset++;
                     Context.OutUnitOffset++;
                     }
     else
         for (size_t wi=0;wi<wInCount;wi++)  //
                     {
                     *wOutPtr = wInPtr[wi];
                     wOutPtr++;
//                     wOutUnitOffset++;
                     Context.OutUnitOffset++;
                     }

//     nadd(wUtf8Char,wOutCount);  // extends allocation only when necessary after end of string
     Context.Count ++; // One character has been processed
     wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

    }// while true
    addConditionalTermination();
    _RETURN_ UST_SUCCESS;
}//utf8VaryingString::getByChunk

UST_Status_type
utf8VaryingString::fromUtf16(const utf16_t* pInString, ZBool *plittleEndian)
{
_MODULEINIT_
utf32_t wCodePoint=0;

size_t wOutCount=0,wInCount=0;
utf8_t wUtf8Char[5];
const utf16_t* wInPtr;
utf8_t* wOutPtr ;

utfSCErr_struct wError;

    clearData();
    Context.reset();
    Context.setStart(pInString);
    Context.OutUnitOffset=0;

    if (plittleEndian)
         {
         Context.EndianRequest = true;
         Context.LittleEndian = *plittleEndian;
         }
        else
        {
        Context.EndianRequest = false;
        Context.LittleEndian = is_little_endian();
        }

    if (!pInString)
         {
         Context.Status=UST_NULLPTR;
         _RETURN_ UST_NULLPTR;
         }
    if (!*pInString)
        {
        Context.Status=UST_EMPTY;
        _RETURN_ UST_EMPTY;
        }
    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }
    ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF16_LE : ZBOM_UTF16_BE;

    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr);

    Context.Status =utf16CharCount(wInPtr,&wOutCount,plittleEndian);
    if (Context.Status < UST_SEVERE)
                {
                _RETURN_ Context.Status;
                }

    allocateUnitsBZero(wOutCount+1); // starting with current number of utf8 character units in input string
    wInPtr=Context.utfGetEffective<utf16_t>();
    wOutCount=0;
    while (true)
    {
    wOutPtr=Data+Context.OutUnitOffset;
    for (size_t wi=0;wi<sizeof(wUtf8Char);wi++)
                                    wUtf8Char[wi]=0;
//     Context.setPosition( wInPtr);
     Context.InPtr = (uint8_t*)wInPtr;
     Context.InUnitOffset = wInPtr-pInString;
     Context.InByteOffset = Context.InPtr -Context.Start;

     Context.OutPtr= (uint8_t*)wOutPtr;
//     Context.OutUnitOffset = wOutUnitOffset;
     Context.OutByteOffset = ((uint8_t*)Data)-((uint8_t*)wOutPtr);

     Context.Status = utf16Decode(wInPtr,&wCodePoint,&wInCount,plittleEndian);

     if (Context.Status==UST_ENDOFSTRING)
                                     break;
     Context.CharUnits+=wInCount;
     Context.CodePoint=wCodePoint;

     if (Context.Status < 0)
         {
         Context.ErrTo ++;
         if (Context.TraceTo)
             {
             wError.Status = Context.Status;
             wError.Action=Context.Action;
             wError.ToFrom= SCErr_ToUnicode;
             wError.UnitOffset = Context.InUnitOffset;
             wError.ByteOffset = Context.InByteOffset;
             wError.Ptr = Context.InPtr;
             wError.codePoint = wCodePoint;
             wError.Size=wInCount;
             Context.ErrorQueue->push(wError);
             }
          if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             addConditionalTermination();// end of string mark
                             _RETURN_ Context.Status;
                             }
          if (Context.Action==ZCNV_Skip)
              {
              wInPtr += wInCount;                // just skip
              Context.Count++;
              continue;
              }
          if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
             {
             size_t wEscapeSize=0;
             while (true)
                 {
                 ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                 UST_Status_type wSt=utfEscapeUtf16<utf8_t>(wOutPtr,
                                                            wInPtr,
                                                            wInCount,
                                                            wRemaining,
                                                            Context.Action,
                                                            wEscapeSize);
                 if (wSt!=UST_TRUNCATED)
                        break;
                 allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                 wOutPtr = Data + Context.OutUnitOffset;
                 }
             Context.OutUnitOffset += wEscapeSize;
             Context.Escaped++;
             Context.OutUnitOffset +=wEscapeSize;
             wOutPtr+=wEscapeSize;
             wInPtr+= wInCount;
             Context.Count++;
             continue;
             }// Context.Action >= ZCNV_Escape_xx
/*  replacement is made within utf16Decode : wCodePoint contains replacement char */
         Context.Substit++;
         }// if (Context.Status < 0)


     Context.Status= utf8Encode(wUtf8Char,&wOutCount,wCodePoint,nullptr);
     if (Context.Status < 0)
         {
         Context.ErrFrom ++;
         if (Context.TraceFrom)
             {
             wError.Status = Context.Status;
             wError.Action=Context.Action;
             wError.ToFrom= SCErr_FromUnicode;
             wError.UnitOffset = Context.OutUnitOffset;
             wError.ByteOffset = Context.OutByteOffset;
             wError.Ptr = Context.OutPtr;
             wError.codePoint = wCodePoint;
             wError.Size=wOutCount;
             Context.ErrorQueue->push(wError);
            }
         if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                         {
                         addConditionalTermination();// end of string mark
                         _RETURN_ Context.Status;
                         }
         if (Context.Action==ZCNV_Skip)
             {
             wInPtr += wInCount;
             Context.Count++;
             continue;
             }
         if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
            {
            size_t wEscapeSize=0;
            while (true)
                {
                ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                UST_Status_type wSt=utfEscapeUtf16<utf8_t>(wOutPtr,
                                                           wInPtr,
                                                           wInCount,
                                                           wRemaining,
                                                           Context.Action,
                                                           wEscapeSize);
                if (wSt!=UST_TRUNCATED)
                       break;
                allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                wOutPtr = Data + Context.OutUnitOffset;
                }
            Context.OutUnitOffset += wEscapeSize;
            Context.Escaped++;
            Context.OutUnitOffset +=wEscapeSize;
            wOutPtr+=wEscapeSize;
            wInPtr+= wInCount;
            Context.Count++;
            continue;
            }// Context.Action >= ZCNV_Escape_xx
         //in any other error cases, use replacement character
         for (wOutCount=0;utf8Replacement[wOutCount];wOutCount++)
                                         wUtf8Char[wOutCount]=utf8Replacement[wOutCount];
         Context.Substit++;
         }//if (Context.Status < 0)

     ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
     if (wCountReminder < 0)
         {
         wCountReminder = -wCountReminder;
         wCountReminder++;
         allocateUnitsBZero(wCountReminder);
         wOutPtr = Data + Context.OutUnitOffset;
         }

     size_t wi=0;
     while ((wi < wOutCount)&&(wUtf8Char[wi]))
                     {
                     *wOutPtr = wUtf8Char[wi];
                     wOutPtr++;
                     Context.OutUnitOffset++;
                     wi++;
                     }

//     nadd(wUtf8Char,wOutCount);  // extends allocation only when necessary after end of string

     Context.OutUnitOffset += wOutCount;
     Context.Count ++; // One character has been processed
     wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

    }// while true
    addConditionalTermination();
//     *wUtf8Ptr=(utf8_t)'\0';
    _RETURN_ UST_SUCCESS;
}//utf8VaryingString::fromUtf16

UST_Status_type
utf8VaryingString::fromUtf32(const utf32_t* pInString, ZBool *plittleEndian)
{
_MODULEINIT_
utf32_t wCodePoint=0;

size_t wOutCount=0,wInCount=0;
utf8_t wUtf8Char[5];
const utf32_t* wInPtr;
utf8_t*  wOutPtr;
size_t wOutUnitOffset=0;
utfSCErr_struct wError;

     clearData();
     Context.reset();
     Context.setStart(pInString);
     Context.OutUnitOffset=0;
     if (!pInString)
             {
             Context.Status=UST_NULLPTR;
             _RETURN_ UST_NULLPTR;
             }
     if (!*pInString)
             {
             Context.Status=UST_EMPTY;
             _RETURN_ UST_EMPTY;
             }
    Context.Strlen=0;
     while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
     if (Context.Strlen == __STRING_MAX_LENGTH__)
                     {
                     _RETURN_ UST_STRNOENDMARK;
                     }
     if (plittleEndian)
             {
             Context.EndianRequest = true;
             Context.LittleEndian = *plittleEndian;
             }
         else
             {
             Context.EndianRequest=false;
             Context.LittleEndian = is_little_endian();
             }

     ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF32_LE : ZBOM_UTF32_BE;

     // Context.Strlen has the number of char units for the input string
     size_t wBOMSize=0;
     Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
     if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                             {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
     wInPtr=pInString+wBOMSize;
     Context.setEffective(wInPtr); // set string pointer to character next BOM if any

     Context.utfSetPosition<utf32_t>(wInPtr);
     /*  NB: for utf32 strlen and charcount are the same */
     allocateUnitsBZero(Context.Strlen+1); // starting with current number of utf8 character units in input string

     while (true)
     {
         wOutPtr=Data+Context.OutUnitOffset;  // to be recomputed each iteration because Data pointer could change

         for (size_t wi=0;wi<sizeof(wUtf8Char);wi++)
                                         wUtf8Char[wi]=0;
//         Context.setPosition( wInPtr);

         Context.InPtr = (uint8_t*)wInPtr;
         Context.InUnitOffset = wInPtr-pInString;
         Context.InByteOffset = Context.InPtr -Context.Start;

         Context.OutPtr= (uint8_t*)wOutPtr;
//         Context.OutUnitOffset = wOutUnitOffset;
         Context.OutByteOffset = ((uint8_t*)Data)-((uint8_t*)wOutPtr);


         Context.Status = utf32Decode(wInPtr,&wCodePoint,&wInCount,plittleEndian);

         if (Context.Status==UST_ENDOFSTRING)
                                         break;

         Context.CharUnits+=wInCount;
         Context.CodePoint=wCodePoint;

         if (Context.Status < 0)
             {
             Context.ErrTo ++;
             if (Context.TraceTo)
                 {
                 wError.Status = Context.Status;
                 wError.Action=Context.Action;
                 wError.ToFrom= SCErr_ToUnicode;
                 wError.UnitOffset = Context.InUnitOffset;
                 wError.ByteOffset = Context.InByteOffset;
                 wError.Ptr = Context.InPtr;
                 wError.codePoint = wCodePoint;
                 wError.Size=wInCount;
                 Context.ErrorQueue->push(wError);
                 }
              if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                 {
                                 addConditionalTermination();// end of string mark
                                 _RETURN_ Context.Status;
                                 }
              if (Context.Action==ZCNV_Skip)
                  {
                  wInPtr += wInCount;                // just skip
                  Context.Count++;
                  continue;
                  }
              if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
                 {
                 size_t wEscapeSize=0;
                 while (true)
                     {
                     ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                     UST_Status_type wSt=utfEscapeUtf32<utf8_t>(wOutPtr,
                                                                wInPtr,
                                                                wInCount,
                                                                wRemaining,
                                                                Context.Action,
                                                                wEscapeSize);
                     if (wSt!=UST_TRUNCATED)
                            break;
                     allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                     wOutPtr = Data + Context.OutUnitOffset;
                     }
                 Context.OutUnitOffset += wEscapeSize;
                 Context.Escaped++;
                 Context.OutUnitOffset +=wEscapeSize;
                 wOutPtr+=wEscapeSize;
                 wInPtr+= wInCount;
                 Context.Count++;
                 continue;
                 }// Context.Action >= ZCNV_Escape_xx
/*  replacement is made within utf32Decode : wCodePoint contains replacement char */
             Context.Substit++;
             }//if (Context.Status < 0)

         Context.CodePoint=wCodePoint;
         Context.Status= utf8Encode(wUtf8Char,&wOutCount,wCodePoint,nullptr);
         if (Context.Status < 0)
             {
             Context.ErrFrom++;
             if (Context.TraceFrom)
                 {
                 wError.Status = Context.Status;
                 wError.Action=Context.Action;
                 wError.ToFrom= SCErr_FromUnicode;
                 wError.UnitOffset = Context.OutUnitOffset;
                 wError.ByteOffset = Context.OutByteOffset;
                 wError.Ptr = Context.OutPtr;
                 wError.codePoint = wCodePoint;
                 wError.Size=wOutCount;
                 Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             addConditionalTermination();// end of string mark
                             _RETURN_ Context.Status;
                             }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
                {
                size_t wEscapeSize=0;
                while (true)
                    {
                    ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                    UST_Status_type wSt=utfEscapeUtf32<utf8_t>(wOutPtr,
                                                               wInPtr,
                                                               wInCount,
                                                               wRemaining,
                                                               Context.Action,
                                                               wEscapeSize);
                    if (wSt!=UST_TRUNCATED)
                           break;
                    allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                    wOutPtr = Data + Context.OutUnitOffset;
                    }
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }// Context.Action >= ZCNV_Escape_xx

             Context.Substit++;
             //in any other error cases, use replacement character (set by utf8Encode)
             }

         ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
         if (wCountReminder < 0)
             {
             wCountReminder = -wCountReminder;
             wCountReminder++;
             allocateUnitsBZero(wCountReminder);
             wOutPtr = Data + Context.OutUnitOffset;
             }

         size_t wi=0;
         while ((wi < wOutCount)&&(wUtf8Char[wi]))
                         {
                         *wOutPtr = wUtf8Char[wi];
                         wOutPtr++;
//                         wOutUnitOffset++;
                         Context.OutUnitOffset++;
                         wi++;
                         }
//         nadd(wUtf8Char,wOutCount);  // extends allocation only when necessary after end of string

         Context.Count ++; // One character has been processed
         Context.OutUnitOffset += wOutCount;
         wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

     }// while true
     addConditionalTermination();
//     *wUtf8Ptr=(utf8_t)'\0';
     _RETURN_ UST_SUCCESS;

} // utf8VaryingString::fromUtf32


UST_Status_type
utf8VaryingString::toUtf16(utf16VaryingString &pUtf16,ZBool *pEndian)
{
_MODULEINIT_
const utf8_t *wInPtr=Data;
utf32_t wCodePoint;
size_t wInCount=0,wOutCount=0;
utf16_t wUtf16Char[3];
size_t wOutUnitOffset=0;
utf16_t* wOutPtr;
utfSCErr_struct wError;

     pUtf16.clearData();
     Context.reset();
     Context.setStart(wInPtr);
     Context.OutUnitOffset=0;

     pUtf16.allocateUnitsBZero(getUnitCount()); // starting with current number of character units

     while (true)
         {
         wOutPtr=pUtf16.Data+Context.OutUnitOffset;
//         Context.setPosition(wInPtr);
         Context.InPtr = (uint8_t*)wInPtr;
         Context.InUnitOffset = wInPtr-Data;
         Context.InByteOffset = Context.InPtr -Context.Start;

         Context.OutPtr= (uint8_t*)wOutPtr;
//         Context.OutUnitOffset = wOutUnitOffset;
         Context.OutByteOffset = ((uint8_t*)pUtf16.Data)-((uint8_t*)wOutPtr);

         Context.Status=utf8Decode(wInPtr,&wCodePoint,&wInCount);
         if (Context.Status==UST_ENDOFSTRING)
                                         break;
         if (Context.Status < 0)
             {
             Context.ErrTo ++;
             if (Context.TraceTo)
                 {
                 wError.Status = Context.Status;
                 wError.Action=Context.Action;
                 wError.ToFrom= SCErr_ToUnicode;
                 wError.UnitOffset = Context.InUnitOffset;
                 wError.ByteOffset = Context.InByteOffset;
                 wError.Ptr = Context.InPtr;
                 wError.codePoint = wCodePoint;
                 wError.Size=wInCount;
                 Context.ErrorQueue->push(wError);
                 }

              if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                 {
                                 pUtf16.addConditionalTermination();// end of string mark
                                 _RETURN_ Context.Status;
                                 }

             Context.Substit++;
             }
         Context.CharUnits += wInCount;
         Context.CodePoint=wCodePoint;
         Context.Status=utf16Encode((utf16_t*)wUtf16Char,&wOutCount,wCodePoint, pEndian);
         if (Context.Status < 0)
             {
             Context.ErrFrom ++;
             if (Context.TraceFrom)
                 {
                 wError.Status = Context.Status;
                 wError.Action=Context.Action;
                 wError.ToFrom= SCErr_FromUnicode;
                 wError.UnitOffset = Context.OutUnitOffset;
                 wError.ByteOffset = Context.OutByteOffset;
                 wError.Ptr = Context.OutPtr;
                 wError.codePoint = wCodePoint;
                 wError.Size=wOutCount;
                 Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             pUtf16.addConditionalTermination();// end of string mark
                             _RETURN_ Context.Status;
                             }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;
                 Context.Count++;
                 continue;
                 }
             //in any other error cases, use replacement character
             Context.Substit++;
             }
         ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
         if (wCountReminder < 0)
             {
             wCountReminder = -wCountReminder;
             wCountReminder++;
             allocateUnitsBZero(wCountReminder);
             wOutPtr = pUtf16.Data + Context.OutUnitOffset;
             }

         size_t wi=0;
         while ((wi < wOutCount)&&(wUtf16Char[wi]))
                         {
                         *wOutPtr = wUtf16Char[wi];
                         wOutPtr++;
                         Context.OutUnitOffset++;
                         wi++;
                         }
//         pUtf16.nadd(wUtf16Char,wOutCount);
         wInPtr += wInCount;
//         wOutUnitOffset += wOutCount;
         Context.Count ++;
         }
     _RETURN_ UST_SUCCESS;
 }//utf8VaryingString::toUtf16

UST_Status_type
utf8VaryingString::toUtf32(utf32VaryingString &pUtf32, ZBool *pEndian)
{
_MODULEINIT_
const utf8_t *wInPtr=Data;
utf32_t wCodePoint;
size_t wInCount=0,wOutCount=0;
size_t wOutUnitOffset=0;
utf32_t* wOutPtr;
utfSCErr_struct wError;

    pUtf32.clearData();
    Context.reset();
    Context.setStart(wInPtr);
    Context.OutUnitOffset=0;

    pUtf32.allocateUnitsBZero(UnitCount); // starting with current number of character units

    while (true)
        {
        wOutPtr=pUtf32.Data+Context.OutUnitOffset;
//        Context.setPosition(wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-Data;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
        Context.OutUnitOffset = Context.OutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)pUtf32.Data)-((uint8_t*)wOutPtr);

        Context.Status=utf8Decode(wInPtr,&wCodePoint,&wInCount);
        if (Context.Status==UST_ENDOFSTRING)
                                        break;
        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }

             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                pUtf32.addConditionalTermination();// end of string mark
                                _RETURN_ Context.Status;
                                }
            Context.Substit++;
            }
        Context.CharUnits += wInCount;
        Context.CodePoint=wCodePoint;
        Context.Status=utf32Encode(&wCodePoint,wCodePoint, pEndian);
        wOutCount=1; // Because utf32 always 1
        if (Context.Status < 0)
            {
            Context.ErrFrom ++;
            if (Context.TraceFrom)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_FromUnicode;
                wError.UnitOffset = Context.OutUnitOffset;
                wError.ByteOffset = Context.OutByteOffset;
                wError.Ptr = Context.OutPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wOutCount;
                Context.ErrorQueue->push(wError);
               }
            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                            {
                            pUtf32.addConditionalTermination();// end of string mark
                            _RETURN_ Context.Status;
                            }
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;
                Context.Count++;
                continue;
                }
            //in any other error cases, use replacement character
            Context.Substit++;
            }
        ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
        if (wCountReminder < 0)
            {
            wCountReminder = -wCountReminder;
            wCountReminder++;
            allocateUnitsBZero(wCountReminder);
            wOutPtr = pUtf32.Data + Context.OutUnitOffset;
            }

        *wOutPtr = wCodePoint;
        wOutPtr++;
        Context.OutUnitOffset++;

//        pUtf32.addUtfUnit(wCodePoint);
        wInPtr += wInCount;
        Context.Count ++;
        }
    _RETURN_ UST_SUCCESS;
}//utf8VaryingString::toUtf32
 //================ utf16VaryingString conversions to and from other formats==============================


ZDataBuffer&
utf16VaryingString::toCString(ZDataBuffer& pZDB)
{
 pZDB.clear();
 char *wStr=nullptr;

    Context.Status=utf16StrToCharStr(&wStr,_Base::getCharset(), Data,getByteSize(), WarningSignal, nullptr);
                        /* see if nullptr could be littleEndian parameter within string descriptor */
    if (wStr)
         pZDB.setString(wStr);
    _free(wStr);
 return pZDB;
}

UST_Status_type
utf16VaryingString::fromUtf8(const utf8_t* pInString)
{
_MODULEINIT_

utf32_t wCodePoint=0;
utf16_t wUtf16Char[3];

const utf8_t*   wInPtr;
size_t          wInCount=0;

utf16_t*        wOutPtr;
size_t         wOutCount=0;
size_t          wOutUnitOffset=0;

utfSCErr_struct wError;

    clearData();
    Context.reset();
    Context.setStart(pInString);
    Context.OutUnitOffset=0;

    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
            {
            Context.Status=UST_EMPTY;
            _RETURN_ UST_EMPTY;
            }

    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }
    // Context.Strlen has the number of char units for the string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=ZBOM_UTF8))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;

    Context.setEffective(wInPtr); // set string pointer to character next BOM if any
    Context.utfSetPosition<utf8_t>(wInPtr);

    Context.Status=utf8CharCount(wInPtr,&wInCount); // count number of utf8 chars

    if (Context.Status < UST_SEVERE)
                {
                _RETURN_ Context.Status;
                }

    wInCount++;  // size in char unit of input string including endofstring mark (for chars allocation)

    wOutPtr=allocateUnitsBZero(wInCount); // starting with current number of utf8 character units in input string

    wInPtr=Context.utfGetEffective<utf8_t>();
    while (true)
    {
//        Context.utfSetPosition<utf8_t>( wInPtr);// sets position + UnitOffset + ByteOffset
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
//        Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)Data)-((uint8_t*)wOutPtr);

        Context.Status = utf8Decode(wInPtr,&wCodePoint,&wInCount); // wInCount is reused as size of char

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            /* set up Error reporting context */
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe or 'stop on error' requested
                                {
                                addConditionalTermination();// end of string mark
                                _RETURN_ Context.Status;
                                }
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;                // just skip
                Context.Count++;
                continue;
                }
            if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
               {
               size_t wEscapeSize=0;
               while (true)
                   {
                   ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                   UST_Status_type wSt=utfEscapeUtf8<utf16_t>(wOutPtr,
                                                              wInPtr,
                                                              wInCount,
                                                              wRemaining,
                                                              Context.Action,
                                                              wEscapeSize);
                   if (wSt!=UST_TRUNCATED)
                          break;
                   allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                   wOutPtr = Data + Context.OutUnitOffset;
                   }
               Context.OutUnitOffset += wEscapeSize;
               Context.Escaped++;
               Context.OutUnitOffset +=wEscapeSize;
               wOutPtr+=wEscapeSize;
               wInPtr+= wInCount;
               Context.Count++;
               continue;
               }// Context.Action >= ZCNV_Escape_xx
/*  replacement is made within utf16Decode : wCodePoint contains replacement char */
            Context.Substit++;  // just signal a substitution is made and continue with replacement char
            }

        Context.Status= utf16Encode(wUtf16Char,&wOutCount,wCodePoint,nullptr);
        if (Context.Status < 0)
            {
            Context.ErrFrom ++;
            if (Context.TraceFrom)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_FromUnicode;
                wError.UnitOffset = Context.OutUnitOffset;
                wError.ByteOffset = Context.OutByteOffset;
                wError.Ptr = Context.OutPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wOutCount;
                Context.ErrorQueue->push(wError);
               }

            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                            {
                            addConditionalTermination();// end of string mark
                            _RETURN_ Context.Status;
                            }
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;
                Context.Count++;
                continue;
                }
            if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
               {
               size_t wEscapeSize=0;
               while (true)
                   {
                   ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                   UST_Status_type wSt=utfEscapeUtf8<utf16_t>(wOutPtr,
                                                              wInPtr,
                                                              wInCount,
                                                              wRemaining,
                                                              Context.Action,
                                                              wEscapeSize);
                   if (wSt!=UST_TRUNCATED)
                          break;
                   allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                   wOutPtr = Data + Context.OutUnitOffset;
                   }
               Context.OutUnitOffset += wEscapeSize;
               Context.Escaped++;
               Context.OutUnitOffset +=wEscapeSize;
               wOutPtr+=wEscapeSize;
               wInPtr+= wInCount;
               Context.Count++;
               continue;
               }// Context.Action >= ZCNV_Escape_xx
            /*  replacement is made within utf16Encode : wUtf16Char contains replacement char - wOutCount is set accordingly*/

            Context.Substit++;
            }
// extends allocation if necessary

        ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
        if (wCountReminder < 0)
            {
            wCountReminder = -wCountReminder;
            wCountReminder++;
            allocateUnitsBZero(wCountReminder);
            wOutPtr = Data + Context.OutUnitOffset;
            }
        size_t wi=0;
        while ((wi < wOutCount)&&(wUtf16Char[wi]))
                        {
                        *wOutPtr= wUtf16Char[wi];
                        wOutPtr++;
                        Context.OutUnitOffset++;
                        wi++;
                        }
//        nadd(wUtf16Char,wOutCount);  // extends allocation only when necessary after end of string
//        wOutPtr+= wOutCount;

        Context.Count ++; // One character has been processed
        wInPtr += wInCount;  // NB: in case of error utf16 count is correctly set to try to skip errored character units
    }// while true
    addConditionalTermination();

    _RETURN_ UST_SUCCESS;

}// utf16VaryingString::fromUtf8


UST_Status_type
utf16VaryingString::fromUtf16(const utf16_t* pInString, ZBool *plittleEndian)
{
 _MODULEINIT_
utf32_t wCodePoint=0;
utf16_t wUtf16Char[3];

const utf16_t*  wInPtr;
size_t         wInCount=0;

utf16_t*        wOutPtr;
size_t         wOutCount=0;
size_t          wOutUnitOffset=0;

utfSCErr_struct wError;

     clearData();
     Context.reset();
     Context.setStart(pInString);
     Context.OutUnitOffset=0;

     if (!pInString)
             {
             Context.Status=UST_NULLPTR;
             _RETURN_ UST_NULLPTR;
             }
     if (!*pInString)
             {
             Context.Status=UST_EMPTY;
             _RETURN_ UST_EMPTY;
             }

     Context.Strlen=0;
     while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
     if (Context.Strlen == __STRING_MAX_LENGTH__)
                     {
                     _RETURN_ UST_STRNOENDMARK;
                     }
     if (plittleEndian)
             {
             Context.EndianRequest = true;
             Context.LittleEndian = *plittleEndian;
             }
         else
             {
             Context.EndianRequest=false;
             Context.LittleEndian = is_little_endian();
             }

     ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF16_LE : ZBOM_UTF16_BE;

     // Context.Strlen has the number of char units for the input string
     size_t wBOMSize=0;
     Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
     if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                             {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
     wInPtr=pInString+wBOMSize;

     Context.setEffective(wInPtr); // set string pointer to character next BOM if any
     Context.utfSetPosition<utf16_t>(wInPtr);

 //    allocateCharsBZero(Context.Strlen+1); // starting with current number of utf8 character units in input string
     wOutPtr=allocateUnitsBZero(Context.Strlen); // starting with current number of utf16 character units in input string

     wInPtr=Context.utfGetEffective<utf16_t>();

     while (true)
     {
        wOutPtr = Data+Context.OutUnitOffset;

//         Context.setPosition( wInPtr);
         Context.InPtr = (uint8_t*)wInPtr;
         Context.InUnitOffset = wInPtr-pInString;
         Context.InByteOffset = Context.InPtr -Context.Start;

         Context.OutPtr= (uint8_t*)wOutPtr;
//         Context.OutUnitOffset = wOutUnitOffset;
         Context.OutByteOffset = ((uint8_t*)Data)-((uint8_t*)wOutPtr);

         Context.Status = utf16Decode(wInPtr,&wCodePoint,&wInCount,plittleEndian);

         if (Context.Status==UST_ENDOFSTRING)
                                         break;
         Context.CharUnits+=wInCount;
         Context.CodePoint=wCodePoint;

         if (Context.Status < 0)
             {
             Context.ErrTo ++;
             if (Context.TraceTo)
                 {
                 wError.Status = Context.Status;
                 wError.Action=Context.Action;
                 wError.ToFrom= SCErr_ToUnicode;
                 wError.UnitOffset = Context.InUnitOffset;
                 wError.ByteOffset = Context.InByteOffset;
                 wError.Ptr = Context.InPtr;
                 wError.codePoint = wCodePoint;
                 wError.Size=wInCount;
                 Context.ErrorQueue->push(wError);
                 }
              if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                 {
                                 addConditionalTermination();// end of string mark
                                 _RETURN_ Context.Status;
                                 }
              if (Context.Action==ZCNV_Skip)
                  {
                  wInPtr += wInCount;                // just skip
                  Context.Count++;
                  continue;
                  }
              if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
                 {
                 size_t wEscapeSize=0;
                 while (true)
                     {
                     ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                     UST_Status_type wSt=utfEscapeUtf16<utf16_t>(wOutPtr,
                                                                wInPtr,
                                                                wInCount,
                                                                wRemaining,
                                                                Context.Action,
                                                                wEscapeSize);
                     if (wSt!=UST_TRUNCATED)
                            break;
                     allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                     wOutPtr = Data + Context.OutUnitOffset;
                     }
                 Context.OutUnitOffset += wEscapeSize;
                 Context.Escaped++;
                 Context.OutUnitOffset +=wEscapeSize;
                 wOutPtr+=wEscapeSize;
                 wInPtr+= wInCount;
                 Context.Count++;
                 continue;
                 }// Context.Action >= ZCNV_Escape_xx
 /*  replacement is made within utf16Decode : wCodePoint contains replacement char */
             Context.Substit++;
             }

         Context.CodePoint=wCodePoint;
         Context.Status= utf16Encode(wUtf16Char,&wOutCount,wCodePoint,nullptr);
         if (Context.Status < 0)
             {
             Context.ErrFrom ++;
             /* set up Error reporting context */
             if (Context.TraceFrom)
                 {
                 wError.Status = Context.Status;
                 wError.Action=Context.Action;
                 wError.ToFrom= SCErr_FromUnicode;
                 wError.UnitOffset = Context.InUnitOffset;
                 wError.ByteOffset = Context.InByteOffset;
                 wError.Ptr = Context.InPtr;
                 wError.codePoint = wCodePoint;
                 wError.Size=wInCount;
                 Context.ErrorQueue->push(wError);
                 }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             addConditionalTermination();// end of string mark
                             _RETURN_ Context.Status;
                             }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
                {
                size_t wEscapeSize=0;
                while (true)
                    {
                    ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                    UST_Status_type wSt=utfEscapeUtf16<utf16_t>(wOutPtr,
                                                               wInPtr,
                                                               wInCount,
                                                               wRemaining,
                                                               Context.Action,
                                                               wEscapeSize);
                    if (wSt!=UST_TRUNCATED)
                           break;
                    allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                    wOutPtr = Data + Context.OutUnitOffset;
                    }
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }// Context.Action >= ZCNV_Escape_xx
/*  replacement is made within utf16Encode : wUtf16Char contains replacement char - wOutCount is set accordingly*/
             Context.Substit++;
             }

         ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
         if (wCountReminder < 0)
             {
             wCountReminder = -wCountReminder;
             wCountReminder++;
             allocateUnitsBZero(wCountReminder);
             wOutPtr = Data + Context.OutUnitOffset;
             }

         size_t wi=0;
         while ((wi < wOutCount)&&(wUtf16Char[wi]))
                         {
                         *wOutPtr = wUtf16Char[wi];
                         wOutPtr++;
                         Context.OutUnitOffset++;
                         wi++;
                         }

         Context.Count ++; // One character has been processed

         wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

     }// while true
     addConditionalTermination();
//     *wUtf8Ptr=(utf8_t)'\0';
     _RETURN_ UST_SUCCESS;
} // utf16VaryingString::fromUtf32

UST_Status_type
utf16VaryingString::fromUtf32(const utf32_t* pInString, ZBool *plittleEndian)
{
 _MODULEINIT_
utf32_t wCodePoint=0;
utf16_t wUtf16Char[3];

const utf32_t*  wInPtr;
size_t         wInCount=0;

utf16_t*        wOutPtr;
size_t         wOutCount=0;
size_t          wOutUnitOffset=0;

utfSCErr_struct wError;

     clearData();
     Context.reset();
     Context.setStart(pInString);
     Context.OutUnitOffset=0;

     if (!pInString)
             {
             Context.Status=UST_NULLPTR;
             _RETURN_ UST_NULLPTR;
             }
     if (!*pInString)
             {
             Context.Status=UST_EMPTY;
             _RETURN_ UST_EMPTY;
             }

     Context.Strlen=0;
     while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
     if (Context.Strlen == __STRING_MAX_LENGTH__)
                     {
                     _RETURN_ UST_STRNOENDMARK;
                     }
     if (plittleEndian)
             {
             Context.EndianRequest = true;
             Context.LittleEndian = *plittleEndian;
             }
         else
             {
             Context.EndianRequest=false;
             Context.LittleEndian = is_little_endian();
             }

     ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF32_LE : ZBOM_UTF32_BE;

     // Context.Strlen has the number of char units for the input string
     size_t wBOMSize=0;
     Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
     if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                             {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
     wInPtr=pInString+wBOMSize;

     Context.setEffective(wInPtr); // set string pointer to character next BOM if any
     Context.utfSetPosition<utf32_t>(wInPtr);


 //    allocateCharsBZero(Context.Strlen+1); // starting with current number of utf8 character units in input string
     wOutPtr=allocateUnitsBZero(Context.Strlen); // starting with current number of utf32 character units in input string

     wInPtr=Context.utfGetEffective<utf32_t>();

     while (true)
     {
        wOutPtr = Data+Context.OutUnitOffset;

//         Context.setPosition( wInPtr);
         Context.InPtr = (uint8_t*)wInPtr;
         Context.InUnitOffset = wInPtr-pInString;
         Context.InByteOffset = Context.InPtr -Context.Start;

         Context.OutPtr= (uint8_t*)wOutPtr;
//         Context.OutUnitOffset = wOutUnitOffset;
         Context.OutByteOffset = ((uint8_t*)Data)-((uint8_t*)wOutPtr);

         Context.Status = utf32Decode(wInPtr,&wCodePoint,&wInCount,plittleEndian);

         if (Context.Status==UST_ENDOFSTRING)
                                         break;
         Context.CharUnits+=wInCount;
         Context.CodePoint=wCodePoint;

         if (Context.Status < 0)
             {
             Context.ErrTo ++;
             if (Context.TraceTo)
                 {
                 wError.Status = Context.Status;
                 wError.Action=Context.Action;
                 wError.ToFrom= SCErr_ToUnicode;
                 wError.UnitOffset = Context.InUnitOffset;
                 wError.ByteOffset = Context.InByteOffset;
                 wError.Ptr = Context.InPtr;
                 wError.codePoint = wCodePoint;
                 wError.Size=wInCount;
                 Context.ErrorQueue->push(wError);
                 }
              if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                 {
                                 addConditionalTermination();// end of string mark
                                 _RETURN_ Context.Status;
                                 }
              if (Context.Action==ZCNV_Skip)
                  {
                  wInPtr += wInCount;                // just skip
                  Context.Count++;
                  continue;
                  }
              if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
                 {
                 size_t wEscapeSize=0;
                 while (true)
                     {
                     ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                     UST_Status_type wSt=utfEscapeUtf32<utf16_t>(wOutPtr,
                                                                wInPtr,
                                                                wInCount,
                                                                wRemaining,
                                                                Context.Action,
                                                                wEscapeSize);
                     if (wSt!=UST_TRUNCATED)
                            break;
                     allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                     wOutPtr = Data + Context.OutUnitOffset;
                     }
                 Context.OutUnitOffset += wEscapeSize;
                 Context.Escaped++;
                 Context.OutUnitOffset +=wEscapeSize;
                 wOutPtr+=wEscapeSize;
                 wInPtr+= wInCount;
                 Context.Count++;
                 continue;
                 }// Context.Action >= ZCNV_Escape_xx
 /*  replacement is made within utf32Decode : wCodePoint contains replacement char */
             Context.Substit++;
             }

         Context.CodePoint=wCodePoint;
         Context.Status= utf16Encode(wUtf16Char,&wOutCount,wCodePoint,nullptr);
         if (Context.Status < 0)
             {
             Context.ErrFrom ++;
             /* set up Error reporting context */
             if (Context.TraceFrom)
                 {
                 wError.Status = Context.Status;
                 wError.Action=Context.Action;
                 wError.ToFrom= SCErr_FromUnicode;
                 wError.UnitOffset = Context.InUnitOffset;
                 wError.ByteOffset = Context.InByteOffset;
                 wError.Ptr = Context.InPtr;
                 wError.codePoint = wCodePoint;
                 wError.Size=wInCount;
                 Context.ErrorQueue->push(wError);
                 }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             addConditionalTermination();// end of string mark
                             _RETURN_ Context.Status;
                             }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
                {
                size_t wEscapeSize=0;
                while (true)
                    {
                    ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                    UST_Status_type wSt=utfEscapeUtf32<utf16_t>(wOutPtr,
                                                               wInPtr,
                                                               wInCount,
                                                               wRemaining,
                                                               Context.Action,
                                                               wEscapeSize);
                    if (wSt!=UST_TRUNCATED)
                           break;
                    allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                    wOutPtr = Data + Context.OutUnitOffset;
                    }
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }// Context.Action >= ZCNV_Escape_xx
/*  replacement is made within utf16Encode : wUtf16Char contains replacement char - wOutCount is set accordingly*/
             Context.Substit++;
             }

         ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
         if (wCountReminder < 0)
             {
             wCountReminder = -wCountReminder;
             wCountReminder++;
             allocateUnitsBZero(wCountReminder);
             wOutPtr = Data + Context.OutUnitOffset;
             }

         size_t wi=0;
         while ((wi < wOutCount)&&(wUtf16Char[wi]))
                         {
                         *wOutPtr = wUtf16Char[wi];
                         wOutPtr++;
                         Context.OutUnitOffset++;
                         wi++;
                         }

         Context.Count ++; // One character has been processed

         wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

     }// while true
     addConditionalTermination();
//     *wUtf8Ptr=(utf8_t)'\0';
     _RETURN_ UST_SUCCESS;
} // utf16VaryingString::fromUtf32


utf8VaryingString*
utf16VaryingString::toUtf8(utf8VaryingString& pUtf8)
{
const utf16_t *wInData=Data;
utf32_t wUtf32Char;
size_t wInCount=0,wOutCount=0;
utf8_t wUtf8Char[5];

     pUtf8.clearData();
     Context.reset();
     Context.setStart(wInData);

     pUtf8.allocateUnitsBZero(getUnitCount()); // starting with current number of character units

     while (true)
         {
         Context.setPosition(wInData);
         Context.Status=utf16Decode(wInData,&wUtf32Char,&wInCount,nullptr);  // endian is supposed to be current system endian
         if (Context.Status==UST_ENDOFSTRING)
                                         break;
         if (Context.Status < 0)
             {
             Context.ErrFrom ++;

              if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                 {
                                 pUtf8.addConditionalTermination();// end of string mark
                                 return nullptr; // exit with nullptr return
                                 }
             wUtf32Char=cst_Unicode_Replct_utf32;  //in any other case, use replacement character
             Context.Substit++;
             }
         Context.CodePoint=wUtf32Char;
         Context.Status=utf8Encode(wUtf8Char,&wOutCount,wUtf32Char);
         if (Context.Status < 0)
             {
             Context.ErrTo ++;
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             pUtf8.addConditionalTermination();// end of string mark
                             return nullptr; // exit with nullptr return
                             }
             //in any other error cases, use replacement character
             for (wOutCount=0;cst_Unicode_Replct_utf8[wOutCount];wOutCount++)
                                             wUtf8Char[wOutCount]=cst_Unicode_Replct_utf8[wOutCount];
             Context.Substit++;
             }

         pUtf8.nadd(wUtf8Char,wOutCount);
         wInData += wInCount;
         }
     return &pUtf8;
}// utf16VaryingString::toUtf8

utf32VaryingString*
utf16VaryingString::toUtf32(utf32VaryingString& pUtf32)
{
const utf16_t *wInPtr=Data;
utf32_t wUtf32Char;
size_t wInCount=0;

    pUtf32.clearData();
    Context.reset();
    Context.setStart(wInPtr);

    pUtf32.allocateUnitsBZero(getUnitCount()); // starting with current number of character units

    while (true)
        {
        Context.setPosition(wInPtr);
        Context.Status=utf16Decode(wInPtr,&wUtf32Char,&wInCount,nullptr);
        if (Context.Status==UST_ENDOFSTRING)
                                        break;
        if (Context.Status < 0)
            {
            Context.ErrFrom ++;

             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                pUtf32.addConditionalTermination();// end of string mark
                                return nullptr; // exit with nullptr return
                                }
            wUtf32Char=cst_Unicode_Replct_utf32;  //in any other case, use replacement character
            Context.Substit++;
            }
        Context.CharUnits += wInCount;
        Context.CodePoint=wUtf32Char;
        Context.Status=utf32Encode(&wUtf32Char,wUtf32Char, nullptr);
        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                            {
                            pUtf32.addConditionalTermination();// end of string mark
                            return nullptr; // exit with nullptr return
                            }
            //in any other error cases, use replacement character
            Context.Status=utf32Encode(&wUtf32Char,cst_Unicode_Replct_utf32, nullptr);
            Context.Substit++;
            }

        pUtf32.addUtfUnit(wUtf32Char);
        wInPtr += wInCount;
        }
    return &pUtf32;
}// utf16VaryingString::toUtf32

 /**
 * @brief utf16VaryingString::getLittleEndian returns a clone as utf16VaryingString with a certified little endian content
 * @param pLittleEndian an utf16VaryingString that will receive cloned data, modified to little endian if necessary.
 * @return utf16VaryingString with a certified big endian content
 */
 utf16VaryingString&
 utf16VaryingString::getLittleEndian(utf16VaryingString& pLittleEndian)
 {
  if (is_little_endian())
          {
          pLittleEndian=*this;
          return pLittleEndian;
          }

  pLittleEndian.allocateUnitsBZero(_Base::getUnitCount());
  utf16_t* wPtr=_Base::getContentPtr();
  utf16_t* wPtr1=pLittleEndian.getContentPtr();
  while (*wPtr)
          *wPtr1=forceReverseByteOrder<utf16_t>(*wPtr);
  return pLittleEndian;
 }

 /**
  * @brief utf16VaryingString::getBigEndian returns a clone as utf16VaryingString with a certified big endian content
  * @param pBigEndian an utf16VaryingString that will receive cloned data, modified to big endian if necessary.
  * @return utf16VaryingString with a certified big endian content
  */

 utf16VaryingString&
 utf16VaryingString::getBigEndian(utf16VaryingString& pBigEndian)
 {
     if (!is_little_endian())
             {
             pBigEndian=*this;
             return pBigEndian;
             }
     pBigEndian.allocateUnitsBZero(_Base::getUnitCount());
     utf16_t* wPtr=_Base::getContentPtr();
     utf16_t* wPtr1=pBigEndian.getContentPtr();
     while (*wPtr)
             *wPtr1=forceReverseByteOrder<utf16_t>(*wPtr);
     return pBigEndian;
 }



//================ utf32=============================
/**
* @brief utf32VaryingString::toCString tries to convert utf32 varying string content to char* in a returned ZDataBuffer pZDB.
* @param pZDB a ZDataBuffer that will contain in return the converted string to char*
* @return ZDataBuffer pZDB with converted content to char* if successfull or nullptr if a problem occurred.
*/
ZDataBuffer*
utf32VaryingString::toCString(ZDataBuffer& pZDB)
{
 pZDB.clearData();
 char *wStr=nullptr;
    utf32StrToCharStr(&wStr,_Base::getCharset(), Data,getByteSize(),WarningSignal, nullptr);
            /* see if nullptr could be littleEndian parameter within string descriptor */
    if (wStr)
        pZDB.setString(wStr);
 _free(wStr);
 return &pZDB;
}
/**
* @brief utf32VaryingString::fromUtf8
* @param pUtf8
* @return
*/
UST_Status_type
utf32VaryingString::fromUtf8(const utf8_t* pInString)
{
_MODULEINIT_
utf32_t wCodePoint=0;

const utf8_t* wInPtr;
size_t wInCount=0;

utf32_t*        wOutPtr;
size_t         wOutCount=0;
size_t          wOutUnitOffset=0;

utfSCErr_struct wError;

    clearData();
    Context.reset();
    Context.setStart(pInString);
    Context.OutUnitOffset=0;

    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
            {
            Context.Status=UST_EMPTY;
            _RETURN_ UST_EMPTY;
            }
    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }

    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=ZBOM_UTF8))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;

    Context.setEffective(wInPtr); // set string pointer to character next BOM if any
    Context.utfSetPosition<utf8_t>(wInPtr);

    Context.Status=utf8CharCount(wInPtr,&wInCount);
    if (Context.Status < UST_SEVERE)
                {
                _RETURN_ Context.Status;
                }
    wInCount++; // size in char unit of input string including endofstring mark (for chars allocation)

    allocateUnitsBZero(wInCount); // starting with current number of utf8 character in input string
    wInPtr=Context.utfGetEffective<utf8_t>();
    while (true)
    {
//        Context.setPosition( wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
//        Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)Data)-((uint8_t*)wOutPtr);

        Context.Status = utf8Decode(wInPtr,&wCodePoint,&wInCount);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            /* set up Error reporting context */
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                addConditionalTermination();// end of string mark
                                _RETURN_ Context.Status;
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
                {
                size_t wEscapeSize=0;
                while (true)
                    {
                    ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                    UST_Status_type wSt=utfEscapeUtf8<utf32_t>(wOutPtr,
                                                               wInPtr,
                                                               wInCount,
                                                               wRemaining,
                                                               Context.Action,
                                                               wEscapeSize);
                    if (wSt!=UST_TRUNCATED)
                           break;
                    allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                    wOutPtr = Data + Context.OutUnitOffset;
                    }
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }// Context.Action >= ZCNV_Escape_xx
/*  replacement is made within utf16Decode : wCodePoint contains replacement char */
            Context.Substit++;
            }

        Context.Status= utf32Encode(&wCodePoint,wCodePoint,nullptr);
        wOutCount=1; // always 1 for utf32
        if (Context.Status < 0)
            {
            Context.ErrFrom ++;
            if (Context.TraceFrom)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_FromUnicode;
                wError.UnitOffset = Context.OutUnitOffset;
                wError.ByteOffset = Context.OutByteOffset;
                wError.Ptr = Context.OutPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wOutCount;
                Context.ErrorQueue->push(wError);
               }
            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                            {
                            addConditionalTermination();// end of string mark
                            _RETURN_ Context.Status;
                            }
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;
                Context.Count++;
                continue;
                }
            if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
               {
               size_t wEscapeSize=0;
               while (true)
                   {
                   ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                   UST_Status_type wSt=utfEscapeUtf8<utf32_t>(wOutPtr,
                                                              wInPtr,
                                                              wInCount,
                                                              wRemaining,
                                                              Context.Action,
                                                              wEscapeSize);
                   if (wSt!=UST_TRUNCATED)
                          break;
                   allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                   wOutPtr = Data + Context.OutUnitOffset;
                   }
               Context.OutUnitOffset += wEscapeSize;
               Context.Escaped++;
               Context.OutUnitOffset +=wEscapeSize;
               wOutPtr+=wEscapeSize;
               wInPtr+= wInCount;
               Context.Count++;
               continue;
               }// Context.Action >= ZCNV_Escape_xx
            /*  replacement is made within utf32Encode : wCodePoint contains replacement char - wOutCount is set accordingly*/

            Context.Substit++;
            }//if (Context.Status < 0)

// extends allocation if necessary

        ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
        if (wCountReminder < 0)
            {
            wCountReminder = -wCountReminder;
            wCountReminder++;
            allocateUnitsBZero(wCountReminder);
            wOutPtr = Data + Context.OutUnitOffset;
            }

        *wOutPtr= wCodePoint;
        wOutPtr++;
        Context.OutUnitOffset++;

//        addUtfUnit(wCodePoint);  // extends allocation only when necessary after end of string

        Context.Count ++; // One character has been processed

        wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

    }// while true
    addConditionalTermination();

    _RETURN_ UST_SUCCESS;

}// utf32VaryingString::fromUtf8



/**
 * @brief utf32VaryingString::fromUtf16 takes an utf16_t input string and loads it to current string after having converted it to utf32_t
 *                                memory is allocated to make converted string content fit.
 * @param pInString         utf16_t string
 * @param plittleEndian     optional : if not omitted and set to true, input string (pInString) is requested to be little endian
 *                          if set to false, input string is requested to be big endian
 *                          if omittted, string is requested to have current system endianness
 * @return
 */
UST_Status_type
utf32VaryingString::fromUtf16(const utf16_t* pInString, ZBool *plittleEndian)
{
_MODULEINIT_
utf32_t wCodePoint=0;

size_t wOutCount=0,wInCount=0;

const utf16_t* wInPtr;
utf32_t* wOutPtr ;
size_t wOutUnitOffset=0;
utfSCErr_struct wError;


    clearData();
    Context.reset();
    Context.setStart(pInString);
    Context.OutUnitOffset=0;

    if (plittleEndian)
         {
         Context.EndianRequest = true;
         Context.LittleEndian = *plittleEndian;
         }
        else
        {
        Context.EndianRequest = false;
        Context.LittleEndian = is_little_endian();
        }


    if (!pInString)
         {
         Context.Status=UST_NULLPTR;
         _RETURN_ UST_NULLPTR;
         }
    if (!*pInString)
        {
        Context.Status=UST_EMPTY;
        _RETURN_ UST_EMPTY;
        }
    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }
    ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF16_LE : ZBOM_UTF16_BE;

    // Context.Strlen has the number of char units for the input string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;

    Context.setEffective(wInPtr); // set string pointer to character next BOM if any
    Context.utfSetPosition<utf16_t>(wInPtr);

    Context.Status =utf16CharCount(wInPtr,&wInCount,plittleEndian);
    if (Context.Status < UST_SEVERE)
                {
                _RETURN_ Context.Status;
                }
    wInCount++;
    allocateUnitsBZero(wInCount); // starting with current number of utf8 character units in input string

    wInPtr=Context.utfGetEffective<utf16_t>();

    while (true)
    {
        wOutPtr = Data+Context.OutUnitOffset;

//     Context.setPosition( wInPtr);

     Context.InPtr = (uint8_t*)wInPtr;
     Context.InUnitOffset = wInPtr-pInString;
     Context.InByteOffset = Context.InPtr -Context.Start;

     Context.OutPtr= (uint8_t*)wOutPtr;
//     Context.OutUnitOffset = wOutUnitOffset;
     Context.OutByteOffset = ((uint8_t*)Data)-((uint8_t*)wOutPtr);

     Context.Status = utf16Decode(wInPtr,&wCodePoint,&wInCount,plittleEndian);

     if (Context.Status==UST_ENDOFSTRING)
                                     break;

     Context.CharUnits+=wInCount;
     Context.CodePoint=wCodePoint;

     if (Context.Status < 0)
         {
         Context.ErrTo ++;
         if (Context.TraceTo)
             {
             wError.Status = Context.Status;
             wError.Action=Context.Action;
             wError.ToFrom= SCErr_ToUnicode;
             wError.UnitOffset = Context.InUnitOffset;
             wError.ByteOffset = Context.InByteOffset;
             wError.Ptr = Context.InPtr;
             wError.codePoint = wCodePoint;
             wError.Size=wInCount;
             Context.ErrorQueue->push(wError);
             }
          if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             addConditionalTermination();// end of string mark
                             _RETURN_ Context.Status;
                             }
          if (Context.Action==ZCNV_Skip)
              {
              wInPtr += wInCount;                // just skip
              Context.Count++;
              continue;
              }
          if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
             {
             size_t wEscapeSize=0;
             while (true)
                 {
                 ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                 UST_Status_type wSt=utfEscapeUtf16<utf32_t>(wOutPtr,
                                                            wInPtr,
                                                            wInCount,
                                                            wRemaining,
                                                            Context.Action,
                                                            wEscapeSize);
                 if (wSt!=UST_TRUNCATED)
                        break;
                 allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                 wOutPtr = Data + Context.OutUnitOffset;
                 }
             Context.OutUnitOffset += wEscapeSize;
             Context.Escaped++;
             Context.OutUnitOffset +=wEscapeSize;
             wOutPtr+=wEscapeSize;
             wInPtr+= wInCount;
             Context.Count++;
             continue;
             }// Context.Action >= ZCNV_Escape_xx
 //        wCodePoint=cst_Unicode_Replct_utf32;  //in any other case, use replacement character
         Context.Substit++;
         }

     Context.CodePoint=wCodePoint;
     Context.Status= utf32Encode(&wCodePoint,wCodePoint,nullptr);
     if (Context.Status < 0)
         {
         Context.ErrFrom ++;
         if (Context.TraceFrom)
             {
             wError.Status = Context.Status;
             wError.Action=Context.Action;
             wError.ToFrom= SCErr_FromUnicode;
             wError.UnitOffset = Context.InUnitOffset;
             wError.ByteOffset = Context.InByteOffset;
             wError.Ptr = Context.InPtr;
             wError.codePoint = wCodePoint;
             wError.Size=wInCount;
             Context.ErrorQueue->push(wError);
             }
         if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                         {
                         addConditionalTermination();// end of string mark
                         _RETURN_ Context.Status;
                         }
         if (Context.Action==ZCNV_Skip)
             {
             wInPtr += wInCount;                // just skip
             Context.Count++;
             continue;
             }
         if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
            {
            size_t wEscapeSize=0;
            while (true)
                {
                ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                UST_Status_type wSt=utfEscapeUtf16<utf32_t>(wOutPtr,
                                                           wInPtr,
                                                           wInCount,
                                                           wRemaining,
                                                           Context.Action,
                                                           wEscapeSize);
                if (wSt!=UST_TRUNCATED)
                       break;
                allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                wOutPtr = Data + Context.OutUnitOffset;
                }
            Context.OutUnitOffset += wEscapeSize;
            Context.Escaped++;
            Context.OutUnitOffset +=wEscapeSize;
            wOutPtr+=wEscapeSize;
            wInPtr+= wInCount;
            Context.Count++;
            continue;
            }// Context.Action >= ZCNV_Escape_xx
/*  replacement is made within utf16Encode : wUtf16Char contains replacement char - wOutCount is set accordingly*/
         Context.Substit++;
         }

     ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
     if (wCountReminder < 0)
         {
         wCountReminder = -wCountReminder;
         wCountReminder++;
         allocateUnitsBZero(wCountReminder);
         wOutPtr = Data + Context.OutUnitOffset;
         }

     *wOutPtr=wCodePoint;
     wOutPtr++;
     Context.OutUnitOffset++;


     Context.Count ++; // One character has been processed

     wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

    }// while true
    addConditionalTermination();
//     *wUtf8Ptr=(utf8_t)'\0';
    _RETURN_ UST_SUCCESS;
}// utf32VaryingString::fromUtf16


UST_Status_type
utf32VaryingString::fromUtf32(const utf32_t* pInString, ZBool *plittleEndian)
{
_MODULEINIT_
utf32_t wCodePoint=0;

size_t wOutCount=0,wInCount=0;

const utf32_t* wInPtr;
utf32_t* wOutPtr ;
size_t wOutUnitOffset=0;
utfSCErr_struct wError;


    clearData();
    Context.reset();
    Context.setStart(pInString);
    Context.OutUnitOffset=0;

    if (plittleEndian)
         {
         Context.EndianRequest = true;
         Context.LittleEndian = *plittleEndian;
         }
        else
        {
        Context.EndianRequest = false;
        Context.LittleEndian = is_little_endian();
        }


    if (!pInString)
         {
         Context.Status=UST_NULLPTR;
         _RETURN_ UST_NULLPTR;
         }
    if (!*pInString)
        {
        Context.Status=UST_EMPTY;
        _RETURN_ UST_EMPTY;
        }
    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }
    ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF32_LE : ZBOM_UTF32_BE;

    // Context.Strlen has the number of char units for the input string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;

    Context.setEffective(wInPtr); // set string pointer to character next BOM if any
    Context.utfSetPosition<utf32_t>(wInPtr);

    allocateUnitsBZero(Context.Strlen); // starting with current number of utf32 character units in input string

    wInPtr=Context.utfGetEffective<utf32_t>();

    while (true)
    {
        wOutPtr = Data+Context.OutUnitOffset;

//     Context.setPosition( wInPtr);

     Context.InPtr = (uint8_t*)wInPtr;
     Context.InUnitOffset = wInPtr-pInString;
     Context.InByteOffset = Context.InPtr -Context.Start;

     Context.OutPtr= (uint8_t*)wOutPtr;
//     Context.OutUnitOffset = wOutUnitOffset;
     Context.OutByteOffset = ((uint8_t*)Data)-((uint8_t*)wOutPtr);

     Context.Status = utf32Decode(wInPtr,&wCodePoint,&wInCount,plittleEndian);

     if (Context.Status==UST_ENDOFSTRING)
                                     break;

     Context.CharUnits+=wInCount;
     Context.CodePoint=wCodePoint;

     if (Context.Status < 0)
         {
         Context.ErrTo ++;
         if (Context.TraceTo)
             {
             wError.Status = Context.Status;
             wError.Action=Context.Action;
             wError.ToFrom= SCErr_ToUnicode;
             wError.UnitOffset = Context.InUnitOffset;
             wError.ByteOffset = Context.InByteOffset;
             wError.Ptr = Context.InPtr;
             wError.codePoint = wCodePoint;
             wError.Size=wInCount;
             Context.ErrorQueue->push(wError);
             }
          if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             addConditionalTermination();// end of string mark
                             _RETURN_ Context.Status;
                             }
          if (Context.Action==ZCNV_Skip)
              {
              wInPtr += wInCount;                // just skip
              Context.Count++;
              continue;
              }
          if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
             {
             size_t wEscapeSize=0;
             while (true)
                 {
                 ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                 UST_Status_type wSt=utfEscapeUtf32<utf32_t>(wOutPtr,
                                                            wInPtr,
                                                            wInCount,
                                                            wRemaining,
                                                            Context.Action,
                                                            wEscapeSize);
                 if (wSt!=UST_TRUNCATED)
                        break;
                 allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                 wOutPtr = Data + Context.OutUnitOffset;
                 }
             Context.OutUnitOffset += wEscapeSize;
             Context.Escaped++;
             Context.OutUnitOffset +=wEscapeSize;
             wOutPtr+=wEscapeSize;
             wInPtr+= wInCount;
             Context.Count++;
             continue;
             }// Context.Action >= ZCNV_Escape_xx
 //        wCodePoint=cst_Unicode_Replct_utf32;  //in any other case, use replacement character
         Context.Substit++;
         }

     Context.CodePoint=wCodePoint;
     Context.Status= utf32Encode(&wCodePoint,wCodePoint,nullptr);
     if (Context.Status < 0)
         {
         Context.ErrFrom ++;
         if (Context.TraceFrom)
             {
             wError.Status = Context.Status;
             wError.Action=Context.Action;
             wError.ToFrom= SCErr_FromUnicode;
             wError.UnitOffset = Context.InUnitOffset;
             wError.ByteOffset = Context.InByteOffset;
             wError.Ptr = Context.InPtr;
             wError.codePoint = wCodePoint;
             wError.Size=wInCount;
             Context.ErrorQueue->push(wError);
             }
         if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                         {
                         addConditionalTermination();// end of string mark
                         _RETURN_ Context.Status;
                         }
         if (Context.Action==ZCNV_Skip)
             {
             wInPtr += wInCount;                // just skip
             Context.Count++;
             continue;
             }
         if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2)) /* escape character according required nethod*/
            {
            size_t wEscapeSize=0;
            while (true)
                {
                ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                UST_Status_type wSt=utfEscapeUtf32<utf32_t>(wOutPtr,
                                                           wInPtr,
                                                           wInCount,
                                                           wRemaining,
                                                           Context.Action,
                                                           wEscapeSize);
                if (wSt!=UST_TRUNCATED)
                       break;
                allocateUnitsBZero(wEscapeSize+1);     /* not enough space : allocate it */
                wOutPtr = Data + Context.OutUnitOffset;
                }
            Context.OutUnitOffset += wEscapeSize;
            Context.Escaped++;
            Context.OutUnitOffset +=wEscapeSize;
            wOutPtr+=wEscapeSize;
            wInPtr+= wInCount;
            Context.Count++;
            continue;
            }// Context.Action >= ZCNV_Escape_xx
/*  replacement is made within utf16Encode : wUtf16Char contains replacement char - wOutCount is set accordingly*/
         Context.Substit++;
         }

     ssize_t wCountReminder=getUnitCount()-Context.OutUnitOffset-wOutCount;
     if (wCountReminder < 0)
         {
         wCountReminder = -wCountReminder;
         wCountReminder++;
         allocateUnitsBZero(wCountReminder);
         wOutPtr = Data + Context.OutUnitOffset;
         }

     *wOutPtr=wCodePoint;
     wOutPtr++;
     Context.OutUnitOffset++;


     Context.Count ++; // One character has been processed

     wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

    }// while true
    addConditionalTermination();
//     *wUtf8Ptr=(utf8_t)'\0';
    _RETURN_ UST_SUCCESS;
}// utf32VaryingString::fromUtf32

utf8VaryingString*
utf32VaryingString::toUtf8(utf8VaryingString &pUtf8)
{
const utf32_t *wInPtr=Data;
utf32_t wUtf32Char;
size_t wInCount=0,wOutCount=0;
utf8_t wUtf8Char[5];

     pUtf8.clearData();
     Context.reset();
     Context.setStart(wInPtr);

     pUtf8.allocateUnitsBZero(UnitCount); // starting with current number of character units

     while (true)
         {
         Context.setPosition(wInPtr);
         Context.Status=utf32Decode(wInPtr,&wUtf32Char,&wInCount,nullptr);  // endian is supposed to be current system endian
         if (Context.Status==UST_ENDOFSTRING)
                                    break;
         if (Context.Status < 0)
             {
             Context.ErrFrom ++;

              if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                 {
                                 pUtf8.addConditionalTermination();// end of string mark
                                 return nullptr; // exit with nullptr return
                                 }
             wUtf32Char=cst_Unicode_Replct_utf32;  //in any other case, use replacement character
             Context.Substit++;
             }
         Context.CodePoint=wUtf32Char;
         Context.Status=utf8Encode(wUtf8Char,&wOutCount,wUtf32Char);
         if (Context.Status < 0)
             {
             Context.ErrTo ++;
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             pUtf8.addConditionalTermination();// end of string mark
                             return nullptr; // exit with nullptr return
                             }
             //in any other error cases, use replacement character
             for (wOutCount=0;cst_Unicode_Replct_utf8[wOutCount];wOutCount++)
                                             wUtf8Char[wOutCount]=cst_Unicode_Replct_utf8[wOutCount];
             Context.Substit++;
             }

         pUtf8.nadd(wUtf8Char,wOutCount);
         wInPtr += wInCount;
         }
    return &pUtf8;
} // utf32VaryingString::toUtf8

/**
* @brief utf32VaryingString::toUtf16 converts utf32 content of varying string to utf16 and sets content of pUtf16.
* Endianness is given by optional argument pEndian.
* If omitted, current system endianness is used.
* @param pUtf16
* @param pEndian
* @return
*/
utf16VaryingString *
utf32VaryingString::toUtf16(utf16VaryingString &pUtf16, ZBool *pEndian)
{
const utf32_t *wInPtr=Data;
utf32_t wUtf32Char;
size_t wInCount=0,wOutCount=0;
utf16_t wUtf16Char[3];

     Context.reset();
     Context.setStart(wInPtr);

     pUtf16.allocateUnitsBZero(UnitCount); // starting with current number of character units

     while (true)
         {
         Context.setPosition(wInPtr);
         Context.Status=utf32Decode(wInPtr,&wUtf32Char,&wInCount,nullptr);
         if (Context.Status==UST_ENDOFSTRING)
                                         break;
         if (Context.Status < 0)
             {
             Context.ErrFrom ++;

              if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                 {
                                 pUtf16.addConditionalTermination();// end of string mark
                                 return nullptr; // exit with nullptr return
                                 }
             wUtf32Char=cst_Unicode_Replct_utf32;  //in any other case, use replacement character
             Context.Substit++;
             }
         Context.CharUnits += wInCount;
         Context.CodePoint=wUtf32Char;
         Context.Status=utf16Encode((utf16_t*)wUtf16Char,&wOutCount,wUtf32Char, pEndian);
         if (Context.Status < 0)
             {
             Context.ErrTo ++;
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                             {
                             pUtf16.addConditionalTermination();// end of string mark
                             return nullptr; // exit with nullptr return
                             }
             //in any other error cases, use replacement character
             Context.Status=utf16Encode((utf16_t*)wUtf16Char,&wOutCount,cst_Unicode_Replct_utf16, pEndian);
             Context.Substit++;
             }

         pUtf16.nadd(wUtf16Char,wOutCount);
         wInPtr += wInCount;
         }
     return &pUtf16;
} // utf32VaryingString::toUtf16

/*utf32VaryingString&
utf32VaryingString::addBOM(ZBool *pEndian)
{
size_t wByteSize=0;
utf32_t* wString=addUtf32Bom((utf32_t*)Data,&wByteSize,pEndian);
setData(wString,wByteSize);
_free(wString);
return *this;
}*/


utf32VaryingString&
utf32VaryingString::getLittleEndian(utf32VaryingString& pLittleEndian)
{
 if (is_little_endian())
         {
         pLittleEndian=*this;
         return pLittleEndian;
         }

 pLittleEndian.allocateUnitsBZero(_Base::getUnitCount());
 utf32_t* wPtr=_Base::getContentPtr();
 utf32_t* wPtr1=pLittleEndian.getContentPtr();
 while (*wPtr)
         *wPtr1=forceReverseByteOrder<utf32_t>(*wPtr);
 *wPtr1=(utf32_t)'\0';
 return pLittleEndian;
}
utf32VaryingString&
utf32VaryingString::getLittleEndianWBOM(utf32VaryingString& pLittleEndian)
{
    pLittleEndian.setData(&cst_utf32BOMLE,sizeof(utf32_t));
    if (is_little_endian())
         {
         pLittleEndian.nadd(Data,getUnitCount());
         return pLittleEndian;
         }

 utf32_t* wPtr1=pLittleEndian.extendCharsBZero(_Base::getUnitCount()); // creates room for populating current string content
 utf32_t* wPtr=_Base::getContentPtr();
 while (*wPtr)
         *wPtr1++=forceReverseByteOrder<utf32_t>(*wPtr++);
 *wPtr1=(utf32_t)'\0';
 return pLittleEndian;
}

utf32VaryingString&
utf32VaryingString::getBigEndian(utf32VaryingString& pBigEndian)
{
    if (!is_little_endian())
            {
            pBigEndian=*this;
            return pBigEndian;
            }
    pBigEndian.allocateUnitsBZero(_Base::getUnitCount());
    utf32_t* wPtr=_Base::getContentPtr();
    utf32_t* wPtr1=pBigEndian.getContentPtr();
    while (*wPtr)
            *wPtr1++=forceReverseByteOrder<utf32_t>(*wPtr++);
    *wPtr1=(utf32_t)'\0';
    return pBigEndian;
}
utf32VaryingString&
utf32VaryingString::getBigEndianWBOM(utf32VaryingString& pBigEndian)
{
    pBigEndian.setData(&cst_utf32BOMBE,sizeof(utf32_t));

    if (!is_little_endian())
         {
         pBigEndian.nadd(Data,getUnitCount());
         return pBigEndian;
         }

 utf32_t* wPtr1=pBigEndian.extendCharsBZero(_Base::getUnitCount()); // creates room for populating current string content
 utf32_t* wPtr=_Base::getContentPtr();
 while (*wPtr)
         *wPtr1++=forceReverseByteOrder<utf32_t>(*wPtr++);
 *wPtr1=(utf32_t)'\0';
 return pBigEndian;
}

 //===============Varying strings========================================
#include <ztoolset/zwstrings.h>
#include <wchar.h>
#ifdef __COMMENT__

 #ifdef QT_CORE_LIB
 QString
  varyingCString::toQString()
  {
  return QString::fromUtf8(DataChar,Size);
  }
 varyingCString&
 varyingCString::fromQString(QString&pInQString)
 {
     size_t wSize=pInQString.size();
     allocate(wSize+1);
     memmove(DataChar,pInQString.toUtf8().data(),wSize);
     DataChar[wSize]='\0';
     return *this;
 }
#endif // QT_CORE_LIB
 /**
  * @brief varyingCString::toVaryingWString converts and replace actual content of varyingCString to a varyingWString given as argument.
  * @param pOutWString WString to receive converted characters
  * @return
  */
 varyingWString&
 varyingCString::toVaryingWString(varyingWString&pOutWString)
 {
 size_t wRSize =strlen(DataChar);
 size_t wSize ;
     mbstate_t wBuf;
     memset(&wBuf,0,sizeof(wBuf));
     pOutWString.allocate(wRSize+sizeof(wchar_t));


     wSize=mbsrtowcs(pOutWString.WDataChar,(const char**)&DataChar,wRSize,&wBuf);
     if (wSize!=wRSize)
        {
         fprintf(stderr,"%s>> Error : a problem occurred while converting CString to WString. Max char converted is %ld characters while %ld characters requested\n",
                 _GET_FUNCTION_NAME_,
                 wSize,
                 wRSize);
        }
     return pOutWString;
 }
 varyingCString&
 varyingCString::fromVaryingWString(varyingWString&pInWString)
 {
     mbstate_t wBuf;
     memset(&wBuf,0,sizeof(wBuf));

     size_t wRSize=(pInWString.Size*sizeof(wchar_t))-1;
     allocate(wRSize+1);
 //     int wSize= mbstowcs(content,pInCString.DataChar,_capacity);
      size_t wSize=wcsrtombs(DataChar,(const wchar_t**)&pInWString.WDataChar,wRSize,&wBuf);
      if (wSize!=wRSize)
         {
          fprintf(stderr,"%s>> Error : a problem occurred while converting WString to CString. Max char converted is %ld characters while %ld size requested\n",
                  _GET_FUNCTION_NAME_,
                  wSize,
                  wRSize);
         }
     return *this;
 }

 varyingCString&
 varyingCString::fromWString_Ptr(wchar_t* pInWString)
 {
     mbstate_t wBuf;
     memset(&wBuf,0,sizeof(wBuf));

     size_t wRSize=(wcslen(pInWString));
     allocate(wRSize+1);
 //     int wSize= mbstowcs(content,pInCString.DataChar,_capacity);
      size_t wSize=wcsrtombs(DataChar,(const wchar_t**)&pInWString,wRSize,&wBuf);
      if (wSize!=wRSize)
         {
          fprintf(stderr,"%s>> Error : a problem occurred while converting WString to CString. Max char converted is %ld characters while %ld characters requested\n",
                  _GET_FUNCTION_NAME_,
                  wSize,
                  wRSize);
         }
      DataChar[wSize]='\0';
     return *this;
 }

 /**
  * @brief varyingCString::fromWString_PtrCount Converts a WString on pCount characters to set current varyingCString content.
  * @param pInWString a wchar_t* string ended with \0 character
  * @param pCount  size in number of characters of the WString to be converted : without \0 ending character
  * @return
  */
 varyingCString&
 varyingCString::fromWString_PtrCount(wchar_t* pInWString, size_t pCount)
 {
     mbstate_t wBuf;
     memset(&wBuf,0,sizeof(wBuf));

     size_t wRSize=(pCount);
     allocate(wRSize+1);
      size_t wSize=wcsrtombs(DataChar,(const wchar_t**)&pInWString,wRSize,&wBuf);
      if (wSize!=(wRSize))
         {
          fprintf(stderr,"%s>> Error : a problem occurred while converting WString to CString. Max char converted is %ld characters while %ld characters requested\n",
                  _GET_FUNCTION_NAME_,
                  wSize,
                  wRSize);
         }
     DataChar[wSize]='\0';
     return *this;
 }

 ZDataBuffer *varyingCString::_exportURF(ZDataBuffer *pURF)
 {
 uint64_t wSize=(uint64_t)Size;
 ZTypeBase wType=ZType_VaryingCString;

    wType=_reverseByteOrder_T<ZTypeBase>(wType);
    pURF->setData(&wType,sizeof(ZTypeBase));

    wSize=_reverseByteOrder_T<size_t>(wSize);
    pURF->appendData(&wSize, sizeof(wSize));
    pURF->appendData(*this);
    return pURF;
 }
 /**
  * @brief varyingCString::_importURF
  * @param pUniversal a pointer to URF header data
  * @return  ZStatus
  */
 varyingCString&
 varyingCString::_importURF(unsigned char* &pURF)
 {
 ZTypeBase wType;
 //size_t wSize= pSize-sizeof(ZTypeBase)-sizeof(size_t);
 uint64_t wUniversalSize=0;
 size_t wOffset=0;


     memmove(&wType,pURF,sizeof(ZTypeBase));
     wType=_reverseByteOrder_T<ZTypeBase>(wType);
     wOffset+= sizeof(ZTypeBase);

     memmove(&wUniversalSize,(pURF+wOffset),sizeof(wUniversalSize));
     wUniversalSize=_reverseByteOrder_T<size_t>(wUniversalSize);
     wOffset+= sizeof(wUniversalSize);

     setData((pURF+wOffset),wUniversalSize);
/*     allocate(wUniversalSize);
     memmove(DataChar,,wUniversalSize);*/
     return *this;
 }// _importURF

 ZStatus
 varyingCString::getUniversalFromURF(unsigned char* pURFDataPtr,ZDataBuffer& pUniversal)
 {
 _MODULEINIT_
  uint64_t wEffectiveUSize ;
  ZTypeBase wType;
  unsigned char* wURFDataPtr = pURFDataPtr;

      memmove(&wType,wURFDataPtr,sizeof(ZTypeBase));
      wType=_reverseByteOrder_T<ZTypeBase>(wType);
      wURFDataPtr += sizeof (ZTypeBase);
      if (wType!=ZType_VaryingCString)
          {
          fprintf (stderr,
                   "%s>> Error invalid URF data type <%X> <%s> while expecting <%s> ",
                   _GET_FUNCTION_NAME_,
                   wType,
                   decode_ZType(wType),
                   decode_ZType(ZType_VaryingCString));
          _RETURN_ ZS_INVTYPE;
          }

      memmove (&wEffectiveUSize,wURFDataPtr,sizeof(uint64_t));        // first is URF byte size (including URF header size)
      wEffectiveUSize=_reverseByteOrder_T<uint64_t>(wEffectiveUSize);
      wURFDataPtr += sizeof (uint64_t);

      wEffectiveUSize = wEffectiveUSize - (uint32_t)(sizeof(ZTypeBase) + (sizeof(uint32_t)*2)); // compute net Universal size
      pUniversal.allocateBZero((wEffectiveUSize)); // fixed string must have canonical characters count allocated

     memmove(pUniversal.Data,wURFDataPtr,wEffectiveUSize);
     _RETURN_ ZS_SUCCESS;
 }//getUniversalFromURF


#endif // __COMMENT__


#endif // UTFVARYINGSTRING_CPP

