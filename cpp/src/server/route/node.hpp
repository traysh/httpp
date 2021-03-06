#ifndef ROUTE_NODE_HPP
#define ROUTE_NODE_HPP

#include <map>
#include <stdexcept>
#include <string>

#include "server/controller.hpp"
#include "http/endpoint.hpp"
#include "util/string.hpp"
#include "node_lookup_result.hpp"


namespace Server {
namespace Route {
class Node {
    using Endpoint = HTTP::Endpoint;
    using MethodType = HTTP::MethodType;

public:
    explicit Node()
        : _children(), _controllers() {}

    bool Empty() { return _children.empty();  }

    void Add(const Endpoint& endpoint, const Controller& controller) {
        const auto& clean_path = Util::String::RemoveLeadingOrTrailing(endpoint.Path, '/');
        if (clean_path.empty()) {
            const auto& [_, emplaced] = _controllers.emplace(endpoint.Method, controller);
            if (!emplaced) {
                // TODO: print route and method
                throw std::domain_error("Route already configured");
            }
            return;
        }

        auto [child_path, unprocessed] = Util::String::Split(clean_path, "/");
        if (child_path[0] == ':') {
            _childParameterName = child_path.substr(1);
            child_path = ':';
        }

        auto [node_it, _] = _children.emplace(child_path, Node{});
        node_it->second.Add(Endpoint{unprocessed, endpoint.Method}, controller);
    }

    inline NodeLookupResponse Get(const Endpoint& endpoint) const {
        using ParametersType = typename NodeLookupResponse::ParametersType;

        const auto& clean_path = Util::String::RemoveLeadingOrTrailing(endpoint.Path, '/');
        if (clean_path.empty()) {
            if (auto it = _controllers.find(endpoint.Method); it != _controllers.end()) {
                return { &it->second, ParametersType() };
            }
            
            if (auto it = _children.find("*"); it != _children.end()) {
                return it->second.Get(Endpoint{"", endpoint.Method});
            }

            return {};
        }

        const auto [child_path, unprocessed] = Util::String::Split(clean_path, "/");
        if (auto it = _children.find(child_path); it != _children.end()) {
            return it->second.Get(Endpoint{unprocessed, endpoint.Method});
        }

        // look for parameterized path node
        if (auto it = _children.find(":"); it != _children.end()) {
            auto [controller, parameters] = it->second.Get(Endpoint{unprocessed, endpoint.Method});
            parameters[_childParameterName] = child_path;
            return {controller, std::move(parameters)};
        }

        if (auto it = _children.find("*"); it != _children.end()) {
            auto [controller, parameters] = it->second.Get(Endpoint{"", endpoint.Method});
            if (unprocessed.empty()) {
                parameters["*"] = child_path;
            }
            else {
                parameters["*"] = child_path + "/" + unprocessed;
            }
            return {controller, std::move(parameters)};
        }

        return {};
    }

private:
    std::map<const std::string, Node> _children;
    std::map<MethodType, Controller> _controllers;
    std::string _childParameterName;
};
}
}

#endif // ROUTE_NODE_HPP
