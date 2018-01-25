
#include <atomic>
#include <csignal>

#include "config.h"
#include "options.h"
#include "server.h"
#include "plugin_internal.h"
#include "stdoutLog.h"

using namespace lauSim;

volatile std::sig_atomic_t termreq;

void term_handler(int signal){
    termreq = 1;
}

int main(int argc, char **argv) {
    Options opts;
    Server *server;
    Config *conf;
    plugin_manager *plugins;
    
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


    if (server->init(opts.loglevel))
        return 1;

    plugins->add_role(conf->com_actor, PL_COM_ACTOR);
    plugins->add_role(conf->com_notify, PL_COM_EXTERN);
    plugins->add_role(conf->logger, PL_LOGGER);
    plugins->add_role(conf->fault_manager, PL_FAULT_MANAGER);

    plugins->logger_used->log_fun(LL_Info, "initialization complete");

    termreq = 0;
    std::signal(SIGTERM, term_handler);
    std::signal(SIGABRT, term_handler);
    std::signal(SIGINT, term_handler);

    while(!termreq)
        server->do_tic();

    google::protobuf::ShutdownProtobufLibrary();
    plugins->logger_used->log_fun(LL_Info, "cleaning up");
    plugins->cleanup();
}