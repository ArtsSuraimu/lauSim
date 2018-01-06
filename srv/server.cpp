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
#include "options.h"
#include "plugin_internal.h"
#include "config.h"
#include "proto/laik_ext.pb.h"

using namespace lauSim;

// time keeping

std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
std::chrono::nanoseconds duration;

// configuration

plugin_manager *plugins;
fault_manager *manager;
com *com_actor;
com *com_notify;
config conf;

size_t num_nodes;
node **nodes;

// initialization

volatile std::sig_atomic_t termreq;
Options opts;

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
    plugins->logger_used->set_ll(ll);
}

int init(unsigned loglevel) {

    plugins = plugin_manager::get_instance();
    if (plugins->init()){ 
        // since the plugin manager has not been properly initialize, there is no logger available
        std::cout << "[FATAL] unable to initialize plugin system" << std::endl;
        return 1;
    }

    // plugin manager has been initialized, from now on logger_used is set
    update_log_level(loglevel);

    if (conf.load_config(opts.cfile.c_str(), plugins)){
        plugins->logger_used->log_fun(LL_Fatal, "error while loading config");
        return 1;
    }

    if (conf.logger != nullptr) {
        plugins->logger_used = conf.logger->lf();
        update_log_level(loglevel);
    }
    
    plugins->logger_used->log_fun(LL_Debug, "config loaded successfully");

    // all plugins have been loaded, run post init hooks now

    for (auto &plu: plugins->plugins) {
        if (plu.first->post_init == nullptr)
            continue;
        if (plu.first->post_init() != 0) {
            plugins->logger_used->log_fun(LL_Fatal, (std::string("error calling post init hook for ") + std::string(plu.first->name)).c_str());
            return 1;
        }
    }

    // check if the required plugins are configured
    if (conf.com_actor == nullptr ||
            conf.com_notify == nullptr ||
            conf.manager == nullptr){
        plugins->logger_used->log_fun(LL_Fatal, "configuration incomplete");
        return 1;
    }

    com_actor = conf.com_actor->c();
    com_notify = conf.com_notify->c();

    manager = conf.manager->fm();
    if (manager->get_nodes(&num_nodes, &nodes)) {
        plugins->logger_used->log_fun(LL_Fatal, "unable to get nodes");
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

    if (conf.tic_length == 0)
        return;

    for (timestamp += duration; timestamp < std::chrono::high_resolution_clock::now(); timestamp += duration)
        skip++;

    if (skip) 
        plugins->logger_used->log_fun(LL_Warning, (std::to_string(skip) + std::string(" tics skipped - increase tic time?")).c_str());
    
    std::this_thread::sleep_until(timestamp);
}

void manage_fails(unsigned long tic) {
    char buf[512];
    std::string protoStrBuf;
    laik_ext_msg fail_msg;
    fault **failed;
    int i, num_failed;

    num_failed = manager->get_fail(&failed);

    if (num_failed) {
        snprintf(buf, sizeof(buf), "Tic #%lu", tic);
        plugins->logger_used->log_fun(LL_Info, buf);
        fail_msg.Clear();

        for (i = 0; i < num_failed ; i++) {
            com_actor->notify_fail(failed[i]->node, failed[i]->component, failed[i]->severity);
            if (failed[i]->component == NULL) {
                snprintf(buf, sizeof(buf), "Node %s %s", failed[i]->node, (failed[i]->severity) ? "failed" : "recovered");
                plugins->logger_used->log_fun(LL_Info, buf);

                // send fail message
                if (failed[i]->severity == 0) {
                    fail_msg.add_spare_nodes(failed[i]->node, strlen(failed[i]->node));
                } else {
                    fail_msg.add_failing_nodes(failed[i]->node, strlen(failed[i]->node));
                }
            }
        }

        fail_msg.SerializeToString(&protoStrBuf);
        com_notify->notify_extern(protoStrBuf.c_str(), protoStrBuf.length());
    }
}

int main(int argc, char **argv) {
    unsigned long tic_num = 0;
    
    if (opts.parse(argc, argv) != 0)
        return -1;

    if (opts.cfile.empty()) {
        std::cout << "usage: " << ((argc > 0) ? argv[0] : "lauSim") << " -c <file>" << std::endl;
        return -1;
    }

    if (init(opts.loglevel))
        return 1;

    plugins->add_role(conf.com_actor, PL_COM_ACTOR);
    plugins->add_role(conf.com_notify, PL_COM_EXTERN);
    plugins->add_role(conf.logger, PL_LOGGER);
    plugins->add_role(conf.manager, PL_FAULT_MANAGER);

    plugins->logger_used->log_fun(LL_Info, "initialization complete");

    while(!termreq) {
        manager->tic();
        manage_fails(tic_num);
        ++tic_num;
        skip_to_end_of_tic();
    }

    google::protobuf::ShutdownProtobufLibrary();
    plugins->logger_used->log_fun(LL_Info, "cleaning up");
    plugins->cleanup();
}