#include "listensocket.hpp"
#include "listensocket_mocks.hpp"
#include "connection.hpp"

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

using namespace std;

ListenSocket::ListenSocket() : _fd(0)
{
	memset(&_addr, 0, sizeof(_addr));
}

ListenSocket::~ListenSocket()
{
    // TODO close socket
}

// TODO move to config?
namespace {
    constexpr int address_length = 16; // FIXME: not valid for ipv6
}


bool ListenSocket::Listen(const char* address,
                          const unsigned short& port,
                          const int max_backlog)
{
    if (_ready) {
        // TODO exception
        return false;
    }

    char sane_address[address_length];
    sane_address[address_length -1] = 0;
    memcpy(sane_address, address, 15); // xxx.xxx.xxx.xxx

	_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct in_addr addr;
    if (inet_aton(sane_address, &addr) == 0) {
        // TODO exception
        return false;
    }

    struct sockaddr_in socket_addr;
	socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr = addr;
        
    
	socket_addr.sin_port = htons(port);

    if (::bind(_fd, (struct sockaddr*)&socket_addr,
               sizeof(socket_addr)) != 0) {
        return false; // TODO
    }
    
    if (mockable::listen(_fd, max_backlog) != 0) {
        return false; // TODO
    }

    _ready = true;
    
    return true;
}

Connection* ListenSocket::Accept(const int timeout_ms) {
    fd_set read_fd_set;
    FD_ZERO (&read_fd_set);

    struct timeval timeout = {
        timeout_ms / 1000, // .tv_sec
        (timeout_ms % 1000) * 1000,  // .tv_usec
    };

    if (mockable::select (FD_SETSIZE, &read_fd_set, nullptr, nullptr, &timeout) < 0) {
        return nullptr; // TODO exception?
    }
    if (!FD_ISSET (_fd, &read_fd_set)) {
        return nullptr;
    }

    struct sockaddr_in connection_address;
    socklen_t connection_address_size = sizeof(connection_address);

    memset(&connection_address, 0, sizeof(struct sockaddr));

    int connfd = mockable::accept(_fd,
                                  reinterpret_cast<struct sockaddr*>(&connection_address),
                                  &connection_address_size);

    return new Connection(connfd, connection_address);
}

