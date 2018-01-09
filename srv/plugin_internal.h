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

/**
 * Handles initializing of and access to plugins
 */ 
class plugin_manager {
public:
    /**
     * enforces a singleton pattern so there only exists one plugin manager at a time
     */
    static plugin_manager *get_instance();
    int init();
    /**
     * loads a library potentially containing plugins and calls its init function
     * 
     * @param file The shared object to be loaded
     * @param argc the number of arguments provided to the init function
     * @param argv the arguments provided to the init functions
     * @return 0 on success
     */
    int load_library(char *file, int argc, char **opts);
    /**
     * registers a plugin
     * 
     * usually called by the init function of a shared object
     * @param the plugin to be registerd
     * @return 0 on success
     */
    int register_plugin(plugin *p);
    /**
     * retreive a plugin by name
     * @return a pointer to the plugin struct on success or a nullptr
     */
    plugin * get_by_name(const char *name);
    /**
     * retreive a plugin by type
     * 
     * the type field can be an "or" of different types. The first plugin found that fullfills all mentioned types is returned.
     * @return a pointer to the plugin struct on success or a nullptr
     */
    plugin * get_by_type(plugin_type_t type);
    /**
     * enumerates all loaded plugins with the role they are currently used in
     * 
     * the role is always a subset of type of the plugin
     * @param pl pointer will be overwritten with a filled array of plugins with attached roles
     * @return the number of plugins
     */
    unsigned all_plugins(pl_role **pl);
    /**
     * adds a role, a plugin is currently used in
     * @param p pointer to the plugin struct
     * @return 0 on success
     */
    int add_role(plugin *p, plugin_type_t new_role);
    /**
     * sets the role a plugin is currently used in
     * @param p pointer to the plugin struct
     * @return 0 on sucess
     */
    int set_role(plugin *p, plugin_type_t new_role);
    /**
     * unregisters a plugin
     * @param name The name of the plugin
     */
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