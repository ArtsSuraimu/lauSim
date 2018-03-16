#ifndef PRELOAD_H
#define PRELOAD_H

#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <backend_if.h>
#include <backend.h>

struct lauSim_state_struct {
    /**
     * current state of the network. A transition in both directions is possible
     * value | semantic
     * -----:|:---------
     *     0 | the network is in nominal state
     *     1 | the network has failed
     */
    unsigned long net_state;
    /**
     * The thread that is listening on the fifo (waiting for commands)
     */
    pthread_t thr;
    /**
     * Status of the initialization (1 iff initialized)
     */
    int is_init;
    /**
     * mutex to synchronize access to the vector of fds
     */
    pthread_mutex_t fds_mutex;
    /**
     * vector of file descriptors (fds)
     */
    int *fds;
    /**
     * number of file descriptors
     */
    unsigned num_fds;
    /**
     * the fds used to signal to interrupt the thread listening on the fifo
     */
    int req_close_fds[2];
};

struct lauSim_hooks_struct{
    /**
     * The original libc function for socket, as the symbol is shadowed
     */
    int (*socket) (int domain, int type, int protocol);
    /**
     * The original libc function for accept, as the symbol is shadowed
     */
    int (*accept) (int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
    /**
     * The original libc function for close, as the symbol is shadowed
     */
    int (*close) (int fd);
};

/**
 * This function initializes the hooks by resolving the symbols (ignoring the shadowning by this library)
 * Then it initializes the backend
 * @return 0 on success
 */
int __attribute__((constructor)) init_hooks();

/**
 * destructor used to clean up the backend if the application exits
 */
int __attribute__((destructor)) lauSim_hook_cleanup();

/**
 * Switches the state of the network
 * @param severity The severity of the fault. 0 meaning nominal operation, 100 fault of this unit
 * @return 0 on success
 */
int lauSim_switch_net(unsigned long severity);

/**
 * resolves the address of a symbol by its name, ignores shadowning by this library
 * @param fun the name of the symbol to be resolved
 * @return address of the symbol or NULL on failure
 */
void *lauSim_resolve(const char *fun);

/**
 * adds a file descriptor to the list of (open) file descriptors
 * @param sock the file descriptor
 * @return 0 on success
 */
int add_sock_to_list(int sock);

/**
 * checks if a file descriptor is in the list of (open) file descriptors
 * @param sock the file descriptor to look for
 * @return 1 if it is in the list, 0 else
 */
int is_in_list(int sock);

/**
 * removes a file descriptor from the list of (open) file descriptors
 * @param sock the file descriptor to remove
 * @return 0 if the file descriptor was removed
 */
int remove_sock_from_list(int sock);

#endif