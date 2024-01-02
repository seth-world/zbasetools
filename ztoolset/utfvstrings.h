#ifndef UTFVTRINGS_H
#define UTFVTRINGS_H
/**  @file utfvstrings.h In this file is defined a template FIXED string container that is used as base for various sizes of strings.
 *  some further derived structs might have additional functions (like uriString, for manipulating files and directories)
 *
 * This data may be directly moved as a field for a record.
 *
 * As we are using a template,
 * - detailed methods for the template have to be in the header file,
 * - all derived class definitions have to be in the same header file.
 *
 * templateString Template is hidden below derived classes definition.
 * Each fixed string below has a constant that corresponds to it representing maximum size of its data (plus one).
 *
 * - templateString : native template
 * - codeString : fixed string for code management  - cst_codelen
 * - descString : fixed string for various descriptions - cst_desclen
 * - uriString : fixed string for files and directories pathes management - cst_urilen
 * - messageString : for message - cst_messagelen
 * - exceptionString : exception message - cst_exceptionlen
 * - identityString : for identity and authentication - cst_identitylen
 * - keywordString : for managing keywords - cst_keywordlen
 *
 *
 * This file is divided in Sections.
 *
 */

#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/uristring.h>
/** @brief The utf8VaryingString class Alias to utf8VaryingString : ZString object unicode utf8 encoded with varying length. */
typedef utf8VaryingString utf8VaryingString;
typedef utf16VaryingString utf16String;
typedef utf32VaryingString utf32String;

#endif // UTFVSTRINGS_H
