/** @file zcharset.h Charset base level routines
    Holds the logic of utf multi-character units.
    Holds the conversions from and to various character encodings.

    Uses heavily icu facilities.

    Dedicated to charset operations, counting, transformation and translation :

  - counting number of characters
  - parsing strings and returning character per character using a context
  - analyzing or skipping BOMs
  - trying to convert utf strings to C strings and use substitution char if not successfull

  - analyzing and converting character endodings

  Nota Bene :
        - convertions to and from utf formats are done at the string level (fixed or varying) :
           + utffixedstring.h
           + utfvaryingstring.h

        - base utf utility routines such as utfStrcpy, utfStrlen are contained in utfutils.h

  */
#ifndef ZCHARSET_H
#define ZCHARSET_H
#include <config/zconfig.h>
#include <ztoolset/zerror.h>
#include <ztoolset/zerror_min.h>
#include <ztoolset/utfstrctx.h>

#include <stdint.h>

#include <ztoolset/zmodulestack.h>

typedef uint8_t         utf8_t ;
typedef uint16_t        utf16_t ;
typedef uint32_t        utf32_t ;

/** hereafter the substition character used while converting utf32 to char and substitution failed */

#define __UTF_SUBSTITUTION_CHAR__  '@'



namespace zbs {

/**
 * @defgroup CHARACTER_ENCODING Character encoding and character storage format
 *@{
 *
 * @par charset and strings
 *
 *  Using strings means nothing without specifying how characters are coded inside the string.<br>
 *  The same code might mean different character representations according the encoding.<br>
 *  Character encodings are furthermore called charsets.<br>
 *
 *  This is the reason why each ZString is specified to have an encoding - charset, right at the instantiation of the string object.<br>
 *
 * So,when it is required to create a char string, the created string has an associated charset,
 * corresponding to how the characters are internally encoded ;<br>
 * This charset could be Latin-1, US-ASCII, etc.<br>
 *
 *
 * If no charset is specified, then the system default (defined by locale) charset encoding is taken, which is a charset.<br>
 * The function getDefaultCharset() gets this charset value.<br>
 *
 * Indeed, with string objects like utf8FixedString or utf16VaryingString, for example, charset is implicitly taken to be respectively in those cases utf8 and utf16.<br>
 *
 * @par Character encoding and character storage format
 *
 *  Character code is distinct from character storage format :<br>
 *
 * - character code : the way a single character is expressed according to a character table giving a character code.<br>
 * Examples: US-ASCII, UNICODE, ISO LATIN 1<br>
 *  This character mapping mostly refers to locale.<br>
 *
 * - character storage format : how the character code is stored in memory.
 *  + how much bytes are basically reserved as a character unit (one character may use multiple character units to be stored)
 *  + endianness of the storage : big endian or little endian.
 *  + fixed length storage vs varying length
 *
 * By instance, the same UNICODE codepoint (character code) could be stored onto multiple character storage formats.<br>
 *  In practise, a character encoding scheme couples both character coding and character storage format.<br>
 *
 * Examples: UTF8  (character unit is one unsigned byte) , UTF32 (character unit is 4 unsigned bytes)<br>
 *   UTF formats store all UNICODE codepoints.
 *
 * Warning: UCS2 and UCS4 are legacy names<br>
 *  UCS2 : UTF16 storage format - UNICODE encoding with NO surrogate characters possibility (no multiple character units per character).<br>
 *  UCS4 : UTF32 storage format - UNICODE encoding.<br>
 *
 *  Both UCS2 and UCS4 may be big endian or little endian.<br>

 *  see @ref CHARUNIT <br>
 *  see @ref CHARACTER_CONVERSION <br>
 *  see [Unicode character sets] (http://www.unicode.org/charts/) <br>
 *
 *
 *  @weakgroup CHARACTER_CONVERSION  Character encoding conversion

  A string of characters is mandatorily associated with a character encoding type.<br>
  A string without mentionning encoding, is best case an unknown format, wrong case an errored string containing wrong information.<br>
  If no special encoding is mentionned, then system default encoding, coming from system locale, is retained.<br>

  zbasesystem does not recommend to use a special character encoding. Any encoding could be used, as there are embedded conversion from one format to another.<br>

  Character encoding depends on the desired purpose :

  - UTF-8 is somehow much more compact, and used by most of internet browsers. However it consumes much more processing time to decode/encode and use.
  - char* string means nothing without mentionning LATIN-1 or ISO-8900-5 for instance.
  - UTF32 is faster in processing time, but it consumes storage space, and is subject to little/big endian conversions
  - UTF16 is used by most of utilities :
    . QT QSTRING uses UTF16 as internal representation (internally based on ICU routines)
    . ICU uses UTF16 as internal pivot representation as well as output sortkeys.


 @par UNICODE conversion

 zbasesystem has its own built-in conversions to and from the three UNICODE formats, UTF-8 UTF16 and UTF32.<br>
So that, it is an embedded, faster conversion (this is also true for character counting).

 utf8 objects (fixed or varying strings) converts to and from any other UNICODE format, as well as LATIN-1 (ISO 8859-1)c haracter strings.<br>

 This is as well the case for utf16 and utf32.

 @par Case of BOMs

 zbasesystem does not maintain nor use internally BOMs.<br>
 However, BOMs could be encountered while parsing external strings.<br>

 BOMs are automatically <b>verified and skipped</b> at the only following conditions :
 - it is strictly located as the first element within the string to parse,
 - it corresponds to the expected character format :<br>
 If a conversion is asked from utf16 little endian, and a corresponding BOM (utf16 little endian) is detected, then,
the BOM is skipped.<br>

Strings with leading BOMs are therefore considered to start right after the BOM, BOM itself excluded.<br>
Counting the number of characters of these strings will count only the valid number of characters starting after the leading BOM.

 @par Wrong or invalid BOMs

 In case an unexpected BOM is encountered, an error is emitted (mostly UST_Status), and corresponding characters are considered to be invalid characters.<br>
 BOMs found in the middle of a string flow will be considered as invalid characters and will be subject to be replaced with replacement character.<br>

 @par external conversions

 Any conversion from and to format mentionned in table in section @ref CHARACTER_ENCODING, may be converted using zbasesystem string objects.


  @weakgroup ICU_ENCODING Supported character encodings

 Herebelow are respectively the codes and the meaning of supported character encodings.

@verbatim

Code        Description                             direct conversion function
----------+----------------------------------------+---------------------------+
ASCII 	    7-bit ASCII                             Yes     (use ICU algorythmic conversion)
LATIN1 	    ISO 8859-1 Western European             Yes     (use ICU algorythmic conversion)
ISO8859_2 	ISO 8859-2 Eastern European
ISO8859_3 	ISO 8859-3 Southeast European
ISO8859_4 	ISO 8859-4 Baltic
ISO8859_5 	ISO 8859-5 Cyrillic
ISO8859_6 	ISO 8859-6 Arabic
ISO8859_7 	ISO 8859-7 Greek
ISO8859_8 	ISO 8859-8 Hebrew
ISO8859_9 	ISO 8859-9 Latin 5 (Turkish)
ISO8859_10 	ISO 8859-10 Latin 6 (Nordic)
ISO8859_11 	ISO 8859-11 Thai
ISO8859_13 	ISO 8859-13 Latin 7 (Baltic Rim)
ISO8859_14 	ISO 8859-14 Latin 8 (Celtic)
ISO8859_15 	ISO 8859-15 Latin 9 (Western Europe)
UTF_8 	UTF-8 encoding of Unicode                   Yes (some implemented internally)
EUC_CN 	Simplified Chinese Combined (367 + 1382)
EUC_KR 	Korean EUC Combined (367 + 971)
EUC_JP 	Japanese Combined (895 + 952 + 896 + 953)
EUC_TW 	Taiwan Extended UNIX Code (CNS 11643-1986), Combined (367 + 960 + 961)
UCS2 	UCS-2 (Really UTF-16 BE)
CP037 	IBM® EBCDIC US English
CP037_S390 	IBM EBCDIC US English LF & NL reversed
CP256 	IBM EBCDIC Netherlands
CP259 	IBM EBCDIC Symbols Set 7
CP273 	IBM EBCDIC German
CP274 	IBM EBCDIC Belgium
CP275 	IBM EBCDIC Brazil
CP276 	IBM EBCDIC French-Canada
CP277 	IBM EBCDIC Danish
CP278 	IBM EBCDIC Swedish
CP280 	IBM EBCDIC Italian
CP282 	IBM EBCDIC Portugal
CP284 	IBM EBCDIC Latin American Spanish
CP285 	IBM EBCDIC UK English
CP290 	IBM EBCDIC Japanese Katakana
CP297 	IBM EBCDIC French
CP420 	IBM EBCDIC Arabic
CP421 	IBM EBCDIC Maghreb/French
CP423 	IBM EBCDIC Greek
CP424 	IBM EBCDIC Latin/Hebrew
CP437 	MS-DOS US English
CP500 	IBM EBCDIC 500V1
CP708 	Arabic (ASMO 708)
CP709 	Arabic (ASMO 449+, BCON V4)
CP710 	Arabic (Transparent Arabic)
CP720 	Arabic (Transparent ASMO)
CP737 	reek (formerly 437G)
CP770 	Lithuanian Standard RST 1095-89
CP771 	KBL (Lithuanian and Russian characters)
CP772 	Lithuanian Standard LST 1284:1993
CP773 	Lithuanian (Mix of 771 and 775)
CP774 	Lithuanian Standard 1283:1993
CP775 	Baltic
CP776 	Lithuanian 770 extended
CP777 	Lithuanian 771 extended
CP778 	Lithuanian 775 extended
CP790 	Mazovia (Polish + codepage 437 extended characters
CP803 	IBM EBCDIC Hebrew (old)
CP813 	ISO 8859-7 Greek/Latin
CP819 	ISO 8859-1 Latin Alphabet No. 1
CP833 	IBM EBCDIC Korean SBCS
CP834 	IBM EBCDIC Korean DBCS
CP835 	IBM EBCDIC Traditional Chinese DBCS
CP837 	IBM EBCDIC Simplified Chinese DBCS
CP838 	IBM EBCDIC Thai
CP850 	MS-DOS Latin 1
CP851 	MS-DOS Greek
CP852 	MS-DOS Slavic (Latin 1)
CP853 	MS-DOS Turkey Latin 3 (replaced by Latin 5)
CP855 	IBM Cyrillic (primarily Russian)
CP856 	PC Hebrew
CP857 	IBM Turkish (Latin 5)
CP860 	MS-DOS Portuguese
CP861 	MS-DOS Icelandic
CP862 	Hebrew (Migration)
CP863 	MS-DOS Canadian-French
CP864 	PC Arabic
CP865 	MS-DOS Nordic
CP866 	MS-DOS Russian
CP868 	MS-DOS Urdu
CP869 	IBM Modern Greek
CP870 	IBM EBCDIC Multilingual Latin 2
CP871 	IBM EBCDIC Icelandic
CP872 	PC Cyrillic with Euro update
CP874 	MS-DOS Thai, superset of TIS 620
CP875 	IBM EBCDIC Greek
CP878 	KOI-R (Cyrillic)
CP880 	Cyrillic Multilingual
CP899 	PC Symbols
CP905 	IBM EBCDIC Turkey Latin 3 (replaced by Latin 5)
CP912 	ISO 8859-2; ROECE Latin-2 Mulitlingual
CP913 	ISO 8859-3 Southeast European
CP914 	ISO 8859-4 Baltic
CP915 	ISO 8859-5; Cyrillic; 8-bit ISO
CP916 	ISO 8859-8; Hebrew
CP918 	IBM EBCDIC Urdu
CP920 	ISO 8859-9; Latin 5
CP921 	ISO Baltic (8-bit)
CP922 	ISO Estonia (8-bit)
CP929 	Thai PC double byte
CP930 	IBM EBCDIC Japanese Katakana Extended, Combined (290 + 300)
CP931 	IBM EBCDIC Japanese Latin-Kanji, Combined (037 + 300)
CP932 	MS Windows Japanese, superset of Shift-JIS, Combined (897 + 301)
CP933 	IBM EBCDIC Korean Combined (833 + 834)
CP934 	Korean PC Combined (891 + 926)
CP935 	IBM EBCDIC Simplified Chinese, Combined (836 + 837)
CP936 	MS Windows Simplified Chinese, Combined (903 + 928)
CP937 	IBM EBCDIC Traditional Chinese, Combined (037 + 835)
CP938 	Traditional Chinese Combined (904 + 927)
CP939 	IBM EBCDIC Japanese Latin Extended, Combined (1027 + 300)
CP942 	MS-DOS Japanese Kana Combined (1041 + 301)
CP943 	MS-DOS Japanese Combined (1041 + 941)
CP944 	Korean PC Combined (1040 + 926)
CP946 	Simplified Chinese PC Combined (1042 + 928)
CP948 	MS-DOS Traditional Chinese, Combined (1043 + 927)
CP949 	MS Windows Korean, superset of KS C 5601-1992, Combined (1088 + 951)
CP950 	MS Windows Traditional Chinese, superset of Big 5, Combined (1114 + 947)
CP1004 	PC-data Latin-1 extended desktop publishing
CP1006 	Urdu, 8-bit
CP1008 	Arabic, 8-bit ISO/ASCII
CP1025 	IBM EBCDIC Cyrillic
CP1026 	IBM EBCDIC Turkish
CP1027 	IBM EBCDIC Japanese Extended Single Byte
CP1040 	Korean PC extended Single Byte
CP1041 	Japanese PC extended Single Byte
CP1043 	Traditional Chinese extended Single Byte
CP1046 	Arabic
CP1047 	Latin 1 / Open Systems (US 3270)
CP1047_S390 	Latin 1 / Open Systems (US 3270) LF & NL reversed
CP1051 	HP-UX Latin1
CP1097 	IBM EBCDIC Farsi
CP1098 	MS-DOS Farsi
CP1112 	IBM EBCDIC Baltic Multilingual
CP1114 	Traditional Chinese Single Byte (IBM Big 5)
CP1115 	Simplified Chinese Single Byte (IBM GB)
CP1122 	IBM EBCDIC Estonia
CP1123 	IBM EBCDIC Cyrillic Ukraine
CP1124 	Cyrillic Ukraine 8-bit
CP1130 	IBM EBCDIC Vietnamese
CP1137 	IBM EBCDIC India
CP1140 	IBM EBCDIC US (with Euro)
CP1141 	IBM EBCDIC Germany, Austria (with Euro)
CP1142 	IBM EBCDIC Denmark (with Euro)
CP1143 	IBM EBCDIC Sweden (with Euro)
CP1144 	IBM EBCDIC Italy (with Euro)
CP1145 	IBM EBCDIC Spain (with Euro)
CP1146 	IBM EBCDIC UK Ireland (with Euro)
CP1147 	IBM EBCDIC France (with Euro)
CP1148 	IBM EBCDIC International Latin1 (with Euro)
CP1149 	IBM EBCDIC Iceland (with Euro)
CP1153 	IBM EBCDIC Latin2 (with Euro)
CP1154 	IBM EBCDIC Cyrillic (with Euro)
CP1155 	IBM EBCDIC Turkish (with Euro)
CP1156 	IBM EBCDIC Baltic Multilingual (with Euro)
CP1157 	IBM EBCDIC Estonia (with Euro)
CP1158 	IBM EBCDIC Cyrillic Ukraine (with Euro)
CP1159 	SBCS Traditional Chinese Host (with Euro)
CP1160 	IBM EBCDIC Thailand (with Euro)
CP1164 	IBM EBCDIC Vietnamese (with Euro)
CP1250 	MS Windows Latin 2 (Central Europe)
CP1251 	MS Windows Cyrillic (Slavic)
CP1252 	MS Windows Latin 1 (ANSI), superset of Latin1
CP1253 	MS Windows Greek
CP1254 	MS Windows Latin 5 (Turkish), superset of ISO 8859-9
CP1255 	MS Windows Hebrew
CP1256 	MS Windows Arabic
CP1257 	MS Windows Baltic Rim
CP1258 	MS Windows Vietnamese
CP1279 	Hitachi Japanese Katakana Host
CP1361 	MS Windows Korean (Johab)
CP1381 	MS-DOS Simplified Chinese Combined (1115 + 1380)
CP1383 	China EUC
CP1386 	GBK Chinese
CP1392 	Simplified Chinese GB18030
CP5026 	IBM EBCDIC Japan Katakana-Kanji Combined (290 + 300)
CP5028 	Japan Mixed Combined (897 + 301)
CP5031 	IBM EBCDIC Simplified Chinese Combined (836 + 837)
CP5033 	IBM EBCDIC Traditional Chinese Combined (037 + 835)
CP5035 	IBM EBCDIC Japan Latin Combined (1027 + 300)
CP5038 	Japan Mixed Combined (1041 + 301)
CP5045 	Korean PC Combined (1088 + 951)
CP5050 	Japanese EUC Combined (895 + 952 + 896 + 953)
CP5488 	Simplified Chinese GB18030
CP9125 	IBM EBCDIC Korean Combined (833 + 834)
EuroShift_JIS 	Test code page, Shift-JIS with European characters
SBCS 	Single Byte Code Set
DBCS 	Double Byte Code Set
MBCS 	MultiByte Code Set
UTF16BE 	utf-16 big endian                       Yes (some implemented internally)
UTF16LE 	utf-16 little endian                    Yes (some implemented internally)
UTF32BE 	utf-32 big endian                       Yes (some implemented internally)
UTF32LE 	utf-32 little endian                    Yes (some implemented internally)
HZ 	HZ code set
SCSU 	SCSU code set    (utf compression)
ISCII 	iscii code set   (indian ascii)
UTF7 	utf-7
BOCU1 	bocu1   : utf with scsu (utf compression)
UTF16 	utf16 code set                              Remark: is UTF16_BE
UTF32 	utf32 code set                              Remark: is UTF32_BE
CESU8 	cesu8 code set
GB18030 	gb18030 code set
@endverbatim

  @weakgroup SORT_INDEX_KEY  Sort key - index key

  @par Sort key - necessity and usage

  In order to compare efficiently strings, it is necessary to create a key, hashcoded from string content that orders contained characters.<br>
  It takes into account, in particular, special characters (accented characters, etc...) that are local dependent.<br>
  This key is based on 16 bit units (uint16_t) and is called Sort key.

  @par Index key

  For strings content, an index key is a sort key -local dependent- that is converted to big endian whenever necessary.<br>

  see @utf8FixedString::getSortkey
  see @utf8FixedString::getIndexkey

 *
 *
 *
 * @weakgroup CHARUNIT Number of characters in an utf string
 *
 *  Character unit is the elementary base of storage format :
 *  - utf8 : one byte (utf8_t is uint8_t)
 *  - utf16 : two bytes (utf16_t is uint16_t)
 *  - utf32 : four bytes (utf32_t is uint32_t )
 *
 *  utf8 character may have more than one character unit as its storage representation.
 *
 *  @par Endianness of character storage format
 *
 *  As soon as storage format character unit is greater than a byte, we must have two options : big endian or little endian.
 *
 *  For instance, there is no "pure" utf16. Utf16 must be big endian or little endian.
 *
 *  @par multiple units vs single unit format
 *
 *  As utf8 and utf16 may have more than one utf8 or utf16 unit for one single character (utf8 up to three),
 *  counting the number of characters using a kind of <strlen> means nothing to get the correct number of characters.<br>
 *  In addition, utf string may be prefixed with a BOM that is not a character (this is the case including for utf32).
 *
 *  @par counting character or counting character units
 *
 *  We need to differentiate :
 *
 *  <b>Character units count</b> : number of character units (utf8 utf16 utf32 according case),
 *  This is given with the equivalent <strlen> routine : utfStrlen<_Utf> (_Utf parameter being one of {utf8_t,utf16_t,utf32_t}.<br>
 *  Units count is also, by extension, the capacity of an utf fixed string expressed in character units.
 * Units counts however does not represent the size in bytes of the string.<br>
 * To get the size in bytes, the following formula applies : Units count * size of unit.<br>
 * Where size of unit is :
 * - utf8_t : 1
 * - utf16_t: 2
 * - utf32_t: 4
 *
 *  Canonical character count : number of effective (displayable) characters in the string
 *  - skipping BOM if any
 *  - counting multi-units character as one single character
 *
 *  So that, character count != character units != number of bytes.<br>
 * But each of these have their own usage.
 *
 *  @see is_multiple
 *
 *
 *@}
 *
 */
}


template <class _Utf>
/**
 * @brief utfStrlen computes current length in _Utf characters of _Utf string pString
 * @param pString
 * @return
 */
 size_t utfStrlen (const _Utf* pString)
{

    if (pString==nullptr)
                    {return 0;}
    size_t wLen=0;
//    _Utf wC=*pString;
    while ((pString[wLen])&&(wLen < __STRING_MAX_LENGTH__))
        {
        wLen++;
        }
    _ASSERT_(wLen >= __STRING_MAX_LENGTH__,"Fatal error: endofmark has not been found in string\n",1)
    return wLen;
}


/** brief cst_default_delimiter default delimiter set of characters per character storage format
 */
static const char cst_default_delimiter_Char[] = {
    (char)' ',
    (char)'\t',
    (char)'\n',
    (char)'\r',
    (char)'\0'  // must end with '\0'
};
static const utf8_t cst_default_delimiter_U8[] = {
    (utf8_t)' ',
    (utf8_t)'\t',
    (utf8_t)'\n',
    (utf8_t)'\r',
    (utf8_t)'\0'  // must end with '\0'
};
static const utf16_t cst_default_delimiter_U16[] = {
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
static const size_t cst_UCSSpaces16NB=17;
static const utf16_t cst_UCSSpaces16 [] ={
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


static const utf8_t CharAndPunctuation[256] = {
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

#define __UNICODE_MAX_CODEPOINT__   0x10FFFF

extern utf16_t cst_UnicodeSpaces [];

extern utf8_t  cst_Latin1_Upcase [];

extern utf8_t cst_Latin1_Lowcase [];

extern utf8_t cst_Latin1_Only_Char[];


#ifndef ZCHARSET_CPP
extern const char      cst_default_delimiter_Char[] ;
extern const utf8_t    cst_default_delimiter_U8[] ;
extern const utf16_t   cst_default_delimiter_U16[] ;
extern const utf32_t   cst_default_delimiter_U32[] ;
#endif // ZCHARSET_CPP
#ifdef __COMMENT__
#ifndef __UTF_REPLACEMENT__
#define __UTF_REPLACEMENT__
extern utf32_t  utf32Replacement ;
extern utf16_t  utf16Replacement[3] ;
extern utf8_t   utf8Replacement[5];
#endif // __UTF_REPLACEMENT__
#endif
static utf32_t  utf32Replacement = 0xFFFD ;
static utf16_t  utf16Replacement[2] = {0xFFFD,0} ;
static utf8_t   utf8Replacement[5] = {0xEF, 0xBF, 0xBD,0,0} ;


/**
 * @brief changeReplacementCodepoint Replaces global replacement character.<br>
 * A global replacement character is defined for the application.<br>
 * By default, this character is Unicode replacement character, i.e. 0xFFFD (big endian).<br>
 * This replacement character is available under utf8, utf16 and utf32 format.<br>
 * However, it may be changed by calling this routine.<br>
 * An utf32 codepoint is given in input, and this codepoint is checked and converted to utf8, utf16 and utf32 then stored as global values.<br>
 * Given codepoint must have current system endianness (big or little endian according system endianness).<br>
 * if any kind of error occurs, replacement codepoint is not changed.
 * @param pReplacement a valid utf32 unicode codepoint
 * @return a ZStatus with ZS_SUCCESS if operation is successfull.<br>
 * in case of error, see returned statuses for routines <br>
 * utf32Encode() <br>
 * utf16Encode() <br>
 * utf8Encode() <br>
 */
ZStatus changeReplacementCodepoint(utf32_t pReplacement);


const utf32_t cst_Unicode_Replct_utf32  =0xFFFD;
const utf16_t cst_Unicode_Replct_utf16  =0xFFFD;
const utf8_t  cst_Unicode_Replct_utf8[5] ={0xEF, 0xBF, 0xBD,0,0};
const char cst_Unicode_Replct_char[] ={'@',0};

const utf8_t cst_utf8BOM[3] =    { 0xEF,0xBB,0xBF };        // Size 3
const utf16_t cst_utf16BOMBE  = { 0xFEFF };// Size 2
const utf16_t cst_utf16BOMLE  = { 0xFFFE }; // Warning : may be confused with cst_utf32BOMLE (Size 4)
const utf32_t cst_utf32BOMBE  = 0x0000FEFF ;    // Size 4
const utf32_t cst_utf32BOMLE =  0xFFFE0000 ;

const utf8_t cst_utf16BOMBE_Byte[]  = { 0xFE,0xFF };
const utf8_t cst_utf16BOMLE_Byte[]  = { 0xFF,0xFE };

// utf32 BOM  00 00 FE FF (big endian) - FF FE 00 00 (little endian)

const utf8_t cst_utf32BOMBE_Byte [4] = { 0,0,0xFE,0xFF };    // Size 4
const utf8_t cst_utf32BOMLE_Byte [4] = { 0xFF,0xFE,0,0 };    // Warning : may be confused with cst_utf16BOMLE (Size 2)


/** utf string parsing for characters */

#ifndef __ZBOM__
#define __ZBOM__
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

/*
template <class _Utf>
_Utf* getDefaultDelimiter (void);
*/

template <class _Utf>
const _Utf cst_default_delimiter[] = {
        (_Utf)' ',
        (_Utf)'\t',
        (_Utf)'\n',
        (_Utf)'\r',
        (_Utf)'\0'  // must end with '\0'
    };

template <class _Utf>
const _Utf* getDefaultDelimiter (void)
{
    return cst_default_delimiter<_Utf>;
}

/**
 * @brief ZCharset_type : gives the character encoding and storage format
 *
 *
 */

typedef uint16_t ZCharsetBase;

enum ZCharset_type : ZCharsetBase
{
    ZCHARSET_ERROR              = 0xFFFF,   /* No char encoding detected or invalid char encoding*/
    ZCHARSET_NOTSUPPORTED       = 0x1FFF,    /* char encoding detected but not supported */

    ZCHARSET_SYSTEMDEFAULT      = 0,        /* Current system default charset encoding*/

    ZCHARSET_LITTLEENDIAN       = 0x0010,
    ZCHARSET_ENCODING_MASK      = 0x000F,

//  =========character content encoding====================

    ZCHARSET_ASCII              = 0x0001, /* pure ASCII  US-ASCII*/


    ZCHARSET_8859_1             = 0x0002,/* ISO-8859-1 ISO Latin 1 */
    ZCHARSET_LATIN_1            = 0x0002,/* ISO-8859-1 ISO Latin 1 (alias)*/

    ZCHARSET_8859_2             = 0x0003,/* ISO-8859-2 ISO Latin 2   Central European */
    ZCHARSET_LATIN_2            = 0x0003,/* ISO-8859-1 ISO Latin 2 (alias)*/

    ZCHARSET_8859_3             = 0x0004,/* ISO-8859-3          South European  */
    ZCHARSET_8859_4             = 0x0005,/* ISO-8859-4          Northern European */
    ZCHARSET_8859_5             = 0x0006,/* ISO-8859-5          Cyrillic */
    ZCHARSET_8859_6             = 0x0007,/* ISO-8859-6          Arab*/
    ZCHARSET_8859_7             = 0x0008,/* ISO-8859-7          Greek*/
    ZCHARSET_8859_8             = 0x0009,/* ISO-8859-8          Hebrew*/
    ZCHARSET_8859_9             = 0x000A,/* ISO-8859-9          Turkish*/


    ZCHARSET_EBCDIC             = 0x000B, /* EBCDIC-US - no more seriously used */

    ZCHARSET_2022               = 0x000C,/* ISO-2022-JP  : not really managed. Treated as single by character unit storage */
    ZCHARSET_SHIFT_JIS          = 0x000D,/* Double byte Japanese Industrial Standard- Legacy */
    ZCHARSET_EUC_JP             = 0x000E, /* EUC-JP : not really managed. Treated as single by character unit storage*/

    ZCHARSET_UNICODE            = 0x000F, /* Unicode encoding whatever UTF storage format is : This is the default */

//=============== Character storage format ==========================

    ZCHARSET_UTF8               = 0x010F, /* Storage format : UTF-8 : single byte character unit storage*/

    ZCHARSET_UTF16              = 0x020F, /* Storage format : UTF-16 (default : big endian): two bytes character unit storage */
    ZCHARSET_UTF16BE            = 0x020F, /* Storage format : UTF-16 big endian   UTF16-BE*/
    ZCHARSET_UTF16LE            = 0x021F, /* Storage format : UTF-16 little endian UTF16-LE*/

    ZCHARSET_UCS2               = 0x020F, /* UCS-2 (default:big endian) : UCS2 is UNICODE encoded character */
    ZCHARSET_UCS2BE             = 0x020F, /* UCS-2 big endian      UCS-2BE*/
    ZCHARSET_UCS2LE             = 0x021F, /* UCS-2 little endian   UCS-2LE*/

    ZCHARSET_UTF32              = 0x040F, /* Storage format : UTF32 (default: big endian)-: four bytes character unit storage */
    ZCHARSET_UTF32BE            = 0x040F, /* Storage format : UTF32 big endian */
    ZCHARSET_UTF32LE            = 0x041F, /* Storage format : UTF32 little endian  */

    ZCHARSET_UCS4               = 0x040F, /* UCS-4 (by default: big endian): UCS4 is UNICODE encoded character */
    ZCHARSET_UCS4BE             = 0x040F, /* UCS-4 big endian */
    ZCHARSET_UCS4LE             = 0x041F, /* UCS-4 little endian UCS4-LE*/

    ZCHARSET_UCS4_2143          = 0x042F, /* UCS-4 unusual ordering */
    ZCHARSET_UCS4_3412          = 0x044F, /* UCS-4 unusual ordering */

    ZCHARSET_UTF7               = 0x080F, /* UTF-7 : single byte character unit storage for email : Unicode encoding*/

    ZCHARSET_IMAP_MAILBOX       = 0x1000, /* based on utf7 ? */
    ZCHARSET_COMPOUND_TEXT      = 0x1001, /* https://www.x.org/releases/X11R7.6/doc/xorg-docs/specs/CTEXT/ctext.html */
    ZCHARSET_BOCU1              = 0x1002, /* Binary Ordered Compression for Unicode   https://www.unicode.org/notes/tn6/ */
    ZCHARSET_CESU8              = 0x1003, /*Compatibility Encoding Scheme for UTF-16 : 8bits encoding http://www.unicode.org/reports/tr26/ */
    ZCHARSET_HZ                 = 0x1004, /* HanZi : chineese characters : simplified and traditional chineese https://en.wikipedia.org/wiki/HZ_(character_encoding) */
    ZCHARSET_SBCS               = 0x1005, /* Single byte coding */
    ZCHARSET_DBCS               = 0x1006, /* Double byte coding : example CJK (Chineese Japanesse Korean) */
    ZCHARSET_LMBCS_1            = 0x1007, /* Lotus multi-byte character set variants 1-8, 11, 16-19 */
    ZCHARSET_LMBCS_2            = 0x1008,
    ZCHARSET_LMBCS_3            = 0x1009,
    ZCHARSET_LMBCS_4            = 0x100A,
    ZCHARSET_LMBCS_5            = 0x100B,
    ZCHARSET_LMBCS_6            = 0x100C,
    ZCHARSET_LMBCS_8            = 0x100D,
    ZCHARSET_LMBCS_11           = 0x100E,
    ZCHARSET_LMBCS_16           = 0x100F,
    ZCHARSET_LMBCS_17           = 0x1010,
    ZCHARSET_LMBCS_18           = 0x1011,
    ZCHARSET_LMBCS_19           = 0x1012,
    ZCHARSET_SCSU               = 0x1013, /* Standard Compression Scheme for Unicode (cf BOCU) https://www.unicode.org/notes/tn6/#SCSU */
    ZCHARSET_MBCS               = 0x1014, /* Multi-byte Coding Scheme  */

    ZCHARSET_ISCII              = 0x101F  /* Indian charset vs ASCII */


} ;

#include <unicode/ucnv.h>

struct GCharset_struct{
        const char* Name;
        ZCharset_type  Charset;
        UConverterType ICUCnvType;
        int8_t UnitSize;
};
const GCharset_struct GCharsetTable []={
    {"US_ASCII",ZCHARSET_ASCII,     UCNV_US_ASCII,  1},
    {"LATIN_1", ZCHARSET_LATIN_1,   UCNV_LATIN_1,   1},
    {"UTF8", ZCHARSET_UTF8,         UCNV_UTF8,      1},
    {"UTF16BE", ZCHARSET_UTF16BE,   UCNV_UTF16_BigEndian,   2},
    {"UTF16LE", ZCHARSET_UTF16LE,   UCNV_UTF16_LittleEndian,2},
    {"UTF32BE", ZCHARSET_UTF32BE,   UCNV_UTF32_BigEndian,   4},
    {"UTF32LE", ZCHARSET_UTF32LE,   UCNV_UTF32_LittleEndian,4},
    {"EBCDIC", ZCHARSET_EBCDIC,     UCNV_EBCDIC_STATEFUL,   1},
    {"SCSU", ZCHARSET_SCSU,         UCNV_SCSU,              1},
    {"BOCU1", ZCHARSET_BOCU1,       UCNV_BOCU1,             1},
    {"ISO_2022", ZCHARSET_2022,     UCNV_ISO_2022,          1},
    {"IMAP_MAILBOX", ZCHARSET_IMAP_MAILBOX,UCNV_IMAP_MAILBOX,1},
    {"COMPOUND_TEXT", ZCHARSET_COMPOUND_TEXT,UCNV_COMPOUND_TEXT,1},
    {"CESU8", ZCHARSET_CESU8,       UCNV_CESU8,             1},
    {"HZ", ZCHARSET_HZ,             UCNV_HZ,                1},        /* HanZi : chineese */
    {"SBCS", ZCHARSET_SBCS,         UCNV_SBCS,              1},
    {"DBCS", ZCHARSET_DBCS,         UCNV_DBCS,              1},
    {"LMBCS_1", ZCHARSET_LMBCS_1,   UCNV_LMBCS_1,           1},
    {"LMBCS_2", ZCHARSET_LMBCS_2,   UCNV_LMBCS_2,           1},
    {"LMBCS_3", ZCHARSET_LMBCS_3,   UCNV_LMBCS_3,           1},
    {"LMBCS_4", ZCHARSET_LMBCS_4,UCNV_LMBCS_4,              1},
    {"LMBCS_5", ZCHARSET_LMBCS_5,UCNV_LMBCS_5,              1},
    {"LMBCS_6", ZCHARSET_LMBCS_6,UCNV_LMBCS_6,              1},
    {"LMBCS_11", ZCHARSET_LMBCS_11,UCNV_LMBCS_11,           1},
    {"LMBCS_16", ZCHARSET_LMBCS_16,UCNV_LMBCS_16,           1},
    {"LMBCS_17", ZCHARSET_LMBCS_17,UCNV_LMBCS_17,           1},
    {"LMBCS_18", ZCHARSET_LMBCS_18,UCNV_LMBCS_18,           1},
    {"LMBCS_19", ZCHARSET_LMBCS_19,UCNV_LMBCS_19,           1},
    {"ISCII", ZCHARSET_ISCII,UCNV_ISCII,                    1},
/*  other converters supported by ZCharset_type but not by icu algo internal converters */
    {"ISO_8859_2", ZCHARSET_8859_2,UCNV_UNSUPPORTED_CONVERTER,           1},
    {"LATIN_2", ZCHARSET_8859_2,UCNV_UNSUPPORTED_CONVERTER,              1},
    {"ISO8859_3", ZCHARSET_8859_3,UCNV_UNSUPPORTED_CONVERTER,           1},
    {"ISO8859_4", ZCHARSET_8859_4,UCNV_UNSUPPORTED_CONVERTER,           1},
    {"ISO8859_5", ZCHARSET_8859_5,UCNV_UNSUPPORTED_CONVERTER,           1},
    {"ISO8859_6", ZCHARSET_8859_6,UCNV_UNSUPPORTED_CONVERTER,           1},
    {"ISO8859_7", ZCHARSET_8859_7,UCNV_UNSUPPORTED_CONVERTER,           1},
    {"ISO8859_8", ZCHARSET_8859_8,UCNV_UNSUPPORTED_CONVERTER,           1},
    {"ISO8859_9", ZCHARSET_8859_9,UCNV_UNSUPPORTED_CONVERTER,           1},
/*  other converters available but not supported by ZCharset_type and not by icu algo internal converters */
    {"NOTSUPPORTED", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1},

    {"ISO8859_10", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, //Latin 6 (Nordic)
    {"ISO8859_11", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-11 Thai
    {"ISO8859_13", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-13 Latin 7 (Baltic Rim)
    {"ISO8859_14", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-14 Latin 8 (Celtic)
    {"ISO8859_15", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-15 Latin 9 (Western Europe)

    {"EUC_CN", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Simplified Chinese Combined (367 + 1382)
    {"EUC_KR", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Korean EUC Combined (367 + 971)
    {"EUC_JP", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Japanese Combined (895 + 952 + 896 + 953)
    {"EUC_TW", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Taiwan Extended UNIX Code (CNS 11643-1986), Combined (367 + 960 + 961)
    {"UCS2", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	UCS-2 (Really UTF-16 BE)
    {"CP037", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM® EBCDIC US English
    {"CP037_S390", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC US English LF & NL reversed
    {"CP256", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Netherlands
    {"CP259", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Symbols Set 7
    {"CP273", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC German
    {"CP274", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Belgium
    {"CP275", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Brazil
    {"CP276", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, //	IBM EBCDIC French-Canada
    {"CP277", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Danish
    {"CP278", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Swedish
    {"CP280", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Italian
    {"CP282", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Portugal
    {"CP284", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Latin American Spanish
    {"CP285", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC UK English
    {"CP290", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Japanese Katakana
    {"CP297", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC French
    {"CP420", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Arabic
    {"CP421", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Maghreb/French
    {"CP423", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Greek
    {"CP424", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Latin/Hebrew
    {"CP437", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS US English
    {"CP500", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC 500V1
    {"CP708", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Arabic (ASMO 708)
    {"CP709", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Arabic (ASMO 449+, BCON V4)
    {"CP710", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Arabic (Transparent Arabic)
    {"CP720", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Arabic (Transparent ASMO)
    {"CP737", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	reek (formerly 437G)
    {"CP770", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Lithuanian Standard RST 1095-89
    {"CP771", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	KBL (Lithuanian and Russian characters)
    {"CP772", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Lithuanian Standard LST 1284:1993
    {"CP773", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Lithuanian (Mix of 771 and 775)
    {"CP774", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Lithuanian Standard 1283:1993
    {"CP775", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Baltic
    {"CP776", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Lithuanian 770 extended
    {"CP777", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Lithuanian 771 extended
    {"CP778", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Lithuanian 775 extended
    {"CP790", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Mazovia (Polish + codepage 437 extended characters
    {"CP803", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Hebrew (old)
    {"CP813", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-7 Greek/Latin
    {"CP819", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-1 Latin Alphabet No. 1
    {"CP833", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Korean SBCS
    {"CP834", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Korean DBCS
    {"CP835", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Traditional Chinese DBCS
    {"CP837", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Simplified Chinese DBCS
    {"CP838", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Thai
    {"CP850", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Latin 1
    {"CP851", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Greek
    {"CP852", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Slavic (Latin 1)
    {"CP853", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Turkey Latin 3 (replaced by Latin 5)
    {"CP855", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM Cyrillic (primarily Russian)
    {"CP856", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	PC Hebrew
    {"CP857", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM Turkish (Latin 5)
    {"CP860", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Portuguese
    {"CP861", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Icelandic
    {"CP862", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Hebrew (Migration)
    {"CP863", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Canadian-French
    {"CP864", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	PC Arabic
    {"CP865", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Nordic
    {"CP866", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Russian
    {"CP868", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Urdu
    {"CP869", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, //	IBM Modern Greek
    {"CP870", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Multilingual Latin 2
    {"CP871", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Icelandic
    {"CP872", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	PC Cyrillic with Euro update
    {"CP874", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Thai, superset of TIS 620
    {"CP875", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Greek
    {"CP878", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	KOI-R (Cyrillic)
    {"CP880", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Cyrillic Multilingual
    {"CP899", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	PC Symbols
    {"CP905", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Turkey Latin 3 (replaced by Latin 5)
    {"CP912", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-2; ROECE Latin-2 Mulitlingual
    {"CP913", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-3 Southeast European
    {"CP914", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-4 Baltic
    {"CP915", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-5; Cyrillic; 8-bit ISO
    {"CP916", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-8; Hebrew
    {"CP918", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Urdu
    {"CP920", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO 8859-9; Latin 5
    {"CP921", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO Baltic (8-bit)
    {"CP922", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	ISO Estonia (8-bit)
    {"CP929", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Thai PC double byte
    {"CP930", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Japanese Katakana Extended, Combined (290 + 300)
    {"CP931", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Japanese Latin-Kanji, Combined (037 + 300)
    {"CP932", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Japanese, superset of Shift-JIS, Combined (897 + 301)
    {"CP933", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Korean Combined (833 + 834)
    {"CP934", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Korean PC Combined (891 + 926)
    {"CP935", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Simplified Chinese, Combined (836 + 837)
    {"CP936", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Simplified Chinese, Combined (903 + 928)
    {"CP937", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Traditional Chinese, Combined (037 + 835)
    {"CP938", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Traditional Chinese Combined (904 + 927)
    {"CP939", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Japanese Latin Extended, Combined (1027 + 300)
    {"CP942", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, //	MS-DOS Japanese Kana Combined (1041 + 301)
    {"CP943", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Japanese Combined (1041 + 941)
    {"CP944", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Korean PC Combined (1040 + 926)
    {"CP946", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Simplified Chinese PC Combined (1042 + 928)
    {"CP948", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Traditional Chinese, Combined (1043 + 927)
    {"CP949", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Korean, superset of KS C 5601-1992, Combined (1088 + 951)
    {"CP950", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Traditional Chinese, superset of Big 5, Combined (1114 + 947)
    {"CP1004", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	PC-data Latin-1 extended desktop publishing
    {"CP1006", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Urdu, 8-bit
    {"CP1008", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Arabic, 8-bit ISO/ASCII
    {"CP1025", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Cyrillic
    {"CP1026", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Turkish
    {"CP1027", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Japanese Extended Single Byte
    {"CP1040", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Korean PC extended Single Byte
    {"CP1041", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Japanese PC extended Single Byte
    {"CP1043", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Traditional Chinese extended Single Byte
    {"CP1046", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Arabic
    {"CP1047", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Latin 1 / Open Systems (US 3270)
    {"CP1047_S390", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Latin 1 / Open Systems (US 3270) LF & NL reversed
    {"CP1051", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	HP-UX Latin1
    {"CP1097", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Farsi
    {"CP1098", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Farsi
    {"CP1112", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Baltic Multilingual
    {"CP1114", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Traditional Chinese Single Byte (IBM Big 5)
    {"CP1115", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Simplified Chinese Single Byte (IBM GB)
    {"CP1122", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Estonia
    {"CP1123", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Cyrillic Ukraine
    {"CP1124", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Cyrillic Ukraine 8-bit
    {"CP1130", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Vietnamese
    {"CP1137", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC India
    {"CP1140", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC US (with Euro)
    {"CP1141", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Germany, Austria (with Euro)
    {"CP1142", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Denmark (with Euro)
    {"CP1143", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Sweden (with Euro)
    {"CP1144", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Italy (with Euro)
    {"CP1145", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Spain (with Euro)
    {"CP1146", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC UK Ireland (with Euro)
    {"CP1147", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC France (with Euro)
    {"CP1148", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC International Latin1 (with Euro)
    {"CP1149", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Iceland (with Euro)
    {"CP1153", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Latin2 (with Euro)
    {"CP1154", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Cyrillic (with Euro)
    {"CP1155", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Turkish (with Euro)
    {"CP1156", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Baltic Multilingual (with Euro)
    {"CP1157", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Estonia (with Euro)
    {"CP1158", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Cyrillic Ukraine (with Euro)
    {"CP1159", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	SBCS Traditional Chinese Host (with Euro)
    {"CP1160", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Thailand (with Euro)
    {"CP1164", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Vietnamese (with Euro)
    {"CP1250", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Latin 2 (Central Europe)
    {"CP1251", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Cyrillic (Slavic)
    {"CP1252", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Latin 1 (ANSI), superset of Latin1
    {"CP1253", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Greek
    {"CP1254", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, //	MS Windows Latin 5 (Turkish), superset of ISO 8859-9
    {"CP1255", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Hebrew
    {"CP1256", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Arabic
    {"CP1257", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Baltic Rim
    {"CP1258", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Vietnamese
    {"CP1279", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Hitachi Japanese Katakana Host
    {"CP1361", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS Windows Korean (Johab)
    {"CP1381", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	MS-DOS Simplified Chinese Combined (1115 + 1380)
    {"CP1383", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	China EUC
    {"CP1386", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	GBK Chinese
    {"CP1392", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Simplified Chinese GB18030
    {"CP5026", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Japan Katakana-Kanji Combined (290 + 300)
    {"CP5028", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Japan Mixed Combined (897 + 301)
    {"CP5031", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Simplified Chinese Combined (836 + 837)
    {"CP5033", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Traditional Chinese Combined (037 + 835)
    {"CP5035", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Japan Latin Combined (1027 + 300)
    {"CP5038", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Japan Mixed Combined (1041 + 301)
    {"CP5045", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Korean PC Combined (1088 + 951)
    {"CP5050", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Japanese EUC Combined (895 + 952 + 896 + 953)
    {"CP5488", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Simplified Chinese GB18030
    {"CP9125", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	IBM EBCDIC Korean Combined (833 + 834)
    {"EuroShift_JIS", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	Test code page, Shift-JIS with European characters

    {"GB18030", ZCHARSET_NOTSUPPORTED,UCNV_UNSUPPORTED_CONVERTER,     1}, // 	gb18030 code set

    /*  end */
    {nullptr,ZCHARSET_ERROR,UCNV_UNSUPPORTED_CONVERTER,                 -1}
};


/** @brief charsetUnitSize returns character unit size (number of bytes per character unit) for a given charset.<br>
 * NB: used internally to check compatibility of ZCharset_type with ZString char unit size.
 */
ssize_t charsetUnitSize(ZCharset_type pCharset);
/**
 * @brief getDefaultEncoding gets the system default character encoding name as a string.
 */
const char* getDefaultEncodingName(void);
/**
 * @brief changeDefaultEncoding Force changing the character encoding name
 * that will further be used as system default character encoding .<br>
 * pEncoding must be a valid encoding name recognized by locale and icu.<br>
 * A control is made on given encoding name : it must be a recognized encoding name otherwise a ZS_INVCHARSET status is returned.
 * @warning
 * 1- This is not a system-wide modification and concerns <b>only</b> the current application environment.<br>
 * 2- This changes also system default encoding charset code @see changeDefaultCharset()
 *
 * @return a ZStatus :<br>
 * ZS_SUCCESS if default encoding name has successfully be changed.<br>
 * ZS_INVCHARSET if given name is invalid.
 */
ZStatus changeDefaultEncodingName(const char* pEncoding);

/** @brief getDefaultCharset gets the system default charset as a ZCharset_type. If not yet defined, computes it and sets it global.
 Subsequent calls will deliver the computed system default charset code value */
ZCharset_type getDefaultCharset(void);

/** @brief changeDefaultCharset Force changing the charset code value
 * that will further be used as system default charset code value.
 * This is not a system-wide modification and concerns <b>only</b> the current application environment.
 * @return the charset code as system default.
 */
void changeDefaultCharset(ZCharset_type pNewDefaultCharset);
/**
 * @brief validateCharsetName tests wether a charset (encoding) name given as an input string is acceptable, and returns its useable name.
 */
const char* validateCharsetName (const char* pName);

/** @brief nameToZCharset Extracts from a string containing charset/codeset name to its corresponding ZCharset_type
 * <\b>
 *  Valid  input string may be "blabla -$ UtF-8" gives ZCHARSET_UTF8
 *  or "en-ANSI-X-34-1968" gives ZCHARSET_ASCII
 *  <\b>Returns ZCHARSET_ERROR if not found
 */
ZCharset_type nameToZCharset (const char* pName);

#include <unicode/ucnv.h>
/** @brief ZCharsetToUcnv internal routine for converting ZCharset_type to ICU compatible codeset */
UConverterType ZCharsetToUcnv (ZCharset_type pCharset);
/**
 * @brief decode_ZCharset  returns the canonical name as a string from a ZCharset_type
 */
const char* decode_ZCharset(const ZCharset_type pCharset);
/**
 * @brief encode_ZCharset return a ZCharset_type from a canonical charset name.<br>
 * Name is not case sensitive but must be exact.<br>
 * If Name is not found, ZCHARSET_ERROR is returned.
 */
ZCharset_type encode_ZCharset(const char *pCharset);

struct ZCharsetCol_struct {
    ZCharset_type   Charset;
    char            Language[20];
    int             Confidence;
    void clear() {memset (Language,0,sizeof(Language)); Confidence=0;Charset=ZCHARSET_SYSTEMDEFAULT;}
};

/*  internal icu stuff  see utfkey.cpp for routine content */

UST_Status_type testIcuError(const char*pModule, UErrorCode pCode, bool pWarningSignal, const char *pMessage,...);
UST_Status_type testIcuFatal(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...);

#ifdef __COMMENT__
/**
 * @brief detectCharset tries to detect with what charset is encoded a given string pInString of pByteLen length (could be the first characters of the string).
 *  Returns<br>
 * - a level of confidence of what charset has been found (given as parameter) if not omitted (not nullptr),
 * - a ZCharset_type corresponding to charset found.
 */
ZCharset_type detectCharset(const char*pInString,
                            ssize_t pByteLen=-1,
                            int32_t *pConfidence=nullptr,
                            ZBool pWarningSignal=false);
/**
 * @brief detectCharsetAll tries to detect a list of possible charsets suspected to encode a given string pInString of pByteLen length (could be the first characters of the string).
 *  Returns<br>
 * - a ZArray corresponding to possible charsets found (given as parameter)
 * - number of detected charsets, or -1 if an error occurred.
 */
ssize_t detectCharsetAll(const char*pInString,
                         ssize_t pByteLen,
                         zbs::ZArray<ZCharsetCol_struct>& pCharsetAr,
                         ZBool pWarningSignal=false);
#endif // __COMMENT__
/*
 *  utf strings utilities
 */
/*
bool utfIsSpace (utf16_t pChar);

utf8_t utf8DropAccute(utf8_t pCharacter);

utf8_t utf8Lower(utf8_t pCharacter);

utf8_t utf8Upper(utf8_t pCharacter);

template <class _Utf=char>
_Utf utfUpper(_Utf pCharacter);
template <class _Utf=char>
_Utf utfLower(_Utf pCharacter);

template <class _Utf=char>
_Utf utfDropAccute(_Utf pCharacter);

template <class _Utf=char>
_Utf* utfToUpper(const _Utf*pString,_Utf*pOutString);

template <class _Utf=char>
size_t utfStrlen(const _Utf* pString);


template <class _Utf=char>
_Utf* utfStrcpy(_Utf *s1, const _Utf *s2);

template <class _Utf=char>
_Utf* utfStrncpy(_Utf *s1, const _Utf *s2,size_t pCount);

template <class _Utf=char>
_Utf* utfStrcat(_Utf *wDest, const _Utf *wSrc);

template <class _Utf=char>
_Utf* utfStrncat(_Utf *wDest, const _Utf *wSrc, size_t pCount);

template <class _Utf=char>
_Utf* utfStrchr(const _Utf *wStr,const _Utf wChar);

template <class _Utf=char>
_Utf* utfStrrchr(_Utf* wStr,_Utf wChar);
template <class _Utf=char>
const _Utf* utfStrrchr(const _Utf* wStr,const _Utf wChar);

template <class _Utf=char>
_Utf* utfStrstr(const _Utf *string,_Utf* substring);

template <class _UTF=char>
int utfStrcmp(const _UTF* pString1, _UTF* pString2,int pMaxSize);

template <class _UTF=char>
int utfStrncmp(const _UTF* pString1, _UTF* pString2,int pMaxSize);

template <class _UTF=char>
int utfStrncasecmp(const _UTF* pString1, _UTF* pString2,int pMaxSize);

template <class _Utf=char>
const _Utf* utfFindLastNonChar(const _Utf *pContent,_Utf pChar);

template <class _Utf=char>
const _Utf* utfFindLastNonSet(const _Utf *pContent,_Utf *pDelimiter);

template<class _Utf=char>
_Utf* utfFirstNotinSet(_Utf* pString,const _Utf* pSet);

template<class _Utf=char>
_Utf * utfFirstinSet (_Utf*pString,const _Utf *pSet);

template<class _Utf=char>
_Utf * utfLastinSet(const _Utf *pStr, const _Utf *pSet);

template <class _Utf=char>
_Utf* utfLastNotChar(const _Utf *pStr, const _Utf pChar);

template <class _Utf=char>
_Utf* utfLastNotinSet(const _Utf *pStr, const _Utf* pSet);

template <class _Utf=char>
bool utfIsTerminatedBy(_Utf *pStr,_Utf pChar);

template <class _Utf=char>
_Utf* utfLTrim (_Utf*pString, const _Utf *pSet);

template <class _Utf=char>
_Utf* utfRTrim (_Utf*pString, const _Utf *pSet);

template <class _Utf=char>
_Utf* utfTrim (char*pString, const char *pSet);

template <class _Tp=char>
bool utfIsTerminatedBy(const _Tp *pStr,_Tp pChar);

template<class _Utf=char>
void utfConditionalNL(_Utf *pString);

template<class _Utf=char>
void utfAddConditionalTermination(_Utf *pString,size_t pSize);
*/

#ifdef QT_CORE_LIB
#include <QString>
class ZDataBuffer;

ZStatus _fromQString(ZDataBuffer& pOutZDB,const ZCharset_type pCharset,QString pQString);


#endif // QT_CORE_LIB


ZStatus _fromCharset(unsigned char* pOutString,
                     const size_t pOutSize,
                     const ZCharset_type pOutCharset,
                     const unsigned char* pInString,
                     const size_t pInSize,
                     const ZCharset_type pInCharset);

ssize_t approxCharCountFromSize(const ZCharset_type pCharset, size_t pInSize);
ssize_t approxSizeFromCharCount(const ZCharset_type pCharset, size_t pCharCount);

template <class _Utf>
ssize_t utfCharCount(_Utf* pString, ZStatus *pStatus, _Utf** pUtfEffective, uint8_t *pEndianProcess=nullptr)
{

    switch (sizeof(_Utf))
    {
    case sizeof(utf8_t):
        return utf8CharCount(pString,pStatus,pUtfEffective);
    case sizeof(utf16_t):
        return utf16CharCount(pString,pStatus,pUtfEffective,pEndianProcess) ;
    case sizeof(utf32_t):
        return utf32CharCount(pString,pStatus,pUtfEffective);
    }
}

template <class _Utf>
_Utf utfUpper(_Utf pCharacter)
{
  if (pCharacter >= sizeof(cst_ToUpper))
    return pCharacter;
  return (_Utf)cst_ToUpper[(int)pCharacter];
}//utfUpper

template <class _Utf>
_Utf* utfToUpper(const _Utf*pString,_Utf*pOutString)
{
    if (!pString)
          return nullptr;
    _Utf * wOutString =(_Utf *) pString;
    if (pOutString)
          wOutString=pOutString;

    while (*pString)
      {
       *wOutString++=utfUpper(*pString++);
       }
    return wOutString;
}


template <class _Utf>
 _Utf utfLower(_Utf pCharacter)
{
    return (_Utf)cst_ToLower[(int)pCharacter];
}



template <class _Utf>
bool utfIsSpace (_Utf pChar)
{

    if ((pChar==(_Utf)0x20)||(pChar==(_Utf)0xA0))
                  return true;
    for (int wi=0;wi < cst_UCSSpaces16NB;wi++)
              if (pChar==(_Utf)cst_UCSSpaces16[wi])
                          return true;
    return false;
}// utfIsSpace

template <class _Utf>
 _Utf utfDropAccute(_Utf pCharacter)
{
    if (pCharacter >= sizeof(cst_DropAcute))
              return pCharacter;
    return (_Utf)cst_DropAcute[(int)pCharacter];
}//utfDropAccute

 /*
 Unicode Combining Diacritical Marks
 Range is
 U+0300	u+036F
 */
 inline
 bool
 isDiacritical(const utf32_t pChar)
 {
     return ((pChar>0x2FF)&&(pChar<0x370));
 }


 /**
 * @brief detectBOM detects if string starts with a BOM. Analyzes the BOM if any and returns BOM type
 * return ZBOM_NoBOM if no BOM is detected.
 *
 *  Detected BOMs could be<br>
 *  - utf8
 *  - utf16 big endian
 *  - utf16 little endian
 *  - utf32 big endian
 *  - utf32 little endian
 *  - utf7
 *  - EBCDIC
 *
 * @note parsed string may be at least 4 bytes long. If not, a memory access violation may occur (segmentation fault).
 *
 * see ZBOM_type
 *
 * @param[in] pString   string to analyze. May be utf8_t, utf16_t or utf32_t,...
 * @param[in] pMinByteSize Minimum size in bytes of the string to analyze : minimum size garanteed for BOM search
 *                         or total string size (if ZDataBuffer for instance).
 * @param[out] pBOMSize    returned size of the detected BOM. set to 0 if no BOM is detected.
 * @return a ZBOM_type describing the encountered BOM. Set to ZBOM_NoBOM if no BOM detected.
 */
ZBOM_type detectBOM(const void *pString, size_t pMinByteSize, size_t &pBOMSize);

const utf8_t*  detectUtf8Bom(const utf8_t* pInString, ZBOM_type *pBOM);
const utf16_t* detectUtf16Bom(const utf16_t* pInString, ZBOM_type *pBOM);
const utf32_t* detectUtf32Bom(const utf32_t* pInString, ZBOM_type * pBOM);
/**
 * @brief utf8CharSize  Computes the number of character units within a given utf8 character given as pUtf8
 * @param[in] pUtf8         an utf8 string pointer
 * @param[out] pUnitCount   In case of success : number of character units for character.<br>
 *                          Number of invalid/illegal character units if status is errorred.<br>
 *                          -1 if errored character and endofstring mark has not be found.
 * @return an UST_Status_type with the possible following values<br>
 *  <b>UST_SUCCESS</b>     Found correct utf8 character<br>
 *  <b>UST_ENDOFSTRING</b> End of string mark has been found<br>
 *<br>
 *  <b>UST_BOMUTF8</b> Found utf8 BOM : not an error - string content will follow<br>
 *<br>
 *  <b>UST_ILLEGAL</b> Error : utf8 character is ill formed : value greater than 0X80 and 0x40 bit not set.<br>
 *                     A search for next valid utf8 character unit is performed.<br>
 *                     pUnitCount is set to the number of invalid characters found until a valid utf8 character is found again (either mono or multi-units character).<br>
 *  <b>UST_STRINGOVERFLW</b> Error : utf8 character is ill formed<br>
 *                                   no valid character has been found and no endofstring mark has been found.<br>
 *                                   (search for endofstring mark beyond __STRING_MAX_LENGTH__)<br>
 *                                   pUnitCount is set to -1.<br>
 *                                   Character before this one is to be considered as end of string.<br>
 *  <b>UST_NULLPTR</b> Error: one required argument or more is nullptr<br>
 */
UST_Status_type utf8CharSize(const utf8_t *pInString, size_t *pUnitCount);
UST_Status_type utf8CharSizeChunk(const utf8_t *pInString, size_t*pUnitCount, size_t pChunkSize);
UST_Status_type utf16CharSize(const utf16_t *pUtf16Char, size_t *pUnitCount, ZBool *plittleEndian=nullptr);
// NB utf32 char size is always 1
/**
* @brief utf8CharCount gets the number of character of an utf8 string:<br>
*      - counting multi-byte character as one unique character<br>
*      - controlling and skipping BOM if any<br>
*
* if utfStrCtx::StopOnError boolean flag is set to true, counting will stop at first illegal utf8 character encountered.<br>
* In any cases, utfStrCtx::ErrorQueue will not be fed in this routine.<br>
*
* When routine successfully returns, utfStrCtx::utfGetEffective() template routine will return the effective start of utf8 string, skipping BOM if any encountered.
*
* @note
* 1- String length in character units(bytes) must be <b>at minimum</b> 4 bytes long.
* This is necessary for detecting possible BOM in the beginning of string.<br>
* 2- When an utf8 multi-unit character first byte is encountered, no control is made on units following header unit.
* For counting purposes (and for response time reasons), following units are supposed to be in line with the header they refer to.<br>
* This could explain some discrepancies in counting results and effective decoding.
* A detailed control of unit structure is made within utf8Decode() routine.
*
* @param[in pString utf8 string to count characters from
* @param[out] pCount    number of utf8 characters counted, multi-units characters counting for 1.<br>
*                       errored utf8 characters are counted.
* @param[out] pContext  mandatory string parsing context.
* @return an utfStatus_type value.<br>
*               if positive, everything went OK.<br>
*               <b>UST_SUCCESS</b> if everything went OK<br>
*               <b>UST_EMPTY</b> EndOfString mark has been encountered as first character : string is empty<br>
*               <b>UST_NULLPTR</b> Input string is null or pContext is null<br>
*               <b>UST_IVBOM</b>   if a non-utf8 BOM has been encountered.
*                                  ZException is set with appropriate details. see ZException_min::getLast()<br>
*               <b>UST_ILLEGAL</b> invalid utf8_t character<br>
*/
UST_Status_type utf8CharCount(const utf8_t *pInString,
                              size_t *pCount,
                              utfStrCtx *pContext);

UST_Status_type utf8CharCount(const utf8_t *pInString,
                              size_t *pCount);

UST_Status_type utf8CanonicalCount(const utf8_t* pInString, size_t  *pCount);

UST_Status_type utf8DiacriticalCharCount(const utf8_t* pInString,
                                         size_t  *pCount);

UST_Status_type utf16CharCount(const utf16_t* pInString,
                               size_t  *pCount,
                               utfStrCtx *pContext,
                               ZBool *plittleEndian);

/**
 * @brief utf16CharCount Counts the number of character units of an utf16 string : multi-units characters are counted each for 1 character.<br>
 * Character is validated against its unit value and surrogate pairs if any.
 *
 * As no parsing context is used:<br>
 *  a status below UST_SEVERE stops counting execution,<br>
 *  a status for ill formed character does not counting, errored characters are counted and the very last errored status is returned.<br>
 * @param pInString utf16 string to count
 * @param pCount    returned number of utf16 characters
 * @param plittleEndian optional request for endianness :<br>
 * if set to true : little endian is requested<br>
 * if set to false: big endian is requested<br>
 * if omitted (set to nullptr): current system endianness is requested.
 * @return an UST_Status_type with the following possible values :<br>
 * <b>UST_SUCCESS</b> counting went OK.<br>
 * <b>UST_NULLPTR</b> one required argument is null<br>
 * <b>UST_IVBOM</b>  A leading BOM has been encountered, and this BOM describes an invalid encoding vs requested encoding and endianness (i.e. utf16 little endian while big endian was requested)<br>
 * One of UST_Status_type returned by utf16CharSize()<br>
 * <b>UST_ILLEGAL</b> utf16 char unit is not in utf16 unicode value range (has a value > 0xDFFF)<br>
 * <b>UST_IVUTF16SUR_1</b> Invalid surrogate first character<br>
 * <b>UST_IVUTF16SUR_2</b> Invalid surrogate second character<br>
 * <b>UST_TRUNCCONTBYTE</b> endofstring mark has been encountered while expecting surrogate second character.<br>
 */
UST_Status_type
utf16CharCount(const utf16_t* pInString,
               size_t *pCount,
               ZBool *plittleEndian=nullptr);

UST_Status_type
utf16DiacriticalCharCount(const utf16_t* pInString,
                          size_t *pCount,
                          ZBool *plittleEndian=nullptr);


UST_Status_type utf32CharCount(const utf32_t *pInString,
                               size_t *pCount,
                               utfStrCtx *pContext,
                               ZBool *plittleEndian);

UST_Status_type utf32CharCount(const utf32_t* pInString,
                               size_t  *pCount,
                               ZBool *plittleEndian=nullptr);

UST_Status_type utf32DiacriticalCharCount(const utf32_t* pInString,
                                          size_t  *pCount,
                                          ZBool *plittleEndian=nullptr);

UST_Status_type utf8Encode(utf8_t (&pUtf8)[5],
                            size_t *pUtf8Count,
                            utf32_t pCodepoint,
                            ZBool *pEndianness=nullptr);
/**
 * @brief utf8Decode Extracts the first utf8_t char found from an utf8_t string
 *  and returns its codepoint value as an utf32_t character.<br>
 *  Utf8 character may be multi-byte(unit) or not.<br>
 *  Utf8 character unit(s) count of returned character is returned into pUtf8Count.<br>
 *  In case of invalid/illegal utf8 character, pUtf8Count contains the number of invalid units
 *  and pReplacement is returned as codePoint.<br>
 *  pRepacement is defaulted to cst_Unicode_Replct_utf32 i.e. 0xFFFD
 *
 * @note 1- Muti-units character may be up to 5 character units while standard allows 4 character units at maximum.
 *       This choice is deliberately taken to align with some external utilities.<br>
 *       2- No BOM is allowed. If an utf8 BOM is encountered, then it is treated as illegal character.
 *
 * Depending on returned UST_Status, pUtf8Size has the following content :<br>
 *  <b>UST_SUCCESS</b> : pUtf8Size is set to number of consumed character units from the string for the parsed character<br>
 *  <b>UST_ENDOFSTRING</b> : pUtf8Size is set to 0. pUtf32CodePoint is set to 0.<br>
 *  <b>UST_NULLPTR</b> : pUtf8Size is set to -1. pUtf32CodePoint is set to 0.<br>
 * <br>
 *  <b>UST_ILLEGAL</b> : the first encountered unit value does not correspond to an utf8 admitted value range.<br>
 *                      pUtf8Size is set to 1. Replacement codePoint is returned as pUtf32CodePoint.<br>
 *  <b>UST_MISSCONTBYTE</b> : pUtf8Size is set to the number of detected utf8 character units (according first utf8 byte).<br>
 *                              Replacement codePoint is returned as pUtf32CodePoint.<br>
 *  <b>UST_TRUNCCONTBYTE</b> ; pUtf8Size is set to the number of utf8 character units before endofstring mark.
 *
 * @param[in] pInString   pointer to utf8 string to get first utf8 character
 * @param[out] pUtf32CodePoint    resulting utf32 codepoint
 * @param[out] pUtf8Count number of utf8 character units consumed by the current utf8 character within given pInString.
 * @param[in] pReplacement replacement character as an utf32_t codePoint. Defaulted to 0xFFFD.
 * @param[in[ pRemaingSize optional remaining string size to parse. if not nullptr, it is checked and may induce an UST_TRUNCATEDCHAR.
 * @return an UST_Status_type containing operation status.<br>
 *<br>
 *  <b>UST_SUCCESS</b>     Decoding went OK<br>
 *  <b>UST_ENDOFSTRING</b> End of string mark has been found<br>
 *  <b>UST_NULLPTR</b>     A required argument is null<br>
 *<br>
 *  <b>UST_ILLEGAL</b>         Invalid first utf8 byte (out of utf8 code range)<br>
 *  <b>UST_MISSCONTBYTE</b> Missing continuation byte.
 *  <b>UST_TRUNCCONTBYTE</b> endofmark string found in middle of an utf8 continuation bytes sequence.
 *  <b>UST_TRUNCATEDCHAR</b> pRemainingSize exhausted in the middle of current multi-units character.
 *
 */
UST_Status_type utf8Decode(const utf8_t *pInString,
                           utf32_t *pCodePoint,
                           size_t *pUtf8Count,
                           size_t *pRemainingSize=nullptr);

UST_Status_type utf16Encode(utf16_t *pUtf16Result,
                            size_t *pUtf16Count,
                            utf32_t pCodePoint,
                            ZBool *plittleEndian=nullptr); /**< encode an UTF16 character(s) set from its UTF32 unique value to utf16 string pUtf16Result*/

/**
 * @brief utf16Decode   decode utf16 string pointed (double pointer) by pUtf16String and gives corresponding utf32 character.<br>
 * pUtf16String is updated to point to character unit next to one processed.<br>
 * In case of error other than BOM error, returned utf32 character is set to unicode replacement character.<br>
 *
 * @param[in-out] pUtf16String a pointer to the utf16 character to decode : may be an utf16 string or a pointer to a single utf16 character.
 * @param[out] pUtf32Char  decoded utf32 character or cst_Unicode_Replct_utf32 (0xFFFD replacement character) if errored
 * @param[out] pUnitCount  size in utf16 character units of current utf16 character
 * @param[in] plittleEndian optional : request for little endian (true) or big endian (false).
 *                          If omitted (nullptr), current system endianess is taken as requested.
 * @return an UST_Status_type as utfStatus_type
 *  <b>UST_SUCCESS</b>         decoding went OK<br>
 *  <b>UST_ENDOFSTRING</b>      End of string mark has been found<br>
 *  <b>UST_BOMUTF16BIG</b>      found utf16 BOM big endian while big endian was requested : not an error - string content will follow<br>
 *  <b>UST_BOMUTF16LITTLE</b>   found utf16 BOM little endian while little endian was requested : not an error - string content will follow<br>
 *
 *  <b>UST_IVBOMUTF16BIG</b>    Error: found utf16 BOM big endian while little endian was requested<br>
 *  <b>UST_IVBOMUTF16LITTLE</b>  Error: found utf16 BOM little endian while big endian was requested<br>
 *
 *  <b>UST_IVUTF16SURR_1</b>  Error : invalid surrogate first character<br>
 *  <b>UST_IVUTF16SURR_2</b>  Error : invalid surrogate second character<br>
 *
 *  <b>UST_ILLEGAL</b>      Error : an utf16 character unit has been found with an invalid Unicode value greater than 0xDFFF<br>
 *  <b>UST_IVCODEPOINT</b>  Error : resulting codepoint has an invalid Unicode value greater than 0x10FFFF<br>
 *
 *  <b>UST_NULLPTR</b>       Error: one required argument or more is nullptr<br>
 */
UST_Status_type utf16Decode(const utf16_t *pUtf16String,
                            utf32_t* pCodePoint,
                            size_t *pUnitCount,
                            ZBool *plittleEndian=nullptr);
/**
 * @brief utf32Encode checks and encodes an utf32_t character codepoint : performs little/big endian conversion when necessary or at request.
 *
 * @param[out] pUtf32String resulting utf32_t character
 * @param[in] pUtf32Char
 * @param[in] plittleEndian (optional) explicit mention of Endianness of the utf32 character to encode.<br>
 *              <b>true</b> : input is requested to be little endian<br>
 *              <b>false</b>: input is requested to be big endian<br>
 *              <b>omitted</b>: system current endianness is requested<br>
 * @return UST_Status_type <br>
 *   <b>UST_SUCCESS</b> if encoding went well<br>
 *   <b>UST_IVCODEPOINT</b> if given value is an invalid Unicode utf32 value (greater than __UNICODE_MAX_CODEPOINT__).<br>
 *      In this case, pUtf32Result is set to utf32 replacement character.
 */
UST_Status_type utf32Encode(utf32_t *pUtf32Result,
                            utf32_t pUtf32Char,
                            ZBool *plittleEndian=nullptr);

/**
 * @brief utf32Decode decodes to utf32 (pUtf32Char) the first utf32 character pointed by pUtf32String according given optional endianness.<br>
 * Does not detect any BOM. BOMs are treated as invalid Codepoints.
 *
 * @param[in] pUtf32String      utf32 pointer to current character to decode.
 * @param[out] pCodePoint       resulting utf32 character codepoint according requested endianness (see plittleEndian)
 * @param[out] pUtf32Size       size of current char in string in utf32 character units : always 1 (used for homogeneity of routine calling)
 * @param[in] plittleEndian     optional requested little endian (true) or big endian (false).<br>
 *                          If omitted (nullptr) current system endianness is taken as requested.
 * @return an UST_Status_type with operation status:<br>
 *
 *  <b>UST_SUCCESS</b>        decoding went OK<br>
 *  <b>UST_ENDOFSTRING</b>     End of string mark has been found<br>
 *<br>
 *   <b>UST_IVCODEPOINT</b> if given value is an invalid Unicode utf32 value.<br>
 *      In this case, pCodePoint is set to utf32 replacement character value.
 */
UST_Status_type utf32Decode(const utf32_t *pUtf32String,
                            utf32_t* pCodePoint,
                            size_t *pUtf32Size,
                            ZBool *plittleEndian=nullptr);
/**
 * @brief utf32ToChar tries to convert an utf32 character to a single byte char.
 *              If not possible, then returns false and set character to substitution char __UTF_SUBSTITUTION_CHAR__
 *              If possible, then returns true and set pCharOut to the corresponding character.
 * @param pCharOut  resulting char or __UTF_SUBSTITUTION_CHAR__
 * @param pCharIn   utf32 character to transform
 * @return          true if transformation is OK, false if __UTF_SUBSTITUTION_CHAR__ is used
 */
bool utf32ToChar(char *pCharOut, const utf32_t pCharIn);

utf8_t* addUtf8Bom(utf8_t*pString,size_t*pByteSize);
utf16_t* addUtf16Bom(utf16_t* pString,size_t*pByteSize, ZBool* plittleEndian);
utf32_t* addUtf32Bom(utf32_t* pString, size_t*pByteSize, ZBool *plittleEndian);

/**
 * @brief utf8StrToCharStr Converts an utf8 string of size (in bytes) pInSize to a char string with charset pOutCharset.
 *  @param pOutCharStr Output char string is allocated and therefore needs to be freed by calling routine.<br>
 *  @param pOutCharset a ZCharset_type mentionning the encoding of the output char string.
 *  @param pInString   utf8 string to store converted content.
 *  @param pInSize must be set with the input string length <b>in bytes</b> (not in characters), or -1 (default value)<br>
 *  In this case, first endofstring mark will be searched to define input string length.<br>
 *  @param pWarningSignal is a boolean that allows to generate a ZException warning description in case of warning if set to true.
 *  Defaulted to false : warning is signaled by return status but no ZException is generated.
 *  @return A ZStatus is returned with<br>
 * - <b>ZS_SUCCESS</b> when everything went well<br>
 * - <b>ZS_INVCHARSET</b>  destination charset is invalid; either character unit size is inappropriate, or ZCharset code is not recognized.
 *  In this case, ZException is set with appropriate messages<br>
 * - <b>ZS_ICUERROR</b>  an error occurred during charset conversion. In this case, ZException is set with appropriate message<br>
 * - <b>ZS_MEMERROR</b> Routine cannot allocate memory (Severity_Fatal severity of error).<br>
 *   In this case, and only if pWarningSignal is set to true, ZException is set with an explainatory message.<br>
 *   @see ZStatus
 */
ZStatus utf8StrToCharStr(char **pOutCharStr,
                         ZCharset_type pOutCharset,
                         const utf8_t* pInString,
                         const ssize_t pInByteSize=-1,
                         ZBool pWarningSignal=false);
/**
 * @brief utf16StrToCharStr Converts an utf16 string of size (in bytes) pInSize to a char string with charset pOutCharset.
 *  @param pOutCharStr Output char string is allocated and therefore needs to be freed by calling routine.<br>
 *  @param pOutCharset a ZCharset_type mentionning the encoding of the output char string.
 *  @param pInString   utf16 string to store converted content.
 *  @param pInSize must be set with the input string length <b>in bytes</b> (not in characters), or -1 (default value)<br>
 *  In this case, first endofstring mark will be searched to define input string length.<br>
 *  @param pWarningSignal is a boolean that allows to generate a ZException warning description in case of warning if set to true.<br>
 *  Warning status may then be tested using ZException_min::getLastStatus() routine.
 *  ZException_min::getLast() will return the full content of statuses, severity and messages.<br>
 *  Nota Bene: this concerns the global defined object ZException.<br>
 *  Defaulted to false :no ZException is generated.
 *  @param plittleEndian a pointer to an optional boolean that defines requested endianess from the input string :<br>
 *  if omitted (nullptr) system current endianness is requested.<br>
 *  If set to true, then little endian is explicitely requested.<br>
 *  if set to false, then big endian is explicitely requested<br>.
 *  @return A ZStatus is returned with<br>
 * - <b>ZS_SUCCESS</b> when everything went well<br>
 * - <b>ZS_INVCHARSET</b>  destination charset is invalid; either character unit size is inappropriate, or ZCharset code is not recognized.
 * In this case, ZException is set with appropriate messages<br>
 * - <b>ZS_ICUERROR</b>  an error occurred during charset conversion. In this case, ZException is set with appropriate message<br>
 * - <b>ZS_MEMERROR</b> Routine cannot allocate memory (Severity_Fatal severity of error).<br>
 *   In this case, and only if pWarningSignal is set to true, ZException is set with an explainatory message.<br>
 *   @see ZStatus
 */
ZStatus utf16StrToCharStr(char **pOutCharStr,
                          ZCharset_type pOutCharset,
                          const utf16_t* pInString,
                          const ssize_t pInSize,
                          ZBool pWarningSignal=false,
                          ZBool *plittleEndian=nullptr);
/**
 * @brief utf32StrToCharStr Converts an utf32 string of size (in bytes) pInSize to a char string with charset pOutCharset.
 *  @param pOutCharStr Output char string is allocated and therefore needs to be freed by calling routine.<br>
 *  @param pOutCharset a ZCharset_type mentionning the encoding of the output char string.
 *  @param pInString   utf32 string to store converted content.
 *  @param pInSize must be set with the input string length <b>in bytes</b> (not in characters), or -1 (default value)<br>
 *  In this case, first endofstring mark will be searched to define input string length.<br>
 *  @param pWarningSignal is a boolean that allows to generate a ZException warning description in case of warning if set to true.
 *  Defaulted to false : warning is signaled by return status but no ZException is generated.
 *  @param plittleEndian a pointer to an optional boolean that defines requested endianess from the input string :<br>
 *  if omitted (nullptr) system current endianness is requested.<br>
 *  If set to true, then little endian is explicitely requested.<br>
 *  if set to false, then big endian is explicitely requested<br>.
 *  @return A ZStatus is returned with<br>
 * - <b>ZS_SUCCESS</b> when everything went well<br>
 * - <b>ZS_INVCHARSET</b>  destination charset is invalid; either character unit size is inappropriate, or ZCharset code is not recognized.
 * In this case, ZException is set with appropriate messages<br>
 * - <b>ZS_ICUERROR</b>  an error occurred during charset conversion. In this case, ZException is set with appropriate messages<br>
 * - <b>ZS_MEMERROR</b> Routine cannot allocate memory (Severity_Fatal severity of error).<br>
 *   In this case, and only if pWarningSignal is set to true, ZException is set with an explainatory message.<br>
 *   @see ZStatus
 */
ZStatus utf32StrToCharStr(char **pOutCharStr,
                          ZCharset_type pOutCharset,
                          const utf32_t* pInString,
                          const ssize_t pInSize,
                          ZBool pWarningSignal=false,
                          ZBool *plittleEndian=nullptr);
/*
UST_Status_type utf8StrToCStr(char **pCStr, const utf8_t* pInString, utfStrCtx* pContext);
UST_Status_type utf16StrToCStr(char **pCStr, const utf16_t *pInString, utfStrCtx *pContext, bool *plittleEndian=nullptr);
UST_Status_type utf32StrToCStr (char **pCStr, const utf32_t* pInString, utfStrCtx* pContext, bool *pEndianProcess=nullptr);
*/


//========================Single character Unicode conversion ========================

UST_Status_type
singleUtf8toUtf16 (utf16_t* pOutChar,size_t* pOutCount,
                   utf8_t*pInChar,size_t pInSize,
                   size_t* pCurrLen,size_t pMaxLength);

UST_Status_type
singleUtf8toUtf32 (utf32_t *pOutChar, size_t* pOutSize,
                   utf8_t*pInChar, size_t pInSize,
                   size_t* pCurrLen, size_t pMaxLength);

UST_Status_type
singleUtf16toUtf8 (utf8_t* pOutChar,size_t* pOutCount,
                   utf16_t*pInChar,size_t pInSize,
                   size_t* pCurrLen,size_t pMaxLength);

UST_Status_type
singleUtf16toUtf32 (utf32_t* pOutChar,size_t* pOutSize,
                    utf16_t*pInChar,size_t pInSize,
                    size_t* pCurrLen,size_t pMaxLength);

UST_Status_type
singleUtf32toUtf8 (utf8_t* pOutChar,size_t* pOutCount,
                   utf32_t*pInChar,size_t pInSize,
                   size_t* pCurrLen,size_t pMaxLength);

UST_Status_type
singleUtf32toUtf16 (utf16_t *pOutChar, size_t* pOutCount,
                    utf32_t pInChar, size_t pInSize,
                    size_t* pCurrLen, size_t pMaxLength);


//============End single Unicode character conversion=================
class ZDataBuffer;

ZStatus iconvFromTo(const ZCharset_type pFromCharset,
                    const unsigned char* pInString,
                    size_t pInSize,
                    const ZCharset_type pToCharset,
                    ZDataBuffer *pOutString);

template <size_t _Sz> class templateString;


template <size_t _Sz>
ZStatus _fromISOLatin1(templateString<_Sz>& pOutString,const unsigned char* pInString, size_t pInSize);
template <size_t _Sz>
ZStatus _toISOLatin1(ZDataBuffer& pOutString,templateString<_Sz>& pinString);

ZCharset_type getZCharsetFromXMLEncoding(int8_t pXMLCode);
int8_t getXMLEncodingFromZCharset(ZCharset_type pZCharCode);

/**
 * @brief guessZCharset tries to guess what is the ZCharset_type corresponding to given string pString with byte length plen
 * @return
 */
ZCharset_type guessZCharset (const uint8_t *pString, size_t pLen);

const char* decode_XMLCharEncoding(int8_t pEncoding);

UST_Status_type testIcuError(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...);
UST_Status_type testIcuFatal(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...);

ZStatus ztestIcuError(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...);
ZStatus ztestIcuFatal(const char*pModule,UErrorCode pCode,bool pWarningSignal,const char* pMessage,...);



#endif // ZCHARSET_H
