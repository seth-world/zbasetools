#ifndef ZDATABUFFER_H
#define ZDATABUFFER_H


//#ifndef __ZDATABUFFER__
//#define __ZDATABUFFER__
#ifdef QT_CORE_LIB
#include <QByteArray>
#include <QString>
#endif

#include <errno.h>
#include <ztoolset/zerror.h>
#include <ztoolset/zarray.h>
#include <stdint.h>
#include <ztoolset/zcharset.h>
#include <type_traits>
/*
typedef uint8_t     utf8_t ;
typedef uint16_t    utf16_t ;
typedef uint32_t    utf32_t ;*/
typedef uint32_t    ucs4_t ;



#ifndef __ZCRYPTMETHOD_TYPE__
#define __ZCRYPTMETHOD_TYPE__
enum ZCryptMethod_type : uint8_t
{
    ZCM_Uncrypted       = 0,
    ZCM_Base64          = 1,
    ZCM_AES256          = 2,
    ZCM_SHA256          = 3,
    ZCM_MD5             = 5
};
#endif // __ZCRYPTMETHOD_TYPE__

/*
//#ifndef ZCHARSET_CPP
extern const char cst_default_delimiter_Char[] ;
extern const utf8_t cst_default_delimiter_U8[] ;
extern const utf16_t cst_default_delimiter_U16[] ;
extern const utf32_t cst_default_delimiter_U32[] ;
//#endif // ZCHARSET_CPP
*/
#include <ztoolset/zfunctions.h>
/*
#ifndef __DEFAULTDELIMITERSET__

#define __DEFAULTDELIMITERSET__  u8" \t\n\r"
#define __DEFAULTDELIMITERSET_U16__  u" \t\n\r"
#define __DEFAULTDELIMITERSET_U32__  L" \t\n\r"



const char* _firstNotinSet(const char*pString, const char *pSet=cst_default_delimiter_Char);
const char* _firstinSet (const char*pString,const char *pSet=cst_default_delimiter_Char);
const char* _lastinSet (const char*pString,const char *pSet=cst_default_delimiter_Char);
const char* _strchrReverse(const char *pStr,const char pChar);

char* _LTrim (char*pString, const char *pSet=cst_default_delimiter_Char);
char* _RTrim (char*pString, const char *pSet=cst_default_delimiter_Char);
char* _Trim (char*pString, const char *pSet=cst_default_delimiter_Char);

char* _toUpper(char *pStr,char *pOutStr=nullptr);
char* _expurgeSet(char *pString, const char *pSet=cst_default_delimiter_Char);
char* _expurgeString(char *pString, const char *pSubString);
int _strncasecmp(const char*pString1,const char*pString2,size_t pMaxLen);


#endif
*/
/**
 * @brief The ZDataBuffer class    Base class for buffering and processing data.

    @Note  as ZDB allocates/reallocates and frees dynamically memory, it should be PASSED AS REFERENCE to lower routines.
                Otherwise a double free/corrupted error will occur.

    @Note ZDataBuffer has some string management features. However it is not a string manager.
        It is up to user to take care about '\0' termination and its impact on size and memory allocation.
        Size is the exact content of ZDataBuffer and NOT the equivalent of strlen() value.
        However, it exist addTermination() method that allows to add a '\0' termination character whenever required.

    features :
        - data store
        - data concatenation

        - data encoding decoding

        - search and comparizon routines


 */
using namespace zbs;
#ifndef __URISTRING__
class uriString;
#endif
#ifndef CHECKSUM_H
struct checkSum;
#endif
#ifndef MD5_H
struct md5;
#endif
#ifndef ZCRYPT_H
namespace zbs{
class ZCryptKeyAES256;
class ZCryptVectorAES256;
}
#endif

class ZDataBuffer
{
 protected:
    ZDataBuffer& _copyFrom(const ZDataBuffer& pIn)
    {
       allocate (pIn.Size);
       memmove(Data,pIn.Data,pIn.Size);
       CryptMethod=pIn.CryptMethod;
       Size=pIn.Size;
       return *this;
    }
public:

 //   ZDataBuffer& operator= (const ZDataBuffer&) = delete;       // may assign because of overloaded operator =
    unsigned char * Data=nullptr;
    char*           DataChar=nullptr;
    void*           VoidPtr=nullptr;
    wchar_t*        WDataChar=nullptr;
public:

    size_t             Size=0;
    ZCryptMethod_type   CryptMethod=ZCM_Uncrypted;

    ZDataBuffer (void);
    ZDataBuffer (void *pData,ssize_t pSize) { allocate (pSize); memmove(Data,pData,(size_t)pSize);Size=(size_t)pSize;}
    ZDataBuffer (size_t pSize) {allocate((ssize_t)pSize);}

    ZDataBuffer (const char* pData)
    {
      setText(pData);
    }
    ZDataBuffer (const ZDataBuffer& pIn) {_copyFrom(pIn);}
    ZDataBuffer (const ZDataBuffer&& pIn) {_copyFrom(pIn);}
    ZDataBuffer& operator= (const ZDataBuffer& pIn) {_copyFrom(pIn); return *this;}
    ZDataBuffer& operator= (const ZDataBuffer&& pIn) {_copyFrom(pIn); return *this;}

    size_t getByteSize(void) {return Size;}

    ~ZDataBuffer ()
    { freeData();}

    unsigned char *getData(void) {return Data;}

    void clear (void) {freeData(); CryptMethod=ZCM_Uncrypted;}
    void reset (void)
    {
        memset(Data,0,Size);
        CryptMethod=ZCM_Uncrypted;
    }
    friend class ZDBiterator ;

    //
    // \brief setData
    //         loads ZDataBuffer with a content defined by a pointer to data to load (pData) and a size of data to load (pSize).
    //         ZDataBuffer storage is adjusted accordingly IF AVAILABLE MEMORY AMOUNT IS LESS than required.
    //         If amount of memory available is greater than required, no memory reallocation is done.
    //
    ZDataBuffer&
    setData(const void *pData,size_t pSize)
    {
        allocate (pSize);
        memmove(Data,pData,pSize);
        return(*this);
    }
    ZDataBuffer&
    setData(const unsigned char* &&pData,size_t pSize)
    {
        allocate (pSize);
        memmove(Data,pData,pSize);
        return(*this);
    }


    ZDataBuffer&
    setData(const ZDataBuffer& pBuffer)
    {
        CryptMethod=pBuffer.CryptMethod;
        allocate (pBuffer.Size);
        memmove(Data,pBuffer.Data,Size);

        return(*this);
    }
    ZDataBuffer&
    setInt(int pIn)
    {
      char wBuf[20];
      sprintf(wBuf,"%d",pIn);
      size_t wLen=0;
      while (wBuf[wLen])
          wLen++;

      setData(wBuf,wLen);
      return *this;
    }
    ZDataBuffer&
    setText(const char* pIn)
    {
      size_t wLen=0;
      while (pIn[wLen])
        wLen++;
      allocate(wLen);
      char* wPtr=DataChar;
      size_t wL=0;
      while (wL<wLen)
        *wPtr++=pIn[wL++];
      return *this;
    }

    template <class _Utf>
    ZDataBuffer&
    setUtf(const _Utf* pIn)
    {
      size_t wLen=0;
      while (pIn[wLen])
        wLen++;
      allocate(wLen*sizeof(_Utf));
      _Utf* wPtr=(_Utf*)DataChar;
      size_t wL=0;
      while (wL<wLen)
        *wPtr++=pIn[wL++];
      return *this;
    }

    ZDataBuffer&
    appendData(const void *pData,const size_t pSize) {
                                          size_t wOld=Size;
                                          extend(pSize);
                                          memmove(&Data[wOld],pData,pSize);
                                          return(*this);
                                         }
    ZDataBuffer&
    appendData(const ZDataBuffer& pBuffer) {
                                          size_t wOld=Size;
                                          extend(pBuffer.Size);
                                          memmove(&DataChar[wOld],pBuffer.DataChar,pBuffer.Size);
                                          return(*this);
                                           }


     ZDataBuffer&
     appendText(const char *pData)
      {
       size_t wOld=Size;
       size_t wLen=0;
       while (pData[wLen])
         wLen++;
       extend(wLen);
       size_t wL=0;
       while (wL<wLen)
         DataChar[wOld++]=pData[wL++];
//       memmove(&DataChar[wOld],pData,wLen);
       return(*this);
      }
      template <class _Utf>
      ZDataBuffer&
      appendUtf(const _Utf *pData)
      {
        size_t wOld=Size;
        size_t wLen=0;
        while (pData[wLen])
          wLen++;
        extend(wLen*sizeof(_Utf));
        _Utf* wPtr=&DataChar[wOld];
        size_t wL=0;
        while (wL<wLen)
          *wPtr++=pData[wL++];
//        memmove(&DataChar[wOld],pData,wLen);
        return(*this);
      }
    /** @brief truncate routines : truncate and Ctruncate reduce the size of ZDataBuffer to pLen (Ctruncate adds a '\0' termination)*/
    const ZDataBuffer& truncate(size_t pLen) {allocate(pLen); return *this;}
    const ZDataBuffer& Ctruncate(size_t pLen) {allocate(pLen);DataChar[pLen-1]='\0'; return *this;}
    /** @brief changeData replaces data content at pOffset with given data pointer on pLength. Extents storage if necessary. */
    const ZDataBuffer& changeData(void* pData,size_t pLength,size_t pOffset)
                {
                if (Size<(pOffset+pLength))
                        {
                        size_t wExt = (pOffset+pLength)-Size ;
                        extend(wExt);
                        }
                memmove(&Data[pOffset],pData,pLength);
                return(*this);
                }
    /** @brief changeData replaces data content at pOffset with given ZDataBuffer content. Extents storage if necessary*/
   ZDataBuffer& changeData(ZDataBuffer& pData,size_t pOffset)
                {
                if (Size<(pOffset+pData.Size))
                        {
                        size_t wExt = (pOffset+pData.Size)-Size ;
                        extend(wExt);
                        }
                memmove(&Data[pOffset],pData.Data,pData.Size);
                return(*this);
                }
    /** @brief eraseData erase data content at pOffset on pLength. Adjusts storage size. Abort if pOffset+pLength greater than actual data size with ENOMEM.*/
    ZDataBuffer& eraseData (size_t pOffset,size_t pLength)
            {
            if (Size<(pOffset+pLength))
                    {
                    errno =ENOMEM;
                    fprintf(stderr,"ZDataBuffer operation not allowed : exceeding storage boundary.\n%s\n",strerror(ENOMEM));
                    abort();
                    }
            memmove (Data+pOffset,Data+pOffset+pLength,Size-pOffset-pLength);
            allocate (Size-pLength);
            return *this;
            }

    bool isNull(void) {return (Data==nullptr);}
    bool isEmpty(void) {return(Size==0);}

    void freeData(void)
        {
        if (Data!=nullptr)
                        free(Data);
        Data=nullptr;
        DataChar=nullptr;
        VoidPtr=nullptr;
        Size=0;
         return;
        }


    char * toString(void) {return DataChar;}
//    ZDataBuffer* toUtf16(ZDataBuffer& pOutBuf);
//    wchar_t* toWString(void) {return WDataChar;}
    ZDataBuffer& fromString(const char* pString) { allocate(strlen(pString)+1); strncpy(DataChar,pString,strlen(pString)); return *this; }
    ZDataBuffer&fromWString(const wchar_t* pWString) { allocate((wcslen(pWString)+1)*sizeof(WDataChar[0])); wcsncpy(WDataChar,pWString,wcslen(pWString)); return *this; }
    /*
 *   ======ZDataBuffer has NO export/import facilities==============
 *
    ZDataBuffer *_exportURF(ZDataBuffer *pUniversal);
    ZDataBuffer &_importURF(unsigned char *pUniversal);
*/

    template<class _Tp>
    _Tp moveTo() const
    {
        _Tp wOut;

        if (sizeof(_Tp) != Size)
            {
            fprintf(stderr,
                "ZDataBuffer::moveTo-F-INVTYP Invalid data size <%ld> for type size <%ld>\n",
                Size,
                sizeof(_Tp));
            abort();
            }
        memmove(&wOut, Data, sizeof(_Tp));

        return wOut;
    }
    template<class _Tp>
    _Tp* moveToPtr() { return static_cast<_Tp*>(Data); }

/**
 * @brief moveOut <_Tp> loads the raw content of the ZDataBuffer to a template defined data structure pOutData (Compound) given as _Tp
 * @param pOutData
 * @return
 */
/*
template <typename _Tp>
_Tp& moveOut(typename std::enable_if<!(std::is_pointer<_Tp>::value||std::is_array<_Tp>::value),_Tp> &pOutData,ssize_t pOffset=0)
{
    ssize_t wSize;
    if (sizeof(_Tp) >= Size )
                wSize=Size;
        else
                wSize=sizeof(_Tp);

    memmove(&pOutData,Data+pOffset,wSize);
    return pOutData;
}
*/
template <typename _Tp>
_Tp& moveOut(typename std::enable_if<std::is_integral<_Tp>::value,_Tp> &pOutData,ssize_t pOffset=0)
{
  ssize_t wSize;
  if (sizeof(_Tp) >= Size )
    wSize=Size;
  else
    wSize=sizeof(_Tp);

  memmove(&pOutData,Data+pOffset,wSize);
  return pOutData;
}
template <typename _Tp>
_Tp& moveOut(typename std::enable_if<std::is_array<_Tp>::value,_Tp> &pOutData,ssize_t pOffset=0)
{
    ssize_t wSize;
    if (sizeof(_Tp) >= Size )
                wSize=Size;
        else
                wSize=sizeof(_Tp);

    memmove(pOutData,Data+pOffset,wSize);
    return pOutData;
}
template <typename _Tp>
_Tp& moveOut(typename std::enable_if<std::is_pointer<_Tp>::value,_Tp> &pOutData,ssize_t pOffset=0)
{
    ssize_t wSize;
    if (sizeof(_Tp) >= Size )
                wSize=Size;
        else
                wSize=sizeof(_Tp);

    memmove(pOutData,Data+pOffset,wSize);
    return pOutData;
}

    template <typename _Tp>
    /**
     * @brief move <_Tp> loads the content of  template defined data structure pInData (Compound) given as _Tp  to raw content of the ZDataBuffer
     * @param pInData
     * @return
     */
    ZDataBuffer& moveIn(_Tp &pInData)
    {
        ssize_t wSize=sizeof(_Tp) ;
        allocate(sizeof(_Tp));
        memmove(Data,&pInData,wSize);
        return *this;
    }



    ZDataBuffer& clearData(void) {if (Data==nullptr) return(*this); memset(Data,0,Size); return (*this); }

    ZDataBuffer& addTermination(void) {allocate (Size+1); Data[Size]=0; return *this;}
    ZDataBuffer& addConditionalTermination(void)
        {
        if (DataChar[Size-1]==0)
                            return *this;
        allocate (Size+1);
        Data[Size]='\0';
        return *this;
        }
    ZDataBuffer& utfAddConditionalTermination(void)
        {
        if (DataChar[Size-1]==0)
                            return *this;
        allocate (Size+1);
        Data[Size]='\0';
        return *this;
        }
    ZDataBuffer& toUpper(void)
                    {
        for (size_t wi=0;wi<Size;wi++)
                if ((DataChar[wi]>=cst_lowercase_begin)&&(DataChar[wi]<=cst_lowercase_end))
                                DataChar[wi] -= cst_upperization ;
        return *this;}

    ZDataBuffer& toLower(void)
                    {
        for (size_t wi=0;wi<Size;wi++)
                if ((DataChar[wi]>=cst_uppercase_begin)&&(DataChar[wi]<=cst_uppercase_end))
                                DataChar[wi] += cst_upperization ;
        return *this;}

    /** @brief allocate()  Allocates pSize bytes to storage space. If ZDataBuffer contains data : existing data will NOT be lost*/
    unsigned char *allocate(ssize_t pSize);
    /** @brief allocate()  Allocates pSize bytes to storage space and set all space to binary zero- Existing data WILL BE lost*/
    unsigned char *allocateBZero(ssize_t pSize);
    unsigned char *extend(ssize_t pSize);
    unsigned char* extendBZero(ssize_t pSize);

    template <class _Tp>
    _Tp* utfAllocate_T (ssize_t pCount)
    {
        return (_Tp*) allocate((ssize_t)(pCount*sizeof(_Tp)));
    }
    template <class _Tp>
    _Tp* utfAllocateBZero_T (ssize_t pCount)
    {
        return (_Tp*) allocateBZero((ssize_t)(pCount*sizeof(_Tp)));
    }
    template <class _Tp>
    _Tp* utfExtend_T (ssize_t pCount)
    {
        return (_Tp*) extend(pCount*sizeof(_Tp));
    }
    template <class _Utf>
    _Utf* utfExtendBZero_T (ssize_t pCount)
    {
        return (_Utf*) extendBZero((ssize_t)(pCount*sizeof(_Utf)));
    }

    template <class _Utf>
    ZDataBuffer& utfAddConditionalTermination(void)
    {
        _Utf* wUtfData=(_Utf*)Data;
        size_t wCount = Size / sizeof(_Utf);
        if (wUtfData[wCount]==0)
                        return *this;
        utfExtendBZero_T<_Utf>(1);
        return *this;
    }

#ifdef QT_CORE_LIB
   QByteArray toQByteArray(void) { return (QByteArray ((char *)Data,Size)); }
   QString toQString(void)      {return QString(toQByteArray());}
#endif // QT_CORE_LIB
   ssize_t bsearch (void *pKey,const size_t pKeySize,const size_t pOffset=0);
   ssize_t bsearch(ZDataBuffer &pKey,const size_t pOffset=0);

   ssize_t bsearchCaseRegardless (void *pKey, ssize_t pSize, ssize_t pOffset);
   ssize_t bsearchCaseRegardless (ZDataBuffer &pKey, ssize_t pOffset);

   ssize_t bstartwithCaseRegardless (void *pKey,ssize_t pSize,ssize_t pOffset);
   ssize_t bstartwithCaseRegardless (ZDataBuffer pKey,size_t pOffset);

   ssize_t breverseSearch(void *pKey, ssize_t pKeySize, ssize_t pOffset=0);
   ssize_t breverseSearch(ZDataBuffer &pKey,const ssize_t pOffset=0);

   ssize_t breverseSearchCaseRegardless(void *pKey, ssize_t pKeySize, ssize_t pOffset=0);
   ssize_t breverseSearchCaseRegardless(ZDataBuffer &pKey,const ssize_t pOffset=0);

   int compare(const void *pCompare,long pSize=-1)
            {
            ssize_t wsize=(pSize<0)?Size:pSize;
            if (wsize > (ssize_t)Size)
                 wsize=Size;
            return(memcmp(Data,pCompare,wsize));
            }
   bool isEqual (const void *pCompare,long pSize=-1) ;
   bool isGreater (const void *pCompare,long pSize=-1);
   bool isLess (const void *pCompare,long pSize=-1);

   bool contains (const char *pString)
          {
            return !(strstr(DataChar,pString)==nullptr);
          }



   unsigned char & operator [] (long pIdx) {return Data[pIdx];}

   const ZDataBuffer & operator = (const char *pCharIn)
                                { allocate(strlen(pCharIn)+1);
                                strcpy(DataChar,pCharIn);
                                return(*this);}

// see with CTORs   const ZDataBuffer & operator = (const ZDataBuffer &pDataBuffer)


   const ZDataBuffer & operator = (const long pValue)
                               {  setData (&pValue,sizeof(pValue)); return *this;}
   const ZDataBuffer & operator = (const size_t pValue)
                               {  setData (&pValue,sizeof(pValue)); return *this;}

   const ZDataBuffer & operator = (const float pValue)
                               {  setData (&pValue,sizeof(pValue)); return *this;}
   const ZDataBuffer & operator = (const double pValue)
   {
       setData(&pValue, sizeof(pValue));
       return *this;
   }

   ZStatus fomFile(uriString &pURI);
   ZStatus operator << (uriString &pURI);

#ifdef QT_CORE_LIB

   const ZDataBuffer & fromQByteArray(const QByteArray pQByteArray)
                       { allocate(pQByteArray.size());
                       memmove(Data,pQByteArray.data(),Size);
                       return(*this);}
    const ZDataBuffer & operator = (const QByteArray pQByteArray)
                                { allocate(pQByteArray.size());
                                memmove(Data,pQByteArray.data(),Size);
                                return(*this);}

    const ZDataBuffer & operator = (const QString pQString)
                                { return(fromQByteArray(pQString.toUtf8()));}
#endif
    const ZDataBuffer & operator += (const char* pString)
                                 { appendData(pString,strlen(pString)+1); return *this;}
    const ZDataBuffer & operator += (const ZDataBuffer &pDataBuffer)
                                 { return appendData(pDataBuffer.Data,pDataBuffer.Size);}
    const ZDataBuffer & operator += (const long pValue)
                                { return appendData (&pValue,sizeof(pValue));}
    const ZDataBuffer & operator += (const size_t pValue)
                                { return appendData (&pValue,sizeof(pValue));}
    const ZDataBuffer & operator += (const float pValue)
                                { return appendData (&pValue,sizeof(pValue));}
    const ZDataBuffer & operator += (const double pValue)
                                { return appendData (&pValue,sizeof(pValue));}
//-------- crypting checksum md5-----------------------------------

    ZStatus encryptB64(ZDataBuffer *pZDB=nullptr); /* Base64 encrypt current ZDataBuffer object content. If pZDB is omitted then current object's content is replaced by B64 encoded data*/
    ZStatus encryptB64( void *pString, size_t pSize ) {setData(pString,pSize); return(encryptB64());} // loads then encode the content of ZDataBuffer to Base64

    ZStatus uncryptB64( ZDataBuffer *pZDB=nullptr ); /* decrypt the content of current ZDataBuffer from Base64. If pZDB is omitted then current object's content is replaced by B64 encoded data*/
    ZStatus uncryptB64( void *pString, size_t pSize ) {setData(pString,pSize); return(uncryptB64());}// loads then decode the content of ZDataBuffer from Base64

    md5 getmd5( void );
//    md5 getmd5( unsigned char* pData, ssize_t pSize );

    checkSum getcheckSum( void ); /** gets a checksum object from current object full content */
//    checkSum getcheckSum( unsigned char* pData, ssize_t pSize );

    checkSum* newcheckSum( void ); /** creates a checkSum object using new -Warning: to be further deleted */
//    checkSum* generatecheckSum( unsigned char* pData, ssize_t pSize )  ;

    /** @brief ZDataBuffer::encryptAES256() encrypts current object's content to AES256 according mandatory given Key and Vector
     * @param [in] pKey     zbs::ZCryptKeyAES256 containing crypting key
     * @oaram [in] pVector  zbs::ZCryptKeyAES256 containing crypting vector
     * @param [out-optional] pZDB receives the encrypted data. if omitted (nullptr) current object's data is replaced
     */
    ZStatus encryptAES256(const ZCryptKeyAES256 &pKey,
                          const ZCryptVectorAES256 &pVector,
                          ZDataBuffer* pZDB=nullptr);

    /** @brief encryptAES256toFile() write to file encrypted current object's content to AES256 according given Key and Vector */
    ZStatus encryptAES256toFile(const char*pFileName, /** same as previous but puts the encrypted data within pFileName file */
                                const ZCryptKeyAES256& pKey,
                                const ZCryptVectorAES256& pVector);

    ZStatus uncryptAES256(const ZCryptKeyAES256 &pKey, /** uncrypts current object's content to AES256 according mandatory given Key and Vector */
                          const ZCryptVectorAES256 &pVector,
                          ZDataBuffer* pZDB=nullptr); /** pZDB receives the plain text data. if omitted current object's data is replaced*/

    ZStatus uncryptAES256FromFile(const char*pFileName, /** same as previous but load encrypted data from pFileName file */
                                ZCryptKeyAES256 pKey,
                                ZCryptVectorAES256 pVector);

//-------- End crypting checksum md5-----------------------------------

    void move(void* pDest)                  {memmove (pDest,Data,Size);}
    /**
     * @brief moven moves current ZDataBuffer content to pDest on ZDataBuffer Size length with a maximum given by pSize
     * @param pDest
     * @param pSize
     */
    void moven(void* pDest,ssize_t pSize)    {ssize_t wS=pSize > (ssize_t)Size? Size:pSize; memmove(pDest,Data,wS);}


    inline const unsigned char* firstNotinSet(const char *pSet=cst_default_delimiter_Char)
            {return((const unsigned char*)_firstNotinSet((const char*)DataChar,pSet));}

    char* LTrim(const char *pSet=cst_default_delimiter_Char)
            {return(_LTrim(DataChar,pSet));}

    char* RTrim(const char *pSet=cst_default_delimiter_Char)
            {return(_RTrim(DataChar,pSet));}

    char* Trim(const char *pSet=cst_default_delimiter_Char)
            {return(_Trim(DataChar,pSet));}

    ZDataBuffer& setChar(const char pChar, size_t pStart=0, long pSize=-1) ;
    // equivalent of setData but for a C string
    ZDataBuffer& setString(const char* pString) ;
    // equivalent of appendData but for a C string
    ZDataBuffer& appendString(const char* pString) ;

    ZDataBuffer& setUtf8(const utf8_t* pString);
    ZDataBuffer& setUtf16(const utf16_t* pString);
    ZDataBuffer& setUtf32(const utf32_t* pString);
    ZDataBuffer& setUcs4(const ucs4_t* pString);

    void dumpHexa (const size_t pOffset, const long pSize, ZDataBuffer &pLineHexa, ZDataBuffer &pLineChar);
    void dumpHexa_1 (const size_t pOffset, const long pSize, ZDataBuffer &pLineHexa, ZDataBuffer &pLineChar);

    friend int rulerSetup (ZDataBuffer &pRulerHexa, ZDataBuffer &pRulerAscii,int pColumn);

    void Dump_old(const int pColumn=16,FILE* pOutput=stderr);
    void Dump(const int pColumn=16,ssize_t pLimit=-1,FILE* pOutput=stderr);
    void Dump(const char*pFilePath,const int pColumn=16,ssize_t pLimit=-1);

}; // ZDataBuffer


int rulerSetup (ZDataBuffer &pRulerHexa, ZDataBuffer &pRulerAscii,int pColumn);

template <typename _Tp>
zbs::ZArray<_Tp>& ZDataBufferToZArray (ZDataBuffer &pDataBuffer,zbs::ZArray<_Tp> &pZArray);

class ZBlob :public ZDataBuffer
{
public:
    ZBlob(){}
    ZBlob (const ZBlob&) = delete;                  // cannot copy
    ZDataBuffer *_exportURF(ZDataBuffer *pUniversal);
    ZStatus _importURF(unsigned char* pUniversal);
    static ZStatus getUniversalFromURF(unsigned char* pURFDataPtr,ZDataBuffer& pUniversal);

};//ZBlob



#endif // ZDATABUFFER_H
