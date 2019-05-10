#include "server.hpp"
#include "connection.hpp"

#include <sstream>
#include <iostream>
#include <functional>
#include <future>

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

    _run = true;
    while(_run) {
        auto* connection = _socket.Accept(10); // FIXME hardcoded
        if (connection == nullptr) {
            continue;
        }

        auto request = connection->ReadData();

        // TODO router
        auto result = std::async(std::launch::async, [&]() {
            std::cout << request->data()<< std::endl;
        });
    }
}
