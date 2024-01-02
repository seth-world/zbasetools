#ifndef ZBS_BASEPARAMETERS_H
#define ZBS_BASEPARAMETERS_H

#include "zstatus.h"

#define __PARSER_WORK_DIRECTORY__  "zparserworkdir"
#define __PARSER_PARAM_DIRECTORY__  "zparserparamdir"
#define __PARSER_ICON_DIRECTORY__ "zparsericondir"

#define __WORK_DIRECTORY__  "zparserworkdir"
#define __PARAM_DIRECTORY__  "zparserparamdir"
#define __ICON_DIRECTORY__ "zparsericondir"

#define __GENERAL_PARAMETERS_FILE__ "generalparameters.xml"

/**
 * @brief The ZVerbose_type enum this setup does not concern error messages but only information message.
 *
 *  using appropriate setVerbose(); flag value, it is possible to select the domain to get information from.
 */

typedef uint32_t ZVerbose_Base ;
enum ZVerbose_type : ZVerbose_Base
{
    ZVB_NoVerbose   = 0x0,          //< false : no verbose at all
    ZVB_Basic       = 0x00000001,   //< message with no particular domain

    ZVB_Mutex       = 0x00000100,   //< Mutexes management
    ZVB_Thread      = 0x00000200,   //< Threads management
    ZVB_Stats       = 0x00001000,   //< Collects stats and performance data

    ZVB_NetStats    = 0x00002000,   //< Collects stats and performance for net operations

    ZVB_Xml         = 0x00004000, // < Xml engine verbose

    ZVB_ZRF         = 0x01000000,   //< ZRandomFile

    ZVB_MemEngine   = 0x00000002, /* file memory space allocation and management operations */
    ZVB_FileEngine  = 0x00000004, /* file access operations from file engine */
    ZVB_SearchEngine= 0x00000008, /* all search operations from search engines */

    ZVB_ZMF         = 0x02000000,   //< ZMasterFile
    ZVB_ZIF         = 0x04000000,   //< ZIndexFile
    ZVB_Net         = 0x08000000,   //< Sockets servers SSL & protocol

    ZVB_QT          = 0x10000000,   //< Qt windows & transactional

    ZVB_ALL         = 0xFFFFFFFF    //< All messages are displayed
};

namespace zbs {

class ZBaseParameters
{
public:
    ZBaseParameters();

    void setVerbose (ZVerbose_Base pVerbose) {Verbose = pVerbose; }
    void addVerbose (ZVerbose_Base pVerbose) {Verbose |= pVerbose; }
    void removeVerbose (ZVerbose_Base pVerbose) {Verbose &= ~pVerbose ; }
    void clearVerbose (ZVerbose_Base pVerbose) {Verbose = ZVB_NoVerbose; }

    bool VerboseBasic () {return Verbose & ZVB_Basic;}
    bool VerboseThread () {return Verbose & ZVB_Thread;}
    bool VerboseStats () {return Verbose & ZVB_Stats;}
    bool VerboseNet () {return Verbose & ZVB_Net;}
    bool VerboseNetStats () {return Verbose & ZVB_NetStats;}
    bool VerboseMutex () {return Verbose & ZVB_Mutex;}

    bool VerboseXml() {return Verbose & ZVB_Xml;}

    bool VerboseZRF () {return Verbose & ZVB_ZRF;}
    bool VerboseZMF () {return Verbose & ZVB_ZMF;}
    bool VerboseZIF () {return Verbose & ZVB_ZIF;}

    bool VerboseMemEngine () {return Verbose & ZVB_MemEngine;}
    bool VerboseFileEngine () {return Verbose & ZVB_FileEngine;}
    bool VerboseSearchEngine () {return Verbose & ZVB_SearchEngine;}

    ZVerbose_Base Verbose=ZVB_NoVerbose;
};

} // namespace zbse

extern zbs::ZBaseParameters* BaseParameters;

#endif // ZBS_BASEPARAMETERS_H
