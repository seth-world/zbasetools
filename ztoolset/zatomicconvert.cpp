#include <unistd.h>
#include <stdint.h>

#include <ztoolset/zlimit.h>
//--------------- convert data----------------------------------------------

/**
 * @brief _negate Complements any Atomic data value (NOT operation, byte per byte)
 * @param pValue
 */
unsigned char* _negate(unsigned char* pValue, ssize_t pSize)
{
    uint8_t* wValuePtr=(uint8_t*)pValue;
    for (ssize_t wi=0;wi<pSize;wi++)
            {
                wValuePtr[wi] = ~ wValuePtr[wi];
            }
    return (pValue);
}// _negate


/**
* @brief is_little_endian  defines if system is using little Endian atomic data storage (reverse byte order)
* @return
*/
ZBool is_little_endian(void)
{
  union {
       uint32_t i;
       int8_t c[4];
   } e = { 0x00000001 };

   return (e.c[0]==1);
} //is_little_endian

