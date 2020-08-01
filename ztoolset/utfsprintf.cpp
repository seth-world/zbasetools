#ifndef UTFSPRINTF_CPP
#define UTFSPRINTF_CPP

#include <ztoolset/utfsprintf.h>
#include <ztoolset/zicuconverter.h>

#include <cmath> // std::isnan std::isfinite pow powl (builtin_powl for gnu)
#include <locale.h>  /* for GNU and windows contains localeconv() function data */
#include <stddef.h> /* ptrdiff_t */
#include <stdint.h>


void set_Float_F_Rounding_GT(ZBool pGT) {GFloat_F_Rnd_GT=pGT;}
void set_Float_E_Rounding_GT(ZBool pGT) {GFloat_E_Rnd_GT=pGT;}
void set_Float_A_Rounding_GT(ZBool pGT) {GFloat_A_Rnd_GT=pGT;}


int
getExponent(LONG_DOUBLE pLDValue,LONG_DOUBLE pLDBase)
{
int wExponent=0;
    if (pLDValue<0)           /* fabs */
            pLDValue=-pLDValue;
    if (pLDValue < 1.0)
        {
        while ((pLDValue!=0) &&  !((int)pLDValue)) {
            pLDValue = pLDValue * pLDBase;
            wExponent--;
            };
        }
    else
        {
        while ((int)pLDValue ){
            pLDValue = pLDValue / pLDBase;
            wExponent++;
            };
            wExponent--;
//            pLDValue = pLDValue * pLDBase ;

        }// else
    return wExponent;
}//getExponent

LONG_DOUBLE
abs_val(LONG_DOUBLE value)
{
    if (value < 0)
        return -value;

    return value;
}

/* Microsoft does not over pow with long double : powl is used instead */
inline
LONG_DOUBLE
/* in case powl is not defined */
powB(LONG_DOUBLE pBase,LONG_DOUBLE pExp)
{
    LONG_DOUBLE         result = 1.0;
    while (pExp > 0.0) {
        result *= pBase;
        pExp--;
    }
    return result;
}//powB

long
utfRound(LONG_DOUBLE value,double pBase)
{
    long            intpart;
    double  wFBase = pBase / 20.0 ;
    intpart = value;
    value = value - intpart;
    if (value >= wFBase)
        intpart++;

    return intpart;
}



#ifdef TEST_SNPRINTF
#ifndef LONG_STRING
#define LONG_STRING 1024
#endif
int
main(void)
{
    char            buf1[LONG_STRING];
    char            buf2[LONG_STRING];
    char           *fp_fmt[] = {
        "%-1.5f",
        "%1.5f",
        "%123.9f",
        "%10.5f",
        "% 10.5f",
        "%+22.9f",
        "%+4.9f",
        "%01.3f",
        "%4f",
        "%3.1f",
        "%3.2f",
        "%.0f",
        "%.1f",
        NULL
    };
    double          fp_nums[] =
        { -1.5, 134.21, 91340.2, 341.1234, 0203.9, 0.96, 0.996,
        0.9996, 1.996, 4.136, 0
    };
    char           *int_fmt[] = {
        "%-1.5d",
        "%1.5d",
        "%123.9d",
        "%5.5d",
        "%10.5d",
        "% 10.5d",
        "%+22.33d",
        "%01.3d",
        "%4d",
        NULL
    };
    long            int_nums[] = { -1, 134, 91340, 341, 0203, 0 };
    int             x, y;
    int             fail = 0;
    int             num = 0;

    printf("Testing snprintf format codes against system sprintf...\n");

    for (x = 0; fp_fmt[x] != NULL; x++)
        for (y = 0; fp_nums[y] != 0; y++) {
            snprintf(buf1, sizeof(buf1), fp_fmt[x], fp_nums[y]);
            sprintf(buf2, fp_fmt[x], fp_nums[y]);
            if (strcmp(buf1, buf2)) {
                printf
                    ("snprintf doesn't match Format: %s\n\tsnprintf = %s\n\tsprintf  = %s\n",
                     fp_fmt[x], buf1, buf2);
                fail++;
            }
            num++;
        }

    for (x = 0; int_fmt[x] != NULL; x++)
        for (y = 0; int_nums[y] != 0; y++) {
            snprintf(buf1, sizeof(buf1), int_fmt[x], int_nums[y]);
            sprintf(buf2, int_fmt[x], int_nums[y]);
            if (strcmp(buf1, buf2)) {
                printf
                    ("snprintf doesn't match Format: %s\n\tsnprintf = %s\n\tsprintf  = %s\n",
                     int_fmt[x], buf1, buf2);
                fail++;
            }
            num++;
        }
    printf("%d tests failed out of %d.\n", fail, num);
}
#endif                          /* SNPRINTF_TEST */

/**
 * @brief utfFormatUtfStringChunk
 * @param pOutOET
 * @param pBuffer
 * @param pCurrlen
 * @param pMaxlen
 * @param pInOET
 * @param pInString
 * @param pInUnit
 * @param pConverter
 * @return
 */
UST_Status_type
utfFormatStringChunk(OutEncoding_type pOutOET,
                        void* pBuffer,
                        size_t* pCurrlen,
                        size_t pMaxlen,
                        OutEncoding_type pInOET,
                        void* pInString,
                        size_t pInUnit,
                        zicuConverter** pConverter)
{
_MODULEINIT_

#ifdef __COMMENT__

    if (*pConverter==nullptr) // if global converter has not be initialized, initialize it
        {
        *pConverter=new zicuConverter;
        char* pOutCSName;
        switch (pOutOET)
        {
        case OET_Char:
            pOutCSName=getDefaultEncodingName();
            break;
        case OET_UTF8:
            pOutCSName="UTF8";
            break;
        case OET_UTF16:
            pOutCSName="UTF16BE";
            if (is_little_endian())
                    pOutCSName= "UTF16LE";
            break;
        case OET_UTF32:
            pOutCSName="UTF32BE";
            if (is_little_endian())
                    pOutCSName= "UTF32LE";
            break;
        default:
            break;
        }
        (*pConverter)->setup(pOutCSName,true);
//        atexit(&zcleanGConverter);
        }

zicuConverter* wConverter=*pConverter;

// check string size

    _Utf2* wStringValue=pStringValue;

    for (wStrln = 0; wStringValue[wStrln]&&(wStrln< __STRING_MAX_LENGTH__); ++wStrln);     /* strlen */
    if (wStrln==__STRING_MAX_LENGTH__)
        {
        fprintf (stderr,"%s> Fatal error: endofmark has not been found in string\n",
                 _GET_FUNCTION_NAME_);
        _ABORT_
        }
#endif //__COMMENT__


    utf32_t wCodePoint;
    size_t wInCount, wOutCount, wInLength=0, wOutLength=0;
    utf8_t* wUtf8InString=(utf8_t*)pInString, *wUtf8Buffer=(utf8_t*)pBuffer;
    utf16_t* wUtf16InString=(utf16_t*)pInString, *wUtf16Buffer=(utf16_t*)pBuffer;
    utf32_t* wUtf32InString=(utf32_t*)pInString, *wUtf32Buffer=(utf32_t*)pBuffer;
    utf8_t  wUtf8Char[5];
    utf16_t wUtf16Char[3];
    size_t wIdx=0;

    if (pInOET==pOutOET) // if character encodings are the same treat them as a copy
        {
        size_t wIdx=0;
        switch (pInOET)
        {
        case OET_Char:
            while (wIdx < pInUnit)
            {
                if (pBuffer)    /* only if pBuffer is not null : if buffer is null only count char units */
                        {
                        if (*pCurrlen >= pMaxlen)
                                        {_RETURN_ UST_TRUNCATED;}
                        wUtf8Buffer[*pCurrlen]=wUtf8InString[wIdx]; // char is supposed to have same format as utf8_t : but may be not same encoding (not Unicode)
                        }
                (*pCurrlen)++;
                wIdx++;
            }
            _RETURN_ UST_SUCCESS;
        case OET_UTF8:
            while (wIdx < pInUnit)
            {
                if (pBuffer) /* only if pBuffer is not null */
                        {
                        if (*pCurrlen >= pMaxlen)
                                        {_RETURN_ UST_TRUNCATED;}
                        wUtf8Buffer[*pCurrlen]=wUtf8InString[wIdx];
                        }
                (*pCurrlen)++;
                wIdx++;
            }
            _RETURN_ UST_SUCCESS;
        case OET_UTF16:
            while (wIdx < pInUnit)
            {
                if (pBuffer) /* only if pBuffer is not null */
                        {
                        if (*pCurrlen >= pMaxlen)
                                        {_RETURN_ UST_TRUNCATED;}
                        wUtf16Buffer[*pCurrlen]=wUtf16InString[wIdx];
                        }
                (*pCurrlen)++;
                wIdx++;
            }
            _RETURN_ UST_SUCCESS;
        case OET_UTF32:
            while (wIdx < pInUnit)
            {
                if (pBuffer)
                        {
                        if (*pCurrlen >= pMaxlen)
                                        {_RETURN_ UST_TRUNCATED;}
                        wUtf32Buffer[*pCurrlen]=wUtf32InString[wIdx];
                        }
                (*pCurrlen)++;
                wIdx++;
            }
            _RETURN_ UST_SUCCESS;
        default:
            break;
        }//switch (pInOET)
        }//  if (pInOET==pOutOET)


/*  if character encodings are not the same : convert by character unit */

    while ((wInLength< pInUnit)&&(*pCurrlen<pMaxlen))
    {
    switch (pInOET)
    {
    case OET_Char:   /* input type char is not treated in this routine except if the same as output type */
        _ASSERT_(true,"invalid source string encoding OET_Char",1);
        /*
        fprintf(stderr,"%s> fatal error : invalid source string encoding OET_Char\n",_GET_FUNCTION_NAME_);
        _ABORT_;*/
    case OET_UTF8:
        utf8Decode(wUtf8InString,&wCodePoint,&wInCount,&pInUnit);
        wUtf8InString +=   wInCount;
        wInLength += wInCount;
        break;
    case OET_UTF16:
        utf16Decode(wUtf16InString,&wCodePoint,&wInCount,nullptr);
        wUtf16InString +=   wInCount;
        wInLength += wInCount;
        break;
    case OET_UTF32:
        utf32Decode(wUtf32InString,&wCodePoint,&wInCount,nullptr);
        wUtf32InString ++;
        wInLength ++;
        break;
    default:
        break;
    }

    switch (pOutOET)
    {
    case OET_Char:   // char is not treated in this routine
        fprintf(stderr,"%s> fatal error : invalid destination string encoding OET_Char\n",_GET_FUNCTION_NAME_);
        _ABORT_;
    case OET_UTF8:
        utf8Encode(wUtf8Char,&wOutCount,wCodePoint,nullptr);

        if (!pBuffer)   /* when pBuffer is null just aggregate the char units and return successfully */
            {
            (*pCurrlen) += wOutCount;
            break ;
            }
        if ((((*pCurrlen) + wOutCount)) >= pMaxlen )
                                _RETURN_ UST_TRUNCATED;
        wIdx=0;
        while (wIdx < wOutCount)
            {
            wUtf8Buffer[*pCurrlen]= wUtf8Char[wIdx];
            wIdx++;
            (*pCurrlen)++;
            }
        break;
    case OET_UTF16:
        wUtf16Char[0]=0;
        wUtf16Char[1]=0;
        wUtf16Char[2]=0;
        utf16Encode((utf16_t*)wUtf16Char,&wOutCount,wCodePoint,nullptr);
        if (!pBuffer)   /* when pBuffer is null just aggregate the char units and return successfully */
            {
            *pCurrlen += wOutCount;
            break ;
            }
        if ((((*pCurrlen) + wOutCount)) >= pMaxlen )
                                _RETURN_ UST_TRUNCATED;
        wUtf16Buffer[*pCurrlen]=wUtf16Char[0];
        (*pCurrlen)++;
        if (wUtf16Char[1])
            {
            wUtf16Buffer[*pCurrlen]=wUtf16Char[1];
            (*pCurrlen)++;
            }

        break;
    case OET_UTF32:

        if (!pBuffer)   /* when pBuffer is null just aggregate the char units and return successfully */
            {
            (*pCurrlen) ++;
            break ;
            }
        if ((((*pCurrlen) + 1 )) >= pMaxlen )
                                _RETURN_ UST_TRUNCATED;

        utf32Encode(&wCodePoint,wCodePoint,nullptr);
        wUtf32Buffer[*pCurrlen]=wCodePoint;
        (*pCurrlen)++;
        break;
    default:
        break;
    } //switch (pOutOET)


    }//while ((wInLength< pInUnit)&&(*pCurrlen<pMaxlen))


    _RETURN_ UST_SUCCESS;
}// utfFormatUtfStringChunk




/**
 * @brief utfDoprintf internal routine : does the effective snprintf job
 *
 *
 * Memory allocation, character set conversion and format processing
 *
 * if buffer size overflow errno is set to ENOMEM.
 *
 * Main output
 *  _Utf class is
 * 1. char
 *  - encoded to SystemDefault AND SystemDefault is utf8 utf16 or utf32 : normal case see (2.)
 *  - encoded to SystemDefault AND SystemDefault encoding is not utf8 utf16 or utf32 : fallback to other encoding
 *  - other encoding
 * Unicode UTF16LE or UTF16BE is used (according system default endianness)
 * an internal buffer is allocated as utf6_t * with units count equal to pMaxlen.
 * All process is made towards utf16.
 *
 * When returning, a conversion is made from utf16_t towards output char* and output encoding.
 *
 * 2. pCharset is Unicode utf8, utf16 or utf32 :
 * Routine is used as it is with a character by character conversion when necessary.
 *
 * string litteral in format expression :
 *
 *  must be Unicode -and ONLY Unicode- utf8 (%s), utf16 (%S - %w) or utf32 (%W)
 *  litterals as arguments are converted to destination  (Unicode) and utfxxReplacement character is used in case of conversion issue (meaning no true unicode characters).
 *
 * @param pCharset
 * @param pBuffer
 * @param pMaxlen
 * @param pFormat
 * @param args
 * @return length in character units of pBuffer after operation
 */


#ifndef __COMPACT_CODE__
inline
#endif
size_t
utfVsnprintf(ZCharset_type pCharset,void *pBuffer, size_t pMaxlen, const utfFmt *pFormat, va_list args)
{
_MODULEINIT_
    errno=0;
    char*   wBufferChar=nullptr;    /* Buffer is char */
    utf8_t* wBufferUtf8=nullptr;    /* Buffer is utf8 */
    utf16_t*wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */

    //static const utfFmt* wFormat ; /* must keep offset to pFormat */
    size_t          wCurrlen;
    size_t          wLen;
    size_t wOutCount=0;

    if (!pFormat)
        {
        switch (pCharset)
        {
        case ZCHARSET_UTF8:
        {
        wBufferUtf8= static_cast<utf8_t*>(pBuffer);
        wCurrlen = utfStrlen<utf8_t>(cst_Null<utf8_t>);
        if (!pBuffer)
                return wCurrlen;
        wLen = ( wCurrlen < pMaxlen )?wCurrlen:pMaxlen;
        utfStrncpy<utf8_t>(wBufferUtf8,cst_Null<utf8_t>,wLen);
        _RETURN_ wCurrlen;
        }
        case ZCHARSET_UTF16:
        {
        wBufferUtf16= static_cast<utf16_t*>(pBuffer);
        wCurrlen = utfStrlen<utf16_t>(cst_Null<utf16_t>);
        if (!pBuffer)
                return wCurrlen;
        wLen = ( wCurrlen < pMaxlen )?wCurrlen:pMaxlen;
        utfStrncpy<utf16_t>(wBufferUtf16,cst_Null<utf16_t>,wLen);
        _RETURN_ wCurrlen;
        }
        case ZCHARSET_UTF32:
        {
        wBufferUtf32= static_cast<utf32_t*>(pBuffer);
        wCurrlen = utfStrlen<utf32_t>(cst_Null<utf32_t>);
        if (!pBuffer)
                return wCurrlen;
        wLen = ( wCurrlen < pMaxlen )?wCurrlen:pMaxlen;
        utfStrncpy<utf32_t>(wBufferUtf32,cst_Null<utf32_t>,wLen);
        _RETURN_ wCurrlen;
        }
        default:
        {
        wBufferChar= static_cast<char*>(pBuffer);
        wCurrlen = utfStrlen<utf32_t>(cst_Null<utf32_t>);
        if (!pBuffer)
                return wCurrlen;
        wLen = ( wCurrlen < pMaxlen )?wCurrlen:pMaxlen;
        utfStrncpy<char>(wBufferChar,cst_Null<char>,wLen);
        _RETURN_ wCurrlen;
        }
        }// switch
        }//  if (!pFormat)

    ZCharset_type wOutCharset;
    ZBool wOutIsSameFmt=false;

    OutEncoding_type wOutEncoding;

    zicuConverter* wConverter;

    const char* wCharsetName;
    switch (pCharset)
    {
    case ZCHARSET_SYSTEMDEFAULT:
        pCharset = getDefaultCharset();  /* lets go thru and exploit pCharset with its real value */
    case ZCHARSET_UTF8:
        wOutIsSameFmt=true;
        wOutEncoding=OET_UTF8;
/*        wConverter.setup("UTF8",true);    Converter will be set up once by underlying routines whenever required */
        break;
    case ZCHARSET_UTF16:
        wOutEncoding=OET_UTF16;
        wBufferUtf16=static_cast<utf16_t*>(pBuffer);
        wCharsetName="UTF16BE";
        if (is_little_endian())
                    wCharsetName="UTF16LE";
/*        wConverter.setup(wCharsetName,true);  Converter will be set up once by underlying routines whenever required */
        break;
    case ZCHARSET_UTF32:
        wOutEncoding=OET_UTF32;
        wCharsetName="UTF32BE";
        if (is_little_endian())
                    wCharsetName="UTF32LE";
/*        wConverter.setup(wCharsetName,true);   Converter will be set up once by underlying routines whenever required */
        break;
    default:
        /* if destination encoding is not Unicode then process as utf16 and convert utf16 to destination charset encoding */
        wOutEncoding=OET_UTF16;
        wOutCharset= pCharset;
        wBufferUtf16=(utf16_t*) calloc(pMaxlen,sizeof(utf16_t)); /* allocate enough utf16 character units */
        ZCharset_type wUtf16Charset= ZCHARSET_UTF16BE;
        if (is_little_endian())
                    wUtf16Charset= ZCHARSET_UTF16LE;

        wConverter=new zicuConverter;
        wConverter->setup(wCharsetName,true);

        wOutCount=utfSubDoprintfvoid(wOutEncoding,
                                     wBufferUtf16,
                                     pMaxlen,
                                     pFormat,
                                     args,
                                     &wConverter);
        /* then convert to target charset encoding */

        wBufferChar=static_cast<char*>(pBuffer);

        wConverter->Utf16toEncoding(decode_ZCharset(wOutCharset),wBufferChar,pMaxlen,
                                    wBufferUtf16,wOutCount );

        _free(wBufferUtf16);
        _RETURN_ wOutCount;
    }//switch (pCharset)

    _RETURN_ utfSubDoprintfvoid(wOutEncoding,pBuffer,
                              pMaxlen, pFormat, args,&wConverter);
}//utfVsnprintf

/**
 * @brief utfSingleChar puts a single character from argument list va_list to pBuffer.
 *
 *  single char : format syntax addressed there a single char
 *  %c or %cs  expects single unit utf8   as int        (as within standard)
 *  %cw or %cS expects single unit utf16  as int        (extension)
 *  %cW expects utf32 codepoint           as uint32_t   (extension)
 *
 *  Single character is converted to appropriate output buffer format whenever required.
 *
 * @param pOutOET   Output buffer data type
 * @param pBuffer   Output buffer as a void* pointer
 * @param pCurrlen  Current offset within pBuffer. This offset is been updated as soon as it is less than pMaxlen - 1 and pBuffer not null
 * @param pMaxlen   maximum length available within pBuffer if pBuffer is not null
 * @param pFormat   a pointer to utfFmt string containing current format character to be treated : must be __C_LOW__
 * @param args      argument list
 * @return ZBool    false if format is not 'c' true in all other cases.
 */
#ifndef __COMPACT_CODE__
inline
#endif
ZBool
utfSingleChar(OutEncoding_type pOutOET,void* pBuffer,size_t* pCurrlen, size_t pMaxlen,const utfFmt** pFormat,size_t pFieldMinSize,FMT_Flags_type pFMTFlags, va_list args)
{
    if ((*pFormat)[0]!=(utfFmt)__C_LOW__)
                            return (ZBool)false;

    (*pFormat)++;/*  Utf extension : get next character to see what kind of format : default is utf8 */

    utf8_t* wBufferUtf8 =  static_cast<utf8_t*>(pBuffer);;
    utf16_t* wBufferUtf16 = static_cast<utf16_t*>(pBuffer);;
    utf32_t* wBufferUtf32 =  static_cast<utf32_t*>(pBuffer);;

    size_t wOutCount=0;

/* before anything : padding if necessary */

    int wPadLen = pFieldMinSize -1; // -1 because single char
    if (wPadLen<0)
            {wPadLen=0;}

    if (pFMTFlags & FMTF_Left) /* left justify */
                wPadLen = - wPadLen;
/* Left padding */
    switch(pOutOET)
        {
        case OET_UTF16:  /* utf16 to utf16 */
            while (wPadLen>0)
                {
//                wBufferUtf16 = static_cast<utf16_t*>(pBuffer);
                utfSPFOutChar<utf16_t>(wBufferUtf16,pCurrlen,pMaxlen,(utf16_t) __SPACE__);
                wPadLen--;
                }
            break;
        case OET_UTF8: /* utf16 to utf8 */
        wBufferUtf8 = static_cast<utf8_t*>(pBuffer);
            while (wPadLen>0)
                {
                utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen,(utf8_t) __SPACE__);
                wPadLen--;
                }
            break;
        case OET_UTF32:/* utf16 to utf32 */
        wBufferUtf32 = static_cast<utf32_t*>(pBuffer);
            while (wPadLen>0)
                {
                utfSPFOutChar<utf32_t>(wBufferUtf32,pCurrlen,pMaxlen,(utf32_t) __SPACE__);
                wPadLen--;
                }
            break;
        default:    // OET_Char : ignore
            break;
        } // switch(pOutOET)

/* end left padding */

    while (true) // just an empty loop to jump to padding
    {
    if ((pFMTFlags & FMTF_Long)||((*pFormat)[0]==__S_UPPER__)||((*pFormat)[0]==__W_LOWER__))
        {
        utf16_t wUtf16Char=(utf16_t)va_arg(args,int);
        switch(pOutOET)
            {
            /* see multi-unit utf8 and utf16 (surrogate) char : to be treated :
                currently treated as single unit char */
            case OET_UTF16:  /* utf16 to utf16 */
//                wBufferUtf16 = static_cast<utf16_t*>(pBuffer);
                wBufferUtf16[*pCurrlen] = wUtf16Char;
                (*pCurrlen)++;
                break;
            case OET_UTF8: /* utf16 to utf8 */
//                wBufferUtf8 = static_cast<utf8_t*>(pBuffer);
                singleUtf16toUtf8(wBufferUtf8,&wOutCount,
                                  &wUtf16Char,1, // may be modified to point on an utf16 2 units character serie
                                  pCurrlen,pMaxlen);
                break;
            case OET_UTF32:/* utf16 to utf32 */
//                wBufferUtf32 = static_cast<utf32_t*>(pBuffer);
                singleUtf16toUtf32(wBufferUtf32,&wOutCount,
                                  &wUtf16Char,1, // may be modified to point on an utf16 2 units character serie
                                  pCurrlen,pMaxlen);
                break;
        default: // OET_Char : ignore
            break;
            } // switch(pOutOET)
        if (((*pFormat)[0]==__S_UPPER__)||((*pFormat)[0]==__W_LOWER__))
                                (*pFormat)++; /* two units have been treated from format if %cw and %cS  */

        break; //exit while true
        }//if (((*pFormat)[0]==__S_UPPER__)||((*pFormat)[0]==__W_LOWER__))

// %cW :  single utf32 char */
    if ((pFMTFlags & FMTF_LongLong)||((*pFormat)[0]==__W_UPPER__))
    {
    utf32_t wUtf32Char=va_arg(args,utf32_t);
    switch(pOutOET)
            {
            /* see multi-unit utf8 and utf16 (surrogate) char : to be treated :
                currently treated as single unit char */
            case OET_UTF32:  /* utf32 to utf32 */
//                wBufferUtf32 = static_cast<utf32_t*>(pBuffer);
                wBufferUtf32[*pCurrlen] = wUtf32Char;
                (*pCurrlen)++;
                break;
            case OET_UTF8: /* utf32 to utf8 */
//                wBufferUtf8 = static_cast<utf8_t*>(pBuffer);
                singleUtf32toUtf8(wBufferUtf8,&wOutCount,
                                  &wUtf32Char,1, // may be modified to point on an utf16 2 units character serie
                                  pCurrlen,pMaxlen);
                break;
            case OET_UTF16:/* utf32 to utf16 */
//                wBufferUtf16 = static_cast<utf16_t*>(pBuffer);
                singleUtf32toUtf16(wBufferUtf16,&wOutCount,
                                  wUtf32Char,1, // may be modified to point on an utf16 2 units character serie
                                  pCurrlen,pMaxlen);
                break;
            } // switch(pOutOET)
    if ((*pFormat)[0]==__W_UPPER__)
                    (*pFormat)++; /* two units have been treated from format if %cw and %cS  */

    break; //exit while true

    }// if ((*pFormat)[0]==__W_UPPER__)
/* here remains the following cases
__S_LOW__ or only __C_LOW__ with no additional qualifier : must be treated as utf8 unique character*/

    utf8_t wUtf8Char=(utf8_t)va_arg(args,int);
    switch(pOutOET)
        {
        /* see multi-unit utf8 and utf16 (surrogate) char : to be treated :
            currently treated as single unit char */
        case OET_Char:
//            wBufferUtf8 = static_cast<utf8_t*>(pBuffer);
            if (pBuffer){
                    wBufferUtf8[*pCurrlen] = wUtf8Char;
                    }
            (*pCurrlen)++;
            break;
        case OET_UTF8:  /* utf8 to utf8 */
//            wBufferUtf8 = static_cast<utf8_t*>(pBuffer);
            if (pBuffer){
                    wBufferUtf8[*pCurrlen] = wUtf8Char;
                    }
            (*pCurrlen)++;
            break;
        case OET_UTF16: /* utf8 to utf16 */
//            wBufferUtf16 = static_cast<utf16_t*>(pBuffer);
            singleUtf8toUtf16(wBufferUtf16,&wOutCount,
                              &wUtf8Char,1, // may be modified to point on an utf8 unit character serie
                              pCurrlen,pMaxlen);
            break;
        case OET_UTF32:
//            wBufferUtf32 = static_cast<utf32_t*>(pBuffer);
            singleUtf8toUtf32(wBufferUtf32,&wOutCount,
                              &wUtf8Char,1, // may be modified to point on an utf8 unit character serie
                              pCurrlen,pMaxlen);
            break;
        default:
            break;
        } // switch(wOutEncoding)

    if ((*pFormat)[0]==__S_LOW__)
                        (*pFormat)++;
    break;
    } //while (true)


/* Right padding */
    switch(pOutOET)
        {
        case OET_UTF16:  /* utf16 to utf16 */
            while (wPadLen<0)
                {
                utfSPFOutChar<utf16_t>(wBufferUtf16,pCurrlen,pMaxlen,(utf16_t) __SPACE__);
                wPadLen++;
                }
            break;
        case OET_UTF8: /* utf16 to utf8 */
            while (wPadLen<0)
                {
                utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen,(utf8_t) __SPACE__);
                wPadLen++;
                }
            break;
        case OET_UTF32:/* utf16 to utf32 */
            while (wPadLen<0)
                {
                utfSPFOutChar<utf32_t>(wBufferUtf32,pCurrlen,pMaxlen,(utf32_t) __SPACE__);
                wPadLen++;
                }
            break;
        default:    // OET_Char : ignore
            break;
        } // switch(pOutOET)

 /* end right padding */




    return (ZBool)true;
}//utfSingleChar

inline
void
signalNull(OutEncoding_type pOutOET,
           void*            pBuffer,
           size_t*          pCurrlen,
           size_t           pMaxlen)
{
char* wBufferChar;
utf8_t* wBufferUtf8=nullptr;    /* Buffer is utf8 */
utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */
    switch (pOutOET)
        {
        case OET_Char:
            wBufferChar=static_cast<char*>(pBuffer);
            utfError<char>(wBufferChar,pCurrlen,pMaxlen,cst_Null<char>);
            break;
        case OET_UTF8:
            wBufferUtf8=static_cast<utf8_t*>(pBuffer);
            utfError<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen,cst_Null<utf8_t>);
            break;
        case OET_UTF16:
            wBufferUtf16=static_cast<utf16_t*>(pBuffer);
            utfError<utf16_t>(wBufferUtf16,pCurrlen,pMaxlen,cst_Null<utf16_t>);
            break;
        case OET_UTF32:
            wBufferUtf32=static_cast<utf32_t*>(pBuffer);
            utfError<utf32_t>(wBufferUtf32,pCurrlen,pMaxlen,cst_Null<utf32_t>);
            break;
        default:
            break;
        }
    return;
}
#ifndef __COMPACT_CODE__
inline
#endif
void
padField(OutEncoding_type pOutOET,
         void* pBuffer,
         int pPadLen,
         size_t*          pCurrlen,     /* current buffer offset */
         size_t           pMaxlen,      /* maximum buffer offset */
         int*             pCurFldLen,   /* current field length */
         int              pFieldMax)    /* maximum field length allowed or set to int_max */
{
char* wBufferChar;
utf8_t* wBufferUtf8=nullptr;    /* Buffer is utf8 */
utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */


    if (pPadLen==0)
                return;
    if (pPadLen<0)
            pPadLen=-pPadLen;

    switch(pOutOET)
        {
        case OET_Char:
            wBufferChar=static_cast<char*>(pBuffer);
            while ((pPadLen>0)&&(*pCurFldLen < pFieldMax))
                {
                utfSPFOutChar<char>((char*)wBufferUtf8,pCurrlen,pMaxlen,(char) __SPACE__);
                pPadLen--;
                (*pCurFldLen)++;
                }
            break;
        case OET_UTF8:
            wBufferUtf8=static_cast<utf8_t*>(pBuffer);
            while ((pPadLen>0)&&(*pCurFldLen < pFieldMax))
                {
                utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen,(utf8_t) __SPACE__);
                pPadLen--;
                (*pCurFldLen)++;
                }
            break;
        case OET_UTF16:
            wBufferUtf16=static_cast<utf16_t*>(pBuffer);
            while ((pPadLen>0)&&(*pCurFldLen < pFieldMax))
                {
                utfSPFOutChar<utf16_t>(wBufferUtf16,pCurrlen,pMaxlen,(utf16_t) __SPACE__);
                pPadLen--;
                (*pCurFldLen)++;
                }
            break;

        case OET_UTF32:
            wBufferUtf32=static_cast<utf32_t*>(pBuffer);
            while ((pPadLen>0)&&(*pCurFldLen < pFieldMax))
                {
                utfSPFOutChar<utf32_t>(wBufferUtf32,pCurrlen,pMaxlen,(utf32_t) __SPACE__);
                pPadLen--;
                (*pCurFldLen)++;
                }
            break;
        default:
            break;
        } // switch(pOutOET)
    return;
}//padField
/**
 * @brief utfFormatUtf8Field
 *
 * In case of severe status (status value < UST_Severe) status is returned and
 * returned utf string content is set to decode_UST(status value).
 *
 * @param pOutOET
 * @param pBuffer
 * @param pCurrlen
 * @param pMaxlen
 * @param pInString
 * @param pInOET
 * @param pFMTFlags
 * @param pFieldMinSize
 * @param pFieldMaximum
 * @param pConverter
 * @return
 */
UST_Status_type
utfFormatUtf8Field(OutEncoding_type pOutOET,
                   void*            pBuffer,
                   size_t*          pCurrlen,
                   size_t           pMaxlen,
                   const utf8_t*    pInString,
                   OutEncoding_type pInOET,
                   FMT_Flags_type   pFMTFlags,
                   int              pFieldMinSize,
                   int              pFieldMaximum,
                   zicuConverter** pConverter)
{
_MODULEINIT_

    const utf8_t* wUtf8String=pInString; /* Argument value for an utf8_t field */

    utf8_t* wBufferUtf8=nullptr;    /* Buffer is utf8 */
    utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */

    utf32_t wCodePoint;
    size_t wUtf8Count;
    UST_Status_type wSt;

    int             wPadLen;      /* amount to pad , string length */
    size_t          wCharCount;   /* size of string in canonical character counts (and not character units) */
    size_t          wU=0;
    int             wFieldCount=0; /* used to check maximum slots for field if any flag */

    _ASSERT_(pInOET!=OET_UTF8,"Fatal error : invalid character set for input string OET_UTF8 expected while <%s> found\n",
             decode_OET(pInOET));

    if (pInString == 0)
        {
        signalNull(pOutOET,pBuffer,pCurrlen,pMaxlen); /* string content is set to <null> */
        _RETURN_ UST_NULLPTR;
        }//if (pInString == 0)

    if (!(pFMTFlags & FMTF_Precision))
                    pFieldMaximum = __INT_MAX__ ; /* no maximum limit requested : set to maximum length */

/* first step : get canonical character count */

//        wSt=utf8CharCount(wUtf8String,&wCharCount);
        wSt=utf8DiacriticalCharCount(wUtf8String,&wCharCount);/* diacritical exclusion */

        _ASSERT_(wSt < UST_SEVERE,"Error while parsing string encoding <%s>. Error code is <%s>\n",
                 decode_OET(pInOET),
                 decode_UST((wSt)));
/*  padding if necessary      */

        wPadLen = pFieldMinSize - wCharCount;
        if (wPadLen<0)
                wPadLen=0;

        if (pFMTFlags & FMTF_Left) /* left justify */
                    wPadLen = - wPadLen;

    /* Left padding */

        if (wPadLen>0)
            {
            padField(pOutOET,pBuffer,wPadLen,pCurrlen,pMaxlen,&wFieldCount,pFieldMaximum);
            wFieldCount=wPadLen;
            }// if (wPadLen>0)
    /* end left padding */

    /* because format specifier Min and Max size concerns number of canonical characters,
     * we do have to use utf8CharSize routine to process string canonical character per canonical character
     * until end ofstring OR maximum size*/
    if (pOutOET==OET_UTF8) // simple copy to output buffer but per canonical char
            {
            wBufferUtf8 = static_cast<utf8_t*>(pBuffer);

            while (*wUtf8String && (wFieldCount < pFieldMaximum))
                {
                wSt=utf8Decode(wUtf8String,&wCodePoint,&wUtf8Count);
                if (UST_Is_Error(wSt))// if error, put illegal char, skip whole character.
                    {
                    wU=0;
                    while(utf8Replacement[wU])  /* utf8Replacement array ends with '\0' */
                            utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen,utf8Replacement[wU++]);
                    wUtf8String+=wUtf8Count;
                    }
                else
                    {
                    wU=wUtf8Count;
                    while(wU--)
                        {
                        utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen,*(wUtf8String++));
                        }
                    }
                if (!isDiacritical(wCodePoint)) /* diacritical mark must be skipped */
                                    wFieldCount ++; /* only one character is written there */
                } // while

            } //if (pOutOET==OET_UTF8)
    else
    if (pOutOET==OET_UTF16)
        {
        wBufferUtf16 = static_cast<utf16_t*>(pBuffer);
        utf32_t wCodePoint;
        size_t wUtf8Count, wUtf16Count;
        utf16_t wUtf16Char[3];

        while ((*wUtf8String)&& (wFieldCount < pFieldMaximum))
        {
        wSt=utf8Decode(wUtf8String,&wCodePoint,&wUtf8Count,nullptr);
        _ASSERT_(wSt<UST_SEVERE,"utf8 string decoder encountered a fatal error status <%s>",decode_UST(wSt))
        if (UST_Is_Error(wSt))// if error, put illegal char, skip whole character.
            {
            wU=0;
            while(utf8Replacement[wU])  /* utf8Replacement array ends with '\0' */
                    utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen,utf8Replacement[wU++]);
            wUtf8String+=wUtf8Count;
            wFieldCount ++; /* only one character is written there */
            continue;
            }
        wSt=utf16Encode((utf16_t*)&wUtf16Char,&wUtf16Count,wCodePoint,nullptr);
        _ASSERT_(wSt<UST_SEVERE,"utf16 string encoder encountered a fatal error status <%s>",decode_UST(wSt))

        for (size_t wi=0;wi<wUtf16Count;wi++)
                {
                utfSPFOutChar<utf16_t>(wBufferUtf16,pCurrlen,pMaxlen, wUtf16Char[wi]);
                }
        wUtf8String += wUtf8Count;
        if (!isDiacritical(wCodePoint)) /* diacritical mark must be skipped */
                            wFieldCount ++; /* only one character is written there */
        }//while (*wUtf8value)

    }//if (pOutOET==OET_UTF16)
    else
    if (pOutOET==OET_UTF32)
        {
        wBufferUtf32 = static_cast<utf32_t*>(pBuffer);

        while ((*wUtf8String)&& (wFieldCount < pFieldMaximum))
        {
        wSt=utf8Decode(wUtf8String,&wCodePoint,&wUtf8Count,nullptr);
        _ASSERT_(wSt<UST_SEVERE,"utf8 string decoder encountered a fatal error status <%s>",decode_UST(wSt))
        if (UST_Is_Error(wSt))// if error, put illegal char, skip whole character.
            {
            wU=0;
            while(utf8Replacement[wU])  /* utf8Replacement array ends with '\0' */
                    utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen,utf8Replacement[wU++]);
            wUtf8String+=wUtf8Count;
            wFieldCount ++; /* only one character is written there */
            continue;
            }

        utfSPFOutChar<utf32_t>(wBufferUtf32,pCurrlen,pMaxlen, wCodePoint);

        wUtf8String += wUtf8Count;
        if (!isDiacritical(wCodePoint)) /* diacritical mark must be skipped */
                            wFieldCount ++; /* only one character is written there */
        }//while (*wUtf8value)

        }//if (pOutOET==OET_UTF32)

/* Right padding */
    if (wPadLen<0)
        {
        padField(pOutOET,pBuffer,wPadLen,pCurrlen,pMaxlen,&wFieldCount,pFieldMaximum);
        }// if (wPadLen<0)
/* end right padding */

   _RETURN_ UST_SUCCESS;
}//utfFormatUtf8Field

UST_Status_type
utfFormatUtf16Field(OutEncoding_type pOutOET,
                    void*            pBuffer,
                    size_t*          pCurrlen,
                    size_t           pMaxlen,
                    utf16_t*         pInString,
                    OutEncoding_type pInOET,
                    FMT_Flags_type   pFMTFlags,
                    int              pFieldMinSize,
                    int              pFieldMaximum,
                    zicuConverter** pConverter)
{
_MODULEINIT_

    utf16_t*         wUtf16String=pInString; /* Argument value for an utf16_t field */

    utf8_t* wBufferUtf8=nullptr;    /* Buffer is utf8 */
    utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */

    utf32_t wCodePoint;
    size_t wUtf16Count,wU=0;

//    utf8_t wUtf8Char[5];

    int             wPadLen;      /* amount to pad , string length */
    size_t          wCharCount;   /* size of string in canonical character counts (and not character units) */
    UST_Status_type wSt;

    int             wFieldCount=0; /* used to check maximum slots for field if any flag */

    _ASSERT_(pInOET!=OET_UTF16,"Fatal error : invalid character set for input string OET_UTF16 expected while <%s> found\n",
             decode_OET(pInOET));

    if (pInString == 0)
        {
        signalNull(pOutOET,pBuffer,pCurrlen,pMaxlen);
        _RETURN_ UST_NULLPTR;
        }//if (pInString == 0)

    if (!(pFMTFlags & FMTF_Precision))
                    pFieldMaximum = __INT_MAX__ ; /* no maximum limit requested : set to maximum length */

/* first step : get canonical character count */

//        wSt=utf16CharCount(wUtf16String,&wCharCount);
        wSt=utf16DiacriticalCharCount(wUtf16String,&wCharCount,nullptr);/* diacritical exclusion - expects default system endianness*/
        _ASSERT_(wSt<UST_SEVERE,"utf16 string parser encountered a fatal error status <%s>",decode_UST(wSt))

/*  padding if necessary      */

        wPadLen = pFieldMinSize - wCharCount;
        if (wPadLen<0)
                wPadLen=0;

        if (pFMTFlags & FMTF_Left) /* left justify */
                    wPadLen = - wPadLen;

    /* Left padding */
        if (wPadLen>0)
            {
            padField(pOutOET,pBuffer,wPadLen,pCurrlen,pMaxlen,&wFieldCount,pFieldMaximum);
            }// if (wPadLen>0)
    /* end left padding */

    if (pOutOET==OET_UTF16) // simple copy to output buffer
            {
            wBufferUtf16 = static_cast<utf16_t*>(pBuffer);
            while ((*wUtf16String)&& (wFieldCount < pFieldMaximum))
                {
                wSt=utf16Decode(wUtf16String,&wCodePoint,&wUtf16Count,nullptr); /* assuming default system endianess */
                _ASSERT_(wSt<UST_SEVERE,"utf16 string decoder encountered a fatal error status <%s>",decode_UST(wSt))
                if (UST_Is_Error(wSt))// if error, put illegal char, skip whole character.
                    {
                    wU=0;
                    while(utf16Replacement[wU])  /* utf16Replacement array ends with '\0' */
                            utfSPFOutChar<utf16_t>(wBufferUtf16,pCurrlen,pMaxlen,utf16Replacement[wU++]);
                    wUtf16String+=wUtf16Count;
                    continue;
                    }
                for (size_t wi=0;(wi<wUtf16Count)&&(*wUtf16String);wi++)
                        {
                        utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen, *(wUtf16String++));
                        }
                if (!isDiacritical(wCodePoint)) /* diacritical mark must be skipped */
                                    wFieldCount ++; /* only one character is written there */
                }// while

            } //if (pOutOET==OET_UTF16)
    else
    if (pOutOET==OET_UTF8)
        {
        wBufferUtf8 = static_cast<utf8_t*>(pBuffer);
        utf32_t wCodePoint;
        size_t wUtf8Count;
        utf8_t wUtf8Char[5];

        while ((*wUtf16String)&& (wFieldCount < pFieldMaximum))
        {
        wSt=utf16Decode(wUtf16String,&wCodePoint,&wUtf16Count,nullptr);
        _ASSERT_(wSt<UST_SEVERE,"utf16 string decoder encountered a fatal error status <%s>",decode_UST(wSt))
        if (UST_Is_Error(wSt))// if error, put illegal char, skip whole character.
            {
            wU=0;
            while(utf8Replacement[wU])  /* utf8Replacement array ends with '\0' */
                    utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen,utf8Replacement[wU++]);
            wUtf16String+=wUtf16Count;
            wFieldCount ++; /* only one character is written there */
            continue;
            }
        wSt=utf8Encode(wUtf8Char,&wUtf8Count,wCodePoint,nullptr);
        _ASSERT_(wSt<UST_SEVERE,"utf8 string encoder encountered a fatal error status <%s>",decode_UST(wSt))
        if (pBuffer) /* pBuffer is not null : take care of size */
            {
            if (wUtf8Count+(*pCurrlen)>= (pMaxlen-1) )
                                {_RETURN_ UST_TRUNCATED;}
            }
        for (size_t wi=0;wi<wUtf8Count;wi++)
                {
                utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen, wUtf8Char[wi]);
                }
        wUtf16String += wUtf16Count;
        if (!isDiacritical(wCodePoint)) /* diacritical mark must be skipped */
                            wFieldCount ++; /* only one character is written there */
        }//while (*wUtf16value)

    }//if (pOutOET==OET_UTF16)
    else
    if (pOutOET==OET_UTF32)
        {
        wBufferUtf32 = static_cast<utf32_t*>(pBuffer);

        while ((*wUtf16String)&& (wFieldCount < pFieldMaximum))
        {
        wSt=utf16Decode(wUtf16String,&wCodePoint,&wUtf16Count,nullptr);
        _ASSERT_(wSt<UST_SEVERE,"utf16 string decoder encountered a fatal error status <%s>",decode_UST(wSt))
        if (UST_Is_Error(wSt))// if error, put illegal char, skip whole character.
            {
            /* utf32Replacement is one unique codepoint */
            utfSPFOutChar<utf32_t>(wBufferUtf32,pCurrlen,pMaxlen,utf32Replacement);
            wUtf16String+=wUtf16Count;
            wFieldCount ++; /* only one character is written there */
            continue;
            }
        utfSPFOutChar<utf32_t>(wBufferUtf32,pCurrlen,pMaxlen, wCodePoint);

        wUtf16String += wUtf16Count;

        if (!isDiacritical(wCodePoint)) /* diacritical mark must be skipped */
                            wFieldCount ++; /* only one character is written there */
        }//while (*wUtf8value)

        }//if (pOutOET==OET_UTF32)

/* Right padding */
    if (wPadLen<0)
        {
        padField(pOutOET,pBuffer,wPadLen,pCurrlen,pMaxlen,&wFieldCount,pFieldMaximum);
        }// if (wPadLen<0)
 /* end right padding */

   _RETURN_ UST_SUCCESS;
}//utfFormatUtf16Field

#include <ztoolset/zdatabuffer.h>

UST_Status_type
utfFormatUtf32Field(OutEncoding_type pOutOET,
                    void*            pBuffer,
                    size_t*          pCurrlen,
                    size_t           pMaxlen,
                    utf32_t*         pInString,
                    OutEncoding_type pInOET,
                    FMT_Flags_type   pFMTFlags,
                    int              pFieldMinSize,
                    int              pFieldMaximum,
                    zicuConverter** pConverter)
{
_MODULEINIT_

    utf32_t* wUtf32String=pInString; /* Argument value for an utf16_t field */

    utf8_t*  wBufferUtf8=nullptr;    /* Buffer is utf8 */
    utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */

    utf32_t wCodePoint;
//    size_t wUtf16Count;

//    utf8_t wUtf8Char[5];

    int             wPadLen;      /* amount to pad , string length */
    size_t          wCharCount;   /* size of string in canonical character counts (and not character units) */
    size_t          wU;           /* work size */
    UST_Status_type wSt;

    int             wFieldCount=0; /* used to check maximum slots for field if any flag */

    _ASSERT_(pInOET!=OET_UTF32,"Fatal error : invalid character set for input string OET_UTF32 expected while <%s> found\n",
             decode_OET(pInOET));

    if (pInString == nullptr)
        {
        signalNull(pOutOET,pBuffer,pCurrlen,pMaxlen);
        _RETURN_ UST_NULLPTR;
        }//if (pInString == 0)

    if (!(pFMTFlags & FMTF_Precision))
                    pFieldMaximum = __INT_MAX__ ; /* no maximum limit requested : set to maximum length */

/* first step : get canonical character count */

    wCharCount=utfStrlen<utf32_t>(wUtf32String);

     wSt=utf32DiacriticalCharCount(wUtf32String,&wCharCount,nullptr);/* diacritical exclusion - expects default system endianness*/
    _ASSERT_(wSt<UST_SEVERE,"utf32 string parser encountered a fatal error status <%s>",decode_UST(wSt))

/*  padding if necessary      */

    wPadLen = pFieldMinSize - wCharCount;
    if (wPadLen<0)
                wPadLen=0;

    if (pFMTFlags & FMTF_Left) /* left justify */
                    wPadLen = - wPadLen;

    /* Left padding */
    if (wPadLen>0)
            {
            padField(pOutOET,pBuffer,wPadLen,pCurrlen,pMaxlen,&wFieldCount,pFieldMaximum);
            }// if (wPadLen>0)
    /* end left padding */

    if (pOutOET==OET_UTF32) // simple copy to output buffer
            {
            wBufferUtf32= static_cast<utf32_t*>(pBuffer);

            while (*wUtf32String && (wFieldCount < pFieldMaximum))
                {
                    utfSPFOutChar<utf32_t>(wBufferUtf32,pCurrlen,pMaxlen,*wUtf32String);
                    wUtf32String++;
                    if (!isDiacritical(*wUtf32String)) /* diacritical mark must be skipped */
                                        wFieldCount ++; /* only one character is written there */
                }

            } //if (pOutOET==OET_UTF32)
    else
    if (pOutOET==OET_UTF8)
        {
        wBufferUtf8 = static_cast<utf8_t*>(pBuffer);
        utf32_t wCodePoint;
        size_t wUtf8Count;
        utf8_t wUtf8Char[5];


        while ((*wUtf32String)&& (wFieldCount < pFieldMaximum))
        {
        wCodePoint = *wUtf32String;
        wSt=utf8Encode(wUtf8Char,&wUtf8Count,wCodePoint,nullptr);
        _ASSERT_(wSt<UST_SEVERE,"utf8 string encoder encountered a fatal error status <%s>",decode_UST(wSt))
        if (UST_Is_Error(wSt))// if error, put illegal char, skip whole character.
            {
            wU=0;
            while(utf8Replacement[wU])  /* utf8Replacement array ends with '\0' */
                    utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen,utf8Replacement[wU++]);
            wUtf32String ++;
            wFieldCount ++; /* only one character is written there */
            continue;
            }
        if (pBuffer) /* pBuffer is not null : take care of size */
            {
            if (wUtf8Count+(*pCurrlen)>= (pMaxlen-1) )
                                {_RETURN_ UST_TRUNCATED;}
            }
        for (size_t wi=0;wi<wUtf8Count;wi++)
                {
                utfSPFOutChar<utf8_t>(wBufferUtf8,pCurrlen,pMaxlen, wUtf8Char[wi]);
                }
        wUtf32String ++;
        if (!isDiacritical(wCodePoint)) /* diacritical mark must be skipped */
                            wFieldCount ++; /* only one character is written there */
        }//while (*wUtf8value)

    }//if (pOutOET==OET_UTF16)
    else
    if (pOutOET==OET_UTF16)
        {
        utf16_t wUtf16Char[3];
        size_t wUtf16Count;
        wBufferUtf16 = static_cast<utf16_t*>(pBuffer);

        while ((*wUtf32String)&& (wFieldCount < pFieldMaximum))
        {
        wCodePoint=*wUtf32String;
        wSt=utf16Encode((utf16_t*)&wUtf16Char,&wUtf16Count,wCodePoint,nullptr);
        _ASSERT_(wSt<UST_SEVERE,"utf16 string encoder encountered a fatal error status <%s>",decode_UST(wSt))
        if (UST_Is_Error(wSt))// if error, put illegal char, skip whole character.
            {
            wU=0;
            while(utf16Replacement[wU])  /* utf16Replacement array ends with '\0' */
                    utfSPFOutChar<utf16_t>(wBufferUtf16,pCurrlen,pMaxlen,utf8Replacement[wU++]);
            wUtf32String ++;
            wFieldCount ++; /* only one character is written there */
            continue;
                    }
        for (size_t wi=0;wi<wUtf16Count;wi++)
                {
                utfSPFOutChar<utf16_t>(wBufferUtf16,pCurrlen,pMaxlen, wUtf16Char[wi]);
                }
        wUtf32String ++;
        if (!isDiacritical(wCodePoint)) /* diacritical mark must be skipped */
                            wFieldCount ++; /* only one character is written there */
        }//while (*wUtf32value)


        }//if (pOutOET==OET_UTF16)

/* Right padding */
    if (wPadLen<0)
        {
        padField(pOutOET,pBuffer,wPadLen,pCurrlen,pMaxlen,&wFieldCount,pFieldMaximum);
        }// if (wPadLen<0)
 /* end right padding */

   _RETURN_ UST_SUCCESS;
}//utfFormatUtf32Field


void
CheckFormatSignal(const utfFmt*pCheckFormatStart,
                  const utfFmt*pCurrFormat,
                  int pFieldMinSize,
                  int pFieldPrecision,
                  int pCurrencySpace,
                  FMT_Flags_type pFmtFlags)
{
    utfFmt wCheckFormatSgt[50];
    memset (wCheckFormatSgt,0,sizeof(wCheckFormatSgt));
    utfStrncpy<utfFmt>(wCheckFormatSgt, pCheckFormatStart,pCurrFormat-pCheckFormatStart);
    fprintf(stdout,
            "%s-CheckFormat> format string <%s> \n"
            "\t flags <%s>\n"
            "\t minimum size <%d>",
            _GET_FUNCTION_NAME_,
            (char*)wCheckFormatSgt,
            decode_FMTFlag(pFmtFlags),
            pFieldMinSize);

    if (pFmtFlags & FMTF_Amount)
    fprintf(stdout,"precision <%d> currency <%s> spaces before <%d>\n",
            pFieldPrecision,
            pFmtFlags&FMTF_Currency?"<yes>":"<none>",
            pCurrencySpace);
    else if (pFmtFlags & FMTF_Float)
        fprintf(stdout,
                "minimum size <%d> precision <%d>\n",
                pFieldMinSize,
                pFieldPrecision);
    else
        fprintf(stdout,
                "minimum size <%d> maximum size <%d>\n",
                pFieldMinSize,
                pFieldPrecision);
    return;
}


/**
 * @brief utfSubDoprintfvoid
 *
 *  Formatting flags
 *

 https://en.wikipedia.org/wiki/Printf_format_string


 Rules:
 ------
  Sign flag:
    If both the ' ' and '+' flags appear, the ' ' flag will be ignored

  Padding (left and right) :
    Padding is done under the constraint of maximum field allowed if any mentionned.

  Zero padding :
    The result of zero padding flag with non-numeric conversion specifier is undefined.
    Solaris and HPUX 10 does zero padding in this case, Digital Unix and Linux does not.

    Retained strategy:
    Zero padding is disabled for non-numeric conversion i.e. characters and strings.

  Field width:
    Negative field width argument value means left justified (and only as argument - '*' option )

  Precision and Maximum field length allowed :
    For floatting point fields (double and long double), precision field is used as maximum number of digits at the right of decimal point.
    For other fields : maximum number of canonical characters allowed (including padding).
    Number of canonical characters are NOT number of character units : one utf8 canonical character may have up to 4 character units.

    No negative values are allowed here. If a negative value is encountered here, it is ignored.


 *
 *
 * @param pOutOET
 * @param pBuffer
 * @param pMaxlen
 * @param pFormat
 * @param args
 * @param pConverter
 * @return
 */

size_t
utfSubDoprintfvoid(OutEncoding_type pOutOET, /* Output buffer utf type : only Unicode is allowed there */
                   void *pBuffer,           /* Buffer to be filled in */
                   size_t pMaxlen,          /* maximum buffer offset in character units */
                   const utfFmt *pFormat,   /* an utfFmt string containing formatting instructions */
                   va_list args,
                   zicuConverter**  pConverter,
                   uspf_check       pCheckFormat)
{
_MODULEINIT_

    LONG_DOUBLE         wFDValue;       /* Argument value for a double or long double (if exists) */

    utf8_t*  wBufferUtf8=static_cast<utf8_t*>(pBuffer);   /* Buffer is utf8 */
    utf16_t *wBufferUtf16=static_cast<utf16_t*>(pBuffer);  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=static_cast<utf32_t*>(pBuffer);  /* Buffer is utf32 */

    //========================
    size_t          wCurrlen=0; /* current buffer offset in character units */
    //========================
    size_t wOutCount=0;
    size_t wCheckLen=0;
    wCurrlen  = 0;

    int wArgInt;

    const utfFmt *wCheckFormatStart;

    UST_Status_type wSt;

    FMT_Flags_type wFmtFlags=FMTF_Nothing;

    int wFieldMinSize=0;

    int wCurrencySpace=0;

    /* Precision field :
     *   - gives the precision for a double or a long double (number of digits at the right of dot point)
     *   - for other types : gives the maximum number of slots (position) for the field to be inserted into output buffer
     */
    int wFieldPrecision=0;

    const utfFmt* wFmtStart=nullptr;

    size_t wFmtStringLen=0;

//    utf32_t* wUtf32String=nullptr;

    int wFormatSpecifier=0;

    ZBool wParseLoop=true;
//============================================================
    while ( (*pFormat) != __ENDOFSTRING__)  // Main loop there
//============================================================
    {

    /* --------- Capture and transcode to output buffer encoding anything until __PERCENT__ character */
    wFmtFlags=FMTF_Nothing;

    wFmtStart= pFormat;
    wFmtStringLen=0;

    wFieldPrecision=0;
    wFieldMinSize=0;
    wCurrencySpace=0;

    wFDValue=0.0;
    wArgInt=0;

    while (( (*pFormat)  != __ENDOFSTRING__) && ((*pFormat)  != (utfFmt)__PERCENT__)) // capture until __PERCENT__ character
        {
        wFmtStringLen++;
        pFormat++;
        }

    /* transcode if anything to transcode
            segment of pFormat is to be considered as a string of type utfFmt to convert to output char encoding

            segment starts at wFmtStart with a length of wFmtStringLen
    */

    if (wFmtStringLen) /* if there is anything to convert...  */
        {
        wSt=utfFormatStringChunk(pOutOET,
                                 pBuffer,
                                 &wCurrlen,
                                 pMaxlen,
                                 OET_UTF8,
                                 (void *)wFmtStart,
                                 wFmtStringLen,
                                 pConverter);
        if (wSt < UST_SEVERE)
                    {_RETURN_ wSt;}
        if (pCheckFormat)
            {
            utfFmt wCheckFormatSgt[50];
            memset (wCheckFormatSgt,0,sizeof(wCheckFormatSgt));
            utfStrncpy<utfFmt>(wCheckFormatSgt, wFmtStart,wFmtStringLen);

            fprintf(stdout,
                    "%s-CheckFormat> format string segment <%s> \n",
                    _GET_FUNCTION_NAME_,
                    wCheckFormatSgt);
            }
        continue;
        }//if (wFmtStringLen)

    wCheckFormatStart=pFormat; /* used to report format string segment when using check format */
    pFormat++; /* skip percent sign */


    /*  Now we may address __PERCENT__ format option  */
    wFormatSpecifier++;

    if (*pFormat ==__PERCENT__) /* address immediately double percent sign : percent admits no formatting flags */
    {
        switch (pOutOET)
        {
        case OET_Char:
            utfSPFOutChar<char> ((char*)wBufferUtf8,&wCurrlen,pMaxlen,(char)__PERCENT__);
            break;
        case OET_UTF8:
            utfSPFOutChar<utf8_t> (wBufferUtf8,&wCurrlen,pMaxlen,(utf8_t)__PERCENT__);
            break;
        case OET_UTF16:
            utfSPFOutChar<utf16_t> (wBufferUtf16,&wCurrlen,pMaxlen,(utf16_t)__PERCENT__);
            break;
        case OET_UTF32:
            utfSPFOutChar<utf32_t> (wBufferUtf32,&wCurrlen,pMaxlen,(utf32_t)__PERCENT__);
            break;
        default:
            break;
         }// switch

        pFormat++;
        continue;
    }//if (wFmtChar==__PERCENT__)

    if (*pFormat == __N_LOW__) /* address immediately 'n' option : nothing in buffer- expects a pointer to an int */
    {
        int* wArgValue = va_arg(args,int*);
        wArgValue[0]= wCurrlen;
        pFormat++;
        continue;
    }//if (wFmtChar==__N_LOW__)


/*       %[flags][width][.precision][data length]specifier
*/
/* Flags < - + space # 0 >
 *
 * flags	description
-	Left-justify within the given field width; Right justification is the default (see width sub-specifier).
+	Forces to preceed the result with a plus or minus sign (+ or -) even for positive numbers. By default, only negative numbers are preceded with a - sign.
(space)	If no sign is going to be written, a blank space is inserted before the value.
#	Used with o, x or X specifiers the value is preceeded with 0, 0x or 0X respectively for values different than zero.
Used with a, A, e, E, f, F, g or G it forces the written output to contain a decimal point even if no more digits follow. By default, if no digits follow, no decimal point is written.
0	Left-pads the number with zeroes (0) instead of spaces when padding is specified (see width sub-specifier).
*/

/* parse flags */

    wParseLoop=true;

    while (wParseLoop && (*pFormat))
        {
        switch ((*pFormat))
        {
        case __ZERO__:
            wFmtFlags |= FMTF_ZeroPad ;
            pFormat++;
            break;
        case __MINUS__:
            wFmtFlags |= FMTF_Left ;
            pFormat++;
            break;
        case __PLUS__:
            wFmtFlags |= FMTF_Plus ;
            pFormat++;
            break;
        case __SPACE__:
            wFmtFlags |= FMTF_SignSpace ;
            pFormat++;
            break;
         case __HASH__:
            wFmtFlags |= FMTF_Hash ;
            pFormat++;
            break;
/* extensions */
        case __PRECEEDS__:
            wFmtFlags |= FMTF_SignPrec ;
            pFormat++;
            break;
        case __FOLLOWS__:
            wFmtFlags |= FMTF_SignFolw ;
            pFormat++;
            break;
        default:
            wParseLoop=false;
            break;
        }// switch ((*pFormat))
        }//while (wParseLoop && (*pFormat))

        /* If both the ' ' and '+' flags appear, the ' ' flag should be ignored */
        if (wFmtFlags & FMTF_Plus){
                   wFmtFlags = wFmtFlags & ~(FMTF_SignSpace);
                    }


/* Parse field width and precision indications possibilities

    *.<ddd>                 width is an argument, precision is hardcoded
    *.*                     width and precision are both arguments
    <dddd>.<precision>      with and precision are both hardcoded
    <dddd>.*                widh is hardcoded and precision is argument

*/
/*
 *  1- field width if <*> or numeric digits
 */

        if  ((*pFormat) == __ASTERISK__) // slots # is an argument
        {
        wArgInt= va_arg(args,int);
        if (wArgInt>=0)
                {
                wFieldMinSize=wArgInt;
                }
            else            /* negative field width means left justified */
                {
                 wFieldMinSize= - wArgInt;
                 wFmtFlags |= FMTF_Left;
                }

        pFormat++;
        wFmtFlags |= FMTF_Minimum;
        }//if  (wFmtChar == __ASTERISK__)

        else //  no __ASTERISK__ search for hardcoded minimum width
        {
        while (*pFormat && utfIsdigit<utfFmt>(*pFormat)) // search for slot # indications
            {
            wFieldMinSize = (10 * wFieldMinSize) + utfChar_to_int<utfFmt>(*pFormat);
            pFormat++;
            wFmtFlags |= FMTF_Minimum;
            }
        }//else

/*-----------------------------------------------------------
*      2-   if a __DOT__ is there
*           Numeric field precision for a double or long double
*           OR maximum slots to edit for others : warning : expressed in canonical character slots and NOT in character units.
*
*/

    if (*pFormat==__DOT__)
        {
        pFormat++;

        if  (*pFormat == __ASTERISK__) // slots number is an argument
            {
            wArgInt= va_arg(args,int);
            if (wArgInt>=0)            // negative means invalid : no Precision - skip it
                    {
                    wFieldPrecision=wArgInt;
                    wFmtFlags |= FMTF_Precision;
                    }
            pFormat++;
            }//if  (wFmtChar == __ASTERISK__)

        else //  no __ASTERISK__ search for hardcoded precision
            {
            while (*pFormat && utfIsdigit<utfFmt>(*pFormat)) // search for slot # indications
                {
                wFieldPrecision = (10 * wFieldPrecision) + utfChar_to_int<utfFmt>(*pFormat);
                pFormat++;
                wFmtFlags |= FMTF_Precision; /* I know, this is repetitive, but... sure it has been done */
                }
            }//else
        }//if (wFmtChar==__DOT__)

    wParseLoop=true;
    while (wParseLoop)
    {
        switch(*pFormat)
        {
        /* format flags extension to currencies */
                    /* %'$<K   or %$>K*/
                case  __GROUP_SIGN__:  /* request for thousands grouping separator */
                        wFmtFlags |= FMTF_Group ;
                        pFormat++;
                        break;
                case  __CURRENCY_UPPER__:   /* request for currency symbol */
                case  __CURRENCY_LOW__:     /* upper and low are synonyms : only field type decides (Amount low or upper) wether uppercase or lowercase */
                    wFmtFlags |= FMTF_Currency ;
                    pFormat++;
                    while ((*pFormat==(utfFmt)__SPACE__)
                           ||(*pFormat==__FOLLOWS__)
                           ||(*pFormat==__PRECEEDS__))
                    {
                    if (*pFormat==__FOLLOWS__)
                            {
                            wFmtFlags |= FMTF_CurFollows;  /* (default) explicit : currency sign follows by default */
                            pFormat++;
                            continue;
                            }
                        if (*pFormat==__PRECEEDS__)
                            {
                            wFmtFlags |= FMTF_CurPreceeds; /* currency sign preceeds value */
                            pFormat++;
                            }
                        if (*pFormat==__SPACE__)
                            {
                            wCurrencySpace++;
                            pFormat++;
                            }
                    }
                    break;
        default:
            wParseLoop=false;

        }
    }//while (wParseLoop)


/* parse length specifiers ( numeric data type) : <h>  short <l> long  <ll> long long <L> long double */

    switch (*pFormat)
    {
    case __H_LOW__:     /* <h> is short int - warning : it is promoted to int as argument (says standard) */
        wFmtFlags |= FMTF_ShortInt ;
        pFormat++;
        break;
    case __L_LOW__:
        if (pFormat[1]==__L_LOW__) /* <ll> means long long */
            {
            wFmtFlags |= FMTF_LongLong ;
            pFormat++;
            }
        else
            {
            wFmtFlags |= FMTF_Long ;
            }
        pFormat++;
        break;
    case __L_UPPER__:
        wFmtFlags |= FMTF_LongLong ;
        pFormat++;
        break;
    case __Q_LOW__:
        wFmtFlags |= FMTF_LongLong ;
        pFormat++;
        break;
    default:
        break;
    }//switch (wFmtChar)


/* at this stage, we will process type field's content :
 *
 * Remains the following possibilities

Single character :
<%>  already processed
<c>  single utf8_t character : No multi units character allowed here
Extensions:
<cS> or <cw> single utf16 character : No multi units character allowed here
<cW>  utf32 character

String :
<s> utf8 string         : common way to pass multi-units utf8_t character
<S> or <w> utf16 string
<W> utf32 string

Numeric values :
<d> <i> integer : size according size modifier (short, int, long, long long)
<u> <o> <x> <X>  : unsigned integers  : size according size modifier (short, int, long, long long) see utfFormatLong()
<f> <F> : double  lower or uppercase  : if L flag : long double - format is iiiii.dddd  see utfFormatFPE()
<e> <E> : double lower or uppercase with scientific notation : if L flag : long double - format is iiiiiiii see utfFormatFPE()
<g> <G> : shortest representation of both <f> <F> or <e> <E> (lower or upper according the case) see utfFormatFPG()

<a> <A> : double or long double hexadecimal representation see utfFormatFPA()

pointer : <p> pointer is expressed in hexadecimal as an unsigned long

====Extension=======
<k> <K> : amount see utfFormatFPK()

*/

/* single char : format syntax addressed there a single char
 *  %c or %cs  expects single unit utf8   as unsigned char
 *  %cw or %cS expects single unit utf16  as unsigned int
 *  %cW expects utf32                     as uint32_t
 *
 *  a single char has no formatting flags : %-8c is illegal
 */

    if (*pFormat == __C_LOW__)
        {
        utfSingleChar(pOutOET,pBuffer,&wCurrlen,pMaxlen,&pFormat,wFieldMinSize, wFmtFlags, args);
        continue;
        }//if (wFmtChar==__C_LOW__)

/*
String :
<s> utf8 string         : common way to pass multi-units utf8_t character
<S> or <w> utf16 string  (standard says wchar_t but its size may vary according platform)
<W> utf32 string
*/

/* Analyze and process string field argument value */

    switch (*pFormat)
        {
        case __S_UPPER__:
        case __W_LOWER__:
            wFmtFlags |= FMTF_Long + FMTF_String ;
            pFormat++;
            break;
        case __S_LOW__:
            wFmtFlags |= FMTF_String;
            pFormat++;
            break;
        case __W_UPPER__:
            wFmtFlags |= FMTF_LongLong + FMTF_String ;
            pFormat++;
            break;
        default:
            break;
        }

    if (wFmtFlags & FMTF_String)
    {
        if (pCheckFormat&uspf_FormatAlone)
                    {
                    CheckFormatSignal(wCheckFormatStart,
                                      pFormat,
                                      wFieldMinSize,
                                      wFieldPrecision,
                                      wCurrencySpace,
                                      wFmtFlags);
                    continue;
                    }
        /* either __W_UPPER__ or __S_LOWER__ with 'll' or 'L' or 'q' modifier : utf32 */
        if (wFmtFlags & FMTF_LongLong)
            {
            utf32_t* wUtf32String=va_arg(args,utf32_t*);
//            wUtf32String=(utf32_t*)va_arg(args,const char*);

            wCheckLen= wCurrlen;
            utfFormatUtf32Field(pOutOET,
                               pBuffer,
                               &wCurrlen,
                               pMaxlen,
                               wUtf32String,
                               OET_UTF32,
                               wFmtFlags,
                               wFieldMinSize,
                               wFieldPrecision, // precision is maximum number of slots
                               pConverter);
            if (pCheckFormat)
                {
                CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                fprintf(stdout,
                        "\t Field is utf32 - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                }
            continue;
            }
        /* either __S_UPPER__ or __W_LOWER__ or __S_LOWER__ with 'l' modifier : utf16 */
        if (wFmtFlags & FMTF_Long)
            {
            utf16_t* wUtf16String=va_arg(args,utf16_t*);
            wCheckLen= wCurrlen;
            utfFormatUtf16Field(pOutOET,
                               pBuffer,
                               &wCurrlen,
                               pMaxlen,
                               wUtf16String,
                               OET_UTF16,
                               wFmtFlags,
                               wFieldMinSize,
                               wFieldPrecision, // precision is maximum number of slots
                               pConverter);
            if (pCheckFormat)
                {
                CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                fprintf(stdout,
                        "\t Field is utf16 - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                }
            continue;
            }
        /* up to here remains __S_LOW__ only option without modifier : utf8 */
        const utf8_t* wUtf8String=va_arg(args,const utf8_t*);
        wCheckLen= wCurrlen;
        utfFormatUtf8Field(pOutOET,
                           pBuffer,
                           &wCurrlen,
                           pMaxlen,
                           wUtf8String,
                           OET_UTF8,
                           wFmtFlags,
                           wFieldMinSize,
                           wFieldPrecision, // precision is maximum number of slots
                           pConverter);
        if (pCheckFormat)
            {
            CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
            fprintf(stdout,
                    "\t Field is utf8 - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
            }
        continue;
    }//if (wFmtChar & FMTF_String)


/*  Here remains only numeric values
Numeric values :
<d> <i> integer : size according size modifier (short, int, long, long long)
<u> <o> <x> <X>  : unsigned integers  : size according size modifier (short, int, long, long long)
<f> <F> : double  lower or uppercase  : if L flag : long double - format is iiiii.dddd  other modifiers are ignored
<e> <E> : double lower or uppercase with scientific notation : if L flag : long double - format is iiiiiiiie-xxddddd
<g> <G> : shortest representation of both <f> <F> or <e> <E> (lower or upper according the case)
<a> <A> : double or long double hexadecimal representation

<p>     : void* pointer expressed as an hexadecimal address over a long int
*/
/*    Analyze numeric field format        */

    switch (*pFormat)   /* address numeric editing base */
    {
    case __P_UPPER__:
        wFmtFlags |= FMTF_Uppercase;
    case __P_LOW__:
        wFmtFlags |= FMTF_Pointer;
        wFmtFlags |= FMTF_Int;
        wFmtFlags |= FMTF_Hexa;
        pFormat++;
        break;
    case __T_LOW__:
        wFmtFlags |= FMTF_PTRDIF_T;
        wFmtFlags |= FMTF_Int;
        wFmtFlags |= FMTF_Hexa;
        pFormat++;
        break;
    case __J_LOW__:
        wFmtFlags |= FMTF_Intmax_t;
        pFormat++;
        break;
    case __Z_LOW__:
        wFmtFlags |= FMTF_SizeT;
        pFormat++;
        break;
    case __U_LOW__:
        wFmtFlags |= FMTF_Unsigned;
        wFmtFlags |= FMTF_Int;
        pFormat++;
        break;
    case __O_LOW__:
        wFmtFlags |= FMTF_Octal;
        wFmtFlags |= FMTF_Int;
        pFormat++;
        break;
    case __X_UPPER__:
        wFmtFlags |= FMTF_Uppercase;    /* let it go thru */
    case __X_LOW__ :
        wFmtFlags |= FMTF_Hexa;
        wFmtFlags |= FMTF_Int;
        pFormat++;
        break;
    case __I_LOW__:
    case __D_LOW__:
        wFmtFlags |= FMTF_Int;
        pFormat++;
        break;
    case __F_UPPER__:
        wFmtFlags |= FMTF_Uppercase;  /* let it go thru */
    case __F_LOW__:
        wFmtFlags |= FMTF_Float;
        pFormat++;
        break;
    case __E_UPPER__:
        wFmtFlags |= FMTF_Uppercase;    /* let it go thru */
    case __E_LOW__:
        wFmtFlags |= FMTF_Float;
        wFmtFlags |= FMTF_Exponent;
        pFormat++;
        break;
    case __A_UPPER__:
        wFmtFlags |= FMTF_Uppercase;    /* let it go thru */
    case __A_LOW__:
        wFmtFlags |= FMTF_Float;
        wFmtFlags |= FMTF_Hexa;
        pFormat++;
        break;
    case __G_UPPER__:
        wFmtFlags |= FMTF_Uppercase;    /* let it go thru */
    case __G_LOW__:
        wFmtFlags |= FMTF_Float;
        wFmtFlags |= FMTF_GFormat;
        pFormat++;
        break;
/*  add-on : amount */
    case __AMOUNT_UPPER__:
        wFmtFlags |= FMTF_Uppercase;    /* let it go thru */
    case __AMOUNT_LOW__:
//        wFmtFlags |= FMTF_Float;
        wFmtFlags |= FMTF_Amount;
        pFormat++;
        break;


    default:
        break;
    }// switch (wFmtChar)

/* Process numeric field argument value */
/* special formats */
    if (wFmtFlags & FMTF_Pointer)
        {
        if (pCheckFormat&uspf_FormatAlone)
                    {
                    CheckFormatSignal(wCheckFormatStart,
                                      pFormat,
                                      wFieldMinSize,
                                      wFieldPrecision,
                                      wCurrencySpace,
                                      wFmtFlags);
                    continue;
                    }
        wCheckLen= wCurrlen;
        void* wArg=va_arg(args,void*);
        utfMainFormatInteger<uintptr_t>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                                (uintptr_t) wArg,wFieldMinSize,wFieldPrecision,wFmtFlags);
        if (pCheckFormat)
            {
            CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
            fprintf(stdout,
                    "\t Field is <void*> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
            }
        continue;
        }//if (wFmtFlags & FMTF_Pointer)

    if (wFmtFlags & FMTF_PTRDIF_T)
        {
        if (pCheckFormat&uspf_FormatAlone)
                    {
                    CheckFormatSignal(wCheckFormatStart,
                                      pFormat,
                                      wFieldMinSize,
                                      wFieldPrecision,
                                      wCurrencySpace,
                                      wFmtFlags);
                    continue;
                    }
        wCheckLen= wCurrlen;
        ptrdiff_t wArg=va_arg(args,ptrdiff_t);
        utfMainFormatInteger<ptrdiff_t>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                                (ptrdiff_t)wArg,wFieldMinSize,wFieldPrecision,wFmtFlags);
        if (pCheckFormat)
            {
            CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
            fprintf(stdout,
                    "\t Field is <ptrdiff_t> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
            }
        continue;
        }//if (wFmtFlags & FMTF_PTRDIF_T)

    if (wFmtFlags & FMTF_Intmax_t)
        {
        if (pCheckFormat&uspf_FormatAlone)
                    {
                    CheckFormatSignal(wCheckFormatStart,
                                      pFormat,
                                      wFieldMinSize,
                                      wFieldPrecision,
                                      wCurrencySpace,
                                      wFmtFlags);
                    continue;
                    }
        wCheckLen= wCurrlen;
        intmax_t wArg=va_arg(args,intmax_t);
        utfMainFormatInteger<intmax_t>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                                wArg,wFieldMinSize,wFieldPrecision,wFmtFlags);
        if (pCheckFormat)
            {
            CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
            fprintf(stdout,
                    "\t Field is <intmax_t> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
            }
        continue;
        }//if (wFmtFlags & FMTF_Intmax_t)

    if (wFmtFlags & FMTF_SizeT)
        {
        if (pCheckFormat&uspf_FormatAlone)
                    {
                    CheckFormatSignal(wCheckFormatStart,
                                      pFormat,
                                      wFieldMinSize,
                                      wFieldPrecision,
                                      wCurrencySpace,
                                      wFmtFlags);
                    continue;
                    }
        wCheckLen= wCurrlen;
        size_t wArg=va_arg(args,size_t);
        utfMainFormatInteger<size_t>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                                wArg,wFieldMinSize,wFieldPrecision,wFmtFlags);
        if (pCheckFormat)
            {
            CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
            fprintf(stdout,
                    "\t Field is <size_t> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
            }
        continue;
        }//if (wFmtFlags & FMTF_Intmax_t)



/* Integer suite */
    if (wFmtFlags & FMTF_Int)
        {
        if (pCheckFormat&uspf_FormatAlone)
                    {
                    CheckFormatSignal(wCheckFormatStart,
                                      pFormat,
                                      wFieldMinSize,
                                      wFieldPrecision,
                                      wCurrencySpace,
                                      wFmtFlags);
                    continue;
                    }
        wCheckLen= wCurrlen;
        if (wFmtFlags & FMTF_LongLong)
            {
            if (wFmtFlags & FMTF_Unsigned)
                {
                ULONG_LONG wArg=va_arg(args,ULONG_LONG);
                utfMainFormatInteger<ULONG_LONG>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                                 wArg,wFieldMinSize,wFieldPrecision,wFmtFlags);
                if (pCheckFormat)
                    {
                    CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                    fprintf(stdout,
                            "\t Field is <unsigned long long int> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                    }
                continue;
                }
            LONG_LONG wArg = va_arg(args,LONG_LONG);
#ifdef __F_NUM_GENERIC__
            utfMainFormatGeneric<LONG_LONG>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                             wArg,wFieldMinSize,wFieldPrecision,wFmtFlags,(const char*)wCheckFormatStart,(size_t)(pFormat-wCheckFormatStart));
#else
            utfMainFormatInteger<LONG_LONG>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                             wArg,wFieldMinSize,wFieldPrecision,wFmtFlags);
#endif // __F_NUM_GENERIC__
            if (pCheckFormat)
                {
                CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                fprintf(stdout,
                        "\t Field is <long long int> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                }
            continue;
            }//if (wFmtFlags & FMTF_LongLong)
        if (wFmtFlags & FMTF_Long)
            {
            if (wFmtFlags & FMTF_Unsigned)
                {
                unsigned long int wArg=va_arg(args,unsigned long int);
#ifdef __F_NUM_GENERIC__
            utfMainFormatGeneric<unsigned long int>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                             wArg,wFieldMinSize,wFieldPrecision,wFmtFlags,
                                                    (const char*)wCheckFormatStart,(size_t)(pFormat-wCheckFormatStart));
#else
                utfMainFormatInteger<unsigned long int>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                                        wArg,wFieldMinSize,wFieldPrecision,wFmtFlags);
#endif
                if (pCheckFormat)
                    {
                    CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                    fprintf(stdout,
                            "\t Field is <unsigned long int> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                    }
                continue;
                }
            long int wArg = va_arg(args,long int);
#ifdef __F_NUM_GENERIC__
            utfMainFormatGeneric<long int>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                             wArg,wFieldMinSize,wFieldPrecision,wFmtFlags,
                                           (const char*)wCheckFormatStart,(size_t)(pFormat-wCheckFormatStart));
#else
            utfMainFormatInteger<long int>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                           wArg,wFieldMinSize,wFieldPrecision,wFmtFlags);
#endif
            if (pCheckFormat)
                {
                CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                fprintf(stdout,
                        "\t Field is <long int> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                }

            continue;
            }//if (wFmtFlags & FMTF_Long)

        /* NB: short int is promoted to int or unsigned int (says the standard rules) */
        if (wFmtFlags & FMTF_Unsigned)
            {
            unsigned int wArg=va_arg(args,unsigned int);
#ifdef __F_NUM_GENERIC__
            utfMainFormatGeneric<unsigned int>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                             wArg,wFieldMinSize,wFieldPrecision,wFmtFlags,
                                               (const char*)wCheckFormatStart,(size_t)(pFormat-wCheckFormatStart));
#else
            utfMainFormatInteger<unsigned int>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                               wArg,wFieldMinSize,wFieldPrecision,wFmtFlags);
#endif
            if (pCheckFormat)
                {
                CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                fprintf(stdout,
                        "\t Field is <unsigned int> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                }
            continue;
            }

        int wArg = va_arg(args,int);
#ifdef __F_NUM_GENERIC__
            utfMainFormatGeneric<int>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                             wArg,wFieldMinSize,wFieldPrecision,wFmtFlags,
                                      (const char*)wCheckFormatStart,(size_t)(pFormat-wCheckFormatStart));
#else
        utfMainFormatInteger<int>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                  wArg,wFieldMinSize,wFieldPrecision,wFmtFlags);
#endif
        if (pCheckFormat)
            {
            CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
            fprintf(stdout,
                    "\t Field is <int> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
            }
        continue;
        }//if (wFmtFlags & FMTF_Int)


/* floatting point suite : double and long double if exists */

    if (wFmtFlags & FMTF_Amount)
        {
        if (pCheckFormat&uspf_FormatAlone)
                    {
                    CheckFormatSignal(wCheckFormatStart,
                                      pFormat,
                                      wFieldMinSize,
                                      wFieldPrecision,
                                      wCurrencySpace,
                                      wFmtFlags);
                    continue;
                    }
        wCheckLen=wCurrlen;
        if (wFmtFlags & FMTF_LongLong)
                {
                wFDValue = va_arg(args,LONG_DOUBLE);

                utfMainFormatFPK<LONG_DOUBLE>(pOutOET,pBuffer,&wCurrlen,pMaxlen,wFDValue,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                if (pCheckFormat)
                    {
                    CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                    fprintf(stdout,
                            "\t Field is <long Amount(double)> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                    }
                }
                else
                {
                wFDValue = va_arg(args,double);
                utfMainFormatFPK<LONG_DOUBLE>(pOutOET,pBuffer,&wCurrlen,pMaxlen,wFDValue,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                if (pCheckFormat)
                    {
                    CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                    fprintf(stdout,
                            "\t Field is <Amount> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                    }
                }
        continue;
        }
    else if (wFmtFlags & FMTF_Float)
        {
        if (pCheckFormat&uspf_FormatAlone)
                    {
                    CheckFormatSignal(wCheckFormatStart,
                                      pFormat,
                                      wFieldMinSize,
                                      wFieldPrecision,
                                      wCurrencySpace,
                                      wFmtFlags);
                    continue;
                    }
        wCheckLen=wCurrlen;
        if (wFmtFlags & FMTF_LongLong)
                {
                wFDValue = va_arg(args,LONG_DOUBLE);
#ifdef __F_NUM_GENERIC__
                utfMainFormatGeneric<LONG_DOUBLE>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                             wFDValue,wFieldMinSize,wFieldPrecision,wFmtFlags,
                                                  (const char*)wCheckFormatStart,(size_t)(pFormat-wCheckFormatStart));
#else
                if (wFmtFlags & FMTF_GFormat)
                    utfMainFormatFPG<LONG_DOUBLE>(pOutOET,pBuffer,&wCurrlen,pMaxlen,wFDValue,wFieldMinSize,wFieldPrecision,wFmtFlags);
                else if (wFmtFlags & FMTF_Exponent)
                    utfMainFormatFPE<LONG_DOUBLE>(pOutOET,pBuffer,&wCurrlen,pMaxlen,wFDValue,wFieldMinSize,wFieldPrecision,wFmtFlags);
                    else if (wFmtFlags & FMTF_Hexa)
                         utfMainFormatFPA<LONG_DOUBLE>(pOutOET,pBuffer,&wCurrlen,pMaxlen,wFDValue,wFieldMinSize,wFieldPrecision,wFmtFlags);
                         else
                    utfMainFormatFP<LONG_DOUBLE>(pOutOET,pBuffer,&wCurrlen,pMaxlen,wFDValue,wFieldMinSize,wFieldPrecision,wFmtFlags);
#endif
                if (pCheckFormat)
                    {
                    CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                    fprintf(stdout,
                            "\t Field is <long double> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                    }
                }
            else
                {
                wCheckLen=wCurrlen;
                wFDValue = va_arg(args,double);
#ifdef __F_NUM_GENERIC__
                utfMainFormatGeneric<double>(pOutOET,pBuffer,&wCurrlen,pMaxlen,
                                             wFDValue,wFieldMinSize,wFieldPrecision,wFmtFlags,
                                             (const char*)wCheckFormatStart,(size_t)(pFormat-wCheckFormatStart));
#else
                if (wFmtFlags & FMTF_GFormat)
                    utfMainFormatFPG<double>(pOutOET,pBuffer,&wCurrlen,pMaxlen,wFDValue,wFieldMinSize,wFieldPrecision,wFmtFlags);
                else if (wFmtFlags & FMTF_Exponent)
                    utfMainFormatFPE<double>(pOutOET,pBuffer,&wCurrlen,pMaxlen,wFDValue,wFieldMinSize,wFieldPrecision,wFmtFlags);
                    else if (wFmtFlags & FMTF_Hexa)
                        utfMainFormatFPA<double>(pOutOET,pBuffer,&wCurrlen,pMaxlen,wFDValue,wFieldMinSize,wFieldPrecision,wFmtFlags);
                        else
                    utfMainFormatFP<double>(pOutOET,pBuffer,&wCurrlen,pMaxlen,wFDValue,wFieldMinSize,wFieldPrecision,wFmtFlags);
#endif
                if (pCheckFormat)
                    {
                    CheckFormatSignal(wCheckFormatStart,pFormat,wFieldMinSize,wFieldPrecision,wCurrencySpace,wFmtFlags);
                    fprintf(stdout,
                            "\t Field is <double> - output size is <%ld>\n", (size_t)wCurrlen-wCheckLen);
                    }
                }
        continue;
        }// (wFmtFlags & FMTF_Float)

/* an invalid character has been encountered here : report it to stderr */
char wFormatBuf[50];
const utfFmt* wFmtPtr= wCheckFormatStart;
size_t wIF=0,wFM=pFormat+4-wCheckFormatStart;
    while (*wFmtPtr && wIF<wFM)
            wFormatBuf[wIF++]=*(wFmtPtr++);
    wFormatBuf[wIF]='\0';

    fprintf (stderr,
             "%s>>  --------while processing format specifier rank <%d> <%s>, character \/%c\/ has not been understood------- \n"
             " Warning: format specifier is skipped. Check your vars. A mismatch vars/format specifier will occur.\n",
             _GET_FUNCTION_NAME_,
             wFormatSpecifier,
             wFormatBuf,
             (char)*pFormat);
//==================
    } // Main loop
//==================

    if (!pBuffer)   /* if out string is null return length */
            { _RETURN_ wCurrlen;}

    if (wCurrlen >= (pMaxlen - 1))
                wCurrlen= pMaxlen - 1 ;


    switch (pOutOET)
    {
    case OET_Char:
        wBufferUtf8[wCurrlen]=(char)__ENDOFSTRING__;
        break;
    case OET_UTF8:
        wBufferUtf8[wCurrlen]=(utf8_t)__ENDOFSTRING__;
        break;
    case OET_UTF16:
        wBufferUtf16[wCurrlen]=(utf16_t)__ENDOFSTRING__;
        break;
    case OET_UTF32:
        wBufferUtf32[wCurrlen]=(utf32_t)__ENDOFSTRING__;
        break;
    default:
        break;
    }// switch wOutEncoding

 if (pCheckFormat & uspf_FormatAndVars)
            fprintf(stdout,"%s> Output size <%ld> character units\n",
                    _GET_FUNCTION_NAME_,
                    wCurrlen);
_RETURN_ wCurrlen;
} //utfSubDoprintfvoid


//======================== Main routines==================================================

size_t
utfSprintf(ZCharset_type pCharset, void *pBuffer,  const utf8_t *pFormat, ...)
{

/*    VA_LOCAL_DECL;

    VA_START(pFormat);
    VA_SHIFT(pString, _Utf *);
    VA_SHIFT(pCount, size_t);
    VA_SHIFT(pFormat, _Utf *);*/
    va_list args;
    va_start(args,pFormat);


//    size_t wRet= utfVsnprintf(pCharset,pString, pCount, pFormat, ap);
    size_t wRet= utfVsnprintf(pCharset,pBuffer, cst_MaxSprintfBufferCount,(utf8_t*) pFormat, args);

    va_end(args);
//    VA_END;
    return (wRet);
}
void
utfCheckFormat(const utfFmt*pFormat,...)
{
    va_list args;
    va_start(args,pFormat);
    utfSubDoprintfvoid(OET_UTF8, /* Output buffer utf type : only Unicode is allowed there */
                       nullptr,  /* Buffer to be filled in */
                       0,        /* maximum buffer offset in character units */
                       pFormat,  /* an utfFmt string containing formatting instructions */
                       args,
                       nullptr,
                       uspf_FormatAlone);
    va_end(args);
}
size_t
utfCheckFullFormat(const utfFmt*pFormat,...)
{
size_t wRet;
    va_list args;
    va_start(args,pFormat);

    wRet=utfSubDoprintfvoid(OET_UTF8,   /* Output buffer utf type : only Unicode is allowed there */
                       nullptr,         /* Buffer to be filled in */
                       0,               /* maximum buffer offset in character units */
                       pFormat,         /* an utfFmt string containing formatting instructions */
                       args,
                       nullptr,
                       uspf_FormatAndVars);
    va_end(args);
    return wRet;
}

size_t
utfSnprintf(ZCharset_type pCharset, void *pString, size_t pCount, const utf8_t *pFormat, ...)
{
    va_list args;
    va_start(args,pFormat);
    size_t wRet= utfVsnprintf(pCharset,pString, pCount,(utf8_t*) pFormat, args);

    va_end(args);
    return (wRet);
}




const char*
decode_OET(OutEncoding_type pOET)
{
const char* pOutCSName;
switch (pOET)
{
case OET_Char:
    return getDefaultEncodingName();
case OET_UTF8:
    return "UTF8";
case OET_UTF16:
    pOutCSName="UTF16BE";
    if (is_little_endian())
            pOutCSName= "UTF16LE";
    return pOutCSName;
case OET_UTF32:
    pOutCSName="UTF32BE";
    if (is_little_endian())
            pOutCSName= "UTF32LE";
    return pOutCSName;
default:
    return "Unknown OET";
    break;
}
}

char wFMTFBuffer[500];
const char*
decode_FMTFlag(FMT_Flags_type pFlag)
{
    memset (wFMTFBuffer,0,sizeof(wFMTFBuffer));

    if (pFlag & FMTF_Left)
        strcat(wFMTFBuffer,"FMTF_Left ");
    if (pFlag & FMTF_Plus)
        strcat(wFMTFBuffer,"FMTF_Plus ");
    if (pFlag & FMTF_SignSpace)
        strcat(wFMTFBuffer,"FMTF_SignSpace ");
    if (pFlag & FMTF_ZeroPad)
        strcat(wFMTFBuffer,"FMTF_ZeroPad ");
    if (pFlag & FMTF_Hash)
        strcat(wFMTFBuffer,"FMTF_Hash ");

    if (pFlag & FMTF_Minimum)
        strcat(wFMTFBuffer,"FMTF_Minimum ");
    if (pFlag & FMTF_Precision)
        strcat(wFMTFBuffer,"FMTF_Precision ");


    if (pFlag & FMTF_Int)
        strcat(wFMTFBuffer,"FMTF_Int ");
    if (pFlag & FMTF_Float)
        strcat(wFMTFBuffer,"FMTF_Float ");
    if (pFlag & FMTF_String)
        strcat(wFMTFBuffer,"FMTF_String ");
    if (pFlag & FMTF_Other)
        strcat(wFMTFBuffer,"FMTF_Other ");
    if (pFlag & FMTF_ShortInt)
        strcat(wFMTFBuffer,"FMTF_ShortInt ");
    if (pFlag & FMTF_Long)
        strcat(wFMTFBuffer,"FMTF_Long ");
    if (pFlag & FMTF_LongLong)
        strcat(wFMTFBuffer,"FMTF_LongLong ");


    if (pFlag & FMTF_Unsigned)
        strcat(wFMTFBuffer,"FMTF_Unsigned ");
    if (pFlag & FMTF_Octal)
        strcat(wFMTFBuffer,"FMTF_Octal ");
    if (pFlag & FMTF_Hexa)
        strcat(wFMTFBuffer,"FMTF_Hexa ");

    if (pFlag & FMTF_Exponent)
        strcat(wFMTFBuffer,"FMTF_Exponent ");
    if (pFlag & FMTF_GFormat)
        strcat(wFMTFBuffer,"FMTF_GFormat ");

    if (pFlag & FMTF_Pointer)
        strcat(wFMTFBuffer,"FMTF_Pointer ");
    if (pFlag & FMTF_SizeT)
        strcat(wFMTFBuffer,"FMTF_SizeT ");
    if (pFlag & FMTF_PTRDIF_T)
        strcat(wFMTFBuffer,"FMTF_PTRDIF_T ");
    if (pFlag & FMTF_Intmax_t)
        strcat(wFMTFBuffer,"FMTF_Intmax_t ");

    if (pFlag & FMTF_NonSignif)
        strcat(wFMTFBuffer,"FMTF_NonSignif ");

    if (pFlag & FMTF_NonSignif)
        strcat(wFMTFBuffer,"FMTF_NonSignif ");

    if (pFlag & FMTF_Amount)
        strcat(wFMTFBuffer,"FMTF_Amount ");

    if (pFlag & FMTF_Currency)
        strcat(wFMTFBuffer,"FMTF_Currency ");
    if (pFlag & FMTF_Group)
        strcat(wFMTFBuffer,"FMTF_Group ");

    if (pFlag & FMTF_CurPreceeds)
        strcat(wFMTFBuffer,"FMTF_CurPreceeds ");
    if (pFlag & FMTF_CurFollows)
        strcat(wFMTFBuffer,"FMTF_CurFollows ");
    if (pFlag & FMTF_SignPrec)
        strcat(wFMTFBuffer,"FMTF_SignPrec ");
    if (pFlag & FMTF_SignFolw)
        strcat(wFMTFBuffer,"FMTF_SignFolw ");

    return (const char*)wFMTFBuffer;
}


#endif // UTFSPRINTF_CPP
