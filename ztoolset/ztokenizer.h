#ifndef ZTOKENIZER_H
#define ZTOKENIZER_H
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zarray.h>
#include <ztoolset/zfunctions.h>
#include <ztoolset/zutfstrings.h>
namespace zbs
{

class ZToken : public ZArray<char *>
{
public:
    typedef ZArray<char *> _Base;
    ZToken() {}
    ~ZToken() {while (size()) zfree (popR());}
    void push(const char *pElement) {add(pElement);}
    void add(const char*pToken)
    {
    size_t wS = strlen(pToken);

        while ((wS>0)&&((pToken[wS-1]=='\n')||(pToken[wS-1]=='\r'))) // eliminate last lf or cr
                                                        wS--;
        char* wToken =(char*)malloc (wS+1);

        memmove(wToken,pToken,wS);
        wToken[wS]='\0';

        _Base::push(wToken);
    }

};
const int cst_maxTokenLen=256;
class ZTokenizer: public ZDataBuffer
{
public:
    typedef ZDataBuffer _Base;
    ZTokenizer();
    ZToken Tokens;

    using ZDataBuffer::operator =;

    void tokenize( const char *pSeparatorSet=" \n\r")
    {
      char* wPtr=(char*)Data;

        _Base::Trim();

        Tokens.clear();

        wPtr=strtok(wPtr,pSeparatorSet);

        while (wPtr!=nullptr)
        {
            Tokens.add(wPtr);
            wPtr=strtok(nullptr,pSeparatorSet);

        }

    return;
    }

};




}// zbs
#endif // ZTOKENIZER_H
