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
#include <sstream>
#include <thread>

#include <lausim/plugin.h>
#include <lausim/node.h>

#include "plugin_internal.h"
#include "config.h"
#include <laik_ext.pb.h>
#include <backchannel.pb.h>
#include "server.h"

using namespace lauSim;

// time keeping

std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
std::chrono::nanoseconds duration;

Server *srv_instance = nullptr;

int Server::init(unsigned loglevel) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    conf = Config::get_instance();
    plugins = plugin_manager::get_instance();

    tic = 0;

    // plugin manager has been initialized, from now on logger_used is set
    
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
    if (conf->com_actor == nullptr ||
            conf->com_notify == nullptr ||
            conf->fault_manager == nullptr){
        plugins->logger_used->log_fun(LL_Fatal, "configuration incomplete");
        return 1;
    }

    com_actor = conf->com_actor->c();
    com_notify = conf->com_notify->c();

    if ((com_actor->capabilities & COM_ASYNC) == COM_ASYNC) {
        plugins->logger_used->log_fun(LL_Debug, "asynchronous backchannel");
        com_actor->set_callback(on_message);
    }

    manager = conf->fault_manager->fm();
    if (manager->get_nodes(&num_nodes, &nodes)) {
        plugins->logger_used->log_fun(LL_Fatal, "unable to get nodes");
        return 1;
    }

    duration = std::chrono::nanoseconds(conf->tic_length);

    return 0;
}

bool Server::all_nodes_ready() {
    size_t i;
    for (i = 0; i < num_nodes; i++) {
        if (nodes[i]->state != NODE_STATE_READY)
            return false;
    }
    return true;
}

int Server::set_node_state(const char *name, unsigned state) {
    size_t i;

    for (i = 0; i < num_nodes; i++) {
        if (strcmp(name, nodes[i]->name) == 0) {
            nodes[i]->state = state;
            return 0;
        }
    }

    return 1;
}

extern "C" void Server::on_message(uint8_t *msg, size_t len) {
    char buf[0x80];
    const char * status;
    unsigned state = NODE_STATE_UNKNOWN;
    if (srv_instance == nullptr)
        return;
    Backchannel back;
    if (!back.ParseFromString(std::string((char *)msg, len))) {
        srv_instance->plugins->logger_used->log_fun(LL_Warning, "unable to decode message on backchannel");
        return;
    }
    switch (back.type()) {
        case Backchannel::LOG:
            if (!back.has_logerrmsg())
                break;
            srv_instance->plugins->logger_used->log_fun(LL_Info, back.logerrmsg().c_str());
            break;
        case Backchannel::ERROR:
            if (!back.has_logerrmsg())
                break;
            srv_instance->plugins->logger_used->log_fun(LL_Error, back.logerrmsg().c_str());
            break;
        case Backchannel::STATUS:
            if (!back.has_newstatus()) {
                srv_instance->plugins->logger_used->log_fun(LL_Warning, "status message did not contain a status");    
                break;
            }
            
            switch (back.newstatus()) {
            case Backchannel::STREADY:
                status = "ready";
                state = NODE_STATE_READY;
                break;
            case Backchannel::STERROR:
                status = "error";
                state = NODE_STATE_ERROR;
                break;
            case Backchannel::STFINISHED:
                status = "finished";
                state = NODE_STATE_FINISHED;
                break;
            default:
                status = "unknown status";
                break;
            }
            
            srv_instance->set_node_state(back.nodename().c_str(), state);
            snprintf(buf, sizeof(buf) - 1, "[%s] new status: %s", back.nodename().c_str(), status);
            srv_instance->plugins->logger_used->log_fun(LL_Info, buf);
            break;
        default:
            if (srv_instance->other_cb != nullptr)
                srv_instance->other_cb(msg, len);
    }
}

void Server::skip_to_end_of_tic() {
    unsigned skip = 0;
    tic++;

    if (conf->tic_length == 0)
        return;

    for (timestamp += duration; timestamp < std::chrono::high_resolution_clock::now(); timestamp += duration)
        skip++;

    if (skip) 
        plugins->logger_used->log_fun(LL_Warning, (std::to_string(skip) + std::string(" tics skipped - increase tic time?")).c_str());
    
    std::this_thread::sleep_until(timestamp);
    tic += skip;
}

size_t Server::get_messages(Backchannel ***bck) {
    return 0;
}

Server *Server::get_instance() {
    if (srv_instance == nullptr)
        srv_instance = new Server();
    return srv_instance;
}

void Server::send_synced_signal() {
    size_t i;
    for (i = 0; i < num_nodes; i++)
        com_actor->notify_fail(nodes[i]->name, NULL, 0);
}

int Server::do_tic() {
    if (tic == 0)
        timestamp = std::chrono::high_resolution_clock::now();
    manager->tic();
    manage_fails();
    skip_to_end_of_tic();
    return 0;
}

void Server::cleanup() {
    if (srv_instance != nullptr)
        delete srv_instance;
}

void Server::manage_fails() {
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