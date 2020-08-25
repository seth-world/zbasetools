#ifndef URISTRING_H
#define URISTRING_H

#include <zconfig.h>
#include <ztoolset/zlimit.h>

#include <ztoolset/utffixedstring.h>

#include <zcrypt/checksum.h>
#include <ztoolset/zdate.h>



#ifdef QT_CORE_LIB
#include <QUrl>
#endif // QT_CORE_LIB






#ifndef ZDIR_FILE_TYPE
#define ZDIR_FILE_TYPE
enum ZDir_File_type {
    ZDFT_Nothing        = 0,
    ZDFT_RegularFile    = 1,
    ZDFT_Directory      = 2,
    ZDFT_All            = 0x0f
};
#endif //ZDIR_FILE_TYPE


using namespace zbs;


//
//! \brief The uriStat_struct struct stores the statistics about uriString file
//
struct uriStat_struct {
    char        Dev[50];        //!< device
    ZDateFull   Created;        //!< creation date time
    ZDateFull   LastModified;   //!< last modification date time
    int         Rights;         //!< rights

    long long   Size;           //!< file size
    char        Owner[150];     //!< Owner name

    uriStat_struct (void) {memset(this,0,sizeof(uriStat_struct));}
    void clear(void)
    {
        memset(Dev,0,sizeof(Dev));
        Created.clear();
        LastModified.clear();
        Rights=0;
        Size=0;
        memset(Owner,0,sizeof(Owner));
    }
};

class utf8VaryingString;
class utf16VaryingString;
class utf32VaryingString;
#define __URISTRING__
/**
 * @brief The uriString class Expand templateString template capabilities to file and directories management.
 * includes descString and codeString moves.
 *
 */
class uriString : public utf8FixedString<cst_urilen+1>
{
public:
    typedef  utftemplateString<cst_urilen+1,utf8_t> _Base;
//    typedef utf8FixedString                      _Base;

    uriString(void) {}
    uriString(const utf8_t *pString) {fromUtf(pString);}
    uriString(const char* pString) {strset((const utf8_t*)pString);}
//    uriString(const uriString& pURI) {fromURI(pURI);}

    const char* toCString_Strait() {return (const char*)content;}
    const char* toCChar() const {return (const char*)content;}

    using _Base::operator =;
    using _Base::operator +=;
    using _Base::operator ==;
    using _Base::operator !=;
    using _Base::operator >;
    using _Base::operator <;
    using _Base::fromUtf;



    uriString& operator + (const utf8_t*pChar)
    {
        this->add(pChar);
        return *this;
    }

    uriString& operator = (utfdescString &pSource);
    uriString& operator += (utfdescString &pSource);

    uriString& operator = (utfcodeString &pCode) ;
    uriString& operator += (utfcodeString &pCode);

    ZStatus operator <<(ZDataBuffer& pDBS);

    bool operator == (utfdescString& pCompare) ;
    bool operator != (utfdescString &pCompare);
    bool operator > (utfdescString& pCompare) ;
    bool operator < (utfdescString &pCompare);

 //   uriString& fromURI(const uriString& pURI) ;
    uriString &fromURI(const uriString* pURI) ;

    uriString& fromdescString(utfdescString &pString);
    uriString & fromcodeString(utfcodeString &pURI) ;

//----------------URISTRING Section-------------------
//Nota Bene descString must have been defined ( __DESCSTRING__ pre-proc parameter must be set)

//------file and directory operations--------------

    static uriString getHomeDir(void) ;

    utfdescString getFileExtension() ;
    utfdescString getDirectoryPath() ;
    utfdescString getLastDirectoryName();
    utfdescString getBasename() ;
    utfdescString getRootBasename();

    uriString &setDirectoryPath(uriString &pDirectoryPath);

    static utfdescString getUniqueName (const char* pRootName);

    void setUniqueName (const char* pRootName);

    void setExtension (const char* pExtension);

    bool        isDirectory (void);
    bool        isRegularFile(void);

    uriString& addConditionalDirectoryDelimiter(void);
    uriString& addDirectoryDelimiter(void);

#ifdef QT_CORE_LIB

    uriString &fromQUrl(QUrl &pUrl);

#endif //  QT_CORE_LIB

// ---------------Mime------------------
    bool addMimeFileExtension (utfdescString pMimeName);
    bool addMimeFileExtension (utfdescString *pMimeName) {return(addMimeFileExtension(*pMimeName));}

// -----Control & file operations-------
//

    ZStatus getStatR(uriStat_struct &pZStat);

    long long getFileSize(void) ;

    checkSum getChecksum(void);

    ZStatus loadContent(ZDataBuffer &pDBS) ;
    ZStatus loadUtf8(utf8VaryingString &pUtf8) ;
    ZStatus loadUtf16(utf16VaryingString &pUtf16) ;
    ZStatus loadUtf32(utf32VaryingString &pUtf32) ;

    ZStatus loadContentZTerm(ZDataBuffer &pDBS) ; /* same as loadContent but forces Zero termination */

    ZStatus writeContent (ZDataBuffer &pDBS) ;
    /**
     * @brief writeContent writes utf8 string content to file pointed by uriString.
     *  NB: end mark '\0' is not written to file.
     * @param pStr utf8 string to wrtie
     * @return  a ZStatus
     */
    ZStatus writeContent (utf8VaryingString &pStr) ;
//    ZStatus writeText (varyingCString &pDBS) ;

    bool    exists(void)  ;
    ZStatus suppress(void)  ;


    ZStatus fileList(ZArray<uriString>* pList=nullptr);
    ZStatus list(ZArray<uriString>* pList=nullptr, ZDir_File_type pZDFT=ZDFT_All);
    ZStatus subDir(ZArray<uriString>* pList=nullptr);

 //----------------END URISTRING Section-------------------

};// uriString---------------------------------------------

uriString operator + (const uriString pIn1,const uriString pIn2);




#endif // URISTRING_H
