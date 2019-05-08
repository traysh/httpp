#include <gtest/gtest.h>
#include <cstring>
#include <unistd.h>
#include <memory>

#include "listensocket.hpp"
#include "listensocket_mocks.hpp"
#include "connection.hpp"

#include <map>

using namespace std;

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

TEST(ListenSocket, ListenFails) {
    mockable::listen.Register(
        [](int, int){
            return 1;
        }
    );

    const auto address = "0.0.0.0";
    const unsigned short port = 9940;

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_FALSE(socket.Listen(address, port));
    EXPECT_FALSE(socket.Ready());
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

TEST(ListenSocket, AcceptSuccess) {
    mockable::select.Register(
        [](int, fd_set* readfds, fd_set*, fd_set*,
           struct timeval* timeout){
            sleep(timeout->tv_sec);
            usleep(timeout->tv_usec);
            memset(readfds, 0xff, sizeof(fd_set));
            return 1;
        }
    );

    mockable::accept.Register(
        [](int fd, sockaddr *, unsigned int *){
            return fd + 1;
        }
    );

    const auto address = "0.0.0.0";
    const unsigned short port = 9936;

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_TRUE(socket.Listen(address, port));
    EXPECT_TRUE(socket.Ready());

    unique_ptr<Connection>connection(socket.Accept(100));
    EXPECT_NE(connection, nullptr);
}

TEST(ListenSocket, AcceptButNoClients) {
    const auto address = "0.0.0.0";
    const unsigned short port = 9937;

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_TRUE(socket.Listen(address, port));
    EXPECT_TRUE(socket.Ready());

    unique_ptr<Connection>connection(socket.Accept(100));
    EXPECT_EQ(connection, nullptr);
}

TEST(ListenSocket, AcceptSelectFails) {
    mockable::select.Register(
        [](int, fd_set*, fd_set*, fd_set*,
           struct timeval*){
            return -1;
        }
    );

    const auto address = "0.0.0.0";
    const unsigned short port = 9938;

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_TRUE(socket.Listen(address, port));
    EXPECT_TRUE(socket.Ready());

    unique_ptr<Connection>connection(socket.Accept(100));
    EXPECT_EQ(connection, nullptr);
}

} // namespace

