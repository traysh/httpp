#pragma once

#include <string>
#include <vector>
#include <arpa/inet.h>

class Connection {
public:
    using Data = std::vector<char>;

    Connection(const int fd, const sockaddr_in& address);

    Data* ReadData();
    std::string Address() { return inet_ntoa(_address.sin_addr); }
    unsigned short Port() { return ntohs(_address.sin_port); }

private:
    //TODO config?
    static constexpr size_t BufferSize = 1024;
    int _fd;
    struct sockaddr_in _address;
};
