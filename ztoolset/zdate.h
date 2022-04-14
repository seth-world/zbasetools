#ifndef ZDATE_H
#define ZDATE_H

#include <ztoolset/ztime.h>
#include <ztoolset/ztimer.h>
#ifdef QT_CORE_LIB
#include <QDate>
#endif


//
//---------------Dates section---------------------
//


struct ZDateFull_string {
    char Year[5];
    char Month[3];
    char Day[3];
    char Hour[3];
    char Min[3];
    char Sec[3];

};

struct ZDate_string {
    char Year[5];
    char Month[3];
    char Day[3];

};
//typedef templateString<char [cst_datelen+1]>        dateString;
//typedef templateString<char [cst_fulldatelen+1]>    datefullString;
class ZDateFull;
#ifndef __UTFDESCSTRING__
class utfdescString;
#endif
class ZDate
{

public:
    uint16_t Year;
    uint8_t  Month;
    uint8_t  Day;
    ZDate() { memset(this, 0, sizeof(ZDate)); }
    ZDate(ZDate &pIn) { _copyFrom(pIn); }
    ZDate(ZDate &&pIn) { _copyFrom(pIn); }

    bool isInvalid() {
      uint8_t* wPtr=(uint8_t*)this;
      size_t wi=0;
      while ( wi < sizeof(ZDate) )
        if (wPtr[wi++])
          return false;
      return true;
    }
    void setInvalid() {
      memset(this, 0, sizeof(ZDate));
    }

    ZDate &_copyFrom(ZDate &pIn)
    {
        memset(this, 0, sizeof(ZDate));
        Year = pIn.Year;
        Month = pIn.Month;
        Day = pIn.Day;
        return *this;
    }
    ZDate &_copyFrom(ZDate &&pIn)
    {
        memset(this, 0, sizeof(ZDate));
        Year = pIn.Year;
        Month = pIn.Month;
        Day = pIn.Day;
        return *this;
    }
    ZDate &operator=(ZDate &pIn) { return _copyFrom(pIn); }
    ZDate &operator=(ZDate &&pIn) { return _copyFrom(pIn); }

static ZDate currentDate(void); //-----------Static get date-------------------

ZDate fromZDateString (ZDate_string &wDateS);
ZDate fromString(char* pDate);

uint32_t      _export(void) const;  // export in universal format
void          _import(uint32_t pIDate); // import from universal format
ZDataBuffer*  _exportURF(ZDataBuffer *pZDB) const; // export in URF format
size_t        _exportURF_Ptr(unsigned char *&pURF) const; // export in URF format
size_t        getURFSize() const ;
ZStatus       _importURF(const unsigned char *&pZDB);     // import from URF format
ZStatus getValueFromUniversal(const unsigned char* pUniversalDataPtr);

size_t getUniversal_Ptr(unsigned char * &pUniversalPtr) const;

static ZStatus getUniversalFromURF(const unsigned char* pURFDataPtr, ZDataBuffer& pUniversal,const  unsigned char **pURFDataPtrOut=nullptr); // extracts Universal value from URF data

ZDate fromZDateFull(const ZDateFull &wDateFull);
utfdescString toFormatted(const char* pFormat="%F");

#ifdef QT_CORE_LIB
#include <QDate>

ZDate fromQDate (QDate pQDate);
ZDate fromQString(QString pDate);

QDate toQDate(void);

#endif // QT_CORE_LIB

int compare(ZDate &pDC)const { return memcmp(this, &pDC, sizeof(ZDate)); }

bool operator < (ZDate &pDateFull) { return (memcmp (this,&pDateFull,sizeof(ZDate))<0);}
bool operator > (ZDate &pDateFull) { return (memcmp (this,&pDateFull,sizeof(ZDate))>0);}
bool operator <= (ZDate &pDateFull) { return !(memcmp (this,&pDateFull,sizeof(ZDate))>0);}
bool operator >= (ZDate &pDateFull) { return !(memcmp (this,&pDateFull,sizeof(ZDate))<0);}


//  editing format
private:
struct tm DateInternal;
    inline ZDate& _toInternal(void);
    inline ZDate& _fromInternal(void);
};//ZDate
#include <ctime>
#include <time.h>

extern const char ZDateFormat_Standard[]      ;
extern const char *ZTimeFormat_Standard       ;
extern const char *ZDateTimeFormat_Standard   ;

class ZDateFull: public tm
{
public:
    uint16_t Year;
    uint8_t Month;
    uint8_t Day;
    uint8_t Hour;
    uint8_t Min;
    uint8_t Sec;

    typedef ZDateFull::tm _Base;

    ZDateFull() { memset(this, 0, sizeof(ZDateFull)); }
    ZDateFull(const ZDateFull &pIn) { _copyFrom(pIn); }
    ZDateFull(const ZDateFull &&pIn) { _copyFrom(pIn); }

    bool isInvalid() {
      uint8_t* wPtr=(uint8_t*)this;
      size_t wi=0;
      while ( wi < sizeof(ZDateFull) )
        if (wPtr[wi++])
          return false;
      return true;
    }
    void setInvalid() {
      memset(this, 0, sizeof(ZDateFull));
    }

    ZDateFull &_copyFrom(const  ZDateFull &pIn)
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
    ZDateFull &_copyFrom(const  ZDateFull &&pIn)
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
    ZDateFull &operator=(const ZDateFull &pIn) { return _copyFrom(pIn); }
    ZDateFull &operator=(const ZDateFull &&pIn) { return _copyFrom(pIn); }



    struct tm& getBase(void) {return (struct tm&)*this;}

    ZDateFull& _fromTimet(time_t pTime);

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
ZDataBuffer*  _exportURF(ZDataBuffer *pZDB) const; // export in URF format
size_t        _exportURF_Ptr(unsigned char* &pURF) const;

size_t        getURFSize() const;
ZStatus       _importURF(const unsigned char *&pZDB);     // import from URF format
static ZStatus getUniversalFromURF(const unsigned char* pURFDataPtr, ZDataBuffer& pUniversal,const unsigned char **pURFDataPtrOut=nullptr); // extracts Universal value from URF data

size_t getUniversal_Ptr(unsigned char * &pUniversalPtr) const;

ZStatus getValueFromUniversal(const unsigned char *pUniversalDataPtr);

/* UTC ISO 8601 "2011-10-08T07:07:09.000Z" */
utfcodeString toUTC()
{
    utfcodeString wUTC;
    wUTC.sprintf("%04d-%02d-%02dT%02d:%02d:%02d-%03dZ", Year, Month, Day, Hour, Min, Sec, 0);
    return wUTC;
}
void fromUTC(const utfcodeString &pIn)
{
    clear();
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
    wD++;
    ssize_t wH = pIn.locate((const utf8_t *) ":", wD);
    if (wH < 0)
    {
        fprintf(stderr, "ZDateFull::fromUTC-E_INVFMT Invalid UTC date format <%s>\n", pIn.toCChar());
        return;
    }
    Hour = pIn.subString(wD, wH - wD).toInt(10);
    wH++;
    ssize_t wm = pIn.locate((const utf8_t *) ":", wH);
    if (wm < 0)
    {
        fprintf(stderr, "ZDateFull::fromUTC-E_INVFMT Invalid UTC date format <%s>\n", pIn.toCChar());
        return;
    }
    Min = pIn.subString(wH, wm - wH).toInt(10);
    wm++;
    ssize_t ws = pIn.locate((const utf8_t *) ":", wm);
    if (ws < 0)
    {
        fprintf(stderr, "ZDateFull::fromUTC-E_INVFMT Invalid UTC date format <%s>\n", pIn.toCChar());
        return;
    }
    Sec = pIn.subString(wm, ws-wm).toInt(10);
}

void clear() {memset(this,0,sizeof(ZDateFull));}
ZDateFull fromTimespec(timespec &pTimespec);
timespec  toTimespec(void);

static ZDateFull currentDateTime(void);

ZDateFull& getCurrentDateTime(void) {return fromZDateFull(currentDateTime());}

ZDateFull fromZDateFullString(const ZDateFull_string &pDate);

ZDateFull fromString(char* pDate);

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

utfdescString toLocaleFormatted(void);
utfdescString toFormatted(const char* pFormat="%F %T");

ZDateFull& fromZDateFull(const ZDateFull &pZD) ; /* see _copyFrom */

/** requested by ZMIndex (see zam.h and zmindex.h)*/
int compare(ZDateFull &pDC) const { return memcmp(this, &pDC, sizeof(ZDateFull)); }

bool operator < (const ZDateFull &pDateFull) { return (memcmp (this,&pDateFull,sizeof(ZDateFull))<0);}
bool operator > (const ZDateFull &pDateFull) { return (memcmp (this,&pDateFull,sizeof(ZDateFull))>0);}
bool operator <= (const ZDateFull &pDateFull) { return !(memcmp (this,&pDateFull,sizeof(ZDateFull))>0);}
bool operator >= (const ZDateFull &pDateFull) { return !(memcmp (this,&pDateFull,sizeof(ZDateFull))<0);}


private:
//    struct tm DateInternal;
    inline ZDateFull& _toInternal(void);
    inline ZDateFull& _toInternalBase(void);
    inline ZDateFull& _fromInternal(void);
};  // struct ZDateFull


#endif // ZDATE_H
