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
                    abort();
    memmove(content,pURI->content, cst_urilen+1);
    return(*this);
}

ZStatus
uriString::operator << (ZDataBuffer& pDBS)
{
    return writeContent (pDBS) ;
}

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


uriString&
uriString::operator = (const utfdescString &pSource) {return fromdescString(pSource);}


/**
* @brief addConditionalDirectoryDelimiter  adds a directory delimiter if and only if there is no delimiter at the end of the uriString yet
*/
uriString&
uriString::addConditionalDirectoryDelimiter(void)
{
    long wS=strlen();
    if ((isEmpty())||(wS<1))
                    return(*this);
    RTrim();
    wS=strlen();
    if (content[wS-1]==(utf8_t)Delimiter)
                            return(*this);
    if (wS+1>cst_urilen)
                    {
                    fprintf(stderr,"%s-F-STRINGOVERFLOW Out of boundaries while appending delimiter char",_GET_FUNCTION_NAME_);
                    abort();
                    }
    content[wS]=(utf8_t)Delimiter;
    wS++;
    content[wS]=(utf8_t)'\0';
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
size_t wS=strlen();

    if (wS>cst_urilen-1)
                {
                fprintf(stderr,"%s-F-STRINGOVERFLOW Out of boundaries while appending delimiter char",_GET_FUNCTION_NAME_);
                abort();
                }
    content[wS]=Delimiter;
    wS++;
    content[wS]='\0';
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
    utf8_t* wPtr=pMimeName.strchr((utf8_t)'/');
    if (wPtr==nullptr)
                return false;
    wPtr++;
    utf8_t* wPtr1 = utfStrchr<utf8_t>((content + 2),(utf8_t)'.');
    if (wPtr1==nullptr)
            {
            wPtr1=&content[strlen()];
            *wPtr1='.';
            }
    wPtr1++;
    *wPtr1=(utf8_t)'\0';
    add(wPtr);
    return true;
}//addMimeFileExtension
//
// ----------PathName searches and extractions
//
utfdescString DSBuffer;

utfdescString uriString::getFileExtension()
{
    utfdescString pExt=content;
    utf8_t *wPtr = strchr((utf8_t)'.');
    if (wPtr!=nullptr)
            { wPtr++; pExt=wPtr;}
    return pExt;
}

/**
 * @brief uriString::getDirectoryPath Returns a descString containing the file's directory path
        i. e. </directory path/><root base name>.<extension>

 * @return a descString with the file's directory path
 */
utfdescString
uriString::getDirectoryPath ()
{
    utfdescString pExt=content;
    utf8_t *wPtr = pExt.strrchr((utf8_t)Delimiter);
    if (wPtr==nullptr)
                pExt.clear();
         else
            {
            wPtr++;
            wPtr[0]='\0';
            }
    return pExt;
}//getDirectoryPath
/**
 * @brief uriString::getDirectoryPath Returns a descString containing the file's directory path
        i. e. </directory path/><root base name>.<extension>

 * @return a descString with the file's directory path
 */
utfdescString
uriString::getLastDirectoryName()
{
    utfdescString pExt;

    utf8_t* wPtr1=content+(UnitCount-1);
    while((wPtr1>content)&&(*wPtr1!=(utf8_t)Delimiter))
            wPtr1--;

    utf8_t *wPtr2=--wPtr1;

    while((wPtr2>content)&&(*wPtr2!=(utf8_t)Delimiter))
            wPtr2--;
    pExt.clear();
    if (wPtr2>content)
            wPtr2++;
    utf8_t* wPtr3=pExt.content;
    while (wPtr2 < wPtr1)
            *wPtr3++=*wPtr2++;
    return pExt;
}//getLastDirectoryName
/**
 * @brief uriString::getBasename       returns a descString containing the file's full base name

 i. e. </directory path/><root base name>.<extension>
 Where full base name is : <root base name>.<extension> without directory path

 * @return a descString with the file's base name
 */

utfdescString
uriString::getBasename ()
{
utfdescString pExt;
    utf8_t *wPtr = strrchr((utf8_t)Delimiter);
    if (wPtr==nullptr)
                pExt.fromUtf( content ) ;
         else
            {
            wPtr++;
            pExt.fromUtf( wPtr  );
            }

/*    wPtr = pExt.strrchr((utf8_t)'.');
    if (wPtr==nullptr)
                return pExt;
    *wPtr=(utf8_t)'\0';*/
    return pExt;
} // getBasename

/**
 * @brief uriString::getRootBasename returns a descString containing the file's root base name
 *
 *  i. e. </directory path/><root base name>.<extension>
 * @return a descString with the file's root base name
 */
utfdescString
uriString::getRootBasename ()
{

    utfdescString pExt=getBasename();
    utf8_t *wPtr = pExt.strchr((utf8_t)'.');
    if (wPtr!=nullptr)
            {
            *wPtr='\0';
            }
    return pExt;
}//getRootBasename

/**
 * @brief uriString::setDirectoryPath Replaces the current directory path (if any) with the directory path mentionned in given uriString (pDirectoryPath)
  * @warning pDirectoryPath must be tested BEFORE using this method to be a valid directory.
 * @param pDirectoryPath an uriString that will contain the directory path that will replace the current one
 * @return a reference to current uriString object
 */
uriString&
uriString::setDirectoryPath (uriString &pDirectoryPath)
{
utfdescString wBaseName;
    wBaseName = getBasename();

    strset (pDirectoryPath.toString());
    addConditionalDirectoryDelimiter();
    add(wBaseName.content);

    return *this;
}//setDirectoryPath

utfdescString
uriString::getUniqueName (const char* pRootName)
{
    char wTempName[30];
    strncpy(wTempName,pRootName,20);
    strncat(wTempName,"XXXXXX",29);

#ifdef __USE_WINDOWS__
    return utfdescString((utf8_t*)_mktemp(wTempName));
#else
    return utfdescString((utf8_t*)mktemp(wTempName));
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
    utf8_t* wPtr=strrchr( '.');

    if (wPtr==nullptr)
            {
            add((utf8_t*)".");
            add((utf8_t*)pExtension);
            return;
            }
    wPtr++;
    utf8_t* wPtrmax=content + (UnitCount-1);
    while((*pExtension)&&(wPtr<wPtrmax))
        {
        *wPtr=(utf8_t)*pExtension;
        pExtension++;
        wPtr++;
        }
    *wPtr=0;
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
uriString::exists(void)
{
struct stat wStatBuffer ;
    int wSt= stat(toCChar(),&wStatBuffer);
    if (wSt<0)
            {
            return(false);
            }
    return(true);
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
uriString::getStatR(uriStat &pZStat)
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
                                   content);
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
uriString::getFileSize(void)
{
 uriStat wStat;
 if (getStatR(wStat)!=ZS_SUCCESS)
                 {
                if (ZVerbose)
                        {
                         fprintf(stderr,"%s-E-ERRSTAT error on getStatR file %s \n",
                                 _GET_FUNCTION_NAME_,
                                 content);
                         ZException.printLastUserMessage(stderr);
                         }
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
uriString::loadContent (ZDataBuffer& pDBS)
{
ZStatus wSt;
uriStat wStat;
    if (!exists())
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_NOTFOUND,
                                  Severity_Error,
                                  "-E-FILENOTFOUND Requested file %s does not exist \n",
                                  toString());
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


    FILE *wFile =fopen(toCString_Strait(),"rb");

    if (wFile==nullptr)
            {
            ZException.getErrno(errno);
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_ERROPEN,
                                  Severity_Error,
                                  "-E-ERROPEN error on fopen file for read  <%s> \n",
                                  content);
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
                                              content);
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
uriString::loadUtf8(utf8VaryingString &pUtf8)
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
uriString::loadUtf16(utf16VaryingString &pUtf16)
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
uriString::loadUtf32(utf32VaryingString &pUtf32)
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
                                  content);
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
                                      content);
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
                                  content);
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

#endif // URISTRING_CPP
