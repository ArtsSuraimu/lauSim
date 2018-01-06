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

class ComMosquitto : public mosqpp::mosquittopp {
public:
    ComMosquitto(const char *id);
    virtual ~ComMosquitto();
    static int init_mosquitto();
    static int cleanup_mosquitto();
    int init(const char * address = "localhost", int port = 1883, unsigned keep_alive = 60);
    com *get_com();
    void cleanup();
    int notify_fail(const char *target, const char *component, unsigned severity);
    int notify_extern(const char *msg, unsigned length);

    virtual void on_message(const struct mosquitto_message *);
private:
    std::string id;
    bool is_init;
};

plugin mosquitto_plugin = {
    2,                              // version
    PL_COM_ACTOR | PL_COM_EXTERN,   // type
    name,                           // name

    post_init,
    cleanup,
    get_com,
    nullptr,
    nullptr
};

}

#endif