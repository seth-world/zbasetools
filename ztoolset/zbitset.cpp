#ifndef ZBITSET_CPP
#define ZBITSET_CPP
#include <ztoolset/zbitset.h>
#include <ztoolset/zexceptionmin.h>

using namespace zbs;

void
ZBitset::_allocate(const uint16_t pBitsToAllocate)
{
    Size=(uint16_t)((double)pBitsToAllocate/(double)UnitBitSize);

    if ((pBitsToAllocate % UnitBitSize))
                                    Size++;
    BitSize = Size * UnitBitSize ;      // theorical number of bits
    EffectiveBitSize = pBitsToAllocate;   // highest bit rank to be addressed : CurrentBitSize-1
    if (bit!=nullptr)
            bit =(ZBitsetType*)realloc(bit,Size);
        else
            bit=(ZBitsetType*)malloc(Size);
    if (bit==nullptr)
            {
            fprintf(stderr,"%s-F-ALLMEM Cannot allocate memory for bitset. requested size in bits is <%d> in bytes : <%d>\n",
                    _GET_FUNCTION_NAME_,
                    pBitsToAllocate,
                    UnitBitSize);
            abort();
            }
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
uint16_t wReminder= (pBitRank % UnitBitSize);
uint16_t wRank = (pBitRank /UnitBitSize) ;
    if (wRank>=Size)
        {
        fprintf(stderr,"%s-F-OVRFLW Bitset capacity overflow : acces requested bit rank <%lld> [ addressing byte <%ld>]\n"
                " while rank allowed is [0,%lld] [maximum in bytes <%ld>]\n",
                _GET_FUNCTION_NAME_,
                pBitRank,
                wRank,
                (long long)((Size*UnitBitSize)-1),
                Size*UnitByteSize);
        return ZS_OUTBOUNDHIGH;
        }

    bit[wRank] |= (1<< wReminder);
    return ZS_SUCCESS;
}
ZStatus
ZBitset::reset(const size_t pBitRank)
{
uint16_t wReminder= (pBitRank % UnitBitSize);
uint16_t wRank = (pBitRank /UnitBitSize) ;
if (wRank>=Size)
    {
    fprintf(stderr,"%s-F-OVRFLW Bitset capacity overflow : acces requested bit rank <%lld> [ addressing byte <%ld>]\n"
            " while rank allowed is [0,%lld] [maximum in bytes <%ld>]\n",
            _GET_FUNCTION_NAME_,
            pBitRank,
            wRank,
            (long long)((Size*UnitBitSize)-1),
            Size*UnitByteSize);
    return ZS_OUTBOUNDHIGH;
    }
    bit[wRank]&= ~(1<< wReminder);
    return ZS_SUCCESS;
}

void
ZBitset::print(FILE*pOutput)
{
uint8_t byte;
int wi,wRank;
int wNum=0;
    fprintf(pOutput,
            " Byte number  |");
    for (wRank=0;wRank<Size;wRank++)
    {
        fprintf(pOutput,
                "%7d|",wRank);
    }
    fprintf (pOutput,"\n");
    fprintf(pOutput,
            "      offset  |");
    for (wRank=0;wRank<Size;wRank++)
    {

        fprintf(pOutput,
                "01234567");
    }
    fprintf (pOutput,"\n");
    fprintf(pOutput,
            "Decimal offset|");
    for (wRank=0;wRank<Size;wRank++)
    {
        for (size_t wz=0;wz<8;wz++)
                {
                printf("%1d",wNum++);
                if (wNum>=10)
                        wNum=0;
                }
    }

    char wArrow[100];
    memset(wArrow,'-',sizeof(wArrow));
    for (long wi=1;wi<getByteSize();wi++)
                            wArrow[wi*10]='+';
    wArrow[getByteSize()*10]='\0';

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
            fprintf(pOutput,"%u", byte);
        }
    }
   fprintf(pOutput,"\n");
   return;
}

bool
ZBitset::test(const size_t pBitRank)
{
    uint16_t wReminder= (uint16_t)(pBitRank % UnitBitSize);
    uint16_t wRank = (uint16_t)(pBitRank /UnitBitSize) ;
    if (wRank>=Size)
        {
        fprintf(stderr,"%s-F-OVRFLW Bitset capacity overflow : acces requested bit rank <%lld> [ addressing byte <%ld>]\n"
                " while rank allowed is [0,%lld] [maximum in bytes <%ld>]\n",
                _GET_FUNCTION_NAME_,
                pBitRank,
                wRank,
                (long long)((Size*UnitBitSize)-1),
                Size*UnitByteSize);
        return false;
        }

    return ((bit[wRank]) & (1<<(wReminder)));
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
ZBitset::_exportURF(ZDataBuffer& pBitsetExport)
{
uint16_t wByteSize = (uint16_t)getByteSize();
uint16_t wEffectiveBitSize = (uint16_t)getEffectiveBitSize();
ZTypeBase wType=ZType_bitset;
unsigned char* wURFPtr;

    printf ("bitset export\n byte size %d\n effective bit size %d \n",wByteSize,wEffectiveBitSize);
     // bitset is stored as a sequence of bytes without taking UnitByteSize into account

    wURFPtr=pBitsetExport.allocateBZero(sizeof(wType)+sizeof(uint16_t)+sizeof(uint16_t)+(size_t)getByteSize());

    wURFPtr=setURFBufferValue<ZTypeBase>(wURFPtr,wType);

//    wByteSize=reverseByteOrder_Conditional<uint16_t>(wByteSize);
//    pBitsetExport.appendData(&wByteSize,sizeof(wByteSize));

    wURFPtr=setURFBufferValue<uint16_t>(wURFPtr,wByteSize);

//    wEffectiveBitSize=reverseByteOrder_Conditional<uint16_t>(wEffectiveBitSize);
//    pBitsetExport.appendData(&wEffectiveBitSize,sizeof(wEffectiveBitSize));
//    pBitsetExport.appendData(,(size_t)getByteSize());

    wURFPtr=setURFBufferValue<uint16_t>(wURFPtr,wEffectiveBitSize);
    memmove (wURFPtr,bit,wByteSize);

    return pBitsetExport;
}
/**
 * @brief ZBitset::_importURF
 * @param pBitsetPtr pointer to begining of URF data to import (pointing to beginning of header)
 * @return the pointer next to imported bitset or nullptr if an error occurred
 */
unsigned char*
ZBitset::_importURF(unsigned char* pBitsetPtr)
{
_MODULEINIT_
uint16_t    wByteSize,wEffectiveBitSize;
ZTypeBase   wType;

//    memmove(&wType,pBitsetPtr,sizeof(ZTypeBase));
//    wType=reverseByteOrder_Conditional<ZTypeBase>(wType);

    pBitsetPtr=getURFBufferValue<ZTypeBase>(&wType,pBitsetPtr);

    if (wType!=ZType_bitset)
    {
        ZException.setMessage(_GET_FUNCTION_NAME_,
                              ZS_INVTYPE,
                              Severity_Severe,
                              "Invalid ZType_type in imported data. found 0x%X <%s> while expected <ZType_bitset>",
                              wType,
                              decode_ZType(wType));
        _RETURN_ nullptr;
    }

    pBitsetPtr=getURFBufferValue<uint16_t>(&wByteSize,pBitsetPtr);

    pBitsetPtr=getURFBufferValue<uint16_t>(&wEffectiveBitSize,pBitsetPtr);

    printf ("bitset import\n byte size %d\n effective bit size %d \n",wByteSize,wEffectiveBitSize);

    _allocate(wEffectiveBitSize);
    clear();
    memmove(bit,pBitsetPtr,wByteSize);
    pBitsetPtr+=wByteSize;

    printf (" Bitset after import\n");
    print(stdout);

    _RETURN_ pBitsetPtr;
} // _importURF


#endif // ZBITSET_CPP
