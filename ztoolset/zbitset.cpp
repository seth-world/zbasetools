#ifndef ZBITSET_CPP
#define ZBITSET_CPP
#include <ztoolset/zbitset.h>
#include <ztoolset/zexceptionmin.h>

using namespace zbs;

void
ZBitset::_allocate(const size_t pBitsToAllocate)
{
  errno=0;

    Size=(size_t)((double)pBitsToAllocate/(double)UnitBitSize);

    if ((pBitsToAllocate % UnitBitSize) > 0)
                                    Size++;
    BitCapacity = Size * UnitBitSize ;    // theorical number of bits
    EffectiveBitSize = pBitsToAllocate;   // highest bit rank to be addressed : CurrentBitSize-1
    if (bit!=nullptr)
          bit =zrealloc(bit,Size);
        else
          bit=zmalloc<ZBitsetType>(Size);

    for (int wi=0; wi < Size ; wi++)
      bit[wi]=0;
    return;
}// _allocate


void
ZBitset::clear()
{
    if (bit==nullptr)
        return;
    memset(bit,0,Size);
    return;
}


ZStatus
ZBitset::set(const size_t pBitRank)
{
size_t wReminder= (pBitRank % UnitBitSize);
size_t wByteRank = (pBitRank /UnitBitSize) ;
    if (wByteRank >= Size)
        {
        fprintf(stderr,"ZBitset::set-F-OVRFLW Bitset capacity overflow : acces requested bit rank <%ld> [ addressing byte <%d>]\n"
                " while rank allowed is [0,%ld] [maximum in bytes <%d>]\n",
                pBitRank,
                wByteRank,
                (long)((Size*UnitBitSize)-1),
                Size*UnitByteSize);
        return ZS_OUTBOUNDHIGH;
        }

    bit[wByteRank] |= (1 << wReminder);
    return ZS_SUCCESS;
}
ZStatus
ZBitset::reset(const size_t pBitRank)
{
size_t wReminder= (pBitRank % UnitBitSize);
size_t wByteRank = (pBitRank /UnitBitSize) ;
if (wByteRank>=Size)
    {
    fprintf(stderr,"%s-F-OVRFLW Bitset capacity overflow : acces requested bit rank <%lld> [ addressing byte <%ld>]\n"
            " while rank allowed is [0,%lld] [maximum in bytes <%ld>]\n",
            _GET_FUNCTION_NAME_,
            pBitRank,
            wByteRank,
            (long long)((Size*UnitBitSize)-1),
            Size*UnitByteSize);
    return ZS_OUTBOUNDHIGH;
    }
    bit[wByteRank] &= ~(1 << wReminder);
    return ZS_SUCCESS;
}

void
ZBitset::print(FILE*pOutput)
{
uint8_t byte;
int wi,wRank;
int wNum=0;
/*    fprintf(pOutput,
            " Byte number  |");
    for (wRank=0;wRank<Size;wRank++)
    {
        fprintf(pOutput,
                "%-7d|",wRank);
    }
    fprintf (pOutput,"\n");
    fprintf(pOutput,
            " Byte offset  |");
    for (wRank=0;wRank<Size;wRank++)
    {
        fprintf(pOutput,
                "01234567");
    }
    fprintf (pOutput,"\n");
*/
    fprintf(pOutput,
        "Decimal offset|");
    for (wRank=0;wRank < (1 + getbitSize()/10);wRank++)
    {
      fprintf(pOutput,
          "%-9d|",wRank);
    }
    fprintf (pOutput,"\n");
    fprintf(pOutput,
            "              |");

    for (wRank=0;wRank < getbitSize();wRank++)
          {
          fprintf(pOutput,"%1d",wNum++);
          if (wNum>=10)
            wNum=0;
          }


    char* wArrow=zmalloc<char>(getbitSize()+2);

    memset(wArrow,'-',getbitSize());
    for (long wi=10;wi<getbitSize();)
      {
      wArrow[wi]='+';
      wi += 10 ;
      }
    int wEndArrow=getbitSize();
    wArrow[wEndArrow++]='+';
    wArrow[wEndArrow]='\0';

    fprintf (pOutput,
             "\n"
             "              +%s\n",
             (char*)wArrow);


    fprintf(pOutput,
            "       content|");
    for (wRank=0;wRank<Size;wRank++)
    {
        for (wi=0;wi<8;wi++)
        {
            byte = (bit[wRank] >> wi) & 1;
//            fprintf(pOutput,"%u", byte);
            fprintf(pOutput,"%c", byte?'1':' ');
        }
    }
   fprintf(pOutput,"|\n");

   fprintf (pOutput,
       "              +%s\n",
       (char*)wArrow);

   zfree(wArrow);
   return;
}

bool
ZBitset::test(const size_t pBitRank)
{
  if (isNull())
    return true;

    size_t wReminder= (pBitRank % UnitBitSize);
    size_t wByteRank = (pBitRank /UnitBitSize) ;
    if (wByteRank >= Size)
        {
        fprintf(stderr,"ZBitset::test-F-OVRFLW Bitset capacity overflow : acces requested bit rank <%ld> [ addressing byte <%ld>]\n"
                " while rank allowed is [0,%ld] [maximum in bytes <%ld>]\n",
                pBitRank,
                wByteRank,
                ((Size*UnitBitSize)-1),
                Size*UnitByteSize);
        return false;
        }

    return bool((bit[wByteRank]) & (1<<(wReminder)));
}

/**
 * @brief ZBitset::_export
 *
 * ZBitset export format
 *    +---------------------------------------> ZType_bitset
 *    |    +----------------------------------> uint16_t : size of bitset in bytes
 *    |    |   +------------------------------> uint16_t : CurrentBitSize
 *    |    |   |          +-------------------> bitset content
 *    |    |   |          |
 *  |...|...|...|------------------->
 *  0   4   8   12
 *
 *
 * @param pBitsetExport
 * @return
 */

ZDataBuffer&
ZBitset::_exportAppendURF(ZDataBuffer& pBitsetExport)
{
uint16_t wByteSize = (uint16_t)getByteSize();
uint16_t wEffectiveBitSize = (uint16_t)getEffectiveBitSize();
ZTypeBase wType=ZType_bitset;
unsigned char* wURFPtr;

    printf ("bitset export\n byte size %d\n effective bit size %d \n",wByteSize,wEffectiveBitSize);
     // bitset is stored as a sequence of bytes without taking UnitByteSize into account

    if (bit==nullptr)
      {
      _exportAtomic<ZTypeBase>(ZType_bitsetFull,pBitsetExport);
      return pBitsetExport;
      }

    wURFPtr=pBitsetExport.extendBZero((size_t)getExportSize()); /* create room */

    wURFPtr=setURFBufferValue<ZTypeBase>(wURFPtr,wType);

    wURFPtr=setURFBufferValue<uint16_t>(wURFPtr,getByteSize());

    wURFPtr=setURFBufferValue<uint16_t>(wURFPtr,wEffectiveBitSize);
    memmove (wURFPtr,bit,(size_t)getByteSize());

    return pBitsetExport;
}//_exportAppendURF

ZStatus
ZBitset::_importURF(const unsigned char* &pPtrIn)
{

uint16_t    wByteSize,wEffectiveBitSize;
ZTypeBase   wType;

//    memmove(&wType,pBitsetPtr,sizeof(ZTypeBase));
//    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);

    pPtrIn=getURFBufferValue<ZTypeBase>(&wType,pPtrIn);

    if (wType!=ZType_bitset)
    {
      if (wType==ZType_bitsetFull)
        {
        setNull();
        return ZS_OMITTED;
        }
      /* if not ZType_bitset then it is unexpected */
      ZException.setMessage("ZBitset::_importURF",
                              ZS_INVTYPE,
                              Severity_Error,
                              "Invalid ZType_type in imported data. found 0x%X <%s> while expected one of <ZType_bitset> or <ZType_bitsetFull>",
                              wType,
                              decode_ZType(wType));
        return  ZS_INVTYPE;
    }//if (wType!=ZType_bitset)

    pPtrIn=getURFBufferValue<uint16_t>(&wByteSize,pPtrIn);

    pPtrIn=getURFBufferValue<uint16_t>(&wEffectiveBitSize,pPtrIn);

//    printf ("bitset import\n byte size %d\n effective bit size %d \n",wByteSize,wEffectiveBitSize);

    _allocate(wEffectiveBitSize);
    clear();
    memmove(bit,pPtrIn,wByteSize);
    pPtrIn+=wByteSize;

    return  ZS_SUCCESS;
} // _importURF


#endif // ZBITSET_CPP
