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

typedef struct tag_plugin{
    int version;
    plugin_type_t pl_type;
    char *name;

    post_init_fun post_init;
    cleanup_fun cleanup;
    get_com_fun c;
    get_fault_manager_fun fm;
    get_logger_fun lf;
} plugin;

typedef struct tag_pl_role{
    plugin_type_t role;
    plugin *pl;
} pl_role;

typedef struct {
    int version;
    int (*register_plugin) (plugin *);
    plugin *(*by_name) (const char *name);
    plugin *(*by_type) (plugin_type_t type);
    logger *(*get_logger) ();
    unsigned (*all_plugins) (pl_role **pl);
    int (*set_role) (plugin *, plugin_type_t new_role);
    int (*add_role) (plugin *, plugin_type_t new_role);
} plugin_manager_interface;

typedef int (*init_fun) (const plugin_manager_interface*, int argc, char **argv);

#ifdef __cplusplus
}
}
#endif

#endif
