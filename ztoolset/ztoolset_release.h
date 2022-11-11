#ifndef ZTOOLSET_RELEASE_H
#define ZTOOLSET_RELEASE_H


/*  release 3-1-0 : correction pack  date 25-11-2022

Changes in main objects export - string enhancements - xml primitives -
    change ZDate & ZDateFull
        bug corrections

            added getURFSize

                utfvtemplatestring :
    added
    - move
    - shiftRight
    - insert

        Checksum and md5
            fix _exportURF returned size

                ZDate & ZDateFull

    - suppress struct tm as base class
    - add date and time controls
    - change string input

        uriString
            fix operators and default contructor with utf8_t string

                Bitset : fix various bugs and display


  all data structures :
    _exportURF appends to ZDataBuffer
    _exportURF returns ssize_t
*/

#endif // ZTOOLSET_RELEASE_H
