#ifndef CHARMAN_H
#define CHARMAN_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

typedef uint8_t utf8_t;

class CharMan{
public:
    CharMan() {memset(content,0,sizeof(content));}
    CharMan(CharMan& pIn)  {_copyFrom(pIn);}
    CharMan(CharMan&& pIn)  {_copyFrom(pIn);}
    CharMan(const char *pIn) { strncpy(content, pIn, sizeof(content)); }


    CharMan(const int pInt) {sprintf(content, "%d", pInt);}
    CharMan(const unsigned int pInt) {sprintf(content, "%u", pInt);}
    CharMan(const long pInt) {sprintf(content, "%ld", pInt);}
    CharMan(const unsigned long pInt) {sprintf(content, "%lu", pInt);}
    CharMan(const float pInt) {sprintf(content, "%g", (double)pInt);}
    CharMan(const double pInt) {sprintf(content, "%g", pInt);}

    CharMan& _copyFrom (CharMan& pIn)
        {
        strncpy(content,pIn.content,sizeof(content));
        return *this;
        }
    CharMan& operator= (CharMan& pIn) {return _copyFrom(pIn);}
    CharMan& operator= (CharMan&& pIn) {return _copyFrom(pIn);}
    CharMan& operator= (const char* pIn)
    {
        strncpy(content,pIn,sizeof(content));
        return *this;
    }
    CharMan& operator += (const char* pIn)
    {
        strncat(content,pIn,sizeof(content));
        return *this;
    }


    char& operator[](size_t pIdx) { if (pIdx>=sizeof(content)) return content[0]; return content[pIdx]; }

    void clear() {memset(content,0,sizeof(content));}

    void sprintf(const char* pFormat,...)
    {
        va_list args;
        va_start(args,pFormat);
        vsnprintf(content,sizeof(content),pFormat,args);
        va_end(args);
    }

    void strcat(const char *pIn) { ::strncat(content, pIn, sizeof(content)); }

    void trunc(const utf8_t*pPtr,int pSize)
    {
        ::strncpy(content,(char*)pPtr,size_t(pSize)>(sizeof(content)-1)?sizeof(content)-1:size_t(pSize));
    }
    void trunc(const char*pPtr,size_t pSize)
    {
      ::strncpy(content,pPtr,pSize>(sizeof(content)-1)?sizeof(content)-1:pSize);
    }
    size_t Maxlen() {return sizeof(content);}

    size_t getUnitCount() {return sizeof(content);}

    char* toCChar() {return content;}
    char content[250];
};

#endif // CHARMAN_H
