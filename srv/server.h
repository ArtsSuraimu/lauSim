#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <cstdint>
#include <mutex>
#include <lausim/types.h>
#include <backchannel.pb.h>

namespace lauSim {

/**
 * This class implements the server. The Server coordinates the different plugins
 * and querries the fault manager for the current faults and forwards these fault to
 * the appropriate communication plugins.
 */
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
    void send_synced_signal();
    void cleanup();
private:
    Server() = default;
    /**
     * the type of the backchannel used
     */
    unsigned backchannel_type;
    /**
     * coordinates access (to|of) the backchannel
     */
    std::mutex backchannel_mutex;
    /**
     * a buffer of messages from the backchannel. It can be used to temporarly
     * hold messages from an asynchronous backchannel for a synchronous fault manager
     */
    std::vector<Backchannel> messages;
    /**
     * the current tic number
     */
    uint64_t tic;
    // gives easy access to different parts of the server
    Config *conf;
    plugin_manager *plugins;
    fault_manager *manager;
    com *com_actor;
    com *com_notify;
    /**
     * the number of nodes exposed by the fault manager to the server
     * e.g. the nodes that have actors attached to it. A PDU may be represented
     * by the fault manager, but not by a node, therefore it may be private to
     * the fault_manager
     */
    size_t num_nodes;
    /**
     * an array of nodes. The type is designed so that the fault manager can easily derive
     * a struct from node and not need to create a copy of all notes for the server.
     * access to node "i" is like "nodes[i]->attribute". The type is (nodes *(node[1]))
     */
    node **nodes;
    /**
     * If the backend is asynchronous, a callback is registered in the com interface. However only one callback
     * is stored at a time. Therefore this varaible stores the address of an eventually previously registered
     * callback, so that messages can be forwarded to other parts of the backend (e.g. fault manager)
     */
    msg_callback other_cb = nullptr;
};

}

#endif