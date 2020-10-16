#ifndef ZCHARSET_CPP
#define ZCHARSET_CPP

#include <ztoolset/zdatabuffer.h>
//#include <ztoolset/templatestring.h>

#include <ztoolset/zcharset.h>

#include <libxml2/libxml/encoding.h>

#include <ztoolset/zexceptionmin.h>

#include <iconv.h>

#include <ztoolset/zatomicconvert.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zutfstrings.h>

//#include <ztoolset/zfunctions.h>
#include <ztoolset/utfutils.h>

#include <ztoolset/zlimit.h>

using namespace zbs;

/** following codelet is taken from gcc compiler source gcc 8.2.0 - file <safe-ctype.h>  author Zack Weinberg
*/
#include <locale.h>
#include <langinfo.h>

/* Determine host character set.  */
#define HOST_CHARSET_UNKNOWN 0
#define HOST_CHARSET_ASCII   1
#define HOST_CHARSET_EBCDIC  2

#if  '\n' == 0x0A && ' ' == 0x20 && '0' == 0x30 \
   && 'A' == 0x41 && 'a' == 0x61 && '!' == 0x21
#  define HOST_CHARSET HOST_CHARSET_ASCII
#else
# if '\n' == 0x15 && ' ' == 0x40 && '0' == 0xF0 \
   && 'A' == 0xC1 && 'a' == 0x81 && '!' == 0x5A
#  define HOST_CHARSET HOST_CHARSET_EBCDIC
# else
#error "Severe Error-Unrecognized basic host character set"
# endif
#endif
/** following codelet is taken from gcc compiler source gcc 8.2.0 - file <charset.c>
*/
#if HOST_CHARSET == HOST_CHARSET_ASCII
#define SOURCE_CHARSET "UTF-8"
#define LAST_POSSIBLY_BASIC_SOURCE_CHAR 0x7e
#elif HOST_CHARSET == HOST_CHARSET_EBCDIC
#define SOURCE_CHARSET "UTF-EBCDIC"
#define LAST_POSSIBLY_BASIC_SOURCE_CHAR 0xFF
#else
#error "Severe Error-Unrecognized basic host character set"
#endif

static ZCharset_type DefaultCharset=ZCHARSET_ERROR;
static const char* DefaultEncodingName = nullptr;

/*static utf32_t  utf32Replacement = 0xFFFD ;
static utf16_t  utf16Replacement = 0xFFFD ;
static utf8_t  utf8Replacement[5] = {0xEF, 0xBF, 0xBD,0,0} ;


utf32_t  utf32Replacement = 0xFFFD ;
utf16_t  utf16Replacement[3] = {0xFFFD,0,0 } ;
utf8_t  utf8Replacement[5] = {0xEF, 0xBF, 0xBD,0,0} ;
*/
const char* getDefaultEncodingName(void)
{
    if (DefaultEncodingName)
            return DefaultEncodingName;
/*
const char* wCharsetName =  getenv("LANG");

   if ((wCharsetName==nullptr)||(wCharsetName[0]==0))
            {
            wCharsetName=nl_langinfo (CODESET);  // gcc says that most systems support nl_langinfo nowadays
            if ((wCharsetName==nullptr)||(wCharsetName[0]==0))
                                    wCharsetName=SOURCE_CHARSET;
            }
    DefaultEncodingName=wCharsetName;
*/

// --------ICU gives the certified system default charset-------------
    UErrorCode wICUError=U_ZERO_ERROR;

    UConverter* wCnv = ucnv_open (nullptr,&wICUError);
    const char* wCharsetName =ucnv_getName(wCnv,&wICUError);
    ucnv_close (wCnv);

    return wCharsetName;
}

ZStatus changeDefaultEncodingName(const char* pEncoding)
{
    ZCharset_type wCS=nameToZCharset(pEncoding); // test if valid encoding name
    if ((wCS==ZCHARSET_ERROR)||(wCS==ZCHARSET_NOTSUPPORTED))
                                            return ZS_INVCHARSET;
    DefaultEncodingName=pEncoding;
    DefaultCharset = wCS;
    return ZS_SUCCESS;
}

ZCharset_type
getDefaultCharset(void)
{
    if (DefaultCharset!=ZCHARSET_ERROR)
                        return DefaultCharset;

/*
 *     const char* wCharsetName =  getenv("LANG");

    if ((wCharsetName==nullptr)||(wCharsetName[0]==0))
            {
            wCharsetName=nl_langinfo (CODESET);  // gcc says that most systems support nl_langinfo nowadays
            if ((wCharsetName==nullptr)||(wCharsetName[0]==0))
                                    wCharsetName=SOURCE_CHARSET;
            }*/
    DefaultCharset=nameToZCharset(getDefaultEncodingName());

    if (DefaultCharset==ZCHARSET_ERROR) // unrecognized charset
            return (DefaultCharset=nameToZCharset(SOURCE_CHARSET));   // use default system charset

/*    if (DefaultCharset==ZCHARSET_ASCII )  // if default is US-ASCII
        {
        if (pAllowUSACSII)              // and US-ASCII is allowed
                    return DefaultCharset; // return it
        return (DefaultCharset=nameToZCharset(SOURCE_CHARSET));        // else use default system charset
        }*/
    return DefaultCharset;
}//getDefaultCharset

void
changeDefaultCharset(ZCharset_type pNewDefaultCharset)
{
    DefaultCharset=pNewDefaultCharset;
    return;
}

ZStatus changeReplacementCodepoint(utf32_t pReplacement)
{
    utf32_t  wutf32Replacement = 0xFFFD ;
    utf16_t  wutf16Replacement[3] = {0xFFFD,0,0} ;
    utf8_t  wutf8Replacement[5] = {0xEF, 0xBF, 0xBD,0,0} ;

// --- First try to encode replacement codepoint to target ut encodings   --

    size_t wCount;

    UST_Status_type wSt=utf32Encode(&wutf32Replacement,pReplacement,nullptr);
    if (UST_Is_Error(wSt))
                    return UST_to_ZStatus( wSt);
    wSt=    utf8Encode( wutf8Replacement,&wCount,pReplacement,nullptr);
    if (UST_Is_Error(wSt))
                    return UST_to_ZStatus( wSt);
    wSt=utf16Encode((utf16_t*)wutf16Replacement,&wCount,pReplacement,nullptr);
    if (UST_Is_Error(wSt))
                    return UST_to_ZStatus( wSt);
//----All format encoding are successful : save values ------------

    utf32Replacement=wutf32Replacement;
    for (size_t wi=0;wi<sizeof(utf16Replacement);wi++)
            utf16Replacement[wi]=wutf16Replacement[wi];

    for (size_t wi=0;wi<5;wi++)
            utf8Replacement[wi]=wutf8Replacement[wi];

    return ZS_SUCCESS;
}


//------------------ BOMs -------------------------------

//  utf1 BOM :        F7 64 4C

const utf8_t cst_utf1BOM[3] = 	{ 0xF7, 0x64, 0x4C };       // Size 3

//  utf7 BOM :        2B 2F 76 ( 38 | 39 | 2B | 2F )

const utf8_t cst_utf7BOM1[4] = 	{ 0x2B, 0x2F, 0x76, 0x38};  // Size 4
const utf8_t cst_utf7BOM2[4] = 	{ 0x2B, 0x2F, 0x76, 0x39};
const utf8_t cst_utf7BOM3[4] = 	{ 0x2B, 0x2F, 0x76, 0x2B};
const utf8_t cst_utf7BOM4[4] = 	{ 0x2B, 0x2F, 0x76, 0x2F};

//  utf EBCDIC BOm	DD 73 66 73

const utf8_t cst_utfEBCDIC[4] = 	{ 0xDD, 0x73, 0x66, 0x73};  // Size 4

// utf16 BOM FE FF  (big endian) - FF FE (little endian)

/*
const utf16_t cst_utf16BOMBE  = { 0xFEFF };// Size 2
const utf16_t cst_utf16BOMLE  = { 0xFFFE }; // Warning : may be confused with cst_utf32BOMLE (Size 4)

const utf8_t cst_utf16BOMBE_Byte[]  = { 0xFE,0xFF };
const utf8_t cst_utf16BOMLE_Byte[]  = { 0xFF,0xFE };

// utf32 BOM  00 00 FE FF (big endian) - FF FE 00 00 (little endian)

const utf8_t cst_utf32BOMBE_Byte [4] = { 0,0,0xFE,0xFF };    // Size 4
const utf8_t cst_utf32BOMLE_Byte [4] = { 0xFF,0xFE,0,0 };    // Warning : may be confused with cst_utf16BOMLE (Size 2)

const utf32_t cst_utf32BOMBE  = 0x0000FEFF ;    // Size 4
const utf32_t cst_utf32BOMLE =  0xFFFE0000 ;

// utf8 BOM  EF BB BF

const utf8_t cst_utf8BOM[3] =    { 0xEF,0xBB,0xBF };        // Size 3
*/

#ifdef __COMMENT__

/** brief cst_default_delimiter default delimiter set of characters per character storage format
 */
const char cst_default_delimiter_Char[] = {
    (char)' ',
    (char)'\t',
    (char)'\n',
    (char)'\r',
    (char)'\0'  // must end with '\0'
};
const utf8_t cst_default_delimiter_U8[] = {
    (utf8_t)' ',
    (utf8_t)'\t',
    (utf8_t)'\n',
    (utf8_t)'\r',
    (utf8_t)'\0'  // must end with '\0'
};
const utf16_t cst_default_delimiter_U16[] = {
    (utf16_t)' ',
    (utf16_t)'\t',
    (utf16_t)'\n',
    (utf16_t)'\r',
    (utf16_t)'\0'  // must end with '\0'
};

const utf32_t cst_default_delimiter_U32[] = {
        (utf32_t)' ',
        (utf32_t)'\t',
        (utf32_t)'\n',
        (utf32_t)'\r',
        (utf32_t)'\0'  // must end with '\0'
    };


/** UNICODE Space characters
 *
U+0020 	SPACE           Depends on font, typically 1/4 em, often adjusted
U+00A0 	NO-BREAK SPACE 	As a space, but often not adjusted
-------------Above FF---------------------------------------------------------
U+1680 	OGHAM SPACE MARK 	Unspecified; usually not really a space but a dash
U+180E 	MONGOLIAN VOWEL SEPARATOR 	foo᠎bar 	No width
U+2000 	EN QUAD 	foo bar 	1 en (= 1/2 em)
U+2001 	EM QUAD 	foo bar 	1 em (nominally, the height of the font)
U+2002 	EN SPACE 	foo bar 	1 en (= 1/2 em)
U+2003 	EM SPACE 	foo bar 	1 em
U+2004 	THREE-PER-EM SPACE 	foo bar 	1/3 em
U+2005 	FOUR-PER-EM SPACE 	foo bar 	1/4 em
U+2006 	SIX-PER-EM SPACE 	foo bar 	1/6 em
U+2007 	FIGURE SPACE 	foo bar 	“Tabular width”, the width of digits
U+2008 	PUNCTUATION SPACE 	foo bar 	The width of a period “.”
U+2009 	THIN SPACE 	foo bar 	1/5 em (or sometimes 1/6 em)
U+200A 	HAIR SPACE 	foo bar 	Narrower than THIN SPACE
U+200B 	ZERO WIDTH SPACE 	foo​bar 	Nominally no width, but may expand
U+202F 	NARROW NO-BREAK SPACE 	foo bar 	Narrower than NO-BREAK SPACE (or SPACE)
U+205F 	MEDIUM MATHEMATICAL SPACE 	foo bar 	4/18 em
U+3000 	IDEOGRAPHIC SPACE 	foo　bar 	The width of ideographic (CJK) characters.
U+FEFF 	ZERO WIDTH NO-BREAK SPACE 	foo﻿bar 	No width (the character is invisible)
*/
const size_t cst_UCSSpaces16NB=17;
const utf16_t cst_UCSSpaces16 [] ={
    0x0020,
    0x00A0,
    0x2001,
    0x2002,
    0x2003,
    0x2004,
    0x2005,
    0x2006,
    0x2007,
    0x2008,
    0x2009,
    0x200A,
    0x200B,
    0x202F,
    0x205F,
    0x3000,
    0xFEFF  // BOM
};



/**
UNICODE Conversion (based on ISO/CEI 8859-15)
------------------------------------------------
Substitute Accented characters

Accute	  conv  conv
range	  char	code

C0-C6 	-> A   	41
C7      -> C	43
C8-CB	-> E	45
CC-CF	-> I   	49
D0      -> D	44
D1      -> N	4E
D2-D6	-> O	4F
D8      -> O	4F
D9-DC	-> U	55
DD      -> Y 	59
DF      -> S	53
E0-E6	-> a	61
E7      -> c	63
E8-EB	-> e	65
EC-EF	-> i	69
F0      -> o	6F
F1      -> n	6E
F2-F6	-> o	6F
F8      -> o	6F
F9-FC	-> u	75
FD      -> y	79
FF      -> y	79
--------------------------------------------------------
SPACES (more unicode spacing char available above FF)
20      -> space
A0      -> non breaking space
----------------------------------------------------------
Uppercase to lower letter
41-5A  	-> 61-7A
C0-D6	-> E0-F6
D8-DD	-> F8-FD
-----------------------------------------------------------
Lower to Uppercase letter
61-7A	-> 41-5A
E0-F6	-> C0-D6
F8-FD	-> D8-DD
--------------------------------
*/

/** cst_DropAcute : converts accented charaters to normal characters without acute (and special characters i.e. ß to S)
  */
static const utf8_t cst_DropAcute[256]= {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x08,0x09,0x0A,0x00,0x00,0x0D,0x0E,0x0F, // backspace=0x08 - tab=0x09 lf=0x0A CR=0x0D
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x00, // Esc=1B
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27, // space=0x20 then punctuation
    0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F, // punctuation
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37, // [numbers
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F, // numbers] [0X3A-> punctuation
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47, // @=0x40] [0x41-> Capital letters
    0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F, //
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,// ->0x5A Capital letters] [0x5B -> punctuation
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,// 0x60=accent-[0x61 ;0x7A]  small letters(a-z)
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x43, // C0-C6 	-> A   	41 * C7      -> C	43
    0x45,0x45,0x45,0x45,0x49,0x49,0x49,0x49, // C8-CB	-> E	45 * CC-CF	-> I   	49
    0x44,0x4E,0x4F,0x4F,0x4F,0x4F,0x4F,0xD7, // D0      -> D	44 * D1      -> N	4E * D2-D6	-> O	4F *
    0x4F,0x55,0x55,0x55,0x55,0x59,0xDE,0x53, // D8      -> O	4F * D9-DC	-> U	55 * DD      -> Y 	59 * DF      -> S	53
    0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x63, // E0-E6	-> a	61 * E7      -> c	63
    0x65,0x65,0x65,0x65,0x69,0x69,0x69,0x69, // E8-EB	-> e	65 * EC-EF	-> i	69
    0x6F,0x6E,0x6F,0x6F,0x6F,0x6F,0x6F,0xF7, // F0      -> o	6F * F1      -> n	6E * F2-F6	-> o	6F
    0x6F,0x75,0x75,0x75,0x75,0x79,0xFE,0x79  // F8      -> o	6F * F9-FC	-> u	75 * FD      -> y	79 * FF      -> y	79
};


static const utf8_t cst_ToLower[256] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x08,0x09,0x0A,0x00,0x00,0x0D,0x0E,0x0F, // backspace=0x08 - tab=0x09 lf=0x0A CR=0x0D
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x00, // Esc=1B
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27, // space=0x20 then punctuation
    0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F, // punctuation
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37, // [numbers
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F, // numbers] [0X3A-> punctuation
    0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67, // @=0x40] [0x41-> Capital letters : 41-5A  	-> 61-7A
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F, //
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F,// ->0x5A Capital letters] [0x5B -> punctuation
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,// 0x60=accent-[0x61 ;0x7A]  small letters(a-z)
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7, // C0-D6	-> E0-F6
    0xE8,0xE9,0xEA,0xEB,0xEC,0xCD,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xD7,
    0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xDE,0xDF, // D8-DD	-> F8-FD
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
    0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
    0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};
static const utf8_t cst_ToUpper[256] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x08,0x09,0x0A,0x00,0x00,0x0D,0x0E,0x0F, // backspace=0x08 - tab=0x09 lf=0x0A CR=0x0D
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x00, // Esc=1B
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27, // space=0x20 then punctuation
    0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F, // punctuation
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37, // [numbers
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F, // numbers] [0X3A-> punctuation
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47, // @=0x40] [0x41-> Capital letters
    0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F, //
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,// ->0x5A Capital letters] [0x5B -> punctuation
    0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,// 61-7A	-> 41-5A
    0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x5A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7, // A0 : Non Breaking Space : NBSP
    0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
    0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7, // E0-F6	-> C0-D6
    0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,
    0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xFE,0xFF  // F8-FD	-> D8-DD
};

static const utf8_t cst_Natural[256] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x08,0x09,0x0A,0x00,0x00,0x0D,0x0E,0x0F, // backspace=0x08 - tab=0x09 lf=0x0A CR=0x0D
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x00, // Esc=1B
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27, // space=0x20 then punctuation
    0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F, // punctuation
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37, // [numbers
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F, // numbers] [0X3A-> punctuation
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47, // @=0x40] [0x41-> Capital letters
    0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F, //
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,// ->0x5A Capital letters] [0x5B -> punctuation
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,// 0x60=accent-[0x61 ;0x7A]  small letters(a-z)
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7, // A0 : Non Breaking Space : NBSP
    0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
    0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
    0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
    0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};



static const xmlChar CharAndPunctuation[256] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x08,0x09,0x0A,0x00,0x00,0x0D,0x0E,0x0F, // backspace=0x08 - tab=0x09 lf=0x0A CR=0x0D
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x00, // Esc=1B
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27, // space=0x20 then punctuation
    0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F, // punctuation
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37, // [numbers
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F, // numbers] [0X3A-> punctuation
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47, // @=0x40] [0x41-> Capital letters
    0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F, //
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x5A,0x7B,0x5C,0x5D,0x5E,0x5F,// ->0x5A Capital letters] [0x5B -> punctuation
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,// 0x60=accent-[0x61 ;0x7A]  small letters(a-z)
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
    0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
    0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
    0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};

  const utf16_t cst_Surrogate_1 = 0xD800;
  const utf16_t cst_Surrogate_2 = 0xDC00;
  const utf16_t cst_Surrogate[] = {0xD800,0xDC00};

#endif //__COMMENT::


//#include <unicode/i18n/csdetect.h>
//#include <unicode/i18n/csmatch.h>
/*
ZCharset_type detectCharset(const char*pInString, ssize_t pByteLen,int32_t *pConfidence,ZBool pWarningSignal)
{
    int32_t wByteLen= pByteLen;
    UErrorCode wErr=U_ZERO_ERROR;


    if (wByteLen<0)
            {
            wByteLen=0;
            const char* wPtr=pInString;
            while (*wPtr++)
                    wByteLen++;
            }
    icu::CharsetDetector wCSD(wErr);
    wCSD.setStripTagsFlag(true);      // avoid html markups
    wCSD.setText(pInString,wByteLen);

    const icu::CharsetMatch* wCSM=wCSD.detect(wErr);
    if (testIcuFatal(_GET_FUNCTION_NAME_,
                     wErr,
                     pWarningSignal,
                     "while intantiating charset detector") < 0)
                            {
                            if (pConfidence)
                                    *pConfidence = 0;
                            return ZCHARSET_ERROR;
                            }

    const char* wCharsetName= wCSM->getName();
    if(pConfidence)
        {
        *pConfidence=wCSM->getConfidence();
        }

    return encode_ZCharset(wCharsetName);
}//detectCharset

ssize_t detectCharsetAll(const char*pInString,ssize_t pByteLen, ZArray<ZCharsetCol_struct>& pCharsetAr, ZBool pWarningSignal)
{
    int32_t wByteLen= pByteLen, wMatchNb=0;
    UErrorCode wErr=U_ZERO_ERROR;
    ZCharsetCol_struct wCS;

    pCharsetAr.clear();

    if (wByteLen<0)
            {
            wByteLen=0;
            const char* wPtr=pInString;
            while (*wPtr++)
                    wByteLen++;
            }
    icu::CharsetDetector wCSD(wErr);
    if (testIcuFatal(_GET_FUNCTION_NAME_,
                     wErr,
                     pWarningSignal,
                     "while intantiating charset detector") < 0)
                                            return -1;
    wCSD.setText(pInString,wByteLen);

    const icu::CharsetMatch *const* wCSMList;
    wCSMList=wCSD.detectAll(wMatchNb,wErr);
    if (testIcuError(_GET_FUNCTION_NAME_,
                     wErr,
                     pWarningSignal,
                     "while detecting charset matches") < 0)
                                            return -1;

    for (size_t wcsd=0;wcsd < wMatchNb;wcsd++)
        {
        wCS.clear();
        wCS.Charset = encode_ZCharset( wCSMList[wcsd]->getName());
        wCS.Confidence = wCSMList[wcsd]->getConfidence();
        ::strncpy (wCS.Language,wCSMList[wcsd]->getLanguage(),sizeof(wCS.Language));
        pCharsetAr.push(wCS);
        }
    return pCharsetAr.size();
}//detectCharsetAll
*/
utf8_t utf8DropAccute(utf8_t pCharacter)
{
    return cst_DropAcute[pCharacter];
}

/**
 * @brief utf8Encode encodes a codepoint as an utf32 character (pUtf32) into an utf8 sequence pointed by pUtf8
 * pUtf8 must point to an array of 5 utf8 characters.
 * The last character of pUtf8 after the last one being used is set to zero.
 * In case of illegal/invalid character format, resulting codePoint is set to replacement character for utf8 format.
 * This replacement character may be changed at a global level using changeReplacementCodepoint() routine.
 *
 * @param[out] pUtf8 reference to an array of 5 utf8 characters. It will receive encoded utf8 character.<br>
 *  last utf8 character unit is followed by (utf8_t)'\0', so that resulting character may be considered as an utf8 string.
 * @param[out] pUtf8Count number of utf8 characters units used among the 5 utf8 characters to encode given utf32 codepoint.<br>
 *  This field is set to the size of replacement character if utf32 codepoint is out of valid unicode range.
 *  For replacement default, this size is 3.
 * @param[in] pUtf32Codepoint utf32 codepoint to encode.
 * @param[in] pRepacement utf8 replacement character sequence. Defaulted (nullptr) to {0xEF, 0xBF, 0xBD,0}
 * which is the utf8 encoding for 0xFFFD, standard Unicode replacement character.
 * @param[in] pEndian Optional endianness of given utf32 codepoint.<br> little endian is true, and big endian if false.<br>
 *  If omitted (nullptr) current system endianness is used.
 * @return  a utfStatus_type
 *  <b>UST_SUCCESS</b> if everything went OK
 *  <b>UST_IVCODEPOINT</b> if codepoint is outside valid unicode codepoint range.<br>
 *  In this case, resulting utf8 character is set to replacement character and  pUtf8Count is set to 3 (size of utf8 replacement character)
 *
 */
UST_Status_type utf8Encode( utf8_t (&pUtf8)[5],
                            size_t *pUtf8Count,
                            utf32_t pCodepoint,
                            ZBool *pEndianness)
{
    pUtf8[0] = 0;
    pUtf8[1] = 0;
    pUtf8[2] = 0;
    pUtf8[3] = 0;
    pUtf8[4] = 0;

    utf32_t wUtf32=pCodepoint;

    if (pEndianness)  // if not omitted
        if (is_little_endian()==*pEndianness)
                wUtf32=forceReverseByteOrder<utf32_t>(pCodepoint);

    if (wUtf32 < 0x80) {
       pUtf8[0] = (utf8_t)wUtf32 ;
       *pUtf8Count = 1;// utf8 character unit is 1
       return UST_SUCCESS;
    }
    if (wUtf32 < 0x0800) {
       pUtf8[0] = (wUtf32>>6)  & 0x1F | 0xC0;
       pUtf8[1] = (wUtf32>>0)  & 0x3F | 0x80;
       pUtf8[2] = 0;
       pUtf8[3] = 0;
       *pUtf8Count = 2;// number of utf8 character units is 2
       return UST_SUCCESS;
    }
    if (wUtf32<0x010000) {
       pUtf8[0] = (wUtf32>>12) & 0x0F | 0xE0;
       pUtf8[1] = (wUtf32>>6)  & 0x3F | 0x80;
       pUtf8[2] = (wUtf32>>0)  & 0x3F | 0x80;
       pUtf8[3] = 0;
       *pUtf8Count = 3;// number of utf8 character units is 3
       return UST_SUCCESS;
    }
    if (wUtf32<0x110000){
       pUtf8[0]  = (wUtf32>>18) & 0x07 | 0xF0;
       pUtf8[1]  = (wUtf32>>12) & 0x3F | 0x80;
       pUtf8[2]  = (wUtf32>>6)  & 0x3F | 0x80;
       pUtf8[3]  = (wUtf32>>0)  & 0x3F | 0x80;
       *pUtf8Count = 4;// number of utf8 character units is 4
       return UST_SUCCESS;
    }

    // Out of range error : set to replacement character and issue invalid charset status

    *pUtf8Count = 1;// number of utf8 character units
    long wi=0;
    while ((wi < sizeof(utf8Replacement)) && utf8Replacement[wi])
        {
         pUtf8[wi]=utf8Replacement[wi];
         wi++;
         (*pUtf8Count)++;
        }

/*  Corresponds to UST_ILLEGAL for utf32 codePoint :
    Standard says cannot code on more than 4 character units i.e. greater than (or equal to) 0x110000
*/
    return UST_IVCODEPOINT;

}// encode_utf8



/* ==============This after copied verbatim from libxml2/xmlstring.c from D. Veillard==============
*
************************************************************************
*                                                                      *
*              Generic UTF8 handling routines                          *
*                                                                      *
* From rfc2044: encoding of the Unicode values on UTF-8:               *
*                                                                      *
* UCS-4 range (hex.)           UTF-8 octet sequence (binary)           *
* 0000 0000-0000 007F   0xxxxxxx                                       *
* 0000 0080-0000 07FF   110xxxxx 10xxxxxx                              *
* 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx                     *
*                                                                      *
* I hope we won't use values > 0xFFFF anytime soon !                   *
*                                                                      *
************************************************************************/


  /**
   * @brief _UtfreverseChar
   *
     Char. number range  |        UTF-8 octet sequence
        (hexadecimal)    |              (binary)
     --------------------+---------------------------------------------
     0000 0000-0000 007F | 0xxxxxxx
     0000 0080-0000 07FF | 110xxxxx 10xxxxxx
     0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
     0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   *
   * @param pString
   * @param pStringLen
   */


  /**
   * @brief utf8GetReverseChar
   * @param[in] pString       utf8_t* string to reverse scan
   * @param[in-out] pStringLen    pointer to optional ssize_t string length : updated to point to length WITHOUT last found character
   *                          if nullptr : string size is omitted. No update to this field
   *                          if -1 : string length is first computed then updated to point to length WITHOUT last found character
   * @param[in-out] pStringBeforeChar mandatory pointer to the string position just before last found character
   *          MUST NOT BE NULL
   *          if points to a value greater than nullptr : used as current string pointer to find next last character.
   *          pStringLen will be updated accordingly
   *          if points to nullptr : pStringBeforeChar will be first computed
   * @param[out] pCharContent      utf32_t character found
   * @param[out] pUtf8CharLen      size in bytes of character found as an utf8 format within string
   * @return a ZStatus ZS_SUCCESS     if everything went right : returned character is pCharContent
   *                   ZS_INVCHARSET  utf8 surrogate mechanism corruption :
   *                              - a singled character byte has been found in the middle of a sequence
   *                              -
   *                  ZS_INVSIZE
   *                  ZS_FIELDCAPAOVFLW : more than 1 first byte + 3 continuation bytes have been found
   *
   *                  ZS_OUTBOUNDLOW :
   */
ZStatus
utf8GetPreviousChar(utf8_t* pString,ssize_t* pStringLen,utf8_t** pStringBeforeChar,utf32_t* pCharContent,ssize_t* pUtf8CharLen)
{
utf8_t* wString=pString;
utf32_t wChar=0;
int wIdx=0, wLen=0, wStringLen=0;
utf32_t wSplit[4];

  if (*pStringBeforeChar==pString)
      {
      *pStringBeforeChar=nullptr;
      return ZS_OUTBOUNDLOW;
      }
  if (*pStringBeforeChar)
      {
      wString=*pStringBeforeChar;
      wStringLen=*pStringLen;
      }
  else
  {
  if((!pStringLen)||(*pStringLen < 0))
      {
      while(*wString++)
                  wStringLen++;
      }
  else
      {
      wStringLen=*pStringLen;
      wString+=wStringLen;
      }
  }// else
  wChar=*wString;
  if (*wString < 0x80)  // one byte character
          {
          *pCharContent =(utf32_t) wChar ;    // return character content
          wString --;
          wStringLen--;
          *pStringBeforeChar = wString;       // return position before char
          if (pStringLen)
                  *pStringLen= wStringLen; // if pStringLen not omitted : return string length before char
          return ZS_SUCCESS;
          }
//------------------Up to here : we should find utf8 surrogate composed character-------------------

  while (wString>=pString)
  {
      wChar=*wString;
      if (*wString < 0x80)  // invalid one byte character found here
              {
              *pCharContent =(utf32_t) -1 ;       // return invalid character content
//                wString --;
              *pStringBeforeChar = wString;       // return position to valid one byte char
              *pUtf8CharLen = 0;
              if (pStringLen)
                      *pStringLen= wStringLen; // if pStringLen not omitted : return string length before char
              return ZS_INVCHARSET;
              }

//        if (wChar & 0x80)           /*  1000 0000    -> 0x80     NB: Always true*/
//        {
      if (( wChar & ~ 0x80 ) && !(wChar & 0x40))
              {
              if (wIdx==2)
                      return ZS_FIELDCAPAOVFLW; // no more than 3 bytes as continuation byte
              wSplit[wIdx++] = (utf32_t)( wChar & ~ 0x80 ) ;  // continuation byte : 1000 0000
              wString --;
              continue;
              }
//-----------------Up to here : found first surrogate character : analyze it ------------------------

      if (( wChar & ~0xC0 ) && !(wChar & 0x20))
              {           //                        +---------> must be zero
                          //                        |   +-----> char data
                          //                        |---|--
              wLen = 2;   // two bytes character  1100 0000
              if (wIdx==3)
                      return ZS_FIELDCAPAOVFLW; // no more than 4 bytes
              wSplit[wIdx++] = (utf32_t)( wChar & ~0xC0 );
              break ; // found leading byte
              }
      if (( wChar & ~0xE0 ) && !(wChar & 0x10))
              {         //                          +---------> must be zero !
                        //                          |   +-----> char data
                        //                          | --|-
              wLen = 3; // three bytes character 1110 0000
              if (wIdx==3)
                      return ZS_FIELDCAPAOVFLW; // no more than 4 bytes
              wSplit[wIdx++] = (utf32_t)( wChar & ~0xE0 );
              break; // found leading byte
              }
      if (( wChar & ~0xF0 ) && !(wChar & 0x08))
              {         //                            +---------> must be zero !
                        //                            | +-----> char data
                        //                            |-|-
              wLen = 4; // four  bytes character 1111 0000
              if (wIdx==3)
                      return ZS_FIELDCAPAOVFLW; // no more than 4 bytes
              wSplit[wIdx++] = (utf32_t)( wChar & ~0xF0 );
              break;      // found leading byte
              }

      *pCharContent =(utf32_t) -1 ;       // return invalid character content
      *pStringBeforeChar = wString;       // return position to valid one byte char
      *pUtf8CharLen = 0;
      if (pStringLen)
              *pStringLen= wStringLen; // if pStringLen not omitted : return string length before char
      return ZS_INVCHARSET;
//        } // if (wChar & 0x80) NB: always true

  }// while

ZStatus wSt=ZS_SUCCESS;

  wString --;
  *pStringBeforeChar = wString;       // return position before char

  if (wIdx!=(wLen-1))
          {
          fprintf (stderr,
                   "%s>> Error : utf8 encoding incoherence between declared size in bytes <%d> and effective surrogate bytes <%s>\n",
                   _GET_FUNCTION_NAME_,
                   wLen,
                   wIdx);

          wSt= ZS_INVSIZE;  // incoherence between size found and size mentionned in first byte
          // but return nevertheless effective stored character value
          }
  wChar=0;
  int wShift6=0;
  for (wIdx=0;wIdx<wLen;wIdx++)
      {
      wChar += (wSplit[wIdx]<<(wShift6*6));  // NB : 2 bits are taken per byte as 'continuation mark' : so must shift 6 bits by 6 bits
      wShift6++;
      }
  *pCharContent =wChar;
  *pUtf8CharLen= wLen;
  return wSt;
}//utf8GetReverseChar

inline ZBool utf8IsContByte(utf8_t pUtf)
{
//   return ((pUtf & 0x80) && !(pUtf & 0x40)); //   first bit set-second bit must be zero     1000 0000
   return ((pUtf & 0b10000000) && !(pUtf & 0b01000000)); //   first bit set-second bit must be zero     1000 0000
}

/**
 * @brief utf8CheckFormat parse utf8 string and checks first character utf8 format.
 * Counts number of character units composing the utf8 character.
 * Check utf8 format validity, and returns appropriate utfStatus_type value if character is ill formed.
 * In case of error, number of illegal/errored units is returned.
 *
 * @param pInString pointer to utf8 character to check and count
 * @param pUtf8Count returned number of character units
 * @return an utfStatus_type<br>
 * <b>UST_SUCCESS</b> a well formed utf8 character is counted.<br>
 * <b>UST_ILLEGAL</b> if first character unit has an value out of utf8 range<br>
 * <b>UST_MISSCONTBYTE</b> a multi-units character missed a continuation unit vs indications given by its first unit (byte)br>
 * <b>UST_TRUNCCONTBYTE</b> endofstring mark has been encountered in the middle of a multi-units character.<br>
 * Equivalent to UST_TRUNCATEDCHAR when using ICU utilities.br>
 */
inline UST_Status utf8CheckFormat(const utf8_t* pInString, ssize_t* pUtf8Count)
{
utf8_t wC;
    *pUtf8Count=0;
    wC=pInString[0];
    if (wC==__ENDOFSTRING__)
                return UST_ENDOFSTRING;
    if (pInString[0]< 0x80)  // one byte character
            {
//            wChar[0] = wC ;
//            *pUtf32CodePoint = (utf32_t)wC;  // return character content
            *pUtf8Count=1;
            return UST_SUCCESS;
            }

/* check utf8 format */
    while (true)
    {
//    if (( wC & 0xC0 )&&!(wC & 0x20))
    if (( pInString[0] & 0b11000000 ) && !(pInString[0] & 0b00100000))
            {           //                        +---------> must be zero if this is header byte : 2 octets
                        //                        |   +-----> char data
                        //                        |---|--
                        // two bytes character  1100 0000
            *pUtf8Count=2;

            break;
                }//  (( wC & 0xC0 )&&!(wC & 0x20)) -> 2 bytes utf8 character

//    if (( wC & 0xE0 )&& !(wC & 0x10))
    if (( pInString[0] & 0b11100000 ) && !(pInString[0] & 0b00010000))
            {       //                          +---------> must be zero  must be zero if this is header byte : 3 octets
                    //                          |   +-----> char data
                    //                          | --|-
                    // three bytes character 1110 0000

            *pUtf8Count=3;
            break;
            }//  (( wChar & 0xE0 )&& !(wChar & 0x10)) -> 3 bytes utf8 character
//    if ((wC & 0xF0) && !(wC & 0x08))
    if ((pInString[0] & 0b11110000) && !(pInString[0] & 0b00001000))
            {         //                            +--------->  must be zero if this is header byte : 4 octets
                      //                            | +-----> char data
                      //                            |-|-
                      // four  bytes character 1111 0000

            *pUtf8Count=4;
            break;
            }//  ((wChar & 0xF0) && !(wChar & 0x08)) -> 4 bytes utf8 character

    // test for 5 character units for one utf8
    if ((pInString[0] & 0b11111000) && !(pInString[0] & 0b00000100))
            {
            *pUtf8Count=5;
            break;
            }//  ((wChar & 0xF0) && !(wChar & 0x08)) -> 5 bytes utf8 character  : this is invalid but taken for future use

    *pUtf8Count=1;
 //   *pUtf32CodePoint=pReplacement;
    return UST_ILLEGAL;
    }// while true

//-------------search for continuation byte(s)--------------------


    for(ssize_t wCi=1;wCi<(*pUtf8Count);wCi++) // checking everything is OK
    {
    if (!pInString[wCi]) // if endofstring mark has been found : utf8 character is truncated in middle
            return UST_TRUNCCONTBYTE;
    wC=pInString[wCi];   // get utf8 char unit content
//    pString++; // pointing to next utf8 char unit now
//    if ((wC & 0x80) && !(wC & 0x40)) //   first bit set-second bit must be zero     1000 0000
    if (utf8IsContByte(wC))
            {
            continue; // fine continue
            }
//            *pUtf32CodePoint = cst_Unicode_Replct_utf32 ;
//    *pUtf32CodePoint =pReplacement; // set character to current replacement character
    *pUtf8Count = wCi;              // return partial units count for errored char
    return UST_MISSCONTBYTE;
    }// for

    return UST_SUCCESS;
}//utf8CheckFormat
/**
 * @brief utf8CheckFormatChunk checks format and counts number of character units for the first encountered character of an utf8 string.<br>
 * Base routine inline used in all other utf8 routines.
 * @param pInString     utf8 string to parse
 * @param pUtf8Count    returned number of characters
 * @param pChunkSize    Optional : remaining string size for utf8 character to check starting at pInString
 * @return an UST_Status with the corresponding possible values :<br>
 * UST_SUCCESS      everything went well.<br>
 * UST_NULLPTR      string in input is null.<br>
 * UST_ENDOFSTRING  first character of string is endofstring mark OR pChunkSize is zero.<br>
 * UST_ILLEGAL      unicode encoding is wrong<br>
 * UST_TRUNCATEDCHAR expecting a following utf8 byte while string size is exhausted.<br>
 * UST_TRUNCCONTBYTE endofstring mark has been encountered while expecting a continuation utf8 byte.<br>
 * UST_MISSCONTBYTE  continuation byte is missing in the middle of an utf8 string.<br>
 */
inline UST_Status utf8CheckFormatChunk(const utf8_t* pInString, size_t* pUtf8Count,size_t* pChunkSize)
{
utf8_t wC;
    *pUtf8Count=0;
    wC=pInString[0];

    if (!pInString)
                return UST_NULLPTR;
    if (!pInString[0])
                return UST_ENDOFSTRING;

    if (pChunkSize)
            {
            if (*pChunkSize)
                return UST_ENDOFSTRING;
            }
    if (pInString[0]< 0x80)  // one byte character
            {
            *pUtf8Count=1;
            return UST_SUCCESS;
            }

/* check utf8 format */
    while (true)
    {
//    if (( wC & 0xC0 )&&!(wC & 0x20))
    if (( pInString[0] & 0b11000000 ) && !(pInString[0] & 0b00100000))
            {           //                        +---------> must be zero if this is header byte : 2 octets
                        //                        |   +-----> char data
                        //                        |---|--
                        // two bytes character  1100 0000
            *pUtf8Count=2;

            break;
                }//  (( wC & 0xC0 )&&!(wC & 0x20)) -> 2 bytes utf8 character

//    if (( wC & 0xE0 )&& !(wC & 0x10))
    if (( pInString[0] & 0b11100000 ) && !(pInString[0] & 0b00010000))
            {       //                          +---------> must be zero  must be zero if this is header byte : 3 octets
                    //                          |   +-----> char data
                    //                          | --|-
                    // three bytes character 1110 0000

            *pUtf8Count=3;
            break;
            }//  (( wChar & 0xE0 )&& !(wChar & 0x10)) -> 3 bytes utf8 character
//    if ((wC & 0xF0) && !(wC & 0x08))
    if ((pInString[0] & 0b11110000) && !(pInString[0] & 0b00001000))
            {         //                            +--------->  must be zero if this is header byte : 4 octets
                      //                            | +-----> char data
                      //                            |-|-
                      // four  bytes character 1111 0000

            *pUtf8Count=4;
            break;
            }//  ((wChar & 0xF0) && !(wChar & 0x08)) -> 4 bytes utf8 character

    // test for 5 character units for one utf8
    if ((pInString[0] & 0b11111000) && !(pInString[0] & 0b00000100))
            {
            *pUtf8Count=5;
            break;
            }//  ((wChar & 0xF0) && !(wChar & 0x08)) -> 5 bytes utf8 character  : this is invalid but taken for future use

    *pUtf8Count=1;
 //   *pUtf32CodePoint=pReplacement;
    return UST_ILLEGAL;
    }// while true

//-------------search for continuation byte(s)--------------------
    if (pChunkSize)
    {
    if ((*pChunkSize) < (*pUtf8Count))
                {
                return UST_TRUNCATEDCHAR;
                }
    }
    for(ssize_t wCi=1;wCi<(*pUtf8Count);wCi++) // checking everything is OK
    {
    if (!pInString[wCi]) // if endofstring mark has been found : utf8 character is truncated in middle
            return UST_TRUNCCONTBYTE;
    wC=pInString[wCi];   // get utf8 char unit content
//    pString++; // pointing to next utf8 char unit now
//    if ((wC & 0x80) && !(wC & 0x40)) //   first bit set-second bit must be zero     1000 0000
    if (utf8IsContByte(wC))
            {
            continue; // fine continue
            }
//            *pUtf32CodePoint = cst_Unicode_Replct_utf32 ;
//    *pUtf32CodePoint =pReplacement; // set character to current replacement character
    *pUtf8Count = wCi;              // return partial units count for errored char
    return UST_MISSCONTBYTE;
    }// for

    return UST_SUCCESS;
}//utf8CheckFormatChunk


UST_Status_type utf8Decode(const utf8_t *pInString,
                           utf32_t* pCodePoint,
                           size_t *pUtf8Count,
                           size_t* pRemainingSize)
{
    *pCodePoint = 0;

    if (pInString==nullptr)
        {
        *pUtf8Count=-1;
        return UST_NULLPTR;
        }

    if (pInString[0]==0)
        {
        *pUtf8Count=0;
        return UST_ENDOFSTRING; // not advancing string pointer to next char unit
        }

    *pUtf8Count=0;


    UST_Status wSt =utf8CheckFormatChunk(pInString,pUtf8Count,pRemainingSize);
    if (wSt!=UST_SUCCESS)
                {
                *pCodePoint=utf32Replacement;
                return wSt;
                }
   // get utf32 codepoint : source http://www.cplusplus.com/forum/general/31270/

    utf32_t wCodepoint;
    switch (*pUtf8Count)
    {
    case 1: // < 0x80
    {
        *pCodePoint = (utf32_t)pInString[0];  // return character content
        return UST_SUCCESS;
    }
    case 2:
    {
         wCodepoint=  ((pInString[0 ] & 0x1F) << 6) ;
         wCodepoint+= ((pInString[1] & 0x3F));
        break;
    }
    case 3:
    {
        wCodepoint=  ((pInString[0 ] & 0x0F) << 12) ;
        wCodepoint+= ((pInString[1] & 0x3F)<< 6);
        wCodepoint+= ((pInString[2] & 0x3F));
        break;
    }
    case 4:
    {
        wCodepoint=  ((pInString[0 ] & 0x07) << 18) ;
        wCodepoint+= ((pInString[1] & 0x3F)<< 12);
        wCodepoint+= ((pInString[2] & 0x3F)<< 6);
        wCodepoint+= ((pInString[3] & 0x3F));
        break;
    }
    case 5: /* this is not standard but may happen. so address it */
    {
        wCodepoint=  ((pInString[0] & 0x07) << 24) ;
        wCodepoint+= ((pInString[1] & 0x3F) << 18) ;
        wCodepoint+= ((pInString[2] & 0x3F)<< 12);
        wCodepoint+= ((pInString[3] & 0x3F)<< 6);
        wCodepoint+= ((pInString[4] & 0x3F));
        break;
    }
    }// switch

    *pCodePoint = wCodepoint;

    return UST_SUCCESS;
}// utf8Decode


/**
   * @brief _Utf8PreviousChar get previous utf8 character
   * @param pString
   * @param pChar
   * @param pStringLen
   * @return
   */
utf8_t*
_Utf8PreviousChar(utf8_t* pString,utf32_t &pChar,utfStrCtx* pContext)
{

size_t wLen=0;
utf8_t wChar;
int wIdx=0;
utf32_t wSplit[4];


    if (pContext==nullptr)
                    abort();
    if (!pContext->Status)
        {
        pContext->Reverse = true;
        pContext->InPtr=pString;
        pContext->Strlen=0;
        while (!*pContext->InPtr)
                            pContext->Strlen++;
        pContext->Start=pString+pContext->Strlen;
        }


    if (pContext->InPtr >= pString)
        {
        pContext->Status = UST_ENDOFSTRING;
        return nullptr;
        }

    while (pContext->InPtr >= pString)
      {
          wChar=*pContext->InPtr;
          if (wChar < 0x80)  // one byte character
                  {
                  pChar =(utf32_t) wChar ;
                  pContext->InPtr --;
                  pChar=pContext->CodePoint = (utf32_t)wChar;
                  pContext->Strlen--;
                  return pContext->InPtr;
                  }

          if (wChar & 0x80)           /*  1000 0000    -> 0x80 */
          {
          if (!(wChar & 0x40))        /*  0100 0000    -> 0x40  must be set to zero if continuation byte*/
                  {
                  if (wIdx==3)
                          return nullptr ; // no more than 3 bytes as continuation byte
                  wSplit[wIdx++] = (utf32_t)( wChar & ~ 0x80 ) ;  // continuation byte : 1000 0000
                  pContext->InPtr --;
                  pContext->Strlen--;
                  continue;
                  }

    // Here after must be  headers bytes

          if (!(wChar & 0x20))        /*  0010 0000    -> 0x20  must be set to zero if 2 continuation bytes */
                  {           //                        +---------> must be zero
                              //                        |   +-----> char data
                              //                        |---|--
                  wLen = 2;   // two bytes character  1100 0000
                  wSplit[wIdx++] = (utf32_t)( wChar & ~0xC0 );
                  break ;
                  }
          if (!(wChar & 0x10))           /*  0001 0000    -> 0x10 */
                  {         //                          +---------> must be zero !
                            //                          |   +-----> char data
                            //                          | --|-
                  wLen = 3; // three bytes character 1110 0000
                  wSplit[wIdx++] = (utf32_t)( wChar & ~0xE0 );
                  break;
                  }
          if (!(wChar & 0x08))           /*  0000 1000    -> 0x08 */
                  {         //                            +---------> must be zero !
                            //                            | +-----> char data
                            //                            |-|-
                  wLen = 4; // four  bytes character 1111 0000
                  wSplit[wIdx++] = (utf32_t)( wChar & ~0xF0 );
                  break;
                  }
          // invalid unicode codepoint
          pContext->Status=UST_IVCODEPOINT;
          return nullptr;
          } // if (wChar & 0x80)
      }// while


      if (wIdx!=wLen)
            {
            pContext->Status=UST_MISSCONTBYTE;
            return nullptr;  // incoherence
            }
      pChar=pContext->CodePoint=0;
      int wI=0;
      for (;wIdx>=0;wIdx--)
          {
          pChar=pContext->CodePoint += wSplit[wIdx]<<(wI*8);
          wI++;
          }
      pContext->InPtr --;
      pContext->Strlen--;
      return pContext->InPtr;
}// _Utf8PreviousChar

utf16_t*
_Utf16PreviousChar(utf16_t* pString,utf32_t &pChar,size_t* pStringLen,utfStrCtx* pContext)
{

size_t wLen=0;
utf8_t wChar;
int wIdx=0;
utf32_t wSplit[4];


    if (pContext==nullptr)
                    abort();
    if (!pContext->Status)
        {
        pContext->Reverse = true;
        pContext->setPosition(pString);
        pContext->Strlen=0;
        while (!*pContext->getPosition())
                            pContext->Strlen++;
        pContext->setStart(pString+pContext->Strlen);
        }
    /*     if(!pStringLen)
          while(*wStringChar++);
      else
          wStringChar+=*pStringLen;
    */

    if (pContext->utfGetPosition<utf16_t>() >= pString)
        {
        pContext->Status = UST_ENDOFSTRING;
        return nullptr;
        }

    while (pContext->InPtr >= (uint8_t*)pString)
      {
          wChar=*pContext->InPtr;
          if (wChar < 0x80)  // one byte character
                  {
                  pChar =(utf32_t) wChar ;
                  pContext->InPtr --;
                  pChar=pContext->CodePoint = (utf32_t)wChar;
                  pContext->Strlen--;
                  return pContext->utfGetPosition<utf16_t>();
                  }

          if (wChar & 0x80)           /*  1000 0000    -> 0x80 */
          {
          if (!(wChar & 0x40))        /*  0100 0000    -> 0x40  must be set to zero if continuation byte*/
                  {
                  if (wIdx==3)
                          return nullptr ; // no more than 3 bytes as continuation byte
                  wSplit[wIdx++] = (utf32_t)( wChar & ~ 0x80 ) ;  // continuation byte : 1000 0000
                  pContext->InPtr --;
                  pContext->Strlen--;
                  continue;
                  }

    // Here after must be  headers bytes

          if (!(wChar & 0x20))        /*  0010 0000    -> 0x20  must be set to zero if 2 continuation bytes */
                  {           //                        +---------> must be zero
                              //                        |   +-----> char data
                              //                        |---|--
                  wLen = 2;   // two bytes character  1100 0000
                  wSplit[wIdx++] = (utf32_t)( wChar & ~0xC0 );
                  break ;
                  }
          if (!(wChar & 0x10))           /*  0001 0000    -> 0x10 */
                  {         //                          +---------> must be zero !
                            //                          |   +-----> char data
                            //                          | --|-
                  wLen = 3; // three bytes character 1110 0000
                  wSplit[wIdx++] = (utf32_t)( wChar & ~0xE0 );
                  break;
                  }
          if (!(wChar & 0x08))           /*  0000 1000    -> 0x08 */
                  {         //                            +---------> must be zero !
                            //                            | +-----> char data
                            //                            |-|-
                  wLen = 4; // four  bytes character 1111 0000
                  wSplit[wIdx++] = (utf32_t)( wChar & ~0xF0 );
                  break;
                  }
          // invalid utf encoding
          pContext->Status=UST_IVCODEPOINT;
          return nullptr;
          } // if (wChar & 0x80)
      }// while


      if (wIdx!=wLen)
            {
            pContext->Status=UST_MISSCONTBYTE;
              return nullptr;  // incoherence
            }
      pChar=pContext->CodePoint=0;
      int wI=0;
      for (;wIdx>=0;wIdx--)
          {
          pChar=pContext->CodePoint += wSplit[wIdx]<<(wI*8);
          wI++;
          }
      pContext->InPtr --;
      pContext->Strlen--;
      return pContext->utfGetPosition<utf16_t>();
}// _Utf8PreviousChar

/**
 * @brief _Utf8reverseChar extracts reverse utf8 character
 * @param pString
 * @param pChar
 * @param pStringLen
 * @return
 */
utf8_t*
_Utf8reverseChar(utf8_t* pString,utf32_t &pChar,size_t* pStringLen)
{
utf8_t w4char = 0xF0; // 8+4+2+1 0      1111 0000
utf8_t w3char = 0xE0 ;// 8+4+2   0      1110 0000
utf8_t w2char = 0xC0; // 8+4     0      1100 0000

utf8_t wContd = 0x80; // 8       0      1000 0000

size_t wLen=0;
utf8_t* wStringChar=pString;
utf8_t wChar;
int wIdx=0;
utf32_t wSplit[4];

    if(!pStringLen)
        while(*wStringChar++);
    else
        wStringChar+=*pStringLen;



    while (wStringChar>=pString)
    {
        wChar=*wStringChar;
        if (wChar < 0x80)  // one byte character
                {
                pChar =(utf32_t) wChar ;
                wStringChar --;
                return wStringChar;
                }

        if (wChar & 0x80)           /*  1000 0000    -> 0x80 */
        {
        if (!(wChar & 0x40))        /*  0100 0000    -> 0x40 */
                {
                if (wIdx==3)
                        return nullptr ; // no more than 3 bytes as continuation byte
                wSplit[wIdx++] = (utf32_t)( wChar & ~ 0x80 ) ;  // continuation byte : 1000 0000
                wStringChar --;
                continue;
                }

        if (!(wChar & 0x20))        /*  0010 0000    -> 0x20 */
                {           //                        +---------> must be zero
                            //                        |   +-----> char data
                            //                        |---|--
                wLen = 2;   // two bytes character  1100 0000
                wSplit[wIdx++] = (utf32_t)( wChar & ~0xC0 );
                break ;
                }
        if (!(wChar & 0x10))           /*  0001 0000    -> 0x10 */
                {         //                          +---------> must be zero !
                          //                          |   +-----> char data
                          //                          | --|-
                wLen = 3; // three bytes character 1110 0000
                wSplit[wIdx++] = (utf32_t)( wChar & ~0xE0 );
                break;
                }
        if (!(wChar & 0x08))           /*  0000 1000    -> 0x08 */
                {         //                            +---------> must be zero !
                          //                            | +-----> char data
                          //                            |-|-
                wLen = 4; // four  bytes character 1111 0000
                wSplit[wIdx++] = (utf32_t)( wChar & ~0xF0 );
                break;
                }
        // invalid utf encoding
        return nullptr;
        } // if (wChar & 0x80)
    }// while

    if (wIdx!=wLen)
            return nullptr;  // incoherence
    pChar=0;
    int wI=0;
    for (;wIdx>=0;wIdx--)
        {
        pChar += wSplit[wIdx]<<(wI*8);
        wI++;
        }
    return wStringChar;

//-----first octet--------------------
    if (wChar<0x80)  // one byte character
        {
        pChar = wChar ;
        pString --;
        }

    if ((wChar&w4char)==w4char)  // expecting 4 bytes

    if ((wChar&w4char)==w3char)
    if ((wChar&w3char)==w2char)

    while (wStringChar>=pString)
    {
        while ((wStringChar>=pString)&&(0x80 & (*wStringChar)))
        {
        int wIdx = 0;
        if ((*wStringChar) & 0x80)
                {
                wSplit[wIdx]= ((*wStringChar) & ~(0x80));
                wIdx ++;
                }
        }

        wStringChar--;
    }

}//_Utf8reverseChar


UST_Status_type
utf8CharSize(const utf8_t *pInString,size_t*pUnitCount)
{
    return utf8CheckFormatChunk(pInString,pUnitCount,nullptr);
}
UST_Status_type
utf8CharSizeChunk(const utf8_t *pInString,size_t*pUnitCount,size_t pChunkSize)
{
    return utf8CheckFormatChunk(pInString,pUnitCount,&pChunkSize);
}
#ifdef __COMMENT__
UST_Status_type
utf8CharSize(const utf8_t *pUtf8,ssize_t*pUnitCount)
{
utf8_t  wMask;

      *pUnitCount=0;
      if (pUtf8 == nullptr)
          return UST_NULLPTR;
      if (pUtf8[0]==0)
          return UST_ENDOFSTRING;
    *pUnitCount=1;
    if (pUtf8[0] < 0x80)
            {
//            *pUnitCount=1;
            return UST_SUCCESS;
            }

      /* check valid UTF8 character */
      if (!(pUtf8[0] & 0x40))
          /* illegal utf8 value : find first next legal character : < 0x80 or pUtf8[wi] & 0x40 or EndofString mark*/
            {
            size_t wi = 1;

                while (wi< __STRING_MAX_LENGTH__)  // if EndOfString mark not found then pUnitCount will be -1
                    {
                    if ((!pUtf8[wi])||(pUtf8[wi]<0x80)||(pUtf8[wi]& 0x40))
                                                    *pUnitCount=(ssize_t)wi;  // return errored char units
                    wi++;
                    }

                /* character is ill formed and no valid character has been found as well as no endofstring mark has been found.*/
                 *pUnitCount=-1;
                return UST_STRINGOVERFLW;
            }//(!(pUtf8[0] & 0x40))

      /* determine number of bytes in char according first byte*/
      *pUnitCount = 2;
      for (wMask=0x20; wMask != 0; wMask>>=1)
        {
          if (!(pUtf8[0] & wMask))
                        return UST_SUCCESS;
          (*pUnitCount)++;
        }
      return UST_ILLEGAL;
}// utf8CharSize
#endif // __COMMENT__


/**
* @brief _Utf16CharSize computes size of uft16_t character, either 1 or 2 * utf16_t char depending whether there is surrogate characters or not
* @param pUtf16Char      utf16_t* string to scan
* @param plittleEndian   requested endianness for string to scan
 * @return an UST_Status_type as utfStatus_type<br>
 *  <b>UST_SUCCESS</b>         decoding went OK<br>
 *  <b>UST_ENDOFSTRING</b>    End of string mark has been found
 *
 *  <b>UST_IVUTF16SURR_1</b> Error : invalid surrogate first character<br>
 *  <b>UST_IVUTF16SURR_2</b> Error : invalid surrogate second character<br>
 *  <b>UST_TRUNCCONTBYTE</b> A first surrogate character unit has been found but endofstring mark is found before second surrogate character unit.
 *
  *  <b>UST_ILLEGAL</b>     Error : an utf16 character unit has been found with a value greater than 0xDFFF<br>
 *  <b>UST_NULLPTR</b>     Error: one required argument or more is nullptr<br>
*/
UST_Status_type
utf16CharSize(const utf16_t *pUtf16Char, size_t *pUnitCount, ZBool *plittleEndian)
{
bool wEndianProcess=false;  // by default endianness is not processed
bool wlittleEndianRequested = is_little_endian(); // take system endianness as default

    if (plittleEndian) // if not omitted
            {
           wEndianProcess= (*plittleEndian!=is_little_endian());
           wlittleEndianRequested=*plittleEndian;
            }

    if (pUtf16Char == NULL)
                      return UST_NULLPTR;

    *pUnitCount=0;

    if (pUtf16Char[0]==0)
            {
            return UST_ENDOFSTRING;
            }

    utf16_t wChar= pUtf16Char[0];
    if (wEndianProcess)
            wChar=forceReverseByteOrder<utf16_t>(wChar);
    if (wChar < 0xD800)
                    {
                    *pUnitCount=1;
                    return UST_SUCCESS;// non surrogate character : normal utf16_t
                    }

    /*  Utf16 ranges
     *      1rst surrogate D800 - D8FF
     *      2nd  surrogate DC00 - DFFF
     *
     *      since EOOO  : invalid utf16 codepoint
     */

    if (wChar > 0xDFFF)
             {
             *pUnitCount=1;
             return UST_ILLEGAL;
             }
          /* check valid UTF16 character */
    if (!(wChar & 0xD800))        // first
                        {
                        *pUnitCount=1; // updated to skip eventually errored character
                        return UST_IVUTF16SUR_1; // invalid surrogate first character
                        }

    utf16_t wChar2= pUtf16Char[1];

    if (!wChar2)  // if endofstring mark : truncated surrogate pair
        {
        *pUnitCount=1;
        return UST_TRUNCCONTBYTE;
        }
    if (wEndianProcess)
            wChar2=forceReverseByteOrder<utf16_t>(wChar2);
    if (!(wChar2 &0xDC00))     // second
                        {
                        *pUnitCount=2;  // updated to skip eventually errored character
                        return UST_IVUTF16SUR_2; // invalid surrogate second character
                        }

    *pUnitCount= 2;
    return UST_SUCCESS;
  }// _Utf16CharSize




UST_Status_type
utf16Decode(const utf16_t *pUtf16String, utf32_t* pCodePoint, size_t* pUnitCount, ZBool *plittleEndian)
{
utf16_t wChar16_1, wChar16_2;

bool wEndianProcess=false;  // by default endianness is not processed
bool wlittleEndianRequested = is_little_endian(); // take system endianness as default

    if (plittleEndian) // if not omitted
            {
           wEndianProcess= (*plittleEndian!=is_little_endian());
           wlittleEndianRequested=*plittleEndian;
            }
    if (pUtf16String == nullptr)
                      return UST_NULLPTR;
    if (!*pUtf16String)
                      return UST_ENDOFSTRING;

   *pCodePoint=0;

    wChar16_1= pUtf16String[0];
    if (wEndianProcess) // if we have to process endianness
                wChar16_1=forceReverseByteOrder<utf16_t>(wChar16_1); // do it

    if (wChar16_1 < (utf16_t)0xD800)// non surrogate character : normal unique utf16_t character
                    {
                    *pCodePoint = (utf32_t)wChar16_1;
                    *pUnitCount=1;
                    return UST_SUCCESS;
//                    return 1;
                    }
    /*  Utf16 ranges
     *      1rst surrogate D800 - D8FF
     *      2nd  surrogate DC00 - DFFF
     *
     *      since EOOO  : invalid utf16 codepoint
     */

    if (wChar16_1 > 0xDFFF)
             {
             *pUnitCount=1;
             return UST_ILLEGAL; // out of range
             }

    wChar16_2= pUtf16String[1];
    if (wEndianProcess)// if we have to process endianness
              wChar16_2=forceReverseByteOrder<utf16_t>(wChar16_2);// do it

    /* check valid UTF16 character */
    if (!(wChar16_1 & 0xD800))        // invalid first surrogate character
            {
            *pCodePoint = utf32Replacement;  // set resulting utf32 to unicode replacement character
            *pUnitCount=1;  // set size appropriately to skip bad utf16 character
            return UST_IVUTF16SUR_1;
//            return -1; // invalid surrogate first character
            }
    if (!(wChar16_2 &0xDC00))     // invalid second surrogate character
            {
            *pCodePoint = utf32Replacement;  // set resulting utf32 to unicode replacement character
            *pUnitCount=2;          // set size appropriately to skip bad utf16 character
            return UST_IVUTF16SUR_2;
            }

    utf32_t wChar32=0;

    wChar32 = 0x10000;
    wChar32 += (wChar16_1 & 0x03FF) << 10;
    wChar32 += (wChar16_2 & 0x03FF);
    *pCodePoint=wChar32;
    *pUnitCount=2;
    if (wChar32 >__UNICODE_MAX_CODEPOINT__) /* if resulting codepoint is illegal set to replacement */
                    {
                    *pCodePoint = utf32Replacement;
                    return UST_IVCODEPOINT;
                    }
    return UST_SUCCESS;
  }// utf16Decode

/**
 * @brief utf16NextCharValue gets the first utf16 character of the string pointed by pString and returns corresponding utf32 character.
 * @param[in] pString       pointer to utf16 string from which to read first character
 * @param[out] pUtf16Size   size in character units of current character
 * @param[out] pChar        utf32 codepoint
 * @param[out] pUSTStatus  an utfStatus_type describing operation status
 * @param[in] plittleEndian optional little endian parameter (true) or big endian (false) - omitted if nullptr
 * @return an UST_Status_type containing operation status.
 *
 *  UST_SUCCESS         decoding went OK
 *  UST_ENDOFSTRING     End of string mark has been found
 *  UST_BOMUT162BIG     found utf16 BOM big endian while big endian was requested : not an error - string content will follow
 *  UST_BOMUTF16LITTLE  found utf16 BOM little endian while little endian was requested : not an error - string content will follow
 *
 *  UST_IVBOMUTF16BIG   Error: found utf16 BOM big endian while little endian was requested
 *  UST_IVBOMUTF16LITTLE Error: found utf16 BOM little endian while big endian was requested
 */
UST_Status_type
utf16NextCharValue(const utf16_t* pString, size_t*pUtf16Size, utf32_t* pChar, ZBool* plittleEndian)
{
    int16_t wUSTStatus=UST_BOMUTF16;
    while (wUSTStatus&UST_BOMUTF16) // skip bom if correct
    {
        if((wUSTStatus=utf16Decode(pString,pChar,pUtf16Size,plittleEndian))<0)
                                                                        return wUSTStatus;
//        pString+=*pUtf16Size;
    }
    return wUSTStatus;
}


/**
* @brief utf16NextChar
* @param[in] pString        utf16 pointer to a string to convert
* @param[out] pUtf32        a pointer to the resulting character in utf32 format. Utf32 endianness is current system endianness.
* @param[in-out] pContext   current conversion context information. must be initialized to binary zero at first call-
* @param[in] pLittleEndian  a pointer to a boolean mentionning wether utf16 format is little endian (true) or big endian (false).
*                           if equals nullptr (omitted) then current system endianness is taken for converting utf16.
* @return  an utf16_t* pointing to next utf16 character in the string or nullptr if any error occurred.
*/
const utf16_t* utf16NextChar(const utf16_t* pString, utf32_t*pUtf32, utfStrCtx *pContext, bool* plittleEndian)
{
utf16_t wChar16_1,wChar16_2;
bool wProcessEndian=false;
bool wlittleEndianRequested=false;
    if (plittleEndian)
            {
            pContext->EndianRequest = true;
            wlittleEndianRequested=pContext->LittleEndian = *plittleEndian;
            wProcessEndian=*plittleEndian!=is_little_endian();
            }

    if (!pContext)
        {
        fprintf(stderr,"%s-S- pContext must point to a valid utfStrCtx\n",_GET_FUNCTION_NAME_);
        return nullptr;
        }
    if (!pString)
        {
        fprintf(stderr,"%s> pStringNextChar must point to an utf16* string\n",_GET_FUNCTION_NAME_);
        pContext->Status = UST_NULLPTR;
        return nullptr;
        }
    if (!pContext->Status)
        {
        pContext->reset();
         pContext->setStart(pString);
         pContext->setPosition(pString);
         const utf16_t* wString= pString;
         while(*wString++)
                     pContext->Strlen++;

         pContext->Status=UST_SUCCESS;
        }

    if (pContext->InPtr==pContext->Start)
    {
    utf8_t* wTestBom=(utf8_t*)pContext->InPtr;              // one utf8 char in string
    if (wTestBom[0]==cst_utf16BOMBE_Byte[0])
        {
        if (wTestBom[1]!=cst_utf16BOMBE_Byte[1])
            {
            pContext->Status=UST_IVBOMUTF16 ;
            return nullptr;
            }


        pContext->InPtr++;  // skip BOM for true string char
        }
        else
        if (wTestBom[0]==cst_utf16BOMLE_Byte[0])
            {
            if (wTestBom[1]!=cst_utf16BOMLE_Byte[1])
                {
                pContext->Status=UST_IVBOMUTF16 ;
                return nullptr;
                }
            // little endian requested

            pContext->InPtr++; // skip BOM for true string char
            }
    }

    pContext->CodePoint=0;

    if (!*pContext->InPtr)
        {
        pContext->Status =UST_ENDOFSTRING;
        *pUtf32=pContext->CodePoint = 0;
        return pContext->utfGetPosition<utf16_t>();
        }

    if (wProcessEndian)
            wChar16_1 = forceReverseByteOrder<utf16_t>(*pContext->utfGetPosition<utf16_t>());
            else
            wChar16_1= *pContext->utfGetPosition<utf16_t>();

    if (wChar16_1 < (utf16_t)0xD800)
                    {
                    *pUtf32=pContext->CodePoint= (utf32_t)wChar16_1;
                    pContext->InPtr++;
                    return pContext->utfGetPosition<utf16_t>();// non surrogate character : normal unique utf16_t character
                    }

    pContext->InPtr++;
    if (wProcessEndian)
            wChar16_2 = forceReverseByteOrder<utf16_t>(*pContext->InPtr);
            else
            wChar16_2= *pContext->InPtr;

    /* check valid UTF16 character */
    if (!(wChar16_1 & 0xD800))        // first
                {
                pContext->Status = UST_IVUTF16SUR_1;// invalid surrogate first character
                return nullptr;
    //                      return -2; // invalid surrogate first character
                }

    if (!(wChar16_2 &0xDC00))     // second
                {
                pContext->Status = UST_IVUTF16SUR_2;// invalid surrogate second character
                return nullptr;
    //                      return -3; // invalid surrogate second character
                }

    pContext->CodePoint=0;

    pContext->CodePoint = 0x10000;
    pContext->CodePoint += (wChar16_1 & 0x03FF) << 10;
    pContext->CodePoint += (wChar16_2 & 0x03FF);
    *pUtf32=pContext->CodePoint;
    pContext->InPtr ++;
    return pContext->utfGetPosition<utf16_t>();
}//utf16NextChar


/**
 * @brief utf16Encode encodes an utf32 character to utf16 unicode format
 * @param[out] pUtf16Result resulting utf16 string: must be an array of 3 * utf16_t. Last character unit is followed by an (utf16_t) zero.
 * @param[out] pUtf16Count utf16_t size (number of utf16_t character units) of the resulting utf16 character.
 *
 * @param[in] pUtf32Codepoint utf32 character codepoint to convert. This data must have current system endianness.
 * @param[in] plittleEndian Optional flag for endianness of the resulting utf16 format : if omitted, current endianness is used.

    pLittleEndian           system endianness       wSetLittleEndian    Comment
                            is_little_endian()

    omitted                 don't care              false = don't care  UTF16 or UTF32 is converted with system endianness

    true                    true                    false (as if omitted)

    true                    false                   true                Endianness conversion big -> little endian

    false                   true                    true                Endianness conversion little -> big endian

    false                   false                   false (as if omitted)


 * @return an UST_Status_type
 *  UST_SUCCESS if encoding went OK
 *  UST_IVCHARSET if utf32 codepoint value is outside of unicode codepoint range.
 *  In this case, pUtf16Count is set to 1 and pUtf16Result is set cst_Unicode_Replct_utf16
 */
UST_Status_type utf16Encode(utf16_t *pUtf16Result, size_t* pUtf16Count, utf32_t pCodePoint, ZBool *plittleEndian)
{
ZBool wEndianProcess=false;

    pUtf16Result[0]= 0;
    pUtf16Result[1]= 0;
    pUtf16Result[2]= 0;

    if (plittleEndian)
            wEndianProcess=(is_little_endian()!=*plittleEndian);

    if (pCodePoint < 0x10000)
        {
        if (wEndianProcess)
                pUtf16Result[0]=  forceReverseByteOrder<utf16_t>(pUtf16Result[0]);
            else
                *pUtf16Result = (utf16_t)pCodePoint;
        *pUtf16Count = 1;
        return UST_SUCCESS;
        }
    if (pCodePoint > __UNICODE_MAX_CODEPOINT__) /* if illegal codepoint set to replacement character */
        {
//        *pUtf16Count = 1;
        for (*pUtf16Count=0;(*pUtf16Count)<sizeof(utf16Replacement) && utf16Replacement[*pUtf16Count] ;(*pUtf16Count)++)
                                pUtf16Result[*pUtf16Count]=utf16Replacement[*pUtf16Count] ;
        return UST_IVCODEPOINT;
        }

// source  http://unicode.org/faq/utf_bom.html

// constants
    const utf32_t LEAD_OFFSET = 0xD800 - (0x10000 >> 10);
//    const utf32_t SURROGATE_OFFSET = 0x10000 - (0xD800 << 10) - 0xDC00;

    // computations
    utf16_t wLead = LEAD_OFFSET + (pCodePoint >> 10);
    utf16_t wTrail = 0xDC00 + (pCodePoint & 0x3FF);

    if (wEndianProcess)
        {
        wLead  =  forceReverseByteOrder<utf16_t>(wLead);
        wTrail =  forceReverseByteOrder<utf16_t>(wTrail);
        }

    pUtf16Result[0]=wLead;
    pUtf16Result[1]=wTrail;

    *pUtf16Count = 2;
    return UST_SUCCESS;
}// utf16Encode

UST_Status_type
utf32Decode(const utf32_t *pUtf32String, utf32_t* pCodePoint, size_t* pUtf32Size, ZBool *plittleEndian)
{

ZBool wEndianProcess=false;  // by default endianness is not processed
ZBool wlittleEndianRequested = is_little_endian(); // take system endianness as default

    if (plittleEndian) // if not omitted
            {
           wEndianProcess= (*plittleEndian!=is_little_endian());
           wlittleEndianRequested=*plittleEndian;
            }

    *pUtf32Size = 1; // always 1

    if (!pUtf32String)
                return UST_NULLPTR;
    if (!(*pUtf32String))
                return UST_ENDOFSTRING;

utf32_t wCodePoint = *pUtf32String;
UST_Status_type wSt=UST_SUCCESS;


    if (wEndianProcess)
                *pCodePoint = forceReverseByteOrder<utf32_t>(wCodePoint);
            else
                *pCodePoint= wCodePoint;

    if (wCodePoint > __UNICODE_MAX_CODEPOINT__ )
            {
            wCodePoint= utf32Replacement ;
            wSt=UST_IVCODEPOINT;
            }
    return wSt;
} // utf32Decode



UST_Status_type utf32Encode(utf32_t *pUtf32Result, utf32_t pCodePoint, ZBool *plittleEndian)
{
bool wEndianProcess=false;  // by default endianness is not processed : current system endianness is expected

    if (pCodePoint > __UNICODE_MAX_CODEPOINT__ )
            {
            *pUtf32Result= utf32Replacement ;
            return UST_IVCODEPOINT;
            }
    if (plittleEndian)
            wEndianProcess = (*plittleEndian!=is_little_endian());
    if (wEndianProcess)
                    *pUtf32Result = forceReverseByteOrder<utf32_t>(pCodePoint);
                else
                    *pUtf32Result= pCodePoint;

    return UST_SUCCESS;
}


bool utf32ToChar(char* pCharOut,const utf32_t pCharIn)
{
    if (pCharIn > 0xFF ) //@todo : must try to de-accute character
        {
        *pCharOut = __UTF_SUBSTITUTION_CHAR__;
        return false;
        }
    *pCharOut=(char) pCharIn;
    return true;
}

//#include <unicode/ucnv.h>
#include <ztoolset/utfconverter.h>
#include <ztoolset/utfvaryingstring.h>

ZStatus utf8StrToCharStr (char **pOutCharStr,
                         ZCharset_type pOutCharset,
                         const utf8_t* pInString,
                         const ssize_t pInByteSize,
                         ZBool pWarningSignal)
{
_MODULEINIT_
ZStatus wSt;
static UConverter* wUtf8Conv=nullptr;
static UConverter* wExtConv=nullptr;
    if (!pOutCharStr)
            _ABORT_
    if (!pInString)
            _ABORT_

    *pOutCharStr=nullptr;

ZCharset_type wCharset;
    if (pOutCharset==ZCHARSET_SYSTEMDEFAULT)
            wCharset=getDefaultCharset();
        else
            wCharset=pOutCharset;
    if (charsetUnitSize(wCharset) > sizeof(char))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVCHARSET,
                              Severity_Severe,
                              "Invalid charset <%s> for converting to char string - size of character unit is <%ld> \n",
                              decode_ZCharset(wCharset),
                              charsetUnitSize(wCharset));
        _RETURN_ ZS_INVCHARSET ;
        }
    const char* wOutName=decode_ZCharset(wCharset);
    if (!wOutName)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVCHARSET,
                              Severity_Severe,
                              "Unsupported charset value <%X> for converting to char string - size of character unit is <%ld> \n",
                              wCharset,
                              charsetUnitSize(wCharset));
        _RETURN_ ZS_INVCHARSET;
        }

    utf8Converter wutf8Cnv(true);

 //   return wutf8Cnv.toZCharset(pOutCharset,*pOutString,pOutSize,pInString,pInSize);


    ssize_t wInByteSize =pInByteSize;

    if (wInByteSize<0)
        {
        wInByteSize=0;
        while (pInString[wInByteSize++]) ;  // get strlen
        }

    // open (first call) or reset (subsequent calls) converters

    UErrorCode wErr=U_ZERO_ERROR;
    if (wUtf8Conv==nullptr)
            {
            wUtf8Conv=  ucnv_open("utf-8",&wErr);
            if ((wSt=ztestIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                             wErr,
                             pWarningSignal,
                             " while opening ICU  utf-8 converter for converting <%s> to utf8\n",
                             wOutName))<0) { _RETURN_ wSt;}
            }
        else
             ucnv_reset(wUtf8Conv);

    if (wExtConv==nullptr)
            {
            wExtConv=  ucnv_open(wOutName,&wErr);
            if ((wSt=ztestIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                             wErr,
                             pWarningSignal,
                             " while opening ICU  utf-8 converter for converting <%s> to utf8\n",
                             wOutName))<0) { _RETURN_ wSt;}
            }
        else
             ucnv_reset(wExtConv);


    // step 1- convert to utf16 pivot string

    utfStrCtx wContext;
    UConverterToUCallback wInOldCallBack;
    const void* wInOldContext;

    UChar* wUPivot= (UChar*)calloc(wInByteSize,sizeof(char16_t));
    UChar* wUActual=wUPivot, *wULimit=&wUPivot[wInByteSize-1];
    const utf8_t* wInPtr=pInString,*wInLimit=&pInString[wInByteSize-1];
    int32_t* wInOffsets= (int32_t*)calloc(wInByteSize,sizeof(int32_t));

    ucnv_setToUCallBack(wUtf8Conv,
                        &_toCallback,
                        &wContext,
                        &wInOldCallBack,
                        &wInOldContext,
                        &wErr
                        );

    ucnv_toUnicode(wUtf8Conv,
                   &wUActual,
                   wULimit,     // Out string boundary
                   (const char**)&wInPtr,
                   (const char*)wInLimit,    // pInString boundary
                   wInOffsets,    // get the set of corresponding offsets to source string (for fun)
                   true,        // true : last piece of chunk... we may stream in data with that
                   &wErr);

    if ((wSt=ztestIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     wErr,
                     pWarningSignal,
                     " while Converting ut8 string to unicode pivot\n",
                     wOutName))<0) { _RETURN_ wSt;}



    // step2 convert from utf16 pivot to destination charset

    utfStrCtx wContextOut;
    UConverterFromUCallback wOutOldCallBack;
    const void* wOutOldContext;

//    UChar* wUPivot= (UChar*)calloc(wInByteSize,sizeof(char16_t));
//    UChar* wUActual=wUPivot, *wULimit=&wUPivot[wInByteSize-1];
    *pOutCharStr=(char*)calloc(wInByteSize,sizeof(char));
    if(!*pOutCharStr)
            return ZS_MEMERROR;

    char* wOutPtr=*pOutCharStr,*wOutLimit=&(*pOutCharStr)[wInByteSize-1];
    int32_t* wOutOffsets= (int32_t*)calloc(wInByteSize,sizeof(int32_t));
    wUActual=wUPivot;  // reset position at top of wUPivot

    ucnv_setFromUCallBack(wExtConv,
                        &_fromCallback,
                        &wContextOut,
                        &wOutOldCallBack,
                        &wOutOldContext,
                        &wErr
                        );
//                  reset callbacks
    ucnv_fromUnicode(wExtConv,
                     &wOutPtr,
                     wOutLimit,    // OutString boundary
                     (const UChar**)&wUActual,
                     wULimit,     // Out string boundary
                     wOutOffsets,    // get the set of corresponding offsets to pivot string (for fun)
                     true,        // true : last piece of chunk... we may stream in data with that
                     &wErr);

    if ((wSt=ztestIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     wErr,
                     pWarningSignal,
                     " while Converting ut8 string to unicode pivot\n",
                     wOutName))<0) { _RETURN_ wSt;}

    //  free allocated memory

    _free(wInOffsets);
    _free(wOutOffsets);
    _free(wUPivot);

    // restore callbacks to their original state - or close them

    ucnv_setToUCallBack(wUtf8Conv,
                        wInOldCallBack,
                        &wInOldContext,
                        nullptr,
                        nullptr,
                        &wErr
                        );
    ucnv_setFromUCallBack(wExtConv,
                        wOutOldCallBack,
                        &wOutOldContext,
                        nullptr,
                        nullptr,
                        &wErr
                        );

/*    U_STABLE void U_EXPORT2
    ucnv_convertEx(UConverter *targetCnv, UConverter *sourceCnv,
                   char **target, const char *targetLimit,
                   const char **source, const char *sourceLimit,
                   UChar *pivotStart, UChar **pivotSource,
                   UChar **pivotTarget, const UChar *pivotLimit,
                   UBool reset, UBool flush,
                   UErrorCode *pErrorCode);*/

    wErr=U_ZERO_ERROR;

    int32_t wTheoLen= ucnv_fromAlgorithmic(wExtConv,       //get the output theorical length
                                           UCNV_UTF8,
                                           nullptr, (int32_t) 0,
                                           (const char*)pInString, (int32_t) pInByteSize,
                                           &wErr);
    if (testIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     wErr,
                     pWarningSignal,
                     " while converting utf-8 (icu algo built in) to charset <%s>\n",
                     wOutName)>0)
                        { _RETURN_ ZS_ICUERROR; }

    char* wOutString = (char*)malloc(wTheoLen);
    wTheoLen= ucnv_fromAlgorithmic(wExtConv,
                                   UCNV_UTF8,
                                   (char *)wOutString, (int32_t) wTheoLen,
                                   (const char*)pInString, (int32_t) pInByteSize,
                                   &wErr);
    ucnv_close(wExtConv);
    if (testIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     wErr,
                     true,
                     " while converting utf-8 (icu algo built in) to charset <%s>\n",
                     wOutName)>0){
                        free (wOutString);
                        _RETURN_ ZS_ICUERROR;
                        }
    *pOutCharStr=wOutString;
    _RETURN_ ZS_SUCCESS;
} // utf8StrToCharStr

ZStatus utf16StrToCharStr (char **pOutCharStr,
                                   ZCharset_type pOutCharset,
                                   const utf16_t* pInString,
                                   const ssize_t pInSize,
                                   ZBool pWarningSignal,
                                   ZBool* plittleEndian)
{
_MODULEINIT_

    if (!pOutCharStr)
            _ABORT_;
    if (!pInString)
            _ABORT_;

    *pOutCharStr=nullptr;

ZCharset_type wCharset;

    if (pOutCharset=ZCHARSET_SYSTEMDEFAULT)
            wCharset=getDefaultCharset();
        else
            wCharset=pOutCharset;
    if (charsetUnitSize(wCharset)>sizeof(char))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVCHARSET,
                              Severity_Severe,
                              "Invalid charset <%s> for converting to char string - size of character unit is <%ld> \n",
                                    decode_ZCharset(wCharset),
                                    charsetUnitSize(wCharset));
        _RETURN_ ZS_INVCHARSET;
        }
    const char* wOutName=decode_ZCharset(wCharset);
    if (!wOutName)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVCHARSET,
                              Severity_Severe,
                              "Invalid charset code <%X> for converting to char string - code is not recognized/managed. \n",
                                    wCharset);
        _RETURN_ ZS_INVCHARSET;
        }
    UConverterType wInputCnv=UCNV_UTF16_BigEndian;
    const char* wUCNV_Name="UCNV_UTF16_BigEndian";

    // if plittleEndian omitted and system default is littleEndian or plittleEndian is true

    if (((!plittleEndian)&&is_little_endian())||(*plittleEndian)){
                wInputCnv=UCNV_UTF16_LittleEndian;
                wUCNV_Name="UCNV_UTF16_LittleEndian";
                }
    UErrorCode wErr=U_ZERO_ERROR;
    UConverter* wConv=  ucnv_open(wOutName,&wErr);

    if (testIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     wErr,
                     true,
                     " while opening icu converter for charset <%s>\n",
                     wOutName) < UST_SEVERE) {_RETURN_ ZS_ICUERROR;}

    wErr=U_ZERO_ERROR;

    int32_t wTheoLen= ucnv_fromAlgorithmic(wConv,       //get the output theorical length
                                           wInputCnv,
                                           nullptr, (int32_t) 0,
                                           (const char*)pInString, (int32_t) pInSize,
                                           &wErr);
    if (testIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     wErr,
                     pWarningSignal,
                     " while converting <%s> (icu algo built in) to charset <%s>\n",
                     wUCNV_Name,
                     wOutName) < UST_SEVERE) { _RETURN_ ZS_ICUERROR;}
    char* wOutString = (char*)malloc(wTheoLen);
    if (!wOutString)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_MEMERROR,
                              Severity_Fatal,
                              "Cannot allocate memory size <%d>", wTheoLen);
        _RETURN_ ZS_MEMERROR;
        }
    wTheoLen= ucnv_fromAlgorithmic(wConv,
                                   wInputCnv,
                                   (char *)wOutString, (int32_t) wTheoLen,
                                   (const char*)pInString, (int32_t) pInSize,
                                   &wErr);
    ucnv_close(wConv);
    if (testIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     wErr,
                     pWarningSignal,
                     " while converting <%s> (icu algo built in) to charset <%s>\n",
                     wUCNV_Name,
                     wOutName) < UST_SEVERE){
                                            free (wOutString);
                                            _RETURN_ ZS_ICUERROR;
                                            }
    *pOutCharStr=wOutString;
_RETURN_ ZS_SUCCESS;
} // utf16StrToCStr

ZStatus utf32StrToCharStr (char **pOutCharStr,
                                   ZCharset_type pOutCharset,
                                   const utf32_t* pInString,
                                   const ssize_t  pInSize,
                                   ZBool pWarningSignal,
                                   ZBool *plittleEndian)
{
_MODULEINIT_

    if (!pOutCharStr)
            _ABORT_;
    if (!pInString)
            _ABORT_;

    *pOutCharStr=nullptr;

ZCharset_type wCharset;
    if (pOutCharset=ZCHARSET_SYSTEMDEFAULT)
            wCharset=getDefaultCharset();
        else
            wCharset=pOutCharset;
    if (charsetUnitSize(wCharset)>sizeof(char))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVCHARSET,
                              Severity_Severe,
                              "Invalid charset <%s> for converting to char string - size of character unit is <%ld> \n",
                                    decode_ZCharset(wCharset),
                                    charsetUnitSize(wCharset));
        _RETURN_ ZS_INVCHARSET;
        }
    const char* wOutName=decode_ZCharset(wCharset);
    if (!wOutName)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVCHARSET,
                              Severity_Severe,
                              "Invalid charset code <%X> for converting to char string - code is not recognized/managed. \n",
                                    wCharset);
        _RETURN_ ZS_INVCHARSET;
        }

    UConverterType wInputCnv=UCNV_UTF32_BigEndian;
    const char* wUCNV_Name="UCNV_UTF32_BigEndian";

    // if plittleEndian omitted and system default is littleEndian or plittleEndian is true

    if (((!plittleEndian)&&is_little_endian())||(*plittleEndian)){
                wInputCnv=UCNV_UTF32_LittleEndian;
                wUCNV_Name="UCNV_UTF32_LittleEndian";
                }

    UErrorCode wErr=U_ZERO_ERROR;
    UConverter* wConv=  ucnv_open(wOutName,&wErr);

    if (testIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     wErr,
                     pWarningSignal,
                     " while opening icu converter for charset <%s>\n",
                     wOutName) < UST_SEVERE)
                                {_RETURN_ ZS_ICUERROR;}

    wErr=U_ZERO_ERROR;

    int32_t wTheoLen= ucnv_fromAlgorithmic(wConv,       //get the output theorical length
                                           wInputCnv,
                                           nullptr, (int32_t) 0,
                                           (const char*)pInString,
                                           (int32_t) pInSize,
                                           &wErr);
    if (testIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     wErr,
                     pWarningSignal,
                     " while converting <%s> (icu algo built in) to charset <%s>\n",
                     wUCNV_Name,
                     wOutName) < UST_SEVERE) {_RETURN_ ZS_ICUERROR;}
    char* wOutString = (char*)malloc(wTheoLen);
    if (!wOutString)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_MEMERROR,
                              Severity_Fatal,
                              "Cannot allocate memory size <%d>", wTheoLen);
        _RETURN_ ZS_MEMERROR;
        }
    wTheoLen= ucnv_fromAlgorithmic(wConv,
                                   wInputCnv,
                                   (char *)wOutString, (int32_t) wTheoLen,
                                   (const char*)pInString, (int32_t) pInSize,
                                   &wErr);

    ucnv_close(wConv); // in any case close converter

    if (testIcuFatal(_GET_FUNCTION_NAME_,  // in case of error, raise this message in ZException
                     wErr,
                     pWarningSignal,
                     " while converting <%s> (icu algo built in) to charset <%s>\n",
                     wUCNV_Name,
                     wOutName) < UST_SEVERE){
                                            free (wOutString);
                                            _RETURN_ ZS_ICUERROR;
                                            }
    *pOutCharStr=wOutString;
    _RETURN_ ZS_SUCCESS;
} // utf32StrToCStr

/**
 * @brief utf8StrToCStr
 *  Allocates memory for a C string pointed by pCStr and tries to convert utf8 string pStrIn to char*.
 *  Allocated memory has to be freed by calling routine.
 *
 *
 *  returns an int containing utfStatus_type :
 *  if positive
 *      - UST_SUCCESS : everything OK
 *     If one or more characters have been substitued,
 *     then utfStrCtx::Substit has a non zero value representing the number of substitutions.
 *  if negative : see below possibilities
 *
 * @param pCStr
 * @param pStrIn
 * @param pContext Context of the parsing : if Substit field is not equal to zero, substitutions have been made
 * @param pErrored Optional number of substitutions made : not given if nullptr
 * @return an int with utfStatus_type value
 *         if positive CString has been generated
 *  UST_SUCCESS : everything went OK
 *
 *          if negative problems have been encountered
 *  UST_NULLPTR : mandatory argument is nullptr (missing) : pCStr, pStrIn or pContext
 *  UST_MEMERROR : cannot allocate memory for C String
 */
UST_Status_type utf8StrToCStr (char **pCStr, const utf8_t* pInString, utfStrCtx *pContext)
{
_MODULEINIT_

    if (!pCStr)
            _ABORT_;
    if (!pInString)
            _ABORT_;
    if (!pContext)
            _ABORT_;
    *pCStr=nullptr;

size_t wUtf8Count=0;
utf32_t wCodePoint;
size_t wCOffset=0;
const utf8_t* wPtr;
utfStrCtx wCountContext;

    pContext->reset();
    pContext->setStart(pInString);

    wPtr=detectUtf8Bom(pInString,&pContext->BOM);
    if ((pContext->BOM!=ZBOM_NoBOM)&&(pContext->BOM!=ZBOM_UTF8))
                                            return UST_IVBOMUTF8; // invalid BOM could have been detected
    pContext->setEffective(wPtr);

    pContext->Status=utf8CharCount(wPtr,&pContext->Strlen,&wCountContext);
    if (pContext->Status < UST_SEVERE)
                {
                pContext->InPtr=wCountContext.InPtr;
                pContext->Count=wCountContext.Count;
                pContext->CharUnits=wCountContext.CharUnits;
                pContext->ErrFrom=wCountContext.ErrFrom;
                return pContext->Status;
                }
    *pCStr=(char*)calloc(pContext->Strlen+1,sizeof(utf8_t));
    if (!*pCStr)
            {
            pContext->Status=UST_MEMERROR;
            return pContext->Status;
            }

//    pContext->partialInit(pInString);

    while (*wPtr)
        {
        pContext->setPosition(wPtr);

        pContext->Status = utf8Decode(wPtr,&wCodePoint,&wUtf8Count);
        if (pContext->Status==UST_ENDOFSTRING)
                                        break;
/*        if (pContext->Status==UST_BOMUTF8) // skip Valid BOM if any, without counting anything
                        {
                        wPtr += wUtf8Count;
                        continue;
                        }*/
        if (pContext->Status < 0)
            {
            pContext->ErrFrom ++;

            if ((pContext->StopOnConvErr) ||(pContext->Status < UST_SEVERE))  // if error is severe
                                {
                                (*pCStr)[wCOffset]=0;
                                return pContext->Status;
                                }
            (*pCStr)[wCOffset++]=__UTF_SUBSTITUTION_CHAR__;  // utf16 char is ill formed : substitution if StopOnConvErr is not set
            pContext->Substit ++;
            pContext->Count ++;
            pContext->CodePoint= __UTF_SUBSTITUTION_CHAR__;
            wPtr += wUtf8Count;
            continue;
            }

        if(!utf32ToChar(&(*pCStr)[wCOffset++],wCodePoint))
                                            pContext->Substit ++;
        pContext->CodePoint=(*pCStr)[wCOffset-1];
        pContext->Count ++;
        pContext->CharUnits += wUtf8Count;
        wPtr += wUtf8Count;
        }// while

    (*pCStr)[wCOffset]=0;
    return UST_SUCCESS;
} // utf8StrToCStr

/**
 * @brief utf16StrToCStr
 *  Allocates memory for a C string pointed by pCStr and tries to convert utf16 string pStrIn to char*.
 *  Allocated memory has to be freed by calling routine.
 *
 *
 *  returns an int containing utfStatus_type :
 *  if positive
 *      - UST_SUCCESS : everything OK
 *      - UST_SUBSTIT : one or more characters have been substitued
 *  if negative : see below possibilities
 *
 *
 *
 *
 * @param pCStr
 * @param pStrIn
 * @param pContext Context of the parsing
 * @param pErrored Optional number of substitutions made : not given if nullptr
 * @return an int with utfStatus_type value
 *         if positive CString has been generated
 *  UST_SUCCESS : everything went OK
 *  UST_SUBSTIT : one or more character(s) has been substitued with __UTF_SUBSTITUTION_CHAR__
 *          if negative problems have been encountered
 *  UST_NULLPTR : mandatory argument is nullptr (missing) : pCStr, pStrIn or pContext
 *  UST_MEMERROR : cannot allocate memory for C String
 *
 */
UST_Status_type utf16StrToCStr (char **pCStr, const utf16_t* pInString, utfStrCtx *pContext, ZBool *plittleEndian)
{
    pContext->reset();
    pContext->setStart(pInString);
    if (!pCStr)
            return UST_NULLPTR;
    *pCStr=nullptr;
    if (!pInString)
            return UST_NULLPTR;
    if (!pContext)
            {
            return UST_NULLPTR;
            }
    if (!*pInString)
            {
            pContext->Status=UST_EMPTY;
            return UST_EMPTY;
            }

size_t wUtf16Count=0;
utf32_t wUtf32Char;
size_t wCOffset=0;
UST_Status_type wSt=0;
const utf16_t* wPtr;
utfStrCtx wCountContext;


    wPtr=detectUtf16Bom(pInString,&pContext->BOM);
    pContext->setEffective(wPtr);

    if (pContext->BOM!=ZBOM_NoBOM)
        {
        bool wlittleEndian=is_little_endian();
        if (plittleEndian)
                wlittleEndian=*plittleEndian;
        switch (pContext->BOM)
        {
        case ZBOM_UTF16_BE:
            {
            if(wlittleEndian)
                    {
                    pContext->Status=UST_IVBOMUTF16BE;
                    return pContext->Status;
                    }
            break;
            }
        case ZBOM_UTF16_LE:
            {
            if(!wlittleEndian)
                    {
                    pContext->Status=UST_IVBOMUTF16LE;
                    return pContext->Status;
                    }
            break;
            }
        default:
            break;
        }
        }//(pContext->BOM!=ZBOM_NoBOM)

    pContext->Status=utf16CharCount(wPtr,&pContext->Strlen,&wCountContext,plittleEndian);
    if (pContext->Status < UST_SEVERE)
                {
                pContext->InPtr=wCountContext.InPtr;
                pContext->Status=wCountContext.Status;
                pContext->Count=wCountContext.Count;
                pContext->CharUnits=wCountContext.CharUnits;
                pContext->ErrFrom=wCountContext.ErrFrom;
                return wSt;
                }

    *pCStr=(char*)malloc(pContext->Strlen+1);
    if (!*pCStr)
            {
            pContext->Status=UST_MEMERROR;
            return UST_MEMERROR;
            }

//    pContext->partialInit(pInString);
    wPtr=pContext->utfGetEffective<utf16_t>();
    while (*wPtr)
        {
        pContext->setPosition(wPtr);

        pContext->Status = utf16Decode(wPtr,&wUtf32Char,&wUtf16Count,plittleEndian);
        pContext->CharUnits += wUtf16Count;
        if (pContext->Status==UST_ENDOFSTRING)
                                        break;
/*        if (((pContext->Status&UST_BOMUTF16)==UST_BOMUTF16)&&(pContext->Status > UST_SEVERE)) // skip Valid BOM if any, without counting anything
                        {
                        wPtr += wUtf16Count;
                        continue;
                        }*/
        if (pContext->Status < 0)
            {
            pContext->ErrFrom ++;

            if ((pContext->StopOnConvErr) ||(pContext->Status < UST_SEVERE))  // if error is severe
                                {
                                (*pCStr)[wCOffset]=0;
                                return pContext->Status;
                                }
            (*pCStr)[wCOffset++]=__UTF_SUBSTITUTION_CHAR__;  // utf16 char is ill formed : substitution if StopOnConvErr is not set
            pContext->Substit ++;
            pContext->CharUnits += wUtf16Count;
            wPtr += wUtf16Count;
            continue;
            }

        if(!utf32ToChar(&(*pCStr)[wCOffset++],wUtf32Char))
                                    {
                                    pContext->ErrTo ++;
                                    pContext->Substit ++;
                                    pContext->Count ++;
                                    }
        wPtr += wUtf16Count;
        pContext->CodePoint=wUtf32Char;
        }// while

    (*pCStr)[wCOffset]=0;

    return UST_SUCCESS;
} // utf16StrToCStr
/**
 * @brief utf16StrToCStr
 *  Allocates memory for a C string pointed by pCStr and tries to convert utf16 string pStrIn to char*.
 *  Allocated memory has to be freed by calling routine.
 *
 *
 *  returns an int containing utfStatus_type :
 *  if positive
 *      - UST_SUCCESS : everything OK
 *      - UST_SUBSTIT : one or more characters have been substitued
 *  if negative : see below possibilities
 *
 *
 *
 *
 * @param pCStr
 * @param pStrIn
 * @param pContext Context of the parsing
 * @param pErrored Optional number of substitutions made : not given if nullptr
 * @return an int with utfStatus_type value
 *         if positive CString has been generated
 *  UST_SUCCESS : everything went OK
 *  UST_SUBSTIT : one or more character(s) has been substitued with __UTF_SUBSTITUTION_CHAR__
 *          if negative problems have been encountered
 *  UST_NULLPTR : mandatory argument is nullptr (missing) : pCStr, pStrIn or pContext
 *  UST_MEMERROR : cannot allocate memory for C String
 *
 */
UST_Status_type utf32StrToCStr (char **pCStr, const utf32_t* pInString, utfStrCtx *pContext, ZBool *plittleEndian)
{
    pContext->reset();
    pContext->setStart(pInString);
    if (!pCStr)
        return UST_NULLPTR;
    *pCStr=nullptr;

    if (!pContext)
            return UST_NULLPTR;
    if (!pInString)
            return UST_NULLPTR;
    if (!*pInString)
            {
            pContext->Status=UST_EMPTY;
            return UST_EMPTY;
            }
size_t wUtf32Count=0;
utf32_t wCodePoint;
size_t wCOffset=0;

const utf32_t* wPtr;
utfStrCtx wCountContext;



    wPtr=detectUtf32Bom(pInString,&pContext->BOM);
    pContext->setEffective(wPtr);

    if (pContext->BOM!=ZBOM_NoBOM)
        {
        bool wlittleEndian=is_little_endian();
        if (plittleEndian)
                wlittleEndian=*plittleEndian;
        switch (pContext->BOM)
        {
        case ZBOM_UTF32_BE:
            {
            if(wlittleEndian)
                    {
                    pContext->Status=UST_IVBOMUTF32BE;
                    return pContext->Status;
                    }
            break;
            }
        case ZBOM_UTF32_LE:
            {
            if(!wlittleEndian)
                    {
                    pContext->Status=UST_IVBOMUTF32LE;
                    return pContext->Status;
                    }
            break;
            }
        default:
            break;
        }
        }//(pContext->BOM!=ZBOM_NoBOM)

    pContext->Status=utf32CharCount(wPtr,&pContext->Strlen,&wCountContext,plittleEndian);
    if (pContext->Status < UST_SEVERE)
                {
                pContext->InPtr=wCountContext.InPtr;
                pContext->Count=wCountContext.Count;
                pContext->CharUnits=wCountContext.CharUnits;
                pContext->ErrFrom=wCountContext.ErrFrom;
                return pContext->Status;
                }

    *pCStr=(char*)malloc(pContext->Strlen+1);
    if (!*pCStr)
            {
            pContext->Status=UST_MEMERROR;
            return pContext->Status;
            }
    wPtr=pContext->utfGetEffective<utf32_t>(); // in order to skip eventually BOM

//    pContext->partialInit(pInString);

    while (*wPtr)
        {
        pContext->setPosition(wPtr);
        pContext->Status = utf32Decode(wPtr,&wCodePoint,&wUtf32Count,plittleEndian);
        pContext->CharUnits += wUtf32Count;
        if (pContext->Status==UST_ENDOFSTRING)
                                        break;
 /*       if (((pContext->Status&UST_BOMUTF32)==UST_BOMUTF32)&&(pContext->Status > UST_SEVERE)) // skip Valid BOM if any, without counting anything
                        {
                        wPtr ++;
                        continue;
                        }*/
        if (pContext->Status < 0)
            {
            pContext->ErrFrom ++;

            if ((pContext->StopOnConvErr) ||(pContext->Status < UST_SEVERE))  // if error is severe
                                {
                                (*pCStr)[wCOffset]=0;
                                return pContext->Status;
                                }
            (*pCStr)[wCOffset++]=__UTF_SUBSTITUTION_CHAR__;  // utf16 char is ill formed : substitution if StopOnConvErr is not set
            pContext->Substit ++;
            pContext->Count ++;
            pContext->CodePoint=wCodePoint;
            wPtr ++;
            continue;
            }
        if(!utf32ToChar(&(*pCStr)[wCOffset++],wCodePoint))
                        {
                        pContext->ErrTo ++;
                        pContext->Substit ++;
                        pContext->Count ++;
                        }
        pContext->CodePoint=wCodePoint;
        pContext->CharUnits ++;
        *wPtr++;
        }
    (*pCStr)[wCOffset]=0;
    return UST_SUCCESS;
} // utf32StrToCStr
/**
 * @brief addUtf8Bom allocates memory and adds a leading utf8 BOM to given utf8_t* string.
 *  Returns the exact byte size of the string including '\0' end mark.
 * @param[in] pString the utf8 string to duplicate
 * @param[out] pByteSize exact number of bytes of the string after adding BOM
 * @return a newly allocated string with a leading utf8 BOM and duplicated from given utf8 string content
 */
utf8_t* addUtf8Bom(utf8_t*pString, size_t *pByteSize)
{
_MODULEINIT_
    size_t wSize=utfStrlen<utf8_t>(pString)+1;
    wSize=(wSize*sizeof(utf8_t))+2;
    *pByteSize=wSize;
    utf8_t* wNewString = (utf8_t*)malloc(wSize+2);
    utf8_t* wPtr=wNewString;
    if (!wNewString)
    {
        fprintf(stderr,"%s-F-CANTALLOC Fatal error: Cannot allocate memory size %ld.\n",
                _GET_FUNCTION_NAME_,
                wSize+2);
        _ABORT_
    }
    long wi=0;
    while (wi<sizeof(cst_utf8BOM))
            *wPtr++ = cst_utf8BOM[wi++];
//    wi=0;
    while (*pString)
        *wPtr++ = *pString++;
    *wPtr=(utf8_t)'\0';
    _RETURN_ wNewString;
}// addUtf8Bom
/**
 * @brief addUtf16Bom allocates memory and adds a leading utf16 BOM to given utf16_t* string.
 *  Returns the exact byte size of the string including '\0' end mark (and not the character units count)
 * @param[in pString the utf16 string to duplicate
 * @param pByteSize pByteSize exact number of bytes of the string after adding leading BOM
 * @param[in] plittleEndian optional parameter choosing what BOM to add : either cst_utf16BOMBE if false, or cst_utf16BOMLE if true.
 * if omitted (set to nullptr), then current system endianness is taken.
 * @return a newly allocated utf16 string with a leading utf16 BOM and duplicated from given utf16 string content
 * Indeed it is up to calling routine to free allocated memory.
 */
utf16_t* addUtf16Bom(utf16_t* pString, size_t*pByteSize, ZBool *plittleEndian)
{
_MODULEINIT_
    utf16_t* wBOM=(utf16_t*)cst_utf16BOMBE;
    if (plittleEndian)
        {
        if(*plittleEndian)
                wBOM=(utf16_t*)cst_utf16BOMLE;
        }
        else
        if (is_little_endian())
                wBOM=(utf16_t*)cst_utf16BOMLE;
    size_t wSize=utfStrlen<utf16_t>(pString)+2;  // counting BOM size (one utf16_t) plus '\0' mark as utf16_t
    *pByteSize=wSize*sizeof(utf16_t);
    utf16_t* wNewString = (utf16_t*)calloc(wSize,sizeof(utf16_t));
    utf16_t* wPtr=wNewString;
    if (!wNewString)
    {
        fprintf(stderr,"%s-F-CANTALLOC Fatal error: Cannot allocate memory size %ld utf16_t character units.\n",
                _GET_FUNCTION_NAME_,
                wSize);
        _ABORT_
    }
    *wPtr++= *wBOM;
    while (*pString)
        *wPtr++= *pString++;
    *wPtr=(utf16_t)'\0';
    _RETURN_ wNewString;
}// addUtf16Bom
/**
 * @brief addUtf32Bom allocates memory and adds a leading utf32 BOM to given utf32_t* string.
 *  Returns the exact byte size of the string including '\0' end mark (and not the character units count)
 * @param[in pString the utf32 string to duplicate
 * @param pByteSize pByteSize exact number of bytes of the string after adding leading BOM
 * @param[in] plittleEndian optional parameter choosing what BOM to add : either cst_utf32BOMBE if false, or cst_utf32BOMLE if true.
 * if omitted (set to nullptr), then current system endianness is taken.
 * @return a newly allocated utf32 string with a leading utf32 BOM and duplicated from given utf32 string content
 * Indeed it is up to calling routine to free allocated memory.
 */
utf32_t* addUtf32Bom(utf32_t* pString,size_t*pByteSize, ZBool* plittleEndian)
{
    _MODULEINIT_
        utf32_t* wBOM=(utf32_t*)cst_utf32BOMBE_Byte;
        if (plittleEndian)
            {
            if (*plittleEndian)
                            wBOM=(utf32_t*)cst_utf32BOMLE_Byte;
            }
        else
        if (is_little_endian())
                wBOM=(utf32_t*)cst_utf32BOMLE_Byte;
        size_t wSize=utfStrlen<utf32_t>(pString)+2;  // counting BOM size (one utf32_t) plus '\0' mark as utf32_t
        *pByteSize=wSize*sizeof(utf32_t);
        utf32_t* wNewString = (utf32_t*)calloc(wSize,sizeof(utf32_t));
        utf32_t* wPtr=wNewString;
        if (!wNewString)
        {
            fprintf(stderr,"%s-F-CANTALLOC Fatal error: Cannot allocate memory size %ld utf16_t character units.\n",
                    _GET_FUNCTION_NAME_,
                    wSize);
            _ABORT_
        }
        *wPtr++= *wBOM;
        while (*pString)
            *wPtr++ = *pString++;
        *wPtr=(utf32_t)'\0';
        _RETURN_ wNewString;
}// addUtf32Bom
/*================End copy==================================================*/
/**
* @brief detectUtf8Bom  detects if there is an utf8 signature (BOM) leading the string and returns the first byte of utf8 string without BOM
* @param pString utf8 input string
* @return the first byte of utf8 effective character stream
*/
const utf8_t *detectUtf8Bom(const utf8_t *pInString,ZBOM_type* pBOM)
{
    *pBOM=ZBOM_NoBOM;
    if (pInString==nullptr)
                  return nullptr;
    if (!pInString[0])
              return pInString;
    size_t wi=0;
    for  (; pInString[wi]&&(wi<sizeof(cst_utf8BOM));wi++)
            {
            if (pInString[wi]!=cst_utf8BOM[wi])
                {
                *pBOM=ZBOM_NoBOM;
                return pInString;
                }
            }
    *pBOM=ZBOM_UTF8;
    return pInString+wi;

}//detectUtf8Bom
/**
* @brief detectUtf16Bom detects if there is an utf16 signature (BOM) leading the string
*          - returns the first byte of utf8 string without BOM
*          - returns a boolean when positionned (true) mentions that format is little endian
*              Otherwise, default is big endian.
*
*  reminder : BOMs
*
*  byte order      0  1  2  3
*
*  utf8            EF BB BF --     no big/little endian
*
*  utf16           FE FF -- --     utf16 big endian (default for utf16)
*
*  utf16 LE        FF FE -- --     utf16 little endian
*
*  utf32           00 00 FE FF     utf32 big endian (default for utf32)
*
*  utf32 LE        FF FE 00 00     utf32 little endian
*
* @param pString utf16 input string with utf8 byte per byte acces
* @return
*/
const utf16_t* detectUtf16Bom(const utf16_t *pInString, ZBOM_type* pBOM)
{
_MODULEINIT_


  if (!pInString)
                    _ABORT_;
  if (!pBOM)
                    _ABORT_;

utf8_t* wPtr=(utf8_t*)pInString;

  *pBOM=ZBOM_NoBOM;
  if (!pInString[0])     // Warning : could be BOM for utf32 big endian
          return pInString;

  if (wPtr[0]== cst_utf16BOMBE_Byte[0])// utf16 big endian (default)
              {
              if (wPtr[1]== cst_utf16BOMBE_Byte[1])
                      {
                      *pBOM=ZBOM_UTF16_BE;
                      return &pInString[1];
                      }
              return pInString;//  no BOM detected : cannot be LE Bom
              }
  if (wPtr[0]== cst_utf16BOMLE_Byte[0])// utf16 big endian (default)
              {
              if (wPtr[1]== cst_utf16BOMLE_Byte[1])
                      {
                      *pBOM=ZBOM_UTF16_LE;
                      return &pInString[1];
                      }
              return pInString;
              }
  return pInString; // up to here no BOM detected
}//detectUtf16Bom
/**
* @brief _skipUtf32Bom detects if there is an utf16 signature (BOM) leading the string
*          - returns the first byte of utf8 string without BOM
*          - returns a boolean when positionned (true) mentions that format is little endian
*              Otherwise, default is big endian.
*
*  reminder : BOMs
*
*  byte order      0  1  2  3
*
*  utf8            EF BB BF --     no big/little endian
*
*  utf16           FE FF -- --     utf16 big endian (default for utf16)
*
*  utf16 LE        FF FE -- --     utf16 little endian
*
*  utf32           00 00 FE FF     utf32 big endian (default for utf32)
*
*  utf32 LE        FF FE 00 00     utf32 little endian
*
* @param pString utf16 input string
* @return
*/
const utf32_t* detectUtf32Bom(const utf32_t *pInString,ZBOM_type* pBOM)
{
_MODULEINIT_
  if (!pInString)
              _ABORT_;
  if (!pBOM)
              _ABORT_;

  *pBOM=ZBOM_NoBOM;
  if (!memcmp(pInString,cst_utf32BOMBE_Byte,4))
              {
              *pBOM=ZBOM_UTF32_BE;
              return &pInString[1];
              }
  if (!memcmp(pInString,cst_utf32BOMLE_Byte,4))
              {
              *pBOM=ZBOM_UTF32_LE;
              return &pInString[1];
              }
  return pInString; // up to here no BOM detected
}//_skipUtf32Bom



ZBOM_type
detectBOM(const void *pString,size_t pMinByteSize,size_t & pBOMSize)
{
      pBOMSize = 0;
      if (!pString)
            return ZBOM_NoBOM;
//-------------String size is at least 2------------------------
/*
*     utf16
*/
      if (pMinByteSize<2)
            return ZBOM_NoBOM;

      if (!memcmp(pString,&cst_utf16BOMBE,sizeof(cst_utf16BOMBE)))
        {
          pBOMSize=sizeof(cst_utf16BOMBE);
          return ZBOM_UTF16_BE;
        }
      if (!memcmp(pString,&cst_utf16BOMLE,sizeof(cst_utf16BOMLE))) // could confuse with utf32BOMLE
        {
          if (pMinByteSize>3)    // manage possible confusion with cst_utf32BOMLE
          {
          if (!memcmp(pString,cst_utf32BOMLE_Byte,sizeof(cst_utf32BOMLE_Byte)))
                {
                  pBOMSize=sizeof(cst_utf32BOMLE_Byte);
                  return ZBOM_UTF32_LE;
                }
          }
          pBOMSize=sizeof(cst_utf16BOMLE);
          return ZBOM_UTF16_LE;
        }
//-------------String size is at least 3------------------------
 /*
  *   utf1 - utf 8
  */
      if (pMinByteSize<3)
                return ZBOM_NoBOM;

      if (!memcmp(pString,cst_utf1BOM,sizeof(cst_utf1BOM)))
            {
              pBOMSize=sizeof(cst_utf1BOM);
              return ZBOM_UTF1;
            }

      if (!memcmp(pString,cst_utf8BOM,sizeof(cst_utf8BOM)))
            {
              pBOMSize=sizeof(cst_utf8BOM);
              return ZBOM_UTF8;
            }


//-------------String size is at least 4------------------------
/*
 *  utf32 - ebcdic -utf7
 */

      if (pMinByteSize<4)
                return ZBOM_NoBOM;

      if (!memcmp(pString,cst_utf32BOMBE_Byte,sizeof(cst_utf32BOMBE_Byte)))
            {
              pBOMSize=sizeof(cst_utf32BOMBE_Byte);
              return ZBOM_UTF32_BE;
            }

      if (!memcmp(pString,cst_utf32BOMLE_Byte,sizeof(cst_utf32BOMLE_Byte))) // see possible confusion with cst_utf16BOMLE
            {
              pBOMSize=sizeof(cst_utf32BOMLE_Byte);
              return ZBOM_UTF32_LE;
            }
    uint8_t* wStringU8=(uint8_t*)pString;
      if (!memcmp(pString,cst_utf7BOM1,sizeof(cst_utf7BOM1)-1)) // utf7 BOM have 4 ending variants
            {
            switch (wStringU8[3])
                {
                case 0x38 :
                case 0x39:
                case 0x2B:
                case 0x2F:
                  return ZBOM_UTF7;
                default:
                    fprintf(stderr,"%s>> Error: Detected start of utf7 BOM (first 3 bytes). Invalid 4th byte %x, expected 0x38, 0x39, 0x76 or 0x2F.\n",
                            _GET_FUNCTION_NAME_,
                            wStringU8[3]);
                    return ZBOM_BOMERROR;
                }
            }// if memcmp utf7BOM1

      if (!memcmp(pString,cst_utfEBCDIC,sizeof(cst_utfEBCDIC)))
            {
              pBOMSize=sizeof(cst_utfEBCDIC);
              return ZBOM_EBCDIC;
            }

    pBOMSize = 0;
    return ZBOM_NoBOM;

} // detectBOM




UST_Status_type utf8CharCount(const utf8_t* pInString, size_t  *pCount, utfStrCtx *pContext)
{
ssize_t wUnitCount=0;
const utf8_t* wInPtr=pInString;

    *pCount=0;
    if (!pContext)
          {
          return UST_NULLPTR;
          }
    pContext->reset();

    if (!pInString)
          {
          pContext->Status=UST_NULLPTR;
          return pContext->Status;
          }
    if (!*pInString)
          {
          pContext->Status=UST_EMPTY;
          return pContext->Status;
          }
    pContext->setStart(pInString);

    size_t wMinSize=0;
    while ((wMinSize<4)&&(pInString[wMinSize++]));

    size_t wBomSize=0;
    ZBOM_type wBOM= detectBOM(pInString,wMinSize,wBomSize);
    if ((wBOM!=ZBOM_NoBOM)&&(wBOM!=ZBOM_UTF8))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_IVBOM,
                                  Severity_Error,
                                  " invalid utf8 BOM encountered <%s>",
                                  decode_ZBOM(wBOM));
            pContext->Status=UST_IVBOM;
            return pContext->Status;
            }

    pContext->setEffective(pInString+wBomSize);
    wInPtr = pContext->utfGetEffective<utf8_t>();

    *pCount=0;

//--------count char size by char size : utf8 char may be single byte (general case for acii-us) or multi-byte

  while (true)
      {
      pContext->utfSetPosition<utf8_t>(wInPtr);
       pContext->Status=utf8CheckFormat(wInPtr,&wUnitCount);

//     pContext->Status=utf8CharSize(wInPtr,&wUnitCount);
      if (pContext->Status==UST_ENDOFSTRING)
                                      break;
      if (pContext->Status < 0)
          {
          pContext->ErrTo ++;

           if ((pContext->StopOnConvErr) ||(pContext->Status < UST_SEVERE))  // if error is severe
                              {
                              return pContext->Status; // exit with  return
                              }
          pContext->Substit++;//in any other case, should use replacement character (but not for counting)
          }
      pContext->Count++;
      pContext->CharUnits += wUnitCount;
      (*pCount) ++;
      wInPtr += wUnitCount;
      }// while

  pContext->Status= UST_SUCCESS;
  return pContext->Status;
}//utf8CharCount






UST_Status_type utf8CharCount(const utf8_t* pInString, size_t  *pCount)
{
ssize_t wUnitCount=0;
UST_Status_type wSt=UST_SUCCESS, wRetSt=UST_SUCCESS;

const utf8_t* wInPtr=pInString;

    *pCount=0;
    if (!pInString)
          {
          return UST_NULLPTR;
          }
    if (!*pInString)
          {
          return UST_EMPTY;
          }

    size_t wMinSize=0;
    while ((wMinSize<4)&&(pInString[wMinSize++]));

    size_t wBomSize=0;
    ZBOM_type wBOM= detectBOM(pInString,wMinSize,wBomSize);
    if ((wBOM!=ZBOM_NoBOM)&&(wBOM!=ZBOM_UTF8))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_IVBOM,
                                  Severity_Error,
                                  " invalid utf8 BOM encountered <%s>",
                                  decode_ZBOM(wBOM));
            return UST_IVBOM;
            }

    wInPtr = pInString+wBomSize;

    *pCount=0;

//--------count char size by char size : utf8 char may be single byte (general case for acii-us) or multi-byte

  while (true)
      {

       wSt=utf8CheckFormat(wInPtr,&wUnitCount);

       if (wSt==UST_ENDOFSTRING)
                                      break;
      if (wSt < 0)
          {
           if ((wSt < UST_SEVERE))  // if error is severe
                              {
                              return wSt;
                              }
          wRetSt=wSt;
          }
      (*pCount) += wUnitCount;
      wInPtr += wUnitCount;
      }// while

  return wRetSt;
}//utf8CharCount

UST_Status_type utf8CanonicalCount(const utf8_t* pInString, size_t  *pCount)
{
size_t wUnitCount=0;
UST_Status_type wSt=UST_SUCCESS, wRetSt=UST_SUCCESS;

const utf8_t* wInPtr=pInString;

    *pCount=0;
    if (!pInString)
          return UST_NULLPTR;

    if (!*pInString)
          return UST_EMPTY;

    size_t wMinSize=0;
    while ((wMinSize<4)&&(pInString[wMinSize++]));

    size_t wBomSize=0;
    ZBOM_type wBOM= detectBOM(pInString,wMinSize,wBomSize);
    if ((wBOM!=ZBOM_NoBOM)&&(wBOM!=ZBOM_UTF8))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_IVBOM,
                                  Severity_Error,
                                  " invalid utf8 BOM encountered <%s>",
                                  decode_ZBOM(wBOM));
            return UST_IVBOM;
            }

    wInPtr = pInString+wBomSize;
    *pCount=0;

//--------count char size by char size : utf8 char may be single byte (general case for acii-us) or multi-byte
utf32_t wCodePoint;
  while (true)
      {

       wSt=utf8Decode(wInPtr,&wCodePoint,&wUnitCount,nullptr);

       if (wSt==UST_ENDOFSTRING)
                                      break;
      if (wSt < 0)
          {
           if ((wSt < UST_SEVERE))  // if error is severe

          wRetSt=wSt;
          }

      (*pCount) ++;
      wInPtr += wUnitCount;
      }// while

  return wRetSt;
}//utf8CanonicalCount

/**
 * @brief utf8DiacriticalCharCount Counts canonical character number : characters affected with Unicode combining diacritical mark are counted for one character.<br>
 *
 * Errors management : if an invalid utf8 character is encountered, either UST_ILLEGAL or UST_INVALID
 *
 * @param pInString utf8_t encoded string
 * @param pCount    returned counted number
 * @return an UST_Status_type with a UST_Status value.<br>
 * UST_SUCCESS
 * UST_NULLPTR      pInString is null
 * UST_EMPTY        pInString is empty. Returned pCount is set to 0.
 * UST_IVBOM        a BOM has been detected that is not
 * UST_ENDOFSTRING
 *
 */
UST_Status_type utf8DiacriticalCharCount(const utf8_t* pInString, size_t  *pCount)
{
size_t wUnitCount=0;
UST_Status_type wSt=UST_SUCCESS, wRetSt=UST_SUCCESS;

const utf8_t* wInPtr=pInString;

    *pCount=0;
    if (!pInString)
          return UST_NULLPTR;

    if (!*pInString)
          return UST_EMPTY;

    size_t wMinSize=0;
    while ((wMinSize<4)&&(pInString[wMinSize++]));

    size_t wBomSize=0;
    ZBOM_type wBOM= detectBOM(pInString,wMinSize,wBomSize);
    if ((wBOM!=ZBOM_NoBOM)&&(wBOM!=ZBOM_UTF8))
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_IVBOM,
                                  Severity_Error,
                                  " invalid utf8 BOM encountered <%s>",
                                  decode_ZBOM(wBOM));
            return UST_IVBOM;
            }

    wInPtr = pInString+wBomSize;
    *pCount=0;

//--------count char size by char size : utf8 char may be single byte (general case for acii-us) or multi-byte
utf32_t wCodePoint;
  while (true)
      {

       wSt=utf8Decode(wInPtr,&wCodePoint,&wUnitCount,nullptr);

       if (wSt==UST_ENDOFSTRING)
                                      break;
      if (wSt < 0)
          {
           if ((wSt < UST_SEVERE))  // if error is severe

          wRetSt=wSt;
          }
      if (!isDiacritical(wCodePoint))
                      (*pCount) ++;
      wInPtr += wUnitCount;
      }// while

  return wRetSt;
}//utf8DiacriticalCharCount

/**
* @brief utf16CharCount gets the number of character of an utf16 string:
*                          - counting multi-byte character as one unique character
*                          - skipping header (little endian/big endian header) if any
* @param[in pString utf16 string to count characters from
* @param[out] pStatus an int containing an utfStatus_type value.
*                       if positive, everything went OK.
*               UST_SUCCESS if everything went OK
*               UST_NULLPTR if input string is nullptr
*               UST_ILLEGAL invalid utf16_t multi-byte character
* @param[out] pUtf8Effective  optional : effective start of utf8 string content, skipping BOM if any
*
* @return number of effective utf16 characters, single unit or multi-units without BOM if any
*       returns -1 if any error occurred
*/
UST_Status_type utf16CharCount(const utf16_t* pInString,
                               size_t *pCount,
                               utfStrCtx *pContext,
                               ZBool *plittleEndian)
{
size_t wUnitCount=0;

    if (plittleEndian)
            {
            pContext->EndianRequest = true;
            pContext->LittleEndian = *plittleEndian;
            }
        else
        {
        pContext->EndianRequest = false;
        pContext->LittleEndian=is_little_endian();
        }

    if (!pInString)
          {
          pContext->Status=UST_NULLPTR;
          return UST_NULLPTR;
          }
    pContext->setStart(pInString);

    *pCount=0;
    while(pInString[wUnitCount]&&(wUnitCount<5))
                                wUnitCount++;
    size_t wBOM_Size=0;
    ZBOM_type wBOM = pContext->LittleEndian?ZBOM_UTF16_LE:ZBOM_UTF16_BE;
    ZBOM_type wBOM1=detectBOM(pInString,wUnitCount,wBOM_Size);
    if ((wBOM1!=ZBOM_NoBOM)&&(wBOM1!=wBOM))
                                return UST_IVBOM;
    pInString+= wBOM_Size;
//--------count char size by char size : utf8 char may be single byte (general case for acii-us) or multi-byte

  while (true)
      {
      pContext->setPosition(pInString);
      pContext->Status=utf16CharSize(pInString,&wUnitCount,plittleEndian);
      if (pContext->Status==UST_ENDOFSTRING)
                                      break;
      if (pContext->Status < 0)
          {
          pContext->ErrFrom ++;

           if ((pContext->StopOnConvErr) ||(pContext->Status < UST_SEVERE))  // if error is severe
                              {
                              return pContext->Status; // exit with  return
                              }
          pContext->Substit++;//in any other case, should use replacement character (but not for counting)
          }
      pContext->Count++;
      pContext->CharUnits += wUnitCount;
      (*pCount) ++;
      pInString += wUnitCount;
      }// while

  pContext->Status= UST_SUCCESS;
  return pContext->Status;
}//utf16CharCount


UST_Status_type utf16CharCount(const utf16_t* pInString,size_t  *pCount, ZBool *plittleEndian)
{
size_t wUnitCount=0;
UST_Status_type wSt, wRetSt=UST_SUCCESS;
size_t wOffset=0,wMinLen=0,wBOMSize=0;
ZBool wEndianRequest,wLittleEndian;
const utf16_t* wInPtr;
    if (plittleEndian)
            {
            wEndianRequest = true;
            wLittleEndian = *plittleEndian;
            }
        else
            wLittleEndian=is_little_endian();

    if (!pInString)
          {
          return UST_NULLPTR;
          }

    while ((pInString[wMinLen])&&(wMinLen<5))
                                wMinLen++;


    ZBOM_type wBOM =wLittleEndian?ZBOM_UTF16_LE:ZBOM_UTF16_BE;
    wBOMSize=0;
    ZBOM_type wInBOM=detectBOM(pInString,wMinLen,wBOMSize);
    if ((wInBOM!=ZBOM_NoBOM)&&(wInBOM!=wBOM))
                            return UST_IVBOM;

    wInPtr=pInString+wBOMSize;
//--------count char size by char size : utf8 char may be single byte (general case for acii-us) or multi-byte
    *pCount=0;
    while (true)
      {
      wSt=utf16CharSize(wInPtr,&wUnitCount,plittleEndian);
      if (wSt==UST_ENDOFSTRING)
                                      break;
      if (wSt<0)
        {
          if (wSt<UST_SEVERE)
                    return wSt;
          wRetSt=wSt;
        }
      (*pCount) ++;
      wInPtr += wUnitCount;
      }// while

  return wRetSt;
}//utf16CharCount

UST_Status_type
utf16DiacriticalCharCount(const utf16_t* pInString,size_t  *pCount, ZBool *plittleEndian)
{
size_t wUnitCount=0;
UST_Status_type wSt, wRetSt=UST_SUCCESS;
size_t wOffset=0,wMinLen=0,wBOMSize=0;
ZBool wEndianRequest,wLittleEndian;
const utf16_t* wInPtr;
    if (plittleEndian)
            {
            wEndianRequest = true;
            wLittleEndian = *plittleEndian;
            }
        else
            wLittleEndian=is_little_endian();

    if (!pInString)
          {
          return UST_NULLPTR;
          }

    while ((pInString[wMinLen])&&(wMinLen<5))
                                wMinLen++;


    ZBOM_type wBOM =wLittleEndian?ZBOM_UTF16_LE:ZBOM_UTF16_BE;
    wBOMSize=0;
    ZBOM_type wInBOM=detectBOM(pInString,wMinLen,wBOMSize);
    if ((wInBOM!=ZBOM_NoBOM)&&(wInBOM!=wBOM))
                            return UST_IVBOM;

    wInPtr=pInString+wBOMSize;
    utf32_t wCodePoint;
//--------count char size by char size : utf8 char may be single byte (general case for acii-us) or multi-byte
    *pCount=0;
    while (true)
      {
      wSt=utf16Decode(wInPtr,&wCodePoint,&wUnitCount,plittleEndian);
      if (wSt==UST_ENDOFSTRING)
                                      break;
      if (wSt<0)
        {
          if (wSt<UST_SEVERE)
                    return wSt;
          wRetSt=wSt;
        }
      if (!isDiacritical(wCodePoint))
                              (*pCount) ++;
      wInPtr += wUnitCount;
      }// while

  return wRetSt;
}//utf16DiacriticalCharCount

UST_Status_type utf32CharCount(const utf32_t* pInString, size_t  *pCount, utfStrCtx *pContext, ZBool *plittleEndian)
{
bool wProcessEndian=false;
bool wlittleEndianRequested=is_little_endian();
    if (plittleEndian) // if not omitted
        {
        pContext->EndianRequest=true;
        wProcessEndian=*plittleEndian!=is_little_endian();
        wlittleEndianRequested=pContext->LittleEndian=*plittleEndian;
        }
    pContext->reset();

    if (!pInString)
          {
          pContext->Status=UST_NULLPTR;
          return UST_NULLPTR;
          }

    pContext->setStart(pInString);

    *pCount=0;

//--------count char size by char size : utf8 char may be single byte (general case for acii-us) or multi-byte

    pContext->setPosition(pInString);
    
    if (pInString[0]==cst_utf32BOMBE)
        {
        pContext->setEffective(pInString+1);
        pContext->BOM= ZBOM_UTF32_BE;
        if (wlittleEndianRequested)
                {
                pContext->Status = UST_IVBOMUTF32BE ;
                return pContext->Status;
                }
        }
    else
        {
        if (pInString[0]==cst_utf32BOMLE)
            {
            pContext->setEffective(pInString+1);
            pContext->BOM= ZBOM_UTF32_LE;
            if (!wlittleEndianRequested)
                    {
                    pContext->Status = UST_IVBOMUTF32LE ;
                    return pContext->Status;
                    }
            }
        }// else


    while (*pInString)
      {
      pContext->setPosition(pInString);
      
      if (pInString[0]>0x10FFFF)
          {
          pContext->ErrFrom ++;
          pContext->Status=UST_IVCODEPOINT;

          if (pContext->StopOnConvErr)  // if error is to be considered as severe
                              {
                              return pContext->Status; // exit with  return
                              }
          pContext->Substit++;//in any other case, should use replacement character (but not for counting)
          }

      pContext->Count++;
      pContext->CharUnits ++;
      (*pCount) ++;

      pInString ++;
      }// while

    pContext->Status= UST_SUCCESS;
    return pContext->Status;    
}//utf32CharCount

UST_Status_type utf32CharCount(const utf32_t* pInString, size_t  *pCount,  ZBool *plittleEndian)
{
bool wProcessEndian=false;
bool wlittleEndianRequested=is_little_endian();
    if (plittleEndian) // if not omitted
        {
        wProcessEndian=*plittleEndian!=is_little_endian();
        wlittleEndianRequested=*plittleEndian;
        }

    if (!pInString)
          {
          return UST_NULLPTR;
          }

    *pCount=0;

//--------count char size by char size : utf8 char may be single byte (general case for acii-us) or multi-byte

    if (pInString[0]==cst_utf32BOMBE)
        {
        if (wlittleEndianRequested)
                {
                return UST_IVBOMUTF32BE;
                }
        }
    else
        {
        if (pInString[0]==cst_utf32BOMLE)
            {
            if (!wlittleEndianRequested)
                    {
                    return UST_IVBOMUTF32LE;
                    }
            }
        }// else


    while (*pInString)
      {
      (*pCount) ++;

      pInString ++;
      }// while

    return UST_SUCCESS;
}//utf32CharCount

UST_Status_type utf32DiacriticalCharCount(const utf32_t* pInString, size_t  *pCount, ZBool *plittleEndian)
{
bool wProcessEndian=false;
bool wlittleEndianRequested=is_little_endian();
    if (plittleEndian) // if not omitted
        {
        wProcessEndian=*plittleEndian!=is_little_endian();
        wlittleEndianRequested=*plittleEndian;
        }

    if (!pInString)
          {
          return UST_NULLPTR;
          }


    *pCount=0;

//--------count char size by char size : utf8 char may be single byte (general case for acii-us) or multi-byte


    if (pInString[0]==cst_utf32BOMBE)
        {
        if (wlittleEndianRequested)
                {
                return UST_IVBOMUTF32BE;
                }
        }
    else
        {
        if (pInString[0]==cst_utf32BOMLE)
            {
            if (!wlittleEndianRequested)
                    {
                    return UST_IVBOMUTF32LE;
                    }
            }
        }// else


    while (*pInString)
      {
      if (!isDiacritical(pInString[0]))
              {
              (*pCount) ++;
              }
      pInString ++;
      }// while

    return UST_SUCCESS;
}//utf32DiacriticalCharCount



ZStatus iconvFromTo(const ZCharset_type pFromCharset,
                  const unsigned char* pInString,
                  size_t pInSize,
                  const ZCharset_type pToCharset,
                  ZDataBuffer* pOutString)
{
_MODULEINIT_

ZStatus wSt;
  const char* wfromCharset=decode_ZCharset(pFromCharset);
  const char* wtoCharset=decode_ZCharset(pToCharset);
  size_t wInSize=pInSize;

  ssize_t wCharCount= approxCharCountFromSize(pFromCharset,pInSize); // extrapolate number of char from input string/ encoding
  if (wCharCount<0)
          {
          ZException.addToLast(" while getting character count from input string");
          _RETURN_ (ZException.getLastStatus());
          }
  ssize_t wOutSize= approxSizeFromCharCount(pToCharset,wCharCount);
  if (wOutSize<0)
          {
          ZException.addToLast(" while getting char size from encoding for output string");
          _RETURN_ (ZException.getLastStatus());
          }

  pOutString->allocateBZero((size_t)(wOutSize));

  iconv_t wiconvCtx = iconv_open(wtoCharset,
                                 wfromCharset);
  if (wiconvCtx==(iconv_t)-1)
  {
      ZException.setMessageCplt(_GET_FUNCTION_NAME_,
                                ZS_ICONVINIT,
                                Severity_Severe,
                                "Cannot initialize charset conversion context (iconv_open) for converting from  <%s> to <%s>",
                                wfromCharset,
                                wtoCharset);
      switch(errno)
      {
      case EINVAL:
          {
          ZException.setComplement("The conversion specified from <%s> to <%s> is not supported by the implementation of libiconv.",
                                   wfromCharset, wtoCharset);
          break;
          }
      case ENFILE:
          {
          ZException.setComplement("Cannot allocate file descriptor : too much files open.");
          break;
          }
      case ENOMEM:
          {
          ZException.setComplement("Not enough disk storage available.");
          break;
          }
      case EMFILE:
          {
          ZException.setComplement("Cannot get a new file descriptor.Maximum files are currently open in the calling process. See FOPEN_FILE limit.");
          break;
          }
      }
      _RETURN_ ZS_ICONVINIT;
  }// if (wiconvCtx==(iconv_t)-1)

  size_t wC=0;

  while ((wC==0))
  {
  wC=iconv (wiconvCtx,(char **)&pInString,&wInSize,(char **)&pOutString->Data,&pOutString->Size);
  if (wC==(size_t)-1)
      {
      if (errno == E2BIG)
              {

              wCharCount= approxCharCountFromSize(pFromCharset,pInSize); // extrapolate number of char from input string/ encoding
              if (wCharCount<0)
                      {
                      ZException.addToLast(" while getting character count from input string");
                      return (ZS_INVVALUE);
                      }
              wOutSize= approxSizeFromCharCount(pToCharset,wCharCount);
              if (wOutSize<0)
                      {
                      ZException.addToLast(" while getting char size from encoding for output string");
                      return (ZS_INVVALUE);
                      }
              if (wOutSize==0)
                      wOutSize=charsetUnitSize(pToCharset);  // at least one character

              pOutString->extendBZero(wOutSize);
              continue;
              }//(errno == E2BIG)

      ZException.setMessageCplt(_GET_FUNCTION_NAME_,
                                ZS_ICONVSTOPPED,
                                Severity_Error,
                                "Icon charset conversion stopped while converting from <%s> to <%s> at character position <%ld> within input string",
                                pFromCharset,
                                pToCharset,
                                (pInSize-wInSize));
      wSt=ZS_ICONVSTOPPED;
      switch(errno)
      {
      case EILSEQ:
          {
          ZException.setComplement("Input conversion stopped due to an input byte that does not belong to the input codeset.");
          _RETURN_ wSt;
          }
/*       case E2BIG:
          {
          _RETURN_ ZS_NEEDMORESPACE;
          }*/
      case EINVAL:
          {
          ZException.setComplement("Input conversion stopped due to an incomplete character or shift sequence at the end of the input buffer.");
          _RETURN_ wSt;
          }
      case EBADF:
          {
          ZException.setComplement("Bad iconv context : The cd argument is not a valid open conversion descriptor.");
          _RETURN_ wSt;
          }
      }// switch(errno)
      }// if (wC==(size_t)-1)

      wSt=ZS_SUCCESS;
      break;
  }// while ((wC==0))

  iconv_close (wiconvCtx);

  _RETURN_ wSt;
}// iconvFromTo


ssize_t  approxCharCountFromSize(const ZCharset_type pCharset,size_t pInSize)
{
  ssize_t  wUnitSize=charsetUnitSize(pCharset) ;
  if (wUnitSize<1)
          return -1;
  return (pInSize/wUnitSize);
}
ssize_t  approxSizeFromCharCount(const ZCharset_type pCharset, size_t pCharCount)
{
  ssize_t  wUnitSize=charsetUnitSize(pCharset) ;
  if (wUnitSize<1)
          return -1;
  return (pCharCount*wUnitSize);
}

ZStatus
_fromCharset(unsigned char* pOutString,
           const size_t pOutSize,
           const ZCharset_type pOutCharset,
           const unsigned char* pInString,
           const size_t pInSize,
           const ZCharset_type pInCharset)
{
ZStatus wSt;
ZDataBuffer wZDB;
size_t wSize;
  wSt= iconvFromTo( pInCharset,
                    pInString,
                    pInSize,
                    pOutCharset,
                    &wZDB);
  if (wSt!=ZS_SUCCESS)
                  return wSt;
  wZDB.addConditionalTermination();
  wSize=wZDB.Size;
  if (wSize >= pOutSize)
                  {
                  wSize=pOutSize-2;     // must take care of final '\0'
                  wSt=ZS_FIELDCAPAOVFLW;
                  }
  pOutString[pOutSize]='\0';
  memmove(pOutString,wZDB.Data,wSize);
  return wSt;
}// _fromEncoded

template <size_t _Sz>
ZStatus
_fromISOLatin1(templateString<_Sz>& pOutString,const unsigned char* pInString, size_t pInSize)
{
_MODULEINIT_
ZStatus wSt;
ZDataBuffer wZDB;
size_t wSize;
  wSt= iconvFromTo( ZCHARSET_LATIN_1,
                    pInString,
                    pInSize,
                    pOutString.Charset,
                    wZDB);
  if (wSt!=ZS_SUCCESS)
                  _RETURN_ wSt;
  wZDB.addConditionalTermination();
  wSize=wZDB.Size;
  if (wSize >= pOutString._capacity)
                  {
                  wSize=pOutString._capacity-1;
                  wSt=ZS_FIELDCAPAOVFLW;
                  }
  pOutString.clear();
  pOutString=wZDB.DataChar;
  _RETURN_ wSt;
}// _fromISOLatin1

  template <size_t _Sz>
  ZStatus
_toISOLatin1(ZDataBuffer& pOutString,templateString<_Sz>& pInString)
{
_MODULEINIT_
const char* toConvCode="ISO8991-1";
ZStatus wSt;
ZDataBuffer wZDB;
size_t wSize;
  wSt= iconvFromTo(pInString.Charset ,
                   toConvCode,
                   (const unsigned char*)pInString.content,
                   pInString.size(),
                   wZDB);
  if (wSt!=ZS_SUCCESS)
                  _RETURN_ wSt;
  wZDB.addConditionalTermination();
  _RETURN_ wSt;
}// _toISOLatin1

template <size_t _Sz>
ZStatus
_fromGuessEncoding(templateString<_Sz>& pOutString,const unsigned char* pInString, size_t pInSize)
{
_MODULEINIT_
ZStatus wSt;
size_t wSize;
ZDataBuffer wZDB;
  uint16_t wGuess= guessZCharset(pInString,pInSize);

  wSt= iconvFromTo(decode_ZCharset( wGuess),
                   decode_ZCharset(pOutString.Charset),
                   pInString,
                   pInSize,
                   wZDB);
  if (wSt!=ZS_SUCCESS)
          _RETURN_ wSt;
  wSize=wZDB.Size;
  if (wSize > pOutString._capacity)
                  {
                  wSize=pOutString._capacity-1;
                  wSt=ZS_FIELDCAPAOVFLW;
                  }
  pOutString.clear();
  pOutString=wZDB.DataChar;
  _RETURN_ pOutString;
}// _fromGuessEncoding
template <size_t _Sz,size_t _Sz1>
ZStatus
_fromEncoded(templateString<_Sz>& pOutString, templateString<_Sz1>& pInString)
{
_MODULEINIT_
const char* wfromConvCode=decode_ZCharset(pInString.Charset);
const char* wtoConvCode=decode_ZCharset(pOutString.Charset);
ZStatus wSt;
ZDataBuffer wZDB;
size_t wSize;
  wSt= iconvFromTo(wfromConvCode ,
                   wtoConvCode,
                   (const unsigned char*)pInString.content,
                   pInString.size(),
                   wZDB);
  if (wSt!=ZS_SUCCESS)
                  _RETURN_ wSt;
  wZDB.addConditionalTermination();
  wSize=wZDB.Size;
  if (wSize > pOutString._capacity)
                  {
                  wSize=pOutString._capacity-1;
                  wSt=ZS_FIELDCAPAOVFLW;
                  }
  pOutString.clear();
  pOutString=wZDB.DataChar;
  _RETURN_ wSt;
}// _fromEncoded

template <size_t _Sz>
ZStatus
_fromZCharset(templateString<_Sz>& pOutString,const ZCharset_type pInCharset, const unsigned char* pInString, size_t pInSize)
{
_MODULEINIT_
const char* toConvCode="ISO8991-1";
ZStatus wSt;
ZDataBuffer wZDB;
size_t wSize;
  wSt= iconvFromTo(pInCharset ,
                   (const unsigned char*)pInString,
                   pInSize,
                   pOutString.Charset,
                   wZDB);
  if (wSt!=ZS_SUCCESS)
                  _RETURN_ wSt;
  wZDB.addConditionalTermination();
  wSize=wZDB.Size;
  if (wSize > pOutString._capacity)
                  {
                  wSize=pOutString._capacity-1;
                  wSt=ZS_FIELDCAPAOVFLW;
                  }
  pOutString.clear();
  strncpy(pOutString.content,wZDB.DataChar,wSize);
  _RETURN_ wSt;
}// _fromZCharset

#ifdef QT_CORE_LIB
/**
* @brief _fromQString  loads the content of given QString after having converted it to given ZCharset_type charset encoding (pCharset)
*
* @param pOutZDB
* @param pCharset
* @param pQString
* @return
*/
ZStatus
_fromQString(ZDataBuffer& pOutZDB,const ZCharset_type pCharset,QString pQString)
{
ZStatus wSt=ZS_SUCCESS;
ZCharsetBase wCharset = pCharset & (~ ZCHARSET_LITTLEENDIAN ); // get rid of endianness
  switch (wCharset)
  {
   case ZCHARSET_ASCII:
      {
      pOutZDB.setString(pQString.toStdString().c_str());
//        strncpy(content,pQString.toStdString().c_str(),sizeof(content)-1);
      break;
      }
  case ZCHARSET_LATIN_1:
      {
      pOutZDB.setString(pQString.toLatin1().data());
//        strncpy(content,pQString.toLatin1(),sizeof(content)-1);
      break;
      }

  case ZCHARSET_UTF8:
      {
      pOutZDB.setString(pQString.toUtf8().data());
//        strncpy(content,pQString.toUtf8(),sizeof(content)-1);
      break;
      }
  case ZCHARSET_UTF16:
      {
      pOutZDB.setUtf16((utf16_t*)pQString.utf16());
//        strncpy(content,pQString.toUtf8(),sizeof(content)-1);
      break;
      }
  case ZCHARSET_UCS4:
      {
      QVector<uint> wVector=pQString.toUcs4();
      pOutZDB.allocate(wVector.size());
      pOutZDB.setData(wVector.constData(),wVector.length());
//        strncpy(content,pQString.toUtf8(),sizeof(content)-1);
      break;
      }
  default:
      {
      ZDataBuffer wZDB;
      QString wQS=pQString.normalized(QString::NormalizationForm_KC);  // compatibility decompositon then canonical recomposition
//        wQBA=(char*)wQS.utf16();
      wSt=iconvFromTo(ZCHARSET_UTF16,
                      (const unsigned char *)wQS.utf16(),
                      wQS.size(),
                      pCharset,
                      &wZDB);
      }// default
  } // switch

  return wSt;
}// _fromQString
template <class _Utf>
ZStatus
utfFromQString(_Utf* pUtfString,const size_t pCapacity,const ZCharset_type pCharset,QString pQString)
{
ZStatus wSt=ZS_SUCCESS;
ZCharsetBase wCharset = pCharset & (~ ZCHARSET_LITTLEENDIAN ); // get rid of endianness
  switch (wCharset)
  {
   case ZCHARSET_ASCII:
      {
      utfnFromChar<_Utf>(pUtfString,pCapacity,pQString.toStdString().c_str());
//        strncpy(content,pQString.toStdString().c_str(),sizeof(content)-1);
      break;
      }
  case ZCHARSET_LATIN_1:
      {

      utfnFromChar<_Utf>(pUtfString,pCapacity,(const char*)pQString.toLatin1().data());
      break;
      }

  case ZCHARSET_UTF8:
      {
      utfStrncpy<_Utf>(pUtfString,pCapacity, (utf8_t*)pQString.toUtf8().data());
      break;
      }
  case ZCHARSET_UTF16:
      {
      utfStrncpy<_Utf>(pUtfString,pCapacity,(utf16_t*)pQString.utf16());
      break;
      }
  case ZCHARSET_UCS4:
      {
      QVector<uint> wVector=pQString.toUcs4();
      utfStrncpy<_Utf>(pUtfString,pCapacity,(utf32_t*)wVector);
      break;
      }
  case ZCHARSET_UTF32:
      {
      QVector<uint> wVector=pQString.toUcs4();
      utfStrncpy<_Utf>(pUtfString,pCapacity,(utf32_t*)wVector);
      break;
      }
  default:
      {
      ZDataBuffer wZDB;
      QString wQS=pQString.normalized(QString::NormalizationForm_KC);  // compatibility decompositon then canonical recomposition
      wSt=iconvFromTo(ZCHARSET_UTF16,
                      (const unsigned char *)wQS.utf16(),
                      wQS.size(),
                      pCharset,
                      &wZDB);
      }// default
  } // switch

  return wSt;
}// _fromQString

#endif // QT_CORE_LIB

  #include <ztoolset/encodedString.h>
  ZStatus
  encodedString::fromUtf8 ( utf8_t* pString)
  {
      ZBOM_type wBOM;
      const utf8_t* wutf8_Ptr=detectUtf8Bom(pString,&wBOM);
      if (wBOM!=ZBOM_UTF8)
                return ZS_INVCHARSET;

      return iconvFromTo(ZCHARSET_UTF8,
                         wutf8_Ptr,
                         utfStrlen(pString)+1,  // including 0 termination utf8_t character
                         Charset,
                         this);
  }

ZStatus
encodedString::fromUtf16 ( utf16_t* pInString,bool &pLittleEndian)
{
ZBOM_type wBOM;
  const utf16_t* wutf_Ptr=detectUtf16Bom((const utf16_t*)pInString,&wBOM);

  ZCharsetBase wCharset = ZCHARSET_UTF16;
  if (pLittleEndian)
          wCharset |= ZCHARSET_LITTLEENDIAN;

  return iconvFromTo((const ZCharset_type)wCharset,
                     (const unsigned char*)wutf_Ptr,
                     (utfStrlen(wutf_Ptr)+1)*sizeof(utf16_t),
                     Charset,
                     (_Base*)this);
}
ZStatus
encodedString::fromUtf32 ( ucs4_t* pString,bool &pLittleEndian)
{
ZBOM_type wBOM;
  const utf32_t* wutf_Ptr=detectUtf32Bom((const utf32_t*)pString,&wBOM);
  ZCharsetBase wCharset = ZCHARSET_UTF32;
  if (pLittleEndian)
          wCharset |= ZCHARSET_LITTLEENDIAN;
  size_t wInLen = (utfStrlen(wutf_Ptr)+1)*sizeof(ucs4_t); // get size in bytes : include trailing 0 value
  return iconvFromTo((const ZCharset_type)wCharset,
                     (const unsigned char*)wutf_Ptr,
                     wInLen,
                     Charset,
                     (_Base*)this);
}//fromUcs4



  utf8_t* getutf8BOM (void) {return (utf8_t*)cst_utf8BOM;}
  size_t getutf8BOMSize (void) {return sizeof(cst_utf8BOM);}
  utf16_t getutf16BOMBE (void) {return cst_utf16BOMBE;}
  size_t getutf16BOMSize (void) {return sizeof(cst_utf16BOMLE);}
  utf16_t getutf16BOMLE (void) {return cst_utf16BOMLE;}

  utf32_t getutf32BOMBE (void) {utf32_t wBom; memmove(&wBom,&cst_utf32BOMBE_Byte,4);return wBom;}
  utf32_t getutf32BOMLE (void) {utf32_t wBom; memmove(&wBom,&cst_utf32BOMLE_Byte,4);return wBom;}
  size_t getutf32BOMSize (void) {return sizeof(cst_utf32BOMLE_Byte);}

//--------------- code routines----------------------

  const char*
  decode_ZBOM(ZBOM_type pZBOM)
  {
      switch (pZBOM)
      {
      case ZBOM_NoBOM:
          return "ZBOM_NoBOM";
      case ZBOM_UTF8:
          return "ZBOM_UTF8";
      case ZBOM_UTF16_BE:
          return "ZBOM_UTF16_BE";
      case ZBOM_UTF16_LE:
          return "ZBOM_UTF16_LE";
      case ZBOM_UTF32_BE:
          return "ZBOM_UTF32_BE";
      case ZBOM_UTF32_LE:
          return "ZBOM_UTF32_LE";
      case ZBOM_UTF7:
          return "ZBOM_UTF7";
      case ZBOM_EBCDIC:
          return "ZBOM_EBCDIC";
      case ZBOM_UTF1:
          return "ZBOM_UTF1";
      case ZBOM_SCSU:
          return "ZBOM_SCSU";
      case ZBOM_BOCU_1:
          return "ZBOM_BOCU_1";
      case ZBOM_GB_18030:
          return "ZBOM_GB_18030";
      case ZBOM_BOMERROR:
          return "ZBOM_BOMERROR";
      default:
          return "ZBOM_BOMERROR";
      }
  }// decode_ZBOM

UConverterType ZCharsetToUcnv(ZCharset_type pCharset)
{
    for (size_t wi=0;GCharsetTable[wi].Name;wi++)
            if (GCharsetTable[wi].Charset==pCharset)
                            return GCharsetTable[wi].ICUCnvType;
    return UCNV_UNSUPPORTED_CONVERTER;

}//ZCharsetToUcnv




ZCharset_type
nameToZCharset (const char* pName)
{
    const char* wPtr=pName;
    size_t wLen=0;
    while (*wPtr++)
            wLen++;

    wLen++;
    char* wName=utfExpurgeSet(pName,"-_ .$");

    utfToUpper<char>(wName,nullptr);
// NB: ANSI_X3.4-1968 is US_ASCII
    if (strstr(wName,"ANSIX341968")||strstr(wName,"USASCII")){
                free(wName);
                return ZCHARSET_ASCII;}

    if (strstr(wName,"UTF7")){
                free(wName);
                return ZCHARSET_UTF7;}

    if (strstr(wName,"UTF8")){
                free(wName);
                return ZCHARSET_UTF8;}
    if (strstr(wName,"UTF16LE")){
                free(wName);
                return ZCHARSET_UTF16LE;}
    if ((strstr(wName,"UTF32BE")||(strstr(wName,"UCS4")))){
                free(wName);
                return ZCHARSET_UTF16BE;}
    if (strstr(wName,"UTF32LE")){
                free(wName);
                return ZCHARSET_UTF32LE;}
    if ((strstr(wName,"UTF32BE")||(strstr(wName,"UCS4")))){
                free(wName);
                return ZCHARSET_UTF32BE;}
    if (strstr(wName,"UTF32LE")){
                free(wName);
                return ZCHARSET_UTF32LE;}
    if ((strstr(wName,"ISO88591"))||(strstr(wName,"LATIN1"))){
                free(wName);
                return ZCHARSET_LATIN_1;}
    if ((strstr(wName,"ISO88592"))||(strstr(wName,"LATIN2"))){
                free(wName);
                return ZCHARSET_LATIN_2;}

    if ((strstr(wName,"ISO88593"))||(strstr(wName,"LATIN3"))){
                free(wName);
                return ZCHARSET_8859_3;}
    if (strstr(wName,"ISO88594")){
                free(wName);
                return ZCHARSET_8859_4;}
    if (strstr(wName,"ISO88595")){
                free(wName);
                return ZCHARSET_8859_5;}
    if (strstr(wName,"ISO88596")){
                free(wName);
                return ZCHARSET_8859_6;}
    if (strstr(wName,"ISO88597")){
                free(wName);
                return ZCHARSET_8859_7;}
    if (strstr(wName,"ISO88598")){
                free(wName);
                return ZCHARSET_8859_8;}
    if (strstr(wName,"ISO88599")){
                free(wName);
                return ZCHARSET_8859_9;}
    if (strstr(wName,"ISO885910")){
                free(wName);
                return ZCHARSET_NOTSUPPORTED;}
    if (strstr(wName,"ISO885911")){
                free(wName);
                return ZCHARSET_NOTSUPPORTED;}
    if (strstr(wName,"ISO885913")){
                free(wName);
                return ZCHARSET_NOTSUPPORTED;}
    if (strstr(wName,"ISO885914")){
                free(wName);
                return ZCHARSET_NOTSUPPORTED;}
    if (strstr(wName,"ISO885915")){
                free(wName);
                return ZCHARSET_NOTSUPPORTED;}
//
    if (strstr(wName,"IMAPMAILBOX")){
                free(wName);
                return ZCHARSET_IMAP_MAILBOX;}

    if (strstr(wName,"EBCDIC")){
                free(wName);
                return ZCHARSET_EBCDIC;}
//
    if (strstr(wName,"ISO2022JIS")){
                free(wName);
                return ZCHARSET_2022;}
    if (strstr(wName,"SHIFTJIS")){
                free(wName);
                return ZCHARSET_SHIFT_JIS;}
    if (strstr(wName,"HZ")){
                free(wName);
                return ZCHARSET_HZ;}
    if (strstr(wName,"BOCU1")){
                free(wName);
                return ZCHARSET_BOCU1;}
    if (strstr(wName,"CESU8")){
                free(wName);
                return ZCHARSET_CESU8;}
    if (strstr(wName,"SCSU")){
                free(wName);
                return ZCHARSET_SCSU;}
    if (strstr(wName,"ISCII")){
                free(wName);
                return ZCHARSET_ISCII;}

    return ZCHARSET_ERROR;
}

const char*
validateCharsetName (const char* pName)
{

    const char* wPtr=pName;
    size_t wLen=0;
    while (*wPtr++)
            wLen++;

    wLen++;
    char* wName=utfExpurgeSet(pName,"-_ .$");

    utfToUpper<char>(wName,nullptr);
// NB: ANSI_X3.4-1968 is US_ASCII
    if (strstr(wName,"ANSIX341968")||strstr(wName,"USASCII")){
                free(wName);
                return "US-ASCII";}

    if (strstr(wName,"UTF7")){
                free(wName);
                return "UTF7";}

    if (strstr(wName,"UTF8")){
                free(wName);
                return "UTF8";}
    if ((strstr(wName,"UTF16BE")||(strstr(wName,"UCS2")))){
                free(wName);
                return "UTF16BE";}
    if (strstr(wName,"UTF16LE")){
                free(wName);
                return "UTF16LE";}
    if (strstr(wName,"UTF32LE")){
                free(wName);
                return "UTF32LE";}
    if ((strstr(wName,"UTF32BE")||(strstr(wName,"UCS4")))){
                free(wName);
                return "UTF32BE";}
    if (strstr(wName,"UTF32LE")){
                free(wName);
                return "UTF32LE";}
    if ((strstr(wName,"ISO88591"))||(strstr(wName,"LATIN1"))){
                free(wName);
                return "ISO8859-1";}
    if ((strstr(wName,"ISO88592"))||(strstr(wName,"LATIN2"))){
                free(wName);
                return "ISO8859-2";}
    if ((strstr(wName,"ISO88593"))||(strstr(wName,"LATIN3"))){
                free(wName);
                return "ISO8859-3";}
    if (strstr(wName,"ISO88594")){
                free(wName);
                return "ISO8859-4";}
    if (strstr(wName,"ISO88595")){
                free(wName);
                return "ISO8859-5";}
    if (strstr(wName,"ISO88596")){
                free(wName);
                return "ISO8859-6";}
    if (strstr(wName,"ISO88597")){
                free(wName);
                return "ISO8859-7";}
    if (strstr(wName,"ISO88598")){
                free(wName);
                return "ISO8859-8";}
    if (strstr(wName,"ISO88599")){
                free(wName);
                return "ISO8859-9";}
    if (strstr(wName,"ISO885910")){
                free(wName);
                return "ISO8859-10";}
    if (strstr(wName,"ISO885911")){
                free(wName);
                return "ISO8859-11";}
    if (strstr(wName,"ISO885913")){
                free(wName);
                return "ISO8859-13";}
    if (strstr(wName,"ISO885914")){
                free(wName);
                return "ISO8859-14";}
    if (strstr(wName,"ISO885915")){
                free(wName);
                return "ISO8859-15";}
//
    if (strstr(wName,"IMAPMAILBOX")){
                free(wName);
                return "IMAPMAILBOX";}
//
    if (strstr(wName,"ISO2022JIS")){
                free(wName);
                return "ISO2022-JIS";}
    if (strstr(wName,"SHIFTJIS")){
                free(wName);
                return "SHIFT-JIS";}
    if (strstr(wName,"HZ")){
                free(wName);
                return "HZ";}
    if (strstr(wName,"BOCU1")){
                free(wName);
                return "BOCU1";}
    if (strstr(wName,"CESU8")){
                free(wName);
                return "CESU8";}
    if (strstr(wName,"SCSU")){
                free(wName);
                return "SCSU";}
    if (strstr(wName,"ISCII")){
                free(wName);
                return "ISCII";}

// if not found try ICU internal names

    UErrorCode wICUErr=U_ZERO_ERROR;

    char* wName1=utfFirstinSet<char>((char*)pName,"_ .$");
    if (wName1)
            wName1++;
        else
            wName1=(char*)pName;
    const char* wCanonicalName=ucnv_getCanonicalName(wName1,"IANA",&wICUErr);
    if (wCanonicalName)
            return wCanonicalName;
    UConverter* wCnv = ucnv_open(wName1,&wICUErr);
    wCanonicalName = ucnv_getName(wCnv,&wICUErr);
    ucnv_close(wCnv);
    return wCanonicalName;
}//validateCharsetName

ssize_t
charsetUnitSize(ZCharset_type pCharset)
{

    if (pCharset==ZCHARSET_SYSTEMDEFAULT)
            return charsetUnitSize(getDefaultCharset());
    for (size_t wi=0;GCharsetTable[wi].Name;wi++)
                    if (pCharset==GCharsetTable[wi].Charset)
                                    return (ssize_t)GCharsetTable[wi].UnitSize;
    return -1;

}//charsetUnitSize


//========================Single character Unicode conversion ========================

UST_Status_type
singleUtf8toUtf16 (utf16_t* pOutChar,size_t* pOutCount,
                   utf8_t*pInChar,size_t pInSize,
                   size_t* pCurrLen,size_t pMaxLength)
{
_MODULEINIT_
    utf32_t wCodePoint;
    size_t  wUtf8Count,wUtf16Count;

    utf16_t wUtf16Char[3]={0,0,0};
    if (utf8Decode(pInChar,&wCodePoint,&wUtf8Count,&pInSize) < UST_SEVERE)
        _ABORT_;

    if (utf16Encode((utf16_t*)wUtf16Char,&wUtf16Count,wCodePoint,nullptr) < UST_SEVERE)
                                                    {_ABORT_;}

    if (!pOutChar)  /* output buffer is null, so do not take care of pMaxLength and return length */
            {
            (*pCurrLen)+= wUtf16Count;
            _RETURN_ UST_SUCCESS;
            }

    if (((*pCurrLen) + wUtf16Count) >= pMaxLength)
                        {_RETURN_ UST_TRUNCATED;}

    *pOutCount=wUtf16Count;


    size_t wi=0;
    while (wUtf16Count--)
        {
        *pOutChar = wUtf16Char[wi];
        wi++;
        (*pCurrLen)++;
        }

    _RETURN_ UST_SUCCESS;
}//singleUtf8toUtf16

UST_Status_type
singleUtf8toUtf32 (utf32_t *pOutChar, size_t* pOutSize,
                   utf8_t*pInChar, size_t pInSize,
                   size_t* pCurrLen, size_t pMaxLength)
{
_MODULEINIT_
    utf32_t wCodePoint;
    size_t  wUtf8Count;
    if (!pOutChar)  /* output buffer is null, so do not take care of pMaxLength and return length */
            {
            (*pCurrLen)++;
            _RETURN_ UST_SUCCESS;
            }
    if (((*pCurrLen) + 1) >= pMaxLength)
                            {_RETURN_ UST_TRUNCATED;}
    if (utf8Decode(pInChar,pOutChar,&wUtf8Count,&pInSize) < UST_SEVERE)
                            _ABORT_;
    *pOutSize=1;
    pOutChar++;
    (*pCurrLen)++;
    _RETURN_ UST_SUCCESS;
}//singleUtf8toUtf32

UST_Status_type
singleUtf16toUtf8 (utf8_t* pOutChar,size_t* pOutCount,
                   utf16_t*pInChar,size_t pInSize,
                   size_t* pCurrLen,size_t pMaxLength)
{
_MODULEINIT_
    utf32_t wCodePoint;
    size_t  wUtf8Count;
    utf8_t wUtf8Char[5];
    if (utf16Decode(pInChar,&wCodePoint,&wUtf8Count,nullptr) < UST_SEVERE)
                            _ABORT_;

    if (utf8Encode(wUtf8Char,&wUtf8Count,wCodePoint,nullptr) < UST_SEVERE)
                            _ABORT_;
    if (!pOutChar)  /* output buffer is null, so do not take care of pMaxLength and return length */
            {
            (*pCurrLen)+= wUtf8Count;
            _RETURN_ UST_SUCCESS;
            }
    if (((*pCurrLen) + wUtf8Count) >= pMaxLength)
                                {_RETURN_ UST_TRUNCATED;}

    *pOutCount=wUtf8Count;
    size_t wi=0;
    while (wUtf8Count--)
        {
        pOutChar[*pCurrLen] = wUtf8Char[wi];
        wi++;
        (*pCurrLen)++;
        }
    _RETURN_ UST_SUCCESS;
}//singleUtf16toUtf8
UST_Status_type
singleUtf16toUtf32 (utf32_t* pOutChar,size_t* pOutSize,
                    utf16_t*pInChar,size_t pInSize,
                    size_t* pCurrLen,size_t pMaxLength)
{
_MODULEINIT_
    utf32_t wCodePoint;
    size_t  wUtf16Count;
    if (((*pCurrLen) + 1) >= pMaxLength)
                            {_RETURN_ UST_TRUNCATED;}
    if (!pOutChar)  /* output buffer is null, so do not take care of pMaxLength and return length */
            {
            (*pCurrLen)++;
            _RETURN_ UST_SUCCESS;
            }
    if (utf16Decode(pInChar,pOutChar,&wUtf16Count,nullptr) < UST_SEVERE)
                            _ABORT_;
    *pOutSize=1;
    (*pCurrLen)++;
    _RETURN_ UST_SUCCESS;
}//singleUtf16toUtf32
UST_Status_type
singleUtf32toUtf8 (utf8_t* pOutChar,size_t* pOutCount,
                   utf32_t*pInChar,size_t pInSize,
                   size_t* pCurrLen,size_t pMaxLength)
{
_MODULEINIT_
    utf32_t wCodePoint;
    size_t  wUtf32Count,wUtf8Count;
    utf8_t wUtf8Char[5];
    if (utf32Decode(pInChar,&wCodePoint,&wUtf32Count,nullptr) < UST_SEVERE)
                            _ABORT_;

    if (utf8Encode(wUtf8Char,&wUtf8Count,wCodePoint,nullptr) < UST_SEVERE)
                            _ABORT_;

    if (!pOutChar)  /* output buffer is null, so do not take care of pMaxLength and return length */
            {
            (*pCurrLen)+= wUtf8Count;
            _RETURN_ UST_SUCCESS;
            }
    if (((*pCurrLen) + wUtf8Count) >= pMaxLength)
                        {_RETURN_ UST_TRUNCATED;}

    *pOutCount=wUtf8Count;
    size_t wi=0;
    while (wUtf8Count--)
        {
        pOutChar[*pCurrLen] = wUtf8Char[wi];
        wi++;
        (*pCurrLen)++;
        }

    _RETURN_ UST_SUCCESS;
}//singleUtf32toUtf8
UST_Status_type
singleUtf32toUtf16 (utf16_t *pOutChar, size_t* pOutCount,
                    utf32_t pInChar, size_t pInSize,
                    size_t* pCurrLen, size_t pMaxLength)
{
_MODULEINIT_
    utf32_t wCodePoint;
    size_t  wUtf16Count;
    utf16_t wUtf16Char[3];
    if (utf16Encode(wUtf16Char,&wUtf16Count,wCodePoint,nullptr) < UST_SEVERE)
                            _ABORT_;
    if (!pOutChar)  /* output buffer is null, so do not take care of pMaxLength and return length */
            {
            (*pCurrLen)+= wUtf16Count;
            _RETURN_ UST_SUCCESS;
            }
    if (((*pCurrLen) + wUtf16Count) >= pMaxLength)
                        {_RETURN_ UST_TRUNCATED;}
    size_t wi=0;
    while (wUtf16Count--)
        {
        pOutChar[*pCurrLen] = wUtf16Char[wi];
        wi++;
        (*pCurrLen)++;
        }
    *pOutCount=wUtf16Count;
    _RETURN_ UST_SUCCESS;
}//singleUtf32toUtf16





ZCharset_type getZCharsetFromXMLEncoding(int8_t pXMLCode)
{
  switch (pXMLCode)
  {
  case XML_CHAR_ENCODING_ASCII:
      {
      return ZCHARSET_ASCII;
      }
  case XML_CHAR_ENCODING_UTF8:
      {
      return ZCHARSET_UTF8;
      }
  case XML_CHAR_ENCODING_UTF16LE:
      {
      return ZCHARSET_ASCII;
      }
  case XML_CHAR_ENCODING_UTF16BE:
      {
      return ZCHARSET_ASCII;
      }
  case XML_CHAR_ENCODING_UCS4LE:
      {
      return ZCHARSET_UCS4LE;
      }
  case XML_CHAR_ENCODING_UCS4BE:
      {
      return ZCHARSET_UCS4BE;
      }
  case XML_CHAR_ENCODING_EBCDIC:
      {
      return ZCHARSET_EBCDIC;
      }
  case XML_CHAR_ENCODING_UCS4_2143:
      {
      return ZCHARSET_ERROR;
      }
  case XML_CHAR_ENCODING_UCS4_3412:
      {
      return ZCHARSET_ERROR;
      }
  case XML_CHAR_ENCODING_UCS2:
      {
      return ZCHARSET_UCS2BE;
      }
  case XML_CHAR_ENCODING_8859_1:
      {
      return ZCHARSET_8859_1;
      }
  case XML_CHAR_ENCODING_8859_2:
      {
      return ZCHARSET_8859_2;
      }
  case XML_CHAR_ENCODING_8859_3:
      {
      return ZCHARSET_8859_3;
      }
  case XML_CHAR_ENCODING_8859_4:
      {
      return ZCHARSET_8859_4;
      }
  case XML_CHAR_ENCODING_8859_5:
      {
      return ZCHARSET_8859_5;
      }
  case XML_CHAR_ENCODING_8859_6:
      {
      return ZCHARSET_8859_6;
      }
  case XML_CHAR_ENCODING_8859_7:
      {
      return ZCHARSET_8859_7;
      }
  case XML_CHAR_ENCODING_8859_8:
      {
      return ZCHARSET_8859_8;
      }
  case XML_CHAR_ENCODING_8859_9:
      {
      return ZCHARSET_8859_9;
      }
  case XML_CHAR_ENCODING_2022_JP:
      {
      return ZCHARSET_2022;
      }
  case XML_CHAR_ENCODING_SHIFT_JIS:
      {
      return ZCHARSET_SHIFT_JIS;
      }
  case XML_CHAR_ENCODING_EUC_JP:
      {
      return ZCHARSET_EUC_JP;
      }
  case XML_CHAR_ENCODING_ERROR:
      {
      return ZCHARSET_ERROR;
      }
  case XML_CHAR_ENCODING_NONE:
      {
      return ZCHARSET_SYSTEMDEFAULT;
      }
  default:
      return ZCHARSET_ERROR;
  }
}// getZCharFromXMLCode

  int8_t getXMLEncodingFromZCharset(ZCharset_type pZCharCode)
  {
      switch (pZCharCode)
      {
      case ZCHARSET_ASCII:
          {
          return XML_CHAR_ENCODING_ASCII;
          }
      case ZCHARSET_UTF8:
          {
          return XML_CHAR_ENCODING_UTF8;
          }
      case ZCHARSET_UTF16LE:
          {
          return XML_CHAR_ENCODING_ASCII;
          }
      case ZCHARSET_UTF16BE:
          {
          return XML_CHAR_ENCODING_ASCII;
          }
      case ZCHARSET_UCS4LE:
          {
          return XML_CHAR_ENCODING_UCS4LE;
          }
      case ZCHARSET_UCS4BE:
          {
          return XML_CHAR_ENCODING_UCS4BE;
          }
      case ZCHARSET_EBCDIC:
          {
          return XML_CHAR_ENCODING_EBCDIC;
          }
   /*   case ZCHARSET_UCS4_2143:
          {
          return XML_CHAR_ENCODING_ERROR;
          }
      case ZCHARSET_UCS4_3412:
          {
          return XML_CHAR_ENCODING_ERROR;
          }
      case ZCHARSET_UCS2LE:
          {
          return XML_CHAR_ENCODING_UCS2;
          }
      case ZCHARSET_UCS2BE:
          {
          return XML_CHAR_ENCODING_UCS2;
          }*/
      case ZCHARSET_8859_1:
          {
          return XML_CHAR_ENCODING_8859_1;
          }
      case ZCHARSET_8859_2:
          {
          return XML_CHAR_ENCODING_8859_2;
          }
      case ZCHARSET_8859_3:
          {
          return XML_CHAR_ENCODING_8859_3;
          }
      case ZCHARSET_8859_4:
          {
          return XML_CHAR_ENCODING_8859_4;
          }
      case ZCHARSET_8859_5:
          {
          return XML_CHAR_ENCODING_8859_5;
          }
      case ZCHARSET_8859_6:
          {
          return XML_CHAR_ENCODING_8859_6;
          }
      case ZCHARSET_8859_7:
          {
          return XML_CHAR_ENCODING_8859_7;
          }
      case ZCHARSET_8859_8:
          {
          return XML_CHAR_ENCODING_8859_8;
          }
      case ZCHARSET_8859_9:
          {
          return XML_CHAR_ENCODING_8859_9;
          }
      case ZCHARSET_2022:
          {
          return XML_CHAR_ENCODING_2022_JP;
          }
      case ZCHARSET_SHIFT_JIS:
          {
          return XML_CHAR_ENCODING_SHIFT_JIS;
          }
      case ZCHARSET_EUC_JP:
          {
          return XML_CHAR_ENCODING_EUC_JP;
          }
      case ZCHARSET_ERROR:
          {
          return XML_CHAR_ENCODING_ERROR;
          }
      case ZCHARSET_SYSTEMDEFAULT:
          {
          return XML_CHAR_ENCODING_NONE;
          }
      default:
          return ZCHARSET_ERROR;
      }
  }// getZCharFromXMLCode


const char*
decode_ZCharset(const ZCharset_type pCharset)
{

    for (size_t wi=0;GCharsetTable[wi].Charset;wi++)
      if (pCharset==GCharsetTable[wi].Charset)
                    return GCharsetTable[wi].Name;
    return nullptr;

}//decode_ZCharset


ZCharset_type
encode_ZCharset(const char* pCharset)
{
    for (size_t wi=0;GCharsetTable[wi].Charset;wi++)
        if (strcasecmp(pCharset,GCharsetTable[wi].Name)==0)
                            return GCharsetTable[wi].Charset;
    return ZCHARSET_ERROR;
}//encode_ZCharset

ZCharset_type guessZCharset (uint8_t* pString, size_t pLen)
        { return  getZCharsetFromXMLEncoding((uint8_t)xmlDetectCharEncoding(pString,pLen)); }

const char*
decode_XMLCharEncoding(int8_t pEncoding)
{
  switch(pEncoding)
      {
  case XML_CHAR_ENCODING_ERROR:
      return "XML_CHAR_ENCODING_ERROR";
  case XML_CHAR_ENCODING_NONE:
      return "XML_CHAR_ENCODING_NONE";
  case XML_CHAR_ENCODING_UTF8:
      return "XML_CHAR_ENCODING_UTF8";
  case XML_CHAR_ENCODING_UTF16LE:
      return "XML_CHAR_ENCODING_UTF16LE";
  case XML_CHAR_ENCODING_UTF16BE:
      return "XML_CHAR_ENCODING_UTF16BE";
  case XML_CHAR_ENCODING_UCS4LE:
      return "XML_CHAR_ENCODING_UCS4LE";
  case XML_CHAR_ENCODING_UCS4BE:
      return "XML_CHAR_ENCODING_UCS4BE";
  case XML_CHAR_ENCODING_EBCDIC:
      return "XML_CHAR_ENCODING_EBCDIC";
  case XML_CHAR_ENCODING_UCS4_2143:
      return "XML_CHAR_ENCODING_UCS4_2143";
  case XML_CHAR_ENCODING_UCS4_3412:
      return "XML_CHAR_ENCODING_UCS4_3412";
  case XML_CHAR_ENCODING_UCS2:
      return "XML_CHAR_ENCODING_UCS2";
  case XML_CHAR_ENCODING_8859_1:
      return "XML_CHAR_ENCODING_8859_1";
  case XML_CHAR_ENCODING_8859_2:
      return "XML_CHAR_ENCODING_8859_2";
  case XML_CHAR_ENCODING_8859_3:
      return "XML_CHAR_ENCODING_8859_3";
  case XML_CHAR_ENCODING_8859_4:
      return "XML_CHAR_ENCODING_8859_4";
  case XML_CHAR_ENCODING_8859_5:
      return "XML_CHAR_ENCODING_8859_5";
  case XML_CHAR_ENCODING_8859_6:
      return "XML_CHAR_ENCODING_8859_6";
  case XML_CHAR_ENCODING_8859_7:
      return "XML_CHAR_ENCODING_8859_7";
  case XML_CHAR_ENCODING_8859_8:
      return "XML_CHAR_ENCODING_8859_8";
  case XML_CHAR_ENCODING_8859_9:
      return "XML_CHAR_ENCODING_8859_9";
  case XML_CHAR_ENCODING_2022_JP:
      return "XML_CHAR_ENCODING_2022_JP";
  case XML_CHAR_ENCODING_SHIFT_JIS:
      return "XML_CHAR_ENCODING_SHIFT_JIS";
  case XML_CHAR_ENCODING_EUC_JP:
      return "XML_CHAR_ENCODING_EUC_JP";
  case XML_CHAR_ENCODING_ASCII:
      return "XML_CHAR_ENCODING_ASCII";
  default:
      return "Unknown char encoding";
      }
}// decode_CharEncoding

utfdescString GUST_Lib;
const char* decode_UST(const UST_Status_type pStatus)
{

   GUST_Lib.clear();
   if (pStatus==UST_Nothing)
                 return "UST_Nothing";
  if (pStatus < UST_SEVERE)
                GUST_Lib = (const utf8_t*)"UST_SEVERE";

  if ((pStatus&UST_ILLEGAL)==UST_ILLEGAL)
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_ILLEGAL"); }
  if ((pStatus&UST_UNASSIGNED)==UST_UNASSIGNED)
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_UNASSIGNED"); }
  if ((pStatus&UST_IRREGULAR)==UST_IRREGULAR)
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_IRREGULAR"); }

  switch (pStatus)
  {
      case UST_SUCCESS:
        {GUST_Lib.addConditionalOR((utf8_t*) "UST_SUCCESS"); break;}
      case UST_ENDOFSTRING:
        {GUST_Lib.addConditionalOR((utf8_t*) "UST_ENDOFSTRING"); break;}
      case UST_TRUNCATED:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_TRUNCATED"); break;}
      case UST_NULLPTR:
       {GUST_Lib.addConditionalOR((utf8_t*) "UST_NULLPTR"); break;}
      case UST_IVBOM:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_IVBOM"); break;}
      case UST_IVBOMUTF8:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_IVBOMUTF8"); break;}
      case UST_IVBOMUTF16:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_IVBOMUTF16"); break;}
      case UST_IVBOMUTF32:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_IVBOMUTF32"); break;}
      case UST_ENCODING:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_ENCODING"); break;}
      case UST_IVUTF16SUR_1:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_IVUTF16SUR_1"); break;}
      case UST_IVUTF16SUR_2:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_IVUTF16SUR_2"); break;}
      case UST_UTF8CAPAOVFLW:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_UTF8CAPAOVFLW"); break;}
      case UST_MISSCONTBYTE:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_MISSCONTBYTE"); break;}
      case UST_UTF8IVCONTBYTES:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_UTF8IVCONTBYTES"); break;}
      case UST_IVBOMUTF16BE:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_IVBOMUTF16BE"); break;}
      case UST_IVBOMUTF16LE:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_IVBOMUTF16LE"); break;}

      case UST_MEMERROR:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_MEMERROR"); break;}




      case UST_BOMUTF8:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_BOMUTF8"); break;}
      case UST_BOMUTF16:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_BOMUTF16"); break;}
      case UST_BOMUTF16BE:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_BOMUTF16BE"); break;}
      case UST_BOMUTF16LE:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_BOMUTF16LE"); break;}

      case UST_BOMUTF32:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_BOMUTF32"); break;}

      case UST_BOMUTF32BE:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_BOMUTF32BE"); break;}
      case UST_BOMUTF32LE:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_BOMUTF32LE"); break;}

      case UST_IVCODEPOINT:
      {GUST_Lib.addConditionalOR((utf8_t*) "UST_IVCODEPOINT"); break;}

//  default: {GUST_Lib.addConditionalOR((utf8_t*) "Unknown UST Status"); break;}
  }// switch

  if (GUST_Lib.isEmpty())
            return "Unknown UST Status";
  return (const char*)GUST_Lib.toCString_Strait();
} // decode_UST


#include <ztoolset/utfutils.h>




#endif // ZCHARSET_CPP
