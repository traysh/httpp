#pragma once
#include <sys/select.h>
#include <arpa/inet.h>

namespace mockable {
template<bool Prod = true> int select(
        int nfds,
        fd_set* readfds, fd_set* writefds,
        fd_set* errorfds,
        struct timeval* timeout) {
    return ::select(nfds, readfds, writefds, errorfds, timeout);
}
template<bool Prod = true> int accept(int fd, struct sockaddr* addr, socklen_t* size) {
    return ::accept(fd, addr, size);
}
}


