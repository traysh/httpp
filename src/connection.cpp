#include "connection.hpp"

Connection::Connection(const int fd, const sockaddr_in& address)
    : _fd(fd), _address(address)
{
}

Connection::Data* Connection::ReadData() {
    (void)_fd; //TODO
    return nullptr; // TODO
}
