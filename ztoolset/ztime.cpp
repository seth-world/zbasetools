#ifndef ZTIME_CPP
#define ZTIME_CPP
#include "string.h"
#include <ztoolset/ztime.h>
#include <cstdio>
#include <ztoolset/zerror.h>
/**
 * @brief ZTime::absoluteFromDelay
 * @param pDelay
 * @return
 */
ZTime&
ZTime::absoluteFromDelay(ZTime pDelay)
{
    getCurrentTime();
    *this += pDelay ;
    return *this;
}//ZTimeFromDelay


#ifndef __USE_WINDOWS__
#include <unistd.h>
#endif

void zsleep (unsigned long pMilliseconds)
{

#ifdef __USE_WINDOWS__
    Sleep(pMilliseconds);
#else
    usleep(pMilliseconds);
#endif
}

#ifdef __USE_WINDOWS__


LARGE_INTEGER
getFILETIMEoffset()
{
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}
/**
 * @brief clock_gettime windows emulation of clock_gettime
 * @param X
 * @param tv
 * @return
 */
int
clock_gettime(int X, struct timespec *tv)
{
    LARGE_INTEGER           t;
    FILETIME            f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) QueryPerformanceCounter(&t);
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_usec = t.QuadPart % 1000000;
    return (0);
}

#endif // __USE_WINDOWS__

char* ZTime::toString(char* pBuf,unsigned int pLen, const char* pFormat,ZDelayPrecision_type pDelayType) {
//        int timespec2str(char *buf, uint len, struct timespec *ts) {
        int wRet;
        struct tm t;

        tzset();
        if (localtime_r(&(tv_sec), &t) == NULL)
            return nullptr;

        if (pFormat==NULL)
                wRet = strftime(pBuf, pLen, "%d-%m-%y %T", &t);
            else
                wRet = strftime(pBuf, pLen, pFormat, &t);

        if (wRet == 0)
                {
                fprintf (stderr,"%s>> invalid time conversion \n", _GET_FUNCTION_NAME_);
                pBuf[0]='\0';
                return pBuf;
                }
        pLen -= wRet - 1;
        if (pDelayType<ZDPT_Milliseconds)
                                    return pBuf;
        int wReminder = tv_nsec;
        int wMilli = wReminder/1000000;
        wReminder = wReminder-(wMilli*1000000);
        int wMicro = wReminder/1000;
        int wNano = wReminder-(wMicro*1000);

        if (pDelayType>ZDPT_Seconds)
        {
        wRet = snprintf(&pBuf[strlen(pBuf)], pLen, ".%03ld", wMilli);
        if (pDelayType>ZDPT_Milliseconds)
        {
        wRet = snprintf(&pBuf[strlen(pBuf)], pLen, ".%03ld", wMicro );
        if (pDelayType>ZDPT_Microseconds)
        {
        wRet = snprintf(&pBuf[strlen(pBuf)], pLen, ".%03ld", wNano );
        }
        }
        }
//            if (wRet >= pLen)
//                return 3;

        return pBuf;

}


char* ZTime::delaytoString(char* pBuf,unsigned int pLen,ZDelayPrecision_type pDelayType)
{
    int wDays;
    int wHours;
    int wMin;
    int wSec;
    long wReminder=tv_sec;
        wDays=(int)(wReminder/86400);
        wReminder=wReminder-(wDays*86400);
        wHours=(int)wReminder/3600;
        wReminder=wReminder-(wHours*3600);
        wMin=(int)wReminder/60;
        wSec=wReminder-(wMin*60);

        int wMilli;
        int wMicros;
        int wNanos;
        wReminder=tv_nsec;
        wMilli=(int)tv_nsec/1000000;
        wReminder=(int)wReminder-(wMilli*1000000);
        wMicros=(int)wReminder/1000;
        wNanos=wReminder-(wMicros*1000);

/*        snprintf(pBuf,pLen,"%d%s%d%s%d%s%d",
                 wDays,
                 wDays>0?"-":"",
                 wHours,
                 wHours>0?":":"",
                 wMin,
                 wHours>0?":":"",
                 wSec);
*/
        memset(pBuf,0,pLen);

        if (wDays>0)
            snprintf(pBuf,pLen,"%2d-",wDays);
        if (wHours>0)
            snprintf(&pBuf[strlen(pBuf)],pLen-strlen(pBuf),"%2d:",wHours);
        if (wMin>0)
            snprintf(&pBuf[strlen(pBuf)],pLen-strlen(pBuf),"%2d:",wMin);
        // anyway the seconds
        snprintf(&pBuf[strlen(pBuf)],pLen-strlen(pBuf),"%2d",wSec);
/*        snprintf(pBuf,pLen,"%2d-%2d:%2d:%2d",
                 wDays,
                 wHours,
                 wMin,
                 wSec);*/
        if (pDelayType>0)
            {
            snprintf(&pBuf[strlen(pBuf)],pLen-strlen(pBuf),"-%03d",wMilli);
            }
        if (pDelayType>1)
            {
            snprintf(&pBuf[strlen(pBuf)],pLen-strlen(pBuf),".%03d",wMicros);
            }
        if (pDelayType>2)
            {
            snprintf(&pBuf[strlen(pBuf)],pLen-strlen(pBuf),".%03d",wNanos);
            }
/*
        snprintf(pBuf,pLen,"%sd%s:%s:%s-%s.%s.%s",
                 wDays,
                 wHours,
                 wMin,
                 wSec,
                 wMilli,
                 wMicros,
                 wNanos);
*/
        return pBuf;

}
timeval setTimevalFromMicroSeconds(long pMicroseconds)
{
timeval pTime;
    memset(&pTime,0,sizeof(pTime));
    pTime.tv_sec = (int) (pMicroseconds/1000000);
    pTime.tv_usec = pMicroseconds - (pTime.tv_sec*1000000);
    return pTime;
}

timeval setTimevalFromMilliSeconds(long pMilliseconds)
{
timeval pTime;
    memset(&pTime,0,sizeof(pTime));
    pTime.tv_sec = (int) (pMilliseconds/1000);
    pTime.tv_usec = (pMilliseconds*1000) - (pTime.tv_sec*1000000);
    return pTime;
}
long getMilliSecondsFromTimeval(timeval pTime)
{
    return (long) ((pTime.tv_sec*1000)+(pTime.tv_usec/1000000));
}

long getMicroSecondsFromTimeval(timeval pTime)
{
    return (long) ((pTime.tv_sec*1000000)+(pTime.tv_usec/1000));
}

#endif // ZTIME_CPP
