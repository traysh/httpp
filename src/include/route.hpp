#pragma once

#include "controller.hpp"
#include "httprequest.hpp"

template<class ConnectionType>
class Route {
    public:
        using ControllerType = Controller<ConnectionType>;
        using MethodType = HTTPRequest::MethodType;
        using KeyType = std::pair<std::string, MethodType>;

        using CallableType = typename ControllerType::CallableType;
        using NoRequestCallableType = typename ControllerType::NoRequestCallableType;

        std::string Path;
        MethodType Method;
        ControllerType Controller;

        Route(const std::string& path, const MethodType& method,
              ControllerType controller)
            : Path(path), Method(method), Controller(controller) {}

        Route(const std::string& path, const MethodType& method,
              CallableType callable)
            : Path(path), Method(method), Controller({ callable }) {}

        Route(const std::string& path, const MethodType& method,
              NoRequestCallableType callable)
            : Path(path), Method(method), Controller({ callable }) {}
};

