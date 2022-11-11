#ifndef MD5_H
#define MD5_H

#include <string.h>  // for memset
#include <ztoolset/zlimit.h>

#include <openssl/md5.h>

#include <ztoolset/charman.h>
#include <ztoolset/zstatus.h>

class ZDataBuffer;

//#include <ztoolset/zdatabuffer.h>
class utf8VaryingString;

struct md5 {
    unsigned char content[cst_md5];

    md5(void) {clear();}
    md5& _copyFrom(const md5& pIn);

    md5(const md5& pIn) {_copyFrom(pIn);}
    md5(const md5&& pIn) {_copyFrom(pIn);}

    md5& operator = (const md5& pIn) {return _copyFrom(pIn);}

    md5(unsigned char *pInBuffer,size_t pLen) {compute(pInBuffer,pLen);}

//    md5(ZDataBuffer &pZDBData) {compute(pZDBData.Data,pZDBData.Size); return;}
    md5& compute(unsigned char* pData, const size_t pSize);
    utf8VaryingString toHexa() const ; /** converts md5 expressed in a hexadecimal string to its value. string must be allocated with cst_md5Hex+1 */
//    ZStatus fromHexa(const char* pInput) ;
    ZStatus fromHexa(const utf8VaryingString& pInput);
    void clear(void) {memset(content,0,cst_md5); return;}

    bool isInvalid() const
    {
      for (int wi=0;wi<cst_md5;wi++)
        if (content[wi])
          return false;
      return true;
    }
    bool isValid() const {return !isInvalid();}

    void _export(unsigned char* pPtrOut) const
    {
      for (int wi=0;wi < cst_md5Hexa;wi++)
        pPtrOut[wi]=content[wi];
    }

    void _import(const unsigned char* &pPtrIn)
    {
      unsigned char* wPtrIn=content;
      memset (content,0,sizeof(content));
      int wi=cst_md5Hexa;
      while (wi--)
        *wPtrIn++ = *pPtrIn++;
    }

    size_t        _exportURF(ZDataBuffer &pUniversal) const;
    size_t        _exportURF_Ptr(unsigned char*& pURF) const;

    size_t        getURFSize() const;
    size_t        getURFHeaderSize() const;
    size_t        getUniversalSize() const;

    ssize_t _importURF(const unsigned char* &pUniversal);

    bool operator == (const md5 *pChecksum) const {return (memcmp(content,pChecksum->content,sizeof(content))==0);}
    bool operator == (const md5 &pChecksum) const {return (memcmp(content,pChecksum.content,sizeof(content))==0);}
    bool operator != (const md5 &pChecksum) const {return !(memcmp(content,pChecksum.content,sizeof(content))==0);}
    bool operator == (const char* pInput) ;
};


#endif // MD5_H
