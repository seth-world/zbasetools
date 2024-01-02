#ifndef ZBASEDATATYPE_CPP
#define ZBASEDATATYPE_CPP
#include <ztoolset/zbasedatatypes.h>
#include <ztoolset/zexceptionmin.h>
#include <ztoolset/utfvaryingstring.h>
#ifdef __COMMENT__

/*  ------------ Defaults values ----------------------------------------------------------------------
 */
/*
const char ZDateFormat_Standard [] = {"dd-MMM-yyyy"};
const char *ZTimeFormat_Standard = "hh:mm:ss";
const char *ZDateTimeFormat_Standard = "dd-MMM-yyyy hh:mm:ss";

char ZDateFormat[50]    = {"dd-MMM-yyyy"};
char ZTimeFormat[50]    = {"hh:mm:ss"};
char ZDateTimeFormat[50]= {"dd-MMM-yyyy hh:mm:ss"};
*/
const char *FMTLocale         ="%c";                      // defaults date and time formats for the product
const char *FMTStraight       = "%d/%m/%y-%H:%M:%S";
const char *FMTLong           = "%d-%b-%Y-%I:%M:%S %p";
const char *FMTDateOnly       = "%d/%m/%y";
const char *FMTTimeOnly       = "%H:%M:%S";

//--------------- ZDateFull-------------------------c
//

ZDateFull ZDateFull::fromZDateFullString(ZDateFull_string &pDate)
{
char wBuf[5];
    memmove (wBuf,pDate.Year,4);
    wBuf[4]='\0';
    Year = atoi(wBuf);
    memmove (wBuf,pDate.Month,2);
    wBuf[2]='\0';
    Month = atoi(wBuf);
    memmove (wBuf,pDate.Month,2);
    wBuf[2]='\0';
    Month = atoi(wBuf);
    memmove (wBuf,pDate.Day,2);
    wBuf[2]='\0';
    Day = atoi(wBuf);
    memmove (wBuf,pDate.Hour,2);
    wBuf[2]='\0';
    Hour = atoi(wBuf);
    memmove (wBuf,pDate.Min,2);
    wBuf[2]='\0';
    Min = atoi(wBuf);
    memmove (wBuf,pDate.Sec,2);
    wBuf[2]='\0';
    Sec = atoi(wBuf);

    _toInternal();

    return(*this);
}  // fromDateFullString

ZDateFull
ZDateFull::fromTimespec(timespec &pTimespec)
{
    memset (this,0,sizeof(ZDateFull));
    struct tm*wTm;
    wTm = localtime (&pTimespec.tv_sec);
    memmove(&DateInternal,wTm,sizeof(tm));
    Sec=wTm->tm_sec;
    Min=wTm->tm_min;
    Hour=wTm->tm_hour;
    Day=wTm->tm_mday;
    Month=wTm->tm_mon + 1;
    Year=wTm->tm_year + 1900;
    return (*this) ;
}


timespec
ZDateFull::toTimespec(void)
{
    timespec wTS ;
    wTS.tv_sec= mktime (&DateInternal);
    wTS.tv_nsec = 0;
    return wTS;
}

#ifdef QT_CORE_LIB
ZDateFull
ZDateFull::fromQDateTime (QDateTime pQDate)
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
ZDateFull::fromQDate (QDate &pQDate)
{
    memset(this,0,sizeof(ZDateFull));
    Year = pQDate.year();
    Month = pQDate.month();
    Day = pQDate.day();
    _toInternal();
    return (*this) ;
}
#endif// QT_CORE_LIB

ZDateFull
ZDateFull::currentDateTime(void)
{
time_t   wTime_t;
tm*      wT;
ZDateFull wDT;
    wTime_t =std::time(0);
    wT = std::localtime(&wTime_t);
    if(wT==nullptr)
        {
        perror("ZDateFull::currentDateTime");
        abort();
        }
    wDT.DateInternal = *wT;

    wDT.Year = wDT.DateInternal.tm_year + 1900;
    wDT.Month = wDT.DateInternal.tm_mon + 1;
    wDT.Day = wDT.DateInternal.tm_mday ;

    wDT.Hour = wDT.DateInternal.tm_hour ;
    wDT.Min = wDT.DateInternal.tm_min ;
    wDT.Sec = wDT.DateInternal.tm_sec ;
    return wDT;
}
ZDateFull&
ZDateFull::_toInternal(void)
{
    Year = DateInternal.tm_year + 1900;
    Month = DateInternal.tm_mon + 1;
    Day = DateInternal.tm_mday ;

    Hour = DateInternal.tm_hour ;
    Min = DateInternal.tm_min ;
    Sec = DateInternal.tm_sec ;
    return *this;
}// _toInternal

ZDateFull&
ZDateFull::_fromInternal(void)
{
    DateInternal.tm_year=Year - 1900;
    DateInternal.tm_mon = Month - 1;
    DateInternal.tm_mday = Day ;

    DateInternal.tm_hour= Hour ;
    DateInternal.tm_min = Min ;
    DateInternal.tm_sec = Sec ;
    return *this;
} // _fromInternal

/**
 * @brief ZDateFull::toFormatted returns a descString with  a formatted full date according a format description.
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
descString
ZDateFull::toFormatted(char* pFormat)
{
    descString wOut;
    tm wT;
    memset(&wT,0,sizeof(wT));

    wT.tm_year = Year-1900;
    wT.tm_mon = Month -1 ;
    wT.tm_mday = Day;
    wT.tm_min = Min;
    wT.tm_hour=Hour;
    wT.tm_sec = Sec ;

    strftime(wOut.content,sizeof(wOut.content),pFormat,&wT);

    return wOut;
}//toFormatted


descString
ZDateFull::toLocaleFormatted(void)
{
    return toFormatted("%c");

}//toSystemFormatted



ZDateFull ZDateFull::fromString(char* pDate)
{
    ZDateFull_string wDFS;
    memset(&wDFS,0,sizeof(ZDateFull_string));
    size_t wi=strlen(pDate);
    if (wi>sizeof(ZDateFull_string))
                    wi=sizeof(ZDateFull_string);
    memmove(&wDFS,pDate,wi);
    return(fromZDateFullString(wDFS));
} // fromString

#ifdef QT_CORE_LIB
ZDateFull ZDateFull::fromQString(QString pDate)
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



#ifdef QT_CORE_LIB

QDateTime ZDateFull::toQDateTime(void)
{
    QDateTime wQD ;
    const QDate wQD1(Year,Month,Day);
    const QTime wQT(Hour,Min,Sec);

    wQD.setDate(wQD1);
    wQD.setTime(wQT);
    return wQD ;
}
#endif //#ifdef QT_CORE_LIB


descString
ZDateFull::toDateFormatted(const char *pFormat)
{
    descString wRet;
    wRet=toQDate().toString(pFormat);
    return (wRet);
}
descString
ZDateFull::toDateTimeFormatted(const char*pFormat)
{
    descString wRet;
    wRet=toQDateTime().toString(pFormat);
    return (wRet);
}
descString
ZDateFull::toTimeFormatted (const char * pFormat)
{
    descString wRet;
    wRet=toQTime().toString(pFormat);
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
#endif // __COMMENT__






/**
 * @brief getHamFloat calculates the Hamming distance between 2 strings
 * @param pStr1
 * @param pStr2
 * @return  a ZHamFloat containing
 * . a long with the size difference between the two strings
 * . a double with the Hamming on the shortest string size
 */
ZHamFloat_struct
_getHamFloat (const char *pStr1, const char *pStr2)
{
    ZHamFloat_struct wHF ;

    size_t wLen ;
    size_t wl1=strlen(pStr1);
    size_t wl2=strlen(pStr2);
    if (wl1>wl2)
            {
            wLen = wl2 ;
            wHF.Size_Delta = wl1-wl2;
            }
            else
            {
            wLen=wl1;
            wHF.Size_Delta = wl2-wl1;
            }

    wHF.Distance=0;
    for (wl1=0;wl1<wLen;wl1++)
                {
                wHF.Distance = wHF.Distance + (double)!(pStr1[wl1]==pStr2[wl1]);
                }
//    wHF.Distance = wHF.Distance + (double)abs(wHF.Size_Delta);

    fprintf (stdout, " Size delta <%ld> Distance <%G>  <%s> <%s>\n",
             wHF.Size_Delta,
             wHF.Distance,
             pStr1,
             pStr2);
  return(wHF);
}// getHamming


ZHamming_struct
_getHamming (const char *pStr1, const char *pStr2)
{
    ZHamming_struct wH ;

    size_t wLen ;
    size_t wl1=strlen(pStr1);
    size_t wl2=strlen(pStr2);
    if (wl1>wl2)
            {
            wLen = wl2 ;
            wH.Size_Delta = wl1-wl2;
            }
            else
            {
            wLen=wl1;
            wH.Size_Delta = wl2-wl1;
            }
    wH.Distance=0;
    for (wl1=0;wl1<wLen;wl1++)
                {
                wH.Distance = wH.Distance + (long)!(pStr1[wl1]==pStr2[wl1]);
                }
  return(wH);
}// getHamming


const char *
decode_ZCM (ZCryptMethod_type pZCM)
{
    switch (pZCM)
    {
    case (ZCM_Uncrypted):
        return ("ZCM_Uncrypted");
    case (ZCM_Base64):
        return ("ZCM_Base64");
    case (ZCM_AES256):
        return ("ZCM_AES256");
    case (ZCM_SHA256):
        return ("ZCM_SHA256");
    case (ZCM_MD5):
        return ("ZCM_MD5");
    default:
        return ("ZCM_Unknown");

    }//switch
} //decode_ZCM

ZCryptMethod_type
encode_ZCM (const char * pZCMStr)
{
    if (strcasecmp(pZCMStr,"ZCM_Uncrypted")==0)
        return ZCM_Uncrypted;
    if (strcasecmp(pZCMStr,"ZCM_Base64")==0)
        return ZCM_Base64;
    if (strcasecmp(pZCMStr,"ZCM_AES256")==0)
        return ZCM_AES256;
    if (strcasecmp(pZCMStr,"ZCM_SHA256")==0)
        return ZCM_SHA256;
    if (strcasecmp(pZCMStr,"ZCM_AES256")==0)
        return ZCM_AES256;
    if (strcasecmp(pZCMStr,"ZCM_MD5")==0)
        return ZCM_MD5;
    return ZCM_Uncrypted;
} //decode_ZCM

#endif //ZBASEDATATYPE_CPP
