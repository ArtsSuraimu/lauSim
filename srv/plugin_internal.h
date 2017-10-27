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