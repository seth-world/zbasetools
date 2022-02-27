#ifndef UTFVTEMPLATESTRING_H
#define UTFVTEMPLATESTRING_H

#include <zconfig.h>

#ifdef QT_CORE_LIB
#include <QByteArray>
#include <QString>
#endif

#include <errno.h>
#include <stdint.h>
#include <ztoolset/zerror.h>
#include <ztoolset/zarray.h>
#include <ztoolset/zcharset.h>
#include <ztoolset/utfsprintf.h>
#include <ztoolset/utfutils.h>


#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include <zcrypt/checksum.h>
#include <zcrypt/md5.h>


size_t UVScalcDecodeLengthB64(const void *b64input, size_t pSize);


template <class _Utf>
class utfVaryingString;



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

#ifndef __DEFAULTDELIMITERSET__
#define __DEFAULTDELIMITERSET__  " \t\n\r"

const char* _firstNotinSet(const char*pString, const char *pSet=__DEFAULTDELIMITERSET__);
const char* _firstinSet (const char*pString,const char *pSet=__DEFAULTDELIMITERSET__);
const char* _lastinSet (const char*pString,const char *pSet=__DEFAULTDELIMITERSET__);
const char* _strchrReverse(const char *pStr,const char pChar);

char* _LTrim (char*pString, const char *pSet=__DEFAULTDELIMITERSET__);
char* _RTrim (char*pString, const char *pSet=__DEFAULTDELIMITERSET__);
char* _Trim (char*pString, const char *pSet=__DEFAULTDELIMITERSET__);

char* _toUpper(char *pStr,char *pOutStr=nullptr);
char* _expurgeSet(char *pString, const char *pSet=__DEFAULTDELIMITERSET__);
char* _expurgeString(char *pString, const char *pSubString);
int _strncasecmp(const char*pString1,const char*pString2,size_t pMaxLen);
#endif

/**
 * @brief The utfVaryingString class    Base class for buffering and processing data.

    @Note  as utfVaryingString allocates/reallocates and frees dynamically memory, it should be PASSED AS REFERENCE to lower routines.
                Otherwise a double free/corrupted error will occur.

    @Note utfVaryingString has some string management features. However it is not a string manager.
        It is up to user to take care about '\0' termination and its impact on size and memory allocation.
        Size is the exact content of utfVaryingString and NOT the equivalent of strlen() value.
        However, it exist addTermination() method that allows to add a '\0' termination character whenever required.

    features :
        - data store
        - data concatenation

        - data encoding decoding

        - search and comparizon routines


 */
using namespace zbs;
class uriString;
struct checkSum;

#include <ztoolset/ztypetype.h>
#include <ztoolset/utfstringcommon.h>

class utfKey;

template <class _Utf>
class utfVaryingString : public utfStringDescriptor
{
protected:
    utfVaryingString& _copyFrom(const utfVaryingString& pIn)
    {
      utfStringDescriptor::_copyFrom(pIn);
//      CryptMethod=pIn.CryptMethod;
      allocateBytes( pIn.ByteSize);
      memmove(Data,pIn.Data,ByteSize);
      CheckData();
      return(*this);
    }

public:
    utfVaryingString (const utfVaryingString& pIn);
    utfVaryingString (const utfVaryingString&& pIn);


    _Utf *          Data=nullptr;
    utfVaryingString (void) ;
    utfVaryingString (_Utf *pData,ssize_t pCount);
    utfVaryingString (size_t pCount);
    utfVaryingString (const utfStringDescriptor& pIn);
    void utfInit(void);
    ~utfVaryingString () ;

    _Utf*           getContentPtr(void) {return Data;}
    void clear (void) {freeData(); CryptMethod=ZCM_Uncrypted;}
    void reset (void)
    {
      if (Data)
      {
        memset(Data,0,ByteSize);
        CryptMethod=ZCM_Uncrypted;
      }
      CheckData();
    }

    void setEmpty(void)
      {
      if (Data!=nullptr)
        memset(Data,0,ByteSize);CryptMethod=ZCM_Uncrypted;
      CheckData();
    }

    friend class ZDBiterator ;
    //
    // \brief setData
    //         loads utfVaryingString with a content defined by a pointer to data to load (pData) and a size of data to load (pSize).
    //         utfVaryingString storage is adjusted accordingly IF AVAILABLE MEMORY AMOUNT IS LESS than required.
    //         If amount of memory available is greater than required, no memory reallocation is done.
    //
    utfVaryingString&
    setData(const _Utf* &pData,size_t pSize)
    {
        CryptMethod=ZCM_Uncrypted;
        allocateBytes (pSize);
        memmove(Data,pData,pSize);
        CheckData();
        return(*this);
    }
    utfVaryingString&
    setData(const _Utf* &&pData,size_t pSize)
    {
        CryptMethod=ZCM_Uncrypted;
        allocateBytes (pSize);
        memmove(Data,pData,pSize);
        CheckData();
        return(*this);
    }
    inline void CheckData()
    {
      if (Check!=0xFFFFFFFF)
      {
        fprintf(stderr,"utfVaryingString::CheckData-F-OUTBOUND memory error.\n");
        abort();
      }
    }
    utfVaryingString&
    setData(const utfVaryingString& pBuffer)
    {
        CryptMethod=pBuffer.CryptMethod;
        allocateBytes( pBuffer.ByteSize);
        memmove(Data,pBuffer.Data,ByteSize);

        return(*this);
    }
    utfVaryingString&
    setInt(const int pIn)
    {
      char wBuffer[20];
      int wL=::sprintf(wBuffer,"%d",pIn);
      allocateUnitsBZero(wL+1);
      _Utf* wPtr=Data;
      size_t wi=0;
      while (wL--)
        *wPtr++=wBuffer[wi++];
      return *this;
    }
    utfVaryingString&
    setLong(const long pIn)
    {
      char wBuffer[20];
      int wL=::sprintf(wBuffer,"%ld",pIn);
      allocateUnitsBZero(wL+1);
      _Utf* wPtr=Data;
      size_t wi=0;
      while (wL--)
        *wPtr++=wBuffer[wi++];
      return *this;
    }
    utfVaryingString&
    setFloat(const double pIn)
    {
      char wBuffer[30];
      int wL=::sprintf(wBuffer,"%g",pIn);
      allocateUnitsBZero(wL+1);
      _Utf* wPtr=Data;
      size_t wi=0;
      while (wL--)
        *wPtr++=wBuffer[wi++];
      return *this;
    }
    utfVaryingString&
    appendData(const void *pData,const size_t pSize) {
                                          size_t wOld=ByteSize;
                                          extendBytes(pSize);
                                          memmove(&Data[wOld],pData,pSize);
                                          return(*this);
                                         }
    utfVaryingString&
    appendData(utfVaryingString& pBuffer)
        {
        if (pBuffer.isEmpty())
                return *this;
        size_t wOld=ByteSize;
        extendBytes(pBuffer.ByteSize);
        memmove(&DataByte[wOld],pBuffer.Data,pBuffer.ByteSize);
        return(*this);
        }
    /** @brief truncate routines : truncate and Ctruncate reduce the size of utfVaryingString to pLen (Ctruncate adds a '\0' termination)*/
    utfVaryingString<_Utf>& truncate(ssize_t pCount); /** truncates string to pLen character units from beginning of _Utf string */

    /** @brief changeData replaces data content at pOffset with given data pointer pData on pCount.
     * Extents storage if necessary. */
    utfVaryingString& changeData(_Utf* pData,size_t pCount,size_t pOffset)
                {
                if (UnitCount<(pOffset+pCount))
                        {
                        size_t wExt = 1+(pOffset+pCount)-UnitCount ;
                        extendUnitsBZero(wExt);
                        }
                _Utf* wPtr= Data+pOffset;
                _Utf* wPtrEnd=&Data[UnitCount];
                for (size_t wi =0;(wi < pCount)&&(wPtr < wPtrEnd);wi++) /* control on wPtrEnd is not necessary */
                                        *wPtr++=*pData++;
                return(*this);
                }
    /** @brief changeData replaces data content at pOffset with given utfVaryingString content.
     * Extents storage if necessary*/
   utfVaryingString& changeData(const utfVaryingString<_Utf>& pData,size_t pOffset)
          {
          if (UnitCount<(pOffset+pData.UnitCount))
                  {
                  size_t wExt = 1 + (pOffset+pData.UnitCount)-UnitCount ;
                  extendUnitsBZero(wExt);
                  }
          _Utf* wPtr= Data+pOffset;
          _Utf* wSrcPtr=pData.Data;

          for (size_t wi =0;wi < pData.UnitCount;wi++)
                                  *wPtr++=wSrcPtr++;
          return(*this);
          }

   /** @brief duplicate() duplicates string content in a memory zone. This memory zone must be freed later on. */
  _Utf* duplicate() const
  {
    if (isEmpty())
      return nullptr;

    _Utf* wDupPtr=(_Utf*)malloc(ByteSize);
    if (wDupPtr==nullptr)
    {
      fprintf (stdout,"utfVaryingString::duplicate-S-MEMALLOC cannot allocate %ld bytes.\n",ByteSize);
      return nullptr;
    }

    memset(wDupPtr,0,ByteSize);

    _Utf* wInPtr=wDupPtr;
    _Utf* wToReplace;

    _Utf* wCurrentPtr = Data;

    _Utf* wDataBoundary = Data ;
    wDataBoundary += UnitCount;

    /* effective string duplication to current memory zone */

    while (*wCurrentPtr && (wCurrentPtr < wDataBoundary))  /* either string is zero terminated or use data boundary */
      *wInPtr++=*wCurrentPtr++;

    return wDupPtr;
  }//duplicate


  /** @brief findEnquoted() extracts substring enquoted by pQuote1 at left and pQuote2 at right
 *  returns an allocated string with substring content duly terminated with '\0'.
 *  returned substring needs to be freed by callee.
 */
  utfVaryingString findEnquoted(_Utf pQuote1,_Utf pQuote2,size_t pOffset=0)
  {
    if (pOffset > UnitCount-2)
      return utfVaryingString();

    _Utf* wStr=utfFindEnquoted<_Utf>(Data+pOffset,pQuote1,pQuote2);
    utfVaryingString wReturn;
    wReturn.strset(wStr);
    zfree(wStr);
    return wReturn;
  }

  bool isNumeric()
  {
    return utfIsdigit<_Utf>(Data[0]);
  }

  ZArray<utfVaryingString> strtok(const _Utf* pSeparator)
  {
    ZArray<utfVaryingString> wReturn;
    utfVaryingString wToken;
    if (isEmpty())
      return wReturn;

    _Utf* wPtrOrig=duplicate();
    _Utf* wPtr;
    _Utf* wPtr1=nullptr;

    wPtr=wPtrOrig;
    while (wPtr)
      {
      wPtr1=utfFirstinSet<_Utf>(wPtr,pSeparator);
      if (!wPtr1)
        break ;
      *wPtr1=0;
      wPtr1++;
      wToken = wPtr;
      wReturn.push(wToken);
      wPtr=utfFirstNotinSet<_Utf>(wPtr1,pSeparator);
      }

    zfree (wPtrOrig);
    return wReturn;
  }//strtok
  /**
   * @brief countWords counts the number of words contained within string
   * @return a size_t value with number of words
   */
  size_t countWords() const
  {
    if (isEmpty())
      return 0;

    const char* wCSeparator=" ,;./\\<>[]\n\r";
    _Utf pSeparator[20];
    int wi=0;
    for (wi=0;wCSeparator[wi];wi++)
      pSeparator[wi]=_Utf(wCSeparator[wi]);

    while (wi<20)
      pSeparator[wi++]=0;

    size_t wCount=0;
    _Utf* wPtrOrig=duplicate();
    _Utf* wPtr;
    _Utf* wPtr1=nullptr;

    wPtr=wPtrOrig;
    while (wPtr)
    {
      wPtr1=utfFirstinSet<_Utf>(wPtr,pSeparator);
      if (!wPtr1)
        break ;
      *wPtr1=0;
      wPtr1++;
      wCount++;
      wPtr=utfFirstNotinSet<_Utf>(wPtr1,pSeparator);
    }

    zfree (wPtrOrig);
    return wCount;
  }//countWords

  /**
 * @brief utftemplateString<_Sz,_Utf>::eliminateChar scans current string content and wipes (eliminates) the given char.
 * After having called this procedure, the given char does no more exist within the string,
 * so that string length is reduced in proportion of number of char eliminated.
 * @param[in] pChar character to eliminate
 * @return a reference to the current utftemplateString
 */
  utfVaryingString &
  eliminateChar (_Utf pChar)
  {
    _Utf* wPtr = Data ;
    int wCount=0;
    while (*wPtr!=(_Utf)'\0')
    {
      if (*wPtr==pChar)
        {
        utfStrcpy<_Utf>(wPtr,&wPtr[1]);
        wCount++;
        }
      else
        wPtr++;
    }//while
    if (wCount > 0)
        truncate(UnitCount - wCount);
    return *this;
  }//eliminateChar

/** @brief replace() replaces all occurrences of string pToBeReplaced by string pReplace */

  utfVaryingString& replace(const _Utf* pToBeReplaced, const _Utf* pToReplace)
    {
      if (pToBeReplaced==nullptr)
          return *this;
      if (*pToBeReplaced == 0)  // empty string : nothing to be replaced
        return *this;
      ssize_t wSizeToBeReplaced=0;  /* compute size to be replaced in units */
      const _Utf* wPtr=pToBeReplaced;
      while (*wPtr++)
        wSizeToBeReplaced++;

      ssize_t wSizeToReplace=0; /* compute size to replace in units */
      wPtr=pToReplace;
      while (*wPtr++)
        wSizeToReplace++;

      ssize_t wDelta = wSizeToReplace - wSizeToBeReplaced;

      size_t wMatchNb=0;
      _Utf* wMatchPtr;
      _Utf* wCurrentPtr;
      const _Utf* wReplace=pToReplace;

      wMatchPtr=utfStrstr<_Utf>(Data,pToBeReplaced);
      if (wMatchPtr==nullptr)  /* to be replaced is not found */
        {
//        fprintf(stdout,"utfVaryingString::replace   O replacement made. String <%s> not found.\n",pToReplace);
        return *this;
        }

//      fprintf(stdout,"utfVaryingString::replace to replace size is <%ld> to be replaced size is <%ld>  delta is <%ld> \n",
//            wSizeToReplace,wSizeToBeReplaced,wDelta);

      if (wDelta == 0) /* tobereplaced same size than toreplace */
      {
        while (wMatchPtr!=nullptr)
        {
          wMatchNb ++;
          wCurrentPtr = wMatchPtr;
          wReplace=pToReplace;
          while (*wReplace)
            *wCurrentPtr++=*wReplace++;
          wMatchPtr = wCurrentPtr;
        wMatchPtr=utfStrstr<_Utf>(wMatchPtr,pToBeReplaced);
        }
//      fprintf(stdout,"utfVaryingString::replace  %ld replacements made.\n",wMatchNb);
      return *this;
      }


      if (wDelta < 0) /* tobereplaced is larger than toreplace : must truncate allocation */
      {
      size_t wFormerUnitCount = UnitCount;

      ssize_t wSizeToReduce=0;

      _Utf* wReadPtr=duplicate();
      if (wReadPtr==nullptr)
          return *this;

      _Utf* wTargetPtr=Data;

      wMatchNb = 0;


      wCurrentPtr=wReadPtr;
      wMatchPtr=utfStrstr<_Utf>(wReadPtr,pToBeReplaced);

      while (wMatchPtr!=nullptr)
      {
        wMatchNb ++;
        wSizeToReduce += wDelta; // decrease Size to reduce

        /* copy duplicated string until first match */

        while (wCurrentPtr < wMatchPtr)
          *wTargetPtr++=*wCurrentPtr++;

        /* copy replacement string to target */
        _Utf* wToReplace=(_Utf*)pToReplace;

        while (*wToReplace)
          *wTargetPtr++=*wToReplace++;

        wCurrentPtr += wSizeToBeReplaced;
        /* find next match */
        wMatchPtr=utfStrstr<_Utf>(wCurrentPtr,pToBeReplaced);
      } // while (wMatchPtr!=nullptr)

      /* no more match : end up in copying the rest of source string until end of string mark */
      wMatchPtr = wReadPtr;
      wMatchPtr += wFormerUnitCount;

      while (*wCurrentPtr && ( wCurrentPtr < wMatchPtr))
        *wTargetPtr++=*wCurrentPtr++;

      *wCurrentPtr = 0; /* put an end of string mark */

      /* free allocated string */

      zfree (wReadPtr);

//      fprintf (stdout,"utfVaryingString::replace %ld replaced.\n",
//          wMatchNb);


      if (wDelta < 0)
      truncate(UnitCount+wSizeToReduce);

      return *this;
      }//if (wDelta < 0)

/*  size tobereplaced less than size toreplace (Delta > 0) : must increase allocation
 *  -------------------------------------------
 */

       /* duplicate string content in a readonly memory zone */

      _Utf* wReadPtr=duplicate();
      if (wReadPtr==nullptr)
        return *this;

      /* count number of matches */

      wCurrentPtr = wMatchPtr;

      size_t wSizeToExtend = 0;
      wMatchNb = 0;
      while (wCurrentPtr!=nullptr)
            {
            wCurrentPtr += wSizeToBeReplaced;
            wSizeToExtend += wDelta ;
            wMatchNb ++;
            wCurrentPtr=utfStrstr<_Utf>(wCurrentPtr,pToBeReplaced);
            }
 //     fprintf (stdout,"utfVaryingString::replace %ld matched. Extending string with <%d> units.\n",
 //               wMatchNb,
 //               wSizeToExtend);

      size_t wFormerUnitCount = UnitCount;

      /* allocate full size to string */
      allocateUnitsBZero(UnitCount+wSizeToExtend);

      _Utf* wTargetPtr=Data;

      wMatchNb = 0;


      wCurrentPtr=wReadPtr;

      wMatchPtr=utfStrstr<_Utf>(wCurrentPtr,pToBeReplaced);

      while (wMatchPtr!=nullptr)
        {
          wMatchNb++;
          /* copy duplicated string until first match */

          while (wCurrentPtr < wMatchPtr)
            *wTargetPtr++=*wCurrentPtr++;

          /* copy replacement string to target */
          _Utf* wToReplace=(_Utf*)pToReplace;

          while (*wToReplace)
            *wTargetPtr++=*wToReplace++;

          /* skip <to be replaced> in source string */

          wCurrentPtr += wSizeToBeReplaced;

          /* find next match */
          wMatchPtr=utfStrstr<_Utf>(wCurrentPtr,pToBeReplaced);
        } // while (wMatchPtr!=nullptr)

     /* no more match : end up in copying the rest of source string until end of string mark */
     wMatchPtr = wReadPtr;
     wMatchPtr += wFormerUnitCount;

     while (*wCurrentPtr && ( wCurrentPtr < wMatchPtr))
        *wTargetPtr++=*wCurrentPtr++;

     *wCurrentPtr = 0; /* put an end of string mark */

     /* free allocated string */

     zfree (wReadPtr);

//     fprintf (stdout,"utfVaryingString::replace %ld replaced.\n",
//          wMatchNb);

     /* done */
     return *this;

    }//replace


   utfVaryingString& replace(utfVaryingString<_Utf>&& pToBeReplaced,utfVaryingString<_Utf>&& pReplace)
          {  return replace (pToBeReplaced.Data,pReplace.Data);}

    /** @brief removeData suppresses data content at pOffset on pCount character units.
     * Adjusts storage size.
     * Abort if pOffset+pLength greater than actual data size with ENOMEM.*/
    utfVaryingString& removeData (size_t pOffset,size_t pCount)
            {

            if (UnitCount<(pOffset+pCount))
                    {
                    errno =ENOMEM;
                    fprintf(stderr,"utfVaryingString operation not allowed : exceeding storage boundary.\n%s\n",strerror(ENOMEM));
                    abort();
                    }

        _Utf* wPtr = Data+pOffset, *wPtrSrc= Data+pOffset+pCount;
        size_t wCount=pCount;
        while (wCount--)
                *wPtr++=wPtrSrc++;
            allocateUnits (UnitCount-pCount);
            return  *this;
            }

    bool isNull(void) const {return (Data==nullptr);}
    bool isEmpty(void) const  {return (Data==nullptr) || (ByteSize==0) || (Data[0]==0);}

    void freeData(void);

    const _Utf* toUtf(void) const
    {
      if (Data==nullptr)
        return (const _Utf*)"";
      return Data;
    }

/*  string routines : strcpy is replaced by strset - strcat is replaced by stradd, etc. */

//    int compare(const _Utf* pString2) {return utfStrcmp<_Utf>(Data,pString2);}                       /** corresponds to strcmp */
//    int ncompare(const _Utf* pString2,size_t pCount) {return utfStrncmp<_Utf>(Data,pString2,pCount);}/** corresponds to strncmp */
    int compare(const _Utf* pString2) const ;                      /* corresponds to strcmp */

    /* following is set to able to compare any utf varying string with a char string : it will be set within derived classes */
    template <class _Utf1>
    int compareV(const _Utf1* pString2) const
    {
    _Utf wCChar;
    if (pString2==nullptr)
                return 1;
    if (pString2==nullptr)
                return 1;

    const _Utf *s1 = (const _Utf *) Data;
    const char *s2 = pString2;
    wCChar=(_Utf)*s2;
    while ((*s1 == wCChar )&&(*s1)&&(*s2))
        {
        s1++;
        s2++;
        wCChar=(_Utf)*s2;
        }
    return *s1 - wCChar;
    }
    int ncompare(const _Utf* pString2,size_t pCount) ; /** corresponds to strncmp */

    ssize_t strlen() const {return (utfStrlen<_Utf>(Data));}

    const _Utf* toString() const ;
    _Utf* toNString (_Utf*pString, size_t pCount);

    void fromUtf (const _Utf* pIn);

    utfVaryingString<_Utf> &strset( const _Utf *wSrc); /** corresponds to strcpy: sets currents string content with wSrc. Allocates characters for containing wSrc*/
    utfVaryingString<_Utf>& strnset ( const _Utf *wSrc, size_t pCount);/** corresponds to strncpy: sets currents string content on pCount with wSrc. Allocates characters */

    utfVaryingString<_Utf> &add(const _Utf *wSrc) {return _addV<const _Utf>(wSrc);}   /** corresponds to strcat: add _Utf string to current string. Extends allocation */
    utfVaryingString<_Utf> &nadd( const _Utf *wSrc, size_t pCount); /** corresponds to strncat: adds counted _Utf string wSrc to current string. Extends allocation */
    utfVaryingString<_Utf> &addUtfUnit(const _Utf pChar); /** adds an Utf unit at the end of varying string (not necessarily a character) plus _Utf \0 sign. */

    utfVaryingString<_Utf> &add(const utfVaryingString<_Utf>& wSrc) {return _addV<_Utf>(wSrc.Data);}
    utfVaryingString<_Utf> &add(const utfVaryingString<_Utf>&& wSrc){return _addV<_Utf>(wSrc.Data);}

    utfVaryingString<_Utf> &add(const std::string& wSrc){return _addV<char>((char*)wSrc.c_str());}
    utfVaryingString<_Utf> &add(const std::string&& wSrc){return _addV<char>((char*)wSrc.c_str());}


    utfVaryingString<_Utf> &addConditionalOR(const _Utf*pString)
    {
      if (!pString)
        return *this;
      if (!*pString)
        return *this;
      _Utf wOr[4];
      wOr[0]=_Utf(' ');
      wOr[1]=_Utf('|');
      wOr[2]=_Utf(' ');
      wOr[3]=0;
      if (!isEmpty())
        add(wOr);

      return _addV<const _Utf>(pString);
    }


    /**
     * @brief _rfindEOSM reverse find for end of string mark
     * @return a pointer to end of string mark
     */
    _Utf* _rfindEOSM();


    bool startsCaseWith(const _Utf* pString);
    bool startsWith(const _Utf* pString) const;

    template <class _Utf1>
    utfVaryingString<_Utf> &_addV( _Utf1* wSrc)
    {
      if (wSrc==nullptr)
        return *this;
      if (!wSrc[0])
        return *this;

      size_t wAddCount=utfStrlen<_Utf1>(wSrc);
      if (!wAddCount)
        return *this;

      _Utf* wPtr;
      if (Data==nullptr)
        wPtr=allocateUnits(wAddCount+1);
      else
      {
        wPtr=Data;
        while (*wPtr && (wPtr < &Data[UnitCount]))
          wPtr++;

        size_t wOffset=wPtr-Data;

 //       ssize_t wNeed =  ( wCount + wP + 1 ) - UnitCount ;
        ssize_t wNeed =  ( wOffset + wAddCount +  1 ) - UnitCount ;

        if (wNeed > 0)
          _Utf* wPtr2=extendUnits(wNeed); /* extendUnits change Data pointer value */
        wPtr=Data+wOffset;
      }
      while(*wSrc && wAddCount--)
      {
        *wPtr=(_Utf)*wSrc;
        wPtr++;
        wSrc++;
      }
      *wPtr= 0; /* put last end of string mark */
      return *this;
    }// addV
//    utfVaryingString<_Utf> &add( const _Utf *wSrc, size_t pCount); /** corresponds to strncat: adds counted _Utf string wSrc to current string. Extends allocation */

    ZStatus sprintf( const std::conditional<std::is_same<_Utf,char>::value, utf8_t, _Utf> *pFormat,...);/** sets currents string content with wSrc. Allocates characters */
    ZStatus addsprintf(const std::conditional<std::is_same<_Utf,char>::value, utf8_t, _Utf>* pFormat,...);  /** adds formatted content to current string. Extends characters allocation to make string fit */

    int vsnprintf(size_t pMaxSize,const typename std::conditional<std::is_same<_Utf,char>::value,utf8_t,_Utf>* pFormat,va_list args);
    int vsnprintf(size_t pMaxSize,const char* pFormat,va_list args);

    ZStatus sprintf(const char *pFormat,...);/** sets currents string content with wSrc. Allocates characters */
    ZStatus addsprintf(const char* pFormat,...);  /** adds formatted content to current string. Extends characters allocation to make string fit */

    utfVaryingString<_Utf> &fromCString_Straight(const char*pCString);  /** sets content to C String. Allocates/reallocates characters to make string fit */
    utfVaryingString<_Utf> &addCString_Straight(const char*pInString);  /** adds C String to current string. Extends characters allocation to make string fit */

    utfVaryingString<_Utf>& fromUtf_Straight(const _Utf* pInString);
    utfVaryingString<_Utf>& addUtf_Straight(const _Utf* pInString);

    utfVaryingString<_Utf>& fromStdString(const std::string pString);
    utfVaryingString<_Utf>& fromStdWString(const std::wstring pWString);


    /** @brief strchr() finds first occurrence of a single _Utf character wChar within current string content.
     * returns an _Utf* pointer to char position, or nullptr if not found or if string is empty */
    _Utf * strchr(_Utf wChar) const
    {
      if (isEmpty())
        return nullptr;
      return utfStrchr<_Utf>(Data,wChar);
    }
    /** @brief strchr_o() finds first occurrence of a single _Utf character wChar within current string content.
     * returns the offset to char position, or -1 if not found or if string is empty */
    long strchr_o(_Utf wChar) const
    {
      if (isEmpty())
        return -1;
      _Utf* wPtr=utfStrchr<_Utf>(Data,wChar);
      if (!wPtr)
        return -1;
      return  (wPtr - Data) ;
    }//strchr_o
    /** @brief strrchr() finds last occurrence of _Utf char wChar within current string content.
     * returns an _Utf* pointer to char position, or nullptr if not found or if string is empty */
    _Utf * strrchr(_Utf wChar)const
    {
      if (isEmpty())
        return nullptr;
      return utfStrrchr<_Utf>(Data,wChar);
    }
    /** @brief strrchr_o() finds last occurrence of _Utf char wChar within current string content.
     * returns the offset to char position, or -1 if not found or if string is empty */
    long strrchr_o(_Utf wChar)const
    {
      if (isEmpty())
        return -1;
      _Utf* wPtr=utfStrrchr<_Utf>(Data,wChar);
      if (!wPtr)
        return -1;
      return  (wPtr - Data) ;
    }//strrchr_o

    /** @brief strstr() finds first _Utf substring within current string.
     *  returns a pointer to char position,  nullptr if not found or if string is empty */
    _Utf * strstr(const _Utf *pSubstring) const
    {
      if (isEmpty())
        return nullptr;
      return utfStrstr<_Utf>(Data,pSubstring);
    }
    /** @brief strstr_o() finds first _Utf substring within current string.
    *  returns the offset to char position,  -1 if not found or if string is empty */
    long strstr_o(const _Utf *pSubstring) const
    {
      if (isEmpty())
        return -1;
      _Utf* wPtr=utfStrstr<_Utf>(Data,pSubstring);
      if (!wPtr)
        return -1;
      return  (wPtr - Data) ;
    }

    /** @brief strcasestr() finds first _Utf substring CASE REGARDLESS within current string.
     *  returns a pointer to char position,  nullptr if not found or if string is empty */
    _Utf* strcasestr(const _Utf *pSubstring) const
    {
      if (isEmpty())
        return nullptr;
      return utfStrcasestr<_Utf>(Data,pSubstring);
    }
    /** @brief strcasestr_o() finds first _Utf substring CASE REGARDLESS within current string.
     *   returns the offset to char position,  -1 if not found or if string is empty */
    long strcasestr_o(const _Utf *pSubstring) const
    {
      if (isEmpty())
        return -1;
      _Utf* wPtr=utfStrcasestr<_Utf>(Data,pSubstring);
      if (!wPtr)
        return -1;
      return  (wPtr - Data) ;
    }

    /** @brief strrstr() finds last _Utf substring within current string.
     *  returns a pointer to char position,  nullptr if not found or if string is empty */
    _Utf *strrstr(const _Utf *pSubstring) const
    {
      if (isEmpty())
        return nullptr;
      return utfStrstr<_Utf>(Data,pSubstring);
    }
    /** @brief strrstr_o() finds last _Utf substring within current string.
     *   returns the offset to char position,  -1 if not found or if string is empty */
    long strrstr_o(const _Utf *pSubstring) const
    {
      if (isEmpty())
        return -1;
      _Utf* wPtr=utfStrstr<_Utf>(Data,pSubstring);
      if (!wPtr)
        return -1;
      return  (wPtr - Data) ;
    }

    /** @brief strrcasestr() finds last _Utf substring CASE REGARDLESS within current string.
     *  returns a pointer to char position,  nullptr if not found or if string is empty */
    _Utf *strrcasestr(const _Utf *pSubstring) const
    {
      if (isEmpty())
        return nullptr;
      return utfStrrcasestr<_Utf>(Data,pSubstring);
    }
    /** @brief strrcasestr_o() finds last _Utf substring CASE REGARDLESS within current string.
     *   returns the offset to char position,  -1 if not found or if string is empty */
    long strrcasestr_o(const _Utf *pSubstring) const
    {
      if (isEmpty())
        return -1;
      _Utf* wPtr=utfStrrcasestr<_Utf>(Data,pSubstring);
      if (!wPtr)
        return -1;
      return  (wPtr - Data) ;
    }

    /** @brief subString() returns a new string with content of current string starting a position pOffset and with pLen character units */
    utfVaryingString<_Utf> subString(size_t pOffset, int pLen=-1) const;

//------------------ operator overloads  ------------------------------------------------

    _Utf operator [] (const size_t pIdx)  { if(pIdx>getUnitCount()) return NULL; return (Data[pIdx]);}

    utfVaryingString<_Utf> & operator = (const _Utf* pString) {return strset(pString);}
    utfVaryingString<_Utf> & operator += (const _Utf* pString) {return add(pString);}

     utfVaryingString<_Utf>& operator += (const _Utf pChar) { return addUtfUnit(pChar); }
//    utfVaryingString&fromWString(const wchar_t* pWString) { allocate((wcslen(pWString)+1)*sizeof(WDataChar[0])); wcsncpy(WDataChar,pWString,wcslen(pWString)); return *this; }


     utfVaryingString& operator = (const utfVaryingString&& pIn) { return _copyFrom(pIn); }
     utfVaryingString& operator = (const utfVaryingString& pIn) {return _copyFrom(pIn);}

     utfVaryingString& operator += (const utfVaryingString& pIn) { return add(pIn); }
     utfVaryingString& operator += (const utfVaryingString&& pIn) { return add(pIn); }

     utfVaryingString& operator += (const std::string& pIn) { return _addV<char>((char*)pIn.c_str()); }
     utfVaryingString& operator += (const std::string&& pIn) { return _addV<char>((char*)pIn.c_str()); }


     bool operator == (const utfVaryingString& pIn) { return compare(pIn.Data)==0; }
     bool operator == (const _Utf* pIn) { return compare(pIn)==0; }
     bool operator != (const utfVaryingString& pIn) { return compare(pIn.Data); }
     bool operator != (const _Utf* pIn) { return compare(pIn); }

     bool operator > (const utfVaryingString& pIn) { return compare(pIn.Data)>0; }
     bool operator < (const utfVaryingString& pIn) { return compare(pIn.Data)<0; }

     double toDouble()
     {
         return utfStrtod<_Utf>(Data,nullptr);
     }
     int toInt(int pBase=10)
     {
         return utfStrtoi<_Utf>(Data,nullptr,pBase);
     }
     int toLong(int pBase=10)
     {
         return utfStrtol<_Utf>(Data,nullptr,pBase);
     }
     unsigned int toUInt(unsigned int pBase=10)
     {
         return (unsigned int)utfStrtoul<_Utf>(Data,nullptr,pBase);
     }
     unsigned long toULong(int pBase=10)
     {
         return utfStrtoul<_Utf>(Data,nullptr,pBase);
     }

    ZDataBuffer* _exportURF(ZDataBuffer *pURF) const;
    size_t _importURF(const unsigned char *&pURF);

     /** @brief _exportUVF  Exports a string to a Universal Varying Format (dedicated format for strings)
     *  Universal Varying  Format stores string data into a varying length string container excluding '\0' character terminator
     *  led by
     *   - uint8_t : char unit size
     *   - UVF_Size_type : number of character units of the string.
     * @return a ZDataBuffer with string content in Varying Universal Format set to big endian
     */
    ZDataBuffer _exportUVF() const;
    /** @brief _exportUVF same as previous but extends pUVF with string content in uvf format */
    ZDataBuffer& _exportAppendUVF(ZDataBuffer& pUVF) const;

    /** @brief _exportUVFPtr exports string as UVF format to a pointer and a max size of pSizeMax. Pointer is not updated */
    ZStatus _exportUVFPtr(unsigned char *pPtrOut, UVF_Size_type pSizeMax) const;

     /** @brief _importUVF Import string from Varying Universal Format (dedicated format for strings)
     *  Varying Universal Format stores string data into a varying length string container excluding '\0' character terminator
     *  led by
     *   - uint8_t : char unit size
     *   - UVF_Size_type : number of character units of the string.
     * Important : imported string format (utf-xx) must be the same as current string
     * @param[in,out] pUniversalPtr pointer to Varying Universal formatted data header.
     *                this pointer is updated to point to first byte after imported data.
     * @return total size IN BYTES of  bytes used from pUniversalPtr buffer (Overall used size including header)
     */
    size_t _importUVF(const unsigned char *&pPtrIn);

    /** @brief _getexportUVFSize() compute the requested export size in bytes for current string, including header */
    UVF_Size_type _getexportUVFSize() const;

    /** @brief _getimportUVFSize() returns total size in byte of data, including header, the import will use from input buffer.*/
    UVF_Size_type _getimportUVFSize(const unsigned char* pUniversalPtr);


    static ZStatus getUniversalFromURF(ZTypeBase pType, const unsigned char *pURFDataPtr, ZDataBuffer& pUniversal, const unsigned char **pURFDataPtrOut=nullptr);



    template<class _Tp>
    _Tp moveTo() const
    {
      _Tp wOut;

      if (sizeof(_Tp) != ByteSize)
      {
        fprintf(stderr,
            "utfVaryingString::moveTo-F-INVTYP Invalid data size <%ld> for type size <%ld>\n",
            ByteSize,
            sizeof(_Tp));
        abort();
      }
      memmove(&wOut, Data, sizeof(_Tp));
      if (Check!=0xFFFFFFFF)
      {
        fprintf(stderr,"utfVaryingString::clearData-F-OUTBOUND memory problem.\n");
        abort();
      }

      return wOut;
    }
    template<class _Tp>
    _Tp* moveToPtr() { return static_cast<_Tp*>(Data); }


/**
 * @brief moveOut <_Tp> loads the raw content of the utfVaryingString to a template defined data structure pOutData (Compound) given as _Tp
 * @param pOutData
 * @return
 */
template <typename _Tp>
_Tp& moveOut(typename std::enable_if<!(std::is_pointer<_Tp>::value||std::is_array<_Tp>::value),_Tp> &pOutData,ssize_t pOffset=0)
{
    ssize_t wSize;
    if (sizeof(_Tp) >= ByteSize )
                wSize=ByteSize;
        else
                wSize=sizeof(_Tp);

    memmove(&pOutData,Data+pOffset,wSize);
    if (Check!=0xFFFFFFFF)
    {
      fprintf(stderr,"utfVaryingString::clearData-F-OUTBOUND memory problem.\n");
      abort();
    }
    return pOutData;
}
template <typename _Tp>
_Tp& moveOut(typename std::enable_if<std::is_array<_Tp>::value,_Tp> &pOutData,ssize_t pOffset=0)
{
    ssize_t wSize;
    if (sizeof(_Tp) >= ByteSize )
                wSize=ByteSize;
        else
                wSize=sizeof(_Tp);

    memmove(pOutData,Data+pOffset,wSize);
    if (Check!=0xFFFFFFFF)
    {
      fprintf(stderr,"utfVaryingString::clearData-F-OUTBOUND memory problem.\n");
      abort();
    }
    return pOutData;
}
template <typename _Tp>
_Tp& moveOut(typename std::enable_if<std::is_pointer<_Tp>::value,_Tp> &pOutData,ssize_t pOffset=0)
{
    ssize_t wSize;
    if (sizeof(_Tp) >= ByteSize )
                wSize=ByteSize;
        else
                wSize=sizeof(_Tp);

    memmove(pOutData,Data+pOffset,wSize);
    if (Check!=0xFFFFFFFF)
    {
      fprintf(stderr,"utfVaryingString::clearData-F-OUTBOUND memory problem.\n");
      abort();
    }
    return pOutData;
}

    template <typename _Tp>
    /**
     * @brief move <_Tp> loads the content of  template defined data structure pInData (Compound) given as _Tp  to raw content of the utfVaryingString
     * @param pInData
     * @return
     */
    utfVaryingString& moveIn(_Tp &pInData)
    {
        ssize_t wSize=sizeof(_Tp) ;
        allocateBytes(sizeof(_Tp));
        memmove(Data,&pInData,wSize);
        if (Check!=0xFFFFFFFF)
        {
          fprintf(stderr,"utfVaryingString::clearData-F-OUTBOUND memory problem.\n");
          abort();
        }
        return *this;
    }



    utfVaryingString& clearData(void)
    {
      if (Data==nullptr)
        return(*this);
      memset(Data,0,ByteSize);
      if (Check!=0xFFFFFFFF)
        {
        fprintf(stderr,"utfVaryingString::clearData-F-OUTBOUND memory problem.\n");
        abort();
        }
      return (*this);
    }

    utfVaryingString& addTermination(void) {allocateUnits (UnitCount+1); Data[UnitCount-1]=0; return *this;}
    utfVaryingString& addConditionalTermination(void)
        {
        if (Data[UnitCount-1]==0)
                            return *this;
        allocateUnits (UnitCount+1);
        Data[UnitCount-1]=(_Utf)'\0';
        return *this;
        }

  utfVaryingString toUpper(void) const;
  utfVaryingString dropAccute(void) const;
  utfVaryingString toLower(void) const ;

    _Utf*   allocateBytes(ssize_t pSize);
    _Utf*   allocateUnits(ssize_t pCharCount);
    _Utf* allocateBytesBZero(ssize_t pSize);
    _Utf* allocateUnitsBZero(ssize_t pCharCount);


    _Utf* extendUnitsCond(const size_t pCount);
    _Utf* extendBytes (ssize_t pByteSize);
    _Utf* extendUnits (ssize_t pCharCount);
    _Utf* extendBytesBZero(ssize_t pSize);
    _Utf* extendUnitsBZero(ssize_t pCharCount);





   ssize_t bsearch (void *pKey, const size_t pKeyByteSize, const size_t pOffset=0);
   ssize_t bsearch(utfVaryingString &pKey,const size_t pOffset=0);

   ssize_t bsearchCaseRegardless (_Utf *pKey, ssize_t pSize, ssize_t pOffset);
   ssize_t bsearchCaseRegardless (utfVaryingString &pKey, ssize_t pOffset);

   ssize_t bstartwithCaseRegardless (void *pKey,ssize_t pSize,ssize_t pOffset=0);
   ssize_t bstartwithCaseRegardless (utfVaryingString pKey,size_t pOffset=0);

   ssize_t breverseSearch(void *pKey, ssize_t pKeySize, ssize_t pOffset=0);
   ssize_t breverseSearch(utfVaryingString<_Utf> &pKey, const ssize_t pOffset=0);

   ssize_t breverseSearchCaseRegardless(void *pKey, ssize_t pKeySize, ssize_t pOffset=0);
   ssize_t breverseSearchCaseRegardless(utfVaryingString<_Utf> &pKey,const ssize_t pOffset=0);

   bool isEqual (const _Utf *pCompare, long pSize=-1) ;
   bool isEqualCase (const _Utf *pCompare) ;
   bool isGreater (const _Utf *pCompare, long pSize=-1);
   bool isLess (const _Utf *pCompare, long pSize=-1);

   bool isEqual (const utfVaryingString<_Utf> &pCompare) ;
   bool isEqualCase (const utfVaryingString<_Utf> &pCompare) ;
   bool isGreater (const utfVaryingString<_Utf> &pCompare);
   bool isLess (const utfVaryingString<_Utf> &pCompare);


   bool contains (const _Utf *pString)
          {
            return !(strstr(pString)==nullptr);
          }
    /** @brief locate()  locates a substring pString in utftemplateString
    *                            and returns its offset from beginning as a ssize_t value.*/
    ssize_t locate(const _Utf *pString) const;
    /** @brief locate()  locates a substring pString in utftemplateString and returns its offset
    *                     since offset pOffset (zero meaning beginning of string) as a ssize_t value.*/
    ssize_t locate(const _Utf *pString, size_t pOffset) const;
    ssize_t locate(const utfVaryingString<_Utf>& pString) const;
    /** @brief locateCase()  locates a substring pString CASE REGARDLESS in utftemplateString
    *                         and returns its offset from beginning as a ssize_t value.*/
    ssize_t locateCase(const _Utf* pString) const;
    /** @brief locateCase()  locates a substring pString CASE REGARDLESS in utftemplateString
    *                        and returns its offset since pOffset (zero meaning beginning of string) as a ssize_t value.*/
    ssize_t locateCase(const _Utf* pString,size_t pOffset) const;

   _Utf  operator [] (long pIdx) {return Data[pIdx];}

   //const utfVaryingString<_Utf> & operator = (const char *pCharIn)
   //                             { return fromCString(pCharIn);}

/*   const utfVaryingString<_Utf> & operator = (const utfVaryingString<_Utf> &pString)
                                { allocateUnits(pString.ByteSize);
                                memmove(Data,pString.Data,ByteSize);
                                return(*this);}
*/
   utfVaryingString & operator = (const long pValue)
                    { sprintf("%ld",pValue); return *this;}
   utfVaryingString & operator = (const size_t pValue)
                    { sprintf("%ld",pValue); return *this;}


   utfVaryingString & operator = (const float pValue)
                    { sprintf("%g",pValue); return *this;}

   utfVaryingString & operator = (const double pValue)
                    { sprintf("%g",pValue); return *this;}

   ZStatus operator << (uriString & pURI);

 //   const utfVaryingString & operator += (const char* pString)
 //                                { appendData(pString,strlen(pString)+1); return *this;}
 //   utfVaryingString & operator += (const utfVaryingString &pDataBuffer)
 //                                { return appendData(pDataBuffer.Data,pDataBuffer.ByteSize);}
    utfVaryingString & operator += (const long pValue)
                                { return appendData (&pValue,sizeof(pValue));}
    utfVaryingString & operator += (const size_t pValue)
                                { return appendData (&pValue,sizeof(pValue));}
    utfVaryingString & operator += (const float pValue)
                                { return appendData (&pValue,sizeof(pValue));}
    utfVaryingString & operator += (const double pValue)
                                { return appendData (&pValue,sizeof(pValue));}

    void encodeB64( void);         // encode the content of utfVaryingString to Base64
    void decodeB64( void );     // decode the content of utfVaryingString from Base64

    checkSum getcheckSum(void) const
    {
      return checkSum((const unsigned char*)Data,strlen());
    }// gets a checkSum from string content without '\0'

    md5 getMD5( void ) const {return md5((unsigned char*)Data,strlen());} // gets a md5 from string content without '\0'

    void move(void* pDest)                  {memmove (pDest,Data,ByteSize);}
    /**
     * @brief moven moves current utfVaryingString content to pDest on utfVaryingString Size length with a maximum given by pSize
     * @param pDest
     * @param pSize
     */
    void moven(void* pDest,ssize_t pSize)    {ssize_t wS=pSize>ByteSize? ByteSize:pSize; memmove(pDest,Data,wS);}


    _Utf* firstNotinSet(const _Utf *pSet=__DEFAULTDELIMITERSET__)
            {return(utfFirstNotinSet<_Utf>(Data,pSet));}

    _Utf* lastNotinSet(const _Utf *pSet=__DEFAULTDELIMITERSET__)
            {return(utfLastNotinSet<_Utf>(Data,pSet));}

    _Utf* LTrim(const _Utf *pSet=__DEFAULTDELIMITERSET__)
            {return(utfLTrim<_Utf>(Data,pSet));}

    _Utf* RTrim(const _Utf *pSet=__DEFAULTDELIMITERSET__)
            {return(utfRTrim<_Utf>(Data,pSet));}

    _Utf* Trim(const _Utf *pSet=__DEFAULTDELIMITERSET__)
            {return(utfTrim<_Utf>(Data,pSet));}


    utfVaryingString<_Utf> Left (size_t pLen) const;
    utfVaryingString<_Utf> Right (size_t pLen) const ;

    utfVaryingString<_Utf> &setChar(const _Utf pChar, size_t pStart=0, ssize_t pCount=-1) ; /** sets the region of pStart on pCount to pChar. Extends allocation if necessary */

    void dumpHexa (const size_t pOffset, const size_t pCount, ZDataBuffer &pLineHexa, ZDataBuffer &pLineChar);

 //   friend int rulerSetup (utfVaryingString<_Utf> &pRulerHexa, utfVaryingString<_Utf> &pRulerAscii,int pColumn);

    void Dump(const int pColumn=16, ssize_t pLimit=-1,FILE* pOutput=stderr);

//=============== Context and conversion==============================================
    utfStrCtx   getContext(void) {return Context;}
    char*       dumpContext(void) {return Context._dump();}
    /** @weakgroup ActionOnConversionIssue Behavior and available options on conversion issues.<br>
     * Defines the behavior of conversion routines when an issue happens during conversion.
     *
     * <b>setStopOnError()</b> Simply stops conversion when an issue occur. An illegal character encountered will stop conversion.<br>
     * <b>setSubstitute()</b>  Substitutes invalid character with globally defined substitution character and continues conversion.<br>
     * see changeReplacementCodepoint() for explainations on how changing substitution character.<br>
     * <b>setSkip()</b>      Simply skips (ignore) invalid/illegal character<br>
     * <b>setEscape_C()</b>  Replaces any illegal character encountered with its C universal character encoding counterpart :<br>
     *  utf8 and utf16 \uhhhh  utf32 \Uhhhhhhhh   where h is an hexadecimal digit<br>
     */
    /** @brief setStopOnError simply stops conversion when an issue occur. An illegal character encountered will stop conversion. */
    void setStopOnError(void)   {Context.StopOnConvErr=true;}
    /** @brief setSubstitute Substitutes invalid character with globally defined substitution character and continues conversion.<br>
     * see changeReplacementCodepoint() for explainations on how changing substitution character.<br> */
    void setSubstitute(void)    {Context.setAction(ZCNV_Substitute); Context.StopOnConvErr=false;}
    /** @brief setSkip Ignores invalid/illegal character : output string will have no mention of this illegal character at all. */
    void setSkip(void)          {Context.setAction(ZCNV_Skip); Context.StopOnConvErr=false;}
    /** @brief setEscape_C Replaces any illegal character encountered in input string with its C universal character encoding counterpart :<br>
     *  utf8 and utf16 \\uhhhh  utf32 \\Uhhhhhhhh   where h is an hexadecimal digit*/
    void setEscape_C (void)     {Context.setAction(ZCNV_Escape_C); Context.StopOnConvErr=false;}
    /** @brief setEscape_Java Replaces any illegal character encountered in input string with its Java escaped character counterpart :<br>
     *  utf8 and utf16 \\uhhhh   where h is an hexadecimal digit.<br>
     * Remark: Java uses utf16 as standard, so that, using utf32 is a non-sense with this escape mode.*/
    void setEscape_Java (void)  {Context.setAction(ZCNV_Escape_Java); Context.StopOnConvErr=false;}
    /** @brief setEscape_ICU Replaces any illegal character encountered in input string with icu convention counterpart :<br>
     *  i.e. utf8 and utf16 \%Uhhhh  utf32 \%Uhhhhhhhh     where h is an hexadecimal digit.<br>
        Remark: this is not a standard notation, and is used only by icu. <br>
        It is not used by icu for utf32 character and covers only utf16 characters.<br>
        Using this format for utf32 is an extrapolation.*/
    void setEscape_ICU (void)   {Context.setAction(ZCNV_Escape_ICU); Context.StopOnConvErr=false;}
    /** @brief setEscape_XMLDec Replaces any illegal character encountered in input string with its XML decimal convention counterpart :<br>
     *  i.e. &#ddddd;     where d is a decimal digit*/
    void setEscape_XMLDec (void){Context.setAction(ZCNV_Escape_XML_Dec); Context.StopOnConvErr=false;}
    /** @brief setEscape_XMLHex Replaces any illegal character encountered in input string with its XML hexadecimal convention counterpart :<br>
     *  i.e. utf8 &#xhh;  utf16 &#xhhhh;  utf32 &#xhhhhhhhh;    where h is an hexadecimal digit*/
    void setEscape_XMLHex(void) {Context.setAction(ZCNV_Escape_XML_Hex); Context.StopOnConvErr=false;}
    /** @brief setEscape_Unicode Replaces any illegal character encountered in input string with its Unicode convention counterpart :<br>
     *  i.e. utf8 utf16 {U+hhhh}  utf32 {U+hhhhhhhh}    where h is an hexadecimal digit.<br>
        Remark: This is a notation convention. There is no guarantee that character codepoint is a valid codepoint: by the way, it should not be a valid codepoint.*/
    void setEscape_Unicode(void){Context.setAction(ZCNV_Escape_Unicode); Context.StopOnConvErr=false;}
    /** @brief setEscapeCSS2 Replaces any illegal character encountered in input string with its CSS2 character encoding counterpart :<br>
     *  utf8 \\hh utf16 \\hhhh  utf32 \\hhhhhhhh   where h is an hexadecimal digit*/
    void setEscapeCSS2 (void)    {Context.setAction(ZCNV_Escape_CSS2); Context.StopOnConvErr=false;}
    /** @brief setWarningSignal sets or resets an optional flag that allows -or disallows- conversion engine to generate warnings during character set conversion.<br>
     * In case of warning, ZException (ZException_min object) is created with warning content and severity set to Severity_Warning.<br>
     * This could especially be used when using icu conversion utilities.<br>
     * see ZException_min object definition.
     */
    void setWarningSignal(ZBool pOnOff) {WarningSignal=pOnOff;}
//=============SortKey====================================
    void newSortKey(void);

//    utf32VaryingString& addBOM( ZBool *pEndian);
protected:
    utfStrCtx   Context;
    ZBool       WarningSignal=false;
    ZBool       littleEndian=false;  // RFFU:this induce we might have native strings with endianness different from system default

    utfKey*     SortKey=nullptr;
}; // utfVaryingString

template <class _Utf>
utfVaryingString<_Utf>::utfVaryingString(void)

{
    Data=nullptr;
    DataByte=nullptr;
    utfInit();
    return;
}

template <class _Utf>
utfVaryingString<_Utf>::utfVaryingString(const utfVaryingString& pIn) : utfStringDescriptor(pIn)

{
    Data=nullptr;
    DataByte=nullptr;
    utfInit();
    _copyFrom(pIn);
    return;
}
template <class _Utf>
utfVaryingString<_Utf>::utfVaryingString(const utfVaryingString&& pIn): utfStringDescriptor(pIn)
{
    Data=nullptr;
    DataByte=nullptr;
    utfInit();
    _copyFrom(pIn);
    return;
}

template <class _Utf>
utfVaryingString<_Utf>::utfVaryingString (_Utf *pData,ssize_t pCount)
{
    utfInit();
    strnset(pData,pCount); return;
}

template <class _Utf>
utfVaryingString<_Utf>::utfVaryingString (size_t pCount)
{
    utfInit();
    allocateUnits(pCount);
}

template <class _Utf>
utfVaryingString<_Utf>::utfVaryingString (const utfStringDescriptor& pIn)
{
  utfStringDescriptor::_copyFrom(pIn);
  allocateBytes( pIn.ByteSize);
}

 template <class _Utf>
 void
 utfVaryingString<_Utf>::utfInit(void)
 {
    Check=0xFFFFFFFF;
    zfree(Data);

//    Data=nullptr;
    DataByte=nullptr;
    UnitCount=0;
    ByteSize=0;
    CryptMethod=ZCM_Uncrypted;
    Charset=ZCHARSET_SYSTEMDEFAULT;
    littleEndian = is_little_endian();
    return;
}// utfInit

template <class _Utf>
 utfVaryingString<_Utf>:: ~utfVaryingString ()
 {
     freeData();
//     if (SortKey!=nullptr) delete SortKey;
 }



/**
 * @brief utfVaryingString::truncate truncates allocated characters to pCount _Utf characters including '\0' _Utf last character.
 * @param pCount
 * @return
 */
template <class _Utf>
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::truncate(ssize_t pCount)
{

    if ((pCount>__INVALID_SIZE__)||(pCount<0))
    {
        fprintf(stderr,
                "%s-F-INVALIDSIZE  Fatal error trying to truncate varying utf string to invalid character count %ld \n",
                _GET_FUNCTION_NAME_,
                pCount);
        abort();
    }

    size_t wByteSize = pCount * sizeof(_Utf);
    /* reallocate enough size for requested units number plus end sign cst_ZSTART */
    _Utf* wData = zrealloc(Data,wByteSize + sizeof(uint32_t));

    ByteSize =  wByteSize;
    UnitCount = pCount;
    DataByte= (uint8_t*)wData;
    Data = wData;
    wData[pCount-1]=(_Utf)'\0';

    /* set the end of allocated / reallocated memory as cst_ZSTART */
    uint32_t* wP2=(uint32_t*)&DataByte[ByteSize];
    *wP2=cst_ZSTRINGEND;
    return  *this;
}// truncate

template <class _Utf>
void
utfVaryingString<_Utf>::freeData(void)
{
    zfree(Data);
    ByteSize=0;
    UnitCount=0;   
    DataByte=nullptr;
    return;
}// freeData


//============Classical _Utf string operations=================================
template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::strset sets string content with wSrc content , after having allocated enough characters for wSrc to fit.
 * @param wSrc _Utf source string
 * @return
 */
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::strset ( const _Utf *wSrc)
{

  ssize_t wCount=utfStrlen<_Utf>(wSrc);
  if (!wCount)
    return *this;

  _Utf* wPtr=allocateUnits(wCount+1);
  while (*wSrc)
      {
      *wPtr=*wSrc;
      wPtr++;
      wSrc++;
      }
  *wPtr=0;
  return *this;
}
template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::strnset sets string content with at maximum pCount character of wSrc content , after having allocated pCount.
 *  Padds resulting string to Zero until pCount character length if source string wSrc is not long enough.
 * @param wSrc  _Utf source string
 * @param pCount maximum number of character to set string with (since beginning of wSrc)
 * @return
 */
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::strnset ( const _Utf *wSrc, size_t pCount)
{  
  _Utf*wPtr=allocateUnits(pCount+1);
  size_t wi=0;
  if (wSrc!=nullptr)
  {
  for (;*wSrc && (wi < pCount) ;wi++)
      {
      *wPtr=*wSrc;
      wPtr++;
      wSrc++;
      }
  }// if (wSrc)
  while (wi++ < pCount)
    {
    *wPtr = 0;
    wPtr++;
    }
  addConditionalTermination();
  return *this;
}// strnset

template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::sprintf set current string content to a formatted _Utf* string (Warning: char is a special case).
 * Characters are allocated for the resulting string to fit.
 * To avoid collision with addsprintf overload for utftemplateString<_Sz,char>, pFormat is conditionned to be set to utf8_t when char is second template class and set to char in other cases.
 * This allows for utf string to have a format string that is a const char*, while it is the default when using char as second template class.
 *
 * Maximum resulting _Utf string will be cst_MaxSprintfBufferSize character units (set to 1024 by default : see utfvsprintf.h file).
 * If this length is bypassed, then resulting added string will be simply truncated without any more indication
 * .*
 * @param[in]  pFormat a formatting string corresponding to printf syntax :
 *          either a const _Utf* string when template class has _Utf equal to utf8_t, utf16_t or utf32_t
 *          or a const utf8_t* string when template class has _Utf equal to char (to avoid invalid overload).
 * @param[in] varying arguments as used with printf syntax
 * @return ZStatus value with following possible values
 *
 *  ZS_SUCCESS : everything went good
 *  ZS_MEMERROR : cannot allocate memory for internal buffers (system error)
 *  ZS_MEMOVFLW : internal buffers have not enough space for utf string to be processed.
 *                in this case, setMaxSprintfBufferCount(<amount>) has to be used prior invoking this method to increase allocated space.
 *                NB: Allocated space is by default __DEFAULT_SPRINTF_BUFFERCOUNT__
 */
ZStatus utfVaryingString<_Utf>::sprintf(const typename std::conditional<std::is_same<_Utf,char>::value,utf8_t,_Utf>* pFormat,...)  /** set current string to formatted content. Extends characters allocation to make string fit */
{
    if (!pFormat)
            {
            fprintf(stderr,"%s>> format string is nullpptr\n",_GET_FUNCTION_NAME_);
            return ZS_NULLPTR;
            }

    va_list ap;
    va_start(ap, pFormat);

    _Utf* wMS=zmalloc<_Utf>(MaxSprintfBufferCount*sizeof(_Utf));

    ssize_t wStringCount=(ssize_t)utfVsnprintf(Charset,wMS,MaxSprintfBufferCount,pFormat,ap);
    va_end(ap);

//    size_t wCount = utfStrlen<_Utf>(Data)+wStringCount+1;

    _Utf* *wAddPtr=wMS,*wPtr=allocateUnits(wStringCount+1);
//    _Utf* *wAddPtr=wMS,*wPtr=allocateUnits(wStringCount);

    if (wPtr==nullptr)
      return ZS_MEMERROR;

    while (wStringCount--)
                {
                *wPtr=*wAddPtr;
                wPtr++;
                wAddPtr++;
                }
    *wPtr=(_Utf)'\0';
    zfree (wMS);
    if (errno==ENOMEM)
        return ZS_MEMOVFLW;
    return ZS_SUCCESS;
}//sprintf

template <class _Utf>
int utfVaryingString<_Utf>::vsnprintf(size_t pMaxSize,const char* pFormat,va_list args)
{
  ZStatus wSt=ZS_SUCCESS;
  if (!pFormat)
  {
    fprintf(stderr,"%s>> format string is nullpptr\n",_GET_FUNCTION_NAME_);
    return -1;
  }

  size_t wFormatSize=::strlen(pFormat);
  if (wFormatSize >MaxSprintfBufferCount)
  {
    errno=ENOMEM;
    wSt=ZS_MEMOVFLW;
    wFormatSize=MaxSprintfBufferCount;
  }
  _Utf* wFormat=(_Utf*)malloc((wFormatSize+1)*sizeof(_Utf));
  if (wFormat==nullptr)
    {
    errno=ENOMEM;
    return ZS_MEMERROR;
    }

  _Utf* wPtr=wFormat;
  const char* wPtr1=pFormat;
  size_t wi=0;
  while (*wPtr1 && ( wi++ < wFormatSize) )
      *wPtr++=(_Utf)*wPtr1++;
  *wPtr=(_Utf)'\0';


  _Utf* wMS=(_Utf*)malloc((pMaxSize+1)*sizeof(_Utf));
  if (wMS==nullptr)
    {
    zfree(wFormat);
    fprintf(stderr,"%s>> cannot allocate memory for byte size <%d>\n",_GET_FUNCTION_NAME_,MaxSprintfBufferCount);
    return -1;
    }

  size_t wEffectiveSize=utfVsnprintf(Charset,wMS,pMaxSize,wFormat,args);
  size_t wSVSize=wEffectiveSize;

  _Utf* wAddPtr=wMS;
  wPtr=allocateUnits(wEffectiveSize+1);

  if (wPtr==nullptr)
    {
    errno=ENOMEM;
    return -1;
    }
  while (wEffectiveSize--)
  {
    *wPtr=*wAddPtr;
    wPtr++;
    wAddPtr++;
  }
  *wPtr=(_Utf)'\0';
  zfree (wMS);
  zfree (wFormat);
  return wSVSize;
}//vsnprintf format is char*

template <class _Utf>
int utfVaryingString<_Utf>::vsnprintf(size_t pMaxSize,const typename std::conditional<std::is_same<_Utf,char>::value,utf8_t,_Utf>* pFormat,va_list args)
{
  if (!pFormat)
  {
    fprintf(stderr,"%s>> format string is nullpptr\n",_GET_FUNCTION_NAME_);
    return -1;
  }

  _Utf* wMS=(_Utf*)malloc((pMaxSize+1)*sizeof(_Utf));
  if (wMS==nullptr)
  {
    fprintf(stderr,"%s>> cannot allocate memory for byte size <%d>\n",_GET_FUNCTION_NAME_,MaxSprintfBufferCount);
    return -1;
  }

  size_t wEffectiveSize=utfVsnprintf(Charset,wMS,pMaxSize,pFormat,args);
  size_t wSVSize=wEffectiveSize;

  _Utf* *wAddPtr=wMS,*wPtr=allocateUnits(wEffectiveSize+1);

  if (wPtr==nullptr)
  {
    errno=ENOMEM;
    return -1;
  }
  while (wEffectiveSize--)
  {
    *wPtr=*wAddPtr;
    wPtr++;
    wAddPtr++;
  }
  *wPtr=(_Utf)'\0';
  zfree (wMS);
  return wSVSize;
}//vsnprintf format is _Utf*
template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::addsprintf  concatenates a _Utf formatted string (pFormat) to existing string, defined by its _Utf format and its varying arguments list.
 * Allocates characters to make it fit into string.
 *
 * To avoid collision with addsprintf overload for utftemplateString<_Sz,char>, pFormat is conditionned to be set to utf8_t when char is second template class and set to char in other cases.
 * This allows for utf string to have a format string that is a const char*, while it is the default when using char as second template class.
 *
 * Maximum resulting _Utf string will be cst_MaxSprintfBufferSize character units (set to 1024 by default : see utfvsprintf.h file).
 * If this length is bypassed, then resulting added string will be simply truncated without any more indication.
 *
 * @param[in]  pFormat a formatting string corresponding to printf syntax :
 *          either a const _Utf* string when template class has _Utf equal to utf8_t, utf16_t or utf32_t
 *          or a const utf8_t* string when template class has _Utf equal to char (to avoid invalid overload).
 * @param[in] varying arguments as used with printf syntax
 * @return ZStatus value with following possible values
 *
 *  ZS_SUCCESS : everything went good
 *  ZS_MEMERROR : cannot allocate memory for internal buffers (system error)
 *  ZS_MEMOVFLW : internal buffers have not enough space for utf string to be processed.
 *                in this case, setMaxSprintfBufferCount(<amount>) has to be used prior invoking this method to increase allocated space.
 *                NB: Allocated space is by default __DEFAULT_SPRINTF_BUFFERCOUNT__
 *
 */
ZStatus
utfVaryingString<_Utf>::addsprintf(const typename std::conditional<std::is_same<_Utf,char>::value,utf8_t,_Utf>* pFormat,...)  /** adds formatted content to current string. Extends characters allocation to make string fit */
{

    if (!pFormat)
            {
            fprintf(stderr,"%s>> format string is nullpptr\n",_GET_FUNCTION_NAME_);
            return ZS_NULLPTR;
            }

    va_list ap;
    va_start(ap, pFormat);
    _Utf* wMS=(_Utf*)malloc(MaxSprintfBufferCount*sizeof(_Utf));
    if (wMS==nullptr)
        return ZS_MEMERROR;


//    size_t wStringCount=(size_t)utfVsnprintf<_Utf>(wMS,cst_MaxSprintfBufferSize,pFormat,ap);
    size_t wStringCount=utfVsnprintf(Charset,wMS,MaxSprintfBufferCount,pFormat,ap);
    va_end(ap);

 //   utfAddConditionalTermination<_Utf>(wMS,wStringCount);

    _Utf* wPtr=extendUnits(wStringCount);
//    extendChars(wStringCount+1);
//    _Utf* wPtr= Data+utfStrlen<_Utf>(Data);
    _Utf* wAddPtr=(_Utf*)&wMS[0];

    while (*wAddPtr)
                {
                *wPtr=*wAddPtr;
                wPtr++;
                wAddPtr++;
                }
     *wPtr=(_Utf)'\0';
    zfree (wMS);
    if (errno==ENOMEM)
        return ZS_MEMOVFLW;
    return ZS_SUCCESS;
}// addsprintf

template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::sprintf  sets a _Utf formatted string (pFormat) to existing string, defined by its _Utf format and its varying arguments list.
 * Allocates characters to make it fit into string.
 *
 * To avoid collision withsprintf overload for utftemplateString<_Sz,char>, pFormat is conditionned to be set to utf8_t when char is second template class and set to char in other cases.
 * This allows for utf string to have a format string that is a const char*, while it is the default when using char as second template class.
 *
 * Maximum resulting _Utf string will be cst_MaxSprintfBufferSize character units (set to 1024 by default : see utfvsprintf.h file).
 * If this length is bypassed, then resulting added string will be simply truncated without any more indication.
 *
 * @param[in]  pFormat a formatting string corresponding to printf syntax :
 *          either a const _Utf* string when template class has _Utf equal to utf8_t, utf16_t or utf32_t
 *          or a const utf8_t* string when template class has _Utf equal to char (to avoid invalid overload).
 * @param[in] varying arguments as used with printf syntax
 * @return ZStatus value with following possible values
 *
 *  ZS_SUCCESS : everything went good
 *  ZS_MEMERROR : cannot allocate memory for internal buffers (system error)
 *  ZS_MEMOVFLW : internal buffers have not enough space for utf string to be processed.
 *                in this case, setMaxSprintfBufferCount(<amount>) has to be used prior invoking this method to increase allocated space.
 *                NB: Allocated space is by default __DEFAULT_SPRINTF_BUFFERCOUNT__
 *
 */
ZStatus
utfVaryingString<_Utf>::sprintf(const char* pFormat,...)  /** set current string to formatted content. Extends characters allocation to make string fit */
{
  ZStatus wSt=ZS_SUCCESS;
    if (!pFormat)
            {
            fprintf(stderr,"%s>> format string is nullpptr\n",_GET_FUNCTION_NAME_);
            return ZS_NULLPTR;
            }
    va_list ap;
    va_start(ap, pFormat);

    size_t wFormatSize=::strlen(pFormat);
    if (wFormatSize >MaxSprintfBufferCount)
      {
      errno=ENOMEM;
      wSt=ZS_MEMOVFLW;
      wFormatSize=MaxSprintfBufferCount;
      }
    _Utf* wFormat=(_Utf*)malloc((wFormatSize+1)*sizeof(_Utf));
    if (wFormat==nullptr)
      {
      errno=ENOMEM;
      return ZS_MEMERROR;
      }

    _Utf* wPtr=wFormat;
    const char* wPtr1=pFormat;
    size_t wi=0;
    while (*wPtr1 && ( wi++ < wFormatSize) )
          *wPtr++=(_Utf)*wPtr1++;
    *wPtr=(_Utf)'\0';

//    _Utf wMS[cst_MaxSprintfBufferCount];
    _Utf* wMS=(_Utf*)malloc(MaxSprintfBufferCount*sizeof(_Utf));
    if (wMS==nullptr)
        {
        zfree(wFormat);
        return ZS_MEMERROR;
        }
//    ssize_t wStringCount=(ssize_t)utfVsnprintf<_Utf>(wMS,cst_MaxSprintfBufferSize,wFormat,ap);
    size_t wStringCount=utfVsnprintf(Charset,wMS,MaxSprintfBufferCount,wFormat,ap);
    va_end(ap);

//    size_t wCount = utfStrlen<_Utf>(Data)+wStringCount+1;

    _Utf* wAddPtr=&wMS[0];
    wPtr=allocateUnits(wStringCount+1);
//    wPtr=allocateUnits(wStringCount);
    while (wStringCount--)
                {
                *wPtr=*wAddPtr;
                wPtr++;
                wAddPtr++;
                }
    *wPtr=(_Utf)'\0';
    zfree(wFormat);
    zfree(wMS);
    if (errno==ENOMEM)
        return ZS_MEMOVFLW; /* buffer overflow */

    return wSt;
}//sprintf

template <class _Utf>

/**
 * @brief utfVaryingString<_Utf>::addsprintf  concatenates a _Utf formatted string (pFormat) to existing string, defined by its _Utf format and its varying arguments list.
 *  Allocates character units to make it fit into string.
 *  String could be ended by endofstring mark or not : Adds in any cases an end of string mark after operation.
 *
 * To avoid collision with addsprintf overload for utftemplateString<_Sz,char>, pFormat is conditionned to be set to utf8_t when char is second template class and set to char in other cases.
 * This allows for utf string to have a format string that is a const char*, while it is the default when using char as second template class.
 *
 * Maximum resulting _Utf string will be cst_MaxSprintfBufferSize character units (set to 1024 by default : see utfvsprintf.h file).
 * If this length is bypassed, then resulting added string will be simply truncated without any more indication.
 *
 * @param[in]  pFormat a formatting string corresponding to printf syntax :
 *          either a const _Utf* string when template class has _Utf equal to utf8_t, utf16_t or utf32_t
 *          or a const utf8_t* string when template class has _Utf equal to char (to avoid invalid overload).
 *
 * @param[in] varying arguments as used with printf syntax
 * @return ZStatus value with following possible values
 *
 *  ZS_SUCCESS : everything went good
 *  ZS_MEMERROR : cannot allocate memory for internal buffers (system error)
 *  ZS_MEMOVFLW : internal buffers have not enough space for utf string to be processed.
 *                in this case, setMaxSprintfBufferCount(<amount>) has to be used prior invoking this method to increase allocated space.
 *                NB: Allocated space is by default __DEFAULT_SPRINTF_BUFFERCOUNT__
 *
 */
ZStatus
utfVaryingString<_Utf>::addsprintf(const char* pFormat,...)  /** adds formatted content to current string. Extends characters allocation to make string fit */
{

    if (!pFormat)
            {
            fprintf(stderr,"%s>> format string is nullpptr\n",_GET_FUNCTION_NAME_);
            return ZS_NULLPTR;
            }

    va_list ap;
    va_start(ap, pFormat);


    _Utf* wFormat=(_Utf*)malloc(MaxSprintfBufferCount*sizeof(_Utf));
    if (wFormat==nullptr)
        return ZS_MEMERROR;
    _Utf* wPtr=wFormat;
    const char* wPtr1=pFormat;
    size_t wi=0;
    while (*wPtr1 && wi++< (MaxSprintfBufferCount-1))
                                    *wPtr++=(_Utf)*wPtr1++;
    *wPtr=(_Utf)'\0';

    _Utf* wMS=(_Utf*)malloc(MaxSprintfBufferCount*sizeof(_Utf));
    if (wMS==nullptr)
        {
        zfree(wFormat);
        return ZS_MEMERROR;
        }
//    size_t wStringCount=(size_t)utfVsnprintf<_Utf>(wMS,cst_MaxSprintfBufferSize,wFormat,ap);
    size_t wStringCount=utfVsnprintf(Charset,wMS,cst_MaxSprintfBufferCount,wFormat,ap);
    va_end(ap);

 //   utfAddConditionalTermination<_Utf>(wMS,wStringCount);
//    wPtr=extendUnitsCond(wStringCount);

    _addV<_Utf>(&wMS[0]);

#ifdef __COMMENT__
    if (Data && (UnitCount > 0))
    {
    if (Data[UnitCount-1]==0)  /* Before add : string is terminated by end of string mark */
      {
        wPtr=extendUnits(wStringCount); /* extend the exact number of incoming units */
        wPtr--;                 /* point to end of string mark */
      }
    else      /* Before add : string is NOT terminated by end of string mark : there will be an added unit at the end for end of string mark */
      wPtr=extendUnits(wStringCount+1); /* point to first added unit */
    }
    else
      wPtr=extendUnits(wStringCount+1); /* point to first added unit */



//    _Utf* wPtr= Data+utfStrlen<_Utf>(Data);
    _Utf* wAddPtr=(_Utf*)&wMS[0];

    while (*wAddPtr)
                {
                *wPtr=*wAddPtr;
                wPtr++;
                wAddPtr++;
                }

     *wPtr=(_Utf)'\0';  /* put last end of string mark */
#endif // __COMMENT__
     zfree(wFormat);
     zfree(wMS);
    if (errno==ENOMEM)
        return ZS_MEMOVFLW; /* buffer overflow */
    return ZS_SUCCESS;
}

template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::extendCharCond test if pCount additional character units will fit into allocated character units.
 *  if not, allocates the difference within available character units in string (after '\0' mark) and requested count.
 *  in any case, returns an _Utf pointer to first available character for extending string (set to '\0')
 * after having eventually extended appropriately character units allocation.
 * @param pCount requested number of character units
 * @return an _Utf pointer to first available character unit (set to '\0')
 */
inline _Utf*
utfVaryingString<_Utf>::extendUnitsCond(const size_t pCount)
{
  size_t wCurrentCount=0;
  _Utf* wPtr=&Data[UnitCount-1];  /* set to end of string memory */

  /* reverse search for 0 */
  while (*wPtr && (wPtr > Data))
    wPtr--;

  /* reverse  search for not 0 */
  while (!*wPtr && (wPtr > Data))
    wPtr--;

  if (wPtr==Data) /*  if string is NOT terminated by end of string mark  */
    return extendUnitsBZero(wCurrentCount+1); /* string is NOT terminated by end of string mark : there will be an added unit at the end for end of string mark */

  wPtr++; /* position to end of string mark */


  size_t wStrSize=wPtr-Data; /* string size in bytes */

  size_t wRemain = (ByteSize - wStrSize) / sizeof(_Utf) ;

  if (wRemain < pCount) /* if no need to extend */
      return wPtr;

  extendUnitsBZero(pCount - wRemain);

  return wPtr;

}
#ifdef __COMMENT__
template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::add adds (concatenates) _Utf string content to current string.
 *  Extends characters allocation to make string fit
 * @param wSrc
 * @return
 */
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::add(const _Utf *wSrc)
{
    if (!wSrc)
            return *this;

    size_t wCount=utfStrlen<_Utf>(wSrc);
    if (!wCount)
            return *this;

    _Utf* wPtr=extendUnits(wCount);
    while(*wSrc && wCount--)
            {
            *wPtr=*wSrc;
            wPtr++;
            wSrc++;
            }
    *wPtr=(_Utf)'\0';
    return *this;
}// stradd

template <class _Utf>
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::_add(const utfVaryingString<_Utf>& wSrc)
{
    if (wSrc.isEmpty())
            return *this;


    size_t wCount=0;
    if (wSrc.isEmpty())
            return *this;

    _Utf* wSrcPtr=wSrc.Data;
    while (*wSrcPtr++ && (wCount < wSrc.UnitCount))
      wCount++;

    wSrcPtr=wSrc.Data;

    _Utf* wPtr=extendUnits(wCount);
    while(*wSrcPtr && wCount--)
            {
            *wPtr=*wSrcPtr;
            wPtr++;
            wSrcPtr++;
            }
    *wPtr=(_Utf)'\0';
    return *this;
}// add
#endif // __COMMENT__
template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::strnadd concatenates wSrc _Utf string to current _Utf content with at maximum pCount characters from wSrc.
 *  Extends character unit capacity to make it fit to current string.
 *  string is extended either for adding wSrc whole string, or to add pCount character of wSrc.
 * Last _Utf character '\0' will be added as last character, after that pCount or wSrc length will be added to current string length.
 *
 * @param wSrc _Utf string to concatenate
 * @param pCount Maximum number of characters from wSrc to concatenate
 * @return
 */
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::nadd( const _Utf *wSrc, size_t pCount)
{
    size_t wCount=utfStrlen<_Utf>(wSrc);
    size_t wAddCount=wCount < pCount ? wCount:pCount;
    _Utf* wPtr=extendUnits(wCount);
    while(*wSrc && wAddCount--)
                    {
                    *wPtr=*wSrc;
                    wPtr++;
                    wSrc++;
                    }
    *wPtr=(_Utf)'\0';
    return *this;
} //nadd

template<class _Utf>
_Utf*
utfVaryingString<_Utf>::_rfindEOSM()
{
  // size_t wS=utfStrlen(Data);

  if (Data==nullptr)
    return nullptr;

  _Utf* wPtr1= &Data[UnitCount-1];

  while (*wPtr1 && (wPtr1 > Data))  /* reverse find first zero */
    wPtr1--;

  while ( !*wPtr1 && (wPtr1 > Data))  /* reverse find first not zero */
    wPtr1--;

  wPtr1++;  /* point to end of string mark */

  return wPtr1;

} //_rfindEOM


/**
 * @brief addUtfUnit adds an Utf unit at the end of varying string (a unit is not necessarily a character) plus _Utf \0 sign.
 *  extends conditionally the string's units allocation if necessary (strlen() vs allocation)
 * @param pChar _Utf character unit to add
 * @return the current string object as reference
 */
template<class _Utf>
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::addUtfUnit(const _Utf pChar)
{
 // size_t wS=utfStrlen(Data);

  if (Data==nullptr)
    {
     _Utf* wPtr= extendUnits(2);
     *wPtr++=pChar;
     *wPtr=0;
     return *this;
    }

  _Utf* wPtr=Data;
  while (*wPtr && (wPtr < &Data[UnitCount]))
    wPtr++;

 /* _Utf* wPtr= _rfindEOSM();
  if (wPtr==nullptr)
  {
    wPtr= extendUnits(2);
    *wPtr++=pChar;
    *wPtr=0;
    return *this;
  }
*/
  size_t wOffset = wPtr-Data;

  extendUnitsBZero(2);  /* changes Data position */

  wPtr=Data + wOffset;

  *wPtr++=pChar;
  *wPtr=0;
  return *this;
} //addUtfUnit

template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::fromCString sets (replaces) existing string content  with pInString.
 *  Sets allocation with number of characters of pInString.
 *  pInString is supposed to be US-ASCII or at least, unicode compatible one byte per char string.
 * @param pInString
 * @return
 */
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::fromCString_Straight(const char* pCString)
{
    if (!pCString)
            return *this;
    allocateUnits(::strlen(pCString)+1);
    _Utf* wPtr=Data;
    while (*pCString)
                *wPtr++=(_Utf)*pCString++;
    *wPtr=(_Utf)'\0';
    return *this;
}// fromCString
template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::addCString_Straight  adds (concatenates) pInString as a char* string to existing string.
 *  Extends allocation with number of characters of pInString.
 *  pInString has to be US-ASCII or at least, unicode compatible one byte per char string.
 * @param pInString
 * @return
 */
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::addCString_Straight(const char* pInString)
{
    clearData();
    _Utf* wPtr=extendUnits(::strlen(pInString)+1);
    while (*pInString)
                *wPtr++=(_Utf)pInString++;
    *wPtr=(_Utf)'\0';
    return *this;
}//addCString_Straight

template <class _Utf>
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::fromUtf_Straight(const _Utf* pInString)
{
    clearData();
    if (!pInString)
            return nullptr;
    if (!*pInString)
        {
            return *this;
        }
    allocateUnits(utfStrlen<_Utf>(pInString)+1);
    _Utf* wPtr=Data;
    while (*pInString)
                *wPtr++=(_Utf)*pInString++;
    *wPtr=(_Utf)'\0';
    return *this;
}// fromUtf_Straight
template <class _Utf>
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::addUtf_Straight(const _Utf* pInString)
{
    clearData();
    _Utf* wPtr=extendUnits(utfStrlen<_Utf>(pInString)+1);
    while (*pInString)
                *wPtr++=pInString++;
    *wPtr=(_Utf)'\0';
    return *this;
}// fromUtf_Straigh

template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::fromCString sets (replaces) existing string content  with pInString.
 *  Sets allocation with number of characters of pInString.
 *  pInString is supposed to be US-ASCII or at least, unicode compatible one byte per char string.
 * @param pInString
 * @return
 */
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::fromStdString(const std::string pString)
{

    allocateUnits(pString.size()+1);
    _Utf* wPtr=Data;
    long wi=0;
    while (pString[wi])
                *wPtr++=(_Utf)pString[wi++];
    *wPtr=(_Utf)'\0';
    return *this;
}// fromStdString

template <class _Utf>
/**
 * @brief utfVaryingString<_Utf>::fromCString sets (replaces) existing string content  with pInString.
 *  Sets allocation with number of characters of pInString.
 *  pInString is supposed to be US-ASCII or at least, unicode compatible one byte per char string.
 * @param pInString
 * @return
 */
utfVaryingString<_Utf>&
utfVaryingString<_Utf>::fromStdWString(const std::wstring pWString)
{
    allocateUnits(pWString.size()+1);
    _Utf* wPtr=Data;

    size_t wi=0;
    while (pWString[wi])
                *wPtr++=(_Utf)pWString[wi++];
    *wPtr=(_Utf)'\0';
    return *this;
}// fromStdWString


template<class _Utf>
utfVaryingString<_Utf> utfVaryingString<_Utf>::subString(size_t pOffset, int pLen) const
{

  utfVaryingString<_Utf> wStrReturn; /* create same string type as current but empty */

  if (Data==nullptr)
    return wStrReturn;

  wStrReturn.utfStringDescriptor::_copyFrom(*this);
  if (pLen>=0)
        wStrReturn.strnset(&Data[pOffset],pLen);
    else
        wStrReturn.strset(&Data[pOffset]);
  return wStrReturn;
}//subString




//======================= Base operations==================================================================

/**
 * @brief utfVaryingString::allocateBytes      Allocates pSize bytes to storage space.
 *
 * If utfVaryingString contains data : existing data will NOT be lost
 *
 * Size to be allocated is controled to be a positive number and less than parameter __INVALID_SIZE__
 *
 * If an invalid size is detected, then this is considered as a programmer error and excution is aborted using abort(),
 * with an appropriate message written to stderr.
 *
 * __INVALID_SIZE__ (zerror.h) is defined as the maximum size to be allocated for utfVaryingString.
 *
 *
 * @param[in] pSize size in bytes to allocate
 * @return a pointer to allocated memory as unsigned char*
 */
template <class _Utf>
_Utf*
utfVaryingString<_Utf>::allocateBytes(ssize_t pSize)
{
  uint32_t* wP2=nullptr;
    if ((pSize>__INVALID_SIZE__)||(pSize<0))
    {
        fprintf(stderr,
                "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid size %ld \n",
                _GET_FUNCTION_NAME_,
                pSize);
        abort() ;

    }
     /* allocate/reallocate enough size for requested byte size plus end sign cst_ZMEND */
    if (Data==nullptr)
      Data=zmalloc<_Utf>((size_t)(pSize)+ sizeof(uint32_t));
    else
      {
      wP2=(uint32_t*)&DataByte[ByteSize];
      *wP2=0;/* clear former cst_ZSTRINGEND marker before reallocating memory */
      Data=zrealloc(Data,(size_t)(pSize)+ sizeof(uint32_t));
      }

    ByteSize=pSize;
    UnitCount = ByteSize/ sizeof(_Utf);
    DataByte= (uint8_t*)Data;

    /* set the end of allocated / reallocated memory as cst_ZMEND */
    wP2=(uint32_t*)&DataByte[ByteSize];
    *wP2=cst_ZSTRINGEND;

    return Data;
}//allocateBytes

template <class _Utf>
_Utf*
utfVaryingString<_Utf>::allocateUnits(ssize_t pCharCount)
{
  uint32_t* wP2=nullptr;
    if ((pCharCount>__INVALID_SIZE__)||(pCharCount<0))
    {
        fprintf(stderr,
                "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid character units %ld \n",
                _GET_FUNCTION_NAME_,
                pCharCount);
        abort() ;

    }
 size_t wByteSize= (pCharCount) * sizeof(_Utf);

 /* reallocate enough size for requested units number plus end sign cst_ZSTART */
    if (Data==nullptr)
        Data=zmalloc<_Utf>((size_t)wByteSize + sizeof(uint32_t));
    else
      {
      wP2=(uint32_t*)&DataByte[ByteSize];
      *wP2=0;/* clear former cst_ZSTRINGEND marker before reallocating memory */
      Data=zrealloc(Data,(size_t)wByteSize + sizeof(uint32_t));
      }
    ByteSize=wByteSize;
    UnitCount = pCharCount;
    DataByte= (uint8_t*)Data;

    /* set the end of allocated / reallocated memory as cst_ZSTRINGEND*/
     wP2=(uint32_t*)&DataByte[ByteSize];
    *wP2=cst_ZSTRINGEND;

    return Data;
}//allocateChars


/**
 * @brief utfVaryingString::allocateBZero allocates pSize bytes and sets it to binary zero
 * @param[in] pSize size in bytes to allocate
 * @return a pointer to allocated memory as unsigned char*
 */
template <class _Utf>
_Utf*
utfVaryingString<_Utf>::allocateBytesBZero(ssize_t pSize)
{
    memset(allocateBytes(pSize),0,pSize);
    return Data;
}
template <class _Utf>
_Utf*
utfVaryingString<_Utf>::allocateUnitsBZero(ssize_t pCharCount)
{
    allocateUnits(pCharCount);
    memset(Data,0,ByteSize);
    return Data;
}


/**
 * @brief utfVaryingString::extendBytes
 *  Extends allocated memory space with at least pSize bytes without loosing data :
 *
 *  New size is then computed with Size + pSize (Existing size + extension size)
 *
 *  Existing data is NOT lost and will be copied at beginning of new allocated space
 *
 * @param[in] pSize size to extend
 * @return    pointer to first available byte of extended space as a unsigned char*
 */
template <class _Utf>
_Utf*
utfVaryingString<_Utf>::extendBytes(ssize_t pByteSize)
{
  uint32_t* wP2=nullptr;
    if ((pByteSize>__INVALID_SIZE__)||(pByteSize<0))
    {
        fprintf(stderr,
                "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid size %ld \n",
                _GET_FUNCTION_NAME_,
                pByteSize);
        abort();
    }

    if (Data==nullptr)
    {
        ByteSize= pByteSize;
        UnitCount = ByteSize/sizeof(_Utf);
        /* allocate/reallocate requested space plus uint32_t for cst_ZSTRINGEND */
        Data=zmalloc<_Utf>(ByteSize + sizeof(uint32_t));
        DataByte= (uint8_t*)Data;

        /* set the end of allocated / reallocated memory as cst_ZSTRINGEND */
        wP2=(uint32_t*)&DataByte[ByteSize];
        *wP2=cst_ZSTRINGEND;

        return  Data;
    }// Data==nullptr

    wP2=(uint32_t*)&DataByte[ByteSize];
    *wP2=0;/* clear former cst_ZSTRINGEND marker before reallocating memory */
    /* allocate/reallocate requested space plus uint32_t for cst_ZSTRINGEND */
    Data =zrealloc<_Utf>(Data,ByteSize + pByteSize + sizeof(uint32_t));

    _Utf* wExtentPtr=(_Utf*)(&Data [UnitCount]);
    ByteSize+=pByteSize;
    UnitCount=ByteSize/sizeof(_Utf);
    DataByte= (uint8_t*)Data;

    /* set the end of allocated / reallocated memory as cst_ZSTRINGEND */
    wP2=(uint32_t/* allocate/reallocate requested space plus uint32_t for cst_ZSTRINGEND */*)&DataByte[ByteSize];
    *wP2=cst_ZSTRINGEND;

    return  wExtentPtr;  // returns the first byte of extended memory
}// extendBytes

template <class _Utf>
_Utf*
utfVaryingString<_Utf>::extendUnits(ssize_t pCharCount)
{
  uint32_t* wP2=nullptr;
  if ((pCharCount > __INVALID_SIZE__) || !(pCharCount > 0))
    {
        fprintf(stderr,
                "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid count of characters %ld \n",
                _GET_FUNCTION_NAME_,
                pCharCount);
        abort();
    }
    size_t wByteSize= pCharCount*sizeof(_Utf);

    /* allocate/reallocate requested space plus uint32_t for cst_ZSTRINGEND */
    if (Data==nullptr)
      {
        Data=zmalloc<_Utf>(wByteSize + sizeof(uint32_t));

        ByteSize = wByteSize;
        UnitCount= pCharCount;
        DataByte= (uint8_t*)Data;

        /* set the end of allocated / reallocated memory as cst_ZSTRINGEND */
        wP2=(uint32_t*)&DataByte[ByteSize];
        *wP2=cst_ZSTRINGEND;

        return  Data;
    }// Data==nullptr

    wP2=(uint32_t*)&DataByte[ByteSize];
    *wP2=0;/* clear former cst_ZSTRINGEND marker before reallocating memory */

    /* allocate/reallocate requested space plus uint32_t for cst_ZSTRINGEND */
    Data =zrealloc(Data,ByteSize + wByteSize + sizeof(uint32_t));

    _Utf* wExtentPtr=(_Utf*)(&Data [UnitCount]);

    UnitCount+=pCharCount;
    DataByte= (uint8_t*)Data;

    ByteSize+=wByteSize;

    /* set the end of allocated / reallocated memory as cst_ZMEND */
    wP2=(uint32_t*)&DataByte[ByteSize];
    *wP2=cst_ZSTRINGEND;

    return  wExtentPtr;  // returns the first byte of extended memory
}// extendUnits
/**
 * @brief utfVaryingString::extendBytesBZero extends allocated space with pSize bytes and set the new allocated space to binary 0
 *
 * @param[in] pSize size to extend
 * @return    pointer to first available byte of extended space as _Utf*
 */
template <class _Utf>
_Utf*
utfVaryingString<_Utf>::extendBytesBZero(ssize_t pSize)
{
    if (!(pSize>0))
        return Data;
    ssize_t wOldSize=ByteSize;
    _Utf* wData=extendBytes(pSize);
    memset(wData,0,ByteSize-wOldSize); // ByteSize-wOldSize is necessary because pSize may be adjusted to sizeof(_Utf) by excess
    return wData;
}//extendBytesBZero

/**
 * @brief utfVaryingString<_Utf>::extendCharsBZero extends allocated space with pCharCount character units and set the new allocated space to binary 0
 * @param pCharCount character units number to extend string with
 * @return pointer to first available byte of extended space as _Utf*
 */
template <class _Utf>
_Utf*
utfVaryingString<_Utf>::extendUnitsBZero(ssize_t pCharCount)
{
    if (!(pCharCount>0))
        return Data;

    _Utf* wData=extendUnits(pCharCount);
    memset(wData,0,pCharCount*sizeof(_Utf));
    return wData;
}//extendCharsBZero

template <class _Utf>
_Utf*
utfVaryingString<_Utf>::toNString (_Utf*pString, size_t pCount)  /** sets content to pString. Eventually extends characters allocation to make string fit */
{

    if (!pCount)
                return nullptr;
    utfStrncpy<_Utf>(pString,Data,pCount);
    return pString;
}// toNString

template <class _Utf>
const _Utf*
utfVaryingString<_Utf>::toString () const  /** sets content to pString. Eventually extends characters allocation to make string fit */
{
  return (const _Utf*)Data;
}// toNString

template <class _Utf>
void
utfVaryingString<_Utf>::fromUtf (const _Utf* pIn)  /** sets content to pString. Eventually extends characters allocation to make string fit */
{
  strset(pIn);
  return ;
}// toNString
//===============Special operations=============================================================
/**
 * @brief utfVaryingString::setChar         set string content to pChar starting at pStart byte for pCount characters long
 *      Extends character units allocation if necessary.
 *
   - pStart AND pSize are ommitted, then the whole content (0 length Size) is set to pChar
   - pStart + pSize >  actual Size : Size is adjusted (extend method) to pStart+pSize+1
 *
 * @param pChar  _Utf char to set string with
 * @param pStart starting position counted as character units from beginning
 * @param pCount number of character units to set
 * @return a reference to current utfVaryingString object
 */
template <class _Utf>
utfVaryingString<_Utf> &
utfVaryingString<_Utf>::setChar(const _Utf pChar,size_t pStart,ssize_t pCount)
{
    ssize_t wCount , wStart;

    wStart=pStart;
    if (pStart<0)
            wStart=0;
    wCount=pCount;
    if (pCount==-1)
            wCount=UnitCount-wStart;

    if ((wStart+wCount) >= UnitCount)
                extendUnitsBZero(1+(wStart+pCount)-UnitCount); // make room if necessary : set to binary zero extended portion

    _Utf* wPtr= Data+wStart;
    while (wCount--)
            *wPtr++=pChar;

    return *this;
}//setChar


/**
 * @brief utftemplateString<_Sz,_Utf>::Left return the left part of current utftemplateString on pLen within a string of same type than current.
 * @param pLen
 * @return
 */
template <class _Utf>
utfVaryingString<_Utf>
utfVaryingString<_Utf>::Left (size_t pLen) const
{
  utfVaryingString<_Utf> wU ((utfStringDescriptor*)this); /* create a string with same structure as current */
  wU.strnset(Data,pLen);
  return wU;
}
/**
 * @brief keywordString::Right returns a keywordString with a content of length pLen containing
 * the pLen last characters (on the right) of keywordString actual content.
 * if pLen is greater than actual content size, then it takes the whole content from start.
 * @param pLen
 * @return
 */
template <class _Utf>
utfVaryingString<_Utf>
utfVaryingString<_Utf>::Right (size_t pLen) const
{
  utfVaryingString<_Utf> wU ((utfStringDescriptor*)this); /* create a string with same structure as current */
  int wStart= utfStrlen<_Utf>(Data)-pLen;
  if (wStart<0)
    wStart=0;
  wU.strnset(&Data[wStart],pLen);
  return wU;
}

//============= Import Export to URF format=================================

//include <ztoolset/ztypes.h>
#include <ztoolset/zdatabuffer.h>

template <class _Utf>
/**
 * @brief _exportURF export current varying utf string into an URF format.
 *  Container used is a ZDataBuffer that may be provided by calling procedure.
 *  Universal Record Format is ALWAYS big endian regardless what platform is used.
 *
 *  URF format for an utf Varying string.
 *
 *  URF header depends on the type of object given by the first header information : ZTypeBase (ZType_type)
 *  for an utfVaryingString, header is as follows
 *
 *  URF data is converted to Universal format.
 *  Universal format is big endian with special encoding for atomic data that allows to sort on the whole data in a coherent, simple maner.
 *
 *header :
 *   - ZTypeBase : ZType_utf8VaryingString, ZType_utf16VaryingString,... Gives the Character Unit size with ZType_AtomicMask
 *   - uint64_t number of character units effectively contained within the string.
 *
 *
 *  see <ZType_Type vs Header sizes> in file <zindexedfiles/znaturalfromurf.cpp>
 *data :
 *  a suite of _Utf character unit sequense up to number of characters units.
 *  _Utf character units are converted to big endian if necessary.
 *
 * limits :
 *
 *  Maximum size is in bytes :      	18446744073709551615 (2^64-1)
 *
 * @param pURF  a ZDataBuffer that will contain URF data in return.
 * @return
 */
ZDataBuffer*
utfVaryingString<_Utf>::_exportURF(ZDataBuffer *pURF) const
{
unsigned char* wURF_Ptr;
    URF_Varying_Size_type wByteSize=(URF_Varying_Size_type)ByteSize;

    wURF_Ptr=pURF->allocateBZero(wByteSize+sizeof(ZTypeBase)+sizeof(URF_Varying_Size_type));

    wURF_Ptr=setURFBufferValue<ZTypeBase>(wURF_Ptr,ZType);
    wURF_Ptr=setURFBufferValue<URF_Varying_Size_type>(wURF_Ptr,wByteSize);

   utfSetReverse<_Utf>((_Utf*)wURF_Ptr,
                           (_Utf*) DataByte,
                           (const size_t)getUnitCount());// character units count

   return pURF;
}//_exportURF



/**
 * @brief utfVaryingString<_Utf>::_importURF imports URF data from pURF and feeds current utfVaryingString object.
 * @param pUniversal a pointer to URF header data
 * @return  ZStatus ZS_SUCCESS if everything went well - ZS_INVTYPE is URF data has not the appropriate ZType_type.
 */

template <class _Utf>
size_t utfVaryingString<_Utf>::_importURF(const unsigned char* &pURF)
{
ZTypeBase               wType=ZType_Nothing;
URF_Varying_Size_type   wUniversalSize=0;
_Utf*                   wPtrOut=nullptr;
size_t                  wUnits=0;
size_t                  wTotalSize=0;

  errno=0;
//    pURF=getURFBufferValue<ZTypeBase>(&wType,pURF);

    wTotalSize += _importAtomic<ZTypeBase>(wType,pURF);

/* ZType control */
  if (wType!= getZType())
      {
      fprintf (stderr,"utfVaryingString::_importURF-E-IVTYP Invalid URF type <%s> while expecting <%s>.\n",
                          decode_ZType(wType),
                          decode_ZType(getZType()));
      errno=EPERM;
      return 0;
      }

  wTotalSize += _importAtomic<URF_Varying_Size_type>(wUniversalSize,pURF);
//    pURF=getURFBufferValue<URF_Varying_Size_type>(&wUniversalSize,pURF);


  wPtrOut=allocateBytes(wUniversalSize+sizeof(_Utf));
  _Utf* wPtrIn=(_Utf*)pURF;
  wUnits=wUniversalSize/sizeof(_Utf);
  int wi=0;
  if (sizeof(_Utf)==1)
    while (wi++ < wUnits)
      *wPtrOut++=*wPtrIn++;
  else
    while (wi++ < wUnits)
      *wPtrOut++=reverseByteOrder_Conditional<_Utf>(*wPtrIn++);

  *wPtrOut=0;
  wTotalSize += wUniversalSize;

  return wTotalSize;
}// _importURF


#pragma pack(push)
#pragma pack(1)
struct UVF_Header_S
{
  uint8_t UnitSize=0;
  UVF_Size_type UnitCount=0;
};
#pragma pack(pop)


template <class _Utf>
/**
 * @brief _exportVUniversal  Exports a string to a Universal Varying Format
 *  Universal Varying  Format stores string data into a varying length string container excluding '\0' character terminator
 *  led by
 *   - uint8_t : char unit size
 *   - UVF_Size_type : number of character units of the string.
 * @return a ZDataBuffer with string content in Varying Universal Format set to big endian
 */
ZDataBuffer
utfVaryingString<_Utf>::_exportUVF() const
{
  ZDataBuffer wUVF;
  return _exportAppendUVF(wUVF);
}// _exportUVF



template <class _Utf>
ZDataBuffer&
utfVaryingString<_Utf>::_exportAppendUVF(ZDataBuffer& pUVF) const
{
  UVF_Header_S wH;
  wH.UnitSize=sizeof(_Utf);
  wH.UnitCount=0;
  /* Count effective char units excluding (_Utf)'\0' mark starting from beginning*/
  if (isEmpty())
    {
    return pUVF.appendData(&wH,sizeof(UVF_Header_S));
    }

  UVF_Size_type wUnitCount=0;
  while ((Data[wUnitCount]!=0)&&(wUnitCount < UnitCount))
        wUnitCount++;

  size_t wByteLen=wUnitCount*sizeof(_Utf); // get number of bytes out of this

  /* allocate additional storage for export */
  unsigned char* wPtrTarg = pUVF.extend((ssize_t)wByteLen+sizeof(UVF_Header_S));

  /* set export data with char unit size */
//  *wPtrTarg = (uint8_t)sizeof (_Utf);
//  wPtrTarg++;
  /* prepare and set unit count to export data */
//  UVF_Size_type wUnitCount_Export=reverseByteOrder_Conditional<UVF_Size_type>(wUnitCount);
//  memmove(wPtrTarg,&wUnitCount_Export,sizeof(UVF_Size_type));
//  wPtrTarg += sizeof(UVF_Size_type);

  wH.UnitCount=reverseByteOrder_Conditional<UVF_Size_type>(wUnitCount);
  memmove(wPtrTarg,&wH,sizeof(UVF_Header_S));
  wPtrTarg += sizeof(UVF_Header_S);


  /* export char units : each char unit must be reversed if necessary (big /little endian) */

  _Utf* wPtrOut=(_Utf*)(wPtrTarg);
  _Utf* wPtrIn=Data;

  if (isEmpty())
      return pUVF;

  if (sizeof (_Utf)==1)
    while ((*wPtrIn)&&(wUnitCount--))
      *wPtrOut++=*wPtrIn++;
  else
    while ((*wPtrIn)&&(wUnitCount--))
      *wPtrOut++=reverseByteOrder_Conditional<_Utf>(*wPtrIn++);

  return pUVF;
}// _exportUVF


template <class _Utf>
ZStatus
utfVaryingString<_Utf>::_exportUVFPtr(unsigned char*pPtrOut,UVF_Size_type pSizeMax) const
{
  if (pSizeMax <= sizeof(UVF_Header_S))
    return ZS_INVSIZE;
  if (pPtrOut==nullptr)
    return ZS_NULLPTR;

  ZStatus wSt=ZS_SUCCESS;
  UVF_Header_S wH;
  wH.UnitSize=sizeof(_Utf);
  wH.UnitCount=0;

  size_t wSizeMax=pSizeMax - sizeof(UVF_Header_S);

   /* Count effective char units excluding (_Utf)'\0' mark starting from beginning*/

  while ((Data[wH.UnitCount]!=0)&&(wH.UnitCount < UnitCount))
    wH.UnitCount++;

  if (pSizeMax < (wH.UnitSize * wH.UnitCount) + sizeof(UVF_Header_S))
    {
    wH.UnitCount = (pSizeMax-sizeof(UVF_Header_S)) / wH.UnitSize;
    wSt=ZS_TRUNCATED;                       /* not an error : set the warning on */
    }

  UVF_Size_type wUnitCount=wH.UnitCount;
  wH.UnitCount = reverseByteOrder_Conditional<UVF_Size_type>(wH.UnitCount) ;  /* serialize : unitsize is byte */

  memmove(pPtrOut,&wH,sizeof(UVF_Header_S));
  pPtrOut +=   sizeof(UVF_Header_S);

  if (isEmpty())
    return wSt;

  _Utf* wPtrOut=(_Utf*)(pPtrOut);


  _Utf* wPtrIn=Data;

  if (sizeof (_Utf)==1)
    while ((*wPtrIn)&&(wUnitCount--))
      *wPtrOut++=*wPtrIn++;
  else
    while ((*wPtrIn)&&(wUnitCount--))
      *wPtrOut++=reverseByteOrder_Conditional<_Utf>(*wPtrIn++);

  return wSt;
}// _exportUVF

template <class _Utf>
/**
 * @brief _getexportUVFSize() compute the requested export size IN BYTES for current string
 */
UVF_Size_type
utfVaryingString<_Utf>::_getexportUVFSize() const
{
  if (isEmpty())
    return sizeof(UVF_Header_S);
  /* Count effective char units excluding (_Utf)'\0' mark starting from beginning*/
  UVF_Size_type wUnitCount = 0;

  while ((Data[wUnitCount]!=0)&&(wUnitCount < UnitCount))
      wUnitCount++;


  UVF_Size_type wByteLen=(wUnitCount*sizeof(_Utf)) +sizeof(UVF_Header_S);
  return wByteLen;

}//_getimportUVFSize

/** gets utf format name from char unit size */
const char* getUnitFormat(uint8_t pSize);



template <class _Utf>
/**
 * @brief _importUVF Import string from Varying Universal Format
 *  Varying Universal Format stores string data into a varying length string container excluding '\0' character terminator
 *  led by a uint16_t mentionning the number of characters of the string that follows.
 * Important : imported string format (utf-xx) must be the same as current string
 * @param pUniversalPtr pointer to Varying Universal formatted data header
 * @return total size IN BYTES of consumed bytes in pUniversalPtr buffer (Overall size of string in UVF)
 */
size_t
utfVaryingString<_Utf>::_importUVF(const unsigned char *&pPtrIn)
{
  const unsigned char* wPtrSrc=pPtrIn;

  /* get UVF header */
  UVF_Header_S wH;
  memmove(&wH,pPtrIn,sizeof(UVF_Header_S));
  wH.UnitCount = reverseByteOrder_Conditional<UVF_Size_type>(wH.UnitCount) ;

  pPtrIn +=sizeof(UVF_Header_S);

  /* first get and control char unit size */
  if (wH.UnitSize!=sizeof(_Utf))
    {
    fprintf(stderr,"_importUVF-E-IVUSIZE Imported string format <%s> does not correspond to current string format <%s>",
        getUnitFormat(wH.UnitSize),
        getUnitFormat(sizeof(_Utf)));
    return 0;
    }
  /* get char unit count to load excluding (_Utf)'\0' mark */

  /* allocate size of string in char units */
  allocateUnits(wH.UnitCount + 1); // remember that '\0' is not exported

  /* import string per char unit */

  _Utf* wPtrOut=Data ;
//  _Utf* wPtrIn=(_Utf*)(pPtrIn);
  _Utf* wPtrEnd = (_Utf*)&pPtrIn[wH.UnitCount];
  int wCount = wH.UnitCount;
  if ((wH.UnitSize==1)||(!is_little_endian()))
    while (wCount-- && (pPtrIn < wPtrEnd))
      *wPtrOut++ = *pPtrIn++;
  else
    while (wCount-- && (pPtrIn < wPtrEnd))
      *wPtrOut++ = reverseByteOrder_Conditional<_Utf>(*pPtrIn++);

  *wPtrOut = 0;  /* add string termination sign */
//  pPtrIn =(const unsigned char*)wPtrIn;
//  pPtrIn++; /* because of 1 byte of security */
  return pPtrIn-wPtrSrc;
}// _importUVF
/**
 * @brief _getimportUVFSize() returns total size in byte of data to import, including header.
 */
template <class _Utf>
UVF_Size_type
utfVaryingString<_Utf>::_getimportUVFSize(const unsigned char *pPtrIn)
{
  /* get UVF header */
  UVF_Header_S wH;
  memmove(&wH,pPtrIn,sizeof(UVF_Header_S));

  pPtrIn +=sizeof(UVF_Header_S);

  /* get char unit count to load excluding (_Utf)'\0' mark */
  wH.UnitCount = reverseByteOrder_Conditional<UVF_Size_type>(wH.UnitCount) ;
  wH.UnitCount = wH.UnitCount * wH.UnitSize;

  return wH.UnitCount + (UVF_Size_type)sizeof(UVF_Header_S);
}//_getimportUVFSize



template <class _Utf>
ZStatus
utfVaryingString<_Utf>::getUniversalFromURF(ZTypeBase pType, const unsigned char* pURFDataPtr, ZDataBuffer& pUniversal,const unsigned char **pURFDataPtrOut)
{

 URF_Varying_Size_type wEffectiveUSize ;
 ZTypeBase wType;
 const unsigned char* wURFDataPtr = pURFDataPtr;

     memmove(&wType,wURFDataPtr,sizeof(ZTypeBase));
     wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
     wURFDataPtr += sizeof (ZTypeBase);
     if (wType!=pType)
         {
         fprintf (stderr,
                  "%s>> Error invalid URF data type <%X> <%s> while expecting <%s> ",
                  _GET_FUNCTION_NAME_,
                  wType,
                  decode_ZType(wType),
                  decode_ZType(pType));
         return  ZS_INVTYPE;
         }

    memmove (&wEffectiveUSize,wURFDataPtr,sizeof(URF_Varying_Size_type));        // first is URF byte size (excluding URF header size)
    wEffectiveUSize=reverseByteOrder_Conditional<URF_Varying_Size_type>(wEffectiveUSize);
    wURFDataPtr += sizeof (URF_Varying_Size_type);

    pUniversal.allocateBZero((wEffectiveUSize)); // string must have characters count allocated

    memmove(pUniversal.Data,wURFDataPtr,wEffectiveUSize);
    if (pURFDataPtrOut)
    {
      *pURFDataPtrOut = wURFDataPtr + wEffectiveUSize;
    }

    return  ZS_SUCCESS;
}//getUniversalFromURF



/**
 * @brief utfVaryingString::bsearch   Binary search
 *
 *  searches in a binary mode for a Key contained in pKey with pKeySize length
 *  in utfVaryingString::Data Content of Size length.
 *
 *  Returns the offset of pKey if it has been found. -1 if nothing has been found
 *
 * @param[in] pKey   a void* pointer to byte sequence to search for
 * @param[in] pKeySize  length of byte sequence
 * @param[in] pOffset   starting offset for search in utfVaryingString::Data
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */
template <class _Utf>
ssize_t
utfVaryingString<_Utf>::bsearch (void *pKey,
                      const size_t pKeyByteSize,
                      const size_t pOffset)
{
    long widx = 0;
    long wistart = -1;
    uint8_t* wKey = (uint8_t *)pKey;
    long wHighIdx = ByteSize;

    for (long wi=pOffset; wi <wHighIdx ;wi++)
    {
        if (wKey[widx]==DataByte[wi])
        {
            if (wistart==-1)
                wistart=wi;
            widx++;
            if (widx==pKeyByteSize)
            {
                return (wistart);
            }
            continue;
        }// if wKey
        widx=0;
        wistart=-1;
    }
    return (wistart) ;
}//bsearch
/**
 * @brief utfVaryingString::bsearch   Binary search
 *
 *  searches in a binary mode for a Key contained in pKey with pKeySize length
 *  in utfVaryingString::Data Content of Size length.
 *
 *  Returns the offset of pKey if it has been found. -1 if nothing has been found
 *
 * @param[in] pKey      utfVaryingString containing byte sequence to search for, with its length as Size of utfVaryingString
 * @param[in] pOffset   starting offset for search in utfVaryingString::Data
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */
template <class _Utf>
ssize_t
utfVaryingString<_Utf>::bsearch (utfVaryingString<_Utf> &pKey,const size_t pOffset)

{
    return (bsearch(pKey.DataByte,pKey.ByteSize,pOffset)) ;
}//bsearch

/**
 * @brief utfVaryingString::bsearchCaseRegardless binary search BUT compares CASE REGARDLESS.
 * Returns an offset in character units from pOffset within the whole utfVaryingString considered as a bulk of characters units.
 * @param pKey a suit of _Utf characters to find
 * @param pSize Length of pKey in character units
 * @param pOffset offset to start search from in character units
 * @return an offset from pOffset in character units pointing to the start of found content if found or -1 if not found
 */
template <class _Utf>
ssize_t
utfVaryingString<_Utf>::bsearchCaseRegardless (_Utf *pKey, ssize_t pSize, ssize_t pOffset)
{
    long widx = 0;
    long wistart = -1;
    _Utf *wKey = (_Utf *)pKey;
    long wHighIdx = this->ByteSize ;
    _Utf ToCompareFromKey ;
    _Utf ToCompareFromArray;

    for (ssize_t wi=pOffset; wi < UnitCount ;wi++)
    {
        ToCompareFromArray = utfUpper<_Utf>(Data[wi]);
        ToCompareFromKey = utfUpper<_Utf>(wKey[widx]);
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

template <class _Utf>
ssize_t
utfVaryingString<_Utf>::bsearchCaseRegardless (utfVaryingString &pKey,ssize_t pOffset)
{
    return (bsearchCaseRegardless(pKey.Data,pKey.UnitCount,pOffset));
}
template <class _Utf>
ssize_t
utfVaryingString<_Utf>::bstartwithCaseRegardless (void *pKey, ssize_t pSize, ssize_t pOffset)
{
    _Utf *wKey = (_Utf *)pKey;
    long widx=0;
    _Utf ToCompareFromKey ;
    _Utf ToCompareFromArray;

    if (this->UnitCount<pSize)
                return -1;
    ssize_t wi=pOffset;
    while (wi<pSize)
    {
        ToCompareFromArray = utfUpper<_Utf>(Data[wi]);
        ToCompareFromKey = utfUpper<_Utf>(wKey[widx]);

        if (ToCompareFromKey!=ToCompareFromArray)
            return -1;
        wi++;
        widx++;
    }
    return (0) ; // OK
}//bstartwithCaseRegardless

template <class _Utf>
ssize_t
utfVaryingString<_Utf>::bstartwithCaseRegardless (utfVaryingString pKey,size_t pOffset)
{
    return (bstartwithCaseRegardless(pKey.Data,pKey.ByteSize,pOffset));
}

/**
 * @brief breverseSearch   Binary REVERSE search regardless case
 *
 *  Reverse search in a binary mode for a Key contained in pKey with pKeySize length
 *  in utfVaryingString::Data Content of Size length.
 *
 *  Returns the offset of pKey if it has been found. -1 if nothing has been found
 *
 *  The returned offset is the STARTING byte position of pKey in utfVaryingString content
 *
 * @param[in] pKey   a void* pointer to byte sequence to search for
 * @param[in] pKeySize  length of byte sequense
 * @param[in] pOffset   starting offset for reverse search in Buffer
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */
template <class _Utf>
ssize_t
utfVaryingString<_Utf>::breverseSearchCaseRegardless(void *pKey,
                                          ssize_t pKeySize,
                                          ssize_t pOffset)
{
    long widx = pKeySize-1;
    _Utf *wKey = (char *)pKey;
    _Utf ToCompareFromArray , ToCompareFromKey;
    //long wHighIdx = Size;

    if (pOffset<0)
        pOffset = UnitCount;
    if (pOffset>UnitCount)
        pOffset = UnitCount;

    for (long wi=pOffset; wi >= 0 ;wi--)
    {
        ToCompareFromArray = utfUpper<_Utf>(Data[wi]);
        ToCompareFromKey = utfUpper<_Utf>(wKey[widx]);

        if (ToCompareFromKey==ToCompareFromArray)
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
 *  in utfVaryingString::Data Content of Size length.
 *
 *  Returns the offset of pKey if it has been found. -1 if nothing has been found
 *
 *  The returned offset is the STARTING byte position of pKey in utfVaryingString content
 *
 * @param[in] pKey      utfVaryingString containing byte sequence to search for, with its length as Size of utfVaryingString
 * @param[in] pOffset   starting offset for search in utfVaryingString::Data
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */
template <class _Utf>
ssize_t
utfVaryingString<_Utf>::breverseSearchCaseRegardless (utfVaryingString<_Utf> &pKey,
                                           const ssize_t pOffset)
{

    return (breverseSearchCaseRegardless(pKey.DataByte,pKey.ByteSize,pOffset)) ;
}//breverseSearchCaseRegardless
/**
 * @brief breverseSearch   Binary REVERSE search  Reverse search in a binary mode for a Key contained in pKey with pKeySize length
 *  in utfVaryingString::Data Content of Size length.
 *
 * This means finds the first occurrence of pKey on pKeySize length starting by the end of utfVaryingString::Data content (Size) if pOffset is -1
 * or pOffset if pOffset > 0.
 *
 *  Returns
 *  - the offset of pKey if it has been found.
 *  - (-1) if nothing has been found
 *
 *  The returned offset is the STARTING byte position of pKey in utfVaryingString content from the end of utfVaryingString content.
 *
 * @param[in] pKey   a void* pointer to byte sequence to search for
 * @param[in] pKeySize  length of byte sequense
 * @param[in] pOffset   starting offset for reverse search in Buffer
 * @return the offset of pKey if it has been found. -1 if nothing has been found
 */
template <class _Utf>
ssize_t
utfVaryingString<_Utf>::breverseSearch (void *pKey,
                             ssize_t pKeySize,
                             ssize_t pOffset)
{
    long widx = pKeySize-1;
    _Utf *wKey = (_Utf *)pKey;
    //long wHighIdx = Size;

    if (pOffset<0)
        pOffset = UnitCount;
    if (pOffset>UnitCount)
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

//--------utfVaryingString content tests-----------------------

/**
 * @brief utfVaryingString::isEqual Tests whether the content given by void* on pSize length exists in utfVaryingString::Data
 * If pSize is ommitted (<1) then test is made on size of utfVaryingString content.
 * @param[in] pCompare void* pointer on data to compare
 * @param[in] pSize Length to test on. if <1 then size of utfVaryingString content is taken
 * @return true if equal, false if not
 */
template <class _Utf>
bool
utfVaryingString<_Utf>::isEqual (const _Utf *pCompare,long pSize)
{
    if(pSize<1)
            return(compare(pCompare)==0);
        else
            return(ncompare(pCompare,pSize)==0);
}

template <class _Utf>
bool
utfVaryingString<_Utf>::isEqualCase (const _Utf *pCompare)
{

  _Utf* wPtr1=Data;
  const _Utf* wPtr2=pCompare;

  while (*wPtr1 && *wPtr2 && (utfUpper(*wPtr1++)==utfUpper(*wPtr2++))) ;

  if ((*wPtr1==0) && (*wPtr2==0))
    return true;
  return false;
}

template <class _Utf>
bool
utfVaryingString<_Utf>::isEqualCase (const utfVaryingString<_Utf>& pCompare)
{

  _Utf* wPtr1=Data;
  _Utf* wPtr2=pCompare.Data;

  while (*wPtr1 && *wPtr2 && (utfUpper(*wPtr1++)==utfUpper(*wPtr2++))) ;

  if ((*wPtr1==0) && (*wPtr2==0))
    return true;
  return false;
}

template <class _Utf>
bool
utfVaryingString<_Utf>::isEqual (const utfVaryingString<_Utf>& pCompare)
{
    return(ncompare(pCompare.Data,pCompare.ByteSize)==0);
}
template <class _Utf>
bool
utfVaryingString<_Utf>::isGreater (const utfVaryingString<_Utf>& pCompare)
{
    return(ncompare(pCompare.Data,pCompare.ByteSize)>0);
}
template <class _Utf>
bool
utfVaryingString<_Utf>::isLess (const utfVaryingString<_Utf>& pCompare)
{
    return(ncompare(pCompare.Data,pCompare.ByteSize)<0);
}
/**
 * @brief utfVaryingString::isGreater Tests whether the content given by void* on pSize length is greater than the same segment utfVaryingString::Data
 * If pSize is ommitted (<1) then test is made on size of utfVaryingString content.
 * @param[in] pCompare void* pointer on data to compare
 * @param[in] pSize Length to test on. if <1 then size of utfVaryingString content is taken
 * @return true if greater, false if not
 */
template <class _Utf>
bool
utfVaryingString<_Utf>::isGreater (const _Utf *pCompare,long pSize)
{
    if(pSize<1)
            return(compare(pCompare)>0);
        else
            return(ncompare(pCompare,pSize)>0);
}
/**
 * @brief utfVaryingString::isLess Tests whether the content given by void* on pSize length is less than the same segment utfVaryingString::Data
 * If pSize is ommitted (<1) then test is made on size of utfVaryingString content.
 * @param[in] pCompare void* pointer on data to compare
 * @param[in] pSize Length to test on. if <1 then size of utfVaryingString content is taken
 * @return true if is less, false if not
 */
template <class _Utf>
bool
utfVaryingString<_Utf>::isLess (const _Utf *pCompare,long pSize)
{
    if(pSize<1)
            return(compare(pCompare)<0);
        else
            return(ncompare(pCompare,pSize)<0);
}



/**
 * @brief utfVaryingString::dumpHexa dumps a segment of data of current _Utf string starting at pOffset on pCount length
 *   and returns both ascii and hexadecimal values (utf8_t* strings).
 *
 *  Returns the current utfVaryingString content using 2 utf8_t varying strings :
 *   - one for printable hexadecimal result,
 *   - the other is printable ascii result.
 *
 *  Content dump is done from pOffset on pCount length expressed as character units
 *
 * @param[in] pOffset   Offset in character units (starting from 0) of string content to be printed
 * @param[in] pCount    Size in character units of the content to dump
 * @param[out] pLineHexa a ZDataBuffer with resulting content expressed in hexadecimal
 * @param[out] pLineChar a ZDataBuffer with resulting content in US-ascii
 * @return
 */
template <class _Utf>
void
utfVaryingString<_Utf>::dumpHexa (const size_t pOffset,
                                  const size_t pCount,
                                  ZDataBuffer &pLineHexa,
                                  ZDataBuffer &pLineChar)
{
ZDataBuffer wZDB;
    wZDB.setData(Data,ByteSize);
    wZDB.dumpHexa(pOffset*sizeof(_Utf),pCount*sizeof(_Utf),pLineHexa,pLineChar);
    return ;
} // dumpHexa

template <class _Utf>
inline int
utfVaryingString<_Utf>::compare(const _Utf* pString2) const
{

  if (Data==pString2)
      return 0;
  if (Data==nullptr)
      return -1;
  if (pString2==nullptr)
      return 1;

    const _Utf *s1 = (const _Utf *) Data;
    const _Utf *s2 = (const _Utf *) pString2;
    while ((*s1 == *s2 )&&(*s1)&&(*s2))
        {
        s1++;
        s2++;
        }
    return *s1 - *s2;

}/** corresponds to strcmp */

template <class _Utf>
inline int
utfVaryingString<_Utf>::ncompare(const _Utf* pString2,size_t pCount)
{
    const _Utf* wPtr=Data;
    if (Data==pString2)
                return 0;
    if (pString2==nullptr)
                return 1;
    if (pCount<1)
                pCount=0;
    int wCount = pCount>0?0:-1;
    int wComp = (int)(*wPtr - *pString2);
    while ((!wComp)&&(*wPtr)&&(*pString2)&&(wCount<pCount))
    {
    wPtr++;
    pString2++;
    if (pCount)
                wCount++;
    wComp=(int)(*wPtr - *pString2);
    }
    if (wComp)
            return wComp;
    if (wCount==pCount)
            return wComp;
    // up to here wComp==0 (equality)
    //    test string lengths
    if (*wPtr==0) // string 1 exhausted
            {
            if (*pString2==0)   // so is string2
                        return 0;   // perfect equality
            return -1; // string 2 is greater than string1 (because more characters)
            }
    // up to here wComp==0 but string1 is not exhausted
    //
    return 1; // string 1 is longer
}/** corresponds to strncmp */


/**
 * @brief locate  locates a substring pString in utftemplateString and returns its offset from beginning as a ssize_t value.
 * returns a negative value if substring has not been found.
 * @param pString a const char* string containing the substring to find
 * @return offset (starting from 0) of the substring in main string if substring is found, a negative value if not.
 */
template<class _Utf>
ssize_t utfVaryingString<_Utf>::locate(const _Utf *pString) const
{
    if (!pString)
        return -1;
    ssize_t wRet = strstr(pString) - Data;
    if (wRet < 0)
        return -1;
    return wRet;
}

template<class _Utf>
ssize_t utfVaryingString<_Utf>::locate(const _Utf *pString, size_t pOffset) const
{
    if (!pString)
        return -1;
    ssize_t wRet = strstr(pString,pOffset) - Data;
    if (wRet < 0)
        return -1;
    return wRet;
}

/**
 * @brief locate  locates a substring pString in utftemplateString and returns its offset from beginning as a ssize_t value.
 * Returns a negative value if substring has not been found.
 * @param pString a utftemplateString of same type containing the substring to find
 * @return offset (starting from 0) of the substring in main string if substring is found, a negative value if not.
 */
template<class _Utf>
ssize_t utfVaryingString<_Utf>::locate(const utfVaryingString<_Utf> &pString) const
{
    if (pString.isEmpty())
        return 0;
    ssize_t wRet = strstr(pString.Data) - Data;
    if (wRet < 0)
        return -1;
    return wRet;
}
template <class _Utf>
void utfVaryingString<_Utf>::Dump(const int pColumn, ssize_t pLimit, FILE* pOutput)
{

  ZDataBuffer wUtfZDB;
  wUtfZDB.setData((const char*)Data,ByteSize);
  wUtfZDB.Dump(pColumn,pLimit,pOutput);
}
/*
template <class _Utf>
void utfVaryingString<_Utf>::Dump_1(const int pColumn, ssize_t pLimit, FILE* pOutput)
{
  ZDataBuffer wRulerAscii;
  ZDataBuffer wRulerHexa;

  int wColumn=rulerSetup (wRulerHexa, wRulerAscii,pColumn);


  ZDataBuffer wLineChar;
  ZDataBuffer wLineHexa;

  if (!ByteSize)
  {
    fprintf(pOutput,"<empty>\n");
    return;
  }
  if (!Data)
  {
    fprintf(pOutput,"<null>\n");
    return;
  }
  size_t wLimit;
  if ((pLimit > 0) && ((size_t)pLimit > ByteSize))
    wLimit = (size_t)pLimit;
  else
    wLimit = ByteSize;

  fprintf (pOutput,
      "Offset  %s  %s\n",
      wRulerHexa.DataChar,
      wRulerAscii.DataChar);

  size_t wOffset=0;
  int wL=0;
  while ((wOffset+(size_t)pColumn) < wLimit)
    {
    dumpHexa(wOffset,pColumn,wLineHexa,wLineChar);
    fprintf(pOutput,"%6d |%s |%s|\n",wL,wLineHexa.DataChar,wLineChar.DataChar);
    wL+=pColumn;
    wOffset+=pColumn;
    }

  if (wLimit < ByteSize)
  {
    fprintf (pOutput," <%ld> bytes more not dumped.\n",ByteSize-pLimit);
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
}//Dump
*/

template <class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::startsCaseWith Test if String starts with pString regarless case after having trimmed out character from pSet
 * @param[in] pString substring to search
 * @param[in] pSet set of characters to consider as being eliminated (trimmed)(defaulted to __DEFAULTDELIMITERSET__)
 * @return a bool set to true if current string starts with given string pString
 */
bool
utfVaryingString<_Utf>::startsCaseWith(const _Utf* pString)
{
  return !utfStrncasecmp<_Utf>(Data,pString,utfStrlen<_Utf>(pString));
}
/**
 * @brief utftemplateString::startsWith Test if String starts with pString after having trimmed out character from pSet
 * @param[in] pString substring to search
 * @param[in] pSet  character set to trim off (defaulted to __DEFAULTDELIMITERSET__)
 * @return true if found, false if not found
 */
template <class _Utf>
bool
utfVaryingString<_Utf>::startsWith(const _Utf* pString) const
{
  return !utfStrncmp<_Utf>(Data,pString,utfStrlen<_Utf>(pString));
}

template <class _Utf>
utfVaryingString<_Utf>
    utfVaryingString<_Utf>::toUpper(void) const
{
  utfVaryingString<_Utf> wRet(*this);
  for (size_t wi=0;wi<wRet.UnitCount;wi++)
    wRet.Data[wi]=utfUpper<_Utf>(wRet.Data[wi]);

  return wRet;
}
template <class _Utf>
utfVaryingString<_Utf>
utfVaryingString<_Utf>::toLower(void) const
{
  utfVaryingString<_Utf> wRet(*this);
  for (size_t wi=0;wi<wRet.UnitCount;wi++)
    wRet.Data[wi]=utfLower<_Utf>(wRet.Data[wi]);

  return wRet;
}
template <class _Utf>
utfVaryingString<_Utf>
utfVaryingString<_Utf>::dropAccute(void) const
{
  utfVaryingString<_Utf> wRet(*this);
  for (size_t wi=0;wi<wRet.UnitCount;wi++)
    wRet.Data[wi]=utfDropAccute<_Utf>(wRet.Data[wi]);

  return wRet;
}

//---------End utfVaryingString--------------------------

#endif // UTFVTEMPLATESTRING_H
