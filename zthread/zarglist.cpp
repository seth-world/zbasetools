#ifndef ZARGLIST_CPP
#define ZARGLIST_CPP

#include "zarglist.h"

using namespace zbs;

ZArgList::ZArgList (void) {}
ZArgList::~ZArgList(void) {}

ZThread* ZArgList::getZThread() {return static_cast<ZThread*>(last());}

#endif //ZARGLIST_CPP
