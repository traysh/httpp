#include "server.hpp"

#include <iostream>

int main() {
    const unsigned short port = 9933;

    Server server;
    server.SetReuseAddress();

    auto& router = server.GetRouter();
    router.Add({
        { "/ping", HTTPRequest::MethodType::Get,
            [](const auto& request, auto& response) {
                response << "Hello world from " << request.Path;
        }}, 
        { "/echo", HTTPRequest::MethodType::Post,
            [](const auto& request, auto& response) {
                response << request.Body.CStr();
        }}
    });

    server.Serve(port);

    return 0;
}
