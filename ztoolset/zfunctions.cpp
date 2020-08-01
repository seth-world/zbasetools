#ifndef ZFUNCTIONS_CPP
#define ZFUNCTIONS_CPP

/*  Base utilities or system functions
 *  all these functions' prototypes are located in ztoolset_common.h
 *
 */

#include <ztoolset/zfunctions.h>
#include <ztoolset/zerror.h>
//#include <ztoolset/zutfstrings.h>
//#include <ztoolset/zexceptionmin.h>

char *dumpSequence (void *pPtr,size_t pSize,char *pBuffer)
 {
    char *wPtr = (char *)pPtr ;
    for (size_t wi=0; wi < pSize;wi ++)
                {
                if ((wPtr[wi]>31)&&(wPtr[wi]<127))
                        {
                        pBuffer[wi]=wPtr[wi];
                        continue;
                        }
                if (((wPtr[wi]>6)&&(wPtr[wi]<14))||(wPtr[wi]==27))
                {
                    pBuffer[wi] = '@' ;
                    continue;
                }
                    pBuffer[wi] = '.' ;
                }// for
    pBuffer[pSize]='\0';
    return(pBuffer);
} // dumpSequence

char *dumpSequenceHexa (void *pPtr,long pSize,char* pBuffer,char* pBuffer1)
 {
 size_t wj=0;
 unsigned char wHexa ;
 unsigned char* wPtr =(unsigned char*) pPtr ;

    for (size_t wi=0; wi < pSize;wi ++)
                {
                wHexa = wPtr[wi];
                _Zsprintf(&pBuffer1[wj],"%02X",wHexa);
                if ((wPtr[wi]>31)&&(wPtr[wi]<127))
                        {
                        pBuffer[wj]=' ';
                        pBuffer[wj+1]=wPtr[wi];
                        wj+=2;
                        continue;
                        }
                if (((wPtr[wi]>6)&&(wPtr[wi]<14))||(wPtr[wi]==27))
                {
                    pBuffer[wj]=' ';
                    pBuffer[wj+1] = '@' ;
                    wj+=2;
                    continue;
                }
                pBuffer[wj]=' ';
                pBuffer[wj+1] = '.' ;
                wj+=2;
                }// for
    pBuffer[pSize*2]='\0';
    pBuffer1[pSize*2]='\0';
    return(pBuffer);
} // dumpSequenceHexa

const char * decode_ZSA_Action (ZSA_Action &ZS)
    {
    switch (ZS)
            {
            case ZSA_NoAction :
                    {
                        return ("ZSA_NoAction");
                    }
            case ZSA_Error :
                    {
                        return ("ZSA_Push");
                    }
            case  ZSA_Push_Front  :
                    {
                        return ("ZSA_Push_Front");
                    }
            case  ZSA_Push  :
                    {
                        return ("ZSA_Push");
                    }
            case ZSA_Insert :
                    {
                        return ("ZSA_Insert");
                    }
            case  ZSA_Remove :
                    {
                        return ("ZSA_Remove");
                    }

            default :
                    {
                        return ("ZSA_UNKNOWN");
                    }

            }// switch
}


char wBuffer [100];
const char*
decode_ZLockMask(zlock_type pLock)
{
    if (pLock==ZLock_Nolock)
                return "ZLock_Nolock";
    if (pLock==ZLock_All)
                return "ZLock_All";

    memset (wBuffer,0,100);
    if (pLock&ZLock_Exclusive)
            {
            strcat(wBuffer,"ZLock_Exclusive ");
            }
    if (pLock&ZLock_Delete)
            {
            strcat(wBuffer,"ZLock_Delete ");
            }
    if (pLock&ZLock_Read)
            {
            strcat(wBuffer,"ZLock_Read ");
            }
    if (pLock&ZLock_Modify)
            {
            strcat(wBuffer,"ZLock_Modify ");
            }
    if (pLock&ZLock_Read)
            {
            strcat(wBuffer,"ZLock_Read ");
            }

    return wBuffer;
} //decode_ZLockMask

/**
 * @brief decode_ZStatus gives a constant string that explains the give code from ZStatus enum
 * @param ZS
 * @return              a constant string that names the given code
 */
const char * decode_ZStatus (ZStatus ZS)
    {
    switch (ZS)
            {
        case ZS_SUCCESS :
                {
                    return ("ZS_SUCCESS");
                }
        case ZS_ENDCLIENT :
                {
                    return ("ZS_ENDCLIENT");
                }
        case ZS_CONFWARN :
                {
                    return ("ZS_CONFWARN");
                }

        case ZS_CANCEL :
                {
                    return ("ZS_CANCEL");
                }
        case ZS_ERROR :
                {
                    return ("ZS_ERROR");
                }
        case ZS_NOTFOUND :
                {
                    return ("ZS_NOTFOUND");
                }
        case ZS_OUTBOUNDLOW :
                {
                    return ("ZS_OUTBOUNDLOW");
                }
        case ZS_OUTBOUNDHIGH  :
                {
                    return ("ZS_OUTBOUNDHIGH");
                }
        case ZS_OUTBOUND  :
                {
                    return ("ZS_OUTBOUND");
                }
        case ZS_DUPVIOLATION :
                {
                    return ("ZS_DUPVIOLATION");
                }
        case ZS_DUPLICATEKEY :
                {
                    return ("ZS_DUPLICATEKEY");
                }

        case ZS_KEYFIELDPARTIAL :
                {
                    return ("ZS_KEYFIELDPARTIAL");
                }
        case ZS_KEYFIELDOUTBOUND :
                {
                    return ("ZS_KEYFIELDOUTBOUND");
                }
        case ZS_FIELDMISSING :
                {
                    return ("ZS_FIELDMISSING");
                }
        case ZS_FIELDLOOSEPREC :
                {
                    return ("ZS_FIELDLOOSEPREC");
                }
        case ZS_FIELDCAPAOVFLW :
                {
                    return ("ZS_FIELDCAPAOVFLW");
                }
        case ZS_INVADDRESS :
                {
                    return ("ZS_INVADDRESS");
                }
        case ZS_INVOP :
                {
                    return ("ZS_INVOP");
                }

        case ZS_INVTYPE :
                {
                    return ("ZS_INVTYPE");
                }
        case ZS_INVNAME :
                {
                    return ("ZS_INVNAME");
                }
        case ZS_INVSIZE :
                {
                    return ("ZS_INVSIZE");
                }

        case ZS_INVINDEX :
                {
                    return ("ZS_INVINDEX");
                }
        case ZS_INVVALUE :
                {
                    return ("ZS_INVVALUE");
                }
        case ZS_EXCEPTION :
                {
                    return ("ZS_EXCEPTION");
                }

        case ZS_FOUND :
                {
                    return ("ZS_FOUND");
                }
        case ZS_S_ERASEKEY :
                {
                return ("ZS_S_ERASEKEY");
                }
        case ZS_S_ERASEREVERSE :
                {
                return ("ZS_S_ERASEREVERSE");
                }


        case ZS_USERERROR :
                {
                    return ("ZS_USERERROR");
                }

        case ZS_COMROLLERROR :
                {
                    return ("ZS_COMROLLERROR");
                }

// -------------------------------------------------------File errors

        case ZS_EOF :
                {
                    return ("ZS_EOF");
                }
        case ZS_READERROR :
                {
                    return ("ZS_READERROR");
                }
        case ZS_WRITEERROR :
                {
                    return ("ZS_WRITEERROR");
                }

        case ZS_READTIMEOUT :
                {
                    return ("ZS_READTIMEOUT");
                }
        case ZS_WRITETIMEOUT :
                {
                    return ("ZS_WRITETIMEOUT");
                }
        case ZS_READPARTIAL :
                {
                    return ("ZS_READPARTIAL");
                }
        case ZS_WRITEPARTIAL :
                {
                    return ("ZS_WRITEPARTIAL");
                }

        case ZS_ERROPEN :
                {
                    return ("ZS_ERROPEN");
                }
        case ZS_FILENOTEXIST :
                {
                    return ("ZS_FILENOTEXIST");
                }

        case ZS_EMPTYFILE :
                {
                    return ("ZS_EMPTYFILE");
                }
    case ZS_FILEERROR :
            {
                return ("ZS_FILEERROR");
            }

    case ZS_FILEPOSERR :
            {
                return ("ZS_FILEPOSERR");
            }
    case ZS_INVBLOCKADDR:
            {
                return ("ZS_INVBLOCKADDR");
            }
    case ZS_NOTDIRECTORY :
            {
                return ("ZS_NOTDIRECTORY");
            }
    case ZS_FILENOTOPEN :
            {
                return ("ZS_FILENOTOPEN");
            }

    case ZS_BADFILEHEADER :
            {
                return ("ZS_BADFILEHEADER");
            }
    case ZS_BADFILEDESC :
            {
                return ("ZS_BADFILEDESC");
            }
    case ZS_BADFILEVERSION :
            {
                return ("ZS_BADFILEVERSION");
            }
    case ZS_BADFILERESERVED :
            {
                return ("ZS_BADFILERESERVED");
            }
    case ZS_BADMCB :
            {
                return ("ZS_BADMCB");
            }
    case ZS_BADICB :
            {
                return ("ZS_BADICB");
            }
    case ZS_BADFILETYPE :
            {
                return ("ZS_BADFILETYPE");
            }
    case ZS_FILETYPEWARN :
            {
                return ("ZS_FILETYPEWARN");
            }


    case ZS_MALFORMED :
            {
                return ("ZS_MALFORMED");
            }

    case ZS_CORRUPTED :
            {
                return ("ZS_CORRUPTED");
            }

    case ZS_CRYPTERROR :
            {
                return ("ZS_CRYPTERROR");
            }
    case ZS_BADCHECKSUM :
            {
                return ("ZS_BADCHECKSUM");
            }
    case ZS_BADFILEATTR :
            {
                return ("ZS_BADFILEATTR");
            }

    case ZS_ACCESSRIGHTS :
            {
                return ("ZS_ACCESSRIGHTS"); // problem with accessrights ; either to set access rights on the resource or cannot access due to access rights limitation
            }

    case ZS_LOCKED :
            {
                return ("ZS_LOCKED");  // resource is locked and cannot be accessed
            }

    case ZS_LOCKREAD :
            {
                return ("ZS_LOCKREAD");  // resource is locked and cannot be accessed
            }
    case ZS_LOCKWRITE :
            {
                return ("ZS_LOCKWRITE");  // resource is locked and cannot be accessed
            }
    case ZS_LOCKDELETE :
            {
                return ("ZS_LOCKDELETE");  // resource is locked and cannot be accessed
            }
    case ZS_LOCKCREATE :
            {
                return ("ZS_LOCKCREATE");  // resource is locked while being in creation and cannot be accessed
            }

    case ZS_LOCKWRITEDELETE :
            {
                return ("ZS_LOCKWRITEDELETE");  // resource is locked and cannot be accessed
            }
    case ZS_LOCKALL :
            {
                return ("ZS_LOCKALL");  // resource is locked and cannot be accessed
            }
    case ZS_LOCKBADOWNER :
            {
                return ("ZS_LOCKBADOWNER");  //  Owner requesting lock modification is not the owner of the lock
            }
    case  ZS_LOCKBADID :
            {
                return ("ZS_LOCKBADID");  // Bad lock id : no corresponding lock in lockmanager database
            }
    case  ZS_LOCKPENDING :
            {
                return ("ZS_LOCKPENDING");  // Bad lock id : no corresponding lock in lockmanager database
            }
    case  ZS_LOCKINTERR :
            {
                return ("ZS_LOCKINTERR");  // Bad lock id : no corresponding lock in lockmanager database
            }
    case  ZS_LOCKINVALID :
            {
                return ("ZS_LOCKINVALID");
            }

    case ZS_MODEINVALID :
    {
        return("ZS_MODEINVALID");
    }

//------------Socket & network------------------
            case ZS_SOCKSETUP :
            {
                return("ZS_SOCKSETUP");
            }
            case ZS_SOCKBIND :
            {
                return("ZS_SOCKBIND");
            }
            case ZS_SOCKLISTEN :
            {
                return("ZS_SOCKLISTEN");
            }
            case ZS_SOCKACCEPT :
            {
                return("ZS_SOCKACCEPT");
            }
            case ZS_SOCKSELECT :
            {
                return("ZS_SOCKSELECT");
            }

            case ZS_AUTHREJECTED :
            {
                return("ZS_AUTHREJECTED");
            }
            case ZS_SOCKADDRESS :
            {
                return("ZS_SOCKADDRESS");
            }
            case ZS_SOCKCONNECT :
            {
                return("ZS_SOCKCONNECT");
            }
            case ZS_SOCKERROR :
            {
                return("ZS_SOCKERROR");
            }
            case ZS_LDAPFAIL :
            {
                return("ZS_LDAPFAIL");
            }
// -------Users Authorization management
//

    case  ZS_BADUSER :
            {
                return ("ZS_BADUSER");  // user is not either not recognized or not authorized
            }
    case  ZS_BADUSERNAME :
            {
                return ("ZS_BADUSERNAME");  //user name is not recognized
            }
    case  ZS_BADUSERID :
            {
                return ("ZS_BADUSERID");  // user id is not recognized
            }
    case  ZS_BADPASSWORD :
            {
                return ("ZS_BADPASSWORD");  // given password does not match
            }
//----------Journaling Threads and system---------------------
    case  ZS_EMPTY :
                return ("ZS_EMPTY");  // queue is empty
    case  ZS_QUEUEERROR :
                return ("ZS_QUEUEERROR");  // Error on queue while queueing / dequeuing element
    case  ZS_SYSTEMERROR :
                return ("ZS_SYSTEMERROR");  //  base system routine error (example getpwd, etc..), or thread creation, mutex, etc.
    case  ZS_NULLPTR :
                return ("ZS_NULLPTR");  //< encountered a null pointer (NULL or nullptr) while expecting argument

//-----------------------Char encoding-----------------------------
    case  ZS_ICONVINIT :
                return ("ZS_ICONVINIT");  //< iconv init failure
    case  ZS_ICONVSTOPPED :
                return ("ZS_ICONVSTOPPED"); //< iconv processing failure
    case  ZS_NEEDMORESPACE :
                return ("ZS_NEEDMORESPACE");  //< iconv : output buffer exhausted : need more space in output
    case  ZS_INVCHARSET :
                return ("ZS_INVCHARSET");  //< invalid charset : not recognized or not authorized for target string

//----------------------MIME data-----------------

    case ZS_MIMEINV :
                return ("ZS_MIMEINV");
    case ZS_ENTITYERROR :
                return ("ZS_ENTITYERROR");

    case ZS_MEMERROR :
                return ("ZS_MEMERROR");
    case ZS_MEMOVFLW :
                return ("ZS_MEMOVFLW");

    case ZS_XMLERROR :
                return ("ZS_XMLERROR");
    case ZS_XMLWARNING :
                return ("ZS_XMLWARNING");
    default :
                return ("Unknownn ZStatus");

            }// switch
}//decode_ZStatus

/**
 * @brief DecodeZOperation   gives a const char string of max 25 chars mentioning the ZOperation given in parameter
 * @param pOp
 * @return              a constant string that names the given code
 */
const char* decode_ZOperation(ZOp &pOp)
{
    switch (pOp)
    {
    case    ZO_Nothing :
        {
        return ("ZO_Nothing");
        }
case     ZO_Add  :
{
return ("ZO_Add");
}
case    ZO_Erase :
{
return ("ZO_Erase");
}
case    ZO_Replace :
{
return ("ZO_Replace");
}
case    ZO_Insert  :
{
return ("ZO_Insert");
}
case    ZO_Swap :
{
return ("ZO_Swap");
}
case    ZO_Push :
{
return ("ZO_Push");
}
case    ZO_Push_front :
{
return ("ZO_Push_front");
}
case    ZO_Pop :
{
return ("ZO_Pop");
}
    case    ZO_Pop_front :
    {
    return ("ZO_Pop_front");
    }
    case    ZO_Reset :
    {
    return ("ZO_Reset");
    }
    case    ZO_Master :
    {
    return ("ZO_Master");
    }
    case    ZO_Slave :
    {
    return ("ZO_Slave");
    }
    case    ZO_join :
    {
    return ("ZO_join");
    }
    case    ZO_joinWithDefault :
    {
    return ("ZO_joinWithDefault");
    }
    case    ZO_joinNotFound:
    {
    return ("ZO_joinNotFound");
    }
    case    ZO_Unique :
    {
    return ("ZO_Unique");
    }
    case    ZO_joinUnique :
    {
    return ("ZO_joinUnique");
    }
    case    ZO_joinUniqueWithDefault :
    {
    return ("ZO_joinUniqueWithDefault");
    }
    case    ZO_joinUniqueNotFound :
    {
    return ("ZO_joinUniqueNotFound");
    }

//!----------------ZRF Add-ons----------------
//!
//!
    case    ZO_Free :
    {
    return ("ZO_Free");
    }
    case    ZO_Writeblock :
    {
    return ("ZO_Writeblock");
    }
    case    ZO_Remove :
    {
    return ("ZO_Remove");
    }

//!----------------End ZRF Add-ons----------------

default:
    {
    return ("Unknown Zoperation");
    }
    } // switch
}//decode_ZOperation
/**
 * @brief decodeZAMState   Maximum 15 char
 * @param pZAM
 * @return              a constant string that names the given code
 */
const char * decode_ZAMState (ZAMState_type pZAM)
{
    switch (pZAM)
            {
            case    ZAMNothing :
                {
                return ("ZAMNothing");
                }
    case     ZAMInserted  :
        {
        return (" ZAMInserted");
        }
    case    ZAMMarkedDelete :
        {
        return ("ZAMMarkedDelete");
        }
    case    ZAMReplaced :
        {
        return ("ZAMReplace");
        }
    case    ZAMErrInsert :
        {
        return ("ZAMErrInsert");
        }
    case    ZAMErrDelete :
        {
        return ("ZZAMErrDelete");
        }
    case    ZAMErrReplace :
        {
        return ("ZAMErrReplace");
        }
    case    ZAMbeginmark :
        {
        return ("ZAMbeginmark");
        }
    case    ZAMendmark :
        {
        return ("ZAMendmark");
        }
    case    ZAMRolledBack :
        {
        return ("ZAMRolledBack");
        }
    case    ZAMRBckErased :
        {
        return ("ZAMRBckErased");
        }
    case    ZAMRBckInserted :
        {
        return ("ZAMRBckInserted");
        }
    case    ZAMRBckReplaced :
        {
        return ("ZAMRBckReplaced");
        }
    case    ZAMErrored :
        {
        return ("ZAMErrored");
        }
    default:
            {
            return ("Unkown ZAMState");
            }

    }//switch
} //DecodeZAMState


const char * decode_ShortSeverity (Severity_type pSeverity)
{
    switch (pSeverity)
            {
            case (Severity_Nothing) :
                {
                return ("---");
                }
            case (Severity_Information) :
                {
                return ("-I-");
                }
    case (Severity_Warning) :
        {
        return ("-W-");
        }
    case (Severity_Error) :
        {
        return ("-E-");
        }
    case (Severity_Severe) :
        {
        return ("-S-");
        }
    case (Severity_Fatal) :
        {
        return ("-F-");
        }
    default :
        {
        return ("***");
        }
    } // switch
} // DecodeSeverity

/**
 * @brief decode_Severity decode Severity_type enum
 * @param pSeverity     the code to decode
 * @return              a constant string that names the given code
 */
const char * decode_Severity (Severity_type pSeverity)
{
    switch (pSeverity)
            {
            case (Severity_Nothing) :
                {
                return ("---");
                }
            case (Severity_Information) :
                {
                return ("Severity_Information");
                }
    case (Severity_Warning) :
        {
        return ("Severity_Warning");
        }
    case (Severity_Error) :
        {
        return ("Severity_Error");
        }
    case (Severity_Severe) :
        {
        return ("Severity_Severe");
        }
    case (Severity_Fatal) :
        {
        return ("Severity_Fatal");
        }
    default :
        {
        return ("Unknown Severity");
        }
    } // switch
} // DecodeShortSeverity


void zprintMessage (Severity_type pSeverity,
                    const char * pModule,
                    const char * pShortMessage,
                    char * pFile,
                    int pLine ,
                    char *pMsg)
    {
        fprintf (stdout,"%s%s%s %s\n",
                 pModule,
                 decode_ShortSeverity(pSeverity),
                 pShortMessage,
                 pMsg);
        fprintf (stdout, "File %s Line %d \n",pFile,pLine);

    }

void zprintSystemMessage (Severity_type pSeverity,
                          int perrno,
                    const char * pModule,
                    const char * pShortMessage,
                    char * pFile,
                    int pLine ,
                    char *pMsg)
    {

        fprintf (stdout,"%s%s%s %s\n%s\n",
                 pModule,
                 decode_ShortSeverity(pSeverity),
                 pShortMessage,
                 strerror(perrno),
                 pMsg);
        fprintf (stdout, "File %s Line %d \n",pFile,pLine);

    }


template <typename _ReturnTp>
_ReturnTp HCode(char *pStr)
{
    _ReturnTp wI ;

    char wC[sizeof(_ReturnTp)];
    memset (wC,0,sizeof(_ReturnTp));

    size_t wL = strlen (pStr);
    long wM = 0;
    for (size_t wi=0;wi<wL;wi++)
                {
                wC[wM]= (wC[wM] + pStr[wi]) % 0xFF ;
                wM++;
                if (wM==sizeof(_ReturnTp))
                            wM=0;
                }
     memcpy (&wI,wC,sizeof(_ReturnTp));
     return (wI);
}



double timeval_dif (timeval &pbeg,timeval &pend)
{
    return (( pend.tv_usec-pbeg.tv_usec ) + ((pend.tv_sec-pbeg.tv_sec)*1000000));

}


char *_printStdStringField(void *pField,bool IsPointer,char *pBuf)
{
    std::string **wField1=nullptr;
    std::string *wField=nullptr;
    if (IsPointer)
        {
        wField1 = static_cast <std::string **>(pField);
        wField=*wField1;
        }
        else
        wField = static_cast <std::string *>(pField);
    sprintf(pBuf,"%s",wField->c_str());
    return(pBuf);
}
namespace zbs
{
char *_printStdStringField(void *pField, bool IsPointer, char *pBuf)
{
    return ::_printStdStringField(pField,IsPointer,pBuf);
}
}


/**
 * @brief _Zsprintf same as sprintf but returns a const char pointer immediately usable as string
 * @param pBuf
 * @param pFormat
 * @return
 */
const char* _Zsprintf(char *pBuf,const char* pFormat,...)
{
    va_list args;
    va_start (args, pFormat);
    vsprintf(pBuf,pFormat,args);
    va_end(args);
    return(pBuf);
}

#ifdef __USE_WINDOWS__

#include <direct.h>  // chdir (linux chdir is contained in unistd.h)

#endif

int
set_default_Directory (const char *pDefaultdirectory)
{
    if (pDefaultdirectory==nullptr)
                        return (0);

size_t si = strlen (pDefaultdirectory);
char Dir[FILENAME_MAX];
    strcpy (Dir,pDefaultdirectory);

    if (Dir[si-1]==Delimiter)
                                        Dir[si-1]='\0';
   return(chdir(Dir));
} // set_default

/**=======================================================================================================
  * Sub-Process section :
  *
  *    NB: ZLaunch is deprecated
  *
  * @Note: To be portable to windows, creating a sub-process is only possible for launching a program image :
  * windows does NOT authorize to execute a segment of code of the father process as it is possible with fork().
  *
  * @brief ZSpawn_Program  creates a sub process (child) and launch a program image until completion (good or not).
  * returns an int containing
  *     . -1 if something went wrong during the sub process creation
  *     . the value returned by executed program if the process went normally
  *
  *     Program file name to execute is contained in pProgram
  *
 * @param pProgram  image path either full path or only image name  (searched within %PATH% env context)
 * @param argv      arguments to launch the image with. LAST ARGUMENT MUST BE nullptr
 * @return          a ZSpawn_Return structure
 *  - Status : main process status
 *          __ZSPAWN_SUCCESS__      0
 *          __ZSPAWN_ERROR__        -1
 *          __ZSPAWN_TIMEOUT__      -2
 *
 *  - ChildStatus : the status returned by the sub process. set to -1 in case of error or timeout
 *
 *
 *
  *
  */




#ifdef __USE_WINDOWS__


ZSpawn_Return
ZSpawn_Program(const char *pProgram,char * const argv[],const char *pWorkingDirectory, const int pMilliSecondsTimeout)
{

STARTUPINFO si;
PROCESS_INFORMATION pi;
//int wWaitTimes = 10;          // main process will wait 10 times before timing out
//DWORD wMilliSecondsTimeout = 2000;   // default timeout is set to 2 seconds
DWORD   wRet;
int     wErr,wi;
char    wErrMsg[__MESSAGE_SIZEMAX__];
char * wArgv[__ZSPAWN_MAX_ARG__ + 1];
struct ZSpawn_Return wReturn;
CEscapedString ArgumentEscaped;
char wProgImage [FILENAME_MAX];

char wCommandString [__MESSAGE_SIZEMAX__];

    wCommandString [0] = '\0';

    wi = 0;
    while ((argv[wi]!=nullptr)&&(wi<__ZSPAWN_MAX_ARG__))
            {
            strcat(wCommandString, ArgumentEscaped.escapeReserved((char *)argv[wi]));
            strcat (wCommandString," ");
            wi ++;
            }
    strcat (wCommandString,"\r\n");

    memset( &si,0, sizeof(si) );
    si.cb = sizeof(si);
    memset( &pi,0, sizeof(pi) );

    // Start the child process.
    if( !CreateProcess( (LPCWSTR)pProgram,   // Module name (executable program file)
                        (LPWSTR)wCommandString,        // Command line
                        NULL,           // Process handle not inheritable
                        NULL,           // Thread handle not inheritable
                        FALSE,          // Set handle inheritance to FALSE
                        0,              // No creation flags
                        NULL,           // Use parent's environment block
                        (LPCWSTR)pWorkingDirectory,           // Child process working directory
                        &si,            // Pointer to STARTUPINFO structure
                        &pi )           // Pointer to PROCESS_INFORMATION structure
                        )
        {
        _getLastWindowsError(wErr,wErrMsg,__MESSAGE_SIZEMAX__);
        fprintf(stderr, "%s-E-PROCCREAT CreateProcess failed <%d> :%s\n",_GET_FUNCTION_NAME_,wErr,wErrMsg  );
        wReturn.Status= __ZSPAWN_ERROR__;
        wReturn.ChildStatus = -1;
        }
    wRet=WAIT_TIMEOUT;
    wRet=WaitForSingleObject( pi.hProcess, (DWORD)pMilliSecondsTimeout );// Wait for completion or timeout

    // Close process and thread handles.

    if (wRet==WAIT_TIMEOUT){
                    wReturn.Status=__ZSPAWN_TIMEOUT__;
                    wReturn.ChildStatus = -1;
                    goto ZSpawn_Program_exit ;
                    }

    GetExitCodeProcess(pi.hProcess,(LPDWORD)&wReturn.ChildStatus);
    wReturn.Status= __ZSPAWN_SUCCESS__;

ZSpawn_Program_exit:
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    return wReturn;
}// ZLaunch_Program




#else
/**
 * @brief ZSpawn_Program  creates a child process and run the program pProgram
 *
 * uses unistd exec function to fork and execute an image with argv[] using path
 *          Argv [0] MUST NOT mention image name. Argv[0] is added by ZLaunch
 * @param pProgram image path either full path or only image name  (searched within %PATH% env context)
 * @param argv arguments to launch the image with. LAST ARGUMENT MUST BE nullptr
 * @param[out] pPipeFd an array of 2 int for the created pipe (read,write). If nullptr (omitted), no pipe is returned
 * @return 0 if successfull, -1 if failed
 */

ZSpawn_Return ZSpawn_Program(const char *pProgram,char * const argv[],const char *pWorkingDirectory, const int pMilliSecondsTimeout)
{
pid_t wPid = 0;
int result = 0, filedes[2], wRet, status, wi;
char wProgImage[FILENAME_MAX];
char * wArgv[10];
CEscapedString *ArgEscaped[10];
struct ZSpawn_Return wReturn = {__ZSPAWN_ERROR__,-1};

    strcpy (wProgImage,pProgram);
    wi = 0;
    while (argv[wi]!=nullptr)
            {
            strcat (wProgImage, " ");
            strcat (wProgImage,"\'");
            strcat (wProgImage, argv[wi] );
            strcat (wProgImage,"\' \r\n");
            wi ++;
            }

    wArgv[0]=(char *)pProgram;
    wi = 0;
    while (argv[wi]!=nullptr)
            {
            wArgv[wi+1]=argv[wi];
            wi ++;
            }

    ArgEscaped[0]=new CEscapedString ((char *)pProgram);
    wArgv[0]= ArgEscaped[0]->Content;
    wi = 0;
    while (argv[wi]!=nullptr)
            {
            ArgEscaped[wi+1]=new CEscapedString ((char *)argv[wi]);
            wArgv[wi+1]= ArgEscaped[wi+1]->Content;
            wi ++;
            }

    /* Create child process: */
    wPid = fork();
//-----------------------from here following code is executed by both parent and child process---------

    if (wPid < 0 ) // we are in the parent process and fork operation went wrong
    {
        fprintf (stderr,"%s>>error creating fork process : %s\n",_GET_FUNCTION_NAME_,strerror(errno));
        wReturn.Status=__ZSPAWN_ERROR__;
        return wReturn;
    }

    if (wPid != 0) // were are in the parent process (wPid not zero)
    {
        fprintf (stdout,"%s>> Parent process : Created child process. PID: <%d>\n",_GET_FUNCTION_NAME_,wPid);
    }
    result = pipe(filedes); // create the pipe to communicate within both parent and child process
    if (result == -1)
        {
        fprintf(stderr,"%s-E-PIPEFAILED>> Failed to create pipe %s\n",_GET_FUNCTION_NAME_,strerror(errno));
        wReturn.Status=__ZSPAWN_ERROR__;
        return wReturn;
        }
//------------------- wPid == 0 : we are in the child process--------------------------------------
    if (wPid == 0)      // were are in the child process (wPid is zero)
    {
//        int wSt=system (ProgImage);
//        std::cerr << "Sub Process returned " << WEXITSTATUS(wSt) << ".\n";

        strcpy (wProgImage,"/usr/bin/");
        strcat (wProgImage,pProgram);
        if (pWorkingDirectory!= nullptr)
        {
        wRet=set_default_Directory(pWorkingDirectory);
        if (!wRet)
                    {
                    fprintf(stdout,"%s-I-DIRSET>> Working directory set to %s\n",_GET_FUNCTION_NAME_,pWorkingDirectory);
                    }
                else
                    {
                    fprintf(stderr,"%s-E-DIRFAILED>> Failed to set Working directory to %s\n",_GET_FUNCTION_NAME_,pWorkingDirectory);
                    }
        }
        fprintf(stdout,"%s-I-PROGPATH>> child process : program image file %s\n",_GET_FUNCTION_NAME_,wProgImage);

        for (int wi=0;ArgEscaped[wi]!=nullptr;wi ++)
                {
                fprintf (stdout,"                 argv[%d] : <%s>\n", wi, ArgEscaped[wi]->Content);
                }

        wRet=execvp(ArgEscaped[0]->Content,wArgv) ; // Execute the program with given arguments
        if (wRet)
                {
                fprintf(stderr,"%s-E-EXECERR>> Child process-->Error executing program image : %s\n",_GET_FUNCTION_NAME_,strerror(errno));
                exit(wRet); // child process exits with failure
                }
//        fprintf(stderr,"%s-E-EXIT>> Child Process returns : %s\n",_GET_FUNCTION_NAME_,WEXITSTATUS(wRet) );
        exit(__ZSPAWN_SUCCESS__);
    }// wPid == 0
//------------------- wPid > 0 : we are in the parent process--------------------------------------

// here will wait to child process completion millisecond per millisecond until chil process completion or timeout exhausted

int timeout =pMilliSecondsTimeout;
    while ((wRet=waitpid(wPid, &status, WNOHANG))==0) // Wait for the child process to return.
    {
        if (--timeout<0)
            {
                fprintf(stderr,"%s-E-TIMEOUT>> Timeout <%d> milliseconds exhausted on child Process exhausted \n",
                        _GET_FUNCTION_NAME_ ,
                        pMilliSecondsTimeout);
                wReturn.Status=__ZSPAWN_TIMEOUT__;
                return wReturn;
            }
    usleep(1000);       // sleep for next millisecond
    }// while
    if (wRet<0)
        {
      fprintf(stderr,"%s-E-ERRWAIT>> Error waiting child process : %s  \n",_GET_FUNCTION_NAME_,strerror(errno) );
      wReturn.Status=__ZSPAWN_TIMEOUT__;
      return wReturn;
        }
    if (ZVerbose)
        fprintf(stdout,"%s-E-EXIT>> Child Process returned status: <%d>\n",_GET_FUNCTION_NAME_,WEXITSTATUS(status) );
    wReturn.ChildStatus = WEXITSTATUS(status);
    if (WIFEXITED(status))
        {
        wReturn.Status = __ZSPAWN_SUCCESS__;
        return wReturn;
        }
    wReturn.Status = __ZSPAWN_ERROR__;
    return wReturn;
}// ZLaunch_Program



/**
 * @brief ZLaunch uses unistd exec function to fork and execute an image with argv[] using path
 *          Argv [0] MUST NOT mention image name. Argv[0] is added by ZLaunch
 * @param pPath image path either full path or only image name  (searched within %PATH% env context)
 * @param argv arguments to launch the image with. LAST ARGUMENT MUST BE nullptr
 * @param[out] pPipeFd an array of 2 int for the created pipe (read,write). If nullptr (omitted), no pipe is returned
 * @return 0 if successfull, -1 if failed
 */

int ZLaunch(const char *pPath,char * const argv[],const char *pDirectory)
{
    pid_t PID = 0;
    int result = 0, filedes[2], status, wi;
    char ProgImage[255];
    char * wArgv[10];
    CEscapedString *ArgEscaped[10];

            strcpy (ProgImage,pPath);
            wi = 0;
            while (argv[wi]!=nullptr)
                    {
                    strcat (ProgImage, " ");
                    strcat (ProgImage,"\'");
                    strcat (ProgImage, argv[wi] );
                    strcat (ProgImage,"\' \r\n");
                    wi ++;
                    }

            wArgv[0]=(char *)pPath;
            wi = 0;
            while (argv[wi]!=nullptr)
                    {
                    wArgv[wi+1]=argv[wi];
                    wi ++;
                    }

    ArgEscaped[0]=new CEscapedString ((char *)pPath);
    wArgv[0]= ArgEscaped[0]->Content;
    wi = 0;
    while (argv[wi]!=nullptr)
            {
            ArgEscaped[wi+1]=new CEscapedString ((char *)argv[wi]);
            wArgv[wi+1]= ArgEscaped[wi+1]->Content;
            wi ++;
            }

    /* Create child process: */
    PID = fork();

    if (PID == -1) {
        fprintf (stderr,"%s>>error creating fork process : %s\n",_GET_FUNCTION_NAME_,strerror(errno));
        return -1;
    }

    result = pipe(filedes);

    if (PID != 0) {
        fprintf (stdout,"%s>> Created child process. PID: %d\n",_GET_FUNCTION_NAME_,PID);
    }

    if (result == -1) {
        return 1;
    }

    if (PID == 0) { // This is the child process
//        int wSt=system (ProgImage);
//        std::cerr << "Sub Process returned " << WEXITSTATUS(wSt) << ".\n";

        strcpy (ProgImage,"/usr/bin/");
        strcat (ProgImage,pPath);
        if (pDirectory!= nullptr)
        {
            int wSt=set_default_Directory(pDirectory);
        if (!wSt)
                    {
                    fprintf(stdout,"%s-I-DIRSET>> Working directory set to %s\n",_GET_FUNCTION_NAME_,pDirectory);
                    }
                else
                    {
                    fprintf(stderr,"%s-E-DIRFAILED>> Failed to set Working directory to %s\n",_GET_FUNCTION_NAME_,pDirectory);
                    }
        }
        fprintf(stdout,"%s-I-PROG>> child process : program image file %s\n",_GET_FUNCTION_NAME_,ProgImage);

        for (int wi=0;ArgEscaped[wi]!=nullptr;wi ++)
                {

                fprintf (stderr,"                 argv[%d] : <%s>\n", wi, ArgEscaped[wi]->toChar());
                }

        int wSt=execvp(ArgEscaped[0]->Content,wArgv) ; // Execute the program
        if (wSt)
                {
                perror ("execvp error : ");
                std::cerr << " in function " << _GET_FUNCTION_NAME_ << "\n";
                exit(EXIT_FAILURE);
                }
         std::cerr << "Sub Process returned " << WEXITSTATUS(wSt) << ".\n";
         exit(wSt);
    } else { // This is the parent process

       int timeout =10;
        while (waitpid(PID, &status, WNOHANG)==0) // Wait for the child process to return.
        {
            if (--timeout<0)
                    {
                    std::cerr << " Timeout on sub process\n";
                    return 0;
                }
        sleep(1);
        }
        std::cerr << "Process returned " << WEXITSTATUS(status) << ".\n";

    }//else


    return 0;
}// ZLaunch


/**
 * @brief ZSpawnEscaped( uses system() function to fork and execute an image with argv[] using path
 *          Argv [0] MUST NOT mention image name. Argv[0] is added by ZLaunch
 * @param pPath image path either full path or only image name  (searched within %PATH% env context)
 * @param argv arguments to launch the image with. LAST ARGUMENT MUST BE nullpt
 * @return
 */

int ZSpawnEscaped(const char *pPath,char * const argv[],const char *pDirectory)
{
    pid_t PID = 0;
    int result = 0, filedes[2], status, wi;
    char ProgImage[255];
    CEscapedString *ArgEscaped[10];

    ArgEscaped[0]=new CEscapedString ((char *)pPath);

    if (argv!=nullptr)
            {
    wi = 0;
    while (argv[wi]!=nullptr)
            {
            ArgEscaped[wi+1]=new CEscapedString ((char *)argv[wi]);
            wi ++;
            }
    ArgEscaped[wi+1]=nullptr;
    strcpy (ProgImage,pPath);
    wi = 0;
    while (argv[wi]!=nullptr)
            {
            strcat (ProgImage, " ");
            strcat (ProgImage," \"");
            strcat (ProgImage, ArgEscaped[wi+1]->Content );
            strcat (ProgImage,"\"  ");
            wi ++;
            }
            }// argv !=nullptr

    strcat (ProgImage, "\r\n");


    /* Create child process: */
    PID = fork();

    if (PID == -1) {
        std::cerr << "error creating fork process \n";
        return -1;
    }

    result = pipe(filedes);

    if (PID != 0) {
        std::cerr << "Created child process. PID: " << PID << "\n";
    }

    if (result == -1) {
        return 1;
    }

    if (PID == 0) { // This is the child process

        if (pDirectory!= nullptr)
        {
            int wSt=set_default_Directory(pDirectory);
        if (!wSt)
                    {
                    fprintf(stderr,"%s-I-DIRSET Working directory set to %s\n",_GET_FUNCTION_NAME_,pDirectory);
                    }
                else
                    {
                    fprintf(stderr,"%s-E-DIRFAILED Failed to set Working directory to %s\n",_GET_FUNCTION_NAME_,pDirectory);
                    }
        }
        std::cerr << _GET_FUNCTION_NAME_ << "\n";
        std::cerr << " child process : program image file <"<<ProgImage << ">\n";

        for (int wi=0;ArgEscaped[wi]!=nullptr;wi ++)
                {
                fprintf (stderr,"                 argv[%d] : <%s>\n", wi, ArgEscaped[wi]->Content);
                }
        int wSt=system (ProgImage);
        if (wSt)
                {
                perror ("system error : ");
                std::cerr << " in function " << _GET_FUNCTION_NAME_ << "\n";
                exit(EXIT_FAILURE);
                }
        std::cerr << "Sub Process returned " << WEXITSTATUS(wSt) << ".\n";

        exit(EXIT_SUCCESS);

 //       int wSt=execvp(pPath,argv) ; // Execute the program
 //       if (wSt)
 //               perror ("execvp");
 //        std::cerr << "Sub Process returned " << WEXITSTATUS(wSt) << ".\n";
 //
    } else { // This is the parent process
        int timeout =10;
        while (waitpid(PID, &status, WNOHANG)==0) // Wait for the child process to return.
        {
            if (--timeout<0)
                    {
                    std::cerr << " Timeout on sub process\n";
                    return 0;
                    }
            sleep(1);
        }
        std::cerr << "Process returned " << WEXITSTATUS(status) << ".\n";

    }//else

    return 0;
}
/**
 * @brief ZSpawn( uses system() function to fork and execute an image with argv[] using path
 *          Argv [0] MUST NOT mention image name. Argv[0] is added by ZLaunch
 * @param pPath image path either full path or only image name  (searched within %PATH% env context)
 * @param argv arguments to launch the image with. LAST ARGUMENT MUST BE nullpt
 * @return
 */

int ZSpawn(const char *pPath,char * const argv[],const char *pDirectory)
{
    pid_t PID = 0;
    int result = 0, filedes[2], status, wi;
    char ProgImage[255];
    char *Arg[10];

    Arg[0]=((char *)pPath);
    strcpy (ProgImage,pPath);
    if (argv!=nullptr)
            {
    wi = 0;
    while (argv[wi]!=nullptr)
            {
            Arg[wi+1]=(char *)argv[wi];
            wi ++;


    wi = 0;
    while (argv[wi]!=nullptr)
            {
            strcat (ProgImage, " ");
            strcat (ProgImage," \"");
            strcat (ProgImage, Arg[wi+1] );
            strcat (ProgImage,"\"  ");
            wi ++;
            }
            }// argv !=nullptr

 //   strcat (ProgImage, "\r\n");
    } // argv!=nullptr)
Arg[wi+1]=nullptr;

    /* Create child process: */
    PID = fork();

    if (PID < (pid_t) 0) {
        fprintf(stderr,"%s-F-CANTFORK error creating fork process \n",_GET_FUNCTION_NAME_);
        return -1 ;
    }

    result = pipe(filedes);

    if (PID != 0) {
//        std::cerr << "Created child process. PID: " << PID << "\n";
        fprintf(stderr,"%s-I-SUBPROCSPAWNED Spawned fork process pid %d \n",_GET_FUNCTION_NAME_,(int)PID);
    }

    if (result == -1) {
        return 1;
    }

    if (PID == 0) { // This is the child process

        if (pDirectory!= nullptr)
        {
            int wSt=set_default_Directory(pDirectory);
        if (!wSt)
                    {
                    fprintf(stderr,"%s-I-DIRSET Working directory set to %s\n",_GET_FUNCTION_NAME_,pDirectory);
                    }
                else
                    {
                    fprintf(stderr,"%s-E-DIRFAILED Failed to set Working directory to %s\n",_GET_FUNCTION_NAME_,pDirectory);
                    }
        }
#if __DEBUG_LEVEL__ > 1
        fprintf (stderr, "%s-I-CHILD child process : program image file %s\n",
                 _GET_FUNCTION_NAME_,
                 ProgImage);

        for (int wi=0;Arg[wi]!=nullptr;wi ++)
                {
                fprintf (stderr,"                 argv[%d] : <%s>\n", wi, Arg[wi]);
                }
#endif // __DEBUG_LEVEL__ > 1

        int wSt=system (ProgImage);
        if (wSt)
                {
                perror ("system error : ");
                std::cerr << " in function " << _GET_FUNCTION_NAME_ << "\n";
                exit(EXIT_FAILURE);
                }
        std::cerr << "Sub Process returned " << WEXITSTATUS(wSt) << ".\n";

        exit(EXIT_SUCCESS);

 //       int wSt=execvp(pPath,argv) ; // Execute the program
 //       if (wSt)
 //               perror ("execvp");
 //        std::cerr << "Sub Process returned " << WEXITSTATUS(wSt) << ".\n";
 //
            }// if (PID == 0)

    else
    { // This is the parent process
        int timeout =10;
        while (waitpid(PID, &status, WNOHANG)==0) // Wait for the child process to return.
        {
            if (--timeout<0)
                    {
                    std::cerr << " Timeout on sub process\n";
                    return 0;
                    }
            sleep(1);
        }

    }//else

    return 0;
} // ZSpawn
#endif // not __USE_WINDOWS__


/**
 * @brief findLastNonChar
 *          returns a pointer to the last char of a string that is not pChar.
 *          returns pContent pointer if the string is empty or if all char of the string are pChar.
 * @param pContent  C string to search for
 * @param pChar     Character to skip from end
 * @return
 */
char *findLastNotChar(char *pContent, char pChar)
{
    char *ptr=(char *)(pContent +(strlen(pContent)-1));

    while (ptr>pContent)
                {
                if (ptr[0]!=pChar)
                                 break ;
                ptr --;
                }
    return(ptr);
}//findLastNonChar



bool
isTerminatedBy(const char *pContent,char pChar)
{
    return (findLastNotChar((char*)pContent,' ')[0]==pChar);
}


void conditionalNL(char *pString) {if (!isTerminatedBy(pString,'\n'))
                                        strcat(pString,"\n");return;}

//============== wchar_t =================================

wchar_t*
findWLastNonChar(const wchar_t *pContent,wchar_t pChar)
{
    wchar_t* ptr=(wchar_t *)(pContent +(wcslen(pContent)-1));

    while (ptr>pContent)
                {
                if (ptr[0]!=pChar)
                                 break ;
                ptr --;
                }
    return(ptr);
}//finWdLastNonChar



wchar_t *
findWLastNonChar(const wchar_t *pContent,const wchar_t* pChar)
{
    wchar_t* ptr=(wchar_t *)(pContent +(wcslen(pContent)-1));

    while (ptr>pContent)
                {
                if (ptr[0]!=*pChar)
                                 break ;
                ptr --;
                }
    return(ptr);
}//finWdLastNonChar

bool
isWTerminatedBy(wchar_t *pContent,const wchar_t* pChar)
{
    return (findWLastNonChar(pContent,L" ")[0]==*pChar);
}
bool
isWTerminatedBy(const wchar_t *pContent,wchar_t pChar)
{
    return (findWLastNonChar(pContent,L" ")[0]==pChar);
}

void conditionalWNL(wchar_t *pString,const ssize_t pSizeMax)
{
    if (!isWTerminatedBy(pString,L"\n"))
        if (pSizeMax<0)
                    wcscat(pString,L"\n");
                else
                    wcsncat(pString,L"\n",pSizeMax);
    return;
}





const char*
decode_ZST(char pZST)
{
  switch (pZST)
  {
  case ZST_DUPLICATES :
        {
        return "ZST_DUPLICATES";
        }
  case ZST_NODUPLICATES :
        {
        return "ZST_NODUPLICATES";
        }
  case ZST_Nothing :
        {
        return "ZST_Nothing";
        }
  default:
        {
        return "Unknown ZSort_Type";
        }
  }//switch
}

ZSort_Type
encode_ZST(const char* pZST)
{
    if (strcmp(pZST,"ZST_DUPLICATES")==0)
                        return ZST_DUPLICATES;
    if (strcmp(pZST,"ZST_NODUPLICATES")==0)
                        return ZST_NODUPLICATES;

    return ZST_Nothing;
}

//================String and WString functions==========================
/**
 * @brief _firstNotinSet   utility routine
 *          finds the first byte of string pString that is NOT a byte of pSet.
 *          usage example : find the first non space and non tab (\t) char of a string.
 *
 * @param pString
 * @param pSet
 * @return
 */
const char *
_firstNotinSet (const char*pString,const char *pSet)
{
    if (pString==nullptr)
                return nullptr;
    const char*wStr = pString;
    const char*wSet = pSet;

    for (wStr=pString;*wStr!='\0';wStr++)
            {
                for(wSet = pSet;(*wSet!='\0');wSet++)
                                if(*wSet==*wStr)
                                            break ;
                if (*wSet=='\0')
                            return(wStr);
             }
    if (*wStr=='\0')
            return (nullptr);
    return(wStr);
}
/**
 * @brief _firstinSet
 *              returns a pointer to the first character found within pString that belongs to character set pSet.
 *              returns nullptr if not found
 * @param pString
 * @param pSet
 * @return
 */
const char *
_firstinSet (const char*pString,const char *pSet)
{
    const char*wStr = pString;
    const char*wSet = pSet;

    for (wStr=pString;*wStr!='\0';wStr++)
            {
                for(wSet = pSet;(*wSet!='\0');wSet++)
                                if(*wSet==*wStr)
                                            return(wStr) ;
             }
    if (*wStr=='\0')
            return (nullptr);
    return(wStr);
}
/**
 * @brief _lastinSet
 *          finds the last occurrence of one of the characters set given by pSet within pStr string in reverse (starting to strlen() and ending at 0)
 * @param pStr input string
 * @param pSet set of characters to search for within pStr
 * @return pointer to pChar occurrence within string if found, NULL if not found
 */
const char *
_lastinSet(const char *pStr, const char *pSet)
{
size_t wL = strlen(pSet);
size_t wj = 0;
long wi;
    for (wi=strlen(pStr);(wi >= 0)&&(pStr[wi]!=pSet[wj]);wi--)
                            {
                            for (wj=0;(wj<wL)&&(pStr[wi]!=pSet[wj]);wj++);
                            }
    return ((wi<0) ? NULL: &pStr[wi]);     // return NULL pointer if not found pointer to pChar within string if found
}
/**
 * @brief _LTrim
 *          suppresses within pString the leading characters that belong to pSet
 * @param pString
 * @param pSet
 * @return
 */
char *
_LTrim (char*pString, const char *pSet)
{
    size_t wL =strlen(pString);
    char *wPtr=(char*)_firstNotinSet(pString,pSet);
    if (wPtr==nullptr)
            return(pString);
    wL=wL-(pString- wPtr);
    memmove(pString,wPtr,wL);
    pString[wL]='\0';
    return (pString);
}
/**
 * @brief _RTrim
 *          suppresses within pString the trailing characters that belong to pSet
 * @param pString
 * @param pSet
 * @return
 */
char *
_RTrim (char*pString, const char *pSet)
{

size_t wj;
    size_t w1=strlen(pString)-1;

 //   for (w1=strlen(content);(w1 >= 0)&&(content[w1]==' ');w1--); // empty for loop
    for (;(w1 >= 0);w1--)
            {
            for (wj=0;pSet[wj]!='\0';wj++)
                                if (pString[w1]==pSet[wj])
                                            {
                                            pString[w1]='\0';
                                            break;
                                            }
            if (wj==strlen(pSet))
                                break;

            }
    return (pString);

}//   _RTrim

/**
 * @brief _Trim
 *          suppresses leading and trailing characters that belong to pSet.
 *       NB: characters of pString that belong to pSet and are not leading or trailing are not suppressed
 * @param pString
 * @param pSet
 * @return
 */

char *
_Trim (char*pString, const char *pSet)
{
    _LTrim(pString,pSet);
    return(_RTrim(pString,pSet));
}


/**
 * @brief _strchrReverse find the first occurrence of pChar within pStr string in reverse (starting to strlen() and ending at 0)
 * @param pStr
 * @param pChar
 * @return pointer to pChar occurrence within string if found, NULL if not found
 */

const char *
_strchrReverse(const char *pStr,const char pChar)
{
    for (int w1=strlen(pStr);(w1 >= 0)&&(pStr[w1]!=pChar);w1--)
    return ((w1<0) ? NULL:(char*) &pStr[w1]);     // return NULL pointer if not found pointer to pChar within string if found
}


/**
 * @brief _toUpper converts c string pStr to uppercase in pOutStr string if mentionned,
 *      if pOutStr is ommitted, then pStr is used as retuned string (and then in this case must not be a constant string).
 * @param pStr
 * @param pOutStr
 * @return
 */

char * _toUpper(const char *pStr,char *pOutStr)
{
    char *wR = pOutStr;
    if (wR==NULL)
            wR=(char*)pStr;
    for (size_t wi=0;!(wi>strlen(pStr));wi++)
                wR[wi]=(char)toupper((int)pStr[wi]);
    return (wR);
}



/**
 * @brief _expurgeSet
 *          suppresses any occurrence of character or group of characters that belong to pSet from pString, regardless char occurrence order.
 *          returns pString expurged from characters of pSet.
 * @param pString string to parse
 * @param pSet      set of char to remove from pString
 * @return          a pointer to pString after having removed characters from pSet
 */

char *
_expurgeSet(const char *pString, const char *pSet)
{
    bool wJump=false;
    const char* wSet= pSet;
    const char* wPtr= pString;
    size_t wStrlen=0;
    while (*wPtr++)
            wStrlen++;
    wStrlen++;
    char* wOutString=(char*)calloc(wStrlen,sizeof(char));

    wPtr=pString;
    while (*wPtr)
        {
        while (*wSet) // search if string char is in set
            {
            if (*wSet==*wPtr)
                        {
                        wJump=true;
                        break;
                        }
            wSet++;
            }
        if (!wJump)
                *wOutString=*wPtr;
        wJump=false;
        wPtr++;
        }
    *wOutString=0 ;
    /*
    char *wPtr2;
    while((wPtr=(char*)_firstinSet(pString,pSet))!=nullptr)
    {
    wPtr2=(char*)_firstNotinSet(wPtr,pSet);
    strcpy (wPtr,wPtr2);
    wPtr=wPtr2;
    }
    return pString;
    */
} // _expurgeSet

/**
 * @brief _expurgeString
 *          suppresses any occurrence of substring pSubString from pString,
 *          returns pString expurged from encountered substrings of pString.
 * @param pString
 * @param pSet
 * @return
 */
char *
_expurgeString(const char *pInString, const char *pSubString)
{
    const char* wPtr= pInString;
    size_t wStrlen=0;
    while (*wPtr++)
            wStrlen++;  // get strlen
    wStrlen++;  // count '\0' endofstring mark
    char* wOutString=(char*)calloc(wStrlen,sizeof(char)); // allocate room as input string length
    memset (wOutString,0,wStrlen*sizeof(char));

    wPtr=pInString; // reset wPtr to pInString
    char *wPtr2;
    char* wPtrOut=wOutString;

    size_t wSubStrCount=0;
    while (pSubString[wSubStrCount++]); // get strlen of pSubString into wSubStrCount- without '\0'

    while((wPtr2=(char*)strstr(wPtr,pSubString))!=nullptr)
        {
        while (wPtr<wPtr2)
                *wPtrOut++=*wPtr++;  // copy to out string until substring start

        wPtr+=wSubStrCount;  // then jump substring
        }
    *wPtrOut=0;  // add endofstring mark (not necessary because whole string set to 0)

    return wOutString;
}// _expurgeString

//=================wchar_t=========================================

/**
 * @brief _fWirstNotinSet   utility routine
 *          finds the first byte of string pString that is NOT a byte of pSet.
 *          usage example : find the first non space and non tab (\t) char of a string.
 *
 * @param pString
 * @param pSet
 * @return
 */
const wchar_t *
_WfirstNotinSet (const wchar_t*pString,const wchar_t *pSet)
{
    if (pString==nullptr)
                return nullptr;
    const wchar_t*wStr = pString;
    const wchar_t*wSet = pSet;

    for (wStr=pString;*wStr!=L'\0';wStr++)
            {
                for(wSet = pSet;(*wSet!=L'\0');wSet++)
                                if(*wSet==*wStr)
                                            break ;
                if (*wSet==L'\0')
                            return(wStr);
             }
    if (*wStr==L'\0')
            return (nullptr);
    return(wStr);
}
/**
 * @brief _WfirstinSet
 *              returns a pointer to the first character found within pString that belongs to character set pSet.
 *              returns nullptr if not found
 * @param pString
 * @param pSet
 * @return
 */
const wchar_t *
_WfirstinSet (const wchar_t*pString,const wchar_t *pSet)
{
    const wchar_t*wStr = pString;
    const wchar_t*wSet = pSet;

    for (wStr=pString;*wStr!=L'\0';wStr++)
            {
                for(wSet = pSet;(*wSet!=L'\0');wSet++)
                                if(*wSet==*wStr)
                                            return(wStr) ;
             }
    if (*wStr==L'\0')
            return (nullptr);
    return(wStr);
}
/**
 * @brief _WlastinSet
 *          finds the last occurrence of one of the characters set given by pSet within pStr string in reverse (starting to strlen() and ending at 0)
 * @param pStr input string
 * @param pSet set of characters to search for within pStr
 * @return pointer to pChar occurrence within string if found, NULL if not found
 */
const wchar_t *
_WlastinSet(const wchar_t *pStr, const wchar_t *pSet)
{
size_t wL = wcslen(pSet);
size_t wj = 0;
long wi;
    for (wi=wcslen(pStr);(wi >= 0)&&(pStr[wi]!=pSet[wj]);wi--)
                            {
                            for (wj=0;(wj<wL)&&(pStr[wi]!=pSet[wj]);wj++);
                            }
    return ((wi<0) ? nullptr: &pStr[wi]);     // return NULL pointer if not found pointer to pChar within string if found
}
/**
 * @brief _WLTrim
 *          suppresses within pString the leading characters that belong to pSet
 * @param pString
 * @param pSet
 * @return
 */
wchar_t *
_WLTrim (wchar_t*pString, const wchar_t *pSet)
{
    size_t wL =wcslen(pString);
    wchar_t *wPtr=(wchar_t*)_WfirstNotinSet(pString,pSet);
    if (wPtr==nullptr)
            return(pString);
    wL=wL-(pString- wPtr);
    memmove(pString,wPtr,wL*sizeof(wchar_t));
    pString[wL]=L'\0';
    return (pString);
}
/**
 * @brief _WRTrim
 *          suppresses within pString the trailing characters that belong to pSet
 * @param pString
 * @param pSet
 * @return
 */
wchar_t *
_WRTrim (wchar_t*pString, const wchar_t *pSet)
{

size_t wj;
    size_t w1=wcslen(pString)-1;

    for (;(w1 >= 0);w1--)
            {
            for (wj=0;pSet[wj]!=L'\0';wj++)
                                if (pString[w1]==pSet[wj])
                                            {
                                            pString[w1]=L'\0';
                                            break;
                                            }
            if (wj==wcslen(pSet))
                                break;

            }
    return (pString);

}//   _WRTrim

/**
 * @brief _WTrim
 *          suppresses leading and trailing characters that belong to pSet.
 *       NB: characters of pString that belong to pSet and are not leading or trailing are not suppressed
 * @param pString
 * @param pSet
 * @return
 */

wchar_t *
_WTrim (wchar_t*pString, const wchar_t *pSet)
{
    _WLTrim(pString,pSet);
    return(_WRTrim(pString,pSet));
}


/**
 * @brief _strchrReverse find the first occurrence of pChar within pStr string in reverse (starting to strlen() and ending at 0)
 * @param pStr
 * @param pChar
 * @return pointer to pChar occurrence within string if found, NULL if not found
 */

const wchar_t *
_WstrchrReverse(const wchar_t *pStr,const wchar_t pChar)
{
    for (int w1=wcslen(pStr);(w1 >= 0)&&(pStr[w1]!=pChar);w1--)
    return ((w1<0) ? nullptr:(wchar_t*) &pStr[w1]);     // return NULL pointer if not found pointer to pChar within string if found
}


/**
 * @brief _WtoUpper converts c string pStr to uppercase in pOutStr string if mentionned,
 *      if pOutStr is ommitted, then pStr is used as retuned string (and then in this case must not be a constant string).
 * @param pStr
 * @param pOutStr
 * @return
 */

wchar_t * _WtoUpper(wchar_t *pStr,wchar_t *pOutStr)
{
    wchar_t *wR = pOutStr;
    if (wR==NULL)
            wR=pStr;
    for (size_t wi=0;!(wi>wcslen(pStr));wi++)
                wR[wi]=(wchar_t)towupper((wint_t)pStr[wi]);
    return (wR);
}



/**
 * @brief _WexpurgeSet
 *          suppresses any occurrence of character or group of characters that belong to pSet from pString, regardless char occurrence order.
 *          returns pString expurged from characters of pSet.
 * @param pString
 * @param pSet
 * @return
 */
wchar_t *
_WexpurgeSet(wchar_t *pString, const wchar_t *pSet)
{
    wchar_t *wPtr;
    wchar_t *wPtr2;
    while((wPtr=(wchar_t*)_WfirstinSet(pString,pSet))!=nullptr)
    {
    wPtr2=(wchar_t*)_WfirstNotinSet(pString,pSet);
    wcsncpy (wPtr,wPtr2,wcslen(wPtr2));
    }
    return pString;
}

/**
 * @brief _WexpurgeString
 *          suppresses any occurrence of substring pSubString from pString,
 *          returns pString expurged from encountered substrings of pString.
 * @param pString
 * @param pSet
 * @return
 */
wchar_t *
_WexpurgeString(wchar_t *pString, const wchar_t *pSubString)
{
    wchar_t *wPtr;
    wchar_t *wPtr2;
    size_t wSize=wcslen(pSubString);
    while((wPtr=(wchar_t*)wcsstr(pString,pSubString))!=nullptr)
    {
    wPtr2=wPtr+wSize;
    wcsncpy (wPtr,wPtr2,wcslen(wPtr2));
    }
    return pString;
}

//===============end wchar_t=======================================


#endif //ZFUNCTIONS_CPP
