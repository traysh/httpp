#pragma once

#include <gtest/gtest.h>

#include <string>
#include <sstream>
#include <map>
#include <functional>

// TODO: include mocked funcion address as map key besides the test_case
// (i.e.: _expectations[make_pair(0x32323232, "test_name")]();

#include <iostream>

namespace Mockable {
template <class R, class... Args>
class Mock {
public:
    static void Register(std::function<R(Args...)> func) {
        _expectations.emplace(getTestKey(), func);
    }
    static R Execute(Args... args) {
        return _expectations[getTestKey()](std::forward<Args...>(args...));
    }

private:
    static std::map<std::string, std::function<R(Args...)>> _expectations;
    static inline std::string getTestKey() {
        const auto* test_info = ::testing::UnitTest::GetInstance()->current_test_info();
            
        std::stringstream ss;
        ss << std::string(test_info->test_case_name())
           << std::string("::") << std::string(test_info->name());

        return ss.str();
    }
};
template<class R, class... Args>
std::map<std::string, std::function<R(Args...)>> Mock<R, Args...>::_expectations;
}
