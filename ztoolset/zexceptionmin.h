#ifndef ZEXCEPTIONMIN_H
#define ZEXCEPTIONMIN_H

#include <config/zconfig.h>

#include <exception>
#include <ztoolset/zerror.h>

#include <ztoolset/ztoolset_common.h>
//#include <ztoolset/zbasedatatypes.h>

#include <ztoolset/exceptionstring.h>

//typedef templateString<char [cst_desclen+1]> descString;
//typedef templateString<char [cst_codelen+1]> codeString;

#ifdef __USE_ZTHREAD__
    #include <zthread/zthread.h>
#endif
#ifdef __COMMENT__
#ifdef QT_CORE_LIB
   #include <QString>
//   #include <QtGui>
#ifdef QT_GUI_LIB
   #include <QMessageBox>
#endif
#endif
#endif // __COMMENT__
#include <unicode/utypes.h>  // for UErrorCode etc...
#include <ztoolset/zutfstrings.h>

/** @addtogroup ZBSErrorGroup
 *
 * @{ */




class ZExceptionBase_Data //: public std::exception
{
public:
    ZExceptionBase_Data()=default;
    virtual ~ZExceptionBase_Data() {}

    void clear(void);


    ZExceptionBase_Data& _copyFrom(const ZExceptionBase_Data& pIn);

    ZExceptionBase_Data& _copyFrom(const ZExceptionBase_Data&& pIn)
    {
        return _copyFrom((const ZExceptionBase_Data&)pIn);
    }

    ZExceptionBase_Data& operator=(const ZExceptionBase_Data&& pIn)
    {
        return _copyFrom((const ZExceptionBase_Data&)pIn);
    }


    int             Error       = 0 ;           /**< system error code corresponding to errno. This field is positionned when and only when a system error or a file error occurred. */
    ZStatus         Status      = ZS_NOTHING;   /**< ZStatus describing zbs error*/
    utf8String      Message ;     /**< Application text message. This message is created using a varying list of argument as printf uses.*/
    utf8String      Complement;   /**<  a complementary technical information left to user's choice or generated from system error string (get() or getFileError())*/
    Severity_type   Severity= Severity_Nothing; /**< a Severity_type that mentions the kind of error and its impact on application flow.*/
    utf8String      Module ;                    /**< a string chain mentionning the origin of the error : either method, function routine etc...*/
 };

class ZNetException;
class utf8VaryingString;

class ZExceptionBase : public ZExceptionBase_Data
{
    typedef ZExceptionBase_Data _Base;

    ZExceptionBase& copyFrom(const ZExceptionBase& pIn)
    {
        _Base::_copyFrom((ZExceptionBase_Data&)pIn);
        return *this;
    }
public:
    ZExceptionBase();
    ZExceptionBase(const ZExceptionBase& pIn) {_copyFrom(pIn);}
    ZExceptionBase& operator=(const ZExceptionBase& pIn) { return copyFrom(pIn); }
    virtual ~ZExceptionBase();

//    void clear(void) {memset(this,0,sizeof(ZExceptionBase_Data));}

    virtual char* what() noexcept { return (char*)formatFullUserMessage().toCChar();}


    void setContext (const char *pModule,ZStatus pStatus,Severity_type pSeverity);

    void getErrno(const int pErrno,
             const char *pModule,
             const ZStatus pStatus,
             const Severity_type pSeverity,
             const char* pFormat,
                  va_list arglist);

    void getFileError (FILE *pf,
                       const char *pModule,
                       const ZStatus pStatus,
                       const Severity_type pSeverity,
                       const char*pFormat,
                       va_list arglist) ;
    void getAddrinfo (int pError,
                      const char *pModule,
                      const ZStatus pStatus,
                      const Severity_type pSeverity,
                      const char*pFormat,va_list arglist);
#ifdef __COMMENTED_USE_SSL__

    void getSSLError (const char *pModule,
                      const ZStatus pStatus,
                      const Severity_type pSeverity,
                      const char* pFormat,
                      va_list arglist);


#endif // __USE_SSL__
#ifdef __USE_LDAP__
    void getLDAP     (int pError,
                 const char *pModule,
                 const ZStatus pStatus,
                 const Severity_type pSeverity,
                 const char*pFormat,...);
#endif
#ifdef __USE_WINDOWS__
    void getLastError(const char *pModule,
                      const ZStatus pStatus,
                      const Severity_type pSeverity,
                      const char* pFormat,...);
#endif

    void getIcuError (const char *pModule,
                      const UErrorCode pIcuErr,
//                      const ZStatus pStatus,
                      const Severity_type pSeverity,
                      const char* pFormat,va_list arglist);


#ifdef __USE_WINDOWS__
    void getLastError(const char *pModule,
                      const ZStatus pStatus,
                      const Severity_type pSeverity,
                      const char* pFormat,
                      va_list arglist);
#endif



    void _setMessage (const char *pModule, ZStatus pStatus, Severity_type pSeverity, const char *pFormat, va_list arglist);

    void setComplement (const char *pFormat,...);
    void setComplement (const char *pFormat,va_list arglist);
    void setComplementToStatus();
    void setComplement (const utf8String& pComplement);


    static ZExceptionBase create(const char *pModule, ZStatus pStatus, Severity_type pSeverity,
        const char *pFormat,...)
    {
      ZExceptionBase wRet;
      va_list arglist;
      va_start (arglist, pFormat);
      wRet._setMessage(pModule,pStatus,pSeverity,pFormat,arglist);
      va_end(arglist);
      return wRet;
    }
  static ZExceptionBase createErrno(const int pErrno,const char *pModule, ZStatus pStatus, Severity_type pSeverity,
        const char *pFormat,...)
    {
      ZExceptionBase wRet;
      va_list arglist;
      va_start (arglist, pFormat);
      wRet.getErrno(pErrno,pModule,pStatus,pSeverity,pFormat,arglist);
      va_end(arglist);
      return wRet;
    }
  static ZExceptionBase createFileError(FILE *pf,const char *pModule, ZStatus pStatus, Severity_type pSeverity,
        const char *pFormat,...)
    {
      ZExceptionBase wRet;
      va_list arglist;
      va_start (arglist, pFormat);
      wRet.getFileError(pf,pModule,pStatus,pSeverity,pFormat,arglist);
      va_end(arglist);
      return wRet;
    }

  static ZExceptionBase createAddrinfo(int pError,const char *pModule, ZStatus pStatus, Severity_type pSeverity,
        const char *pFormat,...)
    {
      ZExceptionBase wRet;
      va_list arglist;
      va_start (arglist, pFormat);
      wRet.getAddrinfo(pError,pModule,pStatus,pSeverity,pFormat,arglist);
      va_end(arglist);
      return wRet;
    }

//    void printExceptionContent(FILE *pOutput=stderr);

    utf8VaryingString formatFullUserMessage (bool pEndNL=true);

//    void setFromZNetException(const char *pModule, const ZNetException* pZException);

//    ZExceptionBase &operator = (const ZExceptionBase &pZException) {memmove (this,&pZException,sizeof(ZExceptionBase)); return(*this);}

    utf8VaryingString formatUtf8 (void);

#ifdef __COMMENT__
#ifdef QT_CORE_LIB
    void setComplementQS (QString pQS);



    QString formatUserMessage (void);
#ifdef QT_GUI_LIB
    int messageBox(utfdescString &pTitle, utfexceptionString &pUserMessage);
    int documentMessageBox(utfdescString &pTitle, utfexceptionString &pUserMessage);
    int messageBox(const char* pTitle,const char * pUserMessage);
    int documentMessageBox(const char* pTitle,const char * pUserMessage);
#endif // QT_GUI_LIB

#endif
#endif// __COMMENT__


}; // ZExceptionBase


/**
  @brief The CZExceptionMin class this object is set by application to describe the error with appropriate information.

 - Error : system error code corresponding to errno. This field is positionned when and only when a system error or a file error occurred.
 It is set by the following methods :
    + getErrno
    + getFileError
 - Status : ZStatus describing zbs error    see @ref ZStatus
 - Message : Application text message. This message is created using a varying list of argument as printf uses.
 - Complement : a complementary technical information left to user's choice or generated from system error string (get() or getFileError())
 - Severity : a Severity_type that mentions the kind of error and its impact on application flow.   @see Severity_type
 - Module : a string chain mentionning the origin of the error : either method, function routine etc...

 In case of cascading messages, then an exception stack is maintained starting from zero (the most recent one) to the oldest.


 */
class ZExceptionStack : public zbs::ZArray<ZExceptionBase*>
{
public:
    typedef ZArray<ZExceptionBase*> _Base;
    ZExceptionStack() {}
    ~ZExceptionStack() { clear();}
    using _Base::lastIdx;
//    using _Base::pop;
    using _Base::last;
    using _Base::push_back;
#ifdef __USE_ZTHREAD__
    using _Base::lock;
    using _Base::unlock;
#endif
    using _Base::operator [];
    using _Base::size;

    ZExceptionBase& newBlankException(void)
    {
        ZExceptionBase *wEx=new ZExceptionBase;
        _Base::push(wEx);
        return(*wEx);
    }

    bool isEmpty(void) {return(size()==0);}

    void pop() {
                if (size()< 1)
                        return;
                ZExceptionBase* wB = last();
                delete wB;
                _Base::pop();
                }

    void clear() {
                    while(_Base::size()>0)
                                        pop();
                 }

};

class ZaiErrors;

class ZExceptionMin : public ZExceptionStack
{
public:
//    typedef ZExceptionBase _Base;

    ZExceptionMin() { }


    ZExceptionMin(const ZExceptionMin&)=delete;  // cannot copy because of the Mutex value
    ZExceptionMin& operator = (const ZExceptionMin&)=delete;  // cannot copy because of the Mutex value

    void clearStack(void) ;

    ZExceptionBase& newBlankException(void) {return (ZExceptionStack::newBlankException());}

    void toStack(ZExceptionBase *pException);
    void addToLast(const char *pFormat,...);
    void prependToLast(const char *pFormat,...);
    void printLastUserMessage (FILE *pOutput=stderr, bool pKeep=false);

    bool stackIsEmpty(void) {return ZExceptionStack::isEmpty();}

    ZExceptionBase& last(void)
            {
                if (ZExceptionStack::isEmpty())
                    return(ZExceptionStack::newBlankException());
                return *ZExceptionStack::last();
            }

    void removeLast(void);
    utf8String formatFullUserMessage (void);

    ZStatus getLastStatus(void);
    Severity_type getLastSeverity(void);

    void setContext (const char *pModule,ZStatus pStatus,Severity_type pSeverity);
    void setLastStatus(ZStatus pSt);
    void setLastSeverity(Severity_type pSeverity);

    utf8String getLastMessage(void) ;
    utf8String getLastComplement(void) ;

//    zbs::ZArray<ZExceptionBase*>  ZStack;
//     ZExceptionStack ZStack;

    void getErrno(const int pErrno,
             const char *pModule="",
             const ZStatus pStatus=ZS_ERROR,
             const Severity_type pSeverity=Severity_Error,
             const char* pFormat="",...) ;

    void getFileError (FILE *pf,
                       const char *pModule="",
                       const ZStatus pStatus=ZS_FILEERROR,
                       const Severity_type pSeverity=Severity_Error,
                       const char*pFormat="",...) ;
    void getAddrinfo (int pError,
                      const char *pModule,
                      const ZStatus pStatus,
                      const Severity_type pSeverity,
                      const char*pFormat,...);





    void getIcuError(const char *pModule,
                     const UErrorCode pIcuErr,
                     const Severity_type pSeverity,
                     const char* pFormat,...) ;

/** getZaiError() captures last logged error from ZaiErrors and formats a ZException */
    void getZaiError(ZaiErrors *pErrorlog);

    void setMessage (const char *pModule,ZStatus pStatus,Severity_type pSeverity,const char *pFormat,...);
    void setMessageCplt (const char *pModule,ZStatus pStatus,Severity_type pSeverity,const char *pFormat,...);
    void setComplement (const char *pFormat,...);
    void setComplement (const utf8VaryingString& pComplement);
    void setComplementToZStatus();
//    void setFromZNetException(const char *pModule,const ZNetException *pZException);
    void exit_abort(void);
    void exit_normal(void);
    void zthrow(ZExceptionBase *pException);


    void (*ZExceptionDisplayCallBack) (const utf8VaryingString& pString) = nullptr;
    void setDisplayCallBack(void (*pCallBack) (const utf8VaryingString& )) {ZExceptionDisplayCallBack = pCallBack;}
    FILE* Output=nullptr;
    void setOutput(FILE* pOutput) {Output=pOutput;}

    void _display (const utf8VaryingString& pString) ;


#if __USE_ZTHREAD__
    void Thread_exit_abort(const ZStatus pStatus=ZS_NOTHING);
#endif
    void printUserMessage (FILE *pOutput=stderr, bool pDelete=true);

    utf8VaryingString lastUtf8()   ;
#ifdef __COMMENT__
#ifdef QT_CORE_LIB
    void setComplementQS (QString pQS) {last().setComplementQS(pQS);}

    QString formatUserMessage (void){return last().formatUserMessage();}

#ifdef QT_GUI_LIB
    int messageBox(utfdescString &pTitle, utfexceptionString &pUserMessage)
    {
        return last().messageBox(pTitle,pUserMessage);
    }
    int documentMessageBox(utfdescString &pTitle, utfexceptionString &pUserMessage){ return last().documentMessageBox(pTitle,pUserMessage);}
    int messageBox(const char* pTitle,const char * pUserMessage){return last().documentMessageBox(pTitle,pUserMessage);}
    int documentMessageBox(char* pTitle,char * pUserMessage){return last().documentMessageBox(pTitle,pUserMessage);}
#endif//QT_GUI_LIB
#endif// QT_CORE_LIB
#endif//__COMMENT__
/**
 *  Defines/redefines the Severity level to which an action must be taken : either throw ZException or invoke exit_abort
 *
 *  These severity values may be different : throwing ZException and aborting may have different severity values.
 *
 *  By default
 *      - Throwing exception will never be made (set to Severity_Highest)
 *      - Abort will be invoked at Severity_Fatal
 */
    void setAbortOnSeverity(Severity_type pSeverity) {AbortOnSeverity=pSeverity;}
    void setAbortDefault(void) {AbortOnSeverity=Severity_Fatal;}

    void setAbortPush() { AbortOnSeverityOld=AbortOnSeverity; ThrowOnSeverityOld=ThrowOnSeverity; }
    void setAbortPop() { AbortOnSeverity=AbortOnSeverityOld; ThrowOnSeverity=ThrowOnSeverityOld; }

    void setThrowOnSeverity(Severity_type pSeverity) {ThrowOnSeverity=pSeverity;}
    void setThrowDefault(void) {ThrowOnSeverity=Severity_Highest;}
private:
#ifdef __USE_ZTHREAD__
    zbs::ZMutex     _Mtx;
#endif
    uint8_t         AbortOnSeverity=Severity_Fatal;
    uint8_t         ThrowOnSeverity=Severity_Highest;

    uint8_t         AbortOnSeverityOld=Severity_Fatal;
    uint8_t         ThrowOnSeverityOld=Severity_Fatal;

private:
    void clear() {/*_Base::clear();*/
      ZExceptionDisplayCallBack = nullptr;
      ZExceptionStack::clear();
      AbortOnSeverity=0xFF;
    }
};// ZExceptionMin








#ifndef ZEXCEPTIONMIN_CPP

extern ZExceptionMin               ZException;

#endif

//      extern thread_local ZExceptionMin               ZException;
//    extern ZExceptionMin               ZException;
//        extern thread_local zbs::ZArray<ZExceptionMin>  ZExceptionStack;

/** @brief _ASSERT_ZEXCEPTION_  this macro aborts and dumps ZException if condition is met.
 * It is assumed that underlying routines will set ZException with appropriate descriptive content */


#define _ASSERT_ZEXCEPTION_(__CONDITION__) \
        if (__CONDITION__) \
                ZException.exit_abort();




/** @} */ // ZBSErrorGroup

#endif // ZEXCEPTIONMIN_H
