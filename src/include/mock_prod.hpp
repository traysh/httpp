#pragma once

#include <utility>
#include <cstdlib>

namespace mockable {
template <typename F> class Mock;

template <typename R, typename ...Args>
class Mock<R(Args...)> {
    typedef R (*original_t)(Args...);
public:
    Mock(original_t func) {
        _original = func;
    }

    inline constexpr R operator()(Args... args) {
        return _original(std::forward<Args>(args)...);
    }

private:
    original_t _original;
};
}
