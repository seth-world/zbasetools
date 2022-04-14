#ifndef ZXMLATTRIBUTE_H
#define ZXMLATTRIBUTE_H
#include <libxml/tree.h>
#include <zxml/zxmlnode.h>

class zxmlNameSpace;

class zxmlAttribute : private zxmlNode
{
private:
    zxmlAttribute(xmlAttrPtr pAttribute)  {_xmlInternalAttribute=pAttribute; _xmlInternalNode=(xmlNodePtr)pAttribute;}
public:
    friend class zxmlNode;
    friend class zxmlDoc;
    friend zxmlAttribute* zxmlcreateAttribute(xmlAttrPtr pxmlNode);
    friend zxmlAttribute* zxmlsearchAttributeByInternal(xmlAttrPtr pInternal);
    friend void zxmlremoveAttribute(zxmlAttribute*);

    zxmlAttribute(zxmlNode& pNode,const char*pName,const char* pValue,zxmlNameSpace* pNameSpace=nullptr);
//    ~zxmlAttribute() {if (!_xmlInternalAttribute->doc)xmlFreeProp(_xmlInternalAttribute);}
    ~zxmlAttribute() {
      if (!_xmlInternalNode->doc) {
          xmlFreeProp((xmlAttrPtr)_xmlInternalNode);
          _xmlInternalNode=nullptr;
      }
    }

    zxmlAttribute(const zxmlAttribute&)=delete;
    zxmlAttribute& operator=(const zxmlAttribute&) = delete;  // no copy


    utffieldNameString getName(void);
    utfdescString getValue(void);
    ZStatus getNextAttributeSibling(zxmlAttribute *&pAttribute);
//    ZStatus getNextAttributeSibling(zxmlAttribute* pAttribute);

    ZStatus createNextAttributeSibling(zxmlAttribute* &pAttribute);

private:
 xmlAttrPtr _xmlInternalAttribute;
};

#endif // ZXMLATTRIBUTE_H
