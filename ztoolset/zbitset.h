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
#pragma pack(0)
class ZBitset
{
public:
    typedef uint8_t ZBitsetType;
    ZBitsetType *bit=nullptr;                   // bitset content itself
    uint16_t Size=0;                            // number of ZBitsetType to allocate (of UnitByteSize bytes each)
    uint16_t BitSize=0;                         // total number of bits it represents
    uint16_t EffectiveBitSize=0;                  // number of effective bits requested for allocation: to be exported/imported
    const uint16_t UnitByteSize=sizeof(ZBitsetType); // size in bytes of one ZBitType
    const uint16_t UnitBitSize=UnitByteSize*8;    // size in bits of one ZBitType

    ZBitset() {}
    ZBitset(const uint16_t pBitsToAllocate) {_allocate(pBitsToAllocate);}
    ~ZBitset() {if (bit) free (bit);}

    size_t getbitSize() {return BitSize;}

    void _allocate(const uint16_t pBitsToAllocate);
    void clear() ;
    ZStatus set(const size_t pBitRank);
    ZStatus reset(const size_t pBitRank);
    void print(FILE*pOutput=stdout);
    bool test(const size_t pBitRank);

    uint16_t getByteSize(void)  {return (uint16_t)(Size*UnitByteSize);}
    uint16_t getExportSize(void) {return (uint16_t) (getByteSize()+sizeof(ZTypeBase)+sizeof(uint32_t)+sizeof(size_t)); }
    uint16_t getEffectiveBitSize(void) {return((uint16_t)EffectiveBitSize);}
    ZDataBuffer& _exportURF(ZDataBuffer& pBitsetExport);
    unsigned char *_importURF(unsigned char* pBitPtr);
}; // ZBitset
#pragma pack(pop)
}// zbs
#endif // ZBITSET_H

