#ifndef UTFVARYINGSTRING_H
#define UTFVARYINGSTRING_H
/**

 */
#include <ztoolset/zlimit.h>
#include <stdarg.h>
#include <ztoolset/zatomicconvert.h>

#ifdef QT_CORE_LIB
#include <QString>
#endif

#include <ztoolset/utfvtemplatestring.h>  // must stay here in this position
#include <ztoolset/utfstrctx.h>     // for conversion context

class utf16VaryingString;
class utf32VaryingString;
class stringKey;
/**
 * @brief The utf8VaryingString class ZString object unicode utf8 encoded with varying length.
 */
class utf8VaryingString: public utfVaryingString<utf8_t>
{
protected:
    void _setToUtf8() {ZType=ZType_Utf8VaryingString;Charset=ZCHARSET_UTF8;}
public:

typedef utfVaryingString<utf8_t> _Base;
typedef utf8_t                  _UtfBase;

    utf8VaryingString() {_setToUtf8() ;}

    utf8VaryingString(const char* pIn) {_setToUtf8() ; fromChar(pIn);}
    utf8VaryingString(std::string& pIn) {_setToUtf8() ; fromStdString(pIn);}

    size_t strcount(UST_Status_type &pStatus)  /** Counts the effective number of utf8 characters : multi-character units counts for 1 - skipping BOM */
    {
        size_t wCount;
        pStatus= utf8CharCount(Data,&wCount,&Context);
        return wCount;
    }
    size_t strlen(void) {return utfStrlen<_UtfBase>(Data);}  /** counts the effective number of utf8_t char size : multi bytes char counts for x utf8_t char*/


    const char* toCString_Strait(void) {return (const char*)Data;}

    utf8VaryingString* fromLatin1(const char* pInString);
    utf8VaryingString* fromEncoding(const uint8_t* pInString,ZCharset_type pEncoding);
    utf16VaryingString* getCollationKey(void);

    ZDataBuffer*    getEncrypted(uint8_t* pKey,uint8_t* pVector);
    UST_Status_type getByChunk(const utf8_t *pInString,
                                const size_t pChunkSize);
    UST_Status_type fromUtf8(const utf8_t *pInString);

    utf8VaryingString& fromChar(const char* pInString) {_Base::strset((const utf8_t*)pInString); return *this;}
    /**
     * @brief fromUtf16 converts an utf16_t string pUtf16 into utf8 format and sets this as current string content.
     *  equivalent as strset, but with utf16 to utf8 conversion.
     * Warning: Character units of resulting string (utf8 format) may be greater than from input string (utf16 format) due to multi-character units character
     *
     * @param[in] pUtf16 an utf16_t string
     * @param[in] pByteSize size in bytes of the string to parse.<br>
     *             If set to -1 (default value), string length is computed until (utf8_t)'\0' endofstringmark.
     * @param[in] pLittleEndian: a pointer to a boolean :<br>
     *      - nullptr : endianness is requested to be current system endianness (see function is_little_endian() ).<br>
     *      - points to true  : input utf16 string is requested to be little endian<br>
     *      - points to false : input utf16 string is requested to be big endian<br>
     * @return UST_Status_type with a value of enum utfStatus_type.
     */
    UST_Status_type fromUtf16(const utf16_t* pInString,
                                 ZBool *plittleEndian=nullptr);

    UST_Status_type fromUtf32(const utf32_t* pInString,
                                 ZBool *plittleEndian=nullptr);

    UST_Status_type toUtf16(utf16VaryingString &pUtf16, ZBool *pEndian=nullptr);
    UST_Status_type toUtf32(utf32VaryingString &pUtf32,ZBool *pEndian=nullptr);

    ZDataBuffer&        toCString(ZDataBuffer& pZDB);
    const char*         toCChar();
    std::string         toStdString() {return std::string(toCChar());}

    utf8VaryingString&  fromStdString(std::string& pIn) {strset((const utf8_t*)pIn.c_str()); return *this;}



    utf8VaryingString & operator = (const char* pString) { return fromChar(pString);}
    utf8VaryingString & operator = (std::string&  pString) { return fromStdString(pString);}
    utf8VaryingString & operator += (const char* pString)
    {
        utf8VaryingString wIn(pString);
        add(wIn);
        return *this;
    }


    utf8VaryingString& operator += (utf8VaryingString& pIn) {  appendData(pIn); return *this;}

    bool operator == (const char* pIn) { return compareV<char>(pIn); }
    bool operator != (const char* pIn) { return !compareV<char>(pIn); }

#ifdef QT_CORE_LIB

    const utf8VaryingString & operator = (const QString pQString) { return fromQString(pQString);}

    const QString toQString() {return QString(toCString_Strait());}
    const utf8VaryingString & fromQString(const QString pQString) {_Base::strset(((const utf8_t*)pQString.toUtf8().data())); return *this;}

#endif

};

/**
 * @brief The utf16VaryingString class ZString object unicode utf16 encoded with varying length.<br>
 *          This class fully accepts ucs2 encoding.
 */
class utf16VaryingString: public utfVaryingString<utf16_t>
{
public:
typedef utfVaryingString<utf16_t> _Base;
typedef utf16_t                  _UtfBase;

    using _Base::operator [];
    using _Base::operator =;
    using _Base::operator +=;



    utf16VaryingString() {
                            ZType=ZType_Utf16VaryingString;
                            Charset=ZCHARSET_UTF16;
                            if (is_little_endian())
                                       Charset=(ZCharset_type)(int(Charset)|ZCHARSET_LITTLEENDIAN);
                         }

    ssize_t strcount(UST_Status_type &pStatus)  /** Counts the effective number of utf16 characters : multi-character units counts for 1 - skipping BOM */
    {
        size_t wUnitCount=0;
        pStatus= utf16CharCount(Data,&wUnitCount,&Context,nullptr);
        return wUnitCount;
    }
    size_t strlen(void) {return utfStrlen<_UtfBase>(Data);}  /** counts the number of utf16_t char units : multi bytes char counts for x utf16_t char  */

    ZDataBuffer& toCString(ZDataBuffer& pZDB);

    utf16VaryingString* fromLatin1(const char* pInString);
    /**
     * @brief utf16VaryingString::fromUtf8 converts an utf8 string pInString to utf16 and sets current object content with it.
     *In case of error, conversion context is set up with a detailed status : utfStrCtx::Status, an UST_Status_type
     * that may be decoded using decode_UST() routine.<br>
     *
     * Input string is tested for a possible heading BOM.<br>
     * If a BOM is found, it is checked to be the appropriate BOM for the requested encoding.
     * UST_IVBOM is returned in case BOM type does not correspond, and routine returns  without processing string any further.
     * Found BOM is available within utfStrCtx::BOM context field. see ustStrCtx::dump().
     *
     * @param pInString utf8 string to convert.
     * @return a UST_Status_type with values from utfStatus_type<br>
     * <b>UST_SUCCESS</b> conversion went OK. Endofstring mark has been found in input string.<br>
     * <b>UST_EMPTY</b> Input string is starting with endofmark.<br>
     *
     * <b>UST_NULLPTR</b> a required argument is null.<br>
     * <b>UST_STRNOENDMARK</b> endofstring mark ('\0') has not been found until maximum string length __STRING_MAX_LENGTH__.<br>
     * <b>UST_INVBOM</b> a BOM has been detected and does not correspond to requested encoding format see ZBOM_type.<br>
     *
     * @see utf8CharCount() utf8Decode() utf16Encode()
     */
    UST_Status_type fromUtf8(const utf8_t* pInString);
    UST_Status_type fromUtf16(const utf16_t *pInString, ZBool *plittleEndian=nullptr);
    UST_Status_type fromUtf32(const utf32_t* pInString, ZBool *plittleEndian=nullptr);
    UST_Status_type fromEncoding(const char* pInString,const char*pEncoding);
    utf8VaryingString *toUtf8(utf8VaryingString &pUtf8);
    utf32VaryingString *toUtf32(utf32VaryingString &pUtf32);

    utf16VaryingString& getLittleEndian(utf16VaryingString& pLittleEndian);
    utf16VaryingString& getBigEndian(utf16VaryingString& pBigEndian);

    bool operator == (const char* pIn) { return compareV<char>(pIn); }
    bool operator != (const char* pIn) { return !compareV<char>(pIn); }

};
/**
 * @brief The utf32VaryingString class ZString object unicode utf32 encoded with varying length.<br>
 *      This class fully accepts ucs4 encoding.
 *
 */
class utf32VaryingString: public utfVaryingString<utf32_t>
{
public:

typedef utfVaryingString<utf32_t> _Base;
typedef utf32_t                  _UtfBase;

    utf32VaryingString() {
                         ZType=ZType_Utf32VaryingString;
                         Charset=ZCHARSET_UTF32;
                         if (is_little_endian())
                                    Charset=(ZCharset_type)(int(Charset)|ZCHARSET_LITTLEENDIAN);
                         }

    ssize_t strcount(UST_Status_type &pStatus)  /** Counts the effective number of utf32 characters - skipping BOM */
        {
        size_t wCount;
        pStatus= utf32CharCount(Data,&wCount,&Context,nullptr);/* use current system default endianness */
        return wCount;
        }

    size_t strcount(void){return utfStrlen<_UtfBase>(Data);}        /** Counts the effective number of utf8 characters : multi-bytes character counts for 1 */
    size_t strlen(void) {return utfStrlen<_UtfBase>(Data);}  /** counts the effective number of utf8_t char size : multi bytes char counts for x utf8_t char*/

    ZDataBuffer *toCString(ZDataBuffer& pZDB);

    utf32VaryingString* fromLatin1(const char* pInString);

    UST_Status_type fromUtf8(const utf8_t* pInString);
    UST_Status_type fromUtf32(const utf32_t* pInString, ZBool *pEndian=nullptr);
    UST_Status_type fromUtf16(const utf16_t* pInString, ZBool *pEndian=nullptr);

    utf8VaryingString*  toUtf8(utf8VaryingString &pUtf8);
    utf16VaryingString* toUtf16(utf16VaryingString &pUtf16, ZBool *pEndian=nullptr);

    utf8VaryingString*  toUtf8withBOM(utf8VaryingString &pUtf8);
    utf16VaryingString* toUtf16WithBOM(utf16VaryingString &pUtf16, ZBool *pEndian=nullptr);
    /**
    * @brief utf32VaryingString::getLittleEndian returns a clone as utf32VaryingString with a certified little endian content
    * @param pLittleEndian an utf32VaryingString that will receive cloned data, converted to little endian if necessary.
    * @return utf32VaryingString with a certified big endian content
    */
    utf32VaryingString& getLittleEndian(utf32VaryingString& pLittleEndian);
    /**
     * @brief utf32VaryingString::getBigEndian  returns a clone as utf32VaryingString with a certified big endian content
     * @param pBigEndian an utf32VaryingString that will receive cloned data, converted to big endian if necessary.
     * @return utf32VaryingString with a certified big endian content
     */
    utf32VaryingString& getBigEndian(utf32VaryingString& pBigEndian);
    /**
     * @brief utf32VaryingString::getLittleEndianWBOM  returns a clone as utf32VaryingString with a certified little endian content.<br>
     *  String content is leaded with the appropriate BOM (Byte Order Mark), i. e. utf32 BOM little endian: 0xFEFF0000 .
     * @param pBigEndian an utf32VaryingString that will receive cloned data, converted to little endian if necessary.
     * @return utf32VaryingString with a certified little endian content : content is preceeded by a BOM
     */
    utf32VaryingString& getLittleEndianWBOM(utf32VaryingString& pLittleEndian);
    /**
     * @brief utf32VaryingString::getBigEndianWBOM  returns a clone as utf32VaryingString with a certified big endian content.<br>
     *  String content is leaded with the appropriate BOM (Byte Order Mark), i. e. utf32 BOM big endian: 0x0000FFFE .
     * @param pBigEndian an utf32VaryingString that will receive cloned data, converted to big endian if necessary.
     * @return utf32VaryingString with a certified big endian content : content is preceeded by a BOM
     */
    utf32VaryingString& getBigEndianWBOM(utf32VaryingString& pBigEndian);

    bool operator == (const char* pIn) { return compareV<char>(pIn); }
    bool operator != (const char* pIn) { return !compareV<char>(pIn); }
};


#endif // UTFVARYINGSTRING_H
