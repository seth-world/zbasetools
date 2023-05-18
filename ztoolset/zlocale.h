#ifndef ZLOCALE_H
#define ZLOCALE_H

#include <config/zconfig.h>
#include <locale>
#include <ztoolset/zlimit.h>  /* for utf8_t etc. */

#include <ztoolset/utfstrctx.h> // for UST_Status values
#include <ztoolset/zsymbols.h> // for preprocessor symbol codes


/**
 * @defgroup LOCALE_PARAMS Locale parameters
 *
 *  Locale parameters are obtained thru gnu localeconv() and lconv.<br>
 *  lconv (data structure for locale) and localeconv() (giving dedicated lconv structure) are available also under Windows.<br>
 *
 * By default locale definitions are loaded from system locale.<br>
 * However, these codes may be punctually changed on demand and replaced.<br>
 * Changing locale codes using ad-hoc functions is limitated to current application and does not impact on system locale.<br>
 *
 *
 *
 * - currency symbol    gets/sets the default decimal point symbol getLocaleDecimalPoint()  GChangeLocaleCurrencyCode()  <br>
 * - monetary decimal point gets/sets the default decimal point symbol getLocaleDecimalPoint() GChangelocaleDecimalPoint() <br>
 * - monetary grouping separator gets/sets the default grouping separator symbol getLocaleGroupSep() GChangelocaleGroupSep() <br>
 * - monetary digit grouping    gets the grouping number when groups (thousands by default) are requested.
 * - monetary digit fractional number gets the default number of fractional digits for a monetary amount getLocaleMonetaryDecimal()<br>
 * - default currency position gets the default position of currency symbol (true : preceeds - false : follows)
 * - default sign position   gets the default position of sign symbol (true : preceeds - false : follows)
 *
 */

class ZLocale
{
public:
  ZLocale()=default;
  ~ZLocale()  {GCleanCurrencyCode();}
  void GCleanCurrencyCode(void);
  /**
 * @brief GGetLocaleCodes sets or resets locale dependent data.<br>
 *  To have a valid list of locale definitions to use as argument pLocale,
 * you may type at console level <locale -a>  <br>
 * Locale data is loaded transparently once, the first time a locale element is requested via a getLocalxxxx() routine.<br>
 * stored locale data memory is released using an atexit() defined function.
 *
 * @param pLocale a valid locale name as a const char* string
 */
  UST_Status_type GGetLocaleCodes(const char*pLocale="");
  UST_Status_type GChangeLocaleCurrencyCode(const utf8_t *pCurrency);
  void GChangelocaleDecimalPoint(const utf8_t* pChar);
  void GChangelocaleGroupSep(const utf8_t* pChar);
  int getLocaleGroupNumber(void);
  int getLocaleMonetaryDecimal(void);
  ZBool getLocaleCurrencyPosition(void);
  ZBool getLocaleSignPosition(void);



  template <class _Utf>
  _Utf
  getLocaleDecimalPoint(void)
  {
    if (!GLocaleInfo)
      GGetLocaleCodes();

    if (GDecimalPoint==nullptr)
      return (_Utf)'.';
    return (_Utf)*GDecimalPoint;
  }

  template <class _Utf>
  _Utf*
  getLocaleCurrencyCode(void)
  {
    if (!GLocaleInfo)
      GGetLocaleCodes();

    switch (sizeof(_Utf))
    {
    case sizeof(utf8_t):
      return (_Utf*)GCurrencyCode8;
    case sizeof(utf16_t):
      return (_Utf*)GCurrencyCode16;
    case sizeof(utf32_t):
      return (_Utf*)GCurrencyCode32;
    default:
      break;
    }
  }//getLocalCurrencyCode

  template <class _Utf>
  _Utf
  getLocaleGroupSeparator(void)
  {
    if (!GLocaleInfo)
      GLocaleInfo = localeconv();
    if (!GLocaleInfo->mon_thousands_sep)
    {
      if (!*GLocaleInfo->mon_thousands_sep)
        return (_Utf)__SPACE__;
      else
        return (_Utf)*GLocaleInfo->thousands_sep;
    }
    if (!(_Utf)*GLocaleInfo->mon_thousands_sep)
      return (_Utf)__SPACE__;
    return (_Utf)*GLocaleInfo->mon_thousands_sep;
  }



uint8_t _DecimalPointContent[4];     /* 4 bytes storage for decimal point value */
uint8_t _ThousandSepContent[4];      /* 4 bytes storage for thousand separator value */
uint8_t _CurrencySymbolContent[10];  /* 10 bytes storage for thousand separator value */

const utf8_t*GCurrencyCode=nullptr;
const utf8_t*GDecimalPoint=nullptr;

const utf8_t*GThousandSep=nullptr;

int     GGrouping=0;
int     GDecimalNb=0;

utf8_t*GCurrencyCode8=nullptr;
utf16_t*GCurrencyCode16=nullptr;
utf32_t*GCurrencyCode32=nullptr;
lconv *GLocaleInfo = nullptr;
};//ZLocale

#ifndef ZLOCALE_CPP
extern ZLocale _zlocale;
#endif


/*
void GCleanCurrencyCode(void);

UST_Status_type GGetLocaleCodes(const char *pLocale="");


UST_Status_type GChangeLocaleCurrencyCode(const utf8_t*pCurrency);

void GChangelocaleDecimalPoint(const utf8_t* pChar);
void GChangelocaleGroupSep(const utf8_t *pChar);




int getLocaleGroupNumber(void);
int getLocaleMonetaryDecimal(void);
ZBool getLocaleCurrencyPosition(void);
ZBool getLocaleSignPosition(void);
*/

#endif // ZLOCALE_H
