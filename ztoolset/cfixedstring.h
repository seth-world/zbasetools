#ifndef UTFFIXEDSTRING_H
#define UTFFIXEDSTRING_H
/** utffixedstring.h
 *  this file contains class definitions  AND methods of various derivations for utfTemplateString template.
 *
 *  utf8FixedString     Template for utf8_t based fixed string : container in character unit size is template parameter i. e. number of utf8_t
 *  utf16FixedString    Template for utf16_t based fixed string : container in character unit size is template parameter i. e. number of utf16_t
 *  utf32FixedString    Template for utf36_t based fixed string : container in character unit size is template parameter i. e. number of utf32_t
 *
 *  As they are themselves templates, methods are contains in this include file after classes definitions.
 *
 *  They allows convertion from other utf formats.
 *
 *  Conversion to other formats are NOT managed (and will stay not managed).
 *
 *
 */


#include <config/zconfig.h>




#include <ztoolset/utftemplatestring.h>


/** ============== utf fixed string classes definitions ============================= */

template <size_t _Sz>
class charFixedString : public utftemplateString<_Sz,char>
{
public :

};



#endif // UTFFIXEDSTRING_H
