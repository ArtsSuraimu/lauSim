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
 * This class manages the individual plugins. It is used to load and initialze them
 * Lets you search for plugins and assigns roles to them.
 */ 
class plugin_manager {
public:
    /**
     * enforces a singleton pattern so there only exists one plugin manager at a time
     */
    static plugin_manager *get_instance();
    /**
     * This function is used to load a library potentially containing plugins.
     * After the shared object is loaded using the Library class, it's init methode
     * is called with copies of the arguments provided to this funciton.
     * 
     * @param file The shared object to be loaded
     * @param argc the number of arguments provided to the init function
     * @param argv the arguments provided to the init functions
     * @return 0 on success
     */
    int load_library(char *file, int argc, char **argv);
    /**
     * This function registers a plugin in the plugin manager. It is typically called
     * by the init function of a dynamic library containing the plugin to be registered.
     * A plugin should be registered even if not all it's dependencies neccesarly are loaded
     * and a second stage initialization should be done in these cases using the post init function
     * of the plugin
     * 
     * @param p the plugin to be registerd
     * @return 0 on success
     */
    int register_plugin(plugin *p);
    /**
     * retreive a plugin by name
     * 
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
     * This function is called to enumerate all plugins that are loaded and registered in the plugin manager.
     * For each plugin it also returns the role the plugin is used in. This role is always a subset of the
     * capabilities the plugin has. A plugin only doing communications for example will never be listed as a logger.
     * 
     * @param pl pointer will be overwritten with a filled array of plugins with attached roles
     * @return the number of plugins
     */
    unsigned all_plugins(pl_role **pl);
    /**
     * finds a plugin by the role it is used in. Multiple roles can be specified.
     * The first plugin, that is used in ALL these roles is returned
     * 
     * @param p pointer to the plugin struct
     * @param new_role the roles that plugin is now additionally used in
     * @return 0 on success
     */
    int add_role(plugin *p, plugin_type_t new_role);
    /**
     * sets the role a plugin is currently used in. It has to be a subset of the capabilities of the plugin
     * else the call will faile
     * 
     * @param p pointer to the plugin struct
     * @param new_role The roles the plugin is now exclusively used in
     * @return 0 on sucess
     */
    int set_role(plugin *p, plugin_type_t new_role);

    /**
     * destroys the plugin manager, calling the cleanup function of all plugins and unloading all libraries.
     */
    void cleanup();
    /**
     * pointer to the logger used.
     */
    logger * logger_used;
    /**
     * Stores a mapping between plugins and the role they are used in
     */
    std::map<plugin*, plugin_type_t> plugins;
private:
    plugin_manager();
    bool is_init;
    /**
     * vector holding the shared objects containing the plugins
     */
    std::vector<std::unique_ptr<library>> libraries;
};

}

#endif