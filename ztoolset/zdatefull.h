#ifndef ZDATEFULL_H
#define ZDATEFULL_H

#include <ztoolset/ztime.h>

#include <ctime>
#include <time.h>

#include <ztoolset/zutfstrings.h>  //for utf8VaryingString

extern const char* ZDateFormat_Standard      ;
extern const char *ZTimeFormat_Standard       ;
extern const char *ZDateTimeFormat_Standard   ;


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

class ZDate;

class ZDateFull //: public tm
{
public:
  uint16_t Year;
  uint8_t Month;
  uint8_t Day;
  uint8_t Hour;
  uint8_t Min;
  uint8_t Sec;

 // typedef ZDateFull::tm _Base;
 //                             1   2     3   4     5   6     7   8     9   10    11  12
  const int DayMonth [12] = {  31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 };

  ZDateFull() ;
  ZDateFull(const ZDateFull &pIn) { _copyFrom(pIn); }
  ZDateFull(const ZDateFull &&pIn) { _copyFrom(pIn); }
  ZDateFull(const utf8VaryingString& pDate) {_fromDMY(pDate);}

  bool dateControl(){
    /* day month year control */
    if (Year<1900)
      return false;
    if (Month < 1)
      return false;
    if (Month > 12)
      return false;
    if (Day < 1)
      return false;

    if (Day > DayMonth[Month-1] )
      return false;

    if (Month == 2) {
      if (Year - (int(Year/4)*4)) {
        if (Day > 29)
          return false;
      }
      else {
        if (Day > 28)
          return false;
      }
    }
    return true;
  }

  bool timeControl(){
    if (Hour > 24)
      return false;
    if (Min > 60)
      return false;
    if (Hour > 24)
      return false;
    if (Sec > 60)
      return false;
    return true;
  }

  bool isInvalid() {
    return (dateControl() && timeControl());
  }

  void setInvalid() {
    memset(this, 0, sizeof(ZDateFull));
  }

  ZDateFull &_copyFrom(const  ZDateFull &pIn);

  ZDateFull &operator=(const ZDateFull &pIn) { return _copyFrom(pIn); }
  ZDateFull &operator=(const ZDateFull &&pIn) { return _copyFrom(pIn); }

  struct tm& getBase(void) {return (struct tm&)*this;}


  static ZDateFull fromDMY(const utf8VaryingString &pString);
  static ZDateFull fromMDY(const utf8VaryingString &pString);


  void _fromDMY(const utf8VaryingString &pString);
  void _fromMDY(const utf8VaryingString &pString);

  /* export import */

  uint64_t _export(void) const ;  // export in universal format

  void _exportPtr(unsigned char* &pPtr) const {
    uint64_t wDE=_export();
    memmove(pPtr,&wDE,sizeof(uint64_t));
    pPtr += sizeof(uint64_t);
  }
  void _exportAppend(ZDataBuffer& pZDB) const {
    unsigned char* wPtr=pZDB.extend(sizeof(uint64_t));
    _exportPtr(wPtr);
  }

  void _import(const unsigned char* &pPtrIn) {
    uint64_t* wPtr=(uint64_t* )pPtrIn;
    _import(wPtr[0]);
    pPtrIn+=sizeof(uint64_t);
  }
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

  /* UTC ISO 8601 "2011-10-08T07:07:09.000Z" */
  utf8VaryingString toUTC() const;
  /*  "08-11-2011 07:07:09.000Z" */
  utf8VaryingString toDMY() const;
  /*  "11-08-2011 07:07:09.000Z" */
  utf8VaryingString toMDY() const;

  void fromUTC(const utf8VaryingString &pIn );

  void clear() {memset(this,0,sizeof(ZDateFull));}
  ZDateFull fromTimespec(timespec &pTimespec);
  timespec  toTimespec(void);

  static ZDateFull currentDateTime(void);
  static ZDateFull now();
  static ZDateFull fromDMY(const ZDateFull_string &pDate);

  ZDateFull& getCurrentDateTime(void) {return _copyFrom(currentDateTime());}

  int compareDMY(const ZDateFull& pDate ) const {
    if (Year - pDate.Year)
      return Year - pDate.Year;
    if (Month - pDate.Month)
      return (Month - pDate.Month);
    if (Day - pDate.Day)
      return (Day - pDate.Day);
    return 0;
  }
  int compare(const ZDateFull& pDate ) const {
    int wC=compareDMY(pDate);
    if (wC)
      return wC;
    if (Hour - pDate.Hour)
      return Hour - pDate.Hour;
    if (Min - pDate.Min)
      return (Min - pDate.Min);
    if (Sec - pDate.Sec)
      return (Sec - pDate.Sec);
    return 0;
  }

  bool operator == (const ZDateFull& pDt){return compare(pDt) == 0;}

  bool operator > (const ZDateFull& pDt){return compare(pDt) > 0;}
  bool operator < (const ZDateFull& pDt){return compare(pDt) < 0;}

#ifdef QT_CORE_LIB
  ZDateFull fromQDateTime (const QDateTime &pQDate);
  ZDateFull fromQDate (const QDate &pQDate);
  ZDateFull fromQString(const QString &pDate);
#endif // QT_CORE_LIB
//-----------reverse conversion----------------


#ifdef __DEPRECATED__

  QDateTime toQDateTime(void);

  utfdescString
  toDateFormatted(const char *pFormat=ZDateFormat_Standard);

  utfdescString
  toDateTimeFormatted(const char*pFormat=ZDateTimeFormat_Standard);

  utfdescString
  toTimeFormatted (const char * pFormat=ZTimeFormat_Standard);


  QDate toQDate(void);
  QTime toQTime(void);

  ZDateFull operator = (const QDateTime& pDateTime) {fromQDateTime(pDateTime);return (*this);}

#endif // __DEPRECATED__

  ZDate   toZDate(void);
  ZDateFull fromZDate(const ZDate &wDate);

  utf8VaryingString toLocale(void);
  utf8VaryingString toFormatted(const char* pFormat="%F %T");

  int weekDay();
  int yearDay();

  /** requested by ZMIndex (see zam.h and zmindex.h)*/
//  int compare(ZDateFull &pDC) const { return memcmp(this, &pDC, sizeof(ZDateFull)); }


private:
  //    struct tm DateInternal;
  tm _toInternal();
  ZDateFull& _fromInternal(tm &pTm);
  time_t _toTime_t();
  void _fromTime_t(time_t pTime_t);
};  // struct ZDateFull


#endif // ZDATEFULL_H
