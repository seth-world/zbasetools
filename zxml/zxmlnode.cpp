#ifndef ZXMLNODE_CPP
#define ZXMLNODE_CPP

#include <zxml/zxmldoc.h>
#include <zxml/zxmlnode.h>
#include <zxml/zxmlattribute.h>
#include <zxml/zxmlnamespace.h>

#include <ztoolset/zexceptionmin.h>
#include <zxml/zxmlerror.h>
//#include <libxml/parserInternals.h>.h>
#include <zxml/zxml.h>
//================Nodes==========================


ZNodeCollection::~ZNodeCollection()
{
  while (count())
  {
    XMLderegister(popR());
  }
}

ZElementCollection::~ZElementCollection()
{
  while (count())
  {
    XMLderegister(popR());
  }
}
ZElementCollection& ZElementCollection::_copyFrom(ZElementCollection& pIn)
{
  reset();
  for (long wi=0;wi < pIn.count(); wi++)
    push(pIn[wi]);
  return *this;
}
ZElementCollection& ZElementCollection::_copyFrom(ZNodeCollection& pIn)
{
  reset();
  for (long wi=0;wi < pIn.count(); wi++)
    push((zxmlElement*)pIn[wi]);
  return *this;
}

unsigned long
zxmlNode::getChildElementCount()
{
    if (_xmlInternalNode==nullptr)
                            return -1;
    return xmlChildElementCount(_xmlInternalNode);
}

/**
 * @brief zxmlNode::getFirstChild gets the first node as child of current element
 * @param[out] pFirstNode a pointer to zxmlNode object for the found node (or nullptr if not found)
 * @return ZStatus ZS_SUCCESS if a node is found, ZS_EMPTY if not
 */
ZStatus
zxmlNode::getFirstChild(zxmlNode* &pFirstNode) const
{
    pFirstNode=nullptr;

    if (_xmlInternalNode->children==nullptr)
                            return (ZS_EMPTY);


//    pFirstNode=new zxmlNode(_xmlInternalNode->children);
    pFirstNode=zxmlcreateNode(_xmlInternalNode->children);
    return ZS_SUCCESS;
}// getFirstChild
/**
 * @brief zxmlNode::getLastChild
 * @param[out] pLastNode a pointer to zxmlNode object for the found node (or nullptr if not found)
 * @return ZStatus ZS_SUCCESS if a node is found, ZS_EMPTY if not
 */
ZStatus
zxmlNode::getLastChild(zxmlNode* &pLastNode)
{
    pLastNode=nullptr;
    if (_xmlInternalNode->last==nullptr)
                            return (ZS_EMPTY);

//    pLastNode=new zxmlNode(_xmlInternalNode->last);
    pLastNode=zxmlcreateNode(_xmlInternalNode->last);
    return ZS_SUCCESS;
}// getLastChild
/**
 * @brief zxmlNode::getNextNode
 * @param[out] pNextNode a pointer to zxmlNode object for the found node (or nullptr if not found)
 * @return ZStatus ZS_SUCCESS if a node is found, ZS_EOF if no more nodes are available
 */
ZStatus
zxmlNode::getNextNode(zxmlNode* &pNextNode)
{
    pNextNode=nullptr;

    if (_xmlInternalNode->next==nullptr)
                            return (ZS_EOF);


//    pNextNode=new zxmlNode(_xmlInternalNode->next);
    pNextNode=zxmlcreateNode(_xmlInternalNode->next);
    return ZS_SUCCESS;
}// getNextNode

ZStatus
zxmlNode::getPreviousNode(zxmlNode* &pPrevNode)
{
    pPrevNode=nullptr;

    if (_xmlInternalNode->prev==nullptr)
                            return (ZS_OUTBOUNDLOW);


//    pPrevNode=new zxmlNode(_xmlInternalNode->prev);
    pPrevNode=zxmlcreateNode(_xmlInternalNode->prev);
    return ZS_SUCCESS;
}// getNextNode

void
zxmlNode::setLanguage(const char *pLanguage)
{
    xmlNodeSetLang(_xmlInternalNode,(const xmlChar*)pLanguage);

}// setLanguage

utffieldNameString zxmlNode::getLanguage(void)
{
    return utffieldNameString((const utf8_t*)xmlNodeGetLang(_xmlInternalNode));

}// getLanguage

bool
zxmlNode::hasCData() const
{
    xmlNodePtr wChild=_xmlInternalNode->children;
    while (wChild)
        {
        if ((wChild->type==XML_CDATA_SECTION_NODE)&&!xmlIsBlankNode(wChild))
                        return true;
        wChild=wChild->next;
        }
    return false;
}
bool
zxmlNode::hasText() const
{
    xmlNodePtr wChild=_xmlInternalNode->children;
    while (wChild)
        {
        if ((wChild->type==XML_TEXT_NODE)&&!xmlIsBlankNode(wChild))
                        return true;

        wChild=wChild->next;
        }
    return false;
}
bool
zxmlNode::hasElement() const
{
    xmlNodePtr wChild=_xmlInternalNode->children;
    while (wChild)
        {
        if (wChild->type==XML_ELEMENT_NODE)
                        return true;
        wChild=wChild->next;
        }
    return false;
}
bool
zxmlNode::hasComment() const
{
    xmlNodePtr wChild=_xmlInternalNode->children;
    while (wChild)
        {
        if ((wChild->type==XML_COMMENT_NODE)&&!xmlIsBlankNode(wChild))
                        return true;
        wChild=wChild->next;
        }
    return false;
}

bool
zxmlNode::hasAttribute() const
{
    return _xmlInternalNode->properties;
}
ZStatus
zxmlNode::getFirstChildCData(zxmlNode* &pNode)
{
    pNode=nullptr;
    xmlNodePtr wNode=nullptr;
    if (_xmlInternalNode->children==nullptr)
                                return (ZS_EMPTY);
    wNode=_xmlInternalNode->children;
    while ((wNode->type!=XML_CDATA_SECTION_NODE)&&!xmlIsBlankNode(wNode))
        {
        wNode=wNode->next;
        }
    if (wNode->type!=XML_CDATA_SECTION_NODE)
                return ZS_NOTFOUND;
//    pNode=new zxmlNode(wNode);
    pNode=zxmlcreateNode(wNode);
    return ZS_SUCCESS;
}// getFirstChildCData
ZStatus
zxmlNode::getNextCDataSibling(zxmlNode* &pNode)
{
    pNode=nullptr;
    xmlNodePtr wNode=nullptr;
    if (_xmlInternalNode->next==nullptr)
                                return (ZS_EOF);
    wNode=_xmlInternalNode->next;
    while ((wNode->type!=XML_CDATA_SECTION_NODE)&&!xmlIsBlankNode(wNode))
        {
        wNode=wNode->next;
        }
    if (wNode->type!=XML_CDATA_SECTION_NODE)
                return ZS_EOF;
//    pNode=new zxmlNode(wNode);

    pNode=zxmlcreateNode(wNode);
    return ZS_SUCCESS;
}// getNextCDataSibling
/**
 * @brief zxmlNode::getFirstChildText gets the first text node is any
 * @param pNode a pointer to text node if found- nullptr if not
 * @return
 */
ZStatus
zxmlNode::getFirstChildText(zxmlNode* &pNode)
{

    xmlNodePtr wNode=nullptr,wNodeLast=_xmlInternalNode->last;
    if (_xmlInternalNode->children==nullptr)
                                return (ZS_EMPTY);
    wNode=_xmlInternalNode->children;

    while (wNode&&(wNode->type!=XML_TEXT_NODE)&&(wNode!=wNodeLast)&& xmlIsBlankNode(wNode))
        {
        wNode=wNode->next;
        }


    if (!wNode)
            {
            pNode=nullptr;
            return ZS_NOTFOUND;
            }
    if (wNode->type!=XML_TEXT_NODE)
                {
                pNode=nullptr;
                return ZS_NOTFOUND;
                }

//    pNode=new zxmlNode(wNode);
//    pNode=zxmlsearchNodeByInternal(wNode);
//    if (!pNode)
            pNode=zxmlcreateNode(wNode);
    return ZS_SUCCESS;
}// getFirstChildText

utf8String zxmlNode::getNodePath(void) const
{
    utf8String wPath;
    xmlChar* wBuf=xmlGetNodePath(_xmlInternalNode);
    wPath=wBuf;
    xmlFree(wBuf);
    return wPath;
}


ZStatus
zxmlNode::getNextTextSibling(zxmlNode* &pNode)
{
//    pNode=nullptr;
    xmlNodePtr wNode=nullptr,wNodeLast=_xmlInternalNode->last;

    if (_xmlInternalNode->next==nullptr)
                                return (ZS_EOF);
    wNode=_xmlInternalNode->next;

    while (wNode&&(wNode->type!=XML_TEXT_NODE)&&(wNode!=wNodeLast)&& xmlIsBlankNode(wNode))
        {
        wNode=wNode->next;
        }

    if (!wNode)
            {
            pNode=nullptr;
            return ZS_EOF;
            }
    if (wNode->type!=XML_TEXT_NODE)
            {
            pNode=nullptr;
            return ZS_EOF;
            }

//    pNode=new zxmlNode(wNode);
//    pNode=zxmlsearchNodeByInternal(wNode);
//    if (!pNode)
            pNode=zxmlcreateNode(wNode);
    return ZS_SUCCESS;
}// getNextTextSibling

/**
 * @brief zxmlNode::getNextText obtains the sibling Text node and its content
 * @param pTextNode
 * @param pText
 * @return
 */
ZStatus
zxmlNode::getNextText(zxmlNode *pTextNode,utf8VaryingString &pText)
{
pTextNode=nullptr;
ZStatus wSt=getNextTextSibling(pTextNode);
    if (wSt!=ZS_SUCCESS)
                    return wSt;
    return pTextNode->getNodeText(pText);
}

ZStatus
zxmlNode::getFirstChildComment(zxmlNode* &pNode)
{

    xmlNodePtr wNode=nullptr,wNodeLast=_xmlInternalNode->last;
    if (_xmlInternalNode->children==nullptr)
                                return (ZS_EMPTY);
    wNode=_xmlInternalNode->children;
    while (wNode&&(wNode->type!=XML_COMMENT_NODE)&&(wNode!=wNodeLast))
        {
        wNode=wNode->next;
        }
    if (!wNode)
            {
            pNode=nullptr;
            return ZS_NOTFOUND;
            }
    if (wNode->type!=XML_COMMENT_NODE)
                {
                pNode=nullptr;
                return ZS_NOTFOUND;
                }

//    pNode=new zxmlNode(wNode);
//    pNode=zxmlsearchNodeByInternal(wNode);
//    if (!pNode)
            pNode=zxmlcreateNode(wNode);
    return ZS_SUCCESS;
}// getFirstChildComment
ZStatus
zxmlNode::getNextCommentSibling(zxmlNode* &pNode)
{
//    pNode=nullptr;
    xmlNodePtr wNode=nullptr,wNodeLast=_xmlInternalNode->last;

    if (_xmlInternalNode->next==nullptr)
                                return (ZS_EOF);
    wNode=_xmlInternalNode->next;
    while (wNode&&(wNode->type!=XML_COMMENT_NODE)&&(wNode!=wNodeLast))
        {
        wNode=wNode->next;
        }
    if (!wNode)
            {
            pNode=nullptr;
            return ZS_EOF;
            }
    if (wNode->type!=XML_COMMENT_NODE)
            {
            pNode=nullptr;
            return ZS_EOF;
            }
//    pNode=new zxmlNode(wNode);
//    pNode=zxmlsearchNodeByInternal(wNode);
//    if (!pNode)
    pNode=zxmlcreateNode(wNode);
    return ZS_SUCCESS;
}// getNextCommentSibling

ZStatus
zxmlNode::getFirstAttribute(zxmlAttribute* &pAttribute)
{

    xmlAttrPtr wAttr=nullptr;
    if (_xmlInternalNode->properties==nullptr)
                                return (ZS_EMPTY);
    wAttr=_xmlInternalNode->properties;
    while (wAttr&&(wAttr->type!=XML_ATTRIBUTE_NODE))
        {
        wAttr=wAttr->next;
        }
    if (!wAttr)
            {
            pAttribute=nullptr;
            return ZS_NOTFOUND;
            }

    if (wAttr->type!=XML_ATTRIBUTE_NODE)
            {
            pAttribute=nullptr;
            return ZS_NOTFOUND;
            }
//    pAttribute=new zxmlAttribute((xmlAttrPtr)wNode);
//    pAttribute=zxmlsearchAttributeByInternal((xmlAttrPtr)wAttr);
//    if (!pAttribute)
    pAttribute=zxmlcreateAttribute((xmlAttrPtr)wAttr);
    return ZS_SUCCESS;
}// getFirstAttribute




ZStatus
zxmlNode::getChildByName(zxmlNode* &pNode, const utf8VaryingString &pName)
{
    xmlNodePtr wChild=_xmlInternalNode->children;
    pNode=nullptr;
    while (wChild!=nullptr)
        {
        if (wChild->name)
        {
        if (xmlStrcmp (wChild->name,(xmlChar*)pName.toCChar())==0)
                {
 //               pNode=new zxmlNode(wChild);
                pNode=zxmlcreateNode(wChild);
                return ZS_SUCCESS;
                }
        }
        wChild=wChild->next;
        }
    return ZS_NOTFOUND;
}// getChildByName

/**
 * @brief zxmlNode::getAllChildren find all children nodes with name pName
 */
ZNodeCollection zxmlNode::getAllChildren(const utf8VaryingString &pName)
{
  ZNodeCollection wCollection;
  xmlNodePtr wChild=_xmlInternalNode->children;

  while (wChild!=nullptr)
  {
    if (wChild->name)
    {
      if (xmlStrcmp (wChild->name,(xmlChar*)pName.toCChar())==0)
      {
        wCollection.push(zxmlcreateNode(wChild));
      }
    }
    wChild=wChild->next;
  }
  return wCollection;
}//getAllChildren


bool
zxmlNode::isElement(void) const
{
    if (_xmlInternalNode==nullptr)
            return false;
    return (_xmlInternalNode->type==XML_ELEMENT_NODE);
}

bool
zxmlNode::isText(void) const
{
    if (_xmlInternalNode==nullptr)
            return false;
    return (_xmlInternalNode->type==XML_TEXT_NODE);
}

bool
zxmlNode::isCData(void) const
{
    if (_xmlInternalNode==nullptr)
                            return false;
    return (_xmlInternalNode->type==XML_CDATA_SECTION_NODE);

}

utf8String zxmlNode::getName(void) const
{

    if (_xmlInternalNode==nullptr)
                            return utf8String();

    return utf8String((const utf8_t*)_xmlInternalNode->name);
}
int
zxmlNode::getLine(void) const
{

    if (_xmlInternalNode==nullptr)
                            return -1;

    return _xmlInternalNode->line;
}
ZStatus
zxmlNode::getCData(ZDataBuffer& pCData) const
{

    if (!isCData())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Error,
                              "Node <%s> is either invalid or is not a CData node ",
                              getName().toCChar());
        return  ZS_INVOP;
        }
    xmlBufferPtr wxmlBuf=nullptr;
    if (xmlNodeBufGetContent(wxmlBuf,_xmlInternalNode)<0) // gets any node content including CDATA section
        {
        pCData.clear();
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "Cannot get CData content for node <%s>",
                              getName().toCChar());
        return  ZS_XMLERROR;
        }
    pCData.setData((unsigned char*)xmlBufferContent(wxmlBuf),(size_t)xmlBufferLength(wxmlBuf));
    return  ZS_SUCCESS;
}//getCData
/**
 * @brief zxmlNode::getNodeContent extract a node's CData content
 * @param[out] pCData
 * @return
 */
ZStatus
zxmlNode::getNodeContent(utf8VaryingString& pContent) const
{


    xmlBufPtr wxmlBuf=nullptr;
    if (xmlBufGetNodeContent(wxmlBuf,_xmlInternalNode)<0) // gets any node content including CDATA section
        {
        pContent.clear();
        setXMLZException(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "Cannot get node content ");
        return  ZS_XMLERROR;
        }
    pContent.strset(( utf8_t*)xmlBufContent(wxmlBuf));
    return  ZS_SUCCESS;
}//getNodeContent

ZStatus
zxmlNode::getNodeText(utf8VaryingString& pText)
{


 //   xmlBufPtr wxmlBuf=nullptr;

//    xmlChar* wBuf=xmlNodeListGetString(_xmlInternalNode->doc,_xmlInternalNode,0);

    xmlChar* wBuf=xmlNodeGetContent(_xmlInternalNode);


//    pText.setString((const char*)_xmlInternalNode->content);
    if (wBuf)
        {
        pText.strset((utf8_t*)wBuf);
        xmlFree (wBuf);
        }
    return  ZS_SUCCESS;
}//getNodeContent
/**
 * @brief zxmlNode::getNodeRaw get a node's raw content including CDATA Section content
 * @param[out] pText a ZDataBuffer with raw content
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlNode::getNodeRaw(utf8VaryingString& pText)
{


        pText.clear();
//    pText.setString((const char*)_xmlInternalNode->content);
        if (_xmlInternalNode->content)
                    pText.strset((const utf8_t*)_xmlInternalNode->content);
    return  ZS_SUCCESS;
}//getNodeContent


/**
 * @brief zxmlNode::getText get a node's content as text including CDATA
 * @param[out] pText a ZDataBuffer with text
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlNode::getText(ZDataBuffer& pText) const
{

    if (!isText())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Error,
                              "Node <%s> is either invalid or is not a Text node ",
                              getName().toCChar());
        return  ZS_INVOP;
        }
    xmlBufferPtr wxmlBuf=nullptr;
    if (xmlNodeBufGetContent(wxmlBuf,_xmlInternalNode)<0) // gets any node content including CDATA section
        {
        pText.clear();
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "Cannot get content for node <%s>",
                              getName().toCChar());
        return  ZS_XMLERROR;
        }
    pText.setData((unsigned char*)xmlBufferContent(wxmlBuf),(size_t)xmlBufferLength(wxmlBuf));

    xmlFree(wxmlBuf);
    // conversion from utf8
//    xmlCharEncodingHandler
    //
    pText.addConditionalTermination();
    return  ZS_SUCCESS;
}//getText

ZStatus
zxmlNode::getText(utf8VaryingString& pText) const
{

/*    if (!isText())
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVOP,
                              Severity_Error,
                              "Node <%s> is either invalid or is not a Text node ",
                              getName().toCChar());
        return  ZS_INVOP;
        }*/
    xmlBufferPtr wxmlBuf=nullptr;
    if (xmlNodeBufGetContent(wxmlBuf,_xmlInternalNode)<0) // gets any node content including CDATA section
        {
        pText.clear();
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "Cannot get content for node <%s>",
                              getName().toCChar());
        return  ZS_XMLERROR;
        }
    pText.setData((unsigned char*)xmlBufferContent(wxmlBuf),(size_t)xmlBufferLength(wxmlBuf));

    // conversion from utf8
//    xmlCharEncodingHandler
    //
    xmlFree(wxmlBuf);
    pText.addConditionalTermination();
    return  ZS_SUCCESS;
}//getText
bool
zxmlNode::isComment(void) const
{
    if (_xmlInternalNode==nullptr)
            return false;
    return (_xmlInternalNode->type==XML_COMMENT_NODE);
}

bool
zxmlNode::isDocument(void) const
{
    if (_xmlInternalNode==nullptr)
            return false;
    return (_xmlInternalNode->type == XML_DOCUMENT_NODE);
}

bool
zxmlNode::isHtmlDocument(void) const
{
    if (_xmlInternalNode==nullptr)
            return false;
    return (_xmlInternalNode->type==XML_HTML_DOCUMENT_NODE);
}
//======================Node creation================================


/**
 * @brief zxmlElement::addChild add any node (element, attribute, comment, etc...) to current element
 * @param[in] pNode node to add as a child to current node
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlElement::addChild(zxmlNode* pNode)
{

    if (!pNode)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVVALUE,
                              Severity_Severe,
                              "Cannot add nullptr child to current node");
        return  ZS_INVVALUE;
        }
    if (!xmlAddChild(_xmlInternalNode,pNode->_xmlInternalNode))
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot add child to current node");
        return  ZS_XMLERROR;
        }
    return  ZS_SUCCESS;
}
/**
 * @brief zxmlElement::addChildList adds a tree of nodes (element, attribute, comment, etc...) to current element
 * @param[in] pNode node to add as a child to current node
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlElement::addChildList(zxmlNode* pNode)
{

    if (!pNode)
        {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVVALUE,
                              Severity_Severe,
                              "Cannot add nullptr child list to current node");
        return  ZS_INVVALUE;
        }
    if (!xmlAddChildList(_xmlInternalNode,pNode->_xmlInternalNode))
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot add child list  to current node");
        return  ZS_XMLERROR;
        }
    return  ZS_SUCCESS;
}
/**
 * @brief zxmlNode::setAttribute creates and sets the attribute with pName and pValue to current node and returns the created attribute object
 * @param[out] pAttribute attribute object returned
 * @param[in] pName  name of the attribute node to create
 * @param[in] pValue value for the attribute to create as a string
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlElement::newAttribute(zxmlAttribute* &pAttribute, const char* pName, const char*pValue, zxmlNameSpace *pNameSpace)
{

    pAttribute=nullptr;
    xmlNsPtr wNs=nullptr;
    if (pNameSpace)
            wNs=pNameSpace->_xmlInternalNameSpace;
    xmlAttrPtr wAttr=xmlNewNsProp(_xmlInternalNode,wNs,( const xmlChar *)pName, (const xmlChar *)pValue);
    if (!wAttr)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot add create attribute name <%s> to current element <%s>",pName,getName().toCChar());
        return  ZS_XMLERROR;
        }
//    pAttribute=new zxmlAttribute(wAttr);
    pAttribute=zxmlcreateAttribute(wAttr);
    return  ZS_SUCCESS;
}
/**
 * @brief zxmlElement::setAttribute set or reset an attribute to the given value
 * @param[out] pAttribute a pointer to the zxmlAttribute object that has been created or nullptr in case of failure
 * @param[in] pName     name of the newly attribute to create and set the node with
 * @param[in] pValue a C string for attribute value
 * @param[in] pNameSpace pointer to a zxmlNameSpace object to set the attribute with
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlElement::setAttribute(zxmlAttribute* pAttribute,const char*pName,const char*pValue,zxmlNameSpace* pNameSpace)
{

    xmlNsPtr wNs=nullptr;
    if (pNameSpace)
            wNs=pNameSpace->_xmlInternalNameSpace;
    xmlAttrPtr wAttr=xmlSetNsProp(_xmlInternalNode,wNs,(const xmlChar *)pName,(const xmlChar *)pValue);
    if (!wAttr)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot add set attribute name <%s> to current element <%s>",pName,getName().toCChar());
        return  ZS_XMLERROR;
        }

//    pAttribute=new zxmlAttribute(wAttr);
    pAttribute=zxmlcreateAttribute(wAttr);
    return  ZS_SUCCESS;
}
/**
 * @brief zxmlElement::setNameSpace sets pNameSpace namespace object to current Element
 * @param[in] pNameSpace a pointer to a zxmlNameSpace object
 * @return  ZS_SUCCESS
 */
ZStatus
zxmlElement::setNameSpace(zxmlNameSpace* pNameSpace)
{

    xmlSetNs(_xmlInternalNode,pNameSpace->_xmlInternalNameSpace);
    return  ZS_SUCCESS;
}

//=======================Element======================================

/**
 * @brief zxmlElement::zxmlElement will create a node of type XML_ELEMENT capable of having chilren, attributes, etc...
 * @param[in] pName  Name of the element to create
 * @param[in] pNameSpace  optional namespace to stick the node on
 */
zxmlElement::zxmlElement(const utf8VaryingString &pName,zxmlNameSpace*pNameSpace)
{


    xmlNsPtr wNs=nullptr;
    if (pNameSpace)
            wNs=pNameSpace->_xmlInternalNameSpace;
   _xmlInternalNode=xmlNewNode(wNs, (xmlChar *) pName.toCChar());
   if (!_xmlInternalNode)
    {
    setXMLZException(_GET_FUNCTION_NAME_,
                     ZS_XMLERROR,
                     Severity_Fatal,
                     "Fatal xml error while creating element");
    ZException.exit_abort();
    }

   return ;
}



ZStatus
zxmlElement::getFirstChildElement(zxmlElement* &pElement)
{
    pElement=nullptr;
    xmlNodePtr wChildElement=nullptr;
    wChildElement=xmlFirstElementChild(_xmlInternalNode);
    if (wChildElement==nullptr)
                        return (ZS_EMPTY);

    pElement=(zxmlElement*)zxmlcreateNode(wChildElement);
//    pElement=new zxmlElement((xmlElementPtr)wChildElement);
    return ZS_SUCCESS;
}// getFirstChildElement


ZStatus
zxmlElement::getLastChildElement(zxmlElement *&pElement)
{
    pElement=nullptr;
    xmlNodePtr wChild=nullptr;
    wChild=xmlLastElementChild((xmlNodePtr)_xmlInternalNode);

    if (wChild==nullptr)
                        return (ZS_EMPTY);

    pElement=(zxmlElement*)zxmlcreateNode(wChild);

//    pElement=new zxmlElement((xmlElementPtr)wChild);
    return ZS_SUCCESS;
}// getLastChildElement


ZStatus
zxmlElement::getNextElementSibling(zxmlElement *&pNextElement)
{
    xmlNodePtr wChild=nullptr;
    pNextElement=nullptr;
    wChild=xmlNextElementSibling(_xmlInternalNode);
    if (wChild==nullptr)
                        return (ZS_EOF);

//    pNextElement=new zxmlElement((xmlElementPtr)wChild);
    pNextElement=(zxmlElement*)zxmlcreateNode(wChild);
    return ZS_SUCCESS;
}// getNextElementSibling

ZStatus
zxmlElement::getPreviousElementSibling(zxmlElement *&pElement)
{
    xmlNodePtr wChild=nullptr;
    pElement=nullptr;

    wChild=xmlPreviousElementSibling((xmlNodePtr)_xmlInternalNode);

    if ((wChild==nullptr))
                        return (ZS_OUTBOUNDHIGH);

//    pElement=new zxmlElement((xmlElementPtr)wChild);
    pElement=(zxmlElement*)zxmlcreateNode(wChild);
    return ZS_SUCCESS;
}// getPreviousElementSibling
/**
 * @brief zxmlElement::getAttributeValue for current element gets the value in pValue of attribute named pAttrName
 * @param pAttrName
 * @param pValue
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_NOTFOUND in no attribute of this name has been found
 */
ZStatus
zxmlElement::getAttributeValue(const utf8_t *pAttrName, utf8String &pValue)
{
    pValue.clear();
    if (_xmlInternalNode==nullptr)
                            return ZS_XMLERROR;
xmlChar* wValue;
    wValue=xmlGetProp(_xmlInternalNode,(const xmlChar*)pAttrName);
    if (wValue==nullptr)
            return ZS_NOTFOUND;
    pValue=(const utf8_t*)wValue;
    free (wValue);
    return ZS_SUCCESS;
}
/**
 * @brief zxmlElement::getFirstCData for current node, gets the fist child with CData Section.
 *          Returns a pointer to CData Section node and CData (raw) content.
 * @param[out] pCDataNode pointer to CData Section node if found (nullptr if not found)
 * @param[out] pCData CData raw content
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlElement::getFirstCData(zxmlNode* &pCDataNode,utf8String& pCData)
    {
    ZStatus wSt=getFirstChildCData(pCDataNode);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return pCDataNode->getNodeText(pCData);
    }
/**
 * @brief zxmlElement::getFirstText for current node, gets the fist text child.
 *          Returns a pointer to text node and Text content. Entities may be substituted.
 *
 *  Remark: to get the next sibling node, you have to use <resulting node ptr>->getNextxxx()
 *
 * @param[out] pTextNode pointer to text node if found (nullptr if not found)
 * @param[out] pText Text content
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlElement::getFirstText(zxmlNode* &pTextNode,utf8String& pText)
    {
    ZStatus wSt=getFirstChildText(pTextNode);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return pTextNode->getNodeText(pText);
    }
/**
 * @brief zxmlElement::getFirstComment for current node, gets the fist text child.
 *          Returns a pointer to comment node and raw comment text content.
 *  Remark: to get the next sibling node, you have to use <resulting node ptr>->getNextxxx()
 *
 * @param[out] pTextNode pointer to comment node if found (nullptr if not found)
 * @param[out] pText Raw node text content
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlElement::getFirstComment(zxmlNode* &pTextNode,utf8String& pText)
    {
    ZStatus wSt=getFirstChildComment(pTextNode);
    if (wSt!=ZS_SUCCESS)
                return wSt;
    return pTextNode->getNodeRaw(pText);
    }

//===============Comment Node=======================================


//===============CData Node=======================================

//================Create =========================================
/**
 * @brief zxmlElement::newElementTextChild creates a child element to current element, with a Text node as child
 * @param[out] pNode
 * @param[in] pNodeName
 * @param[in] pText
 * @param[in] pNameSpace
 * @return
 */
ZStatus
zxmlElement::newElementTextChild(zxmlElement* &pNode, const char* pNodeName, const char *pText, zxmlNameSpace*pNameSpace)
{

    xmlNsPtr wNs=nullptr;
    if (pNameSpace)
            wNs=pNameSpace->_xmlInternalNameSpace;

    xmlNodePtr wNode=xmlNewTextChild(_xmlInternalNode,wNs,(const xmlChar*)pNodeName,(const xmlChar*)pText);
    if (!wNode)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot add element text child to current node");
        return  ZS_XMLERROR;
        }

    pNode = (zxmlElement*)zxmlcreateNode(wNode);
    return  ZS_SUCCESS;
}

ZStatus
zxmlElement::newElementTextsprintf(zxmlElement* &pNode, const char *pNodeName, zxmlNameSpace*pNameSpace, const char *pFormat,...)
{

utfdescString wString;
    va_list args;
    va_start (args, pFormat);
    wString.sprintf (pFormat, args);
    va_end(args);
    xmlNsPtr wNs=nullptr;
    if (pNameSpace)
            wNs=pNameSpace->_xmlInternalNameSpace;
    xmlNodePtr wNode=xmlNewTextChild(_xmlInternalNode,wNs,(const xmlChar*)pNodeName,(const xmlChar*)wString.content);
    if (!wNode)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot add element text child to current node");
        return  ZS_XMLERROR;
        }

    pNode = (zxmlElement*)zxmlcreateNode(wNode);
    return  ZS_SUCCESS;
}

ZStatus
zxmlElement::newElementLongTextChild(zxmlElement* &pNode, const char *pNodeName, utf8VaryingString &pText, zxmlNameSpace*pNameSpace)
{

    xmlNsPtr wNs=nullptr;
    if (pNameSpace)
            wNs=pNameSpace->_xmlInternalNameSpace;

    xmlNodePtr wNode=xmlNewTextChild(_xmlInternalNode,wNs,(const xmlChar*)pNodeName,(const xmlChar*)pText.toUtf());
    if (!wNode)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot add element text child to current node");
        return  ZS_XMLERROR;
        }

    pNode = (zxmlElement*)zxmlcreateNode(wNode);
    return  ZS_SUCCESS;
}
/**
 * @brief zxmlElement::newComment adds a new comment node child for current element
 * @param[out] pNode a pointer to newly created comment node
 * @param[in] pText comment text to create comment with
 * @param[in] pText Text content to fill the new node with
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlElement::newComment(zxmlNode* &pNode, const char *pText)
{


    xmlNodePtr wNode=xmlNewComment((const xmlChar*)pText);
    if (!wNode)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot create comment node");
        return  ZS_XMLERROR;
        }

    if (!xmlAddChild(_xmlInternalNode,wNode))
        {
        xmlFreeNode(wNode);
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "Cannot add comment node to Element named <%s>",(const char*)_xmlInternalNode->name);
        xmlFreeNode(wNode);
        return  ZS_XMLERROR;
        }
    pNode=zxmlcreateNode(wNode);
    return  ZS_SUCCESS;
}
/**
 * @brief zxmlElement::newText adds a new text node child for current element
 * @param[out] pNode created text node
 * @param[in] pText Text content to fill the new node with
 * @return a ZStatus ZS_SUCCESS in case of successful operation.
 *                   ZS_XMLERROR if any xml error raised. In this case, ZException is set with appropriate message.
 *                   ZException complement is set with xml internal error if any available.
 */
ZStatus
zxmlElement::newText(zxmlNode* &pNode, utf8VaryingString &pText)
{

    xmlNodePtr wNode=xmlNewText((const xmlChar*)pText.toUtf());
    if (!wNode)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot create text node");
        return  ZS_XMLERROR;
        }

    if (!xmlAddChild(_xmlInternalNode,wNode))
        {
        xmlFreeNode(wNode);
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "Cannot add text node to Element named <%s>",(const char*)_xmlInternalNode->name);
        xmlFreeNode(wNode);
        return  ZS_XMLERROR;
        }

    pNode=zxmlcreateNode(wNode);
    return  ZS_SUCCESS;
}
/**
 * @brief zxmlElement::newCData creates a new CData Section for current element and returns its zxmlNode object pointer
 * @param[out] pNode    pointer to a zxmlNode containing the CData Section
 * @param[in] pCData a ZDataBuffer containing CData content. Content will not be parsed.
 * @return a ZStatus:ZS_SUCCESS for successfull creation,
 *                   ZS_XMLERROR in case of error- ZException will be set with appropriate xml information if any available.
 */
ZStatus
zxmlElement::newCData(zxmlNode* &pNode,ZDataBuffer &pCData)
{

    xmlNodePtr wNode=xmlNewCDataBlock(_xmlInternalNode->doc,(const xmlChar*)pCData.DataChar,(int)pCData.Size);
    if (!wNode)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot create CData section node");
        return  ZS_XMLERROR;
        }

    if (!xmlAddChild(_xmlInternalNode,wNode))
        {
        xmlFreeNode(wNode);
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "Cannot add CData section node to Element named <%s>",(const char*)_xmlInternalNode->name);
        return ZS_XMLERROR;
        }
    pNode=zxmlcreateNode(wNode);
    return  ZS_SUCCESS;
}//newCData


ZStatus
zxmlElement::newCDataElement(zxmlElement* &pElement, const char *pName, ZDataBuffer &pCData, zxmlNameSpace* pNameSpace)
{


    xmlNsPtr wNs=nullptr;
    if (pNameSpace)
            wNs=pNameSpace->_xmlInternalNameSpace;



    xmlNodePtr wElement=xmlNewNode(wNs,(const xmlChar*)pName);
    if (!wElement)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot create element name <%s>",pName);
        return  ZS_XMLERROR;
        }
    xmlNodePtr wNode=xmlNewCDataBlock(_xmlInternalNode->doc,(const xmlChar*)pCData.DataChar,(int)pCData.Size);
    if (!wNode)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "cannot create CData section node");
        return  ZS_XMLERROR;
        }

    pElement=(zxmlElement*) zxmlcreateNode(wElement);

    if (!xmlAddChild(pElement->_xmlInternalNode,wNode))
        {

        xmlFreeNode(wNode);
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "Cannot add CData section node to Element named <%s>",(const char*)pElement->getName().toCChar());


        return ZS_XMLERROR;
        }
    if (!xmlAddChildList(_xmlInternalNode,pElement->_xmlInternalNode))
        {
        xmlFreeNode(wNode);
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "Cannot add element CData section <%s> to Element named <%s>",
                              (const char*)pElement->getName().toCChar(),
                              getName().toCChar());
        zxmlremoveNode((zxmlNode*)pElement);
        return ZS_XMLERROR;
        }

//    zxmlcreateNode(wNode);
    return  ZS_SUCCESS;
}//newCData

/**
 * @brief zxmlElement::setName sets or resets name of an element (must be element and not single node)
 * @param pName
 * @return
 */
void
zxmlElement::setName(const char* pName)
{
    xmlNodeSetName(_xmlInternalNode,(const xmlChar *)pName);
}

/**
 * @brief zxmlElement::newElementNameSpace creates a local namespace for the current element.
 *      returns a namespace object that holds the namespace.
 *
 *  for a default namespace pPrefix should be nullptr
 *
 * @param[out] pNameSpace created object
 * @param[in] pURL      url for namespace
 * @param[in] pPrefix   prefix for namespace
 * @return a ZStatus ZS_XMLERROR and ZException set with message if an error occurs, ZS_SUCCESS if anything went good
 */
ZStatus
zxmlElement::newNameSpace(zxmlNameSpace* &pNameSpace, const char *pURL, const char *pPrefix)
{

    xmlNsPtr wNs=nullptr;
    wNs=xmlNewNs(_xmlInternalNode,(const xmlChar *)pURL,(const xmlChar *)pPrefix);
    if (!wNs)
        {
        setXMLZException(_GET_FUNCTION_NAME_,
                         ZS_XMLERROR,
                         Severity_Error,
                         "Cannot create local XML namespace for DOM element named <%s> url <%s> prefix <%s>",
                         getName().toCChar(),
                         pURL,
                         pPrefix);


        return  ZS_XMLERROR;
        }

    pNameSpace=zxmlcreateNameSpace(wNs);

    return  ZS_SUCCESS;
}// newElementNameSpace
/**
 * @brief zxmlElement::dumpCurrent prints current element's immediate content, text(s) nodes, comment(s), CData section(s)
 * @param pOutput       stdout or other open file as <FILE*>
 * @param pLevel        Current node's path level
 * @param pSpaceIndent  Number of spaces to add at each level
 */
void
zxmlElement::dumpCurrent(FILE*pOutput,int pLevel,int pSpaceIndent)
{

ZStatus wSt;
char wSpace[500];

zxmlNode*wNode,*wNode2=nullptr;
zxmlAttribute* wAttribute;

utf8VaryingString wNodeContent;
utf8VaryingString wNodeText;

    memset (wSpace,' ',sizeof(wSpace));
    wSpace[pLevel*pSpaceIndent]='\0';

    fprintf (pOutput,"%s>Element name <%s> line <%d> child elements <%ld> ",
             wSpace,
             getName().toCChar(),
             getLine(),
             getChildElementCount()
             );
    fprintf (pOutput," %s%s%s%s%s ",
//             wSpace,
             hasElement()?"Has Element(s) ":"",
             hasText()?"Has Text(s) ":"",
             hasComment()?"Has comment(s) ":"",
             hasAttribute()?"Has attribute(s) ":"",
             hasCData()?"Has CData section(s) ":"");

    fprintf (pOutput," Path: <%s>\n",
             getNodePath().toCChar());

    if (hasText())
        {
        wSt=getFirstText(wNode,wNodeText);
        while ((wSt==ZS_SUCCESS)&&(wNode))
            {
            fprintf(pOutput,
                    "%s Text <%s>\n",
                    wSpace,
                    wNodeText.toCChar());
            wSt=wNode->getNextText(wNode2,wNodeText);
            wNode=wNode2;
            }
        }
    if (hasComment())
        {
        wSt=getFirstChildComment(wNode);
        while ((wSt==ZS_SUCCESS)&&(wNode))
            {
            wSt=wNode->getNodeRaw(wNodeContent);
            if (wSt==ZS_SUCCESS)
                    fprintf(pOutput,"%s Comment line <%d> <%s>\n",
                            wSpace,
                            wNode->getLine(),
                            wNodeContent.toCString_Strait());
                else
                    fprintf(pOutput,"%s cannot get comment content\n",
                            wSpace);
            wSt=wNode->getNextCommentSibling(wNode2);
            wNode=wNode2;
            }
        }
    if (hasAttribute())
        {
        wSt=getFirstAttribute(wAttribute);
        while ((wSt==ZS_SUCCESS)&&(wAttribute))
            {
                    fprintf(pOutput,"%s Attribute <%s> : <%s>\n",
                            wSpace,
                            wAttribute->getName().toCChar(),
                            wAttribute->getValue().toString());
            wSt=wAttribute->getNextAttributeSibling(wAttribute);
            }
        }
    if (hasCData())
        {
        wSt=getFirstCData(wNode,wNodeContent);
        while ((wSt==ZS_SUCCESS)&&(wNode))
            {
            fprintf(pOutput,
                    "%s CData content <%s>\n",
                    wSpace,
                    wNodeContent.toCString_Strait());

            wSt=wNode->getNextCData(wNode2,wNodeContent);
            wNode=wNode2;
            }
        }

    return ;

}// dumpCurrent
/**
 * @brief zxmlElement::cascadeDump  prints recursively elements & child nodes since current element to bootom of DOM tree
 * @param pOutput       stdout or other open file as <FILE*>
 * @param pLevel        Current node's path level
 * @param pSpaceIndent  Number of spaces to add at each level
 */
void
zxmlElement::cascadeDump(FILE*pOutput,int pLevel,int pSpaceIndent)
{


ZStatus wSt;
char wSpace[500];
zxmlElement* wElement=nullptr;
    memset (wSpace,' ',sizeof(wSpace));
    wSpace[pLevel*pSpaceIndent]='\0';

    wSt=getFirstChildElement(wElement);
    while ((wElement)&&(wSt==ZS_SUCCESS))
        {
        if (!wElement)
            {
            fprintf(pOutput,"%s>******Nothing to print - null element*********\n",
                    wSpace);
            }
        else
            {
            wElement->dumpCurrent(pOutput,pLevel,pSpaceIndent);

            wElement->cascadeDump(pOutput,pLevel+1,pSpaceIndent);
            }
        wSt=wElement->getNextElementSibling (wElement);
        }
    return ;
}


#endif// ZXMLNODE_CPP
