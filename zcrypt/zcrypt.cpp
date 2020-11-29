#ifndef ZCRYPT_CPP
#define ZCRYPT_CPP

#include <zcrypt/zcrypt.h>
//#include <ztoolset/zexceptionmin.h>
//#include <ztoolset/ztypes.h>
#include <error.h>


#include <iostream>

#include <stdio.h>
#include <cstdio>

#include <ztoolset/zerror.h>
#include <ztoolset/zmem.h>


#include <ztoolset/zutfstrings.h>
#include <ztoolset/zdatabuffer.h>

const char cst_ReplacementChar = '.';


//#include <ztoolset/zbasedatatypes.h>
//#include <ztoolset/zdatabuffer.h>

//#include <QFile>

using namespace zbs;

#ifdef __COMMENT__
int Vmain (void)
{
  /* Set up the key and iv. Do I need to say to not hard code these in a
   * real application? :-)
   */

  /* Message to be encrypted */
  unsigned char *plaintext =
                (unsigned char *)"The quick brown fox jumps over the lazy dog";

  /* Buffer for ciphertext. Ensure the buffer is long enough for the
   * ciphertext which may be longer than the plaintext, dependant on the
   * algorithm and mode
   */
  unsigned char ciphertext[128];

  /* Buffer for the decrypted text */
  unsigned char decryptedtext[128];


  int decryptedtext_len, ciphertext_len;



  /* Encrypt the plaintext */
  ciphertext_len = encrypt (plaintext, strlen ((char *)plaintext), key, iv,
                            ciphertext);

  /* Do something useful with the ciphertext here */
  printf("Ciphertext is:\n");
  BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);

  /* Decrypt the ciphertext */
  decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv,
    decryptedtext);

  /* Add a NULL terminator. We are expecting printable text */
  decryptedtext[decryptedtext_len] = '\0';

  /* Show the decrypted text */
  printf("Decrypted text is:\n");
  printf("%s\n", decryptedtext);
  return 0;
}
#endif // __COMMENT__

static bool ZCryptActive=false;

static void ZCryptContextClear(void)    // launched once per process at process exit for AES cleaning
{
    EVP_cleanup();
    ERR_free_strings();
}

static void ZCrypContextInit(void)  // launched once per process first time for AES initialization
{
    /* Initialise the library */
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);
    atexit(ZCryptContextClear);
    ZCryptActive=true;
}


void ZCryptAES256::init(void)
{
    if (!ZCryptActive)
            ZCrypContextInit();
}

ZCryptAES256::ZCryptAES256(void)
{
    init();
    return;
}


ZCryptAES256::~ZCryptAES256(void)
{
    return;
}

void ZCryptAES256::handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  EVP_CIPHER_CTX_free(ctx);
  return;
}

/**
 * @brief ZCryptAES256::encrypt Encrypts the data contained in the uncrypted buffer (pPlainBuffer, pPlainBufferLen)
 *                              into the crypted buffer (pCryptedBuffer,pCryptedBufferLen)
 * memory is allocated to pCyptedBuffer on pCryptedBufferLen to receive encrypted data
 * @param[out] pCryptedBuffer       a pointer to an unsigned char* string.
 *                                  Will receive encrypted string data after having allocated memory enough.
 * @param[out] pCryptedBufferLen    a pointer to a size_t that will contain encrypted string data length
 * @param[in] pPlainBuffer      an unsigned char* string containing plain text data to AES256 encrypt
 * @param[in] pPlainBufferLen   length of the string to encrypt
 * @param[in] pKey              mandatory ZCryptKeyAES256 object containing encryption key
 * @param[in] pVector           mandatory ZCryptVectorAES256 object containing encryption vector
 * @return a ZStatus set to ZS_SUCCESS if everything went well, set to a value describing the error if not.
 * In case of error, error description is printed on stderr.
 * In case of fatal error, _ABORT_ routine is invoked.
 */
ZStatus ZCryptAES256::encrypt(unsigned char**      pCryptedBuffer,
                              size_t*              pCryptedBufferLen,
                              const unsigned char* pPlainBuffer,
                              const size_t         pPlainBufferLen,
                              ZCryptKeyAES256      pKey,
                              ZCryptVectorAES256   pVector)
{
_MODULEINIT_

    if ((pPlainBuffer==nullptr)||(pCryptedBufferLen==nullptr))
                {
                fprintf (stderr,
                         "%s-F-NNULLPTR  Fatal error :Plain buffer text and/or Crypted buffer length are nullptr\n",
                         _GET_FUNCTION_NAME_);
                _ABORT_
                }
    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
                {
                fprintf (stderr,
                         "%s-F-CIPHERCTXNEW Fatal error in cipher context allocation\n",
                         _GET_FUNCTION_NAME_);
                _ABORT_
                }


//    EVP_CIPHER_CTX_set_key_length(ctx,32);
    EVP_CIPHER_CTX_set_padding( ctx,1);

    int len;

  float SizeCalc = (float)pPlainBufferLen + (float) EVP_MAX_BLOCK_LENGTH;
  long wSize = (long)SizeCalc;

  (*pCryptedBuffer)=(unsigned char*)malloc(wSize);
  if (!*pCryptedBuffer)
          {
          fprintf(stderr,"%s-F-malloc fails to allocate memory for crypted buffer (size is <%ld>)\n",
                  _GET_FUNCTION_NAME_,
                  wSize);
          _ABORT_;
          }
//  pCryptedBuffer.allocate   (wSize);
//  pCryptedBuffer.CryptMethod =ZCM_AES256;

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, pKey.get(), pVector.get()))
                      {
                      fprintf (stderr,
                               "%s-S-ENCRYPTINIT Severe error in EVP encryption initialization (EVP_EncryptInit)\n",
                               _GET_FUNCTION_NAME_);

                       handleErrors();
                       _RETURN_ ZS_CRYPTERROR;
                      }
  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
//  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
//    handleErrors();
//  ciphertext_len = len;
  if(1 != EVP_EncryptUpdate(ctx, *pCryptedBuffer, &len, pPlainBuffer, pPlainBufferLen))
                  {
                  fprintf (stderr,
                           "%s-S-ENCRYPTUPDATE Severe error during EVP encryption process (EVP_EncryptUpdate)\n",
                           _GET_FUNCTION_NAME_);

                   handleErrors();
                   _RETURN_ ZS_CRYPTERROR;
                  }

  *pCryptedBufferLen = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, (*pCryptedBuffer) + len, &len))
                  {
                  fprintf (stderr,
                           "%s-S-ENCRYPTFINAL Error in final encryption phase (EVP_EncryptFinal) \n",
                           _GET_FUNCTION_NAME_);

                   handleErrors();
                   _RETURN_ ZS_CRYPTERROR;
                  }
    (*pCryptedBufferLen)  += len;
    (*pCryptedBuffer)=(unsigned char*)realloc((*pCryptedBuffer),(*pCryptedBufferLen) ); // adjust the size to effective crypted buffer size
    if (!*pCryptedBuffer)
              {
              fprintf(stderr,"%s-F-malloc fails to reallocate memory for crypted buffer (size is <%ld>)\n",
                      _GET_FUNCTION_NAME_,
                      *pCryptedBufferLen);
              _ABORT_;
              }
      /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    ctx=nullptr;
    /* Clean up */

  _RETURN_ ZS_SUCCESS;
}// encrypt

/**
 * @brief ZCryptAES256::uncrypt uncrypt the AES256 crypted buffer content pCryptedBuffer
 *                  using a Key (pKey) and a vector (pVector)
 *                  and moves the decrypted result into the uncrypted buffer pointed by pPlainBuffer
 *                  after having allocated memory of pPlainBufferLen size to store uncrypted string data.
 * Prerequisite : Key and Vector must have been set up with appropriate data
 *
 * @param[out] pPlainBuffer     a pointer to an unsigned char* string.
 *                               Will receive uncrypted string data read from crypted file after having allocated memory enough.
 * @param[out] pPlainBufferLen  a pointer to a size_t that will contain plain text string data length
 * @param[in] pCryptedBuffer    AES256 crypted string to uncrypt
 * @param[in]  pCryptedBufferLen crypted string length
 * @param[in]  pKey             mandatory ZCryptKeyAES256 object containing encryption key
 * @param[in]  pVector          mandatory ZCryptVectorAES256 object containing encryption vector
 * @return a ZStatus : ZS_SUCCESS if everything went well. A status describing the error in other cases.
 * In case of error, error description is printed on stderr.
 * In case of fatal error, _ABORT_ routine is invoked.
 */
ZStatus ZCryptAES256::uncrypt(unsigned char *&pPlainBuffer,
                              size_t& pPlainBufferLen,
                              const unsigned char *pCryptedBuffer,
                              const size_t pCryptedBufferLen,
                               ZCryptKeyAES256 pKey,
                               ZCryptVectorAES256 pVector)
{
_MODULEINIT_

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
                    {
                    fprintf (stderr,
                             "%s-F-CIPHERCTXNEW Fatal error in cipher context allocation\n",
                             _GET_FUNCTION_NAME_);
                    _ABORT_
                    }
 //   EVP_CIPHER_CTX_set_key_length(ctx,32);
    EVP_CIPHER_CTX_set_padding( ctx,1);

  int len;
  //int plaintext_len;

//  float SizeCalc =  (float)*pPlainBufferLen + (float) EVP_MAX_BLOCK_LENGTH;
//  float SizeCalc =  (float)(pPlainBufferLen +  EVP_MAX_BLOCK_LENGTH);
//  long wSize = (long)SizeCalc;

  size_t wSize = pCryptedBufferLen + EVP_MAX_BLOCK_LENGTH;

//   *pPlainBuffer=(unsigned char*)malloc(wSize);
    pPlainBuffer=(unsigned char*)malloc(wSize);
    if (!pPlainBuffer)
          {
          fprintf(stderr,"%s-F-malloc fails to allocate memory for plain text buffer (size is <%ld>)\n",
                  _GET_FUNCTION_NAME_,
                  wSize);
          _ABORT_
          }


  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, pKey.get(),pVector.get() ))
                      {
                      fprintf (stderr,
                               "%s-S-DECRYPTINIT Error in initialization of Crypting context (EVP_DecryptInit_ex)\n",
                               _GET_FUNCTION_NAME_);

                       handleErrors();
                       _free(pPlainBuffer);
                       pPlainBuffer = nullptr;
                       pPlainBufferLen = 0;
                       _RETURN_ ZS_CRYPTERROR;
                      }


  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx,pPlainBuffer, &len, pCryptedBuffer, pCryptedBufferLen))
                {

                  fprintf (stderr,
                           "%s-S-DECRYPTUPDATE Severe error during EVP decryption process (EVP_DecryptUpdate)\n",
                           _GET_FUNCTION_NAME_);

                   handleErrors();
                   free(pPlainBuffer);
                   pPlainBufferLen = 0;
                   _RETURN_ ZS_CRYPTERROR;
                }

//  *pPlainBufferLen = len;
  pPlainBufferLen = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(1 != EVP_DecryptFinal_ex(ctx,(unsigned char*) pCryptedBuffer + len, &len))
                  {
                  fprintf (stderr,
                           "%s-S-DECRYPTFINAL Error in final decryption phase (EVP_EncryptFinal) \n",
                           _GET_FUNCTION_NAME_);

                   handleErrors();
                   free(pPlainBuffer);
                   pPlainBufferLen = 0;
                   _RETURN_ ZS_CRYPTERROR;
                   }


//  (*pPlainBufferLen) += len;
  pPlainBufferLen += len;

//  *pPlainBuffer=(unsigned char*)realloc(*pPlainBuffer,*pPlainBufferLen);
  pPlainBuffer=(unsigned char*)realloc(pPlainBuffer,pPlainBufferLen);
  if (!pPlainBuffer)
          {
          fprintf(stderr,"%s-F-malloc fails to reallocate memory for plain text buffer(size is <%ld>)\n",
                  _GET_FUNCTION_NAME_,
                  pPlainBufferLen);
          free(pPlainBuffer);
          pPlainBufferLen = 0;
          _ABORT_
          }
//  PlainBuffer.Data[PlainBuffer.Size]='\0' ;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);
  ctx=nullptr;
  /* Clean up */
  _RETURN_ ZS_SUCCESS;
}

#ifdef __COMMENT__ // not used any more : key and vector are NOT (and must not be) stored
void
ZCryptAES256::setupKeyandVector (unsigned char* pKey,unsigned char* pIV)
 {
/*     memmove (ZKey,pKey,__CRYPT_KEYSIZE__);
     memmove (ZVector,pIV,__CRYPT_IVSIZE__ );*/
    ZKey.set(pKey);
    ZVector.set(pIV);
     FKeyInit = true;
     return;
 }
#endif // __COMMENT__
/**
 * @brief ZCryptAES256::encryptToFile encrypts AES256 string pPlainBuffer of length pPlainBufferLen, and stores the encrypted data in file pFilePath.
 * pKey and pVector are mandatory key and vector objects.
 * @param[in] pFilePath         a valid file name path for the file to be created / replaced
 * @param[in] pPlainBuffer      string containing plain text to be encrypted and stored in file
 * @param[in] pPlainBufferLen   length of the plain text string to encrypt
 * @param[in] pKey              mandatory encryption key as a ZCryptKeyAES256 object
 * @param[in] pVector           mandatory encryption vector as a ZCryptVectorAES256 object
 * @return a ZStatus : ZS_SUCCESS if everything went well. A status describing the error in other cases.
 * In case of error, error description is printed on stderr.
 * In case of fatal error, _ABORT_ routine is invoked.
 */
ZStatus ZCryptAES256::encryptToFile (const char*pFilePath,
                                     unsigned char* pPlainBuffer,
                                     const size_t pPlainBufferLen,
                                     const ZCryptKeyAES256& pKey,
                                     const ZCryptVectorAES256& pVector)
{
    unsigned char* wCryptedBuffer;
    size_t wCryptedBufferLen=0;
       ZStatus wSt=encrypt(&wCryptedBuffer,&wCryptedBufferLen,pPlainBuffer,pPlainBufferLen,pKey,pVector);
       if (wSt!=ZS_SUCCESS)
            return wSt;
       wSt= writeCryptedBufferToFile (pFilePath, wCryptedBuffer,wCryptedBufferLen);
       free(wCryptedBuffer);
       return wSt;
}//encryptToFile

/**
 * @brief ZCryptAES256::writeCryptedToFile writes the crypted buffer (pCryptedBuffer, pCryptedBufferLen) to a file given by pFilePath
 *  This routine does not make any crypting/uncrypting operation : it only writes pCryptedBuffer on pCryptedBufferLen to file pFilePath.
 *
 * @param[in] pFilePath a valid file name path
 * @param[in] pCryptedBuffer        data to write to file pFilePath
 * @param[in] pCryptedBufferLen     length of data to write to file
 * @return a ZStatus : ZS_SUCCESS if everything went well. A status describing the error in other cases.
 * In case of error, error description is printed on stderr.
 */
ZStatus ZCryptAES256::writeCryptedBufferToFile (const char*pFilePath, unsigned char* pCryptedBuffer,const size_t pCryptedBufferLen)
{
    FILE *wOutfile = fopen(pFilePath,"wb");
    if (wOutfile==nullptr)
        {
        int wErrno=errno;
        fprintf (stderr,
                 "%s-S-ERROPENFILE Severe Error opening file %s for writing crypted data.\n"
                 " Posix error is %d <%s> error content <%s>.\n",
                 _GET_FUNCTION_NAME_,
                 pFilePath,
                 wErrno,
                 decode_POSIXerrno(wErrno),
                 strerror(wErrno));
         return ZS_ERROPEN;
        }

    size_t wSize=fwrite (pCryptedBuffer,pCryptedBufferLen,1,wOutfile);
    if ((wSize!=1)||(ferror(wOutfile)))
            {
            int wErrno=ferror(wOutfile);
            fprintf (stderr,
                     "%s-S-ERROPENFILE Severe Error writing crypted data to file %s.\n"
                     " Posix error is %d <%s> error content <%s>.\n",
                     _GET_FUNCTION_NAME_,
                     pFilePath,
                     wErrno,
                     decode_POSIXerrno(wErrno),
                     strerror(wErrno));
            fclose(wOutfile);
             return ZS_WRITEERROR;
            }
    fclose(wOutfile);
    return(ZS_SUCCESS);
}// writeCryptedToFile


#include <sys/stat.h>

/**
 * @brief ZCryptAES256::uncryptFromFile loads AND UNCRYPTS the content of a crypted file given by pFilePath into the internal plain buffer PlainBuffer (ZDataBuffer type)
 *
 * loads the crypted content of the file
 * uncrypts the content to the uncrypted buffer PlainBuffer pPlainBuffer using a Key pKey and a Vector pVector (using uncrypt() routine)
 * after having allocated memory for storing uncrypted string data
 *
 * Prerequisite : Key and Vector must have been set up with appropriate data
 *                file path to access must point to a valid AES256 encrypted file
 *
 * @param[in] pFilePath         a valid existing file name path containing valid AES256 encrypted data
 * @param[out] pPlainBuffer      a pointer to an unsigned char* string.
 *                               Will receive uncrypted string data read from crypted file after having allocated memory enough.
 * @param[out] pPlainBufferLen  a pointer to a size_t that will contain plain text string data length
 * @param[in] pKey              mandatory ZCryptKeyAES256 object containing encryption key
 * @param[in] pVector           mandatory ZCryptVectorAES256 object containing encryption vector
 * @return a ZStatus : ZS_SUCCESS if everything went well. A status describing the error in other cases.
 * In case of error, error description is printed on stderr.
 * In case of fatal error, _ABORT_ routine is invoked.
 */
ZStatus ZCryptAES256::uncryptFromFile (const char* pFilePath,
                                       unsigned char*& pPlainBuffer,
                                       size_t&          pPlainBufferLen,
                                       const ZCryptKeyAES256& pKey,
                                       const ZCryptVectorAES256& pVector)
{
_MODULEINIT_

struct stat wStatBuffer ;
    int wSt= stat(pFilePath,&wStatBuffer);
    if (wSt<0)
            {
            return ZS_NOTFOUND;
            }
    size_t wCryptedBufferLen=wStatBuffer.st_size;
    unsigned char* wCryptedBuffer=(unsigned char*)malloc(wCryptedBufferLen);
    if (!wCryptedBuffer)
            {
            fprintf(stderr,"%s-F-malloc fails to allocate memory for crypted buffer (size is <%ld>)\n",
                    _GET_FUNCTION_NAME_,
                    wCryptedBufferLen);
            _ABORT_
            }
    FILE *wInfile = fopen(pFilePath,"rb");
    if (wInfile==nullptr)
        {
        int wErrno=errno;
        fprintf (stderr,
                 "%s-S-ERROPENFILE Severe Error opening file %s for writing crypted data.\n"
                 " Posix error is %d <%s> error content <%s>.\n",
                 _GET_FUNCTION_NAME_,
                 pFilePath,
                 wErrno,
                 decode_POSIXerrno(wErrno),
                 strerror(wErrno));
         _RETURN_ ZS_ERROPEN;
        }

        size_t wSize=fread (wCryptedBuffer,wCryptedBufferLen,1,wInfile);

        if ((wSize!=wCryptedBufferLen)&&(!feof(wInfile)))
            {
            int wErrno=ferror(wInfile);
            fprintf (stderr,
                     "%s-S-ERROPENFILE Severe Error reading crypted file %s.\n"
                     " Posix error is %d <%s> error content <%s>.\n",
                     _GET_FUNCTION_NAME_,
                     pFilePath,
                     wErrno,
                     decode_POSIXerrno(wErrno),
                     strerror(wErrno));
            fclose(wInfile);
             _RETURN_ ZS_READERROR;
            }

    fclose(wInfile);
    ZStatus wSts = uncrypt(pPlainBuffer, pPlainBufferLen, wCryptedBuffer, wCryptedBufferLen, pKey, pVector);

    _free(wCryptedBuffer);
//-----------------------------
    _RETURN_ wSts;
//------------------------------
}// uncryptFile
/**
 * @brief encryptB64    This routine uncrypts from Base64 encrypted string pInString of length pInSize
 * and store the plain text result into string pointed by pOutString after having allocated memory for storage.
 * Length of allocated string is returned on a size_t pointed by pOutSize.
 *
 * @param[out] pB64String   a pointer to an unsigned char* string.
 *                          Will receive Base64 encrypted string data after having allocated memory enough.
 * @param[out] pB64Size     a pointer to a size_t that will contain Base64 encrypted string data length
 * @param[in] pInString     plain text string to encrypt
 * @param[in] pInSize       plain text string length
 * @return a ZStatus : ZS_SUCCESS if everything went well. A status describing the error in other cases.
 * In case of error, error description is printed on stderr.
 * In case of fatal error, _ABORT_ routine is invoked.
 */
ZStatus
encryptB64( unsigned char**pB64String,size_t *pB64Size,unsigned char* pInString,const size_t pInSize )
{
_MODULEINIT_
    if (!ZCryptActive)
            ZCrypContextInit();
    BIO *bio, *b64;
//    BUF_MEM *bptr;
    char*  wPtr;
    int wRet;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line do not add \n
    int wLen = BIO_write(b64, pInString, pInSize);
    if (wLen<1)
        {
        fprintf(stderr,"%s-E-BIO_write fails to copy string data (size is <%ld>)\n",
                _GET_FUNCTION_NAME_,
                pInSize);
        ERR_print_errors_fp(stderr);
        BIO_free_all(b64);
        _RETURN_ ZS_BIOERR;
        }
    wRet=BIO_flush(b64);
    //    BIO_get_mem_ptr(b64, &bptr);
    //    setData(bptr->data,bptr->length);
    wLen=BIO_get_mem_data(b64, &wPtr);
    *pB64String=(unsigned char*)malloc(wLen+1);
    if (!*pB64String)
            {
            fprintf(stderr,"%s-F-malloc fails to allocate memory (size is <%d>)\n",
                    _GET_FUNCTION_NAME_,
                    wLen);
            _ABORT_;
            }
    *pB64Size=wLen+1;
    memmove(*pB64String,wPtr,wLen);
    (*pB64String)[wLen]='\0';

    BIO_free_all(b64);
    _RETURN_ ZS_SUCCESS;
}
/**
 * @brief uncryptB64 uncrypt Base64 crypted string pInString of length pInSize into allocated string pOutString.
 * A termination mark '\0' is conditionally added at the end of the uncrypted string (not added if last character is already '\0')
 * pOutString receives the decrypted string data.
 * memory is allocated and set to pOutString, size of allocated memory is returned in pOutSize.
 * Calling procedure must later free this allocated memory.
 *
 * @param[out] pOutString   a pointer to an unsigned char* string. Will receive uncrypted string data after having allocated memory enough.
 * @param[out] pOutSize     a pointer to a size_t that will contain uncrypted string data length
 * @param[in] pB64String    an unsigned char* string containing Base64 encrypted data to uncrypt
 * @param[in] pB64Size      length of the Base64 crypted string to uncrypt
 * @return a ZStatus : ZS_SUCCESS if everything went well. A status describing the error in other cases.
 * In case of error, error description is printed on stderr.
 * In case of fatal error, _ABORT_ routine is invoked.
 */
ZStatus
uncryptB64( unsigned char**pOutString,size_t *pOutSize,unsigned char* pB64String,const size_t pB64Size )
{
_MODULEINIT_
    if ((pB64String==nullptr)||(pOutSize==nullptr))
                {
                fprintf (stderr,
                         "%s-F-NNULLPTR  Fatal error : Encrypted input string and/or plain buffer length are nullptr\n",
                         _GET_FUNCTION_NAME_);
                _ABORT_
                }
    if (!ZCryptActive)
            ZCrypContextInit();
    BIO *bio, *b64;
    BUF_MEM *bptr;
    unsigned char*  wPtr;
    int wRet;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line do not add \n
    int wLen = BIO_write(b64, pB64String, pB64Size);
    if (wLen<1)
            {
            fprintf(stderr,"%s-E-BIO_write fails to copy string data (size is <%ld>)\n",
                    _GET_FUNCTION_NAME_,
                    pB64Size);
            ERR_print_errors_fp(stderr);
            BIO_free_all(b64);
            _RETURN_ ZS_BIOERR;
            }
    wRet=BIO_flush(b64);
    if (wRet<1)
        {
        fprintf(stderr,"%s-E-BIO_flush failed\n",
                _GET_FUNCTION_NAME_);
        ERR_print_errors_fp(stderr);
        BIO_free_all(b64);
        _RETURN_ ZS_BIOERR;
        }
    //    BIO_get_mem_ptr(b64, &bptr);
    //    setData(bptr->data,bptr->length);
    wLen=BIO_get_mem_data(b64, &wPtr);
    *pOutString=(unsigned char*)malloc(wLen+1);
    if (!*pOutString)
            {
            fprintf(stderr,"%s-F-malloc fails to allocate memory (size is <%d>)\n",
                    _GET_FUNCTION_NAME_,
                    wLen);
            BIO_free_all(b64);
            _ABORT_;
            }

    *pOutSize = wLen;
    memmove(*pOutString,wPtr,wLen);

    if ((*pOutString)[wLen-1])  // if last is not a zero mark
                {
                (*pOutString)[wLen] = 0; // add additional termination mark outside the string
                *pOutSize ++;
                }
            else
                {
                *pOutString=(unsigned char*)realloc(*pOutString,wLen); // if already zero terminated, resize(truncate) to fit
                }
    BIO_free_all(b64);
    _RETURN_ ZS_SUCCESS;
}// uncryptB64

/**
 * @brief calcDecodeLengthB64 This routine computes required length of an Base64 encrypted string for storing its uncrypted version
 * @param[in] b64input  encoded Base64 string to compute the size from
 * @param[in] pSize     size of Base64 input string
 * @return          computed length of decrypted (plain text) string
 */
size_t calcDecodeLengthB64(const unsigned char* b64input,size_t pSize)  //Calculates the length of a decoded string
{
    //    size_t len = strlen(b64input),
    size_t len = pSize,
            padding = 0;

    if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
        padding = 2;
    else if (b64input[len-1] == '=') //last char is =
        padding = 1;

    return (len*3)/4 - padding;
}// calcDecodeLengthB64




/* crypting key & vector */


void ZCryptKeyAES256::set(const unsigned char *pkey)
{
  const unsigned char *wPtrIn =  pkey;
  unsigned char *wPtr = content;
  size_t wi = 0;
  while ((wi < cst_AES256cryptKeySize) && (wPtrIn[wi])) {
    content[wi] = wPtrIn[wi];
    wi++;
  }
  while (wi < cst_AES256cryptKeySize)
    content[wi++] = 0;
  return;
}

void ZCryptKeyAES256::set(const char *pkey)
{
  const unsigned char *wPtrIn = (const unsigned char *) pkey;
  unsigned char *wPtr = content;
  size_t wi = 0;
  while ((wi < cst_AES256cryptKeySize) && (wPtrIn[wi])) {
    content[wi] = wPtrIn[wi];
    wi++;
  }
  while (wi < cst_AES256cryptKeySize)
    content[wi++] = 0;
  return;
}

ZDataBuffer ZCryptKeyAES256::toB64()
{
    ZDataBuffer wReturn;
    wReturn.encryptB64(content, cst_AES256cryptKeySize);
    return wReturn;
}
int ZCryptKeyAES256::fromB64(ZDataBuffer &pZDB)
{
    pZDB.uncryptB64();
    clear();
    if (pZDB.Size < cst_AES256cryptKeySize) {
        fprintf(stderr,
                "ZCryptKeyAES256::fromB64-S-INVSIZ Severe Error : invalid size <%ld> of input "
                "string from Hexadecimal "
                "convertion.\n"
                " Checksum hexadecimal string size must be greater than or equal to <%ld>\n",
                pZDB.Size,
                cst_AES256cryptKeySize);
        return -1;
    }
    memmove(content, pZDB.Data, cst_AES256cryptKeySize);
    return 0;
}

utf8VaryingString ZCryptKeyAES256::toStr()
{
    utf8VaryingString wReturn;
    wReturn.allocateUnitsBZero(cst_AES256cryptKeySize + 1);
    int wi;
    for (wi = 0; wi < cst_AES256cryptKeySize; wi++)
          if ((content[wi]<32)||(content[wi]>122))
            wReturn.DataByte[wi] = cst_ReplacementChar;
          else
            wReturn.DataByte[wi] = content[wi];
    /*
        if (isalpha((int) content[wi]))
            wReturn.DataByte[wi] = content[wi];
        else
            wReturn.DataByte[wi] = cst_ReplacementChar;
*/

    wReturn.DataByte[cst_AES256cryptKeySize] = 0; // not necessary because allocateBZeroed
    return wReturn;
}

utf8VaryingString ZCryptKeyAES256::toHexa()
{
    utf8VaryingString wReturn;
    wReturn.allocateUnitsBZero((cst_AES256cryptKeySize * 2) + 1);
    int wi;
    for (wi = 0; wi < cst_AES256cryptKeySize; wi++)
        std::sprintf((char *) &wReturn.DataByte[wi * 2], "%02X", (unsigned int) content[wi]);
    return (wReturn);
}
int ZCryptKeyAES256::fromHexa(const utf8VaryingString &pHexa)
{
    size_t wSize = (cst_AES256cryptKeySize * 2);
    if (pHexa.UnitCount < wSize) {
        fprintf(stderr,
                "ZCryptKeyAES256::fromHexa-S-INVSIZ Severe Error : invalid size <%ld> of input string from Hexadecimal "
                "convertion.\n"
                " Crypting key hexadecimal string size must be greater than or equal to <%ld>\n",
                pHexa.UnitCount,
                wSize);
        return -1;
    }
    char wBuf[3];
    wBuf[2]='\0';
    for(int wi=0; wi<cst_AES256cryptKeySize;wi++)
        {
        ::strncpy(wBuf,(const char*)&pHexa.DataByte[wi*2],2);
        content[wi]=(uint8_t)strtoul(wBuf,nullptr,16);
        }
    return 0;
}//ZCryptKeyAES256::fromHexa


/* 33-126 : range 93   excluding space (32) */

/*
 * [48-57] ; [64-90] ; [97-122]

48 - 122 -> 74

wrong 58 - 63   91 - 96
                       */

inline unsigned char genRndChar()
{
//  std::srand(std::time(nullptr)); this must be done externally to the routine

//  char wC = std::rand() % 93 + 33 ;
//  while ((wC < 32)||(wC > 126))
    unsigned char wC= std::rand() % 74 + 48;
    while (((wC > 57)&&(wC <64))||((wC>90)&&(wC<97)))
          wC= std::rand() % 74 + 48;
  return wC;

//  return std::rand() % 74 + 48 ;
}

ZCryptKeyAES256 ZCryptKeyAES256::generate()
{
ZCryptKeyAES256 wK;
  std::srand(std::time(nullptr));
  for (int wi=0;wi < cst_AES256cryptKeySize;wi++ )
    wK.content[wi]=genRndChar();

  return wK;
}



/* crypting vector */
void ZCryptVectorAES256::set(const unsigned char *pVector)
{
  const unsigned char *wPtrIn = (const unsigned char *) pVector;
  unsigned char *wPtr = content;
  size_t wi = 0;
  while ((wi < cst_AES256cryptVectorSize) && (wPtrIn[wi])) {
    content[wi] = wPtrIn[wi];
    wi++;
  }
  while (wi < cst_AES256cryptVectorSize)
    content[wi++] = 0;
  return;
}

void ZCryptVectorAES256::set(const char *pVector)
{
  const unsigned char *wPtrIn = (const unsigned char *) pVector;
  unsigned char *wPtr = content;
  size_t wi = 0;
  while ((wi < cst_AES256cryptVectorSize) && (wPtrIn[wi])) {
    content[wi] = wPtrIn[wi];
    wi++;
  }
  while (wi < cst_AES256cryptVectorSize)
    content[wi++] = 0;
  return;
}

ZDataBuffer ZCryptVectorAES256::toB64()
{
    ZDataBuffer wReturn;
    wReturn.encryptB64(content, cst_AES256cryptVectorSize);
    return wReturn;
}
int ZCryptVectorAES256::fromB64(ZDataBuffer &pZDB)
{
    pZDB.uncryptB64();
    clear();
    if (pZDB.Size < cst_AES256cryptVectorSize) {
        fprintf(stderr,
                "ZCryptVectorAES256::fromB64-S-INVSIZ Severe Error : invalid size <%ld> of input "
                "string from Hexadecimal "
                "convertion.\n"
                " Checksum hexadecimal string size must be greater than or equal to <%ld>\n",
                pZDB.Size,
                cst_AES256cryptVectorSize);
        return -1;
    }
    memmove(content, pZDB.Data, cst_AES256cryptVectorSize);
    return 0;
}

utf8VaryingString ZCryptVectorAES256::toHexa()
{
    utf8VaryingString wReturn;
    wReturn.allocateUnitsBZero((cst_AES256cryptVectorSize * 2) + 1);
    int wi;
    for (wi = 0; wi < cst_AES256cryptVectorSize; wi++)
        std::sprintf((char *) &wReturn.DataByte[wi * 2], "%02X", (unsigned int) content[wi]);
    return (wReturn);
}



ZCryptVectorAES256 ZCryptVectorAES256::generate()
{
  ZCryptVectorAES256 wK;
//  std::srand(std::time(nullptr));
  for (int wi=0;wi < cst_AES256cryptVectorSize;wi++ )
    wK.content[wi]=genRndChar();

  return wK;
}

utf8VaryingString ZCryptVectorAES256::toStr()
{
    utf8VaryingString wReturn;
    wReturn.allocateUnitsBZero(cst_AES256cryptVectorSize + 1);
    int wi;
    for (wi = 0; wi < cst_AES256cryptVectorSize; wi++)
      if ((content[wi]<32)||(content[wi]>122))
            wReturn.DataByte[wi] = cst_ReplacementChar;
          else
            wReturn.DataByte[wi] = content[wi];

/*        if (isalpha((int) content[wi]))
            wReturn.DataByte[wi] = content[wi];
        else
            wReturn.DataByte[wi] = cst_ReplacementChar;
*/
    wReturn.DataByte[cst_AES256cryptVectorSize] = 0; // not necessary because allocateBZeroed
    return wReturn;
}


int ZCryptVectorAES256::fromHexa(const utf8VaryingString &pHexa)
{
    size_t wSize = (cst_AES256cryptVectorSize * 2);
    if (pHexa.UnitCount < wSize) {
        fprintf(stderr,
                "ZCryptVectorAES256::fromHexa-S-INVSIZ Severe Error : invalid size <%ld> of input string from Hexadecimal "
                "convertion.\n"
                " Crypting key hexadecimal string size must be greater than or equal to <%ld>\n",
                pHexa.UnitCount,
                wSize);
        return -1;
    }
    char wBuf[3];
    wBuf[2]='\0';
    for(int wi=0; wi<cst_AES256cryptVectorSize;wi++)
    {
        ::strncpy(wBuf,(const char*)&pHexa.DataByte[wi*2],2);
        content[wi]=(uint8_t)strtoul(wBuf,nullptr,16);
    }
    return 0;
}//ZCryptVectorAES256::fromHexa






#endif // ZCRYPT_CPP
