#ifndef ZXMLNODE_H
#define ZXMLNODE_H

#include <ztoolset/zutfstrings.h>
#include <libxml/tree.h>
//#include <zxml/zxmldoc.h>
#include <ztoolset/zarray.h>

class zxmlNode;
class ZNodeCollection : public ZArray<zxmlNode*>
{
  ZNodeCollection& _copyFrom(ZNodeCollection& pIn)
  {
    reset();
    for (long wi=0;wi < pIn.count(); wi++)
      push(pIn[wi]);
    return *this;
  }
public:
  ZNodeCollection()=default;
  ZNodeCollection(ZNodeCollection& pIn) {_copyFrom(pIn);}
  ZNodeCollection(ZNodeCollection&& pIn) {_copyFrom(pIn);}
  ~ZNodeCollection();

  ZNodeCollection& operator = (ZNodeCollection& pIn) {return _copyFrom(pIn);}
  ZNodeCollection& operator = (ZNodeCollection&& pIn) {return _copyFrom(pIn);}
};

class zxmlAttribute;
class zxmlNameSpace;
class zxmlElement;

class ZElementCollection : public ZArray<zxmlElement*>
{
  ZElementCollection& _copyFrom(ZElementCollection& pIn);
  ZElementCollection& _copyFrom(ZNodeCollection& pIn);

public:
  ZElementCollection()=default;
  ZElementCollection(ZElementCollection& pIn) {_copyFrom(pIn);}
  ZElementCollection(ZElementCollection&& pIn) {_copyFrom(pIn);}
  ZElementCollection(ZNodeCollection& pIn) {_copyFrom(pIn);}
  ZElementCollection(ZNodeCollection&& pIn) {_copyFrom(pIn);}
  ~ZElementCollection();

  ZElementCollection& operator = (ZElementCollection& pIn) {return _copyFrom(pIn);}
  ZElementCollection& operator = (ZElementCollection&& pIn) {return _copyFrom(pIn);}
  ZElementCollection& operator = (ZNodeCollection& pIn) {return _copyFrom(pIn);}
  ZElementCollection& operator = (ZNodeCollection&& pIn) {return _copyFrom(pIn);}
};



class zxmlNode
{
protected:
    zxmlNode() {}
    zxmlNode(xmlNodePtr pNodePtr) {_xmlInternalNode=pNodePtr;}


    ~zxmlNode() {
      // free node resource only if not part of a document.
      // if part of a doc, will be freed by xmlFreeDoc
 /*     if ((_xmlInternalNode) && (!_xmlInternalNode->doc) && (_xmlInternalNode->line)) {
              xmlFreeNode(_xmlInternalNode);
              _xmlInternalNode=nullptr;

      }
*/
    }
public:
    friend class zxmlGlobal;
    friend class zxmlDoc;
    friend class zxmlAttribute;
    friend class zxmlNameSpace;
    friend class zxmlElement;
    friend zxmlNode* zxmlcreateNode(xmlNodePtr pxmlNode);
    friend zxmlElement* zxmlcreateElement(const char* pxmlNodeName);
    friend zxmlNode* zxmlsearchNodeByInternal(xmlNodePtr pxmlNode);
    friend void zxmlremoveNode(zxmlNode* pNode);

    zxmlNode(const zxmlNode&)=delete;
    zxmlNode& operator=(const zxmlNode&) = delete;  // no copy

    utf8VaryingString getName(void) const;
    int getLine(void) const;
    utf8VaryingString getNodePath(void) const;

    /**
     * @brief zxmlNode::getFirstChild gets the first node as child of current element
     * @param[out] pFirstNode a pointer to zxmlNode object for the found node (or nullptr if not found)
     * @return ZStatus ZS_SUCCESS if a node is found, ZS_EMPTY if not
     */
    ZStatus getFirstChild(zxmlNode* &pFirstNode) const;
    /**
     * @brief zxmlNode::getNextNode
     * @param[out] pNextNode a pointer to zxmlNode object for the found node (or nullptr if not found)
     * @return ZStatus ZS_SUCCESS if a node is found, ZS_EOF if no more nodes are available
     */
    ZStatus getNextNode(zxmlNode* &pNode);
    ZStatus getPreviousNode(zxmlNode* &pPrevNode);
    ZStatus getLastChild(zxmlNode* &pLastNode);

/**
 * @brief zxmlNode::getChildByName find the first child node of current node with name pName.
 * @param pNode returned node if found or nullptr if not found
 * @param pName
 * @return a ZStatus set to ZS_SUCCESS if found - ZS_NOTFOUND if no node with that name is found.
 */
    ZStatus getChildByName(zxmlNode* &pNode, const utf8VaryingString &pName);

    /** @brief getAllChildren() find all children nodes with name pName and returns ZNodeCollection,
     * i.e. a ZArray with node pointers */
    ZNodeCollection getAllChildren(const utf8VaryingString &pName);
//---------------Extract data from current node----------------------------
    /**
     * @brief getNodeContentextract content from current node whatever it is
     * @param pCData
     * @return
     */
    ZStatus getNodeContent(utf8VaryingString &pCData) const;
    /**
     * @brief getCData extracts CData content from current node
     * @param pCData
     * @return
     */
    ZStatus getCData(ZDataBuffer& pCData) const;
    /**
     * @brief getText extracts text from current node (comment or text from text node)
     * @param pText
     * @return
     */
    ZStatus getText(ZDataBuffer& pText) const;
    /**
     * @brief getText extracts text from current node (comment or text from text node)
     * @param pText
     * @return
     */
    ZStatus getText(utf8VaryingString& pText) const;

    ZStatus getNodeText(utf8VaryingString &pText);
    ZStatus getNodeRaw(utf8VaryingString &pText);

    ZStatus getFirstChildCData(zxmlNode* &pNode);            // CData Section is not element but node
    // gets the sibling CData node
    ZStatus getNextCDataSibling(zxmlNode* &pNode);
    // obtains the sibling CData node and its content
    ZStatus getNextCData(zxmlNode* &pCDataNode,utf8VaryingString&pCData)
        {
        ZStatus wSt=getNextCDataSibling(pCDataNode);
        if (wSt!=ZS_SUCCESS)
                    return wSt;
        return pCDataNode->getNodeContent(pCData);
        }


    ZStatus getFirstChildComment(zxmlNode* &pNode);          // Comment is not element but node
    ZStatus getNextCommentSibling(zxmlNode* &pNode);

    ZStatus getFirstAttribute(zxmlAttribute *&pAttribute);
//  for getNextAttributeSibling  see zxmlAttribute::getNextAttributeSibling

    ZStatus getFirstChildText(zxmlNode* &pNode);    // gets first child Text node
    ZStatus getNextTextSibling(zxmlNode* &pNode);   // gets the sibling text node (to current node)
    ZStatus getNextText(zxmlNode *pTextNode, utf8VaryingString &pText); // obtains the sibling Text node and its content



    bool isElement(void) const;
    bool isText(void) const ;
    bool isCData(void) const;
    bool isComment(void) const;
    bool isDocument(void) const;
    bool isHtmlDocument(void) const;

    bool hasElement(void) const;
    bool hasCData(void) const;
    bool hasText(void) const;
    bool hasAttribute(void) const;
    bool hasComment(void) const;

    unsigned long getChildElementCount(void);

    void setLanguage(const char*pLanguage);
    utffieldNameString getLanguage(void);

protected:
xmlNodePtr _xmlInternalNode=nullptr;
};



class zxmlElement : public zxmlNode
{
private:
    zxmlElement(xmlElementPtr pEltPtr):zxmlNode((xmlNodePtr) pEltPtr) {}
    zxmlElement(xmlNodePtr pEltPtr):zxmlNode(pEltPtr) {}
public:
    zxmlElement(const utf8VaryingString &pName, zxmlNameSpace*pNameSpace=nullptr);

    ~zxmlElement() {}

    friend zxmlElement* zxmlcreateElement(const utf8VaryingString &pxmlNodeName);

    void dumpCurrent(FILE*pOutput, int pLevel=0, int pSpaceIndent=4);  // prints current element and direct children
    void cascadeDump(FILE*pOutput, int pLevel=0, int pSpaceIndent=4);  // prints recursively tree node since current element

    ZStatus getFirstChildElement(zxmlElement* &pElement);
    ZStatus getNextElementSibling(zxmlElement* &pElement);
    ZStatus getPreviousElementSibling(zxmlElement* &pElement);
    ZStatus getLastChildElement(zxmlElement* &pElement);

    ZStatus getAttributeValue(const utf8_t* pAttrName, utf8VaryingString &pValue);
/*    ZStatus getFirstAttribute(zxmlAttribute* &pAttribute);
*/
    /** @brief zxmlElement::getFirstText for current node, gets the fist text child.
     *          Returns Text content. Entities may be substituted.
     */
    ZStatus getFirstText(zxmlNode* &pTextNode, utf8VaryingString &pOutText);
    ZStatus getFirstComment(zxmlNode* &pCommentNode, utf8VaryingString &pComment);
    ZStatus getFirstCData(zxmlNode* &pCDataNode, utf8VaryingString &pCData);

    // for next CData node see zxmlNode::getNextCData(zxmlNodeZDataBuffer& pCData);

//===============Elements/nodes creation===============================
// Only elements may have chilren


    void setName(const char* pName);
    ZStatus newElementTextChild(zxmlElement* &pNode, const char *pNodeName, const char *pText, zxmlNameSpace* pNameSpace=nullptr);
    ZStatus newElementTextsprintf(zxmlElement* &pNode, const char *pNodeName, zxmlNameSpace*pNameSpace, const char *pFormat,...);
    ZStatus newElementLongTextChild(zxmlElement* &pNode,const char* pNodeName,utf8VaryingString &pText,zxmlNameSpace* pNameSpace=nullptr);

    ZStatus newText(zxmlNode* &pNode,utf8VaryingString &pText);
    ZStatus newComment(zxmlNode* &pNode, const char *pText);
    ZStatus newCData(zxmlNode *&pNode, ZDataBuffer &pCData);

    ZStatus newCDataElement(zxmlElement* &pElement,const char* pName,ZDataBuffer &pCData,zxmlNameSpace* pNameSpace);

    ZStatus newAttribute(zxmlAttribute* &pAttribute, const char *pName, const char *pContent, zxmlNameSpace* pNameSpace=nullptr);
    ZStatus newNameSpace(zxmlNameSpace *&pNameSpace, const char*pURL, const char *pPrefix);

    ZStatus addChild(zxmlNode *pNode);
    ZStatus addChildList(zxmlNode* pNode);

    ZStatus setAttribute(zxmlAttribute* pAttribute,const char*pName,const char*pValue,zxmlNameSpace* pNameSpace=nullptr);
    ZStatus setNameSpace(zxmlNameSpace* pNameSpace);
    ZStatus unsetNameSpace(zxmlNameSpace* pNameSpace);

//xmlElementPtr _xmlInternalElement=nullptr;
}; // zxmlElement



#endif // ZXMLNODE_H
