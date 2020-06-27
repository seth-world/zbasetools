#ifndef QXMLUTILITIES_H
#define QXMLUTILITIES_H


#include <QDomDocument>

#include <zcrypt/zcrypt.h>
//#include <ztoolset/ztypes.h>
//#include <ztoolset/zauthorization.h>
#include <ztoolset/zbasedatatypes.h>
#include <ztoolset/zutfstrings.h>
#include <ztoolset/zdate.h>
struct checkSum;

QDomElement _appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,char *pStringValue);

QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,char *pStringValue);
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,utfcodeString pCodeValue);
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,utfdescString pDescValue);
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, utfidentityString &pDescValue);
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,ZDate &pDateValue);
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, ZDateFull &pDateFullValue);
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, const ZDateFull &pDateFullValue);
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,int pintValue);
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,long plongValue);
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,long long plonglongValue);
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,bool pboolValue);
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, float pfloatValue);
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, double pdoubleValue);
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, uriString pValue);
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, checkSum pValue);
#ifdef ZAUTHORIZATION_H
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, Id_struct &pValue);
void getElement (Id_struct &pField ,QDomElement &pElement);
#endif
void getElement (utfdescString &pField , QDomElement &pElement);
void getElement (utfcodeString &pField ,QDomElement &pElement);
void getElement (checkSum &pField ,QDomElement &pElement);
void getElement (char *pField ,QDomElement &pElement);
void getElement (float &pField ,QDomElement &pElement);
void getElement (double &pField ,QDomElement &pElement);
void getElement (bool &pField ,QDomElement &pElement);
void getElement (ZDate &pField , QDomElement &pElement);
void getElement (ZDateFull &pField , QDomElement &pElement);
void getElement (int &pField , QDomElement &pElement);
void getElement (uriString &pField ,QDomElement &pElement);
void getElement (Docid_struct &pField ,QDomElement &pElement);
void getElement (utfidentityString &pField ,QDomElement &pElement);
void getElementLong (long &pField ,QDomElement &pElement);
void getElementULong (unsigned long &pField ,QDomElement &pElement);
void getElementSize_t (size_t &pField ,QDomElement &pElement);
void getElementLongLong (long long &pField ,QDomElement &pElement);
void getElementUInt64_t (uint64_t &pField ,QDomElement &pElement);
void getElementUInt32_t (uint32_t &pField ,QDomElement &pElement);

QDomElement appendWidget (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pWindowName, QWidget *pWidget);


//-----------------Routines--------------------------------------
ZStatus
getChildElementValue(QDomNode pNodeWork, const char*pTagName, QString &pContent, bool pMandatory=true);
ZStatus
getChildElementValue(QDomNode pNodeWork, const utf8_t*pTagName, QString &pContent, bool pMandatory=true);

#endif // QXMLUTILITIES_H
