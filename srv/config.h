/*
   Copyright 2017 Clemens Jonischkeit

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
 * @Date: 2017-07-17 13:00:25
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <sstream>

#include "../include/lausim/plugin.h"
#include "plugin_internal.h"

namespace lauSim {

#define com_actor_id 0x100
#define com_notify_id 0x101
#define logger_id 0x102
#define manager_id 0x103
#define com_all_id 0x104
#define tic_id 0x105

enum backChType {
    BC_NONE,
    BC_SYNC,
    BC_ASYNC
};

class config {
public:
    int load_config(const char * filename, plugin_manager *manager);
    int set_config(int id, std::stringstream &, plugin_manager *);
    int load_lib(const std::string &, std::stringstream &, plugin_manager *);
    plugin *com_actor = nullptr;
    plugin *com_notify = nullptr;
    plugin *manager = nullptr;
    plugin *logger = nullptr;
    uint64_t tic_length = 0;
    backChType backCh;
};

}
#endif
