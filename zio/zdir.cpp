#ifndef ZDIR_CPP
#define ZDIR_CPP

#include <zio/zdir.h>
#include <ztoolset/uristring.h>


ZDir::ZDir (const utf8_t *pDirName)
{    
    if (setPath(pDirName)!=ZS_SUCCESS)
                ZException.exit_abort();
}

ZStatus
ZDir::setPath(const utf8_t *pPath)
{
    memset(Path,0,sizeof(Path));
    utfStrcpy<utf8_t> (Path,pPath);
    Dir=opendir((const char*)Path);
    if (Dir==nullptr)
         {
         ZException.getErrno(-1,
                             _GET_FUNCTION_NAME_,
                             ZS_NOTDIRECTORY,
                             Severity_Error,
                             "Error setting directory path to <%s>",
                             Path);
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
uriString ZDir::getPath()
{
    return uriString (Path);
}

void
ZDir::closeDir(void)
{
    closedir(Dir);
    Dir=nullptr;
    memset(Path,0,sizeof(Path));
    return;
}


ZStatus
ZDir::readDir(uriString &pDirEntry, ZDir_File_type pZDFT)
{
int wRet=0;
struct dirent* wDirEntry;
ZStatus wSt;

    if (Dir==nullptr)
            {
            wSt=setPath(Path);
            if (wSt!=ZS_SUCCESS)
                {
                    Dir=nullptr;
                    return wSt;
                }
            }

    while (wRet==0)
    {
//    wRet=readdir_r(Dir,wDirEntry,&wDirEntry);
    wDirEntry=readdir(Dir);
    if ((wDirEntry!=nullptr))
        {
            pDirEntry.clear();
            pDirEntry = Path;
            pDirEntry.addConditionalDirectoryDelimiter();
            pDirEntry +=(const utf8_t*)wDirEntry->d_name;
            switch (pZDFT)
            {
            case ZDFT_All :
                    return ZS_SUCCESS;
            case ZDFT_RegularFile:
                {
                if (!pDirEntry.isRegularFile())
                                continue;
                return ZS_SUCCESS;
                }
            case ZDFT_Directory:
                {
                if (!pDirEntry.isDirectory())
                                continue;
                return ZS_SUCCESS;
                }
            default:
                continue;
            }// switch
        }
        else
            {
            if (Dir!=nullptr)
                    closeDir();
            Dir=nullptr;
            return ZS_EOF;
            }
        ZException.getErrno(wRet,
                            _GET_FUNCTION_NAME_,
                            ZS_FILEERROR,
                            Severity_Error,
                            "Error scanning directory <%s>",Path);
        closeDir();
        Dir=nullptr;
        return ZS_FILEERROR;
    } // while wRet==0
    return ZS_SUCCESS;
}//readdir

ZStatus
ZDir::readDirApprox(uriString &pDirEntry,const utf8_t* pApprox, ZDir_File_type pZDFT)
{
int wRet=0;
struct dirent* wDirEntry;

ZStatus wSt;

    if (Dir==nullptr)
            {
            wSt=setPath(Path);
            if (wSt!=ZS_SUCCESS)
                {
                    Dir=nullptr;
                    return wSt;
                }
            }

    while (wRet==0)
    {
    wRet=readdir_r(Dir,wDirEntry,&wDirEntry);
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
            Dir=nullptr;
            return ZS_EOF;
            }
        ZException.getErrno(wRet,
                            _GET_FUNCTION_NAME_,
                            ZS_FILEERROR,
                            Severity_Error,
                            "Error scanning directory <%s>",Path);
        closeDir();
        Dir=nullptr;
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



#endif // ZDIR_CPP
