#include "http/request.hpp"
#include "http/response.hpp"
#include "server/router.hpp"
#include "server/server.hpp"

#include <fstream>
#include <iostream>

int main() {
    const unsigned short port = 9933;

    Server::Server server;
    server.SetReuseAddress();

    auto& router = server.GetRouter();
    router.Add("/*", HTTP::MethodType::Get,
            [](const HTTP::Request& request, HTTP::Response& response) {
                auto path = request.parameters.empty() ?  "index.html" : request.parameters.at("*");
                std::cout << "requested file: " << path << std::endl;

                if (std::ifstream file(path, std::ios::binary); file.is_open()) {
                    constexpr size_t buf_size = 128 * 1024;
                    char buf[buf_size];
                    
                    do {
                        file.read(buf, buf_size);
                        response << std::string{buf, static_cast<size_t>(file.gcount())};
                    } while (file.good());
                }
                else {
                    response << "could not open file: " << path;
                    response.Status = 404;
                }
        }
    );

    server.Serve(port);

    return 0;
}
