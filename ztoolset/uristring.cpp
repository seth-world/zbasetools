#ifndef URISTRING_CPP
#define URISTRING_CPP
#include <zconfig.h>

#include <ztoolset/uristring.h>
#include <ztoolset/zexceptionmin.h>
#include <ztoolset/zdatabuffer.h>

#include <ztoolset/zutfstrings.h>
#include <ztoolset/utfvaryingstring.h> // for utf8VaryingString utf16VaryingString and utf32VaryingString

#include <zio/zdir.h>

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
    if (pURI==nullptr)
                    _ABORT_
    if (Data==nullptr)
      allocateUnits(pURI->UnitCount);
    strset(pURI->toUtf());
    return(*this);
}
uriString&
uriString::fromURI(const uriString& pURI)
{
  if (pURI==nullptr)
    _ABORT_
  if (Data==nullptr)
    allocateUnits(pURI.UnitCount);
  strset(pURI.toUtf());
  _Base::CheckData();
  return(*this);
}

ZStatus
uriString::operator << (ZDataBuffer& pDBS)
{
    return writeContent (pDBS) ;
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
uriString::list(ZArray<uriString>* pList,ZDir_File_type pZDFT)
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


/**
 * @brief uriString::addMimeFileExtension  add to the uriString the file extension corresponding to the given Mime name.
 *  if uriString already has an extension, then it will be replaced by the given extension out of Mime name.
 *  - returns false if Mime name does not give a file extension,
 *  - returns true otherwise.
 *
 * @param pMimeName
 * @return
 */
bool
uriString::addMimeFileExtension (utfdescString pMimeName)
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

utf8String uriString::getFileExtension() const
{
  if (isEmpty())
    return utf8String ("");

    utf8String wExt;
    utf8_t *wPtr = strchr((utf8_t)'.');
    if (wPtr==nullptr)
      return utf8String("");
    wPtr++;
    wExt=wPtr;
    return wExt;
}

/**
 * @brief uriString::getDirectoryPath Returns a descString containing the file's directory path
        i. e. </directory path/><root base name>.<extension>

 * @return an utf8String with the file's directory path
 */
utf8String
uriString::getDirectoryPath () const
{
  if (isEmpty())
    return utf8String ("");
  utf8String wDir(Data);
 //   utf8_t *wPtr = pExt.strrchr((utf8_t)Delimiter);
    utf8_t *wPtr = &wDir.Data[wDir.UnitCount-1];
    while ((wPtr > wDir.Data) && (*wPtr!=Delimiter))
      wPtr--;
    wPtr++;
    while (wPtr < &wDir.Data[wDir.UnitCount])
        *wPtr++=0;
  return wDir;
}//getDirectoryPath
/**
 * @brief uriString::getLastDirectoryName Returns a descString containing the file's directory path
        i. e. </directory path/><root base name>.<extension>

 * @return a descString with the file's directory path
 */
utf8String
uriString::getLastDirectoryName() const
{
  if (isEmpty())
    return utf8String ("");

    utf8String wDir;

    const utf8_t* wPtr1=Data+(UnitCount-1);
    while((wPtr1>Data)&&(*wPtr1!=(utf8_t)Delimiter))
            wPtr1--;

    const utf8_t *wPtr2=--wPtr1;

    while((wPtr2>Data)&&(*wPtr2!=(utf8_t)Delimiter))
            wPtr2--;
    if (wPtr2>Data)
            wPtr2++;

    wDir = wPtr2;
    utf8_t* wPtr3=wDir.Data;
    while (*wPtr3 &&(*wPtr3!=(utf8_t)Delimiter))
      *wPtr3++;
    while (wPtr3 < &wDir.Data[wDir.UnitCount])
        *wPtr3++=0;
    return wDir;
}//getLastDirectoryName
/**
 * @brief uriString::getBasename       returns a utf8String containing the file's full base name

 i. e. </directory path/><root base name>.<extension>
 Where full base name is : <root base name>.<extension> without directory path

 * @return a descString with the file's base name
 */

utf8String
uriString::getBasename ()const
{
  if (isEmpty())
    return utf8String ("");

utf8String wBasename;
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

/**
 * @brief uriString::getRootname returns a utf8String containing the file's root name
 *
 *  i. e. </directory path/><root name>.<extension>
 * @return a utf8String with the file's root base name
 */
utf8String
uriString::getRootname () const
{
  if (isEmpty())
    return utf8String ("");

  utf8_t* wPtrEnd=nullptr;
    utf8String wRoot=getBasename();
    utf8_t* wPtr = utfStrchr<utf8_t>(wRoot.Data,'.');
    if (wPtr!=nullptr)
      {
      wPtrEnd = &wRoot.Data[wRoot.ByteSize];
      while (wPtr < wPtrEnd)
            *wPtr++=0;
      }
    return wRoot;
}//getRootBasename
#include <limits.h>
#include <stdlib.h>
utf8String
uriString::getUrl () const
{
  if (::strncmp((const char*)Data,"file://",7)==0)
    return utf8String(Data);

  utf8String wReturn;
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
  wReturn = wPtr;
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
utf8String wBaseName;
    wBaseName = getBasename();

    strset ((const utf8_t*)pDirectoryPath.toCChar());
    addConditionalDirectoryDelimiter();
    add(wBaseName);

    return *this;
}//setDirectoryPath

utf8String
uriString::getUniqueName (const char* pRootName)
{
    char wTempName[30];
    ::strncpy(wTempName,pRootName,20);
    ::strncat(wTempName,"XXXXXX",29);

#ifdef __USE_WINDOWS__
    return utf8String((utf8_t*)_mktemp(wTempName));
#else
    return utf8String((utf8_t*)mktemp(wTempName));
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


ZStatus
uriString::renameBck(const char* pBckExt)
{
if (!exists())
  return ZS_FILEERROR;
int wFormerNumber = 1;

  uriString wFormerURI(*this);

  while (wFormerURI.exists())
    {
    wFormerNumber ++;
    wFormerURI.sprintf("%s_%s%02ld",toCChar(),pBckExt,wFormerNumber);
    }
  int wRet= rename(toCChar(),wFormerURI.toCChar());
  if (wRet!=0)
    {
    ZException.getErrno(errno,
        _GET_FUNCTION_NAME_,
        ZS_FILEERROR,
        Severity_Severe,
        "Error renaming existing file <%s> to <%s>\n",
        toCChar(),
        wFormerURI.toCChar());
    return ZS_FILEERROR;
    }

  if (__ZRFVERBOSE__)
    fprintf (stdout,
        "Renamed existing file <%s> to <%s> \n",
        toCChar(),
        wFormerURI.toCChar());
  return ZS_SUCCESS;
}
//
//---------String search and handling
//




#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
/**
 * @brief uriString::exists Test if current file description stored in current uriString exists as a file or a directory
 * @return true if exists, false if not
 */
bool
uriString::exists(void) const
{
struct stat wStatBuffer ;
    int wSt= stat(toCChar(),&wStatBuffer);
    if (wSt<0)
            {
            return(false);
            }
    return(true);
}

void    uriString::setcwd()
{
  char wBuf[cst_urilen];

  strset((const utf8_t*)getcwd((char *)wBuf,cst_urilen));
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
uriString::isDirectory(void)
{
struct stat wStatBuffer ;

    int wSt= stat(toCString_Strait(),&wStatBuffer);
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
    int wSt=remove(toCString_Strait());
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


ZStatus uriString::changeAccessRights(mode_t pMode)
{
  errno=0;
  int wRet=chmod(toCChar(),pMode);
  if (wRet<0)
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

return ZS_SUCCESS;
}

//uriStat_struct ZStat;



/**
 * @brief uriString::getStatR  getfile attributes information from file system for the current uriString filename
 * Return ZS_FILEERROR in case of failure and then, ZException is filled with error information
 * @param[in] pURI
 * @param[out] pZStat
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
 */
ZStatus
uriString::getStatR(uriStat &pZStat)const
{
struct stat wStatBuffer ;
passwd *wUserInfo;

    pZStat.clear();
    int wSt= stat(toCChar(),&wStatBuffer);
    if (wSt<0)
            {
            ZException.setMessage (_GET_FUNCTION_NAME_,
                                   ZS_FILEERROR,
                                   Severity_Error,
                                   "error calling stat for file %s",
                                   toCChar());
            return(ZS_FILEERROR);
            }

    pZStat.Size=wStatBuffer.st_size ;
    pZStat.Created.fromTimespec(wStatBuffer.st_ctim);
    pZStat.LastModified.fromTimespec((wStatBuffer.st_mtim));
    pZStat.Rights = wStatBuffer.st_mode;

    pZStat.Uid = wStatBuffer.st_uid;

    wUserInfo = getpwuid(wStatBuffer.st_uid);

    ::strcpy(pZStat.OwnerName , wUserInfo->pw_name);
    return(ZS_SUCCESS);
}// getStatR


/**
 * @brief uriString::getFileSize returns the file size as a long long or -1 if an error occurred (ZException is set with appropriate infos)
 * @return
 */
long long
uriString::getFileSize(void) const
{
 uriStat wStat;
 if (getStatR(wStat)!=ZS_SUCCESS)
                 {
                 return (-1);
                 }
    return(wStat.Size);
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
uriString::loadContent (ZDataBuffer& pDBS) const
{
ZStatus wSt;
uriStat wStat;
    if (!exists())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_NOTFOUND,
                                  Severity_Error,
                                  "-E-FILENOTFOUND Requested file %s does not exist \n",
                                  toCChar());
/*    #if __DEBUG_LEVEL__ > 1
            ZException.printUserMessage();
    #endif
*/
            return ZS_NOTFOUND;
          }
   if ((wSt=getStatR(wStat))!=ZS_SUCCESS)
                {
                if (ZVerbose)
                        ZException.printLastUserMessage(stderr);
                return (wSt);
                }


    FILE *wFile =fopen(toCChar(),"rb");

    if (wFile==nullptr)
            {
            ZException.getErrno(errno);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_ERROPEN,
                                  Severity_Error,
                                  "-E-ERROPEN error on fopen file for read  <%s> \n",
                                  toCChar());
            if (ZVerbose)
                ZException.printLastUserMessage(stderr);
            return ZS_ERROPEN;
            }

    if (wStat.Size==0)
            return ZS_EMPTY;
    pDBS.allocateBZero(wStat.Size);

    fread(pDBS.Data,pDBS.Size,1,wFile);

    if (!feof(wFile))
            if (ferror(wFile))
                        {
                        ZException.getFileError(wFile);
                        ZException.setMessage(_GET_FUNCTION_NAME_,
                                              ZS_FILEPOSERR,
                                              Severity_Error,
                                              "E-ERROPEN error reading file  <%s> \n %s",
                                              toCChar());
                        fclose(wFile);
                        pDBS.freeData();
                        return (ZS_FILEERROR);
                        }
    fclose(wFile);
    return(ZS_SUCCESS);
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

    ZBOM_type wBOM=detectBOM(wZDBContent.DataChar, wZDBContent.getByteSize(), wBOMSize);
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

    ZBOM_type wBOM=detectBOM(wZDBContent.DataChar, wZDBContent.getByteSize(), wBOMSize);
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

    ZBOM_type wBOM=detectBOM(wZDBContent.DataChar, wZDBContent.getByteSize(), wBOMSize);
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
* @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.@see ZBSError
*/

ZStatus
uriString::writeContent (ZDataBuffer& pDBS)
{

    FILE *wFile =fopen(toCChar(),"wb");

    if (wFile==nullptr)
            {
            ZException.getErrno(errno);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_ERROPEN,
                                  Severity_Error,
                                  "-E-ERROPEN error on fopen file for writing  <%s> \n",
                                  toCChar());
            if (ZVerbose)
                ZException.printLastUserMessage(stderr);
            return ZS_ERROPEN;
            }

    fwrite(pDBS.Data,pDBS.Size,1,wFile);


    if (ferror(wFile))
                {
                ZException.getFileError(wFile,
                                        _GET_FUNCTION_NAME_,
                                      ZS_FILEERROR,
                                      Severity_Error,
                                      "E-ERROPEN error writing to file  <%s> ",
                                      toCChar());
                fclose(wFile);
                return (ZS_FILEERROR);
                }
    fclose(wFile);
    return(ZS_SUCCESS);
}//writeContent
ZStatus
uriString::appendContent (ZDataBuffer& pDBS)
{

  FILE *wFile =fopen(toCChar(),"ab");

  if (wFile==nullptr)
  {
    ZException.getErrno(errno);
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Error,
        "-E-ERROPEN error on fopen file for writing  <%s> \n",
        toCChar());
    if (ZVerbose)
      ZException.printLastUserMessage(stderr);
    return ZS_ERROPEN;
  }

  fwrite(pDBS.Data,pDBS.Size,1,wFile);


  if (ferror(wFile))
  {
    ZException.getFileError(wFile,
        _GET_FUNCTION_NAME_,
        ZS_FILEERROR,
        Severity_Error,
        "E-ERROPEN error writing to file  <%s> ",
        toCChar());
    fclose(wFile);
    return (ZS_FILEERROR);
  }
  fclose(wFile);
  return(ZS_SUCCESS);
}//writeContent

ZStatus
uriString::writeContent (utf8VaryingString &pStr)
{

    FILE *wFile =fopen(toCChar(),"w");

    if (wFile==nullptr)
            {
            ZException.getErrno(errno);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_ERROPEN,
                                  Severity_Error,
                                  "-E-ERROPEN error on fopen file for writing  <%s> \n",
                                  toCChar());
            if (ZVerbose)
                ZException.printLastUserMessage(stderr);
            return ZS_ERROPEN;
            }


//    fwrite(pStr.Data,pStr.strlen()+1,1,wFile);
    fwrite(pStr.Data,pStr.strlen(),1,wFile);


    if (ferror(wFile))
                {
                ZException.getFileError(wFile,
                                        _GET_FUNCTION_NAME_,
                                      ZS_FILEERROR,
                                      Severity_Error,
                                      "E-ERRWRITE error writing to file  <%s> ",
                                      toCChar());
                if (ZVerbose)
                    ZException.printLastUserMessage(stderr);
                fclose(wFile);
                return (ZS_FILEERROR);
                }
    fclose(wFile);
    return(ZS_SUCCESS);
}//writeContent
ZStatus
uriString::appendContent (utf8VaryingString &pStr)
{

  FILE *wFile =fopen(toCChar(),"a");

  if (wFile==nullptr)
  {
    ZException.getErrno(errno);
    ZException.setMessage(_GET_FUNCTION_NAME_,
        ZS_ERROPEN,
        Severity_Error,
        "-E-ERROPEN error on fopen file for writing  <%s> \n",
        toCChar());
    if (ZVerbose)
      ZException.printLastUserMessage(stderr);
    return ZS_ERROPEN;
  }


  //    fwrite(pStr.Data,pStr.strlen()+1,1,wFile);
  fwrite(pStr.Data,pStr.strlen(),1,wFile);


  if (ferror(wFile))
  {
    ZException.getFileError(wFile,
        _GET_FUNCTION_NAME_,
        ZS_FILEERROR,
        Severity_Error,
        "E-ERRWRITE error writing to file  <%s> ",
        toCChar());
    if (ZVerbose)
      ZException.printLastUserMessage(stderr);
    fclose(wFile);
    return (ZS_FILEERROR);
  }
  fclose(wFile);
  return(ZS_SUCCESS);
}//appendContent
ZStatus uriString::writeAES256(ZDataBuffer &pDBS,
                               const ZCryptKeyAES256 &pKey,
                               const ZCryptVectorAES256 &pVector)
{
    return pDBS.encryptAES256toFile(toCChar(), pKey, pVector);
}
/**
 * @brief uriString::getChecksum gives the checksum of the file's content current uriString points to.
 * SHA256 checksum is used.
 * @return a checkSum object
 */
checkSum
uriString::getChecksum (void)
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


/*
ZStatus
uriString::writeText (varyingCString& pDBS)
{
ZStatus wSt;
    FILE *wFile =fopen(content,"w");

    if (wFile==nullptr)
            {
            ZException.getErrno(errno);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_ERROPEN,
                                  Severity_Error,
                                  "-E-ERROPEN error on fopen file for writing  <%s> \n",
                                  content);
            if (ZVerbose)
                ZException.printLastUserMessage(stderr);
            return ZS_ERROPEN;
            }

    fwrite(pDBS.DataChar,pDBS.Size,1,wFile);


    if (ferror(wFile))
                {
                ZException.getFileError(wFile,
                                        _GET_FUNCTION_NAME_,
                                      ZS_FILEERROR,
                                      Severity_Error,
                                      "E-ERROPEN error writing to file  <%s> ",
                                      content);
                fclose(wFile);
                return (ZS_FILEERROR);
                }
    fclose(wFile);
    return(ZS_SUCCESS);
}//writeText
*/


#ifdef __COMMENT__

/**
 * @brief _exportURF export current varying utf string into an URF format.
 *  Container used is a ZDataBuffer that may be provided by calling procedure.
 *  Universal Record Format is ALWAYS big endian regardless what platform is used.
 *
 *  URF format for an utf Varying string.
 *
 *  URF header depends on the type of object given by the first header information : ZTypeBase (ZType_type)
 *  for an utfVaryingString, header is as follows
 *
 *  URF data is converted to Universal format.
 *  Universal format is big endian with special encoding for atomic data that allows to sort on the whole data in a coherent, simple maner.
 *
 *header :
 *   - ZTypeBase : ZType_utf8VaryingString, ZType_utf16VaryingString,... Gives the Character Unit size with ZType_AtomicMask
 *   - uint64_t number of character units effectively contained within the string.
 *
 *
 *  see <ZType_Type vs Header sizes> in file <zindexedfiles/znaturalfromurf.cpp>
 *data :
 *  a suite of _Utf character unit sequense up to number of characters units.
 *  _Utf character units are converted to big endian if necessary.
 *
 * limits :
 *
 *  Maximum size is in bytes :      	18446744073709551615 (2^64-1)
 *
 * @param pURF  a ZDataBuffer that will contain URF data in return.
 * @return
 */
ZDataBuffer
uriString::_exportURF()
{
  ZDataBuffer wReturn;
  unsigned char* wURF_Ptr;
  URF_Varying_Size_type wByteSize=(URF_Varying_Size_type)ByteSize;

  size_t wUnitCount=strlen();

  wURF_Ptr=wReturn.allocateBZero(wUnitCount+sizeof(ZTypeBase)+sizeof(URF_Varying_Size_type));

  wURF_Ptr=setURFBufferValue<ZTypeBase>(wURF_Ptr,ZType);
  wURF_Ptr=setURFBufferValue<URF_Varying_Size_type>(wURF_Ptr,wByteSize);

  utfSetReverse<utf8_t>(wURF_Ptr,content,wUnitCount);

  return wReturn;
}//_exportURF



/**
 * @brief utfVaryingString<_Utf>::_importURF imports URF data from pURF and feeds current utfVaryingString object.
 * @param pUniversal a pointer to URF header data
 * @return  ZStatus ZS_SUCCESS if everything went well - ZS_INVTYPE is URF data has not the appropriate ZType_type.
 */

size_t
uriString::_importURF(unsigned char* pURF)
{
  ZTypeBase               wType=ZType_Nothing;
  URF_Varying_Size_type   wUniversalSize=0;
  utf8_t*                 wOutPtr=nullptr;
  size_t                  wUnits=0;
  pURF=getURFBufferValue<ZTypeBase>(&wType,pURF);

  /* ZType control */
  if (wType!= getZType())
  {
    return ZS_INVTYPE;
  }

  pURF=getURFBufferValue<URF_Varying_Size_type>(&wUniversalSize,pURF);

  wOutPtr=content;

  wUnits=wUniversalSize/sizeof(utf8_t);

  utfSetReverse<utf8_t>(wOutPtr,pURF,(const size_t)wUnits);

  content[wUnits]='\0';

  return ZS_SUCCESS;
}// _importURF



/**
 * @brief _exportVUniversal  Exports a string to a Universal Varying Format
 *  Universal Varying  Format stores string data into a varying length string container excluding '\0' character terminator
 *  led by
 *   - uint8_t : char unit size
 *   - UVF_Size_type : number of character units of the string.
 * @return a ZDataBuffer with string content in Varying Universal Format set to big endian
 */
ZDataBuffer
uriString::_exportUVF()
{
  ZDataBuffer wUVF;

  /* Count effective char units excluding (_Utf)'\0' mark starting from end */
  UVF_Size_type wUnitCount = 0;
  while ((content[wUnitCount]!=0)&&(wUnitCount < cst_urilen))
    wUnitCount++;

  size_t wByteLen=wUnitCount*sizeof(utf8_t); // get number of bytes out of this

  /* allocate storate for export */
  unsigned char* wPtrTarg=wUVF.allocate((ssize_t)wByteLen+sizeof(UVF_Size_type)+1);

  /* set export data with char unit size */
  *wPtrTarg = (uint8_t)sizeof (utf8_t);
  wPtrTarg++;

  /* prepare and set unit count to export data */
  UVF_Size_type wUnitCount_Export=reverseByteOrder_Conditional<UVF_Size_type>(wUnitCount);
  memmove(wPtrTarg,&wUnitCount_Export,sizeof(UVF_Size_type));
  wPtrTarg += sizeof(UVF_Size_type);

  /* export char units : each char unit must be reversed if necessary (big /little endian) */

  utf8_t* wPtrOut=(utf8_t*)(wPtrTarg);
  utf8_t* wPtrIn=content;

  if (sizeof (utf8_t)==1)
    while (*wPtrIn)
      *wPtrOut++=*wPtrIn++;
  else
    while (*wPtrIn)
      *wPtrOut++=reverseByteOrder_Conditional<utf8_t>(*wPtrIn++);

  return wUVF;
}// _exportUVF



/** gets utf format name from char unit size */
const char* getUnitFormat(uint8_t pSize);



/**
 * @brief _importUVF Import string from Varying Universal Format
 *  Varying Universal Format stores string data into a varying length string container excluding '\0' character terminator
 *  led by a uint16_t mentionning the number of characters of the string that follows.
 * Important : imported string format (utf-xx) must be the same as current string
 * @param pUniversalPtr pointer to Varying Universal formatted data header
 * @return total size IN BYTES of consumed bytes in pUniversalPtr buffer (Overall size of string in UVF)
 */
size_t
uriString::_importUVF(unsigned char* &pUniversalPtr)
{
  errno=0;
  unsigned char* wPtrSrc=pUniversalPtr;
  /* get and control char unit size */
  uint8_t wUnitSize=(uint8_t)*wPtrSrc;
  if (wUnitSize!=sizeof(utf8_t))
  {
    fprintf(stderr,"_importUVF-E-IVUSIZE Imported string format <%s> does not correspond to current string format <%s>",
        getUnitFormat(wUnitSize),
        getUnitFormat(sizeof(utf8_t)));
    return 0;
  }

  wPtrSrc++;
  /* get char units to load excluding (_Utf)'\0' mark */
  UVF_Size_type    wUnitCount;
  size_t wLen = _importAtomic<UVF_Size_type>(wUnitCount,wPtrSrc);
  if (wUnitCount > cst_urilen)
    {
    fprintf(stderr,"uriString::_importUVF-W-TRUNC Overflow : imported string length <%d> truncated to <%d>.\n",
        wUnitCount, cst_urilen);
    errno=ENOMEM;
    }
  /* import string per char unit */

  uint8_t* wPtrOut=content ;
  uint8_t* wPtrIn=(uint8_t*)(wPtrSrc);
  uint8_t* wPtrEnd = &wPtrIn[wUnitCount];
//  if (wUnitSize==1)
    while (wPtrIn < wPtrEnd)
      *wPtrOut++=*wPtrIn++;
/*  else
    while (wPtrIn < wPtrEnd)
      *wPtrOut++=reverseByteOrder_Conditional<uint8_t>(*wPtrIn++);*/
  //  addConditionalTermination();
  *wPtrEnd = 0;
  pUniversalPtr += (wUnitCount*sizeof(uint8_t))+sizeof(UVF_Size_type)+1;
  return (wUnitCount*sizeof(uint8_t))+sizeof(UVF_Size_type)+1;
}// _importUVF
/**
 * @brief _getimportUVFSize() returns total size in byte of data to import, including header.
 */

UVF_Size_type
uriString::_getimportUVFSize(unsigned char* pUniversalPtr)
{
  unsigned char* wPtrSrc=pUniversalPtr;
  /* get and control char unit size */
  uint8_t wUnitSize=(uint8_t)*wPtrSrc;
  if (wUnitSize!=sizeof(uint8_t))
  {
    fprintf(stderr,"uriString::_getimportUVFSize-E-IVUSIZE Imported string format <%s> does not correspond to current string format <%s>",
        getUnitFormat(wUnitSize),
        getUnitFormat(sizeof(uint8_t)));
    return 0;
  }
  wPtrSrc++;

  UVF_Size_type    wUnitCount;
  size_t wLen = _importAtomic<UVF_Size_type>(wUnitCount,wPtrSrc);
  if (wUnitCount > cst_urilen)
  {
    fprintf(stderr,"uriString::_getimportUVFSize-W-TRUNC Overflow : imported string length <%d> truncated to <%d>.\n",
        wUnitCount, cst_urilen);
    errno=ENOMEM;
  }
  wUnitCount *= wUnitSize ;
  wUnitCount += sizeof(UVF_Size_type) + 1 ;
  return wUnitCount;
}//_getimportUVFSize

/**
 * @brief _getexportUVFSize() compute the requested export size IN BYTES for current string
 */
UVF_Size_type
uriString::_getexportUVFSize()
{
  /* Count char units excluding (_Utf)'\0' mark starting from end */
  size_t wUnitCount = 0;
  while ((content[wUnitCount]!=0)&&(wUnitCount < cst_urilen))
    wUnitCount++;

  UVF_Size_type wByteLen=wUnitCount +sizeof(UVF_Size_type)+ 1;
  return wByteLen;

}//_getimportUVFSize

#endif // __COMMENT__


#endif // URISTRING_CPP
