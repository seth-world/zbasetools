#ifndef ZAIERRORS_CPP
#define ZAIERRORS_CPP
#include <ztoolset/zaierrors.h>
#include <ztoolset/zexceptionmin.h>

void ZaiErrors::logZException()
{
    if (ZException.count() == 0)
        push(new ZaiError(ZAIES_Info, (const utf8_t*)"No exception logged at this time"));
    else
        push(new ZaiError(cvttoZAIESeverity(ZException.last().Severity), ZException.lastUtf8().toUtf()));

    if (AutoPrint)
        fprintf(stderr,"<%s> %s\n",decode_ZAIES(last()->Severity),last()->Message());
}


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


ZaiE_Severity cvttoZAIESeverity(Severity_type pZEXSev)
{
    switch (pZEXSev) {
    case Severity_Nothing:
    case Severity_Information:
        return ZAIES_Info;
    case Severity_Warning:
        return ZAIES_Warning;
    case Severity_Error:
    case Severity_Severe:
        return ZAIES_Error;
    case Severity_Fatal:
    case Severity_Highest:
        return ZAIES_Fatal;
    }
}
Severity_type cvttoZEXSeverity( ZaiE_Severity pZEXSev)
{
    switch (pZEXSev) {
    case ZAIES_Info:
        return Severity_Information;
    case ZAIES_Warning:
        return Severity_Warning;
    case ZAIES_Error:
        return Severity_Error;
    case ZAIES_Fatal:
        return Severity_Fatal;
    }
}


#endif //ZAIERRORS_CPP
