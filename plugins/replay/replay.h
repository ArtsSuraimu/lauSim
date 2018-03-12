/*
   Copyright 2018 Clemens Jonischkeit

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-07-18
*/

#ifndef REPLAY_H
#define REPLAY_H

#include <stdlib.h>

#include <lausim/node.h>
#include <lausim/fault_manager.h>
#include <lausim/plugin.h>

fault_manager *get_fm(void);
int init (const plugin_manager_interface*, int argc, char **argv);
int cleanup(void);
int post(void);

plugin replay_plugin = {
    1,          // version
    PL_FAULT_MANAGER,          // pl_type fault_manager
    "replay",   // name

    post,       // post init hook
    cleanup,    // cleanup
    NULL,       // get_com_fun
    get_fm,     // get_fault_fun
    NULL        // logger
};

#endif