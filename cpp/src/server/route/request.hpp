#pragma once

#include "server/connection_queue.hpp"
#include <map>
#include <string>

namespace Server {
class Controller;

namespace Route {
struct Request {
    const Controller& controller;
    std::map<std::string, std::string> RouteParameters;
};
}

}
