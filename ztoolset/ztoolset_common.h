#ifndef ZTOOLSET_COMMON_H
#define ZTOOLSET_COMMON_H

#include <zconfig.h>

#include <cstdlib>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <ztoolset/zlimit.h>

#define __STDSTRING__ "std::string"

#ifdef __USE_WINDOWS__
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif



#define _KEY_BUFFER_MAXSIZE 150

#define _DBGPRINT(...) fprintf (stdout, __VA_ARGS__)


struct ZData_Type_struct
{
        size_t  Offset;
        size_t  Size;
        int     Type;
        bool    isArray ;
        bool    isPointer;
        bool    isCString;
        bool    isAtomic;
        bool    isCompound;
        bool    isStdString;
        char    TypeName [200];

        friend bool operator == (ZData_Type_struct &D1,ZData_Type_struct &D2)
            {
            return (strcmp(D1.TypeName,D2.TypeName)==0);
            };
 };



/**
 * @brief The ZSort_Type enum Gives the type of access to index : with duplicate key values or without duplicates
 */
enum ZSort_Type: uint8_t
{
    ZST_Nothing      = 0,
    ZST_NODUPLICATES = 1,  //!< No duplicate allowed
    ZST_DUPLICATES   = 2   //!< Duplicates are allowed
};



typedef uint8_t         zlock_type ;

enum ZLockMask_type : zlock_type
{
    ZLock_Nothing   = 0,            //!< NOP
    ZLock_Nolock    = 0,            //!< no lock has been requested for this resource
    ZLock_Read      = 0x02,         //!< cannot read the resource
    ZLock_Modify    = 0x04,         //!< cannot write/modify the resource
    ZLock_Write     = ZLock_Modify, //!< idem for easy naming use
    ZLock_Delete    = 0x08,         //!< cannot delete resource. Can read everything and modify -except indeed the resource identification-
    ZLock_All       = ZLock_Read|ZLock_Write|ZLock_Delete,          //!< cannot do anything
    ZLock_Exclusive = 0x10,         //!< for compatibility with ZRF_Exclusive
    ZLock_ReadWrite = ZLock_Read | ZLock_Modify,  //!< self explainatory
    ZLock_WriteDelete = ZLock_Write | ZLock_Delete,  //!< cannot write or delete record or file : only read is allowed
    ZLock_Omitted   = 0xFF         //!< Lock is deliberately omitted as an argument

};

enum AccessRights_type {
    ACR_NOACCESS        = 0x00,
    ACR_CANREAD         = 0x01,
    ACR_CANMODIFY       = 0x02
    ,
    ACR_CANREADMODIFY   = 0x03,

    ACR_CANDELETE       = 0x04,
    ACR_CANREADMODDEL   = 0x07,

    ACR_CANSEND         = 0x08,
    ACR_ALL             = 0x0F
};


enum ZSA_Action { ZSA_NoAction      =   0,
                  ZSA_Error         =   -1,
                  ZSA_Push          =   1,
                  ZSA_Push_Front    =   2,
                  ZSA_Insert        =   4,
                  ZSA_Remove        =   0x10};

/**
 * @brief The ZType enum
 *
 *  the char case :
 *
 *      char*  -> Zchar + Zpointer
 *
 *      char   -> Zchar + Znumeric
 *
 *      char [] -> Zchar + Zarray
 *
 */

enum ZType {
            Zno_type        =   0 ,
            Zatomic         =   1 ,
            Zcompound       =   2 ,
            Zpointer        =   4 ,
            Zarray          =   8 ,

            Zstruct         = 0x20 ,
            Zfunction     = 0x0100,

            Zchar         = 0x01000 ,  // cstring pointer = 0x20004
                                         // cstring array = 0x20008

            Znumeric      = 0x100000,

            Zint          = 0x102000,
//            Zenum         = 0x1C000,     // enum is int
            Zlong         = 0x104000,
            Zfloat        = 0x110000,
            Zdouble       = 0x120000,

            ZStdString    = 0x01000000 ,  // std::string is a compound

            ZnoC11compiler = -1,
            ZerroredType   = -2,
            Zunknown      = 0xF0000         // may be a struct or a class
};



enum ZOp:  uint32_t {
    ZO_Nothing              =0,
    ZO_Add                  =0x01,
    ZO_Erase                =0x02,
    ZO_Replace              =0x04,
    ZO_Insert               =0x08,
    ZO_Swap                 =0x10,
    ZO_Push                 =0x11,
    ZO_Push_front           =0x15,
    ZO_Pop                  =0x22,
    ZO_Pop_front            =0x26,
    ZO_Reset                =0xFF,


//!----------ZRF add-ons------------------------
//!
    ZO_Free                 =0x004100,
    ZO_Writeblock           =0x004200,
    ZO_Remove               =0x004400,

//!---------End ZRF add-ons----------------------

    ZO_RollBack             =0x00001000,
    ZO_RollBack_Insert      =ZO_RollBack|ZO_Insert,
    ZO_RollBack_Push        =ZO_RollBack|ZO_Push,
    ZO_RollBack_Replace     =ZO_RollBack|ZO_Replace,
    ZO_RollBack_Pop         =ZO_RollBack|ZO_Pop,
    ZO_RollBack_Erase       =ZO_RollBack|ZO_Erase,

    ZO_Master               =0x00001000,
    ZO_Slave                =0x00002000,

    ZO_join                 =0x00010000,
    ZO_joinWithDefault      =0x00020000,
    ZO_joinNotFound         =0x00040000,
    ZO_Unique               =0x01000000,
    ZO_joinUnique           =0x01010000,
    ZO_joinUniqueWithDefault=0x01020000,
    ZO_joinUniqueNotFound   =0x01040000,


    ZO_Historized           =0x10000000
};


//! @brief ZCommitStatus is returned by user's function if any defined (not nullptr). It indicates the behavior to be taken just after the call.
enum ZCommitStatus {
      ZCS_Nothing,
      ZCS_Success,          //! commit is OK
      ZSC_Skip,             //! journal element must be skipped without interrupting the commit process, next journal element will be processed
      ZCS_Error,            //! commit is not to be continued but control is given back to caller with error.  ZS_USERERROR status is then returned.
      ZCS_Fatal             //! commit is errored. Processing is to be interrupted immediately with signal (abort())
};


enum ZAMState_type      //!< Journaling state for either ZAMain or ZIndex (see concerned data structures)
{
    ZAMNothing      = 0,        //! normal row state
    ZAMInserted     = 1,        //! row has been inserted since last commit
    ZAMMarkedDelete = 2,        //! row is deleted (journaling log) since last commit
    ZAMReplaced     = 4,        //! row has been modified since last commit
    ZAMErrored      = 0x0100,   //! error to be ORed on operation status




    ZAMErrInsert    = 0x0101,
    ZAMErrDelete    = 0x0102,
    ZAMErrReplace   = 0x0104,

    ZAMRolledBack   = 0x010000, //! the row has been rolled back

    ZAMRBckErased   = ZAMRolledBack|ZAMMarkedDelete,
    ZAMRBckInserted = ZAMRolledBack|ZAMInserted,
    ZAMRBckReplaced = ZAMRolledBack|ZAMReplaced,


    ZAMbeginmark    = 0x1FFF,       // AFTER
    ZAMendmark      = 0xFFFF          // AFTER
};


template <class _Type>
struct CUF_struct
{
typedef  ZCommitStatus (*CUF) (const _Type&,ZOp,void *) ;  //! commit user function
};

//#include <ztoolset/zfunctions.h>








#endif // ZTOOLSET_COMMON_H

