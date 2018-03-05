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
 * @Date: 2018-03-05
*/

#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <stdlib.h>

#include <lausim/node.h>
#include <lausim/fault_manager.h>
#include <lausim/plugin.h>

int init (const plugin_manager_interface*, int argc, char **argv);
int cleanup(void);
int post(void);
logger *get_logger(void); 

plugin filelogger_plugin = {
    .version = 1,           // version
    .pl_type = PL_LOGGER,   // pl_type fault_manager
    .name = "filelogger",   // name

    .post_init = post,      // post init hook
    .cleanup = cleanup,     // cleanup
    .c = NULL,              // get_com_fun
    .fm = NULL,             // get_fault_fun
    .lf = get_logger        // logger
};

#endif