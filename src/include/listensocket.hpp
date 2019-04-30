#pragma once

#include <netinet/in.h>
#include <string>

class ListenSocket
{
public:
    ListenSocket();
    virtual ~ListenSocket();

    bool Ready() const { return _ready; }
    bool Listen(const std::string& address,
                const unsigned short& port,
                const int max_backlog = 10); // FIXME

private:
    struct sockaddr_in _addr;
    int _fd;
    bool _ready = false;
};
