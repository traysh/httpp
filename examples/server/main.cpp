#include "http/request.hpp"
#include "http/response.hpp"
#include "router.hpp"
#include "server.hpp"

#include <fstream>
#include <iostream>

int main() {
    const unsigned short port = 9933;

    Server server;
    server.SetReuseAddress();

    auto& router = server.GetRouter();
    router.SetNotFoundHandler([](const HTTP::Request& request, HTTP::Response& response) {
            std::string clean_path;
            size_t i;
            for (i = 0; i < request.Path.size(); ++i) {
                if (request.Path[i] != '/') break;
            }

            clean_path = request.Path.substr(i);
            if (clean_path.empty()) {
                clean_path = "index.html";
            }
            std::cout << "requested file: " << clean_path << std::endl;
            if (std::ifstream file(clean_path); file.is_open()) {
                std::string line;
                while (getline(file, line))
                    response << line << "\n";
            }
            else {
                response << "could not open file: " << clean_path;
                response.Status = 404;
            }
        }
    );

    server.Serve(port);

    return 0;
}
