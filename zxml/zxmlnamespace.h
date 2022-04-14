#ifndef ZXMLNAMESPACE_H
#define ZXMLNAMESPACE_H

#include <libxml/tree.h>
#include <ztoolset/zutfstrings.h>

class zxmlDoc;
class zxmlNode;

class zxmlNameSpace
{
private:
    zxmlNameSpace(xmlNsPtr pInternalNS) {_xmlInternalNameSpace=pInternalNS;}
public:
    friend class zxmlDoc;
    friend class zxmlNode;
    friend zxmlNameSpace* zxmlcreateNameSpace(xmlNsPtr pxmlNode);
    friend zxmlNameSpace* zxmlseachNameSpaceByInternal(xmlNsPtr pxmlNode);
    friend void zxmlremoveNameSpace(zxmlNameSpace* pNode);

    zxmlNameSpace(zxmlDoc* pDoc,const char* pURL,const char* pPrefix);      // global ns
    zxmlNameSpace(zxmlNode* pNode, const char* pURL, const char* pPrefix);    // local ns

    ~zxmlNameSpace() {
      if (_xmlInternalNameSpace) {
        xmlFreeNs(_xmlInternalNameSpace);
        _xmlInternalNameSpace=nullptr;
      }
    }

    zxmlNameSpace(const zxmlNameSpace&)=delete;
    zxmlNameSpace& operator=(const zxmlNameSpace&) = delete;  // no copy

    utffieldNameString getPrefix(void) {return utffieldNameString((const utf8_t*)_xmlInternalNameSpace->prefix);}
    utfdescString getURL(void) {return utfdescString((const utf8_t*)_xmlInternalNameSpace->href);}

   xmlNsPtr _xmlInternalNameSpace=nullptr;
};

#endif // ZXMLNAMESPACE_H
