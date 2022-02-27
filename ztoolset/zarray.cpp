#ifndef ZARRAY_CPP
#define ZARRAY_CPP

#include "zarray.h"
#include <ztoolset/zdatabuffer.h>

using namespace zbs;

void ZAExport::setFromPtr(const unsigned char *&pPtrIn)
{
  memmove(this,pPtrIn,sizeof(ZAExport));
  pPtrIn += sizeof(ZAExport);
}

ZAExport& ZAExport::_copyFrom(const ZAExport& pIn)
{
  EndianCheck = pIn.EndianCheck;

  FullSize =     pIn.FullSize;
  AllocatedSize =    pIn.AllocatedSize;
  CurrentSize =     pIn.CurrentSize;
  DataSize =     pIn.DataSize;
  AllocatedElements =     pIn.AllocatedElements;
  ExtentQuota= pIn.ExtentQuota;
  InitialAllocation =     pIn.InitialAllocation;
  NbElements =     pIn.NbElements;
  return *this;
}

void ZAExport::_convert ()
{
  if (!is_little_endian())
    return;
  EndianCheck = reverseByteOrder_Conditional(EndianCheck);

  FullSize =     reverseByteOrder_Conditional(FullSize);
  AllocatedSize =     reverseByteOrder_Conditional(AllocatedSize);
  CurrentSize =     reverseByteOrder_Conditional(CurrentSize);
  DataSize =     reverseByteOrder_Conditional(DataSize);
  AllocatedElements =     reverseByteOrder_Conditional(AllocatedElements);
  ExtentQuota=reverseByteOrder_Conditional(ExtentQuota);
  InitialAllocation =     reverseByteOrder_Conditional(InitialAllocation);
  NbElements =     reverseByteOrder_Conditional(NbElements);

}

void ZAExport::serialize()
{
  if (!is_little_endian())
    return ;
  if (isReversed())
  {
    fprintf (stderr,"ZAExport::serialize-W-ALRDY ZAExport block already serialized.\n");
    return;
  }
  _convert();
}
void ZAExport::deserialize()
{
  if (!is_little_endian())
    return ;
  if (isNotReversed())
  {
    fprintf (stderr,"ZAExport::deserialize-W-ALRDY ZAExport already deserialized. \n");
    return;
  }
  _convert();
}


ZAExport ZAEimport(const unsigned char* pBuffer)
{
  ZAExport wZAE;
  wZAE.setFromPtr(pBuffer);
  wZAE.deserialize();
  return wZAE;
}




#endif
