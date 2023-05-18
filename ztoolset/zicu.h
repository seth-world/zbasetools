#ifndef ZICU_H
#define ZICU_H
/**
  @file zicu.h contains elements to interface with icu

  */

#include <config/zconfig.h>
#include <ztoolset/zlimit.h>
#include <ztoolset/zerror.h>

#include <unicode/ucnv.h>

#include <ztoolset/utfstrctx.h>

/**
 * @brief testIcuError test wether there is an icu error on last icu operation.(Internal routine)
 * <br> If there is an errored status, error is to be considered to have severity level Severity_Error
 * @param pModule   calling function/method name (_GET_FUNCTION_NAME_)
 * @param pCode     icu Last error code to analyze
 * @param pWarningSignal    boolean. When set to true and in case of warning status,
  *  routine will generate a ZException structure with the description of the warning.
 * @param pMessage  Error message in case of errored status
 *
 * @return an internal UST_Status_type :
 *  <b>UST_SUCCESS</b>-> no error
 *  <b>UST_WARNING</b> -> only warning - if pWarningSignal is set to true a ZException is set with explanation message.
 *  <b>UST_CONVERROR</b> -> there was an icu error : a ZException is systematically set with explanation message.
 */
UST_Status_type testIcuFatal(const char*pModule, UErrorCode pCode, bool pWarningSignal, const char* pMessage,...);
/**
 * @brief testIcuError test wether there is an icu error on last icu operation.(Internal routine)
 * <br> If there is an errored status, error is to be considered to have severity level Severity_Fatal.
 * @param pModule   calling function/method name (_GET_FUNCTION_NAME_)
 * @param pCode     icu Last error code to analyze
 * @param pWarningSignal    boolean. When set to true and in case of warning status,
  *  routine will generate a ZException structure with the description of the warning.
 * @param pMessage  Error message in case of errored status
 *
 * @return an internal UST_Status_type :
 *  <b>UST_SUCCESS</b>-> no error
 *  <b>UST_WARNING</b> -> only warning - if pWarningSignal is set to true a ZException is set with explanation message.
 *  <b>UST_CONVERROR</b> -> there was an icu error : a ZException is systematically set with explanation message.
 */
UST_Status_type testIcuFatal(const char*pModule, UErrorCode pCode, bool pWarningSignal, const char* pMessage,...);


ZStatus ztestIcuError(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...);
ZStatus ztestIcuFatal(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...);

#endif // ZICU_H
