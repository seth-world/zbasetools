#ifndef ZMEM_H
#define ZMEM_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

const uint32_t     cst_ZMSTART      = 0xF6F6F6F6;  //!< Begin marker of a ZArray exported data structure on file it is a palyndroma
const uint32_t     cst_ZSTRINGEND   = 0xFAFAFAFA;  //!< End marker of a string data memory zone (in memory)
const uint32_t     cst_ZBUFFEREND   = 0xFBFBFBFB;  //!< End marker of a ZDataBuffer data structure in memory
const uint32_t     cst_ZCHECK       = 0xFFFFFFFF;   //!< End of utf string internal data structure (string header)

const uint32_t     cst_ZFILEBLOCKSTART = 0xF9F9F9F9;  //!< Begin marker of a file block structure on file it is a palyndroma
const uint32_t     cst_ZBLOCKSTART = 0xF5F5F5F5;  //!< Begin marker of a data structure on file it is a palyndroma
const uint32_t     cst_ZBLOCKEND   = 0xFCFCFCFC;  //!< End marker of a data structure on file : it is a palyndroma
const uint32_t     cst_ZFIELDSTART = 0xF4F4F4F4;  //!< Begin marker of a field data structure on file it is a palyndroma

const uint8_t      cst_ZSTART_BYTE = 0xF5;
const uint8_t      cst_ZFILESTART_BYTE = 0xF9;

template <class _Tp>
void zfree(_Tp *&pPtr)
{
  if (!pPtr)
    return;
  free(pPtr);
  pPtr=nullptr;
}


template <class _Tp>
_Tp* zmalloc(size_t pSize)
{
  _Tp* wPtr= (_Tp*)malloc(pSize);
  if (wPtr==nullptr)
    {
    fprintf(stderr,"zmalloc-F-nullptr fails to allocate memory ( requested size is <%ld>)\n",
        pSize);
    errno=ENOMEM;
    abort();
    }

  return wPtr;
}

template <class _Tp>
_Tp* zrealloc(_Tp* pPtr,size_t pSize)
{
  _Tp* wPtr= (_Tp*)realloc(pPtr,pSize);
  if (wPtr==nullptr)
      {
      fprintf(stderr,"_realloc-F-nullptr fails to reallocate memory ( requested size is <%ld>)\n",
      pSize);
      zfree(pPtr);
      errno=ENOMEM;
      abort();
      }

  return wPtr;
}

template <class _Tp>
void zdelete(_Tp* &pPtr)
{
  if (!pPtr)
    return;
  delete pPtr;
  pPtr=nullptr;
}


class utf8VaryingString;


utf8VaryingString
zmemDump(unsigned char* pData, const size_t pSize, const int pWidth, ssize_t pLimit);



/**
 * @brief zmemDumpHexa dumps a segment of memory pointed by pData and returns both ascii and hexadecimal values.
 *
 *  Returns the current memory using 2 utf8VaryingStrings :
 *   - one for printable hexadecimal result grouped by sets of 4 hexadecimal bytes
 *   - the other is printable ascii result.
 *
 *  Content dump is done from pOffset on pSize length
 *
 * @param[in] pData     Memory address to start
 * @param[in] pSize     Size in bytes of the data to dump as one line
 * @param[out] pLineHexa content expressed in hexadecimal
 * @param[out] pLineChar content in ascii
 * @return
 */
void
zmemDumpHexa (unsigned char* pData, const long pSize,
              utf8VaryingString &pLineHexa, utf8VaryingString &pLineChar);
int
zmemRulerSetup (utf8VaryingString &pRulerHexa, utf8VaryingString &pRulerAscii, int pWidth);



#endif // ZMEM_H
