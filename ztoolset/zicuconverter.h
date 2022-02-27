#ifndef ZICUCONVERTER_H
#define ZICUCONVERTER_H
#include <zconfig.h>
#include <ztoolset/zlimit.h>
#include <ztoolset/zerror.h>

#include <ztoolset/zcharset.h>
#include <ztoolset/zmem.h>

#include <ztoolset/utfstrctx.h>
#include <ztoolset/zicu.h>

#include <ztoolset/cnvcallback.h> // for having icu custom callbacks


class zicuConverter
{
public:
    zicuConverter();

    ~zicuConverter();

    ZStatus setup(const char*pName, ZBool pWarningSignal);
    ZStatus setup(utfStrCtx* pContext,const char*pName, ZBool pWarningSignal);


    UST_Status_type open_toEncoding(const char* pExtEncoding=nullptr);
    ZStatus open_toEncoding_reuse (void);

    ZStatus open_fromEncoding (const char* pExtEncoding=nullptr);
    ZStatus open_fromEncoding_reuse (void);
    /**
     * @brief zicuConverter::Utf16toEncoding Does not allocate any memory :
     * Uses already allocated pOutCharStr with a maximum pOutMax.
     * returns effective units counts in pOutCount
     *
     * @param pOutCharStr
     * @param pOutCount     Number of effective character units used in pOutCharStr
     * @param pOutMax       Maximum allocated number of character units within pOutCharStr
     * @param putf16String  Utf16 input string to convert
     * @param pInCount      Number of utf16 character units : this number must include endofstring mark.
     * @param pExtEncoding  Name of the encoding to convert to
     * @return an UST_Status_type with values from utfStatus_type range of values.
     */
   UST_Status_type Utf16toEncoding (const char* pOutEncoding, char* pOutCharStr, size_t pOutCapacity,
                                    utf16_t* putf16String, size_t pInCount);

    template <class _Utf>
    ZStatus toEncoding (char** pOutCharStr,size_t* pOutCount,
                        _Utf* putfString,ssize_t pInCount,
                        const char* pExtEncoding=nullptr);

    template <class _Utf>
    ZStatus fromEncoding (_Utf** putfString,size_t* pOutSize,
                          const char* pInString,size_t pInSize,
                          const char* pExtEncoding=nullptr);



    inline void _reset(void)
    {
        if (extICUcnv)
                ucnv_reset(extICUcnv);
        if (mainICUcnv)
                ucnv_reset(mainICUcnv);
    }

    void _close(void) {ucnv_close(extICUcnv); extICUcnv=nullptr;}


    void setWarningSignal(ZBool pSignal) {WarningSignal=pSignal;}

private:
    ZStatus _mainCnvOpen(const char *pName);

protected:
    UConverterCallbackReason  Reason;
    UErrorCode  ICUErr=U_ZERO_ERROR;
    UConverter* extICUcnv=nullptr;
    UConverter* mainICUcnv=nullptr;
    utfStrCtx   mainCnvContext;
    utfStrCtx   extContext;

    const char* mainCharset=nullptr;

private:
    ZBool                   WarningSignal=false;

    ZBool                   mainOld_SaveOnce=false;
    UConverterFromUCallback mainOld_FromAction;
    UConverterToUCallback   mainOld_ToAction;
    const void*             mainOld_FromContext=nullptr;
    const void*             mainOld_ToContext=nullptr;

    UConverterFromUCallback extOld_FromAction;
    UConverterToUCallback   extOld_ToAction;
    const void*             extOld_FromContext=nullptr;
    const void*             extOld_ToContext=nullptr;
    UConverterType          extCnvType=UCNV_UNSUPPORTED_CONVERTER;
};



template <class _Utf>
/**
 * @brief zicuConverter::toEncoding converts putfString from converter's encoding to external encoding pExtEncoding.<br>
 * Allocates characters pOutcount units within pOutChar and move the converted string to it as return.
 * @param pOutCharStr
 * @param pOutCount
 * @param putfString
 * @param pInCount
 * @param pExtEncoding
 * @return
 */
ZStatus
zicuConverter::toEncoding (char** pOutCharStr,size_t* pOutCount,
                           _Utf* putfString,ssize_t pInCount,
                           const char* pExtEncoding)
{

ZStatus wSt=ZS_SUCCESS;
UST_Status_type wUSTSt;

    if (!pOutCharStr)
            _ABORT_;
    if (!putfString)
            _ABORT_;
    *pOutCharStr=nullptr;

    /* if destination encoding is omitted, we do not allow icu to take its default encoding,
     * but we provide our default encoding name (it may be set by setDefaultEncoding() function */

    const char* wExtEncoding;
    if (pExtEncoding==nullptr)
            wExtEncoding=getDefaultEncodingName();
        else
            wExtEncoding=pExtEncoding;

    /* manage size of input string : if -1 (omitted) then compute it  */
    ssize_t wCount=pInCount,wInByteSize ;

    if (pInCount<0)
        {
        wCount=0;
        while (putfString[wCount++]) ;  // get strlen
        wCount++;                       // take care of endofstring mark that should be included
        }
    wInByteSize = sizeof(_Utf)*(wCount);
    wUSTSt=open_toEncoding(wExtEncoding);

    if (wUSTSt<UST_SEVERE)
           { return ZS_ICUERROR;}

    // step 1- convert to utf16 pivot string

    UChar* wUPivot= (UChar*)calloc(wCount,sizeof(char16_t));
    if(!wUPivot)
            { return ZS_MEMERROR; }
    UChar* wUActual=wUPivot, *wULimit=&wUPivot[wCount-1];
    const _Utf* wInPtr=putfString,*wInLimit=&putfString[wInByteSize-1];
    int32_t* wInOffsets= (int32_t*)calloc(wCount,sizeof(int32_t));

    ICUErr=U_ZERO_ERROR;
    ucnv_toUnicode(mainICUcnv,
                   &wUActual,
                   wULimit,     // Out string boundary
                   (const char**)&wInPtr,
                   (const char*)wInLimit,    // pInString boundary
                   wInOffsets,    // get the set of corresponding offsets to source string (for fun)
                   true,        // true : last piece of chunk... we may stream in data with that
                   &ICUErr);
    wSt=ztestIcuError(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     ICUErr,
                     WarningSignal,
                     " while Converting <%s> string to unicode pivot\n",mainCharset);
    if (zis_Error(wSt))
                    {
                    zfree(wUPivot);
                    return wSt;
                    }

    // step2 convert from utf16 pivot to destination charset

    *pOutCharStr=(char*)calloc(wCount,sizeof(char));
    if(!*pOutCharStr)
            {
            zfree(wInOffsets);
            zfree(wUPivot);
            return ZS_MEMERROR;
            }
    *pOutCount = wCount*sizeof(char);

    char* wOutPtr=*pOutCharStr,*wOutLimit=&(*pOutCharStr)[wCount-1];
    int32_t* wOutOffsets= (int32_t*)calloc(wCount,sizeof(int32_t));
    wUActual=wUPivot;  // reset position at top of wUPivot

    ICUErr=U_ZERO_ERROR;
    ucnv_fromUnicode(extICUcnv,
                     &wOutPtr,
                     wOutLimit,    // OutString boundary
                     (const UChar**)&wUActual,
                     wULimit,     // Out string boundary
                     wOutOffsets,    // get the set of corresponding offsets to pivot string (for fun)
                     true,        // true : last piece of chunk... we may stream in data with that
                     &ICUErr);

    wSt=ztestIcuError(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     ICUErr,
                     WarningSignal,
                     " while Converting _Utf string to external encoding <%s>\n",
                     wExtEncoding);


    //  free allocated memory

    zfree(wInOffsets);
    zfree(wOutOffsets);
    zfree(wUPivot);

    return wSt;
}//toEncoding




template <class _Utf>
ZStatus
zicuConverter::fromEncoding(_Utf** putfString,size_t* pOutCount,
                              const char* pInString,size_t pInSize,
                              const char* pExtEncoding)
{

ZStatus wSt=ZS_SUCCESS;

    if (!putfString)
            _ABORT_;
    if (!pInString)
            _ABORT_;
    *putfString=nullptr;

    /* if source encoding is omitted, we do not allow icu to take its default encoding,
     * but we provide our default encoding name (it may be set by setDefaultEncoding() function */

    const char* wExtEncoding;
    if (pExtEncoding==nullptr)
            wExtEncoding=getDefaultEncodingName();
        else
            wExtEncoding=pExtEncoding;

    /* manage size of input string : if -1 (omitted) then compute it  */
    ssize_t wCount=pInSize,wInByteSize;

    if (pInSize<0)
        {
        wCount=0;
        while (putfString[wCount++]) ;  // get strlen
        wCount++;                       // take care of endofstring mark that should be included
        }
    wInByteSize = sizeof(_Utf)*(wCount);
    wSt=open_fromEncoding(wExtEncoding);

    if (zis_Error(wSt))
           { return wSt;}

    // step 1- convert from external charset to utf16 pivot string

    UChar* wUPivot= (UChar*)calloc(wCount,sizeof(char16_t));
    if(!wUPivot)
            { return ZS_MEMERROR; }
    UChar* wUActual=wUPivot, *wULimit=&wUPivot[wCount-1];
    const char* wInPtr=pInString,*wInLimit=&pInString[wCount-1];
    int32_t* wInOffsets= (int32_t*)calloc(wCount,sizeof(int32_t));

    ICUErr=U_ZERO_ERROR;
    ucnv_toUnicode(extICUcnv,
                   &wUActual,
                   wULimit,     // Out string boundary
                   (const char**)&wInPtr,
                   (const char*)wInLimit,    // pInString boundary
                   wInOffsets,    // get the set of corresponding offsets to source string (for fun)
                   true,        // true : last piece of chunk... we may stream in data with that
                   &ICUErr);
    wSt=ztestIcuError(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     ICUErr,
                     WarningSignal,
                     " while Converting string encoded from <%s> to unicode pivot\n", wExtEncoding);
    if (zis_Error(wSt))
                    {
                    zfree(wUPivot);
                    return wSt;
                    }

    // step2 convert from unicode (utf16 pivot string) to destination _Utf

    *putfString=(_Utf*)calloc(wCount,sizeof(_Utf));
    if(!*putfString)
            {
            zfree(wInOffsets);
            zfree(wUPivot);
            return ZS_MEMERROR;
            }
    *pOutCount = wCount ;

    _Utf* wOutPtr=*putfString,*wOutLimit=&(*putfString)[wCount-1];
    int32_t* wOutOffsets= (int32_t*)calloc(wCount,sizeof(int32_t));
    wUActual=wUPivot;  // reset position at top of wUPivot

    ICUErr=U_ZERO_ERROR;
    ucnv_fromUnicode(extICUcnv,
                     (char**)&wOutPtr,
                     (char*)wOutLimit,    // OutString boundary
                     (const UChar**)&wUActual,
                     wULimit,     // Out string boundary
                     wOutOffsets,    // get the set of corresponding offsets to pivot string (for fun)
                     true,        // true : last piece of chunk... we may stream in data with that
                     &ICUErr);

    wSt=ztestIcuError(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     ICUErr,
                     WarningSignal,
                     " while Converting unicode (utf16 pivot) string to _Utf <%s>\n", mainCharset);


    //  free allocated memory

    zfree(wInOffsets);
    zfree(wOutOffsets);
    zfree(wUPivot);

    return wSt;
}//fromEncoding


#endif // ZICUCONVERTER_H
