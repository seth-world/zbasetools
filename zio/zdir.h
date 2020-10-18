#ifndef ZDIR_H
#define ZDIR_H
#include <zconfig.h>

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


struct DirMap
{
    DirMap() = default;
    DirMap(const DirMap &pIn) { _copyFrom(pIn); }
    DirMap(const DirMap &&pIn) { _copyFrom(pIn); }
    DirMap(const uriString &pName, const size_t pSize, ZSystemUserId pUid)
    {
        clear();
        Name = pName;
        Size = pSize;
        Uid = pUid;
    }
    DirMap(const uriString &pName,
           const size_t pSize,
           userid_type pUid,
           const ZDateFull &pCreated,
           const ZDateFull &pModified)
    {
        clear();
        Name = pName;
        Size = pSize;
        Uid = ZSystemUserId(pUid);
        Created = pCreated;
        Modified = pModified;
    }

    DirMap &_copyFrom(const DirMap &pIn)
    {
        clear();
        Name = pIn.Name;
        Size = pIn.Size;
        Uid = pIn.Uid;
        Created = pIn.Created;
        Modified = pIn.Modified;
        return *this;
    }

    void clear()
    {
        Name.clear();
        Size = 0;
        Uid = 0;
        Created.clear();
        Modified.clear();
    }

    DirMap &operator=(const DirMap &pIn) { _copyFrom(pIn); }
    DirMap &operator=(const DirMap &&pIn) { _copyFrom(pIn); }

    uriString           Name;
    size_t              Size;
    ZSystemUserId       Uid;
    ZDateFull           Created;
    ZDateFull           Modified;

}; // DirMap



class ZDir : public uriString
{
public:

    ZDir (void) {}
    ZDir (const utf8_t *pDirName) ;
    ZDir(const ZDir &pIn) { _copyFrom(pIn); }
    ZDir(const ZDir &&pIn) { _copyFrom(pIn); }

    ZDir(const uriString &pIn) { setPath(pIn); }

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

    ZStatus setPath (const char*pPath) ;
    ZStatus setPath (const uriString& pPath);
    ZStatus setPath (const uriString&& pPath);

    /** @brief readDir() */
    ZStatus dir( uriString &pDirEntry, ZDir_File_type pZDFT=ZDFT_All);
    ZStatus dirNext(uriString &pDirEntry);
    void _closeReadDir();

    ZStatus dirAll(zbs::ZArray<uriString> &pDirArray, ZDir_File_type pZDFT = ZDFT_All);

    ZStatus fullDirAll(zbs::ZArray<DirMap> &pDirArray, ZDir_File_type pZDFT = ZDFT_All);

    ZStatus fullDir( DirMap &pDirEntry, ZDir_File_type pZDFT=ZDFT_All);
    ZStatus fullDirNext(DirMap &pDirEntry);

    ZStatus dirApprox(uriString &pDirEntry,const utf8_t* pApprox, ZDir_File_type pZDFT);
    ZStatus dirApproxNext(uriString &pDirEntry);

    ZStatus dirByName(DirMap &pDirEntry, ZDir_File_type pZDFT);
    ZStatus dirByNameNext(DirMap &pDirEntry);
    ZStatus dirByNameAll(ZDirSortedByName *pDirEntry, ZDir_File_type pZDFT);

    ZStatus dirBySize(uriString &pDirEntry, ZDir_File_type pZDFT);

    void closeDir(void);
    uriString getPath() { return uriString(*this); }
    bool exists() { return isDirectory(); }

    /** @brief setCurrent() sets this directory content as current directory */
    bool setCurrent();
    /** @brief setCurrentDirectory() this static method sets pDir as current directory without changing ZDir object content.*/
    static bool setCurrentDirectory(const char *pDir);

    static ZStatus mkdir(const char *pPath);
    static ZStatus mkdir(const char *pPath, __mode_t pMode);

    using uriString::operator=;
    using uriString::operator+=;
    using uriString::operator+;
    using uriString::operator==;

    ZDir& operator= (ZDir &pIn) { return _copyFrom(pIn); }
    ZDir& operator= (ZDir&&pIn) { return _copyFrom(pIn); }

private:
    DIR *_SystDir = nullptr;
    ZDir_File_type ZDFT = ZDFT_All;
    ZDirSortedByName *DSBN = nullptr;
    long  DSBNIdx = -1;
//    utf8_t Path[cst_urilen+1];

};

#ifdef __USE_WINDOWS__
bool isDirectory(const char* dirName);
bool isRegularFile(const char* dirName);
#endif // __USE_WINDOWS__

#endif // ZDIR_H
