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


struct ZDate_string {
    char Year[5];
    char Month[3];
    char Day[3];

};
//typedef templateString<char [cst_datelen+1]>        dateString;
//typedef templateString<char [cst_fulldatelen+1]>    datefullString;
class ZDateFull;

class ZDate
{

public:
    uint16_t Year;
    uint8_t  Month;
    uint8_t  Day;

    ZDate() {setInvalid();}
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
        _toInternal();
        return *this;
    }
    ZDate &_copyFrom(ZDate &&pIn)
    {
        memset(this, 0, sizeof(ZDate));
        Year = pIn.Year;
        Month = pIn.Month;
        Day = pIn.Day;
        _toInternal();
        return *this;
    }
    ZDate &operator=(ZDate &pIn) { return _copyFrom(pIn); }
    ZDate &operator=(ZDate &&pIn) { return _copyFrom(pIn); }

  void clear() {memset(this,0,sizeof(ZDate));}


  ZDate fromZDateString (ZDate_string &wDateS);
  ZDate fromString(char* pDate);

  static ZDate currentDate(void); //-----------Static get date-------------------
  static ZDate today() {return currentDate();}
  static ZDate fromDMY(const utf8VaryingString &pString);
  static ZDate fromMDY(const utf8VaryingString &pString);

  void _fromDMY(const utf8VaryingString &pString);
  void _fromMDY(const utf8VaryingString &pString);

uint32_t      _export(void) const;  // export in universal format
void          _import(uint32_t pIDate); // import from universal format
ssize_t        _exportURF(ZDataBuffer &pZDB) const; // export in URF format
ssize_t        _exportURF_Ptr(unsigned char *&pURF) const; // export in URF format
size_t        getURFSize() const ;
size_t        getUniversalSize() const ;
ssize_t     _importURF(const unsigned char *&pZDB);     // import from URF format
ZStatus getValueFromUniversal(const unsigned char* pUniversalDataPtr);

size_t getUniversal_Ptr(unsigned char * &pUniversalPtr) const;

static ZStatus getUniversalFromURF(const unsigned char* pURFDataPtr, ZDataBuffer& pUniversal,const  unsigned char **pURFDataPtrOut=nullptr); // extracts Universal value from URF data

ZDate fromZDateFull(const ZDateFull &wDateFull);
utf8VaryingString toFormatted(const char* pFormat="%F");
utf8VaryingString toLocale();

utf8VaryingString toUTC () const;
void fromUTC(const utf8VaryingString &pIn) ;

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



/** @brief getDateValue returns an int with decoded decimal value with a maximum number of digit of pMaxDigits.
 * If end of string encountered pEnd is set to 1, else set to 0.
 * If end of string has been encountered without any valid decimal digit, then returned value is -1 and pEnd is set to 1.
 */
int getDateValue(const unsigned char* &pPtrIn,int &pEnd,int pMaxDigits);

#endif // ZDATE_H
