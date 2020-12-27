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

                // FIXME not all files are text
                if (std::ifstream file(path); file.is_open()) {
                    std::string line;
                    while (getline(file, line))
                        response << line << "\n";
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
