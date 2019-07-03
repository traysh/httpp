#include "server.hpp"

int main() {
    const unsigned short port = 9933;

    Server server;
    server.SetReuseAddress();

    auto& router = server.GetRouter();
    using Method = HTTPRequest::MethodType;
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
    });
    router.SetNotFoundHandler([](auto& response) {
        response.Status = HTTPResponseStatus::Type::NotFound;
        response << "sorry, I don't know that URL";
    });

    server.Serve(port);

    return 0;
}
