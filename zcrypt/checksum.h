#ifndef CHECKSUM_H
#define CHECKSUM_H
#include <ztoolset/zlimit.h>
#include <ztoolset/zerror.h>
//#include <openssl/sha.h>  // required for checksum

//#include <openssl/md5.h>

//#include <ztoolset/zdatabuffer.h>
/*
#ifndef utfdescString
    class utfdescString;
#endif
*/
#ifdef QT_CORE_LIB
#include <qbytearray.h>
#endif


class ZDataBuffer;
/**
 * @brief checkSum checksum computation and use. Currently SHA256 algorithm is used.
 */
class checkSum
{
public:
    unsigned char content[cst_checksum];

    char StrHexa[cst_checksumHex+1];

    checkSum(void) {clear();}
    checkSum(unsigned char *pInBuffer,size_t pLen) {compute(pInBuffer,pLen);}
//    checkSum(ZDataBuffer &pZDBData) {compute(pZDBData); return;}

    checkSum& setContent(unsigned char *pInBuffer,size_t pLen) {memmove (content,pInBuffer,pLen); return *this;}

    size_t size(void) {return(cst_checksum);}

    bool isClear(void) { char wCompare[cst_checksum];
                         memset(wCompare,0,sizeof(wCompare));
                        return (memcmp(content,wCompare,cst_checksum)); }
    bool isEmpty(void) {return (isClear()); }
    checkSum &clear(void) {memset(content,0,cst_checksum);return(*this);}

    void compute(unsigned char *pInBuffer,size_t pLen);
    void compute(char *pInBuffer,size_t pLen) {compute ((unsigned char *)pInBuffer,pLen); return;}
//    checkSum& compute (ZDataBuffer &pDataBuffer);
//    template <class _Utf>
//    checkSum& compute (utfVaryingString<_Utf> &pDataBuffer);

    ZDataBuffer *_exportURF(ZDataBuffer *pUniversal);
    ZStatus _importURF(unsigned char* &pUniversal);
    static ZStatus getUniversalFromURF(unsigned char* pURFDataPtr,ZDataBuffer& pUniversal);

#ifdef QT_CORE_LIB
    checkSum & fromQByteArray (const QByteArray &pQBA) ;
    checkSum & fromQDataBuffer (const ZDataBuffer &pQDB);
//    QByteArray toQByteArray(void) {return(QByteArray((char*)content,size()));}

    checkSum &operator = (const QByteArray &pQBA) {return(fromQByteArray(pQBA));}
#endif
    checkSum & fromHex(const char *pInput, const size_t pSize) ;
    void fromCheckSum(checkSum& pChecksum) { memmove(content,pChecksum.content,sizeof(content)); return ;}
//    checkSum &  fromHex(QDataBuffer_struct &pQDB) ;

    const char *toHex(void);
    unsigned char * toBinary(void) {return(content);}
//    ZDataBuffer& toZDataBuffer (ZDataBuffer &pQDB) { pQDB.setData( content, size()); return(pQDB);}

    bool        compareHex(const char *pDesc);

//    checkSum &operator = (const checkSum &pcheckSum) { memmove(content,pcheckSum.content,sizeof(content)); return *this;}

    bool operator == (const checkSum *pChecksum) {return (memcmp(content,pChecksum->content,sizeof(content))==0);}
    bool operator == (const checkSum &pChecksum) {return (memcmp(content,pChecksum.content,sizeof(content))==0);}
    bool operator != (const checkSum &pChecksum) {return !(memcmp(content,pChecksum.content,sizeof(content))==0);}
    bool operator == (const char *pInput) ;
};




#endif // CHECKSUM_H
