#ifndef EXCEPTIONSTRING_H
#define EXCEPTIONSTRING_H

const long cst_exceptionlen=512;

#include <ztoolset/utffixedstring.h>
//#include <ztoolset/zutfstrings.h>
//#include <ztoolset/uristring.h>
/*
class utfdescString;
class utfmessageString;
class utfcodeString;
*/
//class uriString;


struct utfexceptionString:public utf8FixedString<cst_exceptionlen+1>
{
    typedef utf8_t  _Utf;
    typedef utf8FixedString<cst_exceptionlen+1> _Base;

    using _Base::operator = ;
    using _Base::operator += ;

//    utfexceptionString() {}

    const char* toCChar(void) {return (const char*)content;}
//char content[cst_exceptionstringlen+1];
//void clear(void) {memset(this,0,sizeof(exceptionString));}
//size_t  size(void) {return (strlen(content)); }
//    char * toString(void) {return(content);}
//    bool isEmpty(void) {return (content[0]=='\0');}
/*    void
    sprintf (const char *pFormat,...) {
                                        va_list args;
                                       va_start (args, pFormat);
                                      vsprintf (content ,pFormat, args);
                                      va_end(args);
                                       return;
                                       }
*/
    //-------------messageString
    utfexceptionString& setFromURI(const uriString &pUri) ;
    utfexceptionString& setFromURI(const uriString *pURI) ;
    utfexceptionString& setFromString(char *pString) ;

 /*   utfexceptionString& setFrommessageString(const utfmessageString &pCode) ;
    utfexceptionString& setFromdescString(const utfdescString &pCode) ;
    utfexceptionString& setFromcodeString(const utfcodeString &pCode) ;*/



// ---------edition----------------



//------------------operator = overload ------------------------------------------------
// only basic string types are allowed here

    utfexceptionString& operator = (const char* pString) {strset((const _Utf*)pString);return (*this);}

    utfexceptionString& operator = (const uriString &pString) ;
    utfexceptionString& operator += (uriString &pString);

    utfexceptionString& operator += (utfexceptionString &pString);
    utfexceptionString& operator += (const char* pString) ;

    utfexceptionString& operator += (const utf8_t* pString) ;


#ifdef QT_CORE_LIB
    QString toQString(void) {QString wRet((const char*)content); return wRet;}

    utfexceptionString& setFromQString(const QString &pString) {return (utfexceptionString&) strset((_Utf*)pString.toUtf8().data());}

    utfexceptionString& operator = (const QString pString) {setFromQString(pString);return(*this);}

    utfexceptionString& operator += (const QString pString) {return (utfexceptionString&)add((_Utf*)pString.toUtf8().data());}

#endif // #ifdef QT_CORE_LIB

    void print(void);
};


#endif // EXCEPTIONSTRING_H
