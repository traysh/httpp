#pragma once

#include <functional>

#include "http/request.hpp"
#include "http/response.hpp"

namespace Server {
class Controller {
    public:
        using CallableType = void(*)(const HTTP::Request&, HTTP::Response&);
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
                _callable = [callable](const HTTP::Request&, HTTP::Response& response) {
                    callable(response);
                };
            }
        }

        operator bool() const {
            return static_cast<bool>(_callable);
        }

        auto operator() (const HTTP::Request& request, HTTP::Response& response)  const {
            return _callable(request, response);
        }

    private:
        using WrappedCallable = std::function<void(const HTTP::Request&,
                                                   HTTP::Response&)>;
        std::function<void(const HTTP::Request&, HTTP::Response&)> _callable;

        Controller() : _callable() {}
};
}
