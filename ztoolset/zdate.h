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

static ZDate currentDate(void); //-----------Static get date-------------------

ZDate fromZDateString (ZDate_string &wDateS);
ZDate fromString(char* pDate);

uint32_t    _export(void);  // export in universal format
void        _import(uint32_t pIDate); // import from universal format
ZDataBuffer *_exportURF(ZDataBuffer *pZDB); // export in URF format
ZStatus     _importURF(unsigned char *pZDB);     // import from URF format
ZStatus getValueFromUniversal(unsigned char* pUniversalDataPtr);

static ZStatus getUniversalFromURF(unsigned char* pURFDataPtr,ZDataBuffer& pUniversal); // extracts Universal value from URF data

ZDate fromZDateFull(ZDateFull& wDateFull);
utfdescString toFormatted(const char* pFormat="%F");

#ifdef QT_CORE_LIB
#include <QDate>

ZDate fromQDate (QDate pQDate);
ZDate fromQString(QString pDate);

QDate toQDate(void);

#endif // QT_CORE_LIB


bool operator < (ZDate &pDateFull) { return (memcmp (this,&pDateFull,sizeof(ZDate))<0);}
bool operator > (ZDate &pDateFull) { return (memcmp (this,&pDateFull,sizeof(ZDate))>0);}
bool operator <= (ZDate &pDateFull) { return !(memcmp (this,&pDateFull,sizeof(ZDate))>0);}
bool operator >= (ZDate &pDateFull) { return !(memcmp (this,&pDateFull,sizeof(ZDate))<0);}
ZDate operator = (ZDate &pDate) {memmove(this,&pDate,sizeof(ZDate));return (*this);}

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

    struct tm& getBase(void) {return (struct tm&)*this;}

    ZDateFull& _fromTimet(time_t pTime);

uint64_t _export(void);  // export in universal format
void _import(uint64_t pIDate); // import from universal format
ZDataBuffer* _exportURF(ZDataBuffer *pZDB); // export in URF format
ZStatus _importURF(unsigned char *pZDB);     // import from URF format
static ZStatus getUniversalFromURF(unsigned char* pURFDataPtr,ZDataBuffer& pUniversal); // extracts Universal value from URF data

ZStatus getValueFromUniversal(unsigned char* pUniversalDataPtr);

void clear() {memset(this,0,sizeof(ZDateFull));}
ZDateFull fromTimespec(timespec &pTimespec);
timespec  toTimespec(void);

static ZDateFull currentDateTime(void);

ZDateFull& getCurrentDateTime(void) {return fromZDateFull(currentDateTime());}

ZDateFull fromZDateFullString(ZDateFull_string &pDate);

ZDateFull fromString(char* pDate);

#ifdef QT_CORE_LIB
ZDateFull fromQDateTime (QDateTime pQDate);
ZDateFull fromQDate (QDate &pQDate);
ZDateFull fromQString(QString pDate);
#endif // QT_CORE_LIB
//-----------reverse conversion----------------


#ifdef QT_CORE_LIB

QDateTime toQDateTime(void);

utfdescString
toDateFormatted(const char *pFormat=ZDateFormat_Standard);

utfdescString
toDateTimeFormatted(const char*pFormat=ZDateTimeFormat_Standard);

utfdescString
toTimeFormatted (const char * pFormat=ZTimeFormat_Standard);


QDate toQDate(void);
QTime toQTime(void);
#endif // QT_CORE_LIB

ZDate   toZDate(void);
ZDateFull fromZDate(ZDate &wDate);

utfdescString toLocaleFormatted(void);
utfdescString toFormatted(const char* pFormat="%F %T");

ZDateFull& fromZDateFull(ZDateFull pZD) ;


bool operator < (ZDateFull &pDateFull) { return (memcmp (this,&pDateFull,sizeof(ZDateFull))<0);}
bool operator > (ZDateFull &pDateFull) { return (memcmp (this,&pDateFull,sizeof(ZDateFull))>0);}
bool operator <= (ZDateFull &pDateFull) { return !(memcmp (this,&pDateFull,sizeof(ZDateFull))>0);}
bool operator >= (ZDateFull &pDateFull) { return !(memcmp (this,&pDateFull,sizeof(ZDateFull))<0);}

ZDateFull& operator = (const ZDateFull &pDateFull) {return (fromZDateFull(pDateFull));}

#ifdef QT_CORE_LIB
ZDateFull operator = (QDateTime pDateTime) {fromQDateTime(pDateTime);return (*this);}
#endif
private:
//    struct tm DateInternal;
    inline ZDateFull& _toInternal(void);
    inline ZDateFull& _toInternalBase(void);
    inline ZDateFull& _fromInternal(void);
};  // struct ZDateFull


#endif // ZDATE_H
