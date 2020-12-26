#include <gtest/gtest.h>
#include <arpa/inet.h>
#include <memory>

#include "connection/connection_mock.hpp"
#include "server/connection_queue.hpp"

using ConnectionPtr = std::unique_ptr<Connection::Connection>;
using ConnectionQueueTest = ::testing::Test;

namespace {

TEST_F(ConnectionQueueTest, InitiallyEmpty) {
    Server::ConnectionQueue queue;

    EXPECT_TRUE(queue.Empty());
    EXPECT_EQ(queue.Size(), 0);
}

TEST_F(ConnectionQueueTest, PushIncreasesSize) {
    Server::ConnectionQueue queue;
    ConnectionPtr connection(new Connection::ConnectionMock({}));

    queue.PushBack(connection);

    EXPECT_EQ(queue.Size(), 1);
    EXPECT_FALSE(queue.Empty());
}

TEST_F(ConnectionQueueTest, PushPopMakesEmpty) {
    Server::ConnectionQueue queue;
    ConnectionPtr connection(new Connection::ConnectionMock({}));

    queue.PushBack(connection);
    queue.PopFront();

    EXPECT_EQ(queue.Size(), 0);
    EXPECT_TRUE(queue.Empty());
}

TEST_F(ConnectionQueueTest, PushPopTwiceReturnsCorrectElement) {
    Server::ConnectionQueue queue;

    Connection::ConnectionMock* connection = new Connection::ConnectionMock({});
    Connection::ConnectionMock* connection2 = new Connection::ConnectionMock({});
    ConnectionPtr connectionPtr(connection);
    ConnectionPtr connectionPtr2(connection2);

    queue.PushBack(connectionPtr);
    queue.PushBack(connectionPtr2);

    auto pop_con = queue.PopFront();
    auto pop_con2 = queue.PopFront();

    EXPECT_EQ(pop_con.get(), connection);
    EXPECT_EQ(pop_con2.get(), connection2);
}

} // namespace
