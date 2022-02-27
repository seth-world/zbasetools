#ifndef ZBITSET_H
#define ZBITSET_H

#include <stdint.h>
#include <iostream>

#define _GLIBCXX_BITSET_WORDS(__n) \
  ((__n) / _GLIBCXX_BITSET_BITS_PER_WORD + \
   ((__n) % _GLIBCXX_BITSET_BITS_PER_WORD == 0 ? 0 : 1))
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/ztypetype.h>
namespace zbs{
#pragma pack(push)
#pragma pack(1)
class ZBitset
{
public:
    typedef uint8_t ZBitsetType;
    ZBitsetType *bit=nullptr;                   // bitset content itself
    size_t      Size=0;                            // number of ZBitsetType to allocate (of UnitByteSize bytes each)
    size_t      BitCapacity=0;                         // total number of bits it represents
    size_t      EffectiveBitSize=0;                  // number of effective bits requested for allocation: to be exported/imported
    const size_t UnitByteSize=sizeof(ZBitsetType); // size in bytes of one ZBitsetType
    const size_t UnitBitSize=UnitByteSize*8;    // size in bits of one ZBitsetType

    ZBitset() {}
    ZBitset(const uint16_t pBitsToAllocate)
    {
      _allocate(pBitsToAllocate);
      clear();
    }
    ~ZBitset() {if (bit) free (bit);}

    /**
     * @brief getbitSize returns the capacity of bitset in bits.
     */
    size_t getbitSize() {return BitCapacity;}
    /**
     * @brief _allocate allocates memory space enough for managing pBitsToAllocate slots (bits) within bitmap
     *                  memory is set/reset to binary zero
     */
    void _allocate(const size_t pBitsToAllocate);
    void clear() ;
    /**
     * @brief setNull sets the object as null bitmap with a BitSize of 0
     */
    void setNull() { zfree(bit); BitCapacity=0;}
    /**
     * @brief isNull returns true if bitset is a null bitmap.
     */
    bool isNull() {  return (bit==nullptr);}
    /**
     * @brief set sets the bit pointed by pBitRank (to 1)
     * @return ZS_SUCCESS if operation is successfull
     *         ZS_OUTBOUNDHIGH if pBitRank is out of biset boundary
     */
    ZStatus set(const size_t pBitRank);
    /**
     * @brief reset resets the bit pointed by pBitRank (to 0)
     * @return ZS_SUCCESS if operation is successfull
     *         ZS_OUTBOUNDHIGH if pBitRank is out of biset boundary
     */
    ZStatus reset(const size_t pBitRank);
    /**
     * @brief print displays all information about bitset to pOutput defaulted to stdout.
     */
    void print(FILE*pOutput=stdout);
    /**
     * @brief test returns true or false depending on the fact that bit pointed by pBitRank is set or not.
     * NB:
     * 1- if ZBitset is a null bitmap (isNull()==true) then any bit at any rank is reputated set,
     * so that, test will return true whatever pBitRank value is.
     * 2- if ZBitset is NOT a null bitmap, and pBitRank points out of bitmap capacity, then test() will return false
     * and an error message is issued on stderr.
     */
    bool test(const size_t pBitRank);

    inline uint16_t getByteSize(void)  {return (uint16_t)(Size*sizeof(ZBitsetType));}
    inline uint16_t getExportSize(void)
    {
      if (bit==nullptr)
        return (uint16_t)sizeof(ZTypeBase);
      return (uint16_t) (sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t)+(size_t)getByteSize());
    }
    uint16_t getEffectiveBitSize(void) {return((uint16_t)EffectiveBitSize);}
    ZDataBuffer& _exportAppendURF(ZDataBuffer& pBitsetExport);
/**
 * @brief ZBitset::_importURF imports a ZBitset structure from an URF
 * @param pPtrIn pointer to begining of URF data to import (pointing to beginning of URF header).
 * This pointer is updated to point next to imported bitset
 * @return a ZStatus with
 *  ZS_SUCCESS if bitset has be successfully downloaded
 *  ZS_OMITTED ZType_bitsetFull has been encountered: bitset is omitted and all bits are reputated to be set
 *            In this case, bitset is set to null (bitset data is nullptr).
 *  ZS_INVTYPE one of ZType_bitset or ZType_bitsetFull has not been encountered.
 */
    ZStatus _importURF(const unsigned char *&pPtrIn);
}; // ZBitset
#pragma pack(pop)
}// zbs
#endif // ZBITSET_H

