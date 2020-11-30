#ifndef ZAIERRORS_H
#define ZAIERRORS_H

#include <ztoolset/zarray.h>
#include <ztoolset/zutfstrings.h>

enum ZaiE_Severity{
    ZAIES_Info      =   0,
    ZAIES_Warning   =   1,
    ZAIES_Error     =   2,
    ZAIES_Fatal     =   4
};

const char* decode_ZAIES(ZaiE_Severity pSeverity);

ZaiE_Severity cvttoZAIESeverity(Severity_type pZEXSev);
Severity_type cvttoZEXSeverity(ZaiE_Severity pZEXSev);

struct ZaiError {
    ZaiError()=default;
    ZaiError(const char* pFormat,...)
    {
        char wMsg[500];
        va_list args;
        va_start (args, pFormat);
        vsnprintf (wMsg,500 ,pFormat, args);
        Severity=ZAIES_Error;
        _Message=wMsg;
        va_end(args);
    }
    /**
     * @brief ZaiError  ZStatus is implicitely defined according pSeverity
     */
    ZaiError(ZaiE_Severity pSeverity,const char*pFormat,va_list pArglist)
    {
        char wMsg[500];
        vsnprintf (wMsg,500 ,pFormat, pArglist);
        _Message=wMsg;
        Severity=pSeverity;

        switch (pSeverity) {
        case ZAIES_Info:
        case ZAIES_Warning:
            Status = ZS_NOTHING;
            break;
        case ZAIES_Error:
            Status = ZS_ERROR;
            break;
        case ZAIES_Fatal:
            Status = ZS_ERROR;
            break;
        }
    }
    ZaiError(ZaiE_Severity pSeverity, ZStatus pSt, const char *pFormat, va_list pArglist)
    {
        char wMsg[500];
        vsnprintf(wMsg, 500, pFormat, pArglist);
        Status=pSt;
        _Message=wMsg;
        Severity=pSeverity;
    }

    ZaiError(ZaiE_Severity pSeverity, const utf8_t *pMessage)
    {
        _Message=pMessage;
        Severity=pSeverity;
    }
/*    ZaiError(ZaiE_Severity pSeverity, const char *pMessage)
    {
        _Message=pMessage;
        Severity=pSeverity;
    }*/
    const char* Message() {return _Message.toCChar();}

    ZStatus getZStatus() { return Status; }

    ZaiE_Severity   Severity;
    ZStatus         Status=ZS_NOTHING;
    utf8String     _Message ;
};

/**
 * @brief The ZaiErrors class holds and manages journal for main objects.
 *
 *  Objects that have journal logging are ZGLWindow and ZGLResource
 */
class ZaiErrors : public zbs::ZArray<ZaiError*>
{
public:
    ZaiErrors()=default;
    ZaiErrors(const char* pContext) {Context=pContext;}

    ~ZaiErrors()
    {
        while (count())
            delete popR();
    }

    ZaiErrors(ZaiErrors& pIn)
    {
        cleanupLogged();
        add(pIn);
    }

    /*   void setContext(const char* pContext)
    {
        Context = pContext;
    }*/
    void setErrorLogContext(const char* pContext,...)
    {
        va_list args;
        va_start (args, pContext);
        char wContext[200];
        memset(wContext,0,sizeof(wContext));
        vsnprintf(wContext,199,pContext,args);
        Context = wContext;
        va_end(args);
    }
    void add (ZaiErrors& pIn)
    {
        for (long wi=0;wi < pIn.count();wi++)
        {
            push(pIn[wi]);
        }
    }

    void clear() { cleanupLogged();}
    void cleanupLogged();
    ZStatus getLastZStatus() { return last()->getZStatus(); }


    /**
   * @brief displayAll prints to pOutput (defaulted to stdout) errors messages from least recent to most recent
   * @param pOutput
   */
    void displayAllLogged(FILE* pOutput=stdout );
    void displayErrors(FILE* pOutput=stdout );
    utf8String getLastError( );
    void log(ZaiE_Severity pSeverity,const char* pFormat,...);
    void logZStatus(ZaiE_Severity pSeverity,ZStatus pSt,const char* pFormat,...);

    void logZException();


    void errorLog(const char* pFormat,...);
    void infoLog(const char* pFormat,...);
    void warningLog(const char* pFormat,...);
    bool hasError()
    {
        return ErrorLevel > ZAIES_Warning ;
        /*      if (count()==0)
          return false;
      for (long wi=0;wi<count();wi++)
            if (Tab[wi]->Severity > ZAIES_Info)
                    return true;
      return false;*/
    }
    bool hasFatal()
    {
        return ErrorLevel > ZAIES_Warning ;
        /*      if (count()==0)
          return false;
      for (long wi=0;wi<count();wi++)
            if (Tab[wi]->Severity > ZAIES_Info)
                    return true;
      return false;*/
    }
    bool hasWarning()
    {
        return ErrorLevel & ZAIES_Warning ;
    }
    bool hasMessages()
    {
        return count()!=0 ;
    }
    void setAutoPrintOn(bool pOnOff) {AutoPrint=pOnOff;}

    bool AutoPrint=false; /* if set then prints info and warning messages to stdout and error messages to stderr as soon as registrated */
    std::string Context;
    int ErrorLevel=0;
};

#endif // ZAIERRORS_H
