#include "listensocket_mocks.hpp"

namespace mockable {
Mock<decltype(::select)> select(::select);
Mock<decltype(::accept)> accept(::accept);
Mock<decltype(::listen)> listen(::listen);
}
