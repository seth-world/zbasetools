#ifndef UTFVARYINGSTRING_H
#define UTFVARYINGSTRING_H
/**

 */
#include <config/zconfig.h>
#include <ztoolset/zlimit.h>
#include <stdarg.h>
#include <ztoolset/zatomicconvert.h>
#include <ztoolset/utfutils.h>

#ifdef QT_CORE_LIB
#include <QString>
#endif

#include <ztoolset/utfvtemplatestring.h>  // must stay here in this position
#include <ztoolset/utfstrctx.h>     // for conversion context

#include <zcrypt/zcrypt.h> // for AES encrypting / decryptin

class utf16VaryingString;
class utf32VaryingString;
class stringKey;
/**
 * @brief The utf8VaryingString class ZString object unicode utf8 encoded with varying length.
 */
class utf8VaryingString: public utfVaryingString<utf8_t>
{
protected:
    void _setToUtf8(ZType_type pType=ZType_Utf8VaryingString)
          {
          ZType=pType;
          Charset=ZCHARSET_UTF8;
          Check=cst_ZCHECK;
          }
public:

typedef utfVaryingString<utf8_t> _Base;
typedef utf8_t                  _UtfBase;
    using _Base::operator [];
    using _Base::operator =;
    using _Base::operator +=;
    using _Base::operator ==;
    using _Base::operator !=;
    utf8VaryingString():_Base() {_setToUtf8(ZType_Utf8VaryingString);}

    utf8VaryingString(const utf8VaryingString& pIn) {_setToUtf8(ZType_Utf8VaryingString); strset(pIn.Data);}
    utf8VaryingString(const utf8VaryingString&& pIn) {_setToUtf8(ZType_Utf8VaryingString); strset(pIn.Data);}

    utf8VaryingString(const char* pIn) {_setToUtf8(ZType_Utf8VaryingString) ; fromChar(pIn);}
    utf8VaryingString(const utf8_t* pIn) {_setToUtf8(ZType_Utf8VaryingString) ; strset(pIn);}

    utf8VaryingString(std::string& pIn) {_setToUtf8() ; fromStdString(pIn);}
    utf8VaryingString(std::string&& pIn) {_setToUtf8() ; fromStdString(pIn);}

protected:
    utf8VaryingString(ZType_type pType):_Base() {_setToUtf8(pType);}
public:

    /** @brief strcount() Counts the effective number of utf8 characters : multi-character units counts for 1 - skipping BOM */
    size_t strcount(UST_Status_type &pStatus)
    {
        size_t wCount;
        pStatus= utf8CharCount(Data,&wCount,&Context);
        return wCount;
    }
    /** @brief strlen() Counts the effective number of utf8 character units : multi-character units counts for its number of bytes - skipping BOM */
    size_t strlen(void) const {return utfStrlen<_UtfBase>(Data);}  /** counts the effective number of utf8_t char size : multi bytes char counts for x utf8_t char*/

    utf8VaryingString& replace(const char* pToBeReplaced,const char* pReplace)
    {
      _Base::replace((const utf8_t*)pToBeReplaced,(const utf8_t*)pReplace);
      return *this;
    }


    const char* toCString_Strait(void) {return (const char*)Data;}

    utf8VaryingString* fromLatin1(const char* pInString);
    utf8VaryingString* fromEncoding(const uint8_t* pInString,ZCharset_type pEncoding);
    utf16VaryingString* getCollationKey(void);

    ZArray<utf8VaryingString> strtok(const utf8_t* pSeparator);

    ZDataBuffer*    getEncrypted(uint8_t* pKey,uint8_t* pVector);
    UST_Status_type getByChunk(const utf8_t *pInString,
                                const size_t pChunkSize);
    UST_Status_type fromUtf8(const utf8_t* pInString);

    utf8VaryingString& fromChar(const char* pInString) {_Base::strset((const utf8_t*)pInString); CheckData(); return *this;}
    utf8VaryingString& fromnChar(const char* pInString,size_t pSize){_Base::strnset((const utf8_t*)pInString,pSize); CheckData(); return *this;}
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

    UST_Status_type fromUtf16(const utf16VaryingString& pInString);

    UST_Status_type fromUtf32(const utf32_t* pInString,
                                 ZBool *plittleEndian=nullptr);

    UST_Status_type fromUtf32(const utf32VaryingString& pInString);

    UST_Status_type toUtf16(utf16VaryingString &pUtf16, ZBool *pEndian=nullptr);
    UST_Status_type toUtf32(utf32VaryingString &pUtf32,ZBool *pEndian=nullptr);

    ZDataBuffer&        toCString(ZDataBuffer& pZDB);
    const char*         toCChar() const;
    std::string         toStdString() {return std::string(toCChar());}

    utf8VaryingString&  fromStdString(std::string& pIn) {strset((const utf8_t*)pIn.c_str()); return *this;}


    int compare(const utf8VaryingString& pIn) const  { return utfVaryingString<utf8_t>::compare(pIn.Data);}
    utf8_t* strstr(const utf8VaryingString& pIn) const { return utfVaryingString<utf8_t>::strstr(pIn.Data);}
    int compareCase(const utf8VaryingString& pIn) const  { return utfVaryingString<utf8_t>::compareCase(pIn);}

    utf8VaryingString&  addConditionalOR(const utf8VaryingString& pIn)
                      {utfVaryingString<utf8_t>::addConditionalOR(pIn.Data); return *this;}


/**
 * @brief utfVaryingString::encodeB64 Encode the utfVaryingString content with Base 64 encoding method (OpenSSL)
 *  Encode the exact content length of utfVaryingString EXCLUDING termination character ('\0').
 * a termination character is added to resulting B64 encoded string.
 * @return  a new utfVaryingString with encoded data. Source string remains unchanged.
 */

    utf8VaryingString encodeB64(void) const;
/**
 * @brief utfVaryingString::decodeB64 decodes utfVaryingString content using Base 64 encoding facilities (OpenSSL).
 * @return a new utfVaryingString with decoded data. Source string remains unchanged.
 */
    utf8VaryingString decodeB64(void) const;

    /** @brief encryptAES256() Encrypts to a crypted ZDataBuffer current string content to AES256 according given Key and Vector */
    ZStatus encryptAES256 ( ZDataBuffer& pEncryptedZDB,
                            const ZCryptKeyAES256& pKey,
                            const ZCryptVectorAES256& pVector );

    /** @brief uncryptAES256() Uncrypts a ZDataBuffer crypted content according given Key and Vector and returns current clear text string  */
    ZStatus uncryptAES256 ( const ZDataBuffer& pEncryptedZDB,
                            const ZCryptKeyAES256& pKey,
                            const ZCryptVectorAES256& pVector);


//    utf8_t operator [] (const size_t pIdx) const  { if(pIdx>getUnitCount()) return 0; return (Data[pIdx]);}
    utf8VaryingString & operator = (const char* pString) {
      Check=cst_ZCHECK;
      return fromChar(pString);
    }
    utf8VaryingString & operator = (const utf8VaryingString& pString) {
      Check=cst_ZCHECK;
      _Base::_copyFrom(pString);
      return *this;
    }
    utf8VaryingString & operator = (const utf8VaryingString&& pString) {
      Check=cst_ZCHECK;
      _Base::_copyFrom(pString);
      return *this;
    }
    utf8VaryingString & operator = (std::string&  pString) {
      Check=cst_ZCHECK;
      return fromStdString(pString);
    }
    utf8VaryingString & operator += (const char* pString) {
        utf8VaryingString wIn(pString);
        add(wIn);
        return *this;
    }




    utf8VaryingString& operator += (const utf8VaryingString& pIn) {
      pIn._check();
      add(pIn);
      return *this;
    }
    utf8VaryingString& operator += (const utf8VaryingString&& pIn) {
      pIn._check();
      add(pIn);
      return *this;
    }

    bool operator == (const char* pIn) const { return compareV<char>(pIn)==0; }

    bool operator != (const char* pIn) { return compareV<char>(pIn); }

#ifdef QT_CORE_LIB

    QByteArray toQByteArray(void) const { return (QByteArray (toCChar())); }
    QString toQString(void) const     {return QString(toCChar());}

    const utf8VaryingString & operator = (const QString pQString) { return fromQString(pQString);}

    const utf8VaryingString & fromQString(const QString pQString)
    {
      strset((const utf8_t*)pQString.toUtf8().data());
      return *this;
    }

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
    using _Base::operator ==;
    using _Base::operator !=;


    utf16VaryingString() {setToUtf16();}

    utf16VaryingString(const utf16VaryingString& pIn):_Base(pIn) {setToUtf16(); strset(pIn.Data);}
    utf16VaryingString(const utf16VaryingString&& pIn):_Base(pIn) {setToUtf16(); strset(pIn.Data);}

    utf16VaryingString(const char* pIn) {setToUtf16() ; fromLatin1(pIn);}
    utf16VaryingString(const utf16_t* pIn) {setToUtf16() ; strset(pIn);}

    utf16VaryingString(std::string& pIn) {setToUtf16() ; fromStdString(pIn);}
    utf16VaryingString(std::string&& pIn) {setToUtf16() ; fromStdString(pIn);}

  void setToUtf16()
    {
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

    utf16VaryingString& fromLatin1(const char* pInString);
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
    utf8VaryingString toUtf8();
    utf32VaryingString toUtf32();

    utf16VaryingString getLittleEndian();
    utf16VaryingString getBigEndian();

#ifdef QT_CORE_LIB

    QByteArray toQByteArray(void) {
      utf8VaryingString wUtf8;
      wUtf8.fromUtf16(toUtf());
      return (QByteArray(wUtf8.toCChar()));
    }
    QString toQString(void) { return QString((const QChar *)toUtf()); }

    const utf16VaryingString &operator=(const QString pQString) {
      return fromQString(pQString);
    }

    //    const QString toQString() {return QString(toCString_Strait());}
    const utf16VaryingString &fromQString(const QString pQString)
    {
      fromUtf16((const utf16_t*)pQString.data());
      return *this;
    }

#endif


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

    using _Base::operator [];
    using _Base::operator =;
    using _Base::operator +=;
    using _Base::operator ==;
    using _Base::operator !=;


     utf32VaryingString() {setToUtf32();}

     utf32VaryingString(const utf32VaryingString& pIn):_Base(pIn) {setToUtf32(); strset(pIn.Data);}
     utf32VaryingString(const utf32VaryingString&& pIn):_Base(pIn) {setToUtf32(); strset(pIn.Data);}

     utf32VaryingString(const char* pIn) {setToUtf32() ; fromLatin1(pIn);}
     utf32VaryingString(const utf8_t* pIn) {setToUtf32() ; fromUtf8(pIn);}
     utf32VaryingString(const utf32_t* pIn) {setToUtf32() ; strset(pIn);}

     utf32VaryingString(std::string& pIn) {setToUtf32() ; fromStdString(pIn);}
     utf32VaryingString(std::string&& pIn) {setToUtf32() ; fromStdString(pIn);}

     void setToUtf32()
     {
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

    utf8VaryingString toUtf8();
    utf16VaryingString toUtf16( ZBool *pEndian=nullptr);

    utf8VaryingString*  toUtf8withBOM(utf8VaryingString &pUtf8);
    utf16VaryingString* toUtf16WithBOM(utf16VaryingString &pUtf16, ZBool *pEndian=nullptr);
    /**
    * @brief utf32VaryingString::getLittleEndian returns a clone as utf32VaryingString with a certified little endian content
    * @param pLittleEndian an utf32VaryingString that will receive cloned data, converted to little endian if necessary.
    * @return utf32VaryingString with a certified big endian content
    */
    utf32VaryingString getLittleEndian();
    /**
     * @brief utf32VaryingString::getBigEndian  returns a clone as utf32VaryingString with a certified big endian content
     * @param pBigEndian an utf32VaryingString that will receive cloned data, converted to big endian if necessary.
     * @return utf32VaryingString with a certified big endian content
     */
    utf32VaryingString getBigEndian();
    /**
     * @brief utf32VaryingString::getLittleEndianWBOM  returns a clone as utf32VaryingString with a certified little endian content.<br>
     *  String content is leaded with the appropriate BOM (Byte Order Mark), i. e. utf32 BOM little endian: 0xFEFF0000 .
     * @param pBigEndian an utf32VaryingString that will receive cloned data, converted to little endian if necessary.
     * @return utf32VaryingString with a certified little endian content : content is preceeded by a BOM
     */
    utf32VaryingString getLittleEndianWBOM();
    /**
     * @brief utf32VaryingString::getBigEndianWBOM  returns a clone as utf32VaryingString with a certified big endian content.<br>
     *  String content is leaded with the appropriate BOM (Byte Order Mark), i. e. utf32 BOM big endian: 0x0000FFFE .
     * @param pBigEndian an utf32VaryingString that will receive cloned data, converted to big endian if necessary.
     * @return utf32VaryingString with a certified big endian content : content is preceeded by a BOM
     */
    utf32VaryingString getBigEndianWBOM();

#ifdef QT_CORE_LIB

    QByteArray toQByteArray(void) {
      utf8VaryingString wUtf8;
      wUtf8.fromUtf32(toUtf());
      return (QByteArray(wUtf8.toCChar()));
    }
    QString toQString(void) {
      QString wQS;
      wQS.fromUcs4(toUtf());
      return wQS;
    }

    const utf32VaryingString & operator = (const QString pQString) { return fromQString(pQString);}

    //    const QString toQString() {return QString(toCString_Strait());}
    const utf32VaryingString & fromQString(const QString pQString)
    {
      fromUtf16((const utf16_t*)pQString.data());
      return *this;
    }
#endif


    bool operator == (const char* pIn) { return compareV<char>(pIn); }
    bool operator != (const char* pIn) { return !compareV<char>(pIn); }
};

utf8VaryingString operator +(const utf8VaryingString& pOne,const utf8VaryingString& pTwo);
utf8VaryingString operator +( utf8VaryingString& pOne,utf8VaryingString& pTwo);
utf8VaryingString operator +(const utf8VaryingString& pOne,const char* pTwo);
utf8VaryingString operator +(const char* pOne,const utf8VaryingString& pTwo);


#endif // UTFVARYINGSTRING_H
