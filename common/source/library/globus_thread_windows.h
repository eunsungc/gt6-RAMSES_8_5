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

/** globus_thread_windows.h Globus Windows Threads Bindings */

#if !defined GLOBUS_THREAD_WINDOWS_H
#define GLOBUS_THREAD_WINDOWS_H

#include "globus_common_include.h"
#include "globus_module.h"
#include "globus_time.h"
#include "globus_list.h"

#define GLOBUS_NULL_POINTER_PARAMETER -1
#define globus_thread_once_t int
#define GLOBUS_THREAD_ONCE_INIT 0
#define GLOBUS_THREAD_ONCE_CALLED 1


typedef void * (*globus_thread_func_t)(void *user_arg); 
typedef void (*globus_thread_key_destructor_func_t)(void *); 

typedef struct globus_thread_key_s
{
    DWORD TLSIndex;
    globus_thread_key_destructor_func_t destructorFunction;
} globus_thread_key_t;

typedef struct globus_i_thread_t
{
	unsigned long threadID;
	UserFunctionInfo userFunctionInfo;
	// list of keys associated with this thread 
	// that have destructor functions
	globus_list_t * dataDestructionKeyList;
} globus_i_thread_t;

typedef unsigned long globus_thread_t;

typedef struct __globus_threadattr_t
{
	void * threadAttribute;
} globus_threadattr_t;

typedef struct _globus_mutexattr_t
{
	LPSECURITY_ATTRIBUTES securityAttributes;
} globus_mutexattr_t;

typedef HANDLE globus_mutex_t;

typedef struct _globus_condattr_t
{
	LPSECURITY_ATTRIBUTES securityAttributes;
} globus_condattr_t;

typedef struct _globus_cond_t
{
	HANDLE events[2];
	int numberOfWaiters;
} globus_cond_t;

/* typedef DWORD globus_abstime_t; */

#ifdef __cplusplus
extern "C" {
#endif

/* API calls */
extern int	globus_thread_create(globus_thread_t *thread,
				     globus_threadattr_t *attr,
				     globus_thread_func_t func,
				     void *user_arg);
extern void	globus_thread_exit(void *status);

#define globus_macro_i_am_only_thread() GLOBUS_FALSE

extern int		globus_threadattr_init(globus_threadattr_t *attr);
extern int		globus_threadattr_destroy(globus_threadattr_t *attr);

extern int		globus_thread_key_create(globus_thread_key_t *key,
				   globus_thread_key_destructor_func_t destructor_func);
extern int		globus_thread_key_delete(globus_thread_key_t key);
extern int		globus_thread_setspecific(globus_thread_key_t key,
						  void *value);
extern void *		globus_thread_getspecific(globus_thread_key_t key);
extern globus_thread_t	globus_thread_self(void);
extern int		globus_thread_equal(globus_thread_t t1,
					    globus_thread_t t2);
extern int		globus_thread_once(globus_thread_once_t *once_control,
					   void (*init_routine)(void));
extern void		globus_thread_yield(void);

extern globus_bool_t    globus_i_am_only_thread(void);

extern int		globus_mutexattr_init(globus_mutexattr_t *attr);
extern int		globus_mutexattr_destroy(globus_mutexattr_t *attr);
extern int		globus_mutex_init(globus_mutex_t *mutex,
					  globus_mutexattr_t *attr);
extern int		globus_mutex_destroy(globus_mutex_t *mutex);
extern int		globus_mutex_lock(globus_mutex_t *mutex);
extern int		globus_mutex_trylock(globus_mutex_t *mutex);
extern int		globus_mutex_unlock(globus_mutex_t *mutex);

extern int		globus_condattr_init (globus_condattr_t *attr);
extern int		globus_condattr_destroy (globus_condattr_t *attr);
extern int		globus_cond_init(globus_cond_t *cond,
					 globus_condattr_t *attr);
extern int		globus_cond_destroy(globus_cond_t *cond);
extern int		globus_cond_wait(globus_cond_t *cond,
					 globus_mutex_t *mutex);
extern int		globus_cond_timedwait(globus_cond_t *cond,
					 globus_mutex_t *mutex,
					 globus_abstime_t * abstime);
extern int		globus_cond_timedwait_rel( globus_cond_t *cv, 
					 globus_mutex_t *mut,
					 long milliseconds );
extern int		globus_cond_signal(globus_cond_t *cond);
extern int		globus_cond_broadcast(globus_cond_t *cond);
extern int      globus_thread_cancel(globus_thread_t thread);



/******************************************************************************
			       Module definition
******************************************************************************/

extern int globus_i_thread_pre_activate();

extern globus_module_descriptor_t globus_i_thread_module;

#define GLOBUS_THREAD_MODULE (&globus_i_thread_module)

#ifdef __cplusplus
}
#endif

#endif /* GLOBUS_THREAD_WINDOWS_H */
