/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-30 13:01:05
*/

#include <iostream>
#include <unistd.h>

#include <lausim/plugin.h>
#include "plugin_internal.h"
#include "config.h"

using namespace lauSim;

plugin_manager plugins;
config conf;

char *cfile = nullptr;

char optstr[] = "c:";

int main(int argc, char **argv) {
    int opt;

    while((opt = getopt(argc, argv, optstr)) > 0) {
        switch(opt) {
        case 'c':
            cfile = optarg;
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

    if (plugins.init()) 
        std::cout << "[FATAL] unable to initialize plugin system" << std::endl;

    if (conf.load_config(cfile, plugins))
        std::cout << "[FATAL] error during initialization" << std::endl;

    if (conf.logger != nullptr)
        plugins.logger_used = conf.logger->lf();

    plugins.logger_used->log_fun(LL_Info, "initialization complete");

    for (plugin *p: plugins.plugins) {
        if (p->post_init == nullptr)
            continue;
        if (p->post_init() == 0) {
            std::cout << "[FATAL] error calling post init hook for " << p->name << std::endl;
        }
    }

    plugins.logger_used->log_fun(LL_Info, "cleaning up");
    plugins.cleanup();
}