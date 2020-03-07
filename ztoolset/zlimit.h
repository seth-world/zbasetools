#ifndef ZLIMIT_H
#define ZLIMIT_H
#include <unistd.h>
#include <cstdint>
#include <stddef.h>
#include <limits.h>     // for utfFmt

#include <openssl/sha.h>  // required for checksum MD5_DIGEST_LENGTH & SHA256_DIGEST_LENGTH
#include <openssl/md5.h> // required for md5


#define __WCHAR_SIZE__ sizeof(__WCHAR_TYPE__);

#define __STRING_MAX_LENGTH__  ULONG_MAX

constexpr int cst_urilen = 2048 ;
constexpr int cst_codelen       = 150  ;
constexpr int cst_desclen       = 250  ;
constexpr int cst_fieldnamelen  = 50  ;
//constexpr int cst_urilen        = 255  ;
constexpr int cst_identitylen   = 255  ; // sysconf(_SC_LOGIN_NAME_MAX)

constexpr int cst_keywordlen    = 300  ;
constexpr int cst_messagelen    = 512  ;
//constexpr long cst_exceptionstringlen = 512  ;
const size_t _cst_default_allocation = 10 ;
const size_t _cst_realloc_quota = 5 ;
const uint8_t _HighValue = 0xFF ;

const int cst_lowercase_begin = 97;
const int cst_lowercase_end = 122;
const int cst_uppercase_begin = 65;
const int cst_uppercase_end = 90;
const int cst_upperization = 32;        // conversion value from lowercase to uppercase letter (should be subtracted from lowercase char to get uppercase)
const char cst_spaceChar = 0x20;


static constexpr int cst_checksum = SHA256_DIGEST_LENGTH;// could be SHA512_DIGEST_LENGTH
static constexpr int cst_checksumHex   = cst_checksum*2 ;

const size_t cst_md5 = MD5_DIGEST_LENGTH;
const size_t cst_md5Hex = cst_md5*2;
/**
 * constants : journaling - index descriptors - journaling history
 */
const size_t _cst_journaling_allocation     = 20; // 20 operations in journaling before claiming for new allocation
const size_t _cst_journaling_reallocquota   = 10; // 10 operations block for space reallocation

const size_t _cst_descriptor_allocation     = 5 ;  // 5 indexes allocation space
const size_t _cst_descriptor_reallocquota   = 2 ;  // 2 indexes incremental number

const size_t _cst_history_allocation = 20 ;
const size_t _cst_history_reallocquota = 5 ;

/** types for URF UVF sizes */

typedef uint16_t UVF_Size_type;             /* Universal Varying Format content size type */
/* Fixed Strings */
typedef uint16_t URF_Fixed_Size_type;      /* Universal Record Format content effective size type (fixed strings): Effective content size */
typedef uint16_t URF_Capacity_type;       /* Universal Record Format canonical size */
/* Arrays */
typedef uint16_t URF_Array_Count_type;      /* Universal Record Format array count : number of array occurrences */
/* Varying strings */
typedef uint64_t URF_Varying_Size_type;     /* Universal Record Format Varying string content size */
/* icu::Collator sort key size type */
typedef uint32_t SortKey_Size_type;          /* Collation key content size type */
typedef uint32_t URF_Date_Size_type;         /* Date size type */

typedef uint8_t* utfKeyData_type;


typedef uint8_t     utf8_t;
typedef uint16_t    utf16_t;
typedef uint16_t    ucs2_t;
typedef uint32_t    utf32_t;
typedef uint32_t    ucs4_t;

typedef uint8_t ZBool;


static const ZBool ZFalse=false;
static const ZBool ZTrue=true;

typedef utf8_t      utfFmt;     /* utfSprintf  format argument data type */


#endif // ZLIMIT_H

