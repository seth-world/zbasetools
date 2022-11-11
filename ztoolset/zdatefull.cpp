#include "zdatefull.h"

#include <ztoolset/zdate.h>

using namespace zbs;



ZDateFull::ZDateFull()
{
 memset(this, 0, sizeof(ZDateFull));
}


//--------------- ZDateFull-------------------------
//

ZDateFull &
ZDateFull::_copyFrom(const  ZDateFull &pIn)
{
  memset(this, 0, sizeof(ZDateFull));
  Year = pIn.Year;
  Month = pIn.Month;
  Day = pIn.Day;
  Hour = pIn.Hour;
  Min = pIn.Min;
  Sec = pIn.Sec;
  return *this;
}


uint64_t
ZDateFull::_export(void) const
{

    wuIn64 wuExport;
    wuExport.Year=reverseByteOrder_Conditional<uint16_t>(Year);
    wuExport.Month = Month;
    wuExport.Day = Day;
    wuExport.Hour = Hour;
    wuExport.Min = Min;
    wuExport.Sec = Sec;

    uint64_t wOut;
    memmove(&wOut,&wuExport,sizeof(uint64_t));
    return wOut;
}// _export


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
void
ZDateFull::_import(uint64_t pIDate)
{
  wuIn64 wX;
  memmove(&wX,&pIDate,sizeof(uint64_t));

  Year=reverseByteOrder_Conditional<uint16_t>(wX.Year);
  Month=wX.Month  ;
  Day=wX.Day;
  Hour=wX.Hour;
  Min=wX.Min;
  Sec=wX.Sec;
  return;
}// _import

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
  int wEnd=0;
  int wValue;
  if (pString.isEmpty())
    return ZDateFull();

  wD.getCurrentDateTime();  /* initialize with current date time */

  const unsigned char* wPtr=pString.Data;
  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0)
    return ZDateFull(); /* return invalid date */
  wD.Day = uint8_t(wValue);

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD._toInternal();
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Month = uint8_t(wValue);
  wValue = getDateValue(wPtr,wEnd,4);
  if (wValue < 0)
    return wD; /* return partial date. other fields are filled with current date time */
  wD.Year = uint16_t(wValue);

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD._toInternal();
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Hour = uint8_t(wValue);

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD._toInternal();
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Min = uint8_t(wValue);

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0) {
    wD._toInternal();
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Sec = uint8_t(wValue);

  wD._toInternal();

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
  int wEnd=0;
  int wValue;
  if (pString.isEmpty())
    return ZDateFull();

  wD.getCurrentDateTime();  /* initialize with current date time */

  const unsigned char* wPtr=pString.Data;

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0)
    return ZDateFull(); /* return invalid date */
  wD.Month = uint8_t(wValue);
  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0)
    return ZDateFull(); /* return invalid date */
  wD.Day = uint8_t(wValue);
  wValue = getDateValue(wPtr,wEnd,4);
  if (wValue < 0)
    return wD; /* return partial date. other fields are filled with current date time */
  wD.Year = uint16_t(wValue);

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0)
    return wD; /* return partial date. other fields are filled with current date time */
  wD.Hour = uint8_t(wValue);
  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0)
    return wD; /* return partial date. other fields are filled with current date time */
  wD.Min = uint8_t(wValue);
  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0)
    return wD; /* return partial date. other fields are filled with current date time */
  wD.Sec = uint8_t(wValue);

  wD._toInternal();

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
    memset (this,0,sizeof(ZDateFull));
    struct tm* wTm;
    wTm = localtime (&pTimespec.tv_sec);
//    memmove(&DateInternal,wTm,sizeof(tm));

    _fromInternal(*wTm);
    return (*this) ;
}


timespec
ZDateFull::toTimespec(void)
{
    timespec wTS ;
    wTS.tv_sec= mktime ((tm*)this);
    wTS.tv_nsec = 0;
    return wTS;
}
ZDateFull
ZDateFull::fromZDate(const ZDate& wDate)
{
  getCurrentDateTime();
  Year=wDate.Year;
  Month=wDate.Month;
  Day=wDate.Day;
  _toInternal();

  return (*this);
} //fromZDateString
#ifdef QT_CORE_LIB
ZDateFull
ZDateFull::fromQDateTime (const QDateTime& pQDate)
{
    Year = pQDate.date().year();
    Month = pQDate.date().month();
    Day = pQDate.date().day();
    Hour = pQDate.time().hour();
    Min = pQDate.time().minute();
    Sec = pQDate.time().second();

    _toInternal();
    return (*this) ;
}

ZDateFull
ZDateFull::fromQDate (const QDate &pQDate)
{
    memset(this,0,sizeof(ZDateFull));
    Year = pQDate.year();
    Month = pQDate.month();
    Day = pQDate.day();
    _toInternal();
    return (*this) ;
}
#endif// QT_CORE_LIB

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
  time_t wTime = _toTime_t();
  tm* wT=localtime (&wTime);

  return wT->tm_wday;
}

int
ZDateFull::yearDay() {
  if (isInvalid())
    return -1;
  time_t wTime = _toTime_t();
  tm* wT=localtime (&wTime);

  return wT->tm_yday;
}


void
ZDateFull::_fromTime_t(time_t pTime)
{
  struct tm * wTm= localtime (&pTime);
  if(wTm==nullptr) {
    setInvalid();
    return;
  }
  _fromInternal(*wTm);
}// _fromTime_t

time_t
ZDateFull::_toTime_t() {
  struct tm wT = _toInternal();
  return mktime(&wT);
}


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
}// _toInternal


tm
ZDateFull::_toInternal()
{
  tm pTm;
    pTm.tm_year=Year - 1900;
    pTm.tm_mon = int(Month - 1);
    pTm.tm_mday = int(Day) ;

    pTm.tm_hour= int(Hour) ;
    pTm.tm_min = int(Min) ;
    pTm.tm_sec = int(Sec) ;
    return pTm;
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
    tm wT;
    memset(&wT,0,sizeof(wT));

    char wBuffer[100];

    wT.tm_year = Year-1900;
    wT.tm_mon = Month -1 ;
    wT.tm_mday = Day;
    wT.tm_min = Min;
    wT.tm_hour=Hour;
    wT.tm_sec = Sec ;

    strftime(wBuffer,100,pFormat,&wT);

    return utf8VaryingString(wBuffer) ;
}//toFormatted


utf8VaryingString
ZDateFull::toLocale(void)
{
    return toFormatted("%c");

}//toLocale




#ifdef QT_CORE_LIB
ZDateFull ZDateFull::fromQString(const QString& pDate)
{
    ZDateFull_string wDFS;
    memset(&wDFS,0,sizeof(ZDateFull_string));
    size_t wi=pDate.size();
    if (wi>sizeof(ZDateFull_string))
                    wi=sizeof(ZDateFull_string);
        memmove(&wDFS,pDate.toUtf8(),wi);
    return(fromZDateFullString(wDFS));
}//fromQString
#endif // QT_CORE_LIB

//-----------reverse conversion----------------


ZDate ZDateFull::toZDate(void)
{
ZDate *wDate =(ZDate*) this;

    return  *wDate;
}



/* UTC conversions */

/* UTC ISO 8601 "2011-10-08T07:07:09.000Z" */
utf8VaryingString ZDate::toUTC() const
{
  utf8VaryingString wUTC;
  wUTC.sprintf("%04d-%02d-%02dT%02d:%02d:%02d-%03dZ", Year, Month, Day, 0, 0, 0, 0);
  return wUTC;
}
void ZDate::fromUTC(const utf8VaryingString &pIn)
{
  clear();

  clear();
  int wNumber=0;
  utf8_t* wPtrIn=(utf8_t*)pIn.Data;
  /* skip leading bullshit */
  while (*wPtrIn && !std::isdigit(*wPtrIn))
    wPtrIn++;

  int wCount=0;
  while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 4)) {
    int wDigit = int((*wPtrIn) -'0');
    wNumber = wNumber * 10;
    wNumber += wDigit;
    wCount++;
    wPtrIn++;
  }
  Year = uint16_t(wNumber);
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

  Month = uint8_t(wNumber);

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

  Day = uint8_t(wNumber);


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

  return ;

  /* hereafter is deprecated

  ssize_t wY = pIn.locate((const utf8_t *) "-");
  if (wY < 0) {
    fprintf(stderr, "ZDateFull::fromUTC-E_INVFMT Invalid UTC date format <%s>\n", pIn.toCChar());
    return;
  }
  Year = pIn.subString(0, wY).toInt(10);
  wY++;
  ssize_t wM = pIn.locate((const utf8_t *) "-", wY);
  if (wM < 0)
  {
    fprintf(stderr, "ZDateFull::fromUTC-E_INVFMT Invalid UTC date format <%s>\n", pIn.toCChar());
    return;
  }
  Month = pIn.subString(wY, wM - wY).toInt(10);
  wM++;
  ssize_t wD = pIn.locate((const utf8_t *) "T", wM);
  if (wD < 0)
  {
    fprintf(stderr, "ZDateFull::fromUTC-E_INVFMT Invalid UTC date format <%s>\n", pIn.toCChar());
    return;
  }
  Day = pIn.subString(wM, wD - wM).toInt(10);
*/

}

utf8VaryingString ZDateFull::toUTC() const
{
  utf8VaryingString wUTC;
  wUTC.sprintf("%04d-%02d-%02dT%02d:%02d:%02d-%03dZ", Year, Month, Day, Hour, Min, Sec, 0);
  return wUTC;
}
utf8VaryingString ZDateFull::toDMY() const
{
  utf8VaryingString wUTC;
  wUTC.sprintf("%02u-%02u-%04u %02u:%02u:%02u-%03u",   Month, Day,Year,Hour, Min, Sec, 0);
  return wUTC;
}
utf8VaryingString ZDateFull::toMDY() const
{
  utf8VaryingString wUTC;
  wUTC.sprintf("%02u-%02u-%04u %02u:%02u:%02u-%03u", Day,  Month, Year,Hour, Min, Sec, 0);
  return wUTC;
}
void ZDateFull::fromUTC(const utf8VaryingString &pIn)
{
  clear();
  int wNumber=0;
  utf8_t* wPtrIn=(utf8_t*)pIn.Data;
  /* skip leading bullshit */
  while (*wPtrIn && !std::isdigit(*wPtrIn))
    wPtrIn++;

  int wCount=0;
  while (*wPtrIn && std::isdigit(*wPtrIn) && (wCount < 4)) {
    int wDigit = int((*wPtrIn) -'0');
    wNumber = wNumber * 10;
    wNumber += wDigit;
    wCount++;
    wPtrIn++;
  }
  Year = uint16_t(wNumber);
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

  Month = uint8_t(wNumber);

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

  Day = uint8_t(wNumber);


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

  Hour = uint8_t(wNumber);

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

  Min = uint8_t(wNumber);

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

  Sec = uint8_t(wNumber);

  return;
}

#ifdef __DEPRECATED__

QDateTime ZDateFull::toQDateTime(void)
{
    QDateTime wQD ;
    const QDate wQD1(Year,Month,Day);
    const QTime wQT(Hour,Min,Sec);

    wQD.setDate(wQD1);
    wQD.setTime(wQT);
    return wQD ;
}

utfdescString
ZDateFull::toDateFormatted(const char *pFormat)
{
    utfdescString wRet;
    wRet.fromQString(toQDate().toString(pFormat)); /* todo : something more clever with that stuff see icu or system built-in capabilities*/
    return (wRet);
}
utfdescString
ZDateFull::toDateTimeFormatted(const char*pFormat)
{
    utfdescString wRet;
    wRet.fromQString(toQDateTime().toString(pFormat));
    return (wRet);
}
utfdescString
ZDateFull::toTimeFormatted (const char * pFormat)
{
    utfdescString wRet;
    wRet.fromQString(toQTime().toString(pFormat));
    return(wRet);
}
QDate ZDateFull::toQDate(void)
{
    QDate wQD ;
    wQD.setDate(Year,Month,Day);
    return wQD ;
}
QTime ZDateFull::toQTime(void)
{
    QTime wQD (Hour,Min,Sec);
    return wQD ;
}

#endif //#ifdef __DEPRECATED__

