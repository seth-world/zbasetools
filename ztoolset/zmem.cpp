
#include <stdlib.h>

void _free(void*pPtr)
{
    if (!pPtr)
            return;
    free(pPtr);
    pPtr=nullptr;
}
