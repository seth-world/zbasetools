#ifndef ZMEM_H
#define ZMEM_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

const uint32_t     cst_ZMSTART = 0xF6F6F6F6;  //!< Begin marker of a data structure on file it is a palyndroma
const uint32_t     cst_ZSTRINGEND   = 0xFAFAFAFA;  //!< End marker of a string data memory zone
const uint32_t     cst_ZBUFFEREND   = 0xFBFBFBFB;  //!< End marker of a ZDataBuffer data structure in memory

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
 *  Returns the current memory using 2 utf8Strings :
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
