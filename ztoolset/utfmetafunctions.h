#ifndef UTFMETAFUNCTIONS_H
#define UTFMETAFUNCTIONS_H

#include "utfvaryingstring.h"
#include "zarray.h"
#include "cmatchresult.h"

namespace zbs {
/*
utf8VaryingString
searchKeywordWeighted(const utf8VaryingString& pIn);
utf8VaryingString
searchSymbolWeighted(const utf8VaryingString& pIn);

utf8VaryingString
searchSymbolApprox(const utf8VaryingString& pIn);
utf8VaryingString
searchKeywordApprox(const utf8VaryingString& pIn);
*/

/**
 * @brief searchAdHocWeighted  searches pIn for a matching keyword within pKeyArray using matchWeight algorithm.
 * @param pIn string to search
 * @param pKeyArray keyword array
 * @param pMatchMin requested minimum number of character (consecutive or not) to be matched.
 * If omitted (negative value : -1 is the default value) then 40 percent of character should match to be retained.
 * @param pRadixMin  requested minimum size of common radix to be found, i. e. number of consecutive, similar characters regardless position in both strings.
 * Default value is 2.
 * If omitted (-1 or negative value) radix size is set to 30 percent of input string size and has to be found in key strings to be retained.
 * @return an utf8VaryingString :
 *  empty if not found
 *  containing highest matching value keyword if found
 */
/*utf8VaryingString searchAdHocWeighted(const utf8VaryingString &pIn,
                                      ZArray<utf8VaryingString> &pKeyArray);
*/
utf8VaryingString searchAdHocWeighted(const utf8VaryingString &pIn,
                                      ZArray<utf8VaryingString> &pKeyArray,
                                      int pMatchMin = -1,        /* minimum number of characters to match */
                                      int pRadixMin = 2);        /* minimum size of radix found */

/**
 * @brief matchWeight searches pIn and pToMatch for a minimum number of character to match pMatchMin and for a common radix of minimum size pRadixMin
 * whereever the radix is located within the input string or the match string.
 * Radix is a sequence of consecutive characters of pRadixMin minimum size.
 * Other matching characters may be non-consecutive and in a wrong order.
 * Returns
 * - false if no radix is found
 * - true when at least radix of minimum size is found
 * - Returns a CMatchResult structure pResult.
 *      CMatchResult gives :
 *          - size of any radix found
 *          - number of matching characters even not consecutive or not in order (avoiding typing mistakes)
 *
 * @param pResult
 * @param pIn
 * @param pToMatch
 * @param pMatchMin
 * @param pRadixMin
 * @return
 */
bool matchWeight(CMatchResult &pResult,
                 const utf8VaryingString &pIn,
                 const utf8VaryingString &pToMatch,
                 int pMatchMin = -1,        /* minimum number of characters to match */
                 int pRadixMin = 2);        /* minimum size of radix to be found */

/**
 * @brief leftPad pads left (leading) pString (utf8VaryingString) with pPadChar on pSize number of char
 * @param pString
 * @param pSize
 * @param pPadChar
 * @return
 */
utf8VaryingString
leftPad(const utf8VaryingString& pString,int pSize,utf8_t pPadChar=' ');

/**
 * @brief rightPad pads right (trailing) pString (utf8VaryingString) with pPadChar on pSize number of char
 * @param pString
 * @param pSize
 * @param pPadChar
 * @return
 */
utf8VaryingString
rightPad(const utf8VaryingString& pString,int pSize,utf8_t pPadChar);

} // namespace zbs

#endif // UTFMETAFUNCTIONS_H
