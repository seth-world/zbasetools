#ifndef ZMODULESTACK_H
#define ZMODULESTACK_H

#include <config/zconfig.h>
#include <ztoolset/zarray.h>

// /home/gerard/Development/zbasetools/zbase/ztoolset/zmodulestack.h

namespace zbs {

class ZModuleStack: public zbs::ZArray<const char *>
{
public:
    ZModuleStack();
    void printStack(FILE*pOutput=stderr);
};


    extern  ZModuleStack* CurStack ;
    extern char *wMsg;

void zprintStack(void) ;

}// namespace zbs

#ifdef __ERROR_MANAGEMENT_LEVEL__

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
#define _EXIT_ \
    zbs::CurStack->printStack(stdout) ; \
    exit
#define _ABORT_ { zbs::CurStack->printStack(stderr) ;  abort() ;}

#ifdef __USE_LIBXML2__
  void cleanupXML(void);

  #undef _ABORT_
  #define _ABORT_ { zbs::CurStack->printStack(stderr) ;   \
            cleanupXML(); \
            abort() ;}
  #undef _EXIT_
  #define  _EXIT_ \
    zbs::CurStack->printStack(stdout) ; \
    cleanupXML(); \
    exit

#endif //__USE_LIBXML2__




#else // not using __ERROR_MANAGEMENT_LEVEL__

#define   __MODULEINIT__ \
    const char *_MODULENAME=_GET_FUNCTION_NAME_ ;

#define __RETURN__   return

#define _EXIT_  exit
#define _ABORT_  {  fprintf(stderr,"...Abort called from module/function <%s>\n",_MODULENAME); \
                    abort();}

#define _PRINTMESSAGE_(__SEVERITY__ ,__SHORTMESSAGE__, ...)  \
 \
sprintf(wMsg,__VA_ARGS__); \
zprintMessage (__SEVERITY__, _MODULENAME,__SHORTMESSAGE__,__FILE__,__LINE__,wMsg) ;


#define _ASSERT_(__CONDITION__,__ABORT_MESSAGE__,...) \
{ if (__CONDITION__)  \
        {fprintf(stderr,"%s>" __ABORT_MESSAGE__ "\n",_GET_FUNCTION_NAME_, __VA_ARGS__);\
                        _ABORT_;} }

#endif // __ERROR_MANAGEMENT_LEVEL__


#ifdef __USE_LIBXML2__
    void cleanupXML(void);

    #define _ABORT_ { fprintf(stderr,"*** Abort - stack dump follows****\n");\
        zbs::CurStack->printStack(stderr) ;   \
        cleanupXML();\
        abort() ;}
#else // (not using libxml2)
    #define _ABORT_ { fprintf(stderr,"*** Abort - stack dump follows****\n");\
        zbs::CurStack->printStack(stderr) ;   \
        abort() ;}
#endif //__USE_LIBXML2__

#ifndef __USE_ZRANDOMFILE__


#endif // __USE_ZRANDOMFILE__ (not using ZRandomFile)


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



#endif // ZMODULESTACK_H
