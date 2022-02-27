#ifndef ZATOMICCONVERT_H
#define ZATOMICCONVERT_H
#include <stdio.h>  // for size_t
#include <stdint.h>
#include <byteswap.h>

bool is_little_endian();    // in zatomicconvert.cpp
bool is_big_endian();       // in zatomicconvert.cpp

unsigned char*  _negate(unsigned char* pValue, ssize_t pSize);

/**
 * @brief _negate Complements any Atomic data value (NOT operation, byte per byte)
 * @param pValue
 */
template <class _Tp>
_Tp _negate(_Tp pValue)
{
    uint8_t *wValuePtr=(uint8_t*)&pValue;
    uint8_t wValueUint8;

    for (size_t wi=0;wi<sizeof(pValue);wi++)
            {
                wValueUint8 = wValuePtr[wi];
                wValuePtr[wi] = ~ wValueUint8;
            }
    return (pValue);
}// _negate


template <class _Tp>
_Tp _negate_T(_Tp &pValue)
{
    uint8_t *wValuePtr=(uint8_t*)&pValue;  // Warning endianness of data

    for (size_t wi=0;wi<sizeof(pValue);wi++)
            {
                wValuePtr[wi] = ~ wValuePtr[wi];
            }
    return (pValue);
}// _negate_T
/**
 * @brief reverseByteOrder_Conditional reverse the data byte order of an atomic value of type _Tp and
 * returns a _Tp data with the reversed value in it.
 * Reverse operation is made if and only if system is little endian.
 * if system is not little endian, pValue is returned as is.
 * @param[in] pValue data value to reverse
 * @return reversed value
 */
template<class _Tp>
static inline
_Tp reverseByteOrder_Conditional(_Tp pValue)
{
    if (!is_little_endian())
                    return pValue;
    switch (sizeof(_Tp))
    {
    case 1:
            return pValue;
    case 2:
            return bswap_16(pValue);
    case 4:
            return bswap_32(pValue);
    case 8:
            return bswap_64(pValue);
    }
    return pValue;
} // _reverseByteOrder

template<class _Tp>
/**
 * @brief forceReverseByteOrder_T Does not test current system endianness. Force reverse byte ordering of given value.
 * Operation is made if system is little endian or not.
 * @param pValue value to reverse byte order
 * @return  reversed byte ordered value
 */
static inline
_Tp forceReverseByteOrder(_Tp pValue)
{
    switch (sizeof(pValue))
    {
    case 1:
            return pValue;
    case 2:
            return bswap_16(pValue);
    case 4:
            return bswap_32(pValue);
    case 8:
            return bswap_64(pValue);
    }
    return pValue;
} // forceReverseByteOrder

#include <string.h>  // for memmove

template<class _Tp>
static inline
_Tp reverseByteOrder_Ptr(const unsigned char* pValue)
{
_Tp wValue;
    memmove(&wValue,pValue,sizeof(wValue));
    return reverseByteOrder_Conditional<_Tp>(wValue);
} // reverseByteOrder_Ptr



#endif // ZATOMICCONVERT_H
