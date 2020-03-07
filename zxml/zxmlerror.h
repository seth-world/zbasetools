#ifndef ZXMLERROR_H
#define ZXMLERROR_H

#include <libxml/parser.h>
#include <libxml/xmlerror.h>
#include <ztoolset/zexceptionmin.h>

const char* decodeXMLErrorSymbol (int pXMLErrNum);
const char* decodeXMLDomainSymbol (int pXMLDomainNum);
const char* decodeXMLDomainDesc (int pXMLDomainNum);
//class exceptionString;
utfexceptionString getXMLLastError(void);

void setXMLZException (const char*pModule,
                       ZStatus pStatus,
                       Severity_type pSeverity,
                       const char *pFormat,...);

#endif // ZXMLERROR_H
