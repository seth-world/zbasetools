#ifndef URISTRING_H
#define URISTRING_H

#include <zconfig.h>
#include <ztoolset/zlimit.h>

//#include <ztoolset/utffixedstring.h>

#include <zcrypt/checksum.h>
#include <ztoolset/zdatefull.h>
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


enum URICopyOptions : uint8_t {
  UCO_Nothing = 0,
  UCO_BackUp  = 1 , /* if set then backs up file using renameBck rules using backup extension "bck" */
  UCO_DoNotReplace = 2 /* if set and if destination file exists, it will not be replaced. error ZS_FILEEXISTS will be issued */
} ;


#define __URISTRING__
/**
 * @brief The uriString class Expand templateString template capabilities to file and directories management.
 * includes descString and codeString moves.
 *
 */
class uriString : public utf8VaryingString
{
public:
//    typedef  utftemplateString<cst_urilen+1,utf8_t> _Base;
    typedef utf8VaryingString           _Base;

    uriString(void) : utf8VaryingString(ZType_URIString) {}
    uriString(const uriString &pIn) : _Base(pIn) {ZType=ZType_URIString;}
    uriString(const uriString &&pIn) : _Base(pIn) {ZType=ZType_URIString;}
    uriString(const utf8VaryingString &pIn) : _Base(pIn) {ZType=ZType_URIString;}
    uriString(const char* pIn) : _Base(pIn) {ZType=ZType_URIString;}
    uriString(const utf8_t* pIn) : _Base(pIn) {ZType=ZType_URIString;}
#ifdef QT_CORE_LIB
    uriString(const QString pIn)
    {
      fromQString(pIn);
    }
#endif


    uriString &operator=(const uriString &pIn) { return fromURI(pIn);}
    uriString &operator=(const uriString &&pIn) { return fromURI(pIn); }

/*
    using _Base::operator =;
    using _Base::operator +=;
    using _Base::operator ==;
    using _Base::operator !=;
    using _Base::operator >;
    using _Base::operator <;
    using _Base::fromUtf;

*/

    uriString& operator + (const utf8_t*pChar)
    {
        this->add(pChar);
        return *this;
    }

//    uriString& operator = (const utfdescString &pSource);

    ZStatus operator << (ZDataBuffer& pDBS);

    uriString &fromURI(const uriString* pURI) ;
    uriString &fromURI(const uriString& pURI) ;
/*
    uriString& fromdescString(const utfdescString &pString);
    uriString & fromcodeString(const utfcodeString &pURI) ;
*/
//----------------URISTRING Section-------------------
//Nota Bene descString must have been defined ( __DESCSTRING__ pre-proc parameter must be set)

//------file and directory operations--------------
    /*
     *    /ddddddd/dddddddd/dddddddddd/<rootname>.<extension>
     *    |                |          |                     |
     *    |                +-last dir-+                     |
     *    +---directory path----------+---base name---------+
     */
    static uriString getHomeDir(void) ;

    /** @brief getFileExtension give the part of base name located after '.' sign     */
    utf8VaryingString getFileExtension() const;
    /** @brief uriString::getDirectoryPath Returns the full file's directory path
     * i. e. </directory path/><root name>.<extension>
     * @return an utf8VaryingString with the full file's directory path including
     */
    utf8VaryingString getDirectoryPath() const;
    /** @brief getLastDirectoryName Returns the last directory mentionned in file's directory path
        i. e. </.../last directory/><root name>.<extension>
     * @return utf8VaryingString with the last file's directory within its directory path or an empty string if uriString is empty.
     */
    utf8VaryingString getLastDirectoryName() const;

    /** @brief uriString::getBasename       returns a utf8VaryingString containing the file's base name\n
    i. e. </directory path/><root name>.<extension>\n
        Where base name is : <root name>.<extension> without any directory path
    * @return utf8VaryingString with the file's base name or an empty string if uriString is empty.
    */
    utf8VaryingString getBasename() const ;

    /** @brief getRootname returns a utf8String containing the file's root name\n
     *  i. e. </directory path/><file's root name>.<extension>
     * @return utf8VaryingString with the file's root name (base name without extension) or an empty string if uriString is empty.
     */
    utf8VaryingString getRootname() const;

    void changeFileExtension(const utf8VaryingString& pExt);
    void changeBasename(const utf8VaryingString& pBasename);

    /** @brief getUrl() gets an url from local file path definition @see getLocal() */
    utf8String getUrl() const;
    /** @brief getLocal () gets an uriString certified to be a local file name (as opposed to url) @see getUrl() */
    uriString getLocal () const;

    uriString &setDirectoryPath(uriString &pDirectoryPath);

    static utf8String getUniqueName (const char* pRootName);

    void setUniqueName (const char* pRootName);

    void setExtension (const char* pExtension);

    bool        isDirectory (void);
    bool        isRegularFile(void);

    /**
     * @brief remove removes the file from file system.
     * @return a ZStatus :
     *  ZS_SUCCESS file has been successfully removed.
     *  ZS_FILERROR  file cannot be removed. Low level explainations are set in ZException.
     *  ZS_ACCESSRIGHTS : if file exists but cannot be suppressed due to accesrights limitation
     *  ZS_LOCKED : the file exists but cannot be suppressed due to lock by another process/user
     *  ZS_FILENOTEXIST : the file does not exist
     *  ZS_FILERROR : other error
     */
    ZStatus remove();

    /**
     * @brief renameBck renames file with a special extension suffix given by pBckExt plus a incremental 2 digit value as follows :\n
     *  <base filename>.<extension>_<pBckExt><nn>
     *
     * pBckExt is defaulted to string "bck".
     *
     * Example :  file name                   toto.doc\n
     *            will be renamed to          toto.doc_bck01  nb: toto.doc does not exist anymore
     *
     *            if a second run is done while toto.doc has been created agein
     *            will be renamed to          toto.doc_bck02
     *
     * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSError
     * @errors :
     *    ZS_FILEERROR  : cannot physically rename file.  ZException is loaded with errno and appropriate error explainations.
     */
    ZStatus renameBck(const char* pBckExt="bck");

    /** @brief backupFile makes a backup copy of current file
     * with given pBckExt extension (defaulted to "bck")
     * according renameBck naming rules.
      */
    ZStatus backupFile(const char* pBckExt="bck");


    /** @brief copyFile copies pSource file to pDest file according pOption
     *  UCO_BackUp : if pDest file exists, then it is renamed using renameBck("bck") rules.
     *  UCO_DoNotReplace : if pDest file exists, then it is renamed using renameBck("bck") rules.
     *  if none of the above options are set :
     *  and if destination file exists, it is replaced and ZS_FILEREPLACED status is returned
     *  in all other case of normal termination, ZS_SUCCESS is returned.
     */
    static ZStatus copyFile(uriString pDest, const uriString pSource, uint8_t pOption);

    uriString& addConditionalDirectoryDelimiter(void);
    uriString& addDirectoryDelimiter(void);
    /** @brief changeAccessRights() change the access right of current file with pMode, linux mode
     *                              this routine resets errno, and errno will be set to internal error code if any.*/
    ZStatus changeAccessRights(mode_t pMode);

    static uriString currentWorkingDirectory();

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

    checkSum getChecksum(void) const;

    ZStatus loadContent(ZDataBuffer &pDBS) const;
    ZStatus loadUtf8(utf8VaryingString &pUtf8) const ;
    ZStatus loadUtf16(utf16VaryingString &pUtf16) const ;
    ZStatus loadUtf32(utf32VaryingString &pUtf32) const ;

    ZStatus loadAES256(ZDataBuffer &pDBS,const ZCryptKeyAES256& pKey,const ZCryptVectorAES256& pVector) ;


    ZStatus loadContentZTerm(ZDataBuffer &pDBS) ; /* same as loadContent but forces Zero termination */

    ZStatus writeContent (ZDataBuffer &pDBS) const ;
    ZStatus appendContent (ZDataBuffer &pDBS) const ;

    ZStatus writeAES256(ZDataBuffer &pDBS,const ZCryptKeyAES256& pKey,const ZCryptVectorAES256& pVector) ;
    /**
     * @brief writeContent writes utf8 string content to file pointed by uriString.
     *  NB: end mark '\0' is not written to file.
     * @param pStr utf8 string to wrtie
     * @return  a ZStatus
     */
    ZStatus writeContent (utf8VaryingString &pStr) const;
    ZStatus appendContent (utf8VaryingString &pStr) const ;
//    ZStatus writeText (varyingCString &pDBS) ;

    bool    exists(void) const  ;

    /* set to userŝ working directory */
    void    setcwd();

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


/* export - import functions are located within base utf8VaryingString class */

};// uriString---------------------------------------------

uriString operator + (const uriString pIn1,const uriString pIn2);




#endif // URISTRING_H
