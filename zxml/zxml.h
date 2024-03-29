#ifndef ZXML_H
#define ZXML_H

#include <zxml/zxmldoc.h>
#include <zxml/zxmlnode.h>
#include <zxml/zxmlattribute.h>
#include <zxml/zxmlnamespace.h>

typedef zxmlNode*       zxmlNodePtr;
typedef zxmlElement*    zxmlElementPtr;
typedef zxmlDoc*        zxmlDocPtr;
typedef zxmlAttribute*  zxmlAttributePtr;
typedef zxmlNameSpace*  zxmlNameSpacePtr;

void XMLderegister(zxmlNode *&pNode);
void XMLderegister(zxmlElement *&pNode);

void XMLderegister(zxmlAttribute *&pNode);
void XMLderegister(zxmlNameSpace *&pNode);
void XMLderegister(zxmlDoc *&Doc);


#endif // ZXML_H
