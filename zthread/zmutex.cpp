#ifndef ZMUTEX_CPP
#define ZMUTEX_CPP


#include <zthread/zmutex.h>
#include <ztoolset/zerror_min.h>

#include <time.h> // for timeout computation

#include <ztoolset/utfvaryingstring.h>

using namespace zbs;

#ifdef __USE_POSIX_ZTHREAD__

//#include <zthread/zmutex_pthread.h>
//----------------Mutexes----------------------------------------

//------------Simple mutex--------------------------------------

#define __ZMUTEXVERBOSE__  BaseParameters->VerboseMutex()

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

    fprintf(stderr,decode_MutexError(_GET_FUNCTION_NAME_,wRet).toCChar());
     abort();
}

ZMutex::~ZMutex() {     pthread_mutex_destroy(&Mutex_id);}

int ZMutex::lock()
{

  if (__ZMUTEXVERBOSE__) {
    fprintf(stdout,"ZMutex-I-LOCK locking ZMutex\n");
    std::cout.flush();
  }
  if (State_locked) {
    fprintf(stderr,"ZMutex-%s>> Mutex has a state of locked while trying to lock it\n",_GET_FUNCTION_NAME_);
    std::cout.flush();
    return  EPERM;
  }

  int wRet=pthread_mutex_lock(&Mutex_id);

  if (wRet==0) {
    State_locked = true;
    if (__ZMUTEXVERBOSE__) {
      fprintf(stdout,"ZMutex-I-LOCK           set state locked  \n");
      std::cout.flush();
    }
    return wRet;
  }
  fprintf(stderr,decode_MutexError(_GET_FUNCTION_NAME_,wRet).toCChar());
  return wRet;
}// lock

void ZMutex::lockBlock()
{
    if (__ZMUTEXVERBOSE__)
        fprintf(stdout,"ZMutex-I-LOCKBLOCK locking ZMutex  \n");

    int wRet=pthread_mutex_lock(&Mutex_id);

    if (wRet==0)
        {
        State_locked = true;
        if (__ZMUTEXVERBOSE__)
          fprintf(stdout,"ZMutex-I-LOCKBLOCK          set state locked  \n");
        return ;
        }

     fprintf(stderr,decode_MutexError(_GET_FUNCTION_NAME_,wRet).toCChar());
     return ;
}// lockBlock

bool
ZMutex::tryLock()
{
  if (__ZMUTEXVERBOSE__) {
    fprintf(stdout,"ZMutex-I-TRYLOCK trying to lock ZMutex  \n");
    std::cout.flush();
  }
  if (State_locked) {
    fprintf(stderr,"ZMutex-tryLock>> Mutex has a state of locked while trying to lock it\n");
    std::cout.flush();
    return  EPERM;
  }

  int wRet=pthread_mutex_trylock(&Mutex_id);

  if (wRet==0) {
    State_locked = true;
    if (__ZMUTEXVERBOSE__) {
      fprintf(stdout,"ZMutex-I-TRYLOCK           set state locked  \n");
      std::cout.flush();
    }
  }
  else {
    if (wRet==EBUSY)
            fprintf(stderr,"ZMutex-tryLock>> EBUSY returned while trying to lock it\n");
        else
          fprintf(stderr,decode_MutexError(_GET_FUNCTION_NAME_,wRet).toCChar());
  }
  return (wRet==0);

} // tryLock()

int
ZMutex::timedLock(ZTime pTimeout)
{
ZTime wTimeoutlimit;
  if (__ZMUTEXVERBOSE__)
    fprintf(stdout,"ZMutex-I-TIMEDLOCK trying to lock ZMutex  \n");

    if (State_locked)
        {
        fprintf(stderr,"ZMutex-%s>> Mutex has a state of locked while trying to lock it\n",_GET_FUNCTION_NAME_);
        return  EPERM;
        }


    wTimeoutlimit.absoluteFromDelay(pTimeout);

    int wRet=pthread_mutex_timedlock(&Mutex_id,&wTimeoutlimit);

    if (wRet!=0)
        {
        if (wRet==ETIMEDOUT)
                    return wRet;
        if (wRet==EBUSY)
            fprintf(stderr,"ZMutex-timedLock>> EBUSY returned while trying to lock ZMutex\n");
        else {
          fprintf(stderr,decode_MutexError(_GET_FUNCTION_NAME_,wRet).toCChar());
          abort();
        }
        }
    State_locked = true;
    if (__ZMUTEXVERBOSE__)
      fprintf(stdout,"ZMutex-I-TIMEDLOCK           set state locked  \n");
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
        fprintf(stdout,"ZMutex-I-UNLOCKBESTTRY unlocking ZMutex  \n");

    int wRet=pthread_mutex_unlock(&Mutex_id);
    if (wRet==0)
            {
            State_locked = false;
            if (__ZMUTEXVERBOSE__)
              fprintf(stdout,"ZMutex-I-UNLOCKBESTTRY            set state unlocked  \n");
            return wRet;
            }
     fprintf(stderr,decode_MutexError(_GET_FUNCTION_NAME_,wRet).toCChar());
     return wRet;
}// unlock

int
ZMutex::unlock()
{
  if (__ZMUTEXVERBOSE__) {
    fprintf(stdout,"ZMutex-I-UNLOCK unlocking ZMutex  \n");
    std::cout.flush();
  }

  if (!State_locked) {
    fprintf(stderr,"ZMutex-unlock>> Mutex has a state of unlocked while unlocking it\n");
    std::cout.flush();
    return EPERM;
  }

  int wRet=pthread_mutex_unlock(&Mutex_id);
  if (wRet==0) {
    State_locked = false;
    if (__ZMUTEXVERBOSE__) {
      fprintf(stdout,"ZMutex-I-UNLOCK               set state unlocked  \n");
      std::cout.flush();
    }
    return wRet;
  }
  fprintf(stderr,decode_MutexError(_GET_FUNCTION_NAME_,wRet).toCChar());
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
     fprintf(stderr,decode_MutexError(_GET_FUNCTION_NAME_,wRet).toCChar());
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
    fprintf(stderr,decode_MutexError(_GET_FUNCTION_NAME_,wRet).toCChar());
     return wRet;
}
bool
ZMtxRecursive::trylock()
{
bool wRet=false;
    if (__ZMUTEXVERBOSE__)
    {
      fprintf(stdout,"ZMutex-I-TRYLOCK trying to lock recursive ZMutex  \n");
    }

    if (OwnedLocks)
        {
        fprintf(stderr,"ZMutex-%s>> Recursive ZMutex has a state of lock already owned by current process while trying to lock it\n",_GET_FUNCTION_NAME_);
        return  EPERM;
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
     fprintf(stderr,decode_MutexError(_GET_FUNCTION_NAME_,wRet).toCChar());
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
    if (__ZMUTEXVERBOSE__)
      fprintf(stdout,"ZMutex-I-TIMEDWAIT           set state locked  \n");
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

/* see https://www.man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html */

utf8VaryingString
decode_MutexError(const char* pModule,int pStatus) {
  utf8VaryingString wReturn;
  utf8VaryingString wErrMes;

  switch(pStatus) {
  case 0:
    wErrMes = "Success";
    break;
  case EAGAIN :
    wErrMes = "<EAGAIN> The mutex could not be acquired because the maximum number of recursive locks for mutex has been exceeded.";
    break;
  case EINVAL :
    wErrMes = "<EINVAL> The mutex was created with the protocol attribute having the value PTHREAD_PRIO_PROTECT and the calling thread's priority is higher than the mutex's current priority ceiling.";
    break;
  case ENOTRECOVERABLE :
    wErrMes = "<ENOTRECOVERABLE> The state protected by the mutex is not recoverable.";
    break;
  case EOWNERDEAD :
    wErrMes = "<EOWNERDEAD>The mutex is a robust mutex and the process containing the previous owning thread terminated while holding the mutex lock. The mutex lock shall be acquired by the calling thread and it is up to the new owner to make the state consistent.";
    break;
  case EDEADLK :
    wErrMes = "<EDEADLK> The mutex type is PTHREAD_MUTEX_ERRORCHECK and the current thread already owns the mutex.";
    break;
  case EBUSY :
    wErrMes = "<EBUSY>  The mutex could not be acquired because it was already locked.";
    break;
  case EPERM :
    wErrMes = "<EPERM>  The mutex type is PTHREAD_MUTEX_ERRORCHECK or PTHREAD_MUTEX_RECURSIVE, or the mutex is a robust mutex, and the current thread does not own the mutex.";
    break;

  default:
    wErrMes = "Unknown mutex error returned";
    break;
  }
  wReturn.sprintf("ZMutex-%s %s",pModule,wErrMes.toString());
  return wReturn;
}



#endif // ZMUTEX_CPP

