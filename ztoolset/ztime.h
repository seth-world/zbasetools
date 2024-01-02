#ifndef ZTIME_H
#define ZTIME_H
#include <config/zconfig.h>
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

#include <ztoolset/charman.h>
//class utf8VaryingString;  // Cannot use utf8VaryingString in this object

//#include <ztoolset/utfvaryingstring.h>

const long cst_nanoconvert=1000000000;



enum ZDelayPrecision_type
{
    ZDPT_Seconds=0,
    ZDPT_Milliseconds=1,
    ZDPT_Microseconds=2,
    ZDPT_Nanoseconds=3
};
class utf8VaryingString;
/**
 * @brief The ZTime struct overloaded from struct timespec : same data - only operators added
 * ZTime precision is nano seconds (ZTime.tv_nsec)
 *
 */
class ZDataBuffer;
class ZTime : public timespec
{
public:
    ZTime () {clear();}
    ZTime (long pT) {tv_sec=pT/cst_nanoconvert; tv_nsec= pT-(tv_sec*cst_nanoconvert);}

    ZTime (const ZTime& pTi) {tv_sec = pTi.tv_sec; tv_nsec = pTi.tv_nsec; }
    ZTime (const timespec& pTi) {tv_sec = pTi.tv_sec; tv_nsec = pTi.tv_nsec; }

    ZTime& _copyFrom(const ZTime& pTi) {tv_sec = pTi.tv_sec; tv_nsec = pTi.tv_nsec; return *this;}

    void clear() {tv_sec=0;  tv_nsec=0;}
    void setNull() {tv_sec=0;  tv_nsec=0;}

    ZTime& operator = (const ZTime& pTi) {return _copyFrom(pTi);}
    ZTime operator = (timespec pTi){return _copyFrom(pTi);}
    ZTime operator = (timeval pTi){tv_sec = pTi.tv_sec; tv_nsec = pTi.tv_usec*1000; return *this;} // micro to nanoseconds

    bool operator == (const ZTime& pIn) {return (tv_sec==pIn.tv_sec && tv_nsec==pIn.tv_nsec);}


    ZTime operator +=  (ZTime pTime);
    ZTime operator -=  (ZTime pTime);

    ZTime operator -  (const ZTime &pTime);
    ZTime operator +  (ZTime &pTi);

    ZTime operator *  (int pMult);
    ZTime operator *  (double pMult);
    ZTime operator /  (int pDiv);
    ZTime operator /  (double pDiv);

    bool isNull() {return tv_sec==0 && tv_nsec==0;}



    /**
     * @brief getTimeVal converts and returns current ZTime content to a timeval time structure
     */
    timeval getTimeVal (void) {timeval wTv; wTv.tv_sec = tv_sec; wTv.tv_usec = tv_nsec/1000 ; return wTv;}

    /**
     * @brief getDelta gets a ZTime with difference between current time and pDelta
     */
    ZTime getDelta(ZTime&& pDelta);

    /**
     * @brief getElapsed returns a ZTime containing difference between current clock time and time of this object
     */
    ZTime getElapsed() { return getCurrentTime() - *this;}

    /**
     * @brief getCurrentTime set ZTime to current time using clock_realtime
     */
    static ZTime getCurrentTime(void) {
      ZTime wRet;
      clock_gettime(CLOCK_REALTIME,&wRet);
      return wRet;
    }

    static ZTime currentTime(void) {ZTime wTi;clock_gettime(CLOCK_REALTIME,&wTi); return wTi;}


    /**
     * @brief absoluteFromDelay computes an absolute time from now + pDelay according pDelayType (seconds, milli, micro, nano)
     * and returns current ZTime content.
     */
    ZTime& absoluteFromDelay (ZTime pDelay);

     //! @brief Seconds returns the current ZTime object set to pTime seconds
    ZTime& Seconds      (long long pTime) { memset(this,0,sizeof(ZTime)); tv_sec=pTime; return *this;}


    long long toMilliseconds (void);
    long long toMicroseconds (void);
    long long toNanoseconds (void);


    ZTime & fromNanoseconds(long long pNanos);
    ZTime & fromMicroseconds(long long pMicros);
    ZTime & fromMilliseconds(long long pMillis);



    ZTime& operator / (long pDividor)
    {
//        long long wFullTime = pTime.toNanoseconds();
//        long long wThisTime = toNanoseconds();
        return    fromNanoseconds(toNanoseconds() /(long long) pDividor);
    }


    ZTime Average(long pTimes) {
        if (pTimes<1)
                pTimes=1;
        return fromNanoseconds(toNanoseconds()/(long long)pTimes);
    }

    char* toString(char* pBuf, size_t pLen, const char* pFormat=nullptr, ZDelayPrecision_type pDelayType=ZDPT_Seconds) ;

//    CharMan toString(const char* pFormat=nullptr,ZDelayPrecision_type pDelayType=ZDPT_Seconds);

    utf8VaryingString toString( const char* pFormat=nullptr, ZDelayPrecision_type pDelayType=ZDPT_Seconds) ;

    char* delaytoString (char* pBuf,unsigned int pLen,ZDelayPrecision_type pDelayType=ZDPT_Milliseconds) ;
    utf8VaryingString delaytoString(ZDelayPrecision_type pPrecision = ZDPT_Milliseconds);

    /* reports ZTime content not as a date-time but as an interval of time */
    utf8VaryingString reportTimeInterval( ZDelayPrecision_type pPrecision=ZDPT_Milliseconds);

    /** @brief _export() serializes ZTime */
    ZDataBuffer _export();
    /** @brief _export() deserializes input pPtrIn and loads ZTime with imported value */
    const unsigned char * _import(const unsigned char *&pPtrIn);


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
