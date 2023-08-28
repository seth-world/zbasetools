#ifndef ZTIMER_H
#define ZTIMER_H

#include <config/zconfig.h>
#include <ztoolset/zutfstrings.h>
#include <ztoolset/ztime.h>

/**
 * @brief The ZTimer class  manages a timer : setup, stop and format resulting interval of time.
 *
 *  - init :        reset the timer and starts counting
 *  - end :         end the counting
 *  - interval :    compute the time difference and provides it as a descString
 *
 *
 */
class ZTimer
{
    ZTime BeginTime , EndTime , DeltaTime;
public:
    ZTimer() {init();}
    void init(void);
    void start (void);
    void end (void);
    ZTime getDeltaTime(void) ;

    ZTime getElapsed();

    void addDeltaTime (ZTimer &pTimer1);
    utf8VaryingString reportDeltaTime(void) ;
    utf8VaryingString reportElapsed(void) ;
    utf8VaryingString reportBeginTime(void) ;
    utf8VaryingString reportEndTime(void) ;
    static
    utf8VaryingString reportZTime(ZTime pTime); // generic static function

    ZTimer operator = (ZTimer pTi) {BeginTime=pTi.BeginTime;
                                    EndTime=pTi.EndTime;
                                    DeltaTime=pTi.DeltaTime;
                                    return *this;}
    ZTimer operator += (ZTimer pTi) {DeltaTime+=pTi.DeltaTime; return *this;}

}; // ZTimer


#endif // ZTIMER_H
