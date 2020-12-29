#ifndef ZMODULESTACK_H
#define ZMODULESTACK_H
#include <zconfig.h>
#include <ztoolset/zarray.h>



namespace zbs {

class ZModuleStack: public zbs::ZArray<const char *>
{
public:
    ZModuleStack();
    void printStack(FILE*pOutput=stderr);
};

#ifdef __USE_ZRANDOMFILE__
#ifndef __ZOPENZRFPOOL__
#define __ZOPENZRFPOOL__
class ZFileDescriptor;
class ZOpenZRFPool: public zbs::ZArray <ZFileDescriptor*>
{
public:
    void addOpenFile(ZFileDescriptor* pFileData)
        { push(pFileData);}
    ZStatus removeFileByObject(ZFileDescriptor*pZRF);
    ZStatus removeFileByFd(int pFd);
    void closeAll();

}; //ZOpenZRFPool

/*#ifndef ZRANDOMFILE_CPP
    extern ZOpenZRFPool* ZRFPool;
#endif //ZRANDOMFILE_CPP*/

#endif // __ZOPENZRFPOOL__

#endif // __USE_ZRANDOMFILE__

#ifndef ZMODULESTACK_CPP
    extern  ZModuleStack* CurStack ;
    extern char *wMsg;
    extern ZOpenZRFPool* ZRFPool;
#endif// ZMODULESTACK_CPP

void zprintStack(void) ;

}// namespace zbs

#ifndef __ERROR_MANAGEMENT_LEVEL__

#define   __MODULEINIT__ \
    const char *_MODULENAME=_GET_FUNCTION_NAME_ ;

#define __RETURN__   return
/*
#define _EXIT_  exit
#define _ABORT_  {  fprintf(stderr,"...Abort called from module/function <%s>\n",_MODULENAME); \
                    abort();}
*/
#define _PRINTMESSAGE_(__SEVERITY__ ,__SHORTMESSAGE__, ...)  \
 \
sprintf(wMsg,__VA_ARGS__); \
zprintMessage (__SEVERITY__, _MODULENAME,__SHORTMESSAGE__,__FILE__,__LINE__,wMsg) ;

#define _ABORT_ {\
        abort() ;}

#define _ASSERT_(__CONDITION__,__ABORT_MESSAGE__,...) \
{ if (__CONDITION__)  \
        {fprintf(stderr,"%s>" __ABORT_MESSAGE__ "\n",_GET_FUNCTION_NAME_, __VA_ARGS__);\
                        _ABORT_;} }


#else
/** @brief MODULEINIT this macro has to be put at the top of the function / method */

#define   __MODULEINIT__ \
    const char *_MODULENAME =_GET_FUNCTION_NAME_ ;\
    if (!zbs::CurStack) \
        zbs::CurStack=new zbs::ZModuleStack; \
    zbs::CurStack->push(_MODULENAME);

/** @brief replaces the 'return' instruction. Warning: you must first have used MODULEINIT macro at top of the function/method */
#define __RETURN__   zbs::CurStack->pop(); return

#define ZRETURN(pInst) { zbs::CurStack->pop(); return pInst ;}


/** @brief replaces an 'exit' instruction and includes a dump of the logical call stack */

#ifdef __USE_ZRANDOMFILE__
#define _EXIT_ \
    zbs::CurStack->printStack(stdout) ; \
    zbs::ZRFPool->closeAll(); \
    exit

#ifdef __USE_LIBXML2__
    void cleanupXML(void);

    #define _ABORT_ { fprintf(stderr,"*** Abort - stack dump follows****\n");\
        zbs::CurStack->printStack(stderr) ;   \
        zbs::ZRFPool->closeAll();\
        cleanupXML();\
        abort() ;}
    #else // __USE_LIBXML2__
    #define _ABORT_ { fprintf(stderr,"*** Abort - stack dump follows****\n");\
        zbs::CurStack->printStack(stderr) ;   \
        zbs::ZRFPool->closeAll();\
        abort() ;}
#endif //__USE_LIBXML2__

#else // __USE_ZRANDOMFILE__  (not using ZRandomFile)
    #define _EXIT_ zbs::CurStack->printStack(stdout) ; \
        exit
    #ifdef __USE_LIBXML2__
        void cleanupXML(void);

        #define _ABORT_ { zbs::CurStack->printStack(stderr) ;   \
            cleanupXML(); \
            abort() ;}
        #else //__USE_LIBXML2__ (not using libxml2)
        #define _ABORT_ { zbs::CurStack->printStack(stderr) ;   \
            abort() ;}
    #endif //__USE_LIBXML2__
    #define _ABORT_ { zbs::CurStack->printStack(stderr) ;  abort() ;}

#endif // __USE_ZRANDOMFILE__


#define _ASSERT_(__CONDITION__,__ABORT_MESSAGE__,...) \
{ if (__CONDITION__)  \
        {fprintf(stderr,"%s>" __ABORT_MESSAGE__ "\n",_GET_FUNCTION_NAME_, __VA_ARGS__);\
                        _ABORT_;} }



#define _PRINTMESSAGE_(__SEVERITY__ ,__SHORTMESSAGE__, ...)  \
  sprintf(wMsg,__VA_ARGS__); \
zprintMessage (__SEVERITY__, _MODULENAME,__SHORTMESSAGE__,__FILE__,__LINE__,wMsg) ;

#define _PRINTSYSTEMMESSAGE_(__SEVERITY__ ,__SHORTMESSAGE__, ...)  \
  sprintf(wMsg,__VA_ARGS__); \
  zprintSystemMessage (__SEVERITY__,errno, _MODULENAME,__SHORTMESSAGE__,__FILE__,__LINE__,wMsg) ;


#endif // __ERROR_MANAGEMENT_LEVEL__
#ifdef __DEPRECATED_utfcodeStringMACRO__
#define _INFORMATION_(__SHORTMESSAGE__, ...) _PRINTMESSAGE_(Severity_Information,__SHORTMESSAGE__,__VA_ARGS__)
#define _WARNING_(__SHORTMESSAGE__, ...) _PRINTMESSAGE_(Severity_Warning,__SHORTMESSAGE__, __VA_ARGS__)
#define _ERROR_(__SHORTMESSAGE__, ...) _PRINTMESSAGE_(Severity_Error,__SHORTMESSAGE__, __VA_ARGS__)
#define _FATAL_(__SHORTMESSAGE__,...) _PRINTMESSAGE_(Severity_Fatal,__SHORTMESSAGE__, __VA_ARGS__)
#define _SYSTEMERROR_ (__SHORTMESSAGE__,...) _PRINTSYSTEMMESSAGE (Severity_Error,__SHORTMESSAGE__,__VA_ARGS__)
#define _SYSTEMFATAL_ (__SHORTMESSAGE__,...) _PRINTSYSTEMMESSAGE (Severity_Fatal,__SHORTMESSAGE__,__VA_ARGS__)
#endif // __DEPRECATED_MACRO__


#endif // ZMODULESTACK_H
