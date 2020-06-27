#ifndef UTFFIXEDSTRING_H
#define UTFFIXEDSTRING_H
/** @file utffixedstring.h This file contains class definitions of various derivations for utfTemplateString template.<br>
 * The objective is to have characterized string types, with fixed capacity, dedicated to unicode formats.
 *
 *  - utf8FixedString     Template for utf8_t based fixed string : container in character unit size is template parameter i. e. number of utf8_t
 *  - utf16FixedString    Template for utf16_t based fixed string : container in character unit size is template parameter i. e. number of utf16_t
 *  - utf32FixedString    Template for utf36_t based fixed string : container in character unit size is template parameter i. e. number of utf32_t
 *
 *  As they are themselves templates, methods are contained in this include file after classes definitions.
 *
 *  They allows conversion from other string encoding/formats.<br>
 *  Conversion in direction of other encoding/formats is NOT managed (and will stay not managed).
 *
 */

#include <zconfig.h>

#include <ztoolset/zcharset.h>

#include <ztoolset/utftemplatestring.h>

#include <ztoolset/utfsprintf.h>
//#include <stdint.h>

#ifdef QT_CORE_LIB
//#include <qglobal.h> // for uint

#include <qstring.h>
#include <qtextstream.h>
#include <qtextcodec.h>

#endif //QT_CORE_LIB

/* ============== utf fixed string classes definitions ============================= */

template <size_t _Sz>
class utf8FixedString : public utftemplateString<_Sz,utf8_t>
{
public:
    typedef utftemplateString<_Sz,utf8_t> _Base;
    typedef utf8_t                      _UtfBase;
    utf8FixedString():utftemplateString<_Sz,utf8_t>(ZType_Utf8FixedString,ZCHARSET_UTF8) {}


    using _Base::operator=;
    using _Base::operator!=;
    using _Base::toUtf;

    using _Base::fromUtf;
    using _Base::fromUtfCount;

    const char*toCChar() const  {return (const char*)_Base::content;}
//    using _Base::fromCString;  No no no and no : char string could be any multi-byte format : do not mix that with unicode-

    /* Counts the effective number of utf8 characters : multi-bytes character counts for 1 . Skips utf8 BOM */

    ZStatus charCount (ssize_t& pCanonicalCount,utf8_t** pUtf8Effective=nullptr)
            {return utf8CharCount(_Base::content,pCanonicalCount,pUtf8Effective);}

    size_t strlen(void) {return utfStrlen<utf8_t>(_Base::content);} /** counts the total number of utf8_t chars units (bytes) composing the string */

    char* toCString_Strait(void) {return (char*)_Base::content;} /** utf8 to C String without possible conversion : one utf8_t equals one char */

    UST_Status_type fromUtf16(const utf16_t* pInString,
                              ZBool *plittleEndian=nullptr); /** converts an utf16 input string to utf8 string */
    UST_Status_type fromUtf32(const utf32_t* pInString,
                              ZBool *plittleEndian=nullptr); /** converts an utf32 input string to utf8 string */
    /**
     * @brief fromUtf8 gets an utf8 string in input and sets current object content with it after having controlled utf8 format,
     * eventually repaced ill formed characters by global replacement character, or stopped parsing if requested by utfStrCtx context action.
     * see changeReplacementCodepoint()
     * @param pInString utf8 string in input to set content with
     * @return an UST_Status_type
     */
    UST_Status_type fromUtf8(const utf8_t* pInString);
    /**
     * @brief utf8FixedString<_Sz>::getByChunk gets an utf8 string by pieces (chunk), managing successive calls.
     * a chunk may end in the middle of an utf8 character : this is managed.<br>
     * Context (utfStrCtx) must be reset once externally by calling routine before getting the first chunk of string.
     * @note getByChunk is only available in utf8 format.
     * @param pInString Chunk of string as input
     * @param pChunkSize  size of current chunk of characters to parse expressed in character units.
     * @return an UST_Status_type containing an utfStatus_type value.
     */
    UST_Status_type getByChunk(const utf8_t* pInChunk, const size_t pChunkSize);

    utf8FixedString<_Sz>* fromWArray(const wchar_t* pInWString);

    ZDataBuffer* toCString() ;

    ZDataBuffer* addBOM(void)
    {
        ZDataBuffer* wFS=new ZDataBuffer;
        wFS->setData(cst_utf8BOM,3);
        wFS->appendData(&_Base::content,_Base::ByteSize);
        return wFS;
    }


#ifdef QT_CORE_LIB

    QString toQString(void) {return QString::fromUtf8((const char*)_Base::content,_Base::getUnitCount());}
    utf8FixedString<_Sz>& fromQString(QString pQStr) {_Base::strset((utf8_t*)pQStr.toUtf8().data()); return *this;}
    utf8FixedString<_Sz>& appendQString(QString pQString) {_Base::add((utf8_t*)pQString.toUtf8().data()); return this;}

    utf8FixedString<_Sz>& operator = (QString pQStr) {return fromQString(pQStr);}

#endif  //QT_CORE_LIB

    void ContextDump(FILE* pOutput=stderr) {Context.dump(pOutput);}
    utfStrCtx Context;

}; //utf8FixedString


template <size_t _Sz>
class utf16FixedString : public utftemplateString<_Sz,utf16_t>
{
public:
typedef utftemplateString<_Sz,utf16_t> _Base;

    utf16FixedString<_Sz>():utftemplateString<_Sz,utf16_t>(ZType_Utf16FixedString,ZCHARSET_UTF16)
            {
            _Base::ZType = ZType_Utf16FixedString;
            if (is_little_endian())
                        _Base::Charset=ZCHARSET_UTF16LE;
                else
                        _Base::Charset=ZCHARSET_UTF16BE;
            }

    using _Base::operator=;
    using _Base::toUtf;

    using _Base::fromUtf;
    using _Base::fromUtfCount;



    ZStatus charCount  (ssize_t& pCanonicalCount,utf8_t** pUtf16Effective=nullptr)
                    {return utf16CharCount(_Base::content,pCanonicalCount,pUtf16Effective);}

    UST_Status_type fromUtf16(const utf16_t *pInString, ZBool *plittleEndian=nullptr);
    UST_Status_type fromUtf8(const utf8_t* pInString); /** converts an utf8 input string to utf16 string */
    UST_Status_type fromUtf32(const utf32_t* pInString,
                              ZBool *plittleEndian=nullptr); /** converts an utf32 input string to utf16 string */
    utf16FixedString<_Sz>* fromWArray(const wchar_t* pInWString);/** converts an wchar_t array to utf16 string */

    utf16FixedString<_Sz> getLittleEndian(void);
    utf16FixedString<_Sz> getBigEndian(void);

    ZDataBuffer*    toCString(void);
    ZDataBuffer* addBOM(void)
    {
        ZDataBuffer* wFS=new ZDataBuffer;
        if (is_little_endian())
            wFS->setData(&cst_utf16BOMLE,sizeof(cst_utf16BOMLE));
        else
            wFS->setData(&cst_utf16BOMBE,sizeof(cst_utf16BOMBE));

        wFS->appendData(&_Base::content,_Base::ByteSize);
        return wFS;
    }

#ifdef QT_CORE_LIB
    utf16FixedString<_Sz>* fromQString(QString pQStr) {fromUtf((const utf16_t*)pQStr.utf16()); return this;}

    QString toQString(void) {return QString::fromUtf16((const char16_t*)_Base::toUtf(),_Base::getUnitCount());}
    utf8FixedString<_Sz>& operator = (QString pQStr) {return fromQString(pQStr);}
#endif//QT_CORE_LIB
    utfStrCtx Context;

};

template <size_t _Sz>
class utf32FixedString : public utftemplateString<_Sz,utf32_t>
{
public:
typedef utftemplateString<_Sz,utf32_t> _Base;
    utf32FixedString<_Sz>():utftemplateString<_Sz,utf32_t>(ZType_Utf32FixedString,ZCHARSET_UTF32)
            {if (is_little_endian())
                        _Base::Charset=ZCHARSET_UTF32LE;
                else
                        _Base::Charset=ZCHARSET_UTF32BE;
            }

    using _Base::operator=;
    using _Base::toUtf;

    using _Base::fromUtf;
    using _Base::fromUtfCount;

    ZStatus charCount  (ssize_t& pCanonicalCount,utf32_t** pUtf32Effective=nullptr)
                {return utf32CharCount(_Base::content,pCanonicalCount,pUtf32Effective);}

    UST_Status_type fromUtf32(const utf32_t *pInString, ZBool* plittleEndian=nullptr);
    UST_Status_type fromUtf8(const utf8_t* pString);
    UST_Status_type fromUtf16(const utf16_t* pString,
                              ZBool *plittleEndian=nullptr); /** converts an utf16 input string to utf32 string */
    utf32FixedString<_Sz>* fromWArray(const wchar_t* pInWString);/** converts an wchar_t array to utf32 string */

    ZDataBuffer* toCString(void) ;

    utf32FixedString<_Sz> getLittleEndian(void);
    utf32FixedString<_Sz> getBigEndian(void);
    ZDataBuffer* addBOM(void)
    {
        ZDataBuffer* wFS=new ZDataBuffer;
        if (is_little_endian())
            wFS->setData(&cst_utf32BOMLE,sizeof(cst_utf16BOMLE));
        else
            wFS->setData(&cst_utf32BOMBE,sizeof(cst_utf16BOMBE));

        wFS->appendData(&_Base::content,_Base::ByteSize);
        return wFS;
    }
#ifdef QT_CORE_LIB
    utf32FixedString<_Sz>* fromQString(QString pQStr) {fromUtf16((utf16_t*)pQStr.utf16()); return this;}

    QString toQString(void)
    {        //return QString::fromUcs4((const char32_t*)_Base::toUtf(),_Base::getUnitCount());
             return QString::fromUcs4((const uint*)_Base::toUtf());
    }
#endif//QT_CORE_LIB
    utfStrCtx Context;
};

/* ==================================== Methods ============================================ */
/* ------------------------------- utf8FixedString-------------------------------------------*/

//==================== utf8FixedString Conversion routines ==============================

/**
 * @brief utf8FixedString<_Sz>:::fromWString_PtrCount gets an array of wchar_t to set utf8_t string content.
 * @param pInWString    an array of wchar_t
 * @param pCount        number of characters to convert from pInWString \0 termination excluded
 * @return
 */
template <size_t _Sz>
utf8FixedString<_Sz>*
utf8FixedString<_Sz>::fromWArray(const wchar_t* pInWString)
{
_MODULEINIT_
    if (!pInWString)
        {
        fprintf(stderr,
                "%s-E-NULLPTR wchar_t string pInWString is nullptr.\n",
                _GET_FUNCTION_NAME_);
        _RETURN_ ZS_NULLPTR;
        }
    _Base::clear();

    if (sizeof(wchar_t)==sizeof(utf32_t)) // wchar_t is supposed to be utf32_t (unix)
    {
        _RETURN_ fromUtf32((const utf32_t*)pInWString,nullptr);
    }
    if (sizeof(wchar_t)==sizeof(utf16_t)) // wchar_t is supposed to be utf16_t (windows)
    {
        _RETURN_ fromUtf16((const utf16_t*)pInWString,nullptr);
    }
// what is this wchar_t ?
    fprintf(stderr,"%s-F-INVSIZ Fatal error : size of wchar_t  <%ld> does not correspond to either utf16 <%ld> or utf32 <%ld>.\n",
            _GET_FUNCTION_NAME_,
            sizeof(wchar_t),
            sizeof(utf16_t),
            sizeof(utf32_t));
    _ABORT_

}// fromWString_Ptr

template <size_t _Sz>
ZDataBuffer*
utf8FixedString<_Sz>::toCString()
{
utf8_t* wPtr=_Base::content;
char *wStr=nullptr;

    Context.reset();
    Context.setStart(wPtr);

   if ((Context.Status=utf8StrToCharStr(&wStr,_Base::getCharset(), wPtr,&Context))<0)
    {
    fprintf (stderr,"%s>> conversion to C String failed at source character unit offset <%ld> status <0x%X> <%s>\n",
             _GET_FUNCTION_NAME_,
             Context.InUnitOffset,
             Context.Status,
             decode_UST(Context.Status));
    _free(wStr);
    return nullptr;
    }
    ZDataBuffer* wZDB=new ZDataBuffer();
    wZDB->appendData(wStr,Context.Strlen+1);
    _free(wStr);
    return wZDB;
}// utf8FixedString::toCString


template<size_t _Sz>
UST_Status_type
utf8FixedString<_Sz>::fromUtf8(const utf8_t* pInString)
{
_MODULEINIT_

utf32_t wCodePoint=0;

size_t wOutCount=0,wInCount=0;

utf8_t* wOutPtr= _Base::content;
size_t          wOutUnitOffset=0;
utfSCErr_struct wError;

const utf8_t* wInPtr;

    _Base::clear();
    Context.reset();
    Context.setStart(pInString);
    Context.setPosition(pInString);

    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
            {
            Context.Status=UST_EMPTY;
            _RETURN_ UST_EMPTY;
            }

    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }
    // Context.Strlen has the number of char units for the string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=ZBOM_UTF8))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any

    Context.utfSetPosition<utf8_t>(wInPtr);

    while (true)
    {
//        Context.setPosition( wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
        Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)_Base::content)-((uint8_t*)wOutPtr);

        Context.Status = utf8CharSize(wInPtr,&wInCount);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        Context.CharUnits+=wInCount;
        Context.CodePoint=0;
        wOutCount= wInCount;    // because we transfer utf8 to utf8 (excepted if illegal)

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf16_t)'\0'; // end of string mark
                                _RETURN_ Context.Status;
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
                {
                size_t wEscapeSize=0;
                ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                UST_Status_type wSt=utfEscapeUtf8<utf8_t>(wOutPtr,
                                                           wInPtr,
                                                           wInCount,
                                                           wRemaining,
                                                           Context.Action,
                                                           wEscapeSize);
                if (wSt==UST_TRUNCATED)
                                    return wSt;
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }
             wOutCount=0;           // compute char units of utf8Replacement
             while (utf8Replacement[wOutCount])
                            wOutCount++;

             Context.Substit++; // substitution char is given in wCodePoint by utf8Decode() routine
            }//if (Context.Status < 0)

        if ((wOutUnitOffset+wOutCount) >= (_Base::getUnitCount()-2))
                    {
                    Context.Status=UST_TRUNCATED;
                    break;
                    }
        if (Context.Status<0)
            for (size_t wi=0;wi<sizeof(utf8Replacement);wi++)  //
                        {
                        *wOutPtr = utf8Replacement[wi];
                        wOutPtr++;
                        wOutUnitOffset++;
                        }
        else
            for (size_t wi=0;wi<wInCount;wi++)  //
                        {
                        *wOutPtr = wInPtr[wi];
                        wOutPtr++;
                        wOutUnitOffset++;
                        }

        wInPtr+=wInCount;
        Context.Count ++;       // One character has been processed
    }// while true
    *wOutPtr=(utf16_t)'\0';
    _RETURN_ UST_SUCCESS;

} // utf8FixedString<_Sz>::fromUtf8

template <size_t _Sz>
UST_Status_type
utf8FixedString<_Sz>::getByChunk(const utf8_t* pInString,size_t pChunkSize)
{
_MODULEINIT_

utf32_t wCodePoint=0;

ssize_t wOutCount=0,wInCount=0;

utf8_t* wOutPtr= _Base::content;
size_t          wOutUnitOffset=0;
utfSCErr_struct wError;

utf8_t wSavedChunk[5] = {0,0,0,0,0};

size_t  wRemainingChunk;
const utf8_t* wInPtr;

    Context.resetIn();
    Context.setStart(pInString);
    Context.setPosition(pInString);

    Context.InByteOffset=0;
    Context.InUnitOffset=0;

    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
            {
            Context.Status=UST_EMPTY;
            _RETURN_ UST_EMPTY;
            }

     wRemainingChunk = pChunkSize;

    if (!Context.Strlen) // is that a warm start ?
    {// No ; initial call
    _Base::clear();
    // Context.Strlen has the number of char units for the string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(pChunkSize > 4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=ZBOM_UTF8))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any

    Context.utfSetPosition<utf8_t>(wInPtr);
    wOutPtr=_Base::content;
    }
    else // warm start
    {
    wInPtr=pInString;
    wOutPtr=Context.OutPtr ;  // restore out pointer to previous state
    wOutUnitOffset = Context.OutUnitOffset;

    if (Context.Status==UST_TRUNCATEDCHAR) // if last status shows previous chunk ended in middle of an utf8 char
        {
        size_t wi=0;
        for (; wi< Context.SavedChunkSize ;wi++)
                    wSavedChunk[wi] = static_cast<utf8_t*>(Context.SavedChunk)[wi];
        for (wi=Context.SavedChunkSize; wi< Context.SavedChunkFullSize ;wi++)
                {
                wSavedChunk[wi]=*wInPtr;
                wInPtr++;
                }
        wSavedChunk[wi]=0;

        Context.setPosition(wInPtr);
        Context.InByteOffset=Context.InPtr-Context.Start;
        Context.InUnitOffset = pInString-wInPtr;
//        Context.Status = utf8CharSizeChunk((const utf8_t*)wSavedChunk,&wInCount,Context.SavedChunkFullSize);

        Context.Status = utf8Decode((const utf8_t*)wSavedChunk,&wCodePoint,&wInCount,&Context.SavedChunkFullSize);

        wRemainingChunk -= Context.InUnitOffset;
        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;

        if (Context.Status<0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf8_t)'\0'; // end of string mark
                                _RETURN_ Context.Status;
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += (wInCount - Context.SavedChunkSize );                // just skip
                 Context.Count++;
//                 continue;   // continuing means entering in the main loop
                 goto utf8_getByChunk_Main;
                 }
             if ((wOutUnitOffset+wInCount) >= (_Base::getUnitCount()-2))
                         {
                         *wOutPtr=(utf8_t)'\0';
                         Context.Status=UST_TRUNCATED;
                         _RETURN_ Context.Status;
                         }
            for (size_t wi=0;wi<sizeof(utf8Replacement);wi++)  //
                        {
                        *wOutPtr = utf8Replacement[wi];
                        wOutPtr++;
                        wOutUnitOffset++;
                        Context.OutUnitOffset++;
                        }
            }//if (Context.Status<0)
        else
            for (size_t wi=0;wi<wInCount;wi++)  //
                        {
                        *wOutPtr = wInPtr[wi];
                        wOutPtr++;
                        wOutUnitOffset++;
                        Context.OutUnitOffset++;
                        }

        }// if (Context.Status==UST_TRUNCATEDCHAR)
    }// Warm start

utf8_getByChunk_Main:

    Context.Strlen+=pChunkSize;  // add chunk size to overall unit length
    /*  reset saved utf8 character */
    for (size_t wi=0;wi< sizeof(wSavedChunk);wi++)
                                wSavedChunk[wi]=0;
    Context.SavedChunkSize=0;
    Context.SavedChunkFullSize=0;

    _free(Context.SavedChunk);

    while (true)
    {
//        Context.setPosition( wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
 //       Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)_Base::content)-((uint8_t*)wOutPtr);

        Context.Status = utf8Decode(wInPtr,&wCodePoint,(size_t*)&wInCount,&wRemainingChunk);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        if (Context.Status==UST_TRUNCATEDCHAR)  // utf char is cut in middle of units
            {  // save it
            _free(Context.SavedChunk);

            Context.SavedChunk=calloc(5,sizeof(utf8_t));
            utf8_t* wPSavedChunk= (utf8_t*)Context.SavedChunk;  /* allocate an utf8 char max length */
            for(size_t wi=0;wi<5;wi++)                          /* set allocated space to 0 */
                        wPSavedChunk[wi]=0;
            for (size_t wi=0;wi < wRemainingChunk;wi++)
                        wPSavedChunk[wi]= wInPtr[wi];/* set context with units already acquired from chunk till end of chunk */
            Context.SavedChunkSize=wRemainingChunk; /* set context with size that belongs to current chunk */
            Context.SavedChunkFullSize=wInCount;    /* set context with total theorical length of character */

            _RETURN_ UST_TRUNCATEDCHAR;
            }
        wRemainingChunk -= wInCount;
        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf16_t)'\0'; // end of string mark
                                _RETURN_ Context.Status;
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             wOutCount=0;           // compute char units of utf8Replacement
             while (utf8Replacement[wOutCount])
                            wOutCount++;
             Context.Substit++; // substitution char is given in wCodePoint by utf8Decode() routine
            }//if (Context.Status < 0)

        if ((Context.OutUnitOffset+wOutCount) >= (_Base::getUnitCount()-2))
                    {
                    *wOutPtr=(utf8_t)'\0';
                    Context.Status=UST_TRUNCATED;
                    _RETURN_ Context.Status;
                    }
        if (Context.Status<0)
            for (size_t wi=0;wi<sizeof(utf8Replacement);wi++)  //
                        {
                        *wOutPtr = utf8Replacement[wi];
                        wOutPtr++;
                        wOutUnitOffset++;
                        Context.OutUnitOffset++;
                        }
        else
            for (size_t wi=0;wi<wInCount;wi++)  //
                        {
                        *wOutPtr = wInPtr[wi];
                        wOutPtr++;
                        wOutUnitOffset++;
                        Context.OutUnitOffset++;
                        }

        wInPtr+=wInCount;
        Context.Count ++;       // One character has been processed
    }// while true
    *wOutPtr=(utf8_t)'\0';
    _RETURN_ UST_SUCCESS;

} // utf8FixedString<_Sz>::getByChunck
/**
 * @brief utf8FixedString::fromUtf16
 * @param[in] pString       utf16 string to scan
 * @param[in] pLittleEndian endianness of utf16 format to decode. If omitted (nullptr) current system endianness is expected.
 * @return
 */
template <size_t _Sz>
UST_Status_type
utf8FixedString<_Sz>::fromUtf16(const utf16_t* pInString, ZBool *plittleEndian) /** converts an utf16 input string to utf8 string */
{
_MODULEINIT_
utf32_t wCodePoint=0;

ssize_t wOutCount=0,wInCount=0;

const utf16_t* wInPtr;
utf8_t wUtf8Char[5];

utf8_t* wOutPtr=_Base::content;
size_t wOutUnitOffset=0;
utfSCErr_struct wError;

    _Base::clear();
    Context.reset();
    Context.setStart(pInString);

    if (plittleEndian)
            {
            Context.EndianRequest = true;
            Context.LittleEndian = *plittleEndian;
            }
        else
            {
            Context.EndianRequest=false;
            Context.LittleEndian = is_little_endian();
            }

    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
        {
        Context.Status=UST_EMPTY;
        _RETURN_ UST_EMPTY;
        }
    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }
    ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF16_LE : ZBOM_UTF16_BE;

    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any


    while (true)
        {
        for (size_t wi=0;wi<sizeof(wUtf8Char);wi++)
                                        wUtf8Char[wi]=0;
//        Context.setPosition(wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
        Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)_Base::content)-((uint8_t*)wOutPtr);

        Context.Status = utf16Decode(wInPtr,&wCodePoint,(size_t*)&wInCount,plittleEndian);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf8_t)'\0'; // end of string mark
                                _RETURN_ Context.Status; // exit with nullptr return
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
                {
                size_t wEscapeSize=0;
                ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                UST_Status_type wSt=utfEscapeUtf16<utf8_t>(wOutPtr,
                                                           wInPtr,
                                                           wInCount,
                                                           wRemaining,
                                                           Context.Action,
                                                           wEscapeSize);
                if (wSt==UST_TRUNCATED)
                                    return wSt;
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }
/*  replacement is made within utf16Decode : wCodePoint contains replacement char */
            Context.Substit++;
            } // if (Context.Status < 0)

        Context.Status= utf8Encode(wUtf8Char,(size_t*)&wOutCount,wCodePoint,nullptr);
        if (Context.Status < 0)
            {
            Context.ErrFrom ++;
            if (Context.TraceFrom)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_FromUnicode;
                wError.UnitOffset = Context.OutUnitOffset;
                wError.ByteOffset = Context.OutByteOffset;
                wError.Ptr = Context.OutPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wOutCount;
                Context.ErrorQueue->push(wError);
               }
            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                            {
                            *wOutPtr=(utf8_t)'\0';   // end of string mark
                            _RETURN_ Context.Status; // exit with nullptr return
                            }
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;
                Context.Count++;
                continue;
                }
            //in any other error cases, use replacement character
//            for (wOutCount=0;utf8Replacement[wOutCount];wOutCount++)
//                                            wUtf8Char[wOutCount]=utf8Replacement[wOutCount];
            Context.Substit++;
            }//if (Context.Status < 0)

        if ((wOutUnitOffset+wOutCount) >= (_Base::getUnitCount()-2))
                    {
                    *wOutPtr=(utf8_t)'\0';
                    Context.Status=UST_TRUNCATED;
                    _RETURN_ Context.Status;
                    }
        for (long wi=0;wi<wOutCount;wi++)
                        {
                        *wOutPtr = wUtf8Char[wi] ;
                        wOutPtr++;
                        }
        wOutUnitOffset+= wOutCount;
        Context.Count ++; // One character has been processed
        wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units
    }// while true
    *wOutPtr=(utf8_t)'\0';
    _RETURN_ UST_SUCCESS;
} // fromUtf16


template <size_t _Sz>
/**
 * @brief utf8FixedString<_Sz>::fromUtf32 sets current object's utf8 content to an utf32_t string (pInString) after conversion.
 *
 * @param pInString
 * @param[in] plittleEndian endianness of utf32 format to decode. If omitted (nullptr) current system endianness is expected.
 * @return
 */
UST_Status_type
utf8FixedString<_Sz>::fromUtf32(const utf32_t* pInString, ZBool *plittleEndian) /** converts an utf32 input string to utf8 string */
{
_MODULEINIT_
utf32_t wCodePoint=0;
size_t wOutCount=0,wInCount=0;
utf8_t wUtf8Char[5];

utf8_t* wOutPtr=_Base::content;
utfSCErr_struct wError;

const utf32_t* wInPtr;

    _Base::clear();
    Context.reset();
    Context.setStart(pInString);
    Context.setPosition(pInString);


    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
            {
            Context.Status=UST_EMPTY;
            _RETURN_ UST_EMPTY;
            }

    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }

    if (plittleEndian)
            {
            Context.EndianRequest = true;
            Context.LittleEndian = *plittleEndian;
            }
            else
            {
            Context.EndianRequest=false;
            Context.LittleEndian = is_little_endian();
            }

    ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF32_LE : ZBOM_UTF32_BE;

    // Context.Strlen has the number of char units for the input string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any

    Context.utfSetPosition<utf32_t>(wInPtr);


    while (true)
    {
        for (size_t wi=0;wi<sizeof(wUtf8Char);wi++)
                                        wUtf8Char[wi]=0;
//        Context.setPosition( wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
        Context.OutByteOffset = ((uint8_t*)_Base::content)-((uint8_t*)wOutPtr);

        Context.Status = utf32Decode(wInPtr,&wCodePoint,&wInCount,plittleEndian);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;
        Context.CharUnits += wInCount;
        Context.CodePoint=wCodePoint;

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf8_t)'\0'; // end of string mark
                                _RETURN_ Context.Status;
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
                {
                size_t wEscapeSize=0;
                ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                UST_Status_type wSt=utfEscapeUtf32<utf8_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                           wInPtr,
                                                           wInCount,
                                                           wRemaining,
                                                           Context.Action,
                                                           wEscapeSize);
                if (wSt==UST_TRUNCATED)
                                    return wSt;
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }
/*  replacement is made within utf32Decode : wCodePoint contains replacement char */
                Context.Substit++;
            }// if (Context.Status < 0)

        Context.Status= utf8Encode(wUtf8Char,&wOutCount,wCodePoint,nullptr); // endianness is current system endianness
        if (Context.Status < 0)
            {
            Context.ErrFrom++;
            if (Context.TraceFrom)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_FromUnicode;
                wError.UnitOffset = Context.OutUnitOffset;
                wError.ByteOffset = Context.OutByteOffset;
                wError.Ptr = Context.OutPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wOutCount;
                Context.ErrorQueue->push(wError);
               }
            if ((Context.StopOnConvErr) ||(Context.Status <  UST_SEVERE))  // if error is severe
                            {
                            *wOutPtr=(utf8_t)'\0';   // end of string mark
                            _RETURN_ Context.Status;
                            }
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;
                Context.Count++;
                continue;
                }
            if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
               {
               size_t wEscapeSize=0;
               ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
               UST_Status_type wSt=utfEscapeUtf32<utf8_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                          wInPtr,
                                                          wInCount,
                                                          wRemaining,
                                                          Context.Action,
                                                          wEscapeSize);
               if (wSt==UST_TRUNCATED)
                                   return wSt;
               Context.OutUnitOffset += wEscapeSize;
               Context.Escaped++;
               Context.OutUnitOffset +=wEscapeSize;
               wOutPtr+=wEscapeSize;
               wInPtr+= wInCount;
               Context.Count++;
               continue;
               }
            Context.Substit++;
            //in any other error cases, use replacement character (set by utf8Encode)
            }//if (Context.Status < 0)

        if ((Context.OutUnitOffset+wOutCount) >= (_Base::getUnitCount()-2))
                    {
                    *wOutPtr=(utf8_t)'\0';
                    Context.Status=UST_TRUNCATED;
                    _RETURN_ Context.Status;
                    }
        for (long wi=0;wi<wOutCount;wi++)
                            {
                            *wOutPtr = wUtf8Char[wi] ; // in case of substitution wUtf8Char contains replacement char (set within utf8Encode)
                            wOutPtr++;
                            Context.OutUnitOffset++;
                            }
        Context.Count ++; // One character has been processed

        wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units
    }// while true
    *wOutPtr=(utf8_t)'\0';
    _RETURN_ UST_SUCCESS;

} // utf8FixedString<_Sz>::fromUtf32

//==================== utf16FixedString Conversion routines ==============================

/**
 * @brief utf8FixedString<_Sz>:::fromWArray gets an array of wchar_t to set utf16_t string content.
 * @param pInWString    an array of wchar_t
 * @param pCount        number of characters to convert from pInWString \0 termination excluded
 * @return
 */
template <size_t _Sz>
utf16FixedString<_Sz>*
utf16FixedString<_Sz>::fromWArray(const wchar_t* pInWString)
{
_MODULEINIT_
    if (!pInWString)
        {
        fprintf(stderr,
                "%s-E-NULLPTR wchar_t string pInWString is nullptr.\n",
                _GET_FUNCTION_NAME_);
        _RETURN_ ZS_NULLPTR;
        }
    _Base::clear();

    if (sizeof(wchar_t)==sizeof(utf32_t)) // wchar_t is supposed to be utf32_t (unix)
        {
        _RETURN_ fromUtf32((const utf32_t*)pInWString,nullptr);
        }
    if (sizeof(wchar_t)==sizeof(utf16_t)) // wchar_t is supposed to be utf16_t (windows)
        {
        size_t wSize=0;
        utf16_t* wPtr=_Base::content;
        while ((*pInWString)&&(wSize++<(_Base::getUnitCount()-2)))
                {
                *wPtr++=*pInWString++;
                }
        if (wSize >= (_Base::getUnitCount()-2))
            {

            fprintf(stderr,
                    "%s-W-CAPAOVFLW Utf16 fixed string capacity overflow : requested %ld while capacity is %ld.\n"
                    "              wchar_t array truncated at %ld character units.\n",
                    _GET_FUNCTION_NAME_,
                    wSize,
                    _Base::getUnitCount(),
                    wSize-1);
            }
        *wPtr++=(utf16_t)'\0';
        _RETURN_ this;
        }
// what is this wchar_t ?
    fprintf(stderr,"%s-F-INVSIZ Fatal error : size of wchar_t  <%ld> does not correspond to either utf16 <%ld> or utf32 <%ld>.\n",
            _GET_FUNCTION_NAME_,
            sizeof(wchar_t),
            sizeof(utf16_t),
            sizeof(utf32_t));
    _ABORT_

}// utf16FixedString<_Sz>::fromWArray

#ifdef __COMMENT__
#ifndef __UTF_REPLACEMENT__
#define __UTF_REPLACEMENT__
extern utf32_t  utf32Replacement ;
extern utf16_t  utf16Replacement[3] ;
extern utf8_t   utf8Replacement[5];
#endif// __UTF_REPLACEMENT__
#endif // __COMMENT__

template<size_t _Sz>
UST_Status_type
utf16FixedString<_Sz>::fromUtf16(const utf16_t* pInString,ZBool* plittleEndian)
{
_MODULEINIT_

utf32_t wCodePoint=0;

ssize_t wOutCount=0,wInCount=0;

utf16_t* wOutPtr= _Base::content;
size_t          wOutUnitOffset=0;
utfSCErr_struct wError;

const utf16_t* wInPtr;

    _Base::clear();
    Context.reset();
    Context.setStart(pInString);
    Context.setPosition(pInString);

    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
            {
            Context.Status=UST_EMPTY;
            _RETURN_ UST_EMPTY;
            }

    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }
    // Context.Strlen has the number of char units for the string
    if (plittleEndian)
            {
            Context.EndianRequest = true;
            Context.LittleEndian = *plittleEndian;
            }
            else
            {
            Context.EndianRequest=false;
            Context.LittleEndian = is_little_endian();
            }

    ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF16_LE : ZBOM_UTF16_BE;

    // Context.Strlen has the number of char units for the input string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected

    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any

    Context.utfSetPosition<utf16_t>(wInPtr);

    while (true)
    {
//        Context.setPosition( wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
        Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)_Base::content)-((uint8_t*)wOutPtr);

        Context.Status = utf16Decode(wInPtr,&wCodePoint,(size_t*)&wInCount,plittleEndian);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;
        wOutCount= wInCount;    // because we transfer utf16 to utf16

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf16_t)'\0'; // end of string mark
                                _RETURN_ Context.Status;
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
                {
                size_t wEscapeSize=0;
                ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                UST_Status_type wSt=utfEscapeUtf16<utf16_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                           wInPtr,
                                                           wInCount,
                                                           wRemaining,
                                                           Context.Action,
                                                           wEscapeSize);
                if (wSt==UST_TRUNCATED)
                                    return wSt;
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }
             wOutCount=0;           // compute char units of utf8Replacement
             while (utf16Replacement[wOutCount])
                            wOutCount++;

             Context.Substit++; // substitution char is given in wCodePoint by utf8Decode() routine
            }//if (Context.Status < 0)

        if ((wOutUnitOffset+wOutCount) >= (_Base::getUnitCount()-2))
                    {
                    *wOutPtr=(utf16_t)'\0';
                    Context.Status=UST_TRUNCATED;
                    _RETURN_ Context.Status;
                    }
        if (Context.Status<0)
            for (size_t wi=0;wi<sizeof(utf16Replacement);wi++)  //
                        {
                        *wOutPtr = utf16Replacement[wi];
                        wOutPtr++;
                        wOutUnitOffset++;
                        }
        else
            for (size_t wi=0;wi<wInCount;wi++)  //
                        {
                        *wOutPtr = wInPtr[wi];
                        wOutPtr++;
                        wOutUnitOffset++;
                        }

        wInPtr+=wInCount;
        Context.Count ++;       // One character has been processed
    }// while true
    *wOutPtr=(utf16_t)'\0';
    _RETURN_ UST_SUCCESS;

} // utf16FixedString<_Sz>::fromUtf16

template <size_t _Sz>
/**
 * @brief utf16FixedString<_Sz>::fromUtf8 sets current object utf16 content with an utf8 string in input(pInString)
 *  During processing utfStrCtx object Context is maintained updated.
 *
 *  utfStrCtx::StopOnConvErr field may be set to true (default value is false).
 *  if true, then conversion will stop when a conversion error occurs.
 *  if false, unicode replacement character is used in place of errored character.
 *
 *  utfStrCtx::Status is set with last error status
 *  Error cases :
 *  UST_ENDOFSTRING end of string mark has been found on input string. Normal termination
 *  UST_BOMUTF8     an utf8 BOM has been encountered. Not an error. BOM is skipped.
 *  UST_TRUNCATED   converted input string content is too large in character units to fit into fixed capacity.
 *                  input string is truncated accordingly.
 *
 *
 * Severe errors
 *  UST_NULLPTR     pInString is nullptr. Nothing is done on string content.
 *
 *  any error with UST_SEVEREMASK set
 * Invalid BOM encountered
 *  UST_IVBOMUTF8
 *  UST_IVBOMUTF16
 *  UST_IVBOMUTF32
 * UST_IVBOMUTF16BIG
 * UST_IVBOMUTF16LITTLE
 * UST_IVBOMUTF32BIG
 * UST_IVBOMUTF32LITTLE
 *
 *  Memory allocation error
 * UST_MEMERROR
 *
 *  Following errors may become severe errors if utfStrCtx::StopOnConvErr is set to true
 *  if set to false, then unicode replacement character is taken
 *
 *  generic errors
 *  UST_IVCHARSET   out of unicode codepoint range
 *
 * utf8 errors
 * UST_UTF8IVCONTBYTES  utf8 invalid continuation byte : a continuation byte was expected while another value has been found.
 * UST_MISSCONTBYTE     utf8 continuation byte is missing
 *
 * UST_IVUTF16SURR_1    invalid utf16 surrogate 1rst found
 * UST_IVUTF16SURR_2    invalid utf16 surrogate 2nd found
 *
 *
 * @param pInString utf8 string in input being converted to utf16
 * @return a pointer to current object.
 * In case of error, nullptr is returned and reasons are available in Context.
 */
UST_Status_type utf16FixedString<_Sz>::fromUtf8(const utf8_t* pInString)
{
_MODULEINIT_

utf32_t wCodePoint=0;

size_t wOutCount=0,wInCount=0;
utf16_t wUtf16Char[3]={0,0,0};

utf16_t* wOutPtr= _Base::content;
size_t          wOutUnitOffset=0;
utfSCErr_struct wError;

const utf8_t* wInPtr;

    _Base::clear();
    Context.reset();
    Context.setStart(pInString);
    Context.setPosition(pInString);

    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
            {
            Context.Status=UST_EMPTY;
            _RETURN_ UST_EMPTY;
            }

    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }
    // Context.Strlen has the number of char units for the string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=ZBOM_UTF8))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any

    Context.utfSetPosition<utf8_t>(wInPtr);

    while (true)
    {
//        Context.setPosition( wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
        Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)_Base::content)-((uint8_t*)wOutPtr);

        Context.Status = utf8Decode(wInPtr,&wCodePoint,&wInCount);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf16_t)'\0'; // end of string mark
                                _RETURN_ Context.Status;
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }

             Context.Substit++; // substitution char is given in wCodePoint by utf8Decode() routine
            }//if (Context.Status < 0)

        Context.Status= utf16Encode((utf16_t*)wUtf16Char,&wOutCount,wCodePoint,nullptr); // no endian
        if (Context.Status < 0)
            {
            Context.ErrFrom ++;
            if (Context.TraceFrom)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_FromUnicode;
                wError.UnitOffset = Context.OutUnitOffset;
                wError.ByteOffset = Context.OutByteOffset;
                wError.Ptr = Context.OutPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wOutCount;
                Context.ErrorQueue->push(wError);
               }
            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                            {
                            *wOutPtr=(utf16_t)'\0';   // end of string mark
                            _RETURN_ Context.Status;
                            }
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;
                Context.Count++;
                continue;
                }
            if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
               {
               size_t wEscapeSize=0;
               ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
               UST_Status_type wSt=utfEscapeUtf8<utf16_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                          wInPtr,
                                                          wInCount,
                                                          wRemaining,
                                                          Context.Action,
                                                          wEscapeSize);
               if (wSt==UST_TRUNCATED)
                                   return wSt;
               Context.OutUnitOffset += wEscapeSize;
               Context.Escaped++;
               Context.OutUnitOffset +=wEscapeSize;
               wOutPtr+=wEscapeSize;
               wInPtr+= wInCount;
               Context.Count++;
               continue;
               }

            /* In case of illegal character, wUtf16Char is loaded with utf16Replacement by utf16Encode() routine*/
            Context.Substit++;
            wOutCount=1;
            }// if (Context.Status < 0)

        if ((wOutUnitOffset+wOutCount) >= (_Base::getUnitCount()-2))
                    {
                    *wOutPtr=(utf16_t)'\0';
                    Context.Status=UST_TRUNCATED;
                    _RETURN_ Context.Status;
                    }

        *wOutPtr = wUtf16Char[0];
        wOutPtr++;
        wOutUnitOffset++;
        if (wOutCount==2)
                    {
                    *wOutPtr = wUtf16Char[1];
                    wOutPtr++;
                    wOutUnitOffset++;
                    }

        Context.Count ++;       // One character has been processed
        wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units
    }// while true
    *wOutPtr=(utf16_t)'\0';
    _RETURN_ UST_SUCCESS;

} // utf16FixedString<_Sz>::fromUtf8



template <size_t _Sz>
UST_Status_type
utf16FixedString<_Sz>::fromUtf32(const utf32_t* pInString,  ZBool *plittleEndian) /** converts an utf16 input string to utf8 string */
{
_MODULEINIT_
utf32_t wCodePoint=0;

ssize_t wOutCount=0,wInCount=0;

const utf32_t* wInPtr;
utf16_t wUtf16Char[3];

utf16_t* wOutPtr= _Base::content;
size_t wOutUnitOffset=0;
utfSCErr_struct wError;

    _Base::clear();
    Context.reset();
    Context.setStart(pInString);
    Context.setPosition(pInString);

    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
            {
            Context.Status=UST_EMPTY;
            _RETURN_ UST_EMPTY;
            }

    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }

    if (plittleEndian)
            {
            Context.EndianRequest = true;
            Context.LittleEndian = *plittleEndian;
            }
            else
            {
            Context.EndianRequest=false;
            Context.LittleEndian = is_little_endian();
            }

    ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF32_LE : ZBOM_UTF32_BE;

    // Context.Strlen has the number of char units for the input string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any

    Context.utfSetPosition<utf32_t>(wInPtr);

    while (true)
    {
//        Context.setPosition( wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
        Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)_Base::content)-((uint8_t*)wOutPtr);

        Context.Status = utf32Decode(wInPtr,&wCodePoint,(size_t*)&wInCount,plittleEndian);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf16_t)'\0'; // end of string mark
                                _RETURN_ Context.Status;
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
                {
                size_t wEscapeSize=0;
                ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                UST_Status_type wSt=utfEscapeUtf32<utf16_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                           wInPtr,
                                                           wInCount,
                                                           wRemaining,
                                                           Context.Action,
                                                           wEscapeSize);
                if (wSt==UST_TRUNCATED)
                                    return wSt;
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }
/*  replacement is made within utf8Decode : wCodePoint contains replacement char */
            Context.Substit++;
            }//if (Context.Status < 0)


        Context.Status= utf16Encode((utf16_t*)wUtf16Char,(size_t*)&wOutCount,wCodePoint,nullptr);// endianness is current system endianness
        if (Context.Status < 0)
            {
            Context.ErrFrom ++;
            if (Context.TraceFrom)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_FromUnicode;
                wError.UnitOffset = Context.OutUnitOffset;
                wError.ByteOffset = Context.OutByteOffset;
                wError.Ptr = Context.OutPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wOutCount;
                Context.ErrorQueue->push(wError);
               }
            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                            {
                            *wOutPtr=(utf16_t)'\0';   // end of string mark
                            _RETURN_ Context.Status;
                            }
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;
                Context.Count++;
                continue;
                }
            if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
               {
               size_t wEscapeSize=0;
               ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
               UST_Status_type wSt=utfEscapeUtf32<utf16_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                          wInPtr,
                                                          wInCount,
                                                          wRemaining,
                                                          Context.Action,
                                                          wEscapeSize);
               if (wSt==UST_TRUNCATED)
                                   return wSt;
               Context.OutUnitOffset += wEscapeSize;
               Context.Escaped++;
               Context.OutUnitOffset +=wEscapeSize;
               wOutPtr+=wEscapeSize;
               wInPtr+= wInCount;
               Context.Count++;
               continue;
               }
            Context.Substit++;
            }//if (Context.Status < 0)

        if ((wOutUnitOffset+wOutCount) >= (_Base::getUnitCount()-2))
                    {
                    *wOutPtr=(utf16_t)'\0';
                    Context.Status=UST_TRUNCATED;
                    _RETURN_ Context.Status;
                    }
        for (size_t wi=0;wi<wOutCount;wi++)
                    {
                    *wOutPtr=wUtf16Char[wi];// in case of substitution wUtf8Char contains replacement char (set within utf8Encode)
                    wOutPtr++;
                    }
        wOutUnitOffset+= wOutCount;
        Context.Count ++;
        wInPtr += wInCount;                // NB: in case of error utf8 count is correctly set

    }// while true
    *wOutPtr=(utf16_t)'\0';
    _RETURN_ UST_SUCCESS;

} // utf16FixedString<_Sz>::fromUtf32

/**
 * @brief utf16FixedString<_Sz>::getLittleEndian returns an utf16FixedString with a certified little endian content
 * @return
 */
template<size_t _Sz>
utf16FixedString<_Sz>
utf16FixedString<_Sz>::getLittleEndian(void)
{
    utf16FixedString<_Sz> wLittleEndian;
    if (is_little_endian())
            {
            wLittleEndian=*this;
            return wLittleEndian;
            }

    utf16_t* wPtr=_Base::content;
    utf16_t* wPtr1=wLittleEndian.getContentPtr();
    while (*wPtr)
            *wPtr1=forceReverseByteOrder<utf16_t>(*wPtr);
    return wLittleEndian;
}

/**
 * @brief utf16FixedString<_Sz>::getBigEndian returns an utf16FixedString with a certified big endian content
 * @return
 */
template<size_t _Sz>
utf16FixedString<_Sz> utf16FixedString<_Sz>::getBigEndian(void)
{
    utf16FixedString<_Sz> wBigEndian;
    if (!is_little_endian())
            {
            wBigEndian=*this;
            return wBigEndian;
            }

    utf16_t* wPtr=_Base::content;
    utf16_t* wPtr1=wBigEndian.getContentPtr();
    while (*wPtr)
            *wPtr1=forceReverseByteOrder<utf16_t>(*wPtr);
    return wBigEndian;
}


template <size_t _Sz>
ZDataBuffer*
utf16FixedString<_Sz>::toCString(void)
{
utf16_t* wPtr=_Base::content;
char *wStr=nullptr;

   if ((Context.Status=utf16StrToCharStr(&wStr,_Base::getCharset(), wPtr,&Context))<0)
    {
    fprintf (stderr,"%s>> conversion to C String failed at source character unit offset <%d> status <0x%X> <%s>\n",
             _GET_FUNCTION_NAME_,
             (int)(Context.InPtr-Context.Start),
             Context.Status,
             decode_UST(Context.Status));
    _free(wStr);
    return nullptr;
    }
    ZDataBuffer* wZDB=new ZDataBuffer();
    wZDB->setString(wStr);
    _free(wStr);
    return wZDB;
}// utf16FixedString::toCString

/** ------------------------------- utf32FixedString-------------------------------------------*/
/**
 * @brief utf32FixedString<_Sz>:::fromWArray gets an array of wchar_t to set utf16_t string content.
 * @param pInWString    an array of wchar_t
 * @param pCount        number of characters to convert from pInWString \0 termination excluded
 * @return
 */
template <size_t _Sz>
utf32FixedString<_Sz>*
utf32FixedString<_Sz>::fromWArray(const wchar_t* pInWString)
{
_MODULEINIT_
    if (!pInWString)
        {
        fprintf(stderr,
                "%s-E-NULLPTR wchar_t string pInWString is nullptr.\n",
                _GET_FUNCTION_NAME_);
        _RETURN_ ZS_NULLPTR;
        }
    _Base::clear();

    if (sizeof(wchar_t)==sizeof(utf16_t)) // wchar_t is supposed to be utf16_t (windows)
        {
        _RETURN_ fromUtf16((const utf32_t*)pInWString,nullptr);
        }
    if (sizeof(wchar_t)==sizeof(utf32_t)) // wchar_t is supposed to be utf32_t (unix)
        {
        size_t wSize=0;
        utf32_t* wPtr=_Base::content;
        while ((*pInWString)&&(wSize < (_Base::getUnitCount()-2)))
                {
                *wPtr=*pInWString;
                wPtr++;
                pInWString++;
                wSize++;
                }
        if (wSize >= (_Base::getUnitCount()-2))
            {

            fprintf(stderr,
                    "%s-W-CAPAOVFLW Utf32 fixed string capacity overflow : requested %ld while capacity is %ld.\n"
                    "              wchar_t array truncated at %ld character units.\n",
                    _GET_FUNCTION_NAME_,
                    wSize,
                    _Base::getUnitCount(),
                    wSize-1);
             }
        _RETURN_ this;
        }
// what is this wchar_t ?
    fprintf(stderr,"%s-F-INVSIZ Fatal error : size of wchar_t  <%ld> does not correspond to either utf16 <%ld> or utf32 <%ld>.\n",
            _GET_FUNCTION_NAME_,
            sizeof(wchar_t),
            sizeof(utf16_t),
            sizeof(utf32_t));
    _ABORT_

}// fromWArray

template<size_t _Sz>
UST_Status_type
utf32FixedString<_Sz>::fromUtf32(const utf32_t* pInString,ZBool* plittleEndian)
{
_MODULEINIT_

utf32_t wCodePoint=0;

ssize_t wOutCount=0,wInCount=0;

utf32_t* wOutPtr= _Base::content;
size_t          wOutUnitOffset=0;
utfSCErr_struct wError;

const utf32_t* wInPtr;

    _Base::clear();
    Context.reset();
    Context.setStart(pInString);
    Context.setPosition(pInString);

    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
            {
            Context.Status=UST_EMPTY;
            _RETURN_ UST_EMPTY;
            }

    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }
    // Context.Strlen has the number of char units for the string
    if (plittleEndian)
            {
            Context.EndianRequest = true;
            Context.LittleEndian = *plittleEndian;
            }
            else
            {
            Context.EndianRequest=false;
            Context.LittleEndian = is_little_endian(); // take system default
            }

    ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF32_LE : ZBOM_UTF32_BE;

    // Context.Strlen has the number of char units for the input string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected

    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any

    Context.utfSetPosition<utf32_t>(wInPtr);

    while (true)
    {
//        Context.setPosition( wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
        Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)_Base::content)-((uint8_t*)wOutPtr);

        Context.Status = utf32Decode(wInPtr,&wCodePoint,(size_t*)&wInCount,plittleEndian);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;
        wOutCount= wInCount;    // because we transfer utf16 to utf16 (excepted if illegal)

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }
             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf16_t)'\0'; // end of string mark
                                _RETURN_ Context.Status;
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
                {
                size_t wEscapeSize=0;
                ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                UST_Status_type wSt=utfEscapeUtf32<utf32_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                           wInPtr,
                                                           wInCount,
                                                           wRemaining,
                                                           Context.Action,
                                                           wEscapeSize);
                if (wSt==UST_TRUNCATED)
                                    return wSt;
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }
             Context.Substit++; // substitution char is given in wCodePoint by utf8Decode() routine
            }//if (Context.Status < 0)

        wOutCount=1;
        Context.Status= utf32Encode(&wCodePoint,wCodePoint,nullptr); // system current endianness is taken
        if (Context.Status < 0)
            {
            Context.ErrFrom ++;
            if (Context.TraceFrom)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_FromUnicode;
                wError.UnitOffset = Context.OutUnitOffset;
                wError.ByteOffset = Context.OutByteOffset;
                wError.Ptr = Context.OutPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wOutCount;
                Context.ErrorQueue->push(wError);
               }
            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                            {
                            *wOutPtr=(utf32_t)'\0';// end of string mark
                            _RETURN_ Context.Status; // exit with nullptr return
                            }
            //in any other error cases, use replacement character
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;
                Context.Count++;
                continue;
                }
            if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
               {
               size_t wEscapeSize=0;
               ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
               UST_Status_type wSt=utfEscapeUtf32<utf32_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                          wInPtr,
                                                          wInCount,
                                                          wRemaining,
                                                          Context.Action,
                                                          wEscapeSize);
               if (wSt==UST_TRUNCATED)
                                   return wSt;
               Context.OutUnitOffset += wEscapeSize;
               Context.Escaped++;
               Context.OutUnitOffset +=wEscapeSize;
               wOutPtr+=wEscapeSize;
               wInPtr+= wInCount;
               Context.Count++;
               continue;
               }
 //           wUtf32Char=utf32Replacement ;
            Context.Substit++;
            wOutCount=1;
            }

        if ((wOutUnitOffset+wOutCount) >= (_Base::getUnitCount()-2))
                    {
                    *wOutPtr=(utf32_t)'\0';
                    Context.Status=UST_TRUNCATED;
                    _RETURN_ Context.Status;
                    }
        *wOutPtr=wCodePoint;
        wOutPtr++;
        wOutUnitOffset++;

        wInPtr+=wInCount;
        Context.Count ++;       // One character has been processed
    }// while true
    *wOutPtr=(utf32_t)'\0';
    _RETURN_ UST_SUCCESS;

} // utf32FixedString<_Sz>::fromUtf32

/**
 * @brief utf32FixedString::fromUtf8 set current object content with given utf8 string pInString
 * @param[in] pString       utf8 string to parse
 * @return a pointer to current object or nullptr if any error occurred.
 */
template <size_t _Sz>
UST_Status_type
utf32FixedString<_Sz>::fromUtf8(const utf8_t* pInString) /** converts an utf16 input string to utf8 string */
{
_MODULEINIT_
utf32_t wCodePoint=0,wUtf32Char;

ssize_t wInCount=0,wOutCount=0;

utf32_t* wOutPtr= _Base::content;
size_t          wOutUnitOffset=0;
utfSCErr_struct wError;

const utf8_t* wInPtr;

    _Base::clear();
    Context.reset();
    Context.setStart(pInString);
    Context.setPosition(pInString);

    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
            {
            Context.Status=UST_EMPTY;
            _RETURN_ UST_EMPTY;
            }

    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ UST_STRNOENDMARK;
                    }

    // Context.Strlen has the number of char units for the input string
    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=ZBOM_UTF8))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any

    Context.utfSetPosition<utf8_t>(wInPtr);

    while (true)
    {
//        Context.setPosition( wInPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
        Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)_Base::content)-((uint8_t*)wOutPtr);

        Context.Status = utf8Decode(wInPtr,&wCodePoint,(size_t*)&wInCount);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        Context.CharUnits+=wInCount;
        Context.CodePoint=wCodePoint;

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }

             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf32_t)'\0'; // end of string mark
                                _RETURN_ UST_to_ZStatus(Context.Status);
                                }
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;                // just skip
                Context.Count++;
                continue;
                }
            if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
               {
               size_t wEscapeSize=0;
               ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
               UST_Status_type wSt=utfEscapeUtf8<utf32_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                          wInPtr,
                                                          wInCount,
                                                          wRemaining,
                                                          Context.Action,
                                                          wEscapeSize);
               if (wSt==UST_TRUNCATED)
                                   return wSt;
               Context.OutUnitOffset += wEscapeSize;
               Context.Escaped++;
               Context.OutUnitOffset +=wEscapeSize;
               wOutPtr+=wEscapeSize;
               wInPtr+= wInCount;
               Context.Count++;
               continue;
               }
/*  replacement is made within utf8Decode : wUtf32Char contains replacement char */
            Context.Substit++;
            }

        wOutCount=1;
        Context.Status= utf32Encode(&wUtf32Char,wCodePoint,nullptr); // system current endianness is taken
        if (Context.Status < 0)
            {
            Context.ErrFrom ++;
            if (Context.TraceFrom)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_FromUnicode;
                wError.UnitOffset = Context.OutUnitOffset;
                wError.ByteOffset = Context.OutByteOffset;
                wError.Ptr = Context.OutPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wOutCount;
                Context.ErrorQueue->push(wError);
               }
            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                            {
                            *wOutPtr=(utf32_t)'\0';// end of string mark
                            _RETURN_ Context.Status; // exit with nullptr return
                            }
            //in any other error cases, use replacement character
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;
                Context.Count++;
                continue;
                }
            if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
               {
               size_t wEscapeSize=0;
               ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
               UST_Status_type wSt=utfEscapeUtf8<utf32_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                          wInPtr,
                                                          wInCount,
                                                          wRemaining,
                                                          Context.Action,
                                                          wEscapeSize);
               if (wSt==UST_TRUNCATED)
                                   return wSt;
               Context.OutUnitOffset += wEscapeSize;
               Context.Escaped++;
               Context.OutUnitOffset +=wEscapeSize;
               wOutPtr+=wEscapeSize;
               wInPtr+= wInCount;
               Context.Count++;
               continue;
               }
 //           wUtf32Char=utf32Replacement ;
            Context.Substit++;
            wOutCount=1;
            }
        if ((wOutUnitOffset+wOutCount) >= (_Base::getUnitCount()-2))
                    {
                    Context.Status=UST_TRUNCATED;
                     break;
                    }

        *wOutPtr = wUtf32Char;
        wOutPtr++;
        wOutUnitOffset++;
        Context.Count ++;       // One character has been processed
        wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

    }// while true
    *wOutPtr=(utf32_t)'\0';
    _RETURN_ UST_SUCCESS;

} // utf32FixedString<_Sz>::fromUtf8

template <size_t _Sz>
UST_Status_type
utf32FixedString<_Sz>::fromUtf16(const utf16_t* pInString,
                                 ZBool *plittleEndian) /** converts an utf16 input string to utf8 string */
{
_MODULEINIT_
utf32_t wCodePoint=0;

size_t wOutCount=0,wInCount=0;
const utf16_t* wInPtr;

utf32_t* wOutPtr= _Base::content;
size_t wOutUnitOffset=0;
utfSCErr_struct wError;

    _Base::clear();
    Context.reset();
    Context.setStart(pInString);

    if (plittleEndian)
            {
            Context.EndianRequest = true;
            Context.LittleEndian = *plittleEndian;
            }
        else
            {
            Context.EndianRequest=false;
            Context.LittleEndian = is_little_endian();
            }
    if (!pInString)
            {
            Context.Status=UST_NULLPTR;
            _RETURN_ UST_NULLPTR;
            }
    if (!*pInString)
        {
        Context.Status=UST_EMPTY;
        _RETURN_ UST_EMPTY;
        }

    Context.Strlen=0;
    while ((pInString[Context.Strlen++])&&(Context.Strlen < __STRING_MAX_LENGTH__));
    if (Context.Strlen == __STRING_MAX_LENGTH__)
                    {
                    _RETURN_ ZS_STRNOENDMARK;
                    }

    ZBOM_type wBOM = Context.LittleEndian ? ZBOM_UTF32_LE : ZBOM_UTF32_BE;

    size_t wBOMSize=0;
    Context.BOM=detectBOM(pInString,(Context.Strlen>4)? 5 : Context.Strlen,wBOMSize);
    if ((Context.BOM!=ZBOM_NoBOM)&&(Context.BOM!=wBOM))
                                            {_RETURN_ UST_IVBOM;} // invalid BOM could have been detected
    wInPtr=pInString+wBOMSize;
    Context.setEffective(wInPtr); // set string pointer to character next BOM if any

    while (true)
    {
//        Context.setPosition( wPtr);
        Context.InPtr = (uint8_t*)wInPtr;
        Context.InUnitOffset = wInPtr-pInString;
        Context.InByteOffset = Context.InPtr -Context.Start;

        Context.OutPtr= (uint8_t*)wOutPtr;
        Context.OutUnitOffset = wOutUnitOffset;
        Context.OutByteOffset = ((uint8_t*)_Base::content)-((uint8_t*)wOutPtr);

        Context.Status = utf16Decode(wInPtr,&wCodePoint,&wInCount,plittleEndian);

        Context.Status = utf16Decode(pInString,&wCodePoint,&wInCount,plittleEndian);

        if (Context.Status==UST_ENDOFSTRING)
                                        break;

        Context.CharUnits+=wInCount;

        if (Context.Status < 0)
            {
            Context.ErrTo ++;
            if (Context.TraceTo)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_ToUnicode;
                wError.UnitOffset = Context.InUnitOffset;
                wError.ByteOffset = Context.InByteOffset;
                wError.Ptr = Context.InPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wInCount;
                Context.ErrorQueue->push(wError);
                }

             if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                                {
                                *wOutPtr=(utf32_t)'\0'; // end of string mark
                                _RETURN_ Context.Status; // exit with nullptr return
                                }
             if (Context.Action==ZCNV_Skip)
                 {
                 wInPtr += wInCount;                // just skip
                 Context.Count++;
                 continue;
                 }
             if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
                {
                size_t wEscapeSize=0;
                ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
                UST_Status_type wSt=utfEscapeUtf16<utf32_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                           wInPtr,
                                                           wInCount,
                                                           wRemaining,
                                                           Context.Action,
                                                           wEscapeSize);
                if (wSt==UST_TRUNCATED)
                                    return wSt;
                Context.OutUnitOffset += wEscapeSize;
                Context.Escaped++;
                Context.OutUnitOffset +=wEscapeSize;
                wOutPtr+=wEscapeSize;
                wInPtr+= wInCount;
                Context.Count++;
                continue;
                }
/*  replacement is made within utf16Decode : wCodePoint contains replacement char */
            Context.Substit++;
            }// if (Context.Status < 0)


        Context.Status= utf32Encode(&wCodePoint,wCodePoint,nullptr);
        if (Context.Status < 0)
            {
            Context.ErrFrom ++;
            if (Context.TraceFrom)
                {
                wError.Status = Context.Status;
                wError.Action=Context.Action;
                wError.ToFrom= SCErr_FromUnicode;
                wError.UnitOffset = Context.OutUnitOffset;
                wError.ByteOffset = Context.OutByteOffset;
                wError.Ptr = Context.OutPtr;
                wError.codePoint = wCodePoint;
                wError.Size=wOutCount;
                Context.ErrorQueue->push(wError);
               }
            if ((Context.StopOnConvErr) ||(Context.Status < UST_SEVERE))  // if error is severe
                            {
                            *wOutPtr=(utf32_t)'\0'; // end of string mark
                            _RETURN_ Context.Status; // exit with nullptr return
                            }
            if (Context.Action==ZCNV_Skip)
                {
                wInPtr += wInCount;
                Context.Count++;
                continue;
                }
            if ((Context.Action >= ZCNV_Escape_ICU)&&(Context.Action <= ZCNV_Escape_CSS2))
               {
               size_t wEscapeSize=0;
               ssize_t wRemaining = _Base::getUnitCount()-Context.OutUnitOffset;
               UST_Status_type wSt=utfEscapeUtf16<utf32_t>(wOutPtr,     // for utfEscapeUtfxx see utfsprint.h
                                                          wInPtr,
                                                          wInCount,
                                                          wRemaining,
                                                          Context.Action,
                                                          wEscapeSize);
               if (wSt==UST_TRUNCATED)
                                   return wSt;
               Context.OutUnitOffset += wEscapeSize;
               Context.Escaped++;
               Context.OutUnitOffset +=wEscapeSize;
               wOutPtr+=wEscapeSize;
               wInPtr+= wInCount;
               Context.Count++;
               continue;
               }
            Context.Substit++;
            }

        if ((Context.Strlen+1) >= (_Base::getUnitCount()-2))
                    {
                    Context.Status=UST_TRUNCATED;
                     break;
                    }

        Context.Strlen ++;
        *wOutPtr = wCodePoint;
        wOutPtr++;
        Context.Count ++;       // One character has been processed
        wInPtr += wInCount;  // NB: in case of error utf8 count is correctly set to try to skip errored character units

    }// while true
    *wOutPtr=(utf32_t)'\0';
    _RETURN_ UST_SUCCESS;
} // utf32FixedString<_Sz>::fromUtf16


/**
 * @brief utf32FixedString<_Sz>::getLittleEndian returns a clone as utf32FixedString with a certified little endian content
 * @return
 */
template<size_t _Sz>
utf32FixedString<_Sz>
utf32FixedString<_Sz>::getLittleEndian(void)
{
    utf32FixedString<_Sz> wLittleEndian;
    if (is_little_endian())
            {
            wLittleEndian=*this;
            return wLittleEndian;
            }

    utf32_t* wPtr=_Base::getContentPtr();
    utf32_t* wPtr1=wLittleEndian.getContentPtr();
    while (*wPtr)
            *wPtr1=forceReverseByteOrder<utf32_t>(*wPtr);
    return wLittleEndian;
}

/**
 * @brief utf32FixedString<_Sz>::getBigEndian  returns a clone as utf32FixedString with a certified big endian content
 * @return
 */
template<size_t _Sz>
utf32FixedString<_Sz>
utf32FixedString<_Sz>::getBigEndian(void)
{
    utf32FixedString<_Sz> wBigEndian;
    if (!is_little_endian())
            {
            wBigEndian=*this;
            return wBigEndian;
            }

    utf32_t* wPtr=_Base::getContentPtr();
    utf32_t* wPtr1=wBigEndian.getContentPtr();
    while (*wPtr)
            *wPtr1=forceReverseByteOrder<utf32_t>(*wPtr);
    return wBigEndian;
}
template <size_t _Sz>
ZDataBuffer*
utf32FixedString<_Sz>::toCString(void)
{
utf32_t* wPtr=_Base::content;
char *wStr=nullptr;

   if ((Context.Status=utf32StrToCStr(&wStr,_Base::getCharset(), wPtr,&Context))<0)
    {
    fprintf (stderr,"%s>> conversion to C String failed at source character unit offset <%d> status <0x%X> <%s>\n",
             _GET_FUNCTION_NAME_,
             (int)(Context.getOffset()),
             Context.Status,
             decode_UST(Context.Status));
    _free(wStr);
    return nullptr;
    }
    ZDataBuffer* wZDB=new ZDataBuffer();
    wZDB->setString(wStr);
    _free(wStr);
    return wZDB;
}// utf32FixedString::toCString

#endif // UTFFIXEDSTRING_H
