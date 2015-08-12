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



#include "globus_callout.h"
#include "globus_common.h"
#include <stdlib.h>
#include <stdio.h>
#include "globus_preload.h"


int main()
{
    globus_callout_handle_t     authz_handle;
    char *                      srcdir;
    char *                      filename;
    globus_result_t             result;

    LTDL_SET_PRELOADED_SYMBOLS();
    printf("1..2\n");

    globus_module_activate(GLOBUS_COMMON_MODULE);
    globus_module_activate(GLOBUS_CALLOUT_MODULE);
    
    result = globus_callout_handle_init(&authz_handle);

    if(result != GLOBUS_SUCCESS)
    {
        goto error_exit;
    }
    srcdir = getenv("srcdir");
    if (srcdir)
    {
        filename = globus_common_create_string("%s/%s", srcdir, "test.conf");
    }
    else
    {
        filename = globus_common_create_string("%s", "test.conf");
    }
    
    result = globus_callout_read_config(authz_handle, filename);

    if(result != GLOBUS_SUCCESS)
    {
        goto error_exit;
    }
    
    result = globus_callout_call_type(authz_handle,
                                      "TEST",
                                      "foo",
                                      "bar");

    if(result != GLOBUS_SUCCESS)
    {
        goto error_exit;
    }
    
    result = globus_callout_handle_destroy(authz_handle);


    if(result != GLOBUS_SUCCESS)
    {
        goto error_exit;
    }

    globus_module_deactivate_all();
    
    printf("ok 2 - callout_test\n");
    return 0;

 error_exit:

    fprintf(stderr,"ERROR: %s",
            globus_error_print_chain(globus_error_get(result)));
    
    globus_module_deactivate_all();

    printf("not ok 2 - callout_test returned %d\n", (int) result);
    return 1;
}
