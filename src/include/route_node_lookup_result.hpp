#ifndef ROUTE_NODE_LOOKUP_RESULT
#define ROUTE_NODE_LOOKUP_RESULT

#include <map>
#include <string>

#include "controller.hpp"

template<class ControllerType = Controller<Connection>>
struct RouteNodeLookupResponse {
    using ParametersType = std::map<std::string, std::string>;
    std::optional<ControllerType> Controller;
    ParametersType RouteParameters;
};

#endif // ROUTE_NODE_LOOKUP_RESULT
