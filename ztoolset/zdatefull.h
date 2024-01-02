#ifndef ZDATEFULL_H
#define ZDATEFULL_H

#include <ztoolset/ztime.h>

#include <ctime>
#include <time.h>

extern long int TimeZoneDiff;

#include <ztoolset/utfvaryingstring.h>  //for utf8VaryingString

extern const char* ZDateFormat_Standard      ;
extern const char *ZTimeFormat_Standard       ;
extern const char *ZDateTimeFormat_Standard   ;


//                              1   2     3   4     5   6     7   8     9   10    11  12
const int DayMonth [12] = {  31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 };

class ZSuperTm : public tm {
public:
  ZSuperTm(){memset (this,0,sizeof(ZSuperTm));}
  ZSuperTm(const ZSuperTm& pIn) { _copyFrom(pIn);}
  ZSuperTm(const struct tm& pIn) { _copyFrom(pIn);}

  tm get() {return (*this);}

  ZSuperTm& operator = (const ZSuperTm& pIn) {return _copyFrom(pIn);}
  ZSuperTm& operator = (const tm& pIn) {return _copyFrom(pIn);}

  ZSuperTm& _setOnlyBaseTime(const tm& pIn) {
    memset(this,0,sizeof(ZSuperTm));
    tm_year = pIn.tm_year;
    tm_mon = pIn.tm_mon;
    tm_mday = pIn.tm_mday;
    tm_hour = pIn.tm_hour;
    tm_min = pIn.tm_min;
    tm_sec = pIn.tm_sec;
    return *this;
  }

  ZSuperTm& _copyFrom(const ZSuperTm& pIn) {
    tm_year = pIn.tm_year;
    tm_mon = pIn.tm_mon;
    tm_mday = pIn.tm_mday;
    tm_hour = pIn.tm_hour;
    tm_min = pIn.tm_min;
    tm_sec = pIn.tm_sec;
    tm_wday = pIn.tm_wday;
    tm_yday = pIn.tm_yday;
    tm_isdst = pIn.tm_isdst;
    tm_gmtoff = pIn.tm_gmtoff;
    //    tm_zone = pIn.tm_zone;
    return *this;
  }

  ZSuperTm& _copyFrom(const struct tm& pIn) {
    tm_year = pIn.tm_year;
    tm_mon = pIn.tm_mon;
    tm_mday = pIn.tm_mday;
    tm_hour = pIn.tm_hour;
    tm_min = pIn.tm_min;
    tm_sec = pIn.tm_sec;
    tm_wday = pIn.tm_wday;
    tm_yday = pIn.tm_yday;
    tm_isdst = pIn.tm_isdst;
    tm_gmtoff = pIn.tm_gmtoff;
    //    tm_zone = pIn.tm_zone;
    return *this;
  }

};



struct ZDateFull_string {
  char Year[5];
  char Month[3];
  char Day[3];
  char Hour[3];
  char Min[3];
  char Sec[3];

};

#pragma pack(push)
#pragma pack(1)
class wuIn64 {
public:
  wuIn64() {}
  uint16_t Year;
  uint8_t  Month;
  uint8_t  Day;
  uint8_t  Hour;
  uint8_t  Min;
  uint8_t  Sec;
  uint8_t  Filler=0;
} ;
#pragma pack(pop)

//class ZDate;

enum ZDatePrecision {
  ZDTPR_invalid = -1,
  ZDTPR_nothing = 0,
  ZDTPR_ymd     = 1,
  ZDTPR_ymdh    = 2,
  ZDTPR_ymdhm   = 3,
  ZDTPR_ymdhms  = 4,
  ZDTPR_nano    = 5
};

const char* decode_ZDatePrecision(ZDatePrecision pCode);

class ZDateFull : public timespec
{
public:
  ZDatePrecision Precision = ZDTPR_nano;

  ZDateFull() ;
  ZDateFull(const ZDateFull &pIn) { _copyFrom(pIn); }
  ZDateFull(const ZDateFull &&pIn) { _copyFrom(pIn); }
  ZDateFull(const utf8VaryingString& pDate) {_fromDMY(pDate);}

  ZDatePrecision getPrecision() {return Precision;}

  bool isInvalid() const {
    return (tv_sec <= 0);
  }

  void setInvalid() {
    tv_sec = -1;
    tv_nsec = 0;
  }

  ZDateFull &_copyFrom(const  ZDateFull &pIn);

  ZDateFull &operator=(const ZDateFull &pIn) { return _copyFrom(pIn); }
  ZDateFull &operator=(const ZDateFull &&pIn) { return _copyFrom(pIn); }

  ZDateFull &operator=(const timespec &pIn)  {  tv_sec=pIn.tv_sec; tv_nsec = pIn.tv_nsec;  return *this;}

  bool operator == (const ZDateFull &pIn) const { return (tv_sec == pIn.tv_sec) ; }
//  bool operator == (const ZDateFull& pDt){return compare(pDt) == 0;}

  bool operator > (const ZDateFull& pDt) const {return tv_sec > pDt.tv_sec ;}
  bool operator >= (const ZDateFull& pDt) const {return tv_sec >= pDt.tv_sec ;}
  bool operator < (const ZDateFull& pDt) const {return tv_sec < pDt.tv_sec ;}
  bool operator <= (const ZDateFull& pDt) const {return tv_sec <= pDt.tv_sec ;}

  int _stringCompare(const utf8VaryingString& pDateString,ZDatePrecision & pOutPrecision) const {
    ZDateFull wD1=fromDMY(pDateString);
    if (wD1.Precision==ZDTPR_invalid){
      wD1=fromDMY(pDateString);
    }
    if (wD1.Precision==ZDTPR_invalid){
      return 1;
    }
    return _Compare(wD1,pOutPrecision);
  }

  /** tries to convert a string to a ZDateFull, best case using DMY then MDY algorithm.
   * according the data found and converted, returns pOutPrecision mentionning the level of data taken into account */
  static ZDateFull fromString(const utf8VaryingString& pDateString)  {
    ZDateFull wD1=fromDMY(pDateString);
    if (wD1.Precision==ZDTPR_invalid){
      wD1=fromDMY(pDateString);
    }
    if (wD1.Precision==ZDTPR_invalid){
      return ZDateFull();
    }
    return wD1;
  }

  int _Compare(const ZDateFull& pDate,const ZDatePrecision & pPrecision=ZDTPR_nano) const {
    if (pPrecision==ZDTPR_invalid)
      return 1;
    tm wComp1 = getLocalTime();
    tm wComp2 = pDate.getLocalTime();
    int wC =wComp1.tm_year - wComp2.tm_year;
    if (wC!=0)
      return wC;
    wC = wComp1.tm_mon - wComp2.tm_mon;
    if (wC!=0)
      return wC;
    wC = wComp1.tm_mday - wComp2.tm_mday;
    if (wC!=0)
      return wC;
    if (pPrecision==ZDTPR_ymd)
      return wC;

    wC = wComp1.tm_hour - wComp2.tm_hour;
    if (wC!=0)
      return wC;
    if (pPrecision==ZDTPR_ymdh)
      return wC;
    wC = wComp1.tm_min - wComp2.tm_min;
    if (wC!=0)
      return wC;
    if (pPrecision==ZDTPR_ymdhm)
      return wC;
    wC = wComp1.tm_sec - wComp2.tm_sec;
    return wC;
  }


  int nanoCompare(const ZDateFull& pDate ) const {
    if (tv_sec == pDate.tv_sec){
      return int(tv_nsec - pDate.tv_nsec);
    }
    return int(tv_sec - pDate.tv_sec);
  }
  int nanoIsEqual(const ZDateFull& pDate ) const { return nanoCompare(pDate)==0;}
  int nanoIsGreater(const ZDateFull& pDate ) const { return nanoCompare(pDate)>0;}
  int nanoIsLess(const ZDateFull& pDate ) const { return nanoCompare(pDate)<0;}

  int year();
  int month();
  int day();
  int weekday();
  int hour();
  int min();
  int sec();

  struct timespec getBase(void) {return (struct timespec&)*this;}
  struct tm getUTC(void) {
    struct tm wTm;

    gmtime_r(&tv_sec, &wTm);
    return wTm;
  }
  struct tm getLocalTime(void) const {
    struct tm wTm;

    localtime_r(&tv_sec, &wTm);
    return wTm;
  }



  /* NB; pOutPrecision gives the precision of the given string used to formulate the ZDateFull object :
   * Example : ZDTPR_ymd : year month and day have been specified. Hour min and sec are missing.
   * ZDTPR_ymdh means that in addition hours have been mentionned
   * ZDTPR_ymdhm means hours and minutes have been mentionned
   * ZDTPR_ymdhms means hours minutes and seconds have been mentionned
   *
   *
*/
  static ZDateFull fromDMY(const utf8VaryingString &pString);
  static ZDateFull fromMDY(const utf8VaryingString &pString);


  void _fromDMY(const utf8VaryingString &pString);
  void _fromMDY(const utf8VaryingString &pString);

  /* export import */

  uint64_t _export(void) const ;  // export in universal format

  void _exportPtr(unsigned char* &pPtr) const;
  void _exportAppend(ZDataBuffer& pZDB) const;

  void _import(const unsigned char* &pPtrIn) ;

  void          _import(uint64_t pIDate); // import from universal format
  ssize_t       _exportURF(ZDataBuffer& pZDB) const; // export in URF format
  ssize_t       _exportURF_Ptr(unsigned char* &pURF) const;

  size_t        getURFSize() const;
  size_t        getURFHeaderSize() const;
  size_t        getUniversalSize() const;
  ssize_t       _importURF(const unsigned char *&pZDB);     // import from URF format

  static ZStatus getUniversalFromURF(const unsigned char* pURFDataPtr, ZDataBuffer& pUniversal,const unsigned char **pURFDataPtrOut=nullptr); // extracts Universal value from URF data

  size_t getUniversal_Ptr(unsigned char * &pUniversalPtr) const;

  ZStatus getValueFromUniversal(const unsigned char *pUniversalDataPtr);

  /** @brief toUTCGMT() converts date value to gmt time and format it into a
   *
    UTC ISO 8601 "2011-10-08T07:07:09.000Z"

    where .000Z represents nano-seconds (value between '.' and 'Z')
    This is set be default to '.000Z' unless pNano is set to true.

    Z indicates Zulu time zone, meaning a gmt time

  */

  /* UTC time format
   *   gmt "08-11-2011 07:07:09.000Z"
   *  non gmt  "08-11-2011 07:07:09.000+02.00" or "08-11-2011 07:07:09.000+02.00" */

  /** @brief toUTC converts date content as local time in utf format with appropriate Time Zone difference trail.
   * @see fromUTC() Trailing Time Zone secton
 */
  utf8VaryingString toXmlValue() const;
  utf8VaryingString toUTC() const;

  /** @brief toUTCGMT converts first internal date-time content into GMT time then format it into UTC format */
  utf8VaryingString toUTCGMT() const;

  /*  "11-08-2011 07:07:09.000Z" */
  utf8VaryingString toDMYhms() const;
  /*  "08-11-2011 07:07:09.000Z" */
  utf8VaryingString toMDYhms() const;
  /*  "08-11-2011" */
  utf8VaryingString toDMY() const;
  /*  "11-08-2011" */
  utf8VaryingString toMDY() const;

  /** @brief fromUTC  gets a date-time from a UTC format, GMT or other.
   * Trailing time zone :
   * -------------------
   *  - if UTC time zone difference is missing then date-time is considered to be local time
   *  - if UTC time zone is mentionned :
   *    'Z' or +00.00 gives a pure gmt time
   *    +99.99 or -99.99  applies to given date as plus or minus hour(2 digits).minutes(2digits) to give gmt time
 */
  void fromUTC(const utf8VaryingString &pIn );
  void fromXmlValue(const utf8VaryingString &pIn );


  ZSuperTm getUTC(const utf8VaryingString &pIn );

  void clear() {memset(this,0,sizeof(ZDateFull));}
  ZDateFull fromTimespec(timespec &pTimespec);
  timespec  toTimespec(void);

  /**
   * @brief currentDateTime this static method returns a ZDateFull object with current date and time
   */
  static ZDateFull currentDateTime(void);
  /**  @brief now alias for currentDateTime()
   */
  static ZDateFull now();
  static ZDateFull fromDMY(const ZDateFull_string &pDate);

  ZDateFull& getCurrentDateTime(void) {return _copyFrom(currentDateTime());}

  /* input format [m]m{/|-}[d]d{/|-}[yy]yy[-hh:mm:ss] */

  static ZStatus checkMDY(const utf8VaryingString& pDateLiteral , tm &pTt , uint8_t &pFed);

  /* input format [d]d{/|-}[m]m{/|-}[yy]yy[-hh:mm:ss] */

  static ZStatus checkDMY(const utf8VaryingString& pDateLiteral , tm& pTt ,uint8_t &pFed);

//-----------reverse conversion----------------


  utf8VaryingString toLocale(void);
  utf8VaryingString toFormatted(const char* pFormat="%F %T");

  int weekDay();
  int yearDay();

  void _toInternal(tm &pTm);
  ZDateFull& _fromInternal();
  time_t _toTime_t();
  void _fromTime_t(time_t pTime_t);

};  // class ZDateFull

/* for gmt conversions */
long getTimeZoneDiff();
const char* getTimeZoneName ();
int getDST ();


enum ZDateFed : uint8_t
{
  ZDTF_Nothing    = 0,
  ZDTF_DMA        = 1,
  ZDTF_HH         = 2,
  ZDTF_MM         = 4,
  ZDTF_SS         = 8
};


/* [d]d{/|-}[m]m{/|-}[yy]yy[-hh:mm:ss] */
ZStatus checkDMY(const utf8VaryingString& pDateLiteral , tm* pTt, uint8_t &pFed);

/* [m]m{/|-}[d]d{/|-}[yy]yy[-hh:mm:ss] */
ZStatus checkMDY(const utf8VaryingString& pDateLiteral , tm* pTt ,uint8_t &pFed);

int getDateValue(utf8_t* &wPtr,int pMaxDigits,char pSep);

#endif // ZDATEFULL_H
