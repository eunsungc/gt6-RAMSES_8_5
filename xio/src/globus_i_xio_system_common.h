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

#ifndef GLOBUS_I_XIO_SYSTEM_COMMON_INCLUDE
#define GLOBUS_I_XIO_SYSTEM_COMMON_INCLUDE

#include "globus_i_xio_config.h"
#include "globus_common.h"
#include "globus_xio_system.h"
#include "globus_xio_driver.h"

GlobusDebugDeclare(GLOBUS_XIO_SYSTEM);

#define GlobusXIOSystemDebugPrintf(level, message)                          \
    GlobusDebugPrintf(GLOBUS_XIO_SYSTEM, level, message)

#define GlobusXIOSystemDebugFwrite(level, buffer, size, count)              \
    GlobusDebugFwrite(GLOBUS_XIO_SYSTEM, level, buffer, size, count)

#define GlobusXIOSystemDebugEnter()                                         \
    GlobusXIOSystemDebugPrintf(                                             \
        GLOBUS_I_XIO_SYSTEM_DEBUG_TRACE,                                    \
        ("[%s] Entering\n", _xio_name))

#define GlobusXIOSystemDebugExit()                                          \
    GlobusXIOSystemDebugPrintf(                                             \
        GLOBUS_I_XIO_SYSTEM_DEBUG_TRACE,                                    \
        ("[%s] Exiting\n", _xio_name))

#define GlobusXIOSystemDebugExitWithError()                                 \
    GlobusXIOSystemDebugPrintf(                                             \
        GLOBUS_I_XIO_SYSTEM_DEBUG_TRACE,                                    \
        ("[%s] Exiting with error\n", _xio_name))

#define GlobusXIOSystemDebugEnterFD(fd)                                     \
    GlobusXIOSystemDebugPrintf(                                             \
        GLOBUS_I_XIO_SYSTEM_DEBUG_TRACE,                                    \
        ("[%s] fd=%lu, Entering\n", _xio_name, (unsigned long)(fd)))

#define GlobusXIOSystemDebugExitFD(fd)                                      \
    GlobusXIOSystemDebugPrintf(                                             \
        GLOBUS_I_XIO_SYSTEM_DEBUG_TRACE,                                    \
        ("[%s] fd=%lu, Exiting\n", _xio_name, (unsigned long)(fd)))

#define GlobusXIOSystemDebugExitWithErrorFD(fd)                             \
    GlobusXIOSystemDebugPrintf(                                             \
        GLOBUS_I_XIO_SYSTEM_DEBUG_TRACE,                                    \
        ("[%s] fd=%lu, Exiting with error\n", _xio_name, (unsigned long)(fd)))

#ifdef WIN32
#include <winsock2.h>
#define GlobusXIOSystemUpdateErrno() (errno = WSAGetLastError())
#else
#define GlobusXIOSystemUpdateErrno()
#endif

#define GlobusXIOSystemDebugRawBuffer(nbytes, buffer)                       \
    do                                                                      \
    {                                                                       \
        GlobusXIOSystemDebugPrintf(                                         \
            GLOBUS_I_XIO_SYSTEM_DEBUG_RAW,                                  \
            ("[%s] Begin RAW data ************\n", _xio_name));             \
        GlobusXIOSystemDebugFwrite(                                         \
            GLOBUS_I_XIO_SYSTEM_DEBUG_RAW, buffer, 1, nbytes);              \
        GlobusXIOSystemDebugPrintf(                                         \
            GLOBUS_I_XIO_SYSTEM_DEBUG_RAW,                                  \
            ("\n[%s] End RAW data ************\n", _xio_name));             \
    } while(0)

#define GlobusXIOSystemDebugRawIovec(nbytes, iovec)                         \
    do                                                                      \
    {                                                                       \
        if(GlobusDebugTrue(                                                 \
            GLOBUS_XIO_SYSTEM, GLOBUS_I_XIO_SYSTEM_DEBUG_RAW))              \
        {                                                                   \
            globus_size_t               _bytes = nbytes;                    \
            int                         _i = 0;                             \
                                                                            \
            while(_bytes > 0)                                               \
            {                                                               \
                globus_size_t           _len = (iovec)[_i].iov_len;         \
                                                                            \
                if(_bytes < _len)                                           \
                {                                                           \
                    _len = _bytes;                                          \
                }                                                           \
                _bytes -= _len;                                             \
                                                                            \
                GlobusDebugMyPrintf(                                        \
                    GLOBUS_XIO_SYSTEM,                                      \
                    ("[%s] Begin RAW data %i ************\n",               \
                    _xio_name, _i));                                        \
                GlobusDebugMyFwrite(                                        \
                    GLOBUS_XIO_SYSTEM,                                      \
                    (iovec)[_i].iov_base, 1, _len);                         \
                GlobusDebugMyPrintf(                                        \
                    GLOBUS_XIO_SYSTEM,                                      \
                    ("\n[%s] End RAW data %i ************\n",               \
                    _xio_name, _i));                                        \
                _i++;                                                       \
            }                                                               \
        }                                                                   \
    } while(0)

#define GlobusIXIOSystemAllocOperation(op_info)                             \
    do                                                                      \
    {                                                                       \
        globus_i_xio_system_op_info_t * _op_info;                           \
                                                                            \
        _op_info = (globus_i_xio_system_op_info_t *)                        \
            globus_memory_pop_node(&globus_i_xio_system_op_info_memory);    \
        if(_op_info)                                                        \
        {                                                                   \
            memset(_op_info, 0, sizeof(globus_i_xio_system_op_info_t));     \
        }                                                                   \
        (op_info) = _op_info;                                               \
    } while(0)

#define GlobusIXIOSystemFreeOperation(op_info)                              \
    (globus_memory_push_node(&globus_i_xio_system_op_info_memory, (op_info)))

#define GlobusIXIOSystemAllocIovec(count, iov)                              \
    do                                                                      \
    {                                                                       \
        int                             _count;                             \
                                                                            \
        _count = (count);                                                   \
                                                                            \
        if(_count < 10)                                                     \
        {                                                                   \
            (iov) = (struct iovec *)                                        \
                globus_memory_pop_node(&globus_i_xio_system_iov_memory);    \
        }                                                                   \
        else                                                                \
        {                                                                   \
            (iov) = (struct iovec *)                                        \
                globus_malloc(sizeof(struct iovec) * _count);               \
        }                                                                   \
    } while(0)

#define GlobusIXIOSystemFreeIovec(count, iovec)                             \
    do                                                                      \
    {                                                                       \
        int                             _count = (count);                   \
        if(_count < 10)                                                     \
        {                                                                   \
            globus_memory_push_node(                                        \
                &globus_i_xio_system_iov_memory, (iovec));                  \
        }                                                                   \
        else                                                                \
        {                                                                   \
            globus_free((iovec));                                           \
        }                                                                   \
    } while(0)

extern globus_memory_t                  globus_i_xio_system_op_info_memory;
extern globus_memory_t                  globus_i_xio_system_iov_memory;

enum globus_i_xio_system_error_levels
{
    GLOBUS_I_XIO_SYSTEM_DEBUG_TRACE     = 1,
    GLOBUS_I_XIO_SYSTEM_DEBUG_DATA      = 2,
    GLOBUS_I_XIO_SYSTEM_DEBUG_INFO      = 4,
    GLOBUS_I_XIO_SYSTEM_DEBUG_RAW       = 8
};

typedef enum
{
    GLOBUS_I_XIO_SYSTEM_OP_ACCEPT,
    GLOBUS_I_XIO_SYSTEM_OP_CONNECT,
    GLOBUS_I_XIO_SYSTEM_OP_READ,
    GLOBUS_I_XIO_SYSTEM_OP_WRITE
} globus_i_xio_system_op_type_t;

typedef enum
{
    /* initial state */
    GLOBUS_I_XIO_SYSTEM_OP_NEW,
    /* transition to this requires fdset lock */
    GLOBUS_I_XIO_SYSTEM_OP_PENDING,
    /* transition to this requires cancel lock */
    GLOBUS_I_XIO_SYSTEM_OP_COMPLETE,
    /* transition to this requires fdset and cancel lock */
    GLOBUS_I_XIO_SYSTEM_OP_CANCELED
} globus_i_xio_system_op_state_t;


#if defined(_WIN32)
struct msghdr
{
    void *                              msg_name;
    int                                 msg_namelen;
    struct iovec *                      msg_iov;
    int                                 msg_iovlen;
    void *                              msg_control;
    int                                 msg_controllen;
    int                                 msg_flags;
};
#endif

typedef struct
{
    /* common members */
    globus_i_xio_system_op_type_t       type;
    globus_i_xio_system_op_state_t      state;
    globus_xio_operation_t              op;
#ifndef WIN32
    struct globus_l_xio_system_s *      handle;
#else
    struct globus_l_xio_win32_socket_s *handle;
#endif
    globus_object_t *                   error;
    void *                              user_arg;
    /* used for reads/writes, 0 for others. here to simplify some things */
    globus_size_t                       nbytes;
    globus_size_t                       waitforbytes;
    globus_off_t                        offset;
    
    union
    {
        /* non data ops -- connect, accept */
        struct
        {
            globus_xio_system_callback_t callback;
            globus_xio_system_socket_t * out_fd;
        } non_data;

        /* data ops */
        struct
        {
            globus_xio_system_data_callback_t   callback;
            struct iovec *              start_iov;
            int                         start_iovc;

            struct iovec *              iov;
            int                         iovc;
            globus_sockaddr_t *         addr;
            int                         flags;
        } data;
    } sop;
} globus_i_xio_system_op_info_t;

globus_result_t
globus_i_xio_system_try_read(
    globus_xio_system_file_t            fd,
    void *                              buf,
    globus_size_t                       buflen,
    globus_size_t *                     nbytes);

globus_result_t
globus_i_xio_system_try_readv(
    globus_xio_system_file_t            fd,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    globus_size_t *                     nbytes);

globus_result_t
globus_i_xio_system_try_recv(
    globus_xio_system_socket_t          fd,
    void *                              buf,
    globus_size_t                       buflen,
    int                                 flags,
    globus_size_t *                     nbytes);

globus_result_t
globus_i_xio_system_try_recvfrom(
    globus_xio_system_socket_t          fd,
    void *                              buf,
    globus_size_t                       buflen,
    int                                 flags,
    globus_sockaddr_t *                 from,
    globus_size_t *                     nbytes);

globus_result_t
globus_i_xio_system_try_recvmsg(
    globus_xio_system_socket_t          fd,
    struct msghdr *                     msghdr,
    int                                 flags,
    globus_size_t *                     nbytes);

globus_result_t
globus_i_xio_system_try_write(
    globus_xio_system_file_t            fd,
    void *                              buf,
    globus_size_t                       buflen,
    globus_size_t *                     nbytes);

globus_result_t
globus_i_xio_system_try_writev(
    globus_xio_system_file_t            fd,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    globus_size_t *                     nbytes);

globus_result_t
globus_i_xio_system_try_send(
    globus_xio_system_socket_t          fd,
    void *                              buf,
    globus_size_t                       buflen,
    int                                 flags,
    globus_size_t *                     nbytes);

globus_result_t
globus_i_xio_system_try_sendto(
    globus_xio_system_socket_t          fd,
    void *                              buf,
    globus_size_t                       buflen,
    int                                 flags,
    const globus_sockaddr_t *           to,
    globus_size_t *                     nbytes);

globus_result_t
globus_i_xio_system_try_sendmsg(
    globus_xio_system_socket_t          fd,
    struct msghdr *                     msghdr,
    int                                 flags,
    globus_size_t *                     nbytes);

globus_result_t
globus_i_xio_system_file_try_read(
    globus_xio_system_file_t            handle,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    globus_size_t *                     nbytes);
    
globus_result_t
globus_i_xio_system_file_try_write(
    globus_xio_system_file_t            handle,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    globus_size_t *                     nbytes);
    
globus_result_t
globus_i_xio_system_socket_try_read(
    globus_xio_system_socket_t          handle,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    int                                 flags,
    globus_sockaddr_t *                 from,
    globus_size_t *                     nbytes);
    
globus_result_t
globus_i_xio_system_socket_try_write(
    globus_xio_system_socket_t          handle,
    const globus_xio_iovec_t *          iov,
    int                                 iovc,
    int                                 flags,
    globus_sockaddr_t *                 to,
    globus_size_t *                     nbytes);

int
globus_i_xio_system_common_activate(void);

int
globus_i_xio_system_common_deactivate(void);

#endif
