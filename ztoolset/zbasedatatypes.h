#ifndef ZBASEDATATYPES_H
#define ZBASEDATATYPES_H
#include <iostream>

#include <QDate>
#include <openssl/evp.h>

#include <ztoolset/zerror.h>

//#include <sys/time.h>
#include <time.h>

#include <type_traits>
#include <ztoolset/ztoolset_common.h>
#include <ztoolset/zdatabuffer.h>
//#include <ztoolset/uristring.h>
#include <ztoolset/ztime.h>
//#include <ztoolset/zdate.h>
#include <ztoolset/zlimit.h>

#include <ztoolset/ztypetype.h> // for typeDemangler() function

//---------Constants-----------------------
//

//constexpr int cst_codelen       = 25 ; // see __CODESTRING__
//constexpr int cst_checksum      = 32;  // for SHA256 - Warning : must be changed if SHA512   see __CHECKSUM__
//constexpr int cst_checksumHex   = cst_checksum*2 ; // see __CHECKSUM__
constexpr int cst_longcodelen   = 50;
//constexpr int cst_desclen       = 150 ; // see __DESCSTRING__
//constexpr int cst_urilen        = 255; // see __URISTRING__
//constexpr int cst_identity      = 255;
constexpr int cst_datelen       = 8;
constexpr int cst_fulldatelen   = 14;
//constexpr int cst_messagelen    = 512;

constexpr int cst_dateformat_len    = 20;
constexpr int cst_dateprint_len     = 20;
constexpr const int cst_headername = 50;
constexpr int cst_headerversion = 10;

const int cst_datetimeformatlen = 50;

/*
#ifndef __DESCSTRING__
#define __DESCSTRING__

static constexpr int cst_desclen       = 150  ;
    typedef templateString<char[cst_desclen+1]> descString;

#endif// __DESCSTRING__

#ifndef __CODESTRING__
#define __CODESTRING__

static constexpr int cst_codelen       = 25  ;
    typedef templateString<char[cst_codelen+1]> codeString;

#endif// __CODESTRING__
//typedef templateString<char [cst_codelen+1]> codeString;
*/

//
//------------Utility routines : String search -----------------
//


struct ZHamming_struct {
    long Size_Delta ; /*! difference of size between the 2 strings */
    long Distance ; /*! Hamming distance calculated on the shortest string */
};

struct ZHamFloat_struct {
    long Size_Delta ; /*! difference of size between the 2 strings */
    double Distance ; /*! Hamming distance calculated on the shortest string and set to string length*/
};
#ifndef __DEFAULTDELIMITERSET__
#define __DEFAULTDELIMITERSET__  " \t\n\r"

const char* _firstNotinSet(const char*pString, const char *pSet=__DEFAULTDELIMITERSET__);
const char* _firstinSet (const char*pString,const char *pSet=__DEFAULTDELIMITERSET__);
const char* _lastinSet (const char*pString,const char *pSet=__DEFAULTDELIMITERSET__);
const char* _strchrReverse(const char *pStr,const char pChar);

char* _LTrim (char*pString, const char *pSet=__DEFAULTDELIMITERSET__);
char* _RTrim (char*pString, const char *pSet=__DEFAULTDELIMITERSET__);
char* _Trim (char*pString, const char *pSet=__DEFAULTDELIMITERSET__);

char* _toUpper(char *pStr,char *pOutStr=nullptr);
char* _expurgeSet(char *pString, const char *pSet=__DEFAULTDELIMITERSET__);
char* _expurgeString(char *pString, const char *pSubString);
#endif

ZHamFloat_struct _getHamFloat (const char *pStr1, const char *pStr2);
ZHamming_struct _getHamming (const char *pStr1, const char *pStr2);


//-----------------End Utility routines : String search -----------------



//
//------------------Main Data code and structures----------------------------
//


#ifndef __ZCRYPTMETHOD_TYPE__
#define __ZCRYPTMETHOD_TYPE__
enum ZCryptMethod_type : uint8_t
{
    ZCM_Uncrypted       = 0,
    ZCM_Base64          = 1,
    ZCM_AES256          = 2,
    ZCM_SHA256          = 3
};
#endif // __ZCRYPTMETHOD_TYPE__

const char *
decode_ZCM (ZCryptMethod_type pZCM);

ZCryptMethod_type
encode_ZCM(const char *pZCMStr);



int rulerSetup (ZDataBuffer &pRulerHexa, ZDataBuffer &pRulerAscii,int pColumn);
class ZDBiterator
{
public:
    unsigned char* Data ;
    ZDBiterator (ZDataBuffer &ZDB) {Data = ZDB.Data;}

};


enum SELDate_type
{
    SEL_Nothing = 0,
    SEL_Due,
    SEL_Start,
    SEL_Completed

};
#ifdef __COMMENT__
#ifndef __URISTAT__
#define __URISTAT__

//#include <QUrl>
//
//! \brief The uriStat_struct struct stores the statistics about uriString file
//
struct uriStat_struct {
    char        Dev[50];        //!< device
    ZDateFull   Created;        //!< creation date time
    ZDateFull   LastModified;   //!< last modification date time
    int         Rights;         //!< rights

    long long   Size;           //!< file size
    char        Owner[150];     //!< Owner name

    uriStat_struct (void) {memset(this,0,sizeof(uriStat_struct));}
    void clear(void) {memset(this,0,sizeof(uriStat_struct));}
};
#endif // __URISTATS__

#endif // __COMMENT__




#ifdef __COMMENT__
struct uriString {
    char content[cst_urilen+1];

    uriString(const char *pString) {fromString(pString);return;}
    uriString(const uriString &pURI) {fromURI(pURI);return;}
    uriString (void) {memset(content,0,sizeof(content));}


    uriString & clear(void) {memset(content,0,sizeof(content)); return *this;}
    bool isEmpty(void) {return (content[0]=='\0');}
    size_t size(void) {return (strlen(content)); }

    uriString &fromURI(const uriString &pURI) ;
    uriString &fromURI(const uriString *pURI);

    uriString &fromString(const char *pString) ;

    uriString &add(const char *pFormat,...);

#ifdef QT_CORE_LIB

    uriString &fromQString(QString pQString);
//    void fromDocid(const Docid_struct &pDocid);

    uriString &fromQUrl(QUrl &pUrl);
    uriString &fromQByteArray(QByteArray&pQString);

    QString toQString(void) ;

#endif //  QT_CORE_LIB

    uriString& fromdescString(const descString &pString);

    static uriString getHomeDir(void) ;

    descString getFileExtension (void) ;
    descString getDirectoryPath (void) ;
    descString getBasename (void) ;
    descString getRootBasename (void);

    uriString &setDirectoryPath(uriString &pDirectoryPath);

    bool        isDirectory (void);
    bool        isRegularFile(void);


    char * toString(void) ;

// ---------------Mime------------------
    bool addMimeFileExtension (descString pMimeName);
    bool addMimeFileExtension (descString *pMimeName) {return(addMimeFileExtension(*pMimeName));}

// -----Control & file operations-------
//

    ZStatus getStatR(uriStat_struct &pZStat);

    long long getFileSize(void) ;

    checkSum getChecksum(void);

    ZStatus loadContent(ZDataBuffer &pDBS) ;
    ZStatus writeContent (ZDataBuffer &pDBS) ;

    bool    exists(void)  ;
    ZStatus suppress(void)  ;


//
//-----------String handling and substring search---------------
//
    int Substr(const char *pSub, int pOffset=0) ;
    int Substr(const char pSub, int pOffset);
    int SubstrCase(const char *pSub, int pOffset=0) ;
    uriString Left (int pLen);
    uriString Right (int pLen);

    uriString& RTrim (const char *pSet=" ");
    uriString& LTrim (const char *pSet=" ");
    uriString& Trim (const char *pSet=" ");

    const char * firstNotinSet (const char *pSet);
//
//    Adding Delimiters
//
    uriString &addConditionalDirectoryDelimiter(void) ;
    uriString &addDirectoryDelimiter(void) ;
// ---------edition----------------

    void
    sprintf (const char *pFormat,...) {
                                        va_list args;
                                       va_start (args, pFormat);
                                      vsprintf (content ,pFormat, args);
                                      va_end(args);
                                       return;
                                       }
//------------------operator = overload ------------------------------------------------
    uriString& operator = (const char *pString) {fromString((const char *)pString);return(*this);}
//    uriString& operator = (const char *pString) {fromString(pString);return(*this);}
//    uriString& operator = (const QString pString) {fromQString(pString);return(*this);}

//    uriString& operator = (const QString *pString) {fromQString(pString);return(*this);}
    uriString& operator = (QByteArray pString) {fromQByteArray(pString);return(*this);}
    uriString& operator = (const descString &pString) {fromdescString(pString);return(*this);}

#ifdef QT_CORE_LIB
    uriString& operator = (QUrl &pUrl) {fromQUrl(pUrl);return(*this);}
    uriString& operator = (QString pString) {fromQString(pString);return(*this);}
    uriString& operator += (const QString &pQString) {strcat (content,pQString.toUtf8());return(*this);}


#endif // QT

    uriString& operator = (const uriString& pURI) {memmove (this,&pURI,sizeof(uriString)); return (*this);}

    uriString& operator += (const char *pString)
                                    {size_t wL=strlen(pString);
                                    if ((wL+size())>cst_urilen ) wL=cst_urilen-size();
                                    strncat(content,pString,wL);
                                    return(*this);}

    uriString& operator += (const uriString &pURI) {strcat (content,pURI.content);return(*this);}
    uriString& operator += (const descString &pDesc) {strcat (content,pDesc.content);return(*this);}

    friend ZStatus operator << (ZDataBuffer& pDataBuffer,uriString &pURI) {return(pURI.loadContent(pDataBuffer));}
    friend ZStatus operator << (uriString& pURI, ZDataBuffer& pDataBuffer) {return(pURI.writeContent(pDataBuffer));}
};
#endif // __COMMENT___

//----------- global operators  << -------------------------------------------------------------------

#ifndef __URISTRING_PUSH_OPERATOR__
#define __URISTRING_PUSH_OPERATOR__

//ZStatus operator << (uriString& pURI, ZDataBuffer& pDataBuffer) {return(pURI.loadContent(pDataBuffer));}
#endif // __URISTRING_PUSH_OPERATOR__





#ifdef __COMMENT__
struct descString : public templateString<char [cst_desclen+1]>
{
    char content[cst_desclen+1];
    size_t size(void) {return (strlen(content)); }
    size_t maxSize(void) {return sizeof(content);}

    descString& fromString(char *pString) {memset (content,0,cst_desclen+1);
                                      if (strlen(pString)>cst_desclen)
                                                    {
                                                    strncpy(content,pString,cst_desclen);
                                                    content[cst_desclen]='\0';
                                                    }
                                      strcpy(content,pString);
                                      return *this;}

    descString& fromQString(const QString pQString){memset (content,0,cst_desclen+1);
                                                if (pQString.size()>cst_desclen)
                                                              strncpy(content,pQString.toUtf8(),cst_desclen);
                                                strcpy(content,pQString.toUtf8());
                                                return *this;}
    descString& fromcodeString(const descString &pString) {memmove(content,pString.content,cst_desclen+1); return *this;}

    QString toQString(void) {QString wRet = content; return wRet;}
    char * toString(void) {return(content);}

    bool isEmpty(void) {return (content[0]=='\0');}
    void clear(void) {memset(content,0,cst_desclen+1);}

    descString& eliminateChar (char pChar)
    {
        char wChar[1] ;
        wChar[0] =  pChar;
        char* wPtr = content ;
        ssize_t wS = size();
        while (wPtr[0]!='\0')
        {
            if (wPtr[0]==wChar[0])
                    {
                    memmove(wPtr,wPtr+1,wS);
                    wS--;
                    continue;
                    }
            wS--;
            wPtr++;
        }//while
        return *this;
    }//eliminateChar

    descString &add(const char *pFormat,...)
    {
        decltype (content) wBuf;
        va_list args;
        va_start (args, pFormat);
        vsprintf (wBuf ,pFormat, args);
        va_end(args);
    size_t wSize = strlen (wBuf);
        if (wSize+strlen(content)>sizeof(content))
                    {
                    wSize=strlen(content)-strlen (wBuf);
                    }

        strncat(content,wBuf,wSize);
    return(*this);
    }


//    friend class identityString;

// ---------edition----------------

    void
    sprintf (const char *pFormat,...) {
                                        va_list args;
                                       va_start (args, pFormat);
                                      vsprintf (content ,pFormat, args);
                                      va_end(args);
                                       return;
                                       }


//------------------operator = overload ------------------------------------------------
    descString& operator = (char *pString) {return(fromString(pString));}
    descString& operator = (const char *pString) {fromString((char *)pString);return(*this);}
    descString& operator = (const QString pString) {return(fromQString(pString));}
    descString& operator = (const descString &pString) {return(fromcodeString(pString));}


    descString& operator += (descString &pString) {
        if ((size()+pString.size())>cst_desclen)
                            {
                            size_t wi = cst_desclen-this->size();
                            strncat(content,pString.content,wi);
                            }
            else
                            strcat (content,pString.content);


        return(*this);}

    descString& operator += (const char * pString) {
        if ((size()+strlen(pString))>cst_desclen)
                            {
                            size_t wi = cst_desclen-size();
                            strncat(content,pString,wi);
                            }
            else
        strcat (content,pString);
        return(*this);}

    bool operator == (const descString &pCompare) { return(strncmp(content,pCompare.content,sizeof(content))==0);}
    bool operator != (const descString &pCompare) { return(!strncmp(content,pCompare.content,sizeof(content))==0);}
    bool operator > (const descString &pCompare) { return(strncmp(content,pCompare.content,sizeof(content))>0);}
    bool operator < (const descString &pCompare) { return(strncmp(content,pCompare.content,sizeof(content))<0);}

    bool operator == (const char *pCompare) { return(strncmp(content,pCompare,sizeof(content))==0);}
    bool operator != (const char *pCompare) { return!(strncmp(content,pCompare,sizeof(content))==0);}
    bool operator > (const char *pCompare) { return(strncmp(content,pCompare,sizeof(content))>0);}
    bool operator < (const char *pCompare) { return(strncmp(content,pCompare,sizeof(content))<0);}

    bool contains (char *pString) {return strstr(content,pString); }
};// descString
#endif // __COMMENT__



//----------Generic functions--------------------------------------------------
/**
 * @brief The ZDemanglingType enum
 *
 * Used by  _get_Data_Type()
 *
 *  the char case :
 *
 *      char*  -> Zchar + Zpointer
 *
 *      char   -> Zchar + Znumeric
 *
 *      char [] -> Zchar + Zarray
 *
 */
enum ZDemanglingType {
    Zno_type        =   0 ,
    Zatomic         =   1 ,
    Zcompound       =   2 ,
    Zpointer        =   4 ,
    Zarray          =   8 ,

    Zstruct         = 0x20 ,
    Zfunction     = 0x0100,

    Zchar         = 0x01000 ,  // cstring pointer = 0x20004
    // cstring array = 0x20008

    Znumeric      = 0x100000,

    Zint          = 0x102000,
    //            Zenum         = 0x1C000,     // enum is int
    Zlong         = 0x104000,
    Zfloat        = 0x110000,
    Zdouble       = 0x120000,

    ZStdString    = 0x01000000 ,  // std::string is a compound

    ZnoC11compiler = -1,
    ZerroredType   = -2,
    Zunknown      = 0xF0000         // may be a struct or a class
};



#define __STDSTRING__ "std::string"
//#include <cxxabi.h>

ZStatus typeDemangler(const char*pName, char *pOutName, size_t pOutSizeMax);

template <typename _Type>
ZData_Type_struct &_get_Data_Type (ZData_Type_struct*pDT_Return)
{
    //struct ZData_Type_struct DT_Return ;

    const char *_Type_Name_Local;

    memset (pDT_Return,0,sizeof(ZData_Type_struct));

    pDT_Return->Size = sizeof(_Type);

    pDT_Return->isPointer=std::is_pointer<_Type>::value ;
    if (pDT_Return->isPointer)
        pDT_Return->Type |= Zpointer ;


    pDT_Return->isArray=std::is_array<_Type>::value ;
    if (pDT_Return->isArray)
    {
        pDT_Return->Type |= Zarray;
    }


    _Type_Name_Local = typeid(_Type).name();

#ifdef __DEPRECATED__
    int wmangling_status ;
    size_t wmangling_length=199;
    /*
  *  WARNING : If _Type has a name that overrides the buffer length (199), a SIGABRT will be thrown (setTerminate() is called)
  *             when calling     __cxa_demangle  (Valgrind says : invalid free operation)
  *  SOLUTION : enlarge TypeName field size
  */
    memset(pDT_Return->TypeName,0,wmangling_length);
    char * wg= abi::__cxa_demangle(_Type_Name_Local,
                                   pDT_Return->TypeName,
                                   &wmangling_length,
                                   &wmangling_status);

    if (wg==NULL)
    {
        pDT_Return->Type = ZerroredType ;
        return (*pDT_Return) ;
    }
#endif // __DEPRECATED__

    typeDemangler(_Type_Name_Local,pDT_Return->TypeName,sizeof(pDT_Return->TypeName));

    while (true)
    {
        if (typeid(_Type).__is_function_p())
        {
            pDT_Return->Type |= Zfunction;
            break;
        }

        if (std::is_fundamental<_Type>() )
        {
            pDT_Return->Type |= Zatomic;
            pDT_Return->isAtomic = true;
        }
        else
        {
            pDT_Return->Type |= Zcompound ;
            pDT_Return->isCompound = true;
            if (strncmp(pDT_Return->TypeName,__STDSTRING__,strlen(__STDSTRING__))==0)
            {
                pDT_Return->isStdString =true;
                pDT_Return->Type |= ZStdString;
                return(*pDT_Return);
            }

        }

        if (!pDT_Return->isPointer)
            pDT_Return->Type |= Znumeric ;


        if (std::is_floating_point<_Type>())
        {
            pDT_Return->Type |= Zfloat;
            break;
        }
        if (strncasecmp(pDT_Return->TypeName,"long",4)==0)
        {
            pDT_Return->Type |= Zlong;
            break;
        }
        if (strncasecmp(pDT_Return->TypeName,"double",6)==0)
        {
            pDT_Return->Type |= Zdouble;
            break;
        }
        if (strncasecmp(pDT_Return->TypeName,"int",3)==0)
        {
            pDT_Return->Type |= Zint;
            break;
        }
        if (strncasecmp(pDT_Return->TypeName,"char",4)==0)
        {
            if (pDT_Return->isPointer)
            {
                pDT_Return->isCString = true ;
            }
            pDT_Return->Type |= Zchar;
            break;
        }
        if (strncasecmp(pDT_Return->TypeName,__STDSTRING__,strlen(__STDSTRING__)))
        {
            pDT_Return->isStdString =true;
            pDT_Return->Type |= ZStdString;
            break;
        }

        pDT_Return->Type |= Zunknown ; // unknow atomic type (may be a struct or a class)
        break;
    }

    return (*pDT_Return) ;
} // getDataType





#endif // ZBASEDATATYPES_H

