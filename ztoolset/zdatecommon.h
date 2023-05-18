#ifndef ZDATECOMMON_H
#define ZDATECOMMON_H

#include <time.h>
#include <memory.h>

struct timespecNull : public timespec
{
  timespecNull() {memset (this,0,sizeof(timespec));}
  static bool isNull(const timespec& pIn) {
    return (pIn.tv_sec==0)&&(pIn.tv_nsec==0);
  }
  bool operator = (const timespec& pIn) {}
};

#endif // ZDATECOMMON_H
