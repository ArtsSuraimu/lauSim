/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-30 13:01:05
*/

#include <iostream>
#include <unistd.h>
#include <cassert>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>

#include <lausim/plugin.h>
#include <lausim/node.h>
#include "plugin_internal.h"
#include "config.h"

using namespace lauSim;

// time keeping

std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
std::chrono::nanoseconds duration;

// configuration

plugin_manager plugins;
fault_manager *manager;
com *com_actor;
com *com_notify;
config conf;

size_t num_nodes;
node **nodes;

// initialization

char *cfile = nullptr;
char optstr[] = "c:l:";
volatile std::sig_atomic_t termreq;

void term_handler(int signal){
    termreq = 1;
}

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

    com_actor = conf.com_actor->c();
    com_notify = conf.com_notify->c();

    manager = conf.manager->fm();
    if (manager->get_nodes(&num_nodes, &nodes)) {
        plugins.logger_used->log_fun(LL_Fatal, "unable to get nodes");
        return 1;
    }

    termreq = 0;
    std::signal(SIGTERM, term_handler);
    std::signal(SIGABRT, term_handler);
    std::signal(SIGINT, term_handler);
    duration = std::chrono::nanoseconds(conf.tic_length);
    timestamp = std::chrono::high_resolution_clock::now();

    return 0;
}

void skip_to_end_of_tic() {
    unsigned skip = 0;

    for (timestamp += duration; timestamp < std::chrono::high_resolution_clock::now(); timestamp += duration)
        skip++;

    if (skip) 
        plugins.logger_used->log_fun(LL_Warning, (std::to_string(skip) + std::string(" tics skipped - increase tic time?")).c_str());
    
    std::this_thread::sleep_until(timestamp);
}

int main(int argc, char **argv) {
    char buf[512];
    int opt;
    unsigned loglevel = 0;
    fault **failed;
    int i, num_failed;

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
            std::cerr << "unrecognized option '" << (char) opt << "'" << std::endl;
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

    while(!termreq) {
        manager->tic();
        num_failed = manager->get_fail(&failed);

        for (i = 0; i < num_failed ; i++) {
            com_actor->notify_fail(failed[i]->node, failed[i]->component, failed[i]->severity);
            if (failed[i]->component == NULL) {
                snprintf(buf, sizeof(buf), "Node %s %s", failed[i]->node, (failed[i]->severity) ? "failed" : "recovered");
                plugins.logger_used->log_fun(LL_Info, buf);
                // TODO send fail via com
            }
        }

        skip_to_end_of_tic();
    }

    plugins.logger_used->log_fun(LL_Info, "cleaning up");
    plugins.cleanup();
}