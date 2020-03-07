#ifndef ZTHREADEXITHANDLER_H
#define ZTHREADEXITHANDLER_H

#include <ztoolset/zarray.h>
#include <zthread/zarglist.h>

#ifndef __ZTHREAD_EXIT_ROUTINE_STRUCT__
#define __ZTHREAD_EXIT_ROUTINE_STRUCT__
namespace zbs {



}
typedef void (*ZTH_ExitFunctor) (zbs::ZArgList*) ;
//typedef zbs::ZArray<void*>* ZTH_ExitArglist;

#endif // __ZTHREAD_EXIT_ROUTINE_STRUCT__

#ifdef ZTHREADEXITHANDLER_CPP
zbs::ZArray<threadRoutine*> GlobalExit;         // global array of functions : these functions will be executed each time a thread finishes

#else
extern zbs::ZArray<threadRoutine*> GlobalExit; // global array of functions
#endif
namespace  zbs {
class ZThread;
class ZThread_Base;
}
/**
 * @brief The ZThreadExitHandler class registers and holds functions to be executed when a thread exits, either normally or anormally.
 *
 *  There are 2 kinds of functions :
 *  - local to the thread : functions are registered using ZThreadExitHandler::registerLocalExit
 *  - global to all threads : functions are registered using ZThreadExitHandler::registerGlobalExit
 *
 * Functions execution order :
 *
 *  During ZThreadExitHandler object's destructor execution, local registered functions are executed first, then global functions.
 * In any case, functions are executed as "first in last out" execution order.
 *
 *
 */
class ZThreadExitHandler
{
public:
    ZThreadExitHandler() = default ;
    ZThreadExitHandler(ZThreadExitHandler&) = delete;               // do not copy
    ZThreadExitHandler& operator= (ZThreadExitHandler&)=delete;     // do not assign

    ~ZThreadExitHandler(void) {exitHandler();}

    friend class zbs::ZThread_Base;
    friend class zbs::ZThread;

    /**
     * @brief exitHandler executes the sequence of registered functions. Execution is done
     *  - first local routines from last registered to first registered
     *  - second global routines from last to first
     *
     *  When exitHandler is called, then thread is marked as terminated
     *
     * Arguments for each routines are passed to called routines.
     */
    void exitHandler(void);

    /**
     * @brief registerGlobalExit adds a new global routine (common to all threads) to be executed when thread finishes.
     * routines are executed from last registered to first registered (last in first out)
     * @param pArgument argument to be used by registered routine
     */


    static void registerGlobalExit(void (*pExitFunction) (void* ),void* pArgument) ;
    /**
     * @brief registerGlobalExit adds a new local routine (specific to the particular thread) to be executed when thread finishes.
     * routines are executed from last registered to first registered (last in first out)
     * @param pArgument argument to be used by registered routine
     */
    void registerLocalExit(void (*pExitFunction) (void* ),void *pArgument) ;
    /**
     * @brief setLocalExitFunctionsQueue used by running thread
     * @param pLocalExit
     */
    void setLocalExitFunctionsQueue (zbs::ZQueue<threadRoutine>* pLocalExit) {LocalExit=pLocalExit;}

    //void assignThread(ZThread_Base* pThread) {Thread=pThread;}

private:
    /*
 local array of functions : private to the thread - copied from ZThread object once it has been started
    */
    zbs::ZQueue<threadRoutine>* LocalExit=nullptr;
    bool Terminated = false;
    zbs::ZThread*        Thread=nullptr;
};



#endif // ZTHREADEXITHANDLER_H
