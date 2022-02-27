#ifndef CESCAPEDSTRING_H
#define CESCAPEDSTRING_H
#include <ztoolset/zerror.h>
#include <string.h>


/**
 * @brief The CEscapedString class takes a C string in input and escapes the characters that needs to be escaped.
 *
 *  example : <'content'> will be replaced by <\'content\'>
 *
 *  <D:\Mydir\myprogram>  by <D:\\MyDir\\myprogram>
 *
 */
class CEscapedString
{
public:
    CEscapedString(const char *pString ) ;
    CEscapedString() {}
    ~CEscapedString() {if (Content!=nullptr) free(Content);}

    char *Content =nullptr;

    void clear()
    {
      zfree(Content);
      Length=0;
    }
    char * escapeReserved (const char*pString);
    char * escapeReservedChar(const char *pString , char *&pReturnString);

    char * toChar(void) {return Content;}

    void allocate(size_t Delta)
    {
      Length += Delta;
      Content =zrealloc(Content,Length);
    }
private:
    long Length=0;

};


#endif // CESCAPEDSTRING_H
