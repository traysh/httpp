#include <gtest/gtest.h>
#include <iostream>
#include <cstring>

#include "listensocket.hpp"

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

} // namespace

