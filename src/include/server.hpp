#pragma once

#include <list>

#include "listensocket.hpp"
#include "connectionqueue.hpp"

class Server {
public:
    Server();

    inline void Serve(const unsigned short& port) {
        return Serve("0.0.0.0", port);
    }
    void Serve(const char* address, const unsigned short& port);
    void SetReuseAddress(const bool& reuseAddress = true);

private:
    void showStartupInfo(const char* address,
                         const unsigned short& port);
    void handleRequests();

    ListenSocket _socket;

    bool _run = true;
    ConnectionQueue _queue;
};
