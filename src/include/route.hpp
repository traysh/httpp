#pragma once

#include <string>

#include "controller.hpp"
#include "http/method_type.hpp"

class Route {
    public:
        using MethodType = HTTP::MethodType;
        using KeyType = std::pair<std::string, MethodType>;

        using CallableType = typename Controller::CallableType;
        using NoRequestCallableType = typename Controller::NoRequestCallableType;

        std::string Path;
        MethodType Method;
        Controller controller;

        Route(const std::string& path, const MethodType& method,
              Controller controller)
            : Path(path), Method(method), controller(controller) {}

        Route(const std::string& path, const MethodType& method,
              CallableType callable)
            : Path(path), Method(method), controller(callable) {}

        Route(const std::string& path, const MethodType& method,
              NoRequestCallableType callable)
            : Path(path), Method(method), controller(callable) {}
};

