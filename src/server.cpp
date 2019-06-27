#include "server.hpp"

#include "connection.hpp"
#include "httprequest.hpp"
#include "httpresponse.hpp"
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

template <class ConnectionType>
void Server<ConnectionType>::SetReuseAddress(const bool& reuseAddress) {
    _socket.SetReuseAddress(reuseAddress);
}

template <class ConnectionType>
void Server<ConnectionType>::showStartupInfo(const char* address,
                             const unsigned short& port) {
    std::stringstream ss;
    ss << "Listening on port " << port;

    if (strcmp(address, "0.0.0.0") != 0) {
        ss << ", from " << address;
    }

    std::cout << ss.str() << std::endl;
}

template <class ConnectionType>
void Server<ConnectionType>::Serve(const char* address,
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

template <class ConnectionType>
void Server<ConnectionType>::handleRequests() {
    while (_run) {
        if (_queue.Empty()) {
            using namespace std;
            this_thread::sleep_for(chrono::milliseconds(50));
            continue;
        }

        auto connection = _queue.PopFront();

        // FIXME use a thread pool
        auto result = std::async(std::launch::async, [&]() {
            RequestHandler handler(*connection);
            for (auto i = 0; i < 60; ++i) { // FIXME
                if (handler.Process() != RequestHandler<Connection>::State::Processing ) {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            auto controller = _router.Get(handler.Request.Path,
                                          handler.Request.Method);
            if (controller) {
                HTTPResponse response(*connection);
                controller(handler.Request, response);
            }
            else {
                // TODO make customizable
                *connection << "HTTP/1.1 404 Not found\r\n"
                               "Connection: Close\r\n\r\n"
                               "Not found\r\n";
            }
        });
    }
}

template class Server<Connection>;
