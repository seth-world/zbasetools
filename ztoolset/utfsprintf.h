#ifndef UTFSPRINTF_H
#define UTFSPRINTF_H
/** @file utfsprintf.h
 *  @brief Multi Unicode character encoding sprintf equivalent with format conversions.
 *
 *
 * @author Gerard POLO @sethWorld
 * @date November, 2018
 *
 * @version Version 1 : Gerard POLO  11/2018
 *
 * This code is mainly based on the following documentation <br>
 *  for GNU : [gcc documentation] (https://www.gnu.org/software/libc/manual/html_node/Formatted-Output.html#Formatted-Output)
 *  for Microsoft :[Microsoft documentation] (https://docs.microsoft.com/en-us/cpp/c-runtime-library/format-specification-syntax-printf-and-wprintf-functions?view=vs-2017)
 * <br>
 *
 *
 * @par Conversion routines
 *
 * For Unicode encoding formats translation and handling zbase built-in routines are used.<br>
 * Mainly, the following routines :<br>
 * - utf8Encode()
 * - utf8Decode()
 * - utf8CharCount()
 * - utf16Encode()
 * - utf16Decode()
 * - utf16CharCount()
 * - utf32Encode()
 * - utf32Decode()
 *
 * For other encodings, a generic converter is used.<br>
 * @see zicuConverter
 *
 * @remark As utfSprintf functions suite is made to convert primarily Unicode format, utf8, utf16 and utf32 are processed directly,
 * while requesting an output buffer which is not of these types will first be processed as utf16 then converted back to requested format using a generic converter.
 *
 * It is to be noted that utf8, utf16 or utf32 string arguments may be prefixed by a corresponding BOM (Byte Order Mark), without impact on the  string content :
 * Only the string content is used and the BOM itself is disguarded, excepted if BOM does not correspond to expected utf format.
 *
 * Please refer to  zbs::CHARACTER_ENCODING   <br>
 *
 \htmlonly <h2><a NAME="Functions">Functions list</a></h2> \endhtmlonly
 <table border="1" cols="2" align="center">
   <tr>
      <th><strong>Main functions name</strong></th>
      <th><strong>Description</strong></th>
    </tr>
    <tr>
      <td>utfSprintf()</td>
      <td>main routine : limited to cst_MaxSprintfBufferCount character units</td>
    </tr>
    <tr>
      <td>utfSnprintf()</td>
      <td>snprintf equivalent</td>
    </tr>
    <tr>
      <td>utfVsnprintf()</td>
      <td>vsnprintf equivalent</td>
    </tr>
    <tr>
      <td>utfCheckFormat()</td>
      <td>parses format and prints various flags and given fields characteristics.</td>
    </tr>
    <tr>
      <td>utfCheckFullFormat() </td>
      <td>checks the full format string, parses format and vars and returns the size of each field as well as the size of the whole buffer.</td>
    </tr>
    <tr>
      <td><strong>Utility routines</strong></td>
      <td><strong>Description</strong></td>
    </tr>
    <tr>
      <td>utfFFToUtf()</td>
      <td>template routine to convert a long double to a string using 'f' format according a precision (leading sign if negative value).</td>
   <tr>
      <td>utfFEToUtf() </td>
      <td>template routine to convert a long double to a string using scientific 'e' format according a precision (leading sign if negative value).</td>
    </tr>
    <tr>
      <td>utfSPFConvertInt()</td>
      <td>template routine to convert an integer, long or long long to a string. Numbers are returned in reverse order : from least significant to most significant digit.</td>
    </tr>
    <tr>
      <td><strong>Locale symbols management</strong></td>
      <td><strong>Description</strong></td>
    </tr>
    <tr>
      <td>GGetLocaleCodes()</td>
      <td>sets or resets locale dependent data using a valid locale name.</td>
    </tr>
    <tr>
      <td>getLocaleCurrencyCode()</td>
      <td>template routine returning the locale defined currency code as an _utf unicode string (utf8_t, utf16_t or utf32_t).</td>
    </tr>
    <tr>
      <td>getLocaleDecimalPoint()</td>
      <td>template routine to obtain locale defined decimal point character as a single _Utf character(template parameter).</td>
    </tr>
    <tr>
      <td>getLocaleGroupSeparator()</td>
      <td>template routine returning locale defined monetary thousand separator as a single _Utf character unit.</td>
    </tr>
    <tr>
      <td>getLocaleGroupNumber()</td>
      <td>returns the grouping number : number of digits per group separated by monetary separator.</td>
    </tr>
    <tr>
      <td>getLocaleCurrencyPosition()</td>
      <td>returns the locale currency sign position vs monetary amount : used as default by format 'k'./td>
    </tr>
    <tr>
      <td>getLocaleSignPosition()</td>
      <td>returns the locale sign position vs monetary amount : used as default by format 'k'</td>
    </tr>
  </table>
 *
 */

#include <zconfig.h>

#include <sys/types.h>

#include <stdarg.h>
#include <stdint.h>
#include <ztoolset/zcharset.h>
#include <ztoolset/utfutils.h>
#include <ztoolset/zlimit.h>
#include <strings.h>
#include <ctype.h>
#include <cmath>

#include <ztoolset/zsymbols.h>

#include <ztoolset/zlocale.h>

#define __HAVE_LONG_DOUBLE__    true
#define __HAVE_LONG_LONG__      true
//#define __F_NUM_GENERIC__

/* uncomment herebelow if you need to suppress inline clauses  and therefore to make code more compact  */
//#define __COMPACT_CODE__

#define __INIT_INTEGER_DIGITS__     25

/* hereafter preprocessor symbol defining rounding algorithm for various float format
 *
Rounding method :
 false : rounding is made on first decimal digit  + round coefficient then truncated
 true  : rounding is made when first decimal digit  > round coefficient, first decimal digit  + round coefficient then truncated, else no rounding.

GFloat_F_Rnd_GT concerns <%f> format and <%g> format when exponent >-4 and < precision
GFloat_E_Rnd_GT concerns <%e> format and <%g> format when exponent <-4 or > precision
GFloat_A_Rnd_GT concerns <%a> format (float hexadecimal)
GFloat_K_Rnd_GT concerns <%k> format (amount)

*/


static ZBool GFloat_F_Rnd_GT=false;
static ZBool GFloat_G_Rnd_GT=false;
static ZBool GFloat_K_Rnd_GT=false;
static ZBool GFloat_E_Rnd_GT=false;
static ZBool GFloat_A_Rnd_GT=false;


void set_Float_F_Rounding_GT(ZBool pGT);
void set_Float_E_Rounding_GT(ZBool pGT);
void set_Float_A_Rounding_GT(ZBool pGT);

# define VA_LOCAL_DECL   va_list ap
# define VA_START(f)     va_start(ap, f)
# define VA_SHIFT(v,t)  ;       /* no-op for ANSI */
//#  define VA_SHIFT(v,t) v = va_arg(ap,t)
# define VA_END          va_end(ap)

#define __DEFAULT_SPRINTF_BUFFERCOUNT__ 2048

static const size_t cst_MaxSprintfBufferCount=__DEFAULT_SPRINTF_BUFFERCOUNT__;
static size_t MaxSprintfBufferCount=__DEFAULT_SPRINTF_BUFFERCOUNT__;

static void setMaxSprintfBufferCount(size_t pValue) {MaxSprintfBufferCount=pValue;}
static void setMaxSprintfBufferCountToDefault() {MaxSprintfBufferCount=__DEFAULT_SPRINTF_BUFFERCOUNT__;}

typedef utf8_t      utfFmt; /* implementation dependent type for pFormat : utf8 by default */

#ifdef __HAVE_LONG_DOUBLE__
#define LONG_DOUBLE long double
#else
#define LONG_DOUBLE double
#endif

#ifdef __HAVE_LONG_LONG__
#define LONG_LONG   long long
#define ULONG_LONG  unsigned long long
#else
#define LONG_LONG   long
#define ULONG_LONG  unsigned long
#endif

#define __USE_LONG_LONG__   true
#define __USE_LONG_DOUBLE__ true


#define __UNIT_THRESHOLD__ 100

/**  @brief abs_val returns absolute value of double (or long double) value */
LONG_DOUBLE abs_val(LONG_DOUBLE value);



/**  @brief powB returns power of Base given by exp : In case of pow or powl has a problem */
LONG_DOUBLE powB(LONG_DOUBLE pBase,LONG_DOUBLE pExp) ;
/**  @brief utfRound returns a long rounded to integer value from a double or long double */
long utfRound(LONG_DOUBLE value, double pBase=10);


/**
 *
 * @defgroup UTFSPRINTF_FORMAT utfSprintf functions suite
 * @{
 * utfSprintf, and following functions, comply with  ISO/IEC 9899:1999, printf() and sprintf() definition.<br>
 *
 * @par Output buffer existence and capacity
 *
 * In contrary of utfSnprintf, utfSprintf does not include a maximum number of character units for output buffer.<br>
 * However, output buffer is limitated to defined constant ccst_MaxSprintfBufferCount character units.<br>
 *
 * When this capacity overflows, processing continues without concrete output to buffer and returned size reflects necessary character units (including end of string mark) to fullfill the operation,
 * while output buffer content is limitated to its capacity.<br>
 * This behavior is the same for utfSnprintf function.<br>
 *
 * When output buffer is null (nullptr or NULL), then a full processing occurs without any output : returned size corresponds to the requested character units for fullfilling the operation (including end of string mark).<br>
 *
 * @par Charset
 *
 * - format string charset : it must be utf8 (utf8_t* string).<br>
 * - output buffer charset : could be one of the following depending on pCharset parameter:<br>
 *
 *  . Unicode: correspondance/translation is made using built-in routines
 *      + utf8
 *      + utf16
 *      + utf32
 *
 *  . other encodings : output buffer is considered as a char* string with a special encoding.<br>
 *  all processing is made using utf16 encoding then translated back to target encoding using icu external conversion routines.
 *
 * @par Format specifications
 *
 * Format specification is the one used by GNU sprintf (with some additional facilities described below - see 'Additional float format specifier') :<br>
 *  %[parameter][flags][width][.precision][length]type

 [printf format specification] (https://en.wikipedia.org/wiki/Printf_format_string) <br>

 <strong>Flags for field specifiers</strong><br>

  <b>Sign flag</b><br>
    If both the ' ' and '+' flags appear, the ' ' flag will be ignored<br>
  <b>Padding (left and right)</b><br>
    Space padding is done under the constraint of maximum field allowed if any mentionned.<br><br>

  <b>Zero padding</b><br>
    The result of zero padding flag with non-numeric conversion specifier is undefined.<br>
    Solaris and HPUX 10 does zero padding in this case, Digital Unix and Linux does not.<br><br>
    Retained strategy:<br>
    Zero padding is disabled for non-numeric conversion i.e. characters and strings.<br><br>

  <b>Field width</b><br>
    Field width specifier represents the minimum size that the field will use within output buffer.<br>
    If actual field width is less than this minimum size, then field will be padded either with spaces or with zeroes.<br>
    Negative field width argument value means left justified (and only as argument i.e. '*' option )<b


  <b>Precision and Maximum allowed field length</b><br>
    For floatting point fields (double and long double), precision field is used as maximum number of digits at the right of decimal point, excepted
    for field specifier 'g' or 'G' : precision field represents the number of significant digits for the value.<br>
    For other field specifiers (strings,integers)  : precision represents the maximum number of canonical characters allowed for the field (including padding).<br>
    Number of canonical characters does NOT mean number of character units : one utf8 canonical character may have up to 4 character units.<br><br>

    No negative value is allowed for precision. If a negative value is encountered here, it is ignored, and precision (or maximum length depending the case) is reputated not mentionned.<br>

    <b>Nota Bene:</b> Field width and maximum length for strings<br>
    Field width and field maximum length are counted as canonical characters (<b>neither character units nor bytes</b>).<br>
    For example, an utf8 character composed of 3 character units is counted for 1 character.<br>
    Idem for utf16 surrogate pairs that are counted as one unique character.<br>
    This count is made in canonical characters while output buffer length is counted in character units.<br>

    <b>Unicode Combining Diacritical Marks</b><br>
    Within some alphabets, as Cyrillic for example, it may be used special marks that modify the aspect of character it relates to :<br>
    one regular character may be followed by a mark, a diacritical mark, that will apply to the preceeding character and modifies its aspect.<br>
    These signs are called 'Unicode combining diacritical marks' and are in the range [U+0300 ,U+036F].<br>
    The resulting aspect is one unique character made of 2 unicode characters (unicode canonical characters).<br>
    Obviously, as formatting privilegiate the visual aspect, and not the effective underlying characters number, we take this phenomenon into account.<br>
    So, diacritical marks are skipped when counting characters for computing maximum field size, minimum field size and padding.<br>
    This applies for string fields only.<br>

    @example
    Hereafter three versions of the same string with different utf formats to illustrate what has been said herebehind.<br>
    Given the following Russian string "Всё смеша́лось" with an accented (via diacritic) char 'а́',
    we may see the perceived character count as 13.<br>
    According the counting routines, it comes :

    - utf8 : u8"Всё смеша́лось"<br>

    utfStrlen<utf8_t>  :        27      (number of character units)
    utf8CharCount() :           14      (number of canonical characters)
    utf8DiacriticalCharCount() :13      (number of canonical characters diacritcs agregated)

    - utf16 : u"Всё смеша́лось"<br>

    utfStrlen<utf16_t>  :       14      (number of character units)
    utf16CharCount() :          14      (number of canonical characters.Remark: no surrogate pair)
    utf16DiacriticalCharCount():13      (number of canonical characters diacritcs agregated)

    - utf32 : U"Всё смеша́лось"<br>

    utfStrlen<utf32_t>  :       14      (number of character units)
    utf32CharCount() :          14      (number of canonical characters)
    utf32DiacriticalCharCount():13      (number of canonical characters diacritcs agregated)

    @see [Combining diacritical marks] (https://en.wikipedia.org/wiki/Combining_Diacritical_Marks)

 <strong>Field type</strong><br>
 Field type is one of {'d','i','p','z','j','f','F','e','E','g','G','k','K','c','s','S','w','W'}<br>
 it defines the expected type of parameter to be formatted, as well as the way this parameter will be formatted.<br>

 * <b>Single character</b><br>
 * Type specifier<br>
 * 'c' or 'cs'  (synonym) single utf8_t character<br>
 * no multi character units allowed there.<br>
 * Should a multi-units character be inserted, use string specifier in place.<br>
 * 'cS' or 'lc' single utf16_t character<br>
 * no multi character units allowed there.<br>
 * Should a multi-units (subrogated pair) character be inserted, use string specifier in place.<br>
 * 'cW' or 'Lc' single utf32_t character.
 *
 * Nota Bene : minimum field size, left (default) and right space padding applies.<br>
 * Example : \%-8lc puts one utf16 character, aligned left, right padded with 7 spaces (for a total length of 8 characters).<br>
 *
 * <b>Strings</b><br>
 *
 * Strings could be one of utf8, utf16 or utf32 string formats (and only from those format).<br>
 * utf8, utf16 and utf32 could be mixed in one routing call according programming necessities.<br>
 * Strings are converted to appropriate, requested output unicode format.<br>
 * Nota bene: US-ASCII may be used as input string by casting the argument string to utf8 (utf8_t*).<br>
 * For other formats, such as EBCDIC, ISCII, or multi-bytes character sets, strings must be first converted to an appropriate unicode format before being used as arguments.<br>
 * Using a non-authorized string format may induce replacement characters to be put randomly into output buffer.<br>
 *
 * BOM: Byte Order Mark may prefix any string argument content without any impact on string content. BOM itself is disguarded if it corresponds to expected unicode format.
 *<br>
 *
 * 's' ut8_t string, multi-character units are allowed.<br>
 * 'S' or 'w' or 'ls' (all these are synonyms) : utf16_t string. Multi-character units are allowed. Character count for multi-units is 1.<br>
 * 'W' or 'Ls'(synonym) : utf32_t string.
 *
 * <b>Size_t</b>
 *
 * <b>Pointer</b>
 * <b>int_max</b>
 *
 * <b>Integer</b><br>
 * @ref utfFormatLong()
 * <br>
 *
 * <b>Float</b><br>
 * Nota bene: decimal point is defined by locale.<br>
 * To change decimal point, you should use GGetLocaleCodes() routine<br>
 *
 *
 * <b>'f' float format : decimal representation</b><br>
 * @ref FORMATFPF
 * <br>
 *
 *
 * <b>'e' float format : scientific representation</b><br>
 * @ref FORMATFPE
 * <br>
 *
 *
 * <b>'g' float format : mixed representatio</b>n<br>
 * @ref FORMATFPG
 * <br>
 *
 * <b>'a' float format : floating point hexadecimal representation</b><br>
 * @ref FORMATFPA
 *<br>
 *
 * @par Additional float format specifier : Amount (and optional currency)
 *
 * Currency code is defined by locale.<br>
 * It could be changed using GGetLocaleCodes() routine, in specifying a valid country/language locale code.<br>
 * Nota Bene : list of valid country/language codes could be obtained in linux in entering <locale -a> in a console window.<br>
 * <b>Format specifier syntax</b><br>
 * %[parameter][flags][width][.precision][sign position][|][y[currency position][currency spaces]][length]type <br>
 *
 * <b>type</b> : 'k' or 'K' mentions that amount format is to be used.<br>
 * using 'K' rather than 'k' has no impact on value formatting.<br>
 * Nota bene: using one of the following formatting options with a type specifier other than 'k' or 'K' has at least no action, at worst, is an error.<br>
 * <b>sign position</b>: '<' indicates that sign, if it has to be mentionned, must be directly preceeding the amount, while '>' indicates that sign will follow directly the amount.<br>
 * Sign position is independant from currency position (see further).<br>
 * <b>'|'</b>: thousand delimiter. If this sign is mentionned, integral digits are grouped and separated by locale defined thousand delimiter.<br>
 * Grouping of digits is also defined by locale.<br>
 * Western digit grouping is made by 3 digits.<br>
 * @remark when padding with zeroes and thousand separator is the very first sign to be put for the value (collision with padding zero),
 *  neither thousand separator nor padding zero are put in front of the value : it is replaced by a space sign.<br>
 * <b>'y' or 'Y'</b> : forces the currency code before or after (depending on currency position) the formatted value<br>
 * Currency code is defined within locale.<br>
 * currency spaces : spaces are optionally one or more spaces to put between currency code and formatted amount.<br>
 * currency position : could be either '<' currency preceeds formatted amount or '>' currency follows formatted amount.<br>
 * By default, currency position is defaulted to given locale position for monetary amounts.
 *
 * <b>K format specifier examples:</b><br>
 *
 *  \%k<br>
 * simple defaulted amount specifier :<br>
 * value is a double.<br>
 * number of fractional digits are those defined in locale frac_digits field.<br>
 * decimal point is defined in locale mon_decimal_point.<br>
 *
 *  \%08.3Lk<br>
 *  Amount expecting a long double value with minimum 8 characters length (canonical characters), padded with zeroes, with 3 decimal digits.<br>
 *
 *  \%yk<br>
 *  Simple defaulted amount value with currency symbol.<br>
 *  Currency symbol is padded without space to value.<br>
 *  Position of currency symbol vs amount is defaulted to locale definition given by p_cs_precedes.<br>
 *
 *  \%y k<br>
 *  Adds currency symbol to amount with defaulted position.<br>
 *  Currency symbol is separated by one space character from amount.<br>
 *  Remark: Number of spaces mentionned here (between 'y' and 'k') will separate currency symbol from amount formatted value.<br>
 *
 *  \%y>k<br>
 *  Currency symbol is forced to follow amount value.<br>
 *
 *  \%|y>k<br>
 * Same as previous format, but format specifier '|' adds thousand digits separators.<br>
 * Thousand digits separator character is defined in monetary locale mon_thousands_sep.<br>
 *
 *  \%+>|y k<br>
 * Same as previous with following modifications :<br>
 * - Currency symbol is separated with one space from amount value.<br>
 * - sign is forced, even in case of positive value. Sign position is following amount.<br>
 * In this case, sign is directly following amount and has preceedance to currency symbol, if mentionned.<br>
 *
 *
 * @ref FORMATFPK
 *
 *
 * @par Coherence control and size control
 *
 * Format string may be tested using 2 functions
 * utfCheckFormat() : this function controls formatting syntax<br>
 * utfCheckFullFormat() this function controls formatting syntax and returns requested size in character units number of the output buffer.<br>
 * So that, to get the required byte size of the buffer, it is necessary to multiply the returned value by size of one character unit (utf8_t, utf16_t or utf32_t).<br>
 *
 * @see [locale specifications-Open group] (http://pubs.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap07.html)
 *@}
 */

/** * @brief utfSprintf same as sprintf routine adapted to any charset encoding.
 * For syntax and usage details. @see UTFSPRINTF_FORMAT
 *
 * @param pCharset  ZCharset_type of the output buffer.
 * @param pString   Resulting output buffer.
 * @param pFormat   an utfFmt (utf8) string containing formatting instructions
 * @return the size in character units (neither characters nor bytes) of the text written to pString.<br>
 * This number includes end of string mark.<br>
 * If pString is null (nullptr or NULL), then requested size of output buffer in character units is returned (endofstring mark included).<br>
 */
size_t   utfSprintf(ZCharset_type pCharset, void *pString, const utf8_t *pFormat, ...);

/**
 * @brief utfSnprintf equivalent of snprintf for an _Utf string.For syntax and usage details. @see UTFSPRINTF_FORMAT
 * @param pString   an _Utf string that will receive resulting string
 * @param pCount    number of _Utf character units available within pString
 * @param pFormat   anl utf8_t* string with a sprintf format syntax
 * @return number of character units of the resulting string until endofstring mark i.e. (_Utf)'\0' (endofstring mark included)
 */
size_t   utfSnprintf(ZCharset_type pCharset,void *pString, size_t pCount, const utf8_t *pFormat, ...);

/**
 * @brief utfVsnprintf  equivalent of vsnprintf for an _Utf string.For syntax and usage details. @see UTFSPRINTF_FORMAT
 * @param pString       an _Utf string that will receive resulting string
 * @param pCount        number of _Utf character units available within pString
 * @param pFormat       an utf8_t* string with a sprintf format syntax
 * @param arg           list of sprintf arguments under format va_list
 * @return number of character units of the resulting string until endofstring mark i.e. (_Utf)'\0' (endofstring mark included)
 */

size_t utfVsnprintf(ZCharset_type pCharset,void *pBuffer, size_t pMaxlen, const utfFmt *pFormat, va_list args);


template <class _Utf>
void     utfSPFOutChar(_Utf *buffer, size_t * currlen, size_t maxlen,
                           _Utf c);


/* reminder printf format source http://www.cplusplus.com/reference/cstdio/printf/
 *
specifier	Output                                   Example
d or i	Signed decimal integer                          392
u	Unsigned decimal integer                            7235
o	Unsigned octal                                      610
x	Unsigned hexadecimal integer                        7fa
X	Unsigned hexadecimal integer (uppercase)            7FA
f	Decimal floating point, lowercase                   392.65
F	Decimal floating point, uppercase                   392.65
e	Scientific notation (mantissa/exponent), lowercase	3.9265e+2
E	Scientific notation (mantissa/exponent), uppercase	3.9265E+2
g	Use the shortest representation: %e or %f           392.65
G	Use the shortest representation: %E or %F           392.65
a	Hexadecimal floating point, lowercase               -0xc.90fep-2
A	Hexadecimal floating point, uppercase               -0XC.90FEP-2
c	Character                                           a
s	String of characters                                sample
p	Pointer address                                     b8000000
n	Nothing printed.
The corresponding argument must be a pointer to a signed int.
The number of characters written so far is stored in the pointed location.
%	A % followed by another % character will write a single % to the stream.	%


 */

template <class _Utf>
inline int utfChar_to_int(_Utf pChar)
{
    if ((pChar>=(_Utf)__ZERO__)&&(pChar<=(_Utf)__NINE__))
                        return (int) (pChar-(_Utf)__ZERO__);
    return 0;
}

#define MAX(p,q) ((p >= q) ? p : q)


template <class _Utf>
static _Utf cst_HexaNumbers_Lower [] = {
    (_Utf)'0',
    (_Utf)'1',
    (_Utf)'2',
    (_Utf)'3',
    (_Utf)'4',
    (_Utf)'5',
    (_Utf)'6',
    (_Utf)'7',
    (_Utf)'8',
    (_Utf)'9',
    (_Utf)'a',
    (_Utf)'b',
    (_Utf)'c',
    (_Utf)'d',
    (_Utf)'e',
    (_Utf)'f'
};
template <class _Utf>
static _Utf cst_HexaNumbers_Upper [] = {
    (_Utf)'0',
    (_Utf)'1',
    (_Utf)'2',
    (_Utf)'3',
    (_Utf)'4',
    (_Utf)'5',
    (_Utf)'6',
    (_Utf)'7',
    (_Utf)'8',
    (_Utf)'9',
    (_Utf)'A',
    (_Utf)'B',
    (_Utf)'C',
    (_Utf)'D',
    (_Utf)'E',
    (_Utf)'F'
};
/* Not a number  for double is NAN or nan */
template <class _Utf>
static _Utf cst_NAN [] = {
    (_Utf)'N',
    (_Utf)'A',
    (_Utf)'N',
    (_Utf)'\0'
};
template <class _Utf>
static _Utf cst_nan [] = {
    (_Utf)'n',
    (_Utf)'a',
    (_Utf)'n',
    (_Utf)'\0'
};

/* infinite for float is INF or inf */
template <class _Utf>
static _Utf cst_INFINITE [] = {
//    (_Utf)'[',
//    (_Utf)'-',
//    (_Utf)']',
    (_Utf)'I',
    (_Utf)'N',
    (_Utf)'F',
    (_Utf)'\0'
};

template <class _Utf>
static _Utf cst_infinite [] = {
//    (_Utf)'[',
//    (_Utf)'-',
//    (_Utf)']',
    (_Utf)'i',
    (_Utf)'n',
    (_Utf)'f',
    (_Utf)'\0'
};
template <class _Utf>
static _Utf cst_Error_charsetChar [] = {
    (_Utf)'<',
    (_Utf)'c',
    (_Utf)'h',
    (_Utf)'a',
    (_Utf)'r',
    (_Utf)'s',
    (_Utf)'e',
    (_Utf)'t',
    (_Utf)'c',
    (_Utf)'h',
    (_Utf)'a',
    (_Utf)'r',
    (_Utf)'>',
    (_Utf)'\0'
};
template <class _Utf>
static _Utf cst_Error_charset8 [] = {
    (_Utf)'<',
    (_Utf)'c',
    (_Utf)'h',
    (_Utf)'a',
    (_Utf)'r',
    (_Utf)'s',
    (_Utf)'e',
    (_Utf)'t',
    (_Utf)'8',
    (_Utf)'>',
    (_Utf)'\0'
};
template <class _Utf>
static _Utf cst_Error_charset16 [] = {
    (_Utf)'<',
    (_Utf)'c',
    (_Utf)'h',
    (_Utf)'a',
    (_Utf)'r',
    (_Utf)'s',
    (_Utf)'e',
    (_Utf)'t',
    (_Utf)'1',
    (_Utf)'6',
    (_Utf)'>',
    (_Utf)'\0'
};
template <class _Utf>
static _Utf cst_Error_charset32 [] = {
    (_Utf)'<',
    (_Utf)'c',
    (_Utf)'h',
    (_Utf)'a',
    (_Utf)'r',
    (_Utf)'s',
    (_Utf)'e',
    (_Utf)'t',
    (_Utf)'3',
    (_Utf)'2',
    (_Utf)'>',
    (_Utf)'\0'
};


template <class _Utf>
static _Utf cst_Null [] = {
    (_Utf)'<',
    (_Utf)'n',
    (_Utf)'u',
    (_Utf)'l',
    (_Utf)'l',
    (_Utf)'>',
    (_Utf)'\0'
};
template <class _Utf>
size_t utfError(_Utf*pBuffer,size_t *pCurlen,size_t pMaxLen,const _Utf pErrorMsg[])
{
    _Utf* wErrPtr=(_Utf*)pErrorMsg;
    while(*wErrPtr)
        {
        if (pBuffer)
            {
            if (*pCurlen < pMaxLen)
                {
                pBuffer[*pCurlen] = *wErrPtr;
                }
            }
        (*pCurlen)++;
        wErrPtr++;
        }
    return *pCurlen;
}

enum OutEncoding_type:uint8_t
{
    OET_Char,
    OET_UTF8,
    OET_UTF16,
    OET_UTF32
} ;
const char* decode_OET(OutEncoding_type pOET);


typedef uint64_t FMT_Flags_type ;
/**
 * @brief The FMT_Flags enum utfSprintf format options.
 * Standard options fit on a uint32_t word.
 *
 * Extensions (specially currency extensions) for K format, are above.
 *
 *
 */
enum FMT_Flags : FMT_Flags_type
{
    FMTF_Nothing    =0,

/* Flags specifiers */

    FMTF_Left       =        0x00000002, /*<->Left-justify within the given field width; Right justification is the default (see width sub-specifier).*/
    FMTF_Plus       =        0x00000004, /*<+> Forces to preceed the result with a plus or minus sign (+ or -) even for positive numbers. By default, only negative numbers are preceded with a - sign.*/
    FMTF_SignSpace  =        0x00000008, /*< >If no sign is going to be written, a blank space is inserted before the value.*/
    FMTF_ZeroPad    =        0x00000010, /*<0> Left-pads the number with zeroes (0) instead of spaces when padding is specified (see width sub-specifier).*/
    FMTF_Hash       =        0x00000020, /*<#> Used with o, x or X specifiers the value is preceeded with 0, 0x or 0X respectively for values different than zero.*/

/* width & precision specifiers */

    FMTF_Minimum    =        0x00000100, /* a minimum character units (slots) has been mentionned, either hardcoded or as an argument
                                <number>   Minimum number of characters units (slots) to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces. The value is not truncated even if the result is larger.
                                <*> width description The width is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.*/
    FMTF_Precision  =        0x00000200, /* precision or maximum length has been mentionned,either hardcoded or as an argument  */

/* data type & presentation format */

    FMTF_Int        =        0x00001000, /* numeric integer : short, long or long long if exists */
    FMTF_Float      =        0X00002000, /* numeric float : double or long double if exists      */
    FMTF_String     =        0x00004000,
    FMTF_Other      =        0x00008000,
/*  */
    FMTF_ShortInt   =        0x00020000, /* <h> short marker                     */
    FMTF_Long       =        0x00040000, /* <l> long marker                      */
    FMTF_LongLong   =        0x00080000, /* <ll> or <L> long long marker         */

    FMTF_Unsigned   =        0x00100000, /* ignored if float -double or long double- indeed */

/*       digits representation variants  */

    FMTF_Octal      =        0x00200000,     /* octal format : applies to integers only */
    FMTF_Hexa       =        0x00400000,     /* hexadecimal format : applies to integers only */

/*       floating point representation variants         */

    FMTF_Exponent   =        0x01000000, /* float exponential (scientific) notation mantissa exponent -
                                    if not set : normal representation. Corresponds to <e> or <E> */
    FMTF_GFormat    =        0x02000000, /* float representation : shortest between normal vs exponential .
                                    corresponds to <g> or <G> */

    /*  if no flag set for FMTF_Float : then normal notation is taken : corresponds to <f> or <F>
        if flag FMTF_Hexa is set, then it corresponds to <a> or <A>
    */

/*       general purpose flag for digit representation */

    FMTF_Uppercase  =         0x08000000,

/*       out of common format and 'exotic'   */

    FMTF_Pointer    =         0x04000000, /* a pointer is 32 bit on x86 and 64 bit on x64 : so it is unsigned long in any cases */

    FMTF_SizeT      =         0x10000000, /* 'z'    */
    FMTF_PTRDIF_T   =         0x20000000, /* 't'    */
    FMTF_Intmax_t   =         0x40000000, /* 'j'    */

    FMTF_NonSignif  =         0x80000000, /* Remove non significant zeroes */

    FMTF_Amount     = 0x0000000100000000,   /* 'K' Amount */

    FMTF_Currency   = 0x0000000200000000,   /* force currency code follow(default) or preceed value */
    FMTF_Group      = 0x0000000400000000,   /* put thousand grouping separator */

    FMTF_CurPreceeds= 0x0000001000000000,   /* currency code preceeds value. If not mentionned : defaulted to locale option*/
    FMTF_CurFollows = 0x0000002000000000,   /* currency code follows value.  If not mentionned : defaulted to locale option(if no locale: default) */

    FMTF_SignPrec   = 0x0000004000000000,   /* sign preceeds currency amount. If not mentionned : defaulted to locale option(if no locale: default)*/
    FMTF_SignFolw   = 0x0000008000000000,   /* sign follows currency amount.  If not mentionned : defaulted to locale option */

    FMTF_Kilo       = 0x0000010000000000,   /* value is represented divided by 1.0E+3 - default is equivalent to %.3f K<cur> */
    FMTF_Million    = 0x0000020000000000,   /* value is represented divided by 1.0E+6 - default is equivalent to %.3f M<cur> */
    FMTF_Billion    = 0x0000040000000000,   /* value is represented divided by 1.0E+9 - default is equivalent to %.3f B<cur> */
    FMTF_End
};
const char* decode_FMTFlag(FMT_Flags_type pFlag);

enum uspf_check:uint8_t
{
    uspf_nothing        =0,
    uspf_FormatAlone    = 1,  /* format string is parsed */
    uspf_FormatAndVars  = 3   /* format string is parsed, vars are parsed, buffer size is computed */
};

/**
 * @brief utfCheckFormat this routine is used to check a given format content.
 * It parses format and prints various flags and given fields characteristics.
 * @param pFormat
 */
void    utfCheckFormat(const utfFmt*pFormat,...);
/**
 * @brief utfCheckFullFormat checks the full format string, parses format and vars and returns the size of each field as well as the size of the whole buffer.
 *  Any size is expressed in character units.
 * @param pFormat
 * @return
 */
size_t  utfCheckFullFormat(const utfFmt*pFormat,...);
//============== Main processing routine ==============================================

class zicuConverter;
size_t
utfSubDoprintfvoid(OutEncoding_type pOutOET, /* Output buffer utf type : only Unicode is allowed there */
                   void *pBuffer,           /* Buffer to be filled in */
                   size_t pMaxlen,          /* maximum buffer offset in character units */
                   const utfFmt *pFormat,   /* an utfFmt string containing formatting instructions */
                   va_list args,
                   zicuConverter** pConverter,
                   uspf_check pCheckFormat=uspf_nothing);



//========================Single character Unicode conversion ========================

UST_Status_type
singleUtf8toUtf16 (utf16_t *pOutChar, size_t* pOutCount,
                   utf8_t*pInChar, size_t pInSize,
                   size_t* pCurrLen, size_t pMaxLength);

UST_Status_type
singleUtf8toUtf32 (utf32_t* pOutChar,size_t* pOutSize,
                   utf8_t*pInChar,size_t pInSize,
                   size_t* pCurrLen,size_t pMaxLength);

UST_Status_type
singleUtf16toUtf8 (utf8_t *pOutChar, size_t* pOutCount,
                   utf16_t*pInChar, size_t pInSize,
                   size_t* pCurrLen, size_t pMaxLength);

UST_Status_type
singleUtf16toUtf32 (utf32_t* pOutChar,size_t* pOutSize,
                    utf16_t*pInChar,size_t pInSize,
                    size_t* pCurrLen,size_t pMaxLength);

UST_Status_type
singleUtf32toUtf8 (utf8_t* pOutChar,size_t* pOutCount,
                   utf32_t*pInChar,size_t pInSize,
                   size_t* pCurrLen,size_t pMaxLength);

UST_Status_type
singleUtf32toUtf16 (utf16_t* pOutChar, size_t* pOutCount,
                    utf32_t pInChar, size_t pInSize,
                    size_t* pCurrLen, size_t pMaxLength);


#include <ztoolset/zicuconverter.h>


UST_Status_type
utfFormatUtfStringChunk(OutEncoding_type pOutOET,
                        void* pBuffer,
                        size_t* pCurrlen,
                        size_t pMaxlen,
                        OutEncoding_type pInOET,
                        void* pInString,
                        size_t pInUnit,
                        zicuConverter** pConverter);



/**
 * @brief utfFormatInteger formats an integer of type _ITp to pBuffer, an _Utf string
 * and returns actual size of formatted integer as it appears in output buffer.
 * Size is expressed in character units and not in bytes.
 *
 *
 * template parameters :
 * _Utf output character format
 * _ITp type of integer to process
 * @param pBuffer       Output buffer as an _Utf string.
 * @param pCurrlen      Buffer current offset in character units
 * @param pMaxlen       Buffer maximum offset available in character units.
 * @param pFieldValue   Field value as a_ITp integer.
 * @param pFieldMin     Minimum digits to print.
 * @param pFieldMax     Overall maximum units slots to be printed. Ignored if negative or zero.
 * @param pFmtFlags     Formatting option expressed as an FMT_Flag_type containing enum FMT_Flags values.
 * @return
 */
template <class _Utf, class _ITp>
size_t
utfFormatInteger(_Utf *pBuffer, size_t * pCurrlen, size_t pMaxlen,
                  _ITp pFieldValue, int pFieldMin, int pFieldMax, FMT_Flags_type pFmtFlags)
{
    _Utf            wSignvalue = 0 ;
    ULONG_LONG      wUvalue;
    size_t          wCAlloc   = pFieldMin + 5 ;
    _Utf *          wConvert  = nullptr;
    int             wPlace = 0;
    int             wSpadlen = 0;        /* amount to space pad */
    int             wZpadlen = 0;        /* amount to zero pad */

    size_t          wLocalSize=*pCurrlen;

    wConvert = (_Utf*) calloc(wCAlloc,sizeof(_Utf));

    ULONG_LONG wBase = 10;

    if (pFmtFlags & FMTF_Hexa)
        {
        wBase=16;
        }
    else
    if (pFmtFlags & FMTF_Octal)
        {
        wBase=8;
        }

    if (pFieldMax < 0)
        pFieldMax = 0;

    wUvalue = pFieldValue;

    /* address field sign   */



    if (!(pFmtFlags & FMTF_Unsigned))
        {
        if (pFieldValue < 0)
            {
            wSignvalue = (_Utf) __MINUS__;
            wUvalue = -pFieldValue;
            }
        else
            if (pFmtFlags & FMTF_Plus)   /* Force a sign */
                    wSignvalue = (_Utf)__PLUS__;
        else
            if (pFmtFlags & FMTF_SignSpace) /*If both the ' ' and '+' flags appear, the ' ' flag will be ignored */
                    wSignvalue = (_Utf)__SPACE__;
        }//if (!(pFMTFlags & FMTF_Unsigned))

    do {
        wConvert[wPlace++] =
                ((pFmtFlags & FMTF_Uppercase)? cst_HexaNumbers_Upper<_Utf> : cst_HexaNumbers_Lower<_Utf>) [wUvalue %  wBase];
        wUvalue = (wUvalue /  wBase);
        if (wPlace == (int)wCAlloc)
            {
            wCAlloc += 5;
            wConvert = zrealloc(wConvert,wCAlloc*sizeof(_Utf));
            }

        } while (wUvalue);

/*    if (pFmtFlags & FMTF_Hexa)
        {
        if ((wPlace + 3) >= (int)wCAlloc )
            {
            wCAlloc += 4;
            wConvert = zrealloc(wConvert,wCAlloc*sizeof(_Utf));
            }
        if (pFmtFlags & FMTF_Uppercase)
                wConvert[wPlace++] = (_Utf) __X_UPPER__;
            else
                wConvert[wPlace++] = (_Utf) __X_LOW__;
        wConvert[wPlace++] = (_Utf) __ZERO__;
        }//if (pFmtFlags & FMTF_Hexa)

    else
      if (pFmtFlags & FMTF_Octal)
        {
        if ((wPlace + 3) >= (int)wCAlloc )
            {
            wCAlloc += 4;
            wConvert = (_Utf*) realloc(wConvert,wCAlloc*sizeof(_Utf));
            }
        if (pFmtFlags & FMTF_Uppercase)
                wConvert[wPlace++] = (_Utf) __X_UPPER__;
            else
                wConvert[wPlace++] = (_Utf) __X_LOW__;
        wConvert[wPlace++] = (_Utf) __O_LOW__;
        }//if (pFmtFlags & FMTF_Octal)
*/
    wConvert[wPlace] = (_Utf)__ENDOFSTRING__;

    wZpadlen = pFieldMax - wPlace;
    wSpadlen = pFieldMin - MAX(pFieldMax, wPlace) - (wSignvalue ? 1 : 0);
    if (wZpadlen < 0)
                wZpadlen = 0;
    if (wSpadlen < 0)
                wSpadlen = 0;

    if (pFmtFlags & FMTF_ZeroPad)
        {
        wZpadlen = MAX(wZpadlen, wSpadlen);
        wSpadlen = 0;
        }
    if (pFmtFlags & FMTF_Left)
                wSpadlen = -wSpadlen;     /* Left Justifty */

    while (wSpadlen > 0)
        {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        --wSpadlen;
        }

    /*  Sign : if needed */

    if (wSignvalue)
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);

    /*    Zeroes   */

    if (wZpadlen > 0)
        {
        while (wZpadlen > 0)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
            --wZpadlen;
            }
        }//if (wZpadlen > 0)

    /*   Digits  */

    while (wPlace > 0)
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wConvert[--wPlace]); /* decrement first */

    /* Left Justified spaces */

    while (wSpadlen < 0)
        {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        ++wSpadlen;
        }

    zfree(wConvert);

    return (*pCurrlen-wLocalSize); // returns current full size of printed integer
}// utfFormatInteger

template <class _ITp>
#ifndef __COMPACT_CODE_
inline
#endif
size_t
utfMainFormatInteger (OutEncoding_type pOutOET, void *pBuffer, size_t * pCurrlen, size_t pMaxlen,
                  _ITp pFieldValue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFMTFlags)
{
    char * wBufferChar=nullptr;
    utf8_t*  wBufferUtf8=nullptr;   /* Buffer is utf8 */
    utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */
    switch (pOutOET)
    {
    case OET_Char:
        wBufferChar=static_cast<char*>(pBuffer);
        return utfFormatInteger<char,_ITp>(wBufferChar,pCurrlen,pMaxlen,
                                                   pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF8:
        wBufferUtf8=static_cast<utf8_t*>(pBuffer);
        return utfFormatInteger<utf8_t,_ITp>(wBufferUtf8,pCurrlen,pMaxlen,
                                                     pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF16:
        wBufferUtf16=static_cast<utf16_t*>(pBuffer);
        return utfFormatInteger<utf16_t,_ITp>(wBufferUtf16,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF32:
        wBufferUtf32=static_cast<utf32_t*>(pBuffer);
        return utfFormatInteger<utf32_t,_ITp>(wBufferUtf32,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    }//switch (pOutOET)
    return 0;// to avoid a boring warning
}//utfMainFormatInteger






/**
 * @brief utfSPFConvertInt Allocates memory and converts an _ITp integer to its printable representation according pFmtFlags.<br>
 *
 *  Numbers are returned in reverse order : from least significant to most significant digit.<br>
 *  This routine makes no padding.<br>
 *  template parameters :<br>
 * _Utf : type of character unit pOutConvert is made of<br>
 * _Itf : type of integer to process (int, long,...) - Only integer types are allowed here.
 *  Floatting point data will give impredictable results.
 *
 * @remark output buffer must be freed by calling routine. @see _free()
 *
 * @param pValue        _ITp integer value to process
 * @param pOutconvert   a pointer to a null _Utf string. Will be allocated and returned with appropriate size :
 *
 * @param pAllocated    a pointer to a size_t : will receive the number of _Utf units allocated to pOutconvert
 *                      if pAllocated is nullptr then allocated units are not returned.
 * This number (allocated digits) is either
 * - equal to pInitDigits + 5
 * - if pOutDigits is greater than pInitDigits + 5, then it is at least enough to store pOutDigits (but may be greater).
 *
 * @param pOutDigits    Number of effective digits written in pOutconvert string
 * @param pInitDigits   Number of initial digits to allocate
 * @param pFmtFlags     a FMT_Flags_type containing FMT_Flags enum values. Retained options are<br>
 * FMTF_Hexa : hexadecimal conversion <br>
 * FMTF_Octal: octal conversion<br>
 * FMTF_Uppercase : hexadecimal digits are uppercase when they are letters.<br>
 *
 * If none of FMTF_Hexa or FMTF_Octal is used, then conversion is made on a decimal basis.
 * If both FMTF_Hexa and FMTF_Octal are set, then FMTF_Hexa prevails and FMTF_Octal is disguarded.
 * @return  Number of effective digits in output buffer.
 */
template <class _Utf,class _ITp>
size_t
utfSPFConvertInt(_ITp pValue,
                 _Utf **pOutconvert,
                 size_t* pAllocated,
                 int*   pOutDigits,
                 int    pInitDigits,             /* initial digits allocation (target)- may be bypassed */
                 FMT_Flags_type pFmtFlags)    /* only 2 flag values are used if set : FMTF_Hexa FMTF_Uppercase   */
{

    size_t          wCAlloc   = pInitDigits + 5 ;
    _Utf *          wConvert  = nullptr;

    int             wDigit;
    ZBool           wAtLeastOne=false;
//    *pAllocated = pInitDigits + 5 ;
    _ASSERT_ (!pOutconvert," _Utf String pointer is null",1);
    zfree(*pOutconvert);

    unsigned int wBase=10;
    if (pFmtFlags & FMTF_Hexa)
                    wBase=16;
        else
        if (pFmtFlags & FMTF_Octal)
                        wBase=8;

    wConvert = (_Utf*) calloc(wCAlloc,sizeof(_Utf));
    _ASSERT_ (!wConvert," cannot allocate memory",1);
//    int wMaxDigits = pMaxDigits > __MAX_INTEGER_DIGITS__?__MAX_INTEGER_DIGITS__:pMaxDigits;


    *pOutDigits=0;
    do {
        wDigit =(int)(pValue % wBase);
        wConvert[(*pOutDigits)++] =
                    ((pFmtFlags & FMTF_Uppercase) ? cst_HexaNumbers_Upper<_Utf> : cst_HexaNumbers_Lower<_Utf>)[wDigit];
        pValue = (pValue / wBase);
        wAtLeastOne=true;
        if ((size_t)(*pOutDigits) == wCAlloc)
            {
            wCAlloc += 5;
            wConvert = zrealloc(wConvert,wCAlloc*sizeof(_Utf));
 //           _ASSERT_ (!wConvert," cannot allocate memory",1);
            }
        } while (pValue );

    if ((size_t)(*pOutDigits) == wCAlloc)
        {
        wCAlloc += 2;
        wConvert = zrealloc(wConvert,wCAlloc*sizeof(_Utf));
//        _ASSERT_ (!wConvert," cannot allocate memory",1);
        }
    wConvert[(*pOutDigits)] = __ENDOFSTRING__;
    if (pAllocated)
            *pAllocated = wCAlloc;
    *pOutconvert=wConvert;
    return  *pOutDigits;
}// utfConvertLong

#include <cmath>

template <class _Utf>
/**
 * @brief utfConvertLDouble Internally 2 unsigned long long are used because float base routine like modf use double and not long double data.
 *  This causes a lack of precision and a wrong result.
 * @param pValue
 * @param pOutconvert
 * @param pAllocated
 * @param pOutDigits
 * @param pInitDigits
 * @param pFmtFlags
 * @return
 */
size_t
utfConvertLDouble (LONG_DOUBLE pValue,      /* value to convert as a long double */
                   _Utf **pOutconvert,      /* converted result in reverse order */
                   size_t* pAllocated,      /* number of allocated digits within pOutconvert */
                   int*   pOutDigits,       /* number of output digits */
                   int    pInitDigits,      /* initial digits allocation (target)- could be bypassed according number of digits to write */
                   FMT_Flags_type pFmtFlags)/* only 2 flag values are used if set : FMTF_Hexa FMTF_Uppercase   */
{

    size_t          wCAlloc   = pInitDigits + 5 ;
    _Utf *          wConvert  = nullptr;

    int             wDigit;
    _ASSERT_ (!pOutconvert," _Utf String pointer is null",1);
    zfree(*pOutconvert);
    int         wBase=10;
    LONG_DOUBLE wFBase=10.0;
    LONG_DOUBLE wFValue=pValue;

    ULONG_LONG  wULValue=0,wULValue10=0;
    LONG_DOUBLE wMan,wMan3;


    if (pFmtFlags & FMTF_Hexa)
                    {
                    wBase=16;
                    wFBase=16.0;
                    }
        else if (pFmtFlags & FMTF_Octal)
                {
                wBase=8;
                wFBase=8.0;
                }


    if (pValue > INT64_MAX)
        {
        wULValue10 = (ULONG_LONG)(pValue /(ULONG_LONG)INT64_MAX);
        wMan = (LONG_DOUBLE)wULValue10;
        wMan = wMan *(LONG_DOUBLE) INT64_MAX;
        wMan3 = pValue - wMan;
        wULValue =(ULONG_LONG)(wMan3);
        }
    else
    wULValue=(ULONG_LONG)pValue;

    wConvert = (_Utf*) calloc(wCAlloc,sizeof(_Utf));
    _ASSERT_ (!wConvert," cannot allocate memory",1);
//    int wMaxDigits = pMaxDigits > __MAX_INTEGER_DIGITS__?__MAX_INTEGER_DIGITS__:pMaxDigits;

    if (wFValue<0)  /* fabs : only positive values are processed here */
        wFValue=-wFValue;

    *pOutDigits=0;
    do {
        wDigit=(int)(wULValue % wBase);
        wConvert[(*pOutDigits)++] =
                    ((pFmtFlags & FMTF_Uppercase) ? cst_HexaNumbers_Upper<_Utf> : cst_HexaNumbers_Lower<_Utf>)[wDigit];
        wULValue = wULValue / wBase;
//        wAtLeastOne=true;
        if ((size_t)(*pOutDigits) == wCAlloc)
            {
            wCAlloc += 5;
            wConvert = zrealloc(wConvert,wCAlloc*sizeof(_Utf));
//            _ASSERT_ (!wConvert," cannot allocate memory",1);
            }
        } while (wULValue > 0 );

    while (wULValue10 > 0 ) {
        wDigit=(int)(wULValue10 % wBase);
        wConvert[(*pOutDigits)++] =
                    ((pFmtFlags & FMTF_Uppercase) ? cst_HexaNumbers_Upper<_Utf> : cst_HexaNumbers_Lower<_Utf>)[wDigit];
        wULValue10 = wULValue10 / wBase;
        if ((size_t)(*pOutDigits) == wCAlloc)
            {
            wCAlloc += 5;
            wConvert = zrealloc(wConvert,wCAlloc*sizeof(_Utf));
//            _ASSERT_ (!wConvert," cannot allocate memory",1);
            }
        } ;
    if ((size_t)(*pOutDigits) == wCAlloc)
        {
        wCAlloc += 2;
        wConvert = zrealloc(wConvert,wCAlloc*sizeof(_Utf));
//        _ASSERT_ (!wConvert," cannot allocate memory",1);
        }
    wConvert[(*pOutDigits)] = __ENDOFSTRING__;
    if (pAllocated)
            *pAllocated = wCAlloc;
    *pOutconvert=wConvert;
    return  *pOutDigits;
}// utfConvertLDouble


#include <math.h> /* isfinite, isnan */
template <class _Utf, class _FTp>
/**
 * @brief utfSignalNan  used to signal a NAN or INFINITE floating point value in place of float value
 * @param pBuffer
 * @param pCurrlen
 * @param pMaxlen
 * @param pFieldMin
 * @param pFieldPrecision
 * @param pSignal           either cst_NAN or cst_nan or cst_INFINITE or cst_infinite
 * @param pFmtFlags
 * @return
 */
size_t
utfSignalNan(_FTp pFValue,_Utf *pBuffer, size_t * pCurrlen, size_t pMaxlen, int pFieldMin,  _Utf* pSignal,FMT_Flags_type pFmtFlags)
{
_Utf wUtfSign =0;
size_t wLocalSize=*pCurrlen;

    if (pFValue < 0)    /* infinite and nan floats are signed - sign must be indicated according format */
        {
        wUtfSign=(_Utf)__MINUS__;
        }
    else
    {
    if (pFmtFlags & FMTF_Plus)
        {
        wUtfSign=(_Utf)__PLUS__;
        }
    }

size_t wL= utfStrlen<_Utf>(pSignal);
    int wPad = pFieldMin - wL - (wUtfSign?1:0);
    if (wPad<0)
            wPad=0;
    if (pFmtFlags & FMTF_Left)
                    wPad = - wPad;


    while(wPad>0)
        {
        utfSPFOutChar<_Utf>(pBuffer,pCurrlen,pMaxlen,(_Utf)__SPACE__);
        wPad--;
        }
    if (wUtfSign)
        {
        utfSPFOutChar<_Utf>(pBuffer,pCurrlen,pMaxlen,wUtfSign);
        }
    size_t wi = 0;
    if (pFmtFlags & FMTF_Uppercase)
            {
            while(cst_NAN<_Utf>[wi])
                    utfSPFOutChar<_Utf>(pBuffer,pCurrlen,pMaxlen,pSignal[wi++]);
            return (*pCurrlen-wLocalSize);
            }
        else
            {
            while(cst_nan<_Utf>[wi])
                    utfSPFOutChar<_Utf>(pBuffer,pCurrlen,pMaxlen,pSignal[wi++]);
            }
    while(wPad<0)
        {
        utfSPFOutChar<_Utf>(pBuffer,pCurrlen,pMaxlen,(_Utf)__SPACE__);
        wPad++;
        }
    return (*pCurrlen-wLocalSize);
}//utfSignalNan

template <class _FTp>
/**
 * @brief FPRound  Dependant from following preprocessor symbols
 *
 * __FLOAT_F_RND_GT__
 * __FLOAT_E_RND_GT__
 * __FLOAT_A_RND_GT__
 *
 * @param pFValue
 * @param pCoef
 * @param pGreater
 * @return
 */
inline
_FTp FPRound(_FTp pFValue,double pCoef,ZBool pGreater=false)
{

_FTp wIntPart,wFractPart;

    wFractPart=modf(pFValue,&wIntPart);
    if (pGreater)
        {
        if (pFValue>0)
                if (wFractPart > pCoef )
                    return floor(pFValue + pCoef) ;
                else
                    return floor(pFValue - pCoef) ;
            else
                return wIntPart;
        }
    else if (pFValue>0)
                    return floor(pFValue + pCoef) ;
            else
                    return floor(pFValue - pCoef);
}//FPRound


template <class _Utf>
int
LDoubleToUtfDigits_F(LONG_DOUBLE pValue,
                   _Utf** pDigitStr,
                   int* pFractionalNb,
                   int* pIntegralNb,
                   size_t* pAllocatedSlots,
                   int pFieldPrecision,
                   FMT_Flags_type pFmtFlags)
{

 int wDigitsNb=0;
 LONG_DOUBLE wLDBase=10.0;
 size_t wAllocatedSlots;
 _Utf* wIntegralDigits=nullptr;
 _Utf* wFractionalDigits=nullptr, *wUtfInPtr,*wUtfOutPtr;
 double wRndCoef = 0.5;

    _ASSERT_ (!pDigitStr," _Utf String pointer is null",1);
    _Utf* wDigitStr;
 /* Because integrity of long double and double fractional part is not garanteed in case of huge number when rounding,
   (in fact, fractional part becomes false) we must dissociate fractional part and integral part of the number and
   process both separately.
  */
    if (pFmtFlags & FMTF_Hexa)
                    {
                    wLDBase=16.0;
                    wRndCoef= 0.8;
                    }
        else if (pFmtFlags & FMTF_Octal)
                    {
                    wLDBase=8.0;
                    wRndCoef= 0.4;
                    }

    if (pValue<0)           /* fabs */
            pValue=-pValue;

    LONG_DOUBLE wLDPrecision=pFieldPrecision;
    LONG_DOUBLE wQuot= powl(wLDBase,wLDPrecision);
    LONG_LONG wISignificant=pValue;
    LONG_DOUBLE wIFSignificant = wISignificant;                     /* integral part    */
    LONG_DOUBLE wFFSignificant = pValue - wIFSignificant;    /* fractional part  */
    int wIntegralDigitsNb=0,wFractionalDigitsNb=0;
    _Utf* wFractPtr;
    /* put fractional part as integral and rounding */

    if (wFFSignificant>0.0)
        {
        wFFSignificant *= wQuot;
        wFFSignificant = FPRound<LONG_DOUBLE> (wFFSignificant,wRndCoef,GFloat_F_Rnd_GT);
        if (wFFSignificant >= wQuot) /* if decimal overflow on fractional part after rounding */
                            {
                            wIFSignificant ++;      /* add 1 unit to integral part */
                            wFFSignificant -= wQuot;/* subtract overflow from fractional part */
                            }
        /* convert separately integral part and fractional part */
        if(pFieldPrecision>0)
                {
                utfConvertLDouble<_Utf>(wFFSignificant,
                                        &wFractionalDigits,
                                        &wAllocatedSlots,
                                        &wFractionalDigitsNb,
                                        __INIT_INTEGER_DIGITS__,
                                        pFmtFlags);
                }
        }
        else
        {
        wFractionalDigitsNb=0;
        if(pFieldPrecision>0)
                wFractPtr=wFractionalDigits=(_Utf*)calloc(pFieldPrecision+1,sizeof(_Utf));
        }

    /* if FMTF_NonSignif flag is not set : pad until precision */
    if(pFieldPrecision>0)
        {
        if (!(pFmtFlags & FMTF_NonSignif))
            {
            wFractPtr=&wFractionalDigits[wFractionalDigitsNb];
            while (wFractionalDigitsNb < pFieldPrecision)
                {
                *wFractPtr=(_Utf)__ZERO__;
                wFractPtr++;
                wFractionalDigitsNb++;
                }
            *wFractPtr=(_Utf)__ENDOFSTRING__;
            }//if (!(pFmtFlags & FMTF_NonSignif))
        }// if(pFieldPrecision>0)

    if (wIFSignificant>0)
            utfConvertLDouble<_Utf>(wIFSignificant,
                                    &wIntegralDigits,
                                    &wAllocatedSlots,
                                    &wIntegralDigitsNb,
                                    __INIT_INTEGER_DIGITS__,
                                    pFmtFlags);
        else
        wIntegralDigitsNb=1; /* unique zero as integral digit */

    /* then concatenate both*/
    wDigitsNb= wFractionalDigitsNb+wIntegralDigitsNb;
    *pAllocatedSlots = wDigitsNb+5;

    wDigitStr = (_Utf*)calloc(*pAllocatedSlots,sizeof(_Utf));
    _ASSERT_ (!wDigitStr," cannot allocate memory",1);

    wUtfOutPtr=wDigitStr;
    if (wFractionalDigits)
        {
        wUtfInPtr=wFractionalDigits;
        while (*wUtfInPtr)
                {
                *wUtfOutPtr = *wUtfInPtr;
                wUtfOutPtr++;
                wUtfInPtr++;
                }
        }//If (wFractionalDigits)
    if (wIFSignificant>0)
        {
        wUtfInPtr=wIntegralDigits;
            while (*wUtfInPtr)
                {
                *wUtfOutPtr = *wUtfInPtr;
                wUtfOutPtr++;
                wUtfInPtr++;
                }
        }
        else
        *(wUtfOutPtr++)=(_Utf)__ZERO__;

    *wUtfOutPtr=(_Utf)__ENDOFSTRING__;
    zfree(wFractionalDigits);
    zfree(wIntegralDigits);

    /* done */
    *pFractionalNb=wFractionalDigitsNb;
    *pIntegralNb = wIntegralDigitsNb;
    *pDigitStr = wDigitStr;
    return  wDigitsNb;
}//LDoubleToUtfDigits_F

int getExponent(LONG_DOUBLE pLDValue,LONG_DOUBLE pLDBase);


/**
 *  Compute precision as significant digits.
 *  Trim trailing fractional zeroes.
 *
 */

template <class _Utf>
int
LDoubleToUtfDigits_G(LONG_DOUBLE pValue,
                     _Utf** pDigitStr,
                     int* pFractionalNb,
                     int* pIntegralNb,
                     size_t* pAllocatedSlots,
                     int pFieldPrecision,
                     FMT_Flags_type pFmtFlags)
{

 int wDigitsNb=0;
 LONG_DOUBLE wLDBase=10.0;
 size_t wAllocatedSlots;
 _Utf* wIntegralDigits=nullptr;
 _Utf* wFractionalDigits=nullptr, *wUtfInPtr,*wUtfOutPtr;
 double wRndCoef = 0.5;

    _ASSERT_(!(pFmtFlags & FMTF_GFormat),"invalid format requested. Only <g> float format is allowed.",1);
    _ASSERT_ (!pDigitStr," _Utf String pointer is null",1);
    _Utf* wDigitStr=nullptr;
 /* Because integrity of long double and double fractional part is not garanteed in case of huge number when rounding,
   (in fact, fractional part becomes false) we must dissociate fractional part and integral part of the number and
   process both separately.
  */
    if (pFmtFlags & FMTF_Hexa)
                    {
                    wLDBase=16.0;
                    wRndCoef= 0.8;
                    }
        else if (pFmtFlags & FMTF_Octal)
                    {
                    wLDBase=8.0;
                    wRndCoef= 0.4;
                    }

    int wExponent = getExponent(pValue,wLDBase);

    /* compute significant digits number for fractional part SDNF

      if exponent >= 0
      SDNF = ( requested significant digits - exponent ) > 0 ? requested significant digits - exponent : 0

      Fract     pFieldPrecision wExponent   Value           Rule            Out
      Precision                                          before roundg
        3           6               3       996.12786       996.127         996.128
        0           0               3       996.12786       996
        0           1               2       99.612786        99             100
        3           5               2       99.612786       99.612           99.613

        1           2               0       1.235645        1.2               1.2
        1           2               0       9.9612786       9.9              10

        8           6               -3      0.0099612786    0.00996127      0.00996128  precision 8 rounded
        3           0               -3      0.0099612786    0.009           0.01        precision 3 rounded trim trailing zeroes
        2           1               -2      0.099612786     0.09            0.1         precision 2 rounded trim trailing zeroes
        1           1               -1      0.99612786      0.9             1

        0.0099612786      exponent is -3 -> wPrecision is 6 -(-3) - 1 = 8
        0.0099612786 * 10^8
        996127.86  round
        996128     add (-exponent) 3 leading zeroes. decimal point is after 1 integral
        0.00996128

        0.099612786     exponent is -2 -> wPrecision is 1 -(-2) - 1 = 2
        0.099612786 * 10^2
        9.9612786
       10.461276        after rounding  >= 10^wPrecision decimal overflow
       10               add abs(exponent)-1 overflow  = (2-1) leading zeroes. decimal point is after 1 integral
      0.010
      0.1

        0.01
        0.0099612786      coef= exponent is -3 -> wPrecision is 2 -(-3) - 1 = 4
        0.0099612786 * 10^coef
        99.612786  round
       100.1128    >= 10^wPrecision Yes decimal overflow
       100                add abs(exponent)-1 overflow (3-1) leading zeroes. decimal point is after 1 integral
       0001
       0.01

        0.0099612786      exponent is -3 -> wPrecision is 6 -(-3) - 1 = 8
        1.0099612786 * 10^8
        100996127.86  round
        100996128     add (-exponent) 3 leading zeroes. decimal point is after 1 integral
        1.00996128

        0.099612786     exponent is -2 -> wPrecision is 1 -(-2) - 1 -> rise = 2
        1.099612786 * 10^2
      109.9612786
      110.461276        after rounding  >= 10^wPrecision decimal overflow
        1.10        / 10^2
        0.1         drop first one and trim trailing zeroes

        0.0099612786      rise= exponent is -3 -> wPrecision is 2 -(-3) - 1 = 4
        1.0099612786 * 10^rise 10^4
    10099.612786  round
    10100.1128    >= 2*rise No
        1.10     / 10^rise (10^4)
        0.1         drop first one and trim trailing zeroes

        requested significants = 1 exp = -1

        0.99612786      0.9             1
        0.99612786      rise= exponent is -1 -> wPrecision is 1 -(-1) - 1 = 1
        1.99612786 * 10^rise 10^1
       19.9612786  round
       20.4612786
       20.          >= 2*(10^rise) yes : impacts on integer part
       integral part + 1 fractional - 10^rise
       integral +1 fractional 1.0
       integral +1 fractional 0.0  drop first one trim trailing zero
       integral 1 fractional <>

    10100.1128    >= 2*rise No
        1.10     / 10^rise (10^4)
        0.1         drop first one and trim trailing zeroes
    */
    LONG_DOUBLE wLDFractPrecision=0-0;

    if (pFieldPrecision < 1)
                pFieldPrecision = 1;
    if (wExponent >= 0) /* if there is an integer part, subtact from significant digits to get from fractional part*/
        {
        wLDFractPrecision = (LONG_DOUBLE)(pFieldPrecision-wExponent - 1);
        }
    else
        wLDFractPrecision=(LONG_DOUBLE)(pFieldPrecision -wExponent - 1 );

    if (wLDFractPrecision<0)
            wLDFractPrecision=0;

    LONG_LONG wISignificant=(LONG_LONG)pValue;
    LONG_DOUBLE wIFSignificant = (LONG_DOUBLE)wISignificant;                /* integral part    */

    LONG_DOUBLE wFFSignificant = ((LONG_DOUBLE)pValue) - wIFSignificant;    /* fractional part  */

    int wIntegralDigitsNb=0,wFractionalDigitsNb=0;

    /* put fractional part as integral and rounding */
    if (wFFSignificant>0.0) /* if there is a fractional part   */
        {
        /*even if fractional precision is 0 we must evaluate rounding from fractional part */
        LONG_DOUBLE wRise= powl(wLDBase,wLDFractPrecision);
        LONG_DOUBLE wOvflw=wRise*2.0;     /* to test if base overfow on fractional */


        wFFSignificant += 1.0; /* for keeping all leading zeroes */
        wFFSignificant *= wRise;
        wFFSignificant = FPRound<LONG_DOUBLE> (wFFSignificant,wRndCoef,GFloat_G_Rnd_GT);

        if (wFFSignificant >= wOvflw) /* if overflow impacts on integral part */
                    {
                    wIFSignificant ++;      /* add 1 unit to integral part */
                    wFFSignificant -= wRise;/* subtract overflow from fractional part */

                    /*If decimal overflow : reevaluate significant digits number after rounding */
                    }//if (wFFSignificant >= wRise)
        if (wLDFractPrecision > 0.0) /* convert only if fractional precision is > 0*/
                utfConvertLDouble<_Utf>(wFFSignificant,
                                        &wFractionalDigits,
                                        &wAllocatedSlots,
                                        &wFractionalDigitsNb,
                                        __INIT_INTEGER_DIGITS__,
                                        pFmtFlags);
        }//if (wFFSignificant>0.0)
    else
        {
        wFractionalDigitsNb=0;
        }

    /* trim trailing zeroes on fractional part */

    if ((pFmtFlags & FMTF_GFormat)||(pFmtFlags & FMTF_NonSignif)) /* <g> format or remove trailing zeroes flag set ?*/
        {
        /* remove trailing zeroes for all fractional digits up until 1rst integer number */
        int wi=0,wj=0;
        while ((wi < wFractionalDigitsNb) &&(wFractionalDigits[wi]==(_Utf)__ZERO__))
                    {
                    wi++;
                    }
        pFieldPrecision -= wi;
        if (wi)
            {
            while (wi<wFractionalDigitsNb)
                {
                wFractionalDigits[wj++]= wFractionalDigits[wi++];
                }
            wFractionalDigitsNb=wj;
            wFractionalDigits[wj]=__ENDOFSTRING__;
            }
        }// if ((pFmtFlags & FMTF_GFormat)||(pFmtFlags & FMTF_NonSignif))


    if (wIFSignificant > 0.0 ) /* if there is an integral part : print anyway */
            utfConvertLDouble<_Utf>(wIFSignificant,
                                    &wIntegralDigits,
                                    &wAllocatedSlots,
                                    &wIntegralDigitsNb,
                                    __INIT_INTEGER_DIGITS__,
                                    pFmtFlags);
        else/* value is less than 1 */
        {
           wIntegralDigitsNb=0; /* unique zero as integral digit */
        }


    /* then concatenate both*/
    wDigitsNb= wFractionalDigitsNb+wIntegralDigitsNb;
    *pAllocatedSlots = wDigitsNb+5;

    wDigitStr = (_Utf*)calloc(*pAllocatedSlots,sizeof(_Utf));
    _ASSERT_ (!wDigitStr," cannot allocate memory",1);
     wUtfOutPtr=wDigitStr;
    if (wDigitsNb<1)
        {
        *wUtfOutPtr++=(_Utf)__ZERO__;
        wDigitsNb=wIntegralDigitsNb= 1;
        }
    else
    {
    if (wFractionalDigits>0)
    {
    wUtfInPtr =wFractionalDigits;
    if (*wUtfInPtr)
        {
        while (*wUtfInPtr)
                {
                *wUtfOutPtr = *wUtfInPtr;
                wUtfOutPtr++;
                wUtfInPtr++;
                }
        wUtfOutPtr--;   /* drop top fractional digit */
        wFractionalDigitsNb--;
        wDigitsNb--;
        }
    }//if (wFractionalDigitsNb>0)
    if (wIFSignificant>0)
        {
        wUtfInPtr =wIntegralDigits;
        while (*wUtfInPtr)
                {
                *wUtfOutPtr = *wUtfInPtr;
                wUtfOutPtr++;
                wUtfInPtr++;
                }
        }
        else
        {
        *(wUtfOutPtr++)=(_Utf)__ZERO__;
        wIntegralDigitsNb=1;
        wDigitsNb++;
        }
    } // else if (wDigitsNb<1)
    *wUtfOutPtr=(_Utf)__ENDOFSTRING__;
    zfree(wFractionalDigits);
    zfree(wIntegralDigits);

    /* done */
    *pFractionalNb=wFractionalDigitsNb;
    *pIntegralNb = wIntegralDigitsNb;
    *pDigitStr = wDigitStr;
    return  wDigitsNb;
}//LDoubleToUtfDigits_G


/**
 * @brief utfFormatFPF Formats to pBuffer, an _Utf string, a floating point value according format <f> or <F>
 *
 * @weakgroup FORMATFPF  floating point value according to format <f> or <F>
 * @{
 *
 * @link https://www.gnu.org/software/libc/manual/html_node/Floating_002dPoint-Conversions.html#Floating_002dPoint-Conversions
 * @link https://docs.microsoft.com/en-us/cpp/c-runtime-library/format-specification-syntax-printf-and-wprintf-functions?view=vs-2017
 *
 * FMTF_Hash option flag forces a decimal point (locale) to be put even if number has no fractional digits.
 * Otherwise decimal point is omitted.
 *
 * Rounding :
 *  Rounding is made on the last significant digit using the algorithm : if greater than +0.5 then add + 0.5 and truncated to greater unit.
 *  This is the common way of doing rounding.
 *  @}
 *
 * @param pBuffer         _Utf string as output
 * @param pCurrlen        current offset within pBuffer so far. Updated with current printed size and returned.
 * @param pMaxlen         Overall maximum slots to be consumed within pBuffer. current offset cannot be greater than this.
 * @param pFvalue         floating point to format
 * @param pMin            minimum slots to be consumed
 * @param pFieldPrecision maximum number of slots to be consumed.
 *                        -1 means default : AIX manpage says the default is 0, but Solaris says the default is 6,
 *                        and sprintf on AIX defaults to 6.
 * @param flags           Formatting flags for position and size. @see FMT_Flags
 */
template <class _Utf, class _FTp>
size_t
utfFormatFPF(_Utf *pBuffer, size_t * pCurrlen, size_t pMaxlen,
      _FTp pFvalue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFmtFlags)
{

    _Utf            wSignvalue=0;        /* sign of expression to be printed 0: no sign __MINUS__ or __PLUS__ or __SPACE__ */
    _Utf            wDecimalPoint=0;     /* decimal point : if has to be written, will be set from locale */
    _FTp            wUfvalue;            /* fvalue unsigned value */
    _Utf*           wDigitStr=nullptr;   /* Signigicant digits to be printed */
    _Utf*           wFconvert=nullptr;   /* fraction part to be printed */
    int             wDigitsNb = 0;         /* number of significant digits */
    size_t          wAllocatedSlots = 0;/* total number of allocated digit slots  */
    int             wPadlen = 0;         /* amount to pad */
    int             wZpadlen = 0;

    size_t          wFieldSize, wSlotsSize;
    size_t          wLocalSize=*pCurrlen;
    int wIntegralDigitsNb,wFractionalDigitsNb;

    if (isnan(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
            {
            return  utfSignalNan(pFvalue, pBuffer, pCurrlen, pMaxlen, pFieldMin,(_Utf*)cst_NAN<_Utf>,  pFmtFlags);
            }
        else
            {
           return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_nan<_Utf>,  pFmtFlags);
            }
        }// if (isnan(pFvalue))
    if (!isfinite(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_INFINITE<_Utf>,  pFmtFlags);
                }
            else
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_infinite<_Utf>,  pFmtFlags);
                }
        }

    _ASSERT_((pFmtFlags & FMTF_Octal)||(pFmtFlags & FMTF_Hexa),"Hexadecimal for float must be addressed using utfFormatFPHex. Octal is invalid for float.",1);

    // no hexa no octal allowed

    /*
     * AIX manpage says the default is 0, but Solaris says the default
     * is 6, and sprintf on AIX defaults to 6
     */
    if (!(pFmtFlags & FMTF_Precision)) /* no precision given */
                    pFieldPrecision = 6;
    if (pFieldPrecision < 0)
                    pFieldPrecision = 6; /* or precision flag is set but number is negative ??? */

    wUfvalue = abs_val(pFvalue);

    /* We support as much as slots has available in float.
     * There is a limitation due to float rules : FLT_MAX is the maximum
     */

//    LONG_LONG wIValue;
    LONG_DOUBLE wFSignificant = pFvalue ;
    ZBool wSubOne=false;
    if (wFSignificant < 0)
            {
            wSignvalue = (_Utf)__MINUS__;
            wFSignificant = - wFSignificant;
            }
        else
        if (pFmtFlags & FMTF_Plus)
                wSignvalue = (_Utf)__PLUS__;
        else if (pFmtFlags & FMTF_SignSpace)
                        wSignvalue = (_Utf)__SPACE__; /* FMTF_Space applies only if FMTF_Plus is not set */
    if (wFSignificant < 1.0)
            {
            wFSignificant = wFSignificant + (LONG_DOUBLE)1.0; /* plus one significant digit to keep leading zeroes if any */
            wSubOne=true;
            }

    wDigitsNb = LDoubleToUtfDigits_F(wFSignificant,
                                   &wDigitStr,
                                   &wFractionalDigitsNb,
                                   &wIntegralDigitsNb,
                                   &wAllocatedSlots,
                                   pFieldPrecision,
                                   pFmtFlags);

    if (pFmtFlags&FMTF_NonSignif) /* if requested to remove non significant zeroes */
        {
        int wi=0,wj=0;
        while ((wi<pFieldPrecision)&&(wDigitStr[wi]==(_Utf)__ZERO__))
                    wi++;
        if (wi)
            {
            while (wi<wDigitsNb) /* remove non significant zeroes */
                {
                wDigitStr[wj++]= wDigitStr[wi++];
                }
            wDigitsNb=wj;
            wDigitStr[wj]=__ENDOFSTRING__;
            }
        }//if (FMTF_NonSignif)

    if ((pFmtFlags & FMTF_Hash)||(pFieldPrecision>0)) /* if FMTF_Hash is set or precision > 0, there is a decimal point */
      wDecimalPoint= _zlocale.getLocaleDecimalPoint<_Utf> ();

    if (wSignvalue &&!(pFmtFlags & FMTF_SignFolw)&&!(pFmtFlags & FMTF_SignPrec)) /* if neither option for sign positionning */
                                                        pFmtFlags |= FMTF_SignPrec; /* sign preceeds : this is the default */

    wFieldSize = wDigitsNb + (wDecimalPoint ? 1 : 0 ) + (wSignvalue ? 1 : 0);
    wSlotsSize = wFieldSize > pFieldMin ? wFieldSize:pFieldMin;

    wPadlen = wSlotsSize - wFieldSize;

    /*  Padding */

    /* zero padding only left padding and only if not space padding */
/*    wZpadlen = pFieldPrecision - wFSlots ;

    if (wZpadlen < 0)
                wZpadlen = 0;*/
    if (wPadlen < 0)
                wPadlen = 0;
    if (pFmtFlags & FMTF_Left)
                    wPadlen = -wPadlen;       /* Left Justifty */

    if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))   /* left padding with zeroes ? */
        {
        if (wSignvalue && (pFmtFlags & FMTF_SignPrec))
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);    /* sign preceeds */
            --wPadlen;
            wSignvalue = 0;
            }
        while (wPadlen > 0)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
            --wPadlen;
            }
        }//if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))
        else
        {   /* No zero padding : pad with space if have to */
        while (wPadlen > 0)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
            --wPadlen;
            }
        if (wSignvalue && (pFmtFlags & FMTF_SignPrec))
                {
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);
                wSignvalue=0;
                }
        }  // else

    /*  Digits are stored in reverse order */
    if (wSubOne) /* and skip artificially added 1 unit if value is a decimal value and set integer part to 0 */
        {
        _Utf wFirstDigit =  wDigitStr[--wDigitsNb]; /* get rid of added 1  to first digit could be 1 or 2 depending on possible rounding */
        wFirstDigit--; /* assuming __ZERO__ comes before __ONE__ */
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen,(_Utf)wFirstDigit);
        wSubOne=false;
        wIntegralDigitsNb=0;
        }
    while (wDigitsNb > 0)
        {
        if (wIntegralDigitsNb==0)  /* when integer part is zero : put the decimal point in place */
                    {
                    if (wDecimalPoint)
                        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDecimalPoint);
                    wDecimalPoint = 0;
                    }
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDigitStr[--wDigitsNb]);
        wIntegralDigitsNb--;
        }

    if (wDecimalPoint)
        {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDecimalPoint);
        }

/* No zero right padding : has already been taken into account with precision conversion */

/*    while (wZpadlen > 0) {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
        --wZpadlen;
        }
*/
/* only right space padding wether necessary */
    while (wPadlen < 0) {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        ++wPadlen;
        }
    if (wSignvalue )
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);

    zfree(wDigitStr);
    zfree(wFconvert);
    return  (*pCurrlen-wLocalSize);
}// utfFormatFPF


/**
 * @brief utfFormatFPE Formats a floatting point value (double or long double according _FTp) in a scientific format:<br>
 *  d.ddddde+dd or d.ddddddE+dd<br>
 *
 * * @weakgroup FORMATFPE  floating point value according to format <e> or <E>
 * @{
 *
 * The ‘%e’ conversion prints its argument in exponential notation, producing output of the form [-]d.ddde[+|-]dd.
 *  Again, the number of digits following the decimal point is controlled by the precision.
 * The exponent always contains at least two digits.
 *  The ‘%E’ conversion is similar but the exponent is marked with the letter ‘E’ instead of ‘d'.
 *
 * @ref https://www.gnu.org/software/libc/manual/html_node/Floating_002dPoint-Conversions.html#Floating_002dPoint-Conversions
 *
 * FMTF_Hash option flag ('#' character in format specifier ) has no effects for this format.<br>
 * for all format options see FMT_Flags
 *
 * see FORMATFPG
 * @}
 *
 *  Returns effective size taken by the value once it has been formatted. This size is expressed in characters units (and not in bytes).
 *
 * @param pBuffer
 * @param pCurrlen
 * @param pMaxlen
 * @param pFvalue
 * @param pFieldMin
 * @param pFieldPrecision
 * @param pFmtFlags
 * @return
 */
template <class _Utf, class _FTp>
size_t
utfFormatFPE(_Utf *pBuffer, size_t * pCurrlen, size_t pMaxlen,
      _FTp pFvalue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFmtFlags)
{

    _Utf            wSignvalue=0;        /* sign of expression to be printed 0: no sign __MINUS__ or __PLUS__ or __SPACE__ */
    _Utf            wDecimalPoint=0;     /* decimal point : if has to be written, will be set from locale */
    _FTp            wUfvalue;            /* fvalue unsigned value */

    _Utf*           wFDigitStr=nullptr ; /* fraction part to be printed (in reverse order)*/
    _Utf*           wEDigitStr=nullptr;  /* exponent value to be printed (in reverse order)*/

    int             wFDigitsNb = 0;         /* number of fractional digits */
    int             wESlots = 0;         /* number of exponent digits */

    int             wPadlen = 0;         /* amount to pad */
    int             wZpadlen = 0;

    LONG_DOUBLE     wLDFSignificant;

    size_t          wLocalSize=*pCurrlen;

    size_t wAllocated=0;                /* Number of allocated digits by utfSPFConvertInt when called */

    unsigned int    wBase=10;
    double          wFBase=10.0;
    double          wRndCoef = 0.5; /* 0.5 (10/20) for base 10  0.8 (16/20) for hexa 0.4 (8/20) for octal  */

    _Utf wExpSign = 0;

    int wFirstDigit = 0;
    int wExponent = 0;

    if (isnan(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
            {
            return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin,(_Utf*)cst_NAN<_Utf>,  pFmtFlags);
            }
        else
            {
           return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_nan<_Utf>,  pFmtFlags);
            }
        }// if (isnan(pFvalue))
    if (!isfinite(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_INFINITE<_Utf>,  pFmtFlags);
                }
            else
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_infinite<_Utf>,  pFmtFlags);
                }
        }

    if (pFmtFlags & FMTF_Hexa) /* this should not be the case but it is kept here for experimental purposes */
                        {
                        wBase = 16;
                        wFBase = (double)16.0;
                        wRndCoef = (double )0.8;
                        }

    /* no octal for float : if FMTF_Octal is set : processed as decimal */

    /*
     * AIX manpage says the default is 0, but Solaris says the default
     * is 6, and sprintf on AIX defaults to 6
     */
    if (!(pFmtFlags & FMTF_Precision )) /* no precision given */
                    pFieldPrecision = 6;
    if (pFieldPrecision < 0)            /* or precision flag is set but number is negative ??? */
                    pFieldPrecision = 6;

/* no octal allowed here */

    if (pFmtFlags & FMTF_GFormat)
            {
            pFieldPrecision --; /*<g> format, precision is significant digits : all fractional digits minus one */
            if (pFieldPrecision < 0)
                    pFieldPrecision = 0;
            }
    /* else precision is full number of fractional digits */

    wUfvalue = (pFvalue<0)?-pFvalue:pFvalue;

    if (wUfvalue==0.0)
        {
        wExponent = 0 ;
        wLDFSignificant = 0.0 ;
        if (pFieldPrecision>0)
                wDecimalPoint = _zlocale.getLocaleDecimalPoint<_Utf>();
        }
    else
        {
        wExponent=getExponent(wUfvalue,(LONG_DOUBLE)wFBase);
/*
 * Target : get <precision+1> integer digits and round,
 *  then get <firstunique>.<precision> digits
 *
 * precision= 5
 *
    0.00123456789       exp = -3
    12345.6789          10^8   rise coeff = precision - exp + exp>0?1:0 rise coeff 2 - -1 +0
    123457

    1.23457

    precision=6

    0.00123456789     exp = -3  overflow = abs(exp) > precision ? abs(exp)+1 : precision+1
    1234567.89      * 10^9       precision - exp
    1234568         / 10^6       precision
    1.234568

    precision = 4

    123.4567890123  exp= +2 overflow = abs(exp) > precision ? abs(exp)+1 : precision+1

    12345.6789      * 10^2  rise coeff = precision - exp
    12346           / 10^4   precision
    1.2346

    precision= 7

    123456.7890123  exp= +5  overflow = abs(exp) > precision ? abs(exp)+1 : precision+1
    12345678.90123  * 10^2   rise coeff = precision - exp
    12345679                 rounding  gt or equal  10^8 no
    1.2345679       / 10^7   precision

    precision = 7

    0.1234567890123  exp= -1  overflow = abs(exp) > precision ? abs(exp)+1 : precision+1
    12345678.90123  * 10^8   rise coeff = precision - exp
    12345679                rounding gt 10^8 (Overflow)? no
    1.2345679       / 10^7   precision

    precision =3

    0.9999      exp = -1 overflow = abs(exp) > precision ? abs(exp)+1 : precision+1
    9999.0    * 10^4
    9999.5      rounding    gt 10^4 ? no (10000)
    9.999     / 10^3


    precision =2

    0.9999      exp = -1
    999.9    * 10^3         rise coeff = precision - exp + exp>0?1:0 rise coeff 2 - -1 +0
   1000       rounding  and truncate   gt 10^3 ? yes: overflow add 1 to coeff = precision + 1
     1.000     / 10^3  10^coeff
     1.000          /10


     precision = 3
    12345678.90123  exp= +8  overflow = 10^(precision+1) = 10000
    1234.567890123  * 10^-4   rise coeff = precision - exp + exp>0?1:0 (3 - 8 + 1)
    1235.0679                rounding gt 10^5 (Overflow)? no
    1.2345679       / 10^3   10^precision

  */
    wLDFSignificant = wUfvalue ;                      /* the whole number */
    LONG_DOUBLE wRise,wOverFlow,wLDBase=(LONG_DOUBLE)wFBase,wLDExponent=(LONG_DOUBLE)wExponent;

    /*  rise coeff = precision - (exp + exp>0?1:0 )rise coeff 2 - -1 +0 */
    wLDExponent =(LONG_DOUBLE)(pFieldPrecision- wExponent );

    wRise=powl(wLDBase,wLDExponent);
    /* overflow = 10^(precision+1)   */
    wOverFlow = powl(wLDBase,(LONG_DOUBLE)(pFieldPrecision+1));

    wLDFSignificant *= wRise;

    wLDFSignificant = FPRound<LONG_DOUBLE>(wLDFSignificant,wRndCoef,GFloat_E_Rnd_GT);

    if (wLDFSignificant >= wOverFlow)
        {
        wExponent++;
        wLDFSignificant /= wLDBase;
        }
    }// else if (wUfvalue==0.0)

    if (wExponent < 0)
                {
                wExpSign = (_Utf)__MINUS__;
                wExponent = - wExponent;
                }
            else
                wExpSign = (_Utf)__PLUS__;

    /* get the exponent size */

    utfSPFConvertInt<_Utf,int>(wExponent, &wEDigitStr,&wAllocated,&wESlots,2,pFmtFlags);
//    if (pFieldPrecision==9)
//                    wSignvalue=0;
    if (pFvalue < 0)
            wSignvalue = (_Utf)__MINUS__;
    else if (pFmtFlags & FMTF_Plus)             /* Force sign  */
                    wSignvalue = (_Utf)__PLUS__;
    else if (pFmtFlags & FMTF_SignSpace)        /* space is used if and only if FMTF_Plus is not set (standard says) */
                    wSignvalue = (_Utf)__SPACE__;

    if (wAllocated <=(size_t) (wESlots + 5))
        {
        wAllocated += 5;
        wEDigitStr =zrealloc(wEDigitStr,wAllocated);
//        _ASSERT_(!wEDigitStr,"cannot allocate memory",1);
        }
/* standard says : exponent has at least 2 digits - but : windows uses at least 3 - lets stick to standard */
#ifdef __FLOAT_WINDOWS__
    if (wESlots < 3) /* utfConvertLong allocation is at least (5 + min) digit units : so do not test allocation */
    {
        while (wESlots < 3)
                wEDigitStr[wESlots++]=(_Utf)__ZERO__;
    }
#else
    if (wESlots < 2) /* utfConvertLong allocation is at least (5 + min) digit units : so do not test allocation */
        wEDigitStr[wESlots++]=(_Utf)__ZERO__;

#endif
    wEDigitStr[wESlots++] = wExpSign ; /* exponent is always signed */


    if (pFmtFlags & FMTF_Uppercase)
            wEDigitStr[wESlots++] = (_Utf) __E_UPPER__ ;
        else
            wEDigitStr[wESlots++] = (_Utf) __E_LOW__ ;

    wEDigitStr[wESlots] = (_Utf)__ENDOFSTRING__;

    utfConvertLDouble<_Utf>(wLDFSignificant, &wFDigitStr,&wAllocated, &wFDigitsNb,pFieldPrecision,pFmtFlags);
/*    if (wLDFSignificant==0.0)
            wFDigitStr[wFDigitsNb++]=(_Utf)__ZERO__;*/
    /*  Manage number of fractional part */

//    if (wFSlots > (pFieldPrecision+wExponent))
//            wFSlots = (pFieldPrecision+wExponent); /* Do not take care of 1rst digit */

    if (pFmtFlags & FMTF_NonSignif) /* remove trailing zeroes flag set ?*/
        {
        /* remove trailing zeroes for all fractional digits up until 1rst integer number */
        int wi=0,wj=0;
        while ((wi < wFDigitsNb) && (wi < pFieldPrecision ) &&(wFDigitStr[wi]==(_Utf)__ZERO__))
                    {
                    wi++;
                    }
        pFieldPrecision -= wi;
        if (wi)
            {
            while (wi<wFDigitsNb)
                {
                wFDigitStr[wj++]= wFDigitStr[wi++];
                }
            wFDigitsNb=wj;
            wFDigitStr[wj]=__ENDOFSTRING__;
            }
        }//if (pFmtFlags & FMTF_NonSignif)

/* here we have to decide wether to print decimal point */

    if ((pFieldPrecision>0)||(pFmtFlags & FMTF_Hash))
                wDecimalPoint= _zlocale.getLocaleDecimalPoint<_Utf> ();  /* get local decimal sign only if there is a precision >0 or Hash flag set*/

    if ((wFDigitsNb<2)&&(wLDFSignificant!=0.0))
                                    wDecimalPoint=0;
    pFmtFlags |= FMTF_SignPrec; /* sign is always preceeding for this format despite flag options taken */

/*  string space structure for a float expressed in scientific format

   Exponential decimal notation

    +i.ffffffffffe+xx
    + = sign of float
    i = integer part (1 digit)
    f = fractional part
    + = sign of exponent : always there
    x = exponent value

    fixed slots :
    sign (if has to be printed)
    <e> or <E> letter               (depending if FMTF_UpperCase is set)
    + sign of exponent

    size so far is

    float sign if needed to be printed
    1 slot : 1 digit
    1 slot : dot
    pFieldPrecision-1 : size of fractional part
    size of exponent (wEplace) - includes <e> or <E>

    Float hexadecimal notation

    0.hhhhhhp+dd

    where
    <h> is a fractional hexadecimal digit
    <p> or <P> depending on FMTF_Uppercase
    dd is the exponent is power of 2 (according FP_RADIX) expressed as a signed DECIMAL integer.

    float precision concerns <h> fractional number of digits (i. e. non null - significant digits)
*/
    wPadlen = pFieldMin  - pFieldPrecision - 1 - wESlots - ((wSignvalue) ? 1 : 0) - (wDecimalPoint?1:0) ;
//    if ((pFieldPrecision > 0)&&(wDecimalPoint))  /* Precision is not zero, then take care of decimal point */
//            {
//            if (wPadlen<0)
//                    wPadlen++;
//                else
//                    wPadlen --;
//            }
    if (!(pFmtFlags & FMTF_NonSignif))
            wZpadlen = pFieldPrecision + 1 - wFDigitsNb;
        else
            wZpadlen=0;
    if (wZpadlen < 0)
                wZpadlen = 0;
    if (wPadlen < 0)
                wPadlen = 0;
    if (pFmtFlags & FMTF_Left)
                    wPadlen = -wPadlen;       /* Left Justifty */

    if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))
        {
        if (wSignvalue)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);
            --wPadlen;
            wSignvalue = 0;
            }
            while (wPadlen > 0)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
            --wPadlen;
            }
        }//if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))

    while (wPadlen > 0) {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        --wPadlen;
        }
    if (wSignvalue)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);
            wSignvalue=0;
            }

/* first digit */
    utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wFDigitStr[--wFDigitsNb]);

//    utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, cst_HexaNumbers_Upper<_Utf>[wFirstDigit]);

    if (wDecimalPoint)  /* decimal dot is written only if precision is not zero and there are digits to print after decimal point*/
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDecimalPoint);

/* fractional part */
//    while ((wFDigitsNb > 0)&&(pFieldPrecision))
    while (wFDigitsNb > 0)
        {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wFDigitStr[--wFDigitsNb]);
//        pFieldPrecision--;
        }
/* Zero padding for fractional part */

    while (wZpadlen > 0)
            {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
        --wZpadlen;
            }

/* exponent */
    while (wESlots > 0)
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wEDigitStr[--wESlots]);

/* right space padding */

    while (wPadlen < 0) {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        ++wPadlen;
    }

    zfree(wFDigitStr);
    zfree(wEDigitStr);
    return  (*pCurrlen-wLocalSize);
}// utfFormatFPS

/**
 * @brief utfFormatFPG Formats a floatting point value (double or long double according _FTp) in either a scientific format:<br>
 *  d.ddddde+dd or d.ddddddE+dd<br> or a decimal format ddddd.ddddd
 *
 * @weakgroup FORMATFPG floating point value according to format <g> or <G>
 * @{
 * Format option 'g' or 'G' produces either a scientific format:<br>
 *  d.ddddde+dd or d.ddddddE+dd<br>
 * or a decimal format ddddd.ddddd<br>
 * The ‘%g’ and ‘%G’ conversions print the argument in the style of ‘%e’ or ‘%E’ (respectively)
 * if the exponent would be less than -4 or greater than or equal to the precision;
 * otherwise they use the ‘%f’ style. A precision of 0, is taken as 1.
 * Trailing zeros are removed from the fractional portion of the result and a decimal-point character appears
 * only if it is followed by a digit.
 *
 * @link https://www.gnu.org/software/libc/manual/html_node/Floating_002dPoint-Conversions.html#Floating_002dPoint-Conversions
 *
 * Trailing zeroes must be removed.
 * GNU says :
 * <if the exponent would be less than -4 or greater than or equal to the precision;
 * otherwise they use the ‘%f’ style. A precision of 0, is taken as 1.
 * Trailing zeros are removed from the fractional portion of the result
 * and a decimal-point character appears only if it is followed by a digit.>
 *
 * Microsoft says:
 * <Signed values are displayed in f or e format, whichever is more compact for the given value and precision.
 * The e format is used only when the exponent of the value is less than -4 or greater than or equal to the precision argument.
 * Trailing zeros are truncated, and the decimal point appears only if one or more digits follow it.>
 *
 * FMTF_Hash option flag has no effects
 *
 *  Returns effective size taken by the value once it has been formatted. This size is expressed in characters units (and not in bytes).
 *
 *
 * Precision : represents the number of significant digits to be printed (integer part + fractional part)
 * Trailing zeroes are removed from fractional part
 * Reminder: for f e and a, precision is the number of fractional digits to be printed (number of digits after the decimal point)
 * and trailing zeroes are not removed.
 * @}
 *
 * @param pBuffer
 * @param pCurrlen
 * @param pMaxlen
 * @param pFvalue
 * @param pFieldMin
 * @param pFieldPrecision
 * @param pFmtFlags
 * @return
 */
template <class _Utf, class _FTp>
size_t
utfFormatFPG(_Utf *pBuffer, size_t * pCurrlen, size_t pMaxlen,
             _FTp pFvalue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFmtFlags)
{

    _Utf            wSignvalue=0;        /* sign of expression to be printed 0: no sign __MINUS__ or __PLUS__ or __SPACE__ */
    _Utf            wDecimalPoint=0;     /* decimal point : if has to be written, will be set from locale */
    _FTp            wUfvalue;            /* fvalue unsigned value */

    _Utf*           wDigitStr=nullptr ; /* fraction part to be printed (in reverse order)*/

    int             wDigitsNb = 0;      /* number of digits */
    int             wFieldSize=0;       /* total number of slots taken by number (including sign, decimal point) */
    int             wSlotsSize=0;
    size_t          wAllocatedSlots = 0;/* total number of allocated digit slots  */
    int             wIntegerNb = 0;     /* number of integer digits */
    int             wFractNb    = 0;    /* number of fractional digits */

    int             wPadlen = 0;         /* amount to pad */
    int             wZpadlen = 0;
    ZBool           wSubOne=false;      /* set to true if value is less than 1.0 */

    LONG_DOUBLE     wFSignificant;
    LONG_LONG       wSignificant,wIntegerPart;           /* fvalue fract part */

    size_t          wLocalSize=*pCurrlen;

    size_t wAllocated=0;                /* Number of allocated digits by utfSPFConvertInt when called */

    unsigned int    wBase=10;
    double          wFBase=10.0;
    double          wRndCoef = 0.5; /* 0.5 (10/20) for base 10  0.8 (16/20) for hexa 0.4 (8/20) for octal  */

    if (isnan(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
            {
            return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin,(_Utf*)cst_NAN<_Utf>,  pFmtFlags);
            }
        else
            {
           return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_nan<_Utf>,  pFmtFlags);
            }
        }// if (isnan(pFvalue))
    if (!isfinite(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_INFINITE<_Utf>,  pFmtFlags);
                }
            else
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_infinite<_Utf>,  pFmtFlags);
                }
        }

    if (pFmtFlags & FMTF_Hexa) /* this should not be the case but it is kept here for experimental purposes */
                        {
                        wBase = 16;
                        wFBase = (double)16.0;
                        wRndCoef = (double)0.8;
                        }

/* check exponent value */

    int wExponent = 0;
    if (pFvalue < 0.0)
            wUfvalue = -pFvalue;
        else
            wUfvalue = pFvalue;


    wExponent = getExponent(wUfvalue,(LONG_DOUBLE)wFBase);

    /*
    * Signed values are displayed in f or e format, whichever is more compact for the given value and precision.
    * A precision of 0 is taken as 1.
    * The e format is used only when the exponent of the value is less than -4 or greater than or equal to the precision argument.
    * Trailing zeros are truncated, and the decimal point appears only if one or more digits follow it.
    * if <f> format is retained, precision (defaulted to 6) represent the number of significant digits to be printed (integer part + fractional part)
    * if scientific <e> notation is retained, precision represents the whole number of fractional digits minus 1 for integer part: only remove trailing zeroes
    *
    */
    if (pFieldPrecision==0)
                pFieldPrecision = 1; /* make this test before following test*/
    if (!(pFmtFlags&FMTF_Precision)||(pFieldPrecision<0))
                        pFieldPrecision = 6;  /* for <g> format precision is defaulted to 6 */


    /* wExponent contains number of integer digits
        so Number of fractional digits is deduced from pFieldPrecision - number of integer digits
    */
    if ((wExponent < -4) || (wExponent >= pFieldPrecision)) /*------------ preference for <e> format---------- */
/* if scientific notation is chosen, precision represents
 * the whole number of fractional digits minus 1 for integer part: additionally trim trailing fractional zeroes */
        {
//        int wFieldPrecision = pFieldPrecision - 1;
        int wFieldPrecision = pFieldPrecision ;

        if (wFieldPrecision<0)
                    wFieldPrecision=0;
        return  utfFormatFPE(pBuffer, pCurrlen,pMaxlen,
                              pFvalue, pFieldMin,wFieldPrecision,pFmtFlags | FMTF_NonSignif);
        }

/* -------------preference for <f> format---------------------- */

/*
    For <g> or <G> format, pFieldPrecision is the number of significant digits to be printed :
    examples :

    1.056000  -> 4 significant digits <1056>
    0.000589  -> 3 significant digits <589>
    etc.

    if pFieldPrecision equals zero then 1 significant digit is to be printed.

   compute number of significant digits

        All integer digits are to be printed whatever pFieldPrecision value is.

        if absolute value less than one :
        significant digits are non zero digits right to the decimal point (fractional digits)

        if absolute value greater than or equal to one :
        significant digits are all integer digits + remaining fractional digits
        remaining fractional digits computed as (pFieldPrecision - integer digits ) < 0 ? 0 :  (pFieldPrecision - integer digits )
*/
    if (pFvalue<0)
        {
        wSignvalue = (_Utf)__MINUS__;
        wUfvalue = -pFvalue;
        }
    else
        {
        wUfvalue = pFvalue;

        if (pFmtFlags & FMTF_Plus)
                wSignvalue = (_Utf)__PLUS__;
        else if (pFmtFlags & FMTF_SignSpace)
                        wSignvalue = (_Utf)__SPACE__; /* FMTF_Space applies only if FMTF_Plus is not set */

        }// else

    wFSignificant= wUfvalue;
    wDigitsNb=LDoubleToUtfDigits_G<_Utf>(wFSignificant,
                                       &wDigitStr,
                                       &wFractNb,
                                       &wIntegerNb,
                                       &wAllocatedSlots,
                                       pFieldPrecision,
                                       pFmtFlags);


    if ((pFmtFlags & FMTF_Hash)||(wFractNb > 0)) /* if FMTF_Hash is set or fractional digit number > 0, there is a decimal point */
                            wDecimalPoint= _zlocale.getLocaleDecimalPoint<_Utf> ();

    if (wSignvalue &&!(pFmtFlags & FMTF_SignFolw)&&!(pFmtFlags & FMTF_SignPrec)) /* if neither option for sign positionning */
                                                        pFmtFlags |= FMTF_SignPrec; /* sign preceeds : this is the default */

    wFieldSize = wDigitsNb + (wDecimalPoint ? 1 : 0 ) + (wSignvalue ? 1 : 0);
    wSlotsSize = wFieldSize > pFieldMin ? wFieldSize:pFieldMin;

    wPadlen = wSlotsSize - wFieldSize;
    /*  Padding */

    /* zero padding only left padding and only if not space padding */

    if (wPadlen < 0)
                wPadlen = 0;
    if (pFmtFlags & FMTF_Left)
                    wPadlen = -wPadlen;       /* Left Justifty */

    if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))   /* left padding with zeroes ? */
        {
        if (wSignvalue && (pFmtFlags & FMTF_SignPrec))
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);    /* sign preceeds */
            --wPadlen;
            wSignvalue = 0;
            }
        while (wPadlen > 0)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
            --wPadlen;
            }
        }//if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))
        else
        {   /* No zero padding : pad with space if have to */
        while (wPadlen > 0)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
            --wPadlen;
            }
        if (wSignvalue && (pFmtFlags & FMTF_SignPrec))
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);
            wSignvalue=0;
            }
        }  // else

    /*  Digits are stored in reverse order */
#ifdef __COMMENT__
    if (wSubOne) /* and skip artificially added 1 unit if value is a decimal value and set integer part to 0 */
        {
        _Utf wFirstDigit =  wDigitStr[--wDigitsNb]; /* get rid of added 1  to first digit could be 1 or 2 depending on possible rounding */
        wFirstDigit--; /* assuming __ZERO__ comes before __ONE__ within character set */
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen,(_Utf)wFirstDigit);
        wSubOne=false;
        wIntegerNb=0;
        }
#endif // __COMMENT__
    while (wDigitsNb > 0)
        {
        if (wIntegerNb==0)  /* when integer part is zero : put the decimal point in place */
                    {
                    if (wDecimalPoint)
                        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDecimalPoint);
                    wDecimalPoint = 0;
                    }
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDigitStr[--wDigitsNb]);
        wIntegerNb--;
        }

    if (wDecimalPoint)
        {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDecimalPoint);
        }

/* No zero right padding : has already been taken into account with precision conversion */
/* only right space padding wether necessary */

    if (wSignvalue) /* if sign remains to be set : sign is right */
        {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);    /* sign preceeds */
        --wPadlen;
        wSignvalue = 0;
        }

    while (wPadlen < 0) {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        ++wPadlen;
        }
    if (wSignvalue )
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);

    zfree(wDigitStr);
    return  (*pCurrlen-wLocalSize);
}// utfFormatFPG


/**
 * @brief utfFormatFPA Formats a floatting point value (double or long double according _FTp) in an hexadecimal format<br>
 *
 *@weakgroup FORMATFPA  floating point value according to format <a> or <A>
 * @{
 *
 * Format specifier 'a' or 'A' formats a floatting point value (double or long double according type size specifier)
 * in an hexadecimal format<br>
 *
 *  Exponent is a power of 2 and is expressed as a signed decimal number with at least 1 digit (gnu standard).
 *  [-]0xh.hhhhp±d,<br>
 * GNU documentation <br>
 *  @link https://www.gnu.org/software/libc/manual/html_node/Floating_002dPoint-Conversions.html#Floating_002dPoint-Conversions
 *
 * Microsoft documentation <br>
 * @link https://docs.microsoft.com/en-us/cpp/c-runtime-library/format-specification-syntax-printf-and-wprintf-functions?view=vs-2017
 *
 * Signed hexadecimal double-precision floating-point value that has the form [-]0xh.hhhhp±d,
 * where h.hhhh are the hex digits (using lower case letters) of the mantissa,
 * and dd are ONE or more digits for the exponent.
 * The precision specifies the number of digits after the decimal point.
 * -----------------
 * Nota bene : if no precision is mentionned, Microsoft says the default precision is 13 while GNU says<br>
 *  <If the precision is zero it is determined to be large enough to provide an exact representation of the number
 * (or it is large enough to distinguish two adjacent values if the FLT_RADIX is not a power of 2)>,
 * which is quite hermetically not understandable.
 * We retain the choice of defaulting precision to 13 digits after the decimal point sign.
 *
 * Trailing zeroes are kept(/added) up until precision (either mentionned, or defaulted),
 * excepted if formatting flag FMTF_NonSignif is set :
 * in this case, trailing zeroes are removed and decimal point is removed excepted if FMTF_Hash flag option is set.
 *
 * Rounding rule : Numbers are rounded to the last significant hexadecimal digit available according precision and value.
 *@}
 *
 * @param pBuffer
 * @param pCurrlen
 * @param pMaxlen
 * @param pFieldPrecision
 * @param pFmtFlags
 * @return
 */
template <class _Utf,class _FTp>
size_t
utfFormatFPA(_Utf *pBuffer, size_t * pCurrlen, size_t pMaxlen,
      _FTp pFvalue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFmtFlags)
{

    _Utf            wSignvalue=0;       /* sign of expression to be printed 0: no sign __MINUS__ or __PLUS__ or __SPACE__ */
    _Utf            wDecimalPoint=0;    /* decimal point : if has to be written, will be set from locale */
    _FTp            wUfvalue;           /* fvalue unsigned value */
    _Utf            wExpSign = 0;       /* Exponent sign */

    _Utf*           wDigitStr=nullptr ; /* fraction part to be printed (in reverse order)*/
    _Utf*           wEDigitStr=nullptr;  /* exponent value to be printed (in reverse order)*/

    int             wDigitsNb = 0;         /* number of fractional digits */
    int             wESlots = 0;         /* number of exponent digits */

    int             wPadlen = 0;         /* amount to pad */
    int             wZpadlen = 0;

    int             wPrecision=pFieldPrecision;

    LONG_DOUBLE     wFSignificant;

    int             wExponent = 0;

    size_t          wLocalSize=*pCurrlen;

    size_t          wAllocatedSlots=0;                /* Number of allocated digits by utfSPFConvertInt when called */

    double          wFBase=16.0;
    double          wRndCoef = 0.5 ; /* 0.5 (10/20) for base 10  0.8 (16/20) for hexa 0.4 (8/20) for octal  */

    if (isnan(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
            {
            return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin,(_Utf*)cst_NAN<_Utf>,  pFmtFlags);
            }
        else
            {
           return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_nan<_Utf>,  pFmtFlags);
            }
        }// if (isnan(pFvalue))
    if (!isfinite(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_INFINITE<_Utf>,  pFmtFlags);
                }
            else
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_infinite<_Utf>,  pFmtFlags);
                }
        }

    _ASSERT_(!(pFmtFlags & FMTF_Hexa),"  format must be hexadecimal for this number %Lf",pFvalue) /* this must not be the case by left for experimental purposes */

    /* no octal for float : if FMTF_Octal is set : processed as default base  */

/*
 * Microsoft says the default precision is 13 while GNU says
 *  If the precision is zero it is determined to be large enough to provide an exact representation of the number
 * (or it is large enough to distinguish two adjacent values if the FLT_RADIX is not a power of 2)
 *
 * Let's take 13 (GNU induces additional, un-necessary computations).
 * However, defaulted precision may induce additional zeroes put at the right of last significant digit.
 * In this case, these zeroes are suppressed.
 *
 */
    if (!(pFmtFlags & FMTF_Precision )) /* no precision given */
                    {
                    wPrecision = 13;
                    }
    if (pFieldPrecision < 0)            /* or precision flag is set but number is negative ??? */
                    wPrecision = 13;

/* no octal allowed here */


/* -----------------address when value is 0.0 ------------------*/
    if (pFvalue==0.0)
    {
        wSignvalue=0;
        if (pFmtFlags & FMTF_Plus)
                wSignvalue = (_Utf)__PLUS__;
        else if (pFmtFlags & FMTF_SignSpace)        /* space is used if and only if FMTF_Plus is not set (standard says) */
                        wSignvalue = (_Utf)__SPACE__;

        if ((pFmtFlags & FMTF_Precision)||(pFmtFlags&FMTF_Hash))
            {
            if (wPrecision < 1)
                    wPrecision = 0;
                else
                    wDecimalPoint = _zlocale.getLocaleDecimalPoint<_Utf>();
            }
            else
            wPrecision = 0;
        if (!wPrecision&&(pFmtFlags&FMTF_Hash))
                    {
                    wDecimalPoint = _zlocale.getLocaleDecimalPoint<_Utf>();
                    wPrecision=1;
                    }
        /* pad len = field minimum size - decimal point - size of exponent - sign size - size of '0X' */
        wPadlen = pFieldMin                 // field minimum size
                - 1                         // size of first zero digit
                - wPrecision           // number of fractional digits
                - (wDecimalPoint?1:0)       // size of decimal point
                - 3                         // size of exponent notation <p+0>
                - (wSignvalue ? 1 : 0)      // sign size if exists
                - 2;                        // size of leading '0X'


        wZpadlen = wPrecision ;    // zero padding is precision : any digit is zero
        if (wZpadlen < 0)
                    wZpadlen = 0;
        if (wPadlen < 0)
                    wPadlen = 0;
        if (pFmtFlags & FMTF_Left)
                        wPadlen = -wPadlen;       /* Left Justifty */
        if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))
            {
/*            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
            if (pFmtFlags&FMTF_Uppercase)
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__X_UPPER__);
            else
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__X_LOW__);
*/            if (wSignvalue)
                {
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);
                --wPadlen;
                wSignvalue = 0;
                }
                while (wPadlen > 0)
                {
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
                --wPadlen;
                }
            }//if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))
        else
        {
        while (wPadlen > 0) {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
            --wPadlen;
            }
        if (wSignvalue)
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
        if (pFmtFlags&FMTF_Uppercase)
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__X_UPPER__);
        else
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__X_LOW__);
        }// else

/* write the number */
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
        if (wDecimalPoint)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDecimalPoint);
            /* Zero padding for fractional part */
            while (wZpadlen > 0)
                    {
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
                --wZpadlen;
                    }
            }

/* exponent */
        if (pFmtFlags & FMTF_Uppercase) /* exponent mark is <p> or <P> */
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen,(_Utf) __P_UPPER__ );
            else
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen,(_Utf) __P_LOW__ );
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__PLUS__);
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);

        return  (*pCurrlen)- wLocalSize;
    }

/* ------------up to here value is not 0.0 ---------------------*/

    if ((pFmtFlags & FMTF_Hash)||(wPrecision > 0))
                            wDecimalPoint= _zlocale.getLocaleDecimalPoint<_Utf> (); /* get local decimal sign. */

    if (pFvalue < 0.0)
            {
            wSignvalue = (_Utf)__MINUS__;
            wUfvalue = - pFvalue;
            }
        else
        {
        wUfvalue = pFvalue;
        if (pFmtFlags & FMTF_Plus)
                wSignvalue = (_Utf)__PLUS__;
        else if (pFmtFlags & FMTF_SignSpace)        /* space is used if and only if FMTF_Plus is not set (standard says) */
                        wSignvalue = (_Utf)__SPACE__;
        }
    wFSignificant= (LONG_DOUBLE)wUfvalue;
    wFSignificant= frexp(wFSignificant,&wExponent);

    wExponent -= 4 ; /* why ? Because frexp delivers a number between 0.5 and 1 and we need an hexa number (0-15) */

    int wISignif1,wISignif = (int)wFSignificant;

    LONG_DOUBLE wRise = powl((LONG_DOUBLE)16.0,(LONG_DOUBLE)wPrecision+1);
    wFSignificant *=  wRise; /* power up to pFieldPrecision time base (hexa) */
/* rounding for this format : numbers are simply rounded as decimal numbers to the appropriate precision */

    wFSignificant = FPRound<LONG_DOUBLE>(wFSignificant,wRndCoef,GFloat_A_Rnd_GT);/* round the significant with appropriate round coef : 0.5*/

/* recompute values after rounding : I know this is not much elegant, but only efficient and accurate */
    LONG_DOUBLE wFSignif1= wFSignificant / wRise;
    wISignif1 = (int)wFSignif1;

    if (wISignif1 > wISignif )  /* first digit hexa overflow ? */
                    wExponent+=4;

/* process exponent : exponent is power of 2 expressed with an always signed decimal number with a minimum of 1 digits
 *  or 3 digits if Windows */
    if (wExponent < 0)
                {
                wExponent = - wExponent;
                wExpSign = (_Utf)__MINUS__;
                }
        else
                wExpSign = (_Utf)__PLUS__;

    utfSPFConvertInt<_Utf,int>(wExponent, &wEDigitStr,&wAllocatedSlots,&wESlots,2,FMTF_Nothing); /* format flag means decimal */
    if (wAllocatedSlots <= ((size_t)wESlots + 5))
        {
        wAllocatedSlots += 5;
        wEDigitStr =zrealloc(wEDigitStr,wAllocatedSlots);
//        _ASSERT_(!wEDigitStr,"cannot allocate memory",1);
        }
/* For format specifier %a standard says : exponent has a minimum of 1 digit, even if exponent is 0
    Exponent is always signed */

    if (wESlots < 1) /* utfConvertLong allocation is at least (5 + min) digit units : so, not necessary to test allocation */
        wEDigitStr[wESlots++]=(_Utf)__ZERO__;

    wEDigitStr[wESlots++] = wExpSign ; /* exponent is always signed */

    if (pFmtFlags & FMTF_Uppercase) /* exponent mark is <p> or <P> */
            wEDigitStr[wESlots++] = (_Utf) __P_UPPER__ ;
        else
            wEDigitStr[wESlots++] = (_Utf) __P_LOW__ ;

    wEDigitStr[wESlots] = (_Utf)__ENDOFSTRING__; /* Done */

/* significant part conversion */
//    utfSPFConvertInt<_Utf,LONG_LONG>(wSignificant, &wFDigitStr,&wAllocatedSlots, &wFDigitsNb,pFieldPrecision+1,pFmtFlags);/* flags apply here*/

   utfConvertLDouble<_Utf>(wFSignificant,
                            &wDigitStr,
                            &wAllocatedSlots,
                            &wDigitsNb,
                            wPrecision+1,
                            pFmtFlags);
/* process unsignificant zeroes */

   /* if no precision requested OR FMTF_NonSignif flag set : trim trailing zeroes */

    if ((pFmtFlags & FMTF_NonSignif)||!(pFmtFlags & FMTF_Precision))
        {
        int wWPrecision = (pFmtFlags & FMTF_Precision)?wPrecision+1:INT_MAX;
        int wi=0;
        while ((wi<wDigitsNb)&&(wi<wWPrecision)&&(wDigitStr[wi]==(_Utf)__ZERO__))
                                wi++;
        if (wi)
            {
            int wJ=0;
            while ((wJ+wi)< wDigitsNb)
                    {
                    wDigitStr[wJ]=wDigitStr[wJ+wi];
                    wJ++;
                    }
            wDigitStr[wJ]=(_Utf)__ENDOFSTRING__;
            wDigitsNb = wJ;
            wPrecision=wDigitsNb-1;
            if (wPrecision<0)
                    wPrecision=0;
            }
        }// if ((pFmtFlags & FMTF_NonSignif)||!(pFmtFlags & FMTF_Precision))

    if ((wPrecision<1)&&(wDigitsNb<2)&&!(pFmtFlags&FMTF_Hash))
                                    wDecimalPoint =0;

/* Nota bene : sign always preceeds the number in scientific notation */

/* left space padding */

/*  string space structure for a float expressed in scientific format

     Float hexadecimal notation

    0.hhhhhhp+dd

    where
    <h> is a fractional hexadecimal digit
    <p> or <P> depending on FMTF_Uppercase
    dd is the exponent is power of 2 (according FP_RADIX) expressed as a signed DECIMAL integer.

    float precision concerns <h> fractional number of digits (i. e. non null - significant digits)
*/
/*                                                       Last 2 concerns <0x> or <0X> mark */
    wPadlen = pFieldMin  - pFieldPrecision - (wDecimalPoint?1:0) - wESlots - ((wSignvalue) ? 1 : 0) - 2 ;
    if (wPrecision > 0)  /* Precision is not zero, then take care of decimal point */
            {
            if (wPadlen<0)
                        wPadlen++;
                    else
                        wPadlen --;
            }
    wZpadlen = wPrecision - wDigitsNb;
    if (wZpadlen < 0)
                wZpadlen = 0;
    if (wPadlen < 0)
                wPadlen = 0;
    if (pFmtFlags & FMTF_Left)
                    wPadlen = -wPadlen;       /* Left Justifty */

    if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))
        {
/*        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
        if (pFmtFlags&FMTF_Uppercase)
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__X_UPPER__);
        else
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__X_LOW__);
*/        if (wSignvalue)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);
            --wPadlen;
            wSignvalue = 0;
            }
            while (wPadlen > 0)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
            --wPadlen;
            }
        }//if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))
    else
    {
    while (wPadlen > 0) {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        --wPadlen;
        }
    if (wSignvalue)
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);
/*    utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
    if (pFmtFlags&FMTF_Uppercase)
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__X_UPPER__);
    else
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__X_LOW__);*/
    }// else


/* first digit */

    utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDigitStr[--wDigitsNb]);

/* here takes place decimal sign (or not if no more digits )
    decimal point is written only if precision is not zero - or precision is zero but FMTF_Hash flag is set
*/
    if (wDecimalPoint)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDecimalPoint);
            if (wDigitsNb < 1)
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
            }
        else
        {
        if ((pFvalue==0.0)&&(wDigitsNb<1))
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
        }
/* fractional part */
    int wWPrecision=wPrecision;
    while ((wDigitsNb > 0)&&(wWPrecision-- > 0))
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDigitStr[--wDigitsNb]);

/* Zero padding for fractional part */

    while (wZpadlen > 0)
            {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
        --wZpadlen;
            }

/* exponent */
    while (wESlots > 0)
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wEDigitStr[--wESlots]);

/* right space padding */

    while (wPadlen < 0) {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        ++wPadlen;
    }

    zfree(wDigitStr);
    zfree(wEDigitStr);
    return  (*pCurrlen-wLocalSize);
}// utfFormatFPA

/**
 * @brief utfFormatFPK Formats to pBuffer, an _Utf string, a floating point value according format <k> or <K>
 *
 * @weakgroup FORMATFPK  floating point value according to format <k> or <K>
 * @{
 * This is an extension to sprintf format specification : k format describes amount formatting:<br>
 *
 * default format is dd'ddd'ddd.ff [<cur>] <br>
 *
 * where <cur> is optional local currency symbol<br>
 *
 * Additional flag option :
 * sign flag value<br>
 * y    FMTF_Currency   add currency symbol  - currency symbol is defined within locale<br>
 * y<   FMTF_Preceeds   currency symbol preceeds the amount<br>
 * y>   FMTF_Follows    currency symbol follows the amount<br><br>
 *
 * |    FMTF_Group  add digit grouping separator - monetary digit separator is defined within locale.<br>
 *                  If not defined, default grouping separator is taken.<br>
 *
 * FMTF_Hash option flag forces a decimal point (locale) to be put even if number has no fractional digits.
 * Otherwise decimal point is omitted.<br>
 * FMTF_Group induce group of n digits (locale grouping number rule) to be separated with locale defined thousand separator character.<br>
 * @see template getLocalThousandsSeparator() <br>
 *
 *
 * Rounding :<br>
 *
 *  Rounding is made on the last significant digit <br>
 * @}
 *
 * @param pBuffer         _Utf string as output
 * @param pCurrlen        current offset within pBuffer so far. Updated with current printed size and returned.
 * @param pMaxlen         Overall maximum slots to be consumed within pBuffer. current offset cannot be greater than this.
 * @param pFvalue         floating point to format
 * @param pMin            minimum slots to be consumed
 * @param pFieldPrecision maximum number of slots to be consumed.
 *                        -1 means default : AIX manpage says the default is 0, but Solaris says the default is 6,
 *                        and sprintf on AIX defaults to 6.
 * @param flags           Formatting flags for position and size. @see FMT_Flags
 */
template <class _Utf, class _FTp>
size_t
utfFormatFPK(_Utf *pBuffer, size_t * pCurrlen, size_t pMaxlen,
             _FTp pFvalue, int pFieldMin, int pFieldPrecision, int pCurrencySpaces,
             FMT_Flags_type pFmtFlags)
{

    _Utf            wSignvalue=0;        /* sign of expression to be printed 0: no sign __MINUS__ or __PLUS__ or __SPACE__ */
    _Utf            wDecimalPoint=0;     /* decimal point : if has to be written, will be set from locale */
    _FTp            wUfvalue;            /* fvalue unsigned value */
    _Utf*           wDigitStr=nullptr;   /* Signigicant digits to be printed */
    _Utf*           wFconvert=nullptr;   /* fraction part to be printed */
    int             wDigitsNb = 0;         /* number of significant digits */
    size_t          wAllocatedSlots = 0;/* total number of allocated digit slots  */
    int             wPadlen = 0;         /* amount to pad */
//    int             wZpadlen = 0;

    size_t          wFieldSize, wSlotsSize;

//    int             wFieldPrecision=pFieldPrecision;

    unsigned int    wBase=10;
    _FTp            wFBase=10.0;
    double          wRndCoef=0.5;
    size_t          wLocalSize=*pCurrlen;

    size_t          wCurrencySize=0;
    _Utf*           wCurrency=nullptr;
    _Utf            wGroupSeparator=0;
    int             wLocalGroupNumber=0, wGroups=0;

    _Utf*           wValueBuffer=nullptr;
    _Utf*           wVBPtr=nullptr;

    if (isnan(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
            {
            return  utfSignalNan(pFvalue, pBuffer, pCurrlen, pMaxlen, pFieldMin,(_Utf*)cst_NAN<_Utf>,  pFmtFlags);
            }
        else
            {
           return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_nan<_Utf>,  pFmtFlags);
            }
        }// if (isnan(pFvalue))
    if (!isfinite(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_INFINITE<_Utf>,  pFmtFlags);
                }
            else
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_infinite<_Utf>,  pFmtFlags);
                }
        }

    _ASSERT_((pFmtFlags & FMTF_Octal)||(pFmtFlags & FMTF_Hexa),"Hexadecimal for float must be addressed using utfFormatFPHex. Octal is invalid for float.",1);
    // no hexa no octal allowed

    /*
     * AIX manpage says the default is 0, but Solaris says the default
     * is 6, and sprintf on AIX defaults to 6
     */
    if (!(pFmtFlags & FMTF_Precision)) /* no precision given */
                    pFieldPrecision = _zlocale.getLocaleMonetaryDecimal();    /* Default amount precision is defined by locale */
    if (pFieldPrecision < 0)
                    pFieldPrecision = _zlocale.getLocaleMonetaryDecimal(); /* or precision flag is set but number is negative ??? */

    wUfvalue = abs_val(pFvalue);

    /* We support as much digits as digits available for a double or a long double float (depending on requested format).
     * The only limitation is due to float rules : FLT_MAX is the maximum
     */

    LONG_DOUBLE wFNewValue = pFvalue ;
    ZBool wSubOne=false;
    if (wFNewValue < 0)
            {
            wSignvalue = (_Utf)__MINUS__;
            wFNewValue = - wFNewValue;
            }
        else
        if (pFmtFlags & FMTF_Plus)
                wSignvalue = (_Utf)__PLUS__;
        else if (pFmtFlags & FMTF_SignSpace)
                        wSignvalue = (_Utf)__SPACE__; /* FMTF_Space applies only if FMTF_Plus is not set */
    if (wFNewValue < 1.0)
            {
            wFNewValue += 1.0; /* plus one significant digit to keep leading zeroes if any */
            wSubOne=true;
            }
    wFNewValue = wFNewValue * powl((LONG_DOUBLE)wFBase,(LONG_DOUBLE)pFieldPrecision);
    /* rounding */
    wFNewValue=FPRound<LONG_DOUBLE>(wFNewValue,wRndCoef,GFloat_F_Rnd_GT);
    /* convert to _Utf string (digits in reverse order) */
    utfConvertLDouble<_Utf>( wFNewValue,
                             &wDigitStr,
                             &wAllocatedSlots,
                             &wDigitsNb,
                             __INIT_INTEGER_DIGITS__,
                             pFmtFlags);

    int wIntegerNb= wDigitsNb - pFieldPrecision ;
    if (wIntegerNb<0)
            wIntegerNb=1; /* no negative value for integer part digit number */

    if (pFmtFlags&FMTF_NonSignif) /* if requested to remove non significant zeroes */
        {
        int wi=0,wj=0;
        while ((wi<pFieldPrecision)&&(wDigitStr[wi]==(_Utf)__ZERO__))
                    wi++;
        if (wi)
            {
            while (wi<wDigitsNb) /* remove non significant zeroes */
                {
                wDigitStr[wj++]= wDigitStr[wi++];
                }
            wDigitsNb=wj;
            wDigitStr[wj]=__ENDOFSTRING__;
            }
        }//if (FMTF_NonSignif)

    if ((pFmtFlags & FMTF_Hash)||(pFieldPrecision>0)) /* if FMTF_Hash is set or precision > 0, there is a decimal point */
                            wDecimalPoint= _zlocale.getLocaleDecimalPoint<_Utf> ();

    if (wSignvalue && !(pFmtFlags & FMTF_SignFolw)&&!(pFmtFlags & FMTF_SignPrec)) /* if neither option for sign positionning */
            {
            if (_zlocale.getLocaleSignPosition())                /* see locale */
                        pFmtFlags |= FMTF_SignPrec;
                    else
                        pFmtFlags |= FMTF_SignFolw;
            }

    if (pFmtFlags & FMTF_Currency)
        {
        wCurrency=_zlocale.getLocaleCurrencyCode<_Utf>();
        wCurrencySize=utfStrlen<_Utf>(wCurrency); /* must be replaced with appropriate utfCount routine : canonical character and not char unit count */
        }


    wFieldSize = wDigitsNb;
    wFieldSize += wGroups ;                     /* number of separators if separator is used(otherwise equals 0) */
    wFieldSize +=  wCurrency? wCurrencySize:0;  /* size of currency if it is used */
    wFieldSize += pCurrencySpaces  ;            /* number of spaces between currency code and amount */
    wFieldSize += (wDecimalPoint ? 1 : 0 )  ;   /* decimal point if used */
    wFieldSize += (wSignvalue ? 1 : 0) ;        /* sign sign if used */

    wSlotsSize = wFieldSize >(size_t)pFieldMin ? wFieldSize:pFieldMin;

    wPadlen = wSlotsSize - wFieldSize;

    wValueBuffer=(_Utf*)malloc((wSlotsSize+10)*sizeof(_Utf));
    memset(wValueBuffer,0,(wSlotsSize+10)*sizeof(_Utf));
    wVBPtr=wValueBuffer;

    int wGC=0,wDigitCnt=0,wEffSeparator=0;
    _Utf wThousandSep=0;
    /*  Padding */

    /* zero padding only left padding and only if not space padding */

    if (wPadlen < 0)
                wPadlen = 0;
    if (pFmtFlags & FMTF_Left)
                    wPadlen = -wPadlen;       /* Left Justifty */

    if (pFmtFlags & FMTF_Currency )
            {
            _Utf* wCur=_zlocale.getLocaleCurrencyCode<_Utf>();
            int wLCur=utfStrlen<_Utf>(wCur);
            wLCur--;
            while (wLCur>=0)
                {
                *(wVBPtr++)=wCur[wLCur--];
                }
            while(pCurrencySpaces--)
                *(wVBPtr++)=(_Utf)__SPACE__;
            }


    int wDigitIdx=0;

    if (pFieldPrecision)
            wDecimalPoint = _zlocale.getLocaleDecimalPoint<_Utf>();

    wThousandSep=0;

    /* No thousand separators for fractional digits */

    if (!pFieldPrecision) /* if there is no fractional digits : set up thousand separator if required*/
        {
            if (pFmtFlags & FMTF_Group)
                    {
                    wThousandSep=_zlocale.getLocaleGroupSeparator<_Utf>();
                    wLocalGroupNumber= _zlocale.getLocaleGroupNumber();
                    }
        }//if (!pFieldPrecision)

    wGC=0;

    while (wDigitIdx<wDigitsNb)
            {
        if ((wDecimalPoint)&&(wDigitIdx==pFieldPrecision))
                        {
                        *(wVBPtr++)=wDecimalPoint;
                        wDecimalPoint=0;
                        if (pFmtFlags & FMTF_Group)
                                {
                                wThousandSep=_zlocale.getLocaleGroupSeparator<_Utf>();
                                wLocalGroupNumber= _zlocale.getLocaleGroupNumber();
                                wGC=0;
                                }
                        }
        if (wThousandSep)
                    {
                    if (wGC>=wLocalGroupNumber)
                        {
                        *(wVBPtr++)=wThousandSep;
                        wPadlen--;
                        if (wPadlen<0)
                                wPadlen=0;
                        wGC=0;
                        }
                    }//if (wThousandSep)
            *(wVBPtr++)=wDigitStr[wDigitIdx++];
            wGC++;
            }
/* when thousand separator is the very first sign to be put : it is replaced by a space*/
    while (wPadlen>0)
        {
        if (wThousandSep)
        {
        if (wGC==wLocalGroupNumber)
            {
            if (wPadlen==1)
                *(wVBPtr++)=(_Utf)__SPACE__;
            else
                *(wVBPtr++)=wThousandSep;

            wPadlen--;
            if (wPadlen<0)
                    wPadlen=0;
            wGC=0;
            }

        }//if (wThousandSep)
        if (wPadlen)
            {
            *(wVBPtr++)=(_Utf)__ZERO__;
            wPadlen--;
            if (wPadlen<0)
                    wPadlen=0;
            }
        wGC++;
        }

    if (wSignvalue)
        *(wVBPtr++)=wSignvalue;


    while (wVBPtr>wValueBuffer)
        {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)*(--wVBPtr));
        }


/* only right space padding wether necessary */
    while (wPadlen < 0) {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        ++wPadlen;
        }

    zfree(wValueBuffer);
    zfree(wDigitStr);
    zfree(wFconvert);
    return  (*pCurrlen-wLocalSize);
}// utfFormatFPK

template <class _Utf, class _FTp>
size_t
utfFormatFPK_1(_Utf *pBuffer, size_t * pCurrlen, size_t pMaxlen,
             _FTp pFvalue, int pFieldMin, int pFieldPrecision, int pCurrencySpaces,
             FMT_Flags_type pFmtFlags)
{

    _Utf            wSignvalue=0;        /* sign of expression to be printed 0: no sign __MINUS__ or __PLUS__ or __SPACE__ */
    _Utf            wDecimalPoint=0;     /* decimal point : if has to be written, will be set from locale */
    _FTp            wUfvalue;            /* fvalue unsigned value */
    _Utf*           wDigitStr=nullptr;   /* Signigicant digits to be printed */
    _Utf*           wFconvert=nullptr;   /* fraction part to be printed */
    int             wDigitsNb = 0;         /* number of significant digits */
    size_t          wAllocatedSlots = 0;/* total number of allocated digit slots  */
    int             wPadlen = 0;         /* amount to pad */
//    int             wZpadlen = 0;

    size_t          wFieldSize, wSlotsSize;

//    int             wFieldPrecision=pFieldPrecision;

    unsigned int    wBase=10;
    _FTp            wFBase=10.0;
    double          wRndCoef=0.5;
    size_t          wLocalSize=*pCurrlen;

    size_t          wCurrencySize=0;
    _Utf*           wCurrency=nullptr;
    _Utf            wGroupSeparator=0;
    int             wLocalGroupNumber=0, wGroups=0;

    _Utf*           wValueBuffer=nullptr;

    if (isnan(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
            {
            return  utfSignalNan(pFvalue, pBuffer, pCurrlen, pMaxlen, pFieldMin,(_Utf*)cst_NAN<_Utf>,  pFmtFlags);
            }
        else
            {
           return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_nan<_Utf>,  pFmtFlags);
            }
        }// if (isnan(pFvalue))
    if (!isfinite(pFvalue))
        {
        if (pFmtFlags & FMTF_Uppercase)
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_INFINITE<_Utf>,  pFmtFlags);
                }
            else
                {
                return  utfSignalNan(pFvalue,pBuffer, pCurrlen, pMaxlen, pFieldMin, (_Utf*)cst_infinite<_Utf>,  pFmtFlags);
                }
        }

    _ASSERT_((pFmtFlags & FMTF_Octal)||(pFmtFlags & FMTF_Hexa),"Hexadecimal for float must be addressed using utfFormatFPHex. Octal is invalid for float.",1);
    // no hexa no octal allowed

    /*
     * AIX manpage says the default is 0, but Solaris says the default
     * is 6, and sprintf on AIX defaults to 6
     */
    if (!(pFmtFlags & FMTF_Precision)) /* no precision given */
                    pFieldPrecision = _zlocale.getLocaleMonetaryDecimal();    /* Default amount precision is defined by locale */
    if (pFieldPrecision < 0)
                    pFieldPrecision = _zlocale.getLocaleMonetaryDecimal(); /* or precision flag is set but number is negative ??? */

    wUfvalue = abs_val(pFvalue);

    /* We support as much digits as digits available for a double or a long double float (depending on requested format).
     * The only limitation is due to float rules : FLT_MAX is the maximum
     */


    LONG_DOUBLE wFNewValue = pFvalue ;
    ZBool wSubOne=false;
    if (wFNewValue < 0)
            {
            wSignvalue = (_Utf)__MINUS__;
            wFNewValue = - wFNewValue;
            }
        else
        if (pFmtFlags & FMTF_Plus)
                wSignvalue = (_Utf)__PLUS__;
        else if (pFmtFlags & FMTF_SignSpace)
                        wSignvalue = (_Utf)__SPACE__; /* FMTF_Space applies only if FMTF_Plus is not set */
    if (wFNewValue < 1.0)
            {
            wFNewValue += 1.0; /* plus one significant digit to keep leading zeroes if any */
            wSubOne=true;
            }
    wFNewValue = wFNewValue * powl((LONG_DOUBLE)wFBase,(LONG_DOUBLE)pFieldPrecision);
    /* rounding */
    wFNewValue=FPRound<LONG_DOUBLE>(wFNewValue,wRndCoef,GFloat_F_Rnd_GT);
    /* convert to _Utf string (digits in reverse order) */
    utfConvertLDouble<_Utf>( wFNewValue,
                             &wDigitStr,
                             &wAllocatedSlots,
                             &wDigitsNb,
                             __INIT_INTEGER_DIGITS__,
                             pFmtFlags);

    int wIntegerNb= wDigitsNb - pFieldPrecision ;
    if (wIntegerNb<0)
            wIntegerNb=1; /* no negative value for integer part digit number */

    if (pFmtFlags&FMTF_NonSignif) /* if requested to remove non significant zeroes */
        {
        int wi=0,wj=0;
        while ((wi<pFieldPrecision)&&(wDigitStr[wi]==(_Utf)__ZERO__))
                    wi++;
        if (wi)
            {
            while (wi<wDigitsNb) /* remove non significant zeroes */
                {
                wDigitStr[wj++]= wDigitStr[wi++];
                }
            wDigitsNb=wj;
            wDigitStr[wj]=__ENDOFSTRING__;
            }
        }//if (FMTF_NonSignif)



    if ((pFmtFlags & FMTF_Hash)||(pFieldPrecision>0)) /* if FMTF_Hash is set or precision > 0, there is a decimal point */
                            wDecimalPoint= _zlocale.getLocaleDecimalPoint<_Utf> ();

    if (wSignvalue && !(pFmtFlags & FMTF_SignFolw)&&!(pFmtFlags & FMTF_SignPrec)) /* if neither option for sign positionning */
            {
            if (_zlocale.getLocaleSignPosition())                /* see locale */
                        pFmtFlags |= FMTF_SignPrec;
                    else
                        pFmtFlags |= FMTF_SignFolw;
            }

    if (pFmtFlags & FMTF_Currency)
        {
        wCurrency=_zlocale.getLocaleCurrencyCode<_Utf>();
        wCurrencySize=utfStrlen<_Utf>(wCurrency); /* must be replaced with appropriate utfCount routine : canonical character and not char unit count */
        }

    wFieldSize = wDigitsNb;
    wFieldSize += wGroups ;                     /* number of separators if separator is used(otherwise equals 0) */
    wFieldSize +=  wCurrency? wCurrencySize:0;  /* size of currency if it is used */
    wFieldSize += pCurrencySpaces  ;            /* number of spaces between currency code and amount */
    wFieldSize += (wDecimalPoint ? 1 : 0 )  ;   /* decimal point if used */
    wFieldSize += (wSignvalue ? 1 : 0) ;        /* sign sign if used */

    wSlotsSize = wFieldSize >(size_t)pFieldMin ? wFieldSize:pFieldMin;

    wPadlen = wSlotsSize - wFieldSize;

    int wGC=0,wDigitCnt=0,wEffSeparator=0;

    /*  Padding */

    /* zero padding only left padding and only if not space padding */
/*    wZpadlen = pFieldPrecision - wFSlots ;

    if (wZpadlen < 0)
                wZpadlen = 0;*/
    if (wPadlen < 0)
                wPadlen = 0;
    if (pFmtFlags & FMTF_Left)
                    wPadlen = -wPadlen;       /* Left Justifty */

    int wFirstSepPosition=0;
    if (pFmtFlags & FMTF_Group)
        {
        wGroupSeparator=_zlocale.getLocaleGroupSeparator<_Utf>();
        wLocalGroupNumber=_zlocale.getLocaleGroupNumber();
        wGroups =((wDigitsNb-pFieldPrecision) /wLocalGroupNumber) ;// get number of separators to use
        wFirstSepPosition=(wDigitsNb-pFieldPrecision) % wLocalGroupNumber;
        if (!((wDigitsNb-pFieldPrecision) % wLocalGroupNumber))
                                wGroups--;
        }
    if ((wGroups<1)||(!wFirstSepPosition))
                        wFirstSepPosition=-1;
            else
            wPadlen -= wGroups;

    if (wPadlen < 0)
                wPadlen = 0;


    if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))   /* left padding with zeroes ? */
        {
        if (wCurrency && (pFmtFlags & FMTF_CurPreceeds)) /* Currency to be printed and in preceeding position before zero padding */
            {
            long wi=0;
            while(wCurrency[wi])
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wCurrency[wi++]);
            while (pCurrencySpaces--)
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
            wCurrency = 0;  /* switch off currency symbol */
            }
        if (wSignvalue && (pFmtFlags & FMTF_SignPrec)) /* if there is a sign and it should preceed */
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);    /* sign preceeds */
            --wPadlen;
            wSignvalue = 0;
            }
/*
 * Compute grouping factor and first separator position
*/

        while ((pFmtFlags & FMTF_Group)&&(wPadlen>0))
            {
            int wIntegralSlots = wPadlen +wDigitsNb-pFieldPrecision;
            if (wIntegralSlots<1)
                                break;
            int  wSlotsAvail=wIntegralSlots-wLocalGroupNumber;
            while ((wPadlen>0)&&(wSlotsAvail > 0))
            {
                wGroups++;
                wSlotsAvail--;  /* take care of thousand sep inserted */
                wPadlen--;
                wSlotsAvail-=wLocalGroupNumber;
            }
            wFirstSepPosition=(wDigitsNb+wPadlen-pFieldPrecision) % wLocalGroupNumber;
            if (wFirstSepPosition==0)
                        wFirstSepPosition=-1;
            break;
           }//while (pFmtFlags & FMTF_Group)

        while (wPadlen > 0)
            {
            if ((wGroupSeparator))
                        {
                        if (wDigitCnt==wFirstSepPosition)
                            {
                            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wGroupSeparator);
                            wEffSeparator++;
                            wGC=0;
                            wPadlen--;
                            }
                        if(wGC == wLocalGroupNumber)
                            {
                            wGC=0;
                            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wGroupSeparator);
                            wEffSeparator++;
                            wPadlen--;
                            }
//                        wGC++;
                        }
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
            wDigitCnt++;
            --wPadlen;
            }
        }//if ((pFmtFlags & FMTF_ZeroPad) && (wPadlen > 0))
        else
        {   /* No zero padding : pad with space if have to */
        while (wPadlen > 0)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
            --wPadlen;
            }

        if (wCurrency && (pFmtFlags & FMTF_CurPreceeds)) /* Currency to be printed and in preceeding position after left space padding */
            {
            long wi=0;
            while(wCurrency[wi])
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wCurrency[wi++]);
            while (pCurrencySpaces--)
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
            wCurrency = 0;  /* switch off currency symbol */
            }
        if (wSignvalue && (pFmtFlags & FMTF_SignPrec)) /* if there is a sign and it should preceed */
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);    /* sign preceeds */
            wSignvalue = 0;
            }
        }  // else

    /*  Digits are stored in reverse order */
    if (wSubOne) /* and skip artificially added 1 unit if value is a decimal value and set integer part to 0 */
        {
        _Utf wFirstDigit =  wDigitStr[--wDigitsNb]; /* get rid of added 1  to first digit could be 1 or 2 depending on possible rounding */
        wFirstDigit--; /* assuming __ZERO__ comes before __ONE__ */
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen,(_Utf)wFirstDigit);
        wSubOne=false;
        wIntegerNb=1;
        wFirstSepPosition--;
        }


    while (wDigitsNb > 0)
        {
        if (wDigitsNb==pFieldPrecision)  /* when integer part is zero : put the decimal point in place */
                    {
                    if (wDecimalPoint)
                        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDecimalPoint);
                    wDecimalPoint = 0;
                    }
        if (wDigitCnt==wFirstSepPosition)
            {
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wGroupSeparator);
            wEffSeparator++;
            }
        else if ((!pFieldPrecision||wDecimalPoint) && wGroupSeparator)
            {
            if(wGC == wLocalGroupNumber)
                {
                wGC=0;
                utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wGroupSeparator);
                wEffSeparator++;
                }
            }
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDigitStr[--wDigitsNb]);
        wIntegerNb--;
        wDigitCnt++;
        wGC++;
        }

    if (wDecimalPoint)
        {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wDecimalPoint);
        }

/* No zero right padding : has already been taken into account with precision conversion */

/*    while (wZpadlen > 0) {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__ZERO__);
        --wZpadlen;
        }
*/
    if (wSignvalue) /* if there is a sign and if it follows : print it */
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wSignvalue);

    if (wCurrency) /* Currency to be printed in following position (default)- preceeded by spaces if any */
        {
        while (pCurrencySpaces--)
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        long wi=0;
        while(wCurrency[wi])
            utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, wCurrency[wi++]);
        wCurrency = 0;  /* switch off currency symbol */
        }
/* only right space padding wether necessary */
    while (wPadlen < 0) {
        utfSPFOutChar<_Utf>(pBuffer, pCurrlen, pMaxlen, (_Utf)__SPACE__);
        ++wPadlen;
        }

    zfree(wDigitStr);
    zfree(wFconvert);
    return  (*pCurrlen-wLocalSize);
}// utfFormatFPK_1

template <class _ITp>
#ifndef __COMPACT_CODE_
inline
#endif
size_t
utfMainFormatFP (OutEncoding_type pOutOET, void *pBuffer, size_t * pCurrlen, size_t pMaxlen,
                  _ITp pFieldValue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFMTFlags)
{
    char * wBufferChar=nullptr;
    utf8_t*  wBufferUtf8=nullptr;   /* Buffer is utf8 */
    utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */
    switch (pOutOET)
    {
    case OET_Char:
        wBufferChar=static_cast<char*>(pBuffer);
        return utfFormatFPF<char,_ITp>(wBufferChar,pCurrlen,pMaxlen,
                                                   pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF8:
        wBufferUtf8=static_cast<utf8_t*>(pBuffer);
        return utfFormatFPF<utf8_t,_ITp>(wBufferUtf8,pCurrlen,pMaxlen,
                                                     pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF16:
        wBufferUtf16=static_cast<utf16_t*>(pBuffer);
        return utfFormatFPF<utf16_t,_ITp>(wBufferUtf16,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF32:
        wBufferUtf32=static_cast<utf32_t*>(pBuffer);
        return utfFormatFPF<utf32_t,_ITp>(wBufferUtf32,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    }//switch (pOutOET)
    return 0;// to avoid a boring warning
}//utfMainFormatFP


template <class _ITp>
#ifndef __COMPACT_CODE_
inline
#endif
size_t
utfMainFormatFPE (OutEncoding_type pOutOET, void *pBuffer, size_t * pCurrlen, size_t pMaxlen,
                  _ITp pFieldValue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFMTFlags)
{
    char * wBufferChar=nullptr;
    utf8_t*  wBufferUtf8=nullptr;   /* Buffer is utf8 */
    utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */
    switch (pOutOET)
    {
    case OET_Char:
        wBufferChar=static_cast<char*>(pBuffer);
        return utfFormatFPE<char,_ITp>(wBufferChar,pCurrlen,pMaxlen,
                                                   pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF8:
        wBufferUtf8=static_cast<utf8_t*>(pBuffer);
        return utfFormatFPE<utf8_t,_ITp>(wBufferUtf8,pCurrlen,pMaxlen,
                                                     pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF16:
        wBufferUtf16=static_cast<utf16_t*>(pBuffer);
        return utfFormatFPE<utf16_t,_ITp>(wBufferUtf16,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF32:
        wBufferUtf32=static_cast<utf32_t*>(pBuffer);
        return utfFormatFPE<utf32_t,_ITp>(wBufferUtf32,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    }//switch (pOutOET)
    return 0;// to avoid a boring warning
}//utfMainFormatFPS


template <class _ITp>
#ifndef __COMPACT_CODE_
inline
#endif
size_t
utfMainFormatFPA (OutEncoding_type pOutOET, void *pBuffer, size_t * pCurrlen, size_t pMaxlen,
                  _ITp pFieldValue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFMTFlags)
{
    char * wBufferChar=nullptr;
    utf8_t*  wBufferUtf8=nullptr;   /* Buffer is utf8 */
    utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */
    switch (pOutOET)
    {
    case OET_Char:
        wBufferChar=static_cast<char*>(pBuffer);
        return utfFormatFPA<char,_ITp>(wBufferChar,pCurrlen,pMaxlen,
                                                   pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF8:
        wBufferUtf8=static_cast<utf8_t*>(pBuffer);
        return utfFormatFPA<utf8_t,_ITp>(wBufferUtf8,pCurrlen,pMaxlen,
                                                     pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF16:
        wBufferUtf16=static_cast<utf16_t*>(pBuffer);
        return utfFormatFPA<utf16_t,_ITp>(wBufferUtf16,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF32:
        wBufferUtf32=static_cast<utf32_t*>(pBuffer);
        return utfFormatFPA<utf32_t,_ITp>(wBufferUtf32,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    }//switch (pOutOET)
    return 0;// to avoid a boring warning
}//utfMainFormatFPS
template <class _ITp>
#ifndef __COMPACT_CODE_
inline
#endif
size_t
utfMainFormatFPG (OutEncoding_type pOutOET, void *pBuffer, size_t * pCurrlen, size_t pMaxlen,
                  _ITp pFieldValue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFMTFlags)
{
    char * wBufferChar=nullptr;
    utf8_t*  wBufferUtf8=nullptr;   /* Buffer is utf8 */
    utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */
    switch (pOutOET)
    {
    case OET_Char:
        wBufferChar=static_cast<char*>(pBuffer);
        return utfFormatFPG<char,_ITp>(wBufferChar,pCurrlen,pMaxlen,
                                                   pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF8:
        wBufferUtf8=static_cast<utf8_t*>(pBuffer);
        return utfFormatFPG<utf8_t,_ITp>(wBufferUtf8,pCurrlen,pMaxlen,
                                                     pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF16:
        wBufferUtf16=static_cast<utf16_t*>(pBuffer);
        return utfFormatFPG<utf16_t,_ITp>(wBufferUtf16,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    case OET_UTF32:
        wBufferUtf32=static_cast<utf32_t*>(pBuffer);
        return utfFormatFPG<utf32_t,_ITp>(wBufferUtf32,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags);
    }//switch (pOutOET)
    return 0;// to avoid a boring warning
}//utfMainFormatFPG

template <class _Utf, class _FTp>
size_t
utfFormatGeneric(_Utf *pBuffer, size_t * pCurrlen, size_t pMaxlen,
             _FTp pFvalue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFmtFlags,const char* pFormat,size_t pFormatLen)
{


    char wIntermediateBuffer[150];
    char* wFormatChunk=nullptr;

    wFormatChunk=(char*)malloc((pFormatLen+1)*sizeof(char));
    memset(wFormatChunk,0,(pFormatLen+1));

    size_t wi=0;
    while (wi<pFormatLen){
            wFormatChunk[wi]=pFormat[wi];
            wi++;
            }

    size_t          wLocalSize=*pCurrlen,wSprintfSize=0;

    wSprintfSize= ::sprintf(wIntermediateBuffer,wFormatChunk,pFvalue);

    wi=0;
    while (wi<wSprintfSize)
            {
            utfSPFOutChar<_Utf>(pBuffer,pCurrlen,pMaxlen,(_Utf)wIntermediateBuffer[wi]);
            wi++;
            }

    zfree(wFormatChunk);
    return  (*pCurrlen-wLocalSize);
}// utfFormatGeneric

template <class _ITp>
#ifndef __COMPACT_CODE_
inline
#endif
size_t
utfMainFormatGeneric (OutEncoding_type pOutOET, void *pBuffer, size_t * pCurrlen, size_t pMaxlen,
                  _ITp pFieldValue, int pFieldMin, int pFieldPrecision, FMT_Flags_type pFMTFlags,const char* pFormat,size_t pFormalLen)
{
    char * wBufferChar=nullptr;
    utf8_t*  wBufferUtf8=nullptr;   /* Buffer is utf8 */
    utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */
    switch (pOutOET)
    {
    case OET_Char:
        wBufferChar=static_cast<char*>(pBuffer);
        return utfFormatGeneric<char,_ITp>(wBufferChar,pCurrlen,pMaxlen,
                                                   pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags,pFormat,pFormalLen);
    case OET_UTF8:
        wBufferUtf8=static_cast<utf8_t*>(pBuffer);
        return utfFormatGeneric<utf8_t,_ITp>(wBufferUtf8,pCurrlen,pMaxlen,
                                                     pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags,pFormat,pFormalLen);
    case OET_UTF16:
        wBufferUtf16=static_cast<utf16_t*>(pBuffer);
        return utfFormatGeneric<utf16_t,_ITp>(wBufferUtf16,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags,pFormat,pFormalLen);
    case OET_UTF32:
        wBufferUtf32=static_cast<utf32_t*>(pBuffer);
        return utfFormatGeneric<utf32_t,_ITp>(wBufferUtf32,pCurrlen,pMaxlen,
                                                      pFieldValue,pFieldMin,pFieldPrecision,pFMTFlags,pFormat,pFormalLen);
    }//switch (pOutOET)
    return 0;// to avoid a boring warning
}//utfMainFormatFPG

template <class _ITp>
#ifndef __COMPACT_CODE_
inline
#endif
size_t
utfMainFormatFPK (OutEncoding_type pOutOET, void *pBuffer, size_t * pCurrlen, size_t pMaxlen,
                  _ITp pFieldValue, int pFieldMin, int pFieldPrecision, int pCurrencySpace,
                  FMT_Flags_type pFMTFlags)
{
    char * wBufferChar=nullptr;
    utf8_t*  wBufferUtf8=nullptr;   /* Buffer is utf8 */
    utf16_t *wBufferUtf16=nullptr;  /* Buffer is utf16 */
    utf32_t *wBufferUtf32=nullptr;  /* Buffer is utf32 */
    switch (pOutOET)
    {
    case OET_Char:
        wBufferChar=static_cast<char*>(pBuffer);
        return utfFormatFPK<char,_ITp>(wBufferChar,pCurrlen,pMaxlen,
                                       pFieldValue,pFieldMin,pFieldPrecision,pCurrencySpace,
                                       pFMTFlags);
    case OET_UTF8:
        wBufferUtf8=static_cast<utf8_t*>(pBuffer);
        return utfFormatFPK<utf8_t,_ITp>(wBufferUtf8,pCurrlen,pMaxlen,
                                         pFieldValue,pFieldMin,pFieldPrecision,pCurrencySpace,
                                         pFMTFlags);
    case OET_UTF16:
        wBufferUtf16=static_cast<utf16_t*>(pBuffer);
        return utfFormatFPK<utf16_t,_ITp>(wBufferUtf16,pCurrlen,pMaxlen,
                                          pFieldValue,pFieldMin,pFieldPrecision,pCurrencySpace,
                                          pFMTFlags);
    case OET_UTF32:
        wBufferUtf32=static_cast<utf32_t*>(pBuffer);
        return utfFormatFPK<utf32_t,_ITp>(wBufferUtf32,pCurrlen,pMaxlen,
                                          pFieldValue,pFieldMin,pFieldPrecision,pCurrencySpace,
                                          pFMTFlags);
    }//switch (pOutOET)
    return 0;// to avoid a boring warning
}//utfMainFormatFPK
template <class _Utf>
inline
void
utfSPFOutChar(_Utf *pBuffer, size_t * pCurrlen, size_t pMaxlen, _Utf pCh)
{
/* if pBuffer is null : just increment current length- no check with pMaxlen is done
 *
 * if offset greater than maximum allowed : do not put out but continue incrementing offset.
 *
 * Standard says sprintf returns full size but only buffer maximum size is written
*/
    if (pBuffer&&(*pCurrlen < pMaxlen))
            {
            pBuffer[*pCurrlen] = pCh;
            }
        else
            errno=ENOMEM;
    (*pCurrlen)++;
    return;
}//utfDopr_outch

template <class _UtfOut, class _UtfIn>
inline
void
utfSPFOutUtf(_UtfOut *pBuffer, size_t * pCurrlen, size_t pMaxlen, _UtfIn pCh)
{
    /* if pBuffer is null : just increment current length- no check with pMaxlen is done
     *
     * if offset greater than maximum allowed : do not put out but continue incrementing offset.
     *
     * Standard says sprintf returns full size but only buffer maximum size is written
    */
    if (pBuffer&&(*pCurrlen < pMaxlen))
            {
            pBuffer[*pCurrlen] =(_UtfOut) pCh;
            }
    (*pCurrlen)++;
    return;
}//utfDopr_outch


#ifdef TEST_SNPRINTF
#ifndef LONG_STRING
#define LONG_STRING cst_MaxSprintfBufferSize
#endif
int
main(void)
{
    char            buf1[LONG_STRING];
    char            buf2[LONG_STRING];
    char           *fp_fmt[] = {
        "%-1.5f",
        "%1.5f",
        "%123.9f",
        "%10.5f",
        "% 10.5f",
        "%+22.9f",
        "%+4.9f",
        "%01.3f",
        "%4f",
        "%3.1f",
        "%3.2f",
        "%.0f",
        "%.1f",
        NULL
    };
    double          fp_nums[] =
        { -1.5, 134.21, 91340.2, 341.1234, 0203.9, 0.96, 0.996,
        0.9996, 1.996, 4.136, 0
    };
    char           *int_fmt[] = {
        "%-1.5d",
        "%1.5d",
        "%123.9d",
        "%5.5d",
        "%10.5d",
        "% 10.5d",
        "%+22.33d",
        "%01.3d",
        "%4d",
        NULL
    };
    long            int_nums[] = { -1, 134, 91340, 341, 0203, 0 };
    int             x, y;
    int             fail = 0;
    int             num = 0;

    printf("Testing snprintf format codes against system sprintf...\n");

    for (x = 0; fp_fmt[x] != NULL; x++)
        for (y = 0; fp_nums[y] != 0; y++) {
            snprintf(buf1, sizeof(buf1), fp_fmt[x], fp_nums[y]);
            sprintf(buf2, fp_fmt[x], fp_nums[y]);
            if (strcmp(buf1, buf2)) {
                printf
                    ("snprintf doesn't match Format: %s\n\tsnprintf = %s\n\tsprintf  = %s\n",
                     fp_fmt[x], buf1, buf2);
                fail++;
            }
            num++;
        }

    for (x = 0; int_fmt[x] != NULL; x++)
        for (y = 0; int_nums[y] != 0; y++) {
            snprintf(buf1, sizeof(buf1), int_fmt[x], int_nums[y]);
            sprintf(buf2, int_fmt[x], int_nums[y]);
            if (strcmp(buf1, buf2)) {
                printf
                    ("snprintf doesn't match Format: %s\n\tsnprintf = %s\n\tsprintf  = %s\n",
                     int_fmt[x], buf1, buf2);
                fail++;
            }
            num++;
        }
    printf("%d tests failed out of %d.\n", fail, num);
}
#endif                          /* SNPRINTF_TEST */

#include <ztoolset/utfstrctx.h>


/**
 * @brief utfEscapeCodepoint  Escapes code point value to an output _Utf string (pOutPtr) according a ZCNV_Action instruction.
 *
 *  Legend hhhhhhhh means hexadecimal value on 8 alpha digits (0-9 and a-f)
 *
 * ZCNV_Escape_C : c standard : \Uhhhhhhhh  utf32 char \uhhhh  utf16 char and \u8hh utf8 char
 * ZCNV_Escape_Java:
 *
 * @param pOutPtr
 * @param pCodePoint
 * @param pRemaining
 * @param pAction
 * @param pEscapedSize
 * @return
 */
template <class _Utf>
UST_Status_type
utfEscapeCodepoint(_Utf** pOutPtr,
                   utf32_t pCodePoint,
                   ssize_t& pRemaining,
                   ZCNV_Action pAction,
                   size_t& pEscapedSize)
{
size_t wCurlen=0;
    _Utf wEscape[20];
    _Utf* wPtr=(_Utf*)wEscape;

    for (size_t wi=0;wi<sizeof(wEscape);wi++)
                    wEscape[wi]=0;

    switch (pAction)
    {
    case (ZCNV_Escape_C):
        *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
        wPtr++;
        *wPtr = (_Utf)UNICODE_U_CODEPOINT;/* add <U> : because input is utf32 */
        wPtr++;
        wCurlen=0;

        utfFormatInteger<_Utf,utf32_t>(wPtr,
                           &wCurlen,
                           10,          // max length
                           pCodePoint,  // value
                           10,          // minimum size to pad
                           -1,           // no max size : only effective consumed digits
                           FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
        /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
           corresponds to    utfSnprintf<_Utf>(wPtr,(size_t)10,"%08X",pCodePoint); */
        break;
    case (ZCNV_Escape_Java):
        *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
        wPtr++;
        *wPtr = (_Utf)UNICODE_U_CODEPOINT;/* add <U> :for utf32 (<u> for utf16 and <u8> for utf8)*/
        wPtr++;
        wCurlen=0;
        wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                  &wCurlen,     // current offset set to 0
                                  (size_t)pRemaining, // max length
                                  pCodePoint, // value
                                  8,       // min digits to print out
                                  -1,       // max digits (don't care)
                                  FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
        /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
           corresponds to    utfSnprintf<_Utf>(wPtr,(size_t)10,"%08X",pCodePoint); */
        break;
    case (ZCNV_Escape_ICU):
        *wPtr = (_Utf)UNICODE_PERCENT_SIGN_CODEPOINT;/* add <%>  */
        wPtr++;
        *wPtr = (_Utf)UNICODE_U_LOW_CODEPOINT;/* add <U>   */
        wPtr++;
        wCurlen=0;
        wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                  &wCurlen,     // current offset set to 0
                                  (size_t)pRemaining, // max length
                                  pCodePoint, // value
                                  8,       // min digits to print out
                                  -1,       // max digits (don't care)
                                  FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
        /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
           corresponds to    utfSnprintf<_Utf>(wPtr,(size_t)10,"%08X",pCodePoint); */
        break;
    case (ZCNV_Escape_XML_Dec):
        *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
        wPtr++;
        *wPtr = (_Utf)UNICODE_U_LOW_CODEPOINT;/* add <u> : because input is utf32 */
        wPtr++;
//        utfSnprintf<_Utf>(wPtr,11,"%08X;",pCodePoint);/* add codepoint hex value leading zeroes*/
        wCurlen=0;
        wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                  &wCurlen,     // current offset set to 0
                                  (size_t)pRemaining, // max length
                                  pCodePoint, // value
                                  1,       // no min digits to print out
                                  -1,       // max digits (don't care)
                                  FMTF_Unsigned  );
        /* Flags are:  decimal no sign no padding */
        break;
    case (ZCNV_Escape_XML_Hex):
        *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
        wPtr++;
        *wPtr = (_Utf)UNICODE_U_LOW_CODEPOINT;/* add <u> : because input is utf32 */
        wPtr++;
        wCurlen=0;
        wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                  &wCurlen,     // current offset set to 0
                                  (size_t)pRemaining, // max length
                                  pCodePoint, // value
                                  8,       // min digits to print out
                                  -1,       // max digits (don't care)
                                  FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
        /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
           corresponds to    utfSnprintf<_Utf>(wPtr,(size_t)10,"%08X",pCodePoint); */
        break;
    case (ZCNV_Escape_Unicode):
        *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <{>  */
        wPtr++;
        *wPtr = (_Utf)UNICODE_U_CODEPOINT;/* add <U> : because input is utf32 */
        wPtr++;
        *wPtr = (_Utf)UNICODE_PLUS_CODEPOINT;/* add <+> : because input is utf32 */
        wPtr++;
        wCurlen=0;
        wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                  &wCurlen,     // current offset set to 0
                                  (size_t)pRemaining, // max length
                                  pCodePoint, // value
                                  8,       // min digits to print out
                                  -1,       // max digits (don't care)
                                  FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
        /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
           corresponds to    utfSnprintf<_Utf>(wPtr,(size_t)10,"%08X",pCodePoint); */
        break;
    case (ZCNV_Escape_CSS2):

        *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
        wPtr++;
        wCurlen=0;
        wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                  &wCurlen,     // current offset set to 0
                                  (size_t)pRemaining, // max length
                                  pCodePoint, // value
                                  8,       // min digits to print out
                                  -1,       // max digits (don't care)
                                  FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
        /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
           corresponds to    utfSnprintf<_Utf>(wPtr,(size_t)10,"%08X",pCodePoint); */
        break;


    default:  // must not be the case anyway
        {
        return UST_SEVERE;
        }
    }// switch

    pEscapedSize = utfStrlen<_Utf>((const _Utf*)wEscape);
    if ((pRemaining) <= (pEscapedSize))
                                        return UST_TRUNCATED;
    for (size_t wi=0;wi<(pEscapedSize);wi++)
                            (*pOutPtr)[wi]=wEscape[wi];
    (pRemaining) -= (pEscapedSize);
    return UST_SUCCESS;
}


/**
 * @brief utfEscapeUtf8 Escapes an utf8 character pInUtf8 of pInSize units length to escape mode given by ZCNV_Action
 * and writes it to an _Utf* string (pOutPtr).<br>
 * Remaining size is checked to be enough for escaped character to fit into out string.
 * If size is not enough for escaped character(s) to fit into output string,
 * then nothing is written out and UST_TRUNCATED is returned.<br>
 * pEscapedSize contains necessary available size in _Utf character units for escaped character to be written.
 *
 * @param pOutPtr pointer to output string of type _Utf. Pointer is updated to next position.
 * @param pInUtf8       Input character to escape (pointer to utf8_t first character unit of the character).
 * @param pInSize       Size of character to escape in utf8_t character units
 * @param pRemaining    Remaining size in character units within pOutPtr string
 * @param pAction       ZCNV_Action mentionning the escape method to apply
 * @param pEscapedSize  size in _Utf character units necessary(if UST_TRUNCATED) or used (if successfull) from pOutPtr
 * @return an UST_Status_type with an utfStatus_type value :
 * - <b>UST_SUCCESS</b> escaped character has been written to pOutPtr.<br>
 *  Size of written data is returned within pEscapedSize.<br>
 * - <b>UST_TRUNCATED</b> pRemaining is not large enough to make escaped character been written to pOutStr.\br>
 *  Required size is returned into pEscapedSize.
 */
template <class _Utf>
UST_Status_type
utfEscapeUtf8(_Utf* pOutPtr,
              const utf8_t* pInUtf8,
              size_t pInSize,
              ssize_t& pRemaining,
              ZCNV_Action pAction,
              size_t& pEscapedSize)
{
size_t wCurlen=0;
    _Utf wEscape[50];
    _Utf* wPtr=(_Utf*)wEscape;

    for (size_t wi=0;wi<sizeof(wEscape);wi++)
                    wEscape[wi]=0;

    switch (pAction)
    {
    case (ZCNV_Escape_C):  /* C universal : utf16 \uhhhh  or utf32 \Uhhhhhhhh   */
        for (size_t wi=0;wi<pInSize;wi++) // use universal C    \uxxxx\uxxxx...   representation mode
            {
            *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_LOW_CODEPOINT;/* add <u> : because input is utf8 */
            wPtr++;
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf8[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            }
        break;
    case (ZCNV_Escape_Java):  /* Java mode \uhhhh */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_LOW_CODEPOINT;/* add <u> */
            wPtr++;
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf8[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            }
        break;
    case (ZCNV_Escape_ICU):  /* icu way of escaping characters : \Uhhhh   */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_PERCENT_SIGN_CODEPOINT;/* add <%>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_CODEPOINT;/* add <U> */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]);/* add hex value leading zeroes by utf8 character*/
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf8[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            }
        break;
    case (ZCNV_Escape_XML_Dec): /* Xml decimal notation &#dddd; */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_AMP_CODEPOINT;/* add <&>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_HASH_CODEPOINT;/* add <#> */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%d",(int)pInUtf8[wi]);/* add decimal value */
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,int>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (int)pInUtf8[wi], // value
                                      1,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to   wPtr+=utfSnprintf<_Utf>(wPtr,10,"%d",(int)pInUtf8[wi]);*/

            *wPtr = (_Utf)UNICODE_SEMICOLON_CODEPOINT;/* add <;> */
            wPtr++;
            }
        break;
    case (ZCNV_Escape_XML_Hex): /* xml hexadecimal notation &#xhhhh;  */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_AMP_CODEPOINT;/* add <&>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_HASH_CODEPOINT;/* add <#> */
            wPtr++;
            *wPtr = (_Utf)UNICODE_X_CODEPOINT;/* add <x> */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]);/* add hex value leading zeroes */
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf8[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max slots (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            *wPtr = (_Utf)UNICODE_SEMICOLON_CODEPOINT;/* add <;> */
            wPtr++;
            }
        break;
    case (ZCNV_Escape_Unicode): /*  Unicode notation {U+hhhh}   */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_LEFT_CURLY_CODEPOINT;/* add <{>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_CODEPOINT;/* add <U>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_PLUS_CODEPOINT;/* add <+>  */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]);/* add hex value leading zeroes */
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf8[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            *wPtr = (_Utf)UNICODE_RIGHT_CURLY_CODEPOINT;/* add <}> */
            wPtr++;
            }
        break;
    case (ZCNV_Escape_CSS2): /*  \hhhh */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]);/* add hex value leading zeroes */
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf8[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            }
        break;
    default:  // must not be the case anyway
        {
        return UST_SEVERE;
        }
    }// switch

    pEscapedSize = utfStrlen<_Utf>((_Utf*)wEscape);
    if (pRemaining <= pEscapedSize)
                                        return UST_TRUNCATED;
    for (size_t wi=0;wi< pEscapedSize;wi++)
                            pOutPtr[wi]=wEscape[wi];
    pRemaining -= pEscapedSize;
    return UST_SUCCESS;
}//utfEscapeUtf8


/**
 * @brief utfEscapeUtf16 Escapes an utf16 character pInUtf16 of pInSize units length to escape mode given by ZCNV_Action
 * and writes it to an _Utf* string (pOutPtr).<br>
 * Remaining size is checked to be enough for escaped character to fit into out string.
 * If size is not enough for escaped character(s) to fit into output string,
 * then nothing is written out and UST_TRUNCATED is returned.<br>
 * pEscapedSize contains necessary available size in _Utf character units for escaped character to be written.
 *
 * @param pOutPtr pointer to output string of type _Utf. Pointer is updated to next position.
 * @param pInUtf16      Input character to escape (pointer to utf16_t first character unit of the character).
 * @param pInSize       Size of character to escape in utf16_t character units
 * @param pRemaining    Remaining size in character units within pOutPtr string
 * @param pAction       ZCNV_Action mentionning the escape method to apply
 * @param pEscapedSize  size in _Utf character units necessary(if UST_TRUNCATED) or used (if successfull) from pOutPtr
 * @return an UST_Status_type with an utfStatus_type value :
 * - <b>UST_SUCCESS</b> escaped character has been written to pOutPtr.<br>
 *  Size of written data is returned within pEscapedSize.<br>
 * - <b>UST_TRUNCATED</b> pRemaining is not large enough to make escaped character been written to pOutStr.\br>
 *  Required size is returned into pEscapedSize.
 */
template <class _Utf>
UST_Status_type
utfEscapeUtf16(_Utf* pOutPtr,
              const utf16_t* pInUtf16,
              size_t pInSize,
              ssize_t& pRemaining,
              ZCNV_Action pAction,
              size_t& pEscapedSize)
{
size_t wCurlen=0;
    _Utf wEscape[50];
    _Utf* wPtr=(_Utf*)wEscape;

    for (size_t wi=0;wi<sizeof(wEscape);wi++)
                    wEscape[wi]=0;

    switch (pAction)
    {
    case (ZCNV_Escape_C):  /* C universal : utf16 \uhhhh  or utf32 \Uhhhhhhhh   */
        for (size_t wi=0;wi<pInSize;wi++) // use universal C    \uxxxx\uxxxx...   representation mode
            {
            *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_LOW_CODEPOINT;/* add <u> : because input is utf8 */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf16[wi]);/* add hex value leading zeroes by utf8 character*/
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf16[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            }
        break;
    case (ZCNV_Escape_Java):  /* Java mode \uhhhh */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_LOW_CODEPOINT;/* add <u> */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf16[wi]);/* add hex value leading zeroes by utf8 character*/
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf16[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            }
        break;
    case (ZCNV_Escape_ICU):  /* icu way of escaping characters : \Uhhhh   */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_PERCENT_SIGN_CODEPOINT;/* add <\>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_CODEPOINT;/* add <U> */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf16[wi]);/* add hex value leading zeroes by utf8 character*/
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf16[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            }
        break;
    case (ZCNV_Escape_XML_Dec): /* Xml decimal notation &#dddd; */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_AMP_CODEPOINT;/* add <&>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_HASH_CODEPOINT;/* add <#> */
            wPtr++;
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf16[wi], // value
                                      1,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int );
            /* Flags are:  no flag : decimal
               corresponds to     wPtr+=utfSnprintf<_Utf>(wPtr,10,"%d",(int)pInUtf16[wi]); */

            *wPtr = (_Utf)UNICODE_SEMICOLON_CODEPOINT;/* add <;> */
            wPtr++;
            }
        break;
    case (ZCNV_Escape_XML_Hex): /* xml hexadecimal notation &#xhhhh;  */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_AMP_CODEPOINT;/* add <&>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_HASH_CODEPOINT;/* add <#> */
            wPtr++;
            *wPtr = (_Utf)UNICODE_X_CODEPOINT;/* add <x> */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf16[wi]);/* add hex value leading zeroes */
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf16[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            *wPtr = (_Utf)UNICODE_SEMICOLON_CODEPOINT;/* add <;> */
            wPtr++;
            }
        break;
    case (ZCNV_Escape_Unicode): /*  Unicode notation {U+hhhh}   */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_LEFT_CURLY_CODEPOINT;/* add <{>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_CODEPOINT;/* add <U> : because input is utf32 */
            wPtr++;
            *wPtr = (_Utf)UNICODE_PLUS_CODEPOINT;/* add <+> : because input is utf32 */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf16[wi]);/* add hex value leading zeroes */
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf16[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            *wPtr = (_Utf)UNICODE_RIGHT_CURLY_CODEPOINT;/* add <}> */
            wPtr++;
            }
        break;
    case (ZCNV_Escape_CSS2): /*  \hhhh */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
            wPtr++;
//            wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf16[wi]);/* add hex value leading zeroes */
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf16_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      (uint16_t)pInUtf16[wi], // value
                                      4,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    wPtr+=utfSnprintf<_Utf>(wPtr,10,"%04X",(uint16_t)pInUtf8[wi]); */
            }
        break;
    default:  // must not be the case anyway
        {
        return UST_SEVERE;
        }
    }// switch

    pEscapedSize = utfStrlen<_Utf>((_Utf*)wEscape);
    if (pRemaining <= pEscapedSize)
                                        return UST_TRUNCATED;
    for (size_t wi=0;wi< pEscapedSize;wi++)
                            pOutPtr[wi]=wEscape[wi];
    pRemaining -= pEscapedSize;
    return UST_SUCCESS;
}//utfEscapeUtf16
/**
 * @brief utfEscapeUtf32 Escapes an utf32 character pInUtf32 of pInSize units length to escape mode given by ZCNV_Action
 * and writes it to an _Utf* string (pOutPtr).<br>
 * Remaining size is checked to be enough for escaped character to fit into out string.
 * If size is not enough for escaped character(s) to fit into output string,
 * then nothing is written out and UST_TRUNCATED is returned.<br>
 * pEscapedSize contains necessary available size in _Utf character units for escaped character to be written.
 *
 * @param pOutPtr pointer to output string of type _Utf. Pointer is updated to next position.
 * @param pInUtf32      Input character to escape (pointer to utf32_t first (and only) character unit of the character).
 * @param pInSize       Size of character to escape in utf32_t character units : always 1
 * @param pRemaining    Remaining size in character units within pOutPtr string
 * @param pAction       ZCNV_Action mentionning the escape method to apply
 * @param pEscapedSize  size in _Utf character units necessary(if UST_TRUNCATED) or used (if successfull) from pOutPtr
 * @return an UST_Status_type with an utfStatus_type value :
 * - <b>UST_SUCCESS</b> escaped character has been written to pOutPtr.<br>
 *  Size of written data is returned within pEscapedSize.<br>
 * - <b>UST_TRUNCATED</b> pRemaining is not large enough to make escaped character been written to pOutStr.\br>
 *  Required size is returned into pEscapedSize.
 */
template <class _Utf>
UST_Status_type
utfEscapeUtf32(_Utf* pOutPtr,
              const utf32_t* pInUtf32,
              size_t pInSize,  // always 1
              ssize_t& pRemaining,
              ZCNV_Action pAction,
              size_t& pEscapedSize)
{
size_t wCurlen=0;
    _Utf wEscape[50];
    _Utf* wPtr=(_Utf*)wEscape;
//    size_t wOLength;

    for (size_t wi=0;wi<sizeof(wEscape);wi++)
                    wEscape[wi]=0;

    switch (pAction)
    {
    case (ZCNV_Escape_C):  /* C universal : utf16 \uhhhh  or utf32 \Uhhhhhhhh   */

            *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_LOW_CODEPOINT;/* add <u> : because input is utf8 */
            wPtr++;
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      pInUtf32[0], // value
                                      8,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    utfSnprintf<_Utf>(wPtr,10,"%08X",(unsigned long)pInUtf32[0]);*/
        break;
    case (ZCNV_Escape_Java):  /* Java mode \uhhhh */

            *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_LOW_CODEPOINT;/* add <u> */
            wPtr++;
//            utfSnprintf<_Utf>(wPtr,10,"%08X",(unsigned long)pInUtf32[0]);/* add hex value leading zeroes by utf8 character*/
//            wPtr+=8;
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      pInUtf32[0], // value
                                      8,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    utfSnprintf<_Utf>(wPtr,10,"%08X",(unsigned long)pInUtf32[0]);*/
        break;
    case (ZCNV_Escape_ICU):  /* icu way of escaping characters : \Uhhhh   */
            *wPtr = (_Utf)UNICODE_PERCENT_SIGN_CODEPOINT;/* add <\>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_CODEPOINT;/* add <U> */
            wPtr++;
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      pInUtf32[0], // value
                                      8,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    utfSnprintf<_Utf>(wPtr,10,"%08X",(unsigned long)pInUtf32[0]);*/
        break;
    case (ZCNV_Escape_XML_Dec): /* Xml decimal notation &#dddd; */

            *wPtr = (_Utf)UNICODE_AMP_CODEPOINT;/* add <&>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_HASH_CODEPOINT;/* add <#> */
            wPtr++;
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      pInUtf32[0], // value
                                      1,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to     wPtr+=utfSnprintf<_Utf>(wPtr,10,"%d",(unsigned long)pInUtf32[0]);*/
            *wPtr = (_Utf)UNICODE_SEMICOLON_CODEPOINT;/* add <;> */
            wPtr++;
        break;
    case (ZCNV_Escape_XML_Hex): /* xml hexadecimal notation &#xhhhh;  */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_AMP_CODEPOINT;/* add <&>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_HASH_CODEPOINT;/* add <#> */
            wPtr++;
            *wPtr = (_Utf)UNICODE_X_CODEPOINT;/* add <x> */
            wPtr++;
//            utfSnprintf<_Utf>(wPtr,10,"%08X",(unsigned long)pInUtf32[0]);/* add hex value leading zeroes */
//            wPtr+=8;
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      pInUtf32[0], // value
                                      8,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    utfSnprintf<_Utf>(wPtr,10,"%08X",(unsigned long)pInUtf32[0]);*/
            *wPtr = (_Utf)UNICODE_SEMICOLON_CODEPOINT;/* add <;> */
            wPtr++;
            }
        break;
    case (ZCNV_Escape_Unicode): /*  Unicode notation {U+hhhh}   */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_LEFT_CURLY_CODEPOINT;/* add <{>  */
            wPtr++;
            *wPtr = (_Utf)UNICODE_U_CODEPOINT;/* add <U> : because input is utf32 */
            wPtr++;
            *wPtr = (_Utf)UNICODE_PLUS_CODEPOINT;/* add <+> : because input is utf32 */
            wPtr++;
//            utfSnprintf<_Utf>(wPtr,10,"%08X",(unsigned long)pInUtf32[0]);/* add hex value leading zeroes */
//            wPtr+=8;
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      pInUtf32[0], // value
                                      8,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    utfSnprintf<_Utf>(wPtr,10,"%08X",(unsigned long)pInUtf32[0]);*/
            *wPtr = (_Utf)UNICODE_RIGHT_CURLY_CODEPOINT;/* add <}> */
            wPtr++;
            }
        break;
    case (ZCNV_Escape_CSS2): /*  \hhhh */
        for (size_t wi=0;wi<pInSize;wi++)
            {
            *wPtr = (_Utf)UNICODE_RS_CODEPOINT;/* add <\>  */
            wPtr++;
//            utfSnprintf<_Utf>(wPtr,10,"%08X",(unsigned long)pInUtf32[0]);/* add hex value leading zeroes */
//            wPtr+=8;
            wCurlen=0;
            wPtr+= utfFormatInteger<_Utf,utf32_t>(wPtr,         // pointer
                                      &wCurlen,     // current offset set to 0
                                      (size_t)pRemaining, // max length
                                      pInUtf32[0], // value
                                      8,        // min digits to print out
                                      -1,       // max digits (don't care)
                                      FMTF_Int | FMTF_Uppercase | FMTF_Unsigned | FMTF_ZeroPad | FMTF_Hexa);
            /* Flags are:  uppercase hexdecimal unsigned and padded with zeroes
               corresponds to    utfSnprintf<_Utf>(wPtr,10,"%08X",(unsigned long)pInUtf32[0]);*/
            }
        break;
    default:  // must not be the case anyway
        {
        return UST_SEVERE;
        }
    }// switch

    pEscapedSize = utfStrlen<_Utf>((_Utf*)wEscape);
    if (pRemaining <= pEscapedSize)
                                        return UST_TRUNCATED;
    for (size_t wi=0;wi< pEscapedSize;wi++)
                            pOutPtr[wi]=wEscape[wi];
    pRemaining -= (ssize_t)pEscapedSize;
    return UST_SUCCESS;
}//utfEscapeUtf32




/**
 * @brief utfFFToUtf    Allocates an _Utf string and converts a long double to _Utf string using format 'f' rules and a given precision.<br>
 *  _Utf string is allocated by routine and ends  with _Utf __END_OF_STRING__ mark.<br>
 * Returned string must be freed by calling routine. @see _free() <br>
 * Template parameter: utf8_t, utf16_t or utf32_t<br>
 *
 * @param pFloat        long double value to convert
 * @param pPrecision    number of digit after decimal point to consider
 * @return an _Utf string with formatted value. This string is allocated by routine and must be freed by calling routine.
 */
template <class _Utf>
_Utf* utfFFToUtf(LONG_DOUBLE pFloat,int pPrecision)
{
    _Utf* wFloatStr=nullptr;

    size_t wSize=25;
    wSize=utfFormatFPG<_Utf>(nullptr,&wSize,25,pFloat,0,pPrecision,FMTF_NonSignif|FMTF_Long|FMTF_Float);

    wFloatStr=(_Utf*)calloc(wSize+1,sizeof(_Utf));
    utfFormatFPG<_Utf>(wFloatStr,&wSize,25,pFloat,0,pPrecision,FMTF_NonSignif|FMTF_Long|FMTF_Float);
    return wFloatStr;
}

/**
 * @brief utfFEToUtf    Allocates an _Utf string and converts a long double to _Utf string using format 'e' rules and a given precision.<br>
 *  _Utf string is allocated by routine and ends  with _Utf __END_OF_STRING__ mark.<br>
 * Returned string must be freed by calling routine. @see _free() <br>
 * Template parameter: utf8_t, utf16_t or utf32_t<br>
 *
 * @param pFloat        long double value to convert
 * @param pPrecision    number of digit after decimal point to consider
 * @return an _Utf string with formatted value. This string is allocated by routine and must be freed by calling routine.
 */
template <class _Utf>
_Utf* utfFEToUtf(LONG_DOUBLE pFloat,int pPrecision)
{
    _Utf* wFloatStr=nullptr;

    size_t wSize=25;
    wSize=utfFormatFPG<_Utf>(nullptr,&wSize,25,pFloat,0,pPrecision,FMTF_Exponent|FMTF_Long|FMTF_Float);

    wFloatStr=(_Utf*)calloc(wSize+1,sizeof(_Utf));
    utfFormatFPG<_Utf>(wFloatStr,&wSize,25,pFloat,0,pPrecision,FMTF_Exponent|FMTF_Long|FMTF_Float);
    return wFloatStr;
}



#endif // UTFSPRINTF_H
