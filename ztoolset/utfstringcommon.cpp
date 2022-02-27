#include <ztoolset/utfstringcommon.h>
#include <ztoolset/utfutils.h>
#include <ztoolset/zsymbols.h>

#include <ztoolset/zexceptionmin.h>

/** @defgroup ZStringGroup Using fixed strings
  *
  *
  */

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
ZStatus
utfStringHeader::_importURFGeneric(const unsigned char *pURFDataPtr)
{
ZTypeBase               wSourceType;
URF_Capacity_type       wCapacity;
URF_Fixed_Size_type     wFUniversalSize;
URF_Varying_Size_type   wVUniversalSize;
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
        return ZS_INVTYPE;
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
        return ZS_INVTYPE;
        }

    if (wSourceType&ZType_VaryingLength) // source is varying string
            {
            pURFDataPtr=getURFBufferValue<URF_Varying_Size_type>(&wVUniversalSize,pURFDataPtr);
            wCapacity=wFUniversalSize=wUGenericSize=(uint64_t)wVUniversalSize;
            }
        else // Source is fixed string
            {
            pURFDataPtr=getURFBufferValue<URF_Capacity_type>(&wCapacity,pURFDataPtr);
            pURFDataPtr=getURFBufferValue<URF_Fixed_Size_type>(&wFUniversalSize,pURFDataPtr);
            wUGenericSize=(uint64_t)wFUniversalSize;
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
                                      wFUniversalSize,
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
        return wFinalSt;
        }
        case ZType_U8:
        {
        wUnits=wUGenericSize/sizeof(utf8_t);
        utfNSetReverse<utf8_t>((utf8_t*)DataByte,
                           (const utf8_t*)pURFDataPtr,
                           wUnits,
                           wCapacity);
        return wFinalSt;
        }
        case ZType_U16:
        {
        wUnits=wUGenericSize/sizeof(utf16_t);
        utfNSetReverse<utf16_t>((utf16_t*)DataByte,
                                (const utf16_t*)pURFDataPtr,
                                wUnits,// character units count
                                (const size_t)wCapacity);   // max units
        return wFinalSt;
        }
        case ZType_U32:
        {
        wUnits=wUGenericSize/sizeof(utf32_t);
        utfNSetReverse<utf32_t>((utf32_t*)DataByte,
                                (const utf32_t*)pURFDataPtr,
                                wUnits, // character units count
                                (const size_t)wCapacity);
        return wFinalSt;
        }
    default:
           {
            return ZS_INVTYPE ;
            }
    }// switch

    return wFinalSt;  // of no use but...

//---------------_Varying string--------------------------------------


_importURF_Varying:

    switch (wTargetType&ZType_AtomicMask)
    {
        case ZType_Char:
        {
        wUnits = wUGenericSize/sizeof(char);
        allocateChars<char>(wUGenericSize+1);
        utfSetReverse<char>((char*)DataByte,(const char*)pURFDataPtr,wUnits);
        break;
        }
        case ZType_U8:
        {
        wUnits = wUGenericSize/sizeof(utf8_t);
        allocateChars<utf8_t>(wUGenericSize+1);
        utfStrnset<utf8_t>((utf8_t*)DataByte,(const utf8_t*)pURFDataPtr,getUnitCount(),wUnits);
        break;
        }
        case ZType_U16:
        {
        wUnits = wUGenericSize/sizeof(utf16_t);
        allocateChars<utf16_t>(wUGenericSize+1);
        utfSetReverse<utf16_t>((utf16_t*)DataByte,(const utf16_t*)pURFDataPtr,wUnits);
        break;
        }
        case ZType_U32:
        {
        wUnits = wUGenericSize/sizeof(utf32_t);
        allocateChars<utf32_t>(wUGenericSize+1);
        utfSetReverse<utf32_t>((utf32_t*)DataByte,(const utf32_t*)pURFDataPtr,wUnits);
        break;
        }
    default:
           {
            return ZS_INVTYPE;
            }
    }// switch

    return wFinalSt;
}// _importURF

#include <ztoolset/zdatabuffer.h>


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
ZStatus
utfStringHeader::_exportURFGeneric(ZDataBuffer* pURF)
{
ZTypeBase   wType;
URF_Capacity_type      wCapacityUnits;
URF_Fixed_Size_type     wFUniversalSize;
URF_Varying_Size_type   wVUniversalSize;

unsigned char* wURFPtr,*wDataPtr=(unsigned char*)DataByte;
size_t wByteSize;
size_t wUnitsCount ;


    if (getZType()&ZType_VaryingLength)
                        goto _exportURF_Varying;
// ------------Fixed strings---------------------------

    wByteSize=sizeof(ZTypeBase)+sizeof(URF_Capacity_type)+sizeof(URF_Fixed_Size_type);

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

     return ZS_INVTYPE;
     }
    }//switch



    wURFPtr=pURF->allocate(wByteSize); // allocate header size plus used string characters unit.

    wType=getZType();
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);

    memmove(wURFPtr,&wType,sizeof(ZTypeBase));
    wURFPtr+=sizeof(ZTypeBase);


    wCapacityUnits=reverseByteOrder_Conditional<URF_Capacity_type>((URF_Capacity_type)getUnitCount());
    memmove(wURFPtr,&wCapacityUnits,sizeof(URF_Capacity_type));
    wURFPtr+= sizeof(URF_Capacity_type);

    wFUniversalSize=reverseByteOrder_Conditional<URF_Fixed_Size_type>(wFUniversalSize);
    memmove(wURFPtr,&wFUniversalSize,sizeof(URF_Fixed_Size_type));
    wURFPtr+= sizeof(URF_Fixed_Size_type);

// Nb : Universal size is effective string data size in bytes and does not take into account header size.
//    wUniversalSize -= (sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t)) ;  // get the length of string data in bytes

    switch (getZType()&ZType_AtomicMask)
    {
        case ZType_Char:
        {
        while (*wDataPtr)
                *wURFPtr++=*wDataPtr++;
//        utfStrnset<char>((char*)wURFPtr,wUniversalSize,(const char*)DataByte,wCanonical);
        return ZS_SUCCESS;
        }
        case ZType_U8:
        {
        while (*wDataPtr)
                *wURFPtr++=*wDataPtr++;
//        utfStrnset<utf8_t>((utf8_t*)wURFPtr,wUniversalSize,(const utf8_t*)DataByte,wCanonical);
        return ZS_SUCCESS;
        }
        case ZType_U16:
        {
        utfSetReverse<utf16_t>((utf16_t*)wURFPtr,
                                (const utf16_t*)DataByte,
                                (const size_t)wUnitsCount);// character units count
        return ZS_SUCCESS;
        }
        case ZType_U32:
        {
        utfSetReverse<utf32_t>((utf32_t*)wURFPtr,
                                (const utf32_t*)DataByte,
                               (const size_t)wUnitsCount);// character units count
        return ZS_SUCCESS;
        }
    default:
           {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVTYPE,
                                  Severity_Error,
                                  " Incoherent atomic character unit for type %X <%s> : invalid for _exportURFGeneric operation",
                                  getZType(),
                                  decode_ZType(getZType()));
            return ZS_INVTYPE ;
            }
    }// switch

    return ZS_SUCCESS;  // of no use but...

//---------------_Varying string--------------------------------------

_exportURF_Varying:

    wUnitsCount=getUnitCount();
    wVUniversalSize = getByteSize();
    wByteSize=sizeof(ZTypeBase)+sizeof(uint64_t)+wVUniversalSize;
    wURFPtr=pURF->allocate(wByteSize); // allocate header size plus used string characters unit.

    wType=getZType();
    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);

    memmove(wURFPtr,&wType,sizeof(ZTypeBase));
    wURFPtr+=sizeof(ZTypeBase);

    wVUniversalSize=reverseByteOrder_Conditional<URF_Varying_Size_type>(wVUniversalSize);
    memmove(wURFPtr,&wVUniversalSize,sizeof(wVUniversalSize));
    wURFPtr+= sizeof(URF_Varying_Size_type);


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
        utfSetReverse<utf16_t>((utf16_t*)wURFPtr,
                                (const utf16_t*)DataByte,
                               (const size_t)wUnitsCount);// character units count
        break;
        }
        case ZType_U32:
        {
        utfSetReverse<utf32_t>((utf32_t*)wURFPtr,
                                (const utf32_t*)DataByte,
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
            return ZS_INVTYPE;
            }
    }// switch

    return ZS_SUCCESS;
}// _exportURFGeneric

/**
 * @brief utfStringHeader::_exportURFOtherGeneric
 * @param pURF
 * @return
 */
/**
 * @brief utfStringHeader::_exportURFOtherGeneric exports a string to another different target string definition.<br>
 * Both strings, Source and Target, must have the same utf signature : both must utf8_t or utf16_t or utf32_t.<br>
 * But other components could be different.<br>
 *
 * a fixed string is defined by<br>
 * ZType (gives the utf signature)<br>
 * Canonical capacity (char units count)<br>
 *
 * a Varying string is defined by<br>
 * ZType (gives the utf signature)<br>
 *
 * @param pTargetURF        output URF content with appropriate header
 * @param pTargetZType      ZTypeBase of target string to create URF data for, from existing string
 * @param pTargetCapacity   fixed string capacity. Set to zero if varying size string (field not used).
 * @return
 */
ZStatus
utfStringHeader::_exportURFAnyGeneric(ZDataBuffer* pTargetURF,
                                        ZTypeBase   pTargetType,
                                        URF_Capacity_type pTargetCapacity) /* if fixed string capacity of fixed string. Set to zero if varying size string */
                                    /* all other values : for fixed string : effective size (URF_Fixed_Size_type)
                                     *                    or for varying string effective size (URF_Varying_Size_type) are set by this routine*/
{
URF_Capacity_type       wSourceCapacity;            /* if fixed : capacity in char units */
URF_Fixed_Size_type     wSourceFUniversalSize,wTargetFUniversalSize;    /* if fixed : effective bytes size */
size_t                  wSourceUnitsCount,wTargetUnitsCount;            /* if fixed : effective units count */
URF_Varying_Size_type   wSourceVUniversalSize,wTargetVUniversalSize;    /* if varying : effective bytes size */
uint64_t                wUGenericSize, wUGenericOriginSize;             /* string bytes size */
uint64_t                wUBytesCapacity;                                /* work area */
ZStatus                 wFinalSt=ZS_SUCCESS;                            /* final status stored to be returned */


unsigned char* wTargetURFPtr,*wSourceDataPtr=(unsigned char*)DataByte;
size_t wByteSize;


    if (!(pTargetType&ZType_String))    /* must be ZString */
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Error,
                              "Invalid target class type. Found %X <%s> while expecting ZType_String",
                              pTargetType,
                              decode_ZType(pTargetType));
        return ZS_INVTYPE;
        }

    ZTypeBase wSourceType= getZType();

    if ((wSourceType&ZType_AtomicMask)!=(pTargetType&ZType_AtomicMask)) // if _Utf (character unit type) is not the same
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Error,
                              "Error: target character unit type 0x%X <%s> has not the same atomic mask as source 0x%X <%s>.",
                              pTargetType,
                              decode_ZType(pTargetType),
                              wSourceType,
                              decode_ZType(wSourceType));
        return ZS_INVTYPE;
        }

    if (wSourceType&ZType_VaryingLength)
            {                                                               /* source is varying string  */
            wSourceVUniversalSize=(URF_Varying_Size_type) getByteSize();    /* get the whole content size as generic size */
            wUGenericSize = (uint64_t)wSourceVUniversalSize;
            wSourceCapacity=0;                                              /* no capacity */
            switch (getZType()&ZType_AtomicMask)                            /* compute units count vs size of atomic */
            {
            case ZType_Char:
            case ZType_UChar:
                wSourceUnitsCount = wUGenericSize/sizeof(char);
                break;
            case ZType_U8:
            case ZType_S8:
                wSourceUnitsCount = wUGenericSize/sizeof(uint8_t);
                break;
            case ZType_S16:
            case ZType_U16:
                wSourceUnitsCount = wUGenericSize/sizeof(uint16_t);
                break;
            case ZType_S32:
            case ZType_U32:
                wSourceUnitsCount = wUGenericSize/sizeof(uint32_t);
                break;
            default:
                return ZS_INVTYPE;
            }
            }// if (wSourceType&ZType_VaryingLength)

        else                                    /* Source is fixed string */
            {                                   /* get effective string size as wSourceUnitsCount, get capacity, get effective byte size */
            switch (getZType()&ZType_AtomicMask)
            {
            case ZType_Char:
            case ZType_UChar:
                wSourceCapacity = getByteSize()/sizeof(uint8_t);
                wSourceUnitsCount = utfStrlen<char>((char*)DataByte);
                wSourceFUniversalSize =wSourceUnitsCount * sizeof(char);
                wUGenericSize = (uint64_t) wSourceFUniversalSize;
                break;
            case ZType_U8:
            case ZType_S8:
                wSourceCapacity = getByteSize()/sizeof(uint8_t);
                wSourceUnitsCount = utfStrlen<utf8_t>((utf8_t*)DataByte);
                wSourceFUniversalSize =wSourceUnitsCount * sizeof(utf8_t);
                wUGenericSize = (uint64_t) wSourceFUniversalSize;
                break;
            case ZType_S16:
            case ZType_U16:
                wSourceCapacity = getByteSize()/sizeof(uint16_t);
                wSourceUnitsCount = utfStrlen<utf16_t>((utf16_t*)DataByte);
                wSourceFUniversalSize = wSourceUnitsCount*sizeof(utf16_t);
                wUGenericSize = (uint64_t) wSourceFUniversalSize;
                break;
            case ZType_S32:
            case ZType_U32:
                wSourceCapacity = getByteSize()/sizeof(uint32_t);
                wSourceUnitsCount = utfStrlen<utf32_t>((utf32_t*)DataByte);
                wSourceFUniversalSize = wSourceUnitsCount * sizeof(uint32_t);
                wUGenericSize = (uint64_t) wSourceFUniversalSize;
                break;
            default:
                return ZS_INVTYPE;
            }

            }// else

    if (pTargetType&ZType_VaryingLength) /* target type is varying ? */
                        goto _exportURFOther_TargetVarying;

// ------------target is fixed string---------------------------

/* compute target capacity in bytes */
    switch (pTargetType&ZType_AtomicMask)
    {
    case ZType_Char:
    case ZType_UChar:
        wUBytesCapacity = pTargetCapacity * sizeof(char);
        if (wUGenericSize > wUBytesCapacity)
                {
                wTargetUnitsCount= pTargetCapacity-1;
                wTargetFUniversalSize=wTargetUnitsCount*sizeof(char);
                wFinalSt=ZS_TRUNCATED;
                break;
                }
        wTargetUnitsCount=(URF_Fixed_Size_type)(wTargetFUniversalSize/sizeof(char));
        wTargetFUniversalSize=(URF_Fixed_Size_type)wUGenericSize;
        break;
    case ZType_U8:
    case ZType_S8:
        wUBytesCapacity = pTargetCapacity * sizeof(uint8_t);
        if (wUGenericSize > wUBytesCapacity)
                {
                wTargetUnitsCount= pTargetCapacity-1;
                wTargetFUniversalSize=wTargetUnitsCount*sizeof(uint8_t);
                wFinalSt=ZS_TRUNCATED;
                break;
                }
        wTargetUnitsCount=(URF_Fixed_Size_type)(wUGenericSize/sizeof(uint8_t));
        wTargetFUniversalSize=(URF_Fixed_Size_type)wUGenericSize;
        break;
    case ZType_S16:
    case ZType_U16:
        wUBytesCapacity = (pTargetCapacity-1) *sizeof(uint16_t);
        if (wUGenericSize > wUBytesCapacity)
                {
                wTargetUnitsCount= pTargetCapacity-1;
                wTargetFUniversalSize=wUBytesCapacity;
                wFinalSt=ZS_TRUNCATED;
                break;
                }

        wTargetUnitsCount=(URF_Fixed_Size_type)(wUGenericSize/sizeof(uint16_t));
        wTargetFUniversalSize=(URF_Fixed_Size_type)wUGenericSize;
        break;
    case ZType_S32:
    case ZType_U32:
        wUBytesCapacity = (pTargetCapacity-1) * sizeof(uint32_t);
        if (wUGenericSize > wUBytesCapacity)
                {
                wTargetUnitsCount= pTargetCapacity-1;
                wTargetFUniversalSize=wUBytesCapacity;
                wFinalSt=ZS_TRUNCATED;
                break;
                }
        wTargetUnitsCount=(URF_Fixed_Size_type)(wUGenericSize/sizeof(uint32_t));
        wTargetFUniversalSize=(URF_Fixed_Size_type)wUGenericSize;
        break;
    default:
        return ZS_INVTYPE;
    }

    wByteSize=wUGenericSize + sizeof(ZTypeBase)+sizeof(URF_Capacity_type)+sizeof(URF_Fixed_Size_type);

    wTargetURFPtr= pTargetURF->allocateBZero(wByteSize);

    wTargetURFPtr=setURFBufferValue<ZTypeBase>(wTargetURFPtr,pTargetType);
    wTargetURFPtr=setURFBufferValue<URF_Capacity_type>(wTargetURFPtr,pTargetCapacity);
    wTargetURFPtr=setURFBufferValue<URF_Fixed_Size_type>(wTargetURFPtr,wTargetFUniversalSize);

    if (wFinalSt==ZS_TRUNCATED)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_TRUNCATED,
                              Severity_Warning,
                              " Target capacity is <%d> bytes while requested string bytes size is <%ld>. String is being truncated.",
                              wTargetFUniversalSize,
                              wUGenericSize);
        }

    switch (getZType()&ZType_AtomicMask)
    {
        case ZType_Char:
        case ZType_UChar:
        {
        utfSetReverse<char>((char*)wTargetURFPtr,
                                (const char*)DataByte,
                                (const size_t)wTargetUnitsCount);// character units count
        return wFinalSt;
        }
        case ZType_U8:
        case ZType_S8:
        {
        while (wTargetUnitsCount--)
                *wTargetURFPtr++=*wSourceDataPtr++;
        return wFinalSt;
        }
        case ZType_U16:
        {
        utfSetReverse<utf16_t>((utf16_t*)wTargetURFPtr,
                                (const utf16_t*)DataByte,
                                (const size_t)wTargetUnitsCount);// character units count
        return wFinalSt;
        }
        case ZType_U32:
        {
        utfSetReverse<utf32_t>((utf32_t*)wTargetURFPtr,
                                (const utf32_t*)DataByte,
                               (const size_t)wTargetUnitsCount);// character units count
        return wFinalSt;
        }
    default:
        break;
    }// switch

    return wFinalSt;  // of no use but...

//---------------Target is varying string--------------------------------------

_exportURFOther_TargetVarying:

    /* up to here : wUGenericSize has source string length in bytes - wSourceUnitsCount has units count */
    wTargetVUniversalSize = (URF_Varying_Size_type)wUGenericSize;

    wTargetURFPtr= pTargetURF->allocateBZero(getByteSize()+sizeof(URF_Varying_Size_type)+ sizeof(ZTypeBase));
    wTargetURFPtr=setURFBufferValue<ZTypeBase>(wTargetURFPtr,wSourceType);
    wTargetURFPtr=setURFBufferValue<URF_Varying_Size_type>(wTargetURFPtr,wTargetVUniversalSize);

    switch (pTargetType&ZType_AtomicMask)
    {
        case ZType_Char:
        {
        utfSetReverse<char>((char*)wTargetURFPtr,
                            (const char*)DataByte,
                            (const size_t)wSourceUnitsCount);

        break;
        }
        case ZType_U8:
        {
        utfSetReverse<utf8_t>((utf8_t*)wTargetURFPtr,
                            (const utf8_t*)DataByte,
                            (const size_t)wSourceUnitsCount);
        break;
        }
        case ZType_U16:
        {
        utfSetReverse<utf16_t>((utf16_t*)wTargetURFPtr,
                            (const utf16_t*)DataByte,
                            (const size_t)wSourceUnitsCount);
        break;
        }
        case ZType_U32:
        {
        utfSetReverse<utf32_t>((utf32_t*)wTargetURFPtr,
                               (const utf32_t*)DataByte,
                               (const size_t)wSourceUnitsCount);// character units count
        break;
        }
    default:
        break;
    }// switch

    return wFinalSt;
}// _exportURFAnyGeneric

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
URF_Fixed_Size_type     wFUniversalSize;
URF_Varying_Size_type   wVUniversalSize;

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

    memmove (&wFUniversalSize,wDataPtr,sizeof(wFUniversalSize));
    wFUniversalSize=reverseByteOrder_Conditional<URF_Fixed_Size_type>(wFUniversalSize);
    wDataPtr += sizeof (URF_Fixed_Size_type);
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

    memmove (&wVUniversalSize,wDataPtr,sizeof(wVUniversalSize));
    wVUniversalSize=reverseByteOrder_Conditional<URF_Varying_Size_type>(wVUniversalSize);
    wDataPtr += sizeof (URF_Varying_Size_type);
//    pUniversal.allocateBZero((size_t)wEffectiveUSize+sizeof(_Utf)); /* allocate size + endofstring mark */
    pUniversal.allocateBZero((size_t)wVUniversalSize); /* allocate size without endofstring mark */
    if (pURFDataPtrOut)
      {
      *pURFDataPtrOut = wDataPtr + wVUniversalSize;
      }

    wUnits=0;
    switch (pType&ZType_AtomicMask)
    {
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
