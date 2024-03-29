#ifndef ENCODEDSTRING_CPP
#define ENCODEDSTRING_CPP

#include <ztoolset/encodedString.h>
#include <ztoolset/zexceptionmin.h>
#include <libxml/encoding.h>

encodedString::encodedString()
{

}
ZStatus
encodedString::fromISOLatin1(const unsigned char* pInString, size_t pInSize)
{

ZStatus wSt;
ZDataBuffer wZDB;
size_t wSize;

    wSt= iconvFromTo( ZCHARSET_LATIN_1,
                      pInString,
                      pInSize,
                      Charset,
                      this);
    if (wSt!=ZS_SUCCESS)
                    return wSt;
   addConditionalTermination();
    Canonical= xmlStrlen(Data);
    return wSt;
}//fromISOLatin1

#endif // ENCODEDSTRING_CPP
