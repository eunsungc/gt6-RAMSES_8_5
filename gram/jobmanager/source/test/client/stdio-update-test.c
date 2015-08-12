/*
 * Copyright 1999-2010 University of Chicago
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

#include "globus_common.h"
#include "globus_gram_client.h"
#include "globus_preload.h"
#include "globus_gram_protocol.h"
#include "globus_gass_transfer.h"

static char * contact_string = NULL;

typedef struct
{
    char * test_name;
    int (*test_case)(void);
}
test_case_t;

typedef struct
{
    globus_mutex_t                      mutex;
    globus_cond_t                       cond;
    int                                 status;
    int                                 failure_code;
    globus_gass_transfer_listener_t     old_listener;
    globus_gass_transfer_request_t      old_request;
    unsigned char                       old_output[64];
    globus_size_t                       old_output_len;
    globus_gass_transfer_listener_t     new_listener;
    globus_gass_transfer_request_t      new_request;
    unsigned char                       new_output[64];
    globus_size_t                       new_output_len;
}
test_monitor_t;

void
test_l_old_listener_callback(
    void *                              callback_arg,
    globus_gass_transfer_listener_t     listener);
void
test_l_new_listener_callback(
    void *                              callback_arg,
    globus_gass_transfer_listener_t     listener);

#define TEST_CASE(x) { #x, x }
#define test_assert(a) if(!(a)) { fprintf(stderr, "%s:%d: Assertion %s failed", __func__, __LINE__, #a); return 1; }
#define test_assert_gram_rc_equals(a, b) \
    { \
        int _a = (a); \
        int _b = (b); \
        if (_a != _b) \
        { \
            fprintf(stderr, "%s:%d: Expected rc = %d, got %d (%s)\n", \
                    __func__, __LINE__, \
                    _b, _a, \
                    globus_gram_protocol_error_string(_a)); \
            return 1; \
        } \
    }

static
void
test_l_gass_fail(
    void *                              callback_arg,
    globus_gass_transfer_request_t      request)
{
    return;
}

static
void
test_l_data_callback(
    void *                              arg,
    globus_gass_transfer_request_t      request,
    globus_byte_t *                     bytes,
    globus_size_t                       length,
    globus_bool_t                       last_data)
{
    test_monitor_t                      *monitor = arg;

    if (last_data)
    {
        globus_gass_transfer_request_destroy(request);
        return;
    }

    globus_gass_transfer_receive_bytes(
        request,
        bytes,
        sizeof(monitor->old_output),
        1,
        test_l_data_callback,
        monitor);
}


static
void
test_l_old_accept_callback(
    void *                              callback_arg,
    globus_gass_transfer_request_t      request)
{
    test_monitor_t                      *monitor = callback_arg;
    int                                 rc;

    globus_mutex_lock(&monitor->mutex);
    monitor->old_request = request;
    globus_mutex_unlock(&monitor->mutex);

    globus_gass_transfer_authorize(
        request,
        GLOBUS_GASS_TRANSFER_LENGTH_UNKNOWN);

    rc = globus_gass_transfer_receive_bytes(
        request,
        monitor->old_output,
        sizeof(monitor->old_output),
        1,
        test_l_data_callback,
        monitor);
    rc = globus_gass_transfer_register_listen(
            monitor->old_listener,
            test_l_old_listener_callback,
            monitor);
}

static
void
test_l_new_accept_callback(
    void *                              callback_arg,
    globus_gass_transfer_request_t      request)
{
    test_monitor_t                      *monitor = callback_arg;
    int                                 rc;

    globus_mutex_lock(&monitor->mutex);
    monitor->new_request = request;
    globus_mutex_unlock(&monitor->mutex);


    rc = globus_gass_transfer_authorize(
        request,
        GLOBUS_GASS_TRANSFER_LENGTH_UNKNOWN);

    rc = globus_gass_transfer_receive_bytes(
        request,
        monitor->new_output,
        sizeof(monitor->new_output),
        1,
        test_l_data_callback,
        monitor);
    rc = globus_gass_transfer_register_listen(
            monitor->new_listener,
            test_l_new_listener_callback,
            monitor);
}

void
test_l_old_listener_callback(
    void *                              callback_arg,
    globus_gass_transfer_listener_t     listener)
{
    int                                 rc;
    globus_gass_transfer_request_t      request;
    globus_gass_transfer_requestattr_t  attr;

    globus_gass_transfer_requestattr_init(&attr, "https");
    globus_gass_transfer_secure_requestattr_set_authorization(
            &attr,
            GLOBUS_GASS_TRANSFER_AUTHORIZE_SELF,
            "https");

    rc = globus_gass_transfer_register_accept(
            &request,
            &attr,
            listener,
            test_l_old_accept_callback,
            callback_arg);
    globus_gass_transfer_requestattr_destroy(&attr);
}
/* test_l_old_listener_callback() */

void
test_l_new_listener_callback(
    void *                              callback_arg,
    globus_gass_transfer_listener_t     listener)
{
    int                                 rc;
    globus_gass_transfer_request_t      request;
    globus_gass_transfer_requestattr_t  attr;

    globus_gass_transfer_requestattr_init(&attr, "https");
    globus_gass_transfer_secure_requestattr_set_authorization(
            &attr,
            GLOBUS_GASS_TRANSFER_AUTHORIZE_SELF,
            "https");

    rc = globus_gass_transfer_register_accept(
            &request,
            &attr,
            listener,
            test_l_new_accept_callback,
            callback_arg);
    globus_gass_transfer_requestattr_destroy(&attr);
}
/* test_l_new_listener_callback() */

static
void
test_l_gram_callback(
    void *                              callback_arg,
    char *                              job_contact,
    int                                 state,
    int                                 errorcode)
{
    test_monitor_t                      *monitor = callback_arg;

    globus_mutex_lock(&monitor->mutex);
    monitor->failure_code = errorcode;
    switch (monitor->status)
    {
        case GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED:
            monitor->status = state;
            break;
        case GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_IN:
            if (state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED)
            {
                monitor->status = state;
            }
        case GLOBUS_GRAM_PROTOCOL_JOB_STATE_PENDING:
            if (state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED &&
                state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_IN)
            {
                monitor->status = state;
            }
            break;
        case GLOBUS_GRAM_PROTOCOL_JOB_STATE_ACTIVE:
            if (state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED &&
                state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_PENDING &&
                state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_IN)
            {
                monitor->status = state;
            }
            break;
        case GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_OUT:
            if (state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED &&
                state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_PENDING &&
                state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_IN &&
                state != GLOBUS_GRAM_PROTOCOL_JOB_STATE_ACTIVE)
            {
                monitor->status = state;
            }
            break;
        case GLOBUS_GRAM_PROTOCOL_JOB_STATE_FAILED:
        case GLOBUS_GRAM_PROTOCOL_JOB_STATE_DONE:
        case GLOBUS_GRAM_PROTOCOL_JOB_STATE_ALL:
        case GLOBUS_GRAM_PROTOCOL_JOB_STATE_SUSPENDED:
            break;
    }
    globus_cond_signal(&monitor->cond);
    globus_mutex_unlock(&monitor->mutex);
}
/* test_l_gram_callback() */

/* Test Case:
 * Submit a job that generates output after a short time.
 * Before that output is generated, send a STDIO_UPDATE signal, to
 * direct the output to a different port.
 * Verify that the output arrives at the new port
 */
int
test_stdio_update(void)
{
    char                                *old_listener_url, *new_listener_url;
    char                                *old_job_contact;
    int                                 rc;
    char                                *callback_contact;
    char                                *old_rsl, *new_rsl;
    test_monitor_t                      monitor;
    const char                          rsl_spec[] =
            "&(executable=/bin/sh)"
            "(arguments=-c 'sleep 30; echo hello;')"
            "(rsl_substitution = (TEST_GASS_URL %s))"
            "(stdout = $(TEST_GASS_URL)/out)";
    const char                          stdio_update_rsl_spec[] =
            "&(stdout = %s/out)";

    globus_mutex_init(&monitor.mutex, NULL);
    globus_cond_init(&monitor.cond, NULL);

    memset(monitor.old_output, 0, sizeof(monitor.old_output));
    memset(monitor.new_output, 0, sizeof(monitor.new_output));
    monitor.old_request = GLOBUS_NULL_HANDLE;
    monitor.new_request = GLOBUS_NULL_HANDLE;
    monitor.status = GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED;
    monitor.failure_code = 0;

    /* Create a pair of listeners and get their base URLs. The job will be
     * submitted with stdout directed to the first, then redirected to the
     * second via a stdio update signal
     */
    rc = globus_gass_transfer_create_listener(
            &monitor.old_listener,
            NULL,
            "https");
    test_assert_gram_rc_equals(rc, GLOBUS_SUCCESS);
    test_assert(monitor.old_listener != GLOBUS_NULL_HANDLE);

    old_listener_url = globus_gass_transfer_listener_get_base_url(
            monitor.old_listener);
    test_assert(old_listener_url != NULL);

    rc = globus_gass_transfer_register_listen(
            monitor.old_listener,
            test_l_old_listener_callback,
            &monitor);
    test_assert_gram_rc_equals(rc, GLOBUS_SUCCESS);

    rc = globus_gass_transfer_create_listener(
            &monitor.new_listener,
            NULL,
            "https");
    test_assert_gram_rc_equals(rc, GLOBUS_SUCCESS);
    test_assert(monitor.new_listener != GLOBUS_NULL_HANDLE);

    new_listener_url = globus_gass_transfer_listener_get_base_url(
            monitor.new_listener);
    test_assert(new_listener_url != NULL);

    rc = globus_gass_transfer_register_listen(
            monitor.new_listener,
            test_l_new_listener_callback,
            &monitor);
    test_assert(rc == GLOBUS_SUCCESS);

    old_rsl = globus_common_create_string(rsl_spec, old_listener_url);
    test_assert(old_rsl != NULL);

    /* Submit the job, do the two-phase commit, then submit a restart
     * request with the new stdout destination
     */
    rc = globus_gram_client_callback_allow(
            test_l_gram_callback,
            &monitor,
            &callback_contact);
    test_assert_gram_rc_equals(rc, GLOBUS_SUCCESS);
    test_assert(callback_contact != NULL);

    rc = globus_gram_client_job_request(
            contact_string,
            old_rsl,
            GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED|
            GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_IN|
            GLOBUS_GRAM_PROTOCOL_JOB_STATE_PENDING|
            GLOBUS_GRAM_PROTOCOL_JOB_STATE_ACTIVE|
            GLOBUS_GRAM_PROTOCOL_JOB_STATE_STAGE_OUT|
            GLOBUS_GRAM_PROTOCOL_JOB_STATE_DONE|
            GLOBUS_GRAM_PROTOCOL_JOB_STATE_FAILED,
            callback_contact,
            &old_job_contact);
    test_assert_gram_rc_equals(rc, GLOBUS_SUCCESS);
    test_assert(old_job_contact != NULL);

    globus_mutex_lock(&monitor.mutex);
    while (monitor.status == GLOBUS_GRAM_PROTOCOL_JOB_STATE_UNSUBMITTED)
    {
        globus_cond_wait(&monitor.cond, &monitor.mutex);
    }

    new_rsl = globus_common_create_string(
            stdio_update_rsl_spec, new_listener_url);
    test_assert(new_rsl != NULL);

    rc = globus_gram_client_job_signal(
            old_job_contact,
            GLOBUS_GRAM_PROTOCOL_JOB_SIGNAL_STDIO_UPDATE,
            new_rsl,
            &monitor.status,
            &monitor.failure_code);
    test_assert_gram_rc_equals(rc, GLOBUS_SUCCESS);

    /* Wait for job to complete. After it's done, check to see which
     * destination got stdout
     */
    while (monitor.status != GLOBUS_GRAM_PROTOCOL_JOB_STATE_DONE &&
           monitor.status != GLOBUS_GRAM_PROTOCOL_JOB_STATE_FAILED)
    {
        globus_cond_wait(&monitor.cond, &monitor.mutex);
    }
    if (monitor.old_request)
    {
        globus_gass_transfer_fail(monitor.old_request, test_l_gass_fail, NULL);
    }

    globus_mutex_unlock(&monitor.mutex);

    if (monitor.new_output[0] == 0)
    {
        fprintf(stderr, "Didn't get expected output to new handle\n");
        test_assert(strcmp((char *) monitor.new_output, "hello\n") == 0);
    }
    if (monitor.old_output[0] != 0)
    {
        fprintf(stderr, "Unexpected output to old handle: %s",
                monitor.old_output);
        test_assert(monitor.old_output[0] == 0);
    }

    return rc;
}
/* test_stdio_update() */

int main(int argc, char *argv[])
{
    int i;
    int failed;
    test_case_t tests[] =
    {
        TEST_CASE(test_stdio_update),
        {NULL, NULL}
    };

    LTDL_SET_PRELOADED_SYMBOLS();
    printf("1..%d\n", (int) (sizeof(tests)/sizeof(tests[0]) - 1));
    contact_string = getenv("CONTACT_STRING");
    if (argc == 2)
    {
        contact_string = argv[1];
    }
    if (contact_string == NULL)
    {
        fprintf(stderr, "Usage: %s CONTACT-STRING\n", argv[0]);
        exit(1);
    }

    globus_module_activate(GLOBUS_GRAM_CLIENT_MODULE);
    globus_module_activate(GLOBUS_GASS_TRANSFER_MODULE);

    for (i = 0, failed = 0; tests[i].test_name != NULL; i++)
    {
        int rc = tests[i].test_case();

        if (rc != 0)
        {
            printf("not ");
            failed++;
        }
        printf("ok  %s\n", tests[i].test_name);
    }
    globus_module_deactivate_all();

    return failed;
}
