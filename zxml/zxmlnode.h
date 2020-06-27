#ifndef ZXMLNODE_H
#define ZXMLNODE_H

#include <ztoolset/zutfstrings.h>
#include <libxml/tree.h>
//#include <zxml/zxmldoc.h>



class zxmlAttribute;
class zxmlNameSpace;

class zxmlNode
{
protected:
    zxmlNode() {}
    zxmlNode(xmlNodePtr pNodePtr) {_xmlInternalNode=pNodePtr;}


    ~zxmlNode() {
                // free node resource only if not part of a document.
                // if part of a doc, will be freed by xmlFreeDoc
                if (!_xmlInternalNode->doc)
                        xmlFreeNode(_xmlInternalNode);
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

    utffieldNameString getName(void);
    int getLine(void);
    utfdescString getNodePath(void);

    ZStatus getFirstChild(zxmlNode* &pFirstNode);
    ZStatus getNextNode(zxmlNode* &pNode);
    ZStatus getPreviousNode(zxmlNode* &pPrevNode);
    ZStatus getLastChild(zxmlNode* &pLastNode);

    ZStatus getChildByName(zxmlNode* &pNextNode,const char*pName);

//---------------Extract data from current node----------------------------
    ZStatus getNodeContent(utf8VaryingString &pCData);            // extract content from current node whatever it is
    ZStatus getCData(ZDataBuffer& pCData);                  // extracts CData content from current node

    ZStatus getText(ZDataBuffer& pText);  // extracts text from current node (comment or text from text node)

    ZStatus getText(utf8VaryingString& pText);

    ZStatus getNodeText(utf8VaryingString &pText);
    ZStatus getNodeRaw(utf8VaryingString &pText);

    ZStatus getFirstChildCData(zxmlNode* &pNode);            // CData Section is not element but node
    ZStatus getNextCDataSibling(zxmlNode* &pNode);           // gets the sibling CData node

    ZStatus getNextCData(zxmlNode* &pCDataNode,utf8VaryingString&pCData) // obtains the sibling CData node and its content
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



    bool isElement(void);
    bool isText(void);
    bool isCData(void);
    bool isComment(void);
    bool isDocument(void);
    bool isHtmlDocument(void);

    bool hasElement(void);
    bool hasCData(void);
    bool hasText(void);
    bool hasAttribute(void);
    bool hasComment(void);

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
    zxmlElement(const char*pName, zxmlNameSpace*pNameSpace=nullptr);


    ~zxmlElement() {}

    friend zxmlElement* zxmlcreateElement(const char* pxmlNodeName);

    void dumpCurrent(FILE*pOutput, int pLevel=0, int pSpaceIndent=4);  // prints current element and direct children
    void cascadeDump(FILE*pOutput, int pLevel=0, int pSpaceIndent=4);  // prints recursively tree node since current element

    ZStatus getFirstChildElement(zxmlElement* &pElement);
    ZStatus getNextElementSibling(zxmlElement* &pElement);
    ZStatus getPreviousElementSibling(zxmlElement* &pElement);
    ZStatus getLastChildElement(zxmlElement* &pElement);

    ZStatus getAttributeValue(const utf8_t* pAttrName, utfdescString &pValue);
/*    ZStatus getFirstAttribute(zxmlAttribute* &pAttribute);
*/
    ZStatus getFirstText(zxmlNode* &pTextNode, utf8VaryingString &pText);
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
