#include <gtest/gtest.h>
#include <cstring>
#include <unistd.h>
#include <memory>

#include "listensocket.hpp"
#include "listensocketexceptions.hpp"
#include "listensocket_mocks.hpp"
#include "connection.hpp"

#include <map>

using namespace std;
using ErrorType = SocketErrorType;

const auto sourceAddress = "0.0.0.0";
const unsigned short port = 9933;

using ListenSocketTest = ::testing::Test;

namespace {
TEST_F(ListenSocketTest, CorrectlyInitialized) {
    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
}

TEST_F(ListenSocketTest, ListenSuccess) {
    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port+1));
    EXPECT_TRUE(socket.Ready());
}

TEST_F(ListenSocketTest, SetReuseAddressFailAlreadyInitialized) {
    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port+2));
    EXPECT_THROW(socket.SetReuseAddress(), SocketError<ErrorType::AlreadyInitialized>);
}

TEST_F(ListenSocketTest, SetReuseAddressSuccess) {
    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());
    EXPECT_TRUE(socket.Ready());
}

TEST_F(ListenSocketTest, ListenFails) {
    mockable::listen.Register([](int fd, int backlog) noexcept(noexcept(::listen(fd, backlog))) {
        return 1;
    });

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_THROW(socket.Listen(sourceAddress, port),
        SocketError<ErrorType::ListenError>);
    EXPECT_FALSE(socket.Ready());
}

TEST_F(ListenSocketTest, ListenTwiceFails) {
    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());

    EXPECT_THROW(socket.Listen(sourceAddress, port),
                 SocketError<ErrorType::AlreadyInitialized>);
    EXPECT_TRUE(socket.Ready());
}

TEST_F(ListenSocketTest, ListenInvalidAddress) {
    const auto sourceAddress = "notaddress";

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_THROW(socket.Listen(sourceAddress, port),
                 SocketError<ErrorType::ConvertAddress>);
    EXPECT_FALSE(socket.Ready());
}

TEST_F(ListenSocketTest, ListenAlreadyBound) {
    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());

    ListenSocket failureSocket;
    failureSocket.SetReuseAddress();
    EXPECT_THROW(failureSocket.Listen(sourceAddress, port),
                 SocketError<ErrorType::BindError>);
    EXPECT_FALSE(failureSocket.Ready());
}

TEST_F(ListenSocketTest, AcceptSuccess) {
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

    mockable::fcntl.Register(
        [](int , int , int){
            return 0;
        }
    );

    mockable::fcntl.Register(
        [](int , int , int){
            return 0;
        }
    );


    mockable::close.Register([](int){ return 0; });

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());

    Connection::Ptr connection(socket.Accept(100));
    EXPECT_NE(connection, nullptr);
}

TEST_F(ListenSocketTest, AcceptButNoClients) {
    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());

    Connection::Ptr connection(socket.Accept(100));
    EXPECT_EQ(connection, nullptr);
}

TEST_F(ListenSocketTest, AcceptSelectFails) {
    mockable::select.Register(
        [](int, fd_set*, fd_set*, fd_set*,
           struct timeval*){
            return -1;
        }
    );

    ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());

    EXPECT_THROW(socket.Accept(100),
        SocketError<ErrorType::SelectError>);
}

} // namespace

