#ifndef ZXMLDOC_CPP
#define ZXMLDOC_CPP

#include <zxml/zxmldoc.h>
#include <ztoolset/zexceptionmin.h>
#include <zxml/zxmlerror.h>

#include <zxml/zxmlnode.h>
#include <zxml/zxmlattribute.h>
#include <zxml/zxmlnamespace.h>

//========================Generic class/functions================================

bool ZXML_Init=false;

/**
 * @brief The zxmlGlobal class global class instantiated at start and deleted when exit (not abort)
 *
 *  in case of abortion, destructor must be called apart. (see ZException_min::abort)
 *
 */
class zxmlGlobal
{
public:
    zxmlGlobal (void)
    {
        xmlInitCharEncodingHandlers();
        xmlInitParser();
        ZXML_Init=true;
        return;
    }

    ~zxmlGlobal()
    {
        while (RegisteredNodes.size())
            {
            xmlFree(RegisteredNodes.popR());
            }
        while (RegisteredAttributes.size())
            {
            xmlFree(RegisteredAttributes.popR());
            }
        while (RegisteredNameSpaces.size())
            {
             xmlFree(RegisteredNameSpaces.popR());
            }
        xmlCleanupCharEncodingHandlers();
        xmlCleanupParser();
    }

    void onAbort() {
                    if (ZXML_Init)
                            this->~zxmlGlobal();
                   }

    ZArray<zxmlNode*> RegisteredNodes;
    ZArray<zxmlAttribute*> RegisteredAttributes;
    ZArray<zxmlNameSpace*> RegisteredNameSpaces;

    bool ZXML_Init=false;
} _xmlGlobal;


void cleanupXML(void)
{
    _xmlGlobal.~zxmlGlobal();
}
/**
 * @brief createNode  zxmlNode creation
 * @param pxmlNode
 * @return
 */
zxmlNode* zxmlcreateNode(xmlNodePtr pxmlNode)
{
    zxmlNode* wNode=zxmlsearchNodeByInternal(pxmlNode);
    if (!wNode)
            {
            wNode=new zxmlNode(pxmlNode);
            _xmlGlobal.RegisteredNodes.push(wNode);
            }
    return wNode;
}
zxmlElement* zxmlcreateElement(const char* pxmlNodeName)
{

    zxmlElement* wNode=new zxmlElement(pxmlNodeName);
    _xmlGlobal.RegisteredNodes.push((zxmlNode*)wNode);
    return wNode;
}
zxmlAttribute* zxmlcreateAttribute(xmlAttrPtr pxmlNode)
{
    zxmlAttribute* wNode=zxmlsearchAttributeByInternal(pxmlNode);
    if (!wNode)
            {
            wNode=new zxmlAttribute(pxmlNode);
            _xmlGlobal.RegisteredAttributes.push(wNode);
            }
    return wNode;
}
zxmlNameSpace* zxmlcreateNameSpace(xmlNsPtr pxmlNode)
{
    zxmlNameSpace* wNode=zxmlsearchNameSpaceByInternal(pxmlNode);
    if (!wNode)
            {
            wNode=new zxmlNameSpace(pxmlNode);
            _xmlGlobal.RegisteredNameSpaces.push(wNode);
            }
    return wNode;
}
/**
 * @brief removeNode zxmlNode deletion
 * @param pNode
 */
void zxmlremoveNode(zxmlNode* pNode)
{
    for (long wi=0;wi<_xmlGlobal.RegisteredNodes.size();wi++)
                if (pNode==_xmlGlobal.RegisteredNodes[wi])
                                delete _xmlGlobal.RegisteredNodes[wi];
    return;
}
void zxmlremoveAttribute(zxmlAttribute* pNode)
{
    for (long wi=0;wi<_xmlGlobal.RegisteredAttributes.size();wi++)
                if (pNode==_xmlGlobal.RegisteredAttributes[wi])
                                delete _xmlGlobal.RegisteredAttributes[wi];
    return;
}
void zxmlremoveNameSpace(zxmlNameSpace* pNode)
{
    for (long wi=0;wi<_xmlGlobal.RegisteredNameSpaces.size();wi++)
                if (pNode==_xmlGlobal.RegisteredNameSpaces[wi])
                                delete _xmlGlobal.RegisteredNameSpaces[wi];
    return;
}
/**
 * @brief zxmlsearchNodeByInternal Searches into active Nodes pointer corresponding to pInternal (xml internal descriptor)
 * @param pInternal
 * @return pointer to active node corresponding if found, nullptr if not found
 */
zxmlNode* zxmlsearchNodeByInternal(xmlNodePtr pInternal)
{
    for (long wi=0;wi<_xmlGlobal.RegisteredNodes.size();wi++)
    {
        if (pInternal==_xmlGlobal.RegisteredNodes[wi]->_xmlInternalNode)
                                            return _xmlGlobal.RegisteredNodes[wi];
    }
    return nullptr;
}
/**
 * @brief zxmlsearchAttributeByInternalSearches into active Attributes pointer corresponding to pInternal (xml internal descriptor)
 * @param pInternal
 * @return pointer to active node corresponding if found, nullptr if not found
 */
zxmlAttribute* zxmlsearchAttributeByInternal(xmlAttrPtr pInternal)
{
    for (long wi=0;wi<_xmlGlobal.RegisteredAttributes.size();wi++)
    {
        if (_xmlGlobal.RegisteredAttributes[wi]->_xmlInternalAttribute==pInternal)
                                            return _xmlGlobal.RegisteredAttributes[wi];
    }
    return nullptr;
}
/**
 * @brief zxmlsearchNameSpaceByInternal
 * @param pInternal
 * @return pointer to active node corresponding if found, nullptr if not found
 */
zxmlNameSpace* zxmlsearchNameSpaceByInternal(xmlNsPtr pInternal)
{
    for (long wi=0;wi<_xmlGlobal.RegisteredNameSpaces.size();wi++)
    {
        if (pInternal==_xmlGlobal.RegisteredNameSpaces[wi]->_xmlInternalNameSpace)
                                            return _xmlGlobal.RegisteredNameSpaces[wi];
    }
    return nullptr;
}


//=======================End Generic=====================================


zxmlDoc::zxmlDoc(const char *pVersion)
{
_MODULEINIT_
    _xmlInternalDoc =xmlNewDoc((const xmlChar*) pVersion);
    if (_xmlInternalDoc==nullptr)
    {
      ZException.setMessage(_GET_FUNCTION_NAME_,ZS_XMLERROR,Severity_Fatal,"Fatal error creating Dom Document");
      ZException.exit_abort();
    }
    _RETURN_;
}//zxmlDoc

/*
ZStatus
zxmlDoc::uncodeToUtf8(xmlCharEncoding &pEncodingCode,ZDataBuffer &pDocOut,ZDataBuffer &pDocIn)
{
    xmlCharEncodingHandlerPtr wEncodingHandler= xmlGetCharEncodingHandler(pEncodingCode);
    if (wEncodingHandler==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVNAME,
                              Severity_Severe,
                              "Cannot find encoding handler corresponding to encoding %d",pEncodingCode);
        return ZS_INVNAME;
        }

    pDocOut.allocate(pDocIn.Size);

    return ZS_SUCCESS;
}
ZStatus
zxmlDoc::codeFromUtf8(xmlCharEncoding &pEncodingCode,ZDataBuffer &pEncoded,ZDataBuffer &pIn)
{
    xmlCharEncodingHandlerPtr wEncodingHandler= xmlGetCharEncodingHandler(pEncodingCode);
    if (wEncodingHandler==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVNAME,
                              Severity_Severe,
                              "Cannot find encoding handler corresponding to encoding %d",pEncodingCode);
        return ZS_INVNAME;
        }
    pEncoded.allocateBZero(pIn.Size);
    
    return ZS_SUCCESS;
}
*/
//======================================Document Creation=================================
/**
 * @brief zxmlDoc::createDomDocument creates a new DOM document structure in memory
 * @param pVersion mandatory version number for the DOM document
 * @return
 */
ZStatus
zxmlDoc::createDomDocument(const char* pVersion)
{
_MODULEINIT_
    if (_xmlInternalDoc)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Severe,
                              "Cannot create document. Document already in use.");
        _RETURN_ ZS_INVOP;
        }
    if (!pVersion)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVVALUE,
                              Severity_Severe,
                              "No document version has been mentionned. Version cannot be null while creating DOM Document.");
        _RETURN_ ZS_INVVALUE;
        }
    _xmlInternalDoc= xmlNewDoc((const xmlChar*)pVersion);
    if (!_xmlInternalDoc)
        {
        setXMLZException(_GET_FUNCTION_NAME_,ZS_XMLERROR, Severity_Severe,"cannot create Dom document-xml internal error");
        _RETURN_ ZS_XMLERROR;
        }
    _RETURN_ ZS_SUCCESS;
}



ZStatus
zxmlDoc::createBlankRootElement(zxmlElement* &pNode,const char*pRootNodeName,zxmlNameSpace*pNameSpace)// no xml error on that routine
{
_MODULEINIT_
    pNode=nullptr;
    xmlNsPtr wNameSpace=nullptr;
    if (pNameSpace)
    {
        wNameSpace=pNameSpace->_xmlInternalNameSpace;
    }

    xmlNodePtr wNode=xmlNewNode( wNameSpace,(const xmlChar *)pRootNodeName); // xmlNewNode generates an XML_ELEMENT_NODE type

    if (!wNode)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,ZS_XMLERROR, Severity_Severe,"Cannot create blank node for root element");
        _RETURN_ ZS_XMLERROR;
        }


    if (xmlDocSetRootElement(_xmlInternalDoc, wNode))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,ZS_INVOP, Severity_Severe,"Cannot associate blank node as root element : root element exist already. Replace root node instead.");
        _RETURN_ ZS_INVOP;
        }


//    pNode= new zxmlNode(wNode);
    RootElement=pNode=(zxmlElement*)zxmlcreateNode(wNode);
    _RETURN_ ZS_SUCCESS;
}//createRootNodefromBlank
/**
 * @brief zxmlDoc::createRootNodefromTree
 *
 *  Check node :
 *
 *  node must exists  (be active)
 *  node must be an Element
 *
 *
 * @param pNode
 * @param pNameSpace
 * @return
 */
ZStatus
zxmlDoc::createRootElementfromTree(zxmlElement* &pNode) // no xml error on that routine
{
_MODULEINIT_


    if ((pNode->_xmlInternalNode)||(!pNode->isElement()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLINVROOTNAME,
                              Severity_Error,
                              "Given node is not an element or is not active.");
        _RETURN_ ZS_XMLINVROOTNAME;
        }
    if (RootElement)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVOP,
                                  Severity_Error,
                                  "Cannot associate tree node as root element : "
                                  "root element <%s>exist already. Replace root node instead.",
                                  RootElement->getName().toString());
            _RETURN_ ZS_INVOP;
            }
    if (!xmlDocSetRootElement(_xmlInternalDoc, pNode->_xmlInternalNode))
            {
            setXMLZException(_GET_FUNCTION_NAME_,
                             ZS_XMLERROR,
                             Severity_Severe,
                             "Cannot associate tree node <%s> as root element.",
                             pNode->getName().toCChar());
            _RETURN_ ZS_XMLERROR;
            }
    RootElement=pNode;
    if (ZVerbose)
            fprintf(stdout,"%s>>Root element <%s> has been created\n",
                    _GET_FUNCTION_NAME_,
                    RootElement->getName().toCChar());

    _RETURN_ ZS_SUCCESS;
}//createRootElementfromTree

/**
 * @brief zxmlDoc::replaceRootNodefromTree
 *
 *  Check node :
 *
 *  node must exists  (be active)
 *  node must be an Element
 *
 *
 * @param pNode
 * @param pNameSpace
 * @return
 */
ZStatus
zxmlDoc::replaceRootElementfromTree(zxmlElement* &pNode) // no xml error on that routine
{
_MODULEINIT_

    if ((pNode->_xmlInternalNode)||(!pNode->isElement()))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLINVROOTNAME,
                              Severity_Error,
                              "Given node is not an element or is not active.");
        _RETURN_ ZS_XMLINVROOTNAME;
        }

    xmlNodePtr wOldNode= xmlDocSetRootElement(_xmlInternalDoc, pNode->_xmlInternalNode);

    if (wOldNode)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,ZS_INVOP, Severity_Severe,"Cannot associate tree node as root element : root element exist already."
                              "Replace root node instead.");
        _RETURN_ ZS_INVOP;
        }
    if (RootElement)
        {

        if (ZVerbose)
                fprintf(stdout,"%s>>Root element <%s> has been deleted to be replaced\n",
                        _GET_FUNCTION_NAME_,
                        RootElement->getName().toString());
        zxmlremoveNode(RootElement);
        RootElement=nullptr;
        }
    RootElement=pNode;
    if (ZVerbose)
            fprintf(stdout,"%s>>Root element has been set to <%s>\n",
                    _GET_FUNCTION_NAME_,
                    RootElement->getName().toString());
    _RETURN_ ZS_SUCCESS;
}//setRootElementfromTree
/**
 * @brief zxmlDoc::newCDataSection create a CData section for the document, free to be set as child to an Element
 * @param pCDataSection
 * @param pNodeName
 * @param pCDataContent
 * @return
 */
ZStatus
zxmlDoc::newCDataSection(zxmlNode* pCDataSection,ZDataBuffer& pCDataContent)
{
_MODULEINIT_
    pCDataSection=nullptr;

    xmlNodePtr wInternalNode = xmlNewCDataBlock	( _xmlInternalDoc,
                                                  (const xmlChar *) pCDataContent.DataChar,
                                                  (int)pCDataContent.Size);
    if (wInternalNode==nullptr)
        {
        setXMLZException (_GET_FUNCTION_NAME_,ZS_XMLERROR,Severity_Error,"Error while creating CDATA section name ");
        _RETURN_ ZS_XMLERROR;
        }

//    pNode= new zxmlNode(wInternalNode);
    pCDataSection=zxmlcreateNode(wInternalNode);

    _RETURN_ ZS_SUCCESS;
}// newCDataNode$
ZStatus
zxmlDoc::newComment(zxmlNode* pComment,utf8VaryingString& pCommentContent)
{
_MODULEINIT_
    pComment=nullptr;
    xmlNodePtr wInternalNode = xmlNewDocComment( _xmlInternalDoc,
                                                  (const xmlChar *) pCommentContent.toUtf());
    if (wInternalNode==nullptr)
        {
        setXMLZException (_GET_FUNCTION_NAME_,ZS_XMLERROR,Severity_Error,"Error while creating comment node");
        _RETURN_ ZS_XMLERROR;
        }
//    pNode= new zxmlNode(wInternalNode);
    pComment=zxmlcreateNode(wInternalNode);

    _RETURN_ ZS_SUCCESS;
}// newComment
/**
 * @brief zxmlDoc::newDocumentNameSpace creates a global namespace for the whole document
 *      returns a namespace object that holds the namespace.
 * @param[out] pNameSpace created object
 * @param[in] pURL      url for namespace
 * @param[in] pPrefix   prefix for namespace
 * @return a ZStatus ZS_XMLERROR and ZException set with message if an error occurs, ZS_SUCCESS if anything went good
 */
ZStatus
zxmlDoc::newDocumentNameSpace(zxmlNameSpace* pNameSpace,const char* pURL,const char* pPrefix)
{
_MODULEINIT_
    xmlNsPtr wNs=nullptr;
    wNs=xmlNewGlobalNs(_xmlInternalDoc,(const xmlChar *)pURL,(const xmlChar *)pPrefix);
    if (!wNs)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Fatal,
                         "Cannot create global XML namespace for DOM document named <%s> url <%s> prefix <%s>",
                         getName().toString(),
                         pURL,
                         pPrefix);


        _RETURN_ ZS_XMLERROR;
        }

    pNameSpace=zxmlcreateNameSpace(wNs);

    _RETURN_ ZS_SUCCESS;
}// newDocumentNameSpace
/**
 * @brief zxmlDoc::newTextElement creates for the current document a new text element with pContent
 *                  It creates two nodes :
 *  - one of type XML_ELEMENT  (father)
 *  - one of type XML_TEXT     (child)
 *
 *  It may be added other nodes as child to the created element.
 *
 * @param pTextNode
 * @param pNodeName
 * @param pContent
 * @return
 */
ZStatus
zxmlDoc::newTextElement(zxmlElement* pTextNode, const char *pNodeName, utf8VaryingString &pContent, zxmlNameSpace* pNameSpace)
{
_MODULEINIT_
    pTextNode=nullptr;

    xmlNsPtr wNs=nullptr;
    if (pNameSpace)
            wNs=pNameSpace->_xmlInternalNameSpace;

    xmlNodePtr wEltNode= xmlNewDocNode(_xmlInternalDoc,wNs,(const xmlChar *)pNodeName,nullptr);
    if (wEltNode==nullptr)
        {
        setXMLZException (_GET_FUNCTION_NAME_,
                          ZS_XMLERROR,
                          Severity_Error,
                          "Error while creating Element Text node <%s>",pNodeName);
        _RETURN_ ZS_XMLERROR;
        }

    xmlNodePtr wTextNode = xmlNewDocTextLen(_xmlInternalDoc, (const xmlChar *) pContent.toUtf(),(int)pContent.strlen());

    if (wTextNode==nullptr)
        {
        setXMLZException (_GET_FUNCTION_NAME_,
                          ZS_XMLERROR,
                          Severity_Error,
                          "Error while creating Text node <%s>",pNodeName);
        xmlFreeNode(wEltNode);
        _RETURN_ ZS_XMLERROR;
        }
    if (!xmlAddChild(wEltNode,wTextNode))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Severe,
                              "Cannot link element node <%s> to its text child. Nodes have not been created. ",pNodeName);
        xmlFreeNode(wEltNode);
        xmlFreeNode(wTextNode);
        _RETURN_ ZS_XMLERROR;
        }

 //   pTextNode= new zxmlNode(wEltNode);
    pTextNode=(zxmlElement*)zxmlcreateNode(wEltNode);
    _RETURN_ ZS_SUCCESS;
}//createTextNode
/**
 * @brief zxmlDoc::newAttribute creates a new attribute for the document, without being linked to a particular node yet
 * @param pAttribute
 * @param pName
 * @param pValue
 * @return
 */
ZStatus
zxmlDoc::newAttribute(zxmlAttribute*pAttribute , const char*pName, const char*pValue)
{
_MODULEINIT_
    pAttribute=nullptr;

    xmlAttrPtr wInternalAttribute = xmlNewDocProp(_xmlInternalDoc, (const xmlChar *) pName,(const xmlChar *) pValue);

    if (wInternalAttribute==nullptr)
        {
        setXMLZException (_GET_FUNCTION_NAME_,
                          ZS_XMLERROR,
                          Severity_Error,
                          "Error while creating Attribute <%s> for document ",pName);
        _RETURN_ ZS_XMLERROR;
        }

//    pAttribute= new zxmlAttribute(wInternalAttribute);
    pAttribute=zxmlcreateAttribute(wInternalAttribute);
    _RETURN_ ZS_SUCCESS;
}//createTextNode
//==================Dom document parsing===================================

ZStatus
zxmlDoc::parseXMLFile(const char*pFilename)
{
_MODULEINIT_

    if (_xmlInternalDoc)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Severe,
                              "Cannot parse an existing DOM document (active)");
        _RETURN_ ZS_INVOP;
        }

    if(DocParser!=nullptr)
                {
                 delete DocParser;
                DocParser=nullptr;
                }
    DocParser=new zxmlParser;
    xmlCtxtResetLastError	(DocParser->Context);

    _xmlInternalDoc=   xmlParseFile(pFilename);
    if (_xmlInternalDoc==nullptr)
        {

        setXMLZException(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              " Error while parsing xml file <%s>",pFilename
                              );
        _RETURN_ ZS_XMLERROR;
        }

    _RETURN_ getRootElement(RootElement);  // update the RootElement with what has been parsed
} // parseXMLLoadedDoc

ZStatus
zxmlDoc::parseXMLLoadedDoc(void)
{
_MODULEINIT_

    if(DocParser!=nullptr)
                {
                 delete DocParser;
                DocParser=nullptr;
                }
    DocParser=new zxmlParser;
    xmlCtxtResetLastError	(DocParser->Context);

    _xmlInternalDoc=   xmlParseMemory(xmlContent.DataChar,xmlContent.Size);
    if (_xmlInternalDoc==nullptr)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              " Error while parsing memory loaded xml file <%s>",URIDocFile.toString()
                              );
        _RETURN_ ZS_XMLERROR;
        }

    _RETURN_ ZS_SUCCESS;
} //



utfdescString
decode_XMLParseOptions(int pOptions)
{
utfdescString wOptions;
    wOptions.clear();
    if (pOptions&XML_PARSE_RECOVER)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_RECOVER"); /* recover on errors */
    if (pOptions&XML_PARSE_NOENT)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_NOENT"); /* substitute entities */
    if (pOptions&XML_PARSE_DTDLOAD)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_DTDLOAD");/* load the external subset */
    if (pOptions&XML_PARSE_DTDATTR)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_DTDATTR");/* default DTD attributes */
    if (pOptions&XML_PARSE_DTDVALID)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_DTDVALID");/* validate with the DTD */
    if (pOptions&XML_PARSE_NOERROR)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_NOERROR");/* suppress error reports */
    if (pOptions&XML_PARSE_NOWARNING)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_NOWARNING");/* suppress warning reports */
    if (pOptions&XML_PARSE_PEDANTIC)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_PEDANTIC");/* pedantic error reporting */
    if (pOptions&XML_PARSE_NOBLANKS)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_NOBLANKS");/* remove blank nodes */
    if (pOptions&XML_PARSE_SAX1)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_SAX1");/* use the SAX1 interface internally */
    if (pOptions&XML_PARSE_XINCLUDE)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_XINCLUDE");/* Implement XInclude substitition  */
    if (pOptions&XML_PARSE_NONET)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_NONET");/* Forbid network access */
    if (pOptions&XML_PARSE_NODICT)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_NODICT");/* Do not reuse the context dictionnary */
    if (pOptions&XML_PARSE_NSCLEAN)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_NSCLEAN");/* remove redundant namespaces declarations */
    if (pOptions&XML_PARSE_NOCDATA)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_NOCDATA");/* merge CDATA as text nodes */
    if (pOptions&XML_PARSE_NOXINCNODE)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_NOXINCNODE");/* do not generate XINCLUDE START/END nodes */
    if (pOptions&XML_PARSE_COMPACT)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_COMPACT");/*   compact small text nodes; no modification of
                                                                    the tree allowed afterwards (will possibly
                                                                    crash if you try to modify the tree) */
    if (pOptions&XML_PARSE_OLD10)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_OLD10");/* parse using XML-1.0 before update 5 */
    if (pOptions&XML_PARSE_NOBASEFIX)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_NOBASEFIX");/* do not fixup XINCLUDE xml:base uris */
    if (pOptions&XML_PARSE_HUGE)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_HUGE");/* relax any hardcoded limit from the parser */
    if (pOptions&XML_PARSE_OLDSAX)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_OLDSAX");/* parse using SAX2 interface before 2.7.0 */
    if (pOptions&XML_PARSE_IGNORE_ENC)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_IGNORE_ENC");/* ignore internal document encoding hint */
    if (pOptions&XML_PARSE_BIG_LINES)
                wOptions.addConditionalOR((const utf8_t*)"XML_PARSE_BIG_LINES");/* Store big lines numbers in text PSVI field */

    return wOptions;
}

ZStatus
zxmlDoc::loadAndParseXMLDoc(const char* pURL,const char* pEncoding,int pOptions)
{
_MODULEINIT_
    if (_xmlInternalDoc)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Severe,
                              "Cannot parse an existing DOM document (active)");
        _RETURN_ ZS_INVOP;
        }
    pOptions += XML_PARSE_NOBLANKS ; // by default remove blank nodes
    _xmlInternalDoc= xmlReadFile(pURL,pEncoding,pOptions);
    if (_xmlInternalDoc==nullptr)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         " Error while parsing memory loaded xml file <%s> requested encoding <%s> requested parsing options <%s>",
                         pURL,
                         pEncoding,
                         decode_XMLParseOptions(pOptions).toCChar()
                         );
        _RETURN_ ZS_XMLERROR;
        }

    _RETURN_ getRootElement(RootElement);
}// loadAndParseXMLDoc


bool zxmlDoc::isWellFormed(void)
{
    if (!_xmlInternalDoc)
                return false;
    return (_xmlInternalDoc->properties&XML_DOC_WELLFORMED);
}

bool zxmlDoc::isHtmlDocument(void)
{
    if (!_xmlInternalDoc)
                return false;
    return (_xmlInternalDoc->properties&XML_DOC_HTML);
}
/**
 * @brief zxmlDoc::getNamedRootElement gets document's root element and checks it corresponds to pRootNodeName.
 *
 * @param pRootNodeName
 * @param pRootElement
 * @return  a ZStatus with ZS_XMLINVROOTNAME if root name does not correspond, ZS_SUCCESS otherwise
 */
ZStatus
zxmlDoc::getNamedRootElement(const char*pRootNodeName, zxmlElement* &pRootElement)
{
_MODULEINIT_
    if (RootElement)
            {
            if (!strcmp(RootElement->getName().toCString_Strait(),pRootNodeName))
                    {
                    pRootElement= RootElement;
                    return ZS_SUCCESS;
                    }
                else
                    {
                    return ZS_NOTFOUND;
                    }
            }
xmlNodePtr wNodePtr=nullptr;
    pRootElement=nullptr;
    xmlCtxtResetLastError	(DocParser->Context);
    wNodePtr=   xmlDocGetRootElement (_xmlInternalDoc);
    if (wNodePtr==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLEMPTY,
                              Severity_Error,
                              "Not root element has been found. Document<%s> appears to be empty.",
                              URIDocFile.toString()
                              );
//        ZException.setComplement(getXMLLastParserError().toString());
        return ZS_XMLEMPTY;
        }

    if (xmlStrcmp(wNodePtr->name,(const xmlChar*)pRootNodeName))
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLINVROOTNAME,
                              Severity_Error,
                              "Requested root element name <%s> does not match <%s>. Invalid document<%s>.",
                              pRootNodeName,
                              wNodePtr->name,
                              URIDocFile.toString()
                              );
        return ZS_XMLINVROOTNAME;
        }


//    pRootElement=new zxmlNode(wNodePtr);
    RootElement=pRootElement=(zxmlElement*)zxmlcreateNode(wNodePtr);

    return ZS_SUCCESS;
} // getNamedRootElement

ZStatus
zxmlDoc::getRootElement( zxmlElement* &pRootElement)
{
_MODULEINIT_

xmlNodePtr wNodePtr=nullptr;
    pRootElement=nullptr;

    if (!RootElement)
        {
        wNodePtr=   xmlDocGetRootElement (_xmlInternalDoc);
        if (wNodePtr==nullptr)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_XMLEMPTY,
                                  Severity_Error,
                                  "Not root element has been found. Document<%s> appears to be empty.",
                                  URIDocFile.toString()
                                  );
    //        ZException.setComplement(getXMLLastParserError().toString());
            _RETURN_ ZS_XMLEMPTY;
            }
//        RootElement=pRootElement=new zxmlNode(wNodePtr);
        RootElement=(zxmlElement*)zxmlcreateNode(wNodePtr);
        }

    pRootElement=RootElement;
    _RETURN_ ZS_SUCCESS;
} // getRootElement

ZStatus
zxmlDoc::getDocComment(utf8VaryingString& pNodeComment)
{
_MODULEINIT_

xmlNodePtr wNodePtr=nullptr;
    wNodePtr=   xmlDocGetRootElement (_xmlInternalDoc);
    if (wNodePtr==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLEMPTY,
                              Severity_Error,
                              "Not root element has been found. Document<%s> appears to be empty.",
                              URIDocFile.toString()
                              );
//        ZException.setComplement(getXMLLastParserError().toString());
        _RETURN_ ZS_XMLEMPTY;
        }
    wNodePtr = wNodePtr->children;
    while (wNodePtr&&(wNodePtr->type!=XML_COMMENT_NODE))
                    wNodePtr = wNodePtr->next ;
    if (!wNodePtr)
        {
        _RETURN_ ZS_NOTFOUND;
        }
    xmlBufferPtr wxmlBuf;
    if (xmlNodeBufGetContent(wxmlBuf,wNodePtr)<0) // gets any node content including CDATA section
        {
        pNodeComment.clear();
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "Cannot get CData content for node <%s>",
                              getName().toString());
        _RETURN_ ZS_XMLERROR;
        }

    pNodeComment.setData((unsigned char*)xmlBufferContent(wxmlBuf),(size_t)xmlBufferLength(wxmlBuf));
    _RETURN_ ZS_SUCCESS;
} // getDocComment


ZStatus
zxmlDoc::searchNameSpaceByHref (zxmlNameSpace*pNameSpace, const char* pRef, zxmlElement *&pElement)
{
_MODULEINIT_
    if (!pElement && !RootElement)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Severe,
                              "Root element has not yet been defined for DOM document and no element has been given for search from");
        _RETURN_ ZS_INVOP;
        }
    xmlNodePtr wNodeStart;
    if (RootElement)
            wNodeStart=RootElement->_xmlInternalNode;
    if (pElement)
            wNodeStart=pElement->_xmlInternalNode;
    xmlNsPtr wNs=xmlSearchNsByHref(_xmlInternalDoc,pElement->_xmlInternalNode,(const xmlChar*)pRef);
    if (!wNs)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                          ZS_NOTFOUND,
                          Severity_Error,
                          "Namespace has not been found");
        return ZS_NOTFOUND;
        }
    pNameSpace=zxmlcreateNameSpace(wNs);
    _RETURN_ ZS_SUCCESS;
}

ZStatus
zxmlDoc::searchNameSpaceByURL(zxmlNameSpace*pNameSpace, const char* pURL, zxmlElement *&pElement)
{
_MODULEINIT_
    if (!pElement && !RootElement)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Severe,
                              "Root element has not yet been defined for DOM document and no element has been given for search from");
        _RETURN_ ZS_INVOP;
        }
    xmlNodePtr wNodeStart;
    if (RootElement)
            wNodeStart=RootElement->_xmlInternalNode;
    if (pElement)
            wNodeStart=pElement->_xmlInternalNode;
    xmlNsPtr wNs=xmlSearchNs(_xmlInternalDoc,pElement->_xmlInternalNode,(const xmlChar*)pURL);
    if (!wNs)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                          ZS_NOTFOUND,
                          Severity_Error,
                          "Namespace has not been found");
        return ZS_NOTFOUND;
        }
    pNameSpace=zxmlcreateNameSpace(wNs);
    _RETURN_ ZS_SUCCESS;
}

utffieldNameString zxmlDoc::getName(void)
{
    if (!_xmlInternalDoc)
            return utffieldNameString((utf8_t*)"null");
    return (utffieldNameString((const utf8_t*)_xmlInternalDoc->name));
}

ZStatus zxmlDoc::getDocEncoding(utffieldNameString &pEncoding)
{
    pEncoding.clear();
    if (_xmlInternalDoc==nullptr)
                return ZS_XMLERROR;
    pEncoding=(const utf8_t*) _xmlInternalDoc->encoding;
    return ZS_SUCCESS;
}
ZStatus
zxmlDoc::getDocEncoding(xmlCharEncoding &pEncodingCode)
{
_MODULEINIT_
    ZStatus wSt;
    ZDataBuffer wZDB;
    wSt=URIDocFile.loadContent(wZDB);
    if (wSt!=ZS_SUCCESS)
                _RETURN_ wSt;
    pEncodingCode= xmlDetectCharEncoding(wZDB.Data,wZDB.Size);
    if (pEncodingCode<0)
            _RETURN_ ZS_INVVALUE;

    _RETURN_ ZS_SUCCESS;
}

int
zxmlDoc::getDocCharset(void)
{
    if (_xmlInternalDoc==nullptr)
                                    return -1;
    return _xmlInternalDoc->charset;
}
//#include <libxml/encoding.h>
utffieldNameString zxmlDoc::getDocVersion(void)
{
_MODULEINIT_
    if (_xmlInternalDoc==nullptr)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,ZS_INVOP,Severity_Error,"Current xml document is not an active document");
            ZException.exit_abort();
            }
    if (!_xmlInternalDoc->version)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,ZS_XMLERROR,Severity_Error,"No version for current xml document");
        ZException.exit_abort();
        }
    _RETURN_ utffieldNameString((const utf8_t*) _xmlInternalDoc->version);
}//getDocVersion


zxmlParser::zxmlParser()
{
_MODULEINIT_

    if (!ZXML_Init)
        {
        xmlInitParser();
        LIBXML_TEST_VERSION
        }
    Context= xmlNewParserCtxt();
    if (Context==nullptr)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Fatal,
                              "cannot initialize XML context");
        ZException.exit_abort();
        }
    if (xmlInitParserCtxt(Context)<0)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Fatal,
                              "cannot initialize XML Parser context");
        ZException.exit_abort();
        }

    _RETURN_;
}

ZStatus
zxmlDoc::logicalDump(FILE*pOutput,int pSpaceIndent)
{
_MODULEINIT_
    if (_xmlInternalDoc==nullptr)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,ZS_INVOP,Severity_Error,"Current xml document is not an active document");
            _RETURN_ ZS_INVOP;
            }

    if (!RootElement)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_INVOP,
                                  Severity_Severe,
                                  "Current xml document does not have a root element yet defined");
            _RETURN_ ZS_INVOP;
            }
    fprintf(pOutput," Document <%s> <%s> version <%s> \n",
          RootElement->getName().toString(),
          (const char*)_xmlInternalDoc->name,
          getDocVersion().toString());

    RootElement->dumpCurrent(pOutput,0,pSpaceIndent);
    RootElement->cascadeDump(pOutput,1,pSpaceIndent);
    _RETURN_ ZS_SUCCESS;
}
/**
 * @brief zxmlDoc::writeDOMDocument Saves the current DOM Document to file pFilename using UTF8 standard character encoding
 *                                  if pFormat is set, then leading spaces are added by element/node level
 * @param pFilename
 * @param pFormat
 * @return
 */
ZStatus
zxmlDoc::writeDOMDocument (const char* pFilename, bool pFormat)
{
_MODULEINIT_
    if (xmlSaveFormatFile(pFilename,_xmlInternalDoc,(int)pFormat?1:0) < 0)
    {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "Cannot write DOM document to <%s>",pFilename);
        _RETURN_ ZS_XMLERROR;
    }
    _RETURN_ ZS_SUCCESS;
}
/**
 * @brief zxmlDoc::writeDocumentEncoded Saves the current DOM Document to file pFilename using character encoding pEncoding
 *                                      if pFormat is set, then leading spaces are added by element/node level
 * @param pFilename
 * @param pEncoding
 * @param pFormat
 * @return
 */
ZStatus
zxmlDoc::writeDOMDocumentEncoded (const char* pFilename,const char*pEncoding, bool pFormat)
{
_MODULEINIT_
    xmlOutputBufferPtr wDocBuffer;

    if (xmlSaveFormatFileEnc(pFilename,_xmlInternalDoc,pEncoding,(int)pFormat?1:0) < 0)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         " Cannot write DOM Document as encoded <%s> on filename <%s>",pEncoding,pFilename);
        _RETURN_ ZS_XMLERROR;
        }
    _RETURN_ ZS_SUCCESS;
}// writeDocumentEncoded



#endif // ZXMLDOC_CPP
