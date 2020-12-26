#ifndef ROUTE_NODE_LOOKUP_RESULT
#define ROUTE_NODE_LOOKUP_RESULT

#include <map>
#include <string>

#include "controller.hpp"

namespace Route {
struct NodeLookupResponse {
    using ParametersType = std::map<std::string, std::string>;
    const Controller* Controller;
    ParametersType RouteParameters;
};
}

#endif // ROUTE_NODE_LOOKUP_RESULT
