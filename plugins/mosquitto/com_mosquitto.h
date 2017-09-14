#ifndef COM_MOSQUITTO_H
#define COM_MOSQUITTO_H

#include <memory>
#include <lausim/plugin.h>
#include <lausim/types.h>
#include <lausim/com.h>
#include <mosquittopp.h>

namespace lauSim {

int post_init(void);
com *get_com(void);
int cleanup(void);
extern logger *log;

char name[] = "mosquitto";

extern "C" int init (const plugin_manager_interface*, int, char **);

class ComMosquitto {
public:
    static int init_mosquitto();
    static int cleanup_mosquitto();
    int init(const char * address = "localhost", int port = 1883, unsigned keep_alive = 60);
    com *get_com();
    int notify_fail(char *target, char *component, unsigned severity);
private:
    std::unique_ptr<mosqpp::mosquittopp> con;
    std::string hostname;
    bool is_init;
};

plugin mosquitto_plugin = {
    1,              // version
    PL_COM_ACTOR,   // type
    name,    // name

    post_init,
    cleanup,
    get_com,
    nullptr,
    nullptr
};

}

#endif