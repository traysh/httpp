#include <gtest/gtest.h>
#include "connectionqueue.hpp"

#include <arpa/inet.h>

namespace {

TEST(ConnectionQueue, InitiallyEmpty) {
    ConnectionQueue queue;

    EXPECT_TRUE(queue.Empty());
    EXPECT_EQ(queue.Size(), 0);
}

TEST(ConnectionQueue, PushIncreasesSize) {
    ConnectionQueue queue;
    sockaddr_in addr;

    Connection::Ptr connection(
            new Connection(0, addr));

    queue.PushBack(connection);

    EXPECT_EQ(queue.Size(), 1);
    EXPECT_FALSE(queue.Empty());
}

TEST(ConnectionQueue, PushPopMakesEmpty) {
    ConnectionQueue queue;
    sockaddr_in addr;

    Connection::Ptr connection(
            new Connection(0, addr));

    queue.PushBack(connection);
    queue.PopFront();

    EXPECT_EQ(queue.Size(), 0);
    EXPECT_TRUE(queue.Empty());
}

TEST(ConnectionQueue, PushPopTwiceReturnsCorrectElement) {
    ConnectionQueue queue;
    sockaddr_in addr;

    Connection* connection = new Connection(0, addr);
    Connection* connection2 = new Connection(1, addr);
    Connection::Ptr connectionPtr(connection);
    Connection::Ptr connectionPtr2(connection2);

    queue.PushBack(connectionPtr);
    queue.PushBack(connectionPtr2);

    auto pop_con = queue.PopFront();
    auto pop_con2 = queue.PopFront();

    EXPECT_EQ(pop_con.get(), connection);
    EXPECT_EQ(pop_con2.get(), connection2);
}

} // namespace
