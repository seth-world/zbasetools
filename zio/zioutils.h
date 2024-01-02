#ifndef ZIOUTILS_H
#define ZIOUTILS_H

#include <config/zconfig.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zstatus.h>

#include <fcntl.h>
#include <filesystem>

#include <ztoolset/utfvaryingstring.h>

#define __progressCallBack__(__NAME__)  std::function<void (int,const utf8VaryingString&)> __NAME__
#define __progressSetupCallBack__(__NAME__)  std::function<void (int,const utf8VaryingString&)> __NAME__

typedef uint8_t ZMNP_type;
enum ZCopyManip_enum : ZMNP_type
{
    ZMNP_Nothing    = 0,    /* if chosen, target file will be replaced if exists, and copied file will have current owner and default permissions */
    ZMNP_Replace    = 1,    /* replace file if exists : if not selected and if no backup selected abort if exists*/
    ZMNP_Backup     = 2 ,   /* backup target file if exists before copy : exclusive with ZMNP_Replace */
    ZMNP_KeepPerm   = 4 ,   /* keep owner and file permissions : if not selected copied file is set with default permission and current owner */
    ZMNP_IncludeAll = 8     /* include all files : header , indexes and their header : managed by ZRandomFile and ZRawMasterFIle corresponding copy routines */
};


utf8VaryingString decode_ZMNP(ZMNP_type pFlag);

class ZaiErrors;

extern int rawCopyPayLoad ;
/**
 * @brief setRawCopyPayLoad sets the default payload for raw copy operation.
 */
void setRawCopyPayLoad(int pPayload) ;

/**
 * @brief rawCopy   copy a source file to a target file, with each file access operation is of pPayLoad bytes.
 *                  if exists, progress call backs are used.
 *                  if exists, pErrorLog is used for reporting results and/or errors.
 *
 *                  To be done : access remote files as source or as target.
 * @param pSource   a valid uriString pointing to an UNIQUE file
 * @param pTarget   a valid uriString pointing to a well formed full file description.
 *                  Target file name must be mentionned.
 *                  No wild card allowed.
 * @param pFlag     see ZCopyManip_enum. Nota bene : ZMNP_IncludeAll has no effect since only low level files are considered here.
 * @param pPayLoad  amount of byte to be read/written at each access. if set to -1, then default payload applies. see setRawCopyPayLoad()
 * @param pErrorLog
 * @return
 */
ZStatus rawCopy(const uriString &pSource,
                const uriString &pTarget,
                uint8_t pFlag = ZMNP_Nothing,
                long pPayLoad = -1,
                __progressCallBack__ (_progressCallBack) = nullptr,
                __progressSetupCallBack__ (_progressSetupCallBack) = nullptr,
                ZaiErrors *pErrorLog = nullptr);
/**
 * @brief rawRenameBck  renames pFile in the same directory to a name with pBckExt extension according standard rules :
 *      <file base name>_<bckext>99
 *      with    <file base name> is <file root>.<file extension>
 *              <bckext> is pBckExt : generally a suite of 3 alphabetic characters e. g. 'bck'
 *              99 a two digits number from 01 to 99 varying according already existing file names.
 *
 *  ATTENTION : For random file suite, please use built in renameBck() method that insures file names alignment.
 * @param pFile         File full description to rename
 * @param pBckExt
 * @param pErrorLog
 * @return same status a rawRename()
 */
ZStatus rawRenameBck(const uriString &pFile,
                     const utf8VaryingString &pBckExt,
                     bool pNoExcept = false,
                     ZaiErrors *pErrorLog = nullptr);
/**
 * @brief rawRename
 *
 *      see https://www.man7.org/linux/man-pages/man2/rename.2.html
 * @param pFile         File full description to rename
 * @param pNewURI
 * @param pErrorLog
 * @return
 */
ZStatus rawRename(const uriString &pFile,
                  const utf8VaryingString &pNewURI,
                  bool pNoExcept = false,
                  ZaiErrors *pErrorLog = nullptr);

/**
     * @brief rawOpen low level open file described by pPath, using __oflag (see https://www.man7.org/linux/man-pages/man2/open.2.html)
     * @param pStatus returned status set to ZS_SUCCESS if operation is successfull
     * @return an int with file descriptor or -1 if open operation is errored.
     * In this later case, ZException is positionned with errno information and accurate error description.
     *
     * pMode is generally one of these O_RDONLY, O_WRONLY, or O_RDWR.
     * for other modes see https://www.man7.org/linux/man-pages/man2/open.2.html
     *
     *
     * in order to create (or replace) a file pMode must be set to O_CREAT|O_WRONLY|O_TRUNC
     *
     */
  ZStatus rawOpen(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath, __FILEOPENMODE__ pMode, __FILEACCESSRIGHTS__ pAccessRights = S_IRWXU|S_IRWXG|S_IROTH);
//  ZStatus rawOpen(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath, __FILEOPENMODE__ pMode);

  ZStatus _rawOpen(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath, __FILEOPENMODE__ pMode, __FILEACCESSRIGHTS__ pPriv,bool pNoExcept);

  ZStatus _rawOpenGetException(int wErrno, const utf8VaryingString& pPath,bool pNoExcept ) ;


  ZStatus rawOpenRead(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath);
  ZStatus rawOpenModify(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath);
  ZStatus rawOpenCreate(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath);


/**
     * @brief rawRead low level read of a file described by its file descriptor pFd of pBytesToRead bytes at most.
     * @param pFd   a valid file descriptor
     * @param pData a ZDataBuffer.  After read operation, ZDataBuffer::Size is allocated and its size adjusted
     *                              to the effective number of bytes read.
     * @param pBytesToRead requested bytes to read at most.
     * @return a ZStatus set to
     *  ZS_SUCCESS if operation is totally completed,
     *  ZS_PARTIAL if not all bytes have been read.
     *  ZS_EOF if end of file has been reached. In this case, pData contains nothing.
     * If not successfull, status is adjusted according errno error and ZException is set with acurate content.
     */
  ZStatus rawRead(__FILEHANDLE__ pFd, ZDataBuffer& pData, size_t pBytesToRead);

  /**
   * @brief rawReadAt reads a file described by its descriptor pFd at address (physical offset) pAddress
   * a number of bytes pBytesToRead and returns read data into pData.
   * If the requested amount of data pBytesToRead cannot be read,
   * the effective size of read data is given by pData.Size.
   * @param pFd         a valid file descriptor
   * @param pData a ZDataBuffer.  After read operation, ZDataBuffer::Size is allocated and its size adjusted
   *                              to the effective number of bytes read.
   * @param pBytesToRead requested bytes to read at most.
   * @param pAddress      physical offset from beginning of file
   * @return a ZStatus set to :
   *  ZS_SUCCESS if operation is totally completed,
   *  ZS_PARTIAL if not all bytes have been read.
   *  ZS_EOF if end of file has been reached. In this case, pData contains nothing.
   *  other errors : see rawSeekToPosition
   *
   */
  ZStatus rawReadAt(__FILEHANDLE__ pFd, ZDataBuffer& pData,size_t pBytesToRead, size_t pAddress);

  ZStatus rawWrite(__FILEHANDLE__ pFd, ZDataBuffer &pData, size_t &pSizeWritten) ;

  ZStatus rawWriteAt(__FILEHANDLE__ pFd, ZDataBuffer& pData,size_t &pSizeWritten, size_t pAddress);

  ZStatus rawFlush(__FILEHANDLE__ pFd);

  ZStatus rawReadText(__FILEHANDLE__ pFd, utf8VaryingString& pString, size_t pBytesToRead);
  ZStatus rawReadTextAt(__FILEHANDLE__ pFd, utf8VaryingString& pString,size_t pBytesToRead, size_t pAddress);
  ZStatus rawWriteText(__FILEHANDLE__ pFd, const utf8VaryingString& pString, size_t &pSizeWritten);
  ZStatus rawWriteTextAt(__FILEHANDLE__ pFd, const utf8VaryingString&& pData,size_t &pSizeWritten, size_t pAddress);
/**
 * @brief rawClose low level close operation of a file described by its file descriptor pFd
 * @param pFd
 * @return ZS_SUCCESS in case of successfull operation. pFd is set to -1.
 * If not successfull, status is adjusted according errno error and ZException is set with acurate content.
 */
  ZStatus rawClose(__FILEHANDLE__ &pFd);

  /**
   * @brief rawFileAllocate allocates pBytes of additional space starting at pOffset for file described by pFd.
   * File must have been openned prior to calling this routine.
   * @return ZS_SUCCESS in case of successfull operation.
  * If not successfull, status is adjusted according errno error and ZException is set with acurate content.
   */
  ZStatus rawAllocate(__FILEHANDLE__ pFd, size_t pOffset, size_t pBytes );

  /**
   * @brief rawSeek
   * @param pFd   a valid descriptor for an open file
   * @param pOff  offset
   * @param pWhence
   * @return ZS_SUCCESS when operation has completed successfully.
   * ZS_BADFILEDESC <EBADF> File descriptor argument is not a open file descriptor.
   * ZS_INVPARAMS <EINVAL>  whence is not valid.  Or: the resulting file offset would be negative, or beyond the end of a seekable device for given offset.
   * ZS_INVADDRESS <ENXIO> whence is SEEK_DATA or SEEK_HOLE, and offset is beyond the end of the file, or whence is SEEK_DATA and offset is within a hole at the end of the file.
   * ZS_INVVALUE <EOVERFLOW> The resulting file offset cannot be represented in returned data format.
   * ZS_INVOP <ESPIPE> file descriptor is associated with a pipe, socket, or FIFO. Operation not allowed.
   * ZS_FILEPOSERR error is not recognized (errno value).
   */
  ZStatus rawSeek( __FILEHANDLE__ pFd, __off_t &pOff, int pWhence);
  /**
   * @brief rawSeekToPosition set current file position at absolute offset pOff since beginning of file corresponding to descriptor pFd
   * @return ZS_SUCCESS in case of successfull operation.
   * If not successfull, status is adjusted according errno error and ZException is set with acurate content.
   * ZS_BADFILEDESC <EBADF> File descriptor argument is not a open file descriptor.
   * ZS_INVPARAMS <EINVAL>  whence is not valid.  Or: the resulting file offset would be negative, or beyond the end of a seekable device for given offset.
   * ZS_INVADDRESS <ENXIO> whence is SEEK_DATA or SEEK_HOLE, and offset is beyond the end of the file, or whence is SEEK_DATA and offset is within a hole at the end of the file.
   * ZS_INVVALUE <EOVERFLOW> The resulting file offset cannot be represented in returned data format.
   * ZS_INVOP <ESPIPE> file descriptor is associated with a pipe, socket, or FIFO. Operation not allowed.
   * ZS_FILEPOSERR error is not recognized (errno value).
   */
  ZStatus rawSeekToPosition(__FILEHANDLE__ pFd, size_t pAddress);
  /**
   * @brief rawSeekToRelative Sets current file position at current position plus (or minus according offset sign)
   *  relative offset pOff for file corresponding to descriptor pFd.
   * In case of success, pAddress is set to the absolute file offset after positionning.
   * pAddress remains unchanged if operation is not succesfull.
   * @return ZS_SUCCESS in case of successfull operation.
  * If not successfull, status is adjusted according errno error and ZException is set with acurate content.
   */
  ZStatus rawSeekToRelative(__FILEHANDLE__ pFd, ssize_t pRelative,size_t& pAddress);
  /**
   * @brief rawSeekBegin Sets current file position at beginning of file corresponding to descriptor pFd.
   * @return ZS_SUCCESS in case of successfull operation.
   * If not successfull, status is adjusted according errno error and ZException is set with acurate content.
   * ZS_BADFILEDESC <EBADF> File descriptor argument is not a open file descriptor.
   * ZS_INVPARAMS <EINVAL>  whence is not valid.  Or: the resulting file offset would be negative, or beyond the end of a seekable device for given offset.
   * ZS_INVADDRESS <ENXIO> whence is SEEK_DATA or SEEK_HOLE, and offset is beyond the end of the file, or whence is SEEK_DATA and offset is within a hole at the end of the file.
   * ZS_INVVALUE <EOVERFLOW> The resulting file offset cannot be represented in returned data format.
   * ZS_INVOP <ESPIPE> file descriptor is associated with a pipe, socket, or FIFO. Operation not allowed.
   * ZS_FILEPOSERR error is not recognized (errno value).
   */
  ZStatus rawSeekBegin(__FILEHANDLE__ pFd);
  /**
   * @brief rawSeekBegin Sets current file position at end of file corresponding to descriptor pFd and
   *  pOff is set with file's end position.
   * @return ZS_SUCCESS in case of successfull operation.
   * If not successfull, status is adjusted according errno error and ZException is set with acurate content.
   * ZS_BADFILEDESC <EBADF> File descriptor argument is not a open file descriptor.
   * ZS_INVPARAMS <EINVAL>  whence is not valid.  Or: the resulting file offset would be negative, or beyond the end of a seekable device for given offset.
   * ZS_INVADDRESS <ENXIO> whence is SEEK_DATA or SEEK_HOLE, and offset is beyond the end of the file, or whence is SEEK_DATA and offset is within a hole at the end of the file.
   * ZS_INVVALUE <EOVERFLOW> The resulting file offset cannot be represented in returned data format.
   * ZS_INVOP <ESPIPE> file descriptor is associated with a pipe, socket, or FIFO. Operation not allowed.
   * ZS_FILEPOSERR error is not recognized (errno value).
   */
  ZStatus rawSeekEnd(__FILEHANDLE__ pFd, size_t &pOff);
  /** same as previous but does not return an offset. Only positions to endoffile */
  ZStatus rawSeekEnd(__FILEHANDLE__ pFd);
  ZStatus rawChangePermissions(__FILEHANDLE__ pFd,__FILEACCESSRIGHTS__ pAccessRights);
  ZStatus _rawStat(__FILEHANDLE__ pFd, struct stat& pStat, bool pLogZException=false);
  ZStatus rawgetPermissions(__FILEHANDLE__ pFd, __FILEACCESSRIGHTS__& pAccessRights, bool pLogZException=false);

  /**
   * @brief rawTruncate effectively truncates (or extend) the file pointed by pFd to a global size of pLength.
   *                    pFd must be a valid file descriptor for a file open for writing,
   *
   * refer to https://www.man7.org/linux/man-pages/man3/ftruncate.3p.html
   *
   * @param pFd      a valid file descriptor open for writing
   * @param pLength  global size of the file after truncate operation.
   * @return ZS_SUCCESS in case of successfull operation.
   * If not successfull, status is adjusted according errno error and ZException is set with acurate content.
   * ZS_INVSIZE <EFBIG><EINVAL> pLength is an invalid length argument (negative or greater than the maximum file size).
   * ZS_SYSTEMERROR <EINTR> A signal was caught during execution.
   * ZS_SYSTEMERROR <EIO>  An I/O error occurred while reading from or writing to file system.
   * ZS_BADFILEDESC <EBADF> Not a file descriptor open for writing.
   */
  ZStatus rawTruncate(__FILEHANDLE__ pFd,size_t pLength);

  /**
   * @brief getNameFromFd returns the full path name of file corresponding to descriptor pFd.
   * @return file name if successfull or empty string if a problem occurred
   */
  utf8VaryingString rawGetNameFromFd(__FILEHANDLE__ pFd);

  bool testSequence (const unsigned char* pSequence,size_t pSeqLen, const unsigned char* pToCompare);

#endif // ZIOUTILS_H
