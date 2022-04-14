#ifndef ZAIERRORS_CPP
#define ZAIERRORS_CPP
#include <ztoolset/zaierrors.h>
#include <ztoolset/zexceptionmin.h>

void ZaiErrors::logZException()
{
  ZaiE_Severity wS= ZAIES_Info;
    if (ZException.count() == 0)
        push(new ZaiError(ZAIES_Info, (const utf8_t*)"No exception logged at this time"));
    else {
      wS=cvttoZAIESeverity(ZException.last().Severity);
        push(new ZaiError(wS, ZException.lastUtf8().toUtf()));
    }
    if (wS >= AutoPrint )
        _print("<%s> %s",decode_ZAIES(last()->Severity),last()->Message());
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
        _print("ZaiErrors::cleanup-W  Warning cleaning up error log while error messages are logged.");
        wOnce=false;
      }
    delete popR();
  }
}
int ZaiErrors::countType(uint8_t pC) {
  int wRet=0;
  for (long wi=0;wi < count();wi++) {
    if (Tab[wi]->Severity & pC)
      wRet++;
  }
  return wRet;
}

void ZaiErrors::setOutput(const char* pOutfile)
{
  Output=fopen(pOutfile,"w");
  if (Output==nullptr)
  {
    Output=stdout;
    fprintf(Output," cannot open file <%s> redirected to stdout\n",pOutfile);
  }
}


utf8VaryingString
ZaiErrors::allLoggedToString()
{
  utf8VaryingString wReturn;

  int wErrors=0;
  int wFatals=0;
  int wInfos=0;
  int wWarns=0;
  for (long wi=0;wi<count();wi++)
  {
    if (Tab[wi]->Severity==ZAIES_Text)
      wReturn.addsprintf("%s\n",Tab[wi]->Message());
    else
      wReturn.addsprintf("<%s> %s\n",decode_ZAIES(Tab[wi]->Severity),Tab[wi]->Message());
    switch (Tab[wi]->Severity)
    {
    case ZAIES_Text:
      break;
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
    wReturn += "No error, info or warning has been logged.\n";
  else
    wReturn.addsprintf("<%d> info(s) <%d> warning(s) <%d> error(s) <%d> fatal errors have been logged.\n",
                        wInfos,wWarns, wErrors,wFatals);
  return wReturn;
}//allLoggedToString


utf8VaryingString ZaiErrors::errorsToString()
{
  utf8VaryingString wReturn;

  int hasOne=0;
  for (long wi=0;wi<count();wi++)
  {
    if (Tab[wi]->Severity > ZAIES_Error)
    {
      wReturn.sprintf("<%s> %s \n",decode_ZAIES(Tab[wi]->Severity),Tab[wi]->Message());
      hasOne++;
    }
  }
  if (hasOne==0)
    wReturn += "No error has been logged.\n";
  else
    wReturn.sprintf("<%d> error(s) have been logged.\n",hasOne);
  return wReturn;
}//errorsToString

/**
   * @brief displayAllLogged prints to pOutput (defaulted to stdout) errors messages from least recent to most recent
   * @param pOutput
   */
void ZaiErrors::displayAllLogged(FILE* pOutput )
{
/*  if (pOutput)
    Output = pOutput;
  else
    Output = stdout;
*/
  if (Context.count() > 0)
    _print("-----------<%s> All Messages Report --------------------------- ",Context.last().toCChar());
  else
    _print("-----------<%s> All Messages Report --------------------------- ","No context set");
  int wErrors=0;
  int wFatals=0;
  int wInfos=0;
  int wWarns=0;
  for (long wi=0;wi<count();wi++)
  {
    if (Tab[wi]->Severity==ZAIES_Text)
      _print("%s",Tab[wi]->Message());
    else
      _print("<%s> %s",decode_ZAIES(Tab[wi]->Severity),Tab[wi]->Message());
    switch (Tab[wi]->Severity)
    {
    case ZAIES_Text:
      break;
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
    _print("No error, info or warning has been logged.");
  else
    _print("<%d> info(s) <%d> warning(s) <%d> error(s) <%d> fatal errors have been logged.",
        wInfos,wWarns, wErrors,wFatals);
}//displayAllLogged

Severity_type ZaiErrors::getSeverity()
{
  Severity_type wS= Severity_Nothing;
  uint8_t wZAIES = ZAIES_Text;
  for (long wi=0;wi<count();wi++)
    {
    if (Tab[wi]->Severity > wZAIES)
      wZAIES = Tab[wi]->Severity ;
    }
    switch (wZAIES)
    {
    case ZAIES_Text:
      return Severity_Nothing;
    case ZAIES_Info:
      return Severity_Information;
    case ZAIES_Warning:
      return Severity_Warning;
    case ZAIES_Error:
      return Severity_Error;
    case ZAIES_Fatal:
      return Severity_Fatal;
    }// switch
}



void ZaiErrors::displayErrors(FILE* pOutput)
{
  if (Context.count() > 0)
    _print("-----------<%s> Error Messages Report  --------------------------- \n",Context.last().toCChar());
  else
    _print("-----------<%s> Error Messages Report  --------------------------- \n","No context set");
  int hasOne=0;
  for (long wi=0;wi<count();wi++)
  {
    if (Tab[wi]->Severity > ZAIES_Info)
    {
      _print("<%s> %s \n",decode_ZAIES(Tab[wi]->Severity),Tab[wi]->Message());
      hasOne++;
    }
  }
  if (hasOne==0)
    _print("No error has been logged.\n");
  else
    _print("<%d> error(s) have been logged.\n",hasOne);
}//displayErrors



utf8String ZaiErrors::getLastError( )
{
  utf8String wErrMsg;
  const char* wContext="no context set";
  if (Context.count()>0)
    wErrMsg.sprintf("-----------<%s> Last Error Message Report --------------------------- \n",Context.last().toCChar());
  else
    wErrMsg.sprintf("-----------<%s> Last Error Message Report --------------------------- \n",wContext);
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
  if (pSeverity >= AutoPrint)
    _print("<%s> %s ",decode_ZAIES(last()->Severity),last()->Message());

  va_end(args);
}
void ZaiErrors::logZStatus(ZaiE_Severity pSeverity,ZStatus pSt,const char* pFormat,...)
{
  ErrorLevel = pSeverity;
  va_list args;
  va_start (args, pFormat);
  push(new ZaiError(pSeverity,pSt,pFormat,args));
  if (pSeverity >= AutoPrint)
    _print("<%s> <%s> %s",decode_ZAIES(last()->Severity),decode_ZStatus(last()->Status),last()->Message());

  va_end(args);
}

void ZaiErrors::_print(const char* pFormat,...)
{
  utf8VaryingString wOut;
  va_list ap;
  va_start(ap, pFormat);
  wOut.vsnprintf(500,pFormat,ap);
  va_end(ap);
  if (_displayCallback==nullptr) {
    if (Output==nullptr) {
      fprintf(stdout,wOut.toCChar());
      fprintf(stdout,"\n");
      std::cout.flush();
      return;
    }
    fprintf(Output,wOut.toCChar());
    fprintf(Output,"\n");
    fflush(Output);
    return;
  }
  _displayCallback(wOut);
}

void ZaiErrors::_print(const utf8VaryingString& pOut)
{
  if (_displayCallback==nullptr) {
    if (Output==nullptr) {
      fprintf(stdout,pOut.toCChar());
      fprintf(stdout,"\n");
      std::cout.flush();
      return;
    }
    fprintf(Output,pOut.toCChar());
    fprintf(Output,"\n");
    fflush(Output);
    return;
  }
  _displayCallback(pOut);
}

void ZaiErrors::errorLog(const char* pFormat,...)
{
  ErrorLevel |= ZAIES_Error;
  va_list args;
  va_start (args, pFormat);
  push(new ZaiError(ZAIES_Error,pFormat,args));
  if (ZAIES_Error >= AutoPrint)
    _print("<%s> %s ",decode_ZAIES(last()->Severity),last()->Message());

  va_end(args);
}
void ZaiErrors::textLog(const char* pFormat,...)
{
//  ErrorLevel = ZAIES_Text;
  va_list args;
  va_start (args, pFormat);
  push(new ZaiError(ZAIES_Text,pFormat,args));

  if (ZAIES_Text >= AutoPrint)
    _print("%s",last()->Message());

  va_end(args);
}
void ZaiErrors::infoLog(const char* pFormat,...)
{
  ErrorLevel |= ZAIES_Info;
  va_list args;
  va_start (args, pFormat);
  push(new ZaiError(ZAIES_Info,pFormat,args));

  if (ZAIES_Info >= AutoPrint)
    _print("<%s> %s ",decode_ZAIES(last()->Severity),last()->Message());

  va_end(args);
}
void ZaiErrors::warningLog(const char* pFormat,...)
{
  ErrorLevel |= ZAIES_Warning;
  va_list args;
  va_start (args, pFormat);
  push(new ZaiError(ZAIES_Warning,pFormat,args));

  if (ZAIES_Warning >= AutoPrint)
    _print("<%s> %s ",decode_ZAIES(last()->Severity),last()->Message());

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
}//decode_ZAIES

utf8VaryingString decode_ZAIE_ErrorLevel(uint8_t pErrorLevel)
{
  utf8VaryingString wRet;

  if (pErrorLevel & ZAIES_Info)
    wRet += "ZAIES_Info";

  if (pErrorLevel & ZAIES_Warning)
    wRet.addConditionalOR((const utf8_t*)"ZAIES_Warning");

  if (pErrorLevel & ZAIES_Error)
    wRet.addConditionalOR((const utf8_t*)"ZAIES_Error");

  if (pErrorLevel & ZAIES_Fatal)
    wRet.addConditionalOR((const utf8_t*)"ZAIES_Fatal");

  return wRet;
}//decode_ZAIES

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
