#include <zconfig.h>

#include "zioutils.h"

#include <fcntl.h>
#include <limits>

#include <sys/stat.h> /* for fchmod */

#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zexceptionmin.h>

#include <sys/stat.h>
#include <ztoolset/uristring.h>
#include <ztoolset/zaierrors.h>

/*
ZStatus rawOpen(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath, __FILEOPENMODE__ pMode )
{
  errno=0;
  pFd=::open(pPath.toCChar(), pMode);
  if (pFd != __FILEHANDLEINVALID__ ) {
    return ZS_SUCCESS;
  }
  return _rawOpenGetException(errno,pPath,false);
}//rawOpen
*/
ZStatus rawOpenRead(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath)
{
    return rawOpen(pFd,pPath, O_RDONLY);
}

ZStatus rawOpenModify(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath)
{
    return rawOpen(pFd,pPath, O_RDWR);
}
ZStatus rawOpenCreate(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath)
{
    return rawOpen(pFd,pPath, O_CREAT|O_WRONLY|O_TRUNC);
}

ZStatus rawOpen(__FILEHANDLE__ &pFd, const utf8VaryingString& pPath, __FILEOPENMODE__ pMode, __FILEACCESSRIGHTS__ pAccessRights )
{
  errno=0;
  pFd=::open(pPath.toCChar(), pMode, pAccessRights);
  if (pFd != __FILEHANDLEINVALID__ ) {
    return ZS_SUCCESS;
  }
  return _rawOpenGetException(errno,pPath,false);
}//rawOpen



ZStatus _rawOpenGetException(int wErrno, const utf8VaryingString& pPath,bool pNoExcept ) {

  /* see https://www.man7.org/linux/man-pages/man2/open.2.html
*/

  ZStatus wSt=ZS_ERROPEN;
  utf8VaryingString wErrMsg;
  switch (wErrno) {
  case EACCES:
    wSt= ZS_ILLEGAL ;
    wErrMsg.sprintf( "<EACCES> The requested access to the file is not allowed, or search permission is denied for one of the directories in the path prefix of pathname, or the file did not exist yet and write access to the parent directory is not allowed."
                    "\nOr Where O_CREAT is specified, the protected_fifos or protected_regular sysctl is enabled, the file already exists and is a FIFO or regular file, the owner of the file is neither the current user nor the owner of the containing directory, and the containing directory is both world- or group-writable and sticky."
                    " file <%s>",pPath.toString());
    break;
  case EBADF:
    wSt = ZS_BADFILEATTR;
    wErrMsg.sprintf( "<EBADF>  pathname is relative but dirfd is neither AT_FDCWD nor a valid file descriptor."
                    " file <%s>",pPath.toString());
    break;
  case EBUSY:
    wSt = ZS_LOCKED;
    wErrMsg.sprintf( "<EBUSY> O_EXCL was specified in flags and pathname refers to a block device that is in use by the system (e.g., it is mounted)."
                    " file <%s>",pPath.toString());
    break;
  case EDQUOT:
    wSt = ZS_MEMOVFLW;
    wErrMsg.sprintf( "<EBUSY> Where O_CREAT is specified, the file does not exist, and the user's quota of disk blocks or inodes on the filesystem has been exhausted."
                    " file <%s>",pPath.toString());
    break;
  case EEXIST:
    wSt = ZS_FILEEXIST;
    wErrMsg.sprintf( "<EEXIST> pathname already exists and O_CREAT and O_EXCL were used."
                    " file <%s>",pPath.toString());
    break;
  case EFAULT:
    wSt = ZS_INVADDRESS;
    wErrMsg.sprintf( "<EFAULT> pathname points outside your accessible address space"
                    " file <%s>",pPath.toString());
    break;
  case EFBIG:
  case EOVERFLOW:
    wSt = ZS_MEMOVFLW;
    wErrMsg.sprintf( "<EOVERFLOW-EFBIG> pathname refers to a regular file that is too large to be opened. "
                    " file <%s>",pPath.toString());
    break;

  case EINTR:
    wSt = ZS_CANCEL;
    wErrMsg.sprintf( "<EEXIST-ENOTEMPTY> While blocked waiting to complete an open of a slow device (e.g., a FIFO; see fifo(7)), the call was interrupted by a signal handler."
                    " file <%s>",pPath.toString());
    break;

  case EINVAL:
    wSt = ZS_INVOP;
    wErrMsg.sprintf( "<EINVAL> The filesystem does not support the O_DIRECT flag.\n"
                    "Or Invalid value in flags.\n"
                    "Or O_TMPFILE was specified in flags, but neither O_WRONLY nor O_RDWR was specified\n"
                    "Or O_CREAT was specified in flags and the final component (basename) of the new file's pathname is invalid (e.g., it contains characters not permitted by the underlying filesystem).\n"
                    "Or The final component (basename) of pathname is invalid."
                    " file <%s>",pPath.toString());
    break;

  case EISDIR:
    wSt= ZS_FILEERROR;
    wErrMsg.sprintf( "<EISDIR> pathname refers to an existing directory, O_TMPFILE and one of O_WRONLY or O_RDWR were specified in flags, but"
                    "this kernel version does not provide the O_TMPFILE functionality."
                    " file <%s>",pPath.toString());
    break;
  case ELOOP:
    wSt= ZS_CORRUPTED;
    wErrMsg.sprintf( "<ELOOP> Too many symbolic links were encountered in resolving pathname.\n"
                    "Or pathname was a symbolic link, and flags specified O_NOFOLLOW but not O_PATH."
                    " file <%s>",pPath.toString());
    break;
  case EMFILE:
    wSt= ZS_MEMOVFLW;
    wErrMsg.sprintf( "<EMFILE> The per-process limit on the number of open file descriptors has been reached."
                    " file <%s>",pPath.toString());
    break;
  case ENFILE:
    wSt= ZS_MEMOVFLW;
    wErrMsg.sprintf( "<ENFILE> The system-wide limit on the number of open file descriptors has been reached."
                    " file <%s>",pPath.toString());
    break;
  case ENAMETOOLONG:
    wSt= ZS_INVNAME;
    wErrMsg.sprintf( "<ENAMETOOLONG> The length of a component of a pathname is longer than {NAME_MAX}."
                    "\nfile <%s>",pPath.toString());
    break;

  case ENODEV:
    wSt= ZS_INVOP;
    wErrMsg.sprintf( "<ENODEV>  pathname refers to a device special file and no corresponding device exists."
                    "\nfile <%s>",pPath.toString());
    break;
  case ENOENT:
    wSt= ZS_FILENOTEXIST;
    wErrMsg.sprintf( "<ENOENT>  A directory component in pathname does not exist or is a dangling symbolic link.\n"
                    "Or pathname refers to a nonexistent directory, O_TMPFILE and one of O_WRONLY or O_RDWR were specified in flags, but this kernel version does not provide the O_TMPFILE functionality.\n"
                    "Or O_CREAT is not set and the named file does not exist."
                    "\nfile <%s>",pPath.toString());
    break;
  case ENOMEM:
    wSt= ZS_MEMOVFLW;
    wErrMsg.sprintf( "<ENOMEM> Insufficient kernel memory was available.\n"
                    "Or The named file is a FIFO, but memory for the FIFO buffer can't be allocated because the per-user hard limit on memory allocation for pipes has been reached and the caller is not privileged."
                    "\nfile <%s>",pPath.toString());
    break;

  case ENOSPC:
    wSt= ZS_MEMOVFLW;
    wErrMsg.sprintf( "<ENOSPC> pathname was to be created but the device containing pathname has no room for the new file."
                    "\nfile <%s>",pPath.toString());
    break;

  case ENOTDIR:
    wSt= ZS_NOTDIRECTORY;
    wErrMsg.sprintf( "<ENOTDIR> A component of either path prefix names an existing file that is neither a directory nor a symbolic link to a directory; or the old argument names a directory and the new argument names a non-directory file; or the old argument contains at least one non- <slash> character and ends with one or more trailing <slash> characters and the last pathname component names an existing file that is neither a directory nor a symbolic link to a directory; or the old argument names an existing non-directory file and the new argument names a nonexistent file, contains at least one non- <slash> character, and ends with one or more trailing <slash> characters;"
                    "Or the new argument names an existing non-directory file, contains at least one non- <slash> character, and ends with one or more trailing <slash> characters."
                    "\nfile <%s>",pPath.toString());
    break;

  case ENXIO:
    wSt= ZS_INVOP;
    wErrMsg.sprintf( "<ENXIO> The file is a device special file and no corresponding device exists.\n"
                    "Or O_NONBLOCK | O_WRONLY is set, the named file is a FIFO, and no process has the FIFO open for reading.\n"
                    "Or The file is a UNIX domain socket."
                    "\nfile <%s>",pPath.toString());
    break;

  case EOPNOTSUPP:
    wSt= ZS_INVOP;
    wErrMsg.sprintf( "<EOPNOTSUPP> The filesystem containing pathname does not support O_TMPFILE."
                    "\nfile <%s>",pPath.toString());
    break;
  case EPERM:
    wSt= ZS_ACCESSRIGHTS;
    wErrMsg.sprintf( "<EPERM> The O_NOATIME flag was specified, but the effective user ID of the caller did not match the owner of the file and the caller was not privileged.\n"
                    "Or The operation was prevented by a file seal-"
                    "\nfile <%s>",pPath.toString());
    break;

  case EROFS:
    wSt= ZS_ACCESSRIGHTS;
    wErrMsg.sprintf( "<EROFS> pathname refers to a file on a read-only filesystem and write access was requested."
                    "\nfile <%s>",pPath.toString());
    break;

  case ETXTBSY:
    wSt= ZS_ACCESSRIGHTS;
    wErrMsg.sprintf( "<ETXTBSY> pathname refers to an executable image which is currently being executed and write access was requested.\n"
                    "Or pathname refers to a file that is currently in use as a swap file, and the O_TRUNC flag was specified.\n"
                    "Or pathname refers to a file that is currently being read by the kernel."
                    "\nfile <%s>",pPath.toString());
    break;

  case EWOULDBLOCK:  /* for open operation this is an error */
    wSt= ZS_WOULDBLOCK;
    wErrMsg.sprintf( "<EWOULDBLOCK> The O_NONBLOCK flag was specified, and an incompatible lease was held on the file."
                    "\nfile <%s>",pPath.toString());
    break;

  default:
    wErrMsg.sprintf( "Unknown error code for opening file <%s>.",pPath.toString());
    break;
  }// switch
  if (!pNoExcept)
    ZException.getErrno(wErrno,
        _GET_FUNCTION_NAME_,
        wSt,
        Severity_Error,
        wErrMsg.toCChar());

  return wSt;
}//_rawOpen

ZStatus
rawWriteAt(__FILEHANDLE__ pFd, ZDataBuffer& pData,size_t &pSizeWritten, size_t pAddress) {
  ZStatus wSt=rawSeekToPosition(pFd,pAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  return rawWrite(pFd,pData,pSizeWritten);
}

ZStatus rawWriteText(__FILEHANDLE__ pFd, const utf8VaryingString& pString, size_t &pSizeWritten)
{
  if (pString.isEmpty())
    return ZS_EMPTY;
  ZDataBuffer wZDB;
  wZDB.allocateBZero(pString.strlen());
  utf8_t* wPtrIn=pString.Data;
  utf8_t* wPtrOut=(utf8_t*)wZDB.Data;
  size_t wCount = pString.UnitCount;
  while (*wPtrIn && wCount--)
  {
    *wPtrOut++ = *wPtrIn++;
  }
  return rawWrite(pFd,wZDB,pSizeWritten);
}

ZStatus rawWrite(__FILEHANDLE__ pFd, ZDataBuffer& pData, size_t &pSizeWritten)
{
  if (pData.isEmpty())
    return ZS_EMPTY;
/* see https://man7.org/linux/man-pages/man2/write.2.html */
  errno=0; /* this function uses errno */
  ssize_t wSS=::write(pFd,pData.Data,pData.Size);
  if (wSS >= 0) {
    pSizeWritten = wSS;
    return ZS_SUCCESS;
  }
  int wErrno = errno;
  ZStatus wSt=ZS_WRITEERROR;
  utf8VaryingString wErrMsg;
  switch (wErrno) {

  case EWOULDBLOCK:  /* EAGAIN for open operation this is not an error */
    return ZS_WOULDBLOCK;
  case EBADF:
    wSt = ZS_BADFILEDESC;
    wErrMsg.sprintf( "<EBADF>  The file descriptor is not a valid file descriptor open for writing."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;

  case EDESTADDRREQ:
    wSt= ZS_SOCKADDRESS ;
    wErrMsg.sprintf( "<EDESTADDRREQ> fd refers to a datagram socket for which a peer address has not been set using connect(2)."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case EDQUOT:
    wSt = ZS_MEMOVFLW;
    wErrMsg.sprintf( "<EDQUOT> The user's quota of disk blocks on the filesystem containing the file referred to by file descriptor has been exhausted."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case EFAULT:
    wSt = ZS_INVADDRESS;
    wErrMsg.sprintf( "<EFAULT> buffer is outside your accessible address space."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case EFBIG:
    wSt = ZS_MEMOVFLW;
    wErrMsg.sprintf( "<EFBIG> An attempt was made to write a file that exceeds the implementation-defined maximum file size or the process's file size limit, or to write at a position past the maximum allowed offset."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;

  case EINTR:
    wSt = ZS_CANCEL;
    wErrMsg.sprintf( "<EINTR>  The call was interrupted by a signal before any data was written; see signal(7)."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;

  case EINVAL:
    wSt = ZS_INVOP;
    wErrMsg.sprintf( "<EINVAL> The file descriptor is attached to an object which is unsuitable for writing; or the file was opened with the O_DIRECT flag, and either the address specified in buf, the value specified in count, or the file offset is not suitably aligned."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;

  case EIO:
    wSt= ZS_FILEERROR;
    wErrMsg.sprintf( "<EIO> A low-level I/O error occurred while modifying the inode."
                       " This error may relate to the write-back of data written by"
                       " an earlier write(), which may have been issued to a"
                       "different file descriptor on the same file."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case ENOSPC:
    wSt= ZS_MEMOVFLW;
    wErrMsg.sprintf( "<ENOSPC> The device containing pathname has no room for the data."
                    "\nfile <%s>",rawGetNameFromFd(pFd).toString());
    break;

  case EPERM:
    wSt= ZS_MEMOVFLW;
    wErrMsg.sprintf( "<EPERM> The operation was prevented by a file seal; see fcntl(2)."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case EPIPE:
    wSt= ZS_MEMOVFLW;
    wErrMsg.sprintf( "<EPIPE> fd is connected to a pipe or socket whose reading end is closed.  When this happens the writing process will also"
                     "receive a SIGPIPE signal.  (Thus, the write return value is seen only if the program catches, blocks or ignores this signal.)"
                     " file <%s>",rawGetNameFromFd(pFd).toString());
    break;


  default:
    wErrMsg.sprintf( "Unknown error code writting file <%s>.",rawGetNameFromFd(pFd).toString());
    break;
  }// switch
  ZException.getErrno(wErrno,
      _GET_FUNCTION_NAME_,
      wSt,
      Severity_Error,
      wErrMsg.toCChar());

  return wSt;
} // rawWrite

ZStatus rawFlush(__FILEHANDLE__ pFd)
{
  errno=0;
  /* see https://linux.die.net/man/2/fdatasync */
  int wRet=fdatasync(pFd); // better than flush
  if (wRet==0)
    return ZS_SUCCESS;
  int wErrno=errno;
  ZStatus wSt=ZS_FILEERROR;
  utf8VaryingString wErrMsg;
  switch (wErrno) {

  case EBADF:
    wSt = ZS_BADFILEDESC;
    wErrMsg.sprintf( "<EBADF>  The file descriptor is not a valid file descriptor open for writting."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;

  case EIO:
    wSt= ZS_FILEERROR ;
    wErrMsg.sprintf( "<EIO> An low.level error occurred during synchronization. "
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case EROFS:
  case EINVAL :
    wSt = ZS_INVOP;
    wErrMsg.sprintf( "<EDQUOT> File descriptor is bound to a special file which does not support synchronization."
                    " file <%s>",rawGetNameFromFd(pFd).toString());
    break;

  default:
    wErrMsg.sprintf( "Unknown error code flushing data for file <%s>.",rawGetNameFromFd(pFd).toString());
    break;
  }// switch
  ZException.getErrno(wErrno,
      _GET_FUNCTION_NAME_,
      wSt,
      Severity_Error,
      wErrMsg.toCChar());

  return wSt;
} // rawFlush

ZStatus
rawReadAt(__FILEHANDLE__ pFd, ZDataBuffer& pData,size_t pBytesToRead,size_t pAddress) {
  ZStatus wSt=rawSeekToPosition(pFd,pAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  return rawRead(pFd,pData,pBytesToRead);
}

ZStatus rawReadText(__FILEHANDLE__ pFd, utf8VaryingString& pString, size_t pBytesToRead)
{
  ZDataBuffer wZDB;
  ZStatus wSt= rawRead(pFd,wZDB,pBytesToRead);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  pString.allocateUnitsBZero(wZDB.Size);
  utf8_t* wPtrOut=pString.Data;
  utf8_t* wPtrIn=(utf8_t*)wZDB.Data;
  size_t wCount = wZDB.Size;
  while (*wPtrIn && wCount--)
  {
    *wPtrOut++ = *wPtrIn++;
  }
  return ZS_SUCCESS;
}
ZStatus rawReadTextAt(__FILEHANDLE__ pFd, utf8VaryingString& pString, size_t pBytesToRead,size_t pAddress)
{
  ZDataBuffer wZDB;
  ZStatus wSt= rawReadAt(pFd,wZDB,pBytesToRead,pAddress);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  pString.allocateUnitsBZero(wZDB.Size);
  utf8_t* wPtrOut=pString.Data;
  utf8_t* wPtrIn=(utf8_t*)wZDB.Data;
  size_t wCount = wZDB.Size;
  while (*wPtrIn && wCount--)
  {
    *wPtrOut++ = *wPtrIn++;
  }
  return ZS_SUCCESS;
}
ZStatus
rawRead(__FILEHANDLE__ pFd, ZDataBuffer& pData,size_t pBytesToRead) {
  ssize_t wBRead=0;
/* see https://man7.org/linux/man-pages/man2/read.2.html */
  ZStatus wSt=ZS_SUCCESS;
  pData.clear();
  pData.allocate(pBytesToRead);
  errno=0;
  wBRead=::read(pFd,pData.Data,pBytesToRead);
  if (wBRead==0) {
      pData.clear();
      return ZS_EOF;
  }
  if (wBRead > 0) {
    if (wBRead < pBytesToRead) {
      wSt = ZS_READPARTIAL;
      pData.truncate(size_t(wBRead));
    }
    else
      wSt = ZS_SUCCESS;
    return wSt;
  }

  int wErrno = errno;
  wSt=ZS_ILLEGAL;
  utf8VaryingString wErrMsg;
  switch (wErrno) {

//  case EAGAIN:
  case EWOULDBLOCK:  /* for read operation these both status are not an error */
    /*
     *        EAGAIN The file descriptor fd refers to a file other than a
              socket and has been marked nonblocking (O_NONBLOCK), and
              the read would block.  See open(2) for further details on
              the O_NONBLOCK flag.

     *        EAGAIN or EWOULDBLOCK
              The file descriptor fd refers to a socket and has been
              marked nonblocking (O_NONBLOCK), and the read would block.
              POSIX.1-2001 allows either error to be returned for this
              case, and does not require these constants to have the
              same value, so a portable application should check for
              both possibilities.
    */

    return ZS_WOULDBLOCK;

  case EBADF:
    wSt = ZS_BADFILEDESC;
    wErrMsg.sprintf( "<EBADF>  file descriptor is not a valid file descriptor or file is not open for reading."
                    "\n file <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case EFAULT:
    wSt = ZS_MEMERROR;
    wErrMsg.sprintf( "<EFAULT>  buffer is outside your accessible address space."
                    "\n file <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case EINTR:
    wSt= ZS_CANCEL ;
    wErrMsg.sprintf( "<EINTR> TThe call was interrupted by a signal before any data was read."
                    "\n file <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case EINVAL:
    wSt = ZS_INVPARAMS;
    wErrMsg.sprintf( "<EINVAL> fd is attached to an object which is unsuitable for reading; or the file was opened with the O_DIRECT flag, and either the address specified in buf, the value specified in count, or the file offset is not suitably aligned.\n"
                      "Or fd was created via a call to timerfd_create(2) and the wrong size buffer was given to read()."
                      "\n file <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case EIO:
    wSt = ZS_MEMOVFLW;
    wErrMsg.sprintf(  "<EIO> I/O error.see corresponding error in https://man7.org/linux/man-pages/man2/read.2.html"
                      "\nfile <%s>",rawGetNameFromFd(pFd).toString());
    break;
  case EISDIR:
    wSt = ZS_BADFILEDESC;
    wErrMsg.sprintf( "<EISDIR> fd refers to a directory."
                      "\nfile <%s>",rawGetNameFromFd(pFd).toString());
    break;


  default:
    wErrMsg.sprintf( "Unknown error code for reading file <%s>.",rawGetNameFromFd(pFd).toString());
    break;
  }// switch
  ZException.getErrno(wErrno,
      _GET_FUNCTION_NAME_,
      wSt,
      Severity_Error,
      wErrMsg.toCChar());

  return wSt;
}// rawRead

ZStatus
rawClose(__FILEHANDLE__ &pFd) {
  /* see https://man7.org/linux/man-pages/man3/close.3p.html */

  errno=0;
  int wRet=::close(pFd);
  if (wRet>=0) {
    pFd=-1;
    return ZS_SUCCESS;
  }
  int wErrno=errno;
  ZStatus wSt=ZS_FILEERROR;
  utf8VaryingString wErrMsg;
  switch (wErrno) {
  case EBADF:
    wErrMsg.sprintf("<EBADF> File descriptor argument is not a open file descriptor. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt = ZS_BADFILEDESC;
    break;
  case EINTR:
    wErrMsg.sprintf("<EINTR> The close() function was interrupted by a signal. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt = ZS_CANCEL;
    break;
  case EIO:
    wErrMsg.sprintf("<EIO> An I/O error occurred while reading from or writing to the file system. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt = ZS_CANCEL;
    break;
  default:
    wErrMsg.sprintf("Unknown error while closing file <%s>.",
        rawGetNameFromFd(pFd).toString());
    wSt = ZS_FILEERROR;
    break;
  }
  ZException.getErrno(wErrno,
      _GET_FUNCTION_NAME_,
      wSt,
      Severity_Error,
      wErrMsg.toCChar());
  return wSt;
} // rawClose



ZStatus _rawSeekError(int wErrno,__FILEHANDLE__ pFd,__off_t &pOff)
{
  ZStatus wSt=ZS_FILEPOSERR;
  utf8VaryingString wErrMsg;
  switch (wErrno) {
  case EBADF:
    wErrMsg.sprintf("<EBADF> File descriptor argument is not a open file descriptor. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_BADFILEDESC;
    break;
  case EINVAL:
    wErrMsg.sprintf("<EINVAL>  whence is not valid.  Or: the resulting file offset would be negative, or beyond the end of a seekable device offset %d. File <%s>",
        pOff,
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_INVPARAMS;
    break;
  case ENXIO:
    wErrMsg.sprintf("<ENXIO> whence is SEEK_DATA or SEEK_HOLE, and offset is beyond the end of the file, or whence is SEEK_DATA and offset is within a hole at the end of the file. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_EOF;
    break;
  case EOVERFLOW:
    wErrMsg.sprintf("<EOVERFLOW> The resulting file offset cannot be represented in returned data format. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_INVVALUE;
    break;
  case ESPIPE:
    wErrMsg.sprintf("<ESPIPE> file descriptor is associated with a pipe, socket, or FIFO. Operation not allowed. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_INVOP;
    break;
  default:
    wErrMsg.sprintf("Unknown error while positionning on file <%s>.",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_FILEPOSERR;
    break;
  }//switch

  ZException.getErrno(wErrno,
      "Seek operation",
      wSt,
      Severity_Error,
      wErrMsg.toCChar());
  return wSt;

  if (BaseParameters->VerboseFileEngine())
    ZException.printLastUserMessage(stderr);
  return wSt;
}

ZStatus rawSeek(__FILEHANDLE__ pFd, __off_t &pOff, int pWhence)
{
  ZStatus wSt=ZS_SUCCESS;
  /* see https://man7.org/linux/man-pages/man2/lseek.2.html */
  errno=0;
  pOff =  lseek(pFd,pOff,pWhence);
  if (pOff >= 0) {
    return ZS_SUCCESS;
  }
  return _rawSeekError(errno,pFd,pOff);
}
ZStatus rawSeekToPosition(__FILEHANDLE__ pFd, size_t pAddress)
{
  ZStatus wSt=ZS_SUCCESS;
  /* see https://man7.org/linux/man-pages/man2/lseek.2.html */
  errno=0;
  __off_t wOff=__off_t(pAddress);
  wOff =  lseek(pFd,wOff,SEEK_SET);
  if (wOff >= 0) {
    return ZS_SUCCESS;
  }
  return _rawSeekError(errno,pFd,wOff);
}
ZStatus rawSeekToRelative(__FILEHANDLE__ pFd, ssize_t pRelative, size_t &pAddress)
{
  ZStatus wSt=ZS_SUCCESS;
  /* see https://man7.org/linux/man-pages/man2/lseek.2.html */
  errno=0;
  __off_t wOff=__off_t(pRelative);
  wOff =  lseek(pFd,wOff,SEEK_CUR);
  if (wOff >= 0) {
    pAddress = size_t(wOff);
    return ZS_SUCCESS;
  }
  return _rawSeekError(errno,pFd,wOff);
}

ZStatus rawSeekBegin(__FILEHANDLE__ pFd)
{
  ZStatus wSt=ZS_SUCCESS;
  /* see https://man7.org/linux/man-pages/man2/lseek.2.html */
  errno=0;
  __off_t wOff =  lseek(pFd,__off_t(0),SEEK_SET);
  if (wOff >= 0) {
    return ZS_SUCCESS;
  }
  return _rawSeekError(errno,pFd,wOff);
}
ZStatus rawSeekEnd(__FILEHANDLE__ pFd, size_t &pOff)
{
  ZStatus wSt=ZS_SUCCESS;
  /* see https://man7.org/linux/man-pages/man2/lseek.2.html */
  errno=0;
  off_t wOff =  lseek(pFd,__off_t(0),SEEK_END);
  if (wOff >= 0) {
    pOff = size_t(wOff);
    return ZS_SUCCESS;
  }
  return _rawSeekError(errno,pFd,wOff);
}

ZStatus rawSeekEnd(__FILEHANDLE__ pFd) {
  size_t wOff=0;
  return rawSeekEnd(pFd,wOff);
}

ZStatus rawAllocate(__FILEHANDLE__ pFd, size_t pOffset,size_t pBytes )
{
  ZStatus wSt=ZS_SUCCESS;
  /* see https://man7.org/linux/man-pages/man3/posix_fallocate.3.html */
  /* errno is not modified by posix_fallocate */
  int wRet =  posix_fallocate(pFd,(off_t) pOffset,(off_t)pBytes);
  if (wRet == 0) {
    return ZS_SUCCESS;
  }

  utf8VaryingString wErrMsg;
  switch (wRet) {
  case EBADF:
    wErrMsg.sprintf("<EBADF> File descriptor argument is not a open file descriptor or is not opened for writing. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_BADFILEDESC;
    break;
  case EINVAL:
    wErrMsg.sprintf("<EINVAL>  offset was less than 0, or len was less than or equal to 0, or the underlying filesystem does not support the operation. Offset %d Allocation %d. File <%s>",
        pOffset,
        pBytes,
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_INVPARAMS;
    break;
  case EFBIG:
    wErrMsg.sprintf("<EFBIG> offset+len exceeds the maximum file size. Offset %d Allocation %d. File <%s>",
        pOffset,
        pBytes,
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_CANTALLOCSPACE;
    break;
  case EINTR:
    wErrMsg.sprintf("<EINTR> A signal was caught during execution of allocation. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_CANCEL;
    break;
  case ENODEV:
    wErrMsg.sprintf("<ENODEV> file descriptor does not refer to a regular file. Operation not allowed. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_INVOP;
    break;
  case ENOSPC:
    wErrMsg.sprintf("<ENOSPC> There is not enough space left on the device containing the file referred to by the file descriptor. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_CANTALLOCSPACE;
    break;
  case EOPNOTSUPP:
    wErrMsg.sprintf("<EOPNOTSUPP> The filesystem containing the file referred to by the file descriptor does not support this operation. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_INVOP;
    break;
  case ESPIPE:
    wErrMsg.sprintf("<ESPIPE> file descriptor is associated with a pipe, socket, or FIFO. Operation not allowed. File <%s>",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_INVOP;
    break;
  default:
    wErrMsg.sprintf("Unknown error while allocating space on file <%s>.",
        rawGetNameFromFd(pFd).toString());
    wSt=ZS_CANTALLOCSPACE;
    break;
  }//switch

  ZException.setMessage(
      _GET_FUNCTION_NAME_,
      wSt,
      Severity_Error,
      wErrMsg.toCChar());

  if (BaseParameters->VerboseFileEngine())
    ZException.printLastUserMessage(stderr);
  return wSt;
} // rawAllocate

ZStatus rawChangePermissions(__FILEHANDLE__ pFd,__FILEACCESSRIGHTS__ pAccessRights)
{
  errno=0;
  int wRet=fchmod(pFd,pAccessRights);
  if (wRet==0)
    return ZS_SUCCESS;

  int wErrno=errno;
  ZStatus wSt=ZS_ILLEGAL;
  utf8VaryingString wErrMsg;
  switch (wErrno) {
    case EACCES:
      wSt= ZS_ACCESSRIGHTS ;
      wErrMsg.sprintf( "<EACCES> A component of either path prefix denies search permission; or one of the directories containing old or new denies write permissions; or, write permission is required and is denied for a directory pointed to by the old or new arguments."
                      " file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    case EPERM:
      wSt= ZS_ACCESSRIGHTS;
      wErrMsg.sprintf( "<EPERM> The effective UID does not match the owner of the file, and the process is not privileged."
                      " Or The file is marked immutable or append-only."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
    case EROFS:
      wSt= ZS_FILEERROR;
      wErrMsg.sprintf( "<EROFS> The named file resides on a read-only filesystem."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
      break;
    case EBADF:
      wSt= ZS_BADFILEDESC;
      wErrMsg.sprintf( "<EBADF> The file descriptor is not valid."
                      " file <%s> ",rawGetNameFromFd(pFd).toString());
      break;
    case EFAULT:
      wSt= ZS_LOCKED;
      wErrMsg.sprintf( "<EFAULT> File's pathname points outside accessible address space."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
      break;
    case EINVAL:
      wSt= ZS_INVPARAMS;
      wErrMsg.sprintf( "<EINVAL> Invalid access rights value."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
      break;
    case EIO:
      wSt= ZS_FILEERROR;
      wErrMsg.sprintf( "<EIO> A physical I/O error has occurred."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
      break;
    case ELOOP:
      wSt= ZS_CORRUPTED;
      wErrMsg.sprintf( "<ELOOP> A loop exists in symbolic links encountered during resolution of the path argument."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
      break;
    case ENAMETOOLONG:
      wSt= ZS_INVNAME;
      wErrMsg.sprintf( "<ENAMETOOLONG> The length of a component of a pathname is longer than {NAME_MAX}."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
      break;
    case ENOENT:
      wSt= ZS_FILENOTEXIST;
      wErrMsg.sprintf( "<ENOENT>  The link named by old does not name an existing file, a component of the path prefix of new does not exist, or either old or new points to an empty string."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
      break;
    case ENOMEM:
      wSt= ZS_MEMOVFLW;
      wErrMsg.sprintf( "<ENOMEM> Insufficient kernel memory was available."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
      break;
    case ENOTDIR:
      wSt= ZS_NOTDIRECTORY;
      wErrMsg.sprintf( "<ENOTDIR> A component of either path prefix names an existing file that is neither a directory nor a symbolic link to a directory; or the old argument names a directory and the new argument names a non-directory file; or the old argument contains at least one non- <slash> character and ends with one or more trailing <slash> characters and the last pathname component names an existing file that is neither a directory nor a symbolic link to a directory; or the old argument names an existing non-directory file and the new argument names a nonexistent file, contains at least one non- <slash> character, and ends with one or more trailing <slash> characters;"
                      " Or the new argument names an existing non-directory file, contains at least one non- <slash> character, and ends with one or more trailing <slash> characters."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
      break;


    case ENOTSUP:
      wSt= ZS_NOTDIRECTORY;
      wErrMsg.sprintf( "<ENOTSUP> Flags specified AT_SYMLINK_NOFOLLOW, which is not supported."
                      " file <%s>",rawGetNameFromFd(pFd).toString());
      break;

    default:
      wErrMsg.sprintf( "Unknown error for changing access rights for file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    }// switch
    ZException.getErrno(wErrno,
        _GET_FUNCTION_NAME_,
        wSt,
        Severity_Error,
        wErrMsg.toCChar());
    return wSt;
} //rawChangePermissions

ZStatus rawgetPermissions(__FILEHANDLE__ pFd,__FILEACCESSRIGHTS__& pAccessRights,bool pLogZException)
{
  struct stat wStat;
  ZStatus wSt=_rawStat(pFd,wStat,pLogZException);
  if (wSt!=ZS_SUCCESS)
    return wSt;
  pAccessRights = (wStat.st_mode & 0x777);
  return ZS_SUCCESS;
}

ZStatus _rawStat(__FILEHANDLE__ pFd,struct stat& pStat, bool pLogZException)
{
  ZStatus wSt=ZS_SUCCESS;
  memset(&pStat,0,sizeof(pStat));
  errno= 0; /* errno is set by this function */
  int wRet= fstat(pFd,&pStat);
  if (wRet < 0) {
    int wErrno=errno;
    utf8VaryingString wErrMsg;
    wSt= ZS_FILEERROR;
    switch (wErrno) {
    case ENOENT:
      wSt= ZS_FILENOTEXIST;
      wErrMsg.sprintf( "<ENOENT> A component of <%s> does not exist or is a dangling symbolic link.",rawGetNameFromFd(pFd).toString());
      break;
    case EACCES:
      wSt= ZS_USERPRIVILEGES;
      wErrMsg.sprintf( "<EACCES> Search permission is denied for one of the directories in the path prefix of <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    case EBADF:
      wSt= ZS_FILENOTOPEN;
      wErrMsg.sprintf( "<EBADF> File descriptor is not a valid open file descriptor for file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    case EFAULT:
      wSt= ZS_CORRUPTED;
      wErrMsg.sprintf( "<EFAULT> Bad address for file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    case EINVAL:
      wSt= ZS_INVPARAMS;
      wErrMsg.sprintf( "<EINVAL> (fstatat()) Invalid flag specified in flags for file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    case ELOOP:
      wSt= ZS_BADFILEDESC;
      wErrMsg.sprintf( "<ELOOP> Too many symbolic links encountered while traversing the path. for file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    case ENAMETOOLONG:
      wSt= ZS_INVNAME;
      wErrMsg.sprintf( "<ENAMETOOLONG> pathname is too long for file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    case ENOMEM:
      wSt= ZS_MEMERROR;
      wErrMsg.sprintf( "<ENOMEM> Out of memory (i.e., kernel memory) for file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    case ENOTDIR:
      wSt= ZS_BADFILEDESC;
      wErrMsg.sprintf( "<ENOTDIR> A component of the path prefix of pathname is not a directory or referring to a file other than a directory for file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    case EOVERFLOW:
      wSt= ZS_BADFILEDESC;
      wErrMsg.sprintf( "<EOVERFLOW> Pathname or fd refers to a file whose size, inode number, or number of blocks cannot be represented for file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    default:
      wErrMsg.sprintf( "Unknown error for file <%s>.",rawGetNameFromFd(pFd).toString());
      break;
    }// switch
    if (pLogZException)
      ZException.getErrno(  wErrno,     /* saved errno */
          _GET_FUNCTION_NAME_,
          wSt,
          Severity_Error,
          wErrMsg.toCChar());

    return(wSt);
  } // if (wRet < 0)

  return ZS_SUCCESS;
}

enum ZFileNameParse_enum : uint8_t
{
    ZFNP_Nothing        = 0,
    ZFNP_SameRoot       = 1,
    ZFNP_SameExt        = 2
};

ZStatus rawRename(const uriString &pFile,
                  const utf8VaryingString &pNewURI,
                  bool pNoExcept,
                  ZaiErrors *pErrorLog)
{
    ZStatus wSt=ZS_SUCCESS;
    errno=0;
    int wRet= std::rename(pFile.toCChar(),pNewURI.toCChar());
    if (wRet==0)
        return ZS_SUCCESS;
    int wErrno=errno;

    utf8VaryingString wErrMsg;
    switch (wErrno) {
    case EACCES:
        wSt = ZS_ACCESSRIGHTS;
        wErrMsg.sprintf("<EACCES> A component of either path prefix denies search permission; "
                        "or one of the directories containing old or new denies write "
                        "permissions; or, write permission is required and is denied for a "
                        "directory pointed to by the old or new arguments."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;
    case EBUSY:
        wSt = ZS_LOCKED;
        wErrMsg.sprintf(
            "<EBUSY> The directory named by old or new is currently in use by the system or "
            "another process, and the implementation considers this an error."
            " Renaming file <%s> to <%s>",
            pFile.toString(),
            pNewURI.toString());
        break;
    case EEXIST:
    case ENOTEMPTY:
        wSt = ZS_FILEEXIST;
        wErrMsg.sprintf("<EEXIST-ENOTEMPTY> The link named by new is a directory that is not "
                        "an empty directory."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;
    case EINVAL:
        wSt = ZS_INVOP;
        wErrMsg.sprintf(
            "<EINVAL> The old pathname names an ancestor directory of the new pathname, or "
            "either pathname argument contains a final component that is dot or dot-dot."
            " Renaming file <%s> to <%s>",
            pFile.toString(),
            pNewURI.toString());
        break;
    case EIO:
        wSt = ZS_FILEERROR;
        wErrMsg.sprintf("<EIO> A physical I/O error has occurred."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;
    case EISDIR:
        wSt = ZS_NOTDIRECTORY;
        wErrMsg.sprintf("<EISDIR> The new argument points to a directory and the old argument "
                        "points to a file that is not a directory."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;
    case ELOOP:
        wSt = ZS_CORRUPTED;
        wErrMsg.sprintf("<ELOOP> A loop exists in symbolic links encountered during resolution "
                        "of the path argument."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;
    case EMLINK:
        wSt = ZS_OUTBOUNDHIGH;
        wErrMsg.sprintf("<EMLINK> The file named by old is a directory, and the link count of "
                        "the parent directory of new would exceed {LINK_MAX}."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;

    case ENAMETOOLONG:
        wSt = ZS_INVNAME;
        wErrMsg.sprintf(
            "<ENAMETOOLONG> The length of a component of a pathname is longer than {NAME_MAX}."
            " Renaming file <%s> to <%s>",
            pFile.toString(),
            pNewURI.toString());
        break;
    case ENOENT:
        wSt = ZS_FILENOTEXIST;
        wErrMsg.sprintf("<ENOENT>  The link named by old does not name an existing file, a "
                        "component of the path prefix of new does not exist, or either old or "
                        "new points to an empty string."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;
    case ENOSPC:
        wSt = ZS_MEMOVFLW;
        wErrMsg.sprintf("<ENOSPC> The directory that would contain new cannot be extended."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;

    case ENOTDIR:
        wSt = ZS_NOTDIRECTORY;
        wErrMsg.sprintf(
            "<ENOTDIR> A component of either path prefix names an existing file that is "
            "neither a directory nor a symbolic link to a directory; or the old argument names "
            "a directory and the new argument names a non-directory file; or the old argument "
            "contains at least one non- <slash> character and ends with one or more trailing "
            "<slash> characters and the last pathname component names an existing file that is "
            "neither a directory nor a symbolic link to a directory; or the old argument names "
            "an existing non-directory file and the new argument names a nonexistent file, "
            "contains at least one non- <slash> character, and ends with one or more trailing "
            "<slash> characters;"
            " or the new argument names an existing non-directory file, contains at least one "
            "non- <slash> character, and ends with one or more trailing <slash> characters."
            " Renaming file <%s> to <%s>",
            pFile.toString(),
            pNewURI.toString());
        break;

    case EPERM:
        wSt = ZS_ACCESSRIGHTS;
        wErrMsg.sprintf("<EPERM> he S_ISVTX flag is set on the directory containing the file "
                        "referred to by old "
                        "and the process does not satisfy the criteria specified in XBD "
                        "Directory Protection with respect to old; or new refers to an "
                        "existing file, the S_ISVTX flag is set on the directory containing "
                        "this file, and the process does not satisfy the criteria specified in "
                        "XBD Directory Protection with respect to this file."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;
    case EROFS:
        wSt = ZS_FILEERROR;
        wErrMsg.sprintf("<EROFS> The requested operation requires writing in a directory on a "
                        "read-only file system."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;
    case EXDEV:
        wSt = ZS_FILEERROR;
        wErrMsg.sprintf("<EXDEV> The links named by new and old are on different file systems "
                        "and the implementation does not support links between file systems."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;
    case ETXTBSY:
        wSt = ZS_LOCKED;
        wErrMsg.sprintf("<ETXTBSY>  The file named by new exists and is the last directory "
                        "entry to a pure procedure (shared text) file that is being executed."
                        " Renaming file <%s> to <%s>",
                        pFile.toString(),
                        pNewURI.toString());
        break;

    default:
        wErrMsg.sprintf("Unknown error for renaming file <%s>.", pFile.toString());
        break;
    }// switch
    if (!pNoExcept) {
        ZException.getErrno(wErrno,
                            _GET_FUNCTION_NAME_,
                            wSt,
                            Severity_Error,
                            wErrMsg.toCChar());


        if (pErrorLog!=nullptr)
                pErrorLog->logZExceptionLast("rawRename");
    }
    return wSt;
}

ZStatus rawRenameBck(const uriString &pFile,
                     const utf8VaryingString &pBckExt,
                     bool pNoExcept,
                     ZaiErrors *pErrorLog)
{
    ZStatus wSt;

    int wFormerNumber = 1;
    uriString  wNewURI ;

    do {
        wFormerNumber ++;
        wNewURI=pFile;
        wNewURI.addsprintf("_%s%02d",pBckExt.toString(),wFormerNumber);
    } while (wNewURI.exists());

    return rawRename(pFile,wNewURI,pNoExcept,pErrorLog);
}


int rawCopyPayLoad = 10000;

void setRawCopyPayLoad(int pPayload) {rawCopyPayLoad = pPayload;}
/**
 * @brief rawCopy
 * @param pSource
 * @param pTarget
 * @param pFlag
 * @param pPayLoad
 * @param pErrorLog
 * @return
 */
ZStatus rawCopy(const uriString &pSource,
                const uriString &pTarget,
                uint8_t pFlag,
                long pPayLoad,
                __progressCallBack__(_progressCallBack),
                __progressSetupCallBack__(_progressSetupCallBack),
                ZaiErrors* pErrorLog)
{
    ZStatus wSt=ZS_SUCCESS;
    __FILEHANDLE__ wSourceFd=__FILEHANDLEINVALID__, wTargetFd=__FILEHANDLEINVALID__;
    ZDataBuffer wBlock;
    uriString   wTarget=pTarget;
    uriString   wSourceDir = pSource.getDirectoryPath();

    if (!pSource.exists()) {
        ZException.setMessage("rawCopy",ZS_FILENOTEXIST,Severity_Error,"Source file %s does not exist.",
                              pSource.toString());
        if (pErrorLog!=nullptr)
            pErrorLog->logZExceptionLast("rawCopy: Copy source file");
        return ZS_FILENOTEXIST;
    }
    if (wTarget.isEmpty()) {
        ZException.setMessage("rawCopy",ZS_EMPTY,Severity_Error,"Target is empty.");
        if (pErrorLog!=nullptr)
            pErrorLog->logZExceptionLast("rawCopy: Copy target file");
        return ZS_EMPTY;
    }

    utf8VaryingString wTargetBase = wTarget.getBasename();
    if (wTargetBase.isEmpty())  {
        wTarget.addConditionalDirectoryDelimiter();
        wTarget += pSource.getBasename();
    }
    else {
        utf8VaryingString wTargetRoot=wTarget.getRootname();
        if (wTargetRoot.isEmpty()) {
            ZException.setMessage("rawCopy",ZS_INVNAME,Severity_Error,"Target file name  %s is invalid. Root name missing.",
                                  wTargetBase.toString());
            if (pErrorLog!=nullptr) {
                pErrorLog->logZExceptionLast("rawCopy: Copy target file");
            }
            return ZS_INVNAME;
        }
    }

    size_t wSourceSize = pSource.getFileSize();

    if (wSourceSize==0) {
        ZException.setMessage("rawCopy",ZS_EMPTYFILE,Severity_Warning,"Source file %s is empty.",
                              pSource.toString());
        if (pErrorLog!=nullptr)
            pErrorLog->warningLog("rawCopy Source file %s is empty.",
                                  pSource.toString());
    }


    if (pSource == wTarget ) {
        ZException.setMessage("rawCopy",ZS_FILEEXIST,Severity_Error,"Cannot copy a file onto itself.",
                              wTargetBase.toString());
        if (pErrorLog!=nullptr) {
            pErrorLog->logZExceptionLast("rawCopy: Copy target file");
        }
        return ZS_FILEEXIST;
    }
    if (wTarget.exists()) {
        if (pFlag & ZMNP_Backup) {
            wSt=rawRenameBck(wTarget,"bck",pErrorLog);
        } // ZMNP_Backup
        else
        if (!(pFlag & ZMNP_Replace)) {
            ZException.setMessage("rawCopy",ZS_FILEEXIST,Severity_Error,"Target file %s already exists and <no replace> option has been chosen.",
                                  wTarget.toString());
            if (pErrorLog!=nullptr) {
                pErrorLog->logZExceptionLast("rawCopy: Copy target file");
            }
            return ZS_FILEEXIST;
        }// if ZMNP_Replace
    } // exists

    wSt=rawOpen(wSourceFd,pSource,O_RDONLY);
    if (wSt!=ZS_SUCCESS)
        return wSt;

    wSt=rawOpen(wTargetFd,wTarget,O_CREAT|O_WRONLY|O_TRUNC);
    if (wSt!=ZS_SUCCESS) {
        rawClose(wSourceFd);
        return wSt;
    }

    if (_progressSetupCallBack!=nullptr)
        _progressSetupCallBack(wSourceSize,pSource);

    if (pPayLoad < 0)
        pPayLoad = rawCopyPayLoad ;

    size_t wSumSizeRead=0,wSumSizeWritten=0,wWriteSize=0;
    int wAccess=0;
    if (wSourceSize > 0) {
        wSt=rawRead(wSourceFd,wBlock,pPayLoad);
        if (_progressCallBack!=nullptr)
            _progressCallBack(wSumSizeRead,"Bytes copied");
        while ((wSt==ZS_SUCCESS)||(wSt==ZS_READPARTIAL)) {

            wAccess++;
            wSumSizeRead += wBlock.Size;
            wSt=rawWrite(wTargetFd,wBlock,wWriteSize);
            if (wSt!=ZS_SUCCESS)
                break;
            wSumSizeWritten += wWriteSize;
            if (_progressCallBack!=nullptr)
                _progressCallBack(wSumSizeWritten,utf8VaryingString());
            wSt=rawRead(wSourceFd,wBlock,pPayLoad);
        }
    }// if source size > 0
    else
        wSt=ZS_SUCCESS;
    if (_progressCallBack!=nullptr)
        _progressCallBack(wSumSizeRead,utf8VaryingString());
rawCopyEnd:
    rawClose(wSourceFd);
    rawClose(wTargetFd);
    if (pErrorLog!=nullptr) {
        pErrorLog->textLog("File copied from %s to %s",pSource.toString(),pTarget.toString());
        pErrorLog->textLog("Bytes read %ld written %ld access times %d End status %s",
                           wSumSizeRead,wSumSizeWritten,wAccess,decode_ZStatus(wSt));
    }
    if ((wSt!=ZS_SUCCESS) && (pErrorLog!=nullptr)) {
        pErrorLog->logZExceptionLast("Copy file");
    }
    return wSt;
}

ZStatus rawTruncate(__FILEHANDLE__ pFd,size_t pLength)
{
    errno=0;
    int wRet=ftruncate(pFd,off_t(pLength));
    if (wRet==0)
        return ZS_SUCCESS;
    int wErrno=errno;
    utf8VaryingString wErrMsg;
    ZStatus wSt;
    switch(wErrno)
    {
    case EINVAL:
    case EFBIG:
        wSt= ZS_INVSIZE;
        wErrMsg.sprintf( "<EFBIG><EINVAL> <%ld> is an invalid length argument (negative or greater than the maximum file size)."
                        "File <%s>.",pLength,rawGetNameFromFd(pFd).toString());
        break;
    case EINTR:
        wSt= ZS_SYSTEMERROR;
        wErrMsg.sprintf( "<EINTR> A signal was caught during execution."
                        "File <%s>.",rawGetNameFromFd(pFd).toString());
        break;

    case EIO:
        wSt= ZS_SYSTEMERROR;
        wErrMsg.sprintf( "<EIO>  An I/O error occurred while reading from or writing to file system."
                        "File <%s>.",rawGetNameFromFd(pFd).toString());
        break;
    case EBADF:
        wSt= ZS_BADFILEDESC;
        wErrMsg.sprintf( "<EBADF>   Not a file descriptor open for writing."
                        "File <%s>.",rawGetNameFromFd(pFd).toString());
        break;

    }
    ZException.getErrno(  wErrno,     /* saved errno */
                        _GET_FUNCTION_NAME_,
                        wSt,
                        Severity_Error,
                        wErrMsg.toCChar());
}


#ifdef __USE_LINUX__
utf8VaryingString
rawGetNameFromFd(__FILEHANDLE__ pFd)
{
  const size_t cst_MaxName = PATH_MAX;
  char wFName[PATH_MAX];
  memset (wFName,0,PATH_MAX);
  utf8VaryingString wPath;
  wPath.sprintf("/proc/self/fd/%d",pFd);
  ssize_t wRet=readlink(wPath.toCChar(),wFName,cst_MaxName);
  if (wRet>=0) {
    return utf8VaryingString(wFName);
  }
  //  int wErrno=errno;
  return utf8VaryingString();
}
#endif
#ifdef __USE_OSX__ | __USE_IOS__
utf8VaryingString
rawGetNameFromFd(__FILEHANDLE__ pFd)
{
  const size_t cst_MaxName = PATH_MAX;
  char wFName[PATH_MAX];
  memset (wFName,0,PATH_MAX);
  utf8VaryingString wPath;

  if (fcntl(pFd, F_GETPATH, wFName) >= 0) {
    return utf8VaryingString(wFName);
  }
  return utf8VaryingString("<cannot get name>");
}

#endif
#ifdef __USE_WINDOWS__
/* see https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlea */
utf8VaryingString
rawGetNameFromFd(__FILEHANDLE__ pFd)
{
  const size_t cst_MaxName = MAX_PATH;
  TCHAR wFName[MAX_PATH];
  char wOName[MAX_PATH];
  for (int wi=0;wi < MAX_PATH;wi++) {
    wFName[wi]=0;
    wOName[wi]=0;
  }
  utf8VaryingString wPath;
  DWORD wRet =  GetFinalPathNameByHandle( pFd, wFName, MAX_PATH, VOLUME_NAME_NT );

  /* If the function fails for any other reason, the return value is zero. To get extended error information, call GetLastError. */
  if (wRet > 0) {
    if (wRet > MAX_PATH)
      return utf8VaryingString("<name too long>");
    if (sizeof(TCHAR)<2)
      return utf8VaryingString((char*)wFName);
    else {
      utf8VaryingString wRet;
      wRet.fromUtf32((utf32_t*)wFName)
    }
  }
  return utf8VaryingString("<cannot get name>");
}
#endif //__USE_WINDOWS__





bool testSequence (const unsigned char* pSequence,size_t pSeqLen, const unsigned char* pToCompare)
{
  while (pSeqLen) {
    if (*pSequence != *pToCompare)
      return false;
    pSequence++;
    pToCompare++;
    pSeqLen--;
  }
  return true;
}
utf8VaryingString
decode_ZMNP(ZMNP_type pFlag)
{
    utf8VaryingString wReturn;
    if (pFlag==0) {
        return ("ZMNP_Nothing");
    }
    if (pFlag & ZMNP_Replace) {
        wReturn.addConditionalOR("ZMNP_Replace");
    }
    if (pFlag & ZMNP_Backup) {
        wReturn.addConditionalOR("ZMNP_Backup");
    }
    if (pFlag & ZMNP_KeepPerm) {
        wReturn.addConditionalOR("ZMNP_KeepPerm");
    }
    if (pFlag & ZMNP_IncludeAll) {
        wReturn.addConditionalOR("ZMNP_IncludeAll");
    }
    return wReturn;
}
