#ifndef URISTRING_CPP

#include <config/zconfig.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

#include <fcntl.h>

#ifdef __USE_WINDOWS__

#include <io.h>

#endif //__USE_WINDOWS__


#include <ztoolset/uristring.h>
#include <ztoolset/zexceptionmin.h>
#include <ztoolset/zdatabuffer.h>

#include <ztoolset/utfvaryingstring.h> // for utf8VaryingString utf16VaryingString and utf32VaryingString

#include <zio/zdir.h>
#include <zio/zioutils.h>

#ifdef QT_CORE_LIB
#include <QUrl>
#include <QString>
#endif

using namespace zbs;
//
//--------------uriString------------------------------
//

/*
uriString&
uriString::fromURI(const uriString &pURI)
{
    memmove(content,pURI.content, cst_urilen+1);
    return(*this);
}
*/
uriString&
uriString::fromURI(const uriString *pURI)
{
    if (pURI==nullptr) {
        clear();
        return *this;
    }
    if (Data==nullptr)
      allocateUnits(pURI->UnitCount);
    strset(pURI->toUtf());
    return(*this);
}
uriString&
uriString::fromURI(const uriString& pURI)
{
  if (pURI.isEmpty())
    return *this;
  if (Data==nullptr)
    allocateUnits(pURI.UnitCount);
  strset(pURI.toUtf());
  _Base::CheckData();
  return(*this);
}

ZStatus uriString::operator << (const utf8VaryingString &pDBS) const
{
    return appendContent (pDBS) ;
}



/*
uriString&
uriString::operator += (utfdescString &pSource)
{
    return (uriString&)add(pSource.toUtf());
}

uriString&
uriString::operator = (utfcodeString &pCode)
{
    return (uriString&)strset(pCode.content);
}
uriString&
uriString::operator += (utfcodeString &pCode)
{
    return (uriString&)add(pCode.toUtf());

}

bool uriString::operator == (utfdescString &pCompare) { return(compare(pCompare.content)==0);}
bool uriString::operator != (utfdescString& pCompare) { return!(compare(pCompare.content)==0);}
bool uriString::operator > (utfdescString& pCompare) { return(compare(pCompare.content)>0);}
bool uriString::operator < (utfdescString& pCompare) { return(compare(pCompare.content)<0);}
*/

//#include <dirent.h>

/**
 * @brief uriString::fileList creates a new content list with regular files contained within directory pointed by current uriString content.
 * @param pList a pointer to a ZArray of uriString. This pointer will be intialized within routine with the appropriate ZArray<uriString> object,
 * then populated.
 * if directory is empty or has no file, then ZS_SUCCESS is returned, pList is instanciated with an empty ZArray<uriString>.
 * @return a ZStatus set to ZS_SUCCESS if everything went OK, pList is loaded with the appropriate entries.
 *  - ZS_BADFILETYPE is not a directory
 *  - ZS_ERROPEN if an error occurred while openning the directory to list its content
 *  - ZS_FILEERROR if an error occurred during reading directory entry content.
 */
ZStatus
uriString::fileList(ZArray<uriString>* pList)
{

    return list(pList,ZDFT_RegularFile);
}//fileList

ZStatus
uriString::list(ZArray<uriString>* pList,ZDirFileEn_type pZDFT)
{

    if (!isDirectory())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_BADFILETYPE,
                                  Severity_Error,
                                  "<%s> is not a directory",toString());
                pList=nullptr;
                return ZS_BADFILETYPE;
            }

uriString   wFile;
ZStatus     wSt;
ZDir        wDir;

    wSt=wDir.setPath(toUtf());
    if (wSt!=ZS_SUCCESS)
            return wSt;
    wSt=wDir.dir(wFile,pZDFT);
    while (wSt==ZS_SUCCESS)
        {
        pList->push(wFile);
        wSt=wDir.dirNext(wFile);
        }
    if (wSt!=ZS_EOF)
            return wSt;
    return ZS_SUCCESS;
}//list
/**
 * @brief uriString::fileList creates a new content list with regular files contained within directory pointed by current uriString content.
 * @param pList a pointer to a ZArray of uriString. This pointer will be intialized within routine with the appropriate ZArray<uriString> object,
 * then populated.
 * if directory is empty or has no subdirectory then ZS_SUCCESS is returned, pList is instantiated with an empty ZArray<uriString>.
 *
 * @return a ZStatus set to ZS_SUCCESS if everything went OK, pList is loaded with the appropriate entries.
 *  - ZS_BADFILETYPE is not a directory
 *  - ZS_ERROPEN if an error occurred while openning the directory to list its content
 *  - ZS_FILEERROR if an error occurred during reading directory entry content.
 */
ZStatus
uriString::subDir(ZArray<uriString>* pList)
{
    return list(pList,ZDFT_Directory);

}//subDir

#ifdef QT_CORE_LIB


uriString&
uriString::fromQUrl(QUrl &pUrl)
{
    fromQString(pUrl.toString(QUrl::PreferLocalFile));
    return(*this);
}


#endif // QT
/*
uriString&
uriString::fromQString(const QString *pQString){memset (content,0,sizeof(content));
                                            if (pQString->size()>cst_urilen)
                                                          strncpy(content,pQString->toUtf8(),cst_urilen);
                                            strcpy(content,pQString->toUtf8());
                                            return(*this);}
*/
/*
uriString&
uriString::fromdescString(const utfdescString &pString)
{
    return (uriString&)strset(pString.toUtf());
}

uriString&
uriString::fromcodeString(const utfcodeString &pCode)
{
    return (uriString&)strset(pCode.toUtf());
}

*/



/**
* @brief addConditionalDirectoryDelimiter  adds a directory delimiter if and only if there is no delimiter at the end of the uriString yet
*/
uriString&
uriString::addConditionalDirectoryDelimiter(void)
{
    long wS=strlen();
    if ((isEmpty())||(wS<1))
                    return(*this);
    utfRTrim(Data);

    wS=strlen();
    if (Data[wS-1]==(utf8_t)Delimiter)
                            return(*this);
    add(__DELIMITER_STRING__);
    return(*this);
}//addConditionalDirectoryDelimiter

/**
* @brief addConditionalDirectoryDelimiter  adds a directory delimiter if and only if there is no delimiter at the end of the uriString yet
* if pToAdd is empty, then nothing is done and path content remains the same.
*/
uriString&
uriString::addWithLeadingCondDirDelim(const utf8VaryingString& pToAdd)
{
    if (pToAdd.isEmpty())
        return *this;
    addConditionalDirectoryDelimiter();
    add(pToAdd);
    return(*this);
}//addConditionalDirectoryDelimiter

uriString
uriString::removeLastDirectoryDelimiter(void) const
{
    uriString wReturn=*this;

    long wS=wReturn.strlen();
    if ((wReturn.isEmpty())||(wS<1))
        return(*this);
    utfRTrim(wReturn.Data);
    wS=wReturn.strlen();
    if (wReturn.Data[wS-1]==(utf8_t)Delimiter) {
        wReturn.Data[wS-1]='\0';
    }
    wReturn.allocateUnits(wS);
    return(wReturn);
}//addConditionalDirectoryDelimiter

/**
 * @brief addDirectoryDelimiter  force adds a directory delimiter at the end of the given uriString
 *
 *  The directory delimiter is only added if there is not already one.
 */
uriString&
uriString::addDirectoryDelimiter(void)
{
    add(__DELIMITER_STRING__);
    return(*this);
}//addDirectoryDelimiter


ZStatus
uriString::check()
{

  const utf8_t* wTrimmed=utfSkipSpaces(Data);
  if (wTrimmed!=Data) {
    ZException.setMessage("uriString::check",ZS_MALFORMED,Severity_Error,"Leading space in path <%s>",Data);
    return ZS_MALFORMED;
  }
  uriStat wStat;
  ZStatus wSt=getStatR(wStat);
  return wSt;
}

//#include <unistd.h>
//#include <pwd.h>
#include <ztoolset/zsystemuser.h>
uriString uriString::getHomeDir(void)
{
    ZSystemUser wUser;

    uriString wHomeDir = wUser.setToCurrentUser().getHomeDir();

/*    uid_t wuid = getuid();
    struct passwd * wuserdata = getpwuid(wuid);
    uriString wHomeDir = wuserdata->pw_dir ;
*/
    return wHomeDir;
}

uriString
uriString::currentWorkingDirectory() {
  char wB[255];
  uriString wReturn = getcwd(wB,255);
  return wReturn;
}

/**
 * @brief uriString::addMimeFileExtension  add to the uriString the file extension corresponding to the given Mime name.
 *  if uriString already has an extension, then it will be replaced by the given extension out of Mime name.
 *  - returns false if Mime name does not give a file extension,
 *  - returns true otherwise.
 *
 * @param pMimeName
 * @return
 */
bool uriString::addMimeFileExtension(const utf8VaryingString &pMimeName)
{
  if (isEmpty())
    return false;

    utf8_t* wMimeExt=pMimeName.strchr((utf8_t)'/');
    if (wMimeExt==nullptr)
                return false;
    wMimeExt++;
    utf8_t* wPtr1 = utfStrchr<utf8_t>((Data + 2),(utf8_t)'.');
    if (wPtr1==nullptr)
            {
            add(".");
            wPtr1=&Data[strlen()];
            }
    wPtr1++;
    *wPtr1=(utf8_t)'\0';
    add(wMimeExt);
    return true;
}//addMimeFileExtension
//
// ----------PathName searches and extractions
//
//utfdescString DSBuffer;

utf8VaryingString uriString::getFileExtension() const
{
  if (isEmpty())
    return utf8VaryingString ("");

    utf8VaryingString wExt;
    utf8_t *wPtr = strchr((utf8_t)'.');
    if (wPtr==nullptr)
      return utf8VaryingString("");
    wPtr++;
    wExt=wPtr;
    return wExt;
}

uriString uriString::getPathTilExtension() const
{
    if (isEmpty())
        return uriString ("");

    utf8_t* wPtrToFree = utfStrdup(Data);
    uriString wExt;

    utf8_t *wPtr = utfStrchr(wPtrToFree,(utf8_t)'.');
    if (wPtr==nullptr) {
        free(wPtrToFree);
        return *this ;
    }
    *wPtr='\0';
    uriString wReturn (wPtrToFree);
    free(wPtrToFree);
    return wReturn;
}


    /** @brief uriString::getDirectoryPath Returns the full file's directory path
     * i. e. </directory path/><root name>.<extension>
     * @return an utf8VaryingString with the full file's directory path including
     */
uriString uriString::getDirectoryPath() const
{
  if (isEmpty())
    return utf8VaryingString ();
  uriString wDir(Data);

  utf8_t *wPtr = wDir.Data + wDir.strlen();
  while ((wPtr > wDir.Data) && (*wPtr != Delimiter) && (*wPtr != '.'))
      wPtr--;
  if (*wPtr == '.') { /* skip '.' if found */
    wPtr--;
    while ((wPtr > wDir.Data) && (*wPtr != Delimiter))
      wPtr--;
  }
  if (wPtr==wDir.Data)  /* no directory delimiter found */
    return utf8VaryingString ();
  /* here wPtr points to Delimiter */
  *wPtr = 0;
  return uriString(wDir.Data);
}//getDirectoryPath
/** @brief getLastDirectoryName Returns the last directory mentionned in file's directory path
        i. e. </.../last directory/><root base name>.<extension>
 * @return an utf8VaryingString with the last file's directory within its directory path
 */
utf8VaryingString
uriString::getLastDirectoryName() const
{
  if (isEmpty())
    return utf8VaryingString ();

    utf8VaryingString wDir = Data; /* duplicate the string */

    utf8_t* wPtr=wDir.Data + wDir.strlen();
    while((wPtr > wDir.Data) && (*wPtr != (utf8_t)Delimiter))
            wPtr--;

    if (wPtr==Data)
      return utf8VaryingString ();

    *wPtr= 0;
    wPtr--;
    while((wPtr > wDir.Data) && (*wPtr != (utf8_t)Delimiter) )
      wPtr--;

    if (wPtr!=wDir.Data)
      wPtr++;
    return utf8VaryingString(wPtr);
}//getLastDirectoryName

  /** @brief uriString::getBasename       returns a utf8VaryingString containing the file's full base name\n
    i. e. </directory path/><root base name>.<extension>\n
        Where full base name is : <root base name>.<extension> without directory path
    * @return an utf8VaryingString with the file's base name
    */
utf8VaryingString
uriString::getBasename ()const
{
  if (isEmpty())
    return utf8VaryingString ("");

utf8VaryingString wBasename;
    utf8_t *wPtr =  strrchr((utf8_t)Delimiter);

    if (wPtr==nullptr)
            wBasename = Data ;
         else
            {
            wPtr++;
            wBasename = wPtr;
            }

    return wBasename;
} // getBasename

/** @brief getRootname returns a utf8VaryingString containing the file's root name\n
 *  i. e. </directory path/><file's root name>.<extension>
 * @return a utf8VaryingString with the file's root base name
 */
utf8VaryingString uriString::getRootname() const
{
  if (isEmpty())
    return utf8VaryingString ("");


  utf8VaryingString wRoot=getBasename();
  utf8_t* wPtrEnd= (utf8_t* )(wRoot.Data+wRoot.strlen());

  utf8_t* wPtr= (utf8_t*)wRoot.Data;

  while (*wPtr && (*wPtr != '.'))
    wPtr++;
  *wPtr = 0;

  return utf8VaryingString(wRoot.Data) ;
}//getRootname




void uriString::changeFileExtension(const utf8VaryingString& pExt) {
  if (pExt.isEmpty())
    return;

  utf8VaryingString wReturn;
  size_t wLen = strlen();

  utf8_t* wPtrDot = strchr('.');

  if (wPtrDot!=nullptr)
    *wPtrDot=0;
  wReturn = Data;

  if (pExt[0]=='.') {
    wReturn += pExt;
    strset(wReturn.Data);
    return;
  }// if (pExt[0]=='.')
  wReturn += ".";
  wReturn += pExt;
  strset(wReturn.Data);
  return;
}//changeFileExtension

void uriString::changeBasename(const utf8VaryingString& pBasename) {
  utf8_t *wPtrBegin =  strrchr((utf8_t)Delimiter);

  utf8VaryingString wResult;

  if (wPtrBegin==nullptr)
    wPtrBegin = Data ;
  else
  {
    wPtrBegin++;
  }

  *wPtrBegin = '\0';

  wResult.strset(Data);

  utf8_t* wPtrEnd =++wPtrBegin;
  while (*wPtrEnd && (*wPtrEnd!='.'))
      wPtrEnd++;

  wResult += pBasename;

  wResult += wPtrEnd;

  strset (wResult.Data);
  return;
}

#include <limits.h>
#include <stdlib.h>
utf8VaryingString
uriString::getUrl () const
{
  if (::strncmp((const char*)Data,"file://",7)==0)
    return utf8VaryingString(Data);

  utf8VaryingString wReturn;
  char* wCRet=nullptr;
  wCRet=realpath((const char*)Data,nullptr);
  wReturn = "file://";
  wReturn += wCRet;
  zfree(wCRet);
  return wReturn;
}//getUrl

uriString
uriString::getLocal () const
{
  if (::strncmp((const char*)Data,"file://",7)!=0)
    return *this;

  uriString wReturn;
  utf8_t* wPtr =(utf8_t*) (Data+7);

  wReturn.strset(wPtr);
  return wReturn;
}//getLocal


/**
 * @brief uriString::setDirectoryPath Replaces the current directory path (if any) with the directory path mentionned in given uriString (pDirectoryPath)
  * @warning pDirectoryPath must be tested BEFORE using this method to be a valid directory.
 * @param pDirectoryPath an uriString that will contain the directory path that will replace the current one
 * @return a reference to current uriString object
 */
uriString&
uriString::setDirectoryPath (uriString &pDirectoryPath)
{
utf8VaryingString wBaseName;
    wBaseName = getBasename();

    strset ((const utf8_t*)pDirectoryPath.toCChar());
    addConditionalDirectoryDelimiter();
    add(wBaseName);

    return *this;
}//setDirectoryPath

utf8VaryingString
uriString::getUniqueName (const char* pRootName)
{
    char wTempName[30];
    ::strncpy(wTempName,pRootName,20);
    ::strncat(wTempName,"XXXXXX",29);

#ifdef __USE_WINDOWS__
    return utf8VaryingString((utf8_t*)_mktemp(wTempName));
#else
    return utf8VaryingString((utf8_t*)mkstemp(wTempName));
#endif //__USE_WINDOWS__


}

void
uriString::setUniqueName (const char* pRootName)
{
    strset (getUniqueName(pRootName).toUtf());
}

void
uriString::setExtension (const char* pExtension)
{

  size_t wExtSize=::strlen(pExtension);
  utf8_t* wPtr=strrchr( '.');

  size_t wSz=wPtr + 1 - Data;

  long wNeed = UnitCount - wSz ;
  wNeed -= wExtSize;

  if (wNeed < 0)
    extendUnitsBZero((- wNeed) + 1 );

  if (wPtr==nullptr)
            add(".");

    wPtr++;
    add(pExtension);
    return;
}

ZStatus uriString::remove() const
{
    /* see https://www.gnu.org/software/libc/manual/html_node/Deleting-Files.html
   */
    ZStatus wSt = ZS_SUCCESS;
    errno = 0;
    if (::remove(toCChar()) != 0) {
        int wErrno = errno;
        wSt = ZS_ILLEGAL;
        utf8VaryingString wErrMsg;
        switch (wErrno) {
        case EACCES:
            wSt = ZS_ACCESSRIGHTS;
            wErrMsg.sprintf("<EACCES> Write permission is denied for the directory from which the "
                            "file is to be removed, or the directory has the sticky bit set and "
                            "you do not own the file. File <%s>",
                            toString());
            break;
        case EBUSY:
            wSt = ZS_LOCKED;
            wErrMsg.sprintf(
                "<EBUSY> This error indicates that the file is being used by the system in such a "
                "way that it can’t be unlinked. For example, you might see this error if the file "
                "name specifies the root directory or a mount point for a file system. File <%s>",
                toString());
            break;
        case ENOENT:
            wSt = ZS_FILENOTEXIST;
            wErrMsg.sprintf("<ENOENT> The file name to be deleted doesn’t exist.File <%s>",
                            toString());
            break;
        case EPERM:
            wSt = ZS_INVOP;
            wErrMsg
                .sprintf("<EPERM> On some systems unlink cannot be used to delete the name of a "
                         "directory, or at least can only be used this way by a privileged user. "
                         "To avoid such problems, use rmdir to delete directories. File <%s>",
                         toString());
            break;
        case EROFS:
            wSt = ZS_ACCESSRIGHTS;
            wErrMsg.sprintf("<EROFS> The directory containing the file name to be deleted is on a "
                            "read-only file system and can’t be modified. File <%s>",
                            toString());
            break;

        default:
            wErrMsg.sprintf("Unknown error for removing file <%s>.", toString());
            break;
        } // switch
        ZException.getErrno(wErrno, _GET_FUNCTION_NAME_, wSt, Severity_Error, wErrMsg.toCChar());
        return wSt;
    }
    return ZS_SUCCESS;
} //remove


uriString
uriString::getBckName(const uriString &pName, const utf8VaryingString& pBckExt){
    int wFormerNumber = 1;
    uriString wBckURI ;
    uriString wPathTilExt = pName.getPathTilExtension() ;

    utf8VaryingString wExt = pName.getFileExtension();
    utf8VaryingString wExtBck ;
    wExtBck.sprintf(".%s_%s%02ld",wExt.toCChar(),pBckExt.toCChar(),wFormerNumber)  ;

    wBckURI =  wPathTilExt + wExtBck ;

    while (wBckURI.exists())
    {
        wFormerNumber ++;
        wExtBck.sprintf(".%s_%s%02ld",wExt.toCChar(),pBckExt.toCChar(),wFormerNumber)  ;
        wBckURI =  wPathTilExt + wExtBck ;
    }
    return wBckURI ;
}

ZStatus
uriString::renameBck(const char* pBckExt) const
{
  if (!exists()) {
    ZException.setMessage("uriString::renameBck",ZS_FILENOTEXIST,Severity_Error,
        "Source file %s does not exist.",toString());
    return ZS_FILENOTEXIST;
  }


  uriString wBckURI = getBckName(*this,pBckExt);
  return rename(wBckURI);
} // renameBck

ZStatus uriString::rename(const utf8VaryingString &pNewURI,
                          bool pNoExcept,
                          ZaiErrors *pErrorLog) const
{
    /* see https://pubs.opengroup.org/onlinepubs/9699919799/functions/rename.html
Upon successful completion, the rename() function shall return 0. Otherwise, it shall return -1, errno shall be set to indicate the error,
and neither the file named by old nor the file named by new shall be changed or created.
*/
    return rawRename(*this,pNewURI,pNoExcept,pErrorLog);
    /*
    errno = 0;
    int wRet = std::rename(toCChar(), pNewURI.toCChar());
    if (wRet < 0) {
        int wErrno = errno;
        ZStatus wSt = ZS_ILLEGAL;
        utf8VaryingString wErrMsg;
        switch (wErrno) {
        case EACCES:
            wSt = ZS_ACCESSRIGHTS;
            wErrMsg.sprintf("<EACCES> A component of either path prefix denies search permission; "
                            "or one of the directories containing old or new denies write "
                            "permissions; or, write permission is required and is denied for a "
                            "directory pointed to by the old or new arguments."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;
        case EBUSY:
            wSt = ZS_LOCKED;
            wErrMsg.sprintf(
                "<EBUSY> The directory named by old or new is currently in use by the system or "
                "another process, and the implementation considers this an error."
                " Renaming file <%s> to <%s>",
                toString(),
                pNewURI.toString());
            break;
        case EEXIST:
        case ENOTEMPTY:
            wSt = ZS_FILEEXIST;
            wErrMsg.sprintf("<EEXIST-ENOTEMPTY> The link named by new is a directory that is not "
                            "an empty directory."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;
        case EINVAL:
            wSt = ZS_INVOP;
            wErrMsg.sprintf(
                "<EINVAL> The old pathname names an ancestor directory of the new pathname, or "
                "either pathname argument contains a final component that is dot or dot-dot."
                " Renaming file <%s> to <%s>",
                toString(),
                pNewURI.toString());
            break;
        case EIO:
            wSt = ZS_FILEERROR;
            wErrMsg.sprintf("<EIO> A physical I/O error has occurred."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;
        case EISDIR:
            wSt = ZS_NOTDIRECTORY;
            wErrMsg.sprintf("<EISDIR> The new argument points to a directory and the old argument "
                            "points to a file that is not a directory."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;
        case ELOOP:
            wSt = ZS_CORRUPTED;
            wErrMsg.sprintf("<ELOOP> A loop exists in symbolic links encountered during resolution "
                            "of the path argument."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;
        case EMLINK:
            wSt = ZS_OUTBOUNDHIGH;
            wErrMsg.sprintf("<EMLINK> The file named by old is a directory, and the link count of "
                            "the parent directory of new would exceed {LINK_MAX}."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;

        case ENAMETOOLONG:
            wSt = ZS_INVNAME;
            wErrMsg.sprintf(
                "<ENAMETOOLONG> The length of a component of a pathname is longer than {NAME_MAX}."
                " Renaming file <%s> to <%s>",
                toString(),
                pNewURI.toString());
            break;
        case ENOENT:
            wSt = ZS_FILENOTEXIST;
            wErrMsg.sprintf("<ENOENT>  The link named by old does not name an existing file, a "
                            "component of the path prefix of new does not exist, or either old or "
                            "new points to an empty string."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;
        case ENOSPC:
            wSt = ZS_MEMOVFLW;
            wErrMsg.sprintf("<ENOSPC> The directory that would contain new cannot be extended."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;

        case ENOTDIR:
            wSt = ZS_NOTDIRECTORY;
            wErrMsg.sprintf(
                "<ENOTDIR> A component of either path prefix names an existing file that is "
                "neither a directory nor a symbolic link to a directory; or the old argument names "
                "a directory and the new argument names a non-directory file; or the old argument "
                "contains at least one non- <slash> character and ends with one or more trailing "
                "<slash> characters and the last pathname component names an existing file that is "
                "neither a directory nor a symbolic link to a directory; or the old argument names "
                "an existing non-directory file and the new argument names a nonexistent file, "
                "contains at least one non- <slash> character, and ends with one or more trailing "
                "<slash> characters;"
                " or the new argument names an existing non-directory file, contains at least one "
                "non- <slash> character, and ends with one or more trailing <slash> characters."
                " Renaming file <%s> to <%s>",
                toString(),
                pNewURI.toString());
            break;

        case EPERM:
            wSt = ZS_ACCESSRIGHTS;
            wErrMsg.sprintf("<EPERM> he S_ISVTX flag is set on the directory containing the file "
                            "referred to by old "
                            "and the process does not satisfy the criteria specified in XBD "
                            "Directory Protection with respect to old; or new refers to an "
                            "existing file, the S_ISVTX flag is set on the directory containing "
                            "this file, and the process does not satisfy the criteria specified in "
                            "XBD Directory Protection with respect to this file."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;
        case EROFS:
            wSt = ZS_FILEERROR;
            wErrMsg.sprintf("<EROFS> The requested operation requires writing in a directory on a "
                            "read-only file system."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;
        case EXDEV:
            wSt = ZS_FILEERROR;
            wErrMsg.sprintf("<EXDEV> The links named by new and old are on different file systems "
                            "and the implementation does not support links between file systems."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;
        case ETXTBSY:
            wSt = ZS_LOCKED;
            wErrMsg.sprintf("<ETXTBSY>  The file named by new exists and is the last directory "
                            "entry to a pure procedure (shared text) file that is being executed."
                            " Renaming file <%s> to <%s>",
                            toString(),
                            pNewURI.toString());
            break;

        default:
            wErrMsg.sprintf("Unknown error for renaming file <%s>.", toString());
            break;
        } // switch
        ZException.getErrno(wErrno, _GET_FUNCTION_NAME_, wSt, Severity_Error, wErrMsg.toCChar());
        return wSt;
    }

    return ZS_SUCCESS;
*/
} // rename



ZStatus
uriString::backupFile(const utf8VaryingString& pBckExt)
{
  if (!exists()) {
    ZException.setMessage("uriString::backupFile",ZS_FILENOTEXIST,Severity_Error,
        "Source file %s does not exist.",toString());
    return ZS_FILENOTEXIST;
  }

  uriString wNewFileURI = getBckName(*this,pBckExt);

  ZDataBuffer wContent;
  ZStatus wSt = loadContent(wContent);

  wSt=wNewFileURI.writeContent(wContent);

  if (wSt!=ZS_SUCCESS)
  {
    return wSt;
  }

  if (BaseParameters->VerboseFileEngine())
    fprintf (stdout,
              "uriString::backupFile-I Copied existing file <%s> to <%s> \n",
              toCChar(), wNewFileURI.toCChar());
  return ZS_SUCCESS;
}


ZStatus
uriString::copyFile(uriString pDest, const uriString pSource,uint8_t pOption)
{
  if (!pSource.exists()) {
    ZException.setMessage("uriString::copyFile",ZS_FILENOTEXIST,Severity_Error,
        "Source file %s does not exist.",pSource.toString());
    return ZS_FILENOTEXIST;
  }

  int wFormerNumber = 1 ;
  bool wDestExists = false ;
  if (pDest.exists()) {
    if (pOption & UCO_BackUp) {
      pDest.renameBck("bck");
    }
    else if (pOption & UCO_DoNotReplace) {
      ZException.setMessage("uriString::copyFile",ZS_FILEEXIST,Severity_Error,
          "Destination file %s already exist while option UCO_NoNotReplace has been set.",pSource.toString());
      return ZS_FILEEXIST;
    } else {
      wDestExists = true;
    }
  } // if (pDest.exists())


  ZDataBuffer wContent;
  ZStatus wSt = pSource.loadContent(wContent);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }

  wSt=pDest.writeContent(wContent);
  if (wSt!=ZS_SUCCESS) {
    return wSt;
  }

  if (wDestExists)
  {
    if (BaseParameters->VerboseFileEngine())
      fprintf (stdout,
        "uriString::copyFile-I-REPLACED Replaced existing file <%s> to <%s> \n",
        pSource.toCChar(),
        pDest.toCChar());
    return ZS_FILEREPLACED ;
  }

  if (BaseParameters->VerboseFileEngine())
    fprintf (stdout,
        "uriString::copyFile-I-CPYDONE Copied existing file <%s> to <%s> \n",
        pSource.toCChar(),
        pDest.toCChar());
  return ZS_SUCCESS;
} //uriString::copyFile
//
//---------String search and handling
//





/**
 * @brief uriString::exists Test if current file description stored in current uriString exists as a file or a directory
 * @return true if exists, false if not
 */
bool
uriString::exists(void) const
{
struct stat wStatBuffer ;
    int wSt= stat(toCChar(),&wStatBuffer);
    if (wSt<0) {
      return(false);
    }
    return(true);
}

bool
uriString::isPath(void) const
{
    return isDirectory();
}


bool
uriString::possiblyXml(void) const
{
    utf8VaryingString wExt = getFileExtension();
    if (wExt.compareCase("XML")==0)
        return true;

    return(false);
}
bool
uriString::possiblyHtml(void) const
{
    utf8VaryingString wExt = getFileExtension();
    if (wExt.compareCase("HTML")==0)
        return true;
    if (wExt.compareCase("HTM")==0)
        return true;
    return(false);
}
bool
uriString::possiblyText(void) const
{
    utf8VaryingString wExt = getFileExtension();
    if (wExt.compareCase("TXT")==0)
        return true;
    if (wExt.isEmpty())
        return true;
    return(false);
}

bool
uriString::possiblyPdf(void) const
{
    utf8VaryingString wExt = getFileExtension();
    if (wExt.compareCase("PDF")==0)
        return true;
    return(false);
}

bool
uriString::possiblyOdf(void) const
{
    utf8VaryingString wExt = getFileExtension();
    if (wExt.compareCase("odf")==0)
        return true;
    if (wExt.compareCase("odg")==0)
        return true;
    if (wExt.compareCase("ods")==0)
        return true;
    if (wExt.compareCase("odp")==0)
        return true;
    return(false);
}
bool
uriString::possiblyIconSource(void) const
{
    utf8VaryingString wExt = getFileExtension();
    if (wExt.compareCase("JPEG")==0)
        return true;
    if (wExt.compareCase("JPG")==0)
        return true;
    if (wExt.compareCase("PNG")==0)
        return true;
    if (wExt.compareCase("GIF")==0)
        return true;
    return(false);
}

bool
uriString::possiblyZMF(void) const
{
    utf8VaryingString wExt = getFileExtension();
    if (wExt.compareCase("zmf")==0)
        return true;
    return(false);
}
bool
uriString::possiblyZRH(void) const
{
    utf8VaryingString wExt = getFileExtension();
    if (wExt.compareCase("zrh")==0)
        return true;
    return(false);
}

void    uriString::setToWorkingDir()
{
  char wBuf[cst_urilen];

  strset((const utf8_t*)getcwd((char *)wBuf,cst_urilen));
}
/*
ZStatus
uriString::createDirectory (const uriString & pPath){

  return ZDir::mkdir(pPath.toCChar());
}
*/
ZStatus
uriString::createDirectory (const uriString & pPath, __FILEACCESSRIGHTS__ pMode){

  return ZDir::mkdir(pPath.toCChar(),pMode);
}

/**
 * @brief uriString::isDirectory test whether the given uri points to a directory
 * @return true if directory, false if not
 */

#ifdef __USE_WINDOWS__

bool
uriString::isDirectory(void) {
  DWORD attribs = ::GetFileAttributesA(toString());
  if (attribs == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return (attribs & FILE_ATTRIBUTE_DIRECTORY);
}
#else
bool
uriString::isDirectory(void) const
{
struct stat wStatBuffer ;

    int wSt= stat(toCChar(),&wStatBuffer);
    if (wSt<0)
            {
            return(false);
            }
    if ((wStatBuffer.st_mode & S_IFMT)== S_IFDIR)
            {
            return(true);
            }

    return(false);

}// isDirectory

#endif //__USE_WINDOWS__
/**
 * @brief uriString::isRegularFile  test whether the given uri points to a regular file
 * @return true if regular file, false if not
 */
#ifdef __USE_WINDOWS__

bool
uriString::isRegularFile(void) {
      DWORD attribs = ::GetFileAttributesA(toString());
      if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
      }
      return (attribs & FILE_ATTRIBUTE_NORMAL);
    }
}
#else
bool
uriString::isRegularFile(void)
{
struct stat wStatBuffer ;
    memset(&wStatBuffer,0,sizeof(wStatBuffer));
    int wSt= stat(toCString_Strait(),&wStatBuffer);
    if (wSt<0)
            {
            return(false);
            }
    if ((wStatBuffer.st_mode & S_IFMT)== S_IFREG)
            {
            return(true);
            }

    return(false);
}// isRegularFile
#endif // __USE_WINDOWS__

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
ZStatus
uriString::suppress(void)
{
  errno=0;
  int wSt=::remove(toCString_Strait());
    if (wSt<0)
            {
            switch (errno)
                {
                case (EPERM) :
                case (EACCES) :
                    {
                    return(ZS_ACCESSRIGHTS);
                    }
            case (EBUSY) :
                    {
                    return(ZS_LOCKED);
                    }
             case (ENOENT) :
                    {
                    return(ZS_FILENOTEXIST);
                    }
             default:
                return (ZS_ERROR);
              }// switch
    }// if

    return(ZS_SUCCESS);
}// suppress

__FILEACCESSRIGHTS__ uriString::getPermissions()
{
  struct statx wStat;

  ZStatus wSt=_getStat(wStat,false);
  if (wSt!=ZS_SUCCESS)
    return -1;
  return (wStat.stx_mode & 0x777);
}


ZStatus uriString::changePermissions(const uriString& pPath,__FILEACCESSRIGHTS__ pPermissions)
{
  return pPath.changePermissions(pPermissions);
}

ZStatus uriString::changePermissions(__FILEACCESSRIGHTS__ pPermissions) const
{
  /* see https://www.man7.org/linux/man-pages/man2/chmod.2.html */
  errno=0;
  int wRet=chmod(toCChar(),pPermissions);
  if (wRet<0)
  {
      int wErrno=errno;
      ZStatus wSt=ZS_ILLEGAL;
      utf8VaryingString wErrMsg;
      switch (wErrno) {
      case EACCES:
        wSt= ZS_ACCESSRIGHTS ;
        wErrMsg.sprintf( "<EACCES> A component of either path prefix denies search permission; or one of the directories containing old or new denies write permissions; or, write permission is required and is denied for a directory pointed to by the old or new arguments."
                        " file <%s>.",toString());
        break;
      case EPERM:
        wSt= ZS_ACCESSRIGHTS;
        wErrMsg.sprintf( "<EPERM> The effective UID does not match the owner of the file, and the process is not privileged."
                         " Or The file is marked immutable or append-only."
                        " file <%s>",toString());
      case EROFS:
        wSt= ZS_FILEERROR;
        wErrMsg.sprintf( "<EROFS> The named file resides on a read-only filesystem."
                        " file <%s>",toString());
        break;
      case EBADF:
        wSt= ZS_BADFILEDESC;
        wErrMsg.sprintf( "<EBADF> The file descriptor is not valid."
                        " file <%s> ",toString());
        break;
      case EFAULT:
        wSt= ZS_LOCKED;
        wErrMsg.sprintf( "<EFAULT> File's pathname points outside accessible address space."
                        " file <%s>",toString());
        break;
      case EINVAL:
        wSt= ZS_INVPARAMS;
        wErrMsg.sprintf( "<EINVAL> Invalid access rights value."
                         " file <%s>",toString());
        break;
      case EIO:
        wSt= ZS_FILEERROR;
        wErrMsg.sprintf( "<EIO> A physical I/O error has occurred."
                        " file <%s>",toString());
        break;
      case ELOOP:
        wSt= ZS_CORRUPTED;
        wErrMsg.sprintf( "<ELOOP> A loop exists in symbolic links encountered during resolution of the path argument."
                        " file <%s>",toString());
        break;
      case ENAMETOOLONG:
        wSt= ZS_INVNAME;
        wErrMsg.sprintf( "<ENAMETOOLONG> The length of a component of a pathname is longer than {NAME_MAX}."
                        " file <%s>",toString());
        break;
      case ENOENT:
        wSt= ZS_FILENOTEXIST;
        wErrMsg.sprintf( "<ENOENT>  The link named by old does not name an existing file, a component of the path prefix of new does not exist, or either old or new points to an empty string."
                        " file <%s>",toString());
        break;
      case ENOMEM:
        wSt= ZS_MEMOVFLW;
        wErrMsg.sprintf( "<ENOMEM> Insufficient kernel memory was available."
                         " file <%s>",toString());
        break;
      case ENOTDIR:
        wSt= ZS_NOTDIRECTORY;
        wErrMsg.sprintf( "<ENOTDIR> A component of either path prefix names an existing file that is neither a directory nor a symbolic link to a directory; or the old argument names a directory and the new argument names a non-directory file; or the old argument contains at least one non- <slash> character and ends with one or more trailing <slash> characters and the last pathname component names an existing file that is neither a directory nor a symbolic link to a directory; or the old argument names an existing non-directory file and the new argument names a nonexistent file, contains at least one non- <slash> character, and ends with one or more trailing <slash> characters;"
                        " Or the new argument names an existing non-directory file, contains at least one non- <slash> character, and ends with one or more trailing <slash> characters."
                        " file <%s>",toString());
        break;


      case ENOTSUP:
        wSt= ZS_NOTDIRECTORY;
        wErrMsg.sprintf( "<ENOTSUP> Flags specified AT_SYMLINK_NOFOLLOW, which is not supported."
                        " file <%s>",toString());
        break;

      default:
        wErrMsg.sprintf( "Unknown error for changing access rights for file <%s>.",toString());
        break;
      }// switch
      ZException.getErrno(wErrno,
          _GET_FUNCTION_NAME_,
          wSt,
          Severity_Error,
          wErrMsg.toCChar());
      return wSt;
  }// if

return ZS_SUCCESS;
} // changeAccessRights




/**
 * @brief uriString::getStatR  getfile attributes information from file system for the current uriString filename
 * @param[in] pURI
 * @param[out] pZStat
 *
 * returned error see https://www.man7.org/linux/man-pages/man2/stat.2.html
 *
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.
      ZS_FILENOTEXIST   <ENOENT> A component of <%s> does not exist or is a dangling symbolic link.
      ZS_USERPRIVILEGES <EACCES> Search permission is denied for one of the directories in the path prefix of <%s>.
      ZS_FILENOTOPEN    <EBADF> File descriptor is not a valid open file descriptor for file <%s>.
      ZS_CORRUPTED      <EFAULT> Bad address for file <%s>.
      ZS_INVPARAMS      <EINVAL> (fstatat()) Invalid flag specified in flags for file <%s>.
      ZS_BADFILEDESC    <ELOOP> Too many symbolic links encountered while traversing the path. for file <%s>.
      ZS_INVNAME        <ENAMETOOLONG> pathname is too long for file <%s>.
      ZS_MEMERROR       <ENOMEM> Out of memory (i.e., kernel memory) for file <%s>.
      ZS_BADFILEDESC    <ENOTDIR> A component of the path prefix of pathname is not a directory or referring to a file other than a directory for file <%s>
      ZS_BADFILEDESC    <EOVERFLOW> Pathname or fd refers to a file whose size, inode number, or number of blocks cannot be represented for file <%s>.
 */
ZStatus
uriString::getStatR(uriStat &pZStat, bool pLogZException)const
{
struct statx wStatBuffer ;
passwd *wUserInfo;
  pZStat.clear();
  ZStatus wSt=_getStat(wStatBuffer,pLogZException);
  if (wSt!=ZS_SUCCESS)
    return wSt;

    pZStat.Size=wStatBuffer.stx_size ;
    pZStat.Created.tv_sec = wStatBuffer.stx_btime.tv_sec;
    pZStat.Created.tv_nsec = wStatBuffer.stx_btime.tv_nsec;
    pZStat.LastModified.tv_sec = wStatBuffer.stx_mtime.tv_sec;
    pZStat.LastModified.tv_nsec = wStatBuffer.stx_mtime.tv_nsec;
    pZStat.Rights = wStatBuffer.stx_mode;

    pZStat.Uid = wStatBuffer.stx_uid;

    wUserInfo = getpwuid(wStatBuffer.stx_uid);

    ::strcpy(pZStat.OwnerName , wUserInfo->pw_name);
    return(wSt);
}// getStatR

ZStatus
uriString::_getStat(struct statx& pStat, bool pLogZException) const
{
  ZStatus wSt=ZS_SUCCESS;
  memset(&pStat,0,sizeof(pStat));
  errno= 0; /* errno is set by this function */
  int wFd=0;
  int wRet= statx(wFd,toCChar(),AT_STATX_SYNC_AS_STAT,STATX_ALL,&pStat);
  if (wRet < 0) {
    int wErrno=errno;
    utf8VaryingString wErrMsg;
    wSt= ZS_FILEERROR;
    switch (wErrno) {
    case ENOENT:
      wSt= ZS_FILENOTEXIST;
      wErrMsg.sprintf( "<ENOENT> A component of <%s> does not exist or is a dangling symbolic link.",toString());
      break;
    case EACCES:
      wSt= ZS_USERPRIVILEGES;
      wErrMsg.sprintf( "<EACCES> Search permission is denied for one of the directories in the path prefix of <%s>.",toString());
      break;
    case EBADF:
      wSt= ZS_FILENOTOPEN;
      wErrMsg.sprintf( "<EBADF> File descriptor is not a valid open file descriptor for file <%s>.",toString());
      break;
    case EFAULT:
      wSt= ZS_CORRUPTED;
      wErrMsg.sprintf( "<EFAULT> Bad address for file <%s>.",toString());
      break;
    case EINVAL:
      wSt= ZS_INVPARAMS;
      wErrMsg.sprintf( "<EINVAL> (fstatat()) Invalid flag specified in flags for file <%s>.",toString());
      break;
    case ELOOP:
      wSt= ZS_BADFILEDESC;
      wErrMsg.sprintf( "<ELOOP> Too many symbolic links encountered while traversing the path. for file <%s>.",toString());
      break;
    case ENAMETOOLONG:
      wSt= ZS_INVNAME;
      wErrMsg.sprintf( "<ENAMETOOLONG> pathname is too long for file <%s>.",toString());
      break;
    case ENOMEM:
      wSt= ZS_MEMERROR;
      wErrMsg.sprintf( "<ENOMEM> Out of memory (i.e., kernel memory) for file <%s>.",toString());
      break;
    case ENOTDIR:
      wSt= ZS_BADFILEDESC;
      wErrMsg.sprintf( "<ENOTDIR> A component of the path prefix of pathname is not a directory or referring to a file other than a directory for file <%s>.",toString());
      break;
    case EOVERFLOW:
      wSt= ZS_BADFILEDESC;
      wErrMsg.sprintf( "<EOVERFLOW> Pathname or fd refers to a file whose size, inode number, or number of blocks cannot be represented for file <%s>.",toString());
      break;
    default:
      wErrMsg.sprintf( "Unknown error for file <%s>.",toString());
      break;
    }// switch
    if (pLogZException)
      ZException.getErrno(  wErrno,     /* saved errno */
          _GET_FUNCTION_NAME_,
          wSt,
          Severity_Error,
          wErrMsg.toCChar());

    return(wSt);
  } // if (wRet < 0)

  return ZS_SUCCESS;
}

uriStat
uriString::getStatR(bool pLogZException) const {
  uriStat wRet;
  ZStatus wSt=getStatR(wRet,pLogZException);
  if (wSt!=ZS_SUCCESS) {
    wRet.setInvalid();
  }
  return wRet;
}


/**
 * @brief uriString::getFileSize returns the file size as a long long or -1 if an error occurred (ZException is set with appropriate infos)
 * @return
 */
long
uriString::getFileSize(void) const
{
  __FILEHANDLE__ wFd=-1;
  ZStatus wSt=rawOpen(wFd,*this,O_RDONLY);
  if (wSt!=ZS_SUCCESS)
    return -1;
  size_t wSize=0;
  wSt=rawSeekEnd(wFd,wSize);
  if (wSt!=ZS_SUCCESS)
    return -1;
  return wSize;
}//getFileSize


/**
 @brief uriString::loadContent Allocates memory and loads the content of the file pointed by uriString content
and returns a QBuffer with a Data pointer to memory content and Size of the buffer.

 Size is STRICTLY the size of the file content.

 Data contains the file content WITHOUT '\0' terminating character added.

 In case of error : Data pointer is nullptr and Size is set to -1.

 @warning
 -# loadContent does not assess wether the file content is crypted or not : Crypted flag is ALWAYS returned as FALSE.
 -# it is up to caller to FREE ALLOCATED MEMORY required for storing file content.

 @param[out] pDBS a ZDataBuffer that will contain the file's content. ZDB will be allocated to appropriate memory size.
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
*/


ZStatus
uriString::_loadContent (ZDataBuffer& pDBS) const
{
ZStatus wSt;
uriStat wStat;
    if (!exists()) {
      ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_FILENOTEXIST,
                            Severity_Error,
                            "-E-FILENOTEXIST Requested file %s does not exist \n",
                            toCChar());
      return ZS_NOTFOUND;
    }
   if ((wSt=getStatR(wStat))!=ZS_SUCCESS) {
     if (BaseParameters->VerboseFileEngine())
       ZException.printLastUserMessage(stderr);
      return (wSt);
   }
   if (wStat.Size==0)
     return ZS_EMPTY;

   __FILEHANDLE__ wFd;
   wSt=rawOpen(wFd,toCChar(), O_RDONLY);
   if (wFd<0)
     return wSt;

    pDBS.allocateBZero(wStat.Size);

    ssize_t wS=::read(wFd,pDBS.Data,pDBS.Size);
    if (wS < 0)
    {
      ZException.getErrno(errno,
          _GET_FUNCTION_NAME_,
          ZS_READERROR,
          Severity_Severe,
          "Error opening file for reading <%s> ",
          toString());
      if (BaseParameters->VerboseFileEngine())
        ZException.printLastUserMessage();
      ::rawClose(wFd);
      return ZS_READERROR;
    }

    wSt=rawRead(wFd,pDBS,wStat.Size);


    rawClose(wFd);
    return(ZS_SUCCESS);
}//loadContent



ZStatus
uriString::loadContent (ZDataBuffer& pDBS) const
{
  ZStatus wSt;
//  uriStat wStat;
  if (!exists()) {
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_FILENOTEXIST,
        Severity_Error,
        "-E-FILENOTEXIST Requested file %s does not exist \n",
        toCChar());
    return ZS_FILENOTEXIST;
  }

  __FILEHANDLE__ wFd;
  wSt=rawOpen(wFd,toCChar(),O_RDONLY);
  if (wSt != ZS_SUCCESS)
    return wSt;

  __off_t wSOff = 0;
  wSt=rawSeek(wFd,wSOff,SEEK_END); /* set to end of file and get back file size in wS */
  if (wSt!=ZS_SUCCESS)  {
    rawClose(wFd);
    return wSt;
  }
  if (wSOff==0) {
    rawClose(wFd);
    return ZS_EMPTY;
  }
  __off_t wSOff1 = 0;
  wSt=rawSeek(wFd,wSOff1,SEEK_SET);  /* set to beginning of file with dummy offset*/
  if (wSt!=ZS_SUCCESS) {
    rawClose(wFd);
    return wSt;
  }
  wSt=rawRead(wFd,pDBS,wSOff);

  rawClose(wFd);
  return wSt;
}//loadContent


ZStatus uriString::loadContentAt(ZDataBuffer &pDBS, __off_t pOffset, size_t pSizeMax) const
{
    ZStatus wSt;
    //  uriStat wStat;
    if (!exists()) {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_FILENOTEXIST,
                              Severity_Error,
                              "-E-FILENOTEXIST Requested file %s does not exist \n",
                              toCChar());
        return ZS_FILENOTEXIST;
    }


    __FILEHANDLE__ wFd;
    wSt=rawOpen(wFd,toCChar(),O_RDONLY);
    if (wSt != ZS_SUCCESS)
        return wSt;

    __off_t wSOff = 0;
    wSt=rawSeek(wFd,wSOff,SEEK_END); /* set to end of file and get back file size in wS */
    if (wSt!=ZS_SUCCESS)  {
        rawClose(wFd);
        return wSt;
    }
    if (wSOff==0) {
        rawClose(wFd);
        return ZS_EMPTY;
    }

//    if ((pOffset +  __off_t(pSizeMax) ) > wSOff ) {
        if ( pOffset >= wSOff) {
            return ZS_OUTBOUNDHIGH;
        }
//        pSizeMax = size_t(wSOff - pOffset) ;

//    }

    wSt=rawSeek(wFd,pOffset,SEEK_SET);  /* set to beginning of file with dummy offset*/
    if (wSt!=ZS_SUCCESS) {
        rawClose(wFd);
        return wSt;
    }
    wSt=rawRead(wFd,pDBS,pSizeMax);

    rawClose(wFd);
    return wSt;
}//loadContent

ZStatus
uriString::loadContentZTerm (ZDataBuffer& pDBS)
{
    ZStatus wSt=loadContent(pDBS);
    if (wSt!=ZS_SUCCESS)
            return wSt;
    pDBS.addConditionalTermination();
    return ZS_SUCCESS;
}

ZStatus uriString::loadAES256(ZDataBuffer &pDBS,
                              const ZCryptKeyAES256 &pKey,
                              const ZCryptVectorAES256 &pVector)
{
    ZStatus wSt = loadContent(pDBS);
    if (wSt != ZS_SUCCESS)
        return wSt;
    return pDBS.uncryptAES256(pKey, pVector);
}


ZStatus
uriString::loadUtf8(utf8VaryingString &pUtf8) const
{
ZStatus wSt;
ZDataBuffer wZDBContent;

    wSt=loadContent(wZDBContent);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    utf8_t* wUtf8Ptr= (utf8_t*)wZDBContent.getData();

    size_t wIdx=0;
    size_t wBOMSize=0;
    size_t wUnits=wZDBContent.getByteSize();

    /* detect any BOM : if encountered skip if utf8 BOM, return error if not */

    ZBOM_type wBOM=detectBOM(wZDBContent.Data, wZDBContent.getByteSize(), wBOMSize);
    if (wBOM!=ZBOM_NoBOM)
        {
        if (wBOM!=ZBOM_UTF8)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_IVBOM,
                                      Severity_Error,
                                      "Invalid BOM encountered <%s> while expecting Utf8 BOM \n",
                                      _GET_FUNCTION_NAME_,
                                      decode_ZBOM(wBOM));
                return ZS_IVBOM;
                }
        wUtf8Ptr += wBOMSize;
        wIdx += wBOMSize;
        }
    pUtf8.allocateBytes(wUnits-wBOMSize);
    while (wIdx < wUnits)
            pUtf8.Data[wIdx++]=(utf8_t)*wUtf8Ptr++;

    pUtf8.addConditionalTermination();

    return ZS_SUCCESS;

}//loadUtf8

ZStatus
uriString::loadUtf16(utf16VaryingString &pUtf16) const
{
ZStatus wSt;
ZDataBuffer wZDBContent;
                             /* if no BOM found, assuming endianness is same as default system endianness */
ZBool wProcessEndian= false; /* no big / little endian processing requested by default*/

    wSt=loadContent(wZDBContent);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    utf16_t* wUtf16Ptr= (utf16_t*)wZDBContent.getData();

    size_t wIdx=0;
    size_t wBOMSize=0;
    size_t wBytes=wZDBContent.getByteSize();

    /* detect any BOM : if encountered skip if utf16 BOM, return error if not
        if BOM is utf16 LE and system is big endian
        or BOM is utf16 BE and system is little endian
        process endianness while loading
        if not : straight forward
    */

    ZBOM_type wBOM=detectBOM(wZDBContent.Data, wZDBContent.getByteSize(), wBOMSize);
    if (wBOM!=ZBOM_NoBOM)
        {
        if ((wBOM & ZBOM_UTF16)!=ZBOM_UTF16)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_IVBOM,
                                      Severity_Error,
                                      "Invalid BOM encountered <%s> while expecting Utf16 BOM \n",
                                      decode_ZBOM(wBOM));
                return ZS_IVBOM;
                }

        wProcessEndian = !((wBOM & ZBOM_LE) && is_little_endian());
        wUtf16Ptr ++;
        }
    wBytes -= wBOMSize;

    size_t wUnits=wBytes/sizeof(utf32_t);

    pUtf16.allocateUnits(wUnits);
    if (wProcessEndian)
        while (wIdx < wUnits)
            pUtf16.Data[wIdx++]=forceReverseByteOrder<utf16_t>(*wUtf16Ptr++);
     else
        while (wIdx < wUnits)
            pUtf16.Data[wIdx++]=*wUtf16Ptr++;

    pUtf16.addConditionalTermination();

    return ZS_SUCCESS;

}//loadUtf16
ZStatus
uriString::loadUtf32(utf32VaryingString &pUtf32) const
{
ZStatus wSt;
ZDataBuffer wZDBContent;
                             /* if no BOM found, assuming endianness is same as default system endianness */
ZBool wProcessEndian= false; /* no big / little endian processing requested by default*/

    wSt=loadContent(wZDBContent);
    if (wSt!=ZS_SUCCESS)
                return wSt;

    utf32_t* wUtf32Ptr= (utf32_t*)wZDBContent.getData();

    size_t wIdx=0;
    size_t wBOMSize=0;
    size_t wBytes=wZDBContent.getByteSize();

    /* detect any BOM : if encountered skip if utf16 BOM, return error if not
        if BOM is utf16 LE and system is big endian
        or BOM is utf16 BE and system is little endian
        process endianness while loading
        if not : straight forward
    */

    ZBOM_type wBOM=detectBOM(wZDBContent.Data, wZDBContent.getByteSize(), wBOMSize);
    if (wBOM!=ZBOM_NoBOM)
        {
        if ((wBOM & ZBOM_UTF32)!=ZBOM_UTF32)
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_IVBOM,
                                      Severity_Error,
                                      "Invalid BOM encountered <%s> while expecting Utf32 BOM \n",
                                      decode_ZBOM(wBOM));
                return ZS_IVBOM;
                }

        wProcessEndian = !((wBOM & ZBOM_LE) && is_little_endian());
        wUtf32Ptr += wBOMSize;
        }
    wBytes -= wBOMSize;
    size_t wUnits=wBytes/sizeof(utf32_t);
    pUtf32.allocateUnits(wUnits);

    if (wProcessEndian)
        while (wIdx < wUnits)
            pUtf32.Data[wIdx++]=forceReverseByteOrder<utf32_t>(*wUtf32Ptr++);
     else
        while (wIdx < wUnits)
            pUtf32.Data[wIdx++]=*wUtf32Ptr++;

    pUtf32.addConditionalTermination();

    return ZS_SUCCESS;
}//loadUtf32

/**
 @brief uriString::writeContent writes the content pointed by a ZDataBuffer given by pDBS to the file pointed by uriString content
 and returns a ZStatus

In case of error :returned ZStatus is appropriately set

@warning
 -# writeContent does not assess wether the file content has to be crypted or not.
 -# it is up to caller to FREE ALLOCATED MEMORY required for storing file content.
* @param[in] pDBS a ZDataBuffer with the content to be written in file
* @param[in] pAccessRights int with access right mask  defaulted to S_IRWXU|S_IRWXG|S_IROTH
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
*/

ZStatus
uriString::writeContent (ZDataBuffer& pDBS,__FILEACCESSRIGHTS__ pAccessRights) const
{
  __FILEHANDLE__ wFd ;
  ZStatus wSt= rawOpen(wFd,*this, O_WRONLY | O_CREAT | O_TRUNC,pAccessRights);
  if (wSt!=ZS_SUCCESS)
    return wSt;
/* posix_fallocate bug
 * when open with mode O_WRONLY | O_CREAT
 * EBADF returned if file already exists.
 *
  wSt=rawAllocate(wFd,__off_t(0),__off_t(pDBS.Size));
  if (wSt!=ZS_SUCCESS)
    return wSt;

  wSt=rawSeekBegin(wFd);
  if (wSt!=ZS_SUCCESS)
    return wSt;
*/
  size_t wSizeWritten=0;
  wSt=rawWrite(wFd,pDBS,wSizeWritten);

  rawClose(wFd);
  return wSt;
}//writeContent


ZStatus
uriString::appendContent (ZDataBuffer& pDBS,__FILEACCESSRIGHTS__ pAccessRights) const
{
  __FILEHANDLE__ wFd =-1;
  ZStatus wSt = rawOpen(wFd,toCChar(),O_WRONLY | O_APPEND,pAccessRights);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  size_t wSk;
  wSt=rawSeekEnd(wFd,wSk);
  if (wSt!=ZS_SUCCESS) {
    goto appendContentEnd;
  }

  wSt=rawWrite(wFd,pDBS,wSk);
  if (wSt!=ZS_SUCCESS)
    return wSt;
appendContentEnd:
  rawClose(wFd);
  return wSt;
}//appendContent

ZStatus
uriString::writeContent (utf8VaryingString &pStr, __FILEACCESSRIGHTS__  pAccessRights) const
{
  ZDataBuffer wDBS;
/*  __FILEHANDLE__ wFd ;
  ZStatus wSt= rawOpen(wFd,*this, O_WRONLY | O_CREAT,pAccessRights);
  if (wSt!=ZS_SUCCESS)
    return wSt;
*/
  size_t wStrLen = pStr.strlen();
  wDBS.allocateBZero(sizeof(uint8_t)*(wStrLen));
  size_t wi=0;
  while (wi < wStrLen) {
    wDBS.Data[wi] = pStr.Data[wi];
    wi++;
  }

  return writeContent(wDBS,pAccessRights);
/*
  wSt=rawAllocate(wFd,__off_t(0),__off_t(wDBS.Size));
  if (wSt!=ZS_SUCCESS)
    return wSt;

  wSt=rawSeekBegin(wFd);
  if (wSt!=ZS_SUCCESS)
    return wSt;


  size_t wSizeWritten=0;
  wSt=rawWrite(wFd,wDBS,wSizeWritten);

  rawClose(wFd);
  return wSt;
*/
}//writeContent

ZStatus
uriString::appendContent (const utf8VaryingString &pStr) const
{
  ZDataBuffer wDBS;
  size_t wStrLen = pStr.strlen();
  wDBS.allocateBZero(sizeof(uint8_t)*(wStrLen));
  size_t wi=0;
  while (wi < wStrLen) {
    wDBS.Data[wi] = pStr.Data[wi];
    wi++;
  }

  return appendContent(wDBS);
}//appendContent

ZStatus
uriString::createFile (__FILEACCESSRIGHTS__ pAccessRights) const
{
    __FILEHANDLE__ wFd ;
    ZStatus wSt= rawOpen(wFd,*this, O_WRONLY | O_CREAT | O_TRUNC,pAccessRights);
    if (wSt!=ZS_SUCCESS)
        return wSt;
    return rawClose(wFd);
}//createFile

ZStatus uriString::writeAES256(ZDataBuffer &pDBS,
                               const ZCryptKeyAES256 &pKey,
                               const ZCryptVectorAES256 &pVector,
                                __FILEACCESSRIGHTS__ pAccessRights)
{
  ZDataBuffer wDBS = pDBS;
  ZStatus wSt=wDBS.encryptAES256toFile(toCChar(), pKey, pVector);
    if (wSt!=ZS_SUCCESS)
      return wSt;
    return writeContent(wDBS,pAccessRights);
}
/**
 * @brief uriString::getChecksum gives the checksum of the file's content current uriString points to.
 * SHA256 checksum is used.
 * @return a checkSum object
 */
checkSum
uriString::getChecksum (void) const
{
checkSum wChecksum;
    wChecksum.clear();

ZDataBuffer wDBS;
    ZStatus wSt= loadContent(wDBS );
    if (wSt==ZS_SUCCESS)
            wChecksum.compute(wDBS.Data,wDBS.Size);
/* if not successfull ZException is positionned */
    return(wChecksum);
}//getChecksum


uriString operator + (const uriString pIn1,const uriString pIn2)
{
    uriString wReturn(pIn1);
    wReturn.add(pIn2);
    return wReturn;
}




#endif // URISTRING_CPP
