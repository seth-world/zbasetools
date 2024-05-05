#ifndef ZTIMER_CPP
#define ZTIMER_CPP

#include "ztimer.h"
#include "utfvaryingstring.h"
//
//-------------  ZTimer ------------------
//
//
/**
 * @brief init Initialize (resets to zero) the time capture session
 */
void
ZTimer::init (void) {
    ZTime::clear();
    DeltaTime.clear();
 //   this-> ZTime::_copyFrom(getCurrentTime());
}

/**
 * @brief start Starts the time capture session
 */
void
ZTimer::start (void)
{
    this-> ZTime::_copyFrom(getCurrentTime());
}

/**
 * @brief ZTimer::end Ends the time capture session and computes the delta time
 *
 */
void
ZTimer::end (void)
{
    DeltaTime += ZTime::getCurrentTime() - (ZTime)*this   ;
    ZTime::clear();
}

ZTime ZTimer::getDeltaTime(void)
{
    return DeltaTime;
//    return ZTime::getCurrentTime() - (ZTime)*this;
}
ZTime ZTimer::getElapsed(void)
{
    return ZTime::getCurrentTime() - (ZTime)*this;
}
void
ZTimer::addDeltaTime (ZTimer &pTimer1)
{
    DeltaTime += pTimer1.DeltaTime;
} //reportDeltaTime

/**
 * @brief ZTimer::reportDeltaTime Formats a descstring with already computed delta time.
 * Delta time is reported under format "hh:mm:ss-ms.mms"
 * @return
 */
utf8VaryingString
ZTimer::reportDeltaTime (void)
{
    if (DeltaTime.isNull())
        end();
    return reportTimeInterval(DeltaTime);
} //reportDeltaTime

utf8VaryingString
ZTimer::reportElapsed(ZDelayPrecision_type pDelayType)
{
  return reportTimeInterval(getElapsed());
} //reportDeltaTime
/**
 * @brief ZTimer::reportBeginTime a descstring with timer begin time.
 * Time is reported under format "hh:mm:ss-ms.mms"
 * @return
 */
utf8VaryingString ZTimer::reportBeginTime(void)
{
    return reportTimeInterval(*this) ;
} //reportBeginTime

/**
 * @brief ZTimer::reportZTimeVal Formats a descstring with a ZTime.
 * Time is reported under format "hh:mm:ss-ms.mms"
 * This is a static method that be used outside context of ZTimer with any ZTime data.
 *
 * @return
 */
utf8VaryingString
ZTimer::reportTimeInterval (ZTime pTime,ZDelayPrecision_type pPrecision)
{
double wDelta , wDeltams,wDeltamms, wDeltans , wRemain ;

short int whh , wmm, wss, wms , wmms , wns;
    wDelta =(double) (pTime.tv_sec);
    whh = (int)(wDelta /3600.0 );
    wRemain  = (int)(wDelta - ((double)whh * 3600.0)) ;

    wmm = (int) (wRemain/60.0) ;
    wRemain  = wRemain - (wmm * 60.0) ;

    wss = (short int)wRemain;

//    deltans = pTime.tv_nsec ;
    if (pTime.tv_nsec<0)
            {
            wss --;
            wDeltans = 1000000000.0 + pTime.tv_nsec; // nano is 10^9 - micro 10^6 - milli 10^3
            }
        else
        wDeltans=pTime.tv_nsec;
    wDeltams=(wDeltans/1000000.0);
    wDeltams=(int)wDeltams;
    wRemain= wDeltans - (wDeltams*1000000);
    wDeltamms=(wRemain/1000.0);
    wDeltamms=(int)(wDeltamms);
    wDeltans= wRemain - (wDeltamms*1000);

    wms=(int)wDeltams;
    wmms=(int)wDeltamms;
    wns=(int)wDeltans;

    utf8VaryingString wReport;
    switch(pPrecision)
    {
    case ZDPT_Seconds:
        wReport.sprintf("%02d:%02d:%02d", whh,wmm,wss);
        break;
    case ZDPT_Milliseconds:
        wReport.sprintf("%02d:%02d:%02d-%03d", whh,wmm,wss,wms);
    case ZDPT_Microseconds:
        wReport.sprintf("%02d:%02d:%02d-%03d.%03d", whh,wmm,wss,wms,wmms);
        break;
    case ZDPT_Nanoseconds:
        wReport.sprintf("%02d:%02d:%02d-%03d.%03d.%03d", whh,wmm,wss,wms,wmms,wns);
        break;
    }
    return wReport;
} //reportZTime


#endif // ZTIMER_CPP
