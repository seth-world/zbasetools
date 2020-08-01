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
const char * decode_ZSA_Action (ZSA_Action &ZS);

const char * decode_ZOperation (ZOp &pOp);

const char * decode_ZAMState (ZAMState_type pZAM);



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
const char* decode_ZLockMask(zlock_type pLock);

const char* decode_ZST(char pZST);
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

#define __STDSTRING__ "std::string"
//#include <cxxabi.h>

void typeDemangler(const char*pName, char *pOutName);

template <typename _Type>
ZData_Type_struct &_get_Data_Type (ZData_Type_struct*pDT_Return)
    {
//struct ZData_Type_struct DT_Return ;

         const char *_Type_Name_Local;

         memset (pDT_Return,0,sizeof(ZData_Type_struct));

         pDT_Return->Size = sizeof(_Type);

         pDT_Return->isPointer=std::is_pointer<_Type>::value ;
         if (pDT_Return->isPointer)
                    pDT_Return->Type |= Zpointer ;


        pDT_Return->isArray=std::is_array<_Type>::value ;
        if (pDT_Return->isArray)
                         {
                        pDT_Return->Type |= Zarray;
                         }


        _Type_Name_Local = typeid(_Type).name();

#ifdef __DEPRECATED__
        int wmangling_status ;
        size_t wmangling_length=199;
/*
  *  WARNING : If _Type has a name that overrides the buffer length (199), a SIGABRT will be thrown (setTerminate() is called)
  *             when calling     __cxa_demangle  (Valgrind says : invalid free operation)
  *  SOLUTION : enlarge TypeName field size
  */
        memset(pDT_Return->TypeName,0,wmangling_length);
        char * wg= abi::__cxa_demangle(_Type_Name_Local,
                                        pDT_Return->TypeName,
                                        &wmangling_length,
                                        &wmangling_status);

        if (wg==NULL)
                {
                pDT_Return->Type = ZerroredType ;
                return (*pDT_Return) ;
                }
#endif // __DEPRECATED__
char wTypeName[200];

    typeDemangler(_Type_Name_Local,(char*)&wTypeName);
/*    if (wSt==ZS_INVTYPE)
            {
            ZException.printLastUserMessage(stderr);
            pDT_Return->Type |= Zunknown ;
            return (*pDT_Return) ;
            }*/
    strcpy(wTypeName,pDT_Return->TypeName);
    while (true)
    {

     if (typeid(_Type).__is_function_p())
                                        {
                                        pDT_Return->Type |= Zfunction;
                                        break;
                                        }

     if (std::is_fundamental<_Type>() )
                {
            pDT_Return->Type |= Zatomic;
            pDT_Return->isAtomic = true;
                 }
                else
                 {
                 pDT_Return->Type |= Zcompound ;
                 pDT_Return->isCompound = true;
                 if (strncmp(pDT_Return->TypeName,__STDSTRING__,strlen(__STDSTRING__))==0)
                                {
                                pDT_Return->isStdString =true;
                                pDT_Return->Type |= ZStdString;
                                return(*pDT_Return);
                                }

                  }

     if (!pDT_Return->isPointer)
                       pDT_Return->Type |= Znumeric ;


       if (std::is_floating_point<_Type>())
                                    {
                                    pDT_Return->Type |= Zfloat;
                                    break;
                                    }
       if (strncasecmp(wTypeName,"long",4)==0)
                                {
                       pDT_Return->Type |= Zlong;
                       break;
                                }
       if (strncasecmp(wTypeName,"double",6)==0)
                                {
                       pDT_Return->Type |= Zdouble;
                       break;
                                }
       if (strncasecmp(wTypeName,"int",3)==0)
                                {
                       pDT_Return->Type |= Zint;
                       break;
                                }
       if (strncasecmp(wTypeName,"char",4)==0)
                                {
                        if (pDT_Return->isPointer)
                                    {
                                    pDT_Return->isCString = true ;
                                    }
                       pDT_Return->Type |= Zchar;
                       break;
                                }
       if (strncasecmp(pDT_Return->TypeName,__STDSTRING__,strlen(__STDSTRING__)))
                      {
                      pDT_Return->isStdString =true;
                      pDT_Return->Type |= ZStdString;
                      break;
                      }

    pDT_Return->Type |= Zunknown ; // unknow atomic type (may be a struct or a class)
    break;
    }

    return (*pDT_Return) ;
} // getDataType



#endif // ZFUNCTIONS_H

