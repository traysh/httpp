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

        Controller(CallableType callable) : _callable(callable) {}
        Controller(NoRequestCallableType callable) {
            if (callable == nullptr) {
                _callable = nullptr;
            }
            else {
                _callable = [callable](const HTTPRequest&,
                                       HTTPResponseType& response) {
                    callable(response);
                };
            }
        }

        operator bool() const {
            return _callable != nullptr;
        }

        auto operator() (const HTTPRequest& request,
                               HTTPResponseType& response)  const {
            return _callable(request, response);
        }

    private:
        using WrappedCallable = std::function<void(const HTTPRequest&,
                                                   HTTPResponseType&)>;
        std::function<void(const HTTPRequest&,
                           HTTPResponse<ConnectionType>&)> _callable;

        Controller() : _callable(nullptr) {}
};

