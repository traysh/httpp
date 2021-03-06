#include "listensocket.hpp"
#include "listensocketexceptions.hpp"
#include "mock/listensocket_mocks.hpp"
#include "connection/connection.hpp"
#include "connection/connection_impl.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

namespace Socket {
ListenSocket::ListenSocket() : _fd(socket(AF_INET, SOCK_STREAM, 0))
{
	memset(&_listenAddress, 0, sizeof(_listenAddress));
}

ListenSocket::~ListenSocket()
{
    if (close(_fd) != 0) {
        std::cerr << __FILE__ " - Error closing socket" << std::endl;
    }
}

// TODO move to config?
namespace {
    constexpr int address_length = 16; // FIXME: not valid for ipv6
}

void ListenSocket::SetReuseAddress(bool reuseAddress) {
    if (_ready) {
        throw Error<ErrorType::AlreadyInitialized>();
    }

    int enable = reuseAddress ? 1 : 0;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        throw Error<ErrorType::SetReuseAddressError>();
    }

void ListenSocket::Listen(const char* address,
                          const unsigned short& port,
                          const int max_backlog)
{
    if (_ready) {
        throw Error<ErrorType::AlreadyInitialized>();
    }

    char sane_address[address_length];
    sane_address[address_length -1] = 0;
    memcpy(sane_address, address, 15); // xxx.xxx.xxx.xxx

    struct in_addr addr;
    if (int ret = inet_aton(sane_address, &addr) == 0) {
        throw Error<ErrorType::ConvertAddress>(ret);
    }

	_listenAddress.sin_family = AF_INET;
    _listenAddress.sin_addr = addr;
	_listenAddress.sin_port = htons(port);

    if (int ret = ::bind(_fd, (struct sockaddr*)&_listenAddress,
               sizeof(_listenAddress)) != 0) {
        throw Error<ErrorType::BindError>(ret);
    }
    
    if (int ret = mockable::listen(_fd, max_backlog) != 0) {
        throw Error<ErrorType::ListenError>(ret);
    }

    _ready = true;
}

ListenSocket::ConnectionPtr ListenSocket::Accept(const int timeout_ms) {
    if (!_ready) {
        throw Error<ErrorType::Unready>();
    }

    fd_set read_fd_set;
    FD_ZERO(&read_fd_set);
    FD_SET(_fd, &read_fd_set);

    struct timeval timeout = {
        timeout_ms / 1000, // .tv_sec
        (timeout_ms % 1000) * 1000,  // .tv_usec
    };

    auto* timeout_ptr = timeout_ms != 0 ? &timeout : nullptr;
    if (mockable::select(FD_SETSIZE, &read_fd_set, nullptr, nullptr,
                         timeout_ptr) < 0) {
        throw Error<ErrorType::SelectError>();
    }
    if (!FD_ISSET(_fd, &read_fd_set)) {
        return std::unique_ptr<Connection::Connection>(nullptr);
    }

    struct sockaddr_in connection_address;
    socklen_t connection_address_size = sizeof(connection_address);

    memset(&connection_address, 0, sizeof(struct sockaddr));

    int connfd = mockable::accept(_fd,
                                  reinterpret_cast<struct sockaddr*>(&connection_address),
                                  &connection_address_size);

    int result = mockable::fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFL, 0) | O_NONBLOCK);
    if (result != 0) {
        throw Error<ErrorType::SetNoWaitError>();
    }

    return std::unique_ptr<Connection::Connection>(new Connection::ConnectionImpl(connfd, connection_address));
}
}
