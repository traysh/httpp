#pragma once

#include <initializer_list>
#include <map>

#include <iostream>

#include "connection.hpp"
#include "controller.hpp"
#include "http/endpoint.hpp"
#include "route.hpp"
#include "route_node.hpp"

template <class ConnectionType = Connection> class Router {
    using MethodType = HTTP::Request::MethodType;
    using Endpoint = HTTP::Request::Endpoint;
    using ControllerType = Controller<ConnectionType>;
    using RouteNodeType = RouteNode<ControllerType>;

    RouteNodeType _routes;

public:
    using RouteType = Route<ConnectionType>;

    Router()
        : _notFoundController([](HTTPResponse<ConnectionType> &response) {
            response.Status = HTTPResponseStatus::Type::NotFound;
            response.Header.emplace("Connection", "Close");
            response << "Not found\r\n";
        }),
          _internalErrorController([](HTTPResponse<ConnectionType> &response) {
            response.Status = HTTPResponseStatus::Type::InternalServerError;
            response.Header.emplace("Connection", "Close");
            response << "Internal server error\r\n";
        }) {}

    template <class Callable>
    inline void Add(const std::string &path, const MethodType &method, Callable callable) {
        _routes.Add(Endpoint{path, method}, ControllerType(callable));
    }

    inline void Add(const RouteType &data) {
        auto [path, method, controller] = data;
        Add(path, method, controller);
    }

    void Add(const std::initializer_list<RouteType> &init) {
        for (const auto &data : init) {
            Add(data);
        }
    }

    template <class Callable> inline void SetNotFoundHandler(Callable callable) {
        _notFoundController = ControllerType(callable);
    }

    inline const ControllerType &InternalServerErrorHandler() {
        return _internalErrorController;
    }

    template <class Callable>
    inline void SetInternalServerErrorHandler(Callable callable) {
        _internalErrorController = ControllerType(callable);
    }

    inline const ControllerType Get(const std::string &path, const MethodType method) const {
        return Get({path, method});
    }

    const ControllerType Get(const Endpoint &key) const {
        const auto &result = _routes.Get(key);
        if (!result.Controller) {
            return _notFoundController;
        }

        return *result.Controller;
    }

private:
    ControllerType _notFoundController;
    ControllerType _internalErrorController;
};
