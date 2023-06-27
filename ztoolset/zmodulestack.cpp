#ifndef ZMODULESTACK_CPP
#define ZMODULESTACK_CPP

#include <ztoolset/zmodulestack.h>



namespace zbs {

ZModuleStack* GStack=nullptr ;
ZModuleStack* CurStack=GStack ;

void clearModuleStack()
{
  if (GStack!=nullptr)
    delete GStack;
}


}// namespace zbs


using namespace zbs;


ZModuleStack::ZModuleStack():zbs::ZArray<const char *>()
{
  atexit(&clearModuleStack);
}

char wMsg[__MESSAGE_SIZEMAX__] ;

void zprintStack(void)
{
    char wFil[50];
    fprintf (stderr,"=====Dump of application call stack====\n");
    for (int wi = 0; wi < CurStack->size();wi ++)
                {
                memset (wFil,' ',(wi+1));
                wFil[wi+1]='\0';
                fprintf(stderr," %2d |%s>%s \n",wi,wFil,CurStack->Tab(wi));
                }
    return;
}
#define __MAX_STACK_DEPTH_REPORT__ 50
#include <ztoolset/zmem.h>
void
ZModuleStack::printStack(FILE*pOutput)
{
    size_t wStackMax=CurStack->size();
    char *wFil=nullptr;
    fprintf (pOutput,"=====Dump of application call stack (recent to oldest)====\n");
    if (CurStack->size()>__MAX_STACK_DEPTH_REPORT__)
        {
        wStackMax = __MAX_STACK_DEPTH_REPORT__;
        }
    wFil= (char*)malloc((wStackMax+1)*sizeof(char));
    int wJ=CurStack->size()>__MAX_STACK_DEPTH_REPORT__?__MAX_STACK_DEPTH_REPORT__:CurStack->size();
    if (CurStack->size() > 0)
        {
        for (int wi = CurStack->size()-1; (wi >= 0) && wJ ;wi --)
                {
                memset (wFil,' ',(wJ+1));
                wFil[wJ+1]='\0';
                fprintf(pOutput," %2d |%s>%s \n",wi,wFil,Tab(wi));
                wJ--;
                if ((wi>0)&&(wJ==0))
                    {
                    fprintf (pOutput,"***** %d more follows ****\n",wi>0?wi-1:wi);
                    break;
                    }
                }
        }//if (CurStack->size() > 0)
    zfree(wFil);
    return;
}//printStack

//#ifdef __USE_ZRANDOMFILE__

//#include <zrandomfile/zrandomfile.h>


//#endif// ZRANDOMFILE_CPP

//#endif // __USE_ZRANDOMFILE__

#endif // ZMODULESTACK_CPP
