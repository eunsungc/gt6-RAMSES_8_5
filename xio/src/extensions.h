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


 GlobusXIODeclareModule(file); GlobusXIODeclareModule(http); GlobusXIODeclareModule(mode_e); GlobusXIODeclareModule(ordering); GlobusXIODeclareModule(queue); GlobusXIODeclareModule(tcp); GlobusXIODeclareModule(telnet); GlobusXIODeclareModule(udp);

static globus_extension_builtin_t       local_extensions[] = 
{
 {GlobusXIOExtensionName(file), GlobusXIOMyModule(file)}, {GlobusXIOExtensionName(http), GlobusXIOMyModule(http)}, {GlobusXIOExtensionName(mode_e), GlobusXIOMyModule(mode_e)}, {GlobusXIOExtensionName(ordering), GlobusXIOMyModule(ordering)}, {GlobusXIOExtensionName(queue), GlobusXIOMyModule(queue)}, {GlobusXIOExtensionName(tcp), GlobusXIOMyModule(tcp)}, {GlobusXIOExtensionName(telnet), GlobusXIOMyModule(telnet)}, {GlobusXIOExtensionName(udp), GlobusXIOMyModule(udp)},
    {NULL, NULL}
};
