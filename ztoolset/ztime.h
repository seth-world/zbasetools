#ifndef ZTIME_H
#define ZTIME_H
#include <zconfig.h>
//#include <ztoolset/templatestring.h>   Do not include templatestring.h in this file


#include <time.h>           // for time types and basic time functions
#include <stdint.h>         // intxx_t data types
#include <memory.h>         // memset

#ifdef __USE_WINDOWS__
#define CLOCK_REALTIME      1
int clock_gettime(int X, struct timespec *tv);
#endif

#ifdef __USE_STD_CHRONO__

#include <chrono>

#endif //  __USE_STD_CHRONO__

enum ZDelayPrecision_type
{
    ZDPT_Seconds=0,
    ZDPT_Milliseconds=1,
    ZDPT_Microseconds=2,
    ZDPT_Nanoseconds=3
};
/**
 * @brief The ZTime struct overloaded from struct timespec : same data - only operators added
 * ZTime precision is nano seconds (ZTime.tv_nsec)
 *
 */
class ZTime : public timespec
{
public:
    ZTime () {clear();}
    ZTime (long pT) {tv_sec=pT/1000000000; tv_nsec= pT-(tv_sec*1000000000);}

    ZTime (ZTime& pTi) {tv_sec = pTi.tv_sec; tv_nsec = pTi.tv_nsec; }

    void clear() {memset(this,0,sizeof(ZTime));}

    ZTime& operator = (ZTime& pTi) {tv_sec = pTi.tv_sec; tv_nsec = pTi.tv_nsec; return *this;}
    ZTime operator = (timespec pTi){tv_sec = pTi.tv_sec; tv_nsec = pTi.tv_nsec; return *this;}
    ZTime operator = (timeval pTi){tv_sec = pTi.tv_sec; tv_nsec = pTi.tv_usec*1000; return *this;} // micro to nanoseconds

    ZTime operator +=  (ZTime pTime)
    {
             tv_sec = tv_sec+pTime.tv_sec;
             tv_nsec = tv_nsec+pTime.tv_nsec;
             if (tv_nsec>1000000000)
                     {
                      tv_sec++;
                      tv_nsec = tv_nsec-1000000000;
                     }
             return *this;
    }

    ZTime operator -=  (ZTime pTime) {
    tv_sec = tv_sec - pTime.tv_sec;

    tv_nsec = tv_nsec - pTime.tv_nsec;
    if (tv_nsec<0)
        {
       tv_sec --;
       tv_nsec =1000000000-pTime.tv_nsec;
       tv_nsec +=tv_nsec;
        }
    return *this;
    }
    /**
     * @brief getTimeVal converts and returns current ZTime content to a timeval time structure
     */
    timeval getTimeVal (void) {timeval wTv; wTv.tv_sec = tv_sec; wTv.tv_usec = tv_nsec/1000 ; return wTv;}


    ZTime operator -  (ZTime &pTime)
    {
        ZTime wDelta;
        wDelta.tv_sec = tv_sec - pTime.tv_sec;

        wDelta.tv_nsec = tv_nsec - pTime.tv_nsec;
        if (wDelta.tv_nsec<0)
            {
           wDelta.tv_sec --;
           wDelta.tv_nsec =1000000000-pTime.tv_nsec;
           wDelta.tv_nsec +=tv_nsec;
            }
        return wDelta;
    }
    ZTime operator +  (ZTime &pTi)
    {
        ZTime wDelta;
        wDelta.tv_sec = tv_sec+pTi.tv_sec;
        wDelta.tv_nsec = tv_nsec+pTi.tv_nsec;
        if (wDelta.tv_nsec>1000000000)
                {
                 wDelta.tv_sec++;
                 wDelta.tv_nsec = wDelta.tv_nsec-1000000000;
                }
        return wDelta;
    }

    /**
     * @brief getCurrentTime set ZTime to current time using clock_realtime
     */
    ZTime& getCurrentTime(void) {clock_gettime(CLOCK_REALTIME,this); return *this;}
    /**
     * @brief absoluteFromDelay computes an absolute time from now + pDelay according pDelayType (seconds, milli, micro, nano)
     * and returns current ZTime content.
     */
    ZTime& absoluteFromDelay (ZTime pDelay);

     //! @brief Seconds returns the current ZTime object set to pTime seconds
    ZTime& Seconds      (long long pTime) { memset(this,0,sizeof(ZTime)); tv_sec=pTime; return *this;}

    long long toMilliseconds (void)
    {long long wT;
        wT=(long long)(tv_nsec/1000000);
        wT+=(long long)(tv_sec*1000);
        return wT;
    }
    long long toMicroseconds (void)
    {long long wT=(long long)(tv_sec*1000000);
        wT+=(long long)(tv_nsec/1000);
        return wT;
    }
    long long toNanoseconds (void)
    {long long wT=tv_sec;
        wT= wT*(long long)1000000000;
        wT+=tv_nsec;
        return wT;
    }

    ZTime & fromNanoseconds(long long pNanos)
    {
        tv_sec=pNanos/(long long)1000000000;
        tv_nsec=pNanos-(tv_sec*(long long)1000000000);
        return *this;
    }
    ZTime & fromMicroseconds(long long pMicros)
    {
        tv_sec=pMicros/(long long)1000000;
        tv_nsec=pMicros-(long long)(tv_sec*1000);
        return *this;
    }
    ZTime & fromMilliseconds(long long pMillis)
    {
        tv_sec=pMillis/(long long)1000;
        tv_nsec=pMillis-(tv_sec*1000000);
        return *this;
    }


    ZTime& operator / (long pDividor)
    {
//        long long wFullTime = pTime.toNanoseconds();
//        long long wThisTime = toNanoseconds();
        return    fromNanoseconds(toNanoseconds() /(long long) pDividor);
    }


    ZTime Average(long pTimes) {
        if (pTimes<1)
                pTimes=1;
        return fromNanoseconds(toNanoseconds()/(long long)pTimes);}

    char* toString(char* pBuf,unsigned int pLen, const char* pFormat=nullptr,ZDelayPrecision_type pDelayType=ZDPT_Seconds) ;

    char* delaytoString (char* pBuf,unsigned int pLen,ZDelayPrecision_type pDelayType=ZDPT_Milliseconds) ;



#ifdef __USE_STD_CHRONO__
    /**
     * @brief toChronoNanos  converts current ZTime object to a std::chrono::duration (so not a date but a duration) in nanoseconds
     */
    std::chrono::nanoseconds toChronoNanos() {
    std::chrono::seconds    wStdSeconds(tv_sec);
    std::chrono::nanoseconds wStdNanos(tv_nsec);
    wStdNanos += wStdSeconds;
    return wStdNanos;
    }// toChrono()

#endif //__USE_STD_CHRONO__



};// ZTime

/**
 * @brief zsleep        makes current thread sleep during pMilliseconds milli seconds. This is a portable routine.
 */
void zsleep (unsigned long pMilliseconds);


timeval setTimevalFromMicroSeconds(long pMicroseconds);
long getMicroSecondsFromTimeval(timeval pTime);
timeval setTimevalFromMilliSeconds(long pMilliseconds);
long getMilliSecondsFromTimeval(timeval pTime);

#endif // ZTIME_H
