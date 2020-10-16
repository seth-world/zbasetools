#ifndef QXMLUTILITIES_CPP
#define QXMLUTILITIES_CPP

#include <iostream>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <zconfig.h>
#include <ztoolset/zexceptionmin.h>
//#include <ztoolset/zflowutility.h>


#include <zxml/qxmlutilities.h>


QDomElement _appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,char *pStringValue)
{
    QDomElement NewElement=pXmlDoc.createElement(pName);
    QDomText NameT=pXmlDoc.createTextNode(pStringValue);
    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}

QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,char *pStringValue)
{
    QDomElement NewElement=pXmlDoc.createElement(pName);
    QDomText NameT=pXmlDoc.createTextNode(pStringValue);
    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,utfdescString pDescValue)
{
    QDomElement NewElement=pXmlDoc.createElement(pName);
    QDomText NameT=pXmlDoc.createTextNode(pDescValue.toCString_Strait());
    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,utfidentityString &pDescValue)
{
    QDomElement NewElement=pXmlDoc.createElement(pName);
    QDomText NameT=pXmlDoc.createTextNode(pDescValue.toCString_Strait());
    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, utfcodeString pCodeValue)
{
    QDomElement NewElement=pXmlDoc.createElement(pName);
    QDomText NameT=pXmlDoc.createTextNode(pCodeValue.toCString_Strait());
    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}
/* this concerns applications therefore must be migrated to application
#ifdef ZAUTHORIZATION_H
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, Id_struct &pValue)
{
    QDomElement NewElement=pXmlDoc.createElement(pName);
    QDomText NameT=pXmlDoc.createTextNode(pValue.tocodeString().toCString_Strait());
    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}

void getElement (Id_struct &pField ,QDomElement &pElement)
{
    pField.fromString( pElement.text().toStdString().c_str(),16) ;  //! get it from hexadecimal
return;
}
#endif
*/
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,ZDate &pDateValue)
{

    QDomElement NewElement=pXmlDoc.createElement(pName);
/*    QDomText NameT=pXmlDoc.createTextNode(Qsprintf("%04d%02d%02d",
                                                   pDateValue.Year,
                                                   pDateValue.Month,
                                                   pDateValue.Day));*/
    utfdescString wStr;
    wStr.sprintf("%04d%02d%02d",
                 pDateValue.Year,
                 pDateValue.Month,
                 pDateValue.Day);

    QDomText NameT=pXmlDoc.createTextNode(wStr.toQString());

    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}

QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,ZDateFull &pDateFullValue)
{
    QDomElement NewElement=pXmlDoc.createElement(pName);
/*    QDomText NameT=pXmlDoc.createTextNode(Qsprintf("%04d%02d%02d%02d%02d%02d",
                                                   pDateFullValue.Year,
                                                   pDateFullValue.Month,
                                                   pDateFullValue.Day,
                                                   pDateFullValue.Hour,
                                                   pDateFullValue.Min,
                                                   pDateFullValue.Sec));*/

    utfdescString wStr;
    wStr.sprintf("%04d%02d%02d%02d%02d%02d",
                 pDateFullValue.Year,
                 pDateFullValue.Month,
                 pDateFullValue.Day,
                 pDateFullValue.Hour,
                 pDateFullValue.Min,
                 pDateFullValue.Sec);

    QDomText NameT=pXmlDoc.createTextNode(wStr.toQString());
    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}

QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,const ZDateFull &pDateFullValue)
{
    QDomElement NewElement=pXmlDoc.createElement(pName);
/*    QDomText NameT=pXmlDoc.createTextNode(Qsprintf("%04d%02d%02d%02d%02d%02d",
                                                   pDateFullValue.Year,
                                                   pDateFullValue.Month,
                                                   pDateFullValue.Day,
                                                   pDateFullValue.Hour,
                                                   pDateFullValue.Min,
                                                   pDateFullValue.Sec));*/
    utfdescString wStr;
    wStr.sprintf("%04d%02d%02d%02d%02d%02d",
                 pDateFullValue.Year,
                 pDateFullValue.Month,
                 pDateFullValue.Day,
                 pDateFullValue.Hour,
                 pDateFullValue.Min,
                 pDateFullValue.Sec);

    QDomText NameT=pXmlDoc.createTextNode(wStr.toQString());
    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}


QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,int pintValue)
{
char wBuf[20];
sprintf(wBuf,"%d",pintValue);
return (_appendElement(pXmlDoc,pNode,pName,wBuf));
}

QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,long long plonglongValue)
{
char wBuf[20];
sprintf(wBuf,"%lld",plonglongValue);
return (_appendElement(pXmlDoc,pNode,pName,wBuf));
}
QDomElement appendElementUInt64 (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,uint64_t plonglongValue)
{
    char wBuf[20];
    sprintf(wBuf,"%lu",plonglongValue);
    return (_appendElement(pXmlDoc,pNode,pName,wBuf));
}
QDomElement appendElementUInt32 (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,uint32_t plonglongValue)
{
    char wBuf[20];
    sprintf(wBuf,"%u",plonglongValue);
    return (_appendElement(pXmlDoc,pNode,pName,wBuf));
}
QDomElement appendElementInt64 (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,int64_t plonglongValue)
{
    char wBuf[20];
    sprintf(wBuf,"%ld",plonglongValue);
    return (_appendElement(pXmlDoc,pNode,pName,wBuf));
}
QDomElement appendElementInt32 (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,int32_t plonglongValue)
{
    char wBuf[20];
    sprintf(wBuf,"%d",plonglongValue);
    return (_appendElement(pXmlDoc,pNode,pName,wBuf));
}
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,long plongValue)
{
char wBuf[20];
sprintf(wBuf,"%ld",plongValue);
return (_appendElement(pXmlDoc,pNode,pName,wBuf));
}

QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,float pfloatValue)
{
char wBuf[25];
sprintf(wBuf,"%+f",pfloatValue);
return (_appendElement(pXmlDoc,pNode,pName,wBuf));
}
QDomElement appendElement (QDomDocument &pXmlDoc, QDomNode &pNode, const char *pName, double pdoubleValue)
{
char wBuf[25];
sprintf(wBuf,"%+f",pdoubleValue);
return (_appendElement(pXmlDoc,pNode,pName,wBuf));
}

#include <ztoolset/uristring.h>

QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,uriString pValue)
{
    QDomElement NewElement=pXmlDoc.createElement(pName);
    QDomText NameT=pXmlDoc.createTextNode(pValue.toCString_Strait());
    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}
QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,checkSum pValue)
{
    QDomElement NewElement=pXmlDoc.createElement(pName);
    QDomText NameT=pXmlDoc.createTextNode(QString(pValue.toHex()));
    pNode.appendChild(NewElement);
    NewElement.appendChild(NameT);

    return(NewElement);
}
//!---------------- end appendElement--------------------------
//!
//!
#include <zcrypt/checksum.h>

void getElement (utfcodeString &pField ,QDomElement &pElement)
{
    pField.fromQString( pElement.text());

return;
}
void getElement (checkSum &pField ,QDomElement &pElement)
{
    QByteArray wQBA (pElement.text().toUtf8());
    pField=QByteArray::fromHex(wQBA);
return;
}
void getElement (utfdescString &pField , QDomElement &pElement)
{
    pField.fromQString( pElement.text());
//    strncpy(pField.content, pElement.text().toUtf8(),cst_desclen);
return;
}

void getElement (char *pField ,QDomElement &pElement)
{
    strcpy(pField, pElement.text().toUtf8());
return;
}

void getElement (bool &pField ,QDomElement &pElement)
{

    if (strncasecmp(pElement.text().toUpper().toUtf8(),"TRUE",4)==0)
                pField = true ;
        else
                pField = false;

return;
}



void getElement (ZDate &pField ,QDomElement &pElement)
{
    pField.fromQString(pElement.text());
return;
}
void getElement (ZDateFull &pField ,QDomElement &pElement)
{
    pField.fromQString(pElement.text());
return;
}


void getElement (uriString &pField ,QDomElement &pElement)
{
    pField.fromQString( pElement.text());
return;
}
/*Docid is application dependant therefore defined within zapplicationtypes.h
 #ifdef ZAUTHORIZATION_H
void getElement (Docid_struct &pField ,QDomElement &pElement)
{
    bool Ok=true;
    pField.id= pElement.text().toLong(&Ok);
    if (!Ok)
            {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_XMLERROR,
                          Severity_Error,
                          "XML_Load Long conversion problem for XML element name <%s> value <%s>",
                          pElement.tagName().toStdString().c_str(),
                          pElement.text().toStdString().c_str());
    ZException.printUserMessage();
            }
return;
}
#endif//#ifdef ZAUTHORIZATION_H
*/
void getElement (utfidentityString &pField , QDomElement &pElement)
{
//    QString welt = pElement.text();
    pField.fromUtf((utf8_t*)pElement.text().toUtf8().data());
return;
}
void getElement (int &pField ,QDomElement &pElement)
{
    bool Ok=true;
    pField= pElement.text().toInt(&Ok);
    if (!Ok)
            {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_XMLERROR,
                          Severity_Error,
                          "XML_Load Int conversion problem for XML element name <%s> value <%s>",
                          pElement.tagName().toStdString().c_str(),
                          pElement.text().toStdString().c_str());
    ZException.printUserMessage();
            }
return;
}
void getElementLong (long &pField ,QDomElement &pElement)
{
bool Ok=true;
    pField= pElement.text().toLong(&Ok);
    if (!Ok)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_XMLERROR,
                                  Severity_Error,
                                  "XML_Load Long conversion problem for XML element name <%s> value <%s>",
                                  pElement.tagName().toStdString().c_str(),
                                  pElement.text().toStdString().c_str());
            ZException.printUserMessage();
            }
return;
}
void getElementSize_t (size_t &pField ,QDomElement &pElement)
{
bool Ok=true;
    pField= pElement.text().toLong(&Ok);
    if (!Ok)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_XMLERROR,
                                  Severity_Error,
                                  "XML_Load Long conversion problem for XML element name <%s> value <%s>",
                                  pElement.tagName().toStdString().c_str(),
                                  pElement.text().toStdString().c_str());
            ZException.printUserMessage();
            }
return;
}
void getElementULong (unsigned long &pField ,QDomElement &pElement)
{
bool Ok=true;
    pField= pElement.text().toLong(&Ok);
    if (!Ok)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_XMLERROR,
                                  Severity_Error,
                                  "XML_Load Long conversion problem for XML element name <%s> value <%s>",
                                  pElement.tagName().toStdString().c_str(),
                                  pElement.text().toStdString().c_str());
            ZException.printUserMessage();
            }
return;
}
void getElementLongLong (long long &pField ,QDomElement &pElement)
{
    bool Ok=true;
    pField= pElement.text().toLongLong(&Ok);
    if (!Ok)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_XMLERROR,
                                  Severity_Error,
                                  "XML_Load LongLong conversion problem for XML element name <%s> value <%s>",
                                  pElement.tagName().toStdString().c_str(),
                                  pElement.text().toStdString().c_str());
            ZException.printUserMessage();
            }
return;
}
void getElementUInt64 (uint64_t &pField ,QDomElement &pElement)
{
    bool Ok=true;
    pField= pElement.text().toULong(&Ok);
    if (!Ok)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_XMLERROR,
                                  Severity_Error,
                                  "XML_Load LongLong conversion problem for XML element name <%s> value <%s>",
                                  pElement.tagName().toStdString().c_str(),
                                  pElement.text().toStdString().c_str());
            ZException.printUserMessage();
            }
return;
}
void getElementUInt32 (uint32_t &pField ,QDomElement &pElement)
{
    bool Ok=true;
    pField= pElement.text().toUInt(&Ok);
    if (!Ok)
            {
            ZException.setMessage(_GET_FUNCTION_NAME_,
                                  ZS_XMLERROR,
                                  Severity_Error,
                                  "XML_Load LongLong conversion problem for XML element name <%s> value <%s>",
                                  pElement.tagName().toStdString().c_str(),
                                  pElement.text().toStdString().c_str());
            ZException.printUserMessage();
            }
return;
}
void getElementInt64 (int64_t &pField ,QDomElement &pElement)
{
    bool Ok=true;
    pField= pElement.text().toLong(&Ok);
    if (!Ok)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "XML_Load LongLong conversion problem for XML element name <%s> value <%s>",
                              pElement.tagName().toStdString().c_str(),
                              pElement.text().toStdString().c_str());
        ZException.printUserMessage();
    }
    return;
}
void getElementInt32 (int32_t &pField ,QDomElement &pElement)
{
    bool Ok=true;
    pField= pElement.text().toInt(&Ok);
    if (!Ok)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_XMLERROR,
                              Severity_Error,
                              "XML_Load LongLong conversion problem for XML element name <%s> value <%s>",
                              pElement.tagName().toStdString().c_str(),
                              pElement.text().toStdString().c_str());
        ZException.printUserMessage();
    }
    return;
}
void getElement (float &pField ,QDomElement &pElement)
{
    bool Valid;
    pField = pElement.text().toFloat(&Valid);
    if (!Valid)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_XMLERROR,
                          Severity_Error,
                          "XML_Load Float conversion problem for XML element name <%s> value <%s>",
                          pElement.tagName().toStdString().c_str(),
                          pElement.text().toStdString().c_str());
    ZException.printUserMessage();
    }
return;
}
void getElement (double &pField ,QDomElement &pElement)
{
    bool Valid;
    pField = pElement.text().toDouble(&Valid);
    if (!Valid)
    {
    ZException.setMessage(_GET_FUNCTION_NAME_,
                          ZS_XMLERROR,
                          Severity_Error,
                          "XML_Load Double conversion problem for XML element name <%s> value <%s>",
                          pElement.tagName().toStdString().c_str(),
                          pElement.text().toStdString().c_str());
    ZException.printUserMessage();
    }
    return;
}



QDomElement appendElement (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pName,bool pboolValue)
{
const char *BoolString = "false";
if (pboolValue)
        BoolString="true";
return (_appendElement(pXmlDoc,pNode,pName,(char *)BoolString));
}

QDomElement appendWidget (QDomDocument &pXmlDoc,QDomNode &pNode, const char *pWindowName, QWidget *pWidget)
{
QDomElement Window=pXmlDoc.createElement(pWindowName);
pNode.appendChild(Window);

QPoint wPoint = pWidget->mapToGlobal(pWidget->pos()) ;

appendElement(pXmlDoc,Window,"width",pWidget->width());
appendElement(pXmlDoc,Window,"height",pWidget->height());
appendElement(pXmlDoc,Window,"x",wPoint.x());
appendElement(pXmlDoc,Window,"y",wPoint.y());
return (Window);
}





#endif  // QXMLUTILITIES_CPP
