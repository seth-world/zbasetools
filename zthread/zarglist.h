#ifndef ZARGLIST_H
#define ZARGLIST_H

#include <ztoolset/zarray.h>

namespace zbs {
class ZThread;
/**
 * @brief The ZArgList class standard argument list for thread functions call and exitHandler
 */
class ZArgList : public ZArray<void*>
{
public:
  ZArgList (void);
  ~ZArgList(void);

  ZThread* getZThread() ;
};

}// zbs
/**
 * @brief The threadRoutine_struct struct
 */
struct threadRoutine {
    threadRoutine() {clear();}
    threadRoutine(ZStatus (*pFunction)(zbs::ZArgList *),zbs::ZArgList* pArgList)

    {
        Function=pFunction;
        ArgList = pArgList;
    }

    threadRoutine(ZStatus (*pFunction) (zbs::ZArgList*),...) // last argument must be nullptr
    {clear();
        Function=pFunction;
        void *wArgPtr=nullptr;

        ArgList = new zbs::ZArgList;
         va_list wArguments;
         va_start(wArguments,pFunction);
         wArgPtr=va_arg(wArguments,void*);
         while(wArgPtr!=nullptr)
         {
            ArgList->push(wArgPtr);
            wArgPtr=va_arg(wArguments,void*);
         }
         va_end(wArguments);
    }
    ~threadRoutine() {if (ArgList) delete ArgList;}

    ZStatus (*Function) (zbs::ZArgList*);
//    zbs::ZArray<void*>*   Argument;
    zbs::ZArgList*   ArgList=nullptr;

    void clear() {Function=nullptr;
                  if (ArgList)
                      delete ArgList;
                  ArgList=nullptr;}

};



#endif // ZARGLIST_H

