#include <gtest/gtest.h>

#include "connection/connection_mock.hpp"
#include "http/endpoint.hpp"
#include "http/method_type.hpp"
#include "server/route/node.hpp"
#include "server/route/node_lookup_result.hpp"
#include "server/route/route.hpp"

namespace {

// debugging helper
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"
constexpr auto printParams = [](auto& params) {
    for (const auto& [key, value] : params.RouteParameters) {
        std::cout << key << ": " << value << std::endl;
    }
};
#pragma clang diagnostic pop

TEST(RouteNodeTest, InitiallyEmpty) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());
}

TEST(RouteNodeTest, GetEmptyReturnsFalseForController) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const HTTP::Endpoint endpoint{"/", HTTP::MethodType::Get};

    const auto& result = node.Get(endpoint);
    EXPECT_FALSE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneController) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();
    const HTTP::Endpoint endpoint{"/", HTTP::MethodType::Get};

    node.Add(endpoint, controller);
    const auto& result = node.Get(endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneChildWithOneController) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();
    HTTP::Endpoint endpoint{"/child", HTTP::MethodType::Get};

    node.Add(endpoint, controller);
    const auto& result = node.Get(endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());

    endpoint = {"/", HTTP::MethodType::Get};
    const auto& other_result = node.Get(endpoint);
    EXPECT_FALSE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOne2ndLevelChildWithOneController) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();
    HTTP::Endpoint endpoint{"/child/inner", HTTP::MethodType::Get};

    node.Add(endpoint, controller);
    const auto& result = node.Get(endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());

    endpoint = {"/", HTTP::MethodType::Get};
    const auto& other_result = node.Get(endpoint);
    EXPECT_FALSE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneChildWithTwoControllers) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();
    HTTP::Endpoint endpoint{"/child", HTTP::MethodType::Get};
    HTTP::Endpoint other_endpoint{"/child", HTTP::MethodType::Post};

    node.Add(endpoint, controller);
    node.Add(other_endpoint, controller);

    const auto& result = node.Get(endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());

    const auto& other_result = node.Get(other_endpoint);
    EXPECT_TRUE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddTwoChildrenWithOneControllerEach) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();
    HTTP::Endpoint endpoint{"/child", HTTP::MethodType::Get};
    HTTP::Endpoint other_endpoint{"/other_child", HTTP::MethodType::Post};

    node.Add(endpoint, controller);
    node.Add(other_endpoint, controller);

    const auto& result = node.Get(endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());

    const auto& other_result = node.Get(other_endpoint);
    EXPECT_TRUE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneParameterizedChildWithOneController) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();
    const HTTP::Endpoint endpoint{"/:child", HTTP::MethodType::Get};
    node.Add(endpoint, controller);

    HTTP::Endpoint get_endpoint{"/best_child", HTTP::MethodType::Get};
    const auto& result = node.Get(get_endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_FALSE(result.RouteParameters.empty());
    EXPECT_EQ(result.RouteParameters.at(endpoint.Path.substr(2)), get_endpoint.Path.substr(1));

    get_endpoint = {"/", HTTP::MethodType::Get};
    const auto& other_result = node.Get(get_endpoint);
    EXPECT_FALSE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneParameterizedChildWithOne2ndLevelController) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();
    const HTTP::Endpoint endpoint{"/:child/action", HTTP::MethodType::Get};
    node.Add(endpoint, controller);

    HTTP::Endpoint get_endpoint{"/best_child/action", HTTP::MethodType::Get};
    const auto& result = node.Get(get_endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_FALSE(result.RouteParameters.empty());
    EXPECT_EQ(result.RouteParameters.at(endpoint.Path.substr(2, strlen("child"))),
            get_endpoint.Path.substr(1, strlen("best_child")));

    get_endpoint = {"/", HTTP::MethodType::Get};
    const auto& other_result = node.Get(get_endpoint);
    EXPECT_FALSE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());

    get_endpoint = {"/best_child", HTTP::MethodType::Get};
    const auto& yet_another_result = node.Get(get_endpoint);
    EXPECT_FALSE(yet_another_result.Controller);
    EXPECT_FALSE(yet_another_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddNestedParameterizedChildren) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();
    const HTTP::Endpoint endpoint{"/:child/:feature/action", HTTP::MethodType::Get};
    node.Add(endpoint, controller);

    HTTP::Endpoint get_endpoint{"/best_child/paint/action", HTTP::MethodType::Get};
    const auto& result = node.Get(get_endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_FALSE(result.RouteParameters.empty());
    EXPECT_EQ(result.RouteParameters.at(endpoint.Path.substr(2, strlen("child"))),
            get_endpoint.Path.substr(1, strlen("best_child")));
    EXPECT_EQ(result.RouteParameters.at(endpoint.Path.substr(9, strlen("feature"))),
            get_endpoint.Path.substr(12, strlen("paint")));

    get_endpoint = {"/", HTTP::MethodType::Get};
    const auto& other_result = node.Get(get_endpoint);
    EXPECT_FALSE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());

    get_endpoint = {"/best_child", HTTP::MethodType::Get};
    const auto& yet_another_result = node.Get(get_endpoint);
    EXPECT_FALSE(yet_another_result.Controller);
    EXPECT_FALSE(yet_another_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneWildcardRoute) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();
    const HTTP::Endpoint endpoint{"/*", HTTP::MethodType::Get};
    node.Add(endpoint, controller);

    std::vector<Server::Route::NodeLookupResponse> results;
    results.push_back(node.Get({"/some_words", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_FALSE(results.back().RouteParameters.empty());

    results.push_back(node.Get({"/@$#%^", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_FALSE(results.back().RouteParameters.empty());

    results.push_back(node.Get({"/some_words/more_words", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_FALSE(results.back().RouteParameters.empty());
    EXPECT_EQ(results.back().RouteParameters.at("*"), "some_words/more_words");
}

TEST(RouteNodeTest, AddWildcardRouteAndSomeNot) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();

    node.Add({"/some_words", HTTP::MethodType::Get}, controller);
    node.Add({"/*", HTTP::MethodType::Get}, controller);
    node.Add({"/@$#%^", HTTP::MethodType::Get}, controller);

    std::vector<Server::Route::NodeLookupResponse> results;

    results.push_back(node.Get({"/some_words", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_TRUE(results.back().RouteParameters.empty());

    results.push_back(node.Get({"/@$#%^", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_TRUE(results.back().RouteParameters.empty());

    results.push_back(node.Get({"/catchme_wildcard", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_FALSE(results.back().RouteParameters.empty());
    EXPECT_EQ(results.back().RouteParameters.at("*"), "catchme_wildcard");

    results.push_back(node.Get({"/catchme_wildcard/nested", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_FALSE(results.back().RouteParameters.empty());
    EXPECT_EQ(results.back().RouteParameters.at("*"), "catchme_wildcard/nested");

    results.push_back(node.Get({"/some_words/more_words", HTTP::MethodType::Get}));
    EXPECT_FALSE(results.back().Controller);
    EXPECT_TRUE(results.back().RouteParameters.empty());
}

TEST(RouteNodeTest, AddNestedWildcardRouteAndSomeNot) {
    Server::Route::Node node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Server::Controller::Null();

    node.Add({"/some_words", HTTP::MethodType::Get}, controller);
    node.Add({"/some_words/*", HTTP::MethodType::Get}, controller);
    node.Add({"/@$#%^", HTTP::MethodType::Get}, controller);

    std::vector<Server::Route::NodeLookupResponse> results;

    results.push_back(node.Get({"/some_words", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_TRUE(results.back().RouteParameters.empty());

    results.push_back(node.Get({"/@$#%^", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_TRUE(results.back().RouteParameters.empty());

    results.push_back(node.Get({"/doesnotexist", HTTP::MethodType::Get}));
    EXPECT_FALSE(results.back().Controller);
    EXPECT_TRUE(results.back().RouteParameters.empty());

    results.push_back(node.Get({"/some_words/catchme_wildcard", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_FALSE(results.back().RouteParameters.empty());
    EXPECT_EQ(results.back().RouteParameters.at("*"), "catchme_wildcard");

    results.push_back(node.Get({"/some_words/catchme_wildcard/even_more", HTTP::MethodType::Get}));
    EXPECT_TRUE(results.back().Controller);
    EXPECT_FALSE(results.back().RouteParameters.empty());
    EXPECT_EQ(results.back().RouteParameters.at("*"), "catchme_wildcard/even_more");
}

}
