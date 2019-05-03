#include <gtest/gtest.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

#include "listensocket.hpp"

using namespace std;

/*
namespace mockable {
template<bool> int select(
        int,
        fd_set* readfds, fd_set*,
        fd_set*,
        struct timeval* timeout);
template<> int select<true>(
        int,
        fd_set* readfds, fd_set*,
        fd_set*,
        struct timeval* timeout) {

    sleep(timeout->tv_sec);
    usleep(timeout->tv_usec);
    memset(readfds, 0xff, sizeof(fd_set));
    return 1;
}
}
*/
namespace {
TEST(ListenSocket, CorrectlyInitialized) {
    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
}

TEST(ListenSocket, ListenSuccess) {
    const auto address = "0.0.0.0";
    const unsigned short port = 9933;

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_TRUE(socket.Listen(address, port));
    EXPECT_TRUE(socket.Ready());
}

TEST(ListenSocket, ListenTwiceFails) {
    const auto address = "0.0.0.0";
    const unsigned short port = 9934;

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_TRUE(socket.Listen(address, port));
    EXPECT_TRUE(socket.Ready());

    EXPECT_FALSE(socket.Listen(address, port));
    EXPECT_TRUE(socket.Ready());
}

TEST(ListenSocket, ListenInvalidAddress) {
    const auto address = "notaddress";
    const unsigned short port = 9933;

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_FALSE(socket.Listen(address, port));
    EXPECT_FALSE(socket.Ready());
}

TEST(ListenSocket, ListenAlreadyBound) {
    const auto address = "0.0.0.0";
    const unsigned short port = 9935;

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_TRUE(socket.Listen(address, port));
    EXPECT_TRUE(socket.Ready());

    ListenSocket failureSocket;
    EXPECT_FALSE(failureSocket.Listen(address, port));
    EXPECT_FALSE(failureSocket.Ready());
}

TEST(ListenSocket, AcceptButNoClients) {
    const auto address = "0.0.0.0";
    const unsigned short port = 9936;

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_TRUE(socket.Listen(address, port));
    EXPECT_TRUE(socket.Ready());

    auto* connections = socket.Accept(100);
    EXPECT_EQ(connections, nullptr);
}

} // namespace

