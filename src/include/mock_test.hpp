#pragma once

#include <gtest/gtest.h>

#include <string>
#include <sstream>
#include <map>
#include <functional>

namespace mockable {

// TODO support multiple registers
template <typename F>
class Mock {
public:
    Mock(F func) {
        _original = func;
    }

    void Register(F func) {
        _expectations.emplace(getTestKey(), func);
    }

    template<typename ...Args>
    auto operator()(Args... args) noexcept(noexcept(_original(args...))) {
        const auto func = _expectations.find(getTestKey());
        if (func == _expectations.end()) {
            return _original(std::forward<Args>(args)...);
        }
        return _expectations[getTestKey()](std::forward<Args>(args)...);
    }

private:
    std::map<std::string, F*> _expectations;
    F* _original;

    static inline std::string getTestKey() {
        const auto* test_info = ::testing::UnitTest::GetInstance()->current_test_info();
            
        std::stringstream ss;
        ss << std::string(test_info->test_case_name())
           << std::string("::") << std::string(test_info->name());

        return ss.str();
    }
};

}

