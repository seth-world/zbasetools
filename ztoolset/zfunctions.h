#ifndef ZFUNCTIONS_H
#define ZFUNCTIONS_H

#include <zconfig.h>
#include <time.h>
#include <ztoolset/cescapedstring.h>
#include <ztoolset/ztoolset_common.h>

char *findLastNotChar(char *pContent,char pChar);
bool isTerminatedBy(const char *pContent,char pChar);
void conditionalNL(char *pString) ;

const char * _firstNotinSet (const char*pString,const char *pSet);
const char * _firstinSet (const char*pString,const char *pSet);
const char * _firstNotinSet (const char*pString,const char *pSet);
const char * _lastinSet(const char *pStr, const char *pSet);

char * _LTrim (char*pString, const char *pSet);
char * _RTrim (char*pString, const char *pSet);
char * _Trim (char*pString, const char *pSet);

const char * _strchrReverse(const char *pStr,const char pChar);
char * _toUpper(const char *pStr, char *pOutStr);
char * _expurgeSet( const char *pString, const char *pSet);
char *_expurgeString(const char *pInString, const char *pSubString);

char *_printStdStringField(void *pField, bool IsPointer, char *pBuf);
namespace zbs
{
char *_printStdStringField(void *pField, bool IsPointer, char *pBuf);
}
const char* _Zsprintf(char *pBuf,const char* pFormat,...);

//=============== wchar_t routines================================
wchar_t* findWLastNonChar(const wchar_t *pContent,wchar_t pChar);
wchar_t *findWLastNonChar(const wchar_t *pContent,const wchar_t* pChar);

bool isWTerminatedBy(wchar_t *pContent,const wchar_t* pChar);
bool isWTerminatedBy(const wchar_t *pContent,wchar_t pChar);
void conditionalWNL(wchar_t *pString,const ssize_t pSizeMax=-1);

const wchar_t *_WfirstNotinSet(const wchar_t *pString, const wchar_t *pSet);
const wchar_t *_WfirstinSet(const wchar_t *pString, const wchar_t *pSet);
const char * _WfirstNotinSet (const char*pString,const char *pSet);
const wchar_t *_WlastinSet(const wchar_t *pStr, const wchar_t *pSet);

wchar_t *_WLTrim(wchar_t *pString, const wchar_t *pSet);
wchar_t *_WRTrim(wchar_t *pString, const wchar_t *pSet);
wchar_t *_WTrim(wchar_t *pString, const wchar_t *pSet);

const wchar_t *_WstrchrReverse(const wchar_t *pStr, const wchar_t pChar);
wchar_t * _WtoUpper(wchar_t *pStr,wchar_t *pOutStr);
wchar_t *_WexpurgeSet(wchar_t *pString, const wchar_t *pSet);
wchar_t *_WexpurgeString(wchar_t *pString, const wchar_t *pSubString);

//=================End wchar_t routines=================================

char *dumpSequence (void *pPtr,size_t pSize,char *pBuffer);
char *dumpSequenceHexa (void *pPtr,long pSize,char* pBuffer,char* pBuffer1);

const char * decode_ZStatus (ZStatus ZS);


/* migrated to zam/zam_include.h  */
//const char * decode_ZOperation (ZOp &pOp);


double timeval_dif (timeval &pbeg,timeval &pend);


template <typename _ReturnTp>
_ReturnTp HCode(char *pStr);




inline long& _Max(long& pVal1,long& pVal2) {return ((pVal1>pVal2)?pVal1:pVal2);}
inline size_t& _Max(size_t& pVal1,size_t& pVal2) {return ((pVal1>pVal2)?pVal1:pVal2);}

inline long _Min(long pVal1,long pVal2) {return ((pVal1<pVal2)?pVal1:pVal2);}
inline size_t _Min(size_t pVal1,size_t pVal2) {return ((pVal1<pVal2)?pVal1:pVal2);}

template <typename _Type>
ZData_Type_struct &_get_Data_Type (ZData_Type_struct*pDT_Return);


//-------------decoding codes-----------------

const char * decode_ZStatus (ZStatus ZS);

const char* decode_ZST(ZSort_Type pZST);
ZSort_Type  encode_ZST(const char* pZST);



//--------------Spawn and child process section-------------------------------------------
#define __ZSPAWN_SUCCESS__   0
#define __ZSPAWN_ERROR__    -1
#define __ZSPAWN_TIMEOUT__  -2


#define __ZSPAWN_MAX_ARG__ 10

int set_default_Directory (const char *pDefaultdirectory);

struct ZSpawn_Return {
    int     Status;
    int     ChildStatus;
};

int ZLaunch(const char* pPath, char * const argv[], const char *pDirectory=nullptr) ;
ZSpawn_Return ZSpawn_Program(const char* pPath, char * const argv[],const char *pDirectory=nullptr,const int pMilliSecondsTimeout=2000 ) ;


bool isRoot();

#ifdef __USE_WINDOWS__

void _getLastWindowsError (int &pErr,char* pMessage,size_t pAvailableSize); /* see zuser.cpp */

uid_t getuid();
uid_t geteuid();

#endif // __USE_WINDOWS__


//#include <ztoolset/zutfstrings.h>
//#include <ztoolset/zexceptionmin.h>

class utf8VaryingString;

/** @brief versions Versions are defined with 3 elements : <version number>.<release major>-<release minor>
 *  these elements are packed in an unsigned long variable
 */

/** @brief getVersionStr() formats a version/major/minor to string from an unsigned long */
utf8VaryingString getVersionStr (unsigned long pVersion); /* instantiated within <ztoolset/zfunctions.cpp> */

/** @brief getVersionNum() get an unsigned long from a string containing version/major/minor */
unsigned long getVersionNum (const utf8VaryingString& pVersion); /* instantiated within <ztoolset/zfunctions.cpp> */




#endif // ZFUNCTIONS_H
