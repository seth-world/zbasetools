#ifndef CESCAPEDSTRING_CPP
#define CESCAPEDSTRING_CPP
#include <ztoolset/cescapedstring.h>


CEscapedString::CEscapedString (const char * pString)
{

    Content = escapeReserved(pString);

//    Content = escapeReservedChar(pString, Content);
}

/**
 * @brief CEscapedString::escapeReservedChar escapes pString in input according the following rules
 *          char to be escaped is prefixed with '\' char.
 *          if char to be escaped is quote or double quote, then it is replaced with three backslash quote or double quote
 * NB: escaping a character consists in preceeding it with a backslash
 *
 * @param pString
 * @param pReturnString
 * @return
 */

char *
CEscapedString::escapeReservedChar (const char *pString , char *&pReturnString)
{
    const char * ptoEscape = "\'\"\\";  // char to escape are quote, doublequote and backslash
    const char *ptr;
    char *PtrReturn;
    ptr = strpbrk(pString,ptoEscape);
    if (ptr == nullptr)
            {
            strcpy(pReturnString,pString);
            return(pReturnString);
            }
        else
            {

           size_t wL = (size_t)(ptr-pString);
            strncpy (pReturnString,pString,wL);
            if ((ptr[0]=='\'')||(ptr[0]=='\"'))  // if quote or double quote need to put three backslash quote or double quote
                    {

#ifdef __USE_WINDOWS__
                    allocate(5);     // allocate 5 char more to contain the escaped 3 quotes and their backslashes (6 - 1)
                    if (ptr[0]=='\'')
                                {
                                strcat (pReturnString,"\\\'\\\'\\\'");
                                }
                            else
                                {
                                strcat (pReturnString,"\\\"\\\"\\\"");
                                }
                    wL += 6;
#else
                allocate(1);     // allocate 1 char more to contain the escaped quote (backslash)
                if (ptr[0]=='\'')
                            {
                            strcat (pReturnString,"\\\'");
                            }
                        else
                            {
                             strcat (pReturnString,"\\\"");
                            }
                    wL += 2;

#endif

                    }// if quote or double quote
                    else
                    {
                    pReturnString[wL]='\\';
                    wL++;
                    pReturnString[wL]=ptr[0];
                    wL++;
                    }

            pReturnString[wL]='\0';
            ptr ++ ;    // point to the char next the one to escape
            PtrReturn=&pReturnString[wL];
            escapeReservedChar(ptr,PtrReturn);
            return (pReturnString);
            }
}//escapeReservedChar


char *
CEscapedString::escapeReserved (const char*pString)
{
    Length =  strlen(pString) + 1;
    Content=(char *)malloc (Length);
    if (Content==nullptr)
                {
                std::cerr << _GET_FUNCTION_NAME_ << "-F-MEMALLOCERR Error allocating memory . Aborting \n";
                abort();
                }
    memset(Content,0,Length);
    Content=escapeReservedChar(pString, Content);
    return (Content);
}



#endif // CESCAPEDSTRING_CPP
