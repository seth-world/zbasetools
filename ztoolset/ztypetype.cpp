#ifndef ZTYPETYPE_CPP
#define ZTYPETYPE_CPP
#include "ztypetype.h"
#include "zsymbols.h"
#include "utfvaryingstring.h"

//======================Encoding Decoding ZType_type=====================================

bool isKeyEligible(ZTypeBase pType)
{
  if (pType & ZType_VaryingMask)
    return false;
  return true;
}


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
  ZTypeBase wType = pType;
  if (pType==ZType_Nothing)
    return "ZType_Nothing" ;

static char ZStringBuffer1[250];

ZTypeBase wSType= pType & ZType_StructureMask ;
ZTypeBase wAType= pType & ZType_AtomicMask ;
  memset(ZStringBuffer1,0,sizeof(ZStringBuffer1));


  if (wType & ZType_Array) {
    utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Array ");
    wType &= ~ ZType_Array;
  }
  if (wType & ZType_Pointer) {
    utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Pointer ");
    wType &= ~ ZType_Pointer;
  }


  if (wType==ZType_URIString) {
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_URIString ");
      return ZStringBuffer1 ;
  }
 /*   if (pType==ZType_Bool){
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Bool ");
      return ZStringBuffer1 ;
    }
*/
    switch (wType)  /* partial type decoding */
    {
      /* atomic types */
    case ZType_AtomicUChar:
    case ZType_UChar:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_UChar ");
      return ZStringBuffer1 ;
    case ZType_AtomicChar:
    case ZType_Char:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Char ");
      return ZStringBuffer1 ;
/* not to be used anymore
     case ZType_AtomicWUChar:
      return "ZType_AtomicWUChar" ;
    case ZType_AtomicWChar:
      return "ZType_AtomicWChar" ;
*/
    case ZType_AtomicU8:
    case ZType_U8:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicU8 ");
      return ZStringBuffer1 ;
    case ZType_AtomicS8:
    case ZType_S8:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicS8 ");
      return ZStringBuffer1 ;

    case ZType_AtomicU16:
    case ZType_U16:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicU16 ");
      return ZStringBuffer1 ;

    case ZType_AtomicS16:
    case ZType_S16:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicS16 ");
      return ZStringBuffer1 ;

    case ZType_AtomicU32:
    case ZType_U32:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicU32 ");
      return ZStringBuffer1 ;
    case ZType_AtomicS32:
    case ZType_S32:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicS32 ");
      return ZStringBuffer1 ;
    case ZType_AtomicU64:
    case ZType_U64:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicU64 ");
      return ZStringBuffer1 ;
    case ZType_AtomicS64:
    case ZType_S64:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicS64 ");
      return ZStringBuffer1 ;
    case ZType_AtomicFloat:
    case ZType_Float:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicFloat ");
      return ZStringBuffer1 ;
    case ZType_AtomicDouble:
    case ZType_Double:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicDouble ");
      return ZStringBuffer1 ;
    case ZType_AtomicLDouble:
    case ZType_LDouble:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicLDouble ");
      return ZStringBuffer1 ;
    case ZType_Bool:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Bool ");
      return ZStringBuffer1 ;

  /* varying strings */
    case ZType_CharVaryingString:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_CharVaryingString ");
      return ZStringBuffer1 ;
    case ZType_Utf8VaryingString:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Utf8VaryingString ");
      return ZStringBuffer1 ;
    case ZType_Utf16VaryingString:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Utf16VaryingString ");
      return ZStringBuffer1 ;
    case ZType_Utf32VaryingString:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Utf32VaryingString ");
      return ZStringBuffer1 ;

      /* fixed string */
    case ZType_CharFixedString:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZTyZType_CharFixedStringpe_Bool ");
      return ZStringBuffer1 ;
    case ZType_Utf8FixedString:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Utf8FixedString ");
      return ZStringBuffer1 ;
    case ZType_Utf16FixedString:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Utf16FixedString ");
      return ZStringBuffer1 ;
    case ZType_Utf32FixedString:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Utf32FixedString ");
      return ZStringBuffer1 ;

      /* other classes */
    case ZType_bitset:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_bitset ");
      return ZStringBuffer1 ;
    case ZType_bitsetFull:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_bitsetFull ");
      return ZStringBuffer1 ;
    case ZType_ZDate:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_ZDate ");
      return ZStringBuffer1 ;
    case ZType_ZDateFull:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_ZDateFull ");
      return ZStringBuffer1 ;
    case ZType_CheckSum:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_CheckSum ");
      return ZStringBuffer1 ;
    case ZType_Resource:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Resource ");
      return ZStringBuffer1 ;
    case ZType_URIString:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_URIString ");
      return ZStringBuffer1 ;
    case ZType_Blob:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Blob ");
      return ZStringBuffer1 ;
    case ZType_StdString:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_StdString ");
      return ZStringBuffer1 ;

    case ZType_Unknown:
      utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Unknown ");
      return ZStringBuffer1 ;
    default:
      break;
    }

/*
    if (pType&ZType_VaryingLength)
                utfAddConditionalOR<char>(ZStringBuffer1, "ZType_VaryingLength");
            else
                utfAddConditionalOR<char>(ZStringBuffer1,"ZType_FixedLength");
*/
  return ZStringBuffer1;

    while (true)
    {
    if (pType & ZType_Atomic)
            {
//            utfAddConditionalOR<char>(ZStringBuffer1," ZType_Atomic") ;
            ::strcpy((char*)ZStringBuffer1,"ZType_Atomic"); // reset string to this litteral
            break;
            }
    if (pType & ZType_Pointer) {
            utfAddConditionalOR<char>(ZStringBuffer1," ZType_Pointer") ;
//            ::strcpy((char*)ZStringBuffer1,"ZType_Pointer"); // reset string to this litteral
            break;
            }
    if (pType & ZType_Array){
           utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Array") ;
//            ::strcpy((char*)ZStringBuffer1,"ZType_Array"); // reset string to this litteral
            break;
            }
    if ((pType & ZType_Class)==ZType_Class){
            utfAddConditionalOR<char>(ZStringBuffer1," ZType_Class") ;
            break;
            }
/*    if (pType & ZType_Enum)
            {
            ZStringBuffer = "ZType_Enum";
            break;
            }*/

    utfAddConditionalOR<char>(ZStringBuffer1,"ZType_StructUnknown");
    break;
    }// while (true)

/* Herebelow is not necessary :atomic type gives signed and endianness option

    if (pType & ZType_Signed)
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Signed ");
        else
            utfAddConditionalOR<char>(ZStringBuffer1,"UnSigned ");
    if (pType & ZType_Endian)
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Endian ");
*/
    while (true) // carefull to put first signed type otherwise detect unsigned while it is signed (same mask with sign byte )
    {
    if ((pType & ZType_Char)==ZType_Char)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Char") ;
            break;
            }
    if ((pType & ZType_UChar)== ZType_UChar) {
        utfAddConditionalOR<char>(ZStringBuffer1,"ZType_UChar") ;
        break;
        }
/*    if ((pType & ZType_WChar)== ZType_WChar)
        {
        ZStringBuffer1.addConditionalOR("ZType_WChar") ;
        break;
        }
    if ((pType & ZType_WUChar)== ZType_WUChar)
        {
        ZStringBuffer1.addConditionalOR("ZType_WUChar") ;
        break;
        }*/
    if ((pType & ZType_S8)==ZType_S8)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_S8") ;
            break;
            }
    if ((pType & ZType_U8)==ZType_U8)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_U8") ;
            break;
            }

    if ((pType & ZType_S16)==ZType_S16)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_S16") ;
            break;
            }
    if ((pType & ZType_U16)==ZType_U16)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_U16") ;
            break;
            }

    if ((pType & ZType_S32)==ZType_S32)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_S32") ;
            break;
            }
    if ((pType & ZType_U32)==ZType_U32)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_U32") ;
            break;
            }

    if ((pType & ZType_S64)==ZType_S64)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_S64") ;
            break;
            }
    if ((pType & ZType_U64)==ZType_U64)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_U64") ;
            break;
            }

    if ((pType & ZType_Float)==ZType_Float)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Float") ;
            break;
            }
    if ((pType & ZType_Double)==ZType_Double)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_Double") ;
            break;
            }
    if ((pType & ZType_LDouble)==ZType_LDouble)
            {
            utfAddConditionalOR<char>(ZStringBuffer1,"ZType_LDouble") ;
            break;
            }

    utfAddConditionalOR<char>(ZStringBuffer1,"ZType_AtomicUnknown");
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

ZTypeBase encode_ZType (const utf8VaryingString &pString)
{
  ZTypeBase wZType=ZType_Nothing ;
/* atomic types */
  while (true) {
    if (pString.strstr((const utf8_t*)"ZType_Atomic")) {
      wZType|=ZType_Atomic; break ; }

    if (pString.strstr((const utf8_t*)"ZType_Char")) {
      wZType|=ZType_Char; break ; }
    if (pString.strstr((const utf8_t*)"ZType_UChar")) {
      wZType|=ZType_UChar; break ; }
    if (pString.strstr((const utf8_t*)"ZType_S8")) {
      wZType|=ZType_S8; break ; }
    if (pString.strstr((const utf8_t*)"ZType_U8")) {
      wZType|=ZType_U8; break ; }
    if (pString.strstr((const utf8_t*)"ZType_S16")) {
      wZType|=ZType_S16; break ; }
    if (pString.strstr((const utf8_t*)"ZType_U16")) {
      wZType|=ZType_U16; break ; }
    if (pString.strstr((const utf8_t*)"ZType_S32")) {
      wZType|=ZType_S32; break ; }
    if (pString.strstr((const utf8_t*)"ZType_U32")) {
      wZType|=ZType_U32; break ; }
    if (pString.strstr((const utf8_t*)"ZType_S64")) {
      wZType|=ZType_S64; break ; }
    if (pString.strstr((const utf8_t*)"ZType_U64")) {
      wZType|=ZType_U64; break ; }
    if (pString.strstr((const utf8_t*)"ZType_Float")) {
      wZType|=ZType_Float; break ; }
    if (pString.strstr((const utf8_t*)"ZType_Double")) {
      wZType|=ZType_Double; break ; }
    if (pString.strstr((const utf8_t*)"ZType_LDouble")) {
      wZType|=ZType_LDouble; break ; }
    if (pString.strstr((const utf8_t*)"ZType_Bool")) {
      wZType|=ZType_Bool; break ; }

    if (pString.strstr((const utf8_t*)"ZType_AtomicChar")) {
      wZType|=ZType_AtomicChar; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicUChar")) {
      wZType|=ZType_AtomicUChar; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicS8")) {
      wZType|=ZType_AtomicS8; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicU8")) {
      wZType|=ZType_AtomicU8; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicS16")) {
      wZType|=ZType_AtomicS16; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicU16")) {
      wZType|=ZType_AtomicU16; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicS32")) {
      wZType|=ZType_AtomicS32; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicU32")) {
      wZType|=ZType_AtomicU32; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicS64")) {
      wZType|=ZType_AtomicS64; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicU64")) {
      wZType|=ZType_AtomicU64; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicFloat")) {
      wZType|=ZType_AtomicFloat; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicLDouble")) {
      wZType|=ZType_AtomicLDouble; break ; }
    if (pString.strstr((const utf8_t*)"ZType_AtomicLDouble")) {
      wZType|=ZType_AtomicLDouble; break ; }
    if (pString.strstr((const utf8_t*)"ZType_Bool")) {
      wZType|=ZType_Bool; break ; }

    /* strings */
    if (pString.strstr((const utf8_t*)"ZType_Utf8VaryingString")) {
      wZType|=ZType_Utf8VaryingString; break ; }
    if (pString.strstr((const utf8_t*)"ZType_utf8VaryingString")) {
      wZType|=ZType_Utf8VaryingString; break ; }

    if (pString.strstr((const utf8_t*)"ZType_Utf16VaryingString")) {
      wZType|=ZType_Utf16VaryingString; break ; }
    if (pString.strstr((const utf8_t*)"ZType_Utf32VaryingString")) {
      wZType|=ZType_Utf32VaryingString; break ; }

    if (pString.strstr((const utf8_t*)"ZType_Utf32FixedString")) {
      wZType|=ZType_Utf32FixedString; break ; }

    if (pString.strstr((const utf8_t*)"ZType_URIString")) {
      wZType|=ZType_URIString; break ; }

    if (pString.strstr((const utf8_t*)"ZType_Blob")) {
      wZType|=ZType_Blob; break ; }
    if (pString.strstr((const utf8_t*)"ZType_StdString")) {
      wZType|=ZType_StdString; break ; }


    if (pString.strstr((const utf8_t*)"ZType_CharFixedString")) {
      wZType|=ZType_CharFixedString; break ; }
    if (pString.strstr((const utf8_t*)"ZType_Utf8FixedString")) {
      wZType|=ZType_Utf8FixedString; break ; }
    if (pString.strstr((const utf8_t*)"ZType_Utf16FixedString")) {
      wZType|=ZType_Utf16FixedString; break ; }
    if (pString.strstr((const utf8_t*)"ZType_Utf32FixedString")) {
      wZType|=ZType_Utf32FixedString; break ; }

    /* dates */
    if (pString.strstr((const utf8_t*)"ZType_ZDateFull")) {
      wZType|=ZType_ZDateFull; break ; }
    if (pString.strstr((const utf8_t*)"ZType_ZDate")) {
      wZType|=ZType_ZDate; break ; }

    /* other classes */

    if (pString.strstr((const utf8_t*)"ZType_CheckSum")) {
      wZType|=ZType_CheckSum; break ; }
    if (pString.strstr((const utf8_t*)"ZType_bitset")) {
      wZType|=ZType_bitset; break ; }
    if (pString.strstr((const utf8_t*)"ZType_bitsetFull")) {
      wZType|=ZType_bitsetFull; break ; }
    if (pString.strstr((const utf8_t*)"ZType_Resource")) {
      wZType|=ZType_Resource; break ; }

    wZType|=ZType_Unknown;
    break;
  } // while (true)

  /* other complementary attributes */

  if (pString.strstr((const utf8_t*)"ZType_Array"))
    wZType|=ZType_Array;

  if (pString.strstr((const utf8_t*)"ZType_Pointer"))
    wZType|=ZType_Pointer;

  return wZType;
}

bool isAtomic(ZTypeBase pType) {
//  ZTypeBase wType = pType & ZType_AtomicMask;
  switch (pType)  /* partial type decoding */
  {
    /* atomic types */
  case ZType_AtomicUChar:
  case ZType_UChar:
    return true ;
  case ZType_AtomicChar:
  case ZType_Char:
    return true ;

  case ZType_AtomicU8:
  case ZType_U8:
    return true ;
  case ZType_AtomicS8:
  case ZType_S8:
    return true ;

  case ZType_AtomicU16:
  case ZType_U16:
   return true ;

  case ZType_AtomicS16:
  case ZType_S16:
    return true ;

  case ZType_AtomicU32:
  case ZType_U32:
    return true ;
  case ZType_AtomicS32:
  case ZType_S32:
    return true ;
  case ZType_AtomicU64:
  case ZType_U64:
    return true ;
  case ZType_AtomicS64:
  case ZType_S64:
    return true ;
  case ZType_AtomicFloat:
  case ZType_Float:
    return true ;
  case ZType_AtomicDouble:
  case ZType_Double:
    return true ;
  case ZType_AtomicLDouble:
  case ZType_LDouble:
    return true ;
  case ZType_Bool:
    return true ;
  default:
    return false;
  }
}


/**
 * @brief decode_ZType decode the ZType_type value given as a long into a human readable string
* @param[in] pType  ZType_type mask to decode
 * @return a C string
*/
const char *ZTypeToCType (ZTypeBase pType, long pCapacity)
{
  //  pInclude.clear();

  ZTypeBase wType = pType;
  if (pType==ZType_Nothing)
    return "ZType_Nothing" ;
  if (pType==ZType_Unknown)
    return "ZType_Unknown" ;
  static utf8VaryingString ZStringBuffer;
//  static CharMan ZStringBuffer;

  ZTypeBase wSType = pType & ZType_StructureMask ;
  ZTypeBase wAType= pType & ZType_AtomicMask ;

  ZStringBuffer.clear();

  if (pType & ZType_Atomic) {
    switch (pType & ~ (ZType_Array | ZType_Pointer))  /* partial type decoding */
      {
        /* atomic types */
      case ZType_AtomicUChar:
      case ZType_UChar:
        ZStringBuffer = "unsigned char";
        break;
      case ZType_AtomicChar:
      case ZType_Char:
        ZStringBuffer = "char";
        break;
        /* not to be used anymore
       case ZType_AtomicWUChar:
        return "ZType_AtomicWUChar" ;
      case ZType_AtomicWChar:
        return "ZType_AtomicWChar" ;
  */
      case ZType_AtomicU8:
      case ZType_U8:
        ZStringBuffer = "uint8_t";
        break;

      case ZType_AtomicS8:
      case ZType_S8:
        ZStringBuffer = "int8_t";
        break;

      case ZType_AtomicU16:
      case ZType_U16:
        ZStringBuffer = "uint16_t";
        break;

      case ZType_AtomicS16:
      case ZType_S16:
        ZStringBuffer = "int16_t";
        break;


      case ZType_AtomicU32:
      case ZType_U32:
        ZStringBuffer = "uint32_t";
        break;

      case ZType_AtomicS32:
      case ZType_S32:
        ZStringBuffer = "int32_t";
        break;

      case ZType_AtomicU64:
      case ZType_U64:
        ZStringBuffer = "uint64_t";
        break;

      case ZType_AtomicS64:
      case ZType_S64:
        ZStringBuffer = "int64_t";
        break;

      case ZType_AtomicFloat:
      case ZType_Float:
        ZStringBuffer = "float";
        break;

      case ZType_AtomicDouble:
      case ZType_Double:
        ZStringBuffer = "double";
        break;

      case ZType_AtomicLDouble:
      case ZType_LDouble:
        ZStringBuffer = "long double";
        break;

      case ZType_Bool:
        ZStringBuffer = "bool";
        break;

      default:
        ZStringBuffer = "Unknown_atomic_type";
        break;
      }

    if (pType & ZType_Pointer) {
      ZStringBuffer += "* ";
    }
    return ZStringBuffer.toCChar();
  }// if (pType & ZType_Atomic)



//  if (pType & ZType_StructureMask) {

  switch (pType & ~ (ZType_Array | ZType_Pointer)){
    case ZType_URIString:
      ZStringBuffer = "uriString";
//      pInclude = "ztoolset/uristring.h";
      break;
    case ZType_Utf8VaryingString:
      ZStringBuffer = "utf8VaryingString";
//      pInclude = "ztoolset/utfvaryingstring.h";
      break;
    case ZType_Utf16VaryingString:
      ZStringBuffer = "utf16VaryingString";
//      pInclude = "ztoolset/utfvaryingstring.h";
      break;
    case ZType_Utf32VaryingString:
      ZStringBuffer = "utf8VaryingString";
//      pInclude = "ztoolset/utfvaryingstring.h";
      break;
    case ZType_Utf8FixedString:
      ZStringBuffer.sprintf("utf8FixedString<%ld>",pCapacity);
//      pInclude = "ztoolset/utffixedstring.h";
      break;
    case ZType_Utf16FixedString:
      ZStringBuffer.sprintf("utf16FixedString<%ld>",pCapacity);
//      pInclude = "ztoolset/utffixedstring.h";
      break;
    case ZType_Utf32FixedString:
      ZStringBuffer.sprintf("utf32FixedString<%ld>",pCapacity);
//      pInclude = "ztoolset/utffixedstring.h";
      break;
    case ZType_ZDate:
      ZStringBuffer = "ZDate";
//      pInclude = "ztoolset/zdate.h";
      break;
    case ZType_ZDateFull:
      ZStringBuffer = "ZDateFull";
//      pInclude = "ztoolset/zdate.h";
      break;
    case ZType_CheckSum:
      ZStringBuffer = "checkSum";
//      pInclude = "zcrypt/checksum.h";
      break;
    case ZType_MD5:
      ZStringBuffer = "md5";
//      pInclude = "zcrypt/md5.h";
      break;
    case ZType_bitset:
      ZStringBuffer = "ZBitset";
//      pInclude = "ztoolset/zbitset.h";
      break;

    case ZType_StdString:
      ZStringBuffer = "std::string";
//      pInclude = "string";  // do not include <string.h> but <string>
      break;

    case ZType_StdWString:
      ZStringBuffer = "std::wstring";
//      pInclude = "string";  // do not include <string.h> but <string>
      break;

    case ZType_Resource:
      ZStringBuffer = "ZResource";
//      pInclude = "zcommon/zresource.h";
      break;
  /* deprecated */
/*    case ZType_bitsetFull:
      ZStringBuffer = "ZBitsetFull ";
      break;
*/
    default:
      ZStringBuffer = "Unknown_class_type";
      break;
  } // switch

    if (pType & ZType_Pointer) {
      ZStringBuffer += "* ";
    }

    return ZStringBuffer.toCChar();
} // ZTypeToCType

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

    if (pString.bsearch((void *)"ZType_Bool",strlen("ZType_Bool"))>-1)
      pZType|=ZType_Bool;

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
/* not to be used anymore
     if (pString.bsearch((void *)"ZType_FixedWString",strlen("ZType_FixedWString"))>-1)
                    {
                    pZType|=ZType_FixedWString;
                    return ZS_SUCCESS;
                    }
*/
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
/* not to be used anymore
    if (pString.bsearch((void *)"ZType_VaryingWString",strlen("ZType_VaryingWString"))>-1)
                    {
                    pZType|=ZType_VaryingWString;
                    return ZS_SUCCESS;
                    }
*/
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
                            pString.Data);
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

int wStatus = 0;
char *wName = nullptr;
    wName = abi::__cxa_demangle(pName, nullptr, nullptr, &wStatus);

switch (wStatus)
        {
    case 0 :
            {
            strncpy(pOutName,wName,cst_desclen);
            zfree (wName);
            return  ZS_SUCCESS;
            }

    case -1:
        {
        fprintf(stderr,"Demangler cannot allocate memory ");
        if (wName)
            zfree (wName);
        return  ZS_MEMERROR ;
        }
    case -2:
        {
        fprintf(stderr,"Invalid name encountered while demangling <%s>\n",
                                pName);
        if (wName)
                zfree(wName);
        return  ZS_INVNAME ;
        }
    case -3:
        {
        fprintf(stderr,"Invalid arguments passed to demangler\n");
        zfree(wName);
        return  ZS_INVVALUE ;
        }
    }//switch
    return  ZS_SUCCESS;
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
        Localfree(lpMsgBuf);
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
    case ZType_Bool:
      return sizeof(bool);
    default:
        return sizeof(uint8_t);
    }
}//getUnitSize


#endif //
