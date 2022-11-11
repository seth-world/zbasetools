#ifndef ZTIMER_CPP
#define ZTIMER_CPP

#include <ztoolset/ztimer.h>
#include <ztoolset/zutfstrings.h>
//
//-------------  ZTimer ------------------
//
//
/**
 * @brief init Initialize (resets to zero) the time capture session
 */
void
ZTimer::init (void)
         {
        DeltaTime.clear();
         }

/**
 * @brief start Starts the time capture session
 */
void
ZTimer::start (void)
         {
        BeginTime.getCurrentTime();
         }

/**
 * @brief ZTimer::end Ends the time capture session and computes the delta time
 *
 */
void
ZTimer::end (void)
         {
    EndTime.getCurrentTime();
//    gettimeofday( &EndTime,nullptr);
    ZTime wTi;
    wTi= EndTime;
    wTi = wTi-BeginTime;
    DeltaTime += wTi;
         }

ZTime ZTimer::getDeltaTime(void)
{
        return EndTime-BeginTime;
}
ZTime ZTimer::getElapsed(void)
{
  return EndTime-BeginTime;
}
void
ZTimer::addDeltaTime (ZTimer &pTimer1)
{
    DeltaTime+= pTimer1.DeltaTime;
} //reportDeltaTime

/**
 * @brief ZTimer::reportDeltaTime Formats a descstring with already computed delta time.
 * Delta time is reported under format "hh:mm:ss-ms.mms"
 * @return
 */
utf8VaryingString
ZTimer::reportDeltaTime (void)
{
    return reportZTime(DeltaTime);
} //reportDeltaTime


/**
 * @brief ZTimer::reportBeginTime a descstring with timer begin time.
 * Time is reported under format "hh:mm:ss-ms.mms"
 * @return
 */
utf8VaryingString ZTimer::reportBeginTime(void)
{
    return reportZTime(BeginTime) ;
} //reportBeginTime
/**
 * @brief ZTimer::reportEndTime a descstring with timer begin time.
 * Time is reported under format "hh:mm:ss-ms.mms"
 * @return
 */
utf8VaryingString
ZTimer::reportEndTime (void)
{
    return reportZTime(EndTime) ;
} //reportEndTime
/**
 * @brief ZTimer::reportZTimeVal Formats a descstring with a ZTime.
 * Time is reported under format "hh:mm:ss-ms.mms"
 * This is a static method that be used outside context of ZTimer with any ZTime data.
 *
 * @return
 */
utf8VaryingString
ZTimer::reportZTime (ZTime & pTime)
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
//    wDeltans = (wDeltans-(wDeltams*1000000.0)-(wDeltamms*1000.0));
    wms=(int)wDeltams;
    wmms=(int)wDeltamms;
    wns=(int)wDeltans;

//    wms =(short int) (wDeltans/1000000.0); // milliseconds
//    wmms = (short int) ((wDeltans/1000.0)-(wms*1000000.0)); // microseconds
//    wns = (short int) (wDeltans-(wms*1000000.0)-(wmms*1000.0)); // nanoseconds

    utf8VaryingString wReport;
    wReport.sprintf("%02d:%02d:%02d-%03d.%03d.%03d", whh,wmm,wss,wms,wmms,wns);

    return wReport;
} //reportZTime


#endif // ZTIMER_CPP
