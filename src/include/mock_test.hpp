#pragma once

#include <gtest/gtest.h>

#include <string>
#include <sstream>
#include <map>
#include <functional>

namespace mockable {


template <typename F> class Mock;

// TODO support multiple registers
template <typename R, typename ...Args>
class Mock<R(Args...)> {
    typedef R (*original_t)(Args...);
public:
    Mock(original_t func) {
        _original = func;
    }

    void Register(original_t func) {
        _expectations.emplace(getTestKey(), func);
    }

    R operator()(Args... args) {
        const auto func = _expectations.find(getTestKey());
        if (func == _expectations.end()) {
            return _original(std::forward<Args>(args)...);
        }
        return _expectations[getTestKey()](std::forward<Args>(args)...);
    }

private:
    std::map<std::string, original_t> _expectations;
    original_t _original;

    static inline std::string getTestKey() {
        const auto* test_info = ::testing::UnitTest::GetInstance()->current_test_info();
            
        std::stringstream ss;
        ss << std::string(test_info->test_case_name())
           << std::string("::") << std::string(test_info->name());

        return ss.str();
    }
};

}

