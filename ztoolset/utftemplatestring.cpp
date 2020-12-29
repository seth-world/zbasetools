#ifndef UTFTEMPLATESTRING_CPP
#define UTFTEMPLATESTRING_CPP

#include <ztoolset/utftemplatestring.h>
//#include <zindexedfile/zdatatype.h>
#include <ztoolset/zexceptionmin.h>


/**
 * @brief _getutfStringURFData Routine to get URF data header information for an utfxxFixedString or utfxxVaryingString.
 * @param pUniversal pointer to first byte of URF data header
 * @return a ZStatus
 */
ZStatus _getutfStringURFData(unsigned char* pURFDataPtr,ZTypeBase& pZType,uint16_t &pCanonical,uint16_t &pUniversalSize)
{
//size_t      wURFByteSize;
size_t      wOffset=0;

    memmove(&pZType,pURFDataPtr,sizeof(ZTypeBase));
    pZType=reverseByteOrder_Conditional<ZTypeBase>(pZType);
    if (!pZType & ZType_String)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid ZType (class data type) while loading utfString URF data header.\n"
                              "Found <%x> <%s> while expecting one of <%x> <%s> (utfxxFixedString or utfxxVaryingString).\n",
                              pZType,
                              decode_ZType(pZType),
                              ZType_String,
                              "ZType_String");
        return ZS_INVTYPE;
        }


    wOffset+= sizeof(ZTypeBase);

    memmove(&pCanonical, pURFDataPtr+wOffset,sizeof(pCanonical));
    pCanonical=reverseByteOrder_Conditional<uint16_t>(pCanonical);
    wOffset+= sizeof(uint16_t);

    memmove(&pUniversalSize, pURFDataPtr+wOffset,sizeof(pUniversalSize));
    pUniversalSize=reverseByteOrder_Conditional<uint16_t>(pUniversalSize);
    wOffset+= sizeof(uint16_t);
    return ZS_SUCCESS;
}// _getutfStringURFData


template <size_t _Sz,size_t _Sz1,class _Utf>
ZStatus
fromEncoded (utftemplateString<_Sz,_Utf>* pOutString,utftemplateString<_Sz1,_Utf>* pInString)
{

ZDataBuffer wZDB;
size_t wSize;
    ZStatus wSt= iconvFromTo(pInString->Charset,
                       (const unsigned char*)pInString->content,
                       pInString->UnitCount,
                       pOutString->Charset,
                       wZDB);
    if (wSt!=ZS_SUCCESS)
                    return wSt;
    wZDB.addConditionalTermination();
    wSize=wZDB.Size;
    if (wSize >= pOutString->_capacity)
                    {
                    wSize=pOutString->_capacity-1;
                    wSt=ZS_FIELDCAPAOVFLW;
                    }
    pOutString->clear();
    pOutString=wZDB.DataChar;
    return wSt;
}// fromISOLatin1



// --------------------Functions--------------------------
//


template <size_t _Sz1,size_t _Sz2>
utftemplateString<_Sz1> &moveutftemplateString(utftemplateString<_Sz1> &pDest, utftemplateString<_Sz2> &pSource)
{
size_t wSize = pDest.size();

    if ((pDest.size()+pSource.size())>sizeof(pDest.content))
                {
                wSize=_Sz1-pDest.size();
                }
                else
                wSize=pSource.size();

    strncat(pDest.content,pSource.content,wSize);
    return pDest;
}




#ifdef __USE_WINDOWS__

#include <shlwapi.h>  // StrStrI  for strstrcase replacement

char * strcasestr(const char* pString,const char *pSubString)
{
    return (char*) StrStrI((PCWSTR)pString,(PCWSTR)pSubString);
}

#endif

#endif // UTFTEMPLATESTRING_CPP
