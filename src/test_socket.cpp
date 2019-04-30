#include <gtest/gtest.h>
#include <iostream>
#include <cstring>

#define private public
#include "socket.hpp"


namespace {
TEST(SocketTest, CorrectlyInitialized) {
    Socket socket;
    EXPECT_FALSE(socket.Ready());
}


} // namespace

