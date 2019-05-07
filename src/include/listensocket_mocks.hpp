#pragma once
#include <sys/select.h>
#include <arpa/inet.h>
#include "mock.hpp"

#define MOCK(NAMESPACE, FUNC) \
    extern Mock<decltype(NAMESPACE :: FUNC)> FUNC;

namespace mockable {

//extern Mock<decltype(::select)> select;
//extern Mock<decltype(::accept)> accept;
MOCK(,select);
MOCK(,accept);
}


