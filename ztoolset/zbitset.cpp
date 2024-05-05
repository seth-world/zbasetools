#ifndef ZBITSET_CPP
#define ZBITSET_CPP
#include <ztoolset/zbitset.h>
#include <ztoolset/zexceptionmin.h>

using namespace zbs;


ZBitset&
ZBitset::_copyFrom(const ZBitset& pIn)
{
    Size=pIn.Size;
    BitCapacity=pIn.BitCapacity;
    EffectiveBitSize=pIn.EffectiveBitSize;

    if (bit)
        zfree(bit);

    bit = zmalloc<ZBitsetType>(pIn.Size*sizeof(ZBitsetType));
    size_t wi=0 ;
    while (wi < Size) {
        bit[wi] = pIn.bit[wi];
    }
    return *this;
}

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
ZBitset::setAll()
{
  if (bit==nullptr) {
    return;
  }
  for (int wi=0;wi < Size;wi++)
    bit[wi]=0xFF;
}
void
ZBitset::resetAll()
{
  for (int wi=0;wi < Size;wi++)
    bit[wi]=0;
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

utf8VaryingString
ZBitset::toString() {

  utf8VaryingString wReturn;
  utf8_t wByte;
  if (isFullBitset())
        return "";
  wReturn.allocateUnitsBZero((Size*8)+1);

  utf8_t* wPtr = wReturn.Data;
  size_t wBitSize = 0;
//  for (size_t wRank=0 ; (wRank < Size) && ( wBitSize < EffectiveBitSize ) ; wRank++) {
  for (size_t wRank=0 ; ( wBitSize < EffectiveBitSize ) ; wRank++) {
    for (long wi=0 ; ( wi < 8 ) && ( wBitSize < EffectiveBitSize )  ; wi++) {
//     wByte = ((bit[wRank] >> wi) & 1)? '1':' ';
      *wPtr++ = utf8_t (((bit[wRank] >> wi) & 1)? '1':'-') ;
      wBitSize++;
    } // for wi
  } // for wRank
  *wPtr=0;
  return wReturn;
} // toString

bool
ZBitset::test(const size_t pBitRank) const
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

ssize_t
ZBitset::_exportURF(ZDataBuffer& pBitsetExport) const
{
ZTypeBase wType=ZType_bitset;
unsigned char* wURFPtr;

    if (bit==nullptr)
      {
      return ssize_t( _exportAtomic<ZTypeBase>(ZType_bitsetFull,&pBitsetExport));
      }

      wURFPtr=pBitsetExport.extendBZero(getURFSize()); /* create room */

      return _exportURF_Ptr(wURFPtr);
}//_exportURF

size_t
ZBitset::getURFHeaderSize() {
  return sizeof(ZTypeBase) + sizeof(uint16_t) + sizeof(uint16_t) ;
}

ssize_t
ZBitset::_exportURF_Ptr(unsigned char*& pURF) const
{
  uint16_t wByteSize = (uint16_t)getByteSize();
  uint16_t wEffectiveBitSize = (uint16_t)getEffectiveBitSize();

//  _DBGPRINT ("bitset export\n byte size %d\n effective bit size %d \n",wByteSize,wEffectiveBitSize)
  // bitset is stored as a sequence of bytes without taking UnitByteSize into account

  if (bit==nullptr) {
    _exportAtomicPtr<ZTypeBase>(ZType_bitsetFull,pURF);
    return ssize_t(sizeof(ZTypeBase));
  }

  _exportAtomicPtr<ZTypeBase>(ZType_bitset,pURF);
  _exportAtomicPtr<uint16_t>(getByteSize(),pURF);
  _exportAtomicPtr<uint16_t>(getEffectiveBitSize(),pURF);
  uint8_t* wBitPtr = (uint8_t*)pURF;
  for (int wi=0;wi < (Size*sizeof(ZBitsetType));wi++)
    wBitPtr[wi] = bit[wi];
//  memmove (pURF,bit,Size*sizeof(ZBitsetType));
  pURF += (size_t)getByteSize();
  return ssize_t(sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t)+(Size*sizeof(ZBitsetType)));
}//_exportURF_Ptr

ssize_t
ZBitset::_importURF(const unsigned char* &pPtrIn)
{
uint16_t    wByteSize,wEffectiveBitSize;
ZTypeBase   wType;

    _importAtomic<ZTypeBase>(wType,pPtrIn);

    if (wType!=ZType_bitset)
    {
      if (wType==ZType_bitsetFull)
        {
        setFullBitset();
        return ssize_t(sizeof(ZTypeBase));
        }
      /* if not ZType_bitset then it is unexpected */
      ZException.setMessage("ZBitset::_importURF",
                              ZS_INVTYPE,
                              Severity_Error,
                              "Invalid ZType_type in imported data. found 0x%X <%s> while expected one of <ZType_bitset> or <ZType_bitsetFull>",
                              wType,
                              decode_ZType(wType));
        return  -1;
    }//if (wType!=ZType_bitset)

    _importAtomic<uint16_t>(wByteSize,pPtrIn);
    _importAtomic<uint16_t>(wEffectiveBitSize,pPtrIn);

//    printf ("bitset import\n byte size %d\n effective bit size %d \n",wByteSize,wEffectiveBitSize);
    size_t wByteSizeS=size_t(wByteSize);
    /* allocate routine specific to ZBitset that allocates bytes from a number of bits */
    /* sets EffectiveBitSize and BitCapacity appropriately */
    _allocate(wEffectiveBitSize);
    clear();
    const uint8_t* wBitPtr=(const uint8_t*)pPtrIn;
    for (int wi=0; wi < wByteSize;wi++){
      bit[wi]=wBitPtr[wi];
    }
    pPtrIn += wByteSizeS;

    return  ssize_t(sizeof(ZTypeBase) + wByteSizeS + sizeof (uint16_t) + sizeof (uint16_t));
} // _importURF
size_t
ZBitset::getURFSize(void) const
{
  if (bit==nullptr)
    return (uint16_t)sizeof(ZTypeBase);
  return (uint16_t) (sizeof(ZTypeBase)+sizeof(uint16_t)+sizeof(uint16_t)+(size_t)getByteSize());
}
size_t
ZBitset::getUniversalSize(void) const
{
  if (bit==nullptr)
    return 0;
  return (Size*sizeof(ZBitsetType));
}
#endif // ZBITSET_CPP
