#ifndef ZXMLATTRIBUTE_CPP
#define ZXMLATTRIBUTE_CPP
#include <zxml/zxmldoc.h>
#include <zxml/zxmlattribute.h>
#include <zxml/zxmlnamespace.h>

#include <zxml/zxmlerror.h>

//===============Attribute Node=======================================

zxmlAttribute::zxmlAttribute(zxmlNode& pNode,const char*pName,const char* pValue,zxmlNameSpace* pNameSpace)
{
_MODULEINIT_

    xmlAttrPtr wAttribute=nullptr;
    if (pNameSpace)
        wAttribute=xmlNewNsProp(pNode._xmlInternalNode,pNameSpace->_xmlInternalNameSpace,(const xmlChar *) pName,(const xmlChar *)pValue);
    else
        wAttribute=xmlNewProp(pNode._xmlInternalNode,(const xmlChar *) pName,(const xmlChar *)pValue);
  if (!wAttribute)
        {
      setXMLZException(_GET_FUNCTION_NAME_,ZS_XMLERROR, Severity_Severe,"cannot create attribute : xml error");
      ZException.exit_abort();
        }
  _xmlInternalAttribute =wAttribute;
  _RETURN_;
}



utffieldNameString
zxmlAttribute::getName(void)
{

    return utffieldNameString ( (const utf8_t*)_xmlInternalAttribute->name);
}

utfdescString
zxmlAttribute::getValue(void)
{
    return utfdescString((const utf8_t*)xmlNodeGetContent(_xmlInternalNode));
}

ZStatus
zxmlAttribute::getNextAttributeSibling(zxmlAttribute *&pAttribute)
{
    xmlAttrPtr wAttr=nullptr;
    if (_xmlInternalAttribute->next==nullptr)
                                return (ZS_EOF);
    wAttr=_xmlInternalAttribute->next;
    while (wAttr&&(wAttr->type!=XML_ATTRIBUTE_NODE))
        {
        wAttr=wAttr->next;
        }
    if (!wAttr)
            {
            pAttribute=nullptr;
            return ZS_EOF;
            }
    if (wAttr->type!=XML_ATTRIBUTE_NODE)
                {
                pAttribute=nullptr;
                return ZS_EOF;
                }
//    pAttribute=new zxmlAttribute((xmlAttrPtr)wNode);
//    pAttribute=zxmlsearchAttributeByInternal((xmlAttrPtr)wAttr);
//    if (!pAttribute)
    pAttribute=zxmlcreateAttribute((xmlAttrPtr)wAttr);
    return ZS_SUCCESS;
}// getNextAttributeSibling

#endif// ZXMLATTRIBUTE_CPP
