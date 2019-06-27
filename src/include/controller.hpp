#pragma once

#include <functional>

#include "httprequest.hpp"
#include "httpresponse.hpp"

template<class ConnectionType>
class Controller {
    public:
        using HTTPResponseType = HTTPResponse<ConnectionType>;
        using CallableType = void(*)(const HTTPRequest&, HTTPResponseType&);
        using NoRequestCallableType = void(*)(HTTPResponseType&);

        static Controller Null() {
            return Controller();
        }

        Controller() : _callable(nullptr) {}
        Controller(CallableType callable) : _callable(callable) {}
        Controller(NoRequestCallableType callable) {
            if (callable == nullptr) {
                _callable = nullptr;
            }
            else {
                _callable = [callable](const HTTPRequest&, HTTPResponseType& response) {
                    callable(response);
                };
            }
        }

        operator bool() {
            return _callable != nullptr;
        }

        auto operator()(const HTTPRequest& request, HTTPResponseType& response) {
            return _callable(request, response);
        }

    private:
        std::function<void(const HTTPRequest&, HTTPResponse<ConnectionType>&)> _callable;
};

