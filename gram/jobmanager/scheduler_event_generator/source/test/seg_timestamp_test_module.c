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

#include "globus_common.h"
#include "globus_scheduler_event_generator.h"

static
int
globus_l_test_module_activate(void);

static
int
globus_l_test_module_deactivate(void);

GlobusExtensionDefineModule(globus_seg_timestamp_test_module) =
{
    "globus_seg_timestamp_test_module",
    globus_l_test_module_activate,
    globus_l_test_module_deactivate,
    NULL,
    NULL,
    NULL,
    NULL
};

int
globus_l_test_module_activate(void)
{
    char * env;
    time_t stamp;
    time_t expected;
    globus_result_t                     result;

    env = getenv("TEST_MODULE_TIMESTAMP");

    if (env == NULL)
    {
        printf ("not ok - no TEST_MODULE_TIMESTAMP environment\n");
        return 0;
    }
    expected = atoi(env);

    if (expected < 0)
    {
        printf ("not ok - non-numeric timestamp\n");
        return 0;
    }

    result = globus_scheduler_event_generator_get_timestamp(&stamp);

    if (result != GLOBUS_SUCCESS)
    {
        printf ("not ok - globus_scheduler_event_generator_get_timestamp\n");
        return 0;
    }

    if (stamp != expected)
    {
        printf ("not ok - timestamp mismatch\n");
        return 0;
    }
    printf("ok - test_module_activate timestamp test\n");
    return 0;
}

int
globus_l_test_module_deactivate(void)
{
    return 0;
}

/* main() */
