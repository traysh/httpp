#include "server.hpp"
#include "connection.hpp"

#include <sstream>
#include <iostream>
#include <functional>

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
        auto* connection = _socket.Accept(10); // TODO
        if (connection == nullptr) {
            continue;
        }

        auto request = connection->ReadData();
        _run = false;

        // TODO router
        std::cout << request->data()<< std::endl;
    }
}
