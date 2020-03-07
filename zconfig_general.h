#ifndef ZCONFIG_GENERAL_H
#define ZCONFIG_GENERAL_H

/*
 *  all utf-x litterals encoding
 *
 *
 */

#define UTF8_L  u8
#define UTF16_L u
#define UTF32_L U


#ifdef  __linux__
    #define __USE_LINUX__
#define APIEXPORT
#define APIIMPORT
#define APICEXPORT extern "C"
#define APICIMPORT extern "C"

#define CFUNCTOR

#elif  defined(_WIN32)
    #define __USE_WINDOWS__
#define APIEXPORT   __declspec(DllExport)
#define APIIMPORT   __declspec(DllImport)
#define APICEXPORT extern "C" __declspec(DllExport)
#define APICIMPORT extern "C" __declspec(DllImport)

#define CFUNCTOR __cdecl

#elif defined(__APPLE__)
#define APIEXPORT
#define APIIMPORT
#define APICEXPORT extern "C"
#define APICIMPORT extern "C"

#define CFUNCTOR

    #include <TargetConditionals.h>
    #ifdef TARGET_OS_IPHONE
        #define __USE_IOS__
    #else
        #define __USE_OSX__
    #endif // __APPLE__
#endif


#ifdef __ANDROID__
# include <android/api-level.h>
    #define __USE_ANDROID__
#endif

/**     compiler must support C+11
  */
#ifdef __USE_WINDOWS__
#if _MSC_VER < 1911
    #error invalid mscv version. must be at least 19.11.25506
#endif // _MSC_VER
#else
#if __cplusplus < 201103L
    #error Compiler must support C+11 compliancy. May be have you omitted to mention -std=c++11 or -std=gnu+11 as compiler flag
#endif // __cplusplus

#endif//__USE_WINDOWS__


/*  =========== PORTABILITY EQUIVALENCE ==================  */

#include <stdint.h>


#ifdef __USE_WINDOWS__
#include <windows.h>
#include <process.h>

typedef  int            ssize_t ;
typedef  uint32_t       pid_t;              // include <process.h>
typedef  uint32_t       uid_t;              // getuid geteuid emulation
typedef  uint32_t       gid_t;              // getuid geteuid emulation

#define getpid      _getpid

// for getuid and geteuid, please include <ztoolset/zfunctions.h>
// for getpwuid include <ztoolset/zfunctions.h>
struct passwd
{
  char *pw_name;		/* Username.  */
  char *pw_passwd;		/* Password.  */
  uid_t pw_uid;         /* User ID.  */
  gid_t pw_gid;         /* Group ID.  */
  char *pw_gecos;		/* Real name.  */
  char *pw_dir;			/* Home directory.  */
  char *pw_shell;		/* Shell program.  */
};



#define strcasecmp _stricmp
#define strncasecmp _strnicmp

// for the following, do not forget to #include <shlwap.h> as it is not provided in standard library for windows
char * strcasestr(const char* pString,const char *pSubString);

#define __END_DIRECTORY_SIGN__ '\\'
#define Delimiter '\\'
#define __EOL__  "\r\n"

#else
#define __EOL__  "\n"
#define __END_DIRECTORY_SIGN__  '/'
#define Delimiter '/'
#endif





#endif // ZCONFIG_GENERAL_H

