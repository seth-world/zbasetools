#ifndef ZICUCONVERTER_CPP
#define ZICUCONVERTER_CPP

#include <ztoolset/zicuconverter.h>

zicuConverter::zicuConverter()
{
}
ZStatus
zicuConverter::setup(const char*pName, ZBool pWarningSignal)
{
    _MODULEINIT_
    setWarningSignal(pWarningSignal);
    _RETURN_ _mainCnvOpen(pName);
}

ZStatus
zicuConverter::setup(utfStrCtx* pContext,const char*pName, ZBool pWarningSignal)
{
    _MODULEINIT_
    setWarningSignal(pWarningSignal);
    mainCnvContext._from(pContext);
    _RETURN_ _mainCnvOpen(pName);


}


zicuConverter::~zicuConverter()
{
    if (mainICUcnv)
            {
            ucnv_close(mainICUcnv);
            extICUcnv=nullptr;
            }
    if (extICUcnv)
            {
            ucnv_close(extICUcnv);
            extICUcnv=nullptr;
            }
}


/**
 * @brief _fromCallback Callback routine Used within ICU<br>
 *                      Evaluate the reason (UConverterCallbackReason)
 *  UCNV_UNASSIGNED :   codePoint is unassigned -> U_INVALID_CHAR_FOUND : may be skipped or stop
 *  UCNV_ILLEGAL :      The code point is illegal -> U_ILLEAL_CHAR_FOUND is set as error : may be substituted or skipped or stop
 *                      For example,
                             \\x81\\x2E is illegal in SJIS because \\x2E
                             is not a valid trail byte for the \\x81
                             lead byte.
 *  UCNV_IRREGULAR:     The code point is not a regular sequence in the encoding
 *                      For example, \\xED\\xA0\\x80..\\xED\\xBF\\xBF
                             are irregular UTF-8 byte sequences for single surrogate
                             code points

 *  UCNV_RESET:         a reset has occurred. Callback should reset its state.
 *
 *  UCNV_CLOSE:         the converter is to be closed. Callback should release any allocated memory.
 *
 *  UCNV_CLONE:         ucnv_safeClone() is called on the converter.
 *                      the pointer available as the
                        'context' is an alias to the original converters'
                        context pointer. If the context must be owned
                        by the new converter, the callback must clone
                        the data and call ucnv_setFromUCallback
                        (or setToUCallback) with the correct pointer.
 *
 * @param pContext  a pointer to utfStrCtx
 * @param pUcnvArgs icu converter internal arguments
 * @param pCodeUnits
 * @param pLength
 * @param pCodePoint
 * @param pReason
 * @param pErrorCode
 */
U_CDECL_BEGIN
void _fromCallback (
        const void* pContext,                   // when calling second hand ucnv callback routine, pContext contains "action code"
        UConverterFromUnicodeArgs *pUcnvArgs,
        const UChar* pCodeUnits,
        int32_t pLength,
        UChar32 pCodePoint,
        UConverterCallbackReason pReason,
        UErrorCode *pErrorCode)
{
_MODULEINIT_
    const char* wEscType=nullptr;
    UErrorCode wICUErr;
    if (pContext==nullptr)
        {
        fprintf (stderr,
                 "%s> Fatal error: callback argument pContext is nullptr while expecting an argument.\n",
                 _GET_FUNCTION_NAME_);
        _ABORT_
        }

    void* wContextPtr= const_cast<void*>(pContext);
    utfStrCtx*   wContext=static_cast<utfStrCtx*>(wContextPtr);

    wContext->CodePoint = pCodePoint;

    switch (pReason)
    {
    /* First : process converter internal management requests i. e. safe close - reset - close */
    case UCNV_CLONE:
        {
            /* ICU says : When 'safe clone' reason is received :
             *  - clone context
             *  - use  ucnv_setToUCallBack() to setup cloned converter
             */
         UConverterFromUCallback OldAction;
         const void*             OldContext=nullptr;
         utfStrCtx*   wNewContext= wContext->_clone();
         ucnv_setFromUCallBack(pUcnvArgs->converter,
                               _fromCallback,
                               (const void*)wNewContext,
                               &OldAction,
                               &OldContext,
                               &wICUErr
                               );
        _RETURN_;
        }
    case UCNV_RESET:
        {
            /* ICU says : When 'reset' reason is received :
             *  - context must be reset
             */
        UConverterFromUCallback OldAction;
        const void*             OldContext=nullptr;
        utfStrCtx*   wNewContext= wContext->_clone();
        ucnv_setFromUCallBack(pUcnvArgs->converter,
                               _fromCallback,
                               (const void*)wNewContext,
                               &OldAction,
                               &OldContext,
                               &wICUErr
                               );
        _RETURN_;
        }
    case UCNV_CLOSE:
        {
            /* ICU says : When 'close' reason is received :
             *   free any allocated memory : if context has been cloned, then delete it
             */
        if (wContext->Cloned)
                    delete wContext;
        _RETURN_;
        }

/* all other reasons are managed by underneeth standard ICU routines - according to ZCNV_Action
        UCNV_UNASSIGNED:
        UCNV_IRREGULAR
        UCNV_ILLEGAL
*/
    default: // there remain actions to apply
        {
        /* First : register character issue within utfStrCtx object */

        wContext->ErrFrom ++;
        if (!wContext->Start)       // first time called : feed base infos
        {
         wContext->setStart(pUcnvArgs->source);
         wContext->setEffective(pUcnvArgs->source);
        }
        wContext->utfSetPosition<UChar>(pUcnvArgs->source+pUcnvArgs->offsets[0]); // icu offsets is a set of offsets

        if (wContext->TraceFrom)      /* if trace is requested for 'From Unicode' operation*/
                {
                UST_Status_type wSt;
                if (pReason==UCNV_UNASSIGNED)
                            wSt=UST_UNASSIGNED;
                    else
                    if (pReason==UCNV_IRREGULAR)
                            wSt=UST_IRREGULAR;
                    else
                    if (pReason==UCNV_ILLEGAL)
                            wSt=UST_ILLEGAL;

                utfSCErr_struct wError;
                wError.UnitOffset=wContext->InUnitOffset;
                wError.Action = wContext->Action;
                wError.codePoint = wContext->CodePoint;
                wError.Status = wSt;
                wError.Size=pUcnvArgs->size;
                wContext->ErrorQueue->push(wError);
//                 wContext->ErrorQueue->newError(wContext->InUnitOffset,wSt,SCErr_Output,pCodePoint,wContext->Action);
                }
        /* second : process requested action according wContext-> Action.

        */
        switch (wContext->Action)
            {
            case ZCNV_Stop:
                {
                UCNV_FROM_U_CALLBACK_STOP(nullptr,
                                          pUcnvArgs,
                                          pCodeUnits,
                                          pLength,
                                          pCodePoint,
                                          pReason,
                                          pErrorCode);
                _RETURN_;
                }
            case ZCNV_Skip:
                    {
                    if (wContext->StopOnConvErr){
                            UCNV_FROM_U_CALLBACK_SKIP(UCNV_SKIP_STOP_ON_ILLEGAL,
                                                      pUcnvArgs,
                                                      pCodeUnits,
                                                      pLength,
                                                      pCodePoint,
                                                      pReason,
                                                      pErrorCode);
                            }
                            else{   // no stop on illegal character
                            UCNV_FROM_U_CALLBACK_SKIP(nullptr,
                                                      pUcnvArgs,
                                                      pCodeUnits,
                                                      pLength,
                                                      pCodePoint,
                                                      pReason,
                                                      pErrorCode);
                            }
                    _RETURN_;
                    }//ZCNV_Skip
            case ZCNV_Substitute:
                    {
                    if (wContext->StopOnConvErr){
                            UCNV_FROM_U_CALLBACK_SUBSTITUTE(UCNV_SUB_STOP_ON_ILLEGAL,
                                                      pUcnvArgs,
                                                      pCodeUnits,
                                                      pLength,
                                                      pCodePoint,
                                                      pReason,
                                                      pErrorCode);
                            }
                            else{   // no stop on illegal character : substitute all errored characters
                            UCNV_FROM_U_CALLBACK_SUBSTITUTE(nullptr,
                                                      pUcnvArgs,
                                                      pCodeUnits,
                                                      pLength,
                                                      pCodePoint,
                                                      pReason,
                                                      pErrorCode);
                            }
                    _RETURN_;
                    }//ZCNV_Substitute


                case ZCNV_Escape_ICU:
                    wEscType=UCNV_ESCAPE_ICU;
                case ZCNV_Escape_Java:
                    wEscType=UCNV_ESCAPE_JAVA;
                case ZCNV_Escape_C:
                    wEscType=UCNV_ESCAPE_C;
                case ZCNV_Escape_XML_Dec:
                    wEscType=UCNV_ESCAPE_XML_DEC;
                case ZCNV_Escape_XML_Hex:
                    wEscType=UCNV_ESCAPE_XML_HEX;
                case ZCNV_Escape_Unicode:
                    wEscType=UCNV_ESCAPE_UNICODE;
                case ZCNV_Escape_CSS2:
                    wEscType=UCNV_ESCAPE_CSS2;

                 UCNV_FROM_U_CALLBACK_ESCAPE(wEscType,
                                          pUcnvArgs,
                                          pCodeUnits,
                                          pLength,
                                          pCodePoint,
                                          pReason,
                                          pErrorCode);
            }// default
            }//switch (wAction)

        }//switch-default
    }// switch pReason

}// _fromCallback


typedef void (U_EXPORT2 *UConverterFromUCallback) (
                    const void* context,
                    UConverterFromUnicodeArgs *args,
                    const UChar* codeUnits,
                    int32_t length,
                    UChar32 codePoint,
                    UConverterCallbackReason reason,
                    UErrorCode *pErrorCode);

typedef void (U_EXPORT2 *UConverterToUCallback) (
                  const void* context,
                  UConverterToUnicodeArgs *args,
                  const char *codeUnits,
                  int32_t length,
                  UConverterCallbackReason reason,
                  UErrorCode *pErrorCode);

void U_EXPORT2 _toCallback (
        const void* pContext,                   // when calling second hand ucnv callback routine, pContext contains "action code"
        UConverterToUnicodeArgs* pUcnvArgs,
        const char* pCodeUnits,
        int32_t pLength,
        UConverterCallbackReason pReason,
        UErrorCode *pErrorCode)
{
_MODULEINIT_
    const char* wEscType=nullptr;
    UErrorCode wICUErr;
    if (pContext==nullptr)
        {
        fprintf (stderr,
                 "%s> Fatal error: callback argument pContext is nullptr while expecting an argument.\n",
                 _GET_FUNCTION_NAME_);
        _ABORT_
        }
    void* wContextPtr= const_cast<void*>(pContext);
    utfStrCtx*   wContext=static_cast<utfStrCtx*>(wContext);
//    wContext->CodePoint = pCodePoint;

    switch (pReason)
    {
    /* First : process converter internal management requests i. e. safe close - reset - close */
    case UCNV_CLONE:
        {
            /* ICU says : When 'safe clone' reason is received :
             *  - clone context
             *  - use  ucnv_setToUCallBack() to setup cloned converter
             */
         UConverterFromUCallback OldAction;
         const void*             OldContext=nullptr;
         utfStrCtx*   wNewContext= wContext->_clone();
         ucnv_setFromUCallBack(pUcnvArgs->converter,
                               &_fromCallback,
                               (const void*)wNewContext,
                               &OldAction,
                               &OldContext,
                               &wICUErr
                               );
        _RETURN_;
        }
    case UCNV_RESET:
        {
            /* ICU says : When 'reset' reason is received :
             *  - context must be reset
             */
        UConverterFromUCallback OldAction;
        const void*             OldContext=nullptr;
        utfStrCtx*   wNewContext= wContext->_clone();
        ucnv_setFromUCallBack(pUcnvArgs->converter,
                               &_fromCallback,
                               (const void*)wNewContext,
                               &OldAction,
                               &OldContext,
                               &wICUErr
                               );
        _RETURN_;
        }
    case UCNV_CLOSE:
        {
            /* ICU says : When 'close' reason is received :
             *   free any allocated memory : if context has been cloned, then delete it
             */
        if (wContext->Cloned)
                    delete wContext;
        _RETURN_;
        }

/* all other reasons are managed by underneeth standard ICU routines - according to ZCNV_Action
        UCNV_UNASSIGNED:
        UCNV_IRREGULAR
        UCNV_ILLEGAL
*/
    default: // there remain actions to apply
        {

        /* First : register the character issue within utfStrCtx object  */

        wContext->ErrTo ++;
        if (!wContext->Start)       // first time called : feed base infos
        {
         wContext->setStart(pUcnvArgs->source);
         wContext->setEffective(pUcnvArgs->source);
        }
        /* Unfortunately : 'to' operation has an input string type as <const char*>. So, template class is <char>. */

        wContext->utfSetPosition<char>(pUcnvArgs->source+pUcnvArgs->offsets[0]);// icu offsets is a set of offsets

        if (wContext->TraceTo)      /* if trace is requested for 'to Unicode' operations */
                {
                UST_Status_type wSt;
                if (pReason==UCNV_UNASSIGNED)
                            wSt=UST_UNASSIGNED;
                    else
                    if (pReason==UCNV_IRREGULAR)
                            wSt=UST_IRREGULAR;
                    else
                    if (pReason==UCNV_ILLEGAL)
                            wSt=UST_ILLEGAL;

                utfSCErr_struct wError;
                wError.UnitOffset=wContext->InUnitOffset;
                wError.Action = wContext->Action;
                wError.codePoint = 0;
                wError.Status = wSt;
                wError.Size=pUcnvArgs->size;
                wContext->ErrorQueue->push(wError);

 /*               wContext->ErrorQueue->newError(wContext->InUnitOffset,
                                               wSt,
                                               SCErr_Input,
                                               0,                   // no codePoint for input operation
                                               wContext->Action);
   */             }
//        wAction= wAction&~ZCNV_Stop_Illegal;  // remove illegal mask if any
        switch (wContext->Action)
            {
            case ZCNV_Stop:
                {
                UCNV_TO_U_CALLBACK_STOP(nullptr,
                                          pUcnvArgs,
                                          pCodeUnits,
                                          pLength,
                                          pReason,
                                          pErrorCode);
                _RETURN_;
                }
            case ZCNV_Skip:
                    {
                    if (wContext->StopOnConvErr){
                            UCNV_TO_U_CALLBACK_SKIP(UCNV_SKIP_STOP_ON_ILLEGAL,
                                                      pUcnvArgs,
                                                      pCodeUnits,
                                                      pLength,
                                                      pReason,
                                                      pErrorCode);
                            }
                            else{   // no stop on illegal character
                            UCNV_TO_U_CALLBACK_SKIP(nullptr,
                                                      pUcnvArgs,
                                                      pCodeUnits,
                                                      pLength,
                                                      pReason,
                                                      pErrorCode);
                            }
                    _RETURN_;
                    }//ZCNV_Skip
            case ZCNV_Substitute:
                    {
                    if (wContext->StopOnConvErr){
                            UCNV_TO_U_CALLBACK_SUBSTITUTE(UCNV_SUB_STOP_ON_ILLEGAL,
                                                      pUcnvArgs,
                                                      pCodeUnits,
                                                      pLength,
                                                      pReason,
                                                      pErrorCode);
                            }
                            else{   // no stop on illegal character : substitute all errored characters
                            UCNV_TO_U_CALLBACK_SUBSTITUTE(nullptr,
                                                      pUcnvArgs,
                                                      pCodeUnits,
                                                      pLength,
                                                      pReason,
                                                      pErrorCode);
                            }
                    _RETURN_;
                    }//ZCNV_Substitute


                case ZCNV_Escape_ICU:
                    wEscType=UCNV_ESCAPE_ICU;
                case ZCNV_Escape_Java:
                    wEscType=UCNV_ESCAPE_JAVA;
                case ZCNV_Escape_C:
                    wEscType=UCNV_ESCAPE_C;
                case ZCNV_Escape_XML_Dec:
                    wEscType=UCNV_ESCAPE_XML_DEC;
                case ZCNV_Escape_XML_Hex:
                    wEscType=UCNV_ESCAPE_XML_HEX;
                case ZCNV_Escape_Unicode:
                    wEscType=UCNV_ESCAPE_UNICODE;
                case ZCNV_Escape_CSS2:
                    wEscType=UCNV_ESCAPE_CSS2;

                 UCNV_TO_U_CALLBACK_ESCAPE(wEscType,
                                          pUcnvArgs,
                                          pCodeUnits,
                                          pLength,
                                          pReason,
                                          pErrorCode);

            }//switch (wAction)

        }//switch-default
    }// switch pReason
    _RETURN_;
}// _FromCallback



ZStatus
zicuConverter::_mainCnvOpen(const char*pName)
{
_MODULEINIT_
    ICUErr=U_ZERO_ERROR;
    mainICUcnv= ucnv_open(pName,&ICUErr);
    ZStatus wSt=ztestIcuFatal(_GET_FUNCTION_NAME_,
                      ICUErr,
                      WarningSignal,
                      " while opening main converter <%s>", pName);

    if (zis_Error(wSt))
                      {_RETURN_ wSt;}

    mainCharset= pName;
    if (!mainOld_SaveOnce)
    {
    ICUErr=U_ZERO_ERROR;
    ucnv_setFromUCallBack(mainICUcnv,
                        &_fromCallback,
                        (const void*)&mainCnvContext,
                        &mainOld_FromAction,
                        &mainOld_FromContext,
                        &ICUErr
                        );
    wSt=ztestIcuFatal(_GET_FUNCTION_NAME_,
                      ICUErr,
                      WarningSignal,
                      " Setting up (from unicode callback) main <%s> converter ",mainCharset);
    if (zis_Error(wSt))
                      {_RETURN_ wSt;}
    ICUErr=U_ZERO_ERROR;
    ucnv_setToUCallBack(mainICUcnv,
                        &_toCallback,
                        (const void*)&mainCnvContext,
                        &mainOld_ToAction,
                        &mainOld_ToContext,
                        &ICUErr
                        );
    wSt=ztestIcuFatal(_GET_FUNCTION_NAME_,
                      ICUErr,
                      WarningSignal,
                      " Setting up (to unicode callback) main <%s> converter ", mainCharset);
    if (zis_Error(wSt))
                      {_RETURN_ wSt;}
    mainOld_SaveOnce=true;
    }
    else
    {
    ICUErr=U_ZERO_ERROR;
    ucnv_setFromUCallBack(mainICUcnv,
                        &_fromCallback,
                        (const void*)&mainCnvContext,
                        nullptr,
                        nullptr,
                        &ICUErr
                        );
    wSt=ztestIcuFatal(_GET_FUNCTION_NAME_,
                      ICUErr,
                      WarningSignal,
                      " Setting up (from unicode callback) main <%s> converter ",mainCharset);
    if (zis_Error(wSt))
                      {_RETURN_ wSt;}
    ICUErr=U_ZERO_ERROR;
    ucnv_setToUCallBack(mainICUcnv,
                        &_toCallback,
                        (const void*)&mainCnvContext,
                        nullptr,
                        nullptr,
                        &ICUErr
                        );
    wSt=ztestIcuFatal(_GET_FUNCTION_NAME_,
                      ICUErr,
                      WarningSignal,
                      " Setting up (to unicode callback) main <%s> converter ", mainCharset);
    if (zis_Error(wSt))
                      {_RETURN_ wSt;}
    }
_RETURN_ wSt;
}



UST_Status_type
zicuConverter::open_toEncoding (const char* pExtEncoding)
{
_MODULEINIT_
UST_Status_type wSt;

    if (!pExtEncoding)
            getDefaultCharset();

    ucnv_resetToUnicode(mainICUcnv); // from utf8 to charset means for utf8converter from utf8 'to Unicode' (utf16 pivot)

    if (extICUcnv)
    {
        ucnv_close(extICUcnv);
        extICUcnv=nullptr;
    }
    ICUErr=U_ZERO_ERROR;
    extICUcnv=ucnv_open(pExtEncoding,&ICUErr);
    if ((wSt=testIcuError(_GET_FUNCTION_NAME_,
                           ICUErr,
                           WarningSignal,
                           "Opening converter for external charset encoding <%s>",
                           pExtEncoding))<0)
                                    {_RETURN_ wSt;}

    extCnvType = ucnv_getType(extICUcnv);


    // setting from Unicode callback : 'to charset' means 'from Unicode' (utf16 pivot) to charset
    ICUErr=U_ZERO_ERROR;
    ucnv_setFromUCallBack(extICUcnv,
                        &_fromCallback,
                        (const void*)&mainCnvContext,
                        &extOld_FromAction,
                        &extOld_FromContext,
                        &ICUErr);

    if ((wSt=testIcuError(_GET_FUNCTION_NAME_,
                           ICUErr,
                           WarningSignal,
                           "Setting up (from Unicode callback) converter for external charset encoding <%s>",
                           pExtEncoding))<0)
                                {_RETURN_ wSt;}
    _RETURN_ UST_SUCCESS;
}// open_toEncoding

ZStatus
zicuConverter::open_toEncoding_reuse (void)
{
_MODULEINIT_
    ucnv_resetToUnicode(mainICUcnv); // from utf8 to charset means for utf8converter from utf8 'to Unicode' (utf16 pivot)

    if (!extICUcnv)
    {
        fprintf(stderr,
                "%s> Fatal error : external ICU converter is to be reused while it has not be yet used.",
                _GET_FUNCTION_NAME_);
        _ABORT_
    }

    ucnv_resetFromUnicode(extICUcnv);

_RETURN_ ZS_SUCCESS;
}//open_toEncoding_reuse

ZStatus
zicuConverter::open_fromEncoding (const char* pExtEncoding)
{
_MODULEINIT_
ZStatus wSt;
    ucnv_resetFromUnicode(mainICUcnv); // to utf8 from charset means for utf8converter to utf8 'from Unicode' (utf16 pivot)

    if (extICUcnv)
    {
        ucnv_close(extICUcnv);
        extICUcnv=nullptr;
    }
    ICUErr=U_ZERO_ERROR;
    extICUcnv=ucnv_open(pExtEncoding,&ICUErr);  // if pExtEncoding is nullptr : ICU will take system default
    if ((ZStatusBase)(wSt=ztestIcuError(_GET_FUNCTION_NAME_,
                           ICUErr,
                           WarningSignal,
                           "Opening converter for external charset encoding <%s>",
                           pExtEncoding))<0)
                                    {_RETURN_ wSt;}

    // setting to Unicode callback : 'from charset' means 'to Unicode' (utf16 pivot) from charset
    ICUErr=U_ZERO_ERROR;
    ucnv_setToUCallBack(extICUcnv,
                        (UConverterToUCallback) &_toCallback,
                        (const void*)&mainCnvContext,
                        &extOld_ToAction,
                        &extOld_ToContext,
                        &ICUErr
                        );
    if ((ZStatusBase)(wSt=ztestIcuError(_GET_FUNCTION_NAME_,
                           ICUErr,
                           WarningSignal,
                           "Setting up (callback) converter for external charset encoding <%s>",
                           pExtEncoding))<0)
                                    {_RETURN_ wSt;}
    _RETURN_ ZS_SUCCESS;
}// open_fromEncoding

ZStatus
zicuConverter::open_fromEncoding_reuse (void)
{
_MODULEINIT_

    if (!mainICUcnv)
            {
            fprintf(stderr,
                    "%s> Fatal error : main ICU converter is to be used while it has not be yet used.",
                    _GET_FUNCTION_NAME_);
            _ABORT_
            }
    ucnv_resetFromUnicode(mainICUcnv); // to utf8 from charset means for utf8converter to utf8 'from Unicode' (utf16 pivot)

    if (!extICUcnv)
            {
            fprintf(stderr,
                    "%s> Fatal error : external ICU converter is to be reused while it has not be yet used.",
                    _GET_FUNCTION_NAME_);
            _ABORT_
            }
    ucnv_resetToUnicode(extICUcnv); // 'from charset' means from charset 'to Unicode' (utf16 pivot)
_RETURN_ ZS_SUCCESS;
}// open_fromEncoding_reuse

/**
 * @brief zicuConverter::Utf16toEncoding Static method that allows to convert from utf16 to a target encoding pOutEncoding.<br>
 * Does not allocate any memory :<br>
 * Uses already allocated pOutCharStr with a maximum pOutCapacity.<br>
 * returns effective units counts in pOutCount
 *
 * @param pOutEncoding  Name of the encoding to convert to
 * @param pOutCharStr   Target conversion buffer
 * @param pOutCapacity  Maximum allocated number of character units within pOutCharStr
 * @param putf16String  Utf16 input string to convert
 * @param pInCount      Number of utf16 character units : this number must include endofstring mark.
 * @return an UST_Status_type with values from utfStatus_type range of values.
 */
UST_Status_type
zicuConverter::Utf16toEncoding (const char* pOutEncoding, char* pOutCharStr, size_t pOutCapacity,
                                utf16_t* putf16String,size_t pInCount)
{
_MODULEINIT_

   UST_Status_type wSt=open_toEncoding(pOutEncoding);
    if (wSt<UST_SEVERE)
           { _RETURN_ wSt;}

    // step 1- convert to utf16 pivot string


    UChar* wULimit= (UChar*)&putf16String[pInCount-1];
    ICUErr=U_ZERO_ERROR;

    // step2 convert from utf16 pivot to destination charset


    char* wOutLimit=&pOutCharStr[pOutCapacity];
    int32_t* wOutOffsets= nullptr;

    ICUErr=U_ZERO_ERROR;
    ucnv_fromUnicode(extICUcnv,
                     &pOutCharStr,
                     wOutLimit,    // OutString boundary
                     (const UChar**)&putf16String,
                     wULimit,     // Out string boundary
                     wOutOffsets,    // get the set of corresponding offsets to pivot string (for fun)
                     true,        // true : last piece of chunk... we may stream in data with that
                     &ICUErr);

    wSt=testIcuError(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     ICUErr,
                     WarningSignal,
                     " while Converting _Utf string to external encoding <%s>\n",
                     pOutEncoding);

    _RETURN_ wSt;
}//Utf16toEncoding





#endif // ZICUCONVERTER_CPP
