#pragma once

#include <arpa/inet.h>

#include <ios>
#include <iostream>
#include <memory>
#include <string>

#include "connection.hpp"
#include "listensocket_mocks.hpp"

namespace Connection {
class ConnectionImpl : public Connection {
   public:

    ConnectionImpl(const int fd, const sockaddr_in& address)
        : _fd(fd), _address(address) {}

    virtual ~ConnectionImpl() {
        if (mockable::close(_fd) != 0) {
            std::cerr << "Error closing socket" << std::endl;
        }
    }

    virtual ConnectionImpl& operator<<(const std::string& text) override {
        write(_fd, text.data(), text.size());
        return *this;
    }

    virtual Connection& operator<<(const char& c) override {
        write(_fd, &c, 1);
        return *this;
    }

    virtual size_t ReadData(char* buffer, size_t size) override {
        int bytesCount = ::recv(_fd, buffer, size, 0);
        if (bytesCount < 0) {  // Read error
            return 0;          // TODO handle error
        }

        return bytesCount;
    }

    std::string Address() { return inet_ntoa(_address.sin_addr); }
    unsigned short Port() { return ntohs(_address.sin_port); }

   private:
    int _fd;
    struct sockaddr_in _address;
};
}
