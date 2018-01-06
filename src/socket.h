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
