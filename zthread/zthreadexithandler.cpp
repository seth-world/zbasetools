#ifndef ZTHREADEXITHANDLER_CPP
#define ZTHREADEXITHANDLER_CPP
#include <zthread/zthreadexithandler.h>
#include <zthread/zthread.h>

#include <zthread/zarglist.h>


void
ZThreadExitHandler::exitHandler(void)
{
    if (ZVerbose)
        {
        fprintf(stdout,"%s>> Entering in exitHandler for thread Process id <%ld> \n",_GET_FUNCTION_NAME_,Thread->getPid());
        }
 /*   while (Thread->ExitRoutinesQueue.size()>0)
    {
        Thread->ExitRoutinesQueue.last()->Function(Thread->ExitRoutinesQueue.last()->ArgList);
        Thread->ExitRoutinesQueue.pop();
    }
*/

/*
    while (LocalExit.size() > 0)        // first execute local exit routines from last to first and destroy
    {
    LocalExit.last().Function (LocalExit.last().Argument);
    LocalExit.pop();
    }

    for (int wi=GlobalExit.size()-1;wi >= 0;wi--)    // then execute global routines from last to first
    {
        GlobalExit[wi].Function (GlobalExit[wi].ArgList);
    }*/
    Terminated=true;
    Thread->setState(ZTHS_Terminated);
}// exitHandler
/*
void
ZThreadExitHandler::registerGlobalExit(void (*pExitFunction) (zbs::ZArray<void*>*),
                                       zbs::ZArray<void*>* pArgument)
{
    threadRoutine_struct wExitRoutine;
    wExitRoutine.Function = pExitFunction;
    wExitRoutine.Argument = pArgument;
    GlobalExit.push(wExitRoutine);
}
void
ZThreadExitHandler::registerLocalExit(void (*pExitFunction) (zbs::ZArray<void*>*),
                                      zbs::ZArray<void*>* pArgument)
{
    threadRoutine_struct wExitRoutine;
    wExitRoutine.Function = pExitFunction;
    wExitRoutine.Argument = pArgument;
    LocalExit.push(wExitRoutine);
}
*/
#endif // ZTHREADEXITHANDLER_CPP
