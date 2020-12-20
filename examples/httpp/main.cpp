#include "httprequest.hpp"
#include "server.hpp"

int main() {
    const unsigned short port = 9933;

    Server server;
    server.SetReuseAddress();

    auto& router = server.GetRouter();
    using Method = HTTP::Request::MethodType;
    router.Add({
        { "/ping", Method::Get,
            [](auto& response) {
                response << "pong";
        }}, 
        { "/hi", Method::Get,
            [](const auto& request, auto& response) {
                response << "Hello from " << request.Path;
        }},
        { "/echo", Method::Post,
            [](const auto& request, auto& response) {
                response << request.Body.CStr();
        }},
        { "/echo/:param", Method::Post,
            [](const auto& request, auto& response) {
                for (const auto& [key, value] : request.parameters) {
                    response << key << ": " << value << "\n";
                }
                response << request.Body.CStr() << "\n";
        }},
        { "/echo/:param/inner", Method::Post,
            [](const auto& request, auto& response) {
                for (const auto& [key, value] : request.parameters) {
                    response << key << ": " << value << "\n";
                }
                response << request.Body.CStr() << " from inner\n";
        }},
        { "/echo/:param/inner/:inner_param", Method::Post,
            [](const auto& request, auto& response) {
                for (const auto& [key, value] : request.parameters) {
                    response << key << ": " << value << "\n";
                }
                response << request.Body.CStr() << " from inner\n";
        }},
     });
    router.SetNotFoundHandler([](auto& response) {
        response.Status = HTTPResponseStatus::Type::NotFound;
        response << "sorry, I don't know that URL";
    });

    server.Serve(port);

    return 0;
}
