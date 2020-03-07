#ifndef TEMPLATEWSTRING_CPP
#define TEMPLATEWSTRING_CPP

    const char* GLocale="en_US.utf8";

#include <ztoolset/templatewstring.h>

#include <wchar.h>

ZStatus
CStringtoWString (wchar_t* pOutWString,const size_t pOutArrayCount,const char* pInCString,const size_t pInArrayCount)
{
ZStatus wSt=ZS_SUCCESS;
mbstate_t wBuf;
ssize_t wRSize=pInArrayCount;
    memset (&wBuf,0,sizeof(wBuf));

    if (pInArrayCount>pOutArrayCount)
        {
        fprintf(stderr,"%s>> Warning : string is been truncated to %ld characters while %ld size requested\n",
                _GET_FUNCTION_NAME_,
                pOutArrayCount-1,
                pInArrayCount-1);

        wRSize= pOutArrayCount-1;
        wSt=ZS_FIELDCAPAOVFLW;
        }
    if (pInArrayCount<pOutArrayCount)
        {
        wRSize= pInArrayCount;
        }

    memset(pOutWString,0,pOutArrayCount*sizeof(pOutWString[0]));
    int wSize=mbsrtowcs(pOutWString,&pInCString,wRSize,&wBuf);
    if (wSize!=wRSize)
    {
     fprintf(stderr,"%s>> Error : a problem occurred while converting CString to WString. Max char converted is %d characters while %ld size requested\n",
             _GET_FUNCTION_NAME_,
             wSize,
             wRSize);
    }
    pOutWString[wRSize]=L'\0';
    return wSt;
}// CStringtoWString

ZStatus
WStringtoCString (char* pOutCString,const size_t pOutArrayCount,const wchar_t* pInCString,const size_t pInArrayCount)
{
ZStatus wSt=ZS_SUCCESS;
mbstate_t wBuf;
size_t wRSize=pInArrayCount;
    memset (&wBuf,0,sizeof(wBuf));

    if (pInArrayCount>pOutArrayCount)
        {
        fprintf(stderr,"%s>> Warning : string is been truncated to %ld characters while %ld size requested\n",
                _GET_FUNCTION_NAME_,
                pOutArrayCount-1,
                pInArrayCount-1);

        wRSize= pOutArrayCount-1;
        wSt=ZS_FIELDCAPAOVFLW;
        }
    memset(pOutCString,0,pOutArrayCount*sizeof(pOutCString[0]));
    int wSize=wcsrtombs(pOutCString,&pInCString,wRSize,&wBuf);
    if (wSize!=wRSize)
    {
     fprintf(stderr,"%s>> Error : a problem occurred while converting WString(wchar_t*) to CString (char*). Max char converted is %ld characters while %ld size requested\n",
             _GET_FUNCTION_NAME_,
             wSize,
             wRSize);
    }
    pOutCString[wRSize]='\0';
    return wSt;;
}// WStringtoCString
#endif // TEMPLATEWSTRING_CPP
