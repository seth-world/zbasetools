#ifndef ZTIMER_H
#define ZTIMER_H

#include <config/zconfig.h>
#include <ztoolset/utfvaryingstring.h>
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
class ZTimer : public ZTime
{
    ZTime  DeltaTime;
public:
    ZTimer() {init();}
    ZTimer(const ZTimer& pIn) {_copyFrom(pIn);}
    ZTimer(const ZTime& pIn) {DeltaTime.clear(); ZTime::_copyFrom(pIn); }

    ZTimer& _copyFrom(const ZTimer& pIn){
        set(pIn);
        DeltaTime = pIn.DeltaTime;
        return *this;
    }

    ZTimer& operator =(const ZTimer& pIn) { return _copyFrom(pIn);}

    void set(ZTime pTi) {ZTime(*this)=(pTi);}
    void clear() {DeltaTime.clear(); ZTime::clear();}
    void init(void);
    void start (void);
    void end (void);
    ZTime getDeltaTime(void) ;

    ZTime getElapsed();

    void addDeltaTime (ZTimer &pTimer1);
    utf8VaryingString reportDeltaTime(void) ;
    utf8VaryingString reportElapsed(ZDelayPrecision_type pDelayType= ZDPT_Milliseconds) ;
    utf8VaryingString reportBeginTime(void) ;
    utf8VaryingString reportEndTime(void) ;
    static utf8VaryingString reportTimeInterval( ZTime pTime, ZDelayPrecision_type pPrecision=ZDPT_Milliseconds); // generic static function

    ZTimer operator += (ZTimer pTi) {DeltaTime+=pTi.DeltaTime; return *this;}

}; // ZTimer


#endif // ZTIMER_H
