#ifndef ZEXCEPTIONMIN_CPP
#define ZEXCEPTIONMIN_CPP
#include <zconfig.h>

#ifdef __USE_WINDOWS__
#include <windows.h>
#else
#include <pwd.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdarg.h> /* both two includes are for va_list va-start...etc. */

#include <exception>
#include <ztoolset/zexceptionmin.h>

#include <ztoolset/zutfstrings.h>
#include <ztoolset/uristring.h>


#include <ztoolset/zbasedatatypes.h>


//#include <ztoolset/znetexception.h>
//#include <openssl/ssl.h>


//    thread_local ZExceptionMin               ZException;
    ZExceptionMin               ZException;
//    thread_local zbs::ZArray<ZExceptionMin>  ZExceptionStack;
/*
#ifdef __USE_ZRANDOMFILE__
    ZRFExceptionMin                ZException;
    zbs::ZArray<ZExceptionMin>   ZRFExceptionStack;
#endif //__USE_ZRANDOMFILE__
*/





//utfexceptionString& utfexceptionString::setFrommessageString(const utfmessageString &pCode){    return (utfexceptionString&)strset(pCode.content);}

utfexceptionString&
utfexceptionString::setFromURI(const uriString &pUri)
{
    uriString& wUri=const_cast<uriString&>(pUri);
    const _Utf* wContent=wUri.toUtf();
    fromUtf(wContent);
    return *this;
//    return ((utfexceptionString&)*_Base::fromUtf8(pUri.toUtf()));
}

//utfexceptionString& utfexceptionString::setFromcodeString(const  utfcodeString &pCode){    return (utfexceptionString&)strset(pCode.content);}

//utfexceptionString& utfexceptionString::setFromdescString(const  utfdescString &pCode){    return (utfexceptionString&)strset(pCode.content);}

/*
exceptionString& exceptionString::setFromString(char * pString)
    {
    clear();
    size_t wL=(strlen(pString)>cst_desclen)?cst_exceptionstringlen:strlen(pString);
    strncpy(content,pString,wL);
    return(*this);
    }
*/





ZExceptionBase::ZExceptionBase()
{
    ZExceptionBase::clear();
//    _Mtx=new zbs::ZMutex;
}

ZExceptionBase::~ZExceptionBase()
{
 //   if (_Mtx!=nullptr)
 //               delete _Mtx;
 //   _Mtx=nullptr;

}


/**
 * @brief CZExceptionMin::getErrno creates an system error description from errno with all elements.
 * Complement is loaded with the system error description obtained with strerror_t() routine
 *
 *  @note may be used with Threads (use mutex)
 *
 * @note compatible with windows (errno and strerror-get window error)
 *
 * @param pModule   method, function, routine where the exception message has been generated
 * @param pErrno    system errno variable
 * @param pStatus   ZStatus describing the error
 * @param pSeverity Severity of the error
 * @param pFormat   Message text format followed by list of arguments
 */
void
ZExceptionBase::getErrno (const int pErrno,
                     const char *pModule,
                     const ZStatus pStatus,
                     const Severity_type pSeverity,
                     const char* pFormat,
                          va_list arglist)
{
/*#if __USE_ZTHREAD__
        _Mtx.lock();
#endif*/
    if (pErrno<0)
            Error = errno;
        else
            Error=pErrno;
     Module=pModule;
     Status = pStatus;
     Severity=pSeverity;
//     Complement=strerror_r(Error,Complement.content,cst_exceptionlen);  // strerror_r is no more supported by windows
     Complement.fromUtf((const utf8_t*)strerror(Error));

     Message.vsprintf_char(pFormat, arglist);

/*#if __USE_ZTHREAD__
     _Mtx.unlock();
#endif*/
    return;
}//getErrno

void
ZExceptionMin::getErrno (const int pErrno,
                     const char *pModule,
                     const ZStatus pStatus,
                     const Severity_type pSeverity,
                     const char* pFormat,...)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
ZExceptionBase* wExceptionBase = new ZExceptionBase;
     va_list args_1;
     va_start (args_1, pFormat);
     wExceptionBase->getErrno (pErrno,
                          pModule,
                          pStatus,
                          pSeverity,
                          pFormat,args_1);
     va_end(args_1);

     toStack(wExceptionBase); // push on stack
#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    if (pSeverity >= ThrowOnSeverity)
                            zthrow (ZStack.last());
    if (pSeverity >= AbortOnSeverity)
                                exit_abort();
    return;
}//getErrno


/**
 * @brief CZExceptionMin::getErrno creates an system error description from errno with all elements.
 * Complement is loaded with the system error description obtained with strerror_t() routine
 *
 *  @note may be used with Threads (use mutex)
 *
 * @note compatible with windows (errno and strerror-get window error)
 *
 * @param pModule   method, function, routine where the exception message has been generated
 * @param pErrno    system errno variable
 * @param pStatus   ZStatus describing the error
 * @param pSeverity Severity of the error
 * @param pFormat   Message text format followed by list of arguments
 */
void
ZExceptionBase::getIcuError(const char *pModule,
                             const UErrorCode pIcuErr,
//                             const ZStatus pStatus,
                             const Severity_type pSeverity,
                             const char* pFormat,va_list arglist)
{
/*#if __USE_ZTHREAD__
        _Mtx.lock();
#endif*/

     Error = (int)pIcuErr;
     Module=pModule;
     Status = ZS_ICUERROR;
     Severity=pSeverity;
//     Complement=strerror_r(Error,Complement.content,cst_exceptionlen);  // strerror_r is no more supported by windows

     Complement.sprintf(" ICU Error code <%d> <%s>",(int)pIcuErr,u_errorName(pIcuErr));

     Message.vsprintf_char(pFormat, arglist);

/*#if __USE_ZTHREAD__
     _Mtx.unlock();
#endif*/
    return;
}//getErrno

void
ZExceptionMin::getIcuError(const char *pModule,
                           const UErrorCode pIcuErr,
                           const Severity_type pSeverity,
                           const char* pFormat,...)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
ZExceptionBase* wExceptionBase = new ZExceptionBase;
     va_list args_1;
     va_start (args_1, pFormat);
     wExceptionBase->getIcuError(pModule,
                                 pIcuErr,
                                 pSeverity,
                                 pFormat,args_1);
     va_end(args_1);

     toStack(wExceptionBase); // push on stack
#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    if (pSeverity >= ThrowOnSeverity)
                            zthrow (ZStack.last());
    if (pSeverity >= AbortOnSeverity)
                                exit_abort();
    return;
}//getErrno


#ifdef __USE_WINDOWS__
void
ZExceptionBase::getLastError (const char *pModule,
                              const ZStatus pStatus,
                              const Severity_type pSeverity,
                              const char* pFormat,va_list arglist)
{
/*#if __USE_ZTHREAD__
        _Mtx.lock();
#endif*/
    _getLastWindowsError(Error,Complement.content,cst_exceptionlen);
     Module=pModule;
     Status = pStatus;
     Severity=pSeverity;

     vsprintf (Message.content ,pFormat, arglist);

/*#if __USE_ZTHREAD__
     _Mtx.unlock();
#endif*/
    return;
}//getLastError

void
ZExceptionMin::getLastError (const char *pModule,
                     const ZStatus pStatus,
                     const Severity_type pSeverity,
                     const char* pFormat,...)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
ZExceptionBase* wExceptionBase = new ZExceptionBase;
     va_list args_1;
     va_start (args_1, pFormat);
     wExceptionBase->getLastError (pModule,
                          pStatus,
                          pSeverity,
                          pFormat,args_1);
     va_end(args_1);

     toStack(wExceptionBase); // push on stack

#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    if (pSeverity >= AbortOnSeverity)
                                exit_abort();
    return;
}//getLastError
#endif // __USE_WINDOWS__



/**
 * @brief CZExceptionMin::get creates an file error description from errno with all elements
 * File error for mentionned file pf is searched and stored in Error field. Complement field will be set with error message using strerror_r() routine.
 *  @note may be used with Threads (use mutex)
 *
 * @param pf        FILE* for the file to be errored
 * @param pModule   method, function, routine where the exception message has been generated
 * @param pStatus   ZStatus describing the error
 * @param pSeverity Severity of the error
 * @param pFormat   Message text format followed by list of arguments
 */
void
ZExceptionBase::getFileError (FILE *pf,
                              const char *pModule,
                              const ZStatus pStatus,
                              const Severity_type pSeverity,
                              const char*pFormat,va_list arglist)
{


 // No mutex lock unlock in this module as it uses functions that themselves use mutex

     if (feof(pf))
            {
            setComplement("EOF encountered");
            _setMessage(pModule,
                       ZS_EOF,
                       pSeverity,
                       pFormat,arglist);

            return;
            }

     getErrno(ferror(pf),pModule,pStatus,pSeverity,pFormat,arglist);

     return;
}// getFileError

void
ZExceptionMin::getFileError (FILE *pf,
                              const char *pModule,
                              const ZStatus pStatus,
                              const Severity_type pSeverity,
                              const char*pFormat,...)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
    ZExceptionBase* wExceptionBase = new ZExceptionBase;
 va_list args;
 va_start (args, pFormat);

 // No mutex lock unlock in this module as it uses functions that themselves use mutex
    wExceptionBase->getFileError (pf,
                         pModule,
                         pStatus,
                         pSeverity,
                         pFormat,args);
    va_end(args);

    toStack(wExceptionBase);
#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    if (pSeverity >= ThrowOnSeverity)
                            zthrow (ZStack.last());
    if (pSeverity >= AbortOnSeverity)
                                exit_abort();
    return;
}// getFileError

#include <netdb.h>
/**
 * @brief ZExceptionBase::getAddrinfo gets the getaddrinfo returned error using gai_strerror
 * @param pError
 * @param pModule
 * @param pStatus
 * @param pSeverity
 * @param pFormat
 */
void
ZExceptionBase::getAddrinfo (int pError,
                             const char *pModule,
                             const ZStatus pStatus,
                             const Severity_type pSeverity,
                             const char*pFormat,va_list arglist)
{
 // No mutex lock unlock in this module as it uses functions that themselves use mutex

    Error = pError;
    ZExceptionBase::setComplement(gai_strerror(pError));
    ZExceptionBase::_setMessage(pModule,
                               pStatus,
                               pSeverity,
                               pFormat,arglist);


     return;
}// getAddrinfo

void
ZExceptionMin::getAddrinfo (int pError,
                             const char *pModule,
                             const ZStatus pStatus,
                             const Severity_type pSeverity,
                             const char*pFormat,...)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
ZExceptionBase* wExceptionBase = new ZExceptionBase;
 va_list args;
 va_start (args, pFormat);

 // No mutex lock unlock in this module as it uses functions that themselves use mutex

    wExceptionBase->getAddrinfo(pError,pModule,pStatus,pSeverity,pFormat,args);
    va_end(args);
    toStack(wExceptionBase);
#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    if (pSeverity >= ThrowOnSeverity)
                            zthrow (ZStack.last());
    if (pSeverity >= AbortOnSeverity)
                                exit_abort();
     return;
}// getAddrinfo

#ifdef __USE_LDAP__
#include <ldap.h>
void
ZExceptionMin::getLDAP (int pError,
                        const char *pModule,
                        const ZStatus pStatus,
                        const Severity_type pSeverity,
                        const char*pFormat,...)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
ZExceptionBase* wExceptionBase = new ZExceptionBase;
 va_list args;
 va_start (args, pFormat);

 // No mutex lock unlock in this module as it uses functions that themselves use mutex
    wExceptionBase->Error = pError;
    wExceptionBase->setComplement(ldap_err2string(pError));
    wExceptionBase->_setMessage(pModule,pStatus,pSeverity,pFormat,args);
    va_end(args);
    toStack(wExceptionBase);
#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    if (pSeverity >= AbortOnSeverity)
                                exit_abort();
     return;
}// getLDAP
#endif // __USE_LDAP__

void
ZExceptionBase::setContext (const char *pModule,ZStatus pStatus,Severity_type pSeverity)
{
    Module=pModule;
    Status=pStatus;
    Severity=pSeverity;
    return;
}
/**
 * @brief ZExceptionBase::setMessage creates an error description with all elements and pushes exception on exception stack (ZExceptionStack)
 *
 * @note may be used with Threads (use mutex)
 *
 * @param pModule   method, function, routine where the exception message has been generated
 * @param pStatus   ZStatus describing the error
 * @param pSeverity Severity of the error
 * @param pFormat   Message text format followed by list of arguments
 */
void
ZExceptionBase::_setMessage (const char *pModule,ZStatus pStatus,Severity_type pSeverity,const char *pFormat,va_list arglist)
{

    setContext(pModule,pStatus,pSeverity);
//va_list args;
//    va_start (args, pFormat);
      Message.vsprintf_char(pFormat, arglist);
//    va_end(args);

return;
}

/**
 * @brief ZExceptionMin::setMessage sets up an exception with all elements to describe error
 *
 *  According pSeverity, and either
 *  - exception may be thrown
 *  - exit_abort may be invoked
 *
 * @param pModule function/method where exception is raised. Generally _GET_FUNCTIONNAME_
 * @param pStatus   a ZS_Status
 * @param pSeverity a Severity_type
 * @param pFormat   a printf style varying argument format
 */
void
ZExceptionMin::setMessage (const char *pModule,ZStatus pStatus,Severity_type pSeverity,const char *pFormat,...)
{
// No mutex lock unlock in this module as it uses functions that themselves use mutex
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
ZExceptionBase* wExceptionBase = new ZExceptionBase;
va_list args;
    va_start (args, pFormat);
    wExceptionBase->_setMessage(pModule,pStatus,pSeverity,pFormat, args);
    va_end(args);
    toStack(wExceptionBase); // push on stack

#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    if (pSeverity >= ThrowOnSeverity)
                            zthrow (ZStack.last());
    if (pSeverity >= AbortOnSeverity)
                                exit_abort();
return;
}//setMessage

/**
 * @brief ZExceptionMin::setMessageCplt same as setMessage but waiting for a complement,
 *                      so that no abort / throw action will be taken until setComplement() will be invoked
 * @param pModule
 * @param pStatus
 * @param pSeverity
 * @param pFormat
 */
void
ZExceptionMin::setMessageCplt (const char *pModule,ZStatus pStatus,Severity_type pSeverity,const char *pFormat,...)
{
// No mutex lock unlock in this module as it uses functions that themselves use mutex
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
ZExceptionBase* wExceptionBase = new ZExceptionBase;
va_list args;
    va_start (args, pFormat);
    wExceptionBase->_setMessage(pModule,pStatus,pSeverity,pFormat, args);
    va_end(args);
    toStack(wExceptionBase); // push on stack

#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
return;
}
/*
void
ZExceptionBase::setFromZNetException(const char *pModule,const ZNetException* pZException)
{

    setContext(pModule,pZException->Status,pZException->Severity);
    Error = pZException->Error;
    Message.sprintf( "ZNetException received>> %s", pZException->Message);
    Complement.fromUtf((const utf8_t*)strerror(Error));

}

void
ZExceptionMin::setFromZNetException(const char *pModule,const ZNetException *pZException)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
    ZExceptionBase* wExceptionBase = new ZExceptionBase;
    wExceptionBase->setFromZNetException(pModule,pZException);
    toStack(wExceptionBase);
#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    return;
}
*/
/**
 * @brief ZExceptionMin::removeLast removes the last error message from exception stack
 */
void
ZExceptionMin::removeLast(void)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
    ZStack.pop() ;

#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
}

/**
 * @brief ZExceptionBase::addToLast Adds a formatted message to last(immediate) ZException message on the exception stack
 *              last exception of Exception stack is updated accordingly.
 *
 *  @note may be used with Threads (use mutex)
 *
 * @param pFormat
 */
void
ZExceptionMin::addToLast(const char *pFormat,...)
{

    if (ZStack.isEmpty())
                        return;
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif

va_list args;
va_start (args, pFormat);
utf8FixedString<cst_messagelen+1> wBuf;

//    memset(wBuf,0,sizeof(wBuf));
//    vsprintf (wBuf ,pFormat, args);
    wBuf.vsprintf_char(pFormat, args);
    va_end(args);
    ZStack.last()->Message.add(wBuf.content);
//    strncat(_Base::Message.content,wBuf,wSize);
#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    if (ZStack.last()->Severity >= ThrowOnSeverity)
                            zthrow (ZStack.last());
    if (ZStack.last()->Severity >= AbortOnSeverity)
                                exit_abort();
return;
} // addToLast



void ZExceptionBase::setComplement (const char *pFormat,...)
{

    va_list arglist;
    va_start (arglist, pFormat);
    Complement.vsprintf_char (pFormat, arglist);
    va_end(arglist);

return;
}

void ZExceptionBase::setComplement (const char *pFormat,va_list arglist)
{

//va_list args;
//va_start (args, pFormat);
        Complement.vsprintf_char (pFormat, arglist);
//        va_end(args);
return;
}

/**
 * @brief ZExceptionMin::setComplement sets up the complement zone of ZException.
 *                  This information is generally coming from lower layers, and gives a more detailed explanaition
 *                  while message itself gives application related information.
 *
 *      setComplement() will throw ZException or invoke exit_abort the same way setMessage() does.
 *
 *  if there is a need to use setComplement, so that setMessageCplt() should be first invoked
 *  to avoid aborting or throwing ZException BEFORE complement is setup.
 *
 * @param pFormat a printf like format that will be used to setup complement.
 */

void ZExceptionMin::setComplement (const char *pFormat,...)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
va_list args;
va_start (args, pFormat);
        ZStack.last()->setComplement (pFormat, args);
        va_end(args);
#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    if (ZStack.last()->Severity >= ThrowOnSeverity)
                            zthrow (ZStack.last());
    if (ZStack.last()->Severity >= AbortOnSeverity)
                                exit_abort();
return;
}
void ZExceptionMin::clearStack(void)
{
#if __USE_ZTHREAD__
    _Mtx.lock();

//    ZStack.lock();
#endif
    ZStack.clear();
#if __USE_ZTHREAD__
     _Mtx.unlock();
//    ZStack.unlock();
#endif
}

void ZExceptionMin::toStack(ZExceptionBase* pException)
{
    ZStack.push_back((ZExceptionBase*)pException);

}// toStack


/**
 * @brief CZExceptionMin::printUserMessage report exception content on mentionned pOutpout (stdout, stderr or file)
 *
 *
 * Example of exception report
 *
@verbatim
 Depth <0>
------ZException content-----
Module.........._setKeyFieldValue
Error code......0
Status..........ZS_INVSIZE
Severity........Severity_Warning
 given array size <6> for pointer exceeds size of key field array size<3> (field rank <1>). Array size truncated to key field array size

--------- Exception(s) content --------------

@endverbatim

- Depth <n> mentions from the most recent to the oldest exception report.
Sometimes errors are stacked (specifically with ZMasterFile when trying to rollback) and are stored in an exception stack.
This stack is dumped by this method.

- Module : internal method or function into which error occurred.
- Error code : if a system error then the system error code (signification will be given in message)
- Status : ZStatus error code
- Severity : this is an indicator of impact of the error on application flow @ref Severity_type
- message : an explainatory message with, best try, run-time informations


 * @param pOutput
 */
void
ZExceptionMin::printUserMessage (FILE *pOutput,bool pDelete)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
    fprintf (pOutput,
             "_______Exception stack (from most recent to oldest)________\n");
    if (ZStack.isEmpty())
                fprintf (pOutput,
                         "         Exception stack is empty\n");
    else
        {
/*    for (long wi=ZStack.lastIdx();wi>-1;wi--)
            {
            fprintf(pOutput,
                    " Depth <%ld>\n",
                    wi);
             fprintf(pOutput,ZStack[wi]->formatFullUserMessage().toString());
            }
*/
        int wi=0;
        if (pDelete)
                {
            while (ZStack.size()>0)
                        {
                        fprintf(pOutput,
                                " Depth <%d>\n",
                                wi);
                         fprintf(pOutput,ZStack.last()->formatFullUserMessage().toCString());
                         ZStack.pop();
                         wi++;
                        }
                }

            else
            {
            wi=0;
            int wj=ZStack.size()-1;
            while (wj>0)
                    {
                    fprintf(pOutput,
                            " Depth <%ld>\n",
                            wi);
                     fprintf(pOutput,ZStack[wj]->formatFullUserMessage().toCString());
                     wi++;
                     wj--;
                    }
            }// else
        }// else
    fprintf (pOutput,
             "_________________End Exception(s) content__________________\n");
#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
}//printUserMessage

/**
 * @brief ZExceptionMin::printLastUserMessage displays the last registered exception and removes it from exception stack.
 * @param pOutput
 */
void
ZExceptionMin::printLastUserMessage (FILE *pOutput)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
    if (ZStack.isEmpty())
        {
         fprintf(pOutput,"%s>> No more exception in exception stack.\n",_GET_FUNCTION_NAME_);
#if __USE_ZTHREAD__
        _Mtx.unlock();
#endif
         return;
        }
    fprintf (pOutput,
             "________________Immediate Exception content_______________\n");
    fprintf (pOutput,ZStack.last()->formatFullUserMessage().toCString());
//    ZStack.last()->Message.clear();
//    ZStack.last()->Complement.clear();
    fprintf (pOutput,
             "__________________________________________________________\n");

    ZStack.pop();

#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
}//ZExceptionMin::printLastUserMessage

utfexceptionString
ZExceptionBase::formatFullUserMessage (void)
{   utfexceptionString wRet;
    wRet.sprintf(
                 "------ZException content-----\n"
                 "Module..............%s\n"
                 "System error code...%d\n"
                 "POSIX error code....%s\n"
                 "Status..............%s\n"
                 "Severity............%s\n",
                 Module.toString(),
                 Error,
                 decode_POSIXerrno(Error),
                 decode_ZStatus(Status),
                 decode_Severity(Severity));
    wRet+=Message.conditionalNL();
    wRet+=Complement.conditionalNL();
    return wRet;
}

utfexceptionString
ZExceptionMin::formatFullUserMessage (void)
{
#if __USE_ZTHREAD__
    _Mtx.lock();
#endif
     utfexceptionString wStr=ZStack.last()->formatFullUserMessage();
#if __USE_ZTHREAD__
    _Mtx.unlock();
#endif
    return wStr;
}

/**
 * @brief CZExceptionMin::exit_abort reports the content of CZException to stderr then aborts the application.
 */
void
ZExceptionMin::exit_abort(void)
        {
        _MODULEINIT_
        printUserMessage(stderr);
        ZStack.clear();
//        exit(EXIT_FAILURE);

        _ABORT_     /* _ABORT_ list all module stack and clean it before aborting */
//        _EXIT_ (EXIT_FAILURE);
        }

void
ZExceptionMin::zthrow(ZExceptionBase* pException)
        {
        throw(pException);
        }

#if __USE_ZTHREAD__ == __USE_POSIX__
void
ZExceptionMin::Thread_exit_abort(const ZStatus pStatus)
{
    if (pStatus!=ZS_NOTHING)
                Status=pStatus;
    printUserMessage(stderr);
    pthread_exit((void*)&Status);
}
#else
void
ZExceptionMin::Thread_exit_abort(const ZStatus pStatus)
{
ZExceptionBase *wExceptionBase = new ZExceptionBase;
    if (pStatus!=ZS_NOTHING)
                wExceptionBase->Status=pStatus;
    wExceptionBase->Status=pStatus;
    toStack(wExceptionBase);

    printUserMessage(stderr);
    ZStack.clear();
    throw(pStatus);     // throw an unmanaged exception so that thread will cancel using destructors
}
#endif // __USE_ZTHREAD__

utfexceptionString& ZExceptionMin::getLastMessage(void)
{
    if (ZStack.isEmpty())
        {
        fprintf (stderr,"%s-F-EmptyStack Fatal error: stack is empty while using message\n",_GET_FUNCTION_NAME_);
        abort();
        }
    return ZStack.last()->Message;
}
utfexceptionString&
ZExceptionMin::getLastComplement(void)
{
    if (ZStack.isEmpty())
        {
        fprintf (stderr,"%s-F-EmptyStack Fatal error: stack is empty while using messageBox\n",_GET_FUNCTION_NAME_);
        abort();
        }
    return ZStack.last()->Complement;
}

ZStatus
ZExceptionMin::getLastStatus(void)
{
    if (ZStack.isEmpty())
            return ZS_SUCCESS;
    return ZStack.last()->Status;
}
Severity_type
ZExceptionMin::getLastSeverity(void)
{
    if (ZStack.isEmpty())
            return Severity_Nothing;
    return ZStack.last()->Severity;
}
void
ZExceptionMin::setContext (const char *pModule,ZStatus pStatus,Severity_type pSeverity)
{
    if (ZStack.isEmpty())
        {
        fprintf(stderr,"%s>> Exception stack is empty while trying to set last exception context with values \n"
                       "%s.\n"
                       "%s\n"
                        "%s\n",
                _GET_FUNCTION_NAME_,
                pModule,
                decode_ZStatus(pStatus),
                decode_Severity(pSeverity));
        return;
        }
    _Mtx.lock();
    ZStack.last()->setContext(pModule,pStatus,pSeverity);
    _Mtx.unlock();
    return ;
}

void
ZExceptionMin::setLastStatus(ZStatus pSt)
{
    if (ZStack.isEmpty())
        {
        fprintf(stderr,"%s>> Exception stack is empty while trying to set last exception status with value %s.\n",
                _GET_FUNCTION_NAME_,
                decode_ZStatus(pSt));
        return;
        }
    _Mtx.lock();
    ZStack.last()->Status= pSt;
    _Mtx.unlock();
    return ;
}
void
ZExceptionMin::setLastSeverity(Severity_type pSeverity)
{
    if (ZStack.isEmpty())
        {
        fprintf(stderr,"%s>> Exception stack is empty while trying to set last exception severity with value %s.\n",
                _GET_FUNCTION_NAME_,
                decode_Severity(pSeverity));
        return;
        }
    _Mtx.lock();
    ZStack.last()->Severity= pSeverity;
    _Mtx.unlock();
    if (pSeverity >= ThrowOnSeverity)
                            zthrow (ZStack.last());
    if (pSeverity >= AbortOnSeverity)
                                exit_abort();
    return ;
}

#ifdef QT_CORE_LIB




QString ZExceptionBase::formatUserMessage (void)
{   QString wRet;
    Message.conditionalNL();
    Complement.conditionalNL();
    Message.toQString();
    wRet += Complement.toQString();
    return wRet;
}


void ZExceptionBase::setComplementQS (QString pQS)
{
    Complement.setFromQString( pQS);
return;
}


int ZExceptionBase::messageBox(const char* pTitle,const char * pUserMessage)
{
    utfdescString wT;
    utfexceptionString wM;
    wT=(const utf8_t*)pTitle;
    wM=(const utf8_t*)pUserMessage;

    return messageBox(wT,wM);
}
#include <ztoolset/zuser.h>
int
ZExceptionBase::messageBox(utfdescString &pTitle,utfexceptionString &pUserMessage)
{
QString wM;
    QMessageBox::Icon wIcon;
    switch (Severity)
            {
            case (Severity_Information):
                {
                wIcon=QMessageBox::Information;
                break;
                }
            case (Severity_Warning):
                {
                wIcon=QMessageBox::Warning;
                break;
                }
    case (Severity_Error):
        {
        wIcon=QMessageBox::Critical;
        break;
        }
    case (Severity_Severe):
        {
        wIcon=QMessageBox::Critical;
        break;
        }
    case (Severity_Fatal):
        {
        wIcon=QMessageBox::Critical;
        break;
        }
    default :
        wIcon=QMessageBox::Question;
            }// switch

    QMessageBox wMsg (wIcon,
                      pTitle.toCString_Strait(),
                      QObject::tr(pUserMessage.toCString()));
    wMsg.setDetailedText(ZException.formatFullUserMessage().toCString());
    wMsg.addButton(QObject::tr("Leave"),QMessageBox::DestructiveRole);
    wMsg.addButton(QObject::tr("Continue"),QMessageBox::AcceptRole);

    wMsg.addButton(QObject::tr("Keep this message"),QMessageBox::ApplyRole);
    wMsg.exec();

    if (wMsg.buttonRole(wMsg.clickedButton())==QMessageBox::ApplyRole)
                        {
                        uriString wErrLog;
/*
                        if (UserInformation.WorkDir[0]=='\0')
                                        wErrLog = HomeDir;
                                else
                                        wErrLog = UserInformation.WorkDir ;
*/
/*                        uid_t wuid = getuid();
                        struct passwd * wuserdata = getpwuid(wuid);
                        wErrLog=wuserdata->pw_dir;
*/
                        ZUser wUser;
                        wErrLog = wUser.setToCurrentUser().getHomeDir().toString();
                        wErrLog.addConditionalDirectoryDelimiter();
                        wErrLog += (utf8_t*)"ErrLog";
                        wErrLog += Module.toString();
                        wErrLog += (utf8_t*)".log";
                        FILE *wFErrLog = fopen(wErrLog.toCString_Strait(),"w");
                        if (wFErrLog==nullptr)
                                    {
                                    fprintf(stderr, "%s-F- Fatal error cannot write Errlog %s\n",
                                            _GET_FUNCTION_NAME_,
                                            wErrLog.toString());
                                    abort();
                                    }
 //                       wM=wuserdata->pw_name;
                        wM=wUser.getName().toCString_Strait();
                        wM += "\n";
                        wM += ZDateFull::currentDateTime().toLocaleFormatted().toCString_Strait();
                        wM += "\n";
                        wM += ZException.formatFullUserMessage().toCString();
                        fwrite(wM.toStdString().c_str(),
                               wM.size(),
                               1,
                               wFErrLog);
                        fclose(wFErrLog);
                        }

    if (__ZQTVERBOSE__)
        fprintf(stderr,wM.toStdString().c_str())  ;

    if (wMsg.buttonRole(wMsg.clickedButton())==QMessageBox::DestructiveRole)
                {
//                GApplication->quit();
                exit(EXIT_SUCCESS);
                }

    return(wMsg.buttonRole(wMsg.clickedButton()));
}//messageBox

int ZExceptionBase::documentMessageBox(const char* pTitle,const char * pUserMessage)
{
    utfdescString wT;
    utfexceptionString wM;
    wT=(const utf8_t*)pTitle;
    wM=(const utf8_t*)pUserMessage;
    return documentMessageBox(wT,wM);
} //documentMessageBox

/**
 * @brief CZException::documentMessageBox
 *                      for documents : offers the choice of removing the document (returning QMessageBox::DestructiveRole)
 * @param pTitle
 * @param pUserMessage
 * @return
 */
int
ZExceptionBase::documentMessageBox(utfdescString &pTitle, utfexceptionString &pUserMessage)
{
QString wM;
    QMessageBox::Icon wIcon;
    switch (Severity)
            {
            case (Severity_Information):
                {
                wIcon=QMessageBox::Information;
                break;
                }
            case (Severity_Warning):
                {
                wIcon=QMessageBox::Warning;
                break;
                }
    case (Severity_Error):
        {
        wIcon=QMessageBox::Critical;
        break;
        }
    case (Severity_Severe):
        {
        wIcon=QMessageBox::Critical;
        break;
        }
    case (Severity_Fatal):
        {
        wIcon=QMessageBox::Critical;
        break;
        }
    default :
        wIcon=QMessageBox::Question;
            }// switch

    QMessageBox wMsg (wIcon,
                      pTitle.toCString_Strait(),
                      QObject::tr(pUserMessage.toCString()));
    wMsg.setDetailedText(ZException.formatFullUserMessage().toCString());
    wMsg.addButton(QObject::tr("Leave"),QMessageBox::AcceptRole);
    wMsg.addButton(QObject::tr("Remove document"),QMessageBox::DestructiveRole);
    wMsg.addButton(QObject::tr("Keep this message"),QMessageBox::ApplyRole);
    wMsg.exec();

    if (wMsg.buttonRole(wMsg.clickedButton())==QMessageBox::ApplyRole)
                        {
                        uriString wErrLog;
/*                        uid_t wuid = getuid();
                        struct passwd * wuserdata = getpwuid(wuid);
                        wErrLog=wuserdata->pw_dir;*/

                        ZUser wUser;
                        wErrLog = wUser.setToCurrentUser().getHomeDir().toString();
                        wErrLog.addConditionalDirectoryDelimiter();
                        wErrLog.addConditionalDirectoryDelimiter();
                        wErrLog += "ErrLog";
                        wErrLog += Module.toString();
                        wErrLog += ".log";
                        FILE *wFErrLog = fopen(wErrLog.toCString_Strait(),"w");
                        if (wFErrLog==nullptr)
                                    {
                                    fprintf(stderr, "%s-F- Fatal error cannot write Errlog %s\n",
                                            _GET_FUNCTION_NAME_,
                                            wErrLog.toString());
                                    abort();
                                    }
//                       wM=wuserdata->pw_name;
                        wM=wUser.getName().toCString_Strait();
                        wM += "\n";
                        wM += ZDateFull::currentDateTime().toLocaleFormatted().toCString_Strait();
                        wM += "\n";
                        wM += ZException.formatFullUserMessage().toCString();
                        fwrite(wM.toStdString().c_str(),
                               wM.size(),
                               1,
                               wFErrLog);
                        fclose(wFErrLog);
                        }

#if __DEBUG_LEVEL__ > 1

        fprintf(stderr,wM.toStdString().c_str())  ;
#endif

    return(wMsg.buttonRole(wMsg.clickedButton()));
}//documentmessageBox


#endif // #ifdef QT_CORE_LIB


#ifdef __USE_SSL__
#include <openssl/ssl.h>
#include <openssl/err.h>

static bool SSLErrorMessages = false;

void
ZExceptionBase::getSSLError (const char *pModule,
                             const ZStatus pStatus,
                             const Severity_type pSeverity,
                             const char* pFormat,
                             va_list arglist)
{

    if (!SSLErrorMessages)
        {
        SSL_load_error_strings();
        SSLErrorMessages=true;
        }

/*#if __USE_ZTHREAD__
        _Mtx->lock();
#endif
*/

     Module=pModule;
     Status = pStatus;
     Severity=pSeverity;
     Complement.clear();

     Error =  ERR_get_error();
ssize_t wRemain = cst_exceptionlen;
ssize_t wOffset = 0;
utfdescString wMsg;
     while (Error!=0)
     {
     ERR_error_string_n(Error,(char*)wMsg.content,wRemain);
     wRemain = wRemain- wMsg.size() ;
     wOffset = wOffset+ wMsg.size();
     Complement += wMsg.toCString_Strait();

     Error =  ERR_get_error();
     if (Error!=0)
            Complement += "\n";
     }
     Message.vsprintf_char (pFormat, arglist);
    if (Complement.isEmpty())
        {
        Complement="<no BIO error>";
        }
    if (Error==0x1408A0C1)
                Complement.add_Char("\n Probably a problem with certificate files or private key files");
/*#if __USE_ZTHREAD__
     _Mtx->unlock();
#endif*/
    return;
}//ZExceptionBase::getSSLError

void
ZExceptionMin::getSSLError(const char *pModule,
                           const ZStatus pStatus,
                           const Severity_type pSeverity,
                           const char*pFormat,...)
{
ZExceptionBase *wExceptionBase = new ZExceptionBase;
 va_list args;
 va_start (args, pFormat);

 // No mutex lock unlock in this module as it uses functions that themselves use mutex

    wExceptionBase->getSSLError(pModule,pStatus,pSeverity,pFormat,args);
    va_end(args);
    toStack(wExceptionBase);

     return;
}// getSSLError

#endif // __USE_SSL__

#ifdef __USE_WINDOWS__

void getLastWindowsError (int &pErr,descString &pMessage)
{
    _getLastWindowsError (pErr,pMessage.content,cst_desclen);
    return;
} // getLastWindowsError

#endif //__USE_WINDOWS__


#endif //#ifndef ZEXCEPTIONMIN_CPP
