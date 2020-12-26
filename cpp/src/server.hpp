#pragma once

#include <list>

#include "connection_queue.hpp"
#include "listensocket.hpp"
#include "router.hpp"

class Server {
public:
    inline void Serve(const unsigned short& port) {
        return Serve("0.0.0.0", port);
    }
    void Serve(const char* address, const unsigned short& port);
    void SetReuseAddress(const bool& reuseAddress = true);

    auto& GetRouter() {
        return _router;
    }

private:
    void showStartupInfo(const char* address,
                         const unsigned short& port);
    void handleRequests();

    ListenSocket _socket;

    bool _run = true;
    ConnectionQueue _queue;
    Router _router;
};
