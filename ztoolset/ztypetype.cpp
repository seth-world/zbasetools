#ifndef ZTYPETYPE_CPP
#define ZTYPETYPE_CPP
#include <ztoolset/ztypetype.h>
#include <ztoolset/zsymbols.h>

//======================Encoding Decoding ZType_type=====================================


template <class _Utf>
_Utf* utfAddConditionalOR(_Utf*pBuffer,const _Utf*pString)
{

    if (!pBuffer)
            return pBuffer;
    size_t wLen=0,wi=0;
    while (pBuffer[wLen]&&(wLen < 250))
                    wLen++;

    if ( wLen )
            {
            pBuffer[wLen++]=(_Utf)__SPACE__;
            pBuffer[wLen++]=(_Utf)__OR__;
            pBuffer[wLen++]=(_Utf)__SPACE__;
            }
    while (pString[wi])
            {
            pBuffer[wLen++]=pString[wi++];
            }
    return pBuffer;
}

/**
 * @brief decode_ZType decode the ZType_type value given as a long into a human readable string
* @param[in] pType  ZType_type mask to decode
 * @return a C string
*/
const char *decode_ZType (ZTypeBase pType)
{
static utf8_t ZStringBuffer1[250];

    if (pType==ZType_Nothing)
                return "ZType_Nothing" ;

ZTypeBase wSType= pType & ZType_StructureMask ;
ZTypeBase wAType= pType & ZType_AtomicMask ;
    memset(ZStringBuffer1,0,sizeof(ZStringBuffer1));

    if (wSType & ZType_ByteSeq)
    {
        if (pType==ZType_Blob)
                    return "ZType_Blob";
        utfAddConditionalOR<utf8_t>(ZStringBuffer1,(const utf8_t*)"ZType_ByteSeq ");
    }
    if (wSType & ZType_bit)
        {
        if (wSType == ZType_bitset)
                    return "ZType_bitset";
        utfAddConditionalOR<utf8_t>(ZStringBuffer1,(const utf8_t*)"ZType_bit ");
        }
    if (wSType & ZType_Array)
                    utfAddConditionalOR<utf8_t>(ZStringBuffer1,(const utf8_t*)"ZType_Array ");

    if ((pType & ZType_String)==ZType_String)
        {
        if ((pType&ZType_Utf8VaryingString)==ZType_Utf8VaryingString)
                                        return "ZType_Utf8VaryingString ";
        if ((pType&ZType_Utf16VaryingString)==ZType_Utf16VaryingString)
                                        return "ZType_Utf16VaryingString ";
        if ((pType&ZType_Utf32VaryingString)==ZType_Utf32VaryingString)
                                        return "ZType_Utf32VaryingString ";
        if ((pType&ZType_CharVaryingString)==ZType_CharVaryingString)
                                        return "ZType_CharVaryingString ";

        if ((pType&ZType_CharFixedString)==ZType_CharFixedString)
                                        return "ZType_CharFixedString ";

        if ((pType&ZType_Utf8FixedString)==ZType_Utf8FixedString)
                                        return "ZType_Utf8FixedString ";
        if ((pType&ZType_Utf16FixedString)==ZType_Utf16FixedString)
                                        return "ZType_Utf16FixedString ";
        if ((pType&ZType_Utf32FixedString)==ZType_Utf32FixedString)
                                        return "ZType_Utf32FixedString ";


        if (pType==ZType_StdString)
                    return "ZType_StdString ";
        if (pType==ZType_FixedCString)
                    return "ZType_FixedCString ";
        if (pType==ZType_FixedWString)
                    return "<Deprecated> ZType_FixedWString ";
        if (pType==ZType_FixedWUString)
                    return "<Deprecated> ZType_FixedWUString ";
        if (pType==ZType_CString)
                    return "<Deprecated>ZType_Pointer | ZType_CString ";
        if (pType==ZType_WString)
                    return "<Deprecated>ZType_Pointer | ZType_WString ";
        if (pType==ZType_VaryingCString)
                    return "<Deprecated>ZType_VaryingCString ";
        if (pType==ZType_VaryingWString)
                    return "<Deprecated>ZType_VaryingWString ";
        return "Unknown String type";
        }
    if ((pType & ZType_Class)==ZType_Class)
        {
        if (pType==ZType_ZDate)
                    {
                    return "ZType_ZDate";
                    }
        if (pType==ZType_ZDateFull)
                    {
                    return "ZType_ZDateFull";
                    }
        if (pType==ZType_CheckSum)
                    return "ZType_CheckSum ";
        }
    if (pType&ZType_VaryingLength)
                utfAddConditionalOR<utf8_t>(ZStringBuffer1, (utf8_t*)"ZType_VaryingLength");
            else
                utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_FixedLength");

    while (true)
    {
    if (pType & ZType_Atomic)
            {
//            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)" ZType_Atomic") ;
            ::strcpy((char*)ZStringBuffer1,"ZType_Atomic"); // reset string to this litteral
            break;
            }
    if (pType & ZType_Pointer)
            {
//            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)" ZType_Pointer") ;
            ::strcpy((char*)ZStringBuffer1,"ZType_Pointer"); // reset string to this litteral
            break;
            }
    if (pType & ZType_Array)
            {
//            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_Array") ;
            ::strcpy((char*)ZStringBuffer1,"ZType_Array"); // reset string to this litteral
            break;
            }
    if ((pType & ZType_Class)==ZType_Class)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)" ZType_Class") ;
            break;
            }
/*    if (pType & ZType_Enum)
            {
            ZStringBuffer = "ZType_Enum";
            break;
            }*/

    utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_StructUnknown");
    break;
    }// while (true)

/* Herebelow is not necessary :atomic type gives signed and endianness option

    if (pType & ZType_Signed)
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_Signed ");
        else
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"UnSigned ");
    if (pType & ZType_Endian)
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_Endian ");
*/
    while (true) // carefull to put first signed type otherwise detect unsigned while it is signed (same mask with sign byte )
    {
    if ((pType & ZType_Char)==ZType_Char)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_Char") ;
            break;
            }
    if ((pType & ZType_UChar)== ZType_UChar)
        {
        utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_UChar") ;
        break;
        }
/*    if ((pType & ZType_WChar)== ZType_WChar)
        {
        ZStringBuffer1.addConditionalOR((utf8_t*)"ZType_WChar") ;
        break;
        }
    if ((pType & ZType_WUChar)== ZType_WUChar)
        {
        ZStringBuffer1.addConditionalOR((utf8_t*)"ZType_WUChar") ;
        break;
        }*/
    if ((pType & ZType_S8)==ZType_S8)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_S8") ;
            break;
            }
    if ((pType & ZType_U8)==ZType_U8)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_U8") ;
            break;
            }

    if ((pType & ZType_S16)==ZType_S16)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_S16") ;
            break;
            }
    if ((pType & ZType_U16)==ZType_U16)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_U16") ;
            break;
            }

    if ((pType & ZType_S32)==ZType_S32)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_S32") ;
            break;
            }
    if ((pType & ZType_U32)==ZType_U32)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_U32") ;
            break;
            }

    if ((pType & ZType_S64)==ZType_S64)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_S64") ;
            break;
            }
    if ((pType & ZType_U64)==ZType_U64)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_U64") ;
            break;
            }

    if ((pType & ZType_Float)==ZType_Float)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_Float") ;
            break;
            }
    if ((pType & ZType_Double)==ZType_Double)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_Double") ;
            break;
            }
    if ((pType & ZType_LDouble)==ZType_LDouble)
            {
            utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_LDouble") ;
            break;
            }

    utfAddConditionalOR<utf8_t>(ZStringBuffer1,(utf8_t*)"ZType_AtomicUnknown");
    break;
    }// while (true)

    if (!ZStringBuffer1[0])
            {
            const char* wUType="Unknown ZType";
            size_t wi=0;
            while (wUType[wi])
                {
                ZStringBuffer1[wi]=(utf8_t)wUType[wi];
                wi++;
                }
            }//if (!ZStringBuffer1[0])

    return (const char*)ZStringBuffer1 ;

} // decode_ZType

#include <ztoolset/zexceptionmin.h>
#include <ztoolset/zdatabuffer.h>

/**
 * @brief encodeZTypeFromString returns a ZTypeBase value from text string containing ZType_xxx keywords
 * @param pZType
 * @param pString
 * @return
 */
ZStatus
encodeZTypeFromString (ZTypeBase &pZType ,ZDataBuffer &pString)
{

    pZType=ZType_Nothing;

    if (pString.bsearch((void *)"ZType_ZDate",strlen("ZType_ZDate"))>-1)
                    pZType|=ZType_ZDate;
    if (pString.bsearch((void *)"ZType_CheckSum",strlen("ZType_ZDate"))>-1)
                    pZType|=ZType_CheckSum;

    if (pString.bsearch((void *)"ZType_Atomic",strlen("ZType_Atomic"))>-1)
                    pZType|=ZType_Atomic;

    if (pString.bsearch((void *)"ZType_Array",strlen("ZType_Array"))>-1)
                    pZType|=ZType_Array;

    if (pString.bsearch((void *)"ZType_Pointer",strlen("ZType_Pointer"))>-1)
                    pZType|=ZType_Pointer;

/*    if (pString.bsearch((void *)"ZType_Enum",strlen("ZType_Enum"))>-1)
                    pZType=ZType_Enum;
*/
    if (pString.bsearch((void *)"ZType_ByteSeq",strlen("ZType_ByteSeq"))>-1)
                    pZType|=ZType_ByteSeq;

    if (pString.bsearch((void *)"ZType_Class",strlen("ZType_Class"))>-1)
                    {
                    pZType|=ZType_Class;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_StdString",strlen("ZType_StdString"))>-1)
                    {
                    pZType=ZType_StdString;
                    return ZS_SUCCESS;
                    }

    if (pString.bsearch((void *)"ZType_bit",strlen("ZType_bit"))>-1)
                    {
                    pZType|=ZType_bit;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_bitset",strlen("ZType_bitset"))>-1)
                    {
                    pZType|=ZType_bitset;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_FixedCString",strlen("ZType_FixedCString"))>-1)
                    {
                    pZType|=ZType_FixedCString;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_FixedWString",strlen("ZType_FixedWString"))>-1)
                    {
                    pZType|=ZType_FixedWString;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_Blob",strlen("ZType_Blob"))>-1)
                    {
                    pZType|=ZType_Blob;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_VaryingCString",strlen("ZType_VaryingCString"))>-1)
                    {
                    pZType|=ZType_VaryingCString;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_VaryingWString",strlen("ZType_VaryingWString"))>-1)
                    {
                    pZType|=ZType_VaryingWString;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_UChar",strlen("ZType_UChar"))>-1)
                    {
                    pZType|=ZType_UChar;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_Char",strlen("ZType_Char"))>-1)
                    {
                    pZType|=ZType_Char;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_U8",strlen("ZType_U8"))>-1)
                    {
                    pZType|=ZType_U8;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_S8",strlen("ZType_S8"))>-1)
                    {
                    pZType|=ZType_S8;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_U16",strlen("ZType_U16"))>-1)
                    {
                    pZType|=ZType_U16;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_S16",strlen("ZType_S16"))>-1)
                    {
                    pZType|=ZType_S16;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_U32",strlen("ZType_U32"))>-1)
                    {
                    pZType|=ZType_U32;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_S32",strlen("ZType_S32"))>-1)
                    {
                    pZType|=ZType_S32;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_U64",strlen("ZType_U64"))>-1)
                    {
                    pZType|=ZType_U64;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_S64",strlen("ZType_S64"))>-1)
                    {
                    pZType|=ZType_S64;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_Float",strlen("ZType_Float"))>-1)
                    {
                    pZType|=ZType_Float;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_Double",strlen("ZType_Double"))>-1)
                    {
                    pZType|=ZType_Double;
                    return ZS_SUCCESS;
                    }
    if (pString.bsearch((void *)"ZType_LDouble",strlen("ZType_LDouble"))>-1)
                    {
                    pZType|=ZType_LDouble;
                    return ZS_SUCCESS;
                    }

    ZException.setMessage(_GET_FUNCTION_NAME_,
                            ZS_INVTYPE,
                            Severity_Error,
                            " Given type has an invalid or unknown atomic data type <%s>",
                            pString.DataChar);
    return ZS_INVTYPE;

}//encodeZTypeFromString


//=================== Demangling names=========================================

#ifdef __GNUG__

#include <cxxabi.h>
/**
 * @brief typeDemangler demangle a data type name : gives the human readable standard data type name from a local type name
 * @param[in] pName     Type name to demangle as a CString
 * @param[out] pOutName Demangled name as an utfdescString content as a char* with maximum cst_desclen
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSErrorSError
 */
ZStatus typeDemangler(const char*pName, char* pOutName, size_t pOutSizeMax)
{
_MODULEINIT_
int wStatus = 0;
char *wName = nullptr;
    wName = abi::__cxa_demangle(pName, nullptr, nullptr, &wStatus);

switch (wStatus)
        {
    case 0 :
            {
            strncpy(pOutName,wName,cst_desclen);
            free (wName);
            _RETURN_ ZS_SUCCESS;
            }

    case -1:
        {
        fprintf(stderr,"Demangler cannot allocate memory ");
        if (wName)
            free (wName);
        _RETURN_ ZS_MEMERROR ;
        }
    case -2:
        {
        fprintf(stderr,"Invalid name encountered while demangling <%s>\n",
                                pName);
        if (wName)
                free(wName);
        _RETURN_ ZS_INVNAME ;
        }
    case -3:
        {
        fprintf(stderr,"Invalid arguments passed to demangler\n");
        free(wName);
        _RETURN_ ZS_INVVALUE ;
        }
    }//switch
    _RETURN_ ZS_SUCCESS;
}// typeDemangler

#endif// __GNUG__

#ifdef __USE_WINDOWS__
//=================== Demangling names=========================================

#include <Windows.h>
#include <Dbghelp.h>
/**
 * @brief typeDemangler demangle a data type name : gives the human readable standard data type name from a local type name
 * @param[in] pName     Type name to demangle as a CString
 * @param[out] pOutName Demangled name as a descString
 * @return  a ZStatus. In case of error, ZStatus is returned and ZException is set with appropriate message.see: @ref ZBSErrorSError
 */
ZStatus
typeDemangler(const char*pName,char* pOutName)
{
int wStatus = 0;
//! initializing symbols
//!
//!
    DWORD  error;
    HANDLE hProcess;

    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);

    hProcess = GetCurrentProcess();

    wStatus = SymInitialize(hProcess, NULL, TRUE);
    if (!wStatus)
    {
        // SymInitialize failed
        error = GetLastError();

        LPVOID lpMsgBuf;
//        LPVOID lpDisplayBuf;

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVTYPE,
                                Severity_Fatal,
                                "Error <%d> %s",
                                error,
                                lpMsgBuf
                                );
        LocalFree(lpMsgBuf);
        ZException.exit_abort();
    }



    wStatus = UnDecorateSymbolName(pName, pOutName, cst_desclen, UNDNAME_COMPLETE);

    if (wStatus==0)
        {
        LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = GetLastError();

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

        ZException.setMessage(_GET_FUNCTION_NAME_,
                                ZS_INVTYPE,
                                Severity_Fatal,
                                "Error <%d> %s",
                                dw,
                                lpMsgBuf
                                );
        LocalFree(lpMsgBuf);
        ZException.exit_abort();
        }

    return ZS_SUCCESS;
}// typeDemangler

#endif // __USE_WINDOWS__

ZStatus typeDemangler(const char *pName, CharMan &pOutName)
{
    return typeDemangler(pName, pOutName.content, sizeof(pOutName.content));
}


inline
size_t
getUnitSize(ZType_type pType)
{
    switch (pType&ZType_AtomicMask)
    {
    case ZType_Char:
    case ZType_UChar:
        return sizeof(char);
    case ZType_U8:
    case ZType_S8:
        return sizeof(uint8_t);
    case ZType_U16:
    case ZType_S16:
        return sizeof(uint16_t);
    case ZType_U32:
    case ZType_S32:
        return sizeof(uint32_t);
    default:
        return sizeof(uint8_t);
    }
}//getUnitSize


#endif //
