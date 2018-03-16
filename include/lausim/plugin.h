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
 * @Date: 2017-06-26 14:20:17 
 * @Last Modified by: C. Jonischkeit
 * @Last Modified time: 2017-06-30 13:02:40
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include <lausim/types.h>
#include <lausim/logger.h>
#include <lausim/com.h>
#include <lausim/fault_manager.h>
//#include <lausim/types.h>

#define PL_INTF_VERSION 7

#ifdef __cplusplus
namespace lauSim {
extern "C" {
#endif

#define PL_NONE 0
#define PL_COM_ACTOR 1
#define PL_COM_EXTERN 2
#define PL_FAULT_MANAGER 4
#define PL_LOGGER 8
#define PL_TOPOLOGY 16
#define PL_OTHER 32

#define SET_PL_TYPE(pl,t) ((pl).pl_type |= t)
#define HAS_PL_TYPE(pl,t) ((pl).pl_type & t)

/**
 * This struct describes a plugin
 */
typedef struct tag_plugin{
    /**
     * the version of the plugin
     */
    int version;
    /**
     * the type of the plugin, can be a composition of multiple basic types
     * tpye    | meaning
     * -------:|:----
     * PL_NONE | -
     * PL_COM_ACTOR | provides communictaion capabilities for the actors
     * PL_COM_EXTERN | provides communication capabilityies for external applications
     * PL_FAULT_MANAGER | provides a fault manager, that decides about the components and severities for faults
     * PL_LOGGER | provides a logging capabilities
     * PL_TOPOLOGY | works hand in hand with the fault manager
     * PL_OTHER | fullfills some other role
     */
    plugin_type_t pl_type;
    /**
     * name of the plugin
     */
    char *name;

    /**
     * function called once all plugins have been initialized
     */
    post_init_fun post_init;
    /**
     * cleanup function called before unloading the plugin
     */
    cleanup_fun cleanup;
    /**
     * get the communicaton backend
     */
    get_com_fun c;
    /**
     * get the fault manager
     */
    get_fault_manager_fun fm;
    /**
     * get the logger
     */
    get_logger_fun lf;
} plugin;

/**
 * a struct that assosiates a plugin with a role it is used in
 */
typedef struct tag_pl_role{
    /**
     * the role it is used in
     */
    plugin_type_t role;
    /**
     * the plugin
     */
    plugin *pl;
} pl_role;

/**
 * provides an interface to the plugin manager for plugins
 */
typedef struct {
    /**
     * version of the plugin manager
     */
    int version;
    /**
     * called to register a plugin
     * @param p the plugin to register
     * @return 0 on success
     */
    int (*register_plugin) (plugin *p);
    /**
     * finds a plugin by its name
     * @param name the name of the plugin to look for
     * @return the plugin on success, else NULL
     */
    plugin *(*by_name) (const char *name);
    /**
     * finds a plugin by the role it is used in. Multiple roles can be specified.
     * The first plugin, that is used in ALL these roles is returned
     */
    plugin *(*by_type) (plugin_type_t type);
    /**
     * get the logger used
     */
    logger *(*get_logger) ();
    /**
     * get a list of all loaded plugins
     * @param pl a pointer to a pointer that is populated by a plugin role association list
     * @return the number of plugins
     */
    unsigned (*all_plugins) (pl_role **pl);
    /**
     * sets the role a plugin is used in. This does not show the capabilities, but must be a
     * subset of the capabilities
     * @return 0 on success
     */
    int (*set_role) (plugin *, plugin_type_t new_role);
    /**
     * adds a usage role to a plugin. Must be a subset of the plugins capabilities.
     * @param new_role the role the plugin is now additionally used in
     * @return 0 on success
     */
    int (*add_role) (plugin *, plugin_type_t new_role);
} plugin_manager_interface;

typedef int (*init_fun) (const plugin_manager_interface*, int argc, char **argv);

#ifdef __cplusplus
}
}
#endif

#endif
