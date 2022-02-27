#ifndef ZUTFSTRINGS_CPP
#define ZUTFSTRINGS_CPP



#include <ztoolset/zutfstrings.h>
#include <ztoolset/zbasedatatypes.h>




/*  Docid is application dependant therefore defined within zapplicationtypes.h
utfcodeString&
utfcodeString::fromDocid(Docid_struct& pDocid)
{
    sprintf("%ld",pDocid.id);
    return *this;
}
 utfcodeString&
 utfcodeString::operator=(Docid_struct& pDocid)
 {
     return fromDocid(pDocid);
 }

*/

utfidentityString & utfidentityString::operator = (const utfdescString& pDesc ) {return (utfidentityString&) strset(pDesc.content); }



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
          return  ZS_INVTYPE;
          }

      memmove (&wEffectiveUSize,wURFDataPtr,sizeof(uint64_t));        // first is URF byte size (including URF header size)
      wEffectiveUSize=_reverseByteOrder_T<uint64_t>(wEffectiveUSize);
      wURFDataPtr += sizeof (uint64_t);

      wEffectiveUSize = wEffectiveUSize - (uint32_t)(sizeof(ZTypeBase) + (sizeof(uint32_t)*2)); // compute net Universal size
      pUniversal.allocateBZero((wEffectiveUSize)); // fixed string must have canonical characters count allocated

     memmove(pUniversal.Data,wURFDataPtr,wEffectiveUSize);
     return  ZS_SUCCESS;
 }//getUniversalFromURF


#endif // __COMMENT__



#endif // ZUTFSTRINGS_CPP

