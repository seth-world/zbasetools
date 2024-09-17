#ifndef ZERROR_MIN_H
#define ZERROR_MIN_H
/*
 *  Contains error management, status error codes and information message level management
 *
 */
#include <config/zconfig.h>

#include <stddef.h>
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include "zlimit.h"
#include "zstatus.h"

//#include <ztoolset/zarray.h>   // do not include zarray.h in this file

//=========================Error codes======================================

//      POSIX errno code description from errno value

struct POSIXerr_struct {const char* Str;int Errno; } ;
extern POSIXerr_struct ZErrno [];

const char * decode_POSIXerrno(int pErrno);



//      POSIX Signal description from signal code

extern const char *ZSignal_Description[][2];

#include <signal.h>

const char* decode_Signal(int pSignal);
const char* decode_SignalLib(int pSignal);

#define _DBGPRINT(...) { fprintf (stdout, __VA_ARGS__); std::cout.flush(); }
#define _ERRPRINT(...) { fprintf (stderr, __VA_ARGS__); std::cout.flush(); }

#define _ASSERT_(__COND__,__MAIN__,...) \
    if (__COND__) {  \
        fprintf(stderr,__MAIN__ __VA_OPT__(,) __VA_ARGS__); \
        exit(EXIT_FAILURE);\
    }

#define _ABORT_ exit(EXIT_FAILURE);

/*

#define __ZBASICVERBOSE__ ()
#define _SET_ZBASICVERBOSE_ addVerbose( ZVB_Basic);
#define _CLEAR_ZBASICVERBOS_ clearVerbose(ZVB_Basic);


#define __ZNETVERBOSE__ (ZVerbose & ZVB_Net)
#define _SET_ZNETVERBOSE_ addVerbose( ZVB_Net);
#define _CLEAR_ZNETVERBOSE_ clearVerbose(ZVB_Net);


#define __ZNETPERFORMANCE__ (ZVerbose & ZVB_NetStats)
#define _SET_ZNETPERFORMANCE_ addVerbose( ZVB_NetStats);
#define _CLEAR_ZNETPERFORMANCE_ clearVerbose(ZVB_NetStats);

#define __ZMUTEXVERBOSE__ (ZVerbose & ZVB_Mutex)
#define _SET_ZMUTEXVERBOSE_ addVerbose(ZVB_Mutex);
#define _CLEAR_ZMUTEXVERBOSE_ clearVerbose(ZVB_Mutex;

#define __ZRFVERBOSE__ (ZVerbose & ZVB_ZRF)
#define _SET_ZRFVERBOSE_ addVerbose( ZVB_ZRF);
#define _CLEAR_ZRFVERBOSE_ clearVerbose(ZVB_ZRF);

#define __ZMFVERBOSE__  (ZVerbose & ZVB_ZMF)
#define _SET_ZMFVERBOSE_ addVerbose( ZVB_ZMF);
#define _CLEAR_ZMFVERBOSE_ clearVerbose(ZVB_ZMF);

#define __ZIFVERBOSE__ (ZVerbose & ZVB_ZIF)
#define _SET_ZIFVERBOSE_ addVerbose( ZVB_ZIF);
#define _CLEAR_ZIFVERBOSE_ clearVerbose(ZVB_ZIF);

#define __ZQTVERBOSE__ (ZVerbose & ZVB_QT)
#define _SET_ZQTVERBOSE_ addVerbose( ZVB_QT);
#define _CLEAR_ZQTVERBOSE_ ZclearVerbose(ZVB_QT);
*/
/**
 * @defgroup  ZBSErrorGroup Errors and exceptions management
 *
 * @{
 * @weakgroup ZBSErrorCategoriesGroup  Errors categories and Error/information messages reporting mode
 *
 * Errors are reported on stderr.\n
 * Information messages are always reported to stdout.\n
 * Fatal errors are reported in any cases to stderr.\n
 *
 * @brief \_\_DEBUG_LEVEL\_\_ is a preprocessor parameter that allows to show or hide errors/warnings according to their severity.\n
 *                  This parameter must be defined in zconfig.h file
 *
 *
 * \_\_DEBUG_LEVEL\_\_ value
 *  -            Fatal errors are reported in any cases
 *
 *  -  3     >2  Severe errors are reported -S-
 *
 *  -  2     >1  Errors are reported	   -E-
 *
 *  -  1     >0  Warnings are reported      -W-
 *
 *
 *  Values of  \_\_DEBUG_LEVEL\_\_  and reporting result
 *
 * -   2    All error messages are reported
 * -   1    Only severe error and error messages are reported
 * -   0    Only severe error message are reported
 *
 *
 *@{
 *
 */
#define __WARNINGREPORT__ 0
#define __ERRORREPORT__   1
#define __SEVEREREPORT__  2

/**
 * @brief The Severity_type enum Severity of an exception object
 */
enum Severity_type : uint8_t {
    Severity_Nothing    = 0 ,  //< Nothing to report
    Severity_Information= 1,   //< Information content
    Severity_Question   = 3,
    Severity_Warning    = 2,   //< Exception concerns a warning
    Severity_Error      = 4,   //< Normal error
    Severity_Severe     = 8,   //< Severe error : this error is abnormal
    Severity_Fatal      = 0x10,//< This error is a fatal error and must stop program execution : either a system error or a programming error
    Severity_Highest    = 0xFF
};

inline ZBool zis_Error(ZStatus pStatus) {return (pStatus<0);}
inline ZBool zis_Warning(ZStatus pStatus) {return (pStatus > ZS_SUCCESS);}


#define __MESSAGE_SIZEMAX__ 1024
#define __INVALID_SIZE__ 18440000000000L

void zprintMessage (Severity_type pSeverity,const char *pModule,const char *pShortMessage, char * pFile, int pLine , char *pMsg);
void zprintSystemMessage (Severity_type pSeverity,
                          int perrno,
                    const char * pModule,
                    const char * pShortMessage,
                    char * pFile,
                    int pLine ,
                    char *pMsg);


const char * decode_Severity (Severity_type pSeverity);
const char * decode_ShortSeverity (Severity_type pSeverity);

//! @} ZBSErrorCategoriesGroup
//! @} ZBSErrorGroup

/** @addtogroup StandardsGroup notation standards
 * @{
 *    . name of the macro is enclosed with '_' character. example : _MACRONAME_
 *    . macro parameter name is enclosed with a double '_'. example : __MACROVARIABLE__
 * @}
 */

#ifndef _GET_FUNCTION_NAME_
    #ifdef __USE_WINDOWS__   //WINDOWS
        #define _GET_FUNCTION_NAME_   __FUNCTION__
    #else          //*NIX
        #define _GET_FUNCTION_NAME_   __func__
    #endif
#endif





#endif // ZERROR_MIN_H

