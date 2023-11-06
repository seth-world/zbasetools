#include "zdatefull.h"
#include <locale.h>

bool HasTimeZone=false;
long int TimeZoneDiff=0;
int DST=0;
char TimeZoneName[20];

const char* GMT="GMT";


void getAllTimeZone() {
  time_t wTimet ;
  time(&wTimet);

  struct tm wTm;
  localtime_r(&wTimet,&wTm);

  TimeZoneDiff = wTm.tm_gmtoff;
  memset (TimeZoneName,0,sizeof(TimeZoneName));
  strncpy(TimeZoneName,wTm.tm_zone,19);

  DST=wTm.tm_isdst;

  HasTimeZone = true;
}


long getTimeZoneDiff (){
  if (!HasTimeZone){
    getAllTimeZone();
  }
  return TimeZoneDiff;
}

const char* getTimeZoneName (){
  if (!HasTimeZone){
    getAllTimeZone();
  }
  return TimeZoneName;
}

int getDST (){
  if (!HasTimeZone){
    getAllTimeZone();
  }
  return DST;
}

//#include <ztoolset/zdate.h>

using namespace zbs;

#include <ztoolset/zlocale.h>

ZDateFull::ZDateFull()
{
 memset(this, 0, sizeof(ZDateFull));
}


//--------------- ZDateFull-------------------------
//

ZDateFull &
ZDateFull::_copyFrom(const  ZDateFull &pIn)
{
  tv_sec =  pIn.tv_sec ;
  tv_nsec = pIn.tv_nsec ;
  return *this;
}


uint64_t
ZDateFull::_export(void) const
{
  struct tm wTm;

  if (gmtime_r(&tv_sec,&wTm)==nullptr)
    return 0;

  wuIn64 wuExport;
  wuExport.Year=reverseByteOrder_Conditional<uint16_t>(uint16_t(wTm.tm_year+1900));
  wuExport.Month = uint8_t(wTm.tm_mon+1);
  wuExport.Day = uint8_t(wTm.tm_mday);
  wuExport.Hour = uint8_t(wTm.tm_hour);
  wuExport.Min = uint8_t(wTm.tm_min);
  wuExport.Sec = uint8_t(wTm.tm_sec);

  uint64_t wOut;
  memmove(&wOut,&wuExport,sizeof(uint64_t));
  return wOut;
}// _export

void
ZDateFull::_import(uint64_t pIDate)
{
  wuIn64 wX;
  memmove(&wX,&pIDate,sizeof(uint64_t));
  struct tm wTm;
  wTm.tm_year = int(reverseByteOrder_Conditional<uint16_t>(wX.Year) - 1900 );
  wTm.tm_mon = int(wX.Month)  ;
  wTm.tm_mday = int(wX.Day)  ;
  wTm.tm_hour = int(wX.Hour)  ;
  wTm.tm_min = int(wX.Min)  ;
  wTm.tm_sec = int(wX.Sec)  ;

  tv_sec=mktime(&wTm); // equivalent to  tv_sec=timelocal(&wTm);
  tv_nsec = 0;

  return;
}// _import

ssize_t ZDateFull::_exportURF(ZDataBuffer &pZDB) const
{
    unsigned char* wPtr=pZDB.extend(sizeof(ZTypeBase)+sizeof(uint64_t));
    return _exportURF_Ptr(wPtr);
}// _export

ssize_t
ZDateFull::_exportURF_Ptr(unsigned char* &pURF) const
{
  uint64_t wD = _export();
  _exportAtomicPtr<ZTypeBase>(ZType_ZDateFull,pURF);
  memmove(pURF,&wD,sizeof(wD));
  pURF += sizeof(wD);
  return sizeof(ZTypeBase) + sizeof(uint64_t);
}// _exportURF_Ptr



ssize_t ZDateFull::_importURF(const unsigned char *&pZDB)
{
  uint64_t wD = 0;
  ZTypeBase wType;

  _importAtomic<ZTypeBase>(wType,pZDB);

  if (wType!=ZType_ZDateFull)
  {
    fprintf(stderr,"%s-Error invalid ZType_type <%X> <%s> while expecting <%s>\n",
        _GET_FUNCTION_NAME_,
        wType,
        decode_ZType(wType),
        decode_ZType(ZType_ZDateFull));
    return -1;
  }
  memmove(&wD,pZDB,sizeof(uint64_t));
  pZDB += sizeof(uint64_t);
  _import(wD);

  return  ssize_t(sizeof(uint64_t)+sizeof(ZTypeBase));
}// _importURF

size_t
ZDateFull::getURFSize() const
{
  return sizeof(ZTypeBase) + sizeof(uint64_t);
}
size_t
ZDateFull::getURFHeaderSize() const
{
  return sizeof(ZTypeBase) ;
}
size_t
ZDateFull::getUniversalSize() const
{
  return sizeof(uint64_t);
}



void ZDateFull::_exportPtr(unsigned char* &pPtr) const {
  uint64_t wDE=_export();
  memmove(pPtr,&wDE,sizeof(uint64_t));
  pPtr += sizeof(uint64_t);
}
void ZDateFull::_exportAppend(ZDataBuffer& pZDB) const {
  unsigned char* wPtr=pZDB.extend(sizeof(uint64_t));
  _exportPtr(wPtr);
}

void ZDateFull::_import(const unsigned char* &pPtrIn) {
  uint64_t* wPtr=(uint64_t* )pPtrIn;
  _import(wPtr[0]);
  pPtrIn+=sizeof(uint64_t);
}


size_t
ZDateFull::getUniversal_Ptr(unsigned char *&pUniversalPtr) const {
  uint64_t wD = _export();
  memmove(pUniversalPtr,&wD,sizeof(uint64_t));
  pUniversalPtr += sizeof(uint64_t);
}

ZStatus
ZDateFull::getUniversalFromURF(const unsigned char* pURFDataPtr,ZDataBuffer& pUniversal,const unsigned char** pURFDataPtrOut)
{
 ZTypeBase wType;
 const unsigned char* wURFDataPtr = pURFDataPtr;

     memmove(&wType,wURFDataPtr,sizeof(ZTypeBase));
     wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
     wURFDataPtr += sizeof (ZTypeBase);
     if (wType!=ZType_ZDateFull)
         {
         fprintf (stderr,
                  "%s>> Error invalid URF data type <%X> <%s> while expecting <%s> ",
                  _GET_FUNCTION_NAME_,
                  wType,
                  decode_ZType(wType),
                  decode_ZType(ZType_ZDateFull));
         return ZS_INVTYPE;
         }

    memmove(pUniversal.Data,wURFDataPtr,sizeof(uint64_t));

    if (pURFDataPtrOut)
      {
      *pURFDataPtrOut = wURFDataPtr + sizeof(uint64_t);
      }

    return ZS_SUCCESS;
}//getUniversalFromURF

ZStatus
ZDateFull::getValueFromUniversal(const unsigned char* pUniversalDataPtr)
{
 uint64_t wUniversal;

    memmove(&wUniversal,pUniversalDataPtr,sizeof(uint64_t));
    _import(wUniversal);
    return ZS_SUCCESS;
}//getValueFromUniversal



/** @brief getDateValue returns an int with decoded decimal value with a maximum number of digit of pMaxDigits.
 * If end of string encountered pEnd is set to 1, else set to 0.
 * If end of string has been encountered without any valid decimal digit, then returned value is -1 and pEnd is set to 1.
 */
int getDateValue(const unsigned char* &pPtrIn,int &pEnd,int pMaxDigits) {
  pEnd=0;
  /* skip leading bullshit */
  while (!isdigit(*pPtrIn)) {
    if (*pPtrIn==0) {
      pEnd = 1;
      return -1;
    }
    pPtrIn++;
  }
  int wValue=0;
  int wDigit=0;
  int wDigNb=0;
  while (isdigit(*pPtrIn) && (wDigNb < pMaxDigits) && *pPtrIn) {
    wDigit = int(*pPtrIn) - '0';
    wValue = (wValue * 10) + wDigit;
    pPtrIn++;
    wDigNb++;
  }
  if (!*pPtrIn)
    pEnd=1;
  return wValue;
}



/** 30-12 is a valid date -> gives 30-12-currentyear current time */
ZDateFull
ZDateFull::fromDMY(const utf8VaryingString &pString)
{
  ZDateFull wD;
  time_t wTimet=0;
  std::time(&wTimet);
  struct tm wTm;
  wD.Precision = ZDTPR_invalid;

  localtime_r(&wTimet,&wTm);

  int wEnd=0;
  int wValue;
  if (pString.isEmpty())
    return ZDateFull();


  const unsigned char* wPtr=pString.Data;
  wTm.tm_mday = getDateValue(wPtr,wEnd,2);
  if (wTm.tm_mday < 0)
    return ZDateFull(); /* return invalid date */

  wD.Precision = ZDTPR_ymd;

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD.tv_sec = mktime(&wTm);
    wD.tv_nsec = 0;
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wTm.tm_mon = wValue - 1 ;


  wValue = getDateValue(wPtr,wEnd,4);
  if (wValue < 0) {
    wD.tv_sec = mktime(&wTm);
    wD.tv_nsec = 0;
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wTm.tm_year = wValue - 1900;


  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD.tv_sec = mktime(&wTm);
    wD.tv_nsec = 0;
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Precision = ZDTPR_ymdh;
  wTm.tm_hour = wValue;

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD.tv_sec = mktime(&wTm);
    wD.tv_nsec = 0;
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Precision = ZDTPR_ymdhm;
  wTm.tm_min = wValue;

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD.tv_sec = mktime(&wTm);
    wD.tv_nsec = 0;
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Precision = ZDTPR_ymdhms;
  wTm.tm_sec = wValue;

  wD.tv_sec = mktime(&wTm);
  wD.tv_nsec = 0;

  return(wD);
}  // fromDMY

void
ZDateFull::_fromDMY(const utf8VaryingString &pString)
{
  _copyFrom(ZDateFull::fromDMY(pString));
  return;
}  // fromDMY

/** 12-30 is a valid date -> gives 12-30-currentyear current time */
ZDateFull
ZDateFull::fromMDY(const utf8VaryingString &pString)
{
  ZDateFull wD;
  time_t wTimet=0;
  std::time(&wTimet);
  struct tm wTm;

  wD.Precision = ZDTPR_invalid;

  localtime_r(&wTimet,&wTm);

  int wEnd=0;
  int wValue;
  if (pString.isEmpty())
    return ZDateFull();


  //  wD.getCurrentDateTime();  /* initialize with current date time */

  const unsigned char* wPtr=pString.Data;
  wTm.tm_mon = getDateValue(wPtr,wEnd,2);
  if (wTm.tm_mon < 0)
    return ZDateFull(); /* return invalid date */

  wD.Precision = ZDTPR_ymd;  /* a partial date has ymd precision */

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD.tv_sec = mktime(&wTm);
    wD.tv_nsec = 0;
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wTm.tm_mday = wValue - 1 ;


  wValue = getDateValue(wPtr,wEnd,4);
  if (wValue < 0) {
    wD.tv_sec = mktime(&wTm);
    wD.tv_nsec = 0;
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wTm.tm_year = wValue - 1900;


  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD.tv_sec = mktime(&wTm);
    wD.tv_nsec = 0;
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Precision = ZDTPR_ymdh;
  wTm.tm_hour = wValue;

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD.tv_sec = mktime(&wTm);
    wD.tv_nsec = 0;
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Precision = ZDTPR_ymdhm;
  wTm.tm_min = wValue;

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD.tv_sec = mktime(&wTm);
    wD.tv_nsec = 0;
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Precision = ZDTPR_ymdhms;
  wTm.tm_sec = wValue;

  wD.tv_sec = mktime(&wTm);
  wD.tv_nsec = 0;

  return(wD);
}  // fromMDY

void
ZDateFull::_fromMDY(const utf8VaryingString &pString)
{
  _copyFrom(ZDateFull::fromMDY(pString));
  return;
}  // fromDMY

ZDateFull
ZDateFull::fromTimespec(timespec &pTimespec)
{
  tv_sec=pTimespec.tv_sec;
  tv_nsec = pTimespec.tv_nsec;
  return (*this) ;
}


timespec
ZDateFull::toTimespec(void)
{
    timespec wTS ;
    wTS.tv_sec=tv_sec;
    wTS.tv_nsec=tv_nsec;
    return wTS;
}


#include <time.h>
//#include <ctime>
ZDateFull
ZDateFull::currentDateTime(void)
{
ZDateFull wDT;

    wDT._fromTime_t(std::time(nullptr));

    return wDT;
}

ZDateFull
ZDateFull::now()
{
  ZDateFull wDT;

  wDT._fromTime_t(std::time(nullptr));

  return wDT;
}


int
ZDateFull::weekDay() {
  if (isInvalid())
    return -1;
  tm* wT=localtime (&tv_sec);
  return wT->tm_wday;
}

int
ZDateFull::yearDay() {
  if (isInvalid())
    return -1;
  tm* wT=localtime (&tv_sec);
  return wT->tm_yday;
}


void
ZDateFull::_fromTime_t(time_t pTime)
{
  tv_sec = pTime;
  tv_nsec= 0;
  return;

}// _fromTime_t

time_t
ZDateFull::_toTime_t() {
  return tv_sec;
}
/*
ZDateFull&
ZDateFull::_fromInternal()
{
  struct tm wTm;
  localtime_r(&tv_sec,&wTm);
  Year = uint16_t(wTm.tm_year + 1900);
  Month = uint8_t(wTm.tm_mon + 1);
  Day = uint8_t(wTm.tm_mday) ;

  Hour = uint8_t(wTm.tm_hour) ;
  Min = uint8_t(wTm.tm_min) ;
  Sec = uint8_t(wTm.tm_sec) ;
  return *this;
}// _fromInternal
*/
/*
ZDateFull&
ZDateFull::_fromInternal(tm & pTm)
{
    Year = uint16_t(pTm.tm_year + 1900);
    Month = uint8_t(pTm.tm_mon + 1);
    Day = uint8_t(pTm.tm_mday) ;

    Hour = uint8_t(pTm.tm_hour) ;
    Min = uint8_t(pTm.tm_min) ;
    Sec = uint8_t(pTm.tm_sec) ;
    return *this;
}// _fromInternal
*/

void
ZDateFull::_toInternal(tm &pTm)
{
    tv_sec = mktime(&pTm); /* store time as local time */
    tv_nsec = 0;
    return ;
} // _toInternal


/**
 * @brief ZDateFull::toFormatted returns a varying string with  a formatted full date according a format description.
 *
 * Pre-defined formats :
 * - FMTLocale   "%c" date and time according locale format
 * - FMTStraight "%d/%m/%y-%H:%M:%S"   gives <31/05/14-22:05:10>
 * - FMTLong     "%d-%b-%Y-%I:%M:%S %p" gives <31-may-2014-10:05:10 pm>
 * - FMTDateOnly "%d/%m/%y"     gives <31/05/14>
 * - FMTTimeOnly "%H:%M:%S"     give <22:05:10>
 *
 * Format allows following options
 *
       %a     The abbreviated name of the day of the week according to the
              current locale.  (Calculated from tm_wday.)

       %A     The full name of the day of the week according to the current
              locale.  (Calculated from tm_wday.)

       %b     The abbreviated month name according to the current locale.
              (Calculated from tm_mon.)

       %B     The full month name according to the current locale.
              (Calculated from tm_mon.)

       %c     The preferred date and time representation for the current
              locale.

       %C     The century number (year/100) as a 2-digit integer. (SU)
              (Calculated from tm_year.)

       %d     The day of the month as a decimal number (range 01 to 31).
              (Calculated from tm_mday.)

       %D     Equivalent to %m/%d/%y.  (Yecch—for Americans only.  Americans
              should note that in other countries %d/%m/%y is rather common.
              This means that in international context this format is
              ambiguous and should not be used.) (SU)

       %e     Like %d, the day of the month as a decimal number, but a
              leading zero is replaced by a space. (SU) (Calculated from
              tm_mday.)

       %E     Modifier: use alternative format, see below. (SU)

       %F     Equivalent to %Y-%m-%d (the ISO 8601 date format). (C99)

       %G     The ISO 8601 week-based year (see NOTES) with century as a
              decimal number.  The 4-digit year corresponding to the ISO
              week number (see %V).  This has the same format and value as
              %Y, except that if the ISO week number belongs to the previous
              or next year, that year is used instead. (TZ) (Calculated from
              tm_year, tm_yday, and tm_wday.)

       %g     Like %G, but without century, that is, with a 2-digit year
              (00-99). (TZ) (Calculated from tm_year, tm_yday, and tm_wday.)

       %h     Equivalent to %b.  (SU)

       %H     The hour as a decimal number using a 24-hour clock (range 00
              to 23).  (Calculated from tm_hour.)

       %I     The hour as a decimal number using a 12-hour clock (range 01
              to 12).  (Calculated from tm_hour.)

       %j     The day of the year as a decimal number (range 001 to 366).
              (Calculated from tm_yday.)

       %k     The hour (24-hour clock) as a decimal number (range 0 to 23);
              single digits are preceded by a blank.  (See also %H.)
              (Calculated from tm_hour.)  (TZ)

       %l     The hour (12-hour clock) as a decimal number (range 1 to 12);
              single digits are preceded by a blank.  (See also %I.)
              (Calculated from tm_hour.)  (TZ)

       %m     The month as a decimal number (range 01 to 12).  (Calculated
              from tm_mon.)

       %M     The minute as a decimal number (range 00 to 59).  (Calculated
              from tm_min.)

       %n     A newline character. (SU)

       %O     Modifier: use alternative format, see below. (SU)

       %p     Either "AM" or "PM" according to the given time value, or the
              corresponding strings for the current locale.  Noon is treated
              as "PM" and midnight as "AM".  (Calculated from tm_hour.)

       %P     Like %p but in lowercase: "am" or "pm" or a corresponding
              string for the current locale.  (Calculated from tm_hour.)
              (GNU)

       %r     The time in a.m. or p.m. notation.  In the POSIX locale this
              is equivalent to %I:%M:%S %p.  (SU)

       %R     The time in 24-hour notation (%H:%M).  (SU) For a version
              including the seconds, see %T below.

       %s     The number of seconds since the Epoch, 1970-01-01 00:00:00
              +0000 (UTC). (TZ) (Calculated from mktime(tm).)

       %S     The second as a decimal number (range 00 to 60).  (The range
              is up to 60 to allow for occasional leap seconds.)
              (Calculated from tm_sec.)

       %t     A tab character. (SU)

       %T     The time in 24-hour notation (%H:%M:%S).  (SU)

       %u     The day of the week as a decimal, range 1 to 7, Monday being
              1.  See also %w.  (Calculated from tm_wday.)  (SU)

       %U     The week number of the current year as a decimal number, range
              00 to 53, starting with the first Sunday as the first day of
              week 01.  See also %V and %W.  (Calculated from tm_yday and
              tm_wday.)

       %V     The ISO 8601 week number (see NOTES) of the current year as a
              decimal number, range 01 to 53, where week 1 is the first week
              that has at least 4 days in the new year.  See also %U and %W.
              (Calculated from tm_year, tm_yday, and tm_wday.)  (SU)

       %w     The day of the week as a decimal, range 0 to 6, Sunday being
              0.  See also %u.  (Calculated from tm_wday.)

       %W     The week number of the current year as a decimal number, range
              00 to 53, starting with the first Monday as the first day of
              week 01.  (Calculated from tm_yday and tm_wday.)

       %x     The preferred date representation for the current locale
              without the time.

       %X     The preferred time representation for the current locale
              without the date.

       %y     The year as a decimal number without a century (range 00 to
              99).  (Calculated from tm_year)

       %Y     The year as a decimal number including the century.
              (Calculated from tm_year)

       %z     The +hhmm or -hhmm numeric timezone (that is, the hour and
              minute offset from UTC). (SU)

       %Z     The timezone name or abbreviation.

       %+     The date and time in date(1) format. (TZ) (Not supported in
              glibc2.)

       %%     A literal '%' character.

Between the '%' character and the
       conversion specifier character, an optional flag and field width may
       be specified.  (These precede the E or O modifiers, if present.)

       The following flag characters are permitted:

       _      (underscore) Pad a numeric result string with spaces.

       -      (dash) Do not pad a numeric result string.

       0      Pad a numeric result string with zeros even if the conversion
              specifier character uses space-padding by default.

       ^      Convert alphabetic characters in result string to uppercase.

       #      Swap the case of the result string.

 * @param pFormat
 * @return
 */

utf8VaryingString
ZDateFull::toFormatted(const char* pFormat)
{
  if (isInvalid())
    return "<invalid full date>";
  char wBuffer[100];

  time_t wT = tv_sec;

  strftime(wBuffer,sizeof(wBuffer),pFormat,std::localtime(&wT));

  return utf8VaryingString(wBuffer) ;
}//toFormatted


utf8VaryingString
ZDateFull::toLocale(void)
{
  if (isInvalid())
    return "<invalid full date>";
  return toFormatted("%c");

}//toLocale





//-----------reverse conversion----------------

/* Deprecated
ZDate ZDateFull::toZDate(void)
{
  if (isInvalid())
    return ZDate();

  ZDate wDate ;
  wDate.fromZDateFull(*this);

  return  wDate;
}
*/


/* UTC conversions */
utf8VaryingString ZDateFull::toXmlValue() const
{
  if (isInvalid())
    return "<!-- invalid date -->";
  struct tm wTm;
  if (gmtime_r(&tv_sec,&wTm)==nullptr)
    return "<!-- invalid date -->";

  utf8VaryingString wUTC;
  wUTC.sprintf("%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", wTm.tm_year+1900, wTm.tm_mon+1, wTm.tm_mday, wTm.tm_hour, wTm.tm_min, wTm.tm_sec, 0);
  return wUTC;
}

utf8VaryingString ZDateFull::toUTCGMT() const
{
  if (isInvalid())
    return "<invalid date>";
  struct tm wTm;
  if (gmtime_r(&tv_sec,&wTm)==nullptr)
    return "<invalid date>";

  utf8VaryingString wUTC;
  wUTC.sprintf("%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", wTm.tm_year+1900, wTm.tm_mon+1, wTm.tm_mday, wTm.tm_hour, wTm.tm_min, wTm.tm_sec, 0);
  return wUTC;
}

utf8VaryingString ZDateFull::toUTC() const
{
  if (isInvalid())
    return "<invalid date>";
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return "<invalid date>";
  long wGmtOffset = wTm.tm_gmtoff;
  utf8VaryingString wUTC ,wTZTrail;

  while (true) {
    if (wGmtOffset==0){
      wTZTrail = "Z";
      break;
    }
    if (wGmtOffset<0){
      wTZTrail = "-";
      wGmtOffset = -wGmtOffset;
    }
    else {
      wTZTrail = "+";
    }
    long wTZHour = wGmtOffset / 3600L;
    long wTZMin =  wGmtOffset  - ( wTZHour * 3600L );
    wTZMin = wTZMin / 60;
    wTZTrail.addsprintf("%2ld.%2ld",wTZHour,wTZMin);
    break;
  }// while true
  wUTC.sprintf("%04d-%02d-%02dT%02d:%02d:%02d.%03%s", wTm.tm_year+1900, wTm.tm_mon+1, wTm.tm_mday, wTm.tm_hour, wTm.tm_min, wTm.tm_sec,0,
      wTZTrail.toString());
  return wUTC;
} //toUTC


utf8VaryingString ZDateFull::toDMYhms() const
{
  if (isInvalid())
    return "<invalid full date>";
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return "<invalid date>";
  utf8VaryingString wLocalTime;
  wLocalTime.sprintf("%02d-%02d-%04d %02d:%02d:%02d-%03d",   wTm.tm_mday,wTm.tm_mon+1, wTm.tm_year+1900,wTm.tm_hour, wTm.tm_min, wTm.tm_sec, 0);
  return wLocalTime;
}
utf8VaryingString ZDateFull::toDMY() const
{
  if (isInvalid())
    return "<invalid full date>";
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return "<invalid date>";
  utf8VaryingString wLocalTime;
  wLocalTime.sprintf("%02d-%02d-%04d",   wTm.tm_mday,wTm.tm_mon+1, wTm.tm_year+1900);
  return wLocalTime;
}
utf8VaryingString ZDateFull::toMDY() const
{
  if (isInvalid())
    return "<invalid full date>";
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return "<invalid date>";
  utf8VaryingString wLocalTime;
  wLocalTime.sprintf("%02d-%02d-%04d",   wTm.tm_mon+1, wTm.tm_mday,wTm.tm_year+1900);
  return wLocalTime;
}

int ZDateFull::year() {
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return -1;
  return wTm.tm_year+1900;
}
int ZDateFull::month() {
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return -1;
  return wTm.tm_mon+1;
}
int ZDateFull::day() {
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return -1;
  return wTm.tm_mday;
}
int ZDateFull::weekday() {
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return -1;
  return wTm.tm_wday;
}
int ZDateFull::hour() {
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return -1;
  return wTm.tm_hour;
}
int ZDateFull::min() {
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return -1;
  return wTm.tm_min;
}
int ZDateFull::sec() {
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return -1;
  return wTm.tm_sec;
}

utf8VaryingString ZDateFull::toMDYhms() const
{
  if (isInvalid())
    return "<invalid full date>";
  struct tm wTm;
  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return "<invalid date>";
  utf8VaryingString wLocalTime;
  wLocalTime.sprintf("%02d-%02d-%04d %02d:%02d:%02d.%03d",   wTm.tm_mon+1,wTm.tm_mday, wTm.tm_year+1900,wTm.tm_hour, wTm.tm_min, wTm.tm_sec, 0);
  return wLocalTime;
}

/*  gmt "08-11-2011 07:07:09.000Z"
   *  non gmt  "08-11-2011 07:07:09.000+02.00" or "08-11-2011 07:07:09.000+02" */
void ZDateFull::fromUTC(const utf8VaryingString &pIn)
{
  ZSuperTm wSTm;
  int wNanoSec=0;
  int wTZHour=0;
  int wTZMin=0;

  int wTZSign = 1;

  clear();
  struct tm wTm, wTm1;
  memset(&wTm,0,sizeof(tm));
  memset(&wTm1,0,sizeof(tm));
  /*  if (localtime_r(&tv_sec,&wTm)==nullptr)
    return ;
*/
  int wNumber=0;
  int wCount=0;
  utf8_t* wPtrIn=(utf8_t*)pIn.Data;

  while (true) {
    /* skip leading bullshit */
    while (*wPtrIn && !std::isdigit(*wPtrIn))
      wPtrIn++;


    while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 4)) {
      int wDigit = int((*wPtrIn) -'0');
      wNumber = wNumber * 10;
      wNumber += wDigit;
      wCount++;
      wPtrIn++;
    }
    wSTm.tm_year = wNumber - 1900;

    /* skip bullshit again if any */
    while (*wPtrIn && !std::isdigit(*wPtrIn))
      wPtrIn++;

    wCount=0;
    wNumber=0;
    while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 2)) {
      int wDigit = int((*wPtrIn) -'0');
      wNumber = wNumber * 10;
      wNumber += wDigit;
      wCount++;
      wPtrIn++;
    }

    wSTm.tm_mon = wNumber - 1;

    /* skip bullshit again if any */
    while (*wPtrIn && !std::isdigit(*wPtrIn))
      wPtrIn++;

    wCount=0;
    wNumber=0;
    while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 2)) {
      int wDigit = int((*wPtrIn) -'0');
      wNumber = wNumber * 10;
      wNumber += wDigit;
      wCount++;
      wPtrIn++;
    }

    wSTm.tm_mday = wNumber ;

    /* repeat for hours */
    while (*wPtrIn && !std::isdigit(*wPtrIn))
      wPtrIn++;

    wCount=0;
    wNumber=0;
    while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 2)) {
      int wDigit = int((*wPtrIn) -'0');
      wNumber = wNumber * 10;
      wNumber += wDigit;
      wCount++;
      wPtrIn++;
    }

    wSTm.tm_hour = wNumber;

    /* repeat for minutes */
    while (*wPtrIn && !std::isdigit(*wPtrIn))
      wPtrIn++;

    wCount=0;
    wNumber=0;
    while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 2)) {
      int wDigit = int((*wPtrIn) -'0');
      wNumber = wNumber * 10;
      wNumber += wDigit;
      wCount++;
      wPtrIn++;
    }

    wSTm.tm_min = wNumber;

    /* allowed to truncate seconds */
    if ((!*wPtrIn) || (*wPtrIn=='Z') || (*wPtrIn=='+') || (*wPtrIn=='-'))
      break;/* get to trailing */

    /* repeat for seconds */
    while (*wPtrIn && !std::isdigit(*wPtrIn))
      wPtrIn++;

    wCount=0;
    wNumber=0;
    while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 2)) {
      int wDigit = int((*wPtrIn) -'0');
      wNumber = wNumber * 10;
      wNumber += wDigit;
      wCount++;
      wPtrIn++;
    }
    wSTm.tm_sec = wNumber;

    /* allowed to truncate nano seconds */
    if ((!*wPtrIn) || (*wPtrIn=='Z') || (*wPtrIn=='+') || (*wPtrIn=='-'))
      break;/* get to trailing */

    /* repeat for nano-seconds */
    while (*wPtrIn && !std::isdigit(*wPtrIn)) {
      wPtrIn++;
    }
    wCount=0;
    wNumber=0;
    while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 3)) {
      int wDigit = int((*wPtrIn) -'0');
      wNumber = wNumber * 10;
      wNumber += wDigit;
      wCount++;
      wPtrIn++;
    }
    wNanoSec = wNumber;
    break;
  }// main while true



//  long wTZDiff = getTimeZoneDiff();

  while (*wPtrIn!=0) {

    if (*wPtrIn=='Z') {
//      wTZDiff = getTimeZoneDiff();
      break;
    }
    else if (*wPtrIn=='+') {
      wTZSign = 1;
    }
    else if (*wPtrIn=='-') {
      wTZSign = -1;
    }
    else
      break;
    wPtrIn++;
    /* time zone difference for hours */
    wNumber=0;
    while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 2)) {
      int wDigit = int((*wPtrIn) -'0');
      wNumber = wNumber * 10;
      wNumber += wDigit;
      wCount++;
      wPtrIn++;
    }
    wTZHour=wNumber;
    /* time zone difference for seconds */
    wNumber=0;
    while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 2)) {
      int wDigit = int((*wPtrIn) -'0');
      wNumber = wNumber * 10;
      wNumber += wDigit;
      wCount++;
      wPtrIn++;
    }
    wTZMin = wNumber;


    break;
  } // while (*wPtrIn!=0)


  wSTm.tm_hour += (wTZSign * wTZHour) ;
  wSTm.tm_min += (wTZSign * wTZMin) ;

  tv_sec= timegm(&wSTm);      /* get wTimet1 as gm time and format wSTm as gmt with current time */
  tv_nsec = wNanoSec;

  return;
}

void ZDateFull::fromXmlValue(const utf8VaryingString &pIn)
{
  fromUTC(pIn);
}


int getDateValue(utf8_t* &wPtr,int pMaxDigits,char pSep) {

  if (!*wPtr)
    return -1;

  int wDigitNb=0,wDigitValue=0,wValue=0,w10=0;
//  while (*wPtr && (*wPtr != pSep) && (*wPtr != '-')) {
  while (*wPtr ) {
    if (wDigitNb == pMaxDigits)
      return -1;
    if ((*wPtr>='0')&&((*wPtr<='9'))) {
      wDigitValue = int(*wPtr - '0');
      wValue = (wValue * 10) + wDigitValue ;
//      w10++;
      wDigitNb++;
      wPtr++;
 //     continue;
    }
  if (!*wPtr)
    return wValue;
  if (*wPtr==pSep) { /* separator could be also '-' by default */
      wPtr++;
      return wValue;
    }

  } // while

  return wValue;  /* end of string has been reached : it is a valid separator (e. g. for year) */
}// getDateValue


/* [d]d{/|-}[m]m{/|-}[yy]yy[-hh:mm:ss] */
ZStatus
ZDateFull::checkDMY(const utf8VaryingString& pDateLiteral , tm& pTt, uint8_t &pFed) {
  int wDay=0,wMonth=0,wYear=0,wHour=0,wMin=0,wSec=0;

  utf8_t* wPtr = pDateLiteral.Data;

  wDay=getDateValue(wPtr,2,'/');
  if (wDay<1)
    return ZS_INV_LITERAL;

  wMonth=getDateValue(wPtr,2,'/');
  if (wMonth<1)
    return ZS_INV_LITERAL;

  wYear=getDateValue(wPtr,4,'-');
  if (wYear<1)
    return ZS_INV_LITERAL;
  pFed |= ZDTF_DMA;

  while (true) {
    wHour=getDateValue(wPtr,2,':');
    if (wHour<1) {
      wHour=0;
      break;
    }
    if (wHour>24)
      return ZS_INV_LITERAL;
    pFed |= ZDTF_HH;


    wMin=getDateValue(wPtr,2,':');
    if (wMin<1) {
      wMin=0;
      break;
    }
    if (wMin>60)
      return ZS_INV_LITERAL;
    pFed |= ZDTF_MM;

    wSec=getDateValue(wPtr,2,'\0');
    if (wSec<1) {
      wSec=0;
      break;
    }
    if (wMin>60)
      return ZS_INV_LITERAL;
    pFed |= ZDTF_SS;

    break;
  }// while true
  /* check values */
  if (wMonth > 12)
    return ZS_INV_LITERAL;
  if (wYear < 1900)
    return ZS_INV_LITERAL;

  if (wDay > DayMonth[wMonth-1])
    return ZS_INV_LITERAL;

  memset(&pTt,0,sizeof(tm));

  pTt.tm_mday=wDay;
  pTt.tm_mon=wMonth-1;
  pTt.tm_year=wYear - 1900;

  pTt.tm_hour=wHour;
  pTt.tm_min=wMin;
  pTt.tm_sec=wSec;

  return ZS_SUCCESS;
} //checkDMY


/* [m]m{/|-}[d]d{/|-}[yy]yy[-hh:mm:ss] */
ZStatus
ZDateFull::checkMDY(const utf8VaryingString& pDateLiteral , tm& pTt ,uint8_t &pFed) {
  int wDay=0,wMonth=0,wYear=0,wHour=0,wMin=0,wSec=0;
  pFed=ZDTF_Nothing;
  utf8_t* wPtr = pDateLiteral.Data;

  wMonth=getDateValue(wPtr,2,'/');
  if (wMonth<1)
    return ZS_INV_LITERAL;

  wDay=getDateValue(wPtr,2,'/');
  if (wDay<1)
    return ZS_INV_LITERAL;


  wYear=getDateValue(wPtr,4,'-');
  if (wYear<1)
    return ZS_INV_LITERAL;

  pFed |= ZDTF_DMA;

  while (true) {
    wHour=getDateValue(wPtr,2,':');
    if (wHour<1) {
      wHour=0;
      break;
    }
    if (wHour>24)
      return ZS_INV_LITERAL;
    pFed |= ZDTF_HH;


    wMin=getDateValue(wPtr,2,':');
    if (wMin<1) {
      wMin=0;
      break;
    }
    if (wMin>60)
      return ZS_INV_LITERAL;
    pFed |= ZDTF_MM;

    wSec=getDateValue(wPtr,2,'\0');
    if (wSec<1) {
      wSec=0;
      break;
    }
    if (wMin>60)
      return ZS_INV_LITERAL;
    pFed |= ZDTF_SS;

    break;
  }// while true
  /* check values */
  if (wMonth > 12)
    return ZS_INV_LITERAL;
  if (wYear < 1900)
    return ZS_INV_LITERAL;

  if (wDay > DayMonth[wMonth-1])
    return ZS_INV_LITERAL;

  memset(&pTt,0,sizeof(tm));

  pTt.tm_mday=wDay;
  pTt.tm_mon=wMonth-1;
  pTt.tm_year=wYear - 1900;

  pTt.tm_hour=wHour;
  pTt.tm_min=wMin;
  pTt.tm_sec=wSec;

  return ZS_SUCCESS;
} //checkMDY


const char* decode_ZDatePrecision(ZDatePrecision pCode)
{
  switch (pCode)
  {
  case ZDTPR_nothing:
    return "ZDTPR_nothing";
  case ZDTPR_invalid:
    return "ZDTPR_invalid";
  case ZDTPR_ymd:
    return "ZDTPR_ymd";
  case ZDTPR_ymdh:
    return "ZDTPR_ymdh";
  case ZDTPR_ymdhm:
    return "ZDTPR_ymdhm";
  case ZDTPR_ymdhms:
    return "ZDTPR_ymdhms";
  case ZDTPR_nano:
    return "ZDTPR_nano";
  }
}
