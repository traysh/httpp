#include "server.hpp"

#include "connection/connection.hpp"
#include "http/request.hpp"
#include "http/response.hpp"
#include "http/requesthandler.hpp"
#include "http/requestparser.hpp"

#include <string.h>
#include <istream>
#include <iostream>
#include <functional>
#include <future>
#include <chrono>
#include <thread>

namespace Server {
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

    while (_run) {
        auto connection = _socket.Accept();
        if (connection == nullptr) {
            continue;
        }

        _queue.PushBack(connection);
    }
}

void Server::handleRequests() {
    while (_run) {
        using namespace std;
        if (_queue.Empty()) {
            this_thread::sleep_for(chrono::milliseconds(5)); // FIXME
            continue;
        }

        auto connection = _queue.PopFront();

        // FIXME use a thread pool
        auto result = std::async(std::launch::async, [&]() {
            using HandlerState = HTTP::RequestHandler::StateType;
            HTTP::RequestHandler handler(*connection, _router);

            for (auto age = handler.Age(); age <= chrono::seconds(3000); // FIXME
                age = handler.Age()) {

                auto state = handler.Process();
                if (state > HandlerState::Processing) {
                    break;
                }
                
                if (state == HandlerState::WaitingForData) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }

            if (handler.State() == HTTP::RequestHandler::StateType::Failed) {
                handler.GenericError();
            }
            else if (handler.Step() != decltype(handler)::StepType::Finished) {
                handler.Timeout();
            }
        });
    }
}
}
