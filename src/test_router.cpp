#include <gtest/gtest.h>

#include <utility>

#include "connection_mock.hpp"
#include "router.hpp"

using RouterTest = ::testing::Test;
using MethodType = HTTPRequest::MethodType;
using ConnectionMock = Mock::Connection<1024>;

auto testController = [](const HTTPRequest&, HTTPResponse<ConnectionMock>&) {
    //  Does nothing
};

TEST_F(RouterTest, CallsAddRouteAndGet) {
    Router<ConnectionMock> router;

    router.Add("/test", MethodType::Get, testController);
    EXPECT_EQ(router.Get({"/test", MethodType::Get}), testController);
}

TEST_F(RouterTest, CallsAddRouteWithPairAndGet) {
    Router<ConnectionMock> router;

    router.Add({"/test", MethodType::Get, testController});
    EXPECT_EQ(router.Get("/test", MethodType::Get), testController);
}

TEST_F(RouterTest, CallsAddWithMultipleRoutesAndGet) {
    auto test2Controller = [](const HTTPRequest&,
                              HTTPResponse<ConnectionMock>&) {
        // Does nothing
    };

    Router<ConnectionMock> router;
    router.Add({
        {"/test", MethodType::Get, testController},
        {"/test2", MethodType::Get, test2Controller}
    });

    EXPECT_EQ(router.Get("/test", MethodType::Get), testController);
    EXPECT_EQ(router.Get("/test2", MethodType::Get), test2Controller);
}

