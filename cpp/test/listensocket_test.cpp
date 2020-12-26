#include <gtest/gtest.h>
#include <cstring>
#include <unistd.h>
#include <memory>

#include "connection/connection.hpp"
#include "socket/listensocket.hpp"
#include "socket/listensocketexceptions.hpp"
#include "mock/listensocket_mocks.hpp"

#include <map>

using namespace std;

const auto sourceAddress = "0.0.0.0";
const unsigned short port = 9933;

using ListenSocketTest = ::testing::Test;

namespace {
TEST_F(ListenSocketTest, CorrectlyInitialized) {
    Socket::ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
}

TEST_F(ListenSocketTest, ListenSuccess) {
    Socket::ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port+1));
    EXPECT_TRUE(socket.Ready());
}

TEST_F(ListenSocketTest, SetReuseAddressFailAlreadyInitialized) {
    Socket::ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port+2));
    EXPECT_THROW(socket.SetReuseAddress(), Socket::Error<Socket::ErrorType::AlreadyInitialized>);
}

TEST_F(ListenSocketTest, SetReuseAddressSuccess) {
    Socket::ListenSocket socket;
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

    Socket::ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_THROW(socket.Listen(sourceAddress, port),
        Socket::Error<Socket::ErrorType::ListenError>);
    EXPECT_FALSE(socket.Ready());
}

TEST_F(ListenSocketTest, ListenTwiceFails) {
    Socket::ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());

    EXPECT_THROW(socket.Listen(sourceAddress, port),
                 Socket::Error<Socket::ErrorType::AlreadyInitialized>);
    EXPECT_TRUE(socket.Ready());
}

TEST_F(ListenSocketTest, ListenInvalidAddress) {
    const auto sourceAddress = "notaddress";

    Socket::ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_THROW(socket.Listen(sourceAddress, port),
                 Socket::Error<Socket::ErrorType::ConvertAddress>);
    EXPECT_FALSE(socket.Ready());
}

TEST_F(ListenSocketTest, ListenAlreadyBound) {
    Socket::ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());

    Socket::ListenSocket failureSocket;
    failureSocket.SetReuseAddress();
    EXPECT_THROW(failureSocket.Listen(sourceAddress, port),
                 Socket::Error<Socket::ErrorType::BindError>);
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

    Socket::ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());

    auto connection(socket.Accept(100));
    EXPECT_NE(connection, nullptr);
}

TEST_F(ListenSocketTest, AcceptButNoClients) {
    Socket::ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());

    auto connection(socket.Accept(100));
    EXPECT_EQ(connection, nullptr);
}

TEST_F(ListenSocketTest, AcceptSelectFails) {
    mockable::select.Register(
        [](int, fd_set*, fd_set*, fd_set*,
           struct timeval*){
            return -1;
        }
    );

    Socket::ListenSocket socket;
    EXPECT_FALSE(socket.Ready());
    EXPECT_NO_THROW(socket.SetReuseAddress());
    EXPECT_NO_THROW(socket.Listen(sourceAddress, port));
    EXPECT_TRUE(socket.Ready());

    EXPECT_THROW(socket.Accept(100),
        Socket::Error<Socket::ErrorType::SelectError>);
}

} // namespace

