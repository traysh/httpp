#pragma once

#include <string>
#include <arpa/inet.h>

class Connection {
public:
    using Data = char*;

    Connection(const int fd, const sockaddr_in& address);

    Data* ReadData();
    std::string Address() { return inet_ntoa(_address.sin_addr); }
    unsigned short Port() { return ntohs(_address.sin_port); }

private:
    int _fd;
    struct sockaddr_in _address;
};
