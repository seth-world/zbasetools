#ifndef TEMPLATEWSTRING_H
#define TEMPLATEWSTRING_H
#include <zconfig.h>

#include <ztoolset/zmem.h>
#include <ztoolset/zlimit.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zerror.h>
#include <ztoolset/zdatabuffer.h>

#include <string.h>
#include <stdlib.h>
#include <wchar.h>

#include <ztoolset/zatomicconvert.h>

//#include <ztoolset/zfunctions.h>  // cannot include zfunctions.h there
wchar_t* findWLastNonChar(const wchar_t *pContent,wchar_t pChar);
wchar_t *findWLastNonChar(const wchar_t *pContent,const wchar_t* pChar);

bool isWTerminatedBy(wchar_t *pContent,const wchar_t* pChar);
bool isWTerminatedBy(const wchar_t *pContent,wchar_t pChar);
void conditionalWNL(wchar_t *pString,const ssize_t pSizeMax);

const wchar_t *_WfirstNotinSet(const wchar_t *pString, const wchar_t *pSet);
const wchar_t *_WfirstinSet(const wchar_t *pString, const wchar_t *pSet);
const char * _WfirstNotinSet (const char*pString,const char *pSet);
const wchar_t *_WlastinSet(const wchar_t *pStr, const wchar_t *pSet);

wchar_t *_WLTrim(wchar_t *pString, const wchar_t *pSet);
wchar_t *_WRTrim(wchar_t *pString, const wchar_t *pSet);
wchar_t *_WTrim(wchar_t *pString, const wchar_t *pSet);

const wchar_t *_WstrchrReverse(const wchar_t *pStr, const wchar_t pChar);
wchar_t * _WtoUpper(wchar_t *pStr,wchar_t *pOutStr);
wchar_t *_WexpurgeSet(wchar_t *pString, const wchar_t *pSet);
wchar_t *_WexpurgeString(wchar_t *pString, const wchar_t *pSubString);

ZStatus CStringtoWString(wchar_t* pOutWString,const size_t pOutArrayCount,const char* pInCString,const size_t pInArrayCount);
ZStatus WStringtoCString(char* pOutWString,const size_t pOutArrayCount,const wchar_t* pInCString,const size_t pInArrayCount);

#ifndef __WDEFAULTDELIMITERSET__
#define __WDEFAULTDELIMITERSET__  L" \t\n\r"

#endif // __WDEFAULTDELIMITERSET__

#include <locale.h>
#ifndef TEMPLATEWSTRING_CPP
    extern const char* GLocale;
#endif

class templatePWString
{
public:
    const size_t    _capacity;
    ZCryptMethod_type   CryptMethod = ZCM_Uncrypted;
    wchar_t         *content;                      //!< main string fixed size content
    size_t capacity(void) {return _capacity;}
    size_t  size(void) {return (wcslen(content)); }
    size_t getByteSize(void) {return(wcslen(content)*sizeof(wchar_t)); }
    size_t getByteCapacity(void) {return(_capacity*sizeof(wchar_t)); }
};
template <size_t _Sz>
class templateWString
{
public:
const size_t    _capacity=_Sz;
ZCryptMethod_type   CryptMethod = ZCM_Uncrypted ;//!< defines wether content is encrypted or not, and what encryption method has been used.
wchar_t         content[_Sz];                    //!< main string fixed size content

    templateWString(void) {templateWString::clear();setlocale(LC_ALL, GLocale);}
    templateWString(char* pString) {setlocale(LC_ALL, GLocale);templateWString::fromString(pString);}

    virtual ~templateWString(void) {}

    templateWString(const wchar_t *pFormat,...)
    {
        templateWString::clear();
        va_list args;
        va_start (args, pFormat);
        vswprintf (content ,_capacity,pFormat, args);
        va_end(args);
        return;
    }

    size_t capacity(void) {return _capacity;}       // returns capacity in number of characters (wchar_t)
    size_t  size(void) {return (wcslen(content)); } // return size of contained string in characters (L'\0' excluded)
    size_t getByteSize(void) {return(wcslen(content)*sizeof(wchar_t)); } // return size of string in bytes (L'\0' excluded)
    size_t getByteCapacity(void) {return(_capacity*sizeof(wchar_t)); } // return capacity in bytes


    ZDataBuffer *_exportURF(ZDataBuffer *pUniversal);
    templateWString& _importURF(unsigned char* &pUniversal);

    static ZStatus getUniversalFromURF(unsigned char *pURFDataPtr, ZDataBuffer& pUniversal);
    static ZStatus getUniversalFromURF_Truncated(unsigned char* pURFDataPtr,ZDataBuffer& pUniversal);

    virtual bool isEmpty(void) {return (content[0]==L'\0');}
    virtual void clear(void) {memset(content,0,sizeof(content));}

    ZDataBuffer & toVaryingCString(ZDataBuffer &pOutCString) ;
    virtual wchar_t * toWString(void) {content[_capacity-1]=L'\0';return(content);}
    virtual int toDouble(void) {return(wcstod(content,nullptr));}
    virtual long toLong(int pBase=10) {return(wcstol(content,nullptr,pBase));}
    virtual long toULong(int pBase=10) {return(wcstoul(content,nullptr,pBase));}

    virtual wchar_t* fill(wchar_t pWChar,size_t pSize)
                {size_t wSize=pSize;
                 if(wSize>=_capacity)
                        wSize= _capacity-1;
                 for (long wi=0;wi<pSize;wi++)
                                content[wi]=pWChar;
                 content[pSize]=L'\0';
                return content;
                }

    void
    sprintf (const wchar_t *pFormat,...)
    {
    va_list args;
    va_start (args, pFormat);
    vswprintf (content ,_capacity,pFormat, args);
    va_end(args);
    return;
    }
    /**
     * @brief _sprintf to be used when cascading from another varying arguments (va_list) function
     * @param pFormat
     * @param pArglist
     */
    void
    _sprintf (const wchar_t *pFormat,va_list pArglist)
    {
    vswprintf (content ,_capacity,pFormat, pArglist);
    return;
    }

    virtual templateWString<_Sz> & conditionalNL(void)
    {
        if (!isWTerminatedBy(content,L"\n"))
                               wcsncat(content,L"\n",_capacity);
        return *this;
    }
    virtual templateWString<_Sz> & addConditionalTermination(void)
    {
        if (content[_Sz-1]!=(wchar_t)L'\0')
                               content[_Sz-1]=(wchar_t)L'\0';
        return *this;
    }

    virtual templateWString<_Sz> &fromString(const wchar_t *pString) ;
    virtual templateWString<_Sz>& fromStringCount(const wchar_t *pString, const int pCount) ;
    templateWString<_Sz>& fromInt(const int pInt) { sprintf(content,L"%d",pInt);
                                                           return(*this);}
    virtual templateWString<_Sz>& fromLong(const int pLong) { sprintf(content,L"%ld",pLong); return(*this);}
    virtual templateWString<_Sz>& fromULong(const int pLong) { sprintf(content,L"%uld",pLong); return(*this);}
//    virtual templateWString<_Sz>& fromtemplateWString(const templateWString<_Sz> &pInString) ;
//    virtual templateWString<_Sz>& fromtemplateWString(const templateWString<_Sz> *pInString) ;


// ---------String content edition----------------
    virtual templateWString<_Sz> &LTrim(const wchar_t *pSet=__WDEFAULTDELIMITERSET__);

    virtual const wchar_t *firstNotinSet(const wchar_t *pSet=__WDEFAULTDELIMITERSET__);
    virtual templateWString<_Sz> &RTrim(const wchar_t *pSet=__WDEFAULTDELIMITERSET__);
    virtual templateWString<_Sz> &Trim(const wchar_t *pSet=__WDEFAULTDELIMITERSET__);

    virtual const wchar_t* strchrReverse(const wchar_t pChar);
    virtual const wchar_t *strspnReverse(const  wchar_t *pSet);

    virtual bool isSpace(void) {return(_WfirstNotinSet(content,__WDEFAULTDELIMITERSET__)==nullptr);}


    virtual bool startsCaseWith(const wchar_t* pString,const wchar_t* pSet=__WDEFAULTDELIMITERSET__);
    virtual bool startsWith(const wchar_t *pString, const wchar_t* pSet=__WDEFAULTDELIMITERSET__);
    virtual bool startsWithApprox(const wchar_t* pString, ssize_t pApprox, const wchar_t* pSet=__WDEFAULTDELIMITERSET__);
    virtual bool startsCaseWithApprox(const wchar_t *pString, int pApprox, const wchar_t *pSet=__WDEFAULTDELIMITERSET__);

    virtual int Substr(const wchar_t *pSub, int pOffset=0);
    virtual int SubstrCase(const wchar_t *pSub, int pOffset=0);
    virtual templateWString<_Sz> Left (int pLen);
    virtual templateWString<_Sz> Right (int pLen);

    virtual templateWString<_Sz> & add(const wchar_t *pFormat,...) ;
    virtual templateWString<_Sz> & eliminateChar (wchar_t pChar);

    virtual bool    contains (const wchar_t* pString) {return (wcsstr(content,pString)!=nullptr);}
    virtual bool    containsCase (const wchar_t* pString)
    {
    templateWString<_Sz> wThisString;
    templateWString<_Sz> wString;
        wThisString = content;
        wThisString.toUpper();
        wString = pString;
        wString.toUpper();
        return (wcsstr(wThisString.content,wString.content)!=nullptr);
    }
    virtual bool    contains (templateWString<_Sz> &pString) {return (wcsstr(content,pString.content)!=nullptr);}
    virtual bool    containsCase (templateWString<_Sz> &pString)
    {
    templateWString<_Sz> wThisString;
    templateWString<_Sz> wString;
        wThisString = content;
        wThisString.toUpper();
        wString = pString;
        wString.toUpper();
        return (wcsstr(wThisString.content,wString.content)!=nullptr);
    }

    virtual size_t locate(const wchar_t *pString);
    virtual size_t locate(templateWString<_Sz> pString);

    virtual templateWString<_Sz>& toUpper(void);
    virtual templateWString<_Sz>& toUpper(templateWString<_Sz>& pOut);

//------------B64------------------------------------

    ZDataBuffer *encryptB64(void);
    templateWString<_Sz> & uncryptB64(ZDataBuffer &pZBD64);

//------------MD5------------------------------------
    md5 getMD5(void);
//-----------Java HashCode---------------------------
    long hashCode(void);

//    ZDataBuffer& toVaryingCString (ZDataBuffer &pOutCString);
    templateWString& fromVaryingCString (ZDataBuffer &pInCString);

    templateWString& fromCString_Ptr (const char* pInCString);
    templateWString& fromCString_PtrCount (const char* pInCString,size_t pCount);
//------------------operator =  ------------------------------------------------
//

    virtual templateWString<_Sz>& operator = (const wchar_t *pString)
    {
        templateWString<_Sz>::fromString(pString);
        return (*this);
    }

    virtual templateWString<_Sz>& operator = (const wchar_t pChar)
    {
        content[0]=pChar;
        content[1]='\0';
        return *this;
    }
    virtual templateWString<_Sz>& operator += (const wchar_t pChar)
    {

        size_t wS= size();
        if (wS>=_capacity-2)
            {
            fprintf(stderr,"templateWString-F-ZS_OUTBOUND  Out of string boundaries\n");
            abort();
            }
        content[wS]=pChar;
        wS++;
        content[wS]=L'\0';
        return *this;
    }
//    virtual templateWString<_Sz>& operator = (char *pString) {return (fromString(pString));}
    virtual templateWString<_Sz>& operator = (templateWString<_Sz>& pString) {memmove (content,pString.content,sizeof(content)); return (*this);}
//    templateWString<_Sz>& operator = (const codeString &pString) {return(fromcodeString(pString));}
    virtual templateWString<_Sz>& operator += (templateWString<_Sz> &pString)
    {
    ssize_t wSize = _capacity - (size()+pString.size());
    if (wSize<0)
            wSize=_capacity -size();
        wcsncat (content,pString.content,(size_t)wSize);
        return(*this);
    } // +=

    virtual templateWString<_Sz>& operator += (const wchar_t * pString)
    {
        ssize_t wSize = _capacity - (size()+wcslen(pString));
        if (wSize<0)
                wSize=_capacity -size();
        wcsncat (content,pString,(size_t)wSize);
        return(*this);
    }
    virtual templateWString<_Sz>& operator += (wchar_t * pString)
    {
        ssize_t wSize = _capacity - (size()+wcslen(pString));
        if (wSize<0)
                wSize=_capacity -size();
        wcsncat (content,pString,(size_t)wSize);
        return(*this);
    }


    virtual bool operator == (const templateWString<_Sz> &pCompare) { return(wcsncmp(content,pCompare.content,_capacity)==0);}
    virtual bool operator != (const templateWString<_Sz> &pCompare) { return(!wcsncmp(content,pCompare.content,_capacity)==0);}
    virtual bool operator > (const templateWString<_Sz> &pCompare) { return(wcsncmp(content,pCompare.content,_capacity)>0);}
    virtual bool operator < (const templateWString<_Sz> &pCompare) { return(wcsncmp(content,pCompare.content,_capacity)<0);}

    virtual bool operator == (const wchar_t *pCompare) { return(wcsncmp(content,pCompare,_capacity)==0);}
    virtual bool operator != (const wchar_t *pCompare) { return!(wcsncmp(content,pCompare,_capacity)==0);}
    virtual bool operator > (const wchar_t *pCompare) { return(wcsncmp(content,pCompare,_capacity)>0);}
    virtual bool operator < (const wchar_t *pCompare) { return(wcsncmp(content,pCompare,_capacity)<0);}


//--------- Qt methods----------------------------------------------

#ifdef QT_CORE_LIB
    QString toQString(void) {QString wRet; wRet.fromWCharArray(content,_capacity); return wRet;}

    virtual templateWString<_Sz>& fromQString(QString pQString)
    {
        if (pQString.size()>_capacity)
                        pQString.truncate(_capacity-1);
     pQString.toWCharArray(content);
    }

    virtual templateWString<_Sz>& fromQByteArray(QByteArray &pQBA)
    {
        QString wQs = pQBA;
        return fromQString(wQs);
    }

    virtual templateWString<_Sz>& operator = (QString pString) {return(fromQString(pString));}
    virtual templateWString<_Sz>& operator += (QString pString)
    {
    wchar_t wComp[_capacity];
    if (pString.size()+wcslen(content)>=_capacity)
                pString.truncate(_capacity-wcslen(content)-1);
    pString.toWCharArray(wComp);
        wcsncat(content,wComp,( _capacity-size()));
        return(*this);
    }


    virtual bool operator == (QString pCompare)
    {
        wchar_t wComp[_capacity];
        if (pCompare.size()>=_capacity)
                    pCompare.truncate(_capacity-1);
        pCompare.toWCharArray(wComp);
        return(wcsncmp(content,wComp,_capacity)==0);
    }
    virtual bool operator != (QString pCompare)
    {
        wchar_t wComp[_capacity];
        if (pCompare.size()>=_capacity)
                    pCompare.truncate(_capacity-1);
        pCompare.toWCharArray(wComp);
        return(!wcsncmp(content,wComp,_capacity)==0);
    }
    virtual bool operator > (QString pCompare)
    {
        wchar_t wComp[_capacity];
        if (pCompare.size()>=_capacity)
                    pCompare.truncate(_capacity-1);
        pCompare.toWCharArray(wComp);
        return(wcsncmp(content,wComp,_capacity)>0);
    }
    virtual bool operator < (QString pCompare)
    {
        wchar_t wComp[_capacity];
        if (pCompare.size()>=_capacity)
                    pCompare.truncate(_capacity-1);
        pCompare.toWCharArray(wComp);
        return(wcsncmp(content,wComp,_capacity)<0);
    }
#endif//QT



};// struct templateWString---------------------------------------------

//-------------templateWString Methods expansion--------------
//

template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::fromString(const wchar_t *pInString)
{
    templateWString::clear();
    if (pInString==nullptr)
            {
            content[0]=L'\0';
            return *this;
            }
    wcsncpy(content,pInString,_capacity-1);
    return(*this);
}
template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::fromStringCount(const wchar_t* pInString,const int pCount)
{
    templateWString<_Sz>::clear();
    int wCount=pCount;
    if (pCount>(_capacity-1))
                wCount = _capacity-1;

    wcsncpy(content,pInString,wCount);
    content[wCount]=L'\0';
    return(*this);
}

/**
 * @brief templateWString<_Sz>::add adds to current string content a formatted
 * @param[in] pFormat a formatting string corresponding to printf syntax
 * @param[in] varying arguments as used with printf syntax
 * @return a reference to current string
 */
template <size_t _Sz>
templateWString<_Sz> &
templateWString<_Sz>::add(const wchar_t *pFormat,...)
{
    wchar_t wBuf[_Sz];
    va_list args;
    va_start (args, pFormat);
    vswprintf (wBuf ,_Sz,pFormat, args);
    va_end(args);
size_t wSize = wcslen (wBuf);
    if (wSize+wcslen(content)>_capacity)
                {
                wSize=wcslen(content)-wcslen (wBuf);
                }

    wcsncat(content,wBuf,wSize);
return(*this);
}
/**
 * @brief templateWString<_Sz>::eliminateChar wipes (eliminates) the given char from string content.
 * After having called this procedure, the given char does no more exist within the string,
 * and string length is reduced in proportion of number of char eliminated.
 * @param[in] pChar character to eliminate
 * @return a reference to the current templateWString
 */
template <size_t _Sz>
templateWString<_Sz> &
templateWString<_Sz>::eliminateChar (wchar_t pChar)
{
    wchar_t wChar[1] ;
    wChar[0] =  pChar;
    wchar_t* wPtr = content ;
    size_t wS = size()*sizeof(wchar_t);
    while (wPtr[0]!=L'\0')
    {
        if (wPtr[0]==wChar[0])
                {
                memmove(wPtr,&wPtr[1],wS);
                wS-=sizeof(wchar_t);
                continue;
                }
            else
                wPtr++;
    }//while
    return *this;
}//eliminateChar





//-----------------------keywordString--------------------------

/**
 * @brief locate  locates a substring pString in templateWString and returns its offset from beginning as a ssize_t value.
 * returns a negative value if substring has not been found.
 * @param pString a const char* string containing the substring to find
 * @return offset (starting from 0) of the substring in main string if substring is found, a negative value if not.
 */
template <size_t _Sz>
size_t
templateWString<_Sz>::locate(const wchar_t* pString)
{
    return (size_t)((size_t)wcsstr(content,pString)-(size_t)&content);
}


/**
 * @brief locate  locates a substring pString in templateWString and returns its offset from beginning as a ssize_t value.
 * returns a negative value if substring has not been found.
 * @param pString a templateWString of same type containing the substring to find
 * @return offset (starting from 0) of the substring in main string if substring is found, a negative value if not.
 */
template <size_t _Sz>
size_t
templateWString<_Sz>::locate(templateWString<_Sz> pString)
{return (ssize_t)((ssize_t)wcsstr(content,pString.content)-(ssize_t)&content);}

/**
 * @brief toUpper Upperizes characters of current string. Current string is modified if ever by characters upperization.
 * @return a reference to current string
 */
template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::toUpper(void)
{
    _WtoUpper(content,nullptr);
    return *this;
}
/**
 * @brief toUpper Upperizes characters of current string. Upperization is made into an output string of same type pOut.
 * @param[out] pOut a template string destinated to receive the upperized string
 * @return a reference to current string
 */
template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::toUpper(templateWString<_Sz>& pOut)
{
    _WtoUpper(content,pOut.content);
    return pOut;
}
/**
 * @brief templateWString::LTrim Left trims templateWString::content with character set pSet.
 * @param pSet set of characters to consider. Defaulted to __DEFAULTDELIMITERSET__
 * @return a reference to current keywordString object
 */
template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::LTrim ( const wchar_t *pSet)
{
    size_t wL =wcslen(content);
    const wchar_t *wPtr=firstNotinSet(pSet);
    if (wPtr==nullptr)
            return(*this);
    wL=wL-(content-(wchar_t *)wPtr);
    wcsncpy(content,wPtr,wL);
    return (*this);
}

/**
 * @brief templateWString::firstNotinSet   finds the first byte of string pString that is NOT a byte of pSet.
 *
 * usage example : find the first non space and non tab (\t) char of a string.
 *
 * @param pSet set of characters to consider. Defaulted to __DEFAULTDELIMITERSET__
 * @return a const char pointer to the first byte of templateWString content that is NOT in pSet
 */
template <size_t _Sz>
const wchar_t *
templateWString<_Sz>::firstNotinSet (const wchar_t *pSet)
{
    return((wchar_t *)_WfirstNotinSet((const wchar_t*)content,pSet));
}

/**
 * @brief templateWString::RTrim Suppresses ending characters that are in pSet (defaulted to __DEFAULTDELIMITERSET__)
 * @param pSet character set to trim off
 * @return a reference to current templateWString object
 */
template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::RTrim (const wchar_t *pSet)
{
size_t wj;
    size_t w1=wcslen(content)-1;

 //   for (w1=strlen(content);(w1 >= 0)&&(content[w1]==' ');w1--); // empty for loop
    for (;(w1 >= 0);w1--)
            {
            for (wj=0;pSet[wj]!=L'\0';wj++)
                                if (content[w1]==pSet[wj])
                                            {
                                            content[w1]=L'\0';
                                            break;
                                            }
            if (wj==wcslen(pSet))
                                break;

            }
    return (*this);
}//   RTrim
template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::Trim(const wchar_t *pSet)
{
 ;
 return(LTrim(pSet).RTrim(pSet));
}

/**
 * @brief strchrReverse find the first occurrence of pChar within pStr string in reverse (starting to strlen() and ending at 0)
 * @param pChar the character to find
 * @return pointer to pChar occurrence within string if found, NULL if not found
 */
template <size_t _Sz>
const wchar_t *
templateWString<_Sz>::strchrReverse(const wchar_t pChar)
{
    for (int w1=wcslen(content);(w1 >= 0)&&(content[w1]!=pChar);w1--)
    return ((w1<0) ? NULL:(wchar_t*) &content[w1]);     // return NULL pointer if not found pointer to pChar within string if found
}

/**
 * @brief templateWString<_Sz>::strspnReverse
 *          finds the first occurrence of one of the characters set given by pSet within keywordString::content in reverse (starting to strlen() and ending at 0)
 * @param pSet set of characters to search for (defaulted to  __DEFAULTDELIMITERSET__).
 * @return pointer to pChar occurrence within keywordString::content if found, NULL if not found
 */
template <size_t _Sz>
const wchar_t *
templateWString<_Sz>::strspnReverse( const wchar_t *pSet)
{
size_t wL = wcslen(pSet);
size_t wj = 0;
long wi;
    for (wi=wcslen(content);(wi >= 0)&&(content[wi]!=pSet[wj]);wi--)
                            {
                            for (wj=0;(wj<wL)&&(content[wi]!=pSet[wj]);wj++);
                            }
    return ((wi<0) ? nullptr: &content[wi]);     // return NULL pointer if not found pointer to pChar within string if found
}


template <size_t _Sz>
/**
 * @brief templateWString<_Sz>::startsCaseWith Test if String starts with pString regarless case after having trimmed out character from pSet
 * @param[in] pString substring to search
 * @param[in] pSet set of characters to consider as being eliminated (trimmed)(defaulted to __DEFAULTDELIMITERSET__)
 * @return a bool set to true if current string starts with given string pString
 */
bool
templateWString<_Sz>::startsCaseWith(const wchar_t* pString,const wchar_t* pSet)
{
    wchar_t* wPtr=_WLTrim(content,pSet);
    if (wcsncasecmp(wPtr,pString,wcslen(pString)))
                            return true;
    return (false);
}
/**
 * @brief templateWString::startsWith Test if String starts with pString after having trimmed out character from pSet
 * @param[in] pString substring to search
 * @param[in] pSet  character set to trim off (defaulted to __DEFAULTDELIMITERSET__)
 * @return true if found, false if not found
 */
template <size_t _Sz>
bool
templateWString<_Sz>::startsWith(const wchar_t* pString,const wchar_t* pSet)
{
    wchar_t* wPtr=_WLTrim(content,pSet);
    if (wcsncmp(content,pString,wcslen(pString)))
                            return true;
    return (false);
}
/**
 * @brief keywordString::startsWithApprox
 *  Test if templateWString::content starts with pString after having trimmed out character in pSet
 * this test is done with an approximation of pApprox. test is done Case sensitive.
 * @param pString substring to search
 * @param pApprox minimum number of character of pString to consider the test valid
 * @param pSet  character set to trim off (defaulted to __DEFAULTDELIMITERSET__)
 * @return true if found, false if not found
 */
template <size_t _Sz>
bool
templateWString<_Sz>::startsWithApprox(const wchar_t *pString, ssize_t pApprox, const wchar_t* pSet)
{
    const wchar_t* wPtr=_WLTrim(content,pSet);
    ssize_t wSize;
    if (wPtr==nullptr)
                wSize=wcslen(content);
            else
                wSize=wPtr-content;
    if (wSize<pApprox)
            return false;
    if (wcsncmp(content,pString,wSize)==0)
                                    return true;
    return (false);
}
/**
 * @brief templateWString::startsCaseWithApprox
 *  Test if templateWString::content starts with pString after having trimmed out character in pSet
 * this test is done with an approximation of pApprox. Case unsensitive.
 * comparison is made without taking into account cases.
 * @param pString substring to search
 * @param pApprox minimum number of character of pString to consider the test valid
 * @param pSet  character set to trim off (defaulted to __DEFAULTDELIMITERSET__)
 * @return true if found, false if not found
 */
template <size_t _Sz>
bool
templateWString<_Sz>::startsCaseWithApprox(const wchar_t* pString,int pApprox,const wchar_t* pSet)
{
    _WLTrim(content,pSet);
    const wchar_t* wPtr = _WfirstinSet(content,pSet);
    size_t wSize;
    if (wPtr==nullptr)
                wSize=wcslen(content);
            else
                wSize=wPtr-content;
    if (wSize<pApprox)
                return false;
    if (wcsncasecmp(content,pString,wSize)==0)
                                        return true;
    return (false);
}


/**
 * @brief templateWString::Substr Searches a substring pSub within templateWString::content starting at pOffset
 * and returns its position SINCE BEGINNING (start 0) of templateWString::content
 *
 * Returns -1 if not found
 *
 * Search is case sentive
 * @param pSub  substring to search
 * @param pOffset offset (start 0) to begin search in keywordString::content
 * @return offset of the beginning of substring (start 0) within keywordString::content
 */
template <size_t _Sz>
int
templateWString<_Sz>::Substr(const wchar_t *pSub, int pOffset)
{
    const wchar_t * wPtr = wcsstr((const wchar_t*)&content[pOffset],pSub);
        if (wPtr==nullptr)
                        return -1;
         return((int)(wPtr-content));
}
/**
 * @brief templateWString::SubstrCase Searches a templateWString pSub within keywordString content starting at pOffset
 * and returns its position SINCE BEGINNING (start 0) of templateWString::content
 *
 * Returns -1 if not found
 *
 * Search is case insentive (case or Uppercase are treated as same)
 *
 * @param pSub C string as a substring to search for
 * @param pOffset offset (start 0) to begin search in keywordString::content
 * @return offset of the beginning of substring (start 0) within keywordString::content
 */

template <size_t _Sz>
int
templateWString<_Sz>::SubstrCase(const wchar_t *pSub, int pOffset) // to be reviewed : not case independant
{
    wchar_t * wPtr = wcsstr((wchar_t*)&content[pOffset],pSub);
        if (wPtr==nullptr)
                        return -1;
         return((int)(wPtr-content));
}

/**
 * @brief templateWString<_Sz>::Left return the left part of current templateWString on pLen within a string of same type than current.
 * @param pLen
 * @return
 */
template <size_t _Sz>
templateWString<_Sz>
templateWString<_Sz>::Left (int pLen)
{
   templateWString<_Sz> wU ;
   wcsncpy(wU.content,content,pLen);
   return wU;
}
/**
 * @brief keywordString::Right returns a keywordString with a content of length pLen containing
 * the pLen last characters (on the right) of keywordString actual content.
 * if pLen is greater than actual content size, then it takes the whole content from start.
 * @param pLen
 * @return
 */
template <size_t _Sz>
templateWString<_Sz>
templateWString<_Sz>::Right (int pLen)
{
   templateWString<_Sz> wU ;
   int wStart= wcslen(content)-pLen;
   if (wStart<0)
                wStart=0;
   wcsncpy(wU.content,&content[wStart],pLen);
   return wU;
}

#include <zcrypt/zcrypt.h>

//-----------Encoding & Decoding-----------------------------------------
template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::uncryptB64(ZDataBuffer &pZBD64)
{
 if (CryptMethod==ZCM_Uncrypted)
                        return *this; // avoid double uncrypting
 ZDataBuffer wZDB ;
 size_t wByteSize= getByteSize();
    wZDB.uncryptB64((unsigned char*)content, wByteSize+1);  // decode to ZDataBuffer
    wZDB.moven((unsigned char*)content,wByteSize);    // move back the decoded content to templateWString content
    CryptMethod = ZCM_Uncrypted;
    return *this;
}

template <size_t _Sz>
ZDataBuffer*
templateWString<_Sz>::encryptB64(void)
{
    if (CryptMethod==ZCM_Base64)     // avoid double crypting
                    return(*this);
    unsigned char*wB64String=nullptr;
    size_t wB64Size=0;
    size_t wByteSize= getByteSize();
    ZStatus wSt=::encryptB64( &wB64String,&wB64Size,(const unsigned char*)content,wcslen(content)*sizeof(wchar_t) );
    if (wSt!=ZS_SUCCESS)
        {
        _free(wB64String);
        return nullptr;
        }
    ZDataBuffer* wZDB= new ZDataBuffer(wB64String,wB64Size);
    _free(wB64String);
    return wZDB;
}

#include <zcrypt/md5.h>  // for md5

template <size_t _Sz>
md5
templateWString<_Sz>::getMD5(void)
{
    size_t wByteSizeLen=strlen(content)*sizeof(content[0]);
    return (md5((unsigned char*)content,wByteSizeLen));
}// getMD5
#include <cmath>
template <size_t _Sz>
long
templateWString<_Sz>::hashCode(void)
{
    long wh = 0;
    long ws=size();
    long double wValue,wExponent;

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
}// hashCode


//===============Export Import=====================================
template <size_t _Sz>
/**
 * @brief templateWString<_Sz>::toCString converts string content from wchar_t to char. May loose precision.
 * @param pOutCString
 * @return
 */
ZDataBuffer&
templateWString<_Sz>::toVaryingCString (ZDataBuffer &pOutCString)
{
    mbstate_t wBuf;
    memset(&wBuf,0,sizeof(wBuf));

    pOutCString.allocate(_capacity);
    int wSize=wcsrtombs(pOutCString.DataChar,content,_capacity,&wBuf);
    return pOutCString;
}
template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::fromVaryingCString (ZDataBuffer &pInCString)
{
mbstate_t wBuf;
    memset(&wBuf,0,sizeof(wBuf));
    size_t wRSize= pInCString.Size-1;

    memset (content,0,getByteCapacity());
    if (pInCString.Size>_capacity)
        {
        fprintf(stderr,"%s>> Warning : string is been truncated to %ld characters while %ld size requested\n",
                _GET_FUNCTION_NAME_,
                _capacity-1,
                pInCString.Size-1);
                pInCString.Ctruncate(_capacity);
                wRSize= _capacity-1;
        }
     int wSize=mbsrtowcs(content,&pInCString.DataChar,_capacity,&wBuf);
     if (wSize!=wRSize)
        {
         fprintf(stderr,"%s>> Error : a problem occurred while converting CString to WString. Max char converted is %ld characters while %ld size requested\n",
                 _GET_FUNCTION_NAME_,
                 wSize,
                 wRSize);
        }
    return *this;
}
template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::fromCString_Ptr (const char* pInCString)
{
    mbstate_t wBuf;
    memset(&wBuf,0,sizeof(wBuf));

    size_t wRSize= strlen(pInCString);
    memset (content,0,getByteCapacity());
    if (wRSize>=_capacity)
        {
        fprintf(stderr,"%s>> Warning : string is been truncated to %ld characters while %ld size requested\n",
                _GET_FUNCTION_NAME_,
                _capacity-1,
                wRSize);
        wRSize= _capacity-1;
        }
     int wSize=mbsrtowcs(content,&pInCString,wRSize,&wBuf);
     if (wSize!=wRSize)
        {
         fprintf(stderr,"%s>> Error : a problem occurred while converting CString to WString. Max char converted is %d characters while %ld size requested\n",
                 _GET_FUNCTION_NAME_,
                 wSize,
                 wRSize);
        }
    content[wRSize]=L'\0';
    return *this;
}
template <size_t _Sz>
templateWString<_Sz>&
templateWString<_Sz>::fromCString_PtrCount (const char* pInCString,size_t pCount)
{
    mbstate_t wBuf;
    memset(&wBuf,0,sizeof(wBuf));

    size_t wRSize= pCount;
    memset (content,0,getByteCapacity());
    if (wRSize>=_capacity)
        {
        fprintf(stderr,"%s>> Warning : string is been truncated to %ld characters while %ld size requested\n",
                _GET_FUNCTION_NAME_,
                _capacity-1,
                wRSize);
//                pInCString.Ctruncate(_capacity);
                wRSize= _capacity-1;
        }
     int wSize=mbsrtowcs(content,&pInCString,wRSize,&wBuf);
     if (wSize!=wRSize)
        {
         fprintf(stderr,"%s>> Error : a problem occurred while converting CString to WString. Max char converted is %d characters while %ld size requested\n",
                 _GET_FUNCTION_NAME_,
                 wSize,
                 wRSize);
        }
     content[wRSize]=L'\0';
    return *this;
}
//=======================Import export Universal/URF ====================================================

template <size_t _Sz1>
/**
 * @brief templateWString<_Sz1>::_exportURF exports the content of wide string to a ZDataBuffer in an URF format
 *
 *      see <zindexedfile/znaturalfromurf.cpp>  <ZType_Type vs URF Header sizes>
 *
 *  URF content returned by _exportURF
 *
 *header :
 *  ZTypeBase      ZType_type of data must be ZType_FixedWString
 *  uint16_t       Canonical size or Array count : capacity = maximum number of character
 *  uint16_t       UVF size : total size in bytes = effective size of data in bytes termination char L'\0' EXCLUDED
 *                  Does NOT includes last L'\0' wchar_t
 *data :
 *  wchar_t sequence : data reversed if necessary (Little Endian)
 *
 *  Maximum size is in bytes :      65'535
 *  Maximum number of characters :  32'765
 *
 * @param pUniversal
 * @return
 */
ZDataBuffer *
templateWString<_Sz1>::_exportURF(ZDataBuffer* pUniversal)
{

wchar_t* wPtr ;
ZTypeBase wType=ZType_FixedWString;

uint16_t wCanonical=(uint16_t)capacity();
size_t wUniversalSize=wcslen(content);
uint16_t wUSizeReversed;
size_t wURFByteSize=(size_t)(((wUniversalSize)*sizeof(content[0]))+(sizeof(uint32_t)*2))+sizeof(ZTypeBase);
size_t wOffset=0;

    if (wUniversalSize>=UINT16_MAX)
        {
        fprintf (stderr,"%s>>Fatal error fixed string maximum capacity overflow <%d> while <%d> authorized.\n",
                 _GET_FUNCTION_NAME_,
                 wUniversalSize,
                 (UINT16_MAX/sizeof(wchar_t)));
        _ABORT_ ;
        }
    wUSizeReversed=(uint16_t)wUniversalSize;

    pUniversal->allocateBZero((ssize_t)wURFByteSize);
                                                                    // URF Header is
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);                    // ZTypeBase in reverseOrder if LE (if little endian)
    memmove(pUniversal->Data,&wType,sizeof(wType));
    wOffset=sizeof(wType);

    wCanonical=reverseByteOrder_Conditional<uint16_t>(wCanonical);// Capacity (canonical length in characters) in reverse order if LE
    memmove(pUniversal->Data+wOffset,&wCanonical,sizeof(wCanonical));
    wOffset+=sizeof(wCanonical);

    wUSizeReversed=reverseByteOrder_Conditional<uint16_t>(wUSizeReversed);   // URF effective byte size INCLUDING header size in reverse order if LE
    memmove(pUniversal->Data+wOffset,&wUSizeReversed,sizeof(wUSizeReversed));
    wOffset+=sizeof(wUSizeReversed);


//    pUniversal->extend((size_t)wUniversalSize);
    wPtr=(wchar_t*)(pUniversal->Data+wOffset);
    for (long wi=0; wi < wUniversalSize;wi++)
            {
            wPtr[wi]=reverseByteOrder_Conditional<wchar_t>(content[wi]);
            }

    return  pUniversal;
}//_exportURF

template <size_t _Sz>
/**
 * @brief templateWString<_Sz>::getUniversalFromURF
 *      Static method that extract Universal value from memory zone pointed by pURFDataPtr
 *      and returns a ZDataBuffer loaded with Universal value with canonical capacity of the string.
 *
 *      output ZDB is of capacity size corresponding to canonical array count * character size
 *      output data is padded with L'\0' character to correspond to canonical capacity
 * @param pURFDataPtr
 * @param pUniversal
 * @return
 */
ZStatus
templateWString<_Sz>::getUniversalFromURF(unsigned char* pURFDataPtr,ZDataBuffer& pUniversal)
{
uint16_t wCanonSize , wEffectiveUSize , wEffectiveArrayCount;
size_t wURFByteSize;
ZTypeBase wType;
unsigned char* wURFDataPtr = pURFDataPtr;

    memmove(&wType,wURFDataPtr,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    wURFDataPtr += sizeof (ZTypeBase);
    if (wType!=ZType_FixedWString)
        {
        fprintf (stderr,
                 "%s>> Error invalid URF data type <%X> <%s> while getting universal value of <%s> ",
                 _GET_FUNCTION_NAME_,
                 wType,
                 decode_ZType(wType),
                 decode_ZType(ZType_FixedWString));
        return ZS_INVTYPE;
        }

    memmove (&wCanonSize,wURFDataPtr,sizeof(uint16_t));     // then canonical size (capacity in number of characters)
    wCanonSize=reverseByteOrder_Conditional<uint16_t>(wCanonSize);
    wURFDataPtr += sizeof (uint16_t);

    memmove (&wEffectiveUSize,wURFDataPtr,sizeof(uint16_t));        // first is URF byte size (including URF header size)
    wEffectiveUSize=reverseByteOrder_Conditional<uint16_t>(wEffectiveUSize);
    wURFDataPtr += sizeof (uint16_t);

 //   wEffectiveUSize = wEffectiveUSize - (uint32_t)(sizeof(ZTypeBase) + (sizeof(uint32_t)*2)); // compute net Universal size

    wEffectiveArrayCount = wEffectiveUSize /(uint16_t)sizeof(content[0]); //compute effective array count : effective number of characters stored

    pUniversal.allocateBZero(((size_t)wCanonSize)*sizeof(content[0])); // fixed string must have canonical characters count allocated

    memmove(pUniversal.Data,wURFDataPtr,wEffectiveUSize);

    return ZS_SUCCESS;
}//getUniversalFromURF

template <size_t _Sz>
/**
 * @brief templateWString<_Sz>::getUniversalFromURF_Truncated
 *      Static method that extract Universal value from memory zone pointed by pURFDataPtr
 *      and returns a ZDataBuffer loaded with Universal value truncated to its effective number of characters.
 *
 *      output ZDB is set to effective number of characters of the string (* size of wchar_t)
 *      output data is NOT padded with L'\0' character
 * @param pURFDataPtr
 * @param pUniversal
 * @return
 */
ZStatus
templateWString<_Sz>::getUniversalFromURF_Truncated(unsigned char* pURFDataPtr,ZDataBuffer& pUniversal)
{
uint16_t wCanonSize , wEffectiveUSize , wEffectiveArrayCount;
size_t wURFByteSize;
ZTypeBase wType;
unsigned char* wURFDataPtr = pURFDataPtr;

    memmove(&wType,wURFDataPtr,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    wURFDataPtr += sizeof (ZTypeBase);
    if (wType!=ZType_FixedWString)
        {
        fprintf (stderr,
                 "%s>> Error invalid URF data type <%X> <%s> while getting universal value of <%s> ",
                 _GET_FUNCTION_NAME_,
                 wType,
                 decode_ZType(wType),
                 decode_ZType(ZType_FixedWString));
        return ZS_INVTYPE;
        }

    memmove (&wCanonSize,wURFDataPtr,sizeof(uint16_t));     // then canonical size (capacity in number of characters)
    wCanonSize=reverseByteOrder_Conditional<uint16_t>(wCanonSize);
    wURFDataPtr += sizeof (uint16_t);

    memmove (&wEffectiveUSize,wURFDataPtr,sizeof(uint16_t));        // first is URF byte size (including URF header size)
    wEffectiveUSize=reverseByteOrder_Conditional<uint16_t>(wEffectiveUSize);
    wURFDataPtr += sizeof (uint16_t);

 //   wEffectiveUSize = wEffectiveUSize - (uint32_t)(sizeof(ZTypeBase) + (sizeof(uint32_t)*2)); // compute net Universal size

    wEffectiveArrayCount = wEffectiveUSize /(uint16_t)sizeof(content[0]); //compute effective array count : effective number of characters stored

    pUniversal.allocateBZero(((size_t)wCanonSize)*sizeof(content[0])); // fixed string must have canonical characters count allocated

    memmove(pUniversal.Data,wURFDataPtr,wEffectiveUSize);

    return ZS_SUCCESS;
}//getUniversalFromURF_Truncated

template <size_t _Sz1>
/**
 * @brief templateWString<_Sz1>::_importURF
 * @param pUniversal
 * @param pSize size IN BYTES to import (and not in number or characters -sizeof(wchar_t) )
 * @return
 */
templateWString<_Sz1> &
templateWString<_Sz1>::_importURF(unsigned char* &pUniversal)
{
ZTypeBase   wType;
uint16_t    wCanonical;
uint16_t    wEffectiveUSize, wEffectiveArrayCount;
size_t      wURFByteSize;
size_t      wOffset=0;
wchar_t* wPtr;

    memmove(&wType,pUniversal,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    wOffset+= sizeof(ZTypeBase);

    memmove(&wCanonical, pUniversal+wOffset,sizeof(wCanonical));
    wCanonical=reverseByteOrder_Conditional<uint16_t>(wCanonical);
    wOffset+= sizeof(wCanonical);

    memmove(&wEffectiveUSize, pUniversal+wOffset,sizeof(wEffectiveUSize));
    wEffectiveUSize=reverseByteOrder_Conditional<uint16_t>(wEffectiveUSize);
    wOffset+= sizeof(wEffectiveUSize);

//    wEffectiveUSize = wEffectiveUSize - (uint32_t)(sizeof(ZTypeBase) + (sizeof(uint32_t)*2));  // subtract URF header size to get net data size

    wEffectiveArrayCount = wEffectiveUSize /(uint16_t) sizeof(content[0]);

    if (wEffectiveArrayCount > _capacity)
        {
        fprintf(stderr,
                "%s>> Error <%s> Capacity of templateWString overflow: requested <%ld> while capacity is <%ld> ",
                _GET_FUNCTION_NAME_,
                decode_ZStatus(ZS_FIELDCAPAOVFLW),
                wEffectiveArrayCount,
                _capacity);
        wEffectiveArrayCount=_capacity-1;
        }
    wPtr=(wchar_t*)(pUniversal+wOffset);
    for (size_t wi=0;wi<wEffectiveArrayCount;wi++)
        {
        content[wi] = reverseByteOrder_Conditional<wchar_t>(wPtr[0]);
        wPtr++;
        }
    return *this;

}// _importURF
#endif // TEMPLATEWSTRING_H
