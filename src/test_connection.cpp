#include <gtest/gtest.h>

#include "connection.hpp"

namespace {

TEST(Connection, CorrectlyInitialized) {
    const int fd = 1;
    const int port = 81;
    const auto address = "10.11.12.13";
    struct sockaddr_in socket_addr;

    struct in_addr addr;
    inet_aton(address, &addr);
    socket_addr.sin_addr = addr;
    socket_addr.sin_port = htons(port);

    Connection connection(fd, socket_addr);
    EXPECT_EQ(connection.Address(), address);
    EXPECT_EQ(connection.Port(), port);
}

}
