#ifndef ZXMLPRIMITIVES_CPP
#define ZXMLPRIMITIVES_CPP

#include <zxml/zxmlprimitives.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zdate.h>
#include <ztoolset/zfunctions.h>
#include <ztoolset/zexceptionmin.h>

using namespace zbs;


int cst_XMLIndent = 3;
void setXmlIdent(int pIndent)
{
    cst_XMLIndent = pIndent;
}



zxmlNode*
XMLsearchForChildTag(zxmlNode* pTopNode, const utf8VaryingString &pTag)
{
  zxmlNode* wNode;
  zxmlNode* wNode1=nullptr;
  if (pTopNode->getName()==pTag)
    return pTopNode;
  ZStatus wSt=pTopNode->getFirstChild(wNode);
  while ((wSt==ZS_SUCCESS)&&(wNode->getName()!=pTag))
  {
    if (XMLsearchForChildTag(wNode,pTag)!=nullptr)
      return wNode;
    wSt=wNode->getNextNode(wNode1);
    XMLderegister(wNode);
    wNode=wNode1;
  }
  return nullptr;
}//XMLsearchForChildTag

bool
XMLhasNamedChild(zxmlNode* pTopNode, const utf8VaryingString &pName)
{
  zxmlNode* wNode;
  zxmlNode* wNode1=nullptr;
  if (pTopNode->getName()==pName)
    return true;
  ZStatus wSt=pTopNode->getFirstChild(wNode);
  while (wSt==ZS_SUCCESS)
  {
    if (wNode->getName()==pName)
        return true;
    wSt=wNode->getNextNode(wNode1);
    XMLderegister(wNode);
    wNode=wNode1;
  }
  return false;
}//XMLsearchForChildTag


utf8VaryingString fmtXMLdeclaration(const utf8VaryingString& pEncoding)
{
  utf8VaryingString wBuffer;
  wBuffer.sprintf ( "<?xml version='1.0' encoding='%s'?>\n",pEncoding.toCChar());
  return wBuffer;
}
utf8VaryingString fmtXMLmainVersion(const utf8VaryingString& pName,unsigned long pFullVersion,const int pLevel)
{
  utf8VaryingString wStr;
  wStr.sprintf("'%s'",getVersionStr(pFullVersion).toCChar());

  return fmtXMLnodeWithAttributes(pName,"version",wStr.toCChar(),pLevel);
}
utf8VaryingString fmtXMLversion(const utf8VaryingString& pName,unsigned long pFullVersion,const int pLevel)
{
  utf8VaryingString wStr;
  wStr.sprintf("'%s'",getVersionStr(pFullVersion).toCChar());
  return fmtXMLchar(pName,wStr.toCChar(),pLevel);
}

utf8VaryingString fmtXMLCData(const utf8VaryingString &pVarName, const ZDataBuffer& pVarContent, const int pLevel)
{
  utf8VaryingString wReturn;
  int wIndent = pLevel * cst_XMLIndent;
//  char wBuffer[500];
//  memset (wBuffer,0,sizeof(wBuffer));
  wReturn.sprintf ("%*c<%s>\n", wIndent,' ',pVarName.toCChar());
  //<![CDATA[ … ]]>
  wReturn += "<![CDATA[";
  utf8_t* wPtr=wReturn.extendBytesBZero(pVarContent.Size);
  size_t wi=0;
  while (wi < pVarContent.Size)
    *wPtr++ = pVarContent.Data[wi++];
  wReturn += "]]>\n";
  wReturn.sprintf ("%*c</%s>\n", wIndent,' ',pVarName.toCChar());
  return wReturn;
}

utf8VaryingString fmtXMLBlob(const utf8VaryingString &pVarName, ZDataBuffer& pVarContent, const int pLevel)
{
  ZDataBuffer wB64 ;
  pVarContent.encryptB64(&wB64);
  return fmtXMLCData(pVarName,wB64,pLevel);
}

ZStatus XMLgetChildBlob(zxmlElement *pElement, const utf8VaryingString& pChildName, ZDataBuffer &pContent,ZaiErrors* pErrorlog,ZaiE_Severity pSeverity)
{
  ZDataBuffer wB64 ;
  ZStatus wSt=XMLgetChildCData(pElement,pChildName,wB64,pErrorlog,pSeverity);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  wSt = wB64.uncryptB64(&pContent);
  if (wSt!=ZS_SUCCESS)
  {

    if (pErrorlog==nullptr) {
        ZException.setMessage("XMLgetChildBlob",wSt,Severity_Error,
                              "XMLgetChildBlob-E-UNCRYPTB64 Error cannot uncrypt B64 node name <%s>",
                              pChildName.toCChar());
    }
    else
        pErrorlog->logZStatus(pSeverity,wSt,"XMLgetChildCData-E-CNTFINDND Error cannot uncrypt B64 node element with name <%s> status <%s>",
                              pChildName.toCChar(),
                              decode_ZStatus(wSt));
  } // wSt
  return wSt;
}

ZStatus XMLgetChildCData(zxmlElement *pElement, const utf8VaryingString& pChildName, ZDataBuffer &pContent,ZaiErrors* pErrorlog,ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  if (!pElement->hasCData()) {
    pErrorlog->errorLog("");
  }
  zxmlElement*  wChild=nullptr;
  zxmlNode*     wChildNodeCData=nullptr;
  wSt=pElement->getChildByName((zxmlNode*&)wChild,pChildName);
  if (wSt!=ZS_SUCCESS)
  {

    if (pErrorlog==nullptr) {
        ZException.setMessage("XMLgetChildCData",wSt,Severity_Error,
                              "XMLgetChildCData-E-CNTFINDND Error cannot find node element with name <%s>",
                              pChildName.toCChar());
        return wSt;
    }
    else
        pErrorlog->logZStatus(pSeverity,wSt,"XMLgetChildCData-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
                              pChildName.toCChar(),
                              decode_ZStatus(wSt));
    return wSt;
  } // wSt
  wSt=wChild->getFirstChildCData(wChildNodeCData);
  if (wSt!=ZS_SUCCESS)
  {
    if (pErrorlog==nullptr) {
        ZException.setMessage("XMLgetChildCData",wSt,Severity_Error,
                              "XMLgetChildCData-E-CNTFINDND Error cannot find CData section within element <%s>",
                              wChild->getName().toCChar());
        return wSt;
    }
    else
        pErrorlog->logZStatus(pSeverity,wSt,"XMLgetChildCData-E-CNTFINDND Error cannot find CData section within element <%s> status <%s>",
                              wChild->getName().toCChar(),
                              decode_ZStatus(wSt));
    return wSt;
  }
  wSt=wChildNodeCData->getCData(pContent);
  if (wSt!=ZS_SUCCESS)
  {
    if (pErrorlog==nullptr) {
        ZException.setMessage("XMLgetChildCData",wSt,Severity_Error,
                              "XMLgetChildCData-E-CNTFINDND Error cannot find CData section within element <%s>",
                              wChild->getName().toCChar());
        return wSt;
    }
    else
        pErrorlog->logZStatus(pSeverity,wSt,"XMLgetChildCData-E-CNTFINDND Error cannot find CData section within element <%s> status <%s>",
                              wChild->getName().toCChar(),
                              decode_ZStatus(wSt));
    return wSt;
  }

}

utf8VaryingString fmtXMLnode(const utf8VaryingString &pNodeName, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>\n",
            wIndent,' ',pNodeName.toCChar());
    return utf8VaryingString(wBuffer);
}

utf8VaryingString fmtXMLendnode(const utf8VaryingString &pNodeName, const int pLevel)
{
  int wIndent=pLevel*cst_XMLIndent;
  char wBuffer[500];
  memset (wBuffer,0,sizeof(wBuffer));
  sprintf (wBuffer,
      "%*c</%s>\n",
      wIndent,' ',pNodeName.toCChar());
  return utf8VaryingString(wBuffer);
}

utf8VaryingString fmtXMLnodeWithAttributes(const utf8VaryingString& pNodeName,
                                     const utf8VaryingString &pAttName,const utf8VaryingString &pAttValue,
                                     const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s %s = \"%s\">\n",
            wIndent,' ',pNodeName.toCChar(),pAttName.toCChar(),pAttValue.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLnodeWithAttributes(const utf8VaryingString &pNodeName,
                                     const utf8VaryingString &pAttName1,const utf8VaryingString &pAttValue1,
                                     const utf8VaryingString &pAttName2,const utf8VaryingString &pAttValue2,
                                     const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s %s = \"%s\" %s = \"%s\" > \n",
            wIndent,' ',pNodeName.toCChar(),pAttName1.toCChar(),pAttValue1.toCChar(),pAttName2.toCChar(),pAttValue2.toCChar());
    return utf8VaryingString(wBuffer);
}

utf8VaryingString fmtXMLnodeWithAttributes( const utf8VaryingString &pNodeName,
                                            const utf8VaryingString &pAttName1,const utf8VaryingString &pAttValue1,
                                            const utf8VaryingString &pAttName2,const utf8VaryingString &pAttValue2,
                                            const utf8VaryingString &pAttName3,const utf8VaryingString &pAttValue3,
                                            const int pLevel)
{
  int wIndent=pLevel*cst_XMLIndent;
  char wBuffer[500];
  memset (wBuffer,0,sizeof(wBuffer));
  sprintf (wBuffer,
      "%*c<%s %s = \"%s\" %s = \"%s\" %s = \"%s\"> \n",
      wIndent,' ',pNodeName.toCChar(),pAttName1.toCChar(),pAttValue1.toCChar(),pAttName2.toCChar(),pAttValue2.toCChar(),pAttName3.toCChar(),pAttValue3.toCChar());
  return utf8VaryingString(wBuffer);
}


utf8VaryingString fmtXMLchar(const utf8VaryingString &pVarName, const utf8VaryingString &pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%s</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent.toCChar(),pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLcomment(const utf8VaryingString &pContent, const int pLevel)
{
  int wIndent=pLevel*cst_XMLIndent;
  char wBuffer[500];
  memset (wBuffer,0,sizeof(wBuffer));
  sprintf (wBuffer,
      "%*c<!-- %s -->\n",
      wIndent,' ',pContent.toCChar());
  return utf8VaryingString(wBuffer);
}

utf8VaryingString fmtXMLcomment(const utf8VaryingString &pContent)
{
  char wBuffer[500];
  memset (wBuffer,0,sizeof(wBuffer));
  sprintf (wBuffer,
      "<!-- %s -->\n",
      pContent.toCChar());
  return utf8VaryingString(wBuffer);
}

utf8VaryingString& suppressLF(utf8VaryingString& pString)
{
  if (pString.isEmpty())
    return pString;
  utf8_t* wPtr=pString.Data+pString.UnitCount;
  while ((wPtr > pString.Data)&&(*wPtr!='\n'))
    wPtr--;
  *wPtr=0;
  return pString;
}
utf8VaryingString& fmtXMLaddInlineComment(utf8VaryingString& pXmlString, const utf8VaryingString &pComment)
{
  suppressLF(pXmlString).addsprintf("<!-- %s -->\n",pComment.toCChar());
  return pXmlString;
}

utf8VaryingString fmtXMLstring(const utf8VaryingString &pVarName,
                               const utf8VaryingString pVarContent,
                               const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%s</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent.toCChar(),pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLdouble(const utf8VaryingString &pVarName,const double pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%g</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLfloat(const utf8VaryingString &pVarName,const float pVarContent, const int pLevel)
{
    return fmtXMLdouble(pVarName,double(pVarContent),pLevel);
}
utf8VaryingString fmtXMLint(const utf8VaryingString &pVarName,const int pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%d</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLuint(const utf8VaryingString &pVarName,const unsigned int pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%u</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLlong(const utf8VaryingString &pVarName, const long pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%ld</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLulong(const utf8VaryingString &pVarName,const unsigned long pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%lu</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLint8(const utf8VaryingString &pVarName,const int8_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%d</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLuint8(const utf8VaryingString &pVarName,const uint8_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%u</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLint16(const utf8VaryingString &pVarName,const int16_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%d</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLuint16(const utf8VaryingString &pVarName,const uint16_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%u</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLint32(const utf8VaryingString &pVarName,const int32_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%d</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLuint32(const utf8VaryingString &pVarName,const uint32_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%u</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLint64(const utf8VaryingString &pVarName,const int64_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%ld</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLuint64(const utf8VaryingString &pVarName,const uint64_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%lu</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}

utf8VaryingString fmtXMLintHexa(const utf8VaryingString &pVarName, const int pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%Xd</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLuintHexa(const utf8VaryingString &pVarName,const unsigned int pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%X</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}

utf8VaryingString fmtXMLlongHexa(const utf8VaryingString &pVarName,const long pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%lXd</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLulongHexa(const utf8VaryingString &pVarName,const unsigned long pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%lX</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}

utf8VaryingString fmtXMLint32Hexa(const utf8VaryingString &pVarName, const int32_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%Xd</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
utf8VaryingString fmtXMLuint32Hexa(const utf8VaryingString &pVarName,const uint32_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>0x%0X</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent,pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}

utf8VaryingString fmtXMLbool(const utf8VaryingString &pVarName,const bool pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%s</%s>\n",
            wIndent,' ',pVarName.toCChar(),pVarContent?"true":"false",pVarName.toCChar());
    return utf8VaryingString(wBuffer);
}
/* Deprecated
utf8VaryingString fmtXMLdate(const utf8VaryingString &pVarName,const ZDate pVarContent, const int pLevel)
{
  int wIndent=pLevel*cst_XMLIndent;
  char wBuffer[500];
  memset (wBuffer,0,sizeof(wBuffer));
  sprintf (wBuffer,
      "%*c<%s>%s</%s>\n",
      wIndent,' ',pVarName.toCChar(),pVarContent.toUTC().toString(),pVarName.toCChar());
  return utf8VaryingString(wBuffer);
}
*/
utf8VaryingString fmtXMLdatefull(const utf8VaryingString &pVarName,const ZDateFull pVarContent, const int pLevel)
{
  int wIndent=pLevel*cst_XMLIndent;
  char wBuffer[500];
  memset (wBuffer,0,sizeof(wBuffer));
  if(pVarContent.isInvalid()){
    sprintf(wBuffer,"%*c<%s><!-- invalid date --></%s>\n",
      wIndent,' ',pVarName.toCChar(),pVarName.toCChar());
  }
  else
  sprintf (wBuffer,
      "%*c<%s>%s</%s>\n",
      wIndent,' ',pVarName.toCChar(),pVarContent.toUTCGMT().toString(),pVarName.toCChar());
  return utf8VaryingString(wBuffer);
}

ZStatus
XMLgetChildText (zxmlElement*pElement,const utf8VaryingString& pChildName,utf8VaryingString& pTextValue) {
  return XMLgetChildText(pElement,pChildName,pTextValue,nullptr,ZAIES_Error);
}


ZStatus
XMLgetChildText (zxmlElement*pElement,const utf8VaryingString& pChildName,utf8VaryingString& pTextValue,ZaiErrors* pErrorlog,ZaiE_Severity pSeverity)
{
    zxmlElement*wChild=nullptr;
    ZStatus wSt=pElement->getChildByName((zxmlNode*&)wChild,pChildName.toCChar());
    if (wSt!=ZS_SUCCESS)
    {

      if (pErrorlog==nullptr) {
        ZException.setMessage("XMLgetChildText",wSt,Severity_Error,
            "XMLgetChildText-E-CNTFINDND Error cannot find node element with name <%s>",
            pChildName.toCChar());
        return wSt;
      }
      else
      pErrorlog->logZStatus(pSeverity,wSt,"XMLgetChildText-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
                       pChildName.toCChar(),
                       decode_ZStatus(wSt));
        return wSt;
    }
    wSt=wChild->getNodeText(pTextValue);
    if (wSt!=ZS_SUCCESS)
    {
      if (pErrorlog==nullptr) {
        ZException.setMessage("XMLgetChildText",wSt,Severity_Error,
            "XMLgetChildText-E-CNTFINDND Error cannot find text within element <%s>",
            wChild->getName().toCChar());
        return wSt;
      }
      else
        pErrorlog->logZStatus(pSeverity,wSt,"XMLgetChildText-E-CNTFINDND Error cannot find text within element <%s> status <%s>",
                       wChild->getName().toCChar(),
                       decode_ZStatus(wSt));
        return wSt;
    }
    return ZS_SUCCESS;
} //XMLgetChildText

ZStatus
XMLgetChildFloat (zxmlElement*pElement,const utf8VaryingString& pChildName,float& pFloat) {
  return XMLgetChildFloat(pElement,pChildName,pFloat,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildFloat (zxmlElement*pElement,const utf8VaryingString &pChildName,float& pFloat,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
        return wSt;
    pFloat=float(wValue.toDouble());
    return ZS_SUCCESS;
} //XMLgetChildFloat
ZStatus
XMLgetChildDouble (zxmlElement*pElement, const utf8VaryingString &pChildName, double& pDouble)
{
  return XMLgetChildDouble(pElement,pChildName,pDouble,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildDouble (zxmlElement*pElement, const utf8VaryingString &pChildName, double& pDouble, ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
      return wSt;
    pDouble=wValue.toDouble();
    return ZS_SUCCESS;
} //XMLgetChildDouble

ZStatus
XMLgetChildBool (zxmlElement*pElement,const utf8VaryingString &pChildName,bool &pBool)
{
 return XMLgetChildBool(pElement,pChildName,pBool,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildBool (zxmlElement*pElement,const utf8VaryingString &pChildName,bool &pBool,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

//  wValue.toUpper();
  if (wValue.toUpper() == (const utf8_t*)"TRUE")
        pBool=true;
    else
        pBool=false;
    return ZS_SUCCESS;
} //XMLgetChildBool


ZStatus
XMLgetChildInt (zxmlElement*pElement,const utf8VaryingString & pChildName,int &pInt)
{
  return XMLgetChildInt(pElement,pChildName,pInt,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildInt (zxmlElement*pElement,const utf8VaryingString & pChildName,int &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pInt=wValue.toInt();
  return ZS_SUCCESS;
} //XMLgetChildInt


ZStatus
XMLgetChildLong (zxmlElement*pElement,const utf8VaryingString & pChildName, long &pLong)
{
  return XMLgetChildLong(pElement,pChildName,pLong,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildLong (zxmlElement*pElement, const utf8VaryingString & pChildName, long &pLong, ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pLong=wValue.toLong(0); /* base 0 : let string content decide for base */
  return ZS_SUCCESS;
} //XMLgetChildInt

ZStatus
XMLgetChildUInt (zxmlElement*pElement,const utf8VaryingString & pChildName, unsigned int &pUInt)
{
  return XMLgetChildUInt(pElement,pChildName,pUInt,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildUInt (zxmlElement*pElement,const utf8VaryingString & pChildName,unsigned int &pUInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pUInt=wValue.toUInt(0);/* base 0 : let string content decide for base */
  return ZS_SUCCESS;
} //XMLgetChildInt


ZStatus
XMLgetChildULong (zxmlElement*pElement,const utf8VaryingString & pChildName, unsigned long &pUInt)
{
  return XMLgetChildULong(pElement,pChildName,pUInt,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildULong (zxmlElement*pElement,const utf8VaryingString & pChildName,unsigned long &pULong,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pULong=wValue.toULong(0);/* base 0 : let string content decide for base */
  return ZS_SUCCESS;
} //XMLgetChildULong

ZStatus
XMLgetChildInt64 (zxmlElement*pElement,const utf8VaryingString & pChildName,int64_t &pLong)
{
 return XMLgetChildInt64 (pElement,pChildName,pLong,nullptr, ZAIES_Error);
}

ZStatus
XMLgetChildInt64 (zxmlElement*pElement,const utf8VaryingString & pChildName,int64_t &pLong,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pLong=wValue.toLong(0);/* base 0 : let string content decide for base */
  return ZS_SUCCESS;
} //XMLgetChildint64


ZStatus
XMLgetChildVersion (zxmlElement*pElement,const utf8VaryingString & pChildName, unsigned long &pVersion)
{
  return XMLgetChildVersion(pElement,pChildName,pVersion,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildVersion (zxmlElement*pElement,const utf8VaryingString& pChildName,unsigned long &pVersion,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pVersion=getVersionNum(wValue);
  return ZS_SUCCESS;
} //XMLgetChildVersion

/* Deprecated
ZStatus
XMLgetChildZDate(zxmlElement *pElement, const utf8VaryingString& pChildName, ZDate &pDate)
{
  return XMLgetChildZDate(pElement,pChildName,pDate,nullptr,ZAIES_Error);
}


ZStatus
XMLgetChildZDate(zxmlElement *pElement, const utf8VaryingString& pChildName, ZDate &pDate, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pDate.fromUTC (wValue);
  return ZS_SUCCESS;
} //XMLgetChildZDate
*/

ZStatus
XMLgetChildZDateFull(zxmlElement *pElement, const utf8VaryingString& pChildName, ZDateFull &pDateFull)
{
  return XMLgetChildZDateFull(pElement,pChildName,pDateFull,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildZDateFull(zxmlElement *pElement, const utf8VaryingString& pChildName, ZDateFull &pDateFull, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pDateFull.fromXmlValue (wValue);
  return ZS_SUCCESS;
} //XMLgetChildZDate



ZStatus
XMLgetChildIntHexa(zxmlElement *pElement, const utf8VaryingString& pChildName,int &pInt)
{
  return XMLgetChildIntHexa(pElement,pChildName,pInt,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildIntHexa (zxmlElement*pElement,const utf8VaryingString & pChildName,int &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pInt=wValue.toInt(16);/* base 16 : hexadecimal expected */
  return ZS_SUCCESS;
} //XMLgetChildIntHexa

ZStatus
XMLgetChildLongHexa(zxmlElement *pElement, const utf8VaryingString& pChildName,long &pLong)
{
  return XMLgetChildLongHexa(pElement,pChildName,pLong,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildLongHexa (zxmlElement*pElement,const utf8VaryingString &pChildName,long &pLong,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pLong=wValue.toLong(16);/* base 16 : hexadecimal expected */
  return ZS_SUCCESS;
} //XMLgetChildLongHexa


ZStatus
XMLgetChildUIntHexa(zxmlElement *pElement, const utf8VaryingString& pChildName,unsigned int &pInt)
{
  return XMLgetChildUIntHexa(pElement,pChildName,pInt,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildUIntHexa (zxmlElement*pElement,const utf8VaryingString &pChildName,unsigned int &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pInt=wValue.toUInt(16);/* base 16 : hexadecimal expected */
  return ZS_SUCCESS;
} //XMLgetChildUIntHexa

ZStatus
XMLgetChildULongHexa(zxmlElement *pElement, const utf8VaryingString& pChildName,unsigned long &pULong)
{
  return XMLgetChildULongHexa(pElement,pChildName,pULong,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildULongHexa (zxmlElement*pElement,const utf8VaryingString &pChildName,unsigned long &pULong,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  ZStatus wSt;
  utf8VaryingString wValue;

  if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
    return wSt;

  pULong=wValue.toULong(16);/* base 16 : hexadecimal expected */
  return ZS_SUCCESS;
} //XMLgetChildULongHexa



ZStatus
XMLgetChildInt8Hexa (zxmlElement*pElement,const utf8VaryingString & pChildName,int8_t &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
    ZStatus wSt;
    utf8VaryingString wValue;
    int wV;

    if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
        return wSt;

    wV=wValue.toInt(16);/* base 16 : hexadecimal expected */
    pInt = int8_t(wV);
    return ZS_SUCCESS;
} //XMLgetChildInt8Hexa


ZStatus
XMLgetChildUInt8Hexa (zxmlElement*pElement,const utf8VaryingString & pChildName,uint8_t &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
    ZStatus wSt;
    utf8VaryingString wValue;
    unsigned int wV;

    if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
        return wSt;

    wV=wValue.toInt(16);/* base 16 : hexadecimal expected */
    pInt = int8_t(wV);
    return ZS_SUCCESS;
} //XMLgetChildUInt8Hexa

ZStatus
XMLgetChildInt16Hexa (zxmlElement*pElement,const utf8VaryingString & pChildName,int16_t &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
    ZStatus wSt;
    utf8VaryingString wValue;
    int wV;

    if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
        return wSt;

    wV=wValue.toInt(16);/* base 16 : hexadecimal expected */
    pInt = int16_t(wV);
    return ZS_SUCCESS;
} //XMLgetChildInt16Hexa

ZStatus
XMLgetChildUInt16Hexa (zxmlElement*pElement,const utf8VaryingString & pChildName,uint16_t &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
    ZStatus wSt;
    utf8VaryingString wValue;
    unsigned int wV;

    if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
        return wSt;

    wV=wValue.toInt(16);/* base 16 : hexadecimal expected */
    pInt = uint16_t(wV);
    return ZS_SUCCESS;
} //XMLgetChildUInt16Hexa

ZStatus
XMLgetChildInt32Hexa (zxmlElement*pElement,const utf8VaryingString & pChildName,int32_t &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
    ZStatus wSt;
    utf8VaryingString wValue;
    int wV;

    if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
        return wSt;

    wV=wValue.toInt(16);/* base 16 : hexadecimal expected */
    pInt = int32_t(wV);
    return ZS_SUCCESS;
} //XMLgetChildInt32Hexa

ZStatus
XMLgetChildUInt32Hexa (zxmlElement*pElement,const utf8VaryingString & pChildName,uint32_t &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
    ZStatus wSt;
    utf8VaryingString wValue;
    unsigned int wV;

    if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
        return wSt;

    wV=wValue.toInt(16);/* base 16 : hexadecimal expected */
    pInt = uint32_t(wV);
    return ZS_SUCCESS;
} //XMLgetChildUInt32Hexa

ZStatus
XMLgetChildInt64Hexa (zxmlElement*pElement,const utf8VaryingString & pChildName,int64_t &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
    ZStatus wSt;
    utf8VaryingString wValue;
    long wV;

    if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
        return wSt;

    wV=wValue.toLong(16);/* base 16 : hexadecimal expected */
    pInt = int64_t(wV);
    return ZS_SUCCESS;
} //XMLgetChildInt32Hexa

ZStatus
XMLgetChildUInt64Hexa (zxmlElement*pElement,const utf8VaryingString & pChildName,uint64_t &pInt,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
    ZStatus wSt;
    utf8VaryingString wValue;
    unsigned long wV;

    if ((wSt=XMLgetChildText(pElement,pChildName,wValue,pErrorlog,pSeverity) ) != ZS_SUCCESS)
        return wSt;

    wV=wValue.toULong(16);/* base 16 : hexadecimal expected */
    pInt = uint64_t(wV);
    return ZS_SUCCESS;
} //XMLgetChildUInt32Hexa



/*
  md5 xml representation

  <name>
    <md5>HEXAVALUE</md5>
  </name>

 */



ZStatus
XMLgetChildMd5(zxmlElement *pElement, const utf8VaryingString& pChildName,md5 &pMd5)
{
  return XMLgetChildMd5(pElement,pChildName,pMd5,nullptr,ZAIES_Error);
}
ZStatus
XMLgetChildMd5 (zxmlElement*pElement,const utf8VaryingString &pChildName,md5 &pMd5,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  utf8VaryingString wValue;
  zxmlElement*wChild=nullptr;
  ZStatus wSt=pElement->getChildByName((zxmlNode*&)wChild,pChildName);
  if (wSt!=ZS_SUCCESS)
    {
    pErrorlog->logZStatus(pSeverity,wSt,"XMLgetChildMd5-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
                          pChildName.toCChar(),
                          decode_ZStatus(wSt));
    return ZS_NOTFOUND;
    }
  int wRet=XMLgetChildText(wChild,"md5",wValue,pErrorlog,pSeverity) ;
  XMLderegister(wChild);
  if (wRet<0) {
    return ZS_INVNAME;
  }

  return pMd5.fromHexa(wValue);
} //XMLgetChildMd5

ZStatus
XMLgetChildCheckSum(zxmlElement *pElement, const utf8VaryingString& pChildName,checkSum &pCheckSum)
{
  return XMLgetChildCheckSum(pElement,pChildName,pCheckSum,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildCheckSum (zxmlElement*pElement,const utf8VaryingString & pChildName,checkSum &pCheckSum,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  utf8VaryingString wValue;
  zxmlElement*wChild=nullptr;
  ZStatus wSt=pElement->getChildByName((zxmlNode*&)wChild,pChildName);
  if (wSt!=ZS_SUCCESS)
    {
    pErrorlog->logZStatus(pSeverity,wSt,"XMLgetChildCheckSum-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
                          pChildName.toCChar(),
                          decode_ZStatus(wSt));
    return ZS_XMLERROR;
    }
  int wRet=XMLgetChildText(wChild,"checksum",wValue,pErrorlog,pSeverity);
  XMLderegister(wChild);
  if ( wRet<0) {
    return ZS_INVNAME;
  }

  utf8_t* wPtr=wValue.Data;
  if (wValue.Data[0]=='<')
  {
    wPtr++;
    utf8_t* wPtr1 = wPtr;
    while (*wPtr1 &&(*wPtr1 != '>'))
      wPtr1++;
    *wPtr1='\0';
  }
  return pCheckSum.fromHexa((const char*)wPtr);

} //XMLgetChildCheckSum

utf8VaryingString fmtXMLmd5(const utf8VaryingString &pVarName, const md5& pVarContent, const int pLevel)
{
  utf8VaryingString wReturn = fmtXMLnode(pVarName,pLevel);

  int wIndent=(pLevel+1)*cst_XMLIndent;
  char wBuffer[500];
  memset (wBuffer,0,sizeof(wBuffer));


  sprintf (wBuffer,
      "%*c<md5>%s</md5>\n",
      wIndent,' ',pVarContent.toHexa().toCChar());

  wReturn += wBuffer;

  wReturn += fmtXMLendnode(pVarName,pLevel);
  return wReturn;
}

utf8VaryingString fmtXMLcheckSum(const utf8VaryingString &pVarName, const checkSum& pVarContent, const int pLevel)
{
  utf8VaryingString wReturn = fmtXMLnode(pVarName,pLevel);

  int wIndent=(pLevel+1)*cst_XMLIndent;
  char wBuffer[500];
  memset (wBuffer,0,sizeof(wBuffer));

  sprintf (wBuffer,
      "%*c<checksum>%s</checksum>\n",
      wIndent,' ',pVarContent.toHexa().toCChar());

  wReturn += wBuffer;

  wReturn += fmtXMLendnode(pVarName,pLevel);
  return wReturn;
}//fmtXMLcheckSum



utf8VaryingString fmtXMLSSLKeyB64(const utf8VaryingString &pVarName, const ZCryptKeyAES256& pVarContent, const int pLevel)
{
  utf8VaryingString wReturn = fmtXMLnode(pVarName,pLevel);

  int wIndent=(pLevel+1)*cst_XMLIndent;
  char wBuffer[500];
  memset (wBuffer,0,sizeof(wBuffer));

  sprintf (wBuffer,
      "%*c<sslkeyaes256b64>%s</sslkeyaes256b64>\n",
      wIndent,' ',pVarContent.toB64().Data);

  wReturn += wBuffer;

  wReturn += fmtXMLendnode(pVarName,pLevel);
  return wReturn;
}//fmtXMLSSLKeyB64


utf8VaryingString fmtXMLSSLVectorB64(const utf8VaryingString &pVarName, const ZCryptVectorAES256& pVarContent, const int pLevel)
{
  utf8VaryingString wReturn = fmtXMLnode(pVarName,pLevel);

  int wIndent=(pLevel+1)*cst_XMLIndent;
  char wBuffer[500];
  memset (wBuffer,0,sizeof(wBuffer));

  sprintf (wBuffer,
      "%*c<sslvectoraes256b64>%s</sslvectoraes256b64>\n",
      wIndent,' ',pVarContent.toB64().Data);

  wReturn += wBuffer;

  wReturn += fmtXMLendnode(pVarName,pLevel);
  return wReturn;
}//fmtXMLSSLVectorB64

ZStatus
XMLgetChildSSLKeyB64 (zxmlElement*pElement,const utf8VaryingString &pChildName,ZCryptKeyAES256 &pKey)
{
  return XMLgetChildSSLKeyB64 (pElement,pChildName,pKey,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildSSLKeyB64 (zxmlElement*pElement,const utf8VaryingString &pChildName,ZCryptKeyAES256 &pKey,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  utf8VaryingString wValue;
  zxmlElement*wChild=nullptr;
  ZStatus wSt=pElement->getChildByName((zxmlNode*&)wChild,pChildName);
  if (wSt!=ZS_SUCCESS)
  {
    pErrorlog->logZStatus(pSeverity,wSt,"XMLgetChildSSLKeyB64-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
                          pChildName.toCChar(),
                          decode_ZStatus(wSt));
    return ZS_XMLERROR;
  }

  if (XMLgetChildText(pElement,"sslkeyaes256b64",wValue,pErrorlog,pSeverity) <0)
    {
    pErrorlog->log(pSeverity,"XMLgetChildSSLKeyB64-E-INVTYPE cannot find node content <sslkeyaes256b64>");
    return ZS_INVTYPE;
    }
  return pKey.set(wValue.decodeB64().toUtf());
} //XMLgetChildSSLKeyB64


ZStatus
XMLgetChildSSLVectorB64 (zxmlElement*pElement,const utf8VaryingString &pChildName,ZCryptVectorAES256 &pKey)
{
  return XMLgetChildSSLVectorB64 (pElement,pChildName,pKey,nullptr,ZAIES_Error);
}

ZStatus
XMLgetChildSSLVectorB64 (zxmlElement*pElement,const utf8VaryingString &pChildName,ZCryptVectorAES256 &pKey,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity)
{
  utf8VaryingString wValue;
  zxmlElement*wChild=nullptr;
  ZStatus wSt=pElement->getChildByName((zxmlNode*&)wChild,pChildName);
  if (wSt!=ZS_SUCCESS)
  {
    pErrorlog->logZStatus(pSeverity,wSt,"XMLgetChildSSLVectorB64-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
        pChildName.toCChar(),
        decode_ZStatus(wSt));
    return ZS_XMLERROR;
  }

  if (XMLgetChildText(pElement,"sslvectoraes256b64",wValue,pErrorlog,pSeverity) <0)
  {
    pErrorlog->log(pSeverity,"XMLgetChildSSLVectorB64-E-INVTYPE cannot find node <sslvectoraes256b64>");
    return ZS_INVTYPE;
  }
  return pKey.set(wValue.decodeB64().toUtf());

} //XMLgetChildSSLKeyB64


size_t Payload=100;

void setPayload(size_t pPayload) {Payload = pPayload;}

/**
 * @brief XMLLoadEntity
 * @param pFd           file handler - file must be open for reading
 * @param pEntity       keyword to search without any '<' or '>' character
 * @param pXMLContent   returned Xml content containing entity including leading entity markup and end entity markup
 * @param pOffset       input : file offset to start search
 *                      output : file offset pointing to first byte AFTER end entity markup
 * @return              a ZStatus
 */
ZStatus
XMLLoadEntity(__FILEHANDLE__ pFd, const utf8VaryingString& pEntity,utf8VaryingString& pXMLContent,size_t& pOffset)
{
  bool wEof=false;
  size_t wCurrentOffset = pOffset;
  ZDataBuffer wZDB ;
  ssize_t wOffsetStart=-1 , wOffsetEnd=-1 ;
  size_t wEffectiveOffsetStart = 0 , wEffectiveOffsetEnd = 0 ;

  utf8VaryingString wEntityStart , wEntityEnd;
  wEntityStart.sprintf("<%s>",pEntity.toString());
  wEntityEnd.sprintf("</%s>",pEntity.toString());
  size_t wESLen=wEntityStart.strlen();
  size_t wEELen=wEntityEnd.strlen();

  pXMLContent = fmtXMLdeclaration();

  ZStatus wSt=rawSeekToPosition(pFd,pOffset);
  if (wSt!=ZS_SUCCESS)
    return wSt;

 // utf8_t* wPtrIn=reinterpret_cast<utf8_t*> (wZDB.Data);
  while ((wOffsetStart<0)&& (!wEof)) {
    wSt=rawReadAt(pFd,wZDB,Payload,wCurrentOffset);
    if (wSt!=ZS_SUCCESS) {
      if ((wSt==ZS_READPARTIAL)||(wSt==ZS_EOF))
          wEof=true;
      else
          return wSt;
    }

    wOffsetStart=wZDB.bsearch(&wEntityStart,0);
    if (wOffsetStart < 0) {
      if (wEof)
          return ZS_NOTFOUND;
      wCurrentOffset += Payload - wEELen ;
    }
  }
  wEffectiveOffsetStart = wCurrentOffset + wOffsetStart;
  wCurrentOffset = wEffectiveOffsetStart;
  ZDataBuffer wZDBContent;
  wZDBContent.setData (wZDB.Data + wOffsetStart , wZDB.Size - wOffsetStart);
  wZDB.setData(wZDBContent);
  wZDBContent.clear();
  wOffsetEnd=wZDB.bsearch(&wEntityEnd,wEELen);
  while ((wOffsetEnd<0) && (!wEof)) {
      wCurrentOffset += wZDB.Size - wEELen ;  /* because end entity might be truncated at the end of buffer */
      wZDBContent.appendData(wZDB.Data,wZDB.Size - wEELen);

      wSt=rawReadAt(pFd,wZDB,Payload,wCurrentOffset);
      if (wSt!=ZS_SUCCESS) {
      if ((wSt==ZS_READPARTIAL)||(wSt==ZS_EOF)) {
          wEof=true;
          wSt=ZS_EOF;
      }
      else
          return wSt;
      }
    wOffsetEnd=wZDB.bsearch(&wEntityEnd,0);
  }
  wZDBContent.appendData(wZDB.Data, wOffsetEnd + wEELen);
  wEffectiveOffsetEnd =  wCurrentOffset + wOffsetEnd + wEELen ;

  pXMLContent.setData((const utf8_t*)wZDBContent.Data,wZDBContent.Size);
  pXMLContent.addConditionalTermination();

  pOffset = wCurrentOffset + wOffsetEnd + wEELen ;
  if (wEof)
    return ZS_EOF ;

  return ZS_SUCCESS;
} // XMLLoadEntity

#endif //ZXMLPRIMITIVES_CPP
