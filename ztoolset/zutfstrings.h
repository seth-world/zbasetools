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
    typedef utf8FixedString<cst_codelen+1> _Base;
    utfcodeString () {utfInit(ZType_Utf8FixedString,ZCHARSET_UTF8);}
    utfcodeString(utfcodeString& pIn):_Base(pIn) { }
    utfcodeString(utfcodeString&& pIn):_Base(pIn) { }
    utfcodeString(const char* pIn) {utfInit(ZType_Utf8FixedString,ZCHARSET_UTF8); _Base::strset((const utf8_t*)pIn);}

#ifdef QT_CORE_LIB
    utfcodeString (QString &pQS)
    { utfStrncpy<utf8_t>(content,(utf8_t*)pQS.toUtf8().data(),cst_codelen);}
    utfcodeString (QString pQS)
    { utfStrncpy<utf8_t>(content,(utf8_t*)pQS.toUtf8().data(),cst_codelen);}
    QString getCodeString() {QString wQs((char*)content); return wQs;}
#endif // QT_CORE_LIB

//    const char* toCString_Strait(void) {return (const char*)content;}

    utfcodeString& fromDocid(Docid_struct& pDocid);

//    using _Base::templateString;
    using _Base::operator =;
    using _Base::operator +=;

    utfcodeString & operator = (const utfcodeString& pIn ) {return (utfcodeString&)_copyFrom(pIn);}
    utfcodeString & operator = (const utfcodeString&& pIn ) {return (utfcodeString&)_copyFrom(pIn);}

    utfcodeString& operator=(Docid_struct& pDocid);

//    int compare(utfcodeString &pComp) { return utfStrcmp<utf8_t>(content, pComp.content); }

};
#ifndef __UTFDESCSTRING__
#define __UTFDESCSTRING__

class utfdescString : public utf8FixedString<cst_desclen+1>
{
public:
//    typedef utftemplateString<cst_desclen+1,utf8_t> _Base;
    typedef utf8FixedString<cst_desclen+1> _Base;
    utfdescString(void) {utfInit(ZType_Utf8FixedString,ZCHARSET_UTF8);}
    utfdescString(const utf8_t* pString) {utfInit(ZType_Utf8FixedString,ZCHARSET_UTF8);fromUtf(pString);return ;}

    utfdescString(utfdescString& pIn):_Base(pIn) {}
    utfdescString(utfdescString&& pIn):_Base(pIn) {}

    utfdescString(const char* pString) {utfInit(ZType_Utf8FixedString,ZCHARSET_UTF8);fromUtf((const utf8_t*)pString);}

    utfdescString & fromcodeString(const utfcodeString &pCode) {return (utfdescString&)strset(pCode.content);}

//        const char* toCString(void) {return (const char*)content;}

//    utfdescString & operator =(const char* pCode) {strset((const utf8_t*)pCode); return *this;}

    utfdescString& operator = (utfdescString& pIn) {return (utfdescString&)_copyFrom(pIn);}
    utfdescString& operator = (utfdescString&& pIn) {return (utfdescString&)_copyFrom(pIn);}
        using _Base::operator += ;
        using _Base::operator = ;
        using _Base::UnitCount;
//    utfdescString& operator = (const char* pString) {return (utfdescString&)fromCString(pString);}

    int compare(const utfdescString &pComp) const { return utfStrcmp<utf8_t>(content, pComp.content); }
};

#endif// __UTFDESCSTRING__


class utffieldNameString : public utf8FixedString<cst_fieldnamelen+1>
{
public:
    utffieldNameString() {}
//    utffieldNameString(const char* pString) {fromCString(pString);}
    utffieldNameString(const utf8_t* pString) {fromUtf(pString);}
    typedef utf8FixedString<cst_fieldnamelen+1> _Base;
    utffieldNameString(utffieldNameString &pIn):_Base(pIn) { }
    utffieldNameString(utffieldNameString &&pIn):_Base(pIn) { }

    using _Base::operator =  ;
    using _Base::operator += ;

        utffieldNameString & fromcodeString(const utfcodeString &pCode) {return(utffieldNameString&) strset(pCode.content);}

        utffieldNameString & operator = (const utfcodeString &pCode)  {return (utffieldNameString&)strset(pCode.content);}
        utffieldNameString & operator += (const utfcodeString &pCode) {return (utffieldNameString&)add(pCode.content);}

        utffieldNameString & operator = (const utffieldNameString &pCode)  {return (utffieldNameString&)strset(pCode.content);}
        utffieldNameString & operator += (const utffieldNameString &pCode) {return (utffieldNameString&)add(pCode.content);}

    bool compare(utffieldNameString &pComp) { return utfStrcmp<utf8_t>(content, pComp.content); }
};// utffieldNameString


class utfdoublecodeString : public utf8FixedString<cst_codelen*2+2>
{
public:
    typedef utf8FixedString<cst_codelen*2+2> _Base;
    utfdoublecodeString(void) {Key1=(utfcodeString*)this; Key2=(utfcodeString*)(this +sizeof(utfcodeString));}
    utfdoublecodeString(utfcodeString &pKey1, utfcodeString &pKey2)
    {
        Key1->strset(pKey1.content);
        Key2->strset(pKey2.content);
    }
    utfdoublecodeString(utfdoublecodeString &pIn):_Base(pIn) {  }
    utfdoublecodeString(utfdoublecodeString &&pIn):_Base(pIn) {  }

    utfcodeString getKey1() { return ((utfcodeString&)(*Key1)); }
    utfcodeString getKey2() { return ((utfcodeString&)(*Key2)); }

    void setKey1(const char *pKey1) { Key1->strset((const utf8_t *) pKey1); }
    void setKey2(const char *pKey2) { Key2->strset((const utf8_t *) pKey2); }

    void setKey1(const utfcodeString &pKey1) { Key1->strset(pKey1.content); }
    void setKey2(const utfcodeString &pKey2) { Key2->strset(pKey2.content); }


    utfcodeString* Key1=Key1=(utfcodeString*)this;
    utfcodeString* Key2=(utfcodeString*)(Key1 +sizeof(utfcodeString));
    void clear(void) {Key1->clear();Key2->clear(); return;}

    using  _Base::operator =;
    using _Base::operator += ;

    utfdoublecodeString & operator = (const utfdoublecodeString& pIn ) {return (utfdoublecodeString&)_copyFrom(pIn);}
    utfdoublecodeString & operator = (const utfdoublecodeString&& pIn ) {return (utfdoublecodeString&)_copyFrom(pIn);}

//    bool compare(utffieldNameString &pComp) { return utfStrcmp<utf8_t>(content, pComp.content); }
};


class utfmessageString : public utf8FixedString<cst_messagelen+1>
{
public:
    typedef utf8FixedString<cst_messagelen+1> _Base;
    utfmessageString () {}
    utfmessageString(utfmessageString &pIn):_Base(pIn) {}
    utfmessageString(utfmessageString &&pIn):_Base(pIn) { }
    using  _Base::operator =;
    using _Base::operator += ;
    utfmessageString & operator = (const utfmessageString& pIn ) {return (utfmessageString&)_copyFrom(pIn);}
    utfmessageString & operator = (const utfmessageString&& pIn ) {return (utfmessageString&)_copyFrom(pIn);}

//    const char* toCString_Strait(void) {return (const char*)content;}

};

class utfkeywordString : public utf8FixedString<cst_keywordlen+1>
{
public:
    typedef utf8FixedString<cst_keywordlen+1> _Base;
    utfkeywordString () {}
    utfkeywordString(utfkeywordString &pIn):_Base(pIn) {  }
    utfkeywordString(utfkeywordString &&pIn):_Base(pIn) {}

    using  _Base::operator =;
    using _Base::operator += ;

    utfkeywordString & operator = (const utfkeywordString& pIn ) {return (utfkeywordString&)_copyFrom(pIn);}
    utfkeywordString & operator = (const utfkeywordString&& pIn ) {return (utfkeywordString&)_copyFrom(pIn);}

//    const char* toCString_Strait(void) {return (const char*)content;}
};


class utfidentityString : public utf8FixedString<cst_identitylen+1>
{
public:
    typedef utf8FixedString<cst_identitylen+1> _Base;
    utfidentityString() {}
    utfidentityString(const char*pString):_Base(pString){}
    utfidentityString(const utf8_t*pString){strset(pString);}
    utfidentityString(const utfidentityString& pIn ):_Base(pIn) {}
    utfidentityString(const utfidentityString&& pIn ):_Base(pIn) {}

//    const char* toCString_Strait(void) {return (const char*)content;}

    using _Base::operator += ;
    using _Base::operator = ;
    using _Base::operator == ;


    utfidentityString & operator = (const utfidentityString& pIn ) {return (utfidentityString&)_copyFrom(pIn);}
    utfidentityString & operator = (const utfidentityString&& pIn ) {return (utfidentityString&)_copyFrom(pIn);}

    utfidentityString & operator = (const utfdescString& pDesc ) ;
    utfidentityString & operator = (const utfcodeString& pDesc ) {return (utfidentityString&) strset(pDesc.content);}
    utfidentityString & operator = (const char* pDesc ) {return (utfidentityString&) strset((const utf8_t*)pDesc); }



};

#include <ztoolset/utfvaryingstring.h>
typedef utf8VaryingString utf8String;
typedef utf16VaryingString utf16String;
typedef utf32VaryingString utf32String;


#endif // ZUTFSTRINGS_H
