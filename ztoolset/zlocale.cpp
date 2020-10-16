#ifndef ZLOCALE_CPP
#define ZLOCALE_CPP

#include <ztoolset/zmem.h> // for _free()
#include <ztoolset/zerror.h> // for macros & status
#include <ztoolset/zcharset.h>

#include <ztoolset/zlocale.h>

void GCleanCurrencyCode(void)
{
    _free(GCurrencyCode8);
    _free(GCurrencyCode16);
    _free(GCurrencyCode32);
}

UST_Status_type
GGetLocaleCodes(const char*pLocale)
{
    atexit(&GCleanCurrencyCode);
    setlocale(LC_ALL, pLocale);
    setlocale(LC_NUMERIC, pLocale);
    setlocale(LC_MONETARY, pLocale);
    setlocale(LC_TIME, pLocale);

     GLocaleInfo = localeconv();

     GDecimalPoint = (const utf8_t*)GLocaleInfo->mon_decimal_point;
     if (!GDecimalPoint || (GDecimalPoint[0]==0))
                        GDecimalPoint = (const utf8_t*)GLocaleInfo->decimal_point;

     GThousandSep = (const utf8_t*)GLocaleInfo->mon_thousands_sep;
     GGrouping =(int) *GLocaleInfo->mon_grouping;
     GDecimalNb = (int) GLocaleInfo->int_frac_digits;



     return GChangeLocaleCurrencyCode((utf8_t*)GLocaleInfo->currency_symbol);
}

UST_Status_type
 GChangeLocaleCurrencyCode(const utf8_t *pCurrency)
{
_MODULEINIT_
UST_Status_type wSt;
    if (!pCurrency)
           {
           fprintf(stderr,"%s> Warning: no local currency symbol has been given. Currency code is not changed from locale default\n",_GET_FUNCTION_NAME_);
//           GCurrencyCode = "";
           return UST_NULLPTR;
           }
    GCurrencyCode=pCurrency;
    size_t wLen=utfStrlen<utf8_t>(GCurrencyCode);
    wLen++;
    utf8_t*wCurPtr= (utf8_t*)pCurrency;
    utf32_t wCodePoint=0;
    utf16_t wUtf16[3];
    utf8_t wUtf8[5];
    size_t wUtf8Count,wUtf16Count=0,wi=0;
    utf32_t* wUtf32Ptr=GCurrencyCode32=(utf32_t*)calloc(wLen,sizeof(utf32_t));
    utf16_t* wUtf16Ptr=GCurrencyCode16=(utf16_t*)calloc(wLen,sizeof(utf16_t));
    utf8_t* wUtf8Ptr=GCurrencyCode8=(utf8_t*)calloc(wLen,sizeof(utf8_t));

    while (*wCurPtr)
        {
        *wUtf8Ptr= *wCurPtr;
        wCurPtr++;
        wUtf8Ptr++;
        }
    *wUtf8Ptr=(utf8_t)'\0';
    wCurPtr= (utf8_t*)pCurrency;
    while ((*wCurPtr)&&(wi<wLen))
        {
        wSt=utf8Decode(wCurPtr,&wCodePoint,&wUtf8Count,nullptr);
        _ASSERT_(UST_Is_Severe(wSt),"while parsing utf8 currency code %s",(const char*)pCurrency)
        *wUtf32Ptr=wCodePoint;
        wUtf32Ptr++;

        wSt=utf16Encode((utf16_t*)wUtf16,&wUtf16Count,wCodePoint,nullptr);
        _ASSERT_(UST_Is_Severe(wSt),"while encoding utf16 currency code %s",(const char*)pCurrency)
        for (long wj=0;wj<wUtf16Count;wj++)
               {
               *wUtf16Ptr=wUtf16[wj];
               wUtf16Ptr++;
               }
        wCurPtr+=wUtf8Count;
        }
    *wUtf32Ptr=(utf32_t)'\0';
    *wUtf16Ptr=(utf16_t)'\0';
    if (ZVerbose)
            fprintf (stdout,"%s> Currency symbol set to &s.\n",
                     _GET_FUNCTION_NAME_,
                     (const char*)GCurrencyCode8);
    return UST_SUCCESS;
}// GChangeCurrencyCode


void
GChangelocaleDecimalPoint(const utf8_t* pChar)
{
    GDecimalPoint=pChar;
    return;
}
void
GChangelocaleGroupSep(const utf8_t* pChar)
{
    GThousandSep=pChar;
    return;
}


int
getLocaleGroupNumber(void)
{
if (!GLocaleInfo)
     GGetLocaleCodes();

return GGrouping;
}

int
getLocaleMonetaryDecimal(void)
{
if (!GLocaleInfo)
     GGetLocaleCodes();

return GDecimalNb;
}
/**
 * @brief getLocaleCurrencyPosition returns true if currency symbol preceeds amount, false if succeeds
 * @return
 */
ZBool
getLocaleCurrencyPosition(void)
{
if (!GLocaleInfo)
     GGetLocaleCodes();

return GLocaleInfo->p_cs_precedes;
}
/**
 * @brief getLocaleSignPosition returns true if sign preceeds amount, false if succeeds
 * @return
 */
ZBool
getLocaleSignPosition(void)
{
if (!GLocaleInfo)
     GGetLocaleCodes();

return (GLocaleInfo->n_sign_posn==1)||(GLocaleInfo->n_sign_posn==3);
}

#endif // ZLOCALE_CPP
