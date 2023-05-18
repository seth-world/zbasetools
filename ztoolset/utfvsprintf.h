#ifndef UTFVSPRINTF_H
#define UTFVSPRINTF_H


//==========================DO NOT USE THIS FILE : to be removed ================================


#include <config/zconfig.h>
#include <ztoolset/zcharset.h>


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

//#include <pmon.h>



/** In this file is defined a template FIXED string container that is used as base for various sizes of strings.
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
 * - utftemplateString : native template
 * - codeString : fixed string for code management  - cst_codelen
 * - descString : fixed string for various descriptions - cst_desclen
 * - uriString : fixed string for files and directories pathes management - cst_urilen
 * - messageString : for message - cst_messagelen
 * - exceptionString : exception message - cst_exceptionlen
 * - identityString : for identity and authentication - cst_identitylen
 * - keywordString : for managing keywords - cst_keywordlen
 *
 *
 * This file is divided in Sections.
 *
 */

#include <iostream>
#include <stdlib.h>

//#include <openssl/sha.h>  // required for checksum

#include <ztoolset/zerror.h>
#include <ztoolset/zdatabuffer.h>
#include <string.h>
#include <ztoolset/zlimit.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zatomicconvert.h>
#include <ztoolset/zcharset.h>

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
char *_expurgeSet(char *pString, const char *pSet);
char *_expurgeString(char *pString, const char *pSubString);

char *_printStdStringField(void *pField, bool IsPointer, char *pBuf);
namespace zbs
{
char *_printStdStringField(void *pField, bool IsPointer, char *pBuf);
}
const char* _Zsprintf(char *pBuf,const char* pFormat,...);




#define FLOATINGPT
#define NEWFP

#include <stdlib.h>
#include <stdio.h>


const int FMT_LJUST     =1;
const int FMT_RJUST0    =2;
const int FMT_CENTER    =3;



int
utf8Vsprintf(utf8_t *d, const utf8_t *s, va_list ap)

{
    const utf8_t *t;
    utf8_t *p, *dst, tmp[40];
    char* wTmpA[40];
//    unsigned int n;
    unsigned long n;
    int fmt, trunc, haddot, width, base, longlong;
#ifdef FLOATINGPT
    double dbl;

#ifndef NEWFP
    EP ex;
#endif
#endif

    dst = d;
    for (; *s;) {
        if (*s == u8'%') {
            s++;
            fmt = FMT_RJUST;
            width = trunc = haddot = longlong = 0;
            for (; *s; s++) {
                if (utfStrchr<utf8_t>(u8"bcdefgilopPrRsuxX%", *s))
                    break;
                else if (*s == '-')
                    fmt = FMT_LJUST;
                else if (*s == '0')
                    fmt = FMT_RJUST0;
                else if (*s == '~')
                    fmt = FMT_CENTER;
                else if (*s == '*') {
                    if (haddot)
                        trunc = va_arg(ap, int);
                    else
                        width = va_arg(ap, int);
                } else if (*s >= u8'1' && *s <= u8'9') {
                    size_t wi=0;
                    for (wi=0; isdigit(*s); s++, wi++){
                            wTmpA[wi]=(char)*s;
                            }
//                    utfStrncpy<utf8_t>(tmp, t, s - t);
                    wTmpA[wi] = 0;
//                    atob(&n, tmp, 10);
                    n=strtoul( wTmpA,wTmpA+wi,10);
                    if (haddot)
                        trunc = n;
                    else
                        width = n;
                    s--;
                } else if (*s == u8'.')
                    haddot = 1;
            }
            if (*s == u8'%') {
                *d++ = u8'%';
                *d = 0;
            } else if (*s == 's') {
                p = va_arg(ap, utf8_t *);

                if (p)
                    utfStrcpy<utf8_t>(d, p);
                else
                    utfStrcpy<utf8_t>(d, u8"(null)");
            } else if (*s == u8'c') {
                n = va_arg (ap, int);
                *d = n;
                d[1] = 0;
            } else {
                if (*s == u8'l') {
                    if (*++s == u8'l') {
                        longlong = 1;
                        ++s;
                    }
                }
                if (utfStrchr<utf8_t>(u8"bdiopPrRxXu", *s)) {
                    if (*s == u8'd' || *s == u8'i')
                        base = -10;
                    else if (*s == u8'u')
                        base = 10;
                    else if (*s == u8'x' || *s == u8'X')
                        base = 16;
                    else if(*s == u8'p' || *s == u8'P') {
                        base = 16;
                        if (*s == u8'p') {
                            *d++ = u8'0';
                            *d++ = u8'x';
                        }
                        fmt = FMT_RJUST0;
                        if (sizeof(long) > 4) {
                            width = 16;
                            longlong = 1;
                        } else {
                            width = 8;
                        }
                    }
                    else if(*s == u8'r' || *s == u8'R') {
                        base = 16;
                        if (*s == u8'r') {
                            *d++ = u8'0';
                            *d++ = u8'x';
                        }
                        fmt = FMT_RJUST0;
                        if (sizeof(register_t) > 4) {
                            width = 16;
                            longlong = 1;
                        } else {
                            width = 8;
                        }
                    }
                    else if (*s == u8'o')
                        base = 8;
                    else if (*s == u8'b')
                        base = 2;
                    if (longlong)
                        llbtoa(d, va_arg (ap, quad_t),
                            base);
                    else
                        btoa(d, va_arg (ap, int), base);

                    if (*s == u8'X')
                        strtoupper(d);
                }
#ifdef FLOATINGPT
                else if (strchr ("eEfgG", *s)) {
static void dtoa (char *, double, int, int, int);
                    dbl = va_arg(ap, double);
                    dtoa(d, dbl, *s, width, trunc);
                    trunc = 0;
                }
#endif
            }
            if (trunc)
                d[trunc] = 0;
            if (width)
                str_fmt (d, width, fmt);
            for (; *d; d++);
            s++;
        } else
        *d++ = *s++;
    }
    *d = 0;
    return (d - dst);
}



//======================Generic functions===========================================================

template <size_t _Sz1,size_t _Sz2>
utftemplateString<_Sz1>& moveutftemplateString (utftemplateString<_Sz1>& pDest,utftemplateString<_Sz2>& pSource);



//======================End Generic functions===========================================================
#ifdef __COMMENT__

//-------------utftemplateString Methods expansion--------------
//

template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::fromString(const char* pInString)
{
  _Utf*wData=content;
  size_t wi=0;
  utftemplateString::clear();
  if (pInString==nullptr)
  {
    content[0]='\0';
    return *this;
  }
  for (;(pInString!=0)&&(wi<_capacity);*pInString++,wData++,wi++)
    *wData=(_Utf)*pInString;
  return(*this);
}
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::fromStringCount(const char* pInString,const size_t pCount)
{
  utftemplateString<_Sz,_Utf>::clear();
  int wCount=pCount;
  if (pCount>(_capacity-1))
    wCount = _capacity-1;

  strncpy(content,pInString,wCount);
  //    content[wCount]='\0';
  return(*this);
}

template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::fromWString_Ptr(const wchar_t* pInWString)
{
  utftemplateString<_Sz,_Utf>::clear();
  size_t wCount=wcslen(pInWString);
  if (wCount>=_capacity)
    wCount = _capacity-1;

  mbstate_t wBuf;
  memset(&wBuf,0,sizeof(wBuf));
  size_t wRSize=(wCount);
  //     int wSize= mbstowcs(content,pInCString.DataChar,_capacity);
  size_t wSize=wcsrtombs(content,(const wchar_t**)&pInWString,wRSize,&wBuf);
  if (wSize!=wRSize)
  {
    fprintf(stderr,"%s>> Error : a problem occurred while converting wchar_t* String to Fixed String. Max char converted is %ld characters while %ld size requested\n",
        _GET_FUNCTION_NAME_,
        wSize,
        wRSize);
  }

  content[wSize]='\0';
  return(*this);
}
template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::fromWString_PtrCount
 * @param pInWString
 * @param pCount       number of characters to convert from pInWString \0 termination excluded
 * @return
 */
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::fromWString_PtrCount(const wchar_t* pInWString,const size_t pCount)
{
  utftemplateString<_Sz,_Utf>::clear();
  size_t wCount=pCount;
  if (pCount>=_capacity)
    wCount = _capacity-1;

  mbstate_t wBuf;
  memset(&wBuf,0,sizeof(wBuf));

  size_t wRSize=(wCount);
  //     int wSize= mbstowcs(content,pInCString.DataChar,_capacity);
  size_t wSize=wcsrtombs(content,(const wchar_t**)&pInWString,wRSize,&wBuf);
  if (wSize!=wRSize)
  {
    fprintf(stderr,"%s>> Error : a problem occurred while converting wchar_t* String to Fixed String. Max char converted is %ld characters while %ld size requested\n",
        _GET_FUNCTION_NAME_,
        wSize,
        wRSize);
  }

  content[wSize]='\0';
  return(*this);
}

/**
 * @brief utftemplateString<_Sz,_Utf>::add adds to current string content a formatted
 * @param[in] pFormat a formatting string corresponding to printf syntax
 * @param[in] varying arguments as used with printf syntax
 * @return a reference to current string
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf> &
utftemplateString<_Sz,_Utf>::add(const char *pFormat,...)
{
  char wBuf[_Sz];
  va_list args;
  va_start (args, pFormat);
  vsprintf (wBuf ,pFormat, args);
  va_end(args);
  size_t wSize = strlen (wBuf);
  if (wSize+strlen(content)>sizeof(content))
  {
    wSize=strlen(content)-strlen (wBuf);
  }

  strncat(content,wBuf,wSize);
  return(*this);
}
/**
 * @brief utftemplateString<_Sz,_Utf>::eliminateChar wipes (eliminates) the given char from string content.
 * After having called this procedure, the given char does no more exist within the string,
 * and string length is reduced in proportion of number of char eliminated.
 * @param[in] pChar character to eliminate
 * @return a reference to the current utftemplateString
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf> &
utftemplateString<_Sz,_Utf>::eliminateChar (char pChar)
{
  char wChar[1] ;
  wChar[0] =  pChar;
  char* wPtr = content ;
  size_t wS = size();
  while (wPtr[0]!='\0')
  {
    if (wPtr[0]==wChar[0])
    {
      memmove(wPtr,&wPtr[1],wS);
      wS--;
      continue;
    }
    else
      wPtr++;
  }//while
  return *this;
}//eliminateChar

/**
 * @brief utftemplateString<_Sz,_Utf>::addConditionalOR append pValue to current string.
 *      if string is not empty, then append OR sign plus spaces < | > before appending string content.
 * @param pValue
 * @return
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::addConditionalOR (const char*pValue)
{
  utftemplateString<_Sz,_Utf> wString =toString();
  wString.LTrim();
  if (wString.content[0]!='\0')
    add(" | ");
  add(pValue);
  return *this;
}

//-----------------------keywordString--------------------------

/**
 * @brief locate  locates a substring pString in utftemplateString and returns its offset from beginning as a ssize_t value.
 * returns a negative value if substring has not been found.
 * @param pString a const char* string containing the substring to find
 * @return offset (starting from 0) of the substring in main string if substring is found, a negative value if not.
 */
template <size_t _Sz,class _Utf>
size_t
utftemplateString<_Sz,_Utf>::locate(const char* pString)
{
  return (size_t)((size_t)strstr(content,pString)-(size_t)&content);
}


/**
 * @brief locate  locates a substring pString in utftemplateString and returns its offset from beginning as a ssize_t value.
 * returns a negative value if substring has not been found.
 * @param pString a utftemplateString of same type containing the substring to find
 * @return offset (starting from 0) of the substring in main string if substring is found, a negative value if not.
 */
template <size_t _Sz,class _Utf>
size_t
utftemplateString<_Sz,_Utf>::locate(utftemplateString<_Sz,_Utf> pString) {return (ssize_t)((ssize_t)strstr(content,pString.content)-(ssize_t)&content);}

/**
 * @brief toUpper Upperizes characters of current string. Current string is modified if ever by characters upperization.
 * @return a reference to current string
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::toUpper(void)
{
  _toUpper(content,nullptr);
  return *this;
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
  _toUpper(content,pOut.content);
  return pOut;
}
/**
 * @brief utftemplateString::LTrim Left trims utftemplateString::content with character set pSet.
 * @param pSet set of characters to consider. Defaulted to __DEFAULTDELIMITERSET__
 * @return a reference to current keywordString object
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::LTrim ( const char *pSet)
{
  size_t wL =strlen(content);
  const char *wPtr=firstNotinSet(pSet);
  if (wPtr==nullptr)
    return(*this);
  wL=wL-(content-(char *)wPtr);
  strncpy(content,wPtr,wL);
  return (*this);
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
const char *
utftemplateString<_Sz,_Utf>::firstNotinSet (const char *pSet)
{
  return((char *)_firstNotinSet((const char*)content,pSet));
}

/**
 * @brief utftemplateString::RTrim Suppresses ending characters that are in pSet (defaulted to __DEFAULTDELIMITERSET__)
 * @param pSet character set to trim off
 * @return a reference to current utftemplateString object
 */
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::RTrim (const char *pSet)
{
  size_t wj;
  size_t w1=strlen(content)-1;

  //   for (w1=strlen(content);(w1 >= 0)&&(content[w1]==' ');w1--); // empty for loop
  for (;(w1 >= 0);w1--)
  {
    for (wj=0;pSet[wj]!='\0';wj++)
      if (content[w1]==pSet[wj])
      {
        content[w1]='\0';
        break;
      }
    if (wj==strlen(pSet))
      break;

  }
  return (*this);
}//   RTrim
template <size_t _Sz,class _Utf>
utftemplateString<_Sz,_Utf>&
utftemplateString<_Sz,_Utf>::Trim(const char *pSet)
{
  LTrim(pSet);
  return(RTrim(pSet));
}

/**
 * @brief strchrReverse find the first occurrence of pChar within pStr string in reverse (starting to strlen() and ending at 0)
 * @param pChar the character to find
 * @return pointer to pChar occurrence within string if found, NULL if not found
 */
template <size_t _Sz,class _Utf>
const char *
utftemplateString<_Sz,_Utf>::strchrReverse(const char pChar)
{
  for (int w1=strlen(content);(w1 >= 0)&&(content[w1]!=pChar);w1--)
    return ((w1<0) ? nullptr:(char*) &content[w1]);     // return NULL pointer if not found pointer to pChar within string if found
}

/**
 * @brief utftemplateString<_Sz,_Utf>::strspnReverse
 *          finds the first occurrence of one of the characters set given by pSet within keywordString::content in reverse (starting to strlen() and ending at 0)
 * @param pSet set of characters to search for (defaulted to  __DEFAULTDELIMITERSET__).
 * @return pointer to pChar occurrence within keywordString::content if found, NULL if not found
 */
template <size_t _Sz,class _Utf>
const char *
utftemplateString<_Sz,_Utf>::strspnReverse( const char *pSet)
{
  size_t wL = strlen(pSet);
  size_t wj = 0;
  long wi;
  for (wi=strlen(content);(wi >= 0)&&(content[wi]!=pSet[wj]);wi--)
  {
    for (wj=0;(wj<wL)&&(content[wi]!=pSet[wj]);wj++);
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
utftemplateString<_Sz,_Utf>::startsCaseWith(const char* pString,const char* pSet)
{
  _LTrim(content,pSet);
  if (strncasecmp(content,pString,strlen(pString)))
    return true;
  return (false);
}
/**
 * @brief utftemplateString::startsWith Test if String starts with pString after having trimmed out character from pSet
 * @param[in] pString substring to search
 * @param[in] pSet  character set to trim off (defaulted to __DEFAULTDELIMITERSET__)
 * @return true if found, false if not found
 */
template <size_t _Sz,class _Utf>
bool
utftemplateString<_Sz,_Utf>::startsWith(const char* pString,const char* pSet)
{
  _LTrim(content,pSet);
  if (strncmp(content,pString,strlen(pString)))
    return true;
  return (false);
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
utftemplateString<_Sz,_Utf>::startsWithApprox(const char* pString,ssize_t pApprox,const char* pSet)
{
  _LTrim(content,pSet);
  const char* wPtr = _firstinSet(content,pSet);
  ssize_t wSize;
  if (wPtr==nullptr)
    wSize=strlen(content);
  else
    wSize=wPtr-content;
  if (wSize<pApprox)
    return false;
  if (strncmp(content,pString,wSize)==0)
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
utftemplateString<_Sz,_Utf>::startsCaseWithApprox(const char* pString,int pApprox,const char* pSet)
{
  _LTrim(content,pSet);
  const char* wPtr = _firstinSet(content,pSet);
  size_t wSize;
  if (wPtr==nullptr)
    wSize=strlen(content);
  else
    wSize=wPtr-content;
  if (wSize<pApprox)
    return false;
  if (strncasecmp(content,pString,wSize)==0)
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
utftemplateString<_Sz,_Utf>::Substr(const char* pSub, int pOffset)
{
  const char * wPtr = strstr((const char*)&content[pOffset],pSub);
  if (wPtr==nullptr)
    return -1;
  return((int)(wPtr-content));
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
utftemplateString<_Sz,_Utf>::SubstrCase(const char *pSub, int pOffset)
{
  char * wPtr = strcasestr(&content[pOffset],pSub);
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
utftemplateString<_Sz,_Utf>::Left (int pLen)
{
  utftemplateString<_Sz,_Utf> wU ;
  strncpy(wU.content,content,pLen);
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
utftemplateString<_Sz,_Utf>::Right (int pLen)
{
  utftemplateString<_Sz,_Utf> wU ;
  int wStart= strlen(content)-pLen;
  if (wStart<0)
    wStart=0;
  strncpy(wU.content,&content[wStart],pLen);
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
  wZDB.decodeB64(content, size()+1);  // decode to ZDataBuffer
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
  wZDB.encodeB64(content, size()+1);  // encode to a ZDataBuffer
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
  ZDataBuffer wZDB ;
  wZDB.encodeMD5((unsigned char*)content, size()+1);  // encode to a ZDataBuffer
  wZDB.moven(content,sizeof(content));    // move back the encoded string to utftemplateString content
  content[wZDB.Size] = '\0';
  CryptMethod = ZCM_MD5;
  return *this;
}
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
template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::_exportURF exports the content of string to a ZDataBuffer in an URF format
 *
 *
 *header :
 *   - ZTypeBase (ZType_FixedCString)
 *   - uint16_t (canonical capacity) number of characters.  example : cst_desclen+1 (canonical length of descString)
 *   - uint16_t effective string length termination char L'\0' EXCLUDED
 *
 *  see <ZType_Type vs Header sizes> in file <zindexedfiles/znaturalfromurf.cpp>
 *data :
 *  char sequence :
 *
 *  Maximum size is in bytes :      65535
 *  Maximum number of characters :  65534
 *
 *
 * @param[out] pUniversal   Data in URF Format
 *
 * @return
 */

ZDataBuffer*
utftemplateString<_Sz,_Utf>::_exportURF(ZDataBuffer*pURFData)
{

  ZTypeBase wType=ZType_FixedCString;
  uint16_t wCanonical=(uint16_t)capacity();

  size_t wUniversalSize=strlen(content);

  if (wUniversalSize>=UINT16_MAX)
  {
    fprintf (stderr,"%s>>Fatal error fixed string maximum capacity overflow <%d> while <%d> authorized.\n",
        _GET_FUNCTION_NAME_,
        wUniversalSize,
        UINT16_MAX);
    _ABORT_ ;
  }
  uint16_t wUSizeReversed= (uint16_t)wUniversalSize;

  size_t wURFByteSize=(size_t)(wUniversalSize+(sizeof(uint16_t)*2))+sizeof(ZTypeBase);
  size_t wOffset=0;
  //uint32_t wUSizeReversed=_reverseByteOrder_T<uint32_t>(wUniversalSize+1);

  pURFData->allocateBZero((ssize_t)wURFByteSize);
  // URF Header is
  wType=reverseByteOrder_Conditional<ZTypeBase>(wType);        // ZTypeBase in reverseOrder if LE (if little endian)
  memmove(pURFData->Data,&wType,sizeof(wType));
  wOffset=sizeof(wType);

  wCanonical=reverseByteOrder_Conditional<uint16_t>(wCanonical);             // Capacity (canonical length) in reverse order if LE
  memmove(pURFData->Data+wOffset,&wCanonical,sizeof(wCanonical));
  wOffset+=sizeof(uint16_t);

  wUSizeReversed=reverseByteOrder_Conditional<uint16_t>(wUSizeReversed);       // URF effective byte size including header size in reverse order if LE
  memmove(pURFData->Data+wOffset,&wUSizeReversed,sizeof(wUSizeReversed));
  wOffset+=sizeof(uint16_t);

  memmove(pURFData->Data+wOffset,content,(size_t)wUniversalSize);  // nb: '\0' is put by difference of 1 char in the end

  return  pURFData;
}// _exportURF

template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::_importURF
 * @param pUniversal pointer to URF data header
 * @return
 */
utftemplateString<_Sz,_Utf> &
utftemplateString<_Sz,_Utf>::_importURF(unsigned char* pURFDataPtr)
{
  ZTypeBase   wType;
  uint16_t    wCanonical;
  uint16_t    wUniversalSize;
  //size_t      wURFByteSize;
  size_t      wOffset=0;

  memmove(&wType,pURFDataPtr,sizeof(ZTypeBase));
  wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
  wOffset+= sizeof(ZTypeBase);

  memmove(&wCanonical, pURFDataPtr+wOffset,sizeof(wCanonical));
  wCanonical=reverseByteOrder_Conditional<uint16_t>(wCanonical);
  wOffset+= sizeof(uint16_t);

  memmove(&wUniversalSize, pURFDataPtr+wOffset,sizeof(wUniversalSize));
  wUniversalSize=reverseByteOrder_Conditional<uint16_t>(wUniversalSize);
  wOffset+= sizeof(uint16_t);


  if (wUniversalSize>_capacity)
  {
    fprintf(stderr,
        "%s>> Error <%s> Capacity of utftemplateString overflow: requested %d while capacity is %ld . String truncated.\n",
        _GET_FUNCTION_NAME_,
        decode_ZStatus(ZS_FIELDCAPAOVFLW),
        wUniversalSize,
        _capacity);
    wUniversalSize=_capacity-1 ;
  }
  memset(content,0,sizeof(content));
  memmove(content,pURFDataPtr+wOffset,wUniversalSize);

  return *this;
}// _importURF

template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::_exportVUniversal  Exports a fixed string to a Varying Universal Format
 *  Varying Universal Format stores fixed string data into a varying length string container excluding '\0' character terminator
 *  leaded by an uint16_t mentionning the number of characters of the string that follows.
 *
 * @param pUniversal a ZDataBuffer with string content in Varying Universal Format
 * @return
 */
ZDataBuffer*
utftemplateString<_Sz,_Utf>::_exportUVF(ZDataBuffer*pUniversal)
{
  uint16_t wUByteSize_Exp,wUByteSize=(uint16_t)strlen(content);

  //uint32_t wUSizeReversed=_reverseByteOrder_T<uint32_t>(wUniversalSize+1);

  pUniversal->allocateBZero((ssize_t)wUByteSize+sizeof(uint16_t));
  // URF Header is

  wUByteSize_Exp=reverseByteOrder_Conditional<uint16_t>(wUByteSize);       // effective number of characters of the string in reverse order if LE
  memmove(pUniversal->Data,&wUByteSize_Exp,sizeof(wUByteSize_Exp));

  memmove(pUniversal->Data+sizeof(wUByteSize),content,(size_t)wUByteSize);  // nb: '\0' is excluded
  pUniversal->Dump();
  return pUniversal;
}// _exportUniversal

template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString<_Sz,_Utf>::_importVUniversal Import string from Varying Universal Format
 *  Varying Universal Format stores string data into a varying length string container excluding '\0' character terminator
 *  leaded by a uint16_t mentionning the number of characters of the string that follows.
 *
 * @param pUniversalPtr pointer to Varying Universal formatted data header
 * @return total size of consumed bytes in pUniversalPtr buffer (Overall size of string in VUF)
 */
size_t
utftemplateString<_Sz,_Utf>::_importUVF(unsigned char* pUniversalPtr)
{
  uint16_t    wUByteSize;

  memmove(&wUByteSize, pUniversalPtr,sizeof(wUByteSize));
  wUByteSize=reverseByteOrder_Conditional<uint16_t>(wUByteSize);

  if (wUByteSize>_Sz)  // use of _Sz in place of _capacity must NOT be changed
  {
    fprintf(stderr,
        "%s>> Warning: <%s> Capacity of utftemplateString overflow: requested %d while capacity is %ld . String truncated.\n",
        _GET_FUNCTION_NAME_,
        decode_ZStatus(ZS_FIELDCAPAOVFLW),
        wUByteSize,
        _Sz);
    wUByteSize=_Sz-1;
  }
  memset(content,0,sizeof(content));
  if (wUByteSize>0)
    memmove(content,pUniversalPtr+sizeof(wUByteSize),wUByteSize);

  return (sizeof(wUByteSize)+wUByteSize);
}// _importUniversal

template <size_t _Sz,class _Utf>
/**
 * @brief utftemplateString::getUniversalFromURF
 *      Static method that extract Universal value from memory zone pointed by pURFDataPtr
 *      and returns a ZDataBuffer loaded with Universal value padded with binary zeros to its canonical number of characters.
 *
 *      output ZDB is of capacity size corresponding to canonical array count * character size
 *      output data is padded with '\0' character up until capacity of current fixed string.
 * @param pURFDataPtr
 * @param pUniversal
 * @return
 */
ZStatus
utftemplateString<_Sz,_Utf>::getUniversalFromURF(unsigned char* pURFDataPtr,ZDataBuffer &pUniversal,unsigned char** pURFDataPtr)
{
  uint16_t wCanonSize , wEffectiveUSize;
  ZTypeBase wType;
  unsigned char*wDataPtr=pURFDataPtr;
  memmove(&wType,wDataPtr,sizeof(ZTypeBase));
  wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
  if (wType!=ZType_FixedCString)
  {
    fprintf (stderr,
        "%s>> Error invalid URF data type <%X> <%s> while getting universal value of <%s> ",
        _GET_FUNCTION_NAME_,
        wType,
        decode_ZType(wType),
        decode_ZType(ZType_FixedCString));
    return ZS_INVTYPE;
  }
  wDataPtr += sizeof (ZTypeBase);

  memmove (&wCanonSize,wDataPtr,sizeof(wCanonSize));
  wCanonSize=reverseByteOrder_Conditional<uint16_t>(wCanonSize);
  wDataPtr += sizeof (uint16_t);

  memmove (&wEffectiveUSize,wDataPtr,sizeof(wEffectiveUSize));
  wEffectiveUSize=reverseByteOrder_Conditional<uint16_t>(wEffectiveUSize);
  wDataPtr += sizeof (uint16_t);

  pUniversal.allocateBZero(wCanonSize);       // fixed string must have canonical characters count allocated
  memmove(pUniversal.DataChar,wDataPtr,(size_t)wEffectiveUSize); // effective number of char is effective universal size
      // without '\0' terminator
  if (pURFDataPtrOut)
  {
    *pURFDataPtrOut = wURFDataPtr + wEffectiveUSize;
  }

  return ZS_SUCCESS;
}//getUniversalFromURF
/**
 * @brief utftemplateString::getUniversalFromURF_Truncated
 *      Static method that extract Universal value from memory zone pointed by pURFDataPtr
 *      and returns a ZDataBuffer loaded with Universal value truncated to its effective number of characters.
 *      output ZDB is of capacity size corresponding to canonical array count * character size
 *      output data is padded with '\0' character up until capacity of current fixed string.
 * @param pURFDataPtr
 * @param pUniversal
 * @return
 */
template <size_t _Sz,class _Utf>
ZStatus
utftemplateString<_Sz,_Utf>::getUniversalFromURF_Truncated(unsigned char* pURFDataPtr,ZDataBuffer &pUniversal,unsigned char** pURFDataPtrOut)
{
  uint16_t wCanonSize , wEffectiveUSize;
  ZTypeBase wType;
  unsigned char*wDataPtr=pURFDataPtr;
  memmove(&wType,wDataPtr,sizeof(ZTypeBase));
  wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
  if (wType!=ZType_FixedCString)
  {
    fprintf (stderr,
        "%s>> Error invalid URF data type <%X> <%s> while getting universal value of <%s> ",
        _GET_FUNCTION_NAME_,
        wType,
        decode_ZType(wType),
        decode_ZType(ZType_FixedCString));
    return ZS_INVTYPE;
  }
  wDataPtr += sizeof (ZTypeBase);

  memmove (&wCanonSize,wDataPtr,sizeof(wCanonSize));
  wCanonSize=reverseByteOrder_Conditional<uint16_t>(wCanonSize);
  wDataPtr += sizeof (uint16_t);

  memmove (&wEffectiveUSize,wDataPtr,sizeof(wEffectiveUSize));
  wEffectiveUSize=reverseByteOrder_Conditional<uint16_t>(wEffectiveUSize);
  wDataPtr += sizeof (uint16_t);

  pUniversal.setData(wDataPtr,(size_t)wEffectiveUSize);   // effective number of char is effective universal size
      // without '\0' terminator
  if (pURFDataPtrOut)
  {
    *pURFDataPtrOut = wURFDataPtr + wEffectiveUSize;
  }
  return ZS_SUCCESS;
}//getUniversalFromURF_Truncated

//=========================== Conversion to and from=========================




//-----------End Encoding & Decoding-----------------------------------------









//-------------End utftemplateString Methods expansion---------------------------------------
#endif // __COMMENT__






//=================END Base Template utftemplateString SECTION=============================








//-------------End utftemplateString Methods expansion---------------------------------------





#endif // UTFVSPRINTF_H

