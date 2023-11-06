#ifndef ZXMLERROR_H
#define ZXMLERROR_H

#include <libxml/parser.h>
#include <libxml/xmlerror.h>
#include <ztoolset/zexceptionmin.h>
#include <ztoolset/utfvaryingstring.h>

const char* decodeXMLErrorSymbol (int pXMLErrNum);
const char* decodeXMLDomainSymbol (int pXMLDomainNum);
const char* decodeXMLDomainDesc (int pXMLDomainNum);
//class exceptionString;
utf8VaryingString getXMLLastError(void);

void setXMLZException (const char*pModule,
                       ZStatus pStatus,
                       Severity_type pSeverity,
                       const char *pFormat,...);

class zxmlLineCol {
public:
    zxmlLineCol() {}
    zxmlLineCol(const zxmlLineCol& pIn) {_copyFrom(pIn);}

    zxmlLineCol& _copyFrom(const zxmlLineCol& pIn) {
        line = pIn.line;
        col = pIn.col;
        return *this;
    }
    zxmlLineCol set(xmlErrorPtr pErr) {
        line=pErr->line;
        col=pErr->int2;
        return *this;
    }
    int line=0;
    int col=0;
};

class zxmlError : _xmlError
{
public:
    zxmlError() {memset (this,0,sizeof(_xmlError));}
    ZStatus get() ;
    static utf8VaryingString getMessage();
    static zxmlLineCol getLineCol() ;
    static utf8VaryingString getFullMessage();

    /* using pXmlCode that must contain the xml code that has been parsed, returns the xml code line that is errored */
    static utf8VaryingString getErroredXmlCodeLine(const utf8VaryingString& pXmlCode);

};


#endif // ZXMLERROR_H
