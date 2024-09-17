#include "cmatchresult.h"

namespace zbs {


CMatchResult& CMatchResult::_copyFrom(const CMatchResult& pIn)
{
    Match=pIn.Match;
    Radix=pIn.Radix;
    return *this;
}

void CWeightList::add(CWeight& pIn)
{
    int wi=0;
    while (( wi < count() ) && ( pIn.Match < Tab(wi).Match) )
        wi++;
    if ((wi < count())&&(pIn.Match == Tab(wi).Match)) {
        while (( wi < count() ) && ( pIn.Match == Tab(wi).Match) && ( pIn.Radix < Tab(wi).Radix) )
            wi++;
    }

    insert(pIn,wi);
}

} // namespace zbs
