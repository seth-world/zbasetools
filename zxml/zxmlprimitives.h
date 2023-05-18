#ifndef ZXMLPRIMITIVES_H
#define ZXMLPRIMITIVES_H
#include <assert.h>
#include <ztoolset/zaierrors.h>
#include <zxml/zxml.h>
//#include <stdint-gcc.h>
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
zxmlNode* XMLsearchForChildTag(zxmlNode* pTopNode, const utf8VaryingString &pTag);

#define XMLsearchForNamedChild  XMLsearchForChildTag

/**
 * @brief XMLhasNamedChild search within pTopNode's children if there is at least one child with name pName.
 * @return  true if there is at least one child with name pName or false if not
 */
bool XMLhasNamedChild(zxmlNode* pTopNode, const utf8VaryingString &pName);

/**
 * @brief fmtXMLdeclaration adds an xml declaration <?xml version=... to file.
 * Indeed this must be the first content of xml text file, and indeed there is no level to that declaration.
 * NB: must not be ended by any declaration.
 * @param pEncoding xml text file encoding. Defaulted to UTF-8. No check is made on encoding code string validity.
 * @return an utf8VaryingString with xml declaration
 */
utf8VaryingString fmtXMLdeclaration(const utf8VaryingString& pEncoding="UTF-8");


/**
 * @brief fmtXMLversion generates an Xml root declaration with Main root name and version number
 * @param pName         Main root node name
 * @param pFullVersion  an unsigned long containing Version number, release major and release minor packed in appropriate way.
 * @param pLevel        generates a space indentation defined by (pLevel * cst_XMLIndent) leading spaces. Generally 0 (default)
 * @return an utf8VaryingString with corresponding xml code
 */
utf8VaryingString fmtXMLmainVersion(const utf8VaryingString& pName, unsigned long pFullVersion, const int pLevel=0);


/**
 * @brief fmtXMLnode starts a single xml node with name pNodeName and indentation level pLevel (NB: must be ended with fmtXMLendnode)
 * @param pNodeName
 * @param pLevel generates an indentation defined by (pLevel * cst_XMLIndent) leading spaces
 * @return a utf8VaryingString
 */
utf8VaryingString fmtXMLnode(const utf8VaryingString &pNodeName, const int pLevel);
/**
 * @brief fmtXMLnodeWitAttribute (and its overloads) starts a single xml node with name pNodeName and indentation level pLevel
 *                          with attribut of name pAttName, and value (text only) pAttValue
 *  Successive overloads add attributes and corresponding values up to 3 attributes and values.
 * (NB: must be ended with fmtXMLendnode)
 * @param pNodeName
 * @param pLevel generates a space indentation defined by (pLevel * cst_XMLIndent) leading spaces
 * @param pAttName Name of node's unique attribute
 * @param pAttValue text value of node's attribute
 * @return a utf8VaryingString
 */
utf8VaryingString fmtXMLnodeWithAttributes(const utf8VaryingString &pNodeName,
                                    const utf8VaryingString &pAttName, const utf8VaryingString &pAttValue,
                                    const int pLevel);

utf8VaryingString fmtXMLnodeWithAttributes(const utf8VaryingString &pNodeName,
                                     const utf8VaryingString &pAttName1, const utf8VaryingString &pAttValue1,
                                     const utf8VaryingString &pAttName2, const utf8VaryingString &pAttValue2,
                                     const int pLevel);

utf8VaryingString fmtXMLnodeWithAttributes(const utf8VaryingString &pNodeName,
                                     const utf8VaryingString &pAttName1, const utf8VaryingString &pAttValue1,
                                     const utf8VaryingString &pAttName2, const utf8VaryingString &pAttValue2,
                                     const utf8VaryingString &pAttName3, const utf8VaryingString &pAttValue3,
                                     const int pLevel);
/**
 * @brief fmtXMLendnode ends xml node @see fmtMXLnode()
 * @param pNodeName
 * @param pLevel generates an indentation defined by (pLevel * cst_XMLIndent) leading spaces
 * @return
 */
utf8VaryingString fmtXMLendnode(const utf8VaryingString &pNodeName, const int pLevel);

/**
 * @brief fmtXMLchar adds a child node named pVarName with content pVarContent with identation pLevel
 * @param pVarName
 * @param pVarContent
 * @param pLevel generates an indentation defined by (pLevel * cst_XMLIndent) leading spaces
 * @return
 */

utf8VaryingString fmtXMLchar(const utf8VaryingString& pVarName, const utf8VaryingString& pVarContent, const int pLevel);

utf8VaryingString fmtXMLcomment(const utf8VaryingString &pContent, const int pLevel);

/** @brief fmtXMLaddInlineComment() adds an xml comment pContent at the end of previous xml line */
utf8VaryingString &fmtXMLaddInlineComment(utf8VaryingString &pXmlString, const utf8VaryingString &pComment);

utf8VaryingString fmtXMLchar(const utf8VaryingString &pVarName, const utf8VaryingString &pVarContent, const int pLevel);

utf8VaryingString fmtXMLstring(const utf8VaryingString &pVarName, utf8String &pVarContent, const int pLevel);
utf8VaryingString fmtXMLdouble(const utf8VaryingString &pVarName, const double pVarContent, const int pLevel);
utf8VaryingString fmtXMLfloat(const utf8VaryingString &pVarName, const float pVarContent, const int pLevel);
utf8VaryingString fmtXMLint(const utf8VaryingString &pVarName, const int pVarContent, const int pLevel);
utf8VaryingString fmtXMLuint(const utf8VaryingString &pVarName, const unsigned int pVarContent, const int pLevel);
utf8VaryingString fmtXMLlong(const utf8VaryingString &pVarName, const long pVarContent, const int pLevel);
utf8VaryingString fmtXMLulong(const utf8VaryingString &pVarName, const unsigned long pVarContent, const int pLevel);
utf8VaryingString fmtXMLint32(const utf8VaryingString &pVarName, const int32_t pVarContent, const int pLevel);
utf8VaryingString fmtXMLint64(const utf8VaryingString &pVarName, const int64_t pVarContent, const int pLevel);
utf8VaryingString fmtXMLuint32(const utf8VaryingString &pVarName, const uint32_t pVarContent, const int pLevel);
utf8VaryingString fmtXMLuint64(const utf8VaryingString &pVarName, const uint64_t pVarContent, const int pLevel);
utf8VaryingString fmtXMLbool(const utf8VaryingString &pVarName, const bool pVarContent, const int pLevel);
// Deprecated
//utf8VaryingString fmtXMLdate(const utf8VaryingString &pVarName, const ZDate pVarContent, const int pLevel);
utf8VaryingString fmtXMLdatefull(const utf8VaryingString &pVarName, const ZDateFull pVarContent, const int pLevel);

utf8VaryingString fmtXMLintHexa(const utf8VaryingString &pVarName, const int pVarContent, const int pLevel);
utf8VaryingString fmtXMLuintHexa(const utf8VaryingString &pVarName, const unsigned int pVarContent, const int pLevel);
utf8VaryingString fmtXMLlongHexa(const utf8VaryingString &pVarName, const long pVarContent, const int pLevel);
utf8VaryingString fmtXMLulongHexa(const utf8VaryingString &pVarName, const unsigned long pVarContent, const int pLevel);


utf8VaryingString fmtXMLmd5(const utf8VaryingString &pVarName, const md5& pVarContent, const int pLevel);
utf8VaryingString fmtXMLcheckSum(const utf8VaryingString &pVarName, const checkSum& pVarContent, const int pLevel);

utf8VaryingString fmtXMLSSLKeyB64(const utf8VaryingString &pVarName, const zbs::ZCryptKeyAES256& pVarContent, const int pLevel);
utf8VaryingString fmtXMLSSLVectorB64(const utf8VaryingString &pVarName, const zbs::ZCryptVectorAES256& pVarContent, const int pLevel);


utf8VaryingString fmtXMLversion(const utf8VaryingString& pName, unsigned long pFullVersion, const int pLevel=0);

/*
utf8VaryingString fmtXMLint32Hexa(const char *pVarName, const int32_t pVarContent, const int pLevel)
{return fmtXMLintHexa(pVarName, pVarContent, pLevel);}
utf8VaryingString fmtXMLuint32Hexa(const char *pVarName, const uint32_t pVarContent, const int pLevel)
{return fmtXMLuintHexa(pVarName, pVarContent, pLevel);}
utf8VaryingString fmtXMLint64Hexa(const char *pVarName, const int64_t pVarContent, const int pLevel)
{return fmtXMLlongHexa(pVarName, pVarContent, pLevel);}
utf8VaryingString fmtXMLuint64Hexa(const char *pVarName, const uint64_t pVarContent, const int pLevel)
{return fmtXMLulongHexa(pVarName, pVarContent, pLevel);}
*/

#if (SYSTEM32)

#else
#define fmtXMLint32Hexa fmtXMLintHexa
#define fmtXMLuint32Hexa fmtXMLuintHexa
#define fmtXMLint64Hexa fmtXMLlongHexa
#define fmtXMLuint64Hexa fmtXMLulongHexa

#endif


/**
 * @brief XMLgetChildText
 * @param pElement
 * @param pChildName
 * @param pTextValue
 * @param pErrorlog
 * @param pSeverity
 * @return return 0 if successfull or -1 if errored.
 */
ZStatus XMLgetChildText(zxmlElement *pElement,
                        const utf8VaryingString &pChildName,
                        utf8String &pTextValue,
                        ZaiErrors *pErrorlog,
                        ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus XMLgetChildText(zxmlElement *pElement,
                        const utf8VaryingString &pChildName,
                        utf8String &pTextValue);

ZStatus XMLgetChildFloat(zxmlElement *pElement,
                        const utf8VaryingString &pChildName,
                        float &pFloat);

ZStatus XMLgetChildFloat(zxmlElement *pElement,
                     const utf8VaryingString &pChildName,
                     float &pFloat,
                     ZaiErrors *pErrorlog,
                     ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus XMLgetChildDouble(zxmlElement *pElement,
    const utf8VaryingString &pChildName,
    double &pDouble);

ZStatus XMLgetChildDouble(zxmlElement *pElement,
                      const utf8VaryingString &pChildName,
                      double &pDouble,
                      ZaiErrors *pErrorlog,
                      ZaiE_Severity pSeverity=ZAIES_Error);

ZStatus
XMLgetChildBool (zxmlElement*pElement,const utf8VaryingString &pChildName,bool &pBool);

ZStatus XMLgetChildBool(zxmlElement *pElement,
                    const utf8VaryingString &pChildName,
                    bool &pBool,
                    ZaiErrors *pErrorlog,
                    ZaiE_Severity pSeverity=ZAIES_Error);

ZStatus XMLgetChildInt (zxmlElement*pElement,const utf8VaryingString & pChildName,int &pInt);
ZStatus XMLgetChildUInt(zxmlElement *pElement, const utf8VaryingString &pChildName, unsigned int &pUInt);
ZStatus XMLgetChildLong(zxmlElement *pElement, const utf8VaryingString &pChildName, long &pLong);
ZStatus XMLgetChildULong(zxmlElement *pElement, const utf8VaryingString &pChildName, unsigned long &pULong);

ZStatus XMLgetChildInt64 (zxmlElement*pElement,const utf8VaryingString & pChildName,int64_t &pLong);
ZStatus XMLgetChildInt64 (zxmlElement*pElement,const utf8VaryingString & pChildName,int64_t &pLong,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity);

ZStatus XMLgetChildVersion (zxmlElement*pElement,const utf8VaryingString & pChildName, unsigned long &pVersion);

/* Deprecated */
//ZStatus XMLgetChildZDate(zxmlElement *pElement, const utf8VaryingString& pChildName, ZDate &pDate);
ZStatus XMLgetChildZDateFull(zxmlElement *pElement, const utf8VaryingString& pChildName, ZDateFull &pDateFull);


ZStatus XMLgetChildIntHexa(zxmlElement *pElement, const utf8VaryingString &pChildName, int &pLong);
ZStatus XMLgetChildLongHexa(zxmlElement *pElement, const utf8VaryingString &pChildName, long &pLong);
ZStatus XMLgetChildUIntHexa(zxmlElement *pElement, const utf8VaryingString &pChildName, unsigned int &pLong);
ZStatus XMLgetChildULongHexa(zxmlElement *pElement, const utf8VaryingString &pChildName, unsigned long &pLong);



ZStatus XMLgetChildInt(zxmlElement *pElement, const utf8VaryingString &ChildName, int &pInt, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus XMLgetChildUInt(zxmlElement *pElement, const utf8VaryingString &pChildName, unsigned int &pUInt, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus XMLgetChildLong(zxmlElement *pElement, const utf8VaryingString &pChildName, long &pLong, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus XMLgetChildULong(zxmlElement *pElement, const utf8VaryingString &pChildName, unsigned long &pULong, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus XMLgetChildVersion(zxmlElement *pElement, const utf8VaryingString& pChildName, unsigned long &pVersion, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);

/* Deprecated */
//ZStatus XMLgetChildZDate(zxmlElement *pElement, const utf8VaryingString& pChildName, ZDate &pDate, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus XMLgetChildZDateFull(zxmlElement *pElement, const utf8VaryingString& pChildName, ZDateFull &pDateFull, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);


ZStatus XMLgetChildIntHexa(zxmlElement *pElement, const utf8VaryingString &pChildName, int &pLong, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus XMLgetChildLongHexa(zxmlElement *pElement, const utf8VaryingString &pChildName, long &pLong, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus XMLgetChildUIntHexa(zxmlElement *pElement, const utf8VaryingString &pChildName, unsigned int &pLong, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);
ZStatus XMLgetChildULongHexa(zxmlElement *pElement, const utf8VaryingString &pChildName, unsigned long &pLong, ZaiErrors *pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);

#define fmtXMLZStatus fmtXMLint64

#define XMLgetChildInt32 XMLgetChildInt
#define XMLgetChildUInt32 XMLgetChildUInt
#define XMLgetChildUInt64 XMLgetChildULong
#define XMLgetChildZStatus XMLgetChildInt64

#define XMLgetChildInt32Hexa XMLgetChildIntHexa
#define XMLgetChildUInt32Hexa XMLgetChildUIntHexa
#define XMLgetChildInt64Hexa XMLgetChildLongHexa
#define XMLgetChildUInt64Hexa XMLgetChildULongHexa



#if (SYSTEM32)
#define XMLgetChildInt32 XMLgetChildLong
#define XMLgetChildUInt32 XMLgetChildULong
//#define XMLgetChildInt64 XMLgetChildLongLong
#define XMLgetChildUInt64 XMLgetChildULongLong

#define XMLgetChildInt32Hexa XMLgetChildLongHexa
#define XMLgetChildUInt32Hexa XMLgetChildULongHexa
#define XMLgetChildInt64Hexa XMLgetChildLongLongHexa
#define XMLgetChildUInt64Hexa XMLgetChildULongLongHexa
#else
#define XMLgetChildInt32 XMLgetChildInt
#define XMLgetChildUInt32 XMLgetChildUInt
//#define XMLgetChildInt64 XMLgetChildLong
#define XMLgetChildUInt64 XMLgetChildULong

#define XMLgetChildInt32Hexa XMLgetChildIntHexa
#define XMLgetChildUInt32Hexa XMLgetChildUIntHexa
#define XMLgetChildInt64Hexa XMLgetChildLongHexa
#define XMLgetChildUInt64Hexa XMLgetChildULongHexa

#endif

ZStatus XMLgetChildMd5(zxmlElement *pElement, const utf8VaryingString& pChildName,md5 &pMd5);

ZStatus
XMLgetChildMd5 (zxmlElement*pElement,const utf8VaryingString &pChildName,md5 &pMd5,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);

ZStatus XMLgetChildCheckSum(zxmlElement *pElement, const utf8VaryingString& pChildName,checkSum &pCheckSum);

ZStatus
XMLgetChildCheckSum (zxmlElement*pElement,const utf8VaryingString &pChildName,checkSum &pCheckSum,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);

ZStatus
XMLgetChildSSLKeyB64 (zxmlElement*pElement, const utf8VaryingString &pChildName, zbs::ZCryptKeyAES256 &pKey);

ZStatus
XMLgetChildSSLKeyB64 (zxmlElement*pElement, const utf8VaryingString &pChildName, zbs::ZCryptKeyAES256 &pKey, ZaiErrors* pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);

ZStatus
XMLgetChildSSLVectorB64 (zxmlElement*pElement,const utf8VaryingString &pChildName,zbs::ZCryptVectorAES256 &pKey);

ZStatus
XMLgetChildSSLVectorB64 (zxmlElement*pElement,const utf8VaryingString &pChildName,zbs::ZCryptVectorAES256 &pKey,ZaiErrors* pErrorlog, ZaiE_Severity pSeverity=ZAIES_Error);



/**
 * @brief setXmlIdent sets the indentation for xml code generation to pIdent
 * @param pIndent
 */
void setXmlIdent(int pIndent);

/**
 * @brief suppressLF  eliminates the first encountered character '\n' (LF) from the given string
 */
utf8VaryingString& suppressLF(utf8VaryingString& pString);

#endif // ZXMLPRIMITIVES_H
