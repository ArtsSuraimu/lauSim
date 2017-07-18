/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-30 13:00:05
*/

#include <functional>
#include <algorithm>
#include <cstring>
#include <iostream>

#include <lausim/logger.h>
#include <lausim/plugin.h>
#include "plugin_internal.h"

using namespace lauSim;

void log_stdout(log_level l, char *str) {
    const char * prefix;
    switch (l) {
    default:
    case LL_None:
        prefix = "[NONE] ";
        break;
    case LL_Debug:
        prefix = "[DEBUG]";
        break;
    case LL_Info:
        prefix = "[INFO] ";
        break;
    case LL_Warning:
        prefix = "[WARN] ";
        break;
    case LL_Error:
        prefix = "[ERROR]";
        break;
    case LL_Fatal:
        prefix = "[FATAL]";
        break;
    }

    std::cout << prefix << " " << str << std::endl;
}

logger standard_log = {
    1,
    log_stdout
};

std::function<int (plugin*)> register_plugin_fwd = [](plugin*) {return -1;};
std::function<plugin *(const char *)> by_name_fwd = [](const char*){return nullptr;};
std::function<plugin *(plugin_type_t)> by_type_fwd = [](plugin_type_t){return nullptr;};
std::function<logger *()> get_logger_fwd = [](){return &standard_log;};

int register_plugin_c(plugin *p) {
    return register_plugin_fwd(p);
}

plugin *by_name_c(const char *name) {
    return by_name_fwd(name);
}

plugin *by_type_c(plugin_type_t pt) {
    return by_type_fwd(pt);
}

logger *get_logger() {
    return get_logger_fwd();
}

const plugin_manager_interface interface = {
    plugin_manager_version,
    register_plugin_c,
    by_name_c,
    by_type_c,
    get_logger
};

int plugin_manager::init() {
    if (is_init)
        return -1;
    logger_used = &standard_log;
    register_plugin_fwd = [this](plugin *p){return this->register_plugin(p);};
    by_name_fwd = [this](const char *name) {return this->get_by_name(name);};
    by_type_fwd = [this](plugin_type_t type) {return this->get_by_type(type);};
    get_logger_fwd = [this]() {return logger_used;};
    is_init = true;
    return 0;
}

int plugin_manager::load_library(char *file, int argc, char **argv) {
    library nlib;
    init_fun init;

    nlib.argc = argc;
    

    if (nlib.open(file))
        return -1;
    
    init = (init_fun) nlib.get_sym_addr("init");
    if (init == nullptr)
        return -2;
    
    // create a copy of the arguments
    nlib.argv = new char*[argc];
    for (int i = 0; i < argc; i ++) {
        nlib.argv[i] = new char[strlen(argv[i]) + 1];
        strcpy(nlib.argv[i], argv[i]);
    }

    if (init(&interface, argc, nlib.argv))
        return -3;
    
    libraries.push_back(nlib);
    return 0;
}

int plugin_manager::register_plugin(plugin *p){
    if (this->get_by_name(p->name) != nullptr)
        return -1;
    plugins.push_back(p);
    return 0;
}

plugin* plugin_manager::get_by_name(const char *name) {
    auto pos = std::find_if(plugins.begin(), plugins.end()
                            , [=](const plugin *p){return 0 == strcmp(name, p->name);});
    if (pos == plugins.end())
        return nullptr;
    else return *pos;
}

plugin* plugin_manager::get_by_type(plugin_type_t type) {
    auto pos = std::find_if(plugins.begin(), plugins.end()
                            , [=](const plugin *p){return (p->pl_type & type) == type;});
    if (pos == plugins.end())
        return nullptr;
    else return *pos;
}
