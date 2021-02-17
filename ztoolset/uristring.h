#ifndef URISTRING_H
#define URISTRING_H

#include <zconfig.h>
#include <ztoolset/zlimit.h>

#include <ztoolset/utffixedstring.h>

#include <zcrypt/checksum.h>
#include <ztoolset/zdate.h>
#include <ztoolset/userid.h>


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

class utf8VaryingString;
class utf16VaryingString;
class utf32VaryingString;


//
//! \brief The uriStat_struct struct stores the statistics about uriString file
//
class uriStat {
public:
    char            Dev[50];        //!< device
    ZDateFull       Created;        //!< creation date time
    ZDateFull       LastModified;   //!< last modification date time
    int             Rights;         //!< rights
    ZSystemUserId   Uid;            //!< System user identification
    long long       Size;           //!< file size
    char            OwnerName[150]; //!< Owner name

    uriStat(void) { clear(); }
    uriStat(uriStat &pIn) { _copyFrom(pIn); }
    uriStat(uriStat&&pIn) { _copyFrom(pIn); }

    uriStat &_copyFrom(uriStat &pIn)
    {
        clear();
        strcpy(Dev, pIn.Dev);
        Created = pIn.Created;
        LastModified = pIn.LastModified;
        Rights = pIn.Rights;
        Size = pIn.Size;
        strcpy(OwnerName, pIn.OwnerName);
        return *this;
    }

    uriStat & operator= (uriStat &pIn) { return _copyFrom(pIn); }
    uriStat & operator= (uriStat&&pIn) { return _copyFrom(pIn); }

    void clear(void)
    {
        memset(Dev,0,sizeof(Dev));
        Created.clear();
        LastModified.clear();
        Rights=0;
        Size=0;
        Uid.clear();
        memset(OwnerName,0,sizeof(OwnerName));
    }
};


#define __URISTRING__
/**
 * @brief The uriString class Expand templateString template capabilities to file and directories management.
 * includes descString and codeString moves.
 *
 */
class uriString : public utf8FixedString<cst_urilen+1>
{
public:
//    typedef  utftemplateString<cst_urilen+1,utf8_t> _Base;
    typedef utf8FixedString<cst_urilen+1>           _Base;

    uriString(void) {clear();}
    uriString(const uriString &pIn) : _Base(pIn) {}
    uriString(const uriString &&pIn) : _Base(pIn) {}
#ifdef QT_CORE_LIB
    uriString(const QString pIn)
    {
      fromQString(pIn);
    }
#endif


    uriString &operator=(const uriString &pIn) { return (uriString&)_Base::operator=(pIn);}
    uriString &operator=(const uriString &&pIn) { return (uriString&)_Base::operator=(pIn); }

    uriString(const utf8_t *pString) {fromUtf(pString);}
    uriString(const char* pString) {strset((const utf8_t*)pString);}
//    uriString(const uriString& pURI) {fromURI(pURI);}

//    const char* toCString_Strait() {return (const char*)content;}
    const char* toCChar() const {return (const char*)content;}

    const uriString toConst() const { return *this; }

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

    uriString& operator = (const utfdescString &pSource);
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

    uriString& fromdescString(const utfdescString &pString);
    uriString & fromcodeString(const utfcodeString &pURI) ;

//----------------URISTRING Section-------------------
//Nota Bene descString must have been defined ( __DESCSTRING__ pre-proc parameter must be set)

//------file and directory operations--------------

    static uriString getHomeDir(void) ;

    utfdescString getFileExtension() const;
    utfdescString getDirectoryPath() const;
    utfdescString getLastDirectoryName() const;
    utfdescString getBasename() const ;
    utfdescString getRootBasename() const;

    /** @brief getUrl() gets an url from local file path definition */
    utf8String getUrl() const;
    /** @brief getLocal () gets an uriString certified to be a local file name */
    uriString getLocal () const;

    uriString &setDirectoryPath(uriString &pDirectoryPath);

    static utfdescString getUniqueName (const char* pRootName);

    void setUniqueName (const char* pRootName);

    void setExtension (const char* pExtension);

    bool        isDirectory (void);
    bool        isRegularFile(void);

    uriString& addConditionalDirectoryDelimiter(void);
    uriString& addDirectoryDelimiter(void);
    /** @brief changeAccessRights() change the access right of current file with pMode, linux mode
     *                              this routine resets errno, and errno will be set to internal error code if any.*/
    ZStatus changeAccessRights(mode_t pMode);

#ifdef QT_CORE_LIB

    uriString &fromQUrl(QUrl &pUrl);

#endif //  QT_CORE_LIB

// ---------------Mime------------------
    bool addMimeFileExtension (utfdescString pMimeName);
    bool addMimeFileExtension (utfdescString *pMimeName) {return(addMimeFileExtension(*pMimeName));}

// -----Control & file operations-------
//

    ZStatus getStatR(uriStat &pZStat) const;

    long long getFileSize(void) const  ;

    checkSum getChecksum(void);

    ZStatus loadContent(ZDataBuffer &pDBS) const;
    ZStatus loadUtf8(utf8VaryingString &pUtf8) ;
    ZStatus loadUtf16(utf16VaryingString &pUtf16) ;
    ZStatus loadUtf32(utf32VaryingString &pUtf32) ;

    ZStatus loadAES256(ZDataBuffer &pDBS,const ZCryptKeyAES256& pKey,const ZCryptVectorAES256& pVector) ;


    ZStatus loadContentZTerm(ZDataBuffer &pDBS) ; /* same as loadContent but forces Zero termination */

    ZStatus writeContent (ZDataBuffer &pDBS) ;
    ZStatus appendContent (ZDataBuffer &pDBS) ;

    ZStatus writeAES256(ZDataBuffer &pDBS,const ZCryptKeyAES256& pKey,const ZCryptVectorAES256& pVector) ;
    /**
     * @brief writeContent writes utf8 string content to file pointed by uriString.
     *  NB: end mark '\0' is not written to file.
     * @param pStr utf8 string to wrtie
     * @return  a ZStatus
     */
    ZStatus writeContent (utf8VaryingString &pStr) ;
    ZStatus appendContent (utf8VaryingString &pStr) ;
//    ZStatus writeText (varyingCString &pDBS) ;

    bool    exists(void) const  ;
    /**
 * @brief uriString::suppress  suppresses the file pointed to by uriString content.
 *
 * - ZS_ACCESSRIGHTS : if file exists but cannot be suppressed due to accesrights limitation
 * - ZS_LOCKED : the file exists but cannot be suppressed due to lock by another process/user
 * - ZS_FILENOTEXIST : the file does not exist
 * - ZS_ERROR : other error
 *
 * - ZS_SUCCESS : file has been successfully removed.
 *
 * @note no exception is set by this routine
 *
 * @return a ZStatus
 */
    ZStatus suppress(void)  ;


    ZStatus fileList(ZArray<uriString>* pList=nullptr);
    ZStatus list(ZArray<uriString>* pList=nullptr, ZDir_File_type pZDFT=ZDFT_All);
    ZStatus subDir(ZArray<uriString>* pList=nullptr);

 //----------------END URISTRING Section-------------------
#ifdef __COMMENT__
    ZDataBuffer _exportURF();
    size_t _importURF(unsigned char* pURF);

    /** @brief _exportUVF  Exports a string to a Universal Varying Format (dedicated format for strings)
     *  Universal Varying  Format stores string data into a varying length string container excluding '\0' character terminator
     *  led by
     *   - uint8_t : char unit size
     *   - UVF_Size_type : number of character units of the string.
     * @return a ZDataBuffer with string content in Varying Universal Format set to big endian
     */
    ZDataBuffer _exportUVF();
    /** @brief _importUVF Import string from Varying Universal Format (dedicated format for strings)
     *  Varying Universal Format stores string data into a varying length string container excluding '\0' character terminator
     *  led by
     *   - uint8_t : char unit size
     *   - UVF_Size_type : number of character units of the string.
     * Important : imported string format (utf-xx) must be the same as current string
     * @param[in,out] pUniversalPtr pointer to Varying Universal formatted data header.
     *                this pointer is updated to point on the first byte after imported data.
     * @return total size IN BYTES of  bytes used from pUniversalPtr buffer (Overall used size including header)
     */
    size_t _importUVF(unsigned char *&pUniversalPtr);

    /** @brief _getexportUVFSize() compute the requested export size in bytes for current string, including header */
    UVF_Size_type _getexportUVFSize();

    UVF_Size_type _getimportUVFSize(unsigned char* pUniversalPtr);
#endif // __COMMENT__
};// uriString---------------------------------------------

uriString operator + (const uriString pIn1,const uriString pIn2);




#endif // URISTRING_H
