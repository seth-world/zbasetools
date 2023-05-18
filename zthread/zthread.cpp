#ifndef ZTHREAD_CPP
#define ZTHREAD_CPP
#include <zthread/zthread.h>
#include <ztoolset/zerror.h>
#include <ztoolset/zexceptionmin.h>
#include <zthread/zthreadexithandler.h>

#define __ZTHREADVERBOSE__  ZVerbose & ZVB_Thread

using namespace zbs;

ZThreadEventQueue::ZThreadEventQueue()
{

}

ZThread*      MainThread=nullptr;

thread_local ZThreadExitHandler atThreadExit; // defines the exit handler locally for each thread
thread_local ZThread*      ThisThread=nullptr; // pointer to main thread ZThread object */



/**
 * @brief ZThread::start starts the thread using pFunction as code.
 * @note thread attributes, if ever, should have been previously set using ZThread::init() routine.
 * if thread type is ZTH_Inherit, then thread attributes are set to their default values.
 * @param[in] pFunction static routine to execute within the thread
 * @param[in] pArglist a pointer to arguments data. Data has to be casted to the requested type in order to be used.
 * @return a boolean set to true if thread has successfully started and false if an error occurred.
 * if an error occurred, then ZException is set with appropriate details.
 */
#ifdef __USE_POSIX_ZTHREAD__

ZThread_Base::ZThread_Base(ZThread_type pType, int pPriority ,int pStackSize)
{
    ThreadId.clear();
    Type=pType;
    Priority=pPriority;
    StackSize = pStackSize;
    init(Type,Priority,StackSize);
    return;
}
/*
ZThread_Base::ZThread_Base(void* (*pFunction)(void*),void* pArglist,ZThread_type pType, ZThread_Priority pPriority ,int pStackSize)
{
        ThreadId.clear();
        Type=pType;
        Priority=pPriority;
        StackSize = pStackSize;
        init(Type,Priority,StackSize);
        State=ZTHS_Created;
        _start (pFunction, pArglist);
        return;
}
*/
ZThread_Base::~ZThread_Base(void)
{
    pthread_attr_destroy(&Thread_Attributes);
}

/**
 * @brief ZThread::init initialize thread attributes
 * @note this has to be done BEFORE thread is started using ZThread::start() routine
 * @param[in] pType         type of thread. defaulted to ZTH_Inherit see @ref ZThread_type
 * @param[in] pPriority     maximum scheduling priority for the thread. If <1 then thread priority is set to its system default. defaulted to -1.
 * @param[in] pStackSize    stack size for the thread. if < 1 then stack size is set to system default value. defaulted to -1.
 * @return a boolean set to true if successfull operation and false in case of error.
 * in case of error, ZException is set with appropriate details.
 */
bool
ZThread_Base::init(const ZThread_type pType,const ZThread_Priority pPriority,const int pStackSize)
{
int wRet;
    _Mtx.lock();
    Type = pType;
    if (State==ZTHS_Created)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Error,
                              "Cannot initialize ZThread attributes while it is running.");
        _Mtx.unlock();
        return false;
        }

    wRet=pthread_attr_init(&Thread_Attributes);
    Type=pType;
    setType(Type);

    Priority = pPriority;

    if (pPriority>0)
            setPriority(pPriority);
    setStackSize(pStackSize);
    State=ZTHS_Created;
    _Mtx.unlock();
    return true;
}// init



ZStatus ZThread_Base::_start(void*(*pFunction)(void *), void *pArgList)
{
  if (State>=ZTHS_Active)
    {
        fprintf (stderr,"ZThread::start-F-ACTIVE  Thread is active while trying to start it.\n");
        abort();
    }
int wRet;
    if (Type&ZTH_Inherit)
        {
        wRet = pthread_create(ThreadId.get_IdPtr(),
                              nullptr,
                              pFunction,
                              pArgList);
        }

    else
        {
        wRet = pthread_create(ThreadId.get_IdPtr(),
                              &Thread_Attributes,
                              pFunction,
                              pArgList);
        }
/*
         see https://linux.die.net/man/3/pthread_create
  NB: errno is not positionned but error is returned
*/
    if (wRet!=0) {
      ZStatus wSt=ZS_ILLEGAL;
      utf8VaryingString wErrMsg;
      switch (wRet) {
      case EPERM:
        wSt= ZS_ACCESSRIGHTS;
        wErrMsg.sprintf( "<EPERM> No permission to set the scheduling policy and parameters specified in attributes.");
        break;
      case EINVAL:
        wSt= ZS_INVPARAMS;
        wErrMsg.sprintf( "<EINVAL> Invalid settings in attributes.");
        break;
      case EAGAIN:
        wSt= ZS_CANTALLOCSPACE;
        wErrMsg.sprintf( "<EAGAIN> Insufficient resources to create another thread, or a system-imposed limit on the number of threads was encountered. The latter case may occur in two ways: the RLIMIT_NPROC soft resource limit (set via setrlimit(2)), which limits the number of process for a real user ID, was reached; or the kernel's system-wide limit on the number of threads, /proc/sys/kernel/threads-max, was reached.");
        break;

      default:
        wErrMsg.sprintf( "Unknown error for creating thread.");
        break;
      }// switch
      ZException.getErrno(wRet,
          _GET_FUNCTION_NAME_,
          wSt,
          Severity_Error,
          wErrMsg.toCChar());
      return wSt;
    }

    State=ZTHS_Active;

    if (Type&ZTH_Detached) {
      wRet=pthread_detach(*ThreadId.get_IdPtr());
      if (wRet!=0) {
/*
 * see https://man7.org/linux/man-pages/man3/pthread_detach.3.html
 *   NB: errno is not positionned but error is returned
 */
          ZStatus wSt=ZS_ILLEGAL;
          utf8VaryingString wErrMsg;
          switch (wRet) {
          case ESRCH:
            wSt= ZS_INVVALUE;
            wErrMsg.sprintf( "<ESRCH> No thread with the ID thread could be found.");
            break;
          case EINVAL:
            wSt= ZS_INVPARAMS;
            wErrMsg.sprintf( "<EINVAL> thread is not a joinable thread.");
            break;

          default:
            wErrMsg.sprintf( "Unknown error for detaching thread.");
            break;
          }// switch
          ZException.getErrno(wRet,
                    _GET_FUNCTION_NAME_,
                    wSt,
                    Severity_Error,
                    wErrMsg.toCChar());
          return wSt;
      } // if (wRet!=0)
    }// if (Type&ZTH_Detached)
    return ZS_SUCCESS;
}// _start

/**
 * @brief ZThread::join Joins the thread to current thread. Current thread will wait for thread to terminate using ZThread::exitThread() routine
 * @return returns a boolean set to true if operation ends successfully.
 * In case of error, false is returned and ZException is set with appropriate message and status.
 */
ZStatus ZThread_Base::join()
{
  /* see https://www.man7.org/linux/man-pages/man3/pthread_join.3.html
   */
    int wRet=pthread_join(ThreadId.get_Native_Handle(),nullptr);  // no return value
    if (wRet==0)
            return ZS_SUCCESS;
    ZStatus wSt=ZS_ILLEGAL;
    utf8VaryingString wErrMsg;
    switch (wRet) {
    case ESRCH:
      wSt= ZS_INVVALUE;
      wErrMsg.sprintf( "<ESRCH> No thread with the ID thread could be found.");
      break;
    case EINVAL:
      wSt= ZS_ILLEGAL;
      wErrMsg.sprintf( "<EINVAL> Thread is not a joinable thread or another thread is already waiting to join with this thread.");
      break;
    case EDEADLK:
      wSt= ZS_LOCKINTERR;
      wErrMsg.sprintf( "<EDEADLK> A deadlock was detected (e.g., two threads tried to join with each other); or thread specifies the calling thread.");
      break;
    default:
      wErrMsg.sprintf( "Unknown error for joining thread.");
      break;
    }// switch
    ZException.getErrno(wRet,
        _GET_FUNCTION_NAME_,
        wSt,
        Severity_Error,
        wErrMsg.toCChar());
    return wSt;
} // join
/**
 * @brief ZThread::detach Detaches the thread from father's thread.
 * @return returns a boolean set to true if operation ends successfully.
 * In case of error, false is returned and ZException is set with appropriate message and status.
 */
ZStatus ZThread_Base::detach()
{
    int wRet=pthread_detach (ThreadId.get_Native_Handle());
    if (wRet==0)
            {
            int wType = (int)Type;
            wType &= ~ZTH_Joinable ;
            wType |= ZTH_Detached;
            Type = (ZThread_type)wType;
            return ZS_SUCCESS;
            }

/*
 * see https://man7.org/linux/man-pages/man3/pthread_detach.3.html
 *   NB: errno is not positionned but error is returned
 */
        ZStatus wSt=ZS_ILLEGAL;
        utf8VaryingString wErrMsg;
        switch (wRet) {
        case ESRCH:
          wSt= ZS_INVVALUE;
          wErrMsg.sprintf( "<ESRCH> No thread with the ID thread could be found.");
          break;
        case EINVAL:
          wSt= ZS_INVPARAMS;
          wErrMsg.sprintf( "<EINVAL> thread is not a joinable thread.");
          break;

        default:
          wErrMsg.sprintf( "Unknown error for detaching thread.");
          break;
        }// switch
        ZException.getErrno(wRet,
            _GET_FUNCTION_NAME_,
            wSt,
            Severity_Error,
            wErrMsg.toCChar());
        return wSt;
} // detach
/**
 * @brief ZThread::initPriority initializes the priority value for Thread to be created.
 * A value of -1 (default) or 0 indicates that parameter is set to its default.
 * @note Thread parameters are to be set BEFORE Thread is created.
 * @param pPriority
 */
ZStatus
ZThread_Base::initPriority(ZThread_Priority pPriority)
{
    sched_param param;

    /* set the priority; others are unchanged */

    param.sched_priority =(int) pPriority;

    /* set the new scheduling param */
    int wRet = pthread_attr_setschedparam (&Thread_Attributes, &param);
/*
 * see https://man7.org/linux/man-pages/man3/pthread_attr_setschedparam.3.html
 *      and https://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html
 */
    if (wRet==0) {
      Priority = pPriority;
      return ZS_SUCCESS ;
    }

    fprintf(stderr,"ZThread::setPriority-F-CANTSETPRIO Cannot set ZThread priority. Most probably ZThread attributes are invalid  : %s\n",
            strerror(wRet));

    ZStatus wSt=ZS_INVVALUE;
    utf8VaryingString wErrMsg;
    switch (wRet) {
    case EINVAL:
      wSt= ZS_INVVALUE;
      wErrMsg.sprintf( "<EINVAL> The priority specified in param does not make sense for the current scheduling policy of thread attributes.");
      break;
    case ENOTSUP:
      wSt= ZS_INVVALUE;
      wErrMsg.sprintf( "<ENOTSUP> Not supported : The implementation does not support the requested feature or value.");
      break;
    default:
      wSt=ZS_ILLEGAL;
      wErrMsg.sprintf( "Unknown error for setting thread priority.");
      break;
    }// switch
    ZException.getErrno(wRet,
        _GET_FUNCTION_NAME_,
        wSt,
        Severity_Error,
        wErrMsg.toCChar());
  return wSt ;
}// initPriority

void
ZThread_Base::setPriority(ZThread_Priority pPriority)
    {
    struct sched_param  wNewPriority;
    wNewPriority.__sched_priority=pPriority;
    initPriority(pPriority); // then update thread attributes
    int wRet=pthread_setschedparam(get_Native_Handle(),
                                   SCHED_OTHER,// default for linux
                                   (const struct sched_param*)&wNewPriority
                                   );
    if (wRet!=0)
            {
            fprintf(stderr,"ZThread::setPriority-F-CANTSETPRIO Cannot set ZThread priority. Most probably ZThread attributes are invalid and/or ZThread hasn't been created : %s\n",
                    strerror(wRet));
            abort();
            }

    return ;

}// setPriority

/**
 * @brief ZThread::setType sets the type of Thread to be created. see @ref ZThread_type.
 * This parameter is defaulted to ZTH_Inherit
 * @note Thread parameters are to be set BEFORE Thread is created.
 * @param pType
 */
void
ZThread_Base::setType(const int pType)
{
int wRet=0;
    Type =(ZThread_type) pType;
    if(Type&ZTH_Detached)
                {
                    wRet=pthread_attr_setdetachstate(&Thread_Attributes,PTHREAD_CREATE_DETACHED);

                }
    else
    if(Type&ZTH_Joinable)
                {
                    wRet=pthread_attr_setdetachstate(&Thread_Attributes,PTHREAD_CREATE_JOINABLE);
                }

    if (wRet!=0)
                    {
                    fprintf (stderr,"ZThread::setType-F- Error initializing ZThread attribute : <%s>\n",
                             strerror(wRet));
                    abort();
                    }
    return ;

}// setType
/**
 * @brief ZThread::setStackSize sets the stack size for the Thread to be created.
 * A value of -1 (default) or 0 indicates that parameter is set to its default
 * @note Thread parameters are to be set BEFORE Thread is created.
 * @param pStackSize
 */
void
ZThread_Base::setStackSize (const int pStackSize)
{
    StackSize = pStackSize;
    if (pStackSize <1)
                    return;
    int wRet = pthread_attr_setstacksize(&Thread_Attributes, StackSize);
    if (wRet == 0)
            return;
    fprintf (stderr,"ZThread::setStackSize-F- Error setting Thread stack size  : <%s>\n",
             strerror(wRet));
    abort();

}// setStackSize

ZThread_Priority
ZThread_Base::getPriority( void )
    {
    sched_param param;
    /* get the new scheduling param */
    int wRet = pthread_attr_getschedparam (&Thread_Attributes, &param);
    if (wRet!=0)
            {
            fprintf(stderr,"ZThread::getPriority-F-CANTGETPRIO Cannot get ZThread priority. Most probably ZThread attributes are invalid and/or ZThread hasn't been created : %s\n",
                    strerror(wRet));
            abort();
            }
    return ((ZThread_Priority) param.__sched_priority);
}

bool
ZThread_Base::kill(void)
{
    _Mtx.lock();

    if ((State==ZTHS_Terminated)||(State<ZTHS_Active))
                    fprintf (stderr,"ZThread::kill-W>> Warning sending signal to inactive ZThread.\n");
    int wRet=pthread_kill(ThreadId.get_Native_Handle(),SIGABRT);

    if (wRet==0)
            {
            if (__ZTHREADVERBOSE__)
                    fprintf(stderr,"ZThread::kill-E-CANTSEND>> Cannot send signal to ZThread id <%ld>",ThreadId.get_Native_Handle());

             return false;
             }
    _Mtx.unlock();
    return true;
} // kill

/*  Nota Bene :
 *
 *  Following code is deprecated because :
 *
 *  - it uses non portable Posix pthread facilities
 *
 *  - when using inter-thread signal : non portable to windows.
 *
 *  in counterpart, the strategy of positionning ZThread::State has been taken :
 *
 *  ZThread::cancelRequest() must be used : it positions State to ZTHS_CancelRequested.
 *
 *  Child threads must periodically test ZThread::State to know if a cancel request has been emitted.
 *
 *
 *
 */

#ifdef __DEPRECATED__
/**
 * @brief ZThread::  sends the signal SIGABRT to the thread
 * @return a boolean set to true if sent is successful and false if an error occurred.
 * if an error occurred, then ZException is set with appropriate details.
 */




/**
 * @brief ZThread::  sends the signal SIGINT to the thread
 * @return a boolean set to true if sent is successful and false if an error occurred.
 * if an error occurred, then ZException is set with appropriate details.
 */

bool
ZThread_Base::cancel(void)
{
    return sendSignal(SIGINT);
} // kill
/**
 * @brief ZThread::  sends a signal pSignal to the thread
 * @return a boolean set to true if sent is successful and false if an error occurred.
 * if an error occurred, then ZException is set with appropriate details.
 */

bool
ZThread_Base::sendSignal(const int pSignal)
{
    _Mtx.lock();

    if ((State==ZTHS_Terminated)||(State<ZTHS_Active))
                    fprintf (stderr,"ZThread::sendSignal-W>> Warning sending signal to inactive ZThread.\n");
    int wRet=pthread_kill(Id,pSignal);

    if (wRet==0)
            {
            if (__ZTHREADVERBOSE__)
                    fprintf(stderr,"ZThread::sendSignal-E-CANTSEND>> Cannot send signal to ZThread id <%d>",Id);

             return false;
             }
    _Mtx.unlock();
    return true;
} // sendSignal
/**
 * @brief ZThread::exit terminates current thread and returns pReturn data.
 * @param[in] pReturn returned data. no return if set to nullptr (default value)
 */
void
ZThread_Base::exitThread(void* pReturn)
{
    Mtx.lock();
    Created=false;
    Id = 0;
    State = ZTHS_Terminated;
    Mtx.unlock();
    pthread_exit(pReturn);
}
#endif //__DEPRECATED__

//#ifdef __DEPRECATED__
void
ZThread_Base::exitThread(int pReturn)
{
    setState(ZTHS_Terminated);
    _created=false;
//    ThreadId.clear();
    exit (pReturn);
}
//#endif // __DEPRECATED__

#else //============= __USE_STD_ZTHREAD__

ZThread_Base::ZThread_Base(ZThread_type pType, int pPriority ,int pStackSize)
{
//    Id=0;
    Type=pType;
    Priority=pPriority;
    StackSize = pStackSize;

    return;
}

ZThread_Base::ZThread_Base(void*(* pFunction)(void*),void* pArglist,ZThread_type pType, ZThread_Priority pPriority ,int pStackSize)
{
//        Id=0;
        Type=pType;
        Priority=pPriority;
        StackSize = pStackSize;

        State=ZTHS_Created;
        _start (pFunction, pArglist);
        return;
}
ZThread_Base::~ZThread_Base(void)
{
    if (StdThread)
            delete StdThread;
    StdThread=nullptr;
}



bool
ZThread_Base::_start(void*(*pFunction)(void *), void *pArglist)
{
    if (State>=ZTHS_Active)
    {
        fprintf (stderr,"ZThread::start-F-ACTIVE  Thread is active while trying to start it.\n");
        abort();
    }


//    Arglist = pArglist;
//    ThreadMainFunction=pFunction;

    StdThread= new std::thread(pFunction,pArglist);
    ThreadId=StdThread->get_id();

    if (Type&ZTH_Detached)
                StdThread->detach();

    State=ZTHS_Active;
    return true;
}// _start



/**
 * @brief ZThread::join Joins the thread to current thread. Current thread will wait for thread to terminate using ZThread::exit() routine
 * @return returns a boolean set to true if operation ends successfully.
 * In case of error, false is returned and ZException is set with appropriate message and status.
 */
bool
ZThread_Base::join()
{
    StdThread->join();

    return true;
} // join

bool
ZThread_Base::detach()
{
    StdThread->detach();

    return true;
} // detach


/**
 * @brief ZThread::  kills the thread
 *
 * invokes terminate()
 * executes the associated
 * @return a boolean set to true if thread has successfully terminated and false if an error occurred.
 */

bool
ZThread_Base::kill(void)
{

    _Mtx.lock();
    delete StdThread; // invokes terminate();
    StdThread=nullptr;
    ThreadId.clear();
//    memset(&Id,0,sizeof(Id));
     Created = false;
     State = ZTHS_Terminated;
     _Mtx.unlock();
     return true;
} // kill
/**
 * @brief ZThread::setPriority sets the priority value for Thread to be created.
 * A value of -1 (default) or 0 indicates that parameter is set to its default.
 * @note Thread parameters are to be set BEFORE Thread is created.
 * @param pPriority
 */
void
ZThread_Base::setPriority(ZThread_Priority pPriority)
    {
    Priority = pPriority;
    return ;

}// setPriority
/**
 * @brief ZThread::setType sets the type of Thread to be created. see @ref ZThread_type.
 * This parameter is defaulted to ZTH_Inherit
 * @note Thread parameters are to be set BEFORE Thread is created.
 * @param pType
 */
void
ZThread_Base::setType(const int pType)
{

    Type = pType;

    return ;

}// setType
void
ZThread_Base::initStackSize(const int pStackSize)
{
    StackSize=pStackSize;
}
/**
 * @brief ZThread::setStackSize sets the stack size for the Thread to be created.
 * A value of -1 (default) or 0 indicates that parameter is set to its default
 * @note Thread parameters are to be set BEFORE Thread is created.
 * @param pStackSize
 */
void
ZThread_Base::setStackSize (const int pStackSize)
{
fprintf(stderr,"ZThread::setStackSize-W-CANT Cannot set stackSize using pure STD ZThreads.\n"
         "Nota Bene: For windows, only current thread may adapt stack size using <SetThreadStackGuarantee> function\n");
}// setStackSize

ZThread_Priority
ZThread_Base::getPriority( void )
    {
    return (Priority);
}

#ifndef __USE_WINDOWS__
/**
 * @brief ZThread_Base::exitThread using std::thread, there is no real exit thread facility.
 *  So an exception is thrown (equal to pReturn) to unwind stack.
 * @param pReturn
 */
void
ZThread_Base::exitThread(int pReturn)
{
    setState(ZTHS_Terminated);
    Created=false;
    ThreadId.clear();
    if (__ZTHREADVERBOSE__)
        {
        fprintf (stderr,"%s>> exitThread has been called : an exception will be thrown with value %d\n",_GET_FUNCTION_NAME_,pReturn);
        }
    throw (pReturn); // we need to throw an unmanaged exception equal to pReturn to unwind stack
}
#endif // __USE_WINDOWS__

#endif //__USE_STD_ZTHREAD__

#ifdef __USE_WINDOWS__
void
ZThread_Base::initPriority(ZThread_Priority pPriority)
{
    Priority=pPriority;
}


void
ZThread_Base::setPriority(ZThread_Priority pPriority)

BOOL wRet = SetThreadPriority((HANDLE) get_Native_Handle(), pPriority);
    if (!wRet)
            {
            fprintf(stderr,"ZThread::setPriority-F-CANTSETPRIO Cannot set ZThread priority. Most probably ZThread attributes are invalid and/or ZThread hasn't been created : %s\n",
                    strerror(wRet));
            abort();
            }
    initPriority(pPriority); // then update thread attributes
    return ;

}// setPriority

bool
ZThread_Base::kill(void)
{
    Mtx.lock();
    DWORD wExitCode;
    HANDLE wThreadId = (HANDLE) get_Native_Handle();
    bool wRet=TerminateThread(wThreadId,wExitCode);  // TerminateThread does NOT unwind stack
    if (wRet==0)
            {
            ZException.getLastError(_GET_FUNCTION_NAME_,
                                    ZS_SYSTEMERROR,
                                    Severity_Error,
                                    "Cannot kill ZThread id <%d>",Id);
            Mtx.unlock();
            return false;
            }
     Created = false;
     State = ZTHS_Terminated;
     Mtx.unlock();
     return true;
} // kill


void
ZThread_Base::exitThread(int pReturn)
{
    setState(ZTHS_Terminated);
    Created=false;
    ThreadId.clear();

    ExitThread ((DWORD)pReturn); // Windows ExitThread unwinds the stack.
}
#endif

#ifdef __NOT_PORTABLE__
// not portable to windows neither to Android
/**
 * @brief ZThread::timedjoin Same as join()
 * Joins the thread to current thread. Current thread will wait for thread to terminate using ZThread::exit() routine
 * during the time given by pTimeout.
 * @return returns a boolean set to true if operation ends successfully.
 * In case of error, false is returned and ZException is set with appropriate message and status.
 */
bool ZThread_Base::timedjoin(void **pArgRet,const struct timespec pTimeout)
{
    int wRet=pthread_timedjoin_np(Id,pArgRet,pTimeout);
    if (wRet==0)
            return true;
    ZException.getErrno(wRet,
                   _GET_FUNCTION_NAME_,
                   ZS_SYSTEMERROR,
                   Severity_Error,
                   "Cannot join ZThread");
    return false;
} // join
#endif //__NOT_PORTABLE__


#ifdef __USE_WIN_ZTHREAD__
#include <zthread/zthread_win.cpp>
#endif// __USE_WIN_ZTHREAD__

void
ZThread_Base::setState (ZThreadState_type pState)
{
    _Mtx.lock();
    State=pState;
    _Mtx.unlock();
}
//===================ZThread============================================================

ZThread::ZThread(ZThread* pFather)
{
  memset(Identity,0,sizeof(Identity));
  Father=pFather;
}

ZThread::~ZThread() // when thread object is deleted, registered exit routines are executed, then routine structure are deleted
{

    while (!ExitRoutinesQueue.isEmpty())
    {
        threadRoutine* wExitRoutine = ExitRoutinesQueue.popR();
        wExitRoutine->Function(wExitRoutine->ArgList);
        delete wExitRoutine;
    }//while
    if (ArgList)
      delete ArgList;
}

/**
 * @brief ZThread::registerExitFunction registers the function that will be called at Thread cancellation
 *  This function will be executed even in case of 'brutal' thread deletion.
 *
 * @param pExitFunction a ZTH_Functor for the function to be executed
 * @param pExitArguments a ZTH_Arglist with the list of arguments to be passed to function
 */
void
ZThread::registerExitFunction(ZTH_Functor pExitFunction,...)

{
    va_list wExitArguments;
    va_start(wExitArguments,pExitFunction);

    threadRoutine* wExitFunction=new threadRoutine(pExitFunction,wExitArguments);
    ExitRoutinesQueue.push(wExitFunction);
    va_end(wExitArguments);
}


/*void
ZThread::registerExitFunction(threadRoutine &pExitFunction)

{
    ExitRoutinesQueue.push(pExitFunction);
}*/

void
ZThread::cancelRequest()
{
ZTHEvent wEvent;
    wEvent.Signal = ZTSGN_CancelRequest;
    EventQueue.push(wEvent);
    setState(ZTHS_CancelRequested);
}
void
ZThread::dormantRequest()
{
ZTHEvent wEvent;
    wEvent.Signal = ZTSGN_SetDormant;
    EventQueue.push(wEvent);
    setState(ZTHS_DormantRequest);
}
void
ZThread::sleepRequest(const unsigned long pMilliseconds )
{
ZTHEvent wEvent;
    wEvent.Signal = ZTSGN_Sleep;
    wEvent.Sleep.Milliseconds = pMilliseconds;
    EventQueue.push(wEvent);
    setState(ZTHS_SleepRequested);
}
void
ZThread::executeFunctionRequest(ZTH_Functor pFunction, ZArgList *pArglist)
{
ZTHEvent wEvent;
    wEvent.Signal = ZTSGN_Execute;
    wEvent.Execute.Function = pFunction;
    wEvent.Execute.Arglist = pArglist;
    EventQueue.push(wEvent);
    setState(ZTHS_ExecuteRequested);
}
/**
 * @brief ZThread::start starts thread's main loop
 *
 *  initialize thread's parameters
 *  executes the given function within the thread and within the loop
 *  at any return of the functions, checks wether event queue contains signals to the thread
 *  if so, signal will be executed see @ref ZThreadSignal_type for signals descriptions
 *
 *
 *  ArgList will have the following content
 *
 *  Depth
 *
 *  lastIdx() : Pointer to ZThread object
 *  -1          Pointer to user function to execute
 *  -2          ----Last user argument----
 *          ...
 *  0           first user argument
 *
 *
 *
 * @param pFunction
 * @param pArglist
 */

void
ZThread::startLoopArg(ZTH_Functor pFunction, ZArgList *pArgList)
{
    copyArguments(pArgList);
    ArgList->push((void*)pFunction);
    ArgList->push(this);
    _start(&_threadLoop,ArgList);
}
void
ZThread::startLoopVariadic(ZTH_Functor pFunction,...)
{
void *wArgPtr=nullptr;

    if (ArgList==nullptr)
      ArgList=new ZArgList;
    ArgList->reset();

     va_list wArguments;
     va_start(wArguments,pFunction);
     wArgPtr=va_arg(wArguments,void*);
     while(wArgPtr!=nullptr)
     {
       ArgList->push(wArgPtr);
        wArgPtr=va_arg(wArguments,void*);
     }
     va_end(wArguments);
    ArgList->push((void*)pFunction);
    ArgList->push(this);
    _start(&_threadLoop,ArgList);

}//startLoopVariadic

void
ZThread::startLoop(ZTH_Functor pFunction,int argc,char** argv)
{
    if (ArgList==nullptr)
            ArgList = new ZArgList;
    ArgList->reset();

    for (int wi=0; wi < argc; wi++)
            addArgument(argv[wi]);

    if (MainThread==nullptr)
          MainThread = this;      /* push pointer to main thread ZThread object (this object)*/

    ArgList->push((void*)pFunction);
    ArgList->push(this);/* push pointer to local thread ZThread object */
    _start(&_threadLoop,ArgList);
}

void
ZThread::startNoLoopArg(ZTH_Functor pFunction, ZArgList *pArgList)
{
    copyArguments(pArgList);
    ArgList->push((void*)pFunction);
    ArgList->push(this);
    _start(_threadNoLoop,ArgList);
}

void
ZThread::startNoLoop(ZTH_Functor pFunction,int argc,char** argv)
{
    if (ArgList==nullptr)
            ArgList = new ZArgList;
    ArgList->reset();

    for (int wi=0; wi < argc; wi++)
            addArgument(argv[wi]);

    ArgList->push((void*)pFunction);
    ArgList->push(this);

    _start(_threadNoLoop,ArgList);
}
void
ZThread::startNoLoop(ZTH_Functor pFunction)
{
    if (ArgList==nullptr)
            ArgList = new ZArgList;
    ArgList->clear();
    ArgList->push((void*)pFunction);
    ArgList->push(this);

    _start(_threadNoLoop,(void*)ArgList);
}

bool
ZThread::startNoLoopVariadic(ZTH_Functor pFunction,...)
{
void *wArgPtr=nullptr;
long wArgCount=0;

    if (ArgList==nullptr)
                ArgList = new ZArgList;
    ArgList->clear();

     va_list wArguments;
     va_start(wArguments,pFunction);
     wArgPtr=va_arg(wArguments,void*);
     while(wArgPtr!=nullptr)
     {
        ArgList->push(wArgPtr);
        wArgCount++;
        wArgPtr=va_arg(wArguments,void*);
     }
     va_end(wArguments);
     ArgList->push((void*)pFunction);
     ArgList->push(this);
     return _start(_threadNoLoop,ArgList);
}


/**
 * @brief _threadLoop  this is the main thread function : it is a loop that executes the Thread main function
 *
 * after execution, Signal is tested and may involve various actions.
 *
 * pArglist is a pointer to a ZArray<void*> (ZArglist) with the following structure (from last to first)
 *
 *  1-  pointer to the the arguments for the main thread function to launch
 *  2-  pointer to a ZTH_Functor <i.e.  ZStatus (*ZTH_Functor)(zbs::ZArray<void*>) > main thread function to launch
 *  3-  pointer to ZThread data structure
 *
 * @param pArglist
 * @return
 */




void *
_threadLoop( void* pArgList)
{
ZArgList* wArglist = static_cast<ZArgList*> (pArgList);

ZThread* wThread = static_cast<ZThread*>   (wArglist->popR());
ThisThread=wThread;
/* MainThread contains a pointer to main thread ZThread object */

ZTH_Functor wMainFunction = reinterpret_cast<ZTH_Functor>    (wArglist->popR());

    ThisThread->ProcessId = getpid ();         // keep thread process id in the ZThread object
    ThisThread->ThreadId=ThisThread->getId();  // store thread id

    while (ThisThread->ThreadStatus==ZS_SUCCESS)
        {
        ThisThread->ThreadStatus = (*wMainFunction)(wArglist);
        if (ThisThread->ThreadStatus!=ZS_SUCCESS)
                                                break;
        // if status is not OK : End the thread
         ThisThread->ThreadStatus= ThisThread->processEventQueue();

        }// while !wExitThread


  return &ThisThread->ThreadStatus;

}//_threadLoop


/**
 * @brief _threadNoLoop
 *
 * calling convention :
 *
 *  functors are
 *
 * @param pArgList
 * @return
 */
void *
_threadNoLoop( void* pArgList)
{
void* wVoidPtr;

    ZArgList* wArgList = static_cast<ZArgList*> (pArgList);

// for debug on arguments within thread
    fprintf(stdout,"_threadNoLoop>> Arguments:\n");
    for (long wi=0;wi<wArgList->size();wi++)
            fprintf (stdout,"%ld>> <%p>\n",wi,wArgList->Tab[wi]);


    ZThread* wThread = static_cast<ZThread*>   (wArgList->popRP(&wVoidPtr));
    ThisThread = wThread;

    ZTH_Functor wMainFunction = reinterpret_cast<ZTH_Functor>    (wArgList->popRP(&wVoidPtr));

    wThread->ProcessId = getpid ();      // keep thread process id in the ZThread object
    wThread->ThreadId=getSysThreadId();  // store current system thread id

    if (__ZTHREADVERBOSE__)
        {
        fprintf (stdout,"%s>Thread has been created. Thread pid is <%ld> thread id is <%ld>\n",
                 _GET_FUNCTION_NAME_,
                 (long)wThread->ProcessId,
                 (long)wThread->ThreadId.get_Native_Handle());
        }

    wThread->ThreadStatus = (*wMainFunction)(wArgList);

    if (__ZTHREADVERBOSE__)
        {
        const char* wBuf;
        switch (wThread->ThreadStatus)
            {
            case ZS_SUCCESS:
                {
                wBuf="Normal";
                break;
                }
            case ZS_ENDCLIENT:
                {
                wBuf="At client request";
                break;
                }
            case ZS_CANCEL:
                {
                wBuf="On event cancel request";
                break;
                }
        default:
                {
                wBuf="Abnormal";

                break;
                }
            }   //switch

        fprintf (stdout,"%s>> <%s> Termination of thread <%s> id <%ld> with status <%s> \n",
                 _GET_FUNCTION_NAME_,
                 wBuf,
                 wThread->getIdentity(),
                 (long)wThread->ThreadId.get_Native_Handle(),
                 decode_ZStatus(wThread->ThreadStatus));
        } // __ZTHREADVERBOSE__
    if (!ZException.stackIsEmpty())
        {
        fprintf (stderr,"%s>> thread <%ld> status <%s> Error stack follows\n",
                 _GET_FUNCTION_NAME_,
                 (long)ThisThread->ThreadId.get_Native_Handle(),
                 decode_ZStatus(wThread->ThreadStatus) );
        ZException.printUserMessage(stderr,true);
        }
    return &wThread->ThreadStatus;

}// _threadNOLoop

ZStatus
ZThread::processEventQueue()
{
ZTHEvent wEvent;
ZStatus wSt=ZS_SUCCESS;
//bool wExitThread=false;

    EventQueue.lock();
    while (EventQueue.size()>0)
    {
    wEvent = EventQueue.last();
    switch (wEvent.Signal)
    {
    case ZTSGN_CancelRequest:
            {
            setState(ZTHS_Cancelling);
//            wExitThread = true;

// user registered cancellation routine will be executed when thread_local exit handler object will be deleted

//            _Base::exitThread(ZS_CANCEL);
            threadRoutine *wRoutine;
            while(ExitRoutinesQueue.size()>0)
                {
                wRoutine=ExitRoutinesQueue.last();
                wRoutine->Function(wRoutine->ArgList);
                delete wRoutine;
                ExitRoutinesQueue.pop();
                }

            setState(ZTHS_Terminated);
            return (ZS_CANCEL);
            }//ZTSGN_CancelRequest

    case ZTSGN_Sleep:
        {
        setSleep(wEvent.Sleep.Milliseconds);
        break;
        }
    case ZTSGN_SetDormant:
        {
/*               wThread->setState(ZTHS_Dormant);
        wThread->_MtxCond.wait();
        wThread->setState(ZTHS_Active);*/
        setDormant();
        break;
        }
    case ZTSGN_Execute:
        {
        setState(ZTHS_Executing);

        wSt= wEvent.Execute.Function(wEvent.Execute.Arglist);

        if (wSt!=ZS_SUCCESS)
                 _Base::exitThread(wSt);
         setState(ZTHS_Active);
        break;
        }
    }// switch
    EventQueue.pop(); // forget this and get next
    } // while (wThread->EventQueue.size()>0)
    EventQueue.unlock();

    return wSt;
}// processEventQueue


/**
 * @brief getSysThreadId  get current thread  ZThread_Id object
 * @return
 */
ZThread_Id getSysThreadId(void)
{

#ifdef __USE_POSIX_ZTHREAD__
ZThread_Native_Handle_type wid = pthread_self();

ZThread_Id wId (wid);
#else
ZThread_Id wId(std::this_thread::get_id());
#endif
return wId;
}


zbs::ZThread* getThisThread()
{
  return ThisThread;
}


void ZThread::addArgument(void* pArgument)
{
  if (ArgList==nullptr)
    initArgList();
  ArgList->push(pArgument);
}
void ZThread::addArgument(ZTH_Functor pFunction)
{
  if (ArgList==nullptr)
    initArgList();
  ArgList->push((void*)pFunction);
}
void ZThread::addThisThreadAsArgument()
{
  if (ArgList==nullptr)
    initArgList();
  ArgList->push((void*)this);
}
void ZThread::addArgumentInt(int pArgument)
{
  union {
    void*  Ptr;
    int    Int;
  } wArg;
  if (ArgList==nullptr)
    initArgList();
  wArg.Int=pArgument;
  ArgList->push(wArg.Ptr);
}

void ZThread::addArgumentList(int argc,char** argv)
{
  for (int wi=0;wi < argc;wi++)
    addArgument(argv[wi]);
}
void ZThread::copyArguments(zbs::ZArgList* pArgList)
{
  if (ArgList==nullptr)
      initArgList();
  for (long wi=0;wi<pArgList->count();wi++)
    ArgList->push(pArgList->Tab[wi]);
}

/**
     * @brief getArgument gets a void pointer from argument stack at position wi without destroying it.
     * if wi is outside argument stack, then it returns nullptr
     * @param wi argument index to get
     * @return a void pointer or nullptr if argument index is outside argument stack
     */

void* ZThread::getArgument(int wi)
{
  if (!ArgList)
    return nullptr;
  if (wi >= ArgList->count())
    return nullptr;
  return ArgList->Tab[wi];
}
/**
     * @brief getArgumentInt gets an int from argument stack at position wi without destroying it.
     * if wi is outside argument stack, then it returns nullptr
     * @param wi argument index to get
     * @return an int or nullptr if argument index is outside argument stack
     */
int ZThread::getArgumentInt(int wi)
{
  union {
    void*  Ptr;
    int    Int;
  } wArg;
  if (!ArgList)
    return 0;
  if (wi >= ArgList->count())
    return 0;
  wArg.Ptr=ArgList->Tab[wi];
  return wArg.Int;
}
/**
     * @brief popArgument gets the last argument as a void pointer from argument stack and removes it from stack
     * @return a void pointer or nullptr if argument stack has not been initialized
     */
void* ZThread::popArgument()
{
  if (!ArgList)
    return nullptr;
  return ArgList->popR();
}
/**
     * @brief popArgument gets the last argument as an int from argument stack and removes it from stack
     * @return an int or nullptr if argument stack has not been initialized
     */
int ZThread::popArgumentInt()
{
  union {
    void*  Ptr;
    int    Int;
  } wArg;
  if (!ArgList)
    return 0;
  wArg.Ptr=ArgList->popR();
  return wArg.Int;
}
/**
     * @brief popFrontArgument gets the first argument as a void pointer from argument stack and removes it from stack
     * @return a void pointer or nullptr if argument stack has not been initialized
     */
void* ZThread::popFrontArgument()
{
  if (!ArgList)
    return nullptr;
  return ArgList->popR_front();
}
/**
     * @brief popFrontArgumentInt gets the first argument as an int from argument stack and removes it from stack
     * @return an int or nullptr if argument stack has not been initialized
     */
int ZThread::popFrontArgumentInt()
{
  union {
    void*  Ptr;
    int    Int;
  } wArg;
  if (!ArgList)
    return 0;
  wArg.Ptr=ArgList->popR_front();
  return wArg.Int;
}
/**
     * @brief getArgumentsCount returns the current size of argument stack. Returns -1 if argument stack is null (has not been initialized).
     *
     * @return
     */
int ZThread::getArgumentsCount()
{
  if (!ArgList)
    return -1;
  return ArgList->count();
}


const char *
decode_ZTHS(ZThreadState_type pZTHS)
{
    switch (pZTHS)
    {
    case ZTHS_Nothing:
        return "ZTHS_Nothing";
    case ZTHS_Created:
        return "ZTHS_Created";
    case ZTHS_Active:
        return "ZTHS_Active";
    case ZTHS_CancelRequested:
        return "ZTHS_CancelRequested";
    case ZTHS_Cancelling:
        return "ZTHS_Cancelling";
    case ZTHS_SleepRequested:
        return "ZTHS_SleepRequested";
    case ZTHS_Sleeping:
        return "ZTHS_Sleeping";
    case ZTHS_DormantRequest:
        return "ZTHS_DormantRequest";
    case ZTHS_Dormant:
        return "ZTHS_Dormant";
    case ZTHS_ExecuteRequested:
        return "ZTHS_ExecuteRequested";
    case ZTHS_Executing:
        return "ZTHS_Executing";
    case ZTHS_Terminated:
        return "ZTHS_Terminated";

    default:
        return "Unknown Thread State";
    }
}// decodeZTHS
const char *
decode_ZTSGN(ZThreadSignal_type pZTSG)
{
    switch (pZTSG)
    {
    case ZTSGN_Nothing:
        return "ZTSGN_Nothing";
    case ZTSGN_CancelRequest:
        return "ZTSGN_CancelRequest";
    case ZTSGN_Sleep:
        return "ZTSGN_Sleep";
    case ZTSGN_SetDormant:
        return "ZTSGN_SetDormant";
    case ZTSGN_Execute:
        return "ZTSGN_Execute";
    case ZTSGN_Terminate:
        return "ZTSGN_Terminate";


    default:
        return "Unknown Thread Signal";
    }
}// decodeZTSGN

#include <ztoolset/zutfstrings.h>

int
encode_ZTSGN (char *pString)
{
 int   State = ZTSGN_Nothing;
    if (pString==nullptr)
                {
                    return State;
                }
    if (strlen(pString)==0)
                {
                    return State;
                }
utfdescString wBuf;
    wBuf.clear();
    wBuf = (const utf8_t*)pString;
    wBuf.toUpper();
    if (wBuf==(const utf8_t*)"ZTSGN_NOTHING")
            {
                return State;
            }
//============= ZThread signals===================================

    if (wBuf.contains((utf8_t*)"ZTSGN_CANCELREQUEST"))
                     State |= ZTSGN_CancelRequest;
    if (wBuf.contains((utf8_t*)"ZTSGN_SLEEP"))
                     State |= ZTSGN_Sleep;
    if (wBuf.contains((utf8_t*)"ZTSGN_SETDORMANT"))
                     State |= ZTSGN_SetDormant;
    if (wBuf.contains((utf8_t*)"ZTSGN_EXECUTE"))
                     State |= ZTSGN_Execute;
    if (wBuf.contains((utf8_t*)"ZTSGN_TERMINATE"))
                     State |= ZTSGN_Terminate;

}// encode_ZTSGN


#endif //ZTHREAD_CPP
