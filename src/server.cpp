#include "server.hpp"

#include "connection.hpp"
#include "httprequest.hpp"
#include "requesthandler.hpp"
#include "requestparser.hpp"
#include "socketstreambuffer.hpp"

#include <string.h>
#include <istream>
#include <iostream>
#include <functional>
#include <future>
#include <chrono>
#include <thread>

Server::Server() {
}

void Server::SetReuseAddress(const bool& reuseAddress) {
    _socket.SetReuseAddress(reuseAddress);
}

void Server::showStartupInfo(const char* address,
                             const unsigned short& port) {
    std::stringstream ss;
    ss << "Listening on port " << port;

    if (strcmp(address, "0.0.0.0") != 0) {
        ss << ", from " << address;
    }

    std::cout << ss.str() << std::endl;
}

void Server::Serve(const char* address,
                   const unsigned short& port) {
    showStartupInfo(address, port);
    _socket.Listen(address, port);

    auto handlerThread = std::async(std::launch::async, [&]() {
        this->handleRequests();
    });

    // FIXME it seems non-sense to setup a timeout to accept
    // and then do nothing and accept again.
    while (_run) {
        auto connection = _socket.Accept(10); // FIXME hardcoded
        if (connection == nullptr) {
            continue;
        }

        _queue.PushBack(connection);
    }
}

void Server::handleRequests() {
    while (_run) {
        if (_queue.Empty()) {
            using namespace std;
            this_thread::sleep_for(chrono::milliseconds(50));
            continue;
        }

        auto connection = _queue.PopFront();

        // TODO router
        auto result = std::async(std::launch::async, [&]() {
            RequestHandler handler(*connection);
            while (handler.Process() == RequestHandler<Connection>::State::Processing);
            *connection << "olá\n";
            std::cout << "Saindo!" << std::endl;
        });
    }
}
