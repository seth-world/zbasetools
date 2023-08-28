#ifndef ZIOUTILS_H
#define ZIOUTILS_H

#include <config/zconfig.h>
#include <ztoolset/zdatabuffer.h>
#include <ztoolset/zstatus.h>

#include <fcntl.h>

/**
     * @brief rawOpen low level open file described by pPath, using __oflag (see https://www.man7.org/linux/man-pages/man2/open.2.html)
     * @param pStatus returned status set to ZS_SUCCESS if operation is successfull
     * @return an int with file descriptor or -1 if open operation is errored.
     * In this later case, ZException is positionned with errno information and accurate error description.
     */
  ZStatus rawOpen(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath, __FILEOPENMODE__ pMode, __FILEACCESSRIGHTS__ pPriv);
  ZStatus rawOpen(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath, __FILEOPENMODE__ pMode);

  ZStatus _rawOpen(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath, __FILEOPENMODE__ pMode, __FILEACCESSRIGHTS__ pPriv,bool pNoExcept);

  ZStatus _rawOpenGetException(int wErrno, const utf8VaryingString& pPath,bool pNoExcept ) ;


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
   * @brief getNameFromFd returns the full path name of file corresponding to descriptor pFd.
   * @return file name if successfull or empty string if a problem occurred
   */
  utf8VaryingString rawGetNameFromFd(__FILEHANDLE__ pFd);

  bool testSequence (const unsigned char* pSequence,size_t pSeqLen, const unsigned char* pToCompare);

#endif // ZIOUTILS_H
