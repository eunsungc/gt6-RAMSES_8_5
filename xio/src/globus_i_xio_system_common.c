/*
 * Copyright 1999-2006 University of Chicago
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "globus_i_xio_system_common.h"
#include <unistd.h>
#include <limits.h>

#ifndef WIN32
#define GlobusLXIOSystemWouldBlock(err)                                     \
    (err == EAGAIN || err == EWOULDBLOCK)
#else
#define GlobusLXIOSystemWouldBlock(err)                                     \
    (err == EAGAIN || err == EWOULDBLOCK || err == WSAEWOULDBLOCK)
#endif

GlobusDebugDefine(GLOBUS_XIO_SYSTEM);

globus_memory_t                         globus_i_xio_system_op_info_memory;
globus_memory_t                         globus_i_xio_system_iov_memory;
static globus_bool_t                    globus_l_xio_system_memory_initialized;
static int                              globus_l_xio_iov_max;

int
globus_i_xio_system_common_activate(void)
{
    GlobusXIOName(globus_i_xio_system_common_activate);

    GlobusDebugInit(GLOBUS_XIO_SYSTEM, TRACE DATA INFO RAW);

    GlobusXIOSystemDebugEnter();

    /* I am going to leave this memory around after deactivation.  To safely
     * destroy them, I would need a lot more synchronization of kicked out
     * callbacks
     */
    if(globus_module_activate(GLOBUS_XIO_MODULE) != GLOBUS_SUCCESS)
    {
        goto error_activate;
    }

    if(!globus_l_xio_system_memory_initialized)
    {
        globus_l_xio_system_memory_initialized = GLOBUS_TRUE;
        globus_memory_init(
            &globus_i_xio_system_op_info_memory,
            sizeof(globus_i_xio_system_op_info_t),
            10);
        globus_memory_init(
            &globus_i_xio_system_iov_memory, sizeof(struct iovec) * 10, 10);
    }

    globus_l_xio_iov_max = -1;

#ifdef _SC_IOV_MAX
    if (globus_l_xio_iov_max == -1)
    {
        /* If this returns -1, the limit might be indefinite if errno is
         * unchanged, but that doesn't mean infinite. It's unclear what
         * one is to do in that case
         */
        globus_l_xio_iov_max = sysconf(_SC_IOV_MAX);
    }
#endif

#ifdef IOV_MAX
    if (globus_l_xio_iov_max == -1)
    {
        globus_l_xio_iov_max = IOV_MAX;
    }
#endif

#ifdef _XOPEN_IOV_MAX
    if (globus_l_xio_iov_max == -1)
    {
       globus_l_xio_iov_max = _XOPEN_IOV_MAX;
    }
#endif

    if (globus_l_xio_iov_max == -1)
    {
        globus_l_xio_iov_max = 16;
    }

    GlobusXIOSystemDebugExit();
    return GLOBUS_SUCCESS;

error_activate:
    GlobusXIOSystemDebugExitWithError();
    GlobusDebugDestroy(GLOBUS_XIO_SYSTEM);
    return GLOBUS_FAILURE;
}

int
globus_i_xio_system_common_deactivate(void)
{
    GlobusXIOName(globus_i_xio_system_common_deactivate);

    GlobusXIOSystemDebugEnter();

    globus_module_deactivate(GLOBUS_XIO_MODULE);

    GlobusXIOSystemDebugExit();
    GlobusDebugDestroy(GLOBUS_XIO_SYSTEM);
    return GLOBUS_SUCCESS;
}

#ifndef WIN32

globus_result_t
globus_i_xio_system_try_read(
    globus_xio_system_file_t            fd,
    void *                              buf,
    globus_size_t                       buflen,
    globus_size_t *                     nbytes)
{
    globus_ssize_t                      rc = 0;
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_try_read);

    GlobusXIOSystemDebugEnterFD(fd);

    /* calls to this with buflen == 0 are requesting select only */
    if(buflen)
    {
        do
        {
            rc = read(fd, buf, buflen);
            GlobusXIOSystemUpdateErrno();
        } while(rc < 0 && errno == EINTR);

        if(rc < 0)
        {
            if(GlobusLXIOSystemWouldBlock(errno))
            {
                rc = 0;
            }
            else
            {
                result = GlobusXIOErrorSystemError("read", errno);
                goto error_errno;
            }
        }
        else if(rc == 0) /* what about UDP? */
        {
            result = GlobusXIOErrorEOF();
            goto error_eof;
        }

        GlobusXIOSystemDebugPrintf(
            GLOBUS_I_XIO_SYSTEM_DEBUG_DATA,
            ("[%s] Read %d bytes (buflen = %d)\n", _xio_name, rc, buflen));

        GlobusXIOSystemDebugRawBuffer(rc, buf);
    }

    *nbytes = rc;

    GlobusXIOSystemDebugExitFD(fd);
    return GLOBUS_SUCCESS;

error_errno:
error_eof:
    *nbytes = 0;
    GlobusXIOSystemDebugExitWithErrorFD(fd);
    return result;
}

globus_result_t
globus_i_xio_system_try_readv(
    globus_xio_system_file_t            fd,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    globus_size_t *                     nbytes)
{
    globus_ssize_t                      rc;
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_try_readv);

    GlobusXIOSystemDebugEnterFD(fd);

    do
    {
#ifdef HAVE_READV
        rc = readv(fd, iov,
                (iovc > globus_l_xio_iov_max) ? globus_l_xio_iov_max : iovc);
#else
        rc = read(fd, iov[0].iov_base,iov[0].iov_len);
#endif
        GlobusXIOSystemUpdateErrno();
    } while(rc < 0 && errno == EINTR);

    if(rc < 0)
    {
        if(GlobusLXIOSystemWouldBlock(errno))
        {
            rc = 0;
        }
        else
        {
            result = GlobusXIOErrorSystemError("readv", errno);
            goto error_errno;
        }
    }
    else if(rc == 0)
    {
        result = GlobusXIOErrorEOF();
        goto error_eof;
    }

    *nbytes = rc;

    GlobusXIOSystemDebugPrintf(
        GLOBUS_I_XIO_SYSTEM_DEBUG_DATA,
        ("[%s] Read %d bytes\n", _xio_name, rc));

    GlobusXIOSystemDebugRawIovec(rc, iov);

    GlobusXIOSystemDebugExitFD(fd);
    return GLOBUS_SUCCESS;

error_errno:
error_eof:
    *nbytes = 0;
    GlobusXIOSystemDebugExitWithErrorFD(fd);
    return result;
}

#endif

globus_result_t
globus_i_xio_system_try_recv(
    globus_xio_system_socket_t          fd,
    void *                              buf,
    globus_size_t                       buflen,
    int                                 flags,
    globus_size_t *                     nbytes)
{
    globus_ssize_t                      rc = 0;
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_try_recv);

    GlobusXIOSystemDebugEnterFD(fd);

    if(buflen)
    {
        do
        {
            rc = recv(fd, buf, buflen, flags);
            GlobusXIOSystemUpdateErrno();
        } while(rc < 0 && errno == EINTR);

        if(rc < 0)
        {
            if(GlobusLXIOSystemWouldBlock(errno))
            {
                rc = 0;
            }
            else
            {
                result = GlobusXIOErrorSystemError("recv", errno);
                goto error_errno;
            }
        }
        else if(rc == 0)
        {
            result = GlobusXIOErrorEOF();
            goto error_eof;
        }

        GlobusXIOSystemDebugPrintf(
            GLOBUS_I_XIO_SYSTEM_DEBUG_DATA,
            ("[%s] Read %d bytes\n", _xio_name, rc));

        GlobusXIOSystemDebugRawBuffer(rc, buf);
    }

    *nbytes = rc;

    GlobusXIOSystemDebugExitFD(fd);
    return GLOBUS_SUCCESS;

error_errno:
error_eof:
    *nbytes = 0;
    GlobusXIOSystemDebugExitWithErrorFD(fd);
    return result;
}

globus_result_t
globus_i_xio_system_try_recvfrom(
    globus_xio_system_socket_t          fd,
    void *                              buf,
    globus_size_t                       buflen,
    int                                 flags,
    globus_sockaddr_t *                 from,
    globus_size_t *                     nbytes)
{
    globus_ssize_t                      rc = 0;
    globus_result_t                     result;
    globus_socklen_t                    len;
    GlobusXIOName(globus_i_xio_system_try_recvfrom);

    GlobusXIOSystemDebugEnterFD(fd);

    if(buflen)
    {
        do
        {
            len = sizeof(globus_sockaddr_t);
            rc = recvfrom(
                fd,
                buf,
                buflen,
                flags,
                (struct sockaddr *) from,
                &len);
            GlobusXIOSystemUpdateErrno();
        } while(rc < 0 && errno == EINTR);

        if(rc < 0)
        {
            if(GlobusLXIOSystemWouldBlock(errno))
            {
                rc = 0;
            }
            else
            {
                result = GlobusXIOErrorSystemError("recvfrom", errno);
                goto error_errno;
            }
        }
        else if(rc == 0)
        {
            result = GlobusXIOErrorEOF();
            goto error_eof;
        }

        GlobusXIOSystemDebugPrintf(
            GLOBUS_I_XIO_SYSTEM_DEBUG_DATA,
            ("[%s] Read %d bytes\n", _xio_name, rc));

        GlobusXIOSystemDebugRawBuffer(rc, buf);
    }

    *nbytes = rc;

    GlobusXIOSystemDebugExitFD(fd);
    return GLOBUS_SUCCESS;

error_errno:
error_eof:
    *nbytes = 0;
    GlobusXIOSystemDebugExitWithErrorFD(fd);
    return result;
}

globus_result_t
globus_i_xio_system_try_recvmsg(
    globus_xio_system_socket_t          fd,
    struct msghdr *                     msghdr,
    int                                 flags,
    globus_size_t *                     nbytes)
{
    globus_ssize_t                      rc;
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_try_recvmsg);

    GlobusXIOSystemDebugEnterFD(fd);

#ifdef WIN32
    {
        DWORD                           received = 0;
        DWORD                           oflags = flags;
        INT                             size = msghdr->msg_namelen;

        rc = WSARecvFrom(
            fd,
            (WSABUF *) msghdr->msg_iov,
            msghdr->msg_iovlen,
            &received,
            &oflags,
            msghdr->msg_name,
            &size,
            0,
            0);
        if(rc != 0)
        {
            GlobusXIOSystemUpdateErrno();
            rc = -1;
        }
        else
        {
            rc = received;
        }
    }
#elif defined(HAVE_RECVSMSG)
    {
        globus_size_t                   orig_iovc;

        orig_iovc = msghdr->msg_iovlen;
        msghdr->msg_iovlen = (orig_iovc > globus_l_xio_iov_max)
                ?  globus_l_xio_iov_max : orig_iovc;

        do
        {
            rc = recvmsg(fd, msghdr, flags);
            GlobusXIOSystemUpdateErrno();
        } while(rc < 0 && errno == EINTR);

        msghdr->msg_iovlen = orig_iovc;
    }
#else
    {
        /* XXX this is not an acceptable work around for udp sockets */
        do
        {
            if (msghdr->msg_name)
            {
                rc = recvfrom(
                        fd,
                        msghdr->msg_iov[0].iov_base,
                        msghdr->msg_iov[0].iov_len,
                        flags,
                        (struct sockaddr *) msghdr->msg_name,
                        &msghdr->msg_namelen);
            }
            else
            {
                rc = recv(
                        fd,
                        msghdr->msg_iov[0].iov_base,
                        msghdr->msg_iov[1].iov_len,
                        flags);
            }

            GlobusXIOSystemUpdateErrno();
        } while(rc < 0 && errno == EINTR);
    }
#endif

    if(rc < 0)
    {
        if(GlobusLXIOSystemWouldBlock(errno))
        {
            rc = 0;
        }
        else
        {
            result = GlobusXIOErrorSystemError("recvmsg", errno);
            goto error_errno;
        }
    }
    else if(rc == 0)
    {
        result = GlobusXIOErrorEOF();
        goto error_eof;
    }

    *nbytes = rc;

    GlobusXIOSystemDebugPrintf(
        GLOBUS_I_XIO_SYSTEM_DEBUG_DATA,
        ("[%s] Read %d bytes\n", _xio_name, rc));

    GlobusXIOSystemDebugRawIovec(rc, msghdr->msg_iov);

    GlobusXIOSystemDebugExitFD(fd);
    return GLOBUS_SUCCESS;

error_errno:
error_eof:
    *nbytes = 0;
    GlobusXIOSystemDebugExitWithErrorFD(fd);
    return result;
}

#ifndef WIN32

globus_result_t
globus_i_xio_system_try_write(
    globus_xio_system_file_t            fd,
    void *                              buf,
    globus_size_t                       buflen,
    globus_size_t *                     nbytes)
{
    globus_ssize_t                      rc = 0;
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_try_write);

    GlobusXIOSystemDebugEnterFD(fd);

    /* calls to this with buflen == 0 are requesting select only */
    if(buflen)
    {
        do
        {
            rc = write(fd, buf, buflen);
            GlobusXIOSystemUpdateErrno();
        } while(rc < 0 && errno == EINTR);

        if(rc < 0)
        {
            if(GlobusLXIOSystemWouldBlock(errno))
            {
                rc = 0;
            }
            else
            {
                result = GlobusXIOErrorSystemError("write", errno);
                goto error_errno;
            }
        }

        GlobusXIOSystemDebugPrintf(
            GLOBUS_I_XIO_SYSTEM_DEBUG_DATA,
            ("[%s] Wrote %d bytes\n", _xio_name, rc));

        GlobusXIOSystemDebugRawBuffer(rc, buf);
    }

    *nbytes = rc;

    GlobusXIOSystemDebugExitFD(fd);
    return GLOBUS_SUCCESS;

error_errno:
    *nbytes = 0;
    GlobusXIOSystemDebugExitWithErrorFD(fd);
    return result;
}

globus_result_t
globus_i_xio_system_try_writev(
    globus_xio_system_file_t            fd,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    globus_size_t *                     nbytes)
{
    globus_ssize_t                      rc;
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_try_writev);

    GlobusXIOSystemDebugEnterFD(fd);

    do
    {
#ifdef HAVE_WRITEV
        rc = writev(fd, iov, (iovc > globus_l_xio_iov_max)
                ? globus_l_xio_iov_max : iovc);
#else
        rc = write(fd, iov[0].iov_base, iov[0].iov_len);
#endif
        GlobusXIOSystemUpdateErrno();
    } while(rc < 0 && errno == EINTR);

    if(rc < 0)
    {
        if(GlobusLXIOSystemWouldBlock(errno))
        {
            rc = 0;
        }
        else
        {
            result = GlobusXIOErrorSystemError("writev", errno);
            goto error_errno;
        }
    }

    *nbytes = rc;

    GlobusXIOSystemDebugPrintf(
        GLOBUS_I_XIO_SYSTEM_DEBUG_DATA,
        ("[%s] Wrote %d bytes\n", _xio_name, rc));

    GlobusXIOSystemDebugRawIovec(rc, iov);

    GlobusXIOSystemDebugExitFD(fd);
    return GLOBUS_SUCCESS;

error_errno:
    *nbytes = 0;
    GlobusXIOSystemDebugExitWithErrorFD(fd);
    return result;
}

#endif

globus_result_t
globus_i_xio_system_try_send(
    globus_xio_system_socket_t          fd,
    void *                              buf,
    globus_size_t                       buflen,
    int                                 flags,
    globus_size_t *                     nbytes)
{
    globus_ssize_t                      rc = 0;
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_try_send);

    GlobusXIOSystemDebugEnterFD(fd);

    if(buflen)
    {
        do
        {
            rc = send(fd, buf, buflen, flags);
            GlobusXIOSystemUpdateErrno();
        } while(rc < 0 && errno == EINTR);

        if(rc < 0)
        {
            if(GlobusLXIOSystemWouldBlock(errno))
            {
                rc = 0;
            }
            else
            {
                result = GlobusXIOErrorSystemError("send", errno);
                goto error_errno;
            }
        }

        GlobusXIOSystemDebugPrintf(
            GLOBUS_I_XIO_SYSTEM_DEBUG_DATA,
            ("[%s] Wrote %d bytes\n", _xio_name, rc));

        GlobusXIOSystemDebugRawBuffer(rc, buf);
    }

    *nbytes = rc;

    GlobusXIOSystemDebugExitFD(fd);
    return GLOBUS_SUCCESS;

error_errno:
    *nbytes = 0;
    GlobusXIOSystemDebugExitWithErrorFD(fd);
    return result;
}

globus_result_t
globus_i_xio_system_try_sendto(
    globus_xio_system_socket_t          fd,
    void *                              buf,
    globus_size_t                       buflen,
    int                                 flags,
    const globus_sockaddr_t *           to,
    globus_size_t *                     nbytes)
{
    globus_ssize_t                      rc = 0;
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_try_sendto);

    GlobusXIOSystemDebugEnterFD(fd);

    if(buflen)
    {
        do
        {
            rc = sendto(
                fd,
                buf,
                buflen,
                flags,
                (struct sockaddr *) to,
                GlobusLibcSockaddrLen(to));
            GlobusXIOSystemUpdateErrno();
        } while(rc < 0 && errno == EINTR);

        if(rc < 0)
        {
            if(GlobusLXIOSystemWouldBlock(errno))
            {
                rc = 0;
            }
            else
            {
                result = GlobusXIOErrorSystemError("sendto", errno);
                goto error_errno;
            }
        }

        GlobusXIOSystemDebugPrintf(
            GLOBUS_I_XIO_SYSTEM_DEBUG_DATA,
            ("[%s] Wrote %d bytes\n", _xio_name, rc));

        GlobusXIOSystemDebugRawBuffer(rc, buf);
    }

    *nbytes = rc;

    GlobusXIOSystemDebugExitFD(fd);
    return GLOBUS_SUCCESS;

error_errno:
    *nbytes = 0;
    GlobusXIOSystemDebugExitWithErrorFD(fd);
    return result;
}

globus_result_t
globus_i_xio_system_try_sendmsg(
    globus_xio_system_socket_t          fd,
    struct msghdr *                     msghdr,
    int                                 flags,
    globus_size_t *                     nbytes)
{
    globus_ssize_t                      rc;
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_try_sendmsg);

    GlobusXIOSystemDebugEnterFD(fd);

#ifdef WIN32
    {
        DWORD                           sent = 0;

        rc = WSASendTo(
            fd,
            (WSABUF *) msghdr->msg_iov,
            msghdr->msg_iovlen,
            &sent,
            flags,
            msghdr->msg_name,
            msghdr->msg_namelen,
            0,
            0);
        if(rc != 0)
        {
            GlobusXIOSystemUpdateErrno();
            rc = -1;
        }
        else
        {
            rc = sent;
        }
    }
#elif defined(HAVE_SENDMSG)
    {
        globus_size_t                   orig_iovc;

        orig_iovc = msghdr->msg_iovlen;
        msghdr->msg_iovlen = (orig_iovc > globus_l_xio_iov_max)
                ? globus_l_xio_iov_max : orig_iovc;

        do
        {
            rc = sendmsg(fd, msghdr, flags);
            GlobusXIOSystemUpdateErrno();
        } while(rc < 0 && errno == EINTR);

        msghdr->msg_iovlen = orig_iovc;
    }
#else
    {
        /* XXX this is not an acceptable work around for udp sockets */
        do
        {
            if (msghdr->msg_name)
            {
                rc = sendto(
                        fd,
                        msghdr->msg_iov[0].iov_base,
                        msghdr->msg_iov[0].iov_len,
                        flags,
                        (struct sockaddr *) msghdr->msg_name,
                        msghdr->msg_namelen);
            }
            else
            {
                rc = send(
                        fd,
                        msghdr->msg_iov[0].iov_base,
                        msghdr->msg_iov[0].iov_len,
                        flags);
            }
            GlobusXIOSystemUpdateErrno();
        } while (rc < 0 && errno == EINTR);
    }
#endif

    if(rc < 0)
    {
        if(GlobusLXIOSystemWouldBlock(errno))
        {
            rc = 0;
        }
        else
        {
            result = GlobusXIOErrorSystemError("sendmsg", errno);
            goto error_errno;
        }
    }

    *nbytes = rc;

    GlobusXIOSystemDebugPrintf(
        GLOBUS_I_XIO_SYSTEM_DEBUG_DATA,
        ("[%s] Wrote %d bytes\n", _xio_name, rc));

    GlobusXIOSystemDebugRawIovec(rc, msghdr->msg_iov);

    GlobusXIOSystemDebugExitFD(fd);
    return GLOBUS_SUCCESS;

error_errno:
    *nbytes = 0;
    GlobusXIOSystemDebugExitWithErrorFD(fd);
    return result;
}

#ifndef WIN32

globus_result_t
globus_i_xio_system_file_try_read(
    globus_xio_system_file_t            handle,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    globus_size_t *                     nbytes)
{
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_file_try_read);

    GlobusXIOSystemDebugEnter();

    if(iovc == 1)
    {
        result = globus_i_xio_system_try_read(
            handle, iov->iov_base, iov->iov_len, nbytes);
    }
    else
    {
        result = globus_i_xio_system_try_readv(handle, iov, iovc, nbytes);
    }

    GlobusXIOSystemDebugExit();
    return result;
}

globus_result_t
globus_i_xio_system_file_try_write(
    globus_xio_system_file_t            handle,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    globus_size_t *                     nbytes)
{
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_file_try_write);

    GlobusXIOSystemDebugEnter();

    if(iovc == 1)
    {
        result = globus_i_xio_system_try_write(
            handle, iov->iov_base, iov->iov_len, nbytes);
    }
    else
    {
        result = globus_i_xio_system_try_writev(handle, iov, iovc, nbytes);
    }

    GlobusXIOSystemDebugExit();
    return result;
}

#endif

globus_result_t
globus_i_xio_system_socket_try_read(
    globus_xio_system_socket_t          handle,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    int                                 flags,
    globus_sockaddr_t *                 from,
    globus_size_t *                     nbytes)
{
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_socket_try_read);

    GlobusXIOSystemDebugEnter();

#if !defined(WIN32) && !defined(TARGET_ARCH_NETOS)
    /* posix can use readv for sockets */
    if(!flags && !from && iovc > 1)
    {
        result = globus_i_xio_system_try_readv(handle, iov, iovc, nbytes);
    }
    else
#endif
    if(iovc == 1)
    {
        if(from)
        {
            result = globus_i_xio_system_try_recvfrom(
                handle, iov->iov_base, iov->iov_len, flags, from, nbytes);
        }
        else
        {
            result = globus_i_xio_system_try_recv(
                handle, iov->iov_base, iov->iov_len, flags, nbytes);
        }
    }
    else
    {
        struct msghdr                   msghdr;

        memset(&msghdr, 0, sizeof(msghdr));
        msghdr.msg_iov = (struct iovec *) iov;
        msghdr.msg_iovlen = iovc;
        if(from)
        {
            msghdr.msg_name = from;
            msghdr.msg_namelen = sizeof(globus_sockaddr_t);
        }

        result = globus_i_xio_system_try_recvmsg(
            handle, &msghdr, flags, nbytes);
    }

    GlobusXIOSystemDebugExit();
    return result;
}

globus_result_t
globus_i_xio_system_socket_try_write(
    globus_xio_system_socket_t          handle,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    int                                 flags,
    globus_sockaddr_t *                 to,
    globus_size_t *                     nbytes)
{
    globus_result_t                     result;
    GlobusXIOName(globus_i_xio_system_socket_try_write);

    GlobusXIOSystemDebugEnter();

#if !defined(WIN32) && !defined(TARGET_ARCH_NETOS)
    /* posix can use writev for sockets */
    if(!flags && !to && iovc > 1)
    {
        result = globus_i_xio_system_try_writev(handle, iov, iovc, nbytes);
    }
    else
#endif
    if(iovc == 1)
    {
        if(to)
        {
            result = globus_i_xio_system_try_sendto(
                handle, iov->iov_base, iov->iov_len, flags, to, nbytes);
        }
        else
        {
            result = globus_i_xio_system_try_send(
                handle, iov->iov_base, iov->iov_len, flags, nbytes);
        }
    }
    else
    {
        struct msghdr                   msghdr;

        memset(&msghdr, 0, sizeof(msghdr));
        msghdr.msg_iov = (struct iovec *) iov;
        msghdr.msg_iovlen = iovc;
        if(to)
        {
            msghdr.msg_name = (struct sockaddr *) to;
            msghdr.msg_namelen = GlobusLibcSockaddrLen(to);
        }

        result = globus_i_xio_system_try_sendmsg(
            handle, &msghdr, flags, nbytes);
    }

    GlobusXIOSystemDebugExit();
    return result;
}
