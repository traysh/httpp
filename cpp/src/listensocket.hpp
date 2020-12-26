#pragma once

#include <memory>
#include <netinet/in.h>

namespace Connection {
class Connection;
}

class ListenSocket
{
public:
    using ConnectionPtr = std::unique_ptr<Connection::Connection>;

    ListenSocket();
    virtual ~ListenSocket();

    bool Ready() const { return _ready; }
    void Listen(const char* address,
                const unsigned short& port,
                const int max_backlog = 10); // FIXME
    ConnectionPtr Accept(const int timeout_ms = 0);
    void SetReuseAddress(bool reuseAddress = true);

private:
    struct sockaddr_in _listenAddress;
    int _fd;
    bool _ready = false;
};
