#ifndef ZUTFSTRINGS_CPP
#define ZUTFSTRINGS_CPP



#include <ztoolset/zutfstrings.h>
#include <ztoolset/zbasedatatypes.h>




utfidentityString & utfidentityString::operator = (const utfdescString& pDesc ) {return (utfidentityString&) strset(pDesc.content); }



#endif // ZUTFSTRINGS_CPP

