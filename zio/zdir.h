#ifndef ZDIR_H
#define ZDIR_H
#include <config/zconfig.h>

#include <stddef.h>
#ifdef __USE_WINDOWS__
#include <zio/dirent_win.h>
#else
#include <dirent.h>
#endif

#include <ztoolset/zerror.h>
#include <ztoolset/zexceptionmin.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/uristring.h>




namespace zbs {

typedef uint8_t ZDirFileEn_type;
enum ZDirFileEn :  ZDirFileEn_type {
    ZDFT_Nothing        = 0,
    ZDFT_RegularFile    = 1,
    ZDFT_Directory      = 2,
    ZDFT_SymbolicLink   = 4,
    ZDFT_Other          = 8,
    ZDFT_Hidden         = 10,
    ZDFT_All            = 0xFF
};

class ZDirSortedByName;


class DirMap
{
public:
  DirMap() {clear();}
  DirMap(const DirMap &pIn) { _copyFrom(pIn); }
  DirMap(const DirMap &&pIn) { _copyFrom(pIn); }

  DirMap &_copyFrom(const DirMap &pIn)
    {
        clear();
        Name = pIn.Name;
        BaseName = pIn.BaseName;
        Size = pIn.Size;
        Uid = pIn.Uid;
        Created = pIn.Created;
        Modified = pIn.Modified;
        Type = pIn.Type;
        return *this;
    }

    void clear()
    {
        Name.clear();
        BaseName.clear();
        Size = 0;
        Uid = 0;
        Created.clear();
        Modified.clear();
        Type=0;
    }

    DirMap &operator=(const DirMap &pIn) { _copyFrom(pIn); return *this;}
    DirMap &operator=(const DirMap &&pIn) { _copyFrom(pIn); return *this;}

    uriString           Name;
    utf8VaryingString   BaseName;
    size_t              Size = 0;

    ZSystemUserId       Uid = 0;
    ZDateFull           Created;
    ZDateFull           Modified;
    ZDirFileEn_type     Type = 0;
}; // DirMap

class ZFileNameSelection;
class ZFileSelection;

class ZDir : public uriString
{
public:

    ZDir (void) {}
    ZDir (const utf8_t *pDirName) ;
    ZDir(const ZDir &pIn):uriString(pIn) { _copyFrom(pIn); }
    ZDir(const ZDir &&pIn):uriString(pIn) { _copyFrom(pIn); }

    ZDir(const uriString &pIn) { setPath(pIn); }
//#ifdef QT_CORE_LIB
//    ZDir(const QString pIn) { setPath(pIn); }
//#endif


    ~ZDir()
    {
        if (_SystDir != nullptr)
            closedir(_SystDir);
    }

    ZDir &_copyFrom(const ZDir &pIn)
    {
        setPath(pIn);
        return *this;
    }

    void clear()
    {
        if (_SystDir != nullptr)
            closedir(_SystDir);
        _SystDir = nullptr;
        uriString::clear();
    }

    typedef uint8_t DENM_type;
    enum DirEntryNameMatch : DENM_type
    {
        Nothing         =    0,  /* no filter */
        All             =    0,
        ExactMatch      =    2,  /* exact match (case regardless if CaseRegardless option is Ored */
        StartsWith      =    4,
        EndsWith        =    8,
        Contains        =   StartsWith | EndsWith,
        CaseRegardless  = 0x20,  /* if Ored, may combine with any other of above */
    };

/**
 * @brief ZDir::setPath initializes ZDir object with a directory path pPath. It requires a file descriptor.
 *  directory path object is destroyed and then, file descriptor is released, when object is destroyed.
 * pPath must exist and be accessible by current process.
 *
 * for exception content see https://www.man7.org/linux/man-pages/man3/opendir.3.html
 *
 * @return a ZStatus ZS_SUCCESS when successfull.
 *  if an error occurs, ZException is positionned with the appropriate message preceeded by errno symbolic value.
 */
    ZStatus setPath (const utf8VaryingString& pPath) ;

    void reset() { closeDir();}

    /** @brief dir
     *
     * for exception content see https://www.man7.org/linux/man-pages/man3/readdir.3.html
     * @param pDirEntry
     * @param pZDFT
     * @return
     */
    ZStatus dir(uriString &pDirEntry, ZDirFileEn_type pRequestedType=ZDFT_All);
    ZStatus dirNext(uriString &pDirEntry);
    void _closeReadDir();

    static int countElements(uriString &pDirEntry,ZDirFileEn_type pZDFT=ZDFT_All);

    ZStatus dirAll(zbs::ZArray<uriString> &pDirArray, ZDirFileEn_type pZDFT = ZDFT_All);

    ZStatus fullDirAll(zbs::ZArray<::DirMap> &pDirArray, ZDirFileEn_type pZDFT = ZDFT_All);

    ZStatus fullDir( DirMap &pDirEntry, ZDirFileEn_type pZDFT=ZDFT_RegularFile|ZDFT_Directory|ZDFT_Hidden);
//    ZStatus fullDirNext(::DirMap &pDirEntry);

    ZStatus fullDirSel(DirMap &pDirEntry, ZFileSelection &pSelection, ZDirFileEn_type pTypeToSelect=ZDFT_RegularFile|ZDFT_Directory|ZDFT_Hidden);

    ZStatus _fullDir(dirent* &wDirEntry, ZDirFileEn_type & pOutType, ZDirFileEn_type pTypeToSelect=ZDFT_All);

    ZStatus dirApprox(::DirMap &pDirEntry, const utf8VaryingString &pApprox, ZDirFileEn_type pZDFT);
    ZStatus dirApproxAll(zbs::ZArray<::DirMap> &pDirArray,const utf8VaryingString& pApprox, ZDirFileEn_type pZDFT = ZDFT_All);

    ZStatus dirByName(::DirMap &pDirEntry, ZDirFileEn_type pZDFT, ZDirSortedByName &DSBN);

//    ZStatus dirByName( ZArray<DirMap> &DSBN, ZDirFileEn_type pZDFT);
    ZStatus dirBySize( ZArray<::DirMap> &DSBN, ZDirFileEn_type pZDFT);

    ZStatus dirByNameNext(::DirMap &pDirEntry);
    ZStatus dirByNameAll(ZDirSortedByName *pDirEntry, ZDirFileEn_type pZDFT);

    ZStatus dirBySize(uriString &pDirEntry, ZDirFileEn_type pZDFT);

    void closeDir(void);
    uriString getPath() { return uriString(*this); }
    bool exists() { return isDirectory(); }

    /** @brief setCurrent() sets this directory content as current directory */
    bool setCurrent();
    /** @brief setCurrentDirectory() this static method sets pDir as current directory without changing ZDir object content.*/
    static bool setCurrentDirectory(const char *pDir);

/**
 * @brief ZDir::mkdir this static method creates a new directory corresponding to pPath with access rights mode pMode
 * defaulted to S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH when pMode is omitted or set to zero.
 *
 *  For exception content see https://linux.die.net/man/3/mkdir
 *
 * @return a ZStatus ZS_SUCCESS when successfull.
 *  if an error occurs, ZException is positionned with the appropriate message preceeded by errno symbolic value.
 */
    static ZStatus mkdir(const uriString&pPath, __mode_t pMode=0);

    using uriString::operator=;
    using uriString::operator+=;
    using uriString::operator+;
    using uriString::operator==;

    ZDir& operator= (ZDir &pIn) { return _copyFrom(pIn); }
    ZDir& operator= (ZDir&&pIn) { return _copyFrom(pIn); }

private:
    DIR *_SystDir = nullptr;
    ZDirFileEn_type ZDFT = ZDFT_All;
    ZDirSortedByName *DSBN = nullptr;
    long  DSBNIdx = -1;
};



class ZFileNameSelection {
public:
    ZFileNameSelection() {}
    ZFileNameSelection(const ZFileNameSelection& pIn) {_copyFrom(pIn);}
    ZFileNameSelection& _copyFrom(const ZFileNameSelection& pIn);
    void clear() {
        SelRoot.clear();
        SelExt.clear();
        ExtFlag = RootFlag = ZDir::Nothing;
    }


    utf8VaryingString SelRoot;
    ZDir::DENM_type RootFlag;
    utf8VaryingString SelExt;
    ZDir::DENM_type ExtFlag;
};

class ZFileSelection : public ZArray<ZFileNameSelection>
{
public:
    ZFileSelection() {}
    ZFileSelection(const ZFileSelection& pIn) {
        _copyFrom(pIn);
    }
    ZFileSelection& _copyFrom(const ZFileSelection& pIn);

    void clear();

    ZFileSelection& addSelPhrase(const utf8VaryingString& pSelPhrase);
    ZFileSelection& setSelPhrase(const utf8VaryingString& pSelPhrase);

    void includeDirectories() {
        BaseTypeMask |= ZDFT_Directory ;
    }
    void includeAll() {
        BaseTypeMask |= ZDFT_All ;
    }
    void includeHidden() {
        BaseTypeMask |= ZDFT_Hidden ;
    }
    void includeBaseTypes() {
        BaseTypeMask |= (ZDFT_RegularFile | ZDFT_SymbolicLink) ;
    }

    ZFileSelection& operator=(const ZFileSelection& pIn) { return _copyFrom(pIn); }

    ZFileSelection& operator << (const utf8VaryingString& pSelPhrase) ;

    static void parseTerm(utf8_t *wPtr, utf8VaryingString&   pSelStr, ZDir::DENM_type&     pSelFlag);
    static bool parseSingleFileNameSelection(utf8_t* pPtr, ZFileNameSelection& pFileNameSelection);
    static void parseFileNameSelections(const utf8VaryingString& wSelContent, ZFileSelection& pSelection);

    utf8VaryingString display() { return Phrase; }

    utf8VaryingString dump();


    utf8VaryingString Phrase;
    ZDirFileEn_type BaseTypeMask = ZDFT_Nothing  ;
};

const char* decode_ZDFT(ZDirFileEn_type pType);
const utf8VaryingString decode_DENM(ZDir::DENM_type pType);


} // namespace zbs

#ifdef __USE_WINDOWS__
bool isDirectory(const char* dirName);
bool isRegularFile(const char* dirName);
#endif // __USE_WINDOWS__


#endif // ZDIR_H
