#pragma once

#include <utility>
#include <cstdlib>

namespace mockable {
template <typename F>
class Mock {
public:
    Mock(F func) {
        _original = func;
    }

    template<typename ...Args>
    inline auto operator()(Args... args) noexcept(noexcept(_original(args...))) {
        return _original(std::forward<Args>(args)...);
    }

private:
    F* _original;
};
}
