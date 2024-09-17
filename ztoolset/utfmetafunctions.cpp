#include "utfmetafunctions.h"

namespace zbs {


utf8VaryingString
searchAdHocWeighted(const utf8VaryingString& pIn, ZArray<utf8VaryingString>& pKeyArray)
{
    CWeightList WeightList ;

    if (pKeyArray.count()==0)
        return utf8VaryingString();

//    utf8_t* wPtrIn=pIn.Data ;
    size_t wInLen=pIn.strlen();

    int wMinimum = 0;

    if ( wInLen < 4 )
        wMinimum = (wInLen - 1)>2?(wInLen - 1):wInLen;
    else
        wMinimum = wInLen - (wInLen * 30 / 100) ;


    int wi=0 ;
    for (; wi < pKeyArray.count() ; wi++) {
        CWeight wWeight;
        CMatchResult wMR;
        if(matchWeight(wMR,pIn,pKeyArray[wi],wMinimum)) {
            wWeight.Index = wi ;
            wWeight.Match = wMR.Match;
            wWeight.Radix = wMR.Radix;
            WeightList.add(wWeight);
        }
    }// for

    /* return the highest pounded if match is more than minimum */

    if (WeightList.count()>0)
        return pKeyArray[WeightList.Tab(0).Index] ;

    return utf8VaryingString();
}//searchAdHocWeighted

utf8VaryingString
searchAdHocWeighted(const utf8VaryingString& pIn,
                    ZArray<utf8VaryingString>& pKeyArray,
                    int pMatchMin ,
                    int pRadixMin )
{
    CWeightList WeightList ;

    if (pKeyArray.count()==0)
        return utf8VaryingString();

    //    utf8_t* wPtrIn=pIn.Data ;
    size_t wInLen=pIn.strlen();

    int wMinimum = 0;

    if ( wInLen < 4 )
        wMinimum = (wInLen - 1)>2?(wInLen - 1):wInLen;
    else
        wMinimum = wInLen - (wInLen * 30 / 100) ;


    int wi=0 ;
    for (; wi < pKeyArray.count() ; wi++) {
        CWeight wWeight;
        CMatchResult wMR;
        if(matchWeight(wMR,pIn,pKeyArray[wi],wMinimum,pRadixMin)) {
            wWeight.Index = wi ;
            wWeight.Match = wMR.Match;
            wWeight.Radix = wMR.Radix;
            WeightList.add(wWeight);
        }
    }// for

    /* return the highest pounded if match is more than minimum */

    if (WeightList.count()>0)
        return pKeyArray[WeightList.Tab(0).Index] ;

    return utf8VaryingString();
}//searchAdHocWeighted




#ifdef __COMMENT__
utf8VaryingString
searchKeywordWeighted(const utf8VaryingString& pIn)
{
    CWeightList WeightList ;
    ZArray<ZSearchKeyWord>* wKeyWordList=&KeywordList;// for debug
    WeightList.clear();

    utf8_t* wPtrIn=pIn.Data ;
    size_t wInLen=pIn.strlen();
    int wMin = wInLen - 3 ;
    if ( wInLen < 4 )
        wMin = (wInLen - 1)>2?(wInLen - 1):wInLen;
    int wMatch=0 ;
    int wi=0 ;
    for (; wi < KeywordList.count() ; wi++) {
        wMatch=0;
        CWeight wWeight;
        wPtrIn=pIn.Data ;
        utf8_t* wPtrKw=KeywordList[wi].Text.Data ;
        while (*wPtrIn && *wPtrKw ) {
            if ( utfUpper(*wPtrIn) == utfUpper(*wPtrKw))
                wMatch++;
            wPtrIn++;
            wPtrKw++;
        }// while
        if (wMatch>0) {
            wWeight.Match = wMatch ;
            wWeight.Index = wi ;
            WeightList.add(wWeight);
        }
    }// for

    /* return the highest pounded if match is more than minimum */

    if (WeightList.count()>0)
        if ( WeightList.Tab(0).Match >= wMin )
            return KeywordList[WeightList.Tab(0).Index].Text ;

    return utf8VaryingString();
}//searchKeywordWeighted
#endif // __COMMENT__


class CWeightList  WeightList ;



bool
matchWeight(CMatchResult& pResult,const utf8VaryingString& pIn,const utf8VaryingString& pToMatch,int pMatchMin,int pRadixMin)
{
    pResult.clear();
    int wRadCur=0;
    size_t wInLen=0 , wKwLen=0 ;
    utf8_t* wPtrIn=pIn.Data ;
    utf8_t* wPtrKw=pToMatch.Data ;

    /* get size of word to match */
    while (*wPtrKw) {
        wPtrKw++;
        wKwLen++;
    }

    /* first get match count */
    while (*wPtrIn) {
        wPtrKw=pToMatch.Data ;

        while (*wPtrIn && *wPtrKw) {
            if ( utfUpper(*wPtrIn) == utfUpper(*wPtrKw)) {
                pResult.Match++;
                break;
            }
            wPtrKw++;
        }// while
        wInLen++;
        wPtrIn++;
    }// while


    /* get consecutive count : radix
     * search consecutive sequences of segments of keyword in input string
     *
     *  for each character of input string
     *   search for possible substrings matches in keyword string
     *
    */
    utf8_t* wPtrIn1=wPtrIn=pIn.Data ;

    while (*wPtrIn) {
        wPtrKw=pToMatch.Data ;
        wPtrIn1 = wPtrIn;
        while (*wPtrIn1 && *wPtrKw) {
            if ( utfUpper(*wPtrIn1) == utfUpper(*wPtrKw)) {
                wRadCur++;
                if ( wRadCur > pResult.Radix )
                    pResult.Radix = wRadCur;
                wPtrKw++;
                wPtrIn1++;
                continue ;
            }
            wRadCur=0;
            wPtrIn1 = wPtrIn;
            wPtrKw++;
        }// while

        wInLen++;
        wPtrIn++;
    }// while
    /*
    if ( wRadCur > pResult.Radix )
        pResult.Radix = wRadCur;
*/
    if (pMatchMin < 1) {
        if ( wInLen < 4 )
            pMatchMin = (wInLen - 1) > 2 ? (wInLen - 1):wInLen;
        else
            pMatchMin = std::min(wInLen,wKwLen) * 40 / 100 ;
    }
    if (pRadixMin < 1) {
        if ( wInLen < 4 )
            pRadixMin = pMatchMin - 2 >= 2 ? pMatchMin - 1 : pMatchMin  ;
        else
            pRadixMin = std::min(wInLen,wKwLen) * 20 / 100 ;
    }
    if ((pResult.Match < pMatchMin) || (pResult.Radix < pRadixMin))
        return false;

    return true ;
} // matchWeight




utf8VaryingString
leftPad(const utf8VaryingString& pString,int pSize,utf8_t pPadChar)
{
    utf8VaryingString wReturn;
    ssize_t wPad = ssize_t(pSize - pString.strlen());
    if (wPad > 0)
        wReturn.setChar(pPadChar,0,wPad);
    wReturn += pString;
    return wReturn;
}

utf8VaryingString
rightPad(const utf8VaryingString& pString,int pSize,utf8_t pPadChar)
{
    utf8VaryingString wReturn = pString ;
    ssize_t wPad = ssize_t(pSize - pString.strlen());
    if (wPad > 0)
        wReturn.addChar(pPadChar,wPad);
    return wReturn;
}


} // namespace zbs
