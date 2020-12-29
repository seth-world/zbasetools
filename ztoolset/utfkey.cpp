#ifndef UTFKEY_CPP
#define UTFKEY_CPP

#include <ztoolset/utfkey.h>

#include <ztoolset/zcharset.h>

#include <ztoolset/zdatabuffer.h>

void
utfKeyContent::setKeyData(utfKeyData_type pKeyData,size_t pKeyCount)
{

    allocate (KeyCount);
    memmove(KeyData,pKeyData,pKeyCount);
    return;
}

utfKeyData_type
utfKeyContent::allocate (ssize_t pCount)
{

    if (KeyData)
            {
            free(KeyData);
            }
    KeyData=(uint8_t* )malloc (pCount);
    if (!KeyData)
    {
        fprintf(stderr,"%s-Fatal error cannot allocate memory for utfKeyContent key content\n",_GET_FUNCTION_NAME_);
        abort();
    }
    return KeyData;
}
ZDataBuffer*
utfKeyContent::exportUVF(void)
{

    UVF_Size_type wKeyCount=reverseByteOrder_Conditional<UVF_Size_type>((UVF_Size_type)KeyCount);
    ZDataBuffer* wZDB=new ZDataBuffer(&wKeyCount,sizeof(wKeyCount));
    wZDB->appendData(KeyData,(size_t)KeyCount);
    return wZDB;
}
utfKeyContent&
utfKeyContent::importUVF(ZDataBuffer&pUVFValue)
{
    UVF_Size_type wKeyCount;
//    pUVFValue.moveOut<UVF_Size_type>(wKeyCount);
    wKeyCount = pUVFValue.moveTo<UVF_Size_type>();


    KeyCount=reverseByteOrder_Conditional<UVF_Size_type>((UVF_Size_type)wKeyCount);
    allocate((size_t)wKeyCount);
    setKeyData((utfKeyData_type)(pUVFValue.Data+sizeof(UVF_Size_type)),(size_t)wKeyCount);
    return *this;
}

utfKey::utfKey(icu::Locale* pLocale)
{
    if (Locale==nullptr)
        {
            Locale=new icu::Locale;
            Locale->getRoot();
            return;
        }
    Locale=pLocale;
}
void utfKey::setlocale(const char*pLanguage,const char*pCountry,const char*pVariant)
{
    if (Locale)
        delete Locale;
    if (pLanguage==nullptr)
        {
        Locale=new icu::Locale;
        Locale->getDefault();
        }
        else
        Locale= new icu::Locale(pLanguage,pCountry,pVariant);

    _newCollator();
}
utfKeyData_type
utfKey::_computeKey(void)
    {

    LastError=U_ZERO_ERROR;
    if (Col==nullptr)
            {
            Col = icu::Collator::createInstance(LastError);  // should be created with a icu::Locale object -> database will depend on
            if (testIcuFatal(_GET_FUNCTION_NAME_,LastError,true,"While instantiating icu::Collator object"))
                                                                                                abort();
            }
    int32_t wKeyCount=Col->getSortKey((const char16_t*)Data,(int32_t)getUnitCount(),nullptr,_BaseKey::KeyCount);

    _BaseKey::allocate((ssize_t)wKeyCount);

    wKeyCount=Col->getSortKey((const char16_t*)Data,(int32_t)getUnitCount(),_BaseKey::KeyData,_BaseKey::KeyCount);

    return _BaseKey::KeyData;
    }
void utfKey::_newCollator(void)
{

    if (Col)
                delete Col;
    if (Locale==nullptr)
        Col = icu::Collator::createInstance(LastError);
        else
        Col = icu::Collator::createInstance(*Locale,LastError);  // should be created with a icu::Locale object -> database will depend on
    if (testIcuFatal(_GET_FUNCTION_NAME_,LastError,true,"While instantiating icu::Collator object"))
                                                                                        abort();
    return ;
}
bool utfKey::setNormalization (bool pOnOff)
{
    if (pOnOff)
        Col->setAttribute(UCOL_NORMALIZATION_MODE,UCOL_ON,LastError);
    else
        Col->setAttribute(UCOL_NORMALIZATION_MODE,UCOL_OFF,LastError);
    return testIcuError(_GET_FUNCTION_NAME_,LastError,true,"While setting icu Collator Normalization mode");
}

bool utfKey::setStrength (ZKeyStrength pStrength)
{
    Col->setAttribute(UCOL_STRENGTH,(UColAttributeValue)pStrength,LastError);
    return testIcuError(_GET_FUNCTION_NAME_,LastError,true,"While setting icu Collator Normalization mode");
}
/* sort order Uppercase first / lowercase first or default
*
*/
int utfKey::setUpperFirst(void)
{
    Col->setAttribute(UCOL_CASE_FIRST,UCOL_UPPER_FIRST,LastError);
    return testIcuError(_GET_FUNCTION_NAME_,LastError,true,"While setting icu Collator Uppercase first mode");
}
int utfKey::setLowerFirst (void)
{
    Col->setAttribute(UCOL_CASE_FIRST,UCOL_LOWER_FIRST,LastError);
    return testIcuError(_GET_FUNCTION_NAME_,LastError,true,"While setting icu Collator Lowercase first mode");
}
int utfKey::setCaseDefault (void)
{
    Col->setAttribute(UCOL_CASE_FIRST,UCOL_OFF,LastError);
    return testIcuError(_GET_FUNCTION_NAME_,LastError,true,"While setting icu Collator Case default mode");
}

#include <ztoolset/zexceptionmin.h>

UST_Status_type testIcuError(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...)
{
    if (pCode==U_ZERO_ERROR)
                return UST_SUCCESS;
char wMessage[cst_messagelen+1];
    va_list args_1;
    va_start (args_1, pMessage);
    vsnprintf(wMessage,cst_messagelen,pMessage,args_1);
    va_end(args_1);
    if (pCode<0){
    if (pWarningSignal)
        {
        ZException.getIcuError(pModule,
                               pCode,
                               Severity_Warning,
                               "ICU warning detected : %s ",wMessage);


        }
        return UST_CONVWARNING;
        }
// icu signals an error (U_Error_Code > U_ZERO_ERROR)

    ZException.getIcuError(pModule,
                           pCode,
                           Severity_Error,
                           "ICU error detected : %s ",pMessage);
    return UST_CONVERROR;
}

UST_Status_type testIcuFatal(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...)
{
    if (pCode==U_ZERO_ERROR)
                return 0;
char wMessage[cst_messagelen+1];
    va_list args_1;
    va_start (args_1, pMessage);
    vsnprintf(wMessage,cst_messagelen,pMessage,args_1);
    va_end(args_1);
    if ((pCode<0)&&(pWarningSignal))
        {
        ZException.getIcuError(pModule,
                               pCode,
                               Severity_Warning,
                               "ICU warning detected : %s ",wMessage);

        return 1;
        }
    ZException.getIcuError(pModule,
                           pCode,
                           Severity_Fatal,
                           "ICU fatal error detected : %s ",pMessage);
    return -1;
}

ZStatus ztestIcuError(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...)
{
char wMessage[cst_messagelen+1];
    va_list args_1;
    va_start (args_1, pMessage);
    vsnprintf(wMessage,cst_messagelen,pMessage,args_1);
    va_end(args_1);
    UST_Status_type wSt=testIcuError(_GET_FUNCTION_NAME_,pCode,true,wMessage);
    if (wSt== UST_SUCCESS)
            return ZS_SUCCESS;
    if (wSt > UST_SUCCESS)
            return ZS_ICUWARNING;
    return ZS_ICUERROR;
}
ZStatus ztestIcuFatal(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...)
{
char wMessage[cst_messagelen+1];
    va_list args_1;
    va_start (args_1, pMessage);
    vsnprintf(wMessage,cst_messagelen,pMessage,args_1);
    va_end(args_1);
    UST_Status_type wSt=testIcuError(_GET_FUNCTION_NAME_,pCode,true,wMessage);
    if (wSt== UST_SUCCESS)
            return ZS_SUCCESS;
    if (wSt > UST_SUCCESS)
            return ZS_ICUWARNING;
    return ZS_ICUERROR;
}

icu::Collator::ECollationStrength ZKSToIcuStrength (ZKeyStrength pStrength)
{
  switch (pStrength)
  {
  case  ZKS_PRIMARY:
          return icu::Collator::PRIMARY;
  case  ZKS_SECONDARY:
          return icu::Collator::SECONDARY;
  case  ZKS_TERTIARY:
          return icu::Collator::TERTIARY;
  case  ZKS_QUATENARY:
          return icu::Collator::QUATERNARY;
  case  ZKS_IDENTICAL:
          return icu::Collator::IDENTICAL;
  default:
          return icu::Collator::PRIMARY;
  }
}//ZKSToIcuStrength

#endif // UTFKEY_CPP

