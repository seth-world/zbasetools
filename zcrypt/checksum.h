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
  //  char StrHexa[cst_checksumHexa+3];

  checkSum(void) {clear();}
  checkSum& _copyFrom(const checkSum& pIn);

  checkSum(const checkSum& pIn) {_copyFrom(pIn);}
  checkSum(const checkSum&& pIn) {_copyFrom(pIn);}

  checkSum& operator = (const checkSum& pIn) {return _copyFrom(pIn);}

  checkSum(const unsigned char *pInBuffer,size_t pLen) {compute(pInBuffer,pLen);}

  checkSum& setContent(const unsigned char *pInBuffer,size_t pLen) {memmove (content,pInBuffer,pLen); return *this;}

  size_t size(void) const {return(cst_checksum);}

  bool isClear(void) const  { char wCompare[cst_checksum];
                         memset(wCompare,0,sizeof(wCompare));
                        return (memcmp(content,wCompare,cst_checksum)); }
  bool isEmpty(void) const  {return (isClear()); }
  checkSum &clear(void) {memset(content,0,cst_checksum);return(*this);}

  void compute(const unsigned char *pInBuffer,size_t pLen);
  void compute(const char *pInBuffer,size_t pLen) {compute ((const unsigned char *)pInBuffer,pLen); return;}

    size_t        _exportURF(ZDataBuffer &pUniversal) const;
    size_t        _exportURF_Ptr(unsigned char* &pURF) const;
    size_t        getURFSize() const;
    size_t        getURFHeaderSize() const;
    size_t        getUniversalSize() const;
    ssize_t       _importURF(const unsigned char *&pUniversal);
    static ZStatus getUniversalFromURF(const unsigned char* pURFDataPtr, ZDataBuffer& pUniversal,const unsigned char **pURFDataPtrOut=nullptr);

    ZDataBuffer _export() const;
    size_t _import(const unsigned char *&pUniversal);



#ifdef QT_CORE_LIB
    checkSum & fromQByteArray (const QByteArray &pQBA) ;
    checkSum &operator = (const QByteArray &pQBA) {return(fromQByteArray(pQBA));}
#endif
    /** @brief fromHexa() converts a checkSum pInput expressed as a string containing hexadecimal printable values and set it as binary content
     * remark : hexadecimal string must have a size of cst_checksumHexa, not less, not more  */
    ZStatus fromHexa(const char *pInput, const size_t pSize) ;
    ZStatus fromHexa(const utf8VaryingString &pInput) ;
    void fromCheckSum(checkSum& pChecksum) { memmove(content,pChecksum.content,sizeof(content)); return ;}
//    checkSum &  fromHex(QDataBuffer_struct &pQDB) ;
    /** @brief toHexa() delivers checkSum content as a string containing hexadecimal printable values */
    utf8VaryingString toHexa(void) const;
    /** @brief toBinary() delivers the content of checkSum in its binary format */
    unsigned char * toBinary(void) {return(content);}
//    ZDataBuffer& toZDataBuffer (ZDataBuffer &pQDB) { pQDB.setData( content, size()); return(pQDB);}

    /** @brief compareHexa() converts string pDesc from text hexadecimal format to internal binary checksum then compares it with current*/
    bool        compareHexa(const char *pDesc);

//    checkSum &operator = (const checkSum &pcheckSum) { memmove(content,pcheckSum.content,sizeof(content)); return *this;}

    bool operator == (const checkSum *pChecksum) {return (memcmp(content,pChecksum->content,sizeof(content))==0);}
    bool operator == (const checkSum &pChecksum) {return (memcmp(content,pChecksum.content,sizeof(content))==0);}
    bool operator != (const checkSum &pChecksum) {return !(memcmp(content,pChecksum.content,sizeof(content)));}
    bool operator == (const char* pInput)   ;
};




#endif // CHECKSUM_H
