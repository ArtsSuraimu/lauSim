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

#include <mutex>
#include <memory>
#include <lausim/plugin.h>
#include <lausim/types.h>
#include <lausim/com.h>
#include <mosquittopp.h>
#include <thread>

#if PL_INTF_VERSION != 7
    #error "Plugin interface Version missmatch"
#endif

namespace lauSim {

int post_init(void);
com *get_com(void);
int cleanup(void);
extern logger *log;

char name[] = "mosquitto";

extern "C" int init (const plugin_manager_interface*, int, char **);

class ComMosquitto : public mosqpp::mosquittopp {
public:
    /**
     * creates a new com mosquitto
     * 
     * @param id the client id of this mosquitto client instance
     */
    ComMosquitto(const char *id);
    virtual ~ComMosquitto();
    /**
     * initializes the mosquitto library
     */
    static int init_mosquitto();
    /**
     * cleanup of the mosquitto library
     */
    static int cleanup_mosquitto();
    /**
     * initializes the communication interface
     * 
     * connects to the mosquitto server and subscribes to the apropriate topics
     * @param address Server address of the mqtt broker (default = localhost)
     * @param port Port of the mqtt broker (default = 1883)
     * @param keep_alive interval for the keep alive packets (default = 60)
     * @return 0 on success
     */
    int init(const char * address = "localhost", int port = 1883, unsigned keep_alive = 60);
    /**
     * returns the com interface for this instance
     */
    com *get_com();
    /**
     * cleanup the connection
     */
    void cleanup();
    /**
     * notifies a client that a component has failed
     * 
     * @param target the name of the target node
     * @param component Name of the component that is faulty
     * @param severity The severity of the fault [0,100]
     * @return 0 on success
     */
    int notify_fail(const char *target, const char *component, unsigned severity);
    /**
     * nitifies lauSim of a failed node
     * 
     * @param msg Message to be sent
     * @param length Length of the message
     * @return 0 on success
     */
    int notify_extern(const char *msg, unsigned length);

    /**
     * updates the callback to where messages are forwarded
     *
     * @param cb the new callback
     * @return the previous callback or nullptr if none was set
     */
    msg_callback set_callback(msg_callback cb);

    void on_message(const struct mosquitto_message *);
    void on_connect(int);
    void on_subscribe(int, int, const int *);
private:
    std::mutex backchannel_mutex;
    std::string id;
    std::thread rcv_thr;
    int mid;
    bool is_init;
    msg_callback lausim_callback;
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