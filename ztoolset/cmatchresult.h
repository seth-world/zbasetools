#ifndef CMATCHRESULT_H
#define CMATCHRESULT_H

#include "zarray.h"

namespace zbs {


class CMatchResult
{
public:
    CMatchResult()=default;
    CMatchResult(const CMatchResult& pIn) {_copyFrom(pIn); }
    CMatchResult(int pMatch,int pRadix) : Match(pMatch),Radix(pRadix) {}
    CMatchResult& _copyFrom(const CMatchResult& pIn);
    CMatchResult& operator = (const CMatchResult& pIn) {return _copyFrom(pIn);}

    void clear()
    {
        Match=0;
        Radix=0;
    }

    int Match=0;
    int Radix=0;
};


class CWeight : public CMatchResult
{
public:
    CWeight()=default;
    CWeight(int pMatch,int pRadix,int pIndex) : CMatchResult(pMatch,pRadix),Index(pIndex) {}
    CWeight(CMatchResult pMR,int pIndex) : CMatchResult(pMR),Index(pIndex) {}
    /*
    int Match=0;
    int Radix=0;
*/
    int Index=0;
};

class CWeightList : public ZArray<CWeight>
{
public:
    CWeightList()=default;
    CWeightList(CWeightList&)=delete;
    /* add in decreasing order */

    void add(CWeight& pIn);

    void add(int pMatch,int pRadix,int pIndex)
    {
        CWeight wW(pMatch,pIndex,pRadix);
        add(wW);
    }
    void add(CMatchResult pMR,int pIndex)
    {
        CWeight wW(pMR,pIndex);
        add(wW);
    }
} ;

} // namespace zbs
#endif // CMATCHRESULT_H
