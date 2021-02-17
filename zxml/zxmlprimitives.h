#ifndef ZXMLPRIMITIVES_H
#define ZXMLPRIMITIVES_H
#include <assert.h>
#include <ztoolset/zaierrors.h>
#include <zxml/zxml.h>
#include <stdint-gcc.h>
#include <stdint.h>
#include <sys/types.h>
#ifndef ZXMLPRIMITIVES_CPP
extern int cst_XMLIndent;
#endif


/**
 * @brief XMLsearchForChildTag recursively scans xml tree (children and sibling nodes) starting at node pTopNode
 *  for a specific node with name equal to pTag
 *  and returns the first node found within xml sub-tree that has name equal to tag or returns nullptr if not found.
 * @param pTopNode top level node to start search from : NB: this node is not deregistrered by the routine.
 * Returned node must be deregistrated.
 */
zxmlNode* XMLsearchForChildTag(zxmlNode* pTopNode, const char* pTag);

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
std::string fmtXMLnodeWithAttributes(const char *pNodeName,
                                    const char *pAttName,const char *pAttValue,
                                    const int pLevel);

std::string fmtXMLnodeWithAttributes(const char *pNodeName,
                                     const char *pAttName1,const char *pAttValue1,
                                     const char *pAttName2,const char *pAttValue2,
                                     const int pLevel);

std::string fmtXMLnodeWithAttributes(const char *pNodeName,
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

std::string fmtXMLcomment(const char *pContent, const int pLevel);


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
/*
std::string fmtXMLint32Hexa(const char *pVarName, const int32_t pVarContent, const int pLevel)
{return fmtXMLintHexa(pVarName, pVarContent, pLevel);}
std::string fmtXMLuint32Hexa(const char *pVarName, const uint32_t pVarContent, const int pLevel)
{return fmtXMLuintHexa(pVarName, pVarContent, pLevel);}
std::string fmtXMLint64Hexa(const char *pVarName, const int64_t pVarContent, const int pLevel)
{return fmtXMLlongHexa(pVarName, pVarContent, pLevel);}
std::string fmtXMLuint64Hexa(const char *pVarName, const uint64_t pVarContent, const int pLevel)
{return fmtXMLulongHexa(pVarName, pVarContent, pLevel);}
*/

#if (SYSTEM32)

#else
#define fmtXMLint32Hexa fmtXMLintHexa
#define fmtXMLuint32Hexa fmtXMLuintHexa
#define fmtXMLint64Hexa fmtXMLlongHexa
#define fmtXMLuint64Hexa fmtXMLulongHexa

#endif



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
int XMLgetChildUInt(zxmlElement *pElement, const char *pChildName, unsigned int &pInt, ZaiErrors *pErrorlog);
int XMLgetChildLong(zxmlElement *pElement, const char *pChildName, long &pLong, ZaiErrors *pErrorlog);
int XMLgetChildULong(zxmlElement *pElement, const char *pChildName, unsigned long &pLong, ZaiErrors *pErrorlog);




int XMLgetChildIntHexa(zxmlElement *pElement, const char *pChildName, int &pLong, ZaiErrors *pErrorlog);
int XMLgetChildLongHexa(zxmlElement *pElement, const char *pChildName, long &pLong, ZaiErrors *pErrorlog);
int XMLgetChildUIntHexa(zxmlElement *pElement, const char *pChildName, unsigned int &pLong, ZaiErrors *pErrorlog);
int XMLgetChildULongHexa(zxmlElement *pElement, const char *pChildName, unsigned long &pLong, ZaiErrors *pErrorlog);

#define XMLgetChildInt32 XMLgetChildInt
#define XMLgetChildUInt32 XMLgetChildUInt
#define XMLgetChildInt64 XMLgetChildLong
#define XMLgetChildUInt64 XMLgetChildULong

#define XMLgetChildInt32Hexa XMLgetChildIntHexa
#define XMLgetChildUInt32Hexa XMLgetChildUIntHexa
#define XMLgetChildInt64Hexa XMLgetChildLongHexa
#define XMLgetChildUInt64Hexa XMLgetChildULongHexa



#if (SYSTEM32)
#define XMLgetChildInt32 XMLgetChildLong
#define XMLgetChildUInt32 XMLgetChildULong
#define XMLgetChildInt64 XMLgetChildLongLong
#define XMLgetChildUInt64 XMLgetChildULongLong

#define XMLgetChildInt32Hexa XMLgetChildLongHexa
#define XMLgetChildUInt32Hexa XMLgetChildULongHexa
#define XMLgetChildInt64Hexa XMLgetChildLongLongHexa
#define XMLgetChildUInt64Hexa XMLgetChildULongLongHexa
#else
#define XMLgetChildInt32 XMLgetChildInt
#define XMLgetChildUInt32 XMLgetChildUInt
#define XMLgetChildInt64 XMLgetChildLong
#define XMLgetChildUInt64 XMLgetChildULong

#define XMLgetChildInt32Hexa XMLgetChildIntHexa
#define XMLgetChildUInt32Hexa XMLgetChildUIntHexa
#define XMLgetChildInt64Hexa XMLgetChildLongHexa
#define XMLgetChildUInt64Hexa XMLgetChildULongHexa

#endif


/**
 * @brief setXmlIdent sets the indentation for xml code generation to pIdent
 * @param pIndent
 */
void setXmlIdent(int pIndent);

#endif // ZXMLPRIMITIVES_H
