#ifndef ZSTATUS_H
#define ZSTATUS_H

#include <stdint.h>

// @addtogroup ZBSErrorGroup General status returned : this is the list of error / warnings that may be returned
// @{
/**
 * @brief The ZStatus enum General status returned : this is the list of error / warnings that may be returned
 */
typedef int64_t ZStatusBase;
enum ZStatus: ZStatusBase
{
                ZS_NOTHING          =       0,          //< NOP
                ZS_ERROR            =   -0x01 ,     //< Generic error
                ZS_WARNING          =   +0x02 ,     // Generic warning (not negative)
                ZS_INVALIDSTATUS    = 0xFFFFFFFF,   //this status is invalid
                ZS_TOBEDONE         = 0x0FFFFFFF,   // this has not yet been implemented

                ZS_REJECTED         =   -0x10,


                ZS_NOTFOUND         =   -0x04,      //< Requested element has not been found

                ZS_OUTBOUND         =   -0x20,    //< Requested key value is not in the range of any existing key values
                ZS_OUTBOUNDLOW      =   -0x21,    //< Requested key value is lower than any value in the range of existing key values
                ZS_OUTBOUNDHIGH     =   -0x22,    //< Requested key value is higher to any value in the range of existing key values


                ZS_DUPVIOLATION     =   -0x40,          //< Range of values induces a duplicate key while rebuilding index. This is a severe error that may be caused by an inappropriate access to ZAM content.
                ZS_DUPLICATEKEY     =   -0x41,          //< Duplicate key has been found and prevents to complete operation (No duplicates allowed)


                ZS_KEYFIELDPARTIAL  =   0x0A,   //< Due to variable length of records a key field composing a key has been partially extracted reaching the end of the variable record.This is Not an error.
                ZS_KEYFIELDOUTBOUND =   0x0B,   //< Due to variable length of records a key field definition points out of record boundaries and will be missing because not extracted. This is NOT an error.
                ZS_FIELDMISSING     =   0x0C,   //< Field is missing for current record. This is NOT an error.
                ZS_FIELDLOOSEPREC   =  -0x0D,   //< loosing precision while converting field. This is an error.
                ZS_FIELDCAPAOVFLW   =  -0x0E,   //< Field capacity overflow. This is an error.


                ZS_INVOP            =   -0xF0,      //< Invalid operation
                ZS_INVTYPE          =   -0xF1,      //< Data type not managed
                ZS_INVNAME          =   -0xF2,      //< Name unknown : CZDictionary field name does not exist
                ZS_INVSIZE          =   -0xF3,      //< Invalid size. Example invalid size of sequence to search
                ZS_INVADDRESS       =   -0xF4,      //< Invalid address given. With ZRandomFile, this means that the address has not been found as a valid start of Block within ZBAT (Access Table pool)
                                                    // Or during a remove on index file given ZMasterFile address does not match the one stored in Index file rank obtained.
                                                    // Or invalid rank for a zgetSingleField or zwriteSingleField
                                                    // Or field definition violates an existing dictionary key field during a changeFieldValue operation
                ZS_INVVALUE         =   -0xF5,      //< Invalid value provided
                ZS_EXCEPTION        =   -0xF6,      //< An exception has been set

                ZS_INVPARAMS        =   -0xF7,      // invalid parameters

                ZS_OMITTED          =   -0xF8,      // parameter present but omitted value : example bitset with ZType_bitsetFull
//
//                                          +---------> Collection of records
//                                          |
//                                          x--
                ZS_INVINDEX         =   -0x0101,    //< Invalid ZIndexFile object while trying to access a ZMasterFile index

                ZS_FOUND            =    0x0103 ,       //< Key value has successfully been found



              /* ZSearchParser  */

                ZS_MISS_KEYWORD     =     -0x008001,
                ZS_MISS_FIELD       =     -0x008002,
                ZS_MISS_PUNCTSIGN   =     -0x008003,  /* missing punctuation sign as semi colon or dot */
                ZS_MISS_LITERAL     =     -0x008004,  /* expected literal (value or path) */
                ZS_MISS_OPERATOR    =     -0x008005,  /* expected operator  */

                ZS_INV_ENTITY       =     -0x008010,  /* invalid fentity name */
                ZS_INV_FIELD        =     -0x008012,  /* invalid field */
                ZS_INV_MODIFIER     =     -0x008013,  /* invalid modifier */
                ZS_INV_LITERAL      =     -0x008014,  /* literal is malformed ex. invalid date format */
                ZS_INV_OPERATOR     =     -0x008015,  /* Operator is not allowed here */
                ZS_SYNTAX_ERROR     =     -0x008016,  /* syntax error : ex. closed parenthesis without corresponding open parenthesis */


                ZS_S_ERASEKEY         =   -0x100100,    //< Error while erasing Key
                ZS_S_ERASEREVERSE     =   -0x100200,    //< Error while erasing reverse index

                ZS_S_ERASEZAM         =   -0x101000,    //< error returned by ZIndex while trying to erase ZAM Row during a remove or removeALL operation

                ZS_USERERROR          =   -0xF00000,    //< user routine sent back an error during a commit operation
                ZS_COMROLLERROR       =   -0x800000,    //< commit / rollback error
//
// -------------------------------------------------------File errors
//
//                                             xx--------> bytes available for file status
                ZS_EOF                =    0x001100,        //< End of File found
                ZS_READPARTIAL        =    0x001200,        //< Not an error : read has been done but returned data size is not equal to expected size
                ZS_WRITEPARTIAL       =   -0x001220,        //< Write error : block has been only partially written on file

                ZS_READERROR          =   -0x001201,        //< there has been an error while reading file
                ZS_WRITEERROR         =   -0x001401,        //< there has been an error while writing file

                ZS_TIMEOUT            =   -0x001002,        // timeout mask

                ZS_READTIMEOUT        =   -0x001202,        //< Timeout on read operation
                ZS_WRITETIMEOUT       =   -0x001402,        //< Timeout on write operation



  /*   *
    zmail_nothing                           =     0,
    zmail_success                           =     1,
    zmail_timeout                           =     2,        // could be ANDED to get generic timeout error
    zmail_connect_timeout                   =     8,

    zmail_read_timeout                      =-0x1202,
    zmail_write_timeout                     =-0x1402,

    zmail_smtp_transmit_error               =-0x1801,
    zmail_badparams                         =-0x1802,
    zmail_invalidparams                     =-0x1804,
    zmail_encrypt_conn_timeout              =-0x1808,

    zmail_socketisclosed                    =-0x1810
   *
   */


                ZS_ERROPEN            =   -0x001801,        //< File cannot be openned
                ZS_FILENOTEXIST       =   -0x003801,        //< file must exist but no file with this name has been found
                ZS_NOTDIRECTORY       =   -0x003802,        //< given path is NOT a Directory while directory was requested
                ZS_FILENOTOPEN        =   -0x003803,        //< file is not open as it must be
                ZS_INVOPENMODE        =   -0x003804,        //< file is not open with appropriate mod
                ZS_FILEEXIST          =   -0x009001,        //< file must not exist but a file with this name has been found
                ZS_FILEREPLACED       =    0x009001,        //< NOT AN ERROR : file has been replaced
                ZS_EMPTYFILE          =    0x005000,        //< not an error : just a fact. File is empty
                ZS_FILEERROR          =   -0x001000,        //< general file error
                ZS_FILEPOSERR         =   -0x001010,        //< file positionning error : a fsetpos or fgetpos or fseek operation ended with error

                ZS_INVBLOCKADDR       =   -0x001011,        //< Invalid block address : given address does not correspond to a beginning of a block
                ZS_INVBLOCK           =   -0x001012,        //< Invalid block

                ZS_CORRUPTED          =   -0x001013,        //< Data is corrupted

                ZS_ENDIAN             =   -0x001014,        //< data is not reversed/serialized as expected

                ZS_CANTALLOCSPACE     =   -0x001020,        //< Cannot allocate more free space in Free pool

                ZS_WOULDBLOCK         =   -0x001030,        //< EWOULDBLOCK encountered

                ZS_BADFILEHEADER      =   -0x002000,        //< file header (FCB + ZBAT + ZFBT) is malformed or corrupted
                ZS_BADFILEDESC        =   -0x002001,        //< file descriptor (ZFileDescriptor) is invalid malformed or corrupted
                ZS_BADFILEVERSION     =   -0x002002,        //< file header version is not the right one
                ZS_BADFILERESERVED    =   -0x002003,        //< file header reserved infradata is wrong
                ZS_BADMCB             =   -0x002004,        //< file header Master Control Block infradata is wrong
                ZS_BADICB             =   -0x002005,        //< file header Index Control Block infradata is wrong
                ZS_BADFILETYPE        =   -0x002006,        //< given file type is not open see @ref ZFile_type and @ref ZRandomFile::_open
                ZS_FILETYPEWARN       =   +0x002007,        //< NOT AN ERROR : Warning :given file type is not the native file type see @ref ZFile_type and @ref ZRandomFile::_open
                ZS_BADDIC             =   -0x002008,        //< bad dictionary : dictionary is either malformed or inexistant

                ZS_CRYPTERROR         =   -0x004000,        //< invalid crypted file content

                ZS_BADCHECKSUM        =   -0x008100,        //< File content checksum does not match what expected
                ZS_BADFILEATTR        =   -0x008200,        //< File attributes do not match what expected

                ZS_MALFORMED        =   -0x00001111,         //< Name is malformed. Exemple : give file pathname has an invalid extension name ( equal to __HEADER_FILEEXTENSION__)

                ZS_ACCESSRIGHTS       = -0x01000000,        //< cannot set/modify access rights for the file : accessright problem see message

//                                           2X--------------> available for lock management status with prefix mask = 0x20
                ZS_LOCKED             = -0x00200000,        //< resource is locked and cannot be accessed
                ZS_LOCKDELETE         = -0x00200001,        //< resource is locked for deleting
                ZS_LOCKWRITE          = -0x00200003,        //< resource is locked for writing and deletion : read only allowed
                ZS_LOCKREAD           = -0x00200007,        //< resource is locked for read write and delete

                ZS_LOCKALL            = -0x00200007,        //< resource is locked for reading writing deleting : alias of ZS_LOCKREAD

                ZS_LOCKERROR          = -0x00200008,  //< an error occurred in base mutex routines- see errno

                ZS_LOCKCREATE         = -0x00210000,        //< resource is locked being created
                ZS_LOCKENTITY         = -0x00220000,        //< the whole entity is locked

                ZS_LOCKBADOWNER       = -0x00280000,        //< Owner requesting lock modification is not the owner of the lock
                ZS_LOCKBADID          = -0x00280001,        //< Bad lockid given : no corresponding active lock in Lockmanager database
                ZS_LOCKPENDING        = -0x00300000,        //< Lock is pending and has not yet been activated. It will be activated as soon as resource will be freed
                ZS_LOCKINTERR         = -0x00310000,        //< Lock internal error
                ZS_LOCKALREADY        = -0x00320000,        //< User has already locked the resource and tries to lock again the same resource

                ZS_MODEINVALID        = -0x00400000,        //< File open mode does not allow current access attempt. Example file open mode = ZRF_ReadOnly and attempt to _insertFirstPhasePrepare()

//----------------------------Network----------------------------------------------------------------------------------

                ZS_SOCKSETUP          = -0x00800001,        //< Cannot setup socket
                ZS_SOCKBIND           = -0x00800002,        //< Cannot bind socket
                ZS_SOCKLISTEN         = -0x00800004,        //< Error while server was listening on socket
                ZS_SOCKACCEPT         = -0x00800008,        //< Error while server accepts new connection
                ZS_SOCKSELECT         = -0x00800009,        //< Error while calling select function on socket
                ZS_AUTHREJECTED       = -0x00800010,        //< Authentification rejected by Authenticate function
                ZS_SOCKADDRESS        = -0x00800011,        //< Invalid INet address to convert
                ZS_SOCKCONNECT        = -0x00800012,        //< Client socket cannot connect to
                ZS_SOCKERROR          = -0x00800020,        //< socket generic error : refer to errno content (ZException::Error,ZException::Complement)
                ZS_CONNECTTIMEOUT     = -0x00800013,        //zmail_encrypt_conn_timeout
                ZS_SOCKISCLOSED       = -0x00800021,        // zmail_socketisclosed

                ZS_LDAPFAIL           = -0x00800100,        //< Failed with LDAP (system or network error)


                ZS_SMTP_ERROR         = -0x00802000,    // generic smtp error - error message follows

                ZS_SMTP_IVMAIL        = -0x00802001,    // invalid mail address or mailbox unavailable


                ZS_IMAP_ERROR         = -0x00801800,   // generic imap error

                ZS_IMAP_TRANSMIT      = -0x00801801,   // zmail_smtp_transmit_error
                ZS_IMAP_BADPARAM      = -0x00801802,  //zmail_badparams
                ZS_IMAP_INVPARAM      = -0x00801804, // zmail_invalidparams

                ZS_IMAPNEXT           =  0x00801801,   // not an error : imap server will send a next message
                ZS_IMAPWAITNEXT       =  0x00801802,   // not an error : imap server is waiting for a complement

                ZS_IMAPREJECTED       = -0x00801808,   // imap server rejected client command



                ZS_SOCKBROKENPIPE     = -0x00810001,        //< Received broken pipe signal : communication has been cancelled by pair while trying to write bytes.


                ZS_SSLINIT            = -0x00820001,        //< OpenSSL cannot initialize
                ZS_SSLCERT            = -0x00820002,        //< OpenSSL problem with certificates or key while initializing SSL context

                ZS_SSLINVKEY          = -0x00820010,        //< OpenSSL error on key

                ZS_SSLACCEPT          = -0x00820020,        //< OpenSSL error cannot initiate SSL handshake (SSL_accept)

                ZS_SSLCERTREJECTED    = -0x00820030,        //< OpenSSL error Certificated is rejected

                ZS_SSLREADERR         = -0x00820040,        //< OpenSSL read error
                ZS_SSLWRITEERR        = -0x00820050,        //< OpenSSL write error

                ZS_BIOERR             = -0x00821000,        //< BIO error

//                                           1X--------------> available for users management status with prefix mask = 0x10
                ZS_BADUSER            = -0x00100000,        //< user is not either not recognized or not authorized
                ZS_BADUSERNAME        = -0x00110000,        //< user name is not recognized
                ZS_BADUSERID          = -0x00120000,        //< user id is not recognized
                ZS_BADPASSWORD        = -0x00140000,        //< given password does not match
                ZS_USERPRIVILEGES     = -0x00180000,        // user do not have privileges for doing such operation

/*                ZS_LOCKALL            = -0x02700000,        //< resource is locked for reading writing deleting
                ZS_LOCKWRITEDELETE    = -0x02300000,        //< resource is locked for reading writing deleting
                ZS_LOCKBADOWNER       = -0x02800000,        //< Owner requesting lock modification is not the owner of the lock
                ZS_LOCKBADID          = -0x02800001,        //< Bad lockid given : no corresponding active lock in Lockmanager database*/
//-----------------------Journaling-------------------------------

                ZS_EMPTY              = -0x08000001,        //< queue is empty
                ZS_QUEUEERROR         = -0x08000010,        //< Error on queue while queueing / dequeuing element
                ZS_SYSTEMERROR        = -0x08000100,        //< base system routine error (example getpwd, etc..)
                ZS_NULLPTR            = -0x08000002,        //< encountered a null pointer (NULL or nullptr) while expecting argument

//-----------------------Char encoding-----------------------------
                ZS_ICONVINIT          = -0x00901000,        //!< iconv init failure
                ZS_ICONVSTOPPED       = -0x00902000,        //!< iconv processing failure
                ZS_NEEDMORESPACE      = -0x00904000,        //!< output buffer exhausted : need more space in output
                ZS_INVCHARSET         = -0x00905000,        //!< invalid charset code - not recognized or not adapted to target string

                ZS_IVBOM             = -0x00906000,        //!< invalid BOM has been found : found BOM does not correspond to expected encoding
                ZS_CONVERR            = -0x00907000,        //!< Character conversion error
                ZS_ILLEGAL            = -0x00908000,        //!< Illegal character encoding format detected
                ZS_IVCODEPOINT        = -0x00909000,        //!< Invalid codepoint : when converting from Unicode to destination encoding an invalid unicode value has been detected

                ZS_TRUNCATED          =  0x0090A000,        //!< warning :Input has been truncated to fit into destination fixed capacity - not an error

                ZS_NOENDMARK       = -0x0090B000,           //!< endofString mark ('\0') has not be found on string : search of this mark has been made until __STRING_MAX_LENGTH__ without success.

                ZS_ICUERROR           = -0x00910000,        //!< ICU error. ICU Status (UErrorCode) is given apart within complement
                ZS_ICUWARNING         =  0x00920000,        //!< ICU warning (positive value) : not an error-(UErrorCode) is given apart within complement


//-------------------------Mime-----------------------------------
                ZS_MIMEINV            = -0x10000000,        //< invalid MimeData
                ZS_MEMERROR           = -0x20000000,        //< cannot allocate memory or memory error

                ZS_MEMOVFLW           = -0x20000001,        // buffer or memory overflow : memory allocated size must be increased


                ZS_XMLERROR           = -0x40000000,       //< xml formatting or parsing error
                ZS_XMLCORRUPTED       = -0x40000001,       //< xml document appears to be corrupted
                ZS_XMLEMPTY           = -0x40000002,       //< xml no root element found : empty xml document
                ZS_XMLMISSREQ         = -0x40000003,       //< xml document misses a required field
                ZS_XMLINVROOTNAME     = -0x40000010,       //< xml invalid root element name
                ZS_XMLINVNODENAME     = -0x40000011,       //< xml invalid node name

                ZS_XMLWARNING         = +0x40000000,        //< xml formatting or parsing warning

                ZS_ENTITYERROR        = -0x80000000,        //< invalid entity


                ZS_SUCCESS          =    1,                 //< Everything went OK
                ZS_ENDCLIENT        =    3,                 //< Client thread ended OK
                ZS_CANCEL           =    5,                 //< Client has been requested to cancel @see ZThreadState_type ZTHS_CancelRequested

                ZS_HELP             =    8,                 //< Help requested


                ZS_CONFWARN         =   0x10,
                ZS_REPLACED         =   0x20,               //< Application : successful replacement
                ZS_CREATED          =   0x21                //< Application : successful creation
             }  ;
// @}

const char * decode_ZStatus (ZStatus ZS);  // located in zfunction.ccp
#endif // ZSTATUS_H
