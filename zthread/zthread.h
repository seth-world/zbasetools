#ifndef ZTHREAD_H
#define ZTHREAD_H

#include <config/zconfig.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include <cstdarg>

#include <zthread/zmutex.h>
#include <zthread/zthreadexithandler.h>
#include <zthread/zarglist.h>

#include <functional>


void* _threadLoop(void* pArgList);
void* _threadNoLoop(void* pArgList);

/**
 * @brief The ZThread_type enum At thread creation time, indicates the type of thread to be created
 */
enum ZThread_type : int8_t {
    ZTH_Notype      =0x0 ,
    ZTH_Inherit     =0x01,        //!< Thread will inherit the whole parameters from its parent
    ZTH_Detached    =0x02,      //!< Thread is created with a state of detached
    ZTH_Joinable    =0x04,      //!< Thread is created joinable
};

enum ZThreadState_type : uint16_t {
    ZTHS_Nothing            = 0,
    ZTHS_Created            = 1,
    ZTHS_Active             = 2,
    ZTHS_CancelRequested    = 4,
    ZTHS_Cancelling         = 8,
    ZTHS_SleepRequested     = 0x0010,
    ZTHS_Sleeping           = 0x0020,
    ZTHS_DormantRequest     = 0x0040,
    ZTHS_Dormant            = 0x0080,
    ZTHS_ExecuteRequested   = 0x0100,
    ZTHS_Executing          = 0x0200,
    ZTHS_Terminated         = 0xFFFF
};

const char *
decode_ZTHS(ZThreadState_type pZTHS);

enum ZThreadSignal_type : uint16_t {
    ZTSGN_Nothing            = 0,
    ZTSGN_CancelRequest      = 4,
    ZTSGN_Sleep             = 0x10,
    ZTSGN_SetDormant        = 0x20,
    ZTSGN_Execute           = 0x30,
    ZTSGN_Terminate         = 0xFF,  // not used
};
const char *
decode_ZTSGN(ZThreadSignal_type pZTSG);

int
encode_ZTSGN (char *pString);

/*typedef     ZStatus (*ZTH_Functor)(zbs::ZArray<void*>*)  ; // any function to be launched within a ZThread
typedef     zbs::ZArray<void*>*      ZTH_Arglist;          // argument list for these functions
*/
typedef     ZStatus (*ZTH_Functor)(zbs::ZArgList*)  ;       // any function to be launched within a ZThread

struct ZTHEventCancel
{
ZThreadSignal_type Signal;      // must be   ZTSGN_CancelRequest
} ;
struct ZTHEventSleep
{
ZThreadSignal_type  Signal;         // must be   ZTSGN_Sleep
unsigned long       Milliseconds;   // duration to sleep for in milliseconds
} ;
struct ZTHEventExecute
{
  ZTHEventExecute() {clear();}
  void clear()
  {
    memset(this,0,sizeof(ZTHEventExecute));
  }
ZThreadSignal_type  Signal;     // must be   ZTSGN_Execute
ZTH_Functor         Function;   // Function to execute
zbs::ZArgList*      Arglist;    // argument list for the function to be executed
} ;
/**
 * @brief The ZTHEvent union Component of thread event queue : allow to manage threads behavior.
 */
union ZTHEvent {
public:
  ZTHEvent& _copyFrom(const ZTHEvent& pIn)
  {
    clear();
    if (pIn.Signal==ZTSGN_Sleep)
      {
      Sleep.Signal=ZTSGN_Sleep;
      Sleep.Milliseconds = pIn.Sleep.Milliseconds;
      return *this;
      }
    if (pIn.Signal==ZTSGN_Execute)
      {
      Execute.Signal = ZTSGN_Execute;
      Execute.Function = pIn.Execute.Function;
      Execute.Arglist = pIn.Execute.Arglist;
      return *this;
      }
    /* simple signal */
    Signal=pIn.Signal;
    return *this;
  }

  void clear()
  {
    size_t wSize=std::max(sizeof(ZTHEventSleep),sizeof (ZTHEventExecute));
//     memset(&Execute,0,sizeof (ZTHEventExecute));
     Execute.clear();
  }

  ZTHEvent() {clear();}
  ZTHEvent(const ZThreadSignal_type pSignal) {clear(); Signal=pSignal;}
  ZTHEvent(const ZTHEvent& pIn) {_copyFrom(pIn);}
  ZTHEvent(const ZTHEvent&& pIn) {_copyFrom(pIn);}

  ZTHEvent& operator = (const ZTHEvent& pIn) {return _copyFrom(pIn);}
  ZTHEvent& operator = (const ZTHEvent&& pIn) {return _copyFrom(pIn);}
ZThreadSignal_type  Signal;
struct ZTHEventSleep    Sleep;
struct ZTHEventExecute  Execute;
};

class ZThreadEventQueue : public zbs::ZQueue<ZTHEvent>
{
public:
  using zbs::ZQueue<ZTHEvent>::lock;
  using zbs::ZQueue<ZTHEvent>::unlock;

  ZThreadEventQueue();
  void addSignal(const ZThreadSignal_type pSignal)
  {
    ZTHEvent wEvent;
    wEvent.Signal = pSignal;
    push(wEvent);
  }
  void addSleep(const long pMillisec)
  {
    ZTHEvent wEvent;
    wEvent.Sleep.Signal = ZTSGN_Sleep;
    wEvent.Sleep.Milliseconds = pMillisec;
    push(wEvent);
  }
  void addExec(const ZTH_Functor pFunction,zbs::ZArgList*& pArgs)
  {
    ZTHEvent wEvent;
    wEvent.Execute.Signal = ZTSGN_Execute;
    wEvent.Execute.Function = pFunction;
    wEvent.Execute.Arglist = pArgs;
    push(wEvent);
  }
};


class ZThreadExitHandler;
#ifndef ZTHREAD_CPP
extern thread_local ZThreadExitHandler atThreadExit;
#endif

#ifdef __USE_POSIX_ZTHREAD__

#include <pthread.h>

#pragma message ("ZTHREAD-W-USINGPOSIX  Using Posix ZThreads : check that you compile option -pthread with AND link library option -lpthread. \n")

namespace zbs {

typedef int             ZThread_Priority ;
typedef pthread_t       ZThread_Native_Handle_type;
class ZThread_Id {
public:
    ZThread_Id() {clear();}
    ZThread_Id ( ZThread_Native_Handle_type &pId) {Id=pId;}

    void clear () {memset(&Id,0,sizeof(ZThread_Native_Handle_type));}
    ZThread_Native_Handle_type get_Native_Handle(void) {return Id;}

    ZThread_Native_Handle_type*    get_IdPtr(void)  {return &Id;}

//    bool operator == (const ZThread_Id &pId) {return (this->Id==pId.Id);}
    bool operator == (const ZThread_Id pId) {return (this->Id==pId.Id);}
    bool operator == (pid_t &pId) {return (this->Id==(ZThread_Native_Handle_type)pId);}

    bool isClear() {return Id==0 ;}


protected:
     ZThread_Native_Handle_type  Id;
};



class ZThread_Base
{
public:
    ZThread_Base(ZThread_type pType=ZTH_Inherit, int pPriority =-1,int pStackSize=-1);

/*
    ZThread_Base(const ZThread_Base& pIn) {_copyFrom(pIn);}

    ZThread_Base& _copyFrom(const ZThread_Base& pIn)
    {
      Father=pIn.Father;
      _created=pIn._created;
      _joinable=pIn._joinable;
      Thread_Attributes=pIn.Thread_Attributes;
      ThreadId=pIn.ThreadId;
      ProcessId=pIn.ProcessId;
      Priority=pIn.Priority;
      Type=pIn.Type;
      State=pIn.State;
      StackSize=pIn.StackSize;
    }
*/


//    ZThread_Base(void *(*pFunction)(void *), void* pArglist, ZThread_type pType=ZTH_Inherit, ZThread_Priority pPriority =-1, int pStackSize=-1);
    ~ZThread_Base(void) ;

    ZThread_Base (const ZThread_Base&) = delete;                  // cannot copy
    ZThread_Base& operator= (const ZThread_Base&) = delete;       // cannot assign

    friend void *::_threadLoop( void* pArglist);
    friend class ::ZThreadExitHandler ;

    void clear()
    {
      _created=false;
      _joinable=false;
      memset(&Thread_Attributes,0,sizeof(Thread_Attributes));
      ThreadId.clear();
      ProcessId=0;
      Priority=0;
      Type=ZTH_Notype;
      State=ZTHS_Nothing;
      StackSize=-1;
    }

    /** @brief setPriority sets the priority of a running thread and updates threads attributes */
    void setPriority(ZThread_Priority pPriority);

    /** @brief initPriority  initialize thread priority value in ZThread parameters (does nothing on thread itself) */
    ZStatus initPriority(ZThread_Priority pPriority);
    void setType(const int pType);
    void initStackSize (const int pStackSize);
    void setStackSize (const int pStackSize);

    ZStatus _start(void* (*pFunction)(void *), void *pArgList);


    void lock(void) {_Mtx.lock(); return;}
    void unlock(void){_Mtx.unlock(); return;}

    /**
     * @brief kill send SIGABRT signal to the thread. To be used exceptionnally and carefully. Consider cancelRequest() in place.
     */
    bool kill(void);

#ifdef __DEPRECATED__
    bool cancel(void);
    bool sendSignal(const int pSignal);
#endif // __DEPRECATED__


    void setActive(void )  { setState(ZTHS_Active); }

    void exitThread(int pStatus) ;

    ZStatus join();
    ZStatus detach();

    /**
     * @brief getId  For current running thread,
     * returns the system thread identification number (not pid).
     * @return
     */
    static ZThread_Native_Handle_type sysgetThreadId (void) {return((ZThread_Native_Handle_type)pthread_self() );}
    /**
     * @brief getId return the given thread id as stored when creating the thread
     * @return
     */
    inline ZThread_Id&  getId(void)     {return(ThreadId);}
    inline pid_t        getPid(void)    {return ProcessId;}

    inline ZThread_Native_Handle_type get_Native_Handle(void) {return (ThreadId.get_Native_Handle());}

    ZThread_Priority getPriority( void );

    ZThreadState_type getState (void) {return State;}  // getState must remains with no mutex locking
    ZThread_type getType (void) {return Type;}

    void setState (ZThreadState_type pState) ;

    bool isCreated () {return _created;}
    bool isJoinable() {return _joinable;}
private:
    bool init(const ZThread_type pType=ZTH_Inherit, const ZThread_Priority pPriority=-1, const int pStackSize=-1);

//-----------Thread data---------------------------------

protected:
    bool                _created = false;
    bool                _joinable = true;

    pthread_attr_t      Thread_Attributes;
    ZThread_Id          ThreadId;

    pid_t               ProcessId;
    ZThread_Priority    Priority;
    ZThread_type        Type= ZTH_Notype;
    ZThreadState_type   State= ZTHS_Nothing;
    ssize_t             StackSize=-1;

    ZMutex              _Mtx; /* not copied */

}; // ZThread
#ifdef __COMMENT__
#ifndef __ZTHREAD_ID_OPERATOR_EQUAL__
#define __ZTHREAD_ID_OPERATOR_EQUAL__
bool operator == (ZThread_Id& pR,ZThread_Id&pL) {return (pR.get_Native_Handle()==pL.get_Native_Handle());}
//bool operator == (const ZThread_Id& pR,const ZThread_Id&pL) {return (((ZThread_Id)pR).get_Native_Handle()==((ZThread_Id)pL).get_Native_Handle());}
#endif //__ZTHREAD_ID_OPERATOR_EQUAL__
#endif //__COMMENT__+
} // namespace zbs




#endif //====================  __USE_STD_ZTHREAD__ ===============================================
#ifdef __USE_STD_ZTHREAD__
#include <thread>

#pragma message ("ZTHREAD-W-USINGSTD  Using std::thread : check that you link glibc library. \n")

namespace zbs {


typedef std::thread::native_handle_type     ZThread_Native_Handle_type;
typedef int                                 ZThread_Priority ;

class ZThread_Id : public std::thread::id
{
public:
    ZThread_Id() {clear();}
    ZThread_Id ( ZThread_Native_Handle_type &pId) {clear(); Id=pId;}
//    ZThread_Id ( std::thread::id *pThreadId) {clear(); ThreadId=pThreadId;}
    ZThread_Id ( std::thread::id pThreadId) {clear(); memmove(this,&pThreadId,sizeof(std::thread::id));}
    void clear () {memset(this,0,sizeof(ZThread_Id));}
    ZThread_Native_Handle_type get_Native_Handle(void) {return Id;}

    ZThread_Native_Handle_type*    get_Id(void)  {return &Id;}

//    bool operator == (const ZThread_Id &pId) {return (this->Id==pId.Id);}
    bool operator == (const ZThread_Id pId) {return (this->Id==pId.Id);}
    bool operator == (pid_t &pId) {return (this->Id==(ZThread_Native_Handle_type)pId);}

    bool isClear() {return Id==0 ;}

protected:
     ZThread_Native_Handle_type  Id;
//     std::thread::id*            ThreadId;
};
typedef ZThread_Id                     ZThread_Id_type;


class ZThread_Base
{
public:
    /**
     * @brief ZThread CTOR : creates ZThread data structure without starting the thread
     * @param pType
     * @param pPriority
     * @param pStackSize
     */
    ZThread_Base(ZThread_type pType=ZTH_Inherit, int pPriority =-1,int pStackSize=-1);
    /**
     * @brief ZThread CTOR2 : creates ZThread data structure AND starts the thread with pFunction
     * @param pFunction function to start the thread with. It must have the formal definition of ZTH_Functor.
     * @param pType     Characteristics of the thread (joinable or detached)
     * @param pPriority
     * @param pStackSize
     */
    ZThread_Base(void* (*pFunction)(void*),void* pArglist,ZThread_type pType=ZTH_Inherit, ZThread_Priority pPriority =-1,int pStackSize=-1);

    ~ZThread_Base(void) ;
    ZThread_Base (const ZThread_Base&) = delete;
    ZThread_Base& operator= (const ZThread_Base&) = delete;

    friend void *::_threadLoop( void* pArglist);
    friend class ::ZThreadExitHandler ;

    void initPriority(ZThread_Priority pPriority);
    void setPriority(ZThread_Priority pPriority); // does nothing with pure std::thread (windows is ok)

    void setType(const int pType);

    void initStackSize(const int pStackSize);
    void setStackSize (const int pStackSize);// does nothing with std::thread and windows

    bool init(const ZThread_type pType=ZTH_Inherit, const ZThread_Priority pPriority=-1, const int pStackSize=-1);

    bool _start(void* (*pFunction)(void *), void *pArglist);

    void lock(void) {_Mtx.lock(); return;}
    void unlock(void){_Mtx.unlock(); return;}
#ifdef __DEPRECATED__
    bool cancel(void);
    bool sendSignal(const int pSignal);
#endif // __DEPRECATED__
    bool kill(void);


    void exitThread(int pStatus) ;

    bool join();
    bool detach();

    /**
     * @brief getId  returns the system thread identification number (not pid) in calling system routine
     * @return
     */
    static ZThread_Id_type sysgetThreadId (void)
        {
        ZThread_Id wId (std::this_thread::get_id());
        return wId;
//        return (ZThread_Id_type) std::this_thread::get_id();
        }
//        return((ZThread_Id_type)std::this_thread::native_handle() ); }
    /**
     * @brief getId return the given thread id as stored when creating the thread
     * @return
     */
    inline ZThread_Id_type getId(void) {return StdThread->get_id();}
    inline pid_t getPid(void)   {return ProcessId;}
    inline ZThread_Native_Handle_type getNativeHandle(void) {return StdThread->native_handle();}

    ZThread_Priority getPriority( void );
    bool isJoinable (void)  {if (StdThread!=nullptr) return StdThread->joinable(); else return false;}
    
    ZThreadState_type getState (void) {return State;}  // getState must remains with no mutex locking
    int getType (void) {return Type;}


//protected :
    void setState (ZThreadState_type pState) ;

//-----------Thread data---------------------------------
public:
  bool          Created = false;
private:
    std::thread*        StdThread=nullptr;         // std::thread to be created using start() function

protected:
    ZThread_Id_type     ThreadId;
    pid_t               ProcessId;
    ZThread_Priority    Priority;
    int                 Type=       ZTH_Notype;
    ZThreadState_type   State=      ZTHS_Nothing;
    ssize_t             StackSize=  -1;

    ZMutex              _Mtx;



}; // ZThread_Base

} // namespace zbs

#endif // using __USE_STD_ZTHREAD__

#ifdef __USE_WIN_ZTHREAD__
#include <zthread/zthread_win.h>
#endif// __USE_WIN_ZTHREAD__

namespace zbs {


class ZThread : public ZThread_Base
{
public:
    typedef     ZThread_Base _Base;
    friend void *::_threadLoop( void* pArglist);
    friend void *::_threadNoLoop( void* pArglist);

    friend class ZThreadExitHandler ;

    ZThread() = delete;
    ZThread(ZThread* pFather);
    ~ZThread() ;

    ZThread(ZThread&) = delete;
    ZThread& operator = (ZThread&)=delete;

    ZThread* getNewThread() { return  new ZThread(this);}

    /**
     * @brief startLoop  starts a thread loop with function pFunction.
     * Arguments are those collected from main() function.
     * Arguments are copied to internal argument stack.
     *
     *  ZArgList
     * ----------
     *  pFunction will receive a pointer to zbs::ZArgList object that will contain :
     * rank   content
     *  0     pFunction address
     *  1     pointer to currrent ZThread object
     *  2...  Arguments passed to routine
     *        in the order mentionned when startLoop, startLoopArg or startLoopVariadic has been called
     *
     *
     * @param pFunction function to start the thread with.
     * @param argc
     * @param argv
     */
    void startLoop(ZTH_Functor pFunction,int argc,char** argv);
    void startLoopArg(ZTH_Functor pFunction, ZArgList* pArgList);
    void startLoopVariadic(ZTH_Functor pFunction,...); // last argument must be nullptr

    /**
     * @brief startNoLoopArg startNoLoop starts a thread with no thread loop.
     *  Argument stack is a dedicated ZArgList pointer.
     * Arguments are copied to internal argument stack.
     * @param pFunction
     * @param pArgList
     */
    void startNoLoopArg(ZTH_Functor pFunction,ZArgList* pArgList);
    /**
     * @brief startNoLoop starts a thread with no loop.
     * Arguments are those collected from main() function.
     * @param pFunction a pointer of type ZTH_Functor to the routine to execute within created thread
     */
    void startNoLoop(ZTH_Functor pFunction,int argc,char** argv);
    /**
     * @brief startNoLoop starts a thread with no loop.
     * Arguments (if any) must have been set within MainFunctionArguments using appropriate method : i.e. ZThread::addArgument()
     * @param pFunction a pointer of type ZTH_Functor to the routine to execute within created thread
     */
    void startNoLoop(ZTH_Functor pFunction);
    /**
     * @brief ZThread::startNoLoopVariadic
     * calling conventions :
     *  - functors are passed as it is (it is already a pointer)
     *  - all other variable data types must be passed using a pointer on them
     *  - no rvalue allowed
     *
     *  IMPORTANT: Last argument MUST be nullptr.
     *  -----------------------------------------
     *
     *  -> There could not be "omitted" arguments using nullptr value.
     *
     * @param pFunction user Function to launch within the created thread. It must be a ZTH_Functor :
     *  - a pointer to a function
     *  - with a unique argument as a pointer to a ZArgList that will contain user's arguments
     *  each argument must be unstacked in reverse order of the call and casted to a pointer onto the appropriate data type
     *  - returning a ZStatus
     *
     *
     * Arguments : ZArgList.
     *
     *  index     content
     *  0         argument rank 0
     *  1         argument rank 1
     *  ...
     *  n         pointer to function
     *  last      pointer to ZThread owning the thread
     *
     *
     */
    bool startNoLoopVariadic(ZTH_Functor pFunction,...); // last argument must be nullptr

//    void registerExitFunction(ZTH_Functor pExitFunction, ZArgList *pExitArguments);
    void registerExitFunction(ZTH_Functor pExitFunction,...);
//    void registerExitFunction(threadRoutine &pExitFunction);

    void cancelRequest(void );
    void dormantRequest(void ) ;
    void sleepRequest(const unsigned long pMilliseconds )  ;
    void executeFunctionRequest(ZTH_Functor pFunction, ZArgList *pArglist);
    void wakeup(void)       {_MtxCond.signal();}

    void setDormant(void) {State=ZTHS_Dormant; _MtxCond.wait();State=ZTHS_Active;}
    void setSleep(unsigned long pMilliseconds) {State=ZTHS_Sleeping; zsleep(pMilliseconds); State=ZTHS_Active;}

    bool    isCurrent(void) {return (State>=ZTHS_Active);}
    bool    isActive(void)  {return (State==ZTHS_Active);}
    bool    isCancelling(void) {return (State==ZTHS_Cancelling);}
    bool    isDormant() {return (State==ZTHS_Dormant);}

//    void exit(void* pReturn=nullptr);

    ZMutexCondition* getMutexCondition(void) {return &_MtxCond;}

    /** @brief waitForEvent puts the thread in dormant mode until one of addSignalEvent, addCancelEvent ,.. routines are invoked */
    ZStatus waitForEvent()
      {
      setDormant();
      return processEventQueue();
      }
    /** @brief addSleepEvent set the current thread to sleep pmsec milliseconds */
    void addSleepEvent(const long pmsec)
      {
      EventQueue.addSleep(pmsec);
      wakeup();
      }

    void addSignalEvent(const ZThreadSignal_type pSignal)
        {
        EventQueue.addSignal(pSignal);
        wakeup();
        }
    /** @brief addCancelEvent signal to thread a cancel event request, then wake up the thread */
    void addCancelEvent()
        {
          EventQueue.addSignal(ZTSGN_CancelRequest);
          wakeup();
        }
    /** @brief addTerminateEvent signal to thread a terminate event request, then wake up the thread */
    void addTerminateEvent()
        {
          EventQueue.addSignal(ZTSGN_Terminate);
          wakeup();
        }
    void addExecEvent(const ZTH_Functor pFunction,zbs::ZArgList*& pArgs)
      {
      EventQueue.addExec(pFunction,pArgs);
      wakeup();
      }

//    ZQueue<ZTHEvent>    EventQueue;  //event queue

    ZStatus processEventQueue(void) ;

    /** @brief setIdentity user may describe freely the thread  max 149 char */
    void setIdentity(const char*pString) { strncpy(Identity,pString,149); return;}
    void addIdentity(const char*pString) {strncat(Identity,pString,149); return;}
    const char* getIdentity(void) {return Identity;}

    ZThread_Native_Handle_type get_Native_Handle(void) {return ThreadId.get_Native_Handle();}

    /*
     * ============thread arguments management ================
    */
    void initArgList() {ArgList=new zbs::ZArgList; ArgList->clear();}
    void addArgument(void* pArgument);
    void addArgument(ZTH_Functor pFunction);
    void addThisThreadAsArgument();
    void addArgumentInt(int pArgument);

    void addArgumentList(int argc,char** argv);
    void copyArguments(zbs::ZArgList* pArgList);

    /**
     * @brief getArgument gets a void pointer from argument stack at position wi without destroying it.
     * if wi is outside argument stack, then it returns nullptr
     * @param wi argument index to get
     * @return a void pointer or nullptr if argument index is outside argument stack
     */

    void* getArgument(int wi);
    /**
     * @brief getArgumentInt gets an int from argument stack at position wi without destroying it.
     * if wi is outside argument stack, then it returns nullptr
     * @param wi argument index to get
     * @return an int or nullptr if argument index is outside argument stack
     */
    int getArgumentInt(int wi);
    /**
     * @brief popArgument gets the last argument as a void pointer from argument stack and removes it from stack
     * @return a void pointer or nullptr if argument stack has not been initialized
     */
    void* popArgument();
    /**
     * @brief popArgument gets the last argument as an int from argument stack and removes it from stack
     * @return an int or nullptr if argument stack has not been initialized
     */
    int popArgumentInt();
    /**
     * @brief popFrontArgument gets the first argument as a void pointer from argument stack and removes it from stack
     * @return a void pointer or nullptr if argument stack has not been initialized
     */
    void* popFrontArgument();
    /**
     * @brief popFrontArgumentInt gets the first argument as an int from argument stack and removes it from stack
     * @return an int or nullptr if argument stack has not been initialized
     */
    int popFrontArgumentInt();
    /**
     * @brief getArgumentsCount returns the current size of argument stack. Returns -1 if argument stack is null (has not been initialized).
     *
     * @return
     */
    int getArgumentsCount();

protected:
    ZMutexCondition     _MtxCond;
    char Identity[150]; // string max 149 : free comment describing thread
public:
  ZThread*                  Father=nullptr;
  ZQueue<threadRoutine*>    ExitRoutinesQueue;
  ZThreadEventQueue         EventQueue;
//    ZThreadExitHandler  *ExitHandler=nullptr;

//private:
  ZArgList*       ArgList=nullptr; // Only to be used with variadic start;
  ZStatus         ThreadStatus=ZS_NOTHING;
};




} // namespace zbs


/**
 * @brief getSysThreadId  get current thread  ZThread_Id object
 * @return
 */
zbs::ZThread_Id getSysThreadId(void);

zbs::ZThread* getThisThread();

// functions

const char* decode_Signal (int pSignal);


#endif // ZTHREAD_H
