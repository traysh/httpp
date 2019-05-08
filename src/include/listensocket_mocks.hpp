#pragma once
#include <sys/select.h>
#include <arpa/inet.h>
#include "mock.hpp"

#define MOCK(ORIGINAL, TARGET) \
    extern Mock<decltype(ORIGINAL)> TARGET;

namespace mockable {
MOCK(::select, select);
MOCK(::accept, accept);
MOCK(::listen, listen);
}


