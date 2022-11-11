#include <ztoolset/utfstringcommon.h>
#include <ztoolset/utfutils.h>
#include <ztoolset/zsymbols.h>

#include <ztoolset/zexceptionmin.h>
#include <ztoolset/zdatabuffer.h>
#ifdef __DEPRECATED__
/**
 * @brief utfStringHeader::_importURFGeneric imports a ZString content, either fixed or varying, from a stream with URF format.<br>
 * Strings as Source and Target may be of any kind : char, utf8, utf16 or utf32.<br>
 * BUT Sourge string must have the same utf signature (utf8, utf16 or utf32) than Target string.<br>
 * This means that no conversion is made.<br>
 * @todo : in case of truncation : verify no utf8 orphans character units are set or utf16 surrogate orphan are set to target string.
 * Do not allow orphans in a truncated string.
 *
 * @warning : not to be used with a varying string as target :
 * in this case, Data pointer from varying string will not be updated and will point to weird memory emplacement.
 *
 *
 * Structure of stream data to import is <br>
 *
 * <b>Fixed string</b><br>
 * - ZType
 * - Units Count                (URF_Unit_Size_type i. e. uint16_t)     capacity of fixed string
 * - Total universal byte size  (URF_Fixed_Size_type i. e. uint16_t)    effective content of string __END_OF_STRING__ mark excluded
 * - content
 *
 *<b>Varying string</b><br>
 * - ZType                      (ZTypeBase i. e. uint32_t)
 * - Total universal byte size  (URF_Varying_Size_type i. e. uint64_t) effective content of string __END_OF_STRING__ mark excluded
 *
 * Remark: Varying string may cause Data pointer to point to weird memory location.<br>
 *<br>
 * @param pURFDataPtr a pointer to a stream of unsigned char bytes containing the URF string content
 * @return a ZStatus
 */
ssize_t
utfStringHeader::_importURFGeneric(const unsigned char *&pURFDataPtr)
{
ZTypeBase               wSourceType;
URF_Capacity_type       wCapacity;
URF_UnitCount_type      wUnitCount;
//URF_Varying_Size_type   wVUniversalSize;
ssize_t                  wUGenericSize;
ssize_t                  wUnits;

ZStatus         wFinalSt=ZS_SUCCESS;


    pURFDataPtr=getURFBufferValue<ZTypeBase>(&wSourceType,pURFDataPtr);

    if (!(wSourceType&ZType_String))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Error,
                              " Invalid class type. Found %X <%s> while expecting a ZType_String",
                              wSourceType,
                              decode_ZType(wSourceType));
        return 0;
        }

    ZTypeBase wTargetType= getZType();

    if ((wSourceType&ZType_AtomicMask)!=(wTargetType&ZType_AtomicMask)) // if _Utf (character unit type) is not the same
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Error,
                              "ZString error : target character unit type %X <%s> has not the same atomic mask as source %X <%s>. No format conversion is made here.",
                              wTargetType,
                              decode_ZType(wTargetType),
                              wSourceType,
                              decode_ZType(wSourceType));
        return 0;
        }

    if (wSourceType&ZType_VaryingLength) // source is varying string
            {
            pURFDataPtr=getURFBufferValue<URF_UnitCount_type>(&wUnitCount,pURFDataPtr);
            wCapacity = wUnits = wUnitCount;
            wUGenericSize=(uint64_t)wUnitCount * getUnitSize(ZType) ;
            }
        else // Source is fixed string
            {
            pURFDataPtr=getURFBufferValue<URF_Capacity_type>(&wCapacity,pURFDataPtr);
            pURFDataPtr=getURFBufferValue<URF_UnitCount_type>(&wUnitCount,pURFDataPtr);
            wUGenericSize=(uint64_t) size_t(wUnitCount) * getUnitSize(ZType) ;
            }

    if (wTargetType & ZType_VaryingLength)  /* if target is Varying then go to varying */
                            goto _importURF_Varying;

    /* Else target is fixed string */
// ------------Fixed strings---------------------------

    if (wUGenericSize > getByteSize())
                {
                ZException.setMessage(_GET_FUNCTION_NAME_,
                                      ZS_TRUNCATED,
                                      Severity_Warning,
                                      " Capacity of utftemplateString overflow: requested %d while capacity in bytes is %ld . String truncated.",
                                      wUGenericSize,
                                      getByteSize());
                wUGenericSize=getByteSize()-1 ; // yes : but will be appropriately truncated when dividing by sizeof(_Utf) of utf16 and utf32
                wFinalSt=ZS_TRUNCATED;
                }

    switch (wTargetType&ZType_AtomicMask)
    {
        case ZType_Char:
        {
        wUnits=wUGenericSize/sizeof(char);
        utfNSetReverse<char>((char*)DataByte,
                             (const char*)pURFDataPtr,
                             wUnits,
                             wCapacity);
        return wUGenericSize;
        }
        case ZType_U8:
        {
        wUnits=wUGenericSize/sizeof(utf8_t);
        utfNSetReverse<utf8_t>((utf8_t*)DataByte,
                           (const utf8_t*)pURFDataPtr,
                           wUnits,
                           wCapacity);
        return wUGenericSize;
        }
        case ZType_U16:
        {
        wUnits=wUGenericSize/sizeof(utf16_t);
        utfNSetReverse<utf16_t>((utf16_t*)DataByte,
                                (const utf16_t*)pURFDataPtr,
                                wUnits,// character units count
                                (const size_t)wCapacity);   // max units
        return wUGenericSize;
        }
        case ZType_U32:
        {
        wUnits=wUGenericSize/sizeof(utf32_t);
        utfNSetReverse<utf32_t>((utf32_t*)DataByte,
                                (const utf32_t*)pURFDataPtr,
                                wUnits, // character units count
                                (const size_t)wCapacity);
        return wUGenericSize;
        }
    default:
           {
            return 0 ;
            }
    }// switch

    return wUGenericSize;  // of no use but...

//---------------_Varying string--------------------------------------


_importURF_Varying:

    switch (wTargetType&ZType_AtomicMask)
    {
        case ZType_Char:
        {
        wUnits = wUGenericSize/sizeof(char);
        allocateChars<char>(wUGenericSize+1);
        char* wPtr=(char*)DataByte;
        while (wUnits--)
          *wPtr++= *pURFDataPtr++;
//        utfSetReverse<char>(wPtr,(const char*&)pURFDataPtr,wUnits);
        return wUGenericSize;
        }
        case ZType_U8:
        {
        wUnits = wUGenericSize/sizeof(utf8_t);
        utf8_t* wPtr=(utf8_t*)DataByte;
        allocateChars<utf8_t>(wUGenericSize+1);
        while (wUnits--)
          *wPtr++= *pURFDataPtr++;
//        utfStrnset<utf8_t>((utf8_t*)wPtr,(const utf8_t*)pURFDataPtr,getUnitCount(),wUnits);
        return wUGenericSize;
        }
        case ZType_U16:
        {
        utf16_t* wPtr=(utf16_t*)DataByte;
        wUnits = wUGenericSize/sizeof(utf16_t);
        allocateChars<utf16_t>(wUGenericSize+1);
        utfSetReverse<utf16_t>(wPtr,(const utf16_t*&)pURFDataPtr,wUnits);
        return wUGenericSize;
        }
        case ZType_U32:
        {
        utf32_t* wPtr=(utf32_t*)DataByte;
        wUnits = wUGenericSize/sizeof(utf32_t);
        allocateChars<utf32_t>(wUGenericSize+1);
        utfSetReverse<utf32_t>(wPtr,(const utf32_t*&)pURFDataPtr,wUnits);
        return wUGenericSize;
        }
    default:
           {
            return 0;
            }
    }// switch

    return wUGenericSize;
}// _importURFGeneric





/**
 * @brief utfStringHeader::_exportURFGeneric exports a ZString content to a ZDataBuffer, either fixed or varying, into URF format (pURF).<br>
 * String may be of any kind : char, utf8, utf16 or utf32.<br>
 * Structure of source data for the export is the one defined by <br>
 *
 * <b>Fixed string</b><br>
 * - ZType
 * - Units Count                (URF_Unit_Size_type i. e. uint16_t)     capacity of fixed string
 * - Total universal byte size  (URF_Fixed_Size_type i. e. uint16_t)    effective content of string __END_OF_STRING__ mark excluded
 * - content
 *
 *<b>Varying string</b><br>
 * - ZType                      (ZTypeBase i. e. uint32_t)
 * - Total universal byte size  (URF_Varying_Size_type i. e. uint64_t) effective content of string __END_OF_STRING__ mark excluded
 *<br>
 *  Any data is set to big endian.
 *
 * @param pURF  a ZDataBuffer that will contain URF formatted string
 * @return a ZStatus : ZS_SUCCESS if everything went OK, ZS_INVTYPE if an incoherence raised
 */
ssize_t
utfStringHeader::_exportURFGeneric(ZDataBuffer& pURF)
{
ZTypeBase   wType;
URF_Capacity_type      wCapacityUnits;
URF_UnitCount_type     wFUniversalSize;

unsigned char* wURFPtr,*wDataPtr=(unsigned char*)DataByte;
ssize_t wByteSize;
size_t wUnitsCount ;


    if (getZType()&ZType_VaryingLength)
                        goto _exportURF_Varying;
// ------------Fixed strings---------------------------

    wByteSize=sizeof(ZTypeBase)+sizeof(URF_Capacity_type)+sizeof(URF_UnitCount_type);

    switch (getZType()&ZType_AtomicMask)
    {
    case ZType_Char:
    case ZType_U8:
        {
        wUnitsCount=utfStrlen<utf8_t>((utf8_t*)DataByte);
        wFUniversalSize=(utf8_t)(wUnitsCount*sizeof(utf8_t));
        wByteSize+=wUnitsCount;
        break;
        }
    case ZType_U16:
    {
    wUnitsCount=utfStrlen<utf16_t>((utf16_t*)DataByte);
    wFUniversalSize=(uint16_t)(wUnitsCount*sizeof(utf16_t));
    wByteSize+=(wUnitsCount*sizeof(utf16_t));
    break;
    }
    case ZType_U32:
    {
    wUnitsCount=utfStrlen<utf32_t>((utf32_t*)DataByte);
    wFUniversalSize=(uint16_t)(wUnitsCount*sizeof(utf32_t));
    wByteSize+=wFUniversalSize;
    break;
    }
    default:
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_INVTYPE,
                          Severity_Error,
                          " Incoherent atomic character unit for type %X <%s> : invalid for _exportURFGeneric operation",
                          getZType(),
                          decode_ZType(getZType()));

     return 0;
     }
    }//switch



    wURFPtr=pURF.extendBZero(wByteSize); // allocate header size plus used string characters unit.

    _exportAtomicPtr<ZTypeBase>(getZType(),wURFPtr);
    _exportAtomicPtr<URF_Capacity_type>((URF_Capacity_type)getUnitCount(),wURFPtr);
    _exportAtomicPtr<URF_UnitCount_type>((URF_UnitCount_type)wFUniversalSize,wURFPtr);

// Nb : Universal size is effective string data size in bytes and does not take into account header size.
//    wUniversalSize -= (sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t)) ;  // get the length of string data in bytes

    switch (getZType()&ZType_AtomicMask)
    {
        case ZType_Char:
        {
        while (*wDataPtr)
                *wURFPtr++=*wDataPtr++;
//        utfStrnset<char>((char*)wURFPtr,wUniversalSize,(const char*)DataByte,wCanonical);
        return wByteSize;
        }
        case ZType_U8:
        {
        while (*wDataPtr)
                *wURFPtr++=*wDataPtr++;
//        utfStrnset<utf8_t>((utf8_t*)wURFPtr,wUniversalSize,(const utf8_t*)DataByte,wCanonical);
        return wByteSize;
        }
        case ZType_U16:
        {
          utf16_t* wPtr=(utf16_t*)wURFPtr;
          const utf16_t* wPtrIn=(const utf16_t*)DataByte;
        utfSetReverse<utf16_t>(wPtr, wPtrIn,
                                (const size_t)wUnitsCount);// character units count
        return wByteSize;
        }
        case ZType_U32:
        {
          utf32_t* wPtr=(utf32_t*)wURFPtr;
          const utf32_t* wPtrIn=(const utf32_t*)DataByte;
        utfSetReverse<utf32_t>(wPtr,wPtrIn,
                               (const size_t)wUnitsCount);// character units count
        return wByteSize;
        }
    default:
           {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Error,
                                  " Incoherent atomic character unit for type %X <%s> : invalid for _exportURFGeneric operation",
                                  getZType(),
                                  decode_ZType(getZType()));
            return 0 ;
            }
    }// switch

    return wByteSize;  // of no use but...

//---------------_Varying string--------------------------------------

_exportURF_Varying:


    wUnitsCount=getUnitCount();
    wByteSize=sizeof(ZTypeBase)+sizeof(uint64_t)+getByteSize();
    wURFPtr=pURF.extendBZero(wByteSize); // allocate header size plus used string characters unit.


    _exportAtomicPtr<ZTypeBase>(getZType(),wURFPtr);
    _exportAtomicPtr<URF_UnitCount_type>((URF_UnitCount_type)wUnitsCount,wURFPtr);


    switch (getZType()&ZType_AtomicMask)
    {
        case ZType_Char:
        {
        memmove(wURFPtr,DataByte,getByteSize());
        break;
        }
        case ZType_U8:
        {
        memmove(wURFPtr,DataByte,getByteSize());
        break;
        }
        case ZType_U16:
        {
          utf16_t* wPtr=(utf16_t*)wURFPtr;
          const utf16_t* wPtrIn=(const utf16_t*)DataByte;
        utfSetReverse<utf16_t>(wPtr,wPtrIn,
                               (const size_t)wUnitsCount);// character units count
        break;
        }
        case ZType_U32:
        {
          utf32_t* wPtr=(utf32_t*)wURFPtr;
          const utf32_t* wPtrIn=(const utf32_t*)DataByte;
          utfSetReverse<utf32_t>(wPtr,wPtrIn,
              (const size_t)wUnitsCount);// character units count
        break;
        }
    default:
           {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Error,
                                  " Incoherent atomic character unit for type %X <%s> : invalid for _exportURFGeneric operation",
                                  getZType(),
                                  decode_ZType(getZType()));
            return 0;
            }
    }// switch

    return wByteSize;
}// _exportURFGeneric

/**
 * @brief utfStringHeader::getUniversalFromURF Static function that extracts universal content (__END_OF_STRING__ mark excluded) from an URF content.
 * @param pType
 * @param pURFDataPtr       pointer to to input data structured as URF data
 * @param pUniversal
 * @return a ZStatus
 */
ZStatus
utfStringHeader::getUniversalFromURF(ZType_type pType,const unsigned char* pURFDataPtr, ZDataBuffer &pUniversal, const unsigned char **pURFDataPtrOut)
{
URF_Capacity_type       wCapacity;
size_t                  wUnits;
URF_UnitCount_type    wFUniversalSize;
URF_UnitCount_type    wUnitsCount ;

const unsigned char*wDataPtr=pURFDataPtr;

ZTypeBase wType;

utf8_t*wU8Ptr=nullptr;
const utf8_t*wU8PtrIn= nullptr;
utf16_t*wU16Ptr=nullptr;
const utf16_t*wU16PtrIn= nullptr;
utf32_t*wU32Ptr=nullptr;
const utf32_t*wU32PtrIn= nullptr;


    memmove(&wType,wDataPtr,sizeof(ZTypeBase));
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);
    if (wType!=pType)
        {
        fprintf (stderr,
                 "%s>> Error invalid URF data type <%X> <%s> while requested universal value of <%s> \n",
                 _GET_FUNCTION_NAME_,
                 wType,
                 decode_ZType(wType),
                 decode_ZType(pType));
        return ZS_INVTYPE;
        }
    wDataPtr += sizeof (ZTypeBase);

    if (wType & ZType_VaryingLength)
                    goto _getUniversal_Varying ;

    memmove (&wCapacity,wDataPtr,sizeof(wCapacity));
    wCapacity=reverseByteOrder_Conditional<URF_Capacity_type>(wCapacity);
    wDataPtr += sizeof (URF_Capacity_type);

    memmove (&wUnitsCount,wDataPtr,sizeof(wUnitsCount));
    wFUniversalSize=reverseByteOrder_Conditional<URF_UnitCount_type>(wUnitsCount);
    wDataPtr += sizeof (URF_UnitCount_type);
    pUniversal.allocateBZero((size_t)wFUniversalSize); /* allocate size without endofstring mark */

    if (pURFDataPtrOut)
      {
      *pURFDataPtrOut = wDataPtr + wFUniversalSize;
      }

    wUnits=0;
    switch (pType&ZType_AtomicMask)
    {
    case ZType_Char:
    case ZType_U8:
        wU8Ptr=(utf8_t*)pUniversal.Data;
        wU8PtrIn= (const utf8_t*)wDataPtr;
        wUnits=wFUniversalSize/sizeof(utf8_t); /* compute effective units number */
        while (wUnits--)
                {
                *wU8Ptr++=*wU8PtrIn++;
                }
        break;
    case ZType_U16:
        wU16Ptr=(utf16_t*)pUniversal.Data;
        wU16PtrIn= (utf16_t*)wDataPtr;
        wUnits=wFUniversalSize/sizeof(utf16_t); /* compute effective units number */
        while (wUnits--)
                {
                *wU16Ptr++=*wU16PtrIn++;
                }
        break;
    case ZType_U32:
        wU32Ptr=(utf32_t*)pUniversal.Data;
        wU32PtrIn= (utf32_t*)wDataPtr;
        wUnits=wFUniversalSize/sizeof(utf32_t); /* compute effective units number */
        while (wUnits--)
                {
                *wU32Ptr++=*wU32PtrIn++;
                }
        break;
    default:
        return ZS_INVTYPE;
    }// switch

    return ZS_SUCCESS;

_getUniversal_Varying:

    /* for a varying string no canonical size, and string size fits into a uint64_t */

    memmove (&wUnitsCount,wDataPtr,sizeof(wUnitsCount));
    wUnitsCount=reverseByteOrder_Conditional<URF_Varying_Size_type>(wUnitsCount);
    wDataPtr += sizeof (URF_Varying_Size_type);
//    pUniversal.allocateBZero((size_t)wEffectiveUSize+sizeof(_Utf)); /* allocate size + endofstring mark */
    pUniversal.allocateBZero((size_t)wUnitsCount*); /* allocate size without endofstring mark */
    if (pURFDataPtrOut)
      {
      *pURFDataPtrOut = wDataPtr + wVUniversalSize;
      }

    wUnits=0;
    switch (pType&ZType_AtomicMask)
    {
    case ZType_UChar:
    case ZType_Char:
    case ZType_U8:
        wU8Ptr=(utf8_t*)pUniversal.Data;
        wU8PtrIn= (const utf8_t*)wDataPtr;
        wUnits=wVUniversalSize/sizeof(utf8_t); /* compute effective units number */
        while (wUnits--)
                {
                *wU8Ptr++=*wU8PtrIn++;
                }
        break;
    case ZType_U16:
        wU16Ptr=(utf16_t*)pUniversal.Data;
        wU16PtrIn= (const utf16_t*)wDataPtr;
        wUnits=wVUniversalSize/sizeof(utf16_t); /* compute effective units number */
        while (wUnits--)
                {
                *wU16Ptr++=*wU16PtrIn++;
                }
        break;
    case ZType_U32:
        wU32Ptr=(utf32_t*)pUniversal.Data;
        wU32PtrIn= (const utf32_t*)wDataPtr;
        wUnits=wVUniversalSize/sizeof(utf32_t); /* compute effective units number */
        while (wUnits--)
                {
                *wU32Ptr++=*wU32PtrIn++;
                }
        break;
    default:
        return ZS_INVTYPE;
    }// switch

    return ZS_SUCCESS;

}//getUniversalFromURF_Truncated
#endif // __DEPRECATED__
