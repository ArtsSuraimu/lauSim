
#include <atomic>
#include <csignal>
#include <iostream>
#include <chrono>
#include <thread>

#include "config.h"
#include "options.h"
#include "server.h"
#include "plugin_internal.h"
#include "stdoutLog.h"

using namespace lauSim;

volatile std::sig_atomic_t termreq;
size_t tic_num = 0;

void term_handler(int signal){
    termreq = 1;
}

int main(int argc, char **argv) {
    Options opts;
    Server *server;
    Config *conf;
    plugin_manager *plugins;
    pl_role *loaded;
    unsigned num;
    size_t nameLen, maxNameLen;
    char buf[512];
    
    if (opts.parse(argc, argv) != 0)
        return -1;

    if (opts.cfile.empty()) {
        std::cout << "usage: " << ((argc > 0) ? argv[0] : "lauSim") << " -c <file>" << std::endl;
        return -1;
    }

    server = Server::get_instance();
    conf = Config::get_instance();
    plugins = plugin_manager::get_instance();

    plugins->logger_used->set_ll(opts.loglevel);

    if (conf->load_config(opts.cfile.c_str())){
        plugins->logger_used->log_fun(LL_Fatal, "error while loading config");
        return 1;
    }

    if (conf->logger != nullptr) {
        plugins->logger_used = conf->logger->lf();
        plugins->logger_used->set_ll(opts.loglevel);
    }

    plugins->add_role(conf->com_actor, PL_COM_ACTOR);
    plugins->add_role(conf->com_notify, PL_COM_EXTERN);
    plugins->add_role(conf->logger, PL_LOGGER);
    plugins->add_role(conf->fault_manager, PL_FAULT_MANAGER);

    if (server->init(opts.loglevel))
        return 1;

    plugins->logger_used->log_fun(LL_Info, "initialization complete");

    termreq = 0;
    std::signal(SIGTERM, term_handler);
    std::signal(SIGABRT, term_handler);
    std::signal(SIGINT, term_handler);

    num = plugins->all_plugins(&loaded);

    maxNameLen = strlen(loaded[0].pl->name);
    for (unsigned i = 1; i < num; i++) {
        if ((nameLen = strlen(loaded[0].pl->name)) > maxNameLen)
            maxNameLen = nameLen;
    }

    snprintf(buf, sizeof(buf) - 1, "%-*s CA CE MA LO TO OT", (int) maxNameLen, "Name");
    plugins->logger_used->log_fun(LL_Info, buf);
    for (unsigned i = 0; i < num; i++) {
        snprintf(buf, sizeof(buf) - 1, "%-*s %c%c %c%c %c%c %c%c %c%c %c%c",
            (int) maxNameLen,
            loaded[i].pl->name,
            HAS_PL_TYPE((*loaded[i].pl), PL_COM_ACTOR) ? 'A' : '-',
            (loaded[i].role & PL_COM_ACTOR) ? 'U' : '-',
            HAS_PL_TYPE((*loaded[i].pl), PL_COM_EXTERN) ? 'A' : '-',
            (loaded[i].role & PL_COM_EXTERN) ? 'U' : '-',
            HAS_PL_TYPE((*loaded[i].pl), PL_FAULT_MANAGER) ? 'A' : '-',
            (loaded[i].role & PL_FAULT_MANAGER) ? 'U' : '-',
            HAS_PL_TYPE((*loaded[i].pl), PL_LOGGER) ? 'A' : '-',
            (loaded[i].role & PL_LOGGER) ? 'U' : '-',
            HAS_PL_TYPE((*loaded[i].pl), PL_TOPOLOGY) ? 'A' : '-',
            (loaded[i].role & PL_TOPOLOGY) ? 'U' : '-',
            HAS_PL_TYPE((*loaded[i].pl), PL_OTHER) ? 'A' : '-',
            (loaded[i].role & PL_OTHER) ? 'U' : '-');
        plugins->logger_used->log_fun(LL_Info, buf);
    }
    free(loaded);

    if (opts.sync) {
        while(!termreq) {
            if (server->all_nodes_ready())
                break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        plugins->logger_used->log_fun(LL_Info, "all nodes ready");
        server->send_synced_signal();
    }

    while(!termreq && (opts.max_tics == 0 || opts.max_tics > tic_num++))
        server->do_tic();

    google::protobuf::ShutdownProtobufLibrary();
    plugins->logger_used->log_fun(LL_Info, "cleaning up");
    plugins->cleanup();
    conf->cleanup();
    server->cleanup();
}
