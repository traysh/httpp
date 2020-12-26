#pragma once

#include <initializer_list>
#include <iostream>
#include <map>

#include "controller.hpp"
#include "http/endpoint.hpp"
#include "http/responsestatus.hpp"
#include "route/route.hpp"
#include "route/node.hpp"
#include "route/node_lookup_result.hpp"
#include "route/request.hpp"

class Router {
    using MethodType = HTTP::MethodType;
    using Endpoint = HTTP::Endpoint;

    Route::Node _routes;

   public:
    Router()
        : _notFoundController([](auto &response) {
              response.Status = HTTP::ResponseStatus::Type::NotFound;
              response.Header.emplace("Connection", "Close");
              response << "Not found\r\n";
          }),
          _internalErrorController([](auto &response) {
              response.Status = HTTP::ResponseStatus::Type::InternalServerError;
              response.Header.emplace("Connection", "Close");
              response << "Internal server error\r\n";
          }) {}

    template <class Callable>
    inline void Add(const std::string &path, const MethodType &method,
                    const Callable &callable) {
        _routes.Add(Endpoint{path, method}, Controller(callable));
    }

    inline void Add(const Route::Route &data) {
        auto [path, method, controller] = data;
        Add(path, method, controller);
    }

    void Add(const std::initializer_list<Route::Route> &init) {
        for (const auto &data : init) {
            Add(data);
        }
    }

    inline const Controller &NotFoundHandler() const { return _notFoundController; }

    template <class Callable>
    inline void SetNotFoundHandler(Callable callable) {
        _notFoundController = Controller(callable);
    }

    inline Controller &InternalServerErrorHandler() {
        return _internalErrorController;
    }

    template <class Callable>
    inline void SetInternalServerErrorHandler(Callable callable) {
        _internalErrorController = Controller(callable);
    }

    inline auto Get(const std::string &path, const MethodType method) const {
        return Get({path, method});
    }

    inline const Route::Request Get(const Endpoint &key) const {
        const auto &result = _routes.Get(key);
        if (!result.Controller) {
            return {_notFoundController, std::move(result.RouteParameters)};
        }

        return {*result.Controller, std::move(result.RouteParameters)};
    }

   private:
    Controller _notFoundController;
    Controller _internalErrorController;
};
