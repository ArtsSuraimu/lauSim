#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <functional>
#include <string>
#include <unistd.h>


#include "com_mosquitto.h"

#define HOST_NAME_MAX 255
#define LAST_WILL_TOPIC "last_will"

namespace lauSim {
static bool init_state = false;
ComMosquitto instance;
logger *log;

int notify_fail (const char *, const char *, unsigned);
int notify_extern (const char *, unsigned);
plugin_manager_interface* pi;

com com_interface{
    notify_fail,   // notify_fail
    notify_extern  // notify_extern
};

int
ComMosquitto::init_mosquitto() {
    int ret = 0;
    if (!init_state) {
        ret = mosqpp::lib_init();
        if (!ret)
            init_state = true;
        
    }
    return ret;
}

int
ComMosquitto::cleanup_mosquitto() {
    int ret = 0;
    if (init_state) {
        ret = mosqpp::lib_cleanup();
    }
    return ret;
}

int ComMosquitto::init(const char * addr, int port, unsigned keep_alive) {
    char buf[HOST_NAME_MAX + 1];
    int msqerr;

    if (is_init || !init_state)
        return -1;
    log->log_fun(LL_Debug, "MQTT create new client");
    con = std::unique_ptr<mosqpp::mosquittopp>(new mosqpp::mosquittopp());
    log->log_fun(LL_Debug, "MQTT new client created");
    if ((msqerr = con->connect(addr, port, keep_alive))) {
        log->log_fun(LL_Error, mosqpp::strerror(msqerr));
        return -1;
    }
    log->log_fun(LL_Debug, "MQTT connected");
    memset(buf, 0, sizeof(buf));
    gethostname(buf, sizeof(buf));
    hostname = std::string(buf);
    con->will_set(LAST_WILL_TOPIC, strlen(buf), buf);
    is_init = true;
    return 0;
}

int ComMosquitto::notify_fail(const char *target, const char *cmp, unsigned severity){
    std::stringstream topic;
    std::stringstream msg_buf;
    std::string msg;
    int msqerr;

    if (!is_init)
        return false;
    topic << "envelope/" << target;
    if (cmp != nullptr)
        topic << "/" << cmp;
    msg_buf << "0x" << std::hex << severity;
    msg = msg_buf.str();
    log->log_fun(LL_Debug, "sending msg");
    if ((msqerr = con->publish(NULL, topic.str().c_str(), msg.length(), msg.c_str()))) {
        log->log_fun(LL_Error, mosqpp::strerror(msqerr));
        return -1;
    }
    return 0;
}

int ComMosquitto::notify_extern(const char *msg, unsigned length) {
    if (!is_init)
        return 1;
    con->publish(NULL, "envelope/lausim", length, msg);
    return 0;
}

extern "C" int init (const plugin_manager_interface* pli, int argc, char **argv) {
    pi = (plugin_manager_interface*) pli;
    int sc;

    if (pli->version != PL_INTF_VERSION) {
        std::cerr << "[MQTT] Plugin Interface Version Missmatch" << std::endl;
        return 1;
    }

    log = pli->get_logger();
    log->log_fun(LL_Debug, "MQTT start init");
    
    ComMosquitto::init_mosquitto();
    log->log_fun(LL_Debug, "MQTT lib initialized");
    
    switch (argc) {
        case 0:
            sc = instance.init();
            break;
        case 1:
            sc = instance.init(argv[0]);
            break;
        case 2:
            sc = instance.init(argv[0], strtoul(argv[1], nullptr, 0));
            break;
        default:
            sc = instance.init(argv[0], strtoul(argv[1], nullptr, 0), strtoul(argv[2], nullptr, 0));
            break;
    }

    if (sc) {
        log->log_fun(LL_Error, "[MQTT] initialization failed");
        return sc;
    }

    log->log_fun(LL_Debug, "MQTT finished");

    if (pli->register_plugin(&mosquitto_plugin)){
        ComMosquitto::cleanup_mosquitto();
        return 1;
    }

    return 0;
}

void ComMosquitto::cleanup(){
    if (is_init)
        con->disconnect();
}

int post_init() {
    log = pi->get_logger();
    return 0;
}

int cleanup() {
    instance.cleanup();
    ComMosquitto::cleanup_mosquitto();
    return 0;
}

com *get_com() {
    return &com_interface;
}

int notify_fail(const char *node, const char *component, unsigned severity) {
    return instance.notify_fail(node, component, severity);
}

int notify_extern(const char *node, unsigned length) {
    return instance.notify_extern(node, length);
}
}