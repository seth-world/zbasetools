#include "utfutils.h"
// instantiations

template<unsigned char> size_t utfStrlen(const unsigned char* pString);


const char*
getUnitFormat(uint8_t pSize)
{
  switch(pSize)
  {
  case 1:
    return "UTF-8";
  case 2:
    return "UTF-16";
  case 4:
    return "UTF-32";
  default:
    return "Invalid char unit size";
  }
}


#include <ztoolset/zdatabuffer.h>

size_t _getexportCharArrayUVFSize(const char*pDataIn)
{
  UVF_Size_type wUnitCount = 0;
  while (pDataIn[wUnitCount]!=0)
    wUnitCount++;
  return (ssize_t)wUnitCount+sizeof(UVF_Size_type)+1;
}

void _exportCharArrayUVF(const char*pDataIn,ZDataBuffer& wReturn)
{
  UVF_Size_type wUnitCount = 0;
  while (pDataIn[wUnitCount]!=0)
    wUnitCount++;
  unsigned char* wPtrTarg=wReturn.extend((ssize_t)wUnitCount+sizeof(UVF_Size_type)+1);
  /* set export data with char unit size */
  *wPtrTarg = (uint8_t)sizeof (utf8_t);
  wPtrTarg++;
  /* prepare and set unit count to export data */
  UVF_Size_type wUnitCount_Export=reverseByteOrder_Conditional<UVF_Size_type>(wUnitCount);
  memmove(wPtrTarg,&wUnitCount_Export,sizeof(UVF_Size_type));
  wPtrTarg += sizeof(UVF_Size_type);

  while (*pDataIn)
      *wPtrTarg++=*pDataIn++;

  return ;
}//_exportCharArrayUVF

size_t _getimportCharArrayUVFSize(unsigned char*& pUniversalPtr)
{
  errno=0;
  unsigned char* wPtrSrc = pUniversalPtr;
  /* get and control char unit size */
  uint8_t wUnitSize=(uint8_t)*wPtrSrc;
  if (wUnitSize!=sizeof(char))
  {
    errno=EPERM;
    fprintf(stderr,"_importUVF-E-IVUSIZE Imported string format <%s> does not correspond to current string format <char>",
        getUnitFormat(wUnitSize));
    return 0;
  }
  wPtrSrc++;
  /* get char units to load excluding (_Utf)'\0' mark */
  UVF_Size_type    wUnitCount;
  size_t wLen = _importAtomic<UVF_Size_type>(wUnitCount,wPtrSrc);
  return (wUnitCount*sizeof(char))+sizeof(UVF_Size_type)+1;
}//_getCharArrayUVFSize

size_t _importCharArrayUVF(char* pDataOut,size_t pMaxSize,unsigned char*& pUniversalPtr)
{
  errno=0;
  unsigned char* wPtrSrc = pUniversalPtr;
  /* get and control char unit size */
  uint8_t wUnitSize=(uint8_t)*wPtrSrc;
  if (wUnitSize!=sizeof(char))
  {
    errno=EPERM;
    fprintf(stderr,"_importUVF-E-IVUSIZE Imported string format <%s> does not correspond to current string format <char>",
        getUnitFormat(wUnitSize));
    return 0;
  }
  wPtrSrc++;
  /* get char units to load excluding (_Utf)'\0' mark */
  UVF_Size_type    wUnitCount;
  size_t wLen = _importAtomic<UVF_Size_type>(wUnitCount,wPtrSrc);
  if (wUnitCount > pMaxSize)
  {
    fprintf(stderr,"uriString::_importUVF-W-TRUNC Overflow : imported string length <%d> truncated to <%ld>.\n",
        wUnitCount, pMaxSize);
    errno=ENOMEM;
  }
  /* import string per char unit */

  char* wPtrOut=pDataOut ;
  char* wPtrIn=(char*)(wPtrSrc);
  char* wPtrEnd = &wPtrIn[wUnitCount];
  while (wPtrIn < wPtrEnd)
    *wPtrOut++=*wPtrIn++;
  *wPtrEnd = 0;
  pUniversalPtr +=(wUnitCount*sizeof(char))+sizeof(UVF_Size_type)+1;
  return (wUnitCount*sizeof(char))+sizeof(UVF_Size_type)+1;
}//_importCharArrayUVF
