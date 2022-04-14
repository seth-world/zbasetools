#ifndef ZXMLDOC_H
#define ZXMLDOC_H
//#include <libxml++/document.h>
#include <ztoolset/zutfstrings.h>
#include <ztoolset/uristring.h>

//#include <libxml++/libxml++.h>

#include <libxml/parser.h>

#include <libxml/tree.h>
class zxmlNode;
class zxmlAttribute;
class zxmlNameSpace;
class zxmlElement;

zxmlNode* zxmlcreateNode(xmlNodePtr pxmlNode);
zxmlElement* zxmlcreateElement(const utf8VaryingString &pxmlNodeName);
zxmlAttribute* zxmlcreateAttribute(xmlAttrPtr pxmlNode);
zxmlNameSpace* zxmlcreateNameSpace(xmlNsPtr pxmlNode);

void zxmlremoveNode(zxmlNode* pNode);
void zxmlremoveAttribute(zxmlAttribute* pNode);
void zxmlremoveNameSpace(zxmlNameSpace* pNode);

zxmlNode* zxmlsearchNodeByInternal(xmlNodePtr pInternal);
zxmlAttribute* zxmlsearchAttributeByInternal(xmlAttrPtr pInternal);
zxmlNameSpace* zxmlsearchNameSpaceByInternal(xmlNsPtr pInternal);


class zxmlParser
{
public:
    zxmlParser() ;
    ~zxmlParser() {if (Context)
                        xmlFreeParserCtxt(Context);
                  }

    void clear(void) {if (Context)
                            xmlClearParserCtxt(Context);
                     }
    void free(void) {if (Context)
                        xmlFreeParserCtxt(Context);
                     Context=nullptr;
                    }


uriString DocFile;

xmlParserCtxtPtr Context=nullptr;
};




class zxmlNode;
class zxmlElement;
class zxmlAttribute;
class zxmlNameSpace;

class zxmlDoc
{
public:
    zxmlDoc(const char* pVersion);  // for Dom document creation
    zxmlDoc() ;                     // for Dom document parsing

    zxmlDoc(const zxmlDoc&)=delete;
    zxmlDoc& operator=(const zxmlDoc&) = delete;  // no copy

    ~zxmlDoc() ;

    friend class zxmlNameSpace;
    friend class zxmlNode;
    friend class zxmlAttribute;

//========================Document parsing======================================

    void DocDump (FILE* pOutput,int pFormat=1) {xmlDocFormatDump(pOutput,_xmlInternalDoc,pFormat); return;}

    /**
     * @brief logicalDump lists current's xml document content in a structured way
     * @param pOutput
     * @param pSpaceIndent
     * @return
     */
    ZStatus logicalDump(FILE*pOutput,int pSpaceIndent=4);

    void setURI (const char* pFileName) {URIDocFile=(const utf8_t*)pFileName;}

    /** Nota bene :
     * encoding parameter is defined by a string "utf8","utf16",...
     *  if omitted (nullptr) then utf8 default encoding is used.
     * parsing option is defined by xmlParserOption from libxml2.
     *  by default, when no parsing option (0), then XML_PARSE_NOBLANKS is used (remove blank nodes).
     */

    /**
     * @brief loadAndParseXMLDoc loads xml document pURL and parses it according its encoding pEncoding
     * and options pOptions . for parsing options see xmlParserOption
     * @return  a ZStatus. ZException is set appropriately in case of error.
     */
    ZStatus loadAndParseXMLDoc(const char* pURL,const char* pEncoding,int pOptions);
    /**
     * @brief ParseXMLDocFromMemory Parses an xml document loaded within an utf8 string according its encoding pEncoding
     * and options pOptions . for parsing options see xmlParserOption
     * @return  a ZStatus. ZException is set appropriately in case of error.
     */
    ZStatus ParseXMLDocFromMemory(const char* pMemory,int pSize,const char* pEncoding,int pOptions);

    ZStatus parseXMLFile (const char*pFilename);

    ZStatus parseXMLLoadedDoc(void) ;

    ZStatus getNamedRootElement(const char*pRootNodeName, zxmlElement *&pRootElement);
    ZStatus getRootElement(zxmlElement *&pRootElement);

    ZStatus searchNameSpaceByURL (zxmlNameSpace*pNameSpace, const char* pURL,zxmlElement* &pElement);
    ZStatus searchNameSpaceByHref (zxmlNameSpace*pNameSpace, const char* pRef,zxmlElement* &pElement);

    ZStatus getDocComment (utf8VaryingString &pNodeComment);
    ZStatus getDocAttribute (const char* pName,utf8VaryingString &pNodeAttribute);
    utffieldNameString getDocVersion(void);

    utffieldNameString getName(void) ;
    ZStatus getDocEncoding(utffieldNameString &pEncoding) ;
    ZStatus getDocEncoding(xmlCharEncoding &pEncodingCode);
    int     getDocCharset(void) ;

    bool isHtmlDocument(void);
    bool isWellFormed(void) ;


//============================Document creation====================================

    ZStatus createDomDocument(const char* pVersion) ;
    ZStatus createBlankRootElement(zxmlElement* &pNode, const char *pRootNodeName, zxmlNameSpace *pNameSpace);

    ZStatus createRootElementfromTree(zxmlElement* &pNode);
    ZStatus replaceRootElementfromTree(zxmlElement* &pNode);

    ZStatus newCDataSection(zxmlNode* pNode, ZDataBuffer& pCDataContent);
    ZStatus newComment(zxmlNode* pCommentNode, utf8VaryingString &pCommentContent);

    ZStatus newAttribute(zxmlAttribute* pAttribute,const char*pName,const char*pValue);
    ZStatus newDocumentNameSpace(zxmlNameSpace* pNameSpace,const char* pURL,const char* pPrefix);

    ZStatus writeDOMDocument(const char* pFilename, bool pFormat);
    ZStatus writeDOMDocumentEncoded (const char* pFilename, const char*pEncoding, bool pFormat);

    ZStatus newTextElement(zxmlElement *pTextNode, const char *pNodeName, utf8VaryingString& pContent, zxmlNameSpace* pNameSpace);


utffieldNameString Version;

ZDataBuffer xmlContent;
uriString URIDocFile;
xmlDocPtr _xmlInternalDoc=nullptr;
zxmlParser* DocParser=nullptr;
zxmlElement* RootElement=nullptr;
};


#endif // ZXMLDOC_H
