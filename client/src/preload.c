/*
   Copyright 2018 Clemens Jonischkeit

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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include "backend_if.h"
#include "backend.h"

int lauSim_caps = SUBSYS_NET;
unsigned long lauSim_net_status = 0;
pthread_t lauSim_thr;
int lauSim_hook_is_init = 0;
pthread_mutex_t lauSim_fds_mutex = PTHREAD_MUTEX_INITIALIZER;
int *lauSim_sock_fds = NULL;
unsigned lauSim_num_fds = 0;


struct {
    int req_close_fds[2];
    int (*socket) (int domain, int type, int protocol);
    int (*accept) (int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
    int (*close) (int fd);
} lauSim_hooks;

int lauSim_switch_net(unsigned long severity) {
    unsigned i;
    if (lauSim_net_status == 0 && severity != 0) {
        // TODO disable network
        fputs("disabeling network connections\n", stderr);
        pthread_mutex_lock(&lauSim_fds_mutex);
        for (i = 0; i < lauSim_num_fds; i++) {
            shutdown(lauSim_sock_fds[i], SHUT_RDWR);
        }
        free(lauSim_sock_fds);
        lauSim_num_fds = 0;
        pthread_mutex_unlock(&lauSim_fds_mutex);
        lauSim_net_status = 1;
        return 0;
    }

    if (lauSim_net_status != 0 && severity == 0) {
        // TODO enable network
        lauSim_net_status = 0;
        return 0;
    }

    // nothing to do
    return 0;
}

void lauSim_cleanup_net() {
    lauSim_switch_net(0);
}

void lauSim_cleanup() {
    lauSim_cleanup_net();
    // TODO cleanup mem and pwr
}

int lauSim_set_state(enum en_subsys sys, unsigned long severity) {
    int res;
    switch (sys) {
    case SUBSYS_NET:
        res = lauSim_switch_net(severity);
        break;
    case SUBSYS_PWR:
        lauSim_backend_cleanup();
        _exit(0);
    case SUBSYS_MEM:
        // TODO implement
        res = 0;
        break;
    default:
        res = 1;
        break;
    }
    return res;
}

void *lauSim_resolve(const char *fun) {
    void *sym;

    sym = dlsym(RTLD_NEXT, fun);
    if (sym == NULL) {
        fprintf(stderr, "[FATAL] [lauSim] Error while resolving %s: %s\n", fun, dlerror());
        exit(1);
    }

    return sym;
}

void *lauSim_run_main(void *ign) {
    exit(lauSim_main());
    return NULL;
}

int add_sock_to_list(int sock) {
    if (sock < 0)
        return 1;
    pthread_mutex_lock(&lauSim_fds_mutex);
    lauSim_sock_fds = realloc(lauSim_sock_fds, ++lauSim_num_fds);
    lauSim_sock_fds[lauSim_num_fds - 1] = sock;
    pthread_mutex_unlock(&lauSim_fds_mutex);
    return 0;
}

int is_in_list(int sock) {
    unsigned i;
    pthread_mutex_lock(&lauSim_fds_mutex);
    for (i = 0; i < lauSim_num_fds; ++i) {
        if (lauSim_sock_fds[i] == sock)
            break;
    }
    pthread_mutex_unlock(&lauSim_fds_mutex);
    return i < lauSim_num_fds;
}

int remove_sock_from_list(int sock) {
    unsigned i;
    int ret = 1;
    pthread_mutex_lock(&lauSim_fds_mutex);
    for (i = 0; i < lauSim_num_fds; ++i) {
        if (lauSim_sock_fds[i] == sock) {
            memmove(&lauSim_sock_fds[i], &lauSim_sock_fds[i + 1], sizeof(int) * (--lauSim_num_fds - i));
            ret = 0;
            break;
        }
    }
    pthread_mutex_unlock(&lauSim_fds_mutex);
    return ret;
}

int accept (int socket, struct sockaddr *restrict address, socklen_t *restrict address_len) {
    int ret = lauSim_hooks.accept(socket, address, address_len);
    if (is_in_list(socket)) {
        if (add_sock_to_list(ret) == 0)
            fprintf(stderr, "accepted socket: %d\n", ret);
    }
    return ret;
}

int socket (int domain, int type, int protocol) {
    int ret = lauSim_hooks.socket(domain, type, protocol);
    if (domain == AF_INET || domain == AF_INET6) {
        if (add_sock_to_list(ret) == 0)
            fprintf(stderr, "opened socket: %d\n", ret);
    }
    return ret;
}

int close(int fd) {
    if (remove_sock_from_list(fd) == 0)
        fprintf(stderr, "closed socket: %d\n", fd);
    return lauSim_hooks.close(fd);
}

int __attribute__((constructor)) init_hooks() {
    if (lauSim_hook_is_init)
        return 0;
    
    if (pipe(lauSim_hooks.req_close_fds) < 0) {
        perror("pipe");
        return 1;
    }
    lauSim_req_close_fd = lauSim_hooks.req_close_fds[0];
    lauSim_hooks.socket = lauSim_resolve("socket");
    lauSim_hooks.close = lauSim_resolve("close");
    lauSim_hooks.accept = lauSim_resolve("accept");
    lauSim_hook_is_init = 1;
    lauSim_init(0, NULL);
    return 0;
}

int lauSim_backend_init() {
    pthread_create(&lauSim_thr, NULL, lauSim_run_main, NULL);
    return 0;
}

int __attribute__((destructor)) lauSim_hook_cleanup() {
    write(lauSim_hooks.req_close_fds[1], "X", 1);
    pthread_join(lauSim_thr, NULL);
    lauSim_hooks.close(lauSim_hooks.req_close_fds[0]);
    lauSim_hooks.close(lauSim_hooks.req_close_fds[0]);
    return 0;
}