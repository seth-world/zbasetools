#ifndef ZTIME_CPP
#define ZTIME_CPP

#include "ztime.h"

#ifndef __USE_WINDOWS__
#include <unistd.h>
#endif

#include <string.h>
#include <cstdio>

#include <ztoolset/zerror.h>
#include <ztoolset/zdatabuffer.h>

#include <ztoolset/utfvaryingstring.h>

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

char* ZTime::toString(char* pBuf,size_t pLen, const char* pFormat,ZDelayPrecision_type pDelayType) {
//        int timespec2str(char *buf, uint len, struct timespec *ts) {
        int wRet;
        struct tm t;

        tzset();
        if (localtime_r(&(tv_sec), &t) == nullptr)
            return nullptr;

        if (pFormat==nullptr)
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
        wRet = snprintf(&pBuf[strlen(pBuf)], pLen, ".%03d", wMilli);
        if (pDelayType>ZDPT_Milliseconds)
        {
        wRet = snprintf(&pBuf[strlen(pBuf)], pLen, ".%03d", wMicro );
        if (pDelayType>ZDPT_Microseconds)
        {
        wRet = snprintf(&pBuf[strlen(pBuf)], pLen, ".%03d", wNano );
        }
        }
        }
//            if (wRet >= pLen)
//                return 3;

        return pBuf;

}

long long
ZTime::toMilliseconds (void)
{long long wT;
  wT=(long long)(tv_nsec/1000000);
  wT+=(long long)(tv_sec*1000);
  return wT;
}

long long
ZTime::toMicroseconds (void)
{long long wT=(long long)(tv_sec*cst_nanoconvert);
  wT+=(long long)(tv_nsec/1000);
  return wT;
}
long long
ZTime::toNanoseconds (void)
{long long wT=tv_sec;
  wT= wT*(long long)cst_nanoconvert;
  wT+=tv_nsec;
  return wT;
}



ZTime & ZTime::fromNanoseconds(long long pNanos)
{
  tv_sec=pNanos/(long long)cst_nanoconvert;
  tv_nsec=pNanos-(tv_sec*(long long)cst_nanoconvert);
  return *this;
}
ZTime & ZTime::fromMicroseconds(long long pMicros)
{
  tv_sec=pMicros/(long long)1000000;
  tv_nsec=pMicros-(long long)(tv_sec*1000);
  return *this;
}
ZTime & ZTime::fromMilliseconds(long long pMillis)
{
  tv_sec=pMillis/(long long)1000;
  tv_nsec=pMillis-(tv_sec*1000000);
  return *this;
}

/*
utf8VaryingString
 ZTime::toString(const char* pFormat,ZDelayPrecision_type pDelayType)
{
  utf8VaryingString wReturn;
  wReturn.allocateUnitsBZero(150);
  toString((char*)wReturn.Data,wReturn.getUnitCount(),pFormat,pDelayType);
  return wReturn;
}
*/

CharMan ZTime::toString(const char* pFormat,ZDelayPrecision_type pDelayType)
{
  CharMan wReturn;
  toString(wReturn.content,wReturn.getUnitCount(),pFormat,pDelayType);
  return wReturn;
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

ZDataBuffer ZTime::_export()
{
  ZDataBuffer wReturn;

  _exportAtomic<__time_t>(tv_sec,wReturn);
  _exportAtomic<__syscall_slong_t>(tv_nsec,wReturn);
  return wReturn;
}
const unsigned char *ZTime::_import(const unsigned char *&pPtrIn)
{
  _importAtomic<__time_t>(tv_sec,pPtrIn);
  _importAtomic<__syscall_slong_t>(tv_nsec,pPtrIn);
  return pPtrIn;
}



ZTime ZTime::operator -  (ZTime &pTime)
{
  ZTime wDelta;
  wDelta.tv_sec = tv_sec - pTime.tv_sec;

  wDelta.tv_nsec = tv_nsec - pTime.tv_nsec;
  if (wDelta.tv_nsec < 0)
  {
    wDelta.tv_sec --;
    wDelta.tv_nsec = tv_nsec + cst_nanoconvert - pTime.tv_nsec;
  }
  return wDelta;
}

ZTime ZTime::getDelta(ZTime &&pDelta) {
    ZTime wDelta;
    wDelta.tv_sec = tv_sec - pDelta.tv_sec;

    wDelta.tv_nsec = tv_nsec - pDelta.tv_nsec;
    if (wDelta.tv_nsec < 0)
    {
      wDelta.tv_sec --;
      wDelta.tv_nsec =cst_nanoconvert-pDelta.tv_nsec;
      wDelta.tv_nsec +=tv_nsec;
    }
    return wDelta;
}


ZTime ZTime::operator+  (ZTime &pTi)
{
  ZTime wDelta;
  wDelta.tv_sec = tv_sec+pTi.tv_sec;
  wDelta.tv_nsec = tv_nsec+pTi.tv_nsec;
  if (wDelta.tv_nsec>1000000000)
  {
    wDelta.tv_sec++;
    wDelta.tv_nsec = wDelta.tv_nsec-cst_nanoconvert;
  }
  return wDelta;
}






ZTime ZTime::operator +=  (ZTime pTime)
{
  tv_sec = tv_sec+pTime.tv_sec;
  tv_nsec = tv_nsec+pTime.tv_nsec;
  if (tv_nsec > cst_nanoconvert)
  {
    tv_sec++;
    tv_nsec = tv_nsec-cst_nanoconvert;
  }
  return *this;
}

ZTime ZTime::operator -=  (ZTime pTime) {
  tv_sec = tv_sec - pTime.tv_sec;

  if ((tv_nsec - pTime.tv_nsec) < 0)
  {
    tv_sec --;
    tv_nsec = tv_nsec + cst_nanoconvert - pTime.tv_nsec;
    tv_nsec += tv_nsec;
  }
  else
    tv_nsec = tv_nsec - pTime.tv_nsec;
  return *this;
}

ZTime ZTime::operator*  (int pMult)
{
  ZTime wResult;
  double wmult = double(pMult);
  double wsec = double(tv_sec);
  double wnsec = double (tv_nsec);
  wsec = wsec * wmult;

  wResult.tv_sec = long(wsec);

  wnsec = wnsec * wmult;
  if (wnsec > 1000000000.0) {
    long wS = long (wnsec / 1000000000.0) ;
    wnsec = wnsec - (double(wS) * 1000000000.0);

    wResult.tv_sec += wS ;
  }
  wResult.tv_nsec = long(wnsec);

  return wResult;
}

ZTime ZTime::operator*  (double pMult)
{
  ZTime wResult;

  double wsec = double(tv_sec);
  double wnsec = double (tv_nsec);
  wsec = wsec * pMult;

  wResult.tv_sec = long(wsec);

  wnsec = wnsec * pMult;
  if (wnsec > 1000000000.0) {
    long wS = long (wnsec / 1000000000.0) ;
    wnsec = wnsec - (double(wS) * 1000000000.0);

    wResult.tv_sec += wS ;
  }
  wResult.tv_nsec = long(wnsec);

  return wResult;
}

ZTime ZTime::operator/  (int pDiv)
{
  ZTime wResult;
  double wdiv = double(pDiv);
  double wsec = double(tv_sec);
  double wnsec = double (tv_nsec);

  wsec = wsec / wdiv;

  wResult.tv_sec = long(wsec);

  wnsec = wnsec / wdiv;
  wnsec = wnsec +(wsec - (double (long(wsec)) * 1000000000.0)) ;

  wResult.tv_nsec = long(wnsec);

  return wResult;
}

ZTime ZTime::operator/  (double pDiv)
{
  ZTime wResult;

  double wsec = double(tv_sec);
  double wnsec = double (tv_nsec);

  wsec = wsec / pDiv;

  wResult.tv_sec = long(wsec);

  wnsec = wnsec / pDiv;
  wnsec = wnsec +(wsec - ( double (long(wsec)) * 1000000000.0)) ;

  wResult.tv_nsec = long(wnsec);

  return wResult;
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
