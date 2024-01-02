#ifndef UTFUTILS_H
#define UTFUTILS_H

/**
  *  Contains routines to manipulate utf format strings.
  *
  *
  * includes numeric to utf string and reciprocally conversions
  *
  *  - utfChartoint : conversion from an utf character to its numeric digit value as an int
  *  - utfStrtoint : conversion from an utf string to int (signed)
  *  - utfStrtol   : conversion from an utf string to long (signed)
  *  - utfStrtoul  : conversion from an utf string to unsigned long
  *  - utfStrtod   : conversion from an utf string to double
  *
  *
  *  - utfItoa      :    from int to Utf string according a base
  *  - utfLtoa      : from long to Utf string according a base
  *
  *
  *
  *
  *
  *
  */

#include <config/zconfig.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "zmem.h" // for zfree()
#include "zcharset.h"

#include "zatomicconvert.h"





/*
template <class _Utf>
_Utf* utfStrdup(const _Utf* pString); // see strdup
template <class _Utf>
_Utf* utfStrrev(_Utf* pStr);        // see strrev (strrev is not standard)
*/
//================ numeric to string and string to numeric conversions===================

template <class _Utf>
 bool utfIsdigit(_Utf pChar)
{
    if ((pChar >= (_Utf)'0') && (pChar <= (_Utf)'9'))
                        return true;
    return false;
}
 template <class _Utf>
 bool utfIshexa(_Utf pChar)
 {
     if ((pChar >= (_Utf)'0') && (pChar <= (_Utf)'9'))
                         return true;
     if ((pChar >= (_Utf)'A') && (pChar <= (_Utf)'F'))
                         return true;
     if ((pChar >= (_Utf)'a') && (pChar <= (_Utf)'f'))
                         return true;
     return false;
 }
static  int is_real(double x)
{
    const double Inf = 1.0 / 0.0;
    return (x < Inf) && (x >= -Inf);
}

template <class _Utf>
bool utfIsAsciiChar(_Utf pChar)
{
  if ((pChar >= (_Utf)'0') && (pChar <= (_Utf)'9'))
    return true;
  if ((pChar >= (_Utf)'A') && (pChar <= (_Utf)'Z'))
    return true;
  if ((pChar >= (_Utf)'a') && (pChar <= (_Utf)'z'))
    return true;
 /* switch (pChar)
  {
  case (_Utf)'é':
  case (_Utf)'à':
  case (_Utf)'è':
  case (_Utf)'ü':
  case (_Utf)'ö':
  case (_Utf)'ç':
  case (_Utf)'*':
  case (_Utf)'"':

  }
*/
  return false;
}


enum ZNumBase_type:uint8_t
{
    ZNBase_Nothing = 0,
    ZNBase_Binary = 2,
    ZNBase_Octal = 8,
    ZNBase_Decimal=10,
    ZNBase_Hexa = 16,
    ZNBase_Any  = 0xF0,
    ZNBase_Invalid=0xFF
};
/** ANSI says:
 *   The `utfStrcpy' function copies the string pointed to by `pStr2' (including
 *   the terminating null character) into the array pointed to by `pStr1'.
 *   If copying takes place between objects that overlap, the behavior
 *   is undefined.
 *   The `strcpy' function returns the value of `pStr1'.  [4.11.2.3]
 *  if pStr2 is nullptr nothing is done.
 *  if pStr1 is nullptr nothing is done, nullptr is returned.
 */
template <class _Utf>
_Utf *
utfStrcpy(_Utf *pStr1, const _Utf *pStr2)
{
  if (pStr1==nullptr)
    return nullptr;
  if (pStr2==nullptr)
    return pStr1;
  _Utf *s = pStr1;
  while ((*s++ = *pStr2++) != 0) ;
//  *s=0;
  return (pStr1);
}
template <class _Utf>
/**
 * @brief utfStrdup duplicates the _Utf string pStr into a new memory allocated string.
 * Remark: it is up to calling routine to free allocated memory
 * @param pStr _Utf string to duplicate
 * @return  duplicated _Utf string
 */
_Utf*
utfStrdup(const _Utf* pStr)
{
  if (pStr==nullptr)
    return nullptr;
  size_t wSize=utfStrlen<_Utf>(pStr) ;
  _Utf* wDup= (_Utf*)malloc((wSize+1)*sizeof(_Utf));
  utfStrcpy<_Utf>(wDup,pStr);
  wDup[wSize]=0;
  return wDup;
}

template <class _Utf>
/**
 * @brief utfLTrimSpaces skip space and non breaking space characters at the beginning of the string
 * @param pString
 * @return
 */
 _Utf* utfSkipSpaces(_Utf*pString)
{
  if (pString==nullptr)
    return nullptr;
     for (;(*pString)&&((*pString==(_Utf)0x020)||(*pString==(_Utf)0x0A));pString++);
     return pString;
}
template <class _Utf>
/**
 * @brief utfRTrimSpaces get rid of  space and non breaking space characters at the end of the string.
 * an (_Utf)'\0' is added after the last non space character of the string - or at the first character of the string if string is filled with spaces.
 *  pString content is being modified accordingly when trailing spaces are found.
 * @param pString
 * @return
 */
 _Utf* utfRTrimSpaces(_Utf*pString)
{
  if (pString==nullptr)
    return nullptr;
_Utf* wPtr=pString+ utfStrlen<_Utf>(pString);
     for (;(wPtr>pString)&&(*wPtr)&&((*wPtr==(_Utf)0x020)||(*wPtr==(_Utf)0x0A));wPtr--);
     *wPtr=0;
     return pString;
}

const int cst_UTIMinus      = -0x0F;

const int cst_UTINotDigit   = -1;
const int cst_UTISpace      = -2;
const int cst_UTIDot        = -3;
const int cst_UTIComma      = -4;
const int cst_UTIQuote      = -5;

const int cst_UTIPlus       = -6;

const int cst_UTIExponent   = -7;

const int cst_UTINotInBase  = -9;

template <class _Utf>
/**
 * @brief utftoint converts a single _Utf char into its digit int value. Returns -1, if not a valid digit.
 * @param pChar
 * @param pHexa boolean allowing hexa decimal digits (true) or not (false)
 * @return
 */
 int utftoint(_Utf pChar,ZNumBase_type pBase=ZNBase_Decimal)
{
int wValue=-1;
    while (true)
    {
    if ((pChar>=(_Utf)'0')&&(pChar<=(_Utf)'9'))
            {
            wValue=(int) (pChar-(_Utf)'0');
            break;
            }
    if ((pChar>=(_Utf)'A')&&(pChar<=(_Utf)'F'))
            {
            wValue= (int) (pChar-(_Utf)'A'+10);
            break;
            }
    if ((pChar>=(_Utf)'a')&&(pChar<=(_Utf)'f'))
            {
            wValue= (int) (pChar-(_Utf)'a'+10);
            break;
            }
    if ((pChar==0x20)||(pChar==0x0A))
                        return cst_UTISpace;   // found a space. May be a formatting mark
    if ((pChar==(_Utf)'.'))
                        return cst_UTIDot;     // found a dot
    if ((pChar==(_Utf)','))
                        return cst_UTIComma;   // found a comma
    if ((pChar==(_Utf)'\''))
                        return cst_UTIQuote;   // found a single quote
    if ((pChar==(_Utf)'-'))
                        return cst_UTIMinus;   // found Minus sign
    if ((pChar==(_Utf)'+'))
                        return cst_UTIPlus;    // found Plus sign

    return cst_UTINotDigit;  // not a valid digit and/or admitted punctuation mark
    }// while true

//-----------Check digit value vs requested base-------------------
    switch (pBase)
    {
    case ZNBase_Binary:
        {
        if ((wValue<0)||(wValue>1))
                        break;
        return wValue;
        }
    case ZNBase_Octal:
        {
        if ((wValue<0)||(wValue>7))
                        break;
        return wValue;
        }

    case ZNBase_Decimal:
        {
        if ((wValue<0)||(wValue>9))
            {
            if (pChar==(_Utf)'E')
                        return cst_UTIExponent;
                    else
                        break;
            }
        return wValue;
        }
    case ZNBase_Hexa:
        {
        if ((wValue<0)||(wValue>0x0F))
                        break;
        return wValue;
        }
    }

    return cst_UTINotInBase;  // not a valid digit regarding requested numeric base
}// utfChartoint




template <class _Utf>
/**
 * @brief utfStrtoi converts to an integer an _Utf sequence pointed by pString.
 * if pEndPtr is mentionned, then it will contain in return the first _Utf character after the sequense used.
 * Leading spaces are disgarded.
 * Leading minus or plus sign are taken.
 * No other _Utf characters than digits are allowed inside the string sequense :
 *  example :
 *   -123 is correct
 *   + 123 is ill formed
 *   25 259 is ill formed and will give +25 as resulting number
 *  123- is correct and will give -123
 *  123+ is correct and will give 123
 *
 *
 * Error cases :
 *
 *  If any error, INT_MIN will be returned and errno will be set to EINVAL.
 *  errno is not reset, so that, if no error occurs, errno will stay to its preceeding value without change.
 *  errno cannot be tested to define if there is an error or not.
 *
 *  - invalid requested base : example pBase=3
 *
 *  - requested base is not the one defined in leading mark :
 *      error examples :
 *              pBase=10 and numeric string is "0x123"  -> decimal base while hexadecimal numeric string
 *
 *  - numeric string does not correspond to requested base
 *              pBase=2 and numeric string is "123" -> binary base while decimal value
 *
 *
 * @param[in]   pString _Utf string containing the
 * @param[out]  pEndPtr optional a pointer to an _Utf pointer. If not omitted, will receive the position after integer sequense.
 * @param[in]   pBase mandatory base. Admitted values are
 *  2 for binary
 *  8 for octal
 *  10 for decimal (default value)
 *  16 for hexadecimal
 *  0 for any base : in this case a header like 0x (hexa) or 0b (binary) or 0 (octal)
 *
 * base : 0 -> any base- Base is dependant from string
 * base : 2 -> binary expected  example 0b0010001
 * base : 8 -> octal expected   example 08050
 *     Remark: according to standard notation any numeric litteral starting with leading 0 (not 0x not 0b) is octal
 * base : 16-> hexadecimal expected example 0xAF9B
 *
 * @return an integer value with decoded value from string OR INT_MIN in case of ill base/string
 */
int utfStrtoi(_Utf*pString,_Utf**pEndPtr,int pBase=10)
{
    int wRes=0;
    int wR;
    int wNegative=0;

    if (!pString)
            return 0;

    _Utf* wPtr= utfStrdup<_Utf>(pString);
    _Utf* wPtrToFree=wPtr;

    ZNumBase_type wBase ;
    switch (pBase)
    {
    case 2:
    {
        wBase=ZNBase_Binary;
        break;
    }
    case 8:
    {
        wBase=ZNBase_Octal;
        break;
    }
    case 16:
    {
        wBase=ZNBase_Hexa;
        break;
    }
    case 10:
    {
        wBase=ZNBase_Decimal;
        break;
    }
    case -1:
    {
        wBase=ZNBase_Any;
        break;
    }
    default:
        {
            errno=EINVAL;  // stupid value entered as pBase : set errno and return stupid result
            return INT_MIN;
        }// default
    }// switch


    wPtr=utfSkipSpaces<_Utf>(wPtr);  // get rid of leading spaces (and non breaking spaces)

    _Utf*wPtr1=wPtr;
    if (wPtr1[0]==(_Utf)'-')                // if minus sign leading
        {
        wPtr++;
        wPtr1++;
        wNegative=1;
        }
    // minus sign followed by minus sign will be ill formed but don't care
    if (wPtr1[0]==(_Utf)'+')                // if plus sign leading
        {
        wPtr++;
        wPtr1++;
        wNegative=0;  // reset any minus sign effect if previously found
        }
    // -----------------------Base indication------------------------------
    if (wPtr1[0]==(_Utf)'0')// if any base indication
        {
        if ((wPtr1[1]==(_Utf)'x')||(wPtr1[1]==(_Utf)'X'))
                {
                if ((wBase!=ZNBase_Any) && (wBase!=ZNBase_Hexa))
                        goto utfStrtoi_Incorrect;
                wBase=ZNBase_Hexa;     // this is hexadecimal
                wPtr1=&wPtr[2]; // skip this header
                }
        else
        if ((wPtr1[1]==(_Utf)'b')||(wPtr1[1]==(_Utf)'B'))
                {
                if ((wBase!=ZNBase_Any) && (wBase!=ZNBase_Binary))
                            goto utfStrtoi_Incorrect;
                wBase=ZNBase_Binary;     // this is Binary
                wPtr1=&wPtr[2]; // skip this header
                }
        else
        if ((wPtr1[1]==(_Utf)'0'))
                {
                if ((wBase!=ZNBase_Any) && (wBase!=ZNBase_Octal))
                            goto utfStrtoi_Incorrect;
                wBase=ZNBase_Octal;     // this is Octal
                wPtr1=&wPtr[2]; // skip this header
                }
        }
    /* if no heading base indication and wBase is ZNBase_Any, then it is assumed to be decimal */
    if (wBase==ZNBase_Any)
            wBase=ZNBase_Decimal;

    while(*wPtr1)
            {
            if ((wR = utftoint<_Utf>(*wPtr1,wBase))>=0)
                        {
                        wRes *= (int)wBase;// because ZNBase_type  has base value in its type description
                        wRes+=wR ;
                        }
                else
                {
                break; // up to non valid digit
                }
            wPtr1++;
            }// while
    if ((wR==cst_UTIMinus)||(wNegative))  // if minus sign trailing or leading
                {
                    wRes=-wRes;
                    wPtr1++;
                }

    if (pEndPtr)
        {
        *pEndPtr=wPtr1;
        }


    zfree(wPtrToFree);
    return wRes;
utfStrtoi_Incorrect:
    zfree(wPtrToFree);
    errno=EINVAL;       // set errno
    return INT_MIN;     // and return invalid value
}//utfStrtoi





#include <limits.h>
/**
 * @brief utfStrtoul converts pString to long (signed) according base
 * base : 0 -> any base- Base is dependant from string
 * base : 2 -> binary expected  example 0b0010001
 * base : 8 -> octal expected   example 08050
 *     Remark: according to standard notation any numeric litteral starting with leading 0 (not 0x not 0b) is octal
 * base : 16-> hexadecimal expected example 0xAF9B
 *
 * Strings with negative numeric value generate errno to be set to EINVAL while returned value is set to ULONG_MAX,
 * and an error message is displayed on stderr.
 */
template <class _Utf>
long utfStrtol(const _Utf*pString,_Utf**pPtrend,int base)
{
ZNumBase_type wBase;

    if (!pString)
            return 0;
    bool wHexa=false ;
    _Utf* wPtr= utfStrdup<_Utf>(pString);
    _Utf* wPtrToFree=wPtr;

    long wRes;
    _Utf wUtf;
    unsigned long wCutoff;
    int wNeg = 0, wAny, wCutlim;

    int wValue;

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */


    wPtr=utfSkipSpaces(wPtr);// skip space and non breaking space characters
/** */
    wUtf=*wPtr++;
    if (wUtf == (_Utf)'-') {
            wNeg = 1;
            wUtf = *wPtr++;
        } else if (wUtf ==(_Utf) '+')
            wUtf = *wPtr++;
        if ((base == 0 || base == 16) &&
            wUtf == (_Utf)'0' && (*wPtr == (_Utf)'x' || *wPtr == (_Utf)'X')) {
            wUtf = wPtr[1];
            wPtr += 2;
            base = 16;
        } else if ((base == 0 || base == 2) &&
            wUtf == (_Utf)'0' && (*wPtr == (_Utf)'b' || *wPtr == (_Utf)'B')) {
            wUtf = wPtr[1];
            wPtr += 2;
            base = 2;
        }
        if (base == 0)
            base = (wUtf == (_Utf)'0') ? 8 : 10;

        switch(base)
        {
        case 10:
          wBase=ZNBase_Decimal;
          break;
        case 2:
          wBase=ZNBase_Binary;
          break;
        case 8:
          wBase=ZNBase_Octal;
          break;
        case 16:
          wBase=ZNBase_Hexa;
          break;
        }



//=================================================

    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value, divided by the
     * base.  An input number that is greater than this value, if
     * followed by a legal input character, is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance, if the range for longs is
     * [-2147483648..2147483647] and the input base is 10,
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
     * a value > 214748364, or equal but the next digit is > 7 (or 8),
     * the number is too big, and we will return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */
    wCutoff = wNeg ? -(unsigned long)LONG_MIN : LONG_MAX;
    wCutlim = wCutoff % (unsigned long)base;
    wCutoff /= (unsigned long)base;
    for (wRes = 0, wAny = 0;; wUtf = *wPtr++) {
        if ((wUtf==(_Utf)0x20) ||(wUtf==(_Utf)0x0A)) // skip space and non-breaking space within number (formatting marks)
                                    continue;
        if ((wValue= utftoint(wUtf,wBase))<0)  // returns -1 if not valid digit according wHexa (hexadecimal or not)
                                        break ;
        if (wValue >= base)
                        break;
        if (wAny < 0 || wRes > wCutoff || wValue == wCutoff && wValue > wCutlim)
            wAny = -1;
        else {
            wAny = 1;
            wRes *= base;
            wRes += wValue;
        }
    }// for
    if (wAny < 0) {
        wRes = wNeg ? LONG_MIN : LONG_MAX;
        errno = ERANGE;
    } else if (wNeg)
        wRes = -wRes;
    if (pPtrend != 0)
        *pPtrend = (_Utf *)(wAny ? wPtr - 1 : pString);

    zfree(wPtrToFree);

    return (wRes);
}//utfStrtol

/**
 * @brief utfStrtoul converts pString to unsigned long according base
 * base : 0 -> any base- Base is dependant from string
 * base : 2 -> binary expected  example bx0010001
 * base : 8 -> octal expected   example Ox0805
 * base : 16-> hexadecimal expected example 0xAF9B
 *
 * Strings with negative value generate errno to be positionned to EINVAL while returned value is set to ULONG_MAX,
 * and an error message is displayed on stderr.
 */
template <class _Utf>
unsigned long utfStrtoul(_Utf*pString,_Utf**pPtrend,int base=0)
{

    if (!pString)
            return 0;
    ZNumBase_type wBase=ZNBase_Any ;

    _Utf* wPtr= utfStrdup<_Utf>(pString);
    _Utf *wPtrToFree = wPtr;

    unsigned long wResult;
    _Utf wUtf;
    unsigned long wCutoff;
    int wNeg = 0, wAny, wCutlim;

    int wValue;

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */


    wPtr=utfSkipSpaces(wPtr);// skip space and non breaking space characters
/** */
    wUtf = *wPtr;
    wPtr++;
    if (wUtf == (_Utf)'-') {
            wNeg = 1;
            wUtf = *wPtr++;
            fprintf(stderr,
                "utfStrtoul-F-INVTEXT  Invalid unsigned long character string <%s>\n"
                "                      Cannot cast negative value to unsigned long.\n",
                pString);
            errno = EINVAL;
            return ULONG_MAX;
        } else if (wUtf ==(_Utf) '+')
            wUtf = *wPtr++;
        if ((base == 0 || base == 16) &&
            wUtf == (_Utf)'0' && (*wPtr == (_Utf)'x' || *wPtr == (_Utf)'X')) {
            wUtf = wPtr[1];
            wPtr += 2;
            base = 16;
            wBase=ZNBase_Hexa;
        } else if ((base == 0 || base == 2) &&
            wUtf == (_Utf)'0' && (*wPtr == (_Utf)'b' || *wPtr == (_Utf)'B')) {
            wUtf = wPtr[1];
            wPtr += 2;
            base = 2;
            wBase=ZNBase_Binary;
        }
        if (base == 0)
            base = (wUtf == (_Utf)'0') ? 8 : 10;

        switch(base)
        {
        case 10:
          wBase=ZNBase_Decimal;
          break;
        case 2:
          wBase=ZNBase_Binary;
          break;
        case 8:
          wBase=ZNBase_Octal;
          break;
        case 16:
          wBase=ZNBase_Hexa;
          break;
        }

//=================================================

    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value, divided by the
     * base.  An input number that is greater than this value, if
     * followed by a legal input character, is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance, if the range for longs is
     * [-2147483648..2147483647] and the input base is 10,
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
     * a value > 214748364, or equal but the next digit is > 7 (or 8),
     * the number is too big, and we will return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */
    wCutoff = wNeg ? -(unsigned long)LONG_MIN : LONG_MAX;
    wCutlim = wCutoff % (unsigned long)base;
    wCutoff /= (unsigned long)base;
    for (wResult = 0, wAny = 0;; wUtf = *wPtr++) {
        if ((wUtf==(_Utf)0x20) ||(wUtf==(_Utf)0x0A)) // skip space and non-breaking space within number (formatting marks)
                                    continue;
        if ((wValue= utftoint(wUtf,wBase))<0)  // returns -1 if not valid digit according wHexa (hexadecimal or not)
                                        break ;
        if (wValue >= base)
                        break;
        if ( (wAny < 0) || (wResult > wCutoff) || (wValue == wCutoff) && (wValue > wCutlim) )
            wAny = -1;
        else {
            wAny = 1;
            wResult *= base;
            wResult += wValue;
        }
    }// for
    if (wAny < 0) {
        wResult = wNeg ? LONG_MIN : LONG_MAX;
        errno = ERANGE;
    } else if (wNeg)
        wResult = -wResult;
    if (pPtrend != 0)
        *pPtrend = (_Utf *)(wAny ? wPtr - 1 : pString);

    zfree(wPtrToFree);
    return (wResult);
}//utfStrtoul

#include <errno.h>
//#include <float.h>  // __DBL_MIN_EXP__ and __DBL_MAX_EXP__ or see math.h


template <class _Utf>
/**
 * @brief utfStrtod converts a string to a double
        adapted for _Utf from strtod.c Copyright (C) 2002 Michael Ringgaard. All rights reserved.
        In case of success errno is set to 0
        In case of error errno is positionned to
        ENOMEM : cannot allocate memory
        EINVAL : no digits have been detected
        ERANGE : value exceeds double capacity


 * @param pString
 * @param pEndptr
 * @return a double
 */
double utfStrtod(const _Utf *pString, const _Utf **pEndptr)
 {

    const _Utf* wPtr=utfSkipSpaces(pString);// skip space and non breaking space characters

     double wNumber;
     int exponent;
     int negative;
     double p10;
     int n;
     int num_digits;
     int num_decimals;
     const double Inf = 1.0 / 0.0;
     // Handle optional sign
     negative = 0;
     switch (*wPtr) {
     case (_Utf)'-':
     negative = 1;       // Fall through to increment position
     case (_Utf)'+':
     wPtr++;
     }

     wNumber = 0.;
     exponent = 0;
     num_digits = 0;
     num_decimals = 0;

     // Process string of digits
     while (utfIsdigit<_Utf>(*wPtr)) {
     wNumber = wNumber * 10. + (*wPtr - '0');
     wPtr++;
     num_digits++;
     }

     // Process decimal part
     if (*wPtr == (_Utf)'.')
     {
     wPtr++;

     while (utfIsdigit<_Utf>(*wPtr))
        {
         wNumber = wNumber * 10. + (double)(*wPtr - (_Utf)'0');
         wPtr++;
         num_digits++;
         num_decimals++;
        }// while

     exponent -= num_decimals;
     }//if (*wPtr == (_Utf)'.')

     if (num_digits == 0)
        {
         errno = EINVAL;
         return 0.0;
         }
     // Correct for sign
     if (negative)
     wNumber = -wNumber;

     // Process an exponent string
     if (*wPtr == (_Utf)'e' || *wPtr == (_Utf)'E')
        {
     // Handle optional sign
     negative = 0;
     switch (*++wPtr)
     {
     case (_Utf)'-':
         negative = 1;   // Fall through to increment pos
     case (_Utf)'+':
         wPtr++;
     }// switch

     // Process string of digits
     n = 0;
     while (utfIsdigit<_Utf>(*wPtr)) {
         n = n * 10 + (int)(*wPtr - (_Utf)'0');
         wPtr++;
     }

     if (negative)
         exponent -= n;
     else
         exponent += n;
     }

     if (exponent < __DBL_MIN_EXP__ || exponent > __DBL_MAX_EXP__) {
                    errno = ERANGE;
                    return Inf;
                    }
     // Scale the result
     p10 = 10.;
     n = exponent;
     if (n < 0)
     n = -n;
     while (n) {
     if (n & 1) {
         if (exponent < 0)
         wNumber /= p10;
         else
         wNumber *= p10;
     }
     n >>= 1;
     p10 *= p10;
     }

     if (!is_real(wNumber))
                errno = ERANGE;
     if (pEndptr)
        *pEndptr = wPtr;

     return wNumber;
}// utfStrtod

//================= string operations==============================

template<class _Utf>
/**
 * @brief utfStrnset copies wSrc content for at maximum pSrcMaxCount units to pDest containing at maximum pMaxUnit.
 *  Destination string is padded with 0 up to pMaxUnit.
 * @param pDest     Destination utf string
 * @param wSrc      Source utf string.
 * @param pCapacity Maximum number of character units available in pDest
 * @param pSrcMaxCount  Maximum number of character units to copy from wSrc.
 * @return
 */

_Utf* utfStrnset ( _Utf *pDest, const _Utf *pSrc,ssize_t pCapacity,ssize_t pSrcMaxCount)
{
    if (!pSrc)
            return pDest;
    _Utf*wPtr=pDest;
    while (*pSrc && pSrcMaxCount-- && pCapacity--)
                            *wPtr++=*pSrc++;
    while (pCapacity-- > 0)
            *wPtr++=(_Utf)'\0';

    return pDest;
}// utfStrnset

template <class _Utf>
/**
 * @brief utfNSetReverse sets an _Utf string pStringOut with pOutMax character units
 *      with _Utf string data pDataIn of pInSize character units length
 *      until (_Utf)'\0' is reached or at maximum with pInSize character units
 *      Each _Utf character is reversed according system endianness.
 *      Number of character units copied cannot be greater than pOutMax-2
 *      Complements pStringOut to (_Utf)0
 * @param pStringOut    _Utf string destination of the copy
 * @param pDataIn       Input _Utf data to copy
 * @param pInSize       Maximum number of character units to copy from pDataIn
 * @param pOutMax       Number of total character units available within pStringOut
 */
void utfNSetReverse(_Utf*pStringOut,const _Utf*pDataIn,const size_t pInCount,const size_t pOutCapacity)
{

    size_t wi=0;
    if (!pStringOut)
            return;
    if (!pDataIn)
            return;

    if (sizeof(_Utf)==1)
      while ((wi<pOutCapacity-2)&&(wi++<pInCount)) // checked
        *pStringOut++=*pDataIn++ ;
    else
      while ((wi<pOutCapacity-2)&&(wi++<pInCount)) // checked
        *pStringOut++=reverseByteOrder_Conditional<_Utf>(*pDataIn++) ;

    while (wi++<pOutCapacity)  // complement to binary zero
            *pStringOut++ = 0;
    return;
}

template <class _Utf>
/**
 * @brief utfSetReverse sets an _Utf string pStringOut
 *      with _Utf string data pDataIn with at maximum pInSize character units length
 *      Each _Utf character is reversed according system endianness.
 *      Number of character units copied cannot be greater than pDataIn
 *      Ending mark (_Utf)0 is NOT copied.
 * @param pStringOut    _Utf string destination of the copy
 * @param pDataIn       Input _Utf data to copy
 * @param pInSize       Maximum number of character units to copy from pDataIn
 */
void utfSetReverse(_Utf*&pStringOut,const _Utf*&pDataIn,const size_t pInCount)
{
    size_t wi=0;
    if (!pStringOut)
            return;
    if (!pDataIn)
            return;
    if (sizeof(_Utf)==1)
      while (wi++<pInCount) // checked
        *pStringOut++=*pDataIn++ ;
    else
      while (wi++<pInCount) // checked
              *pStringOut++=reverseByteOrder_Conditional<_Utf>(*pDataIn++) ;
    return;
}
#ifdef __DEPRECATED__
#include "zdatabuffer.h"
void  _exportCharArrayUVF(const char*pDataIn,ZDataBuffer & wReturn);
size_t _getexportCharArrayUVFSize(const char*pDataIn);
/** @brief _importCharArrayUVF imports pUniversalPtr an array of char in UVF from into pDataOut with size max pMaxSize
 *  pUniversalPtr is updated to point to first byte next to imported data
 */
size_t _importCharArrayUVF(char* pDataOut, size_t pMaxSize,const unsigned char *&pUniversalPtr);
/** @brief _getimportCharArrayUVFSize computes required size for importing UVF format pUniversalPtr */
size_t _getimportCharArrayUVFSize(const unsigned char*& pUniversalPtr);
#endif // __DEPRECATED__
template <class _Utf>
/* Compare S1 and S2, returning less than, equal to or
   greater than zero if S1 is lexicographically less than,
   equal to or greater than S2.  */
 int
utfStrcmp (const _Utf *p1, const _Utf *p2)
{
  const _Utf *s1 = (const _Utf *) p1;
  const _Utf *s2 = (const _Utf *) p2;
  while ((*s1++ == *s2++ )&&(*s1)&&(*s2));
  return *s1 - *s2;
}

/*template <class _UTF>
int utfStrncmp_T(_UTF* pString1, _UTF* pString2,int pMaxSize);*/
/**
 * @brief utfStrncmp_T compare pString1 to pString2 content at maximum pMaxSize utf8 characters.
 *      if pString1 is strictly equal to pString2 returns 0.
 *      return >0 at the first character of pString1 greater than the pString2 same position
 *      returns <0 at the first character of pString1 lower thant pString2 same position
 *      returns 1 if any character of pString2 equals pString1 content but pString1 is longer than pString2
 *      returns -1 if any character of pString 1 equals pString2 content but pString2 is longer than pString1
 *
 *      Comparizon is made on at maximum pMaxSize if greater than 0 or on the whole string if equal or less than 0
 *
 * @param pString1
 * @param pString2
 * @param pMaxSize
 * @return
 */
template <class _Utf>
 int
utfStrncmp(const _Utf* pString1,const  _Utf* pString2,int pCountMax)
{
  assert (pCountMax > 0);

  if (pString1==pString2)
                return 0;
  if (pString1==nullptr)
                return -1;
  if (pString2==nullptr)
                return 1;

  int wi=0;
  int wComp = (int)(pString1[wi] - pString2[wi]);
  wi++;

  for (;(!wComp) && pString1[wi] && pString2[wi] && (wi < pCountMax); wi++ ) {
    wComp = (int)(pString1[wi] - pString2[wi]);
  }
  if (wComp)
    return wComp;
  if (wi==pCountMax)
    return wComp;
    // up to here wComp==0 (equality)
    //    test string lengths
  if (pString1[wi]==0) { // string 1 exhausted
    if (pString2[wi]==0)   // so is string2
      return 0;   // same content and lengths are equal
    return -1; // string 2 is greater than string1 (because more characters)
  }

// up to here wComp==0 but string1 is not exhausted

  return 1; // string 1 is longer

}//utfStrncmp

template <class _Utf>
/**
 * @brief utfStrncasecmp compares 2 _Utf strings on pCountMax characters case regardless.
 *
 *      if pString1 is strictly equal to pString2 returns 0.
 *      return >0 at the first character of pString1 greater than the pString2 same position
 *      returns <0 at the first character of pString1 lower thant pString2 same position
 *      returns 1 if any character of pString2 equals pString1 content but pString1 is longer than pString2
 *      returns -1 if any character of pString 1 equals pString2 content but pString2 is longer than pString1
 * @param pString1
 * @param pString2
 * @param pCountMax
 * @return
 */
 int
utfStrncasecmp(const _Utf* pString1, const _Utf* pString2,int pCountMax)
{
    if (pString1==pString2)
                return 0;
    if (pString1==nullptr)
                return -1;
    if (pString2==nullptr)
                return 1;
    if (pCountMax<1)
                pCountMax=0;
//    int wCount = pCountMax>0?0:-1;
//    int wCount =0;

//    _Utf wComp1,wComp2;

//    utf8_t w1,w2;
//    w1=utfUpper<_Utf>(*pString1);
//    w2=utfUpper<_Utf>(*pString2);
    int wi=0;
    int wComp = (int)(utfUpper<_Utf>(pString1[wi]) - utfUpper<_Utf>(pString2[wi]));
    wi++;
    for (;(!wComp) && pString1[wi] && pString2[wi] && (wi < pCountMax);wi++)
        {
//        w1=utfUpper<_Utf>(pString1[wi]);
//        w2=utfUpper<_Utf>(pString2[wi]);
        wComp = (int)(utfUpper<_Utf>(pString1[wi]) - utfUpper<_Utf>(pString2[wi]));
        }
    if (wComp)
            return wComp;
    if (wi==pCountMax)
            return wComp;
    // up to here wComp==0 (equality)
    //    test string lengths
    if (pString1[wi]==0) // string 1 exhausted
            {
            if (pString2[wi]==0)   // so is string2
                        return 0;   // same content and lengths are equal
            return -1; // string 2 is greater than string1 (because more characters)
            }
    // up to here wComp==0 but string1 is not exhausted
    //
    return 1; // string 1 is longer

}//utfStrncasecmp

#ifdef __COMMENT__
template <class _Utf>
/**
 * @brief utfStrlen computes current length in _Utf characters of _Utf string pString
 * @param pString
 * @return
 */
 size_t utfStrlen (const _Utf* pString)
{

    if (pString==nullptr)
                    {return 0;}
    size_t wLen=0;
    _Utf wC;
    while ((wC=pString[wLen])&&(wLen < __STRING_MAX_LENGTH__))
        {
        wLen++;
        }
    _ASSERT_(wLen >= __STRING_MAX_LENGTH__,"Fatal error: endofmark has not been found in string\n",1)
    return wLen;
}

#endif // __COMMENT__

/**

 */
template <class _Utf>
/**
 * @brief utfStrncpy
 *    The `utfStrncpy' function copies not more than `pCount' _Utf characters (characters
 *   that follow a null _Utf character are not copied) from the array pointed to
 *   by `pStr2' to the array pointed to by `pStr1'.
 *   source and destination may be objects that overlap.
 *   If the array pointed to by `pStr2' is a string that is shorter than `pCount'
 *   characters, _Utf null characters are appended to the copy in the array
 *   pointed to by `pStr1', until `pCount' characters have been written.
 *   The `utfStrncpy' function returns the value of `pStr1'.
 *  if pStr2 is nullptr nothing is done.
 *  if pStr1 is nullptr nothing is done, nullptr is returned.
 * @param pStr1
 * @param pStr2
 * @param pCount
 * @return
 */
 _Utf *
utfStrncpy(_Utf *pStr1, const _Utf *pStr2,size_t pCount)
{
    if (pStr1==nullptr)
                    return 0;
    if (pStr2==nullptr)
                    return pStr1;
    while (*pStr2 && pCount--)
                    *pStr1++=*pStr2++;
    while (pCount--)
            *pStr1++=(_Utf)'\0';
    return pStr1;
}

template <class _Utf>
 _Utf *
utfStrcat(_Utf *wDest, const _Utf *wSrc)
{
    utfStrcpy<_Utf> (wDest + utfStrlen (wDest), wSrc);
    return wDest;
}
/**
 * utfStrncat_T template appends at maximum pCount _Utf characters from wSrc to wDest,
 *  and pads to _Utf 0 wDest until pCount if character count of wSrc is less than pCount.
 */
template <class _Utf>
 _Utf *
utfStrncat(_Utf *wDest, const _Utf *wSrc, size_t pCount)
{

  _Utf * wD= wDest + utfStrlen (wSrc);
  size_t wS = utfStrlen (wSrc);
  wS=( wS > pCount )? pCount : wS;
  memmove (wD, wSrc, wS);
  wD[wS] = 0;
  if (pCount > wS)
          memset(wD+wS,0,pCount-wS);
  return wDest;
}// utfStrncat
/**
 utfStrchr() function locates the ﬁrst occurrence of wChar (_Utf character) in the string pointed to by wStr.
 The terminating null character is considered to be part of the string.
Return value
    a pointer to the located character, or a null pointer if the character does not occur in the string.
*/
template <class _Utf>
 _Utf *
utfStrchr(const _Utf *wStr,_Utf wChar)
{
  if (wStr==nullptr)
    return nullptr;

  while (*wStr != wChar)
    if (!*wStr++)
      return nullptr;
  return ( _Utf *)wStr;
}
/**
 *  utfStrrchr returns the last occurrence of _Utf character wChar within _Utf string wStr.
 *  returns nullptr if not found
 */
template <class _Utf>
 _Utf *
utfStrrchr(_Utf* wStr,_Utf wChar)
{
  if (wStr==nullptr)
    return nullptr;

  _Utf* wD=wStr+utfStrlen(wStr);
  while (*wD != wChar) {
    if (wD==wStr)
      return nullptr;
    wD--;
  }
  return wD;
}

 template <class _Utf>
 /**
  * @brief utfStrrchr reverse search for a single utf character withing an utf string.
  * @param wStr string to search
  * @param wChar character to reverse search for
  * @return  a pointer (_Utf*) to found character within string or nullptr if not found
  */
 const  _Utf *
 utfStrrchr(const _Utf* wStr,const _Utf wChar)
 {
   if (!wStr)
     return nullptr;

   const _Utf* wD=wStr+utfStrlen(wStr);
   while (*wD != wChar)
         {
         if (wD--==wStr)
                     return nullptr;
         }
     return wD;
 }

 /** @brief utfFindEnquoted() extracts substring from pInstring enquoted by pQuote1 at left and pQuote2 at right
 *  returns an allocated string with substring content duly terminated with '\0'.
 *  returned substring needs to be freed by callee.
 */
 template <class _Utf>
 _Utf* utfFindEnquoted(const _Utf* pInString,_Utf pQuote1,_Utf pQuote2)
 {
   if (!pInString)
     return nullptr;

   _Utf* wPtr = utfStrchr<_Utf>(pInString,pQuote1);
   wPtr++;
   if (wPtr==nullptr)
     return nullptr;
   _Utf* wPtr2 = utfStrchr<_Utf>(wPtr,pQuote2);
   if (wPtr2==nullptr)
     return nullptr;
   size_t wSize=(wPtr2-wPtr)+1;
   _Utf* wStr=(_Utf* )malloc(wSize*sizeof(_Utf));
   _Utf* wPtr3=wStr;

   while (*wPtr && (wPtr < wPtr2))
   {
     *wPtr3=*wPtr;
     wPtr++;
     wPtr3++;
   }
   *wPtr3=0;
   return wStr;
 }//findEnquoted


template <class _Utf>
/**
 * @brief utfFirstinSet
 *              returns a pointer to the first _Utf character found within pString that belongs to _Utf character set pSet.
 *              returns nullptr if not found
 * @param pString
 * @param pSet
 * @return
 */
 _Utf *
utfFirstinSet (_Utf*pString,const _Utf *pSet)
{
  if (!pString)
    return nullptr;
    _Utf*wStr = pString;
    const _Utf*wSet = pSet;

    for (wStr=pString;*wStr!=0;wStr++)
            {
                for(wSet = pSet;(*wSet!=0);wSet++)
                                if(*wSet==*wStr)
                                            return(wStr) ;
             }
    if (*wStr==0)
            return (nullptr);
    return(wStr);
}

template <class _Utf>
/**
 * @brief utfLastinSet
 *          finds the last occurrence of one of the _Utf character set given by pSet
 *   within pStr _Utf string in reverse (starting to utfStrlen() and ending at 0)
 *   Returns nullptr if not found.
 * @param pStr
 * @param pSet
 * @return
 */
 _Utf *
utfLastinSet(const _Utf *pStr, const _Utf *pSet)
{
  if (!pStr)
    return nullptr;
size_t wL = utfStrlen(pSet);
size_t wj = 0;
long wi;
    for (wi=utfStrlen(pStr);(wi >= 0)&&(pStr[wi]!=pSet[wj]);wi--)
                            {
                            for (wj=0;(wj<wL)&&(pStr[wi]!=pSet[wj]);wj++);
                            }
    return ((wi<0) ? nullptr: &pStr[wi]);     // return NULL pointer if not found pointer to pChar within string if found
}

template <class _Utf>
 _Utf *
utfLastNotinSet( _Utf *pStr, const _Utf *pSet)
{
  if (pStr==nullptr)
    return nullptr;
  if (!pSet)
      pSet=getDefaultDelimiter<_Utf>();
size_t wL = utfStrlen(pSet);
size_t wj = 0;
_Utf* wStr=pStr+utfStrlen(pStr);
long wi;
    for (;wStr!=pStr;wStr--)
        {
        for (wj=0;(wj < wL)&&(*wStr != pSet[wj]);wj++);
        if (wj==wL)
                return wStr;
        }
    return (nullptr);     // return NULL pointer if not found pointer to pChar within string if found
}


template <class _Utf>
 _Utf *
utfLastNotChar(const _Utf *pStr, const _Utf pChar)
{
  if (!pStr)
    return nullptr;

    _Utf *wPtr=(_Utf *)(pStr +utfStrlen<_Utf>(pStr));

    while ((wPtr>pStr)&&(*wPtr!=pChar))
                        {
                        wPtr --;
                        }
    return(wPtr);
}

template <class _Utf>
bool
utfIsTerminatedBy(_Utf *pStr,_Utf pChar)
{
  if (!pStr)
    return false;
  return (utfLastNotChar((_Utf*)pStr,(_Utf)0x20)[0]==pChar);
}


/**
 * @brief utfReplaceSet replaces all occurrence of given character set with pReplace character within pString
 * @param pOutString _Utf resulting string
 * @param pInString _Utf input string
 * @param pSet        set of _Utf characters to replace
 * @param pReplace    _Utf replacement character
 * @return
 */
template <class _Utf>
_Utf *
utfReplaceSet(_Utf*pString, const _Utf *pSet, _Utf pReplace)
{
  if (!pString)
    return nullptr;

    _Utf *wIn=pString;
  _Utf* wSet=pSet;
    while(*wIn)
      {
        for (wSet=pSet;*wSet!=0;wSet++)
                  if (*wSet==*wIn)
                      {
                      *wIn= pReplace;
                      break;
                      }
        wIn++;
      }
    return pString;
}// utfReplaceSet


template <class _Utf>
_Utf*
utfStrrev(_Utf* pStr)
{
    if (!pStr)
            return pStr;
    if (!*pStr)
            return pStr;
//    _Utf* wStr=utfStrdup<_Utf>(pStr);
    _Utf*wStrL =pStr,*wStrR=pStr+utfStrlen<_Utf>(pStr)-1 ;
    _Utf wChar;
    while (wStrL<wStrR)
        {
        wChar = *wStrL;
        if (wStrL<wStrR)
                {
                *wStrL=*wStrR;
                *wStrR=wChar;
                }
        wStrL++;
        wStrR--;

        }
    return pStr;
}

template <class _Utf>
/**
 * @brief utfItoa converts an int number to its _Utf string equivalent according pBase ( 10 by default)
 * @param pInt integer to convert
 * @param pString   destination _Utf string
 * @param pLen      available _Utf character units in pString
 * @param pBase     base of conversion
 * @return  0 if anything went OK, -1 if a problem raised
 */
int
utfItoa(int pInt, _Utf* pString, size_t pLen, int pBase=10)
{
  int wSum = pInt;
  int wi = 0;
  int wDigit;
  bool wNegative=false;
  if (!pString)
    return -1;
  if (pLen == 0)
    return -1;
  if (pInt<0)
  {
    pInt = pInt * -1;
    wNegative=true;
  }
  do
  {
    wDigit = wSum % pBase;
    if (wDigit < 0xA)
      pString[wi++] = ((_Utf)'0') + wDigit;
    else
      pString[wi++] = ((_Utf)'A') + wDigit - 0xA;
    wSum /= pBase;
  }while (wSum && (wi < (pLen - 1)));

  if (wNegative)
  {
    if (wi == (pLen - 2) && wSum)
      return -1;
    pString[wi++]=(_Utf)'-';
  }
  if (wi == (pLen - 1) && wSum)
    return -1;
  pString[wi] = (_Utf)'\0';
  utfStrrev<_Utf>(pString);
  return 0;
}// utfItoa
#include <string.h>
template <class _Utf>
/**
 * @brief utfFromCString  allocates a new _Utf string and loads pString in it. Returned string must be freed by calling routine.
 *                      Warning: char characters are moved to new _Utf format without any conversion, as it is.
 *
 *      For reciprocical conversion (_Utf to Char)  see utf type by utf type in zcharset.h file :
 *
 *      utf8StrToCStr       : tries to convert utf8 string to C string
        utf16StrToCStr      : tries to convert utf16 string to C string
        utf32StrToCStr      : tries to convert utf32 string to C string
 *
 * @param pCString  a const char* string
 * @return a new allocated _Utf string. Returns nullptr if pString is nullptr or pString is empty (first character is '\0')
 */
_Utf*
utfFromCString(const char* pCString)
{
    if (!pCString)
            return nullptr;
    if (!*pCString)
            return nullptr;
    _Utf* wUtf=(_Utf*)malloc(sizeof(_Utf)*(strlen(pCString)+1));
    size_t wi=0;
    for (;*pCString;pCString++,wi++)
                            wUtf[wi]=(_Utf)*pCString;
    wUtf[wi]=0;
    return wUtf;
}
template <class _Utf>
/**
 * @brief utfnFromChar moves char characters to _Utf characters until end of pString or maximum capacity reached.
 * @param pUtfString an _Utf string
 * @param pCapacity maximum number of _Utf characters available in pUtfString
 * @param pString a const char* string
 * @return pUtfString after pString has been moved
 */
_Utf*
utfnFromChar(_Utf*pUtfString,const size_t pCapacity, const char* pString)
{
    if (!pString)
            {
            pUtfString[0]=0;
            return pUtfString;
            }
    if (!*pString)
            {
            pUtfString[0]=0;
            return pUtfString;
            }
//    _Utf* wUtf=(_Utf*)malloc(sizeof(_Utf)*(::strlen(pString)+1));
    size_t wi=0;
    for (;*pString && (wi<pCapacity);pString++,wi++)
                            pUtfString[wi]=(_Utf)*pString;
    while (wi<pCapacity)
            pUtfString[wi++]=(_Utf)'\0';
    return pUtfString;
}//utfnFromChar


template<class _Utf>
/**
 * @brief utfDecode generic template for decoding an _Utf character to get its utf32 codepoint
 *  this routine is used by utfVSprintf.
 * @param pString
 * @param pUtf32CodePoint
 * @param pUtfCount
 * @param plittleEndian
 * @return
 */
UST_Status_type
utfDecode(_Utf* pString,utf32_t* pUtf32CodePoint, size_t* pUtfCount,ZBool *plittleEndian=nullptr )
{

    if (typeid(_Utf).hash_code()==typeid(char).hash_code())
        {
        *pUtf32CodePoint=(utf32_t)pString[0];
        pString++;
        return UST_SUCCESS;
        }

    switch (sizeof(_Utf))
    {
    case sizeof(utf8_t):
     return utf8Decode((const utf8_t*)pString,pUtf32CodePoint,pUtfCount)  ;
    case sizeof(utf16_t):
    {
//     return utf16NextCharValue((const utf16_t*)pString,pUtfSize, pUtf32CodePoint,plittleEndian)  ;
     return utf16Decode((const utf16_t*)pString, pUtf32CodePoint,pUtfCount,plittleEndian)  ;

    }
    case sizeof(utf32_t):
    {
     return utf32Decode((const utf32_t*)pString,pUtf32CodePoint,pUtfCount,plittleEndian)  ;
    }
    default:
        fprintf(stderr,"utfDecode-F-ILLEGAL invalid character size of utf data.\n");
        exit (EXIT_FAILURE);
    }
}//utfGetNextCharacter
/*
 *----------------------------------------------------------------------
 *
 * utfStrstr --
 *
 *	Locate the first instance of a substring in a string.
 *
 * Results:
 *	If string contains substring, the return value is the
 *	location of the first matching instance of substring
 *	in string.  If string doesn't contain substring, the
 *	return value is 0.  Matching is done on an exact
 *	character-for-character basis with no wildcards or special
 *	characters.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

template <class _Utf>
 _Utf *
utfStrstr(const _Utf *string,/* String to search. */
          const _Utf* substring) /* Substring to try to find in string. */
/*    register _Utf *string;
    _Utf *substring;		*/
{
    const _Utf *wString=string;
    const _Utf *a, *b;

    /* First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */

    b = substring;
    if (*b == 0) {
    return (_Utf*)string;
    }
    for ( ; *wString != 0; wString += 1) {
    if (*wString != *b) {
        continue;
    }
    a = wString;
    while (true) {
        if (*b == 0) {
        return (_Utf*)wString;
        }
        if (*a++ != *b++) {
        break;
        }
    }
    b = substring;
    }
    return nullptr;
}// utfStrstr

template <class _Utf>
bool
utfIsSeparator(_Utf pChar,const _Utf* pSep=nullptr)
{
    if (pSep==nullptr)
        pSep=cst_default_delimiter<_Utf>;
    for(int wi=0;pSep[wi]!=0;wi++) {
        if (pSep[wi]==pChar)
            return true;
    }
    return false;
}

template <class _Utf>
const _Utf*
utfGetToken(const _Utf *pString , const _Utf* pToken)
{
    if (pString==nullptr)
        return nullptr;
    if (pToken==nullptr)
        return nullptr;

    size_t wTokenSize=0;
    const _Utf* wTokenPtr=pToken;
    while (*wTokenPtr!=0) {
        if (utfIsSeparator<_Utf>(*wTokenPtr)) {
            fprintf(stderr,"utfGetToken-E-INVTOKEN Invalid token <%s> : contains unauthorized, invalid character.\n",pToken);
            return nullptr;
        }
        wTokenSize++;
        wTokenPtr++;
    } // while
    const _Utf* wPtr=pString;
    wTokenPtr = pToken;
    bool wFullToken = false;
    const _Utf* wStartPtr=nullptr;
    while (*wPtr!=0) {
        if (*wPtr == *wTokenPtr) {
            wStartPtr=wPtr;
            wPtr++;
            wTokenPtr++;
            while (*wPtr == *wTokenPtr) {
                wTokenPtr++;
                wPtr++;
                if (*wTokenPtr == 0) {
                    if ((*wPtr == 0) || utfIsSeparator(*wPtr)) {
                        return wStartPtr;
                    }
                }
            } // while (*wPtr == *wTokenPtr)
            wTokenPtr = pToken;
            continue;
        }// if (*wPtr == *wTokenPtr)
        wStartPtr=nullptr;
        wPtr++;
    } // while (*wPtr!=0)

    return nullptr;
}// utfGetToken

template <class _Utf>
bool
utfHasToken(const _Utf *pString , const _Utf* pToken)
{
    return utfGetToken<_Utf>(pString,pToken)!=nullptr;
}

template <class _Utf>
const _Utf*
utfGetTokenCase(const _Utf *pString , const _Utf* pToken)
{
    if (pString==nullptr)
        return nullptr;
    if (pToken==nullptr)
        return nullptr;

    size_t wTokenSize=0;
    const _Utf* wTokenPtr=pToken;
    while (*wTokenPtr!=0) {
        if (utfIsSeparator<_Utf>(*wTokenPtr)) {
            fprintf(stderr,"utfGetToken-E-INVTOKEN Invalid token <%s> : contains unauthorized, invalid character.\n",pToken);
            return nullptr;
        }
        wTokenSize++;
        wTokenPtr++;
    }
    const _Utf* wPtr=pString;
    wTokenPtr = pToken;
    bool wFullToken = false;
    const _Utf* wStartPtr=nullptr;
    while (*wPtr!=0) {
        if (utfUpper(*wPtr) == utfUpper(*wTokenPtr)) {
            wStartPtr=wPtr;
            wPtr++;
            wTokenPtr++;
            while (utfUpper(*wPtr) == utfUpper(*wTokenPtr)) {
                wTokenPtr++;
                wPtr++;
                if (*wTokenPtr == 0) {
                    if ((*wPtr == 0) || utfIsSeparator(*wPtr)) {
                        return wStartPtr;
                    }
                }
            } // while (*wPtr == *wTokenPtr)
            wTokenPtr = pToken;
            continue;
        }// if (*wPtr == *wTokenPtr)
        wStartPtr=nullptr;
        wPtr++;
    } // while (*wPtr!=0)

    return nullptr;
}// utfGetTokenCase

template <class _Utf>
bool
utfHasTokenCase(const _Utf *pString , const _Utf* pToken)
{
    return utfGetTokenCase<_Utf>(pString,pToken)!=nullptr;
}

/*
 *----------------------------------------------------------------------
 *
 * utfStrstr --
 *
 *	Locate the first instance of a substring in a string.
 *
 * Results:
 *	If string contains substring, the return value is the
 *	location of the first matching instance of substring
 *	in string.  If string doesn't contain substring, the
 *	return value is 0.  Matching is done on an exact
 *	character-for-character basis with no wildcards or special
 *	characters.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

template <class _Utf>
 _Utf *
utfStrrstr(const _Utf *pString,/* String to search. */
          const _Utf* pSubstring)/* Substring to try to find in string. */
/*    register _Utf *pString;
    _Utf *pSubstring;		*/
{
    ssize_t wStrCount = utfStrlen<_Utf>(pString);
    if (wStrCount<1)
            return nullptr;
    ssize_t wSubCount = utfStrlen<_Utf>(pSubstring);
    if (wSubCount<1)
            return nullptr;
    const _Utf *wString=pString+wStrCount;
    const _Utf *wSubStr=pSubstring+wSubCount;
    const _Utf *a, *b;

    /* First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */

    b = pSubstring+wSubCount;
/*    if (*b == 0) {
    return (_Utf*)pString;
    }*/
    for ( ; *wString != pString; wString --) {
    if (*wString != *b) {
        continue;
    }
    a = wString;
    while (1) {
        if (b == pSubstring)
                        return (_Utf*)wString;

        if (*a-- != *b--) {
        break;
        }
    }
    b = pSubstring+wSubCount;
    }
    return nullptr;
}// utfStrrstr


/**
 * @brief utfStrcasestr finds first occurrence CASE REGARDLESS of _Utf pSubstring within _Utf string pString.
 *  returns nullptr if not found or if one of these strings is empty.
 *
 * @param pString
 * @param substring
 * @return a pointer to
 */
 template <class _Utf>
 _Utf *
utfStrcasestr(const _Utf *pString,/* String to search. */
          const _Utf* pSubstring)/* Substring to try to find in string. */
{
    const _Utf *wString=pString,*wSavedString=pString;
    const _Utf *a, *b;


    /* First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */

    b = pSubstring;
    if (*b == 0) {
    return (_Utf*)pString;
    }
    for ( ; *wString != 0; wString += 1) {
    if (utfUpper<_Utf>(*wString) != utfUpper<_Utf>(*b)) {
        continue;
    }
    a = wString;
    while (1) {
        if (*b == 0) {
        return (_Utf*)wString;
        }
        if (utfUpper<_Utf>(*a++) != utfUpper<_Utf>(*b++))
                                                        break;

    }
    b = pSubstring;
    }
    return NULL;
}// utfStrstr

template <class _Utf>
/**
 * @brief utfStrrcasestr Reverse finds last occurrence CASE REGARDLESS of _Utf substring within _Utf string pString.
 * @param pString
 * @param pSubstring
 * @return an _Utf string pointer to the beginning of found substring. nullptr if nothing has been found.
 */
 _Utf *
utfStrrcasestr(const _Utf *pString,/* String to search. */
          const _Utf* pSubstring)/* Substring to try to find in string. */
{
    ssize_t wStrCount = utfStrlen<_Utf>(pString);
    if (wStrCount<1)
            return nullptr;
    ssize_t wSubCount = utfStrlen<_Utf>(pSubstring);
    if (wSubCount<1)
            return nullptr;
    const _Utf *wString=pString+wStrCount;
    const _Utf *wSubStr=pSubstring+wSubCount;
    const _Utf *a, *b;

    /* First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */

    b = pSubstring+wSubCount;
/*    if (*b == 0) {
    return (_Utf*)pString;
    }*/
    for ( ; *wString != pString; wString --) {
    if (utfUpper<_Utf>(*wString) != utfUpper<_Utf>(*b)) {
        continue;
    }
    a = wString;
    while (1) {
        if (b == pSubstring)
                    return (_Utf*)a;

        if (utfUpper<_Utf>(*a--) != utfUpper<_Utf>(*b--))
                                            break;
    }
    b = pSubstring+wSubCount;
    }
    return nullptr;
}// utfStrrcasestr
/**
 * @brief utfFindLastNonChar
 *          returns a pointer to the last char of a string that is not pChar.
 *          returns pContent pointer if the string is empty or if all char of the string are pChar.
 * @param pContent <_Utf> string to search for
 * @param pChar    <_Utf> Character to skip from end
 * @return
 */
template <class _Utf>
 const _Utf *
utfFindLastNonChar(const _Utf *pContent,_Utf pChar)
{
    _Utf *wPtr=(char *)(pContent +(utfStrlen(pContent)-1));
    _Utf* wContent=pContent;
    _Utf wChar=pChar;

    for (;(wPtr>wContent)&&(*wPtr!=wChar);wPtr--) ;
    return(wPtr);
}//utfFindLastNonChar

template <class _Utf>
/**
 * @brief utfFindLastNonSpace finds the last character of an _Utf string that is NOT space (0x20 or 0x0A)
 *      returns an _Utf pointer to the first character of pContent that is not space;
 * @param pContent string to search
 * @return
 */
 const _Utf *
utfFindLastNonSpace(const _Utf *pContent)
{
    _Utf *wPtr=(char *)(pContent +(utfStrlen(pContent)-1));
    _Utf* wContent=pContent;

    for (;(wPtr>wContent)&&((*wPtr==(_Utf)0x020)||(*wPtr==(_Utf)0x0A));wPtr--);
    return(wPtr);
}//utfFindLastNonSpace
 /**
  * @brief utfFindLastNonSet finds the last _Utf character of pString that does NOT belong to _Utf character set pSet
  * @param pString an _Utf character string terminated by 0
  * @param pSet a pointer to an array of _Utf character. The last character of the array MUST BE 0
  * @return
  */
template <class _Utf>
const _Utf *
utfFindLastNonSet(const _Utf *pString,_Utf *pSet)
{
    _Utf *wPtr=(char *)(pString +(utfStrlen(pString)-1));
     _Utf* wContent=pString;
    _Utf* wDelimiter=pSet;
    size_t wS= utfStrlen<_Utf>(pSet);
    uint8_t wF=true;
    int wi=0;
    while((wi < wS)&&(wF)&&(wPtr>wContent))
        {
        if (*wPtr==wDelimiter[wi])
                {
               wPtr --;
               wi=0;
                }
            else
                wi++;
        }// while
    return(wPtr);
}//findLastNonDelimiter

template <class _Tp>
bool utfIsTerminatedBy(const _Tp *pStr, _Tp pChar)
{
    return (utfFindLastNonSpace<_Tp>(pStr)[0]==pChar);
}

template<class _Utf>
void utfConditionalNL(_Utf *pString)
{
    size_t wLast = utfStrlen(pString);
    if (!(pString[wLast],(_Utf)'\n'))
                {
                pString[wLast]=(_Utf)'\n';
                wLast++;
                pString[wLast]=0;
                }
    return;
}//utfConditionalNL



template <class _Utf>
/**
 * @brief utfFindLastNotSpace
 *          finds the last _Utf character of string pString that is NOT a space character (0x20 or 0xA0).
 * @param pString
 * @return
 */
const _Utf *
utfFindLastNotSpace(const _Utf *pContent)
{
    _Utf *wPtr=(char *)(pContent +(utfStrlen(pContent)-1));
    _Utf* wContent=pContent;

    for (;(wPtr>=wContent)&&((*wPtr==(_Utf)0x020)||(*wPtr==(_Utf)0x0A));wPtr--);

    return(wPtr);
}//utfFindLastNotSpace

template <class _Utf>
/**
 * @brief utfFindLastNotChar
 *          finds the last _Utf character of string pString that is NOT a given character.
 * @param pString   utf string to scan
 * @param pChar     given utf character to skip
 * @return a pointer to the last utf character of pString that is NOT pChar
 */
const _Utf *
utfFindLastNotChar(const _Utf *pContent,_Utf pChar)
{
    _Utf *wPtr=(char *)(pContent +(utfStrlen(pContent)-1));
    _Utf* wContent=pContent;
    _Utf wChar=pChar;

    for (;(wPtr>=wContent)&&(*wPtr==wChar);wPtr--);

    return(wPtr);
}//utfFindLastNotChar

template <class _Utf>
/**
 * @brief utfFirstNotinSet
 *          finds the first _Utf character of string pString that is NOT a byte of pSet.
 *          usage example : find the first non space and non tab char of an _Utf string.
 * @param pString
 * @param pSet
 * @return
 */
_Utf*
utfFirstNotinSet( _Utf* pString,const _Utf* pSet)
{
    if (pString==nullptr)
                return nullptr;
    const _Utf*wStr = pString;
    if (!pSet)
        pSet=getDefaultDelimiter<_Utf>();
    const _Utf*wSet = pSet;

    for (;*wStr!=0;wStr++)
            {
                for(wSet = pSet;(*wSet!=0);wSet++)
                                if(*wSet==*wStr)
                                            break ;
                if (*wSet==0)
                            return((_Utf*)wStr);
             }
    if (*wStr==0)
            return (nullptr);
    return((_Utf*)wStr);
}//utfFirstNotinSet

template <class _Utf>
/**
 * @brief utfLTrim
 *          Suppresses trailing space characters (0x20 or 0xA0) in string pString.
 * @param pString
 * @return
 */
 _Utf*
utfLTrim(_Utf* pString)
{
    if (pString==nullptr)
                return nullptr;
     _Utf*wStr = pString;

    for (;(*wStr!=0)&&((*wStr==(_Utf)0x20)||(*wStr==(_Utf)0x0A));wStr++);
    return utfStrcpy<_Utf>(pString,wStr);
}//utfLTrim
template <class _Utf>
/**
 * @brief utfLTrimSet    Suppresses any leading _Utf characters defined in pSet found in string pString and returns pString.
 *  if pSet is nullptr (omitted) then default delimiter set defined for _Utf character is taken.
 * @see
 *
 * @param pString
 * @param pSet
 * @return
 */
 _Utf*
utfLTrimSet(_Utf* pString,const _Utf *pSet=nullptr)
{
    if (pString==nullptr)
                return nullptr;
     _Utf*wStr = pString;

    const _Utf* wSet=pSet;
    if (!wSet)
            wSet=getDefaultDelimiter<_Utf>();
    const _Utf* wPtr=utfFirstNotinSet<_Utf>(pString,wSet);
    if (wPtr==nullptr)
            return(pString);
    return utfStrcpy<_Utf>(pString,wStr);
}//utfLTrim
template <class _Utf>
const _Utf *
utfRTrim(_Utf *pString)
{
    _Utf *wPtr=(pString +(utfStrlen(pString)-1));
    _Utf* wContent=pString;

    for (;(wPtr>=wContent)&&(((*wPtr==0x020)||(*wPtr==0x0A))||(*wPtr==0x0D));wPtr--);
    wPtr++;
    *wPtr=0;
    return(pString);
}//utfRTrim

template <class _Utf>
/**
 * @brief utfLTrim
 *          suppresses within _Utf string pString the leading characters that belong to pSet _Utf set of characters
 * @param pString
 * @param pSet
 * @return
 */
 _Utf *
utfLTrim (_Utf*pString, const _Utf *pSet)
{
    const _Utf* wSet=pSet;
    if (!wSet)
            wSet=getDefaultDelimiter<_Utf>();

//    size_t wL =utfStrlen<_Utf>(pString);
    _Utf *wPtr=(_Utf*)utfFirstNotinSet<_Utf>(pString,wSet);
    if (wPtr==nullptr)
            return(pString);
    return utfStrcpy<_Utf>(pString,wPtr);
}// utfLTrim

template <class _Utf>
/**
 * @brief utfLTrim
 *          suppresses within _Utf string pString the leading characters that belong to pSet _Utf set of characters
 * @param pString
 * @param pSet
 * @return
 */
const _Utf *
utfLTrimPtr (_Utf*pString, const _Utf *pSet)
{
  const _Utf* wSet=pSet;
  if (!wSet)
    wSet=getDefaultDelimiter<_Utf>();

  //    size_t wL =utfStrlen<_Utf>(pString);
  _Utf *wPtr=(_Utf*)utfFirstNotinSet<_Utf>(pString,wSet);
  if (wPtr==nullptr)
    return(pString);
  return wPtr;
}// utfLTrim



template <class _Utf>
/**
 * @brief utfRTrim
 *          suppresses within _Utf string pString the trailing characters that belong to pSet
 * @param pString
 * @param pSet
 * @return
 */
 _Utf *
utfRTrim (_Utf*pString,const _Utf *pSet)
{
    const _Utf* wSet=pSet;
    if (!wSet)
            wSet=getDefaultDelimiter<_Utf>();
size_t wj;
ssize_t w1=utfStrlen<_Utf>(pString);
size_t wSetLen=utfStrlen<_Utf>(pSet);
 //   for (w1=strlen(content);(w1 >= 0)&&(content[w1]==' ');w1--); // empty for loop
    for (;(w1 >= 0);w1--)
            {
            for (wj=0;wSet[wj]!=0;wj++)
                                if (pString[w1]==wSet[wj])
                                            {
                                            pString[w1]=0;
                                            break;
                                            }
            if (wj==wSetLen)
                                break;

            }
    return (pString);

}//   utfRTrim


template <class _Utf>
/**
 * @brief utfTrim
 *          suppresses leading and trailing _Utf characters that belong to pSet.
 *       NB: _Utf characters of pString that belong to pSet and are not leading or trailing are not suppressed
 * @param pString
 * @param pSet
 * @return
 */
  _Utf *
utfTrim (_Utf* pString,const _Utf *pSet)
{
/*    if (pSet==nullptr)
                pSet={0x020 , 0};
*/
    const _Utf* wSet=pSet;
    if (!wSet)
        wSet=getDefaultDelimiter<_Utf>();
    utfLTrim(pString,wSet);
    return(utfRTrim(pString,wSet));
}

template<class _Utf>
/**
 * @brief utfAddConditionalTermination  adds a termination character ('\0') as the last character in pCount position if not already existing.
 * @param pString _Utf string to search for termination
 * @param pCount last _Utf character of the string (total size for a fixed string)
 */
void utfAddConditionalTermination(_Utf *pString,size_t pCount)
{
    if (pString[pCount]!=0)
            pString[pCount]= 0;
    return;
}//utfaddConditionalTermination




/**
 * @brief utfExpurgeSet
 *          Removes any occurrence of _Utf character or group of characters that belong to pSet _Utf character set from pString, regardless char occurrence order.
 *          returns pString expurged from characters of pSet.
 * @param pString
 * @param pSet set of _Utf characters to be removed from pString
 * @return  pString
 */
template <class _Utf=char>
_Utf *
utfExpurgeSet(const _Utf *pInString, const _Utf *pSet)
{
    bool wJump=false;
    const _Utf* wSet= pSet;
    const _Utf* wPtr= pInString;
    size_t wStrlen=0;
    while (*wPtr++)
            wStrlen++;
    wStrlen++;
    _Utf* wOutString=(_Utf*)calloc(wStrlen,sizeof(_Utf));
    _Utf* wOutPtr=wOutString;

    wPtr=pInString;
    while (*wPtr)
        {
        wSet= pSet;
        while (*wSet) // search if string char is in set
            {
            if (*wSet==*wPtr)
                        {
                        wJump=true;
                        break;
                        }
            wSet++;
            }
        if (!wJump)
                *wOutPtr++=*wPtr;

        wJump=false;
        wPtr++;
        }
    *wOutPtr=0 ;
    return wOutString;
}// utfExpurgeSet

/**
 * @brief utfExpurgeString
 *          suppresses all occurrences of _Utf substring pSubString from _Utf string pString,
 *          returns a string expurged from encountered pSubString sequenses from pString.
 *          returned string must be freed by callee.
 *
 * @param pString
 * @param pSubString
 * @return pString expurged from encountered substrings of pSubString.
 */
template <class _Utf>
_Utf *
utfExpurgeString(_Utf *pInString, const _Utf *pSubString)
{
    const _Utf* wPtr= pInString;
    size_t wStrlen=0;
    while (*wPtr++)
            wStrlen++;  // get strlen
    wStrlen++;  // count '\0' endofstring mark
    _Utf* wOutString=(_Utf*)calloc(wStrlen,sizeof(_Utf)); // allocate room as input string length
    memset (wOutString,0,wStrlen*sizeof(_Utf));

    wPtr=pInString; // reset wPtr to pInString
    _Utf *wPtr2;
    _Utf* wPtrOut=wOutString;

    size_t wSubStrCount=0;
    while (pSubString[wSubStrCount++]); // get strlen of pSubString into wSubStrCount- without '\0'

    while((wPtr2=(_Utf*)utfStrstr<_Utf>(wPtr,pSubString))!=nullptr)
        {
        while (wPtr<wPtr2)
                *wPtrOut++=*wPtr++;  // copy to out string until substring start

        wPtr+=wSubStrCount;  // then jump substring
        }
    *wPtrOut=0;  // add endofstring mark (not necessary because whole string set to 0)

    return wOutString;
}// utfExpurgeString


const char*
getUnitFormat(uint8_t pSize);

template <class _Utf>
const char* getStringType(void)
{
  _Utf wA;
  const char* wB;
  char* wC;
  if (typeid(wA).hash_code()==typeid(wB).hash_code())
    return "char";
  if (typeid (wA).hash_code()==typeid(wC).hash_code())
    return "char";
  switch (sizeof(_Utf))
  {
  case sizeof(utf8_t):
    return "UTF8";
  case sizeof(utf16_t):
    return "UTF16";
  case sizeof(utf32_t):
    return "UTF32";
  default:
    return "UNKNOWN";
  }

}



utf8VaryingString formatSize(long long wSize);

const char* getUnitFormat(uint8_t pSize);

#endif // UTFUTILS_H
