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


void ZaiErrors::cleanupLogged()
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


/**
   * @brief displayAll prints to pOutput (defaulted to stdout) errors messages from least recent to most recent
   * @param pOutput
   */
void ZaiErrors::displayAllLogged(FILE* pOutput )
{
  fprintf(pOutput,"-----------<%s> All Messages Report --------------------------- \n",Context.c_str());
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
void ZaiErrors::displayErrors(FILE* pOutput)
{
  fprintf(pOutput,"-----------<%s> Error Messages Report --------------------------- \n",Context.c_str());
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
utf8String ZaiErrors::getLastError( )
{
  utf8String wErrMsg;
  wErrMsg.sprintf("-----------<%s> Las Error Message Report --------------------------- \n",Context.c_str());
  int hasOne=0;
  long wi=lastIdx();
  while (wi>-1)
    if (Tab[wi]->Severity > ZAIES_Info)
    {
      wErrMsg.sprintf("<%s> %s \n",decode_ZAIES(Tab[wi]->Severity),Tab[wi]->Message());
      hasOne++;
      break;
    }

  if (hasOne==0)
    wErrMsg.sprintf("No error has been logged.\n");

  return wErrMsg;
}
void ZaiErrors::log(ZaiE_Severity pSeverity,const char* pFormat,...)
{
  ErrorLevel = pSeverity;
  va_list args;
  va_start (args, pFormat);
  push(new ZaiError(pSeverity,pFormat,args));
  if (AutoPrint)
    fprintf(stderr,"<%s> %s \n",decode_ZAIES(last()->Severity),last()->Message());

  va_end(args);
}
void ZaiErrors::logZStatus(ZaiE_Severity pSeverity,ZStatus pSt,const char* pFormat,...)
{
  ErrorLevel = pSeverity;
  va_list args;
  va_start (args, pFormat);
  push(new ZaiError(pSeverity,pSt,pFormat,args));
  if (AutoPrint)
    fprintf(stderr,"<%s> <%s> %s \n",decode_ZAIES(last()->Severity),decode_ZStatus(last()->Status),last()->Message());

  va_end(args);
}


void ZaiErrors::errorLog(const char* pFormat,...)
{
  ErrorLevel = ZAIES_Error;
  va_list args;
  va_start (args, pFormat);
  push(new ZaiError(ZAIES_Error,pFormat,args));

  if (AutoPrint)
    fprintf(stderr,"<%s> %s \n",decode_ZAIES(last()->Severity),last()->Message());

  va_end(args);
}
void ZaiErrors::infoLog(const char* pFormat,...)
{
  ErrorLevel = ZAIES_Info;
  va_list args;
  va_start (args, pFormat);
  push(new ZaiError(ZAIES_Info,pFormat,args));

  if (AutoPrint)
    fprintf(stderr,"<%s> %s \n",decode_ZAIES(last()->Severity),last()->Message());

  va_end(args);
}
void ZaiErrors::warningLog(const char* pFormat,...)
{
  ErrorLevel = ZAIES_Warning;
  va_list args;
  va_start (args, pFormat);
  push(new ZaiError(ZAIES_Warning,pFormat,args));

  if (AutoPrint)
    fprintf(stderr,"<%s> %s \n",decode_ZAIES(last()->Severity),last()->Message());

  va_end(args);
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
