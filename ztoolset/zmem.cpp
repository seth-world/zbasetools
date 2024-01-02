#include "zmem.h"

#include <ztoolset/utfvaryingstring.h>
typedef utf8VaryingString utf8VaryingString;
/*
void zfree(void*&pPtr)
{
    if (!pPtr)
            return;
    free(pPtr);
    pPtr=nullptr;
}
*/




void
zmemDumpHexa (unsigned char* pData,const long pSize,
              utf8VaryingString &pLineHexa,utf8VaryingString &pLineChar)
{
  size_t wj=0;
  unsigned char wHexa ;
  unsigned char* wPtr = pData ;

  const int wGroupFactor=4;

  int wGroup=pSize/wGroupFactor;

  pLineChar.allocateUnitsBZero(pSize*2+1); /* twice the number of possible characters because  of  "¶" */
  pLineHexa.allocateUnitsBZero((pSize*3)+wGroup+1);

//  pLineChar.clearData();
//  pLineHexa.clearData();

  int wiUtf8=0;

  int wGG=wGroupFactor;

  for (size_t wi=0; wi < pSize ; wi++)
  {
    wHexa = wPtr[wi];
    pLineHexa.addsprintf("%02X ",wHexa);
//    _Zsprintf((char*)&pLineHexa.Data[wj],"%02X ",wHexa);
    if (--wGG==0)
      {
      wGG=wGroupFactor;
      pLineHexa += " ";
      }
    if ((wPtr[wi]>31)&&(wPtr[wi]<127))
    {
      pLineChar.Data[wiUtf8]=wPtr[wi];
      wj+=3;
      wiUtf8++;
      continue;
    }

/*    if (((wPtr[wi]>6)&&(wPtr[wi]<14))||(wPtr[wi]==27))
    {
      ::strcpy((char*)&pLineChar.Data[wiUtf8],"¶");  // "¶"
      wj+=3;
      wiUtf8+=sizeof("¶")-1;
      continue;
    }*/
    pLineChar.Data[wiUtf8] = '.' ;
    wj+=3;
    wiUtf8++;
  }// for
//  pLineHexa.Data[pSize*3]='\0';
//  pLineChar.Data[wiUtf8]='\0';
  return ;
} // zmemdumpHexa

/**
 * @brief rulerSetup
 *      sets up the heading ruler for a block dump according pColumn (number of bytes per row)
 *      returns pColumn eventually modified to make text fit into rulers size
 *
 *
 * @param pRulerHexa
 * @param pRulerAscii
 * @param pColumn
 * @return the effective number of columns
 */
int zmemRulerSetup (utf8VaryingString &pRulerHexa, utf8VaryingString &pRulerAscii,int pWidth)
{
  const char *wHexaTitle = "HexaDecimal" ;
  const char *wAsciiTitle = "Ascii";

  int wColumn=pWidth;
  if (wColumn < strlen(wAsciiTitle))
    {
    wColumn=strlen(wAsciiTitle);
    }
  int wGroup=pWidth/4;

  pRulerHexa.allocateUnitsBZero ((wColumn*3)+wGroup+2);
  pRulerAscii.allocateUnitsBZero(wColumn+1);

  pRulerHexa.setChar('_');
  pRulerAscii.setChar('_');

  pRulerAscii.Data[wColumn]='\0';
  pRulerHexa.Data[(wColumn*3)+wGroup+1]='\0';


  long wSHexa = ((wColumn*3)-strlen(wHexaTitle))/2;
  long wSAscii = (wColumn-strlen(wAsciiTitle))/2;

  pRulerHexa.changeData((utf8_t*)wHexaTitle,strlen(wHexaTitle),wSHexa);
  pRulerAscii.changeData((utf8_t*)wAsciiTitle,strlen(wAsciiTitle),wSAscii);
  return wColumn;
}

/**
 * @brief zmemDump returns an utf8VaryingString with the dump of memory segment starting at pData of length pSize.
 * @param pData   pointer to segment of memory to dump
 * @param pSize   size of memory segment to dump
 * @param pWidth  Number of bytes displayed both in ascii and hexadecimal per row
 * @param pLimit  Maximum number of bytes to display
 * @return
 */
utf8VaryingString zmemDump(unsigned char* pData,const size_t pSize,const int pWidth, ssize_t pLimit)
{
  utf8VaryingString wRulerAscii;
  utf8VaryingString wRulerHexa;
  utf8VaryingString pOutput;

  int wColumn=zmemRulerSetup (wRulerHexa, wRulerAscii,pWidth);

  utf8VaryingString wLineChar;
  utf8VaryingString wLineHexa;

  if ((!pData)||(!pSize))
  {
    pOutput="<null>";
    return pOutput;
  }

  long    wLimit;
  long    wReminder;

  const int wGroupFactor=4;
  int wGroup=pSize/wGroupFactor;


  if ( (pLimit > 0 ) && ((size_t)pLimit < pSize ) )
    wReminder = wLimit = (size_t)pLimit;
  else
    wReminder = wLimit = pSize;

  pOutput.sprintf("____Offset____ %s %s\n",wRulerHexa.toCChar(),wRulerAscii.toCChar());

  size_t wOffset=0;
  int wL=0;
  //    while ((wOffset+(size_t)pColumn) < wLimit)
  while ( wReminder > (size_t)pWidth )
  {
    zmemDumpHexa(pData+wOffset,pWidth,wLineHexa,wLineChar);
    pOutput.addsprintf("%6X-%6d |%s |%s|\n",wL,wL,wLineHexa.toCChar(),wLineChar.toCChar());
    wL+=pWidth;
    wOffset+=(pWidth);
    wReminder -= pWidth;
  }

  if (pLimit < pSize)
  {
    pOutput.addsprintf(" <%ld> bytes more not dumped.\n",pSize-pLimit);
    return pOutput;
  }


  wLineChar.allocateUnitsBZero(pWidth*2+1); /* twice the number of possible characters because  of  "¶" */
  wLineHexa.allocateUnitsBZero((pWidth*3)+wGroup+1);

  unsigned char* wPtr = pData + wOffset;

  int wGG=wGroupFactor;
  int wi=0;
  while (wReminder > 0)
    {
    wReminder--;
    wLineHexa.addsprintf("%02X ",*wPtr);
    if (--wGG==0)
      {
      wGG=wGroupFactor;
      wLineHexa += " ";
      }

    if ((*wPtr>31)&&(*wPtr<127))
        wLineChar.Data[wi++]=*wPtr;
      else
        wLineChar.Data[wi++]='.';
      wPtr++;
    }//while

  while (wi < pWidth)
    {
    wLineHexa.addsprintf("   ",*wPtr);
    if (--wGG==0)
      {
      wGG=wGroupFactor;
      wLineHexa += " ";
      }
    wLineChar.Data[wi++]=' ';
    }

    pOutput.addsprintf("%6X-%6d |%s |%s|\n",wL,wL,wLineHexa.toCChar(),wLineChar.toCChar());

    /* count utf8 characters */
/*    for (long wi=0;(wLineChar.Data[wi] != 0) && (wi < wLineChar.UnitCount);wi++)
    {
      if ( wLineChar.Data[wi] > 127)
      {
        wUtf8Column ++;
        wi++;     // it is an utf8, skip next (hoping not three units per char
      }
    }
*/
/*    char wFormat [50];

    sprintf (wFormat,"#6X-#6d |#-%ds |#-%ds|\n",(pWidth*3)+wGroup,pWidth);
//    sprintf (wFormat,"#6X-#6d |#-%ds |#-%ds|\n",(pWidth*3)+wGroup,wUtf8Column);
    wFormat[0]='%';
    for (int wi=0;wFormat [wi]!='\0';wi++)
      if (wFormat[wi]=='#')
        wFormat[wi]='%';
*/

  return pOutput;
}//zmemDump

