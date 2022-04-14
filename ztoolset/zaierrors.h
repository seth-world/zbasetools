#ifndef ZAIERRORS_H
#define ZAIERRORS_H

#include <ztoolset/zarray.h>
#include <ztoolset/utfvstrings.h>
#include <functional>

//#define __DISPLAYCALLBACK__(__NAME__)  void (*__NAME__) (const utf8VaryingString& pOut)
#define __DISPLAYCALLBACK__(__NAME__)  std::function<void (const utf8VaryingString&)> __NAME__

enum ZaiE_Severity : uint8_t
{
    ZAIES_Text      =   0x01,
    ZAIES_Info      =   0x02,
    ZAIES_Warning   =   0x04,
    ZAIES_Error     =   0x08,
    ZAIES_Fatal     =   0x10,
    ZAIES_None      =   0x00,
    ZAIES_All       =   0xFF
};

const char* decode_ZAIES(ZaiE_Severity pSeverity);

ZaiE_Severity cvttoZAIESeverity(Severity_type pZEXSev);
Severity_type cvttoZEXSeverity(ZaiE_Severity pZEXSev);

class ZaiError {
public:
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
        case ZAIES_Text:
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

    ZStatus         Status=ZS_NOTHING;
    utf8String     _Message ;
    ZaiE_Severity   Severity;
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
    ZaiErrors(const char* pContext)
    {
      Context.push(pContext);
    }

    ~ZaiErrors()
    {
      if ((Output!=stderr)&&(Output!=stdout))
        fclose(Output);

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
        Context.push(wContext);

        textLog("           Context set to <%s>",Context.last().toCChar());

        va_end(args);
    }

    int countType(uint8_t pC);

    int countErrors() {return countType(ZAIES_Error | ZAIES_Fatal);}
    int countWarnings() {return countType(ZAIES_Warning);}

    void popContext()
    {
      if (Context.count()>0)
        Context.pop();
      textLog("            Restoring context to <%s>",Context.last().toCChar());
    }

    void printContext()
    {
      if (Context.count()>0)
          textLog("        Current context is <%s>",Context.last().toCChar());
    }


    void add (ZaiErrors& pIn)
    {
        for (long wi=0;wi < pIn.count();wi++)
        {
            push(pIn[wi]);
        }
    }

    void setOutput(const char* pOutfile);
    void setOutput(const utf8String& pOutfile)
    {
      setOutput(pOutfile.toCChar());
    }

    void clear() { cleanupLogged();}
    void cleanupLogged();



    ZStatus getLastZStatus() { return last()->getZStatus(); }


    Severity_type getSeverity();

    /**
   * @brief displayAll prints to pOutput (defaulted to stdout) errors messages from least recent to most recent
   * @param pOutput
   */
    void displayAllLogged(FILE* pOutput=stdout );

    utf8VaryingString allLoggedToString();
    utf8VaryingString errorsToString();


    void displayErrors(FILE* pOutput=stdout );
    utf8String getLastError( );
    void log(ZaiE_Severity pSeverity,const char* pFormat,...);
    void logZStatus(ZaiE_Severity pSeverity,ZStatus pSt,const char* pFormat,...);

    void logZException();


    void errorLog(const char* pFormat,...);
    void infoLog(const char* pFormat,...);
    void textLog(const char* pFormat,...);
    void warningLog(const char* pFormat,...);
    bool hasError()
    {
      return ErrorLevel & (ZAIES_Error|ZAIES_Fatal) ;
        /*      if (count()==0)
          return false;
      for (long wi=0;wi<count();wi++)
            if (Tab[wi]->Severity > ZAIES_Info)
                    return true;
      return false;*/
    }
    bool hasFatal()
    {
        return ErrorLevel & ZAIES_Fatal ;
    }
    bool hasWarning()
    {
        return ErrorLevel & ZAIES_Warning ;
    }
    bool hasMessages()
    {
        return ErrorLevel ;
    }

    void setAutoPrintOn(ZaiE_Severity pOnOff) {AutoPrint=pOnOff;}
    void setOutput(FILE* pOutput) {Output=pOutput;}

    void _print(const char *pFormat,...);
    void _print(const utf8VaryingString& pOut);

    void setDisplayCallback(__DISPLAYCALLBACK__(pdisplayCallback) ) {_displayCallback=pdisplayCallback;}
    __DISPLAYCALLBACK__(_displayCallback) ;

    ZaiE_Severity AutoPrint=ZAIES_None; /* if set then prints info and warning messages to stdout and error messages to stderr as soon as registrated */
    ZArray<utf8String> Context;
    uint8_t ErrorLevel=0;
    FILE* Output=stderr;
};

#endif // ZAIERRORS_H
