/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-30 13:01:25
*/

#ifndef LAUSIM_SOCKET_H
#define LAUSIM_SOCKET_H

namespace lauSim {

class Socket {
public:
    Socket() : sd(-1), family(AF_UNSPEC), socktype(SOCK_STREAM), protocol(0) {}
    Socket(const Socket &sock);
    bool set_type(int family = AF_UNSPEC, int socktype = SOCK_STREAM, int protocol = 0);
    ~Socket();
    int connect(char *addr, char *service);
    int dup_sock();
private:
    int sd;
    int family;
    int socktype;
    int protocol;
};

}

#endif /*LAUSIM_SOCKET_H*/
