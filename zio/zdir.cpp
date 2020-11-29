#ifndef ZDIR_CPP
#define ZDIR_CPP

#ifdef QT_CORE_LIB
#include <QString>
#endif

#include <zio/zdir.h>
#include <ztoolset/uristring.h>

#include <map>

#include <ztoolset/zarray.h>


ZDir::ZDir (const utf8_t *pDirName)
{
    if (setPath((const char *)pDirName)!=ZS_SUCCESS)
                ZException.exit_abort();
}
ZStatus ZDir::setPath(const uriString &pPath)
{
    return setPath((const char *)pPath.content);
}
ZStatus ZDir::setPath(const uriString &&pPath)
{
    return setPath((const char *)pPath.content);
}

int ZDir::countElements(uriString &pDirEntry,ZDir_File_type pZDFT)
{
  ZDir wDir(pDirEntry);
  uriString wEntry;
  ZStatus wSt=ZS_SUCCESS;
  int wCount=0;
  while ((wSt=wDir.dir(wEntry,pZDFT))==ZS_SUCCESS)
    wCount++;

    return wCount;
}//count
#ifdef QT_CORE_LIB
int ZDir::countElements(const QString pDirEntry, ZDir_File_type pZDFT)
{
  uriString wEntry;
  wEntry=pDirEntry;
  return ZDir::countElements(wEntry,pZDFT);
}//count

ZStatus
ZDir::setPath(const QString pPath)
{
  this->strset((const utf8_t*)pPath.toUtf8().data());
  _SystDir=opendir((const char*)content);
  if (_SystDir==nullptr)
  {
    ZException.getErrno(-1,
        _GET_FUNCTION_NAME_,
        ZS_NOTDIRECTORY,
        Severity_Error,
        "Error setting directory path to <%s>",
        content);
    return ZS_NOTDIRECTORY;
  }
  return ZS_SUCCESS;
}
#endif //QT_CORE_LIB

ZStatus
ZDir::setPath(const char *pPath)
{
//    memset(Path,0,sizeof(Path));
//    utfStrcpy<utf8_t> (Path,pPath);

    this->strset((const utf8_t*)pPath);
    _SystDir=opendir((const char*)content);
    if (_SystDir==nullptr)
         {
         ZException.getErrno(-1,
                             _GET_FUNCTION_NAME_,
                             ZS_NOTDIRECTORY,
                             Severity_Error,
                             "Error setting directory path to <%s>",
                             content);
         return ZS_NOTDIRECTORY;
         }
    return ZS_SUCCESS;
}
ZStatus setPath (const uriString& pPath)
{
    return setPath(pPath.toUtf());
}
ZStatus setPath (const uriString&& pPath)
{
    return setPath(pPath.toUtf());
}
void
ZDir::closeDir(void)
{
    if (_SystDir)
        closedir(_SystDir);
    _SystDir=nullptr;
    return;
}

#include <sys/types.h>
#include <sys/stat.h>

ZStatus ZDir::mkdir(const char *pPath)
{
    __mode_t wMode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    return ZDir::mkdir(pPath, wMode);
}


ZStatus ZDir::mkdir(const char *pPath, __mode_t pMode)
{
    int wRet = ::mkdir(pPath, pMode);

    return ZS_SUCCESS;
    if (wRet) {
        ZException.getErrno(-1,
                            _GET_FUNCTION_NAME_,
                            ZS_FILEERROR,
                            Severity_Error,
                            "Error creating directory path  <%s>",
                            pPath);
        return ZS_FILEERROR;
    }
    return ZS_SUCCESS;
}

void ZDir::_closeReadDir()
{
    if (_SystDir)
        closeDir();
    _SystDir = nullptr;
    return;
}




ZStatus
ZDir::dir(uriString &pDirEntry, ZDir_File_type pZDFT)
{
    ZDFT = pZDFT;
int wRet=0;
struct dirent* wDirEntry;
ZStatus wSt;

    if (_SystDir==nullptr)
            {
            wSt=setPath(content);
            if (wSt!=ZS_SUCCESS)
                {
                    _SystDir=nullptr;
                    return wSt;
                }
            }

    while (wRet == 0) {
        //      wRet=readdir_r(Dir,wDirEntry,&wDirEntry);
        /*
It is recommended that applications use readdir(3) instead of
       readdir_r().  Furthermore, since version 2.24, glibc deprecates
       readdir_r().
*/
        errno = 0;
        wDirEntry = readdir(_SystDir);
        wRet = errno; /* save errno */
        if (wDirEntry == nullptr) // error (errno is then set) or end of directory entry list
            break;
        if ( strcmp(wDirEntry->d_name,".")==0) /*  skip unix two 'dot - dotdot' files */
          continue;
        if ( strcmp(wDirEntry->d_name,"..")==0)
          continue;
        pDirEntry.clear();
        pDirEntry = *this;
        pDirEntry.addConditionalDirectoryDelimiter();
        pDirEntry += (const utf8_t *) wDirEntry->d_name;
        switch (ZDFT) {
        case ZDFT_All:
            return ZS_SUCCESS;
        case ZDFT_RegularFile: {
            if (!pDirEntry.isRegularFile())
                continue;
            return ZS_SUCCESS;
        }
        case ZDFT_Directory: {
            if (!pDirEntry.isDirectory())
                continue;
            return ZS_SUCCESS;
        }
        default:
            continue;
        } // switch

    } // while wRet==0


    if (wRet)
        {
        ZException.getErrno(wRet,
                            _GET_FUNCTION_NAME_,
                            ZS_FILEERROR,
                            Severity_Error,
                            "Error scanning directory <%s>",
                            content);
        wSt = ZS_FILEERROR;
    } else
        wSt = ZS_EOF;

    if (_SystDir)
        closeDir();
    _SystDir = nullptr;
    /* On success, readdir() returns a pointer to a dirent structure.  (This
       structure may be statically allocated; do not attempt to free(3) it.)*/
    //  NO        _free(wDirEntry);


    return wSt;
} //readdir
typedef uint8_t DirInfo_type;
enum DirInfo : uint8_t {
    DIRI_Name = 1,
    DIRI_Size = 2,
    DIRI_Owner = 4,
    DIRI_Dates = 8,

    DIRI_Full = DIRI_Name | DIRI_Size | DIRI_Owner | DIRI_Dates
};



ZStatus ZDir::fullDir(DirMap &pDirEntry, ZDir_File_type pZDFT)
{
    uriStat wStat;
    ZDFT = pZDFT;
    int wRet = 0;
    struct dirent *wDirEntry;
    ZStatus wSt;

    if (_SystDir == nullptr) {
        wSt = setPath(content);
        if (wSt != ZS_SUCCESS) {
            _SystDir = nullptr;
            return wSt;
        }
    }

    while (wRet == 0) {
        //      wRet=readdir_r(Dir,wDirEntry,&wDirEntry);
        /*
It is recommended that applications use readdir(3) instead of
       readdir_r().  Furthermore, since version 2.24, glibc deprecates
       readdir_r().
*/
        errno = 0;
        wDirEntry = readdir(_SystDir);
        wRet = errno; /* save errno */
        if (wDirEntry == nullptr) // error or end of directory
            break;
        if ( strcmp(wDirEntry->d_name,".")==0) /*  skip unix two 'dot - dotdot' files */
          continue;
        if ( strcmp(wDirEntry->d_name,"..")==0)
          continue;

        switch (ZDFT) {
        case ZDFT_All:
            break;
        case ZDFT_RegularFile: {
            if (!pDirEntry.Name.isRegularFile())
                continue;
            break;
        }
        case ZDFT_Directory: {
            if (!pDirEntry.Name.isDirectory())
                continue;
            break;
        }
        default:
            continue;
        } // switch

        pDirEntry.Name.clear();
        pDirEntry.Name = *this;
        pDirEntry.Name.addConditionalDirectoryDelimiter();
        pDirEntry.Name += (const utf8_t *) wDirEntry->d_name;

        pDirEntry.Name.getStatR(wStat);

        pDirEntry.Size = wStat.Size;
        pDirEntry.Uid = wStat.Uid;
        pDirEntry.Created = wStat.Created;
        pDirEntry.Modified = wStat.LastModified;

        return ZS_SUCCESS;
    } // while wRet==0


    if (wRet)
    {
        ZException.getErrno(wRet,
                            _GET_FUNCTION_NAME_,
                            ZS_FILEERROR,
                            Severity_Error,
                            "Error scanning directory <%s>",
                            content);
        wSt = ZS_FILEERROR;
    } else
        wSt = ZS_EOF;

    if (_SystDir)
        closeDir();
    _SystDir = nullptr;
    /* On success, readdir() returns a pointer to a dirent structure.  (This
       structure may be statically allocated; do not attempt to free(3) it.)*/
    //  NO        _free(wDirEntry);


    return wSt;
} //fulldir

ZStatus ZDir::dirNext(uriString &pDirEntry)
{
    return dir(pDirEntry, ZDFT);
}

ZStatus ZDir::dirAll(zbs::ZArray<uriString> &pDirArray, ZDir_File_type pZDFT)
{
    pDirArray.clear();
    uriString wDirEntry;

    ZStatus wSt = dir(wDirEntry, pZDFT);
    while (wSt == ZS_SUCCESS) {
        pDirArray.push(wDirEntry);
        wSt = dir(wDirEntry, pZDFT);
    }
    if (wSt == ZS_EOF)
        wSt = ZS_SUCCESS;
    return wSt;
}


ZStatus ZDir::fullDirAll(zbs::ZArray<DirMap> &pDirArray, ZDir_File_type pZDFT)
{
    pDirArray.clear();
    DirMap wDirEntry;


}


class ZDirSortedByName : public zbs::ZArray<DirMap>
{
public:
    ZDirSortedByName() = default;

    using zbs::ZArray<DirMap>::operator[];

    long add(const DirMap &pIn)
    {
        long wi = 0;
        while ((Tab[wi].Name.compare(pIn.Name) < 0) && (wi < count()))
            wi++;
        if (wi < count())
            insert(pIn, wi);
        else
            push(pIn);
        return wi;
    }

    long add(const uriString &pName,
             size_t pSize,
             userid_type pUid) { return add(DirMap(pName, pSize,pUid)); }
};
class ZDirSortedBySize : public zbs::ZArray<DirMap>
{
public:
    ZDirSortedBySize() = default;


    long add(const DirMap &pIn)
    {
        long wi = 0;
        while ((Tab[wi].Size < pIn.Size) && (wi < count()))
            wi++;
        if (wi < count())
            insert(pIn, wi);
        else
            push(pIn);
    }
    long add(const uriString &pName, const size_t pSize,userid_type pUid) { return add(DirMap(pName, pSize,pUid)); }
};



ZStatus ZDir::dirByName(DirMap &pDirEntry, ZDir_File_type pZDFT)
{
    if (DSBN)
        delete DSBN;

    DSBN = new ZDirSortedByName;
    ZStatus wSt;
    uriString wElt;
    uriString wFull;
    uriStat wStat;
    wSt = dir(wElt, pZDFT);
    while (wSt == ZS_SUCCESS) {
        wFull = content;
        wFull.addConditionalDirectoryDelimiter();
        wFull += wElt;
        wSt = wFull.getStatR(wStat);
        DSBN->add(DirMap(wElt, wStat.Size,wStat.Uid));
        wSt = dirNext(wElt);
    }
    if (wSt != ZS_EOF)
        return wSt;

    if (DSBN->count() > 0) {
        DSBNIdx = 0;
        pDirEntry = DSBN->Tab[DSBNIdx];
        return ZS_SUCCESS;
    }
    return ZS_EOF;
} //readDirSortedByName

ZStatus ZDir::dirByNameNext(DirMap &pDirEntry)
{
    DSBNIdx ++;
    if (DSBN->count() > DSBNIdx)
        {
        pDirEntry = DSBN->Tab[DSBNIdx];
        return ZS_SUCCESS;
        }
    return ZS_EOF;
}

ZStatus
ZDir::dirApprox(uriString &pDirEntry,const utf8_t* pApprox, ZDir_File_type pZDFT)
{
int wRet=0;
struct dirent* wDirEntry;

ZStatus wSt;

    if (_SystDir==nullptr)
            {
            wSt=setPath(content);
            if (wSt!=ZS_SUCCESS)
                {
                    _SystDir=nullptr;
                    return wSt;
                }
            }

    while (wRet==0)
    {
    wRet=readdir_r(_SystDir,wDirEntry,&wDirEntry);
    if ((wDirEntry!=nullptr)&&(wRet=0))
        {
            pDirEntry.clear();
            pDirEntry =(const utf8_t*)wDirEntry->d_name;
            switch (pZDFT)
            {
            case ZDFT_All :
                    return ZS_SUCCESS;
            case ZDFT_RegularFile:
                {
                if (!pDirEntry.isRegularFile())
                                continue;
                if (!pDirEntry.startsWith((const unsigned char*)pApprox))
                                continue;
                return ZS_SUCCESS;
                }
            case ZDFT_Directory:
                {
                if (!pDirEntry.isDirectory())
                                continue;
                if (!pDirEntry.startsWith(pApprox))
                                continue;
                return ZS_SUCCESS;
                }
            default:
                continue;
            }// switch
        }
        if (wRet==0)
            {
            closeDir();
            _SystDir=nullptr;
            return ZS_EOF;
            }
        ZException.getErrno(wRet,
                            _GET_FUNCTION_NAME_,
                            ZS_FILEERROR,
                            Severity_Error,
                            "Error scanning directory <%s>",content);
        closeDir();
        _SystDir=nullptr;
        return ZS_FILEERROR;
    } // while wRet==0
    return ZS_SUCCESS;
}//readdir
#ifdef __USE_WINDOWS__

bool isDirectory(const char* dirName) {
  DWORD attribs = ::GetFileAttributesA(dirName);
  if (attribs == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}
bool isRegularFile(const char* dirName) {
  DWORD attribs = ::GetFileAttributesA(dirName);
  if (attribs == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}
#endif


bool ZDir::setCurrent()
{
    return chdir(toCChar()) == 0;
}

bool ZDir::setCurrentDirectory(const char *pDir)
{
    return chdir(pDir) == 0;
}

#endif // ZDIR_CPP
