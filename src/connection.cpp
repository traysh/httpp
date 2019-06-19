#include "connection.hpp"
#include "listensocket_mocks.hpp"

#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <iostream>

Connection::Connection(const int fd, const sockaddr_in& address)
    : _fd(fd), _address(address)
{
}

Connection::~Connection() {
     if (mockable::close(_fd) != 0) {
        std::cerr << "Error closing socket" << std::endl;
    }
}

size_t Connection::ReadData(char* buffer, size_t size) {
    int bytesCount = ::recv(_fd, buffer, size, 0);
    if (bytesCount < 0) { // Read error
        return 0; // TODO exception
    }

    return bytesCount;
}

void Connection::operator<<(const std::string& text) {
    write(_fd, text.data(), text.size());
}
