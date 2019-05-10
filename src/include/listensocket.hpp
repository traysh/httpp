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
    void Listen(const char* address,
                const unsigned short& port,
                const int max_backlog = 10); // FIXME
   Connection* Accept(const int timeout_ms);
   void SetReuseAddress(bool reuseAddress = true);

private:
    struct sockaddr_in _listenAddress;
    int _fd;
    bool _ready = false;
};
