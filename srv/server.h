#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <cstdint>
#include <mutex>
#include <lausim/types.h>
#include <backchannel.pb.h>

namespace lauSim {

class Server {
public:
    static Server *get_instance();
    int init(unsigned level);
    void skip_to_end_of_tic();
    size_t get_messages(Backchannel ***);
    uint64_t get_tic();
    int do_tic();
    void manage_fails();
    static void on_message(uint8_t *, size_t);
private:
    Server() = default;
    unsigned backchannel_type;
    std::mutex backchannel_mutex;
    std::vector<Backchannel> messages;
    size_t num_msg;
    uint64_t tic;
    Config *conf;
    plugin_manager *plugins;
    fault_manager *manager;
    com *com_actor;
    com *com_notify;
    size_t num_nodes;
    node **nodes;
    msg_callback other_cb = nullptr;
};

}

#endif