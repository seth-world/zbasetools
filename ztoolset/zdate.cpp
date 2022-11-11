#ifndef ZDATE_CPP
#define ZDATE_CPP

#include <ztoolset/zdate.h>
#include <ztoolset/zutfstrings.h>
#include <ztoolset/zlimit.h>

#include <ztoolset/zdatefull.h>

using namespace zbs;
/*  ------------ Defaults values ----------------------------------------------------------------------
 */

const char* ZDateFormat_Standard  = "dd-MMM-yyyy";
const char *ZTimeFormat_Standard = "hh:mm:ss";
const char *ZDateTimeFormat_Standard = "dd-MMM-yyyy hh:mm:ss";

const char *ZDateFormat    = "dd-MMM-yyyy";
const char *ZTimeFormat    = "hh:mm:ss";
const char *ZDateTimeFormat= "dd-MMM-yyyy hh:mm:ss";

const char *FMTLocale         = "%c";                      // defaults date and time formats for the product
const char *FMTStraight       = "%d/%m/%y-%H:%M:%S";
const char *FMTLong           = "%d-%b-%Y-%I:%M:%S %p";
const char *FMTDateOnly       = "%d/%m/%y";
const char *FMTTimeOnly       = "%H:%M:%S";

//----------------ZDate-----------------------------
#pragma pack(push)
#pragma pack(1)
struct wuIn32 {
  uint16_t Year;
  uint8_t  Month;
  uint8_t  Day;
} ;
#pragma pack(pop)

ZDate
ZDate::currentDate(void)
{
time_t   wTime_t;
tm*      wT;
ZDate wDT;
    wTime_t =std::time(0);
    wT = std::localtime(&wTime_t);
    if(wT==nullptr)
        {
        perror("ZDate::currentDate");
        abort();
        }
    wDT.DateInternal = *wT;

    wDT.Year = wDT.DateInternal.tm_year + 1900;
    wDT.Month = wDT.DateInternal.tm_mon + 1;
    wDT.Day = wDT.DateInternal.tm_mday ;
/*
    wDT.Hour = wDT.DateInternal.tm_hour ;
    wDT.Min = wDT.DateInternal.tm_min ;
    wDT.Sec = wDT.DateInternal.tm_sec ;*/
    return wDT;
}

ZDate
ZDate::fromZDateString (ZDate_string &wDateS)
{
    char wBuf[5];
    memmove (wBuf,wDateS.Year,4);
    wBuf[4]='\0';
    Year = atoi(wBuf);
    memmove (wBuf,wDateS.Month,2);
    wBuf[2]='\0';
    Month = atoi(wBuf);
    memmove (wBuf,wDateS.Day,2);
    wBuf[2]='\0';
    Day = atoi(wBuf);

    _toInternal();

    return (*this);
} //fromZDateString

ZDate
ZDate::fromString(char* pDate)
{
    ZDate_string wDFS;
    memset(&wDFS,0,sizeof(ZDate_string));
    size_t wi=strlen(pDate);
    if (wi>sizeof(ZDate_string))
                    wi=sizeof(ZDate_string);
        memmove(&wDFS,pDate,wi);
    return(fromZDateString(wDFS));
}// fromString

ZDate
ZDate::fromZDateFull (const ZDateFull& wDateFull)
{
    Year=wDateFull.Year;
    Month=wDateFull.Month;
    Day=wDateFull.Day;
    _toInternal();

    return (*this);
} //fromZDateString



ZDate
ZDate::fromDMY(const utf8VaryingString &pString)
{
  ZDate wD;
  int wEnd=0;
  int wValue;
  if (pString.isEmpty())
    return ZDate();

  wD.today();  /* initialize with current date  */

  const unsigned char* wPtr=pString.Data;
  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0)
    return ZDate(); /* return invalid date */
  wD.Day = uint8_t(wValue);
  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0)
    return ZDate(); /* return invalid date */
  wD.Month = uint8_t(wValue);
  wValue = getDateValue(wPtr,wEnd,4);
  if (wValue < 0) {
    wD._toInternal();
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Year = uint16_t(wValue);

  wD._toInternal();

  return(wD);
}  // fromDMY

/** 12-30 is a valid date -> gives 12-30-currentyear current time */
ZDate
ZDate::fromMDY(const utf8VaryingString &pString)
{
  ZDate wD=ZDate::today();
  int wEnd=0;
  int wValue;
  if (pString.isEmpty())
    return ZDate();

  const unsigned char* wPtr=pString.Data;

  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0)
    return ZDate(); /* return invalid date */
  wD.Month = uint8_t(wValue);
  wValue = getDateValue(wPtr,wEnd,2);
  if (wValue < 0)
    return ZDate(); /* return invalid date */
  wD.Day = uint8_t(wValue);
  wValue = getDateValue(wPtr,wEnd,4);
  if (wValue < 0) {
    wD._toInternal();
    return wD; /* return partial date. other fields are filled with current date time */
  }
  wD.Year = uint16_t(wValue);
  wD._toInternal();

  return(wD);
}  // fromDMY

void
ZDate::_fromDMY(const utf8VaryingString &pString)
{
  _copyFrom(ZDate::fromDMY(pString));
  return;
}  // fromDMY

void
ZDate::_fromMDY(const utf8VaryingString &pString)
{
  _copyFrom(ZDate::fromMDY(pString));
  return;
}  // fromDMY

#ifdef QT_CORE_LIB

ZDate
ZDate::fromQDate (QDate pQDate)
{
    Year = pQDate.year();
    Month = pQDate.month();
    Day = pQDate.day();
    return (*this) ;
}

ZDate
ZDate::fromQString(QString pDate)
{
    ZDate_string wDFS;
    memset(&wDFS,0,sizeof(ZDate_string));
    size_t wi=pDate.size();
    if (wi>sizeof(ZDate_string))
                    wi=sizeof(ZDate_string);
        memmove(&wDFS,pDate.toUtf8(),wi);
    return(fromZDateString(wDFS));
}// fromQString

//-----------reverse conversion----------------

QDate
ZDate::toQDate(void)
{
QDate wQD ;
    wQD.setDate(Year,Month,Day);
    return(wQD);
}

#endif // QT_CORE_LIB

utf8VaryingString
ZDate::toFormatted(const char* pFormat)
{

    tm wT;
    memset(&wT,0,sizeof(wT));

    char wBuffer[100];


    wT.tm_year = Year-1900;
    wT.tm_mon = Month -1 ;
    wT.tm_mday = Day;

    strftime((char*)wBuffer,100,pFormat,&wT);


    return utf8VaryingString(wBuffer) ;
}//toFormatted

utf8VaryingString
ZDate::toLocale(void)
{
  return toFormatted("%c");

}//toLocale

ZDate&
ZDate::_fromInternal(void)
{
    Year = DateInternal.tm_year + 1900;
    Month = DateInternal.tm_mon + 1;
    Day = DateInternal.tm_mday ;

/*    Hour = DateInternal.tm_hour ;
    Min = DateInternal.tm_min ;
    Sec = DateInternal.tm_sec ;*/
    return *this;
}// _toInternal

ZDate&
ZDate::_toInternal(void)
{
    memset (&DateInternal,0,sizeof(DateInternal));
    DateInternal.tm_year=Year - 1900;
    DateInternal.tm_mon = Month - 1;
    DateInternal.tm_mday = Day ;

/*    DateInternal.tm_hour= Hour ;
    DateInternal.tm_min = Min ;
    DateInternal.tm_sec = Sec ;*/
    return *this;
} // _fromInternal

uint32_t
ZDate::_export(void) const
{
  wuIn32 wuExport;
  wuExport.Year=reverseByteOrder_Conditional<uint16_t>(Year);
  wuExport.Month = Month;
  wuExport.Day = Day;
  uint32_t wOut;
  memmove(&wOut,&wuExport,sizeof(uint32_t));
  return wOut;
}// _export

ssize_t
ZDate::_exportURF(ZDataBuffer& pZDB) const
{
  unsigned char* wPtr=pZDB.extend(sizeof(ZTypeBase)+sizeof(uint32_t));
  return _exportURF_Ptr(wPtr);
}// _exportURF

ssize_t
ZDate::_exportURF_Ptr(unsigned char* &pURF) const
{
  uint32_t wD = _export();
  _exportAtomicPtr<ZTypeBase>(ZType_ZDate,pURF);
  _exportAtomicPtr<uint32_t>(wD,pURF);
  return ssize_t(sizeof(ZTypeBase) + sizeof(uint32_t));
}// _exportURF_Ptr

size_t
ZDate::getURFSize() const
{
  return sizeof(ZTypeBase) + sizeof(uint32_t);
}
size_t
ZDate::getUniversalSize() const
{
  return sizeof(uint32_t);
}

size_t
ZDate::getUniversal_Ptr(unsigned char *&pUniversalPtr) const {
  uint32_t wD=_export();
  _exportAtomicPtr<uint32_t>(wD,pUniversalPtr);
}

void
ZDate::_import(uint32_t pIDate)
{
  wuIn32 wuExport;
  uint32_t wIn;
  memmove(&wuExport,&pIDate,sizeof(uint32_t));

  Year=reverseByteOrder_Conditional<uint16_t>(wuExport.Year);
  Month=wuExport.Month  ;
  Day=wuExport.Day;
  _toInternal();
  return;
}// _import

ssize_t
ZDate::_importURF(const unsigned char* &pZDB)
{
    uint32_t wD =0;
    ZTypeBase wType;
    _importAtomic<ZTypeBase>(wType,pZDB);

    if (wType!=ZType_ZDate)
        {
        fprintf(stderr,"%s-Error invalid ZType_type <%X> <%s>\n",
                _GET_FUNCTION_NAME_,
                wType,
                decode_ZType(wType));
        return -1;
        }
    memmove(&wD,pZDB,sizeof(uint32_t));
    pZDB += sizeof(uint32_t);
    _import(wD);

    return ssize_t(sizeof(uint32_t)+sizeof(ZTypeBase));
}// _import

ZStatus
ZDate::getUniversalFromURF(const unsigned char* pURFDataPtr,ZDataBuffer& pUniversal,const unsigned char** pURFDataPtrOut)
{
 ZTypeBase wType;
 const unsigned char* wURFDataPtr = pURFDataPtr;

     memmove(&wType,wURFDataPtr,sizeof(ZTypeBase));
     wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
     wURFDataPtr += sizeof (ZTypeBase);
     if (wType!=ZType_ZDate)
         {
         fprintf (stderr,
                  "%s>> Error invalid URF data type <%X> <%s> while getting universal value of <%s> ",
                  _GET_FUNCTION_NAME_,
                  wType,
                  decode_ZType(wType),
                  decode_ZType(ZType_ZDate));
         return ZS_INVTYPE;
         }

    memmove(pUniversal.Data,wURFDataPtr,sizeof(uint32_t));
    if (pURFDataPtrOut)
      {
      *pURFDataPtrOut = wURFDataPtr + sizeof(uint32_t);
      }
    return ZS_SUCCESS;
}//getUniversalFromURF

ZStatus
ZDate::getValueFromUniversal(const unsigned char* pUniversalDataPtr)
{
 uint32_t wUniversal;

    memmove(&wUniversal,pUniversalDataPtr,sizeof(uint32_t));
    _import(wUniversal);
    return ZS_SUCCESS;
}//getValueFromUniversal




#endif // ZDATE_CPP
