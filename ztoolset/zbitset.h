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

/**
 * @brief The ZBitset class
 * Full bitset:
 * A full bitset has a no storage (bit==nullptr).
 * - getbitSize() getEffectivebitsNumber() return 0
 * - test(xxx), returns true whatever given rank is
 */

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
    ZBitset(const uint16_t pBitsToAllocate) {
      _allocate(pBitsToAllocate);
      clear();
    }
    ~ZBitset() {if (bit) zfree (bit);}

    /**
     * @brief getbitSize returns the capacity of bitset in bits.
     */
    size_t getbitSize() const {return BitCapacity;}
    /**
     * @brief getEffectivebitsNumber returns the effective number of bits.
     */
    size_t getEffectivebitsNumber() const {return EffectiveBitSize;}
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
    void setFullBitset() { zfree(bit); BitCapacity=0;}
    /**
     * @brief isNull returns true if bitset is a null bitmap.
     */
    bool isNull() const {  return (bit==nullptr);}
    bool isFullBitset() const {  return (bit==nullptr);}
    /**
     * @brief set sets the bit pointed by pBitRank (to 1)
     * @return ZS_SUCCESS if operation is successfull
     *         ZS_OUTBOUNDHIGH if pBitRank is out of biset boundary
     */
    ZStatus set(const size_t pBitRank);
    void setAll();
    void resetAll();
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
    bool test(const size_t pBitRank) const;

    inline uint16_t     getByteSize(void) const  {return (uint16_t)(Size*sizeof(ZBitsetType));}
    size_t              getURFSize(void) const;
    static size_t       getURFHeaderSize() ;
    size_t              getUniversalSize(void) const ;

    uint16_t getEffectiveBitSize(void) const {return((uint16_t)EffectiveBitSize);}
    ssize_t _exportURF(ZDataBuffer& pBitsetExport) const;
    ssize_t _exportURF_Ptr(unsigned char*& pURF) const;

    utf8VaryingString toString() ;

/**
 * @brief ZBitset::_importURF imports a ZBitset structure from an URF
 * @param pPtrIn pointer to begining of URF data to import (pointing to beginning of URF header).
 * This pointer is updated to point next to imported bitset
 * @return a ZStatus with
 *  bitset size       :  bitset has be successfully downloaded
 *  sizeof(ZTypeBase) : ZType_bitsetFull has been encountered: bitset is omitted and all bits are reputated to be set
 *            In this case, bitset is set to null (bitset data is nullptr).
 *  0                 : invalid ZType_type - one of ZType_bitset or ZType_bitsetFull has not been encountered.
 */
ssize_t _importURF(const unsigned char *&pPtrIn);

}; // ZBitset
#pragma pack(pop)
}// zbs
#endif // ZBITSET_H

