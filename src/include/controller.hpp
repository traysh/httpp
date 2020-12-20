#pragma once

#include <functional>

#include "httprequest.hpp"
#include "http/response.hpp"

class Controller {
    public:
        using CallableType = void(*)(const HTTPRequest&, HTTP::Response&);
        using NoRequestCallableType = void(*)(HTTP::Response&);

        static Controller Null() {
            return Controller();
        }

        Controller(CallableType callable) : _callable(callable) {}
        Controller(NoRequestCallableType callable) {
            if (callable == nullptr) {
                _callable = nullptr;
            }
            else {
                _callable = [callable](const HTTPRequest&, HTTP::Response& response) {
                    callable(response);
                };
            }
        }

        operator bool() const {
            return static_cast<bool>(_callable);
        }

        auto operator() (const HTTPRequest& request, HTTP::Response& response)  const {
            return _callable(request, response);
        }

    private:
        using WrappedCallable = std::function<void(const HTTPRequest&,
                                                   HTTP::Response&)>;
        std::function<void(const HTTPRequest&,
                           HTTP::Response&)> _callable;

        Controller() : _callable() {}
};

