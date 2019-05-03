#pragma once

#include <netinet/in.h>
#include <string>

class Connection;

class ListenSocket
{
public:
    ListenSocket();
    virtual ~ListenSocket();

    bool Ready() const { return _ready; }
    bool Listen(const char* address,
                const unsigned short& port,
                const int max_backlog = 10); // FIXME
   Connection* Accept(const int timeout_ms);

private:
    struct sockaddr_in _addr;
    int _fd;
    bool _ready = false;
};
