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
 * @Date: 2017-06-30 12:59:50
*/

#ifndef PLUGIN_INTERNAL_H
#define PLUGIN_INTERNAL_H

#include <vector>
#include <map>
#include <memory>

#include "library.h"
#include <lausim/logger.h>
#include <lausim/plugin.h>

namespace lauSim {

#define plugin_manager_version 1

class plugin_manager {
public:
    static plugin_manager *get_instance();
    int init();
    int load_library(char *file, int argc, char **opts);
    int register_plugin(plugin *p);
    plugin * get_by_name(const char *name);
    plugin * get_by_type(plugin_type_t type);
    unsigned all_plugins(pl_role **pl);
    int add_role(plugin *p, plugin_type_t new_role);
    int set_role(plugin *p, plugin_type_t new_role);
    int unregister_plugin(const char *name);
    void cleanup();
    logger * logger_used;
    std::map<plugin*, plugin_type_t> plugins;
private:
    plugin_manager() = default;
    bool is_init;
    std::vector<std::unique_ptr<library>> libraries;
};

}

#endif