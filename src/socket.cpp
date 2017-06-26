#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <csignal>

#include "socket.h"

using namespace lauSim;

bool Socket::set_type(int family, int socktype, int protocol) {
    if (sd != -1)
        return false;
    this->family = family;
    this->socktype = socktype;
    this->protocol = protocol;
    return true;
}

Socket::Socket(const Socket &sock)
    : family(sock.family)
    , socktype(sock.socktype)
    , protocol(sock.protocol) {
    if ((sd = dup(sock.sd) < 0))
        throw "Unable to dup sd";
}

Socket::~Socket() {
    if (sd > 0)
        close(sd);
}

int Socket::connect(char *addr, char *service) {
    struct addrinfo info, *res = &info;
    int err;

    if (sd >= 0)
        return -1;

    memset(&info, 0, sizeof(info));
    info.ai_family = family;
    info.ai_socktype = socktype;
    info.ai_flags = AI_ADDRCONFIG;

    if (getaddrinfo(addr, service, &info, &res)) {
        std::cerr << "looking up " << addr << ":" << service << "failed" << std::endl;
        if (res != &info)
            freeaddrinfo(res);
        return -1;
    }

    err = sd = socket(res->ai_addr->sa_family, socktype, protocol);
    
    if (sd < 0) {
        std::cerr << "unable to create socket (" << res->ai_addr->sa_family << ":";
        std::cerr << socktype << ")" << std::endl;
        freeaddrinfo(res);
        sd = -1;
        return err;
    }

    if ((err = ::connect(sd, res->ai_addr, res->ai_addrlen))) {
        std::cerr << "unable to connect" << std::endl;
        close(sd);
        freeaddrinfo(res);
        sd = -1;
        return err;
    }

    freeaddrinfo(res);
    return 0;
}
