/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-30 13:01:05
*/

#include <iostream>
#include <unistd.h>
#include <cassert>
#include <cstdlib>

#include <lausim/plugin.h>
#include <lausim/node.h>
#include "plugin_internal.h"
#include "config.h"

using namespace lauSim;

plugin_manager plugins;
fault_manager *manager;
config conf;

char *cfile = nullptr;
size_t num_nodes;
node **nodes;
char optstr[] = "c:l:";

void update_log_level (unsigned new_ll) {
    log_level ll;
    switch (new_ll) {
    case 0:
        ll = LL_None;
        break;
    case 1:
        ll = LL_Debug;
        break;
    case 2:
        ll = LL_Info;
        break;
    case 3:
        ll = LL_Warning;
        break;
    case 4:
        ll = LL_Error;
        break;
    case 5:
        ll = LL_Fatal;
        break;
    }
    plugins.logger_used->set_ll(ll);
}

int init(unsigned loglevel) {
    if (plugins.init()){ 
        // since the plugin manager has not been properly initialize, there is no logger available
        std::cout << "[FATAL] unable to initialize plugin system" << std::endl;
        return 1;
    }

    // plugin manager has been initialized, from now on logger_used is set
    update_log_level(loglevel);

    if (conf.load_config(cfile, plugins)){
        plugins.logger_used->log_fun(LL_Fatal, "error while loading config");
        return 1;
    }

    if (conf.logger != nullptr) {
        plugins.logger_used = conf.logger->lf();
        update_log_level(loglevel);
    }
    
    plugins.logger_used->log_fun(LL_Debug, "config loaded successfully");

    // all plugins have been loaded, run post init hooks now

    for (plugin *p: plugins.plugins) {
        if (p->post_init == nullptr)
            continue;
        if (p->post_init() != 0) {
            plugins.logger_used->log_fun(LL_Fatal, (std::string("error calling post init hook for ") + std::string(p->name)).c_str());
            return 1;
        }
    }

    // check if the required plugins are configured
    if (conf.com_actor == nullptr ||
            conf.com_notify == nullptr ||
            conf.manager == nullptr ||
            conf.tic_length == 0){
        plugins.logger_used->log_fun(LL_Fatal, "configuration incomplete");
        return 1;
    }

    manager = conf.manager->fm();
    if (manager->get_nodes(&num_nodes, &nodes)) {
        plugins.logger_used->log_fun(LL_Fatal, "unable to get nodes");
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    int opt;
    unsigned loglevel = 0;

    // read command line options
    while((opt = getopt(argc, argv, optstr)) > 0) {
        switch(opt) {
        case 'c':
            cfile = optarg;
            break;
        case 'l':
            loglevel = (unsigned) strtoul(optarg, NULL, 0);
            break;
        default:
            std::cout << "unrecognized option '" << (char) opt << "'" << std::endl;
            return -1;
        }
    }

    if (cfile == nullptr) {
        std::cout << "usage: " << ((argc > 0) ? argv[0] : "lauSim") << " -c <file>" << std::endl;
        return -1;
    }

    if (init(loglevel))
        return 1;

    plugins.logger_used->log_fun(LL_Info, "initialization complete");

    plugins.logger_used->log_fun(LL_Info, "cleaning up");
    plugins.cleanup();
}