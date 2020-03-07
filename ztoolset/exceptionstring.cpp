#include <ztoolset/exceptionstring.h>
#include <ztoolset/uristring.h>
utfexceptionString&
utfexceptionString::operator = (const uriString &pString) {return(setFromURI(pString));}
utfexceptionString&
utfexceptionString::operator += (uriString &pString){return (utfexceptionString&)add(pString.toUtf());}

/*
utfexceptionString&
utfexceptionString::operator = (const utfcodeString &pString) {return((utfexceptionString&)strset(pString.content));}


utfexceptionString&
utfexceptionString::operator = (const utfdescString &pString) {return(setFromdescString(pString));}
*/
utfexceptionString&
utfexceptionString::operator += (utfexceptionString &pString) {return (utfexceptionString&)add(pString.content);}
utfexceptionString&
utfexceptionString::operator += (const char* pString) {return ((utfexceptionString&)add((const utf8_t*)pString));}

utfexceptionString&
utfexceptionString::operator += (const utf8_t* pString) {return (utfexceptionString&)add(pString);}
/*utfexceptionString&
utfexceptionString::operator += (utfdescString &pString){return (utfexceptionString&)add(pString.toUtf());}
utfexceptionString&
utfexceptionString::operator += (utfcodeString &pString){return (utfexceptionString&)add(pString.toUtf());}
*/
