#include "connection.hpp"

#include <vector>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

Connection::Connection(const int fd, const sockaddr_in& address)
    : _fd(fd), _address(address)
{
}

Connection::Data* Connection::ReadData() {
    char buffer[BufferSize];
    int bytesCount;

    memset(buffer, 0, BufferSize);

    bytesCount = ::read(_fd, buffer, BufferSize);

    if (bytesCount < 0) { // Read error
        return nullptr; // TODO exception
    }

    if (bytesCount == 0) // EOF
        return nullptr;

    return new Data(buffer, buffer + bytesCount);
}
