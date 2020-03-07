#ifndef ZMUTEX_CPP
#define ZMUTEX_CPP

//#include <zconfig.h>

#include <zthread/zmutex.h>
#include <ztoolset/zerror_min.h>

#include <time.h> // for timeout computation

using namespace zbs;

#ifdef __USE_POSIX_ZTHREAD__

//#include <zthread/zmutex_pthread.h>
//----------------Mutexes----------------------------------------

//------------Simple mutex--------------------------------------

#define __ZMUTEXVERBOSE__  ZVerbose & ZVB_Mutex

ZMutex::ZMutex(void)
{

    int   wRet=pthread_mutexattr_init(&MutexAttributes);
    if (wRet!=0)
        {
         fprintf(stderr,"ZMutex-F-MUTEXATTRINIT Fatal error initializing attributes for ZMutex : %s \n",
                 strerror(wRet));
         abort();
        }
    wRet=pthread_mutexattr_settype( &MutexAttributes,PTHREAD_MUTEX_ERRORCHECK) ;
    if (wRet!=0)
        {
         fprintf(stderr,"ZMutex-F-MUTEXSETTYPE Fatal error changing type of ZMutex : %s \n",
                 strerror(wRet));
         abort();
        }
    wRet=pthread_mutex_init(&Mutex_id,&MutexAttributes);
    if (wRet==0)
                    return;
     fprintf(stderr,"ZMutex-F-ERRINITMTX Fatal error initializing ZMutex : %s \n",
             strerror(wRet));
     abort();
}

ZMutex::~ZMutex() {     pthread_mutex_destroy(&Mutex_id);}

int ZMutex::lock()
{
    if (State_locked)
        {
        fprintf(stderr,"ZMutex-%s>> Mutex has a state of locked while trying to lock it\n",_GET_FUNCTION_NAME_);
        return  EPERM;
        }
    if (__ZMUTEXVERBOSE__)
        fprintf(stdout,"ZMutex-I-LOCK locking ZMutex  \n");
int wRet=pthread_mutex_lock(&Mutex_id);

    if (wRet==0)
        {
        State_locked = true;
        return wRet;
        }

     fprintf(stderr,"ZMutex-E-Lock>> Error locking ZMutex : %s \n",
             strerror(wRet));
     return wRet;
}// lock

void ZMutex::lockBlock()
{
    if (__ZMUTEXVERBOSE__)
        fprintf(stdout,"ZMutex-I-LOCK locking ZMutex  \n");
int wRet=pthread_mutex_lock(&Mutex_id);

    if (wRet==0)
        {
        State_locked = true;
        return ;
        }

     fprintf(stderr,"ZMutex-E-Lock>> Error locking ZMutex : %s \n",
             strerror(wRet));
     return ;
}// lockBlock

bool
ZMutex::tryLock()
{
    if (State_locked)
        {
        fprintf(stderr,"ZMutex-%s>> Mutex has a state of locked while trying to lock it\n",_GET_FUNCTION_NAME_);
        return  EPERM;
        }
    if (__ZMUTEXVERBOSE__)
        fprintf(stdout,"ZMutex-I-LOCK trying to lock ZMutex  \n");


    int wRet=pthread_mutex_trylock(&Mutex_id);

    if (wRet==0)
        {
        State_locked = true;
        }
        else
    {
        if (wRet==EBUSY)
            fprintf(stderr,"ZMutex-tryLock>> EBUSY returned while trying to lock it\n");
    }
    return (wRet==0);

} // tryLock()

int
ZMutex::timedLock(ZTime pTimeout)
{
ZTime wTimeoutlimit;
    if (State_locked)
        {
        fprintf(stderr,"ZMutex-%s>> Mutex has a state of locked while trying to lock it\n",_GET_FUNCTION_NAME_);
        return  EPERM;
        }
    if (__ZMUTEXVERBOSE__)
        fprintf(stdout,"ZMutex-I-LOCK trying to lock ZMutex  \n");

    wTimeoutlimit.absoluteFromDelay(pTimeout);

    int wRet=pthread_mutex_timedlock(&Mutex_id,&wTimeoutlimit);

    if (wRet!=0)
        {
        if (wRet==ETIMEDOUT)
                    return wRet;
        if (wRet==EBUSY)
            fprintf(stderr,"ZMutex-timedLock>> EBUSY returned while trying to lock ZMutex\n");
        fprintf(stderr,"ZMutex-F-timedLock Fatal error waiting for lock (timedlock) : %s \n",
                strerror(wRet));
        abort();
        }
    State_locked = true;
    return (wRet);

} // timedLock()

int
ZMutex::unlockBesttry()
{
    if (!State_locked)
            {
            return 0;
            }
    if (__ZMUTEXVERBOSE__)
        fprintf(stdout,"ZMutex-I-LOCK unlocking ZMutex  \n");
int wRet=pthread_mutex_unlock(&Mutex_id);
    if (wRet==0)
            {
            State_locked = false;
            return wRet;
            }
     fprintf(stderr,"ZMutex-E-UNLOCK Error unlocking ZMutex : %s \n",
             strerror(wRet));
     return wRet;
}// unlock

int
ZMutex::unlock()
{
    if (!State_locked)
            {
            fprintf(stderr,"ZMutex-unlock>> Mutex has a state of unlocked while unlocking it\n");
            return EPERM;
            }
    if (__ZMUTEXVERBOSE__)
        fprintf(stdout,"ZMutex-I-LOCK unlocking ZMutex  \n");
int wRet=pthread_mutex_unlock(&Mutex_id);
    if (wRet==0)
            {
            State_locked = false;
            return wRet;
            }
     fprintf(stderr,"ZMutex-E-UNLOCK Error unlocking ZMutex : %s \n",
             strerror(wRet));
     return wRet;
}// unlock

//------------Timed mutex-------------------------------------




//-------------Recursive mutex---------------------------------

ZMtxRecursive::ZMtxRecursive(void)
{
    OwnedLocks=0;
    int   wRet=pthread_mutexattr_init(&MutexAttributes);
    if (wRet!=0)
        {
         fprintf(stderr,"ZMxRecursive-F-MUTEXATTRINIT Fatal error initializing attributes for ZMutex : %s \n",
                 strerror(wRet));
         abort();
        }
    wRet=pthread_mutexattr_settype( &MutexAttributes,PTHREAD_MUTEX_ERRORCHECK_NP|PTHREAD_MUTEX_RECURSIVE_NP) ;
    if (wRet!=0)
        {
         fprintf(stderr,"ZMxRecursive-F-MUTEXSETTYPE Fatal error changing type of ZMutex : %s \n",
                 strerror(wRet));
         abort();
        }
    wRet=pthread_mutex_init(&Mutex_id,&MutexAttributes);
    if (wRet==0)
                    return;
     fprintf(stderr,"ZMxRecursive-F-ERRINITMTX Fatal error initializing ZMutex : %s \n",
             strerror(wRet));
     abort();
}
ZMtxRecursive::~ZMtxRecursive()
{
    pthread_mutex_destroy(&Mutex_id);
}

int
ZMtxRecursive::lock()
{
    if (OwnedLocks)
        {
        fprintf(stderr,"ZMutex-%s>> Recursive ZMutex has a state of lock already owned by current process while trying to lock it\n",_GET_FUNCTION_NAME_);
        return  EPERM;
        }
    if (__ZMUTEXVERBOSE__)
            {
           fprintf(stdout,"ZMutex-I-LOCK locking recursive ZMutex  \n");
            }
int wRet=pthread_mutex_lock(&Mutex_id);
    if (wRet==0)
        {
        OwnedLocks++;
        return wRet;
        }
    fprintf(stderr,"ZMutex-E-LOCK Error locking ZMutex : %s \n",
         strerror(wRet));
     return wRet;
}
bool
ZMtxRecursive::trylock()
{
bool wRet=false;
    if (OwnedLocks)
        {
        fprintf(stderr,"ZMutex-%s>> Recursive ZMutex has a state of lock already owned by current process while trying to lock it\n",_GET_FUNCTION_NAME_);
        return  EPERM;
        }
    if (__ZMUTEXVERBOSE__)
        {
       fprintf(stdout,"ZMutex-I-TRYLOCK trying to lock recursive ZMutex  \n");
        }
    wRet=!pthread_mutex_trylock(&Mutex_id);
    if (wRet)
            {
            OwnedLocks--;
            }
    return wRet;
}//trylock
int
ZMtxRecursive::unlock()
{
    if (OwnedLocks)
        {
        fprintf(stderr,"ZMutex-%s>> Recursive ZMutex has a state of lock already owned by current process while trying to lock it\n",_GET_FUNCTION_NAME_);
        return  EPERM;
        }
    if (__ZMUTEXVERBOSE__)
        {
        fprintf(stdout,"ZMutex-I-UNLOCK unlocking recursive ZMutex  \n");
        }
    if (OwnedLocks==0)
        {
        fprintf(stderr,"ZMutex-E-Unlock Error unlocking Recursive ZMutex : No lock is owned on this mutex. \n");
        return -1;
        }
int wRet=pthread_mutex_unlock(&Mutex_id);
    if (wRet==0)
            {
            OwnedLocks--;
            return wRet;
            }
     fprintf(stderr,"ZMutex-E-UNLOCK Error unlocking ZMutex : %s \n",
             strerror(wRet));
     return wRet;
}// unlock



/**
 * @brief ZMtxRecursive::unlockAll unlock all locks that the current process has to the current recursive lock
 */
int
ZMtxRecursive::unlockAll(void)
{
int wRet=0;
    while ((OwnedLocks>0)&&(!(wRet=unlock())));

    return wRet;
}

//-------------Condition mutex---------------------------------


ZMutexCondition::~ZMutexCondition ()
{
    pthread_cond_destroy(&Condition);
}

/**
 * @brief ZMutexCondition::init initialize conditional mutex parameters.
 * Deprecated:If pGlobal is set to true, then mutex is global to any process.
 * Conditional mutex is local to current process.
 * If set to false (default), then conditional mutex is local to current process and its descendants.
 * @param pGlobal
 */

ZMutexCondition::ZMutexCondition(void)
{
//    Mtx= new ZMutex ;

    int wRet = pthread_condattr_init(&Condition_Attributes) ;
    if (wRet!=0)
            {
            fprintf(stderr,"ZMutexCondition::init-F-CONDATTRINIT Fatal error while initializing condition mutex attributes : %s \n",
                    strerror(wRet));
            abort();
            }
/*    if (pGlobal)
        {
        wRet=pthread_condattr_setpshared(&Condition_Attributes,PTHREAD_PROCESS_SHARED);
        if (wRet!=0)
            {
            fprintf(stderr,"ZMutexCondition::init-F-CONDATTRINIT Fatal error while setting condition mutex attributes to PTHREAD_PROCESS_SHARED : %s \n",
                    strerror(wRet));
            abort();
            }
        }*/
    pthread_cond_init(&Condition,&Condition_Attributes);
    return;
}// CTOR




/**
 * @brief ZMutexCondition::signal signals a condition for the mutex to local threads
 */
void
ZMutexCondition::signal(void)
{
    int wRet= pthread_cond_signal(&Condition);
    if (wRet==0)
                    return;
     fprintf(stderr,"ZMutexCondition-F-SIGNAL Fatal error while sending signal for conditional mutex : %s \n",
             strerror(wRet));
     abort();
 }// signal

/**
 * @brief ZMutexCondition::broadcast broadcasts a condition for the mutex to processes
 */
void
ZMutexCondition::broadcast(void)
{
    int wRet= pthread_cond_broadcast(&Condition);
    if (wRet==0)
                    return;
     fprintf(stderr,"ZMutexCondition-F-BROADCAST Fatal error while sending broadcast for conditional mutex : %s \n",
             strerror(wRet));
     abort();
 }// broadcast

/**
 * @brief ZMutexCondition::wait hibernates the thread until ZMutexCondition is signaled or broadcasted.
 * base mutex is locked during waiting time and unlocked when thread is awakened.
 */
void
ZMutexCondition::wait(void)
{
    lock();
    int wRet=pthread_cond_wait(&Condition,&getMutexid());
    if (wRet!=0)
    {
    fprintf(stderr,"ZMutexCondition-F-WAITCOND Fatal error waiting for condition : %s \n",
            strerror(wRet));
    unlock();
    abort();
    }
    unlock();
    return;
}// wait

int
ZMutexCondition::timedWait(ZTime pTimeout)
{
ZTime wTimeoutlimit;
    lock();
    wTimeoutlimit.absoluteFromDelay(pTimeout);
    int wRet=pthread_cond_timedwait(&Condition,&getMutexid(),&wTimeoutlimit);
    if (wRet!=0)
        {
        if (wRet==ETIMEDOUT)
                    return wRet;
        if (wRet==EBUSY)
            fprintf(stderr,"ZMutex-timedLock>> EBUSY returned while trying to lock ZMutex\n");
        fprintf(stderr,"ZMutexCondition-F-timedWait Fatal error waiting for condition : %s \n",
                strerror(wRet));
        unlock();
        abort();
        }
    State_locked = true;
    return (wRet);
}// timedWait
#else // ================not  __USE_POSIX_ZTHREAD__  -> __USE_STD_ZTHREAD__===================================

#include <mutex>


//-------Hereafter using std::mutex suite--------------------

ZMutex::ZMutex(void)
{
    Mutex_id= (unsigned long)native_handle();
    State_locked = false;
}// ZMutex CTor

ZMutex::~ZMutex(void)
{
    if (isLocked())
                unlock();
}// DTor

void
ZMutex::lock()
{
    if (State_locked)
        fprintf(stderr,"ZMutex-lock>> Mutex has a state of locked while trying to lock it\n");
    if (__ZMUTEXVERBOSE__)
        fprintf(stdout,"ZMutex-I-LOCK locking ZMutex  \n");
    try {
        _Base::lock();
    }
    catch (int wErr)
    {
        if (wErr==EBUSY)
                fprintf(stderr,"ZMutex-lock>> EBUSY returned while trying to lock it\n");

        fprintf(stderr,"ZMutex-F-lock Fatal error locking ZMutex : %s \n",
                strerror(wErr));
        abort();
    }

    State_locked = true;
    return;
} // lock()

bool
ZMutex::tryLock()
{
    if (State_locked)
        fprintf(stderr,"ZMutex-trylock>> Mutex has a state of locked while trying to lock it\n");

    bool wRet=_Base::try_lock();

    if (wRet)
         State_locked = true;
    return wRet;

} // tryLock()

bool
ZMutex::tryLock_for(ZTime pTimeout)
{
int wRet;
    if (State_locked)
        fprintf(stderr,"ZMutex-trylock>> Mutex has a state of locked while trying to lock it\n");

    if (!(wRet=std::timed_mutex::try_lock_for(pTimeout.toChronoNanos())))
                                        return wRet;
    State_locked = true;
    return wRet;
} // tryLock_for()

void
ZMutex::unlock()
{
    if (!State_locked)
            fprintf(stderr,"ZMutex-unlock>> Mutex has a state of unlocked while unlocking it\n");
    if (__ZMUTEXVERBOSE__)
            fprintf(stdout,"ZMutex-I-unlock unlocking ZMutex  \n");

    _Base::unlock();
    State_locked = false;
    return;
}// unlock

//-----------Recursive mutex------------------------------------

ZMtxRecursive::ZMtxRecursive(void)
{
    Mutex_id =(unsigned long) _Base::native_handle();
    OwnedLocks = 0;
}
ZMtxRecursive::~ZMtxRecursive()
{
}

int
ZMtxRecursive::lock()
{
int wRet=0;
    if (__ZMUTEXVERBOSE__)
        {
        if (OwnedLocks)
            fprintf(stderr,"ZMutex-W-lock>> Recursive ZMutex has a state of lock already owned by current process while trying to lock it\n");
        fprintf(stdout,"ZMutex-I-LOCK locking recursive ZMutex  \n");
        }
    try {
        _Base::lock();
    }
    catch (int wErr)
    {
    return wErr;
    }

    OwnedLocks++;
    return wRet;
} // lock()

bool
ZMtxRecursive::trylock()
{
bool wRet=false;
    if (__ZMUTEXVERBOSE__)
        {
        if (OwnedLocks)
            fprintf(stderr,"ZMutex-W-lock>> Recursive ZMutex has a state of lock already owned by current process while trying to lock it\n");
        fprintf(stdout,"ZMutex-I-TRYLOCK trying to lock recursive ZMutex  \n");
        }

     if( wRet=_Base::try_lock())
                                    OwnedLocks++;

    return wRet;
} // trylock()

bool
ZMtxRecursive::tryLock_for(ZTime &pTimeout)
{
bool wRet=false;
    if (__ZMUTEXVERBOSE__)
        {
        if (OwnedLocks)
            fprintf(stderr,"ZMutex-W-lock>> Recursive ZMutex has a state of lock already owned by current process while trying to lock it\n");
        fprintf(stdout,"ZMutex-I-LOCK locking recursive ZMutex  \n");
        }

    if (!(wRet=std::recursive_timed_mutex::try_lock_for(pTimeout.toChronoNanos())))
                        return wRet;
    OwnedLocks++;
    return wRet;
} // lock()
int
ZMtxRecursive::unlock()
{

    if (!OwnedLocks)
        {
        fprintf(stderr,"ZMutex-E-Unlock Error unlocking Recursive ZMutex : No lock is owned on this mutex. \n");
        return -1;
        }
    if (__ZMUTEXVERBOSE__)
            fprintf(stdout,"ZMutex-I-LOCK unlocking ZMutex  \n");
    try {
    _Base::unlock();
    }
    catch (int wErr)
    {
        return wErr;
    }
    OwnedLocks--;
    return 0;
} // unlock
/**
 * @brief ZMtxRecursive::unlockAll unlock all locks that the current process has to the current recursive lock
 */
int
ZMtxRecursive::unlockAll(void)
{
int wRet=0;
    while ((OwnedLocks>0)&&(!(wRet=unlock())));

    return wRet;
}

//-----------Condition mutex------------------------------------


ZMutexCondition::~ZMutexCondition(void)
{
//    delete Mtx;
} // ~ZMutexCondition

/**
 * @brief ZMutexCondition::signal signals a condition for the mutex to local threads
 */
void
ZMutexCondition::signal(void)
{
    notify_one();

 }// signal
/**
 * @brief ZMutexCondition::broadcast broadcasts a condition for the mutex to processes
 */
void
ZMutexCondition::broadcast(void)
{
    notify_all();

 }// broadcast


/**
 * @brief ZMutexCondition::wait hibernates the thread until ZMutexCondition is signaled or broadcasted.
 * base mutex is locked during waiting time and unlocked when thread is awakened.
 */
void
ZMutexCondition::wait(void)
{

//    _Mtx.lock();
//    std::unique_lock<std::mutex> wMtx(_Mtx,std::adopt_lock_t);
    std::unique_lock<std::mutex> wMtx(_Mtx);
    _Base::wait(wMtx);
    wMtx.unlock();
    return;
}// wait
#endif // else __USE_POSIX_ZTHREAD__

#endif // ZMUTEX_CPP

