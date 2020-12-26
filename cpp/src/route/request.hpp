#pragma once

#include "connection_queue.hpp"
#include <map>
#include <string>

class Controller;

namespace Route {
struct Request {
    const Controller& controller;
    std::map<std::string, std::string> RouteParameters;
};
}