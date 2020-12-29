#ifndef UTFCONVERTER_H
#define UTFCONVERTER_H
#include <zconfig.h>
#include <ztoolset/zicuconverter.h>



/**
 * @brief The utf8Converter class  converter object class using utf8 as main converter (to or from).<br>
 * This object is used to convert to or from utf8 any available, valid encoding.<br>
 * @warning
 * Converting unicode (utf8, ut16, utf32) from one to each other is a facility embedded into zsystembase utf string objects :
 * code is directly embedded, faster executed, and does not need to use this kind of external object, that may generate overhead resource consumption,
 * even if it may be sucessfully done.<br>
 *
 * <b>Object constructor</b><br>
 * <b>pWarningSignal</b> : When this constructor parameter is set to true, possible warnings will be reported during conversion.
 * To this purpose, a ZException object will be created, containing the warning description.<br>
 * By default, this parameter is set to false (no warning reporting).
 */
class utf8Converter: zicuConverter
{
public:
    utf8Converter(ZBool pWarningSignal=false)
    {
    
        if (zis_Error(zicuConverter::setup("UTF8",pWarningSignal)))
                    _ABORT_;
    }

    ZStatus toEncoding (char** pOutCharStr,size_t* pOutCount,
                        utf8_t* putf8String,size_t pInByteSize,
                        const char* pExtEncoding=nullptr)
    {
        return zicuConverter::toEncoding<utf8_t>(pOutCharStr,pOutCount,
                                                 putf8String,pInByteSize,
                                                 pExtEncoding);
    }

    ZStatus fromEncoding (utf8_t** putf8String,size_t* pOutSize,
                          const char* pInString,size_t pInSize,
                          const char* pExtEncoding=nullptr)
    {

        return zicuConverter::fromEncoding<utf8_t> (putf8String,pOutSize,
                                                    pInString,pInSize,
                                                    pExtEncoding);
    }
};
/**
 * @brief The utf16Converter class converter object class using utf16 as main converter (to or from).<br>
 * This object is used to convert to or from utf16 any available, valid encoding.<br>
 * @warning
 * Converting unicode (utf8, ut16, utf32) from one to each other is a facility embedded into zsystembase utf string objects :
 * code is directly embedded, faster executed, and does not need to use this kind of external object, that may generate overhead resource consumption,
 * even if it may be sucessfully done.<br>
 *
 * <b>Object constructor</b><br>
 * <b>plittleEndian</b> set to true, format is requested to be little endian. Set to false, big endian is requested.<br>
 * If omitted (nullptr), then current system endianness is taken (big or little endian). This is the default.<br>
 * <b>pWarningSignal</b> : When this constructor parameter is set to true, possible warnings will be reported during conversion.
 * To this purpose, a ZException object will be created, containing the warning description.<br>
 * By default, this parameter is set to false (no warning reporting).
 */
class utf16Converter: zicuConverter
{
public:
    utf16Converter(ZBool pWarningSignal=false,const ZBool*plittleEndian=nullptr)
    {
        ZBool wlittleEndian= is_little_endian();
        if (plittleEndian)
            {
            wlittleEndian=*plittleEndian;
            }
        if (wlittleEndian)
            {
            
            if (zis_Error(zicuConverter::setup("UTF16LE",pWarningSignal)))
                        _ABORT_;
            }
        else
            {
            
            if (zis_Error(zicuConverter::setup("UTF16BE",pWarningSignal)))
                        _ABORT_;
            }
    }

    ZStatus toEncoding (char** pOutCharStr,size_t* pOutCount,
                        utf16_t* putf16String,size_t pInByteSize,
                        const char* pExtEncoding=nullptr)
    {
        return zicuConverter::toEncoding<utf16_t>(pOutCharStr,pOutCount,
                                                 putf16String,pInByteSize,
                                                 pExtEncoding);
    }

    ZStatus fromEncoding (utf16_t** putf16String,size_t* pOutSize,
                          const char* pInString,size_t pInSize,
                          const char* pExtEncoding=nullptr)
    {

        return zicuConverter::fromEncoding<utf16_t> (putf16String,pOutSize,
                                                    pInString,pInSize,
                                                    pExtEncoding);
    }
};
/**
 * @brief The utf16LEConverter class convenient explicit Little Endian declaration for utf16 converter
 */
class utf16LEConverter: public utf16Converter /** explicit request for little endian utf16 converter */
{
public:
    utf16LEConverter(ZBool pWarningSignal):
        utf16Converter(pWarningSignal,&ZTrue) /* ZTrue : ZBool constant defined in zlimit.h*/
            {}
};
/**
 * @brief The utf16BEConverter class convenient explicit Big Endian declaration for utf16 converter
 */
class utf16BEConverter: public utf16Converter
{
public:
    utf16BEConverter(ZBool pWarningSignal=false):
        utf16Converter(pWarningSignal,&ZFalse) /* ZFalse : ZBool constant defined in zlimit.h*/
            {}
};


/**
 * @brief The utf32Converter class converter object class using utf32 as main converter (to or from).<br>
 * This object is used to convert to or from utf32 any available, valid encoding.<br>
 * @warning
 * Converting unicode (utf8, ut16, utf32) from one to each other is a facility embedded into zsystembase utf string objects :
 * code is directly embedded, faster executed, and does not need to use this kind of external object, that may generate overhead resource consumption,
 * even if it may be sucessfully done.<br>
 *
 * <b>Object constructor</b><br>
 * <b>plittleEndian</b> set to true, format is requested to be little endian. Set to false, big endian is requested.<br>
 * If omitted (nullptr), then current system endianness is taken (big or little endian). This is the default.<br>
 * <b>pWarningSignal</b> : When this constructor parameter is set to true, possible warnings will be reported during conversion.
 * To this purpose, a ZException object will be created, containing the warning description.<br>
 * By default, this parameter is set to false (no warning reporting).
 */
class utf32Converter: public zicuConverter
{
public:
    utf32Converter(ZBool pWarningSignal=false,const ZBool*plittleEndian=nullptr)
    {
        ZBool wlittleEndian= is_little_endian();
        if (plittleEndian)
            {
            wlittleEndian=*plittleEndian;
            }
        if (wlittleEndian)
            {
            
            if (zis_Error(zicuConverter::setup("UTF32LE",pWarningSignal)))
                        _ABORT_;
            }
        else
            {
            
            if (zis_Error(zicuConverter::setup("UTF32BE",pWarningSignal)))
                        _ABORT_;
            }
    }

    ZStatus toEncoding (char** pOutCharStr,size_t* pOutCount,
                        utf32_t* putf32String,size_t pInByteSize,
                        const char* pExtEncoding=nullptr)
    {
        return zicuConverter::toEncoding<utf32_t>(pOutCharStr,pOutCount,
                                                 putf32String,pInByteSize,
                                                 pExtEncoding);
    }

    ZStatus fromEncoding (utf32_t** putf32String,size_t* pOutSize,
                          const char* pInString,size_t pInSize,
                          const char* pExtEncoding=nullptr)
    {

        return zicuConverter::fromEncoding<utf32_t> (putf32String,pOutSize,
                                                    pInString,pInSize,
                                                    pExtEncoding);
    }
};
/**
 * @brief The utf32LEConverter class convenient explicit Little Endian declaration for utf32 converter
 */
class utf32LEConverter: public utf32Converter
{
public:
    utf32LEConverter(ZBool pWarningSignal=false):
        utf32Converter(pWarningSignal,&ZTrue) /* ZTrue : ZBool constant defined in zlimit.h*/
        {}

};
/**
 * @brief The utf32BEConverter class convenient explicit Big Endian declaration for utf32 converter
 */
class utf32BEConverter: public utf32Converter
{
public:
    utf32BEConverter(ZBool pWarningSignal=false): utf32Converter(pWarningSignal,&ZFalse) /* ZFalse : ZBool constant defined in zlimit.h*/
        {}

};
#endif // UTFCONVERTER_H
