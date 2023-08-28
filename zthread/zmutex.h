#ifndef ZMUTEX_H
#define ZMUTEX_H
#include <config/zconfig.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <cstdarg>


#include <ztoolset/zerror_min.h>

#include <ztoolset/ztime.h>



#ifndef __USE_ZTHREAD__

#error "__USE_ZTHREAD__  preprocessor parameter must have been defined ( within zconfig.h file )"

#endif // __USE_ZTHREAD__

#ifdef __USE_POSIX_ZTHREAD__
//#include <zthread/zmutex_pthread.h>

#pragma message "ZTHREAD-W-USINGPOSIX  Using STD Mutexes : check that you link with compiler option -pthread and library option -lpthread . \n"

#include <pthread.h>

namespace zbs {

/**
  @brief Mutex and derived classes lock error codes<br>
The pthread_mutex_lock() and pthread_mutex_trylock() functions shall fail if:<br>
EINVAL
    The mutex was created with the protocol attribute having the value PTHREAD_PRIO_PROTECT and the calling thread's priority is higher than the mutex's current priority ceiling.<br>
<br>
The pthread_mutex_trylock() function shall fail if:<br>
EBUSY
    The mutex could not be acquired because it was already locked.<br>
 <br>
The pthread_mutex_lock(), pthread_mutex_trylock(), and pthread_mutex_unlock() functions may fail if:<br>
EINVAL
    The value specified by mutex does not refer to an initialized mutex object.<br>
EAGAIN
    The mutex could not be acquired because the maximum number of recursive locks for mutex has been exceeded.<br>
<br>
The pthread_mutex_lock() function may fail if:<br>
EDEADLK
    The current thread already owns the mutex.<br>
<br>
The pthread_mutex_unlock() function may fail if:<br>
EPERM
    The current thread does not own the mutex.<br>
<br>
These functions shall not return an error code of [EINTR].

*/



//----------------Mutexes----------------------------------------


typedef pthread_mutex_t    ZMutex_Id_type;

class ZMutex
{
public:
    ZMutex(void);
    ZMutex(ZMutex&)=delete ;                    // cannot copy ZMutex anyway
    ZMutex& operator=(const ZMutex&) = delete;  // no copy

    ~ZMutex();
    /* @brief lock locks current mutex. If mutex is already locked, returns EPERM as status without doing anything.
     */
    int lock();
    /* @brief lockBlock locks current mutex. If mutex is already locked, waits until mutex is unlocked by its owner */
    void lockBlock();
    bool tryLock(void);
    int timedLock(ZTime pTimeout);
    /**
     * @brief unlock unlocks current mutex.<br>
     * if mutex is not locked by current process:<br>
     *  EPERM is returned and an error message is issued to stderr.<br>
     *  routine returns without any system mutex unlocking operation.
     * @return 0 in case of successfull operation. errno value if not.
     */
    int unlock();
    /**
     * @brief unlockBesttry unlocks mutex if it is locked, does nothing if not locked without error message.
     * @return 0 in case of successfull operation. errno value if not.
     */
    int unlockBesttry();

    ZMutex_Id_type& getMutexid(void) {return Mutex_id;}

    bool isLocked(void) {return State_locked;}

protected:

pthread_mutexattr_t     MutexAttributes;

ZMutex_Id_type          Mutex_id;
bool                    State_locked = false;
};



class ZMtxRecursive
{
public:
    ZMtxRecursive();
    ~ZMtxRecursive();
    ZMtxRecursive(ZMtxRecursive&)=delete;                   // not copiable
    ZMtxRecursive& operator= (ZMtxRecursive&)=delete;       // not assignable

    int lock();
    bool trylock();
    /**
     * @brief trylock_for tries to lock the mutex for a maximum period of pTimeout. Returns true if successful or false if not.
     */
    bool trylock_for(const ZTime pTimeout);
    int unlock();
    int unlockAll();
    bool isLocked(void) {return !(OwnedLocks==0);}
    ZMutex_Id_type* getMutexid(void) {return &Mutex_id;}
private:
pthread_mutexattr_t     MutexAttributes;
ZMutex_Id_type          Mutex_id;
int                     OwnedLocks;
};// ZMxRecursive



//---------------Mutex Conditions------------------

typedef pthread_cond_t ZMutexCondition_type;
class ZMutexCondition : public ZMutex  // a Thread condition is always associated with a mutex
{
public:
//    ZMutexCondition(const bool pGlobal=false)// PTHREAD_PROCESS_PRIVATE is the default
    ZMutexCondition(void);// PTHREAD_PROCESS_PRIVATE is the default

    ZMutexCondition(ZMutexCondition&)=delete;                   // not copiable
    ZMutexCondition& operator= (ZMutexCondition&)=delete;       // not assignable

    ~ZMutexCondition(void) ;

//    void init(const bool pGlobal=false);

    void signal(void);
    void broadcast(void);
    void wait(void) ;
    int timedWait(ZTime pTimeout);

private:
        pthread_cond_t Condition;
        pthread_condattr_t Condition_Attributes; // unused : only for PTHREAD_PROCESS_SHARED that is NOT used here
//        ZMutex* Mtx=nullptr;
        ZMutex Mtx;
};
//---------------Timed Mutex-------------
} // namespace zbs

#endif //====================== if not __USE_POSIX_ZTHREAD__   ============================================

//#if ((__USE_ZTHREAD__ & __ZTHREAD_STD__)==__ZTHREAD_STD__)
#ifdef  __ZTHREAD_STD__

#pragma message "ZTHREAD-W-USINGSTD Using STD Mutexes : check that you link with glibc to get benefit of std:: classes and routines. \n"

#include <mutex>
#include <chrono>
#include <condition_variable>

//typedef std::timed_mutex::native_handle_type      ZMutex_Id_type;
typedef unsigned long      ZMutex_Id_type;

namespace zbs {


//----------------Mutexes----------------------------------------



class ZMutex :public std::timed_mutex//: public std::timed_mutex
{
    typedef std::timed_mutex _Base ;
public:
    ZMutex(void) ;

    ZMutex(ZMutex&)=delete;                     // no duplicate
    ZMutex& operator=(const ZMutex&) = delete;  // no copy
    ~ZMutex();

    void lock() ;
    bool tryLock(void);
    /**
     * @brief trylock_for tries to lock the mutex for a maximum period of pTimeout. Returns true if successful or false if not.
     */
    bool tryLock_for(const ZTime pTimeout);
    void unlock();
    ZMutex_Id_type getMutexid(void) {return Mutex_id;}
//    ZMutex_Id_type getMutexid(void) {return native_handle();}

    bool isLocked(void) {return State_locked;}

protected:
ZMutex_Id_type          Mutex_id;
bool                    State_locked = false;

}; // ZMutex



class ZMtxRecursive : private std::recursive_timed_mutex
{
    typedef std::recursive_timed_mutex _Base;
public:
    ZMtxRecursive();
    ZMtxRecursive (ZMtxRecursive&) = delete;                // not copiable
    ZMtxRecursive& operator= (ZMtxRecursive&)=delete;       // not assignable

    ~ZMtxRecursive() ;
    int lock();
    bool trylock();
    /**
     * @brief trylock_for tries to lock the mutex for a maximum period of pTimeout. Returns true if successful or false if not.
     */
    bool tryLock_for(ZTime &pTimeout);
    int unlock();
    int unlockAll();
    /**
     * @brief isLocked returns the number of locks owned or zero (false) if no lock are owned.
     */
    int isLocked(void) {return OwnedLocks;}
    ZMutex_Id_type getMutexid(void) {return Mutex_id;}
private:
ZMutex_Id_type          Mutex_id;
int                     OwnedLocks = 0;
};


//---------------Thread Conditions------------------

typedef std::condition_variable::native_handle_type  ZMutexCondition_type;
class ZMutexCondition :  protected std::condition_variable  // a Thread condition is always associated with a mutex
{
typedef std::condition_variable _Base;
public:
    ZMutexCondition(void)// PTHREAD_PROCESS_PRIVATE is the default : local to current process and its descendants
    {
    }

    ZMutexCondition(ZMutexCondition&)=delete;                   // not copiable
    ZMutexCondition& operator= (ZMutexCondition&)=delete;       // not assignable

    ~ZMutexCondition(void) ;

    void lock()  {_Mtx.lock();}
    void unlock() {_Mtx.unlock();}

    void signal(void);
    void broadcast(void);
    void wait(void) ;
    template <typename TimeUnit>
    void wait_Timeout(TimeUnit pTimeValue);
    template <typename TimeUnit>
    void wait_for (TimeUnit pTimeValue);

private:
    std::mutex _Mtx;
};



} // namespace zbs



#endif // (__USE_ZTHREAD__ & __USE_POSIX_ZTHREAD__)!= __USE_POSIX_ZTHREAD__


utf8VaryingString decode_MutexError(const char* pModule,int pStatus) ;

#endif // ZMUTEX_H
