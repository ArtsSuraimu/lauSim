/*
 * @Author: C. Jonischkeit
 * @Date: 2017-06-26 14:20:17 
 * @Last Modified by: D. Yang
 * @Last Modified time: 2017-06-26 14:21:24
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include "com.h"
#include "failure_manager.h"
#include "types.h"

#define PL_INTF_VERSION 1

#ifdef __cpluplus
namespace lauSim {
extern "C" {
#endif

typedef enum tag_plugin_type{
    PL_Unknown = 0,
    PL_COM,
    PL_MATH,
    PL_FAILURE_MANAGER
}plugin_type_t;

typedef struct tag_plugin{
    int version;
    plugin_type_t type;

    init init_func;
    cleanup cleanup_func;

} plugin;

/* cast these structs to plugin, to pseudo inherit from plugin */
typedef struct tag_com_plugin{
    plugin hdr;
    com c;
} com_plugin;

typedef struct tag_math_plugin{
    plugin hdr;
    math m;
} math_plugin;

typedef struct tag_failure_plugin{
    plugin hdr;
    failure_manager fm;
} failure_manager_plugin;

#ifdef __cpluplus
class plugin{
    public:
        

    private: 
        void* plugin_handles;
    
}
}
}
#endif

#endif