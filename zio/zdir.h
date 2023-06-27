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
#include <ztoolset/zutfstrings.h>
#include <ztoolset/uristring.h>

#ifndef ZDIR_FILE_TYPE
#define ZDIR_FILE_TYPE
enum ZDir_File_type {
    ZDFT_Nothing        = 0,
    ZDFT_RegularFile    = 1,
    ZDFT_Directory      = 2,
    ZDFT_All            = 0x0f
};
#endif //ZDIR_FILE_TYPE


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
        Size = 0;
        Uid = 0;
        Created.clear();
        Modified.clear();
        Type=0;
    }

    DirMap &operator=(const DirMap &pIn) { _copyFrom(pIn); return *this;}
    DirMap &operator=(const DirMap &&pIn) { _copyFrom(pIn); return *this;}

    uriString           Name;
    size_t              Size;

    ZSystemUserId       Uid;
    ZDateFull           Created;
    ZDateFull           Modified;
    ZDirFileEn_type     Type;
}; // DirMap



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
    ZStatus setPath (const utf8String& pPath) ;

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

    ZStatus fullDir( ::DirMap &pDirEntry, ZDirFileEn_type pZDFT=ZDFT_All);
    ZStatus fullDirNext(::DirMap &pDirEntry);

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
//    utf8_t Path[cst_urilen+1];

};

#ifdef __USE_WINDOWS__
bool isDirectory(const char* dirName);
bool isRegularFile(const char* dirName);
#endif // __USE_WINDOWS__

#endif // ZDIR_H
