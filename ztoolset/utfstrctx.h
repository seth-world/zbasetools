#ifndef UTFSTRCTX_H
#define UTFSTRCTX_H
/** @file utfstrctx.h used for string parsing processing : context,UST status, BOM types
  */
#include <config/zconfig.h>
#include <ztoolset/zlimit.h>
#include <ztoolset/zmem.h>

#include <cstdio>

#include <ztoolset/zstatus.h>

typedef int16_t UST_Status_type;

/**
 * @brief The utfStatus_type enum Internal Use only: status of current parsing operation
 *
 *  Positive values are not errors :
 *   - less than UST_SEVERE : severe errors
 *   - negative value greater than UST_SEVERE Error but may generate substitution character.
 *   - positive value : success or warning
 */
enum UST_Status: UST_Status_type {
    UST_Nothing           =   0,
    UST_SUCCESS           =  0x01,  //!< Successful operation
//    UST_WARNING           =  0x2000,
    /* Character decoding :
     * hereafter : may be warnings (if skip or substitute or escape)
     * or error (if stop on error): in this case must be Ored with UST_SEVERE
     */
    UST_STRINGOVERFLW     = -0x08FF, /*!< character is ill formed and no valid character has been found as well as no endofstring mark has been found.*/

    UST_ILLEGAL           = -0x0901, /*!<  Character is illegal (see UST_IVCODEPOINT)<br>
                                            For example, \x81\x2E is illegal in SJIS because \x2E is not a valid trail byte for the \x81 lead byte.
                                            Also, starting with Unicode 3.0.1, non-shortest byte sequences in UTF-8 (like \xC1\xA1 instead of \x61 for U+0061) are also illegal, not just irregular.
                                            The error code U_ILLEGAL_CHAR_FOUND will be set. */
    UST_UNASSIGNED        = -0x0A00, /*!< The code point is unassigned.<br>
                                            The error code U_INVALID_CHAR_FOUND will be set. */
    UST_IRREGULAR         = -0x0B00, /*!< The codepoint is not a regular sequence in the encoding.<br>
                                            For example, \xED\xA0\x80..\xED\xBF\xBF are irregular UTF-8 byte sequences for single surrogate code points.<br>
                                            The error code U_INVALID_CHAR_FOUND will be set. */
    UST_TRUNCATEDCHAR     = -0x0C00, /*!< Multi units character is truncated - character is incomplete (utf8 or utf16). */
    /* End Character decoding */

    UST_ENDOFSTRING       =  0x1F00,  //!< end of string mark encountered
    UST_STRNOENDMARK      = -0x4F00,  //!<  endofString mark ('\0') has not be found on string : search of this mark has been made until __STRING_MAX_LENGTH__ without success.
    UST_EMPTY             =  0x2F00,  //!< given input string is empty
//    UST_SUBSTIT           =  0x03, //!< one or more subtitutions have been made (see utfStrCtx - generally called 'Context')

    UST_SEVERE            = -0x4000, //!< Severe error limit: status value below UST_SEVERE are severe errors.

    UST_NULLPTR           = -0x4001,     //!< a nullptr pointer has been given as argument while argument is required

    UST_IVBOM             = -0x4002,   //!< invalid BOM encountered
    UST_IVBOMUTF8         = -0x400A,   //!< invalid utf8 BOM encountered
    UST_IVBOMUTF16        = -0x4012,   //!< invalid utf16 BOM encountered : first byte of utf16 bom detected but not second byte
    UST_IVBOMUTF32        = -0x4022,   //!< invalid utf32 BOM encountered

    UST_BOMUTF8           =  0x000A,    //!< utf8 BOM detected  : not an error - but not a character

    UST_IVBOMUTF16BE      = -0x4112,    //!< Invalid UTF16 BOM big endian encountered. Example : first byte of utf16 bom big endian detected but wrong second byte or unexpected BOM
    UST_IVBOMUTF16LE      = -0x4212,    //!< Invalid UTF16 BOM little endian encountered. Example :first byte of utf16 bom little endian detected but wrong second byte or unexpected BOM

    UST_BOMUTF16BE       =  0x0112,    //!< correct utf16 bom big endian detected : not an error
    UST_BOMUTF16LE       =  0x0212,    //!< correct utf16 bom little endian detected : not an error
    UST_BOMUTF16         =  0x0012,    //!< utf16 bom detected (generic)

    UST_IVBOMUTF32BE     = -0x4122,    //!< Invalid UTF32 BOM big endian encountered. Example : first byte of utf32 bom big endian detected but wrong second byte or unexpected BOM
    UST_IVBOMUTF32LE  = -0x4222,    //!< Invalid UTF32 BOM little endian encountered. Example :first byte of utf32 bom little endian detected but wrong second byte or unexpected BOM

    UST_BOMUTF32BE       =  0x0122,    //!< correct utf32 bom big endian detected : not an error - but not a character
    UST_BOMUTF32LE    =  0x0222,    //!< correct utf32 bom little endian detected : not an error - but not a character
    UST_BOMUTF32          =  0x0022,    //!< utf32 bom detected (generic)

    UST_ENCODING          = -0x0100,   //!< invalid character encoding
//---unexpected surrogate : severe errors
    UST_IVUTF16SUR_1     =  -0x4201,   //!< found unexpected or invalid utf16 surrogate first byte
    UST_IVUTF16SUR_2     =  -0x4202,   //!< found unexpected or invalid utf16 surrogate second byte
//--utf8 encoding-continuation byte errors

    UST_UTF8CAPAOVFLW     = -0x0800,   //!< utf8 capacity overflow : more than 3 bytes as character continuation bytes have been found
    UST_MISSCONTBYTE      = -0x0801,   //!< utf8 missing continuation character in a multi-byte sequence while expecting continuation character
    UST_UTF8IVCONTBYTES   = -0x0802,    //!< Invalid number of utf8 continuation bytes compared to header byte  : too much continuation bytes or not enough continuation bytes

    UST_TRUNCCONTBYTE     = -0x0803,    //!< endofstring mark has been found in place of continuation byte(s) : either for utf8 or utf16 (surrogate)

    UST_IVCODEPOINT       = -0x0900,   //!< Illegal - invalid codepoint found : mostly out of ranges for codepage or unicode values

    UST_TRUNCATED         =  0x1000,   //!< Warning:string has been truncated to fit within fixed size capacity: not an error

    UST_CONVERROR         = -0x5000,   //!< icu converter error
    UST_CONVWARNING       =  0x5000,   //!< Warning: icu converter warning

    UST_MEMERROR          = -0x4FFF,  //!< memory allocation error
}  ;

inline ZBool UST_Is_Error(UST_Status_type pStatus) {return (pStatus < 0);}
inline ZBool UST_Is_Severe(UST_Status_type pStatus) {return (pStatus < UST_SEVERE);}


ZStatus UST_to_ZStatus(UST_Status_type pUST);

/**
 * @brief decode_UST  returns UST_Status symbol as a const char*
 */
const char* decode_UST(const UST_Status_type pStatus);

#ifndef __ZBOM__
#define __ZBOM__

// =========Initially within zcharset.h & zcharset.cpp ======================
/**
 * @brief The ZBOM_type enum
 */
enum ZBOM_type: uint8_t
{
    ZBOM_NoBOM      = 0,
    ZBOM_LE         = 0x01,    // little endian mask for any BOM
    ZBOM_UTF8       = 0x02,    // Nota Bene utf8 little endian does not exist

    ZBOM_UTF16      = 0x04,    // ZBOM_UTF6 == ZBOM_UTF16_BE
    ZBOM_UTF16_BE   = 0x04,
    ZBOM_UTF16_LE   = 0x05,

    ZBOM_UTF32      = 0x08,    // ZBOM_UTF32 == ZBOM_UTF32_BE
    ZBOM_UTF32_BE   = 0x08,
    ZBOM_UTF32_LE   = 0x09,

    ZBOM_UTF7       = 0x10,
    ZBOM_EBCDIC     = 0x20,
    ZBOM_UTF1       = 0x30,
    ZBOM_SCSU       = 0x40,
    ZBOM_BOCU_1     = 0x50,
    ZBOM_GB_18030   = 0x60,           // chinese official standard

    ZBOM_BOMERROR = 0xFF
};
const char*  decode_ZBOM(ZBOM_type pZBOM);

#endif//__ZBOM__

typedef uint8_t ZCNV_Action_type;
/**
 * @brief ZCNV_Action Describes action to be taken during string conversion in case of invalid/illegal character
 *
 *
 * ZCNV_Escape_xxx Replace invalid character with corresponding escape sequence :<br>
 * \<escape header\>\<value\>\<optional escape trailer\><br>
 *
 * In escape modes description:<br>
 *  hhhhhhhh means an hexadecimal value<br>
 *  dddddddd means a decimal value<br>
 */
enum ZCNV_Action: ZCNV_Action_type{
    ZCNV_Nothing            = 0,
    ZCNV_Stop               = 0x01, //!<     Stops at any illegal/invalid sequence
    ZCNV_Skip               = 0x02, //!< Skip input character if illegal or invalid
    ZCNV_Substitute         = 0x04, //!< Subtitute character with the one given (or the defaulted one) in case of illegal character

    ZCNV_Escape_ICU         = 0x08, //!< icu escape mode \%Uhhhh (utf16 char)
    ZCNV_Escape_Java        = 0x09, //!< Java escape mode \\Uhhhh\\Uhhhhh
    ZCNV_Escape_C           = 0x0A, //!< C escape mode utf8: \\u8hh  utf16:\\uhhhh  utf32:\\Uhhhhhhhh
    ZCNV_Escape_XML_Dec     = 0x0B, //!< XML decimal escape mode &dddddddddd;
    ZCNV_Escape_XML_Hex     = 0x0C, //!< XML hexa escape mode &#xhhhhhhhh;
    ZCNV_Escape_Unicode     = 0x0D, //!< Unicode escape mode {U+hhhhhhhh}
    ZCNV_Escape_CSS2        = 0x0E, //!< CSS2 escape mode \\hhhhhhhh\<space\>

    ZCNV_Reset              = 0xF0  //!< Reset has been received and executed

//    ZCNV_Stop_Illegal   = 0x10 // mask to apply to make action stop at illegal character (not unassigned and not irregular)
};
const char*  decode_ZCNV(ZCNV_Action pZBOM);



enum utfSCEToFrom_type {
    SCErr_Nothing=0,
    SCErr_ToUnicode,            // from charset to unicode
    SCErr_FromUnicode           // from Utf unicode (codePoint) to charset
};
struct utfSCErr_struct {
    uint8_t*            Ptr=0;              // Ptr converted in uint8_t*
    size_t              UnitOffset=0;       // offset in character units from Ptr
    size_t              ByteOffset=0;       // offset in bytes from Ptr
    size_t              Size=0;             // size of the character in character units
    UST_Status_type     Status=UST_SUCCESS;
    utfSCEToFrom_type   ToFrom=SCErr_Nothing;
    utf32_t             codePoint=0;
    ZCNV_Action_type    Action=ZCNV_Nothing;

    utfSCErr_struct& operator = (utfSCErr_struct& pErr) ;
};
class utfSCErrQueue
{
public:
    ~utfSCErrQueue(void)
        {
        clear();
        }
    size_t          Size=0;
    utfSCErr_struct*  Queue=nullptr;
    void clear (void);
    void newError(size_t pUnitOffset,
                  size_t pSize,
                  UST_Status_type pStatus,
                  utfSCEToFrom_type pInOut,
                  utf32_t pCodepoint,
                  ZCNV_Action_type pAction);
    void push(utfSCErr_struct &pContent);
    utfSCErr_struct pop(void);
    utfSCErr_struct* getLast(void) ;
    utfSCErr_struct* get(size_t pIndex) ;
    utfSCErrQueue *_clone(void);
    void dump(FILE* pOutput=stderr);


};





/**
 * @brief The utfStrCtx class utf string parsing context.
 * utfStrCtx context has three purposes :
 * - define the behavior of string parsing engine
 * - finite state for string conversion
 * - reporting and error log
 *
 * Any ZString has an associated context as an utfStrCtx object.
 *
 * @par set behavior of parsing engine
 *
 *
 * @par reporting and errorlog
 *  utfStrCtx may store any action made during character set conversion.
 * Actions are stored in an even queue that may be listed afterwards.
 * This log is activated on demand using utfStrCtx options :<br>
 *  utfStrCtx::TraceTo : trace any issue occurring when converting from a given encoding to unicode,<br>
 *  ustStrCtx::TraceFrom : same as previous when converting from unicode to given encoding.
 *
 * utfStatus_type : String format parsing and conversion use a status much more detailed than global standard status ZStatus.
 * However, a conversion exists from utfStatus_type to ZStatus using the conversion routine UST_to_ZStatus().
 *
 * Context content may be reported using utfStrCtx::dump()
 *
 *
 *
 * @see utfStatus_type  ZBOM_type  ZCNV_Action
 */

class CtxCmd
{
public:
    ZBool       StopOnConvErr=false; //!< Stop on conversion error true: yes - false : use replacement character and continue
    ZBool       Reverse=false;       //!< 0 (false)->forward scan & 1 (true)->reverse scan
    ZCNV_Action Action=ZCNV_Substitute; //!< Action in case of Illegal - Irregular -Unassigned codepoint
    ZBool      TraceFrom=false;      //!< Trace any conversion issue to charset from unicode codePoint : feed ErrorQueue
    ZBool      TraceTo=false;        //!< Trace any conversion issue from charset to unicode codePoint : feed ErrorQueue
    void clear(void) {StopOnConvErr=false;Reverse=false;Action=ZCNV_Substitute;TraceFrom=false;TraceTo=false;}
};
//template <class _Utf>
class utfStrCtx:public CtxCmd
{
public:
    utfStrCtx(void) {init();}
    ~utfStrCtx(void)
                {  if (ErrorQueue) {delete ErrorQueue;} if (SavedChunk) zfree(SavedChunk);  }

    using CtxCmd::StopOnConvErr;
    using CtxCmd::Reverse;
    using CtxCmd::Action;
    using CtxCmd::TraceFrom;
    using CtxCmd::TraceTo;

/*      Input string */
   uint8_t*   Start=0;          //!< start of the input string
   uint8_t*   Effective=0;      //!< Effective start position within the input string, i. e. after the BOM
   uint8_t*   InPtr=0;          //!< current effective position (pointer) within input string (starting after BOM header if any)
   size_t     InUnitOffset=0;   //!< current offset in character units within input string
   size_t     InByteOffset=0;   //!< current offset in bytes within input string
/*      Output string */
   uint8_t*   OutPtr=0;         //!< current effective position (pointer) within input string (starting after BOM header if any)
   size_t     OutUnitOffset=0;  //!< current offset in character units within input string
   size_t     OutByteOffset=0;  //!< current offset in bytes within input string

   size_t    Strlen=0;         //!< size of the string expressed in _Utf characters units(multi-characters counting for number of characters)
   size_t    CharUnits=0;      //!< total _Utf character units number for current character <b>within input string</b>
   size_t     Count=0;          //!< Total number of character processed : not equivalent to character units - one character may use multiple char units
   int        Status=0;         //!< utfStatus_type containing last status of parsing operation
   utf32_t    CodePoint=0;      //!< current character - AFTER conversion - expressed in utf32 codepoint
   //                               If true reverse byte order. If false : use byte order unchanged.
   uint32_t   ErrFrom=0;        //!< number of conversion errors from Unicode codepoint to destination encoding
   uint32_t   ErrTo=0;          //!< number of conversion errors from source encoding to unicode codepoint
   uint32_t   Substit=0;        //!< number of character substitutions made
   uint32_t   Escaped=0;        //!< number of escape character substitutions made
   uint32_t   Skipped=0;        //!< number of character skipped
   ZBOM_type  BOM=ZBOM_NoBOM;   //!< nature of the detected BOM if any
   ZBool      Cloned=false;     //!< this object has been cloned if true

   ZBool      EndianRequest=0;  //!< request for a special endianness has been made : LittleEndian will give what request has been made
   ZBool      LittleEndian=0;   //!< litlle Endian management : 0->false 1->true NB: not used for utf8 .

   utfSCErrQueue* ErrorQueue=nullptr; //!< repository of any conversion issue

   void*          SavedChunk=nullptr;   //!< truncated multi-byte character is saved here
   size_t         SavedChunkSize=0;     //!< number of units saved as chunk
   size_t         SavedChunkFullSize=0; //!< full expected size of chunked character


   void setStopOnError (ZBool pStop)        {StopOnConvErr=pStop;}
   void setAction      (ZCNV_Action pAction){Action=pAction;}
   void setTraceFrom   (ZBool pTrace)       {TraceFrom=pTrace;}
   void setTraceTo     (ZBool pTrace)       {TraceTo=pTrace;}
   void setReverse     (ZBool pReverse)     {Reverse=pReverse;}

   void init(void);
   void reset(void);
   void resetIn(void);
   void resetOut(void);

   utfStrCtx* _clone(void);

   utfStrCtx* _from(utfStrCtx* pInCtx);

   int getStatus(void) {return Status;}
   bool endOfString(void) {return (Status==UST_ENDOFSTRING);}
   size_t getOffset(void) {return (size_t)(InPtr-Start);}

   uint8_t* getPosition(void) {return InPtr;}
   inline void setPosition(const void* pPosition) {InPtr=(uint8_t*)pPosition;}

   /**
    * @brief utfSetPosition sets current position.<br>
    *  In addition, it computes current offset as Start - Position.
    * Computed in bytes (unit8_t) for ByteOffset and in current character unit (_Utf) for UnitOffset.
    */
   template <class _Utf>
inline void utfSetPosition(const _Utf*pPosition)
    {
    setPosition(pPosition);
    InByteOffset=InPtr-Start;
    InUnitOffset = ((_Utf*)InPtr) - ((_Utf*)Start);
    }
   template <class _Utf>
   _Utf* utfGetPosition(void) {return (_Utf*)InPtr;}

   void setOffset(size_t pOffset) {InUnitOffset=pOffset;}

   uint8_t* getStart(void) {return(uint8_t*)Start;}

   template <class _Utf>
   _Utf* utfgetStart(void) {return (_Utf*)Start;}

   void setStart(const void* pStart) {Effective=Start=(uint8_t*)pStart;}

   void setEffective(const void* pEffective) {Effective=(uint8_t*)pEffective;}

   template <class _Utf>
   _Utf* utfGetEffective(void) {return (_Utf*)Effective;}

   void setTraceToUnicode(ZBool pTrace) {TraceTo=pTrace;}
   void setTraceFromUnicode(ZBool pTrace) {TraceFrom=pTrace;}
   /**
    * @brief dump reports context content to pOutput (defaulted to stderr).
    */
   void dump(FILE* pOutput=stderr);
   char* _dump(void);

  };


#endif // UTFSTRCTX_H
