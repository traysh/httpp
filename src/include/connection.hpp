#pragma once

#include <string>
#include <arpa/inet.h>
#include <memory>
#include <ios>

class Connection {
public:
    using Ptr = std::unique_ptr<Connection>;

    Connection(const int fd, const sockaddr_in& address);
    ~Connection();

    void operator<<(const std::string& text);

    size_t ReadData(char* buffer, size_t size);
    std::string Address() { return inet_ntoa(_address.sin_addr); }
    unsigned short Port() { return ntohs(_address.sin_port); }

private:
    int _fd;
    struct sockaddr_in _address;
};
