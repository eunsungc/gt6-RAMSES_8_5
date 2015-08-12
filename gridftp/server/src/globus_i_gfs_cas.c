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
 *  user functions.  used by control.c or DSI implementation if it choses.
 */
#include "globus_i_gridftp_server.h"

#define FTP_SERVICE_NAME "file"

static
char * 
globus_gfs_acl_cas_action_to_string(
    globus_gfs_acl_action_t             action)
{
    switch(action)
    {
        case GFS_ACL_ACTION_DELETE:
            return "delete";
        case GFS_ACL_ACTION_WRITE:
            return "write";
        case GFS_ACL_ACTION_CREATE:
            return "create";
        case GFS_ACL_ACTION_READ:
            return "read";
        case GFS_ACL_ACTION_LOOKUP:
            return "lookup";
        case GFS_ACL_ACTION_AUTHZ_ASSERT:
            return "authz_assert";
        default:
            return NULL;
    }
}

static
void
globus_gfs_acl_cas_cb(
    void *                              callback_arg,
    globus_gsi_authz_handle_t           handle,
    globus_result_t                     result)
{
    globus_i_gfs_acl_handle_t *         acl_handle;
    GlobusGFSName(globus_gfs_acl_cas_cb);
    GlobusGFSDebugEnter();

    acl_handle = (globus_i_gfs_acl_handle_t *) callback_arg;
    globus_gfs_acl_authorized_finished(acl_handle, result);

    GlobusGFSDebugExit();
}

static
int
globus_gfs_acl_cas_init(
    void **                             out_handle,
    globus_gfs_acl_info_t *             acl_info,
    globus_gfs_acl_handle_t             acl_handle,
    globus_result_t *                   out_res)
{
    globus_gsi_authz_handle_t           cas_handle;
    GlobusGFSName(globus_gfs_acl_cas_init);
    GlobusGFSDebugEnter();

    if(acl_info->context == NULL)
    {
        goto err;
    }
    *out_res = globus_gsi_authz_handle_init(
        &cas_handle,
        FTP_SERVICE_NAME,
        acl_info->context,
        globus_gfs_acl_cas_cb,
        acl_handle);
    if(*out_res != GLOBUS_SUCCESS)
    {
        goto err;
    }
    *out_handle = cas_handle;
    GlobusGFSDebugExit();
    return GLOBUS_GFS_ACL_WOULD_BLOCK;

  err:
    GlobusGFSDebugExitWithError();
    return GLOBUS_GFS_ACL_COMPLETE;
}

static
int
globus_gfs_acl_cas_authorize(
    void *                              out_handle,
    globus_gfs_acl_action_t             action,
    globus_gfs_acl_object_desc_t *      object,
    globus_gfs_acl_info_t *             acl_info,
    globus_gfs_acl_handle_t             acl_handle,
    globus_result_t *                   out_res)
{
    globus_gsi_authz_handle_t           cas_handle;
    char *                              full_object;
    char *                              action_str;
    GlobusGFSName(globus_gfs_acl_cas_authorize);
    GlobusGFSDebugEnter();

    cas_handle = (globus_gsi_authz_handle_t) out_handle;
    if(acl_info->context == NULL)
    {
        goto err;
    }

    action_str = globus_gfs_acl_cas_action_to_string(action);
    if(!action_str)
    {
        *out_res = GLOBUS_SUCCESS;
        GlobusGFSDebugExit();
        return GLOBUS_GFS_ACL_COMPLETE;
    }
    
    /*
     * If the action is "authz_assert" then the object contains the assertions
     * received over the gridftp control channel - just pass it unmodified to
     * the authz callout
     */
    if(action == GFS_ACL_ACTION_AUTHZ_ASSERT)
    {
        full_object = globus_libc_strdup(object->name);
    }
    else
    {
        full_object = globus_common_create_string(
            "ftp://%s%s", acl_info->hostname, object->name);
    }    

    *out_res = globus_gsi_authorize(
        cas_handle,
        action_str,
        full_object,
        globus_gfs_acl_cas_cb,
        acl_handle);
    globus_free(full_object);
    if(*out_res != GLOBUS_SUCCESS)
    {
        goto err;
    }

    GlobusGFSDebugExit();
    return GLOBUS_GFS_ACL_WOULD_BLOCK;

  err:
    GlobusGFSDebugExitWithError();
    return GLOBUS_GFS_ACL_COMPLETE;
}

static
void
globus_gfs_acl_cas_destroy_cb(
    void *                              callback_arg,
    globus_gsi_authz_handle_t           handle,
    globus_result_t                     result)
{
    GlobusGFSName(globus_gfs_acl_cas_destroy_cb);
    GlobusGFSDebugEnter();


    GlobusGFSDebugExit();
}

static
void
globus_gfs_acl_cas_destroy(
    void *                              out_handle)
{
    globus_gsi_authz_handle_t           cas_handle;
    GlobusGFSName(globus_gfs_acl_cas_destroy);
    GlobusGFSDebugEnter();

    cas_handle = (globus_gsi_authz_handle_t) out_handle;
    globus_gsi_authz_handle_destroy(
        cas_handle, globus_gfs_acl_cas_destroy_cb, NULL);

    GlobusGFSDebugExit();
}

globus_gfs_acl_module_t                 globus_gfs_acl_cas_module = 
{
    globus_gfs_acl_cas_init,
    globus_gfs_acl_cas_authorize,
    globus_gfs_acl_cas_destroy,
    NULL
};

