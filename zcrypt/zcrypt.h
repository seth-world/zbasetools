#ifndef ZCRYPT_H
#define ZCRYPT_H

/** @file zcrypt.h Base objects and routines for encryption
 *
 *  Excepted this file does not refer to evolved zbase objects.
 *
 *  It uses unsigned char* pointers.
 *
 *  Encryption / uncryption is implemented within 'evolved' objects and use these facilities underneeth.
 *
 *
 */
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include <stdint.h>
#include <string.h>
#include <ztoolset/zerror.h>

#ifndef __ZCRYPTMETHOD_TYPE__
#define __ZCRYPTMETHOD_TYPE__
enum ZCryptMethod_type : uint8_t
{
    ZCM_Uncrypted       = 0,
    ZCM_Base64          = 1,    // self explainatory
    ZCM_AES256          = 2,    // encryption with key and vector
    ZCM_SHA256          = 3,    // checkSum
    ZCM_MD5             = 5     // md5
};
#endif // __ZCRYPTMETHOD_TYPE__
// -----Crypting------------------

#define __CRYPT_KEYSIZE__ 32
#define __CRYPT_IVSIZE__  16
constexpr size_t cst_AES256cryptKeySize = 32;
constexpr size_t cst_AES256cryptVectorSize = 16;



ZStatus encryptB64(unsigned char**pB64String, size_t *pB64Size, unsigned char* pInString, const size_t pInSize );
ZStatus uncryptB64(unsigned char**pOutString, size_t *pOutSize, unsigned char* pB64String, const size_t pB64Size );

namespace zbs {
class ZCryptKeyAES256
{
public:
    ZCryptKeyAES256(void) {memset(content,0,cst_AES256cryptKeySize); }
    ZCryptKeyAES256(const unsigned char*pString) {set(pString);}
    ZCryptKeyAES256(const ZCryptKeyAES256&pIn) { set(pIn.content); }

    unsigned char content[cst_AES256cryptKeySize];
    unsigned char* get(void) {return content;}

    void set(const unsigned char *pkey) { memmove(content, pkey, cst_AES256cryptKeySize); }
    void set(const char *pkey) { memmove(content, pkey, cst_AES256cryptKeySize); }
    ZCryptKeyAES256 &operator=(const ZCryptKeyAES256 &pIn)
    {
        set(pIn.content);
        return *this;
    }
};

//class ZDataBuffer;

class ZCryptVectorAES256
{
public:
    ZCryptVectorAES256(void) {memset(content,0,cst_AES256cryptVectorSize);}
    ZCryptVectorAES256(const unsigned char*pVector) {set(pVector);}
    ZCryptVectorAES256(const ZCryptVectorAES256&pIn) { set(pIn.content); }

    unsigned char content[cst_AES256cryptVectorSize];
    unsigned char* get(void) {return content;}

    void set(const unsigned char *pVector) { memmove(content, pVector, cst_AES256cryptVectorSize); }
    void set(const char *pVector) { memmove(content, pVector, cst_AES256cryptVectorSize); }

    ZCryptVectorAES256 &operator=(const ZCryptVectorAES256 &pIn) { set(pIn.content); return *this;}
};

class ZCryptAES256
{
public:
    ZCryptAES256();
    ~ZCryptAES256() ;

    ZStatus encrypt(unsigned char **pCryptedBuffer,
                    size_t *pCryptedBufferLen,
                    const unsigned char *pPlainBuffer,
                    const size_t pPlainBufferLen,
                    ZCryptKeyAES256     pKey,
                    ZCryptVectorAES256  pVector);


    ZStatus uncrypt(unsigned char **pPlainBuffer,
                    size_t *pPlainBufferLen,
                    const unsigned char *pCryptedBuffer,
                    const size_t pCryptedBufferLen,
                    ZCryptKeyAES256 pKey,
                    ZCryptVectorAES256 pVector);

    void init(void);

    void handleErrors(void);

//   void setupKeyandVector (unsigned char* pKey,unsigned char* pIV);

   ZStatus uncryptFromFile (const char *pFilePath,
                            unsigned char **pPlainBuffer,
                            size_t *pPlainBufferLen,
                            const ZCryptKeyAES256 &pKey,
                            const ZCryptVectorAES256 &pVector);

   ZStatus encryptToFile (const char *pFilePath,
                          unsigned char *pPlainBuffer,
                          const size_t pPlainBufferLen,
                          const ZCryptKeyAES256& pKey,
                          const ZCryptVectorAES256& pVector) ;

   ZStatus writeCryptedBufferToFile (const char *pFilePath, unsigned char *pCryptedBuffer, const size_t pCryptedBufferLen) ;


protected:
    bool FKeyInit = false;
#ifdef __COMMENT__
    unsigned char ZKey [__CRYPT_KEYSIZE__+1];


    /* A 128 bit IV     . 16 bytes*/
    unsigned char *iv = (unsigned char *)"01234567890123456";
    unsigned char ZVector [__CRYPT_IVSIZE__+1];
#endif
    ZCryptKeyAES256     ZKey;
    ZCryptVectorAES256  ZVector;
private:

    /* A 256 bit key    32 bytes */

    EVP_CIPHER_CTX *ctx;

};

}//zbs

size_t calcDecodeLengthB64(const unsigned char* b64input,size_t pSize);

#endif // ZCRYPT_H
