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
 * @Date: 2017-06-30 13:00:05
*/

#include <functional>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <dlfcn.h>
#include <lausim/logger.h>
#include <lausim/plugin.h>
#include "stdoutLog.h"
#include "plugin_internal.h"

using namespace lauSim;

plugin_manager *instance = nullptr;

plugin_manager *plugin_manager::get_instance() {
    if (instance == nullptr)
        instance = new plugin_manager();

    return instance;
}

plugin_manager::plugin_manager() {
    logger_used = &standard_log;
    is_init = true;
}

int register_plugin_c(plugin *p) {
    return instance->register_plugin(p);
}

plugin *by_name_c(const char *name) {
    return instance->get_by_name(name);
}

plugin *by_type_c(plugin_type_t pt) {
    return instance->get_by_type(pt);
}

logger *get_logger() {
    return instance->logger_used;
}

unsigned get_all_plugins(pl_role **p) {
    return instance->all_plugins(p);
}

int man_set_role(plugin *p, plugin_type_t nr) {
    return instance->set_role(p, nr);
}

int man_add_role(plugin *p, plugin_type_t nr) {
    return instance->add_role(p, nr);
}

const plugin_manager_interface interface = {
    PL_INTF_VERSION,
    register_plugin_c,
    by_name_c,
    by_type_c,
    get_logger,
    get_all_plugins,
    man_set_role,
    man_add_role
};

int plugin_manager::set_role(plugin *pl, plugin_type_t new_role) {
    if (pl == nullptr || (pl->pl_type & new_role) != new_role)
        return -1;

    plugins.at(pl) = new_role;
    return 0;
}

int plugin_manager::add_role(plugin *pl, plugin_type_t new_role) {
    if (pl == nullptr || (pl->pl_type & new_role) != new_role)
        return -1;
    
    plugins.at(pl) |= new_role;
    return 0;
}

int plugin_manager::load_library(char *file, int argc, char **argv) {
    std::unique_ptr<library> nlib(new library());
    init_fun init;
    const char *err;

    nlib->argc = argc;
    

    if (nlib->open(file)){
        err = dlerror();
        std::cout << "[FATAL] unable to load " << file << ": " << err << std::endl;
        return -1;
    }
    
    init = (init_fun) nlib->get_sym_addr("init");
    if (init == nullptr){
        std::cout << "[FATAL] unable to get symbol init in " << file << std::endl;
        return -2;
    }
    
    // create a copy of the arguments
    nlib->argv = new char*[argc];
    for (int i = 0; i < argc; i ++) {
        nlib->argv[i] = new char[strlen(argv[i]) + 1];
        strcpy(nlib->argv[i], argv[i]);
    }

    if (init(&interface, argc, nlib->argv)) {
        std::cout << "[FATAL] initialization of " << file << " failed" << std::endl;
        return -3;
    }
    
    libraries.push_back(std::move(nlib));
    return 0;
}

unsigned plugin_manager::all_plugins(pl_role **pl) {
    unsigned i = 0;
    pl_role *roles = (pl_role *) calloc(sizeof(pl_role), plugins.size());
    
    for (auto plu: plugins) {
        roles[i].role = plu.second;
        roles[i++].pl = plu.first;
    }

    *pl = roles;
    return i;
}

int plugin_manager::register_plugin(plugin *p){
    std::stringstream str;
    str << "trying to register new plugin " << p->name;
    logger_used->log_fun(LL_Debug, str.str().c_str());
    if (this->get_by_name(p->name) != nullptr || plugins.find(p) != plugins.end())
        return -1;
    plugins.emplace(p, PL_NONE);
    return 0;
}

plugin* plugin_manager::get_by_name(const char *name) {
    auto pos = std::find_if(plugins.begin(), plugins.end()
                            , [name](std::pair<plugin*, plugin_type_t> info){return 0 == strcmp(name, info.first->name);});
    if (pos == plugins.end())
        return nullptr;
    else return pos->first;
}

plugin* plugin_manager::get_by_type(plugin_type_t type) {
    auto pos = std::find_if(plugins.begin(), plugins.end()
                            , [type](std::pair<plugin*, plugin_type_t> info){return (info.second & type) == type;});
    if (pos == plugins.end())
        return nullptr;
    else return pos->first;
}

void plugin_manager::cleanup() {
    int i;
    for (auto &p: plugins) {
        if (p.first->cleanup != nullptr)
            p.first->cleanup();
    }

    for (std::unique_ptr<library> &lib: libraries) {
        for (i = 0; i < lib->argc; i++) {
            delete[] lib->argv[i];
        }
        delete[] lib->argv;
    }

    libraries.clear();
}