#ifndef ZUTFSTRINGS_H
#define ZUTFSTRINGS_H
/**  @file zutfstrings.h In this file is defined a template FIXED string container that is used as base for various sizes of strings.
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
#include <ztoolset/zlimit.h>
#include <stdarg.h>
#include <ztoolset/zatomicconvert.h>

#include <ztoolset/ztypetype.h>


//=================Various derived strings SECTION=============================

#ifdef QT_CORE_LIB
#include <QString>
#endif

#include <ztoolset/utffixedstring.h>
#include <ztoolset/utfvaryingstring.h>


struct Docid_struct;
class utfcodeString : public utf8FixedString<cst_codelen+1>
{
public:
    typedef utftemplateString<cst_codelen+1,utf8_t> _Base;
    utfcodeString () {}

#ifdef QT_CORE_LIB
    utfcodeString (QString &pQS)
    { utfStrncpy<utf8_t>(content,(utf8_t*)pQS.toUtf8().data(),cst_codelen);}
    utfcodeString (QString pQS)
    { utfStrncpy<utf8_t>(content,(utf8_t*)pQS.toUtf8().data(),cst_codelen);}
    QString getCodeString() {QString wQs((char*)content); return wQs;}
#endif // QT_CORE_LIB

    const char* toCString_Strait(void) {return (const char*)content;}

    utfcodeString& fromDocid(Docid_struct& pDocid);

//    using _Base::templateString;
    using _Base::operator =;

    utfcodeString& operator=(Docid_struct& pDocid);

};
#ifndef __UTFDESCSTRING__
#define __UTFDESCSTRING__

class utfdescString : public utf8FixedString<cst_desclen+1>
{
public:
    typedef utftemplateString<cst_desclen+1,utf8_t> _Base;
    typedef utf8FixedString<cst_desclen+1> _Base8;
    utfdescString(void) {}
    utfdescString(const utf8_t* pString) {fromUtf(pString);return ;}

    utfdescString(utfdescString& pIn) {_copyFrom(pIn); }
    utfdescString(utfdescString&& pIn) {_copyFrom(pIn); }

    utfdescString & fromcodeString(const utfcodeString &pCode) {return (utfdescString&)strset(pCode.content);}

//        const char* toCString(void) {return (const char*)content;}

    utfdescString & operator =(const char* pCode) {strset((const utf8_t*)pCode); return *this;}

    utfdescString& operator = (utfdescString& pIn) {_copyFrom(pIn);}
    utfdescString& operator = (utfdescString&& pIn) {_copyFrom(pIn);}
        using _Base::operator += ;
        using _Base::UnitCount;
//    utfdescString& operator = (const char* pString) {return (utfdescString&)fromCString(pString);}
};

#endif// __UTFDESCSTRING__

//#ifndef __UTFFIELDNAMESTRING__
//#define __UTFFIELDNAMESTRING__
class utffieldNameString : public utf8FixedString<cst_fieldnamelen+1>
{
public:
    utffieldNameString() {}
//    utffieldNameString(const char* pString) {fromCString(pString);}
    utffieldNameString(const utf8_t* pString) {fromUtf(pString);}
    using _Base::operator =  ;
    using _Base::operator += ;

        utffieldNameString & fromcodeString(const utfcodeString &pCode) {return(utffieldNameString&) strset(pCode.content);}

        utffieldNameString & operator = (const utfcodeString &pCode)  {return (utffieldNameString&)strset(pCode.content);}
        utffieldNameString & operator += (const utfcodeString &pCode) {return (utffieldNameString&)add(pCode.content);}

};// utffieldNameString

//#endif// __UTFFIELDNAMESTRING__

class utfdoublecodeString : public utf8FixedString<cst_codelen*2+2>
{
public:
    utfdoublecodeString(void) {Key1=(utfcodeString*)this; Key2=(utfcodeString*)(this +sizeof(utfcodeString));}
    utfcodeString* Key1;
    utfcodeString* Key2;
    void clear(void) {Key1->clear();Key2->clear(); return;}
    const char* toCString_Strait(void) {return (const char*)content;}

};


class utfmessageString : public utf8FixedString<cst_messagelen+1>
{
public:
    using  utftemplateString<cst_messagelen+1,utf8_t>::operator =;

    const char* toCString_Strait(void) {return (const char*)content;}

};

class utfkeywordString : public utf8FixedString<cst_keywordlen+1>
{
public:
    using  utftemplateString<cst_keywordlen+1,utf8_t>::operator =;
    const char* toCString_Strait(void) {return (const char*)content;}
};


class utfidentityString : public utf8FixedString<cst_identitylen+1>
{
public:
typedef utf8FixedString<cst_identitylen+1> _Base;

    utfidentityString() {}
    utfidentityString(const utf8_t*pString){strset(pString);}
    utfidentityString(const utfidentityString& pIn ) {_copyFrom(pIn);}
    utfidentityString(const utfidentityString&& pIn ) {_copyFrom(pIn);}

    const char* toCString_Strait(void) {return (const char*)content;}
    utfidentityString & operator = (const utfidentityString& pIn ) {_copyFrom(pIn);}
    utfidentityString & operator = (const utfidentityString&& pIn ) {_copyFrom(pIn);}

    utfidentityString & operator = (const utfdescString& pDesc ) ;
    utfidentityString & operator = (const utfcodeString& pDesc ) {return (utfidentityString&) strset(pDesc.content);}
    utfidentityString & operator = (const char* pDesc ) {return (utfidentityString&) strset((const utf8_t*)pDesc); }
};



typedef utf8VaryingString utf8String;


#endif // ZUTFSTRINGS_H
