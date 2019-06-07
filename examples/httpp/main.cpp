#include "server.hpp"

#include <iostream>

int main() {
    const unsigned short port = 9933;

    Server server;
    server.SetReuseAddress();
    server.Serve(port);

    return 0;
}
