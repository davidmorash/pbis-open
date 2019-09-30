/*
 * Copyright © BeyondTrust Software 2004 - 2019
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * BEYONDTRUST MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING TERMS AS
 * WELL. IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT WITH
 * BEYONDTRUST, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE TERMS OF THAT
 * SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE APACHE LICENSE,
 * NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU HAVE QUESTIONS, OR WISH TO REQUEST
 * A COPY OF THE ALTERNATE LICENSING TERMS OFFERED BY BEYONDTRUST, PLEASE CONTACT
 * BEYONDTRUST AT beyondtrust.com/contact
 */

/*
 * Module Name:
 *
 *        errno.c
 *
 * Abstract:
 *
 *        UNIX errno code table
 *
 * Authors: Brian Koropoff (bkoropoff@likewise.com)
 *
 */

#define S ERRNO_CODE

S ( 0, STATUS_SUCCESS)
S ( E2BIG, -1 )
S ( EACCES, STATUS_ACCESS_DENIED )
S ( EADDRINUSE, -1 )
S ( EADDRNOTAVAIL, -1 )
S ( EAFNOSUPPORT, -1 )
S ( EAGAIN, STATUS_RETRY )
S ( EALREADY, -1 )
S ( EBADF, STATUS_INVALID_HANDLE )
S ( EBADMSG, STATUS_INVALID_NETWORK_RESPONSE )
S ( EBUSY, -1 )
S ( ECANCELED, -1 )
S ( ECHILD, -1 )
S ( ECONNABORTED, STATUS_CONNECTION_ABORTED )
S ( ECONNREFUSED, STATUS_CONNECTION_REFUSED )
S ( ECONNRESET, STATUS_CONNECTION_RESET )
S ( EDEADLK, -1 )
S ( EDESTADDRREQ, -1 )
S ( EDOM, -1 )
S ( EDQUOT, STATUS_QUOTA_EXCEEDED )
S ( EEXIST, STATUS_OBJECT_NAME_COLLISION )
S ( EFAULT, STATUS_INVALID_BUFFER_SIZE )
S ( EFBIG, -1 )
S ( EHOSTUNREACH, -1 )
S ( EIDRM, -1 )
S ( EILSEQ, -1 )
S ( EINPROGRESS, -1 )
S ( EINTR, STATUS_MORE_PROCESSING_REQUIRED )
S ( EINVAL, STATUS_INVALID_PARAMETER )
S ( EIO, STATUS_UNEXPECTED_IO_ERROR )
S ( EISCONN, -1 )
S ( EISDIR, STATUS_FILE_IS_A_DIRECTORY )
S ( ELOOP, -1 )
S ( EMFILE, STATUS_TOO_MANY_OPENED_FILES )
S ( EMLINK, -1 )
S ( EMSGSIZE, -1 )
S ( EMULTIHOP, -1 )
S ( ENAMETOOLONG, STATUS_NAME_TOO_LONG )
S ( ENETDOWN, -1 )
S ( ENETRESET, -1 )
S ( ENETUNREACH, -1 )
S ( ENFILE, -1 )
S ( ENOBUFS, -1 )
#if defined(ENODATA)
S ( ENODATA, STATUS_RESOURCE_DATA_NOT_FOUND )
#endif
S ( ENODEV, -1 )
S ( ENOENT, STATUS_OBJECT_NAME_NOT_FOUND )
S ( ENOEXEC, -1 )
S ( ENOLCK, -1 )
S ( ENOLINK, -1 )
S ( ENOMEM, STATUS_INSUFFICIENT_RESOURCES )
S ( ENOMSG, -1 )
S ( ENOPROTOOPT, -1 )
S ( ENOSPC, STATUS_DISK_FULL )
#if defined(ENOSR)
S ( ENOSR, -1 )
#endif
#if defined(ENOSTR)
S ( ENOSTR, -1 )
#endif
S ( ENOSYS, STATUS_NOT_SUPPORTED )
S ( ENOTCONN, -1 )
S ( ENOTDIR, STATUS_NOT_A_DIRECTORY )
S ( ENOTEMPTY, STATUS_DIRECTORY_NOT_EMPTY )
S ( ENOTSOCK, -1 )
S ( ENOTSUP,  STATUS_NOT_SUPPORTED)
S ( ENOTTY, -1 )
S ( ENXIO, -1 )
S ( EOPNOTSUPP, -1 )
S ( EOVERFLOW, STATUS_BUFFER_TOO_SMALL )
S ( EPERM, STATUS_ACCESS_DENIED )
S ( EPIPE, -1 )
S ( EPROTO, -1 )
S ( EPROTONOSUPPORT, -1 )
S ( EPROTOTYPE, -1 )
S ( ERANGE, STATUS_BUFFER_TOO_SMALL )
S ( EROFS, STATUS_UNABLE_TO_LOCK_MEDIA )
S ( ESPIPE, -1 )
S ( ESRCH, STATUS_NOT_FOUND )
S ( ESTALE, -1 )
#if defined(ETIME)
S ( ETIME, -1 )
#endif
S ( ETIMEDOUT, STATUS_IO_TIMEOUT )
S ( ETXTBSY, -1 )
S ( EWOULDBLOCK, -1 )
S ( EXDEV, -1 )
#ifdef ENOATTR
S ( ENOATTR, STATUS_NO_EAS_ON_FILE )
#endif

#undef S
