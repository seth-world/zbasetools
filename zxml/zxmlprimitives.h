#ifndef ZXMLPRIMITIVES_H
#define ZXMLPRIMITIVES_H

#include <ztoolset/zaierrors.h>
#include <zxml/zxml.h>

#ifndef ZXMLPRIMITIVES_CPP
extern int cst_XMLIndent;
#endif


/**
 * @brief fmtXMLnode starts a single xml node with name pNodeName and indentation level pLevel (NB: must be ended with fmtXMLendnode)
 * @param pNodeName
 * @param pLevel generates an indentation defined by (pLevel * cst_XMLIndent) leading spaces
 * @return a std::string
 */
std::string fmtXMLnode(const char *pNodeName, const int pLevel);
/**
 * @brief fmtXMLnodeWitAttribute (and its overloads) starts a single xml node with name pNodeName and indentation level pLevel
 *                          with attribut of name pAttName, and value (text only) pAttValue
 *  Successive overloads add attributes and corresponding values up to 3 attributes and values.
 * (NB: must be ended with fmtXMLendnode)
 * @param pNodeName
 * @param pLevel generates an indentation defined by (pLevel * cst_XMLIndent) leading spaces
 * @param pAttName Name of node's unique attribute
 * @param pAttValue text value of node's attribute
 * @return a std::string
 */
std::string fmtXMLnodeWithAttribute(const char *pNodeName,
                                    const char *pAttName,const char *pAttValue,
                                    const int pLevel);

std::string fmtXMLnodeWithAttribute(const char *pNodeName,
                                     const char *pAttName1,const char *pAttValue1,
                                     const char *pAttName2,const char *pAttValue2,
                                     const int pLevel);

std::string fmtXMLnodeWithAttribute(const char *pNodeName,
                                     const int pLevel,
                                     const char *pAttName1,const char *pAttValue1,
                                     const char *pAttName2,const char *pAttValue2,
                                     const char *pAttName3,const char *pAttValue3);
/**
 * @brief fmtXMLendnode ends xml node @see fmtMXLnode()
 * @param pNodeName
 * @param pLevel generates an indentation defined by (pLevel * cst_XMLIndent) leading spaces
 * @return
 */
std::string fmtXMLendnode(const char *pNodeName, const int pLevel);

/**
 * @brief fmtXMLchar adds a child node named pVarName with content pVarContent with identation pLevel
 * @param pVarName
 * @param pVarContent
 * @param pLevel generates an indentation defined by (pLevel * cst_XMLIndent) leading spaces
 * @return
 */

std::string fmtXMLchar(const char *pVarName, const char *pVarContent, const int pLevel);

std::string fmtXMLstring(const char *pVarName, utf8String &pVarContent, const int pLevel);
std::string fmtXMLdouble(const char *pVarName, const double pVarContent, const int pLevel);
std::string fmtXMLfloat(const char *pVarName, const float pVarContent, const int pLevel);
std::string fmtXMLint(const char *pVarName, const int pVarContent, const int pLevel);
std::string fmtXMLlong(const char *pVarName, const long pVarContent, const int pLevel);

std::string fmtXMLintHexa(const char *pVarName, const int pVarContent, const int pLevel);
std::string fmtXMLlongHexa(const char *pVarName, const long pVarContent, const int pLevel);

std::string fmtXMLbool(const char *pVarName, const bool pVarContent, const int pLevel);

std::string fmtXMLchar(const char *pVarName, const char *pVarContent, const int pLevel);

std::string fmtXMLstring(const char *pVarName, utf8String &pVarContent, const int pLevel);
std::string fmtXMLdouble(const char *pVarName, const double pVarContent, const int pLevel);
std::string fmtXMLfloat(const char *pVarName, const float pVarContent, const int pLevel);
std::string fmtXMLint(const char *pVarName, const int pVarContent, const int pLevel);
std::string fmtXMLuint(const char *pVarName, const unsigned int pVarContent, const int pLevel);
std::string fmtXMLlong(const char *pVarName, const long pVarContent, const int pLevel);
std::string fmtXMLulong(const char *pVarName, const unsigned long pVarContent, const int pLevel);
std::string fmtXMLint32(const char *pVarName, const int32_t pVarContent, const int pLevel);
std::string fmtXMLint64(const char *pVarName, const int64_t pVarContent, const int pLevel);
std::string fmtXMLuint32(const char *pVarName, const uint32_t pVarContent, const int pLevel);
std::string fmtXMLuint64(const char *pVarName, const uint64_t pVarContent, const int pLevel);
std::string fmtXMLbool(const char *pVarName, const bool pVarContent, const int pLevel);

std::string fmtXMLintHexa(const char *pVarName, const int pVarContent, const int pLevel);
std::string fmtXMLuintHexa(const char *pVarName, const uint pVarContent, const int pLevel);
std::string fmtXMLlongHexa(const char *pVarName, const long pVarContent, const int pLevel);
std::string fmtXMLulongHexa(const char *pVarName, const unsigned long pVarContent, const int pLevel);

std::string fmtXMLint32Hexa(const char *pVarName, const int32_t pVarContent, const int pLevel)
{return fmtXMLintHexa(pVarName, pVarContent, pLevel);}
std::string fmtXMLuint32Hexa(const char *pVarName, const uint32_t pVarContent, const int pLevel)
{return fmtXMLuintHexa(pVarName, pVarContent, pLevel);}
std::string fmtXMLint64Hexa(const char *pVarName, const int64_t pVarContent, const int pLevel)
{return fmtXMLlongHexa(pVarName, pVarContent, pLevel);}
std::string fmtXMLuint64Hexa(const char *pVarName, const uint64_t pVarContent, const int pLevel)
{return fmtXMLulongHexa(pVarName, pVarContent, pLevel);}

int XMLgetChildText(zxmlElement *pElement,
                    const char *pChildName,
                    utf8String &pTextValue,
                    ZaiErrors *pErrorlog);
int XMLgetChildFloat(zxmlElement *pElement,
                     const char *pChildName,
                     float &pFloat,
                     ZaiErrors *pErrorlog);
int XMLgetChildDouble(zxmlElement *pElement,
                      const char *pChildName,
                      double &pDouble,
                      ZaiErrors *pErrorlog);
int XMLgetChildBool(zxmlElement *pElement,
                    const char *pChildName,
                    bool &pBool,
                    ZaiErrors *pErrorlog);

int XMLgetChildInt(zxmlElement *pElement, const char *pChildName, int &pInt, ZaiErrors *pErrorlog);
int XMLgetChildLong(zxmlElement *pElement, const char *pChildName, long &pLong, ZaiErrors *pErrorlog);

int XMLgetChildIntHexa(zxmlElement *pElement, const char *pChildName, int &pLong, ZaiErrors *pErrorlog);
int XMLgetChildLongHexa(zxmlElement *pElement, const char *pChildName, long &pLong, ZaiErrors *pErrorlog);
int XMLgetChildUIntHexa(zxmlElement *pElement, const char *pChildName, unsigned int &pLong, ZaiErrors *pErrorlog);
int XMLgetChildULongHexa(zxmlElement *pElement, const char *pChildName, unsigned long &pLong, ZaiErrors *pErrorlog);

int XMLgetChildInt32Hexa(zxmlElement *pElement, const char *pChildName, int32_t &pInt32, ZaiErrors *pErrorlog)
{
    return XMLgetChildIntHexa(pElement,pChildName,pInt32,pErrorlog);
}

int XMLgetChildInt64Hexa(zxmlElement *pElement, const char *pChildName, int64_t &pLong, ZaiErrors *pErrorlog)
{
    return XMLgetChildLongHexa(pElement,pChildName,pLong,pErrorlog);
}

int XMLgetChildUInt32Hexa(zxmlElement *pElement, const char *pChildName, uint32_t &pUInt32, ZaiErrors *pErrorlog)
{
    return XMLgetChildUIntHexa(pElement,pChildName,pUInt32,pErrorlog);
}
int XMLgetChildUInt64Hexa(zxmlElement *pElement, const char *pChildName, uint64_t &pULong, ZaiErrors *pErrorlog)
{
    return XMLgetChildULongHexa(pElement,pChildName,pULong,pErrorlog);
}

/**
 * @brief setXmlIdent sets the indentation for xml code generation to pIdent
 * @param pIndent
 */
void setXmlIdent(int pIndent);

#endif // ZXMLPRIMITIVES_H