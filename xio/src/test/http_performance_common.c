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

#include "globus_utp.h"
#include "globus_common.h"
#include "globus_xio.h"
#include "globus_xio_http.h"
#include "globus_xio_tcp_driver.h"

#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#endif
#include <string.h>
#include "http_performance_common.h"

typedef struct
{
    char *                              uri;
    globus_xio_http_request_ready_callback_t
                                        callback;
    void *                              arg;
}
http_test_uri_handler_t;

static
void
http_l_test_server_accept_callback(
    globus_xio_server_t                 server,
    globus_xio_handle_t                 handle,
    globus_result_t                     result,
    void *                              user_arg);

static
void
http_l_test_server_open_callback(
    globus_xio_handle_t                 handle,
    globus_result_t                     result,
    void *                              user_arg);

static
void
http_l_test_server_close_callback(
    globus_xio_handle_t                 handle,
    globus_result_t                     result,
    void *                              user_arg);

static
void
http_l_test_server_request_callback(
    globus_xio_handle_t                 handle,
    globus_result_t                     result,
    globus_byte_t *                     buffer,
    globus_size_t                       len,
    globus_size_t                       nbytes,
    globus_xio_data_descriptor_t        data_desc,
    void *                              user_arg);

int
http_test_initialize(
    globus_xio_driver_t *               tcp_driver,
    globus_xio_driver_t *               http_driver,
    globus_xio_stack_t *                stack)
{
    globus_result_t                     result;
    int                                 rc;

    rc = globus_module_activate(GLOBUS_COMMON_MODULE);
    if (rc != 0)
    {
        rc = 1;

        fprintf(stderr, "Error activation GLOBUS_COMMON\n");

        goto error_exit;
    }

    rc = globus_module_activate(GLOBUS_XIO_MODULE);
    if (rc != 0)
    {
        fprintf(stderr, "Error activating GLOBUS_XIO\n");
        rc = 2;

        goto deactivate_exit;
    }

    result = globus_xio_driver_load("tcp", tcp_driver);
    if (result != GLOBUS_SUCCESS)
    {
        fprintf(stderr,
                "Error loading tcp driver: %s\n",
                globus_object_printable_to_string(globus_error_peek(result)));

        rc = 10;

        goto deactivate_exit;
    }
    result = globus_xio_driver_load("http", http_driver);
    if (result != GLOBUS_SUCCESS)
    {
        fprintf(stderr,
                "Error loading http driver: %s\n",
                globus_object_printable_to_string(globus_error_peek(result)));

        rc = 11;

        goto unload_tcp_exit;
    }

    result = globus_xio_stack_init(stack, NULL);
    if (result != GLOBUS_SUCCESS)
    {
        fprintf(stderr,
                "Error initializing xio stack: %s\n",
                globus_object_printable_to_string(globus_error_peek(result)));
        rc = 12;

        goto unload_http_exit;
    }
    result = globus_xio_stack_push_driver(
            *stack,
            *tcp_driver);
    if (result != GLOBUS_SUCCESS)
    {
        fprintf(stderr,
                "Error pushing tcp onto stack: %s\n",
                globus_object_printable_to_string(globus_error_peek(result)));
        rc = 13;

        goto destroy_stack_exit;
    }

    result = globus_xio_stack_push_driver(
            *stack,
            *http_driver);
    if (result != GLOBUS_SUCCESS)
    {
        fprintf(stderr,
                "Error pushing http onto stack: %s\n",
                globus_object_printable_to_string(globus_error_peek(result)));
        rc = 14;

        goto destroy_stack_exit;
    }

    return 0;

destroy_stack_exit:
    globus_xio_stack_destroy(*stack);
unload_http_exit:
    globus_xio_driver_unload(*http_driver);
unload_tcp_exit:
    globus_xio_driver_unload(*tcp_driver);
deactivate_exit:
    globus_module_deactivate_all();
error_exit:
    return rc;
}
/* initialize() */


globus_result_t
http_test_server_init(
    http_test_server_t *                server,
    globus_xio_driver_t                 tcp_driver,
    globus_xio_driver_t                 http_driver,
    globus_xio_stack_t                  stack)
{
    int                                 rc;
    globus_result_t                     result;
    globus_xio_attr_t                   attr;
    int 				buf_size = TCP_BUF_SIZE;
    GlobusXIOName(http_test_server_init);

    memset(server, '\0', sizeof(http_test_server_t));

    globus_hashtable_init(
            &server->uri_handlers,
            16,
            globus_hashtable_string_hash,
            globus_hashtable_string_keyeq);

    rc = globus_mutex_init(&server->mutex, NULL);
    if (rc != GLOBUS_SUCCESS)
    {
        result = GlobusXIOErrorMemory("mutex");
        goto error_exit;
    }
    rc = globus_cond_init(&server->cond, NULL);
    if (rc != GLOBUS_SUCCESS)
    {
        result = GlobusXIOErrorMemory("cond");
        goto free_mutex_error;
    }
    result = globus_xio_attr_init(&attr);

    if (result != GLOBUS_SUCCESS)
    {
        goto free_cond_error;
    }
    server->http_driver = http_driver;

    result = globus_xio_attr_cntl(
        attr,
        tcp_driver,
        GLOBUS_XIO_TCP_SET_NODELAY,
        GLOBUS_TRUE);
    if (result != GLOBUS_SUCCESS)
    {
        goto destroy_attr_error;
    }
    result = globus_xio_attr_cntl(
        attr,
        tcp_driver,
        GLOBUS_XIO_TCP_SET_SNDBUF,
        buf_size);
    if (result != GLOBUS_SUCCESS)
    {
        goto destroy_attr_error;
    }
    result = globus_xio_attr_cntl(
        attr,
        tcp_driver,
        GLOBUS_XIO_TCP_SET_RCVBUF,
        buf_size);
    if (result != GLOBUS_SUCCESS)
    {
        goto destroy_attr_error;
    }
    result = globus_xio_server_create(
        &server->server, 
        attr,
        stack);

    if (result != GLOBUS_SUCCESS)
    {
        goto destroy_attr_error;
    }
    result = globus_xio_server_get_contact_string(
            server->server,
            &server->contact);
    if (result != GLOBUS_SUCCESS)
    {
        goto destroy_server_error;
    }

    globus_xio_attr_destroy(attr);

    return result;

destroy_server_error:
    globus_xio_server_close(server->server);
destroy_attr_error:
    globus_xio_attr_destroy(attr);
free_cond_error:
    globus_cond_destroy(&server->cond);
free_mutex_error:
    globus_mutex_destroy(&server->mutex);
error_exit:
    return result;
}
/* http_test_server_init() */

globus_result_t
http_test_server_register_handler(
    http_test_server_t *                server,
    const char *                        uri,
    globus_xio_http_request_ready_callback_t
                                        ready_callback,
    void *                              arg)
{
    globus_result_t                     result;
    http_test_uri_handler_t *           handler;
    GlobusXIOName(http_test_server_register_handler);

    globus_mutex_lock(&server->mutex);
    handler = globus_hashtable_remove(&server->uri_handlers, (void*) uri);

    if (handler == NULL)
    {
        handler = globus_libc_malloc(sizeof(http_test_uri_handler_t));
        if (handler == NULL)
        {
            result = GlobusXIOErrorMemory("handler");

            goto error_exit;
        }
        handler->uri = globus_libc_strdup(uri);
        if (handler->uri == NULL)
        {
            result = GlobusXIOErrorMemory("uri");
            goto free_handler_error;
        }
    }

    handler->callback = ready_callback;
    handler->arg = arg;

    globus_hashtable_insert(&server->uri_handlers, handler->uri, handler);
    globus_mutex_unlock(&server->mutex);

    return GLOBUS_SUCCESS;

free_handler_error:
    globus_libc_free(handler);
error_exit:
    globus_mutex_unlock(&server->mutex);

    return result;
}
/* http_test_server_register_handler() */

globus_result_t
http_test_server_run(
    http_test_server_t *                server)
{
    globus_result_t                     result;

    globus_mutex_lock(&server->mutex);
    while (! server->shutdown)
    {
        while ((!server->shutdown) && server->outstanding_operation == 1)
        {
            globus_cond_wait(&server->cond, &server->mutex);
        }

        if ((!server->shutdown) && (server->outstanding_operation == 0))
        {
            result = globus_xio_server_register_accept(
                    server->server,
                    http_l_test_server_accept_callback,
                    server);

            if (result == GLOBUS_SUCCESS)
            {
                server->outstanding_operation++;
            }
            else
            {
                fprintf(stderr,
                        "Error registering accept: %s\n",
                        globus_object_printable_to_string(
                            globus_error_peek(result)));
            }
        }
    }

    while (server->shutdown && server->outstanding_operation > 0)
    {
        globus_cond_wait(&server->cond, &server->mutex);
    }
    server->shutdown_done = GLOBUS_TRUE;
    globus_mutex_unlock(&server->mutex);

    return GLOBUS_SUCCESS;
}
/* http_test_server_run() */

globus_result_t
http_test_server_shutdown(
    http_test_server_t *                server)
{
    globus_mutex_lock(&server->mutex);
    server->shutdown = GLOBUS_TRUE;
    globus_cond_broadcast(&server->cond);
    globus_mutex_unlock(&server->mutex);

    return GLOBUS_SUCCESS;
}
/* http_test_server_shutdown() */

void
http_test_server_destroy(
    http_test_server_t *                server)
{
    http_test_server_shutdown(server);

    globus_mutex_lock(&server->mutex);
    while (!server->shutdown_done)
    {
        globus_cond_wait(&server->cond, &server->mutex);
    }
    globus_mutex_unlock(&server->mutex);
    globus_xio_server_close(server->server);
    globus_mutex_destroy(&server->mutex);
    globus_cond_destroy(&server->cond);

    memset(server, '\0', sizeof(http_test_server_t));

    return ;
}
/* http_test_server_destroy() */

globus_result_t
http_test_server_respond(
    http_test_server_t *                test_server,
    int                                 status_code,
    char *                              reason_phrase,
    globus_xio_http_header_t *          header_array,
    size_t                              header_array_len)
{
    int                                 i;
    globus_result_t                     result;

    globus_mutex_lock(&test_server->mutex);

    if (status_code != 0)
    {
        result = globus_xio_handle_cntl(
                test_server->handle,
                test_server->info->http_driver,
                GLOBUS_XIO_HTTP_HANDLE_SET_RESPONSE_STATUS_CODE,
                status_code);

        if (result != GLOBUS_SUCCESS)
        {
            goto error_exit;
        }
    }

    if (reason_phrase != NULL)
    {
        result = globus_xio_handle_cntl(
                test_server->handle,
                test_server->info->http_driver,
                GLOBUS_XIO_HTTP_HANDLE_SET_RESPONSE_REASON_PHRASE,
                reason_phrase);

        if (result != GLOBUS_SUCCESS)
        {
            goto error_exit;
        }
    }

    for (i = 0 ; i < header_array_len; i++)
    {
        result = globus_xio_handle_cntl(
                test_server->handle,
                test_server->info->http_driver,
                GLOBUS_XIO_HTTP_HANDLE_SET_RESPONSE_HEADER,
                header_array[i].name,
                header_array[i].value);

        if (result != GLOBUS_SUCCESS)
        {
            goto error_exit;
        }
    }
    globus_mutex_unlock(&test_server->mutex);

    return GLOBUS_SUCCESS;

error_exit:
    globus_mutex_unlock(&test_server->mutex);
    return result;
}
/* http_test_server_respond() */

globus_result_t
http_test_server_close_handle(
    http_test_server_t *                test_server)
{
    globus_result_t                     result;

    result = globus_xio_register_close(
            test_server->handle,
            NULL,
            http_l_test_server_close_callback,
            test_server);

    return result;
}

static
void
http_l_test_server_accept_callback(
    globus_xio_server_t                 server,
    globus_xio_handle_t                 handle,
    globus_result_t                     result,
    void *                              user_arg)
{
    http_test_server_t *                test_server = user_arg;
    globus_xio_attr_t                   attr;

    globus_mutex_lock(&test_server->mutex);
    if (result != GLOBUS_SUCCESS)
    {
        goto error_exit;
    }

    result = globus_xio_attr_init(&attr);
    if (result != GLOBUS_SUCCESS)
    {
        goto error_exit;
    }

    test_server->handle = handle;
    result = globus_xio_register_open(
            test_server->handle,
            NULL,
            attr,
            http_l_test_server_open_callback,
            test_server);

    globus_xio_attr_destroy(attr);
error_exit:
    if (result != GLOBUS_SUCCESS)
    {
        test_server->outstanding_operation--;
        globus_cond_signal(&test_server->cond);
    }
    globus_mutex_unlock(&test_server->mutex);
    return;
}
/* http_l_test_server_accept_callback() */

static
void
http_l_test_server_open_callback(
    globus_xio_handle_t                 handle,
    globus_result_t                     result,
    void *                              user_arg)
{
    http_test_server_t *                test_server = user_arg;
    globus_byte_t                       buffer[1];

    /* Processing is done in the request callback */
    result = globus_xio_register_read(
            handle,
            buffer,
            0,
            0,
            NULL,
            http_l_test_server_request_callback,
            test_server);

}
/* http_l_test_server_open_callback() */

static
void
http_l_test_server_request_callback(
    globus_xio_handle_t                 handle,
    globus_result_t                     result,
    globus_byte_t *                     buffer,
    globus_size_t                       len,
    globus_size_t                       nbytes,
    globus_xio_data_descriptor_t        data_desc,
    void *                              user_arg)
{
    http_test_server_t *                test_server = user_arg;
    http_test_uri_handler_t *           uri_handler;
    char *                              method;
    char *                              uri;
    globus_xio_http_version_t           http_version;
    globus_hashtable_t                  headers;

    globus_mutex_lock(&test_server->mutex);

    if (result == GLOBUS_SUCCESS)
    {
        result = globus_xio_data_descriptor_cntl(
                data_desc,
                test_server->http_driver,
                GLOBUS_XIO_HTTP_GET_REQUEST,
                &method,
                &uri,
                &http_version,
                &headers);
        if (result == GLOBUS_SUCCESS && uri != NULL)
        {
            uri_handler = globus_hashtable_lookup(
                    &test_server->uri_handlers,
                    (void*) uri);

            if (uri_handler != NULL)
            {
                globus_mutex_unlock(&test_server->mutex);

                (*uri_handler->callback)(
                    uri_handler->arg,
                    result,
                    method,
                    uri,
                    http_version,
                    headers);
                result = globus_xio_register_read(
                        handle,
                        buffer,
                        0,
                        0,
                        NULL,
                        http_l_test_server_request_callback,
                        test_server);
                return;
            }
        }
    }
    globus_xio_register_close(
            test_server->handle,
            NULL,
            http_l_test_server_close_callback,
            test_server);

    globus_mutex_unlock(&test_server->mutex);
}
/* http_l_test_server_request_callback() */

static
void
http_l_test_server_close_callback(
    globus_xio_handle_t                 handle,
    globus_result_t                     result,
    void *                              user_arg)
{
    http_test_server_t *                test_server = user_arg;
    
    globus_mutex_lock(&test_server->mutex);
    test_server->outstanding_operation--;
    globus_cond_signal(&test_server->cond);
    globus_mutex_unlock(&test_server->mutex);
}
/* http_l_test_server_close_callback() */


globus_result_t
http_test_client_request(
    globus_xio_handle_t *               new_handle,
    globus_xio_driver_t                 tcp_driver,
    globus_xio_driver_t                 http_driver,
    globus_xio_stack_t                  stack,
    const char *                        contact,
    const char *                        uri,
    const char *                        method,
    globus_xio_http_version_t           http_version,
    globus_xio_http_header_t *          header_array,
    size_t                              header_array_length)
{
    char *                              url;
    char *                              fmt = "http://%s/%s";
    globus_xio_attr_t                   attr;
    int                                 i;
    globus_result_t                     result = GLOBUS_SUCCESS;
    GlobusXIOName(http_test_client_request);

    url = globus_libc_malloc(strlen(fmt) + strlen(contact) + strlen(uri) + 1);
    if (url == GLOBUS_NULL)
    {
        result = GlobusXIOErrorMemory("url");

        goto error_exit;
    }

    sprintf(url, fmt, contact, uri);

    result = globus_xio_handle_create(new_handle, stack);

    if (result != GLOBUS_SUCCESS)
    {
        goto free_url_exit;
    }

    globus_xio_attr_init(&attr);

    if (method != NULL)
    {
        result = globus_xio_attr_cntl(
                attr,
                http_driver,
                GLOBUS_XIO_HTTP_ATTR_SET_REQUEST_METHOD,
                method);

        if (result != GLOBUS_SUCCESS)
        {
            goto destroy_attr_exit;
        }
    }

    if (http_version != GLOBUS_XIO_HTTP_VERSION_UNSET)
    {
        result = globus_xio_attr_cntl(
                attr,
                http_driver,
                GLOBUS_XIO_HTTP_ATTR_SET_REQUEST_HTTP_VERSION,
                http_version);

        if (result != GLOBUS_SUCCESS)
        {
            goto destroy_attr_exit;
        }
    }

    result = globus_xio_attr_cntl(
        attr,
        tcp_driver,
        GLOBUS_XIO_TCP_SET_NODELAY,
        GLOBUS_TRUE);
    if (result != GLOBUS_SUCCESS)
    {
        goto destroy_attr_exit;
    }
    result = globus_xio_attr_cntl(
        attr,
        tcp_driver,
        GLOBUS_XIO_TCP_SET_SNDBUF,
        TCP_BUF_SIZE);
    if (result != GLOBUS_SUCCESS)
    {
        goto destroy_attr_exit;
    }
    result = globus_xio_attr_cntl(
        attr,
        tcp_driver,
        GLOBUS_XIO_TCP_SET_RCVBUF,
        TCP_BUF_SIZE);
    if (result != GLOBUS_SUCCESS)
    {
        goto destroy_attr_exit;
    }

    for (i = 0; i < header_array_length; i++)
    {
        result = globus_xio_attr_cntl(
                attr,
                http_driver,
                GLOBUS_XIO_HTTP_ATTR_SET_REQUEST_HEADER,
                header_array[i].name,
                header_array[i].value);

        if (result != GLOBUS_SUCCESS)
        {
            goto destroy_attr_exit;
        }
    }

    result = globus_xio_open(
            *new_handle,
            url, 
            attr);

destroy_attr_exit:
    globus_xio_attr_destroy(attr);

free_url_exit:
    globus_libc_free(url);
error_exit:
    return result;
}

/* http_test_client_request() */
globus_bool_t
http_is_eof(
    globus_result_t                     res)
{
    globus_bool_t                       status = GLOBUS_TRUE;
    globus_result_t                     res2;
    globus_xio_driver_t                 http_driver;

    res2 = globus_xio_driver_load("http", &http_driver);
    if (res2 != GLOBUS_SUCCESS)
    {
        fprintf(stderr,
                "Error loading http driver: %s\n",
                globus_object_printable_to_string(globus_error_peek(res2)));

        goto error_exit;
    }
    status= globus_error_match(
            globus_error_peek(res), GLOBUS_XIO_MODULE, GLOBUS_XIO_ERROR_EOF) ||
            globus_xio_driver_error_match(http_driver, globus_error_peek(res),
            GLOBUS_XIO_HTTP_ERROR_EOF);

    globus_xio_driver_unload(http_driver);

error_exit:
    return status;
}

void
performance_init(
    performance_t *          perf,
    pingpong_func_t	     pingpong,
    next_size_func_t         next_size,
    int                      iterations,
    char *                   name,
    int			     buf_size)
{

    perf->name = name;
    perf->next_size = next_size;
    perf->pingpong = pingpong;
    if(name != GLOBUS_NULL)
    {
        prep_timers(perf, name, iterations, buf_size);
    }
}


void
fill_buffer(
    globus_byte_t *         buffer,
    int                     size)
{
    globus_size_t           i;
    for (i = 0; i < size; i++)
    {
        buffer[i] = (globus_byte_t) rand();
    }
}

int
performance_start_master(
    performance_t *          perf,
    http_test_info_t *       info)
{
    int                      timer = 0;
    int			     rc = 0;
    double                   burp_time;
    char *                   namebuf;
    int                      precision;
    double                   latency;
    double                   bw;

    info->buffer = globus_malloc(THROUGHPUT_MAX_SIZE);
    fill_buffer(info->buffer, THROUGHPUT_MAX_SIZE);

    namebuf = globus_malloc(strlen(perf->name)+30);
    
    info->size = perf->next_size(info->size);
    while(info->size != -1)
    {
        globus_utp_enable_timer(timer);

        rc = perf->pingpong(info, timer);
	if (rc != 0)
	{
	    goto error;
	}
        globus_utp_get_accum_time(
            (unsigned)timer,
            &burp_time,
            &precision);
        latency = burp_time/(info->iterations * 2.0);
        bw = info->size/(latency * 2.0);
	bw = bw / (1024.0 * 1024.0);
	sprintf(namebuf, "%s-%lu-%lf-%lf", perf->name,
                (unsigned long) info->size, latency, bw);
	globus_utp_name_timer(timer, namebuf);

        info->size = perf->next_size(info->size);
        timer++;
    }
    globus_free(namebuf);
    return rc;

error:
    globus_free(namebuf);
    return rc;	
    
}
void
performance_start_slave(
    performance_t *          perf,
    http_test_info_t *       info)
{
    int                      timer = 0;

    info->buffer = globus_malloc(THROUGHPUT_MAX_SIZE);
    fill_buffer(info->buffer, THROUGHPUT_MAX_SIZE);

    info->size = perf->next_size(info->size);
    perf->pingpong(info, timer);
}


int
throughput_next_size(
    int                       last_size)
{
    if(last_size == 0)
    {
        return 1;
    }
    else if(last_size == 1)
    {
        return 1000;
    }
    else if(last_size >= THROUGHPUT_MAX_SIZE)
    {
        return -1;
    }

    return 10*last_size;
}

int
pingpong_next_size(
    int                       last_size)
{
    if (last_size == 0)
    {
	return 1;
    }
    if(last_size >= PINGPONG_MAX_SIZE) 
    {
        return -1;
    }

    return 10*last_size;
}

static int 
num_sizes(
    performance_t *           perf)
{
    int                       last_size = 0;
    int                       n_sizes = 0;

    for(last_size=perf->next_size(0); 
        last_size!=-1; 
        last_size=perf->next_size(last_size))
    {
	n_sizes++;
    }
    return n_sizes;
}


void
prep_timers(
    performance_t *           perf,
    char *                    label,
    int                       iterations,
    int			      buf_size)
{
    int                       sizes;
    const char *              sysname;
    const char *              release;
    const char *              version;
    const char *              machine;
#   ifdef _WIN32
    {
        static char computer_name[MAX_COMPUTERNAME_LENGTH + 1];
        static char win_release[10];
        DWORD computer_name_len = MAX_COMPUTERNAME_LENGTH + 1;
        DWORD dwVersion = 0; 
        DWORD dwMajorVersion = 0;
        DWORD dwMinorVersion = 0; 
        DWORD dwBuild = 0;
        sysname = "Windows";

        dwVersion = GetVersion();
        // Get the Windows version.
        dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
        dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
        sprintf(win_release, "%d.%d", (int)dwMinorVersion, (int)dwMinorVersion);
        release = version = win_release;

        GetComputerName(computer_name, &computer_name_len);
        machine = computer_name;
    }
#   else
    {
        struct utsname            name;
        uname(&name);
        sysname = name.sysname;
        release = name.release;
        version = name.version;
        machine = name.machine;
    }
#   endif

    globus_module_activate(GLOBUS_UTP_MODULE);


    sizes = num_sizes(perf);
    globus_utp_init(sizes, GLOBUS_UTP_MODE_SHARED);

    globus_utp_set_attribute("num-iterations","","%d",iterations);
    globus_utp_set_attribute("num-sizes","","%d",sizes);
    globus_utp_set_attribute("sysname","","%s",sysname);
    globus_utp_set_attribute("release","","%s",release);
    globus_utp_set_attribute("version","","%s",version);
    globus_utp_set_attribute("machine","","%s",machine);
    globus_utp_set_attribute("buffer-size","","%d",buf_size);
}

void
performance_write_timers(
    performance_t *           perf)
{
    char                      namebuf[50];

    sprintf(namebuf, "%s-timings.txt", perf->name);
    globus_utp_write_file(namebuf);
    globus_module_deactivate(GLOBUS_UTP_MODULE);
}
