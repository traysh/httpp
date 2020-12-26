#include <gtest/gtest.h>
#include <arpa/inet.h>
#include <memory>

#include "connection_mock.hpp"
#include "connectionqueue.hpp"

using ConnectionPtr = std::unique_ptr<Connection>;
using ConnectionMockQueueTest = ::testing::Test;

namespace {

TEST_F(ConnectionMockQueueTest, InitiallyEmpty) {
    ConnectionQueue queue;

    EXPECT_TRUE(queue.Empty());
    EXPECT_EQ(queue.Size(), 0);
}

TEST_F(ConnectionMockQueueTest, PushIncreasesSize) {
    ConnectionQueue queue;
    ConnectionPtr connection(new ConnectionMock({}));

    queue.PushBack(connection);

    EXPECT_EQ(queue.Size(), 1);
    EXPECT_FALSE(queue.Empty());
}

TEST_F(ConnectionMockQueueTest, PushPopMakesEmpty) {
    ConnectionQueue queue;
    ConnectionPtr connection(new ConnectionMock({}));

    queue.PushBack(connection);
    queue.PopFront();

    EXPECT_EQ(queue.Size(), 0);
    EXPECT_TRUE(queue.Empty());
}

TEST_F(ConnectionMockQueueTest, PushPopTwiceReturnsCorrectElement) {
    ConnectionQueue queue;

    ConnectionMock* connection = new ConnectionMock({});
    ConnectionMock* connection2 = new ConnectionMock({});
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
