#pragma once

#include "connectionqueue.hpp"
#include <map>
#include <string>

class Controller;

struct RouteRequest {
    const Controller& controller;
    std::map<std::string, std::string> RouteParameters;
};
