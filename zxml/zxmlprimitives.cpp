#ifndef ZXMLPRIMITIVES_CPP
#define ZXMLPRIMITIVES_CPP

#include <zxml/zxmlprimitives.h>

int cst_XMLIndent = 3;
void setXmlIdent(int pIndent)
{
    cst_XMLIndent = pIndent;
}


std::string fmtXMLnode(const char*pNodeName, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>\n",
            wIndent,' ',pNodeName);
    return std::string(wBuffer);
}
std::string fmtXMLnodeWithAttributes(const char*pNodeName,
                                     const char* pAttName,const char*pAttValue,
                                     const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s %s = \"%s\">\n",
            wIndent,' ',pNodeName,pAttName,pAttValue);
    return std::string(wBuffer);
}
std::string fmtXMLnodeWithAttributes(const char*pNodeName,
                                     const char* pAttName1,const char*pAttValue1,
                                     const char* pAttName2,const char*pAttValue2,
                                     const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s %s = \"%s\" %s = \"%s\" > \n",
            wIndent,' ',pNodeName,pAttName1,pAttValue1,pAttName2,pAttValue2);
    return std::string(wBuffer);
}
std::string fmtXMLendnode(const char*pNodeName, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c</%s>\n",
            wIndent,' ',pNodeName);
    return std::string(wBuffer);
}
std::string fmtXMLchar(const char*pVarName,const char* pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%s</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}

std::string fmtXMLstring(const char*pVarName,utf8String& pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%s</%s>\n",
            wIndent,' ',pVarName,pVarContent.toCChar(),pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLdouble(const char*pVarName,const double pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%g</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLfloat(const char*pVarName,const float pVarContent, const int pLevel)
{
    return fmtXMLdouble(pVarName,double(pVarContent),pLevel);
}
std::string fmtXMLint(const char*pVarName,const int pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%d</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLuint(const char*pVarName,const unsigned int pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%u</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLlong(const char*pVarName,const long pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%ld</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLulong(const char*pVarName,const unsigned long pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%lu</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLint32(const char*pVarName,const int32_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%d</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLuint32(const char*pVarName,const uint32_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%u</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLint64(const char*pVarName,const int64_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%ld</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLuint64(const char*pVarName,const uint64_t pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%lu</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}

std::string fmtXMLintHexa(const char*pVarName,const int pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%Xd</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLuintHexa(const char*pVarName,const unsigned int pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%Xu</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}

std::string fmtXMLlongHexa(const char*pVarName,const long pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%lXd</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLulongHexa(const char*pVarName,const unsigned long pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%lXu</%s>\n",
            wIndent,' ',pVarName,pVarContent,pVarName);
    return std::string(wBuffer);
}
std::string fmtXMLbool(const char*pVarName,const bool pVarContent, const int pLevel)
{
    int wIndent=pLevel*cst_XMLIndent;
    char wBuffer[500];
    memset (wBuffer,0,sizeof(wBuffer));
    sprintf (wBuffer,
            "%*c<%s>%s</%s>\n",
            wIndent,' ',pVarName,pVarContent?"true":"false",pVarName);
    return std::string(wBuffer);
}

int
XMLgetChildText (zxmlElement*pElement,const char* pChildName,utf8String& pTextValue,ZaiErrors* pErrorlog)
{
    zxmlElement*wChild=nullptr;
    ZStatus wSt=pElement->getChildByName((zxmlNode*&)wChild,pChildName);
    if (wSt!=ZS_SUCCESS)
    {
        pErrorlog->logZStatus(ZAIES_Error,wSt,"XMLgetChildText-E-CNTFINDND Error cannot find node element with name <%s> status <%s>",
                       pChildName,
                       decode_ZStatus(wSt));
        return -1;
    }
    wSt=wChild->getNodeText(pTextValue);
    if (wSt!=ZS_SUCCESS)
    {
        pErrorlog->logZStatus(ZAIES_Error,wSt,"XMLgetChildText-E-CNTFINDND Error cannot find text within element <%s> status <%s>",
                       wChild->getName().toCChar(),
                       decode_ZStatus(wSt));
        return -1;
    }
    return 0;
} //XMLgetChildText
int
XMLgetChildFloat (zxmlElement*pElement,const char* pChildName,float& pFloat,ZaiErrors* pErrorlog)
{
    utf8String wValue;

    if (XMLgetChildText(pElement,pChildName,wValue,pErrorlog) <0)
        return -1;
    pFloat=float(wValue.toDouble());
    return 0;
} //XMLgetChildFloat
int
XMLgetChildDouble (zxmlElement*pElement,const char* pChildName,double&pDouble,ZaiErrors* pErrorlog)
{
    utf8String wValue;

    if (XMLgetChildText(pElement,pChildName,wValue,pErrorlog) <0)
        return -1;
    pDouble=wValue.toDouble();
    return 0;
} //XMLgetChildDouble
int
XMLgetChildBool (zxmlElement*pElement,const char* pChildName,bool &pBool,ZaiErrors* pErrorlog)
{
    utf8String wValue;

    if (XMLgetChildText(pElement,pChildName,wValue,pErrorlog) <0)
        return -1;
    wValue.toUpper();
    if (wValue=="TRUE")
        pBool=true;
    else
        pBool=false;
    return 0;
} //XMLgetChildBool
int
XMLgetChildInt (zxmlElement*pElement,const char* pChildName,int &pInt,ZaiErrors* pErrorlog)
{
    utf8String wValue;

    if (XMLgetChildText(pElement,pChildName,wValue,pErrorlog) <0)
        return -1;

    pInt=wValue.toInt();
    return 0;
} //XMLgetChildInt
int
XMLgetChildLong (zxmlElement*pElement,const char* pChildName,long &pLong,ZaiErrors* pErrorlog)
{
    utf8String wValue;

    if (XMLgetChildText(pElement,pChildName,wValue,pErrorlog) <0)
        return -1;

    pLong=wValue.toLong();
    return 0;
} //XMLgetChildInt

#endif //ZXMLPRIMITIVES_CPP
