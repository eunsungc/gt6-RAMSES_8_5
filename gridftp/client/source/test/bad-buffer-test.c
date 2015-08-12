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

/*
 * simple authenticated get with bad buffer from the user.
 *
 * makes sure that the ftp client and control libraries will handle a bogus
 * buffer correctly, and send eof/error to the data callback, and finish
 * the transfer.
 */
#include "globus_ftp_client.h"
#include "globus_ftp_client_test_common.h"

static globus_mutex_t lock;
static globus_cond_t cond;
static globus_bool_t done;
static int error = 0;

static
void
done_cb(
	void *					user_arg,
	globus_ftp_client_handle_t *		handle,
	globus_object_t *			err)
{
    char * tmpstr;

    if(err) { error++; }
    if(err) tmpstr = " an";
    else    tmpstr = "out";

    printf("done with%s error\n", tmpstr);
    globus_mutex_lock(&lock);
    done = GLOBUS_TRUE;
    globus_cond_signal(&cond);
    globus_mutex_unlock(&lock);
       
}

int dummy;

static
void
data_cb(
    void *					user_arg,
    globus_ftp_client_handle_t *		handle,
    globus_object_t *				err,
    globus_byte_t *				buffer,
    globus_size_t				length,
    globus_off_t				offset,
    globus_bool_t				eof)
{
    globus_result_t				result;

    if(err) { error++; }
    if(!eof)
    {
	result = globus_ftp_client_register_read(handle,
					(globus_byte_t *) &dummy,
					sizeof(dummy),
					data_cb,
					0);
	printf("result=%d\n", (int) result);
    }
}

int main(int argc, char **argv)
{
    globus_ftp_client_handle_t			handle;
    globus_ftp_client_operationattr_t		attr;
    globus_byte_t				buffer[1024];
    globus_size_t				buffer_length = sizeof(buffer);
    globus_result_t				result;
    globus_ftp_client_handleattr_t		handle_attr;
    char *					src;
    char *					dst;

    LTDL_SET_PRELOADED_SYMBOLS();
    globus_module_activate(GLOBUS_FTP_CLIENT_MODULE);

    globus_mutex_init(&lock, GLOBUS_NULL);
    globus_cond_init(&cond, GLOBUS_NULL);

    globus_ftp_client_operationattr_init(&attr);
    globus_ftp_client_handleattr_init(&handle_attr);

    test_parse_args(argc,
		    argv,
		    &handle_attr,
		    &attr,
		    &src,
		    &dst);


    globus_ftp_client_handle_init(&handle,  &handle_attr);

    done = GLOBUS_FALSE;
    result = globus_ftp_client_get(&handle,
				   src,
				   &attr,
				   GLOBUS_NULL,
				   done_cb,
				   0);
    if(result != GLOBUS_SUCCESS)
    {
	error=1;
	done = GLOBUS_TRUE;
    }
    else
    {
	result = globus_ftp_client_register_read(
	    &handle,
	    (globus_byte_t *) 0x1,
	    buffer_length,
	    data_cb,
	    0);

        if(result != GLOBUS_SUCCESS)
        {
            error=1;
            done = GLOBUS_TRUE;
        }
    }
    globus_mutex_lock(&lock);
    
    while(!done)
    {
	globus_cond_wait(&cond, &lock);
    }
    globus_mutex_unlock(&lock);

    globus_ftp_client_handle_destroy(&handle);
    globus_module_deactivate_all();

    return error;
}
