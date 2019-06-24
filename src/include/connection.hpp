#pragma once

#include <arpa/inet.h>
#include <ios>
#include <iostream>
#include <memory>
#include <string>

#include "listensocket_mocks.hpp"

class Connection {
public:
    using Ptr = std::unique_ptr<Connection>;

    Connection(const int fd, const sockaddr_in& address)
        : _fd(fd), _address(address) {}

    ~Connection() {
         if (mockable::close(_fd) != 0) {
            std::cerr << "Error closing socket" << std::endl;
        }
    }

    template<class T,
             typename = std::enable_if_t<!std::is_array<T>::value
                                         && !std::is_pointer<T>::value>
    >
    inline Connection& operator<<(const T& data) {
        std::string text = std::to_string(data);
        *this << text;
        return *this;
    }

    template<>
    inline Connection& operator<<(const char& c) {
        write(_fd, &c, 1);
        return *this;
    }

    inline Connection& operator<<(const std::string& text) {
        write(_fd, text.data(), text.size());
        return *this;
    }

    size_t ReadData(char* buffer, size_t size) {
        int bytesCount = ::recv(_fd, buffer, size, 0);
        if (bytesCount < 0) { // Read error
            return 0; // TODO handle error
        }

        return bytesCount;
    }

    std::string Address() { return inet_ntoa(_address.sin_addr); }
    unsigned short Port() { return ntohs(_address.sin_port); }

private:
    int _fd;
    struct sockaddr_in _address;
};
