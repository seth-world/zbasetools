#ifndef ZDIR_CPP
#define ZDIR_CPP


#include "zdir.h"

#ifdef QT_CORE_LIB
#include <QString>
#endif

/* for mode_t */
#include <sys/types.h>
#include <sys/stat.h>

#include <ztoolset/uristring.h>

#include <map>

#include <ztoolset/zarray.h>

namespace zbs {

ZDir::ZDir (const utf8_t *pDirName)
{
    if (setPath(pDirName)!=ZS_SUCCESS)
                ZException.exit_abort();
}
/*ZStatus ZDir::setPath(const uriString &pPath)
{
    return setPath((const char *)pPath.content);
}
ZStatus ZDir::setPath(const uriString &&pPath)
{
    return setPath((const char *)pPath.content);
}*/

int ZDir::countElements(uriString &pDirEntry,ZDirFileEn_type pZDFT)
{
  ZDir wDir(pDirEntry);
  uriString wEntry;
  ZStatus wSt=ZS_SUCCESS;
  int wCount=0;
  while ((wSt=wDir.dir(wEntry,pZDFT))==ZS_SUCCESS)
    wCount++;

    return wCount;
}//count


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
ZStatus
ZDir::setPath(const utf8VaryingString &pPath)
{
    errno=0; /* errno is set by this function */
    strset(pPath.toUtf());
    _SystDir=opendir(toCChar());
    if (_SystDir==nullptr) {
      int wErrno=errno;
      ZStatus wSt=ZS_NOTDIRECTORY;
      utf8VaryingString wErrMsg;
      switch (wErrno) {
      case EBADF:
        wSt= ZS_BADFILEDESC;
        wErrMsg.sprintf( "<EBADF> fd is not a valid open file descriptor for  <%s>.",toString());
        break;

      case ENOENT:
        wSt= ZS_NOTDIRECTORY;
        wErrMsg.sprintf( "<ENOENT> Directory <%s> does not exist, or name is an empty string.",toString());
        break;
      case EACCES:
        wSt= ZS_ACCESSRIGHTS;
        wErrMsg.sprintf( "<EACCES> Search permission is denied for one of the directories in the path prefix of <%s>.",toString());
        break;
      case ENOMEM:
        wSt= ZS_MEMERROR;
        wErrMsg.sprintf( "<ENOMEM> Out of memory (i.e., kernel memory) for directory <%s>.",toString());
        break;
      case EMFILE:
        wSt= ZS_OUTBOUNDHIGH;
        wErrMsg.sprintf( "<EMFILE> The per-process limit on the number of open file descriptors has been reached for file <%s>.",toString());
        break;
      case ENFILE:
        wSt= ZS_OUTBOUNDHIGH;
        wErrMsg.sprintf( "<ENFILE> The system-wide limit on the total number of open files has been reached. for file <%s>.",toString());
        break;

      case ENOTDIR:
        wSt= ZS_NOTDIRECTORY;
        wErrMsg.sprintf( "<ENOTDIR> Name is not a directory. File <%s>.",toString());
        break;

      default:
        wErrMsg.sprintf( "Unknown error for directory file <%s>.",toString());
        break;
      }// switch

      ZException.getErrno(wErrno, /* saved errno */
                          _GET_FUNCTION_NAME_,
                          wSt,
                          Severity_Error,
                          wErrMsg.toCChar());
      return wSt;
    } // if (wRet < 0)
    return ZS_SUCCESS;
}
/*ZStatus setPath (const uriString& pPath)
{
    return setPath(pPath.toUtf());
}
ZStatus setPath (const uriString&& pPath)
{
    return setPath(pPath.toUtf());
}*/
void
ZDir::closeDir(void)
{
    if (_SystDir)
        closedir(_SystDir);
    _SystDir=nullptr;
    return;
}


/**
 * @brief ZDir::mkdir this static method creates a new directory corresponding to pPath with access mode pMode
 *  For exception content see https://linux.die.net/man/3/mkdir
 *
 * @return a ZStatus ZS_SUCCESS when successfull.
 *  if an error occurs, ZException is positionned with the appropriate message preceeded by errno symbolic value.
 */
ZStatus ZDir::mkdir(const uriString &pPath, __mode_t pMode)
{
  if (pMode == 0)
    pMode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

  errno=0; /* errno is set by this function */
  int wRet = ::mkdir(pPath.toCChar(), pMode);

  if (wRet < 0) {
    int wErrno=errno;
    ZStatus wSt=ZS_NOTDIRECTORY;
    utf8VaryingString wErrMsg;
    wSt= ZS_FILEERROR;
    switch (wErrno) {
    case EACCES:
      wSt= ZS_ACCESSRIGHTS;
      wErrMsg.sprintf( "<EACCES> Search permission is denied for one of the directories in the path prefix or write permission is denied on the parent directory of the directory to be created for <%s>.",pPath.toString());
      break;
    case ENOTDIR:
      wSt= ZS_INVPARAMS;
      wErrMsg.sprintf( "<ENOTDIR> A component of the path prefix is not a directory.Path <%s>.",pPath.toString());
      break;
    case ENOENT:
      wSt= ZS_NOTDIRECTORY;
      wErrMsg.sprintf( "<ENOENT> A component of the path prefix specified by path does not name an existing directory or path is an empty string.Directory <%s> ",pPath.toString());
      break;
    case EEXIST:
      wSt= ZS_FILEEXIST;
      wErrMsg.sprintf( "<EEXIST> The named file exists. Directory  <%s>.",pPath.toString());
      break;
    case ELOOP:
      wSt= ZS_ILLEGAL;
      wErrMsg.sprintf( "<ELOOP> A loop exists in symbolic links encountered during resolution of the path argument. for directory <%s>.",pPath.toString());
      break;
    case EMLINK:
      wSt= ZS_OUTBOUNDHIGH;
      wErrMsg.sprintf( "<EMLINK> The link count of the parent directory would exceed {LINK_MAX} for directory <%s>.",pPath.toString());
      break;
    case ENAMETOOLONG:
      wSt= ZS_INVNAME;
      wErrMsg.sprintf( "<ENAMETOOLONG> The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX} for directory <%s>.",pPath.toString());
      break;
    case ENOSPC:
      wSt= ZS_MEMOVFLW;
      wErrMsg.sprintf( "<ENOSPC> The file system does not contain enough space to hold the contents of the new directory or to extend the parent directory of the new directory <%s>.",pPath.toString());
      break;
    case EROFS:
      wSt= ZS_ACCESSRIGHTS;
      wErrMsg.sprintf( "<EROFS> The parent directory resides on a read-only file system. Directory <%s>.",pPath.toString());
      break;

    default:
      wErrMsg.sprintf( "Unknown error for directory file <%s>.",pPath.toString());
      break;
    }// switch
    ZException.getErrno(wErrno, /* saved errno */
        _GET_FUNCTION_NAME_,
        wSt,
        Severity_Error,
        wErrMsg.toCChar());
    return wSt;
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
ZDir::dir(uriString &pDirEntry, ZDirFileEn_type pRequestedType)
{
    ZDFT = pRequestedType;
int wErrno=0;
struct dirent* wDirEntry=nullptr;
ZStatus wSt=ZS_SUCCESS;

    if (_SystDir==nullptr) {
      wSt=setPath(Data);
      if (wSt!=ZS_SUCCESS) {
        _SystDir=nullptr;
        return wSt;
      }
    }

    while (true) {
/*
It is recommended that applications use readdir(3) instead of
       readdir_r().  Furthermore, since version 2.24, glibc deprecates
       readdir_r().
*/
        errno = 0;
        wDirEntry = readdir(_SystDir);
        wErrno = errno; /* save errno */
        if (wDirEntry == nullptr) // error (errno is then set) or end of directory entry list (errno remains to 0)
            break;
        if ((wDirEntry->d_name[0]=='.')&&(wDirEntry->d_name[1]=='.'))/*  skip unix two 'dot - dotdot' files */
          continue;
        if ((wDirEntry->d_name[0]=='.') && !(pRequestedType & ZDFT_Hidden))
          continue;

/*
  see https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
*/

        switch (wDirEntry->d_type) {
        case DT_REG: /* regular file */
          if (ZDFT & ZDFT_RegularFile) {
            break;
          }
          continue;
        case DT_DIR: /* directory */
          if (ZDFT & ZDFT_Directory)
            break;
          continue;
        case DT_LNK: /* symbolic link */
          if (ZDFT & ZDFT_SymbolicLink)
            break;
          continue;
        default:
          if ((ZDFT & ZDFT_All) == ZDFT_All)/* if all entries selected then store */
            break;
          continue; /* else skip */
        } // switch


        pDirEntry.clear();
        pDirEntry = *this;
        pDirEntry.addConditionalDirectoryDelimiter();
        pDirEntry += (const utf8_t *) wDirEntry->d_name;
        return ZS_SUCCESS;
    } // while wDirEntry != nullptr


    if (wErrno!=0) {

        ZStatus wSt=ZS_NOTDIRECTORY;
        utf8VaryingString wErrMsg;

        switch (wErrno) {
        case EBADF:
          wSt= ZS_BADFILEDESC;
          wErrMsg.sprintf( "<EBADF> Invalid directory stream descriptor for <%s>.",toString());
          break;

        default:
          wSt=ZS_NOTDIRECTORY;
          wErrMsg.sprintf( "Unknown error for directory file <%s>.",toString());
          break;
        }// switch
        ZException.getErrno(wErrno,
            _GET_FUNCTION_NAME_,
            wSt,
            Severity_Error,
            wErrMsg.toCChar());
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


ZStatus ZDir::fullDir(DirMap &pDirEntry, ZDirFileEn_type pZDFT)
{
    uriStat wStat;
    struct dirent *wDirEntry;
    ZDirFileEn_type wZDFT;
    pDirEntry.clear();

    ZStatus wSt=_fullDir(wDirEntry,wZDFT,pZDFT);

    if (wSt==ZS_SUCCESS) {
        pDirEntry.BaseName = (const utf8_t*)wDirEntry->d_name;
        pDirEntry.Name = *this;
        pDirEntry.Name.addConditionalDirectoryDelimiter();
        pDirEntry.Name += pDirEntry.BaseName;
        if (pDirEntry.Name.getStatR(wStat) == ZS_SUCCESS) {
          pDirEntry.Size = wStat.Size;
          pDirEntry.Uid = wStat.Uid;
          pDirEntry.Created = wStat.Created;
          pDirEntry.Modified = wStat.LastModified;
          pDirEntry.Type = pZDFT;
        }
        return ZS_SUCCESS;
    } // ZS_SUCCESS

    return wSt;
} //fulldir

ZStatus ZDir::_fullDir(struct dirent* &wDirEntry, ZDirFileEn_type &pOutType, ZDirFileEn_type pTypeToSelect)
{
    ZDFT = pTypeToSelect;

    ZStatus wSt=ZS_SUCCESS;
    if (_SystDir == nullptr) {
        wSt = setPath(Data);
        if (wSt != ZS_SUCCESS) {
            _SystDir = nullptr;
            return wSt;
        }
    }

    pOutType=0;
    int wErrno = 0;
    while (wErrno == 0) {
        //      wRet=readdir_r(Dir,wDirEntry,&wDirEntry);
        /*
It is recommended that applications use readdir(3) instead of
       readdir_r().  Furthermore, since version 2.24, glibc deprecates
       readdir_r().
*/
        errno = 0;
        wDirEntry = readdir(_SystDir);
        wErrno = errno; /* save errno */
        if (wDirEntry == nullptr) // error or end of directory
            break;

        /*  skip unix two 'dot - dotdot' files */
        if ((wDirEntry->d_name[1]=='\0')&&(wDirEntry->d_name[0]=='.'))
            continue;
        if ((wDirEntry->d_name[1]!='\0')&&(wDirEntry->d_name[2]=='\0')&&(wDirEntry->d_name[0]=='.')&&(wDirEntry->d_name[1]=='.'))
            continue;


        if (wDirEntry->d_name[0]=='.') {
            if (!(pTypeToSelect & ZDFT_Hidden))
                continue;
            pOutType |= ZDFT_Hidden;
        }

        /*
  see https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
*/

        switch (wDirEntry->d_type) {
        case DT_REG: /* regular file */
            if (ZDFT & ZDFT_RegularFile) {
                pOutType |= ZDFT_RegularFile;
                break;
            }
            continue;
        case DT_DIR: /* directory */
            if (ZDFT & ZDFT_Directory){
                pOutType |= ZDFT_Directory;
                break;
            }
            continue;
        case DT_LNK: /* symbolic link */
            if (ZDFT & ZDFT_SymbolicLink){
                pOutType |= ZDFT_SymbolicLink;
                break;
            }
            continue;
        default:
            if ((ZDFT & ZDFT_All) == ZDFT_All)/* if all entries selected then store */{
                pOutType |= ZDFT_Other;
                break;
            }
            continue; /* else skip */
        } // switch
        return ZS_SUCCESS;
    } // while wRet==0


    if (wErrno) {
        ZStatus wSt=ZS_NOTDIRECTORY;
        utf8VaryingString wErrMsg;

        switch (wErrno) {
        case EBADF:
            wSt= ZS_BADFILEDESC;
            wErrMsg.sprintf( "<EBADF> Invalid directory stream descriptor for <%s>.",toString());
            break;

        default:
            wSt=ZS_NOTDIRECTORY;
            wErrMsg.sprintf( "Unknown error for directory file <%s>.",toString());
            break;
        }// switch
        ZException.getErrno(wErrno,
                            _GET_FUNCTION_NAME_,
                            wSt,
                            Severity_Error,
                            wErrMsg.toCChar());
    } else
        wSt = ZS_EOF;

    closeDir();
    /* On success, readdir() returns a pointer to a dirent structure.  (This
       structure may be statically allocated; do not attempt to free(3) it.)*/

    return wSt;
} //_fulldir

bool
matchFileElement(const utf8VaryingString& pFileElement,const utf8VaryingString& pSelRoot,ZDir::DENM_type pFlag)
{
    if (pFlag==ZDir::Nothing)
        return true;

    bool wCase = pFlag & ZDir::CaseRegardless ;
    pFlag &= ~ ZDir::CaseRegardless ;
    switch ( pFlag )
    {
    case ZDir::ExactMatch:
        if (wCase) {
            if (pFileElement.compareCase(pSelRoot)==0)
                return true;
            return false;
        }
        if (pFileElement.compare(pSelRoot)==0)
            return true;
        return false;
    case ZDir::StartsWith:
        if (wCase) {
            return pFileElement.startsCaseWith(pSelRoot.Data) ;
        }
        return pFileElement.startsWith(pSelRoot.Data) ;
    case ZDir::EndsWith:
        if (wCase) {
            return pFileElement.endsCaseWith(pSelRoot.Data);
        }
        return pFileElement.endsWith(pSelRoot.Data) ;

    case ZDir::Contains:
        if (wCase) {
            return pFileElement.containsCase(pSelRoot.Data);
        }
        return pFileElement.contains(pSelRoot.Data) ;
    default:
        return true;
    }
}

bool
matchFileName(const uriString& wBaseName,const ZFileSelection& pSelection)
{
    bool wSelected = false;
    for (int wi = 0 ; wi < pSelection.count();wi++) {
        if (matchFileElement(wBaseName.getRootname(),pSelection[wi].SelRoot,pSelection[wi].RootFlag)) {
            if (matchFileElement(wBaseName.getFileExtension(),pSelection[wi].SelExt,pSelection[wi].ExtFlag))
                return true;
        }
    }
    return false;
}


ZStatus
ZDir::fullDirSel(DirMap &pDirEntry, ZFileSelection& pSelection,ZDirFileEn_type pTypeToSelect)
{
    uriStat wStat;
    struct dirent *wDirEntry;
    pDirEntry.clear();
    ZStatus wSt = ZS_SUCCESS;

    while (wSt==ZS_SUCCESS) {
        wSt=_fullDir(wDirEntry,pSelection.BaseTypeMask,pTypeToSelect);

        if (wSt==ZS_SUCCESS) {
            uriString wName = (const utf8_t*)wDirEntry->d_name;
            pDirEntry.BaseName = wName;
            if(!matchFileName(wName,pSelection))
                continue;
            pDirEntry.Name = *this;
            pDirEntry.Name.addConditionalDirectoryDelimiter();
            pDirEntry.Name += pDirEntry.BaseName;
            if ((wSt=pDirEntry.Name.getStatR(wStat)) == ZS_SUCCESS) {
                pDirEntry.Size = wStat.Size;
                pDirEntry.Uid = wStat.Uid;
                pDirEntry.Created = wStat.Created;
                pDirEntry.Modified = wStat.LastModified;
                pDirEntry.Type = pSelection.BaseTypeMask;
            }
        } // ZS_SUCCESS
        return wSt;
    } // while ZS_SUCCESS
    return wSt;
} //ZDir::fullDirSel


ZStatus ZDir::dirNext(uriString &pDirEntry)
{
    return dir(pDirEntry, ZDFT);
}

ZStatus ZDir::dirAll(zbs::ZArray<uriString> &pDirArray, ZDirFileEn_type pZDFT)
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
/*
ZStatus ZDir::dirByName( ZArray<::DirMap> &DSBN, ZDirFileEn_type pZDFT)
{
  DSBN.clear();
  ::DirMap wDirEntry;
  ZStatus wSt = fullDir(wDirEntry, pZDFT);
  while (wSt == ZS_SUCCESS) {
    long wi=0;
    while (wDirEntry.Name.compareCase(DSBN.at(wi).Name) < 0)
      wi++;
    if (wi < DSBN.count())
      DSBN.insert(wDirEntry,wi);
    else
      DSBN.push(wDirEntry);
  wSt = fullDir(wDirEntry, pZDFT);
  } // while (wSt == ZS_SUCCESS)

  if (wSt == ZS_EOF)
    wSt = ZS_SUCCESS;
  return wSt;
}
*/
ZStatus ZDir::dirBySize( ZArray<::DirMap> &DSBN, ZDirFileEn_type pZDFT)
{
  DSBN.clear();
  ::DirMap wDirEntry;
  ZStatus wSt = fullDir(wDirEntry, pZDFT);
  while (wSt == ZS_SUCCESS) {
    long wi=0;
    while (wDirEntry.Size < DSBN[wi].Size)
      wi++;
    if (wi < DSBN.count())
      DSBN.insert(wDirEntry,wi);
    else
      DSBN.push(wDirEntry);
    wSt = fullDir(wDirEntry, pZDFT);
  } // while (wSt == ZS_SUCCESS)

  if (wSt == ZS_EOF)
    wSt = ZS_SUCCESS;
  return wSt;
}
ZStatus ZDir::fullDirAll(zbs::ZArray<::DirMap> &pDirArray, ZDirFileEn_type pZDFT)
{
    pDirArray.clear();
    ::DirMap wDirEntry;
    /* warning : to be coded */
    return ZS_SUCCESS;
}


ZStatus
ZDir::dirApprox(::DirMap &pDirEntry,const utf8VaryingString& pApprox, ZDirFileEn_type pZDFT)
{
  ZStatus wSt = ZS_SUCCESS;
  pDirEntry.clear();
  wSt=fullDir(pDirEntry,pZDFT);
  while (wSt==ZS_SUCCESS) {
    if (pDirEntry.Name.startsWith(pApprox.toUtf()))
        return ZS_SUCCESS;
    wSt=fullDir(pDirEntry,pZDFT);
  }
  return wSt;
}//dirApprox

ZStatus
ZDir::dirApproxAll(zbs::ZArray<::DirMap> &pDirArray, const utf8VaryingString& pApprox, ZDirFileEn_type pZDFT)
{
  ::DirMap wDirEntry;
  pDirArray.clear();
  ZStatus wSt = dirApprox(wDirEntry,pApprox,pZDFT);
  while (wSt==ZS_SUCCESS) {
    pDirArray.push(wDirEntry);
    wSt = dirApprox(wDirEntry,pApprox,pZDFT);
  }
  return wSt;
} // dirApproxAll



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



ZFileNameSelection&
ZFileNameSelection::_copyFrom(const ZFileNameSelection& pIn)
{
    SelRoot=pIn.SelRoot;
    RootFlag=pIn.RootFlag;
    SelExt=pIn.SelExt;
    ExtFlag=pIn.ExtFlag;
    return *this;
}

ZFileSelection&
ZFileSelection::_copyFrom(const ZFileSelection& pIn)
{
    ZArray<ZFileNameSelection>::clear();
    for (int wi=0; wi < pIn.count();wi++) {
        push(pIn.TabConst(wi));
    }
    BaseTypeMask = pIn.BaseTypeMask;
    Phrase = pIn.Phrase;
    return *this;
}
void ZFileSelection::clear()
{
    ZArray<ZFileNameSelection>::clear();
    BaseTypeMask = ZDFT_Nothing  ;
    Phrase.clear();
}



ZFileSelection&
ZFileSelection::operator << (const utf8VaryingString& pSelPhrase)
{
    ZFileSelection wFS ;
    parseFileNameSelections(pSelPhrase,wFS);
    for (int wi = 0 ; wi < wFS.count() ; wi++)
        push(wFS[wi]);

    if (!Phrase.isEmpty()) {
        Phrase += " ; " ;
    }
    Phrase += pSelPhrase;
    return *this;
}

ZFileSelection&
ZFileSelection::addSelPhrase(const utf8VaryingString& pSelPhrase)
{
    ZFileSelection wFS ;
    parseFileNameSelections(pSelPhrase,wFS);
    for (int wi = 0 ; wi < wFS.count() ; wi++)
        push(wFS[wi]);

    if (!Phrase.isEmpty()) {
        Phrase += " ; " ;
    }
    Phrase += pSelPhrase;
    return *this;
}
ZFileSelection&
ZFileSelection::setSelPhrase(const utf8VaryingString& pSelPhrase)
{
    clear();
    return addSelPhrase(pSelPhrase);
}


void
ZFileSelection::parseTerm(utf8_t* pPtr,utf8VaryingString&   pSelStr,ZDir::DENM_type&     pSelFlag)
{
    utf8_t* wToBeFreed = utfStrdup<utf8_t>(pPtr);
    pSelStr.clear();
    utf8_t* wPtrStart=wToBeFreed;

    /* Trim leading spaces */
    while ( utfIsSpace(*wPtrStart))
        wPtrStart++;

    /* Trim trailing spaces */
    utf8_t* wEnd = wPtrStart;
    while (*wEnd)  /* search end of string */
        wEnd++;
    wEnd--;
    /* get back ignoring spaces */
    while (utfIsSpace(*wEnd)&&(wEnd > wPtrStart)) {
        *wEnd = 0;
        wEnd--;
    }
    /* string is ready to be parsed */

    pSelFlag = ZDir::Nothing;
    if (*wPtrStart=='*') {
        wPtrStart++;
        if (*wPtrStart==0)  {/* if only wildcard in field then all is selected */
            pSelFlag |= ZDir::All;  /* no restriction */
            free(wToBeFreed);
            return ; /* done */
        }
        pSelFlag |= ZDir::EndsWith;
    }//  if (*wPtr=='*')

    utf8_t* wPtrC=wPtrStart;
    while (*wPtrC && (*wPtrC!='*')) {
        wPtrC++;
    }// while (*wPtrC)
    if (*wPtrC=='*') {
        *wPtrC = 0;
        wPtrC++;
        pSelFlag |= ZDir::StartsWith;
        if (*wPtrC != 0) {
            _ERRPRINT("ZFileSelection::parseTerm-E-MALFORMED In expression <%s> string <*%s> will be ignored.\n",pPtr,wPtrC)
        }
    }
    if (pSelFlag==ZDir::Nothing)
        pSelFlag = ZDir::ExactMatch;
    pSelStr = wPtrStart;
    pPtr=wPtrC;
    while (*pPtr) /* align start pointer to effective end of string to parse */
        pPtr++;
    free(wToBeFreed);
    return ;
}


bool
ZFileSelection::parseSingleFileNameSelection(utf8_t* pPtr,
                             ZFileNameSelection& pFileNameSelection)
{
    pFileNameSelection.clear();

    if (*pPtr==0)
        return false;

    utf8_t* wPtr = pPtr;

    /* trim leading spaces */
    utf8_t* wStart = wPtr;
    while (*wStart && *wStart == ' ')
        wStart++;

    if (*wStart==0)  {/* empty string */
        return false;
    }


    /* trim trailing spaces */
    utf8_t* wEnd = wPtr;
    while (*wEnd)
        wEnd++;
    wEnd--;
    while ((*wEnd==' ')&&(wEnd > wStart )) {
        *wEnd=0;
        wEnd--;
    }

    /* start parsing */

    wPtr = wStart; /* search root name delimiter '.' */

    while (*wPtr && *wPtr != '.') {
        wPtr++;
    }
    bool wNoMore=false;
    if (*wPtr==0)
        wNoMore = true ;
    *wPtr = 0;

    parseTerm(wStart,pFileNameSelection.SelRoot,pFileNameSelection.RootFlag);

    if (wNoMore) {
        pFileNameSelection.ExtFlag = ZDir::ExactMatch;  /* extension must be an empty string */
        return true;
    }

    wStart= wPtr+1;
    parseTerm(wStart,pFileNameSelection.SelExt,pFileNameSelection.ExtFlag);
    return true;
} // parseSingleFileNameSelection

void
ZFileSelection::parseFileNameSelections(const utf8VaryingString& wSelContent, ZFileSelection& pSelection)
{

    pSelection.clear();
    if (wSelContent.isEmpty())
        return;

    utf8_t* wToBeFreed = utfStrdup<utf8_t>(wSelContent.Data);
    utf8_t* wPtr = wToBeFreed;
    utf8_t* wStart = wToBeFreed;

    bool wFEnd=false;
    while (*wPtr) {
        /* break selection phrase from <*.xml ; *gabu.doc ; ... > into <*.xml> <*gabu.doc> <...> */
        while (*wPtr && (*wPtr != ';'))
            wPtr++;

        if (*wPtr!=0) {
            *wPtr=0 ;
            wPtr++;
            wStart=wPtr;
        }
        ZFileNameSelection wFSel;
        if (parseSingleFileNameSelection(wStart,wFSel))
            pSelection.push(wFSel);
    }// while (*wPtr)

    free(wToBeFreed);
    return;
} // parseFileNameSelections

utf8VaryingString
ZFileSelection::dump()
{
    utf8VaryingString wReturn;
    wReturn.sprintf("Phrase <%s>\n",Phrase.toCChar());
    wReturn.addsprintf("Base type mask <%s>\n",decode_ZDFT(BaseTypeMask));
    for (int wi=0; wi < count() ; wi++ ) {

        wReturn.addsprintf("Root <%s> <%s> Ext <%s> <%s>\n",
                           Tab(wi).SelRoot.toCChar(),decode_DENM(Tab(wi).RootFlag).toCChar(),
                           Tab(wi).SelExt.toCChar(),decode_DENM(Tab(wi).ExtFlag).toCChar());
    }
    return wReturn;
}

const char*
decode_ZDFT(ZDirFileEn_type pType)
{

    switch(pType)
    {
    case ZDFT_Nothing:
        return "ZDFT_Nothing";
    case ZDFT_RegularFile:
        return "ZDFT_RegularFile";
    case ZDFT_Directory:
        return "ZDFT_Directory";
    case ZDFT_SymbolicLink:
        return "ZDFT_SymbolicLink";
    case ZDFT_Other:
        return "ZDFT_Other";
    case ZDFT_Hidden:
        return "ZDFT_Hidden";
    case ZDFT_All:
        return "ZDFT_All";
    default:
        return "ZDFT_Invalid";
    }
}


const utf8VaryingString
decode_DENM(ZDir::DENM_type pType)
{
    if (pType==ZDir::All)
        return ("ZDir::All");

    utf8VaryingString wReturn;

    if (pType & ZDir::CaseRegardless) {
        wReturn = "ZDir::CaseRegardless";
    }

    switch(pType & ~ZDir::CaseRegardless)
    {
    case ZDir::ExactMatch:
        wReturn.addConditionalOR("ZDir::ExactMatch");
        return wReturn;
    case ZDir::StartsWith:
        wReturn.addConditionalOR("ZDir::StartsWith");
        return wReturn;
    case ZDir::EndsWith:
        wReturn.addConditionalOR("ZDir::EndsWith");
        return wReturn;
    case ZDir::Contains:
        wReturn.addConditionalOR("ZDir::Contains");
        return wReturn;
    default:
        wReturn.addConditionalOR("ZDir::Invalid");
        return wReturn;
    }
}


} // namespace zbs

#endif // ZDIR_CPP
