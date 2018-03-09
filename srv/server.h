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
    /**
     * initializes the server
     * @param level the log level to be used
     * @return 0 on success
     */
    int init(unsigned level);
    /**
     * skips the remaining time until the next tic starts
     */
    void skip_to_end_of_tic();
    /**
     * pulls messages from the Backchannel (synchronous mode)
     * @param a pointer populated by this function with the messages
     * @return the number of messages
     */
    size_t get_messages(Backchannel ***msgs);
    /**
     * get the current tic number
     * @return tic number
     */
    uint64_t get_tic();
    /**
     * performs all actions neccessary during a tic
     * updating the tic number, propagating it the fault manager, handeling faults
     * @return 0 on success
     */
    int do_tic();
    /**
     * querrys the fault manager for the faults for this tic, sends them to the clients
     */
    void manage_fails();
    /**
     * callback for the asynchronous backend
     * @param msg a buffer containing the message
     * @param len the length of the message
     */
    static void on_message(uint8_t *msg, size_t len);
    /**
     * updates the state of a node
     * @param name the name of the node
     * @param new_state the state the node switched into
     * @return 0 on success
     */
    int set_node_state(const char *name, unsigned new_state);
    /**
     * Checks if all nodes are ready
     * @return true if all nodes are ready, false if any node is not
     */
    bool all_nodes_ready();
    void cleanup();
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