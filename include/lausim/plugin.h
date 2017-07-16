/*
 * @Author: C. Jonischkeit
 * @Date: 2017-06-26 14:20:17 
 * @Last Modified by: C. Jonischkeit
 * @Last Modified time: 2017-06-30 13:02:40
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include "com.h"
#include "fault_manager.h"
#include "types.h"

#define PL_INTF_VERSION 1

#ifdef __cpluplus
namespace lauSim {
extern "C" {
#endif

#define PL_COM 1
#define PL_FAULT_MANAGER 2
#define PL_LOGGER 3
#define PL_TOPOLOGY 4

#define SET_PL_TYPE(pl,t) = (pl |= t)
#define HAS_PL_TYPE(pl,t) = (pl & t)

typedef uint64_t plugin_type_t;
typedef int (*post_init_fun) ();
typedef int (*cleanup_fun) ();

typedef struct tag_plugin{
    int version;
    plugin_type_t pl_type;
    char *name;

    post_init_fun post_init;
    cleanup_fun cleanup;
    get_com_fun c;
    get_fault_manager_fun fm;
} plugin;

typedef struct {
    int version;
    int (*register_plugin) (plugin *);
    plugin *(*by_name) (const char *name);
    plugin *(*by_type) (plugin_type_t type);
} plugin_manager_interface;

typedef int (*init_fun) (const plugin_manager_interface*, int argc, char **argv);

#ifdef __cpluplus
}
}
#endif

#endif