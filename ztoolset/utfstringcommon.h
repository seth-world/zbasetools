#ifndef UTFSTRINGCOMMON_H
#define UTFSTRINGCOMMON_H

#include <zconfig.h>
#include <ztoolset/ztypetype.h>
#include <ztoolset/zcharset.h>

#include <ztoolset/zlimit.h>

#ifndef __ZCRYPTMETHOD_TYPE__
#define __ZCRYPTMETHOD_TYPE__
enum ZCryptMethod_type : uint8_t
{
    ZCM_Uncrypted       = 0,
    ZCM_Base64          = 1,
    ZCM_AES256          = 2,
    ZCM_SHA256          = 3,
    ZCM_MD5             = 5
};
#endif // __ZCRYPTMETHOD_TYPE__

/**
 * @brief The utfStringDescriptor class common base object for fixed AND varying utf strings.
 */
#pragma pack(push)
#pragma pack(0)

class utfStringDescriptor
{
protected:
    utfStringDescriptor &_copyFrom(const utfStringDescriptor &pIn)
    {
        DataByte = pIn.DataByte;
        UnitCount = pIn.UnitCount;
        ByteSize = pIn.ByteSize;
        ZType = pIn.ZType;
        Charset = pIn.Charset;
        CryptMethod = pIn.CryptMethod;
        littleEndian = pIn.littleEndian;
        return *this;
    }
    utfStringDescriptor& _copyFrom(const utfStringDescriptor* pIn)
    {
        DataByte = pIn->DataByte;
        UnitCount = pIn->UnitCount;
        ByteSize = pIn->ByteSize;
        ZType = pIn->ZType;
        Charset = pIn->Charset;
        CryptMethod = pIn->CryptMethod;
        littleEndian = pIn->littleEndian;
        return *this;
    }
public:
    uint8_t*            DataByte=nullptr;  /** pointer to effective data : content if fixed string or Data if varying string */
    size_t              UnitCount=0;     /** current string capacity in terms of character units */
    size_t              ByteSize=0;      /** effective data size in bytes : updated according alloc/realloc if varying string */
    ZType_type          ZType=ZType_Nothing;
    ZCharset_type       Charset=ZCHARSET_UTF8;
    ZCryptMethod_type   CryptMethod=ZCM_Uncrypted;
    ZBool               littleEndian=false;  // RFFU:this induce we might have native strings with endianness different from system default
public:
    utfStringDescriptor() {}
    utfStringDescriptor(utfStringDescriptor& pIn) { _copyFrom(pIn); }
    utfStringDescriptor(utfStringDescriptor&& pIn) { _copyFrom(pIn); }

    utfStringDescriptor(utfStringDescriptor* pIn) { _copyFrom(pIn); }

    inline size_t getUnitCount(void)  {return UnitCount;} /** returns the current string capacity in character units */
    inline ZTypeBase getZType(void) {return ZType;}
    inline size_t getByteSize(void) {return ByteSize;}
    inline ZCharset_type getCharset(void) {return Charset;}
    inline ssize_t getCharsetUnitSize(void) {return charsetUnitSize(Charset);}
    inline ZCryptMethod_type getCryptMethod(void) {return CryptMethod;}
    inline bool isNull (void) {return DataByte==nullptr;} /** test if any allocation has been made (only used for varying string) */
protected:
    inline ZStatus setCharset(ZCharset_type pCharset)
    {
        if (ZCharsetToUcnv(pCharset)==UCNV_UNSUPPORTED_CONVERTER)
                                                return ZS_INVCHARSET;
        Charset=pCharset;
        return ZS_SUCCESS;
    }
} ;
#pragma pack(pop)

class utfStringHeader: public utfStringDescriptor
{
public:

    using utfStringDescriptor::getUnitCount;


    ZStatus _importURFGeneric(unsigned char* pURFDataPtr) ;
    ZStatus _exportURFGeneric(ZDataBuffer* pURF) ;
    ZStatus _exportURFAnyGeneric(ZDataBuffer* pTargetURF, ZTypeBase   pTargetType,URF_Capacity_type pTargetCapacity);
    static ZStatus getUniversalFromURF(ZType_type pType, unsigned char* pURFDataPtr,ZDataBuffer &pUniversal);
    template <class _Utf>
    _Utf* allocateChars(ssize_t pCharCount);
};
typedef utfStringHeader* utfStringHeaderPtr;


template <class _Utf>
_Utf*
utfStringHeader::allocateChars(ssize_t pCharCount)
{
_MODULEINIT_
    if (!(ZType&ZType_VaryingLength))
        {
        fprintf(stderr,
                "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid type of String %x <%s>. Only varying strings are allowed. \n",
                _GET_FUNCTION_NAME_,
                ZType,
                decode_ZType(ZType));
        _ABORT_ ;
        }
    if ((pCharCount>__INVALID_SIZE__)||(pCharCount<0))
    {
        fprintf(stderr,
                "%s-F-INVALIDSIZE  Fatal error trying to allocate invalid character units %ld \n",
                _GET_FUNCTION_NAME_,
                pCharCount);
        _ABORT_ ;

    }
 size_t wByteSize= (pCharCount) * sizeof(_Utf);

    if (DataByte==nullptr)
        DataByte=(uint8_t*)malloc((size_t)wByteSize);
    else
        {
        DataByte=(uint8_t*)realloc(DataByte,(size_t)wByteSize);
        }
    if (DataByte==nullptr)
    { // ZException not defined yet
        fprintf(stderr,
                "Module   %s\n"
                "Status   %s\n"
                "Severity %s\n"
                "-F-ERRMALLOC  Cannot allocate/reallocate memory of size %ld for buffer\n",
                _GET_FUNCTION_NAME_,
                decode_ZStatus( ZS_MEMERROR),
                decode_Severity( Severity_Fatal),
                wByteSize);
        _ABORT_;
    }
    ByteSize=wByteSize;
    UnitCount = pCharCount;
//    DataByte= (uint8_t*)Data;

    return (_Utf*)DataByte;
}//utfStringHeader::allocateChars


template <class _Tp>
inline unsigned char*
setURFBufferValue(unsigned char*pOutPtr,_Tp pValue)
{
    pValue=reverseByteOrder_Conditional<_Tp>(pValue);
    memmove(pOutPtr,&pValue,sizeof(_Tp));
    pOutPtr+= sizeof(_Tp);
    return pOutPtr;
}

template <class _Tp>
inline unsigned char*
getURFBufferValue(_Tp* pValue,unsigned char*pOutPtr)
{
    _Tp wValue;
    memmove(&wValue,pOutPtr,sizeof(_Tp));
    wValue=reverseByteOrder_Conditional<_Tp>(wValue);
    *pValue= wValue;
    pOutPtr+= sizeof(_Tp);
    return pOutPtr;
}

template <class _Tp>
inline size_t
setURFOffsetValue(unsigned char*pURFBuffer,_Tp pValue,size_t &pOffset)
{
    pValue=reverseByteOrder_Conditional<_Tp>(pValue);
    memmove(&pURFBuffer[pOffset],&pValue,sizeof(_Tp));
    pOffset+= sizeof(_Tp);
    return pOffset;
}

template <class _Tp>
inline size_t
getURFOffsetValue(_Tp* pValue,unsigned char*pURFBuffer,size_t &pOffset)
{
    _Tp wValue;
    memmove(&wValue,&pURFBuffer[pOffset],sizeof(_Tp));
    wValue=reverseByteOrder_Conditional<_Tp>(wValue);
    *pValue= wValue;
    pOffset+= sizeof(_Tp);
    return pOffset;
}

#endif // UTFSTRINGCOMMON_H
