#ifndef CHARVARYINGSTRING_H
#define CHARVARYINGSTRING_H
/**

 */
#include <ztoolset/zlimit.h>
#include <stdarg.h>
#include <ztoolset/zatomicconvert.h>

#ifdef QT_CORE_LIB
#include <QString>
#endif

#include <ztoolset/utfvtemplatestring.h>  // must stay here in this position
class utf8VaryingString;
class utf16VaryingString;
class utf32VaryingString;
/**
 * @brief The charVaryingString class a varying storage string based on char character unit with its specific encoding.
 */
class charVaryingString: public utfVaryingString<char>
{
public:

typedef utfVaryingString<char> _Base;
typedef char                  _UtfBase;

    charVaryingString(ZCharset_type pCharset=ZCHARSET_SYSTEMDEFAULT) {ZType=ZType_CharVaryingString;Charset=pCharset;}

    ssize_t strcount(void)  /**< Counts the effective number of char characters : multi-character units counts for 1 - skipping BOM */
    { return utfStrlen<_UtfBase>(Data);}
    size_t strlen(void) {return utfStrlen<_UtfBase>(Data);}  /**< counts the effective number of char unit size : multi bytes char counts for x utf8_t char*/


    const char* toCString(void) {return (const char*)Data;}

    ZDataBuffer* toLatin1(void);

    charVaryingString&  fromSytemDefault(void);
    charVaryingString&  fromLatin1(void);
    charVaryingString&  fromCharset(charVaryingString& pInString,ZCharset_type =ZCHARSET_SYSTEMDEFAULT);


    ZDataBuffer* toSortKey(void);
    ZDataBuffer* toCharset(ZCharset_type pCharset=ZCHARSET_SYSTEMDEFAULT);

//    ZDataBuffer& toCString(ZDataBuffer& pZDB);
    /** @namespace zbs
     * @brief fromUtf8 Tries to convert an utf16_t string pString into current object char format string -according its internal encoding-
     *  and sets this as current string content, after having allocated character units to make string fit.
     *
     *  A warning may be issued by underlying routines. This case is not considered as an error.<br>
     *  In positionning WarningSignal to true, a ZException is set with appropriate warning description details in case of warning.
     *  Warnings may be tested with ZException_min::getLastStatus() and ZException_min::getLastSeverity() methods of global object ZException.
     *
     * @param[in] pUtf32 an utf32_t string ending with (utf32_t)'\0' endofstring mark
     * @param[in] size in bytes of pUtf32 or -1 (default value).<br>
     * In this case, the first endofstring mark will be searched to set string length.
     * @param[in] pLittleEndian: a pointer to a boolean :<br>
     *      - nullptr : endianness is current system endianness (see function is_little_endian() ).<br>
     *      - points to true  : input utf32 string is little endian<br>
     *      - points to false : input utf32 string is big endian<br>
     * @return a pointer to current string object in case of succes or nullptr in case of failure.<br>
     *  In this last case, a ZException is set with appropriate error description.
     *  ZException_min::getLastStatus() and ZException_min::getLastSeverity() methods of global object ZException give the status and the severity of the possible warning/error.
     * @see charVaryingString::setWarningSignal()
     */
    charVaryingString* fromUtf8(const utf8_t *pUtf8, const ssize_t pByteSize=-1);
    /** @namespace zbs
     * @brief fromUtf16 Tries to convert an utf16_t string pString into current object char format string -according its internal encoding-
     *  and sets this as current string content, after having allocated character units to make string fit.
     *
     *  A warning may be issued by underlying routines. This case is not considered as an error.<br>
     *  In positionning WarningSignal to true, a ZException is set with appropriate warning description details in case of warning.
     *  Warnings may be tested with ZException_min::getLastStatus() and ZException_min::getLastSeverity() methods of global object ZException.
     *
     * @param[in] pUtf32 an utf32_t string ending with (utf32_t)'\0' endofstring mark
     * @param[in] size in bytes of pUtf32 or -1 (default value).<br>
     * In this case, the first endofstring mark will be searched to set string length.
     * @param[in] pLittleEndian: a pointer to a boolean :<br>
     *      - nullptr : endianness is current system endianness (see function is_little_endian() ).<br>
     *      - points to true  : input utf32 string is little endian<br>
     *      - points to false : input utf32 string is big endian<br>
     * @return a pointer to current string object in case of succes or nullptr in case of failure.<br>
     *  In this last case, a ZException is set with appropriate error description.
     *  ZException_min::getLastStatus() and ZException_min::getLastSeverity() methods of global object ZException give the status and the severity of the possible warning/error.
     * @see charVaryingString::setWarningSignal()
     */
    charVaryingString* fromUtf16(const utf16_t *pString,
                                 const ssize_t pByteSize,
                                 ZBool *plittleEndian=nullptr);
    /** @namespace zbs
     * @brief fromUtf32 Tries to convert an utf32_t string pUtf32 into current object char format string -according its internal encoding-
     *  and sets this as current string content, after having allocated character units to make string fit.
     *
     * A warning may be issued by underlying routines. This case is not considered as an error.<br>
     * In positionning WarningSignal to true, a ZException is set with appropriate warning description details in case of warning.
     *  Warnings may be tested with ZException_min::getLastStatus() and ZException_min::getLastSeverity() methods of global object ZException.
     *
     * @param[in] pUtf32 an utf32_t string ending with (utf32_t)'\0' endofstring mark
     * @param[in] size in bytes of pUtf32 or -1 (default value).<br>
     * In this case, the first endofstring mark will be searched to set string length.
     * @param[in] pLittleEndian: a pointer to a boolean :<br>
     *      - nullptr : endianness is current system endianness (see function is_little_endian() ).<br>
     *      - points to true  : input utf32 string is little endian<br>
     *      - points to false : input utf32 string is big endian<br>
     * @return a pointer to current string object in case of succes or nullptr in case of failure.<br>
     *  In this last case, a ZException is set with appropriate error description.
     *  ZException_min::getLastStatus() and ZException_min::getLastSeverity() methods of global object ZException give the status and the severity of the possible warning/error.
     * @see setWarningSignal()
     */
    charVaryingString *fromUtf32(const utf32_t* pInString,
                                 const ssize_t pByteSize=-1,
                                 ZBool *plittleEndian=nullptr);


    utf8VaryingString& toUtf8(utf8VaryingString &pUtf8);
    utf16VaryingString& toUtf16(utf16VaryingString &pUtf16, bool *plittleEndian=nullptr);
    utf32VaryingString& toUtf32(utf32VaryingString &pUtf32, bool *plittleEndian=nullptr);

    void setWarningSignal(ZBool pWS=false) {WarningSignal=pWS;}

    utfStrCtx Context;
    ZBool     WarningSignal=false;
};






#endif // CHARVARYINGSTRING_H
