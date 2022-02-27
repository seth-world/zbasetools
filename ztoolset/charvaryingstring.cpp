#ifndef CHARVARYINGSTRING_CPP
#define CHARVARYINGSTRING_CPP

#include <ztoolset/charvaryingstring.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zcharset.h>

 //================ charVaryingString conversions to and from other formats==============================

charVaryingString*
charVaryingString::fromUtf8(const utf8_t* pUtf8, const ssize_t pByteSize)
{


    if (_Base::getCharset()==ZCHARSET_UTF8) // then straight forward
        {
        ssize_t wByteSize=pByteSize;
        const utf8_t* wPtrIn=pUtf8;
        if (pByteSize<0)
            {
            wByteSize=0;
            while (*wPtrIn)
                    wByteSize++;
            wByteSize++;
            wPtrIn=pUtf8;
            }
        char* wPtr=allocateBytes(wByteSize);
        wByteSize--; // skip last for endofstring mark room
        while (wByteSize--)
                *wPtr++=(char)*wPtrIn++;
        *wPtr=0;
        return this;
        }
    Context.reset();

    char *wString=nullptr;
    int wSubstitutions=0;

    int wSt=utf8StrToCharStr(&wString,_Base::getCharset(), pUtf8,pByteSize,WarningSignal);
    if (wSt<0)
    {
        fprintf (stderr,"%s>> Error while converting utf8 string to char string. Error <%d> <%s>\n",
                 _GET_FUNCTION_NAME_,
                 wSt,
                 decode_UST(wSt));
        zfree(wString);
        return this;
    }
    if (wSt!=UST_SUCCESS)
        {
        fprintf (stderr,"%s>> Some substitutions occurred while converting utf8 string to char string. Number <%d>\n",
                 _GET_FUNCTION_NAME_,
                 wSubstitutions);
        }
    strset(wString);
    zfree(wString);
    return this;
}//fromUtf8

charVaryingString*
charVaryingString::fromUtf16(const utf16_t* pInString, const ssize_t pByteSize, ZBool *plittleEndian)
{
    char *wString=nullptr;
    int wSubstitutions=0;

    int wSt=utf16StrToCharStr(&wString,_Base::getCharset(), pInString,pByteSize,WarningSignal, plittleEndian);
    if (wSt<UST_SEVERE)
    {
        fprintf (stderr,"%s>> Error while converting utf16 string to char string. Error <%d> <%s>\n",
                 _GET_FUNCTION_NAME_,
                 wSt,
                 decode_UST(wSt));
        zfree(wString);
        return nullptr;
    }
    if (wSt!=UST_SUCCESS)
        {
        fprintf (stderr,"%s>> Some substitutions occurred while converting utf16 string to char string. Number <%d>\n",
                 _GET_FUNCTION_NAME_,
                 wSubstitutions);
        }
    strset(wString);
    zfree(wString);
    return this;
}//fromUtf16

charVaryingString*
charVaryingString::fromUtf32(const utf32_t* pInString, const ssize_t pByteSize, ZBool *plittleEndian)
{
    setEmpty();
//    utfStrCtx<utf32_t> wContext;
    Context.reset();
    char *wString=nullptr;

    UST_Status_type wSt=utf32StrToCharStr(&wString,
                                          _Base::getCharset(),
                                          pInString,
                                          pByteSize,
                                          WarningSignal,
                                          plittleEndian);
    if (wSt<UST_SEVERE)
    {
        zfree(wString);
        return nullptr;
    }

    strset(wString);
    zfree(wString);
    return this;
}//fromUtf32
/**
  * @brief charVaryingString::toUtf8 sets utf8 varying string content (pUtf8) to current string, moving char characters to utf8 as it is,
  * after having allocated character units to make string fit.
  *
  * Nota bene: utf8 is not subject to endianness.
  *
  * @param[out] pUtf8 resulting utf8 varying string
  * @return a reference to utf8VaryingString object.
  */
 utf8VaryingString&
 charVaryingString::toUtf8(utf8VaryingString &pUtf8)
 {
    Context.reset();
    Context.setStart(Data);
     char *wPtr=Data;
     pUtf8.allocateUnitsBZero(UnitCount);
     utf8_t* wPtr1=pUtf8.Data;

     while (*wPtr)
        {
        *wPtr1++=(utf8_t)*wPtr++;
         Context.CharUnits++;
         Context.Count++;
         }
     return pUtf8;
 }//toUtf8
/**
  * @brief charVaryingString::toUtf16 sets utf16 varying string content (pUtf16) to current string,
  * moving char characters to utf16 as it is (after having processed endianness if required),
  * after having allocated utf16 character units to make string fit.
  *
  * @param[out] pUtf16 resulting utf16 varying string
  * @param[in] pLittleEndian: Optional parameter : a pointer to a boolean-
  *      - nullptr : endianness is current system endianness (see function is_little_endian() ).
  *      - points to true  : input utf32 string is little endian
  *      - points to false : input utf32 string is big endian
  * @return a reference to utf16VaryingString object.
  */
 utf16VaryingString&
 charVaryingString::toUtf16(utf16VaryingString &pUtf16, bool *plittleEndian)
 {
    Context.reset();
    Context.setStart(Data);
    bool wEndianProcess=false;
    if (plittleEndian)
         {
         Context.EndianRequest=true;
         Context.LittleEndian=*plittleEndian;
         if (*plittleEndian!=is_little_endian())
                                     wEndianProcess=true;
         }
     char *wPtr=Data;
     pUtf16.allocateUnitsBZero(UnitCount);
     utf16_t* wPtr1=pUtf16.Data;

     if(wEndianProcess)
         while (*wPtr)
                {
                *wPtr1++=forceReverseByteOrder<utf16_t>((utf16_t)*wPtr++);
                Context.CharUnits++;
                Context.Count++;
                }
         else
             while (*wPtr)
                {
                *wPtr1++=(utf16_t)*wPtr++;
                 Context.CharUnits++;
                 Context.Count++;
                 }

     return pUtf16;
 }//toUtf16
/**
  * @brief charVaryingString::toUtf32 sets utf32 varying string content (pUtf32) to current string,
  *  moving char characters to utf32 as it is (after having processed endianness if required),
  * after having allocated utf32 character units to make string fit.
  *
  * @param[out] pUtf32 resulting utf32 varying string
  * @param[in] pLittleEndian: Optional parameter : a pointer to a boolean-
  *      - nullptr : endianness is current system endianness (see function is_little_endian() ).
  *      - points to true  : input utf32 string is little endian
  *      - points to false : input utf32 string is big endian
   *
   * @return a reference to utf32VaryingString object.
   */
utf32VaryingString&
charVaryingString::toUtf32(utf32VaryingString &pUtf32,bool *plittleEndian)
{
    Context.reset();
    Context.setStart(Data);

    bool wEndianProcess=false;
    if (plittleEndian)
        {
        Context.EndianRequest=true;
        Context.LittleEndian=*plittleEndian;
        if (*plittleEndian!=is_little_endian())
                                    wEndianProcess=true;
        }
     char *wPtr=Data;
     pUtf32.allocateUnitsBZero(UnitCount);
     utf32_t* wPtr1=pUtf32.Data;

     if(wEndianProcess)
         while (*wPtr)
                {
                *wPtr1++=forceReverseByteOrder<utf32_t>((utf32_t)*wPtr++);
                Context.CharUnits++;
                Context.Count++;
                }
         else
             while (*wPtr)
                {
                *wPtr1++=(utf32_t)*wPtr++;
                 Context.CharUnits++;
                 Context.Count++;
                 }

     return pUtf32;
 }//toUtf32





 //===============Varying strings========================================
//#include <ztoolset/zwstrings.h>
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
          return ZS_INVTYPE;
          }

      memmove (&wEffectiveUSize,wURFDataPtr,sizeof(uint64_t));        // first is URF byte size (including URF header size)
      wEffectiveUSize=_reverseByteOrder_T<uint64_t>(wEffectiveUSize);
      wURFDataPtr += sizeof (uint64_t);

      wEffectiveUSize = wEffectiveUSize - (uint32_t)(sizeof(ZTypeBase) + (sizeof(uint32_t)*2)); // compute net Universal size
      pUniversal.allocateBZero((wEffectiveUSize)); // fixed string must have canonical characters count allocated

     memmove(pUniversal.Data,wURFDataPtr,wEffectiveUSize);
     return ZS_SUCCESS;
 }//getUniversalFromURF


#endif // __COMMENT__



#endif // CHARVARYINGSTRING_CPP

