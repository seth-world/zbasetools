#ifndef MD5_H
#define MD5_H

#include <string.h>  // for memset
#include <ztoolset/zlimit.h>

#include <openssl/md5.h>

//#include <ztoolset/zdatabuffer.h>

struct md5 {
    unsigned char content[cst_md5];

    md5(void) {clear();}
    md5(unsigned char *pInBuffer,size_t pLen) {compute(pInBuffer,pLen);}

//    md5(ZDataBuffer &pZDBData) {compute(pZDBData.Data,pZDBData.Size); return;}
    md5& compute(unsigned char* pData, const size_t pSize);
    char *toHex(char *pHexaString); /** converts md5 expressed in a hexadecimal string to its value. string must be allocated with cst_md5Hex+1 */
    md5& fromHex(const char* pInput) ;

    void clear(void) {memset(content,0,cst_md5); return;}
};


#endif // MD5_H
