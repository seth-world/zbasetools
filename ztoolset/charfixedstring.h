#ifndef CHARFIXEDSTRING_H
#define CHARFIXEDSTRING_H
/** charfixedstring.h
 *  this file contains class definitions  AND methods of various derivations for utfTemplateString template specialized for char.
 *
 *  charFixedString     Template for utf8_t based fixed string : container in character unit size is template parameter i. e. number of utf8_t
 *
 *  As they are themselves templates, methods are contains in this include file after classes definitions.
 *
 *  They allows convertion from utf formats.
 *
 *  Conversion to other formats are NOT managed (and will stay not managed).
 *
 *
 */


#include <zconfig.h>
#include <ztoolset/utftemplatestring.h>

template <size_t _Sz>
class charFixedString : public utftemplateString<_Sz,char>
{
public:
    typedef utftemplateString<_Sz,char> _Base;
    typedef char                      _UtfBase;

    charFixedString(ZCharset_type pCharset=ZCHARSET_SYSTEMDEFAULT):utftemplateString<_Sz,char>(ZType_CharFixedString,pCharset)
        { }

    using _Base::operator=;

    ZCharset_type getCharset(void) {return _Base::Charset;}

    /** Counts the effective number of utf8 characters : multi-bytes character counts for 1 . Skips utf8 BOM */

    ZStatus charCount (ssize_t& pCanonicalCount,utf8_t** pUtf8Effective=nullptr)
            {return utfStrlen<char>(_Base::content);}

    size_t strlen(void) {return utfStrlen<char>(_Base::content);} /** counts the total number of utf8_t chars units (bytes) composing the string */

    char* toCString_Strait(void) {return (char*)_Base::content;} /** utf8 to C String without possible conversion : one utf8_t equals one char */
    /**
     * @brief fromUtf8 converts an utf8 string pInString of length(in bytes) pInByteSize
     *  to this char string according its internal charset and sets current string content with.
     * @param pInString     utf8 string to convert
     * @param pInByteSize   length in bytes of utf8 input string or -1.
     * <\b>In this case, first endofstring mark will be searched to define input string length.
     * @return a pointer to current string object after conversion and load of string content, or nullptr if an error occurred.
     *  <\b>In this case, ZException is set with appropriate error message details
     */
    charFixedString<_Sz>* fromUtf8(const utf8_t* pInString, const ssize_t pInByteSize); /** converts an utf8 input string to char string */
    /**
     * @brief fromUtf16 converts an utf16 string pInString of length(in bytes) pInByteSize
     *  to this char string according its internal charset and sets current string content with.
     * @param pInString     utf16 string to convert
     * @param pInByteSize   length in bytes of utf16 input string or -1.
     * <\b>In this case, first endofstring mark will be searched to define input string length.
     * @param plittleEndian optional boolean to request explicitely utf16 little endian (if set to true)
     *  or utf16 big endian if not omitted and set to false.
     * if omitted, then current system endianess is taken as requested.
     * @return a pointer to current string object after conversion and load of string content, or nullptr if an error occurred.
     *  <\b>In this case, ZException is set with appropriate error message details
     */
    charFixedString<_Sz>* fromUtf16(const utf16_t* pInString,
                                    const ssize_t pInByteSize,
                                    bool* plittleEndian=nullptr); /** converts an utf16 input string to char string */

    /**
     * @brief fromUtf32 converts an utf32 string pInString of length(in bytes) pInByteSize
     *  to this char string according its internal charset and sets current string content with.
     * @param pInString     utf32 string to convert
     * @param pInByteSize   length in bytes of utf32 input string or -1.
     * <\b>In this case, first endofstring mark will be searched to define input string length.
     * @param plittleEndian optional boolean to request explicitely utf32 little endian (if set to true)
     *  or utf32 big endian if not omitted and set to false.
     * if omitted, then current system endianess is taken as requested.
     * @return a pointer to current string object after conversion and load of string content, or nullptr if an error occurred.
     *  <\b>In this case, ZException is set with appropriate error message details
     */
    charFixedString<_Sz>* fromUtf32(const utf32_t* pInString,
                                    const ssize_t pInByteSize,
                                    ZBool *plittleEndian=nullptr);

    void setWarningSignal(ZBool pWS) {WarningSignal=pWS;}
//    utf8FixedString& addConditionalOR (const char*pValue) {return (utf8FixedString&)_Base::addConditionalOR((const utf8_t*)pValue);} /** only for utf8 fixed string */
    utfStrCtx Context;
    ZBool       WarningSignal=false;
};

//================== Expanded methods===================================================
template <size_t _Sz>
charFixedString<_Sz>*
charFixedString<_Sz>::fromUtf8(const utf8_t* pInString,const ssize_t pInByteSize) /** converts an utf8 input string to char string */
{
    char* wString=nullptr;


    int wSt=utf8StrToCharStr(&wString,_Base::getCharset(), pInString,pInByteSize);
    if (wSt<0)
    {
        fprintf (stderr,"%s>> Error while converting utf8 string to char string. Error <%d> <%s>\n",
                 _GET_FUNCTION_NAME_,
                 wSt,
                 decode_UST(wSt));
        _free(wString);
        return nullptr;
    }
/*    if (Context.Substit)
        {
        fprintf (stderr,"%s>> Some substitutions occurred while converting utf8 string to char string. Number <%d>\n",
                 _GET_FUNCTION_NAME_,
                 Context.Substit);
        }*/
    char *wPtr1=wString,*wPtr=_Base::content;
    size_t wi=_Base::getUnitCount();
    while ((*wPtr1)&&wi--)
            {
            *wPtr++=*wPtr1++;
            }
    _free(wString);
    while (wi--)
            *wPtr++=0;
    return this;
} // fromUtf8

template <size_t _Sz>
charFixedString<_Sz>*
charFixedString<_Sz>::fromUtf16(const utf16_t* pInString,
                                const ssize_t pInByteSize,
                                bool* plittleEndian) /** converts an utf16 input string to char string */
{
    char* wString=nullptr;

    UST_Status_type wSt=utf16StrToCharStr(&wString,_Base::getCharset(), pInString,pInByteSize,plittleEndian);
    if (wSt < UST_SEVERE)
    {
        fprintf (stderr,"%s>> Error while converting utf16 string to char string. Error <%d> <%s>\n",
                 _GET_FUNCTION_NAME_,
                 wSt,
                 decode_UST(wSt));
        _free(wString);
        return nullptr;
    }
/*    if (Context.Substit)
    {
        fprintf (stderr,"%s>> Some substitutions occurred while converting utf16 string to char string. Number <%d>\n",
                 _GET_FUNCTION_NAME_,
                 Context.Substit);
    }*/
    char *wPtr1=wString,*wPtr=_Base::content;
    size_t wi=_Base::getUnitCount();
    while ((*wPtr1)&&wi--)
            {
            *wPtr++=*wPtr1++;
            }
    _free(wString);
    while (wi--)
            *wPtr++=0;
    return this;
} // fromUtf16
                        /* converts an utf32 input string to char content */
template <size_t _Sz>
charFixedString<_Sz>*
charFixedString<_Sz>::fromUtf32(const utf32_t* pInString,
                                const ssize_t pInByteSize,
                                ZBool* plittleEndian)
{

    char* wString=nullptr;

    UST_Status_type wSt=utf32StrToCharStr(&wString,_Base::getCharset(), pInString,pInByteSize,WarningSignal, plittleEndian);
    if (wSt<UST_SEVERE)
    {
        fprintf (stderr,"%s>> Error while converting utf32 string to char string. Error <%d> <%s>\n",
                 _GET_FUNCTION_NAME_,
                 wSt,
                 decode_UST(wSt));
        _free(wString);
        return  nullptr;
    }
 /*   if (Context.Substit)
    {
        fprintf (stderr,"%s>> Some substitutions occurred while converting utf32 string to char string. Number <%d>\n",
                 _GET_FUNCTION_NAME_,
                 Context.Substit);
    }*/
    char *wPtr1=wString,*wPtr=_Base::content;
    size_t wi=_Base::getUnitCount();
    while ((*wPtr1)&&wi--)
            {
            *wPtr++=*wPtr1++;
            }
    _free(wString);
    while (wi--)
            *wPtr++=0;
    return  this;
} // fromUtf32

#endif // CHARFIXEDSTRING_H
