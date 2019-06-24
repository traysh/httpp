#pragma once

#include <initializer_list>
#include <map>

#include <iostream>

#include "connection.hpp"
#include "httprequest.hpp"
#include "httpresponse.hpp"

template <class ConnectionType = Connection>
class Router {
        using MethodType = HTTPRequest::MethodType;
        using Controller = void(*)(const HTTPRequest&,
                                   HTTPResponse<ConnectionType>&);
        using KeyType = std::pair<std::string, MethodType>;
        using RouteData = std::tuple<std::string, MethodType, Controller>;

        std::map<KeyType, Controller> _mapping;

    public:
        inline void Add(const std::string& path, const MethodType& method,
                 Controller controller) {

            _mapping.emplace(std::make_pair(path, method), controller);
        }

        inline void Add(const RouteData& data) {
            auto [path, method, controller] = data;
            Add(path, method, controller);
        }

        void Add(const std::initializer_list<RouteData>& init) {
            for (const auto& data : init) {
                Add(data);
            }
        }

        inline Controller Get(const std::string& path, const MethodType method) const {
            return Get({path, method});
        }

        Controller Get(const KeyType& key) const {
            auto it = _mapping.find(key);
            if (it == _mapping.end()) {
                return nullptr;
            }

            return *it->second;
        }
};

