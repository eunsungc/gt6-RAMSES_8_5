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
#include "globus_usage.h"
#include "version.h"

#ifndef TARGET_ARCH_ARM
#include "globus_xio_udp_driver.h"
#include <stdarg.h>

static globus_xio_stack_t               globus_l_usage_stats_stack;
static globus_xio_driver_t              globus_l_usage_stats_udp_driver;

#define GLOBUS_L_USAGE_STATS_DEFAULT_TARGETS "usage-stats.globus.org:4810"
#define GLOBUS_L_USAGE_STATS_TIMESTAMP_OFFSET 20

enum
{
    GLOBUS_L_USAGE_STATS_DEBUG_MESSAGES = 0x01
};

GlobusDebugDefine(GLOBUS_USAGE);

#define GlobusUsageStatsDebugPrintf(LEVEL, MESSAGE) \
    GlobusDebugPrintf(GLOBUS_USAGE, LEVEL, MESSAGE)

#define GlobusUsageStatsConvertToPrintable(CHARVALUE)           \
    (((((unsigned int)CHARVALUE) > 31) &&                       \
      ((((unsigned int)CHARVALUE) < 127)) ? CHARVALUE : '.'))

#define GlobusUsageStatsDebugDump(LEVEL, DATA, LENGTH)                  \
    {                                                                   \
        int i = 0;                                                      \
        for(; i < LENGTH; ++i)                                          \
        {                                                               \
            char cv = GlobusUsageStatsConvertToPrintable(DATA[i]);      \
            GlobusUsageStatsDebugPrintf(                                \
                LEVEL, ("%c", cv));                                     \
        }                                                               \
    }
#endif

#define GlobusUsageTr(_msg, _len, _from, _to)     \
do                                                \
{                                                 \
    char *                  _ptr;                 \
    char *                  _stop;                \
    _stop = (char *) _msg + _len;                 \
    _ptr = strchr((char * ) _msg, _from);         \
    while(_ptr != NULL && _ptr <  _stop)          \
    {                                             \
        *_ptr = _to;                              \
        _ptr = strchr((char * ) _ptr, _from);     \
    }                                             \
} while(0)

#define PACKET_SIZE 1472

static int
globus_l_usage_stats_activate();

static int
globus_l_usage_stats_deactivate();

globus_module_descriptor_t
globus_i_usage_stats_module =
{
    "globus_usage_stats_module",
    globus_l_usage_stats_activate,
    globus_l_usage_stats_deactivate,
    NULL, NULL,
    &local_version
};

typedef struct globus_usage_stats_handle_s
{
    uint16_t                            code;
    uint16_t                            version;
    globus_list_t *                     targets;
    globus_xio_handle_t                 xio_handle;
    globus_list_t *                     xio_desc_list;
    const char *                        optout;
    globus_bool_t                       inuse;
    globus_mutex_t                      mutex;
    size_t                              header_length;
    size_t                              data_length;
    unsigned char                       data[PACKET_SIZE];
} globus_i_usage_stats_handle_t;

#ifndef TARGET_ARCH_ARM

static
int
globus_l_usage_stats_split_targets(
    const char *                        targets_string,
    globus_list_t **                    targets)
{
    char *                              tmpstr;
    char *                              target;
    char *                              ptr;

    if(targets_string == NULL)
    {
        return -1;
    }
    
    tmpstr = globus_libc_strdup(targets_string);

    target = tmpstr;
    while((ptr = strchr(target, ',')) != NULL ||
            (ptr = strchr(target, ' ')) != NULL)
    {
        *ptr = '\0';
        globus_list_insert(targets, globus_libc_strdup(target)); 
        target = ptr + 1;
    }
    if(ptr == NULL)
    {
        globus_list_insert(targets, globus_libc_strdup(target)); 
    }               
        
    globus_free(tmpstr);             

    return 0;
}

#endif

static int
globus_l_usage_stats_activate()
{
    globus_result_t                     result = GLOBUS_SUCCESS;
    int                                 rc = 0;

#ifndef TARGET_ARCH_ARM
    globus_l_usage_stats_stack = NULL;
    globus_l_usage_stats_udp_driver = NULL;

    rc = globus_module_activate(GLOBUS_XIO_MODULE);
    if(rc != GLOBUS_SUCCESS)
    {
        return rc;
    }

    GlobusDebugInit(GLOBUS_USAGE, MESSAGES);

    if((result = globus_xio_stack_init(
            &globus_l_usage_stats_stack, NULL)) != GLOBUS_SUCCESS)
    {
        return result;
    }

    if((result = globus_xio_driver_load(
            "udp", &globus_l_usage_stats_udp_driver)) != GLOBUS_SUCCESS)
    {    
        return result;
    }

    if((result = globus_xio_stack_push_driver(
            globus_l_usage_stats_stack, globus_l_usage_stats_udp_driver)) 
       != GLOBUS_SUCCESS)
    {
        return result;
    }

    return 0;
#else
    return 1;
#endif
}

static
int
globus_l_usage_stats_deactivate()
{
#ifndef TARGET_ARCH_ARM
    if(globus_l_usage_stats_stack)
    {
        globus_xio_stack_destroy(globus_l_usage_stats_stack);
    }

    return globus_module_deactivate(GLOBUS_XIO_MODULE);
#else
    return 1;
#endif
}

globus_result_t
globus_usage_stats_handle_init(
    globus_usage_stats_handle_t *       handle,
    uint16_t                            code,
    uint16_t                            version,
    const char *                        targets)
{
    globus_result_t                     result = GLOBUS_SUCCESS;
#ifndef TARGET_ARCH_ARM
    globus_i_usage_stats_handle_t *     new_handle;
    char *                              targets_env;
    globus_list_t *                     targets_list;
    char *                              contact;
    globus_sockaddr_t                   addr;
    int                                 host[16];
    int                                 count;
    char                                hostname[255];
    int                                 rc = 0;
    int                                 i;

    new_handle = globus_calloc(1, sizeof(globus_i_usage_stats_handle_t));
    if(!new_handle)
    {
        return globus_error_put(
            globus_error_construct_error(
                GLOBUS_USAGE_MODULE,
                NULL,
                GLOBUS_USAGE_STATS_ERROR_TYPE_OOM,
                __FILE__,
                _globus_func_name,
                __LINE__,
                "Out of memory"));
    }

    new_handle->optout = getenv("GLOBUS_USAGE_OPTOUT");
    if(new_handle->optout)
    {
        *handle = new_handle;
        return GLOBUS_SUCCESS;
    }

    globus_mutex_init(&new_handle->mutex, NULL);

    new_handle->inuse = GLOBUS_FALSE;
    
    new_handle->code = htons(code);
    new_handle->version = htons(version);

    memset(new_handle->data, 0, PACKET_SIZE);

    memcpy(new_handle->data + new_handle->data_length, 
           (void *)&new_handle->code, 2);
    new_handle->data_length += 2;

    memcpy(new_handle->data + new_handle->data_length, 
           (void *)&new_handle->version, 2);
    new_handle->data_length += 2;

    rc = globus_libc_gethostaddr(&addr);
    if(rc != 0)
    {
        return globus_error_put(
            globus_error_construct_error(
                GLOBUS_USAGE_MODULE,
                NULL,
                GLOBUS_USAGE_STATS_ERROR_TYPE_UNKNOWN_HOSTNAME,
                __FILE__,
                _globus_func_name,
                __LINE__,
                "Unable to get hostaddr."));
    }
    
    result = globus_libc_addr_to_contact_string(
        &addr, GLOBUS_LIBC_ADDR_NUMERIC, &contact);
    if(result != GLOBUS_SUCCESS)
    {
        return result;
    }

    result = globus_libc_contact_string_to_ints(
        contact, host, &count, NULL);
    if(result != GLOBUS_SUCCESS)
    {
        return result;
    }

    globus_libc_free(contact);

    if(count == 4)
    {
        memset(new_handle->data + new_handle->data_length, 0, 12);
        new_handle->data_length += 12;
    }

    for (i = 0; i < count; i++)
    {
        new_handle->data[new_handle->data_length++] = (unsigned char) host[i];
    }

    /* timestamp will go here */
    new_handle->data_length += 4;

    if(globus_libc_gethostname(hostname, 255) == 0)
    {
        new_handle->data_length += 
            sprintf((char *) new_handle->data + new_handle->data_length,
                "HOSTNAME=%s", hostname);
    }
    new_handle->header_length = new_handle->data_length;
    
    if(targets)
    {
        globus_l_usage_stats_split_targets(targets, &new_handle->targets);
    }
    else if((targets_env = getenv("GLOBUS_USAGE_TARGETS")) 
            != NULL)
    {
        globus_l_usage_stats_split_targets(
            targets_env, &new_handle->targets);
    }
    else
    {
        globus_l_usage_stats_split_targets(
            GLOBUS_L_USAGE_STATS_DEFAULT_TARGETS, 
            &new_handle->targets);
    }


    result = globus_xio_handle_create(
        &new_handle->xio_handle,
        globus_l_usage_stats_stack);
    if(result != GLOBUS_SUCCESS)
    {
        return result;
    }

    result = globus_xio_open(
        new_handle->xio_handle,
        NULL, NULL);
    if(result != GLOBUS_SUCCESS)
    {
        return result;
    }

    targets_list = new_handle->targets;
    while(targets_list)
    {
        globus_xio_data_descriptor_t *  dd;
        dd = (globus_xio_data_descriptor_t *) globus_malloc(
            sizeof(globus_xio_data_descriptor_t));
            
        result = globus_xio_data_descriptor_init(
            dd,
            new_handle->xio_handle);
        if(result != GLOBUS_SUCCESS)
        {
            return result;
        }

        result = globus_xio_data_descriptor_cntl(
            *dd,
            globus_l_usage_stats_udp_driver,
            GLOBUS_XIO_UDP_SET_CONTACT,
            (char *)globus_list_first(targets_list));
        if(result != GLOBUS_SUCCESS)
        {
            goto exit;
        }
        
        globus_list_insert(&new_handle->xio_desc_list, dd);
        
        targets_list = globus_list_rest(targets_list);
    }
    
    *handle = new_handle;

    return GLOBUS_SUCCESS;

exit:
#endif
    return result;
}

void
globus_usage_stats_handle_destroy(
    globus_usage_stats_handle_t         vhandle)
{
#ifndef TARGET_ARCH_ARM
    globus_i_usage_stats_handle_t *     handle =
    (globus_i_usage_stats_handle_t *) vhandle;

    if(handle)
    {
        if(handle->targets)
        {
            globus_list_destroy_all(handle->targets, globus_libc_free);
        }
    
        if(handle->xio_desc_list)
        {
            globus_xio_data_descriptor_t *  dd;
            
            while (!globus_list_empty (handle->xio_desc_list)) 
            {
                if((dd = globus_list_remove(
                    &handle->xio_desc_list, handle->xio_desc_list)) != NULL)
                {
                    globus_xio_data_descriptor_destroy(*dd);
                    globus_free(dd);
                }
            }
        }
        if(handle->xio_handle)
        {
            globus_xio_close(handle->xio_handle, NULL);
        }
        
        globus_mutex_destroy(&handle->mutex);

        globus_free(handle);
    }
#endif
}

globus_result_t
globus_l_usage_stats_write_packet(
    globus_usage_stats_handle_t         handle)
{
    globus_result_t                     result = GLOBUS_SUCCESS;
#ifndef TARGET_ARCH_ARM
    globus_result_t                     save_result = GLOBUS_SUCCESS;
    globus_list_t *                     targets_list;
    globus_list_t *                     server_list;
    globus_abstime_t                    stamp;
    uint32_t                            nstamp;
    globus_size_t                       written;


    GlobusTimeAbstimeGetCurrent(stamp);
    nstamp = htonl(stamp.tv_sec);
    memcpy(handle->data + GLOBUS_L_USAGE_STATS_TIMESTAMP_OFFSET, 
           (void *)&nstamp, 4);
    targets_list = handle->xio_desc_list;
    server_list = handle->targets;
    while(targets_list)
    {
        GlobusUsageStatsDebugPrintf(
            GLOBUS_L_USAGE_STATS_DEBUG_MESSAGES,
            ("\n==========SENDING USAGE INFO: %s==(length: %d)===\n",
             (char *)globus_list_first(server_list), handle->data_length));
        GlobusUsageStatsDebugDump(
            GLOBUS_L_USAGE_STATS_DEBUG_MESSAGES,
            handle->data,
            handle->data_length);
        GlobusUsageStatsDebugPrintf(
            GLOBUS_L_USAGE_STATS_DEBUG_MESSAGES,
            ("\n=========================================================\n"));

        result = globus_xio_write(
            handle->xio_handle,
            handle->data,
            handle->data_length,
            0,
            &written,
            *(globus_xio_data_descriptor_t *) 
                globus_list_first(targets_list));
        if(result != GLOBUS_SUCCESS)
        {
            save_result = result;
        }

        targets_list = globus_list_rest(targets_list);
        server_list = globus_list_rest(server_list);
    }

    result = save_result;

#endif
    return result;
}   

globus_result_t
globus_usage_stats_send_array(
    globus_usage_stats_handle_t         handle,
    int                                 param_count,
    char **                             key_array,
    char **                             value_array)
{
    globus_result_t                     result = GLOBUS_SUCCESS;
#ifndef TARGET_ARCH_ARM
    int                                 i = 0;

    if(handle == NULL)
    {
        return globus_error_put(
            globus_error_construct_error(
                GLOBUS_USAGE_MODULE,
                NULL,
                GLOBUS_USAGE_STATS_ERROR_TYPE_OOM,
                __FILE__,
                _globus_func_name,
                __LINE__,
                "Handle is NULL."));
    }
                
    if(handle->optout)
    {
        return result;
    }

    globus_mutex_lock(&handle->mutex);    
    {
        if(handle->inuse)
        {
            globus_mutex_unlock(&handle->mutex);   
            return globus_error_put(
                globus_error_construct_error(
                    GLOBUS_USAGE_MODULE,
                    NULL,
                    GLOBUS_USAGE_STATS_ERROR_TYPE_OOM,
                    __FILE__,
                    _globus_func_name,
                    __LINE__,
                    "Handle is busy."));
        }
        else
        {
            handle->inuse = GLOBUS_TRUE;
        }
    }
    globus_mutex_unlock(&handle->mutex);
    
    handle->data_length = handle->header_length;
    
    if(param_count > 0)
    {
        memcpy(handle->data + handle->data_length, " ", 1);
        handle->data_length += 1;

        for(i = 0; i < param_count; ++i)
        {
            const char *                key;
            const char *                value;
            size_t                      length; 
            size_t                      val_len;
            const unsigned char *       val_start;
            key = key_array[i];
            value = value_array[i];
            
            val_len = strlen(value);
            length = strlen(key) + val_len;

            if(strchr(value, ' '))
            {
                if((PACKET_SIZE - handle->data_length) < (length + 5))
                {
                    return globus_error_put(
                        globus_error_construct_error(
                            GLOBUS_USAGE_MODULE,
                            NULL,
                            GLOBUS_USAGE_STATS_ERROR_TYPE_TOO_BIG,
                            __FILE__,
                            _globus_func_name,
                            __LINE__,
                            "Parameters don't fit into one packet"));
                }
                handle->data_length += sprintf(
                    (char *) handle->data + handle->data_length,
                    "%s=\"%s\" ", key, value);
                val_start = handle->data + handle->data_length - val_len - 2;
                GlobusUsageTr(val_start, val_len, '"', '\'');
                GlobusUsageTr(val_start, val_len, '\n', ' ');
            }
            else
            {
                if((PACKET_SIZE - handle->data_length) < (length + 3))
                {
                    return globus_error_put(
                        globus_error_construct_error(
                            GLOBUS_USAGE_MODULE,
                            NULL,
                            GLOBUS_USAGE_STATS_ERROR_TYPE_TOO_BIG,
                            __FILE__,
                            _globus_func_name,
                            __LINE__,
                            "Parameters don't fit into one packet"));
                }
                handle->data_length += sprintf(
                    (char *) handle->data + handle->data_length,
                    "%s=%s ", key, value);
                val_start = handle->data + handle->data_length - val_len - 1;
                GlobusUsageTr(val_start, val_len, '"', '\'');
                GlobusUsageTr(val_start, val_len, '\n', ' ');
            }
        }
    }
    
    globus_l_usage_stats_write_packet(handle);
    
    globus_mutex_lock(&handle->mutex);    
    {
        handle->inuse = GLOBUS_FALSE;
    }
    globus_mutex_unlock(&handle->mutex);


#endif
    return result;
}   

globus_result_t
globus_usage_stats_send(
    globus_usage_stats_handle_t         handle,
    int                                 param_count,
    ...)
{
    globus_result_t                     result = GLOBUS_SUCCESS;
#ifndef TARGET_ARCH_ARM
    va_list                             ap;

    va_start(ap, param_count);

    result = globus_usage_stats_vsend(
        handle, param_count, ap);
    
    va_end(ap);
#endif

    return result;
}   

globus_result_t
globus_usage_stats_vsend(
    globus_usage_stats_handle_t         handle,
    int                                 param_count,
    va_list                             ap)
{
    globus_result_t                     result = GLOBUS_SUCCESS;
#ifndef TARGET_ARCH_ARM
    int                                 i = 0;

    if(handle == NULL)
    {
        return globus_error_put(
            globus_error_construct_error(
                GLOBUS_USAGE_MODULE,
                NULL,
                GLOBUS_USAGE_STATS_ERROR_TYPE_OOM,
                __FILE__,
                _globus_func_name,
                __LINE__,
                "Handle is NULL."));
    }
                
    if(handle->optout)
    {
        return result;
    }

    globus_mutex_lock(&handle->mutex);    
    {
        if(handle->inuse)
        {
            globus_mutex_unlock(&handle->mutex);   
            return globus_error_put(
                globus_error_construct_error(
                    GLOBUS_USAGE_MODULE,
                    NULL,
                    GLOBUS_USAGE_STATS_ERROR_TYPE_OOM,
                    __FILE__,
                    _globus_func_name,
                    __LINE__,
                    "Handle is busy."));
        }
        else
        {
            handle->inuse = GLOBUS_TRUE;
        }
    }
    globus_mutex_unlock(&handle->mutex);
    
    handle->data_length = handle->header_length;
    
    if(param_count > 0)
    {
        memcpy(handle->data + handle->data_length, " ", 1);
        handle->data_length += 1;

        for(i = 0; i < param_count; ++i)
        {
            const char *                key = va_arg(ap, char *);
            const char *                value = va_arg(ap, char *);
            size_t                      length;
            size_t                      val_len;
            const unsigned char *       val_start;
            
            val_len = strlen(value);
            length = strlen(key) + val_len;

            if(strchr(value, ' '))
            {
                if((PACKET_SIZE - handle->data_length) < (length + 5))
                {
                    return globus_error_put(
                        globus_error_construct_error(
                            GLOBUS_USAGE_MODULE,
                            NULL,
                            GLOBUS_USAGE_STATS_ERROR_TYPE_TOO_BIG,
                            __FILE__,
                            _globus_func_name,
                            __LINE__,
                            "Parameters don't fit into one packet"));
                }
                handle->data_length += sprintf(
                    (char *) handle->data + handle->data_length,
                    "%s=\"%s\" ", key, value);
                val_start = handle->data + handle->data_length - val_len - 2;
                GlobusUsageTr(val_start, val_len, '"', '\'');
                GlobusUsageTr(val_start, val_len, '\n', ' ');
            }
            else
            {
                if((PACKET_SIZE - handle->data_length) < (length + 3))
                {
                    return globus_error_put(
                        globus_error_construct_error(
                            GLOBUS_USAGE_MODULE,
                            NULL,
                            GLOBUS_USAGE_STATS_ERROR_TYPE_TOO_BIG,
                            __FILE__,
                            _globus_func_name,
                            __LINE__,
                            "Parameters don't fit into one packet"));
                }
                handle->data_length += sprintf(
                    (char *) handle->data + handle->data_length,
                    "%s=%s ", key, value);
                val_start = handle->data + handle->data_length - val_len - 1;
                GlobusUsageTr(val_start, val_len, '"', '\'');
                GlobusUsageTr(val_start, val_len, '\n', ' ');
            }
        }
    }
    
    globus_l_usage_stats_write_packet(handle);
    
    globus_mutex_lock(&handle->mutex);    
    {
        handle->inuse = GLOBUS_FALSE;
    }
    globus_mutex_unlock(&handle->mutex);

#endif
    return result;
}
