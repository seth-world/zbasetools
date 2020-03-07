#ifndef ZXMLNAMESPACE_CPP
#define ZXMLNAMESPACE_CPP

#include <zxml/zxmlnamespace.h>
#include <zxml/zxmlerror.h>
#include <zxml/zxmldoc.h>
#include <zxml/zxmlnode.h>


zxmlNameSpace::zxmlNameSpace(zxmlDoc* pDoc,const char* pURL,const char* pPrefix)
{
_MODULEINIT_

    if (!pDoc)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_INVVALUE,
                          Severity_Fatal,
                          " Cannot create global namespace : document pointer must not have null value ");
    ZException.exit_abort();
    }
    _xmlInternalNameSpace=xmlNewGlobalNs(pDoc->_xmlInternalDoc,(const xmlChar *)pURL,(const xmlChar *)pPrefix);
    if (!_xmlInternalNameSpace)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Fatal,
                         "Cannot create global XML namespace for document <%s> url <%s> prefix <%s>",
                         (const char*)pDoc->_xmlInternalDoc->name,
                         pURL,
                         pPrefix);


        ZException.exit_abort();
        }
    _RETURN_;
}// zxmlNameSpace
zxmlNameSpace::zxmlNameSpace(zxmlNode* pNode,const char* pURL,const char* pPrefix)
{
_MODULEINIT_
    if (!pNode)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_INVVALUE,
                          Severity_Fatal,
                          " Cannot create local namespace : element/node pointer must not have null value ");
    ZException.exit_abort();
    }
    _xmlInternalNameSpace=xmlNewNs(pNode->_xmlInternalNode,(const xmlChar *)pURL,(const xmlChar *)pPrefix);
    if (!_xmlInternalNameSpace)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Fatal,
                         "Cannot create local XML namespace");


        ZException.exit_abort();
        }
    _RETURN_;
}// zxmlNameSpace
#endif // ZXMLNAMESPACE_CPP
