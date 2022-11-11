#ifndef UTFKEY_H
#define UTFKEY_H
/**
    @file utfkey.h generates sort keys from ZStrings - uses icu facilities.

    </b>for Strength UCA and DUCET see http://unicode.org/reports/tr10/
 *
 *  </p>for a demo see http://demo.icu-project.org/icu-bin/collation.html#legend
 */
#include <ztoolset/zlimit.h>
#include <ztoolset/zerror.h>

#include <ztoolset/utfvaryingstring.h>

#include <ztoolset/zicu.h>

#include <unicode/locid.h>
#include <unicode/sortkey.h>
#include <unicode/coll.h>
#include <unicode/ucol.h>

#include <unicode/coll.h>

/**
 * @brief The ZKeyStrength enum defines the way sort key is built from a ZString
 *
 *  see icu documentation http://userguide.icu-project.org/collation/concepts
 */
enum ZKeyStrength : uint8_t
{
    ZKS_Nothing = 0,
    ZKS_PRIMARY = icu::Collator::PRIMARY,       /* get rid of accented chars. Space counts as 1 char > null char*/
    ZKS_SECONDARY= icu::Collator::SECONDARY,    /* accented char are sorted after */
    ZKS_TERTIARY = icu::Collator::TERTIARY,     /*  */
    ZKS_QUATENARY = icu::Collator::QUATERNARY,
    ZKS_IDENTICAL = icu::Collator::IDENTICAL,
    ZKS_DEFAULT   = icu::Collator::PRIMARY
} ;
/**
 * @brief ZKSToIcuStrength converts ZKeyStrength to internal icu ECollationStrength code
 * @param pStrength
 * @return
 */
icu::Collator::ECollationStrength ZKSToIcuStrength (ZKeyStrength pStrength);



class ZDataBuffer;
class utfKeyContent
{
public:
    utfKeyData_type     KeyData=nullptr;
    SortKey_Size_type   KeyCount=0;

    void setKeyData(utfKeyData_type pKeyData,size_t pKeyCount);
    utfKeyData_type allocate (ssize_t pCount);

    ZDataBuffer* exportUVF(void);
    utfKeyContent& importUVF(ZDataBuffer&pUVFValue);

    virtual inline bool operator > (utfKeyContent& pKeyOther)
    {
        return (strcmp ((const char*)KeyData,(const char*)pKeyOther.KeyData)>0);
    }
    virtual inline bool operator < (utfKeyContent& pKeyOther)
    {
        return (strcmp ((const char*)KeyData,(const char*)pKeyOther.KeyData)<0);
    }
    virtual inline bool operator == (utfKeyContent& pKeyOther)
    {
        return (strcmp ((const char*)KeyData,(const char*)pKeyOther.KeyData)==0);
    }

};

/**
 * @brief The utfKey class Utility to compute a collation key from a string.<br>
 * Collation key is made according a specific locale.<br>
 *
 * It stores input string within an utf16VaryingString
 * while key content itself is stored into a dedicated structure, utfKeyContent.<br>
 *
 * Case 1: string acquisition
 *
 * step 1. String data is first to be loaded into utf16 varying string using its internal built-in routines
 * - from utf16 (straight forward)
 * - fromUtf8
 * - fromUtf32
 * - Or from a converter, if charset does not correspond to pre-defined string formats : i. e, Latin-1, EBCDIC, SHIFT-JIS, etc...
 *
 * step 2. Then collation key is to be generated.<br>
 * step 3. Afterwards, key may be used to be used : compared, exported (UVF) etc.
 *
 * Case 2: from existing key
 *
 * step 1. Collation key content is imported using UVF format (import-export facility).<br>
 * step 2. After having loaded a collation key content to current key object, key is capable of being used.
 * However, in case of import, original string content is not available within key structure.
 *
 * @warning
 * - Changing locale will change possible order among the computed keys.<br>
 * - Additionally, as collation algorithms are based on ICU, changing ICU version
 * may also change possible order among computed keys.
 *
 * @note
 * - It is not possible to rebuild a string content from its collation key value.
 * - According what preceeds :
 *  + it is mandatory to rebuild key value(s) in case of locale change and/or icu version change
 *  + comparing to collation keys generated with different locales will induce sort order errors.
 *
 *
 */
class utfKey : public utf16VaryingString,utfKeyContent
{
public:
typedef     utf16VaryingString  _BaseString;
typedef     utfKeyContent       _BaseKey;

    using _BaseKey::exportUVF;
    using _BaseKey::importUVF;
    using _BaseKey::setKeyData;

    utfKey(icu::Locale* pLocale);

    ~utfKey()
    {
        if (Locale)
            delete Locale;
        if (Col)
            delete Col;
    }


    inline bool operator > (utfKey& pKeyOther)
    {
        return (strcmp ((const char*)_BaseKey::KeyData,(const char*)pKeyOther.KeyData)>0);
    }
    inline bool operator < (utfKey& pKeyOther)
    {
        return (strcmp ((const char*)_BaseKey::KeyData,(const char*)pKeyOther.KeyData)<0);
    }
    inline bool operator == (utfKey& pKeyOther)
    {
        return (strcmp ((const char*)_BaseKey::KeyData,(const char*)pKeyOther.KeyData)==0);
    }


    inline utfKey* fromUtf16(utf16_t *pInString)
                        {
                        _BaseString::strset(pInString);
                        return this;
                        }


    inline utfKey* fromLatin1(char *pInString)
                        {
// need to import from various ZCharset (Latin1,...)
                        if (_BaseString::fromLatin1(pInString).isEmpty())
                                                            {return  nullptr;}
                        return this;
                        }

    inline utfKey* fromUtf8(const utf8_t* pInString)
                        {
// need to import from various ZCharset (Latin1,...)
                        if (_BaseString::fromUtf8(pInString)!=ZS_SUCCESS)
                                                            {return  nullptr;}
                        return this;
                        }
    const utfKeyData_type getKeyData(ssize_t& pKeyCount) {pKeyCount=KeyCount; return KeyData;}

    utfKeyData_type _computeKey(void);

    void setlocale(const char*pLanguage=nullptr,const char*pCountry=nullptr,const char*pVariant=nullptr);
    
    void _newCollator(void);

    bool setNormalization (bool pOnOff=false);

    bool setStrength (ZKeyStrength pStrength);
   /* sort order Uppercase first / lowercase first or default
    *
    */
    int setUpperFirst (void);
    int setLowerFirst (void);
    int setCaseDefault(void);
    const char* decodeIcuLastError(void)
    {
      return u_errorName(LastError);
    }
    
    icu::Collator*      Col=nullptr;
    icu::Locale* Locale=nullptr;
    UErrorCode LastError=U_ZERO_ERROR;
    ZKeyStrength        Strength=ZKS_DEFAULT;
};

//------------------- Methods -----------------------------------------





#endif // UTFKEY_H
