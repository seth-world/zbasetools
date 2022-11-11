#ifndef UTFTEMPLATESTRING_H
#define UTFTEMPLATESTRING_H

#include <zconfig.h>

/** gets utf format name from char unit size (instantiated in ufvtemplatestring.cpp) */
const char* getUnitFormat(uint8_t pSize);

/** @file utfemplatestring.h  In this file is defined a template FIXED string container that is used as base for various sizes of strings.
 *  some further derived structs might have additional functions (like uriString, for manipulating files and directories)
 *
 * This data may be directly moved as a field for a record.
 *
 * As we are using a template,
 * - detailed methods for the template have to be in the header file,
 * - all derived class definitions have to be in the same header file.
 *
 * utftemplateString Template is hidden below derived classes definition.
 * Each fixed string below has a constant that corresponds to it representing maximum size of its data (plus one).
 *
 * see utffixedstring.h
 * see zutfstring.h
 */

#include <iostream>
#include <stdlib.h>

//#include <openssl/sha.h>  // required for checksum

#include <ztoolset/zmem.h>  // for zfree routine
#include <ztoolset/zerror.h>
#include <ztoolset/zdatabuffer.h>
#include <string.h>
#include <ztoolset/zlimit.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zatomicconvert.h>
#include <ztoolset/zcharset.h>

#include <ztoolset/utfutils.h>

#include <ztoolset/utfsprintf.h>

#include <ztoolset/utfstringcommon.h>

#include <zcrypt/zcrypt.h>
#include <zcrypt/checksum.h>
#include <zcrypt/md5.h>

//#include <ztoolset/zfunctions.h>  // cannot put include zfunctions.h there

char * findLastNotChar(char *pContent,char pChar);
bool isTerminatedBy(const char *pContent,char pChar);
void conditionalNL(char *pString) ;

const char * _firstNotinSet (const char*pString,const char *pSet);
const char * _firstinSet (const char*pString,const char *pSet);
const char * _firstNotinSet (const char*pString,const char *pSet);
const char * _lastinSet(const char *pStr, const char *pSet);

char * _LTrim (char*pString, const char *pSet);
char * _RTrim (char*pString, const char *pSet);
char * _Trim (char*pString, const char *pSet);

const char * _strchrReverse(const char *pStr,const char pChar);
char * _toUpper(char *pStr,char *pOutStr);
char * _expurgeSet(const char *pString, const char *pSet);
char * _expurgeString(const char *pString, const char *pSubString);

char *_printStdStringField(void *pField, bool IsPointer, char *pBuf);
namespace zbs
{
char *_printStdStringField(void *pField, bool IsPointer, char *pBuf);
}
const char* _Zsprintf(char *pBuf,const char* pFormat,...);

//namespace zbs {   // no namespace using generic namespace

#ifndef __DEFAULTDELIMITERSET__
#define __DEFAULTDELIMITERSET__  " \t\n\r"

#endif // __DEFAULTDELIMITERSET__




//=================Base Template utftemplateString SECTION=============================

/**
 *  utftemplateString : creates a base fixed sized string container with a char unit equal to _Utf
 * 
 *  This base template contains all the common routines to process and encode/decode fixed strings
 */
template <size_t _Sz=255,class _Utf=utf8_t>
class utftemplateString : public utfStringDescriptor
{
protected:
    utftemplateString& _copyFrom(const utftemplateString& pIn)
    {
        utfStringDescriptor::_copyFrom(pIn);
        _Utf *wPtr = content;
        const _Utf *wPtrIn = pIn.content;
        size_t wi = 0;
        while (wPtrIn[wi] && (wi < _Sz)) {
            wPtr[wi] = wPtrIn[wi];
            wi++;
        }
        while (wi < _Sz)    /* pad with zero till full capacity */
            wPtr[wi++] = 0;

//        return strnset(content, pIn.content, _Sz); //strnset pads content with 0 till full size

        return *this;
    }
    utftemplateString& _copyFrom(const utftemplateString&& pIn)
    {

        utfStringDescriptor::_copyFrom(pIn);
        _Utf *wPtr = content;
        const _Utf *wPtrIn = pIn.content;
        size_t wi = 0;
        while (wPtrIn[wi] && (wi < _Sz)) {
            wPtr[wi] = wPtrIn[wi];
            wi++;
        }
        while (wi < _Sz) /* pad with zero till full capacity */
            wPtr[wi++] = 0;
        return *this;
    }

    utftemplateString() {clear();}   /* private constructor for internal use only */
public:
    _Utf          content[_Sz];

    typedef utfStringDescriptor _Base;

    utftemplateString(ZType_type pZType,ZCharset_type pCharset) {utfInit(pZType,pCharset);}
//    utftemplateString(char* pString) {utftemplateString::fromString(pString);}
    utftemplateString(const _Utf* pString,ZType_type pZType,ZCharset_type pCharset)
        {
        utfInit(pZType,pCharset);
        fromUtf(pString);
        }

    utftemplateString(const utfStringDescriptor* pStringDesc):utfStringDescriptor(pStringDesc)
    {
    }
    utftemplateString(const utftemplateString& pIn):utfStringDescriptor(pIn) {_copyFrom(pIn);}
    utftemplateString(const utftemplateString&& pIn):utfStringDescriptor(pIn)  {_copyFrom(pIn);}

    utftemplateString& operator = (const utftemplateString& pIn) {return _copyFrom(pIn); }
    utftemplateString& operator = (const utftemplateString&& pIn) {return _copyFrom(pIn); }

    utftemplateString& operator = (const char* pIn)
    {
      Check = cst_ZCHECK;
      return strSetV<char>(pIn);
    }
    utftemplateString& operator = (std::string& pIn)
    {
      Check = cst_ZCHECK;
      return strSetV<char>(pIn.c_str());
    }

    utftemplateString& operator += (const char* pIn) {return addV<char>(pIn);}
    utftemplateString &operator+=(std::string &pIn) { return addV<char>(pIn.c_str()); }


    utftemplateString operator + (const utftemplateString& pIn)
    {
      utftemplateString wD;
      wD.strset(toUtf());
      wD.add(pIn);
      return wD;
    }
    utftemplateString operator + (const utftemplateString&& pIn)
    {
      utftemplateString wD;
      wD.strset(toUtf());
      wD.add(pIn);
      return wD;
    }


    void utfInit(ZType_type pZType,ZCharset_type pCharset)
    {
      Check = cst_ZCHECK;
        DataByte=(uint8_t*)content;
        UnitCount=_Sz;
        ByteSize=_Sz*sizeof(_Utf);
        CryptMethod=ZCM_Uncrypted;
        if (pCharset==ZCHARSET_SYSTEMDEFAULT)
                    pCharset=getDefaultCharset();
        if (charsetUnitSize(pCharset)!=sizeof(_Utf)){
                                fprintf(stderr,
                                        "%s-Fatal Error charset size <%ld> is incompatible with string unit size <%ld>\n",
                                        _GET_FUNCTION_NAME_,
                                        charsetUnitSize(pCharset),
                                        sizeof(_Utf));
                                    _ABORT_ }
        ZType=pZType;
        /* Coherence control between ZType_type and Charset */
        Charset=pCharset;

        littleEndian=is_little_endian();
        return ;
    }// utfInit

    ~utftemplateString(void) {}

    _Utf* getContentPtr(void) {return (_Utf*)content;}
    size_t getCapacity(void)  {return _Sz;}
    const _Utf* toUtf(void) const {return (const _Utf*)content;}

    template <class _Utf1>
    utftemplateString& strSetV(const _Utf1* pString2)
    {
    _Utf wCChar;
    if (pString2==nullptr)
                return *this;

    size_t wCount = 0;
    _Utf *s1 = content;
    const char *s2 = pString2;
    wCChar=(_Utf)*s2;
    while ((*s2)&& wCount < _Sz-1)
        {
        *s1=(_Utf)*s2;
        s1++;
        s2++;
        wCount++;
        }
    *s1=_Utf('\0');
    return *this;
    }

    template <class _Utf1>
    utftemplateString& addV(const _Utf1* pString2)
    {
    _Utf wCChar;
    if (pString2==nullptr)
                return *this;

    size_t wCount = utfStrlen<_Utf>(content);
    _Utf *s1 = content+wCount;
    const char *s2 = pString2;
    wCChar=(_Utf)*s2;
    while ((*s2)&& wCount < _Sz-1)
        {
        *s1=(_Utf)*s2;
        s1++;
        s2++;
        wCount++;
        }
    *s1=_Utf('\0');
    return *this;
    }


    inline int compare(const _Utf *pString2) const  ; /** corresponds to strcmp with native _Utf argument */

    int compare(const utftemplateString &pString2) const { return compare(pString2.content); }

    /* following is set to able to compare any utf varying string with a char string : it will be set within derived classes */
    /* Remark : this template definition has to remain here */
    template <class _Utf1>
    int compareV(const _Utf1* pString2) const
    {
    _Utf wCChar;
    if (pString2==nullptr)
                return 1;
    const _Utf *s1 = (const _Utf *) content;
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



//    inline int compare(const char* pString2) ; /** see template compareV usage in derived classes */
    inline int ncompare(const _Utf* pString2,size_t pCount) ; /** corresponds to strncmp witn native _Utf argument */
    inline int ncompare(const char* pString2,size_t pCount);/** corresponds to strncmp witn native char argument */

    bool isEqualCase (const _Utf *pCompare) ;
    bool isEqualCase (const utftemplateString<_Sz,_Utf>& pCompare) ;

    /**
     * @brief toDouble converts string content to a double
     *
        In case of success errno is set to 0
        In case of error errno is positionned to
        ENOMEM : cannot allocate memory
        EINVAL : no digits have been detected
        ERANGE : value exceeds double capacity
     *
     * @return a double with converted value
     */
    double toDouble();
    int toInt(int pBase=10);
    int toLong(int pBase=10);



 //   ssize_t strlen(void){return (utfStrlen<_Utf>(content));}
    /**
     * @brief utftemplateString<_Sz, _Utf>::strset equivalent of strcpy. copy cannot override string capacity boundaries.
     * string content is padded with zero.
     * @param wSrc source string
     */
    utftemplateString<_Sz,_Utf>& strset ( const _Utf *wSrc);                /** corresponds to strcpy */
    /**
     * @brief utftemplateString<_Sz, _Utf>::strnset sets string content with at maximum pCount character of wSrc content.
     *  Copied string content cannot exceed string capacity (maximum number of character units).
     * Resulting content is padded with zero up to string capacity.
     * @param wSrc _Utf source string
     * @param pCount maximum number of character to set string with (since beginning of wSrc)
     * @return
     */
    utftemplateString<_Sz,_Utf>& strnset(const _Utf *wSrc,size_t pCount);   /** corresponds to strncpy */

    /**
     * @brief utftemplateString<_Sz, _Utf>::stradd Adds wSrc _Utf string content to current content.<br>
     *  NB: Concatenation can not bypass fixed string boundaries.
     * @param wSrc _Utf string to concatenate
     * @return
     */
    utftemplateString<_Sz,_Utf>& add(const _Utf *wSrc) ;                 /** corresponds to strcat */
    utftemplateString<_Sz,_Utf>& add(const utftemplateString<_Sz,_Utf>& pSrc) ;
    /**
     * @brief utftemplateString<_Sz, _Utf>::nadd concatenates wSrc _Utf string content to current content on at maximum pCount character from wSrc.<br>
     * Concatenation may not bypass fixed string boundaries : in this case, concatenated string is truncated without any other indication.
     * String is padded with zero after concatenation.
     * @param wSrc _Utf string to concatenate
     * @param pCount Maximum number of characters from wSrc to concatenate
     * @return
     */
    utftemplateString<_Sz,_Utf>& nadd( const _Utf *wSrc, size_t pCount) ;/** corresponds to strncat */
    /**
     * @brief add_Char corresponds to strcat but with a char input argument an a maximum count pCount.<br>
     * @Warning This routine does NOT convert any character set.<br>
     * It is not suitable to be used with different character encodings.<br>
     */
    utftemplateString<_Sz, _Utf>& add_Char(const char *wSrc);
    /**
     * @brief nadd_Char corresponds to strncat but with a char input argument an a maximum count pCount.<br>
     * @Warning This routine does NOT convert any character set.<br>
     * It is not suitable to be used with different character encodings.<br>
     */
    utftemplateString<_Sz, _Utf>& nadd_Char(const char *wSrc, size_t pCount);   /** corresponds to strcat but */


    /** sets currents string content with an _Utf formatted content.Maximum size is capacity of template string */
//    utftemplateString<_Sz,_Utf>&
//    sprintf(const typename std::enable_if_t<!std::is_same<_NotChar,char>::value,_NotChar> * pFormat,...) /** sets currents string content with a formatted content.Maximum size is capacity of template string */
//    sprintf(const typename std::conditional<std::is_same<_Utf,char>::value,char,_Utf> * pFormat,...); /** sets currents string content with a formatted content.Maximum size is capacity of template string */

    /** adds _Utf formatted  string to current string. Maximum size is capacity of template string : see method banner for explainations*/
//    utftemplateString<_Sz,_Utf>&
//    addsprintf(const typename std::enable_if_t<!std::is_same<_Utf,char>::value,_Utf>* pFormat,...);  /** adds formatted _Utf string to current string. Maximum size is capacity of template string */
//    addsprintf(const typename std::conditional<std::is_same<_Utf,char>::value,char,_Utf>* pFormat,...);  /** adds formatted _Utf string to current string. Maximum size is capacity of template string */
    /**
     * @brief utftemplateString<_Sz, _Utf>::sprintf set current fixed string content to a formatted char* string.<br>
     * Resulting formatted string cannot exceed string boundaries.<br>
     * In this case, formatted string is truncated without any information.<br>
     * @param pFormat contains formal sprintf syntax with an char* storage format (may contain utf8 encoding).
     * @param varying argument list
     * @return utftemplateString object after operation
     */
    utftemplateString<_Sz,_Utf>& sprintf(const char* pFormat,...);
    /**
     * @brief utftemplateString<_Sz, _Utf>::addsprintf concatenates a char* formatted string (pFormat) to existing string, defined by its _Utf format and its varying arguments list.<br>
     * Resulting formatted string cannot exceed string boundaries after concatenation.<br>
     * In this case, formatted string is truncated without any information.<br>
     * String is padded to zero after concatenation.
     * @see UTFSPRINTF_FORMAT
     * @param pFormat contains formal sprintf syntax .
     * @param varying list of arguments
     * @return utftemplateString object after operation
     */
    utftemplateString<_Sz,_Utf>& addsprintf(const char* pFormat,...);

    /**
     * @brief vsprintf to be used when cascading from another varying arguments (va_list) function @see UTFSPRINTF_FORMAT
     */
    size_t vsprintf (const utfFmt *pFormat,va_list pArglist) {    return utfVsnprintf (Charset,content ,UnitCount,pFormat, pArglist);}
    /**
     * @brief vsprintf_char
     * @return
     */
    size_t vsprintf_char (const char *pFormat,va_list pArglist)
    {
    size_t wRet=utfVsnprintf (Charset,utftemplateString<_Sz,_Utf>::content ,UnitCount,(utfFmt*)pFormat, pArglist);
    return wRet;
    }

//    utftemplateString<_Sz,_Utf>& fromCString(const char*pInString);  /** sets current string's content to C String.*/
    utftemplateString<_Sz,_Utf>& addCString(const char*pInString);  /** adds C String to current string content.*/

    _Utf * strchr(const _Utf wChar) const {return (_Utf*)utfStrchr<_Utf>(content,wChar);}
    _Utf * strrchr(const _Utf wChar) const {return (_Utf*)utfStrrchr<_Utf>(content,wChar);}
    _Utf * strstr(const _Utf *pSubstring) const {return (_Utf*)utfStrstr<_Utf>(content,pSubstring);}
    /** @brief strstr() finds first _Utf substring within current and returns a pointer to it - nullptr if not found */
    _Utf * strstr(const _Utf *pSubstring,const size_t pOffset) const {return (_Utf*)utfStrstr<_Utf>(content+pOffset,pSubstring);}
    /** @brief strcasestr() finds first _Utf substring pSubstring CASE REGARDLESS within current since beginning and returns a pointer to it */
    _Utf* strcasestr(const _Utf *pSubstring) const {return (_Utf*)utfStrcasestr<_Utf>(content,pSubstring);}
    /** @brief strcasestr() finds first _Utf substring pSubstring CASE REGARDLESS within current at unit count pOffset and returns a pointer to it */
    _Utf* strcasestr(const _Utf *pSubstring, size_t pOffset) const {return (_Utf*)utfStrcasestr<_Utf>(content+pOffset,pSubstring);}
    _Utf * strrstr(const _Utf *pSubstring)const {return (_Utf*)utfStrstr<_Utf>((const _Utf*)content,pSubstring);} /**< finds last _Utf substring within current and returns a pointer to it */
    _Utf* strrcasestr(const _Utf *pSubstring)const {return (_Utf*)utfStrcasestr<_Utf>((const _Utf*)content,pSubstring);}/**< finds last _Utf substring CASE REGARDLESS within current and returns a pointer to it */
    _Utf* fill(const _Utf pChar,const size_t pStart=0,const ssize_t pCount=-1) ; /**< fills the content from pStart on pCount with chararcter pChar  */

    /**
     * @brief capacity allias for getUnitCount : returns the available capacity in _Utf character units (not in bytes)
     */
    size_t capacity(void) const {return _Base::UnitCount;}
    /**
     * @brief size returns the actual string length in _Utf units. Warning: this is not the character count (see charCount())
     */
    ssize_t  size(void) const {return (utfStrlen<_Utf>(content)); }
//    size_t getCharCapacity(void) {return _Sz;}
    /**
     * @brief charCount computes the unicode canonical character count of the contained utf string (up until _Utf '\0' found
     */
    ssize_t charCount(void) const;

    ZStatus setCharSet(const ZCharset_type pCharset); /** RFFU  */

    ZDataBuffer   _exportURF() const ;
    size_t        getURFSize() const ;
    static size_t getURFHeaderSize() ;
    size_t        getUniversalSize() const;
    ssize_t       _exportURF(ZDataBuffer& pURFData) const ;
    ssize_t       _exportURF_Ptr(unsigned char* &pURF) const ;
    ssize_t       _importURF(const unsigned char* &pURFDataPtr);

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

    /** @brief _importUVF Import string from Varying Universal Format (dedicated format for strings)
     *  Varying Universal Format stores string data into a varying length string container excluding '\0' character terminator
     *  led by
     *   - uint8_t : char unit size
     *   - UVF_Size_type : number of character units of the string.
     * Important : imported string format (utf-xx) must be the same as current string
     *
     *
     * @param[in,out] pUniversalPtr pointer to Varying Universal formatted data header.
     *        This pointer is updated to point on first byte after imported data.
     * @return total size IN BYTES of  bytes used from pUniversalPtr buffer (Overall used size including header)
     */
    size_t _importUVF(unsigned char* & pUniversalPtr);

    /** @brief _getexportUVFSize() compute the requested export size in bytes for current string, including header */
    UVF_Size_type _getexportUVFSize();

    /** @brief _getimportUVFSize() returns total size in byte of data, including header, the import will use from input buffer.*/
    UVF_Size_type _getimportUVFSize(unsigned char* pUniversalPtr);



    /**
     * @brief utftemplateString::getUniversalFromURF Static method that extract an Universal value from memory zone pointed by pURFDataPtr
     *      and returns a ZDataBuffer pUniversal loaded with Universal value padded with binary zeros to its number of character units.<br>
     *      output ZDatatBuffer is of capacity size corresponding to character units count * character units size<br>
     *      output data is padded with '\0' character up until full capacity of current fixed string.
     * @return a ZStatus
     */
    static ZStatus getUniversalFromURF(ZTypeBase pType,const unsigned char* pURFDataPtr,ZDataBuffer &pUniversal,const unsigned char** pURFDataPtrOut=nullptr);
    /**
     * @brief utftemplateString::getUniversalFromURF_Truncated
     *      Static method that extract Universal value from memory zone pointed by pURFDataPtr
     *      and returns a ZDataBuffer loaded with Universal value truncated to its effective number of character units.<br>
     *      output ZDB is of capacity size corresponding to character unit count * character unit size<br>
     *      output data is padded with '\0' character up until full capacity of current fixed string.
     * @return
     */
    static ZStatus getUniversalFromURF_Truncated(ZTypeBase pType,const unsigned char* pURFDataPtr,ZDataBuffer &pUniversal,const unsigned char** pURFDataPtrOut=nullptr);


    utftemplateString<_Sz,_Utf>& fromISOLatin1(const unsigned char* pString, size_t pInSize,ZStatus* pStatus);
    utftemplateString<_Sz,_Utf>* toISOLatin1(utftemplateString<_Sz,_Utf>* pOutString ,ZStatus* pStatus);

    ZStatus fromCharset(const unsigned char* pInString,  /** @todo change within fromCharset to  ZIcuConverters objecs*/
                        const size_t pInSize,
                        const ZCharset_type pInCharset)
        {
        clear();
        return _fromCharset((unsigned char*)content,
                            getByteSize(),
                            Charset,
                            pInString,
                            pInSize,
                            pInCharset);
        }

    //----------------crypting checksum md5---------------------------
    checkSum getCheckSum(void) const
    {
        size_t byteLen = utfStrlen<_Utf>(content)*sizeof(_Utf);
        return checkSum ((unsigned char*)content,byteLen);}// gets a checkSum from string content without '\0'
    md5 getMd5(void) const
    {
        size_t byteLen = utfStrlen<_Utf>(content)*sizeof(_Utf);
        return md5 ((unsigned char*)content,byteLen);
    }// gets a md5 from string content without '\0'

    ZDataBuffer& encryptSHA256(ZDataBuffer& pZDB);
    utftemplateString<_Sz,_Utf>& uncryptSHA256(ZDataBuffer& pZDB);
    ZDataBuffer& encryptB64(ZDataBuffer& pZDB);
    utftemplateString<_Sz,_Utf>& uncryptB64(ZDataBuffer& pZDB);


    //-------------------------------------------------------------------
    bool isEmpty (void) const {return (content[0]==(_Utf)'\0');}
    void clear(void) {memset(content,0,getUnitCount()); UnitCount = _Sz; ByteSize= UnitCount *sizeof(_Utf);}

    const _Utf * toString(void) const {return((const _Utf *)content);}

    /**
     * @brief addUtfUnit adds an Utf unit at the end of string (a unit is not necessarily a character) plus _Utf \0 sign.
     * @param pChar _Utf character unit to add
     * @return the current string object as reference
     */
     utftemplateString<_Sz,_Utf>& addUtfUnit(const _Utf pChar);

    utftemplateString & conditionalNL(void)
    {
        if (!utfIsTerminatedBy<_Utf>(content,(_Utf)'\n'))
            {
            size_t wSize= utfStrlen<_Utf>(content);
            if (wSize>(UnitCount-1))
                        return *this;
            content[wSize++]=(_Utf)'\n';
            content[wSize]=(_Utf)'\0';
            }
        return *this;
    }

    utftemplateString<_Sz,_Utf> &fromUtf(const _Utf *pString) ;
    utftemplateString<_Sz,_Utf>& fromUtfCount(const _Utf *pString, const size_t pCount) ;
    utftemplateString<_Sz,_Utf>& fromWString_Ptr(const wchar_t*pWString);
    utftemplateString<_Sz,_Utf>& fromWString_PtrCount(const wchar_t*pWString,const size_t pCount);

    utftemplateString& fromInt(const int pInt) { sprintf("%d",pInt);
                                                           return(*this);}
    utftemplateString& fromLong(const int pLong) { sprintf("%ld",pLong); return(*this);}
    utftemplateString& fromULong(const int pLong) { sprintf("%uld",pLong); return(*this);}


// ---------String content edition----------------
    utftemplateString &LTrim(const _Utf *pSet=nullptr);

    _Utf *firstNotinSet(const _Utf *pSet=nullptr);
    utftemplateString &RTrim(const _Utf *pSet=nullptr);
    utftemplateString &Trim(const _Utf *pSet=nullptr);

    _Utf* strchrReverse(const _Utf pChar);
    const _Utf* strspnReverse(const  _Utf *pSet);

 //   bool isSpace(void) {return(_firstNotinSet(content,__DEFAULTDELIMITERSET__)==nullptr);}


    bool startsCaseWith(const _Utf* pString,const _Utf* pSet=__DEFAULTDELIMITERSET__);
    bool startsWith(const _Utf* pString) const;
    bool startsWithApprox(const _Utf* pString, ssize_t pApprox, const _Utf* pSet=(const _Utf*)__DEFAULTDELIMITERSET__);
    bool startsCaseWithApprox(const _Utf* pString,int pApprox,const _Utf* pSet=(const _Utf*)__DEFAULTDELIMITERSET__);

     int Substr(const _Utf *pSub, int pOffset=0) const ;
     int SubstrCase(const _Utf *pSub, int pOffset=0) const;

     /** @brief subString() returns a new string with content of current string starting a position pOffset and with pLen character units */
     utftemplateString subString(size_t pOffset, int pLen=-1) const;

     utftemplateString Left (size_t pLen) const ;
     utftemplateString Right (size_t pLen) const;

     utftemplateString & eliminateChar (_Utf pChar);

     bool    contains (const _Utf* pString) const {return (strstr(pString)!=nullptr);}
     bool    containsCase (const _Utf* pString) const {return (strcasestr(pString)!=nullptr);}


     bool    contains (utftemplateString &pString) const  {return (strstr(pString.content)!=nullptr);}
     bool    containsCase (utftemplateString &pString)const {return (strcasestr(pString.content)!=nullptr);}

     /** @brief locate()  locates a substring pString in utftemplateString
      *                            and returns its offset from beginning as a ssize_t value.*/
     ssize_t locate(const _Utf *pString) const;
     /** @brief locate()  locates a substring pString in utftemplateString and returns its offset
      *                     since offset pOffset (zero meaning beginning of string) as a ssize_t value.*/
     ssize_t locate(const _Utf *pString, size_t pOffset) const;
     ssize_t locate(utftemplateString<_Sz,_Utf>& pString) const;
     /** @brief locateCase()  locates a substring pString CASE REGARDLESS in utftemplateString
      *                         and returns its offset from beginning as a ssize_t value.*/
     ssize_t locateCase(const _Utf* pString) const;
     /** @brief locateCase()  locates a substring pString CASE REGARDLESS in utftemplateString
      *                        and returns its offset since pOffset (zero meaning beginning of string) as a ssize_t value.*/
     ssize_t locateCase(const _Utf* pString,size_t pOffset) const;

     utftemplateString<_Sz,_Utf>& toUpper(void);
     utftemplateString<_Sz,_Utf>& toUpper(utftemplateString<_Sz,_Utf>& pOut);

     utftemplateString<_Sz,_Utf>& addConditionalOR (const _Utf*pValue);



//------------B64------------------------------------

 utftemplateString<_Sz,_Utf> & decodeB64(void);
 utftemplateString<_Sz,_Utf> & encodeB64(void);
//------------MD5------------------------------------
 utftemplateString<_Sz,_Utf> & encodeMD5(void);
//-----------Java HashCode---------------------------
 long hashCode(void);




//------------------ operator overloads  ------------------------------------------------
//

     _Utf& operator [] (const size_t pIdx) const
        { if(pIdx>getUnitCount()) _ABORT_ return  (content[pIdx]);}


     utftemplateString<_Sz,_Utf>& operator = (const _Utf *pString) {return utftemplateString<_Sz,_Utf>::strset(pString);}


     utftemplateString<_Sz,_Utf>& operator += (const _Utf pChar)
    {

        size_t wS= utfStrlen<_Utf>(content);
        if (wS >=_Sz-2)
            {
            fprintf(stderr,"utftemplateString-F-ZS_OUTBOUND  Out of string boundaries\n");
            return *this;
            }
        content[wS]=(_Utf)pChar;
        wS++;
        content[wS]=(_Utf)'\0';
        return (*this);
    }

     utftemplateString<_Sz,_Utf>& operator = (utftemplateString<_Sz,_Utf>& pString)  {return strset (pString.content);}
     utftemplateString<_Sz,_Utf>& operator += (utftemplateString<_Sz,_Utf> &pString) { return add (pString.content);} // +=

     utftemplateString<_Sz,_Utf>& operator += (const _Utf * pString) {return add(pString);}

     //utftemplateString<_Sz,_Utf>& operator += (const char * pString) {return addCString(pString);}


     bool operator == (const utftemplateString<_Sz,_Utf> &pCompare) const  { return(compare(pCompare.content)==0);}
     bool operator == (const utftemplateString<_Sz,_Utf>&& pCompare) const { return(compare(pCompare.content)==0);}
     bool operator != (const utftemplateString<_Sz,_Utf> &pCompare) const { return(!compare(pCompare.content)==0);}
     bool operator != (const char* pCompare) const { return(!compareV<char>(pCompare)==0);}
     bool operator > (const utftemplateString<_Sz,_Utf> &pCompare) const { return(compare(pCompare.content)>0);}
     bool operator < (const utftemplateString<_Sz,_Utf> &pCompare) const { return(compare(pCompare.content)<0);}

     bool operator == (const _Utf *pCompare) const { return(compare(pCompare)==0);}
     bool operator == (const char *pCompare) const { return(compareV<char>(pCompare)==0);}

     bool operator != (const _Utf *pCompare) const { return!(compare(pCompare)==0);}
     bool operator > (const _Utf *pCompare) const { return(compare(pCompare)>0);}
     bool operator < (const _Utf *pCompare) const { return(compare(pCompare)<0);}


ZDataBuffer* toZDataBuffer(void)
{
    ZDataBuffer* wDB = new ZDataBuffer;
    wDB->setData(content,ByteSize);
    return wDB;
}

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
//    utf32VaryingString& addBOM( ZBool *pEndian);
protected:
    utfStrCtx   Context;
    ZBool       WarningSignal=false;



};// struct utftemplateString---------------------------------------------

//======================Generic functions===========================================================

template <size_t _Sz1,size_t _Sz2>
utftemplateString<_Sz1>& moveutftemplateString (utftemplateString<_Sz1>& pDest,utftemplateString<_Sz2>& pSource);



//======================End Generic functions===========================================================
#include <typeinfo>
//-------------utftemplateString Methods expansion--------------
//
template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz, _Utf>::charCount gets the effective character count : multi-character character are counted for 1.
 * @return number of canonical character or -1 if an error occurred (invalid utf encoding)
 */
ssize_t
utftemplateString<_Sz,_Utf>::charCount(void) const
{
utfStrCtx wContext;
ssize_t wCount;
ZStatus wSt;

    if (typeid(_Utf).hash_code() == typeid(char).hash_code())
        {
        return ::strlen(content);
        }
    switch(sizeof(_Utf))
    {
    case sizeof(utf8_t):
        {
        wCount= utf8CharCount(content,&wSt,nullptr);
        if (wSt<0)
            {
            fprintf(stderr,"%s>> An error occurred while counting characters in string. Status <%X> <%s>\n",
                    _GET_FUNCTION_NAME_,
                    wSt,
                    decode_ZStatus(wSt));
            }
        return wCount;
        }
    case sizeof(utf16_t):
        {
        wCount=utf16CharCount(content,&wSt,nullptr,nullptr);
        if (wSt<0)
            {
            fprintf(stderr,"%s>> An error occurred while counting characters in string. Status <%X> <%s>\n",
                    _GET_FUNCTION_NAME_,
                    wSt,
                    decode_ZStatus(wSt));
            }
        return wCount;
        }
    case sizeof(utf32_t):
        {
        wCount=utf32CharCount(content,&wSt,nullptr,nullptr);
        if (wSt<0)
            {
            fprintf(stderr,"%s>> An error occurred while counting characters in string. Status <%X> <%s>\n",
                    _GET_FUNCTION_NAME_,
                    wSt,
                    decode_ZStatus(wSt));
            }
        return wCount;
        }
    }

} // charCount

/* same as strset : must be aliased or suppressed */

template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::fromUtf(const _Utf* pInString)
{
    _Utf*wPtr=content;
    size_t wi=0;
    utftemplateString::clear();
    if (pInString==nullptr)
            {
            content[0]=(_Utf)'\0';
            return *this;
            }
    for (wi=0;(*pInString!=0)&&(wi<(getUnitCount()-1));wi++)
                                *wPtr++=(_Utf)*pInString++;
    while (wi++<(getUnitCount()))
        *wPtr++=(_Utf)'\0';
    return(*this);
}//fromUtf

template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::fromUtfCount(const _Utf* pInString,const size_t pCount)
{
    utftemplateString<_Sz,_Utf>::clear();
    int wCount=pCount;
    if (pCount>(UnitCount-1))
                wCount = UnitCount-1;

    utfStrncpy<_Utf>(content,pInString,wCount);
//    content[wCount]='\0';  Strncpy pads to zero
    return(*this);
}






/**
 * @brief utftemplateString<_Sz,_Utf>::eliminateChar scans current string content and wipes (eliminates) the given char.
 * After having called this procedure, the given char does no more exist within the string,
 * so that string length is reduced in proportion of number of char eliminated.
 * @param[in] pChar character to eliminate
 * @return a reference to the current utftemplateString
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf> &
utftemplateString<_Sz,_Utf>::eliminateChar (_Utf pChar)
{
    _Utf* wPtr = content ;
    while (*wPtr!='\0')
    {
        if (*wPtr==pChar)
                utfStrcpy<_Utf>(wPtr,&wPtr[1]);
             else
                wPtr++;
    }//while
    return *this;
}//eliminateChar

/**
 * @brief utftemplateString<_Sz,_Utf>::addConditionalOR append pString to current string with a leading conditional OR mark ('|').
 *      if target string is not empty, then appends OR sign plus spaces < | > before appending string content.
 *      if target string is empty, then appends directly pString without OR sign.
 * @param pValue _Utf string to add to current string
 * @return a reference to current utftemplateString object
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::addConditionalOR (const _Utf*pString)
{
  _Utf wOr[4];
  wOr[0]=_Utf(' ');
  wOr[1]=_Utf('|');
  wOr[2]=_Utf(' ');
  wOr[3]=0;

    if (!pString)
            return *this;
    if (!*pString)
            return *this;
    if (!isEmpty())
            add(wOr);
    add(pString);
    return *this;
} // addConditionalOR

//-----------------------keywordString--------------------------

/**
 * @brief locate  locates a substring pString in utftemplateString and returns its offset from beginning as a ssize_t value.
 * returns a negative value if substring has not been found.
 * @param pString a const char* string containing the substring to find
 * @return offset (starting from 0) of the substring in main string if substring is found, a negative value if not.
 */
template <size_t _Sz,class _Utf>
ssize_t
utftemplateString<_Sz,_Utf>::locate(const _Utf* pString) const
{
    if (!pString)
            return -1;
    ssize_t wRet = strstr(pString) - content;
    if (wRet < 0)
        return -1;
    return wRet;
}

template <size_t _Sz,class _Utf>
ssize_t
utftemplateString<_Sz,_Utf>::locate(const _Utf* pString,size_t pOffset) const
{
    if (!pString)
        return -1;
    ssize_t wRet = strstr(pString,pOffset) - content;
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
template<size_t _Sz, class _Utf>
ssize_t utftemplateString<_Sz, _Utf>::locate(utftemplateString<_Sz, _Utf> &pString) const

{
    if (pString.isEmpty())
        return 0;
    ssize_t wRet = strstr(pString.content) - content;
    if (wRet < 0)
        return -1;
    return wRet;
}

#include <ztoolset/zcharset.h>
/**
 * @brief toUpper Upperizes characters of current string. Current string is modified if ever by characters upperization.
 * @return a reference to current string
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::toUpper(void)
{
    utfToUpper<_Utf>(content,nullptr);
    return *this;
}

template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz, _Utf>::locateCase same as locate() but case regardless.
 * @param pString
 * @return
 */
ssize_t
utftemplateString<_Sz,_Utf>::locateCase(const _Utf* pString) const
{
    if (!pString)
            return -1;

    return (ssize_t)(strcasestr(pString)-content);
}
template <size_t _Sz,class _Utf>
ssize_t
utftemplateString<_Sz,_Utf>::locateCase(const _Utf* pString, size_t pOffset) const
{
    if (!pString)
        return -1;

    return (ssize_t)(strcasestr(pString,pOffset)-content);
}
/**
 * @brief toUpper Upperizes characters of current string. Upperization is made into an output string of same type pOut.
 * @param[out] pOut a template string destinated to receive the upperized string
 * @return a reference to current string
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::toUpper(utftemplateString<_Sz,_Utf>& pOut)
{
    pOut=content;
    utfToUpper<_Utf>(pOut.content);
    return pOut;
}

/**
 * @brief utftemplateString::firstNotinSet   finds the first byte of string pString that is NOT a byte of pSet.
 *
 * usage example : find the first non space and non tab (\t) char of a string.
 *
 * @param pSet set of characters to consider. Defaulted to __DEFAULTDELIMITERSET__
 * @return a const char pointer to the first byte of utftemplateString content that is NOT in pSet
 */
template <size_t _Sz,class _Utf>
_Utf *utftemplateString<_Sz, _Utf>::firstNotinSet(const _Utf *pSet)
{
    return((_Utf *)utfFirstNotinSet<_Utf>((const _Utf*)content,pSet));
}

/**
 * @brief utftemplateString::LTrim Left trims utftemplateString::content with character set pSet.
 * @param pSet set of characters to consider. Defaulted to __DEFAULTDELIMITERSET__
 * @return a reference to current keywordString object
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::LTrim ( const _Utf *pSet)
{
    const _Utf* wSet=pSet;
    if (!wSet)
            wSet=getDefaultDelimiter<_Utf>();
    const _Utf* wPtr=firstNotinSet(wSet);
    if (wPtr==nullptr)
            return(*this);
    strset(wPtr);
    return (*this);
}

/**
 * @brief utftemplateString::RTrim Suppresses ending characters that are in pSet (defaulted to __DEFAULTDELIMITERSET__)
 * @param pSet character set to trim off
 * @return a reference to current utftemplateString object
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::RTrim (const _Utf *pSet)
{
size_t wj;
_Utf* wData=content;
    if (!*wData)
            return *this;
    while (*wData++) ;
    wData--;

const _Utf* wSet=pSet;
    if (!wSet)
            wSet=getDefaultDelimiter<_Utf>();
const _Utf* wSetInit=wSet;

    while (wData>content)
        {
        while (*wSet &&(*wData!=*wSet++))   ;
        if (!*wSet)
                return *this;
        *wData=(_Utf)'\0';
        wSet=wSetInit;
        wData--;
        }
    return *this;

}//   RTrim
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::Trim(const _Utf *pSet)
{
const _Utf* wSet=pSet;
    if (!wSet)
            wSet=getDefaultDelimiter<_Utf>();
     LTrim(wSet);
     return(RTrim(wSet));
}// Trim

/**
 * @brief strchrReverse find the first occurrence of pChar within pStr string in reverse (starting to strlen() and ending at 0)
 * @param pChar the character to find
 * @return pointer to pChar occurrence within string if found, NULL if not found
 */
template <size_t _Sz,class _Utf>
_Utf *utftemplateString<_Sz, _Utf>::strchrReverse(const _Utf pChar)
{
    _Utf* wPtr = content;
    if (!*wPtr)
            return nullptr;
    while (*wPtr++) ;

    wPtr--;
    while ((*wPtr!=pChar)&&(wPtr>content))
                    wPtr--;
    if (wPtr==content)
           return nullptr;
    return wPtr;
}

/**
 * @brief utftemplateString<_Sz,_Utf>::strspnReverse
 *          finds the first occurrence of one of the characters set given by pSet within keywordString::content in reverse (starting to strlen() and ending at 0)
 * @param pSet set of characters to search for (defaulted to  __DEFAULTDELIMITERSET__).
 * @return pointer to pChar occurrence within keywordString::content if found, NULL if not found
 */
template <size_t _Sz,class _Utf>
const _Utf *
utftemplateString<_Sz,_Utf>::strspnReverse( const _Utf *pSet)
{
    const _Utf* wSet=pSet;
        if (!wSet)
                wSet=getDefaultDelimiter<_Utf>();
    size_t wL = utfStrlen<_Utf>(wSet);
    size_t wj = 0;
    long wi;

    for (wi=utfStrlen<_Utf>(content);(wi >= 0)&&(content[wi]!=wSet[wj]);wi--)
                            {
                            for (wj=0;(wj<wL)&&(content[wi]!=wSet[wj]);wj++);
                            }
    return ((wi<0) ? nullptr: &content[wi]);     // return NULL pointer if not found pointer to pChar within string if found
}


template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::startsCaseWith Test if String starts with pString regarless case after having trimmed out character from pSet
 * @param[in] pString substring to search
 * @param[in] pSet set of characters to consider as being eliminated (trimmed)(defaulted to __DEFAULTDELIMITERSET__)
 * @return a bool set to true if current string starts with given string pString
 */
bool
utftemplateString<_Sz,_Utf>::startsCaseWith(const _Utf* pString,const _Utf* pSet)
{
    LTrim(pSet);
    return !utfStrncasecmp<_Utf>(content,pString,utfStrlen<_Utf>(pString));
}
/**
 * @brief utftemplateString::startsWith Test if String starts with pString after having trimmed out character from pSet
 * @param[in] pString substring to search
 * @param[in] pSet  character set to trim off (defaulted to __DEFAULTDELIMITERSET__)
 * @return true if found, false if not found
 */
template <size_t _Sz,class _Utf>
bool
utftemplateString<_Sz,_Utf>::startsWith(const _Utf* pString) const
{
    return !utfStrncmp<_Utf>(content,pString,utfStrlen<_Utf>(pString));
}
/**
 * @brief keywordString::startsWithApprox
 *  Test if utftemplateString::content starts with pString after having trimmed out character in pSet
 * this test is done with an approximation of pApprox. test is done Case sensitive.
 * @param pString substring to search
 * @param pApprox minimum number of character of pString to consider the test valid
 * @param pSet  character set to trim off (defaulted to __DEFAULTDELIMITERSET__)
 * @return true if found, false if not found
 */
template <size_t _Sz,class _Utf>
bool
utftemplateString<_Sz,_Utf>::startsWithApprox(const _Utf* pString,ssize_t pApprox,const _Utf* pSet)
{
    utfLTrim<_Utf>(content,pSet);
    const _Utf* wPtr = utfFirstinSet<_Utf>(content,pSet);
    ssize_t wSize;
    if (wPtr==nullptr)
                wSize=utfStrlen<_Utf>(content);
            else
                wSize=wPtr-content;
    if (wSize<pApprox)
            return false;
    if (ncompare(pString,wSize)==0)
                                    return true;
    return (false);
}
/**
 * @brief utftemplateString::startsCaseWithApprox
 *  Test if utftemplateString::content starts with pString after having trimmed out character in pSet
 * this test is done with an approximation of pApprox. Case unsensitive.
 * comparison is made without taking into account cases.
 * @param pString substring to search
 * @param pApprox minimum number of character of pString to consider the test valid
 * @param pSet  character set to trim off (defaulted to __DEFAULTDELIMITERSET__)
 * @return true if found, false if not found
 */
template <size_t _Sz,class _Utf>
bool
utftemplateString<_Sz,_Utf>::startsCaseWithApprox(const _Utf* pString,int pApprox,const _Utf* pSet)
{
    utfLTrim<_Utf>(content,pSet);
    const _Utf* wPtr = utfFirstinSet<_Utf>(content,pSet);
    size_t wSize;
    if (wPtr==nullptr)
                wSize=utfStrlen<_Utf>(content);
            else
                wSize=wPtr-content;
    if (wSize<pApprox)
            return false;
    if (utfStrncasecmp<_Utf>(content,pString,wSize)==0)
                                        return true;
    return (false);
}


/**
 * @brief utftemplateString::Substr Searches a substring pSub within utftemplateString::content starting at pOffset
 * and returns its position SINCE BEGINNING (start 0) of utftemplateString::content
 *
 * Returns -1 if not found
 *
 * Search is case sentive
 * @param pSub  substring to search
 * @param pOffset offset (start 0) to begin search in keywordString::content
 * @return offset of the beginning of substring (start 0) within keywordString::content
 */
template <size_t _Sz,class _Utf>
int
utftemplateString<_Sz,_Utf>::Substr(const _Utf* pSub, int pOffset) const
{
    const _Utf * wPtr = utfStrstr<_Utf>((const _Utf*)&content[pOffset],pSub);
        if (wPtr==nullptr)
                        return -1;
         return((int)(wPtr-content));
}

template<size_t _Sz, class _Utf>
utftemplateString<_Sz, _Utf> utftemplateString<_Sz, _Utf>::subString(size_t pOffset, int pLen) const
{
    utftemplateString<_Sz, _Utf> wStrReturn; /* create same string type as current but empty*/

    wStrReturn.utfStringDescriptor::_copyFrom(*this);

    if (pLen > 0) {
        if ((pLen + pOffset) > _Sz)
                pLen = _Sz - pOffset ;
         wStrReturn.strnset(&content[pOffset],pLen);
        }
    else
        wStrReturn.strset(&content[pOffset]);
    return wStrReturn;
}
/**
 * @brief utftemplateString::SubstrCase Searches a utftemplateString pSub within keywordString content starting at pOffset
 * and returns its position SINCE BEGINNING (start 0) of utftemplateString::content
 *
 * Returns -1 if not found
 *
 * Search is case insentive (case or Uppercase are treated as same)
 *
 * @param pSub C string as a substring to search for
 * @param pOffset offset (start 0) to begin search in keywordString::content
 * @return offset of the beginning of substring (start 0) within keywordString::content
 */

template <size_t _Sz,class _Utf>
int
utftemplateString<_Sz,_Utf>::SubstrCase(const _Utf *pSub, int pOffset) const
{
    _Utf * wPtr = utfStrcasestr<_Utf>(&content[pOffset],pSub);
        if (wPtr==nullptr)
                        return -1;
         return((int)(wPtr-content));
}

/**
 * @brief utftemplateString<_Sz,_Utf>::Left return the left part of current utftemplateString on pLen within a string of same type than current.
 * @param pLen
 * @return
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>
utftemplateString<_Sz,_Utf>::Left (size_t pLen) const
{
   utftemplateString<_Sz,_Utf> wU ((utfStringDescriptor*)this); /* create a string with same structure as current */
   wU.strnset(content,pLen);
   return wU;
}
/**
 * @brief keywordString::Right returns a keywordString with a content of length pLen containing
 * the pLen last characters (on the right) of keywordString actual content.
 * if pLen is greater than actual content size, then it takes the whole content from start.
 * @param pLen
 * @return
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>
utftemplateString<_Sz,_Utf>::Right (size_t pLen) const
{
    utftemplateString<_Sz,_Utf> wU ((utfStringDescriptor*)this); /* create a string with same structure as current */
   int wStart= utfStrlen<_Utf>(content)-pLen;
   if (wStart<0)
                wStart=0;
   wU.strnset(&content[wStart],pLen);
   return wU;
}
//-----------Charset-----------------------------------------------------

/*template <size_t _Sz,size_t _Sz2>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::fromEncoded(utftemplateString<_Sz2> &pString, ZStatus* pStatus)
{
    *pStatus=_fromEncoded(*this,pString);
    return *this;
}
*/


//-----------Encoding & Decoding-----------------------------------------
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::decodeB64(void)
{
 if (CryptMethod==ZCM_Uncrypted)
                        return *this; // avoid double uncrypting
 ZDataBuffer wZDB ;
    wZDB.uncryptB64(content, size()+1);  // decode to ZDataBuffer
    wZDB.moven(content,sizeof(content));    // move back the decoded content to utftemplateString content
    CryptMethod = ZCM_Uncrypted;
    return *this;
}
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::encodeB64(void)
{
    if (CryptMethod==ZCM_Base64)     // avoid double crypting
                    return(*this);
    ZDataBuffer wZDB ;
    wZDB.encryptB64(content, size()+1);  // encode to a ZDataBuffer
    wZDB.moven(content,sizeof(content));    // move back the encoded string to utftemplateString content
    CryptMethod = ZCM_Base64;
    return *this;
}

template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::encodeMD5(void)
{
    if (CryptMethod==ZCM_MD5)     // avoid double crypting
                    return(*this);
    ZDataBuffer wZDB ((unsigned char*)content, size());
    wZDB.getmd5();  // encode to a ZDataBuffer
    wZDB.moven(content,sizeof(content));    // move back the encoded string to utftemplateString content
    content[wZDB.Size] = '\0';
    CryptMethod = ZCM_MD5;
    return *this;
}

#include <cmath>

template <size_t _Sz,class _Utf>
long
utftemplateString<_Sz,_Utf>::hashCode(void)
{
    long wh = 0;
    long ws=size();
    double wValue,wExponent;

    if ( ws > 0)
    {
        for (int wi = 0; (wi <  size()); wi++)
        {

//            wh = 31*content[wi]^ (ws-wi);
            wValue= 31*content[wi];
            wExponent = ws-wi;
            wh=(long)powl(wValue,wExponent);
        }
    }
    return wh;
}//hashCode

//============= Import Export to URF format=================================

template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::_exportURF exports the content of string to a ZDataBuffer in an URF format
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
 *   - ZTypeBase : ZType_utf8FixedString, ZType_utf16FixedString,... Gives the Character Unit size with ZType_AtomicMask
 *   - URF_Canon_Size_type (canonical capacity) number of character units available for the fixed size container.
 *                  Example : cst_desclen+1 (canonical length of descString)
 *
 *   - URF_Fixed_Size_type effective URF string byte length, included URF header AND termination char unit '\0' EXCLUDED
 *
 *  see <ZType_Type vs Header sizes> in file <zindexedfiles/znaturalfromurf.cpp>
 *data :
 *  a suite of _Utf character unit sequense up to number of characters units.
 *  _Utf character units are converted to big endian if necessary.
 *
 * limits :
 *
 *  Maximum size is in bytes :      65535
 *  Maximum number of character units :  65534 (for an utf8 string) or the minimum between max size in bytes and max size in char units.
 *
 *
 *
 * @param[out] pURFData   Data in URF Format
 *
 * @return
 */
ZDataBuffer
utftemplateString<_Sz,_Utf>::_exportURF() const
{
  ZDataBuffer wURFData;
  _exportURF(&wURFData);
  return wURFData;
}

template <size_t _Sz,class _Utf>
ssize_t
utftemplateString<_Sz,_Utf>::_exportURF(ZDataBuffer& pURFData) const
{
size_t wEffectiveUnitCount= utfStrlen<_Utf>(content);   /* effective string size in unit count '\0' excluded */
URF_UnitCount_type wUniversalSize=URF_UnitCount_type(wEffectiveUnitCount*sizeof(_Utf)); /* effective string size in bytes '\0' excluded */

    if (wUniversalSize>=UINT16_MAX)
        {
        fprintf (stderr,"%s>>Fatal error incoherent fixed string maximum capacity overflow <%d> while <%d> authorized.\n",
                 _GET_FUNCTION_NAME_,
                 wUniversalSize,
                 UINT16_MAX);
        _ABORT_
        }

size_t wTotalByteSize=(size_t)(wUniversalSize+
                      sizeof(URF_UnitCount_type)+
                      sizeof(URF_Capacity_type)+
                      sizeof(ZTypeBase));

  unsigned char* wPtrOut=pURFData.extendBZero((ssize_t)wTotalByteSize);
  return _exportURF_Ptr(wPtrOut);
}// _exportURF

template <size_t _Sz,class _Utf>
ssize_t utftemplateString<_Sz, _Utf>::_exportURF_Ptr(unsigned char* &pURF) const
{
  URF_Capacity_type wCanonical=(URF_Capacity_type)getUnitCount(); /* fixed size in unit count */

  size_t wEffectiveUnitCount= utfStrlen<_Utf>(content);   /* effective string size in unit count '\0' excluded */
  size_t wByteSize = wEffectiveUnitCount * sizeof(_Utf) ;
  URF_UnitCount_type wUnitsCount= URF_UnitCount_type(wEffectiveUnitCount);

  if (wEffectiveUnitCount > wCanonical)
  {
    fprintf (stderr,"%s>>Fatal error incoherent fixed string maximum capacity overflow <%d>(effective) while <%d> (capacity) authorized.\n",
        _GET_FUNCTION_NAME_,
        wEffectiveUnitCount,
        wCanonical);
    _ABORT_
  }

  const _Utf* wPtrIn=content;

  _exportAtomicPtr<ZTypeBase>(ZType,pURF);
  _exportAtomicPtr<URF_Capacity_type>(wCanonical,pURF);
  _exportAtomicPtr<URF_UnitCount_type>(wUnitsCount,pURF);
  _Utf* wPtrOut = (_Utf*)pURF;
  size_t wCount=0;
  if (sizeof(_Utf)==1)
    while ( *wPtrIn && (wCount++ < capacity() ))
      *wPtrOut ++ =*wPtrIn++;
  else
    while (*wPtrIn)
      *wPtrOut ++= reverseByteOrder_Conditional<_Utf>(*wPtrIn++);

  pURF = (unsigned char*)wPtrOut;

  return  ssize_t(sizeof(ZTypeBase) + sizeof(URF_Capacity_type) + sizeof(URF_UnitCount_type) + wByteSize);

}// _exportURF_Ptr

template <size_t _Sz,class _Utf>
size_t
utftemplateString<_Sz,_Utf>::getURFSize() const {
  size_t wEffectiveUnitCount= utfStrlen<_Utf>(content);   /* effective string size in unit count '\0' excluded */
  URF_UnitCount_type wUniversalSize=URF_UnitCount_type(wEffectiveUnitCount*sizeof(_Utf)); /* effective string size in bytes '\0' excluded */

  if (wUniversalSize>=UINT16_MAX) {
    fprintf (stderr,"%s>>Fatal error incoherent fixed string maximum capacity overflow <%d> while <%d> authorized.\n",
        _GET_FUNCTION_NAME_,
        wUniversalSize,
        UINT16_MAX);
    _ABORT_
  }

  return (size_t)(wUniversalSize+
         sizeof(URF_UnitCount_type)+
         sizeof(URF_Capacity_type)+
         sizeof(ZTypeBase));
} // getURFSize

template <size_t _Sz,class _Utf>
size_t
utftemplateString<_Sz,_Utf>::getURFHeaderSize()  {
   return (size_t)(sizeof(URF_UnitCount_type)+
                  sizeof(URF_Capacity_type)+
                  sizeof(ZTypeBase));
} // getURFSize

template <size_t _Sz,class _Utf>
size_t
utftemplateString<_Sz,_Utf>::getUniversalSize() const {
  size_t wEffectiveUnitCount= utfStrlen<_Utf>(content);
  return wEffectiveUnitCount*sizeof(_Utf);/* effective string size in unit count '\0' excluded */
} // getUniversalSize

template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::_importURF
 * @param pUniversal pointer to URF data header
 * @return
 */
ssize_t utftemplateString<_Sz, _Utf>::_importURF(const unsigned char *& pURFDataPtr)
{
ZTypeBase           wType;
URF_Capacity_type   wCanonical;
URF_UnitCount_type  wUnitCount;
size_t              wEffectiveUnitCount;

//size_t      wURFByteSize;
size_t      wOffset=0;
  errno=0;
  wOffset+=_importAtomic<ZTypeBase>(wType,pURFDataPtr);   // updates pURFDataPtr
  if (!(wType&ZType_String))
        {
        fprintf (stderr,"%s>> Invalid class type. Found %x <%s> while expecting a ZType_String\n",
        _GET_FUNCTION_NAME_,
                 wType,
                 decode_ZType(wType));
        errno=EPERM;
        return 0;
        }

  wOffset += _importAtomic<URF_Capacity_type>(wCanonical,pURFDataPtr);   // updates pURFDataPtr
  wOffset += _importAtomic<URF_UnitCount_type>(wUnitCount,pURFDataPtr);   // updates pURFDataPtr

  wEffectiveUnitCount=size_t(wUnitCount);
  if (wEffectiveUnitCount > (capacity()-1))
                {
                fprintf(stderr,
                        "%s>> Error <%s> Capacity of utftemplateString overflow: requested %d while capacity is %ld . String truncated.\n",
                        _GET_FUNCTION_NAME_,
                        decode_ZStatus(ZS_FIELDCAPAOVFLW),
                        wEffectiveUnitCount,
                        UnitCount);
                wUnitCount=wEffectiveUnitCount=UnitCount-1 ;
                errno=ENOMEM;
                }
  int wi=0;
  _Utf* wPtrOut=content;
  _Utf* wPtrIn=(_Utf*)pURFDataPtr;
  if (sizeof(_Utf)==1)
      {
      while (wi++<wEffectiveUnitCount)
          *wPtrOut++=*wPtrIn++ ;
      }
    else
      {
      while (wi++<wEffectiveUnitCount) // checked
          *wPtrOut++=reverseByteOrder_Conditional<_Utf>(*wPtrIn++) ;
      }
    pURFDataPtr += wi * sizeof(_Utf);
    wOffset += wi * sizeof(_Utf);
    return  ssize_t(wOffset);
}// _importURF

/**  Routine to get URF data header information for an utfxxFixedString or utfxxVaryingString. */
ZStatus _getutfStringURFData(const unsigned char* pURFDataPtr,ZTypeBase& pZType,uint16_t &pCanonical,uint16_t &pUniversalSize);


template <size_t _Sz,class _Utf>
/**
 * @brief _exportUVF  Exports a fixed string to a Universal Varying Format
 *  Universal Varying  Format stores fixed string data into a varying length string container excluding '\0' character terminator
 *  leaded by an uint16_t mentionning the number of character units of the string that follows.
 * 
 * @param pUniversal a ZDataBuffer with string content in Varying Universal Format
 * @return 
 */

ZDataBuffer utftemplateString<_Sz, _Utf>::_exportUVF() const
{
    ZDataBuffer wUVF;

/* Count effective char units excluding (_Utf)'\0' mark */
    UVF_Size_type wUnitCount = utfStrlen<_Utf>(content);

    wUVF.allocate((wUnitCount*sizeof(_Utf))+sizeof(UVF_Size_type)+1);

    unsigned char* wPtrTarg=wUVF.Data;

    /* set export data with char unit size */
    *wPtrTarg = (uint8_t)sizeof (_Utf);
    wPtrTarg++;

    /* prepare and set unit count to export data */
    UVF_Size_type wUnitCount_Export=reverseByteOrder_Conditional<UVF_Size_type>(wUnitCount);
    memmove(wPtrTarg,&wUnitCount_Export,sizeof(UVF_Size_type));
    wPtrTarg += sizeof(UVF_Size_type);


    /* export char units : each char unit must be reversed if necessary (big /little endian) */

    _Utf* wPtrOut=(_Utf*)(wPtrTarg);
    const _Utf* wPtrIn=content;

    if ((sizeof (_Utf)==1)||(!is_little_endian()))
      {
      while (*wPtrIn)
        *wPtrOut++=*wPtrIn++;
      }
    else
      while (*wPtrIn)
        *wPtrOut++=reverseByteOrder_Conditional<UVF_Size_type>(*wPtrIn++);

    return wUVF;
}// _exportUVF

template <size_t _Sz,class _Utf>
ZDataBuffer& utftemplateString<_Sz, _Utf>::_exportAppendUVF(ZDataBuffer& pUVF) const
{

  /* Count effective char units excluding (_Utf)'\0' mark */
  UVF_Size_type wUnitCount = utfStrlen<_Utf>(content);


  /* allocate additional storage for exporting  (plus one byte for security) */
  unsigned char* wPtrTarg=pUVF.extendBZero((wUnitCount*sizeof(_Utf))+sizeof(UVF_Size_type)+1);

  /* set export data with char unit size */
  *wPtrTarg = (uint8_t)sizeof (_Utf);
  wPtrTarg++;

  /* prepare and set unit count to export data */
  UVF_Size_type wUnitCount_Export=reverseByteOrder_Conditional<UVF_Size_type>(wUnitCount);
  memmove(wPtrTarg,&wUnitCount_Export,sizeof(UVF_Size_type));
  wPtrTarg += sizeof(UVF_Size_type);


  /* export char units : each char unit must be reversed if necessary (big /little endian) */

  _Utf* wPtrOut=(_Utf*)(wPtrTarg);
  const _Utf* wPtrIn=content;

  if ((sizeof (_Utf)==1)||(!is_little_endian()))
  {
    while (*wPtrIn)
      *wPtrOut++=*wPtrIn++;
  }
  else
    while (*wPtrIn)
      *wPtrOut++=reverseByteOrder_Conditional<UVF_Size_type>(*wPtrIn++);

  return pUVF;
}// _exportUVF

template <size_t _Sz,class _Utf>
/**
 * @brief _getexportUVFSize() compute the requested export size IN BYTES for current string
 */
UVF_Size_type
utftemplateString<_Sz,_Utf>::_getexportUVFSize()
{
  /* Count char units excluding (_Utf)'\0' mark starting from end */
  return UVF_Size_type((utfStrlen<_Utf>(content)*sizeof(_Utf)) +sizeof(UVF_Size_type)+1);

}//_getimportUVFSize

template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::_importVUniversal Import string from Varying Universal Format
 *  Varying Universal Format stores string data into a varying length string container excluding '\0' character terminator 
 *  leaded by a uint16_t mentionning the number of characters of the string that follows.
 * 
 * @param pUniversalPtr pointer to Varying Universal formatted data header
 * @return total size of consumed bytes in pUniversalPtr buffer (Overall size of string in UVF)
 */
size_t
utftemplateString<_Sz,_Utf>::_importUVF(unsigned char*& pPtrIn)
{
  unsigned char* wPtrSrc=pPtrIn;
  /* get and control char unit size */
  uint8_t wUnitSize=(uint8_t)*pPtrIn;
  if (wUnitSize!=sizeof(_Utf))
  {
    fprintf(stderr,"_importUVF-E-IVUSIZE Imported string format <%s> does not correspond to current string format <%s>",
        getUnitFormat(wUnitSize),
        getUnitFormat(sizeof(_Utf)));
    return 0;
  }

  pPtrIn++;
  /* get char units to load excluding (_Utf)'\0' mark */
  UVF_Size_type    wUnitCount;

  memmove(&wUnitCount, pPtrIn ,sizeof(UVF_Size_type));
  wUnitCount=reverseByteOrder_Conditional<UVF_Size_type>(wUnitCount);
  pPtrIn += sizeof(UVF_Size_type);

  /* check capacity */
  if (wUnitCount >= _Sz)
  {
    fprintf(stderr,"_importUVF-W-CAPOVFL Fixed string capacity overflow : importing <%d> char unit while capacity is <%d>. Truncating imported string.",
        wUnitCount,
        _Sz);
    wUnitCount= _Sz-1;
  }

  /* allocate size of string in char units */
//  allocateUnits(wUnitCount+1); // remember that '\0' is not exported

  /* import string per char unit */

  _Utf* wPtrOut=content ;
  _Utf* wPtrIn=(_Utf*)(pPtrIn);
  _Utf* wPtrEnd = &wPtrIn[wUnitCount];
  if (wUnitSize==1)
    while (pPtrIn < wPtrEnd)
      *wPtrOut++=*pPtrIn++;
  else
    while (wPtrIn < wPtrEnd)
      *wPtrOut++=reverseByteOrder_Conditional<UVF_Size_type>(*wPtrIn++);
//  addConditionalTermination();
  *wPtrOut = 0;

  pPtrIn += (wUnitCount*sizeof(_Utf)) + 1;
  return (wUnitCount*sizeof(_Utf))+sizeof(UVF_Size_type)+1;
}// _importUVF

template <size_t _Sz,class _Utf>
/**
 * @brief _getexportUVFSize() compute the requested export size IN BYTES for current string
 */
UVF_Size_type
utftemplateString<_Sz,_Utf>::_getimportUVFSize(unsigned char* pUniversalPtr)
{
  unsigned char* wPtrSrc=pUniversalPtr;
  /* get and control char unit size */
  uint8_t wUnitSize=(uint8_t)*wPtrSrc;
  wPtrSrc++;

  UVF_Size_type    wUnitCount;

  memmove(&wUnitCount, wPtrSrc,sizeof(UVF_Size_type));
  wUnitCount=reverseByteOrder_Conditional<UVF_Size_type>(wUnitCount);
  wUnitCount *= wUnitSize ;
  wUnitCount += sizeof(UVF_Size_type) + 1 ;
  return wUnitCount;

}//_getimportUVFSize

template <size_t _Sz,class _Utf>

ZStatus
utftemplateString<_Sz,_Utf>::getUniversalFromURF(ZTypeBase pType,const unsigned char* pURFDataPtr,ZDataBuffer &pUniversal,const unsigned char** pURFDataPtrOut)
{
URF_UnitCount_type wUnitSize , wEffectiveUSize;
ZTypeBase wType;
    const unsigned char*wDataPtr=pURFDataPtr;
    memmove(&wType,wDataPtr,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    if (wType!=pType)
        {
        fprintf (stderr,
                 "%s>> Error invalid URF data type <%X> <%s> while getting universal value of <%s> ",
                 _GET_FUNCTION_NAME_,
                 wType,
                 decode_ZType(wType),
                 decode_ZType(pType));
        return ZS_INVTYPE;
        }
    wDataPtr += sizeof (ZTypeBase);

    memmove (&wUnitSize,wDataPtr,sizeof(wUnitSize));
    wUnitSize=reverseByteOrder_Conditional<URF_Capacity_type>(wUnitSize);
    wDataPtr += sizeof (URF_Capacity_type);

    memmove (&wEffectiveUSize,wDataPtr,sizeof(wEffectiveUSize));
    wEffectiveUSize=reverseByteOrder_Conditional<URF_UnitCount_type>(wEffectiveUSize);
    wDataPtr += sizeof (URF_UnitCount_type);

    pUniversal.allocateBZero(wUnitSize);       // fixed string must have canonical characters count allocated
    memmove(pUniversal.DataChar,wDataPtr,(size_t)wEffectiveUSize); // effective number of char is effective universal size
                                                                    // without '\0' terminator
    if (pURFDataPtrOut)
    {
      *pURFDataPtrOut = wDataPtr + wEffectiveUSize;
    }
    return ZS_SUCCESS;
}//getUniversalFromURF



template <size_t _Sz,class _Utf>
ZStatus
utftemplateString<_Sz,_Utf>::getUniversalFromURF_Truncated(ZTypeBase pType, const unsigned char *pURFDataPtr, ZDataBuffer &pUniversal, const unsigned char** pURFDataPtrOut)
{
URF_UnitCount_type wUnitNb , wEffectiveUSize;
ZTypeBase wType;
    const unsigned char*wDataPtr=pURFDataPtr;
    memmove(&wType,wDataPtr,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    if (wType!=pType)
        {
        fprintf (stderr,
                 "%s>> Error invalid URF data type <%X> <%s> while getting universal value of <%s> \n",
                 _GET_FUNCTION_NAME_,
                 wType,
                 decode_ZType(wType),
                 decode_ZType(pType));
        return ZS_INVTYPE;
        }
    wDataPtr += sizeof (ZTypeBase);

    memmove (&wUnitNb,wDataPtr,sizeof(wUnitNb));
    wUnitNb=reverseByteOrder_Conditional<URF_Capacity_type>(wUnitNb);
    wDataPtr += sizeof (URF_Capacity_type);

    memmove (&wEffectiveUSize,wDataPtr,sizeof(wEffectiveUSize));
    wEffectiveUSize=reverseByteOrder_Conditional<URF_UnitCount_type>(wEffectiveUSize);
    wDataPtr += sizeof (URF_UnitCount_type);
    pUniversal.allocateBZero((size_t)wEffectiveUSize+sizeof(_Utf));
    _Utf*wPtr= (_Utf*)pUniversal.Data;
    _Utf*wPtrIn= (_Utf*)wDataPtr;
    size_t wUnits=wEffectiveUSize/sizeof(_Utf);
    while (wUnits--)
            {
            *wPtr++=*wPtrIn++;
            }
    *wPtr=(_Utf)__ENDOFSTRING__;

    if (pURFDataPtrOut)
      {
      *pURFDataPtrOut = wDataPtr + wEffectiveUSize;
      }

    return ZS_SUCCESS;
}//getUniversalFromURF_Truncated

//=========================== Conversion to and from=========================




//-----------End character encoding & decoding-----------------------------------------

/* Basic string routines */


template <size_t _Sz,class _Utf>

utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::strset ( const _Utf *wSrc)
{
    if (!wSrc)
            return *this;

    _Utf*   wPtr=content;
    _Utf*   wEndPtr=content+(_Sz-1); /* take care of endofstring sign */

    while (*wSrc && (wPtr < wEndPtr))
    {
        *wPtr=*wSrc;
        wPtr++;
        wSrc++;
    }
    *wPtr=(_Utf)'\0';
    return *this;
}


template <size_t _Sz,class _Utf>

utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::strnset ( const _Utf *wSrc, size_t pCount)
{
    if (!wSrc)
            return *this;
    size_t wi=0;
    _Utf*wPtr=content;
    _Utf*   wEndPtr=content+(_Sz-1); /* take care of endofstring sign */
    for (;*wSrc && pCount-- && (wPtr < wEndPtr);wi++)
                            *wPtr++=*wSrc++;
    while (wi++ < getUnitCount())
            *wPtr++ =0;

    return *this;
}// strnset


template <size_t _Sz,class _Utf>

utftemplateString<_Sz, _Utf> &
utftemplateString<_Sz, _Utf>::add(const _Utf *wSrc)
{
    if (!wSrc)
            return *this;
    size_t wCount=0;
    _Utf* wPtr=content;
    while (*wPtr && (wCount++ < _Sz-1))
            wPtr++;

    while(*wSrc && (wCount++ <_Sz-1) )
                    *wPtr++=*wSrc++;

    *wPtr=(_Utf)'\0';
    return *this;
}// add

template <size_t _Sz,class _Utf>
utftemplateString<_Sz, _Utf> &
utftemplateString<_Sz, _Utf>::add(const utftemplateString<_Sz, _Utf> &pSrc)
{
    size_t wCount=0;
    _Utf* wPtr=content;
    while (*wPtr++ && wCount++< _Sz) ;

    const _Utf* wSrc=pSrc.content;
    while(*wSrc && (wCount++ <_Sz) )
                    *wPtr++=*wSrc++;
    while (wCount++ <_Sz)
            *wPtr=(_Utf)'\0';
    return *this;
}// add


template <size_t _Sz,class _Utf>

utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::nadd( const _Utf *wSrc, size_t pCount)
{
    if (!wSrc)
            return *this;
    size_t wCurSize=0;
    _Utf* wPtr=content;
    while (*wPtr)
        {
        wPtr++;
        wCurSize++;
        }
    while(*wSrc && (wCurSize++ < _Sz) && (pCount--))
                    *wPtr++=*wSrc++;
    while (wCurSize++ < _Sz)
                    *wPtr=(_Utf)'\0';
    return *this;
}// nadd

template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::nadd_Char( const char *wSrc, size_t pCount)
{
    if (!wSrc)
            return *this;
    size_t wCurSize=0;
    _Utf* wPtr=content;
    while (*wPtr)
        {
        wPtr++;
        wCurSize++;
        }
    while(*wSrc && (wCurSize++ < _Sz) && (pCount--))
                    *wPtr++=(_Utf)*wSrc++;
    while (wCurSize++ < _Sz)
                    *wPtr=(_Utf)'\0';
    return *this;
}// nadd_char

template<size_t _Sz, class _Utf>
utftemplateString<_Sz, _Utf> &utftemplateString<_Sz, _Utf>::add_Char(const char *wSrc)
{
    size_t pCount = _Sz;
    if (!wSrc)
        return *this;
    size_t wCurSize=0;
    _Utf* wPtr=content;
    while (*wPtr)
    {
        wPtr++;
        wCurSize++;
    }
    while(*wSrc && (wCurSize++ < _Sz) && (pCount--))
        *wPtr++=(_Utf)*wSrc++;
    while (wCurSize++ < _Sz)
        *wPtr=(_Utf)'\0';
    return *this;
}// nadd_char

template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::addUtfUnit(const _Utf pChar)
{

   size_t wS= utfStrlen<_Utf>(content);
   if (wS >=_Sz-2)
       {
       fprintf(stderr,"utftemplateString::addUtfUnit-E-ZS_OUTBOUND  Out of string boundaries\n");
       return *this;
       }
   content[wS]=(_Utf)pChar;
   wS++;
   content[wS]=(_Utf)'\0';
   return (*this);
}
template <size_t _Sz,class _Utf>
_Utf*
utftemplateString<_Sz,_Utf>::fill(const _Utf pChar,const size_t pStart,const ssize_t pCount) /** fills the content from pStart on pCount with chararcter pChar  */
{
    ssize_t wCount=(pCount>0)?(pCount):(getUnitCount()-pStart-1);
    if (pStart>=UnitCount)
                    return nullptr;
     if((pStart+wCount)>=_Sz)
            wCount= sizeof(content)-1-pStart;
     _Utf* wPtr=&content[pStart];
     while (wCount--)
            *(wPtr++)=pChar;
     *(wPtr)= (_Utf)__ENDOFSTRING__;
     return content;
}

/*  comparaison routines */
template <size_t _Sz,class _Utf>
inline int
utftemplateString<_Sz,_Utf>::compare(const _Utf* pString2) const
{
    if (content==pString2)
                return 0;
    if (pString2==nullptr)
                return 1;

    const _Utf *s1 = (const _Utf *) content;
    const _Utf *s2 = (const _Utf *) pString2;
    while ((*s1 == *s2 )&&(*s1)&&(*s2))
        {
        s1++;
        s2++;
        }
    return *s1 - *s2;

}/** corresponds to strcmp */
/*
template <size_t _Sz,class _Utf>
inline int
utftemplateString<_Sz,_Utf>::compare(const char* pString2)
{
    _Utf wCChar;
    if (pString2==nullptr)
                return 1;

    const _Utf *s1 = (const _Utf *) content;
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
*//** corresponds to strcmp */



template <size_t _Sz,class _Utf>
inline int
utftemplateString<_Sz,_Utf>::ncompare(const _Utf* pString2,size_t pCount)
{
    const _Utf* wPtr=content;
    if (content==pString2)
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

template <size_t _Sz,class _Utf>
inline int
utftemplateString<_Sz,_Utf>::ncompare(const char* pString2,size_t pCount)
{
    const _Utf* wPtr=content;
    if (content==(const _Utf*)pString2)
                return 0;
    if (pString2==nullptr)
                return 1;

    int wCount = 0 ;
    int32_t wComp = (int32_t)(*wPtr - (int32_t)*pString2);
    while ((!wComp)&&(*wPtr)&&(*pString2)&&(wCount < pCount))
        {
        wPtr++;
        pString2++;
        wCount++;
        wComp = (int32_t)(*wPtr - (int32_t)*pString2);
        }

    if (wCount==pCount)
            return wComp;
    if (wComp)
            return wComp ;
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

template <size_t _Sz,class _Utf>
bool
utftemplateString<_Sz,_Utf>::isEqualCase (const _Utf *pCompare)
{

  _Utf* wPtr1=content;
  const _Utf* wPtr2=pCompare;

  int wS=capacity();

  while (*wPtr1 && *wPtr2 && (utfUpper(*wPtr1++)==utfUpper(*wPtr2++))&& wS--) ;
  if ((*wPtr1==0) && (*wPtr2==0))
    return true;
  return false;
}
template <size_t _Sz,class _Utf>
bool
utftemplateString<_Sz,_Utf>::isEqualCase (const utftemplateString<_Sz,_Utf>& pCompare)
{

  _Utf* wPtr1=content;
  _Utf* wPtr2=pCompare.content;
  int wSize=capacity();
  while (*wPtr1 && *wPtr2 && (utfUpper(*wPtr1++)==utfUpper(*wPtr2++)) && wSize-- ) ;

  if ((*wPtr1==0) && (*wPtr2==0))
    return true;
  return false;
}




#ifdef __COMMENT__
template <size_t _Sz,class _Utf>
/**
 * @brief sprintf set current fixed string content to a formatted _Utf* string (Warning: char is a special case).
 *
 * To avoid collision with sprintf overload for utftemplateString<_Sz,char>, pFormat is conditionned to be set to utf8_t when char is second template class and set to char in other cases.
 * This allows for utf string to have a format string that is a const char*, while it is the default when using char as second template class.
 *
 * Resulting formatted string cannot exceed string boundaries after concatenation.
 * In this case, formatted string is truncated without any information.
 * @param pFormat contains formal sprintf syntax (extended for _Utf usage) :
 *          either a const _Utf* string when template class has _Utf equal to utf8_t, utf16_t or utf32_t
 *          or a const utf8_t* string when template class has _Utf equal to char (to avoid invalid overload).
 * @param varying argument list
 * @return utftemplateString object after operation
 */
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::sprintf(const typename std::conditional<std::is_same<_Utf,char>::value,char,_Utf> * pFormat,...) /** sets currents string content with a formatted content.Maximum size is capacity of template string */

{
    if (!pFormat)
            {
            fprintf(stderr,"%s>> format string is nullpptr\n",_GET_FUNCTION_NAME_);
            return *this;
            }

    va_list ap;
    va_start(ap, pFormat);
    size_t wi=utfVsnprintf<_Utf>(content,_Sz,pFormat,ap);
    va_end(ap);
    while (wi < _Sz)
            content[wi++]=(_Utf)'\0';

    return *this;
}// sprintf with _Utf* format


/**
 * @brief utftemplateString<_Sz, _Utf>::addsprintf concatenates a _Utf formatted string (pFormat) to existing string, defined by its _Utf format and its varying arguments list.
 *
 * To avoid collision with addsprintf overload for utftemplateString<_Sz,char>, pFormat is conditionned to be set to utf8_t when char is second template class and set to char in other cases.
 * This allows for utf string to have a format string that is a const char*, while it is the default when using char as second template class.
 * Resulting formatted string cannot exceed string boundaries after concatenation.
 * In this case, formatted string is truncated without any information.
 * String is padded to zero after concatenation.
 *
 * @param pFormat contains formal sprintf syntax (extended for _Utf usage) :
 *          either a const _Utf* string when template class has _Utf equal to utf8_t, utf16_t or utf32_t
 *          or a const utf8_t* string when template class has _Utf equal to char (to avoid invalid overload)
 * @return utftemplateString object after operation
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
//utftemplateString<_Sz,_Utf>::addsprintf(const typename std::enable_if_t<!std::is_same<_Utf,char>::value,_Utf>* pFormat,...)
utftemplateString<_Sz,_Utf>::addsprintf(const typename std::conditional<std::is_same<_Utf,char>::value,char,_Utf>* pFormat,...)
{
    if (!pFormat)
            {
            fprintf(stderr,"%s>> format string is nullpptr\n",_GET_FUNCTION_NAME_);
            return *this;
            }
    va_list ap;
    va_start(ap, pFormat);

    _Utf wMS[_Sz];
    size_t wStringCount=(size_t)utfVsnprintf<_Utf>(wMS,_Sz,pFormat,ap);
    va_end(ap);

    size_t wCount=0;
    _Utf* wPtr= content;

    while (*wPtr++)
                wCount++;

    _Utf* wAddPtr= wMS;

    for (;*wAddPtr && (wCount < _Sz);wCount++)
                                *wPtr++=*wAddPtr++;
    while (wCount++ <_Sz)
            *wPtr++=(_Utf)'\0';

    return *this;
}//addsprintf with _Utf* format

#endif //__COMMENT__

template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::sprintf(const char* pFormat,...)  /** set curent string content to given formatted content. Eventually extends characters allocation to make string fit */
{
    va_list ap;
    va_start(ap, pFormat);
    size_t wi=utfVsnprintf(Charset,content,_Sz,(utfFmt*)pFormat,ap);
    va_end(ap);

    return *this;
}//sprintf with char* format


template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::addsprintf(const char* pFormat,...)
{
    va_list ap;
    va_start(ap, pFormat);


    _Utf* wPtr= content;
    size_t wCount=0;
    while (*wPtr++)
                wCount++;
    wPtr--;
    size_t wStringCount=utfVsnprintf(Charset, wPtr,_Sz-wCount,(utfFmt*)pFormat,ap);
    va_end(ap);

    return *this;
} // addsprintf with char* format

#ifdef __COMMENT__// to be put at a higher level because needs conversion from charset to another
template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz, _Utf>::addCString adds (concatenates) pInString as a char* string to existing string.
 *  pInString is supposed to be US-ASCII or at least, unicode compatible one byte per char string.
 *  Boundaries of fixed string are preserved.
 *  content is padded with zeroes until capacity
 * @param pInString
 * @return
 */
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::addCString(const char* pInString)
{
    size_t wi;
    for (wi=strlen();*pInString && (wi<_Sz);wi++,pInString++)
                            content[wi]=(_Utf)*pInString;
    while (wi<_Sz)
        content[wi++]=(_Utf)'\0';
    return *this;
}//addCString




template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::setCharSet changes the default charset for the current utftemplateString
 * @param pCharset
 * @return
 */
ZStatus
utftemplateString<_Sz,_Utf>::setCharSet(const ZCharset_type pCharset)
{

    if (charsetToICU(pCharset)==ZCHARSET_NOTSUPPORTED)
                return ZS_INVCHARSET;
    Charset=pCharset;
    return  ZS_SUCCESS;
}//setCharSet
#endif// __COMMENT__

template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::fromISOLatin1(const unsigned char* pString, size_t pInSize,ZStatus* pStatus)
{

    *pStatus =_fromISOLatin1(*this,pString,pInSize);
    return  *this;
}// fromISOLatin1


template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>*
utftemplateString<_Sz,_Utf>::toISOLatin1(utftemplateString<_Sz,_Utf>* pString, ZStatus* pStatus)
{

   * pStatus =_toISOLatin1(*this,pString->content,pString->UnitCount);
   return  *this;
}// fromISOLatin1


template <size_t _Sz,class _Utf>
double utftemplateString<_Sz,_Utf>::toDouble()
{
    return utfStrtod<utf8_t>(content,nullptr);
}
template <size_t _Sz,class _Utf>
int utftemplateString<_Sz,_Utf>::toInt(int pBase)
{
    return utfStrtoi<utf8_t>(content,nullptr,pBase);
}
template <size_t _Sz,class _Utf>
int utftemplateString<_Sz,_Utf>::toLong(int pBase)
{
    return utfStrtol<utf8_t>(content,nullptr,pBase);
}

//-------------End utftemplateString Methods expansion---------------------------------------







//=================END Base Template utftemplateString SECTION=============================








//-------------End utftemplateString Methods expansion---------------------------------------
//} // namespace zbs




#endif // UTFTEMPLATESTRING_H

