/*
 * Copyright 1999-2014 University of Chicago
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

#include "globus_net_manager.h"

/**
 * @brief Unregister a network manager
 * @ingroup globus_net_manager
 * @details
 * The globus_net_manager_unregister() function removes this network manager
 * from those which will be called by the network manager interface
 * when network events occur. This is typically called by the network
 * manager when its module is deactivated.
 * @param[in] manager
 *     Manager information to unregister.
 *
 */
globus_result_t
globus_net_manager_unregister(
    globus_net_manager_t               *manager)
{
    globus_extension_registry_remove(
        GLOBUS_NET_MANAGER_REGISTRY, (void *) manager->name);
    
    return GLOBUS_SUCCESS;
}
