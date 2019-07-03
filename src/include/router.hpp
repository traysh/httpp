#pragma once

#include <initializer_list>
#include <map>

#include <iostream>

#include "connection.hpp"
#include "controller.hpp"
#include "route.hpp"

template <class ConnectionType = Connection>
class Router {
        using MethodType = HTTPRequest::MethodType;
        using KeyType = std::pair<std::string, MethodType>;
        using ControllerType = Controller<ConnectionType>;

        std::map<KeyType, ControllerType> _mapping;

    public:
        using RouteType = Route<ConnectionType>;

        Router()
            : _notFoundController([](HTTPResponse<ConnectionType>& response) {
                response.Status = HTTPResponseStatus::Type::NotFound;
                response.Header.emplace("Connection", "Close");
                response << "Not found\r\n";
 
        }),
              _internalErrorController([](HTTPResponse<ConnectionType>&
                                          response) {
                response.Status = HTTPResponseStatus::Type::InternalServerError;
                response.Header.emplace("Connection", "Close");
                response << "Internal server error\r\n";
        }) {}

        template<class Callable>
        inline void Add(const std::string& path, const MethodType& method,
                        Callable callable) {

            _mapping.emplace(std::make_pair(path, method),
                             ControllerType(callable));
        }

        inline void Add(const RouteType& data) {
            auto [path, method, controller] = data;
            Add(path, method, controller);
        }

        void Add(const std::initializer_list<RouteType>& init) {
            for (const auto& data : init) {
                Add(data);
            }
        }

        template<class Callable>
        inline void SetNotFoundHandler(Callable callable) {
            _notFoundController = ControllerType(callable);
        }

        inline const ControllerType& InternalServerErrorHandler() {
            return _internalErrorController;
        }

        template<class Callable>
        inline void SetInternalServerErrorHandler(Callable callable) {
            _internalErrorController = ControllerType(callable);
        }

        inline const ControllerType& Get(const std::string& path,
                                  const MethodType method) const {
            return Get({path, method});
        }

        const ControllerType& Get(const KeyType& key) const {
            auto it = _mapping.find(key);
            if (it == _mapping.end()) {
                return _notFoundController;
            }

            return it->second;
        }

    private:
        ControllerType _notFoundController;
        ControllerType _internalErrorController;
};

