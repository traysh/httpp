#include "listensocket_mocks.hpp"

namespace mockable {
Mock<decltype(::select)> select(::select);
Mock<decltype(::accept)> accept(::accept);
Mock<decltype(::listen)> listen(::listen);
Mock<decltype(::close)> close(::close);
Mock<int(int, int, int)> fcntl([](int a, int b, int c) {
    return ::fcntl(a, b, c);
});
}
