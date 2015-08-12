#if !defined(GFS_GFORK_PLUGIN_H)
#define GFS_GFORK_PLUGIN_H 1

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

#include "globus_xio.h"
#include "globus_xio_tcp_driver.h"
#include "globus_xio_gsi.h"
#include "globus_gfork.h"

#define GFSGforkError(error_msg, _type)                                     \
    globus_error_put(                                                       \
        globus_error_construct_error(                                       \
            NULL,                                                           \
            NULL,                                                           \
            _type,                                                          \
            __FILE__,                                                       \
            _gfs_gfork_func_name,                                           \
            __LINE__,                                                       \
            "%s",                                                           \
            (error_msg)))

#ifdef __GNUC__
#define GFSGForkFuncName(func) static const char * _gfs_gfork_func_name __attribute__((__unused__)) = #func
#else
#define GFSGForkFuncName(func) static const char * _gfs_gfork_func_name = #func
#endif


#define GF_VERSION                  'a'

/* use this to mark buffer in fifo as bad.  we know it was not 
   sent over the wire because we would not have alloed it in the first
   place */
#define GF_VERSION_TIMEOUT         'F'
#define GF_REGISTRATION_TIMEOUT    600

/* header for all */
#define GF_VERSION_LEN             1
#define GF_HEADER_RESERVE_LEN      5
#define GF_MSG_TYPE_LEN            1

#define GF_VERSION_NDX             0
#define GF_HEADER_RESERVE_NDX      (GF_VERSION_NDX+GF_VERSION_LEN)
#define GF_MSG_TYPE_NDX            (GF_HEADER_RESERVE_NDX+GF_HEADER_RESERVE_LEN)


/* dyn be messaging */
#define GF_DYN_AT_ONCE_LEN         (sizeof(uint32_t))
#define GF_DYN_TOTAL_LEN           (sizeof(uint32_t))
#define GF_DYN_ENTRY_COUNT_LEN     (sizeof(uint32_t))
#define GF_DYN_COOKIE_LEN          32
#define GF_DYN_REPO_LEN            108
#define GF_DYN_CS_LEN              108


#define GF_DYN_AT_ONCE_NDX         (GF_MSG_TYPE_NDX+GF_MSG_TYPE_LEN)
#define GF_DYN_TOTAL_NDX           (GF_DYN_AT_ONCE_NDX+GF_DYN_AT_ONCE_LEN)
#define GF_DYN_ENTRY_COUNT_NDX     (GF_DYN_TOTAL_NDX+GF_DYN_TOTAL_LEN)
#define GF_DYN_COOKIE_NDX      (GF_DYN_ENTRY_COUNT_NDX+GF_DYN_ENTRY_COUNT_LEN)
#define GF_DYN_REPO_NDX            (GF_DYN_COOKIE_NDX+GF_DYN_COOKIE_NDX)
#define GF_DYN_CS_NDX              (GF_DYN_REPO_NDX+GF_DYN_REPO_LEN)

#define GF_DYN_PACKET_LEN          (GF_DYN_CS_LEN+GF_DYN_CS_NDX)

/* mem messaging */
#define GF_MEM_LIMIT_NDX            (GF_MSG_TYPE_NDX+GF_MSG_TYPE_LEN)
#define GF_MEM_LIMIT_LEN            (sizeof(uint32_t))

#define GF_MEM_MSG_LEN              (GF_MEM_LIMIT_NDX+GF_MEM_LIMIT_LEN)

/* kill msgin*/
#define GF_KILL_STRING_NDX          (GF_MSG_TYPE_NDX+GF_MSG_TYPE_LEN)
#define GF_KILL_STRING_LEN          128

#define GF_KILL_MSG_LEN             (GF_KILL_STRING_NDX+GF_KILL_STRING_LEN)

/* ready message */
#define GF_READY_MSG_LEN            (GF_MSG_TYPE_NDX+GF_MSG_TYPE_LEN)

/* release message */
#define GF_RELEASE_COUNT_NDX        (GF_MSG_TYPE_NDX+GF_MSG_TYPE_LEN)
#define GF_RELEASE_COUNT_LEN        (sizeof(uint32_t))

#define GF_RELEASE_MSG_LEN          (GF_RELEASE_COUNT_NDX+GF_RELEASE_COUNT_LEN)

typedef enum gfs_gfork_msg_type_e
{
    GFS_GFORK_MSG_TYPE_DYNBE = 1,
    GFS_GFORK_MSG_TYPE_KILL,
    GFS_GFORK_MSG_TYPE_MEM,
    GFS_GFORK_MSG_TYPE_READY,
    GFS_GFORK_MSG_TYPE_ACK,
    GFS_GFORK_MSG_TYPE_NACK,
    GFS_GFORK_MSG_TYPE_CC,
    GFS_GFORK_MSG_TYPE_RELEASE,
    GFS_GFORK_MSG_TYPE_REMOVE_DYNBE
} gfs_gfork_msg_type_t;


typedef enum gfs_gfork_error_e
{
    GFS_GFORK_ERROR_PARAMETER = 1
} gfs_gfork_error_t;

#endif
