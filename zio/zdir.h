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

#ifndef ZDIR_FILE_TYPE
#define ZDIR_FILE_TYPE
enum ZDir_File_type {
    ZDFT_Nothing        = 0,
    ZDFT_RegularFile    = 1,
    ZDFT_Directory      = 2,
    ZDFT_All            = 0x0f
};
#endif //ZDIR_FILE_TYPE
class ZDir
{
public:

    ZDir (void) {}
    ZDir (const utf8_t *pDirName) ;

    ~ZDir() {if (Dir!=nullptr)closedir(Dir);}

    ZStatus setPath (const utf8_t*pPath) ;
    ZStatus setPath (const uriString& pPath);
    ZStatus setPath (const uriString&& pPath);

    ZStatus readDir( uriString &pDirEntry, ZDir_File_type pZDFT=ZDFT_All);
    ZStatus readDirApprox(uriString &pDirEntry,const utf8_t* pApprox, ZDir_File_type pZDFT);
    void closeDir(void);
    uriString getPath();

private:
    DIR *Dir;
    utf8_t Path[cst_urilen+1];

};

#ifdef __USE_WINDOWS__
bool isDirectory(const char* dirName);
bool isRegularFile(const char* dirName);
#endif // __USE_WINDOWS__

#endif // ZDIR_H
