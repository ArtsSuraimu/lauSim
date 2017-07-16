/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-30 12:59:50
*/

#ifndef PLUGIN_INTERNAL_H
#define PLUGIN_INTERNAL_H

#include <vector>

#include "library.h"
#include "../include/lausim/plugin.h"

namespace lauSim {

#define plugin_manager_version 1

class plugin_manager {
public:
    int init();
    int load_library(char *file, int argc, char **opts);
    int register_plugin(plugin *p);
    plugin * get_by_name(const char *name);
    plugin * get_by_type(plugin_type_t type);
    int unregister_plugin(const char *name);
    void cleanup();
private:
    bool is_init;
    std::vector<plugin*> plugins;
    std::vector<library> libraries;
};

}

#endif