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
    void cleanupLogged()
    {
        ErrorLevel = 0;
        bool wOnce=true;
        while(count())
        {
            if (last()->Severity > ZAIES_Warning)
                if (wOnce)
                {
                    fprintf(stdout,"ZaiErrors::cleanup-W  Warning cleaning up error log while error messages are logged.\n");
                    wOnce=false;
                }
            delete popR();
        }
    }
    ZStatus getLastZStatus() { return last()->getZStatus(); }


    /**
   * @brief displayAll prints to pOutput (defaulted to stdout) errors messages from least recent to most recent
   * @param pOutput
   */
    void displayAllLogged(FILE* pOutput=stdout )
    {
        fprintf(pOutput,"-----------<%s> Messages Report --------------------------- \n",Context.c_str());
        int wErrors=0;
        int wFatals=0;
        int wInfos=0;
        int wWarns=0;
        for (long wi=0;wi<count();wi++)
        {
            fprintf(pOutput,"<%s> %s\n",decode_ZAIES(Tab[wi]->Severity),Tab[wi]->Message());
            switch (Tab[wi]->Severity)
            {
            case ZAIES_Info:
                wInfos++;
                break;
            case ZAIES_Warning:
                wWarns++;
                break;
            case ZAIES_Error:
                wErrors++;
                break;
            case ZAIES_Fatal:
                wFatals++;
                break;
            }// switch
        } // for
        if ((wErrors==0) && (wWarns==0) && (wFatals==0))
            fprintf(pOutput,"No error, info or warning has been logged.\n");
        else
            fprintf(pOutput,
                    "<%d> info(s) <%d> warning(s) <%d> error(s) <%d> fatal errors have been logged.\n",
                    wInfos,wWarns, wErrors,wFatals);
    }
    void displayErrors(FILE* pOutput=stdout )
    {
        fprintf(pOutput,"-----------ZModelImport Error Report --------------------------- \n");
        int hasOne=0;
        for (long wi=0;wi<count();wi++)
        {
            if (Tab[wi]->Severity > ZAIES_Info)
            {
                fprintf(pOutput,"<%s> %s \n",decode_ZAIES(Tab[wi]->Severity),Tab[wi]->Message());
                hasOne++;
            }
        }
        if (hasOne==0)
            fprintf(pOutput,"No error has been logged.\n");
        else
            fprintf(pOutput,"<%d> error(s) have been logged.\n",hasOne);
    }
    void log(ZaiE_Severity pSeverity,const char* pFormat,...)
    {
        ErrorLevel = pSeverity;
        va_list args;
        va_start (args, pFormat);
        push(new ZaiError(pSeverity,pFormat,args));
        if (AutoPrint)
            fprintf(stderr,"<%s> %s \n",decode_ZAIES(last()->Severity),last()->Message());

        va_end(args);
    }
    void logZStatus(ZaiE_Severity pSeverity,ZStatus pSt,const char* pFormat,...)
    {
        ErrorLevel = pSeverity;
        va_list args;
        va_start (args, pFormat);
        push(new ZaiError(pSeverity,pSt,pFormat,args));
        if (AutoPrint)
            fprintf(stderr,"<%s> <%s> %s \n",decode_ZAIES(last()->Severity),decode_ZStatus(last()->Status),last()->Message());

        va_end(args);
    }

    void logZException();


    void errorLog(const char* pFormat,...)
    {
        ErrorLevel = ZAIES_Error;
        va_list args;
        va_start (args, pFormat);
        push(new ZaiError(ZAIES_Error,pFormat,args));

        if (AutoPrint)
            fprintf(stderr,"<%s> %s \n",decode_ZAIES(last()->Severity),last()->Message());

        va_end(args);
    }
    void infoLog(const char* pFormat,...)
    {
        ErrorLevel = ZAIES_Info;
        va_list args;
        va_start (args, pFormat);
        push(new ZaiError(ZAIES_Info,pFormat,args));

        if (AutoPrint)
            fprintf(stderr,"<%s> %s \n",decode_ZAIES(last()->Severity),last()->Message());

        va_end(args);
    }
    void warningLog(const char* pFormat,...)
    {
        ErrorLevel = ZAIES_Warning;
        va_list args;
        va_start (args, pFormat);
        push(new ZaiError(ZAIES_Warning,pFormat,args));

        if (AutoPrint)
            fprintf(stderr,"<%s> %s \n",decode_ZAIES(last()->Severity),last()->Message());

        va_end(args);
    }
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
