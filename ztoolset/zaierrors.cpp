#ifndef ZAIERRORS_CPP
#define ZAIERRORS_CPP
#include <ztoolset/zaierrors.h>



const char* decode_ZAIES(ZaiE_Severity pSeverity)
{
    switch (pSeverity)
    {
    case ZAIES_Info:
        return "ZAIES_Info";
    case ZAIES_Warning:
        return "ZAIES_Warning";
    case ZAIES_Error:
        return "ZAIES_Error";
    case ZAIES_Fatal:
        return "ZAIES_Fatal";
    default:
        return "Unknown severity";

    }
}


#endif //ZAIERRORS_CPP
