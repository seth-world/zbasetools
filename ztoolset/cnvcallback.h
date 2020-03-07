#ifndef CNVCALLBACK_H
#define CNVCALLBACK_H
/** @file cnvcallback.h This file contains icu custom callbacks when using UConverters objects.
 *  Routine prototypes in this file are for internal use only and should not be modified.
 *
 *  Routine code detail is located within zicuconverter.cpp file
 */
#include <unicode/ucnv_err.h>
#include <unicode/ucnv.h>



U_STABLE void U_EXPORT2 UCNV_TO_U_CALLBACK_SUBSTITUTE (
                  const void *context,
                  UConverterToUnicodeArgs *toUArgs,
                  const char* codeUnits,
                  int32_t length,
                  UConverterCallbackReason reason,
                  UErrorCode * err);

//#include <unicode/ucnv_cb.h>



/**
 * @brief _FromCallback Callback routine Used within ICU<br>
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
        UErrorCode *pErrorCode);

U_CDECL_END


U_CDECL_BEGIN
void U_EXPORT2 _toCallback (const void* pContext,                   // when calling second hand ucnv callback routine, pContext contains "action code"
        UConverterToUnicodeArgs *pUcnvArgs,
        const char *pCodeUnits,
        int32_t pLength,
        UConverterCallbackReason pReason,
        UErrorCode *pErrorCode);

U_CDECL_END




#endif // CNVCALLBACK_H
