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

#include "globus_xio_driver.h"
#include "globus_xio_popen_driver.h"
#include "version.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <sys/wait.h>
#endif

#define USE_SOCKET_PAIR 1
#define GLOBUS_L_XIO_POPEN_WAITPID_DELAY 500

#ifdef WIN32
#define WNOHANG 0
#endif

GlobusDebugDefine(GLOBUS_XIO_POPEN);

#define GlobusXIOPOpenDebugPrintf(level, message)                            \
    GlobusDebugPrintf(GLOBUS_XIO_POPEN, level, message)

#define GlobusXIOPOpenDebugEnter()                                           \
    GlobusXIOPOpenDebugPrintf(                                               \
        GLOBUS_L_XIO_POPEN_DEBUG_TRACE,                                      \
        (_XIOSL("[%s] Entering\n"), _xio_name))
        
#define GlobusXIOPOpenDebugExit()                                            \
    GlobusXIOPOpenDebugPrintf(                                               \
        GLOBUS_L_XIO_POPEN_DEBUG_TRACE,                                      \
        (_XIOSL("[%s] Exiting\n"), _xio_name))

#define GlobusXIOPOpenDebugExitWithError()                                   \
    GlobusXIOPOpenDebugPrintf(                                               \
        GLOBUS_L_XIO_POPEN_DEBUG_TRACE,                                      \
        (_XIOSL("[%s] Exiting with error\n"), _xio_name))

enum globus_l_xio_error_levels
{
    GLOBUS_L_XIO_POPEN_DEBUG_TRACE       = 1,
    GLOBUS_L_XIO_POPEN_DEBUG_INFO        = 2
};

static
int
globus_l_xio_popen_activate(void);

static
int
globus_l_xio_popen_deactivate(void);

#ifdef popen
#undef popen
#endif
GlobusXIODefineModule(popen) =
{
    "globus_xio_popen",
    globus_l_xio_popen_activate,
    globus_l_xio_popen_deactivate,
    GLOBUS_NULL,
    GLOBUS_NULL,
    &local_version
};

/*
 *  attribute structure
 */
typedef struct xio_l_popen_attr_s
{
    globus_bool_t                       ignore_program_errors;
    globus_bool_t                       use_blocking_io;
    globus_bool_t                       pass_env;
    char *                              program_name;
    char **                             argv;
    int                                 argc;
    char **                             env;
    int                                 env_count;
    globus_xio_popen_preexec_func_t     fork_cb;
} xio_l_popen_attr_t;

/* default attr */
static const xio_l_popen_attr_t         xio_l_popen_attr_default =
{
    GLOBUS_FALSE,
    GLOBUS_FALSE,
    GLOBUS_FALSE,
    NULL,
    NULL,
    0,
    NULL
};

/*
 *  handle structure
 */
typedef struct xio_l_popen_handle_s
{
    globus_xio_system_file_handle_t     in_system;
    globus_xio_system_file_handle_t     out_system;
    globus_xio_system_file_handle_t     err_system;
    globus_xio_system_file_t            infd;
    globus_xio_system_file_t            outfd;
    globus_xio_system_file_t            errfd;    
    globus_bool_t                       use_blocking_io;
    globus_bool_t                       ignore_program_errors;
    globus_mutex_t                      lock; /* only used to protect below */
    globus_off_t                        file_position;
    pid_t                               pid;
    int                                 wait_count;
    int                                 kill_state;
    globus_bool_t                       canceled;
    globus_xio_popen_preexec_func_t     fork_cb;
    globus_xio_operation_t              close_op;
} xio_l_popen_handle_t;

enum
{
    GLOBUS_L_XIO_POPEN_NONE,
    GLOBUS_L_XIO_POPEN_TERM,
    GLOBUS_L_XIO_POPEN_KILL
};


#define GlobusXIOPOpenPosition(handle)                                \
    globus_l_xio_popen_update_position(handle, 0, SEEK_CUR)

static
void
globus_l_popen_waitpid(
    xio_l_popen_handle_t *              handle,
    int                                 opts);

static
globus_off_t
globus_l_xio_popen_update_position(
    xio_l_popen_handle_t *              handle,
    globus_off_t                        offset,
    int                                 whence)
{
    globus_mutex_lock(&handle->lock);
    {
        handle->file_position += offset;
        offset = handle->file_position;
    }
    globus_mutex_unlock(&handle->lock);
    
    return offset;
}

static
globus_result_t
globus_l_xio_popen_attr_init(
    void **                             out_attr)
{
    xio_l_popen_attr_t *                attr;
    globus_result_t                     result;
    GlobusXIOName(globus_l_xio_popen_attr_init);

    GlobusXIOPOpenDebugEnter();
    /*
     *  create a file attr structure and intialize its values
     */
    attr = (xio_l_popen_attr_t *) globus_malloc(sizeof(xio_l_popen_attr_t));
    if(!attr)
    {
        result = GlobusXIOErrorMemory("attr");
        goto error_attr;
    }

    memcpy(attr, &xio_l_popen_attr_default, sizeof(xio_l_popen_attr_t));
    *out_attr = attr;

    GlobusXIOPOpenDebugExit();
    return GLOBUS_SUCCESS;

error_attr:
    GlobusXIOPOpenDebugExitWithError();
    return result;
}

/*
 *  copy an attribute structure
 */
static
globus_result_t
globus_l_xio_popen_attr_copy(
    void **                             dst,
    void *                              src)
{
    xio_l_popen_attr_t *                attr;
    xio_l_popen_attr_t *                src_attr;
    globus_result_t                     result;
    int                                 i;
    GlobusXIOName(globus_l_xio_popen_attr_copy);

    GlobusXIOPOpenDebugEnter();

    src_attr = (xio_l_popen_attr_t *) src;
    attr = (xio_l_popen_attr_t *) globus_malloc(sizeof(xio_l_popen_attr_t));
    if(!attr)
    {
        result = GlobusXIOErrorMemory("attr");
        goto error_attr;
    }

    memcpy(attr, src_attr, sizeof(xio_l_popen_attr_t));
    if(src_attr->program_name != NULL)
    {
        attr->program_name = strdup(src_attr->program_name);
    }
    if(src_attr->argc > 0)
    {
        attr->argv = (char **)globus_calloc(attr->argc+1, sizeof(char*));
        for(i = 0; i < attr->argc; i++)
        {
            attr->argv[i] = strdup(src_attr->argv[i]);
        }
        attr->argv[i] = NULL;
    }
    if(src_attr->env_count > 0)
    {
        attr->env = (char **)globus_calloc(attr->env_count+1, sizeof(char*));
        for(i = 0; i < attr->env_count; i++)
        {
            attr->env[i] = strdup(src_attr->env[i]);
        }
        attr->env[i] = NULL;
    }
    *dst = attr;

    GlobusXIOPOpenDebugExit();
    return GLOBUS_SUCCESS;

error_attr:
    GlobusXIOPOpenDebugExitWithError();
    return result;
}

static
globus_result_t
globus_l_xio_popen_attr_cntl(
    void *                              driver_attr,
    int                                 cmd,
    va_list                             ap)
{
    int                                 i;
    char **                             argv;
    char **                             env;
    xio_l_popen_attr_t *                attr;
    globus_xio_popen_preexec_func_t     cb;
    GlobusXIOName(globus_l_xio_popen_attr_cntl);

    GlobusXIOPOpenDebugEnter();

    attr = (xio_l_popen_attr_t *) driver_attr;

    switch(cmd)
    {
        case GLOBUS_XIO_POPEN_SET_PROGRAM:
            argv = va_arg(ap, char **);
            for(attr->argc = 0; argv[attr->argc] != NULL; attr->argc++)
            {
            }
            attr->argv = calloc(attr->argc + 1, sizeof(char *));
            for(i = 0; i < attr->argc; i++)
            {
                attr->argv[i] = strdup(argv[i]);
            }
            attr->argv[i] = NULL;
            attr->program_name = strdup(attr->argv[0]);
            break;

        case GLOBUS_XIO_POPEN_SET_CHILD_ENV:
            env = va_arg(ap, char **);
            for(attr->env_count = 0; env[attr->env_count] != NULL;
                attr->env_count++)
            {
            }
            attr->env = calloc(attr->env_count + 1, sizeof(char *));
            for(i = 0; i < attr->env_count; i++)
            {
                attr->env[i] = strdup(env[i]);
            }
            attr->env[i] = NULL;
            break;


        case GLOBUS_XIO_POPEN_SET_PASS_ENV:
            attr->pass_env = va_arg(ap, globus_bool_t);
            break;

        case GLOBUS_XIO_POPEN_SET_BLOCKING_IO:
            attr->use_blocking_io = va_arg(ap, globus_bool_t);
            break;

        case GLOBUS_XIO_POPEN_SET_IGNORE_ERRORS:
            attr->ignore_program_errors = va_arg(ap, globus_bool_t);
            break;

        case GLOBUS_XIO_POPEN_SET_PREEXEC_FUNC:
            cb = va_arg(ap, globus_xio_popen_preexec_func_t);
            attr->fork_cb = (globus_xio_popen_preexec_func_t) cb;
            break;

        default:
            break;
    }

    GlobusXIOPOpenDebugExit();
    return GLOBUS_SUCCESS;
}

static
globus_result_t
globus_l_xio_popen_attr_destroy(
    void *                              driver_attr)
{
    int                                 i;
    xio_l_popen_attr_t *                attr;
    GlobusXIOName(globus_l_xio_popen_attr_destroy);

    GlobusXIOPOpenDebugEnter();

    attr = (xio_l_popen_attr_t *) driver_attr;

    if(attr->argc > 0)
    {
        for(i = 0; i < attr->argc; i++)
        {
            free(attr->argv[i]);
        }
        free(attr->argv);
    }
    if(attr->env_count > 0)
    {
        for(i = 0; i < attr->env_count; i++)
        {
            free(attr->env[i]);
        }
        free(attr->env);
    }
    if(attr->program_name != NULL)
    {
        free(attr->program_name);
    }
    globus_free(driver_attr);

    GlobusXIOPOpenDebugExit();
    return GLOBUS_SUCCESS;
}




static
globus_result_t
globus_l_xio_popen_handle_init(
    xio_l_popen_handle_t **             handle)
{
    globus_result_t                     result;
    GlobusXIOName(globus_l_xio_popen_handle_init);
    
    GlobusXIOPOpenDebugEnter();
    
    *handle = (xio_l_popen_handle_t *)
        globus_calloc(1, sizeof(xio_l_popen_handle_t));
    if(!*handle)
    {
        result = GlobusXIOErrorMemory("handle");
        goto error_handle;
    }
    
    globus_mutex_init(&(*handle)->lock, NULL);
    
    GlobusXIOPOpenDebugExit();
    return GLOBUS_SUCCESS;

error_handle:
    GlobusXIOPOpenDebugExitWithError();
    return result;    
}

static
void
globus_l_xio_popen_handle_destroy(
    xio_l_popen_handle_t *              handle)
{
    GlobusXIOName(globus_l_xio_popen_handle_destroy);
    
    GlobusXIOPOpenDebugEnter();
    
    globus_mutex_destroy(&handle->lock);
    globus_free(handle);
    
    GlobusXIOPOpenDebugExit();
}

static
void
globus_l_xio_popen_child(
    const xio_l_popen_attr_t *          attr,
    const globus_xio_contact_t *        contact_info,
    int *                               infds,
    int *                               outfds,
    int *                               errfds)
{
    int                                 rc;

#   if !defined(USE_SOCKET_PAIR)
    close(outfds[1]);
    close(infds[0]);
    rc = dup2(outfds[0], STDIN_FILENO);
    if(rc < 0)
    {
        goto error;
    }
    close(outfds[0]);
    rc = dup2(infds[1], STDOUT_FILENO);
    if(rc < 0)
    {
        goto error;
    }
    close(infds[1]);
#   else
    rc = dup2(outfds[1], STDIN_FILENO);
    if(rc < 0)
    {
        close(outfds[0]);
        close(outfds[1]);
        goto error;
    }
    rc = dup2(infds[1], STDOUT_FILENO);
    if(rc < 0)
    {
        close(infds[0]);
        close(infds[1]);
        goto error;
    }
    close(infds[0]);
    close(infds[1]);
#   endif
    
    if(!attr->ignore_program_errors)
    {
        rc = dup2(errfds[1], STDERR_FILENO);
        if(rc < 0)
        {
            close(errfds[0]);
            close(errfds[1]);
            goto error;
        }
    }
    close(errfds[0]);
    close(errfds[1]);

    /* if necessary, make optional 
    setsid();
    */

    if(attr->pass_env)
    {
        rc = execv(attr->program_name, attr->argv);
    }
    else
    {
        char *                          l_env[] = {0};
        char **                         env;

        env = l_env;
        if(attr->env != NULL)
        {
            env = attr->env;
        }
        rc = execve(attr->program_name, attr->argv, env);
    }

error:
    exit(rc);
}

static
globus_result_t
globus_l_xio_popen_init_child_pipe(
    int                                 fd,
    globus_xio_system_file_handle_t *   out_system)
{
    globus_result_t                     result;
    GlobusXIOName(globus_l_xio_popen_init_child_pipe);

#ifndef WIN32
    fcntl(fd, F_SETFD, FD_CLOEXEC);
#endif

    result = globus_xio_system_file_init(out_system, fd);
    if(result != GLOBUS_SUCCESS)
    {
        result = GlobusXIOErrorWrapFailed(
            "globus_xio_system_file_init", result);
        goto error_init;
    }

    return GLOBUS_SUCCESS;

error_init:

    return result;
}

/*
 *  open a file
 */
static
globus_result_t
globus_l_xio_popen_open(
    const globus_xio_contact_t *        contact_info,
    void *                              driver_link,
    void *                              driver_attr,
    globus_xio_operation_t              op)
{
    int                                 rc;
    int                                 s_fds[2];
    int                                 infds[2];
    int                                 outfds[2];
    int                                 errfds[2];
    xio_l_popen_handle_t *              handle;
    const xio_l_popen_attr_t *          attr;
    globus_result_t                     result;
    GlobusXIOName(globus_l_xio_popen_open);
    
    GlobusXIOPOpenDebugEnter();
    
#ifdef WIN32
    result = GlobusXIOErrorSystemResource("not available for windows");
#else
    attr = (const xio_l_popen_attr_t *) 
        driver_attr ? driver_attr : &xio_l_popen_attr_default;

    /* check that program exists and is exec=able first */

    rc = access(attr->program_name, R_OK | X_OK);
    if(rc != 0)
    {
        result = GlobusXIOErrorSystemError("access check", errno);
        goto error_handle;
    }

    result = globus_l_xio_popen_handle_init(&handle);
    if(result != GLOBUS_SUCCESS)
    {
        result = GlobusXIOErrorWrapFailed(
            "globus_l_xio_popen_handle_init", result);
        goto error_handle;
    }

    handle->ignore_program_errors = attr->ignore_program_errors;
    handle->use_blocking_io = attr->use_blocking_io;
    
#   if defined(USE_SOCKET_PAIR)
    {
        rc = socketpair(AF_UNIX, SOCK_STREAM, 0, s_fds);
        if(rc != 0)
        {
            result = GlobusXIOErrorSystemError("socketpair", errno);
            goto error_in_pipe;
        }

        /* trick the rest of the code into thinking it is using pipe */
        outfds[0] = s_fds[0];
        outfds[1] = s_fds[1];
        infds[0] = s_fds[0];
        infds[1] = s_fds[1];
    }
#   else
    {
        rc = pipe(infds);
        if(rc != 0)
        {
            result = GlobusXIOErrorSystemError("pipe", errno);
            goto error_in_pipe;
        }
        rc = pipe(outfds);
        if(rc != 0)
        {
            result = GlobusXIOErrorSystemError("pipe", errno);
            goto error_out_pipe;
        }
    }
#   endif

    rc = pipe(errfds);
    if(rc != 0)
    {
        result = GlobusXIOErrorSystemError("pipe", errno);
        goto error_err_pipe;
    }
    fcntl(errfds[0], F_SETFL, O_NONBLOCK);
    fcntl(errfds[1], F_SETFL, O_NONBLOCK);
    
    handle->pid = fork();
    if(handle->pid < 0)
    {
        result = GlobusXIOErrorSystemError("fork", errno);
        goto error_fork;
    }
    else if(handle->pid == 0)
    {
        globus_l_xio_popen_child(attr, contact_info, infds, outfds, errfds);
    }

#   if defined(USE_SOCKET_PAIR)
    {
        handle->infd = s_fds[0];
        handle->outfd = s_fds[0];
        
        result = globus_l_xio_popen_init_child_pipe(
            handle->infd,
            &handle->in_system);
        if(result != GLOBUS_SUCCESS)
        {
            goto error_init;
        }
        handle->out_system = handle->in_system;
        close(s_fds[1]);
    }
#   else
    {
        handle->infd = infds[0];
        handle->outfd = outfds[1];

        result = globus_l_xio_popen_init_child_pipe(
            handle->outfd,
            &handle->out_system);
        if(result != GLOBUS_SUCCESS)
        {
            goto error_init;
        }
        result = globus_l_xio_popen_init_child_pipe(
            handle->infd,
            &handle->in_system);
        if(result != GLOBUS_SUCCESS)
        {
            goto error_init;
        }
        close(outfds[0]);
        close(infds[1]);
    }
#   endif

    handle->errfd = errfds[0];
    result = globus_l_xio_popen_init_child_pipe(
        handle->errfd,
        &handle->err_system);
    if(result != GLOBUS_SUCCESS)
    {
        goto error_init;
    }
    close(errfds[1]);
    globus_xio_driver_finished_open(handle, op, GLOBUS_SUCCESS);
    
    GlobusXIOPOpenDebugExit();
    return GLOBUS_SUCCESS;

error_init:
error_fork:
    close(errfds[0]);
    close(errfds[1]);
#   if defined(USE_SOCKET_PAIR)
error_err_pipe:
    close(s_fds[0]);
    close(s_fds[1]);
#   else
error_err_pipe:
    close(outfds[0]);
    close(outfds[1]);
error_out_pipe:
    close(infds[0]);
    close(infds[1]);
#   endif
error_in_pipe:
    globus_l_xio_popen_handle_destroy(handle);
error_handle:
#endif
    GlobusXIOPOpenDebugExitWithError();
    return result;
}

static
void
globus_l_xio_popen_close_oneshot(
    void *                              arg)
{
    xio_l_popen_handle_t *              handle;
    GlobusXIOName(globus_l_xio_popen_close_oneshot);

    GlobusXIOPOpenDebugEnter();
    handle = (xio_l_popen_handle_t *) arg;
            
    globus_l_popen_waitpid(handle, WNOHANG);

    GlobusXIOPOpenDebugExit();
}

static
void
globus_l_popen_waitpid(
    xio_l_popen_handle_t *              handle,
    int                                 opts)
{
    globus_result_t                     result = GLOBUS_SUCCESS;
    int                                 status;
    int                                 rc;
    globus_reltime_t                    delay;
    GlobusXIOName(globus_l_popen_waitpid);

#ifdef WIN32
    result = GlobusXIOErrorSystemResource("not available for windows");
#else
    rc = waitpid(handle->pid, &status, opts);
    if(rc > 0)
    {
        /* if program exited normally, but with a nonzero code OR
         * program exited by signal, and we didn't signal it */
        if(((WIFEXITED(status) && WEXITSTATUS(status) != 0) || 
            (WIFSIGNALED(status) && handle->kill_state != GLOBUS_L_XIO_POPEN_NONE))
            && !handle->ignore_program_errors)
        {
            /* read programs stderr and dump it to an error result */
            globus_size_t                   nbytes = 0;
            globus_xio_iovec_t              iovec;
            char                            buf[8192];

            iovec.iov_base = buf;
            iovec.iov_len = sizeof(buf) - 1;
            
            result = globus_xio_system_file_read(
                handle->err_system, 0, &iovec, 1, 0, &nbytes);
            
            buf[nbytes] = 0;

            if(WIFEXITED(status))
            {
                result = globus_error_put(
                    globus_error_construct_error(
                        GLOBUS_XIO_MODULE,
                        GLOBUS_NULL,
                        GLOBUS_XIO_ERROR_SYSTEM_ERROR,
                        __FILE__,
                        _xio_name,
                        __LINE__,
                        _XIOSL("popened program exited with an error "
                               "(exit code: %d):\n%s"),
                        WEXITSTATUS(status), 
                        buf));
            }
            else
            {
                result = globus_error_put(
                    globus_error_construct_error(
                        GLOBUS_XIO_MODULE,
                        GLOBUS_NULL,
                        GLOBUS_XIO_ERROR_SYSTEM_ERROR,
                        __FILE__,
                        _xio_name,
                        __LINE__,
                        _XIOSL("popened program was terminated by a signal"
                               "(sig: %d)"),
                        WTERMSIG(status)));
            }
        }

        globus_xio_system_file_close(handle->errfd);
        globus_xio_system_file_destroy(handle->err_system);

        globus_xio_driver_finished_close(handle->close_op, result);
        globus_l_xio_popen_handle_destroy(handle);
    }
    else if(rc < 0 || opts == 0)
    {
        /* If the errno is ECHILD, either some other thread or part of the
         * program called wait and got this pid's exit status, or sigaction
         * with SA_NOCLDWAIT prevented the process from becoming a zombie. Not
         * really an error case.
         */
        if (errno != ECHILD)
        {
            result = GlobusXIOErrorSystemError("waitpid", errno);
        }

        globus_xio_system_file_close(handle->errfd);
        globus_xio_system_file_destroy(handle->err_system);

        globus_xio_driver_finished_close(handle->close_op, result);
        globus_l_xio_popen_handle_destroy(handle);
    }
    else
    {
        
        handle->wait_count++;
        
        if(handle->canceled)
        {
            switch(handle->kill_state)
            {
                case GLOBUS_L_XIO_POPEN_NONE:
                    if(handle->wait_count > 5000 / GLOBUS_L_XIO_POPEN_WAITPID_DELAY)
                    {
                        handle->kill_state = GLOBUS_L_XIO_POPEN_TERM;
                        kill(handle->pid, SIGTERM);
                    }
                    break;
                case GLOBUS_L_XIO_POPEN_TERM:
                    if(handle->wait_count > 15000 / GLOBUS_L_XIO_POPEN_WAITPID_DELAY)
                    {
                        handle->kill_state = GLOBUS_L_XIO_POPEN_KILL;
                        kill(handle->pid, SIGKILL);
                    }
                    break;
                default:
                    break;
            } 
        }
        
        GlobusTimeReltimeSet(delay, 0, GLOBUS_L_XIO_POPEN_WAITPID_DELAY);
        globus_callback_register_oneshot(
            NULL,
            &delay,
            globus_l_xio_popen_close_oneshot,
            handle);         
    }

#endif
    GlobusXIOPOpenDebugExit();
}

/*
 *  close a file
 */
static
globus_result_t
globus_l_xio_popen_close(
    void *                              driver_specific_handle,
    void *                              attr,
    globus_xio_operation_t              op)
{
    xio_l_popen_handle_t *              handle;
    GlobusXIOName(globus_l_xio_popen_close);

    GlobusXIOPOpenDebugEnter();
    
    handle = (xio_l_popen_handle_t *) driver_specific_handle;

    handle->close_op = op;
    globus_xio_system_file_destroy(handle->in_system);
   
    globus_xio_system_file_close(handle->infd);

#if !defined(USE_SOCKET_PAIR)
    globus_xio_system_file_close(handle->outfd);
    globus_xio_system_file_destroy(handle->out_system);
#endif

    if(globus_xio_driver_operation_is_blocking(op))
    {
        globus_l_popen_waitpid(handle, 0);
    }
    else
    {
        globus_l_popen_waitpid(handle, WNOHANG);
    }

    GlobusXIOPOpenDebugExit();
    return GLOBUS_SUCCESS;
}

static
void
globus_l_xio_popen_system_read_cb(
    globus_result_t                     result,
    globus_size_t                       nbytes,
    void *                              user_arg)
{
    globus_xio_operation_t              op;
    xio_l_popen_handle_t *              handle;
    GlobusXIOName(globus_l_xio_popen_system_read_cb);
    
    GlobusXIOPOpenDebugEnter();
    
    op = (globus_xio_operation_t) user_arg;
    
    handle = (xio_l_popen_handle_t *) 
        globus_xio_operation_get_driver_specific(op);
        
    handle->canceled = globus_xio_operation_is_canceled(op);
    
    globus_l_xio_popen_update_position(
        handle,
        nbytes,
        SEEK_CUR);
        
    globus_xio_driver_finished_read(op, result, nbytes);
    
    GlobusXIOPOpenDebugExit();
}

/*
 *  read from a file
 */
static
globus_result_t
globus_l_xio_popen_read(
    void *                              driver_specific_handle,
    const globus_xio_iovec_t *          iovec,
    int                                 iovec_count,
    globus_xio_operation_t              op)
{
    xio_l_popen_handle_t *              handle;
    globus_size_t                       nbytes;
    globus_result_t                     result;
    globus_off_t                        offset;
    GlobusXIOName(globus_l_xio_popen_read);

    GlobusXIOPOpenDebugEnter();
    
    handle = (xio_l_popen_handle_t *) driver_specific_handle;
    
    offset = GlobusXIOPOpenPosition(handle);
            
    /* if buflen and waitfor are both 0, we behave like register select */
    if((globus_xio_operation_get_wait_for(op) == 0 &&
        (iovec_count > 1 || iovec[0].iov_len > 0)) ||
        (handle->use_blocking_io &&
        globus_xio_driver_operation_is_blocking(op)))
    {
        result = globus_xio_system_file_read(
            handle->in_system,
            offset,
            iovec,
            iovec_count,
            globus_xio_operation_get_wait_for(op),
            &nbytes);
        
        globus_l_xio_popen_update_position(handle, nbytes, SEEK_CUR);
        globus_xio_driver_finished_read(op, result, nbytes);
        result = GLOBUS_SUCCESS;
    }
    else
    {
        result = globus_xio_system_file_register_read(
            op,
            handle->in_system,
            offset,
            iovec,
            iovec_count,
            globus_xio_operation_get_wait_for(op),
            globus_l_xio_popen_system_read_cb,
            op);
    }
    
    GlobusXIOPOpenDebugExit();
    return result;
}

static
void
globus_l_xio_popen_system_write_cb(
    globus_result_t                     result,
    globus_size_t                       nbytes,
    void *                              user_arg)
{
    globus_xio_operation_t              op;
    xio_l_popen_handle_t *              handle;
    GlobusXIOName(globus_l_xio_popen_system_write_cb);
    
    GlobusXIOPOpenDebugEnter();
    
    op = (globus_xio_operation_t) user_arg;
    
    handle = (xio_l_popen_handle_t *) 
        globus_xio_operation_get_driver_specific(op);
        
    handle->canceled = globus_xio_operation_is_canceled(op);

    globus_l_xio_popen_update_position(
        handle,
        nbytes,
        SEEK_CUR);
        
    globus_xio_driver_finished_write(op, result, nbytes);
    
    GlobusXIOPOpenDebugExit();
}

/*
 *  write to a file
 */
static
globus_result_t
globus_l_xio_popen_write(
    void *                              driver_specific_handle,
    const globus_xio_iovec_t *          iovec,
    int                                 iovec_count,
    globus_xio_operation_t              op)
{
    xio_l_popen_handle_t *              handle;
    globus_size_t                       nbytes;
    globus_result_t                     result;
    globus_off_t                        offset;
    GlobusXIOName(globus_l_xio_popen_write);
    
    GlobusXIOPOpenDebugEnter();
    
    handle = (xio_l_popen_handle_t *) driver_specific_handle;
    
    offset = GlobusXIOPOpenPosition(handle);
            
    /* if buflen and waitfor are both 0, we behave like register select */
    if((globus_xio_operation_get_wait_for(op) == 0 &&
        (iovec_count > 1 || iovec[0].iov_len > 0)) ||
        (handle->use_blocking_io &&
        globus_xio_driver_operation_is_blocking(op)))
    {
        result = globus_xio_system_file_write(
            handle->out_system,
            offset,
            iovec,
            iovec_count,
            globus_xio_operation_get_wait_for(op),
            &nbytes);
        
        globus_l_xio_popen_update_position(handle, nbytes, SEEK_CUR);
        globus_xio_driver_finished_write(op, result, nbytes);
        result = GLOBUS_SUCCESS;
    }
    else
    {
        result = globus_xio_system_file_register_write(
            op,
            handle->out_system,
            offset,
            iovec,
            iovec_count,
            globus_xio_operation_get_wait_for(op),
            globus_l_xio_popen_system_write_cb,
            op);
    }
    
    GlobusXIOPOpenDebugExit();
    return result;
}

static globus_xio_string_cntl_table_t popen_l_string_opts_table[] =

{
    {"blocking", GLOBUS_XIO_POPEN_SET_BLOCKING_IO,
        globus_xio_string_cntl_bool},
    {"pass_env", GLOBUS_XIO_POPEN_SET_PASS_ENV,
        globus_xio_string_cntl_bool},
    {"argv", GLOBUS_XIO_POPEN_SET_PROGRAM,
        globus_xio_string_cntl_string_list},
    {"env", GLOBUS_XIO_POPEN_SET_CHILD_ENV,
        globus_xio_string_cntl_string_list},
    {"ignore_errors", GLOBUS_XIO_POPEN_SET_IGNORE_ERRORS,
        globus_xio_string_cntl_bool},
    {0}
};


static
globus_result_t
globus_l_xio_popen_init(
    globus_xio_driver_t *               out_driver)
{
    globus_xio_driver_t                 driver;
    globus_result_t                     result;
    GlobusXIOName(globus_l_xio_popen_init);
    
    GlobusXIOPOpenDebugEnter();
    
    /* I dont support any driver options, so I'll ignore the ap */
    
    result = globus_xio_driver_init(&driver, "file", GLOBUS_NULL);
    if(result != GLOBUS_SUCCESS)
    {
        result = GlobusXIOErrorWrapFailed(
            "globus_l_xio_popen_init", result);
        goto error_init;
    }

    globus_xio_driver_set_transport(
        driver,
        globus_l_xio_popen_open,
        globus_l_xio_popen_close,
        globus_l_xio_popen_read,
        globus_l_xio_popen_write,
        NULL);

    globus_xio_driver_set_attr(
        driver,
        globus_l_xio_popen_attr_init,
        globus_l_xio_popen_attr_copy,
        globus_l_xio_popen_attr_cntl,
        globus_l_xio_popen_attr_destroy);

    globus_xio_driver_string_cntl_set_table(
        driver,
        popen_l_string_opts_table);

    *out_driver = driver;
    
    GlobusXIOPOpenDebugExit();
    return GLOBUS_SUCCESS;

error_init:
    GlobusXIOPOpenDebugExitWithError();
    return result;
}

static
void
globus_l_xio_popen_destroy(
    globus_xio_driver_t                 driver)
{
    GlobusXIOName(globus_l_xio_popen_destroy);
    
    GlobusXIOPOpenDebugEnter();
    
    globus_xio_driver_destroy(driver);
    
    GlobusXIOPOpenDebugExit();
}

GlobusXIODefineDriver(
    popen,
    globus_l_xio_popen_init,
    globus_l_xio_popen_destroy);

static
int
globus_l_xio_popen_activate(void)
{
    int                                 rc;
    
    GlobusXIOName(globus_l_xio_popen_activate);
    
    GlobusDebugInit(GLOBUS_XIO_POPEN, TRACE INFO);
    
    GlobusXIOPOpenDebugEnter();
    
    rc = globus_module_activate(GLOBUS_XIO_SYSTEM_MODULE);
    if(rc != GLOBUS_SUCCESS)
    {
        goto error_activate;
    }
    
    GlobusXIORegisterDriver(popen);
    
    GlobusXIOPOpenDebugExit();
    return GLOBUS_SUCCESS;

error_activate:
    GlobusXIOPOpenDebugExitWithError();
    GlobusDebugDestroy(GLOBUS_XIO_POPEN);
    return rc;
}

static
int
globus_l_xio_popen_deactivate(void)
{
    GlobusXIOName(globus_l_xio_popen_deactivate);
    
    GlobusXIOPOpenDebugEnter();
    
    GlobusXIOUnRegisterDriver(popen);
    globus_module_deactivate(GLOBUS_XIO_SYSTEM_MODULE);
    
    GlobusXIOPOpenDebugExit();
    GlobusDebugDestroy(GLOBUS_XIO_POPEN);
    
    return GLOBUS_SUCCESS;
}
